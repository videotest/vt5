#if !defined(AFX_PRINTFIELDAX_H__5ED0372C_10C0_4D28_8745_C545EDBF7E9F__INCLUDED_)
#define AFX_PRINTFIELDAX_H__5ED0372C_10C0_4D28_8745_C545EDBF7E9F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PrintFieldAX.h : main header file for PRINTFIELDAX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXApp : See PrintFieldAX.cpp for implementation.

class CPrintFieldAXApp : public COleControlModule
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

#endif // !defined(AFX_PRINTFIELDAX_H__5ED0372C_10C0_4D28_8745_C545EDBF7E9F__INCLUDED)
