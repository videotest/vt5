#pragma once

#include "histint.h"
#include "alloc.h"
#include "PropBag.h"

class CFishProps // класс для получения данных из пропбэгов
{
public:
	CFishProps(IUnknown *punkImage, IUnknown *punkParams);
	~CFishProps();
	template<class Type>
		inline bool GetLongProp(Type* pval, char *szName);
private:
	INamedPropBagPtr m_sptrBag1, m_sptrBag2, m_sptrBag3;
};

template<class Type>
inline bool CFishProps::GetLongProp(Type* pval, char *szName)
{
	return ::GetLongProp(pval, szName, m_sptrBag1, m_sptrBag2, m_sptrBag3);
}


template<class Type>
static inline bool GetLongProp(Type* pval, char *szName, INamedPropBag *pBag1)
{
	_variant_t	var;
	GetProp1(var, szName, pBag1);
	if(var.vt!=VT_EMPTY)
	{
		*pval = var.lVal;
		return true;
	}
	return false;
}

template<class Type>
static inline bool GetLongProp(Type* pval, char *szName, INamedPropBag *pBag1, INamedPropBag *pBag2)
{
	_variant_t	var;
	GetProp1(var, szName, pBag1);
	GetProp1(var, szName, pBag2);
	if(var.vt!=VT_EMPTY)
	{
		*pval = var.lVal;
		return true;
	}
	return false;
}

// потом перенести в общую часть - к INamedPropBag
static inline void GetProp1(_variant_t &var, char *szName, INamedPropBag *pBag1, VARTYPE vt=VT_I4)
{	// получить свойство из pBag1
	if(var.vt==VT_EMPTY && pBag1)
	{
		pBag1->GetProperty( _bstr_t( szName ), &var );
		if(var.vt!=VT_EMPTY && var.vt!=vt)
		{
			var.ChangeType(vt);
			pBag1->SetProperty( _bstr_t( szName ), var );
		}
	}
}

template<class Type>
static inline bool GetLongProp(Type* pval, char *szName, INamedPropBag *pBag1, INamedPropBag *pBag2, INamedPropBag *pBag3)
{
	_variant_t	var;
	GetProp1(var, szName, pBag1);
	GetProp1(var, szName, pBag2);
	GetProp1(var, szName, pBag3);
	if(var.vt!=VT_EMPTY)
	{
		*pval = (Type)var.lVal;
		return true;
	}
	return false;
}

/*
template<class Type>
static inline bool GetLongProp(Type* pval, char *szName, INamedPropBag *pBag1, INamedPropBag *pBag2)
{	// получить свойство из pBag1; если не удастся - из pBag2; если и тут не удастся - вернуть false
	// имеет смысл сделать аналогичные функции и для общения с PropBag + shell.data
	// возможно - есть смысл сделать template для разных типов
	_variant_t	var;
	if(pBag1)
	{
		pBag1->GetProperty( _bstr_t( szName ), &var );
		if( var.vt != VT_EMPTY )
		{
			*pval = var.lVal;
			return true;
		}
	}
	if(pBag2)
	{
		pBag2->GetProperty( _bstr_t( szName ), &var );
		if( var.vt != VT_EMPTY )
		{
			*pval = var.lVal;
			return true;
		}
	}
	return false;
}
*/





