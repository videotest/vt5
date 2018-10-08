#include "stdafx.h"
#include "axbaseview.h"
#include "resource.h"
#include "math.h"
#include "dbase.h"
#include <PropBag.h>

const int	cyCaption = 20;

BEGIN_MESSAGE_MAP(CContainerWnd, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CContainerWnd::OnPaint()
{
	{
		CPaintDC	dc( this );
		
		CRect	rect;
		GetClientRect( rect );

		dc.SelectStockObject( BLACK_PEN );
		dc.SelectStockObject( WHITE_BRUSH );
		dc.SelectStockObject( SYSTEM_FONT );
		dc.Rectangle( rect );
		rect.InflateRect( -2, -2 );

		dc.SetBkMode( TRANSPARENT );
		dc.SetTextColor( RGB( 255, 0, 0 ) );

		CString	str;
		str.Format( "Can't create control\nName - %s\nProgID - %s", 
			(const char *)m_strControlName, (const char *)m_strControlProgID );

		dc.DrawText( str, rect, DT_WORD_ELLIPSIS );

		dc.SetBkMode( OPAQUE );
	}
}



#define IDC_EDIT_CTRL	1000

//#include "mfcole\\OCCIMPL.H"

/////////////////////////////////////////////////////////////////////////////
// CEditCtrl

BEGIN_MESSAGE_MAP(CEditCtrl, CEdit)
	//{{AFX_MSG_MAP(CEditCtrl)
	ON_CONTROL_REFLECT(EN_KILLFOCUS, OnKillfocus)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)	
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CEditCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	int nResult = CEdit::OnCreate( lpCreateStruct );		
	SetFont( CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) ) );

	m_x = lpCreateStruct->x;
	m_y = lpCreateStruct->y;

	return nResult;
	
}

void CEditCtrl::UpdateControlSize()
{
	return;
	CString strText;
	GetWindowText( strText );

	if( strText.IsEmpty() )
		strText = "Text";

	int nWidth, nHeight;

	{	
		CClientDC dc(this);
		CFont* pOldFont = (CFont*)dc.SelectObject( 
				CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) ) );

		CRect rcCalc;
		dc.DrawText( strText, &rcCalc , DT_CALCRECT );
		nWidth  = rcCalc.Width();
		nHeight = rcCalc.Height();

		dc.SelectObject( pOldFont );
	}


	CPoint pt = ((CAxBaseView*)GetParent())->ConvertToWindow( CPoint(m_x,m_y) );
	SetWindowPos( NULL, pt.x, pt.y, nWidth+5, nHeight+5, SWP_NOOWNERZORDER );

}				

void CEditCtrl::OnKillfocus()
{
	PostMessage(WM_CLOSE, 0, 0);
}

void CEditCtrl::OnChange()
{
	UpdateControlSize();
}

void CEditCtrl::PostNcDestroy()
{
	
	delete this;	
}

void CEditCtrl::SetCtrlFont( CFont* pFont )
{
	LOGFONT lf;

	if( pFont )
	{
		pFont->GetLogFont( &lf );
		m_Font.CreateFontIndirect( &lf );
		SetFont( &m_Font );
	}

	
}



LRESULT CEditCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{

	if( message == WM_KEYDOWN )
	{		
		if(wParam == VK_RETURN)
		{
			if( !GetAsyncKeyState(VK_CONTROL) )
			{
				CWnd* pParent = GetParent();
				CAxBaseView* pView = (CAxBaseView*)pParent;
				if( pView/*->IsKindOf( RUNTIME_CLASS(CAxBaseView))*/ )
				{
					CString str;
					GetWindowText(str);
					pView->OnEditCtrlChange( str, !m_bEscapeKey );
				}		


				SendMessage(WM_CLOSE, 0, 0);
				return 0;
			}
		}


		if(wParam == VK_ESCAPE)
		{
			PostMessage(WM_CLOSE, 0, 0);
			return m_bEscapeKey = TRUE;
		}		
	}

	
	return CEdit::DefWindowProc( message,  wParam, lParam);

}


/////////////////////////////////////////////////////////////////////////////
// CAxBaseFrame
CAxBaseFrame::CAxBaseFrame()
{
	m_bDragDropFrame = false;
	m_bSelectFrameMode = false;
	m_rectSelect = NORECT;

	m_bUseParentScroll = false;

	m_bUseDesktopDC = false;

}

void CAxBaseFrame::OnBeginDragDrop()
{
	CFrame::OnBeginDragDrop();
	Redraw();
}

void CAxBaseFrame::OnEndDragDrop()
{
	Redraw();
	CFrame::OnEndDragDrop();
}


void CAxBaseFrame::SelectObject(IUnknown* punkObject, bool bAddToExist)
{
	CAxBaseView* pView = NULL;
	if( m_pView )
		pView = (CAxBaseView*)m_pView;	

/*
	if( pView )
	{
		int nNumObjs = GetObjectsCount();
		for(int i = 0; i < nNumObjs; i++)
		{
			IUnknown *punk = GetObjectByIdx( i );
			pView->UpdateContainerWnd( punk );
			punk->Release();
		}			
	}
	*/

	CFrame::SelectObject( punkObject, bAddToExist );

	if( pView ) pView->OnSelectObject( punkObject );

	if (!punkObject)
	{
		Redraw();
	}	

}



void CAxBaseFrame::Redraw( bool bOnlyPoints, bool bUpdate )
{

	if( (m_bDragDropActive && m_bDragDropFrame) ||
		(m_bInTrackMode && !m_bSelectFrameMode ) )
	{		
		CClientDC dc( 0 );
		Draw( dc , NORECT, 0, 0 );
	}	
	else
	{			
		//paul 7.09.2001. Reason raise not pair Redraw. 
		//CFrame::Redraw( bOnlyPoints, bUpdate );
	}
		
}

POSITION CAxBaseFrame::GetFirstObjectPosition()
{
	ASSERT( m_pView );
	//ASSERT( m_pView->IsKindOf( RUNTIME_CLASS( CAxBaseView ) ) );

	return ((CAxBaseView*)m_pView)->GetControls().GetFirstObjectPosition();
}

IUnknown *CAxBaseFrame::GetNextObject( POSITION &rPos )
{
	ASSERT( m_pView );
	//ASSERT( m_pView->IsKindOf( RUNTIME_CLASS( CAxBaseView ) ) );

	return ((CAxBaseView*)m_pView)->GetControls().GetNextObject( rPos );
}


void CAxBaseFrame::Draw(CDC& dc, CRect rectPaint, BITMAPINFOHEADER *pbih, byte *pdibBits )
{	

	ASSERT( m_pView );
	if( !m_bDragDropActive && m_bDragDropFrame )
		return;

	if( m_bDragDropActive && m_bDragDropFrame )
	{
		//-----------------------
		CClientDC	dcDesktop( m_bUseDesktopDC ? 0 : (CAxBaseView*)m_pView );

		IUnknown *punkObject = 0;
		int nNumObjs = GetObjectsCount();

		for(int i = 0; i < nNumObjs; i++)
		{
			punkObject = GetObjectByIdx( i );
			if( punkObject )
			{

				IActiveXCtrlPtr	ptrControl( punkObject );
				if( ptrControl )
				{
					punkObject->Release();

					CRect	rect;
					
					ptrControl->GetRect( &rect );
					//rect += CPoint( 5, 5 );

					//Scroll support
					ASSERT( m_pView );						
					
					rect = ((CAxBaseView*)m_pView)->ConvertToWindow( rect );

					//m_pView->ClientToScreen( &rect );			

					//+++++++++++++++++++++++++++++
					//CPoint	ptScroll = ::GetScrollPos( m_pView->GetControllingUnknown() );
					CPoint	ptScroll = ((CAxBaseView*)m_pView)->GetScrollPos( );
					
					dc.SetBrushOrg( ptScroll.x%2, ptScroll.y %2 );
					dcDesktop.SetROP2( R2_XORPEN );
					dcDesktop.DrawFocusRect( ConvertToDeskTop( rect ) );
				}
			}
		}


	}
	else
	if( m_bInTrackMode&&!m_bSelectFrameMode )
	{
		ASSERT( !m_bDragDropActive );
		ASSERT( !m_bDragDropFrame );
		//-------------------------
		CClientDC	dc( m_bUseDesktopDC ? 0 : (CAxBaseView*)m_pView );

		CRect	rect( m_rectCurrent );
		rect.NormalizeRect();
		
		rect.DeflateRect(5,5);		

		//Scroll support
		ASSERT( m_pView );		

		
		rect = ((CAxBaseView*)m_pView)->ConvertToWindow( rect );
		//m_pView->ClientToScreen( rect );
		
		//+++++++++++++++++++++++++++++
		//CPoint	ptScroll = ::GetScrollPos( m_pView->GetControllingUnknown() );
		CPoint	ptScroll = ((CAxBaseView*)m_pView)->GetScrollPos( );
		dc.SetBrushOrg( ptScroll.x%2, ptScroll.y %2 );
		dc.SetROP2( R2_XORPEN );
		dc.DrawFocusRect( ConvertToDeskTop( rect, false ) );
	}
	else
	{
		int nNumObjs = GetObjectsCount();
		for(int i = 0; i < nNumObjs; i++)
		{
			IUnknown *punkObject = GetObjectByIdx( i );
			CRect rc = m_pView->GetObjectRect(punkObject);
			punkObject->Release();
			m_pView->DoDrawFrameObjects(punkObject, dc, rectPaint, pbih, pdibBits, (i==0)?odsActive:odsSelected );
		}
	}

	if( m_bSelectFrameMode )
	{
		//+++++++++++++++++++++++++++++		
		CPoint	ptScroll = ((CAxBaseView*)m_pView)->GetScrollPos( );
		dc.SetBrushOrg( ptScroll.x%2, ptScroll.y %2 );

		int	nR2 = dc.SetROP2( R2_XORPEN );
		dc.SelectStockObject( BLACK_PEN );

		dc.SetBkColor( RGB( 255, 255, 255 ) );
		CRect	rect = m_rectSelect;
		rect = ((CAxBaseView*)m_pView)->ConvertToWindow( rect );		
		
		
		dc.SetBrushOrg( ptScroll.x%2, ptScroll.y %2 );
		dc.DrawFocusRect( ConvertToDeskTop( rect, false ) );
		
		dc.SetROP2( nR2 );
	}

}

bool CAxBaseFrame::DoEnterMode(CPoint point)
{	//Scroll support//was
	if( m_bSelectFrameMode  )
	{
		point = ((CAxBaseView*)m_pView)->ConvertToClient( point );
		return CMouseImpl::DoEnterMode( point );
	}

	return CFrame::DoEnterMode( point );
}

