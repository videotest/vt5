// BlankView.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "BlankView.h"
#include "constant.h"
#include "VisualEdit.h"

#include "dbmacros.h"
#include "MenuRegistrator.h"
#include "drag_drop_defs.h"

#include "\vt5\awin\profiler.h"



/////////////////////////////////////////////////////////////////////////////
// CBlankView

// {0DC17DE5-654B-4ff8-A593-E1ECEEFE04AF}
GUARD_IMPLEMENT_OLECREATE(CBlankView, szBlankViewProgID, 
0xdc17de5, 0x654b, 0x4ff8, 0xa5, 0x93, 0xe1, 0xec, 0xee, 0xfe, 0x4, 0xaf);


IMPLEMENT_DYNCREATE(CBlankView, CAxBaseView);


BEGIN_INTERFACE_MAP(CBlankView, CAxBaseView)
	INTERFACE_PART(CBlankView, IID_IBlankView, BlankVw)	
	INTERFACE_PART(CBlankView, IID_IDBaseListener, DBaseListener)
	INTERFACE_PART(CBlankView, IID_IDBFilterView, FilterVw)
	INTERFACE_PART(CBlankView, IID_IClipboardProvider, ClipboardProvider)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CBlankView, BlankVw);
IMPLEMENT_UNKNOWN(CBlankView, ClipboardProvider);


/////////////////////////////////////////////////////////////////////////////
CBlankView::CBlankView()
{
	//m_bLoadingForm	= true;

	//m_bAddDBCtrl		= false;
	//m_bDeleteDBCtrl		= false;

	_OleLockApp( this );

	
	m_sUserName.LoadString(IDS_BLANKVIEW_NAME);
	m_sName = c_szCBlankView;	//"BlankView";


	//Register( GetAppUnknown() );	

	m_pointFormOffset = CPoint( 0, 0 );

	m_bCanResizeForm = false;

	m_BlankViewMode	= bvmPreview;

	m_ptPrevScroll = CPoint( 0, 0 );

	CreateFrame();

	m_block_set_mode = false;
	
}

/////////////////////////////////////////////////////////////////////////////
CBlankView::~CBlankView()
{
	_OleUnlockApp( this );
}

void CBlankView::CreateFrame()
{
	//m_pframeRuntime = RUNTIME_CLASS(CDesktopFrame);	
	m_pframeRuntime = RUNTIME_CLASS(CAxBaseFrame);	
}

void CBlankView::MoveDragFrameToFrame()
{
	/*
	IScrollZoomSitePtr pstrS( GetControllingUnknown() );	
	if( pstrS )
		pstrS->SetAutoScrollMode( FALSE );
	*/
	
	CAxBaseView::MoveDragFrameToFrame();
	
	/*
	if( pstrS )
	{
		pstrS->SetAutoScrollMode( TRUE );	

		IUnknown* punk = m_pframe->GetActiveObject();
		if( punk )
		{
			CRect rc_obj = GetObjectRect( punk );
			punk->Release();	punk = 0;

			pstrS->EnsureVisible( rc_obj );
		}
	}	
	*/
}


void CBlankView::OnDestroy() 
{

	UnRegister( ::GetAppUnknown() );

	//UnRegister listener
	IDocument* pDoc = NULL;
	pDoc = GetDocument();
	if( pDoc )
	{
		sptrIDBaseDocument spDoc(pDoc);
		if( spDoc )
			spDoc->UnRegisterDBaseListener( GetControllingUnknown() );		
		pDoc->Release();
	}

	CAxBaseView::OnDestroy();	
	
}


BEGIN_MESSAGE_MAP(CBlankView, CAxBaseView)
	//{{AFX_MSG_MAP(CBlankView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DND_ON_DRAG_OVER, OnDndOnDragOver)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
