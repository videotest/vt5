#if !defined(AFX_WIZBUTTON_H__3EA676A0_EB54_11D3_A0B3_0000B493A187__INCLUDED_)
#define AFX_WIZBUTTON_H__3EA676A0_EB54_11D3_A0B3_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// WizButton.h : header file
//


class CWizard;
class CFormManager;
/////////////////////////////////////////////////////////////////////////////
// CWizButton window

class CWizButton : public CButton
{
// Construction
public:
	CWizButton(CWizard* pOwner, HWND hwndParent, UINT nID, CString strCaption);
	CWizButton(CFormManager* pOwner, HWND hwndParent, UINT nID, CString strCaption);

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWizButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWizButton();

	// Generated message map functions
protected:
	//{{AFX_MSG(CWizButton)
	afx_msg void OnEnable(BOOL bEnable);
	//}}AFX_MSG
	afx_msg void OnBtnClick();

	DECLARE_MESSAGE_MAP()

	CWizard*	m_pWizard;
	CFormManager* m_pFormManager;

	HWND		m_hwnd_notify;
	unsigned	m_id;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WIZBUTTON_H__3EA676A0_EB54_11D3_A0B3_0000B493A187__INCLUDED_)
