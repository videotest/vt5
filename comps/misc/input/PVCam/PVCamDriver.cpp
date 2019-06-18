#include "StdAfx.h"
#include "resource.h"
#include "PVCamApp.h"
#include "PVCamDriver.h"
#include "VT5Profile.h"
#include "PreviewPixDlg.h"
#include "CamValues.h"
#include "MethCombo.h"
#include "Focus.h"
#include "SettingsBase.h"
#include "MethNameDlg.h"
#include <math.h>

CCamIntValue2 g_Exposure(_T("Parameters"), _T("Exposure"), 100, 1, 2000, 100, 10, 0, IDC_EDIT_EXPOSURE);
CCamIntValue g_Gain(_T("Parameters"), _T("Gain"), 1, 1, 16, IRepr_SmartIntEdit, 0, IDC_EDIT_ENH_GAIN);
extern CCamBoolValue g_Histo;

CCamIntComboValue2  g_PreviewMode(_T("Preview"), _T("Mode"), 0, 0, IDC_COMBO_PREVIEW_MODE,
	CCamValue::WriteToReg);
CCamIntComboValue2  g_CaptureMode(_T("Capture"), _T("Mode"), 0, 0, IDC_COMBO_CAPTURE_MODE,
	CCamValue::WriteToReg);

CCamBoolValue g_IFPreview(_T("InputFrame"), _T("Preview"), false, 0, IDC_CHECK_PREVIEW_INPUT_FRAME,
	CCamValue::WriteToReg|CCamValue::IgnoreMethodic);
CCamBoolValue g_IFCapture(_T("InputFrame"), _T("Capture"), false, 0, IDC_CHECK_CAPTURE_INPUT_FRAME,
	CCamValue::WriteToReg|CCamValue::SkipReinitCamera|CCamValue::IgnoreMethodic);

class CCamIFRect : public CCamValueRect
{
public:
	CCamIFRect(LPCTSTR lpstrSec, int nChain = 0, int nDlgCtrl = -1, DWORD dwFlags = CCamValue::WriteToReg)
		: CCamValueRect(lpstrSec, nChain, nDlgCtrl, dwFlags)
	{
	}
	virtual bool NeedDrawRect()
	{
		return __super::NeedDrawRect() && !((bool)g_IFPreview);
	}
};

CCamIFRect g_IFRect(_T("InputFrame"), 0, IDC_CHECK_ENABLE_INPUT_FRAME,
	CCamValue::WriteToReg|CCamValue::IgnoreMethodic);

/***********************************************************************
*		Build a speed table (to support 1394 cameras)
*
************************************************************************/
static bool BuildSpeedTable(int16 hCam)
{
    int     Speed;
//	int		value;
    int16	MaxSpeeds = 0;
    /*if (!pl_get_param (hCam, PARAM_SPDTAB_INDEX, ATTR_MAX, &MaxSpeeds))
	{
        printf("Could not get number of speed table entries");
       
    }
	else
	{
        MaxSpeeds++;
      printf("Max Speeds = %i\n" , MaxSpeeds);
    }


    for (Speed = 0; Speed < MaxSpeeds; Speed++)
	{

        if (!pl_set_param (hCam, PARAM_SPDTAB_INDEX, &Speed)) {
            printf("Could not set speed table index  " );
            continue;
        }

        if (!pl_get_param (hCam, PARAM_BIT_DEPTH, ATTR_CURRENT, &value)) {
            printf("Could not get BitDepth");
        }

        if (!pl_get_param (hCam, PARAM_PIX_TIME, ATTR_CURRENT, &value)) {
            printf("Could not get PixTime");
        }

        if (!pl_get_param (hCam, PARAM_READOUT_PORT, ATTR_CURRENT, &value)) {
            printf("Could not get ReadoutPort");
        }

        if (!pl_get_param (hCam, PARAM_READOUT_PORT, ATTR_COUNT, &value)) {
            printf("Could not get PortsInUse");
        }

        if (!pl_get_param (hCam, PARAM_GAIN_INDEX, ATTR_CURRENT, &value)) {
            printf("Could not get GainIndex");
        }

        if (!pl_get_param (hCam, PARAM_GAIN_INDEX, ATTR_COUNT, &value)) {
            printf("Could not get MaxGain");
        }
    }*/

    Speed = 0;
    if (!pl_set_param (hCam, PARAM_SPDTAB_INDEX, &Speed))
       return false;
	return true;
}

