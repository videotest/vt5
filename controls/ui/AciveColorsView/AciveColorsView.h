#if !defined(AFX_ACIVECOLORSVIEW_H__0C1F6A91_9DB3_11D3_A536_0000B493A187__INCLUDED_)
#define AFX_ACIVECOLORSVIEW_H__0C1F6A91_9DB3_11D3_A536_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// AciveColorsView.h : main header file for ACIVECOLORSVIEW.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CAciveColorsViewApp : See AciveColorsView.cpp for implementation.

class CAciveColorsViewApp : public COleControlModule
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

#endif // !defined(AFX_ACIVECOLORSVIEW_H__0C1F6A91_9DB3_11D3_A536_0000B493A187__INCLUDED)
