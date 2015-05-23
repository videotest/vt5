// DriverCCam.cpp: implementation of the CDriverCCam class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CCam.h"
#include "DriverCCam.h"
#include "PreviewDialog.h"
#include "Image5.h"
#include <vfw.h>
#include "debug.h"
#include "Vt5Profile.h"
#include "core5.h"
#include "misc_utils.h"
#include "ccapi.h"
#include "ccutil.h"
#include "SettingsDialog.h"
#include "Bayer2RGB.h"
#include <math.h>


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


class CDibDraw
{
public:
	HDRAWDIB	hdd;

	CDibDraw()
	{ hdd = ::DrawDibOpen();}
	~CDibDraw()
	{ DrawDibClose( hdd );}
};

CDibDraw	dd;

int ImageSize(int cx, int cy, int bpp)
{
	return (((cx*bpp+3)>>2)<<2)*cy;
}

static DWORD s_dwTickCountLast = 0;
static DWORD s_dwPeriod = 0;
static bool  s_bConfChanged = false;
static HANDLE g_camera = NULL;


#if defined(_CCAM_MT_)

LONG s_dwThreadRun = 0;
HANDLE s_hCycle = NULL;
HANDLE s_hFinished = NULL;
CRITICAL_SECTION g_CritSectionCamera;
CRITICAL_SECTION g_CritSectionSite;

class CCCamThread : public CWinThread
{
	CDriverCCam *m_pDrv;
public:
	CCCamThread(CDriverCCam *pDrv) : CWinThread()
	{
		m_pDrv = pDrv;
	}
	virtual BOOL InitInstance();
};

BOOL CCCamThread::InitInstance()
{
	timeBeginPeriod(1);
	DWORD dwTicksLast = timeGetTime();//GetTickCount();
	while (s_dwThreadRun)
	{
		EnterCriticalSection(&CDriverCCam::m_csSites);
		m_pDrv->MakeImage();
		for (int i = 0; i < CDriverCCam::m_arrSites.GetSize(); i++)
			CDriverCCam::m_arrSites[i]->Invalidate();
		DWORD dwTicks = timeGetTime();//GetTickCount();
		s_dwPeriod = dwTicks-dwTicksLast;
		dwTicksLast = dwTicks;
		LeaveCriticalSection(&CDriverCCam::m_csSites);
		Sleep(20);
	}
	::SetEvent(s_hFinished);

	return CWinThread::InitInstance();
}

#else

static int s_nAdditionalInit = 0;
static int s_nTimerId = 0;
static CDriverCCam *g_pDrv = NULL;
void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	g_pDrv->MakeImage();
	for (int i = 0; i < CDriverCCam::m_arrSites.GetSize(); i++)
		CDriverCCam::m_arrSites[i]->Invalidate();
	DWORD dwTicks = GetTickCount();
	s_dwPeriod = dwTicks-s_dwTickCountLast;
	s_dwTickCountLast = dwTicks;
}

#endif

const char *pszCamNameBCI4 = "BCi4 USB";
const char *pszCamNameBCI5 = "BCi5 USB";

static VALUEINFOEX s_aValuesIntBCI5[] =
{
	{{"Settings", "StartX"}, {ValueType_Int,0,1279,0,-1}},
	{{"Settings", "StartY"}, {ValueType_Int,0,1023,0,-1}},
	{{"Settings", "Width"}, {ValueType_Int,0,1280,1280,-1}},
	{{"Settings", "Height"}, {ValueType_Int,0,1024,1024,-1}},
	{{"Settings", "IntegrationTime"}, {ValueType_Int,1,1000000,50000,-1}},
	{{"Settings", "Gain"}, {ValueType_Int,0,15,3,-1}, 1, CC_PAR_GAIN},
	{{"Settings", "Offset"}, {ValueType_Int,0,127,3,-1}, 1, CC_PAR_OFFSET},
	{{"Settings", "OffsetBFine"}, {ValueType_Int,0,127,48,-1}, 1, CC_PAR_OFFSET_B_FINE},
	{{"Settings", "Anaval0"}, {ValueType_Int,0,255,220,-1}, 1, CC_PAR_ANAVAL0},
	{{"Settings", "Anaval1"}, {ValueType_Int,0,255,0,-1}},
//	{{"Settings", "Anaval2"}, {ValueType_Int,0,255,0,-1}, 1, CC_PAR_ANAVAL2},
	{{"Settings", "Anaval2"}, {ValueType_Int,0,255,0,-1}, -1},
	{{"Settings", "Anaval3"}, {ValueType_Int,0,255,0,-1}},
	{{"Settings", "TotalSlopes"}, {ValueType_Int,1,4,1,-1}},

	{{"Settings", "IntegrationTimeProc_1"}, {ValueType_Int,1,1000,100,-1}},
	{{"Settings", "IntegrationTimeProc_2"}, {ValueType_Int,1,1000,100,-1}},
	{{"Settings", "IntegrationTimeProc_3"}, {ValueType_Int,1,1000,100,-1}},
	{{"Settings", "Anaval1Extra_1"}, {ValueType_Int,0,255,70,-1}},
	{{"Settings", "Anaval1Extra_2"}, {ValueType_Int,0,255,40,-1}},
	{{"Settings", "Anaval1Extra_3"}, {ValueType_Int,0,255,30,-1}},
};

