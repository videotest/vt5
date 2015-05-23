#include "StdAfx.h"
#include "misc_utils.h"
#include "Ver30.h"


bool IsVersion3x()
{
	IUnknownPtr punk;
	HRESULT hr = CoCreateInstance(clsidVersion3x, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&punk);
	return SUCCEEDED(hr) && punk != 0;
/*	_bstr_t bstrV3x(szTypeVer3xObject);
	IUnknown *punk = CreateTypedObject(bstrV3x);
	bool bVersion3x = punk!=0;
	if (punk) punk->Release();
	return bVersion3x;*/
}