bool CAxBaseFrame::DoStartTracking( CPoint pt )
{	
	//Scroll support
	//pt = ::ConvertToWindow( m_pView->GetControllingUnknown(), pt );
	if( m_bSelectFrameMode  )
	{
		
		RedrawSelectRect();
		return true;
	}
	return CFrame::DoStartTracking( pt );
}

bool CAxBaseFrame::DoTrack( CPoint pt )
{	

	if( m_bSelectFrameMode  )
	{
		RedrawSelectRect( );			

		m_rectSelect.top = min( m_pointFirstPoint.y, pt.y );
		m_rectSelect.left = min( m_pointFirstPoint.x, pt.x );
		m_rectSelect.bottom = max( m_pointFirstPoint.y, pt.y );
		m_rectSelect.right = max( m_pointFirstPoint.x, pt.x );

		RedrawSelectRect();

		
		if( m_pView )
		{
			CAxBaseView* pView = (CAxBaseView*)m_pView;
			pView->OnFrameTrack( pt );
		}
		

		return false;
	}
	
	return CFrame::DoTrack( pt );
	
}

bool CAxBaseFrame::DoFinishTracking( CPoint pt )
{	

	if( m_bSelectFrameMode  )
	{
		if( m_pView )
		{
			CAxBaseView* pView = (CAxBaseView*)m_pView;
			pView->OnFrameFinishTrack( pt );
		}

		RedrawSelectRect();
		//select objects
		EmptyFrame();

		POSITION	pos = GetFirstObjectPosition();

		while( pos )
		{
			IUnknown	*punkObject = GetNextObject( pos );
			IActiveXCtrlPtr	ptrAX( punkObject );

			CRect rect = m_pView->GetObjectRect( ptrAX );


			if( m_rectSelect.PtInRect( rect.TopLeft() ) &&
				m_rectSelect.PtInRect( rect.BottomRight() ) )
			{
				SelectObject( punkObject, true );
			}

			punkObject->Release();
		}

		m_pView->InvalidateRect( GetBoundRect() );
		m_bSelectFrameMode  = false;


		m_rectSelect = NORECT;
		return false;
	}
	return CFrame::DoFinishTracking( pt );
}

bool CAxBaseFrame::DoLButtonUp( CPoint pt )
{
	m_bSelectFrameMode  = false;
	if( m_bDragDropFrame )
		return false;
	if( (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0 )
	{
		//m_pView->PostMessage( WM_KEYDOWN, 13 );
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		::ExecuteAction( GetActionNameByResID( IDS_ACTION_PROPERTIES ) );
	}
	return true;
}

bool CAxBaseFrame::DoLButtonDown( CPoint pt )
{
	if( CResizingFrame::HitTest( pt ) != CResizingFrame::None )
		return false;

	CAxBaseView* pView = NULL;
	if( m_pView )
		pView = (CAxBaseView*)m_pView;

	if( pView ) pView->BeforLButtonDown( pt );

	bool	bRet = CFrame::DoLButtonDown( pt );

	if( pView ) 
		pView->AfterLButtonDown( pt );

	if( m_bDragDropFrame )
		return false;
	if( bRet )
		return true;

	bool	bSelectControls = false;

	int nNumObjs = GetObjectsCount();
	for(int i = 0; i < nNumObjs; i++)
	{
		IUnknown *punkObject = GetObjectByIdx( i );
		if( CheckInterface( punkObject, IID_IActiveXCtrl ) )
			bSelectControls = true;

		if( punkObject )punkObject->Release();
	}


	if( !bSelectControls )
	{
		m_rectSelect.top = pt.y;
		m_rectSelect.left = pt.x;
		m_rectSelect.bottom = pt.y;
		m_rectSelect.right = pt.x;
		m_pointFirstPoint = pt;

		m_bSelectFrameMode  = true;		

		
		//StartTracking( pt );
		return false;
	}

	return false;
}


void CAxBaseFrame::RedrawSelectRect( )
{	
	
	//m_pView->UpdateWindow();
	CClientDC	dc( m_bUseDesktopDC ? 0 : (CAxBaseView*)m_pView );

	CRect	rect = m_rectSelect;

	rect = ((CAxBaseView*)m_pView)->ConvertToWindow( rect );
 

	dc.SelectStockObject( BLACK_PEN );
	dc.SetBkColor( RGB( 255, 255, 255 ) );

	//CPoint ptScroll = ::GetScrollPos( m_pView->GetControllingUnknown() );
	CPoint	ptScroll = ((CAxBaseView*)m_pView)->GetScrollPos( );
	dc.SetBrushOrg( ptScroll.x%2, ptScroll.y %2 );

	dc.DrawFocusRect( ConvertToDeskTop( rect, false ) );
}



CPoint CAxBaseFrame::CalcPointLocation( PointLocation location, double &fAngle )
{	

	CPoint	pointCenter = m_rect.CenterPoint();
	CRect	rect;
	rect.left = int( pointCenter.x - m_rect.Width()*m_fZoomX/2+.5);
	rect.right = int( pointCenter.x + m_rect.Width()*m_fZoomX/2+.5);
	rect.top = int( pointCenter.y - m_rect.Height()*m_fZoomY/2+.5);
	rect.bottom = int( pointCenter.y + m_rect.Height()*m_fZoomY/2+.5);
	//get the angle

	fAngle = 0;	
	
	if( rect.left >= rect.right || rect.top >= rect.bottom )
	{
		//return CPoint( -1, -1 );		
	}
		

	fAngle = ::atan2( (double)rect.Height(), (double)rect.Width() );
	double	dx = rect.Width()/2;
	double	dy = rect.Height()/2;
	double	delta = ::sqrt( dx*dx+dy*dy );

	CPoint	pointWork;

	if( location == Left )
	{
		fAngle = m_fAngle+A180;
		delta = rect.Width()/2;
		pointWork = CPoint( m_rectCurrent.left, m_rectCurrent.CenterPoint().y );
	}
	else
	if( location == Bottom )
	{
		fAngle = m_fAngle+A90;
		delta = rect.Height()/2;
		pointWork = CPoint( m_rectCurrent.CenterPoint().x, m_rectCurrent.bottom );
	}
	else
	if( location == Right )
	{
		fAngle = m_fAngle;
		delta = rect.Width()/2;
		pointWork = CPoint( m_rectCurrent.right, m_rectCurrent.CenterPoint().y );
	}
	else
	if( location == Top )
	{
		fAngle = m_fAngle+A270;
		delta = rect.Height()/2;
		pointWork = CPoint( m_rectCurrent.CenterPoint().x, m_rectCurrent.top );
	}
	else
	if( location == TopRight )
	{
		fAngle = m_fAngle-fAngle;
		pointWork = CPoint( m_rectCurrent.right, m_rectCurrent.top );
	}
	else
	if( location == BottomRight )
	{
		fAngle = m_fAngle+fAngle;
		pointWork = m_rectCurrent.BottomRight();
	}
	else
	if( location == TopLeft )
	{
		fAngle = m_fAngle+fAngle+A180;
		pointWork = m_rectCurrent.TopLeft();
		
	}
	else
	if( location == BottomLeft )
	{
		fAngle = m_fAngle-fAngle+A180;
		pointWork = CPoint( m_rectCurrent.left, m_rectCurrent.bottom );
	}
	else
		ASSERT( FALSE );

	if( m_bRotate )
	{
		pointWork.x = int( pointCenter.x+delta*::cos( fAngle )+.5 );
		pointWork.y = int( pointCenter.y+delta*::sin( fAngle )+.5 );
	}
	
	if( ( m_dwFlags & fcfRotate ) == 0 )
	{
		if( location == TopRight ||location == TopLeft||
			location == BottomLeft ||location == BottomRight )
			fAngle += A90;
	}
	return pointWork;

}


/////////////////////////////////////////////////////////////////////////////
// CAxBaseView

IMPLEMENT_DYNCREATE(CAxBaseView, CWnd);

IMPLEMENT_DYNCREATE(CAxBaseFrame, CFrame);


BEGIN_INTERFACE_MAP(CAxBaseView, CViewBase)
	INTERFACE_PART(CAxBaseView, IID_ILayoutView, LayoutVw)
	INTERFACE_PART(CAxBaseView, IID_IMenuInitializer, EditorMenu)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CAxBaseView, LayoutVw);

BEGIN_MESSAGE_MAP(CAxBaseView, CViewBase)
	//{{AFX_MSG_MAP(CAxBaseView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//ON_WM_KEYDOWN()

HRESULT CAxBaseView::XLayoutVw::GetFormOffset( POINT *ppoint )
{
	METHOD_PROLOGUE_EX(CAxBaseView, LayoutVw);
	ppoint->x = pThis->Offset().x;
	ppoint->y = pThis->Offset().y;
	return S_OK;
}

HRESULT CAxBaseView::XLayoutVw::GetControlObject( IUnknown *punkAXCtrl, IUnknown **ppAX )
{
	_try_nested(CAxBaseView, LayoutVw, GetControlObject)
	{
		ControlData	*pdata = pThis->_Get( punkAXCtrl );

		if( !pdata )
		{
			IUnknown* punk;
			if( punkAXCtrl )
			{
				if(FAILED(punkAXCtrl->QueryInterface(IID_IReportCtrl, (void**)&punk)))
				{										
					return S_FALSE;
				}

				punkAXCtrl->Release();

				pdata = pThis->_Get( punk );
				if( !pdata || pdata->ptrCtrl==0 ){
					return S_FALSE;
				}


			}			
			
		}
		*ppAX = pdata->ptrCtrl;
		if (*ppAX)
			(*ppAX)->AddRef();
		return S_OK;
	}
	_catch_nested;
}




