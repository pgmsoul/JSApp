#pragma once
namespace v8{
	class Ctrl : public WndPtr{
	protected:
		base::gdi::Color	_FontColor;			//控件的前景色(字体颜色).
		base::gdi::Font		_Font;				//这个成员用于显示的字体.
		base::gdi::Brush	_BkgndColor;		//背景色
	public:
		base::Object<CreateStruct>	Param;		//这个成员用于设置窗口生成时的参数,它有自己的默认参数,只需要设置想改变的参数.
		//构造函数。
		Ctrl();
		~Ctrl();
		//根据 Param 参数生成控件，并且重新设置消息函数。
		INT_PTR Create(HWND parent);
		//绑定一个已经生成的窗口，并且重新设置消息函数。
		bool Attach(HWND ctrl);
		//解除绑定的窗口，释放消息函数为原来的值。
		HWND Detach();
	};
	//Edit Control
	class Edit : public Ctrl{
	public:
		Edit();
		//返回控件用于存储字串的空间,使用LocalLock函数和LocalUnlock函数来获取和释放内存句柄.不要向这段内存写入数据,控件是自己管理这段内存的.
		HLOCAL LockTextBuffer(){return (HLOCAL)SendMessage(EM_GETHANDLE,0,0);}
		//这个函数设置控件的字串缓存,它会清空UNDO操作和MODIFY标记.hMem可以是自己分配的内存(LocalLock),或者是前面的函数返回的内存.
		void SetTextBuffer(HLOCAL hMem){SendMessage(EM_SETHANDLE,hMem,0);}
		//设置控件是否有只读属性(ES_READONLY风格).
		bool SetReadOnly(bool readonly){return SendMessage(EM_SETREADONLY,readonly,0)!=0;}
		//滚动到光标位置.
		void ScrollCaret(){SendMessage(EM_SCROLLCARET,0,0);}
		//返回控件包含的文本行数.
		int GetLineCount(){return (int)SendMessage(EM_GETLINECOUNT,0,0);}
		//用已知文本替换选中文本.
		void ReplaceSelText(LPCWSTR str){SendMessage(EM_REPLACESEL,1,str);}
		//返回已经选择文本的起始和结束位置.
		base::Twin16 GetSelect(){return (base::Twin16)SendMessage(EM_GETSEL,0,0);}
		//设置选择文本的起始和结束位置.
		void SetSelect(base::Twin16 sel){SendMessage(EM_SETSEL,sel.x,sel.y);}
		//显示一个汽泡提示信息.
		void ShowToolTip(LPCWSTR tip,LPCWSTR title = 0,int tti = TTI_NONE);
		//隐藏显示的汽泡提示.
		void HideToolTip();
		//UNDO操作
		bool UnDo(){return SendMessage(EM_UNDO,0,0)!=0;}
	};
}