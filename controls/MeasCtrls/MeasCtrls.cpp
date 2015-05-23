// MeasCtrls.cpp : Implementation of CMeasCtrlsApp and DLL registration.

#include "stdafx.h"
#include "MeasCtrls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CMeasCtrlsApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x40075923, 0x4631, 0x4A92, { 0xA6, 0xBE, 0x6F, 0xA8, 0x27, 0x44, 0x8, 0x7A } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CMeasCtrlsApp::InitInstance - DLL initialization

BOOL CMeasCtrlsApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}



// CMeasCtrlsApp::ExitInstance - DLL termination

int CMeasCtrlsApp::ExitInstance()
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
