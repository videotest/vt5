// VTTable.cpp : Implementation of CVTTableApp and DLL registration.

#include "stdafx.h"
#include "VTTable.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CVTTableApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xCEF8513, 0x89B5, 0x486E, { 0x99, 0xB7, 0x35, 0xBC, 0x5E, 0x76, 0x61, 0x60 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CVTTableApp::InitInstance - DLL initialization

BOOL CVTTableApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CVTTableApp::ExitInstance - DLL termination

int CVTTableApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}



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
