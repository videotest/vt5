#include "stdafx.h"
#include "common.h"
#include "afxpriv2.h"
#include "thumbnail.h"
#include "scriptdocint.h"
#include "nameconsts.h"
#include "aliaseint.h"

#include "compressint.h"
#include "string"
#include "editorint.h"

#include "StreamEx.h"
#include "StreamUtils.h"
#include "Utils2.h"

#include <ActivScp.h>
#include <ComDef.h>
#include "script_int.h"
#include "\vt5\AWIN\profiler.h"
#include "\vt5\AWIN\misc_string.h"
#include "\vt5\AWIN\misc_map.h"

void SetValue2(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, double fValue)
{
	_variant_t	var(fValue);
	::SetValue2(punkDoc, pszSection, pszEntry, var);
}

void SetValue2(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, long lValue )
{
	_variant_t	var( lValue );
	::SetValue2( punkDoc, pszSection, pszEntry, var );
}

void SetValue2(IUnknown *punkDoc, const char *pszSection, const char *pszEntry, const char *pszValue )
{
	_variant_t	var( pszValue );
	::SetValue2( punkDoc, pszSection, pszEntry, var );
}

void SetValue2(IUnknown *punk, const char *pszSection, const char *pszEntry, const VARIANT varDefault )
{
	INamedDataPtr sptrND(punk);
	if (sptrND == 0)
	{
		INamedDataObject2Ptr ptrNDO(punk);
		if (ptrNDO == 0) return;
		ptrNDO->InitAttachedData();
		sptrND = ptrNDO;
	}
	::SetValue(sptrND, pszSection, pszEntry, varDefault);
}

