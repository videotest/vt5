// TabBeam.cpp : implementation file
//

#include "stdafx.h"
#include "stdsplitter.h"
#include "TabBeam.h"
#include "SplitterTabbed.h"
#include "TabEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabBeam

CTabBeam::CTabBeam() : m_rc(-1, -1, -1, -1)
{

	m_pEditor = new CTabEditor();

	m_nActiveTab = 0;
	m_numTabs = 0;
	m_lLeftVisiblePos = 0;
	m_nCurLeftVisTab = 0;
	m_lTabsWidth = 0;
	m_nMaxTabIndex = -1;
	m_partialRedraw = false;

	m_fontInActive.CreateFont(::GetSystemMetrics( SM_CYVSCROLL )/2, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, "MS Sans Serif");
	m_fontActive.CreateFont(::GetSystemMetrics( SM_CYVSCROLL )/2, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, "MS Sans Serif");
}


CTabBeam::~CTabBeam()
{
	for (int i = 0; i <= m_arrTabs.GetUpperBound(); i++)
	{
		CTab* pTab = (CTab*)m_arrTabs.GetAt(i);
		pTab->rgnTab.DeleteObject();
		delete pTab;
	}
	m_arrTabs.RemoveAll();

	if (m_pEditor)
	{
		delete m_pEditor;
	}

	m_fontInActive.DeleteObject();
	m_fontActive.DeleteObject();

}


BEGIN_MESSAGE_MAP(CTabBeam, CWnd)
	//{{AFX_MSG_MAP(CTabBeam)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabBeam message handlers



void CTabBeam::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CRect rc;
	GetClientRect( rc );
	CRgn rgn;

	long lHieght = rc.Height();
	//rc.InflateRect(0, 1, 0, 1);
	//dc.DrawEdge(&rc, EDGE_ETCHED, BF_RIGHT);	
	//dc.SelectStockObject( NULL_BRUSH );
	//dc.Rectangle( &rc );
	//GetClientRect( rc );
	
	//rc.InflateRect(0, 0, -2, 0);
	//rgn.CreateRectRgnIndirect((LPRECT)rc);
	//dc.SelectClipRgn(&rgn, RGN_COPY);


	if (m_partialRedraw)
	{
		if (m_rc != CRect(-1, -1, -1, -1) && m_rc != rc)
		{
			CRect tmp = m_rc;
			m_rc.SubtractRect(&rc, &m_rc);
			if (m_rc == CRect(0, 0, 0, 0))
			{
				m_rc = rc;
				return;

				//m_rc = tmp;
				//m_rc.SubtractRect(&m_rc, &rc);
				//m_rc.InflateRect(2, 0, -2, 0);
			}
			else
				//m_rc.InflateRect(2, 0, -2, 0);
			rgn.CreateRectRgnIndirect((LPRECT)m_rc);
			dc.SelectClipRgn(&rgn, RGN_COPY);
			m_rc = rc;
		}
		else
		{
			m_rc = rc;
			return;
		}	
	}

	m_rc = rc;

	dc.FillRect(&rc, &CBrush( ::GetSysColor(COLOR_3DFACE)));

	dc.MoveTo(rc.left, rc.top);
	dc.LineTo(rc.right, rc.top);
	
	//dc.DrawEdge(&rc, EDGE_ETCHED, BF_RIGHT);

	//if (m_partialRedraw)
	//{
		/*m_rc.InflateRect(2, 0, -2, 0);
		rgn2.CreateRectRgnIndirect((LPRECT)m_rc);
		dc.SelectClipRgn(&rgn2, RGN_COPY);*/
		
	//}

	CFont* pFont = dc.SelectObject(&m_fontInActive);

	//GetClientRect( rc );
	dc.SetWindowOrg(m_lLeftVisiblePos, 0);
	for (int i = 0; i <= m_arrTabs.GetUpperBound(); i++)
	{
		if (i == m_nActiveTab)
			continue;
		CTab* pTab = (CTab*)m_arrTabs.GetAt(i);
		dc.FillRgn(&pTab->rgnTab, &CBrush(::GetSysColor(COLOR_3DFACE)));
		dc.FrameRgn(&pTab->rgnTab, &CBrush(::GetSysColor(COLOR_BTNTEXT)), 1, 1);
		dc.SetBkColor(::GetSysColor( COLOR_3DFACE ));
		dc.SetBkMode(TRANSPARENT);
		dc.DrawText(pTab->strLabel, -1, &CRect(pTab->begPos, rc.top, pTab->endPos, rc.bottom), DT_CENTER);
	}

	dc.SelectObject(&m_fontActive);
	CTab* pTab = (CTab*)m_arrTabs.GetAt(m_nActiveTab);
	dc.FillRgn(&pTab->rgnTab, &CBrush(::GetSysColor(COLOR_3DFACE)));
	dc.FrameRgn(&pTab->rgnTab, &CBrush(::GetSysColor(COLOR_BTNTEXT)), 1, 1);
	pTab->rgnTab.OffsetRgn(0, -1);
	dc.FillRgn(&pTab->rgnTab, &CBrush(::GetSysColor(COLOR_WINDOW)));
	pTab->rgnTab.OffsetRgn(0, 1);
	dc.SetBkColor(::GetSysColor(COLOR_WINDOW));
	dc.SetBkMode(TRANSPARENT);
	dc.DrawText(pTab->strLabel, -1, &CRect(pTab->begPos, rc.top, pTab->endPos, rc.bottom), DT_CENTER);

	dc.SelectObject(pFont);
	
	// TODO: Add your message handler code here
	
	// Do not call CButton::OnPaint() for painting messages
}


