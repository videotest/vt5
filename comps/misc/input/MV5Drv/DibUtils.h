#if !defined(__DibUtils_H__)
#define __DibUtils_H__

#include "ImageUtil.h"

/*struct BITMAPINFO256
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[256];
};*/

interface IMV500ProgressNotify
{
	virtual void Start(const char *psz, int nMax) = 0;
	virtual void End() = 0;
	virtual void Notify(int nCur) = 0;
};

void YUVToRGBQUAD(BYTE *pDest, unsigned y, int u, int v);
LPBITMAPINFOHEADER _AllocBitmapInfo(CSize szGrab, bool bGrayScale);
interface IDIBProvider;
LPBITMAPINFOHEADER _AllocBitmapInfo(IDIBProvider *pDIBPrv, CSize szGrab, bool bGrayScale);
inline LPBYTE _GetData(LPBITMAPINFOHEADER lpbi)
{
	if (lpbi->biBitCount == 8)
		return (LPBYTE)(((RGBQUAD *)(lpbi+1))+256);
	else
		return (LPBYTE)(lpbi+1);
};
void _CopyGS8(BOOL bVerticalMirror, BOOL bHorizontalMirror, BYTE *pDst, BYTE *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab);
void _CopyYUV422(BOOL bGrayScale, BOOL bVerticalMirror, BYTE *pDst, WORD *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab);
void _CopyRGB888(BOOL bGrayScale, BOOL bVerticalMirror, BYTE *pDst, BYTE *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab);
void _CopyRGB555(BOOL bGrayScale, BOOL bVerticalMirror, BYTE *pDst, WORD *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab);
void GetFieldFromImage(LPBYTE lpDst, LPBYTE lpSrc, int nField, unsigned nRow, unsigned cy);
void HScaleImage2(LPBYTE lpDst, LPBYTE lpSrc, bool bGrayScale, unsigned cxSrc, unsigned cySrc);
void _YuvCCIR601_422ToY_Vert(BYTE *lpDataGrab, BYTE *lpDataYEnd, DWORD dwRowSrc, DWORD dwRowDst, DWORD cy);
void _YuvCCIR601_422ToRgb888_Vert(BYTE *lpDataGrab, BYTE *lpDataRGBEnd, DWORD dwRowSrc, DWORD dwRowDst, DWORD cx, DWORD cy);
int AVIYuvToRGB888(const char *pszSrc, const char *pszDst, DWORD dwMSecs, CSize szGrab, BOOL bFlipVertically,
	HRESULT &hr, bool bFields, int nScale, IMV500ProgressNotify *pNotify);
int AVIHScale2AndResampleFields(const char *pszSrc, const char *pszDst, HRESULT &hr, CSize &szGrabSrc, int nScale,
	DWORD dwMSecs, IMV500ProgressNotify *pNotify);
CString _TempAVI(const char *pszName);
bool IsEnoughDiskSpace(const char *pszName, DWORD dwFrames, DWORD dwSize, DWORD dwFrames1, DWORD dwSize1);
void DoFreeDir(LPCTSTR lpRec);
void FreePartition(LPCTSTR lpRec);
bool DoIsPartitionFree(LPCTSTR lpRec);
bool IsPartitionFree(LPCTSTR lpRec);
void GetFieldsValues(int &n1, int &n2, LPBYTE lpFrame, LPBYTE &lpField1, LPBYTE &lpField2, unsigned nRow, unsigned cy);








#endif