static CRect GetCaptureRect(int cxCam, int cyCam)
{
	CRect rcSrc = g_IFRect.SafeGetRect(cxCam,cyCam);
	CRect rc;
	rc.left = ((bool)g_HorzMirror)?rcSrc.left:cxCam-rcSrc.right-1;
	rc.right = ((bool)g_HorzMirror)?rcSrc.right:cxCam-rcSrc.left-1;
	rc.top = (!(bool)g_VertMirror)?rcSrc.top:cyCam-rcSrc.bottom-1;
	rc.bottom = (!(bool)g_VertMirror)?rcSrc.bottom:cyCam-rcSrc.top-1;
	return rc;
}


CPVCamDriver::CPVCamDriver(void)
{
	m_CamState = NotInited;
//	m_bReconnect = false;
//	m_bRemoved = false;
//	m_lpbiCap = NULL;
	g_VertMirror.SetId(IDC_CHECK_FLIP_VERT);
	g_HorzMirror.SetId(IDC_CHECK_FLIP_HORZ);
//	g_Gray.SetId(IDC_CHECK_GRAYSCALE);
//	g_OUExpEnabled.SetId(IDC_CHECK_OVER_UNDER_EXPOSURE);
	g_Methodics.SetId(IDC_LIST_SETTINGS);
	m_nIdNewMeth = IDC_BUTTON_NEW_SETTINGS;
	m_nIdDeleteMeth = IDC_BUTTON_DELETE_SETTINGS;
//	g_Focus.SetId(IDC_CHECK_FOCUS);
//	g_FocusRect.SetId(IDC_SHOW_FOCUS_RECT);
//	g_FocusValue.SetId(IDC_STATIC_FOCUS_CURRENT);
//	g_MaxFocus.SetId(IDC_STATIC_FOCUS_PEAK);
//	m_nIdStaticFocusMax = IDC_STATIC_FOCUS_PEAK1;
//	m_nIdStaticFocusCur = IDC_STATIC_FOCUS_CURRENT1;
	CMethNameDlg::s_idd = IDD_DIALOG_SETTING_NAME;
	CMethNameDlg::s_idEdit = IDC_EDIT1;
	g_Brightness.SetId(IDC_EDIT_ENH_BRIGHTNESS);
	g_Contrast.SetId(IDC_EDIT_ENH_CONTRAST);
	g_Gamma.SetId(IDC_EDIT_ENH_GAMMA);
	((VALUEINFO*)g_Gray.GetVI())->nDefault = true;

	m_hCam = 0;
	m_cxCam = m_cyCam = 0;
	m_bUseCircBuf = false;
}

CPVCamDriver::~CPVCamDriver(void)
{
}

IUnknown *CPVCamDriver::DoGetInterface( const IID &iid )
{
	IUnknown *p;
	if (p = CDriverPreview::DoGetInterface(iid))
		return p;
	else
		return ComObjectBase::DoGetInterface(iid);
}

