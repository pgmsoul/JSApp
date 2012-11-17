#include "stdafx.h"
#include "v8lib.h"
namespace v8{
	__declspec(thread) Persistent<Context>* g_c = 0;
	const wchar_t* ToCString(const v8::String::Value& value) {
		return *value ? (wchar_t*)*value : L"<string conversion failed>";
	}

	//设置一个只读属性为JS函数, func 是Js函数体代码, 不包括function(){这里的代码};
	void SetJsFunc(Handle<ObjectTemplate> newObjT,LPCWSTR name,LPCWSTR func){
		base::String str;
		str.Format(L"new Function(\"%s\");",func);
		newObjT->Set(String::New((uint16_t*)name),runJSCode(str),ReadOnly);
	}
	int ReportError(TryCatch& try_catch){
		base::String str;
		v8::HandleScope handle_scope;
		v8::String::Value exception(try_catch.Exception());
		const wchar_t* exception_string = ToCString(exception);
		v8::Handle<v8::Message> message = try_catch.Message();
		if (message.IsEmpty()) {
			str.Format(L"%s\n",exception_string);
		} else {
			base::String buf;
			v8::String::Value filename(message->GetScriptResourceName());
			const wchar_t* filename_string = ToCString(filename);
			int linenum = message->GetLineNumber();
			buf.Format(L"file:\t%s\r\nline:\t%i\r\n%s\n", filename_string, linenum, exception_string);
			str += buf;
			v8::String::Value sourceline(message->GetSourceLine());
			const wchar_t* sourceline_string = ToCString(sourceline);
			buf.Format(L"%s\n", sourceline_string);
			str += buf;
			int start = message->GetStartColumn();
			for (int i = 0; i < start; i++) {
				str += L" ";
			}
			int end = message->GetEndColumn();
			for (int i = start; i < end; i++) {
				str += L"^";
			}
			str += L"\n";
			v8::String::Value stack_trace(try_catch.StackTrace());
			if (stack_trace.length() > 0) {
				const wchar_t* stack_trace_string = ToCString(stack_trace);
				buf.Format(L"%s\n", stack_trace_string);
				str += buf;
			}
		}
		return MessageBox(0,str,L"Error",MB_ICONERROR);
	}

