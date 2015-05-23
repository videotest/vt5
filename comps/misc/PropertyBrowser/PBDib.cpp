// PBDib.cpp: implementation of the PBDib class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PBDib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define DIB_HEADER_MARKER   ((WORD) ('M' << 8) | 'B')

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PBDib::PBDib()
	{
	m_hPalette = NULL;
	m_hDIB = NULL;
	}

PBDib::~PBDib()
	{
	Free();
	}

void PBDib::Free()
	{
	if(m_hDIB)
		{
		::GlobalFree(m_hDIB);
		m_hDIB = NULL;
		}
	if(m_hPalette)
		{
		::GlobalFree(m_hPalette);
		m_hPalette = NULL;
		}
	}

BOOL PBDib::ReadDIBFile(LPCTSTR szFile)
	{
	BITMAPFILEHEADER bmfHeader;
	DWORD dwBitsSize;
	LPSTR pDIB;
	HANDLE hFile;

	Free();
	hFile = ::CreateFile(szFile,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
	if(hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	/*
	* get length of DIB in bytes for use when reading
	*/
	dwBitsSize = ::GetFileSize(hFile,NULL);
	
	/*
	* Go read the DIB file header and check if it's valid.
	*/
	DWORD dwRead;
	if(!ReadFile(hFile,&bmfHeader, sizeof(bmfHeader),&dwRead,NULL))
		{
		::CloseHandle(hFile);
		return FALSE;
		}
	if ( dwRead != sizeof(bmfHeader))
		{
		::CloseHandle(hFile);
		return FALSE;
		}
	
	if (bmfHeader.bfType != DIB_HEADER_MARKER)
		{
		::CloseHandle(hFile);
		return FALSE;
		}
	
		/* Allocate memory for DIB*/

	m_hDIB = ::GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT, dwBitsSize);
	if (m_hDIB == 0)
		{
		::CloseHandle(hFile);
		return FALSE;
		}
	pDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	
	/*
	* Go read the bits.
	*/
	if(!ReadFile(hFile,pDIB, dwBitsSize - sizeof(BITMAPFILEHEADER),&dwRead,NULL))
		{
		::CloseHandle(hFile);
		::GlobalUnlock((HGLOBAL) m_hDIB);
		::GlobalFree((HGLOBAL) m_hDIB);
		return FALSE;
		}

	if (dwRead != dwBitsSize - sizeof(BITMAPFILEHEADER) )
		{
		::CloseHandle(hFile);
		::GlobalUnlock((HGLOBAL) m_hDIB);
		::GlobalFree((HGLOBAL) m_hDIB);
		return FALSE;
		}
	::GlobalUnlock((HGLOBAL) m_hDIB);
	::CloseHandle(hFile);
	BOOL bLoadPalette = CreatePalette();
	if(!bLoadPalette)
		Free();
	return bLoadPalette;
	}

BOOL PBDib::CreatePalette()
	{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPSTR lpbi;              // pointer to packed-DIB
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	BOOL bResult = FALSE;
	
	/* if handle to DIB is invalid, return FALSE */
	
	if (m_hDIB == NULL)
		return FALSE;
	
	lpbi = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	
	/* get pointer to BITMAPINFO (Win 3.0) */
	lpbmi = (LPBITMAPINFO)lpbi;
	
	/* get pointer to BITMAPCOREINFO (old 1.x) */
	lpbmc = (LPBITMAPCOREINFO)lpbi;
	
	/* get the number of colors in the DIB */
	wNumColors = GetNumColors(lpbi);
	
	if (wNumColors != 0)
		{
		/* allocate memory block for logical palette */
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
			+ sizeof(PALETTEENTRY)
			* wNumColors);
		
		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
			{
			::GlobalUnlock((HGLOBAL) m_hDIB);
			return FALSE;
			}
		
		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);
		
		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;
		
		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		for (i = 0; i < (int)wNumColors; i++)
			{
			if (bWinStyleDIB)
				{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
				}
			else
				{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
				}
			}
		
		/* create the palette and get handle to it */
		m_hPalette = ::CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
		}
	
	::GlobalUnlock((HGLOBAL) m_hDIB);
	return (m_hPalette != NULL) ? TRUE : FALSE;
	}

