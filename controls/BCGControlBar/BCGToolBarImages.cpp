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

// BCGToolBarImages.cpp: implementation of the CBCGToolBarImages class.
//
//////////////////////////////////////////////////////////////////////

#include "Stdafx.h"
#include "globals.h"
#include "BCGToolBarImages.h"
#include "BCGToolBar.h"
#include "bcglocalres.h"
#include "bcgbarres.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

static BOOL WriteDIB( LPCTSTR szFile, HANDLE hDIB);
static HANDLE DDBToDIB (HBITMAP bitmap, DWORD dwCompression);


// globals for fast drawing (shared globals)
static HDC hDCGlyphs = NULL;
static HDC hDCMono = NULL;

/*
	DIBs use RGBQUAD format:
		0xbb 0xgg 0xrr 0x00

	Reasonably efficient code to convert a COLORREF into an
	RGBQUAD is byte-order-dependent, so we need different
	code depending on the byte order we're targeting.
*/

#define RGB_TO_RGBQUAD(r,g,b)   (RGB(b,g,r))
#define CLR_TO_RGBQUAD(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))
#define RGBQUAD_TO_CLR(clr)     (RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr)))

// Raster Ops
#define ROP_DSPDxax  0x00E20746L
#define ROP_PSDPxax  0x00B8074AL

/////////////////////////////////////////////////////////////////////////////
// Init / Term

// a special struct that will cleanup automatically
struct _AFX_TOOLBAR_TERM
{
	~_AFX_TOOLBAR_TERM()
	{
		if (hDCMono != NULL)
		{
			::DeleteDC (hDCMono);
			hDCMono = NULL;
		}

		if (hDCGlyphs != NULL)
		{
			::DeleteDC (hDCGlyphs);
			hDCGlyphs = NULL;
		}
	}
};

static const _AFX_TOOLBAR_TERM toolbarTerm;

