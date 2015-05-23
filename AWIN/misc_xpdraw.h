#ifndef __misc_xpdraw_h__
#define __misc_xpdraw_h__

#include "uxtheme.h"
#include "tmschema.h"


class xpdraw
{
public:
	xpdraw();
	~xpdraw();
public:
	static xpdraw *ptr();
	bool	is_xp;

	HMODULE	m_hdll;
public:
	HTHEME (__stdcall *OpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
	HRESULT (__stdcall *CloseThemeData)(HTHEME hTheme);
	HRESULT (__stdcall *DrawThemeBackground)(HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
	HRESULT (__stdcall *DrawThemeText)(HTHEME hTheme, HDC hdc, int iPartId, 
		int iStateId, LPCWSTR pszText, int iCharCount, DWORD dwTextFlags, 
		DWORD dwTextFlags2, const RECT *pRect);

	HRESULT (__stdcall *GetThemeBackgroundExtent)(HTHEME hTheme, OPTIONAL HDC hdc,
		int iPartId, int iStateId, const RECT *pContentRect, 
		OUT RECT *pExtentRect);
	HRESULT (__stdcall *GetThemePartSize)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
		OPTIONAL RECT *prc, enum THEMESIZE eSize, OUT SIZE *psz);
	HRESULT (__stdcall *GetThemeTextExtent)(HTHEME hTheme, HDC hdc, 
		int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
		DWORD dwTextFlags, OPTIONAL const RECT *pBoundingRect, 
		OUT RECT *pExtentRect);
	HRESULT (__stdcall *GetThemeTextMetrics)(HTHEME hTheme, OPTIONAL HDC hdc, 
		int iPartId, int iStateId, OUT TEXTMETRIC* ptm);
	HRESULT (__stdcall *GetThemeBackgroundRegion)(HTHEME hTheme, OPTIONAL HDC hdc,  
		int iPartId, int iStateId, const RECT *pRect, OUT HRGN *pRegion);
	HRESULT (__stdcall *HitTestThemeBackground)(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
		int iStateId, DWORD dwOptions, const RECT *pRect, OPTIONAL HRGN hrgn, 
		POINT ptTest, OUT WORD *pwHitTestCode);
	HRESULT (__stdcall *DrawThemeEdge)(HTHEME hTheme, HDC hdc, int iPartId, int iStateId, 
						   const RECT *pDestRect, UINT uEdge, UINT uFlags, OPTIONAL OUT RECT *pContentRect);
	HRESULT (__stdcall *DrawThemeIcon)(HTHEME hTheme, HDC hdc, int iPartId, 
		int iStateId, const RECT *pRect, HIMAGELIST himl, int iImageIndex);
	BOOL (__stdcall *IsThemePartDefined)(HTHEME hTheme, int iPartId, 
		int iStateId);
	BOOL (__stdcall *IsThemeBackgroundPartiallyTransparent)(HTHEME hTheme, 
		int iPartId, int iStateId);
	HRESULT (__stdcall *GetThemeColor)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT COLORREF *pColor);
	HRESULT (__stdcall *GetThemeMetric)(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
		int iStateId, int iPropId, OUT int *piVal);
	HRESULT (__stdcall *GetThemeString)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT LPWSTR pszBuff, int cchMaxBuffChars);
	HRESULT (__stdcall *GetThemeBool)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT BOOL *pfVal);
	HRESULT (__stdcall *GetThemeInt)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT int *piVal);
	HRESULT (__stdcall *GetThemeEnumValue)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT int *piVal);
	HRESULT (__stdcall *GetThemePosition)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT POINT *pPoint);
	HRESULT (__stdcall *GetThemeFont)(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
		int iStateId, int iPropId, OUT LOGFONT *pFont);
	HRESULT (__stdcall *GetThemeRect)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT RECT *pRect);
	HRESULT (__stdcall *GetThemeMargins)(HTHEME hTheme, OPTIONAL HDC hdc, int iPartId, 
		int iStateId, int iPropId, OPTIONAL RECT *prc, OUT MARGINS *pMargins);
	HRESULT (__stdcall *GetThemeIntList)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT INTLIST *pIntList);
	HRESULT (__stdcall *GetThemePropertyOrigin)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT enum PROPERTYORIGIN *pOrigin);
	HRESULT (__stdcall *SetWindowTheme)(HWND hwnd, LPCWSTR pszSubAppName, 
		LPCWSTR pszSubIdList);
	HRESULT (__stdcall *GetThemeFilename)(HTHEME hTheme, int iPartId, 
		int iStateId, int iPropId, OUT LPWSTR pszThemeFileName, int cchMaxBuffChars);
	COLORREF (__stdcall *GetThemeSysColor)(HTHEME hTheme, int iColorId);
	HBRUSH (__stdcall *GetThemeSysColorBrush)(HTHEME hTheme, int iColorId);
	BOOL (__stdcall *GetThemeSysBool)(HTHEME hTheme, int iBoolId);
	int (__stdcall *GetThemeSysSize)(HTHEME hTheme, int iSizeId);
	HRESULT (__stdcall *GetThemeSysFont)(HTHEME hTheme, int iFontId, OUT LOGFONT *plf);
	HRESULT (__stdcall *GetThemeSysString)(HTHEME hTheme, int iStringId, 
		OUT LPWSTR pszStringBuff, int cchMaxStringChars);
	HRESULT (__stdcall *GetThemeSysInt)(HTHEME hTheme, int iIntId, int *piValue);
	BOOL (__stdcall *IsThemeActive)();
	BOOL (__stdcall *IsAppThemed)();
	HTHEME (__stdcall *GetWindowTheme)(HWND hwnd);
	HRESULT (__stdcall *EnableThemeDialogTexture)(HWND hwnd, DWORD dwFlags);
	BOOL (__stdcall *IsThemeDialogTextureEnabled)(HWND hwnd);
	DWORD (__stdcall *GetThemeAppProperties)();
	void (__stdcall *SetThemeAppProperties)(DWORD dwFlags);
	HRESULT (__stdcall *GetCurrentThemeName)(
		OUT LPWSTR pszThemeFileName, int cchMaxNameChars, 
		OUT OPTIONAL LPWSTR pszColorBuff, int cchMaxColorChars,
		OUT OPTIONAL LPWSTR pszSizeBuff, int cchMaxSizeChars);
	HRESULT (__stdcall *GetThemeDocumentationProperty)(LPCWSTR pszThemeName,
		LPCWSTR pszPropertyName, OUT LPWSTR pszValueBuff, int cchMaxValChars);
	HRESULT (__stdcall *DrawThemeParentBackground)(HWND hwnd, HDC hdc, OPTIONAL RECT* prc);
	HRESULT (__stdcall *EnableTheming)(BOOL fEnable);
};