bool CPVCamDriver::InitCamera()
{
	pl_pvcam_init();
	int16 cam_num;
	pl_cam_get_total(&cam_num);
	if (cam_num == 0)
	{
		pl_pvcam_uninit();
		return false;
	}
	char cam_name[CAM_NAME_LEN];
	pl_cam_get_name(0, cam_name);
	if (pl_cam_open(cam_name, &m_hCam, OPEN_EXCLUSIVE) == FALSE)
	{
		pl_pvcam_uninit();
		return false;
	}
	BuildSpeedTable(m_hCam);
	pl_ccd_get_par_size(m_hCam, &m_cyCam);
	pl_ccd_get_ser_size(m_hCam, &m_cxCam);
	pl_ccd_set_tmp_setpoint(m_hCam, -550);
	// use circular buffers if they are supported
	rs_bool bSupported;
	pl_get_param(m_hCam, PARAM_CIRC_BUFFER, ATTR_AVAIL, (void*)&bSupported );
	m_bUseCircBuf = bSupported != 0;
	m_buff.alloc(12*m_cxCam*m_cyCam);
	m_nGainIndMax = 0;
	pl_get_param(m_hCam, PARAM_GAIN_INDEX, ATTR_MAX, (void *)&m_nGainIndMax);
	short nGain = (short)g_Gain;
	pl_set_param(m_hCam, PARAM_GAIN_INDEX, &nGain);
	pl_get_param(m_hCam, PARAM_BIT_DEPTH, ATTR_CURRENT, (void *)&m_nBitDepth);

	for (int i = 1; i <= 4; i++)
	{
		CString s;
		s.Format(_T("%dx%d"), m_cxCam/i, m_cyCam/i);
		g_PreviewMode.Add(i, s, -1);
		g_CaptureMode.Add(i, s, -1);
	}
/*	CString s1,s2;
	s1.Format(_T("%dx%d"), m_cxCam, m_cyCam);
	s2.Format(_T("%dx%d"), m_cxCam/2, m_cyCam/2);
	g_PreviewMode.Add(1, s1, -1);
	g_PreviewMode.Add(2, s2, -1);*/
	g_PreviewMode.Init();
//	g_CaptureMode.Add(1, s1, -1);
//	g_CaptureMode.Add(2, s2, -1);
	g_CaptureMode.Init();
//	unsigned short nMinExpositure,nMaxExpositure;
//	pl_get_param(m_hCam, PARAM_EXP_TIME, ATTR_MIN, &nMinExpositure);
//	pl_get_param(m_hCam, PARAM_EXP_TIME, ATTR_MAX, &nMaxExpositure);
	return true;
}

void CPVCamDriver::DeinitCamera()
{
	if (m_CamState == Succeeded)
	{
		m_buff.free();
		pl_cam_close(m_hCam);
		m_hCam = 0;
		pl_pvcam_uninit();
	}
	m_CamState = NotInited;
}


class CPreviewPVCamDlg : public CPreviewPixDlg
{
public:
	CPreviewPVCamDlg(int idd, CPreviewIds &PreviewIds, IUnknown *punk, int nDevice, DWORD dwFlags, CWnd* pParent = NULL) :
		CPreviewPixDlg(idd,PreviewIds,punk,nDevice,dwFlags,pParent)
	{
		m_pDriver = NULL;
	}
	DECLARE_MESSAGE_MAP()
	afx_msg void OnMove(int cx, int cy);

public:
	CPVCamDriver *m_pDriver;
};

BEGIN_MESSAGE_MAP(CPreviewPVCamDlg, CPreviewPixDlg)
	//{{AFX_MSG_MAP(CPreviewPVCamDlg)
	ON_WM_MOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CPreviewPVCamDlg::OnMove(int cx, int cy)
{
	if (m_pDriver)
		m_pDriver->Reinit();
}

void CPVCamDriver::Reinit()
{
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0, true);
}

bool CPVCamDriver::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
	BOOL bFirst, BOOL bForInput)
{
	CPreviewIds PreviewIds = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG, IDC_STATIC_PERIOD}, 
		IDC_STATIC_PERIOD, IDC_IMAGE, IDC_ENHANCEMENT, IDC_SETTINGS_PAGE, -1, -1, IDC_HISTOGRAM,
		IDS_FPS, IDS_FRAME_PERIOD};
	CPreviewPVCamDlg  dlg(IDD_PREVIEW,PreviewIds,(IInputPreview *)this,nDevice,0,CWnd::FromHandle(hwndParent));
	dlg.m_pDriver = this;
	dlg.AttachChildDialogToButton(IDC_ENHANCEMENT, IDD_ENHANCEMENT);
	dlg.AttachChildDialogToButton(IDC_IMAGE_PAGE, IDD_IMAGE);
