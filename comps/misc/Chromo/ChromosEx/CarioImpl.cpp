#include "stdafx.h"
#include "carioimpl.h"
#include "resource.h"
#include "editorint.h"
#include "addidiobend.h"

#include "chromo_utils.h"
#include "thumbnail.h"

#include "class_utils.h"
#include "axint.h"
#include "math.h"
#include "\vt5\awin\misc_list.h"
#include "cgh_int.h"

#include "\vt5\awin\profiler.h"
#include "\vt5\awin\misc_drawex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AROUND_BORDER_SIZE	10
#define CROMO_OFFSET		0
#define CROMO_OFFSET_X		10
#define CENTER_LINE_SIZE	10
#define CAPTION_ALIGN		2
#define CGHAMPL_OFFSET_X	2

#define CGHAMPL_RATIO_TEXT_CY 10

#define CROMO_POS_DX		1
#define INNER_INT			5

#define MIN_CELL_WIDTH		100
#define MIN_LINE_HEIGHT		80

long m_nIdioWidth = 25, m_nIdioHeight = 120;

RECT GetDrawingRect( IUnknownPtr drawobjects, IUnknown *punkExcludedObject = 0 /*[vanek] - 13.11.2003*/ )
{
	RECT rc = {0};
	INamedDataObject2Ptr ptrNDO = drawobjects;
	if( ptrNDO == 0 )
		return rc;

	long lpos = 0;
	ptrNDO->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown* punk = 0;
		ptrNDO->GetNextChild( &lpos, &punk );

		// [vanek] - 13.11.2003
        BOOL bIsExcludedObject = punk == punkExcludedObject;

		IRectObjectPtr	ptrObject( punk );
		punk->Release();	punk = 0;

		if( ptrObject == 0 || bIsExcludedObject )
			continue;

		RECT rc2 = {0};

		if(CheckInterface(ptrObject, IID_ICommentObj))
		{
			CRect rc(NORECT);
			sptrICommentObj sptrComm(ptrObject);
			if(sptrComm)
			{
				CRect	rcText = NORECT;
				CPoint	ptArr = CPoint(0,0);
				sptrComm->GetCoordinates(&rcText, &ptArr );
				if(ptArr == CPoint(-1,-1))
					ptArr = rcText.TopLeft();
				CRect rcArr(ptArr.x-1, ptArr.y-1, ptArr.x+1, ptArr.y+1);
				rcArr.NormalizeRect();
				CRect rc;
				rc.UnionRect(&rcText, &rcArr);
				rc.NormalizeRect();
				rc.InflateRect( 5, 5, 5, 5 );
				rc.right += 3;
				rc.bottom += 3;
				rc2 = rc;
			}
		}
		else
			ptrObject->GetRect( &rc2 );

		::UnionRect( &rc, &rc, &rc2 );

	}
	return rc;
}

inline void _find_end_points( IImage3Ptr ptrImage, FPoint &pt1, FPoint &pt2, int nPaneID )
{
	int cxI = 0, cyI = 0;
	ptrImage->GetSize( &cxI, &cyI );

	int nCount = 0;
	bool bStart = false, bEnd = false;

	double nSteps = 1;

	FPoint _pt = pt1;

	double dx = pt2.x - pt1.x;
	double dy = pt2.y - pt1.y;

	double len = _hypot( dx, dy );

	if( !len )
		len = 1;
	dx /= len;
	dy /= len;

	for( int i = 1; ; i++ )
	{
		double	fx = _pt.x + ( dx ) * i;
		double	fy = _pt.y + ( dy ) * i;

		double nx = fx, ny = fy;

		if( int( nx + 0.5 ) >= 0 && int( nx + 0.5 ) < cxI && int(ny + 0.5) >= 0 && int(ny + 0.5) < cyI )
		{
			BYTE *byte = 0;
			ptrImage->GetRowMask( int(ny + 0.5), &byte );

			if( byte[ int( nx + 0.5 ) ] < 128  )
			{
				pt1 = FPoint( nx, ny );
				break;
			}
		}
		else
		{
			pt1 = FPoint( nx, ny );
			break;
		}
	}

	_pt = pt2;

	for( int i = 1; ; i++ )
	{
		double	fx = _pt.x - ( dx ) * i;
		double	fy = _pt.y - ( dy ) * i;

		double nx = fx, ny = fy;

		if( int(nx + 0.5) >= 0 && int(nx + 0.5) < cxI && int( ny + 0.5 ) >= 0 && int( ny + 0.5 ) < cyI )
		{
			BYTE *byte = 0;
			ptrImage->GetRowMask( int( ny + 0.5 ), &byte );

			if( byte[ int(nx + 0.5) ] < 128 )
			{
				pt2 = FPoint( nx, ny );
				break;
			}
		}
		else
		{
			pt2 = FPoint( nx, ny );
			break;
		}
	}

}

/////////////////////////////////////////////////////////////////////////////
//	class CCarioFrame
double student( double p, int n )
{

	double P[10] = { 0.1, 0.3, 0.5, 0.7, 0.8, 0.9, 0.95, 0.98, 0.99, 0.995 };

	double Tp[21][10] = { 
		/*2	 */ 0.16, 0.51, 1.00, 2.0, 3.1, 6.3, 12.7, 31.8, 63.7, 636.7 ,
		/*3	 */ 0.14, 0.45, 0.82, 1.3, 1.9, 2.9, 4.3,  7.0,  9.9,  31.6  ,
		/*4	 */ 0.14, 0.42, 0.77, 1.3, 1.0, 2.4, 3.2,  4.5,  5.8,  12.9 ,
		/*5	 */ 0.13, 0.41, 0.74, 1.2, 1.5, 2.1, 2.8,  3.7,  4.6,  8.6 ,
		/*6	 */ 0.13, 0.41, 0.73, 1.2, 1.5, 2.0, 2.6,  3.4,  4.0,  6.9 ,
		/*7	 */ 0.13, 0.40, 0.72, 1.1, 1.4, 1.9, 2.4,  3.1,  3.7,  6.0 ,
		/*8	 */ 0.13, 0.40, 0.71, 1.1, 1.4, 1.9, 2.4,  3.0,  3.5,  5.4 ,
		/*9	 */ 0.13, 0.40, 0.71, 1.1, 1.4, 1.9, 2.3,  2.9,  3.4,  5.0 ,
		/*10 */ 0.13, 0.40, 0.70, 1.1, 1.4, 1.8, 2.3,  2.8,  3.3,  4.8 ,
		/*11 */ 0.13, 0.40, 0.70, 1.1, 1.4, 1.8, 2.2,  2.8,  3.2,  4.6 ,
		/*12 */ 0.13, 0.40, 0.70, 1.1, 1.4, 1.8, 2.2,  2.7,  3.1,  4.5 ,
		/*13 */ 0.13, 0.40, 0.70, 1.1, 1.4, 1.8, 2.2,  2.7,  3.1,  4.3 ,
		/*14 */ 0.13, 0.39, 0.69, 1.1, 1.4, 1.8, 2.2,  2.7,  3.0,  4.2 ,
		/*15 */ 0.13, 0.39, 0.69, 1.1, 1.3, 1.8, 2.1,  2.6,  3.0,  4.1 ,
		/*16 */ 0.13, 0.39, 0.69, 1.1, 1.3, 1.8, 2.1,  2.6,  2.9,  4.0 ,
		/*17 */ 0.13, 0.39, 0.69, 1.1, 1.3, 1.7, 2.1,  2.6,  2.9,  4.0 ,
		/*18 */ 0.13, 0.39, 0.69, 1.1, 1.3, 1.7, 2.1,  2.6,  2.9,  4.0 ,
		/*19 */ 0.13, 0.39, 0.69, 1.1, 1.3, 1.7, 2.1,  2.6,  2.9,  3.9 ,
		/*20 */ 0.13, 0.39, 0.69, 1.1, 1.3, 1.7, 2.1,  2.6,  2.9,  3.9 ,
		/*25 */ 0.13, 0.39, 0.69, 1.1, 1.3, 1.7, 2.1,  2.5,  2.8,  3.7 ,
		/*30 */ 0.13, 0.39, 0.68, 1.1, 1.3, 1.7, 2.0,  2.5,  2.8,  3.7 
	};
	
	if( p > 0.995 || p < 0.1 )
		return 0;

	int nCol = 0;
	for( int i = 1 ; i < 10; i++ )
	{
		if( P[i] >= p )
		{
//			if( P[i] - p < p - P[i-1] )
				nCol = i;
//			else
//				nCol = i - 1;
			break;
		}
	}

	if( n <= 1 )
		return 0;
	
	if( n >= 20 && n < 25 )
		return (Tp[19][nCol] - Tp[18][nCol]) / 5 * ( n - 20 ) + Tp[18][nCol];
	if( n >= 25 && n < 30 )
		return (Tp[20][nCol] - Tp[19][nCol]) / 5 * ( n - 25 ) + Tp[19][nCol];
	if( n >= 30 )
		return Tp[20][nCol];

	return Tp[n - 2][nCol];
}



double CanonicY(double x1, double y1,double x2, double y2,double x )
{
	if(x1 != x2 )
	{
		double a = ( y2 - y1 ) / ( x2 - x1 );
		return a * ( x - x1 ) + y1;
	}
	return 0;
} 

bool CCarioFrame::SerializeObject( CStreamEx& ar, SerializeParams *pparams )
{
//	return CFrame::SerializeObject( ar, pparams );

	if(ar.IsStoring())
	{
		return CFrame::SerializeObject( ar, pparams );
		EmptyFrame();		

		long nObjectCount = GetObjectsCount();

		ar<<nObjectCount;
		ar<<m_rcBounds.left;
		ar<<m_rcBounds.top;
		ar<<m_rcBounds.right;
		ar<<m_rcBounds.bottom;
		
		ar.Flush();
/*		IUnknown* punkDataObject = 0;
		CString	strObjectType = "";

		for(int i = 0; i < nObjectCount; i++)		
		{
			ar<<m_arrObjectsOffsets[i];
			punkDataObject = GetObjectByIdx(i);
			strObjectType = ::GetObjectKind( punkDataObject );
			ar<<strObjectType;
			punkDataObject->Release();
			punkDataObject = 0;
		}

		ar.Flush();

		//CArchiveStream	stream( &ar );

		for(i = 0; i < nObjectCount; i++)
		{
			punkDataObject = GetObjectByIdx(i);
			sptrINamedDataObject2	sptrN( punkDataObject );
			DWORD	dwFlags = 0;
			sptrN->GetObjectFlags( &dwFlags );
			bool bVirtual  = ( dwFlags & nofHasData ) == 0;
			//if( bVirtual )
			//	sptrN->StoreData( sdfCopyParentData );

			IUnknown	*punkParentObject = 0;
			sptrN->GetParent( &punkParentObject );

			BOOL	bParentLock;

			if( CheckInterface( punkParentObject, IID_IDataObjectList ) )
			{
				IDataObjectListPtr	ptrP( punkParentObject );
				ptrP->GetObjectUpdateLock( &bParentLock );
				ptrP->LockObjectUpdate( true );
			}

			if( bVirtual )
				sptrN->SetParent( 0, sdfCopyParentData );
			else
				sptrN->SetParent( 0, apfNone );

			sptrISerializableObject	sptrS( punkDataObject );
			sptrS->Store( (IStream*)ar, pparams );

			if( bVirtual )
				sptrN->SetParent( punkParentObject, apfAttachParentData );
			else
				sptrN->SetParent( punkParentObject, apfNone );
			if( punkParentObject )
				punkParentObject->Release();

			if( CheckInterface( punkParentObject, IID_IDataObjectList ) )
			{
				IDataObjectListPtr	ptrP( punkParentObject );
				ptrP->LockObjectUpdate( bParentLock );
			}

			//if( bVirtual )
			//	sptrN->StoreData( apfAttachParentData );

			punkDataObject->Release();
			punkDataObject = 0;
		}
		ar.Flush();*/
	}
	else
	{
		EmptyFrame();

		m_arSrcGuids.SetSize( 0 );

		int nObjectCount = 0;
		ar>>nObjectCount;
		ar>>m_rcBounds.left;
		ar>>m_rcBounds.top;
		ar>>m_rcBounds.right;
		ar>>m_rcBounds.bottom;
		m_arrObjectsOffsets.SetSize(nObjectCount);
		CStringArray strarrTypes;
		strarrTypes.SetSize(nObjectCount);
		for(int i = 0; i < nObjectCount; i++)		
		{
			ar>>m_arrObjectsOffsets[i];
			ar>>strarrTypes[i];
		}

		ar.Flush();
		//CArchiveStream	stream( &ar );
		for(i = 0; i < nObjectCount; i++)		
		{
			IUnknown	*punkO = ::CreateTypedObject( strarrTypes[i] );
			if( !punkO )return false;
			CString	strOldName = ::GetObjectName( punkO );
			{
				sptrISerializableObject	sptrS( punkO );
				sptrS->Load( (IStream*)ar, pparams );

				//восстановим GUID здесь

				if( CheckInterface( punkO, IID_IMeasureObject ) )
					m_arSrcGuids.Add( ::GetObjectKey( punkO ) );
				//::SetObjectName( punkO, 0 );
				INumeredObjectPtr	sptrN( punkO );
				sptrN->GenerateNewKey( 0 );
			}
			//punkO->AddRef();
			m_ptrSelectedObjects.Add( punkO );
			GuidKey lkey = ::GetObjectKey( punkO );
			m_ptrSelectedObjectsKeys.Add( lkey );
		}
	}

	//EmptyFrame();
	return true;
}


IMPLEMENT_DYNCREATE(CCarioFrame, CFrame)

/////////////////////////////////////////////////////////////////////////////
//	class chromo_object
chromo_object::chromo_object()
{
	m_pstrRatioTextUP = 0;
	init_defaults( 0 );	
}

/////////////////////////////////////////////////////////////////////////////
chromo_object::~chromo_object()
{
	if( m_pstrRatioTextUP )
		delete []m_pstrRatioTextUP;

	if( m_strAddLines )
		delete []m_strAddLines;

	if( m_strAddLinesFull)
		delete []m_strAddLinesFull;

	m_pstrRatioTextUP = 0;
}

/////////////////////////////////////////////////////////////////////////////
void chromo_object::init_defaults( CCarioImpl *pImpl )
{
	m_guidMeasureObject = INVALID_KEY;
	m_bErected = false;

	m_rectIdio			=
	m_rectChromo		=
	m_rectImage			=
	m_rectCGHGraph		= NORECT;
	m_rectCGHRatio		= NORECT;
	m_rectEditableIdio = NORECT;

	m_sizeTotal			= CSize( 0, 0 );
	m_nCentromereY		= 0;
	m_fLength = 1;
	
	m_ptOffset			= 0;
	
	m_sizeIdio			= CSize( 0, 0 );
	m_sizeEditableIdio  = CSize( 0, 0 );

	m_bShowIdiogramm	= true;


	m_bHasCGHAmplL		= false;
	m_bHasCGHAmplR		= false;

	m_fZoom				= 1.0;
	m_nYOffset			= 0;


	m_strAddLines = 0;
	m_strAddLinesFull = 0;
	m_nLinesCount = 0;

	if( m_pstrRatioTextUP )
		delete []m_pstrRatioTextUP;
	m_pstrRatioTextUP = 0;
}

/////////////////////////////////////////////////////////////////////////////
void chromo_object::deinit(CCarioImpl *pImpl)
{
	init_defaults( pImpl );
}

/////////////////////////////////////////////////////////////////////////////
void chromo_object::load( CStreamEx& stream )
{
	deinit( 0 );

	long lVersion = 4;		
	stream >> lVersion;
	stream.Read( &m_guidMeasureObject, sizeof( GUID ) );
	if( lVersion >= 2 )
		stream.Read( &m_nYOffset, sizeof( m_nYOffset ) );
	if( lVersion >= 3 )
		stream.Read( &m_fZoom, sizeof( m_fZoom ) );
	if( lVersion >= 4 )
		stream >> m_strTextInfo;
}

/////////////////////////////////////////////////////////////////////////////
void chromo_object::save( CStreamEx& stream )
{
	long lVersion = 4;
	stream << lVersion;
	stream.Write( &m_guidMeasureObject, sizeof( GUID ) );
	stream.Write( &m_nYOffset, sizeof( m_nYOffset ) );
	stream.Write( &m_fZoom, sizeof( m_fZoom ) );

	if( !m_strTextInfo.IsEmpty() )
		stream << m_strTextInfo;
}

/////////////////////////////////////////////////////////////////////////////
void chromo_object::save_to_named_data( INamedData* pINamedData, CString strSection )
{
	if( !pINamedData )
		return;

	pINamedData->SetupSection( _bstr_t( strSection ) );				

	BSTR bstr = 0;
	StringFromCLSID( m_guidMeasureObject, &bstr );
	pINamedData->SetValue( _bstr_t( CARIO_GUID_CHROMO_PREFIX ), _variant_t( bstr ) );
	::CoTaskMemFree( bstr );	bstr = 0;
	pINamedData->SetValue( _bstr_t( CARIO_YOFS_OBJECT ), _variant_t( m_nYOffset ) );
	pINamedData->SetValue( _bstr_t( CARIO_ZOOM_OBJECT ), _variant_t( m_fZoom ) );
//	pINamedData->SetValue( _bstr_t( CARIO_SHOW_OBJECT ), _variant_t( (long)m_bShowChromosome ) );
	pINamedData->SetValue( _bstr_t( CARIO_SHOW_IDIO ), _variant_t( (long)m_bShowIdiogramm ) );
//	pINamedData->SetValue( _bstr_t( CARIO_SHOW_CGHBRIGHT ), _variant_t( (long)m_bShowCGHGraph ) );
//	pINamedData->SetValue( _bstr_t( CARIO_SHOW_CGHRATIO ), _variant_t( (long)m_bShowCGHRatio ) );
//	pINamedData->SetValue( _bstr_t( CARIO_SHOW_CGHAMPL ), _variant_t( (long)m_bShowCGHAmpl ) );

	pINamedData->SetValue( _bstr_t( "ObjectText" ), _variant_t( m_strTextInfo ) );
	pINamedData->SetValue( _bstr_t( "ObjectErected" ), _variant_t( long( m_bErected ) ) );
//	pINamedData->SetValue( _bstr_t( CARIO_ADD_LINES ), _variant_t( long( m_bShowAdditionLines ) ) );
//	pINamedData->SetValue( _bstr_t( CARIO_EDITABLE_IDIO ), _variant_t( long( m_bShowEditableIdio ) ) );
}

/////////////////////////////////////////////////////////////////////////////
void chromo_object::load_from_named_data( INamedData* pINamedData, CString strSection, CCarioImpl *pImpl )
{
	deinit( pImpl );

	if( !pINamedData )
		return;

	pINamedData->SetupSection( _bstr_t( strSection ) );
	
	_variant_t var;

	pINamedData->GetValue( _bstr_t( CARIO_GUID_CHROMO_PREFIX ), &var );
	if( var.vt == VT_BSTR )
		CLSIDFromString( var.bstrVal, &m_guidMeasureObject );

	pINamedData->GetValue( _bstr_t( CARIO_YOFS_OBJECT ), &var );
	if( var.vt == VT_I4 ) m_nYOffset = var.lVal;
	pINamedData->GetValue( _bstr_t( CARIO_ZOOM_OBJECT ), &var );
	if( var.vt == VT_R8 ) m_fZoom = var.dblVal;

//	var.Clear();
//	pINamedData->GetValue( _bstr_t( CARIO_SHOW_OBJECT ), &var );
//	if( var.vt == VT_I4 ) m_bShowChromosome = var.lVal != 0;

	var.Clear();
	pINamedData->GetValue( _bstr_t( CARIO_SHOW_IDIO ), &var );
	if( var.vt == VT_I4 ) m_bShowIdiogramm = var.lVal != 0;

//	var.Clear();
//	pINamedData->GetValue( _bstr_t( CARIO_SHOW_CGHBRIGHT ), &var );
//	if( var.vt == VT_I4 ) m_bShowCGHGraph = var.lVal != 0;

//	var.Clear();
//	pINamedData->GetValue( _bstr_t( CARIO_SHOW_CGHRATIO ), &var );
//	if( var.vt == VT_I4 ) m_bShowCGHRatio = var.lVal != 0;

//	var.Clear();
//	pINamedData->GetValue( _bstr_t( CARIO_SHOW_CGHAMPL ), &var );
//	if( var.vt == VT_I4 ) m_bShowCGHAmpl = var.lVal != 0;

	var.Clear();
	pINamedData->GetValue( _bstr_t( "ObjectText" ), &var );
	if( var.vt == VT_BSTR ) m_strTextInfo = (char*)_bstr_t( var.bstrVal );

	var.Clear();
	pINamedData->GetValue( _bstr_t( "ObjectErected" ), &var );
	if( var.vt == VT_I4 ) m_bErected = var.lVal != 0;

//	var.Clear();
//	pINamedData->GetValue( _bstr_t( CARIO_ADD_LINES ), &var );
//	if( var.vt == VT_I4 ) m_bShowAdditionLines = var.lVal != 0;


//	var.Clear();
//	pINamedData->GetValue( _bstr_t( CARIO_EDITABLE_IDIO ), &var );
//	if( var.vt == VT_I4 ) m_bShowEditableIdio = var.lVal != 0;
}

