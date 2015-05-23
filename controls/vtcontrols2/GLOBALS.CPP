#include "stdafx.h"
#if _MSC_VER < 1300
	#include <..\\src\\afximpl.h>
#endif
#include "globals.h"
#include "resource.h"

static const CString strOfficeFontName	= _T("Tahoma");
static const CString strDefaultFontName = _T("MS Sans Serif");
static const CString strVertFontName	= _T("Arial");

static int CALLBACK FontFamalyProcFonts (const LOGFONT FAR* lplf,
									const TEXTMETRIC FAR* /*lptm*/,
									ULONG /*ulFontType*/,
									LPARAM /*lParam*/)
{
	ASSERT (lplf != NULL);

	CString strFont = lplf->lfFaceName;
	return strFont.CollateNoCase (strOfficeFontName) == 0 ? 0 : 1;
}
/////////////////////////////////////////////////////////////////////////////
// Cached system metrics, etc

GLOBAL_DATA globalData;

// Initialization code
GLOBAL_DATA::GLOBAL_DATA()
{
	//---------------------------------------------------------
	// Cached system values (updated in CWnd::OnSysColorChange)
	//---------------------------------------------------------
	hbrBtnShadow = NULL;
	hbrBtnHilite = NULL;
	UpdateSysColors();

	m_hcurStretch = NULL;

	DEVMODE devmode;
	EnumDisplaySettings( NULL,ENUM_CURRENT_SETTINGS,&devmode);
	m_nBitsPerPixel=devmode.dmBitsPerPel;
	
	//------------------
	// Initialize fonts:
	//------------------
	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));

	//----------------------------------
	// Chek for "Tahoma" font existance:
	//----------------------------------
	CWindowDC dc (NULL);
	if (::EnumFontFamilies (dc.GetSafeHdc (), NULL, FontFamalyProcFonts, 0) == 0)
	{
		//---------------------------
		// Found! Use MS Office font!
		//---------------------------
		_tcscpy (lf.lfFaceName, strOfficeFontName);
		lf.lfHeight = 13;
	}
	else
	{
		//-----------------------------
		// Not found. Use default font:
		//-----------------------------
		_tcscpy (lf.lfFaceName, strDefaultFontName);
		lf.lfHeight = -6;
	}

	fontRegular.CreateFontIndirect (&lf);

	//------------------
	// Create bold font:
	//------------------
	lf.lfWeight = 700;
	fontBold.CreateFontIndirect (&lf);

	//----------------------
	// Create vertical font:
	//----------------------
	CFont font;
	if (font.CreateStockObject (DEFAULT_GUI_FONT))
	{
		if (font.GetLogFont (&lf) != 0)
		{
			lf.lfOrientation = 900;
			lf.lfEscapement = 900;
			_tcscpy (lf.lfFaceName, _T("Arial"));
			fontVert.CreateFontIndirect (&lf);
		}
	}

	//------------------------------------
	// Detect the kind of OS: NT or 95/98?
	//------------------------------------
	DWORD dwVersion = ::GetVersion ();
	bIsWindowsNT = (dwVersion < 0x80000000);
}
//*******************************************************************************************
GLOBAL_DATA::~GLOBAL_DATA()
{
	// cleanup standard brushes
	AfxDeleteObject((HGDIOBJ*)&hbrBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnHilite);

	// cleanup standard pens
	AfxDeleteObject ((HGDIOBJ*)&m_hcurStretch);
}
//*******************************************************************************************
void GLOBAL_DATA::UpdateSysColors()
{
	clrBtnFace = ::GetSysColor(COLOR_BTNFACE);
	clrBtnShadow = ::GetSysColor(COLOR_BTNSHADOW);
	clrBtnDkShadow = ::GetSysColor(COLOR_3DDKSHADOW);
	clrBtnLight = ::GetSysColor(COLOR_3DLIGHT);
	clrBtnHilite = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	clrBtnText = ::GetSysColor(COLOR_BTNTEXT);
	clrGrayedText = ::GetSysColor (COLOR_GRAYTEXT);
	clrWindowFrame = ::GetSysColor(COLOR_WINDOWFRAME);

	clrHilite = ::GetSysColor(COLOR_HIGHLIGHT);
	clrTextHilite = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

	clrMenu = ::GetSysColor(COLOR_MENU);
	clrMenuText = ::GetSysColor(COLOR_MENUTEXT);

	AfxDeleteObject((HGDIOBJ*)&hbrBtnShadow);
	AfxDeleteObject((HGDIOBJ*)&hbrBtnHilite);

	hbrBtnShadow = ::CreateSolidBrush(clrBtnShadow);
	ASSERT(hbrBtnShadow != NULL);
	hbrBtnHilite = ::CreateSolidBrush(clrBtnHilite);
	ASSERT(hbrBtnHilite != NULL);

	brBtnFace.CreateSolidBrush(clrBtnFace);
}
