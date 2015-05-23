// Data.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Data.h"
#include "SendTo.h"
#include "Utils.h"
#include "docmiscint.h"
#include "dbase.h"

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
// CDataApp

BEGIN_MESSAGE_MAP(CDataApp, CWinApp)
	//{{AFX_MSG_MAP(CDataApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataApp construction

CDataApp::CDataApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CDataApp::~CDataApp()
{
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CDataApp object

CDataApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDataApp initialization

BOOL CDataApp::InitInstance()
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
	COleObjectFactory::UpdateRegistryAll();

	CActionRegistrator	ar;
	ar.RegisterActions();

	CCompRegistrator	rda( szPluginDocumentsAggr ), 
						rd( szPluginDocument ), 
						ra( szPluginApplicationAggr ), 
						rs( szPluginScriptNamespace ), 
						rv( szPluginViewAggr ),
						rdw( szPluginDockableWindows );

	rdw.RegisterComponent( "Data.ContextViewer" );

	rda.RegisterComponent( "Data.NamedData" );
	
	ra.RegisterComponent( "Data.NamedData" );
	ra.RegisterComponent( "Data.TypeInfoManager" );
	ra.RegisterComponent( szSendToManagerProgID_4Reg );	

	rs.RegisterComponent( "Data.DataDispWrp" );
	rd.RegisterComponent( "Data.DataDispWrp" );

	rv.RegisterComponent("Data.DataContext");
//	rs.RegisterComponent("Data.DataContext");
	rda.RegisterComponent("Data.DataContext");

	rs.RegisterComponent( "Data.Objects" );


	//CCompRegistrator	rm( szPluginApplication );
	//rm.RegisterComponent( "Data.ObjectManager" );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CCompRegistrator	rda( szPluginDocumentsAggr ), 
						rd( szPluginDocument ), 
						ra( szPluginApplicationAggr ), 
						rs( szPluginScriptNamespace ), 
						rv( szPluginViewAggr ), 
						rdw( szPluginDockableWindows );

	rdw.UnRegisterComponent( "Data.ContextViewer" );

	rda.UnRegisterComponent( "Data.NamedData" );
	
	ra.UnRegisterComponent( "Data.NamedData" );
	ra.UnRegisterComponent( "Data.TypeInfoManager" );
	ra.UnRegisterComponent( szSendToManagerProgID_4Reg );	

	rs.UnRegisterComponent( "Data.DataDispWrp" );
	rd.UnRegisterComponent( "Data.DataDispWrp" );

	rv.UnRegisterComponent("Data.DataContext");
//	rs.UnRegisterComponent("Data.DataContext");
	rda.UnRegisterComponent("Data.DataContext");

	CActionRegistrator	ar;
	ar.RegisterActions( false );

	rs.UnRegisterComponent( "Data.Objects" );

	//CCompRegistrator	rm( szPluginApplication );
	//rm.UnRegisterComponent( "Data.ObjectManager" );

	COleObjectFactory::UpdateRegistryAll( false );

	return S_OK;
}


CString GetProviderUserName( IUnknown* punkObj, bool bReturnObjNameIfFailed )
{
	CString strName;

	INamedDataObject2Ptr ptrNDO2( punkObj );
	if( ptrNDO2 )
	{
		IUnknown* punkData = 0;
		ptrNDO2->GetData( &punkData );

		if( punkData )
		{
			IUserNameProviderPtr ptrUNP( punkData );
			punkData->Release();	punkData = 0;

			if( ptrUNP )
			{
				BSTR bstrUN = 0;
				if( S_OK == ptrUNP->GetUserName( punkObj, &bstrUN ) )
				{
					strName = bstrUN;
					::SysFreeString( bstrUN );	bstrUN = 0;
				}
			}				
		}
	}

	if( strName.IsEmpty() && bReturnObjNameIfFailed )
	{
		INamedObject2Ptr ptrNO2( punkObj );
		if( ptrNO2 )
		{
			BSTR bstr = 0;
			ptrNO2->GetName( &bstr );
			strName = bstr;
			::SysFreeString( bstr );	bstr = 0;
		}
	}

	return strName;

}

bool IsDBaseRootObject( IUnknown* punkObj, IUnknown* punkData )
{
	IUnknownPtr ptrData = punkData;
	if( ptrData == 0 )
	{
		INamedDataObject2Ptr ptrNDO2( punkObj );
		if( ptrNDO2 )
		{
			IUnknown* punk = 0;
			ptrNDO2->GetData( &punk );
			if( punk )
			{
				ptrData = punk;
				punk->Release();	punk = 0;
			}
		}
	}

	IDBaseDocumentPtr ptrDBDoc( ptrData );
	if( ptrDBDoc )
	{
		BOOL bDBaseObject = FALSE;			
		GuidKey guidObjRoot;
		ptrDBDoc->IsDBaseObject( punkObj, &bDBaseObject, &guidObjRoot, 0 );
		if( bDBaseObject && guidObjRoot == ::GetObjectKey( punkObj ) )
			return true;
	}

	return false;

}

