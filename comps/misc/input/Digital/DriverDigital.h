// DriverDigital.h: interface for the CDriverDigital class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERDIGITAL_H__BB893CEC_21CE_11D4_A98A_FC9799051A4B__INCLUDED_)
#define AFX_DRIVERDIGITAL_H__BB893CEC_21CE_11D4_A98A_FC9799051A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DriverBase.h"
#include "Input.h"

class CDriverDigital : public CDriverBase, public CInputPreviewImpl
{
	DECLARE_DYNCREATE(CDriverDigital)
	ENABLE_MULTYINTERFACE()

	LPBITMAPINFOHEADER m_lpbi;
	int m_nUseCount;
	CDriverDigital();

public:
// Overrides
	// Support of input preview
	virtual bool OnBeginPreview(IInputPreviewSite *pSite);
	virtual void OnEndPreview(IInputPreviewSite *pSite);
	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual VARIANT OnGetValue(int nDev, LPCTSTR lpName);
	virtual void OnGetSize(short &cx, short &cy);
	virtual void OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	virtual void OnGetPreviewFreq(DWORD &dwFreq);
	// Support of driver base interfaces
	virtual void OnInputImage(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bDialog);
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);

// Implementation
protected:
	virtual ~CDriverDigital();

	// Generated message map functions
	//{{AFX_MSG(CDriverDigital)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverDigital)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverDigital)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

#endif // !defined(AFX_DRIVERDIGITAL_H__BB893CEC_21CE_11D4_A98A_FC9799051A4B__INCLUDED_)
