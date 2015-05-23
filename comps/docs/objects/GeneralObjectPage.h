#if !defined(AFX_GENERALOBJECTPAGE_H__F8295679_C1F3_48D5_B9DB_15B366056334__INCLUDED_)
#define AFX_GENERALOBJECTPAGE_H__F8295679_C1F3_48D5_B9DB_15B366056334__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GeneralObjectPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGeneralObjectPage dialog

class CGeneralObjectPage : public CDialog
{
// Construction
public:
	CGeneralObjectPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGeneralObjectPage)
	enum { IDD = IDD_OBJECTS_GENERAL };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGeneralObjectPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGeneralObjectPage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERALOBJECTPAGE_H__F8295679_C1F3_48D5_B9DB_15B366056334__INCLUDED_)