void CTabBeam::AddTab(CString strLabel, bool setActive)
{
	m_numTabs++;
	m_nMaxTabIndex++;
	CWindowDC dc(this);
	CFont* pFont = dc.SelectObject(GetFont());
	CSize sizeLabel = dc.GetTextExtent(strLabel);
	// [vanek]: as rename view - 07.04.2004
	sizeLabel.cx = max(sizeLabel.cx, MinTabWidth);
	dc.SelectObject(pFont);
	CTab* pTab = new CTab();
	long lbegPos = m_lTabsWidth==0 ? 0 : m_lTabsWidth-CoveringNum;
	pTab->begPos = lbegPos;
	pTab->endPos = lbegPos + sizeLabel.cx + CoveringNum + CoveringNum;
	pTab->strLabel = strLabel;
	pTab->nIndex = m_nMaxTabIndex;
	m_lTabsWidth = pTab->endPos;

	CPoint points[4];
	points[1].y = ::GetSystemMetrics( SM_CYVSCROLL )-1;
	points[2].y = points[1].y;
	points[0] = CPoint(pTab->begPos, 0);
	points[1].x = pTab->begPos+CoveringNum+2;
	points[2].x = pTab->endPos-CoveringNum-2;
	points[3] = CPoint(pTab->endPos, 0);
	pTab->rgnTab.CreatePolygonRgn(points, 4, WINDING);
	m_arrTabs.Add(pTab); 
	//m_arrTabs.InsertAt(m_nMaxTabIndex, pTab);
	//SetActiveTab(m_nMaxTabIndex);
	SetActiveTab(m_numTabs-1, setActive);
}

