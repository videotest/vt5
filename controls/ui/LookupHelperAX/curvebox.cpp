// curvebox.cpp : implementation file
//

#include "stdafx.h"
#include "curvebox.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCurveBox

CCurveBox::CCurveBox()
{
	m_bColorDoc = true;
	m_nMode = 0;
	m_nDragPoint = -1;
	m_xPrev = m_yPrev = -1;
	m_nCurveNum = 1;
	m_bCurveEdit = true;
	
	InitData();
}

CCurveBox::~CCurveBox()
{
}

void CCurveBox::InitData()
{
	for (int i = 0; i < 256; i++)
	{
		m_DataGray[i] = i;
		m_DataBlue[i] = i;
		m_DataGreen[i] = i;
		m_DataRed[i] = i;
		for (int j = 0; j < 4; j++)
			m_DataMan[i][j] = i;
	}
	for (i = 0; i < 4; i++)
	{
		m_xHisto2Low[i] = 0;
		m_xHisto2Up[i] = 255;
	}
	for (i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
			m_yData4[i][j] = 255*i/4;
	}
	for (i = 0; i < 9; i++)
	{
		for (int j = 0; j < 4; j++)
			m_yData8[i][j] = 255*i/8;
	}
}

void CCurveBox::RecalcCompositeCurve()
{
	BYTE *pData = NULL;
	if (m_nCurveNum == 0)
		pData = m_DataGray;
	else if (m_nCurveNum == 1)
		pData = m_DataRed;
	else if (m_nCurveNum == 2)
		pData = m_DataGreen;
	else if (m_nCurveNum == 3)
		pData = m_DataBlue;
	double d;
	int i1,i2;
	for (int i = 0; i < 256; i++)
	{
		// Apply hignlight curve
		int c = i<=m_xHisto2Low[m_nCurveNum]?0:i>=m_xHisto2Up[m_nCurveNum]?255:
			255*(i-m_xHisto2Low[m_nCurveNum])/(m_xHisto2Up[m_nCurveNum]-m_xHisto2Low[m_nCurveNum]);
		// Apply 4-point curve
		d = ((double)c)/(255./4.);
		i1 = (int)(floor(d));
		i2 = (int)(ceil(d));
		c = m_yData4[i1][m_nCurveNum]+(int)((m_yData4[i2][m_nCurveNum]-m_yData4[i1][m_nCurveNum])*(d-i1));
		// Apply 8-point curve
		d = ((double)c)/(255./8.);
		i1 = (int)(floor(d));
		i2 = (int)(ceil(d));
		c = m_yData8[i1][m_nCurveNum]+(int)((m_yData8[i2][m_nCurveNum]-m_yData8[i1][m_nCurveNum])*(d-i1));
		// Apply manual curve
		c = m_DataMan[c][m_nCurveNum];
		if (c < 0) c = 0;
		if (c > 255) c = 255;
		pData[i] = c;
	}

	GetParent()->SendMessage(WM_CURVECHANGE);
}



BEGIN_MESSAGE_MAP(CCurveBox, CStatic)
	//{{AFX_MSG_MAP(CCurveBox)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveBox message handlers

const int g_nBoxSize = 3;

void CCurveBox::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRect rcClient;
	GetClientRect(rcClient);
	rcClient.DeflateRect(g_nBoxSize, g_nBoxSize);
	m_nDragPoint = -1;
	if (m_nMode == 0)
	{
		CPoint pt1(rcClient.left+rcClient.Width()*m_xHisto2Low[m_nCurveNum]/255,rcClient.bottom);
		CPoint pt2(rcClient.left+rcClient.Width()*m_xHisto2Up[m_nCurveNum]/255,rcClient.top);
		CRect rc1(pt1.x-g_nBoxSize,pt1.y-g_nBoxSize,pt1.x+g_nBoxSize,pt1.y+g_nBoxSize);
		CRect rc2(pt2.x-g_nBoxSize,pt2.y-g_nBoxSize,pt2.x+g_nBoxSize,pt2.y+g_nBoxSize);
		if (rc1.PtInRect(point))
			m_nDragPoint = 0;
		if (rc2.PtInRect(point))
			m_nDragPoint = 1;
	}
	else if (m_nMode == 1)
	{
		for (int i = 0; i <= 4; i++)
		{
			CPoint pt(rcClient.left+rcClient.Width()*i/4, rcClient.top+rcClient.Height()-
				rcClient.Height()*m_yData4[i][m_nCurveNum]/255);
			CRect rc(pt.x-g_nBoxSize,pt.y-g_nBoxSize,pt.x+g_nBoxSize,pt.y+g_nBoxSize);
			if (rc.PtInRect(point))
			{
				m_nDragPoint = i;
				break;
			}
		}
	}
	else if (m_nMode == 2)
	{
		for (int i = 0; i <= 8; i++)
		{
			CPoint pt(rcClient.left+rcClient.Width()*i/8, rcClient.top+rcClient.Height()-
				rcClient.Height()*m_yData8[i][m_nCurveNum]/255);
			CRect rc(pt.x-g_nBoxSize,pt.y-g_nBoxSize,pt.x+g_nBoxSize,pt.y+g_nBoxSize);
			if (rc.PtInRect(point))
			{
				m_nDragPoint = i;
				break;
			}
		}
	}
	if (m_nDragPoint != -1 || m_nMode == 3)
		SetCapture();
	m_xPrev = m_yPrev = -1;
