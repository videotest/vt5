#include "stdafx.h"
#include "sew_actions.h"
#include "nameconsts.h"
#include "misc_utils.h"
#include "misc_templ.h"
#include "data5.h"
#include "docview5.h"
#include "sew.h"

#include "stdio.h"
#include "math.h"

//tempo
#include "sew_page.h"


#define AjustColor( x )		color( max( 0, min( colorMax, x ) ) )

_ainfo_base::arg	CAutoSewHorzInfo::s_pargs[] = 
{
	{"Stitch Result",	szArgumentTypeImage, 0, false, true },
	{"Img0",	szArgumentTypeImage, 0, true, true },
	{"Img1",	szArgumentTypeImage, 0, true, true },
	{"Img2",	szArgumentTypeImage, 0, true, true },
	{"Img3",	szArgumentTypeImage, 0, true, true },
	{"Img4",	szArgumentTypeImage, 0, true, true },
	{"Img5",	szArgumentTypeImage, 0, true, true },
	{"Img6",	szArgumentTypeImage, 0, true, true },
	{"Img7",	szArgumentTypeImage, 0, true, true },
	{"Img8",	szArgumentTypeImage, 0, true, true },
	{"Img9",	szArgumentTypeImage, 0, true, true },
	{"Img10",	szArgumentTypeImage, 0, true, true },
	{"Img11",	szArgumentTypeImage, 0, true, true },
	{"Img12",	szArgumentTypeImage, 0, true, true },
	{"Img13",	szArgumentTypeImage, 0, true, true },
	{"Img14",	szArgumentTypeImage, 0, true, true },
	{"Img15",	szArgumentTypeImage, 0, true, true },
	{"Img16",	szArgumentTypeImage, 0, true, true },
	{"Img17",	szArgumentTypeImage, 0, true, true },
	{"Img18",	szArgumentTypeImage, 0, true, true },
	{"Img19",	szArgumentTypeImage, 0, true, true },
	{0, 0, 0, false, false },
};

_ainfo_base::arg	CAutoSewVertInfo::s_pargs[] = 
{
	{"Stitch Result",	szArgumentTypeImage, 0, false, true },
	{"Img0",	szArgumentTypeImage, 0, true, true },
	{"Img1",	szArgumentTypeImage, 0, true, true },
	{"Img2",	szArgumentTypeImage, 0, true, true },
	{"Img3",	szArgumentTypeImage, 0, true, true },
	{"Img4",	szArgumentTypeImage, 0, true, true },
	{"Img5",	szArgumentTypeImage, 0, true, true },
	{"Img6",	szArgumentTypeImage, 0, true, true },
	{"Img7",	szArgumentTypeImage, 0, true, true },
	{"Img8",	szArgumentTypeImage, 0, true, true },
	{"Img9",	szArgumentTypeImage, 0, true, true },
	{"Img10",	szArgumentTypeImage, 0, true, true },
	{"Img11",	szArgumentTypeImage, 0, true, true },
	{"Img12",	szArgumentTypeImage, 0, true, true },
	{"Img13",	szArgumentTypeImage, 0, true, true },
	{"Img14",	szArgumentTypeImage, 0, true, true },
	{"Img15",	szArgumentTypeImage, 0, true, true },
	{"Img16",	szArgumentTypeImage, 0, true, true },
	{"Img17",	szArgumentTypeImage, 0, true, true },
	{"Img18",	szArgumentTypeImage, 0, true, true },
	{"Img19",	szArgumentTypeImage, 0, true, true },
	{0, 0, 0, false, false },
};

int	nAutoScrollSize = 10;

CSewImage::CSewImage()
{
	m_x = m_y = m_cx = m_cy = 0;
	m_pbytes = 0;
}

CSewImage::~CSewImage()
{
	if( m_pbytes  )delete []m_pbytes;
	m_pbytes  = 0;
}

void CSewImage::SetImage( IImage3	*pimg, IColorConvertor *pcnv, int x, int y, int x1, int y1 )
{
	::dbg_assert( m_pbytes == 0 );

	m_x = x;
	m_y = y;
	m_cx = x1-x;
	m_cy = y1-y;
	m_pbytes = new byte[m_cx*m_cy];
	if( !m_pbytes )return;

	int	colors = ::GetImagePaneCount( pimg );
//	pcnv->GetColorPanesCount( &colors );

	color	**ppcolors = new color*[colors];

	int	i, c;

	byte	*pbyte = m_pbytes;

	for( i = y; i < y1; i++ )
	{
		for( c = 0; c < colors; c++ )
		{
			pimg->GetRow( i, c, ppcolors+c );
			ppcolors[c]+=x;
		}
		pcnv->ConvertImageToDIB( ppcolors, pbyte, m_cx, false );
		pbyte+=m_cx;
	}

	delete []ppcolors;


	Contrast();
	Median();
	Kirsch();
}

void CSewImage::Median()
{
}

void CSewImage::Kirsch()
{
}

void CSewImage::Contrast()
{
	byte	*p;

	long	hist[256], i;
	ZeroMemory( &hist, sizeof( hist ) );

	for( i = 0, p = m_pbytes; i < m_cx*m_cy; i++, p++ )
		hist[*p]++;

	int	nMin = 0;
	int	nMax = 255;


	int	nMinPercent = m_cx*m_cy/100;
	int	nMaxPercent = m_cx*m_cy/100;

	while( nMinPercent >= 0 )
	{
		nMinPercent-=hist[nMin];
		nMin++;
	}

	while( nMaxPercent >= 0 )
	{
		nMaxPercent-=hist[nMax];
		nMax--;
	}

	int	a = max(nMax-nMin, 1 );
	int	b = nMin;

	for( i = 0, p = m_pbytes; i < m_cx*m_cy; i++, p++ )
		*p = max( 0, min( 255, (*p-b)*256/a ) );
}


CAutoSewFilter::CAutoSewFilter()
{
	m_nMaxHOvrX = 60;
	m_nMinHOvrX = 20;
	m_nMaxHOvrY = 20;
	m_nMinHOvrY = -20;

	m_nMaxVOvrX = 20;
	m_nMinVOvrX = -20;
	m_nMaxVOvrY = 60;
	m_nMinVOvrY = 20;

	m_nSmoothEdges = 1;
	m_cx = m_cy = 0;
	m_lStep = 5;
	m_colors = 0;
	m_bHorz = true;
}

