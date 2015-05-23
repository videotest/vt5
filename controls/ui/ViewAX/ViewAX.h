#if !defined(AFX_VIEWAX_H__24D8C026_AB3D_4E5B_A175_3C6540738DDC__INCLUDED_)
#define AFX_VIEWAX_H__24D8C026_AB3D_4E5B_A175_3C6540738DDC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ViewAX.h : main header file for VIEWAX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CViewAXApp : See ViewAX.cpp for implementation.

class CViewAXApp : public COleControlModule
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

#endif // !defined(AFX_VIEWAX_H__24D8C026_AB3D_4E5B_A175_3C6540738DDC__INCLUDED)
