#if !defined(AFX_PROPERTYSHEETCTRL_H__9FB2516E_D37E_476B_918B_917B0ECD3548__INCLUDED_)
#define AFX_PROPERTYSHEETCTRL_H__9FB2516E_D37E_476B_918B_917B0ECD3548__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PropertySheetCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPropertySheetCtrl

class CPropertySheetCtrl : public CPropertySheet
{
	DECLARE_DYNAMIC(CPropertySheetCtrl)

// Construction
public:
	CPropertySheetCtrl(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
	CPropertySheetCtrl(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertySheetCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CPropertySheetCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPropertySheetCtrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROPERTYSHEETCTRL_H__9FB2516E_D37E_476B_918B_917B0ECD3548__INCLUDED_)
