// ViewAXStatUIPropPage.h: interface for the CViewAXCGHPropPage class.
//
//////////////////////////////////////////////////////////////////////
#pragma once
#include "resource.h"

class CViewAXStatUIPropPage : public COlePropertyPage  
{
	bool m_bFilled;
	DECLARE_DYNCREATE(CViewAXStatUIPropPage)
	GUARD_DECLARE_OLECREATE_CTRL(CViewAXStatUIPropPage)
public:
	//{{AFX_DATA(CViewAXStatUIPropPage)
	enum { IDD = IDD_PROPPAGE_STATUI };
	//}}AFX_DATA

	CViewAXStatUIPropPage();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
	sptrIViewCtrl _get_viewAX_control();

protected:
	//{{AFX_MSG(CViewAXStatUIPropPage)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};