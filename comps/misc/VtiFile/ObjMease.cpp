#include "StdAfx.h"
#include "ObjMease.h"
#include "Math.h"
//#include "CnvWork.h"
//#include "ClassMng.h"
//#include "RGB2HLS.h"
//#include "Params.h"
//#include "ProgLog.h"
//#include "CfgMan.h"
//#include "MeasOpt.h"


//some extension for safearray work
void StoreSafeArray( CArchive &ar, const VARIANT &varSrc )
{
	ASSERT( ar.IsStoring() );
	ASSERT( varSrc.vt&VT_ARRAY );

	SAFEARRAY	*psa = varSrc.parray;

	DWORD	vt = (varSrc.vt^VT_ARRAY);
	DWORD	cDims = SafeArrayGetDim( psa );
	DWORD	dwSizeItem = SafeArrayGetElemsize( psa );
	DWORD	dwSize = dwSizeItem;

	for( unsigned i = 1; i < cDims+1; i++ )
	{
		long	lup = 0, lbuttom = 0;

		SafeArrayGetLBound( psa, i, &lup );
		SafeArrayGetUBound( psa, i, &lbuttom );

		dwSize *= (lbuttom-lup+1);
	}

	ar << vt;
	ar << cDims;
	ar << dwSize;

	void	*pdata = 0;

	ar.Write( &psa->rgsabound[0], sizeof( SAFEARRAYBOUND )*cDims );

	SafeArrayAccessData( psa, &pdata );

	ASSERT( pdata );

	ar.Write( pdata, dwSize );

	SafeArrayUnaccessData( psa );
}

void LoadSafeArray( CArchive &ar, VARIANT &varSrc )
{
	ASSERT( ar.IsLoading() );
	ASSERT( varSrc.vt&VT_ARRAY );

	DWORD	vt = 0;
	DWORD	cDims = 0;
	DWORD	dwSize = 0;

	ar >> vt;
	ar >> cDims;
	ar >> dwSize;

	SAFEARRAYBOUND	*psaBounds;

	psaBounds = new SAFEARRAYBOUND[cDims];

	ar.Read( psaBounds, sizeof( SAFEARRAYBOUND )*cDims );

//dummy safearray - rotate bounds
	SAFEARRAYBOUND	*psaBounds1 = new SAFEARRAYBOUND[cDims];

	for( unsigned i = 0; i < cDims; i++ )
		psaBounds1[i] = psaBounds[cDims-i-1];

	
	varSrc.parray = SafeArrayCreate((unsigned short)vt, cDims, psaBounds1);

	delete []psaBounds;
	delete []psaBounds1;

	void	*pdata = 0;

	SafeArrayAccessData( varSrc.parray, &pdata );

//	ASSERT( pdata );

	ar.Read( pdata, dwSize );

	SafeArrayUnaccessData( varSrc.parray );
}

CArchive& AFXAPI operator<<(CArchive& ar, COleVariant varSrc)
{
	LPCVARIANT pSrc = (LPCVARIANT)varSrc;

	ar << pSrc->vt;

	// support for VT_ARRAY
	if (pSrc->vt & VT_ARRAY)
	{
		StoreSafeArray( ar, *pSrc );
		return ar;
	}

	if (pSrc->vt & VT_BYREF || pSrc->vt & VT_ARRAY)
		return ar;

	switch (pSrc->vt)
	{
	case VT_BOOL:
		return ar << (WORD)V_BOOL(pSrc);

	case VT_UI1:
		return ar << pSrc->bVal;

	case VT_I2:
		return ar << (WORD)pSrc->iVal;

	case VT_I4:
		return ar << pSrc->lVal;

	case VT_CY:
		ar << pSrc->cyVal.Lo;
		return ar << pSrc->cyVal.Hi;

	case VT_R4:
		return ar << pSrc->fltVal;

	case VT_R8:
		return ar << pSrc->dblVal;

	case VT_DATE:
		return ar << pSrc->date;

	case VT_BSTR:
		{
			DWORD nLen = SysStringByteLen(pSrc->bstrVal);
			ar << nLen;
			if (nLen > 0)
				ar.Write(pSrc->bstrVal, nLen * sizeof(BYTE));

			return ar;
		}

	case VT_ERROR:
		return ar << pSrc->scode;

	case VT_DISPATCH:
	case VT_UNKNOWN:
		return ar;

	case VT_EMPTY:
	case VT_NULL:
		// do nothing
		return ar;

	default:
		ASSERT(FALSE);
		return ar;
	}
}

