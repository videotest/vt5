#include "StdAfx.h"
#include "resource.h"
#include "HamamatsuDriver.h"
#include "VT5Profile.h"
#include "PreviewPixDlg.h"
#include "CamValues.h"
#include "MethCombo.h"
#include "Focus.h"
#include "SettingsBase.h"
#include "MethNameDlg.h"
#include "dcamapi.h"
#include "dcamapix.h"
#include "features.h"
#include "debug.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

struct CHamamatsuData
{
	HDCAM m_hDCam;
	DWORD m_dwCaps;
	long m_lLutMin,m_lLutMax;
	long m_lLutOutMin,m_lLutOutMax;
	CSize m_szImage;
	ccDatatype m_Datatype;

	CHamamatsuData()
	{
		m_hDCam = NULL;
		m_dwCaps = NULL;
		m_lLutMin = 0;
		m_lLutMax = 1500;
		m_lLutOutMin = 0;
		m_lLutOutMax = 255;
		m_szImage = CSize(0,0);
		m_Datatype = ccDatatype_none;
	}
};

static void _dcam_set_feature(HDCAM hDCam, DWORD dwFeatureId, DWORD dwFlags, float fValue)
{
	DCAM_PARAM_FEATURE	param;
	memset(&param, 0, sizeof(param));
	param.hdr.cbSize	= sizeof (param);
	param.hdr.id		= DCAM_IDPARAM_FEATURE;
	param.featureid		= dwFeatureId;
	param.flags			= dwFlags|DCAM_FEATURE_FLAGS_IMMEDIATE;
	param.featurevalue	= fValue;
	BOOL b = dcam_extended(hDCam, DCAM_IDMSG_SETPARAM, & param, sizeof(param));
	if (!b)
		dprintf("dcam_extended failed in _dcam_set_feature, dwFeatureId = %d, "
			"dwFlags = %d, dValue = %g\n\r", dwFeatureId, dwFlags, (double)fValue);
}

CCamIntValue2 g_Exposure(_T("Parameters"), _T("Exposure"), 100, 1, 1000, 10, 1, 0, IDC_EDIT_EXPOSURE);
INTCOMBOITEMDESCR s_aModes[] =
{
	{1, _T("1:1")},
	{2, _T("1:2")},
	{4, _T("1:4")},
	{8, _T("1:8")},
};
CCamIntComboValue  g_CaptureMode(_T("Parameters"), _T("Binning"), s_aModes, 3,
	1, 0, IDC_COMBO_CAPTURE);
CCamBoolValue g_Gain(_T("Parameters"), _T("Gain"), false, 0, IDC_CHECK_GAIN);
CCamBoolValue g_LightMode(_T("Parameters"), _T("LightMode"), true, 0, IDC_CHECK_LIGHTMODE);

CHamamatsuDriver::CHamamatsuDriver()
{
	m_nDevices = 0;
	m_pData = new CHamamatsuData;
	m_lCurFrame = -1;
	m_lFramesCount = -1;
	g_Gray.m_vi.nDefault = 1;
	g_VertMirror.SetId(IDC_CHECK_FLIP_VERT);
	g_HorzMirror.SetId(IDC_CHECK_FLIP_HORZ);
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
	g_Brightness.SetId(IDC_EDIT_ENH_BRIGHTNESS);
	g_Contrast.SetId(IDC_EDIT_ENH_CONTRAST);
	g_Gamma.SetId(IDC_EDIT_ENH_GAMMA);
	CBaseDlg::m_idDefault = IDC_DEFAULT;
}

CHamamatsuDriver::~CHamamatsuDriver()
{
	delete m_pData;
}

bool CHamamatsuDriver::InitCamera()
{
	if (!dcam_init(AfxGetInstanceHandle(), &m_nDevices, 0))
	{
		dprintf("dcam_init failed\n\r");
		return false;
	}
	if (!dcam_open(&m_pData->m_hDCam, NULL))
	{
		dprintf("dcam_open failed\n\r");
		dcam_uninit(AfxGetInstanceHandle(), NULL);
		return false;
	}
	if (dcam_getcapability(m_pData->m_hDCam, &m_pData->m_dwCaps, DCAM_QUERYCAPABILITY_DATATYPE))
	{
		if (m_pData->m_dwCaps & ccDatatype_uint16)
		{
			if (!dcam_setdatatype(m_pData->m_hDCam, ccDatatype_uint16))
				dprintf("dcam_setdatatype(..., ccDatatype_uint16) failed\n\r");
		}
		else
			dprintf("camera does not supports ccDatatype_uint16\n\r");
	}
	else
		dprintf("dcam_getcapability failed\n\r");
	LutReset();
	return true;
}

