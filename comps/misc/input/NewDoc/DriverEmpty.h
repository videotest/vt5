#if !defined(AFX_DRIVEREMPTY_H__C4DADA3D_E12D_4D96_9F16_2A7F1BC373DF__INCLUDED_)
#define AFX_DRIVEREMPTY_H__C4DADA3D_E12D_4D96_9F16_2A7F1BC373DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DriverEmpty.h : header file
//


#include "DriverBase.h"
#include "Input.h"

/////////////////////////////////////////////////////////////////////////////
// CDriverEmpty command target

class CDriverEmpty : public CDriverBase, public CInputPreviewImpl
{
	DECLARE_DYNCREATE(CDriverEmpty)
	ENABLE_MULTYINTERFACE()

	CDriverEmpty();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	virtual void OnSetValue(int nDev, LPCTSTR lpName, VARIANT var);
	virtual void OnGetSize(short &cx, short &cy);
	virtual void OnDrawRect(HDC hDC, LPRECT lpRectSrc, LPRECT lpRectDst);
	virtual void OnInputImage(IUnknown *pUnkTarget, IUnknown *pUnkImg, int nDevice, BOOL bDialog);
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverEmpty)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CDriverEmpty();

	// Generated message map functions
	//{{AFX_MSG(CDriverEmpty)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDriverEmpty)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDriverEmpty)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
//	BEGIN_INTERFACE_PART(Drv, IDriver)
//		com_call InputNative(IUnknown *pUnk);
//	END_INTERFACE_PART(Drv);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEREMPTY_H__C4DADA3D_E12D_4D96_9F16_2A7F1BC373DF__INCLUDED_)
