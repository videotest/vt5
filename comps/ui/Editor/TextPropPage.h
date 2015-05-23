#if !defined(AFX_TEXTPROPPAGE_H__18551BC6_A955_11D3_A548_0000B493A187__INCLUDED_)
#define AFX_TEXTPROPPAGE_H__18551BC6_A955_11D3_A548_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TextPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTextPropPage dialog

class CTextPropPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CTextPropPage);
	GUARD_DECLARE_OLECREATE(CTextPropPage);
// Construction
public:
	CTextPropPage(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CTextPropPage)
	enum { IDD = IDD_PROPPAGE_TEXT };
	CEdit	m_editText;
	CListBox	m_lbText;
	//}}AFX_DATA

	virtual bool OnSetActive();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CTextPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnClickPbAdd();
	afx_msg void OnClickPbDelete();
	afx_msg void OnClickPbWrite();
	afx_msg void OnSelchangeLbText();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG

	BEGIN_INTERFACE_PART(Text, ITextPage)
		com_call SetText( BSTR bstrText );
	END_INTERFACE_PART(Text)
		
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
	
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTPROPPAGE_H__18551BC6_A955_11D3_A548_0000B493A187__INCLUDED_)