//	dlg.AttachChildDialogToButton(IDC_COLOR_BALANCE, IDD_COLOR_BALANCE);
	dlg.AttachChildDialogToButton(IDC_SETTINGS_PAGE, IDD_SETTINGS);
	CBaseDlg::s_bIgnoreThumbTrack = true;
//	s_hwndParent = hwndParent;	
	return dlg.DoModal() == IDOK;
}

/*HRESULT CPVCamDriver::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CSettingsIds Ids = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG,IDC_STATIC_PERIOD},
		IDC_TAB_SELECT, 0,IDS_FPS,0,IDC_HISTOGRAM};
	CSettingsBase dlg((IDriver*)this, 0, IDD_DIALOG_SETTINGS, Ids, NULL);
	dlg.AddPane(IDS_BRIGHTNESS, IDD_SHUTTER);
	dlg.AddPane(IDS_ENHANCEMENT, IDD_ENHANCEMENT);
	dlg.AddPane(IDS_IMAGE, IDD_IMAGE);	
	dlg.AddPane(IDS_SETTINGS, IDD_SETTINGS);	
	dlg.DoModal();
	return S_OK;
}*/

bool CPVCamDriver::OnStartGrab()
{
	unsigned long stream_size;
	CRect rc = ((bool)g_IFPreview)&&g_IFRect.IsEnabled()?GetCaptureRect(m_cxCam,m_cyCam):
		CRect(0,0,m_cxCam,m_cyCam);
	rgn_type region;
	region.s1   = (unsigned short)rc.left;
	region.s2   = (unsigned short)(rc.right - 1);
	region.sbin = (int)g_PreviewMode;
	region.p1	= (unsigned short)rc.top;
	region.p2   = (unsigned short)(rc.bottom - 1);
	region.pbin = (int)g_PreviewMode;
	if (!m_bUseCircBuf)
	{
		pl_exp_init_seq();
		pl_exp_setup_seq(m_hCam, 1,	1, &region,	TIMED_MODE, (int)g_Exposure, &stream_size);
	}
	else
	{
		pl_exp_init_seq();
		pl_exp_setup_cont(m_hCam, 1, &region, TIMED_MODE, (int)g_Exposure, &stream_size, CIRC_OVERWRITE );
		pl_exp_start_cont(m_hCam, m_buff.ptr, m_buff.nsize);
	}
	Start();
	return true;
}

void CPVCamDriver::OnStopGrab()
{
	int16 status;
	uns32 not_needed;
	Stop();
	if (!m_bUseCircBuf)
	{
		pl_exp_finish_seq(m_hCam, m_buff.ptr, 0);
		pl_exp_uninit_seq();
	}
	else
	{
		while (pl_exp_check_cont_status(m_hCam, &status, &not_needed, &not_needed ) &&
			(status != READOUT_FAILED && status != READOUT_COMPLETE) );
		pl_exp_stop_cont(m_hCam, CCS_HALT);
		pl_exp_finish_seq(m_hCam , m_buff.ptr, 0);
		pl_exp_uninit_seq();
	}
}

bool CPVCamDriver::GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi)
{
	bool bFrame;
	int nbin;
	if (bCapture)
	{
		nbin = (int)g_CaptureMode;
		bFrame = (bool)g_IFCapture;
	}
	else
	{
		nbin = (int)g_PreviewMode;
		bFrame = (bool)g_IFPreview;
	}
	if (bFrame&&g_IFRect.IsEnabled())
	{
		CRect rc = g_IFRect.SafeGetRect(m_cxCam, m_cyCam);
		InitDIB(lpbi, rc.Width()/nbin, rc.Height()/nbin, m_nBitDepth, false);
	}
	else
		InitDIB(lpbi, m_cxCam/nbin, m_cyCam/nbin, m_nBitDepth, false);
	return true;
}

