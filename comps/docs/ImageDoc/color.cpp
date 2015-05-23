#include "stdafx.h"
#include "color.h"
#include <math.h>
#include "resource.h"
#include "ccmacro.h"


int nInterpolationMethod=-1; //метод интерполяции при увеличении
// 0 - по ближайшим соседям, 1 - билинейная
// -1 - прочитать значение из shell.data
int nMaskInterpolationMethod=-1; //метод интерполяции масок при увеличении
// 0 - квадратами, 1 - под углами 90 и 45 градусов


#define NotUsed 0 
/////


#define ScaleDoubleToColor(clr) ((color)max( 0, min( 65535,(((clr)+0.5)*65535) ) ))
#define ScaleDoubleToByte(clr)  ((byte)max(0, min(255,(((clr)+0.5)*255))))
//#define ScaleDoubleToColor(clr) ((color) (((clr)+0.5)*65535)) 
//#define ScaleDoubleToByte(clr)  ((byte)(((clr)+0.5)*255))

#define ScaleColorToDouble(clr) ((double)(((clr)/65535.0)-0.5))
#define ScaleByteToDouble(clr) ((double)(((clr)/255.0)-0.5))
inline double HueToRGB(double n1,double n2,double hue) 
{ 
   /* range check: note values passed add/subtract thirds of range */
   if (hue < 0)
      hue += 1.0;

   if (hue > 1.0)
      hue -= 1.0;

   /* return r,g, or b value from this tridrant */
   if (hue < (1.0/6))
      return ( n1 + (((n2-n1)*hue)/(1.0/6)) );
   if (hue < (1.0/2))
      return ( n2 );
   if (hue < (2.0/3))
      return ( n1 + (((n2-n1)*((2.0/3)-hue))/(1.0/6))); 
   else
      return ( n1 );
}  





IMPLEMENT_DYNCREATE(CColorConvertorRGB, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CColorConvertorYUV, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CColorConvertorYIQ, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CColorConvertorHSB, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CColorConvertorGRAY, CCmdTargetEx);

// {19C18618-43F4-11d3-A611-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorRGB, "ImageDoc.ColorConvertorRGB",
0x19c18618, 0x43f4, 0x11d3, 0xa6, 0x11, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

// {7D146711-678B-11d3-A4EB-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorYUV, "ImageDoc.ColorConvertorYUV", 
0x7d146711, 0x678b, 0x11d3, 0xa4, 0xeb, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

// {444CB4F7-B48C-11d3-A558-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorYIQ, "ImageDoc.ColorConvertorYIQ",
0x444cb4f7, 0xb48c, 0x11d3, 0xa5, 0x58, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

// {7FA4AA51-6ABB-11d3-A4ED-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorHSB, "ImageDoc.ColorConvertorHSB",
0x7fa4aa51, 0x6abb, 0x11d3, 0xa4, 0xed, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

// {56350D42-F999-11d3-A0C1-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CColorConvertorGRAY, "ImageDoc.ColorConvertorGRAY",
0x56350d42, 0xf999, 0x11d3, 0xa0, 0xc1, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87);

/////////////////////////////////////////////////////////////////
//color convertor RGB
/////////////////////////////////////////////////////////////////

BEGIN_INTERFACE_MAP(CColorConvertorRGB, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorRGB, IID_IColorConvertorEx, CnvEx)
	INTERFACE_PART(CColorConvertorRGB, IID_IColorConvertorEx2, CnvEx)
	INTERFACE_PART(CColorConvertorRGB, IID_IProvideHistColors, Hist)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorRGB, CnvEx)
IMPLEMENT_UNKNOWN(CColorConvertorRGB, Hist)

HRESULT CColorConvertorRGB::XHist::GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve )
{
	METHOD_PROLOGUE_EX(CColorConvertorRGB, Hist);

	byte	r = 0, g = 0, b = 0;
	if( nPaneNo == 0 )
		*pcrCurve = RGB( 255, 0, 0 );
	else if( nPaneNo == 1 )
		*pcrCurve = RGB( 0, 255, 0 );
	else if( nPaneNo == 2 )
		*pcrCurve = RGB( 0, 0, 255 );

	for( int i = 0; i < 256; i++, pcrArray++ )
	{
		if( nPaneNo == 0 )r = i;
		else if( nPaneNo == 1 )g= i;
		else if( nPaneNo == 2 )b= i;

		*pcrArray = RGB( r, g, b );
	}

	return S_OK;
}

HRESULT CColorConvertorRGB::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorRGB, CnvEx);

	byte	*pLookupTableR = pThis->m_pLookupTableR;
	byte	*pLookupTableG = pThis->m_pLookupTableG;
	byte	*pLookupTableB = pThis->m_pLookupTableB;

	byte	*ptemp = 0;
	if( dwFlags & cidrHilight )
	{
		ptemp = new byte[65536];
		memset( ptemp, 255, 65536 );

		byte	fillR = GetRValue(dwFillColor);																	\
		byte	fillG = GetGValue(dwFillColor);																	\
		byte	fillB = GetBValue(dwFillColor);																	\

		if( fillR )pLookupTableR = ptemp;
		if( fillG )pLookupTableG = ptemp;
		if( fillB )pLookupTableB = ptemp;
	}


	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);
			
		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		CONVERTTODIB_PART1

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);
			
		CONVERTTODIB_PART2
	}

	if( ptemp )delete ptemp;

	return S_OK;
}

HRESULT CColorConvertorRGB::XCnvEx::ConvertImageToDIBRect2( BITMAPINFOHEADER *pbi, BYTE *pdibBits, WORD *pDistBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IDistanceMap *pDistMap, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorRGB, CnvEx);

	byte	*pLookupTableR = pThis->m_pLookupTableR;
	byte	*pLookupTableG = pThis->m_pLookupTableG;
	byte	*pLookupTableB = pThis->m_pLookupTableB;

	byte	*ptemp = 0;
	if( dwFlags & cidrHilight )
	{
		ptemp = new byte[65536];
		memset( ptemp, 255, 65536 );

		byte	fillR = GetRValue(dwFillColor);																	\
		byte	fillG = GetGValue(dwFillColor);																	\
		byte	fillB = GetBValue(dwFillColor);																	\

		if( fillR )pLookupTableR = ptemp;
		if( fillG )pLookupTableG = ptemp;
		if( fillB )pLookupTableB = ptemp;
	}


	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);
			
		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		CONVERTTODIB_PART1_TRANSP

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);
			
		CONVERTTODIB_PART2_TRANSP
	}

	if( ptemp )delete ptemp;

	return S_OK;
}

CColorConvertorRGB::CColorConvertorRGB()
{
	_OleLockApp( this );
}

CColorConvertorRGB::~CColorConvertorRGB()
{
	_OleUnlockApp( this );
}


const char *CColorConvertorRGB::GetCnvName()
{
	return _T("RGB");
}

int CColorConvertorRGB::GetColorPanesCount()
{
	return 3;
}

const char *CColorConvertorRGB::GetPaneName(int numPane)
{
	if (numPane == 0)
		return _T("Red");
	else if (numPane == 1)
		return _T("Green");
	else if (numPane == 2)
		return _T("Blue");
	return _T("");
}

const char *CColorConvertorRGB::GetPaneShortName( int numPane )
{
	if (numPane == 0)
		return _T("R");
	else if (numPane == 1)
		return _T("G");
	else if (numPane == 2)
		return _T("B");
	return _T("");
}

color CColorConvertorRGB::ConvertToHumanValue( color colorInternal, int numPane )
{
	return color( double(colorInternal) * 256.0 / double(colorMax) );
}

DWORD CColorConvertorRGB::GetPaneFlags(int numPane)
{
	return pfOrdinary|pfGray;
}

