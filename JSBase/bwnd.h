#pragma once
namespace v8{
#define WM_CTRLUSER			0x8000					//�ؼ�����Ϣת����ϢID
#define WM_CTRLCOLOR		WM_CTRLUSER + 0x0132	//�ؼ�����ɫ��Ϣ.
#define WM_GETWINDOW		WM_CTRLUSER + 0x01		//��HWND��ȡWindow*ָ��.
#define WM_CTRLMSG			WM_CTRLUSER + 0x02
	extern  __declspec(thread) HWND	_Dlg;	//�Ի�������
	extern __declspec(thread) HACCEL	_Accel;	//���ټ���
	//��ʼ�̵߳�����Ϣѭ��.
	int MLStart();
	//���õ�ǰ�߳��жԻ���ľ������ʹ���ܹ���Ӧ Tab ����
	inline void MLSetDialog(HWND hDlg){_Dlg = hDlg;}
	//���õ�ǰ�̵߳ļ��ټ���
	inline void MLSetAccelerator(HACCEL hacl){_Accel = hacl;}

	//���ں����ķ���ֵ����.
	class  Result : public base::HandleType<INT_PTR>{
	public:
		enum{RESULT_DEFAULT = 0x1,WNDPROC_DISABLE = 0x2};
		INT_PTR		_Flag;	//����λ������ĳЩ�ڲ�Ӧ�á�
		Result():_Flag(0){};
		inline void operator = (INT_PTR result){_Flag |= RESULT_DEFAULT;_Handle = result;}
		//ȱʡ����ֵ����ֵΪtrue��Wndproc��������Ϣ�������ô��ڵ�ȱʡԭ�ڲ����������ҷ������ķ���ֵ��
		//��ֵΪfalse��Wndproc��������Ϣ�������û����� = �������õ�ֵ�������ô��ڵ��ڲ�������
		inline bool Default(){return (_Flag&RESULT_DEFAULT)==0;}
		//��������Ϊȱʡ״̬��
		inline void SetDefault(){_Flag &= ~RESULT_DEFAULT;}
		//�û��Լ�������Ϣ��������Wndproc�Ĵ�����롣Wndproc�ڲ�ֻ�����˺��ٵ�������Ϣ��
		//��������û�һ�㲻��Ҫ���á�
		inline bool _DisableWndproc(){return (_Flag&WNDPROC_DISABLE)!=0;}
		//�����Ƿ����Wndproc����Ϣ�Ĵ�����롣
		inline void _SetDisableWndproc(bool disable){if(disable) _Flag|=WNDPROC_DISABLE;else _Flag&=~WNDPROC_DISABLE;}
	};
	//Windows ��Ϣ�ṹ
	typedef struct  Msg : public base::_struct{
		HWND		hWnd;		//��Ϣ�Ĵ��ھ��.
		UINT		uMsg;		//��ϢID
		WPARAM		wParam;		//����
		LPARAM		lParam;		//����
		Result		Result;	//����ֵ.
	}Msg;
	//���ṹ,���������Ϣ.
	typedef struct  Mouse : public base::_struct{
		Mouse(){}
		Mouse(Mouse& m){*this = m;}
		void operator = (Mouse& m){Event = m.Event;Button = m.Button;WheelDelta = m.WheelDelta;X = m.X;Y = m.Y;Msg=m.Msg;}
		UINT Event;			//��Ӧ�������Ϣ.
		short Button,		//��������,������0,���統Event��WM_MOUSEMOVEʱ.MK_����
			WheelDelta;		//������ֹ���ʱ(Event==WM_MOUSEWHEEL),���ֵ��120��-120,������ķ����й�.
		short X,Y;			//ָ���λ��.
		Msg* Msg;
	}Mouse;
	//�ǿͻ�ȥ���ṹ
	typedef struct  NcMouse : public base::_struct{
		UINT uMsg;		//��ϢID
		UINT hitTest;	//HT������֪ʶ����״̬��
		short x,y;		//��Ļ����
		Result* lpResult;	//����ֵ��
	}NcMouse;
	typedef struct  CtrlMsg : public base::_struct{
		HWND		hWnd;		//��Ϣ�Ĵ��ھ��.
		UINT		uMsg;		//��ϢID,WM_NOTIFY,WM_COMMAND,WM_CTRLCOLOR֮һ��
		WPARAM		wParam;		//����
		LPARAM		lParam;		//����
		Result*		pResult;	//����ֵ.
	}CtrlMsg;
	//��������ʱ�õ��Ľṹ,���ڳ�ʼ�����ڵı�Ҫ����.
	typedef struct  CreateStruct : public base::_struct{
		CreateStruct():ClassStyle(CS_DBLCLKS),Style(WS_VISIBLE),ExStyle(0),Menu(0),lParam(0),Location(200,200),Size(500,300){}
		base::String ClassName,
			Title;					//���ڵı���,ֻ�Զ���������Ч.
		base::Twin16 Location,Size;	//����λ��,��С.
		DWORD ClassStyle,Style,ExStyle;	//���ڵ�������
		union{
			HMENU		Menu;				//�˵�(����������) 
			WORD		Identity;			//ID(�����Ӵ���).
		};				
		base::ResID		Dialog;				//���ڶԻ���
		LPVOID lParam;					//�������.
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
	//���ڶ���Ļ��࣬��������ϵͳ���Զ��崰���࣬�ؼ����Ի���ȫ����������࣬�����̳������.
	//Ptr��׺���඼�ǰ�һ��ָ����󣬴�С�������ָ��Ĵ�С��������ֻ���ɺ��������������ݡ�
	class  WndPtr : public base::HandleType<HWND>{
	public:
		WndPtr(){}
		WndPtr(HWND hwnd){_Handle = hwnd;}
		//��һ�����ڡ�
		void operator = (HWND hwnd){_Handle = hwnd;}
		//���ش��ڵ�����,���ڶ������ڶ�Ӧ��Ļ����,�����Ӵ���,��Ӧ����������.
		bool GetRect(base::Rect& r);		
		//���ش��ڵĿͻ����ߴ�.�����������Ͻ�����һ����(0,0),��Ҳ���ܲ���,����б߿�Ļ�.�����ľ�������û��һ��API���������ṩ.����ʹ��Window��ĳ�Ա����ȡ.
		bool GetClientSize(base::Twin16& c);						
		//���ô�С�����ı�λ��.
		inline bool SetSize(int cx,int cy) {return ::SetWindowPos(_Handle,0,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER)!=0;}
		//����λ�ã����ı��С
		inline bool SetLocation(int x,int y){return ::SetWindowPos(_Handle,0,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER)!=0;}
		//���ô�С��λ�á�
		inline bool SetDimension(int x,int y,int cx,int cy) {return ::MoveWindow(_Handle,x,y,cx,cy,1)!=0;}
		//�ƶ����ڻ�������ڴ�С,dx,dy�Ǵ���λ�õ�ƫ����,cx,cy�Ǵ�С�ı仯��.
		void Offset(int dx, int dy, int cx, int cy);	

		//ȡ���ı��ִ��ĳ��ȣ���������β 0 �ַ���
		inline int TextLength(){return ::GetWindowTextLength(_Handle);}
		//ȡ�ô����ı���text���س�ʼ����
		int GetText(base::String& text);
		//���ô����ı���
		inline bool SetText(LPCWSTR text){return ::SetWindowTextW(_Handle,text)!=0;}
		//���������ü�ʱ��,�ɹ��󴰿ڻᶨʱ����WM_TIMER��Ϣ,ms�Ǽ��ʱ��(����),id�Ǽ�ʱ���ı�ʶ,������id��֮ǰ���õ�һ��timer�����滻ԭ���ġ������޴��ڰ󶨵ļ�ʱ�����Ҳ����滻���е�timer��Ӧ�ø�id��0ֵ����ϵͳ������һ��id�������������id��
		inline UINT_PTR SetTimer(UINT ms,UINT_PTR id){return ::SetTimer(_Handle,id,ms,NULL);}
		//ɾ��һ��timer��id������ʱ���ص�id��
		inline bool KillTimer(UINT_PTR id){return ::KillTimer(_Handle,id)!=0;}
		//Ҫ�󴰿��ػ�ָ��������,erase�Ƿ��ػ汳��,Ҳ�����Ƿ�����WM_ERASEBKGND��Ϣ.
		inline bool Invalidate(const RECT* clip = 0,bool erase = 1){return ::InvalidateRect(_Handle,clip,erase)!=0;}
		//Ҫ�󴰿��ػ�ָ��������,erase�Ƿ��ػ汳��,Ҳ�����Ƿ�����WM_ERASEBKGND��Ϣ.
		inline bool Invalidate(const HRGN hRgn,bool erase = 1){return ::InvalidateRgn(_Handle,hRgn,erase)!=0;}
		//���ش��ڲ�����
		inline LONG_PTR GetWindowParam(int gwl){return ::GetWindowLongPtr(_Handle,gwl);}
		//���ô�����ز���.
		inline LONG_PTR SetWindowParam(int gwl,LONG_PTR value){return ::SetWindowLongPtr(_Handle,gwl,(LONG)value);}
		//���ڵĸ����ھ��.
		inline HWND Parent(){return ::GetParent(_Handle);}
		//��Ϣ����,�и����ɵĲ�������,�����������ʹ�õĲ��������Ƿǳ����.
		template<typename T,typename R> inline LRESULT SendMessage(UINT ttm,T wParam,R lParam){return ::SendMessage(_Handle,ttm,(WPARAM)wParam,(LPARAM)lParam);}
		//Enable ���� Disable һ�����ڡ�
		inline bool Enable(bool enable = 1){return ::EnableWindow(_Handle,enable)!=0;}//return value is pre state of disabled.
		//�򴰿ڷ���WM_CLOSE��Ϣ������ر�û�б�ȡ������true����֮����false��
		bool Close(){return 0!=SendMessage(WM_CLOSE,0,0);}
		//���Ĵ��ڵķ��
		void ModifyStyle(DWORD dwRemove,DWORD dwAdd);
		//���Ĵ��ڵ���չ���
		void ModifyExStyle(DWORD dwRemove,DWORD dwAdd);
		//���ô��ڵĿ�ȡ�
		bool SetWidth(int cx);
		//���ô��ڵĸ߶ȡ�
		bool SetHeight(int cy);
		//���ô��ڵ�ˮƽλ�á�
		bool SetXLocation(int x);
		//���ô��ڵĴ�ֱλ�á�
		bool SetYLocation(int y);
	};
	//�ػ񴰿���Ϣ�ľ�̬��������.
	typedef void (__stdcall*MSGPROC)(Msg*); 
	//Class IWnd ���ṩ�����Ĵ��ڽṹ����Ҫʵ��������ࡣ
	interface  IWnd : public WndPtr{//public HandleType<HWND>//virtual 
	public:
		//��Ϊ��һ���ⲿ����Ҫ�����������,��һ���������������������ⲿ����,������"_"����ʶ.
		LRESULT _WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
		base::Function<LRESULT,HWND,UINT,WPARAM,LPARAM> _WndPointer;	//���ں���.
	private:
		typedef struct _UPOINT : base::_struct{
			UINT x,y;
			bool operator == (_UPOINT up){
				return (x==up.x)&&(y==up.y);
			}
		}_UPOINT;
		base::Diction<_UPOINT> _MsgList;
		base::ObjectList<base::Delegate<Msg*> > _ProcList;
		bool _mLeave;					//����Ƿ��뿪.
	protected:
		base::Twin		_DragMinSize;		//������קʱ����С�ߴ硣
		base::Twin		_DragMaxSize;		//������קʱ����С�ߴ硣
		base::Twin		_MinSize;			//û�й���������С��С.
		base::Twin		_Scr;				//ƫ����
		base::gdi::Color	_FontColor;			//�ؼ���ǰ��ɫ(������ɫ).
		base::gdi::Font	_Font;				//�����Ա������ʾ������.
		base::gdi::Brush	_BkgndColor;		//����ɫ
		base::Twin16		_ClientLocation;	//Client Location;
		LONG_PTR	_QuondamProc;		//����ԭ���Ĵ��ں���.����һ���û��Լ�����Ĵ���,��������޹ؽ�Ҫ,һ����DefWndProc������,����һ��Windows�ڲ��Ĵ���,����Text�ؼ�,�������������ȷ����.
		HCURSOR		_Cursor;			//���ָ����״

	public:
		IWnd(void);
		~IWnd();
		base::Object<CreateStruct>	Param;		//�����Ա�������ô�������ʱ�Ĳ���,�����Լ���Ĭ�ϲ���,ֻ��Ҫ������ı�Ĳ���.
		base::Delegate<WORD>		OnCommand;	//��Ϊ����¼��ᱻƵ����ʹ��,���Լ�����.
		base::Delegate<>			OnCreate;	//WM_CREATE��Ϣ�����������ɿؼ���ʱ������Ϣ��ʵ��������ɺ�ģ�������һ�Ρ�
		base::Delegate<Mouse*>		OnMouse;	//MouseEvent�������������Ϣ.		
		base::Delegate<NcMouse*>	OnNcMouse;	//OnNcMouse����ǿͻ����ģ���WM_NCMOUSELEAVE�����������Ϣ.ʵ����������Ϣ����WM_NCHITTEST����DefWindowProc���������ġ�
		base::Function<void,bool&>	OnClose;	//�ػ�WM_CLOSE��Ϣ��boolֵĬ��Ϊfalse�������Ϊtrue���򴰿ڹرն���ȡ����
		
		//����ƫ��
		inline base::Twin Scr(){return _Scr;}
		//���ֹ���������С�ߴ�
		inline base::Twin MinSize(){return _MinSize;}
		//�ͻ����ڴ��������е�λ��.;
		inline base::Twin16 GetClientPos() const {return _ClientLocation;}
		//���ض�Ӧ��Ϣ�Ĵ���ָ��,���Դ����ָ����ӻ�ɾ����Ӧ�ĺ���.
		base::Delegate<Msg*>* GetMsgHook(UINT msg);
		//���ض�Ӧ��Ϣ�Ĵ���ָ��,���Դ����ָ����ӻ�ɾ����Ӧ�ĺ���.
		base::Delegate<Msg*>* GetMsgHook(UINT msg1,UINT msg2);
		//���ÿؼ��ı���ɫ.��Ϊ-1��ʾ����͸��(����������ػ�����ʱ,�������γ��ص���ʾ�ַ�).
		void SetBkgndColor(COLORREF color);
		//���ÿؼ���ǰ��ɫ(������ɫ).
		void SetFontColor(COLORREF color);
		//���ÿؼ�ʹ�õ�����,ʵ���ϵ����������һ�ξͻ���������һ������.
		bool SetFont(bool reDraw = 0);
		//ȡ�õ�ǰ�������.
		base::gdi::Font* GetFont(){return &_Font;}
		//����һ�����ڣ��������������������Param��ClassName��������������������ڵ����ͣ�������Զ��崰�ڣ������Լ�ע������ࡣ��������ʧ�ܡ�
		//virtual INT_PTR Create(HWND parent){return 0;}
		//�����Ϸ�ʱ����С�ߴ�
		void SetMinDragSize(int cx,int cy){_DragMinSize.SetValue(cx,cy);}
		//�����Ϸ�ʱ�����ߴ�
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
			BK_NONE	= 0, //����ʾ�κα���
			BK_COLOR	= 1,	//��ʾ������ɫ
			BK_IMAGE	= 2,	//��ʾ����ͼ
			BK_IMAGE_ALPHA	= 4	//����ͼ����Alphaͨ����
		};
		View();
		~View();
		//��ǰ����û������֮ǰ,�������ɵ�2������.
		INT_PTR Create(HWND hParent);

		bool Attach(HWND handle);
		HWND Detach();
		//���ñ���ɫ
		void SetBkgndColor(COLORREF color,bool reDraw = 0);
		//ȡ�ñ���ɫ
		COLORREF GetBkgndColor(){return _BkgndColor.GetColor();}
		//���ñ���ͼƬ��alpha��͸���ȡ���Ҫ����bmp�������Դ���Զ�����
		void SetBkgndImage(HBITMAP bmp,bool reDraw = 0);
		//ȡ�ñ���ͼ���һ��������ʹ����ɺ�Ҫ�ֶ�������Դ��
		HBITMAP GetBkgndImage(){return _BkgndImg.GetBitmap();}
		//���ñ���ͼ��͸���ȡ�
		void SetBkgndAlpha(byte alpha,bool redraw = 0);
		//ȡ�ñ���ͼ��͸���ȡ�
		byte GetBkgndAlpha(){return _BkAlpha;}
		//���ñ���ģʽ���ֱ�Ϊ����ɫ������ͼ������alphaͨ���������Ǳ���ͼ��alphaͨ����
		void SetBkgndMode(byte mode,bool redraw = 0);
		//ȡ�ñ���ģʽ��
		uint GetBkgndMode(){return _BkMode;}
		//���ô���ָ�롣
		void SetCursor(HCURSOR cursor);
		//ȡ�õ�ǰ���ָ�롣
		HCURSOR GetCursor(){return _Cursor;}
		//���ô���ָ�룬����Ϊ��Դid����Դ���ڵ�ʵ����0��ʾϵͳ��Դ��
		void SetCursor(base::ResID resurce,HINSTANCE hInst = 0);

		base::Delegate<base::gdi::DC*>	OnDoubleDraw;//һ������������м��뺯����OnDraw�Ͳ��ᱻ���С�����������Զ��������ڣ�Ҳ����˵��ͼ�����Լ����Ǵ��ڵĹ������⡣
		base::Delegate<base::gdi::DC*>	OnDraw;//��ͼ��Ϣ�ڳ�����ʵ�������첽��Ӧ�ó����̵߳�.		
		base::Delegate<HDC>		OnEraseBkgnd;	//Ҳ��������������ͼ,����OnDraw֮�󱻵���.�����ǵĻ����ǲ�ͬ��
		base::Delegate<base::StringMemList*>	OnDropFiles;//��Ӧ��ҷ��Ϣ.
		base::Delegate<base::Twin16>	OnSize;	//WM_SIZE��Ϣ�������ǿͻ����Ĵ�С��
	};
	//Class ScrollWnd
	class  ScrollWnd : public View{
	public:
		ScrollWnd(void);
		~ScrollWnd();
		//������С�Ŀͻ����ߴ�,���ͻ���С������ߴ�,����������.
		void SetMinSize(base::Twin size);	
		//������С�Ŀͻ����ߴ�,���ͻ���С������ߴ�,����������.���cx��cyΪ��ֵ������Ӧ�Ĵ�С���䡣
		void SetMinSize(int cx,int cy);				
		//������С�Ĺ�����λ.
		void SetMinScroll(int hsline, int vsline);		
		//0-hScroll,2-NoScroll,���������ֹ������ڵķ���.
		void SetMouseWheelScroll(int vScroll = 1);		
		//�����Ƿ���Ӧ���̷��������������.
		void SetResponseKeyBoard(bool response = 1);	
		//�������ڣ�dx��ˮƽ�������룬dy����ֱ�������롣
		void Scroll(int dx,int dy);
		//����ָ����������xLine ��ˮƽ������yLine �Ǵ�ֱ���������������������
		void ScrollLine(int xLine,int yLine);
		//����ָ����ҳ����xPage ��ˮƽҳ����yPage �Ǵ�ֱҳ�������������������
		void ScrollPage(int xPage,int yPage);
		//���������㣬xEnd = 1 ���������ұߣ�xEnd = -1 ����������ߣ�yEnd = 1 ���������±ߣ�yEnd = -1 ���������ϱߡ�
		void ScrollEnd(int xEnd,int yEnd);
		//�ػ洰��.����ͻ����й���,����������Լ�����,���Բ��ض��������ƽ��.
		bool Invalidate(const RECT* clip = NULL,bool erase = 1);
		//�ػ洰��.����ͻ����й���,����������Լ�����,���Բ��ض��������ƽ��.
		bool Invalidate(const HRGN clip,bool erase = 1);
		//����һ����,���ݹ���ƫ����.
		void TransDimension(base::Twin& pt);			
		//����һ������,���ݹ���ƫ����.
		void TransDimension(base::Rect& rect);			

