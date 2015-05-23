// MV500Grab.cpp: implementation of the CMV500Grab class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <math.h>
#include <mt_ddapi.h>
#include <vfw.h>
#include "resource.h"
#include "MV500Grab.h"
#include "StdProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//CMV500Grab g_MV500Grab;
CMV500Grab *CMV500Grab::s_pGrab;

CMV500Grab::CMV500Grab()
{
	ASSERT(s_pGrab == NULL);
	s_pGrab = this;
	m_lpData = NULL;
	m_nBuffSize = 0;
	m_bOnGrab = FALSE;
	m_bOverlay = false;
	m_nScreenMode = m_nSurfaceIndex = m_nSurfaceStride = 0;
	m_BoardType = Board_Unknown;
	m_GrabMode = GM_Unknown;
	m_bCanDraw = true;
	m_nOpened = -1;
	m_nDDOpened = -1;
	m_lpDualBuffer = NULL;
	m_nCurChannel = 0;
	m_bFields = false;
	m_bQuickPreview = true;
	m_nTimer = 0;
	m_bTrigger = false;
	m_pGrabSite = NULL;
}

CMV500Grab::~CMV500Grab()
{
	DeinitGrab();
	if (m_lpData)
		free(m_lpData);
	if (m_nDDOpened == 1) MTDDraw_Close();
	if (m_nOpened == 1) MV5Close();
	ASSERT(s_pGrab == this);
	s_pGrab = NULL;
	if (m_lpDualBuffer)
		delete m_lpDualBuffer;
}

int MTPROCALL Callback(ULONG flag, void FAR * something)
{
    return 0;
}

long CMV500Grab::s_lImageNum = 0;
bool CMV500Grab::s_bResetTrigger = false;
static bool s_bTriggered = false;
static DWORD dwTicksTrigger = 0;

int MTPROCALL CallbackTrigger(ULONG flag, void FAR * something)
{
	CMV500Grab::s_lImageNum++;
	CMV500Grab::s_bResetTrigger = false;
	s_bTriggered = true;
	dwTicksTrigger = GetTickCount();
    return 0;
}

static int VideoStd(int n)
{
	n += MV5_VideoStandard_NTSC;
	if (n < MV5_VideoStandard_NTSC || n > MV5_VideoStandard_NTSCmono)
		n = MV5_VideoStandard_NTSC;
	return n;
}

bool CMV500Grab::TestHardware()
{
    /*if (MV5Open(MV5_SDKVersion) < 0)
		return false;
	MV5Close();*/
	return true;
}

int CMV500Grab::GetConnector(int nCamera)
{
	if (m_nCameras > 1)
		return m_arrChannels[nCamera];
	else
		return CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Connector"), 0);
}

void CMV500Grab::SetConnector(int nCamera, int nConnector)
{
	if (m_nCameras > 1)
	{
		m_arrChannels[nCamera] = nConnector;
		CString s;
		s.Format(_T("Connector%d"), nCamera);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Multy"), s, nConnector);
	}
	else
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Source"), _T("Connector"), nConnector);
}

void CMV500Grab::SetDefaultConnectors()
{
	m_arrChannels.SetSize(m_nCameras);
	for (int i = 0; i < m_nCameras; i++)
	{
		CString s;
		s.Format(_T("Connector%d"), i);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Multy"), s, i);
		m_arrChannels[i] = i;
	}
}

static void CalcGrabWindow(int xMax, int yMax, int& x, int& y, int& dx, int& dy, bool bAVI)
{
	BOOL bHorzMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE);
	BOOL bVertMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	x = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowX"), 0);
	x = (((x+3)>>2)<<2);
	y = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowY"), 0);
	y = (((y+3)>>2)<<2);
	int nAVIFrameSize = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("ProcGrabbedImage"), 75);
	dx = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowDX"), xMax);
	if (bAVI)
		dx = nAVIFrameSize*dx/100;
	dx = ((dx>>2)<<2);
	dy = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("GrabWindowDY"), yMax);
	if (bAVI)
		dy = nAVIFrameSize*dy/100;
	dy = ((dy>>2)<<2);
	if (bHorzMirror)
		x = xMax-dx-x;
	if (!bVertMirror)
		y = yMax-dy-y;
}

int  CMV500Grab::CalcBufferSize(bool bCurrentFrame)
{
	int nBuffSize;
	if (bCurrentFrame)
	{
#if 0
		nBuffSize = (m_szGrab.cx+4)*(m_szGrab.cy+4)*2;
		if (m_GrabMode == GM_GS8)
			nBuffSize = (m_szGrab.cx+4)*(m_szGrab.cy+4);
		else if (m_GrabMode == GM_RGB888)
			nBuffSize = (m_szGrab.cx+4)*(m_szGrab.cy+4)*3;
#else
		/*
		nBuffSize = (m_sz.cx+4)*(m_sz.cy+4)*2;
		if (m_GrabMode == GM_GS8)
			nBuffSize = (m_sz.cx+4)*(m_sz.cy+4);
		else if (m_GrabMode == GM_RGB888)
		*/
			nBuffSize = (m_sz.cx+4)*(m_sz.cy+4)*3;
#endif
	}
	else
	{
		nBuffSize = (m_szGrab.cx)*(m_szGrab.cy)*2;
		if (m_GrabMode == GM_GS8)
			nBuffSize = (m_szGrab.cx)*(m_szGrab.cy);
		else if (m_GrabMode == GM_RGB888)
			nBuffSize = (m_szGrab.cx)*(m_szGrab.cy)*3;
	}
	return nBuffSize;
}

int  CMV500Grab::CalcRowSize()
{
	int nBuffSize = m_szGrab.cx*2;
	if (m_GrabMode == GM_GS8)
		nBuffSize = m_szGrab.cx;
	else if (m_GrabMode == GM_RGB888)
		nBuffSize = m_szGrab.cx*3;
	return nBuffSize;
}

void CMV500Grab::Preinit()
{
	if (m_nCameras == 0)
	{
		m_nCameras = CStdProfileManager::m_pMgr->GetProfileInt(NULL, _T("CamerasNumber"), 1, true, true);
		m_nCameras = min(max(m_nCameras,1),2);
		if (m_nCameras > 1)
		{
			m_arrChannels.SetSize(m_nCameras);
			for (int i = 0; i < m_nCameras; i++)
			{
				CString s;
				s.Format(_T("Connector%d"), i);
				int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Multy"), s, i, true);
				m_arrChannels[i] = n;
			}
//			m_nMultyTimeout = CStdProfileManager::m_pMgr->GetProfileInt(_T("Multy"), _("Timeout"), 40, true);
//			m_bMultyFill0 = CStdProfileManager::m_pMgr->GetProfileInt(_T("Multy"), _("Fill0"), false, true);
		}
	}
}

bool CMV500Grab::InitiallyInit()
{
	if (m_nOpened == 0) return false;
	if (m_nOpened != 1)
	{
		// Open board and get board info
		if (MV5Open(MV5_SDKVersion) < 0)
		{
			m_nOpened = 0;
			return false;
		}
		MV5Info info;
		if (MV5InquireBoardInfo(&info) == MV5_OK)
		{
			if (info.Board_Id)
				m_BoardType = Board_MV510;
			else
				m_BoardType = Board_MV500;
		}
		else
			m_BoardType = Board_Unknown;
		if (m_BoardType == Board_MV510 && m_nCameras > 1)
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Format"), _T("HorizontalMirror"), FALSE);
		m_nOpened = 1;
	}
	ReinitChannel();
	return true;
}