static VALUEINFOEX s_aValuesIntBCI4[] =
{
	{{"Settings", "StartX"}, {ValueType_Int,0,1279,0,-1}},
	{{"Settings", "StartY"}, {ValueType_Int,0,1023,0,-1}},
	{{"Settings", "Width"}, {ValueType_Int,0,1280,1280,-1}},
	{{"Settings", "Height"}, {ValueType_Int,0,1024,1024,-1}},
	{{"Settings", "IntegrationTime"}, {ValueType_Int,1,1000000,50000,-1}},
	{{"Settings", "Gain"}, {ValueType_Int,0,3,0,-1}},
	{{"Settings", "Offset"}, {ValueType_Int,16,255,16,-1}},
	{{"Settings", "PixelPrecharge"}, {ValueType_Int,0,255,0,-1}},
	{{"Settings", "Anaval3"}, {ValueType_Int,0,255,250,-1}},
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//IMPLEMENT_DYNCREATE(CDriverCCam, CDriverBase)

CArray<IDriverSite *,IDriverSite *&> CDriverCCam::m_arrSites;
CRITICAL_SECTION CDriverCCam::m_csSites;
bool CDriverCCam::m_bCSSitesInited = false;


CDriverCCam::CDriverCCam()
{
	// To keep the application running as long as an OLE automation 
	//	object is active, the constructor calls AfxOleLockApp.
	AfxOleLockApp();
	m_nGrabCount = 0;
	if (!m_bCSSitesInited)
	{
		m_bCSSitesInited = true;
		InitializeCriticalSection(&m_csSites);
	}
	m_bInitFailed = false;
	m_cx = 1280;
	m_cy = 1024;
	m_bpp = 8;
	m_bColor = m_bBCG = false;
	m_DrawData.lpbi = &m_bi.bmiHeader;
	m_DrawData.pData = NULL;
	m_nSaturation = 0;
	m_szActiveVideo = CSize(1280,1024);
	m_pt1 = CPoint(0,0);
	m_x1 = 0;
	m_y1 = 0;
	m_nBright = m_nContrast = 0;
	m_nBrightR = m_nContrastR = 0;
	m_nBrightG = m_nContrastG = 0;
	m_nBrightB = m_nContrastB = 0;
	m_dGamma = m_dGammaR = m_dGammaG = m_dGammaB = 0.;
	m_bNoWait = true;
	m_nPrevMode = -1;
	m_bHorzMirror = FALSE;
	m_bVertMirror = FALSE;
}

CDriverCCam::~CDriverCCam()
{
	ASSERT(m_nGrabCount == 0);
	if (m_nGrabCount > 0)
	{
		m_nGrabCount = 1;
		StartImageAquisition(0, FALSE);
	}
	if (g_camera) CC_Close(g_camera);
	g_camera = NULL;
	// To terminate the application when all objects created with
	// 	with OLE automation, the destructor calls AfxOleUnlockApp.
	AfxOleUnlockApp();
}

IUnknown *CDriverCCam::DoGetInterface( const IID &iid )
{
	if (iid == IID_IDriver)
		return (IDriver *)this;
	else if (iid == IID_IDriver2)
		return (IDriver2 *)this;
	else if (iid == IID_IDriver3)
		return (IDriver3 *)this;
	else if (iid == IID_IDriver4)
		return (IDriver4 *)this;
	else if (iid == IID_IDriver5)
		return (IDriver5 *)this;
	else if (iid == IID_IDriver6)
		return (IDriver6 *)this;
	else if (iid == IID_IInputPreview2)
		return (IInputPreview2 *)this;
	else if (iid == IID_IInputPreview3)
		return (IInputPreview3 *)this;
	else if (iid == IID_IInputPreview)
		return (IInputPreview *)this;
	else
		return ComObjectBase::DoGetInterface(iid);
}

bool CDriverCCam::Init()
{
	char pcilogicname[80] = "cclvds.ttb" ;	// must be binary file not ttf
//	char cclogicname[80] = "bcusb.ttb";	// For area scan
//	char cclogicname[80] = "bci4u20a.ttb";	// For area scan
	char cclogicname[80] = "bci5usb.ttb";	// For area scan
	if (m_bInitFailed) return false;
	if (g_camera != NULL) return true;
	CString sCameraName = CStdProfileManager::m_pMgr->GetProfileString(_T("Settings"), _T("CameraName"), pszCamNameBCI5, true);
	m_bNoWait = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("NoWaitMode"), 0, true, true)?true:false; 
	g_camera = CC_Open(sCameraName, 0, m_bNoWait?CC_CAPTURE_NOWAIT:CC_CAPTURE_WAIT);
	if (g_camera == NULL)
	{
		m_bInitFailed = true;
		return false;
	}
	CString sPciLogicName = CStdProfileManager::m_pMgr->GetProfileString(_T("Settings"), _T("pcilogicname"), NULL, true);
	CString sLogicName = CStdProfileManager::m_pMgr->GetProfileString(_T("Settings"), _T("cclogicname"), cclogicname, true);
/*	FILE *ccf = fopen(sLogicName, "rb");
	if (ccf == NULL)
	{
		AfxMessageBox(IDS_NOLOGIC, MB_OK|MB_ICONHAND);
		CC_Close(g_camera);
		m_bInitFailed = true;
		return false;
	}
	fclose(ccf);*/
	if (!sPciLogicName.IsEmpty())
	{
		DWORD dwCameraNumber;
		if (CC_LoadInterface(g_camera, sPciLogicName.GetBuffer(0)) == FALSE)
		{
			AfxMessageBox(IDS_NOPCILOGIC, MB_OK|MB_ICONHAND);
			CC_Close(g_camera);
			m_bInitFailed = true;
			return false;
		}
		CC_GetCameraNumber(g_camera , &dwCameraNumber);
	}
	if (CC_LoadCamera(g_camera, sLogicName.GetBuffer(0)) == FALSE)
	{
		AfxMessageBox(IDS_LOGICNOLOAD, MB_OK|MB_ICONHAND);
		CC_Close(g_camera);
		g_camera = NULL;
		m_bInitFailed = true;
		return false;
	}
	if (sCameraName == "BCi5 USB")
	{
		m_pValueInfos = s_aValuesIntBCI5;
		m_nValueInfosCount = sizeof(s_aValuesIntBCI5)/sizeof(s_aValuesIntBCI5[0]);
	}
	else
	{
		m_pValueInfos = s_aValuesIntBCI4;
		m_nValueInfosCount = sizeof(s_aValuesIntBCI4)/sizeof(s_aValuesIntBCI4[0]);
	}
//	InitSettings();
	m_bHorzMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Mirror"), _T("Horizontal"), FALSE, true, true);
	m_bVertMirror = CStdProfileManager::m_pMgr->GetProfileInt(_T("Mirror"), _T("Vertical"), FALSE, true, true);
	return true;
}

static void __check_pos(int &xy, int &cxy, int nMax)
{
	if (xy > nMax-100) xy = nMax-100;
	if (cxy < 100) cxy = 100;
	if (xy+cxy > nMax) cxy = nMax-xy;
	xy = (xy>>1)<<1;
	cxy = (cxy>>2)<<2;
}

static SIZE m_GrabSizes[] =
{
	{1280, 1024},
	{1024, 768},
	{640, 480}
};

const int nGrabSizes = sizeof(m_GrabSizes)/sizeof(m_GrabSizes[0]);

BOOL IsInputDifferent() {return CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("CaptureFullFrame"), FALSE);}

