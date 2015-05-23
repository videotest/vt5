// zoom.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "zoom.h"
#include "Utils.h"

#include "PropPage.h"
#include "\vt5\awin\misc_module.h"

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
// CZoomApp

BEGIN_MESSAGE_MAP(CZoomApp, CWinApp)
	//{{AFX_MSG_MAP(CZoomApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CZoomApp construction

CZoomApp::CZoomApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CZoomApp::~CZoomApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CZoomApp object

CZoomApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CZoomApp initialization

BOOL CZoomApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.	
	VERIFY(LoadVTClass(this) == true);
	COleObjectFactory::RegisterAll();
	AfxEnableControlContainer();

	static module	_module( 0 );
	_module.init( (HINSTANCE)m_hInstance );

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

	CActionRegistrator	r;
	r.RegisterActions();

	CCompRegistrator	rpp(szPluginPropertyPage);
	rpp.RegisterComponent( szZoomNavigatorPropPageProgID_4Reg );


	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ra;
	ra.RegisterActions( false );		
	

	CCompRegistrator	rpp(szPluginPropertyPage);
	rpp.UnRegisterComponent( szZoomNavigatorPropPageProgID_4Reg );

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}


