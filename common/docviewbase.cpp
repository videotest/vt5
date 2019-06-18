#include "stdafx.h"
#include "docviewbase.h"
#include "nameconsts.h"
#include "window5.h"
#include "resource.h"
#include "afxpriv2.h"

#include "ScriptDocInt.h"
#include "frame.h"
#include "math.h"

#include "PropBag.h"


IMPLEMENT_DYNAMIC(CDocTemplateBase, CCmdTargetEx);
//IMPLEMENT_DYNAMIC(CShellDocTemplateBase, CCmdTargetEx);
IMPLEMENT_DYNAMIC(CDocBase, CCmdTargetEx);

BEGIN_INTERFACE_MAP(CDocTemplateBase, CCmdTargetEx)
	INTERFACE_PART(CDocTemplateBase, IID_IDocTemplate, Templ)
	INTERFACE_PART(CDocTemplateBase, IID_INamedObject2, Name)
	INTERFACE_PART(CDocTemplateBase, IID_IRootedObject, Rooted)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CDocTemplateBase, Templ);

/*BEGIN_INTERFACE_MAP(CShellDocTemplateBase, CCmdTargetEx)
	INTERFACE_PART(CShellDocTemplateBase, IID_IShellDocTemplate, ShellTempl)
	INTERFACE_PART(CDocTemplateBase, IID_IRootedObject, Rooted)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CShellDocTemplateBase, ShellTempl);
*/

const char *szFlags = "Flags",
			*szSplitter = "Splitter";




CDocTemplateBase::CDocTemplateBase()
{
	_OleLockApp( this );
	EnableAggregation();

	m_dwFlags = 0;
}

CDocTemplateBase::~CDocTemplateBase()
{
	_OleUnlockApp( this );
}

