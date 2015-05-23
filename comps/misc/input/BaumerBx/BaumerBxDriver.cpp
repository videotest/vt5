#include "StdAfx.h"
#include "resource.h"
#include "BaumerBxDriver.h"
#include "BxLibApiWrapper.h"
#include "VT5Profile.h"
#include "PreviewPixDlg.h"
#include "CamValues.h"
#include "MethCombo.h"
#include "Focus.h"
#include "SettingsBase.h"
#include "MethNameDlg.h"
#include "debug.h"
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct CBaumerBxData
{
	ptDefineDigitalCamera m_pCam1;
	tGAIN m_Gain;
	tSHT  m_Sht;
	tBLOFFSET m_BLOffset;
	tFormat m_DMAFmt;
	int m_nExtImFormat,m_nExtImFormatAdd;
	bool m_bBinning;
	tBxCameraType m_Type;
	tBxCameraStatus m_Status;
	ptDefineImageFormat m_pFmts;
	int m_nFmts;
	CBaumerBxData()
	{
		m_pCam1 = NULL;
		m_nExtImFormat = CF_STANDARD;
		m_nExtImFormatAdd = 1;
		m_bBinning = false;
		memset(&m_Type, 0, sizeof(m_Type));
		memset(&m_Status, 0, sizeof(m_Status));
		m_pFmts = NULL;
		m_nFmts = 0;
	}
};

static void _SetGamma(int nDevice, double dGamma)
{
	WORD awRed[4096],awGreen[4096],awBlue[4096];
	bool bEnable = dGamma==1.?false:true;
	for (int i = 0; i < 4096; i++)
		awRed[i] = awGreen[i] = awBlue[i] = 0;
	for (int i = 0; i < 4096; i++)
	{
		int v = (int)((double)4096*pow(double(i)/4096,dGamma));
		awRed[i] = awGreen[i] = awBlue[i] = v;
	}
	wrapBX_SetGammaLut(nDevice, bEnable, awRed, awGreen, awBlue);
}

CCamIntValue2 g_Exposure(_T("Parameters"), _T("Exposure"), 1000, 1, 10000000, 1000, 1, 0, IDC_EDIT_EXPOSURE);
CCamDoubleValue    g_Gain(_T("Parameters"), _T("Gain"), 1., 0.000001, 1000000.,
	IRepr_LogDoubleEdit, 0, IDC_EDIT_GAIN);
CCamIntValue  g_Offset(_T("Parameters"), _T("Offset"), 1, INT_MIN, INT_MAX, IRepr_SmartIntEdit,
	0, IDC_EDIT_OFFSET);
CCamDoubleValue    g_HGamma(_T("Parameters"), _T("HGamma"), 1., 0.1, 10., IRepr_LogDoubleEdit,
	0, IDC_EDIT_ENH_GAMMA);
INTCOMBOITEMDESCR s_aModes[] =
{
	{0, _T("1:1")},
	{1, _T("1:2")},
	{2, _T("1:4")},
};
CCamIntComboValue  g_BinningMode(_T("Parameters"), _T("Binning"), s_aModes, 2,
	0, 0, IDC_COMBO_CAPTURE);
CCamIntComboValue2  g_CaptureMode(_T("Parameters"), _T("CaptureMode"), 0, 0, IDC_CAPTURE_EX,
	CCamValue::WriteToReg|CCamValue::SkipReinitCamera);
CCamBoolValue g_AutoExposure(_T("Parameters"), _T("AutoExpositure"), false, 0, IDC_CHECK_AUTOEXPOSITION);
CCamIntValue  g_AutoExposureLevel(_T("Parameters"), _T("AutoExposureLevel"), 200, 0, 255);

CCamDoubleValue    g_ColorBalanceRed(_T("Color balance"), _T("Red"), 1., .1, 10., IRepr_LogDoubleEdit,
	0, IDC_EDIT_WB_RED);