/////////////////////////////////////////////////////////////////////////////
//рассчитываем все на свете координаты для объекта, кроме тотального смещения
void chromo_object::layout_object( bool bShowChromoCenterLine, bool m_bShowChromoCentromere, bool bShowMainIdio, AlignChromo ac, int nCurrPane, CCarioImpl *pImpl, int nClassNum )
{
	m_rectIdio.SetRectEmpty();
	m_rectChromo.SetRectEmpty();
	m_rectImage.SetRectEmpty();
	m_rectEditableIdio.SetRectEmpty();
	m_rectFull.SetRectEmpty();

	m_rectCGHGraph.SetRectEmpty();
	m_rectCGHRatio.SetRectEmpty();
	m_rectText.SetRectEmpty();


	m_sizeTotal = CSize( 0, 0 );
	m_nCentromereY = 0;
	m_ptOffset = CPoint( 0, 0 );
	//сначала считаем, что начало координат - некая условная точка, соотв. началу изображения
	//(или идиограммы, если мы не имеем изображения)
	double	chromo_length = 0;
	int nChromoMinX	= 0;
	int nChromoMinY	= 0;
	int nChromoMaxX	= 0;
	int nChromoMaxY	= 0;

	double fCentrLength = 0;
	if( m_ptrChromos != 0 )
	{
		IUnknown* punkImage = 0;
		
		BOOL bOrigImage = FALSE;
		m_ptrChromos->GetEqualizedRotatedImage(  m_bErected ? 0 : AT_Original, &punkImage, &bOrigImage );
		if( punkImage )
		{
			IImagePtr ptrImage( punkImage );	
			punkImage->Release();	punkImage = 0;
	
			if( ptrImage != 0 )
			{
				//пока nChromoMin* - в координатах без увеличения
				int cx, cy;
				cx =  cy = 0;
				ptrImage->GetSize( &cx, &cy ); 
				m_rectImage.right = m_rectImage.left+cx*m_fZoom;
				m_rectImage.bottom = m_rectImage.top+cy*m_fZoom;

				nChromoMaxX = cx;
				nChromoMaxY = cy;

				//get y center point
				long lSize = 0;
				BOOL bIsManual = TRUE;


				m_ptrChromos->GetAxisSize( AT_Mirror|AT_Rotated|AT_VisAngle|AT_LongAxis | ( m_bErected ? 0 : AT_Original ), &lSize, &bIsManual );
				if( lSize )
				{
					POINT* arPt = new POINT[lSize];
					m_ptrChromos->GetAxis( AT_Mirror|AT_Rotated|AT_VisAngle|AT_LongAxis | ( m_bErected ? 0 : AT_Original ), arPt );

					CPoint ptCenter( 0, 0 );
					m_ptrChromos->GetCentromereCoord( AT_Mirror|AT_Rotated|AT_VisAngle|AT_ShortAxis | ( m_bErected ? 0 : AT_Original ), &ptCenter, 0, &bIsManual );

					m_nCentromereY = ptCenter.y * m_fZoom-m_nYOffset;

					bool bMirrorV = arPt[0].y > arPt[lSize - 1].y;

					//ходим вдоль центральной линии. заодно считаем ее пиксельную длину
					bool bFound = 0;
					for( int j=0;j<lSize;j++ )
					{
						if( j > 0 )
							chromo_length += sqrt(double((arPt[j].x-arPt[j-1].x)*(arPt[j].x-arPt[j-1].x)
								+(arPt[j].y-arPt[j-1].y)*(arPt[j].y-arPt[j-1].y)));

						if( !bFound )
						{
							if( bMirrorV )
							{
								if( ptCenter.y < arPt[j].y )
									fCentrLength = chromo_length;
								else
								{
									fCentrLength = chromo_length;
									bFound = 1;
								}
							}
							else
							{
								if( ptCenter.y > arPt[j].y )
									fCentrLength = chromo_length;
								else
								{
									fCentrLength = chromo_length;
									bFound = 1;
								}
							}
						}

						if( bShowChromoCenterLine )
						{
							if( arPt[j].x < nChromoMinX )	nChromoMinX = arPt[j].x;
							if( arPt[j].x > nChromoMaxX )	nChromoMaxX = arPt[j].x;

							if( arPt[j].y < nChromoMinY )	nChromoMinY = arPt[j].y;
							if( arPt[j].y > nChromoMaxY )	nChromoMaxY = arPt[j].y;
							
						}
					}

					if( fCentrLength > ( chromo_length - fCentrLength ) )
					{
						fCentrLength = chromo_length - fCentrLength;
					}

					delete []arPt;	arPt = 0;
				}	
				//теперь nChromoMin* - с увеличением

				{
					nChromoMaxX	++;
					nChromoMaxY ++;
					nChromoMinX		*= m_fZoom;
					nChromoMinY		*= m_fZoom;
					nChromoMaxX		*= m_fZoom;
					nChromoMaxY		*= m_fZoom;

					m_rectChromo.left = nChromoMinX;
					m_rectChromo.right = nChromoMaxX;
					m_rectChromo.top = nChromoMinY;
					m_rectChromo.bottom = nChromoMaxY;
				}

				//на настоящий момент имеем прямоугольник изображения,
				//центромеру и прямоугольник ц. линии в координатах изображения
			}
		}
	}

	m_rectCGHGraph = CRect(0,0,0,0);
	m_rectCGHRatio = CRect(0,0,0,0);

	if( pImpl->m_bShowCGHGraph )
		init_cgh_graph( pImpl );
					    
	if( pImpl->m_bShowCGHRatio )
		init_cgh_ratio( pImpl );

	CRect rcPrevIdio;
	if( m_ptrIdio != 0 && m_bShowIdiogramm )
	{
		if( chromo_length < 1 )chromo_length = 100;	

		m_sizeIdio.cx = int(m_nIdioWidth * m_fZoom);
		
		if( pImpl->m_bShowEditableIdio && pImpl->GetCarioViewMode() == cvmCellar )
		{
			m_sizeIdio.cy = int( chromo_length*m_fZoom  );
			if( m_sizeIdio.cy == 0 )m_sizeIdio.cy = int(chromo_length*m_fZoom);
		}
		else
		{
			m_sizeIdio.cy = int( m_rectImage.Height() );
			if( m_sizeIdio.cy == 0 )m_sizeIdio.cy = int( m_rectImage.Height() );
		}

		
		m_rectIdio.left = 0;

		m_rectIdio.top = 0;
		//все остальное мы рассчитаем - если выравнивание по центромере
		IUnknownPtr sptrW = pImpl->GetViewPtr();
		CWnd *pWnd = GetWindowFromUnknown( sptrW );

		CDC	*cdc = pWnd->GetDC();

		m_ptrIdio->set_show_num( pImpl->m_nShowNums );

		m_ptrIdio->draw( *cdc, m_sizeIdio, &m_rectIdio, 0, IDF_CALCRECT | IDF_IGNORELENGTH, m_fZoom );
		pWnd->ReleaseDC( cdc );

		if( m_rectIdio.top < 0 )
			m_rectIdio.OffsetRect( 0, -m_rectIdio.top );
		

		if( bShowMainIdio )
		{
			m_rectIdio.right = m_rectIdio.left + 2 * m_rectIdio.Width() + INNER_INT;

			if( pImpl->m_nShowNums )
				m_rectIdio.right += INNER_INT;

			if( m_rectIdio.top < 0 )
				m_rectIdio.OffsetRect( 0, -m_rectIdio.top );

			rcPrevIdio = m_rectIdio;
			if( pImpl->m_bShowCGHAmpl )
				m_rectIdio.right += 2 * ( pImpl->m_CGHAMPL_CX + CGHAMPL_OFFSET_X );
				
		}
	}
	else if( bShowMainIdio )
	{
		if( pImpl->m_carioViewMode == cvmCellar )
		{
			ICalcObjectPtr	ptr_calc( m_ptrMeasureObject );
			nClassNum = get_object_class( ptr_calc );
		}

		IUnknownPtr sptrW = pImpl->GetViewPtr();
		CWnd *pWnd = GetWindowFromUnknown( sptrW );

		CDC	*cdc = pWnd->GetDC();

		long	lpos = pImpl->m_idiograms.find( nClassNum );
		if( lpos )
		{
			IIdiogramPtr	ptrIdio = pImpl->m_idiograms.get( lpos );

			m_sizeIdio.cx = int(m_nIdioWidth*m_fZoom);


			if( pImpl->m_bShowEditableIdio && pImpl->GetCarioViewMode() == cvmCellar )
			{
				m_sizeIdio.cy = int( chromo_length*m_fZoom );
				if( m_sizeIdio.cy == 0 )m_sizeIdio.cy = int(chromo_length*m_fZoom);
			}
			else
			{
				m_sizeIdio.cy = int( m_rectImage.Height() );
				if( m_sizeIdio.cy == 0 )m_sizeIdio.cy = int( m_rectImage.Height() );
			}

			ptrIdio->set_show_num( pImpl->m_nShowNums );
			ptrIdio->draw( *cdc, m_sizeIdio, &m_rectIdio, 0, IDF_IGNORELENGTH | IDF_IGNORECENTROMERE | IDF_BLUE | IDF_CALCRECT, m_fZoom );
		}

		pWnd->ReleaseDC( cdc );

		rcPrevIdio = m_rectIdio;
		if( pImpl->m_bShowCGHAmpl )
			m_rectIdio.right += 2 * ( pImpl->m_CGHAMPL_CX + CGHAMPL_OFFSET_X );

	}

	if( pImpl->m_bShowEditableIdio && pImpl->GetCarioViewMode() == cvmCellar )
	{
		if( !m_rectIdio.IsRectEmpty() )
		{
  			double fOffset = ( fCentrLength*m_fZoom ) / (chromo_length*m_fZoom);
			int dx = int(m_rectIdio.Height() * fOffset +0.5) - m_nCentromereY;
			m_rectIdio.OffsetRect( 0, -dx );
		}
	}


//********** For Idio Edit 
	if( pImpl->m_bShowEditableIdio && pImpl->GetCarioViewMode() == cvmCellar )
	{
		if( chromo_length < 1 )chromo_length = 100;	

		m_fLength = m_rectImage.Height();
		{
			ICalcObjectPtr	ptr_calc( m_ptrMeasureObject );
			int nClassNum = get_object_class( ptr_calc );

			long	lpos = pImpl->m_idiograms.find( nClassNum );

			if( !lpos )
			{
				bool b = pImpl->m_bShowCellIdio;

				pImpl->m_bShowCellIdio = true;
				pImpl->load_idiograms();
				pImpl->m_bShowCellIdio = b;

				lpos = pImpl->m_idiograms.find( nClassNum );
			}

			if( lpos )
			{
				IIdiogramPtr	ptrIdio = pImpl->m_idiograms.get( lpos );

				double _l = 0,_c = 0;
				ptrIdio->get_params( &_l, &_c );

				m_fLength = _l;
			}
		}

		m_sizeEditableIdio.cx = int(m_nIdioWidth*m_fZoom);
		m_sizeEditableIdio.cy = int( chromo_length*m_fZoom );
		if( m_sizeEditableIdio.cy == 0 )m_sizeEditableIdio.cy = int(chromo_length*m_fZoom );
		
		int dx = 0;
		if( m_sptrEditableIdio != 0 )
		{
			IIdioBendsPtr sptrB = m_sptrEditableIdio;

			long lCount = 0;
 			sptrB->GetBengsCount( &lCount );

			if( m_fLength < m_rectImage.Height() )
				m_fLength = m_rectImage.Height();

  			double fOffset = ( fCentrLength ) / chromo_length;
			
			dx = int( m_sizeEditableIdio.cy * fOffset + 0.5 ) - m_nCentromereY;

			if( lCount == 0 )
			{
				sptrB->Add( 0, fOffset, IDIC_WHITE, _bstr_t( "11" ), IDIB_SIMPLE );
				sptrB->Add( fOffset, 1, IDIC_WHITE, _bstr_t( "11" ), IDIB_CENTROMERE );

				m_sptrEditableIdio->set_params( m_fLength, fOffset );
			}
			else
			{
				double fLen = 0, fCentr = 0;
				m_sptrEditableIdio->get_params( &fLen, &fCentr );



				if( fLen == -1 || fCentr == -1 )
				{	 
					m_sptrEditableIdio->set_params( m_fLength, fOffset );
					IIdioBendsPtr sptrBends = m_sptrEditableIdio;

					long lSz = 0;
					sptrBends->GetBengsCount( &lSz );

					for( long i = 0; i < lSz; i++ )
					{
						int nFlags = 0;
						sptrBends->GetBendFlag( i, &nFlags );
						if( nFlags == IDIB_CENTROMERE )
						{
							sptrBends->SetBendFlag( i, IDIB_SIMPLE );

							double _pos1 = 0, _pos2 = 0, pos1 = 0, pos2 = 0;

							sptrBends->GetBendRange( i - 1, &_pos1, &_pos2 );
							sptrBends->GetBendRange( i, &pos1, &pos2 );

							sptrBends->SetBendRange( i - 1, _pos1, pos2 );

							sptrBends->Remove( i );

							sptrBends->GetBengsCount( &lSz );

							break;
						}
					}

					for( long i = 0; i < lSz; i++ )
					{
						double fMin, fMax;
						sptrBends->GetBendRange( i, &fMin, &fMax );

						if( fOffset >= fMin && fOffset <= fMax )
						{
							sptrBends->SetBendRange( i, fMin, fOffset );
							sptrBends->AddAfter( i, fOffset, fMax, IDIC_WHITE, 0, IDIB_CENTROMERE );

							int z = 1;
							for( long j = i; j >= 0; j-- )
							{
								CString str;
								str.Format( "1%d", z++ );
								sptrBends->SetBendNumber( j, _bstr_t( str ) );
							}

							z = 1;
							for( long j = i + 1; j < lSz + 1; j++ )
							{
								CString str;
								str.Format( "1%d", z++ );
								sptrBends->SetBendNumber( j, _bstr_t( str ) );
							}

							break;
						}
					}

				}
			}

			IUnknownPtr sptrW = pImpl->GetViewPtr();
			CWnd *pWnd = GetWindowFromUnknown( sptrW );

			CDC	*cdc = pWnd->GetDC();
			m_sptrEditableIdio->set_show_num( pImpl->m_nShowEditableNums );
			m_sptrEditableIdio->draw( *cdc, m_sizeEditableIdio, &m_rectEditableIdio, 0, IDF_CALCRECT | IDF_IGNORECENTROMERE | IDF_IGNORELENGTH, m_fZoom );
			pWnd->ReleaseDC( cdc );

			//if( m_rectEditableIdio.top < 0 )
			//	m_rectEditableIdio.top = 0;

			/*if( m_ptrIdio != 0 && m_bShowIdiogramm )
			{
				m_rectIdio.top = m_rectEditableIdio.top;
				m_rectIdio.bottom = m_rectEditableIdio.bottom;
			}  */
		}

		m_rectEditableIdio.OffsetRect( 0, -dx + m_nYOffset );

	}

	if( pImpl->m_bShowCGHGraph  && m_sptrChart != 0 )
 	{
		if( !m_rectEditableIdio.IsRectEmpty() )
			m_rectCGHGraph = m_rectEditableIdio;
		else
			m_rectCGHGraph = m_rectImage;

		m_rectCGHGraph.right = m_rectCGHGraph.left + pImpl->m_CGHGRAPH_CX * m_fZoom;
	}

	if( pImpl->m_bShowCGHRatio && m_sptrRatio != 0 )
	{
		if( !m_rectEditableIdio.IsRectEmpty() )
			m_rectCGHRatio = m_rectEditableIdio;
		else
			m_rectCGHRatio = m_rectImage;

		m_rectCGHRatio.right = m_rectCGHRatio.left + pImpl->m_CGHRATIO_CX * m_fZoom;
	}
//********** For Idio Edit 
	bool bVMirror = false;
	CRect rcPrev[] = { m_rectImage, m_rectCGHGraph, rcPrevIdio, m_rectCGHRatio, m_rectEditableIdio };
	int nPrev[] = { nChromoMinX, nChromoMaxX, nChromoMinY, nChromoMaxY };
	_update_rects( nChromoMinX, nChromoMaxX, nChromoMinY, nChromoMaxY, bShowMainIdio, pImpl );

	double fMinRatioVal = pImpl->m_fMinRatioVal,
		   fMaxRatioVal = pImpl->m_fMaxRatioVal,
		   fRatioStepVal = pImpl->m_fRatioStepVal;
	if( ( pImpl->m_bShowCGHGraph || pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl || pImpl->m_bShowAdditionLines ) && m_ptrChromos != 0 )
	{
		IUnknown* punkImage = 0;
		BOOL bOrigImage = FALSE;
		m_ptrChromos->GetEqualizedRotatedImage(  m_bErected ? 0 : AT_Original, &punkImage, &bOrigImage );

		IImage3Ptr ptrImage = punkImage;

		if( punkImage )
			punkImage->Release();	

		IUnknown *punkObjList = ::GetActiveObjectFromContext( pImpl->GetViewPtr(), szTypeObjectList );

		if( punkObjList )
			punkObjList->Release();
		
		INamedDataPtr pINamedData = punkObjList;
		BSTR	bstr = 0;
		::StringFromCLSID( m_guidMeasureObject, &bstr );
		CString strGUID = bstr;
		::CoTaskMemFree( bstr );	bstr = 0;

		m_strRatioTextDOWN = "";
		int nAverCount = 0;
		if( pINamedData != 0 )
		{
			long nCount = ::GetValueInt( pINamedData, CGH_ROOT, szCghAveraged, 0 );

			if( nCount > 0 )
			{
				nCount = ::GetValueInt( pINamedData,
				CGH_ROOT "\\" szCghObjects "\\" + strGUID,
				szCghAveraged, 0 );
				nAverCount = nCount;

				if( nCount > 0 )
					m_strRatioTextDOWN.Format( "n=%ld", nCount );
			}
		}

		
		if( ptrImage != 0 )
		{
			long nSz = 0;
			FPOINT *Pts = 0;

			m_ptrChromos->GetAxisSize( AT_Mirror|AT_Rotated|AT_VisAngle|AT_LongAxis | ( m_bErected ? 0 : AT_Original ), &nSz, 0 );

			if( nSz != 0 )
			{
				Pts = new FPOINT[nSz];
				m_ptrChromos->GetAxis( AT_Mirror|AT_Rotated|AT_AxisDouble|AT_VisAngle|AT_LongAxis | ( m_bErected ? 0 : AT_Original ), (POINT*)Pts );
			
				ROTATEPARAMS RotateParams = {0};
				m_ptrChromos->GetRotateParams( &RotateParams );

				bool bMirrorV = Pts[0].y > Pts[nSz - 1].y;
				bool bMirrorH = RotateParams.bMirrorV != 0;

				bVMirror = bMirrorV;

				double *pValueX = new double[ nSz ];
				double *pValueY = new double[ nSz ];

				if( pImpl->m_bShowAdditionLines && pImpl->m_bShowEditableIdio )
				{
					m_strAddLines = new XAddLinesStruct[nSz];
					m_strAddLinesFull = new XAddLinesStruct[nSz];
					m_nLinesCount = nSz;


					::ZeroMemory( m_strAddLines, sizeof( XAddLinesStruct ) * nSz );
					::ZeroMemory( m_strAddLinesFull, sizeof( XAddLinesStruct ) * nSz );
				}

				double *pValueX_R = 0;
				double *pValueY_R = 0;

				if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl )
				{
					pValueX_R = new double[ nSz ];
					pValueY_R = new double[ nSz ];
				}

				int cx, cy;
				ptrImage->GetSize( &cx, &cy );
				CRect rcImg = CRect( 0, 0, cx, cy );

				bool bNormal = false;
				long nSameID = -1;
				_list_t<int> listSkippedBr;
				_list_t<int> listSkippedRat;
				for( long l = 1; l < nSz - 1 ; l++ )
				{
					int e = l + 1, s = l;

					if( l == nSz - 1 )
					{ 
						e = l; s = l - 1; 
					}


					int _offset = 5;
					int _prev = s - _offset > 0 ? _offset : s;
					int _next = s + _offset < nSz - 1? _offset :  nSz - 1 - s - _offset;

					double x = Pts[s + _next].x - Pts[s - _prev].x; // Направляющий вектор
 					double y = Pts[s + _next].y - Pts[s - _prev].y;

					{
						double len = _hypot( x, y );

						if( !len )
							len = 1;
						x /= len;
						y /= len;
					}

					double x2 = y;  // -Нормальный вектор через упрощенный CrossProduct
 					double y2 = -x;

					if( m_bErected )
					{
						double _x = Pts[0].x - Pts[nSz - 1].x; // Направляющий вектор
 						double _y = Pts[0].y - Pts[nSz-1].y;
						double len = _hypot( _x, _y );

						if( !len )
							len = 1;

						_x /= len;
						_y /= len;

						x2 = _y;
						y2 = -_x;
					}

					if( !x2 && !y2 )
					{
						if( bNormal )
						{
							if( pImpl->m_bShowCGHGraph )
							{
								pValueX[l] = l;
								pValueY[l] = pValueY[l - 1];
							}
							if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl )
							{
								pValueX_R[l] = l;
								pValueY_R[l] = pValueY_R[l - 1];
							}
						}
						else
							nSameID = l;

						continue;
					}

					bNormal = true;
					if( l == nSz - 1 )
						s = l;

					FPoint pt1 = FPoint( Pts[s].x + x2, Pts[s].y + y2 ), pt2 = FPoint( Pts[s].x - x2, Pts[s].y - y2 );

					{
						if( nCurrPane == 3 )
							nCurrPane = ::GetValueInt( ::GetAppUnknown(), CGH_ROOT, CGH_BRIGHTPANE, 0 );
						else if( nCurrPane == 4 )
							nCurrPane = ::GetValueInt( ::GetAppUnknown(), CGH_ROOT, CGH_RATIOPANE, 0 );

                 		int nPaneID = nCurrPane;
						_find_end_points( ptrImage, pt1, pt2, nPaneID ); 
					}

					/*
					int i = 1;
					while( rcImg.PtInRect( pt1 ) )
					{
						pt1.x = Pts[s].x + x2 * i;
						pt1.y = Pts[s].y + y2 * i;
						i += 1;
					};

					i = 1;
					while( rcImg.PtInRect( pt2 ) )
					{
						pt2.x = Pts[s].x - x2 * i;
						pt2.y = Pts[s].y - y2 * i;
						i += 1;
					};
					*/


					if( pImpl->m_bShowCGHGraph )
					{
						if( nCurrPane == 3 )
							nCurrPane = ::GetValueInt( ::GetAppUnknown(), CGH_ROOT, CGH_BRIGHTPANE, 0 );
						else if( nCurrPane == 4 )
							nCurrPane = ::GetValueInt( ::GetAppUnknown(), CGH_ROOT, CGH_RATIOPANE, 0 );

                 		int nPaneID = nCurrPane;

						double fVal = _calc_cgh_value( ptrImage, pt1, pt2, nPaneID, 0, 0 );

						if( fVal == -1 )
							listSkippedBr.add_tail( l );

						pValueX[l] = l;
						pValueY[l] = fVal / ( colorMax * 0.5 );
					}

					if( pImpl->m_bShowAdditionLines && pImpl->m_bShowEditableIdio )
					{
						FPOINT _pt1 = {0}, _pt2 = {0};

						if( nCurrPane == 3 )
							nCurrPane = ::GetValueInt( ::GetAppUnknown(), CGH_ROOT, CGH_BRIGHTPANE, 0 );
						else if( nCurrPane == 4 )
							nCurrPane = ::GetValueInt( ::GetAppUnknown(), CGH_ROOT, CGH_RATIOPANE, 0 );

                 		int nPaneID = nCurrPane;

						_calc_cgh_value( ptrImage, pt1, pt2, nPaneID, 0, 0 );
						
						m_strAddLines[l].m_ptsLines.left = pt1.x * m_fZoom;
						m_strAddLines[l].m_ptsLines.right  = pt2.x * m_fZoom;

						m_strAddLines[l].m_ptsLines.top  = pt1.y * m_fZoom;
						m_strAddLines[l].m_ptsLines.bottom = pt2.y * m_fZoom;

						m_strAddLinesFull[l].m_ptsLines.left = pt1.x * m_fZoom;
						m_strAddLinesFull[l].m_ptsLines.right  = pt2.x * m_fZoom;

						m_strAddLinesFull[l].m_ptsLines.top  = pt1.y * m_fZoom;
						m_strAddLinesFull[l].m_ptsLines.bottom = pt2.y * m_fZoom;

						m_strAddLinesFull[l].m_pt = Pts[s];
					}

					if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl )
					{
						int nPaneID = CGH_RATIO;
						double fVal = _calc_cgh_value( ptrImage, pt1, pt2, nPaneID, 0, 0 );

						if( fVal == -1 )
							listSkippedRat.add_tail( l );

						pValueX_R[l] = l;
						pValueY_R[l] = fVal / ( colorMax * 0.5 );
					}

					if( nSameID != -1 )
					{
						for( long z = nSameID; z < l ; z++ )
						{
							if( pImpl->m_bShowCGHGraph )
							{
								pValueX[z] = z;
								pValueY[z] = pValueY[l];
							}
							if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl )
							{
								pValueX_R[z] = z;
								pValueY_R[z] = pValueY_R[l];
							}
						}

						nSameID = -1;
					}


				}

				while( listSkippedBr.count() )
				{
					long lCount = listSkippedBr.count();
					for( long lPos = listSkippedBr.head(); lPos; lPos = listSkippedBr.next( lPos ) )
					{
						int nID = listSkippedBr.get( lPos );
						if( nID + 1 < nSz - 1 )
						{
							int n = nID + 1;
							if( !listSkippedBr.find( n ) )
							{
								pValueY[nID] = pValueY[nID+1];
								listSkippedBr.remove( lPos );
								break;
							}
						}
						else if( nID - 1 > 0 )
						{
							int n = nID - 1;
							if( !listSkippedBr.find( n ) )
							{
								pValueY[nID] = pValueY[nID-1];
								listSkippedBr.remove( lPos );
								break;
							}
						}
					}
					if( lCount == listSkippedBr.count() )
						break;
				}

				while( listSkippedRat.count() )
				{
					long lCount = listSkippedRat.count();
					for( long lPos = listSkippedRat.head(); lPos; lPos = listSkippedRat.next( lPos ) )
					{
						int nID = listSkippedRat.get( lPos );
						if( nID + 1 < nSz - 1 )
						{
							int n = nID + 1;
							if( !listSkippedRat.find( n ) )
							{
								pValueY[nID] = pValueY[nID+1];
								listSkippedRat.remove( lPos );
								break;
							}
						}
						else if( nID - 1 > 0 )
						{
							int n = nID - 1;
							if( !listSkippedRat.find( n ) )
							{
								pValueY[nID] = pValueY[nID-1];
								listSkippedRat.remove( lPos );
								break;
							}
						}
					}
					if( lCount == listSkippedRat.count() )
						break;
				}

				if( pImpl->m_bShowCGHGraph )
				{
					pValueX[0] = pValueX[1] - 1;
					pValueY[0] = pValueY[1];

					pValueX[nSz - 1] = pValueX[nSz - 2] + 1;
					pValueY[nSz - 1] = pValueY[nSz - 2];
				}

				if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl )
				{
					pValueX_R[0] = pValueX_R[1] - 1;
					pValueY_R[0] = pValueY_R[1];

					pValueX_R[nSz - 1] = pValueX_R[nSz - 2] + 1;
					pValueY_R[nSz - 1] = pValueY_R[nSz - 2];
				}

				double fMaxX = max( pValueX[0], pValueX[nSz - 1] );

				for( l = 0; l < nSz ; l++ )
					pValueX[l] = ( pValueX[l] / fMaxX ) * 100;
				
				long halfSz = nSz / 2;
				for( l = 0; l < halfSz ; l++ )
				{
					if( bMirrorH )
					{
						if( pImpl->m_bShowCGHGraph )
						{
							double fTmp = pValueY[nSz - 1 - l];
							pValueY[nSz - 1 - l] = pValueY[l];
							pValueY[l] = fTmp;
						}

						if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl )
						{
							double fTmp = pValueY_R[nSz - 1 - l];
							pValueY_R[nSz - 1 - l] = pValueY_R[l];
							pValueY_R[l] = fTmp;
						}
					}

					if( bMirrorV )
					{
						if( pImpl->m_bShowCGHGraph )
						{
							double fTmp = pValueX[nSz - 1 - l];
							pValueX[nSz - 1 - l] = pValueX[l];
							pValueX[l] = fTmp;
						}

						if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHAmpl )
						{
							double fTmp = pValueX_R[nSz - 1 - l];
							pValueX_R[nSz - 1 - l] = pValueX_R[l];
							pValueX_R[l] = fTmp;
						}
					}
				}

				if( pImpl->m_bShowCGHGraph && m_sptrChart != 0 )
				{

					for( long i = 0; i < nSz; i++ )
						pValueY[i] = int( ( pValueY[i] / 2.0 ) * 255 );

					double fMin = pValueY[0], fMax = pValueY[0];
					for( long i = 1; i < nSz; i++ )
					{
						if( pValueY[i] < fMin )
							fMin = pValueY[i];
						if( pValueY[i] > fMax )
							fMax = pValueY[i];
					}


					double pX_horz[]  = { fMin,   fMax };
					double pY_horz[]  = { pValueX[0],   pValueX[0] };
					double pY_horz2[]  = { pValueX[nSz - 1], pValueX[nSz - 1] };

					m_sptrChart->SetData( 0, pValueY, nSz, cdfDataX ); 	  // Это не ошибка 
					m_sptrChart->SetData( 0, pValueX, nSz, cdfDataY ); 

					m_sptrChart->SmoothChart( 0, 5, 0, csfMovingAverage );
//					m_sptrChart->SmoothChart( 0, 3, 0, csfSpline2 );


					long ls = 0;
					// Производная
					double *_pValueY = 0;
					double *_pValueX = 0;
					
					m_sptrChart->GetData( 0, &_pValueY, &ls, cdfDataX ); 
					m_sptrChart->GetData( 0, &_pValueX, &ls, cdfDataY ); 

					double *pValueY_2 = new double[ ls ];

					for( i = 0; i < ls - 1; i++ )
						pValueY_2[i] =  2*( _pValueY[i + 1] - _pValueY[i] ) / ( _pValueX[i + 1] - _pValueX[i] ) ;

					pValueY_2[nSz - 1] = pValueY_2[nSz - 2];

					m_sptrChart->SetData( 1, pValueY_2, ls, cdfDataX );
					m_sptrChart->SetData( 1, _pValueX, ls, cdfDataY ); 


/*
					m_sptrChart->SetData( 10, pX_horz, 2, cdfDataX );
					m_sptrChart->SetData( 10, pY_horz, 2, cdfDataY ); 

					m_sptrChart->SetData( 20, pX_horz,  2, cdfDataX );
					m_sptrChart->SetData( 20, pY_horz2, 2, cdfDataY ); 
*/

					m_sptrChart->SmoothChart( 1, 5, 0, csfMovingAverage );
//					m_sptrChart->SmoothChart( 1, 3, 0, csfSpline2 );

					delete []pValueY_2;
				}


				if( pImpl->m_bUseAutoScaleRatio && pImpl->m_bShowCGHRatio )
				{
					fMinRatioVal = 1e308;
				    fMaxRatioVal = -1e307;

					IUnknown *punk = 0;
					::CoCreateInstance( CLSID_ChartDrawer, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk );

					IChartDrawerPtr sptrRatio = punk;

					if( sptrRatio == 0 )
						return;

					if( punk )
						punk->Release();

					sptrRatio->SetData( 0, pValueY_R, nSz, cdfDataX ); 	  // Это не ошибка 
					sptrRatio->SetData( 0, pValueX_R, nSz, cdfDataY ); 

					sptrRatio->SmoothChart( 0, 3, 0, csfMovingAverage );
//					sptrRatio->SmoothChart( 0, 3, 0, csfSpline2 );

					IChartAttributesPtr ptrChart = sptrRatio;

					double *pVX = 0;
					long ls = 0;
					sptrRatio->GetData( 0, &pVX, &ls, cdfDataX ); 

					for( UINT i = 0; i < ls; i++ )
					{
						if( pVX[i] < fMinRatioVal )
							fMinRatioVal = pVX[i];

						if( pVX[i] > fMaxRatioVal )
							fMaxRatioVal = pVX[i];
					}

					double nSz = double( ( pImpl->m_fMaxRatioVal - pImpl->m_fMinRatioVal ) / fRatioStepVal );

					fRatioStepVal = ( fMaxRatioVal - fMinRatioVal ) / nSz;
				}
				
				if( m_sptrRatio != 0 )
				{
					IChartAttributesPtr ptrChart = m_sptrRatio;
					int nSz = int( ( fMaxRatioVal - fMinRatioVal) / fRatioStepVal );
					for( int i = 0; i <= nSz; i++ )
					{
						double fVal = fMinRatioVal + fRatioStepVal * i;

						ptrChart->SetChartColor( 100 + i, pImpl->m_clRatioGrad, ccfChartColor ); 
						ptrChart->SetChartLineStyle( 100 + i, PS_DOT,  clfChartStyle );
					}
				}


				if( pImpl->m_bShowCGHRatio || pImpl->m_bShowCGHGraph || pImpl->m_bShowCGHAmpl )
				{
					double pY[]  = { 0,   nSz - 1 };

					double pX[]  = { pImpl->m_fNormalMin, pImpl->m_fNormalMin };
					double pX2[] = { pImpl->m_fNormalMax, pImpl->m_fNormalMax };
					double pX3[] = { pImpl->m_fNormalMid, pImpl->m_fNormalMid };

					if( pImpl->m_bShowCGHRatio && m_sptrRatio != 0 )
					{
						IChartAttributesPtr ptrChart = m_sptrRatio;

						double fMin = fMinRatioVal, fMax = fMaxRatioVal;

						double pX_horz[]  = { fMin,   fMax };
						double pY_horz[]  = { pValueX_R[0],   pValueX_R[0] };
						double pY_horz2[]  = { pValueX_R[nSz - 1], pValueX_R[nSz - 1] };


						int arrCharts[8] = { 0, 1, 2, 3, 10, 20, 1000, 1001 };

						for( int i = 0; i < 8; i++ )
						{
							ptrChart->SetChartRange( arrCharts[i], pY[0], pY[1], crfRangeY );
							ptrChart->SetRangeState( arrCharts[i], 1, crfRangeY );

							ptrChart->SetChartRange( arrCharts[i], fMinRatioVal, fMaxRatioVal, crfRangeX );
							ptrChart->SetRangeState( arrCharts[i], 1, crfRangeX );

						}

						m_sptrRatio->SetData( 0, pValueY_R, nSz, cdfDataX ); 	  // Это не ошибка 
						m_sptrRatio->SetData( 0, pValueX_R, nSz, cdfDataY ); 

						if( pImpl->m_bShowProb && nAverCount > 0 )
						{
							double tp = student( pImpl->m_fProbability, nAverCount ); 

							long lSz = ::GetValueInt( pINamedData, CGH_ROOT "\\" szCghObjects "\\" + strGUID,
										szCghDeviationSize, 0 ), lSz2;

							double *pData = (double *)GetValuePtr( pINamedData,
								CGH_ROOT "\\" szCghObjects "\\" + strGUID,
								szCghDeviation, &lSz2 );

							double *pX_L = new double[nSz], *pX_R = new double[nSz];
							
							for( int i = 0; i < nSz; i++ )
							{
								int j = int( pValueX_R[i] + 0.5 );

								if( j < 0 )
									j = 0;
								else if ( j >= nSz )
									j = nSz - 1;

								double val = tp * pData[j];

								pX_L[i] = pValueY_R[j] - val;
								pX_R[i] = pValueY_R[j] + val;

								if( pX_L[i] < 0 )
									pX_L[i] = 0;

								if( pX_R[i] > 2 )
									pX_R[i] = 2;
							}

							m_sptrRatio->SetData( 1000, pX_L,		nSz, cdfDataX );
							m_sptrRatio->SetData( 1000, pValueX_R, nSz, cdfDataY ); 

							m_sptrRatio->SetData( 1001, pX_R,		nSz, cdfDataX );
							m_sptrRatio->SetData( 1001, pValueX_R, nSz, cdfDataY ); 

							delete []pX_L;
							delete []pX_R;

							delete []pData;

							m_sptrRatio->SmoothChart( 1000, 3, 0, csfMovingAverage );
//							m_sptrRatio->SmoothChart( 1000, 3, 0, csfSpline2 );

							m_sptrRatio->SmoothChart( 1001, 3, 0, csfMovingAverage );
//							m_sptrRatio->SmoothChart( 1001, 3, 0, csfSpline2 );

						}
						else if( m_sptrRatio != 0 )
						{
							IChartAttributesPtr ptrChart = m_sptrRatio;
							ptrChart->ShowChart( 1000, false );
							ptrChart->ShowChart( 1001, false );
						}

						m_sptrRatio->SmoothChart( 0, 3, 0, csfMovingAverage );
//						m_sptrRatio->SmoothChart( 0, 3, 0, csfSpline2 );


						delete []pValueX_R;
						delete []pValueY_R;
						pValueX_R = 0;
						pValueY_R = 0;

						long ls = 0;
						m_sptrRatio->GetData( 0, &pValueY_R, &ls, cdfDataX ); 
						m_sptrRatio->GetData( 0, &pValueX_R, &ls, cdfDataY ); 
						nSz = ls;

						if( pImpl->m_fNormalMin >= fMinRatioVal )
						{
							m_sptrRatio->SetData( 1, pX,  2, cdfDataX );
							m_sptrRatio->SetData( 1, pY,  2, cdfDataY );
						}

						if( pImpl->m_fNormalMax <= fMaxRatioVal )
						{
							m_sptrRatio->SetData( 2, pX2, 2, cdfDataX );
							m_sptrRatio->SetData( 2, pY,  2, cdfDataY );
						}

						if( pImpl->m_fNormalMid >= fMinRatioVal && pImpl->m_fNormalMid <= fMaxRatioVal )
						{
							m_sptrRatio->SetData( 3, pX3, 2, cdfDataX );
							m_sptrRatio->SetData( 3, pY,  2, cdfDataY );
						}

						m_sptrRatio->SetData( 10, pX_horz, 2, cdfDataX );
						m_sptrRatio->SetData( 10, pY_horz, 2, cdfDataY ); 

						m_sptrRatio->SetData( 20, pX_horz,  2, cdfDataX );
						m_sptrRatio->SetData( 20, pY_horz2, 2, cdfDataY ); 


						IChartAttributesPtr ptrChartA = m_sptrRatio;

						ptrChartA->SetColorizedMap( 0, pImpl->m_clRatioMin, fMinRatioVal,      pY[0], pX[0], pY[1] );
						ptrChartA->SetColorizedMap( 0, pImpl->m_clRatioMax, pX2[0], pY[0], fMaxRatioVal,     pY[1] );

						{
							int nSz = int( ( fMaxRatioVal - fMinRatioVal ) / fRatioStepVal + 0.5 );
							double fVal = fMinRatioVal + fRatioStepVal;

							if( pImpl->m_bShowLineOnRatio )
							{
								for( i = 1; i < nSz; i++, fVal += fRatioStepVal )
								{
									if( fVal == pImpl->m_fNormalMin || fVal == pImpl->m_fNormalMid || fVal == pImpl->m_fNormalMax )
										continue;

									if( fVal > fMaxRatioVal || fVal < fMinRatioVal )
										continue;

									double pX[]  = { fVal, fVal };

									ptrChart->SetChartRange( 100 + i, pY[0], pY[1], crfRangeY );
									ptrChart->SetRangeState( 100 + i, 1, crfRangeY );

									ptrChart->SetChartRange( 100 + i, fMinRatioVal, fMaxRatioVal, crfRangeX );
									ptrChart->SetRangeState( 100 + i, 1, crfRangeX );


									m_sptrRatio->SetData( 100 + i, pX, 2, cdfDataX );
									m_sptrRatio->SetData( 100 + i, pY, 2, cdfDataY ); 
								}
							}
							else
							{
								IChartAttributesPtr ptrChart = m_sptrRatio;

								for( i = 1; i < nSz; i++, fVal += fRatioStepVal )
								{
									if( fVal == pImpl->m_fNormalMin || fVal == pImpl->m_fNormalMid || fVal == pImpl->m_fNormalMax )
										continue;

									if( fVal > fMaxRatioVal || fVal < fMinRatioVal )
										continue;

									ptrChart->ShowChart( 100 + i, false );

								}

							}


							{
								double pX[]  = { fMinRatioVal, fMinRatioVal };

								ptrChart->SetChartRange( 100, pY[0], pY[1], crfRangeY );
								ptrChart->SetRangeState( 100, 1, crfRangeY );

								ptrChart->SetChartRange( 100, fMinRatioVal, fMaxRatioVal, crfRangeX );
								ptrChart->SetRangeState( 100, 1, crfRangeX );


								m_sptrRatio->SetData( 100, pX, 2, cdfDataX );
								m_sptrRatio->SetData( 100, pY, 2, cdfDataY ); 
							}
							{
								double pX[]  = { fMaxRatioVal, fMaxRatioVal };

								ptrChart->SetChartRange( 100 + nSz, pY[0], pY[1], crfRangeY );
								ptrChart->SetRangeState( 100 + nSz, 1, crfRangeY );

								ptrChart->SetChartRange( 100 + nSz, fMinRatioVal, fMaxRatioVal, crfRangeX );
								ptrChart->SetRangeState( 100 + nSz, 1, crfRangeX );

								m_sptrRatio->SetData( 100 + nSz, pX, 2, cdfDataX );
								m_sptrRatio->SetData( 100 + nSz, pY, 2, cdfDataY ); 
							}
						}
					}
					else if( !pImpl->m_bShowCGHRatio && pImpl->m_bShowCGHAmpl )
					{

						IUnknown *punk = 0;
						::CoCreateInstance( CLSID_ChartDrawer, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk );

						IChartDrawerPtr sptrChart = punk;

						if( punk )
							punk->Release();
						
						sptrChart->SetData( 0, pValueY_R, nSz, cdfDataX );
						sptrChart->SetData( 0, pValueX_R, nSz, cdfDataY );

						sptrChart->SmoothChart( 0, 3, 0, csfMovingAverage );
//						sptrChart->SmoothChart( 0, 3, 0, csfSpline2 );


						delete []pValueX_R;
						delete []pValueY_R;
						pValueX_R = 0;
						pValueY_R = 0;

						long ls = 0;
						double *pX = 0, *pY = 0;

						sptrChart->GetData( 0, &pY, &ls, cdfDataX ); 
						sptrChart->GetData( 0, &pX, &ls, cdfDataY ); 
						nSz = ls;

						pValueX_R = new double[nSz];
						pValueY_R = new double[nSz];

						CopyMemory( pValueY_R, pY, sizeof(double) * ls );
						CopyMemory( pValueX_R, pX, sizeof(double) * ls );
					}

					if( pImpl->m_bShowCGHAmpl && bShowMainIdio )
					{
						int nID_L = -1, nID_R = -1;

						bMirrorH = pValueX_R[0] < pValueX_R[nSz - 1];

						m_listCGHAmpl_L.clear();
						m_listCGHAmpl_R.clear();

						for( l = 0; l < nSz; l++ )
						{
							if( nID_L == -1 && pValueY_R[l] < pX[0] )
								nID_L = l;
							else if( nID_L != -1 && pValueY_R[l] > pX[0] )
							{
								double y1 = 0, y2 = 0;

								if( nID_L > 0 )
								{
									if( !bMirrorH )
										y1 = CanonicY( pValueY_R[nID_L - 1], pValueX_R[nSz - nID_L - 1], pValueY_R[nID_L], pValueX_R[nSz - nID_L], pX[0] );
									else
										y1 = CanonicY( pValueY_R[nID_L - 1], pValueX_R[nID_L - 1], pValueY_R[nID_L], pValueX_R[nID_L], pX[0] );
								}

								if( !bMirrorH )
									y2 = CanonicY( pValueY_R[l - 1], pValueX_R[nSz - l - 1], pValueY_R[l], pValueX_R[nSz - l], pX[0] );
								else
									y2 = CanonicY( pValueY_R[l - 1], pValueX_R[l - 1], pValueY_R[l], pValueX_R[l], pX[0] );

								if( !bMirrorV )	  
									m_listCGHAmpl_L.add_tail( CInterval( pY[1] - y2 + 1, pY[1] - y1 + 1 ) );
								else
									m_listCGHAmpl_L.add_tail( CInterval( y1, y2 ) );


								nID_L = -1;
								m_bHasCGHAmplL = true;
							}

						}

						if( nID_L != -1 )
						{
							double y1 = 0;

							if( !bMirrorH )
							{
								if( nID_L > 0 )
									y1 = CanonicY( pValueY_R[nID_L - 1], pValueX_R[nSz - nID_L - 1], pValueY_R[nID_L], pValueX_R[nSz - nID_L], pX[0] );
								else
									y1 = pY[0];
							}
							else
							{
								if( nID_L > 0 )
									y1 = CanonicY( pValueY_R[nID_L - 1], pValueX_R[nID_L - 1], pValueY_R[nID_L], pValueX_R[nID_L], pX[0] );
								else
									y1 = pY[0];
							}

							if( !bMirrorV )
								m_listCGHAmpl_L.add_tail( CInterval( 0, pY[1] - y1 + 1 ) );
							else
								m_listCGHAmpl_L.add_tail( CInterval( y1, pY[1] + 1 ) );

							m_bHasCGHAmplL = true;
						}

						for( l = 0; l < nSz; l++ )
						{

							if( nID_R == -1 && pValueY_R[l] > pX2[0] )
								nID_R = l;
							else if( nID_R != -1 && pValueY_R[l] < pX2[0] )
							{
								double y1 = 0, y2 = 0;

								if( nID_R > 0 )
								{
									if( !bMirrorH )
										y1 = CanonicY( pValueY_R[nID_R - 1], pValueX_R[nSz - nID_R - 1], pValueY_R[nID_R], pValueX_R[nSz - nID_R], pX2[0] );
									else
										y1 = CanonicY( pValueY_R[nID_R - 1], pValueX_R[nID_R - 1], pValueY_R[nID_R], pValueX_R[nID_R], pX2[0] );
								}

								if( !bMirrorH )
									y2 = CanonicY( pValueY_R[l - 1], pValueX_R[nSz - l - 1], pValueY_R[l], pValueX_R[nSz - l], pX2[0] );
								else
									y2 = CanonicY( pValueY_R[l - 1], pValueX_R[l - 1], pValueY_R[l], pValueX_R[l], pX2[0] );

								if( !bMirrorV )	  
									m_listCGHAmpl_R.add_tail( CInterval( pY[1] + 1 - y2, pY[1] - y1 + 1 ) );
								else
									m_listCGHAmpl_R.add_tail( CInterval( y1, y2 ) );

								nID_R = -1;
								m_bHasCGHAmplR = true;
							}
						}

						if( nID_R != -1 )
						{
							double y1 = 0;
							
							if( !bMirrorH )
							{
								if( nID_R > 0 )
									y1 = CanonicY( pValueY_R[nID_R - 1], pValueX_R[nSz - nID_R - 1], pValueY_R[nID_R], pValueX_R[nSz - nID_R], pX2[0] );
								else
									y1 = pY[0];
							}
							else
							{
								if( nID_R > 0 )
									y1 = CanonicY( pValueY_R[nID_R - 1], pValueX_R[nID_R - 1], pValueY_R[nID_R], pValueX_R[nID_R], pX2[0] );
								else
									y1 = pY[0];
							}

							if( !bMirrorV )
								m_listCGHAmpl_R.add_tail( CInterval( 0, pY[1] - y1 + 1 ) );
							else
								m_listCGHAmpl_R.add_tail( CInterval( y1, pY[1] + 1 ) );

							nID_R = -1;
							m_bHasCGHAmplR = true;
						}

						if( !( m_bHasCGHAmplL && m_bHasCGHAmplR ) )
						{
							m_rectImage    = rcPrev[0];
							m_rectCGHGraph = rcPrev[1];
							m_rectIdio	   = rcPrev[2];
							m_rectCGHRatio = rcPrev[3];
							m_rectEditableIdio = rcPrev[4];

							nChromoMinX = nPrev[0];
							nChromoMaxX = nPrev[1];
							nChromoMinY = nPrev[2];
							nChromoMaxY = nPrev[3];


							if( m_bHasCGHAmplL || m_bHasCGHAmplR )
								m_rectIdio.right += ( pImpl->m_CGHAMPL_CX + CGHAMPL_OFFSET_X );

							_update_rects( nChromoMinX, nChromoMaxX, nChromoMinY, nChromoMaxY, bShowMainIdio, pImpl );
						}

/*********************************************************/
						long lPos = m_listCGHAmpl_L.head();
						while( lPos )
						{
							CInterval in = m_listCGHAmpl_L.get( lPos );
							in.a /= fabs( pValueX_R[nSz - 1] - pValueX_R[0] );
							in.b /= fabs( pValueX_R[nSz - 1] - pValueX_R[0] );

							m_listCGHAmpl_L.set( in, lPos );

							lPos = m_listCGHAmpl_L.next( lPos );
						}
/*********************************************************/
						lPos = m_listCGHAmpl_R.head();
						while( lPos )
						{
							CInterval in = m_listCGHAmpl_R.get( lPos );
							in.a /= fabs( pValueX_R[nSz - 1] - pValueX_R[0] );
							in.b /= fabs( pValueX_R[nSz - 1] - pValueX_R[0] );

							m_listCGHAmpl_R.set( in, lPos );

							lPos = m_listCGHAmpl_R.next( lPos );
						}
/*********************************************************/

					}
				}

				delete []pValueX;
				delete []pValueY;

				if( !pImpl->m_bShowCGHRatio && pImpl->m_bShowCGHAmpl )
				{
					delete []pValueX_R;
					delete []pValueY_R;
				}

				delete []Pts;
			}
		}

		if( m_pstrRatioTextUP )
			delete []m_pstrRatioTextUP;
		m_pstrRatioTextUP = 0;

		if( pImpl->m_bShowCGHRatio )
		{
			double fStep = ( fMaxRatioVal - fMinRatioVal ) / pImpl->m_nRatioTextRCCount;
			m_pstrRatioTextUP = new CString[pImpl->m_nRatioTextRCCount + 1];

			int i = 0;
			for( double f = fMinRatioVal; f <= fMaxRatioVal; f += fStep )
			{
				CString str;

				if( f - int(f) > 0 )
					str.Format( "%.2lf", f );
				else
					str.Format( "%d", int(f) );

				m_pstrRatioTextUP[ i++ ] = str;
			}

			m_sizeTotal.cy += 2 * CGHAMPL_RATIO_TEXT_CY;

			if( !m_rectIdio.IsRectEmpty() )
				m_rectIdio.OffsetRect( 0, CGHAMPL_RATIO_TEXT_CY );
			if( !m_rectChromo.IsRectEmpty() )
				m_rectChromo.OffsetRect( 0, CGHAMPL_RATIO_TEXT_CY );
			if( !m_rectImage.IsRectEmpty() )
				m_rectImage.OffsetRect( 0, CGHAMPL_RATIO_TEXT_CY );
			if( !m_rectCGHGraph.IsRectEmpty() )
				m_rectCGHGraph.OffsetRect( 0, CGHAMPL_RATIO_TEXT_CY );
			if( !m_rectCGHRatio.IsRectEmpty() )
				m_rectCGHRatio.OffsetRect( 0, CGHAMPL_RATIO_TEXT_CY );
			if( !m_rectEditableIdio.IsRectEmpty() )
				m_rectEditableIdio.OffsetRect( 0, CGHAMPL_RATIO_TEXT_CY );
			
			
			m_nCentromereY += CGHAMPL_RATIO_TEXT_CY;

		}
	}

	m_rectFull.UnionRect( &m_rectFull, &m_rectImage );
	m_rectFull.UnionRect( &m_rectFull, &m_rectCGHGraph );
	m_rectFull.UnionRect( &m_rectFull, &m_rectIdio );
	m_rectFull.UnionRect( &m_rectFull, &m_rectCGHRatio );
	m_rectFull.UnionRect( &m_rectFull, &m_rectEditableIdio );

