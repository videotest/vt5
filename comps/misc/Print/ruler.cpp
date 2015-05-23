/////////////////////////////////////////////////////////////////////////////
// Rulers. Written By Stefan Ungureanu (stefanu@usa.net)
//

#include "stdafx.h"
#include "Ruler.h"
//#include "MainFrm.h"
//#include "SomeView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRulerSplitterWnd

CRulerSplitterWnd::CRulerSplitterWnd()
{
	m_cxSplitter=3;
	m_cySplitter=3;
	m_cxBorderShare=0;
	m_cyBorderShare=0;
	m_cxSplitterGap=3;
	m_cySplitterGap=3;
}

CRulerSplitterWnd::~CRulerSplitterWnd()
{
}


BEGIN_MESSAGE_MAP(CRulerSplitterWnd, CSplitterWnd)
	//{{AFX_MSG_MAP(CRulerSplitterWnd)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CRulerSplitterWnd::HitTest(CPoint pt) const
{
	ASSERT_VALID(this);

//	return CSplitterWnd::HitTest(pt);
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CRulerSplitterWnd message handlers

/////////////////////////////////////////////////////////////////////////////
// CRulerView

IMPLEMENT_DYNCREATE(CRulerView, CView)

CRulerView::CRulerView()
{
	m_rulerType=RT_HORIZONTAL;
	m_scrollPos=0;
	m_lastPos=-1000000000;
}

CRulerView::~CRulerView()
{
}


BEGIN_MESSAGE_MAP(CRulerView, CView)
	//{{AFX_MSG_MAP(CRulerView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulerView drawing

void CRulerView::OnDraw(CDC* pDC)
{
	// clear the cursor position
	m_lastPos=-1000000000;

	/*
	CSomeDoc* pDoc = GetDocument();

	// outbar info...
	CMainFrame* pFrame=((CMainFrame*)AfxGetMainWnd());
	CGfxOutBarCtrl* pBar=&pFrame->m_wndBar;

	// if the bar is not valid (meaning application startup, just ignore the code below)
	if (!pBar) return;
	if (!::IsWindow(pBar->m_hWnd)) return;
	
	// don't draw if there is nothing to
	if (pBar->iLastSel==-1)
		return;

	// set the map mode right
	int oldMapMode=pDC->SetMapMode(MM_HIMETRIC);

	// get the document size
	CSize docSize=pDoc->m_sizeDoc;

	// drawing tools
	CPen wtPen(PS_SOLID, 10, GetSysColor(COLOR_WINDOWTEXT));
	CPen wbkPen(PS_SOLID, 10, GetSysColor(COLOR_WINDOW));
	CPen* pOldPen=pDC->SelectObject(&wbkPen);

	CBrush wbkBr(GetSysColor(COLOR_WINDOW));
	CBrush* pOldBrush=pDC->SelectObject(&wbkBr);

	CFont vFont;
	CFont hFont;
	vFont.CreateFont(300, 0, -900, -900, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|FF_ROMAN, "Times New Roman");
	hFont.CreateFont(400, 0, 000, 000, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_TT_ALWAYS, PROOF_QUALITY, VARIABLE_PITCH|FF_ROMAN, "Times New Roman");
	CFont* pOldFont=pDC->SelectObject((m_rulerType==RT_HORIZONTAL)?&hFont:&vFont);

	int oldTextAlign=pDC->SetTextAlign((m_rulerType==RT_HORIZONTAL)?(TA_RIGHT|TA_TOP):(TA_LEFT|TA_TOP));
	int oldBkMode=pDC->SetBkMode(TRANSPARENT);

	// draw a white rect...
	CRect rulerRect(CPoint(0, 0), docSize);
	CRect clientRect;
	GetClientRect(&clientRect);

	if (m_rulerType==RT_HORIZONTAL)
	{
		rulerRect.bottom=clientRect.bottom+10;
		rulerRect.right=(int)((float)rulerRect.right*GetDocument()->m_zoom);
	}
	else //(m_rulerType==RT_VERTICAL)
	{
		rulerRect.right=clientRect.right+10;
		rulerRect.bottom=(int)((float)rulerRect.bottom*GetDocument()->m_zoom);
	}

	rulerRect.bottom=-rulerRect.bottom*25;
	rulerRect.right=rulerRect.right*25;
	pDC->Rectangle(rulerRect);

	// draw the ruler margin
	pDC->SelectObject(&wtPen);

	if (m_rulerType==RT_HORIZONTAL)
	{
		pDC->MoveTo( rulerRect.right, rulerRect.top);
		pDC->LineTo(rulerRect.right, rulerRect.bottom);
	}
	else //(m_rulerType==RT_VERTICAL)
	{
		pDC->MoveTo(rulerRect.left, rulerRect.bottom);
		pDC->LineTo(rulerRect.right, rulerRect.bottom);
	}

	// draw the ticks (100)
	int tick100=(int)(2500.0*(GetDocument()->m_zoom));
	int tick10=(int)(250.0*(GetDocument()->m_zoom));

	for (int i=1; i<=docSize.cx/100; i++)
	{
		char buffer[100];
		sprintf(buffer, "%d", i*100);

		if (m_rulerType==RT_HORIZONTAL)
		{
			pDC->MoveTo(tick100*i-m_scrollPos, rulerRect.top);
			pDC->LineTo(tick100*i-m_scrollPos, rulerRect.bottom);
			pDC->TextOut(tick100*i-100-m_scrollPos, rulerRect.top+50, CString(buffer));
		}
		else //(m_rulerType==RT_VERTICAL)
		{
			pDC->MoveTo(rulerRect.left, -tick100*i-m_scrollPos);
			pDC->LineTo(rulerRect.right, -tick100*i-m_scrollPos);
			pDC->TextOut(rulerRect.left, -tick100*i+100-m_scrollPos, CString(buffer));
		}
	}

	// draw the ticks (10)
	for (i=1; i<docSize.cx/10; i++)
	{
		if (m_rulerType==RT_HORIZONTAL)
		{
			pDC->MoveTo(tick10*i-m_scrollPos, rulerRect.top-300);
			pDC->LineTo(tick10*i-m_scrollPos, rulerRect.bottom);
		}
		else //(m_rulerType==RT_VERTICAL)
		{
			pDC->MoveTo(rulerRect.left+300, -tick10*i-m_scrollPos);
			pDC->LineTo(rulerRect.right, -tick10*i-m_scrollPos);
		}
	}

	// restore DC settings
	pDC->SetMapMode(oldMapMode);
	pDC->SelectObject(pOldPen);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(pOldFont);
	pDC->SetTextAlign(oldTextAlign);
	pDC->SetBkMode(oldBkMode);
	*/
}

/////////////////////////////////////////////////////////////////////////////
// CRulerView diagnostics

#ifdef _DEBUG
void CRulerView::AssertValid() const
{
	CView::AssertValid();
}

void CRulerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
/*
CSomeDoc* CRulerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSomeDoc)));
	return (CSomeDoc*)m_pDocument;
}
*/
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRulerView message handlers

BOOL CRulerView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// create a brush using the same color as the background
	if (cs.lpszClass == NULL)
	{
		HBRUSH hBr=CreateSolidBrush(GetSysColor(COLOR_MENU));
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS|CS_BYTEALIGNWINDOW, NULL, hBr);
	}

	return CView::PreCreateWindow(cs);
}

