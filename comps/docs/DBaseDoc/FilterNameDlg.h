#if !defined(AFX_FILTERNAMEDLG_H__5888B089_B58F_49FE_A2D0_E6536B2BE8B6__INCLUDED_)
#define AFX_FILTERNAMEDLG_H__5888B089_B58F_49FE_A2D0_E6536B2BE8B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FilterNameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFilterNameDlg dialog

class CFilterNameDlg : public CDialog
{
// Construction
public:
	CFilterNameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CFilterNameDlg)
	enum { IDD = IDD_FILTER_NAME };
	CString	m_strFilterName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterNameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFilterNameDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void OnHelp()
	{
		HelpDisplay( "FilterName" );
	}


	CStringArray m_arFilters;
public:
	void AddFilter( CString strFilter );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERNAMEDLG_H__5888B089_B58F_49FE_A2D0_E6536B2BE8B6__INCLUDED_)
