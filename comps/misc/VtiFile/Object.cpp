#include "StdAfx.h"
#include "Object.h"
//#include "Aphine.h"
#include "ObjMease.h"
//#include "CnvWork.h"
#include "Vt4Params.h"
//#include "ProgLog.h"
#include "RectHand.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_SERIAL(CObjMove, CObject, VERSIONABLE_SCHEMA|3);

extern BOOL	g_bRotateIcons;
//IMPLEMENT_DYNAMIC(CContourArray, CObArray, 1);

#pragma warning(disable:4244)
#pragma warning(disable:4270)

//some add inline
inline int GetDirect(CshortPoint pt)
{
	for(int i = 0; i< 8; i++)
		if(pt==ptDirect[i])
			return i;
	return 8;
}

inline void SetToImage(C8Image &img,  CPoint pt, BYTE b )
{
	if(pt.x < 0 || pt.y < 0 || pt.x > img.GetCX()-1 || pt.y > img.GetCY()-1)
		return;
	img[pt.y][pt.x] = b;
}

inline BYTE GetFromImage(C8Image &img,  CPoint pt)
{
	if(pt.x < 0 || pt.y < 0 || pt.x > img.GetCX()-1 || pt.y > img.GetCY()-1)
		return 0;
	return img[pt.y][pt.x];
}


IMPLEMENT_DYNCREATE(CContour,CObject);

#ifdef __VT4__
CCntrNative *CreateNewContour()
{
	return new CContour;
}
#endif //__VT4__

//CContour class members
CContour::CContour()
	:CCntrNative()
{
	m_iMinSize= 0;
	m_bNotRemove = FALSE;
	m_bFork = FALSE;
	m_nSupLen = 0;
	m_bCycled = true;

	SetBody( BYTE_FILLEDBODY );
	SetFill( BYTE_RAWBODY );
	SetBack( BYTE_FREESPACE );
	SetHole( BYTE_INTRNSPACE );
	SetCont( BYTE_CONTOUR );
}

//функция Realloc(). Для начального размещения и переразмещения памяти.
void CContour::Realloc()
{
	if(m_iSize != m_iMaxSize)return;
	
	CshortPoint	*p = m_ptArray;

	m_iMaxSize+=1024;

	try{
		m_ptArray = (CshortPoint *)AllocMem(m_iMaxSize*sizeof(CshortPoint));
	}
	catch( ... )
	{
		LowMem();
		m_iMaxSize = m_iSize;
		m_ptArray = p;
		return;
	}

	if(p)
	{
		memcpy(m_ptArray, p, m_iSize*sizeof(CshortPoint));
		FreeMem(p);
	}
}

void CContour::Walk( BITMAPINFOHEADER *pbi, CPoint ptStart, int idir )
{
	C8Image	*pimg = (C8Image*)CImageBase::MakeFromBits( pbi, FALSE );

	Walk( *pimg, ptStart, idir );

	delete pimg;
}

void CContour::FillFast( BITMAPINFOHEADER *pbi, BYTE byteExtMask, BYTE byteIntMask, CContourArray &array )
{
	C8Image	*pimg = (C8Image*)CImageBase::MakeFromBits( pbi, FALSE );

	FillFast( *pimg, byteExtMask, byteIntMask, array );

	delete pimg;
}




//функция AddSegment(CPoint ptEnd) предназначена для добавления сегмента к линии
void CContour::AddSegment(CPoint pt)
{
	if(GetSize()==0)
	{
		Add(pt);
		return;
	}

	
	CPoint	ptStart = m_ptArray[m_iSize-1];
	CPoint	ptEnd = pt;

	int		incX, incY, iDistX, iDistY, i, iDirect = 0;

	incX = ptEnd.x>ptStart.x?1:-1;
	incY = ptEnd.y>ptStart.y?1:-1;
	iDistX = ::abs(ptEnd.x-ptStart.x);
	iDistY = ::abs(ptEnd.y-ptStart.y);

	if(iDistX>iDistY)
	{
		iDirect = iDistX;
		for(i = 0; i<iDistX; i++)
		{
			ptStart.x+=incX;
			iDirect-=2*iDistY;
			if(iDirect<0)
			{
				iDirect+=2*iDistX;
				ptStart.y+=incY;
			}
			Add(ptStart);
		}
	}
	else
	{
		iDirect = iDistY;
		for(i = 0; i<iDistY; i++)
		{
			ptStart.y+=incY;
			iDirect-=2*iDistX;
			if(iDirect<0)
			{
				iDirect+=2*iDistY;
				ptStart.x+=incX;
			}
			Add(ptStart);
		}
	}
}


//New! Set markers at external side of contour, what make possible 
//faster fill algorithm
void CContour::SetExternalMarks( C8Image &img, BYTE byteMarks, BYTE *p1stCol )
{
			//verify image
	ASSERT( !img.IsEmpty() );	
	ASSERT( !img.IsTrueColor() );
			//verify contour
	ASSERT( m_iSize > 1 );
	ASSERT( m_ptArray );
			//determine first direct
	
	int iDir1 = GetDirect( m_ptArray[0]-m_ptArray[m_iSize-2] ), iDir2;

	for( DWORD i = 0; i< unsigned(m_iSize) - 1; i++ )
	{
			//new direct's
		iDir2 = (iDir1 > 3)? (iDir1-4):(iDir1+4);
		iDir1 = GetDirect( m_ptArray[i+1] - m_ptArray[i] );

		SetToImage( img, m_ptArray[i], m_byteCont );

		while( TRUE )
		{
			//firstly, find new point
			iDir2=(iDir2 == 7)?0:iDir2+1;
			//if it is last point
			if( iDir1 == iDir2 )
				break;
			//else set mark
			CPoint	pt = m_ptArray[i]+ptDirect[iDir2];
			if( pt.x == -1 && pt.y > -1 && pt.y < img.GetCY() )
			{
				ASSERT( p1stCol );

				p1stCol[pt.y] = byteMarks;
			}
			else
				SetToImage( img, pt, byteMarks );
		}
	}
}

CPoint *CContour::CreateArray( int iZoom, CPoint ptOfs )
{
	if( !m_iSize )
		return 0;

	double	fZoom = iZoom > 0?iZoom:-1./iZoom;

	CPoint *ptPaintArray = (CPoint *)AllocMem( sizeof( CPoint )*(m_iSize+2) );
	for(DWORD dw = 0; dw<unsigned(m_iSize); dw++)
		ptPaintArray[dw] = CPoint( 
		int( ( m_ptArray[dw].x+ptOfs.x+0.5 )*fZoom ), 
		int( ( m_ptArray[dw].y+ptOfs.y+0.5 )*fZoom ) );
	return	ptPaintArray;
}

//New! faster filling procedure.
//function SetExternalMarks must be called before filling
//rectangle must be set 
//used m_byteBody, m_byteHole for filling,
//m_byteFill and m_byteBack for back for filling

//filling array structure by internal contour values
void CContour::FillFast( C8Image &img, BYTE byteExtMask, BYTE byteIntMask, CContourArray &array, C8Image *pimgMeas )
{
	int		x, y;
	BYTE	bCur, bPrev;
			//inside and inside hole flag
	BOOL	bInside = FALSE;
	BOOL	bInsideHole = FALSE;
	int		iSX = max( 0, m_rc.left-1 );
	int		iSY = max( 0, m_rc.top-1 );
	int		iCX = min( img.GetCX(), m_rc.right+1 );
	int		iCY = min( img.GetCY(), m_rc.bottom+1 );

	int	iDeltaX = (m_rc.left == 0) ? 0 : 1;
	int	iDeltaY = (m_rc.top == 0) ? 0 : 1;

	BYTE	*p1st = (BYTE *)::AllocMem( img.GetCY() );
	SetExternalMarks( img, byteExtMask, p1st );


	for( y = iSY; y< iCY; y++ )
	{
		if( m_rc.left == 0 )
			bPrev = p1st[y];
		else
			bPrev = img[y][m_rc.left-1];
		bInside = FALSE;

		for( x = iSX; x< iCX; x++ )
		{
			bCur = img[y][x];

			//test for internal hole

			if( bInside && !bInsideHole	//inside, but not in hole
				&&(bPrev == m_byteFill ||bPrev == m_byteCont )	//prev point - body or contour
				&& bCur == m_byteBack )		//current point - background
			{
				CContour	*p = new CContour;
				
				
				p->Walk( img,  CPoint( x-1, y ), 2 );

			//determine small hole and ignore it
				if( p->GetSize() == 0 )
				{
					delete p;
				}
				else
				{
					p->SetCont( m_byteCont );
					p->SetExternalMarks( img, byteIntMask );

					if( p->GetRect().Width() > m_iMinSize 
						||p->GetRect().Height() > m_iMinSize )
						array.Add( p );
					else
						delete p;
					bCur = byteIntMask;
					bPrev = m_byteCont;
				}
			}

			if( bPrev == byteExtMask && bCur == m_byteCont )
			{
				ASSERT( !bInsideHole );
				ASSERT( !bInside );
				bInside = TRUE;
			}

			if( bPrev == m_byteCont && bCur == byteIntMask )
			{
				ASSERT( bInside );
				ASSERT( !bInsideHole );
				bInsideHole = TRUE;
			}

			if( bPrev == byteIntMask && bCur == m_byteCont )
			{
				ASSERT( bInside );
				ASSERT( bInsideHole );
				bInsideHole = FALSE;
			}

			if( bPrev == m_byteCont && bCur == byteExtMask )
			{
				ASSERT( bInside );
				ASSERT( !bInsideHole );
				bInside = FALSE;
			}

			ASSERT( !( bPrev == byteExtMask && bCur == byteIntMask ) );
			ASSERT( !( bPrev == byteIntMask && bCur == byteExtMask ) );

			//all OK!
			if( bInside && bInsideHole && bCur != m_byteFill )
				img[y][x] = m_byteHole;
			if( bInside && !bInsideHole )
				img[y][x] = m_byteBody;
			if( bCur == byteExtMask )
			{
				ASSERT( !bInside );
				img[y][x] = m_byteBack;
			}

			if( pimgMeas && y-iDeltaY >= iSY && x-iDeltaX >= iSX )
			{
				if( bInside && !bInsideHole )
					(*pimgMeas)[y-iSY-iDeltaY][x-iSX-iDeltaX] = 0xFF;
				if( bInside && bInsideHole )
					(*pimgMeas)[y-iSY-iDeltaY][x-iSX-iDeltaX] = 0xFE;
			}

			bPrev = bCur;
		}
	}
	
	for( int i = 0; i<array.GetSize(); i++ )
		((CContour *)array[i])->Map( img, m_byteBody );

	FreeMem( p1st );
}
//New! Fill only external contour
//use m_byteFill and m_byteBody values
void CContour::FillFast( C8Image &img, BYTE byteExtMask )
{
	int		x, y;
	BYTE	bCur, bPrev;
			//inside and inside hole flag
	BOOL	bInside = FALSE;
	int		iSX = max( 0, m_rc.left-1 );
	int		iSY = max( 0, m_rc.top-1 );
	int		iCX = min( img.GetCX(), m_rc.right+1 );
	int		iCY = min( img.GetCY(), m_rc.bottom+1 );
	BYTE	*p1st = (BYTE *)::AllocMem( img.GetCY() );
	SetExternalMarks( img, byteExtMask, p1st );


	for( y = iSY; y< iCY; y++ )
	{
		if( m_rc.left == 0 )
			bPrev = p1st[y];
		else
			bPrev = img[y][m_rc.left-1];
		bInside = FALSE;

		for( x = iSX; x< iCX; x++ )
		{
			bCur = img[y][x];

			//test for internal hole
			if( bPrev == byteExtMask && bCur == m_byteCont )
			{
//				ASSERT( !bInside );
				bInside = TRUE;
			}

			if( bPrev == m_byteCont && bCur == byteExtMask )
			{
//				ASSERT( bInside );
				bInside = FALSE;
			}

			//all OK!
			if( bInside )
				img[y][x] = m_byteBody;
			if( bCur == byteExtMask )
			{
//				ASSERT( !bInside );
				img[y][x] = m_byteBack;
			}

			bPrev = bCur;
		}
	}
	
	Map( img, m_byteBody );

	FreeMem( p1st );
}


