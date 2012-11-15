#pragma once
namespace v8{
	/* ��װһ��C++���ܵ�JS����, ʹ����3����: 1��C++�Ĺ�����, 2�ǰ�װ����_Wrap��,
	3 ���������ص�JS�����ļ�����, ���̳���BObject����. ֮����ʹ��һ����װ��, ��
	��Ϊ, BObject�಻�ܼ̳�, JS����ÿnewһ������, ������һ��BObject��, ��������
	����(�������ܵ�BObject��), �ǲ������ص�. ���Ե�����ʹ��_Wrap��ļ̳���. Ҫ��
	_Wrap������ǵ��̳�, �Ҳ������麯��, ����ָ��ǿ��ת���������.
	*/
	template<typename CPP,typename PARENT>class _WrapHandle;
	typedef void(*V8LOAD)(v8::Handle<ObjectTemplate>&,v8::Handle<ObjectTemplate>&,base::String&,v8::Handle<FunctionTemplate>&);
	//typedef v8::Handle<Value> (*V8CREATE)(const Arguments&);
	//_BWARP������_WrapHandle�̳�. CPP��C++����, B�Ǽ̳д�ģ����Ķ���, ������.
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
		//JS�������new��ʱ�򣬺��еĺ���
		static v8::Handle<Value> _jsCreate(const Arguments& args){
			Local<Object> self = args.Holder();
			B* co = new B();
			co->_WrapObj = new _BWRAP();
			co->_WrapObj->CPPObj = new CPP;
			co->_WrapObj->PV8Obj = &co->V8Obj;
			co->Wrap(self);
			co->_WrapObj->Initialize();//��ʼ������
			_ASSERT_EXPR(self->GetPointerFromInternalField(0)!=0,L"BObject._jsCreate: Extern Pointer is NULL");
			return self;
		}
		//���ص�JS�����С�
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
			//_ASSERT_EXPR(V8Obj.IsNearDeath(),L"BObject::Unwrap");//V8Obj����Ƿ�ֻ��һ������(��ǰ����), ������, ����ǲ���Ҫ��, ��Ϊ��ǿ��ɾ��.
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
		//��CPP����������һ������ʵ����
		inline static Local<Object> New(){
			return FuncT->InstanceTemplate()->NewInstance()->ToObject();
		}
		inline static B* GetPointer(v8::Handle<Object>& vObj){
			return (B*)vObj->GetPointerFromInternalField(0);
		}
	};
	//CPPObj������һ��HandleType������, ��Ϊֻ������, ���ܵ�������Handle()����.
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
	//�ֲ������Զ������, T������Bind�������󶨺ͽ��.
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