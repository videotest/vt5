#include "stdafx.h"
#include "axview.h"
#include "resource.h"

//#include "mfcole\\OCCIMPL.H"



/*	
class CContainerWnd : public CWnd
{
public:
	CContainerWnd()
	{};
	~CContainerWnd()
	{TRACE("~CContainerWnd\n");};

public:
	COleControlSite	*GetControlSite()
	{	return m_pCtrlSite;	}

protected:
	virtual void PostNcDestroy(){};
};
*/	  

IMPLEMENT_DYNCREATE(CAxView, CWnd);


// {0CC9AF20-A346-4adc-8024-B1516BA2F282}
GUARD_IMPLEMENT_OLECREATE(CAxView, "AxEditor.AxView", 
0xcc9af20, 0xa346, 0x4adc, 0x80, 0x24, 0xb1, 0x51, 0x6b, 0xa2, 0xf2, 0x82);



BEGIN_MESSAGE_MAP(CAxView, CAxBaseView)
	//{{AFX_MSG_MAP(CAxView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


LRESULT CAxView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	return CAxBaseView::WindowProc( message, wParam, lParam );
}

CAxView::CAxView()
{
	_OleLockApp( this );

	EnableAggregation();
	m_pointFormOffset = CPoint( 30, 50 );
	m_rectForm = CRect( m_pointFormOffset, CSize(0, 0));
	
	m_sName = c_szCAxView;
	m_sUserName.LoadString(IDS_AXVIEW_NAME);

	CreateFrame();

	m_bCanResizeForm = true;
}

CAxView::~CAxView()
{
	TRACE( "CAxView::~CAxView()\n" );
	
	_OleUnlockApp( this );
}

void CAxView::AttachActiveObjects()
{
	if( !GetSafeHwnd() )
		return;																		 
	IUnknownPtr	sptrControls( ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeAXForm ), false );

	if( GetObjectKey( m_sptrControls ) == GetObjectKey( sptrControls ) )
		return;
	
	if( GetSafeHwnd() )
		_RemoveAllControls();

	m_sptrControls.Attach( sptrControls );


	CSize	size( 200, 100 );
	sptrIActiveXForm		sptrForm( m_sptrControls );
	if( sptrForm != 0 )sptrForm->GetSize( &size );
	
	CalcFormRect( size );
	

	_UpdateLayout();
	_AddAllControls();

	Invalidate();
}

DWORD CAxView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szArgumenAXForm )  )
		return mvFull;
	if( !strcmp(szType, szArgumenAXCtrl )  )
		return mvFull;
	return mvNone;
}

IUnknown * CAxView::GetNextVisibleObject(LPCTSTR szObjType, IUnknown * punkPrevObject)
{
	IUnknownPtr sptr = 0;
	if (!strcmp(szObjType, szArgumenAXForm))
	{
		if (!punkPrevObject)
			sptr = m_sptrControls;
	}

	if (sptr != 0)
		sptr.AddRef();
	return sptr;
}


void CAxView::CreateFrame()
{	
	m_pframeRuntime = RUNTIME_CLASS(CDesktopFrame);
}


void CAxView::_RemoveControl( IUnknown *punkControl )
{
	POSITION	pos = _Find( punkControl );

	if( !pos )
		return;

	ControlData *pdata = m_controlInfos.GetAt( pos );

	CRect	rectInvalidate = GetObjectRect( punkControl );


	//New code for control autosize
	{
		CRect rcWindow;
		pdata->pwnd->GetWindowRect( &rcWindow );
		sptrIActiveXCtrl	sptrControl( punkControl );
		if( sptrControl )
		{
			CRect rcCtrl;
			sptrControl->GetRect( &rcCtrl );
			
			rcCtrl.right  = rcCtrl.left + rcWindow.Width();
			rcCtrl.bottom = rcCtrl.top  + rcWindow.Height();

			sptrControl->SetRect( rcCtrl );
		}

		
	}

	//GetControlSize


	pdata->pwnd->DestroyWindow();

	delete pdata->pwnd;
	delete pdata;

	m_controlInfos.RemoveAt( pos );

	//m_pframe->Redraw();
	InvalidateRect( rectInvalidate );

	
}


