#ifndef PIXSDK_H_
#define PIXSDK_H_

//guo #include "PxAbstractCamera.h"

//#define _LOADTIME_SDK
//#define _PIXERA_SDK

//guo  #define UCHAR unsigned char

#define WM_PIX_REPAINT			(WM_USER + 1)
#define	WM_PIX_CAPTURE_DONE		(WM_USER + 2)		//	000801
#define	WM_REMOTE_AE_LOCK		(WM_USER + 3)		//	000808
#define	WM_REMOTE_CAPTURE		(WM_USER + 4)		//	000808
#define	WM_REMOTE_SPOT			(WM_USER + 5)		//	000808
#define	WM_PREVIEW_CAPTURE_DONE (WM_USER + 6)		//	000817

struct PxSizeEx 
{
    long cx;
    long cy;
};

struct PxRectEx
{
	long left;
	long top;
	long right;
	long bottom;
};

enum AutoExposureModeEx
{
	kAEUnlocked,
	kAELocked,
	kManualExposure
};

enum AutoExposureStatusEx
{
	kAENotFunctional = 0,
	kAEUnderExposed = 1,
	kAEGoodExposure = 2,
	kAEOverExposed = 3
};

enum WhiteBalanceModeEx
{
	kWBOff = 0,
	kOneTouchWB = 2,
	kAutoWB = 1,
	kManualWB = 3
};

enum MicroModeEx
{
	kBF = 0,
	kFL = 1
};

enum PreviewOrCaptureEx
{
	kPreview,
	kCapture
};

enum AccumulateMethodEx
{
	kNotAccumulating = 0,
	kAddition = 1,			// Adds the image specified number of times.
	//kAddStopOnOverflow = 2,	// Adds the image specified number of times.
  //sergey 05/02/06						// If detects a data of overflow, stops the accumulate.
	//kAverage = 3			// Averages the image specified number of times.
	kAverage = 2
	//end
};

enum PreviewResolutionEx
{
	kFast,
	kFull,
	kZoom
};

enum StillResolutionEx
{
	kWarp,
	kSingle,
	kVGA
};

enum ImageFileTypeEx			//	000803	NALEX
{
	kImageFileTIFF,				//	000803	NALEX	24 & 48 bit
	kImageFileBMP,				//	000803	NALEX	24 bit ONLY
	kImageFileJPEG,				//	000803	NALEX	not supported
	kImageFileFPX				//	000803	NALEX	not supported
};

enum	PixSDKErrorsEx			//	000803	NALEX
{

	kPixNoError						= 0,
	kPixGenericError				= -1,
	kPixFileTypeUnknownError		= -100,
	kPixFileTypeResolutionError		= -101,
	kPixFileTypeNotSupportedError	= -102,
	kPixImageTypeUnknownError		= -200


};

enum	ChannelEx
{
	kLuminanceChannel,
	kRedChannel,
	kGreenChannel,
	kBlueChannel
};

struct	LevelAdjEx
{
	ChannelEx	nChannel;
	int	nInShadow;
	int	nInHighlight;
	double nInGamma;
	int	nOutShadow;
	int	nOutHighlight;
};

enum	OrientationEx
{
	kNoTransform,
	kFlipHorizontal,
	kFlipVertical,
	kRotate180
};

enum	ColorModeEx
{
	kMonochromeCam,
	kColorCamMono,
	kColorCamColor
};

enum	SharpnessLevelEx
{
	kNoSharpness,
	kMinSharpness,
	kMedSharpness,
	kMaxSharpness
};

#ifdef _PIXERA_SDK
#define LINKDLL __declspec( dllexport)
#else
#define LINKDLL __declspec( dllimport)
#endif

