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
	if(!result.IsEmpty()){//只有出错的时候才会返回Empty, 否则即使没有返回值, result仍然是Undefined.
		Local<Object> gObj = getGlobal();
		Local<Function> main = GetJSVariant<Function>(gObj,L"main");
		if(main.IsEmpty()){
			//InnerMsg(L"没有发现 main 函数",MT_ERROR);
		}else if(!main->IsFunction()){
			//InnerMsg(L"main 不是函数",MT_ERROR);
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
