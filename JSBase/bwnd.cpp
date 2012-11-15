#include "stdafx.h"
#include "innerv8.h"
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")

namespace v8{
	//interface WndPtr
	bool WndPtr::GetRect(base::Rect& r){
		if(!::GetWindowRect(_Handle,r)) return false;
		HWND hp = ::GetParent(_Handle);
		if(hp)
		{
			base::Twin p(0,0);
			::ScreenToClient(hp,p);
			r.OffsetLocation(p.x,p.y);
		}
		return 1;
	}
	bool WndPtr::GetClientSize(base::Twin16& c)
	{
		RECT cr;
		if(!GetClientRect(_Handle,&cr)) return false;
		c.x = (short)cr.right;
		c.y = (short)cr.bottom;
		return true;
	}
	void WndPtr::Offset(int x, int y, int cx, int cy)//a window's size can't be set to a negativ number.
	{
		base::Rect mRect;
		if(!GetRect(mRect)) return;
		if(!(cx|cy))//cx=0,cy=0
		{
			if(x|y)//x,y之一不为0;
			{
				::SetWindowPos(_Handle,0,mRect.left+x,mRect.top+y,0,0,SWP_NOZORDER|SWP_NOSIZE);//
			}
		}
		else 
		{
			if(x|y)//x,y之一不为0;
			{
				::SetWindowPos(_Handle,0,mRect.left+x,mRect.top+y,mRect.Width()+cx,mRect.Height()+cy,SWP_NOZORDER);
			}
			else
			{
				::SetWindowPos(_Handle,0,0,0,mRect.Width()+cx,mRect.Height()+cy,SWP_NOZORDER|SWP_NOMOVE);
			}
		}
	}
	void WndPtr::ModifyStyle(DWORD dwRemove,DWORD dwAdd)
	{
		DWORD dwStyle = ::GetWindowLong(_Handle,GWL_STYLE);
		DWORD dwNewStyle = (dwStyle&~dwRemove)|dwAdd;
		if(dwStyle==dwNewStyle) return;
		::SetWindowLong(_Handle,GWL_STYLE,dwNewStyle);
		::SetWindowPos(_Handle,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED|SWP_DRAWFRAME);//
	}
	void WndPtr::ModifyExStyle(DWORD dwRemove,DWORD dwAdd)
	{
		DWORD dwStyle = ::GetWindowLong(_Handle,GWL_EXSTYLE);
		DWORD dwNewStyle = (dwStyle&~dwRemove)|dwAdd;
		if(dwStyle==dwNewStyle) return;
		::SetWindowLong(_Handle,GWL_EXSTYLE,dwNewStyle);
		::SetWindowPos(_Handle,NULL,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_FRAMECHANGED|SWP_DRAWFRAME);//
	}
	bool WndPtr::SetWidth(int cx)
	{
		base::Rect r;
		if(!GetRect(r)) return 0;
		return SetSize(cx,r.Height());
	}
	bool WndPtr::SetHeight(int cy)
	{
		base::Rect r;
		if(!GetRect(r)) return 0;
		return SetSize(r.Width(),cy);
	}
	bool WndPtr::SetXLocation(int x)
	{
		base::Rect r;
		if(!GetRect(r)) return 0;
		return SetLocation(x,r.top);
	}
	bool WndPtr::SetYLocation(int y){
		base::Rect r;
		if(!GetRect(r)) return 0;
		return SetLocation(r.left,y);
	}
	int WndPtr::GetText(base::String& text){
		uint len = ::GetWindowTextLengthW(_Handle);
		if(len>text.Cubage()) text.SetCubage(len);
		len = ::GetWindowTextW(_Handle,text,text.Cubage()+1);
		text.Realize();
		return len;
	}
	IWnd* GetWindowByHandle(HWND hWnd)
	{
		return (IWnd*)::SendMessage(hWnd,WM_GETWINDOW,0,0);
	}
	//class IWnd
	IWnd::IWnd(void):_FontColor(0),_Scr(0,0),_MinSize(0,0),_mLeave(true),_Cursor((HICON)INVALID_HANDLE_VALUE){
		Param.New();
		_BkgndColor.Create(RGB(236,233,216));
		_DragMinSize.SetValue(0,0);
		_DragMaxSize.SetValue(0xffff,0xffff);
		_Font.Create();
		_QuondamProc = (LONG_PTR)::DefWindowProc;
		_WndPointer.Bind(this,&IWnd::_WndProc);
	}
	IWnd::~IWnd(){
		::DestroyWindow(_Handle);
	}
	base::Delegate<Msg*>* IWnd::GetMsgHook(UINT msg){
		return GetMsgHook(msg,msg);
	}
	base::Delegate<Msg*>* IWnd::GetMsgHook(UINT msg1,UINT msg2){
		bool exist;
		_UPOINT up;
		up.x = msg1;
		up.y = msg2;
		int i = _MsgList.Add(up,-1,&exist);
		base::Delegate<Msg*>* d;
		if(exist){
			d = &_ProcList[i];
		}else{
			d = _ProcList.Add();
			//_FuncList.Add();
		}
		return d;
	}
	LRESULT IWnd::_WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam){
		if(!_Handle) _Handle = hWnd;

		Msg msg;
		msg.hWnd	= hWnd;
		msg.uMsg	= uMsg;
		msg.wParam	= wParam;
		msg.lParam	= lParam;

		for(uint i=0;i<_MsgList.Count();i++){
			if((uMsg>=_MsgList[i].x)&&(uMsg<=_MsgList[i].y)){
				_ProcList[i].Call(&msg);
			}
		}
		if(msg.Result._DisableWndproc()) goto return_code;
		if((uMsg>=WM_MOUSEFIRST)&&(uMsg<=WM_MOUSELAST)){//处理鼠标消息.
			if(_mLeave){
				_mLeave = false;
				TRACKMOUSEEVENT tme;
				tme.cbSize = sizeof(TRACKMOUSEEVENT);
				tme.dwFlags = TME_LEAVE;
				tme.dwHoverTime = 0xFFFFFFFF;
				tme.hwndTrack = hWnd;
				if(!TrackMouseEvent(&tme)) _mLeave = true;
			}
			if((_Cursor!=INVALID_HANDLE_VALUE)&&(uMsg==WM_MOUSEMOVE)) ::SetCursor(_Cursor);
			Mouse mouse;
			memcpy(&mouse,&uMsg,12);
			mouse.Msg = &msg;
			OnMouse.Call(&mouse);
		}else if(((uMsg>=WM_NCMOUSEMOVE)&&(uMsg<=WM_NCMBUTTONDBLCLK))||(uMsg==WM_NCMOUSELEAVE)){
			NcMouse nm;
			nm.uMsg = uMsg;
			nm.hitTest = wParam;
			nm.x = LOWORD(lParam);
			nm.y = HIWORD(lParam);
			nm.lpResult = &msg.Result;
			OnNcMouse.Call(&nm);
		}else if((uMsg>=WM_CTLCOLORMSGBOX)&&(uMsg<=WM_CTLCOLORSTATIC)){//控件的颜色消息,转给它们自己处理.
			if((HWND)lParam!=hWnd){
				CtrlMsg cm;
				cm.pResult = &msg.Result;
				cm.hWnd = (HWND)lParam;
				cm.uMsg = WM_CTRLCOLOR;
				cm.wParam = wParam;
				cm.lParam = lParam;
				//如果响应这个消息的函数返回一个非零值,则这个值会被作为返回值,否则返回是由原窗口函数返回.
				::SendMessage((HWND)lParam,WM_CTRLMSG,(WPARAM)0,(LPARAM)&cm);
			}else{//控件自己不会接收到WM_CTLCOLOR类的消息，所以下面的代码一般不会执行到
				return (HRESULT)_BkgndColor.Handle();
			}
		}else if(uMsg==WM_COMMAND){//把控件的WM_COMMAND消息转发给它自己.但是父窗口仍然可以在后面取得这个消息.
			if((lParam!=0)&&((HWND)lParam!=hWnd)){
				CtrlMsg cm;
				cm.pResult = &msg.Result;
				cm.hWnd = (HWND)lParam;
				cm.uMsg = WM_COMMAND;
				cm.wParam = wParam;
				cm.lParam = lParam;
				::SendMessage((HWND)lParam,WM_CTRLMSG,(WPARAM)0,(LPARAM)&cm);
			}
			OnCommand.Call(LOWORD(wParam));
		}else if(uMsg==WM_CREATE){
			OnCreate.Call();
		}else if(uMsg==WM_NOTIFY){//与WM_COMMAND一样,这两个消息往往是控件自己的消息,并不需要主窗口处理.
			if(((LPNMHDR)lParam)->hwndFrom!=hWnd){
				CtrlMsg cm;
				cm.pResult = &msg.Result;
				cm.hWnd = ((LPNMHDR)lParam)->hwndFrom;
				cm.uMsg = WM_NOTIFY;
				cm.wParam = wParam;
				cm.lParam = lParam;
				::SendMessage(((LPNMHDR)lParam)->hwndFrom,WM_CTRLMSG,(WPARAM)0,(LPARAM)&cm);
			}
		}else if(uMsg==WM_GETWINDOW){//响应GetWindowByHandle函数.
			msg.Result = (INT_PTR)this;
		}else if(uMsg==WM_NCDESTROY){
			_Handle = 0;
		}else if(uMsg==WM_MOUSELEAVE){
			_mLeave = true;
			Mouse mouse = *((Mouse*)&uMsg);
			OnMouse.Call(&mouse);
		}else if(uMsg==WM_MOVE){//这几乎是唯一取得客户区位置的方式.(NC类的消息了也有）
			_ClientLocation.SetValue(LOWORD(lParam),HIWORD(lParam));
		}else if(uMsg==WM_GETMINMAXINFO){
			LPMINMAXINFO pmi = (LPMINMAXINFO)lParam;

			base::Rect wr;
			base::Twin16 cs;
			base::Twin msz;

			GetRect(wr);
			GetClientSize(cs);

			msz = _DragMinSize;
			msz.x += wr.Width() - cs.x;
			msz.y += wr.Height() - cs.y;
			if(msz.x>pmi->ptMinTrackSize.x) pmi->ptMinTrackSize.x = msz.x;
			if(msz.y>pmi->ptMinTrackSize.y) pmi->ptMinTrackSize.y = msz.y;

			msz = _DragMaxSize;
			msz.x += wr.Width() - cs.x;
			msz.y += wr.Height() - cs.y;
			if(msz.x<pmi->ptMaxTrackSize.x) pmi->ptMaxTrackSize.x = msz.x;
			if(msz.y<pmi->ptMaxTrackSize.y) pmi->ptMaxTrackSize.y = msz.y;
		}else if(uMsg==WM_CLOSE){
			bool cancel = false;
			OnClose(cancel);
			if(cancel) msg.Result = 0;
		}
return_code:
		if(msg.Result.Default())
			return ((WNDPROC)_QuondamProc)(hWnd,uMsg,wParam,lParam);
		else
			return msg.Result.Handle();
	}
	bool IWnd::SetFont(bool reDraw)
	{
		_Font.Create();
		return ::SendMessage(_Handle,WM_SETFONT,(WPARAM)_Font.Handle(),reDraw)!=0;
	}
	void IWnd::SetBkgndColor(COLORREF color)
	{
		if(0xff000000&color)
			_BkgndColor.Delete();
		else
			_BkgndColor.Create(color);
	}
	void IWnd::SetFontColor(COLORREF color)
	{
		_FontColor = color;
	}
	//Class View
	View** GetAfxWindow(){
		static __declspec(thread) View* pView = 0;
		return &pView;
	}
	LRESULT __stdcall WindowProcedure(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		View* createWnd = *GetAfxWindow();//因为消息会出现嵌套，所以，可能出现这个函数的递归调用，
		if(createWnd==0) //检测createWnd是否被正确的赋值，除非错误的没有设置createWnd的值，正确的调用，这种情况实际上不会出现。
			return ::DefWindowProc(hWnd,msg,wParam,lParam);
		*GetAfxWindow() = 0;//第一时间把实例的窗口句柄赋值。
		createWnd->Attach(hWnd);//改变窗口的消息函数。
		return createWnd->_WndProc(hWnd,msg,wParam,lParam);//不遗漏消息，实际上，在重新设置了窗口的消息函数之后，后面的消息将不会调用这个函数。
	}
	ATOM RegisterWindowClass(base::String& className,UINT style = CS_DBLCLKS)
	{
		WNDCLASSEXW wcex;
		wcex.lpszClassName	= className;
		wcex.cbSize			= sizeof(WNDCLASSEX); 
		wcex.style			= style;
		wcex.lpfnWndProc	= (WNDPROC)&WindowProcedure;
		wcex.cbClsExtra		= 0;
		wcex.cbWndExtra		= 4;
		wcex.hInstance	    = (HINSTANCE)::GetModuleHandle(0);
		wcex.hIcon			= NULL;
		wcex.hCursor		= NULL;
		wcex.hbrBackground	= (HBRUSH)0;
		wcex.lpszMenuName	= NULL;
		wcex.hIconSm		= NULL;
		return RegisterClassExW(&wcex);
	}
	View::View():_BkAlpha(255),_BkMode(0x3){
		_Cursor = ::LoadCursor(0,IDC_ARROW);
		Param->Style |= WS_TABSTOP|WS_CHILD;
		GetMsgHook(WM_PAINT)->Add(this,&View::_DrawProc);
		GetMsgHook(WM_ERASEBKGND)->Add(this,&View::_EraseBkgndProc);
		GetMsgHook(WM_SIZE)->Add(this,&View::_CtrlSizeProc);
		GetMsgHook(WM_DROPFILES)->Add(this,&View::_DropFilesProc);
	}
	View::~View(){
		GetMsgHook(WM_PAINT)->Delete(this,&View::_DrawProc);
		GetMsgHook(WM_ERASEBKGND)->Delete(this,&View::_EraseBkgndProc);
		GetMsgHook(WM_SIZE)->Delete(this,&View::_CtrlSizeProc);
		GetMsgHook(WM_DROPFILES)->Delete(this,&View::_DropFilesProc);
	}
	INT_PTR View::Create(HWND hParent){
		if(_Handle) return 0;
		base::String bn;
		Param->ClassName = L"Form.View.";
		bn.Format(L"%X",(INT_PTR)&bn);//在DLL和EXE中，因为有各自的窗口函数，必须区分开，
		//否则后者注册失败后，将会使用前一个窗口类和窗口函数，就会发生混乱。
		Param->ClassName += bn;
		Param->ClassName += L".";
		bn.Format(L"%X",Param->ClassStyle);
		Param->ClassName += bn;
		RegisterWindowClass(Param->ClassName,Param->ClassStyle);
		View** pv = GetAfxWindow();
		*pv = this;
		::CreateWindowEx(Param->ExStyle,Param->ClassName,Param->Title,Param->Style,Param->Location.x,Param->Location.y,Param->Size.x,Param->Size.y,hParent,Param->Menu,::GetModuleHandle(0),this/*Param->lParam*/);
		if(_Handle==0){
			base::Warning(L"View.Create");
			return 0;
		}
		return (INT_PTR)_Handle;
	}
	bool View::Attach(HWND handle)
	{
		if(IsWindow(handle)==0){
			base::Warning(L"要绑定的句柄不是一个窗口句柄.");
			return 0;
		}
		_QuondamProc = (LONG_PTR)::DefWindowProc;
		_Handle = handle;
		this->SetWindowParam(GWL_WNDPROC,(LONG_PTR)_WndPointer.Procedure());
		return 1;
	}
	HWND View::Detach()
	{
		if(_Handle==0) return 0;
		SetWindowParam(GWL_WNDPROC,_QuondamProc);
		HWND hWnd = _Handle;
		_Handle = 0;
		return hWnd;
	}
	void View::SetCursor(HCURSOR cursor)
	{
		_Cursor = cursor;
		::SetCursor(_Cursor);
	}
	void View::SetCursor(base::ResID resource,HINSTANCE hInst)
	{
		_Cursor = ::LoadCursor(hInst,resource);
		::SetCursor(_Cursor);
	}
	void View::_DoubleDraw(base::gdi::DC* hdc)
	{
		base::Rect rect;
		::GetClientRect(_Handle,rect);
		base::gdi::Bitmap bmp;
		bmp.CreateCompatibleBitmap(rect.Width(),rect.Height());
		base::gdi::DC dc;
		dc.CreateCompatible(0);
		bmp.Select(dc);
		if(_BkgndColor.NotNull()&&(BK_COLOR&_BkMode))
		{
			::FillRect(dc,rect,_BkgndColor);
		}
		if(_BkgndImg.NotNull()&&(BK_IMAGE&_BkMode))
		{
			base::Twin sz = _BkgndImg.Size();
			int iLeft = _Scr.x/sz.x;
			int iRight = (rect.Width()+_Scr.x)/sz.x + 1;
			int jTop = _Scr.y/sz.y;
			int jBottom = (rect.Height()+_Scr.y)/sz.y + 1;
			for(int i=iLeft;i<iRight;i++)
			{
				for(int j=jTop;j<jBottom;j++)
				{
					_BkgndImg.DrawAlpha(dc,_BkAlpha,0!=(_BkMode&BK_IMAGE_ALPHA),i*sz.x-_Scr.x,j*sz.y-_Scr.y);
				}
			}
		}
		OnDoubleDraw.Call(&dc);
		::BitBlt(hdc->Handle(),rect.left,rect.top,rect.right,rect.bottom,dc,0,0,SRCCOPY);
		bmp.Select(dc);
	}
	void View::SetBkgndColor(COLORREF color,bool reDraw)
	{
		if((color&0xFF000000)) _BkgndColor.Delete();
		else _BkgndColor.Create(color);
		if(reDraw) ::InvalidateRect(_Handle,NULL,1);
	}
	void View::SetBkgndImage(HBITMAP bmp,bool redraw)
	{
		if(bmp)
			_BkgndImg.Create(bmp);
		else 
			_BkgndImg.Delete();
		if(redraw) Invalidate();
	}
	void View::SetBkgndAlpha(byte alpha,bool redraw){
		_BkAlpha = alpha;
		if(redraw) Invalidate();
	}
	void View::SetBkgndMode(byte mode,bool redraw /* = 0 */)
	{
		_BkMode = mode;
		if(redraw) Invalidate();
	}
	void View::_DrawProc(Msg* msg){
		PAINTSTRUCT ps;
		base::gdi::DC dc;
		::BeginPaint(_Handle,&ps);
		dc.Attach(ps.hdc);
		if(OnDoubleDraw.Count()){
			_DoubleDraw(&dc);
		}else{
			OnDraw.Call(&dc);
		}
		dc.Detach();
		::EndPaint(_Handle,&ps);
		msg->Result = 0;
	}
	void View::_EraseBkgndProc(Msg* msg){
		if(OnDoubleDraw.Count()){
			OnEraseBkgnd.Call((HDC)msg->wParam);
		}else{
			base::Rect rect;
			::GetClipBox((HDC)msg->wParam,rect);
			if(_BkgndColor.NotNull()&&(BK_COLOR&_BkMode)){
				::FillRect((HDC)msg->wParam,rect,_BkgndColor);
			}
			if(_BkgndImg.NotNull()&&((BK_IMAGE|BK_IMAGE_ALPHA)&_BkMode)){
				base::Twin sz = _BkgndImg.Size();
				int iLeft = (rect.left+_Scr.x)/sz.x;
				int iRight = (rect.right+_Scr.x)/sz.x + 1;
				int jTop = (rect.top+_Scr.y)/sz.y;
				int jBottom = (rect.bottom+_Scr.y)/sz.y + 1;
				for(int i=iLeft;i<iRight;i++){
					for(int j=jTop;j<jBottom;j++){
						_BkgndImg.DrawAlpha((HDC)msg->wParam,_BkAlpha,0!=(_BkMode&BK_IMAGE_ALPHA),i*sz.x-_Scr.x,j*sz.y-_Scr.y);
					}
				}
			}
		}
		msg->Result = 1;
	}
	void View::_DropFilesProc(Msg* msg)
	{
		if(OnDropFiles.Count()==0) return;
		HDROP hd = (HDROP)msg->wParam;
		int count = ::DragQueryFile(hd,-1,0,0);
		wchar_t fn[260];
		base::StringMemList files;
		for(int i=0;i<count;i++)
		{
			::DragQueryFileW(hd,i,fn,260);
			files.Add(fn);
		}
		::DragFinish(hd);			
		OnDropFiles.Call(&files);
	}
	void View::_CtrlSizeProc(Msg* msg)
	{
		if(msg->wParam==SIZE_MINIMIZED) return;
		base::Twin16 cs = *((base::Twin16*)&msg->lParam);
		OnSize.Call(cs);
	}
	//Class ScrollWnd
	ScrollWnd::ScrollWnd(void):_vsPage(0),_hsPage(0),_vsLine(1),_hsLine(1),_ResponseKeyBoard(1),_vsMax(0), _hsMax(0),_MouseWheelScroll(WM_VSCROLL){
		GetMsgHook(WM_KEYDOWN)->Add(this,&ScrollWnd::_KeyDownProc);
		GetMsgHook(WM_SIZE)->Add(this,&ScrollWnd::_SizeProc);
		GetMsgHook(WM_MOUSEWHEEL)->Add(this,&ScrollWnd::_MouseWheelProc);
		GetMsgHook(WM_VSCROLL)->Add(this,&ScrollWnd::_VScrollProc);
		GetMsgHook(WM_HSCROLL)->Add(this,&ScrollWnd::_HScrollProc);
	}
	ScrollWnd::~ScrollWnd(){
		GetMsgHook(WM_KEYDOWN)->Delete(this,&ScrollWnd::_KeyDownProc);
		GetMsgHook(WM_SIZE)->Delete(this,&ScrollWnd::_SizeProc);
		GetMsgHook(WM_MOUSEWHEEL)->Delete(this,&ScrollWnd::_MouseWheelProc);
		GetMsgHook(WM_VSCROLL)->Delete(this,&ScrollWnd::_VScrollProc);
		GetMsgHook(WM_HSCROLL)->Delete(this,&ScrollWnd::_HScrollProc);
	}
	void ScrollWnd::_MouseWheelProc(Msg* msg)
	{
		if(_MouseWheelScroll==0) return;
		if((short)HIWORD(msg->wParam)>0) ::SendMessage(_Handle,_MouseWheelScroll,0,0);
		else ::SendMessage(_Handle,_MouseWheelScroll,1,0);
	}
	void ScrollWnd::ScrollLine(int xLine,int yLine)
	{
		if(xLine>0)
		{
			for(int i=0;i<xLine;i++)
				SendMessage(WM_HSCROLL,SB_LINERIGHT,0);
		}
		else if(xLine<0)
		{
			for(int i=0;i<-xLine;i++)
				SendMessage(WM_HSCROLL,SB_LINELEFT,0);
		}
		if(yLine>0)
		{
			for(int i=0;i<yLine;i++)
				SendMessage(WM_VSCROLL,SB_LINEDOWN,0);
		}
		else if(yLine<0)
		{
			for(int i=0;i<-yLine;i++)
				SendMessage(WM_VSCROLL,SB_LINEUP,0);
		}
	}
	void ScrollWnd::ScrollPage(int xPage,int yPage)
	{
		if(xPage>0)
		{
			for(int i=0;i<xPage;i++)
				SendMessage(WM_HSCROLL,SB_PAGERIGHT,0);
		}
		else if(xPage<0)
		{
			for(int i=0;i<-xPage;i++)
				SendMessage(WM_HSCROLL,SB_PAGELEFT,0);
		}
		if(yPage>0)
		{
			for(int i=0;i<yPage;i++)
				SendMessage(WM_VSCROLL,SB_PAGEDOWN,0);
		}
		else if(yPage<0)
		{
			for(int i=0;i<-yPage;i++)
				SendMessage(WM_VSCROLL,SB_PAGEUP,0);
		}
	}
	void ScrollWnd::ScrollEnd(int xEnd,int yEnd)
	{
		if(xEnd>0)
		{
			SendMessage(WM_HSCROLL,SB_RIGHT,0);
		}
		else if(xEnd<0)
		{
			SendMessage(WM_HSCROLL,SB_LEFT,0);
		}
		if(yEnd>0)
		{
			SendMessage(WM_VSCROLL,SB_BOTTOM,0);
		}
		else if(yEnd<0)
		{
			SendMessage(WM_VSCROLL,SB_TOP,0);
		}
	}
	void ScrollWnd::_KeyDownProc(Msg* msg)
	{
		WORD vKey = (WORD)msg->wParam;
		if(_ResponseKeyBoard==0)
			return;
		switch(vKey)
		{
		case VK_PRIOR:
			SendMessage(WM_VSCROLL,SB_PAGEUP,0);
			break;
		case VK_NEXT:
			SendMessage(WM_VSCROLL,SB_PAGEDOWN,0);
			break;
		case VK_END:
			SendMessage(WM_VSCROLL,SB_BOTTOM,0);
			break;
		case VK_HOME:
			SendMessage(WM_VSCROLL,SB_TOP,0);
			break;
		case VK_LEFT:
			SendMessage(WM_HSCROLL,SB_LINEUP,0);
			break;
		case VK_UP:
			SendMessage(WM_VSCROLL,SB_LINEUP,0);
			break;
		case VK_RIGHT:
			SendMessage(WM_HSCROLL,SB_LINEDOWN,0);
			break;
		case VK_DOWN:
			SendMessage(WM_VSCROLL,SB_LINEDOWN,0);
			break;
		}
	}
	void ScrollWnd::_SizeProc(Msg* msg)
	{
		base::Twin16 _Client;
		if(msg) _Client = *((base::Twin16*)&msg->lParam);
		else GetClientSize(_Client);
		SCROLLINFO si;
		_hsLine = _Client.x/10;
		if(_hsLine==0) _hsLine++;
		_vsLine = _Client.y/10;
		if(_vsLine==0) _vsLine++;
		si.cbSize = sizeof(SCROLLINFO);
		si.fMask = SIF_ALL;
		si.nMin = 0;
		LONG_PTR style = GetWindowParam(GWL_STYLE);
		int cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
		int cxHScroll = GetSystemMetrics(SM_CXHSCROLL);
		int cx = _Client.x + ((style&WS_VSCROLL)?cxVScroll:0);
		int cy = _Client.y + ((style&WS_HSCROLL)?cxHScroll:0);
		bool _bHScroll = cx<_MinSize.x;
		bool _bVScroll = cy<_MinSize.y;
		if(_bHScroll)
		{
			if(cy<(_MinSize.y+cxHScroll)) _bVScroll = 1;
		}
		if(_bVScroll)
		{
			if(cx<(cxVScroll+_MinSize.x)) _bHScroll = 1;
		}
		if(_bHScroll)//滚动条大小为17
		{
			_hsMax = _MinSize.x - _Client.x + 1;
			if(_hsMax<0)
				_hsMax = 0;
			if(_Scr.x>_hsMax)
			{
				ScrollWindow(_Handle,_Scr.x-_hsMax,0,NULL,NULL);
				_Scr.x = _hsMax;
			}
			si.nPage = _Client.x;
			si.nMax = _MinSize.x;
			si.nPos = _Scr.x;
			_hsPage = si.nPage - _hsLine;
			SetScrollInfo(_Handle,0,&si,TRUE);
			ShowScrollBar(_Handle,0,1);
		}
		else
		{
			ShowScrollBar(_Handle,0,0);
			if(_Scr.x)
			{
				ScrollWindow(_Handle,_Scr.x,0,NULL,NULL);
				Invalidate();
				_Scr.x = 0;
			}
			_hsMax = 0;
		}
		if(_bVScroll)//滚动条大小为17
		{
			_vsMax = _MinSize.y - _Client.y + 1;
			if(_vsMax<0)
				_vsMax = 0;
			if(_Scr.y>_vsMax)
			{
				ScrollWindow(_Handle,0,_Scr.y-_vsMax,NULL,NULL);//sometime it is necessary and other time it is must not;
				_Scr.y = _vsMax;
			}
			si.nPage = _Client.y;
			si.nMax = _MinSize.y;
			si.nPos = _Scr.y;
			_vsPage = si.nPage - _vsLine;
			SetScrollInfo(_Handle,1,&si,TRUE);
			ShowScrollBar(_Handle,1,1);
		}
		else
		{
			ShowScrollBar(_Handle,1,0);
			if(_Scr.y)
			{
				ScrollWindow(_Handle,0,_Scr.y,NULL,NULL);
				Invalidate();
				_Scr.y = 0;
			}
			_vsMax = 0;
		}
	}
	void ScrollWnd::_HScrollProc(Msg* msg)
	{
		if(msg->lParam) return;
		else ::SendMessage((HWND)msg->lParam,WM_HSCROLL,msg->wParam,0);
		UINT nSBCode = LOWORD(msg->wParam);
		UINT nPos = HIWORD(msg->wParam);
		int p = _Scr.x;
		switch(nSBCode)
		{
		case 0:
			_Scr.x -= _hsLine;
			if(_Scr.x<0)
				_Scr.x = 0;
			break;
		case 1:
			_Scr.x += _hsLine;
			if(_Scr.x>_hsMax)
				_Scr.x = _hsMax;
			break;
		case 2:
			_Scr.x -= _hsPage;
			if(_Scr.x<0)
				_Scr.x = 0;
			break;
		case 3:
			_Scr.x += _hsPage;
			if(_Scr.x>_hsMax)
				_Scr.x = _hsMax;
			break;
		case SB_THUMBTRACK:
			{
				SCROLLINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cbSize = sizeof(si);
				si.fMask = SIF_TRACKPOS;
				if(GetScrollInfo(_Handle,SB_HORZ,&si))
					_Scr.x = si.nTrackPos;
				else
					_Scr.x = nPos;
				break;
			}
		case 6:
			_Scr.x = 0;
			break;
		case 7:
			_Scr.x = _hsMax;
			break;
		}
		int scCX = p - _Scr.x;
		if(scCX)
		{
			SetScrollPos(_Handle,0,_Scr.x,1);//this auto redraw the invalidate client;
			ScrollWindow(_Handle,scCX,0,NULL,NULL);
			OnScroll.Call(scCX,0);
		}
	}
	void ScrollWnd::_VScrollProc(Msg* msg)
	{
		if(msg->lParam) return;
		else ::SendMessage((HWND)msg->lParam,WM_VSCROLL,msg->wParam,0);
		UINT nSBCode = LOWORD(msg->wParam);
		UINT nPos = HIWORD(msg->wParam);
		int p = _Scr.y;
		switch(nSBCode)
		{
		case SB_LINEUP:
			_Scr.y -= _vsLine;
			if(_Scr.y<0)
				_Scr.y = 0;
			break;
		case SB_LINEDOWN:
			_Scr.y += _vsLine;
			if(_Scr.y>_vsMax)
				_Scr.y = _vsMax;
			break;
		case SB_PAGEUP:
			_Scr.y -= _vsPage;
			if(_Scr.y<0)
				_Scr.y = 0;
			break;
		case SB_PAGEDOWN:
			_Scr.y += _vsPage;
			if(_Scr.y>_vsMax)
				_Scr.y = _vsMax;
			break;
			/*case SB_THUMBPOSITION://释放拖动.
			break;*/
		case SB_THUMBTRACK://拖动
			{
				SCROLLINFO si;
				ZeroMemory(&si, sizeof(si));
				si.cbSize = sizeof(si);
				si.fMask = SIF_TRACKPOS;
				if(GetScrollInfo(_Handle,SB_VERT,&si))
					_Scr.y = si.nTrackPos;
				else
					_Scr.y = nPos;
				break;
			}
		case SB_TOP:
			_Scr.y = 0;
			break;
		case SB_BOTTOM:
			_Scr.y = _vsMax;
			break;
		}
		int scCY = p - _Scr.y;
		if(scCY)
		{
			SetScrollPos(_Handle,1,_Scr.y,1);
			ScrollWindow(_Handle,0,scCY,NULL,NULL);
			OnScroll.Call(0,scCY);
		}
	}
	void ScrollWnd::SetMouseWheelScroll(int vScroll)
	{
		switch(vScroll)
		{
		case 0:
			_MouseWheelScroll = WM_HSCROLL;
			break;
		case 1:
			_MouseWheelScroll = WM_VSCROLL;
			break;
		case 2:
			_MouseWheelScroll = 0;
			break;
		}
	}
	void ScrollWnd::SetMinSize(int cx,int cy)
	{
		if(cx<0) cx = _MinSize.x;
		if(cy<0) cy = _MinSize.y;
		if((cx==_MinSize.x)&&(cy==_MinSize.y)) return;
		_MinSize.x = cx;
		_MinSize.y = cy;
		_SizeProc(0);
		_SizeProc(0);
	}
	void ScrollWnd::SetMinSize(base::Twin size)
	{
		SetMinSize(size.x,size.y);
	}
	void ScrollWnd::SetResponseKeyBoard(bool response)
	{
		_ResponseKeyBoard = response;
	}
	void ScrollWnd::SetMinScroll(int hsLine, int vsLine)
	{
		if((hsLine==_hsLine)&&(vsLine==_vsLine)) return;
		if(hsLine>0) _hsLine = hsLine;
		else
		{
			if((vsLine<=0)||(vsLine==_vsLine)) return;
		}
		if(vsLine>0) _vsLine = vsLine;
		else
		{
			if((hsLine<=0)||(hsLine==_hsLine)) return;
		}
		_SizeProc(0);
	}
	void ScrollWnd::TransDimension(base::Twin& pt)
	{
		pt.Offset(-_Scr.x,-_Scr.y);
	}
	void ScrollWnd::TransDimension(base::Rect& rect)
	{
		rect.OffsetLocation(-_Scr.x,-_Scr.y);
	}
	bool ScrollWnd::Invalidate(const RECT* clip,bool erase)
	{
		if(clip==NULL)
			return ::InvalidateRect(_Handle,NULL,erase)!=0;
		else
		{
			base::Rect rc;
			rc = clip;
			rc.OffsetLocation(-_Scr.x,-_Scr.y);
			return ::InvalidateRect(_Handle,rc,erase)!=0;
		}
	}
	bool ScrollWnd::Invalidate(const HRGN clip,bool erase)
	{
		if(clip==0) return ::InvalidateRgn(_Handle,0,erase)!=0;
		else
		{
			base::gdi::Rgn rgn;
			rgn = clip;
			rgn.Offset(-_Scr.x,-_Scr.y);
			return ::InvalidateRgn(_Handle,rgn,erase)!=0;
		}
	}
	void ScrollWnd::Scroll(int dx,int dy){
		if(dy){
			dy += _Scr.y;
			if(dy<0)
				dy = 0;
			if(dy>_vsMax)
				dy = _vsMax;
			SetScrollPos(_Handle,1,dy,1);
			dy <<= 16;
			dy += SB_THUMBTRACK;
			::SendMessage(_Handle,WM_VSCROLL,dy,0);
		}
		if(dx){
			dx += _Scr.x;
			if(dx<0)
				dx = 0;
			if(dx>_hsMax)
				dx = _hsMax;
			SetScrollPos(_Handle,0,dx,1);
			dx <<= 16;
			dx += SB_THUMBTRACK;
			::SendMessage(_Handle,WM_HSCROLL,dx,0);
		}
	}
	//Class Frame
	Frame::Frame(){
		Param->Style = WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS;
		Param->ExStyle |= WS_EX_CONTROLPARENT;//|WS_EX_COMPOSITED;//启用子窗口的双缓冲//这个风格回让窗口系统按钮失去鼠标滑过效果
	}
	Frame::~Frame(){
		GetMsgHook(WM_DESTROY)->Delete(this,&Frame::_DestroyProc);
	}
	INT_PTR Frame::Create(HWND hParent,int nCmdShow,bool quit){
		if(quit)
			GetMsgHook(WM_DESTROY)->Add(this,&Frame::_DestroyProc);
		INT_PTR b = View::Create(hParent);
		if(b){//对于主窗口,生成时的WS_STYLE是无效的,总是被移除.
			::ShowWindow(_Handle,nCmdShow);//SW常数不支持位组合,只是一个枚举.而WS常数支持位组合,而且大体上是可以和SW对应的.
			::UpdateWindow(_Handle);
		}
		return b;
	}
	void Frame::_DestroyProc(Msg* msg)
	{
		//PostQuitMessage必须在启动线程里调用才有效，ExitProcess可以在任何线程调用，但是它并不结束正在运行的线程。
		//也就是说ExitProcess后面的代码实际上是不会被运行的。
		::PostQuitMessage(0);
	}
	void Frame::Quit(int code){
		if(Close())
			PostQuitMessage(code);
	}
	//Window Function
