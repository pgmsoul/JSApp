#pragma once

#define SETFUNCION(name,func) ot_func->Set(String::New((uint16_t*)name),FunctionTemplate::New(func),ReadOnly)
#define SETCLAFUNC(name,Cla) ot_func->Set(String::New(#name),FunctionTemplate::New(&Cla::_##name),ReadOnly)
#define SETOBJCLAFUNC(obj,name,Cla) obj->Set(String::New(#name),FunctionTemplate::New(&Cla::_##name)->GetFunction(),ReadOnly)
//ReadOnly属性自动屏蔽set调用。
#define SETREADONLYACCESSOR(name,Cla) ot_inst->SetAccessor(String::New((uint16_t*)name),&Cla::_get,&Cla::_set,Handle<Value>(),DEFAULT,ReadOnly)
#define SETACCESSOR(name,Cla) ot_inst->SetAccessor(String::New((uint16_t*)name),&Cla::_get,&Cla::_set)
#define SETGETOR(name,Cla) ot_inst->SetAccessor(String::New((uint16_t*)name),&Cla::_get,0,Handle<Value>(),DEFAULT,ReadOnly)
#define SETOBJCONST(obj,name,PRE) obj->Set(String::New(#name),Uint32::New(PRE##_##name),ReadOnly)
#define SETCONST(name,PRE) objcreate->Set(String::New(#name),Uint32::New(PRE##_##name),ReadOnly)
#define SETCONST2(name,val) objcreate->Set(String::New(#name),Uint32::New(val),ReadOnly)
#define Bool(b) b?True():False()
#define GETARGSINT(i) args.Length()>i?args[i]->Int32Value():0
#define GETARGSVALUE(i,def,T) (args.Length()>i?args[i]->T##Value():def)
namespace v8{
	//因为这个库可以供JSApp用, 也可以供其它DLL用, 而这是一个静态库. 静态库在不同的
	//工程中实际上都有自己的一个拷贝, 所以DLL使用这个变量应该在Initialize中使用传入
	//值初始化它, 这个变量在JSApp中会被初始化.
	//这个变量是一个线程变量, 每个线程都有自己的独立实例.
	extern __declspec(thread) Persistent<Context>* g_c;
	//设置一个只读属性为JS函数, func 是Js函数体代码, 不包括function(){这里的代码};
	void SetJsFunc(Handle<ObjectTemplate> newObjT,LPCWSTR name,LPCWSTR func);
	inline void SetGlobalFunc(Handle<Object>& glb,LPCWSTR name,InvocationCallback func){
		glb->Set(String::New((uint16_t*)name),FunctionTemplate::New(func)->GetFunction(),ReadOnly);
	}
	inline void SetGlobalFunc(Handle<ObjectTemplate>& newObjT,LPCWSTR name,InvocationCallback func){
		newObjT->Set(String::New((uint16_t*)name),FunctionTemplate::New(func),ReadOnly);
	}
	//检测返回值类型, 比如 ->IsFunction , 此函数在类型不对时仍然会返回一个非空值, 只要指定名称的对象存在.
	template<typename T> Local<T> GetJSVariant(Local<Object>& gObj,LPCWSTR name){
		HandleScope store;
		Local<Value> vObj = gObj->Get(String::New((uint16_t*)name));
		Local<T> obj;
		if(vObj.IsEmpty()) return store.Close(obj);
		obj = Local<T>::Cast(vObj);
		return store.Close(obj);
	}
	template<typename T> Local<T> AddJSVariant(Local<Object>& glb,LPCWSTR name){
		HandleScope store;
		Local<T> obj = T::New();
		glb->Set(String::New((uint16_t*)name),obj);
		return store.Close(obj);
	}
	template<typename T> inline void SetJSVariantProperty(Handle<Object>& obj,LPCWSTR name,Handle<T> v){
		obj->Set(String::New((uint16_t*)name),v);
	}
	inline bool StringIs(Handle<Value> val,LPCWSTR str){
		v8::String::Value sv(val);
		return base::WcsEqual((LPCWSTR)*sv,str);
	}
	inline void GetString(Handle<Value> val,base::String& str){
		v8::String::Value sv(val);
		str = (LPCWSTR)*sv;
	}
	inline void GetPropertyString(Handle<Value> val,LPCWSTR prop,base::String& str){
		GetString(val->ToObject()->Get(String::New((uint16_t*)prop)),str);
	}
	inline int GetPropertyInt(Handle<Value> val,LPCWSTR prop){
		return val->ToObject()->Get(String::New((uint16_t*)prop))->Int32Value();
	}
	template<typename T> inline T GetArgs(Handle<Value> val){
		return (T)(INT_PTR)val->Int32Value();
	}
	template<typename T> bool ObjIsType(Handle<Value> v){
		return v->ToObject()->GetConstructor()==T::GetTemplate()->GetFunction();

	}
	int ReportError(TryCatch& err);
	enum MSG_TYPE{
		MT_ERROR = MB_ICONERROR,
		MT_WARN = MB_ICONWARNING,
		MT_INFO = MB_ICONINFORMATION
	};
	Local<Object> getGlobal();
	void releaseContext();
	void initContext();
	int InnerMsg(Handle<Value> msg,MSG_TYPE mt = MT_INFO);
	int InnerMsg(LPCWSTR msg,MSG_TYPE mt);
	//运行一段Js代码, 并且返回结果.
	v8::Handle<Value> runJSCode(LPCWSTR js,LPCWSTR name = 0);
	//加载Js文件, 并且返回结果.
	v8::Handle<Value> runJSFile(LPCWSTR file,LPCWSTR code = 0);
	//运行以UTF-8编码保存的JS代码文本资源.
	Handle<Value> runJSRes(base::ResID jr,LPCWSTR name);
	//读取一个js文件,cp是文件文本编码.
	bool loadFile(base::String& js,LPCWSTR file,LPCWSTR cp = 0);
	//加载一个DLL
	Handle<Value> loadJSDLL(const Arguments& args);
}