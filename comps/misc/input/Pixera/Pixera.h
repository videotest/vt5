// Pixera.h : main header file for the PIXERA DLL
//

#if !defined(AFX_PIXERA_H__4E6A304B_1BA2_4AD2_A6E4_842A5147B90A__INCLUDED_)
#define AFX_PIXERA_H__4E6A304B_1BA2_4AD2_A6E4_842A5147B90A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPixeraApp
// See Pixera.cpp for the implementation of this class
//

class CPixeraApp : public CWinApp
{
public:
	CPixeraApp();
	BOOL m_bDiffProf;
	void InitProfile(LPCTSTR lpName);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPixeraApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual void WinHelpInternal(DWORD_PTR dwData, UINT nCmd = HELP_CONTEXT);
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPixeraApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIXERA_H__4E6A304B_1BA2_4AD2_A6E4_842A5147B90A__INCLUDED_)
