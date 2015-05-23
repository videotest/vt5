// Editor.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "Editor.h"
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
// CEditorApp

BEGIN_MESSAGE_MAP(CEditorApp, CWinApp)
	//{{AFX_MSG_MAP(CEditorApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditorApp construction

CEditorApp::CEditorApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CEditorApp::~CEditorApp()
{
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CEditorApp object

CEditorApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEditorApp initialization

BOOL CEditorApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	CString str(DEF_COMPANY_NAME);
	long lSize = 0;
	GetRegCompanyName(0, &lSize);
	if (lSize)
	{
		GetRegCompanyName(str.GetBuffer(lSize + 1), &lSize);
		str.ReleaseBuffer();
	}

	SetRegistryKey(str);

	VERIFY(LoadVTClass(this) == true);

	AfxEnableControlContainer();
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
	CDataRegistrator	rd;
	rd.RegisterDataObject( true );
	CActionRegistrator	ra;
	ra.RegisterActions();
	CCompRegistrator	rp( szPluginPropertyPage );
	rp.RegisterComponent( szEditorLinePropPageProgID_4Reg );
	rp.RegisterComponent( szEditorRectPropPageProgID_4Reg );
	rp.RegisterComponent( szEditorCirclePropPageProgID_4Reg );
	rp.RegisterComponent( szEditorFontPropPageProgID_4Reg );
	rp.RegisterComponent( szEditorTextPropPageProgID_4Reg );
	rp.RegisterComponent( szEditorDodgePropPageProgID_4Reg );
	rp.RegisterComponent( szEditorArrowPropPageProgID_4Reg );
	rp.RegisterComponent( szEditorColorPickerPropPageProgID_4Reg );
	rp.RegisterComponent( szEditorFloodFillPropPageProgID_4Reg );
	rp.RegisterComponent( szEditorBinLinePropPageProgID_4Reg );
	rp.RegisterComponent( szSelectBinaryPropPageProgID);

	COleObjectFactory::UpdateRegistryAll();
	return S_OK;
}

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDataRegistrator	rd;
	rd.RegisterDataObject( false );
	CActionRegistrator	ra;
	ra.RegisterActions(false);
	CCompRegistrator	rp( szPluginPropertyPage );
	rp.UnRegisterComponent( szEditorLinePropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorRectPropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorCirclePropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorFontPropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorTextPropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorDodgePropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorArrowPropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorColorPickerPropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorFloodFillPropPageProgID_4Reg );
	rp.UnRegisterComponent( szEditorBinLinePropPageProgID_4Reg );
	rp.UnRegisterComponent( szSelectBinaryPropPageProgID);
	COleObjectFactory::UpdateRegistryAll( FALSE  );
	return S_OK;
}
