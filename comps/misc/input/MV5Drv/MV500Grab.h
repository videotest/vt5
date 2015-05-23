// MV500Grab.h: interface for the CMV500Grab class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MV500GRAB_H__FE369221_A340_11D3_A98A_CE9D0D2F3E70__INCLUDED_)
#define AFX_MV500GRAB_H__FE369221_A340_11D3_A98A_CE9D0D2F3E70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <mv5.h>
#include "DibUtils.h"

interface IMV500GrabSite
{
	virtual void OnTrigger() = 0;
};

class CMV500Grab  
{
protected:
	BYTE *m_lpData;
	int m_nBuffSize; // Size of allocated and locked buffer (may be several frames)
//	int FrameSize() {return m_nBuffSize/m_nCameras;}
	MV5Frame m_Frame;
	CSize m_sz;
	CSize m_szGrab;
	bool m_bOnGrab;
	// It is to support Overlay mode
	bool m_bOverlay;
	CSize m_ScreenSize;
	int m_nScreenMode;
	UINT m_nSurfaceIndex;
	UINT m_nSurfaceStride;
	int m_nOpened; // Is MV5Open called? -1 - not called, 0 - failure, 1 - successfully.
	int m_nDDOpened; // Is MTDD_Open() called? ...
	int m_nCameras; // Number of cameras, usually 1
	int m_nTimer;
	int m_nCurChannel; // Current channel for multi-camera input
	BYTE *m_lpDualBuffer; // Dual camera buffer
	CUIntArray m_arrChannels; // For multy-camera input, channel numbers
	DWORD m_dwStart,m_dwLast,m_dwPeriod; // For presentation purposes
	bool m_bQuickPreview;

	bool InitiallyInit();
	bool MTDD_Open();
public:
	enum BoardType
	{
		Board_Unknown = -1,
		Board_MV500   = 0,
		Board_MV510   = 1,
	};
	enum GrabMode
	{
		GM_Unknown   = 0,
		GM_ForDraw   = 0x1000,
		GM_ForGrab   = 0x1001,
		GM_ForAVIMem = 0x1002,
		GM_ForAVIDsk = 0x1002,
		GM_GS8     = 1,
		GM_8bit    = GM_GS8,
		GM_RGB555  = 2,
		GM_RGB16   = GM_RGB555,
		GM_YUV422  = 3,
		GM_YUV16   = GM_YUV422,
		GM_RGB888  = 4,
		GM_RGB24   = GM_RGB888,
		GM_RGB888x = 5,
		GM_RGB32   = GM_RGB888x,
		GM_Reinit  = 0x10000000,
	};
	bool m_bCanDraw;
	bool m_bFields;
	bool m_bTrigger;
	int m_nTriggerMode;
	static long s_lImageNum;
	static bool s_bResetTrigger;
	IMV500GrabSite *m_pGrabSite;
	void RestartTriggerGrab();
	void ResetTrigger(bool bSlip);
protected:
	BoardType m_BoardType;
	GrabMode  m_GrabMode;

	bool GrabAVIDisk(IMV500ProgressNotify *pNotify);
	bool GrabAVIMemory(IMV500ProgressNotify *pNotify);
	void VerticallyMirrorImage(WORD *pBuffer, WORD *lpDataRaw);
	int  CalcRowSize();
public:
	void CopyGrabbedImage(LPBYTE pDst, LPWORD pSrc, bool bAVI);
	int  CalcBufferSize(bool bCurrentFrame);
	static CMV500Grab *s_pGrab;
	CMV500Grab();
	virtual ~CMV500Grab();

	void Preinit(); // Initializing settings, etc. before access to device
	bool TestHardware();
	bool InitGrab(unsigned gm1, bool bAVI, bool bIgnoreScale = false);
	bool StartGrab(bool bFields = false, int nCamera = 0, bool bWait = false);
	void StopGrab();
	void DeinitGrab();
	void ReinitBrightContrast();
	void ReinitChannel();
	bool GrabAVI(IMV500ProgressNotify *pNotify);
	LPBITMAPINFOHEADER CopyGrabbedImage(int nDevice);
	LPBITMAPINFOHEADER Grab(int nDevice, bool bWait);
	BYTE *GetData() {return m_lpData;}
	CSize GetSize() {return m_szGrab;}
	void Draw(CRect DstRect);
	bool IsOverlay() {return m_bOverlay;}
	bool CanDraw() {return m_bCanDraw&&m_GrabMode!=GM_YUV422;}
	int  GetBitsPerSample();
	BoardType GetBoardType() {return m_BoardType;}
	BOOL IsGrayScale(bool bAVI);
	int  GetCamerasNum() {return m_nCameras;}
	void OnTimer();
	LPBYTE GetRealData(int nDev);
	int GetConnector(int nCamera);
	void SetConnector(int nCamera, int nConnector);
	void SetDefaultConnectors();
	DWORD GetPeriod() {return m_dwPeriod;}
	bool CanCapture();
};

//extern CMV500Grab g_MV500Grab;

#endif // !defined(AFX_MV500GRAB_H__FE369221_A340_11D3_A98A_CE9D0D2F3E70__INCLUDED_)
