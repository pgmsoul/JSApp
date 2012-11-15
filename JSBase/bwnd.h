#pragma once
namespace v8{
#define WM_CTRLUSER			0x8000					//控件的消息转移消息ID
#define WM_CTRLCOLOR		WM_CTRLUSER + 0x0132	//控件的颜色消息.
#define WM_GETWINDOW		WM_CTRLUSER + 0x01		//从HWND获取Window*指针.
#define WM_CTRLMSG			WM_CTRLUSER + 0x02
	extern  __declspec(thread) HWND	_Dlg;	//对话框句柄。
	extern __declspec(thread) HACCEL	_Accel;	//加速键表。
	//开始线程的主消息循环.
	int MLStart();
	//设置当前线程中对话框的句柄，以使它能够响应 Tab 键。
	inline void MLSetDialog(HWND hDlg){_Dlg = hDlg;}
	//设置当前线程的加速键表。
	inline void MLSetAccelerator(HACCEL hacl){_Accel = hacl;}

	//窗口函数的返回值类型.
	class  Result : public base::HandleType<INT_PTR>{
	public:
		enum{RESULT_DEFAULT = 0x1,WNDPROC_DISABLE = 0x2};
		INT_PTR		_Flag;	//控制位，用于某些内部应用。
		Result():_Flag(0){};
		inline void operator = (INT_PTR result){_Flag |= RESULT_DEFAULT;_Handle = result;}
		//缺省返回值，此值为true：Wndproc处理完消息，将调用窗口的缺省原内部函数，并且返回它的返回值。
		//此值为false：Wndproc处理完消息，返回用户调用 = 操作设置的值，不调用窗口的内部函数。
		inline bool Default(){return (_Flag&RESULT_DEFAULT)==0;}
		//重新设置为缺省状态。
		inline void SetDefault(){_Flag &= ~RESULT_DEFAULT;}
		//用户自己处理消息，不调用Wndproc的处理代码。Wndproc内部只处理了很少的有限消息，
		//这个函数用户一般不需要调用。
		inline bool _DisableWndproc(){return (_Flag&WNDPROC_DISABLE)!=0;}
		//设置是否禁用Wndproc对消息的处理代码。
		inline void _SetDisableWndproc(bool disable){if(disable) _Flag|=WNDPROC_DISABLE;else _Flag&=~WNDPROC_DISABLE;}
	};
	//Windows 消息结构
	typedef struct  Msg : public base::_struct{
		HWND		hWnd;		//消息的窗口句柄.
		UINT		uMsg;		//消息ID
		WPARAM		wParam;		//参数
		LPARAM		lParam;		//参数
		Result		Result;	//返回值.
	}Msg;
	//鼠标结构,用于鼠标消息.
	typedef struct  Mouse : public base::_struct{
		Mouse(){}
		Mouse(Mouse& m){*this = m;}
		void operator = (Mouse& m){Event = m.Event;Button = m.Button;WheelDelta = m.WheelDelta;X = m.X;Y = m.Y;Msg=m.Msg;}
		UINT Event;			//对应的鼠标消息.
		short Button,		//动作按键,可以是0,比如当Event是WM_MOUSEMOVE时.MK_常数
			WheelDelta;		//当鼠标轮滚动时(Event==WM_MOUSEWHEEL),这个值是120或-120,与滚动的方向有关.
		short X,Y;			//指针的位置.
		Msg* Msg;
	}Mouse;
	//非客户去鼠标结构
	typedef struct  NcMouse : public base::_struct{
		UINT uMsg;		//消息ID
		UINT hitTest;	//HT常数，知识鼠标的状态。
		short x,y;		//屏幕坐标
		Result* lpResult;	//返回值。
	}NcMouse;
	typedef struct  CtrlMsg : public base::_struct{
		HWND		hWnd;		//消息的窗口句柄.
		UINT		uMsg;		//消息ID,WM_NOTIFY,WM_COMMAND,WM_CTRLCOLOR之一。
		WPARAM		wParam;		//参数
		LPARAM		lParam;		//参数
		Result*		pResult;	//返回值.
	}CtrlMsg;
	//窗口生成时用到的结构,用于初始化窗口的必要参数.
	typedef struct  CreateStruct : public base::_struct{
		CreateStruct():ClassStyle(CS_DBLCLKS),Style(WS_VISIBLE),ExStyle(0),Menu(0),lParam(0),Location(200,200),Size(500,300){}
		base::String ClassName,
			Title;					//窗口的标题,只对顶级窗口有效.
		base::Twin16 Location,Size;	//窗口位置,大小.
		DWORD ClassStyle,Style,ExStyle;	//窗口的三类风格
		union{
			HMENU		Menu;				//菜单(对于主窗口) 
			WORD		Identity;			//ID(对于子窗口).
		};				
		base::ResID		Dialog;				//对于对话框
		LPVOID lParam;					//额外参数.
		void SetRect(int left,int top,int width,int height){
			Location.SetValue(left,top);
			Size.SetValue(width,height);
		}
		void AddStyle(DWORD style){
			Style |= style;
		}
		void RemoveStyle(DWORD style){
			Style &= ~style;
		}
		void AddExStyle(DWORD style){
			ExStyle |= style;
		}
		void RemoveExStyle(DWORD style){
			ExStyle &= ~style;
		}
	}CreateStruct;
	//窗口对象的基类，整个窗口系统（自定义窗口类，控件，对话框）全部基于这个类，最好虚继承这个类.
	//Ptr后缀的类都是绑定一个指针对象，大小不会大于指针的大小，所以它只集成函数，不集成数据。
	class  WndPtr : public base::HandleType<HWND>{
	public:
		WndPtr(){}
		WndPtr(HWND hwnd){_Handle = hwnd;}
		//绑定一个窗口。
		void operator = (HWND hwnd){_Handle = hwnd;}
		//返回窗口的区域,对于顶级窗口对应屏幕坐标,对于子窗口,对应父窗口坐标.
		bool GetRect(base::Rect& r);		
		//返回窗口的客户区尺寸.窗户区的左上角坐标一般是(0,0),但也可能不是,如果有边框的话.但它的具体坐标没有一个API函数可以提供.可以使用Window类的成员来获取.
		bool GetClientSize(base::Twin16& c);						
		//设置大小，不改变位置.
		inline bool SetSize(int cx,int cy) {return ::SetWindowPos(_Handle,0,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER)!=0;}
		//设置位置，不改变大小
		inline bool SetLocation(int x,int y){return ::SetWindowPos(_Handle,0,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER)!=0;}
		//设置大小和位置。
		inline bool SetDimension(int x,int y,int cx,int cy) {return ::MoveWindow(_Handle,x,y,cx,cy,1)!=0;}
		//移动窗口或调整窗口大小,dx,dy是窗口位置的偏移量,cx,cy是大小的变化量.
		void Offset(int dx, int dy, int cx, int cy);	

		//取得文本字串的长度，不包括结尾 0 字符。
		inline int TextLength(){return ::GetWindowTextLength(_Handle);}
		//取得窗口文本，text不必初始化。
		int GetText(base::String& text);
		//设置窗口文本。
		inline bool SetText(LPCWSTR text){return ::SetWindowTextW(_Handle,text)!=0;}
		//给窗口设置计时器,成功后窗口会定时发送WM_TIMER消息,ms是间隔时间(毫秒),id是计时器的标识,如果这个id是之前设置的一个timer，则替换原来的。对于无窗口绑定的计时器，且不想替换已有的timer，应该给id传0值，由系统来分配一个id，函数返回这个id。
		inline UINT_PTR SetTimer(UINT ms,UINT_PTR id){return ::SetTimer(_Handle,id,ms,NULL);}
		//删除一个timer。id是设置时返回的id。
		inline bool KillTimer(UINT_PTR id){return ::KillTimer(_Handle,id)!=0;}
		//要求窗口重绘指定的区域,erase是否重绘背景,也就是是否引发WM_ERASEBKGND消息.
		inline bool Invalidate(const RECT* clip = 0,bool erase = 1){return ::InvalidateRect(_Handle,clip,erase)!=0;}
		//要求窗口重绘指定的区域,erase是否重绘背景,也就是是否引发WM_ERASEBKGND消息.
		inline bool Invalidate(const HRGN hRgn,bool erase = 1){return ::InvalidateRgn(_Handle,hRgn,erase)!=0;}
		//返回窗口参数。
		inline LONG_PTR GetWindowParam(int gwl){return ::GetWindowLongPtr(_Handle,gwl);}
		//设置窗口相关参数.
		inline LONG_PTR SetWindowParam(int gwl,LONG_PTR value){return ::SetWindowLongPtr(_Handle,gwl,(LONG)value);}
		//窗口的父窗口句柄.
		inline HWND Parent(){return ::GetParent(_Handle);}
		//消息函数,有更自由的参数类型,这个函数本身使用的参数类型是非常多的.
		template<typename T,typename R> inline LRESULT SendMessage(UINT ttm,T wParam,R lParam){return ::SendMessage(_Handle,ttm,(WPARAM)wParam,(LPARAM)lParam);}
		//Enable 或者 Disable 一个窗口。
		inline bool Enable(bool enable = 1){return ::EnableWindow(_Handle,enable)!=0;}//return value is pre state of disabled.
		//向窗口发送WM_CLOSE消息，如果关闭没有被取消返回true，反之返回false。
		bool Close(){return 0!=SendMessage(WM_CLOSE,0,0);}
		//更改窗口的风格。
		void ModifyStyle(DWORD dwRemove,DWORD dwAdd);
		//更改窗口的扩展风格。
		void ModifyExStyle(DWORD dwRemove,DWORD dwAdd);
		//设置窗口的宽度。
		bool SetWidth(int cx);
		//设置窗口的高度。
		bool SetHeight(int cy);
		//设置窗口的水平位置。
		bool SetXLocation(int x);
		//设置窗口的垂直位置。
		bool SetYLocation(int y);
	};
	//截获窗口消息的静态函数类型.
	typedef void (__stdcall*MSGPROC)(Msg*); 
	//Class IWnd 类提供基本的窗口结构，不要实例化这个类。
	interface  IWnd : public WndPtr{//public HandleType<HWND>//virtual 
	public:
		//因为有一个外部函数要调用这个函数,在一般情况下这个函数不必在外部调用,所以用"_"来标识.
		LRESULT _WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		base::Function<LRESULT,HWND,UINT,WPARAM,LPARAM> _WndPointer;	//窗口函数.
	private:
		typedef struct _UPOINT : base::_struct{
			UINT x,y;
			bool operator == (_UPOINT up){
				return (x==up.x)&&(y==up.y);
			}
		}_UPOINT;
		base::Diction<_UPOINT> _MsgList;
		base::ObjectList<base::Delegate<Msg*> > _ProcList;
		bool _mLeave;					//鼠标是否离开.
	protected:
		base::Twin		_DragMinSize;		//允许拖拽时的最小尺寸。
		base::Twin		_DragMaxSize;		//允许拖拽时的最小尺寸。
		base::Twin		_MinSize;			//没有滚动条的最小大小.
		base::Twin		_Scr;				//偏移量
		base::gdi::Color	_FontColor;			//控件的前景色(字体颜色).
		base::gdi::Font	_Font;				//这个成员用于显示的字体.
		base::gdi::Brush	_BkgndColor;		//背景色
		base::Twin16		_ClientLocation;	//Client Location;
		LONG_PTR	_QuondamProc;		//窗口原来的窗口函数.对于一个用户自己定义的窗口,这个参数无关紧要,一般用DefWndProc来代替,对于一个Windows内部的窗口,比如Text控件,这个参数必须正确传递.
		HCURSOR		_Cursor;			//鼠标指针形状