bool CMV500Grab::MTDD_Open()
{
	if (m_nDDOpened == 0) return false;
	if (m_nDDOpened == 1) return true;
	if (::MTDDraw_Open( NULL, FALSE ) == MV5_OK)
	{
		m_nDDOpened = 1;
		return true;
	}
	else
	{
		m_nDDOpened = 0;
		return false;
	}
}

void CMV500Grab::ReinitChannel()
{
	// Set video standard and connector
	BOOL bCustomCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("CustomCamera"), FALSE, true, true);
	CString sCamFile = CStdProfileManager::m_pMgr->GetProfileString(_T("Source"), _T("CustomCameraFile"), _T("mv5camera.ini"), true, true);
	if (bCustomCamera)
	{
		TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
		_tsplitpath(sCamFile, szDrive, szDir, szFName, szExt);
		GetModuleFileName(0, szPath, _MAX_PATH);
		_tsplitpath(szPath, szDrive[0]?0:szDrive, szDir[0]?0:szDir, NULL, NULL);
		if (strlen(szDir) > 6 && _tcsnicmp(&szDir[_tcslen(szDir)-6],_T("Comps"),5) == 0)
			szDir[strlen(szDir)-6] = 0;
		_tmakepath(szPath, szDrive, szDir, szFName, szExt);
		MV5LoadCameraConfigFile(szPath);
	}
	else
	{
		int nVideoStd = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("VideoStandard"), 0);
		nVideoStd += MV5_VideoStandard_NTSC;
		if (nVideoStd != MV5GetVideoStandard(0))
			MV5SetVideoStandard(nVideoStd);
	}
	int nConnector;
	if (m_nCameras <= 1)
		nConnector = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Connector"), 0);
	else
		nConnector = m_arrChannels[m_nCurChannel];
	if (nConnector != MV5GetVideoChannel())
		MV5SetVideoChannel(nConnector);
}


bool CMV500Grab::InitGrab(unsigned gm1, bool bAVI, bool bIgnoreScale)
{
	GrabMode gm = (GrabMode)(gm1&0xFFFF);
	bool bInitChannel = gm1&0x10000000?true:false;
	// Open board and get board info
	int nWasOpened = m_nOpened;
	if (!InitiallyInit())
		return false;
	// Set video standard and connector
//	if (nWasOpened == 1 && bInitChannel)
//		ReinitChannel();
	int nVideoStd = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("VideoStandard"), 0);
	// Default grab window settings will be affected by video standard
	int nMaxX,nMaxY;
	MV5InquireMaxGrabWindowSize(&nMaxX, &nMaxY);
	int x,y,cx,cy;
