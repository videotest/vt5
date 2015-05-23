// PreviewView.cpp : implementation file
//

#include "stdafx.h"
#include "NewDoc.h"
#include "PreviewView.h"

//serggey 04/10/06
#include "InputUtils.h"
//end

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "CalibrMarker.h"

CDrawPreview::CDrawPreview()
{
	m_pView = NULL;
	m_bDrawPeriod = ::GetValueInt(GetAppUnknown(), "Input", "ShowPeriod", 0)!=0?true:false;
}

CDrawPreview::~CDrawPreview()
{
}

bool CDrawPreview::BeginPreview()
{
	if (m_sptrIP2 != 0)
		return SUCCEEDED(m_sptrIP2->BeginPreview(m_nDev, this));
	else if (m_sptrIP != 0)
		return SUCCEEDED(m_sptrIP->BeginPreview(this));
	else return false;
}

void CDrawPreview::EndPreview()
{
	if (m_sptrIP2 != 0)
		m_sptrIP2->EndPreview(m_nDev, this);
	else if (m_sptrIP != 0)
		m_sptrIP->EndPreview(this);
}

DWORD CDrawPreview::GetFreq()
{
	DWORD dwFreq;
	if (m_sptrIP2 != 0)
		return SUCCEEDED(m_sptrIP2->GetPreviewFreq(m_nDev, &dwFreq))?dwFreq:0;
	else if (m_sptrIP != 0)
		return SUCCEEDED(m_sptrIP->GetPreviewFreq(&dwFreq))?dwFreq:0;
	else
		return 0;
}

bool CDrawPreview::GetSize(CSize &sz)
{
	short cx,cy;
	HRESULT hr = E_UNEXPECTED;
	if (m_sptrIP2 != 0)
		hr = m_sptrIP2->GetSize(m_nDev, &cx, &cy);
	else if (m_sptrIP != 0)
		hr = m_sptrIP->GetSize(&cx, &cy);
	if (SUCCEEDED(hr))
		sz = CSize(cx,cy);
	return SUCCEEDED(hr);
}

void CDrawPreview::DrawRect(CDC *pDC, CRect rcSrc, CRect rcDst)
{
	if (m_sptrIP2 != 0)
		m_sptrIP2->DrawRect(m_nDev, pDC->m_hDC, rcSrc, rcDst);
	else if (m_sptrIP != 0)
		m_sptrIP->DrawRect(pDC->m_hDC, rcSrc, rcDst);
}

bool CDrawPreview::OnCreate(CIPreviewView *pView)
{
	IUnknown *punkDM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkDM)
	{
		sptrIDriverManager sptrDM(punkDM);
		punkDM->Release();
		IUnknown *punkD;
		sptrDM->GetCurrentDeviceAndDriver(NULL, &punkD, &m_nDev);
		m_sptrIP = punkD;
		m_sptrIP2 = punkD;
		punkD->Release();
		if (AnyInterface())
		{
			BeginPreview();
			DWORD dwFreq = GetFreq();
			if (dwFreq > 0)
				pView->SetTimer(274, dwFreq, NULL);
			CSize sizeNew;
			if (GetSize(sizeNew))
			{
				sptrIScrollZoomSite	pstrS( pView->GetControllingUnknown() );
				pstrS->SetClientSize(sizeNew);
			}
		}
	}
	m_pView = pView;
	return m_sptrIP;
}

void CDrawPreview::OnDestroy(CIPreviewView *pView)
{
	pView->KillTimer(1);
	EndPreview();
}

