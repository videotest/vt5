#if !defined(AFX_DRIVERBASE_H__A7BADE84_CA80_4716_85B3_E3267A76F2FD__INCLUDED_)
#define AFX_DRIVERBASE_H__A7BADE84_CA80_4716_85B3_E3267A76F2FD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriverBase.h : header file
//


#include "Input.h"
#include "NewDocStdProfile.h"

class CInputPreviewImpl : public CImplBase
{
protected:
	CInputPreviewImpl();
	~CInputPreviewImpl();

	CArray<IInputPreviewSite *,IInputPreviewSite *&> m_arrSites;

	virtual bool OnBeginPreview(IInputPreviewSite *pSite);
	virtual void OnEndPreview(IInputPreviewSite *pSite);
	virtual void OnGetPreviewFreq(DWORD &dwFreq) {dwFreq = 0;}
	virtual void OnGetSize(short &cx, short &cy) {cx = cy = 0;}
	virtual void OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst) {};
	
	void ChangeSize();
	void Invalidate();

	BEGIN_INTERFACE_PART(InpPrv, IInputPreview)
		com_call BeginPreview(IInputPreviewSite *pSite);
		com_call EndPreview(IInputPreviewSite *pSite);
		com_call GetPreviewFreq(DWORD *pdwFreq);
		com_call GetSize(short *pdx, short *pdy);
		com_call DrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	END_INTERFACE_PART(InpPrv)
};

class CInputWndOverlayImpl : public CImplBase
{
protected:
	CInputWndOverlayImpl() {};
	~CInputWndOverlayImpl() {};

	virtual bool OnInitOverlay() {return false;}
	virtual void OnDeinitOverlay() {}
	virtual void OnGetOvrSize(short &cx, short &cy) {cx = cy = 0;}
	virtual bool OnStartOverlay(HWND hwndDraw, LPRECT lprcSrc, LPRECT lprcDst) {return false;}
	virtual void OnStopOverlay(HWND hwndDraw) {}
	virtual void OnSetDstRect(HWND hwndDraw, LPRECT lprcDst) {}
	virtual void OnSetSrcRect(HWND hwndDraw, LPRECT lprcSrc) {}

	BEGIN_INTERFACE_PART(InpWOvr, IInputWndOverlay)
		com_call InitOverlay();
		com_call DeinitOverlay();
		com_call GetSize(short *pdx, short *pdy);
		com_call StartOverlay(HWND hwndDraw, LPRECT lprcSrc, LPRECT lprcDst);
		com_call StopOverlay(HWND hwndDraw);
		com_call SetDstRect(HWND hwndDraw, LPRECT lprcDst);
		com_call SetSrcRect(HWND hwndDraw, LPRECT lprcSrc);
	END_INTERFACE_PART(InpWOvr)
};


class CInputWindowImpl : public CImplBase
{
protected:
	CInputWindowImpl();
	~CInputWindowImpl();
	CArray<HWND,HWND> m_arr;
	CUIntArray m_arrDev;

	virtual HWND OnCreateWindow(int nDevice, HWND hwndParent) {return NULL;}
	virtual void OnDestroyWindow(HWND hwnd) {}
	virtual void OnSize(HWND hwndCap, HWND hwndParent, SIZE sz) {}
	virtual void OnGetSize(HWND hwnd, short *pdx, short *pdy) {}

	BEGIN_INTERFACE_PART(InpWnd, IInputWindow)
		com_call CreateInputWindow(int nDev, HWND hwndParent, long *plCreated);
		com_call DestroyInputWindow(long lCap);
		com_call OnSize(long lCap, HWND hwndParent, SIZE sz);
		com_call GetSize(long lCap, short *pdx, short *pdy);
		com_call SetInputWindowPos(long lCap, int x, int y, int cx, int cy, DWORD dwFlags) {return S_OK;}
	END_INTERFACE_PART(InpWnd)
};

class CEntry : public CObject
{
public:
	CEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nValue, BOOL bIgnoreMethodic);
	CEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, CString strValue, BOOL bIgnoreMethodic);
	CEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue, BOOL bIgnoreMethodic);
	int m_nCurDev;
	BOOL m_bIgnoreMethodic;
	CString m_sSection;
	CString m_sEntry;
	int m_nValueType;
	int m_nValue;
	CString m_strValue;
	double m_dValue;
};

class CEntryArray : public CTypedPtrArray<CObArray,CEntry*>
{
public:
	~CEntryArray() {DeInit();}
	void DeInit();
};

/////////////////////////////////////////////////////////////////////////////
// CDriverBase command target

