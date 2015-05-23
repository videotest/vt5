#pragma once

// ChoiceGreekSymbolPropPage.h : Declaration of the CChoiceGreekSymbolPropPage property page class.


// CChoiceGreekSymbolPropPage : See ChoiceGreekSymbolPropPage.cpp.cpp for implementation.

class CChoiceGreekSymbolPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CChoiceGreekSymbolPropPage)
	DECLARE_OLECREATE_EX(CChoiceGreekSymbolPropPage)

// Constructor
public:
	CChoiceGreekSymbolPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_CHOICEGREEKSYMBOL };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};

