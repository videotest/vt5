#include "StdAfx.h"
#include "AppDefs.h"
#include "RectHand.h"
#include "Resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma warning(disable:4244)

IMPLEMENT_DYNAMIC(CRectHandObserver,CObservable)

CRectHandObserver::CRectHandObserver()
{
#if defined(__VT4__)
	g_ConfigManager.m_Observable.AddObserver(this);
#endif
}

CRectHandObserver::~CRectHandObserver()
{
#if defined(__VT4__)
	g_ConfigManager.m_Observable.RemoveObserver(this);
#endif
}

void CRectHandObserver::OnUpdateObserver(CObject *pObservable, int nHint, CObject *pObjHint)
{
#if defined(__VT4__)
	if (nHint == CConfigObservable::setInt && (*(CString *)pObjHint) == "DrawCurve"
		|| nHint == CConfigObservable::selectState)
	{
		CRectHandler::Init();
		Update();
	}
#endif
}

IMPLEMENT_SERIAL(CRectHandler,CObject,0);

CRectHandObserver *CRectHandler::m_pObserver = NULL;
BOOL CRectHandler::s_bDrawCurve;

void CRectHandler::Init()
{
#if defined(__VT4__)
	s_bDrawCurve = AfxGetApp()->GetProfileInt("Settings", "DrawCurve", -1);
	if (s_bDrawCurve == -1)
		s_bDrawCurve = g_ConfigManager.GetInt("DrawCurve", 0, "General");
	if (m_pObserver == NULL)
		m_pObserver = new CRectHandObserver;
#endif
//	g_ProgLog << "s_bDrawCurve = " << s_bDrawCurve << "\n";
}

void CRectHandler::Done()
{
	safedelete(m_pObserver);
}

CRectHandler::CRectHandler(double fFract /*= -1*/)
{
	m_fFract  = fFract;
	m_rcInit  = NORECT;
	m_fSin    = 0.;
	m_fCos    = 1.;
	m_ptCen   = CDblPoint(0.,0.);
	m_fScaleX = 1.;
	m_fScaleY = 1.;
	memset(m_pt, 0, 5*sizeof(CPoint));

	m_rcText = NORECT;
	m_fAngle = 0.0;
}

void CRectHandler::operator=(CRectHandler &Sample)
{
	m_rcInit  = Sample.m_rcInit;
	m_fSin    = Sample.m_fSin;
	m_fCos    = Sample.m_fCos;
	m_fFract  = Sample.m_fFract;
	m_ptCen   = Sample.m_ptCen;
	m_fScaleX = Sample.m_fScaleX;
	m_fScaleY = Sample.m_fScaleY;
	m_fAngle  = Sample.m_fAngle;
	memcpy(m_pt,Sample.m_pt,sizeof(m_pt));
	if (m_rcText != NORECT)
		m_rcText = Sample.m_rcText;
}

CRectHandler &CRectHandler::operator+=(CPoint ptOff)
{
	if (m_rcText != NORECT)
		m_rcText += ptOff;
	m_rcInit += ptOff;
	m_ptCen += ptOff;
	for (int i = 0; i < 5; i++)
		m_pt[i] += ptOff;
	for (i = 0; i < 2; i++)
		m_ptMed[i] += ptOff;
	for (i = 0; i < m_Curve.GetSize(); i++)
	{
		CPoint pt(m_Curve.GetAt(i));
		pt += ptOff;
		m_Curve.SetAt(i,pt);
	}
	return *this;
}

#define SZBOX_SIZE      nSizeBox
#define MVBOX_SIZE      nMoveBox
#define RTBOX_SIZE      nRotateBox

#define SZBOX_SIZE0     4
#define MVBOX_SIZE0     2
#define RTBOX_SIZE0     2
#define ALLBOX_SIZE0    10

#define SZBOX_SIZE1     8
#define MVBOX_SIZE1     4
#define RTBOX_SIZE1     4
#define ALLBOX_SIZE1    25

#define SZBOX_SIZE2     20
#define MVBOX_SIZE2     10
#define RTBOX_SIZE2     10
#define ALLBOX_SIZE2    100

