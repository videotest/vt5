#include "StdAfx.h"
#include "resource.h"
#include "progresdriver.h"
#include "VT5Profile.h"
#if defined(__MEX)
#include "mebase.h"
#include "mexapi.h"
#else
#include "meapi.h"
#endif
#include "PreviewPixDlg.h"
#include "CamValues.h"
#include "MethCombo.h"
#include "Focus.h"
#include "SettingsBase.h"
#include "MethNameDlg.h"
CWnd *s1;
struct CProgResParams
{
#if defined(__MEX)
	mexImageParams m_ParamsPrv;
	mexImageParams m_ParamsCap;
	meInfo m_Info;
	CamType m_CamType;
#else
	meImageParams m_ParamsPrv;
	meImageParams m_ParamsCap;
#endif
};

const unsigned long LCL=2288;				// line clock lenght
const unsigned long FPPS=3351;			// fix pixel per shot
const unsigned long PFRQU=12288000;	// pixel frequency
const unsigned long lcl=LCL; 
const unsigned long fix=FPPS;
const unsigned long frequ=PFRQU/1000;
const unsigned long frequ2=(frequ+1)/2;
const unsigned long lcl2=(LCL+1)/2;     // half line clock length for rounding

//sergey 26/04/06
//CCamIntValue2 g_Exposure(_T("Parameters"), _T("Exposure"), 100, 1, 1000, 10, 1, 0, IDC_EDIT_EXPOSURE);
//CCamIntValue2 g_Exposure(_T("Parameters"), _T("Exposure"), 100, 1, 1000, 100, 10, 0, IDC_EDIT_EXPOSURE);
CCamIntValue2 g_Exposure(_T("Parameters"), _T("Exposure"), 100, 1, 100000, 100, 10, 0, IDC_EDIT_EXPOSURE);
//end
#if defined(__MEX)
/*INTCOMBOITEMDESCR s_aModes[] =
{
	{mexShot2F_highqualitycol, _T("2080x1542")},
	{mexShot2F_bin2col, _T("1040x770")},
	{mexShot2F_bin4col, _T("520x384")},
	{mexShot2F_lowqualitycol, NULL, IDS_2080x1542_LOW_QUALITY},
	{mexShot1F_highspeed1, _T("322x244")},
	{mexShot1F_highspeed2, _T("646x488")},
};
CCamIntComboValue  g_PreviewMode(_T("Preview"), _T("Mode"), s_aModes, sizeof(s_aModes)/sizeof(s_aModes[0]),
	mexShot1F_highspeed2);
CCamIntComboValue  g_CaptureMode(_T("Capture"), _T("Mode"), s_aModes, 3,
	mexShot2F_highqualitycol, 0, IDC_COMBO_CAPTURE);*/

//sergey 14/09/06
CCamIntComboValue2  g_PreviewMode(_T("Preview"), _T("Mode"), 0, 0, IDC_COMBO_PREVIEW,
	CCamValue::WriteToReg|CCamValue::SkipReinitCamera);

//CCamIntComboValue2  g_PreviewMode(_T("Preview"), _T("Mode"), 0, 0, -1,
	//CCamValue::WriteToReg|CCamValue::SkipReinitCamera);
//end

CCamIntComboValue2  g_CaptureMode(_T("Capture"), _T("Mode"), 0, 0, IDC_COMBO_CAPTURE,
	CCamValue::WriteToReg|CCamValue::SkipReinitCamera);


/*INTCOMBOITEMDESCR s_aModes[] =
{
	{mexm14_Shot_bwhighres, _T("1360x1024")},
	{mexm14_Shot_bwlowres, _T("680x512")},
	
};
CCamIntComboValue  g_CaptureMode(_T("Capture"), _T("Mode"), s_aModes, sizeof(s_aModes)/sizeof(s_aModes[0]),
	0, 0, IDC_COMBO_CAPTURE);

CCamIntComboValue  g_PreviewMode(_T("Preview"), _T("Mode"), s_aModes, sizeof(s_aModes)/sizeof(s_aModes[0]),
	0, 0, IDC_COMBO_CAPTURE);*/




#else
INTCOMBOITEMDESCR s_aModes[] =
{
	{meSingleShothighqualitycol, _T("2080x1542")},
	{meSingleShotbin2col, _T("1040x770")},
	{meSingleShotbin4col, _T("520x384")},
	{meSingleShotHighFrameRateColLarge, _T("692x512")},
	{meSingleShotHighFrameRateCol, _T("346x256")},
	{meSingleShotlowqualitycol, NULL, IDS_2080x1542_LOW_QUALITY},
	{meSingleShotbin3col, _T("692x514")},
	{meSingleShotbin5col, _T("416x308")},
};
CCamIntComboValue  g_PreviewMode(_T("Preview"), _T("Mode"), s_aModes, sizeof(s_aModes)/sizeof(s_aModes[0]),
	meSingleShotHighFrameRateColLarge);
CCamIntComboValue  g_CaptureMode(_T("Capture"), _T("Mode"), s_aModes, 4,
	meSingleShothighqualitycol, 0, IDC_COMBO_CAPTURE);
#endif
CCamDoubleValue    g_ColorBalanceRed(_T("Color balance"), _T("Red"), 1., 1., 10., IRepr_LogDoubleEdit,
	0, IDC_EDIT_WB_RED);
CCamDoubleValue    g_ColorBalanceGreen(_T("Color balance"), _T("Green"), 1., 1., 10., IRepr_LogDoubleEdit,
	0, IDC_EDIT_WB_GREEN);
CCamDoubleValue    g_ColorBalanceBlue(_T("Color balance"), _T("Blue"), 1., 1., 10., IRepr_LogDoubleEdit,
	0, IDC_EDIT_WB_BLUE);
CCamIntValue       g_ColorBalanceAuto(_T("Color balance"), _T("Auto"), 1, 0, 1, IRepr_Radiobuttons,
	0, IDC_WB_AUTO);

//sergey 14/04/06
CValuesGroup ValuesSizes;
static bool s_bSizesChanged = false;
CCamIntValue g_Gain(_T("Parameters"), _T("Gain"), 1, 0, 80, IRepr_SmartIntEdit, 0, IDC_EDIT_ENH_CONTRAST);
CCamIntValue g_Gamma1(_T("Parameters"), _T("Gamma"), 10, 0, 50, IRepr_SmartIntEdit, 0, IDC_EDIT_ENH_GAMMA);
 //CCamBoolValue g_Histo(_T("Histogram"), _T("Show"), false);
