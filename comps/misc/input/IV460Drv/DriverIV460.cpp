// DriverIV460.cpp : implementation file
//

#include "stdafx.h"
#include "IV460Drv.h"
#include "VT5Profile.h"
#include "DriverIV460.h"
#include "PreviewDlg.h"
#include "Accumulate.h"
#include "debug.h"
#include "CnvImage.h"
#include "iv4Hlp.h"
#include "CameraDlg.h"
#include "SettingsSheet.h"
#include "FramePage.h"
#include "SourceRGBPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

CDibDraw	hdd;


/////////////////////////////////////////////////////////////////////////////
// CDriverIV460

IMPLEMENT_DYNCREATE(CDriverIV460, CDriverBase)

CDriverIV460::CDriverIV460()
{
	EnableAutomation();
	
//	_OleLockApp( this );
	m_dwFlags = FG_ACCUMULATION|FG_INPUTFRAME|FG_OVERLAYCOLORKEY;
	m_strDriverName = "IV460";
	m_strDriverFullName = "Mutech IV-460";
	m_strCategory.LoadString(IDS_INPUT);
	m_nGrabCount = 0;
	m_nBoardInit = -1;
	m_hwndOvr = NULL;
	m_bCameraChanged = FALSE;
	m_bSettingsChanged = FALSE;
}

CDriverIV460::~CDriverIV460()
{
	IV4Close();
//	_OleUnlockApp( this );
}


void CDriverIV460::OnFinalRelease()
{
	CDriverBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CDriverIV460, CDriverBase)
	//{{AFX_MSG_MAP(CDriverIV460)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDriverIV460, CDriverBase)
	//{{AFX_DISPATCH_MAP(CDriverIV460)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IDriverIV460 to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.


// {C15A244E-3145-4a51-B987-50741D649BB3}
static const IID IID_IDriverIV460 =
{ 0xc15a244e, 0x3145, 0x4a51, { 0xb9, 0x87, 0x50, 0x74, 0x1d, 0x64, 0x9b, 0xb3 } };


BEGIN_INTERFACE_MAP(CDriverIV460, CDriverBase)
	INTERFACE_PART(CDriverIV460, IID_IDriverIV460, Dispatch)
	INTERFACE_PART(CDriverIV460, IID_IInputPreview, InpPrv)
	INTERFACE_PART(CDriverIV460, IID_IInputWndOverlay, InpWOvr)
END_INTERFACE_MAP()


// {74432764-01A2-40a5-9966-5AF846B696C6}
GUARD_IMPLEMENT_OLECREATE(CDriverIV460, "IDriver.DriverIV460", 
0x74432764, 0x1a2, 0x40a5, 0x99, 0x66, 0x5a, 0xf8, 0x46, 0xb6, 0x96, 0xc6);

bool CDriverIV460::InitializeBoard()
{
	if (m_nBoardInit == -1)
	{
		m_nBoardInit = 0;
		DWORD dw = IV4Open(IV4_SDK_VERSION);
		dprintf("IV4Open - %d\n\r", dw);
		if (dw == IV4_OK)
		{
			InitializeCamera();
			m_nBoardInit = 1;
		}
	}
	return m_nBoardInit==1;
}

void CDriverIV460::InitializeCamera()
{
	CString sFileName = OnGetValueString(0, NULL, _T("CameraIniFile"), _T("rs170rgb"), ID2_STORE|ID2_IGNOREMETHODIC);
	unsigned nCamera = OnGetValueInt(0, NULL, _T("Camera"), 0, ID2_STORE|ID2_IGNOREMETHODIC);
	IV4HlpInitializeCamera(nCamera, sFileName, &m_camcfg);
	if (m_bCameraChanged)
	{
		ResetSettingsToDefault();
		m_bCameraChanged = FALSE;
	}
	IV4HlpPostInitCamera(m_camcfg, &m_xDrv);
//	IV4SetPixelMode(IV4_24bit_Mode);
}

