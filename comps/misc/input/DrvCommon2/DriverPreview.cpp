#include "StdAfx.h"
#include "DrvPreview.h"
#include "VT5Profile.h"
#include <vfw.h>
#include "ddib.h"
#include "CamValues.h"
#include "MethCombo.h"
#include "Focus.h"
#include "core5.h"
#if !defined(COMMON1)
#include <ComDef.h>
#include "com_main.h"
#include "misc_utils.h"
#endif
#include "debug.h"

const int DummyDX = 1280;
const int DummyDY = 1024;

CDibDraw	dd;
static DWORD s_dwPeriod = 0;
static DWORD s_dwLastTicks = 0;

CCamBoolValue g_VertMirror(_T("Image"), _T("VertMirror"), false);
CCamBoolValue g_HorzMirror(_T("Image"), _T("HorzMirror"), false);
CCamBoolValue g_Gray(_T("Image"), _T("Gray"), false);
INTCOMBOITEMDESCR s_aPlanes[] =
{
	{0, "Red"},
	{1, "Green"},
	{2, "Blue"},
	{3, "Gray"},
};
CCamIntComboValue g_GrayPlane(_T("Image"), _T("GrayPlane"), s_aPlanes, 4, 3);
CCamIntValue g_Brightness(_T("Enhancement"), _T("Brightness"), 0, -255, 255, IRepr_SmartIntEdit);
CCamIntValue g_Contrast(_T("Enhancement"), _T("Contrast"), 0, -255, 255, IRepr_SmartIntEdit);
CCamIntValue g_Gamma(_T("Enhancement"), _T("Gamma"), 0, -100, 100, IRepr_SmartIntEdit);

CStringsIds g_StringsIds;

CCamIntValue g_VideoFPS(_T("Video"), _T("RateFPS"), 25, 1, 100, IRepr_SmartIntEdit);
CCamIntValue g_VideoTime(_T("Video"), _T("Time"), 1, 1, 10, IRepr_SmartIntEdit);
int g_idVideoFileName = -1;
int g_idVideoBrowse = -1;
int g_idsAviFilter = -1;
CCamStringValue g_VideoDest(_T("Video"), _T("Destination"), _T("c:\\capture.avi"),
	_MAX_PATH, IRepr_Static);


bool MakeDummyDIB(IDIBProvider *pDIBPrv, int cx, int cy, int bpp)
{
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(::DIBSize(cx, cy, bpp, true));
	if (lpbi)
		::InitDIB(lpbi, cx, cy, bpp, true);
	return lpbi!=NULL;
}

class CMaintainImage
{
public:
	IImage3 **m_ppimg;
	CMaintainImage(IImage3 *&pimg, IUnknown *punk)
	{
		m_ppimg = &pimg;
		IImage3 *pimg1 = NULL;
		punk->QueryInterface(IID_IImage3, (void **)&pimg1);
		*m_ppimg = pimg1;
	}
	~CMaintainImage()
	{
		if (*m_ppimg)
			(*m_ppimg)->Release();
		(*m_ppimg) = NULL;
	}
};

class CMaintainLPBI
{
public:
	LPBITMAPINFOHEADER *m_ppbi;
	CMaintainLPBI(LPBITMAPINFOHEADER &pbi, LPBITMAPINFOHEADER pbiSet)
	{
		m_ppbi = &pbi;
		*m_ppbi = pbiSet;
	}
	~CMaintainLPBI()
	{
		*m_ppbi = NULL;
	}
};

CRectCursors::CRectCursors()
{
	m_hTop  = m_hBottom = m_hLeft = m_hRight = NULL;
	m_hTopLeft = m_hTopRight = m_hBottomLeft = m_hBottomRight = NULL;
}

CRectCursors::~CRectCursors()
{
}

void CRectCursors::LoadCursors(int t, int b, int l, int r, int tl, int tr, int bl, int br, int m)
{
	m_hTop = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(t));
	m_hBottom = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(b));
	m_hLeft = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(l));
	m_hRight = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(r));
	m_hTopLeft = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(tl));
	m_hBottomLeft = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(bl));
	m_hTopRight = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(tr));
	m_hBottomRight = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(br));
	m_hMove = LoadCursor(app::instance()->handle(), MAKEINTRESOURCE(m));
}

void CRectCursors::LoadStdCursors()
{
	m_hTop = LoadCursor(NULL, IDC_SIZENS);
	m_hBottom = LoadCursor(NULL, IDC_SIZENS);
	m_hLeft = LoadCursor(NULL, IDC_SIZEWE);
	m_hRight = LoadCursor(NULL, IDC_SIZEWE);
	m_hTopLeft = LoadCursor(NULL, IDC_SIZENWSE);
	m_hBottomLeft = LoadCursor(NULL, IDC_SIZENESW);
	m_hTopRight = LoadCursor(NULL, IDC_SIZENESW);
	m_hBottomRight = LoadCursor(NULL, IDC_SIZENWSE);
	m_hMove = LoadCursor(NULL, IDC_HAND);
}


HCURSOR CRectCursors::CursorByRectZone(CRectZone rz)
{
	if (rz == Rect_Top)
		return m_hTop;
	else if (rz == Rect_Bottom)
		return m_hBottom;
	else if (rz == Rect_Left)
		return m_hLeft;
	else if (rz == Rect_Right)
		return m_hRight;
	else if (rz == Rect_TopLeft)
		return m_hTopLeft;
	else if (rz == Rect_TopRight)
		return m_hTopRight;
	else if (rz == Rect_BottomLeft)
		return m_hBottomLeft;
	else if (rz == Rect_BottomRight)
		return m_hBottomRight;
	else if (rz == Rect_Move)
		return m_hMove;
	else
		return NULL;
}

CDriverPreview::CDriverPreview()
{
	m_nGrabCount = 0;
//	InitializeCriticalSection(&m_csSites);
	m_CamState = NotInited;
	m_pimgCap = NULL;
	m_lpbiCap = NULL;
	m_nShift16To16 = 4;
	m_nShift16To8 = 4;
	m_nIdNewMeth = -1;
	m_nIdDeleteMeth = -1;
	m_nIdStaticFocusMax = -1;
	m_nIdStaticFocusCur = -1;
	if (CStdProfileManager::m_pMgr->GetMethodic().IsEmpty())
	{
		CString sMeth = CStdProfileManager::m_pMgr->GetProfileString(_T("Methodics"), _T("Default"), NULL, false, true);
		CStdProfileManager::m_pMgr->InitMethodic(sMeth);
	}
	m_Cursors.LoadStdCursors();
	m_bTrack = false;
	m_TrackRectZone = Rect_None;
	m_pTrackRect = NULL;
	m_bSizeChanged = false;
	m_bZooming = false;
	m_bRectsMapped = false;
	m_nStopGrabCount = 0;
	m_bGrabOk = false;
	m_bSettingsChanged = true; // Settings not initialized
	m_bMakeVideo = false;
	m_bVideoMode = false;
	m_nFrameCount = 0;
	m_bWhiteBalance = false;
}

