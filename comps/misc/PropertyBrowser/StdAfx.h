// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#if !defined(AFX_STDAFX_H__2F1431FA_D876_11D3_A095_0000B493A187__INCLUDED_)
#define AFX_STDAFX_H__2F1431FA_D876_11D3_A095_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0500
#endif
#define _ATL_APARTMENT_THREADED


#include <afxwin.h>

#include <atlbase.h>
#ifdef _ATL_STATIC_REGISTRY
#include <statreg.h>
#endif
//You may derive a class from CComModule and use it if you want to override
//something, but do not change the name of _Module
extern CComModule _Module;
#include <atlcom.h>
#include <atlctl.h>
#include <ocidl.h>	// Added by ClassView

#include "defs.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__2F1431FA_D876_11D3_A095_0000B493A187__INCLUDED)
