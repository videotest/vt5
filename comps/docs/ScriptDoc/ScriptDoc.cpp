// ScriptDoc.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ScriptDoc.h"
#include "Utils.h"

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
// CScriptDocApp

BEGIN_MESSAGE_MAP(CScriptDocApp, CWinApp)
	//{{AFX_MSG_MAP(CScriptDocApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CScriptDocApp construction

CScriptDocApp::CScriptDocApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CScriptDocApp::~CScriptDocApp()
{
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CScriptDocApp object

CScriptDocApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CScriptDocApp initialization

BOOL CScriptDocApp::InitInstance()
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

	CString	strDocument = szDocumentScript;

	CCompRegistrator	rt( szPluginDocs );
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	rd( strDocument );

	rt.RegisterComponent( "ScriptDoc.Template" );
//	rf.RegisterComponent( "ScriptDoc.ConfigFileFilter" );
	rf.RegisterComponent( "ScriptDoc.ScriptFileFilter" );
	rd.RegisterComponent( "ScriptDoc.ScriptDocument" );
	rv.RegisterComponent( "ScriptDoc.ScriptView" );

	CActionRegistrator	ra;
	CDataRegistrator	da;
	ra.RegisterActions();
	da.RegisterDataObject();


	CFileRegistrator fr;
	fr.Register( ".config", "VT5 Config Document", 0, 0/*AfxGetApp()->m_hInstance*/ );


	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll( FALSE  );

	CString	strDocument = szDocumentScript;

	CCompRegistrator	rt( szPluginDocs );
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	rd( strDocument );

	rt.UnRegisterComponent( "ScriptDoc.Template" );
//	rf.UnRegisterComponent( "ScriptDoc.ConfigFileFilter" );
	rf.UnRegisterComponent( "ScriptDoc.ScriptFileFilter" );
	rd.UnRegisterComponent( "ScriptDoc.ScriptDocument" );
	rv.UnRegisterComponent( "ScriptDoc.ScriptView" );

	CActionRegistrator	ra;
	CDataRegistrator	da;
	ra.RegisterActions( false );
	da.RegisterDataObject( false );

	return S_OK;
}