void CDriverCCam::InitSettings(int nMode)
{
	if (m_nPrevMode == nMode || m_nPrevMode >= 0 && m_nPrevMode <= 1 &&
		nMode >= 0 && nMode <= 1 && !IsInputDifferent()) return;
	if (nMode == -1) nMode = m_nPrevMode;
	unsigned long TransferSize;
	CString sSec = nMode<=1?_T("Settings"):_T("Video");
	BOOL bCaptureFullFrame = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("CaptureFullFrame"), FALSE);
	int nGrabSize = nMode==1&&bCaptureFullFrame?0:CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("GrabWindowSize"), 0);
	CString sEntryStartX,sEntryStartY;
	sEntryStartX.Format(_T("StartX%d"), nGrabSize);
	sEntryStartY.Format(_T("StartY%d"), nGrabSize);
	if (nGrabSize >= 0  && nGrabSize < nGrabSizes)
	{
		m_cx = m_GrabSizes[nGrabSize].cx;
		m_cy = m_GrabSizes[nGrabSize].cy;
		m_x = CStdProfileManager::m_pMgr->GetProfileInt(sSec, sEntryStartX, (1280-m_cx)/2, true);
		m_y = CStdProfileManager::m_pMgr->GetProfileInt(sSec, sEntryStartY, (1024-m_cy)/2, true);
		__check_pos(m_x,m_cx,1280);
		__check_pos(m_y,m_cy,1024);
	}
	else
	{
		m_x = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("StartX"), 0, true);
		m_y = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("StartY"), 0, true);
		m_cx = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("Width"), 1280, true);
		__check_pos(m_x,m_cx,1280);
		m_cy = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("Height"), 1024, true);
		__check_pos(m_y,m_cy,1024);
	}
	CC_SetWOI(g_camera,  m_x,  m_y, m_x+m_cx-1, m_y+m_cy-1, 1 ,1 , CC_WOI_LEFTTOP_RIGHTBOTTOM, &TransferSize  ) ;
	for (int i = 0; i < m_nValueInfosCount; i++)
	{
		m_pValueInfos[i].Info.nCurrent = CStdProfileManager::m_pMgr->GetProfileInt(m_pValueInfos[i].Hdr.pszSection,
			m_pValueInfos[i].Hdr.pszEntry, m_pValueInfos[i].Info.nDefault, true);
		if (m_pValueInfos[i].dwCamDataType == 1)
			CC_SetParameter(g_camera, (CC_PARAMETER)m_pValueInfos[i].dwCamData, m_pValueInfos[i].Info.nCurrent);
	}
	ReadSlopesValues();
	InitSlopes();
	InitDatamode();
	m_nBrightR = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RedLuminance"), 0, NULL, true);
	m_nBrightG = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GreenLuminance"), 0, NULL, true);
	m_nBrightB = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("BlueLuminance"), 0, NULL, true);
	m_nContrastR = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RedContrast"), 0, NULL, true);
	m_nContrastG = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GreenContrast"), 0, NULL, true);
	m_nContrastB = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("BlueContrast"), 0, NULL, true);
	m_dGammaR = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RedGamma"), 100, NULL, true)/100.-1.;
	m_dGammaG = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GreenGamma"), 100, NULL, true)/100.-1.;
	m_dGammaB = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("BlueGamma"), 100, NULL, true)/100.-1.;
	m_nSaturation = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Saturation"), 0, NULL, true);
	m_bBCG = false;
	m_bColor = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Color"), false, true) &&
		CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	ReadCnvBuffers(m_Curve8, m_Curve10, m_Curve12);
	InitBuffersAndBI();
	m_nPrevMode = nMode;
}

void CDriverCCam::DoReadSlopesValues(int n1, int n2)
{
	VALUEINFOEX vie;
	char szBuff[256];
	for (int i = n1; i < n2; i++)
	{
		sprintf(szBuff, "IntegrationTimeProc_%d", i);
		CCamValues::GetValueInfoByName("Settings", szBuff, &vie);
		m_nIntTimes[i] = m_nIntTimes[i-1]*vie.Info.nCurrent/1000;
		sprintf(szBuff, "Anaval1Extra_%d", i);
		CCamValues::GetValueInfoByName("Settings", szBuff, &vie);
		m_nResetVolt[i] = m_nResetVolt[i-1]+vie.Info.nCurrent;
	}
}

void CDriverCCam::ReadSlopesValues()
{
	VALUEINFOEX vie;
	CCamValues::GetValueInfoByName("Settings", "IntegrationTime", &vie);
	m_nIntTimes[0] = vie.Info.nCurrent;
	if (SUCCEEDED(CCamValues::GetValueInfoByName("Settings", "TotalSlopes", &vie)))
	{
		m_nSlopes = vie.Info.nCurrent;
		CCamValues::GetValueInfoByName("Settings", "Anaval3", &vie);
		m_nResetVolt[0] = vie.Info.nCurrent;
		DoReadSlopesValues(1, m_nSlopes);
	}
	else
		m_nSlopes = 1;
}

void CDriverCCam::InitSlopes()
{
	if (m_nSlopes <= 1)
	{
		CC_SetParameter(g_camera, CC_PAR_INTSEL, 0);
		CC_SetParameter(g_camera, CC_PAR_CTRLBIT, 256);
		CC_SetParameter(g_camera, CC_PAR_INTEGRATION_TIME, m_nIntTimes[0]);
		CC_SetParameter(g_camera, CC_PAR_ANAVAL3, m_nResetVolt[0]);
	}
	else
	{
		CC_SetParameter(g_camera, CC_PAR_CTRLBIT, 257);
		CC_SetParameter(g_camera, CC_PAR_INTSEL, 0);
		CC_SetParameter(g_camera, CC_PAR_INTEGRATION_TIME, m_nIntTimes[0]);
		CC_SetParameter(g_camera, CC_PAR_ANAVAL3, m_nResetVolt[0]);
		for (int i = 1; i < m_nSlopes; i++)
		{
			CC_SetParameter(g_camera, CC_PAR_INTSEL, i);
			CC_SetParameter(g_camera, CC_PAR_INTEGRATION_TIME, m_nIntTimes[i]);
			CC_SetParameter(g_camera, CC_PAR_ANAVAL1, m_nResetVolt[i]);
		}
	}
}


void CDriverCCam::InitDatamode()
{
	int datamode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Datamode"), CC_DATA_8BIT_11_DOWNTO_4, true);
	m_bpp = BitnessByDatamode(datamode);
	CC_SetParameter(g_camera, CC_PAR_DATA_MODE, datamode);
}

void CDriverCCam::InitBuffersAndBI()
{
	m_DrawData.pData = NULL;
	if (m_bpp == 8)
	{
		if (m_bColor||m_bBCG)
		{
			m_BuffLow.alloc(ImageSize(m_cx,m_cy,1));
			m_BuffCap.alloc(ImageSize(m_cx,m_cy,3));
		}
		else
			m_BuffCap.alloc(ImageSize(m_cx,m_cy,1));
	}
	else
	{
		if (m_bColor||m_bBCG)
		{
			m_BuffLow.alloc(ImageSize(m_cx,m_cy,sizeof(WORD)));
			m_BuffCap.alloc(ImageSize(m_cx,m_cy,3*sizeof(WORD)));
			m_BuffDraw.alloc(ImageSize(m_cx,m_cy,3));
		}
		else
		{
			m_BuffCap.alloc(ImageSize(m_cx,m_cy,sizeof(WORD)));
			m_BuffDraw.alloc(ImageSize(m_cx,m_cy,1));
		}
	}
	memset(&m_bi, 0, sizeof(m_bi));
	m_bi.bmiHeader.biSize = sizeof(m_bi.bmiHeader);
	m_bi.bmiHeader.biBitCount = m_bColor?24:8;
	m_bi.bmiHeader.biPlanes = 1;
	m_bi.bmiHeader.biWidth = m_cx;
	m_bi.bmiHeader.biHeight = m_cy;
	m_bi.bmiHeader.biClrImportant = m_bi.bmiHeader.biClrUsed = m_bColor?0:256;
	for (unsigned i = 0; i < m_bi.bmiHeader.biClrUsed; i++)
		m_bi.bmiColors[i].rgbRed = m_bi.bmiColors[i].rgbGreen = m_bi.bmiColors[i].rgbBlue = i;
	InitCnvBuff();
}