void CColorConvertorRGB::ConvertRGBToImage( byte *_pRGB, color **_ppColor, int _cx )
{
	color	*p0, *p1, *p2;
	p0 = _ppColor[0];
	p1 = _ppColor[1];
	p2 = _ppColor[2];
	byte	*pRGB = _pRGB;
	for( long n = 0, c = 0; n < _cx; n++ )
	{
		p2[n] = ByteAsColor( _pRGB[c++] );
		p1[n] = ByteAsColor( _pRGB[c++] );
		p0[n] = ByteAsColor( _pRGB[c++] );
	}
//	__asm
//	{
//			mov	ebx, _pRGB
//			mov	eax, _ppColor
//			mov	ecx, _cx
//
//
//			push	ebp
//			push	esi
//			push	edi
//
//			mov		ebp, DWORD PTR [eax+0]		//_ppColor[0]
//			mov		esi, DWORD PTR [eax+4]		//_ppColor[0]
//			mov		edi, DWORD PTR [eax+8]		//_ppColor[0]
//			xor		eax, eax
//$loop:
//			mov		ah, BYTE PTR [ebx]
//			mov		WORD PTR [edi], ax
//			inc		ebx
//			mov		ah, BYTE PTR [ebx]
//			mov		WORD PTR [esi], ax
//			inc		ebx
//			mov		ah, BYTE PTR [ebx]
//			mov		WORD PTR [ebp], ax
//			inc		ebx
//
//			add		ebp, 2
//			add		esi, 2
//			add		edi, 2
//
//			dec		ecx
//			jne		$loop
//
//			pop		edi
//			pop		esi
//			pop		ebp
//			
//	}
}

void CColorConvertorRGB::ConvertImageToRGB( color **_ppColor, byte *_pRGB, int _cx )
{
	color	*p0, *p1, *p2;
	p0 = _ppColor[0];
	p1 = _ppColor[1];
	p2 = _ppColor[2];
	for( long n = 0, c = 0; n < _cx; n++ )
	{
		_pRGB[c++] = ColorAsByte( p2[n] );
		_pRGB[c++] = ColorAsByte( p1[n] );
		_pRGB[c++] = ColorAsByte( p0[n] );
	}
//	__asm
//	{
//			mov	ebx, _pRGB
//			mov	eax, _ppColor
//			mov	ecx, _cx
//
//
//			push	ebp
//			push	esi
//			push	edi
//
//			mov		ebp, DWORD PTR [eax+0]		//_ppColor[0]
//			mov		esi, DWORD PTR [eax+4]		//_ppColor[1]
//			mov		edi, DWORD PTR [eax+8]		//_ppColor[2]
//			xor		eax, eax
//			xor		edx, edx
//$loop:
//			mov		ax, WORD PTR [edi]
//			mov		BYTE PTR [ebx], ah
//			inc		ebx
//			mov		ax, WORD PTR [esi]
//			mov		BYTE PTR [ebx], ah
//			inc		ebx
//			mov		ax, WORD PTR [ebp]
//			mov		BYTE PTR [ebx], ah
//			inc		ebx
//
//			inc		ebp
//			inc		ebp
//			inc		esi
//			inc		esi
//			inc		edi
//			inc		edi
//
//			dec		ecx
//			jne		$loop
//
//			pop		edi
//			pop		esi
//			pop		ebp
//			
//	}
}


void CColorConvertorRGB::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];
	for( long n = 0; n < cx; n++ )
	{
		p2[n] = ByteAsColor( pGray[n] );
		p1[n] = ByteAsColor( pGray[n] );
		p0[n] = ByteAsColor( pGray[n] );
	}
}

void CColorConvertorRGB::ConvertImageToGray( color **ppColor, byte *pGray, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	for( long n = 0; n < cx; n++ )
	{
		// [vanek] : порядок пан : r, g, b   - 05.11.2004
		pGray[n] = Bright( ColorAsByte( p2[n] ), 
						ColorAsByte( p1[n] ), 
						ColorAsByte( p0[n] ) );

	}
}

void CColorConvertorRGB::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_RGB));
}

void CColorConvertorRGB::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

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
	
		p0[n] =  ScaleDoubleToColor(RR);
		p1[n] =  ScaleDoubleToColor(GG);
		p2[n] =  ScaleDoubleToColor(BB);
	}

}

void CColorConvertorRGB::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

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
}


void CColorConvertorRGB::GetPaneColor(int numPane, DWORD* pdwColor)
{
	if(pdwColor == 0) return;

	switch(numPane)
	{
	case 0:
		*pdwColor = RGB(255, 0, 0);
		break;
	case 1:
		*pdwColor = RGB(0, 255, 0);
		break;
	case 2:
		*pdwColor = RGB(0, 0, 255);
		break;
	default:
		*pdwColor = 0;
	}
}

void CColorConvertorRGB::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
		*pdwColorValues =  RGB(NotUsed, 0, 0);
		break;
	case 1:
		*pdwColorValues =  RGB(0, NotUsed, 0);
		break;
	case 2:
		*pdwColorValues =  RGB(0, 0, NotUsed);
		break;
	default:
		*pdwColorValues = 0;
	}
}

/////////////////////////////////////////////////////////////////
//color convertor YUV
/////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CColorConvertorYUV, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorYUV, IID_IColorConvertorEx, CnvEx)
	INTERFACE_PART(CColorConvertorYUV, IID_IProvideHistColors, Hist)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorYUV, CnvEx)
IMPLEMENT_UNKNOWN(CColorConvertorYUV, Hist)

HRESULT CColorConvertorYUV::XHist::GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve )
{
	double Y, U, V, R, G, B;
	byte	Rbyte, Gbyte, Bbyte;

	if( nPaneNo == 0 )
	{
		V = ScaleColorToDouble(ByteAsColor(128))*2;
		U = ScaleColorToDouble(ByteAsColor(128))*2;
		Y = ScaleColorToDouble(ByteAsColor(128))*2;
		*pcrCurve = RGB( 0, 0, 0 );
	}
	else if( nPaneNo == 1 )
	{
		V = ScaleColorToDouble(ByteAsColor(128))*2;
		U = ScaleColorToDouble(ByteAsColor(128))*2;
		Y = ScaleColorToDouble(ByteAsColor(128))*2;
		*pcrCurve = RGB( 255, 255, 0 );
	}
	else if( nPaneNo == 2 )
	{
		V = ScaleColorToDouble(ByteAsColor(128))*2;
		U = ScaleColorToDouble(ByteAsColor(128))*2;
		Y = ScaleColorToDouble(ByteAsColor(128))*2;
		*pcrCurve = RGB( 0, 0, 255 );
	}

	for( int i = 0; i < 256; i++, pcrArray++ )
	{
		if( nPaneNo == 0 )Y = ScaleColorToDouble(ByteAsColor(i))*2;
		else if( nPaneNo == 1 )U = ScaleColorToDouble(ByteAsColor(i))*2;
		else if( nPaneNo == 2 )V = ScaleColorToDouble(ByteAsColor(i))*2;
			
		B = Y + 2.0279*U;
		G = Y - 0.3938*U - 0.5805*V;
		R = Y            + 1.1398*V;

		Rbyte = ScaleDoubleToByte(R);
		Gbyte = ScaleDoubleToByte(G);
		Bbyte = ScaleDoubleToByte(B);

		*pcrArray = RGB( Rbyte, Gbyte, Bbyte );
	}

	return S_OK;
}