void CDrawPreview::OnPaint(CIPreviewView *pView, CDC *pDC)
{
	sptrIScrollZoomSite	sptrS( pView->GetControllingUnknown() );
	CSize sizeNZ;
	sptrS->GetClientSize(&sizeNZ);
	CPoint ptPos;
	sptrS->GetScrollPos(&ptPos);
	double dZoom;
	sptrS->GetZoom(&dZoom);
	CPoint ptPosNZ((int)(ptPos.x/dZoom),(int)(ptPos.y/dZoom));
	CSize size((int)(sizeNZ.cx*dZoom),(int)(sizeNZ.cy*dZoom));
	CRect rcWnd;
	pView->GetClientRect(rcWnd);
	CSize sizeWnd(rcWnd.Size());
	CSize sizeDraw(min(size.cx-ptPos.x,sizeWnd.cx),min(size.cy-ptPos.y,sizeWnd.cy));
	CSize sizeDrawNZ((int)(sizeDraw.cx/dZoom),(int)(sizeDraw.cy/dZoom));
	if (size.cx > ptPosNZ.x && size.cy > ptPosNZ.y)
	{
		CRect rcSrc(ptPosNZ.x, ptPosNZ.y, ptPosNZ.x+sizeDrawNZ.cx, ptPosNZ.y+sizeDrawNZ.cy);
		CRect rcDst(0, 0, sizeDraw.cx, sizeDraw.cy);
		DrawRect(pDC, rcSrc, rcDst);
//		if (sizeWnd.cy > rcDst.Height())
//			pDC->FillSolidRect(rcDst.left, rcDst.bottom, rcDst.Width(),
//				sizeWnd.cy-rcDst.Height(), RGB(0,0,0));
//		if (sizeWnd.cx > rcDst.Width())
//			pDC->FillSolidRect(rcDst.right, rcDst.top,
//				sizeWnd.cx-rcDst.Width(), sizeWnd.cy, RGB(0,0,0));
		if (m_bDrawPeriod && m_sptrIP2 != 0)
		{
			DWORD dwPeriod;
			if (SUCCEEDED(m_sptrIP2->GetPeriod(m_nDev, 0, &dwPeriod)) && dwPeriod > 0)
			{
				char szBuff[50];
				_itoa(dwPeriod, szBuff, 10);
				pDC->SetBkMode(TRANSPARENT);
				pDC->DrawText(szBuff, -1, rcDst, DT_LEFT|DT_TOP);
			}
		}
	}

	// Draw Calibration marker
	if(_IsCalibrVisible(pView->GetControllingUnknown()))
	{
		double	fCalibr = 0.0025;
		GUID guidC;
		::GetDefaultCalibration( &fCalibr, &guidC );

		::_DrawCalibr(*pDC, rcWnd, 0, 0, 0, 1, 0,
			dZoom, fCalibr, ptPos, sizeDrawNZ, pView, CPoint(0,0));
	}
}

void CDrawPreview::OnTimer(CIPreviewView *pView, UINT idTimer)
{
	if (idTimer == 274)
		pView->Invalidate(FALSE);
}

HRESULT CDrawPreview::Invalidate()
{
	if (m_pView)
		m_pView->Invalidate();
	return S_OK;
}

HRESULT CDrawPreview::OnChangeSize()
{
	if (m_sptrIP)
	{
		short cx,cy;
		HRESULT hr = m_sptrIP->GetSize(&cx, &cy);
		if (SUCCEEDED(hr))
		{
			sptrIScrollZoomSite	pstrS( m_pView->GetControllingUnknown() );
			CSize	sizeNew(cx, cy);
			pstrS->SetClientSize(sizeNew);
			m_pView->Invalidate();
		}
	}
	return S_OK;
}

//sergey 04/10/06
void CDrawPreview::WindowToImage(CPoint &pt,CRect rcClient1)
{
	CRect rcClient;
	rcClient=rcClient1;
		//GetClientRect(rcClient);
	//sergey 04/10/06
	short cx,cy;
		m_sptrPrv->GetSize(m_nDev,&cx,&cy);
		m_szPrv = CSize(cx,cy);	
		//end
	pt.x = pt.x*m_szPrv.cx/rcClient.Width();
	pt.y = pt.y*m_szPrv.cy/rcClient.Height();
}