void CHamamatsuDriver::DeinitCamera()
{
	dcam_close(m_pData->m_hDCam);
	m_pData->m_hDCam = NULL;
	dcam_uninit(AfxGetInstanceHandle(), NULL);
}

void CHamamatsuDriver::LutReset()
{
	long lDataMin,lDataMax;
	if (dcam_getdatarange(m_pData->m_hDCam, &lDataMax, &lDataMin))
	{
		m_pData->m_lLutMin = lDataMin;
		m_pData->m_lLutMax = lDataMax;
	}
	if (!dcam_setbitsinputlutrange(m_pData->m_hDCam, m_pData->m_lLutMax, m_pData->m_lLutMin))
		dprintf("dcam_setbitsinputlutrange failed\n\r");
	if (!dcam_setbitsoutputlutrange(m_pData->m_hDCam, (BYTE)m_pData->m_lLutOutMax, (BYTE)m_pData->m_lLutOutMin))
		dprintf("dcam_setbitsoutputlutrange failed\n\r");
}

bool CHamamatsuDriver::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
	BOOL bFirst, BOOL bForInput)
{
	CPreviewIds PreviewIds = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG, IDC_STATIC_PERIOD}, 
		IDC_STATIC_PERIOD, IDC_IMAGE, IDC_IMAGE_PAGE, IDC_SETTINGS_PAGE, -1, -1, IDC_HISTOGRAM,
		IDS_FPS, IDS_FRAME_PERIOD};
	CPreviewPixDlg  dlg(IDD_PREVIEW,PreviewIds,(IInputPreview *)this,nDevice,0,CWnd::FromHandle(hwndParent));
	dlg.AttachChildDialogToButton(IDC_IMAGE_PAGE, IDD_IMAGE);
	dlg.AttachChildDialogToButton(IDC_COLOR_BALANCE, IDD_ENHANCEMENT);
	dlg.AttachChildDialogToButton(IDC_SETTINGS_PAGE, IDD_SETTINGS);
	return dlg.DoModal() == IDOK;
}

HRESULT CHamamatsuDriver::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CSettingsIds Ids = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG, IDC_STATIC_PERIOD},
		IDC_TAB_SELECT, 0, IDS_FPS, IDS_FRAME_PERIOD, IDC_HISTOGRAM};
	CSettingsBase dlg((IDriver*)this, 0, IDD_DIALOG_SETTINGS, Ids, NULL);
	dlg.AddPane(IDS_BRIGHTNESS, IDD_SHUTTER);
	dlg.AddPane(IDS_IMAGE, IDD_IMAGE);
	dlg.AddPane(IDS_WHITE_BALANCE, IDD_ENHANCEMENT);
	dlg.AddPane(IDS_SETTINGS, IDD_SETTINGS);
	dlg.DoModal();
	return S_OK;
}

bool CHamamatsuDriver::GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi)
{
	dcam_getdatasize(m_pData->m_hDCam, &m_pData->m_szImage);
	if (m_pData->m_szImage.cx < 0 || m_pData->m_szImage.cy < 0)
	{
		dprintf("dcam_getdatasize failed\n\r");
		return false;
	}
	dcam_getdatatype(m_pData->m_hDCam, &m_pData->m_Datatype);
	InitDIB(lpbi, m_pData->m_szImage.cx, m_pData->m_szImage.cy, 12, false);
//	g_Gray = 0;
	return true;
}