struct CFishData;
class CColorConvertorFISH : public CColorConvertorBase,
							public CEventListenerImpl
{
	DECLARE_DYNCREATE(CColorConvertorFISH);
	GUARD_DECLARE_OLECREATE(CColorConvertorFISH);

	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
private:
	friend struct CFishData;
	byte *m_ColorR;
	byte *m_ColorG;
	byte *m_ColorB;
	long m_nPaneColors;
	BOOL m_bShowPerc;
	long m_nComposedMethod;
	COLORREF DefCombiColor(unsigned uMask, int nPaneCount);
	CString GetColorValueString(IUnknown *punkImage, IUnknown *punkParams, int iPane, int nColor);
protected:
	CColorConvertorFISH();
	virtual ~CColorConvertorFISH();

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize ); // для EventListener

	virtual void ConvertRGBToImage( byte *pRGB, color **ppColor, int cx );
	virtual void ConvertImageToRGB( color **ppColor, byte *pRGB, int cx );
	virtual void ConvertGrayToImage( byte *pGray, color **ppColor, int cx );
	virtual void ConvertImageToGray( color **ppColor, byte *pGray, int cx );
	virtual void ConvertLABToImage( double *pLab, color **ppColor, int cx );
	virtual void ConvertImageToLAB( color **ppColor, double *pLab, int cx );
	virtual void GetConvertorIcon( HICON *phIcon );
	virtual void GetPaneColor(int numPane, DWORD* pdwColor);
	virtual void GetCorrespondPanesColorValues(int numPane, DWORD* pdwColorValues);

	virtual void ConvertRGBToImageEx( byte *pRGB, color **ppColor, int cx, int nPanes );
	virtual bool OnGetUsedPanes(IUnknown *punkImage, IUnknown *punkView, bool *pbUsedPanes, int nSize);
	virtual int  OnGetEditMode(IUnknown *punkImage) {return 2;}
	virtual bool OnGetPalette(IUnknown *punkImage, IUnknown *punkView, COLORREF *pColors, int nColors);

	virtual const char *GetCnvName();
	virtual int GetColorPanesCount();
	virtual const char *GetPaneName(int numPane);
	virtual DWORD GetPaneFlags(int numPane);

	virtual const char *GetPaneShortName( int numPane );
	virtual color ConvertToHumanValue( color colorInternal, int numPane );
	virtual CString GetPaneShortName( int numPane, IUnknown *punkImage, IUnknown *punkParams);
	virtual bool GetColorString(IUnknown *punkImage, IUnknown *punkParams, POINT pt, CString &s);
	virtual void OnInitObjectAfterThresholding(IUnknown *punkNewObjImg, IUnknown *punkBaseImg, INIT_OBJECT_DATA *pData);
	virtual void ConvertImageToRGB2( color **ppColor, byte *pRGB, int cx, int nPane, IUnknown *punkImage, IUnknown *punkParams);
	virtual void ConvertImageToGray2( color **ppColor, byte *pGray, int cx, int nPane, IUnknown *punkImage, IUnknown *punkParams);

	void GetParamsByViewAndImg(IUnknown *punkImage, IUnknown *punkView, int &nPaneCount,
		unsigned &uPanesUsed, int &iShownPane);

	HRESULT DoConvertImageToDIBRect(BITMAPINFOHEADER *pbi, BYTE *pdibBits,
		POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT pointImageOffest, double fZoom,
		POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *punkParams, int nForcePane);
	void ConvertPoint(color *arrColors, CFishData &FishData, byte &RByte, byte &GByte, byte &BByte);


	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);
	BEGIN_INTERFACE_PART(Hist, IProvideHistColors)
		com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve );
	END_INTERFACE_PART(Hist);
	BEGIN_INTERFACE_PART(FishCC, IColorConvertorFISH)
		com_call ReloadColorValue();
		com_call SetColorValue( UINT nID, DWORD dwColor );
	END_INTERFACE_PART(FishCC);
protected:
	void _reload_color_values();
	void _reload_color_values_id(UINT nID, DWORD dwColor );
	void _reload_color_values(IUnknown *punk);
	GuidKey m_guidImage;
};


struct CFishData
{
	int nPaneCount;
	int iShownPane; // composed by default
	unsigned uPanesUsed;
	int arrThresholds[32];
	int nCombos;
	_ptr_t2<unsigned long> arrComboIdx;
	_ptr_t2<byte> arrComboR,arrComboG,arrComboB;
	bool bInvert;

	void Init(CColorConvertorFISH *pCnv, IUnknown *punkImage, IUnknown *punkParams, int iPane = INT_MAX);
};
