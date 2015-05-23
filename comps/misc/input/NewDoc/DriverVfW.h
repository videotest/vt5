// DriverVfW.h: interface for the CDriverVfW class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERVFW_H__E9155F5D_803C_4A2A_B6BD_41B7E49D348A__INCLUDED_)
#define AFX_DRIVERVFW_H__E9155F5D_803C_4A2A_B6BD_41B7E49D348A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DriverBase.h"

class CVfwDeviceDescr
{
public:
	CString m_sDrvName;
	CString m_sDrvVer;
};

class CDriverVfW : public CDriverBase, CInputWindowImpl
{
	DECLARE_DYNCREATE(CDriverVfW)
	ENABLE_MULTYINTERFACE()

	LPBITMAPINFOHEADER m_lpbi;
	CArray<CVfwDeviceDescr,CVfwDeviceDescr&> m_arrDevices;
	void RebuildArray();
	void CancelDlg();
	int VCInput(HWND hWndParent, LPBITMAPINFOHEADER *plpbi, int nDevice, const char *pszDescr, BOOL bNoModal, BOOL bClose, BOOL bSaveImage, BOOL bForAVI = FALSE);

	HWND FindHwndForDevice(int nDev);

	CDriverVfW();

// Attributes
public:
	virtual HWND OnCreateWindow(int nDevice, HWND hwndParent);
	virtual void OnDestroyWindow(HWND hwnd);
	virtual void OnSize(HWND hwndCap, HWND hwndParent, SIZE sz);

// Operations
public:

// Overrides
	virtual int OnGetDevicesCount(); // By default 1 device, names same as driver's
	virtual void OnGetDeviceNames(int nDev, CString &sShortName, CString &sLongName, CString &sCat);
	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual VARIANT OnGetValue(int nDev, LPCTSTR lpName);
	virtual void OnInputImage(IUnknown *pUnkTarget, IUnknown *pUnkImg, int nDevice, BOOL bDialog);
	virtual bool OnInputVideoFile(CString &sVideoFileName, int nDevice, int nMode);
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
	virtual void CDriverVfW::OnExecuteDriverDialog(int nDev, LPCTSTR lpstrName);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverVfW)
	public:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDriverVfW();

	// Generated message map functions
	//{{AFX_MSG(CDriverVfW)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverVfW)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverVfW)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

#endif // !defined(AFX_DRIVERVFW_H__E9155F5D_803C_4A2A_B6BD_41B7E49D348A__INCLUDED_)
