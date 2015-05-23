// objects.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "objects.h"
#include "Utils.h"

#include "PropPage.h"

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
// CObjectsApp

BEGIN_MESSAGE_MAP(CObjectsApp, CWinApp)
	//{{AFX_MSG_MAP(CObjectsApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CObjectsApp construction

CObjectsApp::CObjectsApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CObjectsApp::~CObjectsApp()
{
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CObjectsApp object

CObjectsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CObjectsApp initialization

BOOL CObjectsApp::InitInstance()
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

	CDataRegistrator	rd;
	rd.RegisterDataObject( true );

	CActionRegistrator	ra;
	ra.RegisterActions( true );

	CCompRegistrator	rp(szPluginPropertyPage),
						rm(szPluginMeasurement),
						rsn(szPluginScriptNamespace);

	CCompRegistrator	r(szPluginApplication);
	// [vanek] 29.01.2004: зачем регистирить в application, когда далее будет зарегистрирован в script namespace?
	//r.RegisterComponent("Objects.OpticMeasureMan");

	CString	strDocument = szDocumentImage;
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );

	rm.RegisterComponent("Objects.ImageAreaMeasGroup");
	rm.RegisterComponent("Objects.FereMeasGroup");
	rm.RegisterComponent("Objects.OpticMeasGroup");
	rm.RegisterComponent("Objects.UserManualMeasGroup");

	rsn.RegisterComponent("Objects.MeasGroupMgr");
	rsn.RegisterComponent("Objects.OpticMeasureMan");



	rp.RegisterComponent( "Objects.FixedEllipsePage" );
	rp.RegisterComponent( "Objects.FixedRectPage" );
	rp.RegisterComponent( "Objects.MagickObjectPage" );
	rp.RegisterComponent( "Objects.EditObjectPage" );
	rp.RegisterComponent( "Objects.GeneralObjectPage" );
	rp.RegisterComponent( "Objects.FreeObjectPage" );
	rp.RegisterComponent( "Objects.ObjectEditorPage" );
	rp.RegisterComponent( "Objects.CompositeEditorPage");
	rp.RegisterComponent( "Objects.PhaseEditorPage");

	rf.RegisterComponent( "Objects.FileFilter" );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CDataRegistrator	rd;
	rd.RegisterDataObject( false );

	CActionRegistrator	ra;
	ra.RegisterActions( false );

	CCompRegistrator	rp(szPluginPropertyPage),
						rm(szPluginMeasurement), 
						rsn(szPluginScriptNamespace);

	CCompRegistrator	r(szPluginApplication);
	r.UnRegisterComponent("Objects.OpticMeasureMan");

	CString	strDocument = szDocumentImage;
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );

	rm.UnRegisterComponent("Objects.ImageAreaMeasGroup");
	rm.UnRegisterComponent("Objects.FereMeasGroup");
	rm.UnRegisterComponent("Objects.OpticMeasGroup");
	rm.UnRegisterComponent("Objects.UserManualMeasGroup");

	rsn.UnRegisterComponent("Objects.MeasGroupMgr");
	rsn.UnRegisterComponent("Objects.OpticMeasureMan");

	rp.UnRegisterComponent( "Objects.FixedEllipsePage" );
	rp.UnRegisterComponent( "Objects.FixedRectPage" );
	rp.UnRegisterComponent( "Objects.MagickObjectPage" );
	rp.UnRegisterComponent( "Objects.EditObjectPage" );
	rp.UnRegisterComponent( "Objects.GeneralObjectPage" );
	rp.UnRegisterComponent( "Objects.FreeObjectPage" );
	rp.UnRegisterComponent( "Objects.ObjectEditorPage" );
	rp.UnRegisterComponent(	"Objects.CompositeEditorPage");
	rp.UnRegisterComponent( "Objects.PhaseEditorPage");

	rf.UnRegisterComponent( "Objects.FileFilter" );

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}