LPBITMAPINFOHEADER CDriverIV460::GrabImage(BOOL bConvert, BOOL bGrayScale)
{
	int r;
	if (m_nGrabCount == 0)
	{
		r = IV4StartGrab(IV4_Single_Grab);
		if (r != IV4_OK)
			return NULL;
	}
	/*UWORD Mode, SX, SY, DX, DY;
	r = IV4GetGrabWindow(&Mode, &SX, &SY, &DX, &DY);
	if (r != IV4_OK) return NULL;*/
	short DX,DY;
	OnGetSize(DX,DY);
	BYTE nPixMode;
	r = IV4GetPixelMode(&nPixMode);
	if (r != IV4_OK) return NULL;
	int nBPPSrc = nPixMode==IV4_8bit_Mode?1:nPixMode==IV4_16bit_Mode?2:nPixMode==IV4_24bit_Mode?3:4;
	int nBPPDst = bConvert?bGrayScale?1:3:nBPPSrc;
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)malloc(sizeof(BITMAPINFOHEADER)+256*sizeof(RGBQUAD)+DX*DY*nBPPDst);
	memset(lpbi, 0, sizeof(BITMAPINFOHEADER));
	lpbi->biSize = sizeof(BITMAPINFOHEADER);
	lpbi->biWidth = DX;
	lpbi->biHeight = DY;
	lpbi->biBitCount = 8*nBPPDst;
	lpbi->biPlanes = 1;
	lpbi->biCompression = BI_RGB;
	lpbi->biClrImportant = lpbi->biClrUsed = lpbi->biBitCount==8?256:0;
	LPBYTE lpData = (LPBYTE)(lpbi+1);
	if (lpbi->biClrUsed)
	{
		RGBQUAD *prgb = (RGBQUAD *)(lpbi+1);
		for (int i = 0; i < lpbi->biClrUsed; i++)
			prgb[i].rgbBlue = prgb[i].rgbGreen = prgb[i].rgbRed = i;
		lpData = (LPBYTE)(prgb+lpbi->biClrUsed);
	}
	lpData += nBPPDst*DX*(DY-1);
	if (bConvert)
	{
		CNVFUNC CnvFunc = g_CnvFuncs[nBPPSrc-1][bGrayScale?0:1];
		LPBYTE lpBuff = (LPBYTE)malloc(nBPPSrc*DX);
		for (int i = 0; i < DY; i++,lpData -= nBPPDst*DX)
		{
			IV4RdPixelLine(0, i, DX, lpBuff);
			CnvFunc(lpData, lpBuff, DX, nBPPSrc);
		}
		free(lpBuff);
	}
	else
	{
		for (int i = 0; i < DY; i++,lpData -= nBPPDst*DX)
			IV4RdPixelLine(0, i, DX, lpData);
	}
	return lpbi;
}

void CDriverIV460::ResetSettingsToDefault()
{
	OnSetValueInt(0, _T("Settings"), _T("EnableMargins"), 0, 0);
	/*pDrv.SetInt(0, _T("Settings"), _T("LeftMargin"), 0, 0);
	pDrv.SetInt(0, _T("Settings"), _T("TopMargin"), 0, 0);
	pDrv.SetInt(0, _T("Settings"), _T("RightMargin"), 0, 0);
	pDrv.SetInt(0, _T("Settings"), _T("BottomMargin"), 0, 0);
	pDrv.SetInt(0, _T("Source"), _T("HScale"), 1, ID2_STORE);
	pDrv.SetInt(0, _T("Source"), _T("VScale"), 1, ID2_STORE);*/
	OnSetValueInt(0, _T("Source"), _T("Channel"), m_camcfg.Video_Channel, ID2_STORE);
}

bool CDriverIV460::OnBeginPreview(IInputPreviewSite *pSite)
{
	if (!InitializeBoard())
		return false;
	if (m_nGrabCount++ == 0)
	{
		DWORD dw = IV4StartGrab(IV4_Continue_Grab); 
		dprintf("IV4StartGrab in CDriverIV460::OnBeginPreview: %d\n", dw);
	}
	return CInputPreviewImpl::OnBeginPreview(pSite);
}