//Функция Map. Нарисовать контур на изображении
void CContour::Map( C8Image &img, BYTE byte )
{
	ASSERT(img.IsGrayScale()&&!img.Is16BitColor());
	DWORD dw;
	for(dw = 0; dw<unsigned(m_iSize); dw++)
	{
		int x = m_ptArray[dw].x;
		int y = m_ptArray[dw].y;
		if (y == 0)
			x = m_ptArray[dw].x;
		img[m_ptArray[dw].y][m_ptArray[dw].x] = byte;
	}
}

//Функция Walk. обойти контур
void CContour::Walk(C8Image &img, CPoint ptStart, int iStartDir)
{
	ASSERT(img.IsGrayScale()&&!img.Is16BitColor());
	int		i;

	CshortPoint pt=ptStart;
	DeInit();

	ASSERT(::GetFromImage(img, pt));

				//Это не одиночная точка ?
	for(i = 0; i< 8; i++)
	{
		BYTE	b = ::GetFromImage(img, pt+ptDirect[i]);
		if( b )
			break;
	}
	if(i == 8)	//Это одиночная точка
		return;
				//Стартовое направление
	for(i = 0; i<8; i++)
	{
		if(!::GetFromImage(img, pt+ptDirect[i]))
			break;
	}
				//i - стартовое направление
	if(iStartDir != -1)
	{
		if(::GetFromImage(img, pt+ptDirect[iStartDir]))
			return;
		i = iStartDir;
	}
	else iStartDir = i;
				//находим первую точку
	Add(pt);

	while(1)
	{
		for(;;i=i==7?0:i+1)
		{
			if(::GetFromImage(img, pt+ptDirect[i]))
				break;
		}
		pt+=ptDirect[i];
		Add(pt);
		i=i>2?(i-3):(i+5);
		//i=i>1?(i-2):(i+6);
		if(pt == ptStart)break;

#ifdef _DEBUG
		ASSERT(m_iSize < 100000);
		if(m_iSize > 100000)break;
//		if(m_iSize > 100000)break;
#endif	//_DEBUG	
	}
//Когда через стартовую точку проходит несколько раз

	for(;;i=i==7?0:i+1)
	{
		if(::GetFromImage(img, pt+ptDirect[i]))
			break;
	}
	pt+=ptDirect[i];
	if(pt != m_ptArray[1])
	{
#ifndef _DEBUG
		DWORD	dw= m_iSize;
#endif //_DEBUG
		Add(pt);
		i=i>2?(i-3):(i+5);
		//i=i>1?(i-2):(i+6);
		while(1)
		{
			for(;;i=i==7?0:i+1)
			{
				if(::GetFromImage(img, pt+ptDirect[i]))
					break;
			}
			pt+=ptDirect[i];
			Add(pt);
			i=i>2?(i-3):(i+5);
			//i=i>1?(i-2):(i+6);
			if(pt == ptStart)break;
#ifdef _DEBUG
			ASSERT(m_iSize < 100000);
			if(m_iSize > 100000)break;
#endif	//_DEBUG

		}

	}
}

