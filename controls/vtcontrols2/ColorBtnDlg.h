#if !defined(AFX_COLORBTNDLG_H__B424B2A9_C28D_47D3_8BE6_88B38E6FA5EE__INCLUDED_)
#define AFX_COLORBTNDLG_H__B424B2A9_C28D_47D3_8BE6_88B38E6FA5EE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorBtnDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorBtnDlg dialog

//#include "VTColorPickerCtl.h"	// Added by ClassView

// In order to ease use, these values have been hard coded in colorbtn.rc
// This avoids the need for another header file.


//#define IDD_COLORBTN                    (5100)
//#define IDC_COLOR1                      (5101)
//#define IDC_COLOR20                     (5120)
//#define IDC_OTHER                       (5121)


// Utility class for the dialog
#include "resource.h"
#include "BCGColorDialog.h"	// Added by ClassView
#include "ColorBtn.h"

class CColorBtnDlg : public CDialog
{
// Construction
public:
	void ResetColors();
	void OnCancel();
	CWnd* parent;
	void IncCustom();
	int m_currentcustom;
	
	COLORREF colors[30];
    BYTE used[30];
	bool m_bPalette;
	COLORREF m_Palette[256];


    int colorindex;

	CColorBtnDlg(CWnd* pParent = NULL);   // standard constructor	

     
// Dialog Data
	//{{AFX_DATA(CColorBtnDlg)
	enum { IDD = IDD_COLOR_DLG };
	CColorBtn	m_color9;
	CColorBtn	m_color8;
	CColorBtn	m_color7;
	CColorBtn	m_color6;
	CColorBtn	m_color5;
	CColorBtn	m_color4;
	CColorBtn	m_color30;
	CColorBtn	m_color3;
	CColorBtn	m_color29;
	CColorBtn	m_color28;
	CColorBtn	m_color27;
	CColorBtn	m_color26;
	CColorBtn	m_color25;
	CColorBtn	m_color24;
	CColorBtn	m_color23;
	CColorBtn	m_color22;
	CColorBtn	m_color21;
	CColorBtn	m_color20;
	CColorBtn	m_color19;
	CColorBtn	m_color18;
	CColorBtn	m_color17;
	CColorBtn	m_color16;
	CColorBtn	m_color15;
	CColorBtn	m_color14;
	CColorBtn	m_color13;
	CColorBtn	m_color12;
	CColorBtn	m_color11;
	CColorBtn	m_color10;
	CColorBtn	m_color2;
	CColorBtn	m_color1;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorBtnDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorBtnDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnOther();
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClose();
	//}}AFX_MSG

	void OnColor(UINT id);    
    void EndDialog( int nResult );

	DECLARE_MESSAGE_MAP()

private:
	CBCGColorDialog dlg;
public:
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORBTNDLG_H__B424B2A9_C28D_47D3_8BE6_88B38E6FA5EE__INCLUDED_)
