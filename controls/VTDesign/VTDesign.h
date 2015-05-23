#if !defined(AFX_VTDESIGN_H__BC127B29_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_VTDESIGN_H__BC127B29_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// VTDesign.h : main header file for VTDESIGN.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CVTDesignApp : See VTDesign.cpp for implementation.


void InvalidateCtrl( COleControl* pCtrl,LPOLEINPLACESITEWINDOWLESS pInPlaceSiteWndless, bool bEraseParentBk = false );


class CVTDesignApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

BOOL _IsWindowVisible( CWnd* pWnd );

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTDESIGN_H__BC127B29_026C_11D4_8124_0000E8DF68C3__INCLUDED)
