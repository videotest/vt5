// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__7CDE6807_0AE7_43C6_8E46_F27229A9D164__INCLUDED_)
#define AFX_STDAFX_H__7CDE6807_0AE7_43C6_8E46_F27229A9D164__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define _STLP_NO_IOSTREAMS 1 
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <stdio.h>
#pragma warning(disable:4786)
#include <map>
#include <vector>
//namespace std
//{
//  using namespace stlport;
//}

#include <windows.h>
#include <atlcoll.h>
#include "objbase.h"
#define __IActiveScriptParse_INTERFACE_DEFINED__
#include <ActivScp.h>
#include <comdef.h>
#include "misc_new.h"
#include <math.h>

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__7CDE6807_0AE7_43C6_8E46_F27229A9D164__INCLUDED_)
