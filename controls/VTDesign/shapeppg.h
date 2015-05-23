#if !defined(AFX_SHAPEPPG_H__94BE2ABC_1C4F_4564_B795_DCDDAEB99405__INCLUDED_)
#define AFX_SHAPEPPG_H__94BE2ABC_1C4F_4564_B795_DCDDAEB99405__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// shapeppg.h : header file
//

#include "jdraw\\ColourPicker.h"
#include "jdraw\\ArrowPicker.h"
#include "jdraw\\dashline.h"
#include "jdraw\\linePicker.h"

/////////////////////////////////////////////////////////////////////////////
// CVTShapePropPage : Property page dialog

class CVTShapePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTShapePropPage)
	DECLARE_OLECREATE_EX(CVTShapePropPage)

// Constructors
public:
	CVTShapePropPage();

// Dialog Data
	//{{AFX_DATA(CVTShapePropPage)
	enum { IDD = IDD_PROPPAGE_VTSHAPE };
	CArrowPicker	m_arrowStartPicker;
	CArrowPicker	m_arrowEndPicker;
	CLinePicker	m_lineStylePicker;
	CColourPicker	m_lineColorPicker;
	CColourPicker	m_fillColorPicker;
	CColourPicker	m_backColorPicker;	
	BOOL	m_bTransparentBk;
	BOOL	m_bTransparentFill;
	BOOL	m_bRoundedLine;
	double	m_fLineWidth;
	//}}AFX_DATA

	COLORREF m_lineColor;
	COLORREF m_fillColor;
	COLORREF m_backColor;
	DashStyle m_lineStyle;
	ArrowType m_arrowStart;
	ArrowType m_arrowEnd;

	short nLineStyle;
	short nArrowStart;
	short nEndStart;


// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support	
// Message maps
protected:
	//{{AFX_MSG(CVTShapePropPage)
	virtual BOOL OnInitDialog();
	afx_msg LRESULT OnColorChange(WPARAM lParam, LPARAM wParam);
	afx_msg LRESULT OnLineChange(WPARAM lParam, LPARAM wParam);
	afx_msg LRESULT OnArrowChange(WPARAM lParam, LPARAM wParam);	
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHAPEPPG_H__94BE2ABC_1C4F_4564_B795_DCDDAEB99405__INCLUDED_)
