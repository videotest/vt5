// BatchInput.h : main header file for the BATCHINPUT DLL
//

#if !defined(AFX_BATCHINPUT_H__A5246AC5_51EB_11D6_BA8D_009027594905__INCLUDED_)
#define AFX_BATCHINPUT_H__A5246AC5_51EB_11D6_BA8D_009027594905__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CBatchInputApp
// See BatchInput.cpp for the implementation of this class
//

class CBatchInputApp : public CWinApp
{
public:
	CBatchInputApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBatchInputApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CBatchInputApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BATCHINPUT_H__A5246AC5_51EB_11D6_BA8D_009027594905__INCLUDED_)