CRect CAxView::GetObjectRect( IUnknown *punkObject )
{

	if( CheckInterface( punkObject, IID_IActiveXForm ) )
	{		
		IActiveXFormPtr	ptrF( punkObject );
		CSize	size;
		ptrF->GetSize( &size );
		CRect rect = CRect( Offset(), size );
		rect.InflateRect( 5, 5, 5, 5 );
		return rect;
	}
	else
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
	}		

	return NORECT;
}


IUnknown *CAxView::GetObjectByPoint( CPoint point )
{
	IUnknown* punkACCtrl = CAxBaseView::GetObjectByPoint( point );
	if( punkACCtrl )
		return punkACCtrl;

	//rect
	if( m_sptrControls )
	{
		m_sptrControls.AddRef();
		return m_sptrControls;
	}

	return NULL;
}


void CAxView::SetObjectOffset( IUnknown *punkObject, CPoint pointOffset )
{
	
	CAxBaseView::OnSetObjectOffset( punkObject, pointOffset );

	if( !CheckInterface( punkObject, IID_IActiveXCtrl ) )
		//return pointOffset;
		return;

	pointOffset-= Offset();	

	CRect	rect;
	sptrIActiveXCtrl	sptrControl( punkObject );
	sptrControl->GetRect( &rect );
	rect.right = pointOffset.x+rect.Width();
	rect.bottom = pointOffset.y+rect.Height();
	rect.left = pointOffset.x;
	rect.top = pointOffset.y;

	sptrControl->SetRect( rect );

	ControlData	*pdata = _Get( sptrControl );

	if( !pdata )
		return;

	rect+= Offset();

	pdata->pwnd->MoveWindow( rect );
	//::UpdateWindowAndChilds( pdata->pwnd->GetSafeHwnd() );
	//return pointOffset;
}





