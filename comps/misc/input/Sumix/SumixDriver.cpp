#include "StdAfx.h"
#include "resource.h"
#include "SumixDriver.h"
#include "VT5Profile.h"
#include "PreviewPixDlg.h"
#include "CamValues.h"
#include "MethCombo.h"
#include "Focus.h"
#include "SettingsBase.h"
#include "MethNameDlg.h"
#include "BayerUtils.h"
#include "SMX150.h"
#include "vt4support.h"
#include <math.h>

class CSumixCamDs
{
    
public:
        
    HINSTANCE m_scamlib;

    CSumixCamDs() {
		m_scamlib = NULL;
        ResetFunctions();
//      Initialize();
    }

    virtual ~CSumixCamDs() {
        Shutdown();
    };

    CxOpenDevice_t                  CxOpenDevice;
    CxCloseDevice_t                 CxCloseDevice;
                                                               
    CxGetCameraInfo_t               CxGetCameraInfo;
    CxGetCameraInfoEx_t             CxGetCameraInfoEx;
                                                               
    CxGetStreamMode_t               CxGetStreamMode;
    CxSetStreamMode_t               CxSetStreamMode;
                                                               
    CxGetScreenParams_t             CxGetScreenParams;
    CxSetScreenParams_t             CxSetScreenParams;
    CxActivateScreenParams_t        CxActivateScreenParams;
                                                               
    CxGetFrequency_t                CxGetFrequency;
    CxSetFrequency_t                CxSetFrequency;
                                                               
    CxGetExposureMinMax_t           CxGetExposureMinMax;
    CxGetExposure_t                 CxGetExposure;
    CxSetExposure_t                 CxSetExposure;
                                                               
    CxGetExposureMinMaxMs_t         CxGetExposureMinMaxMs;
    CxGetExposureMs_t               CxGetExposureMs;
    CxSetExposureMs_t               CxSetExposureMs;
                                                               
    CxSetGain_t                     CxSetGain;
    CxGetGain_t                     CxGetGain;
                                                               
    CxGrabVideoFrame_t              CxGrabVideoFrame;
    CxGetFramePtr_t                 CxGetFramePtr;
                                                               
    CxSetBrightnessContrastGamma_t  CxSetBrightnessContrastGamma;
    CxSetConvertionTab_t            CxSetConvertionTab;
    CxGetConvertionTab_t            CxGetConvertionTab;
    CxSetDefaultConvertionTab_t   CxSetDefaultConvertionTab;
                                                               
    CxStartVideo_t                  CxStartVideo;
    CxStopVideo_t                   CxStopVideo;
    CxSetBayerAlg_t                 CxSetBayerAlg;
    CxBayerToRgb_t                  CxBayerToRgb;

    CxGetFrameCounter_t             CxGetFrameCounter;
                                                               
    CxGetSnapshotExposureMinMax_t   CxGetSnapshotExposureMinMax;
    CxGetSnapshotExposure_t         CxGetSnapshotExposure;
    CxSetSnapshotExposure_t         CxSetSnapshotExposure;
                                                               
    CxGetSnapshotExposureMinMaxMs_t CxGetSnapshotExposureMinMaxMs;
    CxGetSnapshotExposureMs_t       CxGetSnapshotExposureMs;
    CxSetSnapshotExposureMs_t       CxSetSnapshotExposureMs;

    CxSetSnapshotExposureMultiplier_t CxSetSnapshotExposureMultiplier;
    CxGetSnapshotExposureMultiplier_t CxGetSnapshotExposureMultiplier;
                                                               
    CxGetSnapshot_t                 CxGetSnapshot;
    CxCancelSnapshot_t				CxCancelSnapshot;

    CxGetDACRawOffset_t             CxGetDACRawOffset;
    CxSetDACRawOffset_t             CxSetDACRawOffset;
    CxGetDACFineOffset_t            CxGetDACFineOffset;
    CxSetDACFineOffset_t            CxSetDACFineOffset;

    CxSetContinuousPrecharge_t		CxSetContinuousPrecharge;
    CxGetContinuousPrecharge_t      CxGetContinuousPrecharge;

    CxSetTristateOut_t 				CxSetTristateOut;
    CxGetTristateOut_t				CxGetTristateOut;
                      
    CxWriteSensorReg_t              CxWriteSensorReg; 
    CxReadSensorReg_t               CxReadSensorReg; 
                                                  
    CxSetControlReg_t               CxSetControlReg; 
    CxGetControlReg_t               CxGetControlReg; 


    void Shutdown() {
        // unload the library
        if (m_scamlib != NULL) {
            ResetFunctions();
            ::FreeLibrary(m_scamlib);
            m_scamlib = NULL;
        }
    }