	public:
		IWnd(void);
		~IWnd();
		base::Object<CreateStruct>	Param;		//这个成员用于设置窗口生成时的参数,它有自己的默认参数,只需要设置想改变的参数.
		base::Delegate<WORD>		OnCommand;	//因为这个事件会被频繁的使用,所以集成它.
		base::Delegate<>			OnCreate;	//WM_CREATE消息，但是在生成控件的时候，这消息其实是生成完成后模拟调用了一次。
		base::Delegate<Mouse*>		OnMouse;	//MouseEvent处理所有鼠标消息.		
		base::Delegate<NcMouse*>	OnNcMouse;	//OnNcMouse处理非客户区的（除WM_NCMOUSELEAVE）所有鼠标消息.实际上所有消息都是WM_NCHITTEST经过DefWindowProc计算后产生的。
		base::Function<void,bool&>	OnClose;	//截获WM_CLOSE消息，bool值默认为false，如果置为true，则窗口关闭动作取消。
		
		//滚动偏移
		inline base::Twin Scr(){return _Scr;}
		//出现滚动条的最小尺寸
		inline base::Twin MinSize(){return _MinSize;}
		//客户区在窗口坐标中的位置.;
		inline base::Twin16 GetClientPos() const {return _ClientLocation;}
		//返回对应消息的代表指针,可以从这个指针添加或删除相应的函数.
		base::Delegate<Msg*>* GetMsgHook(UINT msg);
		//返回对应消息的代表指针,可以从这个指针添加或删除相应的函数.
		base::Delegate<Msg*>* GetMsgHook(UINT msg1,UINT msg2);
		//设置控件的背景色.设为-1表示背景透明(这种情况下重绘字体时,往往会形成重叠显示字符).
		void SetBkgndColor(COLORREF color);
		//设置控件的前景色(字体颜色).
		void SetFontColor(COLORREF color);
		//设置控件使用的字体,实际上调用这个函数一次就会重新生成一个字体.
		bool SetFont(bool reDraw = 0);
		//取得当前字体参数.
		base::gdi::Font* GetFont(){return &_Font;}
		//生成一个窗口，调用这个函数必须设置Param的ClassName参数，这个参数决定窗口的类型，如果是自定义窗口，必须自己注册这个类。否则函数会失败。
		//virtual INT_PTR Create(HWND parent){return 0;}
		//设置拖放时的最小尺寸
		void SetMinDragSize(int cx,int cy){_DragMinSize.SetValue(cx,cy);}
		//设置拖放时的最大尺寸
		void SetMaxDragSize(int cx,int cy){_DragMaxSize.SetValue(cx,cy);}
	};
	IWnd* GetWindowByHandle(HWND hWnd);
	//Class View
	class  View : public IWnd{//public WndPtr//,
	protected:
		byte			_BkAlpha;
		byte			_BkMode;
		base::gdi::DrawBitmap _BkgndImg;

		void _DropFilesProc(Msg* msg);	
		void _DrawProc(Msg* msg);
		void _DoubleDraw(base::gdi::DC* dc);
		void _EraseBkgndProc(Msg* msg);
		void _CtrlSizeProc(Msg* msg);
	public:
		enum{
			BK_NONE	= 0, //不显示任何背景
			BK_COLOR	= 1,	//显示背景颜色
			BK_IMAGE	= 2,	//显示背景图
			BK_IMAGE_ALPHA	= 4	//背景图包含Alpha通道。
		};
		View();
		~View();
		//当前窗口没有销毁之前,不能生成第2个窗口.
		INT_PTR Create(HWND hParent);

		bool Attach(HWND handle);
		HWND Detach();
		//设置背景色
		void SetBkgndColor(COLORREF color,bool reDraw = 0);
		//取得背景色
		COLORREF GetBkgndColor(){return _BkgndColor.GetColor();}
		//设置背景图片，alpha是透明度。不要消毁bmp，这个资源会自动清理。
		void SetBkgndImage(HBITMAP bmp,bool reDraw = 0);
		//取得背景图像的一个副本，使用完成后要手动清理资源。
		HBITMAP GetBkgndImage(){return _BkgndImg.GetBitmap();}
		//设置背景图的透明度。
		void SetBkgndAlpha(byte alpha,bool redraw = 0);
		//取得背景图的透明度。
		byte GetBkgndAlpha(){return _BkAlpha;}
		//设置背景模式，分别为背景色，背景图（不管alpha通道），考虑背景图的alpha通道。
		void SetBkgndMode(byte mode,bool redraw = 0);
		//取得背景模式。
		uint GetBkgndMode(){return _BkMode;}
		//设置窗体指针。
		void SetCursor(HCURSOR cursor);
		//取得当前鼠标指针。
		HCURSOR GetCursor(){return _Cursor;}
		//设置窗体指针，参数为资源id和资源所在地实例，0表示系统资源。
		void SetCursor(base::ResID resurce,HINSTANCE hInst = 0);

		base::Delegate<base::gdi::DC*>	OnDoubleDraw;//一旦在这个调用中加入函数，OnDraw就不会被呼叫。这个函数会自动滚动窗口，也就是说绘图不必自己考虑窗口的滚动问题。
		base::Delegate<base::gdi::DC*>	OnDraw;//绘图消息在程序中实际上是异步于应用程序线程的.		
		base::Delegate<HDC>		OnEraseBkgnd;	//也可以在这里面作图,它在OnDraw之后被调用.但它们的机制是不同的
		base::Delegate<base::StringMemList*>	OnDropFiles;//响应拖曳消息.
		base::Delegate<base::Twin16>	OnSize;	//WM_SIZE消息，参数是客户区的大小。
	};
	//Class ScrollWnd
	class  ScrollWnd : public View{
	public:
		ScrollWnd(void);
		~ScrollWnd();
		//设置最小的客户区尺寸,当客户区小于这个尺寸,滚动条出现.
		void SetMinSize(base::Twin size);	
		//设置最小的客户区尺寸,当客户区小于这个尺寸,滚动条出现.如果cx或cy为负值，则相应的大小不变。
		void SetMinSize(int cx,int cy);				
		//设置最小的滚动单位.
		void SetMinScroll(int hsline, int vsline);		
		//0-hScroll,2-NoScroll,设置鼠标滚轮滚动窗口的方向.
		void SetMouseWheelScroll(int vScroll = 1);		
		//设置是否响应键盘方向键而滚动窗口.
		void SetResponseKeyBoard(bool response = 1);	
		//滚动窗口，dx：水平滚动距离，dy：垂直滚动距离。
		void Scroll(int dx,int dy);
		//滚动指定的行数，xLine 是水平行数，yLine 是垂直行数，负数反方向滚动。
		void ScrollLine(int xLine,int yLine);
		//滚动指定的页数，xPage 是水平页数，yPage 是垂直页数，负数反方向滚动。
		void ScrollPage(int xPage,int yPage);
		//滚动到顶点，xEnd = 1 滚动到最右边，xEnd = -1 滚动到最左边，yEnd = 1 滚动到最下边，yEnd = -1 滚动到最上边。
		void ScrollEnd(int xEnd,int yEnd);
		//重绘窗口.如果客户区有滚动,这个函数会自己处理,所以不必对区域进行平移.
		bool Invalidate(const RECT* clip = NULL,bool erase = 1);
		//重绘窗口.如果客户区有滚动,这个函数会自己处理,所以不必对区域进行平移.
		bool Invalidate(const HRGN clip,bool erase = 1);
		//调整一个点,根据滚动偏移量.
		void TransDimension(base::Twin& pt);			
		//调整一个矩形,根据滚动偏移量.
		void TransDimension(base::Rect& rect);			

		base::Delegate<int,int> OnScroll;//窗口滚动时调用.参数为偏移量,实际上,两个参数总有一个是0.

	private:
		void _MouseWheelProc(Msg* msg);
		void _VScrollProc(Msg* msg);
		void _HScrollProc(Msg* msg);
		void _KeyDownProc(Msg* msg);
		void _SizeProc(Msg* msg);

