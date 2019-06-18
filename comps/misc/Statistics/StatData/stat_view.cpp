#include "stdafx.h"
#include "stat_view.h"

#include "resource.h"
#include <stdio.h>

#include "action_dlg.h"
#include "stat_consts.h"
#include "stat_utils.h"

#include "params.h"
#include "menuconst.h"
#include "misc_utils.h"
#include "MenuRegistrator.h"

#include "statistics.h"

#include "\vt5\awin\misc_list.h"


#define IDC_GRID	2022

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CStatView::CStatView()
{	
	m_bstrName		= "StatDataTable View";

	// [vanek] BT2000:4208 - 21.12.2004
	m_dwWindowFlags |= wfSupportFitDoc2Scr;

	char sz_buf[256];
	sz_buf[0] = 0;

	::LoadString( App::handle(), IDS_STAT_VIEW_NAME, sz_buf, sizeof(sz_buf) );
	
	m_bstrUserName	= sz_buf;

	if( !m_bstrUserName.length() )
		m_bstrUserName = m_bstrName;
}

//////////////////////////////////////////////////////////////////////
CStatView::~CStatView()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CStatView::DoGetInterface( const IID &iid )
{
	if( iid == IID_IPrintView ) return (IPrintView*)this;
	else if( iid == IID_IPersist ) return (IPersist*)this;
	else if( iid == IID_INamedObject2 ) return (INamedObject2*)this;
	else if( iid == IID_IStatTableView ) return (IStatTableView*)this;
	else if( iid == IID_IHelpInfo )return (IHelpInfo*)this;	
	else return CWinViewBase::DoGetInterface( iid );
}
	