IMPLEMENT_UNKNOWN(CAxBaseView, EditorMenu);
HRESULT CAxBaseView::XEditorMenu::OnInitPopup(BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu)
{
	_try_nested(CAxBaseView, EditorMenu, OnInitPopup)
	{
		/*
		sptrIBlankView spView( pThis->GetControllingUnknown() );
		if( spView )
		{
			short nMode;
			spView->GetMode( &nMode );
			if( (BlankViewMode)nMode == bvmFilter )
			{
				return E_FAIL;
			}				
		}
		*/

		if (!hMenu)
			return E_INVALIDARG;

		*phOutMenu = ::CopyMenu(hMenu);
		if (!*phOutMenu)
			return S_FALSE;
		// get menu
		CMenu* pmenu = CMenu::FromHandle(*phOutMenu);
		if (!pmenu)
			return S_FALSE;

		// get command Manager
		IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager2 );

		ASSERT(punk != NULL);
		if (!punk)
			return S_FALSE;

		sptrICommandManager2 sptrCmdMgr(punk);
		punk->Release();
		



		bool bCanDelete = pThis->m_pframe->GetObjectsCount() != 0;


		CString strMenuName;
		BSTR bstrName = 0;

		int nCount = (int)pmenu->GetMenuItemCount();
		for (int i = 0; i < nCount; i++)
		{
			UINT uFlag = MF_BYPOSITION | MF_STRING;
			UINT uItem = pmenu->GetMenuItemID(i);
			bool bChange = true;

			if (!SUCCEEDED(sptrCmdMgr->GetActionName(&bstrName, uItem)))
				continue;
			
			CString strActionName = bstrName;
			::SysFreeString(bstrName);

			AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

			// return action name by Resource ID
			if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_INSERT_CONTROL)))
				strMenuName = "Insert new control";

			else if (0 == lstrcmp(strActionName, GetActionNameByResID(IDS_ACTION_ADD_EVENT_HANDLERS)))
				strMenuName = "Add event handlers";

			else if (0 == lstrcmp(strActionName, "ShowScript"))
				strMenuName = "Edit script";

			else if (0 == lstrcmp(strActionName, "DeleteObject") && bCanDelete)
				strMenuName = "Delete selected controls";

			else
				bChange = false;

			if (bChange)
				pmenu->ModifyMenu(i, uFlag, uItem, strMenuName);
		}

		*phOutMenu = pmenu->Detach();

		return S_OK;
	}
	_catch_nested;
}

void CAxBaseView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	CRect rc;

	::GetCursorPos(&point);

	// get command Manager
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager2 );

	ASSERT(punk != NULL);
	if (!punk)
		return;

	sptrICommandManager2 sptrCmdMgr(punk);
	punk->Release();


	UINT uCMD;
	_bstr_t	bstrMenuName = _T("EditorMenu");

	// register context menu initializer
	sptrCmdMgr->RegisterMenuCreator((IUnknown*)&(this->m_xEditorMenu));

	// execute context menu (return value is action's ID which selected by user)
	sptrCmdMgr->ExecuteContextMenu3(bstrMenuName, point, TPM_RETURNCMD, &uCMD);

	sptrCmdMgr->UnRegisterMenuCreator((IUnknown*)&(this->m_xEditorMenu));

	// set selected action arguments
	bool bExec = true;

	BSTR	bstrActionName;
	CString strArgs = "";
	DWORD	dwFlag = 0;

	if (!SUCCEEDED(sptrCmdMgr->GetActionName(&bstrActionName, uCMD)))
		return;
	
	CString strActionName = bstrActionName;
	::SysFreeString(bstrActionName);


	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction(strActionName, strArgs, dwFlag);
}






CAxBaseView::CAxBaseView()
{
	_OleLockApp( this );

	EnableAggregation();
	m_pointFormOffset = CPoint( 10, 10 );
	CalcFormRect( CSize(0, 0));

	CreateFrame();	
	

	m_bCanResizeForm = false;
	m_bDrawFrame = true;

	m_bObjectSelectProcess	= false;
	m_bRemoveCtrlInProgress	= false;	
	m_bCangeCtrlInProgress	= false;


	AFX_MANAGE_STATE( AfxGetStaticModuleState() ); 
}

CAxBaseView::~CAxBaseView()
{
	TRACE( "CAxBaseView::~CAxBaseView()\n" );		
	
	_OleUnlockApp( this );
}

void CAxBaseView::CreateFrame()
{
	m_pframeRuntime = RUNTIME_CLASS(CAxBaseFrame);	
}


void CAxBaseView::AttachActiveObjects()
{
	if( !GetSafeHwnd() )
		return;																		 //szTypeAXForm
	IUnknownPtr	sptrControls( ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeReportForm ), false );

	if( GetObjectKey( m_sptrControls ) == GetObjectKey( sptrControls ) )
		return;
	
	if( GetSafeHwnd() )
		_RemoveAllControls();

	
	m_pframe->UnselectObjectsWithSpecifiedParent( m_sptrControls );
	m_sptrControls.Attach( sptrControls );

	_UpdateLayout();
	_AddAllControls();	

	Invalidate();
}

bool CAxBaseView::ShowInsertCtrlSheet( bool bShow )
{
	
	CString strInsPage;
		
	IUnknown	*punkPropertySheet = ::GetPropertySheet();

	//
	

	if( punkPropertySheet )
	{			
		IPropertySheetPtr	spPSheet( punkPropertySheet );
		punkPropertySheet->Release();

		int nPagesCount = 0;
		spPSheet->GetPagesCount( &nPagesCount );

		if( spPSheet )
		{
			for( long nPageNo = nPagesCount-1; nPageNo >-1; nPageNo-- )
			{
				IUnknown *punkPage = NULL;
				spPSheet->GetPage( nPageNo, &punkPage );
				if( punkPage )
				{
					sptrIAXInsertPropPage spInsPP( punkPage );
					if( spInsPP )
					{
						CString	str = GetObjectName( punkPage );
						if( bShow )
						{
							int nActivePage = -1;
							spPSheet->GetActivePage( &nActivePage );

							spPSheet->IncludePage( nPageNo );
							sptrIAXInsertPropPage spAXPropPage( punkPage );
							if( spAXPropPage )
								spAXPropPage->OnActivateView();
						

						}
						else
						{
							spPSheet->ExcludePage( nPageNo );
						}


					}
					punkPage->Release();
				}
			}				
			
		}
	}				

	return true;
}

void CAxBaseView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CViewBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );

	if( !strcmp( pszEvent, szEventChangeObject ) )
	{
		if( GetObjectKey( punkFrom ) != GetObjectKey( m_sptrControls ) )
			return;
		_UpdateLayout();
		UpdateSelectionRect();
		Invalidate();
		
	}
	if( !strcmp( pszEvent, szEventChangeObjectList ) )
	{
		if( GetObjectKey( punkFrom ) != GetObjectKey( m_sptrControls ) )
			return;
		IUnknown	*punkObject = punkHit;
		long	nAction = *(long*)pdata;



		if( nAction == cncAdd )
		{
			_AddControl( punkObject );

			if(!m_pframe->m_bDragDropActive  )
				m_pframe->SelectObject(punkHit, false);

			UpdateSelectionRect();
		}
		else if( nAction == cncRemove )
		{
			_RemoveControl( punkObject );
			UpdateSelectionRect();
		}
		else if( nAction == cncChange )
		{
	int nControls = m_sptrControls.GetCount();
			m_bCangeCtrlInProgress = true;
			m_bRemoveCtrlInProgress = true;
			_RemoveControl( punkObject );
			m_bRemoveCtrlInProgress = false;
	nControls = m_sptrControls.GetCount();
			_AddControl( punkObject );
			m_bCangeCtrlInProgress = false;
	nControls = m_sptrControls.GetCount();

			//paul 24.04.2003 BT 3081
			IMultiSelectionPtr ptr_ms( m_pframe->GetControllingUnknown() );
			if( ptr_ms )
				ptr_ms->SyncObjectsRect();


			HWND	hwnd = ::GetFocus();
			if( _IsControlWindow( hwnd ) ) 
				SetFocus();
	nControls = m_sptrControls.GetCount();
			//UpdateSelectionRect();
		}
		else if( nAction == cncReset )
		{
			_RemoveAllControls();
			_AddAllControls();
			//UpdateSelectionRect();
		}
		else if( nAction == cncActivate )
		{
			//do nothing now
			if(!m_pframe->m_bDragDropActive  )
				m_pframe->SelectObject(punkHit, false);
		}


		int nObjectCount = m_pframe->GetObjectsCount();
		if( m_bCanResizeForm && nObjectCount < 1 )
		{
			//AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
			//::ExecuteAction( GetActionNameByResID( IDS_ACTION_RESIZESIZE ) );	
		}
		else
		{
			//GetActionManager
				//IActionManager
		}
			


		
	}

}

//implementation details
void CAxBaseView::_AddControl( IUnknown *punkControl )
{
	DWORD	dwStyle;
	UINT	nID;
	CRect	rect;
	

	BSTR	bstrProgID = 0;
	sptrIActiveXCtrl	sptrC( punkControl );
	sptrC->GetProgID( &bstrProgID );
	sptrC->GetRect( &rect );
	sptrC->GetStyle( &dwStyle );
	sptrC->GetControlID( &nID );

	if( nID == -1 )
		nID = _FindUniqueID( 1000 );

	sptrC->SetControlID( nID );
	
	{
		int nmin_width	= ::GetValueInt( ::GetAppUnknown(), "\\AXEditor", "MinCtrlWidth", 2 );
		int nmin_height	= ::GetValueInt( ::GetAppUnknown(), "\\AXEditor", "MinCtrlHeight", 2 );
		int nwidth	= rect.Width();
		int nheight	= rect.Height();
		if( rect.left + nwidth <= 0  )
			rect.left = 5;
		if( rect.right + nheight <= 0  )
			rect.right = 5;
		if( nwidth <= nmin_width )
			rect.right = rect.left + nmin_width;
		if( nheight <= nmin_height )
			rect.bottom = rect.top + nmin_width;		
	}

	rect+= Offset();

	CString strProgID = bstrProgID;
	::SysFreeString( bstrProgID );

	CString	strName = ::GetObjectName( punkControl );



	if( !IsVisibleCtrlWnd() )
		dwStyle &= ~WS_VISIBLE;


	CContainerWnd	*pwnd = new CContainerWnd();

	bool	bCreated = false;
	try
	{
		bCreated = pwnd->CreateControl( strProgID, 0, dwStyle, rect, this, nID )==TRUE;
		
	}
	catch( CException *pe )
	{
		pe->ReportError();
		pe->Delete();
		bCreated = false;
	}

	/*if( !bCreated )
	{
		delete pwnd;
		return;
	}*/

	ControlData	*pdata = new ControlData;
	pdata->punkCtrlData = punkControl;
	pdata->pwnd = pwnd;
	pdata->ptrCtrl = pdata->pwnd->GetControlUnknown();
	pdata->nID = nID;
	
	pwnd->m_strControlProgID = strProgID;

	unsigned long ViewSubType=0;

	if( !pwnd->GetControlUnknown() )
	{
		pwnd->Create( "static", strName, WS_CHILD|WS_VISIBLE, rect, this, nID );
	}
	else
	{
	}

	IVtActiveXCtrl2Ptr	sptrVTC(pdata->ptrCtrl);
	if(sptrVTC != 0)
		sptrVTC->SetApp(GetAppUnknown());


	if(IViewSubTypePtr pViewSub=punkControl)
	{
		if(SUCCEEDED(pViewSub->GetViewSubType(&ViewSubType)))
		{
		if(ViewSubType>0)
		{
				if(IViewSubTypePtr pControlPropBag=pwnd->GetControlUnknown())
			{
					pControlPropBag->SetViewSubType(ViewSubType);
				}
			}
		}
	}

	::RestoreContainerFromDataObject( punkControl, pdata->ptrCtrl );

	m_controlInfos.AddTail( pdata );

	pwnd->MoveWindow( rect );
	UpdateControlRect( punkControl );
	
	SetFocus();

	m_ptrPrevSelectedObject = punkControl;
}

