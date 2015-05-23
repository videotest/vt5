#if !defined(AFX_PROPERTYSHEETMAN_H__FD74A2D8_7FAC_11D3_A50C_0000B493A187__INCLUDED_)
#define AFX_PROPERTYSHEETMAN_H__FD74A2D8_7FAC_11D3_A50C_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PropertySheetBase.h"

// PropertySheetMan.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// CPropertySheetMan command target

class CPropertySheetMan : public CPropertySheetBase,
						  public CDockWindowImpl
{
	DECLARE_DYNCREATE(CPropertySheetMan)
	GUARD_DECLARE_OLECREATE(CPropertySheetMan)

	ENABLE_MULTYINTERFACE();
public:
	CPropertySheetMan();           // protected constructor used by dynamic creation
	virtual ~CPropertySheetMan();
// Attributes
public:
// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertySheetMan)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
protected:
	// Generated message map functions
	//{{AFX_MSG(CPropertySheetMan)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()

	virtual void OnHide();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSHEETMAN_H__FD74A2D8_7FAC_11D3_A50C_0000B493A187__INCLUDED_)
