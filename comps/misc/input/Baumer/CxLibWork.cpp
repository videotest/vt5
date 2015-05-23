#include "StdAfx.h"
#include "Baumer.h"
#include "CxLibWork.h"
#include "ImLibApi.h"
#include "Input.h"
#include "BaumUtils.h"
#include <math.h>
#include <misc_utils.h>

CBaumerProfile g_BaumerProfile;
CCxLibWork g_CxLibWork;

bool IsDC300(int type )
{
	return type == DC300 || type == DXM33 || type == ARC6000C;
}

bool GainEnabled(int nCamera)
{
	return true;//nCamera==CX13||nCamera==CX13C||nCamera==MX13||nCamera==MX13C;
}

bool BinningGluck(int nCamera)
{
	return g_CxLibWork.m_bBinningGluck && (g_CxLibWork.m_nCamera==CX05||
		g_CxLibWork.m_nCamera==CX13||g_CxLibWork.m_nCamera==CX05C||
		g_CxLibWork.m_nCamera==CX13C||g_CxLibWork.m_nCamera==MX05||
		g_CxLibWork.m_nCamera==MX13||g_CxLibWork.m_nCamera==MX05C||
		g_CxLibWork.m_nCamera==MX13C);
}

bool Is12BitCamera(int nCamera)
{
	return nCamera==ARC1000||nCamera==ARC1000C||nCamera==ARC1800||nCamera==ARC1800C||
		nCamera==ARC4000||nCamera==ARC4000C||nCamera==ARC6000C;
}

bool IsInputAdvanced()
{
	return g_CxLibWork.m_nBitsMode != W8BIT &&
		(g_CxLibWork.m_nConversion == ManualConversion ||
		g_CxLibWork.m_nConversion == AutoConversion && g_CxLibWork.m_CameraStatus.bBinning && BinningGluck(g_CxLibWork.m_nCamera)||
		g_CxLibWork.m_nConversion == NoConversion && (g_CxLibWork.m_bForCapture)) ||
		g_CxLibWork.m_nBitsMode == W8BIT && g_CxLibWork.m_CameraStatus.bBinning && BinningGluck(g_CxLibWork.m_nCamera) ||
		IsDC300(g_CxLibWork.m_nCamera) ||
		g_CxLibWork.m_nGrayMode != NoneGrayMode;
}

bool IsManualConvert() // valid only if IsInput16() == true
{
	return g_CxLibWork.m_nBitsMode == W8BIT ||
		g_CxLibWork.m_nConversion == ManualConversion ||
		g_CxLibWork.m_nConversion == AutoConversion ||
		g_CxLibWork.m_nConversion == NoConversion && (BinningGluck(g_CxLibWork.m_nCamera))||
		IsDC300(g_CxLibWork.m_nCamera);
}

bool Arc6000Gluck()
{
	return false;
//	return g_CxLibWork.m_nBitsMode != W8BIT;
}

bool Arc6000Gluck1()
{
	return g_CxLibWork.m_bARC6000Gluck1 && g_CxLibWork.IsDC300() && g_CxLibWork.m_nBitsMode != W8BIT;
}

bool Arc6000Gluck2()
{
	return g_CxLibWork.m_bARC6000Gluck2 && g_CxLibWork.IsDC300();
}

typedef void (*ConvertData8)(LPBYTE lpDst, LPBYTE lpSrc);
void ConvertDataRed8(LPBYTE lpDst, LPBYTE lpSrc) { lpDst[0] = lpSrc[2];}
void ConvertDataGreen8(LPBYTE lpDst, LPBYTE lpSrc) { lpDst[0] = lpSrc[1];}
void ConvertDataBlue8(LPBYTE lpDst, LPBYTE lpSrc) { lpDst[0] = lpSrc[0];}
void ConvertDataGray8(LPBYTE lpDst, LPBYTE lpSrc) { lpDst[0] = BaumUtils::Bright(int(lpSrc[2]),int(lpSrc[1]),int(lpSrc[0]));}
typedef void (*ConvertData16)(LPWORD lpDst, LPWORD lpSrc);
void ConvertDataRed16(LPWORD lpDst, LPWORD lpSrc) { lpDst[0] = lpSrc[2];}
void ConvertDataGreen16(LPWORD lpDst, LPWORD lpSrc) { lpDst[0] = lpSrc[1];}
void ConvertDataBlue16(LPWORD lpDst, LPWORD lpSrc) { lpDst[0] = lpSrc[0];}
void ConvertDataGray16(LPWORD lpDst, LPWORD lpSrc) { lpDst[0] = BaumUtils::Bright(int(lpSrc[2]),int(lpSrc[1]),int(lpSrc[0]));}

void ConvertRGB2GS(LPBITMAPINFOHEADER lpbiGS, LPBITMAPINFOHEADER lpbiRGB)
{
	int nBPS = DIBBps(lpbiRGB);
	*lpbiGS = *lpbiRGB;
	lpbiGS->biBitCount = lpbiRGB->biBitCount/3;
	lpbiGS->biSizeImage = 0;
	if (lpbiGS->biBitCount == 8)
	{
		lpbiGS->biClrUsed = 256;
		RGBQUAD *pRGB = (RGBQUAD *)(lpbiGS+1);
		for (int i = 0; i < 256; i++)
		{
			pRGB[i].rgbRed = pRGB[i].rgbGreen = pRGB[i].rgbBlue = i;
			pRGB[i].rgbReserved = 0;
		}
		ConvertData8 func;
		if (g_CxLibWork.m_nGrayMode == RedPlane)
			func = ConvertDataRed8;
		else if (g_CxLibWork.m_nGrayMode == GreenPlane)
			func = ConvertDataGreen8;
		else if (g_CxLibWork.m_nGrayMode == BluePlane)
			func = ConvertDataBlue8;
		else
			func = ConvertDataGray8;
		DWORD  dwRowSrc = DIBRowSize(lpbiRGB);
		LPBYTE lpDataSrc = (LPBYTE)(lpbiRGB+1);
		DWORD dwRowDst = DIBRowSize(lpbiGS);
		LPBYTE lpDataDst = (LPBYTE)(pRGB+256);
		for (int y = 0; y < lpbiGS->biHeight; y++)
		{
			for (int x = 0; x < lpbiGS->biWidth; x++)
				func(&lpDataDst[x], &lpDataSrc[3*x]);
			lpDataSrc += dwRowSrc;
			lpDataDst += dwRowDst;
		}
	}
	else
	{
		ConvertData16 func;
		if (g_CxLibWork.m_nGrayMode == RedPlane)
			func = ConvertDataRed16;
		else if (g_CxLibWork.m_nGrayMode == GreenPlane)
			func = ConvertDataGreen16;
		else if (g_CxLibWork.m_nGrayMode == BluePlane)
			func = ConvertDataBlue16;
		else
			func = ConvertDataGray16;
		DWORD  dwRowSrc = DIBRowSize(lpbiRGB)/2;
		LPWORD lpDataSrc = (LPWORD)(lpbiRGB+1);
		DWORD dwRowDst = DIBRowSize(lpbiGS)/2;
		LPWORD lpDataDst = (LPWORD)(lpbiGS+1);
		for (int y = 0; y < lpbiGS->biHeight; y++)
		{
			for (int x = 0; x < lpbiGS->biWidth; x++)
				func(&lpDataDst[x], &lpDataSrc[3*x]);
			lpDataSrc += dwRowSrc;
			lpDataDst += dwRowDst;
		}
	}
}

