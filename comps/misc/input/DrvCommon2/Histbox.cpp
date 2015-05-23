// HistBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "HistBox.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistBox

static LONG s_bInside;

CHistBox::CHistBox()
{
	memset(m_DataGray, 0, sizeof(m_DataGray));
	memset(m_DataRed, 0, sizeof(m_DataRed));
	memset(m_DataGreen, 0, sizeof(m_DataGreen));
	memset(m_DataBlue, 0, sizeof(m_DataBlue));
	m_method = hsmAll;
	m_fTrueColor = TRUE;
	m_nMax = m_nMin = m_nMaxR = m_nMinR = m_nMaxG = m_nMinG = m_nMaxB = m_nMinB = 0;
	m_nAvr = m_nAvrR = m_nAvrG = m_nAvrB = 0;
	m_nMaxValue = 0;
}

CHistBox::~CHistBox()
{
}


BEGIN_MESSAGE_MAP(CHistBox, CStatic)
	//{{AFX_MSG_MAP(CHistBox)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistBox message handlers

static int __correct(int n, int nMin, int nMax)
{
	if (n < nMin) n = nMin;
	else if (n > nMax) n = nMax;
	return n;
}

void CHistBox::Calc(CRect rc, POINT *ptGray, POINT *ptRed, POINT *ptGreen, POINT *ptBlue)
{
	memset(ptGray, 0, 256*sizeof(POINT));
	memset(ptRed, 0, 256*sizeof(POINT));
	memset(ptGreen, 0, 256*sizeof(POINT));
	memset(ptBlue, 0, 256*sizeof(POINT));
	if (m_nMaxValue == 0)
		return;
	int nMaxValue = m_nMaxValue;
	for (int i = 0; i < 256; i++)
		ptGray[i].x = ptRed[i].x = ptGreen[i].x = ptBlue[i].x = rc.left + i*rc.Width()/256;
	if (CanDrawBri())
		for (i = 0; i < 256; i++)
			ptGray[i].y = __correct(rc.bottom-rc.Height()*m_DataGray[i]/nMaxValue,rc.top,rc.bottom);
	if (CanDrawRed()&&m_fTrueColor)
		for (i = 0; i < 256; i++)
			ptRed[i].y = __correct(rc.bottom-rc.Height()*m_DataRed[i]/nMaxValue,rc.top,rc.bottom);
	if (CanDrawGreen()&&m_fTrueColor)
		for (i = 0; i < 256; i++)
			ptGreen[i].y = __correct(rc.bottom-rc.Height()*m_DataGreen[i]/nMaxValue,rc.top,rc.bottom);
	if (CanDrawBlue()&&m_fTrueColor)
		for (i = 0; i < 256; i++)
			ptBlue[i].y = __correct(rc.bottom-rc.Height()*m_DataBlue[i]/nMaxValue,rc.top,rc.bottom);
}

void CHistBox::OnDraw(CDC *pDC)
{
	CRect rc;
	GetClientRect(&rc);
	CBrush br(RGB(255,255,255));
	pDC->FillRect(&rc,&br);
	if (s_bInside) return;
	rc.top += 2;
	rc.bottom -= 2;
	POINT ptGray[256], ptRed[256], ptGreen[256], ptBlue[256];
	Calc(rc, ptGray, ptRed, ptGreen, ptBlue); 
	if (m_fTrueColor)
	{
		if (CanDrawRed())
			DrawPts(pDC, ptRed, RGB( 255, 0, 0 ));
		if (CanDrawGreen())
			DrawPts(pDC, ptGreen, RGB( 0, 255, 0 ));
		if( CanDrawBlue() )
			DrawPts(pDC, ptBlue, RGB( 0, 0, 255 ));
		if (CanDrawBri())
			DrawPts(pDC, ptGray, RGB( 0, 0, 0 ));
	}
	else
		if (CanDrawBri())
			DrawPts(pDC, ptGray, RGB( 0, 0, 0 ));
}


void CHistBox::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	OnDraw(&dc);
}


static int _CalcAvr(unsigned *p, unsigned nPoints)
{
	double dSum = 0.;
	for (int i = 1; i < 256; i++)
		dSum += double(i*p[i]);
	dSum /= double(nPoints);
	return int(dSum);
}

static void _CalcMinMax(unsigned *p, unsigned &nMaxValue, int &nMin, int &nMax)
{
	nMin = nMax = -1;
	for (int i = 0; i < 256; i++)
		if (p[i] > 0)
		{
			nMin = i;
			break;
		}
	for (i = 255; i >= nMin; i--)
		if (p[i] > 0)
		{
			nMax = i;
			break;
		}
	for (i = nMin; i <= nMax; i++)
		nMaxValue = max(nMaxValue,p[i]);
}

