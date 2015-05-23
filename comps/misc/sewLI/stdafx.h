#pragma once

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
//#include "objbase.h"
#include <comdef.h>
#include <commdlg.h>

//common2
#include <comdef.h>
#include <commdlg.h>

//common2
#include "com_main.h"
#include "com_unknown.h"
#include "data_main.h"

#include "nameconsts.h"

#include "core5.h"
#include "window5.h"
#include "docview5.h"

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\type_defs.h"

#include "\vt5\common2\misc_classes.h"
#include "class_utils.h"

#include "\vt5\AWIN\profiler.h"
#include <map>