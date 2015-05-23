#if !defined(AFX_VTCONTROLS2_H__3F727427_E61C_41C5_ADBC_5C631F42F70F__INCLUDED_)
#define AFX_VTCONTROLS2_H__3F727427_E61C_41C5_ADBC_5C631F42F70F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// vtcontrols2.h : main header file for VTCONTROLS2.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVtcontrols2App : See vtcontrols2.cpp for implementation.

class CVtcontrols2App : public COleControlModule
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

#endif // !defined(AFX_VTCONTROLS2_H__3F727427_E61C_41C5_ADBC_5C631F42F70F__INCLUDED)
