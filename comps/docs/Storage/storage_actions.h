#pragma once
#include "stdafx.h"
#include "action_main.h"
#include "action_undo.h"
#include "resource.h"
#include "\vt5\common2\misc_utils.h"

/**********************************************************************************/

//////////////////////////////////////////////////////////////////////
//
//	class CCreateStorageObj
//
//////////////////////////////////////////////////////////////////////
class CCreateStorageObj : public CAction,
						public CUndoneableActionImpl,
						public _dyncreate_t<CCreateStorageObj>
{
public:
	route_unknown();
	virtual IUnknown*	DoGetInterface( const IID &iid );
public:
	CCreateStorageObj();
	virtual ~CCreateStorageObj();

	//CAction
	com_call	DoInvoke();

protected:
	HRESULT	_get_action_user_name( BSTR *pbstrName );
};

//////////////////////////////////////////////////////////////////////
//
//	class CCreateStorageObjInfo
//
//////////////////////////////////////////////////////////////////////
class CCreateStorageObjInfo : public _ainfo_t<IDS_CREATE_STORAGE_OBJ, _dyncreate_t<CCreateStorageObj>::CreateObject, 0>,
							public _dyncreate_t<CCreateStorageObjInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return szTargetAnydoc;}
	static arg	s_pargs[];
};

/**********************************************************************************/