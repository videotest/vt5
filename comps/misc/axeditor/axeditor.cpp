// axeditor.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "axeditor.h"
#include "PropPage.h"
#include "Utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment( lib, "EditorView.lib" )

BEGIN_MESSAGE_MAP(CAxeditorApp, CWinApp)
	//{{AFX_MSG_MAP(CAxeditorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAxeditorApp construction

CAxeditorApp::CAxeditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAxeditorApp object

CAxeditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAxeditorApp initialization

BOOL CAxeditorApp::InitInstance()
{
	::AfxEnableControlContainer();
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

	CString	strDocument = szDocumentScript;
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	rv.RegisterComponent( "AxEditor.AxView" );

	CCompRegistrator	rp( szPluginPropertyPage );
	rp.RegisterComponent( szAXInsertPropPageProgID_4Reg );

	return S_OK;
}


// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ra;
	ra.RegisterActions( false );

	CString	strDocument = szDocumentScript;
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	rv.UnRegisterComponent( "AxEditor.AxView" );

	CCompRegistrator	rp( szPluginPropertyPage );
	rp.RegisterComponent( szAXInsertPropPageProgID_4Reg );

	COleObjectFactory::UpdateRegistryAll( false );
	return S_OK;
}
