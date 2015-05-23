#if !defined(AFX_ACIVECOLORSVIEWPPG_H__0C1F6A9B_9DB3_11D3_A536_0000B493A187__INCLUDED_)
#define AFX_ACIVECOLORSVIEWPPG_H__0C1F6A9B_9DB3_11D3_A536_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// AciveColorsViewPpg.h : Declaration of the CAciveColorsViewPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewPropPage : See AciveColorsViewPpg.cpp.cpp for implementation.

class CAciveColorsViewPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CAciveColorsViewPropPage)
	DECLARE_OLECREATE_EX(CAciveColorsViewPropPage)

// Constructor
public:
	CAciveColorsViewPropPage();

// Dialog Data
	//{{AFX_DATA(CAciveColorsViewPropPage)
	enum { IDD = IDD_PROPPAGE_ACIVECOLORSVIEW };
	CEdit	m_editFileName;
	CButton	m_btnBrowse;
	CString	m_strFileName;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CAciveColorsViewPropPage)
	afx_msg void OnButtonFileopen();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACIVECOLORSVIEWPPG_H__0C1F6A9B_9DB3_11D3_A536_0000B493A187__INCLUDED)
