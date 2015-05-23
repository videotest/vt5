// Metodics.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Metodics.h"
#include "Method.h"
#include "MenuRegistrator.h"
#include "TreeFilter.h"

#include "\vt5\AWIN\misc_dbg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "\vt5\awin\misc_module.h"
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

// CMetodicsApp

BEGIN_MESSAGE_MAP(CMetodicsApp, CWinApp)
END_MESSAGE_MAP()


// CMetodicsApp construction

CMetodicsApp::CMetodicsApp()
{
}


// The one and only CMetodicsApp object

CMetodicsApp theApp;


// CMetodicsApp initialization

BOOL CMetodicsApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);

	COleObjectFactory::RegisterAll();
	
	::AfxEnableControlContainer();

	static module	_module( 0 );
	_module.init( (HINSTANCE)m_hInstance );
	_trace_file_clear( LOG_FILE );

	//TestMethod(); //!!! debug

	return CWinApp::InitInstance();
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

	CActionRegistrator	ar;
	ar.RegisterActions( true );

	CCompRegistrator	r( szPluginScriptNamespace );
	r.RegisterComponent( "Metodics.MethodMan" );

	CCompRegistrator	rdw( szPluginDockableWindows );
	rdw.RegisterComponent( "Metodics.MetodicsTab" );
	rdw.RegisterComponent( "Metodics.MethodPanel" );

	CMenuRegistrator rcm;
	rcm.RegisterMenu( szMetodicsTab, 0, szMetodicsTabUN );
	rcm.RegisterMenu( szMetodicsTabAction, 0, szMetodicsTabActionUN );
	rcm.RegisterMenu( szMethodPanelMenu, 0, szMethodPanelMenuUI );	// [vanek] : method panel's menu - 19.10.2004


	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ar;
	ar.RegisterActions( false );

	CCompRegistrator	rdw( szPluginDockableWindows );
	rdw.UnRegisterComponent( "Metodics.MetodicsTab" );
	rdw.UnRegisterComponent( "Metodics.MethodPanel" );

	CMenuRegistrator rcm;
	rcm.UnRegisterMenu( szMetodicsTab, szMetodicsTabUN );
	rcm.UnRegisterMenu( szMetodicsTabAction, szMetodicsTabActionUN );
	rcm.UnRegisterMenu( szMethodPanelMenu, szMethodPanelMenuUI );	// [vanek] : method panel's menu - 19.10.2004

	COleObjectFactory::UpdateRegistryAll( FALSE );
	return S_OK;
}


