#if !defined(AFX_FORMPAGE_H__5A407F83_E440_11D3_A0A3_0000B493A187__INCLUDED_)
#define AFX_FORMPAGE_H__5A407F83_E440_11D3_A0A3_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormPage.h : header file
//

#include "FormDialog.h"
/////////////////////////////////////////////////////////////////////////////
// CFormPage dialog
//it's page for mainframe settings mode
class CFormPage : public CFormDialog,
				  public CNamedObjectImpl
{
	ENABLE_MULTYINTERFACE();
// Construction
public:
	CFormPage( const char *pszTitle );   // standard constructor
	virtual void SetForm( IUnknown *punkForm );

	
// Dialog Data
	//{{AFX_DATA(CFormPage)
	enum { IDD = IDD_CONTAINER_DOCKDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormPage)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormPage)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMPAGE_H__5A407F83_E440_11D3_A0A3_0000B493A187__INCLUDED_)