CDriverPreview::~CDriverPreview()
{
}

void CDriverPreview::BeforeFinalRelease()
{
	if (m_nGrabCount>0)
	{
		OnStopGrab();
		m_nGrabCount = 0;
	}
	if (m_CamState == Succeeded)
	{
		DeinitCamera();
		m_CamState = NotInited;
	}
}

void CDriverPreview::ReadSettings()
{
	CCamValue::InitChain(0, true);
	m_ExposureMask.Reinit();
	m_bSettingsChanged = true;
}

void CDriverPreview::ResetAllControls()
{
	for (int i = 0; i < m_arrDialogs.GetSize(); i++)
		m_arrDialogs[i]->OnResetSettings(NULL, NULL);
}

void CDriverPreview::UpdateCmdUI()
{
	for (int i = 0; i < m_arrDialogs.GetSize(); i++)
		m_arrDialogs[i]->UpdateCmdUI();
	
}

void CDriverPreview::PlanesReady16(LPWORD lpRed, LPWORD lpGreen, LPWORD lpBlue)
{
	bool bHorzMirror = bool(g_HorzMirror)&&(GetExFlags()&EFG_HMIRROR)==0;
	bool bVertMirror = bool(g_VertMirror)&&(GetExFlags()&EFG_VMIRROR)==0;
	if (m_pimgCap != 0)
	{
		if (m_biCapFmt.biBitCount == 48)
			AttachPlanes16ToRGB(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpRed,
				lpGreen, lpBlue, bHorzMirror, bVertMirror, &m_Conv16To16);
		else if (m_biCapFmt.biBitCount == 16)
		{
			if ((int)g_GrayPlane == 0)
				AttachPlane16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpRed,
					bHorzMirror, bVertMirror, &m_Conv16To16);
			else if ((int)g_GrayPlane == 1)
				AttachPlane16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpGreen,
					bHorzMirror, bVertMirror, &m_Conv16To16);
			else if ((int)g_GrayPlane == 2)
				AttachPlane16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpBlue,
					bHorzMirror, bVertMirror, &m_Conv16To16);
			else
				AttachPlanes16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpRed,
					lpGreen, lpBlue, bHorzMirror, bVertMirror, &m_Conv16To16);
		}
	}
	else
	{
		LPBITMAPINFOHEADER lpbi = NULL;
		if (m_lpbiCap != 0)
			lpbi = m_lpbiCap;
		else if (m_nGrabCount > 0)
			lpbi = m_Dib.m_lpbi;
		if (lpbi != NULL)
		{
			if (lpbi->biBitCount == 24)
				ConvertPlanes16ToDIB24(lpbi, lpRed, lpGreen, lpBlue, bHorzMirror, bVertMirror, &m_Conv16To8);
			else if (lpbi->biBitCount == 8)
			{
				if ((int)g_GrayPlane == 0)
					ConvertPlane16ToDIB8(lpbi, lpRed, bHorzMirror, bVertMirror, &m_Conv16To8);
				else if ((int)g_GrayPlane == 1)
					ConvertPlane16ToDIB8(lpbi, lpGreen, bHorzMirror, bVertMirror, &m_Conv16To8);
				else if ((int)g_GrayPlane == 2)
					ConvertPlane16ToDIB8(lpbi, lpBlue, bHorzMirror, bVertMirror, &m_Conv16To8);
				else
					ConvertPlanes16ToDIB8(lpbi, lpRed, lpGreen, lpBlue, bHorzMirror, bVertMirror, &m_Conv16To8);
			}
			if (m_lpbiCap == 0 && m_nGrabCount > 0)
				OnPrvFrameReady();
		}
	}
}

//void _AttachPlanes16ToGray(IImage3 *pimg, int cx, int cy, LPWORD pSrcR, LPWORD pSrcG, LPWORD pSrcB,
//	bool bHMirror, bool bVMirror, int nShift);


void CDriverPreview::GrayReady16(LPWORD lpGray)
{
	bool bHorzMirror = bool(g_HorzMirror)&&(GetExFlags()&EFG_HMIRROR)==0;
	bool bVertMirror = bool(g_VertMirror)&&(GetExFlags()&EFG_VMIRROR)==0;
	if (m_pimgCap != 0)
	{
		if (m_biCapFmt.biBitCount == 48)
			AttachPlanes16ToRGB(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpGray,
				lpGray, lpGray, bHorzMirror, bVertMirror, &m_Conv16To16);
		else if (m_biCapFmt.biBitCount == 16)
//			_AttachPlanes16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpGray,
//				lpGray, lpGray, bHorzMirror, bVertMirror, m_nShift16To16);
			AttachPlane16ToGray(m_pimgCap, m_biCapFmt.biWidth, m_biCapFmt.biHeight, lpGray,
				bHorzMirror, bVertMirror, &m_Conv16To16);
	}
	else
	{
		LPBITMAPINFOHEADER lpbi = NULL;
		if (m_lpbiCap != 0)
			lpbi = m_lpbiCap;
		else if (m_nGrabCount > 0)
			lpbi = m_Dib.m_lpbi;
		if (lpbi != NULL)
		{
			if (lpbi->biBitCount == 24)
				ConvertPlanes16ToDIB24(lpbi, lpGray, lpGray, lpGray, bHorzMirror, bVertMirror, &m_Conv16To8);
			else if (lpbi->biBitCount == 8)	
			{
			//LPBYTE	lpGray1 =(LPBYTE)lpGray;
				//ConvertPlane16ToDIB8_Mutech(lpbi, lpGray1, bHorzMirror, bVertMirror, &m_Conv16To8);
				ConvertPlane16ToDIB8(lpbi, lpGray, bHorzMirror, bVertMirror, &m_Conv16To8);			
			}
			if (m_lpbiCap == 0 && m_nGrabCount > 0)
				OnPrvFrameReady();
		}
	}
}
//sergey 06/04/06

void CDriverPreview::GrayReady16_Mutech(LPBYTE lpGray)
{
	LPBYTE dest;
	bool bHorzMirror = bool(g_HorzMirror)&&(GetExFlags()&EFG_HMIRROR)==0;
	bool bVertMirror = bool(g_VertMirror)&&(GetExFlags()&EFG_VMIRROR)==0;

	LPBITMAPINFOHEADER lpbi = NULL;
		if (m_lpbiCap != 0)
			lpbi = m_lpbiCap;
		else if (m_nGrabCount > 0)
			lpbi = m_Dib.m_lpbi;
		if (lpbi != NULL)
		{
			dest =(LPBYTE)m_Dib.GetData();
				DisplayImageDataDIB_Mutech(lpbi, lpGray, bHorzMirror, bVertMirror, &m_Conv16To8,dest);

				//ConvertPlane16ToDIB8(lpbi, lpGray, bHorzMirror, bVertMirror, &m_Conv16To8);
			}
				//end
			if (m_lpbiCap == 0 && m_nGrabCount > 0)
				OnPrvFrameReady();
	
}
//end

