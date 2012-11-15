#pragma once
namespace v8{
	void BRectFrom(v8::Handle<Value> vObj,base::Rect* r);
	bool BRectTo(v8::Handle<Value> vObj,base::Rect* r);
	void BTwinFrom(v8::Handle<Value> vObj,base::Twin* r);
	bool BTwinTo(v8::Handle<Value> vObj,base::Twin* r);
}