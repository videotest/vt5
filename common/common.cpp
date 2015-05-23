// common.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "common.h"

//#ifndef _DEBUG
//#pragma message( "Linking with htmlhelp.lib" )
//#pragma comment(lib, "htmlhelp.lib")
//#endif //help avaible only in release version
//#pragma message( "Linking with vfw32.lib" )
//#pragma comment(lib, "vfw32.lib")



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
// CCommonApp

class std_dll CCommonApp : public CWinApp
{
public:
	CCommonApp();
	~CCommonApp();
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


BEGIN_MESSAGE_MAP(CCommonApp, CWinApp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommonApp construction

CCommonApp::CCommonApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

}

CCommonApp::~CCommonApp()
{
}

BOOL CCommonApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// The one and only CCommonApp object

CCommonApp theApp;

/*std_dll BOOL WINAPI DllMain( HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved  )
{
	return true;
}*/


CCommonResource::CCommonResource()
{
	m_hInstOld = AfxGetResourceHandle ();
	ASSERT (m_hInstOld != NULL);

	AfxSetResourceHandle( theApp.m_hInstance );
}

CCommonResource::~CCommonResource()
{
	AfxSetResourceHandle (m_hInstOld);
}
