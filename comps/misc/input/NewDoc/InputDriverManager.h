#if !defined(AFX_INPUTDRIVERMANAGER_H__530DE72D_608F_4218_991A_C393B4FB1283__INCLUDED_)
#define AFX_INPUTDRIVERMANAGER_H__530DE72D_608F_4218_991A_C393B4FB1283__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// InputDriverManager.h : header file
//


class CDeviceInfo;

/////////////////////////////////////////////////////////////////////////////
// CInputDriverManager command target

class CInputDriverManager : public CCmdTargetEx,
						    public CNamedObjectImpl,
						    public CCompManagerImpl,
						    public CRootedObjectImpl
{
	DECLARE_DYNCREATE(CInputDriverManager)
	ENABLE_MULTYINTERFACE()

	CInputDriverManager();           // protected constructor used by dynamic creation

	CTypedPtrArray<CPtrArray, CDeviceInfo*> m_Devices;
	bool m_bInited;

protected:
	void RebuildDevices();

// Attributes
public:
	CString m_sSectName;

// Operations
public:
	CString OnGetCurrentDeviceName();
	void OnSetCurrentDeviceName(CString s);
	IUnknown *GetDriver(int nNum);
	BOOL FindDriver(CString sDevName, IUnknown **ppunkDriver, int *pnDevice);
	int OnGetDevicesCount();
	void OnGetDeviceInfo(int nDevice, CString &sShortName, CString &sLongName, CString &sCategory, int *pnDriver, int *pnDeviceInDriver);
	bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nSelectDriver, int nDialogMode, BOOL bFirst, BOOL bFromSettings, BSTR bstrConf);
	bool SelectDriver(HWND hwndParent, int nMode);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputDriverManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CInputDriverManager();

	// Generated message map functions
	//{{AFX_MSG(CInputDriverManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CInputDriverManager)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CInputDriverManager)
	afx_msg BSTR GetCurrentDriver1();
	afx_msg void SetCurrentDriver(LPCTSTR lpszNewValue);
	afx_msg BSTR GetCurrentDevice();
	afx_msg void SetCurrentDevice(LPCTSTR lpszNewValue);
	afx_msg BSTR GetCurrentDeviceName();
	afx_msg void SetCurrentDeviceName(LPCTSTR lpszNewValue);
	afx_msg void SetValue(LPCTSTR Name, const VARIANT FAR& Value);
	afx_msg VARIANT GetValue(LPCTSTR Name);
	afx_msg long GetDevicesNum();
	afx_msg BSTR GetDeviceName(long iDev);
	afx_msg BSTR GetDeviceCategory(long iDev);
	afx_msg BSTR GetDeviceFullName(long iDev);
	afx_msg void ExecuteDriverDialog(LPCTSTR strName);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	BEGIN_INTERFACE_PART(DrvMan, IDriverManager2)
		com_call GetCurrentDeviceName(BSTR *pbstrShortName);
		com_call SetCurrentDeviceName(BSTR bstrShortName);
		com_call IsInputAvailable(BOOL *pbAvail);
		com_call ExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nSelectDriver, int nDialogMode, BOOL bFirst, BOOL bFromSettings);
		com_call GetCurrentDeviceAndDriver(BOOL *pbFound, IUnknown **ppunkDriver, int *pnDeviceInDriver);
		com_call GetSectionName(BSTR *pbstrSecName);
		com_call GetDevicesCount(int *pnDeviceCount);
		com_call GetDeviceInfo(int nDevice, BSTR *pbstrDevShortName, BSTR *pbstrDevLongName, BSTR *pbstrCategory, int *pnDeviceDriver,
			int *pnDeviceInDriver);
		com_call ExecuteSettings2(HWND hwndParent, IUnknown *pTarget, int nSelectDriver, int nDialogMode,
			BOOL bFirst, BOOL bFromSettings, BSTR bstrConfName);
	END_INTERFACE_PART(DrvMan);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPUTDRIVERMANAGER_H__530DE72D_608F_4218_991A_C393B4FB1283__INCLUDED_)