//********** For Idio Edit 
	if( pImpl->m_carioViewMode == cvmCellar && pImpl->m_bShowClasses )
	{

		IUnknownPtr sptrW = pImpl->GetViewPtr();
		CWnd *pWnd = GetWindowFromUnknown( sptrW );

		if( pWnd )
		{
			CDC *pdc = pWnd->GetDC();
			
			CRect rcText( 0, 0, 300, 0 );

			ICalcObjectPtr	ptr_calc( m_ptrMeasureObject );
			long	class_no = get_object_class( ptr_calc );

			CString strClassName = get_class_name( class_no );

			if( m_strTextInfo.IsEmpty() )
				pdc->DrawText( strClassName, rcText, DT_CALCRECT );
			else
				pdc->DrawText( m_strTextInfo, rcText, DT_WORDBREAK | DT_VCENTER | DT_CENTER | DT_CALCRECT );

//			rcText.right = rcText.left + rcText.Width() * m_fZoom;
//			rcText.bottom = rcText.bottom + rcText.Height() * m_fZoom;

			m_rectText = rcText;

			pWnd->ReleaseDC( pdc );
			
			int bottomY = m_rectImage.bottom;

			if( !m_rectEditableIdio.IsRectEmpty() )
				bottomY = m_rectEditableIdio.bottom;
			else if( !m_rectIdio.IsRectEmpty() )
				bottomY = m_rectIdio.bottom;


			m_rectFull.bottom = bottomY + rcText.Height() + 3;

			m_rectText.OffsetRect( 0, m_rectFull.bottom - rcText.Height() - 3 );

			m_sizeTotal.cy = m_rectFull.Height();

			if( rcText.Width() > m_rectFull.Width() )
			{
				int dx = rcText.Width() - m_rectFull.Width();

				m_rectImage.OffsetRect( dx / 2, 0 );
				m_rectCGHGraph.OffsetRect( dx / 2, 0 );
				m_rectIdio.OffsetRect( dx / 2, 0 );
				m_rectCGHRatio.OffsetRect( dx / 2, 0 );
				m_rectEditableIdio.OffsetRect( dx / 2, 0 );

				m_sizeTotal.cx += dx;
			}
		}
	}
//********** For Idio Edit 
																				  
 	if( pImpl->m_bShowAdditionLines && pImpl->m_bShowEditableIdio )
	{
		FRECT *rc_Tmp = new FRECT[m_nLinesCount];
		long lSz = 0;

		IIdioBendsPtr sptrBend  = m_sptrEditableIdio;
		if( sptrBend )
		{
			double l = 0, c = 0;

			IIdiogramPtr sptrIdio = sptrBend;

			sptrIdio->get_params( &l, &c );

			long lCount = 0;
			sptrBend->GetBengsCount( &lCount );

			long lSize = 0;

			BOOL bIsManual = true;
			m_ptrChromos->GetAxisSize( AT_Mirror|AT_Rotated|AT_VisAngle|AT_LongAxis | ( m_bErected ? 0 : AT_Original ), &lSize, &bIsManual );

			FPOINT* arPt = new FPOINT[lSize];
  			m_ptrChromos->GetAxis( AT_Mirror|AT_AxisDouble|AT_Rotated|AT_VisAngle|AT_LongAxis | ( m_bErected ? 0 : AT_Original ), (POINT *)arPt );

			ROTATEPARAMS RotateParams = {0};
  			m_ptrChromos->GetRotateParams( &RotateParams );
			
			BOOL bMirror = false;
			m_ptrChromos->IsMirrorV( &bMirror );

   			if( !bMirror )
			{
				if( arPt[0].y > arPt[lSize - 1].y )
					bVMirror = !bVMirror;
			}
			else
			{
				if( !m_bErected )
				{
					if( arPt[0].y < arPt[lSize - 1].y )
						bVMirror = !bVMirror;
				}
				else
				{
					if( arPt[0].y > arPt[lSize - 1].y )
						bVMirror = !bVMirror;
				}
			}

			bool bCentr = false;
			for( long i = 0; i < lCount; i++ )
			{
  				double p1 = 0, p2 = 0;
				sptrBend->GetBendRange( i, &p1, &p2 );

				double _p1 = 0, _p2 = 0;
				if( i > 0 )
 					sptrBend->GetBendRange( i - 1, &_p1, &_p2 );

				int nFlag = 0;
				int nFlag2 = 0;
				
				sptrBend->GetBendFlag( i, &nFlag );

 				if( nFlag == IDIB_CENTROMERE )
					bCentr = true;

 				double n2  = p2 * m_nLinesCount + ( m_bErected ? (!bCentr ? 0 : -0.5) : 0 );
				double n1  = p1 * m_nLinesCount + ( m_bErected ? (!bCentr ? 0 : -0.5) : 0 );

				if( bVMirror )
				{
 					n2  = m_nLinesCount * ( 1 - p2 ) + (!bCentr ? 0.5 : 1);
					n1  = m_nLinesCount * ( 1 - p1 ) + (!bCentr ? 0.5 : 1);
				}


				if( i + 1 < lCount )
					sptrBend->GetBendFlag( i + 1, &nFlag2 );

				if( nFlag2 != IDIB_CENTROMERE && p2 != 1 )
					rc_Tmp[lSz++] = m_strAddLines[int(n2)].m_ptsLines;

				if( _p2 != p1 && nFlag != IDIB_CENTROMERE && p1 != 0 )
					rc_Tmp[lSz++] = m_strAddLines[int(n1)].m_ptsLines;
			}

			if( arPt )
				delete []arPt;

			if( m_nLinesCount != lSz )
			{
				delete []m_strAddLines;
				m_strAddLines = new XAddLinesStruct[ lSz ];

				for( long z = 0; z < lSz; z++ )
					m_strAddLines[z].m_ptsLines = rc_Tmp[z];

				m_nLinesCount = lSz;
			}
			else if( lSz == 0 )
			{
				delete []m_strAddLines;
				m_strAddLines = 0;
				m_nLinesCount = lSz;
			}
		}

		delete []rc_Tmp;
	}
}

void chromo_object::_update_rects( int nChromoMinX, int nChromoMaxX, int nChromoMinY, int nChromoMaxY, bool bShowMainIdio, const CCarioImpl *pImpl )
{
/***************************************************************************************************/
	CRect *rcPart[] = { &m_rectImage, &m_rectCGHGraph, &m_rectIdio, &m_rectCGHRatio, &m_rectEditableIdio };
	int nTypes[]    = { 0,            1,			   2,           3,				 4             };
	int nPosID[]    = { pImpl->m_nPosID_C,   pImpl->m_nPosID_G,     pImpl->m_nPosID_I, pImpl->m_nPosID_R, pImpl->m_nPosID_E };

	int nSize = 0;
	int nUsed[4];
	
	if( pImpl->m_bShowChromosome )
		nUsed[nSize++] = 0;
	if( pImpl->m_bShowCGHGraph )
		nUsed[nSize++] = 1;
	if( m_bShowIdiogramm || bShowMainIdio )
		nUsed[nSize++] = 2;
	if( pImpl->m_bShowCGHRatio )
		nUsed[nSize++] = 3;
	if( pImpl->m_bShowEditableIdio )
		nUsed[nSize++] = 4;

	if( !nSize )
		return;

	for( int i = 0; i < nSize; i++ )
	{
		int nMin = nPosID[nUsed[i]]; 
		int nId = -1;
		for( int j = i; j < nSize; j++ )
		{
			if( nPosID[nUsed[j]] < nMin )
			{
				nMin = nPosID[nUsed[j]];
				nId = j;
			}
		}
		if( nId > 0 )
		{

			int nVal = nPosID[nUsed[nId]];
			nPosID[nUsed[nId]] = nPosID[nUsed[i]];
			nPosID[nUsed[i]] = nVal;

			nVal = nTypes[nUsed[nId]];
			nTypes[nUsed[nId]] = nTypes[nUsed[i]];
			nTypes[nUsed[i]] = nVal;
		}
	}

	nChromoMinX = min( nChromoMinX, rcPart[nTypes[nUsed[0]]]->left   );
	nChromoMaxX = max( nChromoMaxX, rcPart[nTypes[nUsed[0]]]->right  );
	nChromoMinY = min( nChromoMinY, rcPart[nTypes[nUsed[0]]]->top    );
	nChromoMaxY = max( nChromoMaxY, rcPart[nTypes[nUsed[0]]]->bottom );

	CRect rcOld = *rcPart[nTypes[nUsed[0]]];
	for( i = 1; i < nSize; i++ )
	{
		int nType = nTypes[nUsed[i]];
		CRect &rc = *rcPart[nType];

		if( !rc.IsRectEmpty() )
		{
			rc.OffsetRect( INNER_INT + rcOld.right, 0 );

			nChromoMinX = min( nChromoMinX, rc.left );
			nChromoMaxX = max( nChromoMaxX, rc.right );
			nChromoMinY = min( nChromoMinY, rc.top );
			nChromoMaxY = max( nChromoMaxY, rc.bottom );
			rcOld = rc;
		}
	}
/***************************************************************************************************/


	//теперь переводим все в координаты объекта и считаем общий размер
	nChromoMinX -= CROMO_OFFSET_X;
	nChromoMaxX += CROMO_OFFSET_X;
	nChromoMinY -= CROMO_OFFSET;
	nChromoMaxY += CROMO_OFFSET;

	int w = m_rectChromo.Width();
	m_rectChromo.left  = m_rectImage.left;
	m_rectChromo.right = m_rectChromo.left + w;


	m_rectIdio.OffsetRect( -nChromoMinX, -nChromoMinY );
	m_rectChromo.OffsetRect( -nChromoMinX, -nChromoMinY );
	m_rectImage.OffsetRect( -nChromoMinX, -nChromoMinY );
	m_rectCGHGraph.OffsetRect( -nChromoMinX, -nChromoMinY );
	m_rectCGHRatio.OffsetRect( -nChromoMinX, -nChromoMinY );
	m_rectEditableIdio.OffsetRect( -nChromoMinX, -nChromoMinY );

	m_nCentromereY-=nChromoMinY;
	m_sizeTotal.cx = nChromoMaxX-nChromoMinX;
	m_sizeTotal.cy = nChromoMaxY-nChromoMinY;

	m_ptOffset = CPoint( 0, 0 );
}


