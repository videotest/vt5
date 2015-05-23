#include "StdAfx.h"
#include "resource.h"
#include <mv5.h>
#include <vfw.h>
#include <direct.h>
#include "DibUtils.h"
#include "StdProfile.h"
#include "input.h"


/*
 * Convert a YUV colour into 24-bit RGB colour.
 *
 * The input Y is in the range 16..235; the input U and V components
 * are in the range -128..+127. The conversion equations for this are
 * (according to CCIR 601):
 *
 *	R = Y + 1.371 V
 *	G = Y - 0.698 V - 0.336 U
 *	B = Y + 1.732 U
 *
 * To avoid floating point, we scale all values by 1024.
 *
 * The resulting RGB values are in the range 16..235: we return these in a
 * DWORD with 8 bits each for red, green and blue, and 8 bits free at
 * the top.
 */
#define RANGE(x, lo, hi)	max(lo, min(hi, x))

void YUVToRGBQUAD(BYTE *pDest, unsigned y, int u, int v)
{
    int ScaledY = RANGE(y, 16, 235) * 1024;
//    int ScaledY = y * 1024;
    int red1, green1, blue1;
    int red, green, blue;
    red1 = (ScaledY + (1404 * v)) / 1024;
    green1 = (ScaledY - (715 * v) - (344 * u)) / 1024;
    blue1 = (ScaledY + (1774 * u)) / 1024;
//	blue1 = ((blue1+221)*255)/697;
    red = RANGE(red1, 0, 255);
    green = RANGE( green1, 0, 255);
    blue = RANGE( blue1, 0, 255);

	((BYTE *)pDest)[0] = blue;
	((BYTE *)pDest)[1] = green;
	((BYTE *)pDest)[2] = red;
}

