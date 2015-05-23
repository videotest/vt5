#if !defined(AFX_COLORRANGECTRLPPG_H__615E7221_02F7_4C1F_880D_7791CF203061__INCLUDED_)
#define AFX_COLORRANGECTRLPPG_H__615E7221_02F7_4C1F_880D_7791CF203061__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ColorRangeCtrlPpg.h : Declaration of the CColorRangePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CColorRangePropPage : See ColorRangeCtrlPpg.cpp.cpp for implementation.

class CColorRangePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CColorRangePropPage)
	DECLARE_OLECREATE_EX(CColorRangePropPage)

// Constructor
public:
	CColorRangePropPage();

// Dialog Data
	//{{AFX_DATA(CColorRangePropPage)
	enum { IDD = IDD_PROPPAGE_COLORRANGE };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CColorRangePropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORRANGECTRLPPG_H__615E7221_02F7_4C1F_880D_7791CF203061__INCLUDED)
