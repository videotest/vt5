#include "StdAfx.h"
#include "IV4Hlp.h"
#include "StdProfile.h"
#include "DriverWrp.h"
#include "debug.h"

int IV4HlpInitializeCamera(unsigned nCamera, LPCTSTR lpIniPath, IV4CameraConfig *pCfg)
{
	int nUseTopBottom = g_StdProf.GetProfileInt(_T("Source"), _T("UseTopBottom"), -1, true);
	IV4CameraConfig camcfg;
	IV4GetDefaultCameraConfig(IV4_Camera_BW_RS170, &camcfg);
	if (nUseTopBottom == 1)
		camcfg.AD_Convert_Flag = IV4_USE_TOP_BOTTOM;
	else if (nUseTopBottom == 0)
		camcfg.AD_Convert_Flag = IV4_USE_GAIN_OFFSET;
	IV4SetBoardConfigEx(0, &camcfg, NULL);  
	IV4GetDefaultCameraConfig(IV4_Camera_BW_CCIR, &camcfg);
	if (nUseTopBottom == 1)
		camcfg.AD_Convert_Flag = IV4_USE_TOP_BOTTOM;
	else if (nUseTopBottom == 0)
		camcfg.AD_Convert_Flag = IV4_USE_GAIN_OFFSET;
	IV4SetBoardConfigEx(1, &camcfg, NULL);
	IV4GetDefaultCameraConfig(IV4_Camera_RGB_RS170, &camcfg);
	if (nUseTopBottom == 1)
		camcfg.RGB_Flag = IV4_USE_TOP_BOTTOM;
	else if (nUseTopBottom == 0)
		camcfg.RGB_Flag = IV4_USE_GAIN_OFFSET;
	IV4SetBoardConfigEx(2, &camcfg, NULL);  
	IV4GetDefaultCameraConfig(IV4_Camera_RGB_CCIR, &camcfg);
	if (nUseTopBottom == 1)
		camcfg.RGB_Flag = IV4_USE_TOP_BOTTOM;
	else if (nUseTopBottom == 0)
		camcfg.RGB_Flag = IV4_USE_GAIN_OFFSET;
	IV4SetBoardConfigEx(3, &camcfg, NULL);
	BOOL bDirOk = FALSE;
	CString sFileName(lpIniPath);
	TCHAR szCDrive[_MAX_DRIVE],szCDir[_MAX_DIR],szCFName[_MAX_FNAME],szCExt[_MAX_EXT];
	TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
	if (!sFileName.IsEmpty())
	{
		_splitpath(sFileName,szDrive,szDir,szFName,szExt);
		GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
		_splitpath(szPath,szCDrive,szCDir,szCFName,szCExt);
		if (szDrive[0] == 0) strcpy(szDrive, szCDrive);
		if (szDir[0] == 0) strcpy(szDir, szCDir);
		if (szFName[0] == 0) strcpy(szFName, szCFName);
		if (szExt[0] == 0) strcpy(szExt, ".ini");
		_makepath(szPath, szDrive, szDir, szFName, szExt);
		int r = IV4SetBoardConfigEx(4, NULL, szPath);
		if (r == IV4_OK)
		{
			bDirOk = TRUE;
			if (nUseTopBottom >= 0)
			{
				IV4GetBoardConfigEx(4, &camcfg, NULL);
				if (nUseTopBottom == 1)
					camcfg.RGB_Flag = IV4_USE_TOP_BOTTOM;
				else if (nUseTopBottom == 0)
					camcfg.RGB_Flag = IV4_USE_GAIN_OFFSET;
				IV4SetBoardConfigEx(4, &camcfg, NULL);
			}

		}
	}
	if (bDirOk)
		nCamera = nCamera>4?4:nCamera;
	else
		nCamera = nCamera>3?3:nCamera;
	IV4SelectCurrentBoardConfig(nCamera);
	if (pCfg)
	{
		int r = IV4GetBoardConfigEx(nCamera, pCfg, szPath);
		if (r != IV4_OK)
		{
			dprintf("IV4GetBoardConfigEx failed in IV4HlpInitializeCamera, r = %d\n", r);
			memset(pCfg, 0, sizeof(*pCfg));
		}
	}
	return 0;
}

BOOL IV4HlpIsMonoCamera(IV4CameraConfig *pCamCfg)
{
	return pCamCfg->Flag_Digital_Port == IV4_ANALOG_PORT &&
		pCamCfg->Input_Data_Format == IV4_DATA_MONO_8 &&
		pCamCfg->Pixel_Mode == IV4_PM_YUV_16;
}

