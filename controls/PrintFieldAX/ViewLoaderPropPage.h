#pragma once

// ViewLoaderPropPage.h : Declaration of the CViewLoaderPropPage property page class.


// CViewLoaderPropPage : See ViewLoaderPropPage.cpp.cpp for implementation.

class CViewLoaderPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CViewLoaderPropPage)
	DECLARE_OLECREATE_EX(CViewLoaderPropPage)

// Constructor
public:
	CViewLoaderPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_VIEWLOADER };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
public:
	short m_tAlgorithm;
	virtual BOOL OnInitDialog();
};

