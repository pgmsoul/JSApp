#pragma once
namespace v8{
	/* 包装一个C++功能到JS环境, 使用了3个类: 1是C++的功能类, 2是包装它的_Wrap类,
	3 是用来加载到JS环境的加载类, 即继承自BObject的类. 之所以使用一个包装类, 是
	因为, BObject类不能继承, JS代码每new一个对象, 就生成一个BObject类, 但是它的
	父类(其它功能的BObject类), 是不被加载的. 所以单独再使用_Wrap类的继承链. 要求
	_Wrap类必须是单继承, 且不能有虚函数, 否则指针强制转换会出问题.
	*/
	template<typename CPP,typename PARENT>class _WrapHandle;
	typedef void(*V8LOAD)(v8::Handle<ObjectTemplate>&,v8::Handle<ObjectTemplate>&,base::String&,v8::Handle<FunctionTemplate>&);
	//typedef v8::Handle<Value> (*V8CREATE)(const Arguments&);
	//_BWARP必须自_WrapHandle继承. CPP是C++对象, B是继承此模版类的对象, 即自身.
	template<typename CPP,typename _BWRAP,typename B> class BObject : public base::_class{
	protected:
		inline void MakeWeak (void) {
			V8Obj.MakeWeak(this, WeakCallback);
			V8Obj.MarkIndependent();
		}
		static void WeakCallback (Persistent<Value> value, void *data) {
			B* obj = static_cast<B*>(data);
			delete obj;
		}
		//JS代码调用new的时候，呼叫的函数
		static v8::Handle<Value> _jsCreate(const Arguments& args){
			Local<Object> self = args.Holder();
			B* co = new B();
			co->_WrapObj = new _BWRAP();
			co->_WrapObj->CPPObj = new CPP;
			co->_WrapObj->PV8Obj = &co->V8Obj;
			co->Wrap(self);
			co->_WrapObj->Initialize();//初始化对象
			_ASSERT_EXPR(self->GetPointerFromInternalField(0)!=0,L"BObject._jsCreate: Extern Pointer is NULL");
			return self;
		}
		//加载到JS环境中。
		static void _loadToJS(v8::Handle<Object>& glb,V8LOAD lf){
			HandleScope store;

			v8::Handle<FunctionTemplate> objcreate = FunctionTemplate::New(_jsCreate);
			GetTemplate(&objcreate);
			v8::Handle<ObjectTemplate> ot_func = objcreate->PrototypeTemplate();
			v8::Handle<ObjectTemplate> ot_inst = objcreate->InstanceTemplate();  
			ot_inst->SetInternalFieldCount(1);
			base::String objname;
			lf(ot_func,ot_inst,objname,objcreate);
			ot_func->Set(String::New("dispose"),FunctionTemplate::New(&BObject::_dispose<BObject>),ReadOnly);
			base::String fstr;
			fstr.Format(L"return '%s'",objname);
			SetJsFunc(ot_inst,L"toString",fstr);
			SETFUNC(handle,_BWRAP);
			glb->Set(String::New((uint16_t*)objname.Handle()),objcreate->GetFunction());
		}
		inline void Wrap (Handle<Object> handle) {
			V8Obj = Persistent<Object>::New(handle);
			_ASSERT_EXPR(V8Obj->InternalFieldCount()!=0,L"Wrap: InternalFieldCount is 0 ");
			V8Obj->SetPointerInInternalField(0,this);
			MakeWeak();
		}
		void Unwrap(){
			if (V8Obj.IsEmpty()) return;
			//_ASSERT_EXPR(V8Obj.IsNearDeath(),L"BObject::Unwrap");//V8Obj检测是否只有一个引用(当前引用), 在这里, 检测是不必要的, 因为是强制删除.
			V8Obj.ClearWeak();
			V8Obj->SetInternalField(0, v8::Undefined());
			V8Obj.Dispose();
			V8Obj.Clear();
			if(_WrapObj) delete _WrapObj;
			_WrapObj = 0;
		}
	public:
		Persistent<Object>	V8Obj;
		_BWRAP*				_WrapObj;

		BObject():_WrapObj(0){}
		~BObject(){
			if (V8Obj.IsEmpty()) return;
			V8Obj.ClearWeak();
			V8Obj->SetInternalField(0, Undefined());
			V8Obj.Dispose();
			V8Obj.Clear();
			if(_WrapObj) delete _WrapObj;
		}
		template<typename CLA>static Handle<Value> _dispose(const Arguments& args){
			HandleScope store;
			CLA* bObj = (CLA*)args.Holder()->GetPointerFromInternalField(0);
			if(bObj==NULL) return False();
			bObj->Unwrap();
			delete (CLA*)bObj;
			return True();
		}
		static Persistent<FunctionTemplate>& GetTemplate(v8::Handle<FunctionTemplate>* ft = 0){
			static Persistent<FunctionTemplate> FuncT;
			if(FuncT.IsEmpty()) FuncT = Persistent<FunctionTemplate>::New(*ft);
			return FuncT;
		}
		//在CPP代码中生成一个对象实例。
		inline static Local<Object> New(){
			return FuncT->InstanceTemplate()->NewInstance()->ToObject();
		}
		inline static B* GetPointer(v8::Handle<Object>& vObj){
			return (B*)vObj->GetPointerFromInternalField(0);
		}
	};
	//CPPObj必须是一个HandleType的子类, 因为只有这样, 才能调用它的Handle()函数.
	class _Handle : public base::HandleType<void*>{
	public:
		Persistent<Object>*			PV8Obj;
		void* CPPObj;
		_Handle():CPPObj(0),PV8Obj(0){}
		void Initialize(){}
	};
	template<typename CPP,typename PARENT>class _WrapHandle : public PARENT{
	public:
		static v8::Handle<Value> _handle(const Arguments& args){
			HandleScope store;
			Local<Object> self = args.Holder();
			BObject<int,_WrapHandle,_Handle>* obj = (BObject<int,_WrapHandle,_Handle>*)self->GetPointerFromInternalField(0);
			_ASSERT_EXPR(obj!=NULL,L"BWndPtr._offset");
			if(obj==NULL) return store.Close(Uint32::New(0));
			_Handle* handle = (_Handle*)obj->_WrapObj->CPPObj;
			if(handle==NULL) return store.Close(Uint32::New(0));
			return store.Close(Uint32::New((UINT_PTR)handle->Handle()));
		}
		~_WrapHandle(){
			if(!CPPObj) return;
			delete (CPP*)CPPObj;
			CPPObj = NULL;
		}
	};
	//局部变量自动解绑功能, T必须有Bind函数来绑定和解绑.
	template<typename T,typename R> class LocalBWrap : public base::_class{
		T* _handle;
	public:
		LocalBWrap():_handle(0){
		}
		LocalBWrap(T* bObj,R* obj){
			_handle = bObj;
			_handle->Bind(obj);
		}
		void Bind(T* bObj,R* obj){
			_handle = bObj;
			_handle->Bind(obj);
		}
		~LocalBWrap(){
			if(_handle) _handle->Bind(0);
		}
	};
}