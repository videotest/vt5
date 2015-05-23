#ifndef __utils2_h__
#define __utils2_h__

#include "defs.h"
#include "object5.h"
#include "data5.h"
#include "core5.h"
#include "misc5.h"
#include "measure5.h"

// If object has no attached named data, this functions creates named data
// and attaches it to object
//set double value to NamedData
std_dll void SetValue2(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue);
//set integer value to NamedData
std_dll void SetValue2(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue );
//set string value to NamedData
std_dll void SetValue2(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue );
//set any value to NamedData
std_dll void SetValue2(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const VARIANT varDefault );





#endif