#include "stdafx.h"
#include "MetodicsTab.h"
#include "resource.h"
#include "drag_drop_defs.h"
#include "MenuRegistrator.h"

#include "misc.h"

namespace MetodicsTabSpace
{
/////////////////////////////////////////////////////////////////////////////////////////
//CMainFrameDropTarget
CMtdTabTarget::CMtdTabTarget()
{
	m_pOwner = 0;
}


void CMtdTabTarget::Register( CMetodicsTab* pWnd )
{
	m_pOwner = pWnd;
	COleDropTarget::Register( pWnd );
}

DROPEFFECT CMtdTabTarget::OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
	DWORD dwKeyState, CPoint point)
{
	if( !pDataObject->IsDataAvailable( CMFCToolBarButton::m_cFormat ) )
		return DROPEFFECT_NONE;
	else
		m_pOwner->OnEnter( pWnd, pDataObject, dwKeyState, point );
	
	return DROPEFFECT_MOVE;
}

DROPEFFECT CMtdTabTarget::OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
{
	if( !pDataObject->IsDataAvailable( CMFCToolBarButton::m_cFormat ) )
		return DROPEFFECT_NONE;
	
	return DROPEFFECT_MOVE;
}

DROPEFFECT CMtdTabTarget::OnDragLeave(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
{
	if( !pDataObject->IsDataAvailable( CMFCToolBarButton::m_cFormat ) )
		return DROPEFFECT_NONE;
	else
	{
		m_pOwner->OnEnter( pWnd, pDataObject, dwKeyState, point );
		return FALSE;
	}

	return DROPEFFECT_MOVE;
	
}

BOOL CMtdTabTarget::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point)
{
	if( pDataObject->IsDataAvailable( CMFCToolBarButton::m_cFormat ) )
		return m_pOwner->OnDrop( pWnd, pDataObject, dropEffect, point );
	
	return FALSE;
}
////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMetodicsTab, CWnd);

CMetodicsTab::CMetodicsTab( ) 
{
	m_strCurContent = ::GetValueString( ::GetAppUnknown(), "\\General", "CurMtdTabContent", "" );

	//m_dwUsedPanes = 0xFFFF;
	_OleLockApp( this );

	m_dwDockSide = AFX_IDW_DOCKBAR_LEFT;
	m_size = CSize(250, 100);
					
	m_sName = "MetodicsTab";
	m_sUserName.LoadString( IDS_METODICS_TAB );

	EnableAutomation();

	Register(0);
//	m_lLastViewKey = INVALID_KEY;
//	m_nItemCount = 0;
//	m_nWidth = ::GetValueInt( ::GetAppUnknown(), FISH_ROOT, FISH_PREVIEW_WIDTH , 80 );

	m_pBar = 0;

	::ZeroMemory( &m_ptContextPoint, sizeof( m_ptContextPoint ) );

	// vanek : drawing disabled icons - 27.02.2005
	CPaneItem::m_ImgDrawer.SetEnableDraw( 0 != GetValueInt( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsEnable, 0 ) );
	CPaneItem::m_ImgDrawer.SetAlpha( (float)(GetValueDouble( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsAlpha, 0.5 )) );
}

CMetodicsTab::~CMetodicsTab()
{
	UnRegister(0);
	
	_OleUnlockApp( this );

	/*for( IT it = m_arrLayers.begin(); it != m_arrLayers.end(); ++it )
		delete (*it);
	m_arrLayers.clear();*/

	if( m_pBar )
	{
		delete m_pBar;
		m_pBar = 0;
	}
}

BEGIN_MESSAGE_MAP(CMetodicsTab, CWnd)
	//{{AFX_MSG_MAP(CFishContextViewer)
    //}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_WM_DESTROY()
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CMetodicsTab, CWnd)
	INTERFACE_PART(CMetodicsTab, IID_IWindow,	Wnd				)
	INTERFACE_PART(CMetodicsTab, IID_IDockableWindow, Dock		)
	INTERFACE_PART(CMetodicsTab, IID_IRootedObject,	Rooted		)
	INTERFACE_PART(CMetodicsTab, IID_IEventListener,	EvList	)
	INTERFACE_PART(CMetodicsTab, IID_INamedObject2,	Name		)
	INTERFACE_PART(CMetodicsTab, IID_IMsgListener,	MsgList		)
	INTERFACE_PART(CMetodicsTab, IID_IHelpInfo,		Help		)
	INTERFACE_PART(CMetodicsTab, IID_IMethodTab,		MtdTab	)
END_INTERFACE_MAP()

