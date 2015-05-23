#if !defined(AFX_HISTAX_H__06FDDE5E_F981_11D3_A0C1_0000B493A187__INCLUDED_)
#define AFX_HISTAX_H__06FDDE5E_F981_11D3_A0C1_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// HistAX.h : main header file for HISTAX.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CHistAXApp : See HistAX.cpp for implementation.

class CHistAXApp : public COleControlModule
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

#endif // !defined(AFX_HISTAX_H__06FDDE5E_F981_11D3_A0C1_0000B493A187__INCLUDED)