#ifdef  __cplusplus
extern "C" { 
#endif  /* cplusplus */

// *** Setup Device Driver
	BOOL LINKDLL __stdcall CAM_LoadDriver(); 
	typedef BOOL LINKDLL (__stdcall *CAM_LOADDRIVERPROC)(); 

	void LINKDLL __stdcall CAM_UnloadDriver();
	typedef void LINKDLL (__stdcall *CAM_UNLOADDRIVERPROC) ();

	BOOL LINKDLL __stdcall CAM_IsDriverLoaded();
	typedef BOOL LINKDLL (__stdcall *CAM_ISDRIVERLOADEDPROC) ();

	void LINKDLL __stdcall CAM_SetAppHandle();
	typedef void LINKDLL (__stdcall *CAM_SETAPPHANDLEPROC) ();

	void LINKDLL __stdcall CAM_GetAppHandle();
	typedef void LINKDLL (__stdcall *CAM_GETAPPHANDLEPROC) ();

	// ??? don't know how to register
	void LINKDLL __stdcall CAM_RegisterErrorHookFunction();
	typedef void LINKDLL (__stdcall *CAM_REGISTERERRORHOOKFUNCTIONPROC) ();

	void LINKDLL __stdcall CAM_GetVersionOfDriver(short *, short *, short *);
	typedef void LINKDLL (__stdcall *CAM_GETVERSIONOFDRIVERPROC) (short *, short *, short *);

	void LINKDLL __stdcall CAM_GetVersion(short *, short *, short *);
	typedef void LINKDLL (__stdcall *CAM_GETVERSIONPROC) (short *, short *, short *);

// *** Setup Camera Condition

	void LINKDLL __stdcall CAM_Init(ColorModeEx theColorMode = kColorCamColor);
	//typedef void LINKDLL (__stdcall *CAM_INITPROC) (ColorModeEx theColorMode = kColorCamColor);

//	void LINKDLL __stdcall CAM_InitMonoCam();
//	typedef void LINKDLL (__stdcall *CAM_INITMONOCAMPROC) ();

	void LINKDLL __stdcall CAM_Exit();
	typedef void LINKDLL (__stdcall *CAM_EXITPROC) ();

	void LINKDLL __stdcall CAM_ClearAll();
	typedef void LINKDLL (__stdcall *CAM_CLEARALLPROC) ();

	void LINKDLL __stdcall CAM_PowerOn();
	typedef void LINKDLL (__stdcall *CAM_POWERONPROC) ();

	void LINKDLL __stdcall CAM_PowerOff();
	typedef void LINKDLL (__stdcall *CAM_POWEROFFPROC) ();

	BOOL LINKDLL __stdcall CAM_IsPowerOn();
	typedef BOOL LINKDLL (__stdcall *CAM_ISPOWERONPROC) ();

	BOOL LINKDLL __stdcall CAM_IsConnected();
	typedef BOOL LINKDLL (__stdcall *CAM_ISCONNECTEDPROC) ();

	void LINKDLL __stdcall CAM_CoolOn();
	typedef void LINKDLL (__stdcall *CAM_COOLONPROC) ();

	void LINKDLL __stdcall CAM_CoolOff();
	typedef void LINKDLL (__stdcall *CAM_COOLOFFPROC) ();

	BOOL LINKDLL __stdcall CAM_IsCoolOn();
	typedef BOOL LINKDLL (__stdcall *CAM_ISCOOLONPROC) ();

	// ??? This might not be supported 
	void LINKDLL __stdcall CAM_GetTemperature();
	typedef void LINKDLL (__stdcall *CAM_GETTEMPERATUREPROC) ();


// *** Automatic Exposure and Photometry

	void LINKDLL __stdcall CAM_AE_SetSpotSize(bool, PxRectEx*, UCHAR, UCHAR, UCHAR);
	typedef void LINKDLL (__stdcall *CAM_AE_SETSPOTSIZEPROC) (bool, PxRectEx *, UCHAR, UCHAR, UCHAR);

	void LINKDLL __stdcall CAM_AE_GetSpotSize(bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);
	typedef void LINKDLL (__stdcall *CAM_AE_GETSPOTSIZEPROC) (bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);

	void LINKDLL __stdcall CAM_AE_SetSpotPosition(bool, PxRectEx*, UCHAR, UCHAR, UCHAR);
	typedef void LINKDLL (__stdcall *CAM_AE_SETSPOTPOSITIONPROC) (bool, PxRectEx*, UCHAR, UCHAR, UCHAR);

	void LINKDLL __stdcall CAM_AE_GetSpotPosition(bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);
	typedef void LINKDLL (__stdcall *CAM_AE_GETSPOTPOSITIONPROC) (bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);

	void LINKDLL __stdcall CAM_AE_GetSpotRegion(bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);
	typedef void LINKDLL (__stdcall *CAM_AE_GETSPOTREGIONPROC) (bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);


	void LINKDLL __stdcall CAM_AE_Start();
	typedef void LINKDLL (__stdcall *CAM_AE_STARTPROC) ();

	void LINKDLL __stdcall CAM_AE_Stop();
	typedef void LINKDLL (__stdcall *CAM_AE_STOPPROC) ();

	void LINKDLL __stdcall CAM_AE_Lock(bool);
	typedef void LINKDLL (__stdcall *CAM_AE_LOOKPROC) (bool);

	AutoExposureModeEx LINKDLL __stdcall CAM_AE_GetMode();
	typedef AutoExposureModeEx LINKDLL (__stdcall *CAM_AE_GETMODEPROC) ();

	AutoExposureStatusEx LINKDLL __stdcall CAM_AE_GetStatus();
	typedef AutoExposureStatusEx LINKDLL (__stdcall *CAM_AE_GETSTATUSPROC) ();

	BOOL LINKDLL __stdcall CAM_AE_SetAdjust(int);
	typedef BOOL LINKDLL (__stdcall *CAM_AE_SETADJUSTPROC) (int);

	int LINKDLL __stdcall CAM_AE_GetAdjust();
	typedef int LINKDLL (__stdcall *CAM_AE_GETADJUSTPROC) ();

	void LINKDLL __stdcall CAM_AE_SetEvalValue(int);
	typedef void LINKDLL (__stdcall *CAM_AE_SETEVALVALUEPROC) (int);

	int LINKDLL __stdcall CAM_AE_GetEvalValue();
	typedef int LINKDLL (__stdcall *CAM_AE_GETEVALVALUEPROC) ();

	void LINKDLL __stdcall CAM_AE_SetSpotColor(bool, PxRectEx*, UCHAR, UCHAR, UCHAR);
	typedef void LINKDLL (__stdcall *CAM_AE_SETSPOTCOLORPROC) (bool, PxRectEx*, UCHAR, UCHAR, UCHAR);

	void LINKDLL __stdcall CAM_AE_GetSpotColor(bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);
	typedef void LINKDLL (__stdcall *CAM_AE_GETSPOTCOLORPROC) (bool*, PxRectEx*, UCHAR*, UCHAR*, UCHAR*);

	void LINKDLL __stdcall CAM_AE_DrawSpot(bool, PxRectEx*, UCHAR, UCHAR, UCHAR);
	typedef void LINKDLL (__stdcall *CAM_AE_DRAWSPOTPROC) (bool, PxRectEx*, UCHAR, UCHAR, UCHAR);

	void LINKDLL __stdcall CAM_AE_SetMicroMode(MicroModeEx);
	typedef void LINKDLL (__stdcall *CAM_AE_SETMICROMODE) (MicroModeEx);

	void LINKDLL __stdcall CAM_AE_GetMicroMode(MicroModeEx*);
	typedef void LINKDLL (__stdcall *CAM_AE_GETMICROMODE) (MicroModeEx*);

// *** Color Balance 

	void LINKDLL __stdcall CAM_WB_SetMode(WhiteBalanceModeEx);
	typedef void LINKDLL (__stdcall *CAM_WB_SETMODEPROC) (WhiteBalanceModeEx);

	WhiteBalanceModeEx LINKDLL __stdcall CAM_WB_GetMode();
	typedef WhiteBalanceModeEx LINKDLL (__stdcall *CAM_WB_GETMODEPROC) ();

	void LINKDLL __stdcall CAM_WB_SetRegion(bool, PxRectEx*, UCHAR, UCHAR, UCHAR);
	typedef void LINKDLL (__stdcall *CAM_WB_SETREGIONPROC) ( bool, PxRectEx*,UCHAR, UCHAR, UCHAR);

	void LINKDLL __stdcall CAM_WB_GetRegion( bool *,PxRectEx*, UCHAR*, UCHAR*, UCHAR*);
	typedef void LINKDLL (__stdcall *CAM_WB_GETREGIONPROC) ( bool*,PxRectEx*, UCHAR*, UCHAR*, UCHAR*);

	void LINKDLL __stdcall CAM_BB_SetRegion( bool, PxRectEx*,UCHAR, UCHAR, UCHAR);
	typedef void LINKDLL (__stdcall *CAM_BB_SETREGIONPROC) ( bool, PxRectEx*,UCHAR, UCHAR, UCHAR);

	void LINKDLL __stdcall CAM_BB_GetRegion( bool*, PxRectEx*,UCHAR*, UCHAR*, UCHAR*);
	typedef void LINKDLL (__stdcall *CAM_BB_GETREGIONPROC) ( bool*, PxRectEx*,UCHAR*, UCHAR*, UCHAR*);

	void LINKDLL __stdcall CAM_WB_SetFactors(double, double, double);
	typedef void LINKDLL (__stdcall *CAM_WB_SETFACTORSPROC) (double, double, double);

	void LINKDLL __stdcall CAM_WB_GetFactors(double *, double *, double *);
	typedef void LINKDLL (__stdcall *CAM_WB_GETFACTORSPROC) (double *, double *, double *);

	void LINKDLL __stdcall CAM_BB_SetFactors(double, double, double);
	typedef void LINKDLL (__stdcall *CAM_BB_SETFACTORSPROC) (double, double, double);

	void LINKDLL __stdcall CAM_BB_GetFactors(double *, double *, double *);
	typedef void LINKDLL (__stdcall *CAM_BB_GETFACTORSPROC) (double *, double *, double *);

	void LINKDLL __stdcall CAM_WB_CalibrateFactors();
	typedef void LINKDLL (__stdcall *CAM_WB_CALIBRATEFACTORSPROC) ();

	void LINKDLL __stdcall CAM_BB_CalibrateFactors();
	typedef void LINKDLL (__stdcall *CAM_BB_CALIBRATEFACTORSPROC) ();

	void LINKDLL __stdcall CAM_WB_SetRegionColor();
	typedef void LINKDLL (__stdcall *CAM_WB_SETREGIONCOLORPROC) ();

	void LINKDLL __stdcall CAM_WB_GetRegionColor();
	typedef void LINKDLL (__stdcall *CAM_WB_GETREGIONCOLORPROC) ();

	void LINKDLL __stdcall CAM_WB_DrawRegion();
	typedef void LINKDLL (__stdcall *CAM_WB_DRAWREGIONPROC) ();

	void LINKDLL __stdcall CAM_BB_SetRegionColor();
	typedef void LINKDLL (__stdcall *CAM_WB_SETREGIONCOLORPROC) ();

	void LINKDLL __stdcall CAM_BB_GetRegionColor();
	typedef void LINKDLL (__stdcall *CAM_BB_GETREGIONCOLORPROC) ();

	void LINKDLL __stdcall CAM_BB_DrawRegion();
	typedef void LINKDLL (__stdcall *CAM_BB_DRAWREGIONPROC) ();

	void LINKDLL __stdcall CAM_BB_Start();
	typedef void LINKDLL (__stdcall *CAM_BB_START) ();

	void LINKDLL __stdcall CAM_BB_Stop();
	typedef void LINKDLL (__stdcall *CAM_BB_STOP) ();


// *** Exposure Condition

	BOOL LINKDLL __stdcall CAM_EXP_SetSpeed(double);
	typedef BOOL LINKDLL (__stdcall *CAM_EXP_SETSPEEDPROC) (double);

	void LINKDLL __stdcall CAM_EXP_GetSpeed(double *);
	typedef void LINKDLL (__stdcall *CAM_EXP_GETSPEEDPROC) (double *);

	BOOL LINKDLL __stdcall CAM_EXP_SetSensitivity(int);
	typedef BOOL LINKDLL (__stdcall *CAM_EXP_SETSENSITIVITYPROC) (int);

	void LINKDLL __stdcall CAM_EXP_GetSensitivity(int *);
	typedef void LINKDLL (__stdcall *CAM_EXP_GETSENSITIVITYPROC) (int *);

// *** Image Processing
	void LINKDLL __stdcall CAM_IP_LoadLevelTable();
	typedef void LINKDLL (__stdcall *CAM_IP_LOADLEVELTABLEPROC) ();

	void LINKDLL __stdcall CAM_IP_SetLuminanceLevels(LevelAdjEx *);
	typedef void LINKDLL (__stdcall *CAM_IP_SETLUMINANCELEVELSPROC) (LevelAdjEx *);

	void LINKDLL __stdcall CAM_IP_GetLuminanceLevels(LevelAdjEx *);
	typedef void LINKDLL (__stdcall *CAM_IP_GETLUMINANCELEVELSPROC) (LevelAdjEx *);

	void LINKDLL __stdcall CAM_IP_SetOrientation(OrientationEx);
	typedef void LINKDLL (__stdcall *CAM_IP_SETORIENTATIONPROC) (OrientationEx);

	OrientationEx LINKDLL __stdcall CAM_IP_GetOrientation();
	typedef OrientationEx LINKDLL (__stdcall *CAM_IP_GETORIENTATIONPROC) ();

	void LINKDLL __stdcall CAM_IP_ColorCapture(ColorModeEx);
	typedef void LINKDLL (__stdcall *CAM_IP_COLORCAPTUREPROC) (ColorModeEx);

	ColorModeEx LINKDLL __stdcall CAM_IP_IsColorCapture();
	typedef ColorModeEx LINKDLL (__stdcall *CAM_IP_ISCOLORCAPTUREPROC) ();

	void LINKDLL __stdcall CAM_IP_SetCaptureSharpness(SharpnessLevelEx);
	typedef void LINKDLL (__stdcall *CAM_IP_SETCAPTURESHARPNESSPROC) (SharpnessLevelEx);

	SharpnessLevelEx LINKDLL __stdcall CAM_IP_GetCaptureSharpness();
	typedef SharpnessLevelEx LINKDLL (__stdcall *CAM_IP_GETCAPTURESHARPNESSPROC) ();

	void LINKDLL __stdcall CAM_IP_SetMedianFilter(bool);
	typedef void LINKDLL (__stdcall *CAM_IP_SETMEDIANFILTERPROC) (bool);

	bool LINKDLL __stdcall CAM_IP_GetMedianFilter();
	typedef bool LINKDLL (__stdcall *CAM_IP_GETMEDIANFILTERPROC) ();

/*
	void LINKDLL __stdcall CAM_IP_EnableSharpnessFilter();
	typedef void LINKDLL (__stdcall *CAM_IP_ENABLESHARPNESSFILTERPROC) ();

	void LINKDLL __stdcall CAM_IP_IsSharpnessFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_IP_ISSHARPNESSFILTERENABLEDPROC) ();
*/

// *** Color Compensation

	void LINKDLL __stdcall CAM_CB_LoadColorMatrix();
	typedef void LINKDLL (__stdcall *CAM_CB_LOADCOLORMATRIXPROC) ();

	void LINKDLL __stdcall CAM_CB_SetRedFactor();
	typedef void LINKDLL (__stdcall *CAM_CB_SETREDFACTORPROC) ();

	void LINKDLL __stdcall CAM_CB_GetRedFactor();
	typedef void LINKDLL (__stdcall *CAM_CB_GETREDFACTORPROC) ();

	void LINKDLL __stdcall CAM_CB_SetGreenFactor();
	typedef void LINKDLL (__stdcall *CAM_CB_SETGREENFACTORPROC) ();

	void LINKDLL __stdcall CAM_CB_GetGreenFactor();
	typedef void LINKDLL (__stdcall *CAM_CB_GETGREENFACTORPROC) ();

	void LINKDLL __stdcall CAM_CB_SetBlueFactor();
	typedef void LINKDLL (__stdcall *CAM_CB_SETBLUEFACTORPROC) ();

	void LINKDLL __stdcall CAM_CB_GetBlueFactor();
	typedef void LINKDLL (__stdcall *CAM_CB_GETBLUEFACTORPROC) ();



// *** Focus Finder
	void LINKDLL __stdcall CAM_FF_Start();
	typedef void LINKDLL (__stdcall *CAM_FF_STARTPROC) ();

	void LINKDLL __stdcall CAM_FF_Stop();
	typedef void LINKDLL (__stdcall *CAM_FF_STOPPROC) ();

	BOOL LINKDLL __stdcall CAM_FF_IsRunning();
	typedef BOOL LINKDLL (__stdcall *CAM_FF_ISRUNNINGPROC) ();

	void LINKDLL __stdcall CAM_FF_GetEvalValue(int* pnCurrent, int* pnPeak);
	typedef void LINKDLL (__stdcall *CAM_FF_GETEVALVALUEPROC) (int* pnCurrent, int* pnPeak);

	void LINKDLL __stdcall CAM_FF_GetFineEvalValue(int* pnCurrent, int* pnPeak);
	typedef void LINKDLL (__stdcall *CAM_FF_GETFINEEVALVALUEPROC) (int* pnCurrent, int* pnPeak);

	void LINKDLL __stdcall CAM_FF_Reset();
	typedef void LINKDLL (__stdcall *CAM_FF_RESETPROC) ();

	void LINKDLL __stdcall CAM_FF_SetRegion(bool bDrawingOnOff, PxRectEx *pRect, UCHAR red, UCHAR green, UCHAR blue);
	typedef void LINKDLL (__stdcall *CAM_FF_SETREGIONPROC) (bool bDrawingOnOff, PxRectEx *pRect, UCHAR red, UCHAR green, UCHAR blue);

	void LINKDLL __stdcall CAM_FF_GetRegion(bool *bDrawingOnOff, PxRectEx *pRect, UCHAR *red, UCHAR *green, UCHAR *blue);
	typedef void LINKDLL (__stdcall *CAM_FF_GETREGIONPROC) (bool *bDrawingOnOff, PxRectEx *pRect, UCHAR *red, UCHAR *green, UCHAR *blue);	

// *** Preview Observation and Acquisition

//	BOOL LINKDLL __stdcall CAM_MIA_SetResolution(PxSizeEx, bool, bool, int, bool, PreviewOrCaptureEx);
//	typedef BOOL LINKDLL (__stdcall *CAM_MIA_SETRESOLUTIONPROC) (PxSizeEx, bool, bool, int, bool, PreviewOrCaptureEx);

	BOOL LINKDLL __stdcall CAM_MIA_SetResolution(PreviewResolutionEx);
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_SETRESOLUTIONPROC) (PreviewResolutionEx);

	void LINKDLL __stdcall CAM_MIA_GetResolution(PreviewResolutionEx *, PxSizeEx *);
	typedef void LINKDLL (__stdcall *CAM_MIA_GETRESOLUTIONPROC) (PreviewResolutionEx *, PxSizeEx *);

	void LINKDLL __stdcall CAM_MIA_GetPixelSize(double *, double *);
	typedef void LINKDLL (__stdcall *CAM_MIA_GETPIXELSIZEPROC) (double *, double *);

	BOOL LINKDLL __stdcall CAM_MIA_StartAutoDraw(HANDLE *, HDC);
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_STARTAUTODRAWPROC) (HANDLE *, HDC);

	BOOL LINKDLL __stdcall CAM_MIA_Start(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_STARTPROC) (HANDLE *);

	void LINKDLL __stdcall CAM_MIA_Stop();
	typedef void LINKDLL (__stdcall *CAM_MIA_STOPPROC) ();

	BOOL LINKDLL __stdcall CAM_MIA_IsRunning();
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_ISRUNNINGPROC) ();

	void LINKDLL __stdcall CAM_MIA_SetAEEvalFunc();
	typedef void LINKDLL (__stdcall *CAM_MIA_SETAEEVALFUNCPROC) ();

	void LINKDLL __stdcall CAM_MIA_GetAEEvalFunc();
	typedef void LINKDLL (__stdcall *CAM_MIA_GETAEEVALFUNCPROC) ();

	void LINKDLL __stdcall CAM_MIA_SetFocusEvalFunc();
	typedef void LINKDLL (__stdcall *CAM_MIA_SETFOCUSEVALFUNCPROC) ();

	void LINKDLL __stdcall CAM_MIA_GetFocusEvalFunc();
	typedef void LINKDLL (__stdcall *CAM_MIA_GETFOCUSEVALFUNCPROC) ();

	void LINKDLL __stdcall CAM_MIA_SetPreDrawFunc(void (*fp)(void *,void *));
	typedef void LINKDLL (__stdcall *CAM_MIA_SETPREDRAWFUNCPROC) (void (*fp)(void *,void *));

	void LINKDLL __stdcall CAM_MIA_GetPreDrawFunc();
	typedef void LINKDLL (__stdcall *CAM_MIA_GETPREDRAWFUNCPROC) ();

	void LINKDLL __stdcall CAM_MIA_SetPostDrawFunc(void (*fp)(void *,void *));
	typedef void LINKDLL (__stdcall *CAM_MIA_SETPOSTDRAWFUNCPROC) (void (*fp)(void *,void *));

	void LINKDLL __stdcall CAM_MIA_GetPostDrawFunc();
	typedef void LINKDLL (__stdcall *CAM_MIA_GETPOSTDRAWFUNCPROC) ();

	void LINKDLL __stdcall CAM_MIA_GetImageData();
	typedef void LINKDLL (__stdcall *CAM_MIA_GETIMAGEDATAPROC) ();

	BOOL LINKDLL __stdcall CAM_MIA_Capture(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_CAPTUREPROC) ();

	BOOL LINKDLL __stdcall CAM_MIA_CaptureFull(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_CAPTUREFULLPROC) ();

	BOOL LINKDLL __stdcall CAM_MIA_SaveDIBIntoBMP(char* szFile);
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_SAVEDIBINTOBMPPROC) (char* szFile);

	BOOL LINKDLL __stdcall CAM_MIA_SetHDC(HDC	theHDC);
	typedef BOOL LINKDLL (__stdcall *CAM_MIA_SETHDCPROC) (HDC theHDC);

