// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define _STLP_NO_IOSTREAMS 1 
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define _USE_MATH_DEFINES
#define _WIN32_WINNT 0x501
#define _WTL_NO_CSTRING

#include <math.h>

// Windows Header Files:
#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#include <atlwin.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atldlgs.h>
#include <atlcrack.h>
#include <atlmisc.h>
//#include <atlgdix.h>
#include <atlsplit.h>
#include <atlscrl.h>
//#include <atlcoll.h>

#include <comdef.h>
#include <atlstr.h>

#include <gdiplus.h>
using namespace Gdiplus;

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

#include "misc_classes.h"

#include "class_utils.h"
#include "\vt5\awin\profiler.h"

// remove min max macros
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif 
// end remove __min __max macros

#include <valarray>
using std::valarray;
typedef valarray<double> dblArray;
#include <map>
using std::map;
#include <hash_map>
//using std::hash_map;
#include <vector>
using std::vector;
#include <set>
using std::set;
//#include <strstream>
//using std::ostringstream;
#include <algorithm>
using std::swap;
using std::find_if;
using std::count_if;
#include <numeric>
using std::accumulate;
#include <functional>
using std::bind2nd;
using std::greater_equal;
#include <limits>
//using std::numeric_limits<double>;
#include <atlsafe.h>

#include "\vt5\com_base\com_defs.h"
#include "\vt5\com_base\debug_misc.h"			
//#ifdef _DEBUG
//#define new DBG_NEW
//#endif

#undef BEGIN_MSG_MAP
#define	BEGIN_MSG_MAP(p) BEGIN_MSG_MAP_EX(p)

inline BOOL _InvalidateRect(
    IN HWND hWnd,
    IN CONST RECT *lpRect,
    IN BOOL bErase)
{
	if( hWnd == 0 )
	{
		trace( "\n-----------Invalid hwnd handle-----------");
	}
	else
		::InvalidateRect( hWnd, lpRect, bErase );
	return true;
}
