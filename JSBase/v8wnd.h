#pragma once
namespace v8{
	class _BWndPtr : public _WrapHandle<WndPtr,_Handle>{
	};
	class BWndPtr : public BObject<WndPtr,_BWndPtr,BWndPtr>{
	protected:
		static Handle<Value> _offset(const Arguments& args);
		static Handle<Value> _client(const Arguments& args);
		static Handle<Value> _close(const Arguments& args);
		static Handle<Value> _invalidate(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BView : public _WrapHandle<ScrollWnd,_BWndPtr>{
	public:
		Persistent<Object>			_param;
		Persistent<Object>			_cd;
		Persistent<Object>			_mouse;

		Persistent<Function>		BOnCreate;	//因为这个事件会被频繁的使用,所以集成它.
		Persistent<Function>		BOnCommand;	//因为这个事件会被频繁的使用,所以集成它.
		Persistent<Function>		BOnMouse;	//MouseEvent处理所有鼠标消息.		
		Persistent<Function>		BOnClose;	//截获WM_CLOSE消息，bool值默认为false，如果置为true，则窗口关闭动作取消。
		Persistent<Function>		BOnDraw;
		Persistent<Function>		BOnSize;

		void onCommand(WORD id);
		void onCreate();
		void onMouse(Mouse* m);
		void onClose(bool& cancel);
		void onDraw(base::gdi::DC* dc);
		void onSize(base::Twin16 cs);

		void initialize();
		void Initialize();
		~_BView();
	};
	class BView : public BObject<ScrollWnd,_BView,BView>{
	protected:
		static Handle<Value> _create(const Arguments& args);
		static Handle<Value> _scr(const Arguments& args);
		static Handle<Value> _logPos(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BFrame : public _WrapHandle<Frame,_BView>{
	public:
		void Initialize();
	};
	class BFrame : public BObject<Frame,_BFrame,BFrame>{
	protected:
		static Handle<Value> _create(const Arguments& args);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	void LoadWndToJS(Handle<Object>& gObj);
}