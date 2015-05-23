// Fish.h : main header file for the FISH DLL
#pragma once

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CFishApp
// See Fish.cpp for the implementation of this class
//

class CFishApp : public CWinApp
{
public:
	CFishApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFishApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CFishApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};



/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
