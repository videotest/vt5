#pragma once 

#include "Driver.h"
#include "DlgSiteImpl.h"
#include "ImageUtil.h"
#include "ExposureMask.h"
#include "CamValues.h"
#include <../common2/com_unknown.h>
#include "CritSecEx.h"

extern CCamBoolValue g_VertMirror;
extern CCamBoolValue g_HorzMirror;
extern CCamBoolValue g_Gray;
extern CCamIntComboValue g_GrayPlane;
extern CCamIntValue g_Brightness;
extern CCamIntValue g_Contrast;
extern CCamIntValue g_Gamma;
extern CCamStringValue g_VideoDest;

class CRectCursors
{
public:
	HCURSOR m_hTop,m_hBottom,m_hLeft,m_hRight;
	HCURSOR m_hTopLeft,m_hTopRight,m_hBottomLeft,m_hBottomRight,m_hMove;

	CRectCursors();
	~CRectCursors();
	void LoadCursors(int t, int b, int l, int r, int tl, int tr, int bl, int br, int m);
	void LoadStdCursors();
	HCURSOR CursorByRectZone(CRectZone rz);
};

struct CStringsIds
{
	int idsVideoCaptured,idsFileNotOpened;
};

extern CStringsIds g_StringsIds;

// Device has hardware mirror and itself process flags g_HorzMirror and g_VertMirror. If no
// wrapper class will mirror image itself.
#define EFG_VMIRROR                        0x80000000
#define EFG_HMIRROR                        0x40000000
#define EFG_PRVVIDEO                       0x20000000

// Flags for CDriverPreview::OnExecuteSettings2

#define ES2_VIDEO    1

struct CCaptureVideoToMemory
{
	DWORD dwFrameSizeSrc;
	BITMAPINFOHEADER  biPrv;
	BITMAPINFOHEADER biDst;
};

class CDriverPreview : public ComObjectBase, public CDriver, public CPreview, public CDriverDialogSite, public IDIBProvider
{
	// For invalidation of states
	CArray<IDriverSite *,IDriverSite *&> m_arrSites;
//	CRITICAL_SECTION m_csSites;
	CArray<IDriverDialog *,IDriverDialog *&> m_arrDialogs;
	bool m_bTrack;
	CRectZone m_TrackRectZone;
	CCamValueRect *m_pTrackRect;
	CPoint m_ptTrackOrig;
	CRect m_rcTrackOrig;
	bool m_bZooming,m_bRectsMapped;
	CConv16To8 m_Conv16To8;
	CConv16To16 m_Conv16To16;
	int m_nStopGrabCount;
	bool m_bGrabOk;
protected:
	CCriticalSectionEx m_csCap; // For synchronization of access to common variables in this
	                            // class from another threads
	// For initialization of camera
	enum CCamState
	{
		NotInited = -1,
		Failed = 0,
		Succeeded = 1,
	};
	CCamState m_CamState;
	bool DoInitCamera();
	int m_nGrabCount;
	// For draw
	CDib m_Dib;
	CDib m_DibCap;
	BITMAPINFOHEADER m_biCapFmt;
	IImage3 *m_pimgCap;
	LPBITMAPINFOHEADER m_lpbiCap;

	// These shifts used in PlanesReady16
	int m_nShift16To16; // Used to obtain native 16-bit image from driver 16-bit (usually low 12 bit valid)
	int m_nShift16To8;  // Used to obtain 8-bit DIB from driver 16-bit planes

	class XStopPreview
	{
		

		CDriverPreview *m_pDrv;
		bool m_bStopPreview;
		bool m_bReinitPreview;
	public:
		bool m_bSizeChanged;
		XStopPreview(CDriverPreview *pDrv, bool bStopPreview, bool bReinitPreview)
		{
			m_pDrv = pDrv;
			m_bStopPreview = bStopPreview;
			m_bReinitPreview = bReinitPreview;
			if (m_bStopPreview)
			{
				if (m_pDrv->m_nStopGrabCount == 0)
					m_pDrv->OnStopGrab();
				m_pDrv->m_nStopGrabCount++;
			}
			m_bSizeChanged = false;
		}
		~XStopPreview()
		{
			if (m_bStopPreview)
			{
				m_pDrv->m_nStopGrabCount--;
				if (m_pDrv->m_nStopGrabCount == 0)
				{
					m_pDrv->PreinitGrab2(CaptureMode_Preview, 0);
					if (m_bReinitPreview)
					{
						BITMAPINFO256 bi;
						m_pDrv->GetFormat(false, &bi.bmiHeader);
						m_pDrv->m_Conv16To8.Init(bi.bmiHeader.biBitCount, (int)g_Brightness, (int)g_Contrast,
							double((int)g_Gamma)/100.);
						m_pDrv->m_Dib.InitBitmap(bi.bmiHeader.biWidth, bi.bmiHeader.biHeight, (bool)g_Gray?8:24);
						//sergey 05.04.06
						if (m_bSizeChanged)
						//end
							m_pDrv->NotifyChangeSizes();
					}
					m_pDrv->OnStartGrab();
				}
			}
		}
	};
	friend class CMaintainVideoMode;
	friend class CMaintainVideo;
	friend class CDriverSynch;
	