int	g_iCurrentLoadVersion = 2;
//CArchive& AFXAPI operator>>(CArchive& ar, COleVariant& varSrc)
CArchive& AFXAPI _ReadVariant(CArchive& ar, COleVariant& varSrc)
{
	LPVARIANT pSrc = &varSrc;

	// Free up current data if necessary
	if (pSrc->vt != VT_EMPTY)
		VariantClear(pSrc);
	ar >> pSrc->vt;

	// support for VT_ARRAY
	if (pSrc->vt & VT_ARRAY)
	{
		//if( g_iCurrentLoadVersion > 1 )
			LoadSafeArray( ar, *pSrc );
		return ar;
	}

	// No support for VT_BYREF & VT_ARRAY
	if (pSrc->vt & VT_BYREF || pSrc->vt & VT_ARRAY)
		return ar;

	switch (pSrc->vt)
	{
	case VT_BOOL:
		return ar >> (WORD&)V_BOOL(pSrc);

	case VT_UI1:
		return ar >> pSrc->bVal;

	case VT_I2:
		return ar >> (WORD&)pSrc->iVal;

	case VT_I4:
		return ar >> pSrc->lVal;

	case VT_CY:
		ar >> pSrc->cyVal.Lo;
		return ar >> pSrc->cyVal.Hi;

	case VT_R4:
		return ar >> pSrc->fltVal;

	case VT_R8:
		return ar >> pSrc->dblVal;

	case VT_DATE:
		return ar >> pSrc->date;

	case VT_BSTR:
		{
			DWORD nLen;
			ar >> nLen;
			if (nLen > 0)
			{
				pSrc->bstrVal = SysAllocStringByteLen(NULL, nLen);
				if (pSrc->bstrVal == NULL)
					AfxThrowMemoryException();
				ar.Read(pSrc->bstrVal, nLen * sizeof(BYTE));
			}
			else
				pSrc->bstrVal = NULL;

			return ar;
		}
		break;

	case VT_ERROR:
		return ar >> pSrc->scode;

	case VT_DISPATCH:
	case VT_UNKNOWN:
		return ar;
	case VT_EMPTY:
	case VT_NULL:
		// do nothing
		return ar;

	default:
		ASSERT(FALSE);
		AfxThrowArchiveException(CArchiveException::badSchema, NULL);
		return ar;
	}
}

