#ifndef __color_h__
#define __color_h__

#include "histint.h"

class CColorConvertorRGB : public CColorConvertorBase			   
{
	DECLARE_DYNCREATE(CColorConvertorRGB);
	GUARD_DECLARE_OLECREATE(CColorConvertorRGB);

	DECLARE_INTERFACE_MAP();
protected:
	CColorConvertorRGB();
	virtual ~CColorConvertorRGB();

	virtual void ConvertRGBToImage( byte *pRGB, color **ppColor, int cx );
	virtual void ConvertImageToRGB( color **ppColor, byte *pRGB, int cx );
	virtual void ConvertGrayToImage( byte *pGray, color **ppColor, int cx );
	virtual void ConvertImageToGray( color **ppColor, byte *pGray, int cx );
	virtual void ConvertLABToImage( double *pLab, color **ppColor, int cx );
	virtual void ConvertImageToLAB( color **ppColor, double *pLab, int cx );
	virtual void GetConvertorIcon( HICON *phIcon );
	virtual void GetPaneColor(int numPane, DWORD* pdwColor);
	virtual void GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues);

	virtual const char *GetCnvName();
	virtual int GetColorPanesCount();
	virtual const char *GetPaneName(int numPane);
	virtual DWORD GetPaneFlags(int numPane);

	virtual const char *GetPaneShortName( int numPane );
	virtual color ConvertToHumanValue( color colorInternal, int numPane );
	
	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx2)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
		com_call ConvertImageToDIBRect2( BITMAPINFOHEADER *pbi, BYTE *pdibBits, WORD *pDistBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IDistanceMap *pDistMap, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);
	BEGIN_INTERFACE_PART(Hist, IProvideHistColors)
		com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve );
	END_INTERFACE_PART(Hist);
};

class CColorConvertorYUV : public CColorConvertorBase
{
	DECLARE_DYNCREATE(CColorConvertorYUV);
	GUARD_DECLARE_OLECREATE(CColorConvertorYUV);

	DECLARE_INTERFACE_MAP();
protected:
	CColorConvertorYUV();
	virtual ~CColorConvertorYUV();

	virtual void ConvertRGBToImage( byte *pRGB, color **ppColor, int cx );
	virtual void ConvertImageToRGB( color **ppColor, byte *pRGB, int cx );
	virtual void ConvertGrayToImage( byte *pGray, color **ppColor, int cx );
	virtual void ConvertImageToGray( color **ppColor, byte *pGray, int cx );
	virtual void ConvertLABToImage( double *pLab, color **ppColor, int cx );
	virtual void ConvertImageToLAB( color **ppColor, double *pLab, int cx );
	virtual void GetConvertorIcon( HICON *phIcon );
	virtual void GetPaneColor(int numPane, DWORD* pdwColor);
	virtual void GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues);

	virtual const char *GetCnvName();
	virtual int GetColorPanesCount();
	virtual const char *GetPaneName(int numPane);
	virtual DWORD GetPaneFlags(int numPane);

	virtual const char *GetPaneShortName( int numPane );
	virtual color ConvertToHumanValue( color colorInternal, int numPane );

	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);
	BEGIN_INTERFACE_PART(Hist, IProvideHistColors)
		com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve );
	END_INTERFACE_PART(Hist);
};

class CColorConvertorYIQ : public CColorConvertorBase
{
	DECLARE_DYNCREATE(CColorConvertorYIQ);
	GUARD_DECLARE_OLECREATE(CColorConvertorYIQ);

	DECLARE_INTERFACE_MAP();
protected:
	CColorConvertorYIQ();
	virtual ~CColorConvertorYIQ();

