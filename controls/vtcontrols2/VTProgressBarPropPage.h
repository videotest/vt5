#pragma once
#include "Resource.h"

// VTProgressBarPropPage.h : Declaration of the CVTProgressBarPropPage property page class.


// CVTProgressBarPropPage : See VTProgressBarPropPage.cpp.cpp for implementation.

class CVTProgressBarPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTProgressBarPropPage)
	DECLARE_OLECREATE_EX(CVTProgressBarPropPage)

// Constructor
public:
	CVTProgressBarPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VTPROGRESSBAR };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
public:
	long m_lMin;
	long m_lMax;
	long m_lPos;
	BOOL m_bSmooth, m_bShowPercent;
};

