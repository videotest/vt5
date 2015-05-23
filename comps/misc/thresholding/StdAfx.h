// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C7ABA9AB_B8C5_4794_A9D8_B25EE7632640__INCLUDED_)
#define AFX_STDAFX_H__C7ABA9AB_B8C5_4794_A9D8_B25EE7632640__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "objbase.h"

#define __IActiveScriptParse_INTERFACE_DEFINED__
#include <ActivScp.h>
#include <comdef.h>
#include <comdefsp.h>

//common2
#include "com_main.h"
#include "com_unknown.h"
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C7ABA9AB_B8C5_4794_A9D8_B25EE7632640__INCLUDED_)
