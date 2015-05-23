#if !defined(AFX_VTSLIDERPPG_H__FAD700BB_9570_484F_A977_7C669B8DA2DF__INCLUDED_)
#define AFX_VTSLIDERPPG_H__FAD700BB_9570_484F_A977_7C669B8DA2DF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTSliderPpg.h : Declaration of the CVTSliderPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CVTSliderPropPage : See VTSliderPpg.cpp.cpp for implementation.

class CVTSliderPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CVTSliderPropPage)
	DECLARE_OLECREATE_EX(CVTSliderPropPage)

// Constructor
public:
	CVTSliderPropPage();

// Dialog Data
	//{{AFX_DATA(CVTSliderPropPage)
	enum { IDD = IDD_PROPPAGE_VTSLIDER };
	BOOL	m_bCycled;
	BOOL	m_bShowNum;
	BOOL	m_bTick;
	CString	m_strHighKeyValue;
	CString	m_bLowKeyValue;
	long	m_nMax;
	long	m_nMin;
	CString	m_strName;
	BOOL	m_bAutoUpdate;
	short	m_nTickFreq;
	BOOL	m_bSingleSlide;
	BOOL	m_bUseSystemFont;
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CVTSliderPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSLIDERPPG_H__FAD700BB_9570_484F_A977_7C669B8DA2DF__INCLUDED)