void CDriverCCam::InitCnvBuff()
{
	if (m_bColor)
	{
		if (m_bpp == 8)
		{
			MakeCnvBuffer<BYTE>(m_Curve8.nPoints, m_Curve8.Red, m_Curve8.Bright, m_CnvR,
				m_nBrightR, m_nContrastR, m_dGammaR, 255);
			MakeCnvBuffer<BYTE>(m_Curve8.nPoints, m_Curve8.Green, m_Curve8.Bright, m_CnvG,
				m_nBrightG, m_nContrastG, m_dGammaG, 255);
			MakeCnvBuffer<BYTE>(m_Curve8.nPoints, m_Curve8.Blue, m_Curve8.Bright, m_CnvB,
				m_nBrightB, m_nContrastB, m_dGammaB, 255);
		}
		else if (m_bpp == 10)
		{
			MakeCnvBuffer<WORD>(m_Curve10.nPoints, m_Curve10.Red, m_Curve10.Bright, m_CnvR,
				m_nBrightR, m_nContrastR, m_dGammaR, 1023);
			MakeCnvBuffer<WORD>(m_Curve10.nPoints, m_Curve10.Green, m_Curve10.Bright, m_CnvG,
				m_nBrightG, m_nContrastG, m_dGammaG, 1023);
			MakeCnvBuffer<WORD>(m_Curve10.nPoints, m_Curve10.Blue, m_Curve10.Bright, m_CnvB,
				m_nBrightB, m_nContrastB, m_dGammaB, 1023);
		}
		else
		{
			MakeCnvBuffer<WORD>(m_Curve12.nPoints, m_Curve12.Red, m_Curve12.Bright, m_CnvR,
				m_nBrightR, m_nContrastR, m_dGammaR, 4095);
			MakeCnvBuffer<WORD>(m_Curve12.nPoints, m_Curve12.Green, m_Curve12.Bright, m_CnvG,
				m_nBrightG, m_nContrastG, m_dGammaG, 4095);
			MakeCnvBuffer<WORD>(m_Curve12.nPoints, m_Curve12.Blue, m_Curve12.Bright, m_CnvB,
				m_nBrightB, m_nContrastB, m_dGammaB, 4095);
		}
	}
}

static void _Conv8To16(WORD *pDst, byte *pSrc, int nSamples)
{
	for (int i = 0; i < nSamples; i++)
		pDst[i] = ((WORD)pSrc[i])<<8;
}

static void _Conv16To8(byte *pDst, WORD *pSrc, int nSamples)
{
	for (int i = 0; i < nSamples; i++)
		pDst[i] = (byte)(pSrc[i]>>8);
}

static void _ShiftBits(WORD *p, int nSize, int nShift)
{
	for (int i = 0; i < nSize; i++)
		p[i] = p[i]<<nShift;
}

static void _EatBits(WORD *p, int nSize, WORD wMask)
{
	for (int i = 0; i < nSize; i++)
		p[i] = p[i]&wMask;
}

void CDriverCCam::MakeImage()
{
	if (m_bpp == 8)
	{
		if (m_bColor)
		{
			CC_CaptureSingle(g_camera, (byte*)m_BuffLow, m_cx*m_cy, CC_NO_TRIGGER, 5/*seconds*/, NULL);
			if (m_bNoWait) CC_CaptureWait(g_camera);
			_DoColorize<BYTE>(m_BuffLow, m_BuffCap, m_cx, m_cy, m_nSaturation,
				m_CnvR, m_CnvG, m_CnvB, ((m_cx+3)>>2)<<2, ((3*m_cx+3)>>2)<<2, 0xFF);
		}
		else
		{
			CC_CaptureSingle(g_camera, (byte*)m_BuffCap, m_cx*m_cy, CC_NO_TRIGGER, 5/*seconds*/, NULL);
			if (m_bNoWait) CC_CaptureWait(g_camera);
			MirrorImageGS<BYTE>((byte*)m_BuffCap, ((m_cx+3)>>2)<<2, m_cx, m_cy, m_bVertMirror,
				m_bHorzMirror);
		}
		m_DrawData.pData = m_BuffCap;
	}
	else
	{
		if (m_bColor||m_bBCG)
		{
			CC_CaptureSingle(g_camera, (byte*)m_BuffLow, m_cx*m_cy*sizeof(WORD), CC_NO_TRIGGER, 5/*seconds*/, NULL);
			if (m_bNoWait) CC_CaptureWait(g_camera);
			_DoColorize<WORD>((WORD*)(byte*)m_BuffLow, (WORD*)(byte*)m_BuffCap, m_cx, m_cy, m_nSaturation,
				(WORD*)(byte*)m_CnvR, (WORD*)(byte*)m_CnvG, (WORD*)(byte*)m_CnvB, ((2*m_cx+3)>>2)<<2,
				((6*m_cx+3)>>2)<<2, m_bpp==10?0x3FF:0xFFF);
			_ShiftBits((WORD*)(byte*)m_BuffCap, m_bColor?m_cx*m_cy*3:m_cx*m_cy, m_bpp==12?4:6);
		}
		else
		{
			CC_CaptureSingle(g_camera, (byte*)m_BuffCap, m_cx*m_cy*sizeof(WORD), CC_NO_TRIGGER, 5/*seconds*/, NULL);
			if (m_bNoWait) CC_CaptureWait(g_camera);
			_ShiftBits((WORD*)(byte*)m_BuffCap, m_cx*m_cy, m_bpp==12?4:6);
			MirrorImageGS<WORD>((WORD*)(byte*)m_BuffCap, ((m_cx+3)>>2)<<2, m_cx, m_cy, m_bVertMirror,
				m_bHorzMirror);
		}
		_Conv16To8(m_BuffDraw, (WORD*)(byte*)m_BuffCap, m_bColor?m_cx*m_cy*3:m_cx*m_cy);
		m_DrawData.pData = m_BuffDraw;
	}
}

void CDriverCCam::MakeVideo(int nFrames, int nFrameSize, DWORD dwFrameMs, IVideoHook *pHook)
{
	m_BuffVideo.alloc(nFrames*nFrameSize);
	m_FrmOk.alloc(nFrames);
	m_FrmOk.zero();
	byte *p = m_BuffVideo;
	DWORD dwStart = GetTickCount();
	DWORD dwRedraw = max(300,dwFrameMs);
	DWORD dwNext = dwStart+dwRedraw;
	for (int iFrame = 0; iFrame < nFrames; iFrame++)
	{
		DWORD dwFrameTime = dwStart+dwFrameMs*iFrame;
		DWORD dwNow = GetTickCount();
		if (dwNow > dwNext)
		{
			if (pHook) pHook->OnFrame(iFrame);
			dwNext = dwNow+dwRedraw;
		}
		if (dwNow < dwFrameTime)
			Sleep(dwFrameTime-dwNow);
		else if (dwNow > dwFrameTime+dwFrameMs)
			continue;
		CC_CaptureSingle(g_camera, p+nFrameSize*iFrame, nFrameSize, CC_NO_TRIGGER, 0, NULL);
		m_FrmOk[iFrame] = 1;
	}
}