void CPVCamDriver::ThreadRoutine()
{
	short status = 0;
	if (!m_bUseCircBuf)
	{
		unsigned long bytes_xferd;
		while (!m_lStopFlag)
		{
			pl_exp_start_seq(m_hCam, m_buff.ptr);
			while (status != READOUT_COMPLETE && status != READOUT_FAILED && !m_lStopFlag)
			{		
				pl_exp_check_status(m_hCam, &status, &bytes_xferd);
			}
			if (!m_lStopFlag)
				GrayReady16((LPWORD)m_buff.ptr);
		}
	}
	else
	{
		uns32 not_need;
		void *caddress = NULL;
		while (!m_lStopFlag)
		{
			while (pl_exp_check_cont_status(m_hCam, &status, &not_need, &not_need) &&
				status != READOUT_COMPLETE && status != READOUT_FAILED && !m_lStopFlag);
			if (m_lStopFlag)
				break;
			if (status == READOUT_FAILED)
			{
				Sleep(200);
				continue;
			}
			if (pl_exp_get_latest_frame(m_hCam, &caddress))
			{
				GrayReady16((LPWORD)caddress);
			}
		}
	}
}


void CPVCamDriver::ProcessCapture()
{
	unsigned long stream_size;
	short status = 0;
	unsigned long bytes_xferd;
	CRect rc = ((bool)g_IFCapture)&&g_IFRect.IsEnabled()?GetCaptureRect(m_cxCam,m_cyCam):
		CRect(0,0,m_cxCam,m_cyCam);
	rgn_type region;
	region.s1   = (unsigned short)rc.left;
	region.s2   = (unsigned short)(rc.right - 1);
	region.sbin = (int)g_CaptureMode;
	region.p1	= (unsigned short)rc.top;
	region.p2   = (unsigned short)(rc.bottom - 1);
	region.pbin = (int)g_CaptureMode;
	pl_exp_init_seq();
	pl_exp_setup_seq(m_hCam, 1,	1, &region,	TIMED_MODE, (int)g_Exposure, &stream_size);
	pl_exp_start_seq(m_hCam, m_buff.ptr);
	while (status != READOUT_COMPLETE)
	{		
		pl_exp_check_status(m_hCam, &status, &bytes_xferd);
	}
	GrayReady16((LPWORD)m_buff.ptr);
	pl_exp_finish_seq(m_hCam, m_buff.ptr, 0);
	pl_exp_uninit_seq();
}

bool CPVCamDriver::TestExposition(int nExposure, HWND hwndDlg)
{
	unsigned long stream_size;
	short status = 0;
	unsigned long bytes_xferd;
	CRect rc = ((bool)g_IFPreview)&&g_IFRect.IsEnabled()?GetCaptureRect(m_cxCam,m_cyCam):
		CRect(0,0,m_cxCam,m_cyCam);
	rgn_type region;
	region.s1   = (unsigned short)rc.left;
	region.s2   = (unsigned short)(rc.right - 1);
	region.sbin = (int)g_PreviewMode;
	region.p1	= (unsigned short)rc.top;
	region.p2   = (unsigned short)(rc.bottom - 1);
	region.pbin = (int)g_PreviewMode;
	pl_exp_setup_seq(m_hCam, 1,	1, &region,	TIMED_MODE, nExposure, &stream_size);
	pl_exp_start_seq(m_hCam, m_buff.ptr);
	while (status != READOUT_COMPLETE)
	{		
		pl_exp_check_status(m_hCam, &status, &bytes_xferd);
	}
	GrayReady16((LPWORD)m_buff.ptr);
	pl_exp_finish_seq(m_hCam, m_buff.ptr, 0);
	int nBri = CalAvrBrightnessProc(m_Dib.m_lpbi, 1, 90);
	return nBri > 200;
}

