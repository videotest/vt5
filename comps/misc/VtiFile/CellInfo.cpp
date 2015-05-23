#include "StdAfx.h"
#include "CellInfo.h"
//#include "GalView.h"
#include "ObjMease.h"
#include "GalHlp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const int iNewObjAreaSize = 20;

IMPLEMENT_SERIAL(CCellInfo, CObject, 1);
IMPLEMENT_SERIAL(CGalParams, CObArray, 1);

void CMyRectTracker::OnChangedRect( const CRect& rectOld )
{
	CRect	rc = m_rect;
	int	iGrid  = 5;

	if( m_iZoom > 0 )
		iGrid *= m_iZoom;
	
	m_rect.left = (m_rect.left+iGrid/2)/iGrid*iGrid;
	m_rect.top = (m_rect.top+iGrid/2)/iGrid*iGrid;
	m_rect.right = (m_rect.right+iGrid/2)/iGrid*iGrid;
	m_rect.bottom = (m_rect.bottom+iGrid/2)/iGrid*iGrid;
}


/////////////////////////////////////////////////////////////////////
//information about each cell - class CCellInfo

CCellInfo::CCellInfo()
{
	m_iClassNo = -1;
	m_rect = NORECT;
	m_pparams = 0;
	m_ptScroll = CPoint( 0, 0 );
}

CCellInfo::~CCellInfo()
{
}


void CCellInfo::Serialize( CArchive &ar )
{
	m_arrObNums.Serialize( ar );

	if( ar.IsStoring() )
	{
		ar<<m_rect.left;
		ar<<m_rect.top;
		ar<<m_rect.right;
		ar<<m_rect.bottom;
		ar<<m_iClassNo;
	}
	else
	{
		ar>>m_rect.left;
		ar>>m_rect.top;
		ar>>m_rect.right;
		ar>>m_rect.bottom;
		ar>>m_iClassNo;
	}
}

void CCellInfo::Draw( CDC &theDC, double fZoom, CPoint ptScroll, UINT uFlags )
{
	ASSERT(FALSE); // Not implemented here !!!
}

int CCellInfo::GetFreePosition()
{
	for( int i = 0;;i++ )
		if( !IsObjectInPos( i ) )
			return i;
}

void CCellInfo::EmptyCell()
{
	m_arrObNums.RemoveAll();
}

CCellInfo &CCellInfo::SetClass( int n )
{
	m_iClassNo = n;
	return *this;
}

CCellInfo &CCellInfo::SetRect( CRect rc )
{
	m_rect = rc;
	return *this;
}

//vizible area properties
BOOL CCellInfo::IsObjVisible( int iObjPos )
{
	ASSERT(FALSE); // Not implemented here !!!
	return TRUE;
}

int CCellInfo::GetObjPos( __int64 iObjNo )
{
	ASSERT(FALSE); // Not implemented here !!!
	return -1;
}

CRect CCellInfo::GetObjRect( int iObjPos )
{
	ASSERT(FALSE); // Not implemented here !!!
	return NORECT;
}

CSize CCellInfo::GetMaxSize()
{
	ASSERT(FALSE); // Not implemented here !!!
	return CSize(0,0);
}

int CCellInfo::AddObject( __int64 iObjNo, int iPos )
{
	if( iPos == -1 )
		iPos = GetFreePosition();
	for( int i = m_arrObNums.GetSize(); i<= iPos; i++ )
		m_arrObNums.Add( 0 );

	m_arrObNums[iPos] = SHORT_CODE( iObjNo )+1;

	return iPos;
}

void CCellInfo::RemoveObject( int iPos )
{
	ASSERT(FALSE); // Not implemented here !!!
}

int CCellInfo::GetObjNoByPoint( CPoint pt )
{
	if( !m_rect.PtInRect( pt ) )
		return -1;

	CPoint	ptScroll = GetScrollPoint();
	int	iPos = (pt.x+ptScroll.x-m_rect.left)/m_pparams->GetObjSize().cx;

	if( iPos < 0 || iPos > m_arrObNums.GetSize()-1 )
		return -1;
	
	return m_arrObNums[iPos]-1;
}

int CCellInfo::GetPositionByPoint( CPoint pt )
{
	CPoint	ptScroll = GetScrollPoint();

	if( !m_rect.PtInRect( pt ) )
		return -1;
	int	iPos = (pt.x-m_rect.left+ptScroll.x)/m_pparams->GetObjSize().cx;

	return iPos;
}

BOOL CCellInfo::IsObjectInPos( int iPos )
{
	if( iPos < 0 )
		return -1;
	if( iPos > m_arrObNums.GetSize()-1 )
		return 0;
	return m_arrObNums[iPos] != 0;
}

//some for scrolling inside cell
BOOL CCellInfo::NeedScrollBar( UINT nBar )
{
	ASSERT(FALSE); // Not implemented here !!!
	return FALSE;
}

void CCellInfo::SetScrollPos( UINT nBar, int iPos )
{
	ASSERT(FALSE); // Not implemented here !!!
}

int CCellInfo::GetScrollSize( UINT nBar )
{
	ASSERT(FALSE); // Not implemented here !!!
	return 0;
}

int CCellInfo::GetScrollPos( UINT nBar )
{
	ASSERT(FALSE); // Not implemented here !!!
	return 0;

}

BOOL CCellInfo::EnsureVisible( __int64 iObjNo )
{
	ASSERT(FALSE); // Not implemented here !!!
	return TRUE;
}

