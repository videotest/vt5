#include "stdafx.h"
#include "complexcolor.h"
#include "math.h"
#include "resource.h"
#include "ccmacro.h"

IMPLEMENT_DYNCREATE(CColorConvertorComplex, CCmdTargetEx);

// {97BC5FE2-430A-4a51-9982-7D4BAC97DF2E}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorComplex, "fft.ColorConvertorComplex",
0x97bc5fe2, 0x430a, 0x4a51, 0x99, 0x82, 0x7d, 0x4b, 0xac, 0x97, 0xdf, 0x2e);


/////////////////////////////////////////////////////////////////
//color convertor GRAY
/////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CColorConvertorComplex, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorComplex, IID_IColorConvertorEx, CnvEx)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorComplex, CnvEx)


HRESULT CColorConvertorComplex::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorComplex, CnvEx);

	CONVERTTODIB_PART1

	int	nRe, nIm;
	byte	byteVal;

	nRe = ColorAsByte(R);
	nIm = ColorAsByte(G);
	byteVal = pThis->bytesConvertTable[(nRe<<8)|nIm];
		
	Rbyte = byteVal;	
	Gbyte = byteVal;
	Bbyte = byteVal;
	

	CONVERTTODIB_PART2
	//_catch_nested
	return S_OK;

/*	{
		if( pbi->biBitCount != 24 )
			return S_FALSE;

		//CTimeTest	test( true, "Converting.." );

		CImageWrp	image( punkImage );

		CPoint	point = image.GetOffset();

		int	cxDIB = pbi->biWidth;
		int	cyDIB = pbi->biHeight;
		int	cx4DIB = (cxDIB*3+3)/4*4;

		int	cx = image.GetWidth();
		int	cy = image.GetHeight();

		int	xStart = rectDest.left;
		int	yStart = rectDest.top;
		int	xEnd = rectDest.right;
		int	yEnd = rectDest.bottom;

		xStart = max( 0, min( pbi->biWidth, xStart ) );
		xEnd = max( 0, min( pbi->biWidth, xEnd ) );
		yStart = max( 0, min( pbi->biHeight, yStart ) );
		yEnd = max( 0, min( pbi->biHeight, yEnd ) );

		int x, y, xsrc, ysrc;

		byte	*pdibDest = pdibBits;
		color	**ppcolorsRe=new color*[cy];
		color	**ppcolorsIm=new color*[cy];
		byte	**ppmasks=new byte*[cy];

		ZeroMemory( ppcolorsRe, sizeof( color*)*cy );
		ZeroMemory( ppcolorsIm, sizeof( color*)*cy );
		ZeroMemory( ppmasks, sizeof( byte*)*cy );

		int		cxDest = rectDest.right-rectDest.left;
		int		cyDest = rectDest.bottom-rectDest.top;
		int		cxSrc = rect.right-rect.left;
		int		cySrc = rect.bottom-rect.top;


		byte	*pmask = 0;
		color	*pim;
		color	*pre;

		int	nImageColorsOffest = rect.left-point.x-rectDest.left*cxSrc/cxDest;
		int	nImageOffestCY = rect.top-point.y-rectDest.top*cySrc/cyDest;
		int	nInitialFlag = (cxDest>=cxSrc)?cxDest+(rectDest.left%(cxDest/cxSrc))*cxSrc:0;

		CRect	rectImage = image.GetRect();
		int	nRe, nIm;
		byte	byteVal;
		

		for( y = 0; y < cy; y++ )
		{
			ppmasks[y] = image.GetRowMask( y )+nImageColorsOffest;
			ppcolorsRe[y] = image.GetRow( y, 0 )+nImageColorsOffest;
			ppcolorsIm[y] = image.GetRow( y, 1 )+nImageColorsOffest;
		}
		

		for( y = yStart; y < yEnd; y++ )
		{
			ysrc = (y-rectDest.top)*cySrc/cyDest+rect.top-point.y;

			pmask = ppmasks[ysrc];
			pre = ppcolorsRe[ysrc];
			pim = ppcolorsIm[ysrc];

			byte	*pdib = pdibDest+cx4DIB*(cyDIB-y-1)+xStart*3;

			if( cxDest >= cxSrc )
			{
				long	lFlag = nInitialFlag;
				pmask += xStart*cxSrc/cxDest;
				pre += xStart*cxSrc/cxDest;
				pim += xStart*cxSrc/cxDest;

				for( x = xStart; x < xEnd; x++ )
				{										
					lFlag	-= cxSrc;
					if( lFlag < 0 )
					{
						lFlag+=cxDest;
						pre++;
						pim++;
						pmask++;
					}

					//xsrc = x*cxSrc/cxDest;
					if( !*pmask )
					{
						pdib++;
						pdib++;
						pdib++;
						continue;
					}


					nRe = ColorAsByte( *pre);
					nIm = ColorAsByte( *pim );
					byteVal = pThis->bytesConvertTable[(nRe<<8)|nIm];
					
					*pdib = byteVal;
					pdib++;
					*pdib = byteVal;
					pdib++;
					*pdib = byteVal;
					pdib++;
				}
			}
			else
			{
				for( x = xStart; x < xEnd; x++ )
				{
					xsrc = x*cxSrc/cxDest;

					if( !pmask[xsrc] )
					{
						pdib++;
						pdib++;
						pdib++;
						continue;
					}
					
					nRe = ColorAsByte( *pre);
					nIm = ColorAsByte( *pim );
					byteVal = pThis->bytesConvertTable[nRe<<8|nIm];
					
					*pdib = byteVal;
					pdib++;
					*pdib = byteVal;
					pdib++;
					*pdib = byteVal;
					pdib++;
				}
			}
		}

		delete ppcolorsRe;
		delete ppcolorsIm;
		delete ppmasks;


		return S_OK;
	}
	_catch_nested;

*/
}


