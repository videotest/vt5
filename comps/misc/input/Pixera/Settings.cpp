#include "StdAfx.h"
#include "Settings.h"
#include "PixSdk.h"
#include "StdProfile.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

int g_nWBMode = kWBOff;
BOOL g_bGetImageFromPreview = FALSE;

static void SaveLevelAdjustment(int nLevel)
{
	static LPCTSTR szLvlNames[] =
	{
		_T("Luminance"),
		_T("Red"),
		_T("Green"),
		_T("Blue")
	};
	LevelAdjEx level;
	memset(&level, 0, sizeof(level));
	level.nChannel = (ChannelEx)(nLevel);
	CAM_IP_GetLuminanceLevels(&level);
	LPCTSTR lpszLvl = szLvlNames[nLevel-kLuminanceChannel];
	CString sSec;
	sSec.Format(_T("LevelAdustment\\%s"), lpszLvl);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("Highlight"), level.nInHighlight);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("Shadow"), level.nInShadow);
	CStdProfileManager::m_pMgr->WriteProfileDouble(sSec, _T("Gamma"), level.nInGamma);
}

static void RestoreLevelAdjustment(int nLevel)
{
	static LPCTSTR szLvlNames[] =
	{
		_T("Luminance"),
		_T("Red"),
		_T("Green"),
		_T("Blue")
	};
	LevelAdjEx level;
	memset(&level, 0, sizeof(level));
	level.nChannel = (ChannelEx)(nLevel);
	CAM_IP_GetLuminanceLevels(&level);
	LPCTSTR lpszLvl = szLvlNames[nLevel-kLuminanceChannel];
	CString sSec;
	sSec.Format(_T("LevelAdustment\\%s"), lpszLvl);
	level.nInHighlight = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("Highlight"), level.nInHighlight);
	level.nInShadow = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("Shadow"), level.nInShadow);
	level.nInGamma = CStdProfileManager::m_pMgr->GetProfileDouble(sSec, _T("Gamma"), level.nInGamma);
	CAM_IP_SetLuminanceLevels(&level);
}

void SaveRect(LPCTSTR sSec, bool bDraw, PxRectEx &rect, UCHAR r, UCHAR g, UCHAR b)
{
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("Show"), bDraw);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("left"), rect.left);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("top"), rect.top);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("right"), rect.right);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("bottom"), rect.bottom);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("red"), (int)r);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("green"), (int)g);
	CStdProfileManager::m_pMgr->WriteProfileInt(sSec, _T("blue"), (int)b);
}

void RestoreDefaultRect(LPCTSTR sSec, bool &bDraw, PxRectEx &rect, UCHAR &r, UCHAR &g, UCHAR &b)
{
	PreviewResolutionEx Rsl;
	PxSizeEx Size;
	CAM_MIA_GetResolution(&Rsl, &Size);
	bDraw = CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("Show"), FALSE) == TRUE;
	rect.left = CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("left"), 100);
	rect.top = CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("top"), 100);
	rect.right = CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("right"), 200);
	rect.bottom = CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("bottom"), 200);
	int nMarg = 10;
	rect.left = min(max(0,rect.left),Size.cx-nMarg);
	rect.right = min(max(nMarg,rect.right),Size.cx);
	rect.top = min(max(0,rect.top),Size.cy-nMarg);
	rect.bottom = min(max(nMarg,rect.bottom),Size.cy);
	if (rect.right < rect.left+nMarg) rect.right = rect.left+nMarg;
	if (rect.bottom < rect.top+nMarg) rect.bottom = rect.top+nMarg;
	r = (UCHAR)CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("red"), 255);
	g = (UCHAR)CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("green"), 255);
	b = (UCHAR)CStdProfileManager::m_pMgr->GetDefaultProfileInt(sSec, _T("blue"), 255);
}

PixRectData RestoreDefaultRect(LPCTSTR sSec)
{
	PixRectData rd;
	RestoreDefaultRect(sSec, rd.bDraw, rd.rect, rd.r, rd.g, rd.b);
	return rd;
}

