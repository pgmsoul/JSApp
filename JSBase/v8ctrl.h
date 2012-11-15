#pragma once
namespace v8{
	class _BCtrl : public _WrapHandle<Ctrl,_Handle>{
	protected:
	public:
		//_BCtrl();
		//~_BCtrl();
		//void Initialize();
	};
	class BCtrl : public BObject<Ctrl,_BCtrl,BCtrl>{
	protected:
		static Handle<Value> _create(const Arguments& args);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BEdit : public _WrapHandle<Edit,_BCtrl>{
	protected:
	public:
		_BEdit();
		~_BEdit();
		//void Initialize();
	};
	class BEdit : public BObject<Edit,_BEdit,BEdit>{
	protected:
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	void LoadCtrlToJS(Handle<Object>& gObj);
}