class CDriverBase : public CCmdTargetEx,
					public CNamedObjectImpl
{
	DECLARE_DYNCREATE(CDriverBase)
	ENABLE_MULTYINTERFACE();

	CString m_strDriverName;
	CString m_strDriverFullName;
	CString m_strCategory;
	DWORD m_dwFlags;
	CEntryArray m_EntryArray;
	CEntry *FindPreviewEntry(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, BOOL bIgnoreMethodic);
	int m_nPreviewMode;
	CStdProfileManager *m_pProfMgr;
	int GetProfileInt(LPCTSTR lpKeyName, int nDefault, bool bWriteToProfile = false);
	CString GetProfileString(LPCTSTR lpKeyName, LPCTSTR lpDefault);

	CDriverBase();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:
	

// Overrides
	virtual int OnGetDevicesCount(); // By default 1 device, names same as driver's
	virtual void OnGetDeviceNames(int nDev, CString &sShortName, CString &sLongName, CString &sCat);
	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual VARIANT OnGetValue(int nDev, LPCTSTR lpName);
	virtual VARIANT OnGetDefaultValue(int nDev, LPCTSTR lpName);
	virtual void OnInputImage(IUnknown *pUnkImage, IUnknown *punkTarget, int nDevice, BOOL bDialog) {};
	virtual bool OnInputVideoFile(CString &sVideoFileName, int nDevice, int nMode) {return false;}
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput) {return true;}
	virtual void OnExecuteDriverDialog(int nDev, LPCTSTR lpstrName) {}
	// It is for oreview image
	virtual void OnUpdateSizes() {}
	virtual void OnBeginPreviewMode();
	virtual void OnEndPreviewMode();
	virtual void OnApplyChanges();
	virtual void OnCancelChanges();
	// These functions supports work with preview settings store
	virtual int  OnGetValueInt(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nDef, UINT nFlags);
	virtual void OnSetValueInt(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nValue, UINT nFlags);
	virtual CString OnGetValueString(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sDef, UINT nFlags);
	virtual void OnSetValueString(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sValue, UINT nFlags);
	virtual double OnGetValueDouble(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dDef, UINT nFlags);
	virtual void OnSetValueDouble(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue, UINT nFlags);
	// Next function must change preview image
	virtual void DoSetValueInt(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, int nValue, UINT nFlags);
	virtual void DoSetValueString(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, LPCTSTR bstrValue, UINT nFlags);
	virtual void DoSetValueDouble(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, double dValue, UINT nFlags);
	virtual void OnSetValueIntPrv(int nCurDev, LPCTSTR strSection, LPCTSTR strEntry, int nValue);
	virtual void OnSetValueStringPrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, LPCTSTR sValue);
	virtual void OnSetValueDoublePrv(int nCurDev, LPCTSTR strSection, LPCTSTR sEntry, double dValue);
	virtual int OnGetBoardInt(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, int nDef) {return 0;}
	virtual CString OnGetBoardString(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, LPCTSTR sValue) {return CString("");}
	virtual double OnGetBoardDouble(int nCurDev, LPCTSTR sSection, LPCTSTR sEntry, double dValue) {return 0.;}
	virtual void ReinitPreview() {}
	virtual	bool OnGetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize) {return false;}
	virtual	bool OnConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk) {return false;}


	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverBase)
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDriverBase();

	// Generated message map functions
	//{{AFX_MSG(CDriverBase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(Drv, IDriver2)
		// IDriver
		com_call InputNative(IUnknown *pUnk, IUnknown *punkTarget, int nDevice, BOOL bDialog);
		com_call InputDIB(IDIBProvider *pDIBPrv, IUnknown *punkTarget, int nDevice, BOOL bFromSettings);
		com_call InputVideoFile(BSTR *pbstrVideoFile, int nDevice, int nMode);
		com_call ExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
		com_call GetFlags(DWORD *pdwFlags);
		com_call GetShortName(BSTR *pbstrShortName);
		com_call GetLongName(BSTR *pbstrShortName);
		com_call GetValue(int nDev, BSTR bstrName, VARIANT *pValue);
		com_call SetValue(int nDev, BSTR bstrName, VARIANT Value);
		com_call ExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
		com_call GetDevicesCount(int *pnCount);
		com_call GetDeviceNames(int nDev, BSTR *pbstrShortName, BSTR *pbstrLongName, BSTR *pbstrCategory);
		// IDriver2
		com_call UpdateSizes();
		com_call BeginPreviewMode();
		com_call EndPreviewMode();
		com_call CancelChanges();
		com_call ApplyChanges();
		com_call GetValueInt(int nCurDev, BSTR bstrSection, BSTR bstrEntry, int *pnValue, UINT nFlags);
		com_call SetValueInt(int nCurDev, BSTR bstrSection, BSTR bstrEntry, int nValue, UINT nFlags);
		com_call GetValueString(int nCurDev, BSTR bstrSection, BSTR bstrEntry, BSTR *pbstrValue, UINT nFlags);
		com_call SetValueString(int nCurDev, BSTR bstrSection, BSTR bstrEntry, BSTR bstrValue, UINT nFlags);
		com_call GetValueDouble(int nCurDev, BSTR bstrSection, BSTR bstrEntry, double *pdValue, UINT nFlags);
		com_call SetValueDouble(int nCurDev, BSTR bstrSection, BSTR bstrEntry, double dValue, UINT nFlags);
		com_call ReinitPreview();
		// IDriver3
		com_call GetImage(int nCurDev, LPVOID *ppData, DWORD *pdwDataSize);
		com_call ConvertToNative(int nCurDev, LPVOID lpData, DWORD dwDataSize, IUnknown *punk);
		com_call GetTriggerMode(int nCurDev, BOOL *pgTriggerMode);
		com_call SetTriggerMode(int nCurDev, BOOL bSet);
		com_call DoSoftwareTrigger(int nCurDev);
		com_call StartImageAquisition(int nCurDev, BOOL bStart);
		com_call IsImageAquisitionStarted(int nCurDev, BOOL *pbAquisition);
		com_call AddDriverSite(int nCurDev, IDriverSite *pSite);
		com_call RemoveDriverSite(int nCurDev, IDriverSite *pSite);
	END_INTERFACE_PART(Drv);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVERBASE_H__A7BADE84_CA80_4716_85B3_E3267A76F2FD__INCLUDED_)
