// DriverPixera.h: interface for the CDriverPixera class.
//
//////////////////////////////////////////////////////////////////////


#if !defined(AFX_DRIVERPIXERA_H__EF71423A_6AF8_4552_B888_442156AF28C5__INCLUDED_)
#define AFX_DRIVERPIXERA_H__EF71423A_6AF8_4552_B888_442156AF28C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Driver.h"
#include "com_unknown.h"
#include "misc_templ.h"
#include "ColorCurve.h"
#include "PixSDK.h"
#include "Image5.h"
#include "Settings.h"

enum RectEditMode
{
	None,
	Exposure,
	ColorBalance,
	BlackBalance,
	Focus,
};
enum RectEditZone
{
	RectNone,
	RectTop,
	RectBottom,
	RectLeft,
	RectRight,
	RectTopLeft,
	RectTopRight,
	RectBottomLeft,
	RectBottomRight,
	RectMove,
};


class CDriverPixera : public ComObjectBase, public CDriver, public CPreview/*, public IInputSettings*/,
	public _dyncreate_t<CDriverPixera>
{
//	CStringArray m_saSecs,m_saVals;
//	CUIntArray m_naDevs;
	CColorCurve m_Curve;

	// Section for work with Pixera SDK
	HANDLE m_hPrv;
	int m_nDrvState; // 0 - not loaded, 1 - loaded, -1 - load failed
	bool Init();

	// For invalidating of preview after new image arrived
	friend void ImageFunc(void*,void*);
	static CArray<IDriverSite *,IDriverSite *&> m_arrSites;
	static CRITICAL_SECTION m_csSites;
	static bool m_bCSSitesInited;
	bool m_bInited;

	// Saving current frame grabber settings into shell.data and restoring it.
	void SaveSettings();
	void RestoreSettings();

	// Initializes or deinitializes preview but doesn't check and change m_nGrabCount
	bool InitMIA();
	void DeinitMIA();

	CDriverPixera::RectEditMode m_RectEditMode;
	RectEditZone m_RectEditZone;
	RectEditZone GetRectEditZone(CPoint pt, CRect rc);
	RectEditMode GetRectEditMode(CPoint pt, RectEditZone *pZone, PxRectEx &RectOrig);
	CPoint m_ptDrag1;
	PxRectEx m_RectOrig;
	bool GetRectEditZoneEx(RectEditZone &Zone, CPoint pt, PxRectEx rc, PxRectEx &RectOrig);
	int m_nPreviewMode;
	PixRectData m_AutoExpSaved,m_FocusSaved,m_WBSaved,m_BBSaved;

	void DoProcessNativeOrDIB(LPBITMAPINFOHEADER lpbi, IImage3 *pImg, IDIBProvider *pDIBPrv);
	HRESULT InputNativeOrDIB(IImage3 *pImg, IDIBProvider *pDIBPrv);
	int m_nGrabCount;
	route_unknown();
	CDriverPixera();
	virtual ~CDriverPixera();
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
	// from IDriver3
	com_call GetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize);
	com_call ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk);
	com_call StartImageAquisition(int nCurDev, BOOL bStart);
	com_call IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition);
	com_call AddDriverSite(int nCurDev, IDriverSite *pSite);
	com_call RemoveDriverSite(int nCurDev, IDriverSite *pSite);
	// from IDriver4
	com_call SetConfiguration(BSTR bstrName);
	// from IDriver5
	com_call GetFocusMode(BOOL *pbFocusMode);
	com_call SetFocusMode(BOOL bFocusMode);
	com_call GetFocusIndicator(int *pnFocusIndicator);
	// from IInputPreview2
	com_call BeginPreview(int nDev, IInputPreviewSite *pSite); //
	com_call EndPreview(int nDev, IInputPreviewSite *pSite); //
	com_call GetPreviewFreq(int nDev, DWORD *pdwFreq); //
	com_call GetSize(int nDev, short *pdx, short *pdy); //
	com_call DrawRect(int nDev, HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst); //
	com_call GetPeriod(int nDevice, int nMode, DWORD *pdwPeriod);
	com_call GetPreviewMode(int nDevice, int *pnMode);
	com_call SetPreviewMode(int nDevice, int nMode);
	// from IInputPreview3
	com_call MouseMove(UINT nFlags, POINT pt);
	com_call LButtonDown(UINT nFlags, POINT pt);
	com_call LButtonUp(UINT nFlags, POINT pt);
	com_call SetCursor(UINT nFlags, POINT pt);
#if 0
	// from IInputSettings
	com_call GetValue(int nCurDev, BSTR bstrSec, BSTR bstrEntry, BOOL bPreview, VARIANT *pValue);
	com_call SetValue(int nCurDev, BSTR bstrSec, BSTR bstrEntry, BOOL bPreview, VARIANT Value);
	com_call BeginPreviewSettings();
	com_call EndPreviewSettings();
	com_call CancelChanges();
	com_call ApplyChanges();
	com_call GetValueInfo(int nCurDev, BSTR bstrSec, BSTR bstrEntry, int *pnType, void *pInfo);
#endif
};

#endif // !defined(AFX_DRIVERPIXERA_H__EF71423A_6AF8_4552_B888_442156AF28C5__INCLUDED_)