bool CHamamatsuDriver::PreinitGrab(bool bCapture)
{
	DWORD dwStatus;
	if (!dcam_getstatus(m_pData->m_hDCam, &dwStatus))
	{
		dprintf("dcam_getstatus failed in CHamamatsuDriver::DoStartGrab\n\r");
		return false;
	}
	if (dwStatus == DCAM_STATUS_BUSY)
	{
		if (!dcam_idle(m_pData->m_hDCam))
		{
			dprintf("dcam_idle failed in CHamamatsuDriver::OnStartGrab\n\r");
			return false;
		}
	}
	dcam_setexposuretime(m_pData->m_hDCam, (int)g_Exposure/1000.);
	dcam_setbinning(m_pData->m_hDCam, (int)g_CaptureMode);
	_dcam_set_feature(m_pData->m_hDCam, DCAM_IDFEATURE_GAIN, DCAM_FEATURE_FLAGS_MANUAL, float(bool(g_Gain)?1.:0.));
	// Gamma and Lightmode seems has not differences (C8484)
	_dcam_set_feature(m_pData->m_hDCam, DCAM_IDFEATURE_GAMMA, DCAM_FEATURE_FLAGS_MANUAL, float(bool(g_LightMode)?1.:0.));
	_dcam_set_feature(m_pData->m_hDCam, DCAM_IDFEATURE_LIGHTMODE, DCAM_FEATURE_FLAGS_MANUAL, float(bool(g_LightMode)?1.:0.));
	return true;
}

bool CHamamatsuDriver::DoStartGrab(bool bCapture, int nFrames, bool bSetTimer)
{
	if (!dcam_precapture(m_pData->m_hDCam, bCapture?ccCapture_Snap:ccCapture_Sequence))
	{
		dprintf("dcam_precapture failed\n\r");
		return false;
	}
	if (!dcam_allocframe(m_pData->m_hDCam, nFrames))
	{
		dprintf("dcam_allocframe failed\n\r");
		return false;
	}
	m_lCurFrame = -1;
	m_lFramesCount = -1;
	if (bSetTimer)
		_SetTimer(20);
	if (!dcam_capture(m_pData->m_hDCam))
	{
		dprintf("dcam_capture failed\n\r");
		dcam_freeframe(m_pData->m_hDCam);
		if (bSetTimer)
			_KillTimer();
		return false;
	}
	return true;
}

bool CHamamatsuDriver::OnStartGrab()
{
	return DoStartGrab(false, 3, true);
}

void CHamamatsuDriver::OnStopGrab()
{
	if (!dcam_idle(m_pData->m_hDCam))
		dprintf("dcam_idle failed in CHamamatsuDriver::OnStopGrab\n\r");
	_KillTimer();
	dcam_freeframe(m_pData->m_hDCam);
}

void CHamamatsuDriver::ProcessCapture()
{
	if (DoStartGrab(true, 1, false))
	{
		do
		{
			long lCurFrame,lFramesCount;
			dcam_gettransferinfo(m_pData->m_hDCam, &lCurFrame, &lFramesCount);
			if (lFramesCount < 0) break;
			if (lCurFrame >= 0)
			{
				LPVOID pData;
				long lRow;
				if (dcam_lockdata(m_pData->m_hDCam, &pData, &lRow, lCurFrame))
				{
					GrayReady16((LPWORD)pData);
					Sleep(200);
					dcam_unlockdata(m_pData->m_hDCam);
				}
				break;
			}
		}
		while (1);
		if (!dcam_idle(m_pData->m_hDCam))
			dprintf("dcam_idle failed in CHamamatsuDriver::OnStopGrab\n\r");
		dcam_freeframe(m_pData->m_hDCam);
	}
}


void CHamamatsuDriver::OnTimer()
{
	long lCurFrame,lFramesCount;
	dcam_gettransferinfo(m_pData->m_hDCam, &lCurFrame, &lFramesCount);
	if (lCurFrame != m_lCurFrame && lFramesCount != m_lFramesCount)
	{
		m_lCurFrame = lCurFrame;
		m_lFramesCount = lFramesCount;
		LPVOID pData;
		long lRow;
		if (dcam_lockdata(m_pData->m_hDCam, &pData, &lRow, m_lCurFrame))
		{
			GrayReady16((LPWORD)pData);
			dcam_unlockdata(m_pData->m_hDCam);
		}
	}
}

void CHamamatsuDriver::OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal)
{
	if (_tcscmp(lpstrEntry,  _T("Binning")) == 0)
		m_bSizeChanged = true;
}


