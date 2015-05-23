// DriverFromAVI.h: interface for the CDriverFromAVI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DRIVERFROMAVI_H__3509FED2_7254_499C_A08E_601A2073C4E1__INCLUDED_)
#define AFX_DRIVERFROMAVI_H__3509FED2_7254_499C_A08E_601A2073C4E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DriverBase.h"
#include "Input.h"

class CDriverFromAVI :  public CDriverBase, public CInputPreviewImpl  
{
	DECLARE_DYNCREATE(CDriverFromAVI)
	ENABLE_MULTYINTERFACE()
	bool m_bFirst;
	GuidKey m_lDocKey;
public:
	CDriverFromAVI();

	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual void OnGetSize(short &cx, short &cy);
	virtual void OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	virtual void OnInputImage(IUnknown *pUnkImg, IUnknown *punkTarget, int nDevice, BOOL bDialog);
	virtual bool OnExecuteSettings(HWND hwndParent, IUnknown *punkTarget, int nDevice, int nMode, BOOL bFirst, BOOL bForInput);

protected:
	virtual ~CDriverFromAVI();

	// Generated message map functions
	//{{AFX_MSG(CDriverFromAVI)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverFromAVI)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverFromAVI)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

};

#endif // !defined(AFX_DRIVERFROMAVI_H__3509FED2_7254_499C_A08E_601A2073C4E1__INCLUDED_)
