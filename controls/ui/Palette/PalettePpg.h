#if !defined(AFX_PALETTEPPG_H__57C4CB58_9E87_11D3_A537_0000B493A187__INCLUDED_)
#define AFX_PALETTEPPG_H__57C4CB58_9E87_11D3_A537_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PalettePpg.h : Declaration of the CPalettePropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CPalettePropPage : See PalettePpg.cpp.cpp for implementation.

class CPalettePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPalettePropPage)
	DECLARE_OLECREATE_EX(CPalettePropPage)

// Constructor
public:
	CPalettePropPage();

// Dialog Data
	//{{AFX_DATA(CPalettePropPage)
	enum { IDD = IDD_PROPPAGE_PALETTE };
	int		m_nCols;
	int		m_nRows;
	int		m_nIntercellSpace;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CPalettePropPage)
	afx_msg void OnButtonFileopen();
	virtual BOOL OnInitDialog();
	afx_msg void OnCheckDrawedge();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PALETTEPPG_H__57C4CB58_9E87_11D3_A537_0000B493A187__INCLUDED)