void ManualConvert(BYTE *pb8, WORD *pb16, BYTE *lpConv, int nSize)
{
	int n = nSize/6;
	for (; n>0; n--)
	{
		pb8[0] = lpConv[pb16[0]];
		pb8[1] = lpConv[pb16[1]];
		pb8[2] = lpConv[pb16[2]];
		pb8 += 3;
		pb16 += 3;
	}
}


void ManualConvertARC6000(BYTE *pb8, WORD *pb16, BYTE *lpConv, LPBITMAPINFOHEADER lpbi)
{
	int cx = lpbi->biWidth;
	int cy = lpbi->biHeight;
	if (lpbi->biBitCount == 8)
	{
		for (int y = 0; y < cy; y++)
		{
			BYTE *pb8_ = pb8+y*((cx+3)/4*4);
			WORD *pb16_ = pb16+y*((cx+3)/4*4);
			for (int x = 0; x < cx; x++)
				*pb8_++ = lpConv[*pb16_++];
			*(pb8_-cx/2-2) = *(pb8_-cx/2-3);
			*(pb8_-cx/2-1) = *(pb8_-cx/2);
		}
	}
	else
	{
		for (int y = 0; y < cy; y++)
		{
			BYTE *pb8_ = pb8+y*((cx*3+3)/4*4);
			WORD *pb16_ = pb16+y*((cx*3+3)/4*4);
			for (int x = 0; x < cx; x++)
			{
				pb8_[0] = lpConv[pb16_[0]];
				pb8_[1] = lpConv[pb16_[1]];
				pb8_[2] = lpConv[pb16_[2]];
				pb8_ += 3;
				pb16_ += 3;
			}
			memcpy(pb8_-3*cx/2-6,pb8_-3*cx/2-9,3);
			memcpy(pb8_-3*cx/2-3,pb8_-3*cx/2,3);
		}
	}
}

LONG s_dwThreadRun = 0;
HANDLE s_hCycle = NULL;
HANDLE s_hFinished = NULL;
BOOL s_bSetup = FALSE;
BOOL s_bSetupComplete = FALSE;
HWND s_hwndProgress;
CRITICAL_SECTION g_CritSectionCamera;
CRITICAL_SECTION g_CritSectionSite;
DWORD m_dwSleep;
DWORD m_dwSleepOnSetup;

class CBaumerThread : public CWinThread
{
	int m_imNumber,m_imIfcNumber;
	void InputDirect(int nMode);
	void InputAdvanced(int nMode);
	void DoCycle(BOOL bNotify = TRUE);
	void DoSetup();
public:
	virtual BOOL InitInstance();
};


void CBaumerThread::DoCycle(BOOL bNotify)
{
	try
	{
		// 1. Wait for new image.
		while (g_CxLibWork.m_CameraStatus.bTriggerEn && !IsTriggerEvent(1) && s_dwThreadRun);
		int stamp;
		DWORD dw = GetImNumber(&stamp);
		while ((m_imNumber == (stamp&0xffff)) && (m_imIfcNumber == (int)(dw&0xffff)))
		{
			Sleep(m_dwSleep);
			dw = GetImNumber(&stamp);
			if (!s_dwThreadRun) return;
		}
		m_imNumber		= stamp&0xffff;
		m_imIfcNumber	= dw   &0xffff;
		// 2. Input image. Input image in color mode only if it's color camera and no binning and
		// color image requested. In other cases input only grayscale image.
		int nMode;
		EnterCriticalSection(&g_CritSectionCamera);
		if (IsDC300(g_CxLibWork.m_nCamera))
			nMode = g_CxLibWork.m_nRom==DC300_FULLFRAME?FULLDC300|g_CxLibWork.m_nTestMode:PREVDC300;
		else
			nMode = g_CxLibWork.m_CameraType.bColor&&!g_CxLibWork.m_CameraStatus.bBinning?1:2;
		if (IsInputAdvanced())
			InputAdvanced(nMode);
		else
			InputDirect(nMode);
		LeaveCriticalSection(&g_CritSectionCamera);
		// 3. Notify preview sites about new image.
		::SetEvent(s_hCycle);
		g_CxLibWork.SetImageCaptured();
		if (bNotify)
		{
			EnterCriticalSection(&g_CritSectionSite);
			for (int i = 0; i < g_CxLibWork.m_arrSites.GetSize(); i++)
				g_CxLibWork.m_arrSites[i]->Invalidate();
			LeaveCriticalSection(&g_CritSectionSite);
		}
	}
	catch(...)
	{
	}
}

void CBaumerThread::InputDirect(int nMode)
{
	BITMAPFILEHEADER bmfHdr;
	::ZeroMemory( &bmfHdr, sizeof(BITMAPFILEHEADER) );
	if (nMode == 1)
		g_CxLibWork.m_lpData = (LPBYTE)(g_CxLibWork.m_lpbi+1);
	else
		g_CxLibWork.m_lpData = (LPBYTE)(((RGBQUAD *)(g_CxLibWork.m_lpbi+1))+256);
	GetBmpDirect(&bmfHdr, g_CxLibWork.m_lpbi, g_CxLibWork.m_lpData, nMode);
}