WORD PBDib::GetNumColors(LPSTR lpbi)
	{
	WORD wBitCount;  // DIB bit count
	
	/*  If this is a Windows-style DIB, the number of colors in the
	 *  color table can be less than the number of bits per pixel
	 *  allows for (i.e. lpbi->biClrUsed can be set to some value).
	 *  If this is the case, return the appropriate value.
	 */
	
	if (IS_WIN30_DIB(lpbi))
		{
		DWORD dwClrUsed;
	
		dwClrUsed = ((LPBITMAPINFOHEADER)lpbi)->biClrUsed;
		if (dwClrUsed != 0)
			return (WORD)dwClrUsed;
		}
	
		/*  Calculate the number of colors in the color table based on
		*  the number of bits per pixel for the DIB.
	*/
	if (IS_WIN30_DIB(lpbi))
		wBitCount = ((LPBITMAPINFOHEADER)lpbi)->biBitCount;
	else
		wBitCount = ((LPBITMAPCOREHEADER)lpbi)->bcBitCount;
	
	/* return number of colors based on bits per pixel */
	switch (wBitCount)
		{
		case 1:
			return 2;
			
		case 4:
			return 16;
			
		case 8:
			return 256;
			
		default:
			return 0;
		}
	}

int PBDib::GetHeight()
	{
	if(!m_hDIB)
		return 0;
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB
	int nHeight = 0;
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);

	
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;

	/* point to the header (whether old or Win 3.0 */

	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;
	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		nHeight = lpbmi->biHeight;
	else  /* it is an other-style DIB, so return its height */
		nHeight = lpbmc->bcHeight;
	::GlobalUnlock(m_hDIB);
	return nHeight;
	}

int PBDib::GetWidth()
	{
	if(!m_hDIB)
		return 0;
	LPBITMAPINFOHEADER lpbmi;  // pointer to a Win 3.0-style DIB
	LPBITMAPCOREHEADER lpbmc;  // pointer to an other-style DIB
	int nWidth = 0;
	LPSTR lpDIB = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	
	
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;
	
	/* point to the header (whether old or Win 3.0 */
	
	lpbmi = (LPBITMAPINFOHEADER)lpDIB;
	lpbmc = (LPBITMAPCOREHEADER)lpDIB;
	/* return the DIB height if it is a Win 3.0 DIB */
	if (IS_WIN30_DIB(lpDIB))
		nWidth = lpbmi->biWidth;
	else  /* it is an other-style DIB, so return its height */
		nWidth = lpbmc->bcWidth;
	::GlobalUnlock(m_hDIB);
	return nWidth;
	
	}

