// AuditTrailView.cpp : implementation file
//

#include "stdafx.h"
#include "AuditTrail.h"
#include "AuditTrailView.h"
#include "AuditTrailObject.h"
#include "MenuRegistrator.h"



/////////////////////////////////////////////////////////////////////////////
// CAuditTrailView

IMPLEMENT_DYNCREATE(CAuditTrailView, CCmdTarget)

CAuditTrailView::CAuditTrailView()
{
	EnableAggregation(); 
	_OleLockApp( this );

	m_sName = c_szCAuditTrailView;
	m_sUserName.LoadString(IDS_AT_VIEW_NAME);

	m_sptrActiveAT = 0;
	SetActiveAT();

	m_bHorz = ::GetValueInt(GetAppUnknown(), "AuditTrail", "HorizontalView", 1) == 1;

	m_bSingle = false;

  m_pframeRuntime = 0;
}

CAuditTrailView::~CAuditTrailView()
{
	_OleUnlockApp( this );
}

BEGIN_MESSAGE_MAP(CAuditTrailView, CViewBase)
	//{{AFX_MSG_MAP(CAuditTrailView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_KEYDOWN()
	ON_WM_NCDESTROY()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {401ACCC1-3201-42af-9CED-EE6A880C4B52}
GUARD_IMPLEMENT_OLECREATE(CAuditTrailView, "AuditTrail.AuditTrailView", 
0x401accc1, 0x3201, 0x42af, 0x9c, 0xed, 0xee, 0x6a, 0x88, 0xc, 0x4b, 0x52);


/////////////////////////////////////////////////////////////////////////////
// CAuditTrailView 

POSITION CAuditTrailView::GetFisrtVisibleObjectPosition()
{
	return (POSITION)(int)(m_sptrActiveAT != 0);
}

IUnknown *CAuditTrailView::GetNextVisibleObject( POSITION &rPos )
{
	IUnknown* punkRet = m_sptrActiveAT;
	rPos = 0;
	if(punkRet)
		punkRet->AddRef();
	return punkRet;
}

void CAuditTrailView::OnPaint() 
{
	CRect rectPaint = NORECT;
	GetUpdateRect(&rectPaint,FALSE);

	CRect rectPaintClient = ::ConvertToClient(GetControllingUnknown(), rectPaint);
	
	CPaintDC dcPaint(this); // device context for painting

	BITMAPINFOHEADER	bmih;
	FILL_BITMAPBITMAPINFOHEADER(bmih, CSize(rectPaintClient.Width(), rectPaintClient.Height()))

	DWORD dwColorWindow = ::GetValueColor(GetAppUnknown(), "AuditTrail", "InactiveThumbnailColor", ::GetSysColor(COLOR_WINDOW));

	if(m_sptrActiveAT == 0 || bmih.biHeight == 0 ||bmih.biWidth == 0)
	{
		CRect	rectImage = NORECT;
		GetClientRect(rectImage);
		dcPaint.FillRect(rectImage, &CBrush(dwColorWindow));
		return;
	}

	//create a DIB section
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dcPaint, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0 );
	ASSERT( hDIBSection );

	//set COLOR_WINDOW background
	
	for(int y = 0; y < bmih.biHeight; y++)
	{
		int nRowOffset = (bmih.biWidth*3+3)/4*4*y;
		for(int x = 0; x < bmih.biWidth; x++)
		{
			pdibBits[nRowOffset + 3*x + 2] = GetRValue(dwColorWindow);
			pdibBits[nRowOffset + 3*x + 1] = GetGValue(dwColorWindow);
			pdibBits[nRowOffset + 3*x + 0] = GetBValue(dwColorWindow);
		}
	}
	//memset(pdibBits, 255, bmih.biSizeImage);

	//drawing AT
	m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));
	m_sptrActiveAT->Draw(&bmih, pdibBits, rectPaintClient);

	//paint DIB to DC
	::DrawDibDraw(g_DrawDibInstance, dcPaint, rectPaint.left, rectPaint.top, rectPaint.Width(), rectPaint.Height(), &bmih, pdibBits, 0, 0, rectPaintClient.Width(), rectPaintClient.Height(), 0);

	::DeleteObject(hDIBSection);
}

int CAuditTrailView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	UpdateDocumentSize();

	return 0;
}

