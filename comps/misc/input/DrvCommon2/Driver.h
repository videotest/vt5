#if !defined(__Driver_H__)
#define __Driver_H__

#include "input.h"
#include "StdProfile.h"
#include "CamDefaults.h"

#define E_NOTIMPLEMENTED E_NOTIMPL

class CDriver : public IDriver6, public CCamValues
{
public:
	IUnknown *DoGetInterface( const IID &iid )
	{
		if (iid == IID_IDriver)
			return (IDriver *)this;
		else if (iid == IID_IDriver2)
			return (IDriver2 *)this;
		else if (iid == IID_IDriver3)
			return (IDriver3 *)this;
		else if (iid == IID_IDriver4)
			return (IDriver4 *)this;
		else if (iid == IID_IDriver5)
			return (IDriver5 *)this;
		else if (iid == IID_IDriver6)
			return (IDriver6 *)this;
		else
			return NULL;
	}

	// Methods from IDriver
	com_call InputNative(IUnknown *, IUnknown *, int, BOOL) {return E_NOTIMPLEMENTED;}
	com_call InputDIB(IDIBProvider *, IUnknown *, int, BOOL) {return E_NOTIMPLEMENTED;}
	com_call InputVideoFile(BSTR *, int, int) {return E_NOTIMPLEMENTED;}
	com_call ExecuteSettings(HWND, IUnknown *, int, int, BOOL, BOOL) {return E_NOTIMPLEMENTED;}
	com_call GetFlags(DWORD *pdwFlags) {*pdwFlags = 0; return S_OK;}
	com_call GetShortName(BSTR *pbstrShortName) {return E_NOTIMPLEMENTED;}
	com_call GetLongName(BSTR *pbstrShortName) {return E_NOTIMPLEMENTED;}
	com_call GetValue(int, BSTR, VARIANT *) {return E_NOTIMPLEMENTED;}
	com_call SetValue(int, BSTR, VARIANT) {return E_NOTIMPLEMENTED;}
	com_call ExecuteDriverDialog(int, LPCTSTR) {return E_NOTIMPLEMENTED;}
	com_call GetDevicesCount(int *pnCount) {*pnCount = 1; return S_OK;}
	com_call GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory) {return E_NOTIMPLEMENTED;}
	// Methods from IDriver2
	com_call UpdateSizes() {return E_NOTIMPLEMENTED;}
	com_call BeginPreviewMode() {return E_NOTIMPLEMENTED;}
	com_call EndPreviewMode() {return E_NOTIMPLEMENTED;}
	com_call CancelChanges() {return E_NOTIMPLEMENTED;}
	com_call ApplyChanges() {return E_NOTIMPLEMENTED;}
	com_call GetValueInt(int, BSTR, BSTR, int*, UINT) {return E_NOTIMPLEMENTED;}
	com_call SetValueInt(int, BSTR, BSTR, int, UINT) {return E_NOTIMPLEMENTED;}
	com_call GetValueString(int, BSTR, BSTR, BSTR *, UINT) {return E_NOTIMPLEMENTED;}
	com_call SetValueString(int, BSTR, BSTR, BSTR, UINT) {return E_NOTIMPLEMENTED;}
	com_call GetValueDouble(int, BSTR, BSTR, double *, UINT) {return E_NOTIMPLEMENTED;}
	com_call SetValueDouble(int, BSTR, BSTR, double, UINT) {return E_NOTIMPLEMENTED;}
	com_call ReinitPreview() {return E_NOTIMPLEMENTED;}
	// Methods from IDriver3
	com_call GetImage(int, LPVOID *, DWORD *) {return E_NOTIMPLEMENTED;}
	com_call ConvertToNative(int, LPVOID, DWORD, IUnknown *) {return E_NOTIMPLEMENTED;}
	com_call GetTriggerMode(int, BOOL *pgTriggerMode) {return E_NOTIMPLEMENTED;}
	com_call SetTriggerMode(int, BOOL bSet) {return E_NOTIMPLEMENTED;}
	com_call DoSoftwareTrigger(int) {return E_NOTIMPLEMENTED;}
	com_call StartImageAquisition(int, BOOL bStart) {return S_OK;}
	com_call IsImageAquisitionStarted(int, BOOL *pbAquisition) {*pbAquisition = FALSE; return S_OK;}
	com_call AddDriverSite(int, IDriverSite *pSite) {return S_OK;}
	com_call RemoveDriverSite(int, IDriverSite *pSite) {return S_OK;}
	// Methods from IDriver4
	com_call SetConfiguration(BSTR bstrName)
	{
		if (CStdProfileManager::m_pMgr)
		{
			CString s(bstrName);
			CStdProfileManager::m_pMgr->InitMethodic(s);
			return S_OK;
		}
		return S_FALSE;
	}
	com_call GetConfiguration(BSTR *pbstrName)
	{
		if (CStdProfileManager::m_pMgr)
		{
			CString s(CStdProfileManager::m_pMgr->GetMethodic());
			*pbstrName = s.AllocSysString();
			return S_OK;
		}
		return E_UNEXPECTED;
	}
	// Methods from IDriver5
	com_call GetFocusMode(BOOL *pbFocusMode) {return E_NOTIMPLEMENTED;}
	com_call SetFocusMode(BOOL bFocusMode) {return E_NOTIMPLEMENTED;}
	com_call GetFocusIndicator(int *pnFocusIndicator) {return E_NOTIMPLEMENTED;}
	com_call GetValuesCount(int nCurDev, int *pnValues) {return CCamValues::GetValuesCount(pnValues);}
	com_call GetValueInfo(int nCurDev, int nValue, VALUEINFOEX *pValueInfo) {return CCamValues::GetValueInfo(nValue,pValueInfo);}
	com_call GetValueInfoByName(int nCurDev, const char *pszSection, const char *pszEntry, VALUEINFOEX *pValueInfo)
		{return CCamValues::GetValueInfoByName(pszSection,pszEntry,pValueInfo);}
	com_call InputVideoFile2(int nDevice, int nMode, IVideoHook *pVideoHook) {return E_NOTIMPLEMENTED;}
	// Methods from IDriver6
	com_call GetTriggerNotifyMode(int nCurDev, DWORD *plTriggerNotifyMode) {return E_NOTIMPL;}
	com_call GetImageSequenceNumber(int nCurDev, long *plSeqNum) {return E_NOTIMPL;}
	com_call ResetTrigger(int nCurDev) {return E_NOTIMPL;}
	com_call GetImage2(int nCurDev, LPBITMAPINFOHEADER *ppbi, LPVOID *ppData, DWORD *pdwSize)
		{return E_NOTIMPL;}
};