void CRectHandler::DrawBoxes( CDC &theDC, POINT *pt, BOOL bXor, double fZoom )
{
	int nSizeBox;
	int nMoveBox;
	int nRotateBox;
	// Prepare drawing tools for draw size boxes.
	int clrRotate = bXor?RGB(128,255,255):RGB(0,128,0);
	int clrSize   = bXor?RGB(255,255,128):RGB(0,255,0);
	int clrLine   = bXor?RGB(255,128,255):RGB(128,0,128);
	CBrush brSize(clrSize);
	CPen   penSize(PS_SOLID,0,clrSize);
	POINT ptCur[5];
	CBrush *pbrOld = (CBrush *)theDC.SelectObject( &brSize );
	ASSERT(pbrOld);
	CPen *ppenOld = (CPen *)theDC.SelectObject( &penSize );
	ASSERT(ppenOld);
	int nPrevRop;
	if (bXor)
		nPrevRop = theDC.SetROP2(R2_XORPEN);
	// Select propriate sizes of boxes
	if (m_rcInit.Width() * fZoom * m_fScaleX < ALLBOX_SIZE1 || 
		m_rcInit.Height() * fZoom * m_fScaleY < ALLBOX_SIZE1)
	{
		nSizeBox   = SZBOX_SIZE0;
		nMoveBox   = MVBOX_SIZE0;
		nRotateBox = RTBOX_SIZE0;
	}
	else if (m_rcInit.Width() * fZoom * m_fScaleX < ALLBOX_SIZE2 || 
		m_rcInit.Height() * fZoom * m_fScaleY < ALLBOX_SIZE2)
	{
		nSizeBox   = SZBOX_SIZE1;
		nMoveBox   = MVBOX_SIZE1;
		nRotateBox = RTBOX_SIZE1;
	}
	else
	{
		nSizeBox   = SZBOX_SIZE2;
		nMoveBox   = MVBOX_SIZE2;
		nRotateBox = RTBOX_SIZE2;
	}
//	nSizeBox = (nSizeBox/int(2*fZoom)+1)*int(2*fZoom);
//	nMoveBox = (nMoveBox/int(2*fZoom)+1)*int(2*fZoom);
//	nRotateBox = (nRotateBox/int(fZoom)+1)*int(fZoom);
	// Draw proportional size boxes.
	double dx = ((double)SZBOX_SIZE) / (m_rcInit.Width() * fZoom * m_fScaleX);
	double dy = ((double)SZBOX_SIZE) / (m_rcInit.Height() * fZoom * m_fScaleY);
	if (dx < 0.5 && dy < 0.5)
	{
		// Upper left.
		ptCur[0].x = pt[0].x;
		ptCur[0].y = pt[0].y;
		ptCur[1].x = pt[0].x + (pt[1].x-pt[0].x)*dx;
		ptCur[1].y = pt[0].y + (pt[1].y-pt[0].y)*dx;
		ptCur[2].x = ptCur[1].x + (pt[2].x-pt[1].x)*dy;
		ptCur[2].y = ptCur[1].y + (pt[2].y-pt[1].y)*dy;
		ptCur[3].x = pt[0].x + (pt[3].x-pt[0].x)*dy;
		ptCur[3].y = pt[0].y + (pt[3].y-pt[0].y)*dy;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Upper right.
		ptCur[0].x = pt[1].x + (pt[0].x-pt[1].x)*dx;
		ptCur[0].y = pt[1].y + (pt[0].y-pt[1].y)*dx;
		ptCur[1].x = pt[1].x;
		ptCur[1].y = pt[1].y;
		ptCur[2].x = pt[1].x + (pt[2].x-pt[1].x)*dy;
		ptCur[2].y = pt[1].y + (pt[2].y-pt[1].y)*dy;
		ptCur[3].x = ptCur[0].x + (pt[3].x-pt[0].x)*dy;
		ptCur[3].y = ptCur[0].y + (pt[3].y-pt[0].y)*dy;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Lower right.
		ptCur[1].x = pt[2].x + (pt[1].x-pt[2].x)*dy;
		ptCur[1].y = pt[2].y + (pt[1].y-pt[2].y)*dy;
		ptCur[0].x = ptCur[1].x + (pt[0].x-pt[1].x)*dx;
		ptCur[0].y = ptCur[1].y + (pt[0].y-pt[1].y)*dx;
		ptCur[2].x = pt[2].x;
		ptCur[2].y = pt[2].y;
		ptCur[3].x = pt[2].x + (pt[3].x-pt[2].x)*dx;
		ptCur[3].y = pt[2].y + (pt[3].y-pt[2].y)*dx;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Lower left.
		ptCur[0].x = pt[3].x + (pt[0].x-pt[3].x)*dy;
		ptCur[0].y = pt[3].y + (pt[0].y-pt[3].y)*dy;
		ptCur[1].x = ptCur[0].x + (pt[1].x-pt[0].x)*dx;
		ptCur[1].y = ptCur[0].y + (pt[1].y-pt[0].y)*dx;
		ptCur[2].x = pt[3].x + (pt[2].x-pt[3].x)*dx;
		ptCur[2].y = pt[3].y + (pt[2].y-pt[3].y)*dx;
		ptCur[3].x = pt[3].x;
		ptCur[3].y = pt[3].y;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
	} // if (dx < 0.5 && dy < 0.5)
	// Draw size boxes.
	CPoint ptVert(pt[2].x-pt[1].x,pt[2].y-pt[1].y);
	CPoint ptHorz(pt[1].x-pt[0].x,pt[1].y-pt[0].y);
	dx = ((double)MVBOX_SIZE) / (m_rcInit.Width() * fZoom * m_fScaleX);
	dy = ((double)MVBOX_SIZE) / (m_rcInit.Height() * fZoom * m_fScaleY);
	CPoint ptOrg;
	if (m_rcInit.Width() * fZoom * m_fScaleX > 2 * SZBOX_SIZE + MVBOX_SIZE &&
		m_rcInit.Height() * fZoom * m_fScaleY > 2 * SZBOX_SIZE + MVBOX_SIZE)
	{
		// Upper.
		ptOrg = CPoint((pt[1].x+pt[0].x)/2,(pt[1].y+pt[0].y)/2);
		ptCur[0].x = ptOrg.x - ptHorz.x*dx/2;
		ptCur[0].y = ptOrg.y - ptHorz.y*dx/2;
		ptCur[1].x = ptCur[0].x + ptHorz.x*dx;
		ptCur[1].y = ptCur[0].y + ptHorz.y*dx;
		ptCur[3].x = ptCur[0].x + ptVert.x*dy;
		ptCur[3].y = ptCur[0].y + ptVert.y*dy;
		ptCur[2].x = ptCur[3].x + ptHorz.x*dx;
		ptCur[2].y = ptCur[3].y + ptHorz.y*dx;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Right.
		ptOrg = CPoint((pt[2].x+pt[1].x)/2,(pt[2].y+pt[1].y)/2);
		ptCur[1].x = ptOrg.x - ptVert.x*dy/2;
		ptCur[1].y = ptOrg.y - ptVert.y*dy/2;
		ptCur[0].x = ptCur[1].x - ptHorz.x*dx;
		ptCur[0].y = ptCur[1].y - ptHorz.y*dx;
		ptCur[2].x = ptCur[1].x + ptVert.x*dy;
		ptCur[2].y = ptCur[1].y + ptVert.y*dy;
		ptCur[3].x = ptCur[2].x - ptHorz.x*dx;
		ptCur[3].y = ptCur[2].y - ptHorz.y*dx;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Lower.
		ptOrg = CPoint((pt[3].x+pt[2].x)/2,(pt[3].y+pt[2].y)/2);
		ptCur[3].x = ptOrg.x - ptHorz.x*dx/2;
		ptCur[3].y = ptOrg.y - ptHorz.y*dx/2;
		ptCur[2].x = ptCur[3].x + ptHorz.x*dx;
		ptCur[2].y = ptCur[3].y + ptHorz.y*dx;
		ptCur[0].x = ptCur[3].x - ptVert.x*dy;
		ptCur[0].y = ptCur[3].y - ptVert.y*dy;
		ptCur[1].x = ptCur[2].x - ptVert.x*dy;
		ptCur[1].y = ptCur[2].y - ptVert.y*dy;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Left.
		ptOrg = CPoint((pt[0].x+pt[3].x)/2,(pt[0].y+pt[3].y)/2);
		ptCur[0].x = ptOrg.x - ptVert.x*dy/2;
		ptCur[0].y = ptOrg.y - ptVert.y*dy/2;
		ptCur[3].x = ptCur[0].x + ptVert.x*dy;
		ptCur[3].y = ptCur[0].y + ptVert.y*dy;
		ptCur[1].x = ptCur[0].x + ptHorz.x*dx;
		ptCur[1].y = ptCur[0].y + ptHorz.y*dx;
		ptCur[2].x = ptCur[3].x + ptHorz.x*dx;
		ptCur[2].y = ptCur[3].y + ptHorz.y*dx;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Draw line boxes
		if (m_fFract >= 0 && s_bDrawCurve)
		{
			CBrush brLine(clrLine);
			CPen   penLine(PS_SOLID,0,clrLine);
			VERIFY(theDC.SelectObject( &brLine ));
			VERIFY(theDC.SelectObject( &penLine ));
			CPoint ptOrg1(pt[0].x+(pt[1].x-pt[0].x)*(1-m_fFract),
				pt[0].y+(pt[1].y-pt[0].y)*(1-m_fFract));
			CPoint ptOrg2(pt[3].x+(pt[2].x-pt[3].x)*(1-m_fFract),
				pt[3].y+(pt[2].y-pt[3].y)*(1-m_fFract));
//			ptOrg = CPoint((ptOrg1.x+ptOrg2.x)/2,(ptOrg1.y+ptOrg2.y)/2);
			ptOrg = CPoint(ptOrg1.x+(ptOrg2.x-ptOrg1.x)/3,
				ptOrg1.y+(ptOrg2.y-ptOrg1.y)/3);
			double dx1 = min(dx,2*FDist(pt[0],ptOrg1)/(m_rcInit.Width()*fZoom));
			double dx2 = min(dx,2*FDist(pt[1],ptOrg1)/(m_rcInit.Width()*fZoom));
			ptCur[0].x = ptOrg.x - ptVert.x*dy/2 - ptHorz.x*dx1/2;
			ptCur[0].y = ptOrg.y - ptVert.y*dy/2 - ptHorz.y*dx1/2;
			ptCur[1].x = ptOrg.x - ptVert.x*dy/2 + ptHorz.x*dx2/2;
			ptCur[1].y = ptOrg.y - ptVert.y*dy/2 + ptHorz.y*dx2/2;
			ptCur[2].x = ptOrg.x + ptVert.x*dy/2 + ptHorz.x*dx2/2;
			ptCur[2].y = ptOrg.y + ptVert.y*dy/2 + ptHorz.y*dx2/2;
			ptCur[3].x = ptOrg.x + ptVert.x*dy/2 - ptHorz.x*dx1/2;
			ptCur[3].y = ptOrg.y + ptVert.y*dy/2 - ptHorz.y*dx1/2;
			ptCur[4].x = ptCur[0].x;
			ptCur[4].y = ptCur[0].y;
			VERIFY(theDC.Polygon(ptCur,5));
		}
	} // if (..)
	// Prepare drawing tools for draw rotate boxes
	CBrush brRotate(clrRotate);
	CPen   penRotate(PS_SOLID,0,clrRotate);
	VERIFY(theDC.SelectObject( &brRotate ));
	VERIFY(theDC.SelectObject( &penRotate ));
	//Draw rotate boxes
	dx = ((double)RTBOX_SIZE) / (m_rcInit.Width() * fZoom * m_fScaleX);
	dy = ((double)RTBOX_SIZE) / (m_rcInit.Height() * fZoom * m_fScaleY);
	if (dx < 0.5 && dy < 0.5)
	{
		// Upper left.
		ptCur[0].x = pt[0].x;
		ptCur[0].y = pt[0].y;
		ptCur[1].x = pt[0].x + (pt[1].x-pt[0].x)*dx;
		ptCur[1].y = pt[0].y + (pt[1].y-pt[0].y)*dx;
		ptCur[2].x = ptCur[1].x + (pt[2].x-pt[1].x)*dy;
		ptCur[2].y = ptCur[1].y + (pt[2].y-pt[1].y)*dy;
		ptCur[3].x = pt[0].x + (pt[3].x-pt[0].x)*dy;
		ptCur[3].y = pt[0].y + (pt[3].y-pt[0].y)*dy;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Upper right.
		ptCur[0].x = pt[1].x + (pt[0].x-pt[1].x)*dx;
		ptCur[0].y = pt[1].y + (pt[0].y-pt[1].y)*dx;
		ptCur[1].x = pt[1].x;
		ptCur[1].y = pt[1].y;
		ptCur[2].x = pt[1].x + (pt[2].x-pt[1].x)*dy;
		ptCur[2].y = pt[1].y + (pt[2].y-pt[1].y)*dy;
		ptCur[3].x = ptCur[0].x + (pt[3].x-pt[0].x)*dy;
		ptCur[3].y = ptCur[0].y + (pt[3].y-pt[0].y)*dy;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Lower right.
		ptCur[1].x = pt[2].x + (pt[1].x-pt[2].x)*dy;
		ptCur[1].y = pt[2].y + (pt[1].y-pt[2].y)*dy;
		ptCur[0].x = ptCur[1].x + (pt[0].x-pt[1].x)*dx;
		ptCur[0].y = ptCur[1].y + (pt[0].y-pt[1].y)*dx;
		ptCur[2].x = pt[2].x;
		ptCur[2].y = pt[2].y;
		ptCur[3].x = pt[2].x + (pt[3].x-pt[2].x)*dx;
		ptCur[3].y = pt[2].y + (pt[3].y-pt[2].y)*dx;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
		// Lower left.
		ptCur[0].x = pt[3].x + (pt[0].x-pt[3].x)*dy;
		ptCur[0].y = pt[3].y + (pt[0].y-pt[3].y)*dy;
		ptCur[1].x = ptCur[0].x + (pt[1].x-pt[0].x)*dx;
		ptCur[1].y = ptCur[0].y + (pt[1].y-pt[0].y)*dx;
		ptCur[2].x = pt[3].x + (pt[2].x-pt[3].x)*dx;
		ptCur[2].y = pt[3].y + (pt[2].y-pt[3].y)*dx;
		ptCur[3].x = pt[3].x;
		ptCur[3].y = pt[3].y;
		ptCur[4].x = ptCur[0].x;
		ptCur[4].y = ptCur[0].y;
		VERIFY(theDC.Polygon(ptCur,5));
	}
	VERIFY(theDC.SelectObject( pbrOld ));
	VERIFY(theDC.SelectObject( ppenOld ));
	if (bXor)
		theDC.SetROP2(nPrevRop);
}