void CBaumerThread::InputAdvanced(int nMode)
{
	switch(g_CxLibWork.m_nBitsMode)
	{
		case W8BIT: break;
		case W10BIT: nMode = (C1024 & ~3)|nMode; break;
		case W12BIT: nMode = (C4096 & ~3)|nMode; break;
		default:return;
	}
	HGLOBAL hg = GetBmpAdvanced(nMode,1);
	PVOID pv = ::GlobalLock(hg);
//	int	size = ::GlobalSize(hg);
	BITMAPFILEHEADER *pbf = (BITMAPFILEHEADER *)pv;
	LPBITMAPINFOHEADER lpbiRead = (LPBITMAPINFOHEADER)(pbf+1);
	LPWORD lpData = (LPWORD)(((LPBYTE)pv)+pbf->bfOffBits);
	int nSize = pbf->bfSize-pbf->bfOffBits;
	if (lpbiRead->biBitCount == 8 || lpbiRead->biBitCount == 24)
	{
		if (lpbiRead->biBitCount == 24 && g_CxLibWork.m_nGrayMode!=NoneGrayMode)
			ConvertRGB2GS(g_CxLibWork.m_lpbi, lpbiRead);
		else
			memcpy(g_CxLibWork.m_lpbi, lpbiRead, nSize);
		g_CxLibWork.m_lpData = (LPBYTE)(((RGBQUAD*)(g_CxLibWork.m_lpbi+1))+g_CxLibWork.m_lpbi->biClrUsed);
	}
	else
	{
		if (DIBIsTrueColor2(lpbiRead) && g_CxLibWork.m_nGrayMode!=NoneGrayMode)
			ConvertRGB2GS(g_CxLibWork.m_lpbi16, lpbiRead);
		else
			memcpy(g_CxLibWork.m_lpbi16, lpbiRead, nSize);
		if (IsManualConvert())
		{
			*g_CxLibWork.m_lpbi = *g_CxLibWork.m_lpbi16;
			g_CxLibWork.m_lpbi->biBitCount = g_CxLibWork.m_lpbi->biBitCount>16?24:8;
			g_CxLibWork.m_lpbi->biSizeImage = 0;
			if (g_CxLibWork.m_lpbi->biBitCount == 8)
			{
				g_CxLibWork.m_lpbi->biClrUsed = 256;
				RGBQUAD *pRGB = (RGBQUAD *)(g_CxLibWork.m_lpbi+1);
				for (int i = 0; i < 256; i++)
				{
					pRGB[i].rgbRed = pRGB[i].rgbGreen = pRGB[i].rgbBlue = i;
					pRGB[i].rgbReserved = 0;
				}
			}
			g_CxLibWork.m_lpData = (LPBYTE)(((RGBQUAD*)(g_CxLibWork.m_lpbi+1))+g_CxLibWork.m_lpbi->biClrUsed);
			WORD *pb16 = (LPWORD)(g_CxLibWork.m_lpbi16+1);
			BYTE *pb8  = g_CxLibWork.m_lpData;
			BYTE *lpConv = g_CxLibWork.m_lpbi16->biBitCount==10||g_CxLibWork.m_lpbi16->biBitCount==30?g_CxLibWork.m_lpConv:g_CxLibWork.m_lpConv12;
			if (Arc6000Gluck1())
				ManualConvertARC6000(pb8, pb16, lpConv, g_CxLibWork.m_lpbi);
			else
				ManualConvert(pb8,pb16,lpConv,nSize);
		}
	}
	::GlobalUnlock(hg);
}

void CalcPointsNum(int *pnBlack, int *pnWhite)
{
	*pnBlack = 0;
	*pnWhite = 0;
	int nRow = ((g_CxLibWork.m_lpbi->biWidth*g_CxLibWork.m_lpbi->biBitCount/8+3)>>2)<<2;
	LPBYTE pData = g_CxLibWork.m_lpData;
	for (int i = 0; i < g_CxLibWork.m_lpbi->biHeight; i++)
	{
		for (int j = 0; j < g_CxLibWork.m_lpbi->biWidth; j++)
		{
			if (pData[3*j]==0&&pData[3*j+1]==0&&pData[3*j+2]==0)
				(*pnBlack)++;
			if (pData[3*j]==255&&pData[3*j+1]==255&&pData[3*j+2]==255)
				(*pnWhite)++;
		}
		pData += nRow;
	}
}

int StepsByValue(int nVal)
{
	int i = 0;
	while (nVal)
	{
		nVal >>= 1;
		i++;
	};
	return i;
};

