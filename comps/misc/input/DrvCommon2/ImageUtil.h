#if !defined(__ImageUtil_H__)
#define __ImageUtil_H__

#include "ComDef.h"
#include "Defs.h"
#include "image5.h"
#include <../common2/misc_templ.h>

struct BITMAPINFO256
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
};

bool AttachDIBBits(IImage3 *pimg, BITMAPINFOHEADER *pbi, byte *pbyteDIBBits);

class CConv16To8 : public _ptr_t2<BYTE>
{
public:
	WORD m_wMask;
	void Init(int bpp, int nBrightness, int nContrast, double dGamma);
};
class CConv16To16 : public _ptr_t2<WORD>
{
public:
	WORD m_wMask;
	void Init(int bpp, int nBrightness, int nContrast, double dGamma);
};

void InitConversion(int nMaxColor, int nShift16To16, int nShift16To8, WORD wMask,
	int nBrightness, int nContrast, double dGamma);
// Attach image data in plains for various types of image

void AttachPlanes16ToRGB(IImage3 *pimg, int cx, int cy, LPWORD lpRed, LPWORD lpGreen, LPWORD lpBlue, bool bHorzMirror, bool bVertMirror, CConv16To16 *pConv);
void AttachPlanes16ToGray(IImage3 *pimg, int cx, int cy, LPWORD lpRed, LPWORD lpGreen, LPWORD lpBlue, bool bHorzMirror, bool bVertMirror, CConv16To16 *pConv);
void AttachPlane16ToGray(IImage3 *pimg, int cx, int cy, LPWORD lpGray, bool bHorzMirror, bool bVertMirror, CConv16To16 *pConv);

void ConvertPlanes16ToDIB24(LPBITMAPINFOHEADER lpbi, LPWORD lpRed, LPWORD lpGreen, LPWORD lpBlue, bool bHorzMirror, bool bVertMirror, CConv16To8 *pConv);
void ConvertPlane16ToDIB8(LPBITMAPINFOHEADER lpbi, LPWORD lpGray, bool bHorzMirror, bool bVertMirror, CConv16To8 *pConv);
void ConvertPlanes16ToDIB8(LPBITMAPINFOHEADER lpbi, LPWORD lpRed, LPWORD lpGreen, LPWORD lpBlue, bool bHorzMirror, bool bVertMirror, CConv16To8 *pConv);
void ConvertDIB24ToDIB8(LPBITMAPINFOHEADER lpbiDst, LPBITMAPINFOHEADER lpbiSrc, int nPlane);
//sergey 06/04/06
void ConvertPlane16ToDIB8_Mutech(LPBITMAPINFOHEADER lpbi, LPBYTE lpGray, bool bHorzMirror, bool bVertMirror, CConv16To8 *pConv);
void DisplayImageDataDIB_Mutech(LPBITMAPINFOHEADER lpbi, LPBYTE pSrcG, bool bHMirror,bool bVMirror, CConv16To8 *pConv,LPBYTE dest);
//end


void CalAvrBrightness(LPBITMAPINFOHEADER lpbi, double *pdBriRed, double *pdBriGreen, double *pdBriBlue);
void MapRectOnDIB(LPBITMAPINFOHEADER lpbi, CRect rc, COLORREF clr);
void MakeDummyImage(IUnknown *punkImg, int cx, int cy);
void CalAvrBrightnessFields(LPBITMAPINFOHEADER lpbi, double *pdBriRed1, double *pdBriRed2,
	double *pdBriGreen1, double *pdBriGreen2, double *pdBriBlue1, double *pdBriBlue2);
int CalAvrBrightnessProc(LPBITMAPINFOHEADER lpbi, int nProcBright = 0, int nProcDark = 0);

inline int ImageSize(int cx, int cy, int bpp)
{
	return (((cx*bpp+3)>>2)<<2)*cy;
}

inline DWORD DIBSize(int cx, int cy, int bpp, bool bIncludeHeader)
{
	int nPalSize = bpp==8?256:0;
	DWORD dwRow = (cx*bpp/8+3)/4*4;
	DWORD dwData = dwRow*cy;
	if (!bIncludeHeader) return dwData;
	DWORD dwSize = sizeof(BITMAPINFOHEADER)+nPalSize*sizeof(RGBQUAD)+dwData;
	return dwSize;
}
inline DWORD DIBSize(LPBITMAPINFOHEADER lpbi, bool bIncludeHeader)
{ return DIBSize(lpbi->biWidth, lpbi->biHeight, lpbi->biBitCount, bIncludeHeader);}
inline DWORD DIBPaletteEntries(LPBITMAPINFOHEADER lpbi) {return lpbi->biClrUsed?
	lpbi->biClrUsed:lpbi->biBitCount==8?256:0;}