    BOOL Initialize() {
        // load the library
        m_scamlib = ::LoadLibrary("SMX150.ds");
    
        if (m_scamlib == NULL) {
            // AfxThrowFileException(CFileException::fileNotFound, -1, "SMX150.dll");
            return FALSE;
        };

        // init the functions as pointers from the dll

        CxOpenDevice = (CxOpenDevice_t)::GetProcAddress( m_scamlib, "CxOpenDevice" );
        CxCloseDevice = (CxCloseDevice_t)::GetProcAddress( m_scamlib, "CxCloseDevice" );
                                      
        CxGetCameraInfo = (CxGetCameraInfo_t)::GetProcAddress( m_scamlib, "CxGetCameraInfo" );
        CxGetCameraInfoEx = (CxGetCameraInfoEx_t)::GetProcAddress( m_scamlib, "CxGetCameraInfoEx" );
                                      
        CxGetStreamMode = (CxGetStreamMode_t)::GetProcAddress( m_scamlib, "CxGetStreamMode" );
        CxSetStreamMode = (CxSetStreamMode_t)::GetProcAddress( m_scamlib, "CxSetStreamMode" );
                                      
        CxGetScreenParams = (CxGetScreenParams_t)::GetProcAddress( m_scamlib, "CxGetScreenParams" );
        CxSetScreenParams = (CxSetScreenParams_t)::GetProcAddress( m_scamlib, "CxSetScreenParams" );
        CxActivateScreenParams = (CxActivateScreenParams_t)::GetProcAddress( m_scamlib, "CxActivateScreenParams" );
                                      
        CxGetFrequency = (CxGetFrequency_t)::GetProcAddress( m_scamlib, "CxGetFrequency" );
        CxSetFrequency = (CxSetFrequency_t)::GetProcAddress( m_scamlib, "CxSetFrequency" );
                                      
        CxGetExposureMinMax = (CxGetExposureMinMax_t)::GetProcAddress( m_scamlib, "CxGetExposureMinMax" );
        CxGetExposure = (CxGetExposure_t)::GetProcAddress( m_scamlib, "CxGetExposure" );
        CxSetExposure = (CxSetExposure_t)::GetProcAddress( m_scamlib, "CxSetExposure" );
                                      
        CxGetExposureMinMaxMs = (CxGetExposureMinMaxMs_t)::GetProcAddress( m_scamlib, "CxGetExposureMinMaxMs" );
        CxGetExposureMs = (CxGetExposureMs_t)::GetProcAddress( m_scamlib, "CxGetExposureMs" );
        CxSetExposureMs = (CxSetExposureMs_t)::GetProcAddress( m_scamlib, "CxSetExposureMs" );
                                      
        CxSetGain = (CxSetGain_t)::GetProcAddress( m_scamlib, "CxSetGain" );
        CxGetGain = (CxGetGain_t)::GetProcAddress( m_scamlib, "CxGetGain" );
                                      
        CxGrabVideoFrame = (CxGrabVideoFrame_t)::GetProcAddress( m_scamlib, "CxGrabVideoFrame" );
        CxGetFramePtr = (CxGetFramePtr_t)::GetProcAddress( m_scamlib, "CxGetFramePtr" );
                                      
        CxSetBrightnessContrastGamma = (CxSetBrightnessContrastGamma_t)::GetProcAddress( m_scamlib, "CxSetBrightnessContrastGamma" );
        CxSetConvertionTab = (CxSetConvertionTab_t)::GetProcAddress( m_scamlib, "CxSetConvertionTab" );
        CxGetConvertionTab = (CxGetConvertionTab_t)::GetProcAddress( m_scamlib, "CxGetConvertionTab" );
        CxSetDefaultConvertionTab = (CxSetDefaultConvertionTab_t)::GetProcAddress( m_scamlib, "CxSetDefaultConvertionTab" );
                                      
        CxStartVideo = (CxStartVideo_t)::GetProcAddress( m_scamlib, "CxStartVideo" );
        CxStopVideo = (CxStopVideo_t)::GetProcAddress( m_scamlib, "CxStopVideo" );
        CxSetBayerAlg = (CxSetBayerAlg_t)::GetProcAddress( m_scamlib, "CxSetBayerAlg" );
        CxBayerToRgb = (CxBayerToRgb_t)::GetProcAddress( m_scamlib, "CxBayerToRgb" );
                     
        CxGetFrameCounter = (CxGetFrameCounter_t)::GetProcAddress( m_scamlib, "CxGetFrameCounter" );
                              
        CxGetSnapshotExposureMinMax = (CxGetSnapshotExposureMinMax_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMinMax" );
        CxGetSnapshotExposure = (CxGetSnapshotExposure_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposure" );
        CxSetSnapshotExposure = (CxSetSnapshotExposure_t)::GetProcAddress( m_scamlib, "CxSetSnapshotExposure" );
                              
        CxGetSnapshotExposureMinMaxMs = (CxGetSnapshotExposureMinMaxMs_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMinMaxMs" );
        CxGetSnapshotExposureMs = (CxGetSnapshotExposureMs_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMs" );
        CxSetSnapshotExposureMs = (CxSetSnapshotExposureMs_t)::GetProcAddress( m_scamlib, "CxSetSnapshotExposureMs" );

        CxSetSnapshotExposureMultiplier = (CxSetSnapshotExposureMultiplier_t)::GetProcAddress( m_scamlib, "CxSetSnapshotExposureMultiplier" );
        CxGetSnapshotExposureMultiplier = (CxGetSnapshotExposureMultiplier_t)::GetProcAddress( m_scamlib, "CxGetSnapshotExposureMultiplier" );
                              
        CxGetSnapshot = (CxGetSnapshot_t)::GetProcAddress( m_scamlib, "CxGetSnapshot" );
        CxCancelSnapshot = (CxCancelSnapshot_t)::GetProcAddress( m_scamlib, "CxCancelSnapshot" );

        CxGetDACRawOffset   = (CxGetDACRawOffset_t)::GetProcAddress( m_scamlib, "CxGetDACRawOffset" );
        CxSetDACRawOffset   = (CxSetDACRawOffset_t)::GetProcAddress( m_scamlib, "CxSetDACRawOffset" );
        CxGetDACFineOffset  = (CxGetDACFineOffset_t)::GetProcAddress( m_scamlib, "CxGetDACFineOffset" );
        CxSetDACFineOffset  = (CxSetDACFineOffset_t)::GetProcAddress( m_scamlib, "CxSetDACFineOffset" );

        CxSetContinuousPrecharge = (CxSetContinuousPrecharge_t)::GetProcAddress( m_scamlib, "CxSetContinuousPrecharge");
        CxGetContinuousPrecharge = (CxGetContinuousPrecharge_t)::GetProcAddress( m_scamlib, "CxGetContinuousPrecharge");

        CxSetTristateOut = (CxSetTristateOut_t)::GetProcAddress( m_scamlib, "CxSetTristateOut");
        CxGetTristateOut = (CxGetTristateOut_t)::GetProcAddress( m_scamlib, "CxGetTristateOut");

        CxWriteSensorReg =  (CxWriteSensorReg_t)::GetProcAddress( m_scamlib, "CxWriteSensorReg");
        CxReadSensorReg  =  (CxReadSensorReg_t)::GetProcAddress( m_scamlib, "CxReadSensorReg");
                                                                
        CxSetControlReg  =  (CxSetControlReg_t)::GetProcAddress( m_scamlib, "CxSetControlReg");
        CxGetControlReg  =  (CxGetControlReg_t)::GetProcAddress( m_scamlib, "CxGetControlReg");


        return TRUE;
    }


private:

    void ResetFunctions() {

        CxOpenDevice = NULL;                 
        CxCloseDevice = NULL;                
                                      
        CxGetCameraInfo = NULL;              
        CxGetCameraInfoEx = NULL;            
                                      
        CxGetStreamMode = NULL;              
        CxSetStreamMode = NULL;              
                                      
        CxGetScreenParams = NULL;            
        CxSetScreenParams = NULL;            
        CxActivateScreenParams = NULL;       
                                      
        CxGetFrequency = NULL;               
        CxSetFrequency = NULL;               
                                      
        CxGetExposureMinMax = NULL;          
        CxGetExposure = NULL;                
        CxSetExposure = NULL;                
                                      
        CxGetExposureMinMaxMs = NULL;        
        CxGetExposureMs = NULL;              
        CxSetExposureMs = NULL;              
                                      
        CxSetGain = NULL;                    
        CxGetGain = NULL;                    
                                      
        CxGrabVideoFrame = NULL;             
        CxGetFramePtr = NULL;                
                                      
        CxSetBrightnessContrastGamma = NULL;
        CxSetConvertionTab = NULL;
        CxGetConvertionTab = NULL;
        CxSetDefaultConvertionTab = NULL;
                                      
        CxStartVideo = NULL;                 
        CxStopVideo = NULL;
        CxSetBayerAlg = NULL;
        CxBayerToRgb = NULL;

        CxGetFrameCounter = NULL;
                                    
        CxGetSnapshotExposureMinMax = NULL;  
        CxGetSnapshotExposure = NULL;        
        CxSetSnapshotExposure = NULL;        
                                      
        CxGetSnapshotExposureMinMaxMs = NULL;
        CxGetSnapshotExposureMs = NULL;      
        CxSetSnapshotExposureMs = NULL;

        CxSetSnapshotExposureMultiplier = NULL;
        CxGetSnapshotExposureMultiplier = NULL;
                                      
        CxGetSnapshot = NULL;
        CxCancelSnapshot = NULL;

        CxGetDACRawOffset   = NULL;
        CxSetDACRawOffset   = NULL;
        CxGetDACFineOffset  = NULL;
        CxSetDACFineOffset  = NULL;

        CxSetContinuousPrecharge = NULL;
        CxGetContinuousPrecharge = NULL;

        CxSetTristateOut = NULL;
        CxGetTristateOut = NULL;

        CxWriteSensorReg = NULL; 
        CxReadSensorReg  = NULL; 
                            
        CxSetControlReg  = NULL; 
        CxGetControlReg  = NULL; 
    }   
};      


struct CSumixParams
{
	CSumixCamDs m_Cam;
	HANDLE m_hDev;
	TCameraInfo m_CamInfo;
	TFrameParams m_Params;

	CSumixParams()
	{
		m_hDev = NULL;
	}
};

CCamIntValue2 g_Exposure(_T("Parameters"), _T("Exposure"), 10000, 1, 100000, 10, 1, 0, IDC_EDIT_EXPOSURE);
CCamIntValue g_Gain(_T("Parameters"), _T("Gain"), 0, 0, 16, IRepr_SmartIntEdit, 0, IDC_EDIT_GAIN);
CCamIntValue g_Gamma1(_T("Parameters"), _T("Gamma"), 0, -127, 127, IRepr_SmartIntEdit, 0, IDC_EDIT_ENH_GAMMA);
//CCamIntComboValue2  g_PreviewMode(_T("Preview"), _T("Mode"), 0, 0, -1,
//	CCamValue::WriteToReg|CCamValue::SkipReinitCamera);
CCamIntValue g_DACRawOffset(_T("Parameters"), _T("DACRawOffset"), 5, 0, 127, IRepr_SmartIntEdit, 0, IDC_EDIT_ENH_DAC_RAW_OFFSET);
CCamIntValue g_DACFineOffset(_T("Parameters"), _T("DACFineOffset"), 30, 0, 127, IRepr_SmartIntEdit, 0, IDC_EDIT_ENH_DAC_FINE_OFFSET);
CCamIntValue g_ColorAlgorythm(_T("Parameters"), _T("ColorAlgorythm"), 3, 0, 5);
CCamBoolValue g_DeadPxls(_T("Parameters"), _T("CorrectDeadPixels"), false, 0, -1, CCamValue::WriteToReg|CCamValue::IgnoreMethodic);
CDeadPxlMask g_DeadPxlMask;
static void EmptyMask()
{
//	g_DeadPxlMask.Empty();
//	g_DeadPxls = false;
}

INTCOMBOITEMDESCR s_aModes[] =
{
	{0, _T("1280x1024")},
	{1, _T("1024x768")},
	{2, _T("768x576")},
	{3, _T("640x480")},
};
CCamIntComboValue  g_CaptureMode(_T("Capture"), _T("Mode"), s_aModes, sizeof(s_aModes)/sizeof(s_aModes[0]),
	0, 0, IDC_COMBO_CAPTURE);

CCamIntValue g_ColorBalanceRed(_T("Color balance"), _T("Red"), 60, 0, 120, IRepr_SmartIntEdit,
	0, IDC_EDIT_WB_RED);
CCamIntValue g_ColorBalanceGreen(_T("Color balance"), _T("Green"), 60, 0, 120, IRepr_SmartIntEdit,
	0, IDC_EDIT_WB_GREEN);
CCamIntValue g_ColorBalanceBlue(_T("Color balance"), _T("Blue"), 60, 0, 120, IRepr_SmartIntEdit,
	0, IDC_EDIT_WB_BLUE);
CCamIntValue       g_ColorBalanceAuto(_T("Color balance"), _T("Auto"), 1, 0, 1, IRepr_Radiobuttons,
	0, IDC_WB_AUTO);

CCamBoolValue      g_CaptureSnapshot(_T("Image"), _T("UseSnapshotForCapture"), false);
CCamBoolValue      g_ExtEnhanceDlg(_T("Extended dialogs"), _T("Enhancement"), false);

CCamIntValue g_Frequency(_T("Parameters"), _T("Frequency"), FREQ_40, FREQ_40, FREQ_6, IRepr_SmartIntEdit, 0, -1);


CValuesGroup ValuesSizes;

static bool s_bSizesChanged = false;


CSumixDriver::CSumixDriver(void)
{
	m_pParams = new CSumixParams;
	m_CamState = NotInited;
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
//	g_Brightness.SetId(IDC_EDIT_ENH_BRIGHTNESS);
//	g_Contrast.SetId(IDC_EDIT_ENH_CONTRAST);
//	g_Gamma.SetId(IDC_EDIT_ENH_GAMMA);
	InitVideo(IDC_FPS, IDC_TIME_LIM, IDC_FILE, IDC_BROWSE, IDS_FILTER);
	g_VideoDest.SetId(IDC_FILE);
	g_StringsIds.idsVideoCaptured = IDS_VIDEO_CAPTURED;
	g_StringsIds.idsFileNotOpened = IDS_FILE_NOT_OPENED;
	ValuesSizes.Add(&g_CaptureMode);
	m_bDarkField = false;
}

CSumixDriver::~CSumixDriver(void)
{
	delete m_pParams;
}

IUnknown *CSumixDriver::DoGetInterface( const IID &iid )
{
	IUnknown *p;
	if (p = CDriverPreview::DoGetInterface(iid))
		return p;
	else
		return ComObjectBase::DoGetInterface(iid);
}


bool CSumixDriver::InitCamera()
{
	if (!m_pParams->m_Cam.Initialize())
		return false;
	m_pParams->m_hDev = m_pParams->m_Cam.CxOpenDevice(0);
	if (m_pParams->m_hDev == NULL)
	{
		m_pParams->m_Cam.Shutdown();
		return false;
	}
	if (!m_pParams->m_Cam.CxGetCameraInfo(m_pParams->m_hDev, &m_pParams->m_CamInfo))
	{
		m_pParams->m_Cam.CxCloseDevice(m_pParams->m_hDev);
		m_pParams->m_hDev = NULL;
		m_pParams->m_Cam.Shutdown();
		return false;
	}
	float fMin,fMax;
	if (m_pParams->m_Cam.CxGetExposureMinMaxMs(m_pParams->m_hDev, &fMin, &fMax))
	{
//		g_Exposure.InitScope((int)(fMin*1000.), (int)(fMax*1000.), (int)(fMin*1000.));
		g_Exposure.InitScope(100, 100000, 10000);
		g_Exposure.Init();
	}
	m_pParams->m_Cam.CxSetFrequency(m_pParams->m_hDev, (BYTE)(int)g_Frequency);
	if (m_pParams->m_CamInfo.SensorType == 0)
		g_Gray.SetValueInt(1);
	if ((bool)g_DeadPxls)
		g_DeadPxlMask.ReadPxlMask();
	return true;
}

void CSumixDriver::DeinitCamera()
{
	m_pParams->m_Cam.CxCloseDevice(m_pParams->m_hDev);
	m_pParams->m_hDev = NULL;
	m_pParams->m_Cam.Shutdown();
}

/*class _CPrepDarkField
{
	CSumixDriver *m_p;
public:
	TFrameParams Params;
	BOOL m_bOk;
	_CPrepDarkField(CSumixDriver *p)
	{
		m_p = p;
		m_p->Stop();
		if (m_p->m_pParams->m_Cam.CxGetScreenParams(m_p->m_pParams->m_hDev,&Params))
			m_bOk = TRUE;
		else
			m_bOk = FALSE;
	}
	~_CPrepDarkField()
	{
		if (m_bOk)
			m_p->m_pParams->m_Cam.CxSetScreenParams(m_p->m_pParams->m_hDev,&Params);
		m_p->Start();
	}
};*/

static void InitFrameParams(TFrameParams &Params, int w, int h)
{
	Params.StartX = (1280-w)/2;
	Params.StartY = (1024-h)/2;
	Params.Width = w;
	Params.Height = h;
}

bool CSumixDriver::WhiteBalanceStep(double &dRed, double &dGreen, double &dBlue)
{
	if (!WaitNextFrame(1, 1000, 2))
		return false;
	CDriverSynch s(this);
	if (m_Dib.IsEmpty())
		return false;
	CalAvrBrightness(m_Dib.m_lpbi, &dRed, &dGreen, &dBlue);
	return true;
}


void CSumixDriver::MakeWhitBalance()
{
	if (m_pParams == NULL || m_pParams->m_hDev == NULL)
		return;
	DWORD dwTimeout = max((int)g_Exposure, 100);
	m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, 0, 0, 0);
	double dRed,dGreen,dBlue;
	if (!WhiteBalanceStep(dRed, dGreen, dBlue))
		return;
//	TRACE("MakeWhitBalance started. dRed = %g, dGreen = %g, dBlue = %g\n", dRed, dGreen, dBlue);
	int nMinR = 0, nMaxR = 120, nMinG = 0, nMaxG = 120, nMinB = 0, nMaxB = 120;
	double dMax;
	if (dRed >= dGreen && dRed >= dBlue)
	{
		dMax = dRed;
		nMaxR = 0;
	}
	else if (dGreen>=dRed&&dGreen>=dBlue)
	{
		dMax = dGreen;
		nMaxG = 0;
	}
	else
	{
		dMax = dBlue;
		nMaxB = 0;
	}
	while (nMaxR > nMinR+1 || nMaxG > nMinG+1 || nMaxB > nMinB+1)
	{
//		TRACE("nMinR = %d, nMaxR = %d, nMinG = %d, nMaxG = %d, nMinB = %d, nMaxB = %d\n", nMinR, nMaxR, nMinG, nMaxG, nMinB, nMaxB);
//		TRACE("R = %d, G = %d, B = %d\n", (nMinR+nMaxR)/2, (nMinG+nMaxG)/2, (nMinB+nMaxB)/2);
		m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, (nMinR+nMaxR)/2, (nMinG+nMaxG)/2,
			(nMinB+nMaxB)/2);
		if (!WhiteBalanceStep(dRed, dGreen, dBlue))
			return;
//		TRACE("dRed = %g, dGreen = %g, dBlue = %g\n", dRed, dGreen, dBlue);
		if (nMaxR > nMinR+1)
		{
			if (dRed > dMax)
				nMaxR = (nMinR+nMaxR)/2;
			else
				nMinR = (nMinR+nMaxR)/2;
		}
		if (nMaxG > nMinG+1)
		{
			if (dGreen > dMax)
				nMaxG = (nMinG+nMaxG)/2;
			else
				nMinG = (nMinG+nMaxG)/2;
		}
		if (nMaxB > nMinB+1)
		{
			if (dBlue > dMax)
				nMaxB = (nMinB+nMaxB)/2;
			else
				nMinB = (nMinB+nMaxB)/2;
		}
	}
	m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, nMinR, nMinG, nMinB);
	g_ColorBalanceRed = nMinR;
	g_ColorBalanceGreen = nMinG;
	g_ColorBalanceBlue = nMinB;
}

bool CSumixDriver::DACFineOffsetStep(double &dDiff)
{
	if (!WaitNextFrame(1, 1000, 2))
		return false;
	CDriverSynch s(this);
	if (m_Dib.IsEmpty())
		return false;
	double dRedBri1,dRedBri2,dGreen1,dGreen2,dBlue1,dBlue2;
	CalAvrBrightnessFields(m_Dib.m_lpbi, &dRedBri1, &dRedBri2, &dGreen1, &dGreen2, &dBlue1, &dBlue2);
	if (m_Dib.bpp() < 24)
		dDiff = fabs(dRedBri2-dRedBri1);
	else
	{
		double dDiffR = fabs(dRedBri2-dRedBri1);
		double dDiffG = fabs(dGreen2-dGreen1);
		double dDiffB = fabs(dBlue2-dBlue1);
		dDiff = max(dDiffR,max(dDiffG,dDiffB));
	}
	return true;
}

void CSumixDriver::MakeDACFineOffset()
{
	if (m_pParams == NULL || m_pParams->m_hDev == NULL)
		return;
	double dMinDiff = 300.;
	BYTE byMinDiffValue = 255;
	for (BYTE byOffset = 0; byOffset < 128; byOffset+=8)
	{
		m_pParams->m_Cam.CxSetDACFineOffset(m_pParams->m_hDev, byOffset);
		double dDiff;
		if (!DACFineOffsetStep(dDiff))
			return;
		if (dDiff < dMinDiff)
		{
			dMinDiff = dDiff;
			byMinDiffValue = byOffset;
		}
	}
	if (byMinDiffValue == 255) return;
	BYTE byOffsetMin = byMinDiffValue<8?0:byMinDiffValue-8;
	BYTE byOffsetMax = byMinDiffValue>119?127:byMinDiffValue+8;
	for (byOffset = byOffsetMin; byOffset <= byOffsetMax; byOffset++)
	{
		m_pParams->m_Cam.CxSetDACFineOffset(m_pParams->m_hDev, byOffset);
		double dDiff;
		if (!DACFineOffsetStep(dDiff))
			return;
		if (dDiff < dMinDiff)
		{
			dMinDiff = dDiff;
			byMinDiffValue = byOffset;
		}
	}
	m_pParams->m_Cam.CxSetDACFineOffset(m_pParams->m_hDev, byMinDiffValue);
	g_DACFineOffset = byMinDiffValue;
}


HRESULT CSumixDriver::OnCmdMsg(int nCode, UINT nID, long lHWND)
{
	if (nID == IDC_WB_BUILD)
	{
		CWaitCursor wc;
		MakeWhitBalance();
		return S_OK;
	}
	else if (nID == IDC_DARK_FIELD)
	{
		{
			CDriverSynch DriverSynch(this);
			g_DeadPxlMask.m_Dib.InitBitmap(m_Dib.cx(), m_Dib.cy(), m_Dib.bpp());
			m_pParams->m_Cam.CxGrabVideoFrame(m_pParams->m_hDev, g_DeadPxlMask.m_Dib.GetData(),
				g_DeadPxlMask.m_Dib.Size(false));
			g_DeadPxlMask.WritePxlMask();
			if (!(bool)g_DeadPxls)
				g_DeadPxls = true;
		}
		return S_OK;
	}
	else if (nID == IDC_BUTTON_AUTO_FINE_OFFSET)
	{
		CWaitCursor wc;
		MakeDACFineOffset();
		return S_OK;
	}
	else if (nID == IDC_BUTTON_DEFAULT_GAIN_GAMMA)
	{
		g_Gain = 0;
		g_Gamma1 = 0;
		return S_OK;
	}
	return __super::OnCmdMsg(nCode, nID, lHWND);
}

HRESULT CSumixDriver::OnUpdateCmdUI(HWND hwnd, int nID)
{
	if (nID == IDC_EDIT_WB_RED || nID == IDC_SPIN_WB_RED || nID == IDC_SLIDER_WB_RED ||
		nID == IDC_EDIT_WB_GREEN || nID == IDC_SPIN_WB_GREEN || nID == IDC_SLIDER_WB_GREEN ||
		nID == IDC_EDIT_WB_BLUE || nID == IDC_SPIN_WB_BLUE || nID == IDC_SLIDER_WB_BLUE)
		::EnableWindow(hwnd, (int)g_Gray==0 && g_ColorBalanceAuto==1);
	else if (nID == IDC_WB_BUILD)
		::EnableWindow(hwnd, (int)g_Gray==0 && (int)g_ColorBalanceAuto==0);
	else if (nID == IDC_WB_AUTO || nID == IDC_WB_MANUAL || nID == IDC_COLOR_BALANCE)
		::EnableWindow(hwnd, (int)g_Gray==0);
	else if (nID == IDC_CHECK_GRAYSCALE)
		::EnableWindow(hwnd, m_pParams->m_CamInfo.SensorType);
	else if (nID == IDC_COLOR_BALANCE || nID == IDC_CHECK_GRAYSCALE)
		::EnableWindow(hwnd, m_pParams->m_CamInfo.SensorType);
	return __super::OnUpdateCmdUI(hwnd, nID);
}

HRESULT CSumixDriver::GetControlInfo(int nID, VALUEINFOEX2 *pValueInfoEx2)
{
	if (nID == IDC_BUTTON_DEFAULT_GAIN_GAMMA)
		return S_OK;
	else
		return __super::GetControlInfo(nID, pValueInfoEx2);
}

bool CSumixDriver::OnExecuteSettings2(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
	BOOL bFirst, BOOL bForInput, DWORD dwFlags)
{
	CPreviewIds PreviewIds = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG, IDC_STATIC_PERIOD}, 
		IDC_STATIC_PERIOD, IDC_IMAGE, IDC_ENHANCEMENT, IDC_VIDEO_SETTINGS, -1, -1, IDC_HISTOGRAM,
		IDS_FPS, IDS_FRAME_PERIOD};
	int iddPreview = IDD_PREVIEW_FRAME;
	if (dwFlags&ES2_VIDEO)
	{
		PreviewIds.m_idProgressVideoStage = IDC_PROGRESS_VIDEO_STAGE;
		PreviewIds.m_idStaticVideoStage = IDC_STATIC_VIDEO_STAGE;
		iddPreview = IDD_PREVIEW_VIDEO;
	}
	DWORD dwDlgFlags = (dwFlags&ES2_VIDEO)?CPreviewPixDlg::ForAvi:CVT4Drv::s_bVT5Settings?CPreviewPixDlg::ForSettings:0;
	CPreviewPixDlg  dlg(iddPreview,PreviewIds,(IInputPreview *)this,nDevice,dwDlgFlags,CWnd::FromHandle(hwndParent));
	if ((bool)g_ExtEnhanceDlg)
		dlg.AttachChildDialogToButton(IDC_ENHANCEMENT, IDD_ENHANCEMENT_EX);
	else
		dlg.AttachChildDialogToButton(IDC_ENHANCEMENT, IDD_ENHANCEMENT);
	dlg.AttachChildDialogToButton(IDC_IMAGE_PAGE, IDD_IMAGE);
	dlg.AttachChildDialogToButton(IDC_COLOR_BALANCE, IDD_COLOR_BALANCE);
	dlg.AttachChildDialogToButton(IDC_SETTINGS_PAGE, IDD_SETTINGS);
	dlg.AttachChildDialogToButton(IDC_VIDEO_SETTINGS, IDD_VIDEO);
	return dlg.DoModal() == IDOK;
}

