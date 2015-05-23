#pragma once

#include "com_unknown.h"
#include "misc_templ.h"
#include "DrvPreview.h"
#include "CamThread.h"
#include "Timer.h"


class CPreviewPixDlg;
struct CSumixParams;
class CSumixDriver : public CDriverPreview, public CCamThread2, public CTimer,
	public _dyncreate_t<CSumixDriver>
{
	CSumixParams *m_pParams;
	_ptr_t2<BYTE> m_Buff8,m_Buff24;
//	friend class _CPrepDarkField;
	bool m_bDarkField;
	bool WhiteBalanceStep(double &dRed, double &dGreen, double &dBlue);
	void MakeWhitBalance();
	bool DACFineOffsetStep(double &dDiff);
	void MakeDACFineOffset();
protected:
//	virtual int GetPreviewZoom() {return 2;}
public:
	CSumixDriver(void);
	~CSumixDriver(void);

	virtual IUnknown *DoGetInterface( const IID &iid );
	route_unknown();

	virtual bool InitCamera();
	virtual void DeinitCamera();

	// for CCamThread
	virtual void ThreadRoutine();
	// for CTimer
	virtual void OnTimer();

	// for CDriverPreview
	DRIVER_PREVIEW_METHODS()
	virtual LPCTSTR OnGetShortName() {return _T("Sumix");}
	virtual int OnGetLongNameId() {return IDS_SUMIX;}
	virtual bool OnStartGrab();
	virtual void OnStopGrab();
	virtual bool CanCaptureDuringPreview();
	virtual bool CanCaptureFromSettings();
//	virtual bool PreinitGrab(bool bCapture);
	virtual bool PreinitGrab2(CCaptureMode cm, DWORD dwParams);
	virtual bool GetFormat(bool bCapture, LPBITMAPINFOHEADER lpbi);
	virtual void ProcessCapture();
	virtual bool OnExecuteSettings2(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode,
		BOOL bFirst, BOOL bForInput, DWORD dwFlags);
	virtual void OnSetValueDouble(LPCTSTR lpstrSec, LPCTSTR lpstrEntry, double dVal);
	virtual void OnResetValues();
	virtual void OnChangeVideoMode();
	virtual bool GetVideoFormats(CCaptureVideoToMemory *pCap);
	virtual void ConvertVideoFrame(LPBYTE lpDataSrc, CDib &Dib);
	virtual DWORD GetExFlags() {return EFG_PRVVIDEO;}

	// for IDriverDialogSite
//	com_call OnInitDlgSize(IDriverDialog *pDlg);
	com_call OnCmdMsg(int nCode, UINT nID, long lHWND);
	com_call OnUpdateCmdUI(HWND hwnd, int nId);
	com_call GetControlInfo(int nID, VALUEINFOEX2 *pValueInfoEx2);

	// for IDriverx
	com_call SetValueInt(int nDev, BSTR bstrSec, BSTR bstrEntry, int nValue, UINT dwFlags);
	com_call GetFlags(DWORD *pdwFlags); //
	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
};

