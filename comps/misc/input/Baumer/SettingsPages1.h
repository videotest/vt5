#if !defined(AFX_SHEETPAGES1_H__5D37543D_B55A_4066_8D35_3CC53546157A__INCLUDED_)
#define AFX_SHEETPAGES1_H__5D37543D_B55A_4066_8D35_3CC53546157A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SheetPages1.h : header file
//

#include "SettingsPages.h"
#include "SmartEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CManualPage dialog

class CManualPage : public CSettingsPage, public IEditSite
{
	DECLARE_DYNCREATE(CManualPage)

	void SetTopBottom();
// Construction
public:
	CManualPage();
	~CManualPage();

// Dialog Data
	//{{AFX_DATA(CManualPage)
	enum { IDD = IDD_MANUAL };
	CComboBox	m_Bits;
	CSliderCtrl	m_SliderGamma;
	CSpinButtonCtrl	m_SpinGamma;
	CSpinButtonCtrl	m_SpinTop;
	CSpinButtonCtrl	m_SpinBottom;
	//}}AFX_DATA
	CSmartLogDoubleEdit m_Gamma;

	void UpdateControls(int nHint = 0);
	virtual void OnChangeValue(CSmartIntEdit *pEdit, int nValue);

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CManualPage)
	public:
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnApply();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CManualPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeEBottom();
	afx_msg void OnChangeETop();
	afx_msg void OnChangeEditGamma();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDeltaposSpinGamma(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangeComboBits();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CTriggerDlg dialog

class CTriggerDlg : public CSettingsPage
{
	DECLARE_DYNCREATE(CTriggerDlg)

// Construction
public:
	CTriggerDlg();
	~CTriggerDlg();

// Dialog Data
	//{{AFX_DATA(CTriggerDlg)
	enum { IDD = IDD_TRIGGER };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA
	void UpdateControls(int nHint = 0);


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTriggerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTriggerDlg)
	afx_msg void OnDischarge();
	afx_msg void OnTriggerMode();
	afx_msg void OnSoftwareTrigger();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHEETPAGES1_H__5D37543D_B55A_4066_8D35_3CC53546157A__INCLUDED_)