void CAxBaseView::_RemoveControl( IUnknown *punkControl )
{
	POSITION	pos = _Find( punkControl );

	if( !pos )
	{
		ASSERT( FALSE );
		return;
	}

	if( m_ptrPrevSelectedObject!=NULL && punkControl == m_ptrPrevSelectedObject )
		m_ptrPrevSelectedObject = NULL;

	ControlData *pdata = m_controlInfos.GetAt( pos );

	CRect	rectInvalidate = GetObjectRect( punkControl );

	if( pdata->pwnd->GetSafeHwnd() )
		pdata->pwnd->DestroyWindow();

	delete pdata->pwnd;
	delete pdata;

	m_controlInfos.RemoveAt( pos );

	//m_pframe->Redraw();
	InvalidateRect( rectInvalidate );

	
}

void CAxBaseView::_AddAllControls()
{
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();

	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		_AddControl( punk );
		punk->Release();
	}
}

void CAxBaseView::_RemoveAllControls()
{
	while( POSITION pos = m_controlInfos.GetHeadPosition() )
	{
		ControlData *pdata = m_controlInfos.GetAt( pos );
		if( pdata->pwnd->GetSafeHwnd() )
			pdata->pwnd->DestroyWindow();
		delete pdata->pwnd;
		delete pdata;

		m_controlInfos.RemoveAt( pos );
	}
	m_pframe->EmptyFrame();
}									 
							
POSITION	CAxBaseView::_Find( IUnknown *punkControl )
{
	POSITION	pos = m_controlInfos.GetHeadPosition();

	while( pos )
	{
		POSITION	posSave = pos;
		ControlData *pdata = m_controlInfos.GetNext( pos );
		
		//if( pdata->punkCtrlData == punkControl )
		//	return posSave;

		//not correct code cos after drag drop keys are indifical
		if( GetObjectKey(pdata->punkCtrlData) == GetObjectKey(punkControl) )
			return posSave;
	}

	return 0;
}

POSITION	CAxBaseView::_Find( CWnd *pwnd )
{
	POSITION	pos = m_controlInfos.GetHeadPosition();

	while( pos )
	{
		POSITION	posSave = pos;
		ControlData *pdata = m_controlInfos.GetNext( pos );

		if( pdata->pwnd == pwnd )
			return posSave;
	}
	return 0;
}

CAxBaseView::ControlData
	*CAxBaseView::_Get( IUnknown *punkControl )
{
	POSITION	posFound = _Find( punkControl );
	if( !posFound )
		return 0;
	return m_controlInfos.GetAt( posFound );
}

CAxBaseView::ControlData
	*CAxBaseView::_Get( CWnd *pwnd )
{
	POSITION	posFound = _Find( pwnd );
	if( !posFound )
		return 0;
	return m_controlInfos.GetAt( posFound );
}


UINT CAxBaseView::_FindUniqueID( UINT nID )
{
	bool	bFound = true;


	bFound = false;
	POSITION	pos = m_controlInfos.GetHeadPosition();

	while( pos )
	{
		POSITION	posSave = pos;
		ControlData *pdata = m_controlInfos.GetNext( pos );

		if( pdata->nID == nID )
		{
			bFound = true;
			break;
		}
	}



	while( bFound )
	{
		nID++;
		bFound = false;
		POSITION	pos = m_controlInfos.GetHeadPosition();

		while( pos )
		{
			POSITION	posSave = pos;
			ControlData *pdata = m_controlInfos.GetNext( pos );

			if( pdata->nID == nID )
			{
				bFound = true;
				break;
			}
		}
	}
	return nID;
}


//////////////////////////////////////////////////////////////////////////////////////////
//message handlers
int CAxBaseView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	sptrIScrollZoomSite	sptrZS( GetControllingUnknown() );
	sptrZS->SetAutoScrollMode( false );

	AttachActiveObjects();

	_UpdateLayout( false );

	m_pframe->SetFlags( fcfResize );
	((CAxBaseFrame*)m_pframeDrag)->m_bDragDropFrame = true;

	//CreateCtrlPane();
	
	return 0;
}

void CAxBaseView::OnDestroy() 
{
	_RemoveAllControls();
	CViewBase::OnDestroy();
}


void CAxBaseView::OnPaint() 
{

	sptrIScrollZoomSite sptrSZS( GetControllingUnknown() );
	if( sptrSZS ) sptrSZS->SetZoom( 1.0 );


	_UpdateLayout( false );
	CRect	rectPaint = NORECT;
	GetUpdateRect( &rectPaint );


	CPaintDC dc(this);	

	dc.SaveDC();

	if( !m_bDrawFrame )
	{
		dc.FillRect( m_rectForm, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );	
		dc.DrawEdge( m_rectForm, BDR_RAISEDINNER, BF_RECT );			
	}
	else
	{
		CString	str = "no form";

		IActiveXFormPtr		ptrForm( m_sptrControls );
		if( ptrForm )
		{
			BSTR	bstr = 0;
			ptrForm->GetTitle( &bstr );
			str = bstr;
			::SysFreeString( bstr );
		}

		dc.FillRect( m_rectForm, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );	
		dc.DrawEdge( m_rectForm, BDR_RAISEDINNER, BF_RECT );
		dc.FillRect( m_rectCaption, &CBrush( ::GetSysColor( COLOR_INACTIVECAPTION ) ) );	
		dc.DrawEdge( m_rectCaption, BDR_SUNKENINNER, BF_RECT );

		CFont	font;
		font.CreateFont( cyCaption-4, 0, 0, 0, FW_BOLD, 0, 0, 0, 1, 0, 0, 0, 0, "Arial" );
		
		CFont	*pold = 
		dc.SelectObject( &font );
		dc.SetBkMode( TRANSPARENT );
		dc.SetTextColor( ::GetSysColor( COLOR_INACTIVECAPTIONTEXT ) );
		dc.DrawText( str, m_rectCaption, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

		dc.SelectObject( pold );
		dc.SetBkMode( OPAQUE );
	}

	dc.RestoreDC( -1 );

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
				CRect rcCtrl;
				pdata->pwnd->GetWindowRect( &rcCtrl );
				ScreenToClient( &rcCtrl );

				CRect rcInt;
				if( rcInt.IntersectRect( &rectPaint, &rcCtrl ) )
				{
					pdata->pwnd->Invalidate();
					pdata->pwnd->UpdateWindow();
				}
			}
		}
	}
	
	
	m_pframe->Draw( dc, NORECT, 0, 0 );
	if( m_pframe->m_bDragDropActive )
		m_pframeDrag->Draw( dc, NORECT, 0, 0 );
		

	/*
	{	
		CPaintDC dc(this);	
		
		CDC* pDC = GetDC();
		if( pDC )
		{	
			pDC->FillRect( m_rectForm, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );	
			pDC->DrawEdge( m_rectForm, BDR_RAISEDINNER, BF_RECT );			
			ReleaseDC(pDC);
			//ValidateRect( &rectPaint );
		}
		
	}
	*/

	
}

bool CAxBaseView::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	dwStyle |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS;		
	return CViewBase::DoCreate( dwStyle, rc, pparent, nID );
}

BOOL CAxBaseView::OnEraseBkgnd(CDC* pDC) 
{
	
	CBitmap bm;				
	bm.LoadBitmap(IDB_PATTERN_VIEW);

	CBrush	pattern( &bm );
	pattern.UnrealizeObject();

	CRect	rect;
	bool	bResult;

	GetClientRect( rect );
	rect.right = m_rectForm.left;

	if (!rect.IsRectEmpty())
	{
		pDC->FillRect(rect, &pattern);
		bResult = true;
	}

	GetClientRect( rect );
	rect.left = m_rectForm.right;

	if (!rect.IsRectEmpty())
	{
		pDC->FillRect(rect, &pattern);
		bResult = true;
	}

	GetClientRect( rect );
	rect.right = m_rectForm.right;
	rect.left = m_rectForm.left;
	rect.top = m_rectForm.bottom;

	if (!rect.IsRectEmpty())
	{
		pDC->FillRect(rect, &pattern);
		bResult = true;
	}

	GetClientRect( rect );
	rect.right = m_rectForm.right;
	rect.left = m_rectForm.left;
	rect.bottom = m_rectForm.top;

	if (!rect.IsRectEmpty())
	{
		pDC->FillRect(rect, &pattern);
		bResult = true;
	}

	return bResult; 
}
			    
void CAxBaseView::_UpdateLayout( bool bRepositionControls )
{
	//m_rectForm = CRect( m_pointFormOffset, CSize(0, 0));

	CSize	size( m_rectForm.right, m_rectForm.bottom );

	CSize	sizeOld;

	sptrIScrollZoomSite	sptrZS( GetControllingUnknown() );
	sptrZS->GetClientSize( &sizeOld );

	if( sizeOld != size )
	{
		sptrZS->SetClientSize( size );
		Invalidate();
	}

	if( !bRepositionControls )
		return;


	POSITION	pos = m_sptrControls.GetFirstObjectPosition();

	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );

		ControlData	*pdata = _Get( punk );
		sptrIActiveXCtrl	sptrControl( punk );

		punk->Release();

		if( !pdata )
			continue;
		if( !pdata->pwnd->GetSafeHwnd() )
			continue;


		CRect	rect;
		CRect	rectCur;

		pdata->pwnd->GetWindowRect( &rectCur );
		ScreenToClient( &rectCur );

		sptrControl->GetRect( &rect );
		rect+=Offset();


		if( rectCur != rect )
		{
			pdata->pwnd->MoveWindow( rect );
//			pdata->pwnd->Invalidate();
//			::UpdateWindowAndChilds( pdata->pwnd->GetSafeHwnd() );

		}
		UpdateControlRect( sptrControl );
	}
}

