#include "stdafx.h"
#include "innerv8.h"

#define GET_INNER_P(BCla,bObj) BCla* inner = (BCla*)bObj->GetPointerFromInternalField(0) 

namespace v8{
#define SET_M_EV(ev) obj->Set(String::New(#ev),Int32::New(WM_##ev),ReadOnly)
	void LoadBMouse(){
		HandleScope store;
		Local<Object> obj = GetJSVariant<Function>(getGlobal(),L"BMouse");
		if(obj.IsEmpty()||obj->IsUndefined()) return;
		SET_M_EV(MOUSEMOVE);
		SET_M_EV(LBUTTONDOWN);
		SET_M_EV(LBUTTONUP);
		SET_M_EV(LBUTTONDBLCLK);
		SET_M_EV(RBUTTONDOWN);
		SET_M_EV(RBUTTONUP);
		SET_M_EV(RBUTTONDBLCLK);
		SET_M_EV(MBUTTONDOWN);
		SET_M_EV(MBUTTONUP);
		SET_M_EV(MBUTTONDBLCLK);
		SET_M_EV(MOUSEWHEEL);
		SET_M_EV(XBUTTONDOWN);
		SET_M_EV(XBUTTONUP);
		SET_M_EV(XBUTTONDBLCLK);
		SET_M_EV(MOUSEHWHEEL);
	}
	bool BMouseTo(v8::Handle<Value> value,Mouse* m){
		return false;
	}
	void BMouseFrom(v8::Handle<Value> val,Mouse* m){
		HandleScope store;
		if(!val->IsObject()) return;
		Local<Object> value = val->ToObject();
		value->Set(String::New("button"),Int32::New(m->Button));
		value->Set(String::New("event"),Int32::New(m->Event));
		value->Set(String::New("wheel"),Int32::New(m->WheelDelta/120));
		value->Set(String::New("x"),Int32::New(m->X));
		value->Set(String::New("y"),Int32::New(m->Y));
	}
	bool BViewParamTo(v8::Handle<Value> value,CreateStruct* cs){
		HandleScope store;
		if(!value->IsObject()) return false;
		Local<Object> vObj = value->ToObject();
		Local<Value> v1;
		v1 = vObj->Get(String::New("text"));
		if(!v1->IsUndefined()){
			v8::String::Value s(v1);
			cs->Title = (LPCWSTR)*s;
		}
		v1 = vObj->Get(String::New("left"));
		if(!v1->IsUndefined())
			cs->Location.x = v1->Int32Value();
		v1 = vObj->Get(String::New("top"));
		if(!v1->IsUndefined())
			cs->Location.y = v1->Int32Value();
		v1 = vObj->Get(String::New("width"));
		if(!v1->IsUndefined())
			cs->Size.y = v1->Int32Value();
		v1 = vObj->Get(String::New("height"));
		if(!v1->IsUndefined())
			cs->Size.y = v1->Int32Value();
		v1 = vObj->Get(String::New("id"));
		if(!v1->IsUndefined())
			cs->Identity = v1->Int32Value();
		v1 = vObj->Get(String::New("classStyle"));
		if(!v1->IsUndefined()){
			if(v1->IsNumber()||v1->Uint32Value()!=0)
				cs->ClassStyle = v1->Int32Value();
		}
		v1 = vObj->Get(String::New("style"));
		if(!v1->IsUndefined()){
			if(v1->IsNumber()||v1->Uint32Value()!=0)
				cs->Style = v1->Int32Value();
		}
		v1 = vObj->Get(String::New("exStyle"));
		if(!v1->IsUndefined()){
			if(v1->IsNumber()||v1->Uint32Value()!=0)
				cs->ExStyle = v1->Int32Value();
		}
		return true;
	}
	void BViewParamFrom(v8::Handle<Value> val,CreateStruct* cs){
		HandleScope store;
		if(!val->IsObject()) return;
		Local<Object> value = val->ToObject();
		value->Set(String::New("text"),String::New((uint16_t*)cs->Title.Handle()));
		value->Set(String::New("left"),Int32::New(cs->Location.x));
		value->Set(String::New("width"),Int32::New(cs->Size.x));
		value->Set(String::New("top"),Int32::New(cs->Location.y));
		value->Set(String::New("height"),Int32::New(cs->Size.y));
		value->Set(String::New("id"),Int32::New(cs->Identity));
		value->Set(String::New("classStyle"),Int32::New(cs->ClassStyle));
		value->Set(String::New("style"),Int32::New(cs->Style));
		value->Set(String::New("exStyle"),Int32::New(cs->ExStyle));
	}

