#if !defined(AFX_CMPPAGE_H__27E491AB_31FD_4535_80D1_1204BDAE498D__INCLUDED_)
#define AFX_CMPPAGE_H__27E491AB_31FD_4535_80D1_1204BDAE498D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CmpPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCmpPage dialog
#define WM_RETURNFOCUS	(WM_USER+1)
class CCmpPage : public CPropertyPageBase
{
	DECLARE_DYNCREATE(CCmpPage)
	PROVIDE_GUID_INFO( )
	GUARD_DECLARE_OLECREATE(CCmpPage)

// Construction
public:
	CCmpPage();
	~CCmpPage();

// Dialog Data
	//{{AFX_DATA(CCmpPage)
	enum { IDD = IDD_COMPARE_PAGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCmpPage)
	public:
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCmpPage)
	afx_msg void OnClickAdd();
	afx_msg void OnClickDefault();
	afx_msg void OnClickOverlay();
	afx_msg void OnClickSub();
	afx_msg void OnClickOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnAngle();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
protected:
	void _UpdateControls();
	virtual void DoLoadSettings();
	virtual void DoStoreSettings();
	void ReturnFocus();
	virtual bool OnSetActive(){ReturnFocus();return true;};
	virtual const char *GetHelpTopic();


	int	m_nOverlayMode;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CMPPAGE_H__27E491AB_31FD_4535_80D1_1204BDAE498D__INCLUDED_)