//	CStatic::OnLButtonDown(nFlags, point);
}

void CCurveBox::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if (m_nDragPoint != -1 || m_nMode == 3)
	{
		m_nDragPoint = -1;
		m_xPrev = m_yPrev = -1;
		ReleaseCapture();
		RecalcCompositeCurve();
	}
	
//	CStatic::OnLButtonUp(nFlags, point);
}

static CPoint s_ptLast(-1, -1);
void CCurveBox::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRect rcInvalid = NORECT;
	GetClientRect(rcInvalid);

	if(s_ptLast == CPoint(-1, -1))
		s_ptLast = rcInvalid.TopLeft();

	if ((m_nDragPoint != -1 || m_nMode == 3) && (nFlags&MK_LBUTTON))
	{
		CRect rcClient = rcInvalid;
		if (point.x < rcClient.left)   point.x = rcClient.left;
		if (point.y < rcClient.top)    point.y = rcClient.top;
		if (point.x > rcClient.right)  point.x = rcClient.right;
		if (point.y > rcClient.bottom) point.y = rcClient.bottom;
		if (m_nMode == 0)
		{
			int x = 255*(point.x-rcClient.left)/rcClient.Width();
			if (m_nDragPoint == 0)
			{
				//rcInvalid = CRect(s_ptLast, point);

				m_xHisto2Low[m_nCurveNum] = x;
				if (m_xHisto2Up[m_nCurveNum] < x) m_xHisto2Up[m_nCurveNum] = x;
			}
			else //if (m_nDragPoint == 1)
			{
				m_xHisto2Up[m_nCurveNum] = x;
				if (m_xHisto2Low[m_nCurveNum] > x) m_xHisto2Low[m_nCurveNum] = x;
			}
			//rcInvalid.left = m_xHisto2Low[m_nCurveNum]*rcClient.Width()/255+rcClient.left;
			//rcInvalid.right = m_xHisto2Up[m_nCurveNum]*rcClient.Width()/255+rcClient.left;
			//rcInvalid.InflateRect(1,1);
		}
		else if (m_nMode == 1)
		{
			if (m_nDragPoint >= 0 && m_nDragPoint <= 4)
			{
				int y = 255*(rcClient.Height()-point.y+rcClient.top)/rcClient.Height();
				m_yData4[m_nDragPoint][m_nCurveNum] = y;
			}
		}
		else if (m_nMode == 2)
		{
			if (m_nDragPoint >= 0 && m_nDragPoint <= 8)
			{
				int y = 255*(rcClient.Height()-point.y+rcClient.top)/rcClient.Height();
				m_yData8[m_nDragPoint][m_nCurveNum] = y;
			}
		}
		else if (m_nMode == 3)
		{
			int yCur = 255*(rcClient.Height()-point.y+rcClient.top)/rcClient.Height();
			int xCur = 255*(point.x-rcClient.left)/rcClient.Width();
			ASSERT(xCur >= 0 && xCur < 256);
			if (m_xPrev > 0 && xCur-m_xPrev != 0)
			{
				ASSERT(m_xPrev >= 0 && m_xPrev < 256);
				if (m_xPrev < xCur)
				{
					for (int x = m_xPrev; x <= xCur; x++)
						m_DataMan[x][m_nCurveNum] = m_yPrev+(yCur-m_yPrev)*(x-m_xPrev)/(xCur-m_xPrev);
				}
				else
				{
					for (int x = xCur; x <= m_xPrev; x++)
						m_DataMan[x][m_nCurveNum] = yCur+(m_yPrev-yCur)*(x-xCur)/(m_xPrev-xCur);
				}
			}
			else
				m_DataMan[xCur][m_nCurveNum] = yCur;
			m_xPrev = xCur;
			m_yPrev = yCur;

			rcInvalid = CRect(s_ptLast, point);
			rcInvalid.top = rcClient.top;
			rcInvalid.bottom = rcClient.bottom;
			rcInvalid.NormalizeRect();
			rcInvalid.InflateRect(7, 0, 7, 0);
			//TRACE("\n***********************************   left=%d, top=%d, right=%d, bottom=%d\n", rcInvalid.left, rcInvalid.top, rcInvalid.right, rcInvalid.bottom);
		}

		InvalidateRect(&rcInvalid, FALSE);

		RecalcCompositeCurve();
	}

	s_ptLast = point;

	UpdateWindow();