HRESULT CColorConvertorYUV::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorYUV, CnvEx);

	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		double Y, U, V, dR, dG, dB;

		V = ScaleColorToDouble(B)*2;
		U = ScaleColorToDouble(G)*2;
		Y = ScaleColorToDouble(R)*2;
			
		dB = Y + 2.0279*U;
		dG = Y - 0.3938*U - 0.5805*V;
		dR = Y            + 1.1398*V;


		byte	*pLookupTableR = pThis->m_pLookupTableR;
		byte	*pLookupTableG = pThis->m_pLookupTableG;
		byte	*pLookupTableB = pThis->m_pLookupTableB;

		if( pLookupTableR && pLookupTableG && pLookupTableB )
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
			Rbyte = pLookupTableR[Rbyte*255];
			Gbyte = pLookupTableG[Gbyte*255];
			Bbyte = pLookupTableB[Bbyte*255];
		}
		else
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
		}

		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		CONVERTTODIB_PART1

		double Y, U, V, dR, dG, dB;

		V = ScaleColorToDouble(B)*2;
		U = ScaleColorToDouble(G)*2;
		Y = ScaleColorToDouble(R)*2;
			
		dB = Y + 2.0279*U;
		dG = Y - 0.3938*U - 0.5805*V;
		dR = Y            + 1.1398*V;


		byte	*pLookupTableR = pThis->m_pLookupTableR;
		byte	*pLookupTableG = pThis->m_pLookupTableG;
		byte	*pLookupTableB = pThis->m_pLookupTableB;

		if( pLookupTableR && pLookupTableG && pLookupTableB )
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
			Rbyte = pLookupTableR[Rbyte*255];
			Gbyte = pLookupTableG[Gbyte*255];
			Bbyte = pLookupTableB[Bbyte*255];
		}
		else
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
		}

		CONVERTTODIB_PART2
	}

	//_catch_nested
	return S_OK;
}


CColorConvertorYUV::CColorConvertorYUV()
{
	_OleLockApp( this );
}

CColorConvertorYUV::~CColorConvertorYUV()
{
	_OleUnlockApp( this );
}


const char *CColorConvertorYUV::GetCnvName()
{
	return _T("YUV");
}

int CColorConvertorYUV::GetColorPanesCount()
{
	return 3;
}

const char *CColorConvertorYUV::GetPaneName(int numPane)
{
	if (numPane == 0)
		return _T("Y");
	else if (numPane == 1)
		return _T("U");
	else if (numPane == 2)
		return _T("V");
	return _T("");
}

const char *CColorConvertorYUV::GetPaneShortName( int numPane )
{
	return GetPaneName( numPane );
}

color CColorConvertorYUV::ConvertToHumanValue( color colorInternal, int numPane )
{
	return colorInternal * 255 / colorMax;
}


DWORD CColorConvertorYUV::GetPaneFlags(int numPane)
{
	if( numPane == 0 )return pfOrdinary|pfGray;
	return pfOrdinary;
}

void CColorConvertorYUV::ConvertRGBToImage( byte *pRGB, color **ppColor, int cx )
{
	if (ppColor==NULL || pRGB==NULL)
		return;
//           Y =  0.29900*R+0.58700*G+0.11400*B
//           U = -0.14740*R-0.28950*G+0.43690*B
//           V =  0.61500*R-0.51500*G-0.10000*B

	double r, g, b, Y, U, V;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		b = ScaleByteToDouble(pRGB[c++]);
		g = ScaleByteToDouble(pRGB[c++]);
		r = ScaleByteToDouble(pRGB[c++]);

		Y = 0.2990*r + 0.5870*g + 0.1140*b;
		U = -0.1474*r - 0.2895*g + 0.4369*b;
		V = 0.6150*r - 0.5150*g - 0.1000*b;
		
		ppColor[0][n] = ScaleDoubleToColor(Y/2);	// Y
		ppColor[1][n] = ScaleDoubleToColor(U/2);	// U
		ppColor[2][n] = ScaleDoubleToColor(V/2);	// V
	}
}

void CColorConvertorYUV::ConvertImageToRGB( color **ppColor, byte *pRGB, int cx )
{
	if (ppColor==NULL || pRGB==NULL)
		return;

//           R = Y          +1.13980*V
//           G = Y-0.39380*U-0.58050*V
//           B = Y+2.02790*U

	double y, u, v, R, G, B;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		y = ScaleColorToDouble(ppColor[0][n])*2;
		u = ScaleColorToDouble(ppColor[1][n])*2;
		v = ScaleColorToDouble(ppColor[2][n])*2;
		
		B = y + 2.0279*u;
		G = y - 0.3938*u - 0.5805*v;
		R = y            + 1.1398*v;

		pRGB[c++] = ScaleDoubleToByte(B);		// B
		pRGB[c++] = ScaleDoubleToByte(G);		// G
		pRGB[c++] = ScaleDoubleToByte(R);		// R				
	}	
}

void CColorConvertorYUV::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
	if (ppColor==NULL || pGray==NULL)
		return;
	for( long n = 0; n < cx; n++ )
	{
		ppColor[2][n] = ByteAsColor( pGray[n] );
		ppColor[1][n] = ByteAsColor( pGray[n] );
		ppColor[0][n] = ByteAsColor( pGray[n] );
	}
}

void CColorConvertorYUV::ConvertImageToGray( color **ppColor, byte *pGray, int cx )
{
	if (ppColor==NULL || pGray==NULL)
		return;
	double y, u, v;
	for( long n = 0; n < cx; n++ )
	{
		y = ScaleColorToDouble(ppColor[0][n])*2;
		u = ScaleColorToDouble(ppColor[1][n])*2;
		v = ScaleColorToDouble(ppColor[2][n])*2;
		pGray[n] = Bright( ScaleDoubleToByte(y + 2.0279*u),
						   ScaleDoubleToByte(y - 0.3938*u - 0.5805*v), 
						   ScaleDoubleToByte(y            + 1.1398*v));
	}
}

void CColorConvertorYUV::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_YUV));
}

void CColorConvertorYUV::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double X, Y, Z, fX, fY, fZ, U, V;
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
		
		RR =  ( 3.240479*X - 1.537150*Y - 0.498535*Z);
		GG =  (-0.969256*X + 1.875992*Y + 0.041556*Z);
		BB =  ( 0.055648*X - 0.204043*Y + 1.057311*Z);

		Y =  0.2990*RR + 0.5870*GG + 0.1140*BB;
		U = -0.1474*RR - 0.2895*GG + 0.4369*BB;
		V =  0.6150*RR - 0.5150*GG - 0.1000*BB;

		p0[n] = ScaleDoubleToColor( Y/2 );	// Y
		p1[n] = ScaleDoubleToColor( U/2 );	// U
		p2[n] = ScaleDoubleToColor( V/2 );	// V
	}

}

void CColorConvertorYUV::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double X, Y, Z, fX, fY, fZ;
	double R, G, B, y, u, v;
	double L, a, b;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		y = ScaleColorToDouble(p0[n])*2;
		u = ScaleColorToDouble(p1[n])*2;
		v = ScaleColorToDouble(p2[n])*2;
		
		B = y + 2.0279*u;
		G = y - 0.3938*u - 0.5805*v;
		R = y            + 1.1398*v;
		
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
}
	
void CColorConvertorYUV::GetPaneColor(int numPane, DWORD* pdwColor)
{
	if(pdwColor == 0) return;

	switch(numPane)
	{
	case 0:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	case 1:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	case 2:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	default:
		*pdwColor = 0;
	}
}

void CColorConvertorYUV::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
		*pdwColorValues =  RGB(NotUsed, 0, 0); 
		break;
	case 1:
		*pdwColorValues =  RGB(0, NotUsed, 0); 
		break;
	case 2:
		*pdwColorValues =  RGB(0, 0, NotUsed);
		break;
	default:
		*pdwColorValues = 0;
	}
}


/////////////////////////////////////////////////////////////////
//color convertor YIQ
/////////////////////////////////////////////////////////////////

BEGIN_INTERFACE_MAP(CColorConvertorYIQ, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorYIQ, IID_IColorConvertorEx, CnvEx)
	INTERFACE_PART(CColorConvertorYIQ, IID_IProvideHistColors, Hist)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorYIQ, CnvEx)
IMPLEMENT_UNKNOWN(CColorConvertorYIQ, Hist)

HRESULT CColorConvertorYIQ::XHist::GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve )
{
	METHOD_PROLOGUE_EX(CColorConvertorYIQ, Hist);

	byte	r = 0, g = 0, b = 0;
	if( nPaneNo == 0 )
		*pcrCurve = RGB( 255, 0, 0 );
	else if( nPaneNo == 1 )
		*pcrCurve = RGB( 0, 255, 0 );
	else if( nPaneNo == 2 )
		*pcrCurve = RGB( 0, 0, 255 );

	for( int i = 0; i < 256; i++, pcrArray++ )
	{
		if( nPaneNo == 0 )r = i;
		else if( nPaneNo == 1 )g= i;
		else if( nPaneNo == 2 )b= i;

		*pcrArray = RGB( r, g, b );
	}

	return S_OK;
}