DWORD CAxBaseView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szArgumenAXForm )  )
		return mvFull;
	if( !strcmp(szType, szArgumenAXCtrl )  )
		return mvFull;
	return mvNone;
}


CRect CAxBaseView::GetObjectRect( IUnknown *punkObject )
{

	if( CheckInterface( punkObject, IID_IActiveXCtrl ) )
	{
		CRect	rect;
		sptrIActiveXCtrl	sptrControl( punkObject );
		sptrControl->GetRect( &rect );
		rect += Offset();
		rect.InflateRect( 5, 5 );
		return rect;
	}

	return NORECT;
}

void CAxBaseView::SetObjectOffset( IUnknown *punkObject, CPoint pointOffset )
{
	if( !CheckInterface( punkObject, IID_IActiveXCtrl ) )
		return;
		/*return pointOffset;*/		

	CRect	rect;
	sptrIActiveXCtrl	sptrControl( punkObject );
	sptrControl->GetRect( &rect );
	rect.right = pointOffset.x+rect.Width();
	rect.bottom = pointOffset.y+rect.Height();
	rect.left = pointOffset.x;
	rect.top = pointOffset.y;

	//REPORT changes
	rect.DeflateRect(5,5,-5,-5);

	sptrControl->SetRect( rect );

	ControlData	*pdata = _Get( sptrControl );

	if( !pdata || ! pdata->pwnd->GetSafeHwnd() )
		return;

	pdata->pwnd->MoveWindow( rect );
	//::UpdateWindowAndChilds( pdata->pwnd->GetSafeHwnd() );
	/*return pointOffset;*/
}

void CAxBaseView::DoResizeRotateObject( IUnknown *punkObject, CFrame *pframe )
{
	
	if( CheckInterface( punkObject, IID_IActiveXCtrl ) )
	{
		
		CRect rect = ((CAxBaseFrame*)pframe)->GetCurentRect();//CalcBoundsRect();
		/*
		BOOL bFlipHorz, bFlipVert;
		bFlipHorz = bFlipVert = FALSE;

		if( rect.left > rect.right )
			bFlipHorz = TRUE;

		if( rect.top > rect.bottom )
			bFlipVert = TRUE;


		if( bFlipHorz | bFlipVert )
		{
			POSITION	pos = _Find( punkObject );
			if( pos )
			{
				ControlData *pdata = m_controlInfos.GetAt( pos );
				if( pdata && pdata->ptrCtrl != 0 )
				{
					IVTPrintCtrl* ptrPrintCtrl = NULL;
					if( SUCCEEDED(pdata->ptrCtrl->QueryInterface( IID_IVTPrintCtrl, (void**)&ptrPrintCtrl )) )
					{
						if( bFlipHorz )
							ptrPrintCtrl->FlipHorizontal( );

						if( bFlipVert )
							ptrPrintCtrl->FlipVertical( );


						ptrPrintCtrl->Release();
					}
					::StoreContainerToDataObject( punkObject, pdata->ptrCtrl );
				}
			}
		}
		*/

		rect.NormalizeRect();

		rect.InflateRect( -5, -5 );
		rect -= Offset();;

		CString	strParams;
		strParams.Format( "%d, %d, %d, %d", rect.left, rect.top, rect.right, rect.bottom );
		
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		ExecuteAction( "AxSetControlSize", strParams );
	}
	else
	{
		if( m_bCanResizeForm && CheckInterface( punkObject, IID_IActiveXForm ) )
		{
			AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

			CRect rect = ((CAxBaseFrame*)pframe)->GetCurentRect();//CalcBoundsRect();
			rect.InflateRect( -5, -5 );
			//rect -= Offset();;

			CString	strParams;
			strParams.Format( "%d, %d", rect.Width(), rect.Height() );			

			ExecuteAction( GetActionNameByResID( IDS_ACTION_RESIZESIZE ), strParams );

			CSize size;
			sptrIActiveXForm sptrForm( m_sptrControls );			
			if( sptrForm )
			{
				sptrForm->GetSize( &size );
				CRect	rectOld = m_rectForm;

				CalcFormRect( size );

				if( m_rectForm != rectOld )
					Invalidate();
			}
			
		}
	}
}


void CAxBaseView::BeforLButtonDown( CPoint pt )
{
	m_bObjectSelectProcess = true;
}


void CAxBaseView::AfterLButtonDown( CPoint pt )
{
	m_bObjectSelectProcess = false;
}


bool CAxBaseView::GetObjectsByPont( CPoint pt, CPtrList& arObject, bool& bCurSelectedUnderPoint,
										 bool bIncludeSelected )
{	
	bCurSelectedUnderPoint = false;

	POSITION pos = m_sptrControls.GetFirstObjectPosition();
	while( pos )
	{
		IUnknown *punk = m_sptrControls.GetNextObject( pos );		
		CRect	rect = GetObjectRect( punk );

		if( rect.PtInRect( pt )  && IsObjectCanBeSelected(punk))
		{
			if( !bCurSelectedUnderPoint )
				bCurSelectedUnderPoint = ((IUnknown*)m_ptrPrevSelectedObject == punk);

			
			if( !bIncludeSelected && (IUnknown*)m_ptrPrevSelectedObject == punk )
			{
				punk->Release();
				continue;
			}

			punk->AddRef();
			arObject.AddTail( punk );	
		}

		punk->Release();
	}

	if( arObject.GetCount() < 1 )
		return false;

	return true;
}

static int nCurrentSelected = -1;

bool CAxBaseView::PtInObject( IUnknown *punkObject, CPoint point )
{
	if( CheckInterface( punkObject, IID_IActiveXForm ) )
		return false;
	CRect rect = GetObjectRect( punkObject );
	return rect.PtInRect( point ) == TRUE;
}

IUnknown *CAxBaseView::GetObjectByPoint( CPoint point )
{

	//test if PrevSelectedObject was deleted
	{
		bool bFound = false;
		POSITION pos = m_sptrControls.GetFirstObjectPosition();
		while( pos )
		{
			IUnknown *punk = m_sptrControls.GetNextObject( pos );		
			if( punk == (IUnknown*)m_ptrPrevSelectedObject )
				bFound = true;

			punk->Release();
		}

		if( !bFound )
			m_ptrPrevSelectedObject = NULL;


	}

	bool bCurSelectedUnderPoint = false;

	//Only one object under point
	CPtrList arObjects;
	if( !GetObjectsByPont( point, arObjects, bCurSelectedUnderPoint ) )
	{
		if( bCurSelectedUnderPoint && ( m_ptrPrevSelectedObject != NULL ) )
		{
			m_ptrPrevSelectedObject->AddRef();

			{
				//Clean up code
				POSITION pos = 0;
				pos = arObjects.GetHeadPosition();

				while( pos )
				{
					IUnknown* punk = NULL;
					punk = (IUnknown*)arObjects.GetNext( pos );
					if( punk )
						punk->Release();

				}

				arObjects.RemoveAll();	
			}

			return m_ptrPrevSelectedObject;
		}
		return NULL;
	}

	
	CRect rcObject = NORECT;
	IUnknownPtr ptrObject;


	//return next or prev object
	if( m_bObjectSelectProcess && bCurSelectedUnderPoint )
	{
		nCurrentSelected++;
		if( nCurrentSelected < 0 || nCurrentSelected >= arObjects.GetCount() )
			nCurrentSelected = 0;


		POSITION posNextSelected = NULL;
		POSITION pos = arObjects.GetHeadPosition();
		int nCounter = 0;
		while( pos )
		{			
			if( nCounter == nCurrentSelected )
				posNextSelected = pos;

			arObjects.GetNext( pos );
			nCounter++;

		}
		if( posNextSelected  )
		{
			ptrObject = (IUnknown*)arObjects.GetAt( posNextSelected );
		}
	}

	if( ptrObject == NULL )
	{

		POSITION pos = 0;
		pos = arObjects.GetHeadPosition();

		while( pos )
		{
			IUnknown* punkNew = (IUnknown*)arObjects.GetNext( pos );
			CRect rectNew = GetObjectRect( punkNew );

			if( (
					(rcObject == NORECT) || //First object
					( rcObject.left <= rectNew.left && rcObject.right  >= rectNew.right &&
					  rcObject.top  <= rectNew.top  && rcObject.bottom >= rectNew.bottom) 
				) 
			  )

			{
				ptrObject = punkNew;
				rcObject = rectNew;
			}
		}
	}



	//Clean up code
	POSITION pos = 0;
	pos = arObjects.GetHeadPosition();

	while( pos )
	{
		IUnknown* punk = NULL;
		punk = (IUnknown*)arObjects.GetNext( pos );
		if( punk )
			punk->Release();

	}

	arObjects.RemoveAll();	

	if( m_bObjectSelectProcess )
		m_ptrPrevSelectedObject = ptrObject;

	if( ptrObject )
	{
		ptrObject->AddRef();
		return ptrObject;
	}

	return NULL;

}

bool CAxBaseView::UpdateContainerWnd( IUnknown* punkObject )
{
	CRect	rect;
	sptrIActiveXCtrl	sptrControl( punkObject );
	if( sptrControl == NULL )
		return false;
	
	sptrControl->GetRect( &rect );	
	rect += Offset();

	POSITION	pos = _Find( punkObject );	

	if( !pos )
		return false;


	ControlData *pdata = m_controlInfos.GetAt( pos );
	if( pdata && pdata->pwnd->GetSafeHwnd() )	
	{
		pdata->pwnd->Invalidate();
		pdata->pwnd->UpdateWindow();
	}

	return true;

}

void CAxBaseView::DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state )
{
	if( state ==  odsSelected || state ==  odsActive )
	{
		CRect	rect;
		sptrIActiveXCtrl	sptrControl( punkObject );
		if( sptrControl )
		{
			sptrControl->GetRect( &rect );	
			rect += Offset();

			POSITION	pos = _Find( punkObject );

			if( pos )
			{
				ControlData *pdata = m_controlInfos.GetAt( pos );
				if( pdata && pdata->pwnd->GetSafeHwnd() )	
				{
					pdata->pwnd->Invalidate();
					pdata->pwnd->UpdateWindow();
				}
			}

		}
		else
		{
			rect = m_rectForm;
		}
		
		//Scroll support
		rect = ConvertToWindow( rect );

		DWORD	dwFlags = CRectTracker::hatchedBorder;
		if( state ==  odsActive )
			dwFlags |= CRectTracker::resizeOutside;

		CRectTracker	tracker( rect, dwFlags );

		dc.SetBkMode( OPAQUE );
		tracker.Draw( &dc );

	}
}