// vanek : drawing disabled icons - 27.02.2005
CImageDrawer CBCGToolBarImages::m_ImgDrawer;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGToolBarImages::CBCGToolBarImages()
{
	m_bModified = FALSE;
	m_bReadOnly = FALSE;
	m_iCount = 0;

	m_hbmImageWell = NULL;
	m_hInstImageWell = NULL;

	m_bUserImagesList = FALSE;

	// initialize the toolbar drawing engine
	static BOOL bInitialized;
	if (!bInitialized)
	{
		hDCGlyphs = CreateCompatibleDC(NULL);

		// Mono DC and Bitmap for disabled image
		hDCMono = ::CreateCompatibleDC(NULL);

		if (hDCGlyphs == NULL || hDCMono == NULL)
			AfxThrowResourceException();

		bInitialized = TRUE;
	}

	m_clrTransparent = (COLORREF) -1;

	// UISG standard sizes
	m_sizeImage = CSize (16, 15);
	m_sizeImageDest = CSize (0, 0);
	m_bStretch = FALSE;
	m_pBmpOriginal = NULL;
}
//*******************************************************************************
CBCGToolBarImages::~CBCGToolBarImages()
{
	ASSERT (m_dcMem.GetSafeHdc () == NULL);
	ASSERT (m_bmpMem.GetSafeHandle () == NULL);
	ASSERT (m_pBmpOriginal == NULL);

	AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);
}
//*******************************************************************************
BOOL CBCGToolBarImages::Load (UINT uiResID, HINSTANCE hinstRes, BOOL bAdd)
{
	LPCTSTR lpszResourceName = MAKEINTRESOURCE (uiResID);
	ASSERT(lpszResourceName != NULL);

	if (!bAdd)
	{
		AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);     // get rid of old one
		m_lstOrigResIds.RemoveAll ();
		m_mapOrigResOffsets.RemoveAll ();
	}
	else if (m_lstOrigResIds.Find (uiResID) != NULL)	// Already loaded, do nothing
	{
		return TRUE;
	}

	m_hInstImageWell = (hinstRes == NULL) ? 
		AfxFindResourceHandle (lpszResourceName, RT_BITMAP) : 
		hinstRes;

	HBITMAP hbmp = (HBITMAP) ::LoadImage (
		m_hInstImageWell,
		lpszResourceName,
		IMAGE_BITMAP,
		0, 0,
		LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);

	if (hbmp == NULL)
	{
		TRACE(_T("Can't load bitmap: %x. GetLastError() = %x\n"), 
			uiResID,
			GetLastError ());
		return FALSE;
	}

	if (bAdd)
	{
		m_mapOrigResOffsets.SetAt (uiResID, m_iCount);

		AddImage (hbmp);
		m_lstOrigResIds.AddTail (uiResID);
		::DeleteObject (hbmp);
	}
	else
	{
		m_hbmImageWell = hbmp;
	}

	UpdateCount ();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGToolBarImages::Load (LPCTSTR lpszBmpFileName)
{
	ASSERT (lpszBmpFileName != NULL);

	AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);     // get rid of old one

	CString strPath = lpszBmpFileName;

	//-----------------------------------------------------------------------
	// If the image path is not defined, try to open it in the EXE directory:
	//-----------------------------------------------------------------------
	if (strPath.Find (_T("\\")) == -1 &&
		strPath.Find (_T("/")) == -1 &&
		strPath.Find (_T(":")) == -1)
	{
		TCHAR lpszFilePath [_MAX_PATH];
		if (::GetModuleFileName (NULL, lpszFilePath, _MAX_PATH) > 0)
		{
			TCHAR path_buffer[_MAX_PATH];   
			TCHAR drive[_MAX_DRIVE];   
			TCHAR dir[_MAX_DIR];
			TCHAR fname[_MAX_FNAME];   
			TCHAR ext[_MAX_EXT];

			_tsplitpath (lpszFilePath, drive, dir, NULL, NULL);
			_tsplitpath (lpszBmpFileName, NULL, NULL, fname, ext);

			_tmakepath (path_buffer, drive, dir, fname, ext);
			strPath = path_buffer;
		}
	}

	//--------------------------------
	// Load images from the disk file:
	//--------------------------------
	m_hbmImageWell = (HBITMAP) ::LoadImage (
		AfxGetInstanceHandle (),
		strPath,
		IMAGE_BITMAP,
		0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);

	if (m_hbmImageWell == NULL)
	{
		TRACE(_T("Can't load bitmap: %s. GetLastError() = %x\r\n"), 
			lpszBmpFileName,
			GetLastError ());
		return FALSE;
	}

	m_bUserImagesList = TRUE;
	m_strUDLPath = strPath;

	if (::GetFileAttributes (strPath) & FILE_ATTRIBUTE_READONLY)
	{
		m_bReadOnly = TRUE;
	}

	UpdateCount ();
	return TRUE;
}
//*******************************************************************************
BOOL CBCGToolBarImages::PrepareDrawImage (CBCGDrawState& ds,
										  CSize sizeImageDest)
{
	if (m_hbmImageWell == NULL)
	{
		return FALSE;
	}

	m_bStretch = FALSE;

	ASSERT(m_hbmImageWell != NULL);
	ASSERT (m_dcMem.GetSafeHdc () == NULL);
	ASSERT (m_bmpMem.GetSafeHandle () == NULL);
	ASSERT (m_pBmpOriginal == NULL);

	// We need to kick-start the bitmap selection process.
	ds.hbmOldGlyphs = (HBITMAP)SelectObject (hDCGlyphs, m_hbmImageWell);
	ds.hbmMono = CreateBitmap (m_sizeImage.cx + 2, m_sizeImage.cy + 2,
					1, 1, NULL);
	ds.hbmMonoOld = (HBITMAP)SelectObject(hDCMono, ds.hbmMono);
	if (ds.hbmOldGlyphs == NULL || ds.hbmMono == NULL || ds.hbmMonoOld == NULL)
	{
		TRACE0("Error: can't draw toolbar.\r\n");
		AfxDeleteObject((HGDIOBJ*)&ds.hbmMono);
		return FALSE;
	}

	if (sizeImageDest.cx <= 0 || sizeImageDest.cy <= 0)
	{
		m_sizeImageDest = m_sizeImage;
	}
	else
	{
		m_sizeImageDest = sizeImageDest;
	}

	if (m_sizeImageDest != m_sizeImage || m_clrTransparent != (COLORREF) -1)
	{
		CWindowDC dc (NULL);

		m_bStretch = (m_sizeImageDest != m_sizeImage);

		m_dcMem.CreateCompatibleDC (NULL);	// Assume display!
		m_bmpMem.CreateCompatibleBitmap (&dc, m_sizeImage.cx + 2, m_sizeImage.cy + 2);

		m_pBmpOriginal = m_dcMem.SelectObject (&m_bmpMem);
		ASSERT (m_pBmpOriginal != NULL);
	}

	return TRUE;
}
//*******************************************************************************
void CBCGToolBarImages::EndDrawImage (CBCGDrawState& ds)
{
	SelectObject(hDCMono, ds.hbmMonoOld);
	AfxDeleteObject((HGDIOBJ*)&ds.hbmMono);
	SelectObject(hDCGlyphs, ds.hbmOldGlyphs);

	m_sizeImageDest = CSize (0, 0);

	if (m_bStretch || m_clrTransparent != (COLORREF) -1)
	{
		ASSERT (m_pBmpOriginal != NULL);

		m_dcMem.SelectObject (m_pBmpOriginal);
		m_pBmpOriginal = NULL;

		::DeleteObject (m_bmpMem.Detach ());
		::DeleteObject (m_dcMem.Detach ());
	}
}
//*******************************************************************************
void CBCGToolBarImages::CreateMask(int iImage, BOOL bHilite, BOOL bHiliteShadow)
{
	// initalize whole area with 0's
	PatBlt(hDCMono, 0, 0, m_sizeImage.cx + 2, m_sizeImage.cy + 2, WHITENESS);

	// create mask based on color bitmap
	// convert this to 1's
	SetBkColor(hDCGlyphs, /*RGB( 192, 192, 192 )*/afxData.clrBtnFace );

	::BitBlt(hDCMono, 0, 0, m_sizeImage.cx, m_sizeImage.cy,
		hDCGlyphs, iImage * m_sizeImage.cx, 0, SRCCOPY);

	if (bHilite)
	{
		// convert this to 1's
		SetBkColor(hDCGlyphs, /*RGB( 192, 192, 192 )*/afxData.clrBtnHilite);

		// OR in the new 1's
		::BitBlt(hDCMono, 0, 0, m_sizeImage.cx, m_sizeImage.cy,
			hDCGlyphs, iImage * m_sizeImage.cx, 0, SRCPAINT);

		if (bHiliteShadow)
		{
			::BitBlt(hDCMono, 1, 1, m_sizeImage.cx + 1, m_sizeImage.cy + 1,
				hDCMono, 0, 0, SRCAND);
		}
	}
}
//********************************************************************************
BOOL CBCGToolBarImages::Draw (CDC* pDCDest, 
				int xDest, int yDest,
				int iImage,
				BOOL bHilite,
				BOOL bDisabled,
				BOOL bIndeterminate)
{
	if (iImage < 0 || iImage >= m_iCount)
	{
		return FALSE;
	}

	if (m_bStretch)
	{
		bHilite = FALSE;
		bIndeterminate = FALSE;
	}

	BOOL bIsTransparent = (m_clrTransparent != (COLORREF) -1);

	CDC* pDC = m_bStretch || bIsTransparent ? &m_dcMem : pDCDest;
	ASSERT_VALID(pDC);

	int x = m_bStretch || bIsTransparent ? 0 : xDest;
	int y = m_bStretch || bIsTransparent ? 0 : yDest;

 	if (m_bStretch || bIsTransparent)
	{
		pDC->FillRect (CRect (CPoint (0, 0), m_sizeImage), 
			&globalData.brBtnFace);
	}

	// vanek : drawing disabled icons - 27.02.2005
	if( bDisabled )
	{
		// create bitmap
		CBitmap bitmap;
		CDC dc_memory;
		{
			CClientDC dcScreen( 0 );						
			dc_memory.CreateCompatibleDC( &dcScreen );
		}

		BITMAPINFOHEADER	bmih;
		::ZeroMemory( &bmih, sizeof( bmih ) );
		bmih.biBitCount = 24;
		bmih.biHeight = m_sizeImage.cy;
		bmih.biWidth = m_sizeImage.cx;
		bmih.biSize = sizeof( bmih );
		bmih.biPlanes = 1;

		byte	*pdibBits = 0;
		HBITMAP	hDIBSection = ::CreateDIBSection( dc_memory.m_hDC, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, 
			(void**)&pdibBits, 0, 0 );

		ASSERT( hDIBSection );            
		bitmap.Attach( hDIBSection );

		CBitmap *poldbmp = 0;
		poldbmp = dc_memory.SelectObject( &bitmap );

		RECT rc = { 0, 0, bmih.biWidth, bmih.biHeight };

		// draw image
		::BitBlt( dc_memory.m_hDC, 0, 0, m_sizeImage.cx, m_sizeImage.cy,	hDCGlyphs, iImage * m_sizeImage.cx, 0, SRCCOPY);
		if( poldbmp )
			dc_memory.SelectObject( poldbmp ); poldbmp = 0;

		// [vanek] - 07.03.2005
		if( m_ImgDrawer.DrawImage( pDCDest->m_hDC, (HBITMAP)(bitmap), CRect(xDest,yDest,xDest+m_sizeImage.cx,yDest+m_sizeImage.cy), idfUseAlpha | idfScalingImage ) )
			return TRUE;
	}

//	if( !bIsTransparent )
	if (!bHilite && !bDisabled)
	{
		//----------------------
		// normal image version:
		//----------------------

		::BitBlt(pDC->m_hDC, x, y,
			m_sizeImage.cx, m_sizeImage.cy,
			hDCGlyphs, iImage * m_sizeImage.cx, 0, SRCCOPY);

	}
	else
	{
		if (bDisabled || bIndeterminate)
		{
			// disabled or indeterminate version
			CreateMask(iImage, TRUE, FALSE);

			pDC->SetTextColor(0L);                  // 0's in mono -> 0 (for ROP)
			pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1's in mono -> 1

			if (bDisabled)
			{
				// disabled - draw the hilighted shadow
				HGDIOBJ hbrOld = pDC->SelectObject (globalData.hbrBtnHilite);
				if (hbrOld != NULL)
				{
					// draw hilight color where we have 0's in the mask
					::BitBlt(pDC->m_hDC, x + 1, y + 1,
						m_sizeImage.cx + 2, m_sizeImage.cy + 2,
						hDCMono, 0, 0, ROP_PSDPxax);

					pDC->SelectObject(hbrOld);
				}
			}

			//BLOCK: always draw the shadow
			{
				HGDIOBJ hbrOld = pDC->SelectObject(globalData.hbrBtnShadow);
				if (hbrOld != NULL)
				{
					// draw the shadow color where we have 0's in the mask
					::BitBlt(pDC->m_hDC, 
						x, y,
						m_sizeImage.cx + 2, m_sizeImage.cy + 2,
						hDCMono, 0, 0, ROP_PSDPxax);

					pDC->SelectObject(hbrOld);
				}
			}
		}

		// if it is checked do the dither brush avoiding the glyph
		if (bHilite || bIndeterminate)
		{
			CBrush* pBrOld = pDC->SelectObject(&globalData.brLight);
			if (pBrOld != NULL)
			{
				CreateMask(iImage, !bIndeterminate, bDisabled);

				pDC->SetTextColor(0L);              // 0 -> 0
				pDC->SetBkColor((COLORREF)0x00FFFFFFL); // 1 -> 1

				// only draw the dither brush where the mask is 1's
				::BitBlt(pDC->m_hDC, x, y, 
					m_sizeImage.cx, m_sizeImage.cy,
					hDCMono, 0, 0, ROP_DSPDxax);

				pDC->SelectObject(pBrOld);
			}
		}
	}

	if (bIsTransparent)
	{
		if (m_bStretch)
		{
			TransparentBlt (pDCDest->GetSafeHdc (), xDest, yDest, 
							m_sizeImage.cx, m_sizeImage.cy, 
							pDC, 0, 0, m_clrTransparent,
							m_sizeImageDest.cx, m_sizeImageDest.cy);
		}
		else
		{
			TransparentBlt (pDCDest->GetSafeHdc (), xDest, yDest, 
							m_sizeImage.cx, m_sizeImage.cy, 
							pDC, 0, 0, m_clrTransparent);
		}
	}
	else if (m_bStretch)
	{
		pDCDest->StretchBlt (xDest, yDest, m_sizeImageDest.cx, m_sizeImageDest.cy,
							pDC, 0, 0, m_sizeImage.cx, m_sizeImage.cy, SRCCOPY);
	}

	return TRUE;
}
//********************************************************************************
void CBCGToolBarImages::FillDitheredRect (CDC* pDC, const CRect& rect)
{
	ASSERT_VALID(pDC);
	pDC->FillRect (&rect, &globalData.brLight);
}
//********************************************************************************
void CBCGToolBarImages::OnSysColorChange()
{
	int iOldCount = m_iCount;

	// re-color bitmap for toolbar
	if (m_hbmImageWell != NULL)
	{
		if (m_bUserImagesList)
		{
			Load (m_strUDLPath);
		}
		else
		{
			ASSERT (m_hInstImageWell != NULL);

			// Image was buit from the resources...
			if (m_lstOrigResIds.IsEmpty ())
			{
				ASSERT (FALSE);
				return;
			}

			AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);     // get rid of old one

			for (POSITION pos = m_lstOrigResIds.GetHeadPosition (); 
				pos != NULL;)
			{
				UINT uiResId = m_lstOrigResIds.GetNext (pos);
				ASSERT (uiResId > 0);

				HBITMAP hbmp = (HBITMAP) ::LoadImage (
					m_hInstImageWell,
					MAKEINTRESOURCE (uiResId),
					IMAGE_BITMAP,
					0, 0,
					LR_CREATEDIBSECTION | LR_LOADMAP3DCOLORS);

				AddImage (hbmp);
				::DeleteObject (hbmp);
			}
		}
	}

	UpdateCount ();
	ASSERT (iOldCount == m_iCount);
}
//********************************************************************************
void CBCGToolBarImages::UpdateCount ()
{
	if (m_hbmImageWell == NULL)
	{
		m_iCount = 0;
		return;
	}

	BITMAP bmp;
	if (::GetObject (m_hbmImageWell, sizeof (BITMAP), &bmp) == 0)
	{
		m_iCount = 0;
		return;
	}

	int iWidth = bmp.bmWidth;
	m_iCount = iWidth / m_sizeImage.cx;
}
/*
PBITMAPINFO CreateBitmapInfoStruct(HWND hwnd, HBITMAP hBmp)
{ 
    BITMAP bmp; 
    PBITMAPINFO pbmi; 
    WORD    cClrBits; 

    // Retrieve the bitmap color format, width, and height. 
    if (!GetObject(hBmp, sizeof(BITMAP), (LPSTR)&bmp)) 
        return 0;

    // Convert the color format to a count of bits. 
    cClrBits = (WORD)(bmp.bmPlanes * bmp.bmBitsPixel); 
    if (cClrBits == 1) 
        cClrBits = 1; 
    else if (cClrBits <= 4) 
        cClrBits = 4; 
    else if (cClrBits <= 8) 
        cClrBits = 8; 
    else if (cClrBits <= 16) 
        cClrBits = 16; 
    else if (cClrBits <= 24) 
        cClrBits = 24; 
    else cClrBits = 32; 

    // Allocate memory for the BITMAPINFO structure. (This structure 
    // contains a BITMAPINFOHEADER structure and an array of RGBQUAD 
    // data structures.) 

     if (cClrBits != 24) 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER) + 
                    sizeof(RGBQUAD) * (UINT)(1<< cClrBits)); 

     // There is no RGBQUAD array for the 24-bit-per-pixel format. 

     else 
         pbmi = (PBITMAPINFO) LocalAlloc(LPTR, 
                    sizeof(BITMAPINFOHEADER)); 

    // Initialize the fields in the BITMAPINFO structure. 

    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER); 
    pbmi->bmiHeader.biWidth = bmp.bmWidth; 
    pbmi->bmiHeader.biHeight = bmp.bmHeight; 
    pbmi->bmiHeader.biPlanes = bmp.bmPlanes; 
    pbmi->bmiHeader.biBitCount = bmp.bmBitsPixel; 
    if (cClrBits < 24) 
        pbmi->bmiHeader.biClrUsed = (1<<cClrBits); 

    // If the bitmap is not compressed, set the BI_RGB flag. 
    pbmi->bmiHeader.biCompression = BI_RGB; 

    // Compute the number of bytes in the array of color 
    // indices and store the result in biSizeImage. 
    // For Windows NT, the width must be DWORD aligned unless 
    // the bitmap is RLE compressed. This example shows this. 
    // For Windows 95/98/Me, the width must be WORD aligned unless the 
    // bitmap is RLE compressed.
    pbmi->bmiHeader.biSizeImage = ((pbmi->bmiHeader.biWidth * cClrBits +31) & ~31) /8
                                  * pbmi->bmiHeader.biHeight; 
    // Set biClrImportant to 0, indicating that all of the 
    // device colors are important. 
     pbmi->bmiHeader.biClrImportant = 0; 
     return pbmi; 
 } 

void CreateBMPFile(HWND hwnd, LPTSTR pszFile, PBITMAPINFO pbi, 
                  HBITMAP hBMP, HDC hDC) 
 { 
     HANDLE hf;                 // file handle 
    BITMAPFILEHEADER hdr;       // bitmap file-header 
    PBITMAPINFOHEADER pbih;     // bitmap info-header 
    LPBYTE lpBits;              // memory pointer 
    DWORD dwTotal;              // total count of bytes 
    DWORD cb;                   // incremental count of bytes 
    BYTE *hp;                   // byte pointer 
    DWORD dwTmp; 

    pbih = (PBITMAPINFOHEADER) pbi; 
    lpBits = (LPBYTE) GlobalAlloc(GMEM_FIXED, pbih->biSizeImage);

    if (!lpBits) 
         return;

    // Retrieve the color table (RGBQUAD array) and the bits 
    // (array of palette indices) from the DIB. 
    if (!GetDIBits(hDC, hBMP, 0, (WORD) pbih->biHeight, lpBits, pbi, 
        DIB_RGB_COLORS)) 
    {
        return;
    }

    // Create the .BMP file. 
    hf = CreateFile(pszFile, 
                   GENERIC_READ | GENERIC_WRITE, 
                   (DWORD) 0, 
                    NULL, 
                   CREATE_ALWAYS, 
                   FILE_ATTRIBUTE_NORMAL, 
                   (HANDLE) NULL); 
    if (hf == INVALID_HANDLE_VALUE) 
        return;
    hdr.bfType = 0x4d42;        // 0x42 = "B" 0x4d = "M" 
    // Compute the size of the entire file. 
    hdr.bfSize = (DWORD) (sizeof(BITMAPFILEHEADER) + 
                 pbih->biSize + pbih->biClrUsed 
                 * sizeof(RGBQUAD) + pbih->biSizeImage); 
    hdr.bfReserved1 = 0; 
    hdr.bfReserved2 = 0; 

    // Compute the offset to the array of color indices. 
    hdr.bfOffBits = (DWORD) sizeof(BITMAPFILEHEADER) + 
                    pbih->biSize + pbih->biClrUsed 
                    * sizeof (RGBQUAD); 

    // Copy the BITMAPFILEHEADER into the .BMP file. 
    if (!WriteFile(hf, (LPVOID) &hdr, sizeof(BITMAPFILEHEADER), 
        (LPDWORD) &dwTmp,  NULL)) 
    {
       return;
    }

    // Copy the BITMAPINFOHEADER and RGBQUAD array into the file. 
    if (!WriteFile(hf, (LPVOID) pbih, sizeof(BITMAPINFOHEADER) 
                  + pbih->biClrUsed * sizeof (RGBQUAD), 
                  (LPDWORD) &dwTmp, ( NULL) ) )
        return;

    // Copy the array of color indices into the .BMP file. 
    dwTotal = cb = pbih->biSizeImage; 
    hp = lpBits; 
    if (!WriteFile(hf, (LPSTR) hp, (int) cb, (LPDWORD) &dwTmp,NULL)) 
           return;

    // Close the .BMP file. 
     if (!CloseHandle(hf)) 
           return;

    // Free memory. 
    GlobalFree((HGLOBAL)lpBits);
}

bool write_bmp( HBITMAP hbmp, HDC hdc, int cx, int cy, int id )
{
	char sz_file[MAX_PATH];
	sprintf( sz_file, "d:\\qq\\tb_%08x_%04d.bmp", id, cx );

	BITMAPINFO* pbi = CreateBitmapInfoStruct( 0, hbmp );
	if( pbi )
	{
		CreateBMPFile( 0, sz_file, pbi, hbmp, hdc );
		::LocalFree( pbi );
	}


	return true;
}
*/
//////////////////////////////////////////
// Image editing methods:
//////////////////////////////////////////


