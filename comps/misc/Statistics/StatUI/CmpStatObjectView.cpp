#include "StdAfx.h"
#include "resource.h"
#include "statui_consts.h"
#include "statui_int.h"

#include "menuconst.h"
#include "misc_utils.h"
#include "MenuRegistrator.h"
#include "\vt5\awin\win_base.h"
#include "axint.h"
#include <object5.h>
#include "statistics.h"
#include "StatUI.h"
#include "CmpStatObject.h"
#include "cmpstatobjectview.h"

namespace ViewSpace 
{

CCmpStatObjectView::CCmpStatObjectView()
	: m_bReadyToShow(false), m_pCmpStatObject(0)
{	
	::ZeroMemory( (LPVOID)&m_rcPrev, sizeof( RECT ) );
	m_bError = false;
	m_strErrorDescr.LoadString( IDS_NO_DATA );

	m_bstrName		= "CmpStatObjectView";
	m_dwWindowFlags |= wfSupportFitDoc2Scr;
	char sz_buf[256];
	::LoadString( App::handle(), IDS_CMP_VIEW_NAME, sz_buf, sizeof( sz_buf ) );
	m_bstrUserName	= sz_buf;
	if( !m_bstrUserName.length() )
		m_bstrUserName = m_bstrName;

	m_rcHorzSplitter = m_rcVertSplitter = NORECT;
	m_rcChartView =  NORECT; m_rcLegendView =  NORECT; m_rcTableView =  NORECT;

	m_dwViews = vtChartView | vtLegendView | vtTableView;

	m_fVertSplitter = 0.5;
	m_fHorzSplitter = 0.5;

	m_nSplitterHalfWidth = 0;
	
	m_bShowen = false;
	m_bLockRecalc = false;

	m_bPrintMode = false;

	m_dx_error = m_dy_error = 0;

	default_init();
}

void CCmpStatObjectView::default_init()
{
	m_nSplitterType = 0;

	m_nPane0_Data = 0;
	m_nPane1_Data = 1;
	m_nPane2_Data = 2;
	m_bShowContext = true;
}

void CCmpStatObjectView::set_modify()
{
	//for( LPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
	//{
	//	IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
	//	if( ptr_object )
	//		SetModifiedFlagToObj( ptr_object );
	//}
}

//////////////////////////////////////////////////////////////////////
CCmpStatObjectView::~CCmpStatObjectView()
{
	//for( LPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
	//{
	//	IUnknownPtr ptr_object = m_list_attached.get( lpos_lst );
	//	if( ptr_object )
	//			store_to_ndata( ptr_object );
	//}

	//_clear_attached();
}

//////////////////////////////////////////////////////////////////////
IUnknown* CCmpStatObjectView::DoGetInterface( const IID &iid )
{
	if( iid == IID_IPrintView ) return (IPrintView*)this;
	else if( iid == IID_IPersist ) return (IPersist*)this;
	else if( iid == IID_INamedObject2 ) return (INamedObject2*)this;	
	else if( iid == IID_IStatObjectView ) return (IStatObjectView*)this;	
	else if( iid == IID_INamedPropBag ) return (INamedPropBag*)this;	
	else if( iid == IID_INamedPropBagSer ) return (INamedPropBag*)this;	
	else if( iid == IID_IHelpInfo ) return (CHelpInfoImpl*)this;	
	else 
		return CWinViewBase::DoGetInterface( iid );
}
	
//////////////////////////////////////////////////////////////////////
void CCmpStatObjectView::DoAttachObjects()
{
	IDataContext3Ptr ptr_context = Unknown();

	if( ptr_context == 0 )
		return;


	//for( LPOS lpos_lst = m_list_attached.head(); lpos_lst; lpos_lst = m_list_attached.next( lpos_lst ) )
	//{
	//	IUnknown *punk_lst = m_list_attached.get( lpos_lst );

	//	store_to_ndata( punk_lst );
	//	break;
	//}

	_clear_attached();


	{
		LONG_PTR lpos_selected = 0;
		_bstr_t bstr_type( szTypeCmpStatObject );

		ptr_context->GetFirstSelectedPos( bstr_type, &lpos_selected );
		if(lpos_selected)
		{
			IUnknownPtr punk_object;
			ptr_context->GetNextSelected( bstr_type, &lpos_selected, &punk_object );
			if(punk_object)
			{
				_add_attach_data(punk_object);
			}
		}
	}


	if(m_pCmpStatObject)
	{
		const ObjectSpace::CCmpStatObject::StatObjects& statObjects=m_pCmpStatObject->_statObjects;
		if(INamedDataPtr ptr_object = m_pCmpStatObject->Unknown())
		{
			load_from_ndata( ptr_object );
			if( IVtActiveXCtrl2Ptr sptr =  Unknown() )
			{
				if(INamedPropBagPtr pControlPropBag=sptr)
				{
					CComVariant ViewSubType;
					if(SUCCEEDED(pControlPropBag->GetProperty(CComBSTR("Views"), &ViewSubType))){
						HRESULT hr=ViewSubType.ChangeType(VT_I4);
						if(SUCCEEDED(hr)){
							if(ViewSubType.lVal>0){
								if(INamedPropBagPtr pNamedPropBag=Unknown()){
									m_dwViews=ViewSubType.lVal;
								}
							}
						}
					}
				}
			}
		}
	}

	m_bLockRecalc = false;
	attach_data();
}

void CCmpStatObjectView::attach_data()
{
	{
		if( m_dwViews & vtChartView )
			m_wndChart.attach_data();
		if( m_dwViews & vtLegendView )
			m_wndLegend.attach_data();
		if( m_dwViews & vtTableView )
			m_wndTable.attach_data();
	}

	if(0 == m_pCmpStatObject || 0 == m_pCmpStatObject->_statObjects.size() || m_nSplitterType < 0 || m_nSplitterType > 3 )
	{ 
		::ShowWindow( m_wndChart.handle(), SW_HIDE );
		::ShowWindow( m_wndLegend.handle(), SW_HIDE );
		::ShowWindow( m_wndTable.handle(), SW_HIDE );

		::ShowWindow( m_wndHorzSplitter, SW_HIDE );
		::ShowWindow( m_wndVertSplitter, SW_HIDE );

		IScrollZoomSitePtr sptrZ = Unknown();
		if( sptrZ )
		{
			SIZE sz = { 10, 10 };
			sptrZ->SetClientSize( sz );
		}
	}else{
		{
			CRect rcClient;
			::GetClientRect(hwnd(),&rcClient);
			SIZE sizeClient={rcClient.Width(),rcClient.Height()};
			SIZE size =_recalc_layout(sizeClient);
			m_hwSplitRoot=SetSplitterStruct();
			if(size.cx>0 && size.cy>0 && m_hwSplitRoot){
				if(IsWindow()){
					SetScrollSize(size.cx, size.cy, 0, TRUE);
					::SetWindowPos(m_hwSplitRoot,NULL, 0, 0,
						size.cx, size.cy, SWP_NOZORDER|SWP_NOACTIVATE);
				}
				IScrollZoomSitePtr sptrZ = this;
				sptrZ->SetClientSize( size );
			}
		}
		//m_wndVertSplitter.SetSplitterPanes(m_wndChart.handle(),m_wndLegend.handle());
		//m_wndHorzSplitter.SetSplitterPanes(m_wndVertSplitter,m_wndTable.handle());

		//m_wndVertSplitter.m_cxyMin =__min(
		//	m_wndChart.get_min_rect().Width(),
		//	m_wndLegend.get_min_rect().Width());
		//m_wndVertSplitter.SetSplitterPos();

		//m_wndHorzSplitter.m_cxyMin=__min(
		//	m_wndChart.get_min_rect().Width(),
		//	m_wndLegend.get_min_rect().Width());
		//m_wndHorzSplitter.SetSplitterPos();
	}
}

//////////////////////////////////////////////////////////////////////
LRESULT	CCmpStatObjectView::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
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
		case WM_ERASEBKGND:		lResult = on_erase_background( (HDC)wParam );
			break;
		case WM_SIZE:			lResult = on_size( (int)LOWORD(lParam), (int)HIWORD(lParam) );
			break;
		case WM_CONTEXTMENU:	lResult = on_context_menu( HWND(wParam), (int)GET_X_LPARAM(lParam), (int)GET_Y_LPARAM(lParam) );
			break;
		case WM_NOTIFY:		
			{
				MSG msg = {0};
				msg.message	= nMsg;
				msg.hwnd	= m_hwnd;
				msg.wParam	= wParam;
				msg.lParam	= lParam;
				::GetCursorPos( &msg.pt );				

				HWND hwnd = ::GetDlgItem( m_hwnd, wParam );
				long	lProcessed = false;
				long	lret_reflect = SendMessage( hwnd, WM_NOTYFYREFLECT, (WPARAM)&lProcessed, (LPARAM)&msg );
				if( lProcessed ) return lret_reflect;
				return 0;   
			}
			break;			
		case WM_VSCROLL:
		case WM_HSCROLL:
			lResult = 0;
			break;
		case WM_SHOWWINDOW: lResult = 0;
			if( wParam )
				m_bShowen = true;
			break;
		case (WM_USER + 0x1501):
			{
				int iR = 0, iC = 0;
				int iItemHot=m_wndTable.m_Grid.SetItemState((int)wParam
					,LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
				m_wndTable.m_Grid.SetFocus();
			}
			break;
		case (WM_USER + 0x5546):
			{
//				m_wndTable.set_active_simple( (int)wParam );
				m_wndTable.attach_data();
			}
			break;
	}
	if( lResult )
		return lResult;

