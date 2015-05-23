// SelPropertyPages.h : header file
//

#ifndef __SELPROPERTYPAGES_H__
#define __SELPROPERTYPAGES_H__

/////////////////////////////////////////////////////////////////////////////
// CCommonPropertyPage dialog

class CCommonPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCommonPropertyPage)

	CStringArray m_saCategories;
	CStringArray m_saShortNames;
	CStringArray m_saLongNames;
// Construction
public:
	CCommonPropertyPage();
	~CCommonPropertyPage();

// Dialog Data
	//{{AFX_DATA(CCommonPropertyPage)
	enum { IDD = IDD_PROPPAGE1 };
	CListBox	m_CreatorsList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCommonPropertyPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCommonPropertyPage)
	afx_msg void OnSelchangeCreatorsList();
	//}}AFX_MSG
	afx_msg void OnHelp();
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CInputPropertyPage dialog

class CInputPropertyPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CInputPropertyPage)

	CStringArray m_saShortNames;
	CStringArray m_saLongNames;

	void UpdateControls();

// Construction
public:
	CInputPropertyPage();
	~CInputPropertyPage();

	bool m_bDisableBack;

// Dialog Data
	//{{AFX_DATA(CInputPropertyPage)
	enum { IDD = IDD_PROPPAGE2 };
	CButton	m_UseFrame;
	CButton	m_EditFrame;
	CEdit	m_EImagesNum;
	CButton	m_Accumulate;
	CSpinButtonCtrl	m_SImagesNum;
	CListBox	m_DriverList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CInputPropertyPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CInputPropertyPage)
	afx_msg void OnSelchangeDriversList();
	afx_msg void OnAccumulation();
	afx_msg void OnChangeEImagesNum();
	afx_msg void OnEditInputFrame();
	afx_msg void OnUseFrame();
	//}}AFX_MSG
	afx_msg void OnHelp();
	DECLARE_MESSAGE_MAP()

};



#endif // __SELPROPERTYPAGES_H__