int CBCGToolBarImages::AddImage (HBITMAP hbmp)
{
	//-------------------------------------------------------
	// Create memory source DC and select an original bitmap:
	//-------------------------------------------------------
	CDC memDCSrc;
	memDCSrc.CreateCompatibleDC (NULL);

	HBITMAP hOldBitmapSrc = NULL;

	int iBitmapWidth;
	int iBitmapHeight;
	int iNewBitmapWidth;

	BITMAP bmp;
	if (::GetObject (hbmp, sizeof (BITMAP), &bmp) == 0)
	{
		return -1;
	}

	iNewBitmapWidth = bmp.bmWidth;
	iBitmapHeight = bmp.bmHeight;

	if (m_hbmImageWell != NULL)
	{
		//-------------------------------
		// Get original bitmap attrbutes:
		//-------------------------------
		if (::GetObject (m_hbmImageWell, sizeof (BITMAP), &bmp) == 0)
		{
			return -1;
		}

		hOldBitmapSrc = (HBITMAP) memDCSrc.SelectObject (m_hbmImageWell);
		if (hOldBitmapSrc == NULL)
		{
			return -1;
		}

		iBitmapWidth = bmp.bmWidth;
		iBitmapHeight = bmp.bmHeight;
	}
	else
	{
		iBitmapWidth = 0;

		hOldBitmapSrc = (HBITMAP) memDCSrc.SelectObject (hbmp);
		if (hOldBitmapSrc == NULL)
		{
			return -1;
		}
	}

	//----------------------------------------------------------
	// Create a new bitmap compatibel with the source memory DC
	// (original bitmap SHOULD BE ALREADY SELECTED!):
	//----------------------------------------------------------
	
	//paul 8.01.2003. Change CreateCompatibleBitmap to CreateDIBSection. ATI 7500 & Intell 740 problem, 
	//when bmp width > 4096
	/*
	HBITMAP hNewBitmap = (HBITMAP) ::CreateCompatibleBitmap (memDCSrc,
									iBitmapWidth + iNewBitmapWidth,
									iBitmapHeight);
	*/

	HBITMAP hNewBitmap = 0;

		byte	*pdibBits = 0;
	{
		BOOL b8Bit = bmp.bmBitsPixel == 8 && bmp.bmPlanes == 1;
		DWORD dwSize = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD);
		LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)new BYTE[dwSize];
		ZeroMemory(lpbi, dwSize);
		lpbi->biBitCount = b8Bit?8:24;
		lpbi->biHeight = iBitmapHeight;
		lpbi->biWidth = iBitmapWidth + iNewBitmapWidth;
		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biPlanes = 1;
		lpbi->biSizeImage=((int)(lpbi->biWidth*lpbi->biBitCount/8+3)/4)*4*lpbi->biHeight;
		if (b8Bit)
		{
			lpbi->biClrUsed = lpbi->biClrImportant = 256;
			::GetDIBColorTable(memDCSrc.GetSafeHdc(),0,256,(RGBQUAD*)(lpbi+1));
		}
		hNewBitmap = ::CreateDIBSection( memDCSrc, (BITMAPINFO*)lpbi, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0 );
		delete lpbi;
		ASSERT( hNewBitmap );
	}	
	

	if (hNewBitmap == NULL)
	{
		memDCSrc.SelectObject (hOldBitmapSrc);
		return -1;
	}

	//------------------------------------------------------
	// Create memory destination DC and select a new bitmap:
	//------------------------------------------------------
	CDC memDCDst;
	VERIFY( memDCDst.CreateCompatibleDC (&memDCSrc) );
	
	HBITMAP hOldBitmapDst = (HBITMAP) memDCDst.SelectObject (hNewBitmap);
	if (hOldBitmapDst == NULL)
	{
		memDCSrc.SelectObject (hOldBitmapSrc);
		::DeleteObject (hNewBitmap);
		return -1;
	}

	if (m_hbmImageWell != NULL)
	{
		//-----------------------------
		// Copy original bitmap to new:
		//-----------------------------
		VERIFY( memDCDst.BitBlt (0, 0, iBitmapWidth, iBitmapHeight,
					&memDCSrc, 0, 0, SRCCOPY) );
	}
		
	//--------------------------------
	// Select a new image and copy it:
	//--------------------------------
	if (memDCSrc.SelectObject (hbmp) == NULL)
	{
		memDCDst.SelectObject (hOldBitmapDst);
		memDCSrc.SelectObject (hOldBitmapSrc);

		::DeleteObject (hNewBitmap);
		return -1;
	}

	VERIFY( memDCDst.BitBlt (iBitmapWidth, 0, iNewBitmapWidth, iBitmapHeight,
				&memDCSrc, 0, 0, SRCCOPY) );

	/*
	
	{
		char sz_buf[1024];
		sprintf( sz_buf, "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!---------%d:%d", iBitmapWidth + iNewBitmapWidth, iBitmapHeight );
		TRACE( sz_buf );

		write_bmp( hNewBitmap, memDCDst.m_hDC, iBitmapWidth + iNewBitmapWidth, iBitmapHeight, (int)this );
	}
	*/

	memDCDst.SelectObject (hOldBitmapDst);
	memDCSrc.SelectObject (hOldBitmapSrc);

	if (m_hbmImageWell != NULL)
		::DeleteObject (m_hbmImageWell);

	m_hbmImageWell = hNewBitmap;
	m_bModified = TRUE;

	UpdateCount ();

	return m_iCount - 1;
}
//*******************************************************************************
BOOL CBCGToolBarImages::UpdateImage (int iImage, HBITMAP hbmp)
{
	if (!m_bUserImagesList)	// Only user images can be edited!
	{
		return FALSE;
	}

	CWindowDC	dc (NULL);
	CBitmap 	bitmap;
	CDC 		memDCSrc;
	CDC 		memDCDst;

	memDCSrc.CreateCompatibleDC (&dc);
	memDCDst.CreateCompatibleDC (&dc);
	
	HBITMAP hOldBitmapDst = (HBITMAP) memDCDst.SelectObject (m_hbmImageWell);
	HBITMAP hOldBitmapSrc = (HBITMAP) memDCSrc.SelectObject (hbmp);

	memDCDst.BitBlt (m_sizeImage.cx * iImage, 0, m_sizeImage.cx, m_sizeImage.cy,
				&memDCSrc, 0, 0, SRCCOPY);

	memDCDst.SelectObject (hOldBitmapDst);
	memDCSrc.SelectObject (hOldBitmapSrc);

	m_bModified = TRUE;
	return TRUE;
}
//*******************************************************************************
BOOL CBCGToolBarImages::DeleteImage (int iImage)
{
	if (!m_bUserImagesList)	// Only user images can be edited!
	{
		return FALSE;
	}

	if (iImage < 0 || iImage >= GetCount ())	// Wrong index
	{
		return FALSE;
	}

	//-------------------------------
	// Get original bitmap attrbutes:
	//-------------------------------
	BITMAP bmp;
	if (::GetObject (m_hbmImageWell, sizeof (BITMAP), &bmp) == 0)
	{
		return -1;
	}

	//-------------------------------------------------------
	// Create memory source DC and select an original bitmap:
	//-------------------------------------------------------
	CDC memDCSrc;
	memDCSrc.CreateCompatibleDC (NULL);

	HBITMAP hOldBitmapSrc = (HBITMAP) memDCSrc.SelectObject (m_hbmImageWell);
	if (hOldBitmapSrc == NULL)
	{
		return -1;
	}

	//----------------------------------------------------------
	// Create a new bitmap compatibel with the source memory DC
	// (original bitmap SHOULD BE ALREADY SELECTED!):
	//----------------------------------------------------------
	HBITMAP hNewBitmap = (HBITMAP) ::CreateCompatibleBitmap (memDCSrc,
									bmp.bmWidth - m_sizeImage.cx,
									bmp.bmHeight);
	if (hNewBitmap == NULL)
	{
		memDCSrc.SelectObject (hOldBitmapSrc);
		return -1;
	}

	//------------------------------------------------------
	// Create memory destination DC and select a new bitmap:
	//------------------------------------------------------
	CDC memDCDst;
	memDCDst.CreateCompatibleDC (&memDCSrc);
	
	HBITMAP hOldBitmapDst = (HBITMAP) memDCDst.SelectObject (hNewBitmap);
	if (hOldBitmapDst == NULL)
	{
		memDCSrc.SelectObject (hOldBitmapSrc);
		::DeleteObject (hNewBitmap);
		return -1;
	}

	//-----------------------------
	// Copy original bitmap to new:
	//-----------------------------

	if (iImage != 0)
	{
		memDCDst.BitBlt (0, 0, 
					m_sizeImage.cx * iImage, bmp.bmHeight,
					&memDCSrc, 0, 0, SRCCOPY);
	}
	
	if (iImage != m_iCount - 1)
	{
		memDCDst.BitBlt (m_sizeImage.cx * iImage, 0, 
					bmp.bmWidth - (m_iCount - iImage - 1) * m_sizeImage.cx, bmp.bmHeight,
					&memDCSrc, 
					m_sizeImage.cx * (iImage + 1), 0, SRCCOPY);
	}

	memDCDst.SelectObject (hOldBitmapDst);
	memDCSrc.SelectObject (hOldBitmapSrc);

	::DeleteObject (m_hbmImageWell);

	m_hbmImageWell = hNewBitmap;
	m_bModified = TRUE;

	UpdateCount ();
	return FALSE;
}
//*******************************************************************************
COLORREF CBCGToolBarImages::MapToSysColor (COLORREF color, BOOL bUseRGBQUAD)
{
	struct COLORMAP
	{
		// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
		DWORD rgbqFrom;
		int iSysColorTo;
	};
	static const COLORMAP sysColorMap[] =
	{
		// mapping from color in DIB to system color
		{ RGB_TO_RGBQUAD(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
		{ RGB_TO_RGBQUAD(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark grey
		{ RGB_TO_RGBQUAD(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright grey
		{ RGB_TO_RGBQUAD(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
	};
	const int nMaps = 4;

	// look for matching RGBQUAD color in original
	for (int i = 0; i < nMaps; i++)
	{
		if (color == sysColorMap[i].rgbqFrom)
		{
			return bUseRGBQUAD ? 
				CLR_TO_RGBQUAD(::GetSysColor(sysColorMap[i].iSysColorTo)) :
				::GetSysColor(sysColorMap[i].iSysColorTo);
		}
	}

	return color;
}
//********************************************************************************
COLORREF CBCGToolBarImages::MapFromSysColor (COLORREF color, BOOL bUseRGBQUAD)
{
	struct COLORMAP
	{
		// use DWORD instead of RGBQUAD so we can compare two RGBQUADs easily
		DWORD rgbTo;
		int iSysColorFrom;
	};
	static const COLORMAP sysColorMap[] =
	{
		// mapping from color in DIB to system color
		{ RGB(0x00, 0x00, 0x00),  COLOR_BTNTEXT },       // black
		{ RGB(0x80, 0x80, 0x80),  COLOR_BTNSHADOW },     // dark grey
		{ RGB(0xC0, 0xC0, 0xC0),  COLOR_BTNFACE },       // bright grey
		{ RGB(0xFF, 0xFF, 0xFF),  COLOR_BTNHIGHLIGHT }   // white
	};
	const int nMaps = 4;

	// look for matching RGBQUAD color in original
	for (int i = 0; i < nMaps; i++)
	{
		COLORREF clrSystem = ::GetSysColor (sysColorMap[i].iSysColorFrom);

		if (bUseRGBQUAD)
		{
			if (color == CLR_TO_RGBQUAD (clrSystem))
			{
				return CLR_TO_RGBQUAD (sysColorMap[i].rgbTo);
			}
		}
		else
		{
			if (/*RGBQUAD_TO_CLR*/ (color) == clrSystem)
			{
				return sysColorMap[i].rgbTo;
			}
		}
	}

	return color;
}
//************************************************************************************
BOOL CBCGToolBarImages::Save (LPCTSTR lpszBmpFileName)
{
	if (!m_bUserImagesList ||	// Only user-defined bitmaps can be saved!
		m_hbmImageWell == NULL)	// Not loaded yet!
	{
		return FALSE;
	}

	if (m_bReadOnly)
	{
		return FALSE;
	}

	CString strFile;
	if (lpszBmpFileName == NULL)
	{
		strFile = m_strUDLPath;
	}
	else
	{
		strFile = lpszBmpFileName;
	}

	if (!m_bModified && strFile == m_strUDLPath)
	{
		return TRUE;
	}

	if (!WriteDIB (strFile, DDBToDIB (m_hbmImageWell, 0)))
	{
		return FALSE;
	}

	m_bModified = FALSE;
	return TRUE;
}
//************************************************************************************
static BOOL WriteDIB( LPCTSTR szFile, HANDLE hDIB)
{
	BITMAPFILEHEADER	hdr;
	LPBITMAPINFOHEADER	lpbi;

	if (!hDIB)
		return FALSE;

	CFile file;
	if( !file.Open (szFile, CFile::modeWrite | CFile::modeCreate))
	{
		return FALSE;
	}

	lpbi = (LPBITMAPINFOHEADER) hDIB;

	int nColors = 1 << lpbi->biBitCount;

	// Fill in the fields of the file header 
	hdr.bfType		= ((WORD) ('M' << 8) | 'B');	// is always "BM"
	hdr.bfSize		= GlobalSize (hDIB) + sizeof( hdr );
	hdr.bfReserved1 	= 0;
	hdr.bfReserved2 	= 0;
	hdr.bfOffBits		= (DWORD) (sizeof( hdr ) + lpbi->biSize +
						nColors * sizeof(RGBQUAD));

	// Write the file header 
	file.Write( &hdr, sizeof(hdr) );

	// Write the DIB header and the bits 
	file.Write( lpbi, GlobalSize(hDIB) );

	return TRUE;
}
//********************************************************************************
static HANDLE DDBToDIB (HBITMAP bitmap, DWORD dwCompression) 
{
	BITMAP			bm;
	BITMAPINFOHEADER	bi;
	LPBITMAPINFOHEADER 	lpbi;
	DWORD			dwLen;
	HANDLE			hDIB;
	HANDLE			handle;
	HDC 			hDC;
	HPALETTE		hPal;

	// The function has no arg for bitfields
	if( dwCompression == BI_BITFIELDS )
		return NULL;

	hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

	// Get bitmap information
	::GetObject(bitmap, sizeof(bm),(LPSTR)&bm);

	// Initialize the bitmapinfoheader
	bi.biSize		= sizeof(BITMAPINFOHEADER);
	bi.biWidth		= bm.bmWidth;
	bi.biHeight 		= bm.bmHeight;
	bi.biPlanes 		= 1;
	bi.biBitCount		= 8;//(WORD) (bm.bmPlanes * bm.bmBitsPixel);
	bi.biCompression	= dwCompression;
	bi.biSizeImage		= 0;
	bi.biXPelsPerMeter	= 0;
	bi.biYPelsPerMeter	= 0;
	bi.biClrUsed		= 0;
	bi.biClrImportant	= 0;

	// Compute the size of the  infoheader and the color table
	int nColors = (1 << bi.biBitCount);
	if( nColors > 256 ) 
		nColors = 0;
	dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

	// We need a device context to get the DIB from
	hDC = ::CreateCompatibleDC (NULL);
	if (hDC == NULL)
	{
		return FALSE;
	}

	HBITMAP bmp = ::CreateBitmap (1, 1, 1, bi.biBitCount, NULL);
	if (bmp == NULL)
	{
		::DeleteDC(hDC);
		return NULL;
	}

	HBITMAP hOldBitmap = (HBITMAP)::SelectObject (hDC, bmp);

	hPal = SelectPalette(hDC,hPal,FALSE);
	RealizePalette(hDC);

	// Allocate enough memory to hold bitmapinfoheader and color table
	hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

	if (!hDIB){
		::SelectPalette(hDC,hPal,FALSE);
		::SelectObject (hDC, hOldBitmap);
		::DeleteObject (bmp);
		::DeleteDC(hDC);
		return NULL;
	}

	lpbi = (LPBITMAPINFOHEADER)hDIB;

	*lpbi = bi;

	// Call GetDIBits with a NULL lpBits param, so the device driver 
	// will calculate the biSizeImage field 
	GetDIBits(hDC, bitmap, 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)lpbi, (DWORD)DIB_RGB_COLORS);

	bi = *lpbi;

	// If the driver did not fill in the biSizeImage field, then compute it
	// Each scan line of the image is aligned on a DWORD (32bit) boundary
	if (bi.biSizeImage == 0){
		bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
						* bi.biHeight;

		// If a compression scheme is used the result may infact be larger
		// Increase the size to account for this.
		if (dwCompression != BI_RGB)
			bi.biSizeImage = (bi.biSizeImage * 3) / 2;
	}

	// Realloc the buffer so that it can hold all the bits
	dwLen += bi.biSizeImage;
	handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE);
	if (handle != NULL)
		hDIB = handle;
	else{
		GlobalFree(hDIB);

		// Reselect the original palette
		SelectPalette(hDC,hPal,FALSE);
		::SelectObject (hDC, hOldBitmap);
		::DeleteObject (bmp);
		::DeleteDC(hDC);
		return NULL;
	}

	// Get the bitmap bits
	lpbi = (LPBITMAPINFOHEADER)hDIB;

	// FINALLY get the DIB
	BOOL bGotBits = GetDIBits( hDC, bitmap,
				0L,				// Start scan line
				(DWORD)bi.biHeight,		// # of scan lines
				(LPBYTE)lpbi 			// address for bitmap bits
				+ (bi.biSize + nColors * sizeof(RGBQUAD)),
				(LPBITMAPINFO)lpbi,		// address of bitmapinfo
				(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

	if( !bGotBits )
	{
		GlobalFree(hDIB);
		
		SelectPalette(hDC,hPal,FALSE);
		::SelectObject (hDC, hOldBitmap);
		::DeleteObject (bmp);
		::DeleteDC(hDC);
		return NULL;
	}

	// Convert color table to the standard 3-d colors:
	DWORD* pColorTable = (DWORD*)(((LPBYTE)lpbi) + (UINT) lpbi->biSize);
	for (int iColor = 0; iColor < nColors; iColor ++)
	{
		pColorTable[iColor] = CBCGToolBarImages::MapFromSysColor (pColorTable[iColor]);
	}

//	::GlobalFree(hDIB);

	SelectPalette(hDC,hPal,FALSE);
	::SelectObject (hDC, hOldBitmap);
	::DeleteObject (bmp);
	::DeleteDC(hDC);

	return hDIB;
}


/////////////////////////////////////////////////////////////////////////////
// CBCGToolBarImages diagnostics

#ifdef _DEBUG
void CBCGToolBarImages::AssertValid() const
{
	CObject::AssertValid();

	ASSERT(m_hbmImageWell != NULL);
}

void CBCGToolBarImages::Dump(CDumpContext& dc) const
{
	CObject::Dump(dc);

	dc << "\nm_bUserImagesList = " << m_bUserImagesList;
	dc << "\nm_sizeImage = " << m_sizeImage;

	if (m_bUserImagesList)
	{
		dc << "\nm_strUDLPath = " << m_strUDLPath;
	}

	if (dc.GetDepth() > 0)
	{
	}

	dc << "\n";
}

#endif

BOOL CBCGToolBarImages::CopyImageToClipboard (int iImage)
{
	CBCGLocalResource locaRes;

	try
	{
		CWindowDC dc (NULL);

		//----------------------
		// Create a bitmap copy:
		//----------------------
		CDC memDCDest;
		memDCDest.CreateCompatibleDC (NULL);
		
		CBitmap bitmapCopy;
		if (!bitmapCopy.CreateCompatibleBitmap (&dc, m_sizeImage.cx, m_sizeImage.cy))
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			return FALSE;
		}

		CBitmap* pOldBitmapDest = memDCDest.SelectObject (&bitmapCopy);

		CBCGDrawState ds;
		PrepareDrawImage (ds, FALSE);
	
		Draw (&memDCDest, 0, 0, iImage);
		EndDrawImage (ds);

		memDCDest.SelectObject (pOldBitmapDest);

		if (!AfxGetMainWnd ()->OpenClipboard ())
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			return FALSE;
		}

		if (!::EmptyClipboard ())
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			::CloseClipboard ();
			return FALSE;
		}


		HANDLE hclipData = ::SetClipboardData (CF_BITMAP, bitmapCopy.Detach ());
		if (hclipData == NULL)
		{
			AfxMessageBox (IDP_BCGBARRES_CANT_COPY_BITMAP);
			TRACE (_T("CBCGToolBar::OnBcgbarresCopyImage error. Error code = %x\n"), GetLastError ());
		}

		::CloseClipboard ();
		return TRUE;
	}
	catch (...)
	{
		AfxMessageBox (IDP_BCGBARRES_INTERLAL_ERROR);
	}

	return FALSE;
}
//****************************************************************************************
void CBCGToolBarImages::Clear ()
{
	//----------------
	// By Guy Hachlili
	//----------------
	m_hInstImageWell = NULL;
	AfxDeleteObject((HGDIOBJ*)&m_hbmImageWell);
	m_hbmImageWell = NULL;

	m_lstOrigResIds.RemoveAll();
	m_mapOrigResOffsets.RemoveAll ();
	m_strUDLPath.Empty();

	m_bUserImagesList = FALSE;
	m_iCount = 0;
	m_bModified = FALSE;
}
//*************************************************************************************
void CBCGToolBarImages::TransparentBlt( HDC hdcDest, int nXDest, int nYDest, int nWidth, 
			int nHeight, CDC* pDcSrc, int nXSrc, int nYSrc,
			COLORREF colorTransparent,
			int nWidthDest/* = -1*/, int nHeightDest/* = -1*/)
{
	CDC dc, memDC, maskDC;
	dc.Attach( hdcDest );
	maskDC.CreateCompatibleDC(&dc);
	CBitmap maskBitmap;

	int cx = nWidthDest == -1 ? nWidth : nWidthDest;
	int cy = nHeightDest == -1 ? nHeight : nHeightDest;
	
	//add these to store return of SelectObject() calls
	CBitmap* pOldMemBmp = NULL;
	CBitmap* pOldMaskBmp = NULL;
	
	memDC.CreateCompatibleDC(&dc);
	CBitmap bmpImage;
	bmpImage.CreateCompatibleBitmap( &dc, cx, cy);
	pOldMemBmp = memDC.SelectObject( &bmpImage );

	if (nWidthDest == -1 || (nWidthDest == nWidth && nHeightDest == nHeight))
	{
		memDC.BitBlt( 0,0,nWidth, nHeight, pDcSrc, nXSrc, nYSrc, SRCCOPY);
	}
	else
	{
		memDC.StretchBlt (0,0, nWidthDest, nHeightDest, pDcSrc, 
						nXSrc, nYSrc, nWidth, nHeight, SRCCOPY);
	}
	
	// Create monochrome bitmap for the mask
	maskBitmap.CreateBitmap (cx, cy, 1, 1, NULL );
	pOldMaskBmp = maskDC.SelectObject( &maskBitmap );
	memDC.SetBkColor( colorTransparent );
	
	// Create the mask from the memory DC
	maskDC.BitBlt (0, 0, cx, cy, &memDC, 0, 0, SRCCOPY);
	
	// Set the background in memDC to black. Using SRCPAINT with black 
	// and any other color results in the other color, thus making 
	// black the transparent color
	memDC.SetBkColor(RGB(0,0,0));
	memDC.SetTextColor(RGB(255,255,255));
	memDC.BitBlt(0, 0, cx, cy, &maskDC, 0, 0, SRCAND);
	
	// Set the foreground to black. See comment above.
	dc.SetBkColor(RGB(255,255,255));
	dc.SetTextColor(RGB(0,0,0));

	dc.BitBlt (nXDest, nYDest, cx, cy, &maskDC, 0, 0, SRCAND);
		
	// Combine the foreground with the background
	dc.BitBlt(nXDest, nYDest, cx, cy, &memDC, 
		0, 0, SRCPAINT);
	
	if (pOldMaskBmp)
		maskDC.SelectObject( pOldMaskBmp );
	if (pOldMemBmp)
		memDC.SelectObject( pOldMemBmp );
	
	dc.Detach();
}
//**********************************************************************************
BOOL CBCGToolBarImages::MapTo3dColors ()
{
	if (m_hbmImageWell == NULL)
	{
		return FALSE;
	}

	//-------------------------------------------------------
	// Create memory source DC and select an original bitmap:
	//-------------------------------------------------------
	CDC memDCSrc;
	memDCSrc.CreateCompatibleDC (NULL);

	HBITMAP hOldBitmapSrc = NULL;

	int iBitmapWidth;
	int iBitmapHeight;

	//-------------------------------
	// Get original bitmap attrbutes:
	//-------------------------------
	BITMAP bmp;
	if (::GetObject (m_hbmImageWell, sizeof (BITMAP), &bmp) == 0)
	{
		return FALSE;
	}

	hOldBitmapSrc = (HBITMAP) memDCSrc.SelectObject (m_hbmImageWell);
	if (hOldBitmapSrc == NULL)
	{
		return FALSE;
	}

	iBitmapWidth = bmp.bmWidth;
	iBitmapHeight = bmp.bmHeight;

	//----------------------------------------------------------
	// Create a new bitmap compatibel with the source memory DC:
	// (original bitmap SHOULD BE ALREADY SELECTED!):
	//----------------------------------------------------------
	HBITMAP hNewBitmap = (HBITMAP) ::CreateCompatibleBitmap (memDCSrc,
									iBitmapWidth,
									iBitmapHeight);
	if (hNewBitmap == NULL)
	{
		memDCSrc.SelectObject (hOldBitmapSrc);
		return -1;
	}

	//------------------------------
	// Create memory destination DC:
	//------------------------------
	CDC memDCDst;
	memDCDst.CreateCompatibleDC (&memDCSrc);

	HBITMAP hOldBitmapDst = (HBITMAP) memDCDst.SelectObject (hNewBitmap);
	if (hOldBitmapDst == NULL)
	{
		memDCSrc.SelectObject (hOldBitmapSrc);
		::DeleteObject (hNewBitmap);
		return FALSE;
	}

	//-----------------------------
	// Copy original bitmap to new:
	//-----------------------------
	memDCDst.BitBlt (0, 0, iBitmapWidth, iBitmapHeight,
				&memDCSrc, 0, 0, SRCCOPY);
		
	//------------------------------------
	// Change a specific colors to system:
	//------------------------------------
	for (int x = 0; x < iBitmapWidth; x ++)
	{
		for (int y = 0; y < iBitmapHeight; y ++)
		{
			COLORREF clrOrig = ::GetPixel (memDCDst, x, y);
			COLORREF clrNew = MapToSysColor (clrOrig);

			if (clrOrig != clrNew)
			{
				::SetPixel (memDCDst, x, y, clrNew);
			}
		}
	}
	
	memDCDst.SelectObject (hOldBitmapDst);
	memDCSrc.SelectObject (hOldBitmapSrc);

	::DeleteObject (m_hbmImageWell);
	m_hbmImageWell = hNewBitmap;

	return TRUE;
}
