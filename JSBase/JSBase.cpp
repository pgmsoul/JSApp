// JSBase.cpp : 定义 DLL 应用程序的导出函数。
//

#include "stdafx.h"
#include "resource.h"
#include "JSBase.h"
#include "innerv8.h"
JSBASE_API void __stdcall Initialize(Persistent<Context>* c){
	g_c = c;
	Local<Object> glb = (*g_c)->Global();
	base::ResID jr(IDR_JS_BASE);
	jr.Instance = baseHandle;
	runJSRes(jr,L"base.pre");
	LoadGDIToJS(glb);
	LoadWndToJS(glb);
	LoadCtrlToJS(glb);
	LoadWndFunc(glb);
	LoadFileToJS(glb);
	jr = IDR_JS_AFTER;
	runJSRes(jr,L"base.after");
}