#define PROC_PIX( Name, dim ) { \
	double val = ::GetValueDouble( GetAppUnknown(), "\\Sew", #Name, m_n##Name ); \
	if( val > 1 ) \
		m_n##Name = (int)val; \
	else \
		m_n##Name= (int)(rcImg.##dim * val); }

bool CAutoSewFilter::InvokeFilter()
{
	CHourglass	wait;
	DWORD	dwTickCount = GetTickCount();
	IImage3Ptr	imageDest( GetDataResult() );

	//get sew params

	RECT rcImg;
	rcImg.right = rcImg.bottom = 10000000;
	

	int	nCount;

	for( nCount = 0; nCount < 20; nCount++ )
	{
		char	sz[255];
		wsprintf( sz, "Img%d", nCount );
		IUnknown	*punkImage = GetDataArgument( sz );
		if( !punkImage )break;
	}

	for( int i = 0; i < nCount; i++ )
	{
		char	sz[255];
		wsprintf( sz, "Img%d", i );
		IUnknown	*punkImage = GetDataArgument( sz );
		if( !punkImage )
			return false;

		{
			IImage3Ptr ptrSrc = punkImage;

			int w, h;
			ptrSrc->GetSize( &w, &h );

			if( w < rcImg.right)
				rcImg.right = w;

			if( h < rcImg.bottom )
				rcImg.bottom = h;
		}
	}




	PROC_PIX( MaxHOvrX, right  )
	PROC_PIX( MaxHOvrY, bottom )

	PROC_PIX( MinHOvrX, right  )
	PROC_PIX( MinHOvrY, bottom )
 
	PROC_PIX( MaxVOvrX, right  )
	PROC_PIX( MaxVOvrY, bottom )

	PROC_PIX( MinVOvrX, right  )
	PROC_PIX( MinVOvrY, bottom )


	m_lStep = ::GetValueInt( GetAppUnknown(), "\\Sew", "ScanQuality", m_lStep );
	m_nSmoothEdges = ::GetValueInt( GetAppUnknown(), "\\Sew", "SmoothEdges", m_nSmoothEdges );
	long	lReportTime = ::GetValueInt( GetAppUnknown(), "\\Sew", "TimeReport", 0 );
	

	ImageHolder	*pHolders = new ImageHolder[nCount];

	if( !pHolders )
		return false;

//get images and check its color system
	bool	bFirstImage = true;
	bool	bGoodImages = true;
	_bstr_t	bstrCC;


	for( i = 0; i < nCount; i++ )
	{
		char	sz[255];
		wsprintf( sz, "Img%d", i );
		IUnknown	*punkImage = GetDataArgument( sz );
		if( !punkImage )
		{
			::ReportError( IDS_WRONGCOLROWS );
			return false;
		}

		ImageHolder	*p = pHolders+i;
		p->image = punkImage;
		p->x = 0;
		p->y = 0;
		p->image->GetSize( &p->cx, &p->cy );


		if( p->cy == 0 || p->cy == 0 )
		{
			::ReportError( IDS_SEW_EMPTYIMAGE );
			bGoodImages = false;
		}


		IUnknown	*punkCC = 0;
		p->image->GetColorConvertor( &punkCC );
		if( !punkCC )
		{
			::ReportError( IDS_SEW_BADCC );
		}

		IColorConvertor2Ptr	ptrCC2( punkCC );

		BSTR	bstrCCName = 0;
		ptrCC2->GetCnvName( &bstrCCName );
		
		punkCC->Release();

		_bstr_t	bstrCCTest = bstrCCName;
		::SysFreeString( bstrCCName );

		if( bFirstImage )
		{
			bstrCC = bstrCCName;
			m_colors = ::GetImagePaneCount( p->image );
			bFirstImage = false;
		}
		else
		{
			if( bstrCC != bstrCCTest )
			{
				::ReportError( IDS_SEW_DIFFERENTCC ) ;
				
				bGoodImages = false;
				break;
			}
		}

		if( m_bHorz )
		{
			p->left.SetImage( p->image, ptrCC2, 0, 0, m_nMaxHOvrX, p->cy );
			p->right.SetImage( p->image, ptrCC2, p->cx-m_nMaxHOvrX, 0, p->cx, p->cy );
		}
		else
		{
			p->top.SetImage( p->image, ptrCC2, 0, 0, p->cx, m_nMaxVOvrY );
			p->bottom.SetImage( p->image, ptrCC2, 0, p->cy-m_nMaxVOvrY, p->cx, p->cy );
		}
	}

	if( !bGoodImages )
	{
		delete pHolders;
		return false;
	}


	int	nMinY, nMaxY, nMinX, nMaxX;
	bFirstImage = true;

	StartNotification( nCount );

	for( i = 0; i < nCount-1; i++ )
	{
		if( m_bHorz )
			_merge_h( pHolders+i, pHolders+i+1 );
		else
			_merge_v( pHolders+i, pHolders+i+1 );
	}

	for( i = 0; i < nCount; i++ )
	{
		if( bFirstImage )
		{
			if( m_bHorz )
			{
				nMinY = (pHolders+i)->y;
				nMaxY = (pHolders+i)->y+(pHolders+i)->cy;
				nMinX = 0;
				nMaxX = (pHolders+nCount-1)->x+(pHolders+nCount-1)->cx;
			}
			else
			{
				nMinY = 0;
				nMaxY = (pHolders+nCount-1)->y+(pHolders+nCount-1)->cy;
				nMinX = (pHolders+i)->x;
				nMaxX = (pHolders+i)->x+(pHolders+i)->cx;
			}

			bFirstImage = false;
		}

		if( m_bHorz )
		{
			nMinY = max( (pHolders+i)->y, nMinY );
			nMaxY = min( (pHolders+i)->y+(pHolders+i)->cy, nMaxY );
		}
		else
		{
			nMinX = max( (pHolders+i)->x, nMinX );
			nMaxX = min( (pHolders+i)->x+(pHolders+i)->cx, nMaxX );
		}
	}

	m_cx = nMaxX-nMinX;
	m_cy = nMaxY-nMinY;

	imageDest->CreateImage( m_cx, m_cy, bstrCC, m_colors );

	int	nOldHPos = 0, nOldVPos = 0;

	for( i = 0; i < nCount; i++ )
	{
		int	x, y, c;
		ImageHolder	*p = pHolders+i;

		if( m_bHorz )
		{
			if( !m_nSmoothEdges )
				nOldHPos = p->x;
			
			for( y = nMinY; y < nMaxY; y++ )
			for( c = 0; c < m_colors; c++ )
			{
				
				color	*psrc = 0;
				color	*pdest = 0;
				p->image->GetRow( y-p->y, c, &psrc );
				imageDest->GetRow( y-nMinY, c, &pdest );

				int	nCommonArea = nOldHPos - p->x;
				memcpy( pdest+nOldHPos, psrc+nCommonArea, sizeof( color )*(p->cx-nCommonArea) );

				pdest+=p->x;

				double	f1 = 0;
				double	f2 = 1;
				double	fdelta = 1./nCommonArea;

				for( x = 0; x < nCommonArea; x++, pdest++, psrc++, f1+=fdelta, f2-=fdelta )
					*pdest = AjustColor( *pdest*f2+*psrc*f1+.5 );
			}

			nOldHPos = p->x + p->cx;
		}
		else
		{
			if( !m_nSmoothEdges )
				nOldVPos = p->y;
			
			int	nCommonArea = nOldVPos-p->y;
			color	*psrc = 0;
			color	*pdest = 0;
			double	f1 = 0;
			double	f2 = 1;
			double	fdelta = 1./nCommonArea;

			for( y = 0; y < nCommonArea; y++, f1+=fdelta, f2-=fdelta )
			for( c = 0; c < m_colors; c++ )
			{
				p->image->GetRow( y, c, &psrc );
				imageDest->GetRow( y+p->y, c, &pdest );

				psrc+=nMinX-p->x;

				for( x = 0; x < m_cx; x++, psrc++, pdest++ )
					*pdest = AjustColor(*pdest*f2+*psrc*f1+.5);
			}

			for( ; y < p->cy; y++ )
			for( c = 0; c < m_colors; c++ )
			{
				p->image->GetRow( y, c, &psrc );
				imageDest->GetRow( y+p->y, c, &pdest );

				psrc+=nMinX-p->x;
				
				memcpy( pdest, psrc, m_cx*sizeof( color ) );
				
			}
			nOldVPos = p->y+p->cy;
		}
	}

	FinishNotification();

	delete []pHolders;

	if( lReportTime )
	{
		char	szTime[40];
		dwTickCount = GetTickCount()-dwTickCount;
		sprintf( szTime, "Takes %d ms", dwTickCount );
		MessageBox( 0, szTime, "SEW", MB_OK );
	}

	return true;
}

HRESULT CAutoSewFilter::GetActionState(DWORD *pdwState)
{
	*pdwState = 0; 
	if (!_Init())
		return S_OK;
	for(int nCount = 0; nCount < 20; nCount++ )
	{
		char	sz[255];
		wsprintf( sz, "Img%d", nCount );
		IUnknown	*punkImage = GetDataArgument( sz );
		if( !punkImage )break;
	}
	if (nCount >= 2)
		*pdwState = 1;
//	IUnknownPtr punk(::GetActiveObjectFromDocument(m_ptrTarget, szTypeImage), false);
//	if (punk != 0)
//		return CFilter::GetActionState(pdwState);
	return S_OK;
}


void CAutoSewFilter::_merge_h( CAutoSewFilter::ImageHolder	*p1,
								CAutoSewFilter::ImageHolder	*p2 )
{

	int	xOpt = 0;
	int	yOpt = 0;

	int	xTest, yTest, xMin, xMax, yMin, yMax;

	xMin = p1->cx-m_nMaxHOvrX;
	xMax = p1->cx-m_nMinHOvrX;
	yMin = m_nMinHOvrY;
	yMax = m_nMaxHOvrY;

	double	fMinRelDiff = 0;
	bool	bFirstTime = true;
	int		cyMin = min( p1->cy, p2->cy );

	int	nCheckStep = 4;

	while( nCheckStep != 0 )
	{
		for( yTest = yMin; yTest <= yMax; yTest+=nCheckStep )
		for( xTest = xMin; xTest <= xMax; xTest+=nCheckStep )
		{
			int	xs, xe, ys, ye;
			xs = xTest;
			xe = min( p1->cx, xTest+p2->cx );
			ys = max( 0, yTest );
			ye = min( cyMin, cyMin+yTest );

			int	x, y;
			int	nDiff = 0, t;
			int nCount = 0;
			byte	*pc1, *pc2;
			int	xs1 = max( -xTest, 0 );
			

			for( y = ys; y < ye; y+=m_lStep )
			{
				pc1 = p1->right.GetRow( xs, y );
				pc2 = p2->left.GetRow( xs1, y/*-*/-yTest );

				for( x = xs; x < xe; x+=m_lStep, pc1+=m_lStep, pc2+=m_lStep )
				{
					t = (int)*pc2-(int)*pc1;
					if( t < 0 )t = -t;

					nDiff+=t;
					nCount++;
				}
			}

			double	fRelDiff = (double)nDiff/((double)nCount);

			if( bFirstTime || fRelDiff < fMinRelDiff )
			{
				fMinRelDiff = fRelDiff;
				xOpt = xTest;
				yOpt = yTest;
				bFirstTime = false;
			}
		}

		xMin = xOpt;
		yMin = yOpt;
		xMax = min( xOpt+nCheckStep-1, xMax );
		yMax = min( yOpt+nCheckStep-1, yMax );

		if( nCheckStep == 4 )
			nCheckStep = 1;
		else
			nCheckStep = 0;
	}


	p2->x = xOpt;
	p2->y = yOpt;

	{
		char	szSewResult[30];
		sprintf( szSewResult, "Result X = %d, Y = %d\n", xOpt, yOpt );
		OutputDebugString( szSewResult );

	}

	p2->x += p1->x;
	p2->y += p1->y;
}

void CAutoSewFilter::_merge_v( CAutoSewFilter::ImageHolder	*p1,
								CAutoSewFilter::ImageHolder	*p2 )
{
	int	xOpt = 0;
	int	yOpt = 0;

	int	xTest, yTest, xMin, xMax, yMin, yMax;

	xMin = m_nMinVOvrX;
	xMax = m_nMaxVOvrX;
	yMin = p1->cy-m_nMaxVOvrY;
	yMax = p1->cy-m_nMinVOvrY;

	double	fMinRelDiff = 0;
	bool	bFirstTime = true;
	int		cxMin = min( p1->cx, p2->cx );

	int	nCheckStep = 4;

	while( nCheckStep != 0 )
	{
		for( yTest = yMin; yTest <= yMax; yTest+=nCheckStep )
		for( xTest = xMin; xTest <= xMax; xTest+=nCheckStep )
		{
			int	xs, xe, ys, ye;
			xs = max( 0, xTest );
			xe = min( cxMin, cxMin+xTest );
			ys = yTest;
			ye = min( p1->cy, yTest+p2->cy );

			int	x, y;
			int	nDiff = 0, t;
			int nCount = 0;
			byte	*pc1, *pc2;
			int	xs1 = max( -xTest, 0 );
			

			for( y = ys; y < ye; y+=m_lStep )
			{
				pc1 = p1->bottom.GetRow( xs, y );
				pc2 = p2->top.GetRow( xs1, y-yTest );

				for( x = xs; x < xe; x+=m_lStep, pc1+=m_lStep, pc2+=m_lStep )
				{
					t = (int)*pc2-(int)*pc1;
					if( t < 0 )t = -t;

					nDiff+=t;
					nCount++;
				}
			}

			double	fRelDiff = (double)nDiff/((double)nCount);

			if( bFirstTime || fRelDiff < fMinRelDiff )
			{
				fMinRelDiff = fRelDiff;
				xOpt = xTest;
				yOpt = yTest;
				bFirstTime = false;
			}
		}

		xMin = xOpt;
		yMin = yOpt;
		xMax = min( xOpt+nCheckStep-1, xMax );
		yMax = min( yOpt+nCheckStep-1, yMax );

		if( nCheckStep == 4 )
			nCheckStep = 1;
		else
			nCheckStep = 0;
	}


	p2->x = xOpt;
	p2->y = yOpt;

	{
		char	szSewResult[30];
		sprintf( szSewResult, "Result X = %d, Y = %d\n", xOpt, yOpt );
		OutputDebugString( szSewResult );

	}

	p2->x += p1->x;
	p2->y += p1->y;
}


/////////////////////////////////////////////////////////////////////////////////////////
//CDIBImage
CDIBImage::CDIBImage()
{
	m_point.x = 0;
	m_point.y = 0;
	m_pbi = 0;
	m_pbits = 0;
	m_pmask = 0;
	m_bVisible = false;
	m_size.cx = 0;
	m_size.cy = 0;
	m_mode = copy;
}

CDIBImage::~CDIBImage()
{
	delete m_pbi;
	delete m_pbits;
	delete m_pmask;
}

bool CDIBImage::IsInitialized()
{
	return m_pbi != 0 && m_pbits != 0;
}

void CDIBImage::DrawRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, RECT rectDest, _point ptBmpOfs, double fZoom, _point ptScroll )
{
	if( !GetVisible() )return;
	if( !m_pbi || !m_pbits )return;

	int		cxImg = m_pbi->biWidth, cyImg = m_pbi->biHeight, cx4Img = (cxImg*3+3)/4*4;

	_point	pointImageOffest;
	pointImageOffest.x = m_point.x;
	pointImageOffest.y = m_point.y;

	int		nRowSize = 0;
	byte	*psrc, *psrcRow;

	int		x, y, xImg, yImg, yBmp;
	byte	*p, *pmask;
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;
	int	xBmpStart = max( int(ceil(( pointImageOffest.x)*fZoom-ptScroll.x)), rectDest.left );				\
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*fZoom-ptScroll.x+.5)), rectDest.right );		\
	int	yBmpStart = max( int(ceil((pointImageOffest.y)*fZoom-ptScroll.y)), rectDest.top );					\
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*fZoom-ptScroll.y+.5)), rectDest.bottom );		\
	int	yImgOld = -1;																						\
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);									\
	int	*pxofs = new int[cx];																				\
	int	*pyofs = new int[cy];																				\
																											\
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )																\
		pxofs[t] = int((t+xBmpStart+ptScroll.x)/fZoom-pointImageOffest.x);									\
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )																\
		pyofs[t] = int((t+yBmpStart+ptScroll.y)/fZoom-pointImageOffest.y);									\
																											
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )
	{
		/*get the pointer to the BGR struct*/																
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;
		/*get the y coord of source image*/																	
		yImg =pyofs[y-yBmpStart];
		if( yImgOld != yImg )
		{
			psrcRow = m_pbits+yImg*cx4Img;
			pmask = m_pmask?m_pmask+yImg*cxImg:0;
			yImgOld = yImg;
		}

		if( m_mode == copy )
		{
			for( x = xBmpStart; x < xBmpEnd; x++ )
			{
				/*get the x coord of source image*/
				xImg = pxofs[x-xBmpStart];
				if( !pmask || pmask[xImg] )
				{
					psrc = psrcRow+xImg*3;

					*p = *psrc; p++;psrc++;
					*p = *psrc; p++;psrc++;
					*p = *psrc; p++;psrc++;
				}
				else
				{
					p++;
					p++;
					p++;
				}
			}
		}
		if( m_mode == add )
		{
			for( x = xBmpStart; x < xBmpEnd; x++ )
			{
				/*get the x coord of source image*/
				xImg = pxofs[x-xBmpStart];
				if( !pmask || pmask[xImg] )
				{
					psrc = psrcRow+xImg*3;

					*p = (*p+*psrc)>>1; p++;psrc++;
					*p = (*p+*psrc)>>1; p++;psrc++;
					*p = (*p+*psrc)>>1; p++;psrc++;
				}
				else
				{
					p++;
					p++;
					p++;
				}
			}
		}
		if( m_mode == sub )
		{
			for( x = xBmpStart; x < xBmpEnd; x++ )
			{
				/*get the x coord of source image*/
				xImg = pxofs[x-xBmpStart];
				if( !pmask || pmask[xImg] )
				{
					psrc = psrcRow+xImg*3;

					*p = max( 0, min( 255, (128+*p-*psrc) ) ); p++;psrc++;
					*p = max( 0, min( 255, (128+*p-*psrc) ) ); p++;psrc++;
					*p = max( 0, min( 255, (128+*p-*psrc) ) ); p++;psrc++;
				}
				else
				{
					p++;
					p++;
					p++;
				}
			}
		}
	}
	delete 	pxofs;
	delete 	pyofs;

}