//Отрисовка на Paint
void CContour::DoPaint1st(CDC &theDC, double fZoom)
{
//	double	fZoom = iZoom > 0 ? iZoom : -1.0/iZoom;

	if(m_iSize == 0)return;
	CPoint *ptPaintArray = (CPoint *)AllocMem(sizeof(CPoint)*m_iSize);
	for(DWORD dw = 0; dw<unsigned(m_iSize); dw++)
		ptPaintArray[dw] = CPoint(int((m_ptArray[dw].x+0.5)*fZoom), int((m_ptArray[dw].y+0.5)*fZoom));

	CPen	*pOldPen, 
		*pPen = new CPen(PS_DOT, 1, RGB(0, 0, 0));

	int iRopOld = theDC.SetROP2( R2_COPYPEN );
	int nBkMode = theDC.SetBkMode(OPAQUE);
	COLORREF clrBk = theDC.SetBkColor(RGB(255,255,255));

	pOldPen = theDC.SelectObject(pPen);
	theDC.Polyline(ptPaintArray, m_iSize);
#if 0
	for (int i = 0; i < m_nSingulPtsNum; i++)
	{
		ASSERT(FALSE);
		int n = m_pSingulPts[i];
		CPoint pt(ptPaintArray[n]);
		theDC.Ellipse(pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
	}
#endif

	theDC.SetBkMode(nBkMode);
	theDC.SetBkColor(clrBk);
	theDC.SelectObject(pOldPen);
	theDC.SetROP2( iRopOld );

	delete 	  pPen;

	FreeMem(ptPaintArray);
}

//Отрисовка на WM_TIMER	
void CContour::DoPaintNext(CDC &theDC, double fZoom)
{
//	double	fZoom = iZoom > 0 ? iZoom : -1.0/iZoom;

	if(m_iSize == 0)return;

	try
	{
	CPoint *ptPaintArray = (CPoint *)AllocMem(sizeof(CPoint)*m_iSize);
	for(DWORD dw = 0; dw<unsigned(m_iSize); dw++)
		ptPaintArray[dw] = CPoint(int((m_ptArray[dw].x+0.5)*fZoom), int((m_ptArray[dw].y+0.5)*fZoom));

	CPen	*pOldPen, 
		*pPen = new CPen(PS_SOLID, 1, RGB(255, 255, 255));

	int	iROP2 = theDC.SetROP2(R2_XORPEN);

	pOldPen = theDC.SelectObject(pPen);
	theDC.Polyline(ptPaintArray, m_iSize);
#if 0
	for (int i = 0; i < m_nSingulPtsNum; i++)
	{
		int n = m_pSingulPts[i];
		CPoint pt(ptPaintArray[n]);
		theDC.Ellipse(pt.x - 2, pt.y - 2, pt.x + 2, pt.y + 2);
	}
#endif

	theDC.SelectObject(pOldPen);
	theDC.SetROP2(iROP2);

	delete 	  pPen;
	FreeMem(ptPaintArray);
	}
	catch( ... )
	{
	}
}

void CContour::Serialize( CArchive &ar )
{
	if( ar.IsStoring() )
	{
		int	is = GetSize();
		ar << is;
		ar.Write( GetPtArray(), sizeof( CshortPoint )*is );
		ar << m_rc.left;
		ar << m_rc.top;
		ar << m_rc.right;
		ar << m_rc.bottom;
	}
	else
	{
		int	is;
		void *pp;
		ar >> is;
		pp = AllocMem( sizeof( CshortPoint )*is );
		ar.Read( pp, sizeof( CshortPoint )*is );
		Attach( (POINTS *)pp, is );
		ar >> m_rc.left;
		ar >> m_rc.top;
		ar >> m_rc.right;
		ar >> m_rc.bottom;
#if defined(_DEBUG)
		for (int j = 0; j < is; j++)
		{
			ASSERT(((CshortPoint*)pp)[j].x >= m_rc.left &&
				((CshortPoint*)pp)[j].x <= m_rc.right);
			ASSERT(((CshortPoint*)pp)[j].y >= m_rc.top &&
				((CshortPoint*)pp)[j].y <= m_rc.bottom);
		}
#endif
	}
}

DWORD CContour::CalcBytesSize() const
{
	DWORD dwSzBase = CCntrNative::CalcBytesSize();
	return dwSzBase+sizeof(CContour)-sizeof(CCntrNative);
}


void CDragState::operator=(CObjMove &s)
{
	safedelete(m_pimgToMove);
	safedelete(m_pimgToDraw);
#if defined(__VT4__) || defined(__PP__)
	safedelete(m_pimgBinToMove);
	safedelete(m_pimgBinToDraw);
#endif
	m_pimgToMove = s.m_pimgToMove;
	m_pimgToDraw = s.m_pimgToDraw;
#if defined(__VT4__) || defined(__PP__)
	m_pimgBinToMove = s.m_pimgBinToMove;
	m_pimgBinToDraw = s.m_pimgBinToDraw;
#endif
	s.m_pimgToMove = NULL;
	s.m_pimgToDraw = NULL;
#if defined(__VT4__) || defined(__PP__)
	s.m_pimgBinToMove = NULL;
	s.m_pimgBinToDraw = NULL;
#endif
}


///////////////////////////////////////////////////////////
//
//here placed members of CObjMove class
////////////////////////////////////
int g_iObjCounter = 0;
static int  m_nZone = -1;


CObjMove::CObjMove() : CObjNative()
{
BEGIN_LOG_METHOD(CObjMove,2);
	m_bDirectUnknown = FALSE;
	m_plUsage = 0;
	m_iCode = g_iObjCounter++;
//	m_pimgBin = new C8Image;
	m_pimgIcon = new C8Image;
	m_pimgBin = NULL;
	m_pimg = 0;
	m_rc = NORECT;
	m_iClassNo = CLASS_AUTO;
	m_pimgToMove = 0;
	m_pimgToDraw = 0;
	m_pimgHostBin = 0;
	m_bDragging = FALSE;
	m_ptOffset = NOPOINT;
	m_bFrame = FALSE;
//	m_bUndo  = FALSE;
	m_bNotDivided = FALSE;
	m_pimgBinToMove = NULL;
	m_pimgBinToDraw = NULL;
	m_bNeedToSerializeIcon = FALSE;

	m_fInitAngle = 0.;
	m_fInitKx = 1.;
	m_fInitKy = 1.;
	m_ptInitCen = CPoint(0,0);
	m_fSavedAngle = 0.;
	m_fSavedKx = 1.;
	m_fSavedKy = 1.;
END_LOG_METHOD();
}

CObjMove::~CObjMove()
{
	DeInit();
	safedelete(m_pimgBin);
	safedelete(m_pimgIcon);
}

CObjNative *CObjMove::Clone()
{
	BEGIN_LOG_METHOD(Clone,2);
	CRuntimeClass *pClass = GetRuntimeClass();
	ASSERT(pClass);
	CObject *p = pClass->CreateObject();
	ASSERT_KINDOF(CObjMove,p);
	((CObjMove *)p)->Attach(*this);
	return (CObjNative *)p;
	/*
//	((CObjMove *)p)->Attach(*this);
	((CObjMove *)p)->DeInit();
	((CObjMove *)p)->AttachCntr( m_contours );
	((CObjMove *)p)->m_rc = m_rc;
	if (((CObjMove *)p)->m_pimgBin == NULL)
		((CObjMove *)p)->m_pimgBin = new C8Image;
	((CObjMove *)p)->m_pimgBin->Attach( m_pimgBin );
#if defined(__VT4__)
	((CObjMove *)p)->m_pimgHostBin = m_pimgHostBin;
#endif
	((CObjMove *)p)->m_pimg = m_pimg;
	((CObjMove *)p)->m_iCode = m_iCode;
	((CObjMove *)p)->m_RectHandle = m_RectHandle;
	((CObjMove *)p)->m_bFrame = m_bFrame;
	((CObjMove *)p)->AttachMoveImage(*this);
	
	return (CObjNative *)p;
	*/
	END_LOG_METHOD();

	return 0;
}

#pragma optimize( "", off )

void CObjMove::DoFree()
{
//	CObjNative::DoFree();
BEGIN_LOG_METHOD(DoFree,2);

	CCntrNative *pp = m_contours.GetSize()?m_contours.GetAt( 0 ):NULL;
	for( int i = 0; i < m_contours.GetSize(); i++ )
	{
		CCntrNative *p = m_contours.GetAt( i );
		delete p;
	}
	if( m_contours.GetSize() )
		m_contours.SetSize(0);
	for( i = 0; i < m_OrigConts.GetSize(); i++ )
	{
		CCntrNative *p = m_OrigConts.GetAt( i );
		delete p;
	}
	if( m_OrigConts.GetSize() )
		m_OrigConts.RemoveAll();
	safedelete( m_pimgToMove );
	safedelete( m_pimgToDraw );
#if defined(__VT4__) || defined(__PP__)
	safedelete( m_pimgBinToMove );
	safedelete( m_pimgBinToDraw );
#endif
	safedelete( m_pimgBin );
//	safedelete( m_pimgIcon );
	m_bDragging = FALSE;
	m_ptOffset = NOPOINT;
END_LOG_METHOD();
}

void CObjMove::DeInit()
{
	//firstly call base class
//	CObjNative::DeInit();
BEGIN_LOG_METHOD(DeInit,2);

	BOOL	bNeedToDelete = TRUE;
	if( m_plUsage )
	{
		ASSERT( *m_plUsage > 0 );
			//in all cases,usage counter greater that zero
		(*m_plUsage)--;
		if( *m_plUsage > 0 )
			bNeedToDelete = FALSE;
		else	//only one object
		{
			delete m_plUsage;
		}
			//detach from object's list
		m_plUsage =  0;
	}

	if( bNeedToDelete )
		DoFree();
	m_rc = NORECT;
	m_iCode = g_iObjCounter++;
	if( m_contours.GetSize() )
		m_contours.RemoveAll();
	if( m_OrigConts.GetSize() )
		m_OrigConts.RemoveAll();
//	m_pimgBin = new C8Image;
//	m_pimgIcon = new C8Image;
	m_pimgBin = NULL;
//	m_pimgIcon = NULL;
	m_pimg = 0;
	m_pimgHostBin = 0;
	m_bDragging = FALSE;
	m_pimgToMove = 0;
	m_pimgToDraw = 0;
	m_pimgBinToMove = 0;
	m_pimgBinToDraw = 0;
	m_ptOffset = NOPOINT;
	m_RectHandle.SetInitRect(NORECT);
END_LOG_METHOD();
}

#pragma optimize( "", on )
	

void CObjMove::CreateBIN( CSize size, BOOL bMapContours /*= FALSE*/ )
{
	AllocCopy();

	if (m_pimgBin == NULL)
		m_pimgBin = new C8Image;
	m_pimgBin->Create( (short)size.cx, (short)size.cy, FALSE );
	
	if (bMapContours)
	{
		if (m_bFrame)
		{
			for (int i = 0; i < m_contours.GetSize(); i++)
				FillContour(*m_pimgBin, (CContour *)m_contours[i], m_rc.TopLeft());
		}
		else
		{
			FillContour(*m_pimgBin, (CContour *)m_contours[0], m_rc.TopLeft());
			for (int i = 1; i < m_contours.GetSize(); i++)
				FillContour(*m_pimgBin, (CContour *)m_contours[i], m_rc.TopLeft(), 254, 255);
		}
	}
}

//create mask by external frame
void CObjMove::CreateMask( CCntrNative *pcntr )
{
	AllocCopy();
	ASSERT( m_pimgBin );
	ASSERT( !m_pimgBin->IsEmpty() );
//	ASSERT_KINDOF(CContour,pcntr);

	((CContour*)pcntr)->Map( *m_pimgBin, FRAME_BODY_RAW );
}

CObjMove &CObjMove::operator +=( CObjMove &objAdd )
{
	if( objAdd.IsEmpty() )
		return *this;
	AllocCopy();
	CRect	rcSrc = m_rc;
	CRect	rcAdd = objAdd;
	C8Image	imgSrc = *GetBIN();
	C8Image imgAdd = *objAdd.GetBIN();

	m_rc.left = min(rcSrc.left, rcAdd.left);
	m_rc.top =  min(rcSrc.top, rcAdd.top);
	m_rc.right = max(rcSrc.right, rcAdd.right);
	m_rc.bottom =  max(rcSrc.bottom, rcAdd.bottom);

	CSize	size = m_rc.Size();
	CRect	rcSave = m_rc;

	CImageBase *pimgTemp = m_pimg;
	CImageBase *pimgTemp1 = m_pimgHostBin;
	DeInit();
	CreateBIN( size );

	m_rc = rcSave;

	int	x, y;

	for(x = rcAdd.left; x< rcAdd.right; x++)
		for(y = rcAdd.top; y< rcAdd.bottom; y++)
			if((imgAdd)[y-rcAdd.top][x-rcAdd.left])
				(*m_pimgBin)[y-m_rc.top][x-m_rc.left] = FRAME_BODY_RAW;
	for(x = rcSrc.left; x< rcSrc.right; x++)
		for(y = rcSrc.top; y< rcSrc.bottom; y++)
			if((imgSrc)[y-rcSrc.top][x-rcSrc.left])
				(*m_pimgBin)[y-m_rc.top][x-m_rc.left] = FRAME_BODY_RAW;

	
	FindContours( TRUE );
	CRect rcSet(m_contours[0]->GetRect());
	if (m_contours.GetSize())
	{
		for (int i = 1; i < m_contours.GetSize(); i++)
		{
			CRect rc(m_contours[i]->GetRect());
			if (rc.left < rcSet.left)
				rcSet.left = rc.left;
			if (rc.right > rcSet.right)
				rcSet.right = rc.right;
			if (rc.top < rcSet.top)
				rcSet.top = rc.top;
			if (rc.bottom > rcSet.bottom)
				rcSet.bottom = rc.bottom;
		}
	}
	SetRect(rcSet);
	m_OrigConts.Attach(m_contours);
	m_pimg = pimgTemp;
	m_pimgHostBin = (C8Image *)pimgTemp1;
	return *this;
}

CObjMove &CObjMove::operator -=( CObjMove &objSub )
{
	if( objSub.IsEmpty() )
		return *this;
	AllocCopy();
	CRect	rcSrc = m_rc;
	CRect	rcSub = objSub;
	C8Image	imgSrc = *GetBIN();
	C8Image imgSub = *objSub.GetBIN();
	
	int	iStartX = max(rcSub.left, rcSrc.left);
	int	iStartY = max(rcSub.top, rcSrc.top);
	int	iEndX = min(rcSub.right, rcSrc.right);
	int	iEndY = min(rcSub.bottom, rcSrc.bottom);

	int	x, y;

	CSize	size = m_rc.Size();
	CRect	rcSave = m_rc;

	CImageBase *pimgTemp = m_pimg;
	CImageBase *pimgTemp1 = m_pimgHostBin;
	DeInit();
	CreateBIN( size );

	m_rc = rcSave;

	for(x = rcSrc.left; x< rcSrc.right; x++)
		for(y = rcSrc.top; y< rcSrc.bottom; y++)
			if((imgSrc)[y-rcSrc.top][x-rcSrc.left])
				(*m_pimgBin)[y-m_rc.top][x-m_rc.left] = FRAME_BODY_RAW;

	for(x = iStartX; x< iEndX; x++)
		for(y = iStartY; y< iEndY; y++)
			if((imgSub)[y-rcSub.top][x-rcSub.left])
				(*m_pimgBin)[y-m_rc.top][x-m_rc.left] = 0;
		

	FindContours( TRUE );
	if (m_contours.GetSize())
	{
		CRect rcSet(m_contours[0]->GetRect());
		for (int i = 1; i < m_contours.GetSize(); i++)
		{
			CRect rc(m_contours[i]->GetRect());
			if (rc.left < rcSet.left)
				rcSet.left = rc.left;
			if (rc.right > rcSet.right)
				rcSet.right = rc.right;
			if (rc.top < rcSet.top)
				rcSet.top = rc.top;
			if (rc.bottom > rcSet.bottom)
				rcSet.bottom = rc.bottom;
		}
		if (m_rc != rcSet)
		{
			ASSERT(rcSet.left >= m_rc.left);
			ASSERT(rcSet.top >= m_rc.top);
			ASSERT(rcSet.right <= m_rc.right);
			ASSERT(rcSet.bottom <= m_rc.bottom);
			CSize szNew = rcSet.Size();
			C8Image *pimgNew = new C8Image(szNew.cx, szNew.cy,
				CImageBase::GrayScale);
			::GetImage(pimgNew, m_pimgBin, rcSet.left - m_rc.left,
				rcSet.top - m_rc.top);
			delete m_pimgBin;
			m_pimgBin = pimgNew;
		}
		SetRect(rcSet);
	}
	m_OrigConts.Attach(m_contours);
	m_pimg = pimgTemp;
	m_pimgHostBin = (C8Image *)pimgTemp1;
	return *this;
}				

void CObjMove::FindContours( BOOL bFindHoles )
{
	int	x, y;
	BOOL	bFindSome = FALSE;
	CContour contour;

	m_contours.DeInit();

	if(!m_pimgBin || m_pimgBin->IsEmpty())
		return;

	for(y = 0; y<m_pimgBin->GetCY(); y++)
		for(x = 0; x<m_pimgBin->GetCX(); x++)
			if((*m_pimgBin)[y][x] == FRAME_BODY_RAW)
			{
				//used m_byteBody, m_byteHole for filling,
				//m_byteFill and m_byteBack for back for filling
				contour.SetBack( 0 );
				contour.SetFill( FRAME_BODY_RAW );
				contour.SetBody( FRAME_BODY );
				contour.SetHole( 0x00 );

				contour.SetCont( FRAME_CONT );
				

				contour.Walk( (*m_pimgBin), CPoint(x, y) );

				if( !contour.GetSize() )
				{
					SetToImage( (*m_pimgBin), CPoint(x, y), 0x00 );
					continue;
				}

				
				if( bFindHoles )
					contour.FillFast( (*m_pimgBin), FRAME_MARK_EXT, FRAME_MARK_INT, m_contours );
				else
					contour.FillFast( (*m_pimgBin), FRAME_MARK_EXT );

				m_contours.Add( new CContour( contour ) );

				bFindSome = TRUE;
			}

	if( !bFindSome )
		DeInit();
	else
		for( int i = 0; i < m_contours.GetSize(); i ++ )
			(*(CContour *)m_contours[i])+=m_rc;
}

void CObjMove::PaintFrameNext( CDC &theDC, double iZoom )
{
	for( int i = 0; i<m_contours.GetSize(); i++ )
		(*(CContour *)m_contours[i]).DoPaintNext( theDC, iZoom );
	if (m_nZone == CRectHandler::Line)
		m_RectHandle.DrawLine(theDC, TRUE, iZoom);
}

void CObjMove::PaintFrame( CDC &theDC, double iZoom, BOOL bActive )
{
	for( int i = 0; i<m_contours.GetSize(); i++ )
		(*(CContour *)m_contours[i]).DoPaint1st( theDC, iZoom );
}

void CObjMove::MapToBin( C8Image &imgBin )
{
	if( !m_pimgBin || m_pimgBin->IsEmpty() || m_rc == NORECT )
		return;

	BYTE	*p;
	CRect rc(m_rc);
	if (rc.top < 0) rc.top = 0;
	if (rc.left < 0) rc.left = 0;
	if (rc.bottom >= imgBin.GetCY()) rc.bottom = imgBin.GetCY()-1;
	if (rc.right >= imgBin.GetCX()) rc.right = imgBin.GetCX()-1;
	int dx = rc.left-m_rc.left;
	int dy = rc.top-m_rc.top;

	for( int j = 0; j< rc.Height(); j++ )
	{
		p = (*m_pimgBin)[j+dy]+dx;
		for( int i = 0; i< rc.Width(); i++ )
			if( p[i] )
				imgBin[j+rc.top][i+rc.left] = 0xFF;
	}
}


BOOL CObjMove::IsPointInObject( CPoint pt, BOOL bActive /*= FALSE*/)
{
#if defined(__SNAPSHOT__) && !defined(__PP__)
	if( m_RectHandle.GetZone(pt,1.0) != CRectHandler::Move )
		return FALSE;
#endif
	if (bActive)
	{
		CRect rc(m_RectHandle.CalcInvalidateRect());
		if( !rc.PtInRect( pt ) )
			return FALSE;
		return TRUE;
	}
	else
	{
		if( !m_rc.PtInRect( pt ) )
			return FALSE;
		if (!m_pimgBin || m_pimgBin->IsEmpty())
			return TRUE;
		if( (*m_pimgBin)[pt.y-m_rc.top][pt.x-m_rc.left]==BYTE_RAWBODY )
			return TRUE;
		return FALSE;
	}
}

void CObjMove::AttachCntr( CContourArray &arr )
{
	for( int i = 0; i < arr.GetSize(); i++ )
	{
		CContour *p = new CContour();
		p->Attach( *arr.GetAt( i ) );
		m_contours.Add( p );
	}
}

void CObjMove::AttachBin( CImageBase *pimg )
{
	ASSERT( pimg );
	ASSERT_VALID( pimg );
	ASSERT( !pimg->Is16BitColor() );
	ASSERT( !pimg->IsTrueColor() );

	AllocCopy();
	if( !m_pimgBin )
		m_pimgBin = new C8Image;
	m_pimgBin->Assign8(*(C8Image *)pimg);
}

void CObjMove::AttachHostBin( CImageBase *pimg )
{
	ASSERT( pimg );
	ASSERT_VALID( pimg );
	ASSERT( !pimg->Is16BitColor() );
	ASSERT( !pimg->IsTrueColor() );

	m_pimgHostBin = (C8Image *)pimg;
}

void CObjMove::AttachImg( void *p )
{
	ASSERT_VALID( (CImageBase*)p );

	m_pimg = (CImageBase*)p;
	CImageBase *pImg = (CImageBase*)p;

	AllocCopy();
	if( m_pimgToMove )
	{
		//if 

		if( pImg->IsTrueColor() && !m_pimgToMove->IsTrueColor() )
		{
			int	cx = m_pimgToMove->GetCX(), cy = m_pimgToMove->GetCY();
			int	x, y, x3;
			C8Image	*ptemp = new C8Image( cx, cy, CImageBase::TrueColor );

			for( y = 0; y<cy; y++ )
				for( x = 0, x3 = 0; x<cx; x++ )
				{
					(*ptemp)[y][x3++]=(*m_pimgToMove)[y][x];
					(*ptemp)[y][x3++]=(*m_pimgToMove)[y][x];
					(*ptemp)[y][x3++]=(*m_pimgToMove)[y][x];
				}
			safedelete(m_pimgToMove);
			m_pimgToDraw->Free();
			m_pimgToDraw->Create( cx, cy, CImageBase::TrueColor );
			m_pimgToMove = ptemp;
		}


		if( !pImg->IsTrueColor() && m_pimgToMove->IsTrueColor() )
		{
			int	cx = m_pimgToMove->GetCX(), cy = m_pimgToMove->GetCY();
			int	x, y, x3;
			C8Image	*ptemp = new C8Image( cx, cy, CImageBase::GrayScale );

			for( y = 0; y<cy; y++ )
				for( x = 0, x3 = 0; x<cx; x++ )
				{
					(*ptemp)[y][x]=_Bright( (*m_pimgToMove)[y][x3+2],
											(*m_pimgToMove)[y][x3+1],
											(*m_pimgToMove)[y][x3] );
					x3+=3;
				}

			safedelete(m_pimgToMove);
			m_pimgToDraw->Free();
			m_pimgToDraw->Create( cx, cy, CImageBase::GrayScale );
			m_pimgToMove = ptemp;
		}
	}
}

// Object will reference to old data
void CObjMove::AttachData( CObjNative &obj )
{
	CObjNative::AttachData( obj );

	m_rc = obj.m_rc;

	m_contours.SetSize( obj.m_contours.GetSize() );
	for( int i = 0; i< obj.m_contours.GetSize(); i++ )
		m_contours.SetAt( i, obj.m_contours.GetAt( i ) );
	m_iCode = obj.m_iCode;
	m_iClassNo = obj.m_iClassNo;
	

	if( obj.IsKindOf( RUNTIME_CLASS(CObjMove) ) )
	{
		m_overlay = ((CObjMove&)obj).m_overlay;
		m_OrigConts.SetSize( ((CObjMove&)obj).m_OrigConts.GetSize() );
		for( int i = 0; i< ((CObjMove&)obj).m_OrigConts.GetSize(); i++ )
			m_OrigConts.SetAt( i, ((CObjMove&)obj).m_OrigConts.GetAt( i ) );
		m_pimgBin = ((CObjMove&)obj).m_pimgBin;
		m_bDirectUnknown = ((CObjMove&)obj).m_bDirectUnknown;
		m_pimg = ((CObjMove&)obj).m_pimg;
		m_pimgHostBin = ((CObjMove&)obj).m_pimgHostBin;
		m_pimgToMove = ((CObjMove&)obj).m_pimgToMove;
		m_pimgToDraw = ((CObjMove&)obj).m_pimgToDraw;
#if defined(__VT4__) || defined(__PP__)
		m_pimgBinToMove = ((CObjMove&)obj).m_pimgBinToMove;
		m_pimgBinToDraw = ((CObjMove&)obj).m_pimgBinToDraw;
#endif
		m_ptOffset = ((CObjMove&)obj).m_ptOffset;
		m_RectHandle = ((CObjMove&)obj).m_RectHandle;
		m_bFrame = ((CObjMove&)obj).m_bFrame;
		m_fInitAngle = ((CObjMove&)obj).m_fInitAngle;
		m_fInitKx = ((CObjMove&)obj).m_fInitKx;
		m_fInitKy = ((CObjMove&)obj).m_fInitKy;
		m_ptInitCen = ((CObjMove&)obj).m_ptInitCen;
	}
}

// Data will be copied
void CObjMove::Attach( CObjNative &obj )
{
//	CObjNative::Attach( obj );

	DeInit();
	//copy data
	AttachCntr( obj.m_contours );
	m_rc = obj.m_rc;
	m_iClassNo = obj.m_iClassNo;

	if( obj.IsKindOf( RUNTIME_CLASS(CObjMove) ) )
	{
		m_overlay = ((CObjMove&)obj).m_overlay;
		m_OrigConts.Attach(((CObjMove&)obj).m_OrigConts);
		if (!m_pimgBin)
			m_pimgBin = new C8Image;
		m_pimgBin->Attach( ((CObjMove&)obj).m_pimgBin );
		if (!m_pimgIcon)
			m_pimgIcon = new C8Image;
		m_pimgIcon->Attach( ((CObjMove&)obj).m_pimgIcon );
		ASSERT(!m_pimgIcon->GetBI() || m_pimgIcon->GetCX() == m_pimgIcon->GetBI()->biWidth &&
		m_pimgIcon->GetCY() == m_pimgIcon->GetBI()->biHeight);
		m_pimg = ((CObjMove&)obj).m_pimg;
		m_pimgHostBin = ((CObjMove&)obj).m_pimgHostBin;
		CObjMove *p = (CObjMove *)&obj;
		if( p->m_pimgToMove )m_pimgToMove = new C8Image( *p->m_pimgToMove );
		if( p->m_pimgToDraw )m_pimgToDraw = new C8Image( *p->m_pimgToDraw );
#if defined(__VT4__) || defined(__PP__)
		if( p->m_pimgBinToMove )m_pimgBinToMove = new C8Image( *p->m_pimgBinToMove );
		if( p->m_pimgBinToDraw )m_pimgBinToDraw = new C8Image( *p->m_pimgBinToDraw );
#endif
		m_ptOffset = ((CObjMove&)obj).m_ptOffset;
		m_RectHandle = ((CObjMove&)obj).m_RectHandle;
		m_bFrame = ((CObjMove&)obj).m_bFrame;
		m_fInitAngle = ((CObjMove&)obj).m_fInitAngle;
		m_fInitKx = ((CObjMove&)obj).m_fInitKx;
		m_fInitKy = ((CObjMove&)obj).m_fInitKy;
		m_ptInitCen = ((CObjMove&)obj).m_ptInitCen;
	}
}

void CObjMove::AttachMoveImage( CObjNative &obj )
{
	AllocCopy();
	if( obj.IsKindOf( RUNTIME_CLASS(CObjMove) ) )
	{
		CObjMove *p = (CObjMove *)&obj;
		m_overlay = p->m_overlay;
		safedelete(m_pimgToMove);
		safedelete(m_pimgToDraw);
#if defined(__VT4__) || defined(__PP__)
		safedelete(m_pimgBinToMove);
		safedelete(m_pimgBinToDraw);
#endif
		if( p->m_pimgToMove )m_pimgToMove = new C8Image( *p->m_pimgToMove );
		if( p->m_pimgToDraw )m_pimgToDraw = new C8Image( *p->m_pimgToDraw );
#if defined(__VT4__) || defined(__PP__)
		if( p->m_pimgBinToMove )m_pimgBinToMove = new C8Image( *p->m_pimgBinToMove );
		if( p->m_pimgBinToDraw )m_pimgBinToDraw = new C8Image( *p->m_pimgBinToDraw );
#endif
	}
}

LPBITMAPINFOHEADER CObjMove::GetBinBI()
{
	return m_pimgBin?m_pimgBin->GetBI():NULL;
}

LPBITMAPINFOHEADER CObjMove::GetImgBI()
{
	ASSERT( m_pimg );
	return m_pimg->GetBI();
}


LPBITMAPINFOHEADER CObjMove::GetIconBI()
{
	ASSERT( m_pimg );

//	CreateIcon( g_PalWork.m_clrIconBack );

	return m_pimgIcon->GetBI();
}

//////////////////////////////////
//CObjArray - override

CObjNative *CObjArray::CreateObjNative()
{
	return new CObjNative();
}

CObjNative *CObjArray::CreateObjMove()
{
	return new CObjMove();
}

CObjNative *CObjArray::CreateObjMeasure()
{
#if defined(__VT4__)
	return new CObjMeasure();
#else
	return NULL;
#endif
}



CObjNative& CObjMove::operator = (CObjNative &obj)
{
BEGIN_LOG_METHOD(equal_operator,2);
	DeInit();

	CObjMove	*p  = (CObjMove *)&obj;

	if( !p->m_plUsage )
		p->m_plUsage = new int( 1 );
	m_plUsage = p->m_plUsage;
	(*m_plUsage)++;
//here copy contours and rect

	AttachData( obj );
END_LOG_METHOD();
	return *this;
}

void CObjMove::AllocCopy( )
{
	if (m_plUsage && *m_plUsage > 1)
		DoAllocCopy();
}

void CObjMove::DoAllocCopy( )
{
	(*m_plUsage)--;
	m_plUsage = 0;
	for (int i = 0; i < m_contours.GetSize(); i++)
	{
		CCntrNative *s = (CCntrNative *)m_contours.GetAt(i);
		CContour *p = (CContour *)s->GetRuntimeClass()->CreateObject();
		p->Attach(*(CCntrNative *)m_contours.GetAt(i));
		m_contours.SetAt( i, p );
	}
	if( IsKindOf( RUNTIME_CLASS(CObjMove) ) )
	{
		for (int i = 0; i < m_OrigConts.GetSize(); i++)
		{
			CCntrNative *s = (CCntrNative *)m_OrigConts.GetAt(i);
			CContour *p = (CContour *)s->GetRuntimeClass()->CreateObject();
			// new !!! BUG fix on move with content.
			p->Attach(*(CCntrNative *)m_OrigConts.GetAt(i));
			m_OrigConts.SetAt( i, p );
		}
		if (m_pimgBin)
			m_pimgBin = new C8Image( *m_pimgBin );
//		if (m_pimgIcon)
//			m_pimgIcon = new C8Image( *m_pimgIcon );
		if (m_pimgToMove)
			m_pimgToMove = new C8Image( *m_pimgToMove );
		if (m_pimgToDraw)
			m_pimgToDraw = new C8Image( *m_pimgToDraw );
#if defined(__VT4__) || defined(__PP__)
		if (m_pimgBinToMove)
			m_pimgBinToMove = new C8Image( *m_pimgBinToMove );
		if (m_pimgBinToDraw)
			m_pimgBinToDraw = new C8Image( *m_pimgBinToDraw );
#endif
	}
}

void CObjArray::Attach( CObjNativeArray *parr )
{
	DeInit();
	for( int i = 0; i < parr->GetSize(); i++ )
	{
		CObjMove	*pobj = (CObjMove *)parr->GetAt( i )->GetRuntimeClass()->CreateObject();
		*pobj = ( *parr->GetAt( i ) );
		Add( pobj );
	}
}

void CObjArray::AttachCopy( CObjNativeArray *parr )
{
	DeInit();
	for( int i = 0; i < parr->GetSize(); i++ )
	{
		CObjMove	*pobj = (CObjMove *)parr->GetAt( i )->GetRuntimeClass()->CreateObject();
		pobj->Attach ( *parr->GetAt( i ) );
		Add( pobj );
	}
}

void CObjArray::AddArray( CObjNativeArray &arr )
{
	for( int i = 0; i < arr.GetSize(); i++ )
	{
		CObjMove	*pobj = (CObjMove *)arr.GetAt( i )->GetRuntimeClass()->CreateObject();
		*pobj = ( *arr.GetAt( i ) );
		Add( pobj );
	}
}

void CObjMove::DoPaint(CDC &theDC, double iZoom, int iType, BOOL bActive )
{
	ASSERT(FALSE); // Not supported here
}

void CObjMove::PaintMoveImage(CDC &theDC, double iZoom, int iType, BOOL bActive, CObjArray *pArr, CPoint ptOffset )
{
	ASSERT(FALSE); // Not supported here
}

void CObjMove::PaintHandles(CDC &theDC, double fZoom, int iType, BOOL bActive )
{
	ASSERT(FALSE); // Not supported here
}

void CObjMove::Serialize( CArchive &ar )
{
	int	iSize=  m_contours.GetSize();
	BOOL	bIsBin = m_pimgBin != 0;
//	int nSchema = ar.IsLoading()?ar.GetObjectSchema():0;
//	ASSERT(nSchema > -1);
	GetSchema(ar);

	if( ar.IsStoring() )
	{
		ar<<(long)m_iCode;
		ar<<(long)(m_iCode>>32);
		ar<<m_rc.left;
		ar<<m_rc.top;
		ar<<m_rc.right;
		ar<<m_rc.bottom;
		ar<<m_iClassNo;
		ar<<m_bDirectUnknown;

		ar<<iSize;

		bIsBin = m_pimgBin != 0;
		ar<<bIsBin;
		if( bIsBin )m_pimgBin->Serialize( ar );

		for(int i= 0; i<iSize; i++ )
		{
			CCntrNative	*p = m_contours[i];
			p->Serialize(ar);
		}

		int iOrigSize = m_OrigConts.GetSize();
		ar<<iOrigSize;
		for(i= 0; i<iOrigSize; i++ )
		{
			CCntrNative	*p = m_OrigConts[i];
			p->Serialize(ar);
		}

		ar << m_bNeedToSerializeIcon;
		if( m_bNeedToSerializeIcon )
			SerializeIcon( ar );
	}
	else
	{	
		long l,h;
		DeInit();
		ar>>l;
		ar>>h;
		m_iCode = (__int64(h)<<32)+l;
		if (g_iObjCounter <= m_iCode)
			g_iObjCounter = m_iCode+1;	
		ar>>m_rc.left;
		ar>>m_rc.top;
		ar>>m_rc.right;
		ar>>m_rc.bottom;
		ar>>m_iClassNo;
		ar>>m_bDirectUnknown;

		ar>>iSize;

		ar>>bIsBin;
		if( bIsBin )
		{
			if( !m_pimgBin )
				m_pimgBin = new C8Image;
			m_pimgBin->Serialize( ar );
		}

		for(int i= 0; i<iSize; i++ )
		{
			CContour	*p = new CContour;
			p->Serialize(ar);
			/*
			int	is;
			void *pp;
			ar>>is;
			pp = AllocMem( sizeof( CshortPoint )*is );
			ar.Read( pp, sizeof( CshortPoint )*is );
			p->Attach( (POINTS *)pp, is );
			
#if defined(_DEBUG)
			for (int j = 0; j < p->GetSize(); j++)
			{
				ASSERT((*p)[j].x >= m_rc.left && (*p)[j].x <= m_rc.right);
				ASSERT((*p)[j].y >= m_rc.top && (*p)[j].y <= m_rc.bottom);
			}
#endif		*/
			m_contours.Add( p );
		}
		int iOrigSize;
		ar>>iOrigSize;
		for(i= 0; i<iOrigSize; i++ )
		{
			CContour	*p = new CContour;
			p->Serialize(ar);
			m_OrigConts.Add( p );
		}
		ar>>m_bNeedToSerializeIcon;
		if( m_bNeedToSerializeIcon )
			SerializeIcon( ar );
	}

	BOOL	bSrc = m_pimgToMove!=0;
	BOOL	bS16 = FALSE;
	BOOL	bPnt = m_pimgToDraw!=0;
	BOOL	bP16 = FALSE;
	BOOL	bSrcb = m_pimgBinToMove!=0;
	BOOL	bSb16 = FALSE;
	BOOL	bPntb = m_pimgBinToDraw!=0;
	BOOL	bPb16 = FALSE;
	
	if( ar.IsStoring() )
	{
		if( bSrc )	bS16 = m_pimgToMove->Is16BitColor();
		if( bPnt )	bP16 = m_pimgToDraw->Is16BitColor();
		if( bSrcb )	bSb16 = m_pimgBinToMove->Is16BitColor();
		if( bPntb )	bPb16 = m_pimgBinToDraw->Is16BitColor();
		ar<<bSrc;
		ar<<bPnt;
		ar<<bS16;
		ar<<bP16;
		ar<<bSrcb;
		ar<<bPntb;
		ar<<bSb16;
		ar<<bPb16;
		if( bSrc )	m_pimgToMove->Serialize( ar );
		if( bPnt )	m_pimgToDraw->Serialize( ar );
		if( bSrcb )	m_pimgBinToMove->Serialize( ar );
		if( bPntb )	m_pimgBinToDraw->Serialize( ar );
		ar<<m_ptOffset.x;
		ar<<m_ptOffset.y;
	}
	else
	{
		ar>>bSrc;
		ar>>bPnt;
		ar>>bS16;
		ar>>bP16;
		ar>>bSrcb;
		ar>>bPntb;
		ar>>bSb16;
		ar>>bPb16;
		if( bSrc )
		{
			m_pimgToMove = new C8Image;
			m_pimgToMove->Serialize( ar );
		}
		if( bPnt )
		{
			m_pimgToDraw = new C8Image;
			m_pimgToDraw->Serialize( ar );
		}
		if( bSrcb )
		{
			m_pimgBinToMove = new C8Image;
			m_pimgBinToMove->Serialize( ar );
		}
		if( bPntb )
		{
			m_pimgBinToDraw = new C8Image;
			m_pimgBinToDraw->Serialize( ar );
		}
		ar>>m_ptOffset.x;
		ar>>m_ptOffset.y;
	}

	m_RectHandle.Serialize(ar);

	if (ar.IsStoring())
	{
		ar << m_fInitAngle;
		ar << m_fInitKx;
		ar << m_fInitKy;
		ar << m_ptInitCen;
	}
	else
	{
		if (m_nObjSchema >= 3)
		{
			ar >> m_fInitAngle;
			ar >> m_fInitKx;
			ar >> m_fInitKy;
			ar >> m_ptInitCen;
		}
	}
}



void CObjMove::CreateIcon( COLORREF crBack, BOOL bForceCreate )
{
	ASSERT(FALSE); // Not supported here
}


void CObjMove::SetLine(double fFract, CView *pView)
{
}

CRect CObjMove::CalcRect()
{
	if (m_contours.GetSize())
	{
		CRect rcSet(m_contours[0]->GetRect());
		for (int i = 1; i < m_contours.GetSize(); i++)
		{
			CRect rc(m_contours[i]->GetRect());
			if (rc.left < rcSet.left)
				rcSet.left = rc.left;
			if (rc.right > rcSet.right)
				rcSet.right = rc.right;
			if (rc.top < rcSet.top)
				rcSet.top = rc.top;
			if (rc.bottom > rcSet.bottom)
				rcSet.bottom = rc.bottom;
		}
		return rcSet;
	}
	return NORECT;
}

CRect CObjMove::CalcOrigRect()
{
	if (m_contours.GetSize())
	{
		CRect rcSet(m_OrigConts[0]->GetRect());
		for (int i = 1; i < m_OrigConts.GetSize(); i++)
		{
			CRect rc(m_OrigConts[i]->GetRect());
			if (rc.left < rcSet.left)
				rcSet.left = rc.left;
			if (rc.right > rcSet.right)
				rcSet.right = rc.right;
			if (rc.top < rcSet.top)
				rcSet.top = rc.top;
			if (rc.bottom > rcSet.bottom)
				rcSet.bottom = rc.bottom;
		}
		return rcSet;
	}
	return NORECT;
}

void CObjMove::ScaleInitContour(POINT *pt)
{
	AllocCopy();
	CPoint *ptn = m_RectHandle.GetPoints();
	double dx = FDistLine(pt[0], pt[1], pt[2]) / FDistLine(ptn[0], 
		ptn[1], pt[2]);
	double dy = FDistLine(pt[1], pt[2], pt[3]) / FDistLine(ptn[1], 
		ptn[2], pt[3]);
	if (m_OrigConts.GetSize() == 0)
		m_OrigConts.Attach(m_contours);
	CRect rc(m_OrigConts[0]->GetRect());
	for (int i = 0; i < m_OrigConts.GetSize(); i++)
	{
		CContour *c = (CContour *)m_OrigConts[i];
		CshortPoint *pts = c->GetPtArray();
		CRect rcSet;
		int nSize = c->GetSize();
		for (int j = 0; j < nSize; j++)
		{
			CDblPoint ptOrg(pts[j].x-rc.left,pts[j].y-rc.top);
			ptOrg.x *= dx;
			ptOrg.y *= dy;
			pts[j].x = (short)(rc.left+ptOrg.x);
			pts[j].y = (short)(rc.top+ptOrg.y);
			if (j == 0)
			{
				rcSet = CRect(pts[j].x,pts[j].y,pts[j].x,pts[j].y);
			}
			else
			{
				if (pts[j].x < rcSet.left)
					rcSet.left = pts[j].x;
				if (pts[j].x > rcSet.right)
					rcSet.right = pts[j].x;
				if (pts[j].y < rcSet.top)
					rcSet.top = pts[j].y;
				if (pts[j].y > rcSet.bottom)
					rcSet.bottom = pts[j].y;
			}
		}
		c->SetRect(rcSet);
	}
}

void CObjMove::MoveContour(POINT ptOffs)
{
	ASSERT(CalcRect().Width() == m_rc.Width());
	AllocCopy();
	ASSERT(CalcRect().Width() == m_rc.Width());
	for (int i = 0; i < m_contours.GetSize(); i++)
	{
		CContour *c = (CContour *)m_contours[i];
		*c += ptOffs;
	}
	ASSERT(CalcRect().Width() == m_rc.Width());
	for (i = 0; i < m_OrigConts.GetSize(); i++)
	{
		CContour *c = (CContour *)m_OrigConts[i];
		*c += ptOffs;
	}
	CRect rcTst(CalcRect());
	ASSERT(CalcRect().Width() == m_rc.Width());
	m_rc = CalcRect();//m_contours[0]->GetRect();
	m_RectHandle += ptOffs;
}

void CObjMove::Rotate(double fSin, double fCos, double fAngle)
{
	if( fabs(fSin) < 0.00001 )
		fSin = 0.0;
	if( fabs(fCos) < 0.00001 )
		fCos = 0.0;
	AllocCopy();
	CRect rcOrig = CalcOrigRect();
	for (int i = 0; i < m_contours.GetSize(); i++)
	{
		CContour Temp;
		Temp.Attach(*m_OrigConts.GetAt(i));
		CshortPoint *p = Temp.GetPtArray();
		int nSize = Temp.GetSize();
		::Resize(rcOrig.TopLeft(),p,nSize,m_RectHandle.GetScaleX(),m_RectHandle.GetScaleY());
		::Rotate(m_RectHandle.GetCenter(),p,nSize,fSin,fCos);
		CContour *c = (CContour *)m_contours[i];
		c->DeInit();
		for (int j = 0; j < nSize; j++)
			c->AddSegment(CPoint(p[j].x,p[j].y));
	}
	m_rc = CalcRect();//m_contours[0]->GetRect();
	m_RectHandle.Rotate(fSin,fCos,fAngle);
}

void CObjMove::Resize()
{
	AllocCopy();
	CRect rcOrig = CalcOrigRect();
	CDblPoint ptCen(m_RectHandle.CalcRealRect().CenterPoint());
	for (int i = 0; i < m_contours.GetSize(); i++)
	{
		CContour Temp;
		Temp.Attach(*m_OrigConts.GetAt(i));
		CshortPoint *p = Temp.GetPtArray();
		int nSize = Temp.GetSize();
		::Resize(rcOrig.TopLeft(),p,nSize,m_RectHandle.GetScaleX(),m_RectHandle.GetScaleY());
		::Rotate(ptCen,p,nSize,m_RectHandle.Sin(),
			m_RectHandle.Cos());
		CContour *c = (CContour *)m_contours[i];
		c->DeInit();
		for (int j = 0; j < nSize; j++)
			c->AddSegment(CPoint(p[j].x,p[j].y));
	}
	m_rc = CalcRect();//m_contours[0]->GetRect();
	ASSERT(m_rc.Width() > 0 && m_rc.Height() > 0);
	m_RectHandle.Resize();
}

void CObjMove::FinalizeData(CView *pView)
{
	ASSERT(FALSE); // Not supported here
}

void CObjMove::MoveTo(CPoint pt, CView *pView, CDocument *pDoc)
{
	CPoint ptCen(m_rc.CenterPoint());
	CPoint ptOffs(pt.x - ptCen.x, pt.y - ptCen.y);
	MoveContour(ptOffs);
}

HRESULT __stdcall CObjMove::MoveTo(int x, int y)
{
	CPoint pt(x,y);
	MoveTo(pt,NULL,NULL);
	return S_OK;
}

static CPoint m_ptObj[5];
static CPoint m_ptOldObj[5];
static CPoint m_ptLine[2];
static double m_fSin,m_fCos;
static CRect  m_rcInit;
static CDblPoint m_ptCen;
static CPoint m_ptOffs;
static double m_fFract;
static BOOL m_bMoved;
static CRect g_rcInv1;


BOOL CObjMove::OnBeginMove(CPoint pt, CPoint ptBmp, int MoveMode, double fZoom, CView *pView)
{
	ASSERT(FALSE); // Not supported here
	return TRUE;
}

void CObjMove::OnMove(CPoint ptStart, CPoint pt, double fZoom)
{
#if defined(__SNAPSHOT__) && !defined(__PP__)
	if( m_nZone != CRectHandler::Move )
		m_nZone = CRectHandler::Outside;
#endif
	if (m_nZone == CRectHandler::SizeSynch0)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[2];
		CPoint ptNOffs = m_RectHandle.CalcNewScale(0,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[2];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[2] == pt1);
	}
	else if (m_nZone == CRectHandler::SizeSynch1)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[3];
		CPoint ptNOffs = m_RectHandle.CalcNewScale(1,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[3];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[3] == pt1);
	}
	else if (m_nZone == CRectHandler::SizeSynch2)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[0];
		m_RectHandle.CalcNewScale(2,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[0];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[0] == pt1);
	}
	else if (m_nZone == CRectHandler::SizeSynch3)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[1];
		CPoint ptNOffs = m_RectHandle.CalcNewScale(3,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[1];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[1] == pt1);
	}
	else if (m_nZone == CRectHandler::SizeTop)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[2];
		CPoint ptNOffs = m_RectHandle.CalcNewScale(4,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[2];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[2] == pt1);
	}
	else if (m_nZone == CRectHandler::SizeLeft)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[1];
		CPoint ptNOffs = m_RectHandle.CalcNewScale(5,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[1];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[1] == pt1);
	}
	else if (m_nZone == CRectHandler::SizeBottom)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[0];
		CPoint ptNOffs = m_RectHandle.CalcNewScale(6,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[0];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[0] == pt1);
	}
	else if (m_nZone == CRectHandler::SizeRight)
	{
		CPoint pt1 = m_RectHandle.GetPoints()[0];
		CPoint ptNOffs = m_RectHandle.CalcNewScale(7,pt,m_ptOffs);
		Resize();
		CPoint pt2 = m_RectHandle.GetPoints()[0];
		MoveContour(CPoint(pt1.x-pt2.x,pt1.y-pt2.y));
		ASSERT(m_RectHandle.GetPoints()[0] == pt1);
	}
	else if (m_nZone == CRectHandler::Rotate0)
	{
		CDblRect rcInit(m_RectHandle.CalcRealRect());
		CDblPoint ptOrg(rcInit.TopLeft());
		::CalcAngle(m_RectHandle.GetCenter(),ptOrg,pt,m_fSin,m_fCos);
		Rotate( m_fSin, m_fCos, AngleFromSin(m_fSin,m_fCos) );
	}
	else if (m_nZone == CRectHandler::Rotate1)
	{
		CDblRect rcInit(m_RectHandle.CalcRealRect());
		CDblPoint ptOrg(rcInit.TopRight());
		::CalcAngle(m_RectHandle.GetCenter(),ptOrg,pt,m_fSin,m_fCos);
		Rotate( m_fSin, m_fCos, AngleFromSin(m_fSin,m_fCos) );
	}
	else if (m_nZone == CRectHandler::Rotate2)
	{
		CDblRect rcInit(m_RectHandle.CalcRealRect());
		CDblPoint ptOrg(rcInit.BottomRight());
		::CalcAngle(m_RectHandle.GetCenter(),ptOrg,pt,m_fSin,m_fCos);
		Rotate( m_fSin, m_fCos, AngleFromSin(m_fSin,m_fCos) );
	}
	else if (m_nZone == CRectHandler::Rotate3)
	{
		CDblRect rcInit(m_RectHandle.CalcRealRect());
		CDblPoint ptOrg(rcInit.BottomLeft());
		::CalcAngle(m_RectHandle.GetCenter(),ptOrg,pt,m_fSin,m_fCos);
		Rotate( m_fSin, m_fCos, AngleFromSin(m_fSin,m_fCos) );
	}
	else if (m_nZone == CRectHandler::Line)
	{
		double fFract;
		CPoint *ptObj = m_RectHandle.GetPoints();
		CPoint ptBas(pt.x-m_ptOffs.x,pt.y-m_ptOffs.y);
		double dx1 = FDistLine(ptBas,ptObj[1],ptObj[2]);
		double dx2 = FDistLine(ptBas,ptObj[0],ptObj[3]);
		CDblRect rcInit = m_RectHandle.CalcRealRect();
		if (dx1 >= rcInit.Width())
			fFract = 1.;
		else if (dx2 >= rcInit.Width())
			fFract = 0.;
		else
			fFract = dx1/rcInit.Width();
		m_RectHandle.SetFract(fFract);
		m_RectHandle.CalcData();
	}
	else if (m_nZone == CRectHandler::Move)
	{
		CPoint ptOffs(pt.x-ptStart.x,pt.y-ptStart.y);
		MoveContour(ptOffs);
	}
}

