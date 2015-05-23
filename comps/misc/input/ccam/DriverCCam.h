// DriverCCam.h: interface for the CDriverCCam class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(__DriverCCam_H__)
#define __DriverCCam_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Driver.h"
#include "com_unknown.h"
#include "misc_templ.h"
#include "Image5.h"
#include "CCamUtils.h"

class CDriverCCam : public ComObjectBase, public CDriver, public CPreview/*, public IInputSettings*/,
	public _dyncreate_t<CDriverCCam>
{
	friend class CCCamThread;
	friend void CALLBACK TimerProc(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);
	static CArray<IDriverSite *,IDriverSite *&> m_arrSites;
	static CRITICAL_SECTION m_csSites;
	static bool m_bCSSitesInited;
	bool m_bInitFailed;//m_bInited;
	CSize m_szActiveVideo; // (1280,1024) now
	int m_x,m_y,m_cx,m_cy,m_bpp,m_shift;
	int m_nBright,m_nContrast,m_nBrightR,m_nContrastR,m_nBrightG,m_nContrastG,m_nBrightB,m_nContrastB;
	double m_dGamma,m_dGammaR,m_dGammaG,m_dGammaB;
	bool m_bColor,m_bExact,m_bBCG;
	unsigned long m_nTransferSize;
	_ptr_t2<byte> m_BuffLow; // already grayscale m_cx*m_cy*12bit or m_bpp
	_ptr_t2<byte> m_BuffCap; // color or g/s m_cx*m_cy*m_bpp
	_ptr_t2<byte> m_BuffDraw; // color or g/s 8-bit image
	CCCamImageData m_DrawData;
	_ptr_t2<byte> m_CnvR; // Buffers for color conversion
	_ptr_t2<byte> m_CnvG;
	_ptr_t2<byte> m_CnvB;
	int m_nSaturation;
	void MakeImage();
	_ptr_t2<byte> m_BuffVideo;
	_ptr_t2<int> m_FrmOk;
	void MakeVideo(int nFrames, int nFrameSize, DWORD dwFrameMs, IVideoHook *pHook);
	void CalcWhiteBalance();
	BITMAPINFO256 m_bi;
	bool m_bNoWait;
	CColorCurve m_Curve8,m_Curve10,m_Curve12;

	int m_nSlopes;
	int m_nIntTimes[4];
	int m_nResetVolt[4];

	bool Init();
	void DoReadSlopesValues(int n1, int n2);
	void ReadSlopesValues();
	void InitSlopes();
	int m_nPrevMode;
	void InitSettings(int nMode); // 0 - preview, 1 - capture, 2 - AVI
	void InitBuffersAndBI();
	void InitDatamode();
	void InitCnvBuff();

	CPoint m_pt1; // For drag
	int m_x1,m_y1;

	// Saving current frame grabber settings into shell.data and restoring it.
//	void SaveSettings();
//	void RestoreSettings();
	HRESULT InputNativeOrDIB(IImage3 *pImg, IDIBProvider *pDIBPrv);
	int m_nGrabCount;
	route_unknown();
	CDriverCCam();
	virtual ~CDriverCCam();

	BOOL m_bHorzMirror,m_bVertMirror;
public:
// Overrides
	virtual IUnknown *DoGetInterface( const IID &iid );

	// from IDriver
	com_call InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings); //
	com_call InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings); //
	com_call InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode); //
	com_call ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput); //
	com_call GetFlags(DWORD *pdwFlags); //
	com_call GetShortName(BSTR *pbstrShortName); //
	com_call GetLongName(BSTR *pbstrShortName); //
	com_call GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue); //
	com_call SetValue(int nCurDev, BSTR bstrName, VARIANT Value); //
	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
	com_call GetDevicesCount(int *pnCount);
	com_call GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory);
	// from IDriver2
	com_call SetValueInt(int, BSTR, BSTR, int, UINT);
	com_call GetValueInt(int, BSTR, BSTR, int*, UINT);
	// from IDriver3
	com_call GetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize);
	com_call ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk);
	com_call GetTriggerMode(int nCurDev, BOOL *pgTriggerMode);
	com_call SetTriggerMode(int nCurDev, BOOL bSet);
	com_call DoSoftwareTrigger(int nCurDev);
	com_call StartImageAquisition(int nCurDev, BOOL bStart);
	com_call IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition);
	com_call AddDriverSite(int nCurDev, IDriverSite *pSite);
	com_call RemoveDriverSite(int nCurDev, IDriverSite *pSite);
	// from IDriver4
	com_call SetConfiguration(BSTR bstrName);
	// from IDriver5
	com_call InputVideoFile2(int nDevice, int nMode, IVideoHook *pVideoHook);
	// from IInputPreviewSite2
	com_call BeginPreview(int nDev, IInputPreviewSite *pSite); //
	com_call EndPreview(int nDev, IInputPreviewSite *pSite); //
	com_call GetPreviewFreq(int nDev, DWORD *pdwFreq); //
	com_call GetSize(int nDev, short *pdx, short *pdy); //
	com_call DrawRect(int nDev, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst); //
	com_call GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod);
	com_call GetPreviewMode(int nDevice, int *pnMode);
	com_call SetPreviewMode(int nDevice, int nMode);
	// from IInputPreviewSite3
	com_call MouseMove(UINT nFlags, POINT pt);
	com_call LButtonDown(UINT nFlags, POINT pt);
	com_call LButtonUp(UINT nFlags, POINT pt);
	com_call SetCursor(UINT nFlags, POINT pt);
};

#endif
