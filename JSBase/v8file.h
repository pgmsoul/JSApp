#pragma once
namespace v8{
	class _BMemory : public _WrapHandle<base::Memory<byte>,_Handle>{
	};
	class BMemory : public BObject<base::Memory<byte>,_BMemory,BMemory>{
	protected:
		static Handle<Value> _zero(const Arguments& args);
		static Handle<Value> _toHex(const Arguments& args);
		static Handle<Value> _fromHex(const Arguments& args);
		static Handle<Value> _toArray(const Arguments& args);
		static Handle<Value> _fromArray(const Arguments& args);
		static Handle<Value> _getString(const Arguments& args);
		static Handle<Value> _fromString(const Arguments& args);
		static Handle<Value> _move(const Arguments& args);
		static Handle<Value> _copyTo(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class Code64 : public base::_class{
	public:
		int dic[128];
		base::String key,code;
		void initKeyDiction();
		uint getCodeLen();
		uint encode64(byte* a,uint len);
		void decode64(byte* mem);
		Code64();
	};
	class _BCode64 : public _WrapHandle<Code64,_Handle>{
	};
	class BCode64 : public BObject<Code64,_BCode64,BCode64>{
	protected:
		static Handle<Value> _encodeMemory(const Arguments& args);
		static Handle<Value> _toMemory(const Arguments& args);
		static Handle<Value> _encodeString(const Arguments& args);
		static Handle<Value> _toString(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class BFileSystem : public base::_class{
	protected:
		static Handle<Value> _isFolder(const Arguments& args);
		static Handle<Value> _isExist(const Arguments& args);
		static Handle<Value> _isFile(const Arguments& args);
		static Handle<Value> _createFolder(const Arguments& args);
		static Handle<Value> _addAttribute(const Arguments& args);
		static Handle<Value> _delAttribute(const Arguments& args);
		static Handle<Value> _delete(const Arguments& args);
		static Handle<Value> _copy(const Arguments& args);
		static Handle<Value> _move(const Arguments& args);
	public:
		static void LoadToJS(v8::Handle<Object>& glb);
	};
	class _BFile : public _WrapHandle<base::File,_Handle>{
	};
	class BFile : public BObject<base::File,_BFile,BFile>{
	protected:
		static Handle<Value> _flush(const Arguments& args);
		static Handle<Value> _open(const Arguments& args);
		static Handle<Value> _create(const Arguments& args);
		static Handle<Value> _close(const Arguments& args);
		static Handle<Value> _read(const Arguments& args);
		static Handle<Value> _write(const Arguments& args);
		static Handle<Value> _get(Local<String> property,const AccessorInfo& info);
		static void _set(Local<String> property, Local<Value> value,const AccessorInfo& info);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	class _BFileSearch : public _WrapHandle<base::FileTree,_Handle>{
	};
	class BFileSearch : public BObject<base::FileTree,_BFileSearch,BFileSearch>{
	protected:
		static Handle<Value> _search(const Arguments& args);
		static void _onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate);
	public:
		inline static void LoadToJS(v8::Handle<Object>& glb){_loadToJS(glb,_onLoad);}
	};
	void LoadFileToJS(Handle<Object>& gObj);
}