int CObjMove::GetMoveMode() {return m_nZone;}


BOOL CObjMove::OnEndMove(CPoint ptBmp, CPoint ptBmpStart, double fZoom,
	CRect &rcInv1, CRect &rcInv2, CView *pView)
{
#if defined(__SNAPSHOT__) && !defined(__PP__)
	if( m_nZone != CRectHandler::Move )
		m_nZone = CRectHandler::Outside;
#endif
	rcInv1 = NORECT;
	rcInv2 = NORECT;
	BOOL bChanged = FALSE;
	m_bDragging = FALSE;
	if (m_nZone == CRectHandler::SizeSynch0 ||
		m_nZone == CRectHandler::SizeSynch1 ||
		m_nZone == CRectHandler::SizeSynch2 ||
		m_nZone == CRectHandler::SizeSynch3 ||
		m_nZone == CRectHandler::SizeTop    ||
		m_nZone == CRectHandler::SizeRight  ||
		m_nZone == CRectHandler::SizeBottom ||
		m_nZone == CRectHandler::SizeLeft   ||
		m_nZone == CRectHandler::Rotate0    ||
		m_nZone == CRectHandler::Rotate1    ||
		m_nZone == CRectHandler::Rotate2    ||
		m_nZone == CRectHandler::Rotate3    ||
		m_nZone == CRectHandler::Move       ||
		m_nZone == CRectHandler::Line)
	{
		if (ptBmp != ptBmpStart)
		{
			rcInv1 = g_rcInv1;
			ASSERT(rcInv1 != NORECT);
			if (m_nZone == CRectHandler::Line)
			{
#if defined(__VT4__)
				BeginMeasure( (CImgDoc *)pView->GetDocument(), NULL );
				DoMeasure();
				EndMeasure();
#endif
			}
			else
				FinalizeData(pView);
			rcInv2 = m_RectHandle.CalcInvalidateRect();
			ASSERT(rcInv2 != NORECT);
			ASSERT(rcInv2.left <= m_rc.left + 1);
			ASSERT(rcInv2.top <= m_rc.top + 1);
			ASSERT(rcInv2.right >= m_rc.right - 1);
			ASSERT(rcInv2.bottom >= m_rc.bottom - 1);
			bChanged = TRUE;
		}
		else
			rcInv1 = g_rcInv1;
	}
	else if (m_nZone == -1)
	{
		// Called from CFrame::DoPasteNative. Invalidate rectangles would be
		// calculated in CFrame::DoPasteNative.
		//no call here, moved top DoPasteNative
		//FinalizeData(pView);
	}
	m_nZone = -1;
	return bChanged;
}


BOOL CObjMove::OnSetCursor(CPoint pt, double fZoom)
{
	if (pt == CPoint(-1,-1))
	{
		ASSERT(m_nZone != -1);
		return CRectHandler::SetZoneCursor(m_nZone,m_RectHandle.Sin());
	}
	return m_RectHandle.OnSetCursor(pt,fZoom);
}

//static double g_fInitAngle = 0.;
//static double fInitKx = 1.;
//static double fInitKy = 1.;
//static CPoint g_ptInitCen;

void CObjMove::InitMoveImage( OVERLAY_KIND ok, CBaseDoc *pDoc)
{
	AllocCopy();
	safedelete( m_pimgToMove );
	safedelete( m_pimgToDraw );
#if defined(__VT4__) || defined(__PP__)
	safedelete( m_pimgBinToMove );
	safedelete( m_pimgBinToDraw );
#endif

	if( !m_pimg )
		return;

//	m_fInitAngle = AngleFromSin(m_RectHandle.Sin(),m_RectHandle.Cos());
	double fInitAngle = AngleFromSin(m_RectHandle.Sin(),m_RectHandle.Cos());
	m_fInitAngle = m_RectHandle.GetAngle();
	m_fInitKx = m_RectHandle.GetScaleX();
	m_fInitKy = m_RectHandle.GetScaleY();
	m_ptInitCen = m_RectHandle.GetCenter();
	m_ptInitCen.x -= m_rc.left;
	m_ptInitCen.y -= m_rc.top;
	m_pimgToMove = new C8Image;
	m_pimgToDraw = new C8Image;
#if defined(__VT4__) || defined(__PP__)
	m_pimgBinToMove = new C8Image;
	m_pimgBinToDraw = new C8Image;
#endif
	m_pimgToMove->Attach( m_pimg, m_rc );
	m_pimgToDraw->Create( m_rc.Width(), m_rc.Height(), m_pimg->IsTrueColor() );
	if (m_pimgToDraw->IsGrayScale() && !m_pimg->Is16BitColor() && _GetProfileInt("Settings", "RealColors", 0))
		m_pimgToDraw->ReplacePalette( ((C8Image *)m_pimg)->GetRGBQuad());
#if defined(__VT4__) || defined(__PP__)
	if (m_bFrame)
	{
		if( m_pimgHostBin )
			m_pimgBinToMove->Attach( m_pimgHostBin, m_rc );
	}
	else
	{
		m_pimgBinToMove->Create( m_rc.Width(), m_rc.Height(), m_pimg->IsTrueColor() );
		::SetImage(m_pimgBinToMove, m_pimgBin, 0, 0);
	}
	m_pimgBinToDraw->Create( m_rc.Width(), m_rc.Height(), m_pimg->IsTrueColor() );
#endif
	m_overlay = ok;
	m_bDragging = TRUE;
	m_ptOffset = NOPOINT;
#if defined(__VT4__)
	if (pDoc) MapBackground(pDoc);
#endif
}

