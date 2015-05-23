#ifndef __imagebase_h__
#define __imagebase_h__

#include "utils.h"
#include "cmnbase.h"
#include "image5.h"
#include "lookupimpl.h"
#include "imagemisc.h"

class std_dll CColorConvertorBase : public CCmdTargetEx,
							public CNamedObjectImpl,
							public CLookupImpl
{
	ENABLE_MULTYINTERFACE();
protected:
	CColorConvertorBase();
	virtual ~CColorConvertorBase();
protected:
	DECLARE_INTERFACE_MAP();

	BEGIN_INTERFACE_PART_EXPORT(Cnv, IColorConvertor6)
		com_call GetColorPanesDefCount( int *pnCount );
		com_call ConvertDIBToImage( byte *pRGB, color **ppColor, int cx, BOOL bColor );
		com_call ConvertImageToDIB( color **ppColor, byte *pRGB, int cx, BOOL bColor );
		com_call GetCnvName(BSTR *pbstrName);
		com_call GetPaneName(int numPane, BSTR *pbstrName);
		com_call GetPaneFlags(int numPane, DWORD *pdwFlags);
		com_call ConvertLABToImage( double *pLab, color **ppColor, int cx );
		com_call ConvertImageToLAB( color **ppColor, double *pLab, int cx );
		com_call GetConvertorIcon( HICON *phIcon );
		com_call GetPaneColor(int numPane, DWORD* pdwColor);
		com_call GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues);
		com_call GetConvertorFlags(DWORD* pdwFlags);
		com_call ConvertDIBToImageEx( byte *pRGB, color **ppColor, int cx, BOOL bColor, int nPanes );
		com_call ConvertImageToDIBEx( color **ppColor, byte *pRGB, int cx, BOOL bColor, int nPanes );
		com_call ConvertLABToImageEx( double *pLab, color **ppColor, int cx, int nPanes );
		com_call ConvertImageToLABEx( color **ppColor, double *pLab, int cx, int nPanes );
		com_call GetUsedPanes(IUnknown *punkImage, IUnknown *punkView, bool *pbUsedPanes, int nSize);
		com_call GetEditMode(IUnknown *punkImage, int *pnColorMode);
		com_call GetPalette(IUnknown *punkImage, IUnknown *punkView, COLORREF *pColors, int nColors);
		com_call InitObjectAfterThresholding(IUnknown *punkNewObjImg, IUnknown *punkBaseImg, INIT_OBJECT_DATA *pData);
		com_call ConvertImageToDIB2( color **ppColor, byte *pRGB, int cx, BOOL bColor, int nPane, IUnknown *punkImage, IUnknown *punkParams);
	END_INTERFACE_PART(Cnv)

	BEGIN_INTERFACE_PART_EXPORT(HumanCnv, IHumanColorConvertor2)
		com_call GetPaneShortName( int numPane, BSTR *pbstrName );
		com_call Convert( color colorInternal, int numPane, color* pcolorHuman );
		com_call GetPaneShortName2(IUnknown *punkImage, IUnknown *punkParams, int numPane, BSTR *pbstrName );
		com_call GetColorString(IUnknown *punkImage, IUnknown *punkParams, POINT pt, BSTR *pbstr);
	END_INTERFACE_PART(HumanCnv)



	virtual void ConvertRGBToImage( byte *pRGB, color **ppColor, int cx ) = 0;
	virtual void ConvertImageToRGB( color **ppColor, byte *pRGB, int cx ) = 0;
	virtual void ConvertGrayToImage( byte *pGray, color **ppColor, int cx ) = 0;
	virtual void ConvertImageToGray( color **ppColor, byte *pGray, int cx ) = 0;
	virtual void ConvertLABToImage( double *pLab, color **ppColor, int cx ) = 0;
	virtual void ConvertImageToLAB( color **ppColor, double *pLab, int cx ) = 0;
	virtual void GetConvertorIcon( HICON *phIcon ) = 0;
	virtual void GetPaneColor(int numPane, DWORD* pdwColor) = 0;
	virtual void GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues) = 0;

	virtual void ConvertRGBToImageEx( byte *pRGB, color **ppColor, int cx, int nPanes ){ ConvertRGBToImage( pRGB, ppColor, cx ); }
	virtual void ConvertImageToRGBEx( color **ppColor, byte *pRGB, int cx, int nPanes ){ ConvertImageToRGB( ppColor, pRGB, cx ); }
	virtual void ConvertGrayToImageEx( byte *pGray, color **ppColor, int cx, int nPanes ){ ConvertGrayToImage( pGray, ppColor, cx ); }
	virtual void ConvertImageToGrayEx( color **ppColor, byte *pGray, int cx, int nPanes ){ ConvertImageToGray( ppColor, pGray, cx ); }
	virtual void ConvertLABToImageEx( double *pLab, color **ppColor, int cx, int nPanes ){ ConvertLABToImage( pLab, ppColor, cx ); }
	virtual void ConvertImageToLABEx( color **ppColor, double *pLab, int cx, int nPanes ){ ConvertImageToLAB( ppColor, pLab, cx ); }
	virtual bool OnGetUsedPanes(IUnknown *punkImage, IUnknown *punkView, bool *pbUsedPanes, int nSize);
	virtual int  OnGetEditMode(IUnknown *punkImage);
	virtual bool OnGetPalette(IUnknown *punkImage, IUnknown *punkView, COLORREF *pColors, int nColors) {return false;}

	virtual const char *GetCnvName() = 0;
	virtual int GetColorPanesCount() = 0;
	virtual const char *GetPaneName(int numPane) = 0;
	virtual DWORD GetPaneFlags(int numPane) = 0;

	virtual const char *GetPaneShortName( int numPane) = 0;
	virtual CString GetPaneShortName( int numPane, IUnknown *punkImage, IUnknown *punkParams);
	virtual color ConvertToHumanValue( color colorInternal, int numPane ) = 0;
	virtual bool GetColorString(IUnknown *punkImage, IUnknown *punkParams, POINT pt, CString &s)
		{return false;}
	virtual void OnInitObjectAfterThresholding(IUnknown *punkNewObjImg, IUnknown *punkBaseImg, INIT_OBJECT_DATA *pData) {}
	virtual void ConvertImageToRGB2( color **ppColor, byte *pRGB, int cx, int nPane, IUnknown *punkImage, IUnknown *punkParams)
		{ ConvertImageToRGB( ppColor, pRGB, cx ); }
	virtual void ConvertImageToGray2( color **ppColor, byte *pGray, int cx, int nPane, IUnknown *punkImage, IUnknown *punkParams)
		{ ConvertImageToGray( ppColor, pGray, cx ); }

	virtual DWORD GetConvertorFlags(){return ccfLabCompatible;};

};

