#if !defined(AFX_TEMPLATEDLG_H__911B19E7_C90C_41F7_B41B_2BF71E2A730A__INCLUDED_)
#define AFX_TEMPLATEDLG_H__911B19E7_C90C_41F7_B41B_2BF71E2A730A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TemplateDlg.h : header file
//

#include "types.h"
/////////////////////////////////////////////////////////////////////////////
// CTemplateDlg dialog

class CTemplateDlg : public CDialog
{
// Construction
public:
	CTemplateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateDlg)
	enum { IDD = IDD_TEMPLATE };
	int		m_nTemplateSource;
	int		m_nNewTemplateTarget;
	int		m_nAlghoritm;
	CString	m_strTemplateObjectName;
	CString	m_strTemplateFileName;
	CString	m_strTemplateName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTemplateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnTenplateSource();
	afx_msg void OnRadio3();
	afx_msg void OnGenerate();
	afx_msg void OnGenerate2();
	afx_msg void OnGenerate3();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void SetWindowsEnable( bool bUpdateData = true );
	void ON_Wnd( UINT nIDD );
	void OFF_Wnd( UINT nIDD );

	sptrIDocumentSite	m_sptrDocSite;
public:
	void SetDocSite( IUnknown* punkDS );

protected:
	void FillExistTemplates();

	
	void GetTemplateSettingsFromDocument();
	void SetTemplateSettingsToDocument();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDLG_H__911B19E7_C90C_41F7_B41B_2BF71E2A730A__INCLUDED_)
