#pragma once

// VTTablePropPage.h : Declaration of the CVTTablePropPage property page class.


// CVTTablePropPage : See VTTablePropPage.cpp.cpp for implementation.

class CVTTablePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTTablePropPage)
	DECLARE_OLECREATE_EX(CVTTablePropPage)

// Constructor
public:
	CVTTablePropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VTTABLE };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPaste();

protected:
	IDispatch*	GetAXInstance();
public:
	afx_msg void OnBnClickedCopy();

	CString	m_sRootSection;
};