//wrapper clas for image data object
class CImageWrp;

class std_dll CClipData
{
public:
	XFORM	m_xform;
	CRgn	m_rgn;
	CRgn	m_rgnSource;

	CPoint	m_pointOffset;
	CImageWrp	*m_pimage;
public:
	CClipData();
	CClipData(const CClipData&img):m_rgn(),m_rgnSource()
	{
		Init(img.m_pimage);
	}
	CClipData& operator=(const CClipData&img){
		m_rgn.DeleteObject();
		m_rgnSource.DeleteObject();
		Init(img.m_pimage);
		return *this;
	}

	void Init( CImageWrp *pimage );
	void CalcRotateRgn( CPoint pointOffset, double fZoom, 
			CPoint	pointOffsetCenter,
			double fZoomX, double fZoomY, 
			double fRotateAngle );
};

class std_dll	CImageWrp : public sptrIImage4
{
public:
	CImageWrp( IUnknown *punk  = 0, bool bAddRef = true );

public:
	void Attach( IUnknown *punk, bool bAddRef = true );
	IUnknown *operator =(IUnknown *);


	bool CreateNew( int cx, int cy, const char *szCCName, int nPanesNum = -1 );	//create a new image
	bool CreateCompatibleImage( CImageWrp &imgSource, bool bCopyData = true, CRect rect = NORECT,  const char *szCCName = 0);		
															//create a compatible image
	IUnknown *CreateVImage( CRect rect );					//create an virtual image

	void SetOffset( const POINT pt, bool bMoveImage = false );	//offset the image rect
	void InitRowMasks();									//initialize the masks
	void InitContours();									//initialize the contours 

	void ResetContours();		//reset the contours
	void ResetMasks();			//reset the masks

public:	//drawing
	void Draw( CDC &dc, const CRect rcDest = NORECT, const CRect rcSrc = NORECT, 
			CPalette *pPal = 0 );						//draw image in specified
														//device context with specified palette
public:	//DIB interface
	bool AttachDIBBits( BITMAPINFOHEADER *pbi, byte *pbyteDIBBits = 0);	//convert data from DIB to image
	BITMAPINFOHEADER *ExtractDIBBits( CRect rc = NORECT );		//convert data from image to DIB
public:	//image attributes and data
	bool IsEmpty();		//is image empty?
	bool IsColor();		//and is t color?
	int  GetEditMode(); //0 - grayscale, 1 - true color, 2 - color with palette
	bool IsVirtual();	//is it virtual
	bool IsMasked();	//is it colour
	bool IsCountored();	//has it contours

	int GetWidth();			//image width
	int GetHeight();		//image height
	int GetColorsCount();	//counter of color per pixel
	CPoint	GetOffset();
	CRect	GetRect();

	color *GetRow( int nRow, int nPane );	//image row
	byte *GetRowMask( int nRow );			//and row mask access
	CString	GetColorConvertor();			//return ColorConvertor name


	bool PtInImage( CPoint point );		//return true if pt in image. 
public:	//contours
	int GetContoursCount();				//return the contours count
	Contour *GetContour( int nPos );	//get the specified contour

	CRgn &GetPaintRgn( IUnknown *punkZoomScrollSite );
	
protected:
	CClipData	m_clipData;
};


#endif //__imagebase_h__