void FillExponentArray(CUIntArray &arr, int nMin, int nMax);
void CBaumerThread::DoSetup()
{
	DWORD dwStart = GetTickCount();
	int nBlack,nWhite;
	int nGain,nOffset,nShutter;
	tCameraType CameraType;
	tCameraStatus CameraStatus;
	bool bGainEnabled = GainEnabled(g_CxLibWork.m_nCamera);
	GetCameraInfo(&CameraType, sizeof(tCameraType), &CameraStatus, sizeof(tCameraStatus));
	DWORD dwPrevSleep = m_dwSleep;
	m_dwSleep = max(m_dwSleepOnSetup,m_dwSleep);
	int nDefaultGain = CameraType.vAmplMin+(CameraType.vAmplMax-CameraType.vAmplMin)/8;
//	nDefaultGain = min(max(nDefaultGain,CameraType.vAmplMin),CameraType.vAmplMax);
	if (bGainEnabled) SetGain(nDefaultGain);
	SetBlackLevel(CameraType.vOffsetMin);
	tSHT sht;
	SetShutterTime(g_CxLibWork.m_sht.timeMin, &sht);
	Sleep(300);
	GetCameraInfo(&CameraType, sizeof(tCameraType), &CameraStatus, sizeof(tCameraStatus));
	nGain = CameraStatus.vAmplif;
	nOffset = CameraStatus.vOffset;
	nShutter = g_CxLibWork.m_sht.timeMin;
	for (int i = 0; i < g_CxLibWork.m_nSkipFr; i++) DoCycle(g_CxLibWork.m_bRedrawOnSetup);
	CalcPointsNum(&nBlack,&nWhite);
	int nMinWhite = g_CxLibWork.m_lpbi->biWidth*g_CxLibWork.m_lpbi->biHeight*g_CxLibWork.m_nProcWhite/100;
	if (nMinWhite < 1) nMinWhite = 1;
	CProgressCtrl *pProgressCtl = (CProgressCtrl *)CWnd::FromHandle(s_hwndProgress);
#if 0
	int nSteps = StepsByValue(g_CxLibWork.m_nMaxShutter-g_CxLibWork.m_sht.timeMin);
	nSteps += StepsByValue(CameraType.vAmplMax-CameraType.vAmplMin);
	if (pProgressCtl) pProgressCtl->SetRange(0, nSteps);
	if (pProgressCtl) pProgressCtl->SetPos(0);
	int nStep = 0;
	// First use shutter. Try to find such position, in that there is minimal number of white points.
	if (nWhite < nMinWhite)
	{
		int nPrevSh1 = g_CxLibWork.m_sht.timeMin;
		int nPrevSh2 = g_CxLibWork.m_nMaxShutter;
		while (s_dwThreadRun && s_bSetup && nPrevSh2-nPrevSh1 >= 2)
		{
			nShutter = (nPrevSh1+nPrevSh2)/2;
			SetShutterTime(nShutter, &sht);
			for (int i = 0; i < g_CxLibWork.m_nSkipFr; i++) DoCycle(g_CxLibWork.m_bRedrawOnSetup);
			CalcPointsNum(&nBlack,&nWhite);
			TRACE2("Shutter %d, %d\n", nShutter, nWhite);
			if (nWhite >= nMinWhite)
				nPrevSh2 = nShutter;
			else
				nPrevSh1 = nShutter;
			if (pProgressCtl) pProgressCtl->SetPos(++nStep);
		}
		if (s_dwThreadRun && s_bSetup && nPrevSh1 != nShutter)
		{
			nShutter = nPrevSh1;
			SetShutterTime(nShutter, &sht);
			TRACE1("Shutter %d\n", nShutter);
		}
	}
	// If maximal value of shutter is not enough for proper histogram, use gain
	if (s_dwThreadRun && s_bSetup && nShutter >= g_CxLibWork.m_nMaxShutter-2)
#else
	// First use shutter. Try to find such position, in that there is minimal number of white points.
	CUIntArray arr;
	FillExponentArray(arr, g_CxLibWork.m_sht.timeMin, g_CxLibWork.m_nMaxShutter);
	int nSteps = StepsByValue(arr.GetSize());
	nSteps += StepsByValue(CameraType.vAmplMax-CameraType.vAmplMin);
	if (pProgressCtl) pProgressCtl->SetRange(0, nSteps);
	if (pProgressCtl) pProgressCtl->SetPos(0);
	int nStep = 0;
	if (nWhite < nMinWhite)
	{
		int nPrevSh1Pos = 0;
		int nPrevSh2Pos = arr.GetSize()-1;
		int nShutterPos = 0;
		while (s_dwThreadRun && s_bSetup && nPrevSh2Pos-nPrevSh1Pos >= 2)
		{
			nShutterPos = (nPrevSh1Pos+nPrevSh2Pos)/2;
			SetShutterTime(arr[nShutterPos], &sht);
			for (int i = 0; i < g_CxLibWork.m_nSkipFr; i++) DoCycle(g_CxLibWork.m_bRedrawOnSetup);
			CalcPointsNum(&nBlack,&nWhite);
			TRACE2("Shutter %d, %d\n", arr[nShutterPos], nWhite);
			if (nWhite >= nMinWhite)
				nPrevSh2Pos = nShutterPos;
			else
				nPrevSh1Pos = nShutterPos;
			if (pProgressCtl) pProgressCtl->SetPos(++nStep);
		}
		if (s_dwThreadRun && s_bSetup && nPrevSh1Pos != nShutterPos)
		{
			nShutterPos = nPrevSh1Pos;
			SetShutterTime(arr[nShutterPos], &sht);
			TRACE1("Shutter %d\n", arr[nShutterPos]);
		}
		nShutter = arr[nShutterPos];
	}
	// If maximal value of shutter is not enough for proper histogram, use gain
	if (s_dwThreadRun && s_bSetup && nShutter >= g_CxLibWork.m_nMaxShutter-2)
#endif
	{
		// Now shutter value m_nMaxShutter-2 is set and it is too dark. Try use m_nMaxShutter.
		SetShutterTime(g_CxLibWork.m_nMaxShutter, &sht);
		for (int i = 0; i < g_CxLibWork.m_nSkipFr; i++) DoCycle(g_CxLibWork.m_bRedrawOnSetup);
		CalcPointsNum(&nBlack,&nWhite);
		if (nWhite < nMinWhite)
		{ // It's too dark, use gain
			nShutter = g_CxLibWork.m_nMaxShutter;
			int nPrevGain1,nPrevGain2;
			nPrevGain1 = nDefaultGain;
			nPrevGain2 = CameraType.vAmplMax;
			while (s_dwThreadRun && s_bSetup && nPrevGain2-nPrevGain1 >= 2)
			{
				nGain = (nPrevGain1+nPrevGain2)/2;
				SetGain(nGain);
				for (int i = 0; i < g_CxLibWork.m_nSkipFr; i++) DoCycle(g_CxLibWork.m_bRedrawOnSetup);
				CalcPointsNum(&nBlack,&nWhite);
				if (nWhite >= nMinWhite)
					nPrevGain2 = nGain;
				else
					nPrevGain1 = nGain;
				TRACE2("Gain %d, %d\n", nGain, nWhite);
				if (pProgressCtl) pProgressCtl->SetPos(++nStep);
			}
			if (nWhite < nMinWhite)
			{
				// Its's too dark and now CameraType.vAmplMax-2 is set. Use CameraType.vAmplMax.
				SetGain(CameraType.vAmplMax);
				nGain = CameraType.vAmplMax;
			}
		} // Use gain
		else
			SetShutterTime(nShutter, &sht);
	} // nShutter >=m_nMaxShutter-2
	g_CxLibWork.m_nFoundShutter = nShutter;
	m_dwSleep = dwPrevSleep;
	s_bSetupComplete = TRUE;
	if (s_dwThreadRun && s_bSetup && g_CxLibWork.m_bShowSetupTime)
	{
		DWORD dwEnd = GetTickCount();
		CString s;
		s.Format("AutoSetup time: %d ms", dwEnd-dwStart);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
	}
}


BOOL CBaumerThread::InitInstance()
{
	m_imNumber = m_imIfcNumber = -1;
	DWORD dwTicks;
	DWORD dwPrevTicks = GetTickCount();
	while (s_dwThreadRun)
	{
		if (s_bSetup && !s_bSetupComplete)
			DoSetup();
		else
			DoCycle(TRUE);
		dwTicks = GetTickCount();
		g_CxLibWork.m_dwTickCount = dwTicks-dwPrevTicks;
		dwPrevTicks = dwTicks;
	}
	::SetEvent(s_hFinished);
	return FALSE;
}


CCxLibWork::CCxLibWork()
{
	m_cx = m_cy = m_bpp = 0;
	m_nInitCount = 0;
	m_nStartCount = 0;
	m_nInitRes = 0;
	m_lpBuff = NULL;
	m_lpbi = NULL;
	m_lpData = NULL;
	m_lpbi16 = NULL;//m_lpData16 = NULL;
	m_lpConv = NULL;
	m_lpConv12 = NULL;
	m_bImageCaptured = false;
	m_rcMargins = CRect(0,0,0,0);
	m_bEnableMargins = FALSE;
	m_bShowSetupTime = FALSE;
	InitializeCriticalSection(&g_CritSectionCamera);
	InitializeCriticalSection(&g_CritSectionSite);
	m_nGrayMode = NoneGrayMode;
	m_nBitsMode = W8BIT;
	m_nConversion = AutoConversion;
	m_bForCapture = FALSE;
	m_nFoundShutter = m_nMaxShutter = 0;
	m_nRom = DC300_FULLFRAME;
	m_nTestMode = 0;
}

CCxLibWork::~CCxLibWork()
{
	DoStop();
	DoFree();
}