	//BWndPtr
	Handle<Value> BWndPtr::_offset(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BWndPtr* obj = (BWndPtr*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BWndPtr._offset");
		WndPtr* handle = (WndPtr*)obj->_WrapObj->CPPObj;
		if(handle==NULL) return False();
		int d[4];
		for(int i=0;i<4;i++){
			if(args.Length()>i) d[i] = args[i]->Int32Value();
			else d[i] = 0;
		}
		handle->Offset(d[0],d[1],d[2],d[3]);
		return Bool(true);
	}
	Handle<Value> BWndPtr::_client(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BWndPtr* obj = (BWndPtr*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BWndPtr._client");
		base::Twin16 cs(0,0);
		WndPtr* handle = (WndPtr*)obj->_WrapObj->CPPObj;
		if(handle==NULL) handle->GetClientSize(cs);
		Handle<Object> vRt = Handle<Object>::Cast(runJSCode(L"new BTwin();"));
		base::Twin tw(cs.x,cs.y);
		BTwinFrom(vRt,&tw);
		return store.Close(vRt);
	}
	Handle<Value> BWndPtr::_close(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BWndPtr* obj = (BWndPtr*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BWndPtr._client");
		WndPtr* handle = (WndPtr*)obj->_WrapObj->CPPObj;
		if(handle==NULL) return False();
		return Bool(handle->Close());
	}
	Handle<Value> BWndPtr::_invalidate(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BWndPtr* obj = (BWndPtr*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BWndPtr._offset");
		WndPtr* handle = (WndPtr*)obj->_WrapObj->CPPObj;
		return Bool(handle->Invalidate());
	}
	Handle<Value> BWndPtr::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		BWndPtr* obj = (BWndPtr*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BWndPtr._set");
		WndPtr* handle = (WndPtr*)obj->_WrapObj->CPPObj;
		if(handle==0) return Undefined();
		v8::String::Value pty(property);
		if(base::WcsEqual((LPCWSTR)*pty,L"text")){
			base::String title;
			handle->GetText(title);
			return String::New((uint16_t*)title.Handle());
		}else if(base::WcsEqual((LPCWSTR)*pty,L"rect")){
			base::Rect r(0,0,0,0);
			handle->GetRect(r);
			Handle<Object> vRt = Handle<Object>::Cast(runJSCode(L"new BRect();"));
			BRectFrom(vRt,&r);
			return store.Close(vRt);
		}else if(base::WcsEqual((LPCWSTR)*pty,L"location")){
			base::Rect r(0,0,0,0);
			handle->GetRect(r);
			Handle<Object> vRt = Handle<Object>::Cast(runJSCode(L"new BTwin();"));
			base::Twin tw(r.left,r.top);
			BTwinFrom(vRt,&tw);
			return store.Close(vRt);
		}else if(base::WcsEqual((LPCWSTR)*pty,L"size")){
			base::Rect r(0,0,0,0);
			handle->GetRect(r);
			Handle<Object> vRt = Handle<Object>::Cast(runJSCode(L"new BTwin();"));
			base::Twin tw(r.Width(),r.Height());
			BTwinFrom(vRt,&tw);
			return store.Close(vRt);
		}
		return Undefined();
	}
	void BWndPtr::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		BView* obj = (BView*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BView._set");
		WndPtr* handle = (WndPtr*)obj->_WrapObj->CPPObj;
		if(handle==0) return;
		v8::String::Value pty(property);
		if(base::WcsEqual((LPCWSTR)*pty,L"text")){
			String::Value str(value);
			handle->SetText((LPCWSTR)*str);
		}else if(base::WcsEqual((LPCWSTR)*pty,L"rect")){
			base::Rect rect;
			if(!BRectTo(value,&rect)) return;
			handle->SetDimension(rect.left,rect.top,rect.Width(),rect.Height());
		}else if(base::WcsEqual((LPCWSTR)*pty,L"location")){
			base::Twin tw;
			if(!BTwinTo(value,&tw)) return;
			handle->SetLocation(tw.x,tw.y);
		}else if(base::WcsEqual((LPCWSTR)*pty,L"size")){
			base::Twin tw;
			if(!BTwinTo(value,&tw)) return;
			handle->SetSize(tw.x,tw.y);
		}
	}
	void BWndPtr::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BWndPtr";
		SETACCESSOR(L"text",BWndPtr);
		SETACCESSOR(L"rect",BWndPtr);
		SETACCESSOR(L"location",BWndPtr);
		SETACCESSOR(L"size",BWndPtr);
		SETFUNC(invalidate,BWndPtr);
		SETFUNC(close,BWndPtr);
		SETFUNC(client,BWndPtr);
		SETFUNC(offset,BWndPtr);
	}
	//BView
	_BView::~_BView(){
		_mouse.Dispose();
		_param.Dispose();
		BOnClose.Dispose();
		BOnCreate.Dispose();
		BOnDraw.Dispose();
		BOnMouse.Dispose();
		BOnCommand.Dispose();
	}
	void _BView::initialize(){
		HandleScope store;
		ScrollWnd* sw = (ScrollWnd*)CPPObj;
		_ASSERT_EXPR(sw!=0,L"_BView::initialize Handle is NULL");
		if(sw==NULL) return;
		v8::Handle<Object> vParam = runJSCode(L"new BViewParam();")->ToObject();
		_param = Persistent<Object>::New(vParam);
		BViewParamFrom(vParam,sw->Param.Handle());
		_mouse = Persistent<Object>::New(runJSCode(L"new BMouse();")->ToObject());
		sw->OnClose.Bind(this,&_BView::onClose);
		sw->OnCreate.Add(this,&_BView::onCreate);
		sw->OnMouse.Add(this,&_BView::onMouse);
		sw->OnDraw.Add(this,&_BView::onDraw);
		sw->OnCommand.Add(this,&_BView::onCommand);
		sw->OnSize.Add(this,&_BView::onSize);
	}
	void _BView::Initialize(){
		initialize();
	}

	void _BView::onCreate(){
		HandleScope store;
		if(BOnCreate.IsEmpty()||!BOnCreate->IsFunction()) return;
		v8::Handle<Value> args[1];
		BOnCreate->Call(*PV8Obj,0,args);
	}
	void _BView::onMouse(Mouse* m){
		HandleScope store;
		if(BOnMouse.IsEmpty()||!BOnMouse->IsFunction()) return;
		v8::Handle<Value> args[1];
		args[0] = _mouse;
		BMouseFrom(args[0],m);
		BOnMouse->Call(*PV8Obj,1,args);
	}
	void _BView::onClose(bool& cancel){
		HandleScope store;
		if(BOnClose.IsEmpty()||!BOnClose->IsFunction()) return;
		v8::Handle<Value> args[1];
		cancel = BOnClose->Call(*PV8Obj,0,args)->BooleanValue();
	}
	void _BView::onCommand(WORD id){
		HandleScope store;
		if(BOnCommand.IsEmpty()||!BOnCommand->IsFunction()) return;
		v8::Handle<Value> args[1];
		args[0] = Int32::New(id);
		BOnCommand->Call(*PV8Obj,1,args);
	}
	void _BView::onDraw(base::gdi::DC* dc){
		HandleScope store;
		if(BOnDraw.IsEmpty()||!BOnDraw->IsFunction()) return;
		v8::Handle<Value> args[1];
		args[0] = Uint32::New((INT_PTR)dc->Handle());
		BOnDraw->Call(*PV8Obj,1,args);
	}
	void _BView::onSize(base::Twin16 cs){
		HandleScope store;
		if(BOnSize.IsEmpty()||!BOnSize->IsFunction()) return;
		v8::Handle<Value> args[2];
		args[0] = Int32::New(cs.x);
		args[1] = Int32::New(cs.y);
		BOnSize->Call(*PV8Obj,2,args);
	}
	ScrollWnd* getView(Handle<Value> obj){
		HandleScope store;
		BView* bObj = (BView*)obj->ToObject()->GetPointerFromInternalField(0);
		_ASSERT_EXPR(bObj!=NULL,L"BView._create");
		ScrollWnd* cpp = (ScrollWnd*)bObj->_WrapObj->CPPObj;
		return cpp;
	}
	Handle<Value> BView::_create(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();

		BView* obj = (BView*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BView._create");
		ScrollWnd* handle = (ScrollWnd*)obj->_WrapObj->CPPObj;
		if(handle==NULL) return False();

		HWND parent;
		if(args.Length()>0){
			parent = (HWND)(INT_PTR)args[0]->Int32Value();
		}else parent = 0;

		Handle<Object> vParam = obj->_WrapObj->_param;
		BViewParamTo(vParam,handle->Param.Handle());
		if(!handle->Create(parent)){
			base::SystemMessageBox(0,L"Create BScrollWnd",MT_ERROR);
			return False();
		}

		return True();  
	}
	Handle<Value> BView::_scr(const Arguments& args){
		HandleScope store;
		ScrollWnd* cpp = getView(args.Holder());
		base::String str;
		base::Twin scr = cpp->Scr();
		str.Format(L"new BTwin(%d,%d)",scr.x,scr.y);
		return store.Close(runJSCode(str));
	}
	Handle<Value> BView::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		BView* obj = (BView*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BView._set");
		ScrollWnd* handle = (ScrollWnd*)obj->_WrapObj->CPPObj;
		if(handle==0) return Undefined();
		v8::String::Value pty(property);
		if(base::WcsEqual((LPCWSTR)*pty,L"minSize")){
			base::Twin tw = handle->MinSize();
			Handle<Object> vRt = Handle<Object>::Cast(runJSCode(L"new BTwin();"));
			BTwinFrom(vRt,&tw);
			return store.Close(vRt);
		}else if(base::WcsEqual((LPCWSTR)*pty,L"param")){
			Handle<Object> vRt = obj->_WrapObj->_param;
			return store.Close(vRt);
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onMouse")){
			return obj->_WrapObj->BOnMouse;
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onCreate")){
			return obj->_WrapObj->BOnCreate;
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onClose")){
			return obj->_WrapObj->BOnClose;
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onCommand")){
			return obj->_WrapObj->BOnCommand;
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onDraw")){
			return obj->_WrapObj->BOnDraw;
		}
		return Undefined();
	}
	Handle<Value> BView::_logPos(const Arguments& args){
		HandleScope store;
		if(args.Length()<0) return False();
		ScrollWnd* sw = getView(args.Holder());
		base::String id;
		GetString(args[0],id);
		if(id.Length()==0) return False();
		WPRRegisterWnd(sw,id);
		return True();
	}
	void BView::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		BView* obj = (BView*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BView._set");
		ScrollWnd* handle = (ScrollWnd*)obj->_WrapObj->CPPObj;
		if(handle==0) return;
		v8::String::Value pty(property);
		if(base::WcsEqual((LPCWSTR)*pty,L"minSize")){
			base::Twin tw;
			if(!BTwinTo(value,&tw)) return;
			handle->SetMinSize(tw.x,tw.y);
		}else if(base::WcsEqual((LPCWSTR)*pty,L"param")){
			//Local<Object> vObj = value->ToObject();
			//InnerMsg(vObj->GetConstructor());
			//obj->_WrapObj->_param = Persistent<Object>::New();
			//BViewParamTo(vRt,handle->Param.Handle());
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onMouse")){
			if(!value->IsFunction()) return;
			obj->_WrapObj->BOnMouse = Persistent<Function>::New(Local<Function>::Cast(value));
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onCreate")){
			if(!value->IsFunction()) return;
			obj->_WrapObj->BOnCreate = Persistent<Function>::New(Local<Function>::Cast(value));
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onClose")){
			if(!value->IsFunction()) return;
			obj->_WrapObj->BOnClose = Persistent<Function>::New(Local<Function>::Cast(value));
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onCommand")){
			if(!value->IsFunction()) return;
			obj->_WrapObj->BOnCommand = Persistent<Function>::New(Local<Function>::Cast(value));
		}else if(base::WcsEqual((LPCWSTR)*pty,L"onDraw")){
			if(!value->IsFunction()) return;
			obj->_WrapObj->BOnDraw = Persistent<Function>::New(Local<Function>::Cast(value));
		}
	}
	void BView::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BView";
		GetTemplate()->Inherit(BWndPtr::GetTemplate());
		SETACCESSOR(L"minSize",BView);
		SETACCESSOR(L"param",BView);
		SETACCESSOR(L"onCreate",BView);
		SETACCESSOR(L"onClose",BView);
		SETACCESSOR(L"onCommand",BView);
		SETACCESSOR(L"onMouse",BView);
		SETACCESSOR(L"onDraw",BView);
		SETFUNC(create,BView);
		SETFUNC(scr,BView);
		SETFUNC(logPos,BView);
	}
	//BFrame
	void _BFrame::Initialize(){
		initialize();
	}
	//Persistent<FunctionTemplate> BFrame::FuncT;
	Handle<Value> BFrame::_create(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();

		BFrame* obj = (BFrame*)self->GetPointerFromInternalField(0);
		_ASSERT_EXPR(obj!=NULL,L"BFrame._create");
		Frame* handle = (Frame*)obj->_WrapObj->CPPObj;
		if(handle==NULL) return False();

		bool quit;
		if(args.Length()>1){
			quit = args[1]->BooleanValue();
		}else quit = 1;

		HWND parent;
		if(args.Length()>0){
			parent = (HWND)(INT_PTR)args[0]->Int32Value();
		}else parent = 0;

		Handle<Object> vParam = obj->_WrapObj->_param;
		BViewParamTo(vParam,handle->Param.Handle());

		if(!handle->Create(parent,1,quit)){
			base::SystemMessageBox(0,L"Create BFrame",MT_ERROR);
			return False();
		}

		return True();  
	}
	void BFrame::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BFrame";
		GetTemplate()->Inherit(BView::GetTemplate());
		SETFUNC(create,BFrame);
	}
	void LoadWndToJS(Handle<Object>& gObj){
		HandleScope store;
		LoadBMouse();
		BWndPtr::LoadToJS(gObj);
		BView::LoadToJS(gObj);
		BFrame::LoadToJS(gObj);
	}
}