CColorConvertorComplex::CColorConvertorComplex()
{
	_OleLockApp( this );

	int	x, y;

	for( y = 0; y < 256; y++ )
		for( x = 0; x < 256; x++ )
		{
			double	fx = x-128;
			double	fy = y-128;

			double	fa = sqrt( fx*fx+fy*fy )/sqrt( (double)128*128+128*128 );	//from 0 to 1
			double	fResult = sqrt( fa )*256;
			bytesConvertTable[(x<<8)|y] = (byte)fResult;
		}

	for( x = 0; x < 256; x++ )
		colorConvertTableBack[x] = (x-128)*(x-128)*4;
}

CColorConvertorComplex::~CColorConvertorComplex()
{
	_OleUnlockApp( this );
}


const char *CColorConvertorComplex::GetCnvName()
{
	return _T("Complex");
}

int CColorConvertorComplex::GetColorPanesCount()
{
	return 2;
}

const char *CColorConvertorComplex::GetPaneName(int numPane)
{
	if (numPane == 0)
		return _T("Real");
	else if (numPane == 1)
		return _T("Image");


	return _T("");
}

const char *CColorConvertorComplex::GetPaneShortName( int numPane )
{
	if (numPane == 0)
		return _T("R");
	else if (numPane == 1)
		return _T("I");

	return _T("");
}

color CColorConvertorComplex::ConvertToHumanValue( color colorInternal, int numPane )
{
	return colorInternal * 255 / colorMax;
}


DWORD CColorConvertorComplex::GetPaneFlags(int numPane)
{
	DWORD dwFlag = pfOrdinary;
	return dwFlag;
}

void CColorConvertorComplex::ConvertRGBToImage( byte *pRGB, color **ppColor, int cx )
{
	if (ppColor == NULL || pRGB == NULL)
		return;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		byte	byte1 = (byte)Bright(pRGB[c++], pRGB[c++], pRGB[c++]);
		ppColor[0][n] = colorMax/2;//colorConvertTableBack[byte1];//ByteAsColor();
		ppColor[1][n] = colorMax/2;//colorConvertTableBack[byte1];

	}
}

void CColorConvertorComplex::ConvertImageToRGB( color **ppColor, byte *pRGB, int cx )
{
	if (ppColor == NULL || pRGB == NULL)
		return;
	byte	byteVal;
	int	x, y;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		x = ColorAsByte( ppColor[0][n] );
		y = ColorAsByte( ppColor[1][n] );

		byteVal = bytesConvertTable[(x<<8)|y];
		pRGB[c++] = byteVal;
		pRGB[c++] = byteVal;
		pRGB[c++] = byteVal;
	}
}