bool CCxLibWork::Init(bool bReinit)
{
	CString sMethName = g_BaumerProfile.GetMethodic();
	if (IsInited() && !bReinit && sMethName == m_sMethName)
	{
		ASSERT(m_lpBuff);
		return m_nInitRes>0;
	}
	m_sMethName = sMethName;
	m_bBinningGluck = g_BaumerProfile.GetProfileInt(_T("Glucks"), _T("BinningGluck"), TRUE, true, true);
	m_bARC6000Gluck1 = g_BaumerProfile.GetProfileInt(_T("Glucks"), _T("ARC6000Gluck1"), TRUE, true, true);
	m_bARC6000Gluck2 = g_BaumerProfile.GetProfileInt(_T("Glucks"), _T("ARC6000Gluck2"), TRUE, true, true);
	// Init library
	memset(&m_CameraStatus, 0, sizeof(m_CameraStatus));
	m_nInitRes = InitCameraSettings(!IsInited());
	// Initialize counter
	m_nInitCount++;
	if (m_nInitRes != 1)
		return false;
	// Init buffers
	m_cx = GetImFrameX();
	m_cy = GetImFrameY();
	m_bpp = GetImBpp();
	int cx1 = max(m_cx, 2500);
	int cy1 = max(m_cy, 1600);
	int nRow = (((cx1*3+3)>>2)<<2);
	int nSize = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+nRow*cy1;
	if (!m_lpBuff)
	{
		m_lpBuff = new BYTE[nSize];
		memset(m_lpBuff, 0, nSize);
	}
	else 
		memset(m_lpbi, 0, sizeof(BITMAPINFOHEADER));
	m_lpbi = (LPBITMAPINFOHEADER)(((LPBITMAPFILEHEADER)m_lpBuff)+1);
	m_lpbi->biSize = sizeof(BITMAPINFOHEADER);
	ReinitSizes();
	m_lpbi->biPlanes = 1;
	m_lpbi->biBitCount = 24;
	m_lpbi->biCompression = BI_RGB;
	m_lpbi->biSizeImage = m_cx*m_cy;
	m_lpbi->biXPelsPerMeter	= 0;
	m_lpbi->biYPelsPerMeter	= 0;
	m_lpbi->biClrUsed 		= 0;
	m_lpbi->biClrImportant	= 0;
	m_lpData = (LPBYTE)(m_lpbi+1);
	m_dwSleep = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("SleepTime"), 0, true);
	m_dwSleepOnSetup = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("SleepTimeDuringSetup"), 20, true);
	m_bRedrawOnSetup = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("RedrawDuringSetup"), FALSE, true);
	m_bShowSetupTime = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ShowSetupTime"), FALSE, true);
	m_nMaxShutter = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("MaxShutterMcs"), 1000000, true);
	m_nProcWhite = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ProcentWhite"), 0, true);
	m_nSkipFr = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("SkipFramesDuringSetup"), 4, true);
	int nGrayMode;
	if (g_BaumerProfile.GetProfileInt(_T("Settings"), _T("GrayScale"), FALSE))
		nGrayMode = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Plane"), TRUE);
	else
		nGrayMode = NoneGrayMode;
	InitGray(nGrayMode);
	m_nConversion = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Conversion"), AutoConversion, true);
	m_nConvStart = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionStart"), 0, true);
	m_nConvEnd = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionEnd"), 0x400, true);
	m_dGamma = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("Gamma"), 1., NULL, true);
	m_nConvStart12 = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionStart12"), 0, true);
	m_nConvEnd12 = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ConversionEnd12"), 0x1000, true);
	m_dGamma12 = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("Gamma12"), 1., NULL, true);
	InitBitnessAndConversion(m_nBitsMode,m_nConversion,m_bForCapture,m_nConvStart,m_nConvEnd,m_dGamma,
		m_nConvStart12,m_nConvEnd12,m_dGamma12);
	return m_nInitRes>0;
}

void CCxLibWork::ReinitSizes()
{
	if (IsDC300())
	{
		m_lpbi->biWidth = m_cx = 2088;
		m_lpbi->biHeight = m_cy = 1550;
	}
	else
	{
		m_lpbi->biWidth = m_cx = GetImFrameX();
		m_lpbi->biHeight = m_cy = GetImFrameY();
	}

}



void CCxLibWork::InitBitnessAndConversion(int nBitsMode, int nConversion, BOOL bForCapture,
	int nStart, int nEnd, double dGamma, int nStart12, int nEnd12, double dGamma12)
{
	const unsigned nConvSize = 0x10000;
	bool bNeedBuff16 = IsInputAdvanced();
	bool bNeedConv = bNeedBuff16 && IsManualConvert();
	bool bNeedConv12 = bNeedConv;
	// Initialize 16-bit buffer
	if (bNeedBuff16 && m_lpbi16 == NULL)//m_lpData16 == NULL)
	{
		int cx1 = max(m_cx, 2500);
		int cy1 = max(m_cx, 1600);
		int nRow = (((cx1*3+3)>>2)<<2);
		m_lpbi16 = (LPBITMAPINFOHEADER)new BYTE[sizeof(BITMAPINFOHEADER)+2*nRow*cy1];
	}
	if (bNeedConv && !m_lpConv)
		m_lpConv = new BYTE[nConvSize];
	// Initialize conversion buffer for 10-bit mode
	if (m_lpConv && nStart != -1)
	{
		if (m_nConversion != ManualConversion)
		{
			nStart = 0;
			nEnd = 1024;
		}
		for (unsigned i = 0; i < (unsigned)nStart; i++)
			m_lpConv[i] = 0;
		for (; i < (unsigned)nEnd; i++)
			m_lpConv[i] = (BYTE)(255*pow(double(i-nStart)/(nEnd-nStart), 1./dGamma));
//			m_lpConv[i] = (i-nStart)*256/(nEnd-nStart);
		for (; i < nConvSize; i++)
			m_lpConv[i] = 255;
	}
	// Initialize conversion buffer for 12-bit mode
	if (bNeedConv12 && !m_lpConv12)
		m_lpConv12 = new BYTE[nConvSize];
	if (m_lpConv12 && nStart12 != -1)
	{
		if (m_nConversion != ManualConversion)
		{
			nStart12 = 0;
			nEnd12 = 4096;
		}
		for (unsigned i = 0; i < (unsigned)nStart12; i++)
			m_lpConv12[i] = 0;
		for (; i < (unsigned)nEnd12; i++)
			m_lpConv12[i] = (BYTE)(255*pow(double(i-nStart12)/(nEnd12-nStart12), 1./dGamma12));
//			m_lpConv12[i] = (i-nStart12)*256/(nEnd12-nStart12);
		for (; i < nConvSize; i++)
			m_lpConv12[i] = 255;
	}
	InitDC300();
}

