// LookupHelperAX.cpp : Implementation of CLookupHelperAXApp and DLL registration.

#include "stdafx.h"
#include "LookupHelperAX.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


CLookupHelperAXApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
	{ 0xF970DB7A, 0x64A1, 0x4da2, { 0x9a, 0x88, 0x1f, 0xae, 0xdd, 0x79, 0x00, 0x12 } };

const WORD _wVerMajor = 1;
const WORD _wVerMinor = 1;


////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXApp::InitInstance - DLL initialization

BOOL CLookupHelperAXApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CLookupHelperAXApp::ExitInstance - DLL termination

int CLookupHelperAXApp::ExitInstance()
{
	// TODO: Add your own module termination code here.

	return COleControlModule::ExitInstance();
}


STDAPI _DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}


/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid, _T("LookupHelperAX.tlb")))
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