HRESULT CSumixDriver::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_INPUTFRAME|FG_SUPPORTSVIDEO;
	return S_OK;
}

HRESULT CSumixDriver::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	CSettingsIds Ids = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG},
		IDC_TAB_SELECT, 0};
	CSettingsBase dlg((IDriver*)this, 0, IDD_DIALOG_SETTINGS, Ids, NULL);
	dlg.AddPane(IDS_BRIGHTNESS, IDD_SHUTTER);
	if ((bool)g_ExtEnhanceDlg)
		dlg.AddPane(IDS_ENHANCEMENT, IDD_ENHANCEMENT);
	else
		dlg.AddPane(IDS_ENHANCEMENT, IDD_ENHANCEMENT_EX);
	dlg.AddPane(IDS_IMAGE, IDD_IMAGE);
	dlg.AddPane(IDS_WHITE_BALANCE, IDD_COLOR_BALANCE);
	dlg.AddPane(IDS_SETTINGS, IDD_SETTINGS);
	dlg.DoModal();
	return S_OK;
}

bool CSumixDriver::OnStartGrab()
{
	m_Buff8.alloc(DIBSize(m_Dib.cx(),m_Dib.cy(),8,false));
	m_Buff24.alloc(DIBSize(m_Dib.cx(),m_Dib.cy(),24,false));
	Start();
	return true;
}