		base::Delegate<int,int> OnScroll;//���ڹ���ʱ����.����Ϊƫ����,ʵ����,������������һ����0.

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
		DWORD _MouseWheelScroll;//���͵�WM_VSCROLL��WM_HSCROLL��Ϣ.
		bool _ResponseKeyBoard;//ָʾ��Ӧ���̣��д�ֱ������ˮƽ��
	};
	//Class Frame
	//Frame�������رհ�ť���Զ��˳�Ӧ�ó��򣬵���������������߳������Ч�������ϣ���
	//�ڲ������PostQuitMessage������ֻ���˳������ڵ��̡߳�
	class  Frame : public ScrollWnd{
	public:
		Frame();
		~Frame();
		//����һ��û��WS_CHILD���������ڣ���Ȼ��ʵ����Ҳ������һ�����ڵ��Ӵ��ڣ�������������������������
		//�����١�quitָ���Ƿ��˳�ʱ���������
		INT_PTR Create(HWND hParent = 0,int nCmdShow = 1,bool quit = 1);
		//�˳���Ϣѭ�������ڹرգ�һ��Ӧ�ó���Ҳ���˳���
		void Quit(int code = 0);
		//big=0,for set small icon,big=1,for set big icon.
		inline HICON SetIcon(HICON icon,bool big=0){return (HICON)SendMessage(WM_SETICON,big,icon);}
		//type=0,for small icon,type=1,for large icon,type=2,for system assign icon for application.
		inline HICON GetIcon(DWORD type=ICON_SMALL){return (HICON)SendMessage(WM_GETICON,type,0);};
	private:
		void _DestroyProc(Msg* msg);
	};
	//���������ڵ�ͼ�꣬����ͼ���Сͼ�ꡣ
	void SetWndIcon(HWND h,base::ResID id);
	//���������ʼ��Ҫʹ�õĿؼ�,Ĭ�ϲ�����ʼ�����пؼ�.
	bool InitControl60(DWORD icc = ICC_WIN95_CLASSES);
	//ImageList����ͼ���ͼ��,����Ҫ���������ؼ�ʹ�õ�ͼ��Դ.
	class ImageList : public base::HandleType<HIMAGELIST>{
	public:
		~ImageList();
		//��һ��HIMAGELIST���.
		void operator = (HIMAGELIST hImgl);
		//���������������ܱ�ʾû�гɹ�����ǰһ�����(��hImgl�ɹ���),��hImglΪNULL.
		bool Attach(HIMAGELIST hImgl);
		//����󶨵ľ��.
		HIMAGELIST Detach();

