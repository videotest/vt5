#if !defined(AFX_COLORPAGE3_H__59CF9B44_61E3_4A1D_B972_4F5570A6C769__INCLUDED_)
#define AFX_COLORPAGE3_H__59CF9B44_61E3_4A1D_B972_4F5570A6C769__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPage3.h : header file
//
#include "PowerColorPicker.h"
class CBCGColorDialog;

/////////////////////////////////////////////////////////////////////////////
// CColorPage3 dialog

class CColorPage3 : public CPropertyPage
{
	DECLARE_DYNCREATE(CColorPage3)

// Construction
public:
	void UpdateRGBL();
	BOOL m_IsReady;
	CBCGColorDialog* m_pDialog;
	CColorPage3();
	~CColorPage3();

// Dialog Data
	//{{AFX_DATA(CColorPage3)
	enum { IDD = IDD_BCGBARRES_COLOR_PAGE_THREE };
	CBCGColorPickerCtrl	m_luminance;
	CBCGColorPickerCtrl	m_hexgreyscale;
	UINT	m_L;
	UINT	m_B;
	UINT	m_G;
	UINT	m_R;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CColorPage3)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorPage3)
	afx_msg void OnDoubleClickedColor();
	virtual BOOL OnInitDialog();
	afx_msg void OnHexColor();
	afx_msg void OnLumColor();
	afx_msg void OnLumChanged();
	afx_msg void OnChangeB();
	afx_msg void OnChangeG();
	afx_msg void OnChangeR();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPAGE3_H__59CF9B44_61E3_4A1D_B972_4F5570A6C769__INCLUDED_)