HRESULT CDriverCCam::BeginPreview(int nDev, IInputPreviewSite *pSite)
{
	HRESULT hr = StartImageAquisition(0,TRUE);
	if (FAILED(hr)) return hr;
	return AddDriverSite(0,pSite);
}

HRESULT CDriverCCam::EndPreview(int nDev, IInputPreviewSite *pSite)
{
	RemoveDriverSite(0,pSite);
	return StartImageAquisition(0,FALSE);
}

HRESULT CDriverCCam::GetSize(int nDev, short *pdx, short *pdy)
{
	*pdx = m_cx;
	*pdy = m_cy;
	return S_OK;
}

HRESULT CDriverCCam::GetPreviewFreq(int nDev, DWORD *pdwFreq)
{
	*pdwFreq = 15;
	return S_OK;
}


HRESULT CDriverCCam::DrawRect(int nDev, HDC hDC, LPRECT prcSrc, LPRECT prcDst)
{
	bool bDraw = false;
	if (g_camera && m_DrawData.pData)
	{
		__EnterCriticalSection(&m_csSites);
		if (prcSrc->bottom-prcSrc->top == prcDst->bottom-prcDst->top &&
			prcSrc->right-prcSrc->left == prcDst->right-prcDst->left)
		{
			int nWidth = prcDst->right-prcDst->left;
			int nHeight = prcDst->bottom-prcDst->top;
			SetDIBitsToDevice(hDC, prcDst->left, prcDst->top, nWidth, nHeight,
				prcSrc->left,m_bi.bmiHeader.biHeight-prcSrc->top-nHeight-1,0,
				m_bi.bmiHeader.biHeight,m_DrawData.pData,(LPBITMAPINFO)&m_bi,
				DIB_RGB_COLORS);
		}
		else
			DrawDibDraw(dd.hdd, hDC, prcDst->left, prcDst->top, prcDst->right-prcDst->left, prcDst->bottom-prcDst->top,
				&m_bi.bmiHeader, m_DrawData.pData, prcSrc->left,prcSrc->top,prcSrc->right-prcSrc->left,prcSrc->bottom-prcSrc->top, 0);
		__LeaveCriticalSection(&m_csSites);
		bDraw = true;
	}
	if (!bDraw)
	{
		HBRUSH br = ::CreateSolidBrush(RGB(0,0,0));
		FillRect(hDC, prcDst, br);
		DeleteObject(br);
	}
	return S_OK;
}

HRESULT CDriverCCam::GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod)
{
	*pdwPeriod = s_dwPeriod;
	return S_OK;
}

HRESULT CDriverCCam::GetPreviewMode(int nDevice, int *pnMode)
{
	*pnMode = 0;
	return S_OK;
}

HRESULT CDriverCCam::SetPreviewMode(int nDevice, int nMode)
{
	return S_OK;
}


HRESULT CDriverCCam::SetValue(int nCurDev, BSTR bstrName, VARIANT Value)
{
	CString s(bstrName);
	((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->WriteProfileValue(NULL, s, Value);

	return S_OK;
}

HRESULT CDriverCCam::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString s(bstrName);
	*pValue = ((CVT5ProfileManager*)CStdProfileManager::m_pMgr)->GetProfileValue(NULL, s);
	return S_OK;
}

HRESULT CDriverCCam::InputNativeOrDIB(IImage3 *pImg, IDIBProvider *pDIBPrv)
{
	return S_OK;
}


HRESULT CDriverCCam::InputNative(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bUseSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	IImage3Ptr sptrImg(pUnkImg);
	if (!Init())
		return E_UNEXPECTED;
	int nMode = m_nPrevMode;
	__EnterCriticalSection(&m_csSites);
	InitSettings(1);
	if (m_nGrabCount == 0 || nMode != 1 && IsInputDifferent())
	{
		DWORD dwSleep = CStdProfileManager::m_pMgr->GetProfileInt(_T("Timeouts"), _T("SleepTimeAfterSettingsApplied"), 400, true, true);
		Sleep(dwSleep);
		MakeImage();
	}
	sptrImg->CreateImage(m_cx, m_cy, m_bColor?_bstr_t("RGB"):_bstr_t("Gray"), -1);
	byte *pData = m_BuffCap;
	DWORD dwRow = m_bpp==8?(m_bColor?((3*m_cx+3)>>2)<<2:((m_cx+3)>>2)<<2):(m_bColor?((6*m_cx+3)>>2)<<2:((2*m_cx+3)>>2)<<2);
	for (int y = 0; y < m_cy; y++)
	{
		if (m_bColor)
		{
			color *pRowR,*pRowG,*pRowB;
			sptrImg->GetRow(y, 0, &pRowR);
			sptrImg->GetRow(y, 1, &pRowG);
			sptrImg->GetRow(y, 2, &pRowB);
			if (m_bpp==8)
			{
				byte *pSrcData = pData+dwRow*(m_cy-y-1);
				for (int x = 0; x < m_cx; x++)
				{
					pRowB[x] = ((color)pSrcData[3*x])<<8;
					pRowG[x] = ((color)pSrcData[3*x+1])<<8;
					pRowR[x] = ((color)pSrcData[3*x+2])<<8;
				}
			}
			else
			{
				WORD *pSrcData = (WORD*)(pData+dwRow*(m_cy-y-1));
				for (int x = 0; x < m_cx; x++)
				{
					pRowB[x] = pSrcData[3*x];
					pRowG[x] = pSrcData[3*x+1];
					pRowR[x] = pSrcData[3*x+2];
				}
			}
		}
		else
		{
			color *pRow;
			sptrImg->GetRow(y, 0, &pRow);
			if (m_bpp==8)
			{
				byte *pSrcData = pData+dwRow*(m_cy-y-1);
				for (int x = 0; x < m_cx; x++)
					pRow[x] = ((color)pSrcData[x])<<8;
			}
			else
				memcpy(pRow, pData+dwRow*(m_cy-y-1), dwRow);
		}
	}
	InitSettings(nMode);
	if (nMode == -1)
		m_nPrevMode = -1;
	__LeaveCriticalSection(&m_csSites);
	return S_OK;
}

HRESULT CDriverCCam::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!Init())
		return E_UNEXPECTED;
	int nMode = m_nPrevMode;
	__EnterCriticalSection(&m_csSites);
	InitSettings(1);
	if (m_nGrabCount == 0 || nMode != 1)
		MakeImage();
	DWORD dwSizeH = sizeof(BITMAPINFOHEADER)+m_bi.bmiHeader.biClrUsed*sizeof(RGBQUAD);
	DWORD dwSizeD = ImageSize(m_cx, m_cy, m_bColor?3:1);
	DWORD dwSize = dwSizeH+dwSizeD;
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(dwSize);
	memcpy(lpbi, &m_bi, dwSizeH);
	LPBYTE lpData = ((LPBYTE)lpbi)+dwSizeH;
	memcpy(lpData, m_DrawData.pData, dwSizeD);
	InitSettings(nMode);
	if (nMode == -1)
		m_nPrevMode = -1;
	__LeaveCriticalSection(&m_csSites);
	return S_OK;
}