bool CDIBImage::AttachImage( IUnknown *punkImage )
{
	dbg_assert( !IsInitialized() );

	m_image = punkImage;

	if( m_image == 0 )
	{
		ReportError( IDS_SEW_NOIMAGE );
		return false;
	}

	if( m_image != 0 )
	{
		m_pbi = new BITMAPINFOHEADER;
		if( !m_pbi )
		{
			ReportError( IDS_SEW_CANTALLOCATE );
			return false;
		}
		ZeroMemory( m_pbi, sizeof( *m_pbi )  );
		m_pbi->biSize = sizeof( *m_pbi );
		m_pbi->biBitCount = 24;
		m_pbi->biPlanes = 1;
		int	cx, cy;
		m_image->GetSize( &cx, &cy );

		m_pbi->biWidth = cx;
		m_pbi->biHeight = cy;

		int	cx4 = (cx*3+3)/4*4;
		m_pbits = new byte[cx4*cy];
		if( !m_pbits )
		{
			ReportError( IDS_SEW_CANTALLOCATE );
			delete m_pbi;m_pbi = 0;
			return false;
		}

		m_pmask = new byte[cx*cy];
		if( !m_pmask )
		{
			ReportError( IDS_SEW_CANTALLOCATE );
			delete m_pbi;m_pbi = 0;
			delete m_pbits;m_pbits = 0;
			return false;
		}

		int	colors;
		IUnknown	*punkCC = 0;
		m_image->GetColorConvertor( &punkCC );
		if( !punkCC )
		{
			ReportError( IDS_NO_COLORCONVERTOR );
			return false;
		}
		IColorConvertorPtr	ptrCC( punkCC );
		punkCC->Release();

		colors = ::GetImagePaneCount( m_image );

		color	**ppcolors = new color*[colors];
		byte	*pmask;
		byte	*pdib = m_pbits;

		int	y, c;
		for( y = 0; y < cy; y++ )
		{
			for( c = 0; c < colors; c++ )
				m_image->GetRow( y, c, &ppcolors[c] );
			m_image->GetRowMask( y, &pmask );
			ptrCC->ConvertImageToDIB( ppcolors, pdib, cx, true );
			memcpy( m_pmask+y*cx, pmask, cx );
			pdib+=cx4;
		}

		delete ppcolors;
	}
	m_size.cx = m_pbi->biWidth;
	m_size.cy = m_pbi->biHeight;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//CSewManual
CSewManual::CSewManual()
{
	m_lCurrentPos = 0;	
	m_rectActiveImage.set( 0, 0, 300, 200 );
	m_xOfs1 = m_xOfs2 = m_yOfs1 = m_yOfs2 = 0;
	m_bTrackFirstTime = true;
	m_pointOffset.x = 0;
	m_pointOffset.y = 0;
	m_bFullZoom = false;
	m_colors = 0;
}

CSewManual::~CSewManual()
{
	DeInit();
}

IUnknown	*CSewManual::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	else if( iid == IID_IInteractiveSewAction )return (IInteractiveSewAction*)this;
	else return CInteractiveAction::DoGetInterface( iid );
}