void CTabBeam::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	//GetParent()->SendMessage(WM_LBUTTONDOWN, nFlags, MAKELONG(point.x, point.y));
	short tab1, tab2;
	if (GetTabFromPoint(point, tab1, tab2))
	{
		if (tab1 != m_nActiveTab)
		{
			if (tab2 >= 0)
				SetActiveTab(tab2);
			else
				SetActiveTab(tab1);
		}
		else
		{
			if (tab1 < m_nCurLeftVisTab)
				ScrollLeft();
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
	//SetFocus();
}

bool CTabBeam::NeedScrollLeft()
{
	if (m_numTabs == 1) return false;
	return m_lLeftVisiblePos > 0;
}

bool CTabBeam::NeedScrollRight()
{
	if (m_numTabs == 1) return false;
	CRect rc;
	GetClientRect( &rc );
	return (rc.Width() < m_lTabsWidth - m_lLeftVisiblePos);
}

void CTabBeam::SetActiveTab(short newActive, bool setView)
{
	if(newActive >= m_numTabs || newActive < 0) return;
	m_nActiveTab = newActive;
	CTab* pTab = (CTab*)m_arrTabs.GetAt(m_nActiveTab);
	CRect rc;
	GetClientRect(&rc);
	while (rc.right > 0 && pTab->endPos+CoveringNum-m_lLeftVisiblePos >= rc.right && pTab->begPos+CoveringNum-m_lLeftVisiblePos >= rc.left)
	{
		//m_lLeftVisiblePos -= rc.right - (pTab->endPos+CoveringNum);
		if (!ScrollRight()) break;
		TRACE0("RIGHT\n");
	}
	while (pTab->begPos+CoveringNum < m_lLeftVisiblePos)
	{
		if (!ScrollLeft()) break;
		TRACE0("LEFT\n");
	}

	((CSplitterTabbed*)GetParent())->SetButtons(NeedScrollLeft() ? 1 : 0, -1);
	((CSplitterTabbed*)GetParent())->SetButtons(-1, NeedScrollRight() ? 1 : 0);

	if (setView)
		((CSplitterTabbed*)GetParent())->SetActiveView(pTab->nIndex);
	Invalidate();
}

bool CTabBeam::GetTabFromPoint(CPoint p, short& tab1, short& tab2)
{
	long curPoint = 0;//m_lLeftVisiblePos;
	p.x += m_lLeftVisiblePos;
	short curTab = 0;
	CTab* pTab;
	while(p.x > curPoint && curTab <= m_arrTabs.GetUpperBound())
	{
		pTab = (CTab*)m_arrTabs.GetAt(curTab);
		curTab++;
		curPoint += pTab->endPos - pTab->begPos;
		if (curTab != 1)
		{
			curPoint -= CoveringNum;
		}
	}
	curTab--;
	if (curTab < 0)
		return false;
	pTab = (CTab*)m_arrTabs.GetAt(curTab);
	if (pTab->rgnTab.PtInRegion(p))
	{
		tab1 = curTab;
		//tab1 = ((CTab*)m_arrTabs.GetAt(curTab))->nIndex;
		tab2 = -1;
		if  (m_arrTabs.GetUpperBound() >= curTab+1)
		{
			pTab = (CTab*)m_arrTabs.GetAt(curTab+1);
			if (pTab->rgnTab.PtInRegion(p))
			{
				tab2 = curTab + 1;
				//tab2 = ((CTab*)m_arrTabs.GetAt(curTab+1))->nIndex;
			}
		}
		return true;
	}
	else
		return false;
}

bool CTabBeam::GetTabFromPos(LPOS lpos, short& ntab1, short& ntab2)
{
	ntab1 = ntab2 = -1;
	CTab* pTab = 0;
    for( int ntab = 0; ntab <= m_arrTabs.GetUpperBound(); ntab ++ )
	{

		pTab = (CTab*)(m_arrTabs.GetAt( ntab ));
		if( !pTab )
			continue;
        
		if( lpos >= pTab->begPos && lpos <= pTab->endPos )
		{
			if( ntab1 == -1 )
				ntab1 = ntab;
			else
			{
				ntab2 = ntab;
				break;
			}
		}
	}

	return true;
}

bool CTabBeam::ScrollLeft()
{
	if (m_nCurLeftVisTab-1 < 0) return false; 

	// [vanek]: check tab - 07.04.2004
    short nCurLeftVisTab = -1;
	GetTabFromPos( m_lLeftVisiblePos, nCurLeftVisTab, nCurLeftVisTab );
	if( nCurLeftVisTab == m_arrTabs.GetUpperBound() )
	{	// last tab
        m_lLeftVisiblePos = ((CTab*)m_arrTabs.GetAt(nCurLeftVisTab))->begPos;
		m_lLeftVisiblePos = max(m_lLeftVisiblePos-CoveringNum, 0);        
	}
	else
	{
		m_lLeftVisiblePos = ((CTab*)m_arrTabs.GetAt(m_nCurLeftVisTab-1))->begPos;
		m_lLeftVisiblePos = max(m_lLeftVisiblePos-CoveringNum, 0);
	}
	
	// [vanek]: check shifting tab - 07.04.2004
	GetTabFromPos( m_lLeftVisiblePos + CoveringNum, m_nCurLeftVisTab, m_nCurLeftVisTab );

	Invalidate();
	return true;
}

bool CTabBeam::ScrollRight()
{
	if (m_nCurLeftVisTab+1 > m_arrTabs.GetUpperBound()) 
	{
		if( m_nCurLeftVisTab == m_arrTabs.GetUpperBound() )
		{
			if( !NeedScrollRight() )
				return false;
			
			// [vanek]: show end of last tab - 07.04.2004
			CRect rc;
			GetClientRect( &rc );
			m_lLeftVisiblePos = ((CTab*)m_arrTabs.GetAt(m_nCurLeftVisTab))->endPos;
			m_lLeftVisiblePos = max(m_lLeftVisiblePos-rc.Width(), 0);
		}
		else
			return false;
	}
	else
	{
        m_lLeftVisiblePos = ((CTab*)m_arrTabs.GetAt(m_nCurLeftVisTab+1))->begPos;
		m_lLeftVisiblePos = max(m_lLeftVisiblePos-CoveringNum, 0);
	}
	
	// [vanek]: check shifting tab - 07.04.2004
	GetTabFromPos( m_lLeftVisiblePos + CoveringNum, m_nCurLeftVisTab, m_nCurLeftVisTab );
	Invalidate();
	return true;
}

void CTabBeam::DelTab(short Number, bool setActive)
{
	if (Number > m_nMaxTabIndex || Number < 0 || m_numTabs <= 1)	
		return;
	int idx = 0;
	CTab* pTab = (CTab*)m_arrTabs.GetAt(0);
	while (idx <= m_arrTabs.GetUpperBound() && Number != pTab->nIndex)
	{
		pTab = ((CTab*)m_arrTabs.GetAt(idx));
		idx++;
	}
	idx--;
	if (Number != pTab->nIndex)
		return;
	long lWidth = pTab->endPos - pTab->begPos;
	if (lWidth  > 0)
		lWidth -= CoveringNum;
	delete pTab;
	idx = max(0, idx);
	m_arrTabs.RemoveAt(idx);
	m_numTabs--;
	if (idx != m_numTabs)
	{
		for(short i = idx; i <= m_arrTabs.GetUpperBound(); i++)
		{
			pTab = (CTab*)m_arrTabs.GetAt(i);
			pTab->begPos -= lWidth;
			pTab->endPos -= lWidth;
			pTab->rgnTab.OffsetRgn(-lWidth, 0);
		}
	}
	else
	{
		m_nMaxTabIndex = ((CTab*)m_arrTabs.GetAt(idx-1))->nIndex; 
		((CSplitterTabbed*)GetParent())->m_maxNumView = m_nMaxTabIndex;
	}
	m_lTabsWidth -= lWidth;
	//SetActiveTab(min(m_numTabs-1, max(0, idx-1)));
	SetActiveTab(max(0, idx-1), setActive);
}

void CTabBeam::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CRect rc;
	GetParent()->GetClientRect(&rc);
	short tab1, tab2;
	if (GetTabFromPoint(point, tab1, tab2))
	{
		if (tab1 != m_nActiveTab)
		{
			if (tab2 >= 0)
				tab1 = tab2;
		}
	}
	else return;

	CTab* pTab = (CTab*)m_arrTabs.GetAt(tab1);
	rc.left = pTab->begPos + ::GetSystemMetrics( SM_CYVSCROLL )*2 - m_lLeftVisiblePos;
	rc.top =  rc.bottom - ::GetSystemMetrics( SM_CYVSCROLL )*3;
	rc.bottom = rc.top + ::GetSystemMetrics( SM_CYVSCROLL )*3/2;
	rc.right = min(rc.right-GetSystemMetrics( SM_CYVSCROLL ), GetSystemMetrics( SM_CYVSCROLL )*8 + rc.left);
	if (m_pEditor->m_hWnd)
	{
		m_pEditor->DestroyWindow();
	}
	((CEdit*)m_pEditor)->Create(WS_CHILD|WS_VISIBLE|WS_BORDER|ES_AUTOHSCROLL/*|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|WS_OVERLAPPED*/, rc, GetOwner(), 555);
	m_pEditor->pTabBeam = this;
	
	/*else
	{
		m_pEditor->MoveWindow(&rc, false);
		//m_pEditor->ShowWindow(SW_SHOW);
		m_pEditor->PostMessage(WM_SHOWWINDOW, WPARAM(TRUE), 0);
		//((CWnd*)m_pEditor)->SetWindowPos(&wndNoTopMost, rc.left, rc.top, rc.Width(), rc.Height(), SWP_SHOWWINDOW|SWP_NOOWNERZORDER|SWP_DRAWFRAME|SWP_FRAMECHANGED);
	}*/
	m_pEditor->SetWindowText(pTab->strLabel);
	m_pEditor->pEditTab = pTab;
	m_pEditor->SetFocus();
}

