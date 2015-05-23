#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

#include <atlstr.h>
#include "classifyactionsbase.h"
#include "ClassSubMenu.h"

inline CString _get_value_string( CString strFileName, CString strSection, CString strKey )
{
	TCHAR szT[4096];
	DWORD dw = ::GetPrivateProfileString( strSection, strKey,
		"", szT, sizeof( szT ) / sizeof( szT[0] ), strFileName );
	if( dw > 0 )
		return szT;
	return "";
} 
class PtrList;

static bool is_phased(IUnknown* unk)
{
	if(unk==0) return false;		

	bool ret = false;
	INamedDataPtr nd = unk;
	if(nd)
	{
		variant_t value;
		nd->GetValue(bstr_t("Phases"), &value);
		ret = (value.iVal!= 0);
	}
	
	return ret;
}
//SetClassPhases -работает только с фазовым объектным листом.
//При изменении класса фазы объединяет её с фазой того же класса if any.
//Возвращает результат объединения 
static IUnknown* SetPhaseClass(IUnknown* olist,		//объектный лист
							   IUnknown* object,	//изменяемый объект
							   CString& classifier,	//классификатор
							   int nClass,			//новый класс
							   PtrList & affected	//список объектов, участвовавших в объединении
							   );

static BOOL CalculatePhase(IUnknown* unkList, IUnknown* pUnkPhase, BOOL bContoured);
/////////////////////////////////////////////////////////////////////////////

class CObjectSetClass : public CAction, public CClassifyActionsBase,
		public CUndoneableActionImpl,
		public _dyncreate_t<CObjectSetClass>
{
protected:
public:
	route_unknown();
	CObjectSetClass();
	~CObjectSetClass();
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	com_call DoInvoke();
	com_call GetActionState( DWORD *pdwState );
//IUndoneableAction
	com_call Undo();
	com_call Redo();
private:
	INamedDataObject2Ptr m_ptrList;
	bool m_bPhased;
}; 

//---------------------------------------------------------------------------
class CObjectSetClassInfo : public _ainfo_t<ID_ACTION_OBJECTSETCLASS, _dyncreate_t<CObjectSetClass>::CreateObject, 0>,
public _dyncreate_t<CObjectSetClassInfo>
{
        route_unknown();
public:
	CObjectSetClassInfo() {}
	arg *args() {return s_pargs;}
	com_call GetUIUnknown( IUnknown **ppunkinterface );
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

#define ACTION_UI( class_name, ui_name )									\
class __ActionUIInfo##class_name											\
{																			\
public:	__ActionUIInfo##class_name()										\
	{__ActionInfo##class_name##::s_pclassUI = RUNTIME_CLASS( ui_name );}	\
};																			\
__ActionUIInfo##class_name	__UIarg##class_name##Helper;


/////////////////////////////////////////////////////////////////////////////

class CSwitchClassFile : public CAction,
		public _dyncreate_t<CSwitchClassFile>
{
protected:
public:
	route_unknown();
	CSwitchClassFile();
	~CSwitchClassFile();
public:
	com_call DoInvoke();
}; 

//---------------------------------------------------------------------------
class CSwitchClassFileInfo : public _ainfo_t<ID_ACTION_SWITCHCLASSFILE, _dyncreate_t<CSwitchClassFile>::CreateObject, 0>,
public _dyncreate_t<CSwitchClassFileInfo>
{
        route_unknown();
public:
	CSwitchClassFileInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

// vanek - 31.10.2003
/////////////////////////////////////////////////////////////////////////////
class CActionGetObjectDescription : public CAction,
		public _dyncreate_t<CActionGetObjectDescription>
{
protected:
public:
	route_unknown();
	CActionGetObjectDescription(){};
public:
	com_call DoInvoke();
	com_call GetActionState( DWORD *pdwState );
}; 

//---------------------------------------------------------------------------
class CActionGetObjectDescriptionInfo : public _ainfo_t<ID_ACTION_GETOBJDESCR, _dyncreate_t<CActionGetObjectDescription>::CreateObject, 0>,
public _dyncreate_t<CActionGetObjectDescriptionInfo>
{
        route_unknown();
public:
	CActionGetObjectDescriptionInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