HRESULT CDriverCCam::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (!Init())
		return E_FAIL;
	InitSettings(2);
	CPreviewDialog dlg((IInputPreview *)this,nDevice,CPreviewDialog::ForAvi);
	return dlg.DoModal() == IDOK ? S_OK : S_FALSE;
}

HRESULT CDriverCCam::InputVideoFile2(int nDevice, int nMode, IVideoHook *pVideoHook)
{
	int nFPSType = CStdProfileManager::m_pMgr->GetProfileInt(_T("Video"), _T("FPSType"), 0, true);
	int nFPS = CStdProfileManager::m_pMgr->GetProfileInt(_T("Video"), _T("FPS"), 15, true);
	CString sFile = CStdProfileManager::m_pMgr->GetProfileString(_T("Video"), _T("PathName"), _T("c:\\capture.avi"), true);
	int nTime = CStdProfileManager::m_pMgr->GetProfileInt(_T("Video"), _T("Time"), 1, true);
	int nFrames = nFPS*nTime;
	int nFrameSize = m_cx*m_cy;
	DWORD dwFrameMS;
	if (nFPSType == 1)
	{
		dwFrameMS = 60000/nFPS;
		nTime = nTime*60000;
	}
	else if (nFPSType == 2)
	{
		dwFrameMS = 3600000/nFPS;
		nTime = nTime*3600000;
	}
	else
	{
		dwFrameMS = 1000/nFPS;
		nTime = nTime*1000;
	}
	if (pVideoHook) pVideoHook->OnStart(2);
	if (pVideoHook) pVideoHook->OnBeginStage(0, "Capturing", nFrames);
	__EnterCriticalSection(&m_csSites);
	MakeVideo(nFrames,nFrameSize,dwFrameMS,pVideoHook);
	__LeaveCriticalSection(&m_csSites);
	if (pVideoHook) pVideoHook->OnEndStage(0);
	if (pVideoHook) pVideoHook->OnBeginStage(1, "Saving", nFrames);
	PAVIFILE pf; 
	AVIFileInit();
	HRESULT hr = AVIFileOpen(&pf, sFile, OF_WRITE|OF_CREATE, NULL); 
	if (SUCCEEDED(hr))
	{
		DWORD dwMSecsPB = CStdProfileManager::m_pMgr->GetProfileInt(_T("AVI"), _T("PlaybackFPS"), 25);
		if (dwMSecsPB == 0) dwMSecsPB = 1;
		dwMSecsPB = 1000/dwMSecsPB;
	    AVISTREAMINFO strhdr;
		PAVISTREAM ps;
		memset(&strhdr, 0, sizeof(strhdr));
		strhdr.fccType = streamtypeVIDEO;
		strhdr.dwScale = dwMSecsPB*1000;
		strhdr.dwRate = 1000000;
		strhdr.dwQuality = 10000;
		SetRect(&strhdr.rcFrame, 0, 0, (int)m_cx, (int)m_cy); 
		hr = AVIFileCreateStream(pf, &ps, &strhdr);
		int iFrmOk = 0;
		if (SUCCEEDED(hr))
		{
			BITMAPINFO256 bi;
			memset(&bi.bmiHeader, 0, sizeof(bi));
			bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
			bi.bmiHeader.biPlanes = 1;
			bi.bmiHeader.biCompression = BI_RGB;
			bi.bmiHeader.biBitCount = 8;
			bi.bmiHeader.biWidth = m_cx;
			bi.bmiHeader.biHeight = m_cy;
			bi.bmiHeader.biClrUsed = 256;
			bi.bmiHeader.biClrImportant = 256;
			for (int i = 0; i < 256; i++)
			{
				bi.bmiColors[i].rgbBlue = bi.bmiColors[i].rgbGreen = bi.bmiColors[i].rgbRed = i;
				bi.bmiColors[i].rgbReserved = 0;
			}
			hr = AVIStreamSetFormat(ps, 0, &bi, sizeof(bi)); 
			DWORD dwSize = (((bi.bmiHeader.biWidth+3)>>2)<<2)*bi.bmiHeader.biHeight;
			DWORD dwLast = GetTickCount();
			for (int i = 0; i < nFrames; i++)
			{
				if (m_FrmOk[i])
				{
					hr = AVIStreamWrite(ps, i, 1, &m_BuffVideo[i*nFrameSize], nFrameSize, AVIIF_KEYFRAME, NULL, NULL);
					iFrmOk++;
					DWORD dw = GetTickCount();
					if (dw >= dwLast+300)
					{
						if (pVideoHook) pVideoHook->OnFrame(i);
						dwLast = dw;
					}
				}
			}
			AVIStreamRelease(ps);
		}
		AVIFileRelease(pf);
		int nMessage = CStdProfileManager::m_pMgr->GetProfileInt(_T("Video"), _T("ShowMessage"), 1, true, true);
		if (nMessage == 2 || nMessage == 1 && iFrmOk < nFrames)
		{
			CString s;
			s.Format(IDS_VIDEO_CAPTURED, (const char *)sFile, iFrmOk, nFrames-iFrmOk);
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		}
	}
	else
	{
		AVIFileExit();
		CString s;
		s.Format(IDS_FILE_NOT_OPENED, sFile);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		return E_FAIL;
	}
	AVIFileExit();
	pVideoHook->OnEndStage(1);
	return S_OK;
};


HRESULT CDriverCCam::ExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nMode == 0)
		return S_OK;
	else
	{
		CPreviewDialog dlg((IInputPreview *)this,nDevice,0);
		return dlg.DoModal() == IDOK ? S_OK : S_FALSE;
	}
}

HRESULT CDriverCCam::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_ACCUMULATION|FG_INPUTFRAME|FG_SUPPORTSVIDEO;
	return S_OK;
}

HRESULT CDriverCCam::GetShortName(BSTR *pbstrShortName)
{
	CString strDriverName("C-Cam");
	*pbstrShortName = strDriverName.AllocSysString();
	return S_OK;
}

HRESULT CDriverCCam::GetLongName(BSTR *pbstrLongName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strDriverFullName;
	strDriverFullName.LoadString(IDS_CCAM);
	*pbstrLongName = strDriverFullName.AllocSysString();
	return S_OK;
}


