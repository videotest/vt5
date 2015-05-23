#ifndef __complex_h__
#define __complex_h__


class CColorConvertorComplex : public CColorConvertorBase
{
	DECLARE_DYNCREATE(CColorConvertorComplex);
	GUARD_DECLARE_OLECREATE(CColorConvertorComplex);

	DECLARE_INTERFACE_MAP();
protected:
	CColorConvertorComplex();
	virtual ~CColorConvertorComplex();

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

	virtual DWORD GetConvertorFlags(){return 0;};

	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);

	inline void ConvertToComplex( color *pcolorRe, color *pcolorIm, byte *pbyte );
	inline byte ConvertToImage( color *pcolorRe, color *pcolorIm );

	byte	bytesConvertTable[256*256];
	color	colorConvertTableBack[65536];
};


#endif //__complex_h__