void CDriverPreview::OnPrvFrameReady()
{
	CDriverSynch s(this);
	if (!m_bMakeVideo && !m_bWhiteBalance)
	{
		m_ExposureMask.SetMask(m_Dib.m_lpbi, m_Dib.GetData(), true);
		if (!m_bZooming)
			CCamValueRect::MapAllRects(m_Dib.m_lpbi);
		DWORD dwTicks = timeGetTime();
		s_dwPeriod = dwTicks-s_dwLastTicks;
		s_dwLastTicks = dwTicks;
		m_bRectsMapped = !m_bZooming;
	}
	if (!m_bMakeVideo || (GetExFlags()&EFG_PRVVIDEO))
	{
		for (int i = 0; i < m_arrSites.GetSize(); i++)
			m_arrSites[i]->Invalidate();
	}
	m_nFrameCount++;
}

void CDriverPreview::NotifyChangeSizes()
{
	CDriverSynch s(this);
	for (int i = 0; i < m_arrSites.GetSize(); i++)
		m_arrSites[i]->OnChangeSize();
}

bool CDriverPreview::WaitNextFrameUI(DWORD dwTimeoutMs, int nFrame)
{
	int nFrameCount = m_nFrameCount;
	DWORD dwNow = GetTickCount();
	MSG msg;
	while (m_nFrameCount < nFrameCount+nFrame && (dwTimeoutMs == 0 || GetTickCount() < dwNow+dwTimeoutMs))
	{
		if (::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (msg.message == WM_QUIT)
				return false;
			GetMessage(&msg, NULL, 0, 0);
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return m_nFrameCount>=nFrameCount+nFrame;
}

bool CDriverPreview::WaitNextFrame(DWORD dwSleep, DWORD dwTimeoutMs, int nFrame)
{
	int nFrameCount = m_nFrameCount;
	DWORD dwNow = GetTickCount();
	while (m_nFrameCount < nFrameCount+nFrame && (dwTimeoutMs == 0 || GetTickCount() < dwNow+dwTimeoutMs))
		Sleep(dwSleep);
	return m_nFrameCount>=nFrameCount+nFrame;
}


bool CDriverPreview::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice,
	int nMode, BOOL bFirst, BOOL bForInput)
{
	return OnExecuteSettings2(hwndParent, pTarget, nDevice, nMode, bFirst, bForInput, 0);
}


HRESULT CDriverPreview::ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
	BOOL bFirst, BOOL bForInput)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nMode == 0)
		return S_OK;
	else
		return OnExecuteSettings(hwndParent, pTarget, nDevice, nMode, bFirst, bForInput)? S_OK : S_FALSE;
}


HRESULT CDriverPreview::InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	//sergey 01/08/06
	bool b1;
	BOOL bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, true, true);
	BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	if (bGetImageFromPreview && m_CamState==Succeeded)
	{
  	
		DWORD dwTick = GetTickCount();
		while(m_Dib.m_lpbi == 0)
		{
			if (GetTickCount()-dwTick>2000) break;
			Sleep(20);
		}
		HRESULT hr = E_FAIL;
		if (m_Dib.m_lpbi)
		{
			
				IImage3Ptr sptrI(pUnkImage);
			 b1= AttachDIBBits(sptrI, m_Dib.m_lpbi,NULL);
			return b1?S_OK:E_FAIL;
				
		}
		
	}
  
		
	//end
	if (m_CamState==Succeeded&&!m_Dib.IsEmpty())
	{
		if (m_nGrabCount>0&&CanCaptureDuringPreview() || bFromSettings&&CanCaptureFromSettings())
		{
			IImage3Ptr sptrI(pUnkImage);
			bool b = AttachDIBBits(sptrI, m_Dib.m_lpbi,NULL);
			return b?S_OK:E_FAIL;
		}
	}
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0&&!CanCaptureDuringPreview(), false);
	if (!DoInitCamera())
	{
		MakeDummyImage(pUnkImage, 1280, 1024);
		return S_OK;
	}
	BITMAPINFO256 bi;
	memset(&m_biCapFmt, 0, sizeof(m_biCapFmt));
	memset(&bi, 0, sizeof(bi));
	if (!PreinitGrab2(CaptureMode_Snapshot, 0) || !GetFormat2(CaptureMode_Snapshot, &bi.bmiHeader))
	{
		MakeDummyImage(pUnkImage, 1280, 1024);
		return S_OK;
	}
	m_bSettingsChanged = false;
	m_Conv16To16.Init(bi.bmiHeader.biBitCount, (int)g_Brightness, (int)g_Contrast,
		double((int)g_Gamma)/100.);
	m_biCapFmt = bi.bmiHeader;
	m_biCapFmt.biBitCount = (bool)g_Gray?16:48;
	CMaintainImage Maintain(m_pimgCap, pUnkImage);
	ProcessCapture();
	return S_OK;
}

HRESULT CDriverPreview::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	if (m_CamState==Succeeded&&!m_Dib.IsEmpty())
	{
		if (m_nGrabCount>0&&CanCaptureDuringPreview() || bFromSettings&&CanCaptureFromSettings())
		{
			DWORD dwSize = m_Dib.Size(true);
			LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(dwSize);
			memcpy(lpbi, m_Dib.m_lpbi, dwSize);
			return S_OK;
		}
	}
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0&&!CanCaptureDuringPreview(), false);
	if (!DoInitCamera())
		return MakeDummyDIB(pDIBPrv, DummyDX, DummyDY, 24)?S_OK:E_FAIL;
	memset(&m_biCapFmt, 0, sizeof(m_biCapFmt));
	BITMAPINFO256 bi;
	memset(&m_biCapFmt, 0, sizeof(m_biCapFmt));
	memset(&bi, 0, sizeof(bi));
	if (!PreinitGrab2(CaptureMode_Snapshot, 0) || !GetFormat2(CaptureMode_Snapshot, &bi.bmiHeader))
		return MakeDummyDIB(pDIBPrv, DummyDX, DummyDY, 24)?S_OK:E_FAIL;
	m_bSettingsChanged = false;
	m_Conv16To8.Init(bi.bmiHeader.biBitCount, (int)g_Brightness, (int)g_Contrast,
		double((int)g_Gamma)/100.);
	m_biCapFmt = bi.bmiHeader;
	m_biCapFmt.biBitCount = (bool)g_Gray?8:24;
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(::DIBSize(m_biCapFmt.biWidth,
		m_biCapFmt.biHeight, m_biCapFmt.biBitCount, true));
	if (lpbi == NULL)
		return E_FAIL;
	::InitDIB(lpbi, m_biCapFmt.biWidth, m_biCapFmt.biHeight, m_biCapFmt.biBitCount, true);
	CMaintainLPBI Maintain(m_lpbiCap, lpbi);
	ProcessCapture();
	return S_OK;
}