void CTabBeam::RenameTab(short idx, CString newName)
{
	if (idx > m_nMaxTabIndex || idx < 0 || newName.IsEmpty())	
		return;
	int i = 0;
	CTab* pTab = (CTab*)m_arrTabs.GetAt(0);
	while (i <= m_arrTabs.GetUpperBound() && idx != pTab->nIndex)
	{
		pTab = ((CTab*)m_arrTabs.GetAt(i));
		i++;
	}
	//i--;
	if (idx != pTab->nIndex)
		return;

	CWindowDC dc(this);
	CFont* pFont = dc.SelectObject(GetFont());
	CSize sizeLabel = dc.GetTextExtent(newName);
	dc.SelectObject(pFont);

	sizeLabel.cx = max(sizeLabel.cx, MinTabWidth);


	long oldEnd = pTab->endPos;

	pTab->endPos = pTab->begPos + sizeLabel.cx + CoveringNum + CoveringNum;
	pTab->strLabel = newName;


	long offset = pTab->endPos - oldEnd;

	m_lTabsWidth += offset;

	CPoint points[4];
	points[1].y = ::GetSystemMetrics( SM_CYVSCROLL )-1;
	points[2].y = points[1].y;
	points[0] = CPoint(pTab->begPos, 0);
	points[1].x = pTab->begPos+CoveringNum+2;
	points[2].x = pTab->endPos-CoveringNum-2;
	points[3] = CPoint(pTab->endPos, 0);
	pTab->rgnTab.DeleteObject();
	pTab->rgnTab.CreatePolygonRgn(points, 4, WINDING);

	ASSERT(i >= 0);
	if (i != m_numTabs)
	{
		if (i == 0) i = 1;
		for(short j = i; j <= m_arrTabs.GetUpperBound(); j++)
		{
			pTab = (CTab*)m_arrTabs.GetAt(j);
			pTab->begPos += offset;
			pTab->endPos += offset;
			pTab->rgnTab.OffsetRgn(offset, 0);
		}
	}

	((CSplitterTabbed*)GetParent())->SetButtons(NeedScrollLeft() ? 1 : 0, -1);
	((CSplitterTabbed*)GetParent())->SetButtons(-1, NeedScrollRight() ? 1 : 0);
	
	Invalidate();
}

void CTabBeam::GetTabName(short idx, CString &strName)
{
	if (idx > m_nMaxTabIndex || idx < 0 )	
		return;
	int i = 0;
	CTab* pTab = (CTab*)m_arrTabs.GetAt(0);
	while (i <= m_arrTabs.GetUpperBound() && idx != pTab->nIndex)
	{
		pTab = ((CTab*)m_arrTabs.GetAt(i));
		i++;
	}
	//i--;
	if (idx != pTab->nIndex)
		return;
	strName = pTab->strLabel;
}

void CTabBeam::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	((CSplitterTabbed*)GetParent())->SetButtons(NeedScrollLeft() ? 1 : 0, -1);
	((CSplitterTabbed*)GetParent())->SetButtons(-1, NeedScrollRight() ? 1 : 0);
	// TODO: Add your message handler code here
	
}

long CTabBeam::GetIndexByTab(long tab)
{
	if (tab > m_arrTabs.GetUpperBound()) return -1;
	return ((CTab*)m_arrTabs.GetAt(tab))->nIndex;
}