CCamDoubleValue    g_ColorBalanceGreen(_T("Color balance"), _T("Green"), 1., .1, 10., IRepr_LogDoubleEdit,
	0, IDC_EDIT_WB_GREEN);
CCamDoubleValue    g_ColorBalanceBlue(_T("Color balance"), _T("Blue"), 1., .1, 10., IRepr_LogDoubleEdit,
	0, IDC_EDIT_WB_BLUE);
CCamIntValue       g_ColorBalanceAuto(_T("Color balance"), _T("Auto"), 1, 0, 1, IRepr_Radiobuttons,
	0, IDC_WB_AUTO);

CCamIntValue       g_GrayFromRGB(_T("Image"), _T("GrayFromRGB"), 0, 0, 1);


static bool s_bSizesChanged = false;


CBaumerBxDriver::CBaumerBxDriver()
{
	m_nDevices = 0;
	m_pData = new CBaumerBxData;
	m_nDevice = 0;
	g_VertMirror.SetId(IDC_CHECK_FLIP_VERT);
	g_HorzMirror.SetId(IDC_CHECK_FLIP_HORZ);
	g_Gray.SetId(IDC_CHECK_GRAYSCALE);
	g_OUExpEnabled.SetId(IDC_CHECK_OVER_UNDER_EXPOSURE);
	g_Methodics.SetId(IDC_LIST_SETTINGS);
	m_nIdNewMeth = IDC_BUTTON_NEW_SETTINGS;
	m_nIdDeleteMeth = IDC_BUTTON_DELETE_SETTINGS;
	g_Focus.SetId(IDC_CHECK_FOCUS);
	g_FocusRect.SetId(IDC_SHOW_FOCUS_RECT);
	g_FocusValue.SetId(IDC_STATIC_FOCUS_CURRENT);
	g_MaxFocus.SetId(IDC_STATIC_FOCUS_PEAK);
	m_nIdStaticFocusMax = IDC_STATIC_FOCUS_PEAK1;
	m_nIdStaticFocusCur = IDC_STATIC_FOCUS_CURRENT1;
	CMethNameDlg::s_idd = IDD_DIALOG_SETTING_NAME;
	CMethNameDlg::s_idEdit = IDC_EDIT1;
	CMethodicsComboValue::s_idsPredefined = IDS_PREDEFINED_METHODIC;
	CBaseDlg::m_idDefault = IDC_DEFAULT;
}

CBaumerBxDriver::~CBaumerBxDriver()
{
	delete m_pData;
}

bool CBaumerBxDriver::InitCamera()
{
	if (!AttachBxLib())
	{
		AfxMessageBox(IDS_LIB_NOT_FOUND, MB_OK|MB_ICONEXCLAMATION);
		return false;
	}
	do
	{
		wrapBX_InitCamSys();
		wrapBX_EnumerateCamSystem(&m_nDevices, &m_pData->m_pCam1);
		if (m_nDevices > 0)
		{
			m_nDevice = CStdProfileManager::m_pMgr->GetProfileInt(_T("Camera"), _T("UsedCamera"), 0, true, true);
			if (m_nDevice >= m_nDevices)
				m_nDevice = m_nDevices-1;
			if (wrapBX_InitCamByNr(m_nDevice) > 0)
			{
				wrapBX_GetCameraInfo(m_nDevice, &m_pData->m_Type, sizeof(m_pData->m_Type), &m_pData->m_Status, sizeof(m_pData->m_Status));
				AnalyzeSnapshotFormats();
				_SetTimer(200);
				return true;
			}
		}
		if (!wrapBX_CameraSystemManager(NULL))
		{
			AfxMessageBox(IDS_INVALID_CAMERA, MB_OK|MB_ICONEXCLAMATION);
			return false;
		}
	}
	while(1);
	return true;
}