void CSumixDriver::OnStopGrab()
{
	Stop();
	_KillTimer();
	m_Buff24.free();
	m_Buff8.free();
	m_pParams->m_Cam.CxSetStreamMode(m_pParams->m_hDev, 0);
}

static void CalcOffsets(int w1, int h1, int w2, int h2, int &x, int &y)
{
	x = (w1-w2)/2;
	y = (h1-h2)/2;
}

bool CSumixDriver::PreinitGrab2(CCaptureMode cm, DWORD dwParams)
//bool CSumixDriver::PreinitGrab(bool bCapture)
{
	TFrameParams Params;
	if (m_pParams->m_Cam.CxGetScreenParams(m_pParams->m_hDev,&Params))
	{
		if (g_CaptureMode == 1)
			InitFrameParams(Params, 1024, 768);
		else if (g_CaptureMode == 2)
			InitFrameParams(Params, 768, 576);
		else if (g_CaptureMode == 3)
			InitFrameParams(Params, 640, 480);
		else
			InitFrameParams(Params, 1280, 1024);
		m_pParams->m_Cam.CxSetScreenParams(m_pParams->m_hDev,&Params);
		if (!g_DeadPxlMask.IsEmpty() && (bool)g_DeadPxls && Params.Width != g_DeadPxlMask.cx() && Params.Height != g_DeadPxlMask.cy())
		{
			g_DeadPxls = false;
			g_DeadPxlMask.Empty();
		}
	}
	if (cm == CaptureMode_Snapshot && (bool)g_CaptureSnapshot)
	{
		m_pParams->m_Cam.CxSetFrequency(m_pParams->m_hDev, FREQ_16);
		float ExpMs = ((float)(int)g_Exposure)/(float)1000.;
		m_pParams->m_Cam.CxSetSnapshotExposureMs(m_pParams->m_hDev, ExpMs, &ExpMs);
	}
	else
	{
		if (!m_pParams->m_Cam.CxSetStreamMode(m_pParams->m_hDev, 1))
			return false;
		float fSet = ((float)(int)g_Exposure)/(float)1000.;
		m_pParams->m_Cam.CxSetExposureMs(m_pParams->m_hDev, fSet, &fSet);
	}
	if (g_Gray)
		m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, 0, 0, 0);
	else
		m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, (int)g_ColorBalanceRed,
			(int)g_ColorBalanceGreen, (int)g_ColorBalanceBlue);
	m_pParams->m_Cam.CxSetBrightnessContrastGamma(m_pParams->m_hDev, 0, 0, (int)g_Gamma1);
	m_pParams->m_Cam.CxSetDACRawOffset(m_pParams->m_hDev, (BYTE)(int)g_DACRawOffset);
	m_pParams->m_Cam.CxSetDACFineOffset(m_pParams->m_hDev, (BYTE)(int)g_DACFineOffset);
	_SetTimer(100);
	return true;
}

