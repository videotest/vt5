// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <stdlib.h>
// TODO: reference additional headers your program requires here

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\type_defs.h"

//debug support
#include "\vt5\com_base\com_defs.h"
#include "\vt5\com_base\debug_misc.h"			
//#ifdef _DEBUG
//#define new DBG_NEW
//#endif

#include "misc_classes.h"

// TODO: reference additional headers your program requires here
#include "objbase.h"
#include "comdef.h"

//common2
#include "com_main.h"
#include "com_unknown.h"
