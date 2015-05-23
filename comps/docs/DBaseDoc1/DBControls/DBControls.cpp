// DBControls.cpp : Implementation of CDBControlsApp and DLL registration.

#include "stdafx.h"
#include "DBControls.h"
#include "Utils.h"


CDBControlsApp NEAR theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0x59213681, 0x18d6, 0x46b4, { 0xaf, 0x62, 0xe7, 0x37, 0xd7, 0x24, 0x10, 0xbf } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

/////////////////////////////////////////////////////////////////////////////
bool GetDBaseDocument( sptrIDBaseDocument& spIDBDoc )
{
	ASSERT(spIDBDoc == NULL );

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return false;
	
	IUnknown* punk = NULL;
	sptrA->GetActiveDocument( &punk );
	if( punk == NULL )
		return false;

	spIDBDoc = punk;

	punk->Release();
	
	if( spIDBDoc != NULL )
		return true;

	return false;
}

CIntIniValue g_DoNotActivateReadonly("DBControls", "DoNotActivateReadonly", FALSE);


////////////////////////////////////////////////////////////////////////////
// CDBControlsApp::InitInstance - DLL initialization

BOOL CDBControlsApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	AfxEnableControlContainer();
	VERIFY(LoadVTClass(this) == true);


	if (bInit)
	{
		// TODO: Add your own module initialization code here.
	}

	return bInit;
}


////////////////////////////////////////////////////////////////////////////
// CDBControlsApp::ExitInstance - DLL termination

int CDBControlsApp::ExitInstance()
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
