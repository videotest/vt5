// forensic.h : main header file for the FORENSIC DLL
//

#if !defined(AFX_FORENSIC_H__46A71596_6AD0_4CFA_AA86_C26F03C33FFF__INCLUDED_)
#define AFX_FORENSIC_H__46A71596_6AD0_4CFA_AA86_C26F03C33FFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CForensicApp
// See forensic.cpp for the implementation of this class
//

class CForensicApp : public CWinApp
{
public:
	CForensicApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CForensicApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CForensicApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORENSIC_H__46A71596_6AD0_4CFA_AA86_C26F03C33FFF__INCLUDED_)
