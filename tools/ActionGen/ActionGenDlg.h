// ActionGenDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CActionGenDlg dialog

class CActionGenDlg : public CDialog
{
// Construction
public:
	CActionGenDlg(CWnd* pParent = NULL);	// standard constructor
	~CActionGenDlg();

// Dialog Data
	//{{AFX_DATA(CActionGenDlg)
	enum { IDD = IDD_ACTIONGEN_DIALOG };
	CString	m_strAction;
	CString	m_strFileName;
	CString	m_strProjectName;
	BOOL	m_nSettings;
	CString	m_strTarget;
	BOOL	m_bTarget;
	BOOL	m_bInteractive;
	BOOL	m_bUndo;
	BOOL	m_bUpdate;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActionGenDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CActionGenDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnGenerate();
	afx_msg void OnBrowse();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