HRESULT CColorConvertorYIQ::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorYIQ, CnvEx)

	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		double Y, I, Q, dR, dG, dB;

		Y = ScaleColorToDouble(R)*2;
		I = ScaleColorToDouble(G)*2;
		Q = ScaleColorToDouble(B)*2;

		dB = Y-1.10370*I+1.70060*Q;
		dG = Y-0.27270*I-0.64680*Q;
		dR = Y+0.95620*I+0.62140*Q;
		
		byte	*pLookupTableR = pThis->m_pLookupTableR;
		byte	*pLookupTableG = pThis->m_pLookupTableG;
		byte	*pLookupTableB = pThis->m_pLookupTableB;

		if( pLookupTableR && pLookupTableG && pLookupTableB )
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
			Rbyte = pLookupTableR[Rbyte*255];
			Gbyte = pLookupTableG[Gbyte*255];
			Bbyte = pLookupTableB[Bbyte*255];
		}
		else
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
		}

		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		CONVERTTODIB_PART1

		double Y, I, Q, dR, dG, dB;

		Y = ScaleColorToDouble(R)*2;
		I = ScaleColorToDouble(G)*2;
		Q = ScaleColorToDouble(B)*2;

		dB = Y-1.10370*I+1.70060*Q;
		dG = Y-0.27270*I-0.64680*Q;
		dR = Y+0.95620*I+0.62140*Q;
		
		byte	*pLookupTableR = pThis->m_pLookupTableR;
		byte	*pLookupTableG = pThis->m_pLookupTableG;
		byte	*pLookupTableB = pThis->m_pLookupTableB;

		if( pLookupTableR && pLookupTableG && pLookupTableB )
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
			Rbyte = pLookupTableR[Rbyte*255];
			Gbyte = pLookupTableG[Gbyte*255];
			Bbyte = pLookupTableB[Bbyte*255];
		}
		else
		{
			Rbyte = ScaleDoubleToByte(dR);
			Gbyte = ScaleDoubleToByte(dG);
			Bbyte = ScaleDoubleToByte(dB);
		}

		CONVERTTODIB_PART2
	}

	//_catch_nested
	return S_OK;
}

CColorConvertorYIQ::CColorConvertorYIQ()
{
	_OleLockApp( this );
}

CColorConvertorYIQ::~CColorConvertorYIQ()
{
	_OleUnlockApp( this );
}


const char *CColorConvertorYIQ::GetCnvName()
{
	return _T("YIQ");
}

int CColorConvertorYIQ::GetColorPanesCount()
{
	return 3;
}

const char *CColorConvertorYIQ::GetPaneName(int numPane)
{
	if (numPane == 0)
		return _T("Y");
	else if (numPane == 1)
		return _T("I");
	else if (numPane == 2)
		return _T("Q");
	return _T("");
}

const char *CColorConvertorYIQ::GetPaneShortName( int numPane )
{
	return GetPaneName( numPane );
}

color CColorConvertorYIQ::ConvertToHumanValue( color colorInternal, int numPane )
{
	return colorInternal * 255 / colorMax;
}


DWORD CColorConvertorYIQ::GetPaneFlags(int numPane)
{
	if( numPane == 0 )return pfOrdinary|pfGray;
	return pfOrdinary;
}

void CColorConvertorYIQ::ConvertRGBToImage( byte *pRGB, color **ppColor, int cx )
{
	if (ppColor==NULL || pRGB==NULL)
		return;
//           Y = 0.299*R+0.587*G+0.114*B
//           I = 0.596*R-0.274*G-0.322*B
//           Q = 0.211*R-0.523*G+0.312*B

	double R, G, B, Y, I, Q;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		B = ScaleByteToDouble(pRGB[c++]);
		G = ScaleByteToDouble(pRGB[c++]);
		R = ScaleByteToDouble(pRGB[c++]);

		Y = 0.299*R+0.587*G+0.114*B;
		I = 0.596*R-0.274*G-0.322*B;
		Q = 0.211*R-0.523*G+0.312*B;

		ppColor[0][n] = ScaleDoubleToColor( Y/2 );	// Y
		ppColor[1][n] = ScaleDoubleToColor( I/2 );	// I
		ppColor[2][n] = ScaleDoubleToColor( Q/2 );	// Q
		
	}
}

void CColorConvertorYIQ::ConvertImageToRGB( color **ppColor, byte *pRGB, int cx )
{
	if (ppColor==NULL || pRGB==NULL)
		return;

//           R = Y+0.95620*I+0.62140*Q
//           G = Y-0.27270*I-0.64680*Q
//           B = Y-1.10370*I+1.70060*Q

	double Y, I, Q;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		Y = ScaleColorToDouble(ppColor[0][n])*2;
		I = ScaleColorToDouble(ppColor[1][n])*2;
		Q = ScaleColorToDouble(ppColor[2][n])*2;

		pRGB[c++] = ScaleDoubleToByte(Y-1.10370*I+1.70060*Q);	// B
		pRGB[c++] = ScaleDoubleToByte(Y-0.27270*I-0.64680*Q);	// G
		pRGB[c++] = ScaleDoubleToByte(Y+0.95620*I+0.62140*Q);	// R		
	}
}

void CColorConvertorYIQ::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
	if (ppColor==NULL || pGray==NULL)
		return;
	for( long n = 0; n < cx; n++ )
	{
		ppColor[2][n] = ByteAsColor( pGray[n] );
		ppColor[1][n] = ByteAsColor( pGray[n] );
		ppColor[0][n] = ByteAsColor( pGray[n] );
	}
}

void CColorConvertorYIQ::ConvertImageToGray( color **ppColor, byte *pGray, int cx )
{
	if (ppColor==NULL || pGray==NULL)
		return;
	double Y, I, Q;
	for( long n = 0; n < cx; n++ )
	{
		Y = ScaleColorToDouble(ppColor[0][n])*2;
		I = ScaleColorToDouble(ppColor[1][n])*2;
		Q = ScaleColorToDouble(ppColor[2][n])*2;
		pGray[n] = Bright( ScaleDoubleToByte(Y-1.10370*I+1.70060*Q), 
						   ScaleDoubleToByte(Y-0.27270*I-0.64680*Q), 
						   ScaleDoubleToByte(Y+0.95620*I+0.62140*Q));
	}
}

void CColorConvertorYIQ::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_YIQ));
}

void CColorConvertorYIQ::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double X, Y, Z, fX, fY, fZ, I, Q;
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

		Y = 0.299*RR+0.587*GG+0.114*BB;
		I = 0.596*RR-0.274*GG-0.322*BB;
		Q = 0.211*RR-0.523*GG+0.312*BB;

		p0[n] = ScaleDoubleToColor( Y/2 );	// Y
		p1[n] = ScaleDoubleToColor( I/2 );	// I
		p2[n] = ScaleDoubleToColor( Q/2 );	// Q
	}

}

void CColorConvertorYIQ::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double X, Y, Z, fX, fY, fZ;
	double R, G, B, y, i, q;
	double L, a, b;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		y = ScaleColorToDouble(p0[n])*2;
		i = ScaleColorToDouble(p1[n])*2;
		q = ScaleColorToDouble(p2[n])*2;
		
        R = y+0.95620*i+0.62140*q;
        G = y-0.27270*i-0.64680*q;
        B = y-1.10370*i+1.70060*q;

	
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
}

void CColorConvertorYIQ::GetPaneColor(int numPane, DWORD* pdwColor)
{
	if(pdwColor == 0) return;

	switch(numPane)
	{
	case 0:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	case 1:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	case 2:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	default:
		*pdwColor = 0;
	}
}

void CColorConvertorYIQ::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
		*pdwColorValues =  RGB(NotUsed, 0, 0); 
		break;
	case 1:
		*pdwColorValues =  RGB(0, NotUsed, 0); 
		break;
	case 2:
		*pdwColorValues =  RGB(0, 0, NotUsed);
		break;
	default:
		*pdwColorValues = 0;
	}
}


