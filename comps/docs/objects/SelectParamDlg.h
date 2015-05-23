#if !defined(AFX_SELECTPARAMDLG_H__CD4BCA17_97D8_4EAF_8202_7DAE0B911FF1__INCLUDED_)
#define AFX_SELECTPARAMDLG_H__CD4BCA17_97D8_4EAF_8202_7DAE0B911FF1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectParamDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSelectParamDlg dialog

class CSelectParamDlg : public CDialog
{
// Construction
public:
	CSelectParamDlg(CWnd* pParent = NULL);   // standard constructor

	void AddParam(CString strParam)
	{
		if(!strParam.IsEmpty()) m_arrParams.Add(strParam);
	};

	CString GetSelectedParamName() {return m_strLastSelected;};

// Dialog Data
	//{{AFX_DATA(CSelectParamDlg)
	enum { IDD = IDD_SELECT_PARAM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectParamDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectParamDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnDblclkListParams();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


	CStringArray	m_arrParams;
	CString			m_strLastSelected;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTPARAMDLG_H__CD4BCA17_97D8_4EAF_8202_7DAE0B911FF1__INCLUDED_)
