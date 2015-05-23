// DataView.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DataView.h"
#include "Utils.h"
#include "proppage.h"
#include "MenuRegistrator.h"


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

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp

BEGIN_MESSAGE_MAP(CDataViewApp, CWinApp)
	//{{AFX_MSG_MAP(CDataViewApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp construction

CDataViewApp::CDataViewApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDataViewApp object

CDataViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDataViewApp initialization

BOOL CDataViewApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	VERIFY(LoadVTClass(this) == true);

	::AfxEnableControlContainer();

	COleObjectFactory::RegisterAll();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

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

	CString	strDocument = szDocumentImage;
	CCompRegistrator	rv(strDocument+"\\"+szPluginView);
	rv.RegisterComponent("DataView.ClassView");
	rv.RegisterComponent("DataView.MeasView");

	CMenuRegistrator	rm;
	rm.RegisterMenu( szStatisticMenu, 0, szStatisticMenuUN );
	rm.RegisterMenu( szMeasurementMenu, 0, szMeasurementMenuUN );

//	CCompRegistrator	rsns(szPluginScriptNamespace);
//	rsns.RegisterComponent("DataView.ClassView");
//	rsns.RegisterComponent("DataView.MeasView");

	CActionRegistrator	ar;
	ar.RegisterActions(true);


	CCompRegistrator	rpp(szPluginPropertyPage);
	rpp.RegisterComponent( szMeasurementPropPageProgID );


	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	strDocument = szDocumentImage;
	CCompRegistrator	rv(strDocument+"\\"+szPluginView);
	rv.UnRegisterComponent("DataView.ClassView");
	rv.UnRegisterComponent("DataView.MeasView");

//	CCompRegistrator	rsns(szPluginScriptNamespace);
//	rsns.UnRegisterComponent("DataView.ClassView");
//	rsns.UnRegisterComponent("DataView.MeasView");


	CActionRegistrator	ar;
	ar.RegisterActions(false);

	COleObjectFactory::UpdateRegistryAll(false);

	return S_OK;
}