HRESULT CDriverCCam::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CSettingsDialog dlg((IInputPreview *)this,nDev);
	return dlg.DoModal() == IDOK ? S_OK : S_FALSE;
}

HRESULT CDriverCCam::GetDevicesCount(int *pnCount)
{
	*pnCount = 1;
	return S_OK;
}

HRESULT CDriverCCam::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nDev != 0) return E_INVALIDARG;
	CString strDriverName("C-Cam");
	CString strDriverFullName,strCategory;
	strDriverFullName.LoadString(IDS_CCAM);
	strCategory.LoadString(IDS_INPUT);
	*pbstrShortName = strDriverName.AllocSysString();
	*pbstrLongName = strDriverFullName.AllocSysString();
	*pbstrCategory = strCategory.AllocSysString();
	return S_OK;
}



HRESULT CDriverCCam::GetImage(int nCurDev, LPVOID *lplpData, DWORD *pdwSize)
{
	*lplpData = &m_DrawData;
	*pdwSize = 0;
	return S_OK;
}

HRESULT CDriverCCam::ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwSize, IUnknown *punkImg)
{
	return S_OK;
}

HRESULT CDriverCCam::GetTriggerMode(int nCurDev, BOOL *pgTriggerMode)
{
	return S_OK;
}

HRESULT CDriverCCam::SetTriggerMode(int nCurDev, BOOL bSet)
{
	return S_OK;
}

HRESULT CDriverCCam::DoSoftwareTrigger(int nCurDev)
{
	return S_OK;
}

HRESULT CDriverCCam::StartImageAquisition(int nCurDev, BOOL bStart)
{
	if (bStart)
	{
		m_nGrabCount++;
		if (m_nGrabCount==1)
		{
			if (!Init())
				return E_UNEXPECTED;
			if (m_nPrevMode == -1) InitSettings(0);
#if defined(_CCAM_MT_)
			InterlockedExchange(&s_dwThreadRun, 1);
			s_hFinished = ::CreateEvent(NULL, TRUE, FALSE, NULL);
			s_hCycle = ::CreateEvent(NULL, TRUE, FALSE, NULL);
			CCCamThread *pThread = new CCCamThread(this);
			pThread->m_bAutoDelete = TRUE;
			pThread->CreateThread();
#else
			g_pDrv = this;
			s_nTimerId = SetTimer(NULL, 1, 50, TimerProc);
#endif
		}
	}
	else
	{
		if (--m_nGrabCount == 0)
		{
#if defined(_CCAM_MT_)
			InterlockedExchange(&s_dwThreadRun, 0);
			::WaitForSingleObject(s_hFinished, INFINITE);
			::CloseHandle(s_hCycle);
			::CloseHandle(s_hFinished);
			s_hCycle = NULL;
			s_hFinished = NULL;
#else
			KillTimer(NULL, s_nTimerId);
#endif
			m_nPrevMode = -1;
		}
	}
	return S_OK;
}

HRESULT CDriverCCam::IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition)
{
	*pbAquisition = m_nGrabCount > 0;
	return S_OK;
}

HRESULT CDriverCCam::AddDriverSite(int nCurDev, IDriverSite *pSite)
{
	__EnterCriticalSection(&m_csSites);
	bool bFound = false;
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
	{
		pSite->AddRef();
		m_arrSites.Add(pSite);
	}
	__LeaveCriticalSection(&m_csSites);
	return S_OK;
}

HRESULT CDriverCCam::RemoveDriverSite(int nCurDev, IDriverSite *pSite)
{
	__EnterCriticalSection(&m_csSites);
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			m_arrSites.RemoveAt(i);
			pSite->Release();
		}
	}
	__LeaveCriticalSection(&m_csSites);
	return S_OK;
}

HRESULT CDriverCCam::SetConfiguration(BSTR bstrName)
{
	CString s1(CStdProfileManager::m_pMgr->GetMethodic());
	HRESULT hr = CDriver::SetConfiguration(bstrName);
	CString s2(CStdProfileManager::m_pMgr->GetMethodic());
	if (SUCCEEDED(hr) && s1 != s2)
	{
		if (m_nGrabCount) __EnterCriticalSection(&m_csSites);
		InitSettings(-1);
		if (m_nGrabCount) __LeaveCriticalSection(&m_csSites);
	}

/*	if (SUCCEEDED(hr))
		if(CAM_IsDriverLoaded() && CAM_IsConnected())
		{
			::RestoreSettings();
			s_bConfChanged = true;
		}*/
	return hr;
}


HRESULT CDriverCCam::MouseMove(UINT nFlags, POINT pt)
{
	if (nFlags&MK_LBUTTON)
	{
		int dx = pt.x-m_pt1.x;
		int dy = pt.y-m_pt1.y;
		int x = min(max(m_x1-dx,0),m_szActiveVideo.cx-m_cx)/2*2;
		int y = min(max(m_y1+dy, 0),m_szActiveVideo.cy-m_cy)/2*2;
		if (x != m_x || y != m_y)
		{
			m_x = x;
			m_y = y;
			unsigned long TransferSize;
			__EnterCriticalSection(&m_csSites);
			CC_SetWOI(g_camera,  m_x,  m_y, m_x+m_cx-1, m_y+m_cy-1, 1 ,1 , CC_WOI_LEFTTOP_RIGHTBOTTOM, &TransferSize);
			__LeaveCriticalSection(&m_csSites);
		}
		return S_OK;
	}
	return S_FALSE;
}

HRESULT CDriverCCam::LButtonDown(UINT nFlags, POINT pt)
{
	m_pt1 = pt;
	m_x1 = m_x;
	m_y1 = m_y;
	return S_FALSE;
}

HRESULT CDriverCCam::LButtonUp(UINT nFlags, POINT pt)
{
	if (m_x != m_x1 || m_y != m_y1)
	{
		int nGrabSize = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("GrabWindowSize"), 0);
		CString sEntryStartX,sEntryStartY;
		sEntryStartX.Format(_T("StartX%d"), nGrabSize);
		sEntryStartY.Format(_T("StartY%d"), nGrabSize);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), sEntryStartX, m_x);
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), sEntryStartY, m_y);
	}
	return S_FALSE;
}

HRESULT CDriverCCam::SetCursor(UINT nFlags, POINT pt)
{
	HCURSOR hCur = NULL;
	if (hCur)
		::SetCursor(hCur);
	return S_OK;
}

