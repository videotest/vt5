#ifndef __image5_h__
#define __image5_h__

#include "ColorConvertorParams.h"

typedef unsigned short color;
typedef byte	mask;

#define ColorAsByte( clr )	byte( ( clr ) >> 8 )
#define ByteAsColor( b )	color( ((color)( b ))<< 8 )

#define colorMax ((color)-1)

enum ContourFlags 
{
	cfInternal = 1, 
	cfExternal = 2, 
	cfEmpty = 0
};

enum ImageFlags
{
	ifVirtual = 1,
	ifMasked = 2,
	ifContoured = 4
};

struct ContourPoint
{
	short	x;
	short	y;
};

struct Contour
{
	long	nContourSize;
	long	nAllocatedSize;
	long	lFlags;
	long	lReserved;

	ContourPoint	*ppoints;
};

enum PaneFlags
{
	pfOrdinary,
	pfCycled,
	pfGray
};

interface IImage : public IUnknown
{
	com_call CreateImage( int cx, int cy, BSTR bstrCCNamed, int nPaneNum ) = 0;
	com_call FreeImage() = 0;

	com_call GetColorConvertor( IUnknown **ppunk ) = 0;

	com_call GetRow( int nRow, int nPane, color **ppcolor ) = 0;
	com_call GetSize( int *pnCX, int *pnCY ) = 0;
};

interface IImage2 : public IImage
{
	com_call CreateVirtual( RECT rect ) = 0;
	com_call InitRowMasks() = 0;
	com_call GetRowMask( int nRow, BYTE **ppByte ) = 0;

	com_call GetOffset( POINT *pt ) = 0;
	com_call SetOffset( POINT pt, BOOL bMoveImage ) = 0;
	com_call GetImageFlags( DWORD *pdwFlags ) = 0;
};

interface IImage3 : public IImage2
{
	com_call InitContours() = 0;
	com_call FreeContours() = 0;

	com_call GetContoursCount( int *piCount ) = 0;
	com_call GetContour( int nPos, Contour **ppContour ) = 0;
	com_call SetContour( int nPos, Contour *pContour ) = 0;
	com_call AddContour( Contour *pContour ) = 0;
};

interface IImage4 : public IImage3
{
	com_call AddPane(int nPane ) = 0;
	com_call RemovePane( int nPane ) = 0;
	com_call GetPanesCount(int *nSz  ) = 0;
};


enum ControlFrameFlags
{
	fcfNone = 0,
	fcfRotate = 1,
	fcfResize = 2
};

enum ImageShowFlags //every of 9 first flags correcpond to image view type
{
	isfFilledImageExceptObjects	 = 1<<0,	//source image filled except object areas
	isfFilledOnlyObjects	     = 1<<1,	//source image with filled object areas
	isfContours		             = 1<<2,	//source image with contours of objects
	isfSourceImage               = 1<<3,	//only main image
	isfPseudoImage	             = 1<<4,    //image filled by one color and objects by another
	isfBinaryFore				 = 1<<5,	//source image in the back of binary and fore of binary filled by color
	isfBinaryBack				 = 1<<6,	//source image in the fore of binary and back of binary filled by color
	isfBinary					 = 1<<7,	//fore and back of binary filled by appropriate colors
	isfBinaryContour			 = 1<<8,	//source image with contours of binary fore
};

interface IImageView : public IUnknown
{
	com_call GetActiveImage( IUnknown **ppunkImage, IUnknown **ppunkSelection ) = 0;
	com_call EnableControlFrame( DWORD dwFrameControlsFlags ) = 0;
	com_call GetImageShowFlags( DWORD *pdwFlags ) = 0;
	com_call SetSelectionMode( long lSelectType ) = 0;
	com_call GetSelectionMode( long *plSelectType ) = 0;
	com_call SetImageShowFlags( DWORD dwFlags ) = 0;
	com_call SetActiveImageDuringPreview( IUnknown *punkImage, IUnknown *punkSelection ) = 0;
};

interface IImageView2 : public IImageView
{
	com_call GetActiveImage2( IUnknown **ppunkImage, IUnknown **ppunkSelection, BOOL bFromPreview ) = 0;
};

interface IColorConvertor : public IUnknown
{
	// [vanek] - 19.01.2004: корректнее спрашивать у image о количестве пан
	com_call GetColorPanesDefCount( int *pnCount ) = 0;
	com_call ConvertDIBToImage( byte *pRGB, color **ppColor, int cx, BOOL bColor ) = 0;
	com_call ConvertImageToDIB( color **ppColor, byte *pRGB, int cx, BOOL bColor ) = 0;
};

interface IColorConvertor2 : public IColorConvertor
{
	com_call GetCnvName(BSTR *pbstrName) = 0;
	com_call GetPaneName(int numPane, BSTR *pbstrName) = 0;
	com_call GetPaneFlags(int numPane, DWORD *pdwFlags) = 0;
};

interface IColorConvertor3 : public IColorConvertor2
{
	com_call ConvertLABToImage( double *pLab, color **ppColor, int cx ) = 0;
	com_call ConvertImageToLAB( color **ppColor, double *pLab, int cx ) = 0;
	com_call GetConvertorIcon( HICON *phIcon ) = 0;
};

enum ColorConvertorFlags
{
	ccfLabCompatible = 1	//if flag set than from this color space can be performed convertion to another lab compatible color space
};

interface IColorConvertor4 : public IColorConvertor3
{
	//represent pane color (in histogramm for ex)
	com_call GetPaneColor(int numPane, DWORD* pdwColor) = 0; 
	//correspond panes values for build specific gradient fill
	com_call GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues) = 0; 
	com_call GetConvertorFlags(DWORD* pdwFlags) = 0;
};

// [vanek] - 19.01.2004
interface IColorConvertor5 : public IColorConvertor4
{
    com_call ConvertDIBToImageEx( byte *pRGB, color **ppColor, int cx, BOOL bColor, int nPanes ) = 0;
	com_call ConvertImageToDIBEx( color **ppColor, byte *pRGB, int cx, BOOL bColor, int nPanes ) = 0;
    com_call ConvertLABToImageEx( double *pLab, color **ppColor, int cx, int nPanes ) = 0;
	com_call ConvertImageToLABEx( color **ppColor, double *pLab, int cx, int nPanes ) = 0;
};

struct INIT_OBJECT_DATA
{
	int m_nSize;
	int m_nPane;
};

interface IColorConvertor6 : public IColorConvertor5
{
	// Caller must allocate array of IImage4::GetPanesCount() bools.
	// Color convertor can either initialize array be true and false and return S_OK
	// or do not change array and return E_NOTIMPL.
	com_call GetUsedPanes(IUnknown *punkImage, IUnknown *punkView, bool *pbUsedPanes, int nSize) = 0;
	// How to show color pickers in editor actions:
	// 0 - grayscale
	// 1 - color
	// 2 - 256 colors whith palette. Palette can be obtained by GetPalette.
	com_call GetEditMode(IUnknown *punkImage, int *pnColorMode) = 0;
	// Obtain palette for color picker. pColors points to array of 256 COLORREFs.
	com_call GetPalette(IUnknown *punkImage, IUnknown *punkView, COLORREF *pColors, int nColors) = 0;
	// Thresholding support. After object detection thresholding will call this method
	// for additional initialization (named data, etc.).
	com_call InitObjectAfterThresholding(IUnknown *punkNewObjImg, IUnknown *punkBaseImg, INIT_OBJECT_DATA *pData) = 0;
	// If nPane==INT_MAX, then pane settings will be given from punkParams
	com_call ConvertImageToDIB2( color **ppColor, byte *pRGB, int cx, BOOL bColor, int nPane, IUnknown *punkImage, IUnknown *punkParams) = 0;
};

interface IColorConvertorLookup : public IUnknown
{
	com_call SetLookupTable(byte* pTable, int cx, int cy) = 0;
	com_call KillLookup() = 0;
};