void CSewManual::DeInit()
{
	m_lCurrentPos = 0;
	TPOS	lpos = m_images.head();
	while( lpos )delete m_images.next( lpos );
	
	m_images.deinit();
}

bool CSewManual::Initialize()
{
	if( !CInteractiveAction::Initialize() )
		return false;


	//add property page
	int	nPage = -1;
	IPropertySheet	*pSheet = ::FindPropertySheet();
	IOptionsPage	*pPage = ::FindPage( pSheet, clsidSewPage, &nPage );
	m_ptrPage = pPage;
	if( nPage != -1 )pSheet->IncludePage( nPage );
	if( pPage )pPage->Release();
	if( pSheet )pSheet->Release();
	

	m_ptrSite->GetZoom( &m_fOriginalZoom );

	DeInit();
	//init images array
	IDataContext2Ptr	ptrContext( m_ptrTarget );


	_bstr_t	bstrImage = szTypeImage;
	LONG_PTR	lpos = 0;
	IUnknown	*punkImage = 0;
	bool	bFirst = true;

	long	lCount = 0;

	ptrContext->GetSelectedCount( bstrImage, &lCount );

	if( lCount < 2 )
	{
		ReportError( IDS_2MOREIMAGES );
		return false;
	}
	ptrContext->GetFirstSelectedPos( bstrImage, &lpos );

	while( lpos )
	{
		ptrContext->GetNextSelected( bstrImage, &lpos, &punkImage );

		IImage2Ptr	ptrImage( punkImage );
		punkImage->Release();

		IUnknown	*punkCC = 0;
		ptrImage->GetColorConvertor( &punkCC );

		if( !punkCC )
		{
			ReportError( IDS_SEW_BADCC );
			return false;
		}

		IColorConvertor2Ptr	ptrCC( punkCC );
		punkCC->Release();

		BSTR	bstrCCName = 0;
		ptrCC->GetCnvName( &bstrCCName );
		_bstr_t	bstrCC( bstrCCName );
		::SysFreeString( bstrCCName );

		if( bFirst )
		{
			m_colors = ::GetImagePaneCount( ptrImage );

			m_bstrCCName = bstrCC;
		}
		else
		{
			if( m_bstrCCName != bstrCC )
			{
				ReportError( IDS_SEW_DIFFERENTCC );
				return false;
			}
		}

		bFirst = false;


		CDIBImage	*pimg = new CDIBImage;
		if( pimg->AttachImage( punkImage ) )
			m_images.insert( pimg );
		else
			delete pimg;
	}

	IViewPtr	ptrView( m_ptrTarget );
	IUnknown	*punkFrame = 0;
	ptrView->GetMultiFrame( &punkFrame, false );
	IMultiSelectionPtr	ptrMS( punkFrame );
	if( punkFrame )punkFrame->Release();
	if( ptrMS )ptrMS->EmptyFrame();



	//get the old scroll size
	m_ptrSite->GetClientSize( &m_sizeScrollOld );

	m_lCurrentPos = m_images.head();
	NextImage();

	InvalidateRect( m_hwnd, 0, 0 );

	SetFocus( m_hwnd );

	return true;
}