class CPreview : public IInputPreview3
{
public:
	IUnknown *DoGetInterface( const IID &iid )
	{
		if (iid == IID_IInputPreview2)
			return (IInputPreview2 *)this;
		else if (iid == IID_IInputPreview3)
			return (IInputPreview3 *)this;
		else if (iid == IID_IInputPreview)
			return (IInputPreview *)this;
		else
			return NULL;
	}
	// From IInputPreview
	com_call BeginPreview(IInputPreviewSite *pSite) {return BeginPreview(0, pSite);}
	com_call EndPreview(IInputPreviewSite *pSite) {return EndPreview(0, pSite);}
	com_call GetPreviewFreq(DWORD *pdwFreq) {return GetPreviewFreq(0, pdwFreq);}
	com_call GetSize(short *pdx, short *pdy) {return GetSize(0, pdx, pdy);}
	com_call DrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst) {return DrawRect(0, hDC, lpRectSrc, lpRectDst);}
	// From IInputPreview2
	com_call BeginPreview(int nDevice, IInputPreviewSite *pSite) {return E_NOTIMPLEMENTED;};
	com_call EndPreview(int nDevice, IInputPreviewSite *pSite) {return E_NOTIMPLEMENTED;};
	com_call GetPreviewFreq(int nDevice, DWORD *pdwFreq) {return E_NOTIMPLEMENTED;};
	com_call GetSize(int nDevice, short *pdx, short *pdy) {return E_NOTIMPLEMENTED;};
	com_call DrawRect(int nDevice, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst) {return E_NOTIMPLEMENTED;};
	com_call GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod) {return E_NOTIMPLEMENTED;};
	com_call GetPreviewMode(int nDevice, int *pnMode) {return E_NOTIMPLEMENTED;};
	com_call SetPreviewMode(int nDevice, int nMode) {return E_NOTIMPLEMENTED;};
	// From IInputPreview3
	com_call MouseMove(UINT nFlags, POINT pt) {return E_NOTIMPLEMENTED;};
	com_call LButtonDown(UINT nFlags, POINT pt) {return E_NOTIMPLEMENTED;};
	com_call LButtonUp(UINT nFlags, POINT pt) {return E_NOTIMPLEMENTED;};
	com_call RButtonDown(UINT nFlags, POINT pt) {return E_NOTIMPLEMENTED;};
	com_call RButtonUp(UINT nFlags, POINT pt) {return E_NOTIMPLEMENTED;};
	com_call SetCursor(UINT nFlags, POINT pt) {return E_NOTIMPLEMENTED;};
};

#endif