	virtual void ConvertRGBToImage( byte *pRGB, color **ppColor, int cx );
	virtual void ConvertImageToRGB( color **ppColor, byte *pRGB, int cx );
	virtual void ConvertGrayToImage( byte *pGray, color **ppColor, int cx );
	virtual void ConvertImageToGray( color **ppColor, byte *pGray, int cx );
	virtual void ConvertLABToImage( double *pLab, color **ppColor, int cx );
	virtual void ConvertImageToLAB( color **ppColor, double *pLab, int cx );
	virtual void GetConvertorIcon( HICON *phIcon );
	virtual void GetPaneColor(int numPane, DWORD* pdwColor);
	virtual void GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues);

	virtual const char *GetCnvName();
	virtual int GetColorPanesCount();
	virtual const char *GetPaneName(int numPane);
	virtual DWORD GetPaneFlags(int numPane);

	virtual const char *GetPaneShortName( int numPane );
	virtual color ConvertToHumanValue( color colorInternal, int numPane );

	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);
	BEGIN_INTERFACE_PART(Hist, IProvideHistColors)
		com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve );
	END_INTERFACE_PART(Hist);
};


class CColorConvertorHSB : public CColorConvertorBase
{
	DECLARE_DYNCREATE(CColorConvertorHSB);
	GUARD_DECLARE_OLECREATE(CColorConvertorHSB);
protected:
	CColorConvertorHSB();
	virtual ~CColorConvertorHSB();

	DECLARE_INTERFACE_MAP();

	virtual void ConvertRGBToImage( byte *pRGB, color **ppColor, int cx );
	virtual void ConvertImageToRGB( color **ppColor, byte *pRGB, int cx );
	virtual void ConvertGrayToImage( byte *pGray, color **ppColor, int cx );
	virtual void ConvertImageToGray( color **ppColor, byte *pGray, int cx );
	virtual void ConvertLABToImage( double *pLab, color **ppColor, int cx );
	virtual void ConvertImageToLAB( color **ppColor, double *pLab, int cx );
	virtual void GetConvertorIcon( HICON *phIcon );
	virtual void GetPaneColor(int numPane, DWORD* pdwColor);
	virtual void GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues);

	virtual const char *GetCnvName();
	virtual int GetColorPanesCount();
	virtual const char *GetPaneName(int numPane);
	virtual DWORD GetPaneFlags(int numPane);

	virtual const char *GetPaneShortName( int numPane );
	virtual color ConvertToHumanValue( color colorInternal, int numPane );

	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);
	BEGIN_INTERFACE_PART(Hist, IProvideHistColors)
		com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve );
	END_INTERFACE_PART(Hist);
};

class CColorConvertorGRAY : public CColorConvertorBase
{
	DECLARE_DYNCREATE(CColorConvertorGRAY);
	GUARD_DECLARE_OLECREATE(CColorConvertorGRAY);

	DECLARE_INTERFACE_MAP();
protected:
	CColorConvertorGRAY();
	virtual ~CColorConvertorGRAY();

	virtual void ConvertRGBToImage( byte *pRGB, color **ppColor, int cx );
	virtual void ConvertImageToRGB( color **ppColor, byte *pRGB, int cx );
	virtual void ConvertGrayToImage( byte *pGray, color **ppColor, int cx );
	virtual void ConvertImageToGray( color **ppColor, byte *pGray, int cx );
	virtual void ConvertLABToImage( double *pLab, color **ppColor, int cx );
	virtual void ConvertImageToLAB( color **ppColor, double *pLab, int cx );
	virtual void GetConvertorIcon( HICON *phIcon );
	virtual void GetPaneColor(int numPane, DWORD* pdwColor);
	virtual void GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues);

	virtual const char *GetCnvName();
	virtual int GetColorPanesCount();
	virtual const char *GetPaneName(int numPane);
	virtual DWORD GetPaneFlags(int numPane);

	virtual const char *GetPaneShortName( int numPane );
	virtual color ConvertToHumanValue( color colorInternal, int numPane );

	
	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx2)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
		com_call ConvertImageToDIBRect2( BITMAPINFOHEADER *pbi, BYTE *pdibBits, WORD *pDistBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IDistanceMap *pDistMap, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);
	BEGIN_INTERFACE_PART(Hist, IProvideHistColors)
		com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve );
	END_INTERFACE_PART(Hist);
};


#endif // __color_h__