#pragma once

#include "v8lib.h"
#include "V8Object.h"
#include "bstruct.h"
#ifndef JSLIB_EXPORTS
#ifdef _DEBUG
#pragma comment(lib,"D:/My Lib/CPP/Library/lib/jslibd.lib")
#else
#pragma comment(lib,"D:/My Lib/CPP/Library/lib/jslib.lib")
#endif
#endif
namespace v8{
	void loadJSLib();
}