/*
	//class Lable
	//class Button
	bool ButtonPtr::SetImageList(HIMAGELIST hImgl,UINT align,LPRECT margin){
		BUTTON_IMAGELIST bi;
		bi.himl = hImgl;
		bi.uAlign = align;
		if(margin)
			::SetRect(&bi.margin,margin->left,margin->top,margin->right,margin->bottom);
		else
			::SetRect(&bi.margin,1,1,1,1);
		return SendMessage(BCM_SETIMAGELIST,0,&bi)!=0;
	}
	//class MonthCalendar
	//class DateTimePicker
	//class ListBox
	int ListBoxPtr::AddString(LPCWSTR str,int index){
		UINT msgId;
		if(index<0) msgId = LB_ADDSTRING;
		else msgId = LB_INSERTSTRING;
		return (int)SendMessage(msgId,(WPARAM)index,(LPARAM)str);
	}
	int ListBoxPtr::GetText(int index,base::String& buf){
		int len = GetTextLength(index);
		buf.SetCubage(len);
		int r = (int)SendMessage(LB_GETTEXT,(WPARAM)index,(LPARAM)buf.Handle());
		buf.Realize();
		return r;
	}
	bool ListBoxPtr::SetText(int index,LPCWSTR str){
		int n = GetCount();
		if(index<0||index>=n){
			return 0;
		}
		index = AddString(str,index);
		if(index<0) return 0;
		int i = GetSelItem();
		if(i==(index+1)){
			SetSelItem(index);
		}
		DeleteString(index+1);
		return 1;
	}

	//class ComboBox
	bool ComboBoxPtr::GetItemText(int index,base::String& str){
		str = L"";
		int len = (int)SendMessage(CB_GETLBTEXTLEN,index,0);
		if(len==CB_ERR) return 0;
		if((int)str.Cubage()<len) str.SetCubage(len);
		if(SendMessage(CB_GETLBTEXT,(WPARAM)index,(LPARAM)str.Handle())==-1) 
			return 0;
		str.Realize();
		return 1;
	}
	//class ComboBoxEx
	int ComboBoxExPtr::AddItem(LPCWSTR str,int image,int indent,int index,int selImage,LPARAM lParam)
	{
		base::AutoStruct<COMBOBOXEXITEM> cbi;
		cbi.iOverlay = 0;
		cbi.cchTextMax = 0;
		cbi.mask = CBEIF_INDENT|CBEIF_TEXT;
		if(image!=-1)
		{
			cbi.mask |= CBEIF_IMAGE|CBEIF_SELECTEDIMAGE;
			cbi.iImage = image;
			if(selImage!=-1)
				cbi.iSelectedImage = selImage;
			else
				cbi.iSelectedImage = image;
		}
		cbi.iIndent = indent;
		cbi.iItem = index;
		cbi.pszText = (LPWSTR)str;
		cbi.lParam = lParam;
		return (int)SendMessage(CBEM_INSERTITEM,0,(LPARAM)&cbi);
	}
	bool ComboBoxExPtr::SetText(int index,LPCWSTR text)
	{
		base::AutoStruct<COMBOBOXEXITEM> cbi;
		cbi.mask = CBEIF_TEXT;
		cbi.pszText = (LPWSTR)text;
		cbi.iItem = index;
		return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)&cbi)!=0;
	}
	bool ComboBoxExPtr::GetText(int index,base::String& text)
	{
		base::AutoStruct<COMBOBOXEXITEM> cbi;
		cbi.mask = CBEIF_TEXT;
		cbi.pszText = text;
		cbi.cchTextMax = text.Cubage() + 1;
		cbi.iItem = index;
		if(SendMessage(CBEM_GETITEM,0,(LPARAM)&cbi)==0) return 0;
		if(cbi.pszText!=text.Handle()) text = cbi.pszText;
		text.Realize();
		return 1;
	}
	int ComboBoxExPtr::GetIndent(int index)
	{
		base::AutoStruct<COMBOBOXEXITEM> cbi;
		cbi.mask = CBEIF_INDENT;
		cbi.iItem = index;
		if(SendMessage(CBEM_GETITEM,0,(LPARAM)&cbi)==0) return 0;
		return cbi.iIndent;
	}
	bool ComboBoxExPtr::SetIndent(int index,int indent)
	{
		base::AutoStruct<COMBOBOXEXITEM> cbi;
		cbi.mask = CBEIF_INDENT;
		cbi.iIndent = indent;
		return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)&cbi)!=0;
	}
	int ComboBoxExPtr::GetImage(int index,int flag)
	{
		base::AutoStruct<COMBOBOXEXITEM> cbi;
		if((index<0)||(index>2))
			return -1;
		cbi.mask = 2 << flag;
		cbi.iItem = index;
		if(SendMessage(CBEM_GETITEM,0,(LPARAM)&cbi)==0)
			return -1;
		switch(flag)
		{
		case 0:
			return cbi.iImage;
		case 1:
			return cbi.iSelectedImage;
		case 2:
			return cbi.iOverlay;
		}
		return -1;
	}
	bool ComboBoxExPtr::SetImage(int index,int image,int flag)
	{
		base::AutoStruct<COMBOBOXEXITEM> cbi;
		if((index<0)||(index>2))
			return false;
		cbi.mask = 2 << flag;
		cbi.iItem = index;
		switch(flag)
		{
		case 0:
			cbi.iImage = image;
			break;
		case 1:
			cbi.iSelectedImage = image;
			break;
		case 2:
			cbi.iOverlay = image;
			break;
		}
		return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)&cbi)!=0;
	}
	//class TreeView
	bool TreeViewPtr::GetItemData(HTREEITEM hti,LPARAM* pParam)
	{
		base::AutoStruct<TVITEMEX> tvi;
		tvi.mask = TVIF_PARAM;
		tvi.hItem = hti;
		if(!TreeView_GetItem(_Handle,&tvi)) return 0;
		*pParam = tvi.lParam;
		return 1;
	}
	bool TreeViewPtr::SetItemData(HTREEITEM hti,LPARAM data)
	{
		base::AutoStruct<TVITEMEX> tvi;
		tvi.mask = TVIF_PARAM;
		tvi.hItem = hti;
		tvi.lParam = data;
		return TreeView_SetItem(_Handle,&tvi)!=0;
	}
	HTREEITEM TreeViewPtr::AddItem(HTREEITEM parent,LPCWSTR text,int imgIndex,int selIndex,HTREEITEM insAfter)
	{
		TVINSERTSTRUCT ti;
		::ZeroMemory(&ti,sizeof(ti));
		ti.hInsertAfter = insAfter;
		ti.hParent = parent;
		ti.itemex.mask = TVIF_IMAGE|TVIF_SELECTEDIMAGE|TVIF_TEXT;
		ti.itemex.iImage = imgIndex;
		ti.itemex.iSelectedImage = selIndex;
		ti.itemex.pszText = (LPWSTR)text;
		return TreeView_InsertItem(_Handle,&ti);
	}
	int TreeViewPtr::GetItemText(HTREEITEM hti,base::String& str)
	{
		base::AutoStruct<TVITEMEX> tvi;
		tvi.mask = TVIF_TEXT;
		tvi.hItem = hti;
		tvi.pszText = str.Handle();
		tvi.cchTextMax = str.Cubage() + 1;
		if(!TreeView_GetItem(_Handle,&tvi))
			return 0;
		str.Realize();
		return str.Length();
	}
	bool TreeViewPtr::SetItemText(HTREEITEM hti,LPCWSTR str)
	{
		base::AutoStruct<TVITEMEX> tvi;
		tvi.mask = TVIF_TEXT;
		tvi.hItem = hti;
		tvi.pszText = (LPWSTR)str;
		return TreeView_SetItem(_Handle,&tvi)!=0;
	}
	int TreeViewPtr::GetImageIndex(HTREEITEM hti,UINT flag)
	{
		base::AutoStruct<TVITEMEX> tvi;
		if(flag)
		{
			tvi.mask = TVIF_SELECTEDIMAGE;
			tvi.iSelectedImage = -1;
		}
		else
		{
			tvi.mask = TVIF_IMAGE;
			tvi.iImage = -1;
		}
		tvi.hItem = hti;
		TreeView_GetItem(_Handle,&tvi);
		if(flag) return tvi.iSelectedImage;
		else return tvi.iImage;
	}
	bool TreeViewPtr::SetImageIndex(HTREEITEM hti,int image,int selImage)
	{
		base::AutoStruct<TVITEMEX> tvi;
		if(selImage>=0)
		{
			tvi.mask |= TVIF_SELECTEDIMAGE;
			tvi.iSelectedImage = selImage;
		}
		if(image>=0)
		{
			tvi.mask |= TVIF_IMAGE;
			tvi.iImage = image;
		}
		tvi.hItem = hti;
		return TreeView_SetItem(_Handle,&tvi)!=0;
	}
	UINT ToolBarPtr::SetExStyle(UINT exStyle,DWORD opt)
	{
		if(opt==0)
		{
			return (UINT)SendMessage(TB_SETEXTENDEDSTYLE,0,exStyle);
		}
		UINT exs = (UINT)SendMessage(TB_GETEXTENDEDSTYLE,0,0);
		exs |= exStyle;
		if(opt==1) exs -= exStyle;
		return (UINT)SendMessage(TB_SETEXTENDEDSTYLE,0,exs);
	}
	UINT ToolBarPtr::SetStyle(UINT style,DWORD opt)
	{
		if(opt==2)
		{
			return (UINT)SendMessage(TB_SETSTYLE,0,style);
		}
		UINT exs = (UINT)SendMessage(TB_GETSTYLE,0,0);
		exs |= style;
		if(opt==1) exs -= style;
		return (UINT)SendMessage(TB_SETSTYLE,0,exs);
	}
	bool ToolBarPtr::AddSplit(int index)
	{
		base::AutoStruct<TBBUTTON> tb;
		tb.fsStyle = TBSTYLE_SEP;
		tb.idCommand = 0x10000;
		return (int)SendMessage(TB_INSERTBUTTON,index,(LPARAM)&tb)!=0;
	}
	bool ToolBarPtr::AddButton(WORD cmdID,int image,LPCWSTR text,BYTE style,BYTE state,int index)
	{
		base::AutoStruct<TBBUTTON> tb;
		tb.iBitmap = image;
		tb.fsState = state;
		tb.fsStyle = style;
		tb.iString = (INT_PTR)text;
		tb.idCommand = cmdID;
		tb.dwData = 0;
		return (int)SendMessage(TB_INSERTBUTTON,index,(LPARAM)&tb)!=0;
	}
	bool ToolBarPtr::SetButtonState(int id,int state,DWORD option)
	{
		int ts;
		switch(option)
		{
		case 0:
			break;
		case 1:
			state |= GetButtonState(id);
			break;
		case 2:
			ts = state;
			state |= GetButtonState(id);
			state -= ts;
			break;
		}
		return (int)SendMessage(TB_SETSTATE,id,state)!=0;
	}
	HIMAGELIST ToolBarPtr::SetImageList(int it,HIMAGELIST hImgl)
	{
		if(it==0)
			return (HIMAGELIST)SendMessage(TB_SETIMAGELIST,0,(LPARAM)hImgl);
		else if(it==1)
			return (HIMAGELIST)SendMessage(TB_SETDISABLEDIMAGELIST,0,(LPARAM)hImgl);
		else if(it==2)
			return (HIMAGELIST)SendMessage(TB_SETHOTIMAGELIST,0,(LPARAM)hImgl);
		return 0;
	}
	HIMAGELIST ToolBarPtr::GetImageList(int type)
	{
		UINT msg;
		switch(type)
		{
		case 0:
			msg = TB_GETIMAGELIST;
			break;
		case 1:
			msg = TB_GETDISABLEDIMAGELIST;
			break;
		case 2:
			msg = TB_GETHOTIMAGELIST;
			break;
		default:
			return 0;
		}
		return (HIMAGELIST)SendMessage(msg,0,0);
	}
	bool ToolBarPtr::LoadBitmap(int it,int cx,COLORREF mask,base::ResID res,HINSTANCE hInst,bool delpre)
	{
		UINT lr = LR_DEFAULTCOLOR|LR_DEFAULTSIZE|LR_CREATEDIBSECTION;
		if(hInst==0)
			lr |= LR_LOADFROMFILE;
		HIMAGELIST hImgl = ::ImageList_LoadImage(hInst,res,cx,16,mask,IMAGE_BITMAP,lr),preImgl;
		if(hImgl==0)
			return 0;
		UINT msg,get;
		switch(it)
		{
		case 0:
			msg = TB_SETIMAGELIST;
			get = TB_GETIMAGELIST;
			break;
		case 1:
			msg = TB_SETDISABLEDIMAGELIST;
			get = TB_GETDISABLEDIMAGELIST;
			break;
		case 2:
			msg = TB_SETHOTIMAGELIST;
			get = TB_GETHOTIMAGELIST;
			break;
		}
		preImgl = (HIMAGELIST)SendMessage(msg,0,(LPARAM)hImgl);
		if((preImgl)&&delpre)
			::ImageList_Destroy(preImgl);
		if(hImgl!=(HIMAGELIST)SendMessage(get,0,0))
		{
			::ImageList_Destroy(hImgl);
			return 0;
		}
		return 1;
	}
	INT_PTR ToolBarPtr::GetButtonInfo(WORD id,DWORD tbif)
	{
		base::AutoStruct<TBBUTTONINFO,true> bi;
		bi.dwMask = tbif;
		if(TBIF_TEXT&tbif)
		{
			static base::String buffer;
			if(tbif&TBIF_BYINDEX)
			{
				bi.dwMask = TBIF_COMMAND|TBIF_BYINDEX;
				if(SendMessage(TB_GETBUTTONINFO,id,&bi)==-1)
					return -1;
				id = (WORD)bi.idCommand;
			}
			uint len = (uint)SendMessage(TB_GETBUTTONTEXT,id,0);
			if(buffer.Cubage()<len)
				buffer.SetCubage(len);
			SendMessage(TB_GETBUTTONTEXT,id,buffer.Handle());
			return (INT_PTR)buffer.Handle();
		}
		if(SendMessage(TB_GETBUTTONINFO,id,&bi)==-1)
			return -1;
		tbif |= TBIF_BYINDEX;
		tbif -= TBIF_BYINDEX;
		switch(tbif)
		{
		case TBIF_COMMAND:
			return bi.idCommand;
		case TBIF_IMAGE:
			return bi.iImage;
		case TBIF_LPARAM:
			return bi.lParam;
		case TBIF_SIZE:
			return bi.cx;
		case TBIF_STATE:
			return bi.fsState;
		case TBIF_STYLE:
			return bi.fsStyle;
		}
		return -1;
	}
	bool ToolBarPtr::SetButtonInfo(WORD id,DWORD tbif,INT_PTR value)
	{
		base::AutoStruct<TBBUTTONINFO,true> bi;
		bi.dwMask = tbif;
		tbif |= TBIF_BYINDEX;
		tbif -= TBIF_BYINDEX;
		switch(tbif)
		{
		case TBIF_COMMAND:
			bi.idCommand = (int)value;
			break;
		case TBIF_IMAGE:
			bi.iImage = (int)value;
			break;
		case TBIF_LPARAM:
			bi.lParam = value;
			break;
		case TBIF_SIZE:
			bi.cx = (WORD)value;
			break;
		case TBIF_STATE:
			bi.fsState = (BYTE)value;
			break;
		case TBIF_STYLE:
			bi.fsStyle = (BYTE)value;
			break;
		case TBIF_TEXT:
			bi.pszText = (LPWSTR)value;
			break;
		default:
			return 0;
		}
		return (int)SendMessage(TB_SETBUTTONINFO,id,&bi)!=0;
	}
	bool ToolBarPtr::GetButtonRect(int index,LPRECT lpRect,bool byIndex)
	{
		if(byIndex)
			return (int)SendMessage(TB_GETITEMRECT,index,(LPARAM)lpRect)!=0;
		else
			return (int)SendMessage(TB_GETRECT,index,(LPARAM)lpRect)!=0;
	}
	//class Progress Bar
	//class ToolTip
	bool ToolTipPtr::AddTip(HWND owner,LPCWSTR text,UINT id,LPRECT pr,UINT ttf)
	{
		base::AutoStruct<TOOLINFO,1> ti;
		ti.uId = id;
		ti.hwnd = owner;
		ti.hinst = 0;
		if(pr==0)
			::GetClientRect(owner,&ti.rect);
		else
			ti.rect = *pr;
		ti.uFlags = ttf|TTF_SUBCLASS;
		ti.lpszText = (LPWSTR)text;
		return 0!=this->SendMessage(TTM_ADDTOOL,0,&ti);
	}
	void ToolTipPtr::DelTip(HWND owner,UINT uID)
	{
		base::AutoStruct<TOOLINFO,1> ti;
		ti.uId = uID;
		ti.hwnd = owner;
		this->SendMessage(TTM_DELTOOL,0,&ti);
	}
	bool ToolTipPtr::SetTipRect(LPRECT pr,HWND owner,UINT id)
	{
		base::AutoStruct<TOOLINFO,1> ti;
		ti.hwnd = owner;
		ti.uId = id;
		GetTipInfo(&ti);
		ti.rect = *pr;
		return SetTipInfo(&ti);
	}
	bool ToolTipPtr::SetTipText(LPCWSTR text,HWND owner,UINT uID)
	{
		base::AutoStruct<TOOLINFO,1> ti;
		ti.hwnd = owner;
		ti.uId = uID;
		GetTipInfo(&ti);
		ti.lpszText = (LPWSTR)text;
		return SetTipInfo(&ti);
	}
	bool ToolTipPtr::SetTipFlag(UINT ttf,HWND owner,UINT id)
	{
		base::AutoStruct<TOOLINFO,1> ti;
		ti.hwnd = owner;
		ti.uId = id;
		GetTipInfo(&ti);
		ti.uFlags = ttf|TTF_SUBCLASS;
		return SetTipInfo(&ti);
	}
	//class TabControl
	bool TabPagePtr::Add(int index,LPWSTR title,int image,LPARAM param)
	{
		TCITEM ti;
		ti.mask = TCIF_IMAGE|TCIF_PARAM|TCIF_TEXT;
		ti.iImage = image;
		ti.lParam = param;
		ti.pszText = title;
		return SendMessage(TCM_INSERTITEM,index,&ti)!=-1;
	}
	//class HotKey
	void HotKeyPtr::SetHotKey(unsigned char comb,unsigned char vk)
	{
		HotkeyParam hk;
		hk.VirtualKey = comb;
		hk.CombineKey = vk;
		SendMessage(HKM_SETHOTKEY,hk.wParam,0);
	}
	HotkeyParam HotKeyPtr::GetHotKey()
	{
		HotkeyParam hk;
		hk.wParam = (WPARAM)SendMessage(HKM_GETHOTKEY,0,0);
		return hk;
	}
	int ListViewPtr::AddColumn(LPWSTR title,int width,int index)
	{
		base::AutoStruct<LVCOLUMN> lvc;
		lvc.mask = LVCF_TEXT|LVCF_WIDTH;
		lvc.pszText = title;
		lvc.cx = width;
		int count = GetColumnCount();
		if((uint)index>(uint)count) index = count;
		return (int)this->SendMessage(LVM_INSERTCOLUMN,index,&lvc);
	}
	int ListViewPtr::AddItem(LPWSTR text,int index)
	{
		base::AutoStruct<LVITEM> lvi;
		lvi.mask = LVIF_TEXT;
		lvi.pszText = text;
		if(index<0) index = 0xfffffff;
		lvi.iItem = index;
		return (int)this->SendMessage(LVM_INSERTITEM,0,&lvi);
	}
	bool ListViewPtr::SetItemText(LPWSTR text,int index,int column)
	{
		base::AutoStruct<LVITEM> lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = column;
		lvi.pszText = text;
		return 0!=this->SendMessage(LVM_SETITEMTEXT,index,&lvi);
	}
	int ListViewPtr::GetItemText(base::String& str,int index,int column)
	{
		base::AutoStruct<LVITEM> lvi;
		lvi.mask = LVIF_TEXT;
		lvi.iSubItem = column;
		str.SetCubage(255);
		str[0] = 0;
		lvi.pszText = str.Handle();
		lvi.cchTextMax = 256;
		int n = (int)this->SendMessage(LVM_GETITEMTEXT,index,&lvi);
		str.Realize();
		return n;
	}
	int ListViewPtr::GetColumnCount()
	{
		HWND header = (HWND)this->SendMessage(LVM_GETHEADER,0,0);
		if(!header) return 0;
		return (int)::SendMessage(header,HDM_GETITEMCOUNT,0,0);
	}
	int HeaderPtr::AdjustSize(HWND parent,int x,int y,int width){
		RECT rcParent; 
		HDLAYOUT hdl; 
		WINDOWPOS wp; 
		GetClientRect(parent,&rcParent); 

		hdl.prc = &rcParent; 
		hdl.pwpos = &wp;
		if(!SendMessage(HDM_LAYOUT, 0, (LPARAM) &hdl))
			return 0; 
		if(width<0) width = wp.cx;
		SetWindowPos(_Handle,wp.hwndInsertAfter, wp.x+x, wp.y+y,width, wp.cy, wp.flags | SWP_SHOWWINDOW); 
		return wp.cy;
	}
	int HeaderPtr::AddItem(LPCWSTR text,int width,int index){
		base::AutoStruct<HDITEM> hdi; 
		hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH; 
		hdi.pszText = (LPWSTR)text; 
		hdi.cxy = width; 
		hdi.fmt = HDF_LEFT | HDF_STRING; 
		if(index<0)
			index = 10000;

		return SendMessage(HDM_INSERTITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::SetItem(int index,LPCWSTR text,int width){
		base::AutoStruct<HDITEM> hdi; 
		hdi.mask = HDI_TEXT | HDI_FORMAT | HDI_WIDTH; 
		hdi.pszText = (LPWSTR)text; 
		hdi.cxy = width; 
		hdi.fmt = HDF_LEFT | HDF_STRING; 

		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	int HeaderPtr::GetItem(int index,base::String& text){
		text.SetCubage(256);
		base::AutoStruct<HDITEM> hdi; 
		hdi.mask = HDI_TEXT | HDI_WIDTH; 
		hdi.pszText = text.Handle();
		hdi.cchTextMax = 256;
		if(!SendMessage(HDM_GETITEM,(WPARAM)index,(LPARAM)&hdi))
			return 0; 
		text.CopyFrom(hdi.pszText);
		text.Realize();
		return hdi.cxy;
	}
	bool HeaderPtr::SetItemWidth(int index,int width){
		base::AutoStruct<HDITEM> hdi; 
		hdi.mask = HDI_WIDTH; 
		hdi.cxy = width; 
		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::SetItemText(int index,LPCWSTR title){
		base::AutoStruct<HDITEM> hdi; 
		hdi.mask = HDI_TEXT; 
		hdi.pszText = (LPWSTR)title; 
		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::SetItem(int index,HDITEM* hdi){
		return 0!=SendMessage(HDM_SETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool HeaderPtr::GetItem(int index,HDITEM* hdi){
		return 0!=SendMessage(HDM_GETITEM,(WPARAM)index, (LPARAM)&hdi); 
	}
	bool Init_Ctrl60(DWORD icc)
	{
		INITCOMMONCONTROLSEX iccex; 
		iccex.dwICC = icc;
		iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
		return InitCommonControlsEx(&iccex)!=0;
	}
	//class ImageList
	ImageList::~ImageList()
	{
		::ImageList_Destroy(_Handle);
	}
	void ImageList::operator = (HIMAGELIST hImgl)
	{
		Attach(hImgl);
	}
	bool ImageList::Attach(HIMAGELIST hImgl)
	{
		if(hImgl==0) return 0;
		::ImageList_Destroy(_Handle);
		_Handle = hImgl;
		return true;
	}
	bool ImageList::Create(int cx,int cy,UINT ilt,int initial,int grow)
	{
		HIMAGELIST handle = ::ImageList_Create(cx,cy,ilt,initial,grow);
		if(handle==0) return 0;
		if(_Handle)::ImageList_Destroy(_Handle);
		_Handle = handle;
		return true;
	}
	HIMAGELIST ImageList::Detach()
	{
		HIMAGELIST hImgl = _Handle;
		_Handle = 0;
		return hImgl;
	}
	bool ImageList::Destroy()
	{
		if(_Handle==0) return 0;
		HIMAGELIST himgl = _Handle;
		_Handle = 0;
		return ::ImageList_Destroy(himgl)==1;
	}
	int ImageList::AddBitmap(HBITMAP bmp,HBITMAP mask)
	{
		int count = ImageList_Add(_Handle,bmp,mask);
		if(count<0) return 0;
		return ImageList_GetImageCount(_Handle) - count;
	}
	bool ImageList::AddIcon(HICON icon)
	{
		return ::ImageList_ReplaceIcon(_Handle,-1,icon)!=-1;
	}
	bool ImageList::Replace(HICON icon,int index)
	{
		if((index<0)||(index>=ImageList_GetImageCount(_Handle))) return false;
		return ::ImageList_ReplaceIcon(_Handle,index,icon)!=-1;
	}
	int ImageList::LoadResource(int cx,COLORREF mask,base::ResID res,HINSTANCE hInst,int grow)
	{
		UINT lr = LR_CREATEDIBSECTION|LR_DEFAULTSIZE;
		if(hInst==0) lr |= LR_LOADFROMFILE;
		HIMAGELIST handle = ::ImageList_LoadImageW(hInst,res,cx,grow,mask,IMAGE_BITMAP,lr);
		if(handle==0) return 0;
		if(_Handle) ::ImageList_Destroy(_Handle);
		_Handle = handle;
		return ImageList_GetImageCount(_Handle);
	}
	bool ImageList::Draw(HDC hdc,int index,int x,int y,UINT style)
	{
		return ::ImageList_Draw(_Handle,index,hdc,x,y,style)!=0;
	}
	bool ImageList::Copy(int iDst,int iSrc)
	{
		return ImageList_Copy(_Handle,iDst,_Handle,iSrc,ILCF_MOVE)!=0;
	}
	bool ImageList::Swap(int iDst,int iSrc)
	{
		return ImageList_Copy(_Handle,iDst,_Handle,iSrc,ILCF_SWAP)!=0;
	}
	HIMAGELIST ImageList::Duplicate()
	{
		return ::ImageList_Duplicate(_Handle);
	}
	COLORREF ImageList::GetBkColor()
	{
		return ::ImageList_GetBkColor(_Handle);
	}
	int ImageList::Count()
	{
		return ::ImageList_GetImageCount(_Handle);
	}
	base::Twin ImageList::GetImageSize()
	{
		base::Twin size(0,0);
		::ImageList_GetIconSize(_Handle,&size.x,&size.y);
		return size;
	}
	HICON ImageList::GetIcon(int index)
	{
		return ::ImageList_GetIcon(_Handle,index,0);
	}
	bool ImageList::Delete(int index)
	{
		if(index<0) return 0;
		return ::ImageList_Remove(_Handle,index)!=0;
	}//is index = -1,then remove all
	bool ImageList::Clear()
	{
		return ImageList_Remove(_Handle,-1)!=0;
	}
	COLORREF ImageList::SetBkColor(COLORREF bk)
	{
		return ::ImageList_SetBkColor(_Handle,bk);
	}
	bool ImageList::SetImageSize(int cx,int cy)
	{
		return ::ImageList_SetIconSize(_Handle,cx,cy)!=0;
	}//this clear the imagelist.
	bool ImageList::SetCount(int count)
	{
		return ::ImageList_SetImageCount(_Handle,count)!=0;
	}*/
	//class ICtrl
	//class Edit
	/*
	void Edit::_CtrlColor(CtrlMsg* pcm)
	{
		::SetTextColor((HDC)pcm->wParam,_FontColor);
		::SetBkColor((HDC)pcm->wParam,_BkgndColor.GetColor());
		*pcm->pResult = (INT_PTR)_BkgndColor.Handle();
	}
	Label::Label()
	{
		Param->ClassName = L"Static";
	}
	//class Button
	Button::Button(){
		Param->ClassName = L"Button";
	}
	GroupBox::GroupBox(){
		Param->ClassName = L"Button";
		Param->Style |= BS_GROUPBOX;
	}
	CheckBox::CheckBox(){
		Param->ClassName = L"Button";
		Param->Style |= BS_AUTOCHECKBOX;
	}
	Radio::Radio(){
		Param->ClassName = L"Button";
		Param->Style |= BS_AUTORADIOBUTTON;
	}
	//class MonthCalendar
	MonthCalendar::MonthCalendar()
	{
		Param->ClassName = L"SysMonthCal32";
	}
	//class DateTimePicker
	DateTimePicker::DateTimePicker()
	{
		Param->ClassName = L"SysDateTimePick32";
	}
	//class ListBox
	ListBox::ListBox()
	{
		Param->Style = WS_HSCROLL|WS_VSCROLL|WS_VISIBLE|LBS_HASSTRINGS;
		Param->ExStyle = WS_EX_CLIENTEDGE;
		Param->ClassName = L"ListBox";
		_BkgndColor.Create(0xffffff);
	}
	//class ComboBox
	ComboBox::ComboBox(){
		Param->ClassName = L"ComboBox";
		Param->Style = WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|CBS_AUTOHSCROLL|CBS_HASSTRINGS;
	}
	//class ComboBoxEx
	ComboBoxEx::ComboBoxEx()
	{
		Param->ClassName = L"ComboBoxEx32";
		Param->Style |= CBS_DROPDOWN|CBS_AUTOHSCROLL|WS_VISIBLE;
	}
	//class TreeView
	TreeView::TreeView()
	{
		Param->ClassName = L"SysTreeView32";
		Param->Style |= TVS_HASBUTTONS|TVS_HASLINES|TVS_INFOTIP|TVS_LINESATROOT;
		Param->ExStyle |= WS_EX_CLIENTEDGE;
		_BkgndColor.Create(0xffffff);
		//this->OnNotify.Bind(this,&TreeView::_OnNotify);
	}
	ToolBar::ToolBar()
	{
		Param->ClassName = L"ToolbarWindow32";
		Param->Style |= TBSTYLE_TRANSPARENT|TBSTYLE_LIST|WS_VISIBLE|TBSTYLE_TOOLTIPS|TBSTYLE_FLAT;
	}
	INT_PTR ToolBar::Create(HWND hParent){
		if(!ICtrl::Create(hParent)) return 0;
		SendMessage(TB_BUTTONSTRUCTSIZE,sizeof(TBBUTTON),0);
		SetExStyle(TBSTYLE_EX_HIDECLIPPEDBUTTONS,1);
		if(IWnd* wp = GetWindowByHandle(hParent)){
			wp->GetMsgHook(WM_SIZE)->Add(this,&ToolBar::_OnParentSize);
		}
		return (INT_PTR)_Handle;
	}
	void ToolBar::_OnParentSize(Msg*)
	{
		SendMessage(TB_AUTOSIZE,0,0);
	}
	//class Progress Bar
	ProgressBar::ProgressBar()
	{
		Param->ClassName = L"msctls_progress32";
	}
	//class ToolTip
	ToolTip::ToolTip(void)
	{
		Param->ClassName = L"tooltips_class32";
		Param->ExStyle = WS_EX_TOPMOST;
		Param->Style = WS_POPUP|TTS_NOPREFIX|TTS_ALWAYSTIP;//|TTS::BALLOON;
	}

	INT_PTR ToolTip::Create(HWND hParent)
	{
		if(!ICtrl::Create(hParent)) return 0;
		::SetWindowPos(_Handle, HWND_TOPMOST,0, 0, 0, 0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		return (INT_PTR)_Handle;
	}
	//class Tab_Ctrl
	TabPage::TabPage()
	{
		Param->ClassName = L"SysTabControl32";
		//OnNotify.Bind(this,&TabPage::_OnNotify);
	}
	//class HotKey
	HotKey::HotKey()
	{
		this->Param->Size.SetValue(146,18);
		Param->ClassName = L"msctls_hotkey32";
	}
	ListView::ListView()
	{
		Param->ClassName = L"SysListView32";
		Param->ExStyle = WS_EX_CLIENTEDGE;
	}
	DWORD ListView::SetExStyle(DWORD exStyle,bool add)
	{
		//这个消息的规则如下，如果 wParam 为 0，则直接设置 lParam 指定的风格；如果 wParam 包含和某个风格，而
		//lParam 不包含这个风格，则这个风格被移除；如果 wParam 不包含某个风格，而 lParam 包含这个风格，这个风
		//格无效；wParam 和 lParam 都包含的风格被添加。
		if(add)
			return SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,exStyle,exStyle);
		else
			return SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE,exStyle,0);
	}
	//class Status Bar
	StatusBar::StatusBar()
	{
		Param->ClassName = L"msctls_statusbar32";
	}
	TrackBar::TrackBar()
	{
		Param->ClassName = L"msctls_trackbar32";
	}
	//class UpDown Control
	UpDown::UpDown()
	{
		Param->ClassName = L"msctls_updown32";
	}
	INT_PTR UpDown::Create(HWND parent){
		return ICtrl::Create(parent);
	}
	INT_PTR UpDown::Create(HWND buddy,HWND parent,int nUpper,int nLower,int nPos)
	{
		if(parent==0)
			parent = GetParent(buddy);
		if(parent==0) return 0;
		DWORD style = GetWindowLong(buddy,GWL_STYLE);
		style |= ES_NUMBER;
		SetWindowLong(buddy,GWL_STYLE,style);
		_Handle = CreateUpDownControl(WS_VISIBLE|WS_CHILD|UDS_SETBUDDYINT|UDS_ALIGNRIGHT|UDS_ARROWKEYS,0,0,0,0,parent,Param->Identity,base::Instance(),buddy,nUpper,nLower,nPos);
		if(_Handle==0){
			base::Warning(L"UpDown.Create");
			return 0;
		}
		OnCreate.Call();
		_QuondamProc = ::SetWindowLongPtr(_Handle,GWLP_WNDPROC,(LONG)(LONG_PTR)_WndPointer.Procedure());
		return (INT_PTR)_Handle;
	}
	//class Header
	Header::Header(){
		Param->ClassName = WC_HEADER;//L"SysHeader32"
		Param->AddStyle(WS_BORDER | HDS_BUTTONS | HDS_HORZ);
	}
	Link::Link(){
		Param->ClassName = WC_LINK;
	}
	IpAddress::IpAddress(){
		Param->ClassName = WC_IPADDRESS;
	}
	IDlg::IDlg(){
		GetMsgHook(WM_INITDIALOG)->Add(this,&IDlg::_MsgHookProc);
		GetMsgHook(WM_ACTIVATE)->Add(this,&IDlg::_MsgHookProc);
		GetMsgHook(WM_CLOSE)->Add(this,&IDlg::_MsgHookProc);
		_defDlgFunc.Bind(this,&IDlg::_defDlgProc);
		_QuondamProc = (LONG_PTR)_defDlgFunc.Procedure();
	}
	HRESULT IDlg::_defDlgProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam){
		return (HRESULT)FALSE;
	}
	void IDlg::_MsgHookProc(Msg* msg){
		if(msg->uMsg==WM_CLOSE){
			msg->Result._SetDisableWndproc(1);//禁用内部调用 OnClose 。
		}else if(msg->uMsg==WM_INITDIALOG){
			OnCreate.Call();
		}else if(msg->uMsg==WM_ACTIVATE){
			if(LOWORD(msg->wParam)==WA_INACTIVE)
				base::SetDialog(0);
			else
				base::SetDialog(_Handle);
		}
	}
	int IDlg::GetCtrlText(int id,base::String& str){
		HWND ctrl = GetDlgItem(_Handle,id);
		if(ctrl==0) return 0;
		int i = GetWindowTextLength(ctrl);
		if((int)str.Cubage()<i) str.SetCubage(i);
		i = GetWindowText(ctrl,str.Handle(),str.Cubage()+1);
		str.Realize();
		return str.Length();
	}
	//class ModelDialog
	ModelDialog::ModelDialog(){
	}
	INT_PTR ModelDialog::Create(HWND hParent){
		if(_Handle) return 0;
		if(hParent==0) hParent = GetDesktopWindow();
		return ::DialogBoxParam(Param->Dialog.Instance,Param->Dialog,hParent,(DLGPROC)_WndPointer.Procedure(),(LPARAM)Param->lParam);
	}
	bool ModelDialog::Close(int code){
		bool cancel = 0;
		OnClose(cancel);
		if(cancel) return 0;
		return 0!=::EndDialog(_Handle,code);
	}
	NormalDialog::NormalDialog(){
		GetMsgHook(WM_ACTIVATE)->Add(this,&NormalDialog::_MsgHookProc);
	}
	INT_PTR NormalDialog::Create(HWND hParent)
	{
		if(_Handle) return 0;
		if(hParent==0) hParent = GetDesktopWindow();
		return (INT_PTR)::CreateDialogParam(Param->Dialog.Instance,Param->Dialog,hParent,(DLGPROC)_WndPointer.Procedure(),(LPARAM)Param->lParam);
	}
	bool NormalDialog::Close(){
		bool cancel = 0;
		OnClose(cancel);
		if(cancel) return 0;
		return 0!=::DestroyWindow(_Handle);
	}*/
	//Message Loop
	static __declspec(thread) HWND	_Dlg;	//对话框句柄。
	static __declspec(thread) HACCEL	_Accel;	//加速键表。
	//开始线程的主消息循环.
	int MLStart(){
		MSG msg;
		while(::GetMessage(&msg,0,0,0)){
			if(_Dlg){
				if(IsDialogMessage(_Dlg,&msg)) continue;
			}
			if(_Accel){
				if(TranslateAccelerator(msg.hwnd,_Accel,&msg)) continue;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		return (int)msg.wParam;
	}
	Handle<Value> CMLStart(const Arguments& args){
		return Int32::New(MLStart());
	}
	Handle<Value> CMLSetDlg(const Arguments& args){
		if(args.Length()>0){
			HWND hdlg = (HWND)(INT_PTR)args[0]->Int32Value();
			MLSetDialog(hdlg);
		}
		return Undefined();
	}
	Handle<Value> CMLSetAccelerator(const Arguments& args){
		if(args.Length()>0){
			HACCEL hacl = (HACCEL)(INT_PTR)args[0]->Int32Value();
			MLSetAccelerator(hacl);
		}
		return Undefined();
	}
	WndPosRestore::WndPosRestore(){}
	WndPosRestore::~WndPosRestore(){}
	void WndPosRestore::_OnCreate(Msg* msg){
		int index = -1;
		for(uint i=0;i<wndList.Count();i++){
			if(wndList[i]->Handle()==msg->hWnd){
				index = i;
				break;
			}
		}
		if(index==-1) return;
		base::RegisterTree rt(0);
		if(!rt.Lock()){
			return;
		}
		base::Xml* pst = rt.GetXml()->GetNode(L"wndpos");
		if(pst==0) return;
		pst = pst->GetNode(wndName[index]);
		if(pst==0) return;
		int p[4];
		if(!pst->GetPropertyInt(L"left",*p)) return;
		if(!pst->GetPropertyInt(L"top",*(p+1))) return;
		if(!pst->GetPropertyInt(L"right",*(p+2))) return;
		if(!pst->GetPropertyInt(L"bottom",*(p+3))) return;
		WndPtr wnd = msg->hWnd;
		wnd.SetDimension(p[0],p[1],p[2]-p[0],p[3]-p[1]);
	}
	void WndPosRestore::_OnDestroy(Msg* msg){
		int index = -1;
		for(uint i=0;i<wndList.Count();i++){
			if(wndList[i]->Handle()==msg->hWnd){
				index = i;
				break;
			}
		}
		if(index==-1) return;
		base::AutoStruct<WINDOWPLACEMENT,1> wp;
		GetWindowPlacement(msg->hWnd,&wp);
		base::RegisterTree rt(0);
		if(!rt.Lock()){
			return;
		}
		base::Xml* pst = rt.GetXml()->GetNode(L"wndpos",1);
		pst = pst->GetNode(wndName[index],1);
		if(pst==0) return;
		pst->SetPropertyInt(L"left",wp.rcNormalPosition.left);
		pst->SetPropertyInt(L"top",wp.rcNormalPosition.top);
		pst->SetPropertyInt(L"right",wp.rcNormalPosition.right);
		pst->SetPropertyInt(L"bottom",wp.rcNormalPosition.bottom);
	}
	bool WndPosRestore::AddWnd(IWnd* wnd,LPCWSTR name){
		if(wnd==0) return 0;
		if(-1!=wndName.IndexOf(name)) return 0;
		wnd->GetMsgHook(WM_DESTROY)->Add(this,&WndPosRestore::_OnDestroy);
		wnd->GetMsgHook(WM_CREATE)->Add(this,&WndPosRestore::_OnCreate);
		//Dialog no WM_CREATE Message
		wnd->GetMsgHook(WM_INITDIALOG)->Add(this,&WndPosRestore::_OnCreate);
		wndList.Add(wnd);
		wndName.Add(name);
		return 1;
	}
	bool WndPosRestore::RemoveWnd(IWnd* wnd)
	{
		int index = wndList.IndexOf(wnd);
		if(index==-1) return 0;
		wndList.Delete(index);
		wndName.Delete(index);
		wnd->GetMsgHook(WM_DESTROY)->Delete(this,&WndPosRestore::_OnDestroy);
		wnd->GetMsgHook(WM_CREATE)->Delete(this,&WndPosRestore::_OnCreate);
		wnd->GetMsgHook(WM_INITDIALOG)->Delete(this,&WndPosRestore::_OnCreate);
		return 1;
	}

	void WndPosRestore::OnCreate(HWND hwnd){
		Msg msg;
		msg.hWnd = hwnd;
		_OnCreate(&msg);
	}
	void WndPosRestore::OnDestroy(HWND hwnd){
		Msg msg;
		msg.hWnd = hwnd;
		_OnDestroy(&msg);
	}
	inline WndPosRestore* getAppWndPosRestore(){
		static WndPosRestore wpr;
		return &wpr;
	}
	void WPRRegisterWnd(IWnd* wnd,LPCWSTR name){
		getAppWndPosRestore()->AddWnd(wnd,name);
	}
	void WPRUnRegisterWnd(IWnd* wnd){
		getAppWndPosRestore()->RemoveWnd(wnd);
	}
	void LoadWndFunc(Handle<Object>& gt){
		SetGlobalFunc(gt,L"CMLStart",&CMLStart);
		SetGlobalFunc(gt,L"CMLSetAcl",&CMLSetAccelerator);
		SetGlobalFunc(gt,L"CMLSetDlg",&CMLSetDlg);
	}

}