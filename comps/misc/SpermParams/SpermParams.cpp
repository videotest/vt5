// SpermParams.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "SpermParams.h"
#include "SpermMeasGroup.h"

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

// CSpermParamsApp

BEGIN_MESSAGE_MAP(CSpermParamsApp, CWinApp)
END_MESSAGE_MAP()


// CSpermParamsApp construction

CSpermParamsApp::CSpermParamsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CSpermParamsApp object

CSpermParamsApp theApp;

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
}

STDAPI DllCanUnloadNow(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	CActionRegistrator	ra;
	ra.RegisterActions();

	CCompRegistrator	rm(szPluginMeasurement);
	rm.RegisterComponent("SpermParams.SpermMeasGroup");

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCompRegistrator	rm(szPluginMeasurement);
	rm.UnRegisterComponent("SpermParams.SpermMeasGroup");

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}


// CSpermParamsApp initialization

BOOL CSpermParamsApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.	
	VERIFY(LoadVTClass(this) == true);

	CWinApp::InitInstance();
	COleObjectFactory::RegisterAll();

	return TRUE;
}
