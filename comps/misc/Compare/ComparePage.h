#if !defined(AFX_COMPAREPAGE_H__B3FE16EC_2A82_4337_91EE_EACCAFFF03C4__INCLUDED_)
#define AFX_COMPAREPAGE_H__B3FE16EC_2A82_4337_91EE_EACCAFFF03C4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ComparePage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CComparePage dialog

class CComparePage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CComparePage)
	GUARD_DECLARE_OLECREATE(CComparePage)

// Construction
public:
	CComparePage();
	~CComparePage();

// Dialog Data
	//{{AFX_DATA(CComparePage)
	enum { IDD = IDD_PAGE_COMPARE };
	long	m_nAngle;
	long	m_nWidth;
	long	m_nXPos;
	long	m_nYPos;
	long	m_nHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CComparePage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CComparePage)
	virtual BOOL OnInitDialog();
	afx_msg void OnClickDown();
	afx_msg void OnClickHeight();
	afx_msg void OnClickHeightDown();
	afx_msg void OnClickLeft();
	afx_msg void OnClickRight();
	afx_msg void OnClickRotateLeft();
	afx_msg void OnClickRotateRight();
	afx_msg void OnClickUp();
	afx_msg void OnClickWidth();
	afx_msg void OnClickWidthDown();
	afx_msg void OnDefault();
	afx_msg void OnChangeEdit();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
protected:
	void DoLoadSettings();
	void DoStoreSettings();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMPAREPAGE_H__B3FE16EC_2A82_4337_91EE_EACCAFFF03C4__INCLUDED_)
