#if !defined(AFX_PAGE_H__D6B8CE33_E83F_11D3_A0AC_0000B493A187__INCLUDED_)
#define AFX_PAGE_H__D6B8CE33_E83F_11D3_A0AC_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Page.h : header file
//
#include "ActiveXSite.h"
/////////////////////////////////////////////////////////////////////////////
// CPage dialog
//it's page for options dialog
class CPage : public CPropertyPage,
			  public CNamedObjectImpl
	
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CPage)

// Construction
public:
	CPage();
	~CPage();

	void SetForm( IUnknown *punkForm );
	void SetDlgTitle(CString strTitle);
	


	CActiveXSite	m_site;

// Dialog Data
	//{{AFX_DATA(CPage)
	enum { IDD = IDD_DEFAULT_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	afx_msg LRESULT OnHelpHitTest(WPARAM, LPARAM);
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	LRESULT OnCaptionChange(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGE_H__D6B8CE33_E83F_11D3_A0AC_0000B493A187__INCLUDED_)
