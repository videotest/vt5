// MV5Drv.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "MV5Drv.h"
#include <comdef.h>
#include "com_main.h"
#include "VT5Profile.h"
#include "MV500VT5Grab.h"
#include "DriverMV500.h"
/*#include "Input.h"
#include "Utils.h"*/

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "htmlhelp.lib")
#pragma comment(lib, "common2.lib")

#define pszDriverMV5 "IDriver.DriverMV5"

// {72820047-5B2C-4638-BFB4-5D38363DDF61}
static const GUID clsidMV5 = 
{ 0x72820047, 0x5b2c, 0x4638, { 0xbf, 0xb4, 0x5d, 0x38, 0x36, 0x3d, 0xdf, 0x61 } };


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

CVT5ProfileManager s_ProfileMgr(_T("MV500"));
CMV500VT5Grab g_MV500Grab;


/////////////////////////////////////////////////////////////////////////////
// CMV5DrvApp

BEGIN_MESSAGE_MAP(CMV5DrvApp, CWinApp)
	//{{AFX_MSG_MAP(CMV5DrvApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMV5DrvApp construction

CMV5DrvApp::CMV5DrvApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMV5DrvApp object

CMV5DrvApp theApp;

BOOL CMV5DrvApp::InitInstance() 
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CDriverMV500>::CreateObject, clsidMV5, pszDriverMV5, szPluginInputDrv);
	ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->LoadObjectInfo();
	pfactory->Release();
	InitSettings();

	/*VERIFY(LoadVTClass(this) == true);
	InitSettings();
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();*/
	return CWinApp::InitInstance();
}

void CMV5DrvApp::InitSettings()
{
	// Produse new .ini file name
	TCHAR szPath[_MAX_PATH];
	TCHAR szDrive[_MAX_DRIVE];
	TCHAR szDir[_MAX_DIR];
	TCHAR szFName[_MAX_FNAME];
	TCHAR szExt[_MAX_EXT];
	// Ini file will dispose in same directory as .dll file
	GetModuleFileName(AfxGetInstanceHandle(), szPath, _MAX_PATH);
	_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
	// Name and extension(.ini) will be default
	_tsplitpath(m_pszProfileName, NULL, NULL, szFName, szExt);
	_tmakepath(szPath, szDrive, szDir, szFName, szExt);
	//First free the string allocated by MFC at CWinApp startup.
	//The string is allocated before InitInstance is called.
	free((void*)m_pszProfileName);
	//Change the name of the .INI file.
	//The CWinApp destructor will free the memory.
	m_pszProfileName=_tcsdup(szPath);
}

int CMV5DrvApp::ExitInstance() 
{	
	App::instance()->Deinit();
	App::instance()->Release();
	
	return CWinApp::ExitInstance();
}


implement_com_exports();


#if 0
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
	// Register driver
	CCompRegistrator	rDrv1( szPluginInputDrv );
	rDrv1.RegisterComponent( "IDriver.DriverMV500" );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// Unregister driver
	CCompRegistrator	rDrv1( szPluginInputDrv );
	rDrv1.UnRegisterComponent( "IDriver.DriverMV500" );

	COleObjectFactory::UpdateRegistryAll( FALSE );
	return S_OK;
}
#endif