// Getting Still Image

	void LINKDLL __stdcall CAM_SIA_SetResolution(StillResolutionEx);
	typedef void LINKDLL (__stdcall *CAM_SIA_SETRESOLUTIONPROC) (StillResolutionEx);

	void LINKDLL __stdcall CAM_SIA_GetResolution(StillResolutionEx *, PxSizeEx *);
	typedef void LINKDLL (__stdcall *CAM_SIA_GETRESOLUTIONPROC) (StillResolutionEx *, PxSizeEx *);

	void LINKDLL __stdcall CAM_SIA_GetPixelSize(double *, double *);
	typedef void LINKDLL (__stdcall *CAM_SIA_GETPIXELSIZEPROC) (double *, double *);

	void LINKDLL __stdcall CAM_SIA_SetRegion();
	typedef void LINKDLL (__stdcall *CAM_SIA_SETREGIONPROC) ();

	void LINKDLL __stdcall CAM_SIA_GetRegion();
	typedef void LINKDLL (__stdcall *CAM_SIA_GETREGIONPROC) ();

	void LINKDLL __stdcall CAM_SIA_SetAccumulateTimes(int);
	typedef void LINKDLL (__stdcall *CAM_SIA_SETACCUMLATETIMESPROC) (int);

	void LINKDLL __stdcall CAM_SIA_GetAccumulateTimes(int *);
	typedef void LINKDLL (__stdcall *CAM_SIA_GETACCUMLATETIMESPROC) (int *);

	void LINKDLL __stdcall CAM_SIA_SetAccumulateMode(AccumulateMethodEx);
	typedef void LINKDLL (__stdcall *CAM_SIA_SETACCUMLATEMODEPROC) (AccumulateMethodEx);

	void LINKDLL __stdcall CAM_SIA_GetAccumulateMode(AccumulateMethodEx *);
	typedef void LINKDLL (__stdcall *CAM_SIA_GETACCUMLATEMODEPROC) (AccumulateMethodEx *);

	BOOL LINKDLL __stdcall CAM_SIA_Start24Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_START24BITPROC) (HANDLE *);

	BOOL LINKDLL __stdcall CAM_SIA_Start8Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_START8BITPROC) (HANDLE *);

	BOOL LINKDLL __stdcall CAM_SIA_Start48Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_START48BITPROC) (HANDLE *);

	BOOL LINKDLL __stdcall CAM_SIA_Start16Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_START16BITPROC) (HANDLE *);

	void LINKDLL __stdcall CAM_SIA_Stop();
	typedef void LINKDLL (__stdcall *CAM_SIA_STOPPROC) ();

	BOOL LINKDLL __stdcall CAM_SIA_IsRunning();
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_ISRUNNINGPROC) ();

	void LINKDLL __stdcall CAM_SIA_SetRegionColor();
	typedef void LINKDLL (__stdcall *CAM_SIA_SETREGIONCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIA_GetRegionColor();
	typedef void LINKDLL (__stdcall *CAM_SIA_GETREGIONCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIA_DrawRegion();
	typedef void LINKDLL (__stdcall *CAM_SIA_DRAWREGIONPROC) ();

	BOOL LINKDLL __stdcall CAM_SIA_Prep8Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_PREP8BITPROC) (HANDLE *);

	BOOL LINKDLL __stdcall CAM_SIA_Prep16Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_PREP16BITPROC) (HANDLE *);

	BOOL LINKDLL __stdcall CAM_SIA_Prep24Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_PREP24BITPROC) (HANDLE *);

	BOOL LINKDLL __stdcall CAM_SIA_Prep48Bit(HANDLE *);
	typedef BOOL LINKDLL (__stdcall *CAM_SIA_PREP48BITPROC) (HANDLE *);

	void LINKDLL __stdcall CAM_SIA_CaptureExpose();
	typedef void LINKDLL (__stdcall *CAM_SIA_CAPTUREEXPOSEPROC) ();

	void LINKDLL __stdcall CAM_SIA_CaptureProcess();
	typedef void LINKDLL (__stdcall *CAM_SIA_CAPTUREPROCESSPROC) ();


