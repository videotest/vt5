#if !defined(AFX_VTCONTROLS_H__6DC21AC2_318B_4D4F_B5A0_1DA27B340679__INCLUDED_)
#define AFX_VTCONTROLS_H__6DC21AC2_318B_4D4F_B5A0_1DA27B340679__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTControls.h : main header file for VTCONTROLS.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVTControlsApp : See VTControls.cpp for implementation.

bool GetFontFromDispatch( COleControl *pOleCtrl, CFont& font, BOOL bUseSystemDefault );

class CVTControlsApp : public COleControlModule
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

#endif // !defined(AFX_VTCONTROLS_H__6DC21AC2_318B_4D4F_B5A0_1DA27B340679__INCLUDED)
