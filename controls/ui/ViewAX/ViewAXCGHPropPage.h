// ViewAXCGHPropPage.h: interface for the CViewAXCGHPropPage class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"

class CViewAXCGHPropPage : public COlePropertyPage  
{
	bool m_bFilled;
	DECLARE_DYNCREATE(CViewAXCGHPropPage)
	GUARD_DECLARE_OLECREATE_CTRL(CViewAXCGHPropPage)
public:
	//{{AFX_DATA(CViewAXCGHPropPage)
	enum { IDD = IDD_PROPPAGE_CGH };
	BOOL	m_bActive;
	BOOL	m_bSelected;
	BOOL	m_bNegative;
	BOOL	m_bMono;
	//}}AFX_DATA

	CViewAXCGHPropPage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	sptrIViewCtrl _get_viewAX_control();

protected:
	//{{AFX_MSG(CViewAXCGHPropPage)
	afx_msg void OnRatio1();
	afx_msg void OnRatio2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