void CBaumerBxDriver::DeinitCamera()
{
//	wrapBX_StopScanMode(m_pData->m_pCam1[m_nDevice].iFgNr);
	_KillTimer();
	DetachBxLib();
}

void CBaumerBxDriver::AnalyzeSnapshotFormats()
{
	int nFmts = 0;
	ptDefineImageFormat pFmts = NULL;
	g_CaptureMode.Add(0, 0, IDS_STANDARD);
	if (!CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("DisableCaptureModes"), FALSE, true, true))
	{
		if (wrapBX_EnumerateBmpImFormats(m_nDevice, &nFmts, &pFmts))
		{
			for (int i = 0; i < nFmts; i++)
			{
				CString s,s1;
				s.Format(IDS_MODE_DESCRIPTION, pFmts[i].iWidth, pFmts[i].iHeight);
				if (pFmts[i].support.bColor)
					s1.LoadString(IDS_COLOR);
				else
					s1.LoadString(IDS_BW);
				s += s1;
				if (pFmts[i].support.b16Bit)
				{
					s1.LoadString(IDS_16BIT);
					s += s1;
				}
				g_CaptureMode.Add(i+1, s,-1);
			}
			m_pData->m_pFmts = pFmts;
			m_pData->m_nFmts = nFmts;
		}
	}
	g_CaptureMode._Init(true);
}

void CBaumerBxDriver::SetBinning(int nMode)
{
	tCmdDesc CmdExt;
	CmdExt.num = 1;
	CmdExt.cmd[0].cmdId = CC_SETBINNINGSIZE;
	if (nMode == 1)
		CmdExt.cmd[0].cmdPa.AsBinningSize.value = BIN2X2;
	else if (nMode == 2)
		CmdExt.cmd[0].cmdPa.AsBinningSize.value = BIN4X4;
	CmdExt.cmd[0].cmdPa.AsBinningSize.ret = 0;
	if (nMode == 0)
		wrapBX_SetBinningMode(m_nDevice, FALSE, NULL);
	else
		wrapBX_SetBinningMode(m_nDevice, TRUE, &CmdExt);
	if (nMode == 1 || nMode == 2)
	{
		CmdExt.cmd[0].cmdId = CC_CORBINNING;
		CmdExt.cmd[0].cmdPa.AsBinningSize.value = 1;
		CmdExt.cmd[0].cmdPa.AsBinningSize.ret = 0;
		wrapBX_SetBinningMode(m_nDevice, TRUE, &CmdExt);
	}
	m_pData->m_bBinning = nMode >= 1;
}

void CBaumerBxDriver::SetWhiteBalance()
{
	if (m_pData->m_Type.bColor)
	{
		double r = g_ColorBalanceRed;
		double g = g_ColorBalanceGreen;
		double b = g_ColorBalanceBlue;
		wrapBX_SetWhiteBalance(m_nDevice, TRUE, TRUE, &r, &g, &b);
	}
}

HRESULT CBaumerBxDriver::GetSize(int nDev, short *pdx, short *pdy)
{
	__super::GetSize(nDev, pdx, pdy);
	if (m_CamState != Succeeded)
		return S_OK;
	int nCamId = m_pData->m_pCam1[m_nDevice].iCamId;
	if (nCamId == DC300 || nCamId == ARC6000C)
	{
		if (m_pData->m_nExtImFormat == CF_STANDARD ||
			m_pData->m_nExtImFormat == CF_C32FRAMEA ||
			m_pData->m_nExtImFormat == CF_C32FRAMEB)
		{
			*pdx *= 2;
			*pdy *= 2;
		}
	}
	return S_OK;
}

