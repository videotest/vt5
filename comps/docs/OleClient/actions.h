#ifndef __oleclient_actions_h__
#define __oleclient_actions_h__

#include "resource.h"
#include "action_main.h"
#include "action_undo.h"

#include "nameconsts.h"

IUnknown* GetActiveOleObjectFromDoc( IUnknown* punkDoc, _bstr_t* pbstrName );


/////////////////////////////////////////////////////////////////////////////
class CInsertOleObject : public CAction,
						public CUndoneableActionImpl,
						public _dyncreate_t<CInsertOleObject>
{
public:
	route_unknown();
public:
	CInsertOleObject();
	virtual ~CInsertOleObject();

	com_call DoInvoke();	

protected:
	bool				m_bOverwriteActive;
	virtual		bool	CreateOleObjectContext( IUnknown* punkOleObject );
	
};


/////////////////////////////////////////////////////////////////////////////
class CInsertOleObjectInfo : public _ainfo_t<IDS_INSERT_OLE_OBJECT, _dyncreate_t<CInsertOleObject>::CreateObject, 0>,
							public _dyncreate_t<CInsertOleObjectInfo>
{
	route_unknown();
public:
	CInsertOleObjectInfo();
	~CInsertOleObjectInfo();

	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

/////////////////////////////////////////////////////////////////////////////
class CInsertOleObjectToExist : public CInsertOleObject,
						//public CUndoneableActionImpl,
						public _dyncreate_t<CInsertOleObjectToExist>
{
public:
	route_unknown();
public:
	CInsertOleObjectToExist();
	virtual ~CInsertOleObjectToExist();	
	com_call GetActionState( DWORD *pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CInsertOleObjectToExistInfo : public _ainfo_t<IDS_INSERT_OLE_OBJECT_TO_EXIST, _dyncreate_t<CInsertOleObjectToExist>::CreateObject, 0>,
							public _dyncreate_t<CInsertOleObjectToExistInfo>
{
	route_unknown();
public:
	CInsertOleObjectToExistInfo(){}
	~CInsertOleObjectToExistInfo(){}

	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];

};


/////////////////////////////////////////////////////////////////////////////
class CInsertOleObjectFromClipboard : public CInsertOleObject,						
						public _dyncreate_t<CInsertOleObjectFromClipboard>
{
public:
	route_unknown();
public:
	CInsertOleObjectFromClipboard();
	virtual ~CInsertOleObjectFromClipboard();

	virtual		bool	CreateOleObjectContext( IUnknown* punkOleObject );

};


/////////////////////////////////////////////////////////////////////////////
class CInsertOleObjectFromClipboardInfo : public _ainfo_t<IDS_CREATE_OLE_OBJECT_FROM_CLIPBOARD, _dyncreate_t<CInsertOleObjectFromClipboard>::CreateObject, 0>,
							public _dyncreate_t<CInsertOleObjectFromClipboardInfo>
{
	route_unknown();
public:
	CInsertOleObjectFromClipboardInfo()
	{
	}
	arg	*args()	{return 0;}
	virtual _bstr_t target()			{return "anydoc";}	
};


/////////////////////////////////////////////////////////////////////////////
class CInsertOleObjectFromClipboardToExist : public CInsertOleObjectFromClipboard,
						//public CUndoneableActionImpl,
						public _dyncreate_t<CInsertOleObjectFromClipboardToExist>
{
public:
	route_unknown();
public:
	CInsertOleObjectFromClipboardToExist();
	virtual ~CInsertOleObjectFromClipboardToExist();	
	com_call GetActionState( DWORD *pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CInsertOleObjectFromClipboardToExistInfo : public _ainfo_t<IDS_CREATE_OLE_OBJECT_FROM_CLIPBOARD_TO_EXIST, _dyncreate_t<CInsertOleObjectFromClipboardToExist>::CreateObject, 0>,
							public _dyncreate_t<CInsertOleObjectFromClipboardToExistInfo>
{
	route_unknown();
public:
	CInsertOleObjectFromClipboardToExistInfo()
	{
	}
	arg	*args()	{return 0;}
	virtual _bstr_t target()			{return "anydoc";}	
};



/////////////////////////////////////////////////////////////////////////////
class CShowOleObject : public CAction,
						public _dyncreate_t<CShowOleObject>
{
public:
	route_unknown();
public:
	CShowOleObject();
	virtual ~CShowOleObject();

	com_call DoInvoke();
	com_call GetActionState( DWORD *pdwState );
};


/////////////////////////////////////////////////////////////////////////////
class CShowOleObjectInfo : public _ainfo_t<IDS_SHOW_OLE_OBJECT, _dyncreate_t<CShowOleObject>::CreateObject, 0>,
							public _dyncreate_t<CShowOleObjectInfo>
{
	route_unknown();
public:
	CShowOleObjectInfo()
	{
	}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetFrame;}	
};



/////////////////////////////////////////////////////////////////////////////
class CActivateOleObject : public CAction,
						public _dyncreate_t<CActivateOleObject>
{
public:
	route_unknown();
public:
	CActivateOleObject();
	virtual ~CActivateOleObject();

	com_call DoInvoke();
	com_call GetActionState( DWORD *pdwState );

	IUnknownPtr GetActiveView();
};


/////////////////////////////////////////////////////////////////////////////
class CActivateOleObjectInfo : public _ainfo_t<IDS_ACTIVATE_OLE_OBJECT, _dyncreate_t<CActivateOleObject>::CreateObject, 0>,
							public _dyncreate_t<CActivateOleObjectInfo>
{
	route_unknown();
public:
	CActivateOleObjectInfo()
	{
	}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return "anydoc";}	
};


/////////////////////////////////////////////////////////////////////////////
class CDeactivateOleObject : public CAction,
						public _dyncreate_t<CDeactivateOleObject>
{
public:
	route_unknown();
public:
	CDeactivateOleObject();
	virtual ~CDeactivateOleObject();

	com_call DoInvoke();
};


/////////////////////////////////////////////////////////////////////////////
class CDeactivateOleObjectInfo : public _ainfo_t<IDS_DEACTIVATE_OLE_OBJECT, _dyncreate_t<CDeactivateOleObject>::CreateObject, 0>,
							public _dyncreate_t<CDeactivateOleObjectInfo>
{
	route_unknown();
public:
	CDeactivateOleObjectInfo()
	{
	}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetViewSite;}	
};



/////////////////////////////////////////////////////////////////////////////
class CShowOleObjectContent : public CAction,
						public _dyncreate_t<CShowOleObjectContent>
{
public:
	route_unknown();
public:
	CShowOleObjectContent();
	virtual ~CShowOleObjectContent();

	com_call DoInvoke();
};


/////////////////////////////////////////////////////////////////////////////
class CShowOleObjectContentInfo : public _ainfo_t<IDS_SHOW_OLE_OBJECT_CONTENT, _dyncreate_t<CShowOleObjectContent>::CreateObject, 0>,
							public _dyncreate_t<CShowOleObjectContentInfo>
{
	route_unknown();
public:
	CShowOleObjectContentInfo()
	{
	}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetViewSite;}	
};


/////////////////////////////////////////////////////////////////////////////
class CUpdateOleObject : public CAction,
						public _dyncreate_t<CUpdateOleObject>
{
public:
	route_unknown();
public:
	CUpdateOleObject();
	virtual ~CUpdateOleObject();

	com_call DoInvoke();
};


/////////////////////////////////////////////////////////////////////////////
class CUpdateOleObjectInfo : public _ainfo_t<IDS_UPDATE_OLE_OBJECT, _dyncreate_t<CUpdateOleObject>::CreateObject, 0>,
							public _dyncreate_t<CUpdateOleObjectInfo>
{
	route_unknown();
public:
	CUpdateOleObjectInfo()
	{
	}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetViewSite;}	
};


#endif //__oleclient_actions_h__