/////////////////////////////////////////////////////////////////
//color convertor HSB
/////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CColorConvertorHSB, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorHSB, IID_IColorConvertorEx, CnvEx)
	INTERFACE_PART(CColorConvertorHSB, IID_IProvideHistColors, Hist)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorHSB, CnvEx)
IMPLEMENT_UNKNOWN(CColorConvertorHSB, Hist)

HRESULT CColorConvertorHSB::XHist::GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve )
{
	METHOD_PROLOGUE_EX(CColorConvertorHSB, Hist);

	byte	bgr[3];
	color H = colorMax/2;
	color S = colorMax;
	color L = colorMax/2;

	if( nPaneNo == 2 )
		S = 0;

	if( nPaneNo == 0 )
		*pcrCurve = RGB( 255, 255, 255 );
	else if( nPaneNo == 1 )
		*pcrCurve = RGB( 0, 0, 255 );
	else if( nPaneNo == 2 )
		*pcrCurve = RGB( 0, 0, 0 );

	color	*ppcolors[3];
	ppcolors[0] = &H;
	ppcolors[1] = &S;
	ppcolors[2] = &L;

	for( int i = 0; i < 256; i++, pcrArray++ )
	{
		if( nPaneNo == 0 )H = ByteAsColor( i );
		else if( nPaneNo == 1 )S = ByteAsColor( i );
		else if( nPaneNo == 2 )L = ByteAsColor( i );

		pThis->ConvertImageToRGB( ppcolors, bgr, 1 );

		*pcrArray = RGB( bgr[2], bgr[1], bgr[0] );
	}

	return S_OK;
}


HRESULT CColorConvertorHSB::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorHSB, CnvEx);
	double fltR, fltG, fltB, H, S, L;

	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR


		H = ScaleColorToDouble(R);
		S = ScaleColorToDouble(G);
		L = ScaleColorToDouble(B);

		
		H+=.5;
		S+=.5;
		L+=.5;
		double Magic1, Magic2;
		if (S == 0) 
		{            /* achromatic case */
			fltR=fltG=fltB=L;
		}	
		else  
		{                    /* chromatic case */
			/* set up magic numbers */
			if (L <= (1.0/2))
				Magic2 = (L*(1.0 + S));
			else
				Magic2 = L + S - ((L*S));
			Magic1 = 2*L-Magic2;

			/* get RGB, change units from HLSMAX to RGBMAX */
			fltR = (HueToRGB(Magic1,Magic2,H+(1.0/3))); 
			fltG = (HueToRGB(Magic1,Magic2,H));
			fltB = (HueToRGB(Magic1,Magic2,H-(1.0/3))); 
		}


		
		fltR-=.5;
		fltG-=.5;
		fltB-=.5;

		byte	*pLookupTableR = pThis->m_pLookupTableR;
		byte	*pLookupTableG = pThis->m_pLookupTableG;
		byte	*pLookupTableB = pThis->m_pLookupTableB;

		if( pLookupTableR && pLookupTableG && pLookupTableB )
		{
			Rbyte = ScaleDoubleToByte(fltR);
			Gbyte = ScaleDoubleToByte(fltG);
			Bbyte = ScaleDoubleToByte(fltB);
			Rbyte = pLookupTableR[Rbyte*255];
			Gbyte = pLookupTableG[Gbyte*255];
			Bbyte = pLookupTableB[Bbyte*255];
		}
		else
		{
			Bbyte = ScaleDoubleToByte(fltB);
			Gbyte = ScaleDoubleToByte(fltG);
			Rbyte = ScaleDoubleToByte(fltR);
		}
		
		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		CONVERTTODIB_PART1


		H = ScaleColorToDouble(R);
		S = ScaleColorToDouble(G);
		L = ScaleColorToDouble(B);

		
		H+=.5;
		S+=.5;
		L+=.5;
		double Magic1, Magic2;
		if (S == 0) 
		{            /* achromatic case */
			fltR=fltG=fltB=L;
		}	
		else  
		{                    /* chromatic case */
			/* set up magic numbers */
			if (L <= (1.0/2))
				Magic2 = (L*(1.0 + S));
			else
				Magic2 = L + S - ((L*S));
			Magic1 = 2*L-Magic2;

			/* get RGB, change units from HLSMAX to RGBMAX */
			fltR = (HueToRGB(Magic1,Magic2,H+(1.0/3))); 
			fltG = (HueToRGB(Magic1,Magic2,H));
			fltB = (HueToRGB(Magic1,Magic2,H-(1.0/3))); 
		}


		
		fltR-=.5;
		fltG-=.5;
		fltB-=.5;

		byte	*pLookupTableR = pThis->m_pLookupTableR;
		byte	*pLookupTableG = pThis->m_pLookupTableG;
		byte	*pLookupTableB = pThis->m_pLookupTableB;

		if( pLookupTableR && pLookupTableG && pLookupTableB )
		{
			Rbyte = ScaleDoubleToByte(fltR);
			Gbyte = ScaleDoubleToByte(fltG);
			Bbyte = ScaleDoubleToByte(fltB);
			Rbyte = pLookupTableR[Rbyte*255];
			Gbyte = pLookupTableG[Gbyte*255];
			Bbyte = pLookupTableB[Bbyte*255];
		}
		else
		{
			Bbyte = ScaleDoubleToByte(fltB);
			Gbyte = ScaleDoubleToByte(fltG);
			Rbyte = ScaleDoubleToByte(fltR);
		}
		
		CONVERTTODIB_PART2
	}

	return S_OK;
}


CColorConvertorHSB::CColorConvertorHSB()
{
	_OleLockApp( this );
}

CColorConvertorHSB::~CColorConvertorHSB()
{
	_OleUnlockApp( this );
}


const char *CColorConvertorHSB::GetCnvName()
{
	return _T("HSB");
}

int CColorConvertorHSB::GetColorPanesCount()
{
	return 3;
}

const char *CColorConvertorHSB::GetPaneName(int numPane)
{
	if (numPane == 0)
		return _T("Hue");
	else if (numPane == 1)
		return _T("Saturation");
	else if (numPane == 2)
		return _T("Brightness");
	return _T("");
}

const char *CColorConvertorHSB::GetPaneShortName( int numPane )
{
	if (numPane == 0)
		return _T("H");
	else if (numPane == 1)
		return _T("S");
	else if (numPane == 2)
		return _T("B");
	return _T("");
}

color CColorConvertorHSB::ConvertToHumanValue( color colorInternal, int numPane )
{
	if( numPane == 0 )//H
	{
		color H;		
		H = ( colorInternal - 65535 + 65535/3);
		H = -color( ( ( (double)H * 360.0 /65535 - 360.0 ) ) );
		return H;

	}
	else
	if( numPane == 1 )//S
	{
		color S = color(( (double)colorInternal * 100.0 / double(colorMax) ));
		return S;
	}
	else
	if( numPane == 2 )//L
	{
		color L = color(( (double)colorInternal * 100.0 / double(colorMax) ));		
		return L;
	}
	
	return colorInternal * 255 / colorMax;
}



DWORD CColorConvertorHSB::GetPaneFlags(int numPane)
{
	if( numPane == 0 )return pfOrdinary|pfCycled;
	else if( numPane == 2 )return pfOrdinary|pfGray;
	else return pfOrdinary;
}

