#pragma once

#include "stdafx.h"
#include "com_unknown.h"
#include "PopupInt.h"

class CClassSubMenu :
	public ComObjectBase,
	public ISubMenu,
	public _dyncreate_t<CClassSubMenu>
{
public:
	route_unknown();

	CClassSubMenu(void);
	~CClassSubMenu(void);

	UINT	m_uiStartItemID;

	//ISubMenu
	com_call SetStartItemID( UINT uiItemID );
	com_call SetSingleObjectName( BSTR bstrObjectName );
	com_call GetFirstItemPos(TPOS* plPos);
	com_call GetNextItem(	
		UINT* puiFlags, 
		UINT* puiItemID, 
		BSTR* pbstrText,				
		UINT* puiParentID,
		TPOS* plPos);
	com_call OnCommand( UINT uiCmd );	

	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_ISubMenu )return (ISubMenu*)this;
		return ComObjectBase::DoGetInterface( iid );
	}

private:
	bool m_bShowUnknown;
};
