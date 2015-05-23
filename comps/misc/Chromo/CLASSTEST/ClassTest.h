// ClassTest.h : main header file for the CLASSTEST application
//

#if !defined(AFX_CLASSTEST_H__98370A35_E651_4244_9782_6D099CDA89C3__INCLUDED_)
#define AFX_CLASSTEST_H__98370A35_E651_4244_9782_6D099CDA89C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClassTestApp:
// See ClassTest.cpp for the implementation of this class
//

class CClassTestApp : public CWinApp
{
public:
	CClassTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClassTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClassTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	CString m_sProfile;
	
};

CString _GetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszDefault = NULL);
void    _SetProfileString(LPCTSTR lpszSection, LPCTSTR lpszEntry, LPCTSTR lpszString);

inline CClassTestApp *_GetApp() {return (CClassTestApp *)AfxGetApp();};
CString _AskForPath(BOOL bDir = TRUE, LPCTSTR pszInitPath = NULL, LPCTSTR pszDefExt = NULL, LPCTSTR pszFilters = NULL); // Ask user for path
void _MakeScriptAddLearn(CStdioFile &file, LPCTSTR pszTeachFilePath, CListBox *pLog);
void _MakeScriptTest(CStdioFile &file, LPCTSTR pszTestPath, LPCTSTR pszCompPath, CListBox *pLog);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLASSTEST_H__98370A35_E651_4244_9782_6D099CDA89C3__INCLUDED_)