void CColorConvertorHSB::ConvertRGBToImage( byte *pRGB, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double BB, GG, RR, H, S, B;//, u, v;

  //Convert RGB to HSL colorspace.
	for( long n = 0, c = 0; n < cx; n++ )
	{
		BB = ScaleByteToDouble(pRGB[c++]);
		GG = ScaleByteToDouble(pRGB[c++]);
		RR = ScaleByteToDouble(pRGB[c++]);


		RR+=0.5;
		GG+=0.5;
		BB+=0.5;
		//RR*=RGBMAX;
		//GG*=RGBMAX;
		//BB*=RGBMAX;
		if(RR<0)RR=0;
		if(RR>1.0)RR=1.0;
		if(GG<0)GG=0;
		if(GG>1.0)GG=1.0;
		if(BB<0)BB=0;
		if(BB>1.0)BB=1.0;

		double cMax, cMin, Rdelta, Gdelta, Bdelta;
		cMax = max( max(RR,GG), BB);
		cMin = min( min(RR,GG), BB);
		B = (cMax+cMin)/2.0;

		if (cMax == cMin) 
		{           /* r=g=b --> achromatic case */
			S = 0;                     /* saturation */
			H = 0;             /* hue */
		}
		else 
		{                        /* chromatic case */
			/* saturation */
			if (B <= (1.0/2))
				S=  (cMax-cMin ) / (cMax+cMin);
			else
				S = ( (cMax-cMin)) / (2.0-cMax-cMin);

			/* hue */
			Rdelta = ( ((cMax-RR)*(1.0/6))) / (cMax-cMin);
			Gdelta = ( ((cMax-GG)*(1.0/6))) / (cMax-cMin);
			Bdelta = ( ((cMax-BB)*(1.0/6))) / (cMax-cMin);

			if (RR == cMax)
				H = Bdelta - Gdelta;
			else if (GG == cMax)
				H = (1.0/3) + Rdelta - Bdelta;
			else /* BB == cMax */
				H = ((2*1.0)/3) + Gdelta - Rdelta;

			if (H < 0)
				H += 1.0;
			if (H > 1.0)
				H -= 1.0;
		}

		H -=.5;
		S -=.5;
		B -=.5;

		//B =  (r + g + b)/3.0;
		/*B = (RR*60+GG*118+BB*22)/200;
		u =  2.0/3.0*RR - 1.0/3.0*GG - 1.0/3.0*BB;
		v = -1.0/3.0*RR + 2.0/3.0*GG - 1.0/3.0*BB;

		H = atan2(v, u);
		S = sqrt(v*v + u*u);
		H/= (2*PI);
		S/=4;
		/*Max=max(RR,max(GG,BB));
		Min=min(RR,min(GG,BB));
		H=(-1.0);
		S=0.0;
		B=(Min+Max)/2.0;
		delta=Max-Min;
		if (delta == 0.0)
			return;
		S=delta/((B <= 0.5) ? (Min+Max) : (2.0-Max-Min));
		
		if (RR == Max)
			H=(GG == Min ? 5.0+(Max-BB)/delta : 1.0-(Max-GG)/delta);
		else
			if (GG == Max)
				H=(BB == Min ? 1.0+(Max-RR)/delta : 3.0-(Max-BB)/delta);
			else
				H=(RR == Min ? 3.0+(Max-GG)/delta : 5.0-(Max-RR)/delta);
		H/=12.0;
		S/=16;
*/
		p0[n] = ScaleDoubleToColor(H);	// H
		p1[n] = ScaleDoubleToColor(S);	// S
		p2[n] = ScaleDoubleToColor(B);	// B
	}
}



void CColorConvertorHSB::ConvertImageToRGB( color **ppColor, byte *pRGB, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];
	double R, G, B, H, S, L;


	for( long n = 0, c = 0; n < cx; n++ )
	{
		H = ScaleColorToDouble(p0[n]);
		S = ScaleColorToDouble(p1[n]);
		L = ScaleColorToDouble(p2[n]);

		/*H*= 2*PI;
		S*=2;
		//if(H>PI)
		//	H-=PI;
		tanH = tan(H);

		u = sqrt(1/(1+tanH*tanH))*S;
		v = u*tanH;

		R = L + u;
		G = L + v;
		B = L - u - v;
		*/
		
		//S *= 8;	
		H+=.5;
		S+=.5;
		L+=.5;
		double Magic1, Magic2;
		if (S == 0) 
		{            /* achromatic case */
			R=G=B=L;
		}	
		else  
		{                    /* chromatic case */
			/* set up magic numbers */
			if (L <= (1.0/2))
				Magic2 = (L*(1.0 + S));
			else
				Magic2 = L + S - ((L*S));
			Magic1 = 2*L-Magic2;

			/* get RGB, change units from HLSMAX to RGBMAX */
			R = (HueToRGB(Magic1,Magic2,H+(1.0/3))); 
			G = (HueToRGB(Magic1,Magic2,H));
			B = (HueToRGB(Magic1,Magic2,H-(1.0/3))); 
		}


		
		//pRGB[c++] = byte(B);
		//pRGB[c++] = byte(G);
		//pRGB[c++] = byte(R);
		//R/=RGBMAX;
		//G/=RGBMAX;
		//B/=RGBMAX;
		R-=.5;
		G-=.5;
		B-=.5;

		pRGB[c++]=ScaleDoubleToByte(B);
		pRGB[c++]=ScaleDoubleToByte(G);
		pRGB[c++]=ScaleDoubleToByte(R);
				
	}


    //Convert HSL to RGB colorspace.
/*	double hue, saturation, luminosity, r, g, b;
		
	for( long n = 0, c = 0; n < cx; n++ )
	{
		hue = ScaleColorToDouble(p0[n])*2;
		saturation = ScaleColorToDouble(p1[n])*16;
		luminosity = ScaleColorToDouble(p2[n]);
		v=(luminosity <= 0.5) ? (luminosity*(1.0+saturation)) : (luminosity+saturation-luminosity*saturation);
		if ((saturation == 0.0) || (hue == -1.0))
		{
			pRGB[c++] =ScaleDoubleToByte(luminosity);
			pRGB[c++] =ScaleDoubleToByte(luminosity);
			pRGB[c++] =ScaleDoubleToByte(luminosity);
			continue;
		}
		y=2*luminosity-v;
		x=y+(v-y)*(6.0*hue-(int) (6.0*hue));
		z=v-(v-y)*(6.0*hue-(int) (6.0*hue));
		switch ((int) (6.0*hue))
		{
			default: r=v; g=x; b=y; break;
			case 0: r=v; g=x; b=y; break;
			case 1: r=z; g=v; b=y; break;
			case 2: r=y; g=v; b=x; break;
			case 3: r=y; g=z; b=v; break;
			case 4: r=x; g=y; b=v; break;
			case 5: r=v; g=y; b=z; break;
		}
		pRGB[c++]=ScaleDoubleToByte(b);
		pRGB[c++]=ScaleDoubleToByte(g);
		pRGB[c++]=ScaleDoubleToByte(r);
		
	}*/
}

void CColorConvertorHSB::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
	for( long n = 0; n < cx; n++ )
	{
		ppColor[2][n] = ByteAsColor( pGray[n] );
		ppColor[1][n] = ByteAsColor( pGray[n] );
		ppColor[0][n] = ByteAsColor( pGray[n] );
	}
}

void CColorConvertorHSB::ConvertImageToGray( color **ppColor, byte *pGray, int cx )
{
	/*for( long n = 0; n < cx; n++ )
		pGray[n] = Bright( ScaleDoubleToByte(ppColor[2][n] + 1.1398*ppColor[0][n]), 
						ScaleDoubleToByte(ppColor[2][n] - 0.3938*ppColor[1][n] - 0.5805*ppColor[0][n]), 
						ScaleDoubleToByte(ppColor[2][n] + 2.0279*ppColor[1][n]));
	*/
}

void CColorConvertorHSB::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_HSB));
}

