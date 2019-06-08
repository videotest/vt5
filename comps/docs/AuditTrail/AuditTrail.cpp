// AuditTrail.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AuditTrail.h"
#include "Utils.h"
#include "MenuRegistrator.h"

#include "resource.h"

#ifdef _DEBUG
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
// CAuditTrailApp

BEGIN_MESSAGE_MAP(CAuditTrailApp, CWinApp)
	//{{AFX_MSG_MAP(CAuditTrailApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailApp construction

CAuditTrailApp::CAuditTrailApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAuditTrailApp object

CAuditTrailApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailApp initialization

BOOL CAuditTrailApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	VERIFY(LoadVTClass(this) == true);
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

	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	r(szPluginApplication);
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );

	r.RegisterComponent("AuditTrail.AuditTrailMan");
				
	rv.RegisterComponent( "AuditTrail.AuditTrailView" );
	rv.RegisterComponent( "AuditTrail.AuditTrailSingleView" );
	rv.RegisterComponent( "AuditTrail.AuditTrailTextView" );

	rf.RegisterComponent("AuditTrail.AuditTrailFile");

	//Register menu
	{
		//Menu name
		CString strATViewMenu = szAuditTrailViewUN;
		strATViewMenu.LoadString( IDS_ATVIEW_CONTEXT_MENU );
		CString strATTextViewMenu = szAuditTrailTextViewUN;
		strATTextViewMenu.LoadString( IDS_ATTEXTVIEW_CONTEXT_MENU );
		
		CMenuRegistrator rcm;
		rcm.RegisterMenu( szAuditTrailView, 0, (LPCSTR)strATViewMenu );
		rcm.RegisterMenu( szAuditTrailTextView, 0, (LPCSTR)strATTextViewMenu );
	}

	
	CActionRegistrator	ra;
	ra.RegisterActions();

	CDataRegistrator	da;
	da.RegisterDataObject();

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	strDocument = szDocumentImage;

	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	r(szPluginApplication);
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );

	r.UnRegisterComponent("AuditTrail.AuditTrailMan");

	rv.UnRegisterComponent( "AuditTrail.AuditTrailView" );
	rv.UnRegisterComponent( "AuditTrail.AuditTrailSingleView" );
	rv.UnRegisterComponent( "AuditTrail.AuditTrailTextView" );

	rf.UnRegisterComponent("AuditTrail.AuditTrailFile");


	//UnRegister menu
	{
		CMenuRegistrator rcm;
		rcm.UnRegisterMenu( szAuditTrailView, 0 );
		rcm.UnRegisterMenu( szAuditTrailTextView, 0 );
	}
	
	
	CActionRegistrator	ra;
	ra.RegisterActions( false );

	CDataRegistrator	da;
	da.RegisterDataObject( false );

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}