void CDriverIV460::OnEndPreview(IInputPreviewSite *pSite)
{
	if (--m_nGrabCount <= 0)
		IV4StopGrab();
	CInputPreviewImpl::OnEndPreview(pSite);
}

bool CDriverIV460::OnInitOverlay()
{
	return InitializeBoard();
}

void CDriverIV460::OnDeinitOverlay()
{
}

void CDriverIV460::OnGetOvrSize(short &cx, short &cy)
{
	OnGetSize(cx,cy);
}

bool CDriverIV460::OnStartOverlay(HWND hwndDraw, LPRECT lprcSrc, LPRECT lprcDst)
{
	CRect rcDst(*lprcDst);
	if (hwndDraw) CWnd::FromHandle(hwndDraw)->ClientToScreen(rcDst);
	int r = IV4SetImageFrameOnVideoFrame(lprcSrc->left, lprcSrc->top,
		lprcSrc->right-lprcSrc->left, lprcSrc->bottom-lprcSrc->top);
	dprintf("IV4SetImageFrameOnVideoFrame on CDriverIV460::OnStartOverlay : %d\n", r);
	if (!g_IV460Profile.GetProfileInt(_T("PreviewDlg"), _T("NaturalSize"), FALSE, true))
	{
		r = IV4SetZoomFactorsEx(lprcSrc->right-lprcSrc->left, rcDst.right-rcDst.left,
			lprcSrc->bottom-lprcSrc->top, rcDst.bottom-rcDst.top);
		dprintf("IV4SetZoomFactorsEx on CDriverIV460::OnStartOverlay : %d\n", r);
	}
	r = IV4SetImageWndOnScreen(rcDst.left, rcDst.top);	
	dprintf("IV4SetImageWndOnScreen on CDriverIV460::OnStartOverlay : %d\n", r);
	r = IV4SetRGBColorKey(
		CStdProfileManager::m_pMgr->GetProfileInt(_T("Overlay"), _T("ColorKeyRed"), 0, true, true),
		CStdProfileManager::m_pMgr->GetProfileInt(_T("Overlay"), _T("ColorKeyGreen"), 0, true, true),
		CStdProfileManager::m_pMgr->GetProfileInt(_T("Overlay"), _T("ColorKeyBlue"), 0, true, true));
	dprintf("IV4SetRGBColorKey in OnInitOverlay : %d\n", r);
	r = IV4SetOverlayMode(IV4_Color_Key_Mode); // turn on the overlay
	dprintf("IV4SetOverlayMode in OnInitOverlay : %d\n", r);
	if (m_nGrabCount++ == 0)
	{
		r = IV4StartGrab(IV4_Continue_Grab); 
		dprintf("IV4StartGrab in CDriverIV460::OnStartOverlay: %d\n", r);
	}
	m_hwndOvr = hwndDraw;
	m_rcOvrSrc = *lprcSrc;
	m_rcOvrDst = *lprcDst;
	return true;
}

void CDriverIV460::OnStopOverlay(HWND hwndDraw)
{
	if (--m_nGrabCount <= 0)
		IV4StopGrab();
	int r = IV4SetOverlayMode(IV4_No_Overlay_Mode); // turn on the overlay
	m_hwndOvr = NULL;
	m_rcOvrSrc = CRect(0,0,0,0);
	m_rcOvrDst = CRect(0,0,0,0);
	dprintf("IV4SetOverlayMode in OnStopOverlay : %d\n", r);
}