HRESULT CDriverCCam::SetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int nValue, UINT nFlags)
{
	_bstr_t bstrSec1(bstrSec);
	_bstr_t bstrEntry1(bstrEntry);
	if (bstrSec1 == _bstr_t("Mirror"))
	{
		if (bstrEntry1==_bstr_t("Horizontal"))
			m_bHorzMirror = nValue;
		else if (bstrEntry1==_bstr_t("Vertical"))
			m_bVertMirror = nValue;
	}
	else if (bstrEntry1==_bstr_t("Color"))
	{
		__EnterCriticalSection(&m_csSites);
		m_bColor = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Color"), false, true)?true:false;
		InitBuffersAndBI();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("Datamode"))
	{
		__EnterCriticalSection(&m_csSites);
		InitDatamode();
		InitBuffersAndBI();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("IntegrationTime"))
	{
		__EnterCriticalSection(&m_csSites);
//		CC_SetParameter(g_camera, CC_PAR_INTEGRATION_TIME , nValue);
		m_nIntTimes[0] = nValue;
		InitSlopes();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("Gain"))
	{
		__EnterCriticalSection(&m_csSites);
		CC_SetParameter(g_camera, CC_PAR_GAIN , nValue);
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("RedLuminance"))
	{
		__EnterCriticalSection(&m_csSites);
		m_nBrightR = nValue;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("GreenLuminance"))
	{
		__EnterCriticalSection(&m_csSites);
		m_nBrightG = nValue;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("BlueLuminance"))
	{
		__EnterCriticalSection(&m_csSites);
		m_nBrightB = nValue;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("RedContrast"))
	{
		__EnterCriticalSection(&m_csSites);
		m_nContrastR = nValue;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("GreenContrast"))
	{
		__EnterCriticalSection(&m_csSites);
		m_nContrastG = nValue;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("BlueContrast"))
	{
		__EnterCriticalSection(&m_csSites);
		m_nContrastB = nValue;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("RedGamma"))
	{
		__EnterCriticalSection(&m_csSites);
		m_dGammaR = nValue/100.-1.;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("GreenGamma"))
	{
		__EnterCriticalSection(&m_csSites);
		m_dGammaG = nValue/100.-1.;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("BlueGamma"))
	{
		__EnterCriticalSection(&m_csSites);
		m_dGammaB = nValue/100.-1.;
		InitCnvBuff();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("Saturation"))
	{
		__EnterCriticalSection(&m_csSites);
		m_nSaturation = nValue;
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("StartX") || bstrEntry1==_bstr_t("StartY") ||
		bstrEntry1==_bstr_t("Width") || bstrEntry1==_bstr_t("Height") ||
		bstrEntry1==_bstr_t("GrabWindowSize"))
	{
		CStdProfileManager::m_pMgr->WriteProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), nValue);
		nFlags = 0;
		__EnterCriticalSection(&m_csSites);
		InitSettings(-1);
		for (int i = 0; i < CDriverCCam::m_arrSites.GetSize(); i++)
			CDriverCCam::m_arrSites[i]->OnChangeSize();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (bstrEntry1==_bstr_t("TotalSlopes"))
	{
		VALUEINFOEX vie;
		if (SUCCEEDED(CCamValues::GetValueInfoByName("Settings", "TotalSlopes", &vie)))
		{
			nValue = Range(nValue,vie.Info.nMin,vie.Info.nMax);
			if (nValue != m_nSlopes)
			{
				__EnterCriticalSection(&m_csSites);
				if (nValue > m_nSlopes)
					DoReadSlopesValues(m_nSlopes, nValue);
				m_nSlopes = nValue;
				InitSlopes();
				__LeaveCriticalSection(&m_csSites);
			}
		}
	}
	else if (wcsncmp(bstrEntry,L"IntegrationTimeProc",wcslen(L"IntegrationTimeProc"))==0||
		wcsncmp(bstrEntry,L"Anaval1Extra",wcslen(L"Anaval1Extra"))==0)
	{
		CStdProfileManager::m_pMgr->WriteProfileInt(_bstr_t(bstrSec), bstrEntry1, nValue);
		nFlags = 0;
		__EnterCriticalSection(&m_csSites);
		ReadSlopesValues();
		InitSlopes();
		__LeaveCriticalSection(&m_csSites);
	}
	else if (_bstr_t(bstrEntry)==_bstr_t("###WBBuild"))
	{
		if (m_bColor)
		{
			__EnterCriticalSection(&m_csSites);
			m_nBright = m_nBrightR = m_nBrightG = m_nBrightB = 0;
			m_nContrast = m_nContrastR = m_nContrastG = m_nContrastB = 0;
			m_dGamma = m_dGammaR = m_dGammaG = m_dGammaB = 0.;
			int nSaturation = m_nSaturation;
			m_nSaturation = 0;
			InitCnvBuff();
			MakeImage();
			if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("UseContrastForWhiteBalance"), TRUE, true, true))
			{
				if (m_bpp==8)
					DoCalcWhiteBalanceContr<BYTE>(m_BuffCap, ((m_cx*3)>>2)<<2, m_cx, m_cy, m_nContrastR,
						m_nContrastG, m_nContrastB, 0xFF);
				else
					DoCalcWhiteBalanceContr<WORD>((WORD*)(BYTE*)m_BuffCap, ((m_cx*3)>>1)<<1, m_cx, m_cy, m_nContrastR,
						m_nContrastG, m_nContrastB, 0xFFFF);
			}
			else
			{
				if (m_bpp==8)
					DoCalcWhiteBalanceBri<BYTE>(m_BuffCap, ((m_cx*3)>>2)<<2, m_cx, m_cy, m_nBrightR,
						m_nBrightG, m_nBrightB, 0xFF);
				else
					DoCalcWhiteBalanceBri<WORD>((WORD*)(BYTE*)m_BuffCap, ((m_cx*3)>>1)<<1, m_cx, m_cy, m_nBrightR,
						m_nBrightG, m_nBrightB, 0xFFFF);
			}
			m_nSaturation = nSaturation;
			InitCnvBuff();
			__LeaveCriticalSection(&m_csSites);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("RedContrast"), m_nContrastR);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("GreenContrast"), m_nContrastG);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("BlueContrast"), m_nContrastB);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("RedGamma"), int(m_dGammaR*100.)+100);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("GreenGamma"), int(m_dGammaG*100.)+100);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("BlueGamma"), int(m_dGammaB*100.)+100);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("RedLuminance"), m_nBrightR);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("GreenLuminance"), m_nBrightG);
			CStdProfileManager::m_pMgr->WriteProfileInt(_T("Settings"), _T("BlueLuminance"), m_nBrightB);
		}
	}
	else
	{
		CStdProfileManager::m_pMgr->WriteProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), nValue);
		nFlags = 0;
		__EnterCriticalSection(&m_csSites);
		InitSettings(-1);
		__LeaveCriticalSection(&m_csSites);
	}
	if (nFlags==1)
		CStdProfileManager::m_pMgr->WriteProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), nValue);
	return S_OK;
}

HRESULT CDriverCCam::GetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int *pnValue, UINT nFlags)
{
	VALUEINFOEX vie;
	HRESULT hrEx = CCamValues::GetValueInfoByName(_bstr_t(bstrSec), _bstr_t(bstrEntry), &vie);
	if (*pnValue == -1 && SUCCEEDED(hrEx))
		*pnValue = vie.Info.nDefault;
	*pnValue = CStdProfileManager::m_pMgr->_GetProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), *pnValue);
	return S_OK;
}