//	CStatic::OnMouseMove(nFlags, point);
}


void CCurveBox::DrawCurve(CDC *pDC, CRect &rcClient, BYTE *pData, COLORREF clr, int nCurveNum)
{
	int i;
	bool bCurrent = m_nCurveNum == nCurveNum;
	CPen pen(PS_SOLID, 1, clr);
	CPen *pOld = (CPen *)pDC->SelectObject(&pen);
	if (m_nMode == -1 || !m_bCurveEdit)// || !bCurrent)
	{
		POINT pt[256];
		for (i = 0; i < 256; i++)
		{
			pt[i].x = rcClient.left+(rcClient.Width()*i)/255;
			pt[i].y = rcClient.top+rcClient.Height()-(rcClient.Height()*pData[i])/255;
			pt[i].x = min(pt[i].x, rcClient.left+rcClient.Width()-1);
			pt[i].y = min(pt[i].y, rcClient.top+rcClient.Height()-1);
		}
		pDC->Polyline(pt, 256);
	}
	else if (m_nMode == 0)
	{
		POINT pt[4];
		pt[0].x = rcClient.left;
		pt[0].y = rcClient.bottom-1;
		pt[1].x = rcClient.left+rcClient.Width()*m_xHisto2Low[nCurveNum]/255;
		pt[1].x = min(pt[1].x, rcClient.left+rcClient.Width()-1);
		pt[1].y = rcClient.bottom-1;
		pt[2].x = rcClient.left+rcClient.Width()*m_xHisto2Up[nCurveNum]/255;
		pt[2].x = min(pt[2].x, rcClient.left+rcClient.Width()-1);
		pt[2].y = rcClient.top;
		pt[3].x = rcClient.right;
		pt[3].y = rcClient.top;
		pDC->Polyline(pt, 4);
		if(bCurrent)
		{
			CRect rc1(pt[1].x-g_nBoxSize,pt[1].y-g_nBoxSize,pt[1].x+g_nBoxSize,pt[1].y+g_nBoxSize);
			CRect rc2(pt[2].x-g_nBoxSize,pt[2].y-g_nBoxSize,pt[2].x+g_nBoxSize,pt[2].y+g_nBoxSize);
			pDC->FillSolidRect(rc1, clr);
			pDC->FillSolidRect(rc2, clr);
		}
	}
	else if (m_nMode == 1)
	{
		POINT pt[5];
		for (int i = 0; i <= 4; i++)
		{
			pt[i].x = rcClient.left+rcClient.Width()*i/4;
			pt[i].y = rcClient.top+(rcClient.Height()-rcClient.Height()*m_yData4[i][nCurveNum]/255);
			pt[i].x = min(pt[i].x, rcClient.left+rcClient.Width()-1);
			pt[i].y = min(pt[i].y, rcClient.top+rcClient.Height()-1);
			if(bCurrent)
			{
				CRect rc(pt[i].x-g_nBoxSize,pt[i].y-g_nBoxSize,pt[i].x+g_nBoxSize,pt[i].y+g_nBoxSize);
				pDC->FillSolidRect(rc, clr);
			}
		}
		pDC->Polyline(pt, 5);
	}
	else if (m_nMode == 2)
	{
		POINT pt[9];
		for (int i = 0; i <= 8; i++)
		{
			pt[i].x = rcClient.left+rcClient.Width()*i/8;
			pt[i].y = rcClient.top+(rcClient.Height()-rcClient.Height()*m_yData8[i][nCurveNum]/255);
			pt[i].x = min(pt[i].x, rcClient.left+rcClient.Width()-1);
			pt[i].y = min(pt[i].y, rcClient.top+rcClient.Height()-1);
			if(bCurrent)
			{
				CRect rc(pt[i].x-g_nBoxSize,pt[i].y-g_nBoxSize,pt[i].x+g_nBoxSize,pt[i].y+g_nBoxSize);
				pDC->FillSolidRect(rc, clr);
			}
		}
		pDC->Polyline(pt, 9);
	}
	else // if (m_nMode == 3)
	{
		POINT pt[256];
		for (i = 0; i < 256; i++)
		{
			pt[i].x = rcClient.left+(rcClient.Width()*i)/255;
			pt[i].y = rcClient.top+(rcClient.Height()-(rcClient.Height()*m_DataMan[i][nCurveNum])/255);
			pt[i].x = min(pt[i].x, rcClient.left+rcClient.Width()-1);
			pt[i].y = min(pt[i].y, rcClient.top+rcClient.Height()-1);
		}
		pDC->Polyline(pt, 256);
	}
	pDC->SelectObject(pOld);
	pen.DeleteObject();
}

void CCurveBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	CRect rcClient;
	GetClientRect(rcClient);
	dc.FillSolidRect(rcClient, m_clrBack);

	CBrush bkBrush(m_clrBack);
	CBrush* pOldBrush = dc.SelectObject(&bkBrush);
	CRect rcBound(rcClient);

	rcBound.DeflateRect(g_nBoxSize,g_nBoxSize);
	dc.Rectangle(rcBound);
	//rcBound.DeflateRect(1,1,1,1);

	dc.IntersectClipRect(rcClient);

	//draw grid
	if(m_bShowGrid)
	{
		CDC dcMem;
		CRect rcBound2 = CRect(0, 0, rcBound.Width(), rcBound.Height());
		
		dcMem.CreateCompatibleDC(&dc);
		CBitmap bitmap;
		bitmap.CreateCompatibleBitmap(&dc, rcBound.Width(), rcBound.Height());
		CBitmap* pOldBmp = dcMem.SelectObject(&bitmap);

		dcMem.SetBkMode(TRANSPARENT);
		CBrush* pBrushOld = dcMem.SelectObject(&CBrush(/*m_clrBack*/(COLORREF)(255,255,255)));
		dcMem.Rectangle(rcBound2);
		dcMem.SelectObject(pBrushOld);

		
		//dcMem.FillSolidRect(rcBound2, m_clrBack);

		rcBound2.DeflateRect(1,1,1,1);
		

		float nH = rcBound.Height()/16.;
		float nW = rcBound.Width()/16.;
		CPen pen(PS_SOLID, 1, RGB(140, 140, 140));
		CPen* pOldPen = dcMem.SelectObject(&pen);
		for(int i = 1; i < 16; i++)
		{
			/*dc.MoveTo(rcBound.left + i*nW, rcBound.top+1);
			dc.LineTo(rcBound.left + i*nW, rcBound.bottom-1);
			dc.MoveTo(rcBound.left+1, rcBound.top + i*nH);
			dc.LineTo(rcBound.right-1, rcBound.top + i*nH);*/
			dcMem.MoveTo(i*nW, 1);
			dcMem.LineTo(i*nW, rcBound2.Height());
			dcMem.MoveTo(1, i*nH);
			dcMem.LineTo(rcBound2.Width(), i*nH);
		}
		dcMem.SelectObject(pOldPen);

		dc.BitBlt(rcBound.left, rcBound.top, rcBound.Width(), rcBound.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOldBmp);
	}

		
	
	// Draw non-current curves
	if (m_nCurveNum != 0 && m_bColorDoc && m_bCurveEdit) 
		DrawCurve(&dc, rcBound, m_DataGray, RGB(0, 0, 0), 0);
	if (m_nCurveNum != 1 && m_bColorDoc) 
		DrawCurve(&dc, rcBound, m_DataRed, RGB(120,0,0), 1);
	if (m_nCurveNum != 2 && m_bColorDoc) 
		DrawCurve(&dc, rcBound, m_DataGreen, RGB(0,120,0), 2);
	if (m_nCurveNum != 3 && m_bColorDoc) 
		DrawCurve(&dc, rcBound, m_DataBlue, RGB(0,0,120), 3);

	// Draw current curves
	if (m_nCurveNum == 0 && m_bCurveEdit) 
		DrawCurve(&dc, rcBound, m_DataGray, RGB(0, 0, 0), 0);
	if (m_nCurveNum == 1 && m_bColorDoc) 
		DrawCurve(&dc, rcBound, m_DataRed, RGB(255,0,0), 1);
	if (m_nCurveNum == 2 && m_bColorDoc) 
		DrawCurve(&dc, rcBound, m_DataGreen, RGB(0,255,0), 2);
	if (m_nCurveNum == 3 && m_bColorDoc) 
		DrawCurve(&dc, rcBound, m_DataBlue, RGB(0,0,255), 3);

	dc.SelectObject(pOldBrush);
}