bool CSumixDriver::GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi)
{
	TFrameParams Params;
	if (!m_pParams->m_Cam.CxGetScreenParams(m_pParams->m_hDev,&Params))
		return false;
	InitDIB(lpbi, Params.Width, Params.Height, 8, false);
	return true;
}

static void ConvertData24To8(TRgbPix *pSrc, LPBYTE pDst, int nSize)
{
	for (int i = 0; i < nSize; i++)
		pDst[i] = pSrc[i].g;
}

static void CaptureDIBData(CDib &Dib, CSumixParams *pParams, LPBYTE lpDst, DWORD dwSize, bool bUseSnapshot)
{
	DWORD dwSizeRet;
	if (bUseSnapshot)
		pParams->m_Cam.CxGetSnapshot(pParams->m_hDev, 5, FALSE, FALSE, TRUE, lpDst, dwSize, &dwSizeRet);
	else
		pParams->m_Cam.CxGrabVideoFrame(pParams->m_hDev, lpDst, dwSize);
	if ((bool)g_DeadPxls)
	{
		int x,y;
		CalcOffsets(g_DeadPxlMask.cx(), g_DeadPxlMask.cy(), Dib.cx(), Dib.cy(), x, y);
		g_DeadPxlMask.CorrectBayerDib(Dib.m_lpbi, lpDst, 8, x, y);
	}
}