//end


//sergey 02/10/06
static int _CoefByMode(int mode,CamType type)
{
	
	

	if (type == cam14Mplus ||type == cam14Cplus)
{
		return 1;
	}
	else {
	   
#if defined(__MEX)
	if (mode == mexShot2F_highqualitycol || mode == mexShot2F_lowqualitycol)
#else
	if (mode == meSingleShothighqualitycol || mode == meSingleShotlowqualitycol)
#endif
		//sergey 14/08/06
		//return 1;
		return 2;
	else return 1;
	//end
	//end
	}
}

/*static int _CoefByMode(int mode)
{
#if defined(__MEX)
	if (mode == mexShot2F_highqualitycol || mode == mexShot2F_lowqualitycol)
#else
	if (mode == meSingleShothighqualitycol || mode == meSingleShotlowqualitycol)
#endif
		return 2;
	else return 1;
}*/
//end
static unsigned long ExposureUnitsToMilliseconds(unsigned long u)
{
	unsigned long m=(fix+u*lcl);	
	return (m%frequ>frequ2 ? (m/frequ+1) : (m/frequ));
}

static unsigned long MillisecondsToExposureUnits(unsigned long m)
{
	unsigned long u=(m>0 ? m*frequ-fix : 0);
	return ((u%lcl)>=lcl2 ? (u/lcl+1) : (u/lcl));  
	// m=0 ---> u=0
}

// Remove-Reinit mechanism if FireWire cable removed/reconnected //////////////
void __stdcall InitCB(unsigned long status, unsigned long UserValue, unsigned __int64 Guid)
{
	CProgResDriver* pDrv = reinterpret_cast<CProgResDriver*>(UserValue);
#if !defined(__MEX)
	if (pDrv->m_CamGuid == (unsigned __int64)0)
		meGetCurrentCamGuid(&pDrv->m_CamGuid);
#endif

	if(Guid == pDrv->m_CamGuid)
	{
		switch(status)
		{
		case GEN_STATUS_NEW_CAMERA: // Camera reconnected
			pDrv->m_bReconnect = true;
			break;
		case GEN_STATUS_CAMERA_REMOVED: // Camera removed
			pDrv->m_bRemoved = true;
			break;
		default:
			break;
		}
	}
}


BOOL __stdcall ImageFinalProcPrv(unsigned long status,void* src[3],unsigned long UserValue)
{
	CProgResDriver *pDrv = reinterpret_cast<CProgResDriver*>(UserValue);
	if (status == IMAGE_READY)
		pDrv->OnImageReadyPrv((unsigned short **)src);
	else if (status == IMAGE_TRANSFER_ERR)
		pDrv->OnImageErrorPrv();
	return TRUE;
}

BOOL __stdcall ImageFinalProcCap(unsigned long status,void* src[3],unsigned long UserValue)
{
	CProgResDriver *pDrv = reinterpret_cast<CProgResDriver*>(UserValue);
	if (status == IMAGE_READY)
		pDrv->OnImageReadyCap((unsigned short **)src);
	else if (status == IMAGE_TRANSFER_ERR)
		pDrv->OnImageErrorCap();
	return TRUE;
}

#if defined(__MEX)
static void InitImageParams(bool bPreview, mexImageParams &Params, CProgResDriver *pDrv, int mode = -1)
#else
static void InitImageParams(bool bPreview, meImageParams &Params, CProgResDriver *pDrv)
#endif
{
	LPCTSTR lpstrSec = bPreview?_T("Preview"):_T("Capture");
	meCamInfo CamInfo;
#if defined(__MEX)
	mexGetCameraInfo(pDrv->m_CamGuid, &CamInfo);
#else
	meGetCameraInfo(&CamInfo);
#endif
	memset(&Params, 0, sizeof(Params));
	::SetRect(&Params.rcimageBounds, 0, 0, CamInfo.SensorX, CamInfo.SensorY);
	//::SetRect(&Params.rcimageBounds, 0,0, 680, 512);
	Params.pImgFinalProc = bPreview?ImageFinalProcPrv:ImageFinalProcCap;
	Params.UserValue = (unsigned long)pDrv;
#if defined(__MEX)
	if (mode == -1)
	{
		if (bPreview)
			Params.mode = (mexAcqMode)(int)g_PreviewMode;
		//Params.mode = mexm14_Shot_bwlowres;
		else
			Params.mode = (mexAcqMode)(int)g_CaptureMode;
	}
	else
		Params.mode = (mexAcqMode)mode;
//	Params.exposureTicks = mexMSecToTicks(pDrv->m_CamGuid, (int)g_Exposure, Params.mode, 0);
	Params.exposureTicks = pDrv->MillisecondsToExposureUnits((int)g_Exposure);
#else
	Params.exposureTime = MillisecondsToExposureUnits((int)g_Exposure);
#endif

//??????????????????????????????????
/*	meCamInfo CamInfo;
#if defined(__MEX)
	mexGetCameraInfo(pDrv->m_CamGuid, &CamInfo);
#else
	meGetCameraInfo(&CamInfo);
#endif
	//memset(&Params, 0, sizeof(Params));
	::SetRect(&Params.rcimageBounds, 0, 0, CamInfo.SensorX, CamInfo.SensorY);
	//::SetRect(&Params.rcimageBounds, 0,0, 680, 512);
	Params.pImgFinalProc = bPreview?ImageFinalProcPrv:ImageFinalProcCap;
	Params.UserValue = (unsigned long)pDrv;*/

//?????????????????????????????????
	Params.buseblackref = FALSE;
	Params.busewhiteref = FALSE;
	Params.bwhitebalance = CStdProfileManager::m_pMgr->GetProfileInt(_T("Parameters"), _T("UseWhiteBalance"),
		FALSE, true);
	Params.bcorrectColors = CStdProfileManager::m_pMgr->GetProfileInt(_T("Parameters"), _T("UseColorCorrection"),
		FALSE, true);
}

#if defined(__MEX)