void CDriverPreview::InitVideo(int idEditRate, int idEditTime, int idStaticPath, int idBrowse,
	int idsAviFilter)
{
	g_VideoFPS.SetId(idEditRate);
	g_VideoTime.SetId(idEditTime);
	g_idVideoFileName = idStaticPath;
	g_idVideoBrowse = idBrowse;
	g_idsAviFilter = idsAviFilter;
}

class CMaintainVideoMode
{
	CDriverPreview *m_pDrv;
public:
	CMaintainVideoMode(CDriverPreview *pDrv)
	{
		m_pDrv = pDrv;
		m_pDrv->m_bVideoMode = true;
		m_pDrv->OnChangeVideoMode();
	}
	~CMaintainVideoMode()
	{
		m_pDrv->m_bVideoMode = false;
		m_pDrv->OnChangeVideoMode();
	}
};

HRESULT CDriverPreview::InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CMaintainVideoMode MaintainVideoMode(this);
	HWND hwnd = NULL;
	IApplicationPtr sptrApp(GetAppUnknown());
	if (sptrApp != 0) sptrApp->GetMainWindowHandle(&hwnd);
	bool b = OnExecuteSettings2(hwnd, NULL, 0, 0, FALSE, FALSE, ES2_VIDEO);
	if (b)
	{
		if (*pbstrVideoFile)
			::SysFreeString(*pbstrVideoFile);
		CString sFile = g_VideoDest;
		*pbstrVideoFile = sFile.AllocSysString();
	}
	return b?S_OK:S_FALSE;
}

void CDriverPreview::MakeVideoFrame(LPBYTE lpDataSrc, DWORD dwSize)
{
	DWORD dwNow = GetTickCount();
	if (m_dwStartVideo == 0)
		m_dwStartVideo = dwNow;
	int nFrame = (dwNow-m_dwStartVideo)/m_dwFrameMs;
	if (nFrame >= 0 && nFrame < m_FrmOk.size())
	{
		LPBYTE lpData = (LPBYTE)m_BuffVideo;
		LPBYTE lpFrame = lpData+nFrame*m_CaptureVideoToMemory.dwFrameSizeSrc;
		DWORD dw = min(m_CaptureVideoToMemory.dwFrameSizeSrc,dwSize);
		memcpy(lpFrame,lpDataSrc,dw);
		m_FrmOk[nFrame] = 1;
		m_nCurFrame = nFrame;
	}
}

void CDriverPreview::MakeVideo(int nFrames, DWORD dwFrameMs, IVideoHook *pHook)
{
	m_dwFrameMs = dwFrameMs;
	m_BuffVideo.alloc(nFrames*m_CaptureVideoToMemory.dwFrameSizeSrc);
	m_BuffVideo.zero();
	m_FrmOk.alloc(nFrames);
	m_FrmOk.zero();
	m_dwStartVideo = 0;//GetTickCount();
	m_nCurFrame = 0;
	int nCurFrame = 0;
	{
		CCritSectionAccess csa(&m_csCap);
		m_bMakeVideo = true;
	}
	DWORD dwTotalTime = dwFrameMs*nFrames;
	while (m_dwStartVideo == 0 || GetTickCount() < m_dwStartVideo+dwTotalTime)
	{
		int nRet;
		MSG msg;
		if (PeekMessage(&msg, 0, 0,0, PM_NOREMOVE))
		{
			if ((nRet = GetMessage(&msg, NULL, 0, 0)) != 0)
			{ 
				if (nRet != -1)
				{
					TranslateMessage(&msg); 
					DispatchMessage(&msg); 
				} 
			}
		}
		if (m_nCurFrame > nCurFrame)
		{
			pHook->OnFrame(m_nCurFrame);
			nCurFrame = m_nCurFrame;
		}
		Sleep(0);
	}
	{
		CCritSectionAccess csa(&m_csCap);
		m_bMakeVideo = false;
	}
}

class CMaintainVideo
{
public:
	BITMAPINFOHEADER bih;
	bool bFmtChanged;
	CDriverPreview *m_pDrv;
	CMaintainVideo(CDriverPreview *pDrv)
	{
		m_pDrv = pDrv;
		bFmtChanged = false;
		CCritSectionAccess csa(&m_pDrv->m_csCap);
		bih = *m_pDrv->m_Dib.m_lpbi;
		m_pDrv->m_Dib.InitBitmap(m_pDrv->m_CaptureVideoToMemory.biPrv.biWidth,
			m_pDrv->m_CaptureVideoToMemory.biPrv.biHeight,
			m_pDrv->m_CaptureVideoToMemory.biPrv.biBitCount);
		bFmtChanged = true;
	}
	~CMaintainVideo()
	{
		if (bFmtChanged)
		{
			CCritSectionAccess csa(&m_pDrv->m_csCap);
			m_pDrv->m_Dib.InitBitmap(bih.biWidth, bih.biHeight, bih.biBitCount);
		}
	}
};

bool CDriverPreview::GetVideoFormats(CCaptureVideoToMemory *pCap)
{
	pCap->biPrv = *m_Dib.m_lpbi;
	pCap->biDst = *m_Dib.m_lpbi;
	pCap->dwFrameSizeSrc = m_Dib.Size(false);
	return true;
}

void CDriverPreview::DoCaptureVideoToMemory(int nFrames, DWORD dwFrameMs, IVideoHook *pHook)
{
	memset(&m_CaptureVideoToMemory, 0, sizeof(m_CaptureVideoToMemory));
	GetVideoFormats(&m_CaptureVideoToMemory);
	DWORD dwFlags = GetExFlags();
	if (dwFlags & EFG_PRVVIDEO)
	{
		CMaintainVideo MaintainVideo(this);
		MakeVideo(nFrames, dwFrameMs, pHook);
	}
	else
		MakeVideo(nFrames, dwFrameMs, pHook);
}


