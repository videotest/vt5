// DBaseGridView.cpp: implementation of the CDBaseGridView class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DBaseDoc.h"
#include "DBaseGridView.h"
#include "constant.h"
#include "types.h"
#include "dbmacros.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CDBaseGridView, CCmdTarget)

//////////////////////////////////////////////////////////////////////
CDBaseGridView::CDBaseGridView()
{
	EnableAutomation();
	EnableAggregation(); 
//	TRACE( "CDBaseGridView::CDBaseGridView()\n" );

	m_sUserName.LoadString(IDS_GRID_NAME);
	m_sName = c_szCDBaseGridView;//"DBaseGridView";


	m_pframeRuntime = 0;

	_OleLockApp( this );

}

//////////////////////////////////////////////////////////////////////
CDBaseGridView::~CDBaseGridView()
{
//	TRACE( "CGalleryView::~CGalleryView()\n" );	


	_OleUnlockApp( this );
}

/////////////////////////////////////////////////////////////////////////////

void CDBaseGridView::OnDestroy() 
{

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

	CViewBase::OnDestroy();	

	UnRegister( GetAppUnknown() );
	
}


/////////////////////////////////////////////////////////////////////////////
DWORD CDBaseGridView::GetMatchType( const char *szType )
{
	if( !strcmp(szType, szTypeQueryObject )  )
		return mvFull;
	return mvNone;
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDBaseGridView, CViewBase)
	//{{AFX_MSG_MAP(CDBaseGridView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

BEGIN_DISPATCH_MAP(CDBaseGridView, CViewBase)
	//{{AFX_DISPATCH_MAP(CDBaseGridView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


// Note: we add support for IID_IGalleryView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.


// {42CA470A-FDBF-47e8-A0B4-E3CDE782623C}
static const IID IID_IDBaseGridView =
{ 0x42ca470a, 0xfdbf, 0x47e8, { 0xa0, 0xb4, 0xe3, 0xcd, 0xe7, 0x82, 0x62, 0x3c } };

BEGIN_INTERFACE_MAP(CDBaseGridView, CViewBase)
	INTERFACE_PART(CDBaseGridView, IID_IDBaseGridView, Dispatch)
	INTERFACE_PART(CDBaseGridView, IID_IDBaseListener, DBaseListener)
	INTERFACE_PART(CDBaseGridView, IID_IDBGridView, DBaseGridVw)
	INTERFACE_PART(CDBaseGridView, IID_IPrintView, PrintView)
	INTERFACE_PART(CDBaseGridView, IID_IDBFilterView, FilterVw)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CDBaseGridView, DBaseGridVw);




// {26292643-6C3A-42d7-885A-60E0AD35C9D6}
GUARD_IMPLEMENT_OLECREATE(CDBaseGridView, szDBaseGridViewProgID, 
0x26292643, 0x6c3a, 0x42d7, 0x88, 0x5a, 0x60, 0xe0, 0xad, 0x35, 0xc9, 0xd6);