void CDrawPreview::DoLButtonDown(UINT nFlags, CPoint point,CRect rcClient1)
{
	WindowToImage(point,rcClient1);
	m_sptrPrv->LButtonDown(nFlags, point);
}

void CDrawPreview::DoLButtonUp(UINT nFlags, CPoint point,CRect rcClient1)
{
	WindowToImage(point,rcClient1);
	m_sptrPrv->LButtonUp(nFlags, point);
}

void CDrawPreview::DoMouseMove(UINT nFlags, CPoint point,CRect rcClient1)
{
	WindowToImage(point,rcClient1);
	m_sptrPrv->MouseMove(nFlags, point);
}

bool CDrawPreview::DoSetCursor(UINT nFlags, CPoint point,CRect rcClient1)
{
	WindowToImage(point,rcClient1);
	return m_sptrPrv->SetCursor(nFlags, point) == S_OK;
}

//end

CDrawWnd::CDrawWnd()
{
	m_hwnd = NULL;
}

CDrawWnd::~CDrawWnd()
{
}

bool CDrawWnd::OnCreate(CIPreviewView *pView)
{
	IUnknown *punkDM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkDM)
	{
		sptrIDriverManager sptrDM(punkDM);
		punkDM->Release();
		IUnknown *punkD;
		int nDev;
		sptrDM->GetCurrentDeviceAndDriver(NULL, &punkD, &nDev);
		m_sptrIW = punkD;
		punkD->Release();
		if (m_sptrIW)
			m_sptrIW->CreateInputWindow(nDev,pView->m_hWnd,&m_hwnd);
		
		
	}
	return m_sptrIW;
}

void CDrawWnd::OnDestroy(CIPreviewView *pView)
{
	if (m_sptrIW)
		m_sptrIW->DestroyInputWindow(m_hwnd);
}

void CDrawWnd::OnSize(CIPreviewView *pView)
{
	SIZE sz;
	sz.cx = 0;
	sz.cy = 0;
	if (m_sptrIW)
		m_sptrIW->OnSize(m_hwnd, pView->m_hWnd, sz);
	
}

/////////////////////////////////////////////////////////////////////////////
// CIPreviewView

IMPLEMENT_DYNCREATE(CIPreviewView, CWnd)

CIPreviewView::CIPreviewView()
{
	EnableAutomation();
	EnableAggregation();
	
	_OleLockApp( this );
	m_pDraw = NULL;

	m_sName = c_szCIPreviewView;
	m_sUserName.LoadString( IDS_PREVIEW_VIEW );
}

CIPreviewView::~CIPreviewView()
{
	DeinitPreview();
	_OleUnlockApp( this );
}

void CIPreviewView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	DeinitPreview();
	if( GetSafeHwnd() )
		DestroyWindow();
	delete this;
}


BEGIN_MESSAGE_MAP(CIPreviewView, CViewBase)
	//{{AFX_MSG_MAP(CIPreviewView)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_DESTROY()

	//sergey 04/10/06
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	//end
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CIPreviewView, CViewBase)
	//{{AFX_DISPATCH_MAP(CIPreviewView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IIPreviewView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {C0D5CB20-6ABF-4DCC-ADBA-9E9C998965A8}
static const IID IID_IIPreviewView =
{ 0xc0d5cb20, 0x6abf, 0x4dcc, { 0xad, 0xba, 0x9e, 0x9c, 0x99, 0x89, 0x65, 0xa8 } };

BEGIN_INTERFACE_MAP(CIPreviewView, CViewBase)
	INTERFACE_PART(CIPreviewView, IID_IIPreviewView, Dispatch)
END_INTERFACE_MAP()

// {E3C7484B-3415-4982-89B2-C4920AB7391F}
GUARD_IMPLEMENT_OLECREATE(CIPreviewView, "PreviewDoc.PreviewView", 0xe3c7484b, 0x3415, 0x4982, 0x89, 0xb2, 0xc4, 0x92, 0xa, 0xb7, 0x39, 0x1f)