//	if (nVideoStd == MV5_VideoStandard_PAL || nVideoStd == MV5_VideoStandard_SECAM || nVideoStd == MV5_VideoStandard_PALm ||
//		nVideoStd == MV5_VideoStandard_PALn || nVideoStd == MV5_VideoStandard_PAL60 || nVideoStd == MV5_VideoStandard_SECAMmono)
//		CalcGrabWindow(768, 576, x, y, cx, cy, bAVI);
//	else
//		CalcGrabWindow(640, 480, x, y, cx, cy, bAVI);
	CalcGrabWindow(nMaxX, nMaxY, x, y, cx, cy, bAVI);
	m_sz = CSize(cx, cy);
	bool bPrevOverlay = m_bOverlay;
	m_bOverlay = false;
	if (gm == GM_ForGrab)
	{
		// For grab the quality of images is most important, but image can be in obtained in some specific format (e.g. YUV)
		// and then converted.
		// For MV-500 in color mode for grab can be used RGB888, YUV442 or RGB555. In gray scale mode RGB888 not
		// needed because it does not give wany advantages over YUV422, but it still will supported.
		// For MV-510, which always operates in gray scale mode, can be used 8bit, YUV442 or RGB555
		if (m_BoardType == Board_MV510)
		{
			if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseGS8", 1, true) && MV5SetPixelMode(MV5_8bit_Mode) == MV5_OK)
				gm = GM_GS8;
			else if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseYUVMode", 1, true) && MV5SetPixelMode(MV5_YUV422_Mode) == MV5_OK)
				gm = GM_YUV422;
			else if (MV5SetPixelMode(MV5_RGB555_Mode) == MV5_OK)
				gm = GM_RGB555;
		}
		else // if board is unknown, assume it is MV-500
		{
			if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseRGB24", 0, true) && MV5SetPixelMode(MV5_RGB888_Mode) == MV5_OK)
				gm = GM_RGB888;
			else if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseYUVMode", 1, true) && MV5SetPixelMode(MV5_YUV422_Mode) == MV5_OK)
				gm = GM_YUV422;
			else if (MV5SetPixelMode(MV5_RGB555_Mode) == MV5_OK)
				gm = GM_RGB555;
		}
		if (gm == GM_ForGrab) // none mode set up
			return false;
	}
	else if (gm == GM_GS8)
		MV5SetPixelMode(MV5_8bit_Mode);
	else if (gm == GM_RGB16)
		MV5SetPixelMode(MV5_RGB555_Mode);
	else if (gm == GM_RGB24)
		MV5SetPixelMode(MV5_RGB888_Mode);
	else if (gm == GM_RGB32)
		MV5SetPixelMode(MV5_RGB888x_Mode);
	else if (gm == GM_YUV16)
		MV5SetPixelMode(MV5_YUV422_Mode);
	else if (gm == GM_ForDraw)
	{
		// For draw overlay mode is available. Image quality not so important as for grab, but image must be 
		// in representable form.
		// For MV-500 can be used RGB888 or RGB555 modes.
		// For MV-510 can be used GS8, RGB888 or RGB555 modes.
		// But YUV overlay is preferrable in any cases.
		bool bOverlayOk = false;
		if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseOverlay", 0, true) &&
			CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE) &&
			m_nCameras == 1 &&
			MTDD_Open())
		{
	        m_ScreenSize = MTDDraw_GetScreenSize();
		    m_nScreenMode = MTDDraw_GetScreenMode();
			bool b = false;
			switch(m_nScreenMode)
			{
			case MV5_8bit_Mode:
	            break; 
			case MV5_RGB888_Mode:
			case MV5_RGB888x_Mode:
			case MV5_RGB555_Mode:
			case MV5_RGB565_Mode:
				b = true;
				break; 
	        default:
				// what shall we do?
				break; 
			}
			if (b && MTDDraw_IsOverlaySupported())
			{
				bOverlayOk = true;
			}
			else
			{
//				MTDDraw_Close();
			}
		}
		if (bOverlayOk)
		{
			HRESULT result;
			result = MTDDraw_CreateOverlayYUV(1, cx, cy, 0, &m_nSurfaceIndex);
			if (result != MV5_OK)
				bOverlayOk = false;
			else
			{
				DWORD dwColorKey = RGB(128,128,0);
				// set DDraw color
				result = MTDDraw_SetDestColorKey(dwColorKey);
			}
		}
		if (bOverlayOk)
		{
			m_bOverlay = true;
			MV5SetPixelMode(MV5_YUV422_Mode);
			gm = GM_YUV422;
		}
		else
		{
			// Overlay mode not set, using DIB in drawable format (GS8, RGB24, RGB 16, see above)
			if (m_BoardType == Board_MV510)
			{
				if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseGS8", 1, true) && MV5SetPixelMode(MV5_8bit_Mode) == MV5_OK)
					gm = GM_GS8;
				else if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseRGB24", 1, true) && MV5SetPixelMode(MV5_RGB888_Mode) == MV5_OK)
					gm = GM_RGB888;
				else if (MV5SetPixelMode(MV5_RGB555_Mode) == MV5_OK)
					gm = GM_RGB555;
			}
			else
			{
				if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseRGB24", 0, true) && MV5SetPixelMode(MV5_RGB888_Mode) == MV5_OK)
					gm = GM_RGB888;
				else if (MV5SetPixelMode(MV5_RGB555_Mode) == MV5_OK)
					gm = GM_RGB555;
			}
			if (gm == GM_ForDraw) // none mode set up
				return false;
		}
	}
	else if (gm == GM_ForAVIMem)
	{
		if (m_BoardType == Board_MV510)
		{
			// For MV-510 optimal is gray scale
			if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseGS8", 1, true) && MV5SetPixelMode(MV5_8bit_Mode) == MV5_OK)
				gm = GM_GS8;
			else if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseYUVMode", 1, true) && MV5SetPixelMode(MV5_YUV422_Mode) == MV5_OK)
				gm = GM_YUV422;
			else if (MV5SetPixelMode(MV5_RGB555_Mode) == MV5_OK)
				gm = GM_RGB555;
		}
		else // if board is unknown, assume it is MV-500
		{
			if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseYUVMode", 1, true) && MV5SetPixelMode(MV5_YUV422_Mode) == MV5_OK)
				gm = GM_YUV422;
			else if (MV5SetPixelMode(MV5_RGB555_Mode) == MV5_OK)
				gm = GM_RGB555;
		}
		if (gm == GM_ForAVIMem) // none mode set up
			return false;
	}
	else
		return false;
	m_GrabMode = gm;
	// Set format settings
	int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Hue"), 0);
	MV5SetHue(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Brightness"), 128);
	MV5SetBrightness(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Contrast"), 128);
	MV5SetContrast(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("VCRMode"), 0);
	MV5SetVCRMode(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("AGC"), FALSE);
	MV5SetAGC(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Saturation"), 128);
	if (IsGrayScale(bAVI))
		n = 0;
	MV5SetSaturation(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), 1);
	if (n != 0 && n != 1)
		n = 0;
	MV5SetMirrorMode(n);
	// set grab window. be sure to set the pixel mode first
	int nScale = bIgnoreScale?0:CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("Scale"), 0);
	float dScale = (float)(nScale==1?2:1);
	m_szGrab = CSize((int)ceil(m_sz.cx/dScale),(int)ceil(m_sz.cy/dScale));
	int nBuffSize = CalcBufferSize(true);
	bool bSizeChanged = nBuffSize != m_nBuffSize;
	if (nBuffSize != m_nBuffSize || m_bOverlay || bPrevOverlay)
	{
		if (!bPrevOverlay && m_nBuffSize && m_lpData)
		{
			free(m_lpData);
			m_lpData = NULL;
		}
		m_nBuffSize = nBuffSize;
		if (m_bOverlay)
		{
			// find the VGA memory address so that we can DMA to it
			m_lpData = MTDDraw_GetSurfaceMemoryPtr(m_nSurfaceIndex, &m_nSurfaceStride);
		}
		else
		{
			m_lpData = (BYTE *)malloc(m_nBuffSize);
			memset(m_lpData, 0, m_nBuffSize);
		}
	}
	if (m_nCameras > 1)
	{
		if (bSizeChanged || m_lpDualBuffer ==  NULL )
		{
			delete m_lpDualBuffer;
			m_lpDualBuffer = new BYTE[2*m_nBuffSize];
			memset(m_lpDualBuffer, 0, 2*m_nBuffSize);
		}
	}
	else if (m_lpDualBuffer)
	{
		delete m_lpDualBuffer;
		m_lpDualBuffer = NULL;
	}

    MV5SetGrabWindow(x, y, cx, cy, &m_Frame);
	MV5SetHScaleFactor(dScale);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Use_MV5SetVScaleFactor"), TRUE, true, true))
		MV5SetVScaleFactor(dScale);
	MV5ConnectInterruptToCallback(0, Callback, MV5_MXfer_Int, NULL);
	m_bTrigger = CStdProfileManager::m_pMgr->GetProfileInt(_T("Trigger"), _T("Enable"), FALSE, true, true)==TRUE;
	int nTriggerPolarity = CStdProfileManager::m_pMgr->GetProfileInt(_T("Trigger"),
		_T("TriggerPolarityHigh"), FALSE, true, true);
	MV5SetTriggerPolarity(nTriggerPolarity?MV5_Active_High:MV5_Active_Low);
	MV5ConnectInterruptToCallback(0, CallbackTrigger, MV5_Trigger_Int, NULL);
	m_nTriggerMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Trigger"), _T("TriggerMode"), 0, true, true);
	return true;
}

//CMultyCamThread *pThread;


VOID CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	CMV500Grab::s_pGrab->OnTimer();
}

 
bool CMV500Grab::StartGrab(bool bFields, int nCamera, bool bWait)
{
	if (m_nCameras > 1)
	{
		m_nCurChannel = nCamera;
		MV5SetVideoChannel(m_arrChannels[nCamera]);
	}
	if (m_bOnGrab) return true;
	if (!m_bOverlay)
		MV5LockMasterBuffer(m_lpData, m_nBuffSize);
	// set the DMA control
	ASSERT(m_nCameras == 1 || !m_bOverlay);
	if (!m_bOverlay)
	{
		if (bFields)
		{
			int n = CalcBufferSize(false);
			MV5SetMasterModeTransferField(&m_Frame, m_lpData, n/2, m_szGrab.cx, 0);
			MV5SetMasterModeTransferField(&m_Frame, m_lpData+n/2, n/2, m_szGrab.cx, 1);
		}
		else
			MV5SetMasterModeTransfer(&m_Frame, m_lpData, m_nBuffSize, m_szGrab.cx);
	}
	else
		MV5SetMasterModeTransfer(&m_Frame, m_lpData, m_nBuffSize, m_nSurfaceStride/2);
//	MV5IRQEnable(
//		MV5_MXfer_Int,     // use MV5_MXfer_Int will generate
								// 60 interrupt per second for NTSC
//		Callback);
	// OK to start grab
	if (m_bTrigger)
	{
		MV5IRQEnable(MV5_Trigger_Int, CallbackTrigger);
		RestartTriggerGrab();
	}
	else
	{
		int r = MV5StartGrab(MV5_Cont_Grab, MV5_Grab_Odd);
		TRACE("Start grab called, r = %d, time = %d\n", r, GetTickCount());
	}
	m_dwStart = m_dwLast = GetTickCount();
	m_dwPeriod = 0;
	m_bOnGrab = true;
	m_bFields = bFields;
	if (bWait)
	{
		for (int i = 0; i < 4; i++)
			MV5WaitForVSync();
		if (m_nCameras > 1)
			memcpy(m_lpDualBuffer+m_nCurChannel*m_nBuffSize, m_lpData, m_nBuffSize);
	}
	if (m_nCameras > 1 || m_bTrigger)
		m_nTimer = SetTimer(NULL, 1, m_bTrigger?40:100, TimerProc);
	return true;
}

