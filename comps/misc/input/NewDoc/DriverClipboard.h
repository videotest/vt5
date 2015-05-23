// DriverClipboard.h: interface for the CDriverClipboard class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERCLIPBOARD_H__578D2749_0333_40A6_A2DF_32E396ACDD41__INCLUDED_)
#define AFX_DRIVERCLIPBOARD_H__578D2749_0333_40A6_A2DF_32E396ACDD41__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DriverBase.h"

class CDriverClipboard : public CDriverBase, public CInputPreviewImpl  
{
	DECLARE_DYNCREATE(CDriverClipboard)
	ENABLE_MULTYINTERFACE()

	CDriverClipboard();

// Attributes
public:

// Operations
public:

// Overrides
	virtual bool OnBeginPreview(IInputPreviewSite *pSite);
	virtual void OnEndPreview(IInputPreviewSite *pSite);
	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual VARIANT OnGetValue(int nDev, LPCTSTR lpName);
	virtual void OnGetSize(short &cx, short &cy);
	virtual void OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	virtual void OnInputImage(IUnknown *pUnkTarget, IUnknown *pUnkImg, int nDevice, BOOL bDialog);
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *pTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);
	virtual void OnGetPreviewFreq(DWORD &dwFreq);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverClipboard)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDriverClipboard();

	// Generated message map functions
	//{{AFX_MSG(CDriverClipboard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverClipboard)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverClipboard)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

#endif // !defined(AFX_DRIVERCLIPBOARD_H__578D2749_0333_40A6_A2DF_32E396ACDD41__INCLUDED_)

