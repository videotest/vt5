// AVIDocMan.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "AVIDocMan.h"
#include "Common.h"
#include "VT5AVI.h"
#include "input.h"
#include "VT5Profile.h"
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
// CAVIDocManApp

BEGIN_MESSAGE_MAP(CAVIDocManApp, CWinApp)
	//{{AFX_MSG_MAP(CAVIDocManApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAVIDocManApp construction

CAVIDocManApp::CAVIDocManApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAVIDocManApp object

CAVIDocManApp theApp;
CVT5ProfileManager s_ProfileMgr(_T("FromVideo"));

BOOL CAVIDocManApp::InitInstance() 
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	AfxEnableControlContainer();
	VERIFY(LoadVTClass(this) == true);

	COleObjectFactory::RegisterAll();
	
	return CWinApp::InitInstance();
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
	// Register data
	CDataRegistrator	rd;
	rd.RegisterDataObject( true );
	// Register actions
	CActionRegistrator	ra;
	ra.RegisterActions( true );
	// Register image creator: from video file
	CCompRegistrator	rDrv1( szPluginInputDrv );
	rDrv1.RegisterComponent( "IDriver.DriverFromVideo" );
	// Register AVI document template, document and view
	CString	strDocument = szDocumentVideo;
	CCompRegistrator	rTpl( szPluginDocs );
	CCompRegistrator	rDoc( strDocument );
	CCompRegistrator	rFltr( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rVw( strDocument+"\\"+szPluginView );
	rTpl.RegisterComponent( "VideoDoc.VideoDocTemplate" );
	rDoc.RegisterComponent( "VideoDoc.VideoDocument" );

	//Paul 26.04.2001 Cos same filter in AviBase.dll
	//rFltr.RegisterComponent( "VideoDoc.AVIFilesFilter" );
	rFltr.RegisterComponent( "VideoDoc.MOVFilesFilter" );
	rFltr.RegisterComponent( "VideoDoc.MPGFilesFilter" );
	rVw.RegisterComponent( "VideoDoc.VideoView" );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// unregister data
	CDataRegistrator	rd;
	rd.RegisterDataObject( false );
	// Unregister actions
	CActionRegistrator	ra;
	ra.RegisterActions( false );
	// Unregister image crator: from video file
	CCompRegistrator	rDrv1( szPluginInputDrv );
	rDrv1.UnRegisterComponent( "IDriver.DriverFromVideo" );
	// Unregister AVI template, document and view
	CString	strDocument = szDocumentVideo;
	CCompRegistrator	rTpl( szPluginDocs );
	CCompRegistrator	rDoc( strDocument );
	CCompRegistrator	rFltr( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rVw( strDocument+"\\"+szPluginView );
	rTpl.UnRegisterComponent( "VideoDoc.VideoDocTemplate" );
	rDoc.UnRegisterComponent( "VideoDoc.VideoDocument" );

	//Paul 26.04.2001 Cos same filter in AviBase.dll
	//rFltr.UnRegisterComponent( "VideoDoc.AVIFilesFilter" );
	rFltr.UnRegisterComponent( "VideoDoc.MOVFilesFilter" );
	rFltr.UnRegisterComponent( "VideoDoc.MPGFilesFilter" );
	rVw.UnRegisterComponent( "VideoDoc.VideoView" );

	COleObjectFactory::UpdateRegistryAll( FALSE );
	return S_OK;
}