BOOL CCellInfo::IsShowFrame() {return 1;}

IObject *CCellInfo::GetObj(__int64 iObjNo)
{
	ASSERT(FALSE); // Not implemented here !!!
	return NULL;
}

int CCellInfo::GetObjAlign() {return 0;}
int CCellInfo::GetNameAlign() {return 0;}
CSize CCellInfo::GetObjSize() {return m_pparams->GetObjSize();}

CString CCellInfo::GetClassName()
{
	ASSERT(FALSE); // Not implemented here !!!
	return _T("Unknown");
}

void CCellInfo::DrawObjIcon(int iObjNo, CDC *pDC, RECT rcDib, RECT rcPaint, BOOL bForceStretch)
{
	ASSERT(FALSE); // Not implemented here !!!
}


//////// /////////////////////////////////////////////////////////////
//cell's array - grid data storage

CGalParams::CGalParams()
{
	m_sizeObj = CSize( 0, 0 );
	m_bLockedUpdate = FALSE;
	m_bWriteToRegistry = TRUE;
	m_font.SetSize( 10 ).SetFaceName( "Arial Cyr" );
//	UpdateObjSize();
}

CGalParams::~CGalParams()
{
	DeInit();
}
	//create default 
void CGalParams::Init()
{
	DeInit();
}

void CGalParams::DeInit()
{
	for( int i = 0; i<GetSize(); i++ )
	{
		CCellInfo	*pinfo = GetAt( i );
		delete pinfo;
	}
	RemoveAll();
	m_iActiveCell = -1;
}
	//configuration interface
void CGalParams::ReadState()
{
	ASSERT(FALSE); // Not implemented here !!!
}

int CGalParams::Add( CCellInfo *pinfo )
{
	ASSERT(FALSE); // Not implemented here !!!
	return m_iActiveCell;
}

void CGalParams::SaveState()
{
	ASSERT(FALSE); // Not implemented here !!!
}

void CGalParams::Serialize( CArchive &ar )
{
	if (!ar.IsStoring())
		DeInit();
	CObArray::Serialize( ar );
	m_font.Serialize( ar );
	if( ar.IsStoring() )
	{
		ar<<m_sizeObj.cx;
		ar<<m_sizeObj.cy;
	}
	else
	{
		ar>>m_sizeObj.cx;
		ar>>m_sizeObj.cy;
	}
	if( !ar.IsStoring() )
	{
		m_bWriteToRegistry = FALSE;
		for( int i = 0; i<GetSize(); i++ )
			GetAt( i )->AttachParams( this );
//		UpdateObjSize();
	}
}

void CGalParams::Draw( CDC &theDC, double fZoom, CPoint ptScroll, UINT uFlags )
{
	ASSERT(FALSE); // Not implemented here !!!
}

CSize CGalParams::GetDrawSize()
{
	ASSERT(FALSE); // Not implemented here !!!
	return CSize(0,0);
}

int CGalParams::GetCellInfoByPoint( CPoint pt )
{
	ASSERT(FALSE); // Not implemented here !!!
	return -1;
}

int CGalParams::GetCellInfoByObjNo( __int64 iObjNo )
{
	ASSERT(FALSE); // Not implemented here !!!
	return -1;
}

int CGalParams::GetCellInfoByClass( int iClass )
{
	ASSERT(FALSE); // Not implemented here !!!
	return -1;
}

__int64 CGalParams::GetObjNoByPoint( CPoint pt )
{
	ASSERT(FALSE); // Not implemented here !!!
	return -1;
}
void CGalParams::DeleteCell()
{
	ASSERT(FALSE); // Not implemented here !!!
}

void CGalParams::InsertCell()
{
	ASSERT(FALSE); // Not implemented here !!!
}

CCellInfo *CGalParams::GetActiveCell()
{
	if( m_iActiveCell == -1 )return 0;
	ASSERT( m_iActiveCell < GetSize() );
	return GetAt( m_iActiveCell );
}

void CGalParams::SetClass( int iPos, int iClass )
{

}

#define DIVIDER	10
void CGalParams::AddCells( int iCount, int iCols )
{
	ASSERT(FALSE); // Not implemented here !!!
}

void CGalParams::ArrageCells()
{
	ASSERT(FALSE); // Not implemented here !!!
}

void CGalParams::ZoomCells( double fZoom )
{
	ASSERT(FALSE); // Not implemented here !!!
}

void CGalParams::SetActiveCell( int idx )
{
	ASSERT(FALSE); // Not implemented here !!!
}


void CGalParams::UpdateAllViews( CGalChangeInfo *p )
{
	ASSERT(FALSE); // Not implemented here !!!
}


void CGalParams::PlaceObjects()
{
	ASSERT(FALSE); // Not implemented here !!!
}

void CGalParams::UpdatePosition()
{
	ASSERT(FALSE); // Not implemented here !!!
}

void CGalParams::DeleteContents()
{
	for( int i = 0; i<GetSize(); i++ )
	{
		CCellInfo	*pinfo = GetAt( i );
		pinfo->EmptyCell();
	}
	UpdateObjSize();
//	UpdateAllViews( &CGalChangeInfo() );
}

LOGFONT *CGalParams::GetFont()
{
	return &m_font.GetLogFont();
}

void CGalParams::UpdateObjSize()
{
	ASSERT(FALSE); // Not implemented here !!!
}