bool CDriverPreview::CaptureVideoToMemory(int nDevice, int nMode, IVideoHook *pVideoHook)
{
	if (m_nGrabCount == 0 || m_Dib.IsEmpty()) // used only from video dialog
		return false;
//	if (!PreinitGrab2(CaptureMode_Video, 0))
//		return false;
/*	BITMAPINFO256 biFmt;
	if (!GetFormat2(CaptureMode_Video, &biFmt.bmiHeader))
	{
//		PostDeinit2();
		return false;
	}*/
	int nFPSType = 0;
	int nFPS = g_VideoFPS;
	CString sFile = g_VideoDest;
	int nTime = g_VideoTime;
	int nFrames = nFPS*nTime;
//	int nFrameSize = m_Dib.Size(false);
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
//	__EnterCriticalSection(&m_csSites);
	DoCaptureVideoToMemory(nFrames,dwFrameMS,pVideoHook);
//	__LeaveCriticalSection(&m_csSites);
	if (pVideoHook) pVideoHook->OnEndStage(0);
	if (pVideoHook) pVideoHook->OnBeginStage(1, "Saving", nFrames);
	PAVIFILE pf; 
	AVIFileInit();
	HRESULT hr = AVIFileOpen(&pf, sFile, OF_WRITE|OF_CREATE, NULL); 
	if (SUCCEEDED(hr))
	{
		DWORD dwMSecsPB = g_VideoFPS;
		if (dwMSecsPB == 0) dwMSecsPB = 1;
		dwMSecsPB = 1000/dwMSecsPB;
		CDib Dib;
		Dib.InitBitmap(m_CaptureVideoToMemory.biDst.biWidth, m_CaptureVideoToMemory.biDst.biHeight,
			m_CaptureVideoToMemory.biDst.biBitCount);
	    AVISTREAMINFO strhdr;
		PAVISTREAM ps;
		memset(&strhdr, 0, sizeof(strhdr));
		strhdr.fccType = streamtypeVIDEO;
		strhdr.dwScale = dwMSecsPB*1000;
		strhdr.dwRate = 1000000;
		strhdr.dwQuality = 10000;
		SetRect(&strhdr.rcFrame, 0, 0, Dib.cx(), Dib.cy()); 
		hr = AVIFileCreateStream(pf, &ps, &strhdr);
		int iFrmOk = 0;
		if (SUCCEEDED(hr))
		{
			if (Dib.bpp() == 8)
				hr = AVIStreamSetFormat(ps, 0, Dib.m_lpbi, sizeof(BITMAPINFOHEADER)+Dib.m_lpbi->biClrUsed*sizeof(RGBQUAD)); 
			else
				hr = AVIStreamSetFormat(ps, 0, Dib.m_lpbi, sizeof(BITMAPINFOHEADER)); 
			DWORD dwSize = m_Dib.Size(false);
			DWORD dwLast = GetTickCount();
			for (int i = 0; i < nFrames; i++)
			{
				if (m_FrmOk[i])
				{
					ConvertVideoFrame(&m_BuffVideo[i*m_CaptureVideoToMemory.dwFrameSizeSrc], Dib);
					hr = AVIStreamWrite(ps, i, 1, Dib.GetData(), Dib.Size(false), AVIIF_KEYFRAME, NULL, NULL);
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
			s.Format(g_StringsIds.idsVideoCaptured, (const char *)sFile, iFrmOk, nFrames-iFrmOk);
			AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		}
	}
	else
	{
		AVIFileExit();
		CString s;
		s.Format(g_StringsIds.idsFileNotOpened, sFile);
		AfxMessageBox(s, MB_OK|MB_ICONEXCLAMATION);
		PostDeinit2();
		return false;
	}
	AVIFileExit();
	pVideoHook->OnEndStage(1);
//	PostDeinit2();
	return true;
}

HRESULT CDriverPreview::InputVideoFile2(int nDevice, int nMode, IVideoHook *pVideoHook)
{
	return CaptureVideoToMemory(nDevice, nMode, pVideoHook) ? S_OK : S_FALSE;
}

HRESULT CDriverPreview::GetShortName(BSTR *pbstrShortName)
{
	CString strDriverName(OnGetShortName());
	*pbstrShortName = strDriverName.AllocSysString();
	return S_OK;
}

HRESULT CDriverPreview::GetLongName(BSTR *pbstrLongName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CString strDriverFullName;
	strDriverFullName.LoadString(OnGetLongNameId());
	*pbstrLongName = strDriverFullName.AllocSysString();
	return S_OK;
}

HRESULT CDriverPreview::GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue)
{
	CString s(bstrName);
	*pValue = CStdProfileManager::m_pMgr->GetProfileValue(NULL, s);
	return S_OK;
}

HRESULT CDriverPreview::SetValue(int nCurDev, BSTR bstrName, VARIANT Value)
{
	CString s(bstrName);
	CStdProfileManager::m_pMgr->WriteProfileValue(NULL, s, Value);
	return S_OK;
}

HRESULT CDriverPreview::GetDevicesCount(int *pnCount)
{
	*pnCount = 1;
	return S_OK;
}

HRESULT CDriverPreview::GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (nDev != 0) return E_INVALIDARG;
	CString strDriverName(OnGetShortName());
	CString strDriverFullName,strCategory;
	strDriverFullName.LoadString(OnGetLongNameId());
	strCategory.LoadString(OnGetInputId());
	*pbstrShortName = strDriverName.AllocSysString();
	*pbstrLongName = strDriverFullName.AllocSysString();
	*pbstrCategory = strCategory.AllocSysString();
	return S_OK;
}

bool CDriverPreview::DoInitCamera()
{
	if (m_CamState == NotInited)
	{
		ReadSettings();
		m_CamState = InitCamera()?Succeeded:Failed;
	}
	return m_CamState==Succeeded?true:false;
}


HRESULT CDriverPreview::StartImageAquisition(int, BOOL bStart)
{
	
	if (bStart)
	{
		m_nGrabCount++;
		if (m_nGrabCount==1)
		{
			m_bGrabOk = false;
			DoInitCamera();
			if (m_CamState == Succeeded)
			{
				BITMAPINFO256 bi;
				if (PreinitGrab2(CaptureMode_Preview,0))
				{
					m_bSettingsChanged = false;
					if (GetFormat2(CaptureMode_Preview, &bi.bmiHeader))
					{
						m_Conv16To8.Init(bi.bmiHeader.biBitCount, (int)g_Brightness,
							(int)g_Contrast, double((int)g_Gamma)/100.);
						m_Dib.InitBitmap(bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, (bool)g_Gray?8:24);
						timeBeginPeriod(1);
						OnStartGrab();
						m_bGrabOk = true;
					}
				}
			}
			else
				return E_FAIL;
		}
	}
	else
	{
		try
	{
		--m_nGrabCount;		
		if (m_nGrabCount == 0 && m_CamState == Succeeded)
		{
			OnStopGrab();
			timeEndPeriod(1);
		}
	}
	catch( CException* e )
   {
      
	  e->ReportError();
   }
	}
	
	return S_OK;
}

HRESULT CDriverPreview::IsImageAquisitionStarted(int, BOOL *pbAquisition)
{
	*pbAquisition = m_nGrabCount > 0;
	return S_OK;
}

HRESULT CDriverPreview::AddDriverSite(int, IDriverSite *pSite)
{
	CDriverSynch s(this);
//	EnterCriticalSection(&m_csSites);
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
//	LeaveCriticalSection(&m_csSites);
	return S_OK;
}