void CRectHandler::DrawCurve( CDC &theDC, BOOL bXor, double fZoom)
{
	ASSERT(FALSE); // not supported here
}

void CRectHandler::DrawLine( CDC &theDC, BOOL bXor, double fZoom)
{
	ASSERT(FALSE); // not supported here
}

void CRectHandler::SetInitRect(CRect rc)
{
	m_rcInit = rc;
	m_fSin   = 0.;
	m_fCos   = 1.;
	m_fScaleX = 1.;
	m_fScaleY = 1.;
	m_fAngle = 0.0;
	if (m_fFract != -1)
		m_fFract = 0.5;
	CalcData();
}

void CRectHandler::GetData(CPoint *pt, CPoint *ptMed, CRect &rcInit,
	CDblPoint &ptCen, double &fSin, double &fCos, double &fFract)
{
	memcpy(pt,m_pt,5*sizeof(CPoint));
	rcInit = m_rcInit;
	ptCen = m_ptCen;
	fSin = m_fSin;
	fCos = m_fCos;
	fFract = m_fFract;
}

int CRectHandler::GetZone(CPoint pto, double fZoom)
{
	int nSizeBox;
	int nMoveBox;
	int nRotateBox;
	if (m_rcInit.Width() * fZoom * m_fScaleX < ALLBOX_SIZE1 || 
		m_rcInit.Height() * fZoom * m_fScaleY < ALLBOX_SIZE1)
	{
		nSizeBox   = SZBOX_SIZE0;
		nMoveBox   = MVBOX_SIZE0;
		nRotateBox = RTBOX_SIZE0;
	}
	else if (m_rcInit.Width() * fZoom * m_fScaleX < ALLBOX_SIZE2 || 
		m_rcInit.Height() * fZoom * m_fScaleY < ALLBOX_SIZE2)
	{
		nSizeBox   = SZBOX_SIZE1;
		nMoveBox   = MVBOX_SIZE1;
		nRotateBox = RTBOX_SIZE1;
	}
	else
	{
		nSizeBox   = SZBOX_SIZE2;
		nMoveBox   = MVBOX_SIZE2;
		nRotateBox = RTBOX_SIZE2;
	}
	CDblPoint pt(pto);
	CDblPoint pt0((double(m_pt[0].x)+0.5)*fZoom, (double(m_pt[0].y)+0.5)*fZoom);
	CDblPoint pt1((double(m_pt[1].x)+0.5)*fZoom, (double(m_pt[1].y)+0.5)*fZoom);
	CDblPoint pt2((double(m_pt[2].x)+0.5)*fZoom, (double(m_pt[2].y)+0.5)*fZoom);
	CDblPoint pt3((double(m_pt[3].x)+0.5)*fZoom, (double(m_pt[3].y)+0.5)*fZoom);
	// distance from left side
	double dLeft = FDistLine(pt, pt0, pt3);
	// distance from top side
	double dTop = FDistLine(pt, pt0, pt1);
	// distance from right side
	double dRight = FDistLine(pt, pt1, pt2);
	// distance from bottom side
	double dBottom = FDistLine(pt, pt2, pt3);
	// real sizes of all object
	double dX = max(FDistLine(pt0,pt1,pt2),FDistLine(pt1,pt0,pt3));
	double dY = max(FDistLine(pt2,pt0,pt1),FDistLine(pt0,pt2,pt3));
	if (dLeft > dX || dRight > dX)
		return Outside;
	if (dTop > dY || dBottom > dY)
		return Outside;
	CDblPoint ptPos(dLeft,dTop);
	CDblSize  szAll(dX, dY);
	if (szAll.cx < 2 * SZBOX_SIZE || szAll.cy < 2 * SZBOX_SIZE)
		return Move;
	if (ptPos.x <= SZBOX_SIZE && ptPos.y <= SZBOX_SIZE)
	{
		if (ptPos.x <= RTBOX_SIZE && ptPos.y <= RTBOX_SIZE)
			return Rotate0;
		else
			return SizeSynch0;
	}
	if (ptPos.x >= szAll.cx-SZBOX_SIZE && ptPos.x <= szAll.cx && ptPos.y <= SZBOX_SIZE)
	{
		if (ptPos.x >= szAll.cx-RTBOX_SIZE && ptPos.x <= szAll.cx && ptPos.y <= RTBOX_SIZE)
			return Rotate1;
		else
			return SizeSynch1;
	}
	if (ptPos.x <= SZBOX_SIZE && ptPos.y >= szAll.cy-SZBOX_SIZE && ptPos.y <= szAll.cy)
	{
		if (ptPos.x <= RTBOX_SIZE && ptPos.y >= szAll.cy-RTBOX_SIZE  && ptPos.y <= szAll.cy)
			return Rotate3;
		else
			return SizeSynch3;
	}
	if (ptPos.x >= szAll.cx-SZBOX_SIZE && ptPos.x <= szAll.cx &&
		ptPos.y >= szAll.cy-SZBOX_SIZE && ptPos.y <= szAll.cy)
	{
		if (ptPos.x >= szAll.cx-RTBOX_SIZE && ptPos.x <= szAll.cx &&
			ptPos.y >= szAll.cy-RTBOX_SIZE && ptPos.y <= szAll.cy)
			return Rotate2;
		else
			return SizeSynch2;
	}
	if (szAll.cx < MVBOX_SIZE + 2 * SZBOX_SIZE ||
		szAll.cy < MVBOX_SIZE + 2 * SZBOX_SIZE)
		return Move;
	if (ptPos.x >= szAll.cx/2-MVBOX_SIZE/2 && ptPos.x <= szAll.cx/2+MVBOX_SIZE/2 &&
		ptPos.y <= MVBOX_SIZE)
		return SizeTop;
	if (ptPos.x >= szAll.cx/2-MVBOX_SIZE/2 && ptPos.x <= szAll.cx/2+MVBOX_SIZE/2 &&
		ptPos.y >= szAll.cy-MVBOX_SIZE && ptPos.y <= szAll.cy)
		return SizeBottom;
	if (ptPos.x <= MVBOX_SIZE &&
		ptPos.y >= szAll.cy/2-MVBOX_SIZE/2 && ptPos.y <= szAll.cy/2+MVBOX_SIZE/2)
		return SizeLeft;
	if (ptPos.x >= szAll.cx-MVBOX_SIZE && ptPos.x <= szAll.cx &&
		ptPos.y >= szAll.cy/2-MVBOX_SIZE/2 && ptPos.y <= szAll.cy/2+MVBOX_SIZE/2)
		return SizeRight;
	if (m_fFract >= 0)
	{
		int cx = szAll.cx*(1-m_fFract);
		if (ptPos.x >= cx-MVBOX_SIZE/2 && ptPos.x <= cx+MVBOX_SIZE/2 &&
			ptPos.y >= szAll.cy/3-MVBOX_SIZE/2 && ptPos.y <= szAll.cy/3+MVBOX_SIZE/2)
			return Line;
	}
	return Move;
}

