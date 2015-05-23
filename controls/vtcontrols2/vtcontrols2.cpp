// vtcontrols2.cpp : Implementation of CVtcontrols2App and DLL registration.

#include "stdafx.h"
#include "vtcontrols2.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CVtcontrols2App NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xd4975fba, 0x118a, 0x4368, { 0xb9, 0x41, 0x8a, 0x44, 0x1a, 0xc, 0xcf, 0xa4 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 4;


////////////////////////////////////////////////////////////////////////////
// CVtcontrols2App::InitInstance - DLL initialization

BOOL CVtcontrols2App::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CVtcontrols2App::ExitInstance - DLL termination

int CVtcontrols2App::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}


/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);

	return NOERROR;
}
