#pragma once

namespace v8{
	class _BPen : public _WrapHandle<base::gdi::Pen,_Handle>{
	public:
		bool _modifyed;
		void Initialize();
		_BPen();
		~_BPen();
	};
	class BPen : public BObject<base::gdi::Pen,_BPen,BPen>{
	protected:
		static Handle<Value> _arc(const Arguments& args);
        static Handle<Value> _polyTo(const Arguments& args);
		static Handle<Value> _moveTo(const Arguments& args);
		static Handle<Value> _lineTo(const Arguments& args);
		static Handle<Value> _drawRect(const Arguments& args);
		static Handle<Value> _roundRect(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BRgn : public _WrapHandle<base::gdi::Rgn,_Handle>{
	public:
		_BRgn();
		~_BRgn();
	};
	class BRgn : public BObject<base::gdi::Rgn,_BRgn,BRgn>{
	protected:
		static Handle<Value> _rect(const Arguments& args);
		static Handle<Value> _poly(const Arguments& args);
		static Handle<Value> _ellipse(const Arguments& args);
		static Handle<Value> _combine(const Arguments& args);
		static Handle<Value> _roundRect(const Arguments& args);
		static Handle<Value> _offset(const Arguments& args);
		static Handle<Value> _equal(const Arguments& args);
		static Handle<Value> _copy(const Arguments& args);
		static Handle<Value> _pointIn(const Arguments& args);
		static Handle<Value> _rectBox(const Arguments& args);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BBrush : public _WrapHandle<base::gdi::Rgn,_Handle>{
	public:
		bool _modifyed;
		void Initialize();
		_BBrush();
		~_BBrush();
	};
	class BBrush : public BObject<base::gdi::Brush,_BBrush,BBrush>{
	protected:
		static Handle<Value> _fillRgn(const Arguments& args);
		static Handle<Value> _fillRect(const Arguments& args);
		static Handle<Value> _ellipse(const Arguments& args);
		static Handle<Value> _poly(const Arguments& args);
		static Handle<Value> _roundRect(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BBitmap : public _WrapHandle<base::gdi::Bitmap,_Handle>{
	public:
		void Initialize();
		_BBitmap();
		~_BBitmap();
	};
	class BBitmap : public BObject<base::gdi::Bitmap,_BBitmap,BBitmap>{
	protected:
		static Handle<Value> _save(const Arguments& args);
		static Handle<Value> _load(const Arguments& args);
		static Handle<Value> _getSize(const Arguments& args);
		static Handle<Value> _setSize(const Arguments& args);
		static Handle<Value> _create(const Arguments& args);
		static Handle<Value> _fromBitmap(const Arguments& args);
		static Handle<Value> _clip(const Arguments& args);
		static Handle<Value> _fromIcon(const Arguments& args);
		static Handle<Value> _fromImage(const Arguments& args);
		static Handle<Value> _getPixelArray(const Arguments& args);
		static Handle<Value> _setPixelArray(const Arguments& args);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BFont : public _WrapHandle<base::gdi::Font,_Handle>{
	public:
		bool _modifyed;
		void Initialize();
		_BFont();
		~_BFont();
	};
	class BFont : public BObject<base::gdi::Font,_BFont,BFont>{
	protected:
		static Handle<Value> _setDefault(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class TextDraw : public base::_class{
	public:
		int color,align;
		TextDraw():color(0),align(0){}
	};
	class _BTextDraw : public _WrapHandle<TextDraw,_Handle>{
	public:
		bool _modifyed;
		void Initialize();
		_BTextDraw();
		~_BTextDraw();
	};
	class BTextDraw : public BObject<TextDraw,_BTextDraw,BTextDraw>{
	protected:
		static Handle<Value> _draw(const Arguments& args);
		static Handle<Value> _drawRect(const Arguments& args);
		static Handle<Value> _calcRect(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BImage : public _WrapHandle<base::gdi::DrawBitmap,_Handle>{
	public:
		bool _modifyed;
		void Initialize();
		_BImage();
		~_BImage();
	};
	class BImage : public BObject<base::gdi::DrawBitmap,_BImage,BImage>{
	protected:
		static Handle<Value> _save(const Arguments& args);
		static Handle<Value> _load(const Arguments& args);
		static Handle<Value> _getSize(const Arguments& args);
		static Handle<Value> _setSize(const Arguments& args);
		static Handle<Value> _create(const Arguments& args);
		static Handle<Value> _fromBitmap(const Arguments& args);
		static Handle<Value> _clip(const Arguments& args);
		static Handle<Value> _fromIcon(const Arguments& args);
		static Handle<Value> _fromImage(const Arguments& args);
		static Handle<Value> _getPixelArray(const Arguments& args);
		static Handle<Value> _setPixelArray(const Arguments& args);
		static Handle<Value> _draw(const Arguments& args);
		static Handle<Value> _alphaDraw(const Arguments& args);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BIcon : public _WrapHandle<base::gdi::Icon,_Handle>{
	public:
		bool _modifyed;
		void Initialize();
		_BIcon();
		~_BIcon();
	};
	class BIcon : public BObject<base::gdi::Icon,_BIcon,BIcon>{
	protected:
		static Handle<Value> _load(const Arguments& args);
		static Handle<Value> _draw(const Arguments& args);
		static Handle<Value> _size(const Arguments& args);
		static Handle<Value> _toBlackWhite(const Arguments& args);
		static Handle<Value> _fromIcon(const Arguments& args);
		static Handle<Value> _fromBitmap(const Arguments& args);
		static Handle<Value> _fromImage(const Arguments& args);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	void LoadGDIToJS(v8::Handle<Object>& glb);
}