// *** Option of Still Image

	void LINKDLL __stdcall CAM_SIO_EnableDocFilter();
	typedef void LINKDLL (__stdcall *CAM_SIO_ENABLEDOCFILTERPROC) ();

	void LINKDLL __stdcall CAM_SIO_IsDocFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_SIO_ISDOCFILTERENABLEDPROC) ();

	void LINKDLL __stdcall CAM_SIO_EnableLowpassFilter();
	typedef void LINKDLL (__stdcall *CAM_SIO_ENABLELOWPASSFILTERPROC) ();

	void LINKDLL __stdcall CAM_SIO_IsLowpassFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_SIO_ISLOWPASSFILTERENABLEDPROC) ();

	void LINKDLL __stdcall CAM_SIO_EnableShadingFilter();
	typedef void LINKDLL (__stdcall *CAM_SIO_ENABLESHADINGFILTERPROC) ();

	void LINKDLL __stdcall CAM_SIO_IsShadingFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_SIO_ISSHADINGFILTERENABLEDPROC) ();

	void LINKDLL __stdcall CAM_SIO_EnableAntialiasFilter();
	typedef void LINKDLL (__stdcall *CAM_SIO_ENABLEANTIALIASFILTERPROC) ();

	void LINKDLL __stdcall CAM_SIO_IsAntialiasFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_SIO_ISANTIALIASFILTERENABLEDPROC) ();

	void LINKDLL __stdcall CAM_SIO_EnableColorSmoothingFilter();
	typedef void LINKDLL (__stdcall *CAM_SIO_ENABLECOLORSMOOTHINGFILTERPROC) ();

	void LINKDLL __stdcall CAM_SIO_IsColorSmoothingFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_SIO_ISCOLORSMOOTHINGFILTERENABLEDPROC) ();

	void LINKDLL __stdcall CAM_SIO_EnableCCDMaskFilter();
	typedef void LINKDLL (__stdcall *CAM_SIO_ENABLECCDMASKFILTERPROC) ();

	void LINKDLL __stdcall CAM_SIO_IsCCDMaskFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_SIO_ISCCDMASKFILTERENABLEDPROC) ();

	void LINKDLL __stdcall CAM_SIO_EnableEdgeContourFilter();
	typedef void LINKDLL (__stdcall *CAM_SIO_ENABLEEDGECONTOURFILTERPROC) ();

	void LINKDLL __stdcall CAM_SIO_IsEdgeContourFilterEnabled();
	typedef void LINKDLL (__stdcall *CAM_SIO_ISEDGECONTOURFILTERENABLEDPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetTimeStampMode();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETTIMESTAMPMODEPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetTimeStampMode();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETTIMESTAMPMODEPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetTimeStampColor();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETTIMESTAMPCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetTimeStampColor();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETTIMESTAMPCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetTimeStampPos();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETTIMESTAMPPOSPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetTimeStampPos();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETTIMESTAMPPOSPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetIDStampMode();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETIDSTAMPMODEPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetIDStampMode();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETIDSTAMPMODEPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetIDStampText();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETIDSTAMPTEXTPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetIDStampText();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETIDSTAMPTEXTPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetIDStampColor();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETIDSTAMPCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetIDStampColor();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETIDSTAMPCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetIDStampPos();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETIDSTAMPPOSPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetIDStampPos();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETIDSTAMPPOSPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetScaleStampMode();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETSCALESTAMPMODEPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetScaleStampMode();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETSCALESTAMPMODEPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetObjectMagnification();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETOBJECTMAGNIFICATIONPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetObjectMagnification();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETOBJECTMAGNIFICATIONPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetZoomMagnification();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETZOOMMAGNIFICATIONPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetZoomMagnification();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETZOOMMAGNIFICATIONPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetTubeMagnification();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETTUBEMAGNIFICATIONPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetTubeMagnification();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETTUBEMAGNIFICATIONPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetScaleStampColor();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETSCALESTAMPCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetScaleStampColor();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETSCALESTAMPCOLORPROC) ();

	void LINKDLL __stdcall CAM_SIO_SetScaleStampPos();
	typedef void LINKDLL (__stdcall *CAM_SIO_SETSCALESTAMPPOSPROC) ();

	void LINKDLL __stdcall CAM_SIO_GetScaleStampPos();
	typedef void LINKDLL (__stdcall *CAM_SIO_GETSCALESTAMPPOSPROC) ();