LRESULT CAxBaseView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CViewBase::WindowProc( message, wParam, lParam );
}



BOOL CAxBaseView::PreTranslateMessage(MSG* pMsg) 
{
	BOOL bRet = CViewBase::PreTranslateMessage(pMsg);

	bool	bControl = false;
	if( pMsg->message >= WM_MOUSEFIRST &&
		pMsg->message <= WM_MOUSELAST )
	{
		if( _IsControlWindow( pMsg->hwnd ) )
		{
			//if( pMsg->message != WM_LBUTTONDOWN )
				PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);			

			bControl = true;
		}
	}


	if( pMsg->message == WM_SETFOCUS||
		pMsg->message == WM_MOUSEACTIVATE || 
		pMsg->message == WM_CHILDACTIVATE||
		pMsg->message == WM_NCACTIVATE||
		pMsg->message == WM_ACTIVATE)
	{
		if( _IsControlWindow( pMsg->hwnd ) )
		{
			SetFocus();
			bControl = true;
		}
	}

	if( pMsg->message == WM_SIZE )
	{
		if( _IsControlWindow( pMsg->hwnd ) )
		{
			TRACE( "Sizing control\n" );
		}
	}

	if( pMsg->message == WM_KEYDOWN )
	{
		return _OnKeyDown( (UINT)pMsg->wParam, LOWORD( pMsg->lParam ), HIWORD( pMsg->lParam ) );
	}

					 
	if( bControl )
		return true;

	return bRet;
}

void CAxBaseView::UpdateSelectionRect()
{
	CRect	rect = m_pframe->GetRect();
	CRect	rectCur;

	IUnknown	*punkObject = m_pframe->GetActiveObject();
	if( !punkObject )
		rectCur = NORECT;
	else
	{
		rectCur = GetObjectRect( punkObject );
		punkObject->Release();
	}

	if( rect != rectCur )
		m_pframe->SetRect( rectCur );
}

bool CAxBaseView::DoDrop( CPoint point )
{
	if( !CViewBase::DoDrop( point ) )
		return false;

	IActiveXFormPtr		ptrForm( m_sptrControls );
	if( ptrForm != 0 )
	{
		ptrForm->AutoUpdateSize();

	}

	return true;
}

void CAxBaseView::OnFinalRelease() 
{
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}


void CAxBaseView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//HINSTANCE m_hInstOld = AfxGetResourceHandle ();
	//AfxSetResourceHandle( AfxGetApp()->m_hInstance );
	// TODO: Add your message handler code here and/or call default
	//::ExecuteAction(_T("AxAddEventHandlers"));		
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	ExecuteAction( GetActionNameByResID( IDS_ACTION_PROPERTIES ) );	
	//AfxSetResourceHandle( m_hInstOld );

	//CViewBase::OnLButtonDblClk(nFlags, point);
}

bool IsWindowParentOfChild( HWND hwndParent, HWND hwndTest )
{
	if( hwndParent == hwndTest )
		return true;

	HWND	hwndChild = ::GetWindow( hwndParent, GW_CHILD );

	while( hwndChild )
	{
		if( IsWindowParentOfChild( hwndChild, hwndTest ) )
			return true;
		hwndChild = ::GetWindow( hwndChild, GW_HWNDNEXT );
	}
	return false;
}

bool CAxBaseView::_IsControlWindow( HWND hWnd, bool bLookChild )
{
	POSITION	pos = m_controlInfos.GetHeadPosition();
	while( pos )
	{
		ControlData	*pdata = m_controlInfos.GetNext( pos );

		if( !pdata || ! pdata->pwnd->GetSafeHwnd() )
			continue;

		if( bLookChild )
		{
			if( IsWindowParentOfChild( pdata->pwnd->GetSafeHwnd(), hWnd ) )
				return true;
		}
		else
		{
			if( pdata->pwnd->GetSafeHwnd() == hWnd )
				return true;
		}

	}
	return false;
}

bool CAxBaseView::ResizeXY(int &dcx, int &dcy)
{
	int nObjs = m_pframe->GetObjectsCount();
	if (nObjs != 1)
		return false;
	for (int i = 0; i < nObjs; i++)
	{
		IUnknownPtr punkObj(m_pframe->GetObjectByIdx(i), false);
		ControlData *pdata = _Get(punkObj);
		IViewCtrlPtr ptrViewCtrl(pdata->ptrCtrl);
		if (ptrViewCtrl != 0 && pdata->pwnd != NULL)
		{
			short ot;
			ptrViewCtrl->GetObjectTransformation(&ot);
			if (ot == otGrowTwoSide)
			{
				CRect rc;
				pdata->pwnd->GetWindowRect(&rc);
				if (rc.Width() != 0 || rc.Height() != 0)
				{
					if (dcx == 0)
						dcx = dcy*rc.Width()/rc.Height();
					else if (dcy == 0)
						dcy = dcx*rc.Height()/rc.Width();
					return true;
				}
			}
		}
	}
	return false;
}

bool CAxBaseView::_OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	int nDeltaX = ::GetValueInt( ::GetAppUnknown(), "\\AXEditor", "DeltaX", 5 );
	int nDeltaY = ::GetValueInt( ::GetAppUnknown(), "\\AXEditor", "DeltaY", 5 );

	if( nDeltaX < 0 )
		nDeltaX = 5;

	if( nDeltaY < 0 )
		nDeltaY = 5;


	int nDeltaXCtrl = ::GetValueInt( ::GetAppUnknown(), "\\AXEditor", "DeltaXCtrl", 5 );
	int nDeltaYCtrl = ::GetValueInt( ::GetAppUnknown(), "\\AXEditor", "DeltaYCtrl", 5 );

	if( nDeltaXCtrl < 0 )
		nDeltaXCtrl = 5;

	if( nDeltaYCtrl < 0 )
		nDeltaYCtrl = 5;

	if( m_sptrControls == 0 )
		return false;

	if( nChar == 13 && ( m_ptrEditObject == 0 ) /*Leave Enter For Edit ctrl( F2 press )*/ )	//enter
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		ExecuteAction( GetActionNameByResID( IDS_ACTION_PROPERTIES ) );
		return true;
	}

	if( nChar == 9 )
	{
		POSITION	pos = m_sptrControls.GetCurPosition();

		if( !pos )
			pos = m_sptrControls.GetFirstObjectPosition();

		if( !pos )
			return false;
		IUnknown	*p = m_sptrControls.GetNextObject( pos );
		p->Release();

		if( !pos )
			pos = m_sptrControls.GetFirstObjectPosition();

		m_sptrControls.SetCurPosition( pos );
		return true;
	}

	if( nChar == VK_LEFT ||
		nChar == VK_RIGHT ||
		nChar == VK_UP || 
		nChar == VK_DOWN )
	{

		int	x, y, cx, cy;
		bool	bShift = (::GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
		bool	bCtrl  = (::GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0;
		x = y = cx = cy = 0;

		if( bCtrl )
		{
			nDeltaX = nDeltaXCtrl;
			nDeltaY = nDeltaYCtrl;
		}



		if( nChar == VK_LEFT )
		{
			if( bShift )cx = -nDeltaX;else x = -nDeltaX;
		}
		if( nChar == VK_RIGHT )
		{
			if( bShift )cx = nDeltaX;else x = nDeltaX;
		}
		if( nChar == VK_UP )
		{
			if( bShift )cy = -nDeltaY;else y = -nDeltaY;
		}
		if( nChar == VK_DOWN )
		{
			if( bShift )cy = nDeltaY;else y = nDeltaY;
		}

		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 



		bool bForm = false;

		CSize sizeForm( 0, 0 );
		if( m_pframe )
		{
			IUnknown *punkObject = m_pframe->GetObjectByIdx( 0 );
			if( punkObject )
			{				
				IActiveXFormPtr	ptrAXF( punkObject );
				if( ptrAXF )
				{
					ptrAXF->GetSize( &sizeForm );
					bForm = true;
				}

				punkObject->Release();
			}
			
		}

		if( bForm )
		{
			int ndX, ndY;
			ndX = ndY = 0;

			if( cx != 0 )
				ndX = cx > 0 ? 1 : -1;

			if( cy != 0 )
				ndY = cy > 0 ? 1 : -1;

			sizeForm.cx += ndX;
			sizeForm.cy += ndY;


			if( sizeForm.cx > 0 && sizeForm.cx )
			{
				CalcFormRect( sizeForm );
				Invalidate();

				CString	str;
				str.Format( "%d, %d", sizeForm.cx, sizeForm.cy );

				ExecuteAction( GetActionNameByResID( IDS_ACTION_RESIZESIZE ), str );
				
			}
		}
		else
		{
//			ResizeXY(cx, cy);
			CString	str;
			str.Format( "%d, %d, %d, %d", x, y, cx, cy );
			ExecuteAction( GetActionNameByResID( IDS_ACTION_MOVESIZE ), str );
		}

		

		{
			IUnknown *punkObject = m_pframe->GetObjectByIdx( 0 );
			if( punkObject )
			{				
				OnSelectObject( punkObject );
				punkObject->Release();
			}
		}		
		return true;
	}

	
	if( nChar == VK_F2 )
	{
		if( !m_pframe->GetObjectsCount() )
			return false;

		IUnknown	*punkObject = m_pframe->GetObjectByIdx( 0 );

		CRect		rect = GetObjectRect( punkObject );
		rect = ConvertToWindow( rect );
		
		IActiveXCtrlPtr	ptrAX( punkObject );
		IActiveXFormPtr	ptrAXF( punkObject );

		punkObject->Release();

		CString	str;

		if( get_object_text( punkObject, str ) )
		{
			m_ptrEditObject = punkObject;
		}
		else if( ptrAX != NULL )
		{
			ControlData* pData= _Get( punkObject );

			if( pData == NULL )
				return false;

			if( pData && pData->pwnd->GetSafeHwnd() )	
			{
				char szBuf[255];
				pData->pwnd->SendMessage( WM_GETTEXT, (LPARAM)254, (LPARAM)szBuf );
				str = szBuf;
			}
			m_ptrEditObject = ptrAX;
		}
		else if( ptrAXF != 0 )
		{
			rect = m_rectCaption;

			m_ptrEditObject = ptrAXF;

			BSTR	bstr;
			ptrAXF->GetTitle( &bstr );
			str = bstr;
			::SysFreeString( bstr );
		}
		else 
			return false;


		CEditCtrl* pEdit = new CEditCtrl;
		
		pEdit->Create(WS_CHILD | WS_VISIBLE | WS_BORDER  |
						ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_MULTILINE,
				rect,  this, IDC_EDIT_CTRL );			
			
		pEdit->SetWindowText( str );
		pEdit->UpdateControlSize();
		pEdit->SetFocus();	
		
		return true;
		
	}
	
	return false;

	//CViewBase::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CAxBaseView::OnEditCtrlChange( CString str, BOOL bSave)
{
	if( bSave )
	{
		IActiveXCtrlPtr	ptrAX( m_ptrEditObject );
		IActiveXFormPtr	ptrAXF( m_ptrEditObject );

		if( set_object_text( m_ptrEditObject, str ) )
		{
			Invalidate();
		}
		else if( ptrAX != 0 )
		{
			ControlData* pData= _Get( m_ptrEditObject );

			if( pData && pData->pwnd->GetSafeHwnd() )	
				pData->pwnd->SendMessage( WM_SETTEXT, 0L, (LPARAM)(LPCTSTR)str );


			if( pData->ptrCtrl )
				::StoreContainerToDataObject( m_ptrEditObject, pData->ptrCtrl );

			Invalidate();
		}
		else if( ptrAXF != 0 )
		{
			_bstr_t	bstrT = (const char *)str;
			ptrAXF->SetTitle( bstrT );

			Invalidate();
		}
		
	}

	m_ptrEditObject = 0;
}

void CAxBaseView::UpdateControlRect( IUnknown *punkControl )
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
	ScreenToClient( rectControl );

	rectControl-= Offset();

	ptrAX->SetRect( rectControl );


	CSize	size( 0, 0 );

	sptrIActiveXForm		sptrForm( m_sptrControls );
	if( sptrForm != 0 )
	{
		sptrForm->AutoUpdateSize();
		sptrForm->GetSize( &size );
		CRect	rectOld = m_rectForm;
		CalcFormRect( size );

		if( m_rectForm != rectOld )
			Invalidate();
	}
}
	

CPoint CAxBaseView::GetScrollPos( )
{
	CPoint pt = ::GetScrollPos( GetControllingUnknown() );
	double fZoom = GetZoom( GetControllingUnknown() );
	return CPoint( int( (double)pt.x / fZoom ), int( (double)pt.y / fZoom ) );
}

//coordinate mapping - screen to client
CRect CAxBaseView::ConvertToClient( CRect rc)
{	
	double fZoom = 1.0;
	CPoint ptScroll = GetScrollPos( );

	rc.left = int( (rc.left+ptScroll.x) / fZoom );//_AjustValueEx( 1./fZoom, (rc.left+ptScroll.x) / fZoom );
	rc.right = int( (rc.right+ptScroll.x) / fZoom );
	rc.top = int( (rc.top+ptScroll.y) / fZoom );
	rc.bottom = int( (rc.bottom+ptScroll.y) / fZoom );

	return rc;	
}

CPoint CAxBaseView::ConvertToClient( CPoint pt)
{
	double fZoom = 1.0;
	CPoint ptScroll = GetScrollPos( );

	pt.x = int( (pt.x+ptScroll.x) / fZoom );
	pt.y = int( (pt.y+ptScroll.y) / fZoom );

	return pt;
}

CSize CAxBaseView::ConvertToClient( CSize size)
{
	double fZoom = 1.0;

	size.cx = int( size.cx / fZoom );
	size.cy = int( size.cy / fZoom );

	return size;
}

//coordinate mapping - client to screen
CRect CAxBaseView::ConvertToWindow( CRect rc)
{
	double fZoom = 1.0;
	CPoint ptScroll = GetScrollPos( );

	rc.left = _AjustValue( fZoom, rc.left*fZoom-ptScroll.x );
	rc.right = _AjustValue( fZoom, rc.right*fZoom-ptScroll.x );
	rc.top = _AjustValue( fZoom, rc.top*fZoom-ptScroll.y );
	rc.bottom = _AjustValue( fZoom, rc.bottom*fZoom-ptScroll.y );

	return rc;
}

CPoint CAxBaseView::ConvertToWindow( CPoint pt)
{
	double fZoom = 1.0;
	CPoint ptScroll = GetScrollPos( );

	pt.x = _AjustValue( fZoom, pt.x * fZoom - ptScroll.x );
	pt.y = _AjustValue( fZoom, pt.y * fZoom - ptScroll.y );

	return pt;
}

CSize CAxBaseView::ConvertToWindow( CSize size)
{
	double fZoom = 1.0;

	size.cx = _AjustValue( fZoom, size.cx * fZoom );
	size.cy = _AjustValue( fZoom, size.cy * fZoom );

	return size;
}

void CAxBaseView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	ShowInsertCtrlSheet( bActivate );
	if( bActivate )
		CreateCtrlPane();
	else
		DestroyCtrlPane();
}


