#pragma once


#include "com_unknown.h"
#include "misc_templ.h"
#include "DrvPreview.h"
#include "CamThread.h"

#if defined c_plusplus
extern "C" 
{
#endif
#include "master.h"
#include "pvcam.h"
#if defined c_plusplus
}
#endif


class CPVCamPixDlg;
class CPVCamDriver : public CDriverPreview, public CCamThread2, public _dyncreate_t<CPVCamDriver>
{
//	bool m_bReconnect,m_bRemoved;
//	CProgResParams *m_pParams;
//	friend void __stdcall InitCB(unsigned long status, unsigned long UserValue, unsigned __int64 Guid);
//	LPBITMAPINFOHEADER m_lpbiCap;
//	void CorrectBrightness(int cx, int cy, int mode, unsigned short* src[3]);
//	int GetCameraBPP();
	short m_hCam;
	unsigned short m_cxCam,m_cyCam;
	bool m_bUseCircBuf;
	_ptr_t2<BYTE> m_buff;

	short m_nGainIndMax;
	short m_nBitDepth;

	bool TestExposition(int nExposure, HWND hwndDlg);
	void ProcessAutoexposure(HWND hwndDlg);

protected:
	virtual int GetPreviewZoom();
public:
//	unsigned __int64 m_CamGuid;

	CPVCamDriver(void);
	~CPVCamDriver(void);

	virtual IUnknown *DoGetInterface( const IID &iid );
	route_unknown();
//	unsigned long MillisecondsToExposureUnits(unsigned long m);

	// for meStartFastAcquisition
	virtual bool InitCamera();
	virtual void DeinitCamera();
	virtual bool CanCaptureDuringPreview();
	virtual bool CanCaptureFromSettings();
//	void OnImageReadyPrv(unsigned short* src[3]);
//	void OnImageErrorPrv();
//	void OnImageReadyCap(unsigned short* src[3]);
//	void OnImageErrorCap();

	// for CCamThread
	virtual void ThreadRoutine();

	// for CDriverPreview
	DRIVER_PREVIEW_METHODS()
	virtual LPCTSTR OnGetShortName() {return _T("PVCam");}
	virtual int OnGetLongNameId() {return IDS_PVCAM;}
	virtual bool OnStartGrab();
	virtual void OnStopGrab();
	virtual bool GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi);
	virtual void ProcessCapture();
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
		BOOL bFirst, BOOL bForInput);
//	virtual void OnSetValueDouble(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dVal);
	virtual void OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal);
//	virtual void OnResetValues();

	// for IDriverDialogSite
//	com_call OnInitDlgSize(IDriverDialog *pDlg);
	com_call OnCmdMsg(int nCode, UINT nID, long lHWND);
	com_call OnUpdateCmdUI(HWND hwnd, int nId);

	// for IDriverx
	com_call GetFlags(DWORD *pdwFlags);
//	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
	com_call SetValueInt(int, BSTR, BSTR, int, UINT);

	void Reinit();
};
