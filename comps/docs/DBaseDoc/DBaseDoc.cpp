// DBaseDoc.cpp : Defines the initialization routines for the DLL.
//
legacy_stdio_definitions
#include "stdafx.h"
#include "DBaseDoc.h"
#include "Constant.h"
#include "OleServInt.h"

#include "PropPage.h"
#include "Utils.h"

#include "MenuRegistrator.h"

#include "statusutils.h"

#pragma comment( lib, "EditorView.lib" )
#pragma comment( lib, "Controls.lib" )
#pragma comment( lib, "zlib.lib" )
#if _MSC_VER >= 1900
#pragma comment(lib, "legacy_stdio_definitions.lib")
#endif

// {5DF7A4E0-7BCB-4c10-B899-31978165482C}

const GUID CDECL BASED_CODE _tlid =
		{ 0x5df7a4e0, 0x7bcb, 0x4c10, { 0xb8, 0x99, 0x31, 0x97, 0x81, 0x65, 0x48, 0x2c } };

const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;





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
// CDBaseDocApp

BEGIN_MESSAGE_MAP(CDBaseDocApp, CWinApp)
	//{{AFX_MSG_MAP(CDBaseDocApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBaseDocApp construction

CDBaseDocApp::CDBaseDocApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDBaseDocApp object
			
CDBaseDocApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDBaseDocApp initialization

BOOL CDBaseDocApp::InitInstance()
{
	// Register all OLE server (factories) as running.  This enables the
	//  OLE libraries to create objects from other applications.
	VERIFY(LoadVTClass(this) == true);
	::AfxEnableControlContainer();
	COleObjectFactory::RegisterAll();


	AfxInitRichEdit();

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



	// Register document template, document and view
	CString	strDocument = szDocumentDBase;
	CCompRegistrator	rTpl( szPluginDocs );
	CCompRegistrator	rDoc( strDocument );
	
	
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );

	CCompRegistrator	rVw( strDocument+"\\"+szPluginView );
	rTpl.RegisterComponent( "DBaseDoc.DBaseDocTemplate" );
	rDoc.RegisterComponent( "DBaseDoc.DBaseDocument" );	
	rVw.RegisterComponent( szGalleryViewProgID4Registry );
	rVw.RegisterComponent( szBlankViewProgID4Registry );	
	rVw.RegisterComponent( szDBaseGridViewProgID4Registry );	

	
	//rf.RegisterComponent( "FileFilters.MSAccessFileFilter" );
	rf.RegisterComponent( "FileFilters.QueryFileFilter" );	


	// Register data
	CDataRegistrator	rd;
	rd.RegisterDataObject( true );
	// Register actions
	CActionRegistrator	ra;
	ra.RegisterActions( true );		


	CCompRegistrator	rpp(szPluginPropertyPage);
	rpp.RegisterComponent( szFilterPropPageProgID_4Reg );


	CCompRegistrator	rsf( szServerFactorySection );
	rsf.RegisterComponent( szAlbomFactoryProgID4 );


	CFileRegistrator fr;
	fr.Register( ".dbd", "VT5 Dbase Document", 3, AfxGetApp()->m_hInstance );



	/*
	CString strGalleryViewMenu;
	strGalleryViewMenu.LoadString( IDS_GALLERY_NAME );

	CString strBlankViewMenu;
	strBlankViewMenu.LoadString( IDS_BLANKVIEW_NAME );
	
	CString strGridViewMenu;
	strGridViewMenu.LoadString( IDS_GRID_NAME );

	CMenuRegistrator rcm;
	CString strTemp;
	
	strTemp.LoadString( IDS_GALLERY_MENU );	
	rcm.RegisterMenu( strGalleryViewMenu, 0, (LPCSTR)strTemp );
	strTemp.LoadString( IDS_BLANK_MENU );
	rcm.RegisterMenu( strBlankViewMenu, 0, (LPCSTR)strTemp );
	strTemp.LoadString( IDS_GRID_MENU );
	rcm.RegisterMenu( strGridViewMenu, 0, (LPCSTR)strTemp );
	*/

	CMenuRegistrator rcm;
	
	rcm.RegisterMenu( szDBBlankMenu, 0, szDBBlankMenuUN );
	rcm.RegisterMenu( szDBGalleryMenu, 0, szDBGalleryMenuUN );	
	rcm.RegisterMenu( szDBGridMenu, 0, szDBGridMenuUN );

	CString strTemp  = "Database Views";
	rcm.RegisterMenu( strTemp, 0, (LPCSTR)strTemp );

	

	return S_OK;
}

// by exporting DllRegisterServer, you can use regsvr.exe
STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	// UnRegister data
	CDataRegistrator	rd;
	rd.RegisterDataObject( false );
	// UnRegister actions
	CActionRegistrator	ra;
	ra.RegisterActions( false );		
	
	// Register document template, document and view
	CString	strDocument = szDocumentDBase;
	CCompRegistrator	rTpl( szPluginDocs );
	CCompRegistrator	rDoc( strDocument );
	CCompRegistrator	rf( strDocument+"\\"+szPluginExportImport );


	CCompRegistrator	rVw( strDocument+"\\"+szPluginView );
	rTpl.UnRegisterComponent( "DBaseDoc.DBaseDocTemplate" );
	rDoc.UnRegisterComponent( "DBaseDoc.DBaseDocument" );
	
	rVw.UnRegisterComponent( szGalleryViewProgID4Registry );
	rVw.UnRegisterComponent( szBlankViewProgID4Registry );
	rVw.UnRegisterComponent( szDBaseGridViewProgID4Registry );

	//rf.UnRegisterComponent( "FileFilters.MSAccessFileFilter" );
	rf.UnRegisterComponent( "FileFilters.QueryFileFilter" );	

	CCompRegistrator	rpp(szPluginPropertyPage);
	rpp.UnRegisterComponent( szFilterPropPageProgID_4Reg );

	CCompRegistrator	rsf( szServerFactorySection );
	rsf.UnRegisterComponent( szAlbomFactoryProgID4 );

	COleObjectFactory::UpdateRegistryAll( FALSE );

	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