void CCxLibWork::InitDC300(bool bForCapture)
{
	if (IsDC300())
	{
		if (m_nDC300QuickView && !bForCapture && !Arc6000Gluck())
			m_nRom = DC300_PREVIEW;
		else
			m_nRom = DC300_FULLFRAME;
		ReadoutMode(m_nRom);
		int p = 4;
		SetScanMode( SM_SSMTC, &p);
	}
}


void CCxLibWork::InitGray(int nGrayMode)
{
	m_nGrayMode = nGrayMode;
}

void CCxLibWork::DoFree()
{
	if (m_nInitRes) StopScanMode();
	delete m_lpbi16;//m_lpData16;
	m_lpbi16 = NULL;//m_lpData16 = NULL;
	delete m_lpBuff;
	m_lpBuff = NULL;
	m_lpbi = NULL;
	m_lpData = NULL;
	m_nGrayMode = NoneGrayMode;
	delete m_lpConv;
	m_lpConv = NULL;
	delete m_lpConv12;
	m_lpConv12 = NULL;
}

bool CCxLibWork::StartGrab()
{
	if (!IsInited())
		return false;
	if (IsStarted())
	{
		m_nStartCount++; // Grab is already started
		return true;
	}
	// Init thread
	InterlockedExchange(&s_dwThreadRun, 1);
	s_hFinished = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	s_hCycle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	CBaumerThread *pThread = new CBaumerThread;
	pThread->m_bAutoDelete = TRUE;
	pThread->CreateThread();
	if (g_BaumerProfile.GetProfileInt(_T("Settings"), _T("PriorityBelowNormal"), TRUE, true))
		pThread->SetThreadPriority(THREAD_PRIORITY_BELOW_NORMAL);
	else
		pThread->SetThreadPriority(THREAD_PRIORITY_NORMAL);
	// Initialize start grab counter
	m_nStartCount++;
	return true;
}

void CCxLibWork::StopGrab()
{
	if (--m_nStartCount <= 0)
		DoStop();
}

void CCxLibWork::DoStop()
{
	InterlockedExchange(&s_dwThreadRun, 0);
	::WaitForSingleObject(s_hFinished, INFINITE);
	::CloseHandle(s_hCycle);
	::CloseHandle(s_hFinished);
	s_hCycle = NULL;
	s_hFinished = NULL;
	m_bImageCaptured = false;
}

bool CCxLibWork::WaitForFrame(int nCount, DWORD dwTimeout)
{
	if (!IsStarted()) return false;
	if (nCount == 0)
	{
		nCount = g_BaumerProfile.GetProfileInt(NULL, _T("WaitFrames"), 3, true);
		dwTimeout = g_BaumerProfile.GetProfileInt(NULL, _T("WaitTimeout"), 30000, true);
	}
	for (int i = 0; i < nCount; i++)
	{
		ResetEvent(s_hCycle);
		DWORD dw = WaitForSingleObject(s_hCycle, dwTimeout);
		if (dw == WAIT_TIMEOUT) return false;
	}
	return true;
}

int CCxLibWork::InitCamera(int nCamera)
{
	int r = InitCxLib(nCamera);
	m_nCamera = nCamera;
	if (r == 1)
	{
		GetCameraInfo(&m_CameraType, sizeof(tCameraType), &m_CameraStatus , sizeof(tCameraStatus));
		SetPixelWidth(m_nBitsMode);
		if (::IsDC300(nCamera))
		{
			m_nRom = ROMFM;
			ReadoutMode(m_nRom);
			int p = 4;
			SetScanMode( SM_SSMTC, &p);
		}
		else
		{
			int p = 3;
			SetScanMode(SM_SSMTC, &p);
		}
	}
	else
		ASSERT( false );

	return r;
}

bool IsWin9x()
{
	return false;
}

int CCxLibWork::InitCameraSettings(bool bReinitCamera)
{
	m_nBitsMode = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("BitsMode"), W8BIT); // Used inside InitCamera
	int r = 1;
	int nCamera = g_BaumerProfile.ReadCamera()->nBaumerId;
	if (Is12BitCamera(nCamera) && m_nBitsMode == W10BIT)
		m_nBitsMode = W12BIT;
	if (bReinitCamera)
	{
		r = InitCamera(nCamera);
		if (r != 1) return r;
	}
	int nOffset = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("BlackLevel"), 0);
	if (nOffset != -1 && nOffset != g_CxLibWork.m_CameraStatus.vOffset)
		SetBlackLevel(nOffset);
	int nGain = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Gain"), -1);
	if (nGain != -1 && nGain != g_CxLibWork.m_CameraStatus.vAmplif && GainEnabled(nCamera))
		SetGain(nGain);
	SetShutterTime(-1, &m_sht);
	BOOL bAutoShutter = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("AutoShutter"), FALSE);
	int nAutoBrightness = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("AutoShutterBrightness"), 200, true);
	AutomaticShutter(bAutoShutter, nAutoBrightness);
	if (!bAutoShutter)
	{
		int nShutter = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ShutterMcs"), 1000);
		tSHT sht;
		SetShutterTime(Range(nShutter,m_sht.timeMin,m_sht.timeMax), &sht);
	}
	if (!IsWin9x())
	{
		BOOL bAGP = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("DirectX"), TRUE);
		CAMERADLLSETS settings;
		settings.VadSearchMode = bAGP;
		DLLSettings(&settings);
	}
	BOOL bBinning = m_CameraType.cBinnMode?g_BaumerProfile.GetProfileInt(_T("Settings"),_T("Binning"),FALSE):FALSE;
	SetBinningMode(bBinning);
	if (bBinning)
	{
		SetWBalance(FALSE, 0, 0, 0);
		SetRgbLut(FALSE, 0, NULL, NULL, NULL);
	}
	else if (m_CameraType.bColor)
	{
		BOOL bWBEnable = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("WhiteBalanceEnable"), FALSE);
		double dWBRed = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Red"), -1.);
		double dWBGreen = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Green"), -1.);
		double dWBBlue = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Blue"), -1.);
		if (bWBEnable && dWBRed > 0. && dWBGreen > 0. && dWBBlue > 0.)
			SetWBalance(bWBEnable, dWBRed, dWBGreen, dWBBlue);
		if (g_BaumerProfile.GetProfileInt(NULL, _T("EnableColorBalancePage"), FALSE, true, true))
		{
			BOOL bCBEnable = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ColorBalanceEnable"), FALSE);
			double dCBRed = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Red"), 1.);
			double dCBGreen = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Green"), 1.);
			double dCBBlue = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Blue"), 1.);
			if (bCBEnable && dCBRed > 0. && dCBGreen > 0. && dCBBlue > 0.)
				SetRgbLut(bCBEnable, 1, &dCBRed, &dCBGreen, &dCBBlue);
		}
		else
			SetRgbLut(FALSE, 0, NULL, NULL, NULL);
	}
	GetCameraInfo(&m_CameraType, sizeof(tCameraType), &m_CameraStatus , sizeof(tCameraStatus));
	m_rcMargins.left = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Left"), 0);
	m_rcMargins.left = min(m_rcMargins.left, MAX_MARGIN);
	m_rcMargins.right = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Right"), 0);
	m_rcMargins.right = min(m_rcMargins.right, MAX_MARGIN);
	m_rcMargins.top = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Top"), 0);
	m_rcMargins.top = min(m_rcMargins.top, MAX_MARGIN);
	m_rcMargins.bottom = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Bottom"), 0);
	m_rcMargins.bottom = min(m_rcMargins.bottom, MAX_MARGIN);
	m_bEnableMargins = g_BaumerProfile.GetProfileInt(_T("Margins"), _T("Enable"), FALSE);
	BOOL bTriggerMode = !bBinning&&!IsDC300()?g_BaumerProfile.GetProfileInt(_T("Trigger"),_T("TriggerMode"),FALSE,true):FALSE;
	BOOL bDischarge = g_BaumerProfile.GetProfileInt(_T("Trigger"), _T("Discharge"), FALSE, true);
	SetTriggerMode(bTriggerMode, bDischarge);
	m_nDC300QuickView = g_BaumerProfile.GetProfileInt(NULL, _T("DC300QuickView"), 1, true, true);
	return r;
}

