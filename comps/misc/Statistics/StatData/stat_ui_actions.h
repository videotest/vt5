#pragma once

#include "action_main.h"
#include "resource.h"
#include "statistics.h"


//////////////////////////////////////////////////////////////////////
//
//	class CHideStatTableColumn
//
//////////////////////////////////////////////////////////////////////
class CHideStatTableColumn : public CAction,
						public _dyncreate_t<CHideStatTableColumn>
{
public:
	route_unknown();
public:
	//invoke
	com_call	DoInvoke();
	com_call	GetActionState( DWORD *pdwState );
};

//////////////////////////////////////////////////////////////////////
class CHideStatTableColumnInfo : public _ainfo_t<IDS_HIDE_STAT_TABLE_COLUMN, _dyncreate_t<CHideStatTableColumn>::CreateObject, 0>,
							public _dyncreate_t<CHideStatTableColumnInfo>
{
	route_unknown();
public:
	virtual _bstr_t target()			{return szTargetViewSite;}
};

//////////////////////////////////////////////////////////////////////
//
//	class CShowAllStatTableColumns
//
//////////////////////////////////////////////////////////////////////
class CShowAllStatTableColumns : public CAction,
						public _dyncreate_t<CShowAllStatTableColumns>
{
public:
	route_unknown();
public:
	//invoke
	com_call	DoInvoke();
	com_call	GetActionState( DWORD *pdwState );
};

//////////////////////////////////////////////////////////////////////
class CShowAllStatTableColumnsInfo : public _ainfo_t<IDS_SHOW_ALL_STAT_TABLE_COLUMNS, _dyncreate_t<CShowAllStatTableColumns>::CreateObject, 0>,
							public _dyncreate_t<CShowAllStatTableColumnsInfo>
{
	route_unknown();
public:
	virtual _bstr_t target()			{return szTargetViewSite;}
};

//////////////////////////////////////////////////////////////////////
//
//	class CustomizeStatTableColumns
//
//////////////////////////////////////////////////////////////////////
class CCustomizeStatTableColumns : public CAction,
						public _dyncreate_t<CCustomizeStatTableColumns>
{
public:
	route_unknown();
public:
	//invoke
	com_call	DoInvoke();
	com_call	GetActionState( DWORD *pdwState );
};

//////////////////////////////////////////////////////////////////////
class CCustomizeStatTableColumnsInfo : public _ainfo_t<IDS_CUSTOMIZE_STAT_TABLE_COLUMNS, _dyncreate_t<CCustomizeStatTableColumns>::CreateObject, 0>,
							public _dyncreate_t<CCustomizeStatTableColumnsInfo>
{
	route_unknown();
public:
	virtual _bstr_t target()			{return szTargetViewSite;}
};