inline xpdraw *xpdraw::ptr()
{
	static xpdraw	xpd;
	return &xpd;
}

inline 	xpdraw::xpdraw()
{
	is_xp = false;

	unsigned	dw = GetVersion();
	if( (dw & 0x80000000)==0 )	//NT
	{
		byte	hi = dw & 0xff;
		byte	lo = (dw & 0xff00)>>8;

		if( hi > 5 || (hi==5&&lo>=1 ))
			is_xp = true;
	}

	m_hdll = ::LoadLibrary( _T("uxtheme.dll") );

	(FARPROC&)OpenThemeData=GetProcAddress( m_hdll, _T("OpenThemeData") );
	(FARPROC&)CloseThemeData = ::GetProcAddress( m_hdll, _T("CloseThemeData") );
	(FARPROC&)DrawThemeBackground = ::GetProcAddress( m_hdll, _T("DrawThemeBackground") );
	(FARPROC&)DrawThemeText = ::GetProcAddress( m_hdll, _T("DrawThemeText") );
	(FARPROC&)GetThemeBackgroundExtent = ::GetProcAddress( m_hdll, _T("GetThemeBackgroundExtent") );
	(FARPROC&)GetThemePartSize = ::GetProcAddress( m_hdll, _T("GetThemePartSize") );
	(FARPROC&)GetThemeTextExtent = ::GetProcAddress( m_hdll, _T("GetThemeTextExtent") );
	(FARPROC&)GetThemeTextMetrics = ::GetProcAddress( m_hdll, _T("GetThemeTextMetrics") );
	(FARPROC&)GetThemeBackgroundRegion = ::GetProcAddress( m_hdll, _T("GetThemeBackgroundRegion") );
	(FARPROC&)HitTestThemeBackground = ::GetProcAddress( m_hdll, _T("HitTestThemeBackground") );
	(FARPROC&)DrawThemeEdge = ::GetProcAddress( m_hdll, _T("DrawThemeEdge") );
	(FARPROC&)DrawThemeIcon = ::GetProcAddress( m_hdll, _T("DrawThemeIcon") );
	(FARPROC&)IsThemePartDefined = ::GetProcAddress( m_hdll, _T("IsThemePartDefined") );
	(FARPROC&)IsThemeBackgroundPartiallyTransparent = ::GetProcAddress( m_hdll, _T("IsThemeBackgroundPartiallyTransparent") );
	(FARPROC&)GetThemeColor = ::GetProcAddress( m_hdll, _T("GetThemeColor") );
	(FARPROC&)GetThemeMetric = ::GetProcAddress( m_hdll, _T("GetThemeMetric") );
	(FARPROC&)GetThemeString = ::GetProcAddress( m_hdll, _T("GetThemeString") );
	(FARPROC&)GetThemeBool = ::GetProcAddress( m_hdll, _T("GetThemeBool") );
	(FARPROC&)GetThemeInt = ::GetProcAddress( m_hdll, _T("GetThemeInt") );
	(FARPROC&)GetThemeEnumValue = ::GetProcAddress( m_hdll, _T("GetThemeEnumValue") );
	(FARPROC&)GetThemePosition = ::GetProcAddress( m_hdll, _T("GetThemePosition") );
	(FARPROC&)GetThemeFont = ::GetProcAddress( m_hdll, _T("GetThemeFont") );
	(FARPROC&)GetThemeRect = ::GetProcAddress( m_hdll, _T("GetThemeRect") );
	(FARPROC&)GetThemeMargins = ::GetProcAddress( m_hdll, _T("GetThemeMargins") );
	(FARPROC&)GetThemeIntList = ::GetProcAddress( m_hdll, _T("GetThemeIntList") );
	(FARPROC&)GetThemePropertyOrigin = ::GetProcAddress( m_hdll, _T("GetThemePropertyOrigin") );
	(FARPROC&)SetWindowTheme = ::GetProcAddress( m_hdll, _T("SetWindowTheme") );
	(FARPROC&)GetThemeFilename = ::GetProcAddress( m_hdll, _T("GetThemeFilename") );
	(FARPROC&)GetThemeSysColor = ::GetProcAddress( m_hdll, _T("GetThemeSysColor") );
	(FARPROC&)GetThemeSysColorBrush = ::GetProcAddress( m_hdll, _T("GetThemeSysColorBrush") );
	(FARPROC&)GetThemeSysBool = ::GetProcAddress( m_hdll, _T("GetThemeSysBool") );
	(FARPROC&)GetThemeSysSize = ::GetProcAddress( m_hdll, _T("GetThemeSysSize") );
	(FARPROC&)GetThemeSysFont = ::GetProcAddress( m_hdll, _T("GetThemeSysFont") );
	(FARPROC&)GetThemeSysString = ::GetProcAddress( m_hdll, _T("GetThemeSysString") );
	(FARPROC&)GetThemeSysInt = ::GetProcAddress( m_hdll, _T("GetThemeSysInt") );
	(FARPROC&)IsThemeActive = ::GetProcAddress( m_hdll, _T("IsThemeActive") );
	(FARPROC&)IsAppThemed = ::GetProcAddress( m_hdll, _T("IsAppThemed") );
	(FARPROC&)GetWindowTheme = ::GetProcAddress( m_hdll, _T("GetWindowTheme") );
	(FARPROC&)EnableThemeDialogTexture = ::GetProcAddress( m_hdll, _T("EnableThemeDialogTexture") );
	(FARPROC&)IsThemeDialogTextureEnabled = ::GetProcAddress( m_hdll, _T("IsThemeDialogTextureEnabled") );
	(FARPROC&)GetThemeAppProperties = ::GetProcAddress( m_hdll, _T("GetThemeAppProperties") );
	(FARPROC&)SetThemeAppProperties = ::GetProcAddress( m_hdll, _T("SetThemeAppProperties") );
	(FARPROC&)GetCurrentThemeName = ::GetProcAddress( m_hdll, _T("GetCurrentThemeName") );
	(FARPROC&)GetThemeDocumentationProperty = ::GetProcAddress( m_hdll, _T("GetThemeDocumentationProperty") );
	(FARPROC&)DrawThemeParentBackground = ::GetProcAddress( m_hdll, _T("DrawThemeParentBackground") );
	(FARPROC&)EnableTheming = ::GetProcAddress( m_hdll, _T("EnableTheming") );

	if( !OpenThemeData || !CloseThemeData || !DrawThemeBackground ||
		!DrawThemeText||!GetThemeBackgroundExtent||!GetThemePartSize ||
		!GetThemeTextExtent || !GetThemeTextMetrics || !GetThemeBackgroundRegion ||
		!HitTestThemeBackground || !DrawThemeEdge || !DrawThemeIcon ||
		!IsThemePartDefined || !IsThemeBackgroundPartiallyTransparent ||
		!GetThemeColor || !GetThemeMetric || !GetThemeString || !GetThemeBool ||
		!GetThemeInt || !GetThemeEnumValue || !GetThemePosition || !GetThemeFont ||
		!GetThemeRect || !GetThemeMargins || !GetThemeIntList || !GetThemePropertyOrigin ||
		!SetWindowTheme || !GetThemeFilename || !GetThemeSysColor || !GetThemeSysColorBrush ||
		!GetThemeSysBool || !GetThemeSysSize || !GetThemeSysFont || !GetThemeSysString ||
		!GetThemeSysInt || !IsThemeActive || !IsAppThemed || !GetWindowTheme ||
		!EnableThemeDialogTexture || !IsThemeDialogTextureEnabled || !GetThemeAppProperties ||
		!SetThemeAppProperties || !GetCurrentThemeName || !GetThemeDocumentationProperty ||
		!DrawThemeParentBackground || !EnableTheming )
		is_xp = false;

}

inline 	xpdraw::~xpdraw()
{
	::FreeLibrary( m_hdll );
}




#endif //__misc_xpdraw_h__