	return CWinViewBase::DoMessage( nMsg, wParam, lParam );
}

//////////////////////////////////////////////////////////////////////
long CCmpStatObjectView::on_create( CREATESTRUCT *pcs )
{
/*
	IWindowPtr ptrWnd = Unknown();
	DWORD dwFlags = 0;
	ptrWnd->GetWindowFlags( &dwFlags );
	ptrWnd->SetWindowFlags( dwFlags | wfZoomSupport );
*/

 	DWORD dwStyle = ::GetWindowLong( m_hwnd, GWL_STYLE );
	dwStyle |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
	::SetWindowLong( m_hwnd, GWL_STYLE, dwStyle );


	IScrollZoomSitePtr sptrZ = this;
	sptrZ->SetAutoScrollMode(TRUE);
	HWND hScrollBar=0; sptrZ->GetScrollBarCtrl(SB_HORZ, (HANDLE*)&hScrollBar);
	if(::IsWindow(hScrollBar)){
		HWND hWndParent=::GetParent(m_hwnd);
		SubclassWindow(hWndParent);
		BOOL b1=::ShowScrollBar(hWndParent,SB_BOTH,FALSE);
		dwStyle = ::GetWindowLong(hWndParent, GWL_STYLE );
		dwStyle |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		dwStyle &= ~(WS_HSCROLL|WS_VSCROLL);
		BOOL b2=::SetWindowLong( hWndParent, GWL_STYLE, dwStyle );
		dwStyle = ::GetWindowLong( hWndParent, GWL_STYLE );

		DWORD dwPrevStyle=SetScrollExtendedStyle(SCRL_NOTHUMBTRACKING,SCRL_NOTHUMBTRACKING);
	}

	INotifyControllerPtr ptr_nc = GetAppUnknown();
	if( ptr_nc )
		ptr_nc->RegisterEventListener( 0, Unknown() );

	ptr_nc = Unknown();
	if( ptr_nc )
		ptr_nc->RegisterEventListener( 0, Unknown() );



	{ RECT rc = { 0,   0  , 100, 200 };
	  m_rcChartView  = rc; }

	{ RECT rc = { 100, 0  , 200, 100 };
	  m_rcLegendView = rc; }

	{ RECT rc = { 100, 100, 200, 200 };
	  m_rcTableView  = rc; }

	char szClassName[] = "ChartViewClassName";


	{
		WNDCLASS	wndclass = {0};
		wndclass.lpszClassName = szClassName;
		wndclass.hInstance = module::hinst();
		wndclass.lpfnWndProc = subclass_proc;
		wndclass.hbrBackground = (HBRUSH)::GetSysColorBrush(COLOR_BTNFACE);
		wndclass.hCursor = ::LoadCursor(0, MAKEINTRESOURCE(IDC_ARROW));

		wndclass.style = CS_DBLCLKS;
		
		::RegisterClass( &wndclass );		
	}

	HWND hWnd=m_wndHorzSplitter.Create( hwnd(), &CWindow::rcDefault/*, 0,
		m_wndHorzSplitter.GetWndStyle(0)|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL*/);
	hWnd=m_wndVertSplitter.Create( hwnd(), &CWindow::rcDefault );

	bool b=m_wndChart.create( WS_CHILD, m_rcChartView, 0, hwnd(), 0, szClassName );
	b=m_wndLegend.create( WS_CHILD, m_rcLegendView, 0, hwnd() );
	b=m_wndTable.create( WS_CHILD, m_rcTableView, 0, hwnd() );

	DWORD dw=SetClassLong(m_wndTable.handle(),GCLP_HBRBACKGROUND,
		(LONG)::GetSysColorBrush(COLOR_BTNFACE));

	m_wndChart.set_view_unkn( Unknown() );
	m_wndLegend.set_view_unkn( Unknown() );
	m_wndTable.set_view_unkn( Unknown() );

	DoAttachObjects();
	if( IVtActiveXCtrl2Ptr sptr =  Unknown() )
	{
		m_bPrintMode = true;

		m_wndChart.SetPrintMode();
		m_wndLegend.SetPrintMode();
		m_wndTable.SetPrintMode();
	}
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CCmpStatObjectView::on_destroy()
{
	if(IsWindow())
		UnsubclassWindow();

	_clear_attached();


	INotifyControllerPtr ptr_nc = GetAppUnknown();
	if( ptr_nc )
		ptr_nc->UnRegisterEventListener( 0, Unknown() );
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CCmpStatObjectView::on_paint()
{
	RECT rcPaint = { 0, 0, 0, 0 };
	::GetUpdateRect( m_hwnd, &rcPaint, false );

	RECT rcPaint2 = { 0, 0, 0, 0 };
 	::GetClientRect( m_hwnd, &rcPaint2 );

	PAINTSTRUCT	ps = {0};
	HDC hdcPaint = ::BeginPaint( m_hwnd, &ps );	

	HDC	hdcMemory = ::CreateCompatibleDC( hdcPaint );

	int nImageWidth		= rcPaint.right  - rcPaint.left  /*+ 1*/;
	int nImageHeight	= rcPaint.bottom - rcPaint.top  /*+ 1*/;

	BITMAPINFOHEADER	bmih = {0};

	bmih.biBitCount = 24;	
	bmih.biWidth	= nImageWidth;
	bmih.biHeight	= nImageHeight;							   
	bmih.biSize		= sizeof( bmih );
	bmih.biPlanes	= 1;							   

	byte	*pdibBits = 0;
 	HBITMAP	hDIBSection = ::CreateDIBSection( hdcMemory, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject( hdcMemory, hDIBSection );

	::SetViewportOrgEx( hdcMemory, -rcPaint.left, -rcPaint.top, 0 );
	::SetBrushOrgEx( hdcMemory, -rcPaint.left % 8, -rcPaint.top % 8, 0 );

	HBRUSH hbrush = ::CreateSolidBrush( ::GetSysColor( COLOR_BTNFACE ) );

	::FillRect( hdcMemory, &rcPaint2, hbrush );
	
	if( m_bError || 0==m_pCmpStatObject)
	{
		::SetBkColor( hdcMemory, ::GetSysColor( COLOR_BTNFACE ) );
		::SetBkMode(hdcMemory,TRANSPARENT);
		::DrawText( hdcMemory, m_strErrorDescr, m_strErrorDescr.GetLength(), &rcPaint2, DT_CENTER | DT_VCENTER | DT_SINGLELINE );
	}
	::DeleteObject( hbrush );	hbrush = 0;

	::SetViewportOrgEx( hdcMemory, 0, 0, 0 );

	::SetDIBitsToDevice( hdcPaint, rcPaint.left, rcPaint.top, nImageWidth, nImageHeight,
 		0, 0, 0, nImageHeight, pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	::SelectObject( hdcMemory, hOldBitmap );
	::DeleteObject( hDIBSection );

	::DeleteObject( hdcMemory );

	::EndPaint( m_hwnd, &ps );
	return 1L;

}


//////////////////////////////////////////////////////////////////////
long CCmpStatObjectView::on_erase_background( HDC hDC )
{
	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CCmpStatObjectView::on_size( int cx, int cy )
{

	if( m_rcPrev.right == cx && m_rcPrev.bottom == cy )
		return 0L;

	if( m_bShowen )
	{
		RECT rc = {0};
		::GetClientRect( m_hwnd, &rc );
		m_rcPrev = rc;

		if( rc.right == rc.left && rc.bottom == rc.top )
			return 0;
	}  

	return 1L;
}

//////////////////////////////////////////////////////////////////////
long CCmpStatObjectView::on_context_menu( HWND hWnd, int x, int y )
{
	if( !m_bShowContext )
		return 0;

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

	_bstr_t	bstrMenuName = rcm.GetMenu( szStatObjectViewNameMenu, 0 );
	ptrCM->ExecuteContextMenu2( bstrMenuName, pt, 0 );	

	return 1L;
}

//////////////////////////////////////////////////////////////////////
HRESULT CCmpStatObjectView::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	_bstr_t	bstr( bstrObjectType );
	if( !strcmp( bstr, szTypeCmpStatObject ) )
		*pdwMatch = mvFull;
	else
		*pdwMatch = mvNone;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
#include <mmsystem.h>
void CCmpStatObjectView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CWinViewBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
	if( !strcmp( pszEvent, szEventNewSettings ) || !strcmp( pszEvent, szEventNewSettingsView ) ||
		!strcmp( pszEvent, szEventChangeObject ) /*&& m_bPrintMode*/ ||  !strcmp( pszEvent, szEventActiveContextChange ) )
	{

		if( !strcmp( pszEvent, szEventChangeObject ) )
		{
			if(m_pCmpStatObject)
			if(m_pCmpStatObject->_statObjects.size())
			{
				IUnknownPtr ptr_object = m_pCmpStatObject->Unknown();
				if( ::GetKey( ptr_object ) != ::GetKey( punkFrom ) )
					return;
			}
		}

		if(m_pCmpStatObject && m_pCmpStatObject->_statObjects.size())
		{
			if(!m_bPrintMode)
			{
				_clear_attached();
				DoAttachObjects();
			}
		}
	}
	

	if( !strcmp( pszEvent, szEventScrollPosition ) )
		if( ::GetKey( punkFrom ) == ::GetKey( Unknown() ) ){
			/*_recalc_layout()*/;
		}
}

//Print support
HRESULT CCmpStatObjectView::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{
	if( !(m_dwViews & vtTableView) )
	{
		IScrollZoomSitePtr ptr_zoom = Unknown();
		double fZoom=1.;
		ptr_zoom->GetZoom( &fZoom );
		CRect rc=_calc_min_view_size();
		{
			_ASSERTE(0==nUserPosX);
			*pnNewUserPosX = *pnReturnWidth;
			*pbContinue = false;
		}
	}
	else
	{
		*pbContinue=m_wndTable.GetPrintWidth(nMaxWidth, *pnReturnWidth, nUserPosX, *pnNewUserPosX);
		}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CCmpStatObjectView::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{
	if( !(m_dwViews & vtTableView) )
	{
		IScrollZoomSitePtr ptr_zoom = Unknown();
		double fZoom=1.;
		ptr_zoom->GetZoom( &fZoom );
	
		CRect rc=_calc_min_view_size();
//		int nHeight = (int)round(rc.Size().cy * fZoom);

		
		//if(nHeight > nUserPosY + nMaxHeight)
		//{
		//	*pnNewUserPosY +=  *pnReturnHeight;
		//	*pbContinue = true;
		//}
		//else
		{
			*pnNewUserPosY =  *pnReturnHeight;
			*pbContinue = false;
			}
		}
		else
		{
		*pbContinue=m_wndTable.GetPrintHeight(nMaxHeight, *pnReturnHeight, nUserPosY, *pnNewUserPosY);
		}
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CCmpStatObjectView::Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags )
{
	if( m_bError )
	{
		RECT rcPT = rectTarget;
		::SetBkMode( hdc, TRANSPARENT);
		::SetTextColor( hdc, ::GetSysColor( COLOR_BTNTEXT ) );
		::DrawText( hdc, m_strErrorDescr, strlen( m_strErrorDescr ), &rcPT, DT_CENTER | DT_VCENTER |DT_SINGLELINE );
	}
	else
	{
		int	nLastPosX = nUserPosX + nUserPosDX;
		int	nLastPosY = nUserPosY + nUserPosDY;
		CRect	rectDraw(nUserPosX, nUserPosY, nLastPosX, nLastPosY);

		if( m_dwViews & vtTableView )
		{
			m_wndTable.Print(CDCHandle(hdc), rectTarget, rectDraw);
		}
		else if( m_dwViews & vtChartView )
			{
			m_wndChart.Print(CDCHandle(hdc), rectTarget, rectDraw);
			}
		else if( m_dwViews & vtLegendView)
			{
			m_wndLegend.Print(CDCHandle(hdc), rectTarget, rectDraw);
		}
	}
	return S_OK;
}

//Persist Impl
//////////////////////////////////////////////////////////////////////
HRESULT CCmpStatObjectView::GetClassID(CLSID *pClassID )
{
	if( !pClassID )	return E_POINTER;

	memcpy( pClassID, &clsidStatObjectView, sizeof(CLSID) );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CCmpStatObjectView::GetFirstVisibleObjectPosition(TPOS *plpos)
{
	if( !plpos )
		return S_FALSE;

	if(0==m_pCmpStatObject || 0==m_pCmpStatObject->_statObjects.size())
		return S_FALSE;

	*plpos = (TPOS)1;
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CCmpStatObjectView::GetNextVisibleObject(IUnknown ** ppunkObject, TPOS *plPos)
{
	if( !plPos )
		return S_FALSE;
	
	if( !ppunkObject )
		return S_FALSE;

	if(0==m_pCmpStatObject->_statObjects.size())
		return S_FALSE;

	*ppunkObject = m_pCmpStatObject->Unknown();
	(*ppunkObject)->AddRef();
	
	*plPos = 0;

	return S_OK;
}

HRESULT CCmpStatObjectView::ShowView( DWORD dwView )
{
	//if( m_dwViews == dwView )
	//	return S_FALSE;

	m_dwViews = dwView;

	IScrollZoomSitePtr sptrZoom = Unknown();
	POINT pt = {0};

	if( sptrZoom != 0 )
		sptrZoom->SetScrollPos( pt );

	if( m_pCmpStatObject && !m_bPrintMode )
	{
		IUnknownPtr ptr_object = m_pCmpStatObject->Unknown();
		if( ptr_object )
			::SetValue( ptr_object, SECT_STATUI_ROOT, "Views", (long)m_dwViews );
	}

	return S_OK;
}
HRESULT CCmpStatObjectView::GetViewVisibility( DWORD *pdwView )
{
	if( !pdwView )
		return S_FALSE;

	*pdwView = m_dwViews;

	return S_OK;

}

const SIZE CCmpStatObjectView::_recalc_layout(SIZE sizeRect)
{

	SIZE sizeNew = {0};

	if( /*!m_bShowen ||*/ m_bLockRecalc || m_pCmpStatObject == 0 )
		return sizeNew;

	if(0==m_pCmpStatObject->_statObjects.size())
	{
		if( m_bError )
			::InvalidateRect( m_hwnd, 0, 0 );
		return sizeNew;
	}

	m_bLockRecalc = true;

	RECT rcClient = {0};
	RECT rcPane0, rcPane1, rcPane2;

	RECT rcPane0_min, rcPane1_min, rcPane2_min;

	m_rcChartView = rcClient; m_rcLegendView = rcClient; m_rcTableView = rcClient;
	rcPane0 = rcPane1 = rcPane2 = rcClient;

	if( !m_bPrintMode ){
		::GetClientRect( m_hwnd, &rcClient );
		rcClient.left=0;
		rcClient.top=0;
		rcClient.right=sizeRect.cx;
		rcClient.bottom=sizeRect.cy;

		if( rcClient.bottom == rcClient.top && rcClient.right == rcClient.left )
	{
		m_bLockRecalc = false;
			return sizeNew;
		}
	}

	IScrollZoomSitePtr sptrZoom = Unknown();
	double _fZoom = 1;
	if( m_bPrintMode )
		sptrZoom->GetZoom( &_fZoom );
	else
		sptrZoom->SetZoom( 1 );

	RECT rcFull = {0};
	{

 		RECT rcMinViewSize = _calc_min_view_size();

		if( m_bPrintMode )
			rcMinViewSize = ScaleRect( rcMinViewSize, _fZoom );
		
		RECT rcAllSize = { 0, 0, __max( rcClient.right, rcMinViewSize.right ),  __max( rcClient.bottom, rcMinViewSize.bottom ) };
		rcClient = rcAllSize;
	}


	rcFull = rcClient;

	DWORD vtPane0 = 0, vtPane1 = 0, vtPane2 = 0;

	if( m_nPane0_Data == 0 )
		vtPane0 = vtChartView;
	else if( m_nPane0_Data == 1 )
		vtPane0 = vtLegendView;
	else if( m_nPane0_Data == 2 )
		vtPane0 = vtTableView;

	if( m_nPane1_Data == 0 )
		vtPane1 = vtChartView;
	else if( m_nPane1_Data == 1 )
		vtPane1 = vtLegendView;
	else if( m_nPane1_Data == 2 )
		vtPane1 = vtTableView;

	if( m_nPane2_Data == 0 )
		vtPane2 = vtChartView;
	else if( m_nPane2_Data == 1 )
		vtPane2 = vtLegendView;
	else if( m_nPane2_Data == 2 )
		vtPane2 = vtTableView;

	RECT rcMinChart  = ( m_dwViews & vtChartView ) ? m_wndChart.get_min_rect() : NORECT;
	RECT rcMinLegend = ( m_dwViews & vtLegendView ) ? m_wndLegend.get_min_rect() : NORECT;
	RECT rcMinTable  = ( m_dwViews & vtTableView ) ? m_wndTable.get_min_rect() : NORECT;

	if( m_bPrintMode )
	{
		rcMinChart  = ScaleRect( rcMinChart, _fZoom );
		rcMinLegend = ScaleRect( rcMinLegend, _fZoom );
		rcMinTable  = ScaleRect( rcMinTable, _fZoom );
	}
	if( !(m_dwViews & vtChartView ) )
		::ZeroMemory( (void *)&rcMinChart, sizeof( RECT ) );
	if( !(m_dwViews & vtLegendView ) )
		::ZeroMemory( (void *)&rcMinLegend, sizeof( RECT ) );
	if( !(m_dwViews & vtTableView ) )
		::ZeroMemory( (void *)&rcMinTable, sizeof( RECT ) );

	if( m_nPane0_Data == 0 )
		rcPane0_min  = rcMinChart;
	else if( m_nPane0_Data == 1 )
		rcPane0_min = rcMinLegend;
	else if( m_nPane0_Data == 2 )
		rcPane0_min  = rcMinTable;

	if( m_nPane1_Data == 0 )
		rcPane1_min  = rcMinChart;
	else if( m_nPane1_Data == 1 )
		rcPane1_min = rcMinLegend;
	else if( m_nPane1_Data == 2 )
		rcPane1_min  = rcMinTable;

	if( m_nPane2_Data == 0 )
		rcPane2_min  = rcMinChart;
	else if( m_nPane2_Data == 1 )
		rcPane2_min = rcMinLegend;
	else if( m_nPane2_Data == 2 )
		rcPane2_min  = rcMinTable;

	long w0_m = rcPane0_min.right - rcPane0_min.left;
	long h0_m = rcPane0_min.bottom - rcPane0_min.top;

	long w1_m = rcPane1_min.right - rcPane1_min.left;
	long h1_m = rcPane1_min.bottom - rcPane1_min.top;

	long w2_m = rcPane2_min.right - rcPane2_min.left;
	long h2_m = rcPane2_min.bottom - rcPane2_min.top;

 	if( m_nSplitterType == 0 || m_nSplitterType == 1 )   //  |-     -|
	{
		if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
		{
			rcPane0 = rcClient;

			long w0 = rcPane0.right - rcPane0.left;

			if( m_nSplitterType == 0 )
			{
				rcPane0.right = (long)( rcPane0.left + __max( (w0 - 2 * m_nSplitterHalfWidth ) * m_fVertSplitter, w0_m ) );
				rcPane0.right = (long)( rcPane0.left + __min( rcPane0.right - rcPane0.left, (w0 - 2 * m_nSplitterHalfWidth ) - __max( w1_m, w2_m ) ) );

				rcClient.left = rcPane0.right + 2 * m_nSplitterHalfWidth;
			}
			else
			{
				rcPane0.left = (long)( rcPane0.right - __max( (w0 - 2 * m_nSplitterHalfWidth ) * m_fVertSplitter, w0_m ) );
				rcPane0.left = (long)( rcPane0.right - __min( rcPane0.right - rcPane0.left, (w0 - 2 * m_nSplitterHalfWidth ) - __max( w1_m, w2_m ) ) );

				rcClient.right = rcPane0.left - 2 * m_nSplitterHalfWidth;
			}
		}
		else if( m_dwViews == vtPane0 )
		{
			if( rcClient.right - rcClient.left < w0_m )
				rcClient.right = rcPane0_min.right;

			if( rcClient.bottom - rcClient.top < h0_m )
				rcClient.bottom = rcPane0_min.bottom;

			rcPane0 = rcClient;
		}

		if( m_dwViews & vtPane1 && m_dwViews & vtPane2 )
		{
			rcPane1 = rcClient;
			
			long h1 = rcPane1.bottom - rcPane1.top;

			rcPane1.bottom = (long)( rcPane1.top + __max( ( h1 - 2 * m_nSplitterHalfWidth ) * m_fHorzSplitter, h1_m ) );
			rcPane1.bottom = (long)( rcPane1.top + __min( rcPane1.bottom - rcPane1.top, ( h1 - 2 * m_nSplitterHalfWidth ) - h2_m ) );


			rcClient.top = rcPane1.bottom + 2 * m_nSplitterHalfWidth;
			
			rcPane2 = rcClient;
		}
		else if( m_dwViews & vtPane1 )
		{
			if( m_dwViews == vtPane1 )
			{
				if( rcClient.right - rcClient.left < w1_m )
					rcClient.right = rcPane1_min.right;

				if( rcClient.bottom - rcClient.top < h1_m )
					rcClient.bottom = rcPane1_min.bottom;
			}

			rcPane1 = rcClient;
		}
		else if( m_dwViews & vtPane2 )
		{
			if( m_dwViews == vtPane2 )
			{
				if( rcClient.right - rcClient.left < w2_m )
					rcClient.right = rcPane2_min.right;

				if( rcClient.bottom - rcClient.top < h2_m )
					rcClient.bottom = rcPane2_min.bottom;
			}

			rcPane2 = rcClient;
		}

		if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
		{
			if( m_nSplitterType == 0 )
			{
				RECT rcSplitter = { rcPane0.right, rcPane0.top, rcPane0.right + 2 * m_nSplitterHalfWidth, rcPane0.bottom };
				m_rcVertSplitter = rcSplitter;
				m_fVertSplitter = ( rcPane0.right - rcPane0.left ) / double( rcFull.right - rcFull.left - 2 * m_nSplitterHalfWidth );
			}
			else
			{
				RECT rcSplitter = { rcPane0.left - 2 * m_nSplitterHalfWidth, rcPane0.top, rcPane0.left, rcPane0.bottom };
				m_rcVertSplitter = rcSplitter;
				
				m_fVertSplitter = ( rcPane0.right - rcPane0.left ) / double( rcFull.right - rcFull.left - 2 * m_nSplitterHalfWidth );
			}
		}

		if( m_dwViews & vtPane1 || m_dwViews & vtPane2 )
		{
			RECT rcSplitter = { rcPane1.left, rcPane1.bottom , rcPane1.right, rcPane2.top };
			m_rcHorzSplitter = rcSplitter;
			if( m_dwViews & vtPane1 )
				m_fHorzSplitter = ( rcPane1.bottom - rcPane1.top ) / double( rcFull.bottom - rcFull.top - 2 * m_nSplitterHalfWidth );
			else
				m_fHorzSplitter = ( rcPane2.bottom - rcPane2.top ) / double( rcFull.bottom - rcFull.top - 2 * m_nSplitterHalfWidth );
		}
	}														  //   |	---
	else if( m_nSplitterType == 2 || m_nSplitterType == 3 )   //  ---    |
	{
		if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
		{
			rcPane0 = rcClient;

			long h0 = rcPane0.bottom - rcPane0.top;


			if( m_nSplitterType == 3 )
			{
				rcPane0.bottom = (long)( rcPane0.top + __max( ( h0 - 2 * m_nSplitterHalfWidth ) * m_fHorzSplitter, h0_m ) );
				rcPane0.bottom = (long)( rcPane0.top + __min( rcPane0.bottom - rcPane0.top, ( h0 - 2 * m_nSplitterHalfWidth ) - __max( h1_m, h2_m ) ) );

				rcClient.top = rcPane0.bottom + 2 * m_nSplitterHalfWidth;
			}
			else
			{
				rcPane0.top = (long)( rcPane0.bottom - __max( ( h0 - 2 * m_nSplitterHalfWidth ) * m_fHorzSplitter, h0_m ) );
				rcPane0.top = (long)( rcPane0.bottom - __min( rcPane0.bottom - rcPane0.top, ( h0 - 2 * m_nSplitterHalfWidth ) - __max( h1_m, h2_m ) ) );

				rcClient.bottom = rcPane0.top - 2 * m_nSplitterHalfWidth;
			}
		}
		else if( m_dwViews == vtPane0 )
		{
			if( rcClient.right - rcClient.left < w0_m )
				rcClient.right = rcPane0_min.right;

			if( rcClient.bottom - rcClient.top < h0_m )
				rcClient.bottom = rcPane0_min.bottom;

			rcPane0 = rcClient;
		}

		if( m_dwViews & vtPane1 && m_dwViews & vtPane2 )
		{
			rcPane1 = rcClient;
			
			long w1 = rcPane1.right - rcPane1.left;

			rcPane1.right = (long)( rcPane1.left + __max(int((w1 - 2*m_nSplitterHalfWidth) * m_fVertSplitter), w1_m ) );
			rcPane1.right = (long)( rcPane1.left + __min(rcPane1.right - rcPane1.left, int((w1 - 2*m_nSplitterHalfWidth) - w2_m)) );

			rcClient.left = rcPane1.right + 2 * m_nSplitterHalfWidth;
			
			rcPane2 = rcClient;
		}
		else if( m_dwViews & vtPane1 )
		{
			if( m_dwViews == vtPane1 )
			{
				if( rcClient.right - rcClient.left < w1_m )
					rcClient.right = rcPane1_min.right;

				if( rcClient.bottom - rcClient.top < h1_m )
					rcClient.bottom = rcPane1_min.bottom;
			}

			rcPane1 = rcClient;
		}
		else if( m_dwViews & vtPane2 )
		{
			if( m_dwViews == vtPane2 )
			{
				if( rcClient.right - rcClient.left < w2_m )
					rcClient.right = rcPane2_min.right;

				if( rcClient.bottom - rcClient.top < h2_m )
					rcClient.bottom = rcPane2_min.bottom;
			}

			rcPane2 = rcClient;
		}

		if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
		{
			if( m_nSplitterType == 3 )
			{
				RECT rcSplitter = { rcPane0.left, rcPane0.bottom, rcPane0.right, rcPane0.bottom + 2 * m_nSplitterHalfWidth };
				m_rcHorzSplitter = rcSplitter;

				if( m_dwViews & vtPane1 )
					m_fVertSplitter = ( rcPane1.bottom - rcPane1.left ) / double( rcFull.right - rcFull.left - 2 * m_nSplitterHalfWidth );
				else
					m_fVertSplitter = ( rcPane2.right - rcPane2.left ) / double( rcFull.right - rcFull.left - 2 * m_nSplitterHalfWidth );
			}
			else
			{
				RECT rcSplitter = { rcPane0.left, rcPane0.top - 2 * m_nSplitterHalfWidth, rcPane0.right, rcPane0.top };
				m_rcHorzSplitter = rcSplitter;

				if( m_dwViews & vtPane1 )
					m_fVertSplitter = ( rcPane1.right - rcPane1.left ) / double( rcFull.right - rcFull.left - 2 * m_nSplitterHalfWidth );
				else
					m_fVertSplitter = ( rcPane2.right - rcPane2.left ) / double( rcFull.right - rcFull.left - 2 * m_nSplitterHalfWidth );
			}
		}

		if( m_dwViews & vtPane1 || m_dwViews & vtPane2 )
		{
			RECT rcSplitter = { rcPane1.right, rcPane1.top , rcPane2.left, rcPane1.bottom };
			m_rcVertSplitter = rcSplitter;
			
			m_fHorzSplitter = ( rcPane0.bottom - rcPane0.top ) / double( rcFull.bottom - rcFull.top - 2 * m_nSplitterHalfWidth );
		}
	}

	if( m_nPane0_Data == 0 )
		m_rcChartView = rcPane0;
	else if( m_nPane0_Data == 1 )
		m_rcLegendView = rcPane0;
	else if( m_nPane0_Data == 2 )
		m_rcTableView = rcPane0;

	if( m_nPane1_Data == 0 )
		m_rcChartView = rcPane1;
	else if( m_nPane1_Data == 1 )
		m_rcLegendView = rcPane1;
	else if( m_nPane1_Data == 2 )
		m_rcTableView = rcPane1;

	if( m_nPane2_Data == 0 )
		m_rcChartView = rcPane2;
	else if( m_nPane2_Data == 1 )
		m_rcLegendView = rcPane2;
	else if( m_nPane2_Data == 2 )
		m_rcTableView = rcPane2;

	if( sptrZoom != 0 )
	{

		sizeNew.cx = __max( sizeNew.cx, m_rcChartView.right );
		sizeNew.cy = __max( sizeNew.cy, m_rcChartView.bottom );

		sizeNew.cx = __max( sizeNew.cx, m_rcLegendView.right );
		sizeNew.cy = __max( sizeNew.cy, m_rcLegendView.bottom );

		sizeNew.cx = __max( sizeNew.cx, m_rcTableView.right );
		sizeNew.cy = __max( sizeNew.cy, m_rcTableView.bottom );
		

		if( m_nSplitterType == 0 || m_nSplitterType == 1 )
		{
			if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
			{
				sizeNew.cx = __max( sizeNew.cx, m_rcVertSplitter.right );
				sizeNew.cy = __max( sizeNew.cy, m_rcVertSplitter.bottom );
			}
			if( m_dwViews & vtPane1 && m_dwViews & vtPane2 )
			{
				sizeNew.cx = __max( sizeNew.cx, m_rcHorzSplitter.right );
				sizeNew.cy = __max( sizeNew.cy, m_rcHorzSplitter.bottom );
			}
		}
		else
		{
			if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
			{
				sizeNew.cx = __max( sizeNew.cx, m_rcHorzSplitter.right );
				sizeNew.cy = __max( sizeNew.cy, m_rcHorzSplitter.bottom );
			}

			if( m_dwViews & vtPane1 && m_dwViews & vtPane2 )
			{
				sizeNew.cx = __max( sizeNew.cx, m_rcVertSplitter.right );
				sizeNew.cy = __max( sizeNew.cy, m_rcVertSplitter.bottom );
			}
		}
		{
			sizeNew.cx = long( sizeNew.cx / _fZoom );
			sizeNew.cy = long( sizeNew.cy / _fZoom );
		}
	}


	sptrZoom->SetClientSize( sizeNew );
//	m_wndChart.m_dwViews = m_dwViews;
	m_bLockRecalc = false;
	return sizeNew;
}

void CCmpStatObjectView::load_from_ndata( INamedDataPtr sptrND )
{
	if( sptrND == 0 )
		return;
	m_bLockRecalc = false;

	m_nSplitterType = ::GetValueInt( sptrND, SECT_STATUI_ROOT, SPLITTER_TYPE, m_nSplitterType );

	m_nPane0_Data = ::GetValueInt( sptrND, SECT_STATUI_ROOT, PANE0_DATA, m_nPane0_Data );
	m_nPane1_Data = ::GetValueInt( sptrND, SECT_STATUI_ROOT, PANE1_DATA, m_nPane1_Data );
	m_nPane2_Data = ::GetValueInt( sptrND, SECT_STATUI_ROOT, PANE2_DATA, m_nPane2_Data );

	m_dwViews = ::GetValueInt( sptrND, SECT_STATUI_ROOT, "Views", m_dwViews );
	m_fHorzSplitter = ::GetValueDouble( sptrND, SECT_STATUI_ROOT, "HSplitter", m_fHorzSplitter );
	m_fVertSplitter = ::GetValueDouble( sptrND, SECT_STATUI_ROOT, "VSplitter", m_fVertSplitter );

	m_bShowContext = ::GetValueInt( sptrND, SECT_STATUI_ROOT, "ShowContext", m_bShowContext ) != 0;

	m_bLockRecalc = true;
	long lV = m_dwViews;
	m_dwViews = -1;
	ShowView( lV );


	//if( m_dwViews & vtChartView )
		m_wndChart.load_from_ndata( sptrND );
	//if( m_dwViews & vtLegendView )
		m_wndLegend.load_from_ndata( sptrND );
	//if( m_dwViews & vtTableView )
		m_wndTable.load_from_ndata( sptrND );
	m_bLockRecalc = false;
}

void CCmpStatObjectView::store_to_ndata( INamedDataPtr sptrND )
{
	if( sptrND == 0 )
		return;

	if( m_bPrintMode )
		return;

	::SetValue( sptrND, SECT_STATUI_ROOT, SPLITTER_TYPE, (long)m_nSplitterType );

	::SetValue( sptrND, SECT_STATUI_ROOT, PANE0_DATA, (long)m_nPane0_Data );
	::SetValue( sptrND, SECT_STATUI_ROOT, PANE1_DATA, (long)m_nPane1_Data );
	::SetValue( sptrND, SECT_STATUI_ROOT, PANE2_DATA, (long)m_nPane2_Data );

	::SetValue( sptrND, SECT_STATUI_ROOT, "Views", (long)m_dwViews );
	::SetValue( sptrND, SECT_STATUI_ROOT, "HSplitter", (double)m_fHorzSplitter );
	::SetValue( sptrND, SECT_STATUI_ROOT, "VSplitter", (double)m_fVertSplitter );

	::SetValue( sptrND, SECT_STATUI_ROOT, "ShowContext", (long)m_bShowContext );

	if( m_dwViews & vtChartView )
		m_wndChart.store_to_ndata( sptrND );
	if( m_dwViews & vtLegendView )
		m_wndLegend.store_to_ndata( sptrND );
	if( m_dwViews & vtTableView )
		m_wndTable.store_to_ndata( sptrND );
}

const RECT CCmpStatObjectView::_calc_min_view_size()
{
	RECT rcPane0_min, rcPane1_min, rcPane2_min;
	DWORD vtPane0 = 0, vtPane1 = 0, vtPane2 = 0;

	m_wndChart.m_bMinSizeCalc = false;
	m_wndLegend.m_bMinSizeCalc = false;
	m_wndTable.m_bMinSizeCalc = false;

	if( m_dwViews & vtChartView )
		m_wndChart.calc_min_rect( m_bPrintMode );
	
	if( m_dwViews & vtLegendView )
		m_wndLegend.calc_min_rect( m_bPrintMode );
	
	if( m_dwViews & vtTableView )
		m_wndTable.calc_min_rect( m_bPrintMode );

	RECT rcMinChart  = ( m_dwViews & vtChartView ) ? m_wndChart.get_min_rect() : NORECT;
	RECT rcMinLegend = ( m_dwViews & vtLegendView ) ? m_wndLegend.get_min_rect() : NORECT;
	RECT rcMinTable  = ( m_dwViews & vtTableView ) ? m_wndTable.get_min_rect() : NORECT;

	if( m_nPane0_Data == 0 )
		vtPane0 = vtChartView;
	else if( m_nPane0_Data == 1 )
		vtPane0 = vtLegendView;
	else if( m_nPane0_Data == 2 )
		vtPane0 = vtTableView;

	if( m_nPane1_Data == 0 )
		vtPane1 = vtChartView;
	else if( m_nPane1_Data == 1 )
		vtPane1 = vtLegendView;
	else if( m_nPane1_Data == 2 )
		vtPane1 = vtTableView;

	if( m_nPane2_Data == 0 )
		vtPane2 = vtChartView;
	else if( m_nPane2_Data == 1 )
		vtPane2 = vtLegendView;
	else if( m_nPane2_Data == 2 )
		vtPane2 = vtTableView;

	if( m_nPane0_Data == 0 )
		rcPane0_min  = rcMinChart;
	else if( m_nPane0_Data == 1 )
		rcPane0_min = rcMinLegend;
	else if( m_nPane0_Data == 2 )
		rcPane0_min  = rcMinTable;

	if( m_nPane1_Data == 0 )
		rcPane1_min  = rcMinChart;
	else if( m_nPane1_Data == 1 )
		rcPane1_min = rcMinLegend;
	else if( m_nPane1_Data == 2 )
		rcPane1_min  = rcMinTable;

	if( m_nPane2_Data == 0 )
		rcPane2_min  = rcMinChart;
	else if( m_nPane2_Data == 1 )
		rcPane2_min = rcMinLegend;
	else if( m_nPane2_Data == 2 )
		rcPane2_min  = rcMinTable;
	
	if( !( m_dwViews & vtPane0 ) )
		::ZeroMemory( &rcPane0_min, sizeof( RECT ) );

	if( !( m_dwViews & vtPane1 ) )
		::ZeroMemory( &rcPane1_min, sizeof( RECT ) );

	if( !( m_dwViews & vtPane2 ) )
		::ZeroMemory( &rcPane2_min, sizeof( RECT ) );

	if( m_nSplitterType == 0 || m_nSplitterType == 1 )   //  |-     -|
	{
		if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
		{
			if( m_nSplitterType == 0 )
			{
				::OffsetRect( &rcPane1_min, rcPane0_min.right - rcPane0_min.left + 2 * m_nSplitterHalfWidth, 0 );
				::OffsetRect( &rcPane2_min, rcPane0_min.right - rcPane0_min.left + 2 * m_nSplitterHalfWidth, 0 );
			}
			else
			{
				int w2 = __max( rcPane1_min.right - rcPane1_min.left, rcPane2_min.right - rcPane2_min.left );
				::OffsetRect( &rcPane0_min, w2 + 2 * m_nSplitterHalfWidth, 0 );
			}
		}

		if( m_dwViews & vtPane1 )
			if( m_dwViews & vtPane2 )
				::OffsetRect( &rcPane2_min, 0, rcPane1_min.bottom - rcPane1_min.top + 2 * m_nSplitterHalfWidth );

	}														  //   |	---
	else if( m_nSplitterType == 2 || m_nSplitterType == 3 )   //  ---    |
	{
		if( m_dwViews & vtPane0 && ( m_dwViews & vtPane1 || m_dwViews & vtPane2 ) )
		{
			if( m_nSplitterType == 3 )
			{
				::OffsetRect( &rcPane1_min, 0, rcPane0_min.bottom - rcPane0_min.top + 2 * m_nSplitterHalfWidth );
				::OffsetRect( &rcPane2_min, 0, rcPane0_min.bottom - rcPane0_min.top + 2 * m_nSplitterHalfWidth );
			}
			else
			{
				int h2 = __max( rcPane1_min.bottom - rcPane1_min.top, rcPane2_min.bottom - rcPane2_min.top );
				::OffsetRect( &rcPane0_min, 0, h2 + 2 * m_nSplitterHalfWidth );
			}
		}

		if( m_dwViews & vtPane1 )
			if( m_dwViews & vtPane2 )
				::OffsetRect( &rcPane2_min, rcPane1_min.right - rcPane1_min.left + 2 * m_nSplitterHalfWidth, 0 );
	}

	IScrollZoomSitePtr ptrZ = Unknown();
	double _fZoom = 1.0;

	rcPane0_min = ScaleRect( rcPane0_min, _fZoom );
	rcPane1_min = ScaleRect( rcPane1_min, _fZoom );
	rcPane2_min = ScaleRect( rcPane2_min, _fZoom );

	SIZE sizeNew = {0};

	sizeNew.cx = __max( sizeNew.cx, rcPane0_min.right );
	sizeNew.cy = __max( sizeNew.cy, rcPane0_min.bottom );

	sizeNew.cx = __max( sizeNew.cx, rcPane1_min.right );
	sizeNew.cy = __max( sizeNew.cy, rcPane1_min.bottom );

	sizeNew.cx = __max( sizeNew.cx, rcPane2_min.right );
	sizeNew.cy = __max( sizeNew.cy, rcPane2_min.bottom );
	

//	sizeNew.cx--;
//	sizeNew.cy--;

	RECT rc = { 0, 0, sizeNew.cx, sizeNew.cy };
	return rc;
}

bool CCmpStatObjectView::_is_ndata_empty( IUnknownPtr sptr )
{
	INamedDataPtr sptrND = sptr;

	if( sptrND == 0 )
		return true;

	sptrND->SetupSection( _bstr_t( SECT_STATUI_ROOT ) );
	
	_variant_t var;
	sptrND->GetValue( _bstr_t( "Loaded" ), &var );

	if( var.vt == VT_EMPTY )
	{
		sptrND->SetValue( _bstr_t( "Loaded" ), _variant_t( 1L ) );
		return true;
	}

	return false;
}

void CCmpStatObjectView::OnChange()
{
	INamedPropBagPtr sptrBag = Unknown();
	if( sptrBag )
	{
		_variant_t var;
		
		sptrBag->GetProperty( _bstr_t( PROPBAG_PANES ), &var );

		if( var.vt == VT_I4 )
		{
			if( var.lVal == vtLegendView || var.lVal == vtTableView){
			}else{
				var.lVal=vtChartView;
			}
				ShowView( var.lVal );
		}
	}
}

bool CCmpStatObjectView::_check_for_error( IUnknownPtr sptrData )
{
	INamedPropBagPtr sptrBag = sptrData;

	if( sptrBag == 0 )
		return false;
	
	_variant_t var;

	sptrBag->GetProperty( _bstr_t( "_has_data_" ), &var );

	if( var.vt != VT_I4 )
		return true;

	if( var.vt == VT_I4 && !var.lVal )
		return true;

	IStatObjectDispPtr sptrStat = sptrData;

	long lClass = 0;
	sptrStat->GetClassCount( &lClass );

	if( !lClass )
		return true;
		

	return false;
}

void CCmpStatObjectView::_clear_attached()
{
	m_bReadyToShow=false;
	m_pCmpStatObject=0;
	m_wndLegend.clear_attached();
//	m_wndTable.clear_attached();
}
}
namespace{

#if _MSC_VER < 1400 
__inline static unsigned char _bittest(const LONG* pdw, int n){
	__asm {
		xor eax,eax
		mov ebx, n
		mov ecx,[pdw]
		bt [ecx], ebx
		adc eax,eax
	}
}
#else
#pragma intrinsic(_bittest)
#endif

	enum Pane {
		pane0=0,
		pane1=-1,
		pane2=-2
	};

	struct splitter{
		bool _bVert;
		enum Pane _pane[2];

		// return HWND of splitter or single window
		HWND SetSplitter(ViewSpace::CCmpStatObjectView* pStatView,
			HWND hParent, HWND* wnds, int* panes)
		{
			BOOL b=0; HWND hw;

			CWindow* pWndSplitter=_bVert?
				dynamic_cast<CWindow*>(&pStatView->m_wndVertSplitter):
				dynamic_cast<CWindow*>(&pStatView->m_wndHorzSplitter);
			_ASSERTE(IsWindow(*pWndSplitter));

			int nPanes=0, iActive=-1;
			HWND hWnd[2]={0};

			for(int i=0; i<2; ++i){
				enum Pane pane=_pane[i];
				if(pane>0 && pane<3){
					hw=this[pane].SetSplitter(pStatView,
						hParent,wnds,panes);
				}else{
					pane=Pane(-pane);
					if(pane<3){
						int dwView=panes[pane];
						hw=wnds[dwView];
						if(_bittest((LONG*)&pStatView->m_dwViews,dwView)){
							b=::ShowWindow(hw,SW_SHOW);
						}else{
							b=::ShowWindow(hw,SW_HIDE);
							hw=0;
						}
					}
				}
				if(hw){
					++nPanes;
					iActive=i;
					hWnd[i]=hw;
				}
			}

			if(2==nPanes)
			{
				hw=::SetParent(*pWndSplitter,hParent);
				_ASSERTE(IsWindow(hw));
				hw=SetParent(hWnd[0],*pWndSplitter);
				_ASSERTE(IsWindow(hw));
				hw=SetParent(hWnd[1],*pWndSplitter);
				_ASSERTE(IsWindow(hw));
				if(_bVert){
					((CSplitterWindowT<true>*)pWndSplitter)->SetSplitterPanes(hWnd[0],hWnd[1]);
				}else{
					((CSplitterWindowT<false>*)pWndSplitter)->SetSplitterPanes(hWnd[0],hWnd[1]);
				}
				pWndSplitter->ShowWindow(SW_SHOW);
				return *pWndSplitter;
			}
			else if(1==nPanes)
			{
				pWndSplitter->ShowWindow(SW_HIDE);
				hw=::SetParent(hWnd[iActive],hParent);
				_ASSERTE(IsWindow(hw));
				return hWnd[iActive];
			}
			else{
				return 0;
			}
		}
	} stplitterType[4][2]={
		{{true,pane0,Pane(1)}, {false,pane1,pane2}},
		{{true,Pane(1),pane0}, {false,pane1,pane2}},
		{{false,Pane(1),pane0}, {true,pane1,pane2}},
		{{false,pane0,Pane(1)}, {true,pane1,pane2}}
	};
}

namespace ViewSpace 
{

HWND CCmpStatObjectView::SetSplitterStruct(void)
{
	HWND hWndRootSplitter=0;
	HWND wnds[3]={m_wndChart.handle(), m_wndLegend.handle(), m_wndTable.handle()};
	int panes[3]={m_nPane0_Data,m_nPane1_Data,m_nPane2_Data};

	if(0<=m_nSplitterType && m_nSplitterType<=3){
		hWndRootSplitter=stplitterType[m_nSplitterType][0].SetSplitter(
			this, hwnd(), wnds, panes);
		::SendMessage(hWndRootSplitter, WM_USER_MIN_SIZE, 0, 0);

		m_wndHorzSplitter.SetSplitterPos();
		m_wndVertSplitter.SetSplitterPos();
	}else{
		_ASSERTE(!"Invalid splitter type");
	}
	return hWndRootSplitter;
}

} // namespace