bool CBaumerBxDriver::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
	BOOL bFirst, BOOL bForInput)
{
	CPreviewIds PreviewIds = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG, IDC_STATIC_PERIOD}, 
		IDC_STATIC_PERIOD, IDC_IMAGE, IDC_CAMERA_PAGE, IDC_SETTINGS_PAGE, -1, -1, IDC_HISTOGRAM,
		IDS_FPS, IDS_FRAME_PERIOD};
	CPreviewPixDlg  dlg(IDD_PREVIEW,PreviewIds,(IInputPreview *)this,nDevice,0,CWnd::FromHandle(hwndParent));
	dlg.AttachChildDialogToButton(IDC_CAMERA_PAGE, IDD_CAMERA);
	dlg.AttachChildDialogToButton(IDC_IMAGE_PAGE, IDD_IMAGE);
	dlg.AttachChildDialogToButton(IDC_COLOR_BALANCE, IDD_COLOR_BALANCE);
	dlg.AttachChildDialogToButton(IDC_SETTINGS_PAGE, IDD_SETTINGS);
	return dlg.DoModal() == IDOK;
}

HRESULT CBaumerBxDriver::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CSettingsIds Ids = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG, IDC_STATIC_PERIOD},
		IDC_TAB_SELECT, 0, IDS_FPS, IDS_FRAME_PERIOD, IDC_HISTOGRAM};
	CSettingsBase dlg((IDriver*)this, 0, IDD_DIALOG_SETTINGS, Ids, NULL);
	dlg.AddPane(IDS_CAMERA_PAGE, IDD_CAMERA_S);
	dlg.AddPane(IDS_IMAGE, IDD_IMAGE);
	if (m_pData->m_Type.bColor)
		dlg.AddPane(IDS_COLOR_BALANCE, IDD_COLOR_BALANCE);
	dlg.AddPane(IDS_SETTINGS, IDD_SETTINGS);
	dlg.DoModal();
	return S_OK;
}

bool CBaumerBxDriver::GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi)
{
	::InitDIB(lpbi, m_pData->m_DMAFmt.dmaImFrameXMax, m_pData->m_DMAFmt.dmaImFrameYMax,
		8, false);
	return true;
}

bool CBaumerBxDriver::PreinitGrab(bool bCapture)
{
	if (m_bSettingsChanged)
	{
		SetBinning((int)g_BinningMode);
		wrapBX_SetTriggerMode(m_nDevice, FALSE, NULL);
		wrapBX_SetDataWidth(m_nDevice, W8BIT);
		wrapBX_SetGainFactor(m_nDevice, 0.0, &m_pData->m_Gain);
		g_Gain.InitScope(m_pData->m_Gain.gainMin, m_pData->m_Gain.gainMax, m_pData->m_Gain.gainMin);
		wrapBX_SetGainFactor(m_nDevice, (int)g_Gain, &m_pData->m_Gain);
		wrapBX_SetShutterTime(m_nDevice, -1, &m_pData->m_Sht);
		g_Exposure.InitScope(m_pData->m_Sht.timeMin, m_pData->m_Sht.timeMax,
			min(max(100000,m_pData->m_Sht.timeMin), m_pData->m_Sht.timeMax));
		wrapBX_SetAutoExposure(m_nDevice, (int)g_AutoExposure, (int)g_AutoExposureLevel);
		wrapBX_SetShutterTime(m_nDevice, (int)g_Exposure, &m_pData->m_Sht);
		wrapBX_SetBlackOffset(m_nDevice, -1, &m_pData->m_BLOffset);
		_SetGamma(m_nDevice, (double)g_HGamma);
		g_Offset.InitScope(m_pData->m_BLOffset.offsetMin, m_pData->m_BLOffset.offsetMax,
			m_pData->m_BLOffset.offsetMin);
		wrapBX_SetBlackOffset(m_nDevice, (int)g_Offset, NULL);
		m_pData->m_DMAFmt.iSizeof = sizeof(m_pData->m_DMAFmt);
		wrapBX_DmaImFormat(m_nDevice, &m_pData->m_DMAFmt);
		wrapBX_GetCameraInfo(m_nDevice, &m_pData->m_Type, sizeof(m_pData->m_Type), &m_pData->m_Status, sizeof(m_pData->m_Status));
		SetWhiteBalance();
	}
	return true;
}

