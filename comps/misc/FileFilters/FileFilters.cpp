 // FileFilters.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "FileFilters.h"
#include "Utils.h"

//#define _LIBNAME_MAGICK	"magick.lib"

//#pragma message( "ImageMagick library linking with " _LIBNAME_MAGICK )
//#pragma comment(lib, _LIBNAME_MAGICK)

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
// CFileFiltersApp

BEGIN_MESSAGE_MAP(CFileFiltersApp, CWinApp)
	//{{AFX_MSG_MAP(CFileFiltersApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileFiltersApp construction

CFileFiltersApp::CFileFiltersApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CFileFiltersApp::~CFileFiltersApp()
{
	TRACE( "CFileFiltersApp::~CFileFiltersApp()\n" );
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CFileFiltersApp object

CFileFiltersApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CImageDocApp initialization

BOOL CFileFiltersApp::InitInstance()
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

	CString	strDocument = szDocumentImage;

	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	rf.RegisterComponent( "FileFilters.GIFileFilter" );
	//rf.RegisterComponent( "FileFilters.TIFileFilter" );
	//rf.RegisterComponent( "FileFilters.JPGFileFilter" );
	rf.RegisterComponent( "FileFilters.PCXFileFilter" );

	//CDataRegistrator	da;
	//da.RegisterDataObject();
	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	strDocument = szDocumentImage;

	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	
	rf.UnRegisterComponent( "FileFilters.GIFileFilter" );
	//rf.UnRegisterComponent( "FileFilters.TIFileFilter" );
	//rf.UnRegisterComponent( "FileFilters.JPGFileFilter" );
	rf.UnRegisterComponent( "FileFilters.PCXFileFilter" );

	//CDataRegistrator	da;
	//da.RegisterDataObject( false );

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}