void CHistBox::InitHisto(LPBITMAPINFOHEADER lpbi, LPBYTE pData)
{
	if (GetSafeHwnd() == 0)
		return;
	if (!IsWindowVisible())
		return;
	if (!GetParent()->IsWindowVisible())
		return;
	InterlockedIncrement(&s_bInside);
	memset(m_DataGray, 0, sizeof(m_DataGray));
	memset(m_DataRed, 0, sizeof(m_DataRed));
	memset(m_DataGreen, 0, sizeof(m_DataGreen));
	memset(m_DataBlue, 0, sizeof(m_DataBlue));
	m_fTrueColor = lpbi->biBitCount == 24;
	BOOL bBri = CanDrawBri();
	BOOL bRed = CanDrawRed()&&m_fTrueColor;
	BOOL bGreen = CanDrawGreen()&&m_fTrueColor;
	BOOL bBlue = CanDrawBlue()&&m_fTrueColor;
	if (lpbi)
	{
		int nRow = ((lpbi->biWidth*lpbi->biBitCount/8+3)>>2)<<2;
		if (pData == NULL) pData = (LPBYTE)(lpbi+1);
		int nHeight = lpbi->biHeight;
		int nWidth = lpbi->biWidth;
		for (int i = 0; i < nHeight; i++)
		{
			LPBYTE pData1 = pData;
			for (int j = 0; j < nWidth; j++)
			{
				if (lpbi->biBitCount == 24)
				{
					if (bBri)
					{
						int nBright = Bright(pData1[2], pData1[1], pData1[0]);
						ASSERT(nBright >= 0 && nBright < 256);
						m_DataGray[nBright]++;
					}
					if (bRed) m_DataRed[pData1[2]]++;
					if (bGreen) m_DataGreen[pData1[1]]++;
					if (bBlue) m_DataBlue[pData1[0]]++;
					pData1 += 3;
				}
				else if (lpbi->biBitCount == 8)
				{
					if (bBri) m_DataGray[pData1[0]]++;
					if (bRed) m_DataRed[pData1[0]]++;
					if (bGreen) m_DataGreen[pData1[0]]++;
					if (bBlue) m_DataBlue[pData1[0]]++;
					pData1++;
				}
			}
			pData += nRow;
		}
		m_nMaxValue = 0;
		if (bBri) _CalcMinMax(m_DataGray, m_nMaxValue, m_nMin, m_nMax);
		else if (bRed) _CalcMinMax(m_DataRed, m_nMaxValue, m_nMinR, m_nMaxR);
		else if (bGreen) _CalcMinMax(m_DataGreen, m_nMaxValue, m_nMinG, m_nMaxG);
		else if (bBlue) _CalcMinMax(m_DataBlue, m_nMaxValue, m_nMinB, m_nMaxB);
		if (bBri) m_nAvr = _CalcAvr(m_DataGray, lpbi->biWidth*lpbi->biHeight);
		else if (bRed) m_nAvrR = _CalcAvr(m_DataRed, lpbi->biWidth*lpbi->biHeight);
		else if (bGreen) m_nAvrG = _CalcAvr(m_DataGreen, lpbi->biWidth*lpbi->biHeight);
		else if (bBlue) m_nAvrB = _CalcAvr(m_DataBlue, lpbi->biWidth*lpbi->biHeight);
	}
	InterlockedDecrement(&s_bInside);
	
	InvalidateRect(NULL);
}

void CHistBox::DrawRows( CDC *pDC, POINT *ppts, COLORREF crFinish )
{
	if( !CanDrawRow() )
		return;

	CRect rc;
	GetClientRect(&rc);
	rc.InflateRect( 0, -2 );

	COLORREF	cr;
	for (int i = 0; i < 256; i++ )
	{
		cr = RGB( GetRValue( crFinish )*i/255,
			GetGValue( crFinish )*i/255,
			GetBValue( crFinish )*i/255);

		CPen	pen( PS_SOLID, 0, cr ), *ppen;

		ppen = pDC->SelectObject( &pen );

		pDC->MoveTo( ppts[i].x, rc.bottom );
		pDC->LineTo( ppts[i].x, ppts[i].y );
		pDC->SelectObject( ppen );
	}
}

void CHistBox::DrawPts( CDC *pDC, POINT *ppts, COLORREF crFinish )
{
	CPen pen( PS_SOLID, 0, crFinish );
	pDC->SelectObject(&pen);
	pDC->Polyline(ppts, 256);
	pDC->SelectStockObject(BLACK_PEN);
}

