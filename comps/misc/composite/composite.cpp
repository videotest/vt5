// composite.cpp : Defines the initialization routines for the DLL.
//
#include "stdafx.h"
#include "misc_utils.h"
#include "Utils.h"

#include "composite.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

// CcompositeApp

BEGIN_MESSAGE_MAP(CCompositeApp, CWinApp)
END_MESSAGE_MAP()


// CcompositeApp construction

CCompositeApp::CCompositeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CcompositeApp object

CCompositeApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xAF2841C9, 0x4216, 0x463C, { 0xB7, 0xB4, 0x89, 0x1A, 0xC, 0x9E, 0xF2, 0x4F } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CcompositeApp initialization

BOOL CCompositeApp::InitInstance()
{
	AfxEnableControlContainer();
	VERIFY(LoadVTClass(this) == true);

	COleObjectFactory::RegisterAll();

	return TRUE;
}

// DllGetClassObject - Returns class factory

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}


// DllCanUnloadNow - Allows COM to unload DLL

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();
	AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid, _T("composite.tlb"));


	CActionRegistrator	r;
	r.RegisterActions( true );

	CCompRegistrator	rm(szPluginMeasurement);
	rm.RegisterComponent("Composite.CompositeMeasGroup");

	return S_OK;
}


// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	r;
	r.RegisterActions( false );

	CCompRegistrator	rm(szPluginMeasurement);
	rm.UnRegisterComponent("Composite.CompositeMeasGroup");


	AfxOleUnregisterTypeLib(_tlid);
	COleObjectFactory::UpdateRegistryAll(FALSE);
		
	return S_OK;
}
