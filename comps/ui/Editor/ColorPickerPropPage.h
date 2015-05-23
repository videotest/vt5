#if !defined(AFX_COLORPICKERPROPPAGE_H__A2237368_5EAD_42AC_88F7_B73AFA088664__INCLUDED_)
#define AFX_COLORPICKERPROPPAGE_H__A2237368_5EAD_42AC_88F7_B73AFA088664__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorPickerPropPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorWnd
class CColorWnd : public CWnd
{
	COLORREF m_color;
public:
	CColorWnd();
	~CColorWnd();
	
	void SetColor( COLORREF color );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorWnd)
	protected:	
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CColorWnd)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	HICON m_hIconCantPickColor;
};


#include "PropPageInt.h"
/////////////////////////////////////////////////////////////////////////////
// CColorPickerPropPage dialog

class CColorPickerPropPage : public CPropertyPageBase
{
	PROVIDE_GUID_INFO( )
	DECLARE_DYNCREATE(CColorPickerPropPage);
	GUARD_DECLARE_OLECREATE(CColorPickerPropPage);
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();

// Construction
public:
	CColorPickerPropPage();   // standard constructor
	~CColorPickerPropPage();

// Dialog Data
	//{{AFX_DATA(CColorPickerPropPage)
	enum { IDD = IDD_PROPPAGE_COLOR_PICKER };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CColorPickerPropPage)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CColorPickerPropPage)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BEGIN_INTERFACE_PART(ColorPickerPropPage, IColorPickerPropPage)
		com_call ChangeColor( DWORD dwRGB, color H, color S, color L );
	END_INTERFACE_PART(ColorPickerPropPage);


	CColorWnd m_wndColor;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORPICKERPROPPAGE_H__A2237368_5EAD_42AC_88F7_B73AFA088664__INCLUDED_)