HRESULT CDriverPreview::RemoveDriverSite(int, IDriverSite *pSite)
{
  try
  {
	CDriverSynch s(this);
//	EnterCriticalSection(&m_csSites);
	for (int i = 0; i < m_arrSites.GetSize(); i++)
	{
		if (m_arrSites[i] == pSite)
		{
			m_arrSites.RemoveAt(i);
			pSite->Release();
		}
	}
//	LeaveCriticalSection(&m_csSites);
  }
  catch( CException* e )
   {
      
	  e->ReportError();
   }
	return S_OK;
}

HRESULT CDriverPreview::BeginPreview(int nDev, IInputPreviewSite *pSite)
{
	HRESULT hr = StartImageAquisition(0,TRUE);
	if (FAILED(hr)) return hr;
	return AddDriverSite(0,pSite);
}

HRESULT CDriverPreview::EndPreview(int nDev, IInputPreviewSite *pSite)
{
	try
	{
	RemoveDriverSite(0,pSite);
	return StartImageAquisition(0,FALSE);
	}
	catch( CException* e )
   {
      
	  e->ReportError();
   }
}

HRESULT CDriverPreview::GetSize(int nDev, short *pdx, short *pdy)
{
	CCritSectionAccess csa(&m_csCap);
	if (m_Dib.m_lpbi != NULL)
	{
		*pdx = (short)m_Dib.cx();
		*pdy = (short)m_Dib.cy();
	}
	else
	{
		*pdx = DummyDX;
		*pdy = DummyDY;
	}
	*pdx *= GetPreviewZoom();
	*pdy *= GetPreviewZoom();
	return S_OK;
}

HRESULT CDriverPreview::GetPreviewFreq(int nDev, DWORD *pdwFreq)
{
	*pdwFreq = 0;
	return S_OK;
}

HRESULT CDriverPreview::GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod)
{
	*pdwPeriod = s_dwPeriod;
	return S_OK;
}

HRESULT CDriverPreview::GetImage(int nDev, LPVOID *ppData, DWORD *pdwDataSize)
{
	if (m_Dib.m_lpbi != NULL)
	{
		*ppData = m_Dib.m_lpbi;
		*pdwDataSize = m_Dib.Size(true);
		return S_OK;
	}
	else
	{
		*ppData = NULL;
		*pdwDataSize = 0;
		return S_FALSE;
	}
}


HRESULT CDriverPreview::DrawRect(int nDevice, HDC hDC, LPRECT prcSrc, LPRECT prcDst)
{

	      
	if (m_CamState == Succeeded && m_bGrabOk)
	{
		CCritSectionAccess csa(&m_csCap);
		if (RectHeight(prcSrc) == RectHeight(prcDst) && RectHeight(prcDst) == m_Dib.cy()&&
			RectWidth(prcSrc) == RectWidth(prcDst) && RectWidth(prcDst) == m_Dib.cx()&&
			GetPreviewZoom() == 1)
		{
			int nWidth = prcDst->right-prcDst->left;
			int nHeight = prcDst->bottom-prcDst->top;
			SetDIBitsToDevice(hDC, prcDst->left, prcDst->top, nWidth, nHeight,
				prcSrc->left,m_Dib.cy()-prcSrc->top-nHeight-1,0,
				m_Dib.cy(),m_Dib.GetData(),(LPBITMAPINFO)m_Dib.m_lpbi,
				DIB_RGB_COLORS);
             
			m_bZooming = false;
		}
		else
		{
			CRect rcSrc;
			int nZoom = GetPreviewZoom();
        
			rcSrc.left = min(max(0,prcSrc->left/nZoom),m_Dib.cx()-1);
			rcSrc.top = min(max(0,prcSrc->top/nZoom),m_Dib.cy()-1);
			
			rcSrc.right = min(max(0,prcSrc->right/nZoom),m_Dib.cx()-1);
			rcSrc.bottom = min(max(0,prcSrc->bottom/nZoom),m_Dib.cy()-1);
			
			DrawDibDraw(dd.hdd, hDC, prcDst->left, prcDst->top, prcDst->right-prcDst->left,
				prcDst->bottom-prcDst->top,	m_Dib.m_lpbi, m_Dib.GetData(), rcSrc.left,
				rcSrc.top,rcSrc.Width(),rcSrc.Height(), 0);
			m_bZooming = true;
		}
	}
	else

		::FillRect(hDC, prcDst, (HBRUSH)::GetStockObject(BLACK_BRUSH));
	if (!m_bRectsMapped)
		CCamValueRect::DrawAllRects(CDC::FromHandle(hDC), prcSrc, prcDst);
	return S_OK;
}

HRESULT CDriverPreview::SetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int nValue,
	UINT nFlags)
{
	CCamValue *p = CCamValue::FindByName(_bstr_t(bstrSec), _bstr_t(bstrEntry));
	if (g_VertMirror.CheckByName(bstrSec, bstrEntry))
		g_VertMirror.SetValueInt(nValue);
	else if (g_HorzMirror.CheckByName(bstrSec, bstrEntry))
		g_HorzMirror.SetValueInt(nValue);
	else if (g_OUExpEnabled.CheckByName(bstrSec, bstrEntry))
	{
		CCritSectionAccess csa(&m_csCap);
		g_OUExpEnabled.SetValueInt(nValue);
		m_ExposureMask.Reinit();
		if (!(int)g_OUExpEnabled)
			m_Dib.InitGrayPalette();
	}
	else if (g_Focus.CheckByName(bstrSec, bstrEntry))
		g_Focus.SetValueInt(nValue);
	else if (g_FocusRect.CheckByName(bstrSec, bstrEntry))
	{
		CCritSectionAccess csa(&m_csCap);
		g_FocusRect.SetValueInt(nValue);
		g_FocusRect.Init();
	}
	
	else if (!p->NeedReinitCamera())
		p->SetValueInt(nValue);
	else
	{
		XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0, true);
		if (p)
			p->SetValueInt(nValue);
		else
		{
		CStdProfileManager::m_pMgr->WriteProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), nValue);
		ReadSettings();
		}
		m_bSizeChanged = false;
		OnSetValueInt(_bstr_t(bstrSec), _bstr_t(bstrEntry), nValue);
		if (m_bSizeChanged)
			StopGrab.m_bSizeChanged = true;
	}
	return S_OK;
}

HRESULT CDriverPreview::GetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int *pnValue,
	UINT nFlags)
{
	CCamValue *p = CCamValue::FindByName(_bstr_t(bstrSec), _bstr_t(bstrEntry));
	if (p)
		*pnValue = p->GetValueInt();
	else
	*pnValue = CStdProfileManager::m_pMgr->_GetProfileInt(_bstr_t(bstrSec), _bstr_t(bstrEntry),
		*pnValue);
	return S_OK;
}