void RestoreRect(LPCTSTR sSec, bool &bDraw, PxRectEx &rect, UCHAR &r, UCHAR &g, UCHAR &b)
{
	PreviewResolutionEx Rsl;
	PxSizeEx Size;
	CAM_MIA_GetResolution(&Rsl, &Size);
	bDraw = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("Show"), FALSE) == TRUE;
	rect.left = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("left"), 100);
	rect.top = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("top"), 100);
	rect.right = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("right"), 200);
	rect.bottom = CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("bottom"), 200);
	int nMarg = 10;
	rect.left = min(max(0,rect.left),Size.cx-nMarg);
	rect.right = min(max(nMarg,rect.right),Size.cx);
	rect.top = min(max(0,rect.top),Size.cy-nMarg);
	rect.bottom = min(max(nMarg,rect.bottom),Size.cy);
	if (rect.right < rect.left+nMarg) rect.right = rect.left+nMarg;
	if (rect.bottom < rect.top+nMarg) rect.bottom = rect.top+nMarg;
	r = (UCHAR)CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("red"), 255);
	g = (UCHAR)CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("green"), 255);
	b = (UCHAR)CStdProfileManager::m_pMgr->GetProfileInt(sSec, _T("blue"), 255);
}

PixRectData RestoreRect(LPCTSTR sSec)
{
	PixRectData rd;
	RestoreRect(sSec, rd.bDraw, rd.rect, rd.r, rd.g, rd.b);
	return rd;
}

void SaveSettings()
{
	AutoExposureModeEx Mode = CAM_AE_GetMode();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Exposure"), _T("Auto"), Mode != kManualExposure);
	if (Mode == kManualExposure)
	{
		double d;
		CAM_EXP_GetSpeed(&d); // 1..10000
		CStdProfileManager::m_pMgr->WriteProfileDouble(_T("Exposure"), _T("Speed"), d);
	}
	else
	{
		int n = CAM_AE_GetAdjust();
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Exposure"), _T("Adjust"), n);
	}
	bool bDraw;
	PxRectEx aRect;
	UCHAR byAER,byAEG,byAEB;
	CAM_AE_GetSpotSize(&bDraw, &aRect, &byAER, &byAEG, &byAEB);
/*	if (bDraw)
		SaveRect(_T("Exposure\\Rect"), bDraw, aRect, byAER, byAEG, byAEB);
	else*/
		CStdProfileManager::m_pMgr->WriteProfileInt(_T("Exposure\\Rect"), _T("Show"), bDraw);
	int nSensitivity;
	CAM_EXP_GetSensitivity(&nSensitivity);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Exposure"), _T("Sensitivity"), nSensitivity);
    //sergey 30/01/06			
	//BOOL bColorCamera=GetPrivateProfileInt(_T("Settings"), _T("ColorCamera"), 0,CStdProfileManager::m_pMgr->GetIniName(true));	
	BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), FALSE, true, true);
	  //BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	//end
	SaveLevelAdjustment(kLuminanceChannel);
	if (bColorCamera)
	{
	SaveLevelAdjustment(kRedChannel);
	SaveLevelAdjustment(kGreenChannel);
	SaveLevelAdjustment(kBlueChannel);
	}
	AccumulateMethodEx AccMode;
	CAM_SIA_GetAccumulateMode(&AccMode);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Accumulation"), _T("Mode"), AccMode);
	int nAccTimes;
	CAM_SIA_GetAccumulateTimes(&nAccTimes);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Accumulation"), _T("Times"), nAccTimes);
	if (bColorCamera)
	{
		//WhiteBalanceModeEx WBMode = CAM_WB_GetMode();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("WhiteBalance"), _T("Mode"), g_nWBMode);
	bool bWBShow;
	PxRectEx WBrect;
	UCHAR byWBR,byWBG,byWBB;
	CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
	SaveRect(_T("WhiteBalance\\Rect"), bWBShow, WBrect, byWBR, byWBG, byWBB);
	double dWBRed,dWBGreen,dWBBlue;
	CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
		
	CStdProfileManager::m_pMgr->WriteProfileDouble(_T("WhiteBalance"), _T("red"), dWBRed);
	CStdProfileManager::m_pMgr->WriteProfileDouble(_T("WhiteBalance"), _T("green"), dWBGreen);
	CStdProfileManager::m_pMgr->WriteProfileDouble(_T("WhiteBalance"), _T("blue"), dWBBlue);
	}
	OrientationEx Orient = CAM_IP_GetOrientation();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Image"), _T("Orientation"), Orient);
	PreviewResolutionEx PrvMode;
	PxSizeEx pxMIA,pxSIA;
	CAM_MIA_GetResolution(&PrvMode,&pxMIA);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Image"), _T("Preview"), PrvMode);
	StillResolutionEx CapMode;
	CAM_SIA_GetResolution(&CapMode, &pxSIA);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Image"), _T("Capture"), CapMode);
	MicroModeEx MicroMode;
	CAM_AE_GetMicroMode(&MicroMode);
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Image"), _T("Microscope"), MicroMode);
	if (bColorCamera)
	{
	BOOL bColor = CAM_IP_IsColorCapture()==kColorCamColor;
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Image"), _T("Color"), bColor);
	}
	BOOL bFocus = CAM_FF_IsRunning();
	CStdProfileManager::m_pMgr->WriteProfileInt(_T("Focus"), _T("Enable"), bFocus);
	bool bDrawingOnOff;
	PxRectEx RectFocus;
	UCHAR byFocR, byFocG, byFocB;
	CAM_FF_GetRegion(&bDrawingOnOff, &RectFocus, &byFocR, &byFocG, &byFocB);
	SaveRect(_T("Focus\\Rect"), bDrawingOnOff, RectFocus, byFocR, byFocG, byFocB);
	bool bBB;
	PxRectEx BBrect;
	UCHAR byBBR, byBBG, byBBB;
	CAM_BB_GetRegion(&bBB, &BBrect, &byBBR, &byBBG, &byBBB);
	SaveRect(_T("BlackBalance\\Rect"), bBB, BBrect, byBBR, byBBG, byBBB);
}