CString GetDescrByMode(CProgResDriver *pDrv, mexAcqMode AcqMode, int nSuffix)
{
	mexImageParams Params;
	InitImageParams(true, Params, pDrv, AcqMode);
	meImageInfo ImageInfo;
	mexGetImageInfo(pDrv->m_CamGuid, &Params, &ImageInfo);
	CString s,sSuffix;
	if (nSuffix == -1)
		s.Format(_T("%dx%d"), ImageInfo.DimX, ImageInfo.DimY);
	else
	{
		sSuffix.LoadString(nSuffix);
		s.Format(_T("%dx%d"), ImageInfo.DimX, ImageInfo.DimY, (LPCTSTR)sSuffix);
	}
	return s;
}

#endif

unsigned long CProgResDriver::MillisecondsToExposureUnits(unsigned long m)
{
	//sergey 26/04/06
	//sergey 02/05/07
	//if ( m_pParams->m_CamType == cam14Mplus)
		//end
	if (m_pParams->m_CamType == cam14Cplus || m_pParams->m_CamType == cam14Mplus)
		//return 400+m*10;//::MillisecondsToExposureUnits(m);
		return m/10;
	else
		//sergey 30/05/06
		return mexMSecToTicks(m_CamGuid, m/10, m_pParams->m_ParamsPrv.mode, 1);
	    //return mexMSecToTicks(m_CamGuid, m, m_pParams->m_ParamsPrv.mode, 1);
	  
	        //end
	
	/*if (m_pParams->m_CamType == cam14Cplus || m_pParams->m_CamType == cam14Mplus)
		return 400+m*10;//::MillisecondsToExposureUnits(m);
	else
		return mexMSecToTicks(m_CamGuid, m, m_pParams->m_ParamsPrv.mode, 0);*/
	//end
}

extern CCamBoolValue g_Histo;
CProgResDriver::CProgResDriver(void)
{
	m_pParams = new CProgResParams;
	m_CamState = NotInited;
	m_bReconnect = false;
	m_bRemoved = false;
	m_lpbiCap = NULL;
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
	//sergey 17.04.06
	//g_Histo.SetId(IDC_GREEN);
	//g_Contrast.SetId(IDC_EDIT_ENH_CONTRAST);
	//g_Gamma.SetId(IDC_EDIT_ENH_GAMMA);
	//end

	//ValuesSizes.Add(&g_CaptureMode);
	//ValuesSizes.Add(&g_PreviewMode);
}

CProgResDriver::~CProgResDriver(void)
{
	delete m_pParams;
}

IUnknown *CProgResDriver::DoGetInterface( const IID &iid )
{
	IUnknown *p;
	if (p = CDriverPreview::DoGetInterface(iid))
		return p;
	else
		return ComObjectBase::DoGetInterface(iid);
}

void __stdcall ExpCtrlCB(unsigned long ticks, unsigned long UserValue)
{
}

