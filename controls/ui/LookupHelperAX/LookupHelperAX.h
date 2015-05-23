#if !defined(AFX_LOOKUPHELPERAX_H__1AFF653C_79B6_4346_996E_8F6392749FD7__INCLUDED_)
#define AFX_LOOKUPHELPERAX_H__1AFF653C_79B6_4346_996E_8F6392749FD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// LookupHelperAX.h : main header file for LOOKUPHELPERAX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXApp : See LookupHelperAX.cpp for implementation.

class CLookupHelperAXApp : public COleControlModule
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

#endif // !defined(AFX_LOOKUPHELPERAX_H__1AFF653C_79B6_4346_996E_8F6392749FD7__INCLUDED)
