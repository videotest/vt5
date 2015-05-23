#if !defined(AFX_DBCONTROLS_H__43AFB885_1DBF_4F8B_885E_D27B15AE0FD2__INCLUDED_)
#define AFX_DBCONTROLS_H__43AFB885_1DBF_4F8B_885E_D27B15AE0FD2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// DBControls.h : main header file for DBCONTROLS.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols
#include "misc_utils.h"

bool GetDBaseDocument( sptrIDBaseDocument& spIDBDoc );
extern CIntIniValue g_DoNotActivateReadonly;

/////////////////////////////////////////////////////////////////////////////
// CDBControlsApp : See DBControls.cpp for implementation.

class CDBControlsApp : public COleControlModule
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

#endif // !defined(AFX_DBCONTROLS_H__43AFB885_1DBF_4F8B_885E_D27B15AE0FD2__INCLUDED)