bool CProgResDriver::InitCamera()
{
#if defined(__MEX)
	long r = mexInit(InitCB, reinterpret_cast<unsigned long>(this));
	if (r == NOERR)
	{
		unsigned nNumberOfCameras = 0;
		unsigned __int64 GUID_List[10];
		r = mexFindCameras(&nNumberOfCameras,GUID_List);
		if (r == NOERR && nNumberOfCameras > 0)
		{
			r = mexOpenCamera(GUID_List[0]);
			if (r == NOERR)
			{
				m_CamGuid = GUID_List[0];
				m_pParams->m_CamType = mexGetCameraType(m_CamGuid);

				//sergey 14/08/06
			BOOL bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, true, true);
	        if (bGetImageFromPreview )
			 {				
				if (m_pParams->m_CamType == cam14Mplus)
				{
					//sergey 28/04/06
					BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), FALSE, true, true);
					//end
					//g_PreviewMode.Add(mexm14_Shot_bin2bw , GetDescrByMode(this, mexm14_Shot_bin2bw, -1), -1);
					g_PreviewMode.Add(mexm14_Shot_highspeed2, GetDescrByMode(this, mexm14_Shot_highspeed2, -1), -1);
				}
				else if (m_pParams->m_CamType == cam14Cplus)
				{
					//sergey 28/04/06
					BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
					//end
					//g_PreviewMode.Add(mexc14_Shot_bin3col     , GetDescrByMode(this, mexc14_Shot_bin3col     , -1), -1);
					//g_PreviewMode.Add(mexc14_Shot_highspeed1   , GetDescrByMode(this, mexc14_Shot_highspeed1    , -1), -1);
					g_PreviewMode.Add(mexc14_Shot_highspeed2, GetDescrByMode(this, mexc14_Shot_highspeed2, -1), -1);
					
				}
				else
				{
					//sergey 28/04/06
					BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
					//end
					//g_PreviewMode.Add(mexShot2F_bin3col  , GetDescrByMode(this, mexShot2F_bin3col , -1), -1);
					g_PreviewMode.Add(mexShot1F_highspeed2 , GetDescrByMode(this, mexShot1F_highspeed2 , -1), -1);
					//g_PreviewMode.Add(mexShot1F_highspeed1  , GetDescrByMode(this, mexShot1F_highspeed1 , -1), -1);					
				 }
				g_PreviewMode.Init();
				//g_CaptureMode.Add(mexm14_Shot_bin2bw , GetDescrByMode(this, mexm14_Shot_bin2bw , -1), -1);
				//g_CaptureMode.Init();
				if (m_pParams->m_CamType == cam14Mplus)
				{
					g_CaptureMode.Add(mexm14_Shot_bwhighres, GetDescrByMode(this, mexm14_Shot_bwhighres, -1), -1);
					g_CaptureMode.Add(mexm14_Shot_bwlowres, GetDescrByMode(this, mexm14_Shot_bwlowres, -1), -1);
					//g_CaptureMode.Add(mexm14_Shot_highspeed2, GetDescrByMode(this, mexm14_Shot_highspeed2, -1), -1);
				}
				else if (m_pParams->m_CamType == cam14Cplus)
				{
					g_CaptureMode.Add(mexc14_Shot_colhighres, GetDescrByMode(this, mexc14_Shot_colhighres, -1), -1);
					g_CaptureMode.Add(mexc14_Shot_collowres, GetDescrByMode(this, mexc14_Shot_collowres, -1), -1);
				}
				else
				{
					
					g_CaptureMode.Add(mexShot2F_highqualitycol, GetDescrByMode(this, mexShot2F_highqualitycol, -1), -1);
					g_CaptureMode.Add(mexShot2F_bin2col, GetDescrByMode(this, mexShot2F_bin2col, -1), -1);
				}
				
//				g_CaptureMode.Add(mexShot2F_bin4col, GetDescrByMode(this, mexShot2F_bin4col, -1), -1);
				g_CaptureMode.Init();
			 }
			 else{
		   //end

				if (m_pParams->m_CamType == cam14Mplus)
				{
					//sergey 28/04/06
					BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), FALSE, true, true);
					//end
					//g_PreviewMode.Add(mexm14_Shot_bin2bw , GetDescrByMode(this, mexm14_Shot_bin2bw, -1), -1);
					//g_PreviewMode.Add(mexm14_Shot_bwhighres, GetDescrByMode(this, mexm14_Shot_bwhighres, -1), -1);
					g_PreviewMode.Add(mexm14_Shot_bwlowres, GetDescrByMode(this, mexm14_Shot_bwlowres, -1), -1);
					//g_PreviewMode.Add(mexm14_Shot_highspeed1, GetDescrByMode(this, mexm14_Shot_highspeed1, -1), -1);
					//g_PreviewMode.Add(mexm14_Shot_highspeed2, GetDescrByMode(this, mexm14_Shot_highspeed2, -1), -1);
				}
				else if (m_pParams->m_CamType == cam14Cplus)
				{
					//sergey 28/04/06
					BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
					//end
					//g_PreviewMode.Add(mexc14_Shot_colhighres, GetDescrByMode(this, mexc14_Shot_colhighres, -1), -1);
					g_PreviewMode.Add(mexc14_Shot_collowres, GetDescrByMode(this, mexc14_Shot_collowres, -1), -1);
					//g_PreviewMode.Add(mexc14_Shot_highspeed1, GetDescrByMode(this, mexc14_Shot_highspeed1, -1), -1);
					//g_PreviewMode.Add(mexc14_Shot_highspeed2, GetDescrByMode(this, mexc14_Shot_highspeed2, -1), -1);
				}
				else
				{
					//sergey 28/04/06
					BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);

					//end

					g_PreviewMode.Add(mexShot1F_highspeed2, GetDescrByMode(this, mexShot1F_highspeed2, -1), -1);
					//g_PreviewMode.Add(mexShot1F_highspeed1, GetDescrByMode(this, mexShot1F_highspeed1, -1), -1);

					//sergey 15/09/06
					//g_PreviewMode.Add(mexShot2F_highqualitycol , GetDescrByMode(this,mexShot2F_highqualitycol, -1), -1); 

					//g_PreviewMode.Add(mexShot_Raw, GetDescrByMode(this, mexShot_Raw, -1), -1); 
					//g_PreviewMode.Add(mexShot2F_bin2col, GetDescrByMode(this, mexShot2F_bin2col, -1), -1);					
					//g_PreviewMode.Add(mexShot2F_bin4col, GetDescrByMode(this, mexShot2F_bin4col, -1), -1);
					//g_PreviewMode.Add(mexShot1F_highspeed2, GetDescrByMode(this, mexShot1F_highspeed2, -1), -1);
					//g_PreviewMode.Add(mexShot1F_highspeed1, GetDescrByMode(this, mexShot1F_highspeed1, -1), -1);

					//g_PreviewMode.Add( mexShot2F_highqualitycol  , GetDescrByMode(this, mexShot2F_highqualitycol , -1), -1); 
                   
					//g_PreviewMode.Add(mexShot2F_bin3col, GetDescrByMode(this, mexShot2F_bin3col, -1), -1);
					//g_PreviewMode.Add(mexShot2F_bin5col , GetDescrByMode(this, mexShot2F_bin5col, -1), -1);
					
					
					
					//end
					
				}
				g_PreviewMode.Init();
				if (m_pParams->m_CamType == cam14Mplus)
				{
					g_CaptureMode.Add(mexm14_Shot_bwhighres, GetDescrByMode(this, mexm14_Shot_bwhighres, -1), -1);
					g_CaptureMode.Add(mexm14_Shot_bwlowres, GetDescrByMode(this, mexm14_Shot_bwlowres, -1), -1);
					//g_CaptureMode.Add(mexm14_Shot_highspeed2, GetDescrByMode(this, mexm14_Shot_highspeed2, -1), -1);
				}
				else if (m_pParams->m_CamType == cam14Cplus)
				{
					g_CaptureMode.Add(mexc14_Shot_colhighres, GetDescrByMode(this, mexc14_Shot_colhighres, -1), -1);
					g_CaptureMode.Add(mexc14_Shot_collowres, GetDescrByMode(this, mexc14_Shot_collowres, -1), -1);
				}
				else
				{
					
					g_CaptureMode.Add(mexShot2F_highqualitycol, GetDescrByMode(this, mexShot2F_highqualitycol, -1), -1);
					g_CaptureMode.Add(mexShot2F_bin2col, GetDescrByMode(this, mexShot2F_bin2col, -1), -1);
				}
				
//				g_CaptureMode.Add(mexShot2F_bin4col, GetDescrByMode(this, mexShot2F_bin4col, -1), -1);
				g_CaptureMode.Init();
				}
				mexSetPeltier(GUID_List[0], 0, 0);
				mexDispose(GUID_List[0], (void*)15, 1);
				mexEqualizer Equ;
				Equ.bEnable = 1;
				Equ.Limit = 0.5;
				mexSetEqualizer(GUID_List[0], &Equ);
				mexEnableExposureReports(GUID_List[0], ExpCtrlCB, 0);
				mexDispose(GUID_List[0], 0, 0);
				mexGetInfo(GUID_List[0], &m_pParams->m_Info);
				m_pParams->m_CamType = mexGetCameraType(m_CamGuid);
				//sergey 17.04.06
				switch (m_pParams->m_CamType)
	              {
	               case cam10plus:
	               case cam12plus:
               //sergey 02/05/07
				   case cam14Cplus:
					   //end
	               g_Contrast.SetId(IDC_EDIT_ENH_CONTRAST);
	               g_Gamma.SetId(IDC_EDIT_ENH_GAMMA);
			   break;
				   case cam14Mplus:
				   g_Contrast.SetId(IDC_EDIT_ENH_CONTRAST);
				   //g_Gamma.SetId(IDC_EDIT_ENH_GAMMA);

				   //sergey 21/06/06
				   double nValue = CStdProfileManager::m_pMgr->GetProfileInt(_T("Parameters"), _T("Gamma"), 0, true);

				   //sergey 02/10/06
				   #if defined(__MEX)
					mexGamma  pGamma;
					double gamma=nValue/10;
					//double gamma=(double)nValue;
					if(gamma<0.2) gamma=0.2;
					if(gamma>5.0) gamma=5.0;

					pGamma.bActive=TRUE;
					pGamma.gamma=gamma;
					mexActivateGammaProcessing(m_CamGuid, &pGamma);
					#else				
				#endif		
				   //end
				   //end

			   break;
				  }
	           //end
			 
				return true;
			}
		}
		mexExit();
	}
