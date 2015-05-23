#if !defined(AFX_PUSHBUTTONPPG_H__54A0BF95_9A92_11D3_B1C5_EB19BDBABA39__INCLUDED_)
#define AFX_PUSHBUTTONPPG_H__54A0BF95_9A92_11D3_B1C5_EB19BDBABA39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PushButtonPpg.h : Declaration of the CPushButtonPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CPushButtonPropPage : See PushButtonPpg.cpp.cpp for implementation.

class CPushButtonPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPushButtonPropPage)
	DECLARE_OLECREATE_EX(CPushButtonPropPage)

// Constructor
public:
	CPushButtonPropPage();

// Dialog Data
	//{{AFX_DATA(CPushButtonPropPage)
	enum { IDD = IDD_PROPPAGE_PUSHBUTTON };
	CButton	m_checkStretch;
	CButton	m_checkDrawBorder;
	CSpinButtonCtrl	m_spinShadowSize;
	CEdit	m_editWidth;
	CEdit	m_editHeight;
	CButton	m_btnBrowse;
	CEdit	m_editFileName;
	CString	m_strFileName;
	int		m_nWidth;
	int		m_nHeight;
	BOOL	m_bAutoSize;
	BOOL	m_bMouseInShadow;
	BOOL	m_bMouseOutShadow;
	BOOL	m_bButtonPressedShadow;
	BOOL	m_bFlatButton;
	BOOL	m_bButtonPressedPicColor;
	BOOL	m_bMouseOutPicColor;
	BOOL	m_bMouseInPicColor;
	BOOL	m_bDrawBorder;
	BOOL	m_bThreeStateButton;
	int		m_nButtonDepth;
	BOOL	m_bDrawFocusRect;
	BOOL	m_bStretch;
	BOOL	m_bRunAction;
	CString	m_strActionName;
	BOOL	m_bAutoRepeat;
	BOOL	m_bDrawFocusedRect;
	BOOL	m_bSemiflat;
	BOOL	m_bUseSystemFont;
	BOOL	m_bShowToolTip;
	CString	m_sToolTip;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void _OnCheckAutosize( BOOL bSave );
	void _OnCheckFlatbutton( BOOL bSave );	

// Message maps
protected:
	//{{AFX_MSG(CPushButtonPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonFileopen();
	afx_msg void OnCheckAutosize();
	afx_msg void OnCheckshadow();
	afx_msg void OnCheckFlatbutton();
	afx_msg void OnAutoSize();
	afx_msg void OnReloadBitmap();
	afx_msg void OnPaste();
	afx_msg void OnCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PUSHBUTTONPPG_H__54A0BF95_9A92_11D3_B1C5_EB19BDBABA39__INCLUDED)