POSITION CAxBaseView::GetFisrtVisibleObjectPosition()
{
	if( m_sptrControls != 0 )
		return (POSITION)1;
	return 0;
}

IUnknown *CAxBaseView::GetNextVisibleObject( POSITION &rPos )
{
	/*
	if( (long)rPos == 1 )
	{
		rPos = 0;
		IUnknown	*punk = m_sptrControls;
		if( punk )punk->AddRef();
		return punk;
	}
	return 0;
	*/
	if( (long)rPos == 1 )
	{
		IUnknown	*p = m_sptrControls;
		if( p == NULL )
		{
			rPos = 0;
			return 0;
		}

		p->AddRef();

		rPos = 0;

		return p;
	}
	return 0;

}

void CAxBaseView::CalcFormRect( CSize size )
{
	if( m_bDrawFrame )
	{
		m_rectForm.left = m_pointFormOffset.x-2;
		m_rectForm.top = m_pointFormOffset.y-cyCaption-6;
		m_rectForm.right = m_pointFormOffset.x+size.cx+2;
		m_rectForm.bottom = m_pointFormOffset.y+size.cy+2;

		m_rectCaption.left = m_rectForm.left+4;
		m_rectCaption.right = m_rectForm.right-4;
		m_rectCaption.top = m_rectForm.top+4;
		m_rectCaption.bottom = m_rectForm.top+4+cyCaption;
	}
	else
	{
		m_rectForm = CRect( m_pointFormOffset, size );
		m_rectCaption = NORECT;
	}
}


////////////////////////////////////////////////////////////////
//
//	Status Pane support
//
////////////////////////////////////////////////////////////////
void CAxBaseView::CreateCtrlPane()
{
	DestroyCtrlPane();
	
	INewStatusBar* psb = 0;
	psb = ::StatusGetBar();
	if( psb )
	{
		int nWidth = ::StatusCalcWidth("200x200 +200,+200", true );
		psb->AddPane( guidPaneAxCtrlSize, nWidth, 0, 0  );
		psb->AddPane( guidPaneAxCtrlPos, nWidth, 0, 0  );

		::StatusSetPaneIcon( guidPaneAxCtrlSize, 
			(HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_SIZE ), IMAGE_ICON, 16, 16, 0 ) );

		::StatusSetPaneIcon( guidPaneAxCtrlPos, 
			(HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_POSITION ), IMAGE_ICON, 16, 16, 0 ) );

		psb->Release();	psb = 0;		
	}

}

////////////////////////////////////////////////////////////////
void CAxBaseView::DestroyCtrlPane()
{
	INewStatusBar* psb = 0;
	psb = ::StatusGetBar();
	if( psb )
	{
		psb->RemovePane( guidPaneAxCtrlSize );
		psb->RemovePane( guidPaneAxCtrlPos );
		psb->Release();	psb = 0;
	}

}


////////////////////////////////////////////////////////////////
void CAxBaseView::UpdateSizePane( CString strText )
{
	StatusSetPaneText( guidPaneAxCtrlSize, (LPCSTR)strText );
}

////////////////////////////////////////////////////////////////
void CAxBaseView::UpdatePosPane( CString strText )
{
	StatusSetPaneText( guidPaneAxCtrlPos, (LPCSTR)strText );
}

////////////////////////////////////////////////////////////////
void CAxBaseView::OnSelectObject(IUnknown* punkObject )
{
	if( !punkObject )
		return;

	CString strRes;
	CRect rc = PaneConvertObjectRect( punkObject );

	CString strSize;
	CString strPos;

	strPos.Format( "[%d,%d]", rc.left, rc.top );
	strSize.Format( "%dx%d", rc.Width(), rc.Height() );
	
	
	UpdateSizePane( strSize );
	UpdatePosPane( strPos );
	SetFocus(); //SBT1812
}

static CRect g_rcObject = NORECT;
static CRect g_rcSelect = NORECT;

static CPoint g_ptOffset = CPoint(0,0);

////////////////////////////////////////////////////////////////
void CAxBaseView::OnEndDragDrop()
{
	CViewBase::OnEndDragDrop();
	g_rcObject = NORECT;
	g_ptOffset = CPoint(0,0);	
}

////////////////////////////////////////////////////////////////
void CAxBaseView::OnSetObjectOffset( IUnknown *punkObject, CPoint pointOffest )
{		
	if( !m_pframe )
		return;	

	CAxBaseFrame* pFrame = (CAxBaseFrame*)m_pframe;

	CRect rcObject = PaneConvertObjectRect( punkObject );	


	CString str;
	str.Format( "\nOffsets:%d, %d", pointOffest.x, pointOffest.y );
	TRACE( str );

	
	str.Format( "\nObject:%d, %d", rcObject.left, rcObject.top );
	TRACE( str );

	int nLeft, nTop;


	CPoint ptOff = rcObject.TopLeft() - g_rcObject.TopLeft();
	
	ptOff	= PaneConvertOffset( ptOff );

	nLeft	= rcObject.left;
	nTop	= rcObject.top;


	if( g_rcObject == NORECT )
	{
		IUnknownPtr ptrObject = punkObject;

		if( ptrObject == NULL )
			return;

		g_rcObject = PaneConvertObjectRect( ptrObject );
	}

	CString strPos;
	strPos.Format( "[%d,%d] %s%d,%s%d", nLeft, nTop, 
						ptOff.x < 0 ? (const char*)"" : (const char*)"+", ptOff.x,
						ptOff.y < 0 ? (const char*)"" : (const char*)"+", ptOff.y
						);
	
	UpdatePosPane( strPos );

}


