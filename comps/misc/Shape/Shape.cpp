// Shape.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"

#include "misc_utils.h"
#include "Utils.h"
#include "Shape.h"
#include "bayesianclassifier.h"
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

// CShapeApp

BEGIN_MESSAGE_MAP(CShapeApp, CWinApp)
END_MESSAGE_MAP()


// CShapeApp construction

CShapeApp::CShapeApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CShapeApp object

CShapeApp theApp;

const GUID CDECL BASED_CODE _tlid =
		{ 0xA4BBDE06, 0x288, 0x4878, { 0x93, 0xE9, 0x3F, 0xC4, 0xE6, 0x14, 0x60, 0x61 } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;


// CShapeApp initialization

BOOL CShapeApp::InitInstance()
{
	CWinApp::InitInstance();
	VERIFY(LoadVTClass(this) == true);
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
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

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll())
		return SELFREG_E_CLASS;
	
	CActionRegistrator	r;
	r.RegisterActions( true );

	CCompRegistrator	rm(szPluginMeasurement);
	rm.RegisterComponent("Shape.ShapeMeasureGroup");
	CCompRegistrator cr(szPluginClasses);
	cr.RegisterComponent(szBayesianClassifier);
	return S_OK;
}


// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	r;
	r.RegisterActions( false );

	CCompRegistrator	rm(szPluginMeasurement);
	rm.UnRegisterComponent("Shape.ShapeMeasGroup");

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return SELFREG_E_TYPELIB;

	if (!COleObjectFactory::UpdateRegistryAll(FALSE))
		return SELFREG_E_CLASS;

	return S_OK;
}