	int m_nIdNewMeth,m_nIdDeleteMeth;
	int m_nIdStaticFocusMax,m_nIdStaticFocusCur;
	CRectCursors m_Cursors;

	CExposureMask m_ExposureMask;
	void OnPrvFrameReady();
	void ResetAllControls();
	void UpdateCmdUI(); // calls IDriverDialog::UpdateCmdUI for all dialogs
	void DoSelectMethodic(int nMethodic);

	bool m_bSizeChanged; // If derived class sets this variable inside OnSetValue..
	                     // IDriverSite::OnChangeSize will be called during initialization of preview
	bool m_bSettingsChanged; // Some settings changed since last call of PreinitGrab
	void NotifyChangeSizes();

	virtual int GetPreviewZoom() {return 1;}
	void InitVideo(int idEditRate, int idEditTime, int idStaticPath, int idBrowse, int idsAviFilter);

	bool m_bWhiteBalance;
	// for waiting nexe frame
	int  m_nFrameCount;
	bool WaitNextFrameUI(DWORD dwTimeoutMs, int nFrame = 1);
	bool WaitNextFrame(DWORD dwSleep, DWORD dwTimeoutMs, int nFrame = 1);

public:
	CDriverPreview();
	~CDriverPreview();

	IUnknown *DoGetInterface( const IID &iid )
	{
		IUnknown *p;
		if (p = CDriver::DoGetInterface(iid))
			return p;
		else if (p = CPreview::DoGetInterface(iid))
			return p;
		else if (p = CDriverDialogSite::DoGetInterface(iid))
			return p;
		return ComObjectBase::DoGetInterface(iid);
	}
	route_unknown();
	virtual void BeforeFinalRelease();

	void PlanesReady16(LPWORD lpRed, LPWORD lpGreen, LPWORD lpBlue);
	void GrayReady16(LPWORD lpGray);
	//sergey 06/04/06
	void GrayReady16_Mutech(LPBYTE lpGray);
	//end