//CArchive& AFXAPI operator>>(CArchive& ar, COleVariant& varSrc)
CArchive& AFXAPI _ReadVariant1(CArchive& ar, COleVariant& varSrc)
{
	LPVARIANT pSrc = &varSrc;

	// Free up current data if necessary
	if (pSrc->vt != VT_EMPTY)
		VariantClear(pSrc);
	ar >> pSrc->vt;

	// No support for VT_BYREF & VT_ARRAY
	if (pSrc->vt & VT_BYREF || pSrc->vt & VT_ARRAY)
		return ar;

	switch (pSrc->vt)
	{
	case VT_BOOL:
		return ar >> (WORD&)V_BOOL(pSrc);

	case VT_UI1:
		return ar >> pSrc->bVal;

	case VT_I2:
		return ar >> (WORD&)pSrc->iVal;

	case VT_I4:
		return ar >> pSrc->lVal;

	case VT_CY:
		ar >> pSrc->cyVal.Lo;
		return ar >> pSrc->cyVal.Hi;

	case VT_R4:
		return ar >> pSrc->fltVal;

	case VT_R8:
		return ar >> pSrc->dblVal;

	case VT_DATE:
		return ar >> pSrc->date;

	case VT_BSTR:
		{
			DWORD nLen;
			ar >> nLen;
			if (nLen > 0)
			{
				pSrc->bstrVal = SysAllocStringByteLen(NULL, nLen);
				if (pSrc->bstrVal == NULL)
					AfxThrowMemoryException();
				ar.Read(pSrc->bstrVal, nLen * sizeof(BYTE));
			}
			else
				pSrc->bstrVal = NULL;

			return ar;
		}
		break;

	case VT_ERROR:
		return ar >> pSrc->scode;

	case VT_DISPATCH:
	case VT_UNKNOWN:
		return ar;

	case VT_EMPTY:
	case VT_NULL:
		// do nothing
		return ar;

	default:
		ASSERT(FALSE);
		AfxThrowArchiveException(CArchiveException::badSchema, NULL);
		return ar;
	}
}


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL	g_bUseSmooth = TRUE;
BOOL	g_bDrawLabels = FALSE;
BOOL	g_bRotateIcons = FALSE;
/////////////////////////////////////////////////////////////////////
//Global functions

IMPLEMENT_SERIAL(CObjMeasure, CObjMove, VERSIONABLE_SCHEMA|3);
IMPLEMENT_SERIAL(CArgList, CObList, 1 );


void inline setColor( C8Image *pimg, 
					   int x1, 
					   int y1, 
					   COLORREF cr )
{
	if( pimg->IsTrueColor() )
	{
		(*pimg)[y1][x1*3]=GetBValue( cr );
		(*pimg)[y1][x1*3+1]=GetGValue( cr );
		(*pimg)[y1][x1*3+2]=GetRValue( cr );
	}
}

double	inline GetLength( double dx, double dy )
{
	return sqrt( dx*dx+dy*dy );
}

void CalcValsXY( double &fLastX, double &fLastY, 
				int x, int y, int dx, int dy )
{
	if( dx==0||dy==0 )
	{
		fLastX = x;
		fLastY = y;
		return;
	}

	ASSERT( abs( dx ) == 1 && 
			abs( dy ) == 1 );

	fLastX = 0.5*dx+y;
	fLastY = 0.5*dy+x;
}

BOOL IsConcave( double a1, double a2 )
{
	double	da = a2-a1;

	if( da < -PI )
		da += 2*PI;
	if( da > PI )
		da -= 2*PI;

	return da > 0;
}


//get segment
int contourGetSegment( CContour *p, int idxCur )
{
	int	iCount = p->GetSize();
	if( idxCur == iCount-1 )
		return -1;

	if( idxCur-1 == iCount-1 )
		return -1;

	int	dx, dy;

	int	idxNext =idxCur + 1;

	dx = (*p)[idxNext].x-(*p)[idxCur].x;
	dy = (*p)[idxNext].y-(*p)[idxCur].y;

	if( ::abs( dx ) == 1 && ::abs( dy ) == 1 )
		return idxCur;

	while( idxNext < iCount-1 )
	{
		idxNext++;

		if( (*p)[idxNext].x-(*p)[idxNext-1].x != dx )
			return idxNext-1;
		if( (*p)[idxNext].y-(*p)[idxNext-1].y != dy )
			return idxNext-1;
	}
	return idxNext;
}

int contourGetLine( CContour *p, int idxCur )
{
	int	iCount = p->GetSize();

	int	idxStep = contourGetSegment( p, idxCur );
	
	if( idxStep == -1 )
		return -1;

	if( idxStep == iCount-1 )
		return idxStep;

	idxStep++;	//pointe to next ponit

	int	dx = (*p)[idxStep].x-(*p)[idxCur].x;
	int	dy = (*p)[idxStep].y-(*p)[idxCur].y;

	while( true )
	{
		int idxStepNext = contourGetSegment( p, idxStep );

		if( idxStepNext == -1 )
			return idxStep;

		if( idxStepNext == iCount-1 )
			return idxStep;

		idxStepNext++;

		if( dx != (*p)[idxStepNext].x-(*p)[idxStep].x )
			return idxStep;
		if( dy != (*p)[idxStepNext].y-(*p)[idxStep].y )
			return idxStep;

		idxStep = idxStepNext;
	}
}


