#include "StdAfx.h"
#include "MeasManual.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#pragma warning ( disable: 4244 )

IMPLEMENT_DYNAMIC(CObjHandyBase, CObjMeasure);
IMPLEMENT_SERIAL(CObjLength, CObjHandyBase, VERSIONABLE_SCHEMA|3);
IMPLEMENT_SERIAL(CObjFreeLength, CObjHandyBase, VERSIONABLE_SCHEMA|3);
IMPLEMENT_SERIAL(CObjAngle, CObjHandyBase, VERSIONABLE_SCHEMA|3);
IMPLEMENT_SERIAL(CObjRadius, CObjAngle, VERSIONABLE_SCHEMA|3);
IMPLEMENT_SERIAL(CObjXYRatio, CObjLength, VERSIONABLE_SCHEMA|3);


/////////////////////////////////////////////////////////////////////
// Методы класса CObjHandyBase - базового класса для объектов ручных измерений
CObjHandyBase::CObjHandyBase()
: CObjMeasure()
{
	m_pView = 0;
	m_fZoom = 0;
}


void CObjHandyBase::OnBeginMeasure(CScrollView *pView, double iZoom)
{
	m_pView = pView;
	m_fZoom =  iZoom > 0 ? iZoom : -1.0/iZoom;
}

void CObjHandyBase::OnEndMeasure()
{
	m_pView = 0;
}

void CObjHandyBase::DoPaint( CDC &theDC, double iZoom, int iType, BOOL bActive )
{
	ASSERT(FALSE); // Not implemented here
}

void CObjHandyBase::DoPaintInt(CDC &theDC, int iType )
{
}

void CObjHandyBase::Redraw()
{
	ASSERT(FALSE); // Not implemented here
}

void CObjHandyBase::DrawMarker(CDC &theDC, CPoint pt)
{
	ASSERT(FALSE); // Not implemented here
}

void CObjHandyBase::OnLButtonDown(CPoint)
{
}

void CObjHandyBase::OnLButtonUp(CPoint)
{
}

void CObjHandyBase::OnMouseMove(CPoint)
{
}

BOOL CObjHandyBase::OnBeginMove(CPoint pt, CPoint ptBmp, int MoveMode,
		double fZoom, CView *pView)
{
	return TRUE;
}

void CObjHandyBase::OnMove(CPoint ptStart, CPoint ptEnd, double fZoom)
{
};

