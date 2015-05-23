// DriverScionFG.h: interface for the CDriverCCam class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(__DriverScionFG_H__)
#define __DriverScionFG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Driver.h"
#include "com_unknown.h"
#include "misc_templ.h"
#include "Image5.h"

class CDriverScion : public ComObjectBase, public CDriver, public CPreview/*, public IInputSettings*/,
	public _dyncreate_t<CDriverScion>
{
	static CArray<IDriverSite *,IDriverSite *&> m_arrSites;
	static CRITICAL_SECTION m_csSites;
	static bool m_bCSSitesInited;
	bool m_bInitFailed;//m_bInited;
	int m_nGrabCount;
	route_unknown();
	CDriverScion();
	virtual ~CDriverScion();

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
	// from IInputPreviewSite2
	com_call BeginPreview(int nDev, IInputPreviewSite *pSite); //
	com_call EndPreview(int nDev, IInputPreviewSite *pSite); //
	com_call GetPreviewFreq(int nDev, DWORD *pdwFreq); //
	com_call GetSize(int nDev, short *pdx, short *pdy); //
	com_call DrawRect(int nDev, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst); //
	com_call GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod);
	com_call GetPreviewMode(int nDevice, int *pnMode);
	com_call SetPreviewMode(int nDevice, int nMode);
};

#endif
