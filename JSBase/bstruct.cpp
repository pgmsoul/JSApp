#include "stdafx.h"
#include "bstruct.h"
namespace v8{
	void BRectFrom(v8::Handle<Value> value,base::Rect* r){
		if(!value->IsObject()) return;
		Local<Object> vObj = value->ToObject();
		vObj->Set(String::New("left"),Int32::New(r->left));
		vObj->Set(String::New("top"),Int32::New(r->top));
		vObj->Set(String::New("right"),Int32::New(r->right));
		vObj->Set(String::New("bottom"),Int32::New(r->bottom));
	}
	bool BRectTo(v8::Handle<Value> value,base::Rect* r){
		if(!value->IsObject()) return false;
		Local<Object> vObj = value->ToObject();
		Local<Value> vl = vObj->Get(String::New("left"));
		if(vl->IsUndefined()) return false;
		Local<Value> vt = vObj->Get(String::New("top"));
		if(vt->IsUndefined()) return false;
		Local<Value> vr = vObj->Get(String::New("right"));
		if(vr->IsUndefined()) return false;
		Local<Value> vb = vObj->Get(String::New("bottom"));
		if(vb->IsUndefined()) return false;
		r->left = vl->Int32Value();
		r->top = vt->Int32Value();
		r->right = vr->Int32Value();
		r->bottom = vb->Int32Value();
		return true;
	}
	//Twin
	void BTwinFrom(v8::Handle<Value> value,base::Twin* r){
		if(!value->IsObject()) return;
		Local<Object> vObj = value->ToObject();
		vObj->Set(String::New("x"),Int32::New(r->x));
		vObj->Set(String::New("y"),Int32::New(r->y));
	}
	bool BTwinTo(v8::Handle<Value> value,base::Twin* r){
		if(!value->IsObject()) return false;
		Local<Object> vObj = value->ToObject();
		Local<Value> vx = vObj->Get(String::New("x"));
		if(vx->IsUndefined()) return false;
		Local<Value> vy = vObj->Get(String::New("y"));
		if(vy->IsUndefined()) return false;
		r->x = vx->Int32Value();
		r->y = vy->Int32Value();
		return true;
	}
}