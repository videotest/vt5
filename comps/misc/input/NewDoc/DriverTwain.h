// DriverTwain.h: interface for the CDriverTwain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERTWAIN_H__6A002FB8_83CF_46C4_BC5A_90CC42C8E116__INCLUDED_)
#define AFX_DRIVERTWAIN_H__6A002FB8_83CF_46C4_BC5A_90CC42C8E116__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DriverBase.h"

class CDriverTwain : public CDriverBase  
{
	DECLARE_DYNCREATE(CDriverTwain)
	ENABLE_MULTYINTERFACE()

	HGLOBAL m_hMem;
	bool m_bDSMInited;
	void InitDSM();

	CDriverTwain();

// Attributes
public:

// Operations
public:

// Overrides
	virtual int OnGetDevicesCount(); // By default 1 device, names same as driver's
	virtual void OnGetDeviceNames(int nDev, CString &sShortName, CString &sLongName, CString &sCat);
	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual VARIANT OnGetValue(int nDev, LPCTSTR lpName);
	virtual void OnInputImage(IUnknown *pUnkTarget, IUnknown *pUnkImg, int nDevice, BOOL bDialog);
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverTwain)
	public:
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDriverTwain();

	// Generated message map functions
	//{{AFX_MSG(CDriverTwain)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverTwain)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverTwain)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

#endif // !defined(AFX_DRIVERTWAIN_H__6A002FB8_83CF_46C4_BC5A_90CC42C8E116__INCLUDED_)