void CColorConvertorComplex::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
	if (ppColor == NULL || pGray == NULL)
		return;

	for( long n = 0; n < cx; n++ )
	{
		ppColor[0][n] = colorMax/2;//colorConvertTableBack[pGray[n]];
		ppColor[1][n] = colorMax/2;
	}
}

void CColorConvertorComplex::ConvertImageToGray( color **ppColor, byte *pGray, int cx )
{
	if (ppColor == NULL || pGray == NULL)
		return;

	int	x, y;
	byte	byteVal;
	for( long n = 0; n < cx; n++ )
	{
		x = ColorAsByte( ppColor[0][n] );
		y = ColorAsByte( ppColor[1][n] );

		byteVal = bytesConvertTable[(x<<8)|y];
		pGray[n] = byteVal;
	}
}

void CColorConvertorComplex::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_COMPLEX));
}

void CColorConvertorComplex::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	/*color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[0];
	p2 = ppColor[0];

	double X, Y, Z, fX, fY, fZ;
    double RR, GG, BB;
	double L, a, b;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		L = pLab[c++];
		a = pLab[c++];
		b = pLab[c++];

		fY = pow((L + 16.0) / 116.0, 3.0);
	    if (fY < 0.008856)
	       fY = L / 903.3;
	    Y = fY;

	    if (fY > 0.008856)
			fY = pow(fY, 1.0/3.0);
		else
			fY = 7.787 * fY + 16.0/116.0;

		fX = a / 500.0 + fY;
		if (fX > 0.206893)
			X = pow(fX, 3.0);
		else
			X = (fX - 16.0/116.0) / 7.787;

		fZ = fY - b /200.0;
		if (fZ > 0.206893)
			Z = pow(fZ, 3.0);
		else
			Z = (fZ - 16.0/116.0) / 7.787;

		X *= 0.950456;
		Y *= 1;
		Z *= 1.088754;
		
		RR =  (3.240479*X - 1.537150*Y - 0.498535*Z);
		GG =  (-0.969256*X + 1.875992*Y + 0.041556*Z);
		BB =  (0.055648*X - 0.204043*Y + 1.057311*Z);

		p0[n] = p1[n] = p2[n] = ByteAsColor((byte)Bright(ScaleDoubleToByte(BB), ScaleDoubleToByte(GG), ScaleDoubleToByte(RR)));
	}*/
}

void CColorConvertorComplex::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	/*color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[0];
	p2 = ppColor[0];

	double X, Y, Z, fX, fY, fZ;
	double R, G, B;
	double L, a, b;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		R = ScaleColorToDouble(p0[n]);
		G = ScaleColorToDouble(p1[n]);
		B = ScaleColorToDouble(p2[n]);

		X = (0.412453*R + 0.357580*G + 0.180423*B)/0.950456;
		Y = (0.212671*R + 0.715160*G + 0.072169*B);
	    Z = (0.019334*R + 0.119193*G + 0.950227*B)/1.088754;

		if (Y > 0.008856)
	    {
	       fY = pow(Y, 1.0/3.0);
		   L = 116.0*fY - 16.0;
		}
		else
		{
			fY = 7.787*Y + 16.0/116.0;
			L = 903.3*Y;
		}
		if (X > 0.008856)
			fX = pow(X, 1.0/3.0);
		else
			fX = 7.787*X + 16.0/116.0;
		if (Z > 0.008856)
			fZ = pow(Z, 1.0/3.0);
		else
			fZ = 7.787*Z + 16.0/116.0;

		a = 500.0*(fX - fY);
		b = 200.0*(fY - fZ);

		pLab[c++] = L;
		pLab[c++] = a;
		pLab[c++] = b;
	}
*/
}

void CColorConvertorComplex::GetPaneColor(int numPane, DWORD* pdwColor)
{
	if(pdwColor == 0) return;

	switch(numPane)
	{
	case 0:
		*pdwColor = RGB(128, 128, 128); 
		break;
	default:
		*pdwColor = 0;
	}
}

void CColorConvertorComplex::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
	case 1:
		*pdwColorValues =  RGB(255, 255, 255); 
		break;
	default:
		*pdwColorValues = 0;
	}
}
