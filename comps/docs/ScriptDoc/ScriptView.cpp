// ScriptView.cpp : implementation file
//

#include "stdafx.h"
#include "ScriptDoc.h"
#include "ScriptView.h"
#include "editcmd.h"
#include "CCrystalTextBuffer.h"
#include "ScriptDataObject.h"
#include "axint.h"

#include "statusutils.h"


IMPLEMENT_DYNCREATE(CScriptView, CCmdTargetEx);

// {52E66404-1EFF-11d3-A5D3-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CScriptView, "ScriptDoc.ScriptView", 
0x52e66404, 0x1eff, 0x11d3, 0xa5, 0xd3, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

/////////////////////////////////////////////////////////////////////////////
// CScriptView
CScriptView::CScriptView()
{
	EnableAggregation();
	s_hResourceInst = theApp.m_hInstance;
	m_sUserName.LoadString(IDS_SCRIPTVIEW_NAME);

	m_dwContext = 0;
}

CScriptView::~CScriptView()
{
	m_mapBreakpoins.RemoveAll();
}


BEGIN_MESSAGE_MAP(CScriptView, CCrystalEditView)
	//{{AFX_MSG_MAP(CScriptView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_MESSAGE_VOID(WM_INITIALUPDATE, OnInitialUpdate)
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CScriptView, CWnd)
	INTERFACE_PART(CScriptView, IID_IWindow, Wnd)
	INTERFACE_PART(CScriptView, IID_IRootedObject, Rooted)
	INTERFACE_PART(CScriptView, IID_IView, View)
	INTERFACE_PART(CScriptView, IID_IEventListener, EvList)
	INTERFACE_PART(CScriptView, IID_INamedObject2, Name)
	INTERFACE_PART(CScriptView, IID_IPrintView, PrintView)
	INTERFACE_PART(CScriptView, IID_IApplicationDebugger, AppDebuger)
	INTERFACE_PART(CScriptView, IID_IApplicationDebuggerUI, AppDebugerUI)
	INTERFACE_PART(CScriptView, IID_IProvideDebuggerIDE, DebugerIDE) 
	INTERFACE_PART(CScriptView, IID_IHelpInfo, Help) 
END_INTERFACE_MAP();


/////////////////////////////////////////////////////////////////////////////
// CScriptView message handlers

void CScriptView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	OnDraw( &dc );	
	// Do not call CWndBase::OnPaint() for painting messages
}



int CScriptView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	sptrIScrollZoomSite	sptrScrollZoom( GetControllingUnknown() );
	sptrScrollZoom->SetAutoScrollMode( false );

	if (CCrystalEditView::OnCreate(lpCreateStruct) == -1)
		return -1;


	Register();

	m_lf.lfHeight = 0;
    m_lf.lfWidth = 9;
    m_lf.lfEscapement = 0;
    m_lf.lfOrientation = 0;
    m_lf.lfWeight = 0;
    m_lf.lfItalic = 0;
    m_lf.lfUnderline = 0;
    m_lf.lfStrikeOut = 0;
    m_lf.lfCharSet = 0;
    m_lf.lfOutPrecision = 0;
    m_lf.lfClipPrecision = 0;
    m_lf.lfQuality = 0;
    m_lf.lfPitchAndFamily = 0;
    strcpy( m_lf.lfFaceName, "Courier New Cyr" );

	SetFont( m_lf );

	OnInitialUpdate();
	
	return 0;
}

void CScriptView::OnDestroy()
{
	UnRegister();
}

void CScriptView::OnInitialUpdate()
{
	CCrystalEditView::OnInitialUpdate();
}

CCrystalTextBuffer *CScriptView::LocateTextBuffer()
{
	IUnknown	*punkObject = ::GetActiveObjectFromContextEx( GetControllingUnknown() );
	//IUnknown	*punkObject = ::GetActiveObjectFromContext( GetControllingUnknown(), szArgumentTypeScript );
	if( !punkObject )
	{
		IUnknown	*punkForm = ::GetActiveObjectFromContext( GetControllingUnknown(), szArgumenAXForm );
		if( !punkForm )
			return 0;

		sptrIActiveXForm	sptrF( punkForm );
		punkForm->Release();

		m_sptrForm = sptrF;

		sptrF->GetScript( &punkObject );

		if( !punkObject )
			return 0;
		
	}
	else
		m_sptrForm = 0;

	m_sptrD=punkObject;

	punkObject->Release();

	if( m_sptrD == 0 )
		return 0;

	DWORD	dw = 0;
	m_sptrD->GetPrivateDataPtr( &dw );

	CCrystalTextBuffer	*pbuf = (CCrystalTextBuffer	*)dw;

	return pbuf;
	//return 0;
}