#else
	long meResult;
	meResult=INITERR;
	m_CamState = Failed;
	unsigned nNumberOfCameras = 9;
	unsigned __int64 GUID_List[10];
	meResult = meFindCameras(&nNumberOfCameras,GUID_List);
	if (meResult == NOERR && nNumberOfCameras > 0)
	{
		meResult = meInitEx(GUID_List[0], &InitCB, reinterpret_cast<unsigned long>(this));
		if (meResult==NOERR)
		{
			meSetWhiteBalance((double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
				(double)g_ColorBalanceBlue);
		}
		return meResult==NOERR;
	}
#endif
	return false;
}

void CProgResDriver::DeinitCamera()
{
	if (m_CamState == Succeeded)
#if defined(__MEX)
	{
		mexCloseCamera(m_CamGuid);
		mexExit();
	}
#else
		meClose();
#endif
	m_CamState = NotInited;
}

/*void CProgResDriver::ReadSettings()
{
	CDriverPreview::ReadSettings();
}*/

HRESULT CProgResDriver::OnInitDlgSize(IDriverDialog *pDlg)
{
	return S_OK;
}

HRESULT CProgResDriver::OnCmdMsg(int nCode, UINT nID, long lHWND)
{
	
	if (nID == IDC_WB_BUILD)
	{
		
		if (!m_Dib.IsEmpty())
		{
			double dRed,dGreen,dBlue;
			CalAvrBrightness(m_Dib.m_lpbi, &dRed, &dGreen, &dBlue);
			dRed /= dGreen;
			dBlue /= dGreen;
			double dRedCur, dGreenCur, dBlueCur;
#if defined(__MEX)
			mexGetCurrentWhiteBalance(m_CamGuid, &dRedCur, &dGreenCur, &dBlueCur);
#else
			meGetCurrentWhiteBalance(&dRedCur, &dGreenCur, &dBlueCur);
#endif
			dRedCur /= dGreenCur;
			dBlueCur /= dGreenCur;
			double dRedNew = dRedCur/dRed, dBlueNew = dBlueCur/dBlue, dGreenNew = 1.;
			double dMin = min(min(dRedNew, dBlueNew), dGreenNew);
			dRedNew = max(dRedNew/dMin,1.);
			dBlueNew = max(dBlueNew/dMin, 1.);
			dGreenNew = max(dGreenNew/dMin, 1.);
#if defined(__MEX)
			mexSetWhiteBalance(m_CamGuid, dRedNew, dGreenNew, dBlueNew);
			
#else
			meSetWhiteBalance(dRedNew, dGreenNew, dBlueNew);
#endif
			g_ColorBalanceRed = dRedNew;
			g_ColorBalanceGreen = dGreenNew;
			g_ColorBalanceBlue = dBlueNew;
		}
		return S_OK;
	}

	/*else if (nID == IDC_BUTTON_DEFAULT_GAIN_GAMMA)
	{
		g_Gain = 1;
		g_Gamma1 = 0;
		return S_OK;
	}*/

	return __super::OnCmdMsg(nCode, nID, lHWND);
}

HRESULT CProgResDriver::OnUpdateCmdUI(HWND hwnd, int nID)
{
	//sergey 28/04/06
	   /* CWnd *s;		
			s=::AfxGetMainWnd();
			//HWND h=::GetDlgItem(*s, IDD_HISTO);
			::EnableWindow(::GetDlgItem(*s, IDC_RED), FALSE);			
	if(nID == IDC_SLIDER1_EXPOSURE ||nID ==IDC_EDIT_EXPOSURE)::EnableWindow(hwnd, 0);	
			//::ShowWindow(::GetDlgItem(*s, IDC_COLOR_BALANCE),SW_RESTORE);*/
	   //sergey 15/08/06
         BOOL bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, true, true);
		 if(bGetImageFromPreview){
			 if (nID == IDC_COMBO_CAPTURE) ::EnableWindow(hwnd, false );
	
		 }
		 //end

		BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
		
					//end
	if (nID == IDC_EDIT_WB_RED || nID == IDC_SPIN_WB_RED || nID == IDC_SLIDER_WB_RED ||
		nID == IDC_EDIT_WB_GREEN || nID == IDC_SPIN_WB_GREEN || nID == IDC_SLIDER_WB_GREEN ||
		nID == IDC_EDIT_WB_BLUE || nID == IDC_SPIN_WB_BLUE || nID == IDC_SLIDER_WB_BLUE)
		::EnableWindow(hwnd, (int)g_ColorBalanceAuto==1);
	else if (nID == IDC_WB_BUILD)
		::EnableWindow(hwnd, (int)g_ColorBalanceAuto==0);
	//sergey 17.04.06
		//::EnableWindow(hwnd, m_pParams->m_bColor);
	if(nID == IDC_RED || nID == IDC_GREEN || nID == IDC_BLUE)			
	    ::EnableWindow(hwnd, bColorCamera);
	else if (nID == IDC_COLOR_BALANCE || nID == IDC_CHECK_GRAYSCALE){
		::EnableWindow(hwnd, bColorCamera );
		
		//::EnableWindow(hwnd, !g_Gray.GetValueInt());
		}
	//sergey 15/08/06
	if(bColorCamera){
		if(g_Gray.GetValueInt()){
			
	if(nID == IDC_RED || nID == IDC_GREEN || nID == IDC_BLUE)			
	    ::EnableWindow(hwnd, false);
			if (nID == IDC_WB_AUTO || nID == IDC_WB_MANUAL ||nID == IDC_WB_BUILD ||nID == IDC_EDIT_WB_RED || nID == IDC_SPIN_WB_RED || nID == IDC_SLIDER_WB_RED ||
		nID == IDC_EDIT_WB_GREEN || nID == IDC_SPIN_WB_GREEN || nID == IDC_SLIDER_WB_GREEN ||
		nID == IDC_EDIT_WB_BLUE || nID == IDC_SPIN_WB_BLUE || nID == IDC_SLIDER_WB_BLUE)
	     ::EnableWindow(hwnd, false);

		if (nID == IDC_COLOR_BALANCE){	
	       ::EnableWindow(hwnd, false ); }
      }
	}
	//end
	
	
	
		
	
	if(!bColorCamera)
	{
		//sergey 02/07/07
		g_Gray.SetValueInt(1);
		::EnableWindow(::GetDlgItem(hwnd, IDC_CHECK_GRAYSCALE), FALSE);
		//end
		::EnableWindow(::GetDlgItem(hwnd, IDC_RED), FALSE);
		::EnableWindow(::GetDlgItem(hwnd, IDC_GREEN ), FALSE);
		::EnableWindow(::GetDlgItem(hwnd, IDC_BLUE), FALSE);
	 if (nID == IDC_WB_AUTO || nID == IDC_WB_MANUAL ||nID == IDC_WB_BUILD ||nID == IDC_EDIT_WB_RED || nID == IDC_SPIN_WB_RED || nID == IDC_SLIDER_WB_RED ||
		nID == IDC_EDIT_WB_GREEN || nID == IDC_SPIN_WB_GREEN || nID == IDC_SLIDER_WB_GREEN ||
		nID == IDC_EDIT_WB_BLUE || nID == IDC_SPIN_WB_BLUE || nID == IDC_SLIDER_WB_BLUE)
	     ::EnableWindow(hwnd, bColorCamera);
	}
	//end

	//sergey 02/10/06
	BOOL bMetal = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Metal"), FALSE, true, true);
	if(bMetal)
	{
		if (nID == IDC_SLIDER_ENH_CONTRAST ||nID == IDC_EDIT_ENH_CONTRAST|| nID == IDC_SPIN_ENH_CONTRAST)
           ::EnableWindow(hwnd, false);
	}
	//end



		//sergey 08/12/06
		BOOL bFish = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Fish"), FALSE, true, true);
		if(bFish){			
                 if (nID == IDC_SETTINGS_PAGE){	
	            ::EnableWindow(hwnd, false ); }
		         }
		//end


	return __super::OnUpdateCmdUI(hwnd, nID);
}

