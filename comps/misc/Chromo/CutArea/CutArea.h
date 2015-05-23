// CutArea.h : main header file for the CUTAREA DLL
//

#if !defined(AFX_CUTAREA_H__7F640A47_2BA1_11D6_BA8D_0002B38ACE2C__INCLUDED_)
#define AFX_CUTAREA_H__7F640A47_2BA1_11D6_BA8D_0002B38ACE2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCutAreaApp
// See CutArea.cpp for the implementation of this class
//

class CCutAreaApp : public CWinApp
{
public:
	CCutAreaApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCutAreaApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CCutAreaApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CUTAREA_H__7F640A47_2BA1_11D6_BA8D_0002B38ACE2C__INCLUDED_)
