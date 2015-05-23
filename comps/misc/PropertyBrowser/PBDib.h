// PBDib.h: interface for the PBDib class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RSDIB_H__239D9024_E22D_11D2_84F5_00C04FD1E2CA__INCLUDED_)
#define AFX_RSDIB_H__239D9024_E22D_11D2_84F5_00C04FD1E2CA__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/* DIB constants */
#define PALVERSION   0x300

/* DIB Macros*/

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))

class PBDib  
{
public:
	HBITMAP CreateBitmapXY(SIZE szBmp);
	HPALETTE ClonePalette();
	HBITMAP CreateBitmap();
	int GetWidth();
	int GetHeight();
	BOOL ReadDIBFile(LPCTSTR szFile);
	PBDib();
	virtual ~PBDib();
protected:
	int PaletteSize(VOID FAR * pv);
	WORD GetNumColors(LPSTR lpbi);
	BOOL CreatePalette();
	void Free();
	HANDLE m_hDIB;
	HPALETTE 	m_hPalette;
};

#endif // !defined(AFX_RSDIB_H__239D9024_E22D_11D2_84F5_00C04FD1E2CA__INCLUDED_)