void CDriverIV460::OnSetDstRect(HWND hwndDraw, LPRECT lprcDst)
{
	CRect rc(*lprcDst);
	CWnd::FromHandle(hwndDraw)->ClientToScreen(rc);
	int r = IV4SetImageWndOnScreen(rc.left, rc.top);	
	dprintf("IV4SetImageWndOnScreen on CDriverIV460::OnSetDstRect : %d\n", r);
	UWORD sx,sy,dx,dy;
	r = IV4GetImageFrameOnVideoFrame(&sx,&sy,&dx,&dy);
	dprintf("IV4GetImageFrameOnVideoFrame on CDriverIV460::OnSetDstRect : r=%d,dx=%d,dy=%d\n",
		r, (int)dx, (int)dy);
	r = IV4SetZoomFactorsEx(dx, rc.right-rc.left, dy, rc.bottom-rc.top);
	dprintf("IV4SetZoomFactorsEx on CDriverIV460::OnStartOverlay : %d\n", r);
	m_hwndOvr = hwndDraw;
	m_rcOvrDst = *lprcDst;
}

void CDriverIV460::OnSetSrcRect(HWND hwndDraw, LPRECT lprcSrc)
{
	int r = IV4SetImageFrameOnVideoFrame(lprcSrc->left, lprcSrc->top,
		lprcSrc->right-lprcSrc->left+1, lprcSrc->bottom-lprcSrc->top+1);
	dprintf("IV4SetImageFrameOnVideoFrame on CDriverIV460::OnSetSrcRect : %d\n", r);
	m_rcOvrSrc = *lprcSrc;
}

/*
void CDriverIV460::OnInputImage(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	LPBITMAPINFOHEADER lpbi;
	InitializeBoard();
	CImageWrp img(pUnkImg);
	BOOL bGrayScale = g_IV460Profile.GetProfileInt(_T("Format"), _T("GrayScale"), FALSE, true);
	BOOL bAccum = g_IV460Profile.GetProfileInt(_T("Settings"), _T("Accumulate"), FALSE);
	if (bAccum)
	{
		int nImagesNum = g_IV460Profile.GetProfileInt(_T("Settings"), _T("AccumulateImagesNum"), FALSE);
		CAccumulate Accum;
		for (int i = 0; i < nImagesNum; i++)
		{
			lpbi = GrabImage(TRUE,bGrayScale);
			Accum.Add(lpbi);
			free(lpbi);
		}
		unsigned nSize = Accum.CalcSize();
		lpbi = (LPBITMAPINFOHEADER)malloc(nSize);
		Accum.GetResult(lpbi, true);
	}
	else
		lpbi = GrabImage(TRUE,bGrayScale);
	if (lpbi)
	{
		LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
		img.AttachDIBBits(lpbi, pData);
		free(lpbi);
	}
}*/

void CDriverIV460::OnInputImage(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bDialog)
{
	CImageWrp img(pUnkImg);
	LPBITMAPINFOHEADER lpbi = OnInputImage(FALSE);
	if (lpbi)
	{
		LPBYTE pData = (LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed);
		img.AttachDIBBits(lpbi, pData);
		free(lpbi);
	}
}


LPBITMAPINFOHEADER CDriverIV460::OnInputImage(BOOL bUseSettings)
{
	LPBITMAPINFOHEADER lpbi;
	InitializeBoard();
	BOOL bGrayScale = g_IV460Profile.GetProfileInt(_T("Format"), _T("GrayScale"), FALSE, true);
	BOOL bAccum = g_IV460Profile.GetProfileInt(_T("Settings"), _T("Accumulate"), FALSE);
	if (bAccum)
	{
		int nImagesNum = g_IV460Profile.GetProfileInt(_T("Settings"), _T("AccumulateImagesNum"), FALSE);
		CAccumulate Accum;
		for (int i = 0; i < nImagesNum; i++)
		{
			lpbi = GrabImage(TRUE,bGrayScale);
			Accum.Add(lpbi);
			free(lpbi);
		}
		unsigned nSize = Accum.CalcSize();
		lpbi = (LPBITMAPINFOHEADER)malloc(nSize);
		Accum.GetResult(lpbi, true);
	}
	else
		lpbi = GrabImage(TRUE,bGrayScale);
	return lpbi;
}


bool CDriverIV460::OnInputVideoFile(CString &sVideoFileName, int nDevice, int nMode)
{
	return false;
}

bool CDriverIV460::OnExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput)
{
	if (nMode == 0) return true;
	CPreviewDlg dlg(&m_xInpPrv);
	dlg.m_pDialogSite = this;
	int r = dlg.DoModal();
	return r == IDOK;
}