HRESULT CSewManual::TerminateInteractive()
{
	if( m_state == Active )
		m_ptrSite->SetClientSize( m_sizeScrollOld );

	return CInteractiveAction::TerminateInteractive();
}


bool CSewManual::Finalize()
{
//	m_ptrSite->SetClientSize( m_sizeScrollOld );

	if( m_bFullZoom )
		SwitchZoom( NOPOINT, false );

	if( !CInteractiveAction::Finalize() )
		return false;

	//repove property page

	int	nPage = -1;
	IPropertySheet	*pSheet = ::FindPropertySheet();
	IOptionsPage	*pPage = ::FindPage( pSheet, clsidSewPage, &nPage );
	if( nPage != -1 )pSheet->ExcludePage( nPage );
	if( pPage )pPage->Release();
	if( pSheet )pSheet->Release();
	m_ptrPage = 0;

	InvalidateRect( m_hwnd, 0, 0 );

	return true;
}

HRESULT CSewManual::Paint( HDC hDC, RECT rectDraw, 
						  IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, 
						  BYTE *pdibCache )
{
	/*::SelectObject( hDC, ::GetStockObject( BLACK_PEN ) );
	::SelectObject( hDC, ::GetStockObject( BLACK_BRUSH ) );
	::Rectangle( hDC, RECT_COORDS_ARG( rectDraw ) );*/

	//get zoom and scroll
	_point	pointScroll;
	double	fZoom;
	IScrollZoomSitePtr	ptrScrollZoom( punkTarget );
	ptrScrollZoom->GetZoom( &fZoom );
	ptrScrollZoom->GetScrollPos( &pointScroll );

	TPOS	lpos = m_images.head();

	while( lpos )
	{
		CDIBImage	*pimage = m_images.next( lpos );

		_point	pointBmpOffset;
		pointBmpOffset.x = rectDraw.left;
		pointBmpOffset.y = rectDraw.top;

		pimage->DrawRect( pbiCache, pdibCache, rectDraw, pointBmpOffset, fZoom, pointScroll );
	}

	::SelectObject( hDC, ::GetStockObject( NULL_BRUSH ) );
	::SelectObject( hDC, ::GetStockObject( WHITE_PEN ) );

	_rect	rect1 = ::_window( m_rectActiveImage, m_ptrSite );
	::Rectangle( hDC, RECT_COORDS_ARG( rect1 ) );

	return S_OK;
}