bool CBaumerBxDriver::OnStartGrab()
{
	int nBuffers = 4;
	wrapBX_SetScanMode(m_pData->m_pCam1[m_nDevice].iFgNr, SM_SSMTC, &nBuffers);
	Start();
	return true;
}

void CBaumerBxDriver::OnStopGrab()
{
	Stop();
	wrapBX_StopScanMode(m_pData->m_pCam1[m_nDevice].iFgNr);
}

class _CBaumerBxSnapshot
{
public:
	CBaumerBxDriver *m_pDrv;
	HGLOBAL m_hg;
	LPBITMAPINFOHEADER m_lpbi;
	LPBYTE m_lpData;
	_CBaumerBxSnapshot(CBaumerBxDriver *pDrv, bool bSkip16bit)
	{
		m_pDrv = pDrv;
		_ptr_t2<BYTE> buf(sizeof(tCmdDesc));
		buf.zero();
		tpCmdDesc pCmdDesc = (tpCmdDesc)(LPBYTE)buf;
		DWORD CamSpecImFmt;
		int n = (int)g_CaptureMode;
		if (n <= 0 || n > m_pDrv->m_pData->m_nFmts)
			CamSpecImFmt = CF_STANDARD|BMPF_08BIT;
		else
		{
			CamSpecImFmt = m_pDrv->m_pData->m_pFmts[n-1].iMode;
			if (m_pDrv->m_pData->m_pFmts[n-1].support.b16Bit && !bSkip16bit)
			{
				pCmdDesc->num = 1;
				pCmdDesc->cmd[0].cmdId = CC_DATAWIDTH;
				pCmdDesc->cmd[0].cmdPa.AsDataWidth.set = W16BIT;
				pCmdDesc->cmd[0].cmdPa.AsDataWidth.ret = 0;
				CamSpecImFmt |= BMPF_16BIT;
			}
			else
				CamSpecImFmt |= BMPF_08BIT;
		}
		int nBuffers = 4;
		wrapBX_SetScanMode(m_pDrv->m_pData->m_pCam1[m_pDrv->m_nDevice].iFgNr, SM_SSMTC, &nBuffers);
		m_hg = wrapBX_GetSnapShotBmp(m_pDrv->m_nDevice, CamSpecImFmt, pCmdDesc, NULL, 0);
		wrapBX_StopScanMode(m_pDrv->m_pData->m_pCam1[m_pDrv->m_nDevice].iFgNr);
		LPBYTE lp = (LPBYTE)GlobalLock(m_hg);
		LPBITMAPFILEHEADER pbmf = (LPBITMAPFILEHEADER)lp;
		m_lpbi = (LPBITMAPINFOHEADER)(pbmf+1);
		m_lpData = lp+pbmf->bfOffBits;
		if ((bool)g_HorzMirror || (bool)g_VertMirror)
			FlipDIB(m_lpbi, m_lpData, (bool)g_HorzMirror, (bool)g_VertMirror);
	}
	~_CBaumerBxSnapshot()
	{
		GlobalUnlock(m_hg);
	}
};

