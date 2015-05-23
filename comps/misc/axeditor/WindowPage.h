#if !defined(AFX_WINDOWPAGE_H__31D839D1_6D4C_4E99_9C75_345B7FA5796C__INCLUDED_)
#define AFX_WINDOWPAGE_H__31D839D1_6D4C_4E99_9C75_345B7FA5796C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WindowPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CWindowPage dialog

class CWindowPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CWindowPage);
	GUARD_DECLARE_OLECREATE(CWindowPage);
// Construction
public:
	CWindowPage();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CWindowPage)
	enum { IDD = IDD_PAGE_WINDOW };
	CSpinButtonCtrl	m_spinHeight;
	CSpinButtonCtrl	m_spinWidth;
	CSpinButtonCtrl	m_spinTop;
	CSpinButtonCtrl	m_spinLeft;
	BOOL	m_bDisabled;
	BOOL	m_bGroup;
	int		m_nLeft;
	CString	m_strName;
	BOOL	m_bTabstop;
	int		m_nTop;
	BOOL	m_bVisible;
	int		m_nWidth;
	int		m_nHeight;
	int		m_nID;
	//}}AFX_DATA

	bool	CheckOwnUnknown( IUnknown *punk );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWindowPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWindowPage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	virtual BOOL OnInitDialog();
	afx_msg void OnSetfocusName();
	afx_msg void OnKillfocusName();
	//}}AFX_MSG
	virtual void OnOK();
	DECLARE_MESSAGE_MAP()

	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(MyPropertyPage, IPropertyPage2)
		com_call SetPageSite(LPPROPERTYPAGESITE);
		com_call Activate(HWND, LPCRECT, BOOL);
		com_call Deactivate();
		com_call GetPageInfo(LPPROPPAGEINFO);
		com_call SetObjects(ULONG, LPUNKNOWN*);
		com_call Show(UINT);
		com_call Move(LPCRECT);
		com_call IsPageDirty();
		com_call Apply();
		com_call Help(LPCOLESTR);
		com_call TranslateAccelerator(LPMSG);
		com_call EditProperty(DISPID);
	END_INTERFACE_PART(MyPropertyPage)
protected:
	IUnknownPtr	m_ptrControl;
	bool		m_bActive;
	
	bool		m_bCanEntry;

	void VerifyUniqueName( bool bSetFocus = false );
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WINDOWPAGE_H__31D839D1_6D4C_4E99_9C75_345B7FA5796C__INCLUDED_)
