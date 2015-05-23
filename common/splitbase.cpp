#include "stdafx.h"
#include "resource.h"
#include "splitbase.h"
#include "docviewbase.h"
#include "nameconsts.h"
#include "wnd_misc.h"

#include "afxpriv.h"


/////////////////////////////////////////////////////////////////////////////////////////
CSplitterBase::XSplitterEntry::XSplitterEntry( int row, int col, BSTR bstr )
{
	m_rc = NORECT;
	bstrViewRuntime = bstr;
	punkView = 0;
	m_pwnd1 = 0;
	m_pwnd2 = 0;
	m_pwnd3 = 0;
	m_pwnd4 = 0;
	m_pwnd5 = 0;
	m_pscrollVert = 0;
	m_pscrollHorz = 0;

	m_wEnableMask = 0xFFFF;

	m_bOwnScrollBars = true;

	nRow = row;
	nCol = col;
}

CSplitterBase::XSplitterEntry::~XSplitterEntry()
{
	if( m_pwnd1 )
	{
		m_pwnd1->DestroyWindow();
		delete m_pwnd1;
	}
	if( m_pwnd2 )
	{
		m_pwnd2->DestroyWindow();
		delete m_pwnd2;
	}
	if( m_pwnd3 )
	{
		m_pwnd3->DestroyWindow();
		delete m_pwnd3;
	}
	if( m_pwnd4 )
	{
		m_pwnd4->DestroyWindow();
		delete m_pwnd4;
	}
	if( m_pwnd5 )
	{
		m_pwnd5->DestroyWindow();
		delete m_pwnd5;
	}

	if( m_bOwnScrollBars )
	{
		if( m_pscrollVert )
		{
			m_pscrollVert->DestroyWindow();
			delete m_pscrollVert;
		}
		if( m_pscrollHorz )
		{
			m_pscrollHorz->DestroyWindow();
			delete m_pscrollHorz;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////


CSplitterBase::CSplitterBase()
{
	_OleLockApp( this );

	EnableAutomation();
	EnableAggregation();

	m_pveActiveView = 0;
	m_sName = "splitter";
}

CSplitterBase::~CSplitterBase()
{
	POSITION	pos = GetFirstViewEntryPosition();

	while( pos )
		delete GetNextViewEntry( pos );

	m_listViews.RemoveAll();
	m_pveActiveView = 0;

	_OleUnlockApp( this );
}

void CSplitterBase::OnFinalRelease()
{
	delete this;
}

BEGIN_MESSAGE_MAP(CSplitterBase, CWnd)
	//{{AFX_MSG_MAP(CSplitterBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_ACTIVATEPANE, OnActivatePane)
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CSplitterBase, CWnd)
	//{{AFX_DISPATCH_MAP(CSplitterBase)
	DISP_FUNCTION(CSplitterBase, "Split", Split, VT_EMPTY, VTS_I2 VTS_I2)
	DISP_FUNCTION(CSplitterBase, "GetActiveViewType", GetActiveViewType, VT_BSTR, VTS_PVARIANT VTS_PVARIANT)	
	DISP_FUNCTION(CSplitterBase, "ActivateView", ActivateView, VT_EMPTY, VTS_I4 VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CSplitterBase, "GetColumnsCount", GetColumnsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CSplitterBase, "GetRowCount", GetRowCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CSplitterBase, "GetWidth", GetWidth, VT_I4, VTS_NONE)
	DISP_FUNCTION(CSplitterBase, "GetHeight", GetHeight, VT_I4, VTS_NONE)
	DISP_FUNCTION(CSplitterBase, "GetRowHeight", GetRowHeight, VT_I4, VTS_I4)
	DISP_FUNCTION(CSplitterBase, "GetColWidth", GetColWidth, VT_I4, VTS_I4)
	DISP_FUNCTION(CSplitterBase, "SetRowHeight", SetRowHeight, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CSplitterBase, "SetColWidth", SetColWidth, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CSplitterBase, "GetViewZoom", GetViewZoom, VT_R8, VTS_I4 VTS_I4)
	DISP_FUNCTION(CSplitterBase, "SetViewZoom", SetViewZoom, VT_EMPTY, VTS_I4 VTS_I4 VTS_R8)
	DISP_FUNCTION(CSplitterBase, "GetViewClientWidth", GetViewClientWidth, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(CSplitterBase, "GetViewClientHeight", GetViewClientHeight, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(CSplitterBase, "GetViewXScroll", GetViewXScroll, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(CSplitterBase, "GetViewYScroll", GetViewYScroll, VT_I4, VTS_I4 VTS_I4)
	DISP_FUNCTION(CSplitterBase, "SetViewScroll", SetViewScroll, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ISplitterWindow to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {AC8FC6D3-27A1-11D3-A5DC-0000B493FF1B}
static const IID IID_ISplitterWindowDisp =
{ 0xac8fc6d3, 0x27a1, 0x11d3, { 0xa5, 0xdc, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CSplitterBase, CWnd)
	INTERFACE_PART(CSplitterBase, IID_ISplitterWindowDisp, Dispatch)
	INTERFACE_PART(CSplitterBase, IID_ISplitterWindow, Splitter)
	INTERFACE_PART(CSplitterBase, IID_IWindow, Wnd)
	INTERFACE_PART(CSplitterBase, IID_IRootedObject, Rooted)
	INTERFACE_PART(CSplitterBase, IID_IHelpInfo, Help)
	INTERFACE_PART(CSplitterBase, IID_INamedObject2, Name)
END_INTERFACE_MAP()
IMPLEMENT_UNKNOWN(CSplitterBase, Splitter);
/////////////////////////////////////////////////////////////////////////////
// CSplitterBase message handlers
HRESULT CSplitterBase::XSplitter::GetActiveView( IUnknown **punkView )
{
	_try_nested(CSplitterBase, Splitter, GetActiveView)
	{
		*punkView = 0;
		if( pThis->m_pveActiveView )
		{
			*punkView = pThis->m_pveActiveView->punkView;
			(*punkView)->AddRef();
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterBase::XSplitter::ChangeViewType( int nRow, int nCol, BSTR bstrViewType )
{
	_try_nested(CSplitterBase, Splitter, ChangeViewType)
	{
		_bstr_t	bstr = bstrViewType;
		pThis->ChangeViewType( nRow, nCol, bstr );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterBase::XSplitter::GetRowColCount( int *pnRows, int *pnCols )
{
	_try_nested(CSplitterBase, Splitter, GetRowColCount)
	{
		POSITION	pos = pThis->GetFirstViewEntryPosition();
		bool	bFirstTime = true;

		int	nMinRow, nMaxRow, nMinCol, nMaxCol;

		while( pos )
		{
			XSplitterEntry	*pe = pThis->GetNextViewEntry( pos );
			if( bFirstTime )
			{
				nMinRow = nMaxRow = pe->nRow;
				nMinCol = nMaxCol = pe->nCol;
				bFirstTime = false;
			}
			else
			{
				nMinRow = min( nMinRow, pe->nRow );
				nMaxRow = max( nMaxRow, pe->nRow );

				nMinCol = min( nMinCol, pe->nCol );
				nMaxCol = max( nMaxCol, pe->nCol );
			}
		}
		*pnRows = nMaxRow-nMinRow+1;
		*pnCols = nMaxCol-nMinCol+1;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterBase::XSplitter::GetViewRowCol( int	nRow, int nCol, IUnknown **ppunkView )
{
	_try_nested(CSplitterBase, Splitter, GetViewTypeRowCol)
	{
		*ppunkView = 0;
		XSplitterEntry *pe = pThis->GetViewEntry( nRow, nCol );
		if( !pe )
			return E_INVALIDARG;

		*ppunkView = pe->punkView;
		(*ppunkView)->AddRef();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterBase::XSplitter::GetViewTypeRowCol( int nRow, int nCol, BSTR *pbstrViewType )
{
	_try_nested(CSplitterBase, Splitter, GetViewTypeRowCol)
	{
		*pbstrViewType = 0;

		XSplitterEntry *pe = pThis->GetViewEntry( nRow, nCol );
		if( !pe )
			return E_INVALIDARG;

		*pbstrViewType = pe->bstrViewRuntime.copy();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterBase::XSplitter::Split( int nRowCount, int nColCount )
{
	_try_nested(CSplitterBase, Splitter, Split)
	{
		pThis->DoSplit( nRowCount, nColCount );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterBase::XSplitter::GetActivePane( int *piRow, int *piCol )
{
	_try_nested(CSplitterBase, Splitter, GetActivePane)
	{						
		*piRow = 0;
		*piCol = 0;

		sptrIFrameWindow	sptrF( pThis->m_punkParent );
		IUnknown	*punk = 0;
		sptrF->GetActiveView( &punk );

		if( !punk )
			return E_INVALIDARG;

		XSplitterEntry *pe = pThis->GetViewEntry( punk );
		punk->Release();

		if( !pe )
			return E_INVALIDARG;

		*piRow = pe->nRow;
		*piCol = pe->nCol;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CSplitterBase::XSplitter::GetScrollBarCtrl( IUnknown *punkView, DWORD sbCode, HANDLE *phCtrl )
{
	_try_nested(CSplitterBase, Splitter, GetActivePane)
	{
		CSplitterBase::XSplitterEntry	*pe = pThis->GetViewEntry( punkView );

		*phCtrl = 0;

		if( !pe )
			return E_INVALIDARG;
		if( sbCode == SB_HORZ )
			*phCtrl = pe->m_pscrollHorz->GetSafeHwnd();
		if( sbCode == SB_VERT )
			*phCtrl = pe->m_pscrollVert->GetSafeHwnd();

		return S_OK;
	}
	_catch_nested;
}


HRESULT CSplitterBase::XSplitter::OptimizeSplitter()
{
	_try_nested(CSplitterBase, Splitter, OptimizeSplitter)
	{
		/*CSplitterBase::XSplitterEntry	*pe = pThis->GetViewEntry( punkView );

		if( !pe )
			return E_INVALIDARG;
		*/
		pThis->OptimizeSplitter();
		
		return S_OK;
	}
	_catch_nested;
}


//CWindowImpl helper
CWnd *CSplitterBase::GetWindow()
{
	return this;
}


//return the current vier name. If no current view name, return the first view
BSTR CSplitterBase::GetViewType( BSTR bstrDefProgID )
{
	bool bfirst_view = ( m_bstrLastViewProgID.length() == 0 );
	VERIFY( m_regViews.GetRegistredComponentCount()!= 0 );
	if( bstrDefProgID )
	{
		m_bstrLastViewProgID = bstrDefProgID;
	}

	//restore last active view
	//paul 18.04.2003
	if( bfirst_view )
	{
		IRootedObjectPtr ptr_ro( GetControllingUnknown() );
		if( ptr_ro )
		{
			IUnknown* punk = 0;
			ptr_ro->GetParent( &punk );
			if( punk )
			{
				IFrameWindowPtr ptr_frame( punk );
				punk->Release();	punk = 0;
				if( ptr_frame )
				{
					ptr_frame->GetDocument( &punk );
					if( punk )
					{
						CString str_last_active = ::GetValueString( punk, LAST_VIEW_SECTION, LAST_VIEW_PROGID, "" );
						punk->Release();	punk = 0;

						if( str_last_active.GetLength() )
						{
							CLSID clsid = {0};
							::CLSIDFromProgID( _bstr_t( str_last_active ), &clsid  );
							IUnknown* punk_view = 0;							
							if( S_OK == ::CoCreateInstance( clsid, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punk_view ) )
							{
								m_bstrLastViewProgID = str_last_active;
								punk_view->Release();
							}
						}
					}
				}
			}
		}
	}

	if( m_bstrLastViewProgID.length() == 0  )
	{
		IUnknown *punkDoc = 0;
		IFrameWindowPtr	ptrFrame( m_punkParent );
		ptrFrame->GetDocument( &punkDoc );

		IUnknown	*punkActiveObject = ::GetActiveObjectFromDocument( punkDoc, 0 );

		/*
		IUnknown	*punkActiveObject = NULL;
		if( punkDoc != NULL )
		{
			sptrIDataContext spDC( punkDoc );
			if( spDC )
			{
				spDC->GetActiveObject( 0, &punkActiveObject );
			}
		}
		*/
		IDocumentSitePtr	ptrDoc = punkDoc;
		if (punkDoc)punkDoc->Release();

		if( punkActiveObject )
		{
			CString	str = ::GetObjectKind( punkActiveObject );
			punkActiveObject->Release();
			DWORD	dw = 0;
			CString strV = ::FindMostMatchView( str, dw );
			if( !strV.IsEmpty() )
				m_bstrLastViewProgID = strV;
		}

		if( !m_bstrLastViewProgID.length() && m_regViews.GetRegistredComponentCount() )
		{
			IUnknown	*punkDocTempl = 0;
			ptrDoc->GetDocumentTemplate( &punkDocTempl );
			IDocTemplatePtr	ptrDocTempl = punkDocTempl;
			if( punkDocTempl )punkDocTempl->Release();

			BSTR	bstr = 0;
			ptrDocTempl->GetDocTemplString( CDocTemplate::docName, &bstr );
			CString	str = bstr;
			::SysFreeString( bstr );
			str = "\\"+str;
			m_bstrLastViewProgID = ::GetValueString( GetAppUnknown(), str, "DefaultView", m_regViews.GetComponentName( 0 ) );
		}

	}

	return m_bstrLastViewProgID;
}

//create a splitter window and fills the component registrator
bool CSplitterBase::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	//get the frame window
	sptrIFrameWindow	sptrFrame( m_punkParent );

	//get the document
	IUnknown *punkDoc = 0;
	if(sptrFrame) sptrFrame->GetDocument( &punkDoc );
	sptrIDocumentSite	sptrDocSite( punkDoc );
	if (punkDoc)
		punkDoc->Release();
	
	//get the document template
	IUnknown *punkTempl = 0;
	if(sptrDocSite) sptrDocSite->GetDocumentTemplate( &punkTempl );
	sptrIDocTemplate	sptrTempl( punkTempl );
	if (punkTempl)
		punkTempl->Release();

	//get the document template name
	BSTR	bstr = 0;
	if(sptrTempl) sptrTempl->GetDocTemplString( CDocTemplate::docName, &bstr );
	CString	strDocument = bstr;
	::SysFreeString( bstr );

	m_strDocTemplateName = strDocument;

	//fill the views array
	m_regViews.SetSectionName( strDocument+"\\"+szPluginView );

	if( m_regViews.GetRegistredComponentCount() == 0 )
		return false;

	dwStyle |= WS_CLIPCHILDREN|WS_CLIPSIBLINGS;
	
	//create a window
	if( !CWindowImpl::DoCreate( dwStyle, rc, pparent, nID ) )
		return false;

	return true;
}

//get splitter entry by row and col
CSplitterBase::XSplitterEntry	*
	CSplitterBase::GetViewEntry( int nRow, int nCol, POSITION *ppos )
{
	POSITION pos = GetFirstViewEntryPosition();

	while( pos )
	{
		if( ppos )	*ppos = pos;
		XSplitterEntry	*pe = GetNextViewEntry( pos );

		if( pe->nCol == nCol && pe->nRow == nRow )
			return pe;
	}

	return 0;
}


//get view entry by window's unknown
CSplitterBase::XSplitterEntry	*
	CSplitterBase::GetViewEntry( IUnknown *punkWnd, POSITION *ppos )
{
	CWnd	*pwnd = GetWindowFromUnknown( punkWnd );

	POSITION pos = GetFirstViewEntryPosition();

	while( pos )
	{
		if( ppos )	*ppos = pos;
		XSplitterEntry	*pe = GetNextViewEntry( pos );

		CWnd	*pwndTest = GetWindowFromUnknown( pe->punkView );

		if( pwnd->GetSafeHwnd() == pwndTest->GetSafeHwnd() )
			return pe;
	}
	return 0;
}

//return entry by scrollbar
CSplitterBase::XSplitterEntry	*
	CSplitterBase::GetViewEntry( int nChildID, POSITION *ppos )
{
	POSITION pos = GetFirstViewEntryPosition();

	while( pos )
	{
		if( ppos )	*ppos = pos;
		XSplitterEntry	*pe = GetNextViewEntry( pos );

		if( pe->m_pscrollHorz && pe->m_pscrollHorz->GetDlgCtrlID() == nChildID )
			return pe;
		if( pe->m_pscrollVert && pe->m_pscrollVert->GetDlgCtrlID() == nChildID )
			return pe;
		if( pe->m_pwnd1 && pe->m_pwnd1->GetDlgCtrlID() == nChildID )
			return pe;
		if( pe->m_pwnd2 && pe->m_pwnd2->GetDlgCtrlID() == nChildID )
			return pe;
		if( pe->m_pwnd3 && pe->m_pwnd3->GetDlgCtrlID() == nChildID )
			return pe;
		if( pe->m_pwnd4 && pe->m_pwnd4->GetDlgCtrlID() == nChildID )
			return pe;
		if( pe->m_pwnd5 && pe->m_pwnd5->GetDlgCtrlID() == nChildID )
			return pe;
	}
	return 0;
}



bool CSplitterBase::CreateView( int nRow, int nCol, BSTR bstrViewType )
{
	bstrViewType = this->GetViewType( bstrViewType );

	//AfxMessageBox("8");

	sptrIFrameWindow	sptrFrame( m_punkParent );

	//AfxMessageBox("9");

//generate the dummy window id
	UINT	nID = 1111+nRow*100+nCol;

	XSplitterEntry	*pe = new XSplitterEntry( nRow, nCol, bstrViewType );


	m_listViews.AddTail( pe );

	double fZoom = 1.;
	BOOL bFit = FALSE;

	if(m_pveActiveView)
	{
		IScrollZoomSite2Ptr sptrSZActive(m_pveActiveView->punkView);
		if(sptrSZActive != 0)
		{
			sptrSZActive->GetFitDoc(&bFit);
			sptrSZActive->GetZoom(&fZoom);
		}
	}

	//if( !m_pveActiveView )
		m_pveActiveView = pe;

	OnCreateView( pe );

	if(sptrFrame) sptrFrame->CreateView( nID, bstrViewType, &pe->punkView );

	
	{
		IScrollZoomSite2Ptr sptrSZNew(pe->punkView);
		if(sptrSZNew != 0)
		{
			sptrSZNew->SetFitDoc(bFit);
			sptrSZNew->SetZoom(fZoom);
		}
	}


	
	CWnd *pwnd = ::GetWindowFromUnknown( pe->punkView );

	if( pwnd )
	{
		pwnd->ShowWindow( SW_SHOW );

		pwnd->SendMessage( WM_INITIALUPDATE );
		pwnd->SendMessage( WM_SIZE );
	}
	
	OnActivatePane();

	return TRUE;
}

static bool _IsNeedActivateObject(IUnknown *punkView)
{
	IView2Ptr sptrView2(punkView);
	if (sptrView2 == 0)
		return true;
	DWORD dwViewFlags = 0;
	if (sptrView2->GetViewFlags(&dwViewFlags) != S_OK)
		return true;
	if (dwViewFlags & View_AutoActivateObjects)
		return false;
	return true;
}

bool CSplitterBase::ChangeViewType( int nRow, int nCol, BSTR bstrViewType )
{
	DO_LOCK_R(false);
	POSITION	pos;
	XSplitterEntry	*pe = GetViewEntry( nRow, nCol, &pos );
	sptrIFrameWindow	sptrFrame( m_punkParent );

	ASSERT( pe );
	if( !pe )
		return false;

	if( pe->bstrViewRuntime == _bstr_t( bstrViewType ) )
		return true;

	CPoint	pointScroll( 0, 0 );
	double	fZoom = 1;
	BOOL bFit = FALSE;

	{
		sptrIScrollZoomSite2	sptrVSOld( pe->punkView );
		if(sptrVSOld!=0)
		{
			sptrVSOld->GetScrollPos( &pointScroll );
			sptrVSOld->GetZoom( &fZoom );
			sptrVSOld->GetFitDoc( &bFit );
		}
	}

	OnDeleteView( pe );

	IUnknown	*punkView = pe->punkView;
//	punkView->AddRef();

	m_listViews.RemoveAt( pos );

	if( m_pveActiveView == pe )
		m_pveActiveView = 0;

	m_bstrLastViewProgID = bstrViewType;
	if( !CreateView( nRow, nCol  ) )
		return false;

// synchronize contexts
	//get the new viewentry
	XSplitterEntry	*peNew = GetViewEntry( nRow, nCol, &pos );
	IDataContext2Ptr sptrNewContext = peNew->punkView;

	sptrIScrollZoomSite2	sptrVSNew( peNew->punkView );
	if(sptrVSNew)
	{
		sptrVSNew->SetFitDoc( bFit );
		sptrVSNew->SetZoom( fZoom );
		sptrVSNew->SetScrollPos( pointScroll );
	}

	if(sptrFrame) sptrFrame->DestroyView( punkView );
	delete pe;

	DoRecalcLayout();
	if (_IsNeedActivateObject(sptrNewContext))
	{
	if (sptrNewContext != 0 && punkView)
	{
//		sptrNewContext->SynchronizeWithContext(punkView);
		IUnknown *punkObj = GetActiveObjectFromContextEx(sptrNewContext);
		if (punkObj)
		{
			sptrNewContext->SetActiveObject(0, punkObj, aofActivateDepended/*|aofSkipIfAlreadyActive*/);
			punkObj->Release();
		}
	}
	}

	OnActivatePane();

	return true;
}

bool CSplitterBase::DeleteView( int nRow, int nCol )
{
	sptrIFrameWindow	sptrFrame( m_punkParent );

	POSITION	pos = 0;
	XSplitterEntry	*pe = GetViewEntry( nRow, nCol, &pos );

	ASSERT( pe );
	if( !pe )return false;

	OnDeleteView( pe );

	if( m_pveActiveView == pe )
		m_pveActiveView = 0;

	IUnknown	*punkView = pe->punkView;

	if(sptrFrame) sptrFrame->DestroyView( punkView );

	delete pe;

	m_listViews.RemoveAt( pos );

	m_pveActiveView = GetViewEntry( 0, 0 );

	if( m_pveActiveView )
	{
		CWnd	*pwnd = GetWindowFromUnknown( m_pveActiveView->punkView );
		if( pwnd )
		{
	//		pwnd->SetFocus();
			CWnd	*pwndParent = pwnd->GetParent();
			if( pwndParent )pwndParent->SendMessage(WM_SETFOCUS);
		}		
	}


	return true;
}

//View list navigation 
POSITION CSplitterBase::GetFirstViewEntryPosition()
{
	return m_listViews.GetHeadPosition();
}

//return view entry by position
CSplitterBase::XSplitterEntry	*
	CSplitterBase::GetNextViewEntry( POSITION &pos )
{
	return (XSplitterEntry	*)m_listViews.GetNext( pos );
}

int CSplitterBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	DoRecalcLayout();
	
	return 0;
}

void CSplitterBase::OnSize(UINT nType, int cx, int cy) 
{
	//CWnd::OnSize(nType, cx, cy);
	
	DoRecalcLayout();	
}

void CSplitterBase::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	XSplitterEntry	*pe = GetViewEntry( pScrollBar->GetDlgCtrlID() );
	if( pe )
	{
		CWnd	*pwnd = ::GetWindowFromUnknown( pe->punkView )->GetParent();
		if( pwnd )
		{
			pwnd->SetFocus();
			pwnd->SendMessage( WM_VSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd );
		}
	}
}

void CSplitterBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	XSplitterEntry	*pe = GetViewEntry( pScrollBar->GetDlgCtrlID() );
	if( pe )
	{
		CWnd	*pwnd = ::GetWindowFromUnknown( pe->punkView )->GetParent();
		if( pwnd )
		{
			pwnd->SetFocus();
			pwnd->SendMessage( WM_HSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)pScrollBar->m_hWnd );
		}
	}
}

void CSplitterBase::ExecuteMenu( CPoint ptMenuPos, DWORD dwFlags )
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	_bstr_t	bstrMenuName = m_strDocTemplateName+_T(" Views");
	sptrCM->ExecuteContextMenu2( bstrMenuName, ptMenuPos, dwFlags );

}

void CSplitterBase::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	
	if( !m_pveActiveView )
		m_pveActiveView = GetViewEntry( 0, 0 );

	if( !m_pveActiveView )
		return;
	CWnd *pwnd = ::GetWindowFromUnknown( m_pveActiveView->punkView );

	if( !pwnd )
		return;
	pwnd->SetFocus();
	OnActivatePane();
}

void CSplitterBase::OnActivatePane()
{
	sptrIFrameWindow	sptrF( m_punkParent );
	IUnknown	*punk = 0;
	sptrF->GetActiveView( &punk );

	if( !punk )
		return;

	XSplitterEntry *pe = GetViewEntry( punk );
	punk->Release();

	if( !pe )
		return;

	if( m_pveActiveView == pe )
		return;

	XSplitterEntry *pseOld = m_pveActiveView;
	m_pveActiveView = pe;

	OnActivateView( m_pveActiveView, pseOld );
}

void CSplitterBase::Split(short nCol, short nRow) 
{
	DoSplit( nRow, nCol );
}

BSTR CSplitterBase::GetActiveViewType(VARIANT FAR* pvarCol, VARIANT FAR* pvarRow) 
{
	CString strResult;	

	ISplitterWindowPtr ptrs( GetControllingUnknown() );
	if( ptrs )
	{
		int nRow, nCol;
		nRow = nCol = 0;
		ptrs->GetActivePane( &nRow, &nCol );
		
		BSTR bstrType = NULL;
		ptrs->GetViewTypeRowCol( nRow, nCol, &bstrType );
		
		strResult = bstrType;
		::SysFreeString( bstrType );

		*pvarRow = _variant_t( (long)nRow );
		*pvarCol = _variant_t( (long)nCol );
	}	

	return strResult.AllocSysString();
}

void CSplitterBase::ActivateView(long lCol, long lRow, LPCTSTR szType) 
{
	ISplitterWindowPtr ptrs( GetControllingUnknown() );
	if( ptrs )
	{
		if( szType && strlen( szType ) )
			ptrs->ChangeViewType( (int)lRow, (int)lCol, _bstr_t(szType) );		

		POSITION pos = GetFirstViewEntryPosition();
		IUnknown* punkView = NULL;
		while( pos )
		{
			XSplitterEntry* pEntry = GetNextViewEntry( pos );
			if( pEntry->nRow == (int)lRow && pEntry->nCol == (int)lCol )
			{
				punkView = pEntry->punkView;
			}		
		}

		if( punkView )
		{

			CWnd	*pwnd = ::GetWindowFromUnknown( punkView );
			if( pwnd )
			{
				pwnd = pwnd->GetParent();
				if( pwnd )
					pwnd->SetFocus();				
			}						
		}		
	}	
	
}


long CSplitterBase::GetColumnsCount()
{
	int nrows = 0;
	int ncols = 0;
	m_xSplitter.GetRowColCount( &nrows, &ncols );

	return ncols;
}

long CSplitterBase::GetRowCount()
{
	int nrows = 0;
	int ncols = 0;
	m_xSplitter.GetRowColCount( &nrows, &ncols );

	return nrows;
}

long CSplitterBase::GetWidth()
{
	return get_width();
}

long CSplitterBase::GetHeight()
{
	return get_height();
}

long CSplitterBase::GetRowHeight(long lRow)
{
	return get_row_height( lRow );
}

long CSplitterBase::GetColWidth(long lCol)
{
	return get_col_width( lCol );
}

void CSplitterBase::SetRowHeight(long lRow, long lHeight)
{
	set_row_height( lRow, lHeight );
}

void CSplitterBase::SetColWidth(long lCol, long lWidth)
{
	set_col_width( lCol, lWidth );
}

double CSplitterBase::GetViewZoom(long lCol, long lRow)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, lCol,  &punkView );
	if( !punkView )
		return 0;

	double fzoom = ::GetZoom( punkView );

	punkView->Release();	punkView = 0;

	return fzoom;
}

void CSplitterBase::SetViewZoom(long lCol, long lRow, double fZoom)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, lCol,  &punkView );
	if( !punkView )
		return;

	IScrollZoomSitePtr ptrSZS( punkView );
	punkView->Release();	punkView = 0;
	if( ptrSZS == 0 )
		return;

	ptrSZS->SetZoom( fZoom );
}

long CSplitterBase::GetViewClientWidth(long lCol, long lRow)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, lCol,  &punkView );
	if( !punkView )
		return 0;

	IScrollZoomSitePtr ptrSZS( punkView );
	punkView->Release();	punkView = 0;
	if( ptrSZS == 0 )
		return 0;

	SIZE size;
	ptrSZS->GetClientSize( &size );

	return size.cx;
}

long CSplitterBase::GetViewClientHeight(long lCol, long lRow)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, lCol,  &punkView );
	if( !punkView )
		return 0;

	IScrollZoomSitePtr ptrSZS( punkView );
	punkView->Release();	punkView = 0;
	if( ptrSZS == 0 )
		return 0;

	SIZE size;
	ptrSZS->GetClientSize( &size );

	return size.cy;
}

long CSplitterBase::GetViewXScroll(long lCol, long lRow)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, lCol,  &punkView );
	if( !punkView )
		return 0;

	IScrollZoomSitePtr ptrSZS( punkView );
	punkView->Release();	punkView = 0;
	if( ptrSZS == 0 )
		return 0;

	POINT pt;
	ptrSZS->GetScrollPos( &pt );

	return pt.x;
}


long CSplitterBase::GetViewYScroll(long lCol, long lRow)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, lCol,  &punkView );
	if( !punkView )
		return 0;

	IScrollZoomSitePtr ptrSZS( punkView );
	punkView->Release();	punkView = 0;
	if( ptrSZS == 0 )
		return 0;

	POINT pt;
	ptrSZS->GetScrollPos( &pt );

	return pt.y;
}

void CSplitterBase::SetViewScroll(long lCol, long lRow, long lXScroll, long lYScroll)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, lCol,  &punkView );
	if( !punkView )
		return;

	IScrollZoomSitePtr ptrSZS( punkView );
	punkView->Release();	punkView = 0;
	if( ptrSZS == 0 )
		return;

	ptrSZS->SetScrollPos( CPoint( lXScroll, lYScroll ) );
}


long CSplitterBase::get_width()
{
	CRect rc;
	GetClientRect( &rc );
	return rc.Width();
}

long CSplitterBase::get_height()
{
	CRect rc;
	GetClientRect( &rc );
	return rc.Height();
}

long CSplitterBase::get_row_height(long lRow)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( lRow, 0,  &punkView );
	if( !punkView )
		return 0;

	IWindowPtr ptrW( punkView );
	punkView->Release();	punkView = 0;
	if( ptrW == 0 )
		return 0;

	HWND hwnd = 0;
	ptrW->GetHandle( (HANDLE*)&hwnd );
	CRect rc = NORECT;
	::GetClientRect( hwnd, &rc );
	return rc.Height();
}

long CSplitterBase::get_col_width(long lCol)
{
	IUnknown* punkView = 0;
	m_xSplitter.GetViewRowCol( 0, lCol,  &punkView );
	if( !punkView )
		return 0;

	IWindowPtr ptrW( punkView );
	punkView->Release();	punkView = 0;
	if( ptrW == 0 )
		return 0;

	HWND hwnd = 0;
	ptrW->GetHandle( (HANDLE*)&hwnd );
	CRect rc = NORECT;
	::GetClientRect( hwnd, &rc );
	return rc.Width();
}

void CSplitterBase::set_row_height(long lRow, long lHeight)
{

}

void CSplitterBase::set_col_width(long lCol, long lWidth)
{

}