bool CSewManual::DoLButtonDown( _point point )
{
	StartTracking( point );
	return true;
}

bool CSewManual::DoLButtonDblClick( _point point )
{
	NextImage();
	return true;
}

bool CSewManual::DoStartTracking( _point point )
{
	m_pointOffset = m_rectActiveImage.top_left().delta( point );

	_rect	rectClient;
	GetClientRect( m_hwnd, &rectClient );
	_rect	rectActive = _window( m_rectActiveImage, m_ptrSite );
	//rectActive.inflate( nAutoScrollSize );

	m_xOfs1 = max( rectClient.left - rectActive.left, 0 );
	m_xOfs2 = max( rectActive.right - rectClient.right, 0 );
	m_yOfs1 = max( rectClient.top - rectActive.top, 0 );
	m_yOfs2 = max( rectActive.bottom - rectClient.bottom, 0 );

	/*if( rectClient.width() > rectActive.width() )
		m_xOfs1 = m_xOfs2 = 0;
	if( rectClient.height() > rectActive.height() )
		m_yOfs1 = m_yOfs2 = 0;*/

	m_bTrackFirstTime = true;

	return true;
}

bool CSewManual::DoFinishTracking( _point point )
{
	m_pointOffset = NOPOINT;
	return true;
}
bool CSewManual::DoTrack( _point point )
{
	if( m_bLeftButton )
	{
		MoveTo( point );
	}
	else
	{
	}
	return true;
}

CDIBImage	*CSewManual::GetWorkImage()
{
	if( !m_lCurrentPos )
		m_lCurrentPos = m_images.head();

	if( m_lCurrentPos )return m_images.get( m_lCurrentPos );

	return 0;
}


LRESULT	CSewManual::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	long	lResult = CInteractiveAction::DoMessage( nMsg, wParam, lParam );
	if( lResult )
		return lResult;

	if( nMsg == WM_PAINT )
	{
		HWND	hwnd;
		{
			IWindowPtr	ptrWindow( m_ptrTarget );
			ptrWindow->GetHandle( (HANDLE*)&hwnd );
		}



		PAINTSTRUCT	paint;
		HDC	hdcPaint = ::BeginPaint( hwnd, &paint );

		int	cx = paint.rcPaint.right - paint.rcPaint.left;
		int	cy = paint.rcPaint.bottom - paint.rcPaint.top;

		if( cx <= 0 || cy <= 0 )
		{
			::EndPaint( hwnd, &paint );
			return 1;
		}

		HDC	hdc = ::CreateCompatibleDC( hdcPaint );

		int	cx4 = (cx*3+3)/4*4;
		BITMAPINFOHEADER	bi;
		ZeroMemory( &bi, sizeof( bi ) );
		bi.biSize = sizeof( bi );
		bi.biWidth = cx;
		bi.biHeight = cy;
		bi.biPlanes = 1;
		bi.biBitCount = 24;

		byte	*pbits = 0;

		HBITMAP	hBitmap = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, 0, (void**)&pbits, 0, 0 );
		::SelectObject( hdc, hBitmap );
		::SetWindowOrgEx( hdc, paint.rcPaint.left, paint.rcPaint.top, 0 );

		if( !hBitmap )
		{
			::DeleteDC( hdc );
			::EndPaint( hwnd, &paint );
			return 1;
		}

		Paint( hdc, paint.rcPaint, m_ptrTarget, &bi, pbits );
		
		::SetDIBitsToDevice( hdcPaint, paint.rcPaint.left, paint.rcPaint.top, cx, cy, 0, 0, 0, cy, pbits, (BITMAPINFO*)&bi, DIB_RGB_COLORS );
		::DeleteDC( hdc );
		::DeleteObject( hBitmap );
		
		::EndPaint( hwnd, &paint );
		
		return 1;
	}

	if( nMsg == WM_ERASEBKGND )
		return 1;

	return 0;
}

