#pragma once


#include "com_unknown.h"
#include "misc_templ.h"
#include "DrvPreview.h"
#include "CamThread.h"
#include "Timer.h"


//class CPreviewPixDlg;
struct CBaumerBxData;
class CBaumerBxDriver : public CDriverPreview, public CCamThread, public CTimer, public _dyncreate_t<CBaumerBxDriver>
{
	int m_nDevices;
	int m_nDevice;
	CBaumerBxData *m_pData;
	void GrabDib();
	void SetBinning(int nMode);
	void SetWhiteBalance();
	void AnalyzeSnapshotFormats();
	friend class _CBaumerBxSnapshot;
public:
	CBaumerBxDriver();
	~CBaumerBxDriver();

	// for CCamThread
	virtual void ThreadRoutine();
	// for CTimer
	virtual void OnTimer();


	// for CDriverPreview
	DRIVER_PREVIEW_METHODS()
	virtual LPCTSTR OnGetShortName() {return _T("BaumerBx");}
	virtual int OnGetLongNameId() {return IDS_BAUMER_BX;}
	virtual bool OnStartGrab();
	virtual void OnStopGrab();
	virtual bool PreinitGrab(bool bCapture);
	virtual bool GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi);
	virtual void ProcessCapture();
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
		BOOL bFirst, BOOL bForInput);
	virtual void OnSetValueInt(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, int nVal);

	virtual bool InitCamera();
	virtual void DeinitCamera();

	com_call OnCmdMsg(int nCode, UINT nID, long lHWND);
	com_call OnUpdateCmdUI(HWND hwnd, int nId);


	com_call GetSize(int nDevice, short *pdx, short *pdy);
	com_call InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings);
	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
	com_call SetValueInt(int nDev, BSTR bstrSec, BSTR bstrEntry, int nValue, UINT dwFlags);
	com_call SetValueDouble(int nDev, BSTR bstrSec, BSTR bstrEntry, double dValue, UINT dwFlags);
};

