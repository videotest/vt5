#if !defined(AFX_HISTODLG_H__D2DD3945_36EE_41F4_AC28_24E14331DCF4__INCLUDED_)
#define AFX_HISTODLG_H__D2DD3945_36EE_41F4_AC28_24E14331DCF4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HistoDlg.h : header file
//

#include "HistBox.h"

/////////////////////////////////////////////////////////////////////////////
// CHistoDlg dialog

class CHistoDlg : public CDialog
{
	CRect m_rcGray, m_rcRed, m_rcGreen, m_rcBlue, m_rcMsg;
	CString m_sMsg;
	void SetMaxColor();
// Construction
public:
	CHistoDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CHistoDlg)
	enum { IDD = IDD_HISTO };
	//sergey 21.12.05
    CStatic	m_Period1;
	//end
	CStatic	m_Static0;
	CStatic	m_MaxColor;
	CStatic	m_MaxPoints;
	CStatic	m_Msg;
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
	//sergey 21.12.05
	void ResetPeriod(DWORD dwFps);
	//end

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
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HISTODLG_H__D2DD3945_36EE_41F4_AC28_24E14331DCF4__INCLUDED_)
