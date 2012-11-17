#pragma once

#define SETFUNCION(name,func) ot_func->Set(String::New((uint16_t*)name),FunctionTemplate::New(func),ReadOnly)
#define SETCLAFUNC(name,Cla) ot_func->Set(String::New(#name),FunctionTemplate::New(&Cla::_##name),ReadOnly)
#define SETOBJCLAFUNC(obj,name,Cla) obj->Set(String::New(#name),FunctionTemplate::New(&Cla::_##name)->GetFunction(),ReadOnly)
//ReadOnly�����Զ�����set���á�
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
	//��Ϊ�������Թ�JSApp��, Ҳ���Թ�����DLL��, ������һ����̬��. ��̬���ڲ�ͬ��
	//������ʵ���϶����Լ���һ������, ����DLLʹ���������Ӧ����Initialize��ʹ�ô���
	//ֵ��ʼ����, ���������JSApp�лᱻ��ʼ��.
	//���������һ���̱߳���, ÿ���̶߳����Լ��Ķ���ʵ��.
	extern __declspec(thread) Persistent<Context>* g_c;
	//����һ��ֻ������ΪJS����, func ��Js���������, ������function(){����Ĵ���};
	void SetJsFunc(Handle<ObjectTemplate> newObjT,LPCWSTR name,LPCWSTR func);
	inline void SetGlobalFunc(Handle<Object>& glb,LPCWSTR name,InvocationCallback func){
		glb->Set(String::New((uint16_t*)name),FunctionTemplate::New(func)->GetFunction(),ReadOnly);
	}
	inline void SetGlobalFunc(Handle<ObjectTemplate>& newObjT,LPCWSTR name,InvocationCallback func){
		newObjT->Set(String::New((uint16_t*)name),FunctionTemplate::New(func),ReadOnly);
	}
	//��ⷵ��ֵ����, ���� ->IsFunction , �˺��������Ͳ���ʱ��Ȼ�᷵��һ���ǿ�ֵ, ֻҪָ�����ƵĶ������.
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
	//����һ��Js����, ���ҷ��ؽ��.
	v8::Handle<Value> runJSCode(LPCWSTR js,LPCWSTR name = 0);
	//����Js�ļ�, ���ҷ��ؽ��.
	v8::Handle<Value> runJSFile(LPCWSTR file,LPCWSTR code = 0);
	//������UTF-8���뱣���JS�����ı���Դ.
	Handle<Value> runJSRes(base::ResID jr,LPCWSTR name);
	//��ȡһ��js�ļ�,cp���ļ��ı�����.
	bool loadFile(base::String& js,LPCWSTR file,LPCWSTR cp = 0);
	//����һ��DLL
	Handle<Value> loadJSDLL(const Arguments& args);
}