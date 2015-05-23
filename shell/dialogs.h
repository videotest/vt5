#ifndef __dialogs_h__
#define __dialogs_h__
// Get the best document template for the named file

class CNewTypeDlg : public CDialog
{
public:
	CDocTemplate*   m_pSelectedTemplate;

public:
	//{{AFX_DATA(CNewTypeDlg)
	enum { IDD = AFX_IDD_NEWTYPEDLG };
	//}}AFX_DATA
	CNewTypeDlg() : CDialog(CNewTypeDlg::IDD)
	{
		m_pSelectedTemplate = NULL;
	}
	~CNewTypeDlg() { }

protected:
	BOOL OnInitDialog();
	void OnOK();
	//{{AFX_MSG(CNewTypeDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__dialogs_h__/////////////////////////////////////////////////////////////////////////////
// CChooseWindowDlg dialog

class CChooseWindowDlg : public CDialog
{
// Construction
public:
	CChooseWindowDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChooseWindowDlg)
	enum { IDD = IDD_CHOOSE_WINDOW };
	CString	m_strWindowName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChooseWindowDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChooseWindowDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkWindows();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