DWORD CAuditTrailView::GetMatchType( const char *szType )
{
	if(!strcmp(szType, szTypeAuditTrailObject))
		return mvFull;
	return mvNone;
}

void CAuditTrailView::UpdateDocumentSize()
{
	if(m_sptrActiveAT == 0)
		return;

	IScrollZoomSitePtr sptrSZ(GetControllingUnknown());
	if(sptrSZ == 0)
		return;

	m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));

	CSize sizeDoc(0, 0);
	m_sptrActiveAT->GetDocumentSize(&sizeDoc);
	sptrSZ->SetClientSize(sizeDoc);

	//scroll window for best view last thumbnail
	/*sizeDoc = ::ConvertToWindow(sptrSZ, sizeDoc);
	CPoint ptPos(0, 0);
	sptrSZ->GetScrollPos(&ptPos);
	if(m_bHorz == TRUE)
		ptPos.x = sizeDoc.cx;
	else
		ptPos.y = sizeDoc.cy;
	sptrSZ->SetScrollPos(ptPos); //it's with invalidate
	*/
	Invalidate();
	EnsureVisibleActiveThumbnail();
}

bool CAuditTrailView::SetActiveAT()
{
	IAuditTrailObjectPtr sptrOldAT = m_sptrActiveAT;

	m_sptrActiveAT = 0;

	IDataContextPtr	sptrContext(GetControllingUnknown());
	if(sptrContext == 0)
		return false;


	{
	IUnknown* punkAT = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeAuditTrailObject), &punkAT);
	m_sptrActiveAT = punkAT;
	if(punkAT)
		punkAT->Release();

	if(m_sptrActiveAT != 0)
	{
		if(sptrOldAT != m_sptrActiveAT && IsWindow(m_hWnd))
			UpdateDocumentSize();
		return true;
	}
	}

	IUnknown* punkImage = 0;
	IUnknown* punkAT = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkImage);

	if(!punkImage)
	{
		return false;
	}

	IUnknown* punkATM = _GetOtherComponent(GetAppUnknown(), IID_IAuditTrailMan);
	IAuditTrailManPtr sptrATman(punkATM);
	if(punkATM)
		punkATM->Release();
	
	if(sptrATman != 0)
	{
		sptrATman->GetAuditTrail(punkImage, &punkAT);
		m_sptrActiveAT = punkAT;
		if(punkAT)
			punkAT->Release();
	}

	punkImage->Release();

	if(sptrOldAT != m_sptrActiveAT && IsWindow(m_hWnd))
	{
		if( m_sptrActiveAT )
			m_sptrActiveAT->ReloadSettings( );

		UpdateDocumentSize();
	}

	return m_sptrActiveAT != 0;
}

void CAuditTrailView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if(!strcmp(pszEvent, szEventChangeObject))
	{
		if(m_sptrActiveAT == 0)//this situation can occur after filter action
			SetActiveAT();

		if(::GetObjectKey(punkFrom) == ::GetObjectKey(m_sptrActiveAT))
		{
			if(pdata && !m_bSingle)
			{
				long nVal = *(long*)pdata;
				short nPrevActive = (short)(nVal>>16);
				short nNewActive = (short)nVal;
				if(m_sptrActiveAT)
				{
					//invalidate
					CRect rcPrevActive = NORECT;
					CRect rcNewActive = NORECT;
					CRect rcInvalid = NORECT;
					m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));
					m_sptrActiveAT->GetThumbnailRect(nPrevActive, &rcPrevActive);
					m_sptrActiveAT->GetThumbnailRect(nNewActive, &rcNewActive);

					rcInvalid.UnionRect(&rcPrevActive, &rcNewActive);

					rcInvalid = ::ConvertToWindow(GetControllingUnknown(), rcInvalid);
					InvalidateRect(&rcInvalid, FALSE);
					EnsureVisibleActiveThumbnail();
				}
			}
			else
				UpdateDocumentSize();
		}
	}
	else if(!strcmp(pszEvent, szEventNewSettings))
	{
		BOOL bOldHorz = m_bHorz;
		m_bHorz = ::GetValueInt(GetAppUnknown(), "AuditTrail", "HorizontalView", 1) == 1;
		if( m_sptrActiveAT )
			m_sptrActiveAT->ReloadSettings( );

		//if(m_bHorz != bOldHorz)
		{
			UpdateDocumentSize();
		}

		Invalidate();
	}
}


