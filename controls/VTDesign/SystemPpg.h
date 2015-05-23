#if !defined(AFX_SYSTEMPPG_H__BC127B51_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_SYSTEMPPG_H__BC127B51_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// SystemPpg.h : Declaration of the CVTSystemPropPage property page class.
#include "jdraw\\ColourPicker.h"
#include "jdraw\\ArrowPicker.h"
#include "jdraw\\dashline.h"
#include "jdraw\\linePicker.h"

////////////////////////////////////////////////////////////////////////////
// CVTSystemPropPage : See SystemPpg.cpp.cpp for implementation.

class CVTSystemPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTSystemPropPage)
	DECLARE_OLECREATE_EX(CVTSystemPropPage)

// Constructor
public:
	CVTSystemPropPage();

// Dialog Data
	//{{AFX_DATA(CVTSystemPropPage)
	enum { IDD = IDD_PROPPAGE_VTDSYSTEM };
	CColourPicker	m_fontColorPicker;
	CColourPicker	m_backColorPicker;		
	int		m_nDataType;
	int		m_vertAlign;
	int		m_horzAlign;
	//}}AFX_DATA
	BOOL	m_bTransparentBk;	
	COLORREF m_backColor;
	COLORREF m_fontColor;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTSystemPropPage)
	afx_msg LRESULT OnColorChange(WPARAM lParam, LPARAM wParam);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYSTEMPPG_H__BC127B51_026C_11D4_8124_0000E8DF68C3__INCLUDED)