// {9133D476-3D21-4932-8FFB-5A7807815109}
GUARD_IMPLEMENT_OLECREATE( CMetodicsTab, "Metodics.MetodicsTab", 
0x9133d476, 0x3d21, 0x4932, 0x8f, 0xfb, 0x5a, 0x78, 0x7, 0x81, 0x51, 0x9);

IMPLEMENT_UNKNOWN( CMetodicsTab, MtdTab );

//////////////////////////////////////////////////////////////////
CWnd *CMetodicsTab::GetWindow()
{
	return this;
}  

//////////////////////////////////////////////////////////////////
void CMetodicsTab::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, "AppInit" ) )
	{
		ICompManagerPtr ptr_cm = GetAppUnknown();
		if( ptr_cm )
			ptr_cm->AddComponent( GetControllingUnknown(), 0 );
	}

	if ( !lstrcmp( pszEvent, szAppActivateView ) || !lstrcmp( pszEvent, szEventActivateObject )  || !lstrcmp( pszEvent, szEventAfterActionExecute )  )
	{
		::InvalidateRect( m_pBar->handle(), 0, TRUE );
	}

	return;
}

/////////////////////////////////////////////////////////////////////////////
void CMetodicsTab::PostNcDestroy()
{
	ICompManagerPtr ptr_cm = GetAppUnknown();
	if( ptr_cm )
	{
		int iCount = 0;
		ptr_cm->GetCount( &iCount );

		for( int i = 0; i < iCount; i++ )
		{
			IUnknown *punk = 0;
			ptr_cm->GetComponentUnknownByIdx( i, &punk );

			if( punk == GetControllingUnknown() )
			{
				ptr_cm->RemoveComponent( i );
				break;
			}
			if( punk )
				punk->Release();
		}
	}
	delete this;
}

int CMetodicsTab::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pBar = new CPaneBar( );
	if( !m_pBar )
        return -1;

	RECT rect = {0};
	GetClientRect( &rect );
	m_pBar->create( WS_VISIBLE | WS_CHILD, rect, 0, m_hWnd, (HMENU)10000 );

	m_target.Register( this  );

	IMethodTabPtr sptrTab = GetControllingUnknown();

	if( !m_strCurContent.IsEmpty() )
		sptrTab->LoadContent( _bstr_t( m_strCurContent ) );

	ModifyStyle( 0, WS_VSCROLL );

	m_pBar->handle_message( WM_NEED_RECALC, -1, 0 );
	return 0;
}

void CMetodicsTab::OnSize(UINT nType, int cx, int cy)
{
	__super::OnSize(nType, cx, cy);

	if( m_pBar )
		::SetWindowPos( m_pBar->handle(), 0, 0, 0, cx, cy, 0 );

	long lindex = -1;
	long lid = -1;
	for( long i = 0; i < m_pBar->get_item_count(); i++ )
	{
		CPane *pPane = m_pBar->get_item( i );
		if( pPane->is_open())
		{
			lindex = i;
			lid = ::GetDlgCtrlID( pPane->handle() );
			break;
		}
	}

	SCROLLINFO info = {0};
 	info.cbSize				= sizeof( SCROLLINFO );
	info.fMask				= SIF_ALL;
	::GetScrollInfo( m_hWnd, SB_VERT,&info );

	::SendMessage( m_pBar->handle(), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, 0 ), 0 );
	
	m_pBar->handle_message	( WM_NEED_RECALC, lid, 1 );

	SCROLLINFO info2 = {0};
 	info2.cbSize				= sizeof( SCROLLINFO );
	info2.fMask				= SIF_ALL;

	::GetScrollInfo( m_hWnd, SB_VERT,&info2 );
/*
	if( info.nPage < info2.nPage && info2.nPos == ( info2.nMax - info2.nPage + 1 ) )
	{
		::SendMessage( m_pBar->handle(), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, info2.nPage - info.nPage ), -1 );
		::SendMessage( m_pBar->handle(), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, info2.nMax - info2.nPage + 1 ), 0 );
	}
*/
	::SendMessage( m_pBar->handle(), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, min( info.nPos, info2.nMax - info2.nPage + 1 ) ), 0 );
	
}

LRESULT CMetodicsTab::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_VSCROLL )
		::SendMessage( m_pBar->handle(), message, wParam, lParam );

	if( message == WM_MOUSEWHEEL )
	{
		SetFocus();
		IUnknownPtr ptr_u( ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow ), false );
		IMainWindowPtr sptr_w = ptr_u;
		if( sptr_w )
		{
			BOOL bhelp_mode = 0;
			sptr_w->IsHelpMode( &bhelp_mode );

			if( !bhelp_mode )
			{
				for( long i = 0; i < m_pBar->get_item_count(); i++ )
				{
					CPane *pPane = m_pBar->get_item( i );

					if( pPane && pPane->is_open() )
					{
						/*CPaneItemPlace *pitem_place = pPane->get_item_place();
						if( pitem_place )
							pitem_place->handle_message( message, wParam, lParam );
							*/
					}
				}
			}
		}
	}
	if( message == WM_GETINTERFACE )
		return (LRESULT)GetControllingUnknown();

  	if( message == WM_IDLEUPDATECMDUI )
	{
		IUnknownPtr ptr_u( ::_GetOtherComponent( ::GetAppUnknown(), IID_IMainWindow ), false );
		IMainWindowPtr sptr_w = ptr_u;
		if( sptr_w )
		{
			BOOL bhelp_mode = 0;
			sptr_w->IsHelpMode( &bhelp_mode );

			if( !bhelp_mode )
			{
				for( long i = 0; i < m_pBar->get_item_count(); i++ )
				{
					CPane *pPane = m_pBar->get_item( i );

					if( pPane && pPane->is_open() )
					{
						CPaneItemPlace *pitem_place = pPane->get_item_place();
						if( pitem_place )
							pitem_place->handle_message( message, wParam, lParam );
					}
				}
			}
		}
	}
	return __super::WindowProc(message, wParam, lParam);
}

void CMetodicsTab::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if( !::GetValueInt( ::GetAppUnknown(), "\\Methodics", "ShowContextMenu", 1  ) )
		return;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;

	bool bItemSelected = false;
	
	CPane *pPane = m_pBar->get_item_by_pos( point );
	if( pPane )
	{
		CPaneItem *pItem = pPane->get_item_by_pos( point );
		if( pItem )
			bItemSelected = true;
	}

	m_ptContextPoint = point;

	CMenuRegistrator rcm;	

	CString strMenuName;

	if( !bItemSelected )
		strMenuName = (LPCSTR)rcm.GetMenu( szMetodicsTab, 0 );
	else
		strMenuName = (LPCSTR)rcm.GetMenu( szMetodicsTabAction, 0 );

	_bstr_t	bstrMenuName = strMenuName;
	sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
}

void CMetodicsTab::OnDestroy()
{
	__super::OnDestroy();

	IMethodTabPtr sptrTab = GetControllingUnknown();

	if( !m_strCurContent.IsEmpty() )
		sptrTab->StoreContent( _bstr_t( m_strCurContent ) );
}

void CMetodicsTab::OnEnable(BOOL bEnable)
{
	__super::OnEnable(bEnable);

	static bool bInside = false;

	if( !bInside )
	{
		// needed for bcg customize
		bInside = true;
		::EnableWindow( GetSafeHwnd(), TRUE );
		bInside = false;
	}
}


BOOL CMetodicsTab::OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point)
{
	if( !::GetValueInt( ::GetAppUnknown(), "\\Methodics", "EnableDragDrop", 1  ) )
		return FALSE;

	AFX_MODULE_STATE* pModuleState = AfxSetModuleState( 0 );

	CMFCToolBarButton* pButton = CMFCToolBarButton::CreateFromOleData (pDataObject);

	AfxSetModuleState( pModuleState );
	
	IApplicationPtr sptrApp = GetAppUnknown();
	
	IUnknown *punkCommandMan = 0;
	sptrApp->GetCommandManager( &punkCommandMan );
	ICommandManager2Ptr sptrCmdMan = punkCommandMan;
	punkCommandMan->Release(); punkCommandMan = 0;

	_bstr_t bstrName;
	sptrCmdMan->GetActionName( bstrName.GetAddress(), pButton->m_nID );

	CString strActionName = (char *)bstrName;

	delete pButton;


	IUnknown *punkActionMan = 0;
	sptrApp->GetActionManager( &punkActionMan );
	IActionManagerPtr sptrMan = punkActionMan;
	punkActionMan->Release(); punkActionMan = 0;

	IUnknown *punkActionInfo = 0;
	sptrMan->GetActionInfo( _bstr_t( strActionName ), &punkActionInfo );

	IActionInfoPtr sptrActionInfo = punkActionInfo;
	
	if( punkActionInfo )
		punkActionInfo->Release(); punkActionInfo = 0;

	if( sptrActionInfo )
	{
		pWnd->ClientToScreen( &point );
		CPane *pPane = m_pBar->get_item_by_pos( point );
		if( pPane )
		{
			if( pPane->is_open() )
				pPane->add_item( sptrActionInfo, point );
		}
	}

	return TRUE;
}

