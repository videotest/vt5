#if !defined(AFX_INSERTCONTROLDLG_H__C6531855_F70A_11D0_8E26_00C04FB68D60__INCLUDED_)
#define AFX_INSERTCONTROLDLG_H__C6531855_F70A_11D0_8E26_00C04FB68D60__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// InsertControlDlg.H : header file
//

/////////////////////////////////////////////////////////////////////////////
// CInsertControlDlg dialog
#include "..\\resource.h"
#include "comcat.h"

class CInsertControlDlg : public CDialog
{
// Construction
public:
	CInsertControlDlg(CWnd* pParent = NULL);   // standard constructor
	~CInsertControlDlg();
// Dialog Data
	//{{AFX_DATA(CInsertControlDlg)
	enum { IDD = IDD_INSERTCONTROL };
	CStatic m_staticServerPath;
	CButton m_butRequiredCategories;
	CButton m_butIgnoreRequiredCategories;
	CButton m_butOK;
	CListBox    m_lbControls;
	//}}AFX_DATA

   CArray< CLSID, CLSID& >	m_arrCLSID;
   CLSID m_clsid;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInsertControlDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
   void RefreshControlList();

	ICatInformationPtr m_pCatInfo;
	CArray< CATID, CATID& > m_aImplementedCategories;
	CArray< CATID, CATID& > m_aRequiredCategories;
	CList< CLSID, CLSID& > m_lControls;

	// Generated message map functions
	//{{AFX_MSG(CInsertControlDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnControlsDblClk();
	afx_msg void OnControlsSelChange();
	//}}AFX_MSG
	afx_msg void OnOK();
	afx_msg void OnHelp();
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INSERTCONTROLDLG_H__C6531855_F70A_11D0_8E26_00C04FB68D60__INCLUDED_)
