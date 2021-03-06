// fft.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "fft.h"
#include "Utils.h"

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
// CFftApp

BEGIN_MESSAGE_MAP(CFftApp, CWinApp)
	//{{AFX_MSG_MAP(CFftApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFftApp construction

CFftApp::CFftApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFftApp object

CFftApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFftApp initialization

BOOL CFftApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
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

	CActionRegistrator	ra;
	ra.RegisterActions();

	//CCompRegistrator	rp;
	CCompRegistrator	rc( szPluginColorCnv );
	//rp.RegisterComponent( "fft.FourierPage" );
	rc.RegisterComponent( "fft.ColorConvertorComplex" );


	return S_OK;
}


// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ra;
	ra.RegisterActions( false );

	//CCompRegistrator	rp;
	CCompRegistrator	rc( szPluginColorCnv );

	rc.UnRegisterComponent( "fft.ColorConvertorComplex" );
	//rp.UnRegisterComponent( "fft.FourierPage" );

	COleObjectFactory::UpdateRegistryAll( false );
	return S_OK;
}