bool check(int &nVal, int nMin, int nMax)
{
	if (nVal < nMin)
	{
		nVal = nMin;
		return true;
	}
	else if (nVal > nMax)
	{
		nVal = nMax;
		return true;
	}
	else
		return false;
}

void RestoreSettings()
{
	//sergey 25/05/06
	if (CStdProfileManager::m_pMgr->GetMethodic().IsEmpty())
	{
		CString sMeth = CStdProfileManager::m_pMgr->GetProfileString(_T("Methodics"), _T("Default"), NULL, false, true);
		CStdProfileManager::m_pMgr->InitMethodic(sMeth);
	}
	//end
	BOOL bAuto = CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("Auto"), 1);
	if (CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("DisableAutoExposure"), TRUE, true))
		bAuto = FALSE;
	if (bAuto)
	{
		CAM_AE_Start();
		CAM_AE_Lock(false);
		int n = CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("Adjust"), 0);
		check(n, -6, 6);
		CAM_AE_SetAdjust(n);
	}
	else
	{
		CAM_AE_Stop();
		double d = CStdProfileManager::m_pMgr->GetProfileDouble(_T("Exposure"), _T("Speed"), 100.);
		CAM_EXP_SetSpeed(d); // 1..10000
	}
	int nSensitivity = CStdProfileManager::m_pMgr->GetProfileInt(_T("Exposure"), _T("Sensitivity"), 100);
	CAM_EXP_SetSensitivity(nSensitivity);
	bool bDrawAESpot;
	PxRectEx aRect;
	UCHAR byAER,byAEG,byAEB;
	RestoreRect(_T("Exposure\\Rect"), bDrawAESpot, aRect, byAER, byAEG, byAEB);
	CAM_AE_SetSpotSize(bDrawAESpot, &aRect, byAER, byAEG, byAEB);
	 //sergey 30/01/06			
	//BOOL bColorCamera=GetPrivateProfileInt(_T("Settings"), _T("ColorCamera"), 0,CStdProfileManager::m_pMgr->GetIniName(true));	
	 BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), FALSE, true, true);
	   //BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	//end
	//BOOL bColorCamera = CStdProfileManager::m_pMgr->GetProfileInt(_T("Settings"), _T("ColorCamera"), TRUE, true, true);
	RestoreLevelAdjustment(kLuminanceChannel);
	if (bColorCamera)
	{
	RestoreLevelAdjustment(kRedChannel);
	RestoreLevelAdjustment(kGreenChannel);
	RestoreLevelAdjustment(kBlueChannel);
	}
	int nAccMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Accumulation"), _T("Mode"), kNotAccumulating);
	check(nAccMode, kNotAccumulating, kAverage);
	CAM_SIA_SetAccumulateMode((AccumulateMethodEx)nAccMode);
	int nAccTimes = CStdProfileManager::m_pMgr->GetProfileInt(_T("Accumulation"), _T("Times"), 1);
	CAM_SIA_SetAccumulateTimes(nAccTimes);
	CAM_SIA_GetAccumulateTimes(&nAccTimes);
	//?????????????????????????????????????
	if (bColorCamera)
	{
		ColorModeEx Color = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"),_T("Color"),TRUE)?kColorCamColor:kColorCamMono;
		CAM_IP_ColorCapture(Color);
	}
	//??????????????????????????????????
	if (bColorCamera)
	{
	int nWBMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("WhiteBalance"), _T("Mode"), kAutoWB);
	check(nWBMode, kWBOff, kManualWB);
	g_nWBMode = nWBMode;
		//CAM_WB_SetMode((WhiteBalanceModeEx)nWBMode);
	bool bWBShow;
	PxRectEx WBrect;
	UCHAR byWBR,byWBG,byWBB;
		//sergey 10/05/06
		if (nWBMode != kAutoWB)
		{
			/*if(IsDlgButtonChecked(IDC_SHOW_WB_RECT)==1)
			{
				CheckDlgButton(IDC_SHOW_WB_RECT, 0);
			}*/
			
			bWBShow = false;
		}
		//sergey 17/05/06
		if (nWBMode == kOneTouchWB)
		{
			CAM_WB_CalibrateFactors();
	       //CAM_WB_SetMode(kAutoWB);
	       CAM_WB_SetMode(kOneTouchWB);
		}
		if (nWBMode == kWBOff)
		{
			CAM_WB_SetMode(kWBOff);
		}

		if (nWBMode == kAutoWB)
		{
			//CAM_WB_SetMode((WhiteBalanceModeEx)nWBMode);
		CAM_WB_SetMode(kAutoWB);		
		}
		
		

	RestoreRect(_T("WhiteBalance\\Rect"), bWBShow, WBrect, byWBR, byWBG, byWBB);
		
		
		
		
		//if(bWBShow)
	CAM_WB_SetRegion(bWBShow,&WBrect,byWBR,byWBG,byWBB);

		 //}
		//end
         //sergey 17/05/06		
		if (nWBMode == kManualWB)
		{
			CAM_WB_SetMode(kManualWB);
		
	double dWBRed = CStdProfileManager::m_pMgr->GetProfileDouble(_T("WhiteBalance"), _T("red"), 1.);
	double dWBGreen = CStdProfileManager::m_pMgr->GetProfileDouble(_T("WhiteBalance"), _T("green"), 1.);
	double dWBBlue = CStdProfileManager::m_pMgr->GetProfileDouble(_T("WhiteBalance"), _T("blue"), 1.);
		//sergey 11/05/06
		//CAM_WB_SetFactors(1.,1.,1.);
		//end
		//Sleep(15);
	CAM_WB_SetFactors(dWBRed,dWBGreen,dWBBlue);
		TRACE("Restore settings, mode = %d, factors are %f,%f,%f\n", nWBMode,dWBRed,dWBGreen,dWBBlue);
	}
		//end
	}
	int nOrient = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("Orientation"), kNoTransform);
	check(nOrient, kNoTransform, kRotate180);
	CAM_IP_SetOrientation((OrientationEx)nOrient);
	int nPrvMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("Preview"), kFast);
	check(nPrvMode, kFast, kZoom);
	CAM_MIA_SetResolution((PreviewResolutionEx)nPrvMode);
	//sergey 11/05/06
	//g_bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, true,true);
	g_bGetImageFromPreview = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("CaptureFromPreview"), 0, TRUE);
	//end
	int nCapMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("Capture"), kSingle);
	check(nCapMode, kWarp, kVGA);
	CAM_SIA_SetResolution((StillResolutionEx)nCapMode);
	int nMicroMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"), _T("Microscope"), kBF);
	check(nMicroMode, kBF, kFL);
	CAM_AE_SetMicroMode((MicroModeEx)nMicroMode);
	if (bColorCamera)
	{
	ColorModeEx Color = CStdProfileManager::m_pMgr->GetProfileInt(_T("Image"),_T("Color"),TRUE)?kColorCamColor:kColorCamMono;
	CAM_IP_ColorCapture(Color);
	}
	BOOL bFocus = CStdProfileManager::m_pMgr->GetProfileInt(_T("Focus"), _T("Enable"), FALSE);
	if (bFocus)
		CAM_FF_Start();
	else
		CAM_FF_Stop();
	bool bDrawingOnOff;
	PxRectEx RectFocus;
	UCHAR byFocR,byFocG,byFocB;
	RestoreRect(_T("Focus\\Rect"), bDrawingOnOff, RectFocus, byFocR, byFocG, byFocB);
	CAM_FF_SetRegion(bDrawingOnOff, &RectFocus, byFocR, byFocG, byFocB);
	bool bBB;
	PxRectEx BBrect;
	UCHAR byBBR, byBBG, byBBB;
	//sergey 10/05/06
	
	RestoreRect(_T("BlackBalance\\Rect"), bBB, BBrect, byBBR, byBBG, byBBB);
	CAM_BB_SetRegion(bBB, &BBrect, byBBR, byBBG, byBBB);
	
	//end
}


