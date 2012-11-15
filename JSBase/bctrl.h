#pragma once
namespace v8{
	class Ctrl : public WndPtr{
	protected:
		base::gdi::Color	_FontColor;			//�ؼ���ǰ��ɫ(������ɫ).
		base::gdi::Font		_Font;				//�����Ա������ʾ������.
		base::gdi::Brush	_BkgndColor;		//����ɫ
	public:
		base::Object<CreateStruct>	Param;		//�����Ա�������ô�������ʱ�Ĳ���,�����Լ���Ĭ�ϲ���,ֻ��Ҫ������ı�Ĳ���.
		//���캯����
		Ctrl();
		~Ctrl();
		//���� Param �������ɿؼ�����������������Ϣ������
		INT_PTR Create(HWND parent);
		//��һ���Ѿ����ɵĴ��ڣ���������������Ϣ������
		bool Attach(HWND ctrl);
		//����󶨵Ĵ��ڣ��ͷ���Ϣ����Ϊԭ����ֵ��
		HWND Detach();
	};
	//Edit Control
	class Edit : public Ctrl{
	public:
		Edit();
		//���ؿؼ����ڴ洢�ִ��Ŀռ�,ʹ��LocalLock������LocalUnlock��������ȡ���ͷ��ڴ���.��Ҫ������ڴ�д������,�ؼ����Լ���������ڴ��.
		HLOCAL LockTextBuffer(){return (HLOCAL)SendMessage(EM_GETHANDLE,0,0);}
		//����������ÿؼ����ִ�����,�������UNDO������MODIFY���.hMem�������Լ�������ڴ�(LocalLock),������ǰ��ĺ������ص��ڴ�.
		void SetTextBuffer(HLOCAL hMem){SendMessage(EM_SETHANDLE,hMem,0);}
		//���ÿؼ��Ƿ���ֻ������(ES_READONLY���).
		bool SetReadOnly(bool readonly){return SendMessage(EM_SETREADONLY,readonly,0)!=0;}
		//���������λ��.
		void ScrollCaret(){SendMessage(EM_SCROLLCARET,0,0);}
		//���ؿؼ��������ı�����.
		int GetLineCount(){return (int)SendMessage(EM_GETLINECOUNT,0,0);}
		//����֪�ı��滻ѡ���ı�.
		void ReplaceSelText(LPCWSTR str){SendMessage(EM_REPLACESEL,1,str);}
		//�����Ѿ�ѡ���ı�����ʼ�ͽ���λ��.
		base::Twin16 GetSelect(){return (base::Twin16)SendMessage(EM_GETSEL,0,0);}
		//����ѡ���ı�����ʼ�ͽ���λ��.
		void SetSelect(base::Twin16 sel){SendMessage(EM_SETSEL,sel.x,sel.y);}
		//��ʾһ��������ʾ��Ϣ.
		void ShowToolTip(LPCWSTR tip,LPCWSTR title = 0,int tti = TTI_NONE);
		//������ʾ��������ʾ.
		void HideToolTip();
		//UNDO����
		bool UnDo(){return SendMessage(EM_UNDO,0,0)!=0;}
	};
}