void CDriverIV460::OnGetSize(short &cx, short &cy)
{
	if (m_nBoardInit == 1)
	{
		/*UWORD Mode, SX, SY, DX, DY;
		DWORD dw = IV4GetGrabWindow(&Mode, &SX, &SY, &DX, &DY);
		dprintf("IV4GetGrabWindow : %d, Mode==%d, SX==%d, SY==%d, DX==%d, DY==%d in CDriverIV460::OnGetSize\n",
			dw, (int)Mode, (int)SX, (int)SY, (int)DX, (int)DY);
		cx = (short)DX;
		cy = (short)DY;*/
		UWORD dx,dy;
		int r = IV4GetVideoFrame(NULL, NULL, &dx, &dy);
		if (r == IV4_OK)
		{
			cx = (short)dx;
			cy = (short)dy;
		}
		else
		{
			cx = 768;
			cy = 576;
			dprintf("IV4GetVideoFrame in CDriverIV460::OnGetSize : %r\n", r);
		}
	}
	else
	{
		dprintf("CDriverIV460::OnGetSize called then CDriverIV460::m_nBoardInit==%d\n", m_nBoardInit);
		cx = 768;
		cy = 576;
	}
}

void CDriverIV460::OnGetPreviewFreq(DWORD &dwFreq)
{
	dwFreq = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("RedrawFrequency"), 30, true);
}

void CDriverIV460::OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst)
{
	if (m_nGrabCount)
	{
		BOOL bGrayScale = g_IV460Profile.GetProfileInt(_T("Format"), _T("GrayScale"), FALSE, true);
		LPBITMAPINFOHEADER lpbi = GrabImage(TRUE, bGrayScale);
		if (lpbi)
		{
			BOOL bVertMirror = 0;
			LPBYTE lpData = ((LPBYTE)(((RGBQUAD *)(lpbi+1))+lpbi->biClrUsed));
			CSize sz(lpbi->biWidth,lpbi->biHeight);
			CRect rcSrc(*lpRectSrc);
			if (rcSrc == CRect(0,0,0,0)) rcSrc = CRect(0,0,sz.cx,sz.cy);
			CRect rcDst(*lpRectDst);
			bool bStretch = rcSrc.Width() != rcDst.Width() || rcSrc.Height() != rcDst.Height();
			if (rcSrc.Width() > sz.cx)
			{
				rcDst.right = rcDst.left + rcDst.Width()*sz.cx/rcSrc.Width();
				rcSrc.right = rcSrc.left + sz.cx;
			}
			if (rcSrc.Height() > sz.cx)
			{
				rcDst.bottom = rcDst.top + rcDst.Height()*sz.cy/rcSrc.Height();
				rcSrc.bottom = sz.cy;
			}
			if (bStretch && bVertMirror)
			{
				 CRect rc(rcSrc.left, sz.cy-rcSrc.bottom, rcSrc.right, sz.cy-rcSrc.top);
				 rcSrc = rc;
			}
			if (bStretch)
				if (0/*bVertMirror*/)
					StretchDIBits(hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(), rcSrc.left, rcSrc.top,
						rcSrc.Width(), rcSrc.Height(), lpData, (BITMAPINFO *)lpbi, DIB_RGB_COLORS, SRCCOPY);
				else
					DrawDibDraw(hdd.hdd, hDC, lpRectDst->left, lpRectDst->top, rcDst.Width(), rcDst.Height(), lpbi,
						lpData, rcSrc.left, rcSrc.top, rcSrc.Width(), rcSrc.Height(), 0);
			else
				SetDIBitsToDevice(hDC, lpRectDst->left-rcSrc.left, lpRectDst->top-rcSrc.top, sz.cx, sz.cy, 0,
					0, 0, sz.cy, lpData, (BITMAPINFO *)lpbi, DIB_RGB_COLORS);
			free(lpbi);
			return;
		}
	}
	HBRUSH br = ::CreateSolidBrush(RGB(0,0,0));
	FillRect(hDC, lpRectDst, br);
	DeleteObject(br);
	// Do not call CStatic::OnPaint() for painting messages
}

