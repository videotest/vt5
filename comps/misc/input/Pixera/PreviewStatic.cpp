// PreviewStatic.cpp : implementation file
//

#include "stdafx.h"
#include "pixera.h"
#include "PreviewStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic

CPreviewStatic::CPreviewStatic(IUnknown *punk, int nDevice)
{
	m_sptrPrv = punk;
	if (m_sptrPrv == 0)
		AfxThrowNotSupportedException();
	m_nDev = nDevice;
}

CPreviewStatic::~CPreviewStatic()
{
}


BEGIN_MESSAGE_MAP(CPreviewStatic, CStatic)
	//{{AFX_MSG_MAP(CPreviewStatic)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPreviewStatic message handlers

void CPreviewStatic::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rcSrc(CPoint(0,0),m_szPrv),rcDst;
	GetClientRect(rcDst);
	m_sptrPrv->DrawRect(m_nDev, dc.m_hDC, rcSrc, rcDst);
}

void CPreviewStatic::InitSizes(bool bOk)
{
	if (bOk)
	{
		short cx,cy;
		m_sptrPrv->GetSize(m_nDev,&cx,&cy);
		m_szPrv = CSize(cx,cy);
	}
	else
		m_szPrv = CSize(600,400);
}

void CPreviewStatic::DoLButtonDown(UINT nFlags, CPoint point)
{
	m_sptrPrv->LButtonDown(nFlags, point);
}

void CPreviewStatic::DoLButtonUp(UINT nFlags, CPoint point)
{
	m_sptrPrv->LButtonUp(nFlags, point);
}

void CPreviewStatic::DoMouseMove(UINT nFlags, CPoint point)
{
	m_sptrPrv->MouseMove(nFlags, point);
}

void CPreviewStatic::DoSetCursor(UINT nFlags, CPoint point)
{
	m_sptrPrv->SetCursor(nFlags, point);
}
