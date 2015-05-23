#ifndef __mfc_defs_h__
#define __mfc_defs_h__

#ifndef _UNICODE
#define _UNICODE_SUFFIX
#else
#define _UNICODE_SUFFIX _T("u")
#endif

#ifndef _DEBUG
#define _DEBUG_SUFFIX
#else
#define _DEBUG_SUFFIX _T("d")
#endif

#ifdef _AFXDLL
#define _STATIC_SUFFIX
#else
#define _STATIC_SUFFIX _T("s")
#endif

#define AFX_WNDCLASS(s) \
	_T("Afx") _T(s) _T("42") _STATIC_SUFFIX _UNICODE_SUFFIX _DEBUG_SUFFIX

#define AFX_WNDCOMMCTL_INTERNET_REG		0x04000	// these are new in IE4
#define AFX_WNDCOMMCTLS_REG				0x00010	// means all original Win95
#define AFX_WNDCOMMCTLSNEW_REG			0x3C000 // INTERNET|COOL|USEREX|DATE

#define AFX_WNDCONTROLBAR   AFX_WNDCLASS("ControlBar")


BOOL AFXAPI AfxEndDeferRegisterClass(LONG fToRegister);
#define AfxDeferRegisterClass(fClass) AfxEndDeferRegisterClass(fClass)

struct AUX_DATA
{
	// system metrics
	int cxVScroll, cyHScroll;
	int cxIcon, cyIcon;

	int cxBorder2, cyBorder2;

	// device metrics for screen
	int cxPixelsPerInch, cyPixelsPerInch;

	// convenient system color
	HBRUSH hbrWindowFrame;
	HBRUSH hbrBtnFace;

	// color values of system colors used for CToolBar
	COLORREF clrBtnFace, clrBtnShadow, clrBtnHilite;
	COLORREF clrBtnText, clrWindowFrame;

	// standard cursors
	HCURSOR hcurWait;
	HCURSOR hcurArrow;
	HCURSOR hcurHelp;       // cursor used in Shift+F1 help

	// special GDI objects allocated on demand
	HFONT   hStatusFont;
	HFONT   hToolTipsFont;
	HBITMAP hbmMenuDot;

	// other system information
	UINT    nWinVer;        // Major.Minor version numbers
	BOOL    bWin95;			// TRUE if Windows 95 (not NT)
	BOOL    bWin4;          // TRUE if Windows 4.0
	BOOL    bNotWin4;       // TRUE if not Windows 4.0
	BOOL    bSmCaption;     // TRUE if WS_EX_SMCAPTION is supported
	BOOL    bMarked4;       // TRUE if marked as 4.0

// Implementation
	AUX_DATA();
	~AUX_DATA();
	void UpdateSysColors();
	void UpdateSysMetrics();
};

extern AFX_DATADEF AUX_DATA afxData;

#define CX_BORDER   1
#define CY_BORDER   1


void EnableWin40Compatibility()
{
	if (afxData.bWin4)
	{
		// Later versions of Windows report "correct" scrollbar metrics
		// MFC assumes the old metrics, so they need to be adjusted.
		afxData.cxVScroll = GetSystemMetrics(SM_CXVSCROLL) + CX_BORDER;
		afxData.cyHScroll = GetSystemMetrics(SM_CYHSCROLL) + CY_BORDER;
		afxData.bMarked4 = TRUE;
	}
}

// Call this API in your InitInstance if your application is marked
// as a Windows 3.1 application.
// This is done by linking with /subsystem:windows,3.1.
void EnableWin31Compatibility()
{
	afxData.cxVScroll = GetSystemMetrics(SM_CXVSCROLL);
	afxData.cyHScroll = GetSystemMetrics(SM_CYHSCROLL);
	afxData.bMarked4 = FALSE;
}

// Initialization code
inline AUX_DATA::AUX_DATA()
{
	// Cache various target platform version information
	DWORD dwVersion = ::GetVersion();
	nWinVer = (LOBYTE(dwVersion) << 8) + HIBYTE(dwVersion);
#ifdef _WIN64
   bWin95 = FALSE;
#else
	bWin95 = (dwVersion & 0x80000000) != 0;
#endif
	bWin4 = (BYTE)dwVersion >= 4;
	bNotWin4 = 1 - bWin4;   // for convenience
	bSmCaption = bWin4;
	bMarked4 = FALSE;

	// determine various metrics based on EXE subsystem version mark
	if (bWin4)
		bMarked4 = (GetProcessVersion(0) >= 0x00040000);

	// Cached system metrics (updated in CWnd::OnWinIniChange)
	UpdateSysMetrics();

	// Cached system values (updated in CWnd::OnSysColorChange)
	hbrBtnFace = NULL;
	UpdateSysColors();

	// Standard cursors
	hcurWait = ::LoadCursor(NULL, IDC_WAIT);
	hcurArrow = ::LoadCursor(NULL, IDC_ARROW);
	ASSERT(hcurWait != NULL);
	ASSERT(hcurArrow != NULL);
	hcurHelp = NULL;    // loaded on demand

	// cxBorder2 and cyBorder are 2x borders for Win4
	cxBorder2 = bWin4 ? CX_BORDER*2 : CX_BORDER;
	cyBorder2 = bWin4 ? CY_BORDER*2 : CY_BORDER;

	// allocated on demand
	hbmMenuDot = NULL;
	hcurHelp = NULL;
}

#ifdef AFX_TERM_SEG
#pragma code_seg(AFX_TERM_SEG)
#endif

void __stdcall AfxDeleteObject(HGDIOBJ* pObject);

// Termination code
inline AUX_DATA::~AUX_DATA()
{
	// clean up objects we don't actually create
	AfxDeleteObject((HGDIOBJ*)&hbmMenuDot);
}

#ifdef AFX_CORE1_SEG
#pragma code_seg(AFX_CORE1_SEG)
#endif

inline void AUX_DATA::UpdateSysColors()
{
	clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);

	hbrBtnFace = ::GetSysColorBrush(COLOR_BTNFACE);
	ASSERT(hbrBtnFace != NULL);
	hbrWindowFrame = ::GetSysColorBrush(COLOR_WINDOWFRAME);
	ASSERT(hbrWindowFrame != NULL);
}

inline void AUX_DATA::UpdateSysMetrics()
{
	// System metrics
	cxIcon = GetSystemMetrics(SM_CXICON);
	cyIcon = GetSystemMetrics(SM_CYICON);

	// System metrics which depend on subsystem version
	if (bMarked4)
		AfxEnableWin40Compatibility();
	else
		AfxEnableWin31Compatibility();

	// Device metrics for screen
	HDC hDCScreen = GetDC(NULL);
	ASSERT(hDCScreen != NULL);
	cxPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSX);
	cyPixelsPerInch = GetDeviceCaps(hDCScreen, LOGPIXELSY);
	ReleaseDC(NULL, hDCScreen);
}

#endif//__mfc_defs_h__