// FontViewer.cpp : implementation file
//

#include "stdafx.h"
#include "editor.h"
#include "FontViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFontViewer

CFontViewer::CFontViewer()
{
	m_clrText = RGB(0,0,0);
	m_clrBack = RGB(255,255,255);
	m_bTransparent = true;
	memset(&m_Font, 0, sizeof(LOGFONT));
}

CFontViewer::~CFontViewer()
{
}


BEGIN_MESSAGE_MAP(CFontViewer, CStatic)
	//{{AFX_MSG_MAP(CFontViewer)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFontViewer message handlers

void CFontViewer::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	CBrush* pOldBrush = 0;
	CRect rc = NORECT;
	GetClientRect(rc);
	dc.FillRect(&rc, &CBrush(::GetSysColor(COLOR_BTNFACE)));
	dc.SetTextColor(m_clrText);
	if(m_bTransparent)
	{
		dc.SetBkMode( TRANSPARENT);
	}
	else
	{
		dc.SetBkMode(OPAQUE);
		pOldBrush = dc.SelectObject(&CBrush(m_clrBack));
		dc.SetBkColor(m_clrBack);

		/*CRect	rect;
		GetClientRect( rect );
		dc.FillRect(&rect, &br);*/
	}
	CFont font;
	font.CreateFontIndirect(&m_Font);
	CFont *pOldFont = dc.SelectObject(&font);
	CString	str;
	GetWindowText(str);
	//dc.DrawText(str, &rc, DT_CALCRECT);
	dc.DrawText(str, &rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE);

		
	dc.SelectObject(pOldBrush);
	dc.SelectObject(pOldFont);
		
	//dc.TextOut(rc.left, rc.top, m_strText);
		
	
	// Do not call CStatic::OnPaint() for painting messages
}

void	CFontViewer::SetTextColor(COLORREF	color)
{
	m_clrText = color;
	Invalidate();
}

void	CFontViewer::SetBackColor(COLORREF	color)
{
	m_clrBack = color;
	Invalidate();
}

void	CFontViewer::SetTransparent(bool bTransparent)
{
	m_bTransparent = bTransparent;
	Invalidate();
}

void	CFontViewer::SetFont(const LOGFONT* lpLogFont)
{
	memcpy(&m_Font, lpLogFont, sizeof(LOGFONT));
	Invalidate();
}