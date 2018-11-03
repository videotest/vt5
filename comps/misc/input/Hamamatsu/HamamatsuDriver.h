#pragma once


#include <../common2/com_unknown.h>
#include <../common2/misc_templ.h>
#include "DrvPreview.h"
#include "Timer.h"


class CPreviewPixDlg;
struct CHamamatsuData;
class CHamamatsuDriver : public CDriverPreview, public CTimer, public _dyncreate_t<CHamamatsuDriver>
{
	long m_nDevices;
	CHamamatsuData *m_pData;
	long m_lCurFrame,m_lFramesCount;
	void LutReset();
	bool DoStartGrab(bool bCapture, int nFrames, bool bSetTimer);
public:
	CHamamatsuDriver();
	~CHamamatsuDriver();

	// for CTimer
	virtual void OnTimer();


	// for CDriverPreview
	DRIVER_PREVIEW_METHODS()
	virtual LPCTSTR OnGetShortName() {return _T("Hamamatsu");}
	virtual int OnGetLongNameId() {return IDS_HAMAMATSU;}
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

	com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
};