void CColorConvertorHSB::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];
	//int MaxRGB = 65535;
	//double delta, Max, Min, Rdelta, Bdelta, Gdelta;

	double X, Y, Z, fX, fY, fZ;
    double RR, GG, BB;
	double L, a, b, H, S, B;
	//int nH;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		L = pLab[c++];
		a = pLab[c++];
		b = pLab[c++];

		//B = L;
		//H = atan2(b, a) - ;
		//S = sqrt(a*a + b*b);

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


		
		/*u =  2.0/3.0*RR - 1.0/3.0*GG - 1.0/3.0*BB;
		v = -1.0/3.0*RR + 2.0/3.0*GG - 1.0/3.0*BB;

		H = atan2(v, u);
		B = (RR + GG + BB)/3.0;
		//B = (RR*60+GG*118+BB*22)/200;
		S = sqrt(v*v + u*u);

		H /= (2*PI);
		S /= 2;
		*/
		//ASSERT(S < .5);
		//ASSERT(H < .5);
		//ASSERT(H > -.5);
		//ASSERT(B < .5);
		//ASSERT(B > -.51);
		//H+=PI;


		//Convert RGB to HSL colorspace.
		//RR+=0.5;
		//GG+=0.5;
		//BB+=0.5;
		/*Max=max(RR,max(GG,BB));
		Min=min(RR,min(GG,BB));
		H=(-1.0);
		S=0.0;
		B=(Min+Max)/2.0;
		delta=Max-Min;
		if (delta == 0.0)
			return;
		S=delta/((B <= 0.5) ? (Min+Max) : (2.0-Max-Min));
		
		if (RR == Max)
			H=(GG == Min ? 5.0+(Max-BB)/delta : 1.0-(Max-GG)/delta);
		else
			if (GG == Max)
				H=(BB == Min ? 1.0+(Max-RR)/delta : 3.0-(Max-BB)/delta);
			else
				H=(RR == Min ? 3.0+(Max-GG)/delta : 5.0-(Max-RR)/delta);
		H/=12.0;
		
		S/=16;
		*/
		

		/* get R, G, and B out of DWORD */
		/* calculate lightness */
		RR+=0.5;
		GG+=0.5;
		BB+=0.5;
		//RR*=RGBMAX;
		//GG*=RGBMAX;
		//BB*=RGBMAX;
		if(RR<0)RR=0;
		if(RR>1.0)RR=1.0;
		if(GG<0)GG=0;
		if(GG>1.0)GG=1.0;
		if(BB<0)BB=0;
		if(BB>1.0)BB=1.0;

		double cMax, cMin, Rdelta, Gdelta, Bdelta;
		cMax = max( max(RR,GG), BB);
		cMin = min( min(RR,GG), BB);
		B = (cMax+cMin)/2.0;

		if (cMax == cMin) 
		{           /* r=g=b --> achromatic case */
			S = 0;                     /* saturation */
			H = 0;             /* hue */
		}
		else 
		{                        /* chromatic case */
			/* saturation */
			if (B <= (1.0/2))
				S=  (cMax-cMin ) / (cMax+cMin);
			else
				S = ( (cMax-cMin)) / (2.0-cMax-cMin);

			/* hue */
			Rdelta = ( ((cMax-RR)*(1.0/6))) / (cMax-cMin);
			Gdelta = ( ((cMax-GG)*(1.0/6))) / (cMax-cMin);
			Bdelta = ( ((cMax-BB)*(1.0/6))) / (cMax-cMin);

			if (RR == cMax)
				H = Bdelta - Gdelta;
			else if (GG == cMax)
				H = (1.0/3) + Rdelta - Bdelta;
			else /* BB == cMax */
				H = ((2*1.0)/3) + Gdelta - Rdelta;

			if (H < 0)
				H += 1.0;
			if (H > 1.0)
				H -= 1.0;
		}

		H -=.5;
		S -=.5;
		B -=.5;
		
		//ASSERT(S > -.5);
		//ASSERT(S < .5);
		//ASSERT(H < .5);
		//ASSERT(H > -.5);
		//ASSERT(B < .5);
		//ASSERT(B > -.51);

		p0[n] = ScaleDoubleToColor(H);	// H
		p1[n] = ScaleDoubleToColor(S);	// S
		p2[n] = ScaleDoubleToColor(B);	// B
	}
}

void CColorConvertorHSB::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	color	*p0, *p1, *p2;
	p0 = ppColor[0];
	p1 = ppColor[1];
	p2 = ppColor[2];

	double X, Y, Z, fX, fY, fZ;
	double RR, GG, BB;
	double L, a, b;
	//double hue, saturation, luminosity;
	double H, S, B;

	//double v, u, tanH;

	for( long n = 0, c = 0; n < cx; n++ )
	{
		H = ScaleColorToDouble(p0[n]);
		S = ScaleColorToDouble(p1[n]);
		B = ScaleColorToDouble(p2[n]);
		H+=.5;
		S+=.5;
		B+=.5;
		double Magic1, Magic2;
		if (S == 0) 
		{            /* achromatic case */
			RR=GG=BB=B;
		}	
		else  
		{                    /* chromatic case */
			/* set up magic numbers */
			if (B <= (1.0/2))
				Magic2 = (B*(1.0 + S));
			else
				Magic2 = B + S - ((B*S));
			Magic1 = 2*B-Magic2;

			/* get RGB, change units from HLSMAX to RGBMAX */
			RR = (HueToRGB(Magic1,Magic2,H+(1.0/3))); 
			GG = (HueToRGB(Magic1,Magic2,H));
			BB = (HueToRGB(Magic1,Magic2,H-(1.0/3))); 
		}
		RR-=.5;
		GG-=.5;
		BB-=.5;
		/*if(RR<-.5)RR=-.5;
		if(RR>.5)RR=.5;
		if(GG<-.5)GG=-.5;
		if(GG>.5)GG=.5;
		if(BB<-.5)BB=-.5;
		if(BB>.5)BB=.5;
		*/



		//hue = ScaleColorToDouble(p0[n])*2;
		//saturation = ScaleColorToDouble(p1[n])*16;
		//luminosity = ScaleColorToDouble(p2[n]);

		/*v=(luminosity <= 0.5) ? (luminosity*(1.0+saturation)) : (luminosity+saturation-luminosity*saturation);

		if ((saturation == 0.0) || (hue == -1.0))
		{
			R = luminosity;
			G = luminosity;
			B = luminosity;
			continue;
		}
		y=2*luminosity-v;
		x=y+(v-y)*(6.0*hue-(int) (6.0*hue));
		z=v-(v-y)*(6.0*hue-(int) (6.0*hue));
		switch ((int) (6.0*hue))
		{
			default: R=v; G=x; B=y; break;
			case 0: R=v; G=x; B=y; break;
			case 1: R=z; G=v; B=y; break;
			case 2: R=y; G=v; B=x; break;
			case 3: R=y; G=z; B=v; break;
			case 4: R=x; G=y; B=v; break;
			case 5: R=v; G=y; B=z; break;
		}
		*/
		//hue *= PI;
		//saturation*=4;
		//tanH = tan(hue);

		//u = sqrt(1/(1+tanH*tanH))*saturation;
		//v = u*tanH;

		//R = luminosity + u;
		//G = luminosity + v;
		//B = luminosity - u - v;


		X = (0.412453*RR + 0.357580*GG + 0.180423*BB)/0.950456;
		Y = (0.212671*RR + 0.715160*GG + 0.072169*BB);
	    Z = (0.019334*RR + 0.119193*GG + 0.950227*BB)/1.088754;

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
}

void CColorConvertorHSB::GetPaneColor(int numPane, DWORD* pdwColor)
{
	if(pdwColor == 0) return;

	switch(numPane)
	{
	case 0:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	case 1:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	case 2:
		*pdwColor = RGB(0, 0, 0); //need to correct!!!
		break;
	default:
		*pdwColor = 0;
	}
}

void CColorConvertorHSB::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
		*pdwColorValues =  RGB(NotUsed, 0, 0); 
		break;
	case 1:
		*pdwColorValues =  RGB(0, NotUsed, 0); 
		break;
	case 2:
		*pdwColorValues =  RGB(0, 0, NotUsed);
		break;
	default:
		*pdwColorValues = 0;
	}
}

/////////////////////////////////////////////////////////////////
//color convertor GRAY
/////////////////////////////////////////////////////////////////

BEGIN_INTERFACE_MAP(CColorConvertorGRAY, CColorConvertorBase)
	INTERFACE_PART(CColorConvertorGRAY, IID_IColorConvertorEx, CnvEx)
	INTERFACE_PART(CColorConvertorGRAY, IID_IColorConvertorEx2, CnvEx)
	INTERFACE_PART(CColorConvertorGRAY, IID_IProvideHistColors, Hist)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CColorConvertorGRAY, CnvEx)