LPBITMAPINFOHEADER _AllocBitmapInfo(CSize szGrab, bool bGrayScale)
{
	LPBITMAPINFOHEADER lpbi;
	if (bGrayScale)
	{
		DWORD dwRowSrc = ((szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = ((szGrab.cx+3)>>2)<<2;
		lpbi = (LPBITMAPINFOHEADER)malloc(sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+dwRowDst*szGrab.cy);
		if (!lpbi) return NULL;
		memset(lpbi, 0, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+dwRowDst*szGrab.cy);
		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biWidth = szGrab.cx;
		lpbi->biHeight = szGrab.cy;
		lpbi->biBitCount = 8;
		lpbi->biPlanes = 1;
		lpbi->biCompression = BI_RGB;
		lpbi->biClrUsed = lpbi->biClrImportant = 256;
		RGBQUAD *pQuad = (RGBQUAD *)(lpbi+1);
		for (int i = 0; i < 256; i++)
		{
			pQuad[i].rgbRed = i;
			pQuad[i].rgbGreen = i;
			pQuad[i].rgbBlue = i;
			pQuad[i].rgbReserved = i;
		}
	}
	else
	{
		DWORD dwRowSrc = ((szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = ((szGrab.cx*3+3)>>2)<<2;
		lpbi = (LPBITMAPINFOHEADER)malloc(sizeof(BITMAPINFOHEADER)+dwRowDst*szGrab.cy);
		if (!lpbi) return NULL;
		memset(lpbi, 0, sizeof(BITMAPINFOHEADER)+dwRowDst*szGrab.cy);
		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biWidth = szGrab.cx;
		lpbi->biHeight = szGrab.cy;
		lpbi->biBitCount = 24;
		lpbi->biPlanes = 1;
		lpbi->biCompression = BI_RGB;
	}
	return lpbi;
}

LPBITMAPINFOHEADER _AllocBitmapInfo(IDIBProvider *pDIBPrv, CSize szGrab, bool bGrayScale)
{
	LPBITMAPINFOHEADER lpbi;
	if (bGrayScale)
	{
		DWORD dwRowSrc = ((szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = ((szGrab.cx+3)>>2)<<2;
		DWORD dwAllocSize = sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+dwRowDst*szGrab.cy;
		lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(dwAllocSize);
		if (!lpbi) return NULL;
		memset(lpbi, 0, sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+dwRowDst*szGrab.cy);
		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biWidth = szGrab.cx;
		lpbi->biHeight = szGrab.cy;
		lpbi->biBitCount = 8;
		lpbi->biPlanes = 1;
		lpbi->biCompression = BI_RGB;
		lpbi->biClrUsed = lpbi->biClrImportant = 256;
		RGBQUAD *pQuad = (RGBQUAD *)(lpbi+1);
		for (int i = 0; i < 256; i++)
		{
			pQuad[i].rgbRed = i;
			pQuad[i].rgbGreen = i;
			pQuad[i].rgbBlue = i;
			pQuad[i].rgbReserved = i;
		}
	}
	else
	{
		DWORD dwRowSrc = ((szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = ((szGrab.cx*3+3)>>2)<<2;
		DWORD dwAllocSize = sizeof(BITMAPINFOHEADER)+dwRowDst*szGrab.cy;
		lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(dwAllocSize);
		if (!lpbi) return NULL;
		memset(lpbi, 0, sizeof(BITMAPINFOHEADER)+dwRowDst*szGrab.cy);
		lpbi->biSize = sizeof(BITMAPINFOHEADER);
		lpbi->biWidth = szGrab.cx;
		lpbi->biHeight = szGrab.cy;
		lpbi->biBitCount = 24;
		lpbi->biPlanes = 1;
		lpbi->biCompression = BI_RGB;
	}
	return lpbi;
}

void _CopyGS8(BOOL bVerticalMirror, BOOL bHorizontalMirror, BYTE *pDst, BYTE *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab)
{
	if (bHorizontalMirror)
	{
		long lRowSrc = bVerticalMirror?-(long)dwRowSrc:dwRowSrc;
		if (bVerticalMirror)
			pSrc += (dwRowSrc)*(szGrab.cy-1);
		for (int y = 0; y < szGrab.cy; y++)
		{
			for (int x = 0; x < szGrab.cx; x++)
				pDst[x] = pSrc[szGrab.cx-1-x];
			pSrc += lRowSrc;
			pDst += dwRowDst;
		}
	}
	else
	{
		if (bVerticalMirror)
		{
			pSrc += (dwRowSrc)*(szGrab.cy-1);
			for (int y = 0; y < szGrab.cy; y++)
			{
				memcpy(pDst, pSrc, szGrab.cx);
				pSrc -= dwRowSrc;
				pDst += dwRowDst;
			}
		}
		else
			memcpy(pDst, pSrc, dwRowSrc*szGrab.cy);
	}
}

void _CopyYUV422(BOOL bGrayScale, BOOL bVerticalMirror, BYTE *pDst, WORD *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab)
{
	if (bGrayScale)
	{
		if (bVerticalMirror)
			pSrc += (dwRowSrc/2)*(szGrab.cy-1);
		for (int y = 0; y < szGrab.cy; y++)
		{
			for (int x = 0; x < szGrab.cx; x++)
			{
				WORD w = pSrc[x];
				pDst[x] = (BYTE)w;
			}
			if (bVerticalMirror)
				pSrc -= dwRowSrc/2;
			else
				pSrc += dwRowSrc/2;
			pDst += dwRowDst;
		}
	}
	else
	{
		if (bVerticalMirror)
			pSrc += (dwRowSrc/2)*(szGrab.cy-1);
		for (int y = 0; y < szGrab.cy; y++)
		{
			for (int x = 0; x < szGrab.cx; x+=2)
			{
				WORD w1 = pSrc[x];
				WORD w2 = pSrc[x+1];
				unsigned y1,y2;
				int u,v;
				y1 = w1&0xFF;
				u  = (w1>>8)-128;
				y2 = w2&0xFF;
				v  = (w2>>8)-128;
				YUVToRGBQUAD(pDst+3*x,y1,u,v);
				YUVToRGBQUAD(pDst+3*x+3,y2,u,v);
			}
			if (bVerticalMirror)
				pSrc -= dwRowSrc/2;
			else
				pSrc += dwRowSrc/2;
			pDst += dwRowDst;
		}
	}
}

void _CopyRGB888(BOOL bGrayScale, BOOL bVerticalMirror, BYTE *pDst, BYTE *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab)
{
	if (bGrayScale)
	{
		if (bVerticalMirror)
			pSrc += (dwRowSrc)*(szGrab.cy-1);
		for (int y = 0; y < szGrab.cy; y++)
		{
			for (int x = 0; x < szGrab.cx; x++)
			{
				pDst[x] = pSrc[3*x];
			}
			if (bVerticalMirror)
				pSrc -= dwRowSrc;
			else
				pSrc += dwRowSrc;
			pDst += dwRowDst;
		}
	}
	else
	{
		if (bVerticalMirror)
		{
			pSrc += (dwRowSrc)*(szGrab.cy-1);
			for (int y = 0; y < szGrab.cy; y++)
			{
				memcpy(pDst, pSrc, dwRowSrc);
				pSrc -= dwRowSrc;
				pDst += dwRowDst;
			}
		}
		else
			memcpy(pDst, pSrc, dwRowSrc*szGrab.cy);
	}
}

void _CopyRGB555(BOOL bGrayScale, BOOL bVerticalMirror, BYTE *pDst, WORD *pSrc, DWORD dwRowDst, DWORD dwRowSrc, CSize szGrab)
{
	if (bGrayScale)
	{
		if (bVerticalMirror)
			pSrc += (dwRowSrc/2)*(szGrab.cy-1);
		for (int y = 0; y < szGrab.cy; y++)
		{
			for (int x = 0; x < szGrab.cx; x++)
			{
				WORD w = pSrc[x];
				pDst[x] = (BYTE)((w>>7)&0xFF);
			}
			if (bVerticalMirror)
				pSrc -= dwRowSrc/2;
			else
				pSrc += dwRowSrc/2;
			pDst += dwRowDst;
		}
	}
	else
	{
		if (bVerticalMirror)
			pSrc += (dwRowSrc/2)*(szGrab.cy-1);
		for (int y = 0; y < szGrab.cy; y++)
		{
			for (int x = 0; x < szGrab.cx; x++)
			{
				WORD w = pSrc[x];
				pDst[3*x+2] = (BYTE)((w>>7)&0xFF);
				pDst[3*x+1] = (BYTE)((w>>2)&0xFF);
				pDst[3*x+0] = (BYTE)((w<<3)&0xFF);
			}
			if (bVerticalMirror)
				pSrc -= dwRowSrc/2;
			else
				pSrc += dwRowSrc/2;
			pDst += dwRowDst;
		}
	}
}

void GetFieldFromImage(LPBYTE lpDst, LPBYTE lpSrc, int nField, unsigned nRow, unsigned cy)
{
	int cy2 = cy/2;
	if (nField)
		lpSrc += nRow*cy2;
	BOOL bCorrect;
	if (nField)
		bCorrect = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("CorrectEvenFieldVertically"), TRUE, true);
	else
		bCorrect = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("CorrectOddFieldVertically"), FALSE, true);
	if (bCorrect)
	{
		memcpy(lpDst, lpSrc, nRow);
		lpSrc += nRow;
		lpDst += nRow;
		for (int i = 1; i < cy2-1; i++)
		{
			memcpy(lpDst, lpSrc, nRow);
			memcpy(lpDst+nRow, lpSrc, nRow);
			lpSrc += nRow;
			lpDst += 2*nRow;
		}
		memcpy(lpDst, lpSrc, nRow);
		memcpy(lpDst+nRow, lpSrc, nRow);
		memcpy(lpDst+2*nRow, lpSrc, nRow);
	}
	else
	{
		for (int i = 0; i < cy2; i++)
		{
			memcpy(lpDst, lpSrc, nRow);
			memcpy(lpDst+nRow, lpSrc, nRow);
			lpSrc += nRow;
			lpDst += 2*nRow;
		}
	}
}

void HScaleImage2(LPBYTE lpDst, LPBYTE lpSrc, bool bGrayScale, unsigned cxSrc, unsigned cySrc)
{
	unsigned cyDst = cySrc/2;
	unsigned cxDst = cxSrc/2;
	unsigned nRowSrc = bGrayScale?((cxSrc+3)>>2)<<2:((3*cxSrc+3)>>2)<<2;
	unsigned nRowDst = bGrayScale?((cxDst+3)>>2)<<2:((3*cxDst+3)>>2)<<2;
	for (unsigned y = 0; y < cyDst; y++)
	{
		LPBYTE p1 = lpSrc;
		LPBYTE p2 = lpDst;
		for (unsigned x = 0; x < cxDst; x++)
		{
			if (bGrayScale)
			{
				*p2++ = *p1++;
				p1++;
			}
			else
			{
				*p2++ = *p1++;
				*p2++ = *p1++;
				*p2++ = *p1++;
				p1 += 3;
			}
		}
		lpSrc += nRowSrc;
		lpDst += nRowDst;
	}

}

void _YuvCCIR601_422ToY_Vert(BYTE *lpDataGrab, BYTE *lpDataYEnd, DWORD dwRowSrc, DWORD dwRowDst, DWORD cy)
{
	for (unsigned i = 0; i < cy; i++)
	{
		MV5YuvCCIR601_422ToY(lpDataGrab, lpDataYEnd, dwRowDst);
		lpDataYEnd -= dwRowDst;
		lpDataGrab += dwRowSrc;
	}
}

void _YuvCCIR601_422ToRgb888_Vert(BYTE *lpDataGrab, BYTE *lpDataRGBEnd, DWORD dwRowSrc, DWORD dwRowDst, DWORD cx, DWORD cy)
{
	for (unsigned i = 0; i < cy; i++)
	{
		MV5YuvCCIR601_422ToRgb888(lpDataGrab, lpDataRGBEnd, cx);
		lpDataRGBEnd -= dwRowDst;
		lpDataGrab += dwRowSrc;
	}
}

// .avi on source is ALREADY 16-bit YUV.
int AVIYuvToRGB888(const char *pszSrc, const char *pszDst, DWORD dwMSecs, CSize szGrab, BOOL bFlipVertically,
	HRESULT &hr, bool bFields, int nScale, IMV500ProgressNotify *pNotify)
{
	PAVIFILE pfSrc;
	PAVIFILE pfDst;
	// Create destination file
	hr = AVIFileOpen(&pfDst, pszDst, OF_WRITE|OF_CREATE, NULL);
	if (FAILED(hr)) return 1;
	// Open source file
	hr = AVIFileOpen(&pfSrc, pszSrc, OF_READ, NULL);
	if (FAILED(hr))
	{
		AVIFileClose(pfDst);
		unlink(pszDst);
		return 2;
	}
	// Create destination stream
	AVISTREAMINFO strhdr;
	PAVISTREAM psDst;
	memset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType = streamtypeVIDEO;
	strhdr.dwScale = dwMSecs*1000; 
	strhdr.dwRate = 1000000;
	strhdr.dwQuality = 10000;
	SetRect(&strhdr.rcFrame, 0, 0, (int)szGrab.cx, (int)szGrab.cy); 
	hr = AVIFileCreateStream(pfDst, &psDst, &strhdr);
	if (FAILED(hr))
	{
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		unlink(pszDst);
		return 3;
	}
	// Open source stream
	PAVISTREAM psSrc;
	hr = AVIFileGetStream(pfSrc, &psSrc, streamtypeVIDEO, 0);
	if (FAILED(hr))
	{
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		unlink(pszDst);
		return 4;
	}
	// Set destination format
	BITMAPINFOHEADER bi;
	memset(&bi, 0, sizeof(bi));
	bi.biSize = sizeof(bi);
	bi.biPlanes = 1;
	bi.biCompression = BI_RGB;
	bi.biBitCount = 24;
	bi.biWidth = bFields&&nScale?szGrab.cx/2:szGrab.cx;
	bi.biHeight = bFields&&nScale?szGrab.cy/2:szGrab.cy;
	hr = AVIStreamSetFormat(psDst, 0, &bi, sizeof(bi)); 
	if (FAILED(hr))
	{
		AVIStreamRelease(psSrc);
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		return 5;
	}
	// Allocate buffers for decompression
	DWORD dwSizeSrc = (((2*szGrab.cx+1)>>1)<<1)*szGrab.cy;
	DWORD dwSizeDst = (((3*szGrab.cx+3)>>2)<<2)*szGrab.cy; // Before decomposition fields to frames
	DWORD dwSizeDstFld = (((3*bi.biWidth+3)>>2)<<2)*bi.biHeight; // After decomposition fields to frames
	LPVOID lpDataDst = malloc(dwSizeDst);
	if (!lpDataDst)
	{
		AVIStreamRelease(psSrc);
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		return 6;
	}
	LPVOID lpDataSrc = malloc(dwSizeSrc);
	if (!lpDataSrc)
	{
		free(lpDataDst);
		AVIStreamRelease(psSrc);
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		return 7;
	}
	LPVOID lpDataDst1 = NULL;
	if (bFields)
	{
		lpDataDst1 = malloc(dwSizeDstFld);
		if (!lpDataDst1)
		{
			free(lpDataDst);
			free(lpDataSrc);
			AVIStreamRelease(psSrc);
			AVIStreamRelease(psDst);
			AVIFileClose(pfSrc);
			AVIFileClose(pfDst);
			return 8;
		}
	}
	// These values will be used only for vertical mirroring
	DWORD dwRowSrc = (((2*szGrab.cx+1)>>1)<<1);
	DWORD dwRowDst = (((3*szGrab.cx+3)>>2)<<2);
	LPBYTE lpDataDstEnd = ((LPBYTE)lpDataDst)+dwRowDst*(szGrab.cy-1);
	// Proceess decompression on frame basis
    // Read the stream data using AVIStreamRead. 
	LONG lStreamStart = AVIStreamStart(psSrc);
	LONG lStreamEnd = AVIStreamEnd(psSrc);
	pNotify->Start("Decompress",bFields?2*lStreamEnd:lStreamEnd);
	for (LONG lStreamSize = lStreamStart; lStreamSize < lStreamEnd; lStreamSize++)
	{ 
		hr = AVIStreamRead(psSrc, lStreamSize, 1, lpDataSrc, dwSizeSrc, NULL, NULL);
		if (SUCCEEDED(hr))
		{
			// First, convert image data from YUV to RGB and, possibly, flip vertically
			if (bFlipVertically)
				_YuvCCIR601_422ToRgb888_Vert((LPBYTE)lpDataSrc, lpDataDstEnd, dwRowSrc, dwRowDst, szGrab.cx, szGrab.cy);
			else
				MV5YuvCCIR601_422ToRgb888(lpDataSrc, lpDataDst, szGrab.cx*szGrab.cy);
			// Decompose fields to separate frames, if needed
			if (bFields)
			{
				if (nScale)
				{
					HScaleImage2((LPBYTE)lpDataDst1, bFlipVertically?(LPBYTE)lpDataDst+dwRowDst*(szGrab.cy/2):(LPBYTE)lpDataDst,
						false, szGrab.cx, szGrab.cy);
					hr = AVIStreamWrite(psDst, 2*lStreamSize, 1, lpDataDst1, dwSizeDstFld, AVIIF_KEYFRAME, NULL, NULL);
					HScaleImage2((LPBYTE)lpDataDst1, bFlipVertically?(LPBYTE)lpDataDst:(LPBYTE)lpDataDst+dwRowDst*(szGrab.cy/2),
						false, szGrab.cx, szGrab.cy);
					hr = AVIStreamWrite(psDst, 2*lStreamSize+1, 1, lpDataDst1, dwSizeDstFld, AVIIF_KEYFRAME, NULL, NULL);
				}
				else
				{
					GetFieldFromImage((LPBYTE)lpDataDst1, (LPBYTE)lpDataDst, bFlipVertically?1:0, dwRowDst, szGrab.cy);
					hr = AVIStreamWrite(psDst, 2*lStreamSize, 1, lpDataDst1, dwSizeDstFld, AVIIF_KEYFRAME, NULL, NULL);
					GetFieldFromImage((LPBYTE)lpDataDst1, (LPBYTE)lpDataDst, bFlipVertically?0:1, dwRowDst, szGrab.cy);
					hr = AVIStreamWrite(psDst, 2*lStreamSize+1, 1, lpDataDst1, dwSizeDstFld, AVIIF_KEYFRAME, NULL, NULL);
				}
			}
			else
				// Save the decompressed data using AVIStreamWrite. 
				hr = AVIStreamWrite(psDst, lStreamSize, 1, lpDataDst, dwSizeDst, AVIIF_KEYFRAME, NULL, NULL);
		}
		pNotify->Notify(bFields?2*lStreamSize:lStreamSize);
	}
	pNotify->End();
	// free buffers and close resources
	if (lpDataDst1) free(lpDataDst1);
	free(lpDataDst);
	free(lpDataSrc);
	AVIStreamRelease(psSrc);
	AVIStreamRelease(psDst);
	AVIFileClose(pfSrc);
	AVIFileClose(pfDst);
	return 0;
}

CString _TempAVI(const char *pszName)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szTemp[_MAX_PATH];
	_splitpath(pszName, szDrive, szDir, NULL, NULL);
	_makepath(szTemp, szDrive, szDir, "$$$temp.avi", NULL);
	return CString(szTemp);
}

bool IsEnoughDiskSpace(const char *pszName, DWORD dwFrames, DWORD dwSize, DWORD dwFrames1, DWORD dwSize1)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szTemp[_MAX_PATH];
	_splitpath(pszName, szDrive, szDir, NULL, NULL);
	_makepath(szTemp, szDrive, szDir, NULL, NULL);
	ULARGE_INTEGER uliAvail;
	ULARGE_INTEGER uliTotal;
	if (GetDiskFreeSpaceEx(szTemp, &uliAvail, &uliTotal, NULL))
	{
		ULONGLONG uli = UInt32x32To64(dwFrames,dwSize);
		if (uli> 0x80000000)
		{
			AfxMessageBox(IDS_FILE_MORE_2GB, MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
		if (dwFrames1 != 0)
		{
			ULONGLONG uli1 = UInt32x32To64(dwFrames1,dwSize1);
			if (uli1> 0x80000000)
			{
				AfxMessageBox(IDS_FILE_MORE_2GB, MB_OK|MB_ICONEXCLAMATION);
				return false;
			}
			uli += uli1;
		}
		if (uli >= uliAvail.QuadPart)
		{
			CString s;
			s.Format(IDS_NOT_ENOUGH_SPACE, szTemp, (int)(uliAvail.QuadPart/1024/1024), (int)(uli/1024/1024));
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
	}
	return true;
}

void DoFreeDir(LPCTSTR lpRec)
{
	_finddata_t fd;
	long l = _tfindfirst(lpRec, &fd);
	if (l == -1) return;
	CStringArray saFiles;
	CStringArray saDirs;
	while (1)
	{
		if (fd.attrib & _A_SUBDIR)
			saDirs.Add(fd.name);
		else
			saFiles.Add(fd.name);
		if (_tfindnext(l, &fd) == -1)
			break;
	}
	_findclose(l);
	for (int i = 0; i < saFiles.GetSize(); i++)
	{
		CString s(saFiles[i]);
		_unlink(s);
	}
	for (i = 0; i < saDirs.GetSize(); i++)
	{
		CString s(saDirs[i]);
		TCHAR szBuf[260];
		_tcscpy(szBuf, s);
		_tcscpy(szBuf, _T("\\*"));
		DoFreeDir(szBuf);
		_rmdir(s);
	}
}

void FreePartition(LPCTSTR lpRec)
{
	TCHAR szSpec[50];
	szSpec[0] = lpRec[0];
	szSpec[1] = 0;
	_tcscat(szSpec, _T(":\\*"));
	DoFreeDir(szSpec);
}

bool DoIsPartitionFree(LPCTSTR lpRec)
{
	TCHAR szSpec[50];
	szSpec[0] = lpRec[0];
	szSpec[1] = 0;
	_tcscat(szSpec, _T(":\\*"));
	_finddata_t fd;
	long l = _tfindfirst(szSpec, &fd);
	if (l == -1) return true;
	bool bRet = true;
	while (1)
	{
		if (_tcsicmp(lpRec, fd.name))
		{
			bRet = false;
			break;
		}
		if (_tfindnext(l, &fd) == -1)
			break;
	}
	_findclose(l);
	return bRet;
}

bool IsPartitionFree(LPCTSTR lpRec)
{
	bool b = DoIsPartitionFree(lpRec);
	if (b)
		return true;
	if (!CStdProfileManager::m_pMgr->GetProfileInt("Settings", "AutomaticallyFreePartition", false, true))
	{
		CString s;
		s.Format(IDS_PARTITION_NOT_FREE, int(lpRec[0]));
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	FreePartition(lpRec);
	b = DoIsPartitionFree(lpRec);
	if (!b)
	{
		CString s;
		s.Format(IDS_CANNOT_FREE_PARTITION, int(lpRec[0]));
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
	}
	return b;
}

void GetFieldsValues(int &n1, int &n2, LPBYTE lpFrame, LPBYTE &lpField1, LPBYTE &lpField2, unsigned nRow, unsigned cy)
{
	BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	BOOL bSwapFields = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("SwapFields"), TRUE, true);
	LPBYTE p = (LPBYTE)lpFrame+nRow*(cy/2);
	if (bSwapFields)
	{
		n2 = bVerticalMirror?0:1;
		n1 = bVerticalMirror?1:0;
		lpField2 = bVerticalMirror?(LPBYTE)lpFrame:p;
		lpField1 = bVerticalMirror?p:(LPBYTE)lpFrame;
	}
	else
	{
		n1 = bVerticalMirror?0:1;
		n2 = bVerticalMirror?1:0;
		lpField1 = bVerticalMirror?(LPBYTE)lpFrame:p;
		lpField2 = bVerticalMirror?p:(LPBYTE)lpFrame;
	}
}

// Source .avi contains fielded grayscale avi with rate 25 fps.
int AVIHScale2AndResampleFields(const char *pszSrc, const char *pszDst, HRESULT &hr, CSize &szGrabSrc, int nScale,
	DWORD dwMSecs, IMV500ProgressNotify *pNotify)
{
	PAVIFILE pfSrc;
	PAVIFILE pfDst;
	// Create destination file
	hr = AVIFileOpen(&pfDst, pszDst, OF_WRITE|OF_CREATE, NULL);
	if (FAILED(hr)) return 1;
	// Open source file
	hr = AVIFileOpen(&pfSrc, pszSrc, OF_READ, NULL);
	if (FAILED(hr))
	{
		AVIFileClose(pfDst);
		unlink(pszDst);
		return 2;
	}
	// Create destination stream
	AVISTREAMINFO strhdr;
	PAVISTREAM psDst;
	memset(&strhdr, 0, sizeof(strhdr));
	strhdr.fccType = streamtypeVIDEO;
	strhdr.dwScale = dwMSecs*1000; 
	strhdr.dwRate = 1000000;
//	strhdr.dwScale = 20*1000; 
//	strhdr.dwRate = 1000000;
	strhdr.dwQuality = 10000;
	SetRect(&strhdr.rcFrame, 0, 0, (int)szGrabSrc.cx, (int)szGrabSrc.cy);
	hr = AVIFileCreateStream(pfDst, &psDst, &strhdr);
	if (FAILED(hr))
	{
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		unlink(pszDst);
		return 3;
	}
	// Open source stream
	PAVISTREAM psSrc;
	hr = AVIFileGetStream(pfSrc, &psSrc, streamtypeVIDEO, 0);
	if (FAILED(hr))
	{
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		unlink(pszDst);
		return 4;
	}
	// Set destination format
	BITMAPINFO256 bi;
	memset(&bi, 0, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biCompression = BI_RGB;
	bi.bmiHeader.biBitCount = 8;
	bi.bmiHeader.biWidth = nScale?szGrabSrc.cx/2:szGrabSrc.cx;
	bi.bmiHeader.biHeight = nScale?szGrabSrc.cy/2:szGrabSrc.cy;
	bi.bmiHeader.biClrUsed = bi.bmiHeader.biClrImportant = 256;
	for (int i = 0; i < 256; i++)
	{
		bi.bmiColors[i].rgbBlue = i;
		bi.bmiColors[i].rgbRed = i;
		bi.bmiColors[i].rgbGreen = i;
		bi.bmiColors[i].rgbReserved = 0;
	}
	hr = AVIStreamSetFormat(psDst, 0, (LPBITMAPINFOHEADER)&bi, sizeof(bi)); 
	if (FAILED(hr))
	{
		AVIStreamRelease(psSrc);
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		return 5;
	}
	// Allocate buffers for decompression. Remember that avi always grayscale.
	DWORD dwRowSrc = ((szGrabSrc.cx+3)>>2)<<2;
	DWORD dwSizeSrc = dwRowSrc*szGrabSrc.cy;
	DWORD dwRowDst = ((bi.bmiHeader.biWidth+3)>>2)<<2;
	DWORD dwSizeDst = dwRowDst*bi.bmiHeader.biHeight;
	LPVOID lpDataDst = malloc(dwSizeDst);
	if (!lpDataDst)
	{
		AVIStreamRelease(psSrc);
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		return 6;
	}
	LPVOID lpDataSrc = malloc(dwSizeSrc);
	if (!lpDataSrc)
	{
		free(lpDataDst);
		AVIStreamRelease(psSrc);
		AVIStreamRelease(psDst);
		AVIFileClose(pfSrc);
		AVIFileClose(pfDst);
		return 7;
	}
	// These variables will be used during fields decomposition
//	BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	int n1,n2;
	LPBYTE lpField1,lpField2;
	GetFieldsValues(n1, n2, (LPBYTE)lpDataSrc, lpField1, lpField2, dwRowSrc, szGrabSrc.cy);
	// Proceess decompression on frame basis
    // Read the stream data using AVIStreamRead. 
	LONG lStreamStart = AVIStreamStart(psSrc);
	LONG lStreamEnd = AVIStreamEnd(psSrc);
	pNotify->Start("Decompress",2*lStreamEnd);
	for (LONG lStreamSize = lStreamStart; lStreamSize < lStreamEnd; lStreamSize++)
	{ 
		hr = AVIStreamRead(psSrc, lStreamSize, 1, lpDataSrc, dwSizeSrc, NULL, NULL);
		if (SUCCEEDED(hr))
		{
			if (nScale)
			{
				HScaleImage2((LPBYTE)lpDataDst, lpField1, true, szGrabSrc.cx, szGrabSrc.cy);
				hr = AVIStreamWrite(psDst, 2*lStreamSize, 1, lpDataDst, dwSizeDst, AVIIF_KEYFRAME, NULL, NULL);
				HScaleImage2((LPBYTE)lpDataDst, lpField2, true, szGrabSrc.cx, szGrabSrc.cy);
				hr = AVIStreamWrite(psDst, 2*lStreamSize+1, 1, lpDataDst, dwSizeDst, AVIIF_KEYFRAME, NULL, NULL);
			}
			else
			{
				GetFieldFromImage((LPBYTE)lpDataDst, (LPBYTE)lpDataSrc, n1, dwRowSrc, szGrabSrc.cy);
				hr = AVIStreamWrite(psDst, 2*lStreamSize, 1, lpDataDst, dwSizeDst, AVIIF_KEYFRAME, NULL, NULL);
				GetFieldFromImage((LPBYTE)lpDataDst, (LPBYTE)lpDataSrc, n2, dwRowSrc, szGrabSrc.cy);
				hr = AVIStreamWrite(psDst, 2*lStreamSize+1, 1, lpDataDst, dwSizeDst, AVIIF_KEYFRAME, NULL, NULL);
			}
		}
		pNotify->Notify(2*lStreamSize);
	}
	pNotify->End();
	// free buffers and close resources
	free(lpDataDst);
	free(lpDataSrc);
	AVIStreamRelease(psSrc);
	AVIStreamRelease(psDst);
	AVIFileClose(pfSrc);
	AVIFileClose(pfDst);
	return 0;
}