void CObjMove::DeInitMoveImage()
{
	AllocCopy();
	m_bDragging = FALSE;
	safedelete( m_pimgToMove );
	safedelete( m_pimgToDraw );
	m_imgToMoveRotated.Free();
#if defined(__VT4__) || defined(__PP__)
	safedelete( m_pimgBinToMove );
	safedelete( m_pimgBinToDraw );
	m_imgBinToMoveRotated.Free();
	if (!IsPermanentAngle())
	{
		m_OrigConts.DeInit();
		m_OrigConts.Attach(m_contours);
		m_RectHandle.SetInitRect(m_rc);
	}
#endif
	m_ptOffset = NOPOINT;
}

/*
void CObjMove::Overlay()
{
	if( !m_pimg )
		return;
	register int	x, y;
	register int	cx, cy;
	register int	xs, ys, xss, yss;
	BYTE	*p1, *p2, *p3, *p4;
	CRect	rc;

	m_bDragging = TRUE;

//	double fAngleOld = m_fInitAngle - AngleFromSin(m_RectHandle.Sin(),m_RectHandle.Cos());
//	double fAngle = m_fInitAngle - m_RectHandle.GetAngle();
	double fAngle = m_fInitAngle - AngleFromSin(m_RectHandle.Sin(),m_RectHandle.Cos());
	double fKx = m_RectHandle.GetScaleX()/m_fInitKx;
	double fKy = m_RectHandle.GetScaleY()/m_fInitKy;
	C8Image *pimgMove;
	bool bNeedRotate = false;
//	C8Image imgNew;
	// Create image for drawing
	if (fAngle != 0. || fKx != 1. || fKy != 1.)
	{
		if (fAngle != m_fSavedAngle || fKx != m_fSavedKx || fKy != m_fSavedKy || m_imgToMoveRotated.IsEmpty() ||
			m_pimgToDraw->GetCX() != m_rc.Width() || m_pimgToDraw->GetCY() != m_rc.Height() ||
			m_pimgBinToDraw->GetCX() != m_rc.Width() || m_pimgBinToDraw->GetCY() != m_rc.Height())
		{
			bNeedRotate = true;
			m_imgToMoveRotated.Create( m_rc.Width(), m_rc.Height(), m_pimg->IsTrueColor() );
			if (m_pimgToDraw->GetCX() != m_rc.Width() || m_pimgToDraw->GetCY() != m_rc.Height())
				m_pimgToDraw->Create( m_rc.Width(), m_rc.Height(), m_pimg->IsTrueColor() );
			CPoint ptCen = m_RectHandle.GetCenter();
			ptCen.x -= m_rc.left;
			ptCen.y -= m_rc.top;
			CMapData md;
			CAphineParams params;
			params.ptCenter = m_ptInitCen;
			params.ptCenterNew = ptCen;
			params.fKx = fKx;
			params.fKy = fKy;
			params.fAngle = fAngle;
			params.bFillOutside = TRUE;
			md.m_rc = CRect( 0, 0, m_pimgToMove->GetCX(), m_pimgToMove->GetCY() );
			DoAphineTransform(&m_imgToMoveRotated,m_pimgToMove,md,params);
			m_fSavedAngle = fAngle;
			m_fSavedKx = fKx;
			m_fSavedKy = fKy;
		}
		pimgMove = &m_imgToMoveRotated;
	}
	else pimgMove = m_pimgToMove;

	rc.left = max( 0, m_rc.left );
	rc.right = min( m_pimg->GetCX(), m_rc.right );
	rc.top = max( 0, m_rc.top );
	rc.bottom = min( m_pimg->GetCY(), m_rc.bottom );

	xs = rc.left-m_rc.left;
	ys = rc.top-m_rc.top;
	xss = rc.left;
	yss = rc.top;

	cx = min(rc.right-m_rc.left,pimgMove->GetCX());
	cy = min(rc.bottom-m_rc.top,pimgMove->GetCY());

	
//256-color version

	if( m_pimg->IsGrayScale() )
	{
		for( y = ys; y < cy; y++ )
		{
			p1 = (*pimgMove)[y];
			p2 = (*(C8Image *)m_pimg)[y+yss-ys]+xss-xs;
			p3 = (*m_pimgToDraw)[y];

			if( m_pimgBin && !m_pimgBin->IsEmpty() )
				p4 = (*m_pimgBin)[y];
			else
				p4 = 0;
			switch( m_overlay )
			{
			case OV_1X1://1:1
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = BYTE((p1[x]+p2[x])>>1);
					else
						p3[x] = p2[x];
				break;
			case OV_1X0://1:0
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = p1[x];
					else
						p3[x] = p2[x];
				break;
			case OV_0X1://0:1
				for( x = xs;  x <cx; x++ )
					p3[x] = p2[x];
				break;
			case OV_1X3://1:3
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = BYTE((p1[x]+(p2[x]<<1)+p2[x])>>2);
					else
						p3[x] = p2[x];
				break;
			case OV_3X1://3:1
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = BYTE(((p1[x]<<1)+p1[x]+p2[x])>>2);
					else
						p3[x] = p2[x];
				break;
	//substraction
			case OV_S_1X3://1:3
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = BYTE( 128+((p1[x]-(p2[x]<<1)-p2[x])>>3) );
					else
						p3[x] = p2[x];
				break;
			case OV_S_3X1://3:1
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = BYTE( 128+(((p1[x]<<1)+p1[x]-p2[x])>>3) );
					else
						p3[x] = p2[x];
				break;
			case OV_S_1X1://1:1
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = BYTE( 128+((p1[x]-p2[x])>>2) );
					else
						p3[x] = p2[x];
				break;
			case OV_S_0X1://1:0
				for( x = xs;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
						p3[x] = BYTE( 255-p2[x] );
					else
						p3[x] = p2[x];
				break;
			}
		}
	}
	else
	{
		register int	x3;
		for( y = ys; y < cy; y++ )
		{
			p1 = (*pimgMove)[y];
			p2 = (*(C8Image *)m_pimg)[y+yss-ys]+(xss-xs)*3;
			p3 = (*m_pimgToDraw)[y];

			if( m_pimgBin && !m_pimgBin->IsEmpty() )
				p4 = (*m_pimgBin)[y];
			else
				p4 = 0;

			switch( m_overlay )
			{
			case OV_1X1://1:1
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = BYTE((p1[x3]+p2[x3])>>1);	x3++;
						p3[x3] = BYTE((p1[x3]+p2[x3])>>1);	x3++;
						p3[x3] = BYTE((p1[x3]+p2[x3])>>1);	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
				break;
			case OV_1X0://1:0
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = p1[x3];	x3++;
						p3[x3] = p1[x3];	x3++;
						p3[x3] = p1[x3];	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
				break;
			case OV_0X1://0:1
				for( x = xs, x3=xs*3;  x <cx; x++ )
				{
					p3[x3] = p2[x3];	x3++;
					p3[x3] = p2[x3];	x3++;
					p3[x3] = p2[x3];	x3++;
				}
				break;
			case OV_1X3://1:3
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = BYTE((p1[x3]+(p2[x3]<<1)+p2[x3])>>2);	x3++;
						p3[x3] = BYTE((p1[x3]+(p2[x3]<<1)+p2[x3])>>2);	x3++;
						p3[x3] = BYTE((p1[x3]+(p2[x3]<<1)+p2[x3])>>2);	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
				break;
			case OV_3X1://3:1
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = BYTE((p2[x3]+(p1[x3]<<1)+p1[x3])>>2);	x3++;
						p3[x3] = BYTE((p2[x3]+(p1[x3]<<1)+p1[x3])>>2);	x3++;
						p3[x3] = BYTE((p2[x3]+(p1[x3]<<1)+p1[x3])>>2);	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
				break;
	//substraction
			case OV_S_1X3://1:3
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = BYTE( 128+((p1[x3]-(p2[x3]<<1)-p2[x3])>>3) );	x3++;
						p3[x3] = BYTE( 128+((p1[x3]-(p2[x3]<<1)-p2[x3])>>3) );	x3++;
						p3[x3] = BYTE( 128+((p1[x3]-(p2[x3]<<1)-p2[x3])>>3) );	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
				break;
			case OV_S_3X1://3:1
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = BYTE( 128+(((p1[x3]<<1)+p1[x3]-p2[x3])>>3) );	x3++;
						p3[x3] = BYTE( 128+(((p1[x3]<<1)+p1[x3]-p2[x3])>>3) );	x3++;
						p3[x3] = BYTE( 128+(((p1[x3]<<1)+p1[x3]-p2[x3])>>3) );	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
				break;
			case OV_S_1X1://1:1
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = BYTE( 128+((p1[x3]-p2[x3])>>2) );	x3++;
						p3[x3] = BYTE( 128+((p1[x3]-p2[x3])>>2) );	x3++;
						p3[x3] = BYTE( 128+((p1[x3]-p2[x3])>>2) );	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
				break;
			case OV_S_0X1://1:0
				for( x = xs, x3=xs*3;  x <cx; x++ )
					if( !p4 || p4[x]==0xFF )
					{
						p3[x3] = BYTE( 255-p2[x3] );	x3++;
						p3[x3] = BYTE( 255-p2[x3] );	x3++;
						p3[x3] = BYTE( 255-p2[x3] );	x3++;
					}
					else
					{
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
						p3[x3] = p2[x3];	x3++;
					}
					break;
			}
		}
	}

#if defined(__VT4__) || defined(__PP__)
	// create binary image for drawing.
	if (m_pimgHostBin == NULL && m_pimgHostBin->IsEmpty())
		return;
	if (fAngle != 0. || fKx != 1. || fKy != 1.)
	{
		if (bNeedRotate || m_imgBinToMoveRotated.IsEmpty())
		{
			m_imgBinToMoveRotated.Create( m_rc.Width(), m_rc.Height(), CImageBase::GrayScale );
			if (m_pimgBinToDraw->GetCX() != m_rc.Width() || m_pimgBinToDraw->GetCY() != m_rc.Height())
				m_pimgBinToDraw->Create( m_rc.Width(), m_rc.Height(), CImageBase::GrayScale );
			CPoint ptCen = m_RectHandle.GetCenter();
			ptCen.x -= m_rc.left;
			ptCen.y -= m_rc.top;
			CMapData md;
			CAphineParams params;
			params.ptCenter = m_ptInitCen;
			params.ptCenterNew = ptCen;
			params.fKx = fKx;
			params.fKy = fKy;
			params.fAngle = fAngle;
			params.bFillOutside = TRUE;
			md.m_rc = CRect( 0, 0, m_pimgBinToMove->GetCX(), m_pimgBinToMove->GetCY() );
			DoAphineTransformFast(&m_imgBinToMoveRotated,m_pimgBinToMove,md,params);
		}
		pimgMove = &m_imgBinToMoveRotated;
	}
	else pimgMove = m_pimgBinToMove;
	rc.left = max( 0, m_rc.left );
	rc.right = min( m_pimgHostBin->GetCX()-1, m_rc.right );
	rc.top = max( 0, m_rc.top );
	rc.bottom = min( m_pimgHostBin->GetCY()-1, m_rc.bottom );
	xs = rc.left-m_rc.left;
	ys = rc.top-m_rc.top;
	xss = rc.left;
	yss = rc.top;
	cx = min(rc.right-m_rc.left,pimgMove->GetCX());
	cy = min(rc.bottom-m_rc.top,pimgMove->GetCY());
	for( y = ys; y < cy; y++ )
	{
		p1 = (*pimgMove)[y];
		p2 = (*(C8Image *)m_pimgHostBin)[y+yss-ys]+xss-xs;
		p3 = (*m_pimgBinToDraw)[y];

		if( m_pimgBin && !m_pimgBin->IsEmpty() )
			p4 = (*m_pimgBin)[y];
		else
			p4 = 0;
		switch( m_overlay )
		{
		case OV_1X1://1:1
			for( x = xs;  x <cx; x++ )
				if( !p4 || p4[x]==0xFF )
					p3[x] = BYTE((p1[x]+p2[x])>>1);
				else
					p3[x] = p2[x];
			break;
		case OV_1X0://1:0
			for( x = xs;  x <cx; x++ )
				if( !p4 || p4[x]==0xFF )
					p3[x] = p1[x];
				else
					p3[x] = p2[x];
			break;
		case OV_0X1://0:1
			for( x = xs;  x <cx; x++ )
				p3[x] = p2[x];
			break;
		case OV_1X3://1:3
			for( x = xs;  x <cx; x++ )
				if( !p4 || p4[x]==0xFF )
					p3[x] = BYTE((p1[x]+(p2[x]<<1)+p2[x])>>2);
				else
					p3[x] = p2[x];
			break;
		case OV_3X1://3:1
			for( x = xs;  x <cx; x++ )
				if( !p4 || p4[x]==0xFF )
					p3[x] = BYTE(((p1[x]<<1)+p1[x]+p2[x])>>2);
				else
					p3[x] = p2[x];
			break;
		}
	}
#endif
}
*/