void ProcessMessages()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CPVCamDriver::ProcessAutoexposure(HWND hwndDlg)
{
	XStopPreview StopGrab(this, m_CamState==Succeeded&&m_nGrabCount>0, true);
	::EnableWindow(hwndDlg, FALSE);
	CWaitCursor wc;
	// Prepare progress bar
	HWND hwndPrg = ::GetDlgItem(hwndDlg, IDC_PROGRESS_AUTOEXPOSURE);
	::ShowWindow(hwndPrg, SW_SHOW);
	::UpdateWindow(hwndPrg);
	int nExp1 = 0,nExp2 = 1000;
	int nStep = 0;
	int nSteps = (int)(ceil(log(nExp2)));
	::SendMessage(hwndPrg, PBM_SETRANGE, 0, MAKELONG(0,nSteps));
	::SendMessage(hwndPrg, PBM_SETPOS, 0, 0);
	ProcessMessages();
//	::UpdateWindow(hwndPrg);
	// Prepare camera
	pl_exp_init_seq();
	// Search optimal exposition
	while (nExp2>nExp1+1&&(nExp2-nExp1)>nExp2/10)
	{
		int nExpTest = (nExp1+nExp2)/2;
		bool bOver = TestExposition(nExpTest,hwndDlg);
		if (bOver)
			nExp2 = nExpTest;
		else
			nExp1 = nExpTest;
		::SendMessage(hwndPrg, PBM_SETPOS, min(++nStep,nSteps-1), 0);
		ProcessMessages();
//		::UpdateWindow(hwndPrg);
	}
	// Uninitialize progressbar & camera
	::ShowWindow(hwndPrg, SW_HIDE);
	pl_exp_uninit_seq();
	g_Exposure.SetValueInt(nExp1,true);
	::EnableWindow(hwndDlg, TRUE);
}


int CPVCamDriver::GetPreviewZoom()
{
	return 1;
//	return (int)g_PreviewMode;
}

bool CPVCamDriver::CanCaptureDuringPreview()
{
	return (int)g_PreviewMode==(int)g_CaptureMode &&
		(bool)g_IFPreview==(bool)g_IFCapture;
}

bool CPVCamDriver::CanCaptureFromSettings()
{
	return (int)g_PreviewMode==(int)g_CaptureMode &&
		(bool)g_IFPreview==(bool)g_IFCapture;
}


HRESULT CPVCamDriver::SetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int nValue,
	UINT nFlags)
{
	return __super::SetValueInt(nCurDev, bstrSec, bstrEntry, nValue, nFlags);
}

void CPVCamDriver::OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal)
{
	if (g_Exposure.CheckByName(lpstrSec, lpstrEntry))
	{
		g_Exposure.SetValueInt(nVal);
	}
	else if (g_Gain.CheckByName(lpstrSec, lpstrEntry))
	{
		g_Gain.SetValueInt(nVal);
		short nGain = (short)g_Gain;
		pl_set_param(m_hCam, PARAM_GAIN_INDEX, &nGain);
	}
	else if (g_IFPreview.CheckByName(lpstrSec, lpstrEntry) ||
		g_PreviewMode.CheckByName(lpstrSec, lpstrEntry) ||
		g_IFRect.CheckByName(lpstrSec, lpstrEntry))
		this->m_bSizeChanged = true;
}

HRESULT CPVCamDriver::OnUpdateCmdUI(HWND hwnd, int nID)
{
	if (nID == IDC_EDIT_ENH_GAIN || nID == IDC_SPIN_ENH_GAIN ||
		nID == IDC_SLIDER_ENH_GAIN)
		::EnableWindow(hwnd, m_nGainIndMax > 1);
	else if (nID == IDC_CHECK_PREVIEW_INPUT_FRAME || nID == IDC_CHECK_CAPTURE_INPUT_FRAME)
		::EnableWindow(hwnd, g_IFRect.IsEnabled());
	return __super::OnUpdateCmdUI(hwnd, nID);
}

HRESULT CPVCamDriver::OnCmdMsg(int nCode, UINT nID, long lHWND)
{
	if (nID == IDC_BUTTON_AUTOEXPOSITION)
	{
		if (nCode == BN_CLICKED)
			ProcessAutoexposure(::GetParent((HWND)lHWND));
		return S_OK;
	}
	else
		return __super::OnCmdMsg(nCode, nID, lHWND);
}

HRESULT CPVCamDriver::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_INPUTFRAME|FG_GETIMAGE_DIB;
	return S_OK;
}