void CSewManual::EnsureVisible( _point point )
{
	_point	pointDelta( 0, 0 );

	if( !m_bTrackFirstTime )
	{
		_rect	rectClient;
		GetClientRect( m_hwnd, &rectClient );
		_rect	rectActive = _window( m_rectActiveImage, m_ptrSite );

		

		if( m_pointOldMove.x > point.x )
			if( rectClient.left - rectActive.left - m_xOfs1> 0 )
				pointDelta.x = rectClient.left - rectActive.left - m_xOfs1;

		if( m_pointOldMove.x < point.x )
			if( rectClient.right - rectActive.right + m_xOfs2 < 0 )
				pointDelta.x = rectClient.right - rectActive.right + m_xOfs2;

		if( m_pointOldMove.y > point.y )
			if( rectClient.top - rectActive.top  - m_yOfs1 > 0 )
				pointDelta.y = rectClient.top - rectActive.top - m_yOfs1;

		if( m_pointOldMove.y < point.y )
			if( rectClient.bottom - rectActive.bottom + m_yOfs2 < 0 )
				pointDelta.y = rectClient.bottom - rectActive.bottom + m_yOfs2;
	}

	m_bTrackFirstTime = false;
	m_pointOldMove = point;

	if( pointDelta == _point( 0, 0 ) )
		return;

	_point	pointScroll, pointCursor;
	m_ptrSite->GetScrollPos( &pointScroll );
	::GetCursorPos( &pointCursor );
	pointScroll = pointScroll.delta( pointDelta );
	pointCursor = pointCursor.offset( pointDelta );
	//m_ptrSite->SetScrollPos( pointScroll );
	HWND	hwndHorzScroll, hwndVertScroll;
	m_ptrSite->GetScrollBarCtrl( SB_HORZ, (HANDLE*)&hwndHorzScroll );
	m_ptrSite->GetScrollBarCtrl( SB_VERT, (HANDLE*)&hwndVertScroll );

	m_ptrSite->LockScroll( true );
	m_ptrSite->SetScrollPos( pointScroll );
	m_ptrSite->LockScroll( false );


	::SetCursorPos( pointCursor.x, pointCursor.y );
}

void CSewManual::MoveTo( _point point )
{
	::InvalidateRect( m_hwnd, &_window( m_rectActiveImage, m_ptrSite ), false );
	m_rectActiveImage.set_offset	( point.offset( m_pointOffset ) );
	CDIBImage	*pimg = GetWorkImage();
	if( pimg )pimg->SetOffset( m_rectActiveImage.top_left() );
	::InvalidateRect( m_hwnd, &_window( m_rectActiveImage, m_ptrSite ), false );
}

void CSewManual::SwitchZoom( _point point, bool bForce )
{
	double	fZoom;

	m_ptrSite->GetZoom( &fZoom );
	_size	sizeClient;
	m_ptrSite->GetClientSize( &sizeClient );
	_rect	rectClient;
	::GetClientRect( m_hwnd, &rectClient );

	int nFullZoom = min( 256*rectClient.width()/sizeClient.cx, 256*rectClient.height()/sizeClient.cy );
	int	nCurrentZoom = int( fZoom*256 );

	m_bFullZoom = nFullZoom == nCurrentZoom;

	if( !m_bFullZoom || bForce )
	{
		m_ptrSite->SetZoom( nFullZoom/256. );
		m_bFullZoom = true;
	}
	else
	{
		m_ptrSite->SetZoom( m_fOriginalZoom );
		_point	pointScroll = _window( point, m_ptrSite );
		pointScroll-=rectClient.center();
		m_ptrSite->SetScrollPos( pointScroll );
		m_bFullZoom = false;
	}
}

bool CSewManual::DoRButtonUp( _point  point )
{
	SwitchZoom( point, false );
	return true;
}

HRESULT CSewManual::GetActionState( DWORD *pdwState )
{
	IDataContext2Ptr	ptrContext( m_ptrTarget );
	if (ptrContext == 0)
		*pdwState = 0;
	else
	{
	_bstr_t	bstrImage = szTypeImage;
	long	count;
	ptrContext->GetSelectedCount( bstrImage, &count );
	*pdwState = count > 1;
	}
	return S_OK;
}

HRESULT CSewManual::DoInvoke()
{
	CHourglass	wait;

	_rect	rectAll;
	bool	bInit = false;

	if( m_bFullZoom )
	{
		IScrollZoomSitePtr	ptrSite( m_ptrTarget );
		ptrSite->SetZoom( m_fOriginalZoom );
		ptrSite->SetScrollPos( _point( 0, 0 ) );
	}

	TPOS	lpos = m_images.head();
	while( lpos )
	{
		CDIBImage	*pimg = m_images.next( lpos );
		if( !bInit )
			rectAll = pimg->GetRect();
		else
			rectAll.merge( pimg->GetRect() );
		bInit = true;
	}

	int	cx = rectAll.width();
	int	cy = rectAll.height();

	_bstr_t	bstrType = szTypeImage;
	IUnknown	*punkImage = ::CreateTypedObject( bstrType );
	IImage2Ptr	ptrImage( punkImage );
	punkImage->Release();
	ptrImage->CreateImage( cx, cy, m_bstrCCName, m_colors  );
	ptrImage->InitRowMasks();



	int	x, y, c;
	byte	*p;
	color	*ps, *pd;
	byte	*pmasks, *pmaskd;
	color	colors[10];


	::GetColors( ptrImage, colors, GetValueColor( GetAppUnknown(), "\\Editor", "Back", RGB( 0, 0, 0 ) ) );

	for( y = 0; y < cy; y++ )
	{
		ptrImage->GetRowMask( y, &p );
		for( c = 0; c < m_colors; c++ )
		{
			ptrImage->GetRow( y, c, &pd );
			for( x = 0; x < cx; x++, pd++ )
				*pd = colors[c];
		}
		memset( p, 0, cx );
	}

	lpos = m_images.head();
	while( lpos ) 
	{
		CDIBImage	*pimage = m_images.next( lpos );

		_rect	rectImage = pimage->GetRect();

		for( c = 0; c < m_colors; c++ )
		for( y = rectImage.top; y < rectImage.bottom; y++ )
		{
			ptrImage->GetRow( y-rectAll.top, c, &pd );	pd+=rectImage.left-rectAll.left;
			ptrImage->GetRowMask( y-rectAll.top, &pmaskd );	pmaskd+=rectImage.left-rectAll.left;
			pimage->GetImage()->GetRow( y - rectImage.top, c, &ps );	
			pimage->GetImage()->GetRowMask( y - rectImage.top, &pmasks );	

			for( x = rectImage.left; x < rectImage.right; x++, ps++, pd++, pmasks++, pmaskd++ )
			{
				if( *pmasks )
				{
					if( *pmaskd )
						*pd = (*pd+*ps)>>1;
					else
						*pd = *ps;
					*pmaskd = 0xff;
				}
				*pd = *ps;
			}
		}
	}

	for( y = 0; y < cy; y++ )
	{
		ptrImage->GetRowMask( y, &p );
		memset( p, 0xFF	, cx );
	}

	//set to doc data
	IUnknown	*punkDoc= 0;
	IViewPtr	ptrView( m_ptrTarget );
	ptrView->GetDocument( &punkDoc );
	::dbg_assert( punkDoc != 0 );
	m_guidTarget = ::GetKey( punkDoc );
	punkDoc->Release();

	SetToDocData( punkDoc, ptrImage );


	IDataContext2Ptr	ptrContext( ptrView );
	ptrContext->SetActiveObject( 0, ptrImage, 0 );
	LeaveMode();

	m_ptrTarget = 0;

	return S_OK;
}