void CRulerView::SetRulerType(UINT rulerType)
{
	m_rulerType=rulerType;
}

void CRulerView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
}

void CRulerView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
	/*
	// outbar info...
	CMainFrame* pFrame=((CMainFrame*)AfxGetMainWnd());
	CGfxOutBarCtrl* pBar=&pFrame->m_wndBar;

	// if the bar is not valid (meaning application startup, just ignore the code below)
	if (!pBar) return;
	if (!::IsWindow(pBar->m_hWnd)) return;

	// do nothing if the update is not for us
	if (((m_rulerType==RT_HORIZONTAL)&&(lHint==VW_VSCROLL))||
		((m_rulerType==RT_VERTICAL)&&(lHint==VW_HSCROLL)))
		return;

	if (((m_rulerType==RT_HORIZONTAL)&&(lHint==VW_VPOSITION))||
		((m_rulerType==RT_VERTICAL)&&(lHint==VW_HPOSITION)))
		return;

	if ((lHint==VW_VSCROLL)||(lHint==VW_HSCROLL))
	{
		// otherwise invalidate
		m_scrollPos=(int)pHint;

		//	Invalidate();
		// repaint the ruler
		CDC* pDC=GetDC();
		OnDraw(pDC);
		ReleaseDC(pDC);

		return;
	}
	else
	{
		if (((lHint==VW_VPOSITION)||(lHint==VW_HPOSITION))&&
			(pBar->iLastSel!=-1))
		{
			int newPos=(int)pHint;
			if (((m_rulerType==RT_HORIZONTAL)&&
				 (newPos>GetDocument()->m_sizeDoc.cx*GetDocument()->m_zoom*25))||
				((m_rulerType==RT_VERTICAL)&&
				 ((-newPos)>GetDocument()->m_sizeDoc.cy*GetDocument()->m_zoom*25)))
				return;

			CRect clientRect;
			GetClientRect(&clientRect);

			CDC* pDC=GetDC();

			// set the map mode right
			int oldMapMode=pDC->SetMapMode(MM_HIMETRIC);

			CPen pen(PS_SOLID, 10, GetSysColor(COLOR_WINDOW));
			CPen* pOldPen=pDC->SelectObject(&pen);

			int oldROP2=pDC->SetROP2(R2_XORPEN);

			if (m_rulerType==RT_HORIZONTAL)
			{
				// erase the previous position
				pDC->MoveTo(m_lastPos-m_scrollPos, -clientRect.top*25);
				pDC->LineTo(m_lastPos-m_scrollPos, -clientRect.bottom*25);

				m_lastPos=newPos;
				// draw the new position
				pDC->MoveTo(m_lastPos-m_scrollPos, -clientRect.top*25);
				pDC->LineTo(m_lastPos-m_scrollPos, -clientRect.bottom*25);
			}

			if (m_rulerType==RT_VERTICAL)
			{
				// erase the previous position
				pDC->MoveTo(clientRect.left*25, m_lastPos-m_scrollPos);
				pDC->LineTo(clientRect.right*25, m_lastPos-m_scrollPos);

				m_lastPos=newPos;
				// draw the new position
				pDC->MoveTo(clientRect.left*25, m_lastPos-m_scrollPos);
				pDC->LineTo(clientRect.right*25, m_lastPos-m_scrollPos);
			}

			pDC->SetROP2(oldROP2);
			pDC->SelectObject(pOldPen);
			pDC->SetMapMode(oldMapMode);

			ReleaseDC(pDC);

			return;
		}
	}

	// don't know what to do, invalidate all
	Invalidate();
	*/
}

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView

IMPLEMENT_DYNCREATE(CRulerCornerView, CView)

CRulerCornerView::CRulerCornerView()
{
}

CRulerCornerView::~CRulerCornerView()
{
}


BEGIN_MESSAGE_MAP(CRulerCornerView, CView)
	//{{AFX_MSG_MAP(CRulerCornerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView drawing

void CRulerCornerView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView diagnostics

#ifdef _DEBUG
void CRulerCornerView::AssertValid() const
{
	CView::AssertValid();
}

void CRulerCornerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRulerCornerView message handlers

BOOL CRulerCornerView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// create a brush using the same color as the background
	if (cs.lpszClass == NULL)
	{
		HBRUSH hBr=CreateSolidBrush(GetSysColor(COLOR_MENU));
		cs.lpszClass = AfxRegisterWndClass(CS_DBLCLKS|CS_BYTEALIGNWINDOW, NULL, hBr);
	}

	return CView::PreCreateWindow(cs);
}
