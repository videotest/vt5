#pragma once

#include "statistics.h"
#include "win_view.h"
#include "impl_help.h"

#include "stat_grid.h"

//////////////////////////////////////////////////////////////////////
//
//	class CStatView
//
//////////////////////////////////////////////////////////////////////
class CStatView : public CWinViewBase, 
				public IPrintView,
				public IPersist,
				public CNamedObjectImpl,
				public IStatTableView,
				public CHelpInfoImpl,
				public _dyncreate_t<CStatView>
{
public:
	CStatView();
	virtual ~CStatView();

public:
	virtual IUnknown*	DoGetInterface( const IID &iid );
	
	route_unknown();
protected:
	virtual void		DoAttachObjects();	

	//mesage handlers
	virtual LRESULT		DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );

	virtual long		on_create( CREATESTRUCT *pcs );
	virtual long		on_destroy();
	virtual long		on_paint();
//	virtual long		on_erase_background( HDC hDC );		//WM_ERASEBKGND
	virtual long		on_size( int cx, int cy );			//WM_SIZE	
	virtual long		on_context_menu( HWND hWnd, int x, int y );	//WM_CONTEXTMENU
	virtual long		on_setfocus( HWND hwnd_lost );

	//interface overriden
	com_call			GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );	
	virtual void		OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//Print support
	com_call			GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX );
	com_call			GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY );
	com_call			Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags );

	//Persist Impl
	com_call			GetClassID(CLSID *pClassID ); 

	com_call			GetFirstVisibleObjectPosition( long *plpos );
	com_call			GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos );

	//IStatTableGrid
	com_call			GetColumnsCount( long* plcount );
	com_call			GetActiveColumn( long* plcolumn );
	com_call			HideColumn( long lcolumn );
	com_call			ShowAllColumns( );
	com_call			CustomizeColumns( );

	//grid
public:
	CStatGrid			m_grid;
};