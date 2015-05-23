#pragma once

#include "action_filter.h"
#include "misc_utils.h"
#include "classify_int.h"

#include <atlstr.h>
#include "nameconsts.h"
#include "resource.h"

#include "classifyactionsbase.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
class CActionTeachObjectAll : public CAction, 
//							  public CClassifyActionsBase,
							  public CUndoneableActionImpl,
							  public _dyncreate_t<CActionTeachObjectAll>
{
public:
	route_unknown();
	CActionTeachObjectAll()	
	{ 
	}
	~CActionTeachObjectAll()
	{
	}
public:
	com_call DoInvoke();
//	com_call DoUndo();
//	com_call DoRedo();

//	com_call GetActionState( DWORD *pdwState );
protected:
	bool _is_avaible();
	CString _get_value_string( CString strFileName, CString strSection, CString strKey )
	{
		TCHAR szT[4096];
		DWORD dw = ::GetPrivateProfileString( strSection, strKey,
			"", szT, sizeof( szT ) / sizeof( szT[0] ), strFileName );
		if( dw > 0 )
			return szT;
		return "";
	}
}; 

//---------------------------------------------------------------------------
class CActionTeachObjectAllInfo : public _ainfo_t<ID_ACTION_TEACH_OBJECT_ALL, _dyncreate_t<CActionTeachObjectAll>::CreateObject, 0>,
	public _dyncreate_t<CActionTeachObjectAllInfo>
{
public:
    route_unknown();
	CActionTeachObjectAllInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "app";}
	static arg s_pargs[];
};
/////////////////////////////////////////////////////////////////////////////////////////////////////