static void _SectionAndEntry(LPCTSTR lpName, CString &sSec, CString &sEntry)
{
	CString s(lpName);
	int n = s.Find("\\/");
	if (n == -1)
	{
		sSec = "Settings";
		sEntry = s;
	}
	else
	{
		sSec = s.Left(n);
		sEntry = s.Mid(n+1);
	}
}

void CDriverIV460::OnSetValue(int nDev, LPCTSTR lpName, VARIANT var)
{
	CString sSec,sEntry;
	_SectionAndEntry(lpName, sSec, sEntry);
	g_IV460Profile.WriteProfileValue(sSec, sEntry, var);
	/*if (var.vt == VT_BSTR)
	{
		CString s(var.bstrVal);
		CIV460Grab::GetGrabber()->OnSetValueString(sSec, sEntry, s, m_arrSites);
	}
	else if (var.vt == VT_I2)
	{
		int nValue = var.iVal;
		CIV460Grab::GetGrabber()->OnSetValueInt(sSec, sEntry, nValue, m_arrSites);
	}
	else if (var.vt == VT_I4)
	{
		int nValue = var.lVal;
		CIV460Grab::GetGrabber()->OnSetValueInt(sSec, sEntry, nValue, m_arrSites);
	}*/
}

VARIANT CDriverIV460::OnGetValue(int nDev, LPCTSTR lpName)
{
	CString sSec,sEntry;
	_SectionAndEntry(lpName, sSec, sEntry);
	return g_IV460Profile.GetProfileValue(sSec, sEntry);
}

void CDriverIV460::GetButtonInfo(int nButton, CButtonInfo *pButtonInfo)
{
	pButtonInfo->m_szButtonName[0] = 0;
	if (nButton == 0)
		pButtonInfo->m_nButtonId = IDC_CAMERA;
	else
		pButtonInfo->m_nButtonId = IDC_SETTINGS;
}

void CDriverIV460::ButtonPress(int nButtonId, ISettingsSite *pSettingsSite, IRedrawHook **ppRedrawHook)
{
	OnBeginPreviewMode();
	if (nButtonId == IDC_CAMERA)
	{
		CCameraDlg dlg;
		dlg.m_pDrv.Attach(&m_xDrv);
		dlg.DoModal();
	}
	else if (nButtonId == IDC_SETTINGS)
	{
		try 
		{
		CSettingsSheet sheet(m_strDriverFullName);
		CFramePage FramePage;
		CSourceRGBPage SourceRGBPage(&m_xDrv);
		FramePage.m_pDrv.Attach(&m_xDrv);
		sheet.AddPage(&FramePage);
		sheet.AddPage(&SourceRGBPage);
		sheet.DoModal();
		}catch(...)
		{
		}
	}
	OnEndPreviewMode();
}

void CDriverIV460::OnUpdateSizes()
{
	for (int i = 0; i < m_arrSites.GetSize(); i++)
		m_arrSites[i]->OnChangeSize();
	if (m_hwndOvr)
		::SendMessage(m_hwndOvr, WM_USER+10, 2, 0);
}

void CDriverIV460::ReinitPreview()
{
	if (!m_bSettingsChanged) return;
	UWORD w0,w1,h0,h1;
	ULONG nOvrMode;
	HWND hwndOvr;
	CRect rcOvrDst;
	int r = IV4GetOverlayMode(&nOvrMode);
	if (r == IV4_OK && nOvrMode != IV4_No_Overlay_Mode)
	{
		hwndOvr = m_hwndOvr;
		rcOvrDst = m_rcOvrDst;
		OnStopOverlay(NULL);
	}
	else if (m_nGrabCount)
	{
		IV4GetZoomFactorsEx(&w0,&w1,&h0,&h1);
		IV4StopGrab();
	}
	InitializeCamera();
	if (r == IV4_OK && nOvrMode != IV4_No_Overlay_Mode)
	{
		short dx,dy;
		OnGetSize(dx,dy);
		CRect rcOvrSrc(0,0,dx,dy);
		OnStartOverlay(hwndOvr, rcOvrSrc, rcOvrDst);
	}
	else if (m_nGrabCount)
	{
		IV4SetZoomFactorsEx(w0,w1,h0,h1);
		IV4StartGrab(IV4_Continue_Grab);
	}
	m_bSettingsChanged = FALSE;
}

