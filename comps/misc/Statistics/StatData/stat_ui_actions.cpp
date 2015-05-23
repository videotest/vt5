#include "stdafx.h"
#include "stat_ui_actions.h"
#include "stat_consts.h"

//////////////////////////////////////////////////////////////////////
//
//	class CHideStatTableColumn
//
//////////////////////////////////////////////////////////////////////
HRESULT	CHideStatTableColumn::DoInvoke()
{
	IStatTableViewPtr ptr_view = m_ptrTarget;
	if( ptr_view == 0 )		return S_FALSE;
	long lcolumn = -1;
	ptr_view->GetActiveColumn( &lcolumn );
	if( lcolumn == -1 )		return S_FALSE;

	ptr_view->HideColumn( lcolumn );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CHideStatTableColumn::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;
	*pdwState = 0;
	IStatTableViewPtr ptr_view = m_ptrTarget;
	if( ptr_view )
	{

		long lcolumn = -1;
		ptr_view->GetActiveColumn( &lcolumn );
		if( lcolumn != -1 )
			*pdwState = afEnabled;
	}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	class CShowAllStatTableColumns
//
//////////////////////////////////////////////////////////////////////
HRESULT	CShowAllStatTableColumns::DoInvoke()
{
	IStatTableViewPtr ptr_view = m_ptrTarget;
	if( ptr_view == 0 )		return S_FALSE;
	ptr_view->ShowAllColumns( );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CShowAllStatTableColumns::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;
	*pdwState = 0;
	IStatTableViewPtr ptr_view = m_ptrTarget;
	if( ptr_view )
		*pdwState = afEnabled;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	class CCustomizeStatTableColumns
//
//////////////////////////////////////////////////////////////////////
HRESULT	CCustomizeStatTableColumns::DoInvoke()
{
	IStatTableViewPtr ptr_view = m_ptrTarget;
	if( ptr_view == 0 )		return S_FALSE;
	ptr_view->CustomizeColumns( );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CCustomizeStatTableColumns::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;
	*pdwState = 0;
	IStatTableViewPtr ptr_view = m_ptrTarget;
	if( ptr_view )
		*pdwState = afEnabled;
	return S_OK;
}
