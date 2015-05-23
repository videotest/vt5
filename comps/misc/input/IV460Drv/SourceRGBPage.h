#if !defined(AFX_SOURCERGBPAGE_H__184A06D2_A0A9_4C28_9093_8DB2BEBF04F6__INCLUDED_)
#define AFX_SOURCERGBPAGE_H__184A06D2_A0A9_4C28_9093_8DB2BEBF04F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SourceRGBPage.h : header file
//

#include "SettingsSheet.h"
#include "DriverWrp.h"

/////////////////////////////////////////////////////////////////////////////
// CSourceRGBPage dialog

class CSourceRGBPage : public CSettingsPage
{
	DECLARE_DYNCREATE(CSourceRGBPage)

// Construction
public:
	CSourceRGBPage(IUnknown *punk = NULL);
	~CSourceRGBPage();


// Dialog Data
	//{{AFX_DATA(CSourceRGBPage)
	enum { IDD = IDD_SOURCE_RGB };
	CSpinButtonCtrl	m_SpOffsetBlue;
	CSpinButtonCtrl	m_SpGainBlue;
	CSliderCtrl	m_SOffsetBlue;
	CSliderCtrl	m_SGainBlue;
	CSpinButtonCtrl	m_SpOffsetRed;
	CSpinButtonCtrl	m_SpOffsetGreen;
	CSpinButtonCtrl	m_SpGainRed;
	CSpinButtonCtrl	m_SpGainGreen;
	CSliderCtrl	m_SOffsetRed;
	CSliderCtrl	m_SOffsetGreen;
	CSliderCtrl	m_SGainRed;
	CSliderCtrl	m_SGainGreen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSourceRGBPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnApply();
	virtual void OnCancel();
	virtual void OnOK();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSourceRGBPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnChannelRgb0();
	afx_msg void OnChannelRgb1();
	afx_msg void OnChangeEGainG();
	afx_msg void OnChangeEGainRb();
	afx_msg void OnChangeEOffsetG();
	afx_msg void OnChangeEOffsetRb();
	afx_msg void OnDeltaposSpGainG(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeltaposSpGainRb(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnChangeEGainB();
	afx_msg void OnChangeEOffsetB();
	afx_msg void OnDeltaposSpGainBlue(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCERGBPAGE_H__184A06D2_A0A9_4C28_9093_8DB2BEBF04F6__INCLUDED_)