//////////////////////////////////////////////////////////////////////
void CStatView::DoAttachObjects()
{
	IStatTablePtr ptr_table;
	{
		IUnknown* punk_table = ::GetActiveObjectFromContext( Unknown(), szTypeStatTable );
		ptr_table = punk_table;
		if( punk_table )
			punk_table->Release();	punk_table = 0;
	}

	m_grid.buid( ptr_table );

	if(m_grid.GetColumnCount()>0){
		::ShowWindow(m_grid.handle(), SW_SHOW );
	}else{
		::ShowWindow(m_grid.handle(), SW_HIDE );
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT	CStatView::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	LRESULT	lResult = 0;
	switch( nMsg )
	{
		case WM_CREATE:			lResult = on_create( (CREATESTRUCT*)lParam );
			break;
		case WM_DESTROY:		lResult = on_destroy();
			break;
		case WM_PAINT:			lResult = on_paint();				
			break;
		//case WM_ERASEBKGND:		lResult = on_erase_background( (HDC)wParam );
		//	break;
		case WM_SIZE:			lResult = on_size( (int)LOWORD(lParam), (int)HIWORD(lParam) );
			break;
		case WM_CONTEXTMENU:	lResult = on_context_menu( HWND(wParam), (int)GET_X_LPARAM(lParam), (int)GET_Y_LPARAM(lParam) );
			break;
		case WM_SETFOCUS:		lResult = on_setfocus( HWND(wParam) );
			break;
		case WM_HSCROLL:
			{
				if( m_grid.handle() )
					::SendMessage( m_grid.handle(), nMsg, wParam, lParam );
				lResult = 1L;
			}
			break;
		case WM_VSCROLL:
			{
				if( m_grid.handle() )
				{	/*										
					if( LOWORD(wParam) == SB_THUMBPOSITION || LOWORD(wParam) == SB_THUMBTRACK )
					{
						HWND h = m_grid.GetScrollbar( SB_VERT );
						::SetScrollPos( h, SB_CTL, HIWORD(wParam), true );
					}
					else*/
						::SendMessage( m_grid.handle(), nMsg, wParam, lParam );
				}

				lResult = 1L;
			}
			break;
		case WM_NOTIFY:		
			{
				MSG msg = {0};
				msg.message	= nMsg;
				msg.hwnd	= m_hwnd;
				msg.wParam	= wParam;
				msg.lParam	= lParam;
				::GetCursorPos( &msg.pt );				

				HWND hwnd = GetDlgItem( m_hwnd, wParam );
				long	lProcessed = false;
				long	lret_reflect = SendMessage( hwnd, WM_NOTYFYREFLECT, (WPARAM)&lProcessed, (LPARAM)&msg );
				if( lProcessed ) return lret_reflect;
				return 0;
			}
			break;
	}
	if( lResult )return lResult;

	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
long CStatView::on_create( CREATESTRUCT *pcs )
{
	_rect rc_grid( 0, 0, pcs->cx, pcs->cy );
	DWORD dw_style =	WS_CHILD | WS_VISIBLE |
						LVS_REPORT | LVS_OWNERDATA | LVS_EDITLABELS |
						WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
	DWORD dw_style_ex = 0;

	m_grid.SetParent( this );
	m_grid.create_ex( dw_style,	rc_grid, 0, m_hwnd, (HMENU)IDC_GRID, WC_LISTVIEW, dw_style_ex );

	DWORD dw_flags = 0;
	GetWindowFlags( &dw_flags );
	SetWindowFlags( dw_flags & ~wfZoomSupport );

	sptrIScrollZoomSite	ptr_szs( Unknown() );
	if( ptr_szs ) ptr_szs->SetAutoScrollMode( false );

	{
		IWindowPtr ptrWnd = Unknown();
		if( ptrWnd )
		{
			DWORD dwFlags = 0;
			ptrWnd->GetWindowFlags( &dwFlags );
			ptrWnd->SetWindowFlags( dwFlags & ~wfZoomSupport );
		}
	}

	DoAttachObjects();

	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CStatView::on_destroy()
{
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CStatView::on_paint()
{
	PAINTSTRUCT	ps = {0};

	_rect rc( 0, 0, 0, 0 );
	::GetUpdateRect( m_hwnd, &rc, false );

	HDC hdc = ::BeginPaint( m_hwnd, &ps );	

	RECT rcClient;
	GetClientRect(m_hwnd,&rcClient);

	if(0>=m_grid.m_list_col.size()){
		HBRUSH hbrush = ::CreateSolidBrush( ::GetSysColor( COLOR_WINDOW ) );
		::FillRect( hdc, &rcClient, hbrush );
		CString sNoData((LPCTSTR)IDS_NO_DATA);
		::DrawText( hdc, sNoData, sNoData.GetLength(), &rcClient, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
		::DeleteObject( hbrush );
	}
	
	::EndPaint( m_hwnd, &ps );

	return 1L;
}

//////////////////////////////////////////////////////////////////////
//long CStatView::on_erase_background( HDC hDC )
//{
//	return 1L;
//}

//////////////////////////////////////////////////////////////////////
long CStatView::on_size( int cx, int cy )
{

	if( m_grid.handle() )
		::MoveWindow( m_grid.handle(), 0, 0, cx, cy, TRUE );
	::InvalidateRect(m_hwnd,NULL,TRUE);
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CStatView::on_context_menu( HWND hWnd, int x, int y )
{
	POINT pt;
	pt.x = x;
	pt.y = y;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return 0;

	ICommandManager2Ptr ptrCM = punk;
	punk->Release();	punk = 0;

	if( ptrCM == NULL )
		return 0;
	
	CMenuRegistrator rcm;	

	_bstr_t	bstrMenuName = rcm.GetMenu( szStatTableViewNameMenu, 0 );
	ptrCM->ExecuteContextMenu2( bstrMenuName, pt, 0 );	

	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CStatView::on_setfocus( HWND hwnd_lost )
{
	if( m_grid.handle() )
		::SetFocus( m_grid.handle() );

	return 0L;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	_bstr_t	bstr( bstrObjectType );
	if( !strcmp( bstr, szTypeStatTable ) )
		*pdwMatch = mvFull;
	else
		*pdwMatch = mvNone;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CStatView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CWinViewBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
	
	// SBT:1207 - 07.12.2004
	if( !strcmp( pszEvent, szEventChangeObject ) ||
		!strcmp( pszEvent, szEventObjectOptionsChange ) ||
		!strcmp( pszEvent, szEventChangeObjectList ) )
	{
		// [vanek] SBT:1247 - 17.12.2004
		if( !punkFrom || ::GetKey( m_grid.m_ptr_table ) == ::GetKey( punkFrom ) )
		{
			m_grid.save_state( );
			m_grid.fill_grid(false);
		}
	}
}

//Print support
//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{
	return m_grid.GetPrintWidth( nMaxWidth, pnReturnWidth, pbContinue, nUserPosX, pnNewUserPosX );
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{
	return m_grid.GetPrintHeight( nMaxHeight, pnReturnHeight, pbContinue, nUserPosY, pnNewUserPosY );
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags )
{
	return m_grid.Print( hdc, rectTarget, nUserPosX, nUserPosY, nUserPosDX, nUserPosDY, bUseScrollZoom, dwFlags );
}

//Persist Impl
//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetClassID(CLSID *pClassID )
{
	if( !pClassID )	return E_POINTER;

	memcpy( pClassID, &clsidStatTableView, sizeof(CLSID) );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetFirstVisibleObjectPosition( long *plpos )
{
	if( !plpos )	return E_POINTER;

	*plpos = 0;
	if( m_grid.m_ptr_table != 0 )
		*plpos = 1;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos )
{
	if( !plPos )	return E_POINTER;

	long lPos		= *plPos;
	*ppunkObject	= 0;
	*plPos			= 0;
	if( lPos == 1 )
	{
		if( m_grid.m_ptr_table )
		{
			m_grid.m_ptr_table->AddRef();
			*ppunkObject = m_grid.m_ptr_table;
			return S_OK;
		}
	}

	return S_OK;
}

//IStatTableGrid
//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetColumnsCount( long* plcount )
{
	if( !plcount )	return E_POINTER;

	*plcount = m_grid.m_list_col.size();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::GetActiveColumn( long* plcolumn )
{
	if( !plcolumn )	return E_POINTER;
	*plcolumn = 1;

	int nCol = -1;
	m_grid.GetFocused( 0, &nCol );
	
	*plcolumn = nCol;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::HideColumn( long lcolumn )
{
	stat_col_info* pci = m_grid.find_col_info_by_num( lcolumn );
	if( !pci )	return S_FALSE;
	pci->m_bvisible = false;	

	m_grid.save_state( EXCHANGE_ORDER|EXCHANGE_WIDTH );	

	NotifyObjectChange( m_grid.m_ptr_table, szEventObjectOptionsChange  );

	INamedDataObjectPtr ptr_ndo( m_grid.m_ptr_table );
	if( ptr_ndo )	ptr_ndo->SetModifiedFlag( TRUE );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::ShowAllColumns( )
{
	for(StatColVector::iterator lpos=m_grid.m_list_col.begin();
		lpos!=m_grid.m_list_col.end(); ++lpos)
	{
		stat_col_info& pci = *lpos;
		pci.m_bvisible = true;
	}
	
	m_grid.save_state( EXCHANGE_ALL );	
	
	NotifyObjectChange( m_grid.m_ptr_table, szEventObjectOptionsChange  );

	INamedDataObjectPtr ptr_ndo( m_grid.m_ptr_table );
	if( ptr_ndo )	ptr_ndo->SetModifiedFlag( TRUE );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatView::CustomizeColumns( )
{	
	//get order
	m_grid.save_state( EXCHANGE_ALL );	
	CCustomizeDlg dlg;

	size_t size = m_grid.m_list_col.size();
	if( !size )	return S_FALSE;

	col_info* parr = new col_info[size];
	if( !parr )	return false;

	::ZeroMemory( parr, size * sizeof(col_info) );

	dlg.set_col_info( parr, size );

	//fill col info array
	int idx = 0;
	for(StatColVector::const_iterator lpos=m_grid.m_list_col.begin();
		lpos!=m_grid.m_list_col.end(); ++lpos)
	{
		const stat_col_info& pci = *lpos;
//		stat_col_info* pci = m_grid.m_list_col.get( lpos );

		parr[idx].norder = pci.m_order;
		//get caption
		parr[idx].sz_caption[0] = 0;
		{			
			if( pci.m_lkey == KEY_CLASS )
				::LoadString( app::handle(), IDS_CLASS_CAPTION, parr[idx].sz_caption, sizeof(parr[idx].sz_caption) );
			else
			{
				_bstr_t bstr_name = "";
				map_meas_params::const_iterator it=m_grid.m_map_meas_params.find( pci.m_lkey );
				if( it!=m_grid.m_map_meas_params.end() )
				{
					ParameterDescriptor_ex* pmp = it->second;
					if( pmp->bstrName )
						bstr_name = pmp->bstrName;
					sprintf( parr[idx].sz_caption, "%s", (char*)bstr_name );
				}
			}
		}
		parr[idx].bvisible	= pci.m_bvisible;
		parr[idx].lparam	= (LPARAM)&pci;
		idx++;
	}

	if( dlg.do_modal( ::GetActiveWindow() ) == IDOK )
	{
		for( idx=0; idx<(int)size; idx++ )
		{
			stat_col_info* pci = (stat_col_info*)parr[idx].lparam;
			pci->m_order	= parr[idx].norder;
			pci->m_bvisible	= parr[idx].bvisible;

		}
		m_grid.save_state( EXCHANGE_ALL );	
		NotifyObjectChange( m_grid.m_ptr_table, szEventObjectOptionsChange );

		INamedDataObjectPtr ptr_ndo( m_grid.m_ptr_table );
		if( ptr_ndo )	ptr_ndo->SetModifiedFlag( TRUE );
	}

	delete [] parr;

	return S_OK;
}