bool CSewManual::DoChar( int nChar, bool bKeyDown )
{
	if( CInteractiveAction::DoChar( nChar, bKeyDown ) )
		return true;

	if( nChar == '-' || nChar == '=' || nChar == '+' )
		SetMode( nChar );

	if( bKeyDown && nChar == VK_RETURN )
		NextImage();
	if( !bKeyDown && nChar == VK_TAB )
		SwitchZoom( NOPOINT, false );

	if( nChar == VK_UP || nChar == VK_DOWN|| nChar == VK_LEFT || nChar == VK_RIGHT )
	{
		_point	point = m_rectActiveImage.top_left();
		_size	size;
		m_ptrSite->GetClientSize( &size );

		int	nDelta = 1;
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			nDelta = 20;

		if( nChar == VK_UP )point.y -= nDelta;
		if( nChar == VK_DOWN )point.y += nDelta;
		if( nChar == VK_LEFT )point.x -= nDelta;
		if( nChar == VK_RIGHT )point.x += nDelta;

		point.x = max( 0, min( size.cx, point.x ) );
		point.y = max( 0, min( size.cy, point.y ) );

		MoveTo( point );
	}

	return false;
}

HRESULT CSewManual::SetMode( UINT nChar )
{
	CDIBImage	*pimage = GetWorkImage();
	if( !pimage )return S_FALSE;

	if( nChar == '-' )
		pimage->SetDrawMode( CDIBImage::sub );

	if( nChar == '+' )
		pimage->SetDrawMode( CDIBImage::add );

	if( nChar == '=' )
		pimage->SetDrawMode( CDIBImage::copy );

	InvalidateRect( m_hwnd, 0, 0 );		
	if( m_ptrPage != 0 )m_ptrPage->LoadSettings();

	return S_OK;
}
HRESULT CSewManual::GetMode( UINT *pnChar )
{
	CDIBImage	*pimage = GetWorkImage();
	if( !pimage )return S_FALSE;

	*pnChar = pimage->GetDrawMode();
	return S_OK;
}

HRESULT CSewManual::NextImage()
{
	if( !m_lCurrentPos )return S_FALSE;

	_point	pointOldScroll;
	m_ptrSite->GetScrollPos( &pointOldScroll );
	pointOldScroll = _client( pointOldScroll, m_ptrSite );



	CDIBImage *pbi = m_images.next( m_lCurrentPos );
	pbi->SetVisible( true );
	pbi->SetOffset( m_rectActiveImage.top_left() );
	pbi->SetDrawMode( CDIBImage::copy );
	if( !m_lCurrentPos )
	{
		Finalize();
		return S_OK;
	}

	bool	bFirst = true;
	_rect	rectAll;
	//CDIBImage	*pdib = 
	TPOS	lpos = m_images.head();
	while( lpos )
	{
		CDIBImage	*pimage = m_images.next( lpos );

		_rect	rect = pimage->GetRect();
		if( bFirst )
			rectAll = rect;
		else
			rectAll.merge( rect );
		bFirst = false;
	}

	m_rectActiveImage = GetWorkImage()->GetRect();	

	rectAll.right += m_rectActiveImage.width()*2;
	rectAll.bottom += m_rectActiveImage.height()*2;

	_point	pointOffset = rectAll.top_left();
	_point	pointNewOffset = _point( m_rectActiveImage.width(), m_rectActiveImage.height() );

	lpos = m_images.head();

	while( lpos )
	{
		CDIBImage	*pimage = m_images.next( lpos );
		_point	pointImage = pimage->GetOffset();

		pointImage = pointImage.delta( pointOffset ).offset( pointNewOffset );

		pimage->SetOffset( pointImage );
	}

	rectAll.set_offset( 0, 0 );

	pointOldScroll = pointOldScroll.delta( pointOffset ).offset( pointNewOffset );
	pointOldScroll = _window( pointOldScroll, m_ptrSite );
	m_ptrSite->SetClientSize( rectAll.size() );
	m_ptrSite->SetScrollPos( pointOldScroll );

	CDIBImage	*pimg = GetWorkImage();
	if( pimg )
	{
		INIT_ZOOM_SCROLL( m_ptrSite );

		pimg->SetVisible( true );
		m_pointOffset.x = 0;
		m_pointOffset.y = 0;

		MoveTo( _point( long(pointScroll.x/fZoom+.5), long(pointScroll.y/fZoom+.5)	) );
	}

	SwitchZoom( NOPOINT, true );
	InvalidateRect( m_hwnd, 0, 0 );		

	if( m_ptrPage != 0 )m_ptrPage->LoadSettings();

	return S_OK;
}


CAutoSewHorzInfo::CAutoSewHorzInfo()
{
	OutputDebugString( "qq\n" );
}