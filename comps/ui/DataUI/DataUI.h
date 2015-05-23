// DataUI.h : main header file for the DATAUI DLL
//

#if !defined(AFX_DATAUI_H__07B3B6EB_15FA_4F88_9958_608A8E01E683__INCLUDED_)
#define AFX_DATAUI_H__07B3B6EB_15FA_4F88_9958_608A8E01E683__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDataUIApp
// See DataUI.cpp for the implementation of this class
//

class CDataUIApp : public CWinApp
{
public:
	CDataUIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataUIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDataUIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAUI_H__07B3B6EB_15FA_4F88_9958_608A8E01E683__INCLUDED_)