void RestoreSettingsLite()
{
	//WhiteBalanceModeEx WBMode = CAM_WB_GetMode();
	int nWBMode = CStdProfileManager::m_pMgr->GetProfileInt(_T("WhiteBalance"), _T("Mode"), kAutoWB);
	if (nWBMode == kManualWB)
	//if (WBMode == kManualWB)
	{
		double dWBRed = CStdProfileManager::m_pMgr->GetProfileDouble(_T("WhiteBalance"), _T("red"), 1.);
		double dWBGreen = CStdProfileManager::m_pMgr->GetProfileDouble(_T("WhiteBalance"), _T("green"), 1.);
		double dWBBlue = CStdProfileManager::m_pMgr->GetProfileDouble(_T("WhiteBalance"), _T("blue"), 1.);
		//sergey 11/05/06
		//CAM_WB_SetFactors(1.,1.,1.);
		//end
		//CAM_WB_SetMode(kWBOff);
		//CAM_WB_SetMode(kManualWB);
		//Sleep(15);
		//CAM_WB_SetMode(kManualWB);
		CAM_WB_SetFactors(dWBRed,dWBGreen,dWBBlue);
		TRACE("RestoreSettingsLite, mode = %d, factors are %f,%f,%f\n", nWBMode,dWBRed,dWBGreen,dWBBlue);
		//TRACE("RestoreSettingsLite, mode = %d, factors are %f,%f,%f\n", (int)WBMode,dWBRed,dWBGreen,dWBBlue);
	}
	/*if (nWBMode == kAutoWB)	
	{
		bool bWBShow;
	PxRectEx WBrect;
	UCHAR byWBR,byWBG,byWBB;
	CAM_WB_GetRegion(&bWBShow,&WBrect,&byWBR,&byWBG,&byWBB);
	//bWBShow = IsDlgButtonChecked(IDC_SHOW_WB_RECT)==1;
	CAM_WB_SetMode(kAutoWB);
	double dWBRed,dWBGreen,dWBBlue;		
		CAM_WB_GetFactors(&dWBRed,&dWBGreen,&dWBBlue);
	CAM_WB_SetRegion(bWBShow,&WBrect,byWBR,byWBG,byWBB);
	}
	if (nWBMode == kOneTouchWB)	
	{
		CAM_WB_CalibrateFactors();
	//CAM_WB_SetMode(kAutoWB);
	CAM_WB_SetMode(kOneTouchWB);
	}*/
}