enum ConvertImageToDIBRectFlags
{
	cidrFillBackByColor = 1,
	cidrUseImageOffset = 2,
	cidrFillBodyByColor = 4,
	cidrHilight = 8,
	cidrIgnoreMask = 16,
	cidrExportImage = 32,
	cidrTranspMask = 128+64,
	cidrTranspOpaque = 0,
	cidrTranspTransparent = 64,
	cidrTranspSmoothTransition = 128,
	cidrTranspNegative = 128+64,
};

interface IColorConvertorEx : public IUnknown
{
	com_call ConvertImageToDIBRect( 
		BITMAPINFOHEADER *pbi, 
		BYTE *pdibBits, 
		POINT ptBmpOfs, 
		IImage2 *punkImage, 
		RECT rectDest, 
		POINT pointImageOffest, 
		double fZoom, POINT ptScroll, 
		DWORD dwFillColor, 
		DWORD dwFlags,
		IUnknown *ppunkParams=0 //параметры View - NamedPropBag
		) = 0;
};

interface IDistanceMap : public IUnknown
{
	com_call CreateNew(int cx, int cy) = 0;
	com_call Destroy() = 0;
	com_call GetRow(int nRow, WORD **ppRow) = 0;
	com_call GetCX(int *pcx) = 0;
	com_call GetCY(int *pcy) = 0;
	com_call AddRect(RECT rc) = 0;
};

interface ITransparencyMap : public IUnknown
{
	com_call CreateNew( int cx, int cy ) = 0;
	com_call Free() = 0;
	com_call GetRow( int nRow, byte **pprow ) = 0;
	com_call GetSize( int *pnCX, int *pnCY ) = 0;
};

interface IColorConvertorEx2 : public IColorConvertorEx
{
	com_call ConvertImageToDIBRect2( 
		BITMAPINFOHEADER *pbi, 
		BYTE *pdibBits,
		WORD *pDistBits,
		POINT ptBmpOfs, 
		IImage2 *punkImage, 
		RECT rectDest, 
		POINT pointImageOffest, 
		double fZoom, POINT ptScroll, 
		DWORD dwFillColor, 
		DWORD dwFlags,
		IDistanceMap *pDistMap,
		IUnknown *ppunkParams=0 //параметры View - NamedPropBag
		) = 0;
};

declare_interface( IImage, "19C18611-43F4-11d3-A611-0090275995FE" )
declare_interface( IImage2, "27FA84B1-548C-11d3-A62A-0090275995FE" )
declare_interface( IImage3, "A04285D1-56E6-11d3-A631-0090275995FE" )
declare_interface( IImage4, "47842811-F63D-459d-B32E-40A8BC7FC49A" )
declare_interface( IImageView, "40EDCD91-53C8-11d3-A626-0090275995FE" )
declare_interface( IImageView2, "C10895EE-A827-4be9-9A01-20377A81C766" )
declare_interface( IColorConvertor, "19C18613-43F4-11d3-A611-0090275995FE" )
declare_interface( IColorConvertor2, "79624890-66CC-11d3-A4EA-0000B493A187" )
declare_interface( IColorConvertor3, "191935B1-B141-11d3-A6C6-0090275995FE" )
declare_interface( IColorConvertor4, "F1173231-FA44-11d3-A0C2-0000B493A187" )
declare_interface( IColorConvertor5, "316E63DC-AA24-4d34-B224-D1362E011BCD" )
declare_interface( IColorConvertor6, "2164B75E-B786-4389-92F6-4110970D2DE6" )
declare_interface( IColorConvertorEx, "B138C161-ABD1-11d3-A6BF-0090275995FE" )
declare_interface( IColorConvertorEx2, "AEFE7B04-7EBE-4be1-8812-3CACE0DEDD40" )
declare_interface( IColorConvertorManager, "6A1FC3C9-6749-11D3-A4EB-0000B493A187" )
declare_interface( IColorConvertorLookup, "464BAC0D-C1DE-463b-B825-C0658E049B35" )
declare_interface( ITransparencyMap, "D3D9B439-6101-47d0-93CA-1619D153F741" )
declare_interface( IDistanceMap, "0256F68A-8B89-4401-99B9-37FD3AE4FEF2" )

#endif // __image5_h__
