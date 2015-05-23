// ChromosEx.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ChromosEx.h"

#include "MenuRegistrator.h"
#include "Utils.h"

#include "consts.h"


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
// CChromosExApp

BEGIN_MESSAGE_MAP(CChromosExApp, CWinApp)
	//{{AFX_MSG_MAP(CChromosExApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChromosExApp construction

#include "\vt5\awin\profiler.h"
CChromosExApp::CChromosExApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CChromosExApp object

CChromosExApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChromosExApp initialization

#include "\vt5\awin\misc_module.h"
BOOL CChromosExApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.	
	VERIFY(LoadVTClass(this) == true);

	COleObjectFactory::RegisterAll();

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

	CString	strDocument = szDocumentImage;
	
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	rv.RegisterComponent( szCarioViewProgID );
	rv.RegisterComponent( szCarioAnalizeViewProgID );

	CMenuRegistrator rcm;
	rcm.RegisterMenu( szCarioView, 0, szCarioViewMenuUN );
	rcm.RegisterMenu( szCarioAnalizeView, 0, szCarioAnalizeViewMenuUN );

	rcm.RegisterMenu( szCarioObjectView, szTypeObject, szCarioObjectViewMenuUN );
	rcm.RegisterMenu( szCarioObjectAnalizeView, szTypeObject, szCarioObjectAnalizeViewMenuUN );

	CActionRegistrator	ra;
	ra.RegisterActions();


	CDataRegistrator	rd;
	rd.RegisterDataObject( true );


	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	strDocument = szDocumentImage;

	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	rv.UnRegisterComponent( szCarioViewProgID );
	rv.UnRegisterComponent( szCarioAnalizeViewProgID );

	CMenuRegistrator rcm;
	rcm.UnRegisterMenu( szCarioView, 0 );
	rcm.UnRegisterMenu( szCarioAnalizeView, 0 );

	CActionRegistrator	ra;
	ra.RegisterActions( FALSE );

	CDataRegistrator	rd;
	rd.RegisterDataObject( false );


	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}