void CObjMove::Overlay()
{
	ASSERT(FALSE); // Not supported here
}


void CObjMove::Map( CBaseDoc *pDoc, CRect rc /*= NORECT*/  )
{
	ASSERT(FALSE); // Not supported here
}

void CObjMove::MapBackground(CBaseDoc *pDoc, CObjNative *pExc /*= NULL*/)
{
	ASSERT(FALSE); // Not supported here
}

void CObjMove::SetRect( CRect rc )
{
	AllocCopy();
	CObjNative::SetRect(rc);
	m_RectHandle.SetInitRect(rc);
}

CRect CObjMove::CalcInvalidateRect()
{
	if( m_rc == NORECT )
		return NORECT;

	return m_RectHandle.CalcInvalidateRect();
}

void CObjMove::OnUpdate(CDocument *pDoc, CObjNative* pSender, LPARAM lHint, CObject* pHint)
{
	ASSERT(FALSE); // Not supported here
}

void CObjMove::operator=(CDragState &s)
{
	AllocCopy();
	safedelete(m_pimgToMove);
	safedelete(m_pimgToDraw);
#if defined(__VT4__) || defined(__PP__)
	safedelete(m_pimgBinToMove);
	safedelete(m_pimgBinToDraw);
#endif
	m_pimgToMove = s.m_pimgToMove;
	m_pimgToDraw = s.m_pimgToDraw;
#if defined(__VT4__) || defined(__PP__)
	m_pimgBinToMove = s.m_pimgBinToMove;
	m_pimgBinToDraw = s.m_pimgBinToDraw;
#endif
	s.m_pimgToMove = NULL;
	s.m_pimgToDraw = NULL;
#if defined(__VT4__) || defined(__PP__)
	s.m_pimgBinToMove = NULL;
	s.m_pimgBinToDraw = NULL;
#endif
}