void CMV500Grab::RestartTriggerGrab()
{
	int r;
	TRACE("RestartTriggerGrab, time = %d\n", GetTickCount());
	if (m_nTriggerMode == 1)
		r = MV5StartGrab(MV5_Single_Grab|MV5_Grab_StartBy_Trigger, MV5_Grab_Odd);
	else if (m_nTriggerMode == 2)
		r = MV5StartGrab(MV5_Cont_Grab|MV5_Grab_StopBy_Trigger, MV5_Grab_Odd);
	else
		r = MV5StartGrab(MV5_Cont_Grab|MV5_Grab_StartBy_Trigger, MV5_Grab_Odd);
}

void CMV500Grab::StopGrab()
{
	if (m_bOnGrab)
	{
		if (m_nCameras > 1 || m_bTrigger)
		{
			KillTimer(NULL,m_nTimer);
			m_nTimer = 0;
		}
		
		// Disable the interrupts for the callback
//		MV5IRQDisable(MV5_MXfer_Int, Callback);
		// stop grab
		if (m_bTrigger)
		{
			MV5IRQDisable(MV5_Trigger_Int, CallbackTrigger);
			s_bTriggered = false;
		}
		TRACE("MV5StopGrab called\n");
		MV5StopGrab(NULL, NULL);
		if (!m_bOverlay)
			MV5UnlockMasterBuffer(m_lpData, m_nBuffSize);
	}
	m_bOnGrab = false; // saying it is not grabbing
}

void CMV500Grab::DeinitGrab()
{
    StopGrab();
	if (m_lpData)
	{
		// unregister the callback
		MV5DisconnectInterruptCallback(0, Callback);
		MV5DisconnectInterruptCallback(0, CallbackTrigger);
		// Terminate video operation for MV-500 board
		if (m_bOverlay)
		{
	        MTDDraw_DestroyOverlayYUV(m_nSurfaceIndex);
			m_bOverlay = false;
			m_lpData = NULL;
			m_nBuffSize = 0;
		}
	}
}

void CMV500Grab::OnTimer()
{
	if (m_nCameras > 1)
	{
		DWORD dwBeg = GetTickCount();
		if (!m_bFields && dwBeg - m_dwLast < 40)
			return;
		MV5StopGrab(NULL, NULL);
		if (m_bFields)
		{
			int nBuffSize = CalcBufferSize(false);
			int nField = MV5GetFieldInfo();
			if (nField == 1)
				memcpy(m_lpDualBuffer+m_nCurChannel*m_nBuffSize, m_lpData, nBuffSize/2);
			else if (nField == 0)
				memcpy(m_lpDualBuffer+m_nCurChannel*m_nBuffSize, m_lpData+nBuffSize/2, nBuffSize/2);
		}
		else
			memcpy(m_lpDualBuffer+m_nCurChannel*m_nBuffSize, m_lpData, m_nBuffSize);
		m_nCurChannel = m_nCurChannel>=m_arrChannels.GetSize()-1?0:m_nCurChannel+1;
		MV5SetVideoChannel(m_arrChannels[m_nCurChannel]);
		memset(m_lpData, 0, m_nBuffSize);
		MV5StartGrab(MV5_Cont_Grab, m_bFields?MV5_Grab_Next:MV5_Grab_Odd);
		DWORD dwTicks = GetTickCount();
		m_dwPeriod = dwTicks-m_dwLast;
		m_dwLast = dwTicks;
	}
	else if (m_bOnGrab && m_bTrigger)
		ResetTrigger(true);
}

void CMV500Grab::ResetTrigger(bool bSlip)
{
	if (m_bOnGrab && m_bTrigger)
	{
		if (s_bTriggered && s_bResetTrigger)
		{
			s_bTriggered = false;
			s_bResetTrigger = false;
			if (bSlip) Sleep(40);
			DWORD dw = GetTickCount();
			TRACE("ResetTrigger, trigger at %d, now = %d, diff = %d, image %d\n",
				dwTicksTrigger, dw,	dw-dwTicksTrigger, s_lImageNum);
			int r = MV5ResetTrigger();
			RestartTriggerGrab();
		}
	}
}

LPBYTE CMV500Grab::GetRealData(int nDev)
{
	if (m_nCameras <= 1)
		return m_lpData;
	else
		return m_lpDualBuffer+nDev*m_nBuffSize;
}

bool CMV500Grab::CanCapture()
{
	return !(m_bFields || m_bOverlay || m_GrabMode == GM_RGB555 && CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseYUVMode", 1, true));
}


void CMV500Grab::ReinitBrightContrast()
{
	int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Hue"), 0);
	MV5SetHue(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Brightness"), 128);
	MV5SetBrightness(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Contrast"), 128);
	MV5SetContrast(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("VCRMode"), 0);
	MV5SetVCRMode(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("AGC"), FALSE);
	MV5SetAGC(n);
	n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Source"), _T("Saturation"), 128);
	if (IsGrayScale(false))
		n = 0;
	MV5SetSaturation(n);
}

void CMV500Grab::Draw(CRect DestRect)
{
	CRect SrcRect(0, 0, m_szGrab.cx, m_szGrab.cy);
    int delta;
    if (DestRect.bottom > m_ScreenSize.cy)
    {
        delta = DestRect.bottom - m_ScreenSize.cy;
        DestRect.bottom = m_ScreenSize.cy;
        SrcRect.bottom -= delta;
    }
    if (DestRect.right > m_ScreenSize.cx)
    {
        delta = DestRect.right - m_ScreenSize.cx;
        DestRect.right = m_ScreenSize.cx;
        SrcRect.right -= delta;
    }
    if (DestRect.left < 0)
    {
        delta = -DestRect.left;
        DestRect.left = 0;
        SrcRect.left += delta;
    }
    MTDDraw_MapSurfaceRect(m_nSurfaceIndex, &DestRect, &SrcRect, FALSE);
}

int CMV500Grab::GetBitsPerSample()
{
	if (m_GrabMode == GM_GS8)
		return 8;
	else if (m_GrabMode == GM_RGB555)
		return 16;
	else if (m_GrabMode == GM_YUV422)
		return 16;
	else if (m_GrabMode == GM_RGB888)
		return 24;
	else if (m_GrabMode == GM_RGB888x)
		return 32;
	else
		return 0;
}

BOOL CMV500Grab::IsGrayScale(bool bAVI)
{
	if (m_BoardType == Board_MV510)
		return TRUE;
	else
		if (bAVI)
			return CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("GrayScale"), 0);
		else
			return CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("GrayScale"), 0);
}

LPBITMAPINFOHEADER CMV500Grab::CopyGrabbedImage(int nDev)
{
	BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	BOOL bHorizontalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE);
	LPBITMAPINFOHEADER lpbi = NULL;
//	BOOL bGrayScale = AfxGetApp()->GetProfileInt(_T("Format"), _T("GrayScale"), 0);
	BOOL bGrayScale = IsGrayScale(false);
	if (m_GrabMode == GM_GS8)
	{
		ASSERT(bGrayScale);
		DWORD dwRowSrc = ((m_szGrab.cx+3)>>2)<<2;
		DWORD dwRowDst = ((m_szGrab.cx+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(m_szGrab, true);
		if (!lpbi) return NULL;
		_CopyGS8(bVerticalMirror, bHorizontalMirror, _GetData(lpbi), (BYTE *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_YUV422)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(m_szGrab, bGrayScale?true:false);
		if (!lpbi) return NULL;
		_CopyYUV422(bGrayScale, bVerticalMirror, (BYTE *)_GetData(lpbi), (WORD *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_RGB888)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*3+3)>>2)<<2;
		DWORD dwRowDst =  bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(m_szGrab, bGrayScale?true:false);
		if (!lpbi) return NULL;
		_CopyRGB888(bGrayScale, bVerticalMirror, (BYTE *)_GetData(lpbi), (BYTE *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_RGB555)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		lpbi = _AllocBitmapInfo(m_szGrab, bGrayScale?true:false);
		if (!lpbi) return NULL;
		_CopyRGB555(bGrayScale, bVerticalMirror, (BYTE *)_GetData(lpbi), (WORD *)GetRealData(nDev), dwRowDst, dwRowSrc, m_szGrab);
	}
	return lpbi;
}

void CMV500Grab::CopyGrabbedImage(LPBYTE pDst, LPWORD pSrc, bool bAVI)
{
	BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	BOOL bHorizontalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("HorizontalMirror"), TRUE, true);
	BOOL bGrayScale = IsGrayScale(bAVI);
	if (m_GrabMode == GM_GS8)
	{
		ASSERT(bGrayScale);
		DWORD dwRowSrc = ((m_szGrab.cx+3)>>2)<<2;
		DWORD dwRowDst = ((m_szGrab.cx+3)>>2)<<2;
		_CopyGS8(bVerticalMirror, bHorizontalMirror, pDst, (BYTE *)pSrc, dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_YUV422)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		_CopyYUV422(bGrayScale, bVerticalMirror, pDst, pSrc, dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_RGB888)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*3+3)>>2)<<2;
		DWORD dwRowDst =  bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		_CopyRGB888(bGrayScale, bVerticalMirror, pDst, (BYTE *)pSrc, dwRowDst, dwRowSrc, m_szGrab);
	}
	else if (m_GrabMode == GM_RGB555)
	{
		DWORD dwRowSrc = ((m_szGrab.cx*2+3)>>2)<<2;
		DWORD dwRowDst = bGrayScale?((m_szGrab.cx+3)>>2)<<2:((m_szGrab.cx*3+3)>>2)<<2;
		_CopyRGB555(bGrayScale, bVerticalMirror, pDst, pSrc, dwRowDst, dwRowSrc, m_szGrab);
	}
}