HRESULT CBaumerBxDriver::InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings)
{
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0&&!CanCaptureDuringPreview(), false);
	if (!DoInitCamera() || !PreinitGrab(true))
		return MakeDummyDIB(pDIBPrv, 1024, 768, 8)?S_OK:E_FAIL;
	_CBaumerBxSnapshot Snapshot(this,true);
	if ((bool)g_Gray && Snapshot.m_lpbi->biBitCount == 24)
	{
		LPBITMAPINFOHEADER lpbiCap = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(::DIBSize(Snapshot.m_lpbi->biWidth,
			Snapshot.m_lpbi->biHeight, 8, true));
		if (lpbiCap == NULL)
			return E_FAIL;
		InitDIB(lpbiCap, Snapshot.m_lpbi->biWidth, Snapshot.m_lpbi->biHeight, 8, false);
		ConvertDIB24ToDIB8(lpbiCap, Snapshot.m_lpbi, (int)g_GrayPlane);
	}
	else
	{
		LPBITMAPINFOHEADER lpbiCap = (LPBITMAPINFOHEADER)pDIBPrv->AllocMem(::DIBSize(Snapshot.m_lpbi, true));
		if (lpbiCap == NULL)
			return E_FAIL;
		memcpy(lpbiCap, Snapshot.m_lpbi, sizeof(BITMAPINFOHEADER)+DIBPaletteEntries(Snapshot.m_lpbi)*sizeof(RGBQUAD));
		memcpy(DIBData(lpbiCap), Snapshot.m_lpData, DIBSize(Snapshot.m_lpbi, false));
	}
	return S_OK;
}


void CBaumerBxDriver::ProcessCapture()
{
	_CBaumerBxSnapshot Snapshot(this,false);
	if (m_pimgCap != 0)
	{
		if ((bool)g_Gray && Snapshot.m_lpbi->biBitCount == 24)
		{
			CDib Dib;
			Dib.InitBitmap(Snapshot.m_lpbi->biWidth, Snapshot.m_lpbi->biHeight, 8);
			ConvertDIB24ToDIB8(Dib.m_lpbi, Snapshot.m_lpbi, (int)g_GrayPlane);
			::AttachDIBBits(m_pimgCap, Dib.m_lpbi, (LPBYTE)::DIBData(Dib.m_lpbi));
		}
		else
			::AttachDIBBits(m_pimgCap, Snapshot.m_lpbi, Snapshot.m_lpData);
	}
}


void CBaumerBxDriver::ThreadRoutine()
{
	HANDLE hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	wrapBX_DefineImageNotificationEvent(SRC_IFC, m_pData->m_pCam1[m_nDevice].iFgNr, hEvent);
	while (1)
	{
		if (WaitForEvent(hEvent))
			break;
		tFormat DmaImFormat;
		DmaImFormat.iSizeof = sizeof(DmaImFormat);
		if (wrapBX_DmaImFormat(m_nDevice, &DmaImFormat))
		{
			int x = ((DmaImFormat.dmaImSizeX+3)>>4)<<4;
			int y = DmaImFormat.dmaImSizeY;
			int modus;
			if (DmaImFormat.dmaFrametransfer)
				y *= 2;
			bool bGrayFromRGB = false;
			int nCamId = m_pData->m_pCam1[m_nDevice].iCamId;
			if (nCamId == DC480 || nCamId == ARC8000C)
			{
				if (m_pData->m_nExtImFormat == CF_C282FLUORRED ||
					m_pData->m_nExtImFormat == CF_C282FLUORBLUE ||
					m_pData->m_nExtImFormat == CF_C282FLUORGREEN)
					modus = 2;
				else
					modus = 1;
			}
			else if (nCamId == DC300 || nCamId == ARC6000C)
			{
				if (m_pData->m_nExtImFormat == CF_C32FRAMEA ||
					m_pData->m_nExtImFormat == CF_C32FRAMEB)
					modus = m_pData->m_nExtImFormatAdd;
				else
					modus = 1;
			}
			else if (m_pData->m_bBinning)
				modus = 2;
			else if ((bool)g_Gray)
			{
				if ((int)g_GrayFromRGB && m_pData->m_Type.bColor)
					bGrayFromRGB = true; // modus will not be used!
				modus = 2;
			}
			else
				modus = m_pData->m_Type.bColor?1:2;
			int bpp = modus==1?24:8;
			{
			CCritSectionAccess csa(&m_csCap);
			int nPrevX = m_Dib.cx(), nPrevY = m_Dib.cy(), nPrevBPP = m_Dib.bpp();
			if (x != m_Dib.cx() || y != m_Dib.cy() || bpp != m_Dib.bpp())
				m_Dib.InitBitmap(x, y, bpp);
			BITMAPFILEHEADER bmfHdr;
			int ret;
			if (bGrayFromRGB)
			{
				CDib DibRGB;
				DibRGB.InitBitmap(x, y, 24);
				ret = wrapBX_GetBmpDirect(m_nDevice, &bmfHdr, DibRGB.m_lpbi,
					(LPBYTE)DibRGB.GetData(), DibRGB.Size(false), 1 );
				ConvertDIB24ToDIB8(m_Dib.m_lpbi, DibRGB.m_lpbi, (int)g_GrayPlane);
			}
			else
				ret = wrapBX_GetBmpDirect(m_nDevice, &bmfHdr, m_Dib.m_lpbi,
					(LPBYTE)m_Dib.GetData(), m_Dib.Size(false), modus );
			if ((bool)g_HorzMirror || (bool)g_VertMirror)
				m_Dib.Flip((bool)g_HorzMirror, (bool)g_VertMirror);
			if (nPrevX != m_Dib.cx() || nPrevY != m_Dib.cy() || nPrevBPP != m_Dib.bpp())
				s_bSizesChanged = true;
			}
			OnPrvFrameReady();
		}
	}
}