IMPLEMENT_UNKNOWN(CColorConvertorGRAY, Hist)

HRESULT CColorConvertorGRAY::XHist::GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve )
{
	METHOD_PROLOGUE_EX(CColorConvertorGRAY, Hist);

	*pcrCurve = RGB( 0, 0, 0 );
	
	for( int i = 0; i < 256; i++, pcrArray++ )
		*pcrArray = RGB( i, i, i );

	return S_OK;
}

HRESULT CColorConvertorGRAY::XCnvEx::ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorGRAY, CnvEx)

	byte	*pLookupTableR = pThis->m_pLookupTableR;
	byte	*pLookupTableG = pThis->m_pLookupTableG;
	byte	*pLookupTableB = pThis->m_pLookupTableB;

	byte	*ptemp = 0;
	if( dwFlags & cidrHilight )
	{
		ptemp = new byte[65536];
		memset( ptemp, 255, 65536 );

		byte	fillR = GetRValue(dwFillColor);																	\
		byte	fillG = GetGValue(dwFillColor);																	\
		byte	fillB = GetBValue(dwFillColor);																	\

		if( fillR )pLookupTableR = ptemp;
		if( fillG )pLookupTableG = ptemp;
		if( fillB )pLookupTableB = ptemp;
	}

	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);

		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		CONVERTTODIB_PART1

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);

		CONVERTTODIB_PART2
	}
	if( ptemp )delete ptemp;
	
	return S_OK;
}

HRESULT CColorConvertorGRAY::XCnvEx::ConvertImageToDIBRect2( BITMAPINFOHEADER *pbi, BYTE *pdibBits, WORD *pDistBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IDistanceMap *pDistMap, IUnknown *ppunkParams)
{
	METHOD_PROLOGUE_EX(CColorConvertorGRAY, CnvEx)

	byte	*pLookupTableR = pThis->m_pLookupTableR;
	byte	*pLookupTableG = pThis->m_pLookupTableG;
	byte	*pLookupTableB = pThis->m_pLookupTableB;

	byte	*ptemp = 0;
	if( dwFlags & cidrHilight )
	{
		ptemp = new byte[65536];
		memset( ptemp, 255, 65536 );

		byte	fillR = GetRValue(dwFillColor);																	\
		byte	fillG = GetGValue(dwFillColor);																	\
		byte	fillB = GetBValue(dwFillColor);																	\

		if( fillR )pLookupTableR = ptemp;
		if( fillG )pLookupTableG = ptemp;
		if( fillB )pLookupTableB = ptemp;
	}

	if(nInterpolationMethod==-1 || nMaskInterpolationMethod==-1)
	{
		nInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageInterpolationMethod", 0);
		nMaskInterpolationMethod = GetValueInt(GetAppUnknown(), "\\General", "ImageMaskInterpolationMethod", 0);
	}
	if(nInterpolationMethod==1 && fZoom>1.1)
	{
		CONVERTTODIB_PART1_BILINEAR

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);

		CONVERTTODIB_PART2_BILINEAR
	}
	else
	{
		CONVERTTODIB_PART1_TRANSP

		Rbyte = pLookupTableR?pLookupTableR[R]:(R>>8);
		Gbyte = pLookupTableG?pLookupTableG[G]:(G>>8);
		Bbyte = pLookupTableB?pLookupTableB[B]:(B>>8);

		CONVERTTODIB_PART2_TRANSP
	}
	if( ptemp )delete ptemp;
	
	return S_OK;
}





CColorConvertorGRAY::CColorConvertorGRAY()
{
	_OleLockApp( this );
}

CColorConvertorGRAY::~CColorConvertorGRAY()
{
	_OleUnlockApp( this );
}


const char *CColorConvertorGRAY::GetCnvName()
{
	return _T("GRAY");
}

int CColorConvertorGRAY::GetColorPanesCount()
{
	return 1;
}

const char *CColorConvertorGRAY::GetPaneName(int numPane)
{
	if (numPane == 0)
		return _T("Gray");
	return _T("");
}

const char *CColorConvertorGRAY::GetPaneShortName( int numPane )
{
	if (numPane == 0)
		return _T("G");
	return _T("");
}

color CColorConvertorGRAY::ConvertToHumanValue( color colorInternal, int numPane )
{
	return colorInternal * 255 / colorMax;
}


DWORD CColorConvertorGRAY::GetPaneFlags(int numPane)
{
	return pfOrdinary|pfGray;
}

void CColorConvertorGRAY::ConvertRGBToImage( byte *pRGB, color **ppColor, int cx )
{
	if (ppColor == NULL || pRGB == NULL)
		return;

	byte	r, g, b, byteC;
	for( long n = 0, c = 0; n < cx; n++ )
	{
		b = pRGB[c++];
		g = pRGB[c++];
		r = pRGB[c++];
		byteC = Bright(r, g, b);
		ppColor[0][n] = ByteAsColor(byteC);

	}
}

void CColorConvertorGRAY::ConvertImageToRGB( color **ppColor, byte *pRGB, int cx )
{
	if (ppColor == NULL || pRGB == NULL)
		return;

	/*if(m_pLookupTable != 0)
	{
		//Extended convertor with usage lookup table
		ASSERT(m_sizeLookupTable.cy >= 3);
		ASSERT(m_sizeLookupTable.cx >= 256);

		int CX = m_sizeLookupTable.cx;

		for( long n = 0, c = 0; n < cx; n++ )
		{
			pRGB[c++] = m_pLookupTable[2*CX + ColorAsByte(ppColor[0][n])];
			pRGB[c++] = m_pLookupTable[1*CX + ColorAsByte(ppColor[0][n])];
			pRGB[c++] = m_pLookupTable[0*CX + ColorAsByte(ppColor[0][n])];
		}
	}
	else
	*/
	{
		for( long n = 0, c = 0; n < cx; n++ )
		{
			pRGB[c++] = ColorAsByte( ppColor[0][n] );
			pRGB[c++] = ColorAsByte( ppColor[0][n] );
			pRGB[c++] = ColorAsByte( ppColor[0][n] );		
		}
	}
}

void CColorConvertorGRAY::ConvertGrayToImage( byte *pGray, color **ppColor, int cx )
{
	if (ppColor == NULL || pGray == NULL)
		return;
	for( long n = 0; n < cx; n++ )
	{
		ppColor[0][n] = ByteAsColor( pGray[n] );
	}
}

void CColorConvertorGRAY::ConvertImageToGray( color **ppColor, byte *pGray, int cx )
{
	if (ppColor == NULL || pGray == NULL)
		return;

	/*if(m_pLookupTable != 0)
	{
		//Extended convertor with usage lookup table
		ASSERT(m_sizeLookupTable.cy >= 3);
		ASSERT(m_sizeLookupTable.cx >= 256);

		for( long n = 0; n < cx; n++ )
		{
			pGray[n] = m_pLookupTable[ColorAsByte(ppColor[0][n])];
		}
	}
	else*/
	{
		for( long n = 0; n < cx; n++ )
		{
			pGray[n] = ColorAsByte( ppColor[0][n] );
		}
	}
}

void CColorConvertorGRAY::GetConvertorIcon( HICON *phIcon )
{
	if(phIcon)
		*phIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_ICON_GRAY));
}

void CColorConvertorGRAY::ConvertLABToImage( double *pLab, color **ppColor, int cx )
{
	color	*p0, *p1, *p2;
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
	}
}

void CColorConvertorGRAY::ConvertImageToLAB( color **ppColor, double *pLab, int cx )
{
	color	*p0, *p1, *p2;
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

}

void CColorConvertorGRAY::GetPaneColor(int numPane, DWORD* pdwColor)
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

void CColorConvertorGRAY::GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues)
{
	if(pdwColorValues == 0) return;
	switch(numPane)
	{
	case 0:
		*pdwColorValues =  RGB(NotUsed, NotUsed, NotUsed); 
		break;
	default:
		*pdwColorValues = 0;
	}
}
