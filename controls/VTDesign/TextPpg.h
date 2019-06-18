#if !defined(AFX_TEXTPPG_H__BC127B42_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_TEXTPPG_H__BC127B42_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// TextPpg.h : Declaration of the CVTTextPropPage property page class.
#include "jdraw\\ColourPicker.h"
#include "jdraw\\ArrowPicker.h"
#include "jdraw\\dashline.h"
#include "jdraw\\linePicker.h"

////////////////////////////////////////////////////////////////////////////
// CVTTextPropPage : See TextPpg.cpp.cpp for implementation.

class CVTTextPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTTextPropPage)
	DECLARE_OLECREATE_EX(CVTTextPropPage)

// Constructor
public:
	CVTTextPropPage();

// Dialog Data
	//{{AFX_DATA(CVTTextPropPage)
	enum { IDD = IDD_PROPPAGE_VTDTEXT };
	CColourPicker	m_fontColorPicker;
	CColourPicker	m_backColorPicker;	
	CString	m_strText;		
	short	m_nDegree;
	int		m_vertAlign;
	int		m_horzAlign;
	CString	m_strKey;
	//}}AFX_DATA
	BOOL	m_bTransparentBk;	
	COLORREF m_backColor;
	COLORREF m_fontColor;


// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTTextPropPage)
	afx_msg LONG OnColorChange(UINT lParam, LONG wParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXTPPG_H__BC127B42_026C_11D4_8124_0000E8DF68C3__INCLUDED)