HWND s_hwndParent;
bool CProgResDriver::OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
	BOOL bFirst, BOOL bForInput)
{
	CPreviewIds PreviewIds = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG, IDC_STATIC_PERIOD}, 
		IDC_STATIC_PERIOD, IDC_IMAGE, IDC_ENHANCEMENT, IDC_SETTINGS_PAGE, -1, -1, IDC_HISTOGRAM,
		IDS_FPS, IDS_FRAME_PERIOD};
	CPreviewPixDlg  dlg(IDD_PREVIEW,PreviewIds,(IInputPreview *)this,nDevice,0,CWnd::FromHandle(hwndParent));
	dlg.AttachChildDialogToButton(IDC_ENHANCEMENT, IDD_ENHANCEMENT);
	dlg.AttachChildDialogToButton(IDC_IMAGE_PAGE, IDD_IMAGE);
	dlg.AttachChildDialogToButton(IDC_COLOR_BALANCE, IDD_COLOR_BALANCE);
	dlg.AttachChildDialogToButton(IDC_SETTINGS_PAGE, IDD_SETTINGS);
	s_hwndParent = hwndParent;	
	return dlg.DoModal() == IDOK;
}

HRESULT CProgResDriver::GetFlags(DWORD *pdwFlags)
{
	*pdwFlags = FG_INPUTFRAME|FG_GETIMAGE_DIB;
	//*pdwFlags = FG_INPUTFRAME|FG_SUPPORTSVIDEO;
	return S_OK;
}

HRESULT CProgResDriver::ExecuteDriverDialog(int nDev, LPCTSTR lpstrName)
{
	
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));
	//sergey 02.05.06
	BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	//end
	//sergey 27/04/06
	/*CSettingsIds Ids = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG},
		IDC_TAB_SELECT, 0};	*/

	/*CSettingsIds Ids = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG,IDC_STATIC_PERIOD},
		IDC_TAB_SELECT, 0,IDS_FPS,0,IDC_HISTOGRAM};*/
	CSettingsIds Ids = {{IDD_HISTO, IDC_HISTO, IDC_GRAY, IDC_RED, IDC_GREEN, IDC_BLUE,
		IDC_STATIC_0, IDC_STATIC_MAX_COLOR, IDC_STATIC_MAX_POINTS, IDC_STATIC_MSG,IDC_STATIC_PERIOD},
		IDC_TAB_SELECT, 0,IDS_FPS,0,IDC_HISTOGRAM};
	//end
	
	CSettingsBase dlg((IDriver*)this, 0, IDD_DIALOG_SETTINGS, Ids, NULL);
	dlg.AddPane(IDS_BRIGHTNESS, IDD_SHUTTER);
	dlg.AddPane(IDS_ENHANCEMENT, IDD_ENHANCEMENT);
	dlg.AddPane(IDS_IMAGE, IDD_IMAGE);	
    //sergey 02.05.06
	if(bColorCamera)
	{
	dlg.AddPane(IDS_WHITE_BALANCE, IDD_COLOR_BALANCE);
	}
	//dlg.AddPane(IDS_WHITE_BALANCE, IDD_HISTO);//sergey 02/06/06???????????????
	//end
	dlg.AddPane(IDS_SETTINGS, IDD_SETTINGS);	
	dlg.DoModal();
	try
	{
	return S_OK;
	}
	catch( CException* e )
   {
      
	  e->ReportError();
	  
	 throw;
   }
}