void CScriptView::OnFinalRelease() 
{
	//UnRegister();
	if( GetSafeHwnd() )
		DestroyWindow();
//	else
	delete this;
	
//	CCrystalEditView::OnFinalRelease();
}


BOOL CScriptView::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		int nVirtKey = (int) pMsg->wParam; 
		if( nVirtKey == VK_LEFT || nVirtKey == VK_RIGHT || nVirtKey == VK_UP || nVirtKey == VK_DOWN )
		{
			UpdatePosPane();
		}
						
	}

	return CCrystalTextView::PreTranslateMessage(pMsg);
}

void CScriptView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager	spCmdMan( punk );

	punk->Release();

	CString	str( "Script View" );

	BSTR	bstr = str.AllocSysString();

	spCmdMan->ExecuteContextMenu( bstr, point );

	::SysFreeString( bstr );
}

void CScriptView::OnActivateObject( IUnknown *punkDataObject )
{
	IUnknown *punkScriptDataObject = 0;
	IUnknown *punkObject = punkDataObject;

	//if( !punkObject )
	{
		punkObject = ::GetActiveObjectFromContextEx( GetControllingUnknown() );//::GetActiveObjectFromContext( GetControllingUnknown(), szArgumentTypeScript );
		//punkObject = ::GetActiveObjectFromContext( GetControllingUnknown(), szArgumentTypeScript );

		if( punkObject )
			punkObject->Release();

	}

	if( CheckInterface( punkObject, IID_IScriptDataObject ) )
	{
		punkScriptDataObject = punkObject;
		punkScriptDataObject->AddRef();
		m_sptrForm = 0;
	}
	else
	if( CheckInterface( punkObject, IID_IActiveXForm ) )
	{
		sptrIActiveXForm	sptrF( punkObject );
		sptrF->GetScript( &punkScriptDataObject );
		m_sptrForm = punkObject;
	}
	else
	if( punkObject )
		return;

	if( ::GetObjectKey( punkScriptDataObject ) == ::GetObjectKey( m_sptrD ) )
	{
		if( punkScriptDataObject ) 
			punkScriptDataObject->Release();
		return;
	}

	if( m_pTextBuffer  )
		m_pTextBuffer->RemoveView( this );

	CCrystalTextBuffer	*pbuf = 0;

	m_sptrD = punkScriptDataObject;

	if( punkScriptDataObject )
	{
		DWORD	dw = 0;
		sptrIScriptDataObject sptr = punkScriptDataObject;
		
		sptr->GetPrivateDataPtr( &dw );

		pbuf = (CCrystalTextBuffer	*)dw;

		punkScriptDataObject->Release();
	}
	else
		m_sptrForm = 0;

	m_pTextBuffer = pbuf;

	if( m_pTextBuffer )
		m_pTextBuffer->AddView( this );

	if( GetSafeHwnd() )
	{
		ResetView();
		RecalcVertScrollBar();
		RecalcHorzScrollBar();

		Invalidate();
	}

	IViewSitePtr	ptrV( GetControllingUnknown() );
	if( ptrV )ptrV->UpdateObjectInfo();
}

BOOL CScriptView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !CCrystalEditView::PreCreateWindow(cs) )
		return false;
	cs.lpszClass = ::AfxRegisterWndClass( CS_DBLCLKS|CS_HREDRAW|CS_VREDRAW|CS_OWNDC, 
							::LoadCursor(NULL, IDC_ARROW), 	(HBRUSH)(COLOR_WINDOW+1) );

	return true;

}

DWORD CScriptView::GetMatchType( const char *szType )
{
	if( !strcmp( szType, szArgumentTypeScript ) )
		return mvFull;
	else if( !strcmp( szType, szArgumenAXForm ) )
		return mvPartial;
	else
		return mvNone;
}

/*
POSITION CScriptView::GetFisrtVisibleObjectPosition()
{
	return (POSITION)((m_sptrD != 0) ? 1 : 0);
}

IUnknown *CScriptView::GetNextVisibleObject( POSITION &rPos )
{
	IUnknown *punk = 0;
	if ((int)rPos == 1)
	{
		rPos = 0;
		punk = m_sptrD;
		if (punk)
			punk->AddRef();
	}
	return punk;
}
*/

