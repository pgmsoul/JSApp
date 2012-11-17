#include "stdafx.h"
#include "innerv8.h"
namespace v8{
	Ctrl* getCtrl(Handle<Value> obj){
		HandleScope store;
		BCtrl* bObj = (BCtrl*)obj->ToObject()->GetPointerFromInternalField(0);
		return (Ctrl*)bObj->_WrapObj->CPPObj;
	}
	Handle<Value> BCtrl::_create(const Arguments& args){
		Ctrl* cpp = getCtrl(args.Holder());
		HWND parent = GetArgs<HWND>(args[0]);
		bool r = cpp->Create(parent)!=0;
		return Bool(r);
	}
	void BCtrl::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		name = L"BCtrl";
		GetTemplate()->Inherit(BWndPtr::GetTemplate());
		SETCLAFUNC(create,BCtrl);
	}

	_BEdit::_BEdit(){
		//CPPObj = new Edit;
	}
	_BEdit::~_BEdit(){
	}
	void BEdit::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		name = L"BEdit";
		GetTemplate()->Inherit(BCtrl::GetTemplate());
	}
	void LoadCtrlToJS(Handle<Object>& gObj){
		HandleScope store;
		BCtrl::LoadToJS(gObj);
		BEdit::LoadToJS(gObj);
	}
}