void chromo_object::init_cgh_graph( const CCarioImpl *pImpl ) 
{
	IUnknown *punk = 0;
	::CoCreateInstance( CLSID_ChartDrawer, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk );

	m_sptrChart = punk;

	if( m_sptrChart == 0 )
		return;

	if( punk )
		punk->Release();

	IChartAttributesPtr ptrChart = m_sptrChart;
	//ptrChart->SetStyle( cfAbsolute );
	ptrChart->SetChartColor( 0, pImpl->m_clBright, ccfChartColor );

	//ptrChart->SetChartColor( 10, pImpl->m_clBrightBorder, ccfChartColor ); // --- up 
	//ptrChart->SetChartColor( 20, pImpl->m_clBrightBorder, ccfChartColor ); // --- down

	ptrChart->SetChartColor( 1, pImpl->m_clBright2, ccfChartColor );
}

void chromo_object::init_cgh_ratio( const CCarioImpl *pImpl )
{
	IUnknown *punk = 0;
	::CoCreateInstance( CLSID_ChartDrawer, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk );

	m_sptrRatio = punk;

	if( m_sptrRatio == 0 )
		return;

	if( punk )
		punk->Release();

	IChartAttributesPtr ptrChart = m_sptrRatio;


	ptrChart->SetChartColor( 0, pImpl->m_clRatioMid, ccfChartColor );

	ptrChart->SetChartColor( 1, pImpl->m_clRatioMin,   ccfChartColor );

	ptrChart->SetChartColor( 2, pImpl->m_clRatioMax,   ccfChartColor );
	ptrChart->SetChartColor( 3, pImpl->m_clRatioMid, ccfChartColor );

	ptrChart->SetChartColor( 10, pImpl->m_clRatioBorder, ccfChartColor ); // --- up 
	ptrChart->SetChartColor( 20, pImpl->m_clRatioBorder, ccfChartColor ); // --- down


	if( pImpl->m_bShowProb )
	{	
		ptrChart->SetChartColor( 1000, pImpl->m_clRatioProb, ccfChartColor );
		ptrChart->SetChartColor( 1001, pImpl->m_clRatioProb, ccfChartColor );
	}
}


double chromo_object::_calc_cgh_value( const IImage3Ptr &ptrImage, const FPoint &pt1, const FPoint &pt2, const int &nPaneID, FPOINT *ret_pt1, FPOINT *ret_pt2 )
{
	double fVal = 0;
	double fVal2 = 0;
	int	nSteps = max( abs( pt2.y - pt1.y ), abs( pt2.x - pt1.x ) );

	int cxI, cyI;
	ptrImage->GetSize( &cxI, &cyI );

	int nCount = 0;
	bool bStart = false, bEnd = false;


	double fBackColor = 0;

	for( int y = 0; y < cyI; y++  )
	{
		BYTE *byte = 0;
		ptrImage->GetRowMask( y, &byte );

		color *clrs = 0;
		ptrImage->GetRow( y, nPaneID, &clrs );
		for( int x = 0; x < cxI; x++  )
		{
			if( byte[ x ] < 128 )
			{
				fBackColor += clrs[ x ];
				nCount++;
			}
		}
	}

	if( nCount )
		fBackColor /= nCount;

	nCount = 0;
	double _nx, _ny = 0;
	for( int i = 0; i < nSteps; i++ )
	{
		double	fx = pt1.x + ( pt2.x - pt1.x ) / nSteps * i;
		double	fy = pt1.y + ( pt2.y - pt1.y ) / nSteps * i;

		double nx = double( fx ), ny = double( fy );
		_nx = nx;
		_ny = ny;

		if( nx >= 0 && nx < cxI && ny >= 0 && ny < cyI )
		{
			BYTE *byte = 0;
			ptrImage->GetRowMask( ny, &byte );

			if( byte[ int(nx) ] >= 128 )
			{
				if( !bStart && ret_pt1 )
				{
					bStart = true;
					ret_pt1->x = nx;
					ret_pt1->y = ny;
				}

				color *clrs = 0;
				ptrImage->GetRow( ny, nPaneID, &clrs );

				fVal += fabs( fBackColor - clrs[ int(nx) ] ) * clrs[ int(nx) ];
				fVal2 += fabs( fBackColor - clrs[ int(nx) ] );
				nCount++;
			}
			else if( bStart && !bEnd && ret_pt2 )
			{
				bEnd = true;
				ret_pt2->x = nx;
				ret_pt2->y = ny;
			}
		}
		else if( bStart && !bEnd && ret_pt2 )
		{
			bEnd = true;
			ret_pt2->x = nx;
			ret_pt2->y = ny;
		}
	}

	if( bStart && !bEnd && ret_pt2 )
	{
		bEnd = true;
		ret_pt2->x = _nx;
		ret_pt2->y = _ny;
	}

	if( fVal2 == 0 )
		return -1;
	
	return fVal / fVal2;
}

/////////////////////////////////////////////////////////////////////////////
void chromo_object::calculate_offsets( int nLine, AlignChromo ac )
{
	m_ptOffset	= CPoint( 0, 0 );

	if( ac == acTop )
	{
		m_ptOffset.y	= nLine;
	}
	else if( ac == acCenterLine )
	{
		m_ptOffset.y	= nLine - m_nCentromereY;
	}
	else if( ac == acBottom )
	{
		m_ptOffset.y	= nLine - m_sizeTotal.cy;
	}
}


/////////////////////////////////////////////////////////////////////////////
//	class chromo_cell
chromo_cell::chromo_cell()
{
	init_defaults();
}

/////////////////////////////////////////////////////////////////////////////
chromo_cell::~chromo_cell()
{
	deinit();
}

/////////////////////////////////////////////////////////////////////////////
void chromo_cell::init_defaults()
{
	m_nCellWidth		= MIN_CELL_WIDTH;
	m_nCellOffset		= 0;
	m_nClassNum			= -1;
	m_rectCell.SetRectEmpty();
}

/////////////////////////////////////////////////////////////////////////////
void chromo_cell::deinit()
{
	for( int i=0;i<m_arChromos.GetSize();i++ )
		delete m_arChromos[i];

	m_arChromos.RemoveAll();
	
	init_defaults();
}

/////////////////////////////////////////////////////////////////////////////
void chromo_cell::load( CStreamEx& stream )
{
	deinit();

	long lVersion = 1;		
	stream >> lVersion;
	stream >> m_nCellWidth;
	stream >> m_nCellOffset;
	stream >> m_nClassNum;

	long lCount = 0;
	stream >> lCount;

	for( long i=0;i<lCount;i++ )
	{
		chromo_object* pinfo = new chromo_object;
		m_arChromos.Add( pinfo );
		pinfo->load( stream );
	}
}

/////////////////////////////////////////////////////////////////////////////
void chromo_cell::save( CStreamEx& stream )
{
	long lVersion = 1;
	stream << lVersion;
	stream << m_nCellWidth;
	stream << m_nCellOffset;
	stream << m_nClassNum;
	
	long lCount = m_arChromos.GetSize();
	stream << (long)lCount;

	for( long i=0;i<lCount;i++ )
	{
		chromo_object* pinfo = m_arChromos[i];			
		pinfo->save( stream );
	}
}

/////////////////////////////////////////////////////////////////////////////
void chromo_cell::save_to_named_data( INamedData* pINamedData, CString strSection )
{
	if( !pINamedData )
		return;

	pINamedData->SetupSection( _bstr_t( strSection ) );		
	pINamedData->SetValue( _bstr_t( CARIO_WIDTH_PREFIX ), _variant_t( (long) m_nCellWidth ) );
	pINamedData->SetValue( _bstr_t( CARIO_OFFSETX_PREFIX ), _variant_t( (long) m_nCellOffset ) );
	pINamedData->SetValue( _bstr_t( CARIO_CLASS_NUM_PREFIX ), _variant_t( (long) m_nClassNum ) );

	if( ::GetObjectKey( ::GetAppUnknown() ) == ::GetObjectKey( pINamedData ) )
		return;

	CString strFormat;
	strFormat.Format( "%s\\%s", strSection, CARIO_CHROMOS_PREFIX );
	pINamedData->SetupSection( _bstr_t( strFormat ) );		

	long lCount = m_arChromos.GetSize();
	pINamedData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( lCount ) );
	for( int i=0;i<lCount;i++ )
	{		
		strFormat.Format( "%s\\%s\\%s%d", strSection, CARIO_CHROMOS_PREFIX, CARIO_CHROMO_PREFIX, i );
		chromo_object* pobject = m_arChromos[i];
		pobject->save_to_named_data( pINamedData, strFormat );
	}		
}

/////////////////////////////////////////////////////////////////////////////
void chromo_cell::load_from_named_data( INamedData* pINamedData, CString strSection, CCarioImpl *pImpl )
{
	deinit();

	if( !pINamedData )
		return;

	pINamedData->SetupSection( _bstr_t( strSection ) );		

	_variant_t var;

	pINamedData->GetValue( _bstr_t( CARIO_WIDTH_PREFIX ), &var );
	if( var.vt == VT_I4 )
		m_nCellWidth = var.lVal;

	var.Clear();

	pINamedData->GetValue( _bstr_t( CARIO_OFFSETX_PREFIX ), &var );
	if( var.vt == VT_I4 )
		m_nCellOffset = var.lVal;

	var.Clear();
	
	pINamedData->GetValue( _bstr_t( CARIO_CLASS_NUM_PREFIX ), &var );
	if( var.vt == VT_I4 )
		m_nClassNum = var.lVal;

	var.Clear();


	if( ::GetObjectKey( ::GetAppUnknown() ) == ::GetObjectKey( pINamedData ) )
		return;


	CString strFormat;
	strFormat.Format( "%s\\%s", strSection, CARIO_CHROMOS_PREFIX );
	pINamedData->SetupSection( _bstr_t( strFormat ) );		

	long lCount = 0;

	pINamedData->GetValue( _bstr_t( CARIO_COUNT_PREFIX ), &var );
	if( var.vt == VT_I4 )
		lCount = var.lVal;

	for( int i=0;i<lCount;i++ )
	{		
		strFormat.Format( "%s\\%s\\%s%d", strSection, CARIO_CHROMOS_PREFIX, CARIO_CHROMO_PREFIX, i );
		chromo_object* pobject = new chromo_object;
		m_arChromos.Add( pobject );
		pobject->load_from_named_data( pINamedData, strFormat, pImpl );
	}		
}


/////////////////////////////////////////////////////////////////////////////
//	class chromo_line
chromo_line::chromo_line()
{
	init_defaults();	

}	

/////////////////////////////////////////////////////////////////////////////
chromo_line::~chromo_line()
{
	deinit();
}

/////////////////////////////////////////////////////////////////////////////
void chromo_line::init_defaults()
{
	m_nLineHeight		= MIN_LINE_HEIGHT;
	m_bCellar			= false;
	m_nCenterLine		= 0;
	m_nCaptionHeight	= 0;
	m_rectLine.SetRectEmpty();
}

/////////////////////////////////////////////////////////////////////////////
void chromo_line::deinit()
{
	for( int i=0;i<m_arCell.GetSize();i++ )
		delete m_arCell[i];

	m_arCell.RemoveAll();

	init_defaults();
}

/////////////////////////////////////////////////////////////////////////////
void chromo_line::load( CStreamEx& stream )
{
	deinit();

	long lVersion = 1;
	stream >> lVersion;
	stream >> m_nLineHeight;
	stream >> m_bCellar;

	long lCount = 0;
	stream >> lCount;

	for( long i=0;i<lCount;i++ )
	{
		chromo_cell* pcell = new chromo_cell;
		m_arCell.Add( pcell );
		pcell->load( stream );
	}
}

/////////////////////////////////////////////////////////////////////////////
void chromo_line::save( CStreamEx& stream )
{
	long lVersion = 1;
	stream << lVersion;
	stream << m_nLineHeight;
	stream >> m_bCellar;

	stream << (long)m_arCell.GetSize();

	for( int i=0;i<m_arCell.GetSize();i++ )
	{
		chromo_cell* pcell = m_arCell[i];
		pcell->save( stream );
	}		
}

/////////////////////////////////////////////////////////////////////////////
void chromo_line::save_to_named_data( INamedData* pINamedData, CString strSection )
{
	if( !pINamedData )
		return;
	
	pINamedData->SetupSection( _bstr_t( strSection ) );		
	pINamedData->SetValue( _bstr_t( CARIO_HIGHT_PREFIX ), _variant_t( (long) m_nLineHeight ) );
	pINamedData->SetValue( _bstr_t( CARIO_CELLAR_PREFIX ), _variant_t( (long) ( m_bCellar ? 1 : 0 ) ) );

	CString strFormat;
	strFormat.Format( "%s\\%s", strSection, CARIO_CELLS_PREFIX );
	pINamedData->SetupSection( _bstr_t( strFormat ) );		

	long lCount = m_arCell.GetSize();
	pINamedData->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( lCount ) );
	for( int i=0;i<lCount;i++ )
	{		
		strFormat.Format( "%s\\%s\\%s%d", strSection, CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, i );
		chromo_cell* pcell = m_arCell[i];
		pcell->save_to_named_data( pINamedData, strFormat );
	}		
}

/////////////////////////////////////////////////////////////////////////////
void chromo_line::load_from_named_data( INamedData* pINamedData, CString strSection, CCarioImpl *pImpl )
{
	deinit();

	if( !pINamedData )
		return;
	
	pINamedData->SetupSection( _bstr_t( strSection ) );		

	_variant_t var;
	pINamedData->GetValue( _bstr_t( CARIO_HIGHT_PREFIX ), &var );
	if( var.vt == VT_I4 )
		m_nLineHeight = var.lVal;

	var.Clear();

	pINamedData->GetValue( _bstr_t( CARIO_CELLAR_PREFIX ), &var );
	if( var.vt == VT_I4 )
		m_bCellar = ( var.lVal == 1L );

	var.Clear();

	CString strFormat;
	strFormat.Format( "%s\\%s", strSection, CARIO_CELLS_PREFIX );

	pINamedData->SetupSection( _bstr_t( strFormat ) );		

	long lCount = 0;
	pINamedData->GetValue( _bstr_t( CARIO_COUNT_PREFIX ), &var );
	if( var.vt == VT_I4  )
		lCount = var.lVal;		
	
	for( int i=0;i<lCount;i++ )
	{		
		strFormat.Format( "%s\\%s\\%s%d", strSection, CARIO_CELLS_PREFIX, CARIO_CELL_PREFIX, i );
		chromo_cell* pcell = new chromo_cell;
		m_arCell.Add( pcell );
		pcell->load_from_named_data( pINamedData, strFormat, pImpl );
	}		
}


