#if !defined(AFX_LINEPPG_H__BC127B33_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_LINEPPG_H__BC127B33_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LinePpg.h : Declaration of the CVTLinePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTLinePropPage : See LinePpg.cpp.cpp for implementation.

class CVTLinePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTLinePropPage)
	DECLARE_OLECREATE_EX(CVTLinePropPage)

// Constructor
public:
	CVTLinePropPage();

// Dialog Data
	//{{AFX_DATA(CVTLinePropPage)
	enum { IDD = IDD_PROPPAGE_VTDLINE };
	short	m_nOrientation;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTLinePropPage)
	afx_msg void OnLeft();
	afx_msg void OnRight();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINEPPG_H__BC127B33_026C_11D4_8124_0000E8DF68C3__INCLUDED)
