// Editor.h : main header file for the EDITOR DLL
//

#if !defined(AFX_EDITOR_H__CC948DE7_80B6_11D3_A513_0000B493A187__INCLUDED_)
#define AFX_EDITOR_H__CC948DE7_80B6_11D3_A513_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CEditorApp
// See Editor.cpp for the implementation of this class
//
#define szSelectBinaryPropPageProgID "Editor.SelectBinaryPropPage"

class CEditorApp : public CWinApp
{
public:
	CEditorApp();
	~CEditorApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditorApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CEditorApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITOR_H__CC948DE7_80B6_11D3_A513_0000B493A187__INCLUDED_)
