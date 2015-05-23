// Baumer.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Input.h"
#include "Baumer.h"
#include "BaumerHelp.h"
#include "DriverBaumer.h"
#include "com_main.h"
#include "CxLibWork.h"

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

void AjustHelpFileName(CWinApp *papp);

#pragma comment(lib, "htmlhelp.lib")
#pragma comment (lib, "common2.lib")

#define pszDriverBaumer "IDriver.DriverBaumer"

// {FE658EB7-81B9-40D4-B1CA-B1A721964B6C}
static const GUID clsidBaumer =
{ 0xfe658eb7, 0x81b9, 0x40d4, { 0xb1, 0xca, 0xb1, 0xa7, 0x21, 0x96, 0x4b, 0x6c } };



/////////////////////////////////////////////////////////////////////////////
// CBaumerApp

BEGIN_MESSAGE_MAP(CBaumerApp, CWinApp)
	//{{AFX_MSG_MAP(CBaumerApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBaumerApp construction

CBaumerApp::CBaumerApp()
{
	m_bDiffProf = FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CBaumerApp object

CBaumerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CBaumerApp initialization

BOOL CBaumerApp::InitInstance()
{
	new App;
	App::instance()->Init((HINSTANCE)AfxGetInstanceHandle());
	App::instance()->ObjectInfo(_dyncreate_t<CDriverBaumer>::CreateObject, clsidBaumer, pszDriverBaumer, szPluginInputDrv);
	ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
	pfactory->LoadObjectInfo();
	pfactory->Release();

	
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	InitProfile(NULL);
	m_bDiffProf = GetProfileInt(_T("General"), _T("FISH"), -1);
	if (m_bDiffProf == -1)
	{
		m_bDiffProf = FALSE;
		WriteProfileInt(_T("General"), _T("FISH"), m_bDiffProf);
	}
	if (!g_BaumerProfile.IsVT5Profile())
		AjustHelpFileName(this);

//	VERIFY(LoadVTClass(this) == true);
//	COleObjectFactory::RegisterAll();
	return TRUE;
}

int CBaumerApp::ExitInstance() 
{	
	App::instance()->Deinit();
	App::instance()->Release();
	return CWinApp::ExitInstance();
}

void CBaumerApp::InitProfile(LPCTSTR lpName)
{
	TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
	GetModuleFileName(m_hInstance, szPath, _MAX_PATH);
	_tsplitpath(szPath, szDrive, szDir, NULL, NULL);
	TCHAR szBaumer[_MAX_FNAME+_MAX_EXT];
	_tcscpy(szBaumer, _T("Baumer"));
	if (m_bDiffProf && lpName && *lpName && _tcsicmp(lpName,_T("Default")))
	{
		_tcscat(szBaumer, _T("_"));
		_tcscat(szBaumer, lpName);
	}
	_tmakepath(szPath, szDrive, szDir, szBaumer, _T(".ini"));
	free((void*)m_pszProfileName);
	m_pszProfileName=_tcsdup(szPath);
}

void CBaumerApp::WinHelpInternal(DWORD dwData, UINT nCmd) 
{
	if (!RunHelpTopic(dwData, nCmd))	
		CWinApp::WinHelp(dwData, nCmd);
}


#if 1
implement_com_exports();
#else
/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	if (NeedComponentInfo(rclsid, riid))
		return GetComponentInfo(ppv);
	return AfxDllGetClassObject(rclsid, riid, ppv);
	
	//paul 2.07.2001
	/*
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	return AfxDllGetClassObject(rclsid, riid, ppv);
	*/
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
	rDrv1.RegisterComponent( "IDriver.DriverBaumer" );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	// Unregister driver
	CCompRegistrator	rDrv1( szPluginInputDrv );
	rDrv1.UnRegisterComponent( "IDriver.DriverBaumer" );

	COleObjectFactory::UpdateRegistryAll( FALSE );
	return S_OK;
}
#endif