	virtual DWORD GetExFlags() {return 0;}
	virtual LPCTSTR OnGetShortName() = 0;
	virtual int OnGetLongNameId() = 0;
	virtual int OnGetInputId() = 0;
	virtual bool InitCamera() {return false;}
	virtual void DeinitCamera() {}
	virtual void ReadSettings();
	 // Called only when settings dialog is needed
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
		BOOL bFirst, BOOL bForInput);
	virtual bool OnExecuteSettings2(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
		BOOL bFirst, BOOL bForInput, DWORD dwFlags) {return false;}
	// OnStartGrab and OnStopGrab will called when camera inited ok and capture not started
	// or really needed to stop (grab counter maintained here).
	virtual bool OnStartGrab() {return false;}
	virtual void OnStopGrab() {}
	// Capure sequence:
	// 1. Call CanCaptureDuringPreview. Stop preview if needed.
	// 2. Call PreinitGrab.
	// 3. Get capture format from driver and allocate data.
	// 4. Call ProcessCapture to process hardware capture.
	// 5. Inside ProcessCapture driver must call PlanesReady or other function to save data.
	// 6. Restart preview if need.
	virtual bool CanCaptureDuringPreview() {return false;}
	virtual bool CanCaptureFromSettings() {return false;}
	virtual bool PreinitGrab(bool bCapture) {return true;}
	enum CCaptureMode
	{
		CaptureMode_Preview  = 0,
		CaptureMode_Snapshot = 1,
		CaptureMode_Video    = 2,
	};
	virtual bool PreinitGrab2(CCaptureMode cm, DWORD dwParams) {return PreinitGrab(cm==CaptureMode_Snapshot);}
	virtual void PostDeinit2() {};
	// Driver can fill only biBitCount, biWidth an biHeight
	virtual bool GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi) {return false;}
	virtual bool GetFormat2(CCaptureMode cm, LPBITMAPINFOHEADER lpbi) {return GetFormat(cm==CaptureMode_Snapshot, lpbi);}
	virtual void ProcessCapture() {};
	// Called only when camera was successfully initialized
	virtual void OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst) {};
	virtual void OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal) {}
	virtual void OnSetValueDouble(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dVal) {}
	virtual void OnResetValues() {} // called when all values changes (e.g on change of the methodic)

	virtual LPBITMAPINFOHEADER CaptureDIB(IDIBProvider *pDIBPrv, bool bNative) {return false;}

	// for capturing video into memory
	_ptr_t2<byte> m_BuffVideo;
	_ptr_t2<int> m_FrmOk;
	int m_nCurFrame;
	bool m_bMakeVideo,m_bVideoMode;
	DWORD m_dwStartVideo;
	DWORD m_dwFrameMs;
	CCaptureVideoToMemory m_CaptureVideoToMemory;
	void MakeVideoFrame(LPBYTE lpDataSrc, DWORD dwSize);
	virtual void MakeVideo(int nFrames, DWORD dwFrameMs, IVideoHook *pHook);
	virtual void DoCaptureVideoToMemory(int nFrames, DWORD dwFrameMs, IVideoHook *pHook);
	virtual bool CaptureVideoToMemory(int nDevice, int nMode, IVideoHook *pVideoHook);
	virtual void OnChangeVideoMode() {}
	virtual bool GetVideoFormats(CCaptureVideoToMemory *pCap);
	virtual void ConvertVideoFrame(LPBYTE lpDataSrc, CDib &Dib) {}


	// IDIBProvider
	virtual void *__stdcall AllocMem(DWORD dwSize) {return new BYTE[dwSize];};
	// IDriver
	com_call ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
	com_call InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings);
	com_call InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings);
	com_call GetShortName(BSTR *pbstrShortName);
	com_call GetLongName(BSTR *pbstrShortName);
	com_call GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue);
	com_call SetValue(int nCurDev, BSTR bstrName, VARIANT Value);
	com_call GetDevicesCount(int *pnCount);
	com_call GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory);
	com_call InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode);
	// IDriver2
	com_call SetValueInt(int, BSTR, BSTR, int, UINT);
	com_call GetValueInt(int, BSTR, BSTR, int*, UINT);
	com_call GetValueDouble(int, BSTR, BSTR, double *, UINT);
	com_call SetValueDouble(int, BSTR, BSTR, double, UINT);
	// IDriver3
	com_call StartImageAquisition(int, BOOL bStart);
	com_call IsImageAquisitionStarted(int, BOOL *pbAquisition);
	com_call AddDriverSite(int, IDriverSite *pSite);
	com_call RemoveDriverSite(int, IDriverSite *pSite);
	com_call GetImage(int, LPVOID *, DWORD *);
	// IDriver4
	com_call SetConfiguration(BSTR bstrName);
	// IDriver5
	com_call InputVideoFile2(int nDevice, int nMode, IVideoHook *pVideoHook);
	com_call GetValueInfoByName(int nCurDev, const char *pszSection, const char *pszEntry, VALUEINFOEX *pValueInfo);
	// IDriverPreview2
	com_call BeginPreview(int nDevice, IInputPreviewSite *pSite);
	com_call EndPreview(int nDevice, IInputPreviewSite *pSite);
	com_call GetPreviewFreq(int nDevice, DWORD *pdwFreq);
	com_call GetSize(int nDevice, short *pdx, short *pdy);
	com_call DrawRect(int nDevice, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	com_call GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod);
	// IDriverPreview3
	com_call MouseMove(UINT nFlags, POINT pt);
	com_call LButtonDown(UINT nFlags, POINT pt);
	com_call LButtonUp(UINT nFlags, POINT pt);
	com_call RButtonDown(UINT nFlags, POINT pt);
	com_call RButtonUp(UINT nFlags, POINT pt);
	com_call SetCursor(UINT nFlags, POINT pt);
	// IDriverDialogSite
	com_call OnInitDialog(IDriverDialog *pDlg);
	com_call GetControlInfo(int nID, VALUEINFOEX2 *pValueInfoEx2);
	com_call OnCloseDialog(IDriverDialog *pDlg);
	com_call OnCmdMsg(int nCode, UINT nID, long lHWND);
	com_call OnUpdateCmdUI(HWND hwnd, int nId);
	com_call OnDelayedRepaint(HWND hwndDlg, IDriverDialog *pDlg);
};

#define DRIVER_PREVIEW_METHODS()   virtual int OnGetInputId() {return IDS_INPUT;}

bool MakeDummyDIB(IDIBProvider *pDIBPrv, int cx, int cy, int bpp);
extern double s_dZoom;



// For synchronization of access to common variables in CDriverPreview
// class from another threads
class CDriverSynch
{
	CDriverPreview *m_p;
public:
	CDriverSynch(CDriverPreview *pDrv)
	{
		m_p = pDrv;
		pDrv->m_csCap.Enter();
	}
	~CDriverSynch()
	{
		m_p->m_csCap.Leave();
	}
};