		int _vsPage;
		int _hsPage;
		int _vsLine;
		int _hsLine;
		int _vsMax;
		int _hsMax;
		DWORD _MouseWheelScroll;//发送的WM_VSCROLL或WM_HSCROLL消息.
		bool _ResponseKeyBoard;//指示响应键盘，有垂直条，有水平条
	};
	//Class Frame
	//Frame对象点击关闭按钮会自动退出应用程序，但是这必须在启动线程里才有效，本质上，它
	//内部会调用PostQuitMessage，所以只会退出它所在的线程。
	class  Frame : public ScrollWnd{
	public:
		Frame();
		~Frame();
		//生成一个没有WS_CHILD风格的主窗口，虽然它实际上也可以是一个窗口的子窗口，但是它并不随着主窗口销毁
		//而销毁。quit指定是否退出时程序结束。
		INT_PTR Create(HWND hParent = 0,int nCmdShow = 1,bool quit = 1);
		//退出消息循环，窗口关闭，一般应用程序也会退出．
		void Quit(int code = 0);
		//big=0,for set small icon,big=1,for set big icon.
		inline HICON SetIcon(HICON icon,bool big=0){return (HICON)SendMessage(WM_SETICON,big,icon);}
		//type=0,for small icon,type=1,for large icon,type=2,for system assign icon for application.
		inline HICON GetIcon(DWORD type=ICON_SMALL){return (HICON)SendMessage(WM_GETICON,type,0);};
	private:
		void _DestroyProc(Msg* msg);
	};
	//设置主窗口的图标，含大图标和小图标。
	void SetWndIcon(HWND h,base::ResID id);
	//这个函数初始化要使用的控件,默认参数初始化所有控件.
	bool InitControl60(DWORD icc = ICC_WIN95_CLASSES);
	//ImageList管理图标或图像,它主要用于其它控件使用的图像源.
	class ImageList : public base::HandleType<HIMAGELIST>{
	public:
		~ImageList();
		//绑定一个HIMAGELIST句柄.
		void operator = (HIMAGELIST hImgl);
		//这个函数返回零可能表示没有成功销毁前一个句柄(但hImgl成功绑定),或hImgl为NULL.
		bool Attach(HIMAGELIST hImgl);
		//解除绑定的句柄.
		HIMAGELIST Detach();

		//生成一个ImageList,cx,cy是图像的大小,一个ImageList只能存储相同大小和色深的图像.ilc是图像的类型,initial初始大小,grow增量.
		bool Create(int cx,int cy,UINT ilc = ILC_COLORDDB,int initial = 16,int grow = 16);
		//从图像资源加载.这个函数是创建一个全新的ImageList,原来的HIMAGELIST实例被删除.cx是生成的IMAGELIST图像宽度,根据这个宽度和加载的
		//图像宽度来确定生成多少个图像,不足一个图像的部分会被舍弃.mask是透明色,res是资源ID,hInst = 0表示从BMP文件加载(不表示从当前实例加载),
		//grow是增量.返回图像个数.
		int LoadResource(int cx,COLORREF mask,base::ResID res,HINSTANCE hInst = 0,int grow = 16);
		//销毁HIMAGELIST.
		bool Destroy();

