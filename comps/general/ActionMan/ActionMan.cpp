// ActionMan.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ActionMan.h"
#include "common.h"
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
// CActionManApp

BEGIN_MESSAGE_MAP(CActionManApp, CWinApp)
	//{{AFX_MSG_MAP(CActionManApp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActionManApp construction

CActionManApp::CActionManApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CActionManApp::~CActionManApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CActionManApp object

CActionManApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CActionManApp initialization

// {8069CA46-02B2-11D3-A5A0-0000B493FF1B}
GUID	IID_ITypeLibID = 
{ 0x8069CA46, 0x02B2, 0x11D3, { 0xA5, 0xA0, 0x00, 0x00, 0xb4, 0x93, 0xff, 0x1b } };

BOOL CActionManApp::InitInstance()
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

	AfxOleUnregisterTypeLib( IID_ITypeLibID );

	return AfxDllCanUnloadNow();
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll();

	AfxOleRegisterTypeLib(AfxGetInstanceHandle(), IID_ITypeLibID, _T("ActionMan.tlb") );

	CCompRegistrator	r( szPluginScriptNamespace ), 
		rdw( szPluginDockableWindows ), 
		rd( szPluginDocumentsAggr );
	
	r.RegisterComponent( "ActionMan.ActionManager" );
	r.RegisterComponent( "ActionMan.ActionState" );
	rd.RegisterComponent( "ActionMan.UndoList" );
	rdw.RegisterComponent( "ActionMan.ActionLogWindow" );

	CActionRegistrator	ra;
	ra.RegisterActions();

	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll( FALSE  );

	CCompRegistrator	r( szPluginScriptNamespace ), 
		rdw( szPluginDockableWindows ), 
		rd( szPluginDocumentsAggr );

	r.UnRegisterComponent( "ActionMan.ActionManager" );
	r.UnRegisterComponent( "ActionMan.ActionState" );
	rd.UnRegisterComponent( "ActionMan.UndoList" );
	rdw.UnRegisterComponent( "ActionMan.ActionLogWindow" );

	CActionRegistrator	ra;
	ra.RegisterActions( false );

	return S_OK;
}


