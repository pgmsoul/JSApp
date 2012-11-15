#pragma once
namespace v8{
	typedef void (__stdcall * MSGCALL)();
	void AddMsg(MSGCALL call);
	Handle<Value> startMsgPump(const Arguments& args);

	class _BThread : public _WrapHandle<base::Thread,_Handle>{
	};
	class BThread : public BObject<base::Thread,_BThread,BThread>{
	protected:
		static void __stdcall _threadProc(void*);
		static Handle<Value> _start(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	void LoadKernelToJS(Handle<Object>& gObj);
}