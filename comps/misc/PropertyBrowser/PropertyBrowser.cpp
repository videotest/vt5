// PropertyBrowser.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//      To build a separate proxy/stub DLL, 
//      run nmake -f PropertyBrowserps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <initguid.h>

#include "PropertyBrowser.h"
#include "AllPropPage.h"

//const IID LIBID_PROPERTYBROWSERLib = {0x2F1431F6,0xD876,0x11D3,{0xA0,0x95,0x00,0x00,0xB4,0x93,0xA1,0x87}};
//const CLSID CLSID_AllPropPage = {0x2F143203,0xD876,0x11D3,{0xA0,0x95,0x00,0x00,0xB4,0x93,0xA1,0x87}};

CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_AllPropPage, CAllPropPage)
END_OBJECT_MAP()

class CPropertyBrowserApp : public CWinApp
{
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPropertyBrowserApp)
	public:
    virtual BOOL InitInstance();
    virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPropertyBrowserApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CPropertyBrowserApp, CWinApp)
	//{{AFX_MSG_MAP(CPropertyBrowserApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

CPropertyBrowserApp theApp;

BOOL CPropertyBrowserApp::InitInstance()
{
    _Module.Init(ObjectMap, m_hInstance, &LIBID_PROPERTYBROWSERLib);
    return CWinApp::InitInstance();
}

int CPropertyBrowserApp::ExitInstance()
{
    _Module.Term();
    return CWinApp::ExitInstance();
}


/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE}

//STDAPI DllCanUnloadNow(void)
//{
//    return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
//}
//
///////////////////////////////////////////////////////////////////////////////
//// Returns a class factory to create an object of the requested type
//
//STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
//{
//    return _Module.GetClassObject(rclsid, riid, ppv);
//}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
    // registers object, typelib and all interfaces in typelib
    return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
    return _Module.UnregisterServer(TRUE);
}



//__declspec(dllexport)
//BOOL WINAPI DllMain(
//  HINSTANCE hinstDLL,  // handle to the DLL module
//  DWORD fdwReason,     // reason for calling function
//  LPVOID lpvReserved   // reserved
//)
//{
//	if( fdwReason == DLL_PROCESS_ATTACH )
//		_Module.Init(ObjectMap, hinstDLL, &LIBID_PROPERTYBROWSERLib);
//	else if( fdwReason == DLL_PROCESS_DETACH )
//		_Module.Term();
//
//	return 1;
//}