static void CaptureToDib(CDib &Dib, _ptr_t2<BYTE> &Buff8, _ptr_t2<BYTE> &Buff24,
	CSumixParams *pParams, bool bUseSnapshot)
{
	DWORD dwSize = Dib.Size(false);
	LPVOID lpDataDst = Dib.GetData();
	if (Dib.bpp() == 8)
	{
		if (pParams->m_CamInfo.SensorType)
		{
			CaptureDIBData(Dib, pParams, (LPBYTE)Buff8, Buff8.size(), bUseSnapshot);
			pParams->m_Cam.CxBayerToRgb((LPBYTE)Buff8, Dib.cx(), Dib.cy(), (int)g_ColorAlgorythm,
					(TRgbPix*)(BYTE*)Buff24);
			ConvertData24To8((TRgbPix*)(BYTE*)Buff24, (LPBYTE)lpDataDst, Dib.cx()*Dib.cy());
		}
		else
			CaptureDIBData(Dib, pParams, (LPBYTE)lpDataDst, dwSize, bUseSnapshot);
	}
	else if (Dib.bpp() == 24)
	{
		CaptureDIBData(Dib, pParams, (LPBYTE)Buff8, Buff8.size(), bUseSnapshot);
		pParams->m_Cam.CxBayerToRgb((LPBYTE)Buff8, Dib.cx(), Dib.cy(), (int)g_ColorAlgorythm,
			(TRgbPix*)lpDataDst);
	}
	if ((bool)g_HorzMirror || (bool)g_VertMirror)
		Dib.Flip((bool)g_HorzMirror, (bool)g_VertMirror);
}

