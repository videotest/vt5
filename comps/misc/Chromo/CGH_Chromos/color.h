#ifndef __color_h__
#define __color_h__

#include "histint.h"
#include "alloc.h"
#include "\vt5\awin\misc_list.h"

void FreeEmpty( void *ptr );
void FreeReleaseUnknown( void *ptr );
void FreeFree( void *ptr );

class CColorConvertorCGH : public CColorConvertorBase,
							public CEventListenerImpl
{
	DECLARE_DYNCREATE(CColorConvertorCGH);
	GUARD_DECLARE_OLECREATE(CColorConvertorCGH);

	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
protected:
	CColorConvertorCGH();
	virtual ~CColorConvertorCGH();

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

	virtual const char *GetCnvName();
	virtual int GetColorPanesCount();
	virtual const char *GetPaneName(int numPane);
	virtual DWORD GetPaneFlags(int numPane);

	virtual const char *GetPaneShortName( int numPane );
	virtual color ConvertToHumanValue( color colorInternal, int numPane );

	double m_fRatioMin;	//threshold для красного
	double m_fRatioMid;
	double m_fRatioMax; //threshold для зеленого
	COLORREF m_PaneColor[3]; //самые сочные цвета 0, 1, 2 пан для цветной отрисовки одиночных пан
	byte m_pal_r[256], m_pal_g[256], m_pal_b[256]; //палитра для 3 паны

	
	BEGIN_INTERFACE_PART(CnvEx, IColorConvertorEx)
		com_call ConvertImageToDIBRect( BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, IImage2 *pimage, RECT rectDest, POINT ptImageOffset, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwFlags, IUnknown *ppunkParams=0);
	END_INTERFACE_PART(CnvEx);
	BEGIN_INTERFACE_PART(Hist, IProvideHistColors)
		com_call GetHistColors( int nPaneNo, COLORREF *pcrArray, COLORREF *pcrCurve );
	END_INTERFACE_PART(Hist);
};

typedef _ptr_t2<color> color_ptr_t;

class CActionCalcRatioPane :
	public CActionBase
{
	DECLARE_DYNCREATE(CActionCalcRatioPane);
	GUARD_DECLARE_OLECREATE(CActionCalcRatioPane);

	ENABLE_UNDO();

public:
	CActionCalcRatioPane();
	virtual ~CActionCalcRatioPane();
	virtual bool Invoke();
	virtual bool IsAvaible();
	//virtual bool IsChecked();

	virtual bool DoUndo();
	virtual bool DoRedo();

private:
	IUnknown *CActionCalcRatioPane::GetContextObject(CString cName, CString cType);

protected:
	 IUnknownPtr m_undo_Image; //для undo - кого правим
	 IUnknownPtr m_undo_List;
	 long m_undo_bImagePane3; //была ли пана 3 в Image
	 long m_undo_bListPane3;  //была ли пана 3 в Object List
private:
	_list_t <IImage3Ptr> m_undoImages;
	_list_t <color *, FreeFree> m_undoData;
};

class CActionSwitchCGHPane :
	public CActionBase
{
	DECLARE_DYNCREATE(CActionSwitchCGHPane);
	GUARD_DECLARE_OLECREATE(CActionSwitchCGHPane);

public:
	CActionSwitchCGHPane();
	virtual ~CActionSwitchCGHPane();
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();

private:
	IUnknown *GetContextObject(CString cName, CString cType);
};

class CActionSwitchCGHPane0 :
	public CActionSwitchCGHPane
{
	DECLARE_DYNCREATE(CActionSwitchCGHPane0);
	GUARD_DECLARE_OLECREATE(CActionSwitchCGHPane0);

public:
	CActionSwitchCGHPane0();
	virtual ~CActionSwitchCGHPane0();
	virtual bool Invoke();
	virtual bool IsAvaible();
	virtual bool IsChecked();
protected:
	long m_lPaneToSwitch;
};

class CActionSwitchCGHPane1 :
	public CActionSwitchCGHPane0
{
	DECLARE_DYNCREATE(CActionSwitchCGHPane1);
	GUARD_DECLARE_OLECREATE(CActionSwitchCGHPane1);

public:
	CActionSwitchCGHPane1();
};

class CActionSwitchCGHPane2 :
	public CActionSwitchCGHPane0
{
	DECLARE_DYNCREATE(CActionSwitchCGHPane2);
	GUARD_DECLARE_OLECREATE(CActionSwitchCGHPane2);

public:
	CActionSwitchCGHPane2();
};

class CActionSwitchCGHPane3 :
	public CActionSwitchCGHPane0
{
	DECLARE_DYNCREATE(CActionSwitchCGHPane3);
	GUARD_DECLARE_OLECREATE(CActionSwitchCGHPane3);

public:
	CActionSwitchCGHPane3();
};

class CActionSwitchCGHPane4 :
	public CActionSwitchCGHPane0
{
	DECLARE_DYNCREATE(CActionSwitchCGHPane4);
	GUARD_DECLARE_OLECREATE(CActionSwitchCGHPane4);

public:
	CActionSwitchCGHPane4();
};



#endif // __color_h__