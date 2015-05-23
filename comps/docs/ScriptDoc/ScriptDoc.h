// ScriptDoc.h : main header file for the SCRIPTDOC DLL
//

#if !defined(AFX_SCRIPTDOC_H__92FDEF70_0C1A_11D3_A5B6_0000B493FF1B__INCLUDED_)
#define AFX_SCRIPTDOC_H__92FDEF70_0C1A_11D3_A5B6_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CScriptDocApp
// See ScriptDoc.cpp for the implementation of this class
//

class CScriptDocApp : public CWinApp
{
public:
	CScriptDocApp();
	~CScriptDocApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptDocApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CScriptDocApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#define szScriptViewProgID "ScriptDoc.ScriptView";

/////////////////////////////////////////////////////////////////////////////
extern CScriptDocApp	theApp;
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_SCRIPTDOC_H__92FDEF70_0C1A_11D3_A5B6_0000B493FF1B__INCLUDED_)