////////////////////////////////////////////////////////////////
void CAxBaseView::OnFrameTrack( CPoint pt )
{
	if( !m_pframe )
		return;

	CAxBaseFrame* pFrame = (CAxBaseFrame*)m_pframe;

	CRect rcSelect = pFrame->GetSelectRect();
	

	if( g_rcSelect == NORECT )
		g_rcSelect = rcSelect;

	if( g_rcObject == NORECT )
	{
		IUnknownPtr ptrObject;
		
		if( pFrame->GetObjectsCount() > 0 )
		{
			IUnknown* punkObject = NULL;
			punkObject = m_pframe->GetObjectByIdx( 0 );
			if( punkObject )
			{
				ptrObject = punkObject;
				punkObject->Release();
			}
		}

		if( ptrObject == NULL )
			return;

		g_rcObject = PaneConvertObjectRect( ptrObject );
	}

	CPoint ptSelFirst = CPoint( g_rcSelect.Width(), g_rcSelect.Height() );	
	CPoint ptSel = CPoint( rcSelect.Width(), rcSelect.Height() );	

	CPoint ptOff = ptSel - ptSelFirst;
	ptOff = PaneConvertOffset( ptOff );

	int nWidth, nHeight;

	nWidth = ptOff.x + g_rcObject.Width();
	nHeight = ptOff.y + g_rcObject.Height();


	StatusSetPaneText( guidPaneAction, "%dx%d %s%d,%s%d", nWidth, nHeight, 
						ptOff.x < 0 ? (const char*)"" : (const char*)"+", ptOff.x,
						ptOff.y < 0 ? (const char*)"" : (const char*)"+", ptOff.y
						);

}

////////////////////////////////////////////////////////////////
void CAxBaseView::OnFrameFinishTrack( CPoint pt )
{
	g_rcObject = NORECT;
	g_rcSelect = NORECT;
	StatusSetPaneText( guidPaneAction, 0 );
	StatusSetPaneIcon( guidPaneAction, 0 );
}

////////////////////////////////////////////////////////////////
CPoint CAxBaseView::PaneConvertOffset( CPoint ptOffset )
{
	return ptOffset;
}

////////////////////////////////////////////////////////////////
CRect CAxBaseView::PaneConvertObjectRect( IUnknown* punkObject )
{
	if( CheckInterface( punkObject, IID_IActiveXForm ) )
	{		
		IActiveXFormPtr	ptrF( punkObject );
		CSize	size;
		ptrF->GetSize( &size );
		CRect rect = CRect( CPoint(0,0), size );		
		return rect;
	}
	else
	{
		if( CheckInterface( punkObject, IID_IActiveXCtrl ) )
		{
			CRect	rect;
			sptrIActiveXCtrl	sptrControl( punkObject );
			sptrControl->GetRect( &rect );
			return rect;
		}
	}		

	return NORECT;
}


/*
IMPLEMENT_DYNCREATE(CAxDesktopFrame, CAxBaseFrame);

CAxDesktopFrame::CAxDesktopFrame()
{
	m_bDragDropFrame = false;
	m_bSelectFrameMode = false;
	m_rectSelect = NORECT;
}

void CAxDesktopFrame::OnBeginDragDrop()
{
	CFrame::OnBeginDragDrop();
	Redraw();
}

void CAxDesktopFrame::OnEndDragDrop()
{
	Redraw();
	CFrame::OnEndDragDrop();
}

void CAxDesktopFrame::Redraw( bool bOnlyPoints, bool bUpdate )
{
	if( (m_bDragDropActive && m_bDragDropFrame) ||
		(m_bInTrackMode && !m_bSelectFrameMode ) )
	{
		CClientDC dc( 0 );
		Draw( dc, NORECT, 0, 0 );
	}
	else
		CFrame::Redraw( bOnlyPoints, bUpdate );
}


void CAxDesktopFrame::Draw(CDC& dc, CRect rectPaint, BITMAPINFOHEADER *pbih, byte *pdibBits )
{
	ASSERT( m_pView );
	if( !m_bDragDropActive && m_bDragDropFrame )
		return;

	if( m_bDragDropActive && m_bDragDropFrame )
	{
		CClientDC	dcDesktop( 0 );

		IUnknown *punkObject = 0;
		int nNumObjs = GetObjectsCount();

		for(int i = 0; i < nNumObjs; i++)
		{
			punkObject = GetObjectByIdx( i );

			if( punkObject )
			{
				IActiveXCtrlPtr	ptrControl( punkObject );
				punkObject->Release();

				if( ptrControl )
				{

					CRect	rect;
					
					ptrControl->GetRect( &rect );
					rect += ((CAxBaseView*)m_pView)->Offset();

					m_pView->ClientToScreen( &rect );

					//Scroll support
					//+++++++++++++++++++++++++++++
					//CPoint	ptScroll = ::GetScrollPos( m_pView->GetControllingUnknown() );
					CPoint	ptScroll = ((CAxBaseView*)m_pView)->GetScrollPos( );

					rect -= ptScroll;
					

					dcDesktop.SetROP2( R2_XORPEN );
					dcDesktop.DrawFocusRect( rect );
				}
			}
		}

	}
	else
	if( m_bInTrackMode&&!m_bSelectFrameMode )
	{
		ASSERT( !m_bDragDropActive );
		ASSERT( !m_bDragDropFrame );

		CClientDC	dc( 0 );

		CRect	rect( m_rectCurrent );
		rect.InflateRect( -5, -5 );

		m_pView->ClientToScreen( rect );

		dc.SetROP2( R2_XORPEN );
		dc.DrawFocusRect( rect );
	}
	else
	{
		int nNumObjs = GetObjectsCount();
		for(int i = 0; i < nNumObjs; i++)
		{
			IUnknown *punkObject = GetObjectByIdx( i );
			CRect rc = m_pView->GetObjectRect(punkObject);
			
			if( !CheckInterface( punkObject, IID_IActiveXForm ) )
			{
				TRACE("Form\n");
			}
				
				

			punkObject->Release();

			m_pView->DoDrawFrameObjects(punkObject, dc, rectPaint, pbih, pdibBits, (i==0)?odsActive:odsSelected );
		}
	}
}

bool CAxDesktopFrame::DoEnterMode(CPoint point)
{
	if( m_bSelectFrameMode  )
		return CMouseImpl::DoEnterMode( point );

	if( GetObjectsCount() > 0 )
	{
		IUnknown	*punkObj = GetObjectByIdx( 0 );

		bool	bForm = CheckInterface( punkObj, IID_IActiveXForm );
		punkObj->Release();

		if( bForm )return true;
	}
	return CFrame::DoEnterMode( point );
}

bool CAxDesktopFrame::DoStartTracking( CPoint pt )
{
	if( m_bSelectFrameMode  )
		return true;
	return CFrame::DoStartTracking( pt );
}

bool CAxDesktopFrame::DoTrack( CPoint pt )
{
	if( m_pView )
	{
		CAxBaseView* pView = (CAxBaseView*)m_pView;
		pView->OnFrameTrack( pt );
	}

	if( m_bSelectFrameMode  )
	{
		RedrawSelectRect();

		m_rectSelect.top = min( m_pointFirstPoint.y, pt.y );
		m_rectSelect.left = min( m_pointFirstPoint.x, pt.x );
		m_rectSelect.bottom = max( m_pointFirstPoint.y, pt.y );
		m_rectSelect.right = max( m_pointFirstPoint.x, pt.x );

		RedrawSelectRect();


		return false;
	}

	return CFrame::DoTrack( pt );
}

bool CAxDesktopFrame::DoFinishTracking( CPoint pt )
{
	if( m_pView )
	{
		CAxBaseView* pView = (CAxBaseView*)m_pView;
		pView->OnFrameFinishTrack( pt );
	}

	if( m_bSelectFrameMode  )
	{

		RedrawSelectRect();	
		//select objects

		CTypedPtrArray<CPtrArray, IUnknown*>ptrSelected;

		POSITION	pos = GetFirstObjectPosition();

		while( pos )
		{
			IUnknown	*punkObject = GetNextObject( pos );
			if( punkObject )
			{
				IActiveXCtrlPtr	ptrAX( punkObject );
				if( ptrAX )
				{

					CRect		rect;
					ptrAX->GetRect( &rect );
					rect += ((CAxBaseView*)m_pView)->Offset();

					if( m_rectSelect.PtInRect( rect.TopLeft() ) &&
						m_rectSelect.PtInRect( rect.BottomRight() ) )
					{
						ptrSelected.Add( punkObject );
					}
				}

				punkObject->Release();
			}
		}

		if( ptrSelected.GetSize() )
		{
			EmptyFrame();
			for( int i = 0; i < ptrSelected.GetSize(); 	i++ )
				SelectObject( ptrSelected[i], true );
		}

		m_pView->InvalidateRect( GetBoundRect() );
		m_bSelectFrameMode  = false;


		m_rectSelect = NORECT;
		return false;
	}
	return CFrame::DoFinishTracking( pt );
}

bool CAxDesktopFrame::DoLButtonUp( CPoint pt )
{
	if( m_bDragDropFrame )
		return false;
	if( (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0 )
	{
		//m_pView->PostMessage( WM_KEYDOWN, 13 );
		::ExecuteAction( GetActionNameByResID( IDS_ACTION_PROPERTIES ) );
	}
	return true;
}

bool CAxDesktopFrame::DoLButtonDown( CPoint pt )
{
	if( CResizingFrame::HitTest( pt ) != CResizingFrame::None )
		return false;

	if( CAxBaseFrame::DoLButtonDown( pt ) )
		return true;

	if( m_bDragDropFrame )
		return false;

	bool	bSelectControls = false;

	int nNumObjs = GetObjectsCount();
	for(int i = 0; i < nNumObjs; i++)
	{
		IUnknown *punkObject = GetObjectByIdx( i );
		if( CheckInterface( punkObject, IID_IActiveXCtrl ) )
			bSelectControls = true;

		if( punkObject )punkObject->Release();
	}


	if( !bSelectControls )
	{
		m_bSelectFrameMode  = true;
		m_rectSelect.top = pt.y;
		m_rectSelect.left = pt.x;
		m_rectSelect.bottom = pt.y;
		m_rectSelect.right = pt.x;
		m_pointFirstPoint = pt;

		RedrawSelectRect();

		
		StartTracking( pt );
		return true;
	}
	return false;
}

void CAxDesktopFrame::RedrawSelectRect()
{
	CClientDC	dc( 0 );

	dc.SetROP2( R2_XORPEN );

	CRect	rect = m_rectSelect;

	CPoint	ptScroll = ((CAxBaseView*)m_pView)->GetScrollPos( );
	rect -= ptScroll;

	m_pView->ClientToScreen( rect );

	//rect = ((CAxBaseView*)m_pView)->ConvertToWindow( rect );

	dc.DrawFocusRect( rect );
}
*/