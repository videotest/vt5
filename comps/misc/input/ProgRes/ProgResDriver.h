#pragma once

#include "com_unknown.h"
#include "misc_templ.h"
#include "DrvPreview.h"


class CPreviewPixDlg;
struct CProgResParams;
class CProgResDriver : public CDriverPreview, public _dyncreate_t<CProgResDriver>
{
	bool m_bReconnect,m_bRemoved;
	CProgResParams *m_pParams;
	friend void __stdcall InitCB(unsigned long status, unsigned long UserValue, unsigned __int64 Guid);
	LPBITMAPINFOHEADER m_lpbiCap;
	void CorrectBrightness(int cx, int cy, int mode, unsigned short* src[3]);
	int GetCameraBPP();

protected:
	virtual int GetPreviewZoom() {return 2;}
public:
	unsigned __int64 m_CamGuid;

	CProgResDriver(void);
	~CProgResDriver(void);

	virtual IUnknown *DoGetInterface( const IID &iid );
	route_unknown();
	unsigned long MillisecondsToExposureUnits(unsigned long m);

	// for meStartFastAcquisition
	virtual bool InitCamera();
	virtual void DeinitCamera();
	void OnImageReadyPrv(unsigned short* src[3]);
	void OnImageErrorPrv();
	void OnImageReadyCap(unsigned short* src[3]);
	void OnImageErrorCap();

	// for CDriverPreview
	DRIVER_PREVIEW_METHODS()
	virtual LPCTSTR OnGetShortName() {return _T("ProgRes");}
	virtual int OnGetLongNameId() {return IDS_PROGRES;}
	virtual bool OnStartGrab();
	virtual void OnStopGrab();
	virtual bool GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi);
	virtual void ProcessCapture();
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
		BOOL bFirst, BOOL bForInput);
	virtual void OnSetValueDouble(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dVal);
	virtual void OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal);
	virtual void OnResetValues();

	// for IDriverDialogSite
	com_call OnInitDlgSize(IDriverDialog *pDlg);
	com_call OnCmdMsg(int nCode, UINT nID, long lHWND);
	com_call OnUpdateCmdUI(HWND hwnd, int nId);

	// for IDriverx
	com_call GetFlags(DWORD *pdwFlags); //
	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
	com_call SetValueInt(int, BSTR, BSTR, int, UINT);
};
