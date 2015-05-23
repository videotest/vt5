#if !defined(AFX_CONTEXTAX_H__CEC45342_9FE0_4B18_B09C_1879D4E5BD20__INCLUDED_)
#define AFX_CONTEXTAX_H__CEC45342_9FE0_4B18_B09C_1879D4E5BD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContextAX.h : main header file for CONTEXTAX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CContextAXApp : See ContextAX.cpp for implementation.

class CContextAXApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTAX_H__CEC45342_9FE0_4B18_B09C_1879D4E5BD20__INCLUDED)