	int InnerMsg(v8::Handle<Value> msg,MSG_TYPE mt){
		HandleScope store;
		v8::String::Value v8s(msg);//Value不转换编码, 必须用Unicode初始化字串.
		return MessageBox(0,(LPCWSTR)*v8s,L"info",mt);
	}
	int InnerMsg(LPCWSTR msg,MSG_TYPE mt){
		return MessageBox(0,msg,L"info",mt);
	}
	v8::Handle<Value> runJSCode(LPCWSTR js,LPCWSTR fn){
		HandleScope store;
		Local<Object> gObj = (*g_c)->Global();
		TryCatch err;
		Handle<Value> name;
		if(fn) name = String::New((uint16_t*)fn);
		Local<Script> script = Script::Compile(String::New((uint16_t*)js),name);
		if(script.IsEmpty()){
			ReportError(err);
			return Undefined();
		}
		Local<Value> result = script->Run(); 
		if(result.IsEmpty()){
			ReportError(err);
		}
		return store.Close(result);
	}
	Handle<Value> runJSFile(LPCWSTR file,LPCWSTR cp){
		base::String js;
		if(!loadFile(js,file,cp)) return Undefined();

		return runJSCode(js,file);
	}
	bool loadFile(base::String& js,LPCWSTR file,LPCWSTR cp){
		DWORD code;
		if(cp){
			base::CodePage page;
			page.Create();
			code = page.GetCode(cp);
		}else code = 0;

		base::FileStream fs;
		if(!fs.Create(file,OPEN_EXISTING)){
			base::String inf;
			inf.Format(L"无法读取文件: %s",file);
			InnerMsg(inf.Handle(),MT_ERROR);
			return 0;
		}
		fs.ReadString(js,0,code);
		return 1;
	}
	Local<Object> getGlobal(){
		return (*g_c)->Global();
	}
	void releaseContext(){
		(*g_c)->Exit();
		(*g_c).Dispose();
		delete g_c;
		g_c = 0;
	}
	v8::Handle<Value> Alert(const v8::Arguments& args){
		int r = 0;
		if(args.Length()>0){
			String::Value str(args[0]);
			r = ::MessageBox(0,(LPCWSTR)*str,L"alert",MB_ICONINFORMATION);
		}else
			r = ::MessageBox(0,L"",L"alert",MB_ICONINFORMATION);
		return Undefined();
	}
	Handle<Value> GetCodePage(const Arguments& args){
		HandleScope store;
		base::String codeName;
		DWORD code;
		if(args.Length()>0){
			GetString(args[0],codeName);
			code = base::GetCodePage(codeName);
		}else code = 3;
		return store.Close(Uint32::New(code));
	}
	Handle<Value> _runJSCode(const Arguments& args){
		if(args.Length()<1) return Undefined();
		HandleScope store;
		String::Value js(args[0]);
		base::String fn;
		if(args.Length()>1){
			String::Value name(args[1]);
			fn = (LPCWSTR)*name;
		}
		return store.Close(runJSCode((LPCWSTR)*js,fn));
	}
	Handle<Value> _runJSFile(const Arguments& args){
		if(args.Length()<1) return Undefined();
		HandleScope store;
		String::Value js(args[0]);
		base::String fn;
		if(args.Length()>1){
			String::Value name(args[1]);
			fn = (LPCWSTR)*name;
		}
		return store.Close(runJSFile((LPCWSTR)*js,fn));
	}
	Handle<Value> _print(const Arguments& args){
		if(args.Length()<1) return Undefined();
		base::String buf;
		GetString(args[0],buf);
		buf += L"\r\n";
		::OutputDebugString(buf);
		return Undefined();
	}	
	void initContext(){
		if(g_c) return;
		g_c = new Persistent<Context>;
		HandleScope store;
		Local<ObjectTemplate> newObjT = ObjectTemplate::New();
		(*g_c) = Context::New(NULL,newObjT);
		(*g_c)->Enter();

		Local<Object> gObj = (*g_c)->Global();
		gObj->Set(String::New("alert"),FunctionTemplate::New(Alert)->GetFunction(),ReadOnly);
		gObj->Set(String::New("getCodePage"),FunctionTemplate::New(GetCodePage)->GetFunction(),ReadOnly);
		gObj->Set(String::New("loadLibrary"),FunctionTemplate::New(loadJSDLL)->GetFunction(),ReadOnly);
		gObj->Set(String::New("runJSCode"),FunctionTemplate::New(&_runJSCode)->GetFunction(),ReadOnly);
		gObj->Set(String::New("runJSFile"),FunctionTemplate::New(&_runJSFile)->GetFunction(),ReadOnly);
		gObj->Set(String::New("output"),FunctionTemplate::New(&_print)->GetFunction(),ReadOnly);
		Handle<Object> glb = runJSCode(L"(function(){return this;})();")->ToObject();
		//global 全局变量,所有顶级对象都是它的属性.
		glb->Set(String::New("global"),glb);
	}
	Handle<Value> freeJSDll(const Arguments& args){
		HandleScope store;
		Local<Value> v = args.Holder()->GetHiddenValue(String::New("handle"));
		HMODULE hDll = (HMODULE)(INT_PTR)v->Uint32Value();
		if(hDll) FreeLibrary(hDll);
		return Undefined();
	}
	Handle<Value> loadJSDLL(const Arguments& args){
		HandleScope store;
		Local<Object> handle = Object::New();
		HMODULE hDll = 0;
		if(args.Length()>0){
			v8::String::Value dll(args[0]);
			hDll = LoadLibraryEx((LPCWSTR)*dll,0,0);
			if(hDll){
				typedef void (__stdcall*INIT)(Persistent<Context>*);
				INIT init = (INIT)::GetProcAddress(hDll,"Initialize");
				if(init){
					init(g_c);
				}
			}
		}
		handle->SetHiddenValue(String::New("handle"),Uint32::New((INT_PTR)hDll));
		handle->Set(String::New("release"),FunctionTemplate::New(&freeJSDll)->GetFunction(),ReadOnly);
		return store.Close(handle);
	}
	Handle<Value> runJSRes(base::ResID jr,LPCWSTR name){
		DWORD sz;
		void* res = base::GetResource(jr,L"RT_JS",&sz);
		if(res==0) return Undefined();
		base::String js;
		js.FromMultiByte((LPCSTR)res,sz,CP_UTF8);
		return runJSCode(js,name);
	}
};