void CCxLibWork::DisableBinning(bool bDisable)
{
	if (IsDC300())	return; // Not supported for ARC6000
	BOOL bBinning = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Binning"), FALSE);
	if (bBinning)
	{
		SetBinningMode(!bDisable);
		Sleep(300);
		int p1 = 3;
		SetScanMode(SM_SSMTC, &p1);
		m_cx = m_lpbi->biWidth = GetImFrameX();
		m_cy = m_lpbi->biHeight = GetImFrameY();
		m_bpp = GetImBpp();
		if (bDisable)
		{
			BOOL bWBEnable = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("WhiteBalanceEnable"), FALSE);
			double dWBRed = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Red"), -1.);
			double dWBGreen = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Green"), -1.);
			double dWBBlue = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("WhiteBalance-Blue"), -1.);
			if (bWBEnable && dWBRed > 0. && dWBGreen > 0. && dWBBlue > 0.)
				SetWBalance(bWBEnable, dWBRed, dWBGreen, dWBBlue);
			BOOL bCBEnable = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("ColorBalanceEnable"), FALSE);
			double dCBRed = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Red"), 1.);
			double dCBGreen = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Green"), 1.);
			double dCBBlue = g_BaumerProfile.GetProfileDouble(_T("Settings"), _T("ColorBalance-Blue"), 1.);
			if (bCBEnable && dCBRed > 0. && dCBGreen > 0. && dCBBlue > 0.)
				SetRgbLut(bCBEnable, 1, &dCBRed, &dCBGreen, &dCBBlue);
		}
		else
		{
			SetWBalance(FALSE, NULL, NULL, NULL);
			SetRgbLut(FALSE, 0, NULL, NULL, NULL);
		}
		GetCameraInfo(&g_CxLibWork.m_CameraType, sizeof(tCameraType), &g_CxLibWork.m_CameraStatus , sizeof(tCameraStatus));
	}
}


void CCxLibWork::StartDisplay(BOOL bStartVideo, int nScaler, HWND hwnd, CRect rcVideo)
{
	if(bStartVideo == FALSE)
	{
		SetVideoDma(FALSE);
		return;
	}
	else
	{
		SetVideoDma(FALSE); Sleep(80);
		SetVideoOutputMode(hwnd, &rcVideo, nScaler);
		SetVideoDma(TRUE); Sleep(80);
	}
}

void CCxLibWork::SetForCapture(BOOL bForCapture)
{
	if (bForCapture == m_bForCapture) return;
	EnterCriticalSection(&g_CritSectionCamera);
	m_bForCapture = bForCapture;
	InitBitnessAndConversion(m_nBitsMode,m_nConversion,m_bForCapture,m_nConvStart,m_nConvEnd,
		m_dGamma,m_nConvStart12,m_nConvEnd12,m_dGamma12);
	DisableBinning(bForCapture?true:false);
//	m_nRom = bForCapture||m_nDC300QuickView==false?DC300_FULLFRAME:DC300_PREVIEW;
	InitDC300(bForCapture==TRUE);
	LeaveCriticalSection(&g_CritSectionCamera);
	if (IsStarted() && m_bForCapture && !m_CameraStatus.bTriggerEn)
		WaitForFrame(0);
}


void CCxLibWork::NotifyChangeSizes()
{
	EnterCriticalSection(&g_CritSectionSite);
	for (int i = 0; i < g_CxLibWork.m_arrSites.GetSize(); i++)
		g_CxLibWork.m_arrSites[i]->OnChangeSize();
	LeaveCriticalSection(&g_CritSectionSite);
}

void CCxLibWork::SetWBalance(BOOL bEnable, double dWBRed, double dWBGreen, double dWBBlue)
{
	SetWhiteBalance(bEnable, 1, &dWBRed, &dWBGreen, &dWBBlue);
}

void CCxLibWork::DoWBalance(double &dWBRed, double &dWBGreen, double &dWBBlue)
{
	if (g_CxLibWork.m_bEnableMargins)
	{
		CRect rc(g_CxLibWork.m_rcMargins.left, g_CxLibWork.m_rcMargins.top, g_CxLibWork.m_cx-
			g_CxLibWork.m_rcMargins.right, g_CxLibWork.m_cy-g_CxLibWork.m_rcMargins.bottom);
		DoWhiteBalanceRect(TRUE, &dWBRed, &dWBGreen, &dWBBlue, rc);
	}
	else
		DoWhiteBalance(TRUE, &dWBRed, &dWBGreen, &dWBBlue);
}

static bool Is05Camera(int nCamera)
{
	if (nCamera == MX05 || nCamera == MX05C || nCamera == CX05 || nCamera == CX05C ||
		nCamera == ARC1000 || nCamera == ARC1000C)
		return true;
	else
		return false;
}

int CCxLibWork::GetShutterTime()
{
	GetCameraInfo(&m_CameraType, sizeof(tCameraType), &m_CameraStatus , sizeof(tCameraStatus));
	double dValue;
	switch (m_CameraStatus.cShutMode)
	{
	case 2: // High speed shutter
		if (Is05Camera(m_nCamera))
			dValue = ((m_CameraStatus.vShutHS-199.)*64.+35.6)/1000.;
		else
			dValue = (1616.*m_CameraStatus.vShutHS+510.)/(29500./2);
		return int(dValue*1000.);
	case 1: // Low speed shutter
		if (Is05Camera(m_nCamera))
			dValue = 0.67*(768*582*m_CameraStatus.vShutLS)/(29500000./2);
		else
			dValue = 0.67*(1616*1043*m_CameraStatus.vShutLS)/(29500000./2);
		return int(dValue*1000000.);
	case 4: // Fast speed shutter
		return 500;
	}
	return 3000;
}

