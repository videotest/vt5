#include "stdafx.h"
#include "resource.h"
#include "docman.h"
#include "dbasedoc.h"
#include "afxpriv2.h"
#include "docmiscint.h"

#include "Wizards.h"

#include "ChoiceActiveQuery.h"
#include "Generate.h"


//#include "imapi.h"

#include "Thumbnail.h"
#include "dbmacros.h"

// {88DA53EE-E80E-460e-9D17-E904DBFD83EB}
GUARD_IMPLEMENT_OLECREATE(CDBaseDocument, "DBaseDoc.DBaseDocument", 
0x88da53ee, 0xe80e, 0x460e, 0x9d, 0x17, 0xe9, 0x4, 0xdb, 0xfd, 0x83, 0xeb);
// {1A6FAB98-BA3C-490f-BBF2-D307B1B401BE}
GUARD_IMPLEMENT_OLECREATE(CDBaseDocTemplate, "DBaseDoc.DBaseDocTemplate",
0x1a6fab98, 0xba3c, 0x490f, 0xbb, 0xf2, 0xd3, 0x7, 0xb1, 0xb4, 0x1, 0xbe);

IMPLEMENT_DYNCREATE(CDBaseDocument, CCmdTargetEx)
IMPLEMENT_DYNCREATE(CDBaseDocTemplate, CCmdTargetEx)


BEGIN_DISPATCH_MAP(CDBaseDocument, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CNamedData)
	DISP_FUNCTION(CDBaseDocument, "SetValue", SetValue, VT_EMPTY, VTS_BSTR VTS_VARIANT)
	DISP_FUNCTION(CDBaseDocument, "GetValue", GetValue, VT_VARIANT, VTS_BSTR)
	DISP_FUNCTION(CDBaseDocument, "SetSection", SetSection, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CDBaseDocument, "GetEntriesCount", GetEntriesCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CDBaseDocument, "GetEntryName", GetEntryName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CDBaseDocument, "RemoveValue", RemoveValue, VT_EMPTY, VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_INamedDataDisp to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {CD99D2C0-E6A2-482d-ACE4-144D5E838019}
static const GUID IID_IDBaseNamedDataDisp = 
{ 0xcd99d2c0, 0xe6a2, 0x482d, { 0xac, 0xe4, 0x14, 0x4d, 0x5e, 0x83, 0x80, 0x19 } };


BEGIN_INTERFACE_MAP(CDBaseDocument, CDocBase)	
	INTERFACE_PART(CDBaseDocument, IID_IDBaseNamedDataDisp, Dispatch)
	INTERFACE_PART(CDBaseDocument, IID_INamedData, Data)	
	INTERFACE_PART(CDBaseDocument, IID_IDataTypeManager, Types)		
	INTERFACE_PART(CDBaseDocument, IID_IDBaseDocument, DBaseDocument)
	INTERFACE_PART(CDBaseDocument, IID_IDBaseDocument2, DBaseDocument)
	INTERFACE_PART(CDBaseDocument, IID_IFileDataObject, File)
	INTERFACE_PART(CDBaseDocument, IID_IFileDataObject2, File)
	INTERFACE_PART(CDBaseDocument, IID_ISerializableObject, Serialize )
	INTERFACE_PART(CDBaseDocument, IID_IDBaseListener, DBaseListener)
	INTERFACE_PART(CDBaseDocument, IID_IDBaseStruct, DBaseStruct)
	INTERFACE_PART(CDBaseDocument, IID_IDBaseFilterProvider, DBaseFilterProvider)
	INTERFACE_PART(CDBaseDocument, IID_IDBConnection, DBConnection)
	INTERFACE_PART(CDBaseDocument, IID_IDBaseFilterHolder, DBaseFilterHolder)	
	INTERFACE_PART(CDBaseDocument, IID_IThumbnailSurface, ThumbnailSurface)	
	INTERFACE_PART(CDBaseDocument, IID_INotifyPlace, NotifyPlace)	
	INTERFACE_PART(CDBaseDocument, IID_IUserNameProvider, UserNameProvider)	
	INTERFACE_PART(CDBaseDocument, IID_IDBLockInfo, LockInfo)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CDBaseDocument, Data);
IMPLEMENT_UNKNOWN(CDBaseDocument, Types);
IMPLEMENT_UNKNOWN(CDBaseDocument, DBaseDocument);
IMPLEMENT_UNKNOWN(CDBaseDocument, DBaseStruct);
IMPLEMENT_UNKNOWN(CDBaseDocument, DBaseFilterProvider);
IMPLEMENT_UNKNOWN(CDBaseDocument, DBConnection);
IMPLEMENT_UNKNOWN(CDBaseDocument, DBaseFilterHolder);
IMPLEMENT_UNKNOWN(CDBaseDocument, ThumbnailSurface);
IMPLEMENT_UNKNOWN(CDBaseDocument, NotifyPlace);
IMPLEMENT_UNKNOWN(CDBaseDocument, UserNameProvider);
IMPLEMENT_UNKNOWN(CDBaseDocument, LockInfo);


/////////////////////////////////////////////////////////////////////////////////////////
//CDBaseDocTemplate
CDBaseDocTemplate::CDBaseDocTemplate()
{
	EnableAggregation();
}

