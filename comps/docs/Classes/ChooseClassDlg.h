#if !defined(AFX_CHOOSECLASSDLG_H__D7D2ADEC_134D_43C2_99FA_C31BF01CB163__INCLUDED_)
#define AFX_CHOOSECLASSDLG_H__D7D2ADEC_134D_43C2_99FA_C31BF01CB163__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChooseClassDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChooseClassDlg dialog
//#include "ObjListWrp.h"

class CChooseClassDlg : public CDialog
{
// Construction
public:
	CChooseClassDlg(CObjectListWrp & wrp, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseClassDlg)
	enum { IDD = IDD_CHOOSE_CLASS };
	CComboBox	m_Combo;
	//}}AFX_DATA
	
	LPCTSTR GetClassName()
	{	return 	m_strClassName;	}
	void	SetClassName(LPCTSTR szSelClassName);
	void FillCombo();

	LPCTSTR GetDialogTitle()
	{	return m_strDialogTitle;	}
	void SetDialogTitle(LPCTSTR szDlgTitle)
	{	m_strDialogTitle = szDlgTitle;	}
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseClassDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	CObjectListWrp &	m_listClasses;
	CString				m_strClassName;
	CPtrArray			m_arrObjects;

	CString				m_strDialogTitle;


	// Generated message map functions
	//{{AFX_MSG(CChooseClassDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOOSECLASSDLG_H__D7D2ADEC_134D_43C2_99FA_C31BF01CB163__INCLUDED_)