BOOL CObjHandyBase::OnEndMove(CPoint pt, CPoint ptBmpStart, double fZoom,
		CRect &rcInv1, CRect &rcInv2, CView *pView)
{
	return TRUE;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//CObjLength

CObjFreeLength::CObjFreeLength()
{
	m_bReady = FALSE;
	m_fStoredLen = 0;
	m_cntr.SetCycled( FALSE );
	//m_cntr
}

void CObjFreeLength::Attach( CObjNative &obj )
{
	CObjHandyBase::Attach(obj);
	if (obj.IsKindOf(RUNTIME_CLASS(CObjFreeLength)))
	{
		m_fStoredLen = ((CObjFreeLength &)obj).m_fStoredLen;
		m_bReady     = ((CObjFreeLength &)obj).m_bReady;
		m_cntr.Attach(((CObjFreeLength &)obj).m_cntr);
	}
}

void CObjFreeLength::AttachData( CObjNative &obj )
{
	CObjHandyBase::AttachData(obj);
	if (obj.IsKindOf(RUNTIME_CLASS(CObjFreeLength)))
	{
		m_fStoredLen = ((CObjFreeLength &)obj).m_fStoredLen;
		m_bReady     = ((CObjFreeLength &)obj).m_bReady;
		m_cntr.Attach(((CObjFreeLength &)obj).m_cntr);
	}
}

void CObjFreeLength::Serialize( CArchive &ar )
{
	m_cntr.Serialize( ar );

	if( ar.IsStoring() )
	{
		ar<<m_fStoredLen;
		ar<<m_bReady;
	}
	else
	{
		ar>>m_fStoredLen;
		ar>>m_bReady;
	}

	m_cntr.SetCycled( FALSE );
}

void CObjFreeLength::OnLButtonDown(CPoint pt )
			//Обработчики сообщений
{
	ASSERT(FALSE); // Not implemented here
}

void CObjFreeLength::OnMouseMove( CPoint pt ) 
{
	ASSERT(FALSE); // Not implemented here
}

void CObjFreeLength::OnLButtonDblClk(CPoint pt)
{
	ASSERT(FALSE); // Not implemented here
}

void CObjFreeLength::DoPaintInt(CDC &theDC, int iType )
{
	ASSERT(FALSE); // Not implemented here
}

void CObjFreeLength::DoMeasure()
{
	ASSERT(FALSE); // Not implemented here
}

double	CObjFreeLength::GetVal()
{
	return m_fStoredLen;
}


void CObjFreeLength::OnEndMeasure()
{
	ASSERT(FALSE); // Not implemented here
}

BOOL CObjFreeLength::IsPointInObject( CPoint pt, BOOL bActive )
{
	ASSERT(FALSE); // Not implemented here
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//Ниже содержатся методы класса CObjLength - объект измерения длинны

CObjLength::CObjLength():
	CObjHandyBase()
{
	m_iPointDefined = 0;
	m_ptStart = m_ptEnd = CPoint( -1, -1 );
}

void CObjLength::Attach( CObjNative &obj )
{
	CObjHandyBase::Attach(obj);
	if (obj.IsKindOf(RUNTIME_CLASS(CObjLength)))
	{
		m_iPointDefined = ((CObjLength &)obj).m_iPointDefined;
		m_ptStart     = ((CObjLength &)obj).m_ptStart;
		m_ptEnd     = ((CObjLength &)obj).m_ptEnd;
	}
}

void CObjLength::AttachData( CObjNative &obj )
{
	CObjHandyBase::AttachData(obj);
	if (obj.IsKindOf(RUNTIME_CLASS(CObjLength)))
	{
		m_iPointDefined = ((CObjLength &)obj).m_iPointDefined;
		m_ptStart     = ((CObjLength &)obj).m_ptStart;
		m_ptEnd     = ((CObjLength &)obj).m_ptEnd;
	}
}

void CObjLength::Serialize( CArchive &ar )
{
	CObjHandyBase::Serialize( ar );
	if( ar.IsStoring() ) 
		ar<<m_ptStart.x
		  <<m_ptStart.y
		  <<m_ptEnd.x
		  <<m_ptEnd.y
		  <<m_iPointDefined;
	else
		ar>>m_ptStart.x
		  >>m_ptStart.y
		  >>m_ptEnd.x
		  >>m_ptEnd.y
		  >>m_iPointDefined;
}

void CObjLength::DoMeasure()
{
	ASSERT(FALSE); // Not implemented here
}

double CObjLength::GetVal()
{
	double	dx, dy;

	dx = (m_ptStart.x-m_ptEnd.x);
	dy = (m_ptStart.y-m_ptEnd.y);

	return ::sqrt(dx*dx+dy*dy);
}


void CObjLength::OnLButtonDown(CPoint pt)
{
	ASSERT(FALSE); // Not implemented here
}

void CObjLength::OnMouseMove(CPoint pt)
{
	ASSERT(FALSE); // Not implemented here
}

void CObjLength::DoPaintInt(CDC &theDC, int iType )
{
	ASSERT(FALSE); // Not implemented here
}

double CObjLength::GetDX()
{
	return ::fabs((double)(m_ptStart.x-m_ptEnd.x));
}

double CObjLength::GetDY()
{
	return ::fabs((double)(m_ptStart.y-m_ptEnd.y));
}

//
void CObjLength::OnEndMeasure()
{
	ASSERT(FALSE); // Not implemented here
	CObjHandyBase::OnEndMeasure();
}

BOOL CObjLength::IsPointInObject( CPoint pt, BOOL bActive )
{
	ASSERT(FALSE); // Not implemented here
	return FALSE;
}


void CObjXYRatio::Attach( CObjNative &obj )
{
	CObjLength::Attach(obj);
}

void CObjXYRatio::AttachData( CObjNative &obj )
{
	CObjLength::AttachData(obj);
}

///////////////////////////////////////////////////////////////////////////////
//CObjXYRatio method - internal class for special case calibration
void CObjXYRatio::DoMeasure() 
{
//	double	fVal = GetVal();
//	SetValueToDocument( (CImgDoc*)pDoc, IDS_LENGTH, fVal );
}


#define FAR_AWAY	10000
void CObjXYRatio::DoPaintInt(CDC &theDC, int iType )
{
	ASSERT(FALSE); // Not implemented here
}

void CObjXYRatio::OnMouseMove(CPoint pt)
{
	ASSERT(FALSE); // Not implemented here
}


double	CObjXYRatio::GetVal()
{
	if( m_iPointDefined < 1 )
		return -1;
	double	dx, dy;

	dx = ::fabs((double)(m_ptStart.x-m_ptEnd.x));
	dy = ::fabs((double)(m_ptStart.y-m_ptEnd.y));

	return (dy>0&&dx>0)?(dy/dx):(-1);
}


///////////////////////////////////////////////////////////////////////////////
//Ниже содержатся методы класса CObjAngle - объект измерения длинны

CObjAngle::CObjAngle():
	CObjHandyBase()
{
	m_iPointDefined = 0;
}

void CObjAngle::Attach( CObjNative &obj )
{
	CObjHandyBase::Attach(obj);
	if (obj.IsKindOf(RUNTIME_CLASS(CObjAngle)))
	{
		m_iPointDefined = ((CObjAngle &)obj).m_iPointDefined;
		m_pt1 = ((CObjAngle &)obj).m_pt1;
		m_pt2 = ((CObjAngle &)obj).m_pt2;
		m_pt3 = ((CObjAngle &)obj).m_pt3;
	}
}

void CObjAngle::AttachData( CObjNative &obj )
{
	CObjHandyBase::AttachData(obj);
	if (obj.IsKindOf(RUNTIME_CLASS(CObjAngle)))
	{
		m_iPointDefined = ((CObjAngle &)obj).m_iPointDefined;
		m_pt1 = ((CObjAngle &)obj).m_pt1;
		m_pt2 = ((CObjAngle &)obj).m_pt2;
		m_pt3 = ((CObjAngle &)obj).m_pt3;
	}
}

void CObjAngle::Serialize( CArchive &ar )
{
	CObjHandyBase::Serialize( ar );
	if( ar.IsStoring() ) 
		ar<<m_pt1.x
		  <<m_pt1.y
		  <<m_pt2.x
		  <<m_pt2.y
		  <<m_pt3.x
		  <<m_pt3.y
		  <<m_iPointDefined;
	else
		ar>>m_pt1.x
		  >>m_pt1.y
		  >>m_pt2.x
		  >>m_pt2.y
		  >>m_pt3.x
		  >>m_pt3.y
		  >>m_iPointDefined;
}

double CObjAngle::Calc(double &dA1, double &dA2)
{
	return 0.;
}


void CObjAngle::DoMeasure()
{
	ASSERT(FALSE); // Not implemented here
}


double CObjAngle::GetVal()
{
	return 0.;
}



void CObjAngle::OnLButtonDown(CPoint pt)
{
	ASSERT(FALSE); // Not implemented here
}

void CObjAngle::OnMouseMove(CPoint pt)
{
	ASSERT(FALSE); // Not implemented here
}

void CObjAngle::DoPaintInt(CDC &theDC, int iType )
{
	ASSERT(FALSE); // Not implemented here
}

BOOL CObjAngle::IsPointInObject( CPoint pt, BOOL bActive )
{
	ASSERT(FALSE); // Not implemented here
	return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//Ниже содержатся методы класса CObjAngle - объект измерения длинны

void CObjRadius::Attach( CObjNative &obj )
{
	CObjAngle::Attach(obj);
}

void CObjRadius::AttachData( CObjNative &obj )
{
	CObjAngle::AttachData(obj);
}


void CObjRadius::DoMeasure()
{
	ASSERT(FALSE); // Not implemented here
}

double CObjRadius::GetVal()
{
	ASSERT(FALSE); // Not implemented here
	return 0.;
}


BOOL CObjRadius::Calc(CPoint &ptCenter, double &r)
{
	ASSERT(FALSE); // Not implemented here
	return TRUE;
}

void CObjRadius::DoPaintInt(CDC &theDC, int iType )
{
	ASSERT(FALSE); // Not implemented here
}

BOOL CObjRadius::IsPointInObject( CPoint pt, BOOL bActive )
{
	ASSERT(FALSE); // Not implemented here
	return FALSE;
}

