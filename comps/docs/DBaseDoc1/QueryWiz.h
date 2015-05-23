#if !defined(AFX_EDITQUERYPAGE_H__6C8CF57E_2426_4B12_9926_09F2F13CAF54__INCLUDED_)
#define AFX_EDITQUERYPAGE_H__6C8CF57E_2426_4B12_9926_09F2F13CAF54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// QueryWiz.h : header file
//

#include "filtercondition.h"
/////////////////////////////////////////////////////////////////////////////
// CEditQueryPage dialog

class CEditQueryPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CEditQueryPage)
	
// Construction
public:
	CEditQueryPage();
	~CEditQueryPage();

// Dialog Data
	//{{AFX_DATA(CEditQueryPage)
	enum { IDD = IDD_NEW_QUERY_WIZARD_SQL };
	CComboBox	m_ctrlMainTable;
	int		m_nSimpleQuery;
	CString	m_strSQL;
	CString	m_strMainTable;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CEditQueryPage)
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
	//{{AFX_MSG(CEditQueryPage)
	afx_msg void OnSimpleQuery();
	afx_msg void OnAdvancedQuery();
	afx_msg void OnInsertField();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()	

	void SetEnable2Controls();

	BOOL	_OnSetActive();
	
	TABLEINFO_ARR_PTR	m_pArrTableIfo;
	TABLEINFO_ARR		m_arrTableInfo;
	bool				FillTableInfo();
	bool				m_bExternalFillTableInfo;
	
public:
	void SetTableInfo( TABLEINFO_ARR_PTR pArrTableIfo ){ m_pArrTableIfo = pArrTableIfo; m_bExternalFillTableInfo = true; }
	
public:
	
	
};


/////////////////////////////////////////////////////////////////////////////
// CTumbnailPage

class CTumbnailPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CTumbnailPage)
// Construction
public:
	CTumbnailPage();   // standard constructor
	~CTumbnailPage();

// Dialog Data
	//{{AFX_DATA(CTumbnailPage)
	enum { IDD = IDD_TUMBNAIL };
	int		m_nGenerateHeight;
	int		m_nGenerateWidth;
	int		m_nViewHeight;
	int		m_nViewWidth;
	int		m_nBorderSize;
	int		m_nCaption;
	int		m_nField;
	//}}AFX_DATA


	bool	m_bSimpleQuery;
	CString	m_strMainTable;
	CString	m_strSQL;

	void	FillControlsFromData( TABLEINFO_ARR_PTR pArrTable );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTumbnailPage)
	public:
	virtual BOOL OnSetActive();
	virtual LRESULT OnWizardBack();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTumbnailPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTumbnailField();
	afx_msg void OnSelchangeTumbnailCaption();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	TABLEINFO_ARR_PTR m_pArrTableIfo;
public:
	void SetTableInfo( TABLEINFO_ARR_PTR pArrTableIfo ){ m_pArrTableIfo = pArrTableIfo; }	


	CString m_strCaptionTable;
	CString m_strCaptionField;

	CString m_strThumbnailTable;
	CString m_strThumbnailField;

	void	GetTableFieldsFromDlg( );

	CArray<_CField*, _CField*> m_arrfiCaption;
	CArray<_CField*, _CField*> m_arrfiThumbnail;
	
	void DestroyFieldArray();

};


/////////////////////////////////////////////////////////////////////////////
// CFieldChooserDlg dialog

class CFieldChooserDlg : public CDialog
{
// Construction
public:
	CFieldChooserDlg(CWnd* pParent = NULL);   // standard constructor
	

// Dialog Data
	//{{AFX_DATA(CFieldChooserDlg)
	enum { IDD = IDD_FIELD_CHOOSER };
	CTreeCtrl	m_ctrlFields;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFieldChooserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFieldChooserDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	IMPLEMENT_HELP( "FieldChooser" );

	TABLEINFO_ARR_PTR m_pArrTableIfo;
public:
	void SetTableInfo( TABLEINFO_ARR_PTR pArrTableIfo ){ m_pArrTableIfo = pArrTableIfo; }

	CString m_strFields;
	CString m_strTable;

	
	afx_msg void OnNMClickFields(NMHDR *pNMHDR, LRESULT *pResult);
};
/////////////////////////////////////////////////////////////////////////////
// CQueryNamePage dialog

class CQueryNamePage : public CPropertyPage
{
	DECLARE_DYNCREATE(CQueryNamePage)

// Construction
public:
	CQueryNamePage();
	~CQueryNamePage();

// Dialog Data
	//{{AFX_DATA(CQueryNamePage)
	enum { IDD = IDD_NEW_QUERY_WIZARD_NAME };
	CString	m_strQueryName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CQueryNamePage)
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
	//{{AFX_MSG(CQueryNamePage)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITQUERYPAGE_H__6C8CF57E_2426_4B12_9926_09F2F13CAF54__INCLUDED_)
