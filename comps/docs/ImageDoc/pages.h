#if !defined(AFX_PAGES_H__84531A53_A0AD_11D3_A6A3_0090275995FE__INCLUDED_)
#define AFX_PAGES_H__84531A53_A0AD_11D3_A6A3_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
// objpages.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFixedObjectPage dialog

class CCropPageDlg : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CCropPageDlg);
	GUARD_DECLARE_OLECREATE(CCropPageDlg);
// Construction
public:
	CCropPageDlg();   // standard constructor
	~CCropPageDlg();

// Dialog Data
	//{{AFX_DATA(CCropPageDlg)
	enum { IDD = IDD_DIALOG_CROP };
	CEdit	m_editWidth;
	CEdit	m_editHeight;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCropPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CCropPageDlg)
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeSizes();
	virtual BOOL OnInitDialog();
	afx_msg void OnRadioCalibr();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	void _UpdateEditors();
	void _UpdateValues();
	bool _fixx( CString &str );

protected:
	double	m_nWidth;
	double	m_nHeight;
	bool	m_bUseCalibration;
	bool	m_bDisableNotify;
};

#endif 