inline DWORD DIBRowSize(int cx, int bpp)
{
	if (bpp > 8 && bpp <= 16) bpp = 16;
	if (bpp > 24 && bpp <= 48) bpp = 48;
	return ((cx*bpp/8+3)>>2)<<2;
}
inline DWORD DIBRowSize(LPBITMAPINFOHEADER lpbi) {return DIBRowSize(lpbi->biWidth,lpbi->biBitCount);}

inline LPVOID DIBData(LPBITMAPINFOHEADER lpbi) {return ((RGBQUAD*)(lpbi+1))+(lpbi->biClrUsed?
	lpbi->biClrUsed:lpbi->biBitCount==8?256:0);}

inline CSize _MaxClientSize(CWnd *pWnd)
{
	CRect rcClient(0,0,800,600);
	CRect rcWnd(rcClient);
	AdjustWindowRect(rcWnd,GetWindowLong(pWnd->m_hWnd,GWL_STYLE),FALSE);
	CSize sz(GetSystemMetrics(SM_CXFULLSCREEN)-(rcWnd.Width()-rcClient.Width()),
		GetSystemMetrics(SM_CYFULLSCREEN)-(rcWnd.Height()-rcClient.Height()));
	return sz;
}

inline int RectWidth(LPRECT lpRect) {return lpRect->right-lpRect->left;}
inline int RectHeight(LPRECT lpRect) {return lpRect->bottom-lpRect->top;}

inline void InitDIB(LPBITMAPINFOHEADER lpbi, int cx, int cy, int bpp, bool bInitData)
{
	int nPalSize = bpp==8?256:0;
	if (bInitData)
		memset(lpbi, 0, DIBSize(cx, cy, bpp, true));
	else
		memset(lpbi, 0, sizeof(*lpbi));
	lpbi->biBitCount = bpp;
	lpbi->biWidth = cx;
	lpbi->biHeight = cy;
	lpbi->biPlanes = 1;
	lpbi->biClrUsed = lpbi->biClrImportant = nPalSize;
	lpbi->biSize = sizeof(BITMAPINFOHEADER);
	if (lpbi->biClrUsed)
	{
		RGBQUAD *pRGB = (RGBQUAD *)(lpbi+1);
		for (int i = 0; i < (int)lpbi->biClrUsed; i++)
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
	}
}

inline void InitGrayPalette(LPBITMAPINFOHEADER lpbi)
{
	if (lpbi->biClrUsed)
	{
		RGBQUAD *pRGB = (RGBQUAD *)(lpbi+1);
		for (int i = 0; i < (int)lpbi->biClrUsed; i++)
			pRGB[i].rgbBlue = pRGB[i].rgbGreen = pRGB[i].rgbRed = i;
	}
}

void FlipDIB(LPBITMAPINFOHEADER lpbi, LPBYTE lpData, bool bHorz, bool bVert);

class CDib
{
public:
	LPBITMAPINFOHEADER m_lpbi;
	CDib()
	{
		m_lpbi = NULL;
	}
	CDib(LPBITMAPINFOHEADER lpbi)
	{
		m_lpbi = lpbi;
	}
	~CDib()
	{
		delete m_lpbi;
	}
	CDib &operator=(LPBITMAPINFOHEADER lpbi)
	{
		delete m_lpbi;
		m_lpbi = lpbi;
	}
	void InitBitmap(int cx, int cy, int bpp)
	{
		if (m_lpbi) delete m_lpbi;
		m_lpbi = (LPBITMAPINFOHEADER)new BYTE[DIBSize(cx, cy, bpp, true)];
		::InitDIB(m_lpbi, cx, cy, bpp, true);
	}
	void InitGrayPalette()
	{
		if (m_lpbi == NULL) return;
		::InitGrayPalette(m_lpbi);
	}
	LPVOID GetData() {return m_lpbi==NULL?NULL:DIBData(m_lpbi);}
	int cx() {return m_lpbi?m_lpbi->biWidth:0;}
	int cy() {return m_lpbi?m_lpbi->biHeight:0;}
	int bpp() {return m_lpbi?m_lpbi->biBitCount:0;}
	int RowSize() {return m_lpbi?DIBRowSize(m_lpbi):0;}
	int Size(bool bIncludeHeader) {return m_lpbi?::DIBSize(m_lpbi,bIncludeHeader):0;}
	bool IsEmpty() {return m_lpbi==NULL;}
	void Empty()
	{
		delete m_lpbi;
		m_lpbi = NULL;
	}
	void Flip(bool bHorz, bool bVert)
	{
		if (m_lpbi != NULL)
			FlipDIB(m_lpbi, NULL, bHorz, bVert);
	}
};

void CopyPlanes(LPBITMAPINFOHEADER lpbi, LPWORD pSrcB, LPWORD pSrcG, LPWORD pSrcR);

inline int _Bright ( int b, int g, int r )	//функция для получения яркости
{
	return (r*60+g*118+b*22)/200;
}



#endif