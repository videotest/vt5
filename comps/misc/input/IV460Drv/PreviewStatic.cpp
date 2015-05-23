// PreviewStatic.cpp : implementation file
//

#include "stdafx.h"
#include "iv460drv.h"
#include "PreviewStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic

CPreviewStatic::CPreviewStatic()
{
	m_pPreview = NULL;
	m_szImg = CSize(0,0);
}

CPreviewStatic::~CPreviewStatic()
{
	if (m_pPreview)
		m_pPreview->Release();
}

void CPreviewStatic::SetPreview(IInputPreview *pPreview)
{
	if (m_pPreview)
		m_pPreview->Release();
	m_pPreview = pPreview;
	if (m_pPreview)
		m_pPreview->AddRef();
}

void CPreviewStatic::SetOvrMode()
{
}


BEGIN_MESSAGE_MAP(CPreviewStatic, CStatic)
	//{{AFX_MSG_MAP(CPreviewStatic)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic message handlers

void CPreviewStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rcClient;
	GetClientRect(rcClient);
	if (m_pPreview)
		m_pPreview->DrawRect(dc.m_hDC, CRect(0,0,0,0), rcClient);
	else
	{
		int r = CStdProfileManager::m_pMgr->GetProfileInt(_T("Overlay"), _T("ColorKeyRed"), 0, true, true);
		int g = CStdProfileManager::m_pMgr->GetProfileInt(_T("Overlay"), _T("ColorKeyGreen"), 0, true, true);
		int b = CStdProfileManager::m_pMgr->GetProfileInt(_T("Overlay"), _T("ColorKeyBlue"), 0, true, true);
		dc.FillSolidRect(rcClient, RGB(r,g,b));
	}
	BOOL bEnableMargins = g_IV460Profile.GetProfileInt(_T("Settings"), _T("EnableMargins"), FALSE);
	BOOL bPreviewAllFrame = g_IV460Profile.GetProfileInt(_T("Settings"), _T("PreviewAllFrame"), FALSE);
	if (bPreviewAllFrame)
	{
		CRect rc(0,0,0,0);
		if (rc != CRect(0,0,0,0) && m_szImg != CSize(0,0) && bEnableMargins)
		{
			dc.IntersectClipRect(rcClient);
			CRect rc1(rcClient.Width()*rc.left/m_szImg.cx, rcClient.Height()*rc.top/m_szImg.cy,
				rcClient.right-rcClient.Width()*rc.right/m_szImg.cx, rcClient.bottom-rcClient.Height()*rc.bottom/m_szImg.cy);
			CBrush br1(RGB(0,0,0));
			CBrush br2(RGB(255,255,255));
			dc.FrameRect(rc1, &br1);
			rc1.InflateRect(1,1,-1,-1);
			dc.FrameRect(rc1, &br2);
			br2.DeleteObject();
			br1.DeleteObject();
		}
	}
}

BOOL CPreviewStatic::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CPreviewStatic::DoLButtonDown(UINT nFlags, CPoint point) 
{
	/*
	BOOL bEnableMargins = g_IV460Profile.GetProfileInt(_T("Settings"), _T("EnableMargins"), FALSE);
	if (m_szImg.cx > 0 && m_szImg.cy > 0 && bEnableMargins)
	{
		CRect rcClient;
		GetClientRect(rcClient);
		m_ptBeg = CPoint(point.x*m_szImg.cx/rcClient.Width(), point.y*m_szImg.cy/rcClient.Height());
	}
	*/
}

void CPreviewStatic::DoLButtonUp(UINT nFlags, CPoint point) 
{
	/*
	BOOL bEnableMargins = g_IV460Profile.GetProfileInt(_T("Settings"), _T("EnableMargins"), FALSE);
	if (bEnableMargins)
	{
		g_IV460Profile.WriteProfileInt(_T("Margins"), _T("Left"), g_CxLibWork.m_rcMargins.left);
		g_IV460Profile.WriteProfileInt(_T("Margins"), _T("Right"), g_CxLibWork.m_rcMargins.right);
		g_IV460Profile.WriteProfileInt(_T("Margins"), _T("Top"), g_CxLibWork.m_rcMargins.top);
		g_IV460Profile.WriteProfileInt(_T("Margins"), _T("Bottom"), g_CxLibWork.m_rcMargins.bottom);
	}
	*/
}

void CPreviewStatic::DoMouseMove(UINT nFlags, CPoint point) 
{
	/*
	BOOL bEnableMargins = g_IV460Profile.GetProfileInt(_T("Settings"), _T("EnableMargins"), FALSE);
	if (m_szImg.cx > 0 && m_szImg.cy > 0 && bEnableMargins)
	{
		CRect rcClient;
		GetClientRect(rcClient);
		CPoint pt(point.x*m_szImg.cx/rcClient.Width(), point.y*m_szImg.cy/rcClient.Height());
		CRect rc(min(pt.x,m_ptBeg.x), min(pt.y,m_ptBeg.y), max(pt.x,m_ptBeg.x), max(pt.y,m_ptBeg.y));
		rc = CRect(rc.left, rc.top, m_szImg.cx-rc.right, m_szImg.cy-rc.bottom);
		rc = CRect(max(min(rc.left,100),0), max(min(rc.top,100),0), max(min(rc.right, 100),0), max(min(rc.bottom, 100),0));
//		g_CxLibWork.m_rcMargins = rc;
	}
	*/
}
