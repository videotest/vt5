#if !defined(AFX_3DBARPPG_H__BC127B4C_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_3DBARPPG_H__BC127B4C_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "jdraw\\ColourPicker.h"
#include "jdraw\\ArrowPicker.h"
#include "jdraw\\dashline.h"
#include "jdraw\\linePicker.h"

// 3DBarPpg.h : Declaration of the CVT3DBarPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVT3DBarPropPage : See 3DBarPpg.cpp.cpp for implementation.

class CVT3DBarPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVT3DBarPropPage)
	DECLARE_OLECREATE_EX(CVT3DBarPropPage)

// Constructor
public:
	CVT3DBarPropPage();

// Dialog Data
	//{{AFX_DATA(CVT3DBarPropPage)
	enum { IDD = IDD_PROPPAGE_VTD3DBAR };
	CColourPicker	m_fontColorPicker;
	CColourPicker	m_backColorPicker;	
	CString	m_strText;
	int		m_nTextAlign;
	int		m_nBorderType;
	int		m_nTextPosition;
	double	m_fLineWidth;
	//}}AFX_DATA
	BOOL	m_bTransparentBk;	
	COLORREF m_backColor;
	COLORREF m_fontColor;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVT3DBarPropPage)
	afx_msg LRESULT OnColorChange(WPARAM lParam, LPARAM wParam);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_3DBARPPG_H__BC127B4C_026C_11D4_8124_0000E8DF68C3__INCLUDED)