BOOL CMetodicsTab::OnEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point)
{
	if( !::GetValueInt( ::GetAppUnknown(), "\\Methodics", "EnableDragDrop", 1  ) )
		return FALSE;

	AFX_MODULE_STATE* pModuleState = AfxSetModuleState( 0 );

	CMFCToolBarButton* pButton = CMFCToolBarButton::CreateFromOleData (pDataObject);

	AfxSetModuleState( pModuleState );
	
	IApplicationPtr sptrApp = GetAppUnknown();
	
	IUnknown *punkCommandMan = 0;
	sptrApp->GetCommandManager( &punkCommandMan );
	ICommandManager2Ptr sptrCmdMan = punkCommandMan;
	punkCommandMan->Release(); punkCommandMan = 0;

	_bstr_t bstrName;
	sptrCmdMan->GetActionName( bstrName.GetAddress(), pButton->m_nID );

	CString strActionName = (char *)bstrName;

	delete pButton;


	IUnknown *punkActionMan = 0;
	sptrApp->GetActionManager( &punkActionMan );
	IActionManagerPtr sptrMan = punkActionMan;
	punkActionMan->Release(); punkActionMan = 0;

	IUnknown *punkActionInfo = 0;
	sptrMan->GetActionInfo( _bstr_t( strActionName ), &punkActionInfo );

	IActionInfoPtr sptrActionInfo = punkActionInfo;
	
	if( punkActionInfo )
		punkActionInfo->Release(); punkActionInfo = 0;

	if( sptrActionInfo )
	{
		pWnd->ClientToScreen( &point );
		CPane *pPane = m_pBar->get_item_by_pos( point );
		if( pPane )
		{
			if( pPane->is_open() )
				pPane->setDragDrop(sptrActionInfo);
			else
				return FALSE;
		}
	}
	return TRUE;
}


