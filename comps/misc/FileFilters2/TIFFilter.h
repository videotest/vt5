#ifndef __TIFFilter_h__
#define __TIFFilter_h__
/*
#ifdef _DEBUG
// {25F96506-C016-4a15-A941-C35477704593}
static const GUID clsidTIFFilter = 
{ 0x25f96506, 0xc016, 0x4a15, { 0xa9, 0x41, 0xc3, 0x54, 0x77, 0x70, 0x45, 0x93 } };

#define pszTIFFilterProgID "FileFilters2.TIFFilterD"
#else
  */
// {BBD87325-543C-482e-8286-2EF32667336A}
static const GUID clsidTIFFilter = 
{ 0xbbd87325, 0x543c, 0x482e, { 0x82, 0x86, 0x2e, 0xf3, 0x26, 0x67, 0x33, 0x6a } };

#define pszTIFFilterProgID "FileFilters2.TIFFilter"
/*
#endif
*/

#define _Write_16_bit_TIFF

#include "FilterPropBase.h"
#include "tiffio.h" 
#include "image5.h"

#define VT5SOFTWARE "Videotest 5.0"

class CImageData
{
public:
	bool bVT5;
	_bstr_t bstrDocName;
	_bstr_t bstrPageName;
	int nPage;
	int nPanes;
	bool bPreview;
	_ptr_t2<char> szCCName;
	_ptr_t2<char> szImgDescr;
	const char *pszND;
	_bstr_t bstrSecName;
	CImageData();
	CImageData(bool bPreview, const char *pszCCName = NULL, IColorConvertor2 *pCC = NULL,
		IImage4 *pimg = NULL);
	void Read(TIFF *tif);
	void Write(TIFF *tif);
	bool IsPageOfMultipage();
};

bool IsConvertorExists(const char *pCCName);
HRESULT _StoreToNamedData(IUnknown *punk, const _bstr_t &bstrSect, const _bstr_t &bstrKey, long l);
HRESULT SafeSetValue(IUnknown *punk, const _bstr_t bstrSect, const _bstr_t bstrKey, _variant_t var);

class CTIFFilter : public CFilterPropBase,
				  public _dyncreate_t<CTIFFilter>	
{
	IUnknown *GetCGHImage();
	bool ReadBinary(IUnknown *punk, TIFF *tif, ULONG imageWidth, ULONG imageLength, ULONG LineSize);
	bool ReadImage(IUnknown *punk, TIFF *tif, ULONG imageWidth, ULONG imageLength, ULONG LineSize,
		int PhotometricInterpretation, unsigned int BitsPerSample);
	bool ReadCGHPane(IUnknown *punk, TIFF *tif, ULONG imageWidth, ULONG imageLength, ULONG LineSize,
		int PhotometricInterpretation, unsigned int BitsPerSample, const char *szDocName);
	bool WriteBinary(IUnknown *punk, TIFF *tif);
	bool WriteImage(IUnknown *punk, TIFF *tif);
	bool WriteCGHImage(IUnknown *punkImage, IUnknown *punkCC, TIFF *tif, int nCX, int nCY);
	bool WritePageHdr(IImage4 *pImage, IColorConvertor2 *pCC, TIFF *tif, int nCX, int nCY,
		int nBitsPerSample, int nSamplesPerPixel, int nPhotometicInterpretation,
		const char *pszCCName, int nPage);
	bool IsMultipage(IImage2 *pImage, IColorConvertor2 *pCC, const char *pszCCName);
	bool WriteMultipageImage(IUnknown *punkImage, IUnknown *punkCC, TIFF *tif, int nCX, int nCY, const char *pszCCName);
	IUnknownPtr FindOrCreateImage(CImageData &ImageData);
	bool ReadPageOfImage(TIFF *tif, ULONG imageWidth, ULONG imageLength,
		ULONG LineSize,int PhotometricInterpretation, unsigned int BitsPerSample,
		CImageData &ImageData);

/*protected:
	virtual bool AfterReadFile();*/

public:
	CTIFFilter();
	virtual ~CTIFFilter();

	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool _InitNew();
};


#endif