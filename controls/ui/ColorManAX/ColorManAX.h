#if !defined(AFX_COLORMANAX_H__5DB73BBC_F590_11D3_A0BC_0000B493A187__INCLUDED_)
#define AFX_COLORMANAX_H__5DB73BBC_F590_11D3_A0BC_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ColorManAX.h : main header file for COLORMANAX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CColorManAXApp : See ColorManAX.cpp for implementation.

class CColorManAXApp : public COleControlModule
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

#endif // !defined(AFX_COLORMANAX_H__5DB73BBC_F590_11D3_A0BC_0000B493A187__INCLUDED)
