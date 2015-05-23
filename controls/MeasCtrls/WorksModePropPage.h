#pragma once
#include "resource.h"

// CWorksModePropPage : Property page dialog

class CWorksModePropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CWorksModePropPage)
	DECLARE_OLECREATE_EX(CWorksModePropPage)

// Constructors
public:
	CWorksModePropPage();

// Dialog Data
	enum { IDD = IDD_WORKSMODEPROPPAGE };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_BWorksMode;
};
