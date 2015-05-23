// MeasManual.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "MeasManual.h"
#include "ClassBase.h"
#include "Utils.h"

#include "MeasGroup.h"


// {F5B36DD2-E81C-4334-98FB-FD2981689505}
GUID IID_ITypeLibID = 
{ 0xf5b36dd2, 0xe81c, 0x4334, { 0x98, 0xfb, 0xfd, 0x29, 0x81, 0x68, 0x95, 0x5 } };

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
// CMeasManualApp

BEGIN_MESSAGE_MAP(CMeasManualApp, CWinApp)
	//{{AFX_MSG_MAP(CMeasManualApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMeasManualApp construction

CMeasManualApp::CMeasManualApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CMeasManualApp::~CMeasManualApp()
{
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CMeasManualApp object

CMeasManualApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMeasManualApp initialization

BOOL CMeasManualApp::InitInstance()
{
	VERIFY(LoadVTClass(this) == true);
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleObjectFactory::RegisterAll();
	AfxEnableControlContainer();

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
	COleObjectFactory::UpdateRegistryAll(true);
	BOOL bRet = AfxOleRegisterTypeLib(AfxGetInstanceHandle(), IID_ITypeLibID, _T("MeasManual.tlb"));

	CCompRegistrator	rs(szPluginScriptNamespace),
						rp(szPluginPropertyPage);

	rs.RegisterComponent("MeasManual.ManualMeasureEventProvider");
	rp.RegisterComponent("MeasManual.MeasPage");

	CDataRegistrator	rd;
	rd.RegisterDataObject(true);

	CActionRegistrator	ra;
	ra.RegisterActions(true);


	/*
	CCompRegistrator	raggr( szPluginApplicationAggr	 );
	raggr.RegisterComponent( "MeasManual.MeasManager" );	
	*/

	CCompRegistrator	rm(szPluginMeasurement);
	rm.RegisterComponent("MeasManual.MeasManualGroup");



	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CActionRegistrator	ra;
	ra.RegisterActions(false);

	CDataRegistrator	rd;
	rd.RegisterDataObject(false);

	CCompRegistrator	rs(szPluginScriptNamespace),
						rp(szPluginPropertyPage);

	rs.UnRegisterComponent("MeasManual.ManualMeasureEventProvider");
	rp.UnRegisterComponent("MeasManual.MeasPage");

	/*
	CCompRegistrator	raggr( szPluginApplicationAggr );	
	raggr.UnRegisterComponent( "MeasManual.MeasManager" );
	*/

	CCompRegistrator	rm(szPluginMeasurement);
	rm.UnRegisterComponent("MeasManual.MeasManualGroup");

	AfxOleUnregisterTypeLib(IID_ITypeLibID);
	COleObjectFactory::UpdateRegistryAll(false);

	return S_OK;
}




IUnknown* GetMeasManualGroup()
{
	IUnknown *punk = 0;
	punk = FindComponentByName( GetAppUnknown(false), IID_IMeasureManager, szMeasurement );
	if( !punk )
		return 0;

	ICompManagerPtr ptrGroupMan( punk );
	punk->Release();	punk = 0;

	if( ptrGroupMan == 0 )
		return 0;

	int	nCount = 0;
	ptrGroupMan->GetCount( &nCount );

	GUID guidManualMeasGroup = ::GetExternalGuid( CManualMeasGroup::guid );


	for( int idx = 0; idx < nCount; idx++ )
	{
		punk = 0;
		ptrGroupMan->GetComponentUnknownByIdx( idx, &punk );

		if( !punk )
			continue;

		IPersistPtr ptrPersist( punk );
		punk->Release(); punk = 0;

		if( ptrPersist == 0 )
			continue;

		//get external CLSID
		CLSID clsid;
		::ZeroMemory( &clsid, sizeof(CLSID) );
		ptrPersist->GetClassID( &clsid );

		if( guidManualMeasGroup == clsid )
		{
			ptrPersist->AddRef();
			return (IUnknown*)ptrPersist;

		}

	}

	return 0;

}