HRESULT CDriverPreview::GetValueDouble(int nDevice, BSTR bstrSec, BSTR bstrEntry, double *pdValue, UINT nFlags)
{
	CCamValue *pValue = CCamValue::FindByName(_bstr_t(bstrSec), _bstr_t(bstrEntry));
	if (pValue)
	{
		if (pValue->GetVI()->nType == ValueType_Double)
			*pdValue = pValue->GetValueDouble();
	}
	else
		*pdValue = CStdProfileManager::m_pMgr->GetProfileDouble(_bstr_t(bstrSec), _bstr_t(bstrEntry),
			*pdValue, NULL);
	return S_OK;
}

HRESULT CDriverPreview::SetValueDouble(int nDevice, BSTR bstrSec, BSTR bstrEntry, double dValue, UINT nFlags)
{
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0, true);
	CCamValue *pValue = CCamValue::FindByName(_bstr_t(bstrSec), _bstr_t(bstrEntry));
	if (pValue)
	{
		if (pValue->GetVI()->nType == ValueType_Double)
			pValue->SetValueDouble(dValue);
	}
	else
	{
		CStdProfileManager::m_pMgr->WriteProfileDouble(_bstr_t(bstrSec), _bstr_t(bstrEntry),
			dValue);
		ReadSettings();
	}
	OnSetValueDouble(_bstr_t(bstrSec), _bstr_t(bstrEntry), dValue);
	return S_OK;
}

HRESULT CDriverPreview::SetConfiguration(BSTR bstrName)
{
	CString sPrev = CStdProfileManager::m_pMgr->GetMethodic();
	CString sMeth(bstrName);
	if (sMeth.IsEmpty())
		sMeth = CStdProfileManager::m_pMgr->GetProfileString(_T("Methodics"), _T("Default"), NULL, false, true);
	__super::SetConfiguration(_bstr_t((LPCTSTR)sMeth));
	if (sPrev != CStdProfileManager::m_pMgr->GetMethodic())
	{
		ReadSettings();
		OnResetValues();
	}
	return S_OK;
}

HRESULT CDriverPreview::GetValueInfoByName(int nCurDev, const char *pszSection, const char *pszEntry, VALUEINFOEX *pValueInfo)
{
	HRESULT hr = CCamValues::GetValueInfoByName(pszSection,pszEntry,pValueInfo);
	if (hr == S_OK)
		return S_OK;
	CCamValue *pValue = CCamValue::FindByName(pszSection, pszEntry);
	if (pValue)
	{
		VALUEHEADER *pvh = pValue->GetHdr();
		BASICVALUEINFO *pbvi = pValue->GetVI();
		if (pvh != NULL && pbvi != NULL && pbvi->nType == ValueType_Int)
		{
			pValueInfo->Hdr = *pvh;
			pValueInfo->Info = *(VALUEINFO *)pbvi;
			pValueInfo->dwCamData = pValueInfo->dwCamDataType = 0;
			return S_OK;
		}
	}
	return E_INVALIDARG;
}

HRESULT CDriverPreview::GetControlInfo(int nID, VALUEINFOEX2 *pValueInfoEx2)
{
	if (nID > 0 && (nID == m_nIdNewMeth || nID == m_nIdStaticFocusMax || nID == m_nIdStaticFocusCur ||
		nID == g_idVideoBrowse))
		return S_FALSE;
	CCamValue *pValue = CCamValue::FindById(nID);
	if (pValue)
	{
		pValue->OnInitControl();
		pValueInfoEx2->nType = 0;
		pValueInfoEx2->pHdr = pValue->GetHdr();
		pValueInfoEx2->pInfo = pValue->GetVI();
		pValueInfoEx2->pReprData = pValue->GetRepr();
		pValueInfoEx2->pDrvCtrlSite = pValue;
		return S_OK;
	}
	return E_INVALIDARG;
}

HRESULT CDriverPreview::OnInitDialog(IDriverDialog *pDlg)
{
	bool bFound = false;
	for (int i = 0; i < m_arrDialogs.GetSize(); i++)
	{
		if (m_arrDialogs.GetAt(i) == pDlg)
		{
			bFound = true;
			break;
		}
	}
	if (!bFound)
		m_arrDialogs.Add(pDlg);
//	if (m_hwndStaticCurFocus == 0 && m_nStaticCurFocus
	return S_OK;
}

HRESULT CDriverPreview::OnCloseDialog(IDriverDialog *pDlg)
{
	for (int i = 0; i < m_arrDialogs.GetSize(); i++)
	{
		if (m_arrDialogs.GetAt(i) == pDlg)
		{
			m_arrDialogs.RemoveAt(i);
			break;
		}
	}
	return S_OK;
}

void CDriverPreview::DoSelectMethodic(int nMethodic)
{
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0, true);
	g_Methodics.SetValueInt(nMethodic,false);
	CStdProfileManager::m_pMgr->WriteProfileString(_T("Methodics"), _T("Default"),
		CStdProfileManager::m_pMgr->GetMethodic(), true);
	ReadSettings();
	OnResetValues();
	ResetAllControls();
	UpdateCmdUI();
//	Sleep(200);
}


HRESULT CDriverPreview::OnCmdMsg(int nCode, UINT nID, long lHWND)
{
	if (nID > 0)
	{
		if (nID == g_Methodics.GetRepr()->nDlgCtrlId && nCode == LBN_SELCHANGE)
		{
			int nCurSel = (int)::SendMessage((HWND)lHWND, LB_GETCURSEL, 0, 0);
			
			if (nCurSel != (int)g_Methodics)
				DoSelectMethodic(nCurSel);			
			return S_OK;
		}
		else if (nID == m_nIdNewMeth)
		{
			int n = g_Methodics.NewMethodic();
			return S_OK;
		}
		else if (nID == m_nIdDeleteMeth && g_Methodics.GetMethodicsCount() > 1)
		{
			CString sMethIni = CStdProfileManager::m_pMgr->GetIniName(false);
			CString sMethDefIni = CStdProfileManager::m_pMgr->GetDefIniName(false);
			if (sMethDefIni == CStdProfileManager::m_pMgr->GetDefIniName(true))
				sMethDefIni.Empty();
			int nCurMethodic = g_Methodics;
			DoSelectMethodic(nCurMethodic>=1?nCurMethodic-1:nCurMethodic+1);
			remove(sMethIni);
			if (!sMethDefIni.IsEmpty())
				remove(sMethDefIni);
			g_Methodics.OnInitControl();
			g_Methodics.ReinitControls();
			return S_OK;
		}
		else if (nID == g_idVideoBrowse)
		{
			CString sFilter;
			sFilter.LoadString(g_idsAviFilter);
			CString sDefault((const char *)g_VideoDest);
			HWND hwndDlg = ::GetParent((HWND)lHWND);
			HWND hwndText = ::GetDlgItem(hwndDlg, g_idVideoFileName);
			::GetWindowText(hwndText, sDefault.GetBuffer(_MAX_PATH), _MAX_PATH);
			sDefault.ReleaseBuffer();
			IApplicationPtr sptrApp(GetAppUnknown());
			HWND hwndMain;
			sptrApp->GetMainWindowHandle(&hwndMain);
			CFileDialog dlg(TRUE, _T(".avi"), sDefault, OFN_OVERWRITEPROMPT, sFilter,
				CWnd::FromHandle(hwndMain));
			if (dlg.DoModal())
			{
				CString s = dlg.GetPathName();
				g_VideoDest.SetValueString(s, true);
			}
			return S_OK;
		}
	}
	return S_FALSE;
}

