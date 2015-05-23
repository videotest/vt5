#if !defined(AFX_TEMPLATESETUP_H__8F2B6321_7D37_4D92_9C2F_3C17C4A1FB74__INCLUDED_)
#define AFX_TEMPLATESETUP_H__8F2B6321_7D37_4D92_9C2F_3C17C4A1FB74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateSetup.h : header file
//

CString _GetValueString( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszDefault );
long _GetValueInt( IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lDefault );


/////////////////////////////////////////////////////////////////////////////
// CTemplateSetup dialog

enum TemplateSource1{
	tsDocument = 0,
	tsFile = 1,
	tsNew = 2
};

enum PrintContext{
	pcAllObject = 0,
	pcActiveView  = 1
};

enum StoreTarget{
	stActiveDocument = 0,	
	stFile = 1
};




class CTemplateSetup : public CDialog
{
// Construction
public:
	CTemplateSetup(CWnd* pParent = NULL);   // standard constructor
	void SeDocumentSite( IDocumentSite*	pDocSite ){
		m_pDocSite = pDocSite;
	}

	BOOL ReadData();
	BOOL WriteData();

	//Only for files copy from *.rpt
	BOOL GenerateNewTemplate( CString strBaseTempl, 
							CString strNewTemplName,							
							BOOL bStore,
							PrintContext pc,
							StoreTarget st
							);


// Dialog Data
	//{{AFX_DATA(CTemplateSetup)
	enum { IDD = IDD_TEMPLATE_SETUP };
	CComboBox	m_ExistCtrl;
	CComboBox	m_ExistDocCtrl;
	CComboBox	m_BasedOnCtrl;
	int		m_nSourceTemplate;
	BOOL	m_bStore;
	int		m_nStoreTarget;
	int		m_nPrintContext;
	CString	m_strNewTemplateName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateControls();
	IDocumentSite*	m_pDocSite;
	CString m_strTemplateName;
	void FillComboBoxes();
	// Generated message map functions
	//{{AFX_MSG(CTemplateSetup)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSourceTemplate();
	afx_msg void OnSourceTemplate1();
	afx_msg void OnSourceTemplate2();
	afx_msg void OnAllDataObject();
	afx_msg void OnAllDataObject2();
	afx_msg void OnStore();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATESETUP_H__8F2B6321_7D37_4D92_9C2F_3C17C4A1FB74__INCLUDED_)
