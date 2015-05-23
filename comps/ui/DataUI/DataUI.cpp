// DataUI.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "DataUI.h"
#include "dataui_consts.h"
#include "MenuRegistrator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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

/////////////////////////////////////////////////////////////////////////////
// CDataUIApp

BEGIN_MESSAGE_MAP(CDataUIApp, CWinApp)
	//{{AFX_MSG_MAP(CDataUIApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataUIApp construction

CDataUIApp::CDataUIApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDataUIApp object

CDataUIApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDataUIApp initialization

BOOL CDataUIApp::InitInstance()
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

	CCompRegistrator	rs( szPluginScriptNamespace );
	rs.RegisterComponent( DATA_CONTEXT_CTRL_PROGID );

	CActionRegistrator	ar;
	ar.RegisterActions( true );

	CCompRegistrator	rdw( szPluginDockableWindows );
	rdw.RegisterComponent( "DataUI.ImageBar" );

	CMenuRegistrator rcm;
	rcm.RegisterMenu( szImageBarNameMenu, 0, szImageBarNameMenuUI );

	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCompRegistrator	rs( szPluginApplication );
	rs.UnRegisterComponent( DATA_CONTEXT_CTRL_PROGID );

	CActionRegistrator	ar;
	ar.RegisterActions( false );

	CCompRegistrator	rdw( szPluginDockableWindows );
	rdw.UnRegisterComponent( "DataUI.ImageBar" );

	CMenuRegistrator rcm;
	rcm.UnRegisterMenu( szImageBarNameMenu, szImageBarNameMenuUI );


	COleObjectFactory::UpdateRegistryAll( false );

	return S_OK;
}
