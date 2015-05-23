#if !defined(AFX_STDAFX_H__BC127B27_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
#define AFX_STDAFX_H__BC127B27_026C_11D4_8124_0000E8DF68C3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define WINVER 0x0501
#define _WIN32_WINNT 0x501

// stdafx.h : include file for standard system include files,
//      or project specific include files that are used frequently,
//      but are changed infrequently

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxext.h>         // MFC extensions
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Comon Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

// Delete the two includes below if you do not wish to use the MFC
//  database classes
#include <afxdb.h>			// MFC database classes

//#define FOR_HOME_WORK

#ifndef FOR_HOME_WORK
#include "comdef.h"
#include "..\\..\\common\\defs.h"
#include "..\\..\\ifaces\\axint.h"
#include "..\\..\\common\\printconst.h"
#else
#include "int.h"
#endif //FOR_HOME_WORK

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__BC127B27_026C_11D4_8124_0000E8DF68C3__INCLUDED_)
