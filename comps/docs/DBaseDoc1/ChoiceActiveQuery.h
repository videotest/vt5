#if !defined(AFX_CHOICEACTIVEQUERY_H__E1298BEA_702F_4844_BE05_005F56BC7CC5__INCLUDED_)
#define AFX_CHOICEACTIVEQUERY_H__E1298BEA_702F_4844_BE05_005F56BC7CC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChoiceActiveQuery.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChoiceActiveQuery dialog

class CChoiceActiveQuery : public CDialog
{
// Construction
public:
	CChoiceActiveQuery(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChoiceActiveQuery)
	enum { IDD = IDD_CHOICE_QUERY };
	CString	m_strActiveQuery;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChoiceActiveQuery)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

public:
	void SetDBaseDocument( IUnknown* pUnkDoc );
// Implementation
protected:
	sptrIDBaseDocument m_spDBaseDoc;

	// Generated message map functions
	//{{AFX_MSG(CChoiceActiveQuery)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	IMPLEMENT_HELP( "ChoiceActiveQuery" );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHOICEACTIVEQUERY_H__E1298BEA_702F_4844_BE05_005F56BC7CC5__INCLUDED_)