HBITMAP PBDib::CreateBitmap()
	{
    LPBITMAPINFOHEADER  lpbi;
    HDC                 hdc;
    HBITMAP             hbm;
	HPALETTE hpalT;
	
    if (!m_hDIB)
        return NULL;
	
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(m_hDIB);
	
    if (!lpbi)
        return NULL;
	
    hdc = GetDC(NULL);
	
    if (m_hPalette)
		{
        hpalT = SelectPalette(hdc,m_hPalette,FALSE);
        RealizePalette(hdc);     // GDI Bug...????
		}
	
    hbm = CreateDIBitmap(hdc,
		(LPBITMAPINFOHEADER)lpbi,
		(LONG)CBM_INIT,
		(LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
		(LPBITMAPINFO)lpbi,
		DIB_RGB_COLORS );
	
    if (m_hPalette)
        SelectPalette(hdc,hpalT,FALSE);
	
    ReleaseDC(NULL,hdc);
    GlobalUnlock(m_hDIB);
    return hbm;
	}

int PBDib::PaletteSize(VOID FAR * pv)
	{
    LPBITMAPINFOHEADER lpbi;
    WORD               NumColors;
	
    lpbi      = (LPBITMAPINFOHEADER)pv;
    NumColors = GetNumColors((LPSTR)lpbi);
	
    if (lpbi->biSize == sizeof(BITMAPCOREHEADER))
        return (NumColors * sizeof(RGBTRIPLE));
    else
        return (NumColors * sizeof(RGBQUAD));
	
	}

HPALETTE PBDib::ClonePalette()
	{
	LPLOGPALETTE lpPal;      // pointer to a logical palette
	HANDLE hLogPal;          // handle to a logical palette
	int i;                   // loop index
	WORD wNumColors;         // number of colors in color table
	LPSTR lpbi;              // pointer to packed-DIB
	LPBITMAPINFO lpbmi;      // pointer to BITMAPINFO structure (Win3.0)
	LPBITMAPCOREINFO lpbmc;  // pointer to BITMAPCOREINFO structure (old)
	BOOL bWinStyleDIB;       // flag which signifies whether this is a Win3.0 DIB
	HPALETTE hPal;
	/* if handle to DIB is invalid, return FALSE */
	
	if (m_hDIB == NULL)
		return NULL;
	
	lpbi = (LPSTR) ::GlobalLock((HGLOBAL) m_hDIB);
	
	/* get pointer to BITMAPINFO (Win 3.0) */
	lpbmi = (LPBITMAPINFO)lpbi;
	
	/* get pointer to BITMAPCOREINFO (old 1.x) */
	lpbmc = (LPBITMAPCOREINFO)lpbi;
	
	/* get the number of colors in the DIB */
	wNumColors = GetNumColors(lpbi);
	
	if (wNumColors != 0)
		{
		/* allocate memory block for logical palette */
		hLogPal = ::GlobalAlloc(GHND, sizeof(LOGPALETTE)
			+ sizeof(PALETTEENTRY)
			* wNumColors);
		
		/* if not enough memory, clean up and return NULL */
		if (hLogPal == 0)
			{
			::GlobalUnlock((HGLOBAL) m_hDIB);
			return NULL;
			}
		
		lpPal = (LPLOGPALETTE) ::GlobalLock((HGLOBAL) hLogPal);
		
		/* set version and number of palette entries */
		lpPal->palVersion = PALVERSION;
		lpPal->palNumEntries = (WORD)wNumColors;
		
		/* is this a Win 3.0 DIB? */
		bWinStyleDIB = IS_WIN30_DIB(lpbi);
		for (i = 0; i < (int)wNumColors; i++)
			{
			if (bWinStyleDIB)
				{
				lpPal->palPalEntry[i].peRed = lpbmi->bmiColors[i].rgbRed;
				lpPal->palPalEntry[i].peGreen = lpbmi->bmiColors[i].rgbGreen;
				lpPal->palPalEntry[i].peBlue = lpbmi->bmiColors[i].rgbBlue;
				lpPal->palPalEntry[i].peFlags = 0;
				}
			else
				{
				lpPal->palPalEntry[i].peRed = lpbmc->bmciColors[i].rgbtRed;
				lpPal->palPalEntry[i].peGreen = lpbmc->bmciColors[i].rgbtGreen;
				lpPal->palPalEntry[i].peBlue = lpbmc->bmciColors[i].rgbtBlue;
				lpPal->palPalEntry[i].peFlags = 0;
				}
			}
		
		/* create the palette and get handle to it */
		hPal = ::CreatePalette(lpPal);
		::GlobalUnlock((HGLOBAL) hLogPal);
		::GlobalFree((HGLOBAL) hLogPal);
		}
	
	::GlobalUnlock((HGLOBAL) m_hDIB);
	return hPal;
	}

HBITMAP PBDib::CreateBitmapXY(SIZE szBmp)
	{

    LPBITMAPINFOHEADER  lpbi;
    HDC                 hdc;
    HBITMAP             hbm;
	HPALETTE hpalT;
 	if(szBmp.cx == 0 || szBmp.cy == 0)
		return NULL;
   if (!m_hDIB)
        return NULL;
	int h = GetHeight();
	int w = GetWidth();
     lpbi = (LPBITMAPINFOHEADER)GlobalLock(m_hDIB);
   hdc = CreateCompatibleDC(NULL);
	hbm = ::CreateCompatibleBitmap(hdc,szBmp.cx,szBmp.cy);
	HBITMAP hOldBmp = (HBITMAP)::SelectObject(hdc,hbm);
    if (m_hPalette)
		{
        hpalT = SelectPalette(hdc,m_hPalette,FALSE);
        RealizePalette(hdc);     // GDI Bug...????
		}
	StretchDIBits(hdc, 0,0, szBmp.cx,szBmp.cy,
		0,0,w,h,
		(LPSTR)lpbi + lpbi->biSize + PaletteSize(lpbi),
		(LPBITMAPINFO)lpbi,
		DIB_RGB_COLORS,
		SRCCOPY);
    if (m_hPalette)
        SelectPalette(hdc,hpalT,FALSE);
	SelectObject(hdc,hOldBmp);	
    ReleaseDC(NULL,hdc);
    GlobalUnlock(m_hDIB);
    return hbm;
	}
