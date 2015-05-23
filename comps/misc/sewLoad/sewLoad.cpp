// sewLoad.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "sewLoad.h"
#include "Utils.h"
#include "MenuRegistrator.h"

#include "resource.h"

#ifdef _DEBUG
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
// CSewLoad

BEGIN_MESSAGE_MAP(CSewLoad, CWinApp)
	//{{AFX_MSG_MAP(CSewLoad)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSewLoad construction

CSewLoad::CSewLoad()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CSewLoad::~CSewLoad()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}
/////////////////////////////////////////////////////////////////////////////
// The one and only CSewLoad object

CSewLoad theApp;

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailApp initialization

BOOL CSewLoad::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	VERIFY(LoadVTClass(this) == true);
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

	CActionRegistrator	ra;
	ra.RegisterActions();
	CCompRegistrator	rp(szPluginObject);
	rp.RegisterComponent("sewLoad.SewLoad");

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCompRegistrator	rm(szPluginObject);
	rm.UnRegisterComponent("sewLoad.SewLoad");

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}

class _DisableNotification
{
	_variant_t varPrev;
public:
	_DisableNotification()
	{
		varPrev = GetValue( GetAppUnknown(), "\\General", "EnableFilterNotify", _variant_t() );
		SetValue( GetAppUnknown(), "\\General", "EnableFilterNotify", (long)0 );
	}
	~_DisableNotification()
	{
		SetValue( GetAppUnknown(), "\\General", "EnableFilterNotify", varPrev );
	}
};

BOOL SewLoadPic(BSTR pathPicName, IUnknown** ppunkPic )
{
	_DisableNotification dn;
	CWaitCursor wc;
	if(!ppunkPic) return FALSE;
	*ppunkPic = 0;

	CString sPath=pathPicName;
	CFileFind ff;
	if (!ff.FindFile(sPath))
		return FALSE;
	IUnknownPtr punk( GetFilterByFile(sPath), false );
		
	if(punk==0) return FALSE;

	sptrIFileFilter3 sptrFileFilter = punk;
	sptrIFileDataObject2	sptrF=sptrFileFilter;
	
	HRESULT hr = sptrF->LoadFile(pathPicName);
	if(FAILED(hr))
	{
		//detach
		sptrFileFilter->AttachNamedData(0,0);
		return FALSE;
	}

	// get created objects
	int nCount = 0;
	sptrFileFilter->GetCreatedObjectCount(&nCount);

	if( nCount > 0 )
	{
		sptrFileFilter->GetCreatedObject( 0, ppunkPic );
	}

	sptrFileFilter->AttachNamedData(0, 0); //detach


	return *ppunkPic!=0;
};


BOOL SewSavePic(BSTR pathPicName, IUnknown* punkPic, IUnknown* pNamedData)
{
	_DisableNotification dn;
	CWaitCursor wc;
	CString sPath=pathPicName;
	IUnknownPtr punk( GetFilterByFile(sPath), false );
		
	if(punk==0) return FALSE;

	sptrIFileFilter3 sptrFileFilter = punk;
	sptrIFileDataObject2	sptrF=sptrFileFilter;
	
	sptrFileFilter->AttachNamedData(pNamedData,0);
	sptrFileFilter->AttachNamedObject(punkPic);
	HRESULT hr = sptrF->SaveFile(pathPicName);
	sptrFileFilter->AttachNamedObject(NULL);
	sptrFileFilter->AttachNamedData(NULL,0);
	return SUCCEEDED(hr);
};