#pragma optimize( "", off )
void MeasurePerimeter( CContour *p, 
					double &fP,	//external perimeter
					double	&fPConcave,	//Concave perimeter
					double	&fPConvex, 
					bool	bOuter,
					C8Image *pimg = 0 )	//Convex perimeter
{
	ASSERT( p );

	if( p->IsEmpty() )
		return;

	if( p->GetSize() <  3 )
	{
		fP += 1;
		fPConcave += 1;
		fPConvex = 0;
	}

	double	x1, y1;	//next point
	double	x2, y2;	//current point

	double	a1, a2;	//two angle
	double	dist;	 //g_ProgLog<<"perimeter ext before"<<fP<<"\n";

	if( g_bUseSmooth )
	{
		p->ApplySmoothFilter( 0, &x2, &y2 );
		p->ApplySmoothFilter( 1, &x1, &y1 );
	}
	else
	{
		x2 = (*p)[0].x;
		y2 = (*p)[0].y;
		x1 = (*p)[1].x;
		y1 = (*p)[1].y;
	}

	a1 = atan2( y1-y2, x1-x2 );
	dist=GetLength( x2-x1, y2-y1 );

	fP+=dist;

	for( DWORD dw = 2; dw< p->GetSize(); dw ++ )
	{
		x2 = x1;
		y2 = y1;
		a2 = a1;				   

		if( g_bUseSmooth )
		{
			p->ApplySmoothFilter( dw, &x1, &y1 );
		}
		else
		{
			x1 = (*p)[dw].x;
			y1 = (*p)[dw].y;
		}
		a1 = atan2( y1-y2, x1-x2 );
		
		dist=GetLength( x2-x1, y2-y1 );
		fP+=dist;
	}

	//measure concave&convex

	double	fAngle1, fAngle2;
	bool	bInit = false;
	CPoint	pt1, pt2, pt3;

	int	idx1, idx2 = 0, idx3;

	int	iCnt = 0;
	int	i;

	while( idx3 !=-1 )
	{
		idx3 = contourGetLine( p, idx2 );

		if( idx3 == -1 )
			break;

		pt2 = (*p)[idx2];
		pt3 = (*p)[idx3];

		fAngle2 = atan2( double(pt3.y-pt2.y), double(pt3.x-pt2.x) );

		if( bInit )
		{
			double	fDelta = fAngle2 - fAngle1;

			if( fDelta > PI )
				fDelta-=2*PI;
			if( fDelta < -PI )
				fDelta+=2*PI;

			int	iStart, iEnd;

			iStart = (idx1+idx2)/2;
			iEnd = (idx2+idx3)/2;

			bool	bCalcCoords = false;
			CPoint	ptOld, pt;

			for( i = iStart; i<iEnd; i++ )
			{
				pt = (*p)[i];

				if( bCalcCoords )
				{
					dist=GetLength( pt.x-ptOld.x, pt.y-ptOld.y );
					if( fDelta < 0 )
						fPConcave+=dist;
					else
						fPConvex+=dist;
				}
				ptOld = pt;
				bCalcCoords = true;
			}
		}

		bInit = true;

		fAngle1 = fAngle2;
		pt1 = pt2;
		pt2 = pt3;
		idx1 = idx2;
		idx2 = idx3;
		iCnt++;
	}

	if( bInit )
	{
		idx3 = contourGetLine( p, 0 );

		pt2 = (*p)[idx2];
		pt3 = (*p)[idx3];

		fAngle2 = atan2( double(pt3.y-pt2.y), double(pt3.x-pt2.x) );

		if( bInit )
		{
			double	fDelta = fAngle2 - fAngle1;

			if( fDelta > PI )
				fDelta-=2*PI;
			if( fDelta < -PI )
				fDelta+=2*PI;

			int	iStart, iEnd;

			iStart = (idx1+idx2)/2;
			iEnd = (idx2+idx3)/2;
			
			bool	bCalcCoords = false;
			CPoint	ptOld, pt;

			for( i = iStart; i<(int)p->GetSize(); i++ )
			{
				pt = (*p)[i];

				if( bCalcCoords )
				{
					dist=GetLength( pt.x-ptOld.x, pt.y-ptOld.y );
					if( fDelta < 0 )
						fPConcave+=dist;
					else
						fPConvex+=dist;
				}
				ptOld = pt;
				bCalcCoords = true;
			}

			for( i = 0; i<iEnd; i++ )
			{
				pt = (*p)[i];

				if( bCalcCoords )
				{
					dist=GetLength( pt.x-ptOld.x, pt.y-ptOld.y );
					if( fDelta < 0 )
						fPConcave+=dist;
					else
						fPConvex+=dist;
				}
				ptOld = pt;
				bCalcCoords = true;
			}
		}
	}



}
#pragma optimize( "", on )