HRESULT CDriverPreview::OnUpdateCmdUI(HWND hwnd, int nID)
{
	if (nID > 0)
	{
		if (nID == m_nIdDeleteMeth)
		{
			BOOL bCanDelete = CStdProfileManager::m_pMgr->GetProfileInt(_T("Methodic"),_T("CanDelete"),0,false,false);
			::EnableWindow(hwnd, bCanDelete&&g_Methodics.GetMethodicsCount()>1);
		}
		else if (nID == g_FocusRect.GetRepr()->nDlgCtrlId)
			::EnableWindow(hwnd, (bool)g_Focus);
		else if (nID == g_FocusValue.id()/* || nID == g_MaxFocus.id() || nID == m_nIdStaticFocusMax||
			nID == m_nIdStaticFocusCur*/)
			::ShowWindow(hwnd, (bool)g_Focus?SW_SHOW:SW_HIDE);
	
		
	}
	return S_FALSE;
}

HRESULT CDriverPreview::OnDelayedRepaint(HWND hwndDlg, IDriverDialog *pDlg)
{
	if ((bool)g_Focus)
	{
		CCritSectionAccess csa(&m_csCap);
		CalcFocus(m_Dib.m_lpbi);
	}
	return S_OK;
}

const int nMinRect = 10;
HRESULT CDriverPreview::MouseMove(UINT nFlags, POINT pt)
{
	if (m_bTrack)
	{
		int dx = pt.x-m_ptTrackOrig.x;
		int dy = pt.y-m_ptTrackOrig.y;
		short nDibX,nDibY;
		GetSize(0, &nDibX, &nDibY);
		if (m_TrackRectZone == Rect_Left || m_TrackRectZone == Rect_TopLeft ||
			m_TrackRectZone == Rect_BottomLeft)
		{
			int l = m_rcTrackOrig.left+dx;
			if (l < 0) l = 0;
			if (l > min(m_rcTrackOrig.right-nMinRect,nDibX-1)) l = min(m_rcTrackOrig.right-nMinRect,nDibX-1);
			m_pTrackRect->m_rc.left = l;
		}
		if (m_TrackRectZone == Rect_Right || m_TrackRectZone == Rect_TopRight ||
			m_TrackRectZone == Rect_BottomRight)
		{
			int r = m_rcTrackOrig.right+dx;
			if (r < max(0,m_rcTrackOrig.left+nMinRect)) r = max(0,m_rcTrackOrig.left+nMinRect);
			if (r > nDibX-1) r = nDibX-1;
			m_pTrackRect->m_rc.right = r;
		}
		if (m_TrackRectZone == Rect_Top || m_TrackRectZone == Rect_TopLeft ||
			m_TrackRectZone == Rect_TopRight)
		{
			int t = m_rcTrackOrig.top+dy;
			if (t < 0) t = 0;
			if (t > min(m_rcTrackOrig.bottom-nMinRect,nDibY-1)) t = min(m_rcTrackOrig.bottom-nMinRect,nDibY-1);
			m_pTrackRect->m_rc.top = t;
		}
		if (m_TrackRectZone == Rect_Bottom || m_TrackRectZone == Rect_BottomLeft ||
			m_TrackRectZone == Rect_BottomRight)
		{
			int b = m_rcTrackOrig.bottom+dy;
			if (b < max(0,m_rcTrackOrig.top+nMinRect)) b = max(0,m_rcTrackOrig.top+nMinRect);
			m_pTrackRect->m_rc.bottom = b;
		}
		if (m_TrackRectZone == Rect_Move)
		{
			if (m_rcTrackOrig.left+dx < 0)
			{
				m_pTrackRect->m_rc.left = 0;
				m_pTrackRect->m_rc.right = m_rcTrackOrig.Width();
			}
			else if (m_rcTrackOrig.right+dx > nDibX)
			{
				m_pTrackRect->m_rc.right = nDibX;
				m_pTrackRect->m_rc.left = nDibX-m_rcTrackOrig.Width();
			}
			else
			{
				m_pTrackRect->m_rc.left = m_rcTrackOrig.left+dx;
				m_pTrackRect->m_rc.right = m_rcTrackOrig.right+dx;
			}
			if (m_rcTrackOrig.top+dy < 0)
			{
				m_pTrackRect->m_rc.top = 0;
				m_pTrackRect->m_rc.bottom = m_rcTrackOrig.Height();
			}
			else if (m_rcTrackOrig.bottom+dy > nDibY)
			{
				m_pTrackRect->m_rc.bottom = nDibY;
				m_pTrackRect->m_rc.top = nDibY-m_rcTrackOrig.Height();
			}
			else
			{
				m_pTrackRect->m_rc.top = m_rcTrackOrig.top+dy;
				m_pTrackRect->m_rc.bottom = m_rcTrackOrig.bottom+dy;
			}
		}
	}
	return S_OK;
}

HRESULT CDriverPreview::LButtonDown(UINT nFlags, POINT pt)
{
	m_bTrack = CCamValueRect::FindRect(pt, &m_TrackRectZone, &m_pTrackRect);
	if (m_bTrack)
	{
		m_ptTrackOrig = pt;
		m_rcTrackOrig = m_pTrackRect->m_rc;
	}
	return S_OK;
}

HRESULT CDriverPreview::LButtonUp(UINT nFlags, POINT pt)
{
	if (m_bTrack && m_pTrackRect)
		//ser
		m_pTrackRect->Save();
	m_bTrack = false;
	m_TrackRectZone = Rect_None;
	m_pTrackRect = NULL;
	return S_OK;
}

HRESULT CDriverPreview::RButtonDown(UINT nFlags, POINT pt)
{
	return S_OK;
}

HRESULT CDriverPreview::RButtonUp(UINT nFlags, POINT pt)
{
	return S_OK;
}

HRESULT CDriverPreview::SetCursor(UINT nFlags, POINT pt)
{
	CRectZone rz;
	CCamValueRect *p = NULL;
	bool bRect = CCamValueRect::FindRect(pt, &rz, &p);
	if (bRect)
	{
		HCURSOR hCur = m_Cursors.CursorByRectZone(rz);
		if (hCur)
		{
			::SetCursor(hCur);
			return S_OK;
		}
	}
	return S_FALSE;
}


