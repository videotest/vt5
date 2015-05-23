// IV460Drv.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "IV460Drv.h"
#include "Input.h"
#include "VT5Profile.h"
#include "Utils.h"

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
// CIV460DrvApp

BEGIN_MESSAGE_MAP(CIV460DrvApp, CWinApp)
	//{{AFX_MSG_MAP(CIV460DrvApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIV460DrvApp construction

CIV460DrvApp::CIV460DrvApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CIV460DrvApp object

CIV460DrvApp theApp;
CIV460Profile g_IV460Profile;
/////////////////////////////////////////////////////////////////////////////
// CIV460DrvApp initialization

BOOL CIV460DrvApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	InitSettings();
	COleObjectFactory::RegisterAll();

	return TRUE;
}


void CIV460DrvApp::InitSettings()
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


/////////////////////////////////////////////////////////////////////////////
// Special entry points required for inproc servers

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
//	if (NeedComponentInfo(rclsid, riid))
//		return GetComponentInfo(ppv);
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
	rDrv1.RegisterComponent( "IDriver.DriverIV460" );
	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	COleObjectFactory::UpdateRegistryAll(false);

	CCompRegistrator	rDrv1( szPluginInputDrv );
	rDrv1.UnRegisterComponent( "IDriver.DriverIV460" );

	return S_OK;
}



static void DoRange(int nValidate, int nMin, int nMax)
{
	if (nValidate < nMin)
		nValidate = nMin;
	if (nValidate > nMax)
		nValidate = nMax;
}

CIV460Profile::CIV460Profile() : CVT5ProfileManager(_T("IV460"), true)
{
}

int CIV460Profile::GetDefaultProfileInt(LPCTSTR lpSection, LPCTSTR lpEntry, int nDef)
{
	int n = CVT5ProfileManager::GetDefaultProfileInt(lpSection, lpEntry, -1);
	if (n == -1)
	{
		if (lpSection && !_tcscmp(lpSection, _T("Settings")))
		{
/*			if (!_tcscmp(lpEntry, _T("BlackLevel")))
				n = g_CxLibWork.m_CameraStatus.vOffset;
			else if (!_tcscmp(lpEntry, _T("Gain")))
				n = g_CxLibWork.m_CameraStatus.vAmplif;
			else if (!_tcscmp(lpEntry, _T("HighSpeedShutter")))
				n = g_CxLibWork.m_CameraStatus.vShutHS;
			else if (!_tcscmp(lpEntry, _T("LowSpeedShutter")))
				n = g_CxLibWork.m_CameraStatus.vShutLS;
*/		}
		if (n == -1)
			n = nDef;
	}
	return n;
}

void CIV460Profile::ValidateInt(LPCTSTR lpSection, LPCTSTR lpEntry, int &nValidate)
{
	if (lpSection && !_tcscmp(lpSection, _T("Settings")))
	{
/*		if (!_tcscmp(lpEntry, _T("BlackLevel")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vOffsetMin, g_CxLibWork.m_CameraType.vOffsetMax);
		else if (!_tcscmp(lpEntry, _T("Gain")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vAmplMin, g_CxLibWork.m_CameraType.vAmplMax);
		else if (!_tcscmp(lpEntry, _T("HighSpeedShutter")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vShutHsMin, g_CxLibWork.m_CameraType.vShutHsMax);
		else if (!_tcscmp(lpEntry, _T("LowSpeedShutter")))
			DoRange(nValidate, g_CxLibWork.m_CameraType.vShutLsMin, g_CxLibWork.m_CameraType.vShutLsMax);
		else if (!_tcscmp(lpEntry, _T("Brightness")))
			DoRange(nValidate, 0, 255);
		else if (!_tcscmp(lpEntry, _T("Contrast")))
			DoRange(nValidate, 0, 127);
		else if (!_tcscmp(lpEntry, _T("Saturation")))
			DoRange(nValidate, 0, 127);
*/	}
}

/*
COLORREF CIV460Profile::GetProfileColor(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clr)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString strSecName("Input");
	IUnknown * punkCM = _GetOtherComponent(GetAppUnknown(), IID_IDriverManager);
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		strSecName = bstr;
		if (bstr)
			::SysFreeString(bstr);
	}
	strSecName += CString("\\") + m_sDriverName;
	strSecName += CString("\\") + lpSection;
	return GetValueColor(sptrND, strSecName, lpEntry, clr);
}

void CIV460Profile::WriteProfileColor(LPCTSTR lpSection, LPCTSTR lpEntry, COLORREF clVal, bool bIgnoreMethodic)
{
	sptrINamedData sptrND(GetAppUnknown());
	CString strSecName("Input");
	IUnknown * punkCM = _GetOtherComponent( GetAppUnknown(), IID_IDriverManager );
	if (punkCM)
	{
		sptrIDriverManager sptrDM(punkCM);
		punkCM->Release();
		BSTR bstr;
		sptrDM->GetSectionName(&bstr);
		strSecName = bstr;
		if (bstr)
			::SysFreeString(bstr);
	}
	strSecName += CString("\\") + m_sDriverName;
	strSecName += CString("\\") + lpSection;
	SetValueColor(sptrND, strSecName, lpEntry, clVal);
}
*/