/////////////////////////////////////////////////////////////////////////////
// CIPreviewView message handlers


void CIPreviewView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	if (m_pDraw)
	{
		m_pDraw->OnPaint(this, &dc);
//		DWORD dw = GetTickCount();
		ProcessDrawing(dc, NORECT, NULL, NULL);
//		TRACE("Drawing time %d\n", GetTickCount()-dw);
	}
	else
	{
		CRect rcClient;
		GetClientRect(rcClient);
		dc.FillSolidRect(rcClient, RGB(0,0,64));
	}
	// Do not call CWnd::OnPaint() for painting messages
}


int CIPreviewView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	InitPreview();
	//sergey 04/10/06
	
	//CRepositionManager::Init(m_Image.m_pView->m_hwndParent, 0, _MaxClientSize(this));
	
	//end
	return 0;
}

bool CIPreviewView::InitPreview()
{
	DeinitPreview();
	CDrawBase *pDraw = NULL;
	pDraw = new CDrawPreview;
	if (!pDraw->OnCreate(this))
	{
		delete pDraw;
		pDraw = NULL;
	}
	if (!pDraw)
	{
		pDraw = new CDrawWnd;
		if (!pDraw->OnCreate(this))
		{
			delete pDraw;
			pDraw = NULL;
		}
	}
	if (!pDraw)
	{
		AfxMessageBox(IDS_NO_PREVIEW, MB_OK|MB_ICONEXCLAMATION);
	}
	
	m_pDraw = pDraw;
	return m_pDraw!=NULL;
}

void CIPreviewView::DeinitPreview()
{
	if (m_pDraw)
	{
		m_pDraw->OnDestroy(this);
		delete m_pDraw;
		m_pDraw = NULL;
	}
}

void CIPreviewView::OnTimer(UINT_PTR nIDEvent) 
{
	if (m_pDraw && nIDEvent == 274)
		m_pDraw->OnTimer(this, nIDEvent);
	CViewBase::OnTimer(nIDEvent);
}

void CIPreviewView::OnSize(UINT nType, int cx, int cy) 
{
	CViewBase::OnSize(nType, cx, cy);
	if (m_pDraw)
		m_pDraw->OnSize(this);
}

void CIPreviewView::OnDestroy() 
{
	DeinitPreview();
	CViewBase::OnDestroy();
}

//sergey 04/10/06
void CIPreviewView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	//sergey 04/10/06
	    CRect rcClient;
		GetClientRect(rcClient);
		//end
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonDown(nFlags, pt,rcClient);
	}
	CViewBase::OnLButtonDown(nFlags, point);
}

void CIPreviewView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	//sergey 04/10/06
	    CRect rcClient;
		GetClientRect(rcClient);
		//end
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoLButtonUp(nFlags, pt,rcClient);
	}
	CViewBase::OnLButtonUp(nFlags, point);
}

void CIPreviewView::OnMouseMove(UINT nFlags, CPoint point) 
{
	//sergey 04/10/06
	    CRect rcClient;
		GetClientRect(rcClient);
		//end
	if ((nFlags & MK_LBUTTON) && m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		m_Image.DoMouseMove(nFlags, pt,rcClient);
	}
	CViewBase::OnMouseMove(nFlags, point);
}

BOOL CIPreviewView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	//sergey 04/10/06
	    CRect rcClient;
		GetClientRect(rcClient);
		//end
	CPoint point;
	GetCursorPos(&point);
	ScreenToClient(&point);
	if (m_rcBase.PtInRect(point))
	{
		CPoint pt(point.x-m_rcBase.left, point.y-m_rcBase.top);
		if (m_Image.DoSetCursor(0, pt,rcClient))
			return TRUE;
	}
	
	return CViewBase::OnSetCursor(pWnd, nHitTest, message);
}
//end