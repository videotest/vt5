//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

#include "stdafx.h"
#include "globals.h"
#include "bcgbarres.h"
#include "bcgsound.h"

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
__declspec(dllexport) GLOBAL_DATA *pGlobalData;

// Initialization code
GLOBAL_DATA::GLOBAL_DATA()
{
	pGlobalData = this;
	//---------------------------------------------------------
	// Cached system values (updated in CWnd::OnSysColorChange)
	//---------------------------------------------------------
	hbrBtnShadow = NULL;
	hbrBtnHilite = NULL;
	UpdateSysColors();

	m_hcurStretch = NULL;
	m_hcurStretchVert = NULL;
	m_hcurHand = NULL;
	m_hiconTool = NULL;

	//------------------
	// Initialize fonts:
	//------------------
	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	::SystemParametersInfo (SPI_GETNONCLIENTMETRICS, sizeof(info), &info, 0);
   
	LOGFONT lf;
	memset (&lf, 0, sizeof (LOGFONT));

	lf.lfCharSet = DEFAULT_CHARSET; // Add by Yuhu.Wang on 99/10/25
	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;	// By Sven Ritter
	lf.lfItalic = info.lfMenuFont.lfItalic;

	//////////////////////////////////////////////
	// Modify by Yuhu.Wang on 99/11/02
	// Check if we should use system font
	//--------------------------------------------
	_tcscpy (lf.lfFaceName, info.lfMenuFont.lfFaceName);
	
	BOOL fUseSystemFont = (info.lfMenuFont.lfCharSet > SYMBOL_CHARSET);
	if (!fUseSystemFont)
	{
		//----------------------------------
		// Check for "Tahoma" font existance:
		//----------------------------------
		CWindowDC dc (NULL);
		if (::EnumFontFamilies (dc.GetSafeHdc (), NULL, FontFamalyProcFonts, 0) == 0)
		{
			//---------------------------
			// Found! Use MS Office font!
			//---------------------------
			_tcscpy (lf.lfFaceName, strOfficeFontName);
		}
		else
		{
			//-----------------------------
			// Not found. Use default font:
			//-----------------------------
			_tcscpy (lf.lfFaceName, strDefaultFontName);
		}
	}
	//--------------------------------------------
	//////////////////////////////////////////////

	fontRegular.CreateFontIndirect (&lf);

	lf.lfUnderline = TRUE;
	fontUnderline.CreateFontIndirect (&lf);
	lf.lfUnderline = FALSE;

	//------------------
	// Create bold font:
	//------------------
	lf.lfWeight = FW_BOLD;
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
			lf.lfEscapement = 2700;	// By Andy
			
			lf.lfHeight = info.lfMenuFont.lfHeight;	// By Sven Ritter
			lf.lfWeight = info.lfMenuFont.lfWeight;
			lf.lfItalic = info.lfMenuFont.lfItalic;

			//////////////////////////////////////////////
			// Modify by Yuhu.Wang on 99/11/02
			//--------------------------------------------
//			if(!fUseSystemFont)
			{
				_tcscpy (lf.lfFaceName, strVertFontName);
			}
			//--------------------------------------------
			//////////////////////////////////////////////

			fontVert.CreateFontIndirect (&lf);

			lf.lfEscapement = 900;
			fontVertCaption.CreateFontIndirect (&lf);
		}
	}

	UpdateTextMetrics ();

	//-----------------------
	// Detect the kind of OS:
	//-----------------------
	OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	::GetVersionEx (&osvi);
	bIsWindowsNT4 = ((osvi.dwPlatformId == VER_PLATFORM_WIN32_NT) &&
					(osvi.dwMajorVersion < 5));
}
//*******************************************************************************************
GLOBAL_DATA::~GLOBAL_DATA()
{
	// Terminate sound thread:
	BCGPlaySystemSound (BCGSOUND_TERMINATE);

	// cleanup standard brushes
	if (hbrBtnShadow != NULL)
	{
		::DeleteObject (hbrBtnShadow);
	}

	if (hbrBtnHilite != NULL)
	{
		::DeleteObject (hbrBtnHilite);
	}

	// cleanup standard cursors
	if (m_hcurStretch != NULL)
	{
		::DeleteObject (m_hcurStretch);
	}

	if (m_hcurStretchVert != NULL)
	{
		::DeleteObject (m_hcurStretchVert);
	}
	
	if (m_hcurHand != NULL)
	{
		::DeleteObject (m_hcurHand);
	}

	if (m_hiconTool != NULL)
	{
		VERIFY(::DestroyIcon (m_hiconTool));
	}
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

	if (hbrBtnShadow != NULL)
	{
		::DeleteObject (hbrBtnShadow);
	}

	if (hbrBtnHilite != NULL)
	{
		::DeleteObject (hbrBtnHilite);
	}

	hbrBtnShadow = ::CreateSolidBrush(clrBtnShadow);
	ASSERT(hbrBtnShadow != NULL);
	hbrBtnHilite = ::CreateSolidBrush(clrBtnHilite);
	ASSERT(hbrBtnHilite != NULL);

	brBtnFace.DeleteObject ();
	brBtnFace.CreateSolidBrush (clrBtnFace);

	brHilite.DeleteObject ();
	brHilite.CreateSolidBrush (clrHilite);

	brBlack.DeleteObject ();
	brBlack.CreateSolidBrush (clrBtnDkShadow);

	CWindowDC dc (NULL);
	m_nBitsPerPixel = dc.GetDeviceCaps (BITSPIXEL);
	brLight.DeleteObject ();
	
	if (m_nBitsPerPixel > 8)
	{
		//-------------------
		// By Maarten Hoeben:
		//-------------------
		COLORREF clrLight = RGB (
			GetRValue(clrBtnFace) + ((GetRValue(clrBtnHilite) -
				GetRValue(clrBtnFace)) / 2 ),
			GetGValue(clrBtnFace) + ((GetGValue(clrBtnHilite) -
				GetGValue(clrBtnFace)) / 2),
			GetBValue(clrBtnFace) + ((GetBValue(clrBtnHilite) -
				GetBValue(clrBtnFace)) / 2),
			);

		brLight.CreateSolidBrush (clrLight);
	}
	else
	{
		HBITMAP hbmGray = CreateDitherBitmap (dc.GetSafeHdc ());
		ASSERT (hbmGray != NULL);

		CBitmap bmp;
		bmp.Attach (hbmGray);

		brLight.CreatePatternBrush (&bmp);
	}
}
//************************************************************************************
BOOL GLOBAL_DATA::SetMenuFont (LPLOGFONT lpLogFont, BOOL bHorz)
{
	ASSERT (lpLogFont != NULL);

	if (bHorz)
	{
		//---------------------
		// Create regular font:
		//---------------------
		fontRegular.DeleteObject ();
		if (!fontRegular.CreateFontIndirect (lpLogFont))
		{
			ASSERT (FALSE);
			return FALSE;
		}

		//-----------------------
		// Create underline font:
		//-----------------------
		lpLogFont->lfUnderline = TRUE;
		fontUnderline.DeleteObject ();
		fontUnderline.CreateFontIndirect (lpLogFont);
		lpLogFont->lfUnderline = FALSE;

		//---------------------------------------------------
		// Create bold font (used in the default menu items):
		//---------------------------------------------------
		long lSavedWeight = lpLogFont->lfWeight;
		lpLogFont->lfWeight = 700;

		fontBold.DeleteObject ();
		BOOL bResult = fontBold.CreateFontIndirect (lpLogFont);

		lpLogFont->lfWeight = lSavedWeight;	// Restore weight

		if (!bResult)
		{
			ASSERT (FALSE);
			return FALSE;
		}
	}
	else	// Vertical font
	{
		fontVert.DeleteObject ();
		if (!fontVert.CreateFontIndirect (lpLogFont))
		{
			ASSERT (FALSE);
			return FALSE;
		}
	}

	UpdateTextMetrics ();
	return TRUE;
}
//************************************************************************************
void GLOBAL_DATA::UpdateTextMetrics ()
{
	CWindowDC dc (NULL);

	CFont* pOldFont = dc.SelectObject (&fontRegular);
	ASSERT (pOldFont != NULL);

	TEXTMETRIC tm;
	dc.GetTextMetrics (&tm);

	m_nTextHeightHorz = tm.tmHeight + 2;

	dc.SelectObject (&fontVert);
	dc.GetTextMetrics (&tm);

	m_nTextHeightVert = tm.tmHeight + 2;

	dc.SelectObject (pOldFont);
}
//*******************************************************************************
HBITMAP GLOBAL_DATA::CreateDitherBitmap (HDC hDC)
{
	struct  // BITMAPINFO with 16 colors
	{
		BITMAPINFOHEADER bmiHeader;
		RGBQUAD      bmiColors[16];
	} 
	bmi;
	memset(&bmi, 0, sizeof(bmi));

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = 8;
	bmi.bmiHeader.biHeight = 8;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 1;
	bmi.bmiHeader.biCompression = BI_RGB;

	COLORREF clr = ::GetSysColor(COLOR_BTNFACE);

	bmi.bmiColors[0].rgbBlue = GetBValue(clr);
	bmi.bmiColors[0].rgbGreen = GetGValue(clr);
	bmi.bmiColors[0].rgbRed = GetRValue(clr);

	clr = ::GetSysColor(COLOR_BTNHIGHLIGHT);
	bmi.bmiColors[1].rgbBlue = GetBValue(clr);
	bmi.bmiColors[1].rgbGreen = GetGValue(clr);
	bmi.bmiColors[1].rgbRed = GetRValue(clr);

	// initialize the brushes
	long patGray[8];
	for (int i = 0; i < 8; i++)
	   patGray[i] = (i & 1) ? 0xAAAA5555L : 0x5555AAAAL;

	HBITMAP hbm = CreateDIBitmap(hDC, &bmi.bmiHeader, CBM_INIT,
		(LPBYTE)patGray, (LPBITMAPINFO)&bmi, DIB_RGB_COLORS);
	return hbm;
}