void CMV500Grab::VerticallyMirrorImage(WORD *pBuffer, WORD *lpDataRaw)
{
	DWORD dwRow = ((m_szGrab.cx*2+1)>>1)<<1;
	WORD *pSrc1 = lpDataRaw;
	WORD *pSrc2 = lpDataRaw+(dwRow/2)*(m_szGrab.cy-1);
	for (int y = 0; y < m_szGrab.cy/2; y++)
	{
		memcpy(pBuffer, pSrc1, dwRow);
		memcpy(pSrc2, pSrc1, dwRow);
		memcpy(pSrc1, pBuffer, dwRow);
		pSrc1 += dwRow/2;
		pSrc2 -= dwRow/2;
	}
	memset(lpDataRaw, 255, 100000);
}

LPBITMAPINFOHEADER CMV500Grab::Grab(int nDev, bool bWait)
{
	if (bWait)
	{
		for (int i = 0; i < 4; i++)
			MV5WaitForVSync();
	}
	return CopyGrabbedImage(nDev);
}

bool CMV500Grab::GrabAVIMemory(IMV500ProgressNotify *pNotify)
{
	ASSERT(m_nCameras == 1);
	// Obtain frame rate and number of frames
	DWORD dwMSecs = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("FPS"), 25);
	bool bFields = false;
	BOOL bTimeBased;
	if (dwMSecs > 25)
	{
		dwMSecs = 25;
		bFields = true;
		bTimeBased = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("TimeBasedCaptureFieldsToMemory"), FALSE, true);
	}
	else
		bTimeBased = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("TimeBasedCaptureFrameToMemory"), TRUE, true);
	int nFPS = dwMSecs;
	if (dwMSecs == 0 || dwMSecs >= 1000) return false;
	dwMSecs = 1000/dwMSecs;
	BOOL bWriteToMemory = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("CaptureToMemory"), 0)==0;
	int nFramesToWrite = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("Time"), 1)*1000/dwMSecs;
	if (nFramesToWrite < 0) return false;
	int nFramesWritten = 0;
	int nBuffSize = 0;
	int nTimeout = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GrabTimeout"), 100, true);
	CPtrArray PtrBuff; // for grabbed frames
	CByteArray Flags;
	CWaitCursor wc;
	MEMORYSTATUS MemStatus;
	bool bUseAviSettings = false;
	m_bCanDraw = false;
	CString sName = CStdProfileManager::m_pMgr->GetProfileString(_T("AVI"), _T("FileName"), _T("c:\\capture.avi"));
	if (InitGrab(GM_ForAVIMem,bUseAviSettings,bFields))
	{
		// Calc needed size of buffer
		if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "SmallBuffer", 1, true))
			nBuffSize = CalcBufferSize(false);
		else
			nBuffSize = m_nBuffSize;
		// Pre-allocate needed buffers
		if (bWriteToMemory)
		{
			memset(&MemStatus, 0, sizeof(MemStatus));
			MemStatus.dwLength = sizeof(MemStatus);
			GlobalMemoryStatus(&MemStatus);
			int nBuffs = (((int)MemStatus.dwAvailPhys)-2*1024)/nBuffSize;
			if (nBuffs < nFramesToWrite)
			{
				DeinitGrab();
				CString s;
				s.Format(IDS_NO_MEMORY_FOR_VCAP, MemStatus.dwAvailPhys/1024, nBuffs);
				AfxMessageBox(s);
				unlink(sName);
				m_bCanDraw = true;
				return false;
			}
		}
		PtrBuff.SetSize(nFramesToWrite);
		Flags.SetSize(nFramesToWrite);
		for (int i = 0; i < nFramesToWrite; i++)
		{
			LPVOID lpBuff = VirtualAlloc(NULL, nBuffSize, MEM_COMMIT, PAGE_READWRITE);
			if (lpBuff == NULL)
			{
				for (int j = 0; j < i; j++)
				{
					VirtualFree(PtrBuff[j], nBuffSize, MEM_DECOMMIT);
					VirtualFree(PtrBuff[j], nBuffSize, MEM_RELEASE);
				}
				DeinitGrab();
				AfxMessageBox("Not enough buffer memory");
				m_bCanDraw = true;
				return false;
			}
			PtrBuff.SetAt(i, lpBuff);
			Flags.SetAt(i, 0);
		}
		// Process capture to memory buffers
		int nFrame = 0;
		pNotify->Start("Record", nFramesToWrite);
		if (StartGrab(bFields))
		{
			if (bTimeBased)
			{
				Sleep(nTimeout);
				DWORD dwBegin = timeGetTime();
				for (int i = 0; i < nFramesToWrite; i++)
				{
					if (timeGetTime() > dwBegin+(i+1)*dwMSecs)
						continue;
					memcpy(PtrBuff[i], m_lpData, nBuffSize);
					Flags.SetAt(i, 1);
					if (i != nFramesToWrite-1)
					{
						DWORD dwWait = dwBegin+(i+1)*dwMSecs;
						while(timeGetTime() < dwWait);
					}
					if ((i%nFPS)==0)
						pNotify->Notify(i);
				}
			}
			else
			{
				int nFrame = 0;
				Sleep(nTimeout);
//				while (MV5GetFieldInfo() == 0);
//				while (MV5GetFieldInfo() == 1);
				BOOL bGrabOnField0 = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GrabOnField0"),
					FALSE, true);
				DWORD dwBegin = 0;//timeGetTime();
				for (;;)
				{
					DWORD dwStartWait = timeGetTime();
					if (bGrabOnField0)
					{
						while (MV5GetFieldInfo() == 0);
						while (MV5GetFieldInfo() == 1);
					}
					else
					{
						while (MV5GetFieldInfo() == 1);
						while (MV5GetFieldInfo() == 0);
					}
					DWORD dwNow = timeGetTime();
					if (dwBegin == 0) dwBegin = dwNow;
					nFrame = (dwNow-dwBegin+dwMSecs/2)/dwMSecs;
					TRACE2("Frame %d(%d ms), start %d ms", nFrame, dwNow);
					TRACE2("start wait %d ms, start copy %d ms", dwStartWait-dwBegin, dwNow-dwBegin);
					if (nFrame >= nFramesToWrite)
						break;
					memcpy(PtrBuff[nFrame], m_lpData, nBuffSize);
					Flags.SetAt(nFrame, 1);
					TRACE1(", ends %d ms\n", (timeGetTime()-dwBegin));
					if ((nFrame%nFPS)==0)
						pNotify->Notify(nFrame);
				}
			}
			StopGrab();
		}
		pNotify->End();
		DeinitGrab();
	}
	// Now save data from buffers to disk file
	BOOL bGrayScale = IsGrayScale(bUseAviSettings);
	int nScale = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("Scale"), 0);
	PAVIFILE pf; 
	AVIFileInit();
	HRESULT hr = AVIFileOpen(&pf, sName, OF_WRITE|OF_CREATE, NULL); 
	if (SUCCEEDED(hr))
	{
		DWORD dwMSecsPB = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("PlaybackFPS"), 25);
		if (dwMSecsPB > 25)	dwMSecsPB = 50;
		if (dwMSecsPB == 0) dwMSecsPB = 1;
		dwMSecsPB = 1000/dwMSecsPB;
	    AVISTREAMINFO strhdr;
		PAVISTREAM ps;
		memset(&strhdr, 0, sizeof(strhdr));
		strhdr.fccType = streamtypeVIDEO;
