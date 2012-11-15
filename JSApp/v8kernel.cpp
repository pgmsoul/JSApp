#include "stdafx.h"
#include "innerv8.h"
namespace v8{
	base::CriticalSection _msgSync;
	HANDLE _mainThread = 0;
	base::ObjectLink<base::Function<void> > _msgPump;
	base::Event _msgEvent;
	void AddMsg(MSGCALL call){
		_msgSync.Lock();
		_msgPump.Add()->Bind(call);
		if(_msgEvent.NotNull()) _msgEvent.Signal();
		_msgSync.Unlock();
	}
	Handle<Value> startMsgPump(const Arguments& args){
		_msgEvent.Create();
		while(1){
			_msgSync.Lock();
			base::Function<void>* f = _msgPump.Element();
			if(f==0){
				_msgSync.Unlock();
				_msgEvent.Wait(100);
				continue;
			}
			_msgSync.Unlock();
			(*f)();
			_msgSync.Lock();
			_msgPump.Delete();
			_msgSync.Unlock();
		}
		return Undefined();
	}
	base::Thread* getThread(Handle<Value> obj){
		HandleScope store;
		BThread* b = (BThread*)obj->ToObject()->GetPointerFromInternalField(0);
		if(b==0) return 0;
		return (base::Thread*)b->_WrapObj->CPPObj;
	}
	void __stdcall BThread::_threadProc(void* p){
		;
	}
	Handle<Value> BThread::_start(const Arguments& args){
		HandleScope store;
		base::Thread* th = getThread(args.Holder());
		int v;
		if(args.Length()>0) v = args[0]->Uint32Value();
		else v = 0;
		th->CallStd<void>(&BThread::_threadProc,(void*)(INT_PTR)v);
		return Undefined();
	}
	void BThread::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BThread";
		SETFUNC(start,BThread);
	}
	void LoadKernelToJS(Handle<Object>& gObj){
		BThread::LoadToJS(gObj);
	}
}