HRESULT CDBaseGridView::XDBaseGridVw::SaveGridSettings()
{
	_try_nested(CDBaseGridView, DBaseGridVw, SaveGRidSettings())
	{			
		pThis->m_ctrlGrid.SaveGridSettingsToQuery();

		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
int CDBaseGridView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;	

	//Register listener
	IDocument* pDoc = NULL;
	pDoc = GetDocument();
	if( pDoc )
	{
		sptrIDBaseDocument spDoc(pDoc);
		pDoc->Release();
		if( spDoc )
		{
			spDoc->RegisterDBaseListener( GetControllingUnknown() );							
		}
	}

	m_ctrlGrid.Create( CRect( 0, 0, lpCreateStruct->cx, lpCreateStruct->cy ), this,  IDC_GRID_CTRL );	
	
	m_ctrlGrid.Init();

	m_ctrlGrid.ModifyStyleEx(WS_BORDER, 0);

	
	sptrIScrollZoomSite	sptrZS(GetControllingUnknown());
	if( sptrZS ) sptrZS->SetAutoScrollMode(false);	

	{
		IWindowPtr ptrWnd = GetControllingUnknown();
		DWORD dwFlags = 0;
		ptrWnd->GetWindowFlags( &dwFlags );
		ptrWnd->SetWindowFlags( ( dwFlags & ~wfZoomSupport ) | wfSupportFitDoc2Scr );
	}

	ResizeGrid();

	Register( GetAppUnknown() );
	
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::OnFinalRelease() 
{	
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::AttachActiveObjects()
{
	
	if( !GetSafeHwnd() )
		return;																		 
}

POSITION CDBaseGridView::GetFisrtVisibleObjectPosition()
{
	return m_ctrlGrid.GetFisrtVisibleObjectPosition();
}

IUnknown * CDBaseGridView::GetNextVisibleObject( POSITION &rPos )
{
	return m_ctrlGrid.GetNextVisibleObject( rPos );
}


/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{
	m_ctrlGrid.OnDBaseNotify(	pszEvent, punkObject, 
								punkDBaseDocument, 
								bstrTableName, bstrFieldName, 
								var );
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::OnSize(UINT nType, int cx, int cy) 
{
	CViewBase::OnSize(nType, cx, cy);
	ResizeGrid();
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::ResizeGrid()
{
	if( !::IsWindow( m_ctrlGrid.GetSafeHwnd() ) )
		return;

	CRect rcClient;
	GetClientRect( &rcClient );
	
	m_ctrlGrid.MoveWindow( &rcClient );	
}



/////////////////////////////////////////////////////////////////////////////
BOOL CDBaseGridView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if( wParam == IDC_GRID_CTRL )
	{
		NM_GRIDVIEW* pnmgv = (NM_GRIDVIEW*)lParam;
		if( pnmgv )
		{
			if( pnmgv->hdr.code == GVN_SELCHANGED )
			{				
				m_ctrlGrid.OnParentGetSelChangeMessage();
				m_ctrlGrid.SetPrevRow( pnmgv->iRow );
			}
		}
	}
	
	return CWnd::OnNotify(wParam, lParam, pResult);
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::OnSetFocus(CWnd* pOldWnd) 
{
	//CViewBase::OnSetFocus(pOldWnd);	
	if( m_ctrlGrid.GetSafeHwnd() )
		m_ctrlGrid.SetFocus( );
}



/////////////////////////////////////////////////////////////////////////////
//
//	print support
//
/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridView::GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX)
{
	return m_ctrlGrid.OnGetPrintWidth(nMaxWidth, nReturnWidth, nUserPosX, nNewUserPosX);
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridView::GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY)
{
	return m_ctrlGrid.OnGetPrintHeight(nMaxHeight, nReturnHeight, nUserPosY, nNewUserPosY);
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags)
{
	CDC* pdc = CDC::FromHandle( hdc );
	if (!pdc)
		return;

	m_ctrlGrid.OnPrintDraw(pdc, rectTarget, CRect(nUserPosX, nUserPosY, nUserPosX + nUserPosDX - 1, nUserPosY + nUserPosDY - 1));
}

/////////////////////////////////////////////////////////////////////////////
//
//Filter Support
//
/////////////////////////////////////////////////////////////////////////////

bool CDBaseGridView::GetActiveField( CString& strTable, CString& strField )
{
	return m_ctrlGrid.GetActiveField( strTable, strField );
}

bool CDBaseGridView::GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue )
{
	return m_ctrlGrid.GetActiveFieldValue( strTable, strField, strValue );
}


/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	ShowFilterPropPage( bActivate );

	/*
	if( bActivate )
		CreateRecordPaneInfo();
	else
		DestroyRecordPaneInfo();
		*/

}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseGridView::ShowFilterPropPage( bool bShow )
{
	ShowDBPage( bShow );
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_ctrlGrid.SendMessage(WM_HSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)(pScrollBar ? pScrollBar->m_hWnd : 0));
	CViewBase::OnHScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseGridView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_ctrlGrid.SendMessage(WM_VSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)(pScrollBar ? pScrollBar->m_hWnd : 0));
	CViewBase::OnVScroll(nSBCode, nPos, pScrollBar);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CDBaseGridView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPCHILDREN;
	return CViewBase::PreCreateWindow(cs);
}

void CDBaseGridView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{	
	if( !strcmp( pszEvent, szEventBeforeActionExecute ) )
		m_ctrlGrid.ForceEndEditing();
	
	CViewBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
}

BOOL CDBaseGridView::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}
