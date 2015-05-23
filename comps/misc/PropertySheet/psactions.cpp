#include "stdafx.h"
#include "psactions.h"
#include "resource.h"

#include "PropertySheetBase.h"


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionShowOptions, CCmdTargetEx);

// olecreate

// {91E49E14-C8C2-4fb9-BDE3-498F4C33A856}
GUARD_IMPLEMENT_OLECREATE(CActionShowOptions, "PropertySheet.ShowOptions",
0x91e49e14, 0xc8c2, 0x4fb9, 0xbd, 0xe3, 0x49, 0x8f, 0x4c, 0x33, 0xa8, 0x56);

// guidinfo

// {DDB9630B-6151-4c7b-9B28-FC1EBCC3C3F2}
static const GUID guidShowOptions =
{ 0xddb9630b, 0x6151, 0x4c7b, { 0x9b, 0x28, 0xfc, 0x1e, 0xbc, 0xc3, 0xc3, 0xf2 } };



//[ag]6. action info

ACTION_INFO_FULL(CActionShowOptions, ID_ACTION_OPTIONS, -1, -1, guidShowOptions);

//[ag]7. targets


//[ag]8. arguments


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionShowOptions implementation
CActionShowOptions::CActionShowOptions()
{
}

CActionShowOptions::~CActionShowOptions()
{
}

bool CActionShowOptions::Invoke()
{
	CPropertySheetBase	sheet( szPluginObjectPage );
	sheet.DoModal();
	return true;
}

//extended UI
bool CActionShowOptions::IsAvaible()
{
	return true;
}

bool CActionShowOptions::IsChecked()
{
	return false;
}