bool CProgResDriver::OnStartGrab()
{
#if defined(__MEX)
	mexStartFastAcquisition(m_CamGuid, &m_pParams->m_ParamsPrv);
	mexSetWhiteBalance(m_CamGuid, (double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
		(double)g_ColorBalanceBlue);
#else
	meStartFastAcquisition(&m_pParams->m_ParamsPrv);
	meSetWhiteBalance((double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
		(double)g_ColorBalanceBlue);
#endif
	return false;
}

void CProgResDriver::OnStopGrab()
{
#if defined(__MEX)
	mexAbortAcquisition(m_CamGuid);
#else
	meAbortAcquisition();
#endif
	Sleep(200);
}

int CProgResDriver::GetCameraBPP()
{
	CamType type = mexGetCameraType(m_CamGuid);
	int bpp;
	switch (type)
	{
	case cam14Cplus:
		bpp = 14*3;
		break;
	case cam14Mplus:
		bpp = 14;
		break;
	case cam10plus:
	case cam12plus:
	default:
		bpp = 12*3;
	}
	return bpp;
}


bool CProgResDriver::GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi)
{
	meImageInfo ImgInfo;
	if (bCapture)
	{
		InitImageParams(false, m_pParams->m_ParamsCap, this);
#if defined(__MEX)
//		m_pParams->m_ParamsCap.mode = (mexAcqMode)(int)g_CaptureMode;
		mexGetImageInfo(m_CamGuid, &m_pParams->m_ParamsCap, &ImgInfo);
#else
		m_pParams->m_ParamsCap.mode = (meScanMode)(int)g_CaptureMode;
		meGetImageInfo(&m_pParams->m_ParamsCap, &ImgInfo);
#endif
		InitDIB(lpbi, ImgInfo.DimX, ImgInfo.DimY, GetCameraBPP(), false);
	}
	else
	{
		InitImageParams(true, m_pParams->m_ParamsPrv, this);
#if defined(__MEX)
		m_pParams->m_ParamsPrv.mode = (mexAcqMode)(int)g_PreviewMode;
		mexGetImageInfo(m_CamGuid, &m_pParams->m_ParamsPrv, &ImgInfo);
		
#else
		m_pParams->m_ParamsPrv.mode = (meScanMode)(int)g_PreviewMode;
		meGetImageInfo(&m_pParams->m_ParamsPrv, &ImgInfo);
#endif
		//sergey 19/06/07
		BOOL bInterpolation = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("Interpolation"), FALSE, true, true);

		
				if(bInterpolation)
	            {    
					//sergey 02/07/07
		             // g_Gray.SetValueInt(1);
		             //end
		         // InitDIB(lpbi, ImgInfo.DimX*2, ImgInfo.DimY, GetCameraBPP(), false);
				  InitDIB(lpbi, ImgInfo.DimX*2, ImgInfo.DimY*2, GetCameraBPP(), false);
				}
				else {
		//InitDIB(lpbi, ImgInfo.DimX*2, ImgInfo.DimY, GetCameraBPP(), false);
		InitDIB(lpbi, ImgInfo.DimX, ImgInfo.DimY, GetCameraBPP(), false);
				       }
		//end
		/*if(m_pParams->m_ParamsPrv.mode==mexm14_Shot_highspeed2)
		InitDIB(lpbi, 680, 512, GetCameraBPP(), false);
		else if(m_pParams->m_ParamsPrv.mode==mexm14_Shot_bwhighres) 
			InitDIB(lpbi, 1360, 1024, GetCameraBPP(), false);*/
		
	}
	return true;
}

void CProgResDriver::ProcessCapture()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDialog *pdlg = new CDialog(IDD_WAIT);
	pdlg->Create(IDD_WAIT, CWnd::FromHandle(s_hwndParent));
	pdlg->CenterWindow();
	MSG msg;
	while(PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
#if defined(__MEX)
	mexAcquisition(m_CamGuid, &m_pParams->m_ParamsCap);
#else
	meAcquisition(&m_pParams->m_ParamsCap);
#endif
	delete pdlg;
}

