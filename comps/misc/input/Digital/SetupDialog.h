#if !defined(AFX_SETUPDIALOG_H__A231B156_80FA_11D1_A291_00002148EA4B__INCLUDED_)
#define AFX_SETUPDIALOG_H__A231B156_80FA_11D1_A291_00002148EA4B__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SetupDialog.h : header file
//

class CInputThread;

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog
#include "ViewGrb.H"

class CSetupDialog : public CDialog
{
// Construction
	CInputThread	*m_ptrd;
	HANDLE			m_hEvt;
	HANDLE			m_hEvt1;
	BOOL			m_bSomeChanged;
public:
	CSetupDialog(CWnd* pParent = NULL);   // standard constructor
	~CSetupDialog();

	static BITMAPINFOHEADER *m_pbi;

// Dialog Data
	//{{AFX_DATA(CSetupDialog)
	enum { IDD = IDD_DLG_SETUP };
	CComboBox	m_comboShutter;
	CComboBox	m_comboAddr;
	CSliderCtrl	m_zlev;
	CSpinButtonCtrl	m_spinFrames;
	CSliderCtrl	m_amp;
	int		m_iFrames;
	int		m_LimMin;
	int		m_LimMax;
	int		m_iZeroLevel;
	BOOL	m_bSuper;
	BOOL	m_bGamma;
	//}}AFX_DATA
	int	m_iAmp, m_iZero, m_iAdr;
	CViewGrb m_View;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnTechShowHide();
	virtual void OnOK();
	afx_msg void OnSomeChanged();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual void OnCancel();
	afx_msg void OnTune();
	afx_msg void OnChangeEdit0();
	afx_msg void OnChangeEdit255();
	afx_msg void OnSelchangeShutter();
	afx_msg void OnSuper();
	afx_msg void OnInterfaceSimple();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void OnMem();
	afx_msg void OnSetZero();
	afx_msg void OnGamma();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnGrabDone( WPARAM, LPARAM );
	afx_msg LRESULT OnSetupMinMax( WPARAM, LPARAM );
	DECLARE_MESSAGE_MAP()

	void EnableControls();
	void StoreVals();
	void StoreDefVals();
	void RestoreDefVals();
	void InitVals();
	void EnableInterface();

	void BreakCureentFrame();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDIALOG_H__A231B156_80FA_11D1_A291_00002148EA4B__INCLUDED_)