void CBaumerBxDriver::OnTimer()
{
	if (s_bSizesChanged)
	{
		NotifyChangeSizes();
		s_bSizesChanged = false;
	}
}

void CBaumerBxDriver::OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal)
{
//	if (g_CaptureMode.CheckByName(lpstrSec, lpstrEntry))
//		s_bSizesChanged = true;
}

HRESULT CBaumerBxDriver::SetValueInt(int nDev, BSTR bstrSec, BSTR bstrEntry, int nValue, UINT dwFlags)
{
	if (g_Exposure.CheckByName(bstrSec, bstrEntry))
	{
		g_Exposure.SetValueInt(nValue);
		wrapBX_SetShutterTime(m_nDevice, (int)g_Exposure, &m_pData->m_Sht);
		return S_OK;
	}
	else if (g_Offset.CheckByName(bstrSec, bstrEntry))
	{
		g_Offset.SetValueInt(nValue);
		wrapBX_SetBlackOffset(m_nDevice, nValue, &m_pData->m_BLOffset);
		return S_OK;
	}
	else if (g_AutoExposure.CheckByName(bstrSec, bstrEntry))
	{
		g_AutoExposure.SetValueInt(nValue);
		wrapBX_SetAutoExposure(m_nDevice, (int)g_AutoExposure, (int)g_AutoExposureLevel);
		if (((bool)g_AutoExposure) == false)
		{
			tSHT sht;
			wrapBX_SetShutterTime(m_nDevice, (int)g_Exposure, &sht);
			/*tSHT sht;
			wrapBX_SetShutterTime(m_nDevice, -1, &sht);
			g_Exposure.SetValueInt(sht.timeNear, true);*/
		}
		return S_OK;
	}
	else if (g_ColorBalanceAuto.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceAuto.SetValueInt(nValue);
		return S_OK;
	}
	else if (g_BinningMode.CheckByName(bstrSec, bstrEntry))
	{
		HRESULT r = __super::SetValueInt(nDev, bstrSec, bstrEntry, nValue, dwFlags);
		NotifyChangeSizes();
		return r;
	}
	else
		return __super::SetValueInt(nDev, bstrSec, bstrEntry, nValue, dwFlags);
}