bool CSumixDriver::CanCaptureDuringPreview()
{
	return !(bool)g_CaptureSnapshot;
}

bool CSumixDriver::CanCaptureFromSettings()
{
	return !(bool)g_CaptureSnapshot;
}

void CSumixDriver::ProcessCapture()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	TFrameParams Params;
	if (!m_pParams->m_Cam.CxGetScreenParams(m_pParams->m_hDev,&Params))
		return;
	CDib Dib;
	Dib.InitBitmap(Params.Width, Params.Height, (bool)g_Gray?8:24);
	_ptr_t2<BYTE> Buff8,Buff24;
	if ((bool)g_Gray)
		Buff24.alloc(DIBSize(Dib.cx(),Dib.cy(),24,false));
	Buff8.alloc(DIBSize(Dib.cx(),Dib.cy(),8,false));
	Sleep(600);
	CaptureToDib(Dib, Buff8, Buff24, m_pParams, (bool)g_CaptureSnapshot);
	if (m_pimgCap != NULL)
		AttachDIBBits(m_pimgCap, Dib.m_lpbi, (LPBYTE)Dib.GetData());
	else if (m_lpbiCap != NULL)
	{
		DWORD dw1 = Dib.Size(false);
		DWORD dw2 = ::DIBSize(m_lpbiCap, false);
		memcpy(::DIBData(m_lpbiCap), Dib.GetData(), min(dw1,dw2));
	}
	if (!(bool)g_CaptureSnapshot)
		m_pParams->m_Cam.CxSetStreamMode(m_pParams->m_hDev, 0);
}

void CSumixDriver::OnSetValueDouble(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dVal)
{
//	if (_tccmp(lpstrSec, g_ColorBalanceRed.GetHdr()->pszSection)==0)
//		mexSetWhiteBalance(m_CamGuid, (double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
//			(double)g_ColorBalanceBlue);
}

void CSumixDriver::OnResetValues()
{
//	mexSetWhiteBalance(m_CamGuid, (double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
//		(double)g_ColorBalanceBlue);
}

void CSumixDriver::ThreadRoutine()
{
	while (m_lStopFlag == 0)
	{
		TFrameParams Params;
		if (m_pParams->m_Cam.CxGetScreenParams(m_pParams->m_hDev,&Params))
		{
			if (Params.Height == m_Dib.cy() && Params.Width == m_Dib.cx())
			{
				if (m_bMakeVideo)
				{
					CDriverSynch csa(this);
//					CCritSectionAccess csa(&m_csCap);
					LPBYTE lpData = (LPBYTE)m_Dib.GetData();
					DWORD dwSize = m_Dib.Size(false);
					m_pParams->m_Cam.CxGrabVideoFrame(m_pParams->m_hDev, lpData, dwSize);
					MakeVideoFrame(lpData, dwSize);
				}
				else
				{
					CDriverSynch csa(this);
					CaptureToDib(m_Dib, m_Buff8, m_Buff24, m_pParams, false);
				}
				OnPrvFrameReady();
				//sergey 30/10/06
			    Sleep(0);
				//Sleep(100);
				//end
			}
			else
			{
				s_bSizesChanged = true;
				Sleep(100);
			}
		}
		else
			Sleep(100);
	}
}