//		strhdr.dwScale = bFields?dwMSecsPB*500:dwMSecsPB*1000;
		strhdr.dwScale = dwMSecsPB*1000;
		strhdr.dwRate = 1000000;
		strhdr.dwQuality = 10000;
		SetRect(&strhdr.rcFrame, 0, 0, (int)m_szGrab.cx, (int)m_szGrab.cy); 
		hr = AVIFileCreateStream(pf, &ps, &strhdr);
		if (SUCCEEDED(hr))
		{
			BITMAPINFO256 bi;
			memset(&bi.bmiHeader, 0, sizeof(bi));
			bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
			bi.bmiHeader.biPlanes = 1;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biBitCount = bGrayScale?8:24;
			if (bFields && nScale)
			{
				// In this case image data will be scaled manually (only horizontally).
				bi.bmiHeader.biWidth = m_szGrab.cx/2;
				bi.bmiHeader.biHeight = m_szGrab.cy/2;
			}
			else
			{
				bi.bmiHeader.biWidth = m_szGrab.cx;
				bi.bmiHeader.biHeight = m_szGrab.cy;
			}
			if (bGrayScale)
			{
				bi.bmiHeader.biClrUsed = 256;
				bi.bmiHeader.biClrImportant = 256;
				for (int i = 0; i < 256; i++)
				{
					bi.bmiColors[i].rgbBlue = bi.bmiColors[i].rgbGreen = bi.bmiColors[i].rgbRed = i;
					bi.bmiColors[i].rgbReserved = 0;
				}
				hr = AVIStreamSetFormat(ps, 0, &bi, sizeof(bi)); 
			}
			else
				hr = AVIStreamSetFormat(ps, 0, &bi, sizeof(bi.bmiHeader)); 
			DWORD dwSize = bGrayScale?(((bi.bmiHeader.biWidth+3)>>2)<<2)*bi.bmiHeader.biHeight:
				(((3*bi.bmiHeader.biWidth+3)>>2)<<2)*bi.bmiHeader.biHeight;
			if (bFields)
			{
				// Special processing with fields decomposition and possibly manual scaling.
//				BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
//				int n1 = bVerticalMirror?0:1;
//				int n2 = bVerticalMirror?1:0;
				DWORD dwSizeSrc = bGrayScale?(((m_szGrab.cx+3)>>2)<<2)*m_szGrab.cy:
					(((3*m_szGrab.cx+3)>>2)<<2)*m_szGrab.cy;
				DWORD dwRowSrc = bGrayScale?(((m_szGrab.cx+3)>>2)<<2):
					(((3*m_szGrab.cx+3)>>2)<<2);
				DWORD dwRowDst = bGrayScale?(((bi.bmiHeader.biWidth+3)>>2)<<2):
					(((3*bi.bmiHeader.biWidth+3)>>2)<<2);
				LPVOID lpData1 = malloc(dwSizeSrc);
				LPVOID lpData2 = malloc(dwSize);
				int n1,n2;
				LPBYTE lpField1,lpField2;
				GetFieldsValues(n1, n2, (LPBYTE)lpData1, lpField1, lpField2, dwRowSrc, m_szGrab.cy);
				pNotify->Start("Saving", 2*nFramesToWrite);
				for (int i = 0; i < nFramesToWrite; i++)
				{
					if (Flags[i])
					{
						CopyGrabbedImage((LPBYTE)lpData1, (LPWORD)PtrBuff[i], bUseAviSettings);
						if (nScale)
							HScaleImage2((LPBYTE)lpData2, lpField1,
								bGrayScale?true:false, m_szGrab.cx,	m_szGrab.cy);
						else
							GetFieldFromImage((LPBYTE)lpData2, (LPBYTE)lpData1, n1, dwRowDst, m_szGrab.cy);
						hr = AVIStreamWrite(ps, 2*i, 1, lpData2, dwSize, AVIIF_KEYFRAME, NULL, NULL);
						if (nScale)
							HScaleImage2((LPBYTE)lpData2, lpField2,
								bGrayScale?true:false, m_szGrab.cx, m_szGrab.cy);
						else
							GetFieldFromImage((LPBYTE)lpData2, (LPBYTE)lpData1, n2, dwRowDst, m_szGrab.cy);
						hr = AVIStreamWrite(ps, 2*i+1, 1, lpData2, dwSize, AVIIF_KEYFRAME, NULL, NULL);
						nFramesWritten+=2;
						pNotify->Notify(2*i);
					}
				}
				pNotify->End();
				free(lpData1);
				free(lpData2);
			}
			else
			{
				LPVOID lpData = malloc(dwSize);
				pNotify->Start("Saving", nFramesToWrite);
				for (int i = 0; i < nFramesToWrite; i++)
				{
					if (Flags[i])
					{
						CopyGrabbedImage((LPBYTE)lpData, (LPWORD)PtrBuff[i], bUseAviSettings);
						hr = AVIStreamWrite(ps, i, 1, lpData, dwSize, AVIIF_KEYFRAME, NULL, NULL);
						nFramesWritten++;
						pNotify->Notify(i);
					}
				}
				pNotify->End();
				free(lpData);
			}
			AVIStreamRelease(ps);
		}
		AVIFileRelease(pf);
	}
	else
	{
		AVIFileExit();
		CString s;
		s.Format(IDS_FILE_NOT_OPENED, sName);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		m_bCanDraw = true;
		return false;
	}
	AVIFileExit();
	for (int i = 0; i < nFramesToWrite; i++)
	{
		VirtualFree(PtrBuff[i], nBuffSize, MEM_DECOMMIT);
		VirtualFree(PtrBuff[i], nBuffSize, MEM_RELEASE);
	}
	if (bFields) nFramesToWrite *= 2;
	if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "ShowMessage", 0, true) ||
		CStdProfileManager::m_pMgr->GetProfileInt("Settings", "ShowMessageWhenSkipped", 1, true) &&
		nFramesToWrite-nFramesWritten != 0)
	{
		CString sMsg;
		if (bFields)
			sMsg.Format(IDS_AVI_PLACED, sName, nFramesWritten, nFramesToWrite-nFramesWritten, ((double)nFramesToWrite)*dwMSecs/1000);
		else
			sMsg.Format(IDS_AVI_PLACED, sName, nFramesWritten, nFramesToWrite-nFramesWritten, ((double)nFramesToWrite)*dwMSecs/1000);
		AfxMessageBox(sMsg);
	}
	m_bCanDraw = true;
	return true;
}

