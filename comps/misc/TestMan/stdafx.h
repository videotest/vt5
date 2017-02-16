// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows NT 4 or later.
#define _WIN32_WINNT 0x0500	// Change this to the appropriate value to target Windows 2000 or later.
#endif						

#include <windows.h>
//#include <atlcoll.h>

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\type_defs.h"

// TODO: reference additional headers your program requires here
#include "objbase.h"

#define __IActiveScriptParse_INTERFACE_DEFINED__
#include <ActivScp.h>
#include <comdef.h>
#include <comdefsp.h>

//common2
#include "com_main.h"
#include "com_unknown.h"
#include <atlcoll.h>

//debug support
#include "\vt5\com_base\debug_misc.h"			
#ifdef _DEBUG
#define new DBG_NEW
#endif	