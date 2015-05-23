// BCGToolbarManuBarButton.h: interface for the CBCGToolbarManuBarButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BCGTOOLBARMANUBARBUTTON_H__644FE23C_D1DB_11D1_A649_00A0C93A70EC__INCLUDED_)
#define AFX_BCGTOOLBARMANUBARBUTTON_H__644FE23C_D1DB_11D1_A649_00A0C93A70EC__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "BCGToolbarButton.h"

class CBCGToolbarMenuBarButton : public CBCGToolbarButton  
{
	DECLARE_SERIAL(CBCGToolbarMenuBarButton)

public:
	CBCGToolbarMenuBarButton();
	CBCGToolbarMenuBarButton(UINT uiID, int iImage, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE);
	virtual ~CBCGToolbarMenuBarButton();

protected:
	HMENU	m_hMenu;
};

#endif // !defined(AFX_BCGTOOLBARMANUBARBUTTON_H__644FE23C_D1DB_11D1_A649_00A0C93A70EC__INCLUDED_)
