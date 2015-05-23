#pragma once

#include "action_main.h"
#include "action_misc.h"

#include "statistics.h"

#include "action_undo.h"
#include "resource.h"

#include "stat_types.h"

//////////////////////////////////////////////////////////////////////
//
//	class CCreateStatTable
//
//////////////////////////////////////////////////////////////////////
class CCreateStatTable : public CAction,
						public CUndoneableActionImpl,
						public _dyncreate_t<CCreateStatTable>
{
public:
	route_unknown();
	virtual IUnknown*	DoGetInterface( const IID &iid );
public:
	CCreateStatTable();
	virtual ~CCreateStatTable();

	//invoke
	com_call	DoInvoke();
	bool		create_stat_group( IStatTable* pi_table, INamedDataObject2* pi_ol, stat_group_ex* pgroup );
	bool		test_exist( IStatTable* pi_table, INamedDataObject2* pi_ol, stat_group_ex* pgroup_new );
	bool		process_table(	IStatTable* pi_table, INamedDataObject2* pi_ol );
	
	com_call	GetActionState( DWORD *pdwState );
	IUnknown*	get_object_list();
};

//////////////////////////////////////////////////////////////////////
class CCreateStatTableInfo : public _ainfo_t<IDS_CREATE_STAT_TABLE, _dyncreate_t<CCreateStatTable>::CreateObject, 0>,
							public _dyncreate_t<CCreateStatTableInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};



//////////////////////////////////////////////////////////////////////
//
//	class CCreateStatTable
//
//////////////////////////////////////////////////////////////////////
class CShowStatTable : public CActionShowView,
						public _dyncreate_t<CShowStatTable>
{
public:
	route_unknown();	
public:
	virtual _bstr_t GetViewProgID();
};


/////////////////////////////////////////////////////////////////////////////
class CShowStatTableInfo : public _ainfo_t<IDS_SHOW_STAT_TABLE, _dyncreate_t<CShowStatTable>::CreateObject, 0>,
							public _dyncreate_t<CShowStatTableInfo>
{
	route_unknown();
public:
	CShowStatTableInfo(){}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetFrame;}	
};

//////////////////////////////////////////////////////////////////////
//
//	class CDeleteObjectFromStatTable
//
//////////////////////////////////////////////////////////////////////
class CStatTableEditBase : public CAction
{
protected:
	com_call		GetActionState( DWORD *pdwState );
	IUnknown*		get_stat_table();	
public:
	virtual bstr_t	GetTableNameTarget();
};


//////////////////////////////////////////////////////////////////////
//
//	class CExportStatTable
//
//////////////////////////////////////////////////////////////////////
class CExportStatTable : public CStatTableEditBase,
						public _dyncreate_t<CExportStatTable>
{
public:
	route_unknown();
public:
	CExportStatTable();
	virtual ~CExportStatTable();

	//invoke
	com_call	DoInvoke();

	bool		export_table_to_stream( IStatTable* pi_table, IStream* pi_stream );
};

//////////////////////////////////////////////////////////////////////
class CExportStatTableInfo : public _ainfo_t<IDS_EXPORT_STAT_TABLE, _dyncreate_t<CExportStatTable>::CreateObject, 0>,
							public _dyncreate_t<CExportStatTableInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

//////////////////////////////////////////////////////////////////////
//
//	class CDeleteObjectFromStatTable
//
//////////////////////////////////////////////////////////////////////
class CDeleteObjectFromStatTable :	public CStatTableEditBase,
									public CUndoneableActionImpl,
									public _dyncreate_t<CDeleteObjectFromStatTable>
{
public:
	route_unknown();
	virtual IUnknown*	DoGetInterface( const IID &iid );
public:
	CDeleteObjectFromStatTable();
	virtual ~CDeleteObjectFromStatTable();

	//invoke
	com_call	DoInvoke();
};

//////////////////////////////////////////////////////////////////////
class CDeleteObjectFromStatTableInfo : public _ainfo_t<IDS_DELETE_OBJECTS, _dyncreate_t<CDeleteObjectFromStatTable>::CreateObject, 0>,
							public _dyncreate_t<CDeleteObjectFromStatTableInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

//////////////////////////////////////////////////////////////////////
//
//	class CFilterStatTable
//
//////////////////////////////////////////////////////////////////////
class CFilterStatTable :	public CStatTableEditBase,
							public CUndoneableActionImpl,
							public _dyncreate_t<CFilterStatTable>							
{
public:
	route_unknown();
	virtual IUnknown*	DoGetInterface( const IID &iid );
public:
	CFilterStatTable();
	virtual ~CFilterStatTable();

	//invoke
	com_call	DoInvoke();
	bool		filter_table( IStatTable* pi_table, IStatTable* pi_table_new );
};

//////////////////////////////////////////////////////////////////////
class CFilterStatTableInfo : public _ainfo_t<IDS_FILTER_STAT_TABLE, _dyncreate_t<CFilterStatTable>::CreateObject, 0>,
							public _dyncreate_t<CFilterStatTableInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

//////////////////////////////////////////////////////////////////////
//
//	class CSewStatTable
//
//////////////////////////////////////////////////////////////////////
class CSewStatTable :	public CAction,
							public CUndoneableActionImpl,
							public _dyncreate_t<CSewStatTable>							
{
public:
	route_unknown();
	virtual IUnknown*	DoGetInterface( const IID &iid );
public:
	CSewStatTable();
	virtual ~CSewStatTable();

	//invoke
	com_call		DoInvoke();
	bool			process( IStatTable* pi_table, IStatTable* pi_table_add );

	com_call		GetActionState( DWORD *pdwState );
	void			get_stat_tables( IUnknown** ppunk1, IUnknown** ppunk2 );	

};

//////////////////////////////////////////////////////////////////////
class CSewStatTableInfo : public _ainfo_t<IDS_ADD_STAT_TABLE, _dyncreate_t<CSewStatTable>::CreateObject, 0>,
							public _dyncreate_t<CSewStatTableInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