HRESULT CDocTemplateBase::XTempl::GetDocTemplString( DWORD dwCode, BSTR *pbstr )
{
	_try_nested(CDocTemplateBase, Templ, CreateDocument)
	{
		if( dwCode == -1 )
			*pbstr = pThis->GetDocTemplString().AllocSysString();
		else
			*pbstr = pThis->ExtractDocTemplString( dwCode ).AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDocTemplateBase::XTempl::GetTemplateFlags( DWORD *pdwFlags )
{
	_try_nested(CDocTemplateBase, Templ, GetTemplateFlags)
	{
		*pdwFlags = pThis->GetDocFlags();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDocTemplateBase::XTempl::GetDocResource( HICON *phIcon )
{
	_try_nested(CDocTemplateBase, Templ, GetDocResource)
	{
		if( phIcon )
			*phIcon = ::LoadIcon( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( pThis->GetResourceID() ) );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDocTemplateBase::XTempl::GetSplitterProgId( BSTR *pbstr )
{
	_try_nested(CDocTemplateBase, Templ, GetDocResource)
	{
		_bstr_t	bstr( pThis->GetSplitterProgId() );
		*pbstr = bstr.copy();

		return S_OK;
	}
	_catch_nested;
}

CString CDocTemplateBase::GetDocTemplateName()
{							   
	return ExtractDocTemplString( CDocTemplate::docName );
}

CString CDocTemplateBase::ExtractDocTemplString( DWORD dwCode )
{
	CString	strSource = GetDocTemplString();
	CString	strString;

	AfxExtractSubString( strString, strSource, (int)dwCode );

	return strString;
}

//initialize the document template
void CDocTemplateBase::OnChangeParent()
{
	//m_strSplitterProgID = ::GetValueString( GetAppUnknown(), GetDocTemplateName(), szSplitter, szStdSplitterProgID );
	//m_dwFlags = ::GetValueInt( GetAppUnknown(), GetDocTemplateName(), szFlags, m_dwFlags );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////
//
BEGIN_INTERFACE_MAP(CDocBase, CCmdTargetEx)
	INTERFACE_PART(CDocBase, IID_IDocument, Doc)
	INTERFACE_PART(CDocBase, IID_INumeredObject, Num)
	INTERFACE_PART(CDocBase, IID_IRootedObject, Rooted)
	INTERFACE_PART(CDocBase, IID_IEventListener, EvList)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CDocBase, Doc);

CDocBase::CDocBase()
{
	_OleLockApp( this );
}

CDocBase::~CDocBase()
{
	_OleUnlockApp( this );
}

void CDocBase::OnFinalRelease()
{
//	UnRegister( GetControllingUnknown() );

	CCmdTargetEx::OnFinalRelease();
}

HRESULT CDocBase::XDoc::Init( IUnknown *punkDocSite, IUnknown *punkDocTempl, IUnknown *punkApp )
{
	_try_nested(CDocBase, Doc, Init)
	{
		pThis->Init();

		return S_OK;
	}
	_catch_nested;
}

void CDocBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szEventActiveObjectChange ) )
	{
		OnActivateObject( punkFrom, punkHit );
	}
}

void CDocBase::Init()
{
	Register( GetControllingUnknown() );
}

HRESULT CDocBase::XDoc::GetDocFlags( DWORD *pdwFlags )
{
	_try_nested(CDocBase, Doc, GetDocFlags)
	{
		*pdwFlags = pThis->GetDocFlags();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CDocBase::XDoc::CreateNew()
{
	_try_nested(CDocBase, Doc, CreateNew)
	{
		sptrIFileDataObject2	sptrF( this );

		if( sptrF->InitNew() != S_OK )
			return E_FAIL;

		if( !pThis->OnNewDocument() )
			return E_FAIL;

		sptrF->SetModifiedFlag( FALSE );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CDocBase::XDoc::LoadNativeFormat( BSTR bstr )
{
	_try_nested(CDocBase, Doc, LoadNativeFormat)
	{
		CString	s( bstr );

		if( pThis->OnOpenDocument( s ) )
			return S_OK;
		else
			return E_FAIL;
	}
	_catch_nested;
}
HRESULT CDocBase::XDoc::SaveNativeFormat( BSTR bstr )
{
	_try_nested(CDocBase, Doc, SaveNativeFormat)
	{
		CString	s( bstr );

		if( pThis->OnSaveDocument( s ) )
			return S_OK;
		else
			return E_FAIL;
	}
	_catch_nested;
}

void CDocBase::OnActivateObject( IUnknown *punkDataObject, IUnknown *punkView )
{
	//UpdateAllViews( szEventActiveObjectChange, punkDataObject );
	_bstr_t	bstrUpdateHint = szEventActiveObjectChange;

	if( !CheckInterface( punkView, IID_IView ) )
		return;

	sptrIView	sptrV( punkView );
	
	sptrV->OnUpdate( bstrUpdateHint, punkDataObject );
}

void CDocBase::UpdateAllViews( const char *psz, IUnknown *punkDataObject )
{
	long	lpos;
	IUnknown	*punkView;

	sptrIDocumentSite	sptrS( GetControllingUnknown() );

	sptrS->GetFirstViewPosition( &lpos );
	_bstr_t	bstrUpdateHint = psz;

	while( lpos )
	{
		sptrS->GetNextView( &punkView, &lpos );

		sptrIView	sptr( punkView );
		sptr->OnUpdate( bstrUpdateHint, punkDataObject );
		punkView->Release();
	}
}
//////////////////////////////////////////////////////////////////////////////////////
//CViewImpl
IMPLEMENT_UNKNOWN_BASE(CViewImpl, View);

HRESULT	CViewImpl::XView::Init( IUnknown *punkDoc, IUnknown *punkSite )
{
	_try_nested_base(CViewImpl, View, Init)
	{
		pThis->m_sptrIDocument = punkDoc;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CViewImpl::XView::GetDocument( IUnknown **ppunkDoc )
{
	METHOD_PROLOGUE_BASE(CViewImpl, View)
	*ppunkDoc = NULL;

	if( pThis->m_sptrIDocument )
	{
		*ppunkDoc = pThis->m_sptrIDocument;
		pThis->m_sptrIDocument->AddRef();
	}
	return S_OK;
}

HRESULT CViewImpl::XView::OnUpdate( BSTR bstr, IUnknown *punkObject )
{
	_try_nested_base(CViewImpl, View, GetDocument)
	{
		CString	str = bstr;
		pThis->OnUpdate( str, punkObject );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CViewImpl::XView::GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch )
{
	METHOD_PROLOGUE_BASE(CViewImpl, View)
	CString	str = bstrObjectType;
	*pdwMatch = pThis->GetMatchType( str );

	return S_OK;
}

HRESULT CViewImpl::XView::GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags /*ObjectFlags*/)
{
	METHOD_PROLOGUE_BASE(CViewImpl, View)
	CString	str = bstrObjectType;
	*pdwFlags = pThis->GetObjectFlags( str );

	return S_OK;
}
HRESULT CViewImpl::XView::DoDragDrop( DWORD *pdwDropEffect )
{
	_try_nested_base(CViewImpl, View, DoDragDrop)
	{	
		*pdwDropEffect = pThis->DoDragDrop();
		return S_OK;
	}
	_catch_nested;
}


HRESULT CViewImpl::XView::GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame)
{
	METHOD_PROLOGUE_BASE(CViewImpl, View)
	pThis->GetMultiFrame(ppunkObject, bDragFrame==TRUE);
	return S_OK;
}

HRESULT CViewImpl::XView::MoveDragFrameToFrame()
{
	_try_nested_base(CViewImpl, View, MoveDragFrameToFrame)
	{	
		pThis->MoveDragFrameToFrame();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CViewImpl::XView::GetDroppedDataObject(IUnknown** punkDO)
{
	_try_nested_base(CViewImpl, View, GetDroppedDataObject)
	{	
		if(punkDO)
			*punkDO = pThis->GetDroppedDataObject();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CViewImpl::XView::OnActivateView( BOOL bActivate, IUnknown *punkOtherView )
{
	_try_nested_base(CViewImpl, View, OnActivateView)
	{	
		pThis->OnActivateView( bActivate != false, punkOtherView );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CViewImpl::XView::GetFirstVisibleObjectPosition( long *plpos )
{
	_try_nested_base(CViewImpl, View, GetFirstVisibleObjectPosition)

	*plpos = (long)pThis->GetFisrtVisibleObjectPosition();
	return S_OK;

	_catch_nested;
}
HRESULT CViewImpl::XView::GetNextVisibleObject( IUnknown ** ppunkObject, long *plPos )
{
	_try_nested_base(CViewImpl, View, GetNextVisibleObject)

	POSITION	pos = (POSITION)*plPos;
	*ppunkObject = pThis->GetNextVisibleObject( pos );
	*plPos = (long)pos;
	return S_OK;

	_catch_nested;
}

HRESULT CViewImpl::XView::GetViewFlags(DWORD *pdwViewFlags)
{
	_try_nested_base(CViewImpl, View, GetViewFlags)
	if (pdwViewFlags)
		*pdwViewFlags = pThis->OnGetViewFlags();
	return S_OK;
	_catch_nested;
}


IDocument *CViewImpl::GetDocument()
{
	if( m_sptrIDocument )
		m_sptrIDocument->AddRef();
	return m_sptrIDocument;
}

void CViewImpl::OnUpdate( const char *szHint, IUnknown *punkObject )
{
	if( !strcmp( szHint, szEventActiveObjectChange ) )
	{
		OnActivateObject( punkObject );
	}	
}

void CViewImpl::OnActivateObject( IUnknown *punkDataObject )
{
}



CScrollBar *CViewImpl::GetScrollBarCtrl( int sbCode ) const
{
	sptrIScrollZoomSite	sptrSite( (IUnknown*)this );

	HWND	hwndSB = 0;
	sptrSite->GetScrollBarCtrl( sbCode, (HANDLE*)&hwndSB );
	CScrollBar	*pctrl = (CScrollBar*)CScrollBar::FromHandle( hwndSB );

	return pctrl;
}

DWORD CViewImpl::GetMatchType( const char *szType )
{
	return mvNone;
}

/////////////////////////////////////////////////////////////////////////////////////////
//activate action
bool CActionShowViewBase::Invoke()
{
	if( !m_punkTarget )
		return false;

	AfterInvoke(); //function name is a historical feature ;)

	sptrIFrameWindow	sptrF( m_punkTarget );

	IUnknown	*punkSplitter = 0;

	sptrF->GetSplitter( &punkSplitter );
	sptrISplitterWindow	sptrS( punkSplitter );

	punkSplitter->Release();
		
	int	nRow, nCol;

	_bstr_t	bstrViewProgID = GetViewProgID();
	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->ChangeViewType( nRow, nCol, bstrViewProgID );

	IUnknown	*punkView = 0;
	sptrS->GetViewRowCol( nRow, nCol, &punkView );

	if( !punkView )return true;

	IViewPtr			ptrView( punkView );
	IDataContext2Ptr	ptrContext( punkView );

	punkView->Release();

	if( ptrView == 0 || ptrContext == 0 )
		return false;



	//gathering all available types
	CStringArray arTypes;
	CStringArray arViewTypes;
	{
		sptrIApplication spApp( ::GetAppUnknown() );
		if( spApp )
		{
			sptrIDataTypeInfoManager	sptrT( spApp );
			if( sptrT )
			{
				long	nTypesCount = 0;
				sptrT->GetTypesCount( &nTypesCount );
				for( long nType = 0; nType < nTypesCount; nType++ )
				{
					BSTR	bstr = 0;
					sptrT->GetType( nType, &bstr );

					if( bstr )
					{
						CString strType = bstr;
						
						DWORD dwMatch = 0;						
						ptrView->GetMatchType( bstr, &dwMatch );
						if( dwMatch >= mvFull )					
							arViewTypes.Add( strType );

						arTypes.Add( strType );
						::SysFreeString( bstr );
					}
				}
			}
		}
	}

	if( arTypes.GetSize() < 1 || arViewTypes.GetSize() < 1 )
		return true;

	//ask view m.b. it have attached object?
	
	for( int i=0;i<arViewTypes.GetSize();i++ )
	{

		_bstr_t bstrType = (LPCSTR)arViewTypes[i];
		long lPos = 0;
		ptrContext->GetFirstSelectedPos( bstrType, &lPos );
		
		while( lPos )
		{
			IUnknown* punk = 0;
			ptrContext->GetNextSelected( bstrType, &lPos, &punk );
			if( punk )
			{
				punk->Release();	punk = 0;
				return true;				
			}
		}
	}

	//View doesn't have active object, type supported by view. Try to activate last active type
	for( i=0;i<arViewTypes.GetSize();i++ )
	{
		_bstr_t bstrType = (LPCSTR)arViewTypes[i];
		IUnknown* punk = 0;
		ptrContext->GetLastActiveObject( bstrType, &punk );
		if( punk )
		{
			ptrContext->SetObjectSelect( punk, 1 );
			punk->Release();
			return true;
		}
	}

	//Still here? No active, last active object, etc... Try to activate first object from context, supp by view
	for( i=0;i<arViewTypes.GetSize();i++ )
	{
		_bstr_t bstrType = (LPCSTR)arViewTypes[i];
		IUnknown* punk = 0;
		long lPos = 0;
		ptrContext->GetFirstObjectPos( bstrType, &lPos );

		while( lPos )
		{
			IUnknown* punk = 0;
			ptrContext->GetNextObject(bstrType, &lPos, &punk );
			if( punk )
			{
//				ptrContext->SetObjectSelect( punk, 1 );
				ptrContext->SetActiveObject(bstrType, punk, aofActivateDepended);
				punk->Release();
				return true;
			}
		}
		
	}




/*	CPtrList	m_listActiveObjects;

	long	lTypeCount = 0;
	ptrContext->GetObjectTypeCount( &lTypeCount );

	for( long lType = 0; lType < lTypeCount; lType++ )
	{
		BSTR	bstrType = 0;
		ptrContext->GetObjectTypeName( lType, &bstrType );

		long	lpos = 0;
		ptrContext->GetFirstSelectedPos( bstrType, &lpos );

		while( lpos )
		{
			IUnknown	*punkObject = 0;
			ptrContext->GetNextSelected( bstrType, &lpos, &punkObject );



		}
	}
	long	lpos;*/


	
	return true;
}


bool CActionShowViewBase::IsChecked()
{
	sptrIFrameWindow	sptrF( m_punkTarget );

	IUnknown	*punkSplitter = 0;

	sptrF->GetSplitter( &punkSplitter );
	if( !punkSplitter )
		return false;

	sptrISplitterWindow	sptrS( punkSplitter );

	punkSplitter->Release();	punkSplitter = 0;
		
	int	nRow, nCol;
	BSTR	bstrView;

	_bstr_t	bstrViewProgID = GetViewProgID();
	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->GetViewTypeRowCol( nRow, nCol, &bstrView );

	_bstr_t	bstrCurrentView = bstrView;
	::SysFreeString(bstrView );

	return bstrViewProgID == bstrCurrentView;

}

//////////////////////////////////////////////////////////////////////////////////////
//CShellDocTemplateImpl
IMPLEMENT_UNKNOWN_BASE(CShellDocTemplateImpl, ShellTempl);

HRESULT CShellDocTemplateImpl::XShellTempl::GetFileFilterByFile(BSTR bstr, IUnknown **punk)
{
	_try_nested_base(CShellDocTemplateImpl, ShellTempl, GetFileFilterByFile)
	{
		CString strName(bstr);
		*punk = (IUnknown*)pThis->GetFileFilter(strName, true);
		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////////////////
//CViewBase	-  base class for views

//////////////////////////////////////////////////////////////////////////////////////
//IPrintView implement
IMPLEMENT_UNKNOWN_BASE(CPrintImpl, PrintView);

HRESULT CPrintImpl::XPrintView::GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX )
{
	_try_nested_base(CPrintImpl, PrintView, GetPrintWidth)
	{
		*pbContinue = pThis->GetPrintWidth( nMaxWidth, *pnReturnWidth, nUserPosX, *pnNewUserPosX );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintImpl::XPrintView::GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY )
{
	_try_nested_base(CPrintImpl, PrintView, GetPrintHeight)
	{
		*pbContinue = pThis->GetPrintHeight( nMaxHeight, *pnReturnHeight, nUserPosY, *pnNewUserPosY );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintImpl::XPrintView::Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags )
{
	_try_nested_base(CPrintImpl, PrintView, Print)
	{
		pThis->Print( hdc, rectTarget, nUserPosX, nUserPosY, nUserPosDX, nUserPosDY, bUseScrollZoom==TRUE, dwFlags );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPrintImpl::XPrintView::GetPrintFlags(DWORD *pdwFlags)
{
	_try_nested_base(CPrintImpl, PrintView, GetPrintFlags)
	{
		*pdwFlags = 0;
		if (pThis->CanSaveToImage())
			*pdwFlags |= PrintView_CanSaveToImage;
		return S_OK;
	}
	_catch_nested;
}

bool CViewBase::GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX )
{
	CRect rc = NORECT;

	GetWindowRect(rc);
	
	IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
	if( ptrZ )
	{
		CSize size;
		ptrZ->GetClientSize( &size );
		double fZoom = 1.0;
		ptrZ->GetZoom( &fZoom );
		rc = CRect( 0, 0, int((double)size.cx * fZoom), int((double)size.cy * fZoom) );

	}

	long nWidth = rc.Width();
	bool bContinue = false;
	if(nWidth > nUserPosX + nMaxWidth)
	{
		nReturnWidth = nMaxWidth;
		nNewUserPosX +=  nReturnWidth;
		bContinue = true;
	}
	else
	{
		nReturnWidth = nWidth - nUserPosX;
		nNewUserPosX = nWidth;
	}
	return bContinue;
}

bool CViewBase::GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY )
{
	CRect rc = NORECT;
	GetWindowRect(rc);
	
	IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
	if( ptrZ )
	{
		CSize size;
		ptrZ->GetClientSize( &size );
		double fZoom = 1.0;
		ptrZ->GetZoom( &fZoom );
		rc = CRect( 0, 0, int((double)size.cx * fZoom), int((double)size.cy * fZoom) );

	}
	

	long nHeight = rc.Height();
	bool bContinue = false;
	if(nHeight > nUserPosY + nMaxHeight)
	{
		nReturnHeight = nMaxHeight;
		nNewUserPosY +=  nReturnHeight;
		bContinue = true;
	}
	else
	{
		nReturnHeight = nHeight - nUserPosY;
		nNewUserPosY = nHeight;
	}
	return bContinue;
}

void CViewBase::Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags )
{
	//do nothing
	CString strMessage, strFormat;
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	strFormat.LoadString( IDS_WARNING_NOT_SUPPORT_PRINT );

	strMessage.Format( strFormat, m_sUserName );
	
	CRect rcBounds = rectTarget;
	
	CDC* pdc = CDC::FromHandle(hdc);	
	if( pdc == NULL)
		return;

	CPoint ptOfset = rectTarget.TopLeft( );
	

	CBrush	brush( RGB(245, 245, 245) );
	//brush.CreateHatchBrush( HS_FDIAGONAL, RGB(0, 0, 0) );	
	brush.UnrealizeObject();

	pdc->SetBkColor( ::GetSysColor( COLOR_3DFACE ) );
	pdc->SetBkMode( TRANSPARENT );
	
	//dc.SetBrushOrg( 7-ptOfset.x%8, 7-ptOfset.y%8 );
	pdc->SelectObject( &brush );
	pdc->Rectangle(rcBounds);
	
	pdc->SelectStockObject( NULL_BRUSH );

	//CRect rcCalc;
	//VERIFY( dc.DrawText( strMessage, &rcCalc, DT_CALCRECT | DT_WORDBREAK ) );

	CRect rcText = rectTarget;
	/*
	rcText.right = rcText.left + rcCalc.Width();
	rcText.bottom = rcText.top + rcCalc.Height();	
	*/

	CFont* pFont = CFont::FromHandle( (HFONT)GetStockObject(ANSI_VAR_FONT) );
	CFont* pOldFont = pdc->SelectObject( pFont );
	COLORREF clrOldText = pdc->SetBkColor( RGB( 0, 0, 0 ) );

	VERIFY( pdc->DrawText( strMessage, &rcText, DT_LEFT | DT_WORDBREAK ) );

	pdc->SelectObject( pOldFont );
	pdc->SetBkColor( clrOldText );



	
}


	
CViewBase::CViewBase()
{
	m_bEventRegistered = false;

	m_nIDEventRepaintSelection = 0;
	m_nIDTimerRepaintSelection = 0;

	m_pframeRuntime = RUNTIME_CLASS(CFrame);
	m_pframe = 0;
	m_pframeDrag = 0;

	m_sName = "";
	m_nDrawFrameStage = -1;
}

CViewBase::~CViewBase()
{
	if(m_bEventRegistered)
	{
		IUnknown* punkDoc = GetDocument();
		UnRegister( punkDoc );
		if (punkDoc)
			punkDoc->Release();
	}

	if( m_pframeDrag )
		delete m_pframeDrag;
	if( m_pframe )
		delete m_pframe;
	
}

void CViewBase::OnActivateObject( IUnknown *punkDataObject )
{
	AttachActiveObjects();
/*	if( GetSafeHwnd() )
		Invalidate();*/
	IViewSitePtr	ptrV( GetControllingUnknown() );
	if( ptrV )ptrV->UpdateObjectInfo();

	::FireEvent(GetAppUnknown(), szEventActivateObject, GetControllingUnknown(), punkDataObject );	
}

void CViewBase::AttachActiveObjects()
{
}

BOOL CViewBase::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//if( message == WM_SETCURSOR )
	{
		MSG	msg;
		msg.hwnd = GetSafeHwnd();
		msg.message = message;
		msg.wParam = wParam;
		msg.lParam = lParam;

		if( ProcessMessage( &msg, pResult) )
			return true;

		if( m_pframe && m_pframe->CMouseImpl::OnWndMsg( message, wParam, lParam, pResult ))
			return true;
	}

//[AY]
//this part of code moved from CViewBase::PreTranslateMessage
//why child window have to receive parent notifiction?

	/*if( m_pframe )
	{
		ASSERT( m_pframe->m_pView == this );
		
		if( m_pframe->CMouseImpl::OnWndMsg( message, wParam, lParam, pResult ))
			return true;
	}

	//creata a message
	MSG	msg;
	ZeroMemory( &msg, sizeof( msg ) );

	msg.message = message;
	msg.hwnd = GetSafeHwnd();
	msg.wParam = wParam;
	msg.lParam = lParam;

	if( ProcessMessage( &msg ) )
		return true;*/


	return CWnd::OnWndMsg( message, wParam, lParam, pResult );
}

IUnknown* CViewBase::GetDroppedDataObject()
{
	return m_dropTarget.m_punkDroppedDataObject;
}

BEGIN_DISPATCH_MAP(CViewBase, CCmdTarget)
	//{{AFX_DISPATCH_MAP(CViewBase)
	DISP_FUNCTION(CViewBase, "GetProperty", disp_GetProperty, VT_VARIANT, VTS_BSTR )
	DISP_FUNCTION(CViewBase, "SetProperty", disp_SetProperty, VT_BOOL, VTS_BSTR VTS_PVARIANT)	
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// {A1389906-3568-4e6b-8E4A-A34786A2B74D}
static const IID IID_IViewDisp = 
{ 0xa1389906, 0x3568, 0x4e6b, { 0x8e, 0x4a, 0xa3, 0x47, 0x86, 0xa2, 0xb7, 0x4d } };


BEGIN_INTERFACE_MAP(CViewBase, CCmdTarget)
	INTERFACE_PART(CViewBase, IID_IWindow, Wnd)
	INTERFACE_PART(CViewBase, IID_IWindow2, Wnd2)
	INTERFACE_PART(CViewBase, IID_IRootedObject, Rooted)
	INTERFACE_PART(CViewBase, IID_IView, View)
	INTERFACE_PART(CViewBase, IID_IEventListener, EvList)
	INTERFACE_PART(CViewBase, IID_IPrintView, PrintView)
	INTERFACE_PART(CViewBase, IID_IPrintView2, PrintView)
	INTERFACE_PART(CViewBase, IID_INamedObject2, Name)
	INTERFACE_PART(CViewBase, IID_IPersist, Persist)
	INTERFACE_PART(CViewBase, IID_IHelpInfo, Help)
	INTERFACE_PART(CViewBase, IID_IViewDispatch, ViewDispatch)
	INTERFACE_PART(CViewBase, IID_IViewDisp, Dispatch)	
	INTERFACE_PART(CViewBase, IID_IView2, View)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CViewBase, ViewDispatch)

BEGIN_MESSAGE_MAP(CViewBase, CWnd)
	//{{AFX_MSG_MAP(CViewBase)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_SETCURSOR()
	ON_WM_HELPINFO()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()


int CViewBase::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//m_dataSource.Init( this );

	if( m_pframeRuntime )
	{
		m_pframe = (CFrame*)m_pframeRuntime->CreateObject();
		m_pframeDrag = (CFrame*)m_pframeRuntime->CreateObject();
		m_pframe->Init(this, true);
		m_pframeDrag->Init(this, false);
	}
	//drag frame always think that it is dragging
	m_dropTarget.Init( this );

	m_nIDEventRepaintSelection = 777;
	m_nIDTimerRepaintSelection = ::SetTimer( GetSafeHwnd(), m_nIDEventRepaintSelection, 100, 0 );


	IUnknown *punkDoc = GetDocument();
	Register( punkDoc );
	if (punkDoc)
		punkDoc->Release();

	m_bEventRegistered = true;

	AttachActiveObjects();

	IViewSitePtr	ptrV( GetControllingUnknown() );
	if( ptrV != 0 )ptrV->UpdateObjectInfo();
	
	return 0;
}


BOOL CViewBase::PreTranslateMessage(MSG* pMsg) 
{
	return CWnd::PreTranslateMessage(pMsg);
}

BOOL CViewBase::OnEraseBkgnd(CDC* pDC) 
{
	BOOL bResult = FALSE;   

	LOCAL_RESOURCE;

	CBrush *pPatBrush;
	CBitmap bm;
	bm.LoadBitmap(IDB_PATTERN_VIEW);

	//sergey 28/11/06
	//pPatBrush = new CBrush( BLACK_BRUSH );
	pPatBrush = new CBrush( &bm );
	//end
	pPatBrush->UnrealizeObject();

	CPoint p;
	CSize	size;

	//get the client size
	double fZoom = 0;
	CPoint ptScrollPos(0, 0);
	{
		sptrIScrollZoomSite	sptrS( GetControllingUnknown() );
		sptrS->GetClientSize( &size );
		//sergey 28/11/06
		//size.cx=800;
		//size.cy=600;
		//end
		sptrS->GetScrollPos( &p );

		sptrS->GetScrollPos(&ptScrollPos);
		sptrS->GetZoom(&fZoom);
	}
   //sergey 28/11/06
	//pDC->SetBrushOrg( 7-p.x%2, 7-p.y%2 );
	pDC->SetBrushOrg( 7-p.x%8, 7-p.y%8 );
	//end


	CRect rect;
	GetClientRect( rect );

	//size = ConvertToWindow( GetControllingUnknown(), size );

	size.cx = int(floor(size.cx*fZoom-ptScrollPos.x + .5));
	size.cy = int(floor(size.cy*fZoom-ptScrollPos.y + .5));

	bool bCenterWindow = ::SendMessage( m_hWnd, WM_MESSAGE_ENABLE_CENTER, 0, 0 );


	if( bCenterWindow )
	{
		CRect rc( 0, 0, size.cx, size.cy );

		if( rc.Width() < rect.Width() )
			rc.OffsetRect( rect.Width() / 2 - rc.Width() / 2, 0 );

		if( rc.Height() < rect.Height() )
			rc.OffsetRect( 0, rect.Height() / 2 - rc.Height() / 2 );

		CRect rc1( 0,0, rc.left, rect.bottom );
		CRect rc2( rc.right,0, rect.right, rect.bottom );

		CRect rc3( rc.left, 0, rc.right, rc.top );
		CRect rc4( rc.left, rc.bottom, rc.right, rect.bottom );

		if (!rc1.IsRectEmpty())
		{
			pDC->FillRect(rc1, pPatBrush);
			bResult = TRUE;
		}
		if (!rc2.IsRectEmpty())
		{
			pDC->FillRect(rc2, pPatBrush);
			bResult = TRUE;
		}

		if (!rc3.IsRectEmpty())
		{
			pDC->FillRect(rc3, pPatBrush);
			bResult = TRUE;
		}

		if (!rc4.IsRectEmpty())
		{
			pDC->FillRect(rc4, pPatBrush);
			bResult = TRUE;
		}

	}
	else
	{
		rect.left = size.cx;
		if (!rect.IsRectEmpty())
		{
			pDC->FillRect(rect, pPatBrush);
			bResult = TRUE;
		}

		rect.left = 0;
		rect.right = size.cx;
		rect.top = size.cy;

		if (!rect.IsRectEmpty())
		{
			pDC->FillRect(rect, pPatBrush);
			bResult = TRUE;
		}
	}

	delete pPatBrush;
	return bResult; 
}

bool CViewBase::DoAttachDragDropObject( IUnknown *punkDataObject, CPoint point )
{
	return false;
}

bool CViewBase::DoDrop( CPoint point )
{
	return false;
}

bool CViewBase::DoDrag( CPoint point )
{
	if( m_pframeDrag->GetBoundRect().TopLeft() == point )
		return true;

	m_pframeDrag->Redraw();
	m_pframeDrag->SetFrameOffset(point);
	m_pframeDrag->Redraw( false, true );

	return true;

}

IUnknown *CViewBase::GetDragDropObject( CPoint point, CRect *prect )
{
	return 0;
}

DWORD CViewBase::DoDragDrop()
{
	if( !m_pframe )return DROPEFFECT_NONE;
	return m_pframe->m_dataSource.DoDragDrop();
}

void CViewBase::GetMultiFrame(IUnknown **ppunkObject, bool bDragFrame)
{
	if( !m_pframe )
	{
		*ppunkObject = 0;
		return;
	}

	IUnknown* punk = 0; 
	if(bDragFrame)
		punk = m_pframeDrag->GetControllingUnknown();
	else
		punk = m_pframe->GetControllingUnknown();
	punk->AddRef();
	if(ppunkObject)
		*ppunkObject = punk;
	else
		punk->Release();
}

void CViewBase::MoveDragFrameToFrame()
{
	if( !m_pframe )return;
	m_pframe->Redraw();

	m_pframe->EmptyFrame();
	m_pframe->SetBoundRect(m_pframeDrag->GetBoundRect());
	int nObjects = m_pframeDrag->GetObjectsCount();

	TRACE( "MoveDragFrameToFrame ");

//	m_pframe->m_ptrSelectedObjects.SetSize(nObjects);
	for(int i = nObjects - 1; i >= 0; i--)
	{
		m_pframe->SelectObject(m_pframeDrag->m_ptrSelectedObjects[i], true);
//		m_pframe->m_ptrSelectedObjects.SetAt(i, m_pframeDrag->m_ptrSelectedObjects[i]);
//		m_pframe->m_ptrSelectedObjects[i]->AddRef();
		m_pframe->m_arrObjectsOffsets.Add(m_pframeDrag->m_arrObjectsOffsets[i]);

		//TRACE( "%d ", GetObjectKey( m_pframe->m_ptrSelectedObjects[i] ) );
	}
	TRACE( "\n");

	m_pframe->m_bDragDropActive = false;
	if(nObjects == 1)
	{
		IUnknown* punkObj = m_pframeDrag->GetObjectByIdx(0);
		if(punkObj)
		{
			CRect rc = GetObjectRect(punkObj);
			m_pframe->SetRect(rc);
			punkObj->Release();
		}
	}
	else
		m_pframe->SetRect(NORECT);
	
	m_pframe->Redraw();
	m_pframeDrag->EmptyFrame();
	//UpdateWindow();
}

void CViewBase::DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state )
{
}

void CViewBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !m_pframe )return;
	_try(CViewBase, OnNotify)
	{
		if( !strcmp( pszEvent, szEventChangeObjectList ) )
		{
			long	lHint=*(long*)pdata;
			if( lHint == cncRemove || lHint == cncReset )
			{
				m_pframe->UnselectObject( punkHit );
			}
			else
			if(lHint == cncChange)
			{
				if( m_pframe->CheckObjectInSelection( punkHit ) )
				{
					m_pframe->UnselectObject( punkHit );
					m_pframe->SelectObject( punkHit, false );
				}
			}
		}	
	}
	_catch;
}

void CViewBase::OnTimer(UINT_PTR nIDEvent) 
{
	if( !m_pframe )return;
	if( nIDEvent == m_nIDEventRepaintSelection )
	{
		if( m_pframeDrag->GetObjectsCount() > 0 )
			return;

		CClientDC	dc( this );
		dc.SetROP2( R2_XORPEN );
		dc.SelectStockObject( WHITE_PEN );


		IUnknown *punkObject = m_pframe->GetActiveObject();
		CRect	rect;
		GetClientRect( &rect );

		if( punkObject )
		{
			DoDrawFrameObjects( punkObject, dc, rect, 0, 0, odsAnimation );
			punkObject->Release();
		}
	}
	
	CWnd::OnTimer(nIDEvent);
}

/////////////////////////////////////////////////////////////////////////////////////////
HRESULT CViewBase::XViewDispatch::GetDispIID( GUID* pguid )
{
	METHOD_PROLOGUE_EX(CViewBase, ViewDispatch)

	if( !pguid ) return E_INVALIDARG;

	*pguid = pThis->get_view_dispiid();

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////////////
GUID CViewBase::get_view_dispiid( )
{
	return IID_IViewDisp;
}

/////////////////////////////////////////////////////////////////////////////////////////
VARIANT CViewBase::disp_GetProperty( LPCTSTR str_name )
{
	_variant_t var;
	INamedPropBagPtr ptr_view( GetControllingUnknown() );
	if( ptr_view )
		ptr_view->GetProperty( _bstr_t( str_name ), &var );

	return var.Detach();
}

/////////////////////////////////////////////////////////////////////////////////////////
BOOL CViewBase::disp_SetProperty( LPCTSTR str_name, VARIANT FAR* pvar )
{
	if( !pvar )
		return false;

	INamedPropBagPtr ptr_view( GetControllingUnknown() );
	if( ptr_view == 0 )
		return false;

	return ( S_OK == ptr_view->SetProperty( _bstr_t( str_name ), *pvar ) );
}




/////////////////////////////////////////////////////////////////////////////////////////
//the preview site base class implementation.
//The main class should draw the objects from Preview Site object הרûו
//CPreviewSiteImpl 
CPreviewSiteImpl::CPreviewSiteImpl()
{
	m_nCurrentStage = 0;
	m_nCurrentPosition = 0;
	m_bProcessPreview = false;

	m_fCurrentZoom = 0;
	m_nCurrentScroll = CPoint( 0, 0 );

	m_nMaxProgress = 0;
}

CPreviewSiteImpl::~CPreviewSiteImpl()
{
	if( m_bProcessPreview )
		DoLeavePreviewMode();
	ASSERT( m_listObjects.GetCount()==0 );
}

void CPreviewSiteImpl::DoEnterPreviewMode()
{
	m_bProcessPreview = true;
	m_nCurrentStage = 0;
	m_nCurrentPosition = 0;

	if( CheckInterface( Unknown(), IID_IScrollZoomSite ) )
	{
		m_fCurrentZoom =  GetZoom( Unknown() );
		m_nCurrentScroll =  GetScrollPos( Unknown() );
	}
}

void CPreviewSiteImpl::DoLeavePreviewMode()
{
	m_nCurrentStage = 0;
	m_nCurrentPosition = 0;

	m_bProcessPreview = false;

	POSITION	pos = GetFisrtPreviewPosition();

	CWnd	*pwnd = GetTargetWindow();

	while( pos )
	{
		IUnknown	*punkObject = GetNextObject( pos );
		

		if( CheckInterface( Unknown(), IID_IScrollZoomSite ) )
		{
			CRect	rect = GetPreviewObjectRect( punkObject );
			rect = ConvertToWindow( Unknown(), rect );
			if( pwnd->GetSafeHwnd() )
				pwnd->InvalidateRect( rect );
		}
	}

	pos = m_listObjects.GetHeadPosition();

	while( pos )
		m_listObjects.GetNext( pos )->Release();
	m_listObjects.RemoveAll();
}

void CPreviewSiteImpl::DoTerminatePreview()
{
}

void CPreviewSiteImpl::DoAddPreviewObject( IUnknown *punkObject )
{
	punkObject->AddRef();
	m_listObjects.AddTail( punkObject );
}

void CPreviewSiteImpl::_PreviewStep( int nNewPos )
{
	POSITION	pos = GetFisrtPreviewPosition();

	CWnd	*pwnd = GetTargetWindow();

	while( pos )
	{
		IUnknown	*punkObject = GetNextObject( pos );
		/*CRect	rectOld = GetPreviewObjectRect( punkObject, m_nCurrentPosition );
		CRect	rectNew = GetPreviewObjectRect( punkObject, nNewPos );

		CRect	rectInvalidate;

		rectInvalidate = rectNew;

		//!!tempo
		rectInvalidate.top = rectOld.bottom;

		rectInvalidate.left = int( ::floor( rectInvalidate.left*m_fCurrentZoom-m_nCurrentScroll.x ) );
		rectInvalidate.top = int( ::floor( rectInvalidate.top*m_fCurrentZoom-m_nCurrentScroll.y ) );
		rectInvalidate.right = int( ::ceil( rectInvalidate.right*m_fCurrentZoom-m_nCurrentScroll.x ) );
		rectInvalidate.bottom = int( ::ceil( rectInvalidate.bottom*m_fCurrentZoom-m_nCurrentScroll.y ) );

		if( pwnd->GetSafeHwnd() )
		{
			pwnd->InvalidateRect( rectInvalidate );
			pwnd->UpdateWindow();
		}*/
	}

	m_nCurrentPosition = nNewPos;
}

IUnknown	*CPreviewSiteImpl::GetFirstPreviewObject()
{
	POSITION	pos = GetFisrtPreviewPosition();

	if( !pos )
		return 0;
	return GetNextObject( pos );
}

void CPreviewSiteImpl::GetOperationInfo( IUnknown *punkLong )
{
	if( punkLong )
	{
		ILongOperationPtr	ptrLong( punkLong );
		ptrLong->GetNotifyRanges( 0, &m_nMaxProgress );
	}
}

//interface implementation
IMPLEMENT_UNKNOWN_BASE(CPreviewSiteImpl, Preview);



HRESULT CPreviewSiteImpl::XPreview::InitPreview( IUnknown *punkLoog )
{
	_try_nested_base(CPreviewSiteImpl, Preview, InitPreview)
	{
		pThis->GetOperationInfo( punkLoog );
		pThis->DoEnterPreviewMode();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPreviewSiteImpl::XPreview::AddPreviewObject( IUnknown *punkObject )
{
	_try_nested_base(CPreviewSiteImpl, Preview, InitPreview)
	{
		pThis->DoAddPreviewObject( punkObject );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPreviewSiteImpl::XPreview::ProgressPreview( int nStage, int nProgress )
{
//	_try_nested_base(CPreviewSiteImpl, Preview, InitPreview)
	METHOD_PROLOGUE_BASE(CPreviewSiteImpl, Preview)
	{
		pThis->_PreviewStep( nProgress );
		return S_OK;
	}
//	_catch_nested;
}

HRESULT CPreviewSiteImpl::XPreview::TerminatePreview()
{
//	_try_nested_base(CPreviewSiteImpl, Preview, InitPreview)
	METHOD_PROLOGUE_BASE(CPreviewSiteImpl, Preview)
	{
		pThis->DoTerminatePreview();
		pThis->m_nCurrentStage = -1;

		return S_OK;
	}
//	_catch_nested;
}

HRESULT CPreviewSiteImpl::XPreview::DeInitPreview()
{
	_try_nested_base(CPreviewSiteImpl, Preview, InitPreview)
	{
		if( pThis->m_nCurrentStage != -1 )
			pThis->_PreviewStep( pThis->m_nMaxProgress );
		pThis->DoLeavePreviewMode();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CPreviewSiteImpl::XPreview::CanThisObjectBeDisplayed( IUnknown *punk, BOOL *pbCan )
{
	_try_nested_base(CPreviewSiteImpl, Preview, InitPreview)
	{
		*pbCan = pThis->CanPreviewObject( punk );
		return S_OK;
	}
	_catch_nested;
}


///////////////////////////easy preview
IMPLEMENT_UNKNOWN_BASE(CEasyPreviewImpl, Preview);

HRESULT CEasyPreviewImpl::XPreview::AttachPreviewData( IUnknown *punkND )
{
	METHOD_PROLOGUE_BASE(CEasyPreviewImpl, Preview);
	if( (punkND == 0)==(pThis->m_ptrPreviewData == 0) )
		return S_OK;

	if( pThis->m_ptrPreviewData )
		pThis->OnLeavePreviewMode();
	pThis->m_ptrPreviewData = punkND;
	if( pThis->m_ptrPreviewData )
		pThis->OnEnterPreviewMode();
	return S_OK;
}

HRESULT CEasyPreviewImpl::XPreview::PreviewReady()
{
	METHOD_PROLOGUE_BASE(CEasyPreviewImpl, Preview);
	pThis->OnPreviewReady();
	return S_OK;
}


