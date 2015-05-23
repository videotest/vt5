// IdioDBase.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "IdioDBase.h"
#include "idio_int.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "MenuRegistrator.h"
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

// CIdioDBaseApp

BEGIN_MESSAGE_MAP(CIdioDBaseApp, CWinApp)
END_MESSAGE_MAP()


// CIdioDBaseApp construction

CIdioDBaseApp::CIdioDBaseApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CIdioDBaseApp object

CIdioDBaseApp theApp;


// CIdioDBaseApp initialization

//#pragma comment(linker, "/EXPORT:DllCanUnloadNow=_DllCanUnloadNow@0,PRIVATE")
//#pragma comment(linker, "/EXPORT:DllGetClassObject=_DllGetClassObject@12,PRIVATE")
//#pragma comment(linker, "/EXPORT:DllRegisterServer=_DllRegisterServer@0,PRIVATE")
//#pragma comment(linker, "/EXPORT:DllUnregisterServer=_DllUnregisterServer@0,PRIVATE")

BOOL CIdioDBaseApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	COleObjectFactory::RegisterAll();

	return TRUE;
}

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

	CString	strDocument = szDocumentIdioDBase;

	CCompRegistrator	rt( szPluginDocs );
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	rd( strDocument );
						
	rt.RegisterComponent( szDocumentIdioDBase ".IdioDBDocTemplate" );
//	rf.RegisterComponent( szDocumentIdioDBase ".IdioFileFilter" );
	rd.RegisterComponent( szDocumentIdioDBase ".IdioDBDocument" );
	rv.RegisterComponent( szDocumentIdioDBase ".IdioDBView" );

	CActionRegistrator	ra;
	ra.RegisterActions();

	CDataRegistrator	da;
	da.RegisterDataObject();

	CMenuRegistrator rcm;
	rcm.RegisterMenu( szIdioDBView, 0, szIdioDBViewUN );
	rcm.RegisterMenu( szIdioCloneDBView, sz_idio_type, szIdiobjectUN );


	CFileRegistrator fr;
	fr.Register( ".dbi", "VT5 IdioDB Document", 0, AfxGetApp()->m_hInstance );
	::SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0 );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	strDocument = szDocumentIdioDBase;

	CCompRegistrator	rt( szPluginDocs );
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	rd( strDocument );

	CActionRegistrator	ra;
	ra.RegisterActions( false );

	rt.UnRegisterComponent( szDocumentIdioDBase ".IdioDBDocTemplate" );
//	rf.UnRegisterComponent( szDocumentIdioDBase ".IdioFileFilter" );
	rd.UnRegisterComponent( szDocumentIdioDBase ".IdioDBDocument" );
	rv.UnRegisterComponent( szDocumentIdioDBase ".IdioDBView" );

	CMenuRegistrator rcm;
	rcm.UnRegisterMenu( szIdioDBView, 0 );
	rcm.UnRegisterMenu( szIdioCloneDBView, sz_idio_type );

	CDataRegistrator	da;
	da.RegisterDataObject( false );

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}