		//����һ��ImageList,cx,cy��ͼ��Ĵ�С,һ��ImageListֻ�ܴ洢��ͬ��С��ɫ���ͼ��.ilc��ͼ�������,initial��ʼ��С,grow����.
		bool Create(int cx,int cy,UINT ilc = ILC_COLORDDB,int initial = 16,int grow = 16);
		//��ͼ����Դ����.��������Ǵ���һ��ȫ�µ�ImageList,ԭ����HIMAGELISTʵ����ɾ��.cx�����ɵ�IMAGELISTͼ����,���������Ⱥͼ��ص�
		//ͼ������ȷ�����ɶ��ٸ�ͼ��,����һ��ͼ��Ĳ��ֻᱻ����.mask��͸��ɫ,res����ԴID,hInst = 0��ʾ��BMP�ļ�����(����ʾ�ӵ�ǰʵ������),
		//grow������.����ͼ�����.
		int LoadResource(int cx,COLORREF mask,base::ResID res,HINSTANCE hInst = 0,int grow = 16);
		//����HIMAGELIST.
		bool Destroy();

		//�滻��Ӧ������ͼ��,HICON��ԴҪ�Լ�����.
		bool Replace(HICON icon,int index);
		//���һ��ͼ��,HICON��ԴҪ�Լ�����.ͼ���ɫ���ImageList��ɫ��Ҫһ��,���򱳾���͸��.
		bool AddIcon(HICON icon);
		//��λͼ��ӵ�ImageList��,������ӵ�����,���������λͼ�Ŀ�ȿ��Էָ�ɵ�ImageList��ͼ�������,����һ���Ĳ�����ȥ.
		int AddBitmap(HBITMAP bmp,HBITMAP mask = 0);
		//ɾ����Ӧ������ͼ��.
		bool Delete(int index);
		//���ͼ��
		bool Clear();
		//��ʾͼ��.
		bool Draw(HDC hdc,int index,int x,int y,UINT style = ILD_NORMAL);
		//������Ӧ������ͼ��.
		bool Copy(int iDst,int iSrc);
		//������Ӧ������ͼ��.
		bool Swap(int iDst,int iSrc);
		//����һ������.
		HIMAGELIST Duplicate();
		//����ͼ������.
		int Count();
		//����ͼ���С.
		base::Twin GetImageSize();
		//������Ӧ������ͼ��.
		HICON GetIcon(int index);
		//���ر���ɫ.
		COLORREF GetBkColor();
		//���ñ���ɫ.
		COLORREF SetBkColor(COLORREF bk);
		//����ͼ���С,������������ͼ��.
		bool SetImageSize(int cx,int cy);
		//�����������ͼ�������,�����������ԭ��������,�����Լ���AddIcon�������λ.
		bool SetCount(int n);
	};
	//PBS_NORMAL = 1,PBS_HOT = 2,PBS_PRESSED = 3,PBS_DISABLED = 4,PBS_DEFAULTED = 5,PBS_STYLUSHOT = 6
	interface  ButtonPtr : public WndPtr{
	public:
		ButtonPtr(){}
		ButtonPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//���ذ�ť��ʾ��������ĳߴ�.
		bool GetIdealSize(LPSIZE ps){return SendMessage(BCM_GETIDEALSIZE,0,ps)!=0;}
		//��ȡ��ť��ѡ��״̬,������3��ֵ:0(δѡ��),1(ѡ��),2(��ȷ��).
		int GetCheck(){return (int)(INT_PTR)SendMessage(BM_GETCHECK,0,0)!=0;}
		//���ð�����ѡ��״̬.//BST_UNCHECKED(0),BST_CHECKED(1),BST_INDETERMINATE(2);
		void SetCheck(int state = BST_CHECKED){SendMessage(BM_SETCHECK,(WPARAM)state,0);}
		//����һ��ImageList��Ϊ��ť��ʾ��ͼ���б�.
		bool SetImageList(HIMAGELIST hImgl,UINT align = BUTTON_IMAGELIST_ALIGN_LEFT,LPRECT margin = 0);
		//���ذ�����ͼ��������ʾ����.
		bool GetImageList(BUTTON_IMAGELIST& bi){return SendMessage(BCM_GETIMAGELIST,0,&bi)!=0;}
		//�����ı���λ�ò���.
		bool SetTextMargin(int left,int top,int right,int bottom){return SendMessage(BCM_SETTEXTMARGIN,0,&left)!=0;}
		//��ȡ�ı���λ�ò���.
		bool GetTextMargin(LPRECT margin){return SendMessage(BCM_GETTEXTMARGIN,0,margin)!=0;}
		//ģ�ⰴ�°�ť����.
		void Click(){SendMessage(BM_CLICK,0,0);}
	};
	/*
	interface  ListBoxPtr : virtual public WndPtr{
	public:
		ListBoxPtr(){}
		ListBoxPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//���һ���ִ�.str��Ҫ��ӵ��ִ�,index������λ��.�����ִ����б��е�����.
		inline int AddString(LPCWSTR str,int index = -1);
		//ɾ��һ���ִ�.index��Ҫɾ�����ִ�������.
		inline bool DeleteString(int index)					{return (int)SendMessage(LB_DELETESTRING,(WPARAM)index,0)!=0;}
		//����һ���ִ�,begin�ǿ�ʼ���ҵ�����,�����ִ���������-1.�������ֻҪ����ƥ��Ϳ���,���Ҳ��ִ�Сд.
		inline int FindString(LPCWSTR str,int begin = -1)	{return (int)SendMessage(LB_FINDSTRING,(WPARAM)begin,(LPARAM)str);}
		//��ȷ����,����Ҫ�������ִ�ƥ��,��Ҫ���ִ�Сд.
		inline int FindExactString(LPCWSTR str,int begin = -1)	{return (int)SendMessage(LB_FINDSTRINGEXACT,(WPARAM)begin,(LPARAM)str);}
		//����Ԫ������.
		inline int GetCount()								{return (int)SendMessage(LB_GETCOUNT,0,0);}
		//�ж����Ƿ��Ѿ���ѡ��.
		inline bool ItemIsSelect(int index)					{return (int)SendMessage(LB_GETSEL,(WPARAM)index,(LPARAM)0)!=0;}
		//����ѡ���������,��-1.�����ڵ��б�.
		inline int GetSelItem()								{return (int)SendMessage(LB_GETCURSEL,0,0);}//this is only use for single select list box,for multisel it return value is the focused item(but single select is always select item);
		//�����б����Ӧ������ѡ��,���index = -1��û���ѡ��.�����ڵ�ѡ�б�.
		inline void SetSelItem(int index)					{SendMessage(LB_SETCURSEL,(WPARAM)index,(LPARAM)0);}//for single select list box, must one item is selected.
		//�����ض��ִ�Ϊѡ��״̬.
		inline int SetSelItem(LPCWSTR str,int start = -1)	{return (int)SendMessage(LB_SELECTSTRING,(WPARAM)start,(LPARAM)str);}
		//��ȡ��ѡʱ��ѡ��������,selҪ�����㹻�Ĵ�С.����ѡ���������.�������ֻ�����ڶ�ѡ�б�,���ڵ�ѡ�б�,���Ƿ���-1.
		inline int GetSelItems(base::Memory<int>* sel)		{return (int)SendMessage(LB_GETSELITEMS,(WPARAM)sel->Length(),(LPARAM)sel->Handle());}//this is only use for multy select style,for single select style it is always return -1;
		//���ö�ѡ�б��ѡ�з�Χ,ֻ�����ڶ�ѡ�б�.���lastС��first,����һ��Χ��ѡ����ᱻ���.��������Ҫע��,�����������
		//��first��last��ͬ,������ʧ��.��Χѡ��������.�����ֻѡ��һ����Բ��ý����İ취,��ѡ2��,�����һ��.��ʹ����LBS_EXTENDEDSEL���
		//���б�,Ҳ���Ժ��������ѡȡ�������,����һ������.
		inline int SetSelItems(WORD first,WORD last)		{return (int)SendMessage(LB_SELITEMRANGEEX,(WPARAM)first,(LPARAM)last);}//if last is less than first,the range is removed from select.
		//��ȡѡ���������.
		inline int GetSelCount()							{return (int)SendMessage(LB_GETSELCOUNT,0,0);}//this is only use for multy select style,for single select style it is always return -1;
		//ȡ����Ӧ��������ı�,����Ԥ����GetTextLengthȡ���ı�����ռ��С,text�Ŀռ����Ҫ���ڷ���ֵ��1.����ȡ���ı��ĳ��Ȼ�-1.
		int GetText(int index,base::String& buf);
		//��������ָ������ı���Window��û���ṩLB_SETTEXT��Ϣ����������ڲ�ͨ��ɾ��������ִ����������ܡ�
		bool SetText(int index,LPCWSTR str);
		//ȡ����Ӧ��������ı�����,��������βNULL�ַ�.
		inline int GetTextLength(int index)					{return (int)SendMessage(LB_GETTEXTLEN,(WPARAM)index,0);}//�����ı�����.
		//��ȡ�ض�λ�õ��������ֵ.
		inline int GetIndexByPosition(base::Twin16 pos)	{return (int)SendMessage(LB_ITEMFROMPOINT,0,(LPARAM)pos.Value);}
		//���������.
		inline void Clear()									{SendMessage(LB_RESETCONTENT,0,0);}
		//���ö����б�ؼ��Ŀ��.
		inline void SetColumnWidth(int width)				{SendMessage(LB_SETCOLUMNWIDTH,(WPARAM)width,0);}
		//����Ԫ�صĻ��Ƹ߶�,�������������Ҫ�ػ�ؼ�..
		inline bool SetItemHeight(int height)				{return (SendMessage(LB_SETITEMHEIGHT,0,(LPARAM)height)!=LB_ERR);}
		//��ȡ������(������).
		inline int GetTopItem()								{return (int)SendMessage(LB_GETTOPINDEX,0,0);}
		//���ö�����.
		inline bool SetTopItem(int index)					{return (SendMessage(LB_SETTOPINDEX,(WPARAM)index,0)!=-1);}
		//ListBox�����Զ�����ˮƽ������.
		inline void SetHScrollWidth(int width)				{SendMessage(LB_SETHORIZONTALEXTENT,(WPARAM)width,0);}
		//����ͨListBoxת����DragListBox��
		inline bool MakeDrglist()							{return 0!=::MakeDragList(_Handle);}
	};
	interface  LabelPtr : virtual public WndPtr{
	public:
		LabelPtr(){}
		LabelPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
	};
	//Button Control
	/*ֻ��XP���İ�ť�ſ�������ͼ���б�ؼ�,���ͼ���б�ؼ�ֻ��һ��ͼ��,����ʾ���ͼ��.����ж���1��ͼ����Ӧ����
	5��ͼ��,�ֱ�������,�н���,���ͣ��,DISABLE״̬�Ͱ���״̬����ʾ
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

		DWORD GetRange(LPSYSTEMTIME pst)	{return DateTime_GetRange(_Handle,pst);}//pst��һ��������SystemTimeԪ�������ָ��.
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
	//ComboBox Control �������һ��ListBox�ؼ�,�����������ؼ��ĺܶຯ���Ǻ����Ƶ�.
	interface  ComboBoxPtr : virtual public WndPtr{
	public:
		ComboBoxPtr(){}
		ComboBoxPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//���һ���ִ�.������ӵ�λ��.
		inline int AddString(LPCWSTR str,int index = -1)		{return (int)SendMessage(CB_INSERTSTRING,(WPARAM)index,(LPARAM)str);}//retrun index of insert item.
		//ɾ��һ���ִ�(ͨ������).
		inline bool DeleteString(int index)						{return SendMessage(CB_DELETESTRING,(WPARAM)index,0)!=-1;}
		//����ִ�.
		inline void Clear()										{SendMessage(CB_RESETCONTENT,0,0);}
		//�����ض�����ı�.
		inline bool SetItemText(int index,LPCWSTR str)			{return SendMessage(CB_SETITEMDATA,(WPARAM)index,(LPARAM)str)!=-1;}
		//�����ض�����ı�.
		bool GetItemText(int index,base::String& str);
		//��ȡԪ�ص�������.
		inline int GetCount()									{return (int)SendMessage(CB_GETCOUNT,0,0);}
		//���ص�ǰѡ���������.
		inline int GetCurSelect()								{return (int)SendMessage(CB_GETCURSEL,0,0);}
		//���õ�ǰ��Ϊ�ض�����.
		inline int SetCurSelect(int index)						{return (int)SendMessage(CB_SETCURSEL,(WPARAM)index,0);}
		//����һ���ִ�.
		inline int FindString(LPCWSTR str,int start = -1)		{return (int)SendMessage(CB_FINDSTRING,(WPARAM)start,(LPARAM)str);}
		//��ȷ����һ���ִ�.
		inline int FindExactString(LPCWSTR str,int start = -1)	{return (int)SendMessage(CB_FINDSTRINGEXACT,(WPARAM)start,(LPARAM)str);}
		//����ѡ����(ͨ���ִ�).
		inline int SelectString(LPCWSTR str,int start = -1)		{return (int)SendMessage(CB_SELECTSTRING,(WPARAM)start,(LPARAM)str);}
		//չ��������.
		inline void ShowDropDown(bool show = 1)					{SendMessage(CB_SHOWDROPDOWN,(WPARAM)show,0);}
		//ʹ�ض���ɼ�.
		inline bool EnsureVisible(int index)					{return (SendMessage(CB_SETTOPINDEX,(WPARAM)index,0)!=-1);}
		//�����û����Լ��������ַ�����,Ĭ��ֵ��30000.
		inline void SetEditLimit(int count)						{SendMessage(CB_LIMITTEXT,(WPARAM)count,0);}//count = 0 for no limit;
	};
	//CoboBoxEx Control ������ʾͼ��.ComboBoxEx��һ������ķ����CBES_EX_CASESENSITIVE,���Զ��г���Ա.
	interface  ComboBoxExPtr : virtual public WndPtr{//ComboBoxEx��֧�������ִ��йصĺ���.
	public:
		ComboBoxExPtr(){}
		ComboBoxExPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//���ÿؼ���ͼ���б�ؼ�
		inline HIMAGELIST SetImageList(HIMAGELIST hImgl)	{return (HIMAGELIST)SendMessage(CBEM_SETIMAGELIST,0,(LPARAM)hImgl);}
		//��ȡ�ؼ���ͼ�񾲾��ؼ�.
		inline HIMAGELIST GetImageList()					{return (HIMAGELIST)SendMessage(CBEM_GETIMAGELIST,0,0);}
		//���һ����,str:�ı�,image:ͼ����ͼ���б�ؼ��е�����,indent:ͼ������ֵ����ƿ��,1��indent���Ϊ10��pix,
		//index:��ӵ�λ��,selImage:��ѡ��ʱ��ͼ������,lParam:���ӵĲ���.һ�������ǵ�image��selImage��ͬʱ�������ʾ����.
		int AddItem(LPCWSTR str,int image = -1,int indent = 0,int index = -1,int selImage = -1,LPARAM lParam = 0);
		//ɾ��һ��.
		inline bool DeleteItem(int index)					{return (SendMessage(CBEM_DELETEITEM,(WPARAM)index,0)==-1);}
		//���.
		inline void Clear()									{SendMessage(CB_RESETCONTENT,0,0);}
		//���ö�����,��Щ���ֻ�����������������,�����������ɲ���������.
		inline DWORD SetExStyle(DWORD cbs)					{return (DWORD)SendMessage(CBEM_SETEXSTYLE,0,(LPARAM)cbs);}
		//���ض�����.
		inline DWORD GetExStyle()							{return (DWORD)SendMessage(CBEM_GETEXSTYLE,0,0);}
		//���������.
		inline bool SetItem(PCOMBOBOXEXITEM pcbi)			{return (int)SendMessage(CBEM_SETITEM,0,(LPARAM)pcbi)!=0;}
		//��ȡ�����.
		inline bool GetItem(PCOMBOBOXEXITEM pcbi)			{return (int)SendMessage(CBEM_GETITEM,0,(LPARAM)pcbi)!=0;}
		//û�а취֪��Ҫȡ���ִ��ĳ���,���Ը�text����һ���ϴ��Capacity,�������ֻȡ���ִ��Ĳ���,��������º����Է���1.
		bool GetText(int index,base::String& text);
		//��index = -1ʱ,���õ��Ǳ༭����ı�.
		bool SetText(int index,LPCWSTR text);
		//��ȡ���ͼ������.
		int GetImage(int index,int flag = 0);// 0 for image,1 for selImage,2 for overlay.
		//�������ͼ������.
		bool SetImage(int index,int image,int flag = 0);
		//����ƽ��.
		bool SetIndent(int index,int indent);
		//����ƽ����
		int GetIndent(int index);
		//����Ԫ������.
		inline int GetCount()								{return (int)SendMessage(CB_GETCOUNT,0,0);}
		//����ѡ���������.
		inline int GetCurSelect()							{return (int)SendMessage(CB_GETCURSEL,0,0);}
		//����ѡ����.
		inline int SetCurSelect(int index)					{return (int)SendMessage(CB_SETCURSEL,(WPARAM)index,0);}
		//���ÿɼ��������.
		inline bool SetVisibleCount(int count)				{return (int)SendMessage(CB_SETMINVISIBLE,(WPARAM)count,0)!=0;}
		//��ȡѡ���������.
		inline int GetVisibleCount()						{return ComboBox_GetMinVisible(_Handle);}
		//���ñ༭�ؼ�������ַ���.
		inline void SetEditLimit(int count)					{SendMessage(CB_LIMITTEXT,(WPARAM)count,0);}//count = 0 for no limit;
		//��ǰ���ô洢�ռ�.
		inline bool ExpendStorage(int count,int strStorage)	{return SendMessage(CB_INITSTORAGE,(WPARAM)count,(LPARAM)strStorage)!=CB_ERRSPACE;}
	};
	//TreeView Control
	/*TreeView�ؼ�������ν��������ʾ�ؼ�,����Ԫ�������ηֲ�,����һ��Ӧ�÷ǳ��㷺�Ľṹ,�����ļ�ϵͳ.TreeView�ؼ����԰�
	����ͼ���б�ؼ�,һ��������ͨ״̬��,һ������ѡ��״̬��.
	interface  TreeViewPtr : virtual public WndPtr{
	public:
		TreeViewPtr(){}
		TreeViewPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//�������,parent:��ӵ�λ��,������TVI_ROOT.text:��ʾ���ı�,imgIndex:ͼ������,selIndex:ѡ��״̬�µ�ͼ������,insAfter:ͬ����������λ��.
		HTREEITEM AddItem(HTREEITEM parent,LPCWSTR text,int imgIndex,int selIndex,HTREEITEM tvi = 0);
		//ɾ������.
		bool DeleteItem(HTREEITEM hItem){return TreeView_DeleteItem(_Handle,hItem)!=0;}
		//������е���.
		bool Clear(){return TreeView_DeleteAllItems(_Handle)!=0;}
		//��������str�㹻������,����ֵ���ִ��ĳ���.
		int GetItemText(HTREEITEM hti,base::String& str);
		//����������ı�.
		bool SetItemText(HTREEITEM hti,LPCWSTR str);
		//ȡ�������ͼ������,flag������TVSIL_STATE��TVSIL_NORMAL֮һ.
		int GetImageIndex(HTREEITEM hti,UINT flag = TVSIL_NORMAL);
		//���������ͼ������,-1��ʾ�������������.
		bool SetImageIndex(HTREEITEM hti,int image,int selImage = -1);
		//ȡ�ÿؼ�ʹ�õ�ͼ���б�ؼ��ľ��,flag��������ѡֵ:TVSIL_NORMAL��TVSIL_STATE.
		HIMAGELIST GetImageList(UINT flag = TVSIL_NORMAL)					{return TreeView_GetImageList(_Handle,flag);}
		//���ÿؼ�ʹ�õ�ͼ���б�ؼ�,flag��������ѡֵ,TVSIL_NORMAL��TVSIL_STATE.
		HIMAGELIST SetImageList(HIMAGELIST hImgl,UINT flag = TVSIL_NORMAL)	{return (HIMAGELIST)SendMessage(TVM_SETIMAGELIST,flag,(HIMAGELIST)hImgl);}//����ImageList,����ѡ��ɾ��ǰһ��ImageList�򷵻�����.hImglΪ0������.
		//���ÿؼ��ı���ɫ,�������������Control���ͬ������,��Ҫ���Ǹ����������ñ���ɫ.-1ʹ��ϵͳ��ȱʡɫ.
		COLORREF SetBkgndColor(COLORREF color)	{return TreeView_SetBkColor(_Handle,color);}
		//��ȡ�ؼ��ı���ɫ.
		COLORREF GetBkgndColor()				{return TreeView_GetBkColor(_Handle);}
		//�����ı�����ɫ,�������ͬ��������Control���ͬ������.��Ϊ�Ǹ�����������ؼ�������.color = -1��ʾʹ��ϵͳ��ȱʡ��ɫ.
		COLORREF SetFontColor(COLORREF color)	{return TreeView_SetTextColor(_Handle,color);}
		//��ȡ�ı���ɫ.
		COLORREF GetFontColor()					{return TreeView_GetTextColor(_Handle);}
		//����ˮƽ�ߵĳ���,���С���������Сֵ,��Ϊ��Сֵ.
		void SetLineIndent(int width)			{TreeView_SetIndent(_Handle,width);}
		//����ˮƽ�ߵĳ���,ȱʡֵ��ԼΪ20.
		int GetLineIndent()						{return TreeView_GetIndent(_Handle);}
		//���û��ߵ���ɫ.
		COLORREF SetLineColor(COLORREF color)	{return TreeView_SetLineColor(_Handle,color);}
		//��ȡ���ߵ���ɫ.
		COLORREF GetLineColor()					{return TreeView_GetLineColor(_Handle);}
		//��ʼ�༭��ǩ,���ر༭�ؼ��ľ��.
		HWND BeginEdit(HTREEITEM hItem)			{return TreeView_EditLabel(_Handle,hItem);}
		//��ɱ�ǩ�༭,cancel = 1��ʾ��ԭ�ı�.
		bool EndEdit(bool cancel = 0)			{return TreeView_EndEditLabelNow(_Handle,cancel)!=0;}
		//��������;all--�Ƿ�Զ༶�����������.
		bool SortChildren(HTREEITEM hti,bool all = 0)	{return TreeView_SortChildren(_Handle,hti,all)!=0;}
		//ʹ�ض�����ɼ�.(ͨ��չ�������).��������ǹ���ʹ��ɼ�,����1,���򷵻�0.
		bool EnsureVisible(HTREEITEM hItem)				{return TreeView_EnsureVisible(_Handle,hItem)!=0;}
		//չ���������ض�����.
		bool Expand(HTREEITEM hItem,UINT tve = TVE_TOGGLE)	{return TreeView_Expand(_Handle,hItem,tve)!=0;}
		//��ȡ�󶨵ı༭�ؼ��ľ��.
		HWND GetEditControl()			{return TreeView_GetEditControl(_Handle);}
		//�����������.���Ϊ16λ���������ֵ.
		int GetItemCount()				{return TreeView_GetCount(_Handle);}
		//��ȡ��ľ���.
		bool GetItemRect(HTREEITEM hItem,LPRECT rect,bool onlyText = 1){return TreeView_GetItemRect(_Handle,hItem,rect,onlyText)!=0;}
		//��ȡ��ĸ߶�.
		int GetItemHeight(){return TreeView_GetItemHeight(_Handle);}
		//������ĸ߶�.
		int SetItemHeight(int height){return TreeView_SetItemHeight(_Handle,height);}		
		//����ToolTip�ؼ�.
		HWND SetToolTipCtrl(HWND hToolTip){return TreeView_SetToolTips(_Handle,hToolTip);}
		//ȡ�ù�����ToolTip�ؼ�.
		HWND GetToolTipCtrl(){return TreeView_GetToolTips(_Handle);}		
		//������Ĳ���
		bool SetItemInfo(LPTVITEMEX pti){return TreeView_SetItem(_Handle,pti)!=0;}
		//��ȡ��Ĳ���.
		bool GetItemInfo(LPTVITEMEX pti){return TreeView_GetItem(_Handle,pti)!=0;}
		//���õ�ǰѡ����Ϊָ����.
		bool SetSelectItem(HTREEITEM hti){return TreeView_SelectItem(_Handle,hti)!=0;}
		//��ȡ��Ķ������ݲ���.
		bool GetItemData(HTREEITEM hti,LPARAM* pParam);
		//������Ķ������ݲ���.
		bool SetItemData(HTREEITEM hti,LPARAM data);
		//���ظ�.
		HTREEITEM GetRoot()							{return TreeView_GetRoot(_Handle);}
		//�����������չ����.
		HTREEITEM GetLastItem()						{return TreeView_GetLastVisible(_Handle);}
		//���ص�һ������(��0���û��).
		HTREEITEM GetFirstChild(HTREEITEM hti)		{return TreeView_GetChild(_Handle,hti);}
		//��һ���ɼ���(�����Ǽ�),hitem����ɼ�.
		HTREEITEM GetNextVisible(HTREEITEM hti)		{return TreeView_GetNextVisible(_Handle,hti);}
		//ǰһ���ɼ���(�����Ǽ�),hitem����ɼ�.
		HTREEITEM GetPreVisible(HTREEITEM hti)		{return TreeView_GetPrevVisible(_Handle,hti);}
		//������һ��ͬ����.
		HTREEITEM GetNext(HTREEITEM hti)			{return TreeView_GetNextSibling(_Handle,hti);}
		//����ǰһ��ͬ����.
		HTREEITEM GetPrevious(HTREEITEM hti)		{return TreeView_GetPrevSibling(_Handle,hti);}
		//���ظ���ľ��;���û�и���,����0.
		HTREEITEM GetParent(HTREEITEM hti)			{return TreeView_GetParent(_Handle,hti);}
		//���ص�ǰѡ����.
		HTREEITEM GetSelectItem()					{return TreeView_GetSelection(_Handle);}
		//ȡ���ض�λ�õ��
		HTREEITEM GetPosItem(int x,int y)			{TVHITTESTINFO ti;ti.pt.x = x;ti.pt.y = y;return TreeView_HitTest(_Handle,&ti);}
		//����һ����ľ��,������tvgnȷ��.��ʵ������ǰ�����к������ܵ����,���Ҷ�������ǰ�����Щ����ȡ��.
		HTREEITEM GetItem(UINT tvgn,HTREEITEM hti = 0)	{return TreeView_GetNextItem(_Handle,hti,tvgn);}
		//��ȡ���CheckBoxѡ��״̬��0��δѡ�У�1��ѡ�У�-1��û��CheckBox��
		int GetItemChecked(HTREEITEM hti)			{return TreeView_GetCheckState(_Handle,hti);}
		//�������CheckBoxѡ��״̬��0��δѡ�У�1��ѡ�С�
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
		//���ɿؼ�.
		INT_PTR Create(HWND hParent);
		//��Ӱ�ť,cmdID:��ť��WM_COMMAND��ϢID,image:ͼ�����б��е�����,text:��ʾ���ı�,tbn:��ť�ķ��,tbstate:��ť��״̬,index:��ӵ�λ��.
		bool AddButton(WORD cmdID,int image,LPCWSTR text,BYTE tbn = BTNS_BUTTON|BTNS_AUTOSIZE,BYTE tbstate = TBSTATE_ENABLED,int index = -1);
		//���һ���ָ���,index��λ������.
		bool AddSplit(int index = -1);
		//����ͼ���б�ؼ�,ToolBar�ؼ�����3��ͼ���б�ؼ�,0:��ͨ,1:DISABLE״̬,2:���ͣ��״̬.
		HIMAGELIST SetImageList(int type,HIMAGELIST hImgl);
		//��ȡͼ���б�ؼ�.0:��ͨ,1:DISABLE״̬,2:���ͣ��״̬.
		HIMAGELIST GetImageList(int type);
		//���ذ�����״̬,ͨ��ID����.
		inline int GetButtonState(int id)	{return (int)SendMessage(TB_GETSTATE,id,0);}
		//���ð�ť��״̬,ͨ��ID����.
		bool SetButtonState(int id,int state,DWORD option = 1);
		//ͨ��TBBUTTON�ṹ����һ����Ӷ����ť.
		inline bool AddButton(TBBUTTON* tbb,int count = 1){return SendMessage(TB_ADDBUTTONS,count,(LPARAM)tbb)!=0;}
		//ɾ���ض��İ�ť.
		inline bool DelButton(int index){return (int)SendMessage(TB_DELETEBUTTON,index,0)!=0;}
		//Enable��Disableһ����ť.
		inline bool EnableButton(int id,bool enable = 0){return (int)SendMessage(TB_ENABLEBUTTON,id,enable)!=0;}
		//���ػ���ʾһ����ť.
		inline bool HideButton(WORD id,bool hide = 1){return (int)SendMessage(TB_HIDEBUTTON,id,MAKELONG(hide,0))!=0;}
		//�л�һ����ť.
		inline bool CheckButton(WORD id,bool check = 1){return (int)SendMessage(TB_CHECKBUTTON,id,MAKELONG(check,0))!=0;}//ʹһ����ť����check״̬,�����������ť�Ƿ���һ��check��ť,����ⲻ��һ��check��ť,����ͨ��������ʹ�����ڷ�check״̬.
		//ģ�ⰴ������.
		inline bool PressButton(WORD id,bool pressed = 1){return (int)SendMessage(TB_PRESSBUTTON,id,MAKELONG(pressed,0))!=0;}//ʹһ����ť������ѹ״̬,����ͨ��������������״̬.
		//
		inline bool GetAnchorHighLight(){return (int)SendMessage(TB_GETANCHORHIGHLIGHT ,0,0)!=0;}
		//
		inline bool SetAnchorHighLight(bool high){return (int)SendMessage(TB_SETANCHORHIGHLIGHT,high,0)!=0;}
		//
		bool LoadBitmap(int type,int cx,COLORREF mask,base::ResID,HINSTANCE hInst = 0,bool delpre = 1);//�����ϣ��ǰһ��Image List���ͷ�,��delpre = 0.
		//
		inline bool SetIconSize(int cx,int cy){return (int)SendMessage(TB_SETBITMAPSIZE,0,MAKELONG(cx,cy))!=0;}//����ȱʡֵ��ͼ��ߴ�.
		//
		inline bool SetButtonSize(int cx,int cy){return (int)SendMessage(TB_SETBUTTONSIZE,0,MAKELONG(cx,cy))!=0;}//��AddButton֮�����,ֻ�ܵ���,����С��ͼ��߶�.
		//
		inline base::Twin16 GetButtonSize(){return (int)SendMessage(TB_GETBUTTONSIZE,0,0);}
		//
		bool GetButtonRect(int index,LPRECT lpRect,bool byIndex = 1);
		//
		bool SetLimitButtonWidth(int min,int max){return (int)SendMessage(TB_SETBUTTONWIDTH,0,MAKELONG(min,max))!=0;}
		//
		inline UINT GetStyle(){return (int)SendMessage(TB_GETSTYLE,0,0);}
		//����ToolBar�ķ��option = 0�����������option = 1���Ƴ�������option = 2������Ϊ������
		UINT SetStyle(UINT style,DWORD option = 0);
		//
		inline UINT GetExStyle(){return (int)SendMessage(TB_GETEXTENDEDSTYLE,0,0);}
		//����ToolBar�Ķ�����option = 0�����������option = 1���Ƴ�������option = 2������Ϊ������
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
		INT_PTR GetButtonInfo(WORD id,DWORD tbif);//return -1 represnt false.-2 indecate no image,����TBIF_BYINDEX�������ֻ����һ��.
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
		//����ֵ�����޺����޵ĵ�16ֵ.�����������ֵ��һ��32λֵ,��GetRange����ȡ��.
		inline base::Twin16 SetRange(int low,int hight){return (base::Twin16)SendMessage(PBM_SETRANGE32,low,hight);}
		//ȡ��32λ��������ֵ.
		inline void GetRange(base::Twin& range){SendMessage(PBM_GETRANGE,0,&range);}
		//���õ�ǰλ��
		inline void SetPosition(int pos){SendMessage(PBM_SETPOS,pos,0);}
		//ȡ�õ�ǰλ��.
		inline uint GetPosition(){return (uint)SendMessage(PBM_GETPOS,0,0);}
		//������С����.
		inline void SetIncrement(int delta){SendMessage(PBM_DELTAPOS,delta,0);}
		//���ò�������.
		inline void SetStepValue(int step){SendMessage(PBM_SETSTEP,step,0);}
		//ǰ��һ��������ֵ.
		inline void Step(){SendMessage(PBM_STEPIT,0,0);}
		//��ʼ��ֹͣ��������������û�У�PBS_MARQUEE������÷�Χ�������refresh��ˢ�����ڣ���λ�Ǻ��롣
		inline void SetMarquee(bool stop = 0,int refresh = 100){SendMessage(PBM_SETMARQUEE,!stop,refresh);}
	protected:
	};
	//ToolTip Control ����ؼ�����������6.0�汾�ؼ�ʱ�ſ���.Ҳ����˵,����Ҫ��MANIFEST.
	interface  ToolTipPtr : virtual public WndPtr
	{
	public:
		ToolTipPtr(){}
		ToolTipPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		INT_PTR Create(HWND hParent);
		//�رջ�򿪿ؼ�����ʾ����.
		inline void Enable(bool enable = 1){this->SendMessage(TTM_ACTIVATE,enable,0);}
		//��һ���������һ��ToolTip,owner:�����ͣ��ʱҪ��ʾTip�Ĵ���,text:��ʾ��Ϣ,id,pr,����������ָ������Ҫ��ʾ��ʾ
		//�������ID,��ΪҪ��ʾ��ͬ����ʾ,��ID����.ttf:��ʾ���.�ظ���ID(����HWND)����,�Ḳ����һ�����á�
		bool AddTip(HWND owner,LPCWSTR text,UINT id = 0,LPRECT pr = 0,UINT ttf = 0);
		//ɾ��һ����ʾ��
		void DelTip(HWND owner,UINT uID = 0);
		//������Ӧ��ʾ�Ĵ�������
		bool SetTipRect(LPRECT pr,HWND owner,UINT uID = 0);
		//����һ����ʾ����Ϣϸ��,��ΪTI�Ľṹ�Ƚϸ��ӣ��������������ȵ���GetTipInfo��ʼ��ti��ֵ����cbSize,hwnd �� uId �����ʼ����
		//Ȼ������Ҫ�ı�ĳ�Ա��ֵ���ٵ������������
		inline bool SetTipInfo(LPTOOLINFO ti){return SendMessage(TTM_SETTOOLINFO,0,&ti)!=0;}
		//ȡ��һ����ʾ����Ϣϸ��,ti��cbSize��Ա�����ʼ��,hwnd �� uId ��ԱҲ����ָ��,��Ϊ��Ϣ��������Ϊ������.
		inline bool GetTipInfo(LPTOOLINFO ti){return 0!=SendMessage(TTM_GETTOOLINFO,0,ti);}
		//ֻ�е�TTS_BALLOON�������ʱ����������ſ��á�title:Ҫ��ʾ�ı����ı�,tti��Ҫ��ʾ��ͼ�꣬0��û��ͼ�꣬1����Ϣ��2�����棬3������
		inline bool SetTitle(LPCWSTR title,int tti = TTI_NONE){return 0!=SendMessage(TTM_SETTITLE,tti,title);}
		//������ʾ���ı���
		bool SetTipText(LPCWSTR text,HWND owner,UINT id);
		//������ʾ�ķ��
		bool SetTipFlag(UINT ttf,HWND owner,UINT id);
		//������ʾ�ı���ɫ��
		inline bool SetTextColor(COLORREF color){return 0!=SendMessage(TTM_SETTIPTEXTCOLOR,color,0);}
		//������ʾ���ڵı�����ɫ��
		inline bool SetBkColor(COLORREF color){return 0!=SendMessage(TTM_SETTIPBKCOLOR,color,0);}
		//������ʾ���ڵĿ�ȡ�
		inline int SetTextWidth(UINT width){return (int)this->SendMessage(TTM_SETMAXTIPWIDTH,0,width);}
		//������ʾ��ʾ���ӳ�ʱ�䡣
		inline void SetDelayTime(UINT ms,UINT ttdt = TTDT_INITIAL){this->SendMessage(TTM_SETDELAYTIME,ttdt,ms);}
		//ǿ����ʾ�������»��ơ�
		inline void UpDate(){SendMessage(TTM_UPDATE,0,0);}
		//������ʾ����ʾʱ���������λ�Ǻ��롣flag:2-show,3-delay,1-reshow��0-all default.
		inline void SetTimeInfo(int t,UINT flag){SendMessage(TTM_SETDELAYTIME,flag,t);}
		//ȡ��ʱ�������flag:2-show,3-delay,1-reshow
		inline int GetTimeInfo(UINT flag){return (int)SendMessage(TTM_GETDELAYTIME,flag,0);}
	};

	//TabPage�ؼ��������ṩ�Կؼ��Ĳ���,��ֻ�ṩһ�����,Ȼ����ҳ��ť��������Ӧ����Ϣ�¼�.����Щ�¼��д���ؼ�����Ϊ,ʹ�����ֵ������ڴ��ڲ�ͬ��ҳ��.
	interface  TabPagePtr : virtual public WndPtr
	{
	public:
		TabPagePtr(){}
		TabPagePtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//����һ������ҳ,index�ǲ����λ������,title����ʾ�ı���,image��ͼ����ͼ���б�ؼ��е�����,param��һ���û��Զ������.
		bool Add(int index,LPWSTR title = 0,int image = -1,LPARAM param = 0);
		//ɾ��ָ����������.
		bool Delete(int index){return 0!=SendMessage(TCM_DELETEITEM,index,0);}
		//�����������ҳ.
		bool Clear(){return 0!=SendMessage(TCM_DELETEALLITEMS,0,0);}
		//���ص�ǰ����ҳ������.
		int GetCurSel(){return (int)SendMessage(TCM_GETCURSEL,0,0);}
		//����ָ������������ҳΪ��ǰҳ.
		void SetCurSel(int index){SendMessage(TCM_SETCURSEL,index,0);}
		//����ͼ���б�ؼ�.
		HIMAGELIST SetImageList(HIMAGELIST imgl){return (HIMAGELIST)SendMessage(TCM_SETIMAGELIST,0,imgl);}
		//ȡ��ͼ���б�ؼ�.
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
	//��ʾ��ϼ���һ���ṹ.
	//�ȼ��ؼ�
	interface  HotKeyPtr : virtual public WndPtr
	{
	public:
		HotKeyPtr(){}
		HotKeyPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//�������������Ч�ļ����,hkcomb��Ҫ���ε���ϼ�,�����Ƕ��HKCOMB_�ೣ�������,��������Щ���ʱ,�ᱻָ����hotkeyfȡ��.
		void SetRules(int hkcomb,int hotkeyf){SendMessage(HKM_SETRULES,MAKEWPARAM(hkcomb,0),MAKELPARAM(hotkeyf,0));}
		//������ϼ�.
		void SetHotKey(unsigned char comb,unsigned char vk);
		//������ϼ�.
		void SetHotKey(HotkeyParam hk){SendMessage(HKM_SETHOTKEY,hk.wParam,0);}
		//ȡ����ϼ�,��λ�Ǽ�,��λ�����.
		HotkeyParam GetHotKey();
		//�󶨵�һ������,���������Ӵ���.
		bool Register(HWND frame){return (int)SendMessage(WM_SETHOTKEY,GetHotKey().wParam,0)>0;}
	};
	interface  ListViewPtr : virtual public WndPtr
	{
	public:
		ListViewPtr(){}
		ListViewPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}
		//���һ���У������е�����λ�û���-1���������б���������ȣ�λ�ã�-1��ʾ��ӵ���󣩡�
		int AddColumn(LPWSTR title,int width,int index = -1);
		//���һ����Ŀ����������һ�е��ı�����λ�ã�-1����ʾ��ӵ���󣬷���λ��������-1.
		int AddItem(LPWSTR text,int index = -1);
		//����һ����Ŀ���ı����������ı�������������������
		bool SetItemText(LPWSTR text,int index,int column);
		//ɾ��ָ���������У�����0�������޷�ɾ�������Ҫɾ�����Ȳ���һ��0��ȵ��У���ɾ����
		bool DelColumn(int index){return 0!=this->SendMessage(LVM_DELETECOLUMN,index,0);}
		//ɾ��ָ���С�
		bool DelItem(int index){return 0!=this->SendMessage(LVM_DELETEITEM,index,0);}
		//������С�
		bool Clear(){return 0!=this->SendMessage(LVM_DELETEALLITEMS,0,0);}
		//�༭��ǩ��
		HWND EditItem(int index){SetFocus(_Handle);return (HWND)this->SendMessage(LVM_EDITLABEL,index,0);}
		//ȡ����ǩ�༭��
		void CancelEdit(){this->SendMessage(LVM_CANCELEDITLABEL,0,0);}
		//�����е�������
		int GetColumnCount();
		//�����е�������
		int GetItemCount(){return (int)this->SendMessage(LVM_GETITEMCOUNT,0,0);}
		//ȡ��ָ����column����index���ı���
		int GetItemText(base::String& text,int index,int column);
		//ȡ�������ض����������������falg ����ȷ��������ͺ�ѡ����� LVNI_SELECTED ����ȡ��ѡ�����������
		//after ָ����������һ��������濪ʼ��-1 ��ʾ���������ע��������ֵ�� 0 ������������0�
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
		//���� StatusBar ��Ϊ�������֣�����������Է������ã������һ�ε���Ϊ׼������ԭ���������ӷֶΣ�
		//����ԭ������ɾ�������ķֶΡ�width ��һ���������飬����Ϊ parts��ÿ������Ԫ�ص�ֵ��Ӧÿ����
		//�ε��ұ߽磬��λ�����أ����ĳ��Ԫ�ر�ǰ���ֵС����ζ������λᱻǰ��Ķ��ڵ������ɼ������
		//���ֵ��-1�����ʾ�����ֱ���ؼ���ĩβ������Ķζ������ڵ�����
		bool SetParts(int parts,int* width){return 0!=SendMessage(SB_SETPARTS,parts,width);}
		//���öε��ı���index �Ƕε�������type �����ֵ���ʾ��񣬱��� SBT_POPOUT ��
		bool SetText(int index,LPCWSTR text,int type = 0){return 0!=SendMessage(SB_SETTEXT,index|type,text);}
	};
	interface  UpDownPtr : virtual public WndPtr
	{
	public:
		UpDownPtr(){}
		UpDownPtr(HWND ctrl){_Handle = ctrl;}
		void operator = (HWND ctrl){_Handle = ctrl;}

		//���ù������ڣ�һ����һ���ı����ڣ�����ǰһ�������ؼ������
		HWND SetBuddy(HWND buddy){return (HWND)SendMessage(UDM_SETBUDDY,buddy,0);}
		//ȡ�ù������ڡ�
		HWND GetBuddy(){return (HWND)SendMessage(UDM_GETBUDDY,0,0);}
		//���÷�Χ��
		void SetRange(int nLower,int nUpper){SendMessage(UDM_SETRANGE32,nLower,nUpper);}
		//ȡ�÷�Χ��
		base::Twin GetRange(){base::Twin rg;SendMessage(UDM_GETRANGE32,&rg.x,&rg.y);return rg;}
		//����λ�á�
		int SetPosition(int nPos){return (int)SendMessage(UDM_SETPOS32,0,nPos);}
		//ȡ��λ�ã�ʵ���������ֵ�Ǵ�buddy����ȡ�õģ��������buddy���ڲ����ڻ����ı�����һ�����֣����ص�ֵ�ǲ����õġ�
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
	//�ؼ��Ļ��࣬��View��ͬ���ؼ����Լ��Ĵ����࣬��ʵ��ֱ���ؼ�������ɣ����Ƕ����ղ�����Ϣ��
	//OnCreate�Ǹ�α��Ϣ�ص�������ֱ��Create���֮��ŵ��õģ���������ӦWM_CREATE��Ϣ��
	*/
	/*
	//����Ŀؼ���Ȼ��ICtrl��Ptr�̳У���û������ռ�ÿռ�ĳ�Ա�����ǲ��ܰ�һ�� new ICtrl �����ָ��
	//ת��Ϊ����ؼ���ָ�룬���ǵĴ�С�ǲ�ͬ�ģ�������iCtrlָ�벻��  new ICtrl �����ģ����Ǳ��� new ListBox
	//�����ת��Ϊ ListBoxָ�롣

	//class Label
	class Label : public ICtrl,public LabelPtr{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		Label();
	};
	//Button Control
	/*ֻ��XP���İ�ť�ſ�������ͼ���б�ؼ�,���ͼ���б�ؼ�ֻ��һ��ͼ��,����ʾ���ͼ��.����ж���1��ͼ����Ӧ����
	5��ͼ��,�ֱ�������,�н���,���ͣ��,DISABLE״̬�Ͱ���״̬����ʾ

	BS_GROUPBOX����Button��Ϊ����͸���������Ʊ������������ڲ���ʹ��WS_CLIPCHILDREN���

	
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
	//ListBox Control ��3�ַ��ֱ��ǵ�ѡ(Ĭ��),������ѡ(LBS_EXTENDEDSEL,������SHIFT��ѡ��һ������),�����ѡ(LBS_MULTIPLESEL,���Ե����ѡȡ��ȡ������һ��)
	class ListBox : public ICtrl,public ListBoxPtr
	{
	public:
		ListBox();
		void operator = (HWND hWnd){Attach(hWnd);}
	protected:
	};
	//ComboBox Control �������һ��ListBox�ؼ�,�����������ؼ��ĺܶຯ���Ǻ����Ƶ�.
	class ComboBox : public ICtrl,public ComboBoxPtr
	{
	public:
		ComboBox();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//CoboBoxEx Control ������ʾͼ��.ComboBoxEx��һ������ķ����CBES_EX_CASESENSITIVE,���Զ��г���Ա.
	class ComboBoxEx : public ICtrl,public ComboBoxExPtr//ComboBoxEx��֧�������ִ��йصĺ���.
	{
	public:
		ComboBoxEx();
		void operator = (HWND hWnd){Attach(hWnd);}
	};
	//TreeView Control
	/*TreeView�ؼ�������ν��������ʾ�ؼ�,����Ԫ�������ηֲ�,����һ��Ӧ�÷ǳ��㷺�Ľṹ,�����ļ�ϵͳ.TreeView�ؼ����԰�
	����ͼ���б�ؼ�,һ��������ͨ״̬��,һ������ѡ��״̬��.
	class TreeView : public ICtrl,public TreeViewPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		/*base::Function<void,LPNMTREEVIEW> OnBeginDrag;	//��ʼ��ҷ
		base::Function<void,LPNMTREEVIEW> OnBeginRDrag;	//��ʼ�Ҽ���ҷ
		base::Function<bool,LPNMTVDISPINFO> OnBeginEdit;	//��ʼ�༭��ǩ,����1,��ȡ���༭��ǩ.
		base::Function<bool,LPNMTVDISPINFO> OnEndEdit;	//��ɱ༭��ǩ,����1,��ȡ���༭�ı�.
		base::Function<void,LPNMTREEVIEW> OnDeleteItem;	//ɾ��Ԫ��.
		base::Function<void,LPNMTREEVIEW> OnExpanded;		//չ�����۵���һ������.
		base::Function<bool,LPNMTREEVIEW> OnExpanding;	//��Ҫչ�����۵�һ������,����1ȡ������.
		base::Function<void,LPNMTREEVIEW> OnSelChanged;	//��ǰѡ����仯.
		base::Function<bool,LPNMTREEVIEW> OnSelChanging;	//��Ҫ�ı䵱ǰѡ����,����1��ֹ����.
		base::Function<UINT,LPNMTVKEYDOWN> OnChar;		//�а�������,����ִ����������,���MSDN˵��.
		
		TreeView();
	};
	//ToolBar Control
	class ToolBar : public ICtrl,public ToolBarPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ToolBar();
		//���ɿؼ�.
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
	//ToolTip Control ����ؼ�����������6.0�汾�ؼ�ʱ�ſ���.Ҳ����˵,����Ҫ��MANIFEST.
	class ToolTip : public ICtrl,public ToolTipPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		ToolTip(void);
		INT_PTR Create(HWND hParent);
	};

	//TabPage�ؼ��������ṩ�Կؼ��Ĳ���,��ֻ�ṩһ�����,Ȼ����ҳ��ť��������Ӧ����Ϣ�¼�.����Щ�¼��д���ؼ�����Ϊ,ʹ�����ֵ������ڴ��ڲ�ͬ��ҳ��.
	class TabPage : public ICtrl,public TabPagePtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		TabPage();
	};
	//��ʾ��ϼ���һ���ṹ.
	//�ȼ��ؼ�
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
		//ListView ����չ����������֮�����������������ã������ɴ���ʱ�ķ�����չ��������ö��������á�add��ʾ��ӻ����Ƴ�exStyleָ���ķ��
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
	//UpDown �ؼ����򸸴��ڷ��͹�������Ϣ
	class UpDown : public ICtrl,public UpDownPtr
	{
	public:
		void operator = (HWND hWnd){Attach(hWnd);}
		UpDown();
		//ֱ������һ�� UDS_SETBUDDYINT UDS_ALIGNRIGHT UDS_ARROWKEYS ���� UpDown �ؼ���nUpeer�����ޣ�nLower�����ޣ�nPos����ǰλ�á�
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
	/*�Ի���������Ԥ�ȷ���255���ؼ�,����Ŀؼ������ڳ�ʼ���Ի����ʱ���ֶ�����.

	�Ի�����Ȼ��һ�����ڣ�ʵ���϶Ի����ǳ��洰�ڵ�һ�ְ�װ����Ҫ�ı��������ط���
	1. ���ɺ���������CreateWindow��CreateWindowEx���������°�װ�õĺ�����
	DialogBoxParam��ģ̬�Ի��򣩺�CreateDialogParam����ģ̬�Ի��򣩣����Ҳ���Ҫ
	ע����������ʵ�Ի������һ���ض��Ĵ����࣬���������ÿؼ�һ��Ԥ��ע����ˡ�
	2. ͬ�����ں���Ҳ�����˰�װ���Ի���Ĵ��ں�����Ȼ��ʽ�Ϻ���ͨ������һ���ģ�
	���Ҵ����ϢҲ��δ������ͷ���������������ǶԻ��򲻷��ͺܶ���Ϣ������WM_CREATE��
	������WM_INITDIALOG���档���ҶԻ���û��һ��DefDialogProc������ȱʡ���أ�
	�Ի��򴰿ں�������ֵһ����FALSE����ô�Ի���ᰴ�ڲ���ȱʡ��ʽ������֮����
	TRUE����ô�ڲ��ͺ��������Ϣ�������κδ���

	��Щ�ض���Ϣ��Ҫ��������ֵ������WM_CTRLCOLORDLG����Ϣ������һ����ˢ�������ô
	�����ˢ���������ƶԻ���ı��������ҶԻ���Ͳ��ٷ���WM_ERASEBKGROUND��Ϣ��

	IDlg �� IWnd �ߵ���Ϣ������ȫ��ͬ������IDlg���յ���Ϣ����ʱ������DefWindowProc��
	���ǵ���_defDlgProcһ��ֱ�ӷ��� 0 �ĺ�����

	ģ̬�Ի���Ĺرհ�ť������WM_COMMAND��OnCommand�����IDCANCEL��֧����ȷ����Close
	���������򲻻��Զ��رա�//
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
	//ģ̬�Ի���ģ̬�Ի������Լ�����Ϣѭ�����������ڼ䣬����Ϣѭ�����ղ�����Ϣ��
	class  ModelDialog : public IDlg
	{
	public:
		ModelDialog();
		//����һ��ģ̬�Ի���,ֱ���Ի�������֮ǰ,����������᷵��.��������µķ���ֵ�ǵ���EndDialog���õ�ֵ.�����ΪhParent
		//����������,����ֵΪ0,�������󷵻�-1.��ʹ�Ի��������Ϊ���ɼ�,�������Ҳ����ʾ�Ի���.
		INT_PTR Create(HWND hParent = 0);
		//�رնԻ���codeָ���˳�����.
		bool Close(int code = 0);
	};
	//��ģ̬�Ի���.
	class  NormalDialog : public IDlg
	{
	public:
		NormalDialog();
		//����һ����ģ̬�Ի���,���ضԻ���ľ��.
		INT_PTR Create(HWND hParent = 0);
		//�رնԻ���.
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
	//wnd����Ҫ��λӛ䛵Ĵ��ڣ�name��ָ��һ�����Q�����ӛ���Ҫ���@�����Q��^�ִ��ڡ�
	bool AddWnd(IWnd* wnd,LPCWSTR name);
	//���WndPosRestore��������ӵĴ����N��������{���@�������Ƴ������ĳɆT�����Ļ��{��
	bool RemoveWnd(IWnd* wnd);
	//����û��HOOK WM_CRAET��Ϣ�Ĵ����ں��ʵ�ʱ������������ģ��
	void OnCreate(HWND hwnd);
	//����û��HOOK WM_DESTROY��Ϣ�Ĵ����ں��ʵ�ʱ������������ģ��
	void OnDestroy(HWND hwnd);
};
//ע��һ��wnd�������������ܹ�����WM_CREATE��WM_DESTROY��Ϣ������������Ϣ��ֱ����úͼ�¼���ڵ�λ�ô�С������
void WPRRegisterWnd(IWnd* wnd,LPCWSTR name);
//���һ��wnd������Զ�λ�ô�С���ܡ�
void WPRUnRegisterWnd(IWnd* wnd);
void LoadWndFunc(Handle<Object>& glb);
}