/*
BEGIN_INTERFACE_MAP(CAxView, CViewBase)
	INTERFACE_PART(CAxView, IID_ILayoutView, LayoutVw)
	INTERFACE_PART(CAxView, IID_IMenuInitializer, EditorMenu)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CAxView, LayoutVw);

BEGIN_MESSAGE_MAP(CAxView, CViewBase)
	//{{AFX_MSG_MAP(CAxView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HRESULT CAxView::XLayoutVw::GetControlObject( IUnknown *punkAXCtrl, IUnknown **ppAX )
{
	_try_nested(CAxView, LayoutVw, GetControlObject)
	{
		ControlData	*pdata = pThis->_Get( punkAXCtrl );

		if( !pdata )
			return 	E_INVALIDARG;
		*ppAX = pdata->ptrCtrl;
		(*ppAX)->AddRef();
		return S_OK;
	}
	_catch_nested;
}

IMPLEMENT_UNKNOWN(CAxView, EditorMenu);
HRESULT CAxView::XEditorMenu::OnInitPopup(BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu)
{
	_try_nested(CAxView, EditorMenu, OnInitPopup)
	{
		if (!hMenu)
			return E_INVALIDARG;

		*phOutMenu = ::CopyMenu(hMenu);
		if (!*phOutMenu)
			return E_INVALIDARG;
		// get menu
		CMenu* pmenu = CMenu::FromHandle(*phOutMenu);
		if (!pmenu)
			return E_FAIL;

		// get command Manager
		IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager2 );

		ASSERT(punk != NULL);
		if (!punk)
			return E_FAIL;

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

void CAxView::OnContextMenu(CWnd* pWnd, CPoint point) 
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






CAxView::CAxView()
{
	_OleLockApp( this );

	EnableAggregation();
	m_pointFormOffset = CPoint( 10, 10 );
	m_rectForm = CRect( m_pointFormOffset, CSize(0, 0));

	m_pframeRuntime = RUNTIME_CLASS(CAxDesktopFrame);

	m_sName = c_szCAxView;
	m_sUserName.LoadString(IDS_AXVIEW_NAME);
}

CAxView::~CAxView()
{
	TRACE( "CAxView::~CAxView()\n" );
	
	_OleUnlockApp( this );
}


void CAxView::AttachActiveObjects()
{
	if( !GetSafeHwnd() )
		return;
	IUnknownPtr	sptrControls( ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeAXForm ), false );

	if( GetObjectKey( m_sptrControls ) == GetObjectKey( sptrControls ) )
		return;
	
	if( GetSafeHwnd() )
		_RemoveAllControls();

	m_sptrControls.Attach( sptrControls );

	_UpdateLayout();
	_AddAllControls();
}

void CAxView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, const _variant_t var )
{
	CViewBase::OnNotify( pszEvent, punkHit, punkFrom, var );

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
		long	nAction = var;

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
			_RemoveControl( punkObject );
			_AddControl( punkObject );

			HWND	hwnd = ::GetFocus();

			if( _IsControlWindow( hwnd ) ) 
				SetFocus();
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
		
	}

}

//implementation details
void CAxView::_AddControl( IUnknown *punkControl )
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

	nID = _FindUniqueID( nID );

	sptrC->SetControlID( nID );

	rect+= m_pointFormOffset;

	CString strProgID = bstrProgID;
	::SysFreeString( bstrProgID );

	CString	strName = ::GetObjectName( punkControl );

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

	if( !bCreated )
	{
		delete pwnd;
		return;
	}

	
	ControlData	*pdata = new ControlData;
	pdata->punkCtrlData = punkControl;
	pdata->pwnd = pwnd;
	pdata->ptrCtrl = pdata->pwnd->GetControlSite()->m_pObject;
	pdata->nID = nID;

	IVtActiveXCtrl2Ptr	sptrVTC(pwnd->GetControlSite()->m_pObject);
	if(sptrVTC != 0)
		sptrVTC->SetApp(GetAppUnknown());

	::RestoreContainerFromDataObject( punkControl, pdata->ptrCtrl );

	UpdateControlRect( punkControl );

	m_controlInfos.AddTail( pdata );
	SetFocus();
}

void CAxView::_RemoveControl( IUnknown *punkControl )
{
	POSITION	pos = _Find( punkControl );

	if( !pos )
		return;

	ControlData *pdata = m_controlInfos.GetAt( pos );

	CRect	rectInvalidate = GetObjectRect( punkControl );


	//New code for control autosize
	{
		CRect rcWindow;
		pdata->pwnd->GetWindowRect( &rcWindow );
		sptrIActiveXCtrl	sptrControl( punkControl );
		if( sptrControl )
		{
			CRect rcCtrl;
			sptrControl->GetRect( &rcCtrl );
			
			rcCtrl.right  = rcCtrl.left + rcWindow.Width();
			rcCtrl.bottom = rcCtrl.top  + rcWindow.Height();

			sptrControl->SetRect( rcCtrl );
		}

		
	}

	//GetControlSize


	pdata->pwnd->DestroyWindow();

	delete pdata->pwnd;
	delete pdata;

	m_controlInfos.RemoveAt( pos );

	//m_pframe->Redraw();
	InvalidateRect( rectInvalidate );

	
}

void CAxView::_AddAllControls()
{
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();

	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		_AddControl( punk );
		punk->Release();
	}
}

void CAxView::_RemoveAllControls()
{
	while( POSITION pos = m_controlInfos.GetHeadPosition() )
	{
		ControlData *pdata = m_controlInfos.GetAt( pos );
		pdata->pwnd->DestroyWindow();
		delete pdata->pwnd;
		delete pdata;

		m_controlInfos.RemoveAt( pos );
	}
	m_pframe->EmptyFrame();
}

POSITION	CAxView::_Find( IUnknown *punkControl )
{
	POSITION	pos = m_controlInfos.GetHeadPosition();

	while( pos )
	{
		POSITION	posSave = pos;
		ControlData *pdata = m_controlInfos.GetNext( pos );

		if( pdata->punkCtrlData == punkControl )
			return posSave;
	}
	return 0;
}

POSITION	CAxView::_Find( CWnd *pwnd )
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

CAxView::ControlData
	*CAxView::_Get( IUnknown *punkControl )
{
	POSITION	posFound = _Find( punkControl );
	if( !posFound )
		return 0;
	return m_controlInfos.GetAt( posFound );
}

CAxView::ControlData
	*CAxView::_Get( CWnd *pwnd )
{
	POSITION	posFound = _Find( pwnd );
	if( !posFound )
		return 0;
	return m_controlInfos.GetAt( posFound );
}


UINT CAxView::_FindUniqueID( UINT nID )
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
int CAxView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	sptrIScrollZoomSite	sptrZS( GetControllingUnknown() );
	sptrZS->SetAutoScrollMode( false );

	AttachActiveObjects();

	_UpdateLayout( false );

	m_pframe->SetFlags( fcfResize );
	((CAxDesktopFrame*)m_pframeDrag)->m_bDragDropFrame = true;
	
	return 0;
}

void CAxView::OnDestroy() 
{
	_RemoveAllControls();
	CViewBase::OnDestroy();
}


void CAxView::OnPaint() 
{
	CRect	rectPaint = NORECT;
	GetUpdateRect( &rectPaint );

	CPaintDC dc(this); 

	dc.FillRect( m_rectForm, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );
	dc.DrawEdge( m_rectForm, BDR_RAISEDINNER, BF_RECT );


	
	ProcessDrawing( dc, rectPaint, 0, 0 );	

	m_pframe->Draw( dc, NORECT, 0, 0 );
	if( m_pframe->m_bDragDropActive )
		m_pframeDrag->Draw( dc, NORECT, 0, 0 );


	//::UpdateWindowAndChilds( GetSafeHwnd(), false );
}

bool CAxView::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	dwStyle |=WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	return CViewBase::DoCreate( dwStyle, rc, pparent, nID );
}

BOOL CAxView::OnEraseBkgnd(CDC* pDC) 
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

void CAxView::_UpdateLayout( bool bRepositionControls )
{
	m_rectForm = CRect( m_pointFormOffset, CSize(0, 0));

	CSize	size( 0, 0 );

	sptrIActiveXForm		sptrForm( m_sptrControls );
	if( sptrForm != 0 )
		sptrForm->GetSize( &size );

	CSize	sizeOld;

	sptrIScrollZoomSite	sptrZS( GetControllingUnknown() );
	sptrZS->GetClientSize( &sizeOld );

	if( sizeOld != size )
	{
		sptrZS->SetClientSize( size );
		Invalidate();
	}

	m_rectForm = CRect( m_pointFormOffset, size );

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


		CRect	rect;
		CRect	rectCur;
		pdata->pwnd->GetWindowRect( &rectCur );
		ScreenToClient( &rectCur );

		sptrControl->GetRect( &rect );
		rect+=m_rectForm.TopLeft();

		if( rectCur != rect )
		{
			pdata->pwnd->MoveWindow( rect );
//			pdata->pwnd->Invalidate();
//			::UpdateWindowAndChilds( pdata->pwnd->GetSafeHwnd() );

		}
		UpdateControlRect( sptrControl );
	}
}

DWORD CAxView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szArgumenAXForm )  )
		return mvFull;
	if( !strcmp(szType, szArgumenAXCtrl )  )
		return mvFull;
	return mvNone;
}


CRect CAxView::GetObjectRect( IUnknown *punkObject )
{
	if( !CheckInterface( punkObject, IID_IActiveXCtrl ) )
		return NORECT;

	CRect	rect;
	sptrIActiveXCtrl	sptrControl( punkObject );
	sptrControl->GetRect( &rect );
	rect += m_rectForm.TopLeft();
	rect.InflateRect( 5, 5 );

	return rect;
}
*/
/*
void CAxView::DoResizeRotateObject( IUnknown *punkObject, CFrame *pframe )
{
	if( !CheckInterface( punkObject, IID_IActiveXCtrl ) )
		return;

	CRect	rect = pframe->CalcBoundsRect();

	rect.InflateRect( -5, -5 );
	rect -= m_pointFormOffset;

	CString	strParams;
	strParams.Format( "%d, %d, %d, %d", rect.left, rect.top, rect.right, rect.bottom );

	ExecuteAction( "AxSetControlSize", strParams );
}

IUnknown *CAxView::GetObjectByPoint( CPoint point )
{
	POSITION	pos = m_sptrControls.GetFirstObjectPosition();

	while( pos )
	{
		IUnknown	*punk = m_sptrControls.GetNextObject( pos );
		CRect	rect = GetObjectRect( punk );

		if( rect.PtInRect( point ) )
			return punk;
		
		punk->Release();
	}
	return 0;
}


void CAxView::DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state )
{
	if( state ==  odsSelected || state ==  odsActive )
	{
		CRect	rect;
		sptrIActiveXCtrl	sptrControl( punkObject );
		sptrControl->GetRect( &rect );
		rect += m_rectForm.TopLeft();

		DWORD	dwFlags = CRectTracker::hatchedBorder;
		if( state ==  odsActive )
			dwFlags |= CRectTracker::resizeOutside;

		CRectTracker	tracker( rect, dwFlags );
		tracker.Draw( &dc );
	}
}

BOOL CAxView::PreTranslateMessage(MSG* pMsg) 
{
	bool	bControl = false;
	if( pMsg->message >= WM_MOUSEFIRST &&
		pMsg->message <= WM_MOUSELAST )
	{
		if( _IsControlWindow( pMsg->hwnd ) )
		{
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

	BOOL bRet = CViewBase::PreTranslateMessage(pMsg);
	if( bControl )
			return true;
	return bRet;
}

void CAxView::UpdateSelectionRect()
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

bool CAxView::DoDrop( CPoint point )
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

void CAxView::OnFinalRelease() 
{
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}


void CAxView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	//::ExecuteAction(_T("AxAddEventHandlers"));		
	ExecuteAction( GetActionNameByResID( IDS_ACTION_PROPERTIES ) );	
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

bool CAxView::_IsControlWindow( HWND hWnd, bool bLookChild )
{
	POSITION	pos = m_controlInfos.GetHeadPosition();
	while( pos )
	{
		ControlData	*pdata = m_controlInfos.GetNext( pos );

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

void CAxView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( m_sptrControls == 0 )
		return;

	if( nChar == 13 )	//enter
	{
		ExecuteAction( GetActionNameByResID( IDS_ACTION_PROPERTIES ) );
	}

	if( nChar == 9 )
	{
		POSITION	pos = m_sptrControls.GetCurPosition();

		if( !pos )
			pos = m_sptrControls.GetFirstObjectPosition();

		if( !pos )
			return;
		IUnknown	*p = m_sptrControls.GetNextObject( pos );
		p->Release();

		if( !pos )
			pos = m_sptrControls.GetFirstObjectPosition();

		m_sptrControls.SetCurPosition( pos );
	}

	if( nChar == VK_LEFT ||
		nChar == VK_RIGHT ||
		nChar == VK_UP || 
		nChar == VK_DOWN )
	{

		int	x, y, cx, cy;
		bool	bShift = (::GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;
		x = y = cx = cy = 0;
		int	delta = 5;


		if( nChar == VK_LEFT )
		{
			if( bShift )cx = -delta;else x = -delta;
		}
		if( nChar == VK_RIGHT )
		{
			if( bShift )cx = delta;else x = delta;
		}
		if( nChar == VK_UP )
		{
			if( bShift )cy = -delta;else y = -delta;
		}
		if( nChar == VK_DOWN )
		{
			if( bShift )cy = delta;else y = delta;
		}

		CString	str;
		str.Format( "%d, %d, %d, %d", x, y, cx, cy );
		ExecuteAction( GetActionNameByResID( IDS_ACTION_MOVESIZE ), str );


		
	}

	//CViewBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CAxView::UpdateControlRect( IUnknown *punkControl )
{
	POSITION	pos = _Find( punkControl );

	if( !pos )
		return;

	ControlData *pdata = m_controlInfos.GetAt( pos );

	IActiveXCtrlPtr	ptrAX( punkControl );

	CRect	rectControl;
	pdata->pwnd->GetWindowRect( &rectControl );
	ScreenToClient( rectControl );

	rectControl-= m_pointFormOffset;

	ptrAX->SetRect( rectControl );
}*/