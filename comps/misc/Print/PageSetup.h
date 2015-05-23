#if !defined(AFX_PAGESETUP_H__246C5858_A819_441C_8304_B063BCD6579D__INCLUDED_)
#define AFX_PAGESETUP_H__246C5858_A819_441C_8304_B063BCD6579D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PageSetup.h : header file
//
struct PageSize
{
	char* szPageName;
	DWORD	data;
	double fWidth;
	double fHeight;
};

/////////////////////////////////////////////////////////////////////////////
// CPageSetup dialog
class CPageSetup : public CDialog
{
	CRect m_rcPrnMargins;
// Construction
public:
	CPageSetup(CWnd* pParent = NULL);   // standard constructor
	void SetData( double fWidth, double fHeight,
					double fLeftField, double fTopField, 
					double fRightField, double fBottomField,
					int nOrientation
					);

// Dialog Data
	//{{AFX_DATA(CPageSetup)
	enum { IDD = IDD_PAGESETUP };
	CComboBox	m_paperCtrl;	
	double	m_fBottom;
	int		m_nOrientation;
	int		m_nPaperName;
	double	m_fHeight;
	double	m_fLeft;
	double	m_fTop;
	double	m_fWidth;
	double	m_fRight;
	BOOL	m_bUseSimpleMode;
	int		m_nBuildMode;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageSetup)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void DrawPage( CDC* pDC );
	void TryUpdateCtrl( UINT nID );
	BOOL m_bInitDlg;
	void RepaintPage();

	int m_nPrnLeft, m_nPrnTop, m_nPrnRight, m_nPrnBottom;

	

	// Generated message map functions
	//{{AFX_MSG(CPageSetup)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePaperSize();
	afx_msg void OnPortrait();
	afx_msg void OnLandscape();
	afx_msg void OnPaint();
	afx_msg void OnChangeWidth();
	afx_msg void OnChangeHeight();
	afx_msg void OnChangeBottom();
	afx_msg void OnChangeRight();
	afx_msg void OnChangeTop();
	afx_msg void OnChangeLeft();
	virtual void OnOK();
	afx_msg void OnHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PAGESETUP_H__246C5858_A819_441C_8304_B063BCD6579D__INCLUDED_)
