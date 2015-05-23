#if !defined(AFX_CARIOANALIZEVIEW_H__07AF6BA5_FC19_4078_983E_490C0F4155F8__INCLUDED_)
#define AFX_CARIOANALIZEVIEW_H__07AF6BA5_FC19_4078_983E_490C0F4155F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CarioAnalizeView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCarioAnalizeView window


#include "carioview.h"
#include "PropBagImpl.h"

class CCarioAnalizeView : public CCarioView
{
	DECLARE_DYNCREATE(CCarioAnalizeView)

	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO( )

// Construction
public:
	CCarioAnalizeView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCarioAnalizeView)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCarioAnalizeView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCarioAnalizeView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CCarioAnalizeView)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCarioAnalizeView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

public:
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CARIOANALIZEVIEW_H__07AF6BA5_FC19_4078_983E_490C0F4155F8__INCLUDED_)