CString CDBaseDocTemplate::GetDocTemplString()
{
	CString	str;
	str.LoadString( IDS_TEMPLATE );
	return str;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CDBaseDocument
CDBaseDocument::CDBaseDocument(): m_dbEngine(this)
{
	//EnableAutomation();
	EnableAggregation();
	m_punkRecordNData	= NULL;
	m_punkDocNData		= NULL;	

	m_bRecordNDataOK = m_bDocNDataOK = false;	

	SetModifiedFlag( TRUE );			

	m_bNeedSetModifiedFlagAfterOpen	= false;
	m_bNeedActivateQueryInView		= false;
	m_bNeedActivateSpecificView		= false;


	m_bShowUserNameInContext		= ( ::GetValueInt( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, "ShowUserNameInContext", 1L ) == 1L );
	m_bAddRootFieldNameInContext	= ( ::GetValueInt( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, "AddRootFieldNameInContext", 0L ) == 1L );

	m_bReadOnly						= false;
	m_str_dbd_open_file_name		= "";
	m_bReadDeleteOnly = false;

}

/////////////////////////////////////////////////////////////////////////////////////////
CDBaseDocument::~CDBaseDocument()
{
	sptrIDBaseDocument spDoc( GetControllingUnknown() );
	if( spDoc )
			spDoc->UnRegisterDBaseListener( spDoc );			

	DeInitRecordNamedData();
	DeInitDocNamedData();

	UnRegister();
	
}

extern bool g_inside_converter;
/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::OnNewDocument()
{			
	if( g_inside_converter )
		return true;

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 

	CWizardPool pool;
	bool bRes = pool.NewDBWizard( GetControllingUnknown() );
	if( !bRes  )
	{		
		if( !::GetValueInt( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, "AskDocCreateFail", 0L ) )
			return false;

		if( AfxMessageBox( IDS_WARNING_CREATE_DOC_ANYWHERE, MB_ICONQUESTION | MB_YESNO ) == IDYES )
			return true;
		else
			return false;
	}


	sptrIQueryObject ptrQ( GetActiveQuery() );
	if( ptrQ )
	{
		IDataContext2Ptr ptrC( GetControllingUnknown() );	
		if( ptrC )
		{
			
			::SetValue( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, "DefaultView", szBlankViewProgID );

			// A.M. fix, BT 3529
//			BOOL bSimple = FALSE;
//			ptrQ->IsSimpleQuery( &bSimple );

			if( ::IsAccessDBase( GetControllingUnknown() ) /*&& bSimple*/ )
			{
				ISelectQueryPtr ptrSQ( ptrQ );
				if( ptrSQ )
					ptrSQ->Insert();

				IDBConnectionPtr ptrDB( GetControllingUnknown() );
				if( ptrDB )
				{
					BSTR bstr = 0;
					ptrDB->GetMDBFileName( &bstr );

					CString strMDBFilePath = bstr;

					if( bstr )
						::SysFreeString( bstr );


					CString strTmp = strMDBFilePath, strFile;

					strFile = strTmp.Left( strTmp.GetLength() - 3 );
					strFile += "dbd";

					IDocumentSitePtr ptr_ds( GetControllingUnknown() );
					if( ptr_ds )
						ptr_ds->SaveDocument( _bstr_t( (LPCSTR)strFile ) );
				}

			}

			{
				long lCount = 0;
				VERIFY( S_OK == ptrC->GetObjectTypeCount( &lCount ) );
				for( long l=0;l<lCount;l++ )
				{
					BSTR bstrTypeName = 0;
					VERIFY( S_OK == ptrC->GetObjectTypeName( l, &bstrTypeName ) );
					VERIFY( S_OK == ptrC->UnselectAll( bstrTypeName ) );

					if( bstrTypeName )
						::SysFreeString( bstrTypeName );
				}				
			}

			//ptrC->SetObjectSelect( ptrQ, 1 );

			//m_bNeedActivateQueryInView = true;			

			//ExecuteAction( "ShowBlankPreview" );
		}
	}

	/*

	IDocumentSitePtr ptrDS( GetControllingUnknown() );
	if( ptrDS )
	{
		IUnknown* punkView = 0;
		ptrDS->GetActiveView( &punkView );
		if( punkView )
		{
			if( ActivateQueryInBlank( punkView ) )
				m_bNeedActivateQueryInView = false;

			punkView->Release();
		}
	}
	*/
	
	return true;
}
   
void CDBaseDocument::Init()
{
	CDocBase::Init();
	Register( );	

	sptrIDBaseDocument spDoc( GetControllingUnknown() );
	if( spDoc )
			spDoc->RegisterDBaseListener( spDoc );

	if( !IsOtherDBaseDocExist() )
		CreateRecordPaneInfo();

}

/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::InitRecordNamedData()
{
	DeInitRecordNamedData();

	m_punkRecordNData = NULL;

	HRESULT hr;
	BSTR bstr;	

	
	CLSID	clsid;
	CString strProgID;
	
	strProgID = szNamedDataProgID; 
	bstr = strProgID.AllocSysString();
	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return false;

	if( bstr )
		::SysFreeString( bstr );
	hr = CoCreateInstance(clsid,
       NULL, CLSCTX_INPROC_SERVER,
       IID_IUnknown, (LPVOID*)&m_punkRecordNData);
	
	if(hr != S_OK)
	{
		m_punkRecordNData = NULL;	
		return false;
	}

	IDocForNamedDataPtr	ptrNDP(m_punkRecordNData);
	if( ptrNDP != 0 )
		ptrNDP->AttachExternalDocument( GetControllingUnknown() );

	m_bRecordNDataOK = true;

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::InitDocNamedData()
{
	DeInitDocNamedData();

	m_punkDocNData = NULL;

	HRESULT hr;
	BSTR bstr;	

	
	CLSID	clsid;
	CString strProgID;
	
	strProgID = szNamedDataProgID; 
	bstr = strProgID.AllocSysString();
	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return false;

	if( bstr )
		::SysFreeString( bstr );

	hr = CoCreateInstance(clsid,
       NULL, CLSCTX_INPROC_SERVER,
       IID_IUnknown, (LPVOID*)&m_punkDocNData);

	if(hr != S_OK)
	{
		m_punkDocNData = NULL;	
		return false;
	}

	IDocForNamedDataPtr	ptrNDP(m_punkDocNData);
	if( ptrNDP != 0 )
		ptrNDP->AttachExternalDocument( GetControllingUnknown() );

	m_bDocNDataOK = true;


	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::DeInitRecordNamedData()
{
	if( m_punkRecordNData )
	{
		m_punkRecordNData->Release();
		m_punkRecordNData = NULL;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::DeInitDocNamedData()
{
	if( m_punkDocNData )
	{
		m_punkDocNData->Release();
		m_punkDocNData = NULL;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
IUnknown* CDBaseDocument::GetRecordNamedData( )
{
	if( m_punkRecordNData == NULL )
		return NULL;

	m_punkRecordNData->AddRef();
	return m_punkRecordNData;
}

/////////////////////////////////////////////////////////////////////////////////////////
IUnknown* CDBaseDocument::GetDocNamedData( )
{
	if( m_punkDocNData != NULL )
	{	
		m_punkDocNData->AddRef();
		return m_punkDocNData;
	}
	else
		return NULL;
}


/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::InitNew()
{
	m_dbEngine.Close();

	IUnknown* punkND = GetRecordNamedData( );
	sptrINamedData spRecordND( punkND );		
	punkND->Release();
	
	if( spRecordND )
		spRecordND->DeleteEntry( 0 );	


	m_filterHolder.DeInit();

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::InitDBaseEngine()
{	
	CString sConnection = GetConnectionString();
	CString sDataSource = _T("Data Source=");
	int nPos = sConnection.Find(sDataSource);
	if (nPos > 0)
	{
		int nPathPos = nPos+sDataSource.GetLength();
		ASSERT(nPathPos < sConnection.GetLength());
		int nEnd = sConnection.Find(_T(';'), nPathPos);
		CString sPath;
		if (nEnd > 0)
			sPath = sConnection.Mid(nPathPos,nEnd-nPathPos);
		else
			sPath = sConnection.Mid(nPathPos);
		TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME],szExt[_MAX_EXT];
		_tsplitpath(sPath, szDrive, szDir, szFName, szExt);
		if (szDrive[0] == 0 || szDir[0] == 0)
		{
			_tsplitpath(m_str_dbd_open_file_name, szDrive[0]?NULL:szDrive, szDir[0]?NULL:szDir, NULL, NULL);
			_tmakepath(szPath, szDrive, szDir, szFName, szExt);
			CString sConnNew = sConnection.Left(nPathPos);
			sConnNew += szPath;
			if (nEnd > 0)
				sConnNew += sConnection.Mid(nEnd);
			sConnection = sConnNew;
		}
	}
	return m_dbEngine.OpenFromString( sConnection );
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::ReadFile( const char *pszFileName )
{
	//test if file read only
	{
		m_str_dbd_open_file_name = pszFileName;

		CFileFind ff;
		if (!ff.FindFile(pszFileName))
			return false;
		ff.FindNextFile();
		if (ff.IsDirectory()) return false;
		if (ff.IsReadOnly())
			m_bReadOnly = true;
		ff.Close();

/*		FILE *fl = fopen( pszFileName, "a" );
		if( fl == 0 )
			m_bReadOnly = true;
		else
			fclose( fl );*/

		/*
		HANDLE hfile = ::CreateFile( pszFileName, FILE_WRITE_DATA, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		DWORD dwErr = GetLastError();
		if( hfile == INVALID_HANDLE_VALUE )
			m_bReadOnly = true;
		else
			::CloseHandle( hfile );	hfile = 0;
		*/
	}

	m_bNeedActivateSpecificView = true;

	if( !IsValidNameData() )			
		return false;

	try
	{
		IUnknown* punkND = GetRecordNamedData( );
		sptrINamedData spRecordND( punkND );		
		punkND->Release();
		
		spRecordND->DeleteEntry( 0 );
		
		sptrIFileDataObject spFO( spRecordND );
		
		
		if( spFO == NULL )
			return false;

		//////////// changed by vanek 03.07.2003 ////////////
		if( E_FAIL == spFO->LoadFile( _bstr_t(pszFileName) ) )
			return false;
		//////////////////////////////////////////////////////
		

		
	}
	catch(CException* e)
	{
		e->Delete();
		return false;
	}


	IUnknown* punkND = GetRecordNamedData( );
	sptrINamedData spND( punkND );
	punkND->Release();
	
	spND->DeleteEntry( _bstr_t(SECTION_DBASERECORDNAMEDDATA) );


	m_filterHolder.LoadFromData( GetControllingUnknown() );


	//Set DBaseDocument pointer to query object
	{		
		IDataContext2Ptr ptrDC2( GetControllingUnknown() );		

		if( ptrDC2 )
		{
			long nCount = 0;
			_bstr_t _bstrQueryType = szTypeQueryObject;
			ptrDC2->GetObjectCount( _bstrQueryType, &nCount );
			long lPos = 0;
			ptrDC2->GetFirstObjectPos( _bstrQueryType, &lPos );
			while( lPos )
			{
				IUnknown* punkQuery = NULL;
				ptrDC2->GetNextObject( _bstrQueryType, &lPos, &punkQuery );
				if( punkQuery == NULL )
					continue;

				IDBChildPtr ptrChild = punkQuery;
				punkQuery->Release();
		
				if( ptrChild )
					ptrChild->SetParent( GetControllingUnknown() );
			}


			//set active query
			IDBaseDocumentPtr ptrThis( GetControllingUnknown() );
			if( ptrThis )
			{
				CString strActiveQuery = ::_GetValueString( ptrThis, 				 
											SECTION_DBASE, KEY_ACTIVE_QUERY, "" );

				IUnknown* punk = ::GetObjectByName( ptrThis, strActiveQuery, szTypeQueryObject );
				if( punk )
				{
					ptrThis->SetActiveQuery( punk );
					punk->Release();
				}				
			}
		}
	}

	
		
	if(!InitDBaseEngine())
		return false;
	

	IDBaseDocumentPtr ptrThis( GetControllingUnknown() );
	

	if( ptrThis )
	{
		IDataContext2Ptr ptrC( GetControllingUnknown() );	

		//last view
		CString strLastProgID = ::_GetValueString( ptrThis, SECTION_DBASE, KEY_DEFAULT_VIEW, "" );
		if( !strLastProgID.IsEmpty() )
		{
			::SetValue( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, "DefaultView", szBlankViewProgID/*strLastProgID*/ );			
		}


		IDBaseFilterHolderPtr prtF( GetControllingUnknown() );
		if( prtF )
		{
			CString strFilter = ::_GetValueString( ptrThis, SECTION_DBASE, KEY_ACTIVE_FILTER, "" );
			bool bApply = ( 1L == ::_GetValueInt( ptrThis, SECTION_DBASE, KEY_FILTER_APLY, 0 ) );

			CString strCustom;
			strCustom.LoadString( IDS_FILTER_CUSTOM );

			if( strFilter != strCustom && bApply && !strFilter.IsEmpty() )
			{
				prtF->SetActiveFilter( _bstr_t( (LPCSTR)strFilter ) );								
			}
		}



		IUnknown* pUnkQuery = NULL;		
		ptrThis->GetActiveQuery( &pUnkQuery );
		
		sptrISelectQuery spQuery( pUnkQuery );
		if( pUnkQuery )
			pUnkQuery->Release();	pUnkQuery = 0;


		if( spQuery )
		{						

			BOOL is_open = FALSE;
			spQuery->IsOpen( &is_open );

			if( !is_open )
				spQuery->Open();

			/*
			long lRecord = ::_GetValueInt( ptrThis, SECTION_DBASE, KEY_CURRENT_RECORD, -1 );
			if( lRecord > 0 )
			{
				spQuery->GoToRecord( lRecord );
			}
			*/

			//unselect all objects
			{
				long lCount = 0;
				VERIFY( S_OK == ptrC->GetObjectTypeCount( &lCount ) );
				for( long l=0;l<lCount;l++ )
				{
					BSTR bstrTypeName = 0;
					VERIFY( S_OK == ptrC->GetObjectTypeName( l, &bstrTypeName ) );
					VERIFY( S_OK == ptrC->UnselectAll( bstrTypeName ) );

					if( bstrTypeName )
						::SysFreeString( bstrTypeName );	bstrTypeName = 0;
				}
			}
			
			//m_bNeedActivateQueryInView = true;

		}
	}		
	

	ReBuildFilterChooserList();		


	/*
	IDocumentSitePtr ptrDS( GetControllingUnknown() );
	if( ptrDS )
	{
		IUnknown* punkView = 0;
		ptrDS->GetActiveView( &punkView );
		if( punkView )
		{
			if( ActivateQueryInBlank( punkView ) )
				m_bNeedActivateQueryInView = false;

			punkView->Release();
		}
	}
	*/

	return true;	
}


/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::WriteFile( const char *pszFileName )
{	
	//save active query name
	{
		IDBaseDocumentPtr ptrThis( GetControllingUnknown() );
		if( ptrThis )
		{
			IUnknown* punk = 0;
			ptrThis->GetActiveQuery( &punk );						
			ISelectQueryPtr ptrQ;

			if( punk )
			{	
				ptrQ = punk;
				punk->Release();	punk = 0;
			}

			if( ptrQ )
			{

				::_SetValue( ptrThis, SECTION_DBASE, KEY_ACTIVE_QUERY, ::GetObjectName( ptrQ ) );

				BOOL bOpen = FALSE;
				ptrQ->IsOpen( &bOpen );				
				::_SetValue( ptrThis, SECTION_DBASE, KEY_QUERY_OPEN, (long)bOpen );

				long lCurRecord = 0;
				ptrQ->GetCurrentRecord( &lCurRecord );
				::_SetValue( ptrThis, SECTION_DBASE, KEY_CURRENT_RECORD, (long)lCurRecord );
			}				

			IDBaseFilterHolderPtr prtF( GetControllingUnknown() );
			if( prtF )
			{

				BOOL bApply = FALSE;
				prtF->GetIsActiveFilterApply( &bApply );

				::_SetValue( ptrThis, SECTION_DBASE, KEY_FILTER_APLY, (long)bApply );				

				CString strFilter;

				BSTR bstrFilter = 0;
				prtF->GetActiveFilter( &bstrFilter );

				strFilter = bstrFilter;

				if( bstrFilter )
					::SysFreeString( bstrFilter );	bstrFilter = 0;

				if( !bApply )
					strFilter.Empty();

				::_SetValue( ptrThis, SECTION_DBASE, KEY_ACTIVE_FILTER, strFilter );

			}

			CString strViewProgID;
			IDocumentSitePtr ptrSite( GetControllingUnknown() );
			if( ptrSite )
			{
				IUnknown* punk = 0;
				ptrSite->GetActiveView( &punk );				

				IPersistPtr ptrP;
				if( punk )
				{
					ptrP = punk;
					punk->Release();	punk = 0;
				}

				if( ptrP )
				{
					GUID guid = INVALID_KEY;
					ptrP->GetClassID( &guid );
					
					BSTR bstrProgID = 0;

					::ProgIDFromCLSID( guid, &bstrProgID );
					
					strViewProgID = bstrProgID;
					
					::CoTaskMemFree( bstrProgID );	bstrProgID = 0;
				}					
			}

			::_SetValue( ptrThis, SECTION_DBASE, KEY_DEFAULT_VIEW, strViewProgID );
		}
	}


	m_filterHolder.StoreToData( GetControllingUnknown() );


	if( !IsValidNameData() ) return false;	
	

	try
	{
		{
			CTimeTest timeTest(true, "PrepareWriteData DBaseDocument:");
			timeTest.m_bEnableFileOutput = false;
			VERIFY( PrepareWriteData( "" ) );
		}


		IUnknown* punkRecordND = GetRecordNamedData( );
		sptrINamedData spRecordND( punkRecordND );
		punkRecordND->Release();
		

		IUnknown* punkDocND = GetDocNamedData( );

		{

			long lBinaryFormat = ::_GetValueInt( ::GetAppUnknown(), 
								SHELL_DATA_DB_SECTION, SHELL_DATA_BINARY_DOC_FORMAT, 1 );

			INamedDataPtr ptrND( punkDocND );
			if( ptrND )
			{
				ptrND->EnableBinaryStore( lBinaryFormat ? true : false );
			}
		}
		
		sptrIFileDataObject spFO( punkDocND );
		punkDocND->Release();

		if( spFO == NULL )
			return false;


		spFO->SaveFile( _bstr_t( pszFileName ) );
		
	}
	catch(CException* e)
	{
		e->Delete();
		return false;
	}
	return true;
}



bool CDBaseDocument::PrepareWriteData(LPCTSTR path)
{
	
	try
	{
		IUnknown* punkRecordND = GetRecordNamedData( );
		IUnknown* punkDocND = GetDocNamedData( );

	   	sptrINamedData spRecordND( punkRecordND );
		sptrINamedData spDocND( punkDocND );

		if( punkRecordND )
			punkRecordND->Release();

		if( punkDocND )
			punkDocND->Release();

		if( spDocND == NULL || spRecordND == NULL )
			return false;

		sptrINamedData sptrSource;
		sptrINamedData sptrTarget;


		sptrSource = spRecordND;
		sptrTarget = spDocND;
		
		CString CurrentPath((!path) ? "" : path);
		_bstr_t bstrPath(CurrentPath);


		// set section
		IS_OK( sptrSource->SetupSection(bstrPath) );
		
		// get count of entries
		long EntryCount = 0;
		IS_OK( sptrSource->GetEntriesCount(&EntryCount) );

		// for all entries
		for (int  i = 0; i < (int)EntryCount; i++)
		{
			try
			{
				BSTR bstr = NULL;
				// get entry name
				
				IS_OK( sptrSource->GetEntryName(i, &bstr) );

				// destructor will release system memory from bstr
				_bstr_t bstrName(bstr, false);
				
				// format a full path for entry
				_bstr_t bstrPathNew = bstrPath + "\\" + bstrName;
		
				// get value
				_variant_t var;
				IS_OK( sptrSource->GetValue(bstrPathNew, &var) );
				
				//bool bOurObjectOrSimpleData = IsOwnDataObject(var);

				//Section values
				CString strPath = (char*)bstrPath;	strPath += "\\";
				CString strEntry = (char*)bstrName;

				bool bRecordNameDataEntry = ( strPath.Find( (LPCSTR)SECTION_DBASERECORDNAMEDDATA ) != -1 );

				if( !bRecordNameDataEntry /*&& bOurObjectOrSimpleData*/ && (var.vt != VT_NULL/*cos trouble when serialize VT_NULL*/) )
				{
					sptrTarget->SetupSection(bstrPathNew);
					IS_OK( sptrTarget->SetValue(bstrPathNew, var) );
				}
				
				// if this entry has children => we want to walk to them
				if (EntryCount > 0)
					PrepareWriteData( bstrPathNew );

				// for next entry on this level restore Section
				IS_OK( sptrSource->SetupSection(bstrPath) );			



			}
			catch(...)
			{
				continue;
			}

		}

	}
	catch(...)
	{		
		return false;
	}

	return true;

}


void CDBaseDocument::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{	
	CDocBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );

	if( !strcmp( pszEvent, szEventBeforeClose ) && 
		::GetObjectKey( punkFrom ) == ::GetObjectKey( GetControllingUnknown() ) )
	{
		ISelectQueryPtr ptrQ = GetActiveQuery();
		if( ptrQ )
		{
			BOOL bOpen = FALSE;
			ptrQ->IsOpen( &bOpen );				

			if( bOpen )
				ptrQ->Update();								
		}		
	}
	else if( !strcmp( pszEvent, "OnOpenDocument" ) || !strcmp( pszEvent, "OnNewDocument" ) )
	{
 		if( !strcmp( pszEvent, "OnOpenDocument" ) && GetStealthKeysNumber() && ::StealthIndexIsAvailable( 30 ) )
		{
			::ExecuteAction( "DeleteRecords", "0" );
		}

		IDocumentSitePtr ptrDS( GetControllingUnknown() );
		if( ptrDS )
		{
			IUnknown* punkView = 0;
			ptrDS->GetActiveView( &punkView );
			if( punkView )
			{
				IBlankViewPtr sptrBlank = punkView;
				
				if( sptrBlank == 0 )
					::ExecuteAction( "ShowBlankPreview", "" );

				if( ActivateQueryInBlank( punkView ) )
					m_bNeedActivateQueryInView = false;

				punkView->Release();
			}
		}

	}
	
	if( !strcmp( pszEvent, szEventAfterOpen ) )
	{
		if( m_bNeedSetModifiedFlagAfterOpen )
		{
			::SetModifiedFlagToDoc( GetControllingUnknown() );
			m_bNeedSetModifiedFlagAfterOpen = false;
		}
	}

	if( !strcmp( pszEvent, szAppActivateView ) )
	{
		UpdateRecordPaneInfo( 0 );

		IViewPtr ptrView( punkFrom );
		IUnknown* punkDoc = 0;
		if( ptrView )
			ptrView->GetDocument( &punkDoc );

		if( punkDoc )
		{
			if( GetObjectKey( GetControllingUnknown() ) == ::GetObjectKey(punkDoc) )
				ReBuildFilterChooserList();	
			
			punkDoc->Release();
			punkDoc = 0;
		}
		
	}
	
	if( !strcmp( pszEvent, szActivateView ) )
	{
		if(this->m_dbEngine.m_pdbLocks)
		this->m_dbEngine.m_pdbLocks->UpdateRecordStatus(TRUE);
		/*IDocumentSitePtr ptrDS( GetControllingUnknown() );
		if( ptrDS )
		{
			IUnknown* punkView = 0;
			ptrDS->GetActiveView( &punkView );
			if( punkView )
			{
				IWindow2Ptr wnd(punkView);
				HANDLE hWnd;
				wnd->GetHandle(&hWnd);
				punkView->Release();
			}
		}*/
		//OnActivateObject( punkFrom, punkHit );

		/*
		if( m_bNeedActivateQueryInView )
		{						
			if( ActivateQueryInBlank( punkFrom ) )
				m_bNeedActivateQueryInView = false;
		}
		else if( m_bNeedActivateSpecificView )
		{
			m_bNeedActivateSpecificView = false;
			*/

			/*
			{
				sptrIQueryObject ptrQ( GetActiveQuery() );
				if( ptrQ )
				{
					IDataContext2Ptr ptrC( punkFrom );	
					if( ptrC )
					{						
						ptrC->SetObjectSelect( ptrQ, 1 );
					}
				}
			}			
			*/
		//}
	}	
	else if( !strcmp( pszEvent, "AfterClose" ) )
	{
		if( !IsOtherDBaseDocExist() )
		{
			DestroyRecordPaneInfo();
		}
	}	
		
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::ActivateQueryInBlank( IUnknown* punkView )
{
	IBlankViewPtr ptrV( punkView );
	if( ptrV == 0 )
		return false;

	sptrIQueryObject ptrQ( GetActiveQuery() );
	if( ptrQ )
	{
		IDataContext2Ptr ptrC( ptrV );	
		if( ptrC )
		{						
			ptrC->SetObjectSelect( ptrQ, 1 );
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::IsOtherDBaseDocExist()
{

	GuidKey guidThis = ::GetObjectKey( GetControllingUnknown() );

	sptrIApplication sptrA( GetAppUnknown( ) );
	if( sptrA == NULL )
		return false;
	

	long	lPosTemplate = 0;	
	sptrA->GetFirstDocTemplPosition( &lPosTemplate );	
	while( lPosTemplate )
	{
		long	lPosDoc = 0;
		sptrA->GetFirstDocPosition( lPosTemplate, &lPosDoc );
		while( lPosDoc )
		{
			IUnknown *punkDoc = 0;
			sptrA->GetNextDoc( lPosTemplate, &lPosDoc, &punkDoc );

			if( !punkDoc )
				continue;

			IUnknownPtr ptrDoc = punkDoc;
			punkDoc->Release();	punkDoc = 0;

			if( !CheckInterface( ptrDoc, IID_IDBaseDocument) )
				continue;

			GuidKey guidDocKey = ::GetObjectKey( ptrDoc );

			if( guidDocKey != guidThis )
				return true;
		}
		sptrA->GetNextDocTempl( &lPosTemplate, 0, 0 );
	} 

	return false;
}

static bool IsDBView(IUnknown *punk)
{
	INamedObject2Ptr sptrNO(punk);
	if (sptrNO == 0) return false;
	_bstr_t bstrName;
	sptrNO->GetName(bstrName.GetAddress());
	if (bstrName == _bstr_t(c_szCBlankView) || bstrName == _bstr_t(c_szCGalleryView) ||
		bstrName == _bstr_t(c_szCDBaseGridView))
		return true;
	else
		return false;
}

static void RestoreSelectedObjects(IUnknown *punkDoc, IUnknown *punkView)
{
	IDataContext2Ptr sptrCtx(punkView);
	IDBaseDocumentPtr sptrDBDoc(punkDoc);
	CPtrArray arrBaseObjects;
	long lTypes;
	sptrCtx->GetObjectTypeCount(&lTypes);
	for (long l = 0; l < lTypes; l++)
	{
		_bstr_t bstrTypeName;
		sptrCtx->GetObjectTypeName(l, bstrTypeName.GetAddress());
		long lPos;
		sptrCtx->GetFirstSelectedPos(bstrTypeName, &lPos);
		while (lPos)
		{
			IUnknownPtr punkObj;
			sptrCtx->GetNextSelected(bstrTypeName, &lPos, &punkObj);
			arrBaseObjects.Add((IUnknown*)punkObj);
		}
	}
	for (int i = 0; i < arrBaseObjects.GetSize(); i++)
	{
		IUnknown *punkBaseObj = (IUnknown *)arrBaseObjects.GetAt(i);
		GUID guidBaseObject = ::GetObjectKey(punkBaseObj);
		for (l = 0; l < lTypes; l++)
		{
			_bstr_t bstrTypeName;
			sptrCtx->GetObjectTypeName(l, bstrTypeName.GetAddress());
			long lPos;
			sptrCtx->GetFirstObjectPos(bstrTypeName, &lPos);
			while (lPos)
			{
				IUnknownPtr punkObj;
				sptrCtx->GetNextObject(bstrTypeName, &lPos, &punkObj);
				GUID guid_db_parent = INVALID_KEY;
				BOOL bdb_object = FALSE;
				BOOL bempty = FALSE;
				sptrDBDoc->IsDBaseObject(punkObj, &bdb_object, &guid_db_parent, &bempty);
				if( ::GetObjectKey(punkObj) != guidBaseObject && guid_db_parent == guidBaseObject &&
					!bempty && bdb_object)
				{
					sptrCtx->SetObjectSelect(punkObj, TRUE);
				}
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
void CDBaseDocument::OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var )
{
	if (!strcmp(pszEvent, szDBaseEventAfterNavigation))
	{
		IDocumentSitePtr sptrDS(GetControllingUnknown());
		long lPos;
		sptrDS->GetFirstViewPosition(&lPos);
		while (lPos)
		{
			IUnknownPtr punkView;
			sptrDS->GetNextView(&punkView, &lPos);
			if (IsDBView(punkView)) continue;
			RestoreSelectedObjects(GetControllingUnknown(), punkView);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	return true;
}

/*
/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::NewDatabase()
{
	CDatabaseDlg dlg( ::GetMainFrameWnd() );
	if( dlg.DoModal( ) != IDOK )
		return false;	
	
	if( dlg.m_strDBPath.Find( ".mdb" ) == -1 )
		dlg.m_strDBPath += ".mdb";

	bool bAnswerOverride = true;

	
	bool bResult = m_dbEngine.CreateDatabase( dlg.m_strDBPath, dlg.m_strTableName, 
					(dlg.m_nTemplate == 0), dlg.m_strTemplateLocation, bAnswerOverride );

	if( !bResult )
		return bResult;


	//CTableDlg dlgTable( ::GetMainFrameWnd() );		
	//dlgTable.Init( &m_dbEngine, dlg.m_strTableName );	

	CDBStructDlg dlgStruct( ::GetMainFrameWnd() );
	dlgStruct.m_strTable = dlg.m_strTableName;
	dlgStruct.SetDBaseEngine( &m_dbEngine );
	
	int nResult = dlgStruct.DoModal( );
	if( nResult != IDOK )
	{
		return false;
	}

	
	sptrIDBaseDocument spDBDoc( GetControllingUnknown() );
	if( spDBDoc == NULL )
		return false;


	spDBDoc->Generate( _bstr_t((LPCTSTR)szEventOnAddTable), _bstr_t((LPCTSTR)dlg.m_strTableName) );





	IUnknown* pUnkQuery = NULL;
	spDBDoc->GetActiveQueryUnknown( &pUnkQuery );

	if( pUnkQuery == NULL )
		return true;

	sptrISelectQuery spQuery( pUnkQuery );
	pUnkQuery->Release();

	if( spQuery == NULL )
		return true;

	spQuery->Open();


	m_bNeedActivateActiveXFrom = true;


	return bResult;
	
}



/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::QueryWizard()
{
	CQueryDlg dlg( ::GetMainFrameWnd() );
	
	CArray<_CTableInfo*, _CTableInfo*>* pArrTableInfo;

	m_dbEngine.GetTablesInfo( &pArrTableInfo, false );
	dlg.SetTableInfoArr( pArrTableInfo );

	if( dlg.DoModal( ) == IDOK )
	{
		//m_strActiveQuery = dlg.GetActiveQuery();
	}

	AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
	::ExecuteAction( GetActionNameByResID( IDS_REQUERY ) );

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
bool CDBaseDocument::ChoiseQuery()
{
	CString strOldActiveQuery = GetActiveQueryName();
	IUnknown* pOldQuery = NULL;
	IUnknown* pNewQuery = NULL;

	sptrIDBaseDocument spDBaseDoc( GetControllingUnknown() );
	if( spDBaseDoc == NULL )
		return false;

	spDBaseDoc->GetQueryUnknownByName( _bstr_t( (LPCTSTR)strOldActiveQuery ), &pOldQuery );	

	CChoiceActiveQuery dlg( ::GetMainFrameWnd() );
	dlg.SetDBaseDocument( GetControllingUnknown() );
	if( dlg.DoModal() ==IDOK )
	{
		if( !dlg.m_strActiveQuery.IsEmpty() )
		{
			SetActiveQueryName( dlg.m_strActiveQuery );
			spDBaseDoc->GetQueryUnknownByName( _bstr_t( (LPCTSTR)GetActiveQueryName() ), 
												&pNewQuery );
		}												
		sptrISelectQuery spOldQuery( pOldQuery );
		sptrISelectQuery spNewQuery( pNewQuery );		

		if( spOldQuery != NULL  )
		{
			spOldQuery->Close();
			sptrIQueryObject spQ( spOldQuery );
			if( spQ ) spQ->SetActiveFilter( _bstr_t("") );
		}

		if( spNewQuery != NULL  )
		{
			sptrIQueryObject spQ( spNewQuery );
			if( spQ ) spQ->SetActiveFilter( _bstr_t("") );
			spNewQuery->Open();			
		}

	}

	if( pOldQuery )
		pOldQuery->Release();

	if( pNewQuery )
		pNewQuery->Release();

	return true;
}

*/

/////////////////////////////////////////////////////////////////////////////////////////
void CDBaseDocument::FireSimpleEvent( const char* szEvent )
{
	_variant_t var;
	m_eventController.FireEvent( szEvent, NULL, NULL, NULL, NULL, var );	
}

/////////////////////////////////////////////////////////////////////////////////////////
CString CDBaseDocument::GetMDBFileNameFromConnectionCtring( CString strConnectionString,  CString strDefPath )
{
	strConnectionString.MakeLower();
	int nPos = strConnectionString.Find( ".mdb" );	
	if( nPos == -1 )
		return "";

	CString strResult;

	bool bEnough = false;
	int nIndex = nPos + 3;
	while( !bEnough )
	{
		strResult += strConnectionString[nIndex];
		nIndex--;
		if( nIndex == -1 || ( nIndex > 0 && strConnectionString[nIndex] == '=' ) )
			bEnough = true;
	}

	strResult.MakeReverse( );
	
	//[max]: отнотительные пути в БД
	
	if( strResult.Find( ":\\") == -1 && strResult.Find( "\\\\") == -1 )
	{
		int r = strDefPath.ReverseFind( '\\' );
		strDefPath = strDefPath.Left( r + 1 );
		strResult = strDefPath + strResult;
	}
 
//	strResult = 
	return strResult;	

}

/*
bool CDBaseDocument::GetTableFieldFromSection( CString strSection, CString& strTable, CString& strField )
{
	int nPoint = strSection.Find( "." );
	if( nPoint == -1 )
		return false;

	if( strSection.GetLength() < 2 )
		return false;

	bool bEnough = false;
	strField = strSection.Right( strSection.GetLength() - nPoint - 1 );

	int nIndex = nPoint-1;
	while( !bEnough )
	{
		if( nIndex == 0 || strSection[nIndex] == '\\' )
		{
			bEnough = true;
			continue;
		}
		strTable += strSection[nIndex];
		nIndex--;
	}

	strTable.MakeReverse();

	return true;
}
*/


/////////////////////////////////////////////////////////////////////////////
IQueryObjectPtr CDBaseDocument::GetActiveQuery()
{
	IDBaseDocumentPtr ptrThis( GetControllingUnknown() );
	if( ptrThis == 0 )
		return 0;

	IUnknown* punk = 0;
	ptrThis->GetActiveQuery( &punk );
	if( !punk )
		return 0;

	IQueryObjectPtr ptrQ( punk );
	punk->Release();

	return ptrQ;
}

/////////////////////////////////////////////////////////////////////////////
CString CDBaseDocument::GetConnectionString()
{
	return _GetValueString( GetControllingUnknown(), SECTION_SETTINGS, CONNECTION_STRING, "" );
}

/////////////////////////////////////////////////////////////////////////////
CString CDBaseDocument::GetsecurityConnectionString()
{
	return GetConnectionString();
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseDocument::SetConnectionString( CString strConn )
{
	_SetValue( GetControllingUnknown(), SECTION_SETTINGS, CONNECTION_STRING, strConn );
}



/////////////////////////////////////////////////////////////////////////////
void CDBaseDocument::SetActiveImage( CString srtActiveObject )
{
	CString strTable, strField;
	
	::GetTableFieldFromPath( srtActiveObject, strTable, strField );

	sptrIQueryObject spQuery = GetActiveQuery();
	if( spQuery == NULL )
		return;

	if( !IsAvailableFieldInQuery( spQuery, strTable, strField )  )
		return;

	CString strEntry;
	strEntry.Format( "%s.%s", (LPCTSTR)strTable, (LPCTSTR)strField );
	CString strVTObjectType;
	
	strVTObjectType = ::_GetValueString( GetControllingUnknown(), SECTION_FIELD_VTOBJECT_TYPE, strEntry, "" );
	if( strVTObjectType != szTypeImage )
		return;

	m_strActiveImage = srtActiveObject;
}

/////////////////////////////////////////////////////////////////////////////
CString CDBaseDocument::GetActiveImage( )
{
	sptrIQueryObject spQuery = GetActiveQuery();

	sptrISelectQuery spSelectQuery( spQuery );

	if( spQuery == NULL || spSelectQuery == NULL )
		return "";

	CString strTable, strField;
	::GetTableFieldFromPath( m_strActiveImage, strTable, strField );

	if( ::IsAvailableFieldInQuery( spQuery, strTable, strField ) )
	{
		if( ::GetFieldVTObjectType( GetControllingUnknown(), strTable, strField ) == szTypeImage )
			return m_strActiveImage;
	}

	strTable.Empty();
	strField.Empty();

	//Try get from tumbnail
	{
		BSTR bstrTable, bstrField;
		spSelectQuery->GetTumbnailLocation( &bstrTable, &bstrField );
		
		strTable = bstrTable;
		strField = bstrField;

		if( bstrTable )
			::SysFreeString( bstrTable );

		if( bstrField )
			::SysFreeString( bstrField );

		if( ::IsAvailableFieldInQuery( spQuery, strTable, strField ) )
		{
			if( ::GetFieldVTObjectType( GetControllingUnknown(), strTable, strField ) == szTypeImage )
				return ::GenerateFieldPath( strTable, strField, false );
		}

		
	}
	
	//try get from query

	int nCount = -1;
	spQuery->GetFieldsCount( &nCount );
	for( int i=0;i<nCount;i++ )
	{		
		BSTR bstrTable, bstrField;
		bstrTable = bstrField = NULL;

		if( S_OK != spQuery->GetField( i, 0, &bstrTable, &bstrField ) )
			continue;
		
		strTable.Empty();
		strField.Empty();
		
		strTable = bstrTable;
		strField = bstrField;

		if( bstrTable )
			::SysFreeString( bstrTable );

		if( bstrField )
			::SysFreeString( bstrField );

		if( ::IsAvailableFieldInQuery( spQuery, strTable, strField ) )
		{
			if( ::GetFieldVTObjectType( GetControllingUnknown(), strTable, strField ) == szTypeImage )
				return ::GenerateFieldPath( strTable, strField, false );
		}
	}

	return "";
}


////////////////////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseDocument::XThumbnailSurface::Draw( BYTE* pbi, HDC hdc, 
												RECT rcTarget, SIZE sizeThumbnail, 
												IUnknown* punkDataObject )
{
	_try_nested(CDBaseDocument, ThumbnailSurface, Draw)
	{	
		if( !punkDataObject )
			return S_FALSE;

		IDBaseDocumentPtr ptrDBDoc( pThis->GetControllingUnknown() );
		if( ptrDBDoc == 0 )
			return S_FALSE;

		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
		
		HICON hIcon = 0;		

		IUnknownPtr ptrActQ;
		IUnknown* punkActiveQuery = 0;
		ptrDBDoc->GetActiveQuery( &punkActiveQuery );
		if( punkActiveQuery )
		{
			ptrActQ = punkActiveQuery;
			punkActiveQuery->Release();	punkActiveQuery = 0;
		}

		if( ::GetObjectKey( ptrActQ ) == ::GetObjectKey( punkDataObject ) )
		{
			hIcon = AfxGetApp()->LoadIcon( IDI_ACTIVE_QUERY );
		}
		else
		{
			GUID guidRootObject = INVALID_KEY;
			BOOL bDBaseObject = FALSE;
			ptrDBDoc->IsDBaseObject( punkDataObject, &bDBaseObject, &guidRootObject, 0 );
			if( bDBaseObject )
			{
				if( ::GetObjectKey( punkDataObject ) == guidRootObject )
					hIcon = AfxGetApp()->LoadIcon( IDI_DBASE_OBJECT_PARENT );
				else
					hIcon = AfxGetApp()->LoadIcon( IDI_DBASE_OBJECT_CHILD );
			}
		}		
		
		if( hIcon )
		{
			::DrawIconEx( hdc, rcTarget.left, rcTarget.top, hIcon, 16, 16, 0, 0, DI_NORMAL );
			VERIFY(::DestroyIcon( hIcon ));
		}
		

		return S_OK;		
		
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseDocument::SetValue(LPCTSTR Entry, const VARIANT FAR& Value)
{
	m_xData.SetValue( _bstr_t( Entry ), Value );
}

/////////////////////////////////////////////////////////////////////////////
VARIANT CDBaseDocument::GetValue(LPCTSTR Entry)
{
	_variant_t var;
	m_xData.GetValue( _bstr_t( Entry ), &var );

	return var.Detach();
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseDocument::SetSection(LPCTSTR Section)
{
	m_xData.SetupSection( _bstr_t( Section ) );
}

/////////////////////////////////////////////////////////////////////////////
long CDBaseDocument::GetEntriesCount()
{
	long lcount = 0;
	m_xData.GetEntriesCount( &lcount );

	return lcount;
}

/////////////////////////////////////////////////////////////////////////////
BSTR CDBaseDocument::GetEntryName(long index)
{
	BSTR bstr = 0;
	m_xData.GetEntryName( index, &bstr );

	return bstr;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseDocument::RemoveValue(LPCTSTR szPath)
{
	m_xData.DeleteEntry( _bstr_t( szPath ) );
}

/////////////////////////////////////////////////////////////////////////////
// CMSAccessFileFilter

IMPLEMENT_DYNCREATE(CMSAccessFileFilter, CCmdTargetEx)

// {5D75060A-C12E-4faf-88D3-EA54A4725D48}
GUARD_IMPLEMENT_OLECREATE(CMSAccessFileFilter, "FileFilters.MSAccessFileFilter", 
0x5d75060a, 0xc12e, 0x4faf, 0x88, 0xd3, 0xea, 0x54, 0xa4, 0x72, 0x5d, 0x48);

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMSAccessFileFilter::CMSAccessFileFilter()
{
	_OleLockApp( this );
	m_strDocumentTemplate = szDocumentDBase;
	m_strFilterName.LoadString( IDS_MSACCESS_FILTERNAME );
	m_strFilterExt = _T(".mdb");
	//AddDataObjectType(szTypeQueryObject);
}

//////////////////////////////////////////////////////////////////////
CMSAccessFileFilter::~CMSAccessFileFilter()
{
	_OleUnlockApp( this );
}

//////////////////////////////////////////////////////////////////////
bool CMSAccessFileFilter::ReadFile( const char *pszFileName )
{
	CFileFind ff;
	if( !ff.FindFile( pszFileName ) )
		return false;

	sptrIApplication spApp( ::GetAppUnknown() );
	if( spApp == NULL )
		return false;


	IDBConnectionPtr spActiveDBaseDoc( m_sptrINamedData );


	//Attach MDB file to active document
	if( spActiveDBaseDoc )
	{
		_bstr_t bstrMDBFilePath( pszFileName );
		spActiveDBaseDoc->LoadAccessDBase( bstrMDBFilePath );
		return true;
	}

	


	return true;
}

//////////////////////////////////////////////////////////////////////
bool CMSAccessFileFilter::WriteFile( const char *pszFileName )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	AfxMessageBox( IDS_WARNING_CANT_WRITE_MDB_FILE, MB_ICONSTOP );
	return false;
}

//////////////////////////////////////////////////////////////////////
bool CMSAccessFileFilter::BeforeReadFile()
{
	return CFileFilterBase::BeforeReadFile();
}

//////////////////////////////////////////////////////////////////////
bool CMSAccessFileFilter::AfterReadFile()
{
	return CFileFilterBase::AfterReadFile();
}

//////////////////////////////////////////////////////////////////////