void CSumixDriver::OnChangeVideoMode()
{
	ValuesSizes.InitSection(m_bVideoMode?_T("Video"):_T("Capture"));
	g_DeadPxlMask.SetVideoMode(m_bVideoMode);
}


bool CSumixDriver::GetVideoFormats(CCaptureVideoToMemory *pCap)
{
	pCap->biPrv = *m_Dib.m_lpbi;
	pCap->biDst = *m_Dib.m_lpbi;
	pCap->biPrv.biBitCount = 8;
	pCap->dwFrameSizeSrc = ::DIBSize(&pCap->biPrv, false);
	return true;
}

void CSumixDriver::ConvertVideoFrame(LPBYTE lpDataSrc, CDib &Dib)
{
	DWORD dwSize = Dib.Size(false);
	LPVOID lpDataDst = Dib.GetData();
	if (Dib.bpp() == 8)
	{
		if (m_pParams->m_CamInfo.SensorType)
		{
			_ptr_t2<byte> Buff24(Dib.cx()*Dib.cy()*sizeof(TRgbPix));
			m_pParams->m_Cam.CxBayerToRgb(lpDataSrc, Dib.cx(), Dib.cy(), (int)g_ColorAlgorythm,
					(TRgbPix*)(BYTE*)Buff24);
			ConvertData24To8((TRgbPix*)(BYTE*)Buff24, (LPBYTE)lpDataDst, Dib.cx()*Dib.cy());
		}
		else
			memcpy(lpDataDst, lpDataSrc, dwSize);
	}
	else if (Dib.bpp() == 24)
	{
		m_pParams->m_Cam.CxBayerToRgb(lpDataSrc, Dib.cx(), Dib.cy(), (int)g_ColorAlgorythm,
			(TRgbPix*)lpDataDst);
	}
	if (((bool)g_HorzMirror || (bool)g_VertMirror))
		Dib.Flip((bool)g_HorzMirror, (bool)g_VertMirror);
}


void CSumixDriver::OnTimer()
{
	if (s_bSizesChanged)
	{
		NotifyChangeSizes();
		s_bSizesChanged = false;
	}
}

HRESULT CSumixDriver::SetValueInt(int nDev, BSTR bstrSec, BSTR bstrEntry, int nValue, UINT dwFlags)
{
	if (g_Exposure.CheckByName(bstrSec, bstrEntry))
	{
		g_Exposure.SetValueInt(nValue);
		float fSet = ((float)(int)g_Exposure)/(float)1000.;
		if (m_pParams && m_pParams->m_hDev != NULL) m_pParams->m_Cam.CxSetExposureMs(m_pParams->m_hDev, fSet, &fSet);
		EmptyMask();
		return S_OK;
	}
	else if (g_Gain.CheckByName(bstrSec, bstrEntry))
	{
		g_Gain.SetValueInt(nValue);
		int nGainMain,nGainR,nGainG,nGainB;
		if (m_pParams && m_pParams->m_hDev != NULL) m_pParams->m_Cam.CxGetGain(m_pParams->m_hDev, &nGainMain, &nGainR, &nGainG, &nGainB);
		if (m_pParams && m_pParams->m_hDev != NULL) m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, nGainR, nGainG, nGainB);
		EmptyMask();
		return S_OK;
	}
	else if (g_Gamma1.CheckByName(bstrSec, bstrEntry))
	{
		g_Gamma1.SetValueInt(nValue);
		if (m_pParams && m_pParams->m_hDev != NULL) m_pParams->m_Cam.CxSetBrightnessContrastGamma(m_pParams->m_hDev, 0, 0, (int)g_Gamma1);
		EmptyMask();
		return S_OK;
	}
	else if (g_DACRawOffset.CheckByName(bstrSec, bstrEntry))
	{
		g_DACRawOffset.SetValueInt(nValue);
		if (m_pParams && m_pParams->m_hDev != NULL) m_pParams->m_Cam.CxSetDACRawOffset(m_pParams->m_hDev, (BYTE)(int)g_DACRawOffset);
		EmptyMask();
		return S_OK;
	}
	else if (g_DACFineOffset.CheckByName(bstrSec, bstrEntry))
	{
		g_DACFineOffset.SetValueInt(nValue);
		if (m_pParams && m_pParams->m_hDev != NULL) m_pParams->m_Cam.CxSetDACFineOffset(m_pParams->m_hDev, (BYTE)(int)g_DACFineOffset);
		EmptyMask();
		return S_OK;
	}
	else if (g_CaptureMode.CheckByName(bstrSec, bstrEntry))
	{
		s_bSizesChanged = true;
		EmptyMask();
		return __super::SetValueInt(nDev, bstrSec, bstrEntry, nValue, dwFlags);
	}
	else if (g_ColorBalanceRed.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceRed.SetValueInt(nValue);
		if (m_pParams && m_pParams->m_hDev != NULL)
			m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, (int)g_ColorBalanceRed,
				(int)g_ColorBalanceGreen, (int)g_ColorBalanceBlue);
		EmptyMask();
		return S_OK;
	}
	else if (g_ColorBalanceGreen.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceGreen.SetValueInt(nValue);
		if (m_pParams && m_pParams->m_hDev != NULL)
			m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, (int)g_ColorBalanceRed,
				(int)g_ColorBalanceGreen, (int)g_ColorBalanceBlue);
		EmptyMask();
		return S_OK;
	}
	else if (g_ColorBalanceBlue.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceBlue.SetValueInt(nValue);
		if (m_pParams && m_pParams->m_hDev != NULL)
			m_pParams->m_Cam.CxSetGain(m_pParams->m_hDev, (int)g_Gain, (int)g_ColorBalanceRed,
				(int)g_ColorBalanceGreen, (int)g_ColorBalanceBlue);
		EmptyMask();
		return S_OK;
	}
	else if (g_ColorBalanceAuto.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceAuto.SetValueInt(nValue);
		return S_OK;
	}
	else
		return __super::SetValueInt(nDev, bstrSec, bstrEntry, nValue, dwFlags);
}
