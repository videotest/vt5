#if !defined(AFX_DBASEWIZ_H__BF2BE790_8FFF_424C_BF61_996CA36C74A4__INCLUDED_)
#define AFX_DBASEWIZ_H__BF2BE790_8FFF_424C_BF61_996CA36C74A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DBaseWiz.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewDBTypePage dialog

class CNewDBTypePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNewDBTypePage)

// Construction
public:
	CNewDBTypePage();
	~CNewDBTypePage();

// Dialog Data
	//{{AFX_DATA(CNewDBTypePage)
	enum { IDD = IDD_NEW_DBASE_WIZARD };
	int		m_nDBaseType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewDBTypePage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNewDBTypePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CNewDBAccessPage dialog

class CNewDBAccessPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNewDBAccessPage)

// Construction
public:
	CNewDBAccessPage();
	~CNewDBAccessPage();

// Dialog Data
	//{{AFX_DATA(CNewDBAccessPage)
	enum { IDD = IDD_NEW_DBASE_WIZARD_ACCESS };
	CString	m_strDBPath;
	int		m_nTemplate;
	CString	m_strTemplatePath;
	BOOL	m_bCreateNewTable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewDBAccessPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNewDBAccessPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnTemplateUseExistDB();
	afx_msg void OnEmptyDatabase();
	afx_msg void OnLoadDbLocation();
	afx_msg void OnLoadTemplateLocation();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void SetAppearance();

};
/////////////////////////////////////////////////////////////////////////////
// CNewDBAdvPage dialog

class CNewDBAdvPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CNewDBAdvPage)

// Construction
public:
	CNewDBAdvPage();
	~CNewDBAdvPage();

// Dialog Data
	//{{AFX_DATA(CNewDBAdvPage)
	enum { IDD = IDD_NEW_DBASE_WIZARD_ADV };
	CString	m_strConnectionString;
	CString	m_strPassword;
	CString	m_strUserName;
	BOOL	m_bAskForLogin;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNewDBAdvPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CNewDBAdvPage)
	afx_msg void OnConnect();
	afx_msg void OnTest();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void SetAppearance();

};
/////////////////////////////////////////////////////////////////////////////
// CDBInfoCreate dialog

class CDBInfoCreate : public CPropertyPage
{
	DECLARE_DYNCREATE(CDBInfoCreate)

// Construction
public:
	CDBInfoCreate();
	~CDBInfoCreate();

// Dialog Data
	//{{AFX_DATA(CDBInfoCreate)
	enum { IDD = IDD_NEW_DBASE_INFO };
	CString	m_strTaskList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDBInfoCreate)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDBInfoCreate)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CDBCreateProcess dialog

class CDBCreateProcess : public CPropertyPage
{
	DECLARE_DYNCREATE(CDBCreateProcess)

// Construction
public:
	CDBCreateProcess();
	~CDBCreateProcess();

// Dialog Data
	//{{AFX_DATA(CDBCreateProcess)
	enum { IDD = IDD_NEW_DBASE_PROCESS };
	CString	m_strProgress;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CDBCreateProcess)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CDBCreateProcess)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DBASEWIZ_H__BF2BE790_8FFF_424C_BF61_996CA36C74A4__INCLUDED_)
