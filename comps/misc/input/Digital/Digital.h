// Digital.h : main header file for the DIGITAL DLL
//

#if !defined(AFX_DIGITAL_H__BB893CE5_21CE_11D4_A98A_FC9799051A4B__INCLUDED_)
#define AFX_DIGITAL_H__BB893CE5_21CE_11D4_A98A_FC9799051A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDigitalApp
// See Digital.cpp for the implementation of this class
//

class CDigitalApp : public CWinApp
{
public:
	CDigitalApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDigitalApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDigitalApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DIGITAL_H__BB893CE5_21CE_11D4_A98A_FC9799051A4B__INCLUDED_)