#if 0
static CameraIds CameraIDS[] =
{
	{MX05,  0, "MX05" },
	{MX05C, 1, "MX05C"},
	{MX13,  2, "MX13" },
	{MX13C, 3, "MX13C"},
	{CX05,  4, "CX05" },
	{CX05C, 5, "CX05C"},
	{CX13,  6, "CX13" },
	{CX13C, 7, "CX13C"},
	{ARC1000, 8, "ARC1000"},// CX05
	{ARC1000C, 9, "ARC1000C"}, // CX05C
	{ARC1800, 10, "ARC1800"}, // CX13
	{ARC1800C, 11, "ARC1800C"}, // CX13C
	{ARC4000, 12, "ARC4000"}, // CX131
	{ARC4000C, 13, "ARC4000C"}, // CX131C
	{ARC6000C, 14, "ARC6000C"}	// CX32C
};
#else
static CameraIds CameraIDS[] =
{
	{CX05,  0, "CX05" },
	{CX05C, 1, "CX05C"},
	{CX13,  2, "CX13" },
	{CX13C, 3, "CX13C"},
	{ARC1000, 4, "ARC1000"},// CX05
	{ARC1000C, 5, "ARC1000C"}, // CX05C
	{ARC1800, 6, "ARC1800"}, // CX13
	{ARC1800C, 7, "ARC1800C"}, // CX13C
	{ARC4000, 8, "ARC4000"}, // CX131
	{ARC4000C, 9, "ARC4000C"}, // CX131C
	{ARC6000C, 10, "ARC6000C"}	// CX32C
};
#endif

#define N_CAMERAS sizeof(CameraIDS)/sizeof(CameraIds)

int CameraIds::GetCameraCount()
{
	return N_CAMERAS;
}

CameraIds *CameraIds::CameraDescrByBaumerId(int nId)
{
	for (int i = 0; i < N_CAMERAS; i++)
	{
		if (CameraIDS[i].nBaumerId == nId)
		{
			return &CameraIDS[i];
		}
	}
	return &CameraIDS[0];
}

CameraIds *CameraIds::CameraDescrByInternalId(int nId)
{
	for (int i = 0; i < N_CAMERAS; i++)
	{
		if (CameraIDS[i].nInternalId == nId)
		{
			return &CameraIDS[i];
		}
	}
	return &CameraIDS[0];
}

CameraIds *CameraIds::CameraDescrByString(LPCTSTR lpName)
{
	for (int i = 0; i < N_CAMERAS; i++)
	{
		if (!_tcsicmp(CameraIDS[i].pszCameraName,lpName))
		{
			return &CameraIDS[i];
		}
	}
	return &CameraIDS[0];
}

static void DoRange(int nValidate, int nMin, int nMax)
{
	if (nValidate < nMin)
		nValidate = nMin;
	if (nValidate > nMax)
		nValidate = nMax;
}

CBaumerProfile::CBaumerProfile() : CVT5ProfileManager(_T("Baumer"), true)
{
}

int CBaumerProfile::GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef)
{
	int n = CVT5ProfileManager::GetDefaultProfileInt(lpSection, lpEntry, -1);
	if (n == -1)
	{
		if (lpSection && !_tcscmp(lpSection, _T("Settings")))
		{
			if (!_tcscmp(lpEntry, _T("BlackLevel")))
				n = g_CxLibWork.m_CameraStatus.vOffset;
			else if (!_tcscmp(lpEntry, _T("Gain")))
				n = g_CxLibWork.m_CameraStatus.vAmplif;
			else if (!_tcscmp(lpEntry, _T("HighSpeedShutter")))
				n = g_CxLibWork.m_CameraStatus.vShutHS;
			else if (!_tcscmp(lpEntry, _T("LowSpeedShutter")))
				n = g_CxLibWork.m_CameraStatus.vShutLS;
		}
		if (n == -1)
			n = nDef;
	}
	return n;
}

void CBaumerProfile::ValidateInt(LPCTSTR lpSection, LPCTSTR lpEntry, int &nValidate)
{

	if (lpSection && !_tcscmp(lpSection, _T("Settings")))
	{
		if (!_tcscmp(lpEntry, _T("BlackLevel")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vOffsetMin, g_CxLibWork.m_CameraType.vOffsetMax);
		else if (!_tcscmp(lpEntry, _T("Gain")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vAmplMin, g_CxLibWork.m_CameraType.vAmplMax);
		else if (!_tcscmp(lpEntry, _T("HighSpeedShutter")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vShutHsMin, g_CxLibWork.m_CameraType.vShutHsMax);
		else if (!_tcscmp(lpEntry, _T("LowSpeedShutter")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vShutLsMin, g_CxLibWork.m_CameraType.vShutLsMax);
		else if (!_tcscmp(lpEntry, _T("Brightness")))
			DoRange(nValidate, 0, 255);
		else if (!_tcscmp(lpEntry, _T("Contrast")))
			DoRange(nValidate, 0, 127);
		else if (!_tcscmp(lpEntry, _T("Saturation")))
			DoRange(nValidate, 0, 127);
	}
}

CameraIds *CBaumerProfile::ReadCamera()
{
	CameraIds *p;
	if (IsVT5Profile())
	{
		VARIANT var = g_BaumerProfile.GetProfileValue(_T("Settings"), _T("Camera"), true);
		if (var.vt == VT_I2)
			p = CameraIds::CameraDescrByBaumerId(var.iVal);
		else if (var.vt == VT_I4)
			p = CameraIds::CameraDescrByBaumerId(var.lVal);
		else if (var.vt == VT_BSTR)
		{
			CString s(var.bstrVal);
			p = CameraIds::CameraDescrByString(s);
		}
		else
			p = CameraIds::CameraDescrByBaumerId(DEF_CAMERA);
		::VariantClear(&var);
	}
	else
	{
		CString s = g_BaumerProfile.GetProfileString(_T("Settings"), _T("Camera"), NULL, false, true);
		if (s.IsEmpty())
		{
			int n = g_BaumerProfile.GetProfileInt(_T("Settings"), _T("Camera"), DEF_CAMERA, false, true);
			p = CameraIds::CameraDescrByBaumerId(n);
		}
		else if (isdigit_ru(s[0]))
		{
			int n = atoi(s);
			p = CameraIds::CameraDescrByBaumerId(n);
		}
		else
			p = CameraIds::CameraDescrByString(s);
	}
	return p;
}