void CDriverIV460::OnSetValueIntPrv(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, int nValue)
{
	if (!strSection)
	{
		if (!_tcscmp(_T("CameraIniFile"), strEntry) || !_tcscmp(_T("Camera"), strEntry))
			m_bCameraChanged = TRUE;
	}
	if (!_tcscmp(strSection, _T("Source")))
	{
		if (!_tcscmp(strEntry, _T("OffsetRed")))
		{
			if (m_camcfg.RGB_Flag != IV4_USE_TOP_BOTTOM)
				IV4SetOffsetEx(IV4_Channel_Current, IV4_Select_Red, (BYTE)nValue);
			return;
		}
		else if (!_tcscmp(strEntry, _T("OffsetGreen")))
		{
			if (m_camcfg.RGB_Flag != IV4_USE_TOP_BOTTOM)
				IV4SetOffsetEx(IV4_Channel_Current, IV4_Select_Green, (BYTE)nValue);
			return;
		}
		else if (!_tcscmp(strEntry, _T("OffsetBlue")))
		{
			if (m_camcfg.RGB_Flag != IV4_USE_TOP_BOTTOM)
				IV4SetOffsetEx(IV4_Channel_Current, IV4_Select_Blue, (BYTE)nValue);
			return;
		}
	}
	m_bSettingsChanged = TRUE;
}

void CDriverIV460::OnSetValueStringPrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, LPCTSTR sValue)
{
	m_bSettingsChanged = TRUE;
}

void CDriverIV460::OnSetValueDoublePrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, double dValue)
{
	if (!_tcscmp(strSection, _T("Source")))
	{
		if (!_tcscmp(sEntry, _T("GainRed")))
		{
			if (m_camcfg.RGB_Flag != IV4_USE_TOP_BOTTOM)
				IV4SetGainEx(IV4_Channel_Current, IV4_Select_Red, (float)dValue);
			return;
		}
		else if (!_tcscmp(sEntry, _T("GainGreen")))
		{
			if (m_camcfg.RGB_Flag != IV4_USE_TOP_BOTTOM)
				IV4SetGainEx(IV4_Channel_Current, IV4_Select_Green, (float)dValue);
			return;
		}
		else if (!_tcscmp(sEntry, _T("GainBlue")))
		{
			if (m_camcfg.RGB_Flag != IV4_USE_TOP_BOTTOM)
				IV4SetGainEx(IV4_Channel_Current, IV4_Select_Blue, (float)dValue);
			return;
		}
	}
	m_bSettingsChanged = TRUE;
}


int CDriverIV460::OnGetBoardInt(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nDef)
{
	if (!_tcscmp(_T("Settings"), sSection))
	{
		if (!_tcscmp(_T("MaxMarginX"), sEntry))
			return 100;
		else if (!_tcscmp(_T("MaxMarginY"), sEntry))
			return 100;
		else if (!_tcscmp(_T("RGBUseTopBottom"), sEntry))
			return m_camcfg.RGB_Flag==IV4_USE_GAIN_OFFSET;
	}
	return 0;
}


CString CDriverIV460::OnGetBoardString(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sValue)
{
	return CString(_T(""));
}

double CDriverIV460::OnGetBoardDouble(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue)
{
	return 0.;
}

void CDriverIV460::OnCancelChanges()
{
	if (m_EntryArray.GetSize())
		m_bSettingsChanged = TRUE;
	CDriverBase::OnCancelChanges();
}


/////////////////////////////////////////////////////////////////////////////
// CDriverIV460 message handlers