/////////////////////////////////////////////////////////////////////////////
//	class CCarioImpl
CCarioImpl::CCarioImpl()
{	
	m_bFromPrint = false;

	m_bSkipNotify = false;
	m_bErectAfterDrop = 1;
	m_nShowNums = 0;
	m_nShowEditableNums = 0;
	m_bActionExecuted = false;
	m_bShowCellIdio = false;
	m_bLockObjectUpdate = false;
	//mouse
	m_mouseMode				= mmNone;

	m_ptMousePrevPos		= CPoint( 0, 0 );
	m_ptOffset				= CPoint( 0, 0 );	

	//activity
	m_nActiveLine			= -1;
	m_nActiveCell			= -1;
	m_nActiveChromo			= -1;

	//cursors
	m_hCursorArrow		= ::LoadCursor( 0, IDC_ARROW );
	m_hCursorVertSize	= ::LoadCursor( 0, IDC_SIZENS );
	m_hCursorHorzSize	= ::LoadCursor( 0, IDC_SIZEWE );
	m_hCursorMoveCell	= ::LoadCursor( 0, IDC_SIZEALL );
	m_hCursorMoveChromo	= 0;
	if( AfxGetApp() )
		m_hCursorMoveChromo = AfxGetApp()->LoadCursor( IDC_MOVE_CROMO );

	::ZeroMemory( &m_lfClassText, sizeof(LOGFONT) );
	m_lfClassText.lfHeight = 16;
	m_lfClassText.lfCharSet = 1;
	strcpy( m_lfClassText.lfFaceName, "Tahoma" );
	m_clrClassText = RGB( 0, 0, 128 );

	m_lpRatioText = m_lfClassText;
	m_lpRatioText.lfHeight = 10;


	//to avoid some build on activate object and on SetMode call
	m_carioViewMode		= cvmNotDefine;

	m_bCanSaveToObjectAndShellData	= true;
	m_bCanFireEventDataChange		= true;
	m_bHasBeenChange				= false;

	m_ptCachedScrollPos = CPoint( 0, 0 );
	m_fCachedZoom = 1;

	m_bNeedChromoAlignLine = true;
	m_sizeClient = CSize(0,0);	

	m_clBright = m_clRatioMin = RGB( 255, 0, 0 );
	m_clBright2 = RGB( 0, 170, 255 );

	m_clRatioMax = RGB( 0, 255, 0 );
	m_clRatioMid = RGB( 0, 0, 255 );

	m_clBrightBorder  = 0;
	m_clBrightBorder2  = RGB(255,255,255);

	m_clGradient = 0;
	m_clGradient2 = RGB( 255, 255, 255 );
	
	m_bShowGradient = true;

	m_clRatioBorder   = 0;
	m_clRatioGrad = RGB( 100, 100, 100 );
	m_clRatioProb = 0;
	m_clAddLinesColor = 0;

	m_fNormalMin = 0.75;
	m_fNormalMid = 1;
	m_fNormalMax = 1.25;
	m_fRatioStepVal = 0.25;

	::ZeroMemory( &m_guidExclude, sizeof( GUID ) );

	m_CGHGRAPH_CX = 25;
	m_CGHRATIO_CX = 25;
	m_CGHAMPL_CX  = 5;
	m_fProbability = 0.99;

	m_bShowProb = true;
	m_bShowLineOnRatio = true;
	m_fMinRatioVal = 0;
	m_fMaxRatioVal = 2;
	m_bUseAutoScaleRatio = false;
	m_bShowClasses = true;

	m_bShowEditableIdio = false;
	m_bShowChromosome = true;

	m_bShowCGHRatio		= false;
	m_bShowCGHAmpl		= false;

	m_bShowCGHGraph		= false;
	m_bShowAdditionLines = false;

	
	m_nRatioTextRCCount = 2;

	m_nPosID_C = 0;
	m_nPosID_G = 2;
	m_nPosID_I = 1;
	m_nPosID_R = 3;
	m_nPosID_E = 4;

	reload_view_settings( GetAppUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
CCarioImpl::~CCarioImpl()
{
	if( m_hCursorArrow )
		::DestroyCursor( m_hCursorArrow );		m_hCursorArrow = 0;

	if( m_hCursorVertSize )
		::DestroyCursor( m_hCursorVertSize );	m_hCursorVertSize = 0;

	if( m_hCursorHorzSize )
		::DestroyCursor( m_hCursorHorzSize );	m_hCursorHorzSize = 0;
	
	if( m_hCursorMoveCell )
		::DestroyCursor( m_hCursorMoveCell );	m_hCursorMoveCell = 0;
	
	if( m_hCursorMoveChromo )
		::DestroyCursor( m_hCursorMoveChromo );	m_hCursorMoveChromo = 0;

	_kill_cario_array();
}

IMPLEMENT_UNKNOWN_BASE(CCarioImpl, CarioObject);

/////////////////////////////////////////////////////////////////////////////
CarioViewMode CCarioImpl::GetCarioViewMode()
{
	return m_carioViewMode;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::is_printing()
{
	return CheckInterface( GetViewPtr(), IID_IViewCtrl );
}

/////////////////////////////////////////////////////////////////////////////
chromo_line* CCarioImpl::_get_cellar()
{
	chromo_line* pcellar = 0;
	int nCellarIdx = _get_cellar_idx();

	
	if( nCellarIdx >= 0 )
	{
		pcellar = m_arCarioLine[nCellarIdx];		
	}
	else
	{
		pcellar = new chromo_line;
		pcellar->init_defaults();
		pcellar->m_nLineHeight = m_sizeDefaultLine.cy;
		pcellar->m_bCellar = true;

		m_arCarioLine.Add( pcellar );
	}

	if( pcellar->m_arCell.GetSize() == 0 )
	{
		chromo_cell* pcell = new chromo_cell;
		pcell->m_nCellWidth = m_sizeDefaultLine.cx;
		pcellar->m_arCell.Add( pcell );
	}

	return pcellar;	
}

/////////////////////////////////////////////////////////////////////////////
int	CCarioImpl::_get_cellar_idx()
{
	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		if( m_arCarioLine[i]->m_bCellar )
			return i;
	}

	return -1;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::_kill_cario_array()
{
	for( int i=0;i<m_arCarioLine.GetSize();i++ )
		delete m_arCarioLine[i];

	m_arCarioLine.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::init_chromo_by_measure( chromo_object *pchromo, IUnknown *punkMeasure )
{
	pchromo->m_ptrMeasureObject = punkMeasure;

	if( GetCarioViewMode() == cvmCellar && m_bShowEditableIdio && pchromo->m_sptrEditableIdio == 0 )
	{
		IUnknownPtr sptrI = _get_editable_idio( pchromo->m_ptrMeasureObject );
		if( sptrI == 0 )
		{
			IUnknown	*punkIdio = CreateTypedObject( _bstr_t( sz_idio_type ) );
			m_bSkipNotify = true;
			if( punkIdio )
			{
				IIdioBendsPtr ptrBends = punkIdio;
				punkIdio->Release();

				pchromo->m_sptrEditableIdio = ptrBends;
				pchromo->m_sptrEditableIdio->set_flags( IDIT_EDITABLE );

				INamedDataObject2Ptr	ptrND = ptrBends;
				ptrND->SetParent( pchromo->m_ptrMeasureObject, 0 );

			}
			m_bSkipNotify = false;
		}
		else
			pchromo->m_sptrEditableIdio = sptrI;
	}

	pchromo->m_ptrChromos = get_chromo_object_from_measure( pchromo->m_ptrMeasureObject );
	pchromo->m_ptrIdio = get_idio_object_from_measure( pchromo->m_ptrMeasureObject );
	pchromo->m_sptrEditableIdio = _get_editable_idio( pchromo->m_ptrMeasureObject );

	m_bShowChromosome &= pchromo->m_ptrChromos != 0;
//	pchromo->m_bShowIdiogramm &= pchromo->m_ptrIdio != 0;

	pchromo->m_guidMeasureObject = ::GetObjectKey( punkMeasure );

	return true;//pchromo->m_bShowChromosome || pchromo->m_bShowIdiogramm;
}

struct _object
{
	IUnknownPtr	punk;
	GUID		guid;
};

bool CCarioImpl::synchronize_with_object_list( IUnknown* punkObjectList )
{
	if( !is_correct_object_list( punkObjectList ) )
		return false;
	CarioViewMode cvm = GetCarioViewMode();

//1. Для начала соберем все объекты - потенциальные хромосомы в список
	_list_t<_object*>	object_list;


	INamedDataObject2Ptr ptrNDO2( punkObjectList );
	if( ptrNDO2 == 0 )return true;
	long	lpos;
	ptrNDO2->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown	*punk = 0;
		ptrNDO2->GetNextChild( &lpos, &punk );

		if( punk )
		{
			_object	*p = new _object;
			p->punk = punk;
			p->guid = ::GetObjectKey( punk );
			punk->Release();

			object_list.insert_before( p, 0 );
		}
	}

//2. теперь ищем объекты, соотв chromo_object
	long	i, j, k;
	for( i = 0; i < m_arCarioLine.GetSize(); i++ )
	{
		chromo_line	*pline = m_arCarioLine[i];
		for( j = 0; j < pline->m_arCell.GetSize(); j++ )
		{
			chromo_cell	*pcell = pline->m_arCell[j];
			for( k=pcell->m_arChromos.GetSize()-1; k>=0 ; k-- )
			{
				chromo_object	*pchromo = pcell->m_arChromos[k];
				_object	*p = 0;

				//для начала найдем подходящий объект в активном списке объектов.
				for( lpos = object_list.head(); lpos; lpos = object_list.next( lpos ) )
				{
					p = object_list.get( lpos );

					//[29.05.2002]
					//проверяем, что объект нормально отнесен к классу
					long lclass = get_object_class( ICalcObjectPtr( p->punk ) );
					if( lclass != pcell->m_nClassNum && pcell->m_nClassNum != -1 )
						continue;

					if( m_carioViewMode != cvmCellar && lclass != -1 && pcell->m_nClassNum == -1 )
						continue;

					if( p->guid == pchromo->m_guidMeasureObject )
						break;
					{
					//если объект плохо проинитился, ставим флаг чтобы его снести
						
						
						
						
					}
				}

				//нифига не нашли - удаляем chromo_object
				if( !lpos )
				{
					pcell->m_arChromos.RemoveAt( k );
					delete pchromo;
					continue;
				}

				//непорядок - указатель на большой объект = 0
				if( pchromo->m_ptrMeasureObject == 0 )
				{
					if( !init_chromo_by_measure( pchromo, p->punk ) )
					//плохо проинитился - удаляем chromo_object
					{
						pcell->m_arChromos.RemoveAt( k );
						delete pchromo;
					}
				}
				else //на этом месте в ячейке уже сидит объект, его надо снести из списка 
					//неиспользованных объектов
				{
					
				}

				object_list.remove( lpos );
				delete p;
			}
		}
	}

//3. на этот момент все оставшиеся объекты не имеют chromo_object-ов
//их просто относим к ячейкам по классам
//этого нельзя делать в режиме анализа
	if( m_carioViewMode != cvmCellar )
		for( lpos = object_list.head(); lpos; lpos = object_list.next( lpos ) )
		{
 			_object	*p = object_list.get( lpos );
			update_chromosome( p->punk, CHROMO_OBJECT_ADD );
			delete p;
		}
	else
		for( lpos = object_list.head(); lpos; lpos = object_list.next( lpos ) )
			delete object_list.get( lpos );

	object_list.clear();


	ptrNDO2->GetActiveChild( &lpos );

	if( lpos )
	{
		IUnknown	*punk = 0;
		ptrNDO2->GetNextChild( &lpos, &punk );

		if( punk )
		{
			on_activate_object( punk );
			punk->Release();
		}
	}
	

	//какая-то фигня на тему подвала - это уже не я писал
	chromo_cell* pcell_cellar = 0;

	chromo_line* pcellar = _get_cellar();
	if( pcellar->m_arCell.GetSize() > 0 )
		pcell_cellar = pcellar->m_arCell[0];

	if( ( is_printing() && GetCarioViewMode() != cvmCellar &&
		pcell_cellar && pcell_cellar->m_arChromos.GetSize() == 0 ) || 
		GetCarioViewMode() == cvmMenu )
	{
		int nCellarIdx = _get_cellar_idx();
		m_arCarioLine.RemoveAt( nCellarIdx );

		delete pcellar;	pcellar = 0;
	}


	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::fill_cromo_array_from_object_list( IUnknown* punkObjectList, CArray<IUnknown*, IUnknown*>& arCromoObj )
{
	INamedDataObject2Ptr ptrNDO2( punkObjectList );
	if( ptrNDO2 == 0 )
		return;

	long lPosChild = 0;
	ptrNDO2->GetFirstChildPosition( &lPosChild );
	while( lPosChild ) 
	{
		IUnknown* punkChild = 0;
		ptrNDO2->GetNextChild( &lPosChild, &punkChild );
		if( !punkChild )
			continue;

		INamedDataObject2Ptr ptrNDOCromoParent( punkChild );
		punkChild->Release();	punkChild = 0;
		if( ptrNDOCromoParent == 0 )
			continue;

		long lPosCromo = 0;
		ptrNDOCromoParent->GetFirstChildPosition( &lPosCromo );
		while( lPosCromo )
		{
			IUnknown* punkChromo = 0;
			ptrNDOCromoParent->GetNextChild( &lPosCromo, &punkChromo );
			if( !punkChromo )
				continue;

			if (::CheckInterface( punkChromo, IID_IChromosome ) )
				arCromoObj.Add( punkChromo );			

			punkChromo->Release();	punkChromo = 0;
		}
		
	}
}

/////////////////////////////////////////////////////////////////////////////
CPoint CCarioImpl::convert_from_window_to_object( CPoint point )
{	
	VERIFY( GetViewPtr() != 0 );

	CPoint ptClient = ConvertToClient( GetViewPtr(), point );

	CPoint ptObject = ptClient;

	return ptObject;
}

/////////////////////////////////////////////////////////////////////////////
/*CRect CCarioImpl::convert_from_obj_to_client( CRect rc )
{
	double fZoom = get_zoom( );
	CRect	_rcZoom = rc;
	
	_rcZoom.left	= _rcZoom.left * fZoom;
	_rcZoom.top		= _rcZoom.top * fZoom;
	_rcZoom.right	= _rcZoom.right * fZoom;
	_rcZoom.bottom	= _rcZoom.bottom * fZoom;

	return _rcZoom;	
}*/

/////////////////////////////////////////////////////////////////////////////
/*CPoint CCarioImpl::convert_from_obj_to_client( CPoint pt )
{
	double fZoom = get_zoom( );
	CPoint _ptZoom;
	
	_ptZoom.x = pt.x * fZoom;
	_ptZoom.y = pt.y * fZoom;

	return _ptZoom;	
}*/

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::update_zoom_scroll()
{
	IScrollZoomSitePtr ptrSZS( GetViewPtr() );
	if( ptrSZS == 0 )return;

	ptrSZS->GetScrollPos( &m_ptCachedScrollPos );
	ptrSZS->GetZoom( &m_fCachedZoom );
}
/////////////////////////////////////////////////////////////////////////////
CRect CCarioImpl::convert_from_wnd_to_object( CRect rc )
{
	CPoint ptScroll = get_scroll_pos();
	double fZoom = get_zoom();

	CRect rcObj;
	rcObj.left		= ( ptScroll.x + rc.left ) / fZoom;
	rcObj.top		= ( ptScroll.y + rc.top ) / fZoom;
	rcObj.right		= rcObj.left + rc.Width() / fZoom;
	rcObj.bottom	= rcObj.top + rc.Height() / fZoom;

	return rcObj;
}

CPoint CCarioImpl::convert_from_wnd_to_object( CPoint pt )
{
	CPoint ptScroll = get_scroll_pos();
	double fZoom = get_zoom();

	CPoint	ptObj;
	ptObj.x		= ( ptScroll.x + pt.x ) / fZoom;
	ptObj.y		= ( ptScroll.y + pt.y ) / fZoom;

	return ptObj;
}

/////////////////////////////////////////////////////////////////////////////
CRect CCarioImpl::convert_from_object_to_wnd( CRect rc )
{
	CPoint ptScroll = get_scroll_pos();
	double fZoom = get_zoom();

	CRect rcWnd;

	rcWnd.left		= rc.left * fZoom - ptScroll.x;
	rcWnd.top		= rc.top * fZoom - ptScroll.y;
	rcWnd.right		= rcWnd.left + rc.Width() * fZoom;
	rcWnd.bottom	= rcWnd.top + rc.Height() * fZoom;

	return rcWnd;
}

CPoint CCarioImpl::convert_from_object_to_wnd( CPoint pt )
{
	CPoint ptScroll = get_scroll_pos();
	double fZoom = get_zoom();

	CPoint	ptWnd;

	ptWnd.x		= pt.x * fZoom - ptScroll.x;
	ptWnd.y		= pt.y * fZoom - ptScroll.y;

	return ptWnd;
}


//occuped area
/////////////////////////////////////////////////////////////////////////////
CRect CCarioImpl::get_chromo_line_rect( int nLineIdx, chromo_line** ppline, bool bAddCaptionHeight )
{
	if( nLineIdx < 0 || nLineIdx >= m_arCarioLine.GetSize() )
		return NORECT;

	chromo_line* pline = m_arCarioLine[nLineIdx];
	if( ppline )*ppline = pline;

	CRect	rect = pline->m_rectLine;
	if( !bAddCaptionHeight )rect.bottom -= pline->m_nCaptionHeight;
	return rect;
}


/////////////////////////////////////////////////////////////////////////////
CRect CCarioImpl::get_chromo_cell_rect(	int nLineIdx, int nCellIdx,
											chromo_line** ppline, chromo_cell** ppcell,
											bool bAddCaptionHeight )
{
	chromo_line* pline = 0;
	CRect rcLine = get_chromo_line_rect( nLineIdx, &pline, bAddCaptionHeight );
	if( !pline )return NORECT;

	if( ppline )*ppline = pline;

	if( nCellIdx  < 0 || nCellIdx >= pline->m_arCell.GetSize() )
		return NORECT;

	chromo_cell* pcell = pline->m_arCell[nCellIdx];
	if( ppcell )*ppcell = pcell;

	CRect	rectCell = pcell->m_rectCell;
	rectCell.top = rcLine.top;
	rectCell.bottom = rcLine.bottom;


	return rectCell;
}

/////////////////////////////////////////////////////////////////////////////
CRect CCarioImpl::get_chromo_object_rect( int nLineIdx, int nCellIdx, int nObjectIdx,
											chromo_line** ppline, chromo_cell** ppcell, chromo_object** ppobject )
{
	chromo_line* pline = 0;
	chromo_cell* pcell = 0;
	CRect rcCell = get_chromo_cell_rect( nLineIdx, nCellIdx, &pline, &pcell );

	if( !pline || !pcell )return NORECT;

	if( ppline )*ppline = pline;
	if( ppcell )*ppcell = pcell;

	if( nObjectIdx < 0 || nObjectIdx >= pcell->m_arChromos.GetSize() )
		return NORECT;

	chromo_object* pobject = pcell->m_arChromos[nObjectIdx];
	
	if( ppobject )*ppobject = pobject;

	CRect	rect( pobject->m_ptOffset, pobject->m_sizeTotal );

	return rect;
}

/////////////////////////////////////////////////////////////////////////////
CSize CCarioImpl::get_occuped_size( IUnknown *punkExcludedObject /*= 0*/ )
{
	if( m_drawobjects )
	{
		RECT rc = { 0, 0, m_sizeClient.cx, m_sizeClient.cy };
		RECT rc2 = GetDrawingRect( m_drawobjects, punkExcludedObject );
		::UnionRect( &rc, &rc, &rc2 );

		m_sizeClient.cx = rc.right;
		m_sizeClient.cy = rc.bottom;

	}
	return	m_sizeClient;
}



/////////////////////////////////////////////////////////////////////////////
//	
//	ICarioObject interface
//	
/////////////////////////////////////////////////////////////////////////////
HRESULT CCarioImpl::XCarioObject::InitFromObjectList( IUnknown* punkObjectList )
{
	METHOD_PROLOGUE_BASE(CCarioImpl, CarioObject);
	return S_OK;
}

//serialization
/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::init_attached_object_named_data( IUnknown* punkObject )
{
	//only for Cario Build
	//return false; [paul 06.08.2002] what about drawing in cellar ???
	
	INamedDataObject2Ptr ptrNDO( punkObject );
	if( ptrNDO == 0 )
		return false;

	INamedDataPtr ptrND( punkObject );
	if( ptrND )
		return true;

	ptrNDO->InitAttachedData();
	ptrND = ptrNDO;

	return ( ptrND != 0 );
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::is_correct_object_list( IUnknown* punk )
{
	if( m_carioViewMode == cvmCellar || m_carioViewMode == cvmMenu )
		return true;

	INamedDataPtr ptrNDO( punk );
	if( ptrNDO == 0 )
		return false;

	long l = ::GetValueInt( ptrNDO, CARIO_ROOT, CARIO_WAS_BUILD, 0L );

	return  ( l == 1L );
}

/////////////////////////////////////////////////////////////////////////////
CString CCarioImpl::get_lines_section()
{
	CarioViewMode cm = GetCarioViewMode();
	if( cm == cvmPreview || cm == cvmDesign || cm == cvmMenu )
		return CARIO_PERVIEW;
	else if( cm == cvmCellar )
		return CARIO_ANALIZE;

	return "";
}


/////////////////////////////////////////////////////////////////////////////
CString	CCarioImpl::get_view_shared_section()
{
	return CARIO_VIEW_SHARED;
}

/////////////////////////////////////////////////////////////////////////////
CString	CCarioImpl::get_view_settings_section()
{
	CarioViewMode cm = GetCarioViewMode();
	if( cm == cvmPreview || cm == cvmDesign || cm == cvmMenu )
		return CARIO_VIEW_PREVIEW;
	else if( cm == cvmCellar )
		return CARIO_VIEW_ANALIZE;

	return CARIO_VIEW_PREVIEW;//cos somebody place reload_shell_data on CCarioImpl::CCarioImpl() - no mode here
}


/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::load_cario_state_from_data( IUnknown* punkNamedData )
{
	_kill_cario_array();

	CString strSection = get_lines_section();

	if( strSection.IsEmpty() )
		return false;	

	m_nActiveLine	= -1;
	m_nActiveCell	= -1;
	m_nActiveChromo	= -1;

	INamedDataPtr ptrND( punkNamedData );
	if( ptrND == 0 )
		return false;
	
	ptrND->SetupSection( _bstr_t( strSection ) );
	
	bool bres = false;
	_variant_t var;
	ptrND->GetValue( _bstr_t( CARIO_COUNT_PREFIX ), &var );
	if( var.vt == VT_I4 )
	{	
		long lCount = var.lVal;

		for( int i=0;i<lCount;i++ )
		{
			chromo_line* pline = new chromo_line;
			m_arCarioLine.Add( pline );
			CString strEntry;
			strEntry.Format( "%s\\%s%d", strSection, CARIO_LINE_PREFIX, i );
			pline->load_from_named_data( ptrND, strEntry, this );
		}

		bres = true;
	}


	ptrND->SetupSection( 0 );
	//_add_cellar();	

	return bres;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::save_cario_state_to_data( IUnknown* punkNamedData )
{
	CString strSection = get_lines_section();

	if( strSection.IsEmpty() )
		return false;

	if( CheckInterface( punkNamedData, IID_IApplication ) )
	{
		if( m_ptrActiveObjectList )//only if object list selected
		{
			if( GetClassFileNameFromObjectList( m_ptrActiveObjectList ) != GetCurrentClassFileName() )
				return true;
		}
	}
	else
	{
		IViewPtr ptrView = GetViewPtr();
		if( ptrView )
		{
			IUnknown* punk_doc = 0;
			ptrView->GetDocument( &punk_doc );
			if( punk_doc )
			{
				IFileDataObject2Ptr ptrFDO2( punk_doc );
				if( ptrFDO2 )	ptrFDO2->SetModifiedFlag( TRUE );
				punk_doc->Release();
			}
		}

	}

	//возможно, нам предлагают сохраниться в объект, который еще не проинитил свою дату,
	//то есть не была вызвана CarioBuild -  в этом случае пытаемся проинитить дату
	INamedDataPtr ptrND = punkNamedData;
	if( ptrND == 0 )
	{
		INamedDataObject2Ptr ptrNDObj = punkNamedData;
		if( ptrNDObj == 0 )return false;
		ptrNDObj->InitAttachedData();
		ptrND = ptrNDObj;

		if( ptrND == 0 )
			return false;
	}

	ptrND->DeleteEntry( _bstr_t( strSection ) );
	ptrND->SetupSection( _bstr_t( strSection ) );
	
	long lCount = m_arCarioLine.GetSize();
	ptrND->SetValue( _bstr_t( CARIO_COUNT_PREFIX ), _variant_t( lCount ) );

	for( int i=0;i<lCount;i++ )
	{
		chromo_line* pline = m_arCarioLine[i];
		CString strEntry;
		strEntry.Format( "%s\\%s%d", strSection, CARIO_LINE_PREFIX, i );
		pline->save_to_named_data( ptrND, strEntry );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::reload_object_from_data( IUnknown* punkNamedData )
{
	_kill_cario_array();

	if( m_ptrActiveObjectList != 0 )
		if( is_correct_object_list( m_ptrActiveObjectList ) )
		{
			if( !load_cario_state_from_data( punkNamedData ) )
				load_cario_state_from_data( ::GetAppUnknown() );
			return build( m_ptrActiveObjectList );
		}
				
	load_cario_state_from_data( ::GetAppUnknown() );
	return build( ::GetAppUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::save_to_object_and_shell_data()
{
	CarioViewMode cvm = GetCarioViewMode();

	if( cvm == cvmNotDefine || cvm == cvmMenu )
		return false;

	if( m_ptrActiveObjectList == 0 )
		return false;

	if( !is_correct_object_list( m_ptrActiveObjectList ) )
		return false;	

	if( ( cvm == cvmCellar ) && !CheckInterface( m_ptrActiveObjectList, IID_INamedData ) )
		init_attached_object_named_data( m_ptrActiveObjectList );

	bool bres = save_cario_state_to_data( m_ptrActiveObjectList );
	if( cvm == cvmDesign )
		bres &= save_cario_state_to_data( ::GetAppUnknown() );

	bres &= save_view_settings( m_ptrActiveObjectList );
	if( cvm == cvmDesign )
		bres &= save_view_settings( ::GetAppUnknown() );

	return bres;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::fire_event_data_change()
{
	if( m_ptrActiveObjectList == 0 )
		return false;

	IViewPtr ptrView( GetViewPtr() );
	if( ptrView )
	{
		IUnknown* punkDoc = 0;
		ptrView->GetDocument( &punkDoc );
		if( punkDoc )
		{
			IUnknownPtr ptrDoc = punkDoc;
			punkDoc->Release();	punkDoc = 0;
			::FireEvent( ptrDoc, szEventCarioChange, ptrView, m_ptrActiveObjectList, 0, 0 );
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
CString CCarioImpl::get_drawing_path()
{
	if( m_carioViewMode == cvmPreview )
		return PREVIEW_DRAWING_PATH;
	else if( m_carioViewMode == cvmCellar )
		return ANALIZE_DRAWING_PATH;

	return "";
}


/////////////////////////////////////////////////////////////////////////////
chromo_object* CCarioImpl::find_chromo_by_guid( GUID guidChromo, int* pnLineIdx, int* pnCellIdx, int* pnChromoIdx  )
{
	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		chromo_line* pline = m_arCarioLine[i];
		for( int j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			for( int k=pcell->m_arChromos.GetSize()-1;k>=0;k-- )
			{
				chromo_object* pobj = pcell->m_arChromos[k];
				if( pobj->m_guidMeasureObject == guidChromo )
				{
					if( pnLineIdx )
						*pnLineIdx = i;

					if( pnCellIdx )
						*pnCellIdx = j;

					if( pnChromoIdx )
						*pnChromoIdx = k;

					return pobj;
				}
			}
		}
	}

	return 0;

}

//action
/////////////////////////////////////////////////////////////////////////////
chromo_line* CCarioImpl::add_line_at_pos( int nLine )
{
	if( nLine < 0 || nLine >= m_arCarioLine.GetSize() )//== cos cellar must be last
		return 0;

	chromo_line* pline = new chromo_line;
	m_arCarioLine.InsertAt( nLine, pline );	

	chromo_cell* pcell = new chromo_cell;
	pcell->m_nCellWidth = m_sizeDefaultLine.cx;
	pline->m_arCell.InsertAt( 0, pcell );

	return pline;
}

/////////////////////////////////////////////////////////////////////////////
chromo_cell* CCarioImpl::add_cell_at_pos( int nLine, int nCell )
{
	if( nLine < 0 || nLine >= m_arCarioLine.GetSize() )
		return 0;

	chromo_line* pline = m_arCarioLine[nLine];

	if( nCell < 0 || nCell > pline->m_arCell.GetSize() )
		return 0;

	chromo_cell* pcell = new chromo_cell;

	pline->m_arCell.InsertAt( nCell, pcell );	

	return pcell;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::delete_line( int nLine )
{
	if( nLine < 0 || nLine >= m_arCarioLine.GetSize() - 1 )//caan't delete cellar
		return false;

	chromo_line* pline = m_arCarioLine[nLine];
	delete pline;	pline = 0;
	
	m_arCarioLine.RemoveAt( nLine );

	if( nLine < m_arCarioLine.GetSize() )
		m_nActiveLine = nLine;
	else
		m_nActiveLine = nLine - 1;

	m_nActiveCell = 0;
	
	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::delete_cell( int nLine, int nCell )
{
	if( nLine < 0 || nLine >= m_arCarioLine.GetSize() )
		return false;

	chromo_line* pline = m_arCarioLine[nLine];

	if( nCell < 0 || nCell >= pline->m_arCell.GetSize() )
		return false;

	chromo_cell* pcell = pline->m_arCell[nCell];

	delete pcell;	pcell = 0;

	pline->m_arCell.RemoveAt( nCell );

	m_nActiveLine = nLine;
	if( nCell < pline->m_arCell.GetSize() )
		m_nActiveCell = nCell;
	else
		m_nActiveCell = nCell - 1;

	if( !pline->m_arCell.GetSize() )
		delete_line( nLine );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
int CCarioImpl::_get_text_caption_height()
{
	if( !m_bShowClassNumber )
		return 0;

	CFont	font;
	font.CreateFontIndirect( &m_lfClassText );
	TEXTMETRIC tm;

	{
		CClientDC dc(0);

		dc.SelectObject( &font );
		dc.GetTextMetrics( &tm );					
	}

	return tm.tmHeight + 2 * CAPTION_ALIGN;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::get_pointers( int nLineIdx, int nCellIdx, int nChromoIdx,
									chromo_line** ppline, chromo_cell** ppcell, chromo_object** ppobject )
{
	if( nLineIdx >= 0 && nLineIdx < m_arCarioLine.GetSize() )
	{
		chromo_line* pline = m_arCarioLine[nLineIdx];
		if( ppline )	*ppline = pline;

		if( nCellIdx >= 0 && nCellIdx < pline->m_arCell.GetSize() )
		{
			chromo_cell* pcell = pline->m_arCell[nCellIdx];
			if( ppcell )	*ppcell = pcell;

			if( nChromoIdx >= 0 && nChromoIdx < pcell->m_arChromos.GetSize() )
			{
				chromo_object* pobj = pcell->m_arChromos[nChromoIdx];
				if( ppobject )	*ppobject = pobj;
				
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::lookup_class( long lclass, int* pnLineIdx, int* pnCellIdx, 
								chromo_line** ppline, chromo_cell** ppcell )
{
	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		chromo_line* pline = m_arCarioLine[i];
		for( int j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			if( pcell->m_nClassNum == lclass )
			{
				if( pnLineIdx )
					*pnLineIdx = i;

				if( pnCellIdx )
					*pnCellIdx = j;

				if( ppline )
					*ppline = pline;

				if( ppcell )
					*ppcell = pcell;
			}
		}
	}	
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::set_chromo_class( chromo_object *pchromo, long lclass )
{
	ICalcObjectPtr	ptrCalc = pchromo->m_ptrMeasureObject;
	if( ptrCalc == 0 )return;

	long	class_no = get_object_class( ptrCalc );

	if( class_no == lclass )
		return;
////////////////////////
//[ay]18.08.2002
//	if( lclass == -1 )
//		return;

	if( m_carioViewMode != cvmCellar )
		set_object_class( ptrCalc, lclass );

	INamedDataObject2Ptr	ptrNamed( ptrCalc );
	IUnknown		*punkParent;
	ptrNamed->GetParent( &punkParent );
	IDataObjectListPtr	ptrList( punkParent );
	if( punkParent )punkParent->Release();
	if( ptrList == 0 )return;

	m_bLockObjectUpdate = true;
	ptrList->UpdateObject( ptrNamed );
	m_bLockObjectUpdate = false;
	
	
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::lookup_chromo_in_array( IUnknown* punkChromo, int* pnline, int* pncell, int* pnobj )
{
	GUID	guid = GetObjectKey( punkChromo );

	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		chromo_line* pline = m_arCarioLine[i];		

		for( int j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			for( int k=0;k<pcell->m_arChromos.GetSize();k++ )
			{
				chromo_object* pobj = pcell->m_arChromos[k];
				if( pobj->m_guidMeasureObject == guid )
				{
					if( pnline )
						*pnline = i;

					if( pncell )
						*pncell = j;

					if( pnobj )
						*pnobj = k;

					return true;
				}
			}
		}
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
IIdiogramPtr CCarioImpl::get_idio_object_from_measure( IUnknown* punkMeasure, bool bCellar )
{
	IMeasureObjectPtr ptrMeasure( punkMeasure );
	if( ptrMeasure == 0 )	return 0;

	INamedDataObject2Ptr ptrNDO( ptrMeasure );
	if( ptrNDO == 0 )		return 0;


	long lPosCromo = 0;
	ptrNDO->GetFirstChildPosition( &lPosCromo );
	while( lPosCromo )
	{
		IUnknown* punkChromo = 0;
		ptrNDO->GetNextChild( &lPosCromo, &punkChromo );
		if( !punkChromo )
			continue;

		IIdiogramPtr ptr_idio( punkChromo );
		punkChromo->Release();	punkChromo = 0;

		if( ptr_idio != 0 )
		{
			unsigned uFlag = -1;
			ptr_idio->get_flags( &uFlag );

			if( uFlag == IDIT_INSERTED )
			{
				if( m_guidExclude == ::GetObjectKey( ptr_idio ) )
				{
					::ZeroMemory( &m_guidExclude, sizeof( GUID ) );
					return 0;
				}
				return ptr_idio;
			}
		}
	}

	return 0;
}


IChromosomePtr CCarioImpl::get_chromo_object_from_measure( IUnknown* punkMeasure )
{
	IMeasureObjectPtr ptrMeasure( punkMeasure );
	if( ptrMeasure == 0 )	return 0;

	INamedDataObject2Ptr ptrNDO( ptrMeasure );
	if( ptrNDO == 0 )		return 0;


	long lPosCromo = 0;
	ptrNDO->GetFirstChildPosition( &lPosCromo );
	while( lPosCromo )
	{
		IUnknown* punkChromo = 0;
		ptrNDO->GetNextChild( &lPosCromo, &punkChromo );
		if( !punkChromo )
			continue;

		IChromosomePtr ptrChromo( punkChromo );
		punkChromo->Release();	punkChromo = 0;

		if( ptrChromo != 0 )
			return ptrChromo;
	}

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::on_objects_drop( drag_n_drop_data* pdata )
{

	if( !pdata )
		return false;
//скорее всего координаты - оконные
	CPoint pt = convert_from_wnd_to_object( pdata->point );
//попробуем найти ячейку, куда юзер бросил хромосому
	int	nDropLine, nDropCell, nPos;
	hit_test_area( pt, &nDropLine, &nDropCell, &nPos );

//нифига не нашли - дропаем в подвал
	if( nDropLine == -1 || nDropCell == -1 )
	{
		nDropLine = m_arCarioLine.GetSize()-1;
		nDropCell = 0;
	}

//а вот и хромосомы, которые тащились
	CArray<GUID, GUID>	arGuidChromo;
	if( !create_chromo_array_from_dd( pdata, arGuidChromo ) )
		return false;

// их надо аккуратно убрать из их существ. ячеек и добавить куда бросили

	int	nLine, nCell, nChromo, nTest;
	for( nLine = 0; nLine < m_arCarioLine.GetSize(); nLine++ )
	{
		chromo_line	*pline = m_arCarioLine[nLine];

		for( nCell = 0; nCell < pline->m_arCell.GetSize(); nCell++ )
		{
			chromo_cell	*pcell = pline->m_arCell[nCell];

			for( nChromo = pcell->m_arChromos.GetSize()-1; nChromo >= 0; nChromo-- )
			{
				chromo_object	*pchromo = pcell->m_arChromos[nChromo];

				for( nTest = 0; nTest < arGuidChromo.GetSize(); nTest++ )
				{
					if( pchromo->m_guidMeasureObject == arGuidChromo[nTest] )
					{
						pcell->m_arChromos.RemoveAt( nChromo );
						delete pchromo;
						break;
					}
				}
			}
		}
	}
//теперь этих объектов нет в кариограмме. запихиваем их в ячейку.
	chromo_cell *pcell;
	get_pointers( nDropLine, nDropCell, 0, 0, &pcell, 0 );

	for( nTest = 0; nTest < arGuidChromo.GetSize(); nTest++ )
	{
		chromo_object	*p = new chromo_object;
		p->m_guidMeasureObject = arGuidChromo[nTest];
		if( nPos >= 0 )
			pcell->m_arChromos.InsertAt( nPos, p );
		else
			pcell->m_arChromos.Add( p );
	}

	m_nActiveLine = nDropLine;
	m_nActiveCell = nDropCell;
	
	if( nPos < 0 )
		m_nActiveChromo = pcell->m_arChromos.GetSize() - 1;
	else
		m_nActiveChromo = nPos;

	arGuidChromo.RemoveAll();

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::create_chromo_array_from_dd( drag_n_drop_data* pdata, CArray<GUID, GUID>& arGuidChromo )
{
	if( !pdata )
		return false;
	
	CCarioFrame	*pframe = GetDragDropFrame();
	if( !pframe )return false;

	for( int i = 0; i < pframe->m_arSrcGuids.GetSize(); i++ )
		arGuidChromo.Add( pframe->m_arSrcGuids[i] );

	return i != 0;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::update_chromosome( IUnknown* punk, long lCode )
{
	//все про объект
	bool	fChanged = false;
	ICalcObjectPtr	ptr_calc( punk );
	long	class_no = get_object_class( ptr_calc );
	GUID	guid_object = GetObjectKey( ptr_calc );

	//будем искать за один раз объект и ячейку, в которую его складывать нужно - 
	//вдруг у объекта класс поменялся
	chromo_object	*pobj_found = 0;
	chromo_cell		*pcell_found = 0;		//подходящая ячейка для этого объекта
	chromo_cell		*pcell_current = 0;		//текущая ячейка объекта
	long			pos_in_cell = 0;
	bool			fObjectInOwnCell = false;

	IIdiogramPtr	ptr_idio, ptr_idio2;
	IChromosomePtr	ptr_chromo;

	//если у объекта нет ни хромосомы, ни  идиограммы, мы его удаляем
	if( lCode != CHROMO_OBJECT_DELETE )
	{
		ptr_idio = get_idio_object_from_measure( punk );
		ptr_chromo = get_chromo_object_from_measure( punk );
		ptr_idio2 = _get_editable_idio( punk );

		if( ptr_idio == 0 && ptr_chromo == 0 )
			lCode = CHROMO_OBJECT_DELETE;
	}

	//ячейка для начала - подвал
	chromo_line*	pline = _get_cellar( );	
	chromo_cell*	pcellar = 0;
	if( pline && pline->m_arCell.GetSize() )
		pcellar = pcell_found = pline->m_arCell[0];

	//пошли
	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		chromo_line* pline = m_arCarioLine[i];		

		for( int j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];

			//нашли ячейку
//			if( !pcell_found )
				if( pcell->m_nClassNum == class_no )
					pcell_found = pcell;

			if( !pobj_found )
				for( int k=pcell->m_arChromos.GetSize()-1; k>=0; k-- )
				{
					chromo_object* ptest = pcell->m_arChromos[k];

					//нашли объект
					if( ptest->m_guidMeasureObject == guid_object )
					{
						if( lCode == CHROMO_OBJECT_DELETE )
						{
							//смотрим, не в подвале ли мы
							pcell->m_arChromos.RemoveAt( k );
							delete ptest;
							fChanged = true;

							if( pcellar == pcell )
							{
								//remove all empty 
								break;

							}
						}
						else
						{
							pcell_current = pcell;
							pos_in_cell = k;
							pobj_found =ptest;
							fObjectInOwnCell = pcell_found == pcell;
						}
					}
				}
		}
	}

	if( lCode == CHROMO_OBJECT_DELETE )
	{
			
		return fChanged;
	}

	//если мы не нашли ячейку для объекта (даже подвала нет) - сваливаем
	if( !pcell_found )
	{
		return fChanged;
	}


	if( !pobj_found && lCode == CHROMO_OBJECT_CHANGED )
	{
		return false;
	}

	//новый объект, если не найден
	if( !pobj_found )
	{
		pobj_found = new chromo_object;
		ASSERT( pcell_current == 0 );
		fChanged = true;
	}

	//всю инфу - в объект
	fChanged |= pobj_found->m_ptrChromos != ptr_chromo;
	fChanged |= pobj_found->m_ptrIdio != ptr_idio;
	fChanged |= pobj_found->m_sptrEditableIdio != ptr_idio2;

/********* Maxim [21.05.02] *****************************************************************************
	pobj_found->m_bShowIdiogramm = true;
/********* Maxim [21.05.02] *****************************************************************************/
	pobj_found->m_bShowIdiogramm = bool( ptr_idio != 0 );
/********* Maxim [21.05.02] *****************************************************************************/
	pobj_found->m_ptrMeasureObject = punk;

	if(  GetCarioViewMode() == cvmCellar && m_bShowEditableIdio && pobj_found->m_sptrEditableIdio == 0 )
	{
		IUnknownPtr sptrI = _get_editable_idio( pobj_found->m_ptrMeasureObject );
		if( sptrI == 0 )
		{
			IUnknown	*punkIdio = CreateTypedObject( _bstr_t( sz_idio_type ) );
			m_bSkipNotify = true;
			if( punkIdio )
			{
				IIdioBendsPtr ptrBends = punkIdio;
				punkIdio->Release();

				pobj_found->m_sptrEditableIdio = ptrBends;
				pobj_found->m_sptrEditableIdio->set_flags( IDIT_EDITABLE );

				INamedDataObject2Ptr	ptrND = ptrBends;
				ptrND->SetParent( pobj_found->m_ptrMeasureObject, 0 );

			}
			m_bSkipNotify = false;
		}
		else
			pobj_found->m_sptrEditableIdio = sptrI;

	}

	pobj_found->m_ptrChromos = ptr_chromo;
	pobj_found->m_ptrIdio = ptr_idio;
	pobj_found->m_sptrEditableIdio = ptr_idio2;

	pobj_found->m_guidMeasureObject = GetObjectKey( pobj_found->m_ptrMeasureObject );

	if( m_bShowCGHGraph )
		pobj_found->init_cgh_graph( this );

	if( m_bShowCGHRatio )
		pobj_found->init_cgh_ratio( this );


	//если надо, объект перемещаем
	if( pcell_found != pcell_current )
	{
		if( pcell_current )
			pcell_current->m_arChromos.RemoveAt( pos_in_cell );
		pcell_found->m_arChromos.Add( pobj_found );
		fChanged = true;
	}
	return fChanged;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::on_activate_object( IUnknown* punkObject )
{
	int nline	= -1;
	int ncell	= -1;
	int nobj	= -1;

	bool	fRet = lookup_chromo_in_array( punkObject, &nline, &ncell, &nobj );

	m_nActiveLine	= nline;
	m_nActiveCell	= ncell;

	// [Max] 
	if( GetCarioViewMode() != cvmDesign )
		m_nActiveChromo	= nobj;
		
	invalidate_rect( 0 );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::build( IUnknown* punkObjectList )
{	
	CarioViewMode cvm = GetCarioViewMode();

	if( cvm == cvmNotDefine )
		return false;

	if( cvm == cvmPreview || cvm == cvmDesign || cvm == cvmCellar || cvm == cvmMenu )
	{
		//is there cellar?
		_get_cellar();

		//add line if not exist
		if( cvm != cvmCellar )
		{
			if( m_arCarioLine.GetSize() == 1 )//only cellar
			{
		 		chromo_line* pline = new chromo_line;
				pline->m_nLineHeight = m_sizeDefaultLine.cy;
				chromo_cell	*pcell = new chromo_cell;
				pcell->m_nCellWidth = m_sizeDefaultLine.cx;
				pline->m_arCell.Add( pcell );
				
				m_arCarioLine.InsertAt( 0, pline );			
			}
		}

		//need delete all but cellar
		if( cvm == cvmCellar )
		{
			for( int i=m_arCarioLine.GetSize()-1;i>=0;i-- )
			{
				chromo_line* pline = m_arCarioLine[i];
				if( !pline->m_bCellar )
				{
					m_arCarioLine.RemoveAt( i );
					delete pline;
				}
			}
		}
		
		set_class_num( );
		synchronize_with_object_list( punkObjectList );
		synchronize_dimmension( SDF_RECALC_ALL );
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::set_class_num( )
{
	int nCurClass = 0;

	chromo_line* pcellar = _get_cellar();

	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		chromo_line* pline = m_arCarioLine[i];		

		if( pline == pcellar )
			nCurClass = -1;

		for( int j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			//set class info
			pcell->m_nClassNum		= nCurClass;
			pcell->m_strClassName	= get_class_name( nCurClass );
			if( nCurClass >= 0 ) nCurClass++;
	
		}
	}

}

/////////////////////////////////////////////////////////////////////////////

#include "propbag.h"
void CCarioImpl::synchronize_dimmension( unsigned flags, IUnknown *punkToSync )
{	
	m_sizeClient.cx = m_sizeClient.cy = 100;

	_variant_t varPane;
	int nCurrPane = 0;

	INamedPropBagPtr sptrBag = GetViewPtr();
	sptrBag->GetProperty( _bstr_t( szPaneNum ), &varPane );
	if( varPane.vt != VT_EMPTY )
		nCurrPane = varPane.lVal;


	if( m_carioViewMode == cvmMenu )
		flags = SDF_NORECALC;

	GUID	guid_test = ::GetObjectKey( punkToSync );
	int nCaptionHeight = _get_text_caption_height();
	long	lCurrentPosY = AROUND_BORDER_SIZE;

	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		chromo_line* pline = m_arCarioLine[i];
		//lCurrentPosY+=nCaptionHeight;

		//это - линия привязки. значение зависит от параметров выравнивания.
		long	lCurrentLine = 0;
		long	nMinCellHeight = pline->m_nLineHeight - nCaptionHeight;
		long	lMaxTopLine = 0, lMaxBottomLine = nMinCellHeight, lMaxCenterLine = nMinCellHeight/3;
		long	lTopLine, lBottomLine, lCenterLine;
		bool	fFirst = true;

		int	j, k;

		pline->m_rectLine.left = AROUND_BORDER_SIZE;
		pline->m_rectLine.top = lCurrentPosY;
		lCurrentPosY+=AROUND_BORDER_SIZE;


		for( j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			for( k = 0; k < pcell->m_arChromos.GetSize(); k++ )
			{
				chromo_object	*pchromo = pcell->m_arChromos[k];

				if( (flags == SDF_RECALC_ALL) || (flags == SDF_RECALC_OBJECT && guid_test == pchromo->m_guidMeasureObject ) )
				{	
					pchromo->layout_object( m_bShowChromoCenterLine, m_bShowChromoCentromere, m_bShowCellIdio, m_chromoAlign, nCurrPane, this, pcell->m_nClassNum );
//					if( m_bShowCellIdio )
//						pchromo->m_sizeTotal.cx+=CROMO_OFFSET_X+m_nIdioWidth;
				}

				lTopLine = pchromo->m_rectImage.top+pchromo->m_rectChromo.top;
				lBottomLine = pchromo->m_rectImage.bottom+pchromo->m_rectChromo.top;
				lCenterLine = pchromo->m_rectImage.top+pchromo->m_nCentromereY+pchromo->m_rectChromo.top;

				if( fFirst )
				{
//					lMaxTopLine = lTopLine;
//					lMaxBottomLine = lBottomLine;
//					lMaxCenterLine = lCenterLine;
					fFirst = false;
				}
				lMaxTopLine = max( lMaxTopLine, lTopLine );
				lMaxBottomLine = max( lMaxBottomLine, lBottomLine );
				lMaxCenterLine = max( lMaxCenterLine, lCenterLine );
			}
		}

		if( !lMaxBottomLine )lMaxBottomLine = pline->m_nLineHeight-nCaptionHeight;

		if( m_chromoAlign == acTop )lCurrentLine = 0;
		else if( m_chromoAlign == acCenterLine )lCurrentLine = lMaxCenterLine/*-lMaxTopLine*/;
		else if( m_chromoAlign == acBottom )lCurrentLine = lMaxBottomLine/*-lMaxTopLine*/;

		pline->m_nCenterLine = lCurrentLine+(lCurrentPosY-pline->m_rectLine.top );//;

		//всем объектам записать смещение
		long	lCurrentPosX = pline->m_rectLine.left;
		int		nMaxCellHeight = 0;

		for( j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			lCurrentPosX += pcell->m_nCellOffset;

			pcell->m_rectCell.left = lCurrentPosX;
			pcell->m_rectCell.top = lCurrentPosY;

			long	lFilledWidth = CROMO_OFFSET;

			for( k = 0; k < pcell->m_arChromos.GetSize(); k++ )
			{
				chromo_object	*pchromo = pcell->m_arChromos[k];
				pchromo->m_ptOffset.x = lCurrentPosX+lFilledWidth;

				if( m_chromoAlign == acTop )
					pchromo->m_ptOffset.y = pline->m_nCenterLine-pchromo->m_rectImage.top;
				else if( m_chromoAlign == acCenterLine )
					pchromo->m_ptOffset.y = pline->m_nCenterLine-pchromo->m_nCentromereY;
				else if( m_chromoAlign == acBottom )
					pchromo->m_ptOffset.y = pline->m_nCenterLine-pchromo->m_rectImage.bottom;

				
				lFilledWidth+=pchromo->m_sizeTotal.cx;

				nMaxCellHeight = max( nMaxCellHeight, pchromo->m_sizeTotal.cy+pchromo->m_ptOffset.y );
				pchromo->m_ptOffset.y += lCurrentPosY-AROUND_BORDER_SIZE;
			}

			lFilledWidth+=CROMO_OFFSET;

			long	nCellWidth = max( pcell->m_nCellWidth, lFilledWidth );
			lCurrentPosX+=nCellWidth;
			pcell->m_rectCell.right = pcell->m_rectCell.left+nCellWidth;
		}


		long	nLineHeight = pline->m_nLineHeight;
		if( nLineHeight < nMaxCellHeight )
			nLineHeight = nMaxCellHeight;


		//обновим прямоугольники ячеек - все в строке одной высоты
		for( j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			pcell->m_rectCell.bottom = pcell->m_rectCell.top+nLineHeight;
		}


		nLineHeight+=AROUND_BORDER_SIZE;
		pline->m_rectLine.right = lCurrentPosX;
		lCurrentPosY=pline->m_rectLine.bottom = pline->m_rectLine.top+nLineHeight+nCaptionHeight;

		if( m_carioViewMode == cvmMenu )
			pline->m_nCaptionHeight = nCaptionHeight;
		else
			pline->m_nCaptionHeight = pline->m_rectLine.Height();

		m_sizeClient.cx = max( pline->m_rectLine.right+AROUND_BORDER_SIZE, m_sizeClient.cx );
		m_sizeClient.cy = max( pline->m_rectLine.bottom+AROUND_BORDER_SIZE, m_sizeClient.cy );
	}

	if( m_bCanSaveToObjectAndShellData && ( m_carioViewMode != cvmDesign || m_bHasBeenChange ) )
		save_to_object_and_shell_data();

	if( m_bCanFireEventDataChange )
		fire_event_data_change();
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::draw_chromo_line( CDC* pdc, int nLineIdx, 
							CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{
	if( nLineIdx < 0 || nLineIdx >= m_arCarioLine.GetSize() )
		return;

	chromo_line* pline = 0;
	CRect rcLine = convert_from_object_to_wnd( get_chromo_line_rect( nLineIdx, &pline, true ) );
	rcLine.InflateRect( 0, 0, 1, 1 );
	if( !pline ) return;

	CRect rc;
	if( !rc.IntersectRect( &rcUpdate, &rcLine ) )
		return;	

	CFont	*pold, font;
	int	nOldHeight = m_lfClassText.lfHeight;
	int nOldWeight = m_lfClassText.lfWeight;	

	if( m_carioViewMode != cvmMenu )
	{
		m_lfClassText.lfHeight*= get_zoom();
	}
	else
	{
		m_lfClassText.lfHeight=16;
		m_lfClassText.lfWeight=FW_BOLD;
	}

	font.CreateFontIndirect( &m_lfClassText );
	m_lfClassText.lfHeight = nOldHeight;
	m_lfClassText.lfWeight = nOldWeight;

	pold = pdc->SelectObject( &font );
	
	if( !is_printing() && (m_carioViewMode == cvmDesign||m_carioViewMode == cvmMenu) )
	{		
		CRect	rect = rcLine;
		rect.InflateRect( -1, -1 );

		bool	bActive = (m_nActiveLine == nLineIdx && m_carioViewMode == cvmDesign );
		pdc->FillRect( &rect, &CBrush( bActive ? m_clrLineActive : m_clrLine ) );

		CPen pen( PS_SOLID, 0, m_clrLineBorder );
		CPen* pOldPen = (CPen*)pdc->SelectObject( &pen );
		pdc->SelectStockObject( NULL_BRUSH );
		pdc->Rectangle( rcLine );

		pdc->SelectObject( pOldPen );
	}


	for( int i=0;i<pline->m_arCell.GetSize(); i++ )
		draw_chromo_cell( pdc, nLineIdx, i, rcUpdate, pbmih, pdibBits );
	
	pdc->SelectObject( pold );
}

IUnknownPtr _GetObjectByName( IUnknown *punkFrom, const BSTR bstrObject, const BSTR bstrType, bool bThrowException = true)
{
	_bstr_t	bstrObjectName = bstrObject;
	_bstr_t	bstrTypeName = bstrType;
	sptrIDataTypeManager	ptrDataType( punkFrom );

	long	nTypesCounter = 0;
	ptrDataType->GetTypesCount( &nTypesCounter );

	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		BSTR	bstrCurTypeName = 0;
		ptrDataType->GetType( nType, &bstrCurTypeName );

		_bstr_t	str( bstrCurTypeName, false );

		if( bstrType && str != bstrTypeName )
			continue;

		IUnknown	*punkObj = 0;
		long	dwPos = 0;

		ptrDataType->GetObjectFirstPosition( nType, &dwPos );

		while( dwPos )
		{
			punkObj = 0;
			ptrDataType->GetNextObject( nType, &dwPos, &punkObj );

			if (bstrObjectName.length() == 0 || ::GetObjectName( punkObj ) == (char*)bstrObjectName)
			{
				IUnknownPtr sptr(punkObj);
				if (punkObj) punkObj->Release();
				return sptr;
			}

			INamedDataObject2Ptr	ptrN( punkObj );
			punkObj->Release();

			long	lpos = 0;
			ptrN->GetFirstChildPosition( &lpos );

			while( lpos )
			{
				IUnknown	*punkObject = 0;
				ptrN->GetNextChild( &lpos, &punkObject );

				if (bstrObjectName.length() == 0 || ::GetObjectName( punkObject ) == (char*)bstrObjectName)
				{
					IUnknownPtr sptrO(punkObject);
					punkObject->Release();
					return sptrO;
				}

				punkObject->Release();


			}

			
		}
	}
	return 0;
}

//загрузить все идиограммы в карту идиограмм
void CCarioImpl::load_idiograms()
{
	m_idiograms.clear();
	if( !m_bShowCellIdio )
		return;

	char	sz_path[MAX_PATH], sz_path_t[MAX_PATH], sz_path2[MAX_PATH];

	GetModuleFileName( 0, sz_path_t, sizeof( sz_path_t ) );
	strcpy( strrchr( sz_path_t, '\\' ), "\\idio_db\\" );


	strcpy( sz_path, ::GetValueString( ::GetAppUnknown(), "\\IdioDBase", "ShowAllFile", "" ) );
	//GetValue( );

	if( !strlen( sz_path ) )
		return ;


	
	strcpy( sz_path2, sz_path );

	{
		{
			IFileDataObjectPtr	sptrD( "Data.NamedData", 0, CLSCTX_INPROC_SERVER );

			if( sptrD )
			{
				if( sptrD->LoadFile( _bstr_t( sz_path2 ) ) ==  S_OK )
				{

					INamedDataObject2Ptr ptrObject = _GetObjectByName( sptrD, 0, _bstr_t( szTypeIdioDBase ) );

					if( ptrObject )
					{
						long lPos = 0;
						ptrObject->GetFirstChildPosition( &lPos );
						while( lPos ) 
						{
							IUnknown *punkI = 0;
							ptrObject->GetNextChild( &lPos, &punkI );
							if( punkI )
							{
								IIdiogramPtr sptrIdio = punkI;
								punkI->Release();

								if( sptrIdio )
								{
									long lClass = -1;
									sptrIdio->get_class( &lClass );

									long	lpos = m_idiograms.find( lClass );
									if( !lpos )
										m_idiograms.set( sptrIdio, lClass );
								}
							}
						}
					}

//					::FindClose( hFileFind );

//					*pzero = 0;
					strcat( sz_path_t, "idio_cellar.idio" );
					FILE *fl = fopen( sz_path_t, "r" );
					if( fl )
					{
						fclose( fl );

						IUnknown	*punk = CreateTypedObject( _bstr_t( sz_idio_type ) );
						if( punk )
						{
							IIdiogramPtr	ptrIdio = punk;
							punk->Release();

							if( ptrIdio->open_text( _bstr_t( sz_path_t ) ) == S_OK )
							{
								long	class_no = 0;
								ptrIdio->get_class( &class_no );
								m_idiograms.set( ptrIdio, -1 );
							}
						}
					}

					return;
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::draw_chromo_cell( CDC* pdc, int nLineIdx, int nCellIdx, 
							CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{
	chromo_line* pline = 0;
	chromo_cell* pcell = 0;
	CRect	rcCellObj = get_chromo_cell_rect( nLineIdx, nCellIdx, &pline, &pcell, true );
	CRect	rcCell = convert_from_object_to_wnd( rcCellObj );
	
	if( !pcell ) return;

	double fZoom = get_zoom();

	CRect rc;
	if( !rc.IntersectRect( &rcUpdate, &rcCell ) )
		return;
		
	bool bActiveCell = ( m_nActiveLine == nLineIdx && m_nActiveCell == nCellIdx 
		&& (m_carioViewMode == cvmDesign||m_carioViewMode == cvmMenu) );

	if( !is_printing() && (m_carioViewMode == cvmDesign||m_carioViewMode == cvmMenu) )
	{		
		CRect	rect = rcCell;
		rect.InflateRect( -1, -1 );

		COLORREF	clr = bActiveCell ? m_clrCellActive : m_clrCell;

		pdc->FillRect( &rect, &CBrush( clr ) );

		CPen pen( PS_SOLID, 0, m_clrCellBorder );
		CPen* pOldPen = (CPen*)pdc->SelectObject( &pen );
		
		pdc->MoveTo( rcCell.left, rcCell.top );
		pdc->LineTo( rcCell.left, rcCell.bottom );

		pdc->MoveTo( rcCell.right, rcCell.top );
		pdc->LineTo( rcCell.right, rcCell.bottom );
		
		pdc->SelectObject( pOldPen );		
	}

	if( m_bShowClassNumber||m_carioViewMode == cvmMenu )
	{
		CString strCaption;
		if( pline->m_bCellar )
			strCaption.LoadString( IDS_CELLAR_CAPTION );
		else
			strCaption = pcell->m_strClassName;

		CRect rcText;
		rcText.SetRectEmpty();
		pdc->DrawText( strCaption, rcText, DT_CALCRECT );

		CRect rcTextOut;

		if( pline->m_bCellar )
			rcTextOut.left	= rcCell.left + 10;
		else
			rcTextOut.left	= rcCell.left + ( rcCell.Width() - rcText.Width() ) / 2;

		rcTextOut.right		= rcTextOut.left + rcText.Width();
		rcTextOut.top		= rcCell.bottom - ( CAPTION_ALIGN * fZoom ) - rcText.Height();
		rcTextOut.bottom	= rcTextOut.top + rcText.Height();

		int nMode = pdc->SetBkMode( TRANSPARENT );
		if( m_carioViewMode == cvmMenu )
			m_clrClassText = m_clrs_cell[min( 5, pcell->m_arChromos.GetSize() )];
		COLORREF clrOldText = pdc->SetTextColor( m_clrClassText );
		

		pdc->DrawText( strCaption, rcTextOut, DT_CENTER|DT_VCENTER|DT_SINGLELINE );
		pdc->SetBkMode( nMode );
		pdc->SetTextColor( clrOldText );
	}
		
	if( m_carioViewMode != cvmMenu )
	{
		for( int i=0;i<pcell->m_arChromos.GetSize(); i++ )
			draw_chromo_object( pdc, nLineIdx, nCellIdx, i, rcUpdate, pbmih, pdibBits );
		
		if( m_bNeedChromoAlignLine )
		{
			CPoint pt1, pt2;

			pt1.x = rcCellObj.left + CENTER_LINE_SIZE / 2;
			pt1.y = rcCellObj.top + pline->m_nCenterLine;

			pt2.x = rcCellObj.right - CENTER_LINE_SIZE / 2;
			pt2.y = pt1.y;

			pt1 = convert_from_object_to_wnd( pt1 );
			pt2 = convert_from_object_to_wnd( pt2 );
						
			CPen pen( PS_SOLID, 0, m_clrAlignLine );
			CPen* pOldPen = (CPen*)pdc->SelectObject( &pen );
			pdc->MoveTo( pt1 );
			pdc->LineTo( pt2 );

			if( m_chromoAlign == acCenterLine )
			{
				pdc->MoveTo( pt1.x, pt1.y - CENTER_LINE_SIZE / 2 );
				pdc->LineTo( pt1.x, pt1.y + CENTER_LINE_SIZE / 2 );

				pdc->MoveTo( pt2.x, pt2.y - CENTER_LINE_SIZE / 2 );
				pdc->LineTo( pt2.x, pt2.y + CENTER_LINE_SIZE / 2 );
			}
			else if( m_chromoAlign == acTop )
			{
				pdc->MoveTo( pt1.x, pt1.y );
				pdc->LineTo( pt1.x, pt1.y + CENTER_LINE_SIZE / 2 );

				pdc->MoveTo( pt2.x, pt2.y );
				pdc->LineTo( pt2.x, pt2.y + CENTER_LINE_SIZE / 2 );
			}
			else if( m_chromoAlign == acBottom )
			{
				pdc->MoveTo( pt1.x, pt1.y );
				pdc->LineTo( pt1.x, pt1.y - CENTER_LINE_SIZE / 2 );

				pdc->MoveTo( pt2.x, pt2.y );
				pdc->LineTo( pt2.x, pt2.y - CENTER_LINE_SIZE / 2 );
			}
	
			pdc->SelectObject( pOldPen );
		}

	}


	/*
	if( m_nActiveLine == nLineIdx && m_nActiveCell == nCellIdx )
	{
		CRect rcCellWnd = convert_from_object_to_wnd( rcCell );
		rc.IntersectRect( &rcCellWnd, &rcUpdate );

 		CRect rcMirror = rc - rcUpdate.TopLeft();		
		DrawMirror( pbmih, pdibBits, rcMirror, m_clrCellMirror );
	}
	*/

}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::draw_chromo_object( CDC* pdc, int nLineIdx, int nCellIdx, int nObjectIdx, 
									CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{

	chromo_line*		pline = 0;
	chromo_cell*		pcell = 0;
	chromo_object*		pobject = 0;

	get_pointers( nLineIdx, nCellIdx, nObjectIdx, &pline, &pcell, &pobject );
	
	if( !pobject ) return;

	CRect rcObject = convert_from_object_to_wnd(pobject->get_total_rect());

	if( !rcObject.IntersectRect( &rcUpdate, &rcObject ) )
		return;
	
	draw_chromosome_image( pdc, pobject, rcUpdate, pbmih, pdibBits );

	if( m_bShowCellIdio )
	{
		chromo_object	*pchromo = pobject;
		CRect	rect;// = CRect( pchromo->m_ptOffset, pchromo->m_sizeTotal );

		rect = pchromo->m_rectIdio;
		rect.OffsetRect( pchromo->m_ptOffset );
//		rect.left = pchromo->m_ptOffset.x + pchromo->m_sizeTotal.cx-CROMO_OFFSET_X-m_nIdioWidth;

		if( !m_nShowNums && m_bShowCGHAmpl && pchromo->m_bHasCGHAmplL )
			rect.left = rect.left + m_CGHAMPL_CX + CGHAMPL_OFFSET_X;

		if( pchromo->m_bShowIdiogramm && pchromo->m_ptrIdio != 0 )
		{
			rect.right = rect.left + rect.Width() / 2 - INNER_INT + CGHAMPL_OFFSET_X;

//			if( !m_nShowNums )
			{
				if( m_bShowCGHAmpl && pchromo->m_bHasCGHAmplR )
					rect.right += CGHAMPL_OFFSET_X;
				if( m_bShowCGHAmpl && pchromo->m_bHasCGHAmplL )
					rect.right += CGHAMPL_OFFSET_X;
			}
		}

		if( !m_nShowNums && m_bShowCGHAmpl && pchromo->m_bHasCGHAmplR )
			rect.right = rect.right - m_CGHAMPL_CX - CGHAMPL_OFFSET_X;

		CRect	rect_wnd = convert_from_object_to_wnd( rect );

		long lClassNum = pcell->m_nClassNum;
		if( m_carioViewMode == cvmCellar )
		{
			ICalcObjectPtr	ptr_calc( pchromo->m_ptrMeasureObject );
			lClassNum = get_object_class( ptr_calc );
		}

		long	lpos = m_idiograms.find( lClassNum );
		if( lpos )
		{
			IIdiogramPtr	ptrIdio = m_idiograms.get( lpos );
			ptrIdio->set_show_num( m_nShowNums );
			CSize sz = rect_wnd.Size();
			if( m_nShowNums )
				sz.cx = m_nIdioWidth * get_zoom() * pchromo->m_fZoom;

			ptrIdio->draw( *pdc, sz, &rect_wnd, 0, IDF_IGNORELENGTH | IDF_IGNORECENTROMERE|IDF_BLUE, get_zoom() * pchromo->m_fZoom );
		}

		if( m_bShowCGHAmpl )
		{
			COLORREF rcColorMin = m_clRatioMin;
			COLORREF rcColorMax = m_clRatioMax;


			if( pchromo->m_bHasCGHAmplL )
			{
				long lPos = pchromo->m_listCGHAmpl_L.head();
				while( lPos )
				{
					CInterval in = pchromo->m_listCGHAmpl_L.get( lPos );

					CRect rc = pchromo->m_rectImage;
					
					rc.left = pchromo->m_rectIdio.left;
					rc.right = rc.left + m_CGHAMPL_CX;

					rc.OffsetRect( pchromo->m_ptOffset );
					rc = convert_from_object_to_wnd( rc );

					double Scale = rc.Height();// / double( pchromo->m_rectImage.Height() );
				
					rc.top    += in.a * Scale;// - Scale;
					rc.bottom = rc.top + ( in.b - in.a ) * Scale;

					pdc->FillRect( rc, &CBrush( rcColorMin ) );

					lPos = pchromo->m_listCGHAmpl_L.next( lPos );
				}
			}

			if( pchromo->m_bHasCGHAmplR )
			{
				long lPos = pchromo->m_listCGHAmpl_R.head();

				while( lPos )
				{
					CInterval in = pchromo->m_listCGHAmpl_R.get( lPos );

					CRect rc = pchromo->m_rectImage;
					rc.left = pchromo->m_rectIdio.left;

					if( !m_nShowNums )
					{
						if( pchromo->m_bHasCGHAmplL )
							rc.left  += m_nIdioWidth + 2 * CGHAMPL_OFFSET_X + m_CGHAMPL_CX;
						else
							rc.left  += m_nIdioWidth + CGHAMPL_OFFSET_X;
					}
					else
					{
						if( pchromo->m_bShowIdiogramm && pchromo->m_ptrIdio != 0 )
						{
							if( pchromo->m_bHasCGHAmplL )
								rc.left  += pchromo->m_rectIdio.Width() - 2 * CGHAMPL_OFFSET_X - m_CGHAMPL_CX - pchromo->m_rectIdio.Width() / 2 - INNER_INT;
							else
								rc.left  += pchromo->m_rectIdio.Width() - CGHAMPL_OFFSET_X - pchromo->m_rectIdio.Width() / 2 - INNER_INT;
						}
						else
						{
							if( pchromo->m_bHasCGHAmplL )
								rc.left  += pchromo->m_rectIdio.Width() - 2 * CGHAMPL_OFFSET_X - m_CGHAMPL_CX;
							else
								rc.left  += pchromo->m_rectIdio.Width() - CGHAMPL_OFFSET_X;
						}
					}

					rc.right = rc.left + m_CGHAMPL_CX;

					rc.OffsetRect( pchromo->m_ptOffset );
					rc = convert_from_object_to_wnd( rc );

					double Scale = rc.Height();// / double( pchromo->m_rectImage.Height() );
				
					rc.top    += in.a * Scale;
					rc.bottom = rc.top + ( in.b - in.a ) * Scale;

					pdc->FillRect( rc, &CBrush( rcColorMax ) );

					lPos = pchromo->m_listCGHAmpl_R.next( lPos );
				}
			}
		}
/*
		CRect rc = pchromo->m_rectIdio;
		rc.OffsetRect( pchromo->m_ptOffset );

		pdc->FillRect( convert_from_object_to_wnd( rc ), &CBrush( RGB( 255, 0, 0 ) ) );
*/
	}

	if( m_nActiveLine == nLineIdx && m_nActiveCell == nCellIdx && m_nActiveChromo == nObjectIdx )
	{		
 		CRect rcMirror = rcObject - rcUpdate.TopLeft();		
		DrawMirror( pbmih, pdibBits, rcMirror, m_clrObjMirror );
	}

}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::draw_chromosome_image( CDC* pdc, chromo_object* pobj, 
									   CRect rectUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{
	if( !pobj )
		return;

	IDrawObject2Ptr		ptrDrawing = pobj->m_ptrChromos;
	IChromosomePtr		ptrChromo = pobj->m_ptrChromos;
	IImage3Ptr			ptrImage;	
	IColorConvertorExPtr	ptrCC;
	

	if( ptrChromo != 0 )
	{
		IUnknown* punkImage = 0;
		BOOL bOriginal = false;
		ptrChromo->GetEqualizedRotatedImage(  pobj->m_bErected ? 0 : AT_Original, &punkImage, &bOriginal );
		if( punkImage )
		{
			ptrImage = punkImage;			
			punkImage->Release();	punkImage = 0;			
		}

		if( ptrImage != 0 && m_bShowChromosome )
		{
			IUnknown* punkCC = 0;
			ptrImage->GetColorConvertor( &punkCC );
			if( punkCC )
			{
				ptrCC = punkCC;
				punkCC->Release();	punkCC = 0;
			}
		}
	}


	double fZoom = get_zoom();
	double fSumZoom = fZoom * pobj->m_fZoom;
	CPoint pointScroll = get_scroll_pos();

	CRect	rectDest;
	
	if( m_bShowChromosome && ptrImage != 0 )
	{
		rectDest = pobj->m_rectImage;
		rectDest.OffsetRect( pobj->m_ptOffset );
		rectDest = convert_from_object_to_wnd( rectDest );

		rectDest.IntersectRect( rectDest, rectUpdate );

		if( pbmih && pdibBits )
		{
			CPoint	pointImage = pobj->m_rectImage.TopLeft();
			pointImage+=pobj->m_ptOffset;

			CPoint	pt( pointImage.x*fZoom/fSumZoom,  
						pointImage.y*fZoom/fSumZoom );

			ptrCC->ConvertImageToDIBRect( pbmih, pdibBits, rectUpdate.TopLeft(), ptrImage, 
				rectUpdate, pt,
				fSumZoom, pointScroll, RGB( 0, 0, 0 ), 0 , this);
		}
 		else//must be printing
		{	
			IThumbnailManagerPtr ptrThumbMan( GetAppUnknown() );
			if( ptrThumbMan )
			{
				IUnknown* punkRenderObject = NULL;

				ptrThumbMan->GetRenderObject( ptrImage, &punkRenderObject );
				if( punkRenderObject )
				{
					IRenderObjectPtr ptrRenderObject( punkRenderObject );
					punkRenderObject->Release();	punkRenderObject = 0;

					if( ptrRenderObject )
					{
						byte	*pdib = 0;

						CSize size = CSize( pobj->m_rectImage.Width(), pobj->m_rectImage.Height() );

						short nSupported = 0;
						ptrRenderObject->GenerateThumbnail( ptrImage, 24, RF_BACKGROUND, m_clrBkg, 
									size, &nSupported, &pdib, this );

						int nOldMM = pdc->GetMapMode();
						CSize sizeOldVE = pdc->GetViewportExt();
						CSize sizeOldWE = pdc->GetWindowExt();
						CPoint ptOldVO = pdc->GetViewportOrg();
						CPoint ptOldWO = pdc->GetWindowOrg();
						
						
						CPoint ptOffset( ptOldVO.x, ptOldVO.y);

						pdc->LPtoDP( &rectDest );

						pdc->SetMapMode( MM_TEXT );
						pdc->SetWindowOrg( ptOffset );

						if( pdib != 0 )
						{
							ptrThumbMan->ThumbnailDraw( pdib, pdc->m_hDC, rectDest, 0 );
							ptrThumbMan->ThumbnailFree( pdib );
						}					
						
						pdc->SetMapMode( nOldMM );

						pdc->SetViewportExt( sizeOldVE );
						pdc->SetWindowExt( sizeOldWE );
						pdc->SetViewportOrg( ptOldVO );
						pdc->SetWindowOrg( ptOldWO );
						
					}
				}
			}			
		}
	}

	if( m_bShowChromosome && ptrDrawing != 0 )
	{		
//		rectDest = pobj->m_rectChromo;
		rectDest = pobj->m_rectImage;

		rectDest.OffsetRect( pobj->m_ptOffset );
		rectDest = convert_from_object_to_wnd( rectDest );

		DRAWOPTIONS draw_opt = {0};

//		draw_opt.ptOffset   = CPoint( ( rectDest.left ) / fSumZoom, ( rectDest.top ) / fSumZoom );
		draw_opt.ptScroll   = CPoint(-rectDest.left, -rectDest.top);
		draw_opt.dZoom		= fSumZoom;

		draw_opt.dwFlags	= AT_Rotated|AT_VisAngle|AT_Mirror;

		if( !pobj->m_bErected )
			draw_opt.dwFlags |= AT_Original;
		
		if( m_bShowChromoCentromere )
			draw_opt.dwFlags	|= AT_Centromere;

		if( m_bShowChromoCenterLine )
			draw_opt.dwFlags	|= AT_LongAxis;	

		// [vanek] BT2000: 3680 - use mask
		draw_opt.dwMask = domScroll | domZoom | domFlags | domOffset;
        		
		ptrDrawing->PaintEx( pdc->m_hDC, rectDest, 0/*GetViewPtr()*/, pbmih, pdibBits, &draw_opt );		
	}

	if( pobj->m_bShowIdiogramm && pobj->m_ptrIdio != 0 )
	{
		rectDest = pobj->m_rectIdio;
		  
		if( m_bShowCellIdio )
		{
			rectDest.left = rectDest.right - ( rectDest.Width() / 2 - INNER_INT + CGHAMPL_OFFSET_X );

//			if( !m_nShowNums && m_bShowCGHAmpl && pchromo->m_bHasCGHAmplL )
//				rect.left = rect.left + m_CGHAMPL_CX + CGHAMPL_OFFSET_X;

			if( !m_nShowNums && m_bShowCGHAmpl && pobj->m_bHasCGHAmplL )
			{
				rectDest.left += INNER_INT - CGHAMPL_OFFSET_X;
				rectDest.right += INNER_INT - CGHAMPL_OFFSET_X;
			}

			if( !m_nShowNums && m_bShowCGHAmpl && pobj->m_bHasCGHAmplR )
				rectDest.right -= CGHAMPL_OFFSET_X;

			if( !m_nShowNums && m_bShowCGHAmpl && pobj->m_bHasCGHAmplL )
				rectDest.right = rectDest.right - CGHAMPL_OFFSET_X;		
		}
	

		rectDest.OffsetRect( pobj->m_ptOffset );
		rectDest = convert_from_object_to_wnd( rectDest );

		pobj->m_ptrIdio->set_show_num( m_nShowNums );

		CSize sz = rectDest.Size();
		if( m_nShowNums )
			sz.cx = m_nIdioWidth * get_zoom() * pobj->m_fZoom;

		pobj->m_ptrIdio->draw( *pdc, sz, rectDest, 0, IDF_IGNORECENTROMERE|IDF_IGNORELENGTH, get_zoom() * pobj->m_fZoom );
	}

/********************/
	if( GetCarioViewMode() == cvmCellar && m_bShowEditableIdio && pobj->m_sptrEditableIdio != 0  )
	{
		rectDest = pobj->m_rectEditableIdio;

		rectDest.OffsetRect( pobj->m_ptOffset );
		rectDest = convert_from_object_to_wnd( rectDest );

		pobj->m_sptrEditableIdio->set_show_num( m_nShowEditableNums );
		CSize sz = rectDest.Size();
		if( m_nShowEditableNums )
			sz.cx = m_nIdioWidth * get_zoom() * pobj->m_fZoom;
		pobj->m_sptrEditableIdio->draw( *pdc, sz, rectDest, 0, IDF_IGNORECENTROMERE | IDF_IGNORELENGTH | IDF_RED, get_zoom() * pobj->m_fZoom );
	}

	if( m_bShowClasses && m_carioViewMode == cvmCellar )
	{
		CFont	*pold, font;
		int	nOldHeight = m_lfClassText.lfHeight;
		int nOldWeight = m_lfClassText.lfWeight;	

		if( m_carioViewMode != cvmMenu )
		{
			m_lfClassText.lfHeight*= get_zoom();
		}
		else
		{
			m_lfClassText.lfHeight=16;
			m_lfClassText.lfWeight=FW_BOLD;
		}

		font.CreateFontIndirect( &m_lfClassText );
		m_lfClassText.lfHeight = nOldHeight;
		m_lfClassText.lfWeight = nOldWeight;

		pold = pdc->SelectObject( &font );

		ICalcObjectPtr	ptr_calc( pobj->m_ptrMeasureObject );
		long	class_no = get_object_class( ptr_calc );

		CString strClassName = get_class_name( class_no );

		if( !pobj->m_strTextInfo.IsEmpty() )
			strClassName = pobj->m_strTextInfo;

		CRect rcText = pobj->m_rectText;

		int dx = ( pobj->m_rectFull.Width() - rcText.Width() ) / 2;

		if( dx < 0 )
			dx = 0;

		rcText.OffsetRect( pobj->m_ptOffset );
		rcText.OffsetRect( dx, 0 );
		rcText = convert_from_object_to_wnd( rcText );

		int nMode = pdc->SetBkMode( TRANSPARENT );
		COLORREF clrOldText = pdc->SetTextColor( m_clrClassText );
		
		pdc->DrawText( strClassName, rcText, DT_VCENTER | DT_CENTER | DT_WORDBREAK );

		pdc->SetBkMode( nMode );
		pdc->SetTextColor( clrOldText );

		pdc->SelectObject( pold );
	}
/********************/

	bool bOK = true;
	if( m_carioViewMode == cvmCellar && !m_bFromPrint )
	{
		if( m_nActiveLine == -1 || m_nActiveCell == -1 || m_nActiveChromo == -1 )
			bOK = false;
		else
		{
			chromo_object* pobject = 0;
			get_pointers( m_nActiveLine, m_nActiveCell, m_nActiveChromo, 0, 0, &pobject );

			if( pobject && pobject->m_rectImage != pobj->m_rectImage )
				bOK = false;
		}
	}

	if( bOK && m_bShowCGHGraph && pobj->m_sptrChart != 0 && m_bShowGradient )
	{
 		rectDest = pobj->m_rectCGHGraph;
		rectDest.OffsetRect( pobj->m_ptOffset );

		CRect rc = rectDest;

		rc = convert_from_object_to_wnd( rc );

		TRIVERTEX        vert[2];
		GRADIENT_RECT    gRect;

		vert [0] .x      = rc.left;
		vert [0] .y      = rc.top;
		vert [0] .Red    = GetRValue( m_clGradient ) * 0x100;
		vert [0] .Green  = GetGValue( m_clGradient ) * 0x100;
		vert [0] .Blue   = GetBValue( m_clGradient ) * 0x100;
		vert [0] .Alpha  = 0x0000;

		vert [1] .x      = rc.right;
		vert [1] .y      = rc.bottom; 
		vert [1] .Red    = GetRValue( m_clGradient2 ) * 0x100;
		vert [1] .Green  = GetGValue( m_clGradient2 ) * 0x100;
		vert [1] .Blue   = GetBValue( m_clGradient2 ) * 0x100;
		vert [1] .Alpha  = 0x0000;

		gRect.UpperLeft  = 0;
		gRect.LowerRight = 1;

		::GradientFill( *pdc, vert, 2, &gRect, 1, GRADIENT_FILL_RECT_H );
	}

	bool bChange = false;

	CSize sizeOldVE = pdc->GetViewportExt();
	CSize sizeOldWE = pdc->GetWindowExt();

	if( ::GetDeviceCaps( pdc->m_hDC, TECHNOLOGY ) != DT_RASDISPLAY )
		bChange = true;

	if( bChange )
	{
		IChartMiscHelperPtr ptrHelper = pobj->m_sptrChart;

		if( ptrHelper == 0 )
			ptrHelper = pobj->m_sptrRatio;

		if( ptrHelper != 0 )
		{
			POINT pt;
			pt.x = sizeOldWE.cx;
			pt.y = sizeOldWE.cy;

			ptrHelper->DPtoLP( DWORD( pdc->m_hDC ), &pt );

			pdc->SetViewportExt( CSize( pt.x, pt.y ) );
		}
	}


	if( bOK && m_bShowCGHGraph && pobj->m_sptrChart != 0 )
	{		
		rectDest = pobj->m_rectCGHGraph;
		rectDest.OffsetRect( pobj->m_ptOffset );

		CRect rc = rectDest;

		rc = convert_from_object_to_wnd( rc );


		pobj->m_sptrChart->Draw( DWORD( pdc->m_hDC ), rc );
	}

	if( m_bShowCGHRatio && pobj->m_sptrRatio != 0 )
	{		
		rectDest = pobj->m_rectCGHRatio;
		rectDest.OffsetRect( pobj->m_ptOffset );
		rectDest = convert_from_object_to_wnd( rectDest );

		pobj->m_sptrRatio->Draw( DWORD( pdc->m_hDC ), rectDest );
	}

	if( bChange )
		pdc->SetViewportExt( sizeOldVE );

	if( m_bShowCGHRatio && pobj->m_sptrRatio != 0 )
	{		
		rectDest = pobj->m_rectCGHRatio;
		rectDest.OffsetRect( pobj->m_ptOffset );
		rectDest = convert_from_object_to_wnd( rectDest );

		CFont Fnt; 
	
		int	nOldHeight = m_lpRatioText.lfHeight;

		m_lpRatioText.lfHeight *= get_zoom();
		Fnt.CreateFontIndirect( &m_lpRatioText );
		m_lpRatioText.lfHeight = nOldHeight;

		CFont *fnt = pdc->SelectObject(  &Fnt );

		int dx = rectDest.Width() / m_nRatioTextRCCount;
		for( int i = 0; i < m_nRatioTextRCCount + 1; i++ )
		{
			CRect rc;
			pdc->DrawText( pobj->m_pstrRatioTextUP[i], &rc, DT_CALCRECT );

			rc = CRect( CPoint( rectDest.left - rc.Width() / 2 + dx * i, rectDest.top - rc.Height() ), CSize( rc.Width(), rc.Height() ) );

			int nMode = pdc->SetBkMode( TRANSPARENT );
			COLORREF clrOldText = pdc->SetTextColor( m_clrClassText );
			
			pdc->DrawText( pobj->m_pstrRatioTextUP[i], rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

			pdc->SetBkMode( nMode );
			pdc->SetTextColor( clrOldText );
		}
		if( !pobj->m_strRatioTextDOWN.IsEmpty() )
		{
			CRect rc;
			pdc->DrawText( pobj->m_strRatioTextDOWN, &rc, DT_CALCRECT );

			rc = CRect( CPoint( rectDest.left, rectDest.bottom ), CSize( rc.Width(), rc.Height() ) );

			int nMode = pdc->SetBkMode( TRANSPARENT );
			COLORREF clrOldText = pdc->SetTextColor( m_clrClassText );
			
			pdc->DrawText( pobj->m_strRatioTextDOWN, rc, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

			pdc->SetBkMode( nMode );
			pdc->SetTextColor( clrOldText );
		}

		pdc->SelectObject( fnt );

	}

	if( bOK && m_bShowCGHGraph && pobj->m_sptrChart != 0 )
	{		
		rectDest = pobj->m_rectCGHGraph;
		rectDest.OffsetRect( pobj->m_ptOffset );

		CRect rc = rectDest;

		rc = convert_from_object_to_wnd( rc );

		CPen pen( PS_SOLID, 1, m_clBrightBorder ), pen2( PS_SOLID, 1, m_clBrightBorder2 ), *pOld = 0;

		pOld = pdc->SelectObject( &pen2 );
		
		pdc->MoveTo( rc.left, rc.bottom );
		pdc->LineTo( rc.left, rc.top );
		pdc->LineTo( rc.right, rc.top );

		pdc->SelectObject( &pen );

		pdc->MoveTo( rc.right, rc.top );
		pdc->LineTo( rc.right, rc.bottom );
		pdc->LineTo( rc.left, rc.bottom );

		pdc->SelectObject( pOld );
	}

	if( !m_bActionExecuted && GetAsyncKeyState( _MASTER_KEY_ ) & 0x8000 )
	{
		short sType = 0;

		CPoint ptClient;
		::GetCursorPos( &ptClient );

		CWnd *pView = GetWindowFromUnknown( GetViewPtr() );
		
		if( pView )
			pView->ScreenToClient( &ptClient );

		if( !sType )
		{
			rectDest = pobj->m_rectCGHGraph;
			rectDest.OffsetRect( pobj->m_ptOffset );
			rectDest = convert_from_object_to_wnd( rectDest );
			
			if( rectDest.PtInRect( ptClient ) )
				sType = 1;
		}

		if( !sType )
		{
			rectDest = pobj->m_rectCGHRatio;
			rectDest.OffsetRect( pobj->m_ptOffset );
			rectDest = convert_from_object_to_wnd( rectDest );

			if( rectDest.PtInRect( ptClient ) )
				sType = 2;
		}

		if( sType )
		{
			CPen pn( PS_SOLID, 1, m_clTargetLine );
			CPen *pOldPen = pdc->SelectObject( &pn );

			pdc->MoveTo( rectDest.left, ptClient.y );
			pdc->LineTo( rectDest.right, ptClient.y );

			pdc->SelectObject( pOldPen );

		}
	}

	if( m_bShowAdditionLines && m_bShowChromosome && GetCarioViewMode() == cvmCellar )
	{
		CPen pen( PS_SOLID, 1, m_clAddLinesColor );
		CPen *pOld = pdc->SelectObject( &pen );
		for( int i = 0; i < pobj->m_nLinesCount; i++ )
		{
			FRECT rectDest = pobj->m_strAddLines[i].m_ptsLines;

			rectDest.OffsetRect( pobj->m_ptOffset );
			rectDest.OffsetRect( pobj->m_rectImage.TopLeft() );

			{
				CPoint ptScroll = get_scroll_pos();
				double fZoom = get_zoom();

				FRECT rcWnd;

				rcWnd.left		= rectDest.left * fZoom - ptScroll.x;
				rcWnd.top		= rectDest.top * fZoom - ptScroll.y;
				rcWnd.right		= rectDest.right * fZoom - ptScroll.x;
				rcWnd.bottom	= rectDest.bottom * fZoom - ptScroll.y;

				rectDest = rcWnd;
			}

			if( CPoint( rectDest.left, rectDest.top ) != CPoint( rectDest.right, rectDest.bottom ) ) 
			{
				::MoveToEx( pdc->m_hDC, rectDest.left, rectDest.top, 0 );
				::LineTo( pdc->m_hDC, rectDest.right, rectDest.bottom );
			}
		}
		pdc->SelectObject( pOld );
	}


}


/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::invalidate_rect( CRect* prect )
{
	if( prect )
	{	
		CRect rc = *prect;
		rc.InflateRect( 1, 1, 1, 1 );
		InvalidateRect( GetHwnd(), &rc, TRUE );
	}
	else
		InvalidateRect( GetHwnd(), 0, TRUE );

	
}

/////////////////////////////////////////////////////////////////////////////
MouseMode CCarioImpl::get_mouse_mode_by_point( CPoint ptObj, int *pnLine, int *pnCell, int *pnPos )
{
	if( m_carioViewMode == cvmNotDefine )
		return mmNone;

	int nLineIdx	= -1;
	int nCellIdx	= -1;
	int nChromoIdx	= -1;

	if( pnLine )*pnLine = -1;
	if( pnCell )*pnCell = -1;
	if( pnPos )*pnPos = -1;

	MouseMode mm = mmNone; 
	chromo_line	*pline;

	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		CRect	rcLine = get_chromo_line_rect( i, &pline, true );

		if( !(ptObj.x >= rcLine.left && ptObj.x < rcLine.right) )
			continue;

		if( m_carioViewMode == cvmDesign )
		{
	/*		if( ptObj.y >= rcLine.top - m_nResizeSense &&
				ptObj.y < rcLine.top + m_nResizeSense )
				return mmResizeLineUp;*/

			if( ptObj.y >= rcLine.bottom - m_nResizeSense &&
				ptObj.y < rcLine.bottom + m_nResizeSense )
			{
				if( pnLine )*pnLine = i;
				return mmResizeLineDown;
			}
		}

		if( ptObj.y >= rcLine.top && 
			ptObj.y < rcLine.bottom )
		{
			if( pnLine )*pnLine = i;
			nLineIdx = i;
			break;
		}
	}

	if( nLineIdx == -1 )return mmNone;

	chromo_cell* pcell = 0;

	for( int j=0;j<pline->m_arCell.GetSize();j++ )
	{
		pcell = pline->m_arCell[j];
				
		CRect rcCell = get_chromo_cell_rect( nLineIdx, j, 0, &pcell, true );
		if( m_carioViewMode == cvmDesign )
		{
	/*		if( ptObj.x >= rcCell.left - m_nResizeSense &&
				ptObj.x < rcCell.left + m_nResizeSense )
				return mmResizeCellLeft;*/

			if( ptObj.x >= rcCell.right - m_nResizeSense &&
				ptObj.x < rcCell.right + m_nResizeSense )
			{
				if( pnCell )*pnCell = j;
				return mmResizeCellRight;
			}
		}

		if( ptObj.x >= rcCell.left&&
			ptObj.x < rcCell.right )
		{
			if( pnCell )*pnCell = j;
			nCellIdx = j;
			break;
		}
	}

	if( nCellIdx == -1 )return mmNone;
	if( m_carioViewMode == cvmDesign )return mmMoveCell;

	for( int k=0;k<pcell->m_arChromos.GetSize();k++ )
	{	
		CRect rcChromo = get_chromo_object_rect( nLineIdx, nCellIdx, k, 0, 0, 0 );
		if( rcChromo.PtInRect( ptObj ) )
		{
			if( pnPos )*pnPos = k;
			nChromoIdx = k;

			if( GetAsyncKeyState( VK_CONTROL ) != 0 )
			{
				if( m_chromoAlign == acCenterLine ) return mmSetChromoOffset;
				else return mmNone;
			}
			else
				return mmMoveChromo;
		}
	}	

	return mmNone;
}

/////////////////////////////////////////////////////////////////////////////
void CCarioImpl::hit_test_area( CPoint ptObj, int* pnLineIdx, int* pnCellIdx, int* pnChromoIdx, bool bTakeLineHeight )
{
	int nLineIdx	= -1;
	int nCellIdx	= -1;
	int nChromoIdx	= -1;

	bool bFoundLine		= false;
	bool bFoundCell		= false;
	bool bFoundChromo	= false;

	MouseMode mm = mmNone; 

	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		if( bFoundLine )
			break;

		chromo_line* pline = 0;
		CRect rcLine = get_chromo_line_rect( i, &pline );
		if( !pline )
			continue;

		if( ptObj.y >= rcLine.top - m_nResizeSense &&
			ptObj.y < rcLine.bottom + m_nResizeSense )
		{
			bFoundLine = true;			
			nLineIdx = i;

			for( int j=0;j<pline->m_arCell.GetSize();j++ )
			{
				if( bFoundCell )
					break;

				chromo_cell* pcell = pline->m_arCell[j];
				
				CRect rcCell = get_chromo_cell_rect( i, j, 0, 0 );
				if( ptObj.x >= rcCell.left - m_nResizeSense &&
					ptObj.x < rcCell.right + m_nResizeSense )
				{					
					bFoundCell = true;
					nCellIdx = j;

					if( m_carioViewMode != cvmDesign )
					{
						for( int k=0;k<pcell->m_arChromos.GetSize();k++ )
						{	
							if( bFoundChromo )
								break;

							CRect rcChromo = get_chromo_object_rect( nLineIdx, nCellIdx, k, 0, 0, 0 );
							if( bTakeLineHeight )
							{
								rcChromo.top	= rcLine.top;
								rcChromo.bottom	= rcLine.bottom;
							}

							if( rcChromo.PtInRect( ptObj ) )
							{
								bFoundChromo = true;
								nChromoIdx = k;
							}
						}
					}
				}
			}
		}
	}	

	if( pnLineIdx )
		*pnLineIdx = nLineIdx;
		
	if( pnCellIdx )
		*pnCellIdx = nCellIdx;
		
	if( pnChromoIdx )
		*pnChromoIdx = nChromoIdx;
	
}

/////////////////////////////////////////////////////////////////////////////
HCURSOR CCarioImpl::get_cursor_by_mode( MouseMode mm )
{
	if( mm == mmMoveCell )
		return m_hCursorMoveCell;
	else if( mm == mmMoveChromo  )
		return m_hCursorMoveChromo;
	else if( /*mm == mmResizeLineUp ||*/ mm == mmResizeLineDown )
		return m_hCursorVertSize;
	else if( /*mm == mmResizeCellLeft ||*/ mm == mmResizeCellRight )
		return m_hCursorHorzSize;

	return m_hCursorArrow;
}



/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::reload_view_settings( IUnknown* punkND )
{
	if( !CheckInterface( punkND, IID_INamedData ) )
		return false;

	CString strSect = get_view_shared_section();	
	if( strSect.IsEmpty() )
		return false;

	bool bCanExit = ( !CheckInterface( punkND, IID_IApplication ) );

	if( bCanExit )
	{
		if( 1L != ::GetValueInt( punkND, strSect, "SettingSave", 0L ) )
			return false;
	}

	m_clTargetLine			= ::GetValueColor( punkND, strSect, "TargetLine", RGB( 0, 128, 0 ) );
	m_clrClassText			= ::GetValueColor( punkND, strSect, "TextClassTitle", RGB( 0, 0, 128 ) );
	m_clrBkg				= ::GetValueColor( punkND, strSect, "BackGroundColor", RGB( 255, 255, 255 ) );
	
	m_clrLine				= ::GetValueColor( punkND, strSect, "LineColor", RGB( 235, 235, 235 ) );
	m_clrLineActive			= ::GetValueColor( punkND, strSect, "ActiveLineColor", RGB( 255, 235, 255 ) );

	m_clrCell				= ::GetValueColor( punkND, strSect, "CellColor", RGB( 255, 255, 255 ) );
	m_clrCellActive			= ::GetValueColor( punkND, strSect, "ActiveCellColor", RGB( 255, 255, 235 ) );

	m_clrs_cell[0]			= ::GetValueColor( punkND, strSect, "CellObject0", RGB( 192, 192, 192 ) );
	m_clrs_cell[1]			= ::GetValueColor( punkND, strSect, "CellObject1", RGB( 140, 198, 198 ) );
	m_clrs_cell[2]			= ::GetValueColor( punkND, strSect, "CellObject2", RGB( 148, 222, 116 ) );
	m_clrs_cell[3]			= ::GetValueColor( punkND, strSect, "CellObject3", RGB( 227, 129, 111 ) );
	m_clrs_cell[4]			= ::GetValueColor( punkND, strSect, "CellObject4", RGB( 235, 142, 103 ) );
	m_clrs_cell[5]			= ::GetValueColor( punkND, strSect, "CellObject5", RGB( 255, 83, 83 ) );

	m_clrLineBorder			= ::GetValueColor( punkND, strSect, "LineBorderColor", RGB( 0, 0, 0 ) );
	m_clrCellBorder			= ::GetValueColor( punkND, strSect, "CellBorderColor", RGB( 0, 0, 0 ) );

	m_clrObjMirror			= ::GetValueColor( punkND, strSect, "ObjectMirrorColor", RGB( 225, 225, 225 ) );

	m_clrAlignLine			= ::GetValueColor( punkND, strSect, "AlignLineColor", RGB( 0, 0, 0 ) );


	m_nMinLineWidth			= ::GetValueInt( punkND, strSect, "MinLineWidth", 100 );	
	m_nMinLineHeight		= ::GetValueInt( punkND, strSect, "MinLineHeight", 100 );
	m_nMinCellWidth			= ::GetValueInt( punkND, strSect, "MinCellWidth", 100 );

	m_sizeDefaultLine.cx	= ::GetValueInt( punkND, strSect, "DefaultLineWidth", 120 );
	m_sizeDefaultLine.cy	= ::GetValueInt( punkND, strSect, "DefaultLineHeight", 100 );
	
	m_chromoAlign			= (AlignChromo)::GetValueInt( punkND, strSect, "ChromoAlign", (long)acTop );

	m_nResizeSense			= ::GetValueInt( punkND, strSect, "ResizeSense", 5 );

	m_clRatioMin = ::GetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_MIN_CHART, m_clRatioMin );
	m_clRatioMax = ::GetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_MAX_CHART, m_clRatioMax );
	m_clRatioMid = ::GetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_MID_CHART, m_clRatioMid );
	m_clBright   = ::GetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART,	  m_clBright );
	m_clBright2   = ::GetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART2,	  m_clBright2 );

	m_clBrightBorder  = ::GetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_BORDER,	  m_clBrightBorder );
	m_clBrightBorder2  = ::GetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_BORDER2,	  m_clBrightBorder2 );

	m_clGradient  = ::GetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_GRADIENT,	  m_clGradient );
	m_clGradient2  = ::GetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_GRADIENT2,	  m_clGradient2 );
	
	m_bShowGradient	= ::GetValueInt( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_SHOW, m_bShowGradient ) != 0;

	

	m_clRatioBorder   = ::GetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_BORDER_CHART,	  m_clRatioBorder );
	m_clRatioGrad   = ::GetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_GRAD_CHART,	  m_clRatioGrad );
	m_clRatioProb = ::GetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_PROB_CHART,	  m_clRatioProb );

	m_fNormalMin = ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIOMIN,	  m_fNormalMin );
	m_fNormalMid = ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIOMID,	  m_fNormalMid );
	m_fNormalMax = ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIOMAX,	  m_fNormalMax );
	m_fRatioStepVal = ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIO_STEP,	  m_fRatioStepVal );
	m_fProbability = ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIO_PROBABILITY,	 m_fProbability );

	m_bShowProb = ::GetValueInt( punkND, CGH_ROOT, CGH_RATIO_SHOWPROB,	 m_bShowProb ) != 0;
	m_bShowLineOnRatio = ::GetValueInt( punkND, CGH_ROOT, CGH_RATIO_SHOWAXIS,	 m_bShowLineOnRatio ) != 0;
	m_bUseAutoScaleRatio = ::GetValueInt( punkND, CGH_ROOT, CGH_RATIO_AUTOSCALE,	 m_bUseAutoScaleRatio ) != 0;
	m_bErectAfterDrop = ::GetValueInt( punkND, CGH_ROOT, CARIO_ERECT_AFTER_DROP,	 m_bErectAfterDrop ) != 0;
	
	
	m_fMinRatioVal = ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIO_MINRANGE,	  m_fMinRatioVal );
	m_fMaxRatioVal = ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIO_MAXRANGE,	  m_fMaxRatioVal );

	m_nRatioTextRCCount = ( m_fMaxRatioVal - m_fMinRatioVal) / ::GetValueDouble( punkND, CGH_ROOT, CGH_RATIO_TEXT_STEP, (m_fMaxRatioVal - m_fMinRatioVal ) / m_nRatioTextRCCount );

	if( !m_nRatioTextRCCount )
		m_nRatioTextRCCount = 1;

	m_CGHGRAPH_CX = ::GetValueInt( punkND, CGH_ROOT, CARIO_CX_BRIGHT,   m_CGHGRAPH_CX );
	m_CGHRATIO_CX = ::GetValueInt( punkND, CGH_ROOT, CARIO_CX_RATIO,   m_CGHRATIO_CX );
	m_CGHAMPL_CX  = ::GetValueInt( punkND, CGH_ROOT, CARIO_CX_AMPL,   m_CGHAMPL_CX );

	m_nPosID_C  = ::GetValueInt( punkND, strSect, CARIO_POS_CHROME, m_nPosID_C );
	m_nPosID_G  = ::GetValueInt( punkND, strSect, CARIO_POS_BRIGHT, m_nPosID_G );
	m_nPosID_I  = ::GetValueInt( punkND, strSect, CARIO_POS_IDIO,   m_nPosID_I );
	m_nPosID_R  = ::GetValueInt( punkND, strSect, CARIO_POS_RATIO,   m_nPosID_R );
	m_nPosID_E  = ::GetValueInt( punkND, strSect, CARIO_POS_E_IDIO,   m_nPosID_E );

	m_nShowNums = ::GetValueInt( punkND, strSect, CARIO_SHOW_NUMS, m_nShowNums );
	m_nShowEditableNums = ::GetValueInt( punkND, strSect, CARIO_SHOW_EDITABLE_NUMS, m_nShowEditableNums );

	m_nIdioWidth = GetValueInt( punkND, strSect, KARYO_IDIOWIDTH, m_nIdioWidth );
	m_nIdioHeight = GetValueInt( punkND, strSect, KARYO_IDIOHEIGHT, m_nIdioHeight );

	strSect = get_view_settings_section();

    m_bShowClasses = ::GetValueInt( punkND, strSect, CARIO_SHOW_CLASSES, (long)m_bShowClasses ) != 0;
	m_bShowEditableIdio = ::GetValueInt( punkND, strSect, CARIO_EDITABLE_IDIO, (long)m_bShowEditableIdio ) != 0;
	m_bShowChromosome	= ::GetValueInt( punkND, strSect, CARIO_SHOW_OBJECT, (long)m_bShowChromosome ) != 0;

	m_bShowCGHRatio = ::GetValueInt( punkND, strSect, CARIO_SHOW_CGHRATIO, (long)m_bShowCGHRatio ) != 0;
	m_bShowCGHAmpl = ::GetValueInt( punkND, strSect, CARIO_SHOW_CGHAMPL, (long)m_bShowCGHAmpl ) != 0;

	m_bShowCGHGraph		= ::GetValueInt( punkND, strSect, CARIO_SHOW_CGHBRIGHT, (long)m_bShowCGHGraph ) != 0;
	m_bShowAdditionLines = ::GetValueInt( punkND, strSect, CARIO_ADD_LINES, (long)m_bShowAdditionLines ) != 0;

	m_clAddLinesColor = ::GetValueColor( punkND, strSect, CARIO_ADD_LINES_COLOR, m_clAddLinesColor );

	m_bShowClassNumber		= ( 1L == ::GetValueInt( punkND, strSect, "ShowClassNumber", 1 ) );
	m_bShowChromoContures	= ( 1L == ::GetValueInt( punkND, strSect, "ShowChromoContures", 1 ) );
	m_bShowChromoCenterLine	= ( 1L == ::GetValueInt( punkND, strSect, "ShowChromoCenterLine", 1 ) );
	m_bShowChromoCentromere	= ( 1L == ::GetValueInt( punkND, strSect, "ShowChromoCentromere", 1 ) );
	
	m_bNeedChromoAlignLine =  ( 1L == ::GetValueInt( punkND, strSect, "ShowChromoAlignmentLine", 1 ) );
	m_bShowCellIdio =  ( 1L == ::GetValueInt( punkND, strSect, "ShowCellIdio", false ) );
	
	load_idiograms();
	
	

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CCarioImpl::save_view_settings( IUnknown* punkND )
{
	if( !CheckInterface( punkND, IID_INamedData ) )
		return false;

	CString strSect = get_view_shared_section();
	if( strSect.IsEmpty() )
		return false;

	::SetValue( punkND, strSect, "SettingSave", 1L );

	::SetValueColor( punkND, strSect, "BackGroundColor", m_clrBkg );
	::SetValueColor( punkND, strSect, "TextClassTitle", m_clrClassText );

	
	::SetValueColor( punkND, strSect, "LineColor", m_clrLine );
	::SetValueColor( punkND, strSect, "ActiveLineColor", m_clrLineActive );

	::SetValueColor( punkND, strSect, "CellColor", m_clrCell );
	::SetValueColor( punkND, strSect, "ActiveCellColor", m_clrCellActive );

	::SetValueColor( punkND, strSect, "CellObject0", m_clrs_cell[0] );
	::SetValueColor( punkND, strSect, "CellObject1", m_clrs_cell[1] );
	::SetValueColor( punkND, strSect, "CellObject2", m_clrs_cell[2] );
	::SetValueColor( punkND, strSect, "CellObject3", m_clrs_cell[3] );
	::SetValueColor( punkND, strSect, "CellObject4", m_clrs_cell[4] );
	::SetValueColor( punkND, strSect, "CellObject5", m_clrs_cell[5] );

	::SetValueColor( punkND, strSect, "LineBorderColor", m_clrLineBorder );
	::SetValueColor( punkND, strSect, "CellBorderColor", m_clrCellBorder );

	::SetValueColor( punkND, strSect, "ObjectMirrorColor", m_clrObjMirror );

	::SetValueColor( punkND, strSect, "AlignLineColor", m_clrAlignLine );



	::SetValue( punkND, strSect, "MinLineWidth", (long)m_nMinLineWidth );	
	::SetValue( punkND, strSect, "MinLineHeight", (long)m_nMinLineHeight );
	::SetValue( punkND, strSect, "MinCellWidth", (long)m_nMinCellWidth );

	::SetValue( punkND, strSect, "DefaultLineWidth",(long)m_sizeDefaultLine.cx );
	::SetValue( punkND, strSect, "DefaultLineHeight", (long)m_sizeDefaultLine.cy );
	
	::SetValue( punkND, strSect, "ChromoAlign", (long)m_chromoAlign );

	::SetValue( punkND, strSect, "ResizeSense", (long)m_nResizeSense );


	::SetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_MIN_CHART, m_clRatioMin );
	::SetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_MAX_CHART, m_clRatioMax );
	::SetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_MID_CHART, m_clRatioMid );
	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART,	m_clBright );
	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART2,	m_clBright2 );

	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_BORDER,	  m_clBrightBorder );
	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_BORDER2,	  m_clBrightBorder2 );

	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_BORDER,	  m_clBrightBorder );
	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_BORDER2,	  m_clBrightBorder2 );

	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_GRADIENT,	  m_clGradient );
	::SetValueColor( punkND, CGH_ROOT, CGH_BRIGHT_COLOR_CHART_GRADIENT2,	  m_clGradient2 );
    
	::SetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_BORDER_CHART,	  m_clRatioBorder );

	::SetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_GRAD_CHART,	  m_clRatioGrad );
	::SetValueColor( punkND, CGH_ROOT, CGH_RATIO_COLOR_PROB_CHART,	  m_clRatioProb );

	::SetValue( punkND, CGH_ROOT, CGH_RATIOMIN,	  (double)m_fNormalMin );
	::SetValue( punkND, CGH_ROOT, CGH_RATIOMID,	  (double)m_fNormalMid );
	::SetValue( punkND, CGH_ROOT, CGH_RATIOMAX,	  (double)m_fNormalMax );
	::SetValue( punkND, CGH_ROOT, CGH_RATIO_STEP, (double)m_fRatioStepVal );
	::SetValue( punkND, CGH_ROOT, CGH_RATIO_STEP, (double)m_fRatioStepVal );
	::SetValue( punkND, CGH_ROOT, CGH_RATIO_PROBABILITY, (double)m_fProbability );

	::SetValue( punkND, CGH_ROOT, CGH_RATIO_SHOWPROB,	 (long)m_bShowProb );
	::SetValue( punkND, CGH_ROOT, CGH_RATIO_SHOWAXIS,	 (long)m_bShowLineOnRatio );
	::SetValue( punkND, CGH_ROOT, CGH_RATIO_AUTOSCALE,	 (long)m_bUseAutoScaleRatio );

	::SetValue( punkND, CGH_ROOT, CGH_RATIO_MINRANGE,	  (double)m_fMinRatioVal );
	::SetValue( punkND, CGH_ROOT, CGH_RATIO_MAXRANGE,	  (double)m_fMaxRatioVal );


	::SetValue( punkND, CGH_ROOT, CARIO_CX_BRIGHT,   (long)m_CGHGRAPH_CX );
	::SetValue( punkND, CGH_ROOT, CARIO_CX_RATIO,   (long)m_CGHRATIO_CX );
	::SetValue( punkND, CGH_ROOT, CARIO_CX_AMPL,   (long)m_CGHAMPL_CX );

	::SetValue( punkND, strSect, CARIO_POS_CHROME, (long)m_nPosID_C );
	::SetValue( punkND, strSect, CARIO_POS_BRIGHT, (long)m_nPosID_G );
	::SetValue( punkND, strSect, CARIO_POS_IDIO,   (long)m_nPosID_I );
	::SetValue( punkND, strSect, CARIO_POS_RATIO,   (long)m_nPosID_R );
	::SetValue( punkND, strSect, CARIO_POS_E_IDIO,   (long)m_nPosID_E );
	
	::SetValue( punkND, strSect, CGH_RATIO_AUTOSCALE,   (long)m_bUseAutoScaleRatio );

	::SetValue( punkND, strSect, KARYO_IDIOWIDTH, m_nIdioWidth );
	::SetValue( punkND, strSect, KARYO_IDIOHEIGHT, m_nIdioHeight );

	::SetValue( punkND, strSect, CARIO_SHOW_NUMS, (long)m_nShowNums );
	::SetValue( punkND, strSect, CARIO_SHOW_EDITABLE_NUMS, (long)m_nShowEditableNums );

	strSect = get_view_settings_section();
    ::SetValue( punkND, strSect, CARIO_SHOW_CLASSES, (long)m_bShowClasses );

	::SetValue( punkND, strSect, CARIO_EDITABLE_IDIO, (long)m_bShowEditableIdio );
	::SetValue( punkND, strSect, CARIO_SHOW_OBJECT, (long)m_bShowChromosome );

	::SetValue( punkND, strSect, CARIO_SHOW_CGHRATIO, (long)m_bShowCGHRatio );
	::SetValue( punkND, strSect, CARIO_SHOW_CGHAMPL, (long)m_bShowCGHAmpl );

	::SetValue( punkND, strSect, CARIO_SHOW_CGHBRIGHT, (long)m_bShowCGHGraph );
	::SetValue( punkND, strSect, CARIO_ADD_LINES, (long)m_bShowAdditionLines );

	::SetValueColor( punkND, strSect, CARIO_ADD_LINES_COLOR, (long)m_clAddLinesColor );


	::SetValue( punkND, strSect, "ShowClassNumber", (long)( m_bShowClassNumber ? 1 : 0 ) );
	::SetValue( punkND, strSect, "ShowChromoContures", (long) (m_bShowChromoContures ? 1 : 0 ) );
	::SetValue( punkND, strSect, "ShowChromoCenterLine", (long)( m_bShowChromoCenterLine ? 1 : 0 ) );
	::SetValue( punkND, strSect, "ShowChromoCentromere", (long)( m_bShowChromoCentromere ? 1 : 0 ) );

	::SetValue( punkND, strSect, "ShowChromoAlignmentLine", (long)( m_bNeedChromoAlignLine ? 1 : 0 ) );
	::SetValue( punkND, strSect, "ShowCellIdio", (long)m_bShowCellIdio );

	return true;
}


chromo_object	*CCarioImpl::get_chromo_object( long line, long cell, long pos )
{
	if( line < 0 || line >= m_arCarioLine.GetSize() )
		return 0;
	chromo_line *pl = m_arCarioLine[line];

	if( cell < 0 || cell >= pl->m_arCell.GetSize() )
		return 0;

	chromo_cell	*pc = pl->m_arCell[cell];

	if( pos < 0 || pos >= pc->m_arChromos.GetSize() )
		return 0;

	return pc->m_arChromos[pos];
}

void CCarioImpl::update_classes()
{
	for( int i=0;i<m_arCarioLine.GetSize();i++ )
	{
		chromo_line* pline = m_arCarioLine[i];		

		for( int j=0;j<pline->m_arCell.GetSize();j++ )
		{
			chromo_cell* pcell = pline->m_arCell[j];
			long	class_no = pcell->m_nClassNum;

			for( int k=pcell->m_arChromos.GetSize()-1; k>=0; k-- )
			{
				chromo_object	*pchromo = pcell->m_arChromos[k];
				set_chromo_class( pchromo, class_no );


			}
		}
	}
}

//переместить все объекты в соответствии с номерами их классов (за исключением подвала)
bool CCarioImpl::arrage_by_classes()
{
	long	line, cell, obj;

	_list_t<chromo_object *>	objects;

	chromo_line* pline = _get_cellar();
	chromo_cell	*pcellar = pline->m_arCell.GetAt( 0 );
	
	for( line=0; line<m_arCarioLine.GetSize(); line++ )
	{
		chromo_line* pline = m_arCarioLine[line];		
		for( cell=0; cell<pline->m_arCell.GetSize(); cell++ )
		{
			chromo_cell* pcell = pline->m_arCell[cell];

			for( obj=0; obj < pcell->m_arChromos.GetSize(); obj++ )
				objects.insert_before( pcell->m_arChromos.GetAt( obj ), 0 );
			pcell->m_arChromos.SetSize(0);
		}
	}

	for( line=0; line<m_arCarioLine.GetSize()-1; line++ )
	{
		chromo_line* pline = m_arCarioLine[line];		
		for( cell=0; cell<pline->m_arCell.GetSize(); cell++ )
		{
			chromo_cell* pcell = pline->m_arCell[cell];
			long	class_no = pcell->m_nClassNum;

			long	lpos_next;
			for( long lpos = objects.head(); lpos; lpos = lpos_next )
			{
				lpos_next = objects.next( lpos );

				chromo_object	*po = objects.get( lpos );

				if( get_object_class( ICalcObjectPtr(po->m_ptrMeasureObject) ) == class_no )
				{	
					pcell->m_arChromos.Add( po );
					objects.remove( lpos );
				}
			}
		}
	}

	for( long lpos = objects.head(); lpos; lpos = objects.next( lpos ) )
		pcellar->m_arChromos.Add( objects.get( lpos ) );
	
	synchronize_dimmension( SDF_NORECALC );
	return true;
}

CString CCarioImpl::get_idio_hint( int nLine, int nCell, int nChromo, CPoint pt )
{
	if( nLine >= 0 && nLine < m_arCarioLine.GetSize() )
	{
		chromo_line* pline = m_arCarioLine[nLine];		
		if( nCell >= 0 && nCell< pline->m_arCell.GetSize() )
		{
			chromo_cell* pcell = pline->m_arCell[nCell];

			if( nChromo >= 0 && nChromo < pcell->m_arChromos.GetSize() )
			{
				chromo_object *pchromo = pcell->m_arChromos[nChromo];
				
				if( pchromo )
				{
					IIdioBendsPtr sptrBends;
					CRect rcIdio;

					if( pchromo->m_bShowIdiogramm && pchromo->m_ptrIdio != 0 )
					{
						rcIdio = pchromo->m_rectIdio;

						if( m_bShowCellIdio )
							rcIdio.left = rcIdio.right - m_nIdioWidth;

						rcIdio.OffsetRect( pchromo->m_ptOffset );
						rcIdio = convert_from_object_to_wnd( rcIdio );

						if( rcIdio.PtInRect( pt ) )
							sptrBends = pchromo->m_ptrIdio;
					}

					if( m_bShowCellIdio && sptrBends == 0 )
					{
						rcIdio = pchromo->m_rectIdio;
						rcIdio.OffsetRect( pchromo->m_ptOffset );
						rcIdio = convert_from_object_to_wnd( rcIdio );

						if( rcIdio.PtInRect( pt ) )
						{
							long lClass = pcell->m_nClassNum;

							if( m_carioViewMode == cvmCellar )
							{
								ICalcObjectPtr	ptr_calc( pchromo->m_ptrMeasureObject );
								lClass = get_object_class( ptr_calc );
							}

							long	lpos = m_idiograms.find( lClass );
							if( lpos )
								sptrBends = m_idiograms.get( lpos );
						}
					}

					
					if( m_bShowEditableIdio && sptrBends == 0 )
					{
						rcIdio = pchromo->m_rectEditableIdio;
						rcIdio.OffsetRect( pchromo->m_ptOffset );
						rcIdio = convert_from_object_to_wnd( rcIdio );

						if( rcIdio.PtInRect( pt ) )
							sptrBends = pchromo->m_sptrEditableIdio;
					}

					if( sptrBends != 0 )
					{
						long lCount = 0;
						sptrBends->GetBengsCount( &lCount );

						IIdiogramPtr ptrIdio = sptrBends;

						bool bCentr = false;

						double _fPos = 0, _fPos2 = 0;
						sptrBends->GetBendRange( 0, &_fPos, &_fPos2 );

						for( long i = 0; i < lCount; i++ )
						{
							double fPos = 0, fPos2 = 0;
							sptrBends->GetBendRange( i, &fPos, &fPos2 );

							int flag = 0;
							sptrBends->GetBendFlag( i, &flag );

							if( flag == IDIB_CENTROMERE )
								bCentr = true;

							double y = ( pt.y - rcIdio.top ) / (double)rcIdio.Height();

							if( y >= fPos  && y <= fPos2 )
							{
								_bstr_t bstr;
								sptrBends->GetBendNumber( i, bstr.GetAddress() );
								
								if( !bCentr && _fPos == 0 || bCentr && _fPos2 == 1 )
									return CString( "p" ) + (char *)bstr;
								else
									return CString( "q" ) + (char *)bstr;
							}
						}
					}
				}
			}
		}
	}
	return "";
}

IIdiogramPtr CCarioImpl::_get_editable_idio( IUnknown* punkMeasure )
{
	IMeasureObjectPtr ptrMeasure( punkMeasure );
	if( ptrMeasure == 0 )	return 0;

	INamedDataObject2Ptr ptrNDO( ptrMeasure );
	if( ptrNDO == 0 )		return 0;


	long lPosCromo = 0;
	ptrNDO->GetFirstChildPosition( &lPosCromo );
	while( lPosCromo )
	{
		IUnknown* punkChromo = 0;
		ptrNDO->GetNextChild( &lPosCromo, &punkChromo );
		if( !punkChromo )
			continue;

		IIdiogramPtr ptr_idio( punkChromo );
		punkChromo->Release();	punkChromo = 0;

		if( ptr_idio != 0 )
		{
			unsigned uFlag = 0;
			ptr_idio->get_flags( &uFlag );

			if( uFlag == IDIT_EDITABLE )
				return ptr_idio;
		}
	}

	return 0;
}
