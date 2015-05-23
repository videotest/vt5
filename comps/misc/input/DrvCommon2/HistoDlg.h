#if !defined(__HistoDlg_H__)
#define __HistoDlg_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistoDlg.h : header file
//

#include "HistBox.h"

/////////////////////////////////////////////////////////////////////////////
// CHistoDlg dialog

struct CHistoIds
{
	int m_nIdDlg;
	int m_nIdHisto,m_nIdGray,m_nIdRed,m_nIdGreen,m_nIdBlue;
	int m_n0,m_nMaxColor,m_nMaxPoints,m_nMsg,m_nStaticPeriod;
};

class CHistoDlg : public CDialog
{
	CHistoIds m_IDs;
	CRect m_rcGray, m_rcRed, m_rcGreen, m_rcBlue, m_rcMsg;
	CString m_sMsg;
	void SetMaxColor();
// Construction
public:
	CHistoDlg(CHistoIds IDs, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHistoDlg)
	CStatic	m_Static0;
	CStatic	m_MaxColor;
	CStatic	m_MaxPoints;
	CStatic	m_Msg;
	CStatic	m_Period;
	CButton	m_Blue;
	CButton	m_Green;
	CButton	m_Red;
	CButton	m_Gray;
	CHistBox m_HistoBox;
	BOOL	m_bGray;
	BOOL	m_bRed;
	BOOL	m_bGreen;
	BOOL	m_bBlue;
	//}}AFX_DATA


	void InitShow();
	void ResetMinMax();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CHistoDlg)
	afx_msg void OnBlue();
	afx_msg void OnGray();
	afx_msg void OnGreen();
	afx_msg void OnRed();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
public:
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif
