// DriverBaumer.h: interface for the CDriverBaumer class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_DRIVERBAUMER_H__EF71423A_6AF8_4552_B888_442156AF28C5__INCLUDED_)
#define AFX_DRIVERBAUMER_H__EF71423A_6AF8_4552_B888_442156AF28C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Driver.h"
#include "com_unknown.h"
#include "misc_templ.h"

class CDriverBaumer : public ComObjectBase, public CDriver, public IInputPreview, public _dyncreate_t<CDriverBaumer>
{
	route_unknown();
	bool m_bDataInBuffer;
	int m_nScaler;
	CDriverBaumer();
	virtual ~CDriverBaumer();
public:
// Overrides
	virtual LPBITMAPINFOHEADER OnInputImage(BOOL bUseSettings);

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
	// from IInputPreviewSite
	com_call BeginPreview(IInputPreviewSite *pSite); //
	com_call EndPreview(IInputPreviewSite *pSite); //
	com_call GetPreviewFreq(DWORD *pdwFreq); //
	com_call GetSize(short *pdx, short *pdy); //
	com_call DrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst); //
};

#endif // !defined(AFX_DRIVERBAUMER_H__EF71423A_6AF8_4552_B888_442156AF28C5__INCLUDED_)
