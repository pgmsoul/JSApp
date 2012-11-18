#include "stdafx.h"
#include "innerv8.h"
namespace v8{
	base::Memory<byte>* getMemory(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BMemory* obj = (BMemory*)self->GetPointerFromInternalField(0);
		if(obj==NULL) return 0;
		return (base::Memory<byte>*)obj->_WrapObj->CPPObj;
	}
	base::Memory<byte>* getMemory(Handle<Object> self){
		BMemory* obj = (BMemory*)self->GetPointerFromInternalField(0);
		if(obj==NULL) return 0;
		return (base::Memory<byte>*)obj->_WrapObj->CPPObj;
	}
	Handle<Value> BMemory::_zero(const Arguments& args){
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return Undefined();
		mem->Zero();
		return Undefined();
	}
	Handle<Value> BMemory::_toHex(const Arguments& args){
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return Undefined();
		base::String hex;
		hex.HexBinary(mem->Handle(),mem->Length());
		return String::New((uint16_t*)hex.Handle());
	}
	Handle<Value> BMemory::_fromHex(const Arguments& args){
		if(args.Length()<1) return False();
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return False();
		base::String hex;
		GetString(args[0],hex);
		hex.ToBinary(*mem);
		return True();
	}
	Handle<Value> BMemory::_getString(const Arguments& args){
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return String::New("");
		DWORD cp = CP_THREAD_ACP;
		if(args.Length()>0){
			base::String code;
			GetString(args[0],code);
			cp = base::GetCodePage(code);
		}
		base::String str;
		str.FromMultiByte((LPCSTR)mem->Handle(),mem->Length(),cp);
		return String::New((uint16_t*)str.Handle());
	}
	Handle<Value> BMemory::_fromString(const Arguments& args){
		if(args.Length()<1) return False();
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return False();
		base::String str;
		GetString(args[0],str);
		DWORD cp = CP_THREAD_ACP;
		if(args.Length()>1){
			base::String code;
			GetString(args[1],code);
			cp = base::GetCodePage(code);
		}
		int len = str.ToMultiByte((base::Memory<char>*)mem,cp);
		mem->SetLength(len);
		return True();
	}
	//把指定起始位置和长度的内存填充到一个数组中, 并且返回它. 如果失败返回一个空数组.
	Handle<Value> BMemory::_toArray(const Arguments& args){
		HandleScope store;
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return store.Close(Array::New());
		uint start = 0,end;
		if(args.Length()>0) start = args[0]->Uint32Value();
		if(mem->Length()<=start) return store.Close(Array::New());
		if(args.Length()>1){
			end = args[1]->Uint32Value() + start;
			if(end>mem->Length()) end = mem->Length();
		}else end = mem->Length();
		if(end<=start) end = start;
		Local<Array> a = Array::New(end-start);
		for(uint i=start,j=0;i<end;i++,j++){
			a->Set(j,Int32::New((*mem)[i]));
		}
		return store.Close(a);
	}
	//最多4个参数, 分别是:源数组,内存目的位置,源数组起始位置,源数组拷贝长度.
	Handle<Value> BMemory::_fromArray(const Arguments& args){
		if(args.Length()<1) return False();
		uint mPos,aPos,aLen;
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return False();
		HandleScope store;
		if(!args[0]->IsArray()) return False();
		Local<Array> a = Local<Array>::Cast(args[0]);
		if(args.Length()>1) mPos = args[1]->Uint32Value();
		else mPos = 0;
		if((int)mPos<0) return False();
		if(args.Length()>2) aPos = args[2]->Uint32Value();
		else aPos = 0;
		uint len = a->Length();
		if(aPos>=len) return False();
		if(args.Length()>3) aLen = args[3]->Uint32Value();
		else aLen = len - aPos;
		if(aLen>(len-aPos)) aLen = len - aPos;
		if(mem->Length()<mPos+aLen){
			if(!mem->SetLength(mPos+len)) return False();
		}
		aLen += aPos;
		for(uint i=mPos,j=aPos;j<aLen;i++,j++){
			(*mem)[i] = a->Get(j)->Int32Value()&0xFF;
		}
		return True();
	}
	Handle<Value> BMemory::_move(const Arguments& args){
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL) return Uint32::New(0);
		uint dst,src,len;
		dst = GETARGSINT(0);
		src = GETARGSINT(1);
		len = GETARGSINT(2);
		return Uint32::New(mem->Move(dst,src,len));
	}
	//最多4个参数: 源Memory, 源起始位置, 源长度, 目的位置.
	Handle<Value> BMemory::_copyTo(const Arguments& args){
		HandleScope store;
		Local<Integer> uZero = Uint32::New(0);
		if(args.Length()<1) return store.Close(uZero);
		uint sPos,sLen,dPos;
		sPos = GETARGSINT(1);
		sLen = GETARGSINT(2);
		dPos = GETARGSINT(3);
		Local<Object> dm = args[0]->ToObject();
		if(dm->GetConstructor()!=BMemory::GetTemplate()->GetFunction()) 
			return store.Close(uZero);
		base::Memory<byte>* dmem = getMemory(dm);
		base::Memory<byte>* mem = getMemory(args);
		if(mem==NULL||dmem==NULL) return store.Close(uZero);
		if((dmem->Length()<=dPos)||(dmem->Length()<dPos+sLen)) return store.Close(uZero);
		uint rst = mem->CopyTo(dmem->Handle()+dPos,sLen,sPos);
		return store.Close(Uint32::New(rst));
	}
	Handle<Value> BMemory::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		base::Memory<byte>* mem = getMemory(info.Holder());
		if(mem){
			if(StringIs(property,L"length")){
				return store.Close(Uint32::New(mem->Length()));
			}
		}
		return store.Close(Uint32::New(0));
	}
	void BMemory::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		HandleScope store;
		base::Memory<byte>* mem = getMemory(info.Holder());
		if(mem==NULL) return;
		if(StringIs(property,L"length")){
			mem->SetLength(value->Uint32Value());
		}
	}
	void BMemory::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BMemory";
		SETACCESSOR(L"length",BMemory);
		SETCLAFUNC(zero,BMemory);
		SETCLAFUNC(fromArray,BMemory);
		SETCLAFUNC(toArray,BMemory);
		SETCLAFUNC(fromHex,BMemory);
		SETCLAFUNC(toHex,BMemory);
		SETCLAFUNC(fromString,BMemory);
		SETCLAFUNC(getString,BMemory);
		SETCLAFUNC(copyTo,BMemory);
		SETCLAFUNC(move,BMemory);
	}

	///////////
	Code64::Code64(){
		key = L"PaAwO65goUf7IK2vi9}xq8cFTEXLCDY1Hd3tV0ryzjbpN{BlnSs4mGRkQWMZJeuh";
		initKeyDiction();
	}
	uint Code64::encode64(byte* a,uint len){
		uint sLen = (len/3)*4;
		uint res = len % 3; //3个字节一组进行处理, 余下特殊处理
		if(res==1) sLen += 2;
		else if(res==2) sLen += 3;
		if(code.Cubage()<sLen) code.SetCubage(sLen);
		code[sLen] = 0;
		uint i = 2, j = 0, v;
		for (; i < len; i += 3,j += 4) {//每3个字节用4个字符表示, 相当于3个字符(实际上是6个字节)用8个字符编码(实际为16个字节), 看起来容量膨胀了很多, 但是在启用压缩的情况下, 这些又被抵消掉了.
			v = a[i - 2] + (a[i - 1] << 8) + (a[i] << 16);
			code[j] = key[v & 0x3f];
			code[j+1] = key[(v >> 6) & 0x3f];
			code[j+2] = key[(v >> 12) & 0x3f];
			code[j+3] = key[(v >> 18)];
		}
		if (res == 1) {//字节余一位时候, 补2个字符, 64*64>256
			v = a[i - 2];
			code[j] = key[v & 0x3f];
			code[j+1] = key[(v >> 6) & 0x3f];
		} else if (res == 2) {//字节余2位的时候, 补3个字节, 64*64*64>256*256, 所以是可行的.
			v = a[i - 2] + (a[i - 1] << 8);
			code[j] = key[v & 0x3f];
			code[j+1] = key[(v >> 6) & 0x3f];
			code[j+2] = key[(v >> 12) & 0x3f];
		}
		code.Realize();
		return sLen;
	}
	void Code64::initKeyDiction(){
		::ZeroMemory(dic,128*4);
		for(uint i=0;i<64;i++){
			dic[key[i]] = i;
		}
	}
	uint Code64::getCodeLen(){
		uint sLen = code.Length();
		for(uint i=0;i<code.Length();i++){
			if(code[i]>=128||dic[code[i]]==0){
				sLen = i;
				code[i] = 0;
				code.Realize();
				break;
			}
		}
		return sLen;
	}
	uint getMemLen(uint codeLen){
		uint res = codeLen % 4;
		uint len = (codeLen/4)*3;
		if(res==2) len += 1;
		else if(res==3) len += 2;
		return len;
	}
	void Code64::decode64(byte* mem){
		uint codeLen = code.Length();
		uint res = codeLen % 4;
		uint i = 3, j = 0, v;
		for (; i < codeLen; i += 4,j += 3) {
			v = dic[code[i - 3]];
			v += dic[code[i - 2]] << 6;
			v += dic[code[i - 1]] << 12;
			v += dic[code[i]] << 18;
			mem[j] = v & 0xff;
			mem[j+1] = (v >> 8) & 0xff;
			mem[j+2] = (v >> 16) & 0xff;
		}
		if (res == 2) {//正确的字节数肯定是余2或3, 没有1的情况, 如果出现, 舍弃.
			v = dic[code[i - 3]];
			v += dic[code[i - 2]] << 6;
			mem[j] = v & 0xff;
		} else if (res == 3) {
			v = dic[code[i - 3]];
			v += dic[code[i - 2]] << 6;
			v += dic[code[i - 1]] << 12;
			mem[j] = v & 0xff;
			mem[j+1] = (v >> 8) & 0xff;
		}
	}
	/////////////////////
	Code64* getCode64(const Arguments& args){
		HandleScope store;
		BCode64* obj = (BCode64*)args.Holder()->GetPointerFromInternalField(0);
		if(obj==NULL) return 0;
		return (Code64*)obj->_WrapObj->CPPObj;
	}
	Code64* getCode64(Handle<Value> self){
		HandleScope store;
		BCode64* obj = (BCode64*)self->ToObject()->GetPointerFromInternalField(0);
		if(obj==NULL) return 0;
		return (Code64*)obj->_WrapObj->CPPObj;
	}
	Handle<Value> BCode64::_encodeMemory(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		if(args.Length()<1) return False();
		Code64* c = getCode64(self);
		if(c==NULL) return False();
		Local<Object> vMem = args[0]->ToObject();
		if(!ObjIsType<BMemory>(vMem)) return False();
		base::Memory<byte>* mem = getMemory(vMem);
		if(mem==NULL) return False();
		c->encode64(mem->Handle(),mem->Length());
		return True();
	}
	Handle<Value> BCode64::_toMemory(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		if(args.Length()<1) return False();
		Local<Object> vMem = args[0]->ToObject();
		if(vMem->GetConstructor()!=BMemory::GetTemplate()->GetFunction()) return False();
		base::Memory<byte>* mem = getMemory(vMem);
		Code64* c = getCode64(self);
		if(mem==NULL||c==NULL) return False();
		uint memLen = getMemLen(c->code.Length());
		mem->SetLength(memLen);
		c->decode64(mem->Handle());
		return True();
	}
	Handle<Value> BCode64::_encodeString(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		Local<Object> self = args.Holder();
		Code64* c = getCode64(self);
		if(c==0) return False();
		base::String str;
		GetString(args[0],str);
		c->encode64((byte*)str.Handle(),str.Length()*2);
		return True();
	}
	Handle<Value> BCode64::_toString(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		Code64* c = getCode64(self);
		if(c==NULL) return store.Close(String::New(""));
		uint memLen = getMemLen(c->code.Length());
		base::String str(memLen/2);
		c->decode64((byte*)str.Handle());
		str.Realize();
		return store.Close(String::New((uint16_t*)str.Handle()));
	}
	Handle<Value> BCode64::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		Handle<Object> self = info.Holder();
		Code64* c = getCode64(self);
		if(c==NULL) return store.Close(String::New(""));
		if(StringIs(property,L"key")){
			return store.Close(String::New((uint16_t*)c->key.Handle()));
		}else if(StringIs(property,L"code")){
			return store.Close(String::New((uint16_t*)c->code.Handle()));
		}
		return Undefined();
	}
	void BCode64::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		HandleScope store;
		Handle<Object> self = info.Holder();
		Code64* c = getCode64(self);
		if(c==NULL) return;
		if(StringIs(property,L"key")){
			base::String key;
			GetString(value,key);
			if(key.Length()!=64) return;
			for(uint i=0;i<64;i++){
				if(-1==key.Find(c->key[i])) return;
			}
			c->key = key;
			c->initKeyDiction();
		}else if(StringIs(property,L"code")){
			GetString(value,c->code);
			c->getCodeLen();
		}
	}
	void BCode64::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BCode64";
		ot_inst->SetAccessor(String::New("key"),_get,_set);
		ot_inst->SetAccessor(String::New("code"),_get,_set);
		SETCLAFUNC(toMemory,BCode64);
		SETCLAFUNC(encodeMemory,BCode64);
		SETCLAFUNC(toString,BCode64);
		SETCLAFUNC(encodeString,BCode64);
	}
	///////////////////////////////////////////
	Handle<Value> BFileSystem::_isFolder(const Arguments& args){
		if(args.Length()<1) return False();
		base::String file;
		GetString(args[0],file);
		bool r = 2==base::GetFileType(file);
		return Bool(r);
	}
	Handle<Value> BFileSystem::_isExist(const Arguments& args){
		if(args.Length()<1) return False();
		base::String file;
		GetString(args[0],file);
		bool r = 0!=base::GetFileType(file);
		return Bool(r);
	}
	Handle<Value> BFileSystem::_isFile(const Arguments& args){
		if(args.Length()<1) return False();
		base::String file;
		GetString(args[0],file);
		bool r = 1==base::GetFileType(file);
		return Bool(r);
	}
	typedef struct _FSINF : public base::_struct{
		Local<Object> recv;
		Local<Function> fileProc,folderProc;
		Local<Object> inf;
	}FSINF;
	void __stdcall _onFileProgress(uint64 copyed,uint64 total,bool& b,void* ud){
		FSINF* fi = (FSINF*)ud;
		Handle<Value> args[2];
		args[0] = Number::New((double)copyed);
		args[1] = Number::New((double)total);
		b = fi->fileProc->Call(fi->recv,2,args)->BooleanValue();
	}
	void __stdcall _onDirProgress(base::DirData* dd,void* ud){
		FSINF* fi = (FSINF*)ud;
		Handle<Value> args[1];
		args[0] = fi->inf;
		fi->inf->Set(String::New("src"),String::New((uint16_t*)dd->SourceDir));
		fi->inf->Set(String::New("dst"),String::New((uint16_t*)dd->DestDir));
		fi->inf->Set(String::New("file"),String::New((uint16_t*)dd->FindData->cFileName));
		fi->inf->Set(String::New("isFolder"),Boolean::New(0!=(dd->FindData->dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)));
		fi->inf->Set(String::New("stop"),Boolean::New(dd->Stop));
		fi->inf->Set(String::New("skip"),Boolean::New(dd->Cancel));
		fi->inf->Set(String::New("error"),Int32::New(dd->Error));
		fi->inf->Set(String::New("preOrAfter"),Boolean::New(!dd->PreOrAfter));
		fi->folderProc->Call(fi->recv,1,args);
		dd->Cancel = fi->inf->Get(String::New("skip"))->BooleanValue();
		dd->Stop = fi->inf->Get(String::New("stop"))->BooleanValue();
	}
	void _bfs(base::DSParam& fs,FSINF& fi,const Arguments& args){
		if(args.Length()>2&&args[2]->IsFunction()){
			fi.recv = args.Holder();
			fi.folderProc = Local<Function>::Cast(args[2]);
			fi.inf = Object::New();
			fs.UserData = &fi;
			fs.OnProgress.Bind(_onDirProgress);
		}
		if(args.Length()>3&&args[3]->IsFunction()){
			fi.fileProc = Local<Function>::Cast(args[3]);
			fs.OnFileProgress.Bind(_onFileProgress);
		}
	}
	Handle<Value> BFileSystem::_delete(const Arguments& args){
		if(args.Length()<1) return False();
		base::DSParam dsp;
		GetString(args[0],dsp.Source);
		HandleScope store;
		FSINF fi;
		if(args.Length()>1&&args[1]->IsFunction()){
			fi.recv = args.Holder();
			fi.inf = Object::New();
			fi.folderProc = Local<Function>::Cast(args[1]);
			dsp.UserData = &fi;
			dsp.OnProgress.Bind(_onDirProgress);
		}
		return Bool(base::DirectorySystem::Delete(&dsp));
	}
	Handle<Value> BFileSystem::_copy(const Arguments& args){
		if(args.Length()<2) return False();
		base::DSParam dsp;
		GetString(args[0],dsp.Source);
		GetString(args[1],dsp.Destinate);
		base::DirectorySystem fs;
		HandleScope store;
		FSINF fi;
		_bfs(dsp,fi,args);
		return Bool(base::DirectorySystem::Copy(&dsp));
	}
	Handle<Value> BFileSystem::_move(const Arguments& args){
		if(args.Length()<2) return False();
		base::DSParam dsp;
		GetString(args[0],dsp.Source);
		GetString(args[1],dsp.Destinate);
		HandleScope store;
		FSINF fi;
		_bfs(dsp,fi,args);
		return Bool(base::DirectorySystem::Move(&dsp));
	}

	Handle<Value> BFileSystem::_createFolder(const Arguments& args){
		if(args.Length()<1) return False();
		base::String file;
		GetString(args[0],file);
		uint b = GETARGSINT(1);
		bool r = base::CreateFolder(file,0!=b);
		return Bool(r);
	}
	Handle<Value> BFileSystem::_addAttribute(const Arguments& args){
		if(args.Length()<2) return False();
		base::String file;
		GetString(args[0],file);
		uint attr = GETARGSINT(1);
		bool r = base::SetFileAttr(file,attr,0);
		return Bool(r);
	}
	Handle<Value> BFileSystem::_delAttribute(const Arguments& args){
		if(args.Length()<2) return False();
		base::String file;
		GetString(args[0],file);
		uint attr = GETARGSINT(1);
		bool r = base::SetFileAttr(file,0,attr);
		return Bool(r);
	}
	void BFileSystem::LoadToJS(v8::Handle<Object>& glb){
		HandleScope store;
		Local<Object> obj = Object::New();
		glb->Set(String::New("BFileSystem"),obj,ReadOnly);

		SETOBJCLAFUNC(obj,isFile,BFileSystem);
		SETOBJCLAFUNC(obj,isFolder,BFileSystem);
		SETOBJCLAFUNC(obj,isExist,BFileSystem);
		SETOBJCLAFUNC(obj,delete,BFileSystem);
		SETOBJCLAFUNC(obj,copy,BFileSystem);
		SETOBJCLAFUNC(obj,move,BFileSystem);

		SETOBJCONST(obj,OK,FS);
		SETOBJCONST(obj,FALSE,FS);
		SETOBJCONST(obj,SAME_NAME,FS);
		SETOBJCONST(obj,OPEN_DEST_FAILED,FS);
		SETOBJCONST(obj,OPEN_SOURCE_FAILED,FS);
		SETOBJCONST(obj,FILE_COPYED,FS);
		SETOBJCONST(obj,COPY_PAUSE,FS);
		SETOBJCONST(obj,CANNOT_ACCESS,FS);
		SETOBJCONST(obj,DEL_FAILED,FS);
		SETOBJCONST(obj,MOVE_FAILED,FS);
		SETOBJCONST(obj,MOVE_COPY_FAILED,FS);
	}
	//////////////////////////////////////
	base::File* getFile(const Arguments& args){
		HandleScope store;
		BFile* obj = (BFile*)args.Holder()->GetPointerFromInternalField(0);
		if(obj==NULL) return 0;
		return (base::File*)obj->_WrapObj->CPPObj;
	}
	base::File* getFile(Handle<Value> v){
		HandleScope store;
		BFile* obj = (BFile*)v->ToObject()->GetPointerFromInternalField(0);
		if(obj==NULL) return 0;
		return (base::File*)obj->_WrapObj->CPPObj;
	}
	//打开一个文件, 如果不存在尝试生成它, 参数: 文件名, 如果是创建, 第二个参数
	//设置新文件的属性. Temperary属性的文件, 关闭后自动删除.
	Handle<Value> BFile::_create(const Arguments& args){
		HandleScope store;
		while(1){
			if(args.Length()<1) break;
			base::File* f = getFile(args);
			if(f==NULL) break;
			base::String name;
			GetString(args[0],name);
			DWORD attr = FILE_ATTRIBUTE_NORMAL;
			if(args.Length()>1){
				attr = args[1]->Uint32Value();
			}
			bool r = f->Create(name,OPEN_ALWAYS,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_GENERIC_READ|FILE_GENERIC_WRITE,attr);
			return Bool(r);
		}
		return False();
	}
	//打开一个文件, 参数: 文件名
	Handle<Value> BFile::_open(const Arguments& args){
		HandleScope store;
		while(1){
			if(args.Length()<1) break;
			base::File* f = getFile(args);
			if(f==NULL) break;
			base::String name;
			GetString(args[0],name);
			bool r = f->Create(name,OPEN_EXISTING,FILE_SHARE_READ|FILE_SHARE_WRITE,FILE_GENERIC_READ|FILE_GENERIC_WRITE,FILE_ATTRIBUTE_NORMAL);
			return Bool(r);
		}
		return False();
	}
	Handle<Value> BFile::_close(const Arguments& args){
		base::File* f = getFile(args);
		if(f==NULL) return False();
		return Bool(f->Close());
	}
	Handle<Value> BFile::_flush(const Arguments& args){
		base::File* f = getFile(args);
		if(f==NULL) False();
		return Bool(f->Flush());
	}
	//参数:memory,readLen,memoryPos. return 实际读取的长度.
	//默认(没有指定readLen和memoryPos)读取最多把Memory填满或者读到文件尾部. 
	//也就是Memory不会自动扩大, 如果指定了readLen且readLen大于Memory可用内
	//存但小于等于文件可读数据, memory会被扩大.
	Handle<Value> BFile::_read(const Arguments& args){
		HandleScope store;
		while(1){
			if(args.Length()<1||!ObjIsType<BMemory>(args[0])) return False();
			base::Memory<byte>* m = getMemory(args[0]->ToObject());
			if(m==NULL) break;
			base::File* f = getFile(args);
			if(f==NULL) break;
			int readLen,memPos;
			if(args.Length()>1){
				if(!args[1]->IsInt32()) break;
				readLen = args[1]->Int32Value();
				if(readLen<0){
					uint64 maxLen = f->GetLength() - f->GetPointer();
					if(maxLen>0x7FFFFFFF) break;
					readLen = (int)maxLen;
				}
			}else readLen = m->Length();
			if(args.Length()>2){
				if(!args[2]->IsInt32()) break;
				memPos = args[2]->Int32Value();
				if(memPos<0||memPos>(int)m->Length()) memPos = m->Length();
			}else memPos = 0;
			int bufLen = m->Length() - memPos;
			if(bufLen<readLen){
				if(!m->SetLength(memPos+readLen)) break;
			}
			readLen = f->Read(m->Handle()+memPos,readLen);
			return store.Close(Int32::New(readLen));
		}
		return store.Close(Int32::New(0));
	}
	//参数:memory,writeLen,memoryPos. return writeLen.
	//如果没有指定writeLen和memoryPos(或者指定的writeLen为负数或者大于memory
	//从指定位置到末尾的大小), memory整个(或从指定位置到末尾)被写入到文件的
	//当前位置. memoryPos为负数或大于它自身大小, 函数返回0, 没有数据写入.
	Handle<Value> BFile::_write(const Arguments& args){
		HandleScope store;
		while(1){
			if(args.Length()<1||!ObjIsType<BMemory>(args[0])) break;
			base::Memory<byte>* m = getMemory(args[0]->ToObject());
			if(m==NULL) break;
			base::File* f = getFile(args);
			if(f==NULL) break;
			int writeLen,memPos;
			if(args.Length()>1){
				if(!args[1]->IsInt32()) break;
				writeLen = args[1]->Int32Value();
			}else writeLen = -1;
			if(args.Length()>2){
				if(!args[2]->IsInt32()) break;
				memPos = args[2]->Int32Value();
				if(memPos<0||memPos>(int)m->Length()) break;
			}else memPos = 0;
			if(writeLen<0||writeLen>(int)m->Length()-memPos)
				writeLen = m->Length() - memPos;
			writeLen = f->Write(m->Handle(),writeLen);
			return store.Close(Int32::New(writeLen));
		}
		return store.Close(Int32::New(0));
	}
	Handle<Value> BFile::_get(Local<String> property,const AccessorInfo& info){
		base::File* f = getFile(info.Holder());
		if(f==NULL) return Uint32::New(0);
		if(StringIs(property,L"length")){
			return Number::New((double)f->GetLength());
		}else if(StringIs(property,L"pointer")){
			return Number::New((double)f->GetPointer());
		}
		return Uint32::New(0);
	}
	void BFile::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		base::File* f = getFile(info.Holder());
		if(f==NULL) return;
		if(StringIs(property,L"length")){
			f->SetLength((uint64)value->NumberValue());
		}else if(StringIs(property,L"pointer")){
			f->SetPointer((uint64)value->NumberValue());
		}
	}
	void BFile::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BFile";
		ot_inst->SetAccessor(String::New("length"),_get,_set);
		ot_inst->SetAccessor(String::New("pointer"),_get,_set);
		SETCLAFUNC(create,BFile);
		SETCLAFUNC(close,BFile);
		SETCLAFUNC(read,BFile);
		SETCLAFUNC(open,BFile);
		SETCLAFUNC(flush,BFile);
		SETCLAFUNC(write,BFile);

		SETCONST(READONLY,FILE_ATTRIBUTE);
		SETCONST(HIDDEN,FILE_ATTRIBUTE);
		SETCONST(SYSTEM,FILE_ATTRIBUTE);
		SETCONST(NORMAL,FILE_ATTRIBUTE);
		SETCONST(TEMPORARY,FILE_ATTRIBUTE);
	}
	/////////////////////////////////////////////////////////
	typedef struct _FTINF : public base::_struct{
		Handle<Object> self;
		Handle<Function> onList;
		Handle<Object> data;
	}FTINF;
	base::FileTree* getFileTree(Handle<Object>& self){
		BFileSearch* obj = (BFileSearch*)self->GetPointerFromInternalField(0);
		if(obj==NULL) return 0;
		return (base::FileTree*)obj->_WrapObj->CPPObj;
	}
	void __stdcall _BFileSearch_onList(base::ListData* ld){
		FTINF* fi = (FTINF*)ld->ExtraData;
		Handle<Value> args[1];
		fi->data->Set(String::New("skip"),Boolean::New(ld->Skip));
		fi->data->Set(String::New("stop"),Boolean::New(ld->Stop));
		fi->data->Set(String::New("flag"),Int32::New(ld->Flag));//0 文件, 1 pre, 2 after.
		fi->data->Set(String::New("original"),String::New((uint16_t*)ld->Original));
		fi->data->Set(String::New("relative"),String::New((uint16_t*)ld->Relative));
		fi->data->Set(String::New("name"),String::New((uint16_t*)ld->FindData->cFileName));
		args[0] = fi->data;
		fi->onList->Call(fi->self,1,args);
		ld->Skip = fi->data->Get(String::New("skip"))->BooleanValue();
		ld->Stop = fi->data->Get(String::New("stop"))->BooleanValue();
	}
	Handle<Value> BFileSearch::_search(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		Handle<Object> self = args.Holder();
		base::FileTree* ft = getFileTree(self);
		if(ft==NULL) return False();
		base::String path;
		GetString(args[0],path);
		FTINF fi;
		fi.self = self;
		fi.onList = Handle<Function>::Cast(self->Get(String::New("onList")));
		fi.data = Object::New();
		base::StringMemList* sml = ft->GetFilter();
		sml->Clear();
		Local<Array> filter = Local<Array>::Cast(self->Get(String::New("filter")));
		if(!filter.IsEmpty()){
			uint len = filter->Length();
			base::String ext;
			for(uint i=0;i<len;i++){
				GetString(filter->Get(i),ext);
				sml->Add(ext);
			}
		}
		Local<Object> param = self->Get(String::New("param"))->ToObject();
		ft->SetSearchSub(param->Get(String::New("childFolder"))->BooleanValue());
		ft->SetPreCall(param->Get(String::New("preCall"))->BooleanValue());
		ft->SetAfterCall(param->Get(String::New("afterCall"))->BooleanValue());
		ft->SetSearchDir(!param->Get(String::New("onlyFile"))->BooleanValue());
		ft->OnList.Bind(&_BFileSearch_onList);
		bool r = ft->Search(path,&fi);
		return Bool(r);
	}
	void BFileSearch::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BFileSearch";
		ot_inst->Set(String::New("search"),FunctionTemplate::New(&BFileSearch::_search),ReadOnly);
		ot_inst->Set("onList",Undefined());
		ot_inst->Set(String::New("filter"),store.Close(Array::New()),ReadOnly);
		Local<Object> param = Object::New();
		ot_inst->Set(String::New("param"),param,ReadOnly);
		param->Set(String::New("childFolder"),False());
		param->Set(String::New("preCall"),True());
		param->Set(String::New("afterCall"),False());
		param->Set(String::New("onlyFile"),True());
	}
	void LoadFileToJS(Handle<Object>& gObj){
		HandleScope store;
		BMemory::LoadToJS(gObj);
		BCode64::LoadToJS(gObj);
		BFileSystem::LoadToJS(gObj);
		BFile::LoadToJS(gObj);
		BFileSearch::LoadToJS(gObj);
	}
}