BOOL CAuditTrailView::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void CAuditTrailView::AttachActiveObjects()
{
	SetActiveAT();
}

void CAuditTrailView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if(!m_bSingle && m_sptrActiveAT != 0)
	{
		point = ::ConvertToClient(GetControllingUnknown(), point);

		long nIndex = -1;
		m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));
		m_sptrActiveAT->GetThumbnailFromPoint(point, &nIndex);

		if(nIndex >= 0)
		{
			CString strParam;
			strParam.Format("\"%d\"", nIndex);
			::ExecuteAction("SetActiveAThumbnail", strParam);
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

void CAuditTrailView::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if(m_sptrActiveAT != 0)
	{
		if( m_bHorz )
		{
			if(nChar == VK_LEFT)//cursor left
			{
				::ExecuteAction("SetPrevAThumbnail");
			}
			else if(nChar == VK_RIGHT)//cursor right
			{
				::ExecuteAction("SetNextAThumbnail");
			}
		}
		else
		{
			if(nChar == VK_UP)//cursor top
			{
				::ExecuteAction("SetPrevAThumbnail");
			}
			else if(nChar == VK_DOWN)//cursor bottom
			{
				::ExecuteAction("SetNextAThumbnail");
			}
		}
	}

	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}
	
bool CAuditTrailView::GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX)
{
	if(m_sptrActiveAT == 0)
		return false;

	m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));
	BOOL bContinue = FALSE;
	IPrintViewPtr sptrPrint(m_sptrActiveAT);
	sptrPrint->GetPrintWidth(nMaxWidth, &nReturnWidth, &bContinue, nUserPosX, &nNewUserPosX);

	return bContinue == TRUE;
}

bool CAuditTrailView::GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY)
{
	if(m_sptrActiveAT == 0)
		return false;
	
	m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));
	BOOL bContinue = FALSE;
	IPrintViewPtr sptrPrint(m_sptrActiveAT);
	sptrPrint->GetPrintHeight(nMaxHeight, &nReturnHeight, &bContinue, nUserPosY, &nNewUserPosY);

	return bContinue == TRUE;
}

void CAuditTrailView::Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags)
{
	if(m_sptrActiveAT == 0)
		return;
	m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));	
	IPrintViewPtr sptrPrint(m_sptrActiveAT);
	sptrPrint->Print(hdc, rectTarget, nUserPosX, nUserPosY, nUserPosDX, nUserPosDY, bUseScrollZoom, dwFlags);
}

void CAuditTrailView::EnsureVisibleActiveThumbnail()
{
	if(m_sptrActiveAT == 0 || m_bSingle)
		return;
	
	long nActive = 0;
	CRect rcActive = NORECT;
	m_sptrActiveAT->SetFlags((m_bSingle ? 0 : (m_bHorz ? atdfHorz : atdfVert)));
	m_sptrActiveAT->GetActiveThumbnail(&nActive);
	m_sptrActiveAT->GetThumbnailRect(nActive, &rcActive);	

	IScrollZoomSitePtr sptrSZ(GetControllingUnknown());
	if(sptrSZ == 0)
		return;
	
	sptrSZ->EnsureVisible(rcActive);
}

void CAuditTrailView::OnFinalRelease() 
{
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}

void CAuditTrailView::OnNcDestroy() 
{
	CViewBase::OnNcDestroy();
	
	// TODO: Add your message handler code here
	
}

void CAuditTrailView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );
	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();
	if( sptrCM == NULL )
		return;

	CPoint pt = point;
	ScreenToClient( &pt );

	pt = ::ConvertToClient( GetControllingUnknown(), pt );

	CMenuRegistrator rcm;	
	CString strMenuName;
	
	if( strMenuName.IsEmpty() )
	{
		CString strViewName = GetViewMenuName();
		strMenuName = (LPCSTR)rcm.GetMenu( strViewName, 0 );
	}		

	if( strMenuName.IsEmpty() )
		return;
	
	sptrCM->ExecuteContextMenu2( _bstr_t(strMenuName), point, 0 );
	
}

const char* CAuditTrailView::GetViewMenuName()
{
	return szAuditTrailView;
}