// *** Store or Restore the conditions

	void LINKDLL __stdcall CAM_StoreUserConditions();
	typedef void LINKDLL (__stdcall *CAM_STOREUSERCONDITIONSPROC) ();

	void LINKDLL __stdcall CAM_RestoreUserConditions();
	typedef void LINKDLL (__stdcall *CAM_RESTOREUSERCONDITIONSPROC) ();

	void LINKDLL __stdcall CAM_StoreCharacteristicConditions();
	typedef void LINKDLL (__stdcall *CAM_STORECHARACTERISTICCONDITIONSPROC) ();

	void LINKDLL __stdcall CAM_RestoreCharacteristicConditions();
	typedef void LINKDLL (__stdcall *CAM_RESTORECHARACTERISTICCONDITIONSPROC) ();

// *** Dialog Box and Tool Bar

	void LINKDLL __stdcall CAM_COLDLG_Show();
	typedef void LINKDLL (__stdcall *CAM_COLDLG_SHOWPROC) ();

	void LINKDLL __stdcall CAM_OPTDLG_Show();
	typedef void LINKDLL (__stdcall *CAM_OPTDLG_SHOWPROC) ();

// *** Image File

	void LINKDLL __stdcall CAM_IMG_ConvertToDDB();
	typedef void LINKDLL (__stdcall *CAM_IMG_CONVERTTODDBPROC) ();

	void LINKDLL __stdcall CAM_IMG_ConvertToDIB();
	typedef void LINKDLL (__stdcall *CAM_IMG_CONVERTTODIBPROC) ();

	void LINKDLL __stdcall CAM_IMG_GetBriefInfoIndirect();
	typedef void LINKDLL (__stdcall *CAM_IMG_GETBRIEFINFOINDIRECTPROC) ();

	void LINKDLL __stdcall CAM_IMG_GetBriefFileInfoIndirect();
	typedef void LINKDLL (__stdcall *CAM_IMG_GETBRIEFFILEINFOINDIRECTPROC) ();

	void LINKDLL __stdcall CAM_IMG_Load();
	typedef void LINKDLL (__stdcall *CAM_IMG_LOADPROC) ();

	int LINKDLL __stdcall CAM_IMG_Save(ImageFileTypeEx type,char *fullPathName,void *theImage);			//	000803	NALEX
	typedef int LINKDLL (__stdcall *CAM_IMG_SAVEPROC) (ImageFileTypeEx, char *,void *);

	void LINKDLL __stdcall CAM_IMGDLG_Load();
	typedef void LINKDLL (__stdcall *CAM_IMGDLG_LOADPROC) ();

	void LINKDLL __stdcall CAM_IMGDLG_Save();
	typedef void LINKDLL (__stdcall *CAM_IMGDLG_SAVEPROC) ();



// *** Image Display

	void LINKDLL __stdcall CAM_DSP_ShowFullScreen();
	typedef void LINKDLL (__stdcall *CAM_DSP_SHOWFULLSCREENPROC) ();


#ifdef  __cplusplus
}
#endif  /* cplusplus */


#ifdef _LOADTIME_SDK 

// TODO: register the path to registry later
// #pragma comment(lib, "C:\\Windows\\ToDll.lib")

#endif

#endif // PIXSDK_H_






