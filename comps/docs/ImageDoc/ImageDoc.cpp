// ImageDoc.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "ImageDoc.h"

#include "PropPage.h"
#include "MenuRegistrator.h"
#include "Utils.h"



//const char szDocumentImage[] = "Image";

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
// CImageDocApp

BEGIN_MESSAGE_MAP(CImageDocApp, CWinApp)
	//{{AFX_MSG_MAP(CImageDocApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageDocApp construction

CImageDocApp::CImageDocApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

CImageDocApp::~CImageDocApp()
{
}


/////////////////////////////////////////////////////////////////////////////
// The one and only CImageDocApp object

CImageDocApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CImageDocApp initialization

BOOL CImageDocApp::InitInstance()
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

	CString	strDocument = szDocumentImage;

	CCompRegistrator	rt( szPluginDocs );
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	rd( strDocument );
	CCompRegistrator	rc( szPluginColorCnv );
	CCompRegistrator	rp( szPluginStatusPane );
	CCompRegistrator	rpp(szPluginPropertyPage);
						
	rt.RegisterComponent( "ImageDoc.ImageDocTemplate" );
	rd.RegisterComponent( "ImageDoc.ImageDocument" );
	rf.RegisterComponent( "ImageDoc.BMPFileFilter" );
//	rf.RegisterComponent( "ImageDoc.ScriptFileFilter" );
	rv.RegisterComponent( "ImageDoc.ImageView" );
	rv.RegisterComponent( "ImageDoc.ObjectsView" );
	rv.RegisterComponent( "ImageDoc.MaskView" );
	rv.RegisterComponent( "ImageDoc.PseudoView" );
	rv.RegisterComponent( "ImageDoc.SrcView" );
	rv.RegisterComponent( "ImageDoc.BinaryFore" );
	rv.RegisterComponent( "ImageDoc.BinaryBack" );
	rv.RegisterComponent( "ImageDoc.Binary" );
	rv.RegisterComponent( "ImageDoc.BinaryContour" );
	
	//rv.RegisterComponent( "ScriptDoc.ScriptView" );
	rc.RegisterComponent( "ImageDoc.ColorConvertorRGB" );
	rc.RegisterComponent( "ImageDoc.ColorConvertorYUV" );
	//rc.RegisterComponent( "ImageDoc.ColorConvertorYIQ" );
	rc.RegisterComponent( "ImageDoc.ColorConvertorHSB" );
	rc.RegisterComponent( "ImageDoc.ColorConvertorGRAY" );
//	rp.RegisterComponent( "ImageDoc.ConvertPane" );
	rpp.RegisterComponent( szImageCropPropPageProgID_4Reg );
	rpp.RegisterComponent( szImagePropPageProgID_4Reg );


	//Register menu
	{
		//Menu name
		/*
		CString strImageViewMenu;
		strImageViewMenu.LoadString( IDS_IMAGEVIEW_CONTEXT_MENU );

		CString strMaskViewMenu;
		strMaskViewMenu.LoadString( IDS_MASKVIEW_CONTEXT_MENU );

		CString strObjectsViewMenu;
		strObjectsViewMenu.LoadString( IDS_OBJECTSVIEW_CONTEXT_MENU );

		CString strSourceViewMenu;
		strSourceViewMenu.LoadString( IDS_SOURCEVIEW_CONTEXT_MENU );

		CString strPseudoViewMenu;
		strPseudoViewMenu.LoadString( IDS_PSEUDOVIEW_CONTEXT_MENU );

		CString strBinaryForeViewMenu;
		strBinaryForeViewMenu.LoadString( IDS_BINARYFOREVIEW_CONTEXT_MENU );

		CString strBinaryBackViewMenu;
		strBinaryBackViewMenu.LoadString( IDS_BINARYBACKVIEW_CONTEXT_MENU );

		CString strBinaryViewMenu;
		strBinaryViewMenu.LoadString( IDS_BINARYVIEW_CONTEXT_MENU );

		CString strBinaryContourViewMenu;
		strBinaryContourViewMenu.LoadString( IDS_BINARYCONTOURVIEW_CONTEXT_MENU );

*/
		CMenuRegistrator rcm;
		//rcm.RegisterMenu( szImageView, 0, (LPCSTR)strImageViewMenu );
		rcm.RegisterMenu( szObjectsView, 0, szObjectsViewUN );
		//rcm.RegisterMenu( szMaskView, 0, (LPCSTR)strMaskViewMenu );
		rcm.RegisterMenu( szSourceView, 0, szSourceViewUN );
		//rcm.RegisterMenu( szPseudoView, 0, (LPCSTR)strPseudoViewMenu );
		//rcm.RegisterMenu( szBinaryForeView, 0, (LPCSTR)strBinaryForeViewMenu );
		//rcm.RegisterMenu( szBinaryBackView, 0, (LPCSTR)strBinaryBackViewMenu );
		rcm.RegisterMenu( szBinaryView, 0, szBinaryViewUN );
		//rcm.RegisterMenu( szBinaryContourView, 0, (LPCSTR)strBinaryContourViewMenu );

		/*
		//Object support
		CString strObjectListMenu;
		strObjectListMenu.LoadString( IDS_OBJECT_LIST_MENU );

		CString strDrawingObjectMenu;
		strDrawingObjectMenu.LoadString( IDS_DRAWING_OBJECT_MENU );				

		CString strImageObjectMenu;
		strImageObjectMenu.LoadString( IDS_IMAGE_CHILD_MENU );		
		*/

		
		rcm.RegisterMenu( szImageCloneView, szTypeObject, szObjectListUN );
		rcm.RegisterMenu( szImageCloneView, szTypeManualMeasObject, szMeasureObjectUN );		
		rcm.RegisterMenu( szImageCloneView, szDrawingObject, szDawingObjectUN );
		rcm.RegisterMenu( szImageCloneView, szTypeImage, szImageChildUN );
		rcm.RegisterMenu( szImageCloneView, szTypeMarker, szMarkerObjectUN );
	}

	CActionRegistrator	ra;
	ra.RegisterActions();

	CDataRegistrator	da;
	da.RegisterDataObject();

	CFileRegistrator fr;
	fr.Register( ".image", "VT5 Image Document", 0, AfxGetApp()->m_hInstance );
	::SHChangeNotify( SHCNE_ASSOCCHANGED, SHCNF_IDLIST, 0, 0 );

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	CString	strDocument = szDocumentImage;

	CCompRegistrator	rt( szPluginDocs );
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );
	CCompRegistrator	rv( strDocument+"\\"+szPluginView );
	CCompRegistrator	rd( strDocument );
	CCompRegistrator	rc( szPluginColorCnv );
	CCompRegistrator	rpp(szPluginPropertyPage);

	rt.UnRegisterComponent( "ImageDoc.ImageDocTemplate" );
	rd.UnRegisterComponent( "ImageDoc.ImageDocument" );
	rf.UnRegisterComponent( "ImageDoc.BMPFileFilter" );
//	rf.UnRegisterComponent( "ImageDoc.ScriptFileFilter" );
	rv.UnRegisterComponent( "ImageDoc.ImageView" );
	rv.UnRegisterComponent( "ImageDoc.ObjectsView" );
	rv.UnRegisterComponent( "ImageDoc.MaskView" );
	rv.UnRegisterComponent( "ImageDoc.PseudoView" );
	rv.UnRegisterComponent( "ImageDoc.SrcView" );
	rv.UnRegisterComponent( "ImageDoc.BinaryFore" );
	rv.UnRegisterComponent( "ImageDoc.BinaryBack" );
	rv.UnRegisterComponent( "ImageDoc.Binary" );
	rv.UnRegisterComponent( "ImageDoc.BinaryContour" );

	
	//rv.UnRegisterComponent( "ScriptDoc.ScriptView" );
	rc.UnRegisterComponent( "ImageDoc.ColorConvertorRGB" );
	rc.UnRegisterComponent( "ImageDoc.ColorConvertorYUV" );
	//rc.UnRegisterComponent( "ImageDoc.ColorConvertorYIQ" );
	rc.UnRegisterComponent( "ImageDoc.ColorConvertorHSB" );
	rc.UnRegisterComponent( "ImageDoc.ColorConvertorGRAY" );
	rpp.UnRegisterComponent( szImageCropPropPageProgID_4Reg );
	rpp.UnRegisterComponent( szImagePropPageProgID_4Reg );

	
	//UnRegister menu
	{
		CMenuRegistrator rcm;
		//rcm.UnRegisterMenu( szImageView, 0 );
		//rcm.UnRegisterMenu( szMaskView, 0);
		rcm.UnRegisterMenu( szObjectsView, 0 );
		rcm.UnRegisterMenu( szSourceView, 0 );
		//rcm.UnRegisterMenu( szPseudoView, 0 );
		//rcm.UnRegisterMenu( szBinaryForeView, 0 );
		//rcm.UnRegisterMenu( szBinaryBackView, 0 );
		rcm.UnRegisterMenu( szBinaryView, 0 );
		//rcm.UnRegisterMenu( szBinaryContourView, 0 );
			

		//Object support		
		rcm.UnRegisterMenu( szImageCloneView, szTypeObject );
		rcm.UnRegisterMenu( szImageCloneView, szDrawingObject );
		rcm.UnRegisterMenu( szImageCloneView, szTypeImage );	
		rcm.UnRegisterMenu( szImageCloneView, szTypeMarker );
	}


	CActionRegistrator	ra;
	ra.RegisterActions( false );

	CDataRegistrator	da;
	da.RegisterDataObject( false );

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}