HRESULT CBaumerBxDriver::SetValueDouble(int nDev, BSTR bstrSec, BSTR bstrEntry, double dValue, UINT dwFlags)
{
	if (g_Gain.CheckByName(bstrSec, bstrEntry))
	{
		g_Gain.SetValueDouble(dValue);
		wrapBX_SetGainFactor(m_nDevice, (double)g_Gain, &m_pData->m_Gain);
		return S_OK;
	}
	else if (g_HGamma.CheckByName(bstrSec, bstrEntry))
	{
		g_HGamma.SetValueDouble(dValue);
		_SetGamma(m_nDevice, dValue);
		return S_OK;
	}
	else if (g_ColorBalanceRed.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceRed.SetValueDouble(dValue);
		SetWhiteBalance();
		return S_OK;
	}
	else if (g_ColorBalanceGreen.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceGreen.SetValueDouble(dValue);
		SetWhiteBalance();
		return S_OK;
	}
	else if (g_ColorBalanceBlue.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceBlue.SetValueDouble(dValue);
		SetWhiteBalance();
		return S_OK;
	}
	else
		return __super::SetValueDouble(nDev, bstrSec, bstrEntry, dValue, dwFlags);
}

HRESULT CBaumerBxDriver::OnCmdMsg(int nCode, UINT nID, long lHWND)
{
	if (nID == IDC_WB_BUILD)
	{
		if (m_pData->m_Type.bColor)
		{
			double r,g,b;
			wrapBX_DoWhiteBalance(m_nDevice, TRUE, &r, &g, &b, CRect(0,0,0,0));
			g_ColorBalanceRed = r;
			g_ColorBalanceGreen = g;
			g_ColorBalanceBlue = b;
			SetWhiteBalance();
		}
		return S_OK;
	}
	return __super::OnCmdMsg(nCode, nID, lHWND);
}

HRESULT CBaumerBxDriver::OnUpdateCmdUI(HWND hwnd, int nID)
{
	if (m_CamState != Succeeded)
		return S_OK;
	int nCamId = m_pData->m_pCam1[m_nDevice].iCamId;
	if (nID == IDC_CHECK_GRAYSCALE)
		::EnableWindow(hwnd, m_pData&&m_pData->m_Type.bColor&&!(nCamId==ARC6000C||nCamId==ARC8000C||
			nCamId==DC300||nCamId==DC480));
	else if (nID == IDC_EDIT_EXPOSURE || nID == IDC_SPIN1_EXPOSURE || nID == IDC_SLIDER1_EXPOSURE ||
		nID == IDC_SPIN2_EXPOSURE || nID == IDC_SLIDER2_EXPOSURE)
		::EnableWindow(hwnd, !(int)g_AutoExposure);
	else if (nID == IDC_EDIT_WB_RED || nID == IDC_SPIN_WB_RED || nID == IDC_SLIDER_WB_RED ||
		nID == IDC_EDIT_WB_GREEN || nID == IDC_SPIN_WB_GREEN || nID == IDC_SLIDER_WB_GREEN ||
		nID == IDC_EDIT_WB_BLUE || nID == IDC_SPIN_WB_BLUE || nID == IDC_SLIDER_WB_BLUE)
		::EnableWindow(hwnd, (int)g_ColorBalanceAuto==1);
	else if (nID == IDC_WB_BUILD)
		::EnableWindow(hwnd, (int)g_ColorBalanceAuto==0);
	else if (nID == IDC_COLOR_BALANCE)
		::EnableWindow(hwnd, m_pData->m_Type.bColor);
	else if (nID == IDC_RED || nID == IDC_GREEN || nID == IDC_BLUE)
		::EnableWindow(hwnd, m_pData->m_Type.bColor);
	else if (nID == IDC_COMBO_CAPTURE)
		::EnableWindow(hwnd, m_pData&&!(nCamId==ARC6000C||nCamId==ARC8000C||nCamId==DC300||nCamId==DC480));
	else if (nID == IDC_CAPTURE_EX)
		::EnableWindow(hwnd, ((VALUEINFO*)g_CaptureMode.GetVI())->nMax>1);
	return __super::OnUpdateCmdUI(hwnd, nID);
}