BOOL IV4HlpIsRGBCamera(IV4CameraConfig *pCamCfg)
{
	return pCamCfg->Flag_Digital_Port == IV4_ANALOG_PORT &&
		pCamCfg->Input_Data_Format == IV4_DATA_RGB_24 &&
		(pCamCfg->Pixel_Mode == IV4_PM_RGB888X_32 || pCamCfg->Pixel_Mode == IV4_PM_RGB888_24);
}

int IV4HlpPostInitCamera(IV4CameraConfig &camcfg, IDriver2 *pDrv1)
{
	CDriverWrp pDrv(pDrv1);
	int r;
	// Set grab window
	BOOL bEnableMargins = pDrv.GetInt(0, _T("Settings"), _T("EnableMargins"), 0, 0);
	if (bEnableMargins)
	{
		int nMode = pDrv.GetInt(0, _T("Settings"), _T("GrabWindowMode"), -1, ID2_STORE);
		int nLeft = pDrv.GetInt(0, _T("Settings"), _T("LeftMargin"), 0, ID2_STORE);
		int nTop = pDrv.GetInt(0, _T("Settings"), _T("TopMargin"), 0, ID2_STORE);
		int nRight = pDrv.GetInt(0, _T("Settings"), _T("RightMargin"), 0, ID2_STORE);
		int nBottom = pDrv.GetInt(0, _T("Settings"), _T("BottomMargin"), 0, ID2_STORE);
		if (nMode == IV4_Frame_Mode || nMode == IV4_Field_Mode || nMode == IV4_NonInterlace_Mode ||
			nLeft > 0 || nTop > 0 || nRight > 0 || nBottom > 0)
		{
			UWORD nMode1,sx1,sy1,dx1,dy1;
			r = IV4GetGrabWindow(&nMode1,&sx1,&sy1,&dx1,&dy1);
			if (r == IV4_OK)
			{
				if (nMode == IV4_Frame_Mode || nMode == IV4_Field_Mode || nMode == IV4_NonInterlace_Mode)
					nMode1 = (UWORD)nMode;
				sx1 = (UWORD)(sx1+nLeft);
				sy1 = (UWORD)(sy1+nTop);
				dx1 = (UWORD)max(dx1-nLeft-nRight,100);
				dy1 = (UWORD)max(dy1-nLeft-nTop,100);
				sx1 = (sx1>>1)<<1;
				dx1 = (dx1>>2)<<2;
				r = IV4SetGrabWindow(nMode1, sx1, sy1, dx1, dy1);
				dprintf("IV4SetGrabWindow in IV4HlpPostInitCamera : %d, %d, (%d,%d,%d,%d)\n", r,
					(int)nMode1, (int)sx1, (int)sy1, (int)dx1, (int)dy1);
			}
			else
				dprintf("IV4GetGrabWindow in IV4HlpPostInitCamera : %d\n", r);
		}
	}
	// Set scale factors
	int nHFactor = pDrv.GetInt(0, _T("Source"), _T("HScale"), 1, ID2_STORE);
	int nVFactor = pDrv.GetInt(0, _T("Source"), _T("VScale"), 1, ID2_STORE);
	if (nHFactor > 0 || nVFactor > 0)
	{
		ULONG uHFactor = nHFactor==1?IV4_HScale_1:nHFactor==2?IV4_HScale_2:nHFactor==4?IV4_HScale_4:
			nHFactor==8?IV4_HScale_8:IV4_HScale_16;
		ULONG uVFactor = nVFactor==1?IV4_VScale_1:nVFactor==2?IV4_VScale_2:nVFactor==4?IV4_VScale_4:
			nVFactor==8?IV4_VScale_8:IV4_VScale_16;
		ULONG uHFactor1, uVFactor1;
		r = IV4GetScaleFactors(&uHFactor1, &uVFactor1);
		if (r == IV4_OK && (uHFactor != uHFactor1 || uVFactor != uVFactor1))
		{
			if (uHFactor == IV4_HScale_1 || uHFactor == IV4_HScale_2 || uHFactor == IV4_HScale_4 ||
				uHFactor == IV4_HScale_8 || uHFactor == IV4_HScale_16)
				uHFactor1 = uHFactor;
			if (uVFactor == IV4_VScale_1 || uVFactor == IV4_VScale_2 || uVFactor == IV4_VScale_4 ||
				uVFactor == IV4_VScale_8 || uVFactor == IV4_VScale_16)
				uVFactor1 = uVFactor;
			r = IV4SetScaleFactors(uHFactor1, uVFactor1);
			dprintf("IV4SetScaleFactors in IV4HlpPostInitCamera : %d (%d,%d)\n", r, (int)uHFactor1,
				(int)uVFactor1);
		}
	}
	r = IV4SetVideoFrame(IV4_Single_Image_Mode, 0, 0, 0);
	// Set source settings, such as top, botton, etc.
	if (IV4HlpIsMonoCamera(&camcfg))
	{
		int nChannel = pDrv.GetInt(0, _T("Source"), _T("Channel"), camcfg.Video_Channel, ID2_STORE);
		if (nChannel >= IV4_Channel_BW_0 && nChannel <= IV4_Channel_BW_3)
		{
			r = IV4SetChannel((BYTE)nChannel);
			if (r != IV4_OK) dprintf("IV4SetChannel in IV4HlpPostInitCamera : %d\n", r);
		}
		if (camcfg.AD_Convert_Flag == IV4_USE_TOP_BOTTOM)
		{
			int nTop = pDrv.GetInt(0, _T("Source"), _T("Top"), camcfg.AD_Convert_Top, ID2_STORE);
			int nBottom = pDrv.GetInt(0, _T("Source"), _T("Bottom"), camcfg.AD_Convert_Bottom, ID2_STORE);
			int nClamp = pDrv.GetInt(0, _T("Source"), _T("Clamp"), camcfg.Clamp_Level, ID2_STORE);
			if (nTop >= 0 && nTop <= 255 && nBottom >= 0 && nBottom <= 255 && nClamp >= 0 && nClamp <= 255)
			{
				r = IV4SetTopBottomEx(IV4_Channel_Current, IV4_Select_BlackWhite, (BYTE)nTop, (BYTE)nBottom, (BYTE)nClamp);
				if (r != IV4_OK) dprintf("IV4SetTopBottomEx in IV4HlpPostInitCamera : %d\n", r);
			}
		}
		else
		{
			int nBrightness = pDrv.GetInt(0, _T("Source"), _T("Brightness"), -1, ID2_STORE);
			if (nBrightness >= 0 && nBrightness <= 255)
			{
				r = IV4SetBrightness((BYTE)nBrightness);
				if (r != IV4_OK) dprintf("IV4SetBrightness in IV4HlpPostInitCamera : %d\n", r);
			}
			double dGain = pDrv.GetDouble(0, _T("Source"), _T("Gain"), camcfg.Video_Gain, ID2_STORE);
			if (dGain >= -3 && dGain <= 18.)
			{
				r = IV4SetGainEx(IV4_Channel_Current, IV4_Select_BlackWhite, (float)dGain);
				if (r != IV4_OK) dprintf("IV4SetGainEx in IV4HlpPostInitCamera : %d, %f\n", r, dGain);
			}
			int nOffset = pDrv.GetInt(0, _T("Source"), _T("Offset"), camcfg.Video_Offset, ID2_STORE);
			if (nOffset >= 0 && nOffset <= 255)
			{
				r = IV4SetOffsetEx(IV4_Channel_Current, IV4_Select_BlackWhite, (BYTE)nOffset);
				if (r != IV4_OK) dprintf("IV4SetOffsetEx in IV4HlpPostInitCamera : %d, %d\n", r, nOffset);
			}
		}
	}
	else if (IV4HlpIsRGBCamera(&camcfg))
	{
		int nChannel = pDrv.GetInt(0, _T("Source"), _T("Channel"), camcfg.Video_Channel, ID2_STORE);
		if (nChannel == IV4_Channel_RGB_0 || nChannel == IV4_Channel_RGB_1)
		{
			r = IV4SetChannel((BYTE)nChannel);
			if (r != IV4_OK) dprintf("IV4SetChannel in IV4HlpPostInitCamera : %d\n", r);
		}
		if (camcfg.RGB_Flag == IV4_USE_TOP_BOTTOM)
		{
			int nTop = pDrv.GetInt(0, _T("Source"), _T("TopRed"), camcfg.RGB_R_Top, ID2_STORE);
			int nBottom = pDrv.GetInt(0, _T("Source"), _T("BottomRed"), camcfg.RGB_R_Bottom, ID2_STORE);
			int nClamp = pDrv.GetInt(0, _T("Source"), _T("ClampRed"), camcfg.RGB_R_Clamp, ID2_STORE);
			if (nTop >= 0 && nTop <= 255 && nBottom >= 0 && nBottom <= 255 && nClamp >= 0 && nClamp <= 255)
			{
				r = IV4SetTopBottomEx(IV4_Channel_Current, IV4_Select_Red, (BYTE)nTop, (BYTE)nBottom, (BYTE)nClamp);
				if (r != IV4_OK) dprintf("IV4SetTopBottomEx in IV4HlpPostInitCamera : %d\n", r);
			}
			nTop = pDrv.GetInt(0, _T("Source"), _T("TopBlue"), camcfg.RGB_R_Top, ID2_STORE);
			nBottom = pDrv.GetInt(0, _T("Source"), _T("BottomBlue"), camcfg.RGB_R_Bottom, ID2_STORE);
			nClamp = pDrv.GetInt(0, _T("Source"), _T("ClampBlue"), camcfg.RGB_R_Clamp, ID2_STORE);
			if (nTop >= 0 && nTop <= 255 && nBottom >= 0 && nBottom <= 255 && nClamp >= 0 && nClamp <= 255)
			{
				r = IV4SetTopBottomEx(IV4_Channel_Current, IV4_Select_Blue, (BYTE)nTop, (BYTE)nBottom, (BYTE)nClamp);
				if (r != IV4_OK) dprintf("IV4SetTopBottomEx in IV4HlpPostInitCamera : %d\n", r);
			}
			nTop = pDrv.GetInt(0, _T("Source"), _T("TopGreen"), camcfg.RGB_G_Top, ID2_STORE);
			nBottom = pDrv.GetInt(0, _T("Source"), _T("BottomGreen"), camcfg.RGB_G_Bottom, ID2_STORE);
			nClamp = pDrv.GetInt(0, _T("Source"), _T("ClampGreen"), camcfg.RGB_G_Clamp, ID2_STORE);
			if (nTop >= 0 && nTop <= 255 && nBottom >= 0 && nBottom <= 255 && nClamp >= 0 && nClamp <= 255)
			{
				r = IV4SetTopBottomEx(IV4_Channel_Current, IV4_Select_Green, (BYTE)nTop, (BYTE)nBottom, (BYTE)nClamp);
				if (r != IV4_OK) dprintf("IV4SetTopBottomEx in IV4HlpPostInitCamera : %d\n", r);
			}
		}
		else
		{
			double dGainRed = pDrv.GetDouble(0, _T("Source"), _T("GainRed"), 0., ID2_STORE);
			if (dGainRed >= -3 && dGainRed <= 18.)
			{
				dprintf("IV4SetGainEx in IV4HlpPostInitCamera 11111: %d, %f\n", r, dGainRed);
				try {
				r = IV4SetGainEx(IV4_Channel_Current, IV4_Select_Red, (float)dGainRed);
				}
				catch(...)
				{
				}
				if (r != IV4_OK) dprintf("IV4SetGainEx in IV4HlpPostInitCamera : %d, %f\n", r, dGainRed);
			}
			double dGainGreen = pDrv.GetDouble(0, _T("Source"), _T("GainGreen"), 0., ID2_STORE);
			if (dGainGreen >= -3 && dGainGreen <= 18.)
			{
				r = IV4SetGainEx(IV4_Channel_Current, IV4_Select_Green, (float)dGainGreen);
				if (r != IV4_OK) dprintf("IV4SetGainEx in IV4HlpPostInitCamera : %d, %f\n", r, dGainGreen);
			}
			double dGainBlue = pDrv.GetDouble(0, _T("Source"), _T("GainBlue"), 0., ID2_STORE);
			if (dGainBlue >= -3 && dGainBlue <= 18.)
			{
				r = IV4SetGainEx(IV4_Channel_Current, IV4_Select_Blue, (float)dGainBlue);
				if (r != IV4_OK) dprintf("IV4SetGainEx in IV4HlpPostInitCamera : %d, %f\n", r, dGainBlue);
			}
			int nOffsetRed = pDrv.GetInt(0, _T("Source"), _T("OffsetRed"), 0, ID2_STORE);
			if (nOffsetRed >= 0 && nOffsetRed <= 255)
			{
				r = IV4SetOffsetEx(IV4_Channel_Current, IV4_Select_Red, (BYTE)nOffsetRed);
				if (r != IV4_OK) dprintf("IV4SetOffsetEx in IV4HlpPostInitCamera : %d, %d\n", r, nOffsetRed);
			}
			int nOffsetGreen = pDrv.GetInt(0, _T("Source"), _T("OffsetGreen"), 0, ID2_STORE);
			if (nOffsetGreen >= 0 && nOffsetGreen <= 255)
			{
				r = IV4SetOffsetEx(IV4_Channel_Current, IV4_Select_Green, (BYTE)nOffsetGreen);
				if (r != IV4_OK) dprintf("IV4SetOffsetEx in IV4HlpPostInitCamera : %d, %d\n", r, nOffsetGreen);
			}
			int nOffsetBlue = pDrv.GetInt(0, _T("Source"), _T("OffsetBlue"), 0, ID2_STORE);
			if (nOffsetBlue >= 0 && nOffsetBlue <= 255)
			{
				r = IV4SetOffsetEx(IV4_Channel_Current, IV4_Select_Blue, (BYTE)nOffsetBlue);
				if (r != IV4_OK) dprintf("IV4SetOffsetEx in IV4HlpPostInitCamera : %d, %d\n", r, nOffsetBlue);
			}
		
		}
	}

	return 0;
}



