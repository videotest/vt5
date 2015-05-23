#pragma once
#include "stdafx.h"
#include "action_main.h"
#include "resource.h"
#include "\vt5\common2\misc_utils.h"
#include "\vt5\ifaces\docview5.h"
#include "\vt5\ifaces\dbase.h"
#include "atlstr.h"
#include "nameconsts.h"
#include "impl_long.h"

/**********************************************************************************/

//////////////////////////////////////////////////////////////////////
//
//	class CActionExportBrightness 
//
//////////////////////////////////////////////////////////////////////
class CActionExportBrightness :	public CAction,
								public CLongOperationImpl,
								public _dyncreate_t<CActionExportBrightness>
{
public:
	route_unknown();
	virtual IUnknown*	DoGetInterface( const IID &iid );
public:
	CActionExportBrightness();
	virtual ~CActionExportBrightness();

	com_call	GetActionState(DWORD *pdwState);
	com_call	DoInvoke();

};

//////////////////////////////////////////////////////////////////////
//
//	class CActionExportBrightnessInfo
//
//////////////////////////////////////////////////////////////////////
class CActionExportBrightnessInfo : public _ainfo_t<IDS_ACTION_EXPORT_BRIGHTNESS, _dyncreate_t<CActionExportBrightness>::CreateObject, 0>,
							public _dyncreate_t<CActionExportBrightnessInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return szTargetAnydoc;}
	static arg	s_pargs[];
};

/**********************************************************************************/

//////////////////////////////////////////////////////////////////////
//
//	class CActionSetActiveImageDB
//
//////////////////////////////////////////////////////////////////////
class CActionSetActiveImageDB :	public CAction,
								public CLongOperationImpl,
								public _dyncreate_t<CActionSetActiveImageDB>
{
public:
	route_unknown();
public:
	CActionSetActiveImageDB();
	virtual ~CActionSetActiveImageDB();

	//com_call	GetActionState(DWORD *pdwState);
	com_call	DoInvoke();

};

//////////////////////////////////////////////////////////////////////
//
//	class CActionSetActiveImageDBInfo
//
//////////////////////////////////////////////////////////////////////
class CActionSetActiveImageDBInfo : public _ainfo_t<IDS_ACTION_SETACTIVE_IMAGEDB, _dyncreate_t<CActionSetActiveImageDB>::CreateObject, 0>,
							public _dyncreate_t<CActionSetActiveImageDBInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return szTargetAnydoc;}
	static arg	s_pargs[];
};