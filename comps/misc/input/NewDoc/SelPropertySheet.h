// SelPropertySheet.h : header file
//
// This class defines custom modal property sheet 
// CSelPropertySheet.
 
#ifndef __SELPROPERTYSHEET_H__
#define __SELPROPERTYSHEET_H__

#include "SelPropertyPages.h"

/////////////////////////////////////////////////////////////////////////////
// CSelPropertySheet

class CSelPropertySheet : public CPropertySheet
{
	DECLARE_DYNAMIC(CSelPropertySheet)

// Construction
public:
	CSelPropertySheet(CWnd* pWndParent = NULL);

// Attributes
public:
	CCommonPropertyPage m_Page1;
	CInputPropertyPage m_Page2;

	CString m_sCreator;

// Operations
public:
	void SetActivePage(int nPage);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelPropertySheet)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSelPropertySheet();

// Generated message map functions
protected:
	//{{AFX_MSG(CSelPropertySheet)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif	// __SELPROPERTYSHEET_H__
