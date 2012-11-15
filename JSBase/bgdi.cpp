#include "stdafx.h"
#include "innerv8.h"

namespace v8{
	void BColorFrom(v8::Handle<Value> val,base::gdi::Color* col){
		Local<Object> obj = val->ToObject();
		obj->Set(String::New("red"),Uint32::New(col->red));
		obj->Set(String::New("green"),Uint32::New(col->green));
		obj->Set(String::New("blue"),Uint32::New(col->blue));
		obj->Set(String::New("alpha"),Uint32::New(col->alpha));
	}
	void BColorTo(v8::Handle<Value> val,base::gdi::Color* col){
		Local<Object> obj = val->ToObject();
		col->red = obj->Get(String::New("red"))->Uint32Value();
		col->green = obj->Get(String::New("green"))->Uint32Value();
		col->blue = obj->Get(String::New("blue"))->Uint32Value();
		col->alpha = obj->Get(String::New("alpha"))->Uint32Value();
	}
	base::gdi::Icon* getIcon(Handle<Value> obj){
		BIcon* b = (BIcon*)obj->ToObject()->GetPointerFromInternalField(0);
		if(b==0) return 0;
		return (base::gdi::Icon*)b->_WrapObj->CPPObj;
	}
	//_BPen
	_BPen::_BPen():_modifyed(1){
		//CPPObj = new base::gdi::Pen;
	}
	_BPen::~_BPen(){
		if(CPPObj) delete (base::gdi::Pen*)CPPObj;
	}
	void _BPen::Initialize(){
	}
	//BPen
	base::gdi::Pen* getPen(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BPen* obj = (BPen*)self->GetPointerFromInternalField(0);
		base::gdi::Pen* pen = (base::gdi::Pen*)obj->_WrapObj->CPPObj;
		if(obj->_WrapObj->_modifyed){
			obj->_WrapObj->_modifyed = 0;
			Local<Value> cObj = self->Get(String::New("color"));
			int color;
			if(cObj->IsObject()){
				base::gdi::Color col;
				BColorTo(cObj,&col);
				color = col;
			}else color = cObj->Uint32Value();
			int style = self->Get(String::New("style"))->Uint32Value();
			int width = self->Get(String::New("width"))->Uint32Value();
			pen->Create(color,style,width);
		}
		return pen;
	}
	Handle<Value> BPen::_arc(const Arguments& args){
		if(args.Length()<3) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		base::Rect pr,ar;
		if(!BRectTo(args[1],&pr)) return False();
		if(!BRectTo(args[2],&ar)) return False();
		base::gdi::Pen* pen = getPen(args);
		pen->Select(hdc);
		bool r = 0!=::Arc(hdc,pr.left,pr.top,pr.right,pr.bottom,ar.left,ar.top,ar.right,ar.bottom);
		pen->Select(hdc);
		return Bool(r);
	}
	Handle<Value> BPen::_polyTo(const Arguments& args){
		if(args.Length()<2) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		if(!args[1]->IsArray()) return False();
		Local<Array> point = Local<Array>::Cast(args[1]);
		base::List<base::Twin> pl;
		uint len = point->Length();
		pl.Expand(len);
		for(uint i=0;i<len;i++){
			Local<Object> vPt = point->Get(i)->ToObject();
			pl[i].x = vPt->Get(String::New("x"))->Int32Value();
			pl[i].y = vPt->Get(String::New("y"))->Int32Value();
		}
		base::gdi::Pen* pen = getPen(args);
		pen->Select(hdc);
		bool r = 0!=PolylineTo(hdc,(LPPOINT)pl.GetHandle(),len);
		pen->Select(hdc);
		return Bool(r);
	}
	Handle<Value> BPen::_moveTo(const Arguments& args){
		if(args.Length()<3) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		int x = args[1]->Int32Value();
		int y = args[2]->Int32Value();
		base::gdi::Pen* pen = getPen(args);
		bool r = ::MoveToEx(hdc,x,y,0)!=0;
		return Bool(r);
	}
	Handle<Value> BPen::_lineTo(const Arguments& args){
		if(args.Length()<3) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		int x = args[1]->Int32Value();
		int y = args[2]->Int32Value();
		base::gdi::Pen* pen = getPen(args);
		pen->Select(hdc);
		bool r = ::LineTo(hdc,x,y)!=0;
		pen->Select(hdc);
		return Bool(r);
	}
	Handle<Value> BPen::_drawRect(const Arguments& args){
		int left,top,right,bottom;
		if(args.Length()>=5){
			left = args[1]->Int32Value();
			top = args[2]->Int32Value();
			right = args[3]->Int32Value();
			bottom = args[4]->Int32Value();
		}else if(args.Length()>=2){
			//可以把left的地址看成一个Rect的地址。
			if(!BRectTo(args[1],(base::Rect*)&left)) return False();
		}else
			return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		base::gdi::Pen* pen = getPen(args);
		pen->Select(hdc);
		MoveToEx(hdc,left,top,0);
		LineTo(hdc,right,top);
		LineTo(hdc,right,bottom);
		LineTo(hdc,left,bottom);
		LineTo(hdc,left,top);
		pen->Select(hdc);
		return True();
	}
	Handle<Value> BPen::_roundRect(const Arguments& args){
		int left,top,right,bottom,width,height;
		if(args.Length()>=7){
			left = args[1]->Int32Value();
			top = args[2]->Int32Value();
			right = args[3]->Int32Value();
			bottom = args[4]->Int32Value();
			width = args[5]->Int32Value();
			height = args[6]->Int32Value();
		}else if(args.Length()>=4){
			//可以把left的地址看成一个Rect的地址。
			if(!BRectTo(args[1],(base::Rect*)&left)) return False();
			width = args[2]->Int32Value();
			height = args[3]->Int32Value();
		}else
			return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		base::gdi::Pen* pen = getPen(args);
		pen->Select(hdc);
		HBRUSH hb = (HBRUSH)::SelectObject(hdc,(HBRUSH)::GetStockObject(NULL_BRUSH));
		bool r = RoundRect(hdc,left,top,right,bottom,width,height)!=0;
		::SelectObject(hdc,hb);
		pen->Select(hdc);
		return Bool(r);
	}
	Handle<Value> BPen::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		return store.Close(self->GetHiddenValue(property));
	}
	void BPen::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		Local<Object> self = info.Holder();
		BPen* obj = (BPen*)self->GetPointerFromInternalField(0);

		v8::String::Value pty(property);
		if(base::WcsEqual((LPCWSTR)*pty,L"color")){
			obj->_WrapObj->_modifyed = 1;
		}else if(base::WcsEqual((LPCWSTR)*pty,L"width")){
			obj->_WrapObj->_modifyed = 1;
		}else if(base::WcsEqual((LPCWSTR)*pty,L"style")){
			obj->_WrapObj->_modifyed = 1;
		}
		self->SetHiddenValue(property,value);
	}
	void BPen::_onLoad(v8::Handle<ObjectTemplate> &ot_func, v8::Handle<ObjectTemplate> &ot_inst, base::String &name, v8::Handle<FunctionTemplate> &objcreate){
		name = L"BPen";
		SETFUNC(moveTo,BPen);
		SETFUNC(lineTo,BPen);
		SETFUNC(drawRect,BPen);
		SETFUNC(roundRect,BPen);
		SETFUNC(polyTo,BPen);
		SETFUNC(arc,BPen);
		v8::Handle<Value> jsf = runJSCode(L"new Function(\"dc\",\"x\",\"y\",\"rx\",\"ry\",\"this.arc(dc,new BRect(x-rx,y-ry,x+rx,y+ry),new BRect(x,y-ry,x,y-ry));\")");
		ot_inst->Set(String::New("ellipse"),jsf,ReadOnly);
		SETACCESSOR(L"color",BPen);
		SETACCESSOR(L"width",BPen);
		SETACCESSOR(L"style",BPen);
		SETCONST(DASH,PS);
		SETCONST(DOT,PS);
		SETCONST(DASHDOT,PS);
		SETCONST(DASHDOTDOT,PS);
		SETCONST(NULL,PS);
		SETCONST(SOLID,PS);
	}
	//_BRgn
	_BRgn::_BRgn(){
		//CPPObj = new base::gdi::Rgn;
	}
	_BRgn::~_BRgn(){
		if(CPPObj) delete (base::gdi::Rgn*)CPPObj;
	}

	//BRgn
	base::gdi::Rgn* getRgn(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BRgn* obj = (BRgn*)self->GetPointerFromInternalField(0);
		base::gdi::Rgn* rgn = (base::gdi::Rgn*)obj->_WrapObj->CPPObj;
		return rgn;
	}
	Handle<Value> BRgn::_rect(const Arguments& args){
		if(args.Length()<=0) return False();
		int left,top,right,bottom;
		if(args.Length()>=4){
			left = args[0]->Int32Value();
			top = args[1]->Int32Value();
			right = args[2]->Int32Value();
			bottom = args[3]->Int32Value();
		}else if(args.Length()>=1){
			if(!BRectTo(args[0],(base::Rect*)&left)) return False();
		}
		base::gdi::Rgn* rgn = getRgn(args);
		bool r = rgn->CreateRect(left,top,right,bottom);
		return Bool(r);
	}
	Handle<Value> BRgn::_poly(const Arguments& args){
		if(args.Length()<=0) return False();
		if(!args[0]->IsArray()) return False();
		Local<Array> point = Local<Array>::Cast(args[0]);
		base::List<base::Twin> pl;
		uint len = point->Length();
		pl.Expand(len);
		for(uint i=0;i<len;i++){
			Local<Object> vPt = point->Get(i)->ToObject();
			pl[i].x = vPt->Get(String::New("x"))->Int32Value();
			pl[i].y = vPt->Get(String::New("y"))->Int32Value();
		}
		int mode;
		if(args.Length()>1){
			mode = args[1]->Uint32Value();
		}else mode = 1;
		base::gdi::Rgn* rgn = getRgn(args);
		bool r = rgn->CreatePolygon((LPPOINT)pl.GetHandle(),len,mode);
		return Bool(r);
	}
	Handle<Value> BRgn::_ellipse(const Arguments& args){
		if(args.Length()<=0) return False();
		int left,top,right,bottom;
		if(args.Length()>=4){
			left = args[0]->Int32Value();
			top = args[1]->Int32Value();
			right = args[2]->Int32Value();
			bottom = args[3]->Int32Value();
		}else if(args.Length()>=1){
			if(!BRectTo(args[0],(base::Rect*)&left)) return False();
		}
		base::gdi::Rgn* rgn = getRgn(args);
		bool r = rgn->CreateEllipse(left,top,right,bottom);
		return Bool(r);
	}
	Handle<Value> BRgn::_combine(const Arguments& args){
		if(args.Length()<=0) return Int32::New(0);
		BRgn* obj = (BRgn*)args[0]->ToObject()->GetPointerFromInternalField(0);
		if(obj==NULL||obj->_WrapObj==NULL||obj->_WrapObj->CPPObj==NULL) return Int32::New(0);
		base::gdi::Rgn* rgn2 = (base::gdi::Rgn*)obj->_WrapObj->CPPObj;
		base::gdi::Rgn* rgn = getRgn(args);
		DWORD cbMode;
		if(args.Length()>1) cbMode = args[1]->Uint32Value();
		else cbMode = base::gdi::Rgn::Or;
		int r = rgn->Combine(rgn2->Handle(),cbMode);
		return Int32::New(r);
	}
	Handle<Value> BRgn::_roundRect(const Arguments& args){
		int left,top,right,bottom,width,height;
		if(args.Length()>=6){
			left = args[0]->Int32Value();
			top = args[1]->Int32Value();
			right = args[2]->Int32Value();
			bottom = args[3]->Int32Value();
			width = args[4]->Int32Value();
			height = args[5]->Int32Value();
		}else if(args.Length()>=3){
			//可以把left的地址看成一个Rect的地址。
			if(!BRectTo(args[0],(base::Rect*)&left)) return False();
			width = args[1]->Int32Value();
			height = args[2]->Int32Value();
		}else
			return False();
		base::gdi::Rgn* rgn = getRgn(args);
		bool r = rgn->CreateRoundRect(left,top,right,bottom,width,height);
		return Bool(r);
	}
	Handle<Value> BRgn::_offset(const Arguments& args){
		if(args.Length()==0) return False();
		int dx,dy;
		if(args.Length()>0) dx = args[0]->Int32Value();
		if(args.Length()>1) dy = args[1]->Int32Value();
		else dy = 0;
		base::gdi::Rgn* rgn = getRgn(args);
		bool r = rgn->Offset(dx,dy);
		return Bool(r);
	}
	Handle<Value> BRgn::_equal(const Arguments& args){
		if(args.Length()<=0) return False();
		BRgn* obj = (BRgn*)args[0]->ToObject()->GetPointerFromInternalField(0);
		if(obj==NULL||obj->_WrapObj==NULL||obj->_WrapObj->CPPObj==NULL) return False();
		base::gdi::Rgn* rgn2 = (base::gdi::Rgn*)obj->_WrapObj->CPPObj;
		base::gdi::Rgn* rgn = getRgn(args);
		bool r = *rgn==*rgn2;
		return Bool(r);
	}
	Handle<Value> BRgn::_copy(const Arguments& args){
		if(args.Length()<=0) return False();
		BRgn* obj = (BRgn*)args[0]->ToObject()->GetPointerFromInternalField(0);
		if(obj==NULL||obj->_WrapObj==NULL||obj->_WrapObj->CPPObj==NULL) return False();
		base::gdi::Rgn* rgn2 = (base::gdi::Rgn*)obj->_WrapObj->CPPObj;
		base::gdi::Rgn* rgn = getRgn(args);
		*rgn = *rgn2;
		return True();
	}
	Handle<Value> BRgn::_rectBox(const Arguments& args){
		Handle<Value> rect = runJSCode(L"new BRect();");
		base::gdi::Rgn* rgn = getRgn(args);
		base::Rect pr;
		if(!rgn->GetRectBox(pr)){
			pr.SetValue(0,0,0,0);
		}else{
			BRectFrom(rect,&pr);
		}
		return rect;
	}
	Handle<Value> BRgn::_pointIn(const Arguments& args){
		if(args.Length()<2) return False();
		base::Twin tw;
		tw.x = args[0]->Int32Value();
		tw.y = args[1]->Int32Value();
		base::gdi::Rgn* rgn = getRgn(args);
		bool r = rgn->PtInRgn(tw);
		return Bool(r);
	}
	void BRgn::_onLoad(v8::Handle<ObjectTemplate> &ot_func, v8::Handle<ObjectTemplate> &ot_inst, base::String &name, v8::Handle<FunctionTemplate> &objcreate){
		name = L"BRgn";
		SETFUNC(rect,BRgn);
		SETFUNC(poly,BRgn);
		SETFUNC(ellipse,BRgn);
		SETFUNC(roundRect,BRgn);
		SETFUNC(combine,BRgn);
		SETFUNC(offset,BRgn);
		SETFUNC(equal,BRgn);
		SETFUNC(copy,BRgn);
		SETFUNC(pointIn,BRgn);
		SETFUNC(rectBox,BRgn);

		SETCONST2(AND,base::gdi::Rgn::And);
		SETCONST2(OR,base::gdi::Rgn::Or);
		SETCONST2(XOR,base::gdi::Rgn::Xor);
		SETCONST2(DIFF,base::gdi::Rgn::Diff);
		SETCONST2(COPY,base::gdi::Rgn::Copy);
		SETCONST2(ALTERNATE,ALTERNATE);
		SETCONST2(ALTERNATE,WINDING);
	}
	//BBrush
	_BBrush::_BBrush():_modifyed(1){
		//CPPObj = new base::gdi::Brush;
	}
	_BBrush::~_BBrush(){
		if(CPPObj) delete (base::gdi::Brush*)CPPObj;
	}
	void _BBrush::Initialize(){
	}
	base::gdi::Brush* getBrush(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BBrush* obj = (BBrush*)self->GetPointerFromInternalField(0);
		base::gdi::Brush* brush = (base::gdi::Brush*)obj->_WrapObj->CPPObj;
		if(obj->_WrapObj->_modifyed){
			obj->_WrapObj->_modifyed = 0;
			Local<Value> cObj = self->Get(String::New("color"));
			int color;
			if(cObj->IsObject()){
				base::gdi::Color col;
				BColorTo(cObj,&col);
				color = col;
			}else color = cObj->Uint32Value();
			brush->Create(color);
		}
		return brush;
	}
	Handle<Value> BBrush::_fillRgn(const Arguments& args){
		if(args.Length()<2) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		if(!StringIs(args[1],L"BRgn")) return False();
		BRgn* rgn = (BRgn*)args[1]->ToObject()->GetPointerFromInternalField(0);
		base::gdi::Brush* brush = getBrush(args);
		bool r = ::FillRgn(hdc,*(base::gdi::Rgn*)(rgn->_WrapObj->CPPObj),brush->Handle())!=0;
		return Bool(r);
	}
	Handle<Value> BBrush::_fillRect(const Arguments& args){
		if(args.Length()<2) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		base::Rect rect;
		if(!BRectTo(args[1],&rect)) return False();
		base::gdi::Brush* brush = getBrush(args);
		bool r = ::FillRect(hdc,rect,brush->Handle())!=0;
		return Bool(r);
	}
	Handle<Value> BBrush::_poly(const Arguments& args){
		HandleScope store;
		if(args.Length()<2) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		if(!args[1]->IsArray()) return False();
		Local<Array> point = Local<Array>::Cast(args[1]);
		base::List<base::Twin> pl;
		uint len = point->Length();
		pl.Expand(len);
		for(uint i=0;i<len;i++){
			Local<Object> vPt = point->Get(i)->ToObject();
			pl[i].x = vPt->Get(String::New("x"))->Int32Value();
			pl[i].y = vPt->Get(String::New("y"))->Int32Value();
		}
		base::gdi::Brush* brush = getBrush(args);
		brush->Select(hdc);
		HPEN hb = (HPEN)::SelectObject(hdc,(HPEN)::GetStockObject(NULL_PEN));
		bool r = ::Polygon(hdc,(LPPOINT)pl.GetHandle(),len)!=0;
		SelectObject(hdc,hb);
		brush->Select(hdc);
		return Bool(r);
	}
	Handle<Value> BBrush::_ellipse(const Arguments& args){
		HandleScope store;
		if(args.Length()<2) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		int left,top,right,bottom;
		if(args.Length()>=5){
			left = args[1]->Int32Value();
			top = args[2]->Int32Value();
			right = args[3]->Int32Value();
			bottom = args[4]->Int32Value();
		}else if(args.Length()>=2){
			if(!BRectTo(args[1],(base::Rect*)&left)) return False();
		}
		base::gdi::Brush* brush = getBrush(args);
		brush->Select(hdc);
		HPEN hb = (HPEN)::SelectObject(hdc,(HPEN)::GetStockObject(NULL_PEN));
		bool r = ::Ellipse(hdc,left,top,right,bottom)!=0;
		SelectObject(hdc,hb);
		brush->Select(hdc);
		return Bool(r);
	}
	Handle<Value> BBrush::_roundRect(const Arguments& args){
		int left,top,right,bottom,width,height;
		if(args.Length()>=7){
			left = args[1]->Int32Value();
			top = args[2]->Int32Value();
			right = args[3]->Int32Value();
			bottom = args[4]->Int32Value();
			width = args[5]->Int32Value();
			height = args[6]->Int32Value();
		}else if(args.Length()>=4){
			//可以把left的地址看成一个Rect的地址。
			if(!BRectTo(args[1],(base::Rect*)&left)) return False();
			width = args[2]->Int32Value();
			height = args[3]->Int32Value();
		}else
			return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		base::gdi::Brush* brush = getBrush(args);
		brush->Select(hdc);
		HPEN hp = (HPEN)::SelectObject(hdc,(HPEN)::GetStockObject(NULL_PEN));
		bool r = RoundRect(hdc,left,top,right,bottom,width,height)!=0;
		::SelectObject(hdc,hp);
		brush->Select(hdc);
		return Bool(r);
	}
	Handle<Value> BBrush::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		return store.Close(self->GetHiddenValue(property));
	}
	void BBrush::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		BPen* obj = (BPen*)self->GetPointerFromInternalField(0);
		v8::String::Value pty(property);
		if(base::WcsEqual((LPCWSTR)*pty,L"color")){
			obj->_WrapObj->_modifyed = 1;
		}
		self->SetHiddenValue(property,value);
	}
	void BBrush::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BBrush";
		SETFUNC(fillRgn,BBrush);
		SETFUNC(fillRect,BBrush);
		SETFUNC(ellipse,BBrush);
		SETFUNC(roundRect,BBrush);
		SETFUNC(poly,BBrush);
		SETACCESSOR(L"color",BBrush);
	}

	//BFont
	_BFont::_BFont():_modifyed(1){
		//CPPObj = new base::gdi::Font;
	}
	_BFont::~_BFont(){
		if(CPPObj) delete (base::gdi::Font*)CPPObj;
	}
	void _BFont::Initialize(){
	}
	base::gdi::Font* getFont(Handle<Value> obj){
		HandleScope store;
		BFont* bf = (BFont*)obj->ToObject()->GetPointerFromInternalField(0);
		if(bf==0) return 0;
		base::gdi::Font* font =  (base::gdi::Font*)bf->_WrapObj->CPPObj;
		if(bf->_WrapObj->_modifyed){
			bf->_WrapObj->_modifyed = 0;
			base::String name;
			GetPropertyString(obj,L"name",name);
			font->SetName(name);
			font->SetSize(GetPropertyInt(obj,L"size"));
			LPLOGFONT lf = font->LogFont();
			lf->lfItalic = GetPropertyInt(obj,L"italic");
			lf->lfWeight = GetPropertyInt(obj,L"weight");
			lf->lfUnderline = GetPropertyInt(obj,L"underline");
			lf->lfStrikeOut = GetPropertyInt(obj,L"strikeout");
			lf->lfEscapement = GetPropertyInt(obj,L"escapement");
			lf->lfOrientation = GetPropertyInt(obj,L"orientation");
			lf->lfQuality = GetPropertyInt(obj,L"quality");
			font->Create();
		}
		return font;
	}
	Handle<Value> BFont::_setDefault(const Arguments& args){
		HandleScope store;
		base::gdi::Font* font = getFont(args.Holder());
		font->SetDefault();
		return Undefined();
	}
	Handle<Value> BFont::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		return store.Close(self->GetHiddenValue(property));
	}
	void BFont::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		BFont* obj = (BFont*)self->GetPointerFromInternalField(0);
		obj->_WrapObj->_modifyed = 1;
		self->SetHiddenValue(property,value);
	}
	void BFont::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BFont";
		SETFUNC(setDefault,BFont);
		SETACCESSOR(L"name",BFont);
		SETACCESSOR(L"size",BFont);
		SETACCESSOR(L"italic",BFont);
		SETACCESSOR(L"weight",BFont);
		SETACCESSOR(L"underline",BFont);
		SETACCESSOR(L"strikeout",BFont);
		SETACCESSOR(L"escapement",BFont);
		SETACCESSOR(L"orientation",BFont);
		SETACCESSOR(L"quality",BFont);
	}
	//BBitmap
	_BBitmap::_BBitmap(){
		//CPPObj = new base::gdi::Bitmap;
	}
	_BBitmap::~_BBitmap(){
		if(CPPObj) delete (base::gdi::Bitmap*)CPPObj;
	}
	void _BBitmap::Initialize(){
	}
	base::gdi::Bitmap* getBitmap(Handle<Value> obj){
		HandleScope store;
		BBitmap* bObj = (BBitmap*)obj->ToObject()->GetPointerFromInternalField(0);
		return (base::gdi::Bitmap*)bObj->_WrapObj->CPPObj;
	}
	Handle<Value> BBitmap::_save(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		base::String file;
		GetString(args[0],file);
		int quality;
		if(args.Length()>1) quality = args[1]->Int32Value();
		else quality = 100;
		base::gdi::Image img;
		if(!img.Create(pbmp->GetClip())) return False();
		bool r = img.Save(file,quality);
		return Bool(r);
	}
	Handle<Value> BBitmap::_load(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		base::String file;
		GetString(args[0],file);
		base::gdi::Image img;
		if(!img.Create(file)) return False();
		bool r = pbmp->Attach(img.GetBitmap());
		return Bool(r);
	}
	Handle<Value> BBitmap::_getSize(const Arguments& args){
		HandleScope store;
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		base::Twin sz = pbmp->Size();
		base::String js;
		js.Format(L"new BTwin(%d,%d)",sz.x,sz.y);
		return runJSCode(js);
	}
	Handle<Value> BBitmap::_setSize(const Arguments& args){
		HandleScope store;
		if(args.Length()<2) return False();
		int cx,cy;
		cx = args[0]->Int32Value();
		cy = args[1]->Int32Value();
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		bool r = pbmp->Resize(cx,cy);
		return Bool(r);
	}
	Handle<Value> BBitmap::_create(const Arguments& args){
		HandleScope store;
		if(args.Length()<2) return False();
		int cx,cy;
		cx = args[0]->Int32Value();
		cy = args[1]->Int32Value();
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		bool r = pbmp->CreateCompatibleBitmap(cx,cy);
		return Bool(r);
	}
	Handle<Value> BBitmap::_fromBitmap(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		if(!StringIs(args[0],L"BBitmap")) return False();
		base::gdi::Bitmap* idb = getBitmap(args[0]);
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		bool r = pbmp->Attach(idb->GetClip());
		return Bool(r);
	}
	Handle<Value> BBitmap::_clip(const Arguments& args){
		HandleScope store;
		if(args.Length()==0) return True();
		int x,y,cx,cy;
		if(args.Length()>0) cx = args[0]->Int32Value();
		else cx = 0;
		if(args.Length()>1) cy = args[1]->Int32Value();
		else cy = 0;
		if(args.Length()>2) x = args[2]->Int32Value();
		else x = 0;
		if(args.Length()>3) y = args[3]->Int32Value();
		else y = 0;
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		bool r = pbmp->ClipBitmap(cx,cy,x,y);
		return Bool(r);
	}
	Handle<Value> BBitmap::_fromIcon(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		if(!StringIs(args[0],L"BIcon")) return False();
		base::gdi::Icon* idb = getIcon(args[0]);
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		bool r = pbmp->FromIcon(*idb);
		return Bool(r);
	}
	Handle<Value> BBitmap::_fromImage(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		if(!StringIs(args[0],L"BImage")) return False();
		Local<Object> vImg = args[0]->ToObject();
		BImage* iObj = (BImage*)vImg->GetPointerFromInternalField(0);
		base::gdi::DrawBitmap* idb = (base::gdi::DrawBitmap*)iObj->_WrapObj->CPPObj;
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		bool r = pbmp->Attach(idb->GetBitmap());
		return Bool(r);
	}
	Handle<Value> BBitmap::_getPixelArray(const Arguments& args){
		HandleScope store;
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		base::Memory<byte> p;
		p.SetUseSysMemory(1);
		pbmp->GetPixelData(&p,32);
		uint len = p.Length()/4;
		Local<Array> a = Array::New(len);
		for(uint i=0,j=0;j<len;i+=4,j++){
			uint v = p[i]+(p[i+1]<<8)+(p[i+2]<<16)+(p[i+3]<<24);
			a->Set((uint32_t)j,Uint32::New(v));
		}
		return a;
	}
	Handle<Value> BBitmap::_setPixelArray(const Arguments& args){
		HandleScope store;
		if(args.Length()<1||!args[0]->IsArray()) return False();
		Local<Array> a = Local<Array>::Cast(args[0]);
		base::Memory<byte> p;
		p.SetUseSysMemory(1);
		uint len = a->Length();
		p.SetLength(len*4);
		for(uint i=0,pi=0;i<len;i++,pi+=4){
			uint c = a->Get(i)->Uint32Value();
			p[pi+3] = (c>>24);
			p[pi+2] = (c>>16)&0xFF;
			p[pi+1] = (c>>8)&0xFF;
			p[pi] = c&0xFF;
		}
		base::gdi::Bitmap* pbmp = getBitmap(args.Holder());
		bool r = 0!=pbmp->SetPixelData(&p,32);
		return Bool(r);
	}
	void BBitmap::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BBitmap";
		SETFUNC(save,BBitmap);
		SETFUNC(load,BBitmap);
		SETFUNC(getSize,BBitmap);
		SETFUNC(setSize,BBitmap);
		SETFUNC(create,BBitmap);
		SETFUNC(fromBitmap,BBitmap);
		SETFUNC(fromIcon,BBitmap);
		SETFUNC(fromImage,BBitmap);
		SETFUNC(clip,BBitmap);
		SETFUNC(getPixelArray,BBitmap);
		SETFUNC(setPixelArray,BBitmap);
	}
	//BTextDraw
	_BTextDraw::_BTextDraw(){
		//CPPObj = new TextDraw;
	}
	_BTextDraw::~_BTextDraw(){
		if(CPPObj) delete (TextDraw*)CPPObj;
	}
	void _BTextDraw::Initialize(){
	}
	TextDraw* getTextDraw(const Arguments& args){
		HandleScope store;
		Local<Object> self = args.Holder();
		BTextDraw* btd = (BTextDraw*)self->GetPointerFromInternalField(0);
		TextDraw* td = (TextDraw*)btd->_WrapObj->CPPObj;
		if(btd->_WrapObj->_modifyed){
			Handle<Value> vColor = self->Get(String::New("color"));
			base::gdi::Color col;
			BColorTo(vColor,&col);
			td->color = col;

			td->align = self->Get(String::New("align"))->Int32Value();
		}
		return td;
	}
	Handle<Value> BTextDraw::_draw(const Arguments& args){
		HandleScope store;
		if(args.Length()<4) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		int x,y;
		x = args[1]->Int32Value();
		y = args[2]->Int32Value();
		base::String str;
		GetString(args[3],str);
		Local<Object> self = args.Holder();
		Local<Value> vFont = self->Get(String::New("font"));
		base::gdi::Font* font;
		if(StringIs(vFont,L"BFont")) font = getFont(vFont);
		else font = 0;
		HGLOBAL hFont = 0;
		if(font!=0){
			hFont = ::SelectObject(hdc,font->Handle());
		}
		TextDraw* td = getTextDraw(args);
		SetTextColor(hdc,td->color);
		int align = 0;
		if(td->align&DT_CENTER) align = TA_CENTER;
		if(td->align&DT_RIGHT) align = TA_RIGHT;
		if(td->align&DT_BOTTOM) align = TA_BOTTOM;
		if(td->align&DT_VCENTER) align = VTA_CENTER;
		SetTextAlign(hdc,align);
		TextOut(hdc,x,y,str,str.Length());
		if(font!=0){
			::SelectObject(hdc,hFont);
		}
		return True();
	}
	Handle<Value> BTextDraw::_drawRect(const Arguments& args){
		HandleScope store;
		if(args.Length()<3) return False();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return False();
		base::Rect rect;
		if(!BRectTo(args[1],&rect)) return False();
		base::String str;
		GetString(args[2],str);
		Local<Object> self = args.Holder();
		Local<Value> vFont = self->Get(String::New("font"));
		base::gdi::Font* font;
		if(StringIs(vFont,L"BFont")) font = getFont(vFont);
		else font = 0;
		HGLOBAL hFont = 0;
		if(font!=0){
			hFont = ::SelectObject(hdc,font->Handle());
		}
		TextDraw* td = getTextDraw(args);
		SetTextColor(hdc,td->color);
		SetTextAlign(hdc,td->align);
		DrawText(hdc,str,str.Length(),rect,td->align);
		if(font!=0){
			::SelectObject(hdc,hFont);
		}
		return True();
	}
	Handle<Value> BTextDraw::_calcRect(const Arguments& args){
		HandleScope store;
		if(args.Length()<2) return Undefined();
		HDC hdc = (HDC)(INT_PTR)args[0]->Int32Value();
		if(hdc==0) return Undefined();
		base::Rect rect(0,0,0,0);
		base::String str;
		GetString(args[1],str);
		Local<Object> self = args.Holder();
		Local<Value> vFont = self->Get(String::New("font"));
		base::gdi::Font* font;
		if(StringIs(vFont,L"BFont")) font = getFont(vFont);
		else font = 0;
		HGLOBAL hFont = 0;
		if(font!=0){
			hFont = ::SelectObject(hdc,font->Handle());
		}
		TextDraw* td = getTextDraw(args);
		SetTextColor(hdc,td->color);
		SetTextAlign(hdc,td->align);
		DrawText(hdc,str,str.Length(),rect,td->align|DT_CALCRECT);
		if(font!=0){
			::SelectObject(hdc,hFont);
		}
		Handle<Value> vRect = runJSCode(L"new BRect()");
		BRectFrom(vRect,&rect);
		return store.Close(vRect);
	}
	Handle<Value> BTextDraw::_get(Local<String> property,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		return store.Close(self->GetHiddenValue(property));
	}
	void BTextDraw::_set(Local<String> property, Local<Value> value,const AccessorInfo& info){
		HandleScope store;
		Local<Object> self = info.Holder();
		BTextDraw* obj = (BTextDraw*)self->GetPointerFromInternalField(0);
		if(StringIs(property,L"color")){
			if(!StringIs(value,L"BColor")) return;
		}else if(StringIs(property,L"align")){
			if(!value->IsNumber()) return;
			obj->_WrapObj->_modifyed = 1;
		}else if(StringIs(property,L"font")){
			if(!StringIs(value,L"BFont")) return;
			obj->_WrapObj->_modifyed = 1;
		}
		self->SetHiddenValue(property,value);
	}
	void BTextDraw::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BTextDraw";
		SETFUNC(draw,BTextDraw);
		SETFUNC(drawRect,BTextDraw);
		SETFUNC(calcRect,BTextDraw);
		SETACCESSOR(L"align",BTextDraw);
		SETACCESSOR(L"font",BTextDraw);
		SETACCESSOR(L"color",BTextDraw);
		SETCONST2(ALIGN_BOTTOM,DT_BOTTOM);
		SETCONST2(ALIGN_VCENTER,DT_VCENTER);
		SETCONST2(ALIGN_LEFT,DT_LEFT);
		SETCONST2(ALIGN_TOP,DT_TOP);
		SETCONST2(ALIGN_RIGHT,DT_RIGHT);
		SETCONST2(ALIGN_CENTER,DT_CENTER);
		SETCONST2(STYLE_WORDBREAK,DT_WORDBREAK);
	}
	//BImage
	_BImage::_BImage(){
		//CPPObj = new base::gdi::DrawBitmap;
	}
	_BImage::~_BImage(){
		if(CPPObj) delete (base::gdi::DrawBitmap*)CPPObj;
	}
	void _BImage::Initialize(){
	}
	base::gdi::DrawBitmap* getImage(Handle<Value> val){
		HandleScope store;
		BImage* obj = (BImage*)val->ToObject()->GetPointerFromInternalField(0);
		return (base::gdi::DrawBitmap*)obj->_WrapObj->CPPObj;
	}
	Handle<Value> BImage::_save(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		base::String file;
		GetString(args[0],file);
		int quality;
		if(args.Length()>1) quality = args[1]->Int32Value();
		else quality = 100;
		base::gdi::Image img;
		if(!img.Create(db->GetBitmap())) return False();
		bool r = img.Save(file,quality);
		return Bool(r);
	}
	Handle<Value> BImage::_load(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		base::String file;
		GetString(args[0],file);
		base::gdi::Image img;
		if(!img.Create(file)) return False();
		bool r = db->Create(img.GetBitmap());
		return Bool(r);
	}
	Handle<Value> BImage::_getSize(const Arguments& args){
		HandleScope store;
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		base::Twin sz = db->Size();
		base::String js;
		js.Format(L"new BTwin(%d,%d)",sz.x,sz.y);
		return store.Close(runJSCode(js));
	}
	Handle<Value> BImage::_setSize(const Arguments& args){
		HandleScope store;
		if(args.Length()<2) return False();
		int cx,cy;
		cx = args[0]->Int32Value();
		cy = args[1]->Int32Value();
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		bool r = db->Resize(cx,cy);
		return Bool(r);
	}
	Handle<Value> BImage::_create(const Arguments& args){
		HandleScope store;
		if(args.Length()<2) return False();
		int cx,cy;
		cx = args[0]->Int32Value();
		cy = args[1]->Int32Value();
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		base::gdi::Bitmap bmp;
		bmp.CreateCompatibleBitmap(cx,cy);
		bool r = db->Create(bmp.Detach());
		return Bool(r);
	}
	Handle<Value> BImage::_fromBitmap(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		if(!StringIs(args[0],L"BBitmap")) return False();
		Local<Object> vImg = args[0]->ToObject();
		BBitmap* iObj = (BBitmap*)vImg->GetPointerFromInternalField(0);
		base::gdi::Bitmap* idb = (base::gdi::Bitmap*)iObj->_WrapObj->CPPObj;
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		bool r = db->Create(idb->Detach());
		return Bool(r);
	}
	Handle<Value> BImage::_clip(const Arguments& args){
		HandleScope store;
		if(args.Length()==0) return True();
		int x,y,cx,cy;
		if(args.Length()>0) cx = args[0]->Int32Value();
		else cx = 0;
		if(args.Length()>1) cy = args[1]->Int32Value();
		else cy = 0;
		if(args.Length()>2) x = args[2]->Int32Value();
		else x = 0;
		if(args.Length()>3) y = args[3]->Int32Value();
		else y = 0;
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		HDC hdc = *db;
		HBITMAP hbmp = (HBITMAP)::SelectObject(hdc,0);
		base::gdi::Bitmap bmp;
		bmp.Attach(hbmp);
		bool r = bmp.ClipBitmap(cx,cy,x,y);
		::SelectObject(hdc,bmp.Detach());
		return Bool(r);
	}
	Handle<Value> BImage::_fromIcon(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		if(!StringIs(args[0],L"BIcon")) return False();
		base::gdi::Icon* idb = getIcon(args[0]);
		base::gdi::Bitmap bmp;
		if(!bmp.FromIcon(*idb)) return False();
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		bool r = db->Create(bmp.Detach());
		return Bool(r);
	}
	Handle<Value> BImage::_fromImage(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		if(!StringIs(args[0],L"BImage")) return False();
		Local<Object> vImg = args[0]->ToObject();
		BImage* iObj = (BImage*)vImg->GetPointerFromInternalField(0);
		base::gdi::DrawBitmap* idb = (base::gdi::DrawBitmap*)iObj->_WrapObj->CPPObj;
		base::gdi::DrawBitmap* db = getImage(args.Holder());

		bool r = db->Create(idb->GetBitmap());
		return Bool(r);
	}
	Handle<Value> BImage::_getPixelArray(const Arguments& args){
		HandleScope store;
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		HDC hdc = db->Handle();
		HBITMAP hbmp = (HBITMAP)::SelectObject(hdc,0);
		base::gdi::Bitmap bmp;
		bmp.Attach(hbmp);
		base::Memory<byte> p;
		p.SetUseSysMemory(1);
		bmp.GetPixelData(&p,32);
		bmp.Detach();
		::SelectObject(hdc,hbmp);
		uint len = p.Length()/4;
		Local<Array> a = Array::New(len);
		for(uint i=0,j=0;j<len;i+=4,j++){
			uint v = p[i]+(p[i+1]<<8)+(p[i+2]<<16)+(p[i+3]<<24);
			a->Set(j,Uint32::New(v));
		}
		return a;
	}
	Handle<Value> BImage::_setPixelArray(const Arguments& args){
		HandleScope store;
		if(args.Length()<1||!args[0]->IsArray()) return False();
		Local<Array> a = Local<Array>::Cast(args[0]);
		base::Memory<byte> p;
		p.SetUseSysMemory(1);
		uint len = a->Length();
		p.SetLength(len*4);
		for(uint i=0,pi=0;i<len;i++,pi+=4){
			uint c = a->Get(i)->Uint32Value();
			p[pi+3] = (c>>24);
			p[pi+2] = (c>>16)&0xFF;
			p[pi+1] = (c>>8)&0xFF;
			p[pi] = c&0xFF;
		}
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		HDC hdc = db->Handle();
		HBITMAP hbmp = (HBITMAP)::SelectObject(hdc,0);
		base::gdi::Bitmap bmp;
		bmp.Attach(hbmp);
		bool r = 0!=bmp.SetPixelData(&p,32);
		bmp.Detach();
		::SelectObject(hdc,hbmp);
		return Bool(r);
	}
	Handle<Value> BImage::_draw(const Arguments& args){
		HandleScope store;
		HDC hdc;
		int x,y,cx,cy,sx,sy,scx,scy;
		if(args.Length()<1) return False();
		hdc = (HDC)(INT_PTR)args[0]->Uint32Value();
		if(args.Length()>1) x = args[1]->Int32Value();
		else x = 0;
		if(args.Length()>2) y = args[2]->Int32Value();
		else y = 0;
		if(args.Length()>3) cx = args[3]->Int32Value();
		else cx = 0;
		if(args.Length()>4) cy = args[4]->Int32Value();
		else cy = 0;
		if(args.Length()>5) sx = args[5]->Int32Value();
		else sx = 0;
		if(args.Length()>6) sy = args[6]->Int32Value();
		else sy = 0;
		if(args.Length()>7) scx = args[7]->Int32Value();
		else scx = 0;
		if(args.Length()>8) scy = args[8]->Int32Value();
		else scy = 0;
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		bool r = db->Draw(hdc,x,y,cx,cy,sx,sy,scx,scy);
		return Bool(r);
	}
	Handle<Value> BImage::_alphaDraw(const Arguments& args){
		HandleScope store;
		HDC hdc;
		BYTE alpha;
		bool aData;
		int x,y,cx,cy,sx,sy,scx,scy;
		if(args.Length()<1) return False();
		hdc = (HDC)(INT_PTR)args[0]->Uint32Value();

		if(args.Length()>1) alpha = (BYTE)args[1]->Int32Value();
		else alpha = 255;
		if(args.Length()>2) aData = args[2]->BooleanValue();
		else aData = false;
		if(args.Length()>3) x = args[3]->Int32Value();
		else x = 0;
		if(args.Length()>4) y = args[4]->Int32Value();
		else y = 0;
		if(args.Length()>5) cx = args[5]->Int32Value();
		else cx = 0;
		if(args.Length()>6) cy = args[6]->Int32Value();
		else cy = 0;
		if(args.Length()>7) sx = args[7]->Int32Value();
		else sx = 0;
		if(args.Length()>8) sy = args[8]->Int32Value();
		else sy = 0;
		if(args.Length()>9) scx = args[9]->Int32Value();
		else scx = 0;
		if(args.Length()>10) scy = args[10]->Int32Value();
		else scy = 0;
		base::gdi::DrawBitmap* db = getImage(args.Holder());
		bool r = db->DrawAlpha(hdc,alpha,aData,x,y,cx,cy,sx,sy,scx,scy);
		return Bool(r);
	}
	void BImage::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BImage";
		SETFUNC(save,BImage);
		SETFUNC(load,BImage);
		SETFUNC(getSize,BImage);
		SETFUNC(setSize,BImage);
		SETFUNC(create,BImage);
		SETFUNC(fromBitmap,BImage);
		SETFUNC(fromIcon,BImage);
		SETFUNC(fromImage,BImage);
		SETFUNC(clip,BImage);
		SETFUNC(getPixelArray,BImage);
		SETFUNC(setPixelArray,BImage);
		SETFUNC(draw,BImage);
		SETFUNC(alphaDraw,BImage);
	}
	//BIcon
	_BIcon::_BIcon(){
		//CPPObj = new base::gdi::Icon;
	}
	_BIcon::~_BIcon(){
		if(CPPObj) delete (base::gdi::Icon*)CPPObj;
	}
	void _BIcon::Initialize(){}
	Handle<Value> BIcon::_load(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::Icon* b = getIcon(args.Holder());
		base::String file;
		GetString(args[0],file);
		int cx,cy;
		cx = GETARGSINT(1);
		cy = GETARGSINT(2);
		bool r = b->LoadFile(file,cx,cy);
		return Bool(r);
	}
	Handle<Value> BIcon::_draw(const Arguments& args){
		HandleScope store;
		if(args.Length()<3) return False();
		base::gdi::Icon* b = getIcon(args.Holder());
		HDC hdc = GetArgs<HDC>(args[0]);
		int x,y,cx,cy;
		x = GETARGSINT(1);
		y = GETARGSINT(2);
		cx = GETARGSINT(3);
		cy = GETARGSINT(4);
		bool r = b->Draw(hdc,x,y,cx,cy);
		return Bool(r);
	}
	Handle<Value> BIcon::_size(const Arguments& args){
		HandleScope store;
		base::gdi::Icon* b = getIcon(args.Holder());
		base::Twin sz = b->Size();
		Handle<Value> btw = runJSCode(L"new BTwin();");
		BTwinFrom(btw,&sz);
		return store.Close(btw);
	}
	Handle<Value> BIcon::_toBlackWhite(const Arguments& args){
		HandleScope store;
		base::gdi::Icon* b = getIcon(args.Holder());
		bool r = base::gdi::IconToBlackWhite(b);
		return Bool(r);
	}
	Handle<Value> BIcon::_fromIcon(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::Icon* b = getIcon(args.Holder());
		if(!StringIs(args[0],L"BIcon")) return False();
		base::gdi::Icon* b2 = getIcon(args[0]);
		b->Attach(b2->GetCopy());
		return True();
	}
	Handle<Value> BIcon::_fromBitmap(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::Icon* b = getIcon(args.Holder());
		if(!StringIs(args[0],L"BBitmap")) return False();
		base::gdi::Bitmap* bmp = getBitmap(args[0]);
		int alpha,cx,cy,x,y,hotX,hotY;
		alpha = GETARGSINT(1);
		cx = GETARGSINT(2);
		cy = GETARGSINT(3);
		x = GETARGSINT(4);
		y = GETARGSINT(5);
		hotX = GETARGSINT(6);
		hotY = GETARGSINT(7);
		bool r = b->FromBitmap(*bmp,alpha,cx,cy,x,y,hotX,hotY);
		return Bool(r);
	}
	Handle<Value> BIcon::_fromImage(const Arguments& args){
		HandleScope store;
		if(args.Length()<1) return False();
		base::gdi::Icon* b = getIcon(args.Holder());
		if(!StringIs(args[0],L"BImage")) return False();
		base::gdi::DrawBitmap* db = getImage(args[0]);
		HBITMAP hbmp = (HBITMAP)::SelectObject(*db,0);
		int alpha,cx,cy,x,y,hotX,hotY;
		alpha = GETARGSINT(1);
		cx = GETARGSINT(2);
		cy = GETARGSINT(3);
		x = GETARGSINT(4);
		y = GETARGSINT(5);
		hotX = GETARGSINT(6);
		hotY = GETARGSINT(7);
		bool r = b->FromBitmap(hbmp,alpha,cx,cy,x,y,hotX,hotY);
		::SelectObject(*db,hbmp);
		return Bool(r);
	}
	void BIcon::_onLoad(v8::Handle<ObjectTemplate>& ot_func,v8::Handle<ObjectTemplate>& ot_inst,base::String& name,v8::Handle<FunctionTemplate>& objcreate){
		HandleScope store;
		name = L"BIcon";
		SETFUNC(load,BIcon);
		SETFUNC(fromIcon,BIcon);
		SETFUNC(fromImage,BIcon);
		SETFUNC(fromBitmap,BIcon);
		SETFUNC(draw,BIcon);
		SETFUNC(size,BIcon);
		SETFUNC(toBlackWhite,BIcon);
	}
	//
	void LoadGDIToJS(v8::Handle<Object>& glb){
		HandleScope store;
		BFont::LoadToJS(glb);
		BBitmap::LoadToJS(glb);
		BImage::LoadToJS(glb);
		BIcon::LoadToJS(glb);
		BTextDraw::LoadToJS(glb);
		BRgn::LoadToJS(glb);
		BPen::LoadToJS(glb);
		BBrush::LoadToJS(glb);
	}
}