void CScriptView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if(strcmp( pszEvent, szEventSetScriptActiveLine ) == 0)
	{
		if(::GetObjectKey(m_sptrD) == ::GetObjectKey(punkFrom))
		{
			long nLine = *(long*)pdata;
			//HighlightText(CPoint(0, nLine), GetLineLength(nLine));
			if(nLine > 0 && GetLineCount() > nLine)
			{
				ScrollToLine(nLine-1);
				SetCursorPos(CPoint(0, nLine));
			}
		}
	}
}

int CScriptView::_GetMaxWidth()
{
	return (GetMaxLineLength()*GetCharWidth()+GetMarginWidth());
}

bool CScriptView::GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX )
{
	long nWidth = _GetMaxWidth();
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

int CScriptView::_GetNumLinesPerHeight(int nBeginLine, int& nHeight)
{
	int nLines = 0;
	int nLineCount = GetLineCount();
	int nLineHeight = GetLineHeight();
	int nCurHeight = 0;
	for(int i = nBeginLine; i < nLineCount; i++)
	{
		nLines = i;
		nCurHeight += nLineHeight;
		if(nHeight < nCurHeight)
		{
			nLines -= 1;
			nCurHeight -= nLineHeight;
			break;
		}
	}

	nLines += 1 - nBeginLine;
	nHeight = nCurHeight;

	return nLines;
}

bool CScriptView::GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY )
{
	nReturnHeight = nMaxHeight;
	int nLines2Print = _GetNumLinesPerHeight(nUserPosY, nReturnHeight);

	nNewUserPosY = nUserPosY + nLines2Print;

	int nLineCount = GetLineCount();

	return nNewUserPosY+1 < nLineCount;
}

void CScriptView::Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags )
{
	CDC* pdc = CDC::FromHandle(hdc);

	int nLineHeight = GetLineHeight();
	CRect rcClient(0, nUserPosY*nLineHeight, nUserPosX+nUserPosDX, (nUserPosY+nUserPosDY)*nLineHeight);
	//CRect rcClient(0, 0, nUserPosX+nUserPosDX, (nUserPosY+nUserPosDY)*nLineHeight);
	
	int nLineCount = GetLineCount();
	
	PrepareSelBounds();

	CDC cacheDC;
	VERIFY(cacheDC.CreateCompatibleDC(pdc));
	CBitmap* pCacheBitmap = new CBitmap;
	VERIFY(pCacheBitmap->CreateCompatibleBitmap(pdc, rcClient.Width(), rcClient.Height()));
	CBitmap *pOldBitmap = cacheDC.SelectObject(pCacheBitmap);


	CRect rcLine;
	rcLine = rcClient;
	rcLine.left = nUserPosX;
	rcLine.top = 0;
	//rcLine.top = nUserPosY*nLineHeight;
	//rcLine.bottom = rcLine.top + nLineHeight;
	CRect rcCacheMargin(0, 0, GetMarginWidth(), nLineHeight);
	CRect rcCacheLine(GetMarginWidth(), 0, rcClient.Width(), nLineHeight);

	int nCurrentLine = nUserPosY;
	//int nCurrentLine = 0;

	while (rcLine.top < rcClient.bottom)
	{
		rcCacheLine = CRect(GetMarginWidth(), rcLine.top, rcClient.Width(), rcLine.top+nLineHeight);
		rcCacheMargin = CRect(0, rcLine.top, GetMarginWidth(), rcLine.top+nLineHeight);

		if (nCurrentLine < nLineCount)
		{
			if(nUserPosX == 0)DrawMargin(&cacheDC, rcCacheMargin, nCurrentLine);
			DrawSingleLine(&cacheDC, rcCacheLine, nCurrentLine);
		}
		else
		{
			if(nUserPosX == 0)DrawMargin(&cacheDC, rcCacheMargin, -1);
			DrawSingleLine(&cacheDC, rcCacheLine, -1);
		}

		nCurrentLine ++;
		rcLine.OffsetRect(0, nLineHeight);
	}
	
	
	//pdc->StretchBlt(rectTarget.left, rectTarget.top, rectTarget.Width(), rectTarget.Height(), &cacheDC, nUserPosX, (nUserPosY*nLineHeight), rcClient.Width()-nUserPosX, (rcClient.Height()-(nUserPosY*nLineHeight)), SRCCOPY);
	pdc->StretchBlt(rectTarget.left, rectTarget.top, rectTarget.Width(), rectTarget.Height(), &cacheDC, nUserPosX, 0, rcClient.Width()-nUserPosX, rcClient.Height(), SRCCOPY);
		
	cacheDC.SelectObject(pOldBitmap);
	cacheDC.DeleteDC();
	delete pCacheBitmap; 
}