void CCurveBox::SetBoxType(bool bCurveEdit)
{
	m_bCurveEdit = bCurveEdit;
	Invalidate();
}

void CCurveBox::SetCurveNum(int nCurveNum)
{
	m_nCurveNum = nCurveNum;
	Invalidate();
}

void CCurveBox::SetColorDoc(bool bColorDoc)
{
	m_bColorDoc = bColorDoc;
	Invalidate();
}


void CCurveBox::SetCurveBCG(BYTE *pGray, BYTE *pBlue, BYTE *pGreen, BYTE *pRed)
{
	//InitData();

	if(pGray)
		memcpy(m_DataGray, pGray, 256);
	if(pBlue)
		memcpy(m_DataBlue, pBlue, 256);
	if(pGreen)
		memcpy(m_DataGreen, pGreen, 256);
	if(pRed)
		memcpy(m_DataRed, pRed, 256);
	Invalidate();
}

void CCurveBox::GetCurveData(int nCurveNum, BYTE *pCurveData)
{
	if (nCurveNum == 1)
		memcpy(pCurveData, m_DataRed, 256);
	else if (nCurveNum == 2)
		memcpy(pCurveData, m_DataGreen, 256);
	else if (nCurveNum == 3)
		memcpy(pCurveData, m_DataBlue, 256);
	else
		memcpy(pCurveData, m_DataGray, 256);
}

void CCurveBox::SetMode(int nMode, bool bInvalidate)
{
	m_nMode = nMode;
	if(bInvalidate)
		Invalidate();
}

void CCurveBox::SetBackColor(COLORREF clr)
{
	m_clrBack = clr;
}

void CCurveBox::ShowGrid(bool bShowGrid, bool bInvalidate)
{
	m_bShowGrid = bShowGrid;
	if(bInvalidate)
		Invalidate();
}

BOOL CCurveBox::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	return TRUE;
}

void CCurveBox::GetState(byte* pState)
{
	for (int i = 0; i < 256; i++)
	{
		*pState = m_DataGray[i];
		pState++;
		*pState = m_DataBlue[i];
		pState++;
		*pState = m_DataGreen[i];
		pState++;
		*pState = m_DataRed[i];
		pState++;
		for (int j = 0; j < 4; j++)
		{
			*pState = m_DataMan[i][j];
			pState++;
		}
	}
	for (i = 0; i < 4; i++)
	{
		*pState = m_xHisto2Low[i];
		pState++;
		*pState = m_xHisto2Up[i];
		pState++;
	}
	for (i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			*pState = m_yData4[i][j];
			pState++;
		}
	}
	for (i = 0; i < 9; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			*pState = m_yData8[i][j];
			pState++;
		}
	}
}

void CCurveBox::SetState(byte* pState)
{
	for (int i = 0; i < 256; i++)
	{
		m_DataGray[i] = *pState;
		pState++;
		m_DataBlue[i] = *pState;
		pState++;
		m_DataGreen[i] = *pState;
		pState++;
		m_DataRed[i] = *pState;
		pState++;
		for (int j = 0; j < 4; j++)
		{
			m_DataMan[i][j] = *pState;
			pState++;
		}
	}
	for (i = 0; i < 4; i++)
	{
		m_xHisto2Low[i] = *pState;
		pState++;
		m_xHisto2Up[i] = *pState;
		pState++;
	}
	for (i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_yData4[i][j] = *pState;
			pState++;
		}
	}
	for (i = 0; i < 9; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m_yData8[i][j] = *pState;
			pState++;
		}
	}

	Invalidate();
}

void CCurveBox::OnDestroy() 
{
	OutputDebugString( "CCurveBox::OnDestroy()\n" );
	CStatic::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

void CCurveBox::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CStatic::OnShowWindow(bShow, nStatus);
	OutputDebugString( "CStatic::OnShowWindow(bShow, nStatus)\n" );
	
	// TODO: Add your message handler code here
	
}
