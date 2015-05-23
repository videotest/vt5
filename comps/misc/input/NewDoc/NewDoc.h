// NewDoc.h : main header file for the NEWDOC DLL
//

#if !defined(AFX_NEWDOC_H__3F27708B_C8EB_11D3_99F5_000000000000__INCLUDED_)
#define AFX_NEWDOC_H__3F27708B_C8EB_11D3_99F5_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "common.h"

/////////////////////////////////////////////////////////////////////////////
// CNewDocApp
// See NewDoc.cpp for the implementation of this class
//

class CNewDocApp : public CWinApp
{
public:
	CNewDocApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNewDocApp)
	public:
	virtual BOOL InitInstance();
	virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CNewDocApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWDOC_H__3F27708B_C8EB_11D3_99F5_000000000000__INCLUDED_)