void CProgResDriver::OnSetValueDouble(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dVal)
{
	if (_tccmp(lpstrSec, g_ColorBalanceRed.GetHdr()->pszSection)==0)
#if defined(__MEX)
		mexSetWhiteBalance(m_CamGuid, (double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
			(double)g_ColorBalanceBlue);
#else
		meSetWhiteBalance((double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
			(double)g_ColorBalanceBlue);
#endif
	Sleep(100);
}

void CProgResDriver::OnResetValues()
{
#if defined(__MEX)
	mexSetWhiteBalance(m_CamGuid, (double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
		(double)g_ColorBalanceBlue);
#else
	meSetWhiteBalance((double)g_ColorBalanceRed, (double)g_ColorBalanceGreen, 
		(double)g_ColorBalanceBlue);
#endif
	//s_bSizesChanged=true;
}

void CProgResDriver::OnImageReadyPrv(unsigned short* src[3])
{
	//sergey 21/06/06
	           m_pParams->m_CamType = mexGetCameraType(m_CamGuid);				
				switch (m_pParams->m_CamType)
	              {	               
				   case cam14Mplus:				  
					   //sergey 02/10/06
				   double nValue = CStdProfileManager::m_pMgr->GetProfileInt(_T("Parameters"), _T("Gamma"), 0, true);
				   #if defined(__MEX)
					mexGamma  pGamma;
					double gamma=nValue/10;
					//double gamma=(double)nValue;
					if(gamma<0.2) gamma=0.2;
					if(gamma>5.0) gamma=5.0;

					pGamma.bActive=TRUE;
					pGamma.gamma=gamma;
					mexActivateGammaProcessing(m_CamGuid, &pGamma);
					#else				
				#endif	
			//end
			   break;
				  }
	           //end			

	CorrectBrightness(m_Dib.cx(), m_Dib.cy(), m_pParams->m_ParamsPrv.mode, src);
	if (src[2] == 0 || src[1] == 0)
		GrayReady16(src[0]);
	else
		PlanesReady16(src[2], src[1], src[0]);
}

void CProgResDriver::OnImageErrorPrv()
{
#if defined(__MEX)
	mexAbortAcquisition(m_CamGuid);
	Sleep(300);
	mexStartFastAcquisition(m_CamGuid, &m_pParams->m_ParamsPrv);
#else
	meAbortAcquisition();
	Sleep(300);
	meStartFastAcquisition(&m_pParams->m_ParamsPrv);
#endif
}

void CProgResDriver::OnImageReadyCap(unsigned short* src[3])
{
	CorrectBrightness(m_biCapFmt.biWidth, m_biCapFmt.biHeight, m_pParams->m_ParamsCap.mode, src);
	if (src[2] == 0 || src[1] == 0)
		GrayReady16(src[0]);
	else
		PlanesReady16(src[2], src[1], src[0]);
}

void CProgResDriver::OnImageErrorCap()
{
#if defined(__MEX)
	mexAbortAcquisition(m_CamGuid);
#else
	meAbortAcquisition();
#endif
}

void CProgResDriver::CorrectBrightness(int cx, int cy, int mode, unsigned short* src[3])
{
	//sergey 02/10/06
	CamType type = mexGetCameraType(m_CamGuid);
	int nCoef = _CoefByMode(mode,type);
	//int nCoef = _CoefByMode(mode);
	//end
	if (nCoef == 1)
		return;
	unsigned nCount = cx*cy;
	for (int c = 0; c < 3; c++)
	{
		LPWORD lpData = src[c];
		for (unsigned i = 0; i < nCount; i++)
		{
			unsigned n = *lpData;
			n = min(n*nCoef, 0xFFF);
			*lpData = (color)n;
			lpData++;
		}
	}
}

HRESULT CProgResDriver::SetValueInt(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int nValue,
	UINT nFlags)
{

	if (g_Exposure.CheckByName(bstrSec, bstrEntry))
	{
		g_Exposure.SetValueInt(nValue);
		m_pParams->m_ParamsPrv.exposureTicks = MillisecondsToExposureUnits((int)g_Exposure);
//		m_pParams->m_ParamsPrv.exposureTicks = mexMSecToTicks(m_CamGuid, (int)g_Exposure, m_pParams->m_ParamsPrv.mode, 0);
		mexModifyLiveParameters(m_CamGuid, m_pParams->m_ParamsPrv.exposureTicks, &m_pParams->m_ParamsPrv.rcimageBounds);
		return S_OK;
	}
	else if (g_ColorBalanceAuto.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceAuto.SetValueInt(nValue);
		return S_OK;
	}
	else if (g_ColorBalanceRed.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceRed.SetValueInt(nValue);
		
		return S_OK;
	}
	else if (g_ColorBalanceGreen.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceGreen.SetValueInt(nValue);
		
		return S_OK;
	}
	else if (g_ColorBalanceBlue.CheckByName(bstrSec, bstrEntry))
	{
		g_ColorBalanceBlue.SetValueInt(nValue);
		
		return S_OK;
	}
	
	else if (g_Gray.CheckByName(bstrSec, bstrEntry))
	{
		g_Gray.SetValueInt(nValue);
		
		//return S_OK;
	}
	//sergey 14/09/06
	/*if (g_PreviewMode.CheckByName(bstrSec, bstrEntry))	
	{
		
		s_bSizesChanged = true;		
			
		g_PreviewMode.SetValueInt(nValue);	
		//g_CaptureMode.SetValueInt(nValue);
		//this->OnStopGrab();
		//this->EndPreview(0,this);
		if(m_nGrabCount>1)--m_nGrabCount;
		this->StartImageAquisition(1,FALSE);
		
		m_pParams->m_ParamsPrv.mode=(mexAcqMode)(int)g_PreviewMode;
		mexIsAcquisitionModeSupported(m_CamGuid,m_pParams->m_ParamsPrv.mode);
		

		this->StartImageAquisition(1,TRUE);
		if(m_nGrabCount<2)m_nGrabCount++;
		//this->OnStartGrab();		
		//mexModifyLiveParameters(m_CamGuid, m_pParams->m_ParamsPrv.mode, &m_pParams->m_ParamsPrv.rcimageBounds);		
		//mexModifyLive(m_CamGuid,&m_pParams->m_ParamsPrv);
		
		
		return S_OK;
		//return __super::SetValueInt(nCurDev, bstrSec, bstrEntry, nValue, nFlags);
	}*/
	//end

	//sergey 14/04/06
	/*if (g_PreviewMode.CheckByName(bstrSec, bstrEntry))	
	{
		
		s_bSizesChanged = true;		
		g_CaptureMode.SetValueInt(nValue,false);
		g_PreviewMode.SetValueInt(nValue,false);	
		
		
		return __super::SetValueInt(nCurDev, bstrSec, bstrEntry, nValue, nFlags);
	}*/
	

	else if (g_Gain.CheckByName(bstrSec, bstrEntry))
	{
		CamType type = mexGetCameraType(m_CamGuid);
	
	switch (type)
	{
		//sergey 02/05/07
	//case cam14Cplus:
		//end
	case cam14Mplus:		
		g_Gain.SetValueInt(nValue);
		//int nGainMain,nGainR,nGainG,nGainB;
		//if (m_pParams && m_pParams->m_hDev != NULL) m_pParams->m_Cam.CxGetGain(m_pParams->m_hDev, &nGainMain, &nGainR, &nGainG, &nGainB);

		//sergey 02/10/06
		#if defined(__MEX)
		mexSetGain(m_CamGuid, nValue/10);	
       #else
	
      #endif
		//end
		
		break;	
		return S_OK;
	}
		
	}
	else if (g_Gamma1.CheckByName(bstrSec, bstrEntry))
	{
		CamType type = mexGetCameraType(m_CamGuid);
	
	  switch (type)
	   {
		   //sergey 02/05/07
	   //case cam14Cplus:
		   //end
	   case cam14Mplus:
		g_Gamma1.SetValueInt(nValue);
		//sergey 02/10/06
		#if defined(__MEX)
		mexGamma  pGamma;
		double gamma=(double)nValue/10;
		//double gamma=(double)nValue;
        if(gamma<0.2) gamma=0.2;
        if(gamma>5.0) gamma=5.0;

		pGamma.bActive=TRUE;
		pGamma.gamma=gamma;
		mexActivateGammaProcessing(m_CamGuid, &pGamma);
		#else
	
      #endif		
		//end
		break;		
		return S_OK;
	   }
	 }
	//sergey 26/05/06
	/*else if (g_Contrast.CheckByName(bstrSec, bstrEntry))
	{
		g_Contrast.SetValueInt(nValue);
		return S_OK;
	}*/

	
	
	//end
	//else
		return __super::SetValueInt(nCurDev, bstrSec, bstrEntry, nValue, nFlags);
}

void CProgResDriver::OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal)
{
//	if (g_Exposure.CheckByName(lpstrSec, lpstrEntry))
	//m_bSizeChanged = true;
		Sleep(100);
}
