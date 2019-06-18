// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _STLP_NO_IOSTREAMS 1 
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <stdio.h>

#include <comdef.h>
#include <commdlg.h>
//common2
#include "data_main.h"

#include "nameconsts.h"

#include "core5.h"
#include "docview5.h"

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\type_defs.h"

#include "\vt5\AWIN\profiler.h"

// remove min max macros
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif 
// end remove _min _max macros

#include <limits>
#include <valarray>
typedef std::valarray<double> dblArray;
#include <map>
using std::map;
#include <vector>
using std::vector;
#include <memory>
using std::auto_ptr;
#include <exception>
using std::exception;
#include <algorithm>

#include <atlstr.h>
#include <stdlib.h>
#include "class_utils.h"

//#include <crtdbg.h>
//
//#include "\vt5\com_base\com_defs.h"
//#include "\vt5\com_base\debug_misc.h"			
//#ifdef _DEBUG
//#define new DBG_NEW
//#endif
#include <atlsafe.h>
#include <ATLComTime.h>
