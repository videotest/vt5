#if !defined(AFX_PRINTFIELDAXPPG_H__58D296DD_31F9_4105_BEFA_1D4BECA42CA6__INCLUDED_)
#define AFX_PRINTFIELDAXPPG_H__58D296DD_31F9_4105_BEFA_1D4BECA42CA6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PrintFieldAXPpg.h : Declaration of the CPrintFieldAXPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXPropPage : See PrintFieldAXPpg.cpp.cpp for implementation.
#include "\vt5\ifaces\printfield.h"

class CPrintFieldAXPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CPrintFieldAXPropPage)
	DECLARE_OLECREATE_EX(CPrintFieldAXPropPage)

// Constructor
public:
	CPrintFieldAXPropPage();

// Dialog Data
	//{{AFX_DATA(CPrintFieldAXPropPage)
	enum { IDD = IDD_PROPPAGE_PRINTFIELDAX };
	CSpinButtonCtrl	m_spinWidth;
	CComboBox	m_comboStyles;
	CComboBox	m_comboGroups;
	//}}AFX_DATA
	CString					m_strKeyToRead;
	BOOL					m_bUseKeyToRead;

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CPrintFieldAXPropPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnWidthChange();
	afx_msg void OnProperties();
	afx_msg void OnCloseupCombo();
	afx_msg void OnCheckUseKey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnOK();

	IUnknown	*GetAXInstance();
	void _update_controls();
protected:
	print_field_params	m_params;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRINTFIELDAXPPG_H__58D296DD_31F9_4105_BEFA_1D4BECA42CA6__INCLUDED)
