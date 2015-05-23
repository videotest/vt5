// StdSplitter.h : main header file for the STDSPLITTER DLL
//

#if !defined(AFX_STDSPLITTER_H__AC8FC6CB_27A1_11D3_A5DC_0000B493FF1B__INCLUDED_)
#define AFX_STDSPLITTER_H__AC8FC6CB_27A1_11D3_A5DC_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CStdSplitterApp
// See StdSplitter.cpp for the implementation of this class
//

class CStdSplitterApp : public CWinApp
{
public:
	CStdSplitterApp();
	~CStdSplitterApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStdSplitterApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CStdSplitterApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDSPLITTER_H__AC8FC6CB_27A1_11D3_A5DC_0000B493FF1B__INCLUDED_)