void CObjMove::SerializeIcon( CArchive &ar )
{
//	ASSERT( m_pimgIcon != 0 );
	if (!m_pimgIcon)
		m_pimgIcon = new C8Image;

	ASSERT(!m_pimgIcon->GetBI() || m_pimgIcon->GetCX() == m_pimgIcon->GetBI()->biWidth &&
		m_pimgIcon->GetCY() == m_pimgIcon->GetBI()->biHeight);

	m_pimgIcon->Serialize( ar );
}

DWORD CObjMove::CalcBytesSize() const
{
	DWORD dwSzBase = sizeof(CObjMove);
	if (m_plUsage && *m_plUsage > 1)
		return dwSzBase;
	if (m_pimgBin)
		dwSzBase += m_pimgBin->GetBitsSize() + sizeof(C8Image);
	if (m_pimgIcon)
		dwSzBase += m_pimgIcon->GetBitsSize() + sizeof(C8Image);
	if (m_pimgToMove)
		dwSzBase += m_pimgToMove->GetBitsSize() + sizeof(C8Image);
	if (m_pimgToDraw)
		dwSzBase += m_pimgToDraw->GetBitsSize() + sizeof(C8Image);
	if (m_pimgBinToMove)
		dwSzBase += m_pimgBinToMove->GetBitsSize() + sizeof(C8Image);
	if (m_pimgBinToDraw)
		dwSzBase += m_pimgBinToDraw->GetBitsSize() + sizeof(C8Image);
	dwSzBase += m_OrigConts.CalcBytesSize();
	dwSzBase += m_contours.CalcBytesSize();
	return dwSzBase;
}

void CObjMove::ResetIcon()
{
	if( m_pimg )
		if( m_pimgIcon && !m_bNeedToSerializeIcon )
			m_pimgIcon->Free();

	ASSERT(!m_pimgIcon->GetBI() || m_pimgIcon->GetCX() == m_pimgIcon->GetBI()->biWidth &&
		m_pimgIcon->GetCY() == m_pimgIcon->GetBI()->biHeight);
}

BOOL CObjMove::IsImportedToGallery()
{
	return m_bNeedToSerializeIcon;
}

CObjNativeArray *CObjArray::OnClone(BOOL bCopyContent)
{
	CObjArray *parr = new CObjArray;
	if (parr && bCopyContent)
		parr->Attach(this);
	return parr;
}
