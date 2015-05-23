#if !defined(__CxLibWork_H__)
#define __CxLibWork_H__

#include "CxLibApi.h"
#include "input.h"
#include "VT5Profile.h"


enum GrayMode
{
	NoneGrayMode = -1,
	GrayPlane    = 0,
	RedPlane     = 1,
	GreenPlane   = 2,
	BluePlane    = 3,
};

enum Conversion
{
	AutoConversion = 0,
	ManualConversion = 1,
	NoConversion = 2,
};

bool IsDC300(int type);


class CCxLibWork
{
	int m_nInitRes;
	int m_nInitCount;
	int m_nStartCount;
	bool m_bImageCaptured;
	CString m_sMethName; // name of current configuration
	void DoFree();
	void DoStop();
public:
	tCameraType m_CameraType;
	tSHT m_sht;
	tCameraStatus m_CameraStatus;
	int m_nCamera;
	LPVOID m_lpBuff; // Main input buffer
	LPBITMAPINFOHEADER m_lpbi;
	LPBYTE m_lpData;
	LPBITMAPINFOHEADER m_lpbi16; //LPWORD m_lpData16; // Used for input of 16-bit images
//	LPBITMAPINFOHEADER m_lpbiGray; // Used for draw and input of gray-scale images
//	LPBYTE m_lpDataGray;
	LPBYTE m_lpConv; // conversion buffer for conversion 10->8, 2**16 elements
	LPBYTE m_lpConv12; // conversion buffer for conversion 12->8, 2**16 elements
	int m_nGrayMode;
	BOOL m_bRedrawOnSetup;
	int m_cx;
	int m_cy;
	int m_bpp;
//	int m_nGetBmpMode;
	int m_nBitsMode;  // Sames as parameter in SetPixelWidth(...)
	int m_nConversion;// enum Conversion
	int m_nConvStart; // For 10-bit mode, Used if m_nConversion == ManualConversion
	int m_nConvEnd;   // For 10-bit mode, Used if m_nConversion == ManualConversion
	double m_dGamma;  // Gamma for 10-bit mode
	int m_nConvStart12; // For 12-bit mode, Used if m_nConversion == ManualConversion
	int m_nConvEnd12;   // For 12-bit mode, Used if m_nConversion == ManualConversion
	double m_dGamma12; // Gamma for 12-bit mode
	BOOL m_bForCapture; // Thread now running for capture
	BOOL m_bShowSetupTime;
	int m_nMaxShutter; // Maximal shutter time (mcs) for DoSetup
	int m_nFoundShutter; // Set in DoSetup
	BOOL m_bEnableMargins;
	CRect m_rcMargins;
	DWORD m_dwTickCount;
	int m_nProcWhite;
	int m_nSkipFr; // Skip these frames on every step during setup
	int m_nRom,m_nTestMode,m_nDC300QuickView; // for ARC6000c
	CArray<IInputPreviewSite *,IInputPreviewSite *&> m_arrSites;

	CCxLibWork();
	~CCxLibWork();
	bool Init(bool bReinit = false); // Initializes buffer and camera.
	bool StartGrab(); // Start worker thread
	void StopGrab(); // Stop worker thread
	void Deinit() {};
	bool IsInited() {return m_nInitCount>0;}
	bool IsInitedOk() {return m_nInitCount>0&&m_nInitRes==1;}
	bool IsStarted() {return m_nStartCount>0;}
	bool IsImageCaptured() {return m_bImageCaptured;}
	void SetImageCaptured(bool b = true) {m_bImageCaptured = b;}
	bool WaitForFrame(int nCount = 1, DWORD dwTimeout = INFINITE);
	void StartDisplay(BOOL bStartVideo, int nScaler, HWND hwnd, CRect rcVideo);
	int  InitCameraSettings(bool bReinitCamera = true);
	int  InitCamera(int nCamera);
	void DisableBinning(bool bDisable = true);
	void InitBitnessAndConversion(int nBitsMode, int nConversion, BOOL bForCapture, int nConvStart,
		 int nConvEnd, double dGamma, int nConvStart12, int nConvEnd12, double dGamma12);
	void InitGray(int nGrayMode);
	void NotifyChangeSizes();
	BOOL IsForCapture() {return m_bForCapture;}
	void SetForCapture(BOOL bForCapture);
	BOOL IsCameraColor() {return m_CameraType.bColor;}
	void SetWBalance(BOOL bEnable, double r, double g, double b);
	void DoWBalance(double &r, double &g, double &b);
	int  GetShutterTime();
	void ReinitSizes();
	bool IsDC300() {return ::IsDC300(m_nCamera);}
	void InitDC300(bool bForCapture = false);

	double &Gamma() {return m_nBitsMode <= W10BIT ? m_dGamma : m_dGamma12;}
	int    &ConvStart() {return m_nBitsMode <= W10BIT ? m_nConvStart : m_nConvStart12;}
	int    &ConvEnd() {return m_nBitsMode <= W10BIT ? m_nConvEnd : m_nConvEnd12;}

	// corrections of various cameraglucks
	BOOL m_bBinningGluck;
	BOOL m_bARC6000Gluck1;
	BOOL m_bARC6000Gluck2;
};

extern CCxLibWork g_CxLibWork;
extern BOOL s_bSetup;
extern BOOL s_bSetupComplete;
extern HWND s_hwndProgress;

struct CameraIds
{
	int nBaumerId;
	int nInternalId;
	const char *pszCameraName;
	static int       GetCameraCount();
	static CameraIds *CameraDescrByBaumerId(int nId);
	static CameraIds *CameraDescrByInternalId(int nId);
	static CameraIds *CameraDescrByString(LPCTSTR lpName);
};

class CBaumerProfile : public CVT5ProfileManager
{
public:
	CBaumerProfile();
	virtual int GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef);
	virtual void ValidateInt(LPCTSTR lpSection, LPCTSTR lpEntry, int &nValidate);
	CameraIds *ReadCamera();
};


extern CBaumerProfile g_BaumerProfile;
extern CRITICAL_SECTION g_CritSectionCamera;
extern CRITICAL_SECTION g_CritSectionSite;

#define DEF_CAMERA      CX05C
#define DEF_BRIGHTNESS  128
#define DEF_CONTRAST    0
#define DEF_SATURATION  64

#define MAX_MARGIN      200

#define nLowSpeedMul    2

bool GainEnabled(int nCamera);
bool Is12BitCamera(int nCamera);
bool Arc6000Gluck2();
#define DC300_FULLFRAME	    ROMFM
#define DC300_PREVIEW		ROMDM
bool IsWin9x();

#endif