		//替换相应索引的图标,HICON资源要自己处理.
		bool Replace(HICON icon,int index);
		//添加一个图标,HICON资源要自己处理.图标的色深和ImageList的色深要一致,否则背景不透明.
		bool AddIcon(HICON icon);
		//把位图添加到ImageList中,返回添加的数量,这个数量是位图的宽度可以分割成的ImageList中图像的数量,不够一个的部分舍去.
		int AddBitmap(HBITMAP bmp,HBITMAP mask = 0);
		//删除相应索引的图像.
		bool Delete(int index);
		//清空图像
		bool Clear();
		//显示图像.
		bool Draw(HDC hdc,int index,int x,int y,UINT style = ILD_NORMAL);
		//拷贝相应索引的图像.
		bool Copy(int iDst,int iSrc);
		//交换相应索引的图像.
		bool Swap(int iDst,int iSrc);
		//创建一个副本.
		HIMAGELIST Duplicate();
		//返回图像数量.
		int Count();
		//返回图像大小.
		base::Twin GetImageSize();
		//返回相应索引的图像.
		HICON GetIcon(int index);
		//返回背景色.
		COLORREF GetBkColor();
		//设置背景色.
		COLORREF SetBkColor(COLORREF bk);
		//设置图像大小,这个操作会清空图像.
		bool SetImageSize(int cx,int cy);
		//这个函数设置图像的容量,如果数量大于原来的数量,必须自己用AddIcon函数填补空位.
		bool SetCount(int n);
	};
	//PBS_NORMAL = 1,PBS_HOT = 2,PBS_PRESSED = 3,PBS_DISABLED = 4,PBS_DEFAULTED = 5,PBS_STYLUSHOT = 6
	interface  ButtonPtr : public WndPtr{
	public:
		ButtonPtr(){}
		ButtonPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//返回按钮显示文字所需的尺寸.
		bool GetIdealSize(LPSIZE ps){return SendMessage(BCM_GETIDEALSIZE,0,ps)!=0;}
		//获取按钮的选中状态,它返回3个值:0(未选中),1(选中),2(不确定).
		int GetCheck(){return (int)(INT_PTR)SendMessage(BM_GETCHECK,0,0)!=0;}
		//设置按键的选中状态.//BST_UNCHECKED(0),BST_CHECKED(1),BST_INDETERMINATE(2);
		void SetCheck(int state = BST_CHECKED){SendMessage(BM_SETCHECK,(WPARAM)state,0);}
		//设置一个ImageList作为按钮显示的图像列表.
		bool SetImageList(HIMAGELIST hImgl,UINT align = BUTTON_IMAGELIST_ALIGN_LEFT,LPRECT margin = 0);
		//返回按键的图像文字显示参数.
		bool GetImageList(BUTTON_IMAGELIST& bi){return SendMessage(BCM_GETIMAGELIST,0,&bi)!=0;}
		//设置文本的位置参数.
		bool SetTextMargin(int left,int top,int right,int bottom){return SendMessage(BCM_SETTEXTMARGIN,0,&left)!=0;}
		//获取文本的位置参数.
		bool GetTextMargin(LPRECT margin){return SendMessage(BCM_GETTEXTMARGIN,0,margin)!=0;}
		//模拟按下按钮动作.
		void Click(){SendMessage(BM_CLICK,0,0);}
	};
	/*
	interface  ListBoxPtr : virtual public WndPtr{
	public:
		ListBoxPtr(){}
		ListBoxPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加一个字串.str是要添加的字串,index是索引位置.返回字串在列表中的索引.
		inline int AddString(LPCWSTR str,int index = -1);
		//删除一个字串.index是要删除的字串的索引.
		inline bool DeleteString(int index)					{return (int)SendMessage(LB_DELETESTRING,(WPARAM)index,0)!=0;}
		//查找一个字串,begin是开始查找的索引,返回字串的索引或-1.这个查找只要部分匹配就可以,而且不分大小写.
		inline int FindString(LPCWSTR str,int begin = -1)	{return (int)SendMessage(LB_FINDSTRING,(WPARAM)begin,(LPARAM)str);}
		//精确查找,不但要求整个字串匹配,还要区分大小写.
		inline int FindExactString(LPCWSTR str,int begin = -1)	{return (int)SendMessage(LB_FINDSTRINGEXACT,(WPARAM)begin,(LPARAM)str);}
		//返回元素数量.
		inline int GetCount()								{return (int)SendMessage(LB_GETCOUNT,0,0);}
		//判断项是否已经被选中.
		inline bool ItemIsSelect(int index)					{return (int)SendMessage(LB_GETSEL,(WPARAM)index,(LPARAM)0)!=0;}
		//返回选中项的索引,或-1.仅用于单列表.
		inline int GetSelItem()								{return (int)SendMessage(LB_GETCURSEL,0,0);}//this is only use for single select list box,for multisel it return value is the focused item(but single select is always select item);
		//设置列表的相应索引项选中,如果index = -1则没有项被选中.仅用于单选列表.
		inline void SetSelItem(int index)					{SendMessage(LB_SETCURSEL,(WPARAM)index,(LPARAM)0);}//for single select list box, must one item is selected.
		//设置特定字串为选中状态.
		inline int SetSelItem(LPCWSTR str,int start = -1)	{return (int)SendMessage(LB_SELECTSTRING,(WPARAM)start,(LPARAM)str);}
		//获取多选时的选中项索引,sel要设置足够的大小.返回选中项的总数.这个函数只能用于多选列表,对于单选列表,总是返回-1.
		inline int GetSelItems(base::Memory<int>* sel)		{return (int)SendMessage(LB_GETSELITEMS,(WPARAM)sel->Length(),(LPARAM)sel->Handle());}//this is only use for multy select style,for single select style it is always return -1;
		//设置多选列表的选中范围,只能用于多选列表.如果last小于first,则这一范围的选中项会被清除.有两点需要注意,这个函数不能
		//让first和last相同,否则函数失败.范围选择允许超界.如果想只选中一项可以采用交集的办法,先选2项,再清除一项.即使对于LBS_EXTENDEDSEL风格
		//的列表,也可以和这个方法选取任意的项,而不一定连续.
		inline int SetSelItems(WORD first,WORD last)		{return (int)SendMessage(LB_SELITEMRANGEEX,(WPARAM)first,(LPARAM)last);}//if last is less than first,the range is removed from select.
		//获取选中项的总数.
		inline int GetSelCount()							{return (int)SendMessage(LB_GETSELCOUNT,0,0);}//this is only use for multy select style,for single select style it is always return -1;
		//取得相应索引项的文本,可以预先用GetTextLength取得文本所需空间大小,text的空间必须要大于返回值加1.返回取得文本的长度或-1.
		int GetText(int index,base::String& buf);
		//设置设置指定项的文本，Window并没有提供LB_SETTEXT消息，这个函数内部通过删除和添加字串完成这个功能。
		bool SetText(int index,LPCWSTR str);
		//取得相应索引项的文本长度,不包括结尾NULL字符.
		inline int GetTextLength(int index)					{return (int)SendMessage(LB_GETTEXTLEN,(WPARAM)index,0);}//返回文本长度.
		//获取特定位置的项的索引值.
		inline int GetIndexByPosition(base::Twin16 pos)	{return (int)SendMessage(LB_ITEMFROMPOINT,0,(LPARAM)pos.Value);}
		//清空所有项.
		inline void Clear()									{SendMessage(LB_RESETCONTENT,0,0);}
		//设置多列列表控件的宽度.
		inline void SetColumnWidth(int width)				{SendMessage(LB_SETCOLUMNWIDTH,(WPARAM)width,0);}
		//设置元素的绘制高度,调用这个函数后要重绘控件..
		inline bool SetItemHeight(int height)				{return (SendMessage(LB_SETITEMHEIGHT,0,(LPARAM)height)!=LB_ERR);}
		//获取顶部项(的索引).
		inline int GetTopItem()								{return (int)SendMessage(LB_GETTOPINDEX,0,0);}
		//设置顶部项.
		inline bool SetTopItem(int index)					{return (SendMessage(LB_SETTOPINDEX,(WPARAM)index,0)!=-1);}
		//ListBox不会自动设置水平滚动条.
		inline void SetHScrollWidth(int width)				{SendMessage(LB_SETHORIZONTALEXTENT,(WPARAM)width,0);}
		//把普通ListBox转换成DragListBox。
		inline bool MakeDrglist()							{return 0!=::MakeDragList(_Handle);}
	};
	interface  LabelPtr : virtual public WndPtr{
	public:
		LabelPtr(){}
		LabelPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	//Button Control
	/*只有XP风格的按钮才可以设置图像列表控件,如果图像列表控件只有一个图像,则显示这个图像.如果有多于1个图像则应该有
	5个图像,分别在正常,有焦点,鼠标停留,DISABLE状态和按下状态下显示
	//DateTimePicker Control
	interface  DateTimePickerPtr : virtual public WndPtr{
	public:
		DateTimePickerPtr(){}
		DateTimePickerPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}

		HWND GetMonthCalControl()			{return DateTime_GetMonthCal(_Handle);}
		void SetFormat(LPCWSTR format)		{DateTime_SetFormat(_Handle,format);}

		COLORREF GetMonthCalColor(int mcsc = MCSC_BACKGROUND)	{return (int)(INT_PTR)DateTime_GetMonthCalColor(_Handle,mcsc);}
		COLORREF SetMonthCalColor(COLORREF color,int mcsc = MCSC_BACKGROUND)	{return (int)(INT_PTR)DateTime_SetMonthCalColor(_Handle,mcsc,color);}

		HFONT GetMonthCalFont()				{return (HFONT)DateTime_GetMonthCalFont(_Handle);}
		void SetMonthCalFont(HFONT font,bool redraw = 0)		{DateTime_SetMonthCalFont(_Handle,font,redraw);}

		DWORD GetRange(LPSYSTEMTIME pst)	{return DateTime_GetRange(_Handle,pst);}//pst是一个含两个SystemTime元素数组的指针.
		bool SetRange(LPSYSTEMTIME pst,DWORD flag = GDTR_MIN|GDTR_MAX)			{return DateTime_SetRange(_Handle,flag,pst)!=0;}

		DWORD GetTime(LPSYSTEMTIME pst)		{return DateTime_GetSystemtime(_Handle,pst);}
		bool SetTime(LPSYSTEMTIME pst,DWORD flag = GDT_VALID)	{return DateTime_SetSystemtime(_Handle,flag,pst)!=0;}
	};
	//Calendar Control
	interface  MonthCalendarPtr : virtual public WndPtr{
	public:
		MonthCalendarPtr(){}
		MonthCalendarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	//ComboBox Control 本身包含一个ListBox控件,所以这两个控件的很多函数是很相似的.
	interface  ComboBoxPtr : virtual public WndPtr{
	public:
		ComboBoxPtr(){}
		ComboBoxPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加一个字串.返回添加的位置.
		inline int AddString(LPCWSTR str,int index = -1)		{return (int)SendMessage(CB_INSERTSTRING,(WPARAM)index,(LPARAM)str);}//retrun index of insert item.
		//删除一个字串(通过索引).
		inline bool DeleteString(int index)						{return SendMessage(CB_DELETESTRING,(WPARAM)index,0)!=-1;}
		//清空字串.
		inline void Clear()										{SendMessage(CB_RESETCONTENT,0,0);}
		//设置特定项的文本.
		inline bool SetItemText(int index,LPCWSTR str)			{return SendMessage(CB_SETITEMDATA,(WPARAM)index,(LPARAM)str)!=-1;}
		//返回特定项的文本.
		bool GetItemText(int index,base::String& str);
		//获取元素的总数量.
		inline int GetCount()									{return (int)SendMessage(CB_GETCOUNT,0,0);}
		//返回当前选择项的索引.
		inline int GetCurSelect()								{return (int)SendMessage(CB_GETCURSEL,0,0);}
		//设置当前项为特定索引.
		inline int SetCurSelect(int index)						{return (int)SendMessage(CB_SETCURSEL,(WPARAM)index,0);}
		//查找一个字串.
		inline int FindString(LPCWSTR str,int start = -1)		{return (int)SendMessage(CB_FINDSTRING,(WPARAM)start,(LPARAM)str);}
		//精确查找一个字串.
		inline int FindExactString(LPCWSTR str,int start = -1)	{return (int)SendMessage(CB_FINDSTRINGEXACT,(WPARAM)start,(LPARAM)str);}
		//设置选定项(通过字串).
		inline int SelectString(LPCWSTR str,int start = -1)		{return (int)SendMessage(CB_SELECTSTRING,(WPARAM)start,(LPARAM)str);}
		//展开下拉框.
		inline void ShowDropDown(bool show = 1)					{SendMessage(CB_SHOWDROPDOWN,(WPARAM)show,0);}
		//使特定项可见.
		inline bool EnsureVisible(int index)					{return (SendMessage(CB_SETTOPINDEX,(WPARAM)index,0)!=-1);}
		//设置用户可以键入的最大字符个数,默认值是30000.
		inline void SetEditLimit(int count)						{SendMessage(CB_LIMITTEXT,(WPARAM)count,0);}//count = 0 for no limit;
	};
	//CoboBoxEx Control 可以显示图标.ComboBoxEx有一个额外的风格是CBES_EX_CASESENSITIVE,会自动列出成员.
	interface  ComboBoxExPtr : virtual public WndPtr{//ComboBoxEx不支持搜索字串有关的函数.
	public:
		ComboBoxExPtr(){}
		ComboBoxExPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//设置控件的图像列表控件
		inline HIMAGELIST SetImageList(HIMAGELIST hImgl)	{return (HIMAGELIST)SendMessage(CBEM_SETIMAGELIST,0,(LPARAM)hImgl);}
		//获取控件的图像静静控件.
		inline HIMAGELIST GetImageList()					{return (HIMAGELIST)SendMessage(CBEM_GETIMAGELIST,0,0);}
		//添加一个项,str:文本,image:图像在图像列表控件中的索引,indent:图像和文字的右移宽度,1个indent大概为10个pix,
		//index:添加的位置,selImage:被选中时的图像索引,lParam:附加的参数.一个问题是当image和selImage不同时会出现显示问题.
		int AddItem(LPCWSTR str,int image = -1,int indent = 0,int index = -1,int selImage = -1,LPARAM lParam = 0);
		//删除一项.
		inline bool DeleteItem(int index)					{return (SendMessage(CBEM_DELETEITEM,(WPARAM)index,0)==-1);}
		//清空.
		inline void Clear()									{SendMessage(CB_RESETCONTENT,0,0);}
		//设置额外风格,这些风格只能用这个函数来设置,而不能在生成参数中设置.
		inline DWORD SetExStyle(DWORD cbs)					{return (DWORD)SendMessage(CBEM_SETEXSTYLE,0,(LPARAM)cbs);}
		//返回额外风格.
		inline DWORD GetExStyle()							{return (DWORD)SendMessage(CBEM_GETEXSTYLE,0,0);}
		//设置项参数.
		inline bool SetItem(PCOMBOBOXEXITEM pcbi)			{return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)pcbi)!=0;}
		//获取项参数.
		inline bool GetItem(PCOMBOBOXEXITEM pcbi)			{return (int)SendMessage(CBEM_GETITEM,0,(LPARAM)pcbi)!=0;}
		//没有办法知道要取得字串的长度,所以给text设置一个较大的Capacity,否则可能只取得字串的部分,这种情况下函数仍返回1.
		bool GetText(int index,base::String& text);
		//当index = -1时,设置的是编辑框的文本.
		bool SetText(int index,LPCWSTR text);
		//获取项的图标索引.
		int GetImage(int index,int flag = 0);// 0 for image,1 for selImage,2 for overlay.
		//设置项的图标索引.
		bool SetImage(int index,int image,int flag = 0);
		//设置平移.
		bool SetIndent(int index,int indent);
		//返回平移量
		int GetIndent(int index);
		//返回元素数量.
		inline int GetCount()								{return (int)SendMessage(CB_GETCOUNT,0,0);}
		//返回选中项的索引.
		inline int GetCurSelect()							{return (int)SendMessage(CB_GETCURSEL,0,0);}
		//设置选中项.
		inline int SetCurSelect(int index)					{return (int)SendMessage(CB_SETCURSEL,(WPARAM)index,0);}
		//设置可见项的数量.
		inline bool SetVisibleCount(int count)				{return (int)SendMessage(CB_SETMINVISIBLE,(WPARAM)count,0)!=0;}
		//获取选中项的数量.
		inline int GetVisibleCount()						{return ComboBox_GetMinVisible(_Handle);}
		//设置编辑控件的最大字符数.
		inline void SetEditLimit(int count)					{SendMessage(CB_LIMITTEXT,(WPARAM)count,0);}//count = 0 for no limit;
		//提前设置存储空间.
		inline bool ExpendStorage(int count,int strStorage)	{return SendMessage(CB_INITSTORAGE,(WPARAM)count,(LPARAM)strStorage)!=CB_ERRSPACE;}
	};
	//TreeView Control
	/*TreeView控件就是所谓的树形显示控件,它的元素以树形分布,这是一种应用非常广泛的结构,比如文件系统.TreeView控件可以绑定
	两个图像列表控件,一个用于普通状态下,一个用于选中状态下.
	interface  TreeViewPtr : virtual public WndPtr{
	public:
		TreeViewPtr(){}
		TreeViewPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加新项,parent:添加的位置,可以是TVI_ROOT.text:显示的文本,imgIndex:图标索引,selIndex:选中状态下的图标索引,insAfter:同级子项的相对位置.
		HTREEITEM AddItem(HTREEITEM parent,LPCWSTR text,int imgIndex,int selIndex,HTREEITEM tvi = 0);
		//删除子项.
		bool DeleteItem(HTREEITEM hItem){return TreeView_DeleteItem(_Handle,hItem)!=0;}
		//清除所有的项.
		bool Clear(){return TreeView_DeleteAllItems(_Handle)!=0;}
		//必须设置str足够的容量,返回值是字串的长度.
		int GetItemText(HTREEITEM hti,base::String& str);
		//设置子项的文本.
		bool SetItemText(HTREEITEM hti,LPCWSTR str);
		//取得子项的图标索引,flag可以是TVSIL_STATE和TVSIL_NORMAL之一.
		int GetImageIndex(HTREEITEM hti,UINT flag = TVSIL_NORMAL);
		//设置子项的图标索引,-1表示不设置这个索引.
		bool SetImageIndex(HTREEITEM hti,int image,int selImage = -1);
		//取得控件使用的图像列表控件的句柄,flag有两个可选值:TVSIL_NORMAL和TVSIL_STATE.
		HIMAGELIST GetImageList(UINT flag = TVSIL_NORMAL)					{return TreeView_GetImageList(_Handle,flag);}
		//设置控件使用的图像列表控件,flag有两个可选值,TVSIL_NORMAL和TVSIL_STATE.
		HIMAGELIST SetImageList(HIMAGELIST hImgl,UINT flag = TVSIL_NORMAL)	{return (HIMAGELIST)SendMessage(TVM_SETIMAGELIST,flag,(HIMAGELIST)hImgl);}//设置ImageList,可以选择删除前一个ImageList或返回其句柄.hImgl为0则解除绑定.
		//设置控件的背景色,这个函数重载了Control类的同名函数,不要用那个函数来设置背景色.-1使用系统的缺省色.
		COLORREF SetBkgndColor(COLORREF color)	{return TreeView_SetBkColor(_Handle,color);}
		//获取控件的背景色.
		COLORREF GetBkgndColor()				{return TreeView_GetBkColor(_Handle);}
		//设置文本的颜色,这个函数同样重载了Control类的同名函数.因为那个函数对这个控件不可用.color = -1表示使用系统的缺省颜色.
		COLORREF SetFontColor(COLORREF color)	{return TreeView_SetTextColor(_Handle,color);}
		//获取文本颜色.
		COLORREF GetFontColor()					{return TreeView_GetTextColor(_Handle);}
		//设置水平线的长度,如果小于允许的最小值,设为最小值.
		void SetLineIndent(int width)			{TreeView_SetIndent(_Handle,width);}
		//返回水平线的长度,缺省值大约为20.
		int GetLineIndent()						{return TreeView_GetIndent(_Handle);}
		//设置划线的颜色.
		COLORREF SetLineColor(COLORREF color)	{return TreeView_SetLineColor(_Handle,color);}
		//获取划线的颜色.
		COLORREF GetLineColor()					{return TreeView_GetLineColor(_Handle);}
		//开始编辑标签,返回编辑控件的句柄.
		HWND BeginEdit(HTREEITEM hItem)			{return TreeView_EditLabel(_Handle,hItem);}
		//完成标签编辑,cancel = 1表示还原文本.
		bool EndEdit(bool cancel = 0)			{return TreeView_EndEditLabelNow(_Handle,cancel)!=0;}
		//排序子项;all--是否对多级子项进行排序.
		bool SortChildren(HTREEITEM hti,bool all = 0)	{return TreeView_SortChildren(_Handle,hti,all)!=0;}
		//使特定的项可见.(通过展开或滚动).如果仅仅是滚动使项可见,返回1,否则返回0.
		bool EnsureVisible(HTREEITEM hItem)				{return TreeView_EnsureVisible(_Handle,hItem)!=0;}
		//展开或收起特定的项.
		bool Expand(HTREEITEM hItem,UINT tve = TVE_TOGGLE)	{return TreeView_Expand(_Handle,hItem,tve)!=0;}
		//获取绑定的编辑控件的句柄.
		HWND GetEditControl()			{return TreeView_GetEditControl(_Handle);}
		//返回项的总数.最大为16位整数的最大值.
		int GetItemCount()				{return TreeView_GetCount(_Handle);}
		//获取项的矩形.
		bool GetItemRect(HTREEITEM hItem,LPRECT rect,bool onlyText = 1){return TreeView_GetItemRect(_Handle,hItem,rect,onlyText)!=0;}
		//获取项的高度.
		int GetItemHeight(){return TreeView_GetItemHeight(_Handle);}
		//设置项的高度.
		int SetItemHeight(int height){return TreeView_SetItemHeight(_Handle,height);}		
		//设置ToolTip控件.
		HWND SetToolTipCtrl(HWND hToolTip){return TreeView_SetToolTips(_Handle,hToolTip);}
		//取得关联的ToolTip控件.
		HWND GetToolTipCtrl(){return TreeView_GetToolTips(_Handle);}		
		//设置项的参数
		bool SetItemInfo(LPTVITEMEX pti){return TreeView_SetItem(_Handle,pti)!=0;}
		//获取项的参数.
		bool GetItemInfo(LPTVITEMEX pti){return TreeView_GetItem(_Handle,pti)!=0;}
		//设置当前选定项为指定项.
		bool SetSelectItem(HTREEITEM hti){return TreeView_SelectItem(_Handle,hti)!=0;}
		//获取项的额外数据参数.
		bool GetItemData(HTREEITEM hti,LPARAM* pParam);
		//设置项的额外数据参数.
		bool SetItemData(HTREEITEM hti,LPARAM data);
		//返回根.
		HTREEITEM GetRoot()							{return TreeView_GetRoot(_Handle);}
		//返回最下面的展开项.
		HTREEITEM GetLastItem()						{return TreeView_GetLastVisible(_Handle);}
		//返回第一个子项(或0如果没有).
		HTREEITEM GetFirstChild(HTREEITEM hti)		{return TreeView_GetChild(_Handle,hti);}
		//下一个可见项(不考虑级),hitem必须可见.
		HTREEITEM GetNextVisible(HTREEITEM hti)		{return TreeView_GetNextVisible(_Handle,hti);}
		//前一个可见项(不考虑级),hitem必须可见.
		HTREEITEM GetPreVisible(HTREEITEM hti)		{return TreeView_GetPrevVisible(_Handle,hti);}
		//返回下一个同级项.
		HTREEITEM GetNext(HTREEITEM hti)			{return TreeView_GetNextSibling(_Handle,hti);}
		//返回前一个同级项.
		HTREEITEM GetPrevious(HTREEITEM hti)		{return TreeView_GetPrevSibling(_Handle,hti);}
		//返回父项的句柄;如果没有父项,返回0.
		HTREEITEM GetParent(HTREEITEM hti)			{return TreeView_GetParent(_Handle,hti);}
		//返回当前选定项.
		HTREEITEM GetSelectItem()					{return TreeView_GetSelection(_Handle);}
		//取得特定位置的项。
		HTREEITEM GetPosItem(int x,int y)			{TVHITTESTINFO ti;ti.pt.x = x;ti.pt.y = y;return TreeView_HitTest(_Handle,&ti);}
		//返回一个项的句柄,具体由tvgn确定.它实际上是前面所有函数功能的组合,而且都可以用前面的那些函数取代.
		HTREEITEM GetItem(UINT tvgn,HTREEITEM hti = 0)	{return TreeView_GetNextItem(_Handle,hti,tvgn);}
		//获取项的CheckBox选中状态，0：未选中，1：选中，-1：没有CheckBox。
		int GetItemChecked(HTREEITEM hti)			{return TreeView_GetCheckState(_Handle,hti);}
		//设置项的CheckBox选中状态，0：未选中，1：选中。
		void SetItemChecked(HTREEITEM hti,bool check)	{TreeView_SetCheckState(_Handle,hti,(UINT)check);}

	protected:
		//void _OnNotify(LPCTRLMSGPARAM pcm);
	};
	//ToolBar Control
	interface  ToolBarPtr : virtual public WndPtr{
	public:
		ToolBarPtr(){}
		ToolBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//生成控件.
		INT_PTR Create(HWND hParent);
		//添加按钮,cmdID:按钮的WM_COMMAND消息ID,image:图标在列表中的索引,text:显示的文本,tbn:按钮的风格,tbstate:按钮的状态,index:添加的位置.
		bool AddButton(WORD cmdID,int image,LPCWSTR text,BYTE tbn = BTNS_BUTTON|BTNS_AUTOSIZE,BYTE tbstate = TBSTATE_ENABLED,int index = -1);
		//添加一个分隔条,index是位置索引.
		bool AddSplit(int index = -1);
		//设置图像列表控件,ToolBar控件包含3个图像列表控件,0:普通,1:DISABLE状态,2:鼠标停留状态.
		HIMAGELIST SetImageList(int type,HIMAGELIST hImgl);
		//获取图像列表控件.0:普通,1:DISABLE状态,2:鼠标停留状态.
		HIMAGELIST GetImageList(int type);
		//返回按键的状态,通过ID索引.
		inline int GetButtonState(int id)	{return (int)SendMessage(TB_GETSTATE,id,0);}
		//设置按钮的状态,通过ID索引.
		bool SetButtonState(int id,int state,DWORD option = 1);
		//通过TBBUTTON结构可以一次添加多个按钮.
		inline bool AddButton(TBBUTTON* tbb,int count = 1){return SendMessage(TB_ADDBUTTONS,count,(LPARAM)tbb)!=0;}
		//删除特定的按钮.
		inline bool DelButton(int index){return (int)SendMessage(TB_DELETEBUTTON,index,0)!=0;}
		//Enable或Disable一个按钮.
		inline bool EnableButton(int id,bool enable = 0){return (int)SendMessage(TB_ENABLEBUTTON,id,enable)!=0;}
		//隐藏或显示一个按钮.
		inline bool HideButton(WORD id,bool hide = 1){return (int)SendMessage(TB_HIDEBUTTON,id,MAKELONG(hide,0))!=0;}
		//切换一个按钮.
		inline bool CheckButton(WORD id,bool check = 1){return (int)SendMessage(TB_CHECKBUTTON,id,MAKELONG(check,0))!=0;}//使一个按钮处于check状态,而不管这个按钮是否是一个check按钮,如果这不是一个check按钮,则不能通过点击鼠标使它处于非check状态.
		//模拟按键动作.
		inline bool PressButton(WORD id,bool pressed = 1){return (int)SendMessage(TB_PRESSBUTTON,id,MAKELONG(pressed,0))!=0;}//使一个按钮处于下压状态,可以通过点击鼠标解除这种状态.
		//
		inline bool GetAnchorHighLight(){return (int)SendMessage(TB_GETANCHORHIGHLIGHT ,0,0)!=0;}
		//
		inline bool SetAnchorHighLight(bool high){return (int)SendMessage(TB_SETANCHORHIGHLIGHT,high,0)!=0;}
		//
		bool LoadBitmap(int type,int cx,COLORREF mask,base::ResID,HINSTANCE hInst = 0,bool delpre = 1);//如果不希望前一个Image List被释放,传delpre = 0.
		//
		inline bool SetIconSize(int cx,int cy){return (int)SendMessage(TB_SETBITMAPSIZE,0,MAKELONG(cx,cy))!=0;}//它的缺省值是图标尺寸.
		//
		inline bool SetButtonSize(int cx,int cy){return (int)SendMessage(TB_SETBUTTONSIZE,0,MAKELONG(cx,cy))!=0;}//在AddButton之后调用,只能调大,不能小于图像高度.
		//
		inline base::Twin16 GetButtonSize(){return (int)SendMessage(TB_GETBUTTONSIZE,0,0);}
		//
		bool GetButtonRect(int index,LPRECT lpRect,bool byIndex = 1);
		//
		bool SetLimitButtonWidth(int min,int max){return (int)SendMessage(TB_SETBUTTONWIDTH,0,MAKELONG(min,max))!=0;}
		//
		inline UINT GetStyle(){return (int)SendMessage(TB_GETSTYLE,0,0);}
		//设置ToolBar的风格，option = 0：添加这个风格，option = 1：移除这个风格，option = 2：设置为这个风格。
		UINT SetStyle(UINT style,DWORD option = 0);
		//
		inline UINT GetExStyle(){return (int)SendMessage(TB_GETEXTENDEDSTYLE,0,0);}
		//设置ToolBar的额外风格，option = 0：添加这个风格，option = 1：移除这个风格，option = 2：设置为这个风格。
		UINT SetExStyle(UINT exStyle,DWORD option = 0);
		//
		inline bool Refresh(){return (int)SendMessage(TB_AUTOSIZE,0,0)!=0;}
		//
		inline int Count(){return (int)SendMessage(TB_BUTTONCOUNT,0,0);}
		//
		inline int IndexOf(int id){return (int)SendMessage(TB_COMMANDTOINDEX,id,0);}
		//
		inline bool SetButtonIcon(int id,int image){return (int)SendMessage(TB_CHANGEBITMAP,id,image)!=0;}//I_IMAGENONE = -2
		//
		inline int GetButtonIcon(int id){return (int)SendMessage(TB_GETBITMAP,id,0);} 
		//
		inline bool SetButtonInfo(WORD id,LPTBBUTTONINFO pbi){return (int)SendMessage(TB_SETBUTTONINFO,id,pbi)!=0;}
		//
		inline int GetButtonInfo(WORD id,LPTBBUTTONINFO pbi){return (int)SendMessage(TB_GETBUTTONINFO,id,pbi);}
		//
		bool SetButtonInfo(WORD id,DWORD tbif,INT_PTR value);
		//
		INT_PTR GetButtonInfo(WORD id,DWORD tbif);//return -1 represnt false.-2 indecate no image,除了TBIF_BYINDEX其它风格只能有一个.
		//
		inline HWND GetToolTipCtrl(){return (HWND)SendMessage(TB_GETTOOLTIPS,0,0);}
		//
		inline HWND SetToolTipCtrl(HWND hToolTip){return (HWND)SendMessage(TB_SETTOOLTIPS,hToolTip,0);}
		//
		inline int GetButtonText(int id,LPWSTR buffer){return (int)SendMessage(TB_GETBUTTONTEXT,id,buffer);}
		//
		inline base::Twin GetBarSize(){base::Twin size(0,0);SendMessage(TB_GETMAXSIZE,0,&size);return size;}
	};
	//ProgressBar Control
	interface  ProgressBarPtr : virtual public WndPtr
	{
	public:
		ProgressBarPtr(){}
		ProgressBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//返回值是上限和下限的低16值.如果上限下限值是一个32位值,用GetRange函数取得.
		inline base::Twin16 SetRange(int low,int hight){return (base::Twin16)SendMessage(PBM_SETRANGE32,low,hight);}
		//取得32位的上下限值.
		inline void GetRange(base::Twin& range){SendMessage(PBM_GETRANGE,0,&range);}
		//设置当前位置
		inline void SetPosition(int pos){SendMessage(PBM_SETPOS,pos,0);}
		//取得当前位置.
		inline uint GetPosition(){return (uint)SendMessage(PBM_GETPOS,0,0);}
		//设置最小步进.
		inline void SetIncrement(int delta){SendMessage(PBM_DELTAPOS,delta,0);}
		//设置步进增量.
		inline void SetStepValue(int step){SendMessage(PBM_SETSTEP,step,0);}
		//前进一个步进的值.
		inline void Step(){SendMessage(PBM_STEPIT,0,0);}
		//开始和停止进度条，适用于没有（PBS_MARQUEE风格）设置范围的情况。refresh是刷新周期，单位是毫秒。
		inline void SetMarquee(bool stop = 0,int refresh = 100){SendMessage(PBM_SETMARQUEE,!stop,refresh);}
	protected:
	};
	//ToolTip Control 这个控件必须在启用6.0版本控件时才可用.也就是说,必须要有MANIFEST.
	interface  ToolTipPtr : virtual public WndPtr
	{
	public:
		ToolTipPtr(){}
		ToolTipPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		INT_PTR Create(HWND hParent);
		//关闭或打开控件的提示功能.
		inline void Enable(bool enable = 1){this->SendMessage(TTM_ACTIVATE,enable,0);}
		//向一个窗口添加一个ToolTip,owner:是鼠标停留时要显示Tip的窗口,text:提示信息,id,pr,这两个参数指定窗口要显示提示
		//的区域和ID,因为要显示不同的提示,用ID区分.ttf:显示风格.重复的ID(或者HWND)参数,会覆盖上一个设置。
		bool AddTip(HWND owner,LPCWSTR text,UINT id = 0,LPRECT pr = 0,UINT ttf = 0);
		//删除一个提示。
		void DelTip(HWND owner,UINT uID = 0);
		//设置响应提示的窗口区域。
		bool SetTipRect(LPRECT pr,HWND owner,UINT uID = 0);
		//设置一个提示的信息细节,因为TI的结构比较复杂，正常的做法是先调用GetTipInfo初始化ti的值，（cbSize,hwnd 和 uId 必须初始化）
		//然后设置要改变的成员的值，再调用这个函数。
		inline bool SetTipInfo(LPTOOLINFO ti){return SendMessage(TTM_SETTOOLINFO,0,&ti)!=0;}
		//取得一个提示的信息细节,ti的cbSize成员必须初始化,hwnd 和 uId 成员也必须指定,因为信息是以它们为索引的.
		inline bool GetTipInfo(LPTOOLINFO ti){return 0!=SendMessage(TTM_GETTOOLINFO,0,ti);}
		//只有当TTS_BALLOON风格被设置时，这个函数才可用。title:要显示的标题文本,tti：要显示的图标，0，没有图标，1，信息，2，警告，3，错误。
		inline bool SetTitle(LPCWSTR title,int tti = TTI_NONE){return 0!=SendMessage(TTM_SETTITLE,tti,title);}
		//设置提示的文本。
		bool SetTipText(LPCWSTR text,HWND owner,UINT id);
		//设置提示的风格。
		bool SetTipFlag(UINT ttf,HWND owner,UINT id);
		//设置提示文本颜色。
		inline bool SetTextColor(COLORREF color){return 0!=SendMessage(TTM_SETTIPTEXTCOLOR,color,0);}
		//设置提示窗口的背景颜色。
		inline bool SetBkColor(COLORREF color){return 0!=SendMessage(TTM_SETTIPBKCOLOR,color,0);}
		//设置提示窗口的宽度。
		inline int SetTextWidth(UINT width){return (int)this->SendMessage(TTM_SETMAXTIPWIDTH,0,width);}
		//设置显示提示的延迟时间。
		inline void SetDelayTime(UINT ms,UINT ttdt = TTDT_INITIAL){this->SendMessage(TTM_SETDELAYTIME,ttdt,ms);}
		//强制提示窗口重新绘制。
		inline void UpDate(){SendMessage(TTM_UPDATE,0,0);}
		//设置提示的显示时间参数，单位是毫秒。flag:2-show,3-delay,1-reshow，0-all default.
		inline void SetTimeInfo(int t,UINT flag){SendMessage(TTM_SETDELAYTIME,flag,t);}
		//取回时间参数，flag:2-show,3-delay,1-reshow
		inline int GetTimeInfo(UINT flag){return (int)SendMessage(TTM_GETDELAYTIME,flag,0);}
	};

	//TabPage控件本身并不提供对控件的操作,它只提供一个框架,然后点击页按钮会引发相应的消息事件.在这些事件中处理控件的行为,使它表现的类似于处在不同的页面.
	interface  TabPagePtr : virtual public WndPtr
	{
	public:
		TabPagePtr(){}
		TabPagePtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//插入一个属性页,index是插入的位置索引,title是显示的标题,image是图标在图标列表控件中的索引,param是一个用户自定义参数.
		bool Add(int index,LPWSTR title = 0,int image = -1,LPARAM param = 0);
		//删除指定索引的项.
		bool Delete(int index){return 0!=SendMessage(TCM_DELETEITEM,index,0);}
		//清空所有属性页.
		bool Clear(){return 0!=SendMessage(TCM_DELETEALLITEMS,0,0);}
		//返回当前属性页的索引.
		int GetCurSel(){return (int)SendMessage(TCM_GETCURSEL,0,0);}
		//设置指定索引的属性页为当前页.
		void SetCurSel(int index){SendMessage(TCM_SETCURSEL,index,0);}
		//设置图像列表控件.
		HIMAGELIST SetImageList(HIMAGELIST imgl){return (HIMAGELIST)SendMessage(TCM_SETIMAGELIST,0,imgl);}
		//取得图像列表控件.
		HIMAGELIST GetImageList(){return (HIMAGELIST)SendMessage(TCM_GETIMAGELIST,0,0);}
	};
	typedef union HotkeyParam
	{
		struct{
			unsigned char VirtualKey,CombineKey;
		};
		struct{
			unsigned short Value;
		};
		struct{
			WPARAM wParam;
		};
		void* operator new(size_t size){return base::MemoryAlloc((int)size);}
		void operator delete(void* p){base::MemoryFree(p);}
	}HotkeyParam;
	//表示组合键的一个结构.
	//热键控件
	interface  HotKeyPtr : virtual public WndPtr
	{
	public:
		HotKeyPtr(){}
		HotKeyPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//这个函数设置无效的键组合,hkcomb是要屏蔽的组合键,可以是多个HKCOMB_类常数的组合,当按下这些组合时,会被指定的hotkeyf取代.
		void SetRules(int hkcomb,int hotkeyf){SendMessage(HKM_SETRULES,MAKEWPARAM(hkcomb,0),MAKELPARAM(hotkeyf,0));}
		//设置组合键.
		void SetHotKey(unsigned char comb,unsigned char vk);
		//设置组合键.
		void SetHotKey(HotkeyParam hk){SendMessage(HKM_SETHOTKEY,hk.wParam,0);}
		//取得组合键,低位是键,高位是组合.
		HotkeyParam GetHotKey();
		//绑定到一个窗口,不是能是子窗口.
		bool Register(HWND frame){return (int)SendMessage(WM_SETHOTKEY,GetHotKey().wParam,0)>0;}
	};
	interface  ListViewPtr : virtual public WndPtr
	{
	public:
		ListViewPtr(){}
		ListViewPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//添加一个列，返回列的索引位置或者-1，参数：列标题名，宽度，位置（-1表示添加到最后）。
		int AddColumn(LPWSTR title,int width,int index = -1);
		//添加一个项目，参数，第一列的文本，列位置（-1）表示添加到最后，返回位置索引或-1.
		int AddItem(LPWSTR text,int index = -1);
		//设置一个条目的文本，参数：文本，行索引，列索引。
		bool SetItemText(LPWSTR text,int index,int column);
		//删除指定索引的列，但是0索引列无法删除，如果要删除，先插入一个0宽度的列，再删除。
		bool DelColumn(int index){return 0!=this->SendMessage(LVM_DELETECOLUMN,index,0);}
		//删除指定行。
		bool DelItem(int index){return 0!=this->SendMessage(LVM_DELETEITEM,index,0);}
		//情况所有。
		bool Clear(){return 0!=this->SendMessage(LVM_DELETEALLITEMS,0,0);}
		//编辑标签。
		HWND EditItem(int index){SetFocus(_Handle);return (HWND)this->SendMessage(LVM_EDITLABEL,index,0);}
		//取消标签编辑。
		void CancelEdit(){this->SendMessage(LVM_CANCELEDITLABEL,0,0);}
		//返回列的总数。
		int GetColumnCount();
		//返回行的总数。
		int GetItemCount(){return (int)this->SendMessage(LVM_GETITEMCOUNT,0,0);}
		//取得指定列column和行index的文本。
		int GetItemText(base::String& text,int index,int column);
		//取得满足特定条件的项的索引，falg 用来确定项的类型和选项，比如 LVNI_SELECTED 用来取得选中项的索引。
		//after 指定搜索从这一索引项后面开始，-1 表示搜索所有项，注意如果这个值是 0 ，它不搜索第0项。
		int GetItemIndex(DWORD flag = LVNI_SELECTED,int after = -1){return (int)SendMessage(LVM_GETNEXTITEM,after,flag);}
		//
		//
		//
		//
		//
	};
	interface  StatusBarPtr : virtual public WndPtr
	{
	public:
		StatusBarPtr(){};
		StatusBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//设置 StatusBar 分为几个部分，这个函数可以反复调用，以最后一次调用为准，多于原来的则增加分段，
		//少于原来的则删除后面多的分段。width 是一个整数数组，长度为 parts。每个数组元素的值对应每个分
		//段的右边界，单位是像素，如果某个元素比前面的值小，意味着这个段会被前面的段遮挡而不可见。如果
		//这个值是-1，则表示这个段直到控件的末尾（后面的段都将被遮挡）。
		bool SetParts(int parts,int* width){return 0!=SendMessage(SB_SETPARTS,parts,width);}
		//设置段的文本，index 是段的索引，type 是文字的显示风格，比如 SBT_POPOUT 。
		bool SetText(int index,LPCWSTR text,int type = 0){return 0!=SendMessage(SB_SETTEXT,index|type,text);}
	};
	interface  UpDownPtr : virtual public WndPtr
	{
	public:
		UpDownPtr(){}
		UpDownPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}

		//设置关联窗口，一般是一个文本窗口，返回前一个关联控件句柄。
		HWND SetBuddy(HWND buddy){return (HWND)SendMessage(UDM_SETBUDDY,buddy,0);}
		//取得关联窗口。
		HWND GetBuddy(){return (HWND)SendMessage(UDM_GETBUDDY,0,0);}
		//设置范围。
		void SetRange(int nLower,int nUpper){SendMessage(UDM_SETRANGE32,nLower,nUpper);}
		//取得范围。
		base::Twin GetRange(){base::Twin rg;SendMessage(UDM_GETRANGE32,&rg.x,&rg.y);return rg;}
		//设置位置。
		int SetPosition(int nPos){return (int)SendMessage(UDM_SETPOS32,0,nPos);}
		//取得位置，实际上这个数值是从buddy窗口取得的，所以如果buddy窗口不存在或者文本不是一个数字，返回的值是不可用的。
		int GetPosition(){return (int)SendMessage(UDM_GETPOS32,0,0);}
	};
	interface  TrackBarPtr : virtual public WndPtr
	{
		TrackBarPtr(){}
		TrackBarPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		void SetRange(WORD min,WORD max){SendMessage(TBM_SETRANGE,1,MAKELPARAM(min,max));}
		void SetPos(LONG pos){SendMessage(TBM_SETPOS,1,pos);}
		int GetPos(){return (LONG)SendMessage(TBM_GETPOS,0,0);}
	};
	interface  HeaderPtr : virtual public WndPtr{
		HeaderPtr(){}
		HeaderPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//set header control size to appropriate parent window dimension,
		//return the control height.when width<0,width will equal parent client width.
		int AdjustSize(HWND parent,int x = 0,int y = 0,int width = -1);
		//insert a item to header control,return the new column item position index or -1 if faild.
		int AddItem(LPCWSTR text,int width,int index = -1);
		//delete item of index.
		bool DelItem(int index){return 0!=SendMessage(HDM_DELETEITEM,(WPARAM)index,0);}
		//set header control column text and width.
		bool SetItem(int index,LPCWSTR text,int width);
		//get column's width and title text,return width of column.
		int GetItem(int index,base::String& text);
		//get count of all column.
		int GetCount(){return (int)SendMessage(HDM_GETITEMCOUNT,0,0);}
		//set item width.
		bool SetItemWidth(int index,int width);
		//set item text
		bool SetItemText(int index,LPCWSTR title);
		//set item feature
		bool SetItem(int index,HDITEM* hdi);
		//get item info.
		bool GetItem(int index,HDITEM* hdi);
	};
	interface  LinkPtr : virtual public WndPtr{
		LinkPtr(){}
		LinkPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	interface  IpAddressPtr : virtual public WndPtr{
		IpAddressPtr(){}
		IpAddressPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	//控件的基类，和View不同，控件有自己的窗口类，事实上直到控件生成完成，我们都接收不到消息。
	//OnCreate是个伪消息回调，它是直到Create完成之后才调用的，而不是响应WM_CREATE消息。
	*/
	/*
	//下面的控件虽然从ICtrl和Ptr继承，且没有增加占用空间的成员，但是不能把一个 new ICtrl 对象的指针
	//转换为下面控件的指针，它们的大小是不同的；如果这个iCtrl指针不是  new ICtrl 产生的，而是比如 new ListBox
	//则可以转换为 ListBox指针。

	//class Label
	class Label : public ICtrl,public LabelPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Label();
	};
	//Button Control
	/*只有XP风格的按钮才可以设置图像列表控件,如果图像列表控件只有一个图像,则显示这个图像.如果有多于1个图像则应该有
	5个图像,分别在正常,有焦点,鼠标停留,DISABLE状态和按下状态下显示

	BS_GROUPBOX风格的Button因为背景透明（不绘制背景），父窗口不能使用WS_CLIPCHILDREN风格。

	
	class Button : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Button();
	};
	class GroupBox : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		GroupBox();
	};
	class CheckBox : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		CheckBox();
	};
	class Radio : public ICtrl,public ButtonPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Radio();
	};
	//Edit Control
	class Edit : public ICtrl,public EditPtr
	{
		void _CtrlColor(CtrlMsg*);
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Edit();
	};
	//DateTimePicker Control
	class DateTimePicker : public ICtrl,public DateTimePickerPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		DateTimePicker();

		//base::Function<void,SystemTime*> OnChanged;
	protected:
	};
	//Calendar Control
	class MonthCalendar : public ICtrl,public MonthCalendarPtr
	{
	public:
		MonthCalendar();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//ListBox Control 有3种风格分别是单选(默认),连续多选(LBS_EXTENDEDSEL,可以用SHIFT来选中一个区域),任意多选(LBS_MULTIPLESEL,可以点击来选取或取消任意一项)
	class ListBox : public ICtrl,public ListBoxPtr
	{
	public:
		ListBox();
		void operator = (HWND hWnd){Attach(hWnd);}
	protected:
	};
	//ComboBox Control 本身包含一个ListBox控件,所以这两个控件的很多函数是很相似的.
	class ComboBox : public ICtrl,public ComboBoxPtr
	{
	public:
		ComboBox();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//CoboBoxEx Control 可以显示图标.ComboBoxEx有一个额外的风格是CBES_EX_CASESENSITIVE,会自动列出成员.
	class ComboBoxEx : public ICtrl,public ComboBoxExPtr//ComboBoxEx不支持搜索字串有关的函数.
	{
	public:
		ComboBoxEx();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//TreeView Control
	/*TreeView控件就是所谓的树形显示控件,它的元素以树形分布,这是一种应用非常广泛的结构,比如文件系统.TreeView控件可以绑定
	两个图像列表控件,一个用于普通状态下,一个用于选中状态下.
	class TreeView : public ICtrl,public TreeViewPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		/*base::Function<void,LPNMTREEVIEW> OnBeginDrag;	//开始拖曳
		base::Function<void,LPNMTREEVIEW> OnBeginRDrag;	//开始右键拖曳
		base::Function<bool,LPNMTVDISPINFO> OnBeginEdit;	//开始编辑标签,返回1,则取消编辑标签.
		base::Function<bool,LPNMTVDISPINFO> OnEndEdit;	//完成编辑标签,返回1,则取消编辑文本.
		base::Function<void,LPNMTREEVIEW> OnDeleteItem;	//删除元素.
		base::Function<void,LPNMTREEVIEW> OnExpanded;		//展开或折叠了一个子项.
		base::Function<bool,LPNMTREEVIEW> OnExpanding;	//正要展开或折叠一个子项,返回1取消动作.
		base::Function<void,LPNMTREEVIEW> OnSelChanged;	//当前选择项变化.
		base::Function<bool,LPNMTREEVIEW> OnSelChanging;	//正要改变当前选择项,返回1阻止动作.
		base::Function<UINT,LPNMTVKEYDOWN> OnChar;		//有按键动作,可以执行搜索操作,详见MSDN说明.
		
		TreeView();
	};
	//ToolBar Control
	class ToolBar : public ICtrl,public ToolBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ToolBar();
		//生成控件.
		INT_PTR Create(HWND hParent);
	private:
		void _OnParentSize(Msg*);
	};
	//ProgressBar Control
	class ProgressBar : public ICtrl,public ProgressBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ProgressBar();
	};
	//ToolTip Control 这个控件必须在启用6.0版本控件时才可用.也就是说,必须要有MANIFEST.
	class ToolTip : public ICtrl,public ToolTipPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ToolTip(void);
		INT_PTR Create(HWND hParent);
	};

	//TabPage控件本身并不提供对控件的操作,它只提供一个框架,然后点击页按钮会引发相应的消息事件.在这些事件中处理控件的行为,使它表现的类似于处在不同的页面.
	class TabPage : public ICtrl,public TabPagePtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		TabPage();
	};
	//表示组合键的一个结构.
	//热键控件
	class HotKey : public ICtrl,public HotKeyPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		HotKey();
	};
	class ListView : public ICtrl,public ListViewPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ListView();
		//ListView 的扩展风格必须生成之后调用这个函数来设置，在生成窗口时的风格和扩展风格中设置都不起作用。add表示添加还是移除exStyle指定的风格。
		DWORD SetExStyle(DWORD exStyle,bool add = 1);
	};
	class StatusBar : public ICtrl,public StatusBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		StatusBar();
	};
	class TrackBar : public ICtrl,public TrackBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		TrackBar();
	};
	//UpDown 控件会向父窗口发送滚动条消息
	class UpDown : public ICtrl,public UpDownPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		UpDown();
		//直接生成一个 UDS_SETBUDDYINT UDS_ALIGNRIGHT UDS_ARROWKEYS 风格的 UpDown 控件，nUpeer：上限；nLower：下限；nPos：当前位置。
		INT_PTR Create(HWND buddy,HWND parent,int nUpper,int nLower,int nPos);
		INT_PTR Create(HWND parent);
	};
	class  Header : public ICtrl,public HeaderPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Header();
	};
	class  Link : public ICtrl,public LinkPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Link();
	};
	class  IpAddress : public ICtrl,public IpAddressPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		IpAddress();
	};
	/*对话框最多可以预先放置255个控件,更多的控件必须在初始化对话框的时候手动生成.

	对话框仍然是一个窗口，实际上对话框是常规窗口的一种包装，主要改变了两个地方：
	1. 生成函数不调用CreateWindow或CreateWindowEx，而是重新包装好的函数：
	DialogBoxParam（模态对话框）和CreateDialogParam（非模态对话框），而且不需要
	注册类名，其实对话框就是一个特定的窗口类，和其它内置控件一样预先注册好了。
	2. 同样窗口函数也进行了包装，对话框的窗口函数虽然形式上和普通窗口是一样的，
	而且大多消息也是未经处理就发给这个函数，但是对话框不发送很多消息，比如WM_CREATE，
	而是用WM_INITDIALOG代替。而且对话框并没有一个DefDialogProc函数供缺省返回，
	对话框窗口函数返回值一般是FALSE，那么对话框会按内部的缺省方式处理，反之返回
	TRUE，那么内部就忽略这个消息，不做任何处理。

	有些特定消息需要返回其他值，比如WM_CTRLCOLORDLG这消息，返回一个画刷句柄，那么
	这个画刷将用来绘制对话框的背景，而且对话框就不再发送WM_ERASEBKGROUND消息。

	IDlg 和 IWnd 走的消息流程完全相同，但是IDlg最终的消息返回时不调用DefWindowProc，
	而是调用_defDlgProc一个直接返回 0 的函数。

	模态对话框的关闭按钮必须在WM_COMMAND（OnCommand）里的IDCANCEL分支里明确调用Close
	函数，否则不会自动关闭。//
	class  IDlg : public IWnd{
	private:
		HRESULT _defDlgProc(HWND hWnd,UINT Msg,WPARAM wParam,LPARAM lParam);
		base::Function<HRESULT,HWND,UINT,WPARAM,LPARAM> _defDlgFunc;
	protected:
		void _MsgHookProc(Msg* msg);
	public:
		IDlg();
		HWND	GetNextGroupItem(HWND ctrl,bool pre = 0){return ::GetNextDlgGroupItem(_Handle,ctrl,pre);}
		HWND	GetNextTabItem(HWND ctrl,bool pre = 0){return ::GetNextDlgTabItem(_Handle,ctrl,pre);}
		int		GetCtrlID(HWND ctrl){return GetDlgCtrlID(ctrl);}
		HWND	GetCtrlHandle(int id){return GetDlgItem(_Handle,id);}
		int		GetCtrlInt(int id,bool sign = 1,int* bSuccess = 0){return ::GetDlgItemInt(_Handle,id,bSuccess,sign);}
		int		GetCtrlText(int id,base::String& str);
		bool	SetCtrlInt(int id,int value,bool sign = 0){return SetDlgItemInt(_Handle,id,value,sign)!=0;}
		bool	SetCtrlText(int id,const wchar_t* text){return SetDlgItemText(_Handle,id,text)!=0;}
	};
	//模态对话框，模态对话框有自己的消息循环。它运行期间，主消息循环接收不到消息。
	class  ModelDialog : public IDlg
	{
	public:
		ModelDialog();
		//生成一个模态对话框,直到对话框销毁之前,这个函数不会返回.正常情况下的返回值是调用EndDialog设置的值.如果因为hParent
		//参数不可用,返回值为0,其它错误返回-1.即使对话框的属性为不可见,这个函数也会显示对话框.
		INT_PTR Create(HWND hParent = 0);
		//关闭对话框，code指定退出代码.
		bool Close(int code = 0);
	};
	//非模态对话框.
	class  NormalDialog : public IDlg
	{
	public:
		NormalDialog();
		//生成一个非模态对话框,返回对话框的句柄.
		INT_PTR Create(HWND hParent = 0);
		//关闭对话框.
		bool Close();
	};*/
class WndPosRestore : public base::_class{
	base::IntegerList<IWnd*> wndList;
	base::StringMemList	wndName;
	void _OnCreate(Msg*);
	void _OnDestroy(Msg*);
public:
	WndPosRestore();
	~WndPosRestore();
	//wnd是需要定位記錄的窗口，name是指定一個名稱，因為記錄需要用這個名稱來區分窗口。
	bool AddWnd(IWnd* wnd,LPCWSTR name);
	//如果WndPosRestore先于它添加的窗口銷毀，必須調用這個函數移除對它的成員函數的回調。
	bool RemoveWnd(IWnd* wnd);
	//对于没有HOOK WM_CRAET消息的窗口在合适的时候调用这个函数模拟
	void OnCreate(HWND hwnd);
	//对于没有HOOK WM_DESTROY消息的窗口在合适的时候调用这个函数模拟
	void OnDestroy(HWND hwnd);
};
//注册一个wnd对象，这对象必须能够接收WM_CREATE和WM_DESTROY消息，在这两个消息里分别设置和记录窗口的位置大小参数。
void WPRRegisterWnd(IWnd* wnd,LPCWSTR name);
//解除一个wnd对象的自动位置大小功能。
void WPRUnRegisterWnd(IWnd* wnd);
void LoadWndFunc(Handle<Object>& glb);
}