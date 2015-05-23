#if !defined(AFX_DRIVERMV500_H__43437A22_09A0_11D4_A98A_FA6524668D51__INCLUDED_)
#define AFX_DRIVERMV500_H__43437A22_09A0_11D4_A98A_FA6524668D51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriverMV500.h : header file
//

#include "Driver.h"
#include "com_unknown.h"
#include "misc_templ.h"
//#include "DibView.h"
#include "SourceDlg.h"

/*struct CAMERA
{
	CDibView *m_lpDibview;
	int m_nChannel;
};*/

const int nCameras = 2;

class CDriverMV500 : public ComObjectBase, public CDriver, public CPreview, public _dyncreate_t<CDriverMV500>,
	public ISourceSite, public IMV500GrabSite
{
	route_unknown();
	CDriverMV500();
	virtual ~CDriverMV500();

	enum Stage
	{
		NotOpened,
		Opened,
		OpenFailed,
		Inited,
	};
	Stage m_Stage;
	bool Open();
	void Close();
	void Init();
	void SelectChannel(int iChannel);
	CTypedPtrArray<CPtrArray,IDriverSite *> m_arrSites;
	int FindSite(IDriverSite *pSite);
	int m_nPreviewMode;
	int m_nTimer;

	bool m_bUseDrawBuffer;
	LPBYTE m_pBufDraw;
	DWORD m_dwDrawBufferSize;

protected:
	static void CALLBACK TimeCallback(UINT uTimerID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);
    // capture info
	int      m_nInit;
	CSize    m_CaptureSize;
	MV5Frame m_Frame;
	int      m_nGrabCount;
//	CAMERA   m_cameras[nCameras];
	int      m_nCamera;
	int      m_nEvent;
	// Input without initialization and waiting
	void DoInputNative(IUnknown *pUnkImage, int nDevice, BOOL bFromSettings);
	bool IsFieldsMode() {return CMV500Grab::s_pGrab->GetCamerasNum()>1&&m_nPreviewMode==Preview_Fast;}

public:
// Overrides
	virtual IUnknown *DoGetInterface( const IID &iid );
	// from IDriver
	com_call InputNative(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bFromSettings); //
	com_call InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings); //
	com_call InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode);
	com_call ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput); //
	com_call GetFlags(DWORD *pdwFlags); //
	com_call GetShortName(BSTR *pbstrShortName); //
	com_call GetLongName(BSTR *pbstrShortName); //
	com_call GetValue(int nCurDev, BSTR bstrName, VARIANT *pValue); //
	com_call SetValue(int nCurDev, BSTR bstrName, VARIANT Value); //
	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
	com_call GetDevicesCount(int *pnCount);
	com_call GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory);
	// from IDriver3
	com_call GetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize);
	com_call ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk);
	com_call GetTriggerMode(int nCurDev, BOOL *pgTriggerMode);
	com_call SetTriggerMode(int nCurDev, BOOL bSet);
//	com_call DoSoftwareTrigger(int nCurDev);
	com_call StartImageAquisition(int nCurDev, BOOL bStart);
	com_call IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition);
	com_call AddDriverSite(int nCurDev, IDriverSite *pSite);
	com_call RemoveDriverSite(int nCurDev, IDriverSite *pSite);
	com_call GetTriggerNotifyMode(int nCurDev, DWORD *plTriggerNotifyMode);
	com_call GetImageSequenceNumber(int nCurDev, long *plSeqNum);
	com_call ResetTrigger(int nCurDev);
	// from IInputPreviewSite2
	com_call BeginPreview(int nDev, IInputPreviewSite *pSite);
	com_call EndPreview(int nDev, IInputPreviewSite *pSite);
	com_call GetPreviewFreq(int nDev, DWORD *pdwFreq);
	com_call GetSize(int nDev, short *pdx, short *pdy);
	com_call DrawRect(int nDev, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	com_call GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod);
	com_call GetPreviewMode(int nDevice, int *pnMode);
	com_call SetPreviewMode(int nDevice, int nMode);

public:
	// ISourceSite
	virtual void Reinit(int nMode);
	// IMV500GrabSite
	virtual void OnTrigger();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVERMV500_H__43437A22_09A0_11D4_A98A_FA6524668D51__INCLUDED_)