HRESULT CMetodicsTab::XMtdTab::AddPane( BSTR bstrName )
{
	_try_nested( CMetodicsTab, MtdTab, AddPane)
	{
 		POINT pt_context = pThis->m_ptContextPoint;
		CPaneBar *pBar =  pThis->m_pBar->get_bar_by_pos( pt_context );
		CPane *pPane =  pThis->m_pBar->get_item_by_pos( pt_context );

		CPaneBar *pBar_Main =  pBar;

		pBar->AddNewPane( (char *)_bstr_t( bstrName ), -1 );
 		pBar_Main->handle_message( WM_NEED_RECALC, -1, 0 );
		
		if( pPane && pPane->handle() )
			pBar_Main->handle_message( WM_NEED_RECALC, ::GetDlgCtrlID( pPane->handle() ), 0 );


		return S_OK;
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::Remove( long lIndex )
{
	_try_nested( CMetodicsTab, MtdTab, Remove )
	{
 		POINT pt_context = pThis->m_ptContextPoint;
		CPaneBar *pBar =  pThis->m_pBar->get_bar_by_pos( pt_context );

		CPane *pPane = 0;

		if( lIndex == -1 )
			lIndex = pBar->get_item_count() - 1;

		RECT rc_pane = {0};

 		pPane = pBar->get_item( lIndex );

		if( pPane )
		{
			::GetWindowRect( pPane->handle(), &rc_pane );

			::DestroyWindow( pPane->handle() );
			pBar->remove_item( lIndex );
		}

		if( !lIndex )
		{
			for( long i = 0; i < pBar->get_item_count(); i++ )
			{
				CPane *pPane = pBar->get_item( i );

				RECT rc_pane_scr = {0};
				::GetWindowRect(  pPane->handle() , &rc_pane_scr );
				 
				::ScreenToClient( ::GetParent( pPane->handle() ), (LPPOINT)&rc_pane_scr  );
				::ScreenToClient( ::GetParent( pPane->handle() ), (LPPOINT)&rc_pane_scr + 1  );

				::OffsetRect( &rc_pane_scr, 0,  rc_pane.top - rc_pane.bottom );
				::MoveWindow( pPane->handle(), rc_pane_scr.left, rc_pane_scr.top, rc_pane_scr.right - rc_pane_scr.left, rc_pane_scr.bottom - rc_pane_scr.top, TRUE );
			}
		}
		if( pBar == pThis->m_pBar )
		{
			::SendMessage( pThis->m_pBar->handle(), WM_VSCROLL, MAKELONG( SB_THUMBTRACK, 0 ), 0 );
			pThis->m_pBar->handle_message( WM_NEED_RECALC, -1, 0 );
		}
		else
		{
			pBar->handle_message( WM_NEED_RECALC, -1, 1 );
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::GetActivePane( long *plIndex )
{
	_try_nested( CMetodicsTab, MtdTab, GetActivePane )
	{
 		POINT pt_context = pThis->m_ptContextPoint;
		CPaneBar *pBar =  pThis->m_pBar->get_bar_by_pos( pt_context );

		if( !plIndex )
			return E_FAIL;

		*plIndex = -1;

		if(0==pBar) return S_OK;

		for( long i = 0; i < pBar->get_item_count(); i++ )
		{
			CPane *pPane = pBar->get_item( i );
			if( pPane->is_open() )
			{
				*plIndex = i;
			}
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::GetPaneCount( long *plCount )
{
	_try_nested( CMetodicsTab, MtdTab, GetPaneCount )
	{
		if( !plCount )
			return E_FAIL;

		*plCount = pThis->m_pBar->get_item_count();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::LoadContent( BSTR bstrFile )
{
	_try_nested( CMetodicsTab, MtdTab, LoadContent )
	{
		if( !bstrFile )
			return E_FAIL;

		IFileDataObjectPtr	sptrF( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );

		sptrF->LoadFile( bstrFile );

		INamedDataPtr sptrND = sptrF;

		pThis->m_pBar->load_content( sptrND, SECT_GENERAL );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::StoreContent( BSTR bstrFile )
{
	_try_nested( CMetodicsTab, MtdTab, StoreContent )
	{
		if( !bstrFile )
			return E_FAIL;

		IFileDataObjectPtr	sptrF( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );

		INamedDataPtr sptrND = sptrF;

		pThis->m_pBar->store_content( sptrND, SECT_GENERAL  );

		return sptrF->SaveFile( bstrFile );
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::GetContextPoint( POINT *lpptContext )
{
	_try_nested( CMetodicsTab, MtdTab, GetContextPoint )
	{

		if( !lpptContext )
			return E_FAIL;

		*lpptContext = pThis->m_ptContextPoint;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::PanePaneItemByPoint( POINT ptPoint )
{
	_try_nested( CMetodicsTab, MtdTab, PanePaneItemByPoint )
	{
		CPane *pPane = pThis->m_pBar->get_item_by_pos( ptPoint );
		if( !pPane )
			return E_FAIL;

		CPaneItem *pItemPt = pPane->get_item_by_pos( ptPoint );

		if( !pItemPt )
			return E_FAIL;

		long lCount = pPane->get_item_count();
		for( long i = 0; i < lCount; i++ )
		{
			CPaneItem *pItem = pPane->get_item( i );

			if( pItemPt == pItem )
			{
				pPane->remove_item( i );
				break;
			}
		}


		::PostMessage( ::GetParent( pPane->handle() ), WM_NEED_RECALC, -1, 0 );
		::PostMessage( ::GetParent( pPane->handle() ), WM_NEED_RECALC, ::GetDlgCtrlID( pPane->handle() ), 0 );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CMetodicsTab::XMtdTab::OpenPane( BSTR bstrPath, BOOL bOpen )
{
	_try_nested( CMetodicsTab, MtdTab, OpenPane )
	{
		pThis->m_pBar->open_mtd_pane( CString( bstrPath ), bOpen );

		return S_OK;
	}
	_catch_nested;
}

}

BOOL MetodicsTabSpace::CMetodicsTab::OnEraseBkgnd(CDC* pDC)
{
	return __super::OnEraseBkgnd(pDC);
}

LRESULT MetodicsTabSpace::CMetodicsTab::OnHelpHitTest(WPARAM wParam, LPARAM lParam)
{
	POINT pt = { 0 };
	::GetCursorPos( &pt );

	CPaneBar* pbar = m_pBar->get_bar_by_pos( pt );

	if( pbar )
		return pbar->help_hit_test( wParam,  lParam );

	return m_pBar->help_hit_test( wParam,  lParam );
}

void MetodicsTabSpace::CMetodicsTab::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	__super::OnVScroll(nSBCode, nPos, pScrollBar);
}


BOOL MetodicsTabSpace::CMetodicsTab::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_GETINTERFACE )
	{
		return (bool)GetControllingUnknown();
	}
	if( message == WM_MOUSEWHEEL )
	{
		short iWheel=GET_WHEEL_DELTA_WPARAM(wParam);
	}

	return __super::OnWndMsg(message, wParam, lParam, pResult);
}

BOOL MetodicsTabSpace::CMetodicsTab::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	/*if( zDelta <= 0 )
    { // вертикальная прокрутка вперед
        PostMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, zDelta), 0);
    }
    else
    { // вертикальная прокрутка назад
        PostMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, zDelta), 0);
    };  */  
	return __super::OnMouseWheel(nFlags, zDelta, pt);
}