BlankViewMode CBlankView::GetBlankViewMode()
{
	return m_BlankViewMode;
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::SetBlankViewMode( BlankViewMode blankViewMode )
{
	BlankViewMode oldBlankViewMode = m_BlankViewMode;
	OnActivateView( false, NULL );
	m_BlankViewMode = blankViewMode;
	OnActivateView( true, NULL );

	/*
	if( oldBlankViewMode == bvmFilter )
		::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );
		*/

	SetNewModeToControls( oldBlankViewMode, m_BlankViewMode );

	if( m_BlankViewMode == bvmPreview )
	{
		if( m_pframe )
			m_pframe->EmptyFrame();
	}

	CWnd::Invalidate();
	
	if( m_BlankViewMode != bvmPreview )
		CWnd::SetFocus();
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::SetNewModeToControls( BlankViewMode oldBlankViewMode, BlankViewMode newBlankViewMode, IUnknown* punk_ctrl )
{
	POSITION	pos = m_controlInfos.GetHeadPosition();
	while( pos )
	{
		ControlData *pdata = m_controlInfos.GetNext( pos );
		if( pdata && pdata->pwnd && pdata->pwnd->GetSafeHwnd() )
		{
			if( punk_ctrl && ::GetObjectKey( punk_ctrl ) != ::GetObjectKey( pdata->punkCtrlData ) )
					continue;

			sptrIDBControl spDBCtrl( pdata->pwnd->GetControlUnknown() );
			if( spDBCtrl )
				spDBCtrl->SetBlankViewMode( (short)oldBlankViewMode, (short)newBlankViewMode );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::AttachActiveObjects()
{
	TIME_TEST( "CBlankView::AttachActiveObjects" );
	if( !GetSafeHwnd() )
		return;																		 
	IUnknownPtr	sptrControls( ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeBlankForm ), false );

	if( GetObjectKey( m_sptrControls ) == GetObjectKey( sptrControls ) )
		return;

	if( GetSafeHwnd() )
		_RemoveAllControls();
	
	m_pframe->UnselectObjectsWithSpecifiedParent( m_sptrControls );
	m_sptrControls.Attach( sptrControls );

	_UpdateLayout();
	
	m_block_set_mode = true;
	{
		_AddAllControls();	
	}
	m_block_set_mode = false;

	SetBlankViewMode( GetBlankViewMode() );

	if( GetBlankViewMode() == bvmPreview )
	{
		IQueryObjectPtr ptrQ( sptrControls );
		CString strTable, strField;
		if( ptrQ )
		{
			int nActiveField = 0;
			ptrQ->GetActiveField( &nActiveField );

			BSTR bstrTable = 0;
			BSTR bstrField = 0;

			if( S_OK == ptrQ->GetField( nActiveField, 0, &bstrTable, &bstrField ) )
			{
				::ActivateDBObjectInContext( bstrTable, bstrField, GetControllingUnknown() );

				if( bstrTable )
					::SysFreeString( bstrTable );	bstrTable = 0;

				if( bstrField )
					::SysFreeString( bstrField );	bstrField = 0;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
int CBlankView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CAxBaseView::OnCreate(lpCreateStruct) == -1)
		return -1;
	

	m_bmpAdd.LoadBitmap( IDB_ADD );
	m_bmpEdit.LoadBitmap( IDB_EDIT );
	m_bmpDelete.LoadBitmap( IDB_DELETE );
	
	sptrIScrollZoomSite	pstrS( GetControllingUnknown() );

	if( pstrS )
		pstrS->SetAutoScrollMode( TRUE );
	

	if( m_pframe && m_pframe->IsKindOf(RUNTIME_CLASS(CAxBaseFrame)) )
		((CAxBaseFrame*)m_pframe)->m_bUseParentScroll = true;

	//Register listener
	IDocument* pDoc = NULL;
	pDoc = GetDocument();
	if( pDoc )
	{
		sptrIDBaseDocument spDoc(pDoc);
		if( spDoc )
			spDoc->RegisterDBaseListener( GetControllingUnknown() );		
		pDoc->Release();
	}

	{
		IWindowPtr ptrWnd = GetControllingUnknown();
		DWORD dwFlags = 0;
		ptrWnd->GetWindowFlags( &dwFlags );
		ptrWnd->SetWindowFlags( ( dwFlags & ~wfZoomSupport ) | wfSupportFitDoc2Scr );
	}

	Register( ::GetAppUnknown() );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{	
	CAxBaseView::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );	
	
	if( !strcmp( pszEvent, szEventScrollPosition ) && 
		( ::GetObjectKey( GetControllingUnknown() ) == ::GetObjectKey(punkFrom) ) )
	{
		if( m_pframe )
			m_pframe->Redraw( true, true );

		CPoint ptScroll(0,0);
		IScrollZoomSitePtr ptrSZS( GetControllingUnknown() );
		if( ptrSZS )
		{	
			ptrSZS->SetZoom( 1.0 );
			ptrSZS->GetScrollPos( &ptScroll );			

			CWnd::ScrollWindow( m_ptPrevScroll.x - ptScroll.x, m_ptPrevScroll.y - ptScroll.y );

			m_ptPrevScroll = ptScroll;
			
		}
				
		CWnd::Invalidate();
		//UpdateWindow();
	}			

}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnSize(UINT nType, int cx, int cy) 
{
	CAxBaseView::OnSize(nType, cx, cy);
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{
	CString strEvent = pszEvent;
	if( strEvent == szDBaseEventBeforQueryOpen ||
		strEvent == szDBaseEventBeforQueryClose
		)
	{
		//notify controls
		//SetNewModeToControls( GetBlankViewMode(), GetBlankViewMode() );
	}
	else if( strEvent == szDBaseEventActiveFieldSet )
	{
		IDocument* pDoc = NULL;
		pDoc = GetDocument();
		if( pDoc )
		{
			if( GetFieldType( pDoc, bstrTableName, bstrFieldName ) == ftVTObject )
			{
				::ActivateDBObjectInContext( bstrTableName, bstrFieldName, GetControllingUnknown() );
			}
			pDoc->Release();
		}
	}
	else if( strEvent == szDBaseEventAfterQueryOpen )
	{
		IQueryObjectPtr ptrQ( m_sptrControls );
		if( ptrQ )
		{
			int nfield = 0;
			ptrQ->GetActiveField( &nfield );
			ptrQ->SetActiveField( nfield );
		}		
	}
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::_AddControl( IUnknown *punkControl )
{
	//PROFILE_FUNCTION( "CBlankView::_AddControl" );
	IActiveXCtrlPtr ptr_ctrl( punkControl );
	CString str_progid;
	CString str_msg;
	if( ptr_ctrl )
	{
		BSTR bstr = 0;
		ptr_ctrl->GetProgID( &bstr );
		str_progid = bstr;
		::SysFreeString( bstr );	bstr = 0;
	}

	/*
	str_msg = "---------------------_AddControl-----";
	str_msg += str_progid; 

	TIME_TEST( str_msg );
	*/

	CAxBaseView::_AddControl( punkControl );
	if( !m_block_set_mode )
		SetNewModeToControls( GetBlankViewMode(), GetBlankViewMode(), punkControl );
}


/////////////////////////////////////////////////////////////////////////////
void CBlankView::RepositionControls( )
{
	return;

	if( !GetSafeHwnd() )
		return;

	CPoint ptScrollOffset = CAxBaseView::GetScrollPos();

	CRect rcClient;
	CWnd::GetClientRect( &rcClient );
	//rcClient = ConvertToClient( rcClient );


	HDWP	hdwp = ::BeginDeferWindowPos( m_controlInfos.GetCount() );
	DWORD	dwFlags = SWP_NOACTIVATE | SWP_NOZORDER | SWP_DRAWFRAME;


	POSITION	pos = m_controlInfos.GetHeadPosition();
	while( pos )
	{
		ControlData *pdata = m_controlInfos.GetNext( pos );
		if( pdata && pdata->pwnd && pdata->pwnd->GetSafeHwnd() )
		{
			CRect rcCtrl;
			CWnd* pWnd = pdata->pwnd;
			if( pWnd && ::IsWindow(pWnd->GetSafeHwnd()) )
			{
				pWnd->GetWindowRect( rcCtrl );
				CWnd::ScreenToClient( &rcCtrl );
				//if( rcClient.IntersectRect(rcCtrl) )
				{
					sptrIActiveXCtrl spAXCtrl( pdata->punkCtrlData );
					if( spAXCtrl )
					{
						CRect rectAXCtrl;
						spAXCtrl->GetRect( &rectAXCtrl );
						//rectAXCtrl = ConvertToWindow( rectAXCtrl );
						rectAXCtrl += (m_pointFormOffset - ptScrollOffset);
						if( rcCtrl != rectAXCtrl )
						{
							//pWnd->MoveWindow( rectAXCtrl, FALSE );
							
							hdwp = ::DeferWindowPos( hdwp, pWnd->GetSafeHwnd(), HWND_NOTOPMOST, 
									rectAXCtrl.left, rectAXCtrl.top, 
									rectAXCtrl.Width(), rectAXCtrl.Height(), dwFlags);							

							pWnd->Invalidate( );
						}
					}				
				}
			}
		}		
	}

	::EndDeferWindowPos( hdwp );
	
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnRButtonDown(UINT nFlags, CPoint point) 
{	
	CAxBaseView::OnRButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
POSITION CBlankView::FindControlByPoint( CPoint pt )
{
	CPoint ptClick = pt;	

	POSITION posFind = NULL;

	POSITION	pos = m_controlInfos.GetHeadPosition();
	while( pos )
	{
		ControlData *pdata = m_controlInfos.GetNext( pos );
		if( pdata && pdata->pwnd && pdata->pwnd->GetSafeHwnd() )
		{						
			CRect rcCtrl;
			pdata->pwnd->GetWindowRect( &rcCtrl );
			CWnd::ScreenToClient( &rcCtrl );
			if( rcCtrl.PtInRect(ptClick) )
			{
				posFind = pos;
			}
		}
	}
	
	return posFind;
}

BOOL CBlankView::PreCreateWindow(CREATESTRUCT& cs) 
{	
	return CAxBaseView::PreCreateWindow(cs);
}

bool CBlankView::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;		
	return CViewBase::DoCreate( dwStyle, rc, pparent, nID );
}


/////////////////////////////////////////////////////////////////////////////
BOOL CBlankView::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnPaint() 
{
	_UpdateLayout( false );
	
	
	CRect rectPaint = NORECT;
	CWnd::GetClientRect( &rectPaint );	


	CPaintDC dc(this); 		


	{
		//dc.FillRect( rectPaint, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );	

		if( m_BlankViewMode == bvmPreview )
		{
			CBrush	pattern( ::GetSysColor( COLOR_3DFACE ) );
			dc.FillRect( rectPaint, &pattern);		
		}
		else
		{
			CBitmap bm;
			bm.LoadBitmap( IDB_PATTERN_VIEW );
			CBrush pattern;
			pattern.CreatePatternBrush( &bm );
			dc.FillRect( rectPaint, &pattern );
		}

		dc.DrawEdge( rectPaint, BDR_RAISEDINNER, BF_RECT );			
	}


	if( GetBlankViewMode() == bvmDesign )
		ProcessDrawing( dc, rectPaint, 0, 0 );	
	
	
	POSITION pos = m_controlInfos.GetHeadPosition();
	while( pos  )
	{
		ControlData *pdata = m_controlInfos.GetNext( pos );
		if( pdata->pwnd->GetSafeHwnd() )
		{
			LONG lExStyle = ::GetWindowLong( pdata->pwnd->GetSafeHwnd(), GWL_EXSTYLE );
			if( lExStyle & WS_EX_TRANSPARENT )
			{
				pdata->pwnd->Invalidate();
				pdata->pwnd->UpdateWindow();
			}
		}
	}			
	
	
	if( GetBlankViewMode() == bvmDesign )
	{		
		m_pframe->Draw( dc, NORECT, 0, 0 );
		if( m_pframe->m_bDragDropActive )
			m_pframeDrag->Draw( dc, NORECT, 0, 0 );
	}

	



}

					
/////////////////////////////////////////////////////////////////////////////
void CBlankView::_UpdateLayout( bool bRepositionControls )
{	
	CAxBaseView::_UpdateLayout( bRepositionControls );
}


void CBlankView::UpdateControlRect( IUnknown *punkControl )
{

	POSITION	pos = _Find( punkControl );

	if( !pos )
		return;

	ControlData *pdata = m_controlInfos.GetAt( pos );

	if( !pdata || !pdata->pwnd->GetSafeHwnd() )	
		return;

	IActiveXCtrlPtr	ptrAX( punkControl );

	CRect	rectControl;
	pdata->pwnd->GetWindowRect( &rectControl );
	

	CWnd::ScreenToClient( rectControl );

	{
		CRect rcWnd = rectControl;

		rcWnd.left		-= m_ptPrevScroll.x;
		rcWnd.right		-= m_ptPrevScroll.x;

		rcWnd.top		-= m_ptPrevScroll.y;
		rcWnd.bottom	-= m_ptPrevScroll.y;

		pdata->pwnd->MoveWindow( rcWnd );
	}

	
	rectControl-= Offset();

	ptrAX->SetRect( rectControl );


	CSize	size( 0, 0 );

	sptrIActiveXForm		sptrForm( m_sptrControls );
	if( sptrForm != 0 )
	{
		m_rectForm = NORECT;
		sptrForm->SetSize( CSize( 0, 0 ) );
		sptrForm->AutoUpdateSize();
		sptrForm->GetSize( &size );
		CRect	rectOld = m_rectForm;
		CalcFormRect( size );

		if( m_rectForm != rectOld )
			CWnd::Invalidate();
	}
}

/////////////////////////////////////////////////////////////////////////////
//
//Filter Support
//
/////////////////////////////////////////////////////////////////////////////

bool CBlankView::GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue )
{
	if( !GetActiveField( strTable, strField ) )
		return false;

	IDocument* pDoc = NULL;
	pDoc = GetDocument();
	if( pDoc == NULL )
		return false;

	sptrIDBaseDocument spDBaseDoc(pDoc);
	pDoc->Release();
	if( spDBaseDoc == NULL )
		return false;


	CString strKey;
	strKey.Format( "%s\\%s.%s",
						(LPCTSTR)SECTION_DBASEFIELDS, 
						(LPCTSTR)strTable, (LPCTSTR)strField );						

	sptrINamedData spND( spDBaseDoc );
	if( spND == NULL ) 
		return false;

	_variant_t var;
	spND->GetValue( _bstr_t((LPCSTR)strKey), &var );
	if( var.vt == VT_NULL ) // SBT1382
	{
		strValue.Empty();
		return true;
	}
	if( !(var.vt == VT_R8 || var.vt == VT_BSTR || var.vt == VT_DATE) )
		return false;

	var.ChangeType( VT_BSTR );
	strValue = var.bstrVal;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CBlankView::GetActiveField( CString& strTable, CString& strField )
{
	IDocument* pDoc = NULL;
	pDoc = GetDocument();
	if( pDoc == NULL )
		return false;

	sptrIDBaseDocument spDBaseDoc(pDoc);
	pDoc->Release();
	if( spDBaseDoc == NULL )
		return false;
	
	IUnknown* punkActiveQuery = NULL;
	spDBaseDoc->GetActiveQuery( &punkActiveQuery );
	if( punkActiveQuery == NULL )
		return false;
	

	sptrIQueryObject spQuery( punkActiveQuery );
	punkActiveQuery->Release();

	if( spQuery == NULL )
		return false;

	{
		sptrISelectQuery spSelectQuery( spQuery );
		if( spSelectQuery == NULL )
			return false;

		BOOL bOpen = FALSE;
		spSelectQuery->IsOpen( &bOpen );
		if( !bOpen )
			return false;

		long nRecordCount = -1;
		spSelectQuery->GetRecordCount( &nRecordCount );
		if( nRecordCount < 1 )
			return false;

	}

	sptrIDBControl spDBControl = NULL;

	if( GetBlankViewMode() != bvmPreview )
		return false;

	int nIndex = 0;
	spQuery->GetActiveField( &nIndex );


	BSTR bstrTable, bstrField;
	bstrTable = bstrField = NULL;

	if( S_OK != spQuery->GetField( nIndex, 0, &bstrTable, &bstrField ) )
		return false;


	strTable = bstrTable;
	strField = bstrField;

	if( bstrTable )
		::SysFreeString( bstrTable );
		
	if( bstrField )
		::SysFreeString( bstrField );

	if( !IsAvailableFieldInQuery( spQuery, strTable, strField ) )
		return false;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CBlankView::GetActiveControlIPreviewMode()
{
	CWnd* pWndFocus = GetFocus();	
	CWnd* pWndFocusParent = NULL;

	if( pWndFocus )
	{
		pWndFocusParent = pWndFocus->GetParent();
	}


	POSITION posFind = NULL;

	POSITION pos = m_controlInfos.GetHeadPosition();
	while( pos )
	{
		ControlData *pdata = m_controlInfos.GetNext( pos );
		if( pdata && pdata->pwnd )
		{						
			if( pWndFocus != NULL && pdata->pwnd->GetSafeHwnd() == pWndFocus->GetSafeHwnd() )
			{
				return pdata->ptrCtrl;
			}
			else
			if( pWndFocusParent != NULL && pdata->pwnd->GetSafeHwnd() == pWndFocusParent->GetSafeHwnd() )
			{
				return pdata->ptrCtrl;
			}
		}
	}

	return NULL;

}

BOOL CBlankView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( m_BlankViewMode == bvmPreview )
		return CWnd::OnWndMsg( message, wParam, lParam, pResult );

/*	if( message == ( WM_USER + 777 ) )
	{
		if( ((CAxBaseFrame*)m_pframe)->m_bSelectFrameMode )
		{
			((CAxBaseFrame*)m_pframe)->m_bSelectFrameMode = false;
			m_pframe->Redraw( );	
			((CAxBaseFrame*)m_pframe)->m_bSelectFrameMode = true;
		}
		//ASSERT( false );
	}
	*/

	
	return CAxBaseView::OnWndMsg( message, wParam, lParam, pResult );		
}


/////////////////////////////////////////////////////////////////////////////
BOOL CBlankView::PreTranslateMessage(MSG* pMsg) 
{	
	/*
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F2 )
	{	
		if( OnDBCtrlProp() )
			return true; 
	}
	*/

	/*
	if( pMsg->message == WM_LBUTTONDBLCLK )
	{
		IBlankViewPtr ptrV( GetControllingUnknown() );
		if( ptrV )
		{
			IUnknown* punk = 0;
			ptrV->GetActiveAXData( &punk );
			if( punk )
			{
				punk->Release();	punk = 0;
				
				AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
				ExecuteAction( GetActionNameByResID( IDS_FIELD_PROPERTY ) );	
				return TRUE;
			}
			
		}
	}
	*/

	if( GetBlankViewMode() == bvmDesign )
		return CAxBaseView::PreTranslateMessage(pMsg);	
	else
		return CWnd::PreTranslateMessage(pMsg);
}


/////////////////////////////////////////////////////////////////////////////
//Interface impl
HRESULT CBlankView::XBlankVw::GetMode( short* pnBlankViewMode )
{
	_try_nested(CBlankView, BlankVw, GetMode)
	{	
		*pnBlankViewMode = (short)pThis->GetBlankViewMode();
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CBlankView::XBlankVw::SetMode( short nBlankViewMode )
{
	_try_nested(CBlankView, BlankVw, SetMode)
	{
		pThis->SetBlankViewMode( (BlankViewMode)nBlankViewMode );
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CBlankView::XBlankVw::GetActiveAXData( IUnknown** ppunkAXData )
{
	_try_nested(CBlankView, BlankVw, GetActiveAXData)
	{
		if( !ppunkAXData )
			return E_INVALIDARG;

		IQueryObjectPtr ptrQ( pThis->m_sptrControls );
		if( ptrQ == 0 )
			return false;


		CString strField, strTable;
		BSTR bstrTable, bstrField;
		bstrTable = bstrField = 0;

		if( pThis->m_BlankViewMode == bvmPreview )
		{
			int nIndex = 0;
			ptrQ->GetActiveField( &nIndex );

			if( S_OK != ptrQ->GetField( nIndex, 0, &bstrTable, &bstrField) )
				return S_FALSE;
		}
		else//design mode
		{
		
			if( !pThis->m_pframe )
				return S_FALSE;

			if( !pThis->m_pframe->GetObjectsCount() )
				return S_FALSE;

			IUnknown *punkObject = pThis->m_pframe->GetObjectByIdx( 0 );
			if( !punkObject )
				return S_FALSE;

			IUnknownPtr ptrObject = punkObject;
			punkObject->Release();	punkObject = 0;

			ControlData* pData= pThis->_Get( ptrObject );
			if( !pData )
				return S_FALSE;

			IDBControlPtr ptrCtrl( pData->ptrCtrl );
			if( ptrCtrl == 0 )
				return S_FALSE;

			ptrCtrl->GetTableName( &bstrTable );
			ptrCtrl->GetFieldName( &bstrField );
			
		}

		strField = bstrField;
		strTable = bstrTable;


		if( bstrTable )
			::SysFreeString( bstrTable );	bstrTable = 0;

		if( bstrField )
			::SysFreeString( bstrField );	bstrField = 0;



		POSITION	pos = pThis->m_controlInfos.GetHeadPosition();
		while( pos )
		{
			ControlData *pdata = pThis->m_controlInfos.GetNext( pos );
			if( pdata && pdata->pwnd && pdata->pwnd->GetSafeHwnd() )
			{
				IDBControlPtr ptrCtrl( pdata->ptrCtrl );
				if( ptrCtrl == 0 )
					continue;

				BSTR _bstrTable, _bstrField;
				_bstrTable = _bstrField = 0;

				ptrCtrl->GetTableName( &_bstrTable );
				ptrCtrl->GetFieldName( &_bstrField );

				bool bFound = (strTable == CString(_bstrTable) && strField == CString(_bstrField) );

				if( _bstrTable )
					::SysFreeString( _bstrTable );

				if( _bstrField )
					::SysFreeString( _bstrField );

				if( bFound )
				{
					if( pdata->punkCtrlData )
					{
						pdata->punkCtrlData->AddRef();
						*ppunkAXData = pdata->punkCtrlData;
					}
				}
			}		
		}


		return S_FALSE;
	}
	_catch_nested;

}

/////////////////////////////////////////////////////////////////////////////
HRESULT CBlankView::XClipboardProvider::GetActiveObject( IUnknown** ppunkObject )
{
	_try_nested(CBlankView, ClipboardProvider, GetActiveObject)
	{
		if( pThis->GetBlankViewMode() != bvmPreview )
			return S_FALSE;

		IQueryObjectPtr ptrQ( pThis->m_sptrControls );
		if( ptrQ == 0 )
			return S_FALSE;

		int nfield = 0;
		ptrQ->GetActiveField( &nfield );

		BSTR bstrTableName = 0;
		BSTR bstrFieldName = 0;		

		if( S_OK == ptrQ->GetField( nfield, 0, &bstrTableName, &bstrFieldName ) )
		{
			CString strTable = bstrTableName;
			CString strField = bstrFieldName;

			if( bstrTableName )
				::SysFreeString( bstrTableName );


			if( bstrFieldName  )
				::SysFreeString( bstrFieldName );

			CString strName;
			strName.Format( "%s.%s", (LPCSTR)strTable, (LPCSTR)strField );

			IDocument* pIDoc = pThis->GetDocument();
			if( pIDoc )
			{
				*ppunkObject = ::GetObjectByName( pIDoc, strName, 0 );
				pIDoc->Release();
			}
		}

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	if( GetBlankViewMode() == bvmDesign )
	{
		CAxBaseView::OnActivateView( bActivate, punkOtherView );
		ShowFilterPropPage( false );
	}
	else
	{
		CAxBaseView::OnActivateView( false, punkOtherView );
		ShowFilterPropPage( bActivate );
	}

	if( bActivate )
	{
		IQueryObjectPtr ptrQ( m_sptrControls );
		if( ptrQ )
		{
			int nfield = 0;
			ptrQ->GetActiveField( &nfield );
			ptrQ->SetActiveField( nfield );
		}
	}

	/*
	if( bActivate )
		CreateRecordPaneInfo();
	else
		DestroyRecordPaneInfo();	
		*/

}

/////////////////////////////////////////////////////////////////////////////
bool CBlankView::ShowFilterPropPage( bool bShow )
{
	ShowDBPage( bShow );
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CAxBaseView::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	CAxBaseView::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CBlankView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szTypeBlankCtrl )  )
		return mvFull;
	if( !strcmp(szType, szTypeBlankForm )  )
		return mvFull;
	return mvNone;
}



/////////////////////////////////////////////////////////////////////////////
void CBlankView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if( m_BlankViewMode != bvmPreview )
	{
		CAxBaseView::OnContextMenu( pWnd, point );
		return;
	}

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;
	
	CMenuRegistrator rcm;	

	_bstr_t	bstrMenuName = rcm.GetMenu( szDBBlankMenu, 0 );
	sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
	
}

/////////////////////////////////////////////////////////////////////////////
bool CBlankView::get_object_text( IUnknown* punk_ax, CString& str_text )
{
	ControlData* pdata = _Get( punk_ax );
	if( !pdata )			return false;

	IDBControlPtr ptr_db_ctrl( pdata->ptrCtrl );
	if( ptr_db_ctrl == 0 )	return false;

	IDBaseStructPtr	ptr_db_doc = m_sptrIDocument;
	if( ptr_db_doc == 0 )	return false;

	_bstr_t bstr_table, bstr_field;
	ptr_db_ctrl->GetTableName( &bstr_table.GetBSTR() );
	ptr_db_ctrl->GetFieldName( &bstr_field.GetBSTR() );

	if( !bstr_table.length() || !bstr_field.length() )	return false;

	_bstr_t bstr_user_name;
	if( S_OK != ptr_db_doc->GetFieldInfo( 
							bstr_table, bstr_field, 
							&bstr_user_name.GetBSTR(), 0,
							0, 0,
							0, 0,
							0, 0 ) )
		return false;

	if( !bstr_user_name )	return false;

	str_text = (char*)bstr_user_name;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CBlankView::set_object_text( IUnknown* punk_ax, CString str_text )
{
	if( !str_text.GetLength() )	return false;

	ControlData* pdata = _Get( punk_ax );
	if( !pdata )			return false;

	IDBControlPtr ptr_db_ctrl( pdata->ptrCtrl );
	if( ptr_db_ctrl == 0 )	return false;

	IDBaseStructPtr	ptr_db_doc = m_sptrIDocument;
	if( ptr_db_doc == 0 )	return false;

	_bstr_t bstr_table, bstr_field;
	ptr_db_ctrl->GetTableName( &bstr_table.GetBSTR() );
	ptr_db_ctrl->GetFieldName( &bstr_field.GetBSTR() );

	if( !bstr_table.length() || !bstr_field.length() )	return false;

	short	nfield_type = 0;
	BOOL	brequired = FALSE;
	BOOL	bdef = FALSE;
	_bstr_t bstr_def_value;
	_bstr_t bstr_vt5_type;

	if( S_OK != ptr_db_doc->GetFieldInfo( 
			bstr_table, bstr_field, 
			0, &nfield_type,
			0, 0,
			&brequired, &bdef,
			&bstr_def_value.GetBSTR(), &bstr_vt5_type.GetBSTR()
		) )
		return false;
	
	ptr_db_doc->SetFieldInfo( 
			bstr_table, bstr_field, 
			_bstr_t( (const char*)str_text ),
			brequired, bdef,
			bstr_def_value, bstr_vt5_type );

	::SetModifiedFlagToDoc( m_sptrIDocument );

	return true;
}

void CBlankView::OnSelectObject(IUnknown* punkObject)
{
	CAxBaseView::OnSelectObject( punkObject );

	ControlData* pData=_Get( punkObject );
	if( !pData )
		return;

	IDBControlPtr ptrCtrl( pData->ptrCtrl );
	if( ptrCtrl == 0 )
	{
		// Control which has not field (e.g rectangle frame) was selected.
		// Set active field to null.
		IQueryObjectPtr ptrQ( m_sptrControls );
		ptrQ->SetActiveField(-1);
		return;
	}
	_bstr_t bstrTableName;
	ptrCtrl->GetTableName(bstrTableName.GetAddress());	
	_bstr_t bstrFieldName;
	ptrCtrl->GetFieldName(bstrFieldName.GetAddress());	

	IQueryObjectPtr ptrQ( m_sptrControls );
	int nCount;
	ptrQ->GetFieldsCount(&nCount);
	_bstr_t bsInnerName,bsQueryTable,bsQueryField;
	for(int nIndex=nCount; --nIndex>=0;){
		ptrQ->GetField(nIndex, bsInnerName.GetAddress(), 
			bsQueryTable.GetAddress(), bsQueryField.GetAddress());
		if(bstrTableName==bsQueryTable && bstrFieldName==bsQueryField){
			ptrQ->SetActiveField(nIndex);
			return;
		}
	}
}

LRESULT CBlankView::OnDndOnDragOver(WPARAM wParam, LPARAM lParam)
{
	if( lParam != sizeof(drag_n_drop_data) )
			return 0;
	if (GetValueInt(::GetAppUnknown(), "\\Database\\Blank", "DisableFieldCopy", 0))
	{
		drag_n_drop_data* pdata = (drag_n_drop_data*)wParam;
		pdata->dwDropEfect = DROPEFFECT_MOVE;
		return 1;
	}
	return 0;
}


/*
/////////////////////////////////////////////////////////////////////////////
bool CBlankView::OnDBCtrlProp()
{

	IQueryObjectPtr ptrQ( m_sptrControls );
	if( ptrQ == 0 )
		return false;

	CString strField, strTable;
	BSTR bstrTable, bstrField;
	bstrTable = bstrField = 0;

	if( m_BlankViewMode == bvmPreview )
	{
		int nIndex = 0;
		ptrQ->GetActiveField( &nIndex );

		if( S_OK != ptrQ->GetField( nIndex, 0, &bstrTable, &bstrField) )
			return false;
	}
	else//design mode
	{
	
		if( !m_pframe )
			return false;

		if( !m_pframe->GetObjectsCount() )
			return false;

		IUnknown *punkObject = m_pframe->GetObjectByIdx( 0 );
		if( !punkObject )
			return false;

		IUnknownPtr ptrObject = punkObject;
		punkObject->Release();	punkObject = 0;

		ControlData* pData= _Get( ptrObject );
		if( !pData )
			return false;

		IDBControlPtr ptrCtrl( pData->ptrCtrl );
		if( ptrCtrl == 0 )
			return false;

		ptrCtrl->GetTableName( &bstrTable );
		ptrCtrl->GetFieldName( &bstrField );
		
	}

	strField = bstrField;
	strTable = bstrTable;


	::SysFreeString( bstrTable );	bstrTable = 0;
	::SysFreeString( bstrField );	bstrField = 0;


	CRect rectCtrl = NORECT;


	POSITION	pos = m_controlInfos.GetHeadPosition();
	while( pos )
	{
		ControlData *pdata = m_controlInfos.GetNext( pos );
		if( pdata && pdata->pwnd && pdata->pwnd->GetSafeHwnd() )
		{
			IDBControlPtr ptrCtrl( pdata->ptrCtrl );
			if( ptrCtrl == 0 )
				continue;

			BSTR _bstrTable, _bstrField;
			_bstrTable = _bstrField = 0;

			ptrCtrl->GetTableName( &_bstrTable );
			ptrCtrl->GetFieldName( &_bstrField );

			bool bFound = (strTable == CString(_bstrTable) && strField == CString(_bstrField) );

			::SysFreeString( _bstrTable );
			::SysFreeString( _bstrField );

			if( bFound )
			{
				rectCtrl = GetObjectRect( pdata->punkCtrlData );
				break;
			}
		}		
	}

	if( rectCtrl == NORECT )
		return false;



	CFrameProp* p = new CFrameProp;//delete on CFrameProp::Destroy()

	CString strCaption;	
	{

		
		IDocument* pIDoc = GetDocument();
		p->m_bar.m_ptrDBaseDoc = pIDoc;
		if( pIDoc )
			pIDoc->Release();

		p->m_bar.m_bstrTable = (LPCSTR)strTable;
		p->m_bar.m_bstrField = (LPCSTR)strField;

		strCaption.Format( IDS_FIELD_PROP_ON_PAGE, (LPCSTR)strTable, (LPCSTR)strField  );
	}




	if( !p->Create (NULL, &afxChNil, 
				WS_SYSMENU | WS_THICKFRAME | WS_CAPTION | WS_POPUP | MFS_SYNCACTIVE | MFS_MOVEFRAME ,
                CRect( 0, 0, 300, 300 ),
                this ) )
		return false;


	p->SetWindowText( strCaption );

	CRect rcWnd;
	CRect rcClient;

	p->GetWindowRect( &rcWnd );
	p->GetClientRect( &rcClient );

	CSize sizeBar = p->GetBarSize();

	CPoint pt( rectCtrl.left + rectCtrl.Width() / 2, rectCtrl.top + rectCtrl.Height() / 2 );

	int cx = sizeBar.cx + rcWnd.Width() - rcClient.Width();
	int cy = sizeBar.cy + rcWnd.Height() - rcClient.Height();

	CRect rc( pt.x - cx / 2, pt.y - cy / 2, pt.x + cx / 2, pt.y + cy / 2 );

	ClientToScreen( &rc );

	if( rc.left < 0 )
		rc += CPoint( -rc.left, 0 );

	if( rc.top < 0 )
		rc += CPoint( 0, -rc.top );

	CWnd* pDeskTop = CWnd::GetDesktopWindow();
	CRect rcDesktop;
	pDeskTop->GetWindowRect( &rcDesktop );

	if( rc.right > rcDesktop.right )
		rc -= CPoint( rc.right - rcDesktop.right, 0 );

	if( rc.bottom > rcDesktop.bottom )
		rc -= CPoint( 0, rc.bottom - rcDesktop.bottom );


	p->MoveWindow( &rc );
	p->ShowWindow( SW_SHOW );

	return true;
}




/////////////////////////////////////////////////////////////////////////////
// CDlgBar dialog

CDlgBar::CDlgBar()
{
	//{{AFX_DATA_INIT(CDlgBar)
	m_strCaption = _T("");
	m_strDefValue = _T("");
	m_bHasDefValue = FALSE;
	//}}AFX_DATA_INIT
}


void CDlgBar::DoDataExchange(CDataExchange* pDX)
{
	CDialogBar::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgBar)
	DDX_Text(pDX, IDC_CAPTION, m_strCaption);
	DDX_Text(pDX, IDC_DEF_VALUE, m_strDefValue);
	DDX_Check(pDX, IDC_HAS_DEF_VALUE, m_bHasDefValue);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgBar, CDialogBar)
	//{{AFX_MSG_MAP(CDlgBar)
	ON_BN_CLICKED(IDC_HAS_DEF_VALUE, OnHasDefValue)
	ON_BN_CLICKED(IDC_OK, _OnOk)
	ON_BN_CLICKED(IDC_CANCEL, _OnCancel)
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_EN_CHANGE(IDC_CAPTION, OnChangeCaption)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_INITDIALOG, HandleInitDialog )    
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
LRESULT CDlgBar::HandleInitDialog(WPARAM wp, LPARAM lp)
{
	GetData();

	if( GetDlgItem(IDC_OK) )
	{
		((CButton*)GetDlgItem(IDC_OK))->
			SetIcon(AfxGetApp()->LoadIcon( IDI_OK ) );
	}

	if( GetDlgItem(IDC_CANCEL) )
	{
		((CButton*)GetDlgItem(IDC_CANCEL))->
			SetIcon(AfxGetApp()->LoadIcon( IDI_CANCEL ) );
	}


	LRESULT l = CDialogBar::HandleInitDialog( wp, lp );
	OnHasDefValue();
	return l;
}

/////////////////////////////////////////////////////////////////////////////
bool CDlgBar::GetData()
{
	if( m_ptrDBaseDoc == 0 )
		return false;

	BSTR bstrUserName = 0;
	BOOL bDef = FALSE;
	BSTR bstrDefValue = 0;
	if( S_OK != m_ptrDBaseDoc->GetFieldInfo( 
			m_bstrTable, m_bstrField, 
			&bstrUserName, 0,
			0, 0,
			0, &bDef,
			&bstrDefValue, 0
		) )
		return false;

	m_strCaption = bstrUserName;
	m_bHasDefValue = bDef;
	m_strDefValue = bstrDefValue;

	::SysFreeString( bstrUserName );	bstrUserName = 0;
	::SysFreeString( bstrDefValue );	bstrDefValue = 0;	

	UpdateData( false );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDlgBar::SetData()	
{
	if( m_ptrDBaseDoc == 0 )
		return false;

	UpdateData( true );


	short nFieldType = 0;
	BOOL bRequiredValue = FALSE;
	BSTR bstrVTObjectType = 0;

	if( S_OK != m_ptrDBaseDoc->GetFieldInfo( m_bstrTable, m_bstrField, 
		0, &nFieldType,
		0, &bstrPrimaryFieldName,
		&bRequiredValue, 0,
		0/, &bstrVTObjectType
		) )
		return false;


	HRESULT hRes = m_ptrDBaseDoc->SetFieldInfo( 
			m_bstrTable, m_bstrField, _bstr_t( (LPCSTR)m_strCaption ),
			bRequiredValue, m_bHasDefValue,
			_bstr_t( m_strDefValue ), bstrVTObjectType
		);

	::SysFreeString( bstrVTObjectType );


	::SetModifiedFlagToDoc( m_ptrDBaseDoc );

	return hRes == S_OK;
}

BOOL CDlgBar::PreCreateWindow(CREATESTRUCT& cs) 
{						
	cs.style		|= DS_CONTROL;
	cs.style		|= WS_TABSTOP;
	cs.dwExStyle	|=	WS_EX_CONTROLPARENT;
	return CDialogBar::PreCreateWindow(cs);
}


BOOL CDlgBar::OnInitDialog() 
{
	CDialogBar::OnInitDialog();		
	return TRUE;	            
}

/////////////////////////////////////////////////////////////////////////////
// CDlgBar message handlers
/////////////////////////////////////////////////////////////////////////////
// CFrameProp

CFrameProp::CFrameProp()
{
	m_bDestroyInProgress = false;
}

CFrameProp::~CFrameProp()
{
}


BEGIN_MESSAGE_MAP(CFrameProp, CMiniFrameWnd)
	//{{AFX_MSG_MAP(CFrameProp)
	ON_WM_CREATE()
	ON_WM_KILLFOCUS()
	ON_WM_CANCELMODE()
	ON_WM_ACTIVATE()
	ON_WM_NCDESTROY()
	ON_WM_KEYDOWN()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CFrameProp message handlers

bool bCanKill = false;

int CFrameProp::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_bar.Create( this, CDlgBar::IDD, WS_CHILD|WS_VISIBLE, 54540 );
	CSize size = GetBarSize();

	m_bar.ShowWindow( SW_SHOWNORMAL );		

	m_bar.MoveWindow( 0, 0, size.cx, size.cy );	

	m_bar.SetFocus();

	bCanKill = true;
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
CSize CFrameProp::GetBarSize()
{	
	return m_bar.m_sizeDefault;
}
/////////////////////////////////////////////////////////////////////////////
void CFrameProp::OnKillFocus(CWnd* pNewWnd) 
{
	CWnd::OnKillFocus(pNewWnd);
}

/////////////////////////////////////////////////////////////////////////////
void CFrameProp::OnCancelMode()
{
	destroy_frame();	
}

/////////////////////////////////////////////////////////////////////////////
void CFrameProp::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
 	CMiniFrameWnd::OnActivate(nState, pWndOther, bMinimized);	

	if( nState == WA_INACTIVE && !m_bDestroyInProgress )
		destroy_frame();		
}

void CFrameProp::PostNcDestroy() 
{		
	//delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CFrameProp::destroy_frame()
{
	
	m_bDestroyInProgress = true;
	DestroyWindow();
	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CFrameProp::OnNcDestroy() 
{
	CMiniFrameWnd::OnNcDestroy();
	
	// TODO: Add your message handler code here
	
}

/////////////////////////////////////////////////////////////////////////////
void CFrameProp::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{		
	CMiniFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
	
	if( nChar == VK_RETURN )
	{
		destroy_frame();
	}
	else
	if( nChar == VK_ESCAPE )
	{
		destroy_frame();
	}

}

/////////////////////////////////////////////////////////////////////////////
BOOL CFrameProp::PreTranslateMessage(MSG* pMsg) 
{	
	if( pMsg->message == WM_GETDLGCODE )
	{
		ASSERT( false );
	}

	if( pMsg->message == WM_KEYDOWN )
	{
		ASSERT( false );
	}

	return CMiniFrameWnd::PreTranslateMessage(pMsg);
	
}

LRESULT CFrameProp::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_GETDLGCODE )
	{
		ASSERT( false );
	}

	if( message == WM_KEYDOWN )
	{
		ASSERT( false );
	}
	
	return CMiniFrameWnd::DefWindowProc(message, wParam, lParam);
}

LRESULT CDlgBar::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if( message == WM_GETDLGCODE )
	{
		ASSERT( false );
	}

	if( message == WM_KEYDOWN )
	{
		ASSERT( false );
	}
	
	return CDialogBar::DefWindowProc(message, wParam, lParam);
}

BOOL CDlgBar::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_GETDLGCODE )
	{
		ASSERT( false );
	}

	if( pMsg->message == WM_KEYDOWN )
	{
		ASSERT( false );
	}
	
	return CDialogBar::PreTranslateMessage(pMsg);
}

void CDlgBar::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( ( nChar == VK_RETURN || nChar == VK_ESCAPE ))
	{
		ASSERT( false );
	}
	
	CDialogBar::OnKeyDown(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
void CDlgBar::OnHasDefValue() 
{
	UpdateData( true );
	
	CWnd* pWnd = GetDlgItem( IDC_DEF_VALUE );
	if( !pWnd )	
		return;

	if( m_bHasDefValue )
		pWnd->EnableWindow( true );
	else
		pWnd->EnableWindow( false );
}
			   
/////////////////////////////////////////////////////////////////////////////
void CDlgBar::_OnOk() 
{	
	SetData();		
	((CFrameProp*)GetParent())->destroy_frame();
}

/////////////////////////////////////////////////////////////////////////////
void CDlgBar::_OnCancel() 
{	
	((CFrameProp*)GetParent())->destroy_frame();	
}




void CFrameProp::OnSize(UINT nType, int cx, int cy) 
{
	CMiniFrameWnd::OnSize(nType, cx, cy);		
}

void CDlgBar::OnChangeCaption() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogBar::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

BOOL CFrameProp::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	
	return CMiniFrameWnd::OnNotify(wParam, lParam, pResult);
}

BOOL CDlgBar::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if( IDC_CAPTION == wParam || IDC_CAPTION == lParam )
		ASSERT( false );
	
	return CDialogBar::OnNotify(wParam, lParam, pResult);
}

*/
