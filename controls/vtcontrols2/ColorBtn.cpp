// ColorBtn.cpp : implementation file
//

#include "stdafx.h"
#include "vtcontrols2.h"
#include "ColorBtn.h"
#include "ColorBtnDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorBtn

CColorBtn::CColorBtn()
{
	m_Checked=FALSE;
	m_IsMouseOver=FALSE;
	m_IsFocused=false;
}
CColorBtn::~CColorBtn()
{
}


BEGIN_MESSAGE_MAP(CColorBtn, CButton)
	//{{AFX_MSG_MAP(CColorBtn)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorBtn message handlers

void CColorBtn::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	int index=GetDlgCtrlID()-IDC_COLOR1;

	CRect rc;
	GetClientRect(rc);
	dc.FillSolidRect(rc,::GetSysColor(COLOR_BTNFACE));
	if(m_IsMouseOver) dc.DrawEdge(rc,BDR_RAISEDINNER ,BF_RECT);
	
	rc.DeflateRect(2,2);
	if(m_Checked) rc.OffsetRect(1,1);
	COLORREF clr = ((CColorBtnDlg*)GetParent())->colors[index];
	if (((CColorBtnDlg*)GetParent())->m_bPalette)
		clr = ((CColorBtnDlg*)GetParent())->m_Palette[GetGValue(clr)];
	CBrush brush(clr);
	CPen pen(PS_SOLID,1,RGB(128,128,128));
	CBrush *oldBrush=dc.SelectObject(&brush);
	CPen *oldPen=dc.SelectObject(&pen);
//	dc.FillRect(rc,&brush);
	dc.Rectangle(rc);
	if(m_IsFocused)
	{
		rc.InflateRect(2,2);
		dc.DrawFocusRect(rc);
	}

	dc.SelectObject(oldPen);
	dc.SelectObject(oldBrush);
	
	// Do not call CButton::OnPaint() for painting messages
}

void CColorBtn::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if(!m_Checked)
	{
		m_Checked=TRUE;
		InvalidateRect(NULL);
	}
	
	CButton::OnLButtonDown(nFlags, point);
}

void CColorBtn::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default


	if(!m_IsMouseOver)
	{
	 m_IsMouseOver=TRUE;
	 InvalidateRect(NULL);
	}
	SetTimer(1,55,NULL);

	
	CButton::OnMouseMove(nFlags, point);
}

void CColorBtn::OnTimer(UINT_PTR nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	POINT pt;
	GetCursorPos(&pt);
	CRect rect;
    GetWindowRect (rect);

	if(!rect.PtInRect(pt))
	{
		m_IsMouseOver=FALSE;
		KillTimer(1);
		InvalidateRect(NULL);
	}

	//CButton::OnTimer(nIDEvent);
}

void CColorBtn::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	m_Checked=FALSE;
	
	CButton::OnLButtonUp(nFlags, point);
}

int CColorBtn::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CButton::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
    m_Checked=FALSE;
	m_IsMouseOver=FALSE;
	
	return 0;
}

void CColorBtn::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);

	m_IsFocused=true;
	
	// TODO: Add your message handler code here
	
}

void CColorBtn::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);

	m_IsFocused=false;
	
	// TODO: Add your message handler code here
	
}


