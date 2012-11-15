#include "stdafx.h"
#include "innerv8.h"
namespace v8{
	Ctrl::Ctrl(){
		Param.New();
		Param->AddStyle(WS_TABSTOP);
		Param->Location.SetValue(10,10);
		Param->Size.SetValue(100,100);
	}
	Ctrl::~Ctrl(){
	}
	INT_PTR Ctrl::Create(HWND hParent){
		if(_Handle) return 0;
		Param->Style |= WS_CHILD;
		_Handle = ::CreateWindowEx(Param->ExStyle,Param->ClassName,Param->Title,Param->Style,Param->Location.x,Param->Location.y,Param->Size.x,Param->Size.y,hParent,Param->Menu,GetModuleHandle(0),Param->lParam);
		if(_Handle==0){
			base::Warning(L"Ctrl.Create failed");
			return 0;
		}
		if(_BkgndColor.IsNull()) _BkgndColor.Create(RGB(236,233,216));
		if(_Font.IsNull()) _Font.Create();
		return (INT_PTR)_Handle;
	}
	//class Edit
	void Edit::ShowToolTip(LPCWSTR tip,LPCWSTR title,int tti){
		EDITBALLOONTIP ebt;
		ebt.cbStruct = sizeof(ebt);
		ebt.ttiIcon = tti;
		ebt.pszText = tip;
		ebt.pszTitle = title;
		Edit_ShowBalloonTip(_Handle,&ebt);
	}
	void Edit::HideToolTip(){
		Edit_HideBalloonTip(_Handle);
	}
	Edit::Edit(){
		Param->ClassName = L"edit";
		Param->ExStyle = WS_EX_CLIENTEDGE;
		Param->Style |= ES_AUTOHSCROLL;
		_BkgndColor.Create(0xffffff);
	}
}