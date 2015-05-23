#if !defined(AFX_GENERATEDLG_H__30E5378C_8130_482A_AADE_BBE90BD86B51__INCLUDED_)
#define AFX_GENERATEDLG_H__30E5378C_8130_482A_AADE_BBE90BD86B51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenerateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGenerateDlg dialog

class CGenerateDlg : public CDialog
{
// Construction
public:
	CGenerateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGenerateDlg)
	enum { IDD = IDD_GENERATE };	
	CString	m_strUIName;
	CString	m_strEvent;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenerateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenerateDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	sptrIDBaseDocument m_spDBaseDoc;
public:
	void SetDBaseDocument( IUnknown* pUnkDBDoc );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERATEDLG_H__30E5378C_8130_482A_AADE_BBE90BD86B51__INCLUDED_)