///////////////////////////////////////////////////////////////////////////////////////////////
//Active Script Debug IDE support//////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////
// IApplicationDebugger implementation
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_UNKNOWN(CScriptView, AppDebuger)

HRESULT CScriptView::XAppDebuger::QueryAlive(void)
{
	_try_nested(CScriptView, AppDebuger, QueryAlive)

	return S_OK;

   _catch_nested;
}
	
HRESULT CScriptView::XAppDebuger::CreateInstanceAtDebugger(REFCLSID rclsid, IUnknown *pUnkOuter, DWORD dwClsContext, REFIID riid, IUnknown **ppvObject)
{
	_try_nested(CScriptView, AppDebuger, CreateInstanceAtDebugger)

	return S_OK;

   _catch_nested;
}

HRESULT CScriptView::XAppDebuger::onDebugOutput(LPCOLESTR pstr)
{
	_try_nested(CScriptView, AppDebuger, onDebugOutput)

	return S_OK;

   _catch_nested;
}

HRESULT CScriptView::XAppDebuger::onHandleBreakPoint(IRemoteDebugApplicationThread *prpt, BREAKREASON br, IActiveScriptErrorDebug *pError)
{
	_try_nested(CScriptView, AppDebuger, onHandleBreakPoint)

	TRACE0("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<onHandleBreakPoint>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

	
	IRemoteDebugApplication* pDebugApp = 0;
	prpt->GetApplication(&pDebugApp);

	pDebugApp->ResumeFromBreakPoint(prpt, BREAKRESUMEACTION_CONTINUE, ERRORRESUMEACTION_AbortCallAndReturnErrorToCaller); 
	
	pDebugApp->Release();
	
	return S_OK;

   _catch_nested;
}

HRESULT CScriptView::XAppDebuger::onClose(void)
{
	_try_nested(CScriptView, AppDebuger, onClose)

	return S_OK;

   _catch_nested;
}

HRESULT CScriptView::XAppDebuger::onDebuggerEvent(REFIID riid, IUnknown *punk)
{
	_try_nested(CScriptView, AppDebuger, onDebuggerEvent)

	return S_OK;

   _catch_nested;
}


///////////////////////////////////////////////////////////////////////////////////////////////
// IApplicationDebuggerUI implementation
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_UNKNOWN(CScriptView, AppDebugerUI)

HRESULT CScriptView::XAppDebugerUI::BringDocumentToTop(IDebugDocumentText* pddt)
{
	_try_nested(CScriptView, AppDebuger, BringDocumentToTop)

	TRACE0("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<BringDocumentToTop>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	return S_OK;

   _catch_nested;
}

HRESULT CScriptView::XAppDebugerUI::BringDocumentContextToTop(IDebugDocumentContext* pddc)
{
	_try_nested(CScriptView, AppDebuger, BringDocumentContextToTop)

	TRACE0("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<BringDocumentContextToTop>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
	return S_OK;

   _catch_nested;
}

///////////////////////////////////////////////////////////////////////////////////////////////
// IProvideDebuggerIDE implementation
///////////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_UNKNOWN(CScriptView, DebugerIDE)
HRESULT CScriptView::XDebugerIDE::GetApplicationDebugger(IApplicationDebugger** ppad)
{
	_try_nested(CScriptView, DebugerIDE, GetApplicationDebugger)

	if(ppad)
		return pThis->QueryInterface(IID_IApplicationDebugger, (void**)ppad);
	else
		return E_INVALIDARG;

   _catch_nested;
}

HRESULT CScriptView::XDebugerIDE::ToggleBreakpoint()
{
	_try_nested(CScriptView, DebugerIDE, ToggleBreakpoint)


	DWORD dwChars = 0;
	CPoint ptPos(0, 0);
	ptPos = pThis->GetCursorPos();
	for(long i = 0; i < ptPos.y; i++)
	{
		dwChars += pThis->GetLineLength(i);
	}
	
	dwChars += ptPos.x;

	bool bPresent = false;
	pThis->m_mapBreakpoins.Lookup(dwChars, bPresent);
	if(bPresent)
	{
		pThis->m_mapBreakpoins.RemoveKey(dwChars);
	}
	else
	{
		pThis->m_mapBreakpoins.SetAt(dwChars, true);
	}

	return S_OK;
	
   _catch_nested;
}

HRESULT CScriptView::XDebugerIDE::SetBreakpointsToEngine(DWORD dwContext)
{
	_try_nested(CScriptView, DebugerIDE, SetBreakpoints)


	pThis->m_dwContext = dwContext;

	POSITION pos = pThis->m_mapBreakpoins.GetStartPosition();

	DWORD dwOffset = 0;
	bool	bPresent = false;
	while(pos)
	{
		pThis->m_mapBreakpoins.GetNextAssoc(pos, dwOffset, bPresent);
		ASSERT(bPresent);
		pThis->SetBreakpoint(dwOffset);
	}

	
	return S_OK;

   _catch_nested;
}


void CScriptView::SetBreakpoint(DWORD dwOffset)
{
	return;
	/*
	IApplicationPtr	sptrApp(GetAppUnknown());
	IUnknown* punkScript = 0;
	sptrApp->GetActiveScript(&punkScript);
	IActiveScriptDebug*	punkDebug = 0;
	punkScript->QueryInterface(__uuidof(IActiveScriptDebug), (void**)&punkDebug);
	if(punkScript)
		punkScript->Release();

	sptrIProvideDebuggerIDE	sptrDebugIDE(GetControllingUnknown());
	
	if(sptrDebugIDE == 0)
	{
		ASSERT(FALSE);
		return;
	}

	IEnumDebugCodeContexts *pescc = 0;
	if(punkDebug)
	{
		punkDebug->EnumCodeContextsOfPosition(m_dwContext, dwOffset, -1, &pescc);
		if(pescc)
		{
			IDebugCodeContext* pdcc = 0;
			DWORD wdActualRead = 0; 
			do
			{
				pescc->Next(1, &pdcc, &wdActualRead);
				if(pdcc)
				{
					HRESULT hr = pdcc->SetBreakPoint(BREAKPOINT_ENABLED);
					pdcc->Release();
					pdcc = 0;
				}
			}while(wdActualRead == 1);

			pescc->Release();
		}

		punkDebug->Release();
	}
	*/
}
	

bool CScriptView::DoTranslateAccelerator( MSG *pMsg )
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (m_hAccel != NULL)
		{
			if (::TranslateAccelerator(m_hWnd, m_hAccel, pMsg))
				return TRUE;
		}
	}

	return false;
}

POSITION CScriptView::GetFisrtVisibleObjectPosition()
{
	if( m_sptrD != 0 )
		return (POSITION)1;
	return 0;
}

IUnknown *CScriptView::GetNextVisibleObject( POSITION &rPos )
{
	if( (long)rPos == 1 )
	{
		rPos = 0;
		IUnknown * punk = (m_sptrForm != 0) ? m_sptrForm : m_sptrD;
		if( punk )punk->AddRef();
		return punk;
	}
	return 0;
}


////////////////////////////////////////////////////////////////
void CScriptView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	if( bActivate )
		CreatePosPane();
	else
		DestroyPosPane();


}

////////////////////////////////////////////////////////////////
void CScriptView::CreatePosPane()
{
	DestroyPosPane();
	
	INewStatusBar* psb = 0;
	psb = ::StatusGetBar();
	if( psb )
	{
		int nWidth = ::StatusCalcWidth("Ln:200, Col:200", false );
		psb->AddPane( guidPaneScriptPos, nWidth, 0, 0  );		

		psb->Release();	psb = 0;		
	}

}

////////////////////////////////////////////////////////////////
void CScriptView::DestroyPosPane()
{
	INewStatusBar* psb = 0;
	psb = ::StatusGetBar();
	if( psb )
	{
		psb->RemovePane( guidPaneScriptPos );		
		psb->Release();	psb = 0;
	}

}

////////////////////////////////////////////////////////////////
void CScriptView::OnSetCaretPos( CPoint pt )
{
	UpdatePosPane();
}

////////////////////////////////////////////////////////////////
void CScriptView::UpdatePosPane()
{
	CString strText;
	CPoint pt( 0, 0 );
	
	pt = GetCursorPos();

	strText.Format( "Ln:%d, Col:%d", pt.y, pt.x );
	StatusSetPaneText( guidPaneScriptPos, (LPCSTR)strText );
}