#define MIN_OBSIZE  10

void CRectHandler::SetContour(POINT *pt)
{
	CDblPoint ptCen(((double)pt[0].x+pt[2].x)/2,((double)pt[0].y+pt[2].y)/2);
	CSize sz(FDistLine(pt[0],pt[1],pt[2]),FDistLine(pt[0],pt[3],pt[2]));
	sz.cx = max(sz.cx,MIN_OBSIZE);
	sz.cy = max(sz.cy,MIN_OBSIZE);
	CRect rc(ptCen.x-sz.cx/2,ptCen.y-sz.cy/2,0,0);
	rc.right  = rc.left + sz.cx;
	rc.bottom = rc.top  + sz.cy;
	ModifyInitRect(rc.left,rc.top,rc.right,rc.bottom);
	if (m_fSin != 0. || m_fCos != 1.)
		CalcAngle(ptCen,CPoint(rc.left,rc.top),pt[0],m_fSin,m_fCos);
	CalcData();
}

void CRectHandler::SetContour(double fSin, double fCos)
{
	m_fSin = fSin;
	m_fCos = fCos;
	CalcData();
}

void CRectHandler::Rotate(double fSin, double fCos, double fAngle )
{
	m_fSin = fSin;
	m_fCos = fCos;
	m_fAngle = fAngle;
	CalcData();
}

void CRectHandler::Resize()
{
	CalcData();
}

void CRectHandler::SetContour(POINT ptOffs)
{
	CRect rc(m_rcInit.left+ptOffs.x,m_rcInit.top+ptOffs.y,
		m_rcInit.right+ptOffs.x,m_rcInit.bottom+ptOffs.y);
	ModifyInitRect(rc.left,rc.top,rc.right,rc.bottom);
	CalcData();
}

void CRectHandler::SetLine(double fFract)
{
	m_fFract = fFract;
	CalcData();
}

BOOL CRectHandler::OnSetCursor(CPoint ptPos, double fZoom)
{
	int nZone = GetZone(ptPos,fZoom);
	if (nZone == Outside) return FALSE;
	return SetZoneCursor(nZone,Sin());
}

BOOL CRectHandler::SetZoneCursor(int nZone, double fSin)
{
	ASSERT(FALSE); // not supported here
	return FALSE;
}

CRect CRectHandler::CalcInvalidateRect()
{
	CRect rc(m_pt[0].x,m_pt[0].y,m_pt[0].x,m_pt[0].y);
	for (int i = 1; i <= 3; i++)
	{
		if (m_pt[i].x < rc.left)
			rc.left = m_pt[i].x;
		if (m_pt[i].x > rc.right)
			rc.right = m_pt[i].x;
		if (m_pt[i].y < rc.top)
			rc.top = m_pt[i].y;
		if (m_pt[i].y > rc.bottom)
			rc.bottom = m_pt[i].y;
	}

	if( m_rcText != NORECT )
	{
		rc.left = min( m_rcText.left, rc.left );
		rc.right = max( m_rcText.right, rc.right );
		rc.top = min( m_rcText.top, rc.top );
		rc.bottom = max( m_rcText.bottom, rc.bottom );
	}
	rc.left--;
	rc.top--;
	rc.right+=2;
	rc.bottom+=2;
	return rc;
}

CPoint CRectHandler::CalcOffs(int nZone, CPoint ptBmp)
{
	if (nZone == CRectHandler::SizeSynch0)
		return CPoint(ptBmp.x-m_pt[0].x,ptBmp.y-m_pt[0].y);
	else if (nZone == CRectHandler::SizeSynch1)
		return CPoint(ptBmp.x-m_pt[1].x,ptBmp.y-m_pt[1].y);
	else if (nZone == CRectHandler::SizeSynch2)
		return CPoint(ptBmp.x-m_pt[2].x,ptBmp.y-m_pt[2].y);
	else if (nZone == CRectHandler::SizeSynch3)
		return CPoint(ptBmp.x-m_pt[3].x,ptBmp.y-m_pt[3].y);
	return CPoint(0,0);
}


// protected members

void CRectHandler::CalcData()
{
	BEGIN_LOG_METHOD(CalcData,2);
	// Calc array of points.
	CRect rc;
	try {
	rc = CalcRealRect();
	} catch(...) {
	};


	::InitPoints(rc, m_pt);
	
	// Calc center.
	m_ptCen = CPoint(((double)rc.left + rc.right) / 2,
		((double)rc.top + rc.bottom) / 2);
	// Rotate quadriliteral on given angle.
	

	::Rotate(m_ptCen, m_pt, 5, m_fSin, m_fCos);
//	CheckRect(m_pt,m_rcInit);
	

	m_pt[4].x = m_pt[0].x;
	m_pt[4].y = m_pt[0].y;
	if (m_fFract != -1)
	{
		// Calc intermediate line.
		m_ptMed[0].x = m_pt[0].x*m_fFract+m_pt[1].x*(1-m_fFract);
		m_ptMed[0].y = m_pt[0].y*m_fFract+m_pt[1].y*(1-m_fFract);
		m_ptMed[1].x = m_pt[3].x*m_fFract+m_pt[2].x*(1-m_fFract);
		m_ptMed[1].y = m_pt[3].y*m_fFract+m_pt[2].y*(1-m_fFract);
		// Calc parabollic curve.
		m_Curve.RemoveAll();
		if (m_fFract == 1)
			m_Curve.RemoveAll();
		CSize  szAll(DistLine(m_pt[0],m_pt[1],m_pt[2]),DistLine(m_pt[2],m_pt[0],m_pt[1]));
		CPoint ptMed(m_pt[0].x*m_fFract+m_pt[1].x*(1-m_fFract),
			m_pt[0].y*m_fFract+m_pt[1].y*(1-m_fFract));
		CSize  szX1(ptMed.x-m_pt[0].x,ptMed.y-m_pt[0].y);
		CSize  szX2(m_pt[1].x-ptMed.x,m_pt[1].y-ptMed.y);
		CSize  szY(m_pt[3].x-m_pt[0].x,m_pt[3].y-m_pt[0].y);
		int x0 = ceil(szAll.cx*(1-m_fFract));
		if (x0 > 0)
		{
			for (int i = x0; i >= 0; i--)
			{
				double xd = ((double)(x0-i))/x0;
				double yd = xd*xd;
				CPoint pt(ceil(ptMed.x-szX1.cx*xd+szY.cx*yd),ceil(ptMed.y-szX1.cy*xd+szY.cy*yd));
				m_Curve.InsertAt(0,pt);
			}
		}
		if (szAll.cx-x0 > 0)
		{
			for (int i = x0; i < szAll.cx; i++)
			{
				double xd = ((double)(i-x0))/(szAll.cx-x0);
				double yd = xd*xd;
				CPoint pt(ceil(ptMed.x+szX2.cx*xd+szY.cx*yd),ceil(ptMed.y+szX2.cy*xd+szY.cy*yd));
				m_Curve.Add(pt);
			}
		}
	}
	END_LOG_METHOD();
}

void CRectHandler::ModifyInitRect(int l, int t, int r, int b)
{
	m_rcInit = CRect(l,t,r,b);
}


CPoint CRectHandler::CalcNewScale(int nAngle, CPoint pt, CPoint ptOffs)
{
	CSize szOld(m_rcInit.Width() * m_fScaleX, m_rcInit.Height() * m_fScaleY);
	// Proportional sizing operations
	double fScaleX = m_fScaleX;
	double fScaleY = m_fScaleY;
	if (nAngle == 0)
	{
		double fNewDX,fNewDY;
		::FDistLine(CPoint(pt.x-ptOffs.x,pt.y-ptOffs.y),m_pt[1],
			m_pt[2], &fNewDX);
		::FDistLine(CPoint(pt.x-ptOffs.x,pt.y-ptOffs.y),m_pt[3],
			m_pt[2], &fNewDY);
		double d = min(fNewDX/(m_rcInit.Width()*m_fScaleX),
			fNewDY/(m_rcInit.Height()*m_fScaleY));
		fScaleX *= d;
		fScaleY *= d;
	}
	else if (nAngle == 1)
	{
		double fNewDX,fNewDY;
		::FDistLine(CPoint(pt.x - ptOffs.x, pt.y - ptOffs.y), m_pt[0],
			m_pt[3], &fNewDX);
		::FDistLine(CPoint(pt.x - ptOffs.x, pt.y - ptOffs.y), m_pt[2],
			m_pt[3], &fNewDY);
		double d = min(fNewDX/(m_rcInit.Width()*m_fScaleX),
			fNewDY/(m_rcInit.Height()*m_fScaleY));
		fScaleX *= d;
		fScaleY *= d;
	}
	else if (nAngle == 2)
	{
		double fNewDX,fNewDY;
		::FDistLine(CPoint(pt.x-ptOffs.x,pt.y-ptOffs.y),
			m_pt[0],m_pt[3],&fNewDX);
		::FDistLine(CPoint(pt.x-ptOffs.x,pt.y-ptOffs.y),
			m_pt[0],m_pt[1],&fNewDY);
		double d = min(fNewDX/(m_rcInit.Width()*m_fScaleX),
			fNewDY/(m_rcInit.Height()*m_fScaleY));
		fScaleX *= d;
		fScaleY *= d;
	}
	else if (nAngle == 3)
	{
		double fNewDX,fNewDY;
		::FDistLine(CPoint(pt.x-ptOffs.x,pt.y-ptOffs.y),
			m_pt[1],m_pt[2],&fNewDX);
		::FDistLine(CPoint(pt.x-ptOffs.x,pt.y-ptOffs.y),
			m_pt[0],m_pt[1],&fNewDY);
		double d = min(fNewDX/(m_rcInit.Width()*m_fScaleX),
			fNewDY/(m_rcInit.Height()*m_fScaleY));
		fScaleX *= d;
		fScaleY *= d;
	}
	// Sizing operations
	else if (nAngle == 4)
	{
		// Top.
		// In sizing operation one edge already stay unchanged,
		// another edge moving.
		// In this case stable edge - (2,3).
		double fNewDY;
		FDistLine(pt,m_pt[2],m_pt[3],&fNewDY);
		fScaleY = fNewDY/m_rcInit.Height();
	}
	else if (nAngle == 5)
	{
		// Left.
		// Stable edge - (1,2).
		double fNewDX;
		FDistLine(pt,m_pt[2],m_pt[1],&fNewDX);
		fScaleX = fNewDX/m_rcInit.Width();
	}
	else if (nAngle == 6)
	{
		// Bottom.
		// Stable edge - (0,1).
		double fNewDY;
		FDistLine(pt,m_pt[0],m_pt[1],&fNewDY);
		fScaleY = fNewDY/m_rcInit.Height();
	}
	else if (nAngle == 7)
	{
		// Right.
		// Stable edge - (3,0).
		double fNewDX;
		FDistLine(pt,m_pt[0],m_pt[3],&fNewDX);
		fScaleX = fNewDX/m_rcInit.Width();
	}
	double xLim = min(m_rcInit.Width(), 6);
	m_fScaleX = max(fScaleX, xLim/m_rcInit.Width());
	double yLim = min(m_rcInit.Height(), 6);
	m_fScaleY = max(fScaleY, yLim/m_rcInit.Height());
	ASSERT(m_fScaleX > 0 && m_fScaleY > 0);
	CSize szNew(m_rcInit.Width() * m_fScaleX, m_rcInit.Height() * m_fScaleY);
	CPoint ptNOffs;
	if (nAngle == 0)
		ptNOffs = CPoint(szOld.cx-szNew.cx,szOld.cy-szNew.cy);
	else if (nAngle == 1)
		ptNOffs = CPoint(0,szOld.cy-szNew.cy);
	else if (nAngle == 2)
		ptNOffs = CPoint(0,0);
	else if (nAngle == 3)
		ptNOffs = CPoint(szOld.cx-szNew.cx,0);
	else if (nAngle == 4)
		ptNOffs = CPoint(0,szOld.cy-szNew.cy);
	else if (nAngle == 5)
		ptNOffs = CPoint(szOld.cx-szNew.cx,0);
	else if (nAngle == 6 || nAngle == 7)
		ptNOffs = CPoint(0,0);
	return ptNOffs;
}

void CRectHandler::Serialize(CArchive &ar)
{
	if (ar.IsLoading())
	{
		ar >> m_rcInit.left;
		ar >> m_rcInit.top;
		ar >> m_rcInit.right;
		ar >> m_rcInit.bottom;
		ar >> m_fSin;
		ar >> m_fCos;
		ar >> m_fScaleX;
		ar >> m_fScaleY;
		ar >> m_fFract;
		CalcData();
	}
	else
	{
		ar << m_rcInit.left;
		ar << m_rcInit.top;
		ar << m_rcInit.right;
		ar << m_rcInit.bottom;
		ar << m_fSin;
		ar << m_fCos;
		ar << m_fScaleX;
		ar << m_fScaleY;
		ar << m_fFract;
	}
}

#pragma optimize( "", off )

CDblRect CRectHandler::CalcRealRect()
{
	CDblRect r(m_rcInit.left, m_rcInit.top,
		m_rcInit.left + m_rcInit.Width() * m_fScaleX,
		m_rcInit.top + m_rcInit.Height() * m_fScaleY);
	return r;
}

#pragma optimize( "", on )