bool CMV500Grab::GrabAVIDisk(IMV500ProgressNotify *pNotify)
{
	ASSERT(m_nCameras == 1);
	CWaitCursor wc;
	// Init variables
	int nTimeout = CStdProfileManager::m_pMgr->GetProfileInt("Settings", "GrabTimeout", 100, true);
	int nFPS = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("FPS"), 25);
	bool bFields = false;
	BOOL bTimeBased;
	if (nFPS > 25)
	{
		nFPS = 25;
		bFields = true;
		bTimeBased = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("TimeBasedCaptureFieldsToDisk"), FALSE, true);
	}
	else
		bTimeBased = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("TimeBasedCaptureFrameToDisk"), TRUE, true);
	DWORD dwMSecs = nFPS;
	if (dwMSecs == 0 || dwMSecs >= 1000) return false;
	dwMSecs = 1000/dwMSecs;
	int nPlaybackFPS = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("PlaybackFPS"), 25);
	if (nPlaybackFPS == 0 || nPlaybackFPS >= 1000) nPlaybackFPS = 1;
	DWORD dwMSecsPB = 1000/nPlaybackFPS;
	int nFramesToWrite = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("Time"), 1)*1000/dwMSecs;
	if (nFramesToWrite < 0) return false;
	int nFramesWritten = 0;
	CString sName = CStdProfileManager::m_pMgr->GetProfileString(_T("AVI"), _T("FileName"), _T("c:\\capture.avi"));
	BOOL bGrayScale = IsGrayScale(true); // For MV-510 always grayscale
	BOOL bVerticalMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("VerticalMirror"), TRUE);
	BOOL bUseYUV = CStdProfileManager::m_pMgr->GetProfileInt("Settings", "UseYUVMode", 1, true);
	BOOL bNotShowMessage = FALSE; // Not show success message
	int nScale = CStdProfileManager::m_pMgr->GetProfileInt(_T("Format"), _T("Scale"), 0);
	PAVIFILE pf;
	AVIFileInit();
	// If furthermore decompression needed then create temporary file
	CString sRec;
	if (m_BoardType!=Board_MV510&&!bGrayScale&&bUseYUV||bFields)
	{
		sRec = _TempAVI(sName);
		unlink(sName); // To prevent fragmantation of disk space
	}
	else
		sRec = sName;
	// Select appropriate grab mode
	GrabMode gm;
	if (m_BoardType == Board_MV510)
		gm = GM_GS8; // Hardware support of grayscale images
	else if (bGrayScale)
		gm = GM_YUV422; // For conversion into grayscale
	else if (bUseYUV)
		gm = GM_YUV422; // Whole AVI will be decompressed after recording finished
	else
		gm = GM_RGB16;
	if (!CStdProfileManager::m_pMgr->GetProfileInt("Settings", "SkipCheckAvailableDiskSpace", 0, true))
	{
		// Calculate buffer size for recording
		DWORD dwSize = gm==GM_GS8||bGrayScale?(((m_szGrab.cx+3)>>2)<<2)*m_szGrab.cy:
			(((2*m_szGrab.cx+1)>>1)<<1)*m_szGrab.cy;
		if (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "CheckForPartitionFree", false, true) &&
			!IsPartitionFree(sRec))
		{
			CString s;
//			s.Format(IDS_PARTITION_NOT_FREE, int(sRec[0]));
//			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
		// Check for available disk space.
		if (m_BoardType!=Board_MV510&&!bGrayScale&&bUseYUV)
		{
			DWORD dwSize1 = (((3*m_szGrab.cx+3)>>2)<<2)*m_szGrab.cy;
			if (!IsEnoughDiskSpace(sRec,nFramesToWrite,dwSize,nFramesToWrite,dwSize1))
			{
				m_bCanDraw = true;
				return false;
			}
		}
		else
			if (!IsEnoughDiskSpace(sRec,nFramesToWrite,dwSize,0,0))
			{
				m_bCanDraw = true;
				return false;
			}
	}
	m_bCanDraw = false;
	if (InitGrab(gm,true,bFields))
	{
	// Create AVI file
		HRESULT hr = AVIFileOpen(&pf, sRec, OF_WRITE|OF_CREATE, NULL);
		if (SUCCEEDED(hr))
		{
			// Create AVI stream
			AVISTREAMINFO strhdr;
			PAVISTREAM ps;
			memset(&strhdr, 0, sizeof(strhdr));
			strhdr.fccType = streamtypeVIDEO;
			strhdr.dwScale = dwMSecsPB*1000; 
			strhdr.dwRate = 1000000;
			strhdr.dwQuality = 10000;
			SetRect(&strhdr.rcFrame, 0, 0, (int)m_szGrab.cx, (int)m_szGrab.cy); 
			hr = AVIFileCreateStream(pf, &ps, &strhdr);
			if (SUCCEEDED(hr))
			{
				// Set stream format
				BITMAPINFO256 bi;
				memset(&bi.bmiHeader, 0, sizeof(bi));
				bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
				bi.bmiHeader.biPlanes = 1;
				bi.bmiHeader.biCompression = gm==GM_YUV422&&!bGrayScale?mmioFOURCC('M','5','0','0'):BI_RGB;
				bi.bmiHeader.biBitCount = gm==GM_GS8||bGrayScale?8:16;
				bi.bmiHeader.biWidth = m_szGrab.cx;
				bi.bmiHeader.biHeight = m_szGrab.cy;
				if (gm==GM_GS8||bGrayScale)
				{
					// If image is gray scale, then initialize gray palette
					for (int i = 0; i < 256; i++)
					{
						bi.bmiColors[i].rgbBlue = i;
						bi.bmiColors[i].rgbRed = i;
						bi.bmiColors[i].rgbGreen = i;
						bi.bmiColors[i].rgbReserved = 0;
					}
					hr = AVIStreamSetFormat(ps, 0, &bi, sizeof(bi)); 
				}
				else
					hr = AVIStreamSetFormat(ps, 0, &bi, sizeof(bi.bmiHeader)); 
				// Calculate buffer size for recording
				DWORD dwSize = gm==GM_GS8||bGrayScale?(((m_szGrab.cx+3)>>2)<<2)*m_szGrab.cy:
					(((2*m_szGrab.cx+1)>>1)<<1)*m_szGrab.cy;
				// Allocate buffer for non-converted and converted data
				LPVOID lpDataRaw = malloc(m_nBuffSize);
				memset(lpDataRaw,0,m_nBuffSize);
				// Row buffer for vertical flipping
				DWORD dwRowSrc = gm==GM_GS8?(((bi.bmiHeader.biWidth+3)>>2)<<2):
					(((2*bi.bmiHeader.biWidth+1)>>1)<<1);
				DWORD dwRowDst = gm==GM_GS8||bGrayScale?(((bi.bmiHeader.biWidth+3)>>2)<<2):
					(((2*bi.bmiHeader.biWidth+1)>>1)<<1);
				BYTE *lpDataEnd = ((BYTE *)lpDataRaw)+(m_szGrab.cy-1)*dwRowDst;
				// Now wee can start grab
				if (StartGrab(bFields))
				{
					Sleep(nTimeout);
					if (bTimeBased)
					{
						DWORD dwBegin = timeGetTime();
						pNotify->Start("Record", nFramesToWrite);
						for (int i = 0; i < nFramesToWrite; i++)
						{
							if (timeGetTime() > dwBegin+(i+1)*dwMSecs)
								continue;
							if (bGrayScale && gm == GM_YUV422)
							{
								if (bVerticalMirror)
									_YuvCCIR601_422ToY_Vert(m_lpData, lpDataEnd, dwRowSrc, dwRowDst, m_szGrab.cy);
								else
									MV5YuvCCIR601_422ToY(m_lpData, lpDataRaw, dwSize);
							}
							else
								memcpy(lpDataRaw, m_lpData, m_nBuffSize); // To avoid mixing data from different frames
							hr = AVIStreamWrite(ps, i, 1, lpDataRaw, dwSize, 0/*AVIIF_KEYFRAME*/, NULL, NULL);
							nFramesWritten++;
							if (i != nFramesToWrite-1)
							{
								DWORD dwWait = dwBegin+(i+1)*dwMSecs;
								while(timeGetTime() < dwWait);
							}
							if ((i%nFPS)==0)
								pNotify->Notify(i);
						}
					}
					else
					{
						int nFrame = 0;
						BOOL *pbFlags = new BOOL[nFramesToWrite];
						memset(pbFlags, 0, nFramesToWrite*sizeof(BOOL));
						BOOL bGrabOnField0 = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GrabOnField0"),
							FALSE, true);
						pNotify->Start("Record", nFramesToWrite);
						DWORD dwBegin = 0;//timeGetTime();
						for (;;)
						{
							if (bGrabOnField0)
							{
								while (MV5GetFieldInfo() == 0);
								while (MV5GetFieldInfo() == 1);
							}
							else
							{
								while (MV5GetFieldInfo() == 1);
								while (MV5GetFieldInfo() == 0);
							}
							DWORD dwNow = timeGetTime();
							if (dwBegin == 0) dwBegin = dwNow;
							nFrame = (dwNow-dwBegin+dwMSecs/2)/dwMSecs;
							if (nFrame >= nFramesToWrite)
								break;
							if (pbFlags[nFrame])
								continue;
							pbFlags[nFrame] = TRUE;
							if (bGrayScale && gm == GM_YUV422)
							{
								if (bVerticalMirror)
									_YuvCCIR601_422ToY_Vert(m_lpData, lpDataEnd, dwRowSrc, dwRowDst, m_szGrab.cy);
								else
									MV5YuvCCIR601_422ToY(m_lpData, lpDataRaw, dwSize);
							}
							else
								memcpy(lpDataRaw, m_lpData, m_nBuffSize); // To avoid mixing data from different frames
							hr = AVIStreamWrite(ps, nFrame, 1, lpDataRaw, dwSize, 0/*AVIIF_KEYFRAME*/, NULL, NULL);
							if ((nFrame%nFPS)==0)
								pNotify->Notify(nFrame);
						}
						for (int i = 0; i < nFramesToWrite; i++)
							if (pbFlags[i])
								nFramesWritten++;
						delete pbFlags;
					}
					pNotify->End();
					StopGrab();
				}
				free(lpDataRaw);
				AVIStreamRelease(ps);
			}
			AVIFileRelease(pf);
		}
		else
		{
			DeinitGrab();
			AVIFileExit();
			CString s;
			s.Format(IDS_FILE_NOT_OPENED, sRec);
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
			m_bCanDraw = true;
			return false;
		}
		DeinitGrab();
	}
	if (m_BoardType!=Board_MV510&&!bGrayScale&&bUseYUV||bFields)
	{
		HRESULT hr;
		int r;
		if (m_BoardType!=Board_MV510&&!bGrayScale&&bUseYUV)
			r = AVIYuvToRGB888(sRec, sName, dwMSecsPB, m_szGrab, bVerticalMirror, hr, bFields, nScale, pNotify);
		else
			r = AVIHScale2AndResampleFields(sRec, sName, hr, m_szGrab, nScale, dwMSecsPB, pNotify);
		if (r)
		{
			bNotShowMessage = TRUE;
			CString sMsg;
			sMsg.Format(IDS_ERROR_DURING_DECOMPRESSION, (const char *)sRec, (const char *)sName, r, hr);
			AfxMessageBox(sMsg);
		}
		unlink(sRec);
	}
	AVIFileExit();
	if (!bNotShowMessage && (CStdProfileManager::m_pMgr->GetProfileInt("Settings", "ShowMessage", 0, true) ||
		CStdProfileManager::m_pMgr->GetProfileInt("Settings", "ShowMessageWhenSkipped", 1, true) &&
		nFramesToWrite-nFramesWritten != 0))
	{
		CString sMsg;
		if (bFields)
			sMsg.Format(IDS_AVI_PLACED, (const char *)sName, 2*nFramesWritten, 2*nFramesToWrite-2*nFramesWritten,
				((double)nFramesToWrite)*dwMSecs/1000);
		else
			sMsg.Format(IDS_AVI_PLACED, (const char *)sName, nFramesWritten, nFramesToWrite-nFramesWritten,
				((double)nFramesToWrite)*dwMSecs/1000);
		AfxMessageBox(sMsg);
	}
	m_bCanDraw = true;
	return true;
}

bool CMV500Grab::GrabAVI(IMV500ProgressNotify *pNotify)
{
	if (m_nCameras != 1) return false;
	bool b;
	int nWriteMethod = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("CaptureToMemory"), 0);
	if (nWriteMethod == 0 || nWriteMethod == 1)
		b = GrabAVIMemory(pNotify);
	else
		b = GrabAVIDisk(pNotify);
	return b;
}







