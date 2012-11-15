#include "stdafx.h"
#include "../JSLib/jslib.h"
#include "resource.h"
#pragma usexpstyle

int __stdcall wWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow){
	int rt = -1;
	initContext();
	HandleScope store;
	runJSRes(IDR_JS_STRUCT);
	Handle<Value> result = runJSFile(L"main.js",L"utf-8");
	if(!result.IsEmpty()){//ֻ�г����ʱ��Ż᷵��Empty, ����ʹû�з���ֵ, result��Ȼ��Undefined.
		Local<Object> gObj = getGlobal();
		Local<Function> main = GetJSVariant<Function>(gObj,L"main");
		if(main.IsEmpty()){
			//InnerMsg(L"û�з��� main ����",MT_ERROR);
		}else if(!main->IsFunction()){
			//InnerMsg(L"main ���Ǻ���",MT_ERROR);
		}else{
			TryCatch err;
			Handle<Value> args[1];
			args[0] = String::New((uint16_t*)lpCmdLine);
			Local<Value> r = main->Call(gObj,1,args);
			if(!err.Exception().IsEmpty()){
				ReportError(err);
			}else
				rt = r->Int32Value();
		}
	}
	releaseContext();
	return rt;
}
