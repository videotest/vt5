// axform.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "axform.h"
#include "Utils.h"

// {BBC08690-FE5A-46c4-B82D-E4ECDA7BD57F}
GUID	IID_ITypeLibID = 
{ 0xbbc08690, 0xfe5a, 0x46c4, { 0xb8, 0x2d, 0xe4, 0xec, 0xda, 0x7b, 0xd5, 0x7f } };



#ifdef _DEBUG
#pragma comment( lib, "EditorView_d.lib" )
#else
#pragma comment( lib, "EditorView.lib" )
#endif//


BEGIN_MESSAGE_MAP(CAxformApp, CWinApp)
	//{{AFX_MSG_MAP(CAxformApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CAxformApp::CAxformApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAxformApp object

CAxformApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAxformApp initialization

BOOL CAxformApp::InitInstance()
{
	AfxEnableControlContainer();
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

	AfxOleRegisterTypeLib(AfxGetInstanceHandle(), IID_ITypeLibID, _T("axform.tlb") );

	CActionRegistrator	ra;
	CDataRegistrator	rd;
	CCompRegistrator	r( szPluginScriptNamespace );
	CString	strDocument = szDocumentScript;
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );

	ra.RegisterActions();
	rd.RegisterDataObject();
	r.RegisterComponent( "axform.FormManager" );

	rf.RegisterComponent( "FileFilters.AXFileFilter" );
	rf.RegisterComponent( "FileFilters.ReportFileFilter" );	

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	
	CActionRegistrator	ra;
	CDataRegistrator	rd;
	CCompRegistrator	r( szPluginScriptNamespace );
	CString	strDocument = szDocumentScript;
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );

	r.UnRegisterComponent( "axform.FormManager" );

	ra.RegisterActions( false );
	rd.RegisterDataObject( false );

	rf.UnRegisterComponent( "FileFilters.AXFileFilter" );
	rf.UnRegisterComponent( "FileFilters.ReportFileFilter" );	

	AfxOleUnregisterTypeLib( IID_ITypeLibID );

	COleObjectFactory::UpdateRegistryAll( false );

	return S_OK;
}