void CreateMeaseImage( C8Image &imgSrc, C8Image &img, CRect rc )
{
	BYTE	byte;
	int		x, y;

	for( y = rc.top; y<rc.bottom; y++ )
	{
		for( x = rc.left; x<rc.right; x++ )
		{
			byte = imgSrc[y][x];

			ASSERT( byte != BYTE_EXTMARKER );
			ASSERT( byte != BYTE_INTMARKER );
			
			if( byte == BYTE_FILLEDBODY )
			{
				imgSrc[y][x] = 0;
			}
			if( byte == BYTE_INTRNSPACE )
			{
				imgSrc[y][x] = 0;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////
// CObjMeasure methods
//Конструктор и 
BOOL CObjMeasure::s_bDrawSmoothed = TRUE;
BOOL CObjMeasure::s_bDrawClass = FALSE;
BOOL CObjMeasure::s_bDrawUnknown = TRUE;


CObjMeasure::CObjMeasure()
	:CObjMove()
{
	m_fAngle = 0;
	m_fArea = 0;
	m_pStoreDoc = 0;
	m_parr = NULL;
}

double CObjMeasure::GetAngle()
{
	return m_fAngle;
}

//destructor
CObjMeasure::~CObjMeasure()
{
	DeInit();
}

CObjNative *CObjMeasure::Clone()
{
	CObjMeasure *pNew = (CObjMeasure *)CObjMove::Clone();
	ASSERT_KINDOF(CObjMeasure,pNew);
	pNew->AttachDataTable(*this);
	return pNew;
}

void CObjMeasure::DeInit()
{
	CObjMove::DeInit();
	m_ArgList.RemoveAll();

	EmptyMap();
}

void CObjMeasure::DoFree()
{
	/*while ( !m_ArgList.IsEmpty() )
	{
		CArg* p = m_ArgList.RemoveHead();
		delete p;
	}*/

	POSITION pos = m_ArgList.GetHeadPosition();

	while( pos )
	{
		CArg *pArg = m_ArgList.GetNext( pos );
		delete pArg;
	}

	m_ArgList.RemoveAll();

	EmptyMap();

	CObjMove::DoFree();
}

void CObjMeasure::DoPaint( CDC &theDC, double iZoom, int iType, BOOL bActive )
{
	CObjMove::DoPaint( theDC, iZoom, iType, bActive );
}


//Отрисовка на изображении
void CObjMeasure::PaintFrame( CDC &theDC, double iZoom, BOOL bActive )
{
	ASSERT(FALSE); // not supported here
}


inline CArg *CObjMeasure::GetArg( WORD nKey )
{
	POSITION pos = GetFirstArgPosition();
	
	while( pos )
	{
		CArg *pArg = GetNextArg( pos );
		if( pArg->GetKey() == nKey )
			return pArg;
	}
	return 0;
}

inline void CObjMeasure::SetArg( WORD wKey, double f )
{
	CArg *pArg = GetArg( wKey );
	if( !pArg )
	{
		pArg = new CArg();
		pArg->SetKey( wKey );
		AddArg( pArg );
	}

	pArg->SetValue( COleVariant( f ) );
}

inline void CObjMeasure::SetArg( CTypeInfo *pInfo, double f )
{
	if (!pInfo || !pInfo->IsEnable() ) return;
	CArg *pArg = GetArg( pInfo->GetKey() );
	if( !pArg )
	{
		pArg = new CArg();
		pArg->SetKey( pInfo->GetKey() );
		AddArg( pArg );
	}
	pArg->SetValue( COleVariant( f ) );
}

extern int s_nAvrBack;

void CObjMeasure::DoMeasure()
{
	ASSERT(FALSE); // not supported here
}

void CObjMeasure::SetValueToDocument( UINT iddStr, double fVal )
{
	ASSERT(FALSE); // not supported here
}


void CObjMeasure::Serialize( CArchive &ar )
{
	CObjMove::Serialize( ar );

	if( ar.IsStoring() )
		ar << m_fArea << m_fAngle;
	else
	{
		ar >> m_fArea >> m_fAngle;
	}

	//g_iCurrentLoadVersion = ar.GetObjectSchema();
	m_ArgList.Serialize( ar );
	SerializeMap( ar );
}

/*void CObjMeasure::DoPaintImage( CDC &theDC, CImageBase *pimg, CRect rcPaint )
{
	CRect	rcClip = m_rcClip;

	double	fZoomByX, fZoomByY, fZoom;

	fZoomByX  = (double)rcPaint.Width()/rcClip.Width();
	fZoomByY  = (double)rcPaint.Height()/rcClip.Height();

	CPoint ptCenter( (rcPaint.left+rcPaint.right)/2,
					(rcPaint.top+rcPaint.bottom)/2 );

	if( fZoomByX < fZoomByY )
	{
		fZoom = fZoomByX;
		int	iHeight = int( rcPaint.Height()/fZoomByY*fZoomByX );
		rcPaint.top = ptCenter.y-iHeight/2;
		rcPaint.bottom = ptCenter.y+iHeight/2;
	}
	else
	{
		fZoom = fZoomByY;
		int	iWidth = int( rcPaint.Width()/fZoomByX*fZoomByY );
		rcPaint.left = ptCenter.x-iWidth/2;
		rcPaint.right = ptCenter.x+iWidth/2;
	}

	C8Image	img;
	img.Create( rcPaint.Width(), rcPaint.Height(), pimg->IsTrueColor() );

//make white and resize
	int	x, y, xm, ym, incr = img.GetIncr(), xs, ys;
	BYTE	*psrc, *pmask, *pdest;

	for( y = 0; y<img.GetCY(); y++ )
	{
		ym = int(y/fZoom);
		ys = ym+rcClip.top;

		if( ys >= rcClip.bottom )
			continue;

		pmask = m_pimg?(*m_pimg)[ym]:0;
		psrc = (*(C8Image*)pimg)[ys];
		pdest = img[y];

		for( x = 0; x<img.GetCX()*incr; x+=incr )
		{
			xm = int(x/fZoom/incr);
			xs = xm+rcClip.left;

			if( rcClip.PtInRect( CPoint( xs, ys ) )
				&&pmask&&pmask[xm] )
			{
				if( incr == 3 )
				{
					pdest[x]=psrc[xs*3];
					pdest[x+1]=psrc[xs*3+1];
					pdest[x+2]=psrc[xs*3+2];
				}
				else
				{
`					pdest[x]=psrc[xs];
				}
			}
			else
			{
				if( incr == 3 )
				{
					pdest[x]=255;
					pdest[x+1]=255;
					pdest[x+2]=255;
				}
				else
				{
					pdest[x]=255;
				}
			}
		}
	}

	rcClip=CRect( 0, 0, rcPaint.Width(), rcPaint.Height() );

	img.Draw( &theDC, rcClip, rcPaint, 0, TRUE );
}*/

//#define DELETE_VAL 

void CObjMeasure::AttachData( CObjNative &obj )
{
	CObjMove::AttachData( obj );
	if( obj.IsKindOf( RUNTIME_CLASS(CObjMeasure) ) )
	{
		m_fArea = ((CObjMeasure&)obj).m_fArea;
		m_fAngle = ((CObjMeasure&)obj).m_fAngle;
		POSITION	pos = ((CObjMeasure&)obj).GetFirstArgPosition();

		while( pos )
			m_ArgList.AddTail( ((CObjMeasure&)obj).GetNextArg( pos ) );

		pos = ((CObjMeasure&)obj).m_map.GetStartPosition();

		while( pos )
		{
			CString	sKey;
			COleVariant	*pvar;

			((CObjMeasure&)obj).m_map.GetNextAssoc( pos, sKey, (void *&)pvar );

			COleVariant	*pnewvar = new COleVariant( *pvar );
			m_map.SetAt( sKey, pnewvar );
#if defined(DELETE_VAL)
			delete pvar;
#endif
		}
	}
}

void CObjMeasure::Attach( CObjNative &obj )
{
	CObjMove::Attach( obj );
	
	if( obj.IsKindOf( RUNTIME_CLASS(CObjMeasure) ) )
	{
		POSITION	pos = ((CObjMeasure&)obj).GetFirstArgPosition();

		m_fArea = ((CObjMeasure&)obj).m_fArea;
		m_fAngle = ((CObjMeasure&)obj).m_fAngle;

		while( pos )
		{
			CArg *pArg = ((CObjMeasure&)obj).GetNextArg( pos );
			CArg *pArg2 = new CArg(*pArg);
			m_ArgList.AddTail( pArg2 );
		}

		pos = ((CObjMeasure&)obj).m_map.GetStartPosition();

		while( pos )
		{
			CString	sKey;
			COleVariant	*pvar;

			((CObjMeasure&)obj).m_map.GetNextAssoc( pos, sKey, (void *&)pvar );

			COleVariant	*pnewvar = new COleVariant( *pvar );
			m_map.SetAt( sKey, pnewvar );
#if defined(DELETE_VAL)
			delete pvar;
#endif
		}
	}
}

void CObjMeasure::DoAllocCopy()
{
	CObjMove::DoAllocCopy();
	POSITION	pos = GetFirstArgPosition();
	while( pos )
	{
		POSITION posPrev = pos;
		CArg *pArg = GetNextArg( pos );
		CArg *pArg2 = new CArg(*pArg);
		m_ArgList.SetAt(posPrev, pArg2);
	}
}

void CObjMeasure::AttachDataTable( CObjMeasure &obj )
{
	POSITION	pos = obj.GetFirstArgPosition();

	m_fArea = ((CObjMeasure&)obj).m_fArea;

	while( pos )
		m_ArgList.AddTail( new CArg( *obj.GetNextArg( pos ) ) );

	pos = ((CObjMeasure&)obj).m_map.GetStartPosition();

	while( pos )
	{
		CString	sKey;
		COleVariant	*pvar;

		((CObjMeasure&)obj).m_map.GetNextAssoc( pos, sKey, (void *&)pvar );

		COleVariant	*pnewvar = new COleVariant( *pvar );
		m_map.SetAt( sKey, pnewvar );
#if defined(DELETE_VAL)
			delete pvar;
#endif
	}

	m_iClassNo = obj.m_iClassNo;
	m_iCode = obj.m_iCode;
}


int CObjMeasure::GetClass()
{
	return m_iClassNo;
}

void CObjMeasure::SetValue( WORD wKey, double fValue )
{
	ASSERT(FALSE); // not supported here
}

COleVariant CObjMeasure::GetValue( WORD wKey )
{
	COleVariant	var;
	CArg *parg = GetArg( wKey );
	if( parg )
		var = parg->GetValue();
	return var;
}

void CObjMeasure::BeginMeasure( CDocument *pdoc, CObjArray *parr )
{	
	ASSERT(FALSE); // not supported here
}


void CObjMeasure::GetCurrentCalibr( double &fByX, double &fByY )
{
	ASSERT(FALSE); // not supported here
}

//some global support
HRESULT CObjMeasure::GetParamCount( int *picount )
{
	*picount = GetArgsCount();
	return S_OK;
}

HRESULT CObjMeasure::GetParamKey( int ipos, WORD *pwKey )
{
	*pwKey = NOKEY;

	POSITION pos=  GetFirstArgPosition();

	CArg	*parg = 0;

	while( pos )
	{
		parg = GetNextArg( pos );

		if( !ipos )
		{
			*pwKey = parg->GetKey();
			break;
		}
		ipos--;
	}

		
	return S_OK;
}

HRESULT CObjMeasure::GetParamName( int ipos, BSTR *pbstr )
{
	*pbstr = 0;

	CString sName = _T("Unknown param");

	WORD wKey = NOKEY;

	POSITION pos=  GetFirstArgPosition();

	CArg	*parg = 0;

	while( pos )
	{
		parg = GetNextArg( pos );

		if( !ipos )
		{
			wKey = parg->GetKey();
			break;
		}
		ipos--;
	}

	if( wKey == NOKEY )
		return S_OK;

	CTypeInfo	*ptype = g_TypesManager.Find( wKey );

	if( !ptype )
		return S_OK;

	sName = ptype->GetLongName();

	*pbstr = sName.AllocSysString();
		
	return S_OK;
}

HRESULT CObjMeasure::IsParamEnabled( WORD wKey )
{
	CTypeInfo	*ptype = g_TypesManager.Find( wKey );

	if( !ptype )
		return S_FALSE;
	else
		return ptype->IsEnable()?S_OK:S_FALSE;
}

HRESULT CObjMeasure::SetUserData( BSTR lpcszName, LPCVARIANT pcvarData )
{
	CString	s( lpcszName );

	COleVariant	*pvar = NULL;
	m_map.Lookup( s, (void *&)pvar );
	m_map.SetAt( s, new COleVariant( pcvarData ) );
	delete pvar;

	return S_OK;
}

HRESULT CObjMeasure::GetUserData( BSTR lpcszName, LPVARIANT pvarData )
{
	VariantClear( pvarData );

	COleVariant	*pvar = 0;
	CString	s( lpcszName );

	if( !m_map.Lookup( s, (void *&)pvar ) )
		return S_OK;

	VariantCopy( pvarData, pvar );
	
	return S_OK;
}


HRESULT CObjMeasure::SetClass( int iKey, BOOL bDirect )
{ 
	m_iClassNo = iKey; 
	m_bDirectUnknown = bDirect; 

	return S_OK;
}


void CObjMeasure::EmptyMap()
{
	POSITION pos = m_map.GetStartPosition();

	while( pos )
	{
		CString	sKey;
		COleVariant	*pvar;

		m_map.GetNextAssoc( pos, sKey, (void *&)pvar );

		m_map.SetAt( sKey, 0 );

		delete pvar;
	}
	m_map.RemoveAll();
}

void CObjMeasure::SerializeMap( CArchive &ar )
{
	if( ar.IsStoring() )
	{
		POSITION pos = m_map.GetStartPosition();

		while( pos )
		{
			CString	sKey;
			COleVariant	*pvar;

			m_map.GetNextAssoc( pos, sKey, (void *&)pvar );

			ar<<sKey;
			ar<<*pvar;

		}
		ar<<CString("~End");
	}
	else
	{
		CString	s;
		ar>>s;

		while( s != "~End" )
		{
			COleVariant	*pvar = new COleVariant();

			if (g_iCurrentLoadVersion == 0)
				_ReadVariant1(ar, *pvar);
			else
				_ReadVariant(ar, *pvar);

			m_map.SetAt( s, pvar );

			ar>>s;
		}
	}
}