void CreateRecordPaneInfo()
{
	DestroyRecordPaneInfo();
	
	INewStatusBar* psb = 0;
	psb = ::StatusGetBar();
	if( psb )
	{
		CString strTest;
		strTest.Format( IDS_RECORD_PANE_FORMAT/*reocord % of %*/, 1000, 1000, 1000 );
		
		int nWidth = ::StatusCalcWidth( strTest, true );
		psb->AddPane( guidPaneRecordInfo, nWidth, 0, 0  );		
		::StatusSetPaneIcon( guidPaneRecordInfo, 
			(HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE( IDI_DBASE ), IMAGE_ICON, 16, 16, 0 ) );


		psb->Release();	psb = 0;		
	}

	UpdateRecordPaneInfo( 0 );

}

/////////////////////////////////////////////////////////////////////////////
void DestroyRecordPaneInfo()
{
	INewStatusBar* psb = 0;
	psb = ::StatusGetBar();
	if( psb )
	{
		psb->RemovePane( guidPaneRecordInfo );
		psb->Release();	psb = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
void UpdateRecordPaneInfo( IUnknown* punkQuery )
{
	ISelectQueryPtr ptrQ( punkQuery );
	if( ptrQ == 0 )
	{
		IApplicationPtr ptrApp( ::GetAppUnknown() );
		if( ptrApp )
		{
			IUnknown* punk = 0;
			ptrApp->GetActiveDocument( &punk );
			if( punk )
			{
				IDBaseDocumentPtr ptrDBaseDoc( punk );
				punk->Release();	punk = 0;

				if( ptrDBaseDoc )
				{
					ptrDBaseDoc->GetActiveQuery( &punk );
					if( punk )
					{
						ptrQ = punk;
						punk->Release();	punk = 0;
					}
				}
			}
		}
	}

	if( ptrQ == 0 )
	{
		DestroyRecordPaneInfo();//StatusSetPaneText( guidPaneRecordInfo, (LPCSTR)"" );
		return;
	}


	INewStatusBar* psb = 0;
	psb = ::StatusGetBar();
	if( psb )
	{
		long lID = -1;
		HRESULT hRes = psb->GetPaneID( guidPaneRecordInfo, &lID );
		psb->Release();

		if( hRes != S_OK || lID < 0 )
		{
			CreateRecordPaneInfo();
		}
	}


	long lSize = 0;
	{
		IQueryObjectPtr ptrQO( ptrQ );
		int nFieldCount = 0;
		ptrQO->GetFieldsCount( &nFieldCount );
		for( int i=0;i<nFieldCount;i++ )
		{
			long l = 0;
			ptrQO->GetFieldLen( i, &l );
			lSize += l;			
		}
	}

	long lCurRecord		= -1;
	long lRecordCount	= -1;
	CString strRecordInfo;	

	ptrQ->GetCurrentRecord( &lCurRecord );
	ptrQ->GetRecordCount( &lRecordCount );

	
	if( lCurRecord < 0 )
		lCurRecord = 0;

	if( lRecordCount < 0 )
		lRecordCount = 0;		


	strRecordInfo.Format( IDS_RECORD_PANE_FORMAT, lCurRecord, lRecordCount, lSize / 1024 );

	StatusSetPaneText( guidPaneRecordInfo, (LPCSTR)strRecordInfo );
}

/////////////////////////////////////////////////////////////////////////////
CString _MakeLower( CString str )
{
	str.MakeLower();
	return str;
}

/////////////////////////////////////////////////////////////////////////////
bool GetUsersInfo( CArray<user_info,user_info&> *parr_users )
{
	if( !parr_users )	return false;

	IDBaseDocumentPtr ptr_db;
	GetDBaseDocument( ptr_db );
	if( ptr_db == 0 )	return false;

	IDBConnectionPtr ptr_conn( ptr_db );
	if( ptr_conn == 0 )	return false;

	IUnknown* punk = 0;
	ptr_conn->GetConnection( &punk );
	if( punk == 0 )		return false;

	ADO::_ConnectionPtr conn_ptr = punk;
	punk->Release();	punk = 0;
	if( conn_ptr == 0 )	return false;

	try
	{
		_variant_t var_restr = vtMissing;
		_variant_t var_id = L"{947bb102-5d43-11d1-bdbf-00c04fb92675}";
		ADO::_RecordsetPtr ptr_rs = conn_ptr->OpenSchema( ADO::adSchemaProviderSpecific, var_restr, var_id );
		long lcount = 0;
		while( !ptr_rs->ADOEOF )
		{
			ADO::FieldsPtr ptr_fields = ptr_rs->Fields;
			if( ptr_fields )
			{
				ADO::FieldPtr ptr_field0 = ptr_fields->GetItem( 0L );
				ADO::FieldPtr ptr_field1 = ptr_fields->GetItem( 1L );

				variant_t var0, var1;
				var0 = ptr_field0->GetValue();
				var1 = ptr_field1->GetValue();

				user_info ui;
				if( var0.vt == VT_BSTR )
					ui.m_str_machine	= var0.bstrVal;
				if( var1.vt == VT_BSTR )
					ui.m_str_user		= var1.bstrVal;
				parr_users->Add( ui );
			}
			lcount++;
			ptr_rs->MoveNext();
		}				
	}
	catch( _com_error &e )
	{
		dump_com_error(e);
	}
	catch(...)
	{}


	return true;
}
