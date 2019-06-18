// DBaseEngine.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "docman.h"
#include "DBaseEngine.h"

/////////////////////////////////////////////////////////////////////////////
// CDBaseEngine

IMPLEMENT_DYNCREATE(CDBaseEngine, CCmdTarget)

CDBaseEngine::CDBaseEngine()
{
	m_pdbLocks = 0;
}
/////////////////////////////////////////////////////////////////////////////
CDBaseEngine::CDBaseEngine( CDBaseDocument* pDocument )
{
	m_pDocument			= pDocument;	
	m_bDirtyTableInfo	= true;
	m_pdbLocks = 0;
}

/////////////////////////////////////////////////////////////////////////////
CDBaseEngine::~CDBaseEngine()
{
	bool bneed_compact = ( m_connPtr != 0 );
	bneed_compact &= ( 1L == ::GetValueInt( ::GetAppUnknown(), SHELL_DATA_DB_SECTION, SHELL_DATA_COMPACT, 0 ) );
	DestroyTableInfoCache();
	Close();

	if( bneed_compact && m_pDocument && !m_pDocument->m_bReadOnly && m_str_conn.GetLength() )
	{
		CWaitCursor wait;
		CString strMdbFileName = CDBaseDocument::GetMDBFileNameFromConnectionCtring( m_str_conn, m_pDocument->m_str_dbd_open_file_name );		
		if( strMdbFileName.GetLength() )
			CompactMDB( strMdbFileName );
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::CreateConnectionObject()
{

	try
	{
		if( m_connPtr == 0 )
		{
			if( !SUCCEEDED( m_connPtr.CreateInstance( __uuidof(ADO::Connection) ) ) )		
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				AfxMessageBox( IDS_WARNING_CONNECTION_OBJECT_FAILED, MB_ICONSTOP | MB_OK );
				return false;
			}
		}

		if( m_catalogPtr == 0 )
		{
			if( S_OK != m_catalogPtr.CreateInstance(__uuidof (ADOX::Catalog) ) )
			{
				AFX_MANAGE_STATE(AfxGetStaticModuleState());
				AfxMessageBox( IDS_WARNING_CATALOG_OBJECT_FAILED, MB_ICONSTOP | MB_OK );		 
				return false;
			}					
		}

	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return false;
	}


	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::IsConnectionOK()
{
	if( m_connPtr == 0 || m_catalogPtr == 0 )
		return false;


	long _result = -1;
	if( S_OK != m_connPtr->get_State( &_result ) )
		return false;
	
	return _result == (long)ADO::adStateOpen;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::IsCatalogOK()
{
	if( m_catalogPtr == 0 )
		return false;

	_variant_t var;
	HRESULT hRes = m_catalogPtr->get_ActiveConnection( &var );
	if( hRes != S_OK || var.vt != VT_DISPATCH )
		return false;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::Close()
{
	m_bDirtyTableInfo = true;

	if( m_connPtr == 0 )
		return false;


	long _result = -1;
	if( S_OK != m_connPtr->get_State( &_result ) )
		return false;

	if( _result == (long)ADO::adStateOpen )
		VERIFY( S_OK == m_connPtr->raw_Close() );


	m_catalogPtr	= 0;
	m_connPtr		= 0;

	m_pDocument->FireSimpleEvent( szDBaseEventConnectionClose );
	delete m_pdbLocks;
	m_pdbLocks = 0;
	return true;
}

/////////////////////////////////////////////////////////////////////////////
IUnknown* CDBaseEngine::GetConnection()
{
	if( !IsConnectionOK() )
		return 0;

	m_connPtr->AddRef();

	return (IUnknown*)m_connPtr;
}

CString SetConnectionProperty( const char* psz_coonnection, const char* psz_property, const char* psz_value )
{
	if( !psz_coonnection || !psz_property || !psz_value )	return "";

	CString strConnectionString = psz_coonnection;
	int nidx_prop = strConnectionString.Find( psz_property );
	if( nidx_prop != -1 )
	{
		int nidx_last = strConnectionString.Find( ";", nidx_prop );
		if( nidx_last == -1 )
			nidx_last = strConnectionString.GetLength();

		if( nidx_last != -1 )
		{
			int nlen = strConnectionString.GetLength();
			int nlen1 = nidx_prop;
			int nlen2 = nlen - nidx_last;
			//if( nlen2 > 0 )
			{
				CString str1 = strConnectionString.Left( nlen1 );
				CString str2 = strConnectionString.Right( nlen2 );
				CString str_new_conn = str1;
				str_new_conn += psz_property;
				str_new_conn += psz_value;
				str_new_conn += str2;
				strConnectionString = str_new_conn;
			}
		}
	}

	return strConnectionString;

}
/////////////////////////////////////////////////////////////////////////////
//
//		Opening connection
//
/////////////////////////////////////////////////////////////////////////////

bool CDBaseEngine::OpenFromString( CString strConnectionString )
{
	Close();
	DestroyTableInfoCache();

	if( !CreateConnectionObject() )
		return false;

	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	m_str_conn = strConnectionString;
	CString strMdbFileName;
	{				
		
		strMdbFileName = CDBaseDocument::GetMDBFileNameFromConnectionCtring( strConnectionString, m_pDocument->m_str_dbd_open_file_name );		

		if( !strMdbFileName.IsEmpty() )
		{
			CFileFind ff;
			if( !ff.FindFile( strMdbFileName ) )
			{
				CString strFormat;
				strFormat.LoadString( IDS_WARNING_FILE_NOT_FOUND );
				CString strError;
				strError.Format( strFormat, strMdbFileName );

				AfxMessageBox( strError, MB_ICONSTOP );

				if( m_pDocument )
				{
					IDBConnectionPtr ptrDBC( m_pDocument->GetControllingUnknown() );
					if( ptrDBC )
					{
						BSTR bstrMDBFileLocation = 0;
						ptrDBC->GetMDBFileName( &bstrMDBFileLocation );

						CString strMDBFileLocation = bstrMDBFileLocation;

						if( bstrMDBFileLocation )
							::SysFreeString( bstrMDBFileLocation );

						CFileDialog dlg(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 0, 0, _FILE_OPEN_SIZE_ );
						strcpy(dlg.m_ofn.lpstrFile, strMDBFileLocation);
						dlg.m_ofn.lpstrFilter = "MSAccess files(*.mdb)\0*.mdb\0\0";
						if(dlg.DoModal() == IDOK)
						{
							strMDBFileLocation = dlg.GetPathName();		
							HRESULT hr = ptrDBC->LoadAccessDBase( _bstr_t( (LPCTSTR)strMDBFileLocation ) );
							
							::SetModifiedFlagToDoc( ptrDBC );

							m_pDocument->SetNeedSetModifiedFlagAfterOpen( true );

							return ( hr == S_OK );
						}	
						
					}
				}

				return false;
			}			
		}
	}

	try
	{	

		//test read only
		if( m_pDocument )
		{			
			if( m_pDocument->m_bReadOnly )
			{
				CString str_path = m_pDocument->m_str_dbd_open_file_name;
				str_path.MakeLower( );
				int nidx = str_path.Find( ".dbd" );
				if( nidx != -1 )
				{
					str_path.SetAt( nidx + 1, 'm' );
					str_path.SetAt( nidx + 2, 'd' );
					str_path.SetAt( nidx + 3, 'b' );

					strConnectionString = SetConnectionProperty( strConnectionString, "Mode=", "Read" );
					strConnectionString = SetConnectionProperty( strConnectionString, "Data Source=", str_path );
				}
			}
			else
				strConnectionString = SetConnectionProperty( strConnectionString, "Data Source=", CDBaseDocument::GetMDBFileNameFromConnectionCtring( strConnectionString, m_pDocument->m_str_dbd_open_file_name ) );
		}

		_bstr_t ConnectionString( strConnectionString );
		_bstr_t UserID("Admin");
		_bstr_t Password;

		CTimeTest time( true, "Open connection ...:");
		time.m_bEnableFileOutput = false;				
		
		m_connPtr->Open( ConnectionString, UserID, Password, 0 );
		time.Report();

		m_catalogPtr->PutActiveConnection( _variant_t( (IDispatch*)m_connPtr ) );

		long luser_count = ::GetValueInt( ::GetAppUnknown(), "\\Database", "UserCount", 0 );
		try
		{
			if( luser_count > 0 )
			{
				_variant_t var_restr = vtMissing;
				_variant_t var_id = L"{947bb102-5d43-11d1-bdbf-00c04fb92675}";
				ADO::_RecordsetPtr ptr_rs = m_connPtr->OpenSchema( ADO::adSchemaProviderSpecific, var_restr, var_id );
				long lcount = 0;
				while( !ptr_rs->ADOEOF )
				{
					lcount++;
					ptr_rs->MoveNext();
				}				
				if( lcount > luser_count )
				{
					AfxMessageBox( IDS_MANY_USERS, MB_ICONERROR );
					Close();
				}
			}
		}
		catch( _com_error &e )
		{
			dump_com_error(e);
		}
		{//KIR 
			delete m_pdbLocks;
			m_pdbLocks = new CDBLocksInfo;
			if(!m_pdbLocks->AttachToDB(strMdbFileName))
				return false;
			m_pdbLocks->StartNotification(&m_pDocument->m_eventController);
		}
	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return false;
	}


	m_pDocument->FireSimpleEvent( szDBaseEventConnectionOpen );		

	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool CDBaseEngine::OpenAccessFile( CString strMDBFile, CString& strConnectionString )
{
	Close();
	if( !CreateConnectionObject() )
		return false;

	strConnectionString = DB_DEFAULT_CONNECTION_STRING;
	strConnectionString += ";";
	strConnectionString += DB_DATASOURCE;
	if (GetValueInt(GetAppUnknown(), "\\Database", "SaveRelativePath", FALSE))
	{
		TCHAR szDrive[_MAX_DRIVE],szDir[_MAX_DIR];
		TCHAR szDriveDbd[_MAX_DRIVE],szDirDbd[_MAX_DIR];
		TCHAR szPath[_MAX_PATH],szFName[_MAX_FNAME],szExt[_MAX_EXT];
		_tsplitpath(strMDBFile, szDrive, szDir, szFName, szExt);
		_tsplitpath(m_pDocument->m_str_dbd_open_file_name, szDriveDbd,
			szDirDbd, NULL, NULL);
		if (!_tcscmp(szDrive,szDriveDbd) && !_tcscmp(szDir,szDirDbd))
		{
			_tmakepath(szPath, NULL, NULL, szFName, szExt);
			strConnectionString += szPath;
		}
		else
			strConnectionString += strMDBFile;	
	}
	else
		strConnectionString += strMDBFile;	


	return OpenFromString( strConnectionString );
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::OpenAdvancedConnection( CString& strConnectionString )
{
	Close();
	if( !CreateConnectionObject() )
		return false;

	ATL::CDataSource datasource;
	if( S_OK != datasource.Open( AfxGetMainWnd()->GetSafeHwnd() ) )
		return false;

	
	BSTR bstr = 0;
	if( datasource.GetInitializationString( &bstr, true ) != S_OK )
		return false;	

	strConnectionString = bstr;

	if( bstr )
		::SysFreeString( bstr );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//		Table info cache
//
/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::IsDirtyTableInfo()
{
	return m_bDirtyTableInfo;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseEngine::SetDirtyTableInfo( bool bDirty )
{
	m_bDirtyTableInfo = bDirty;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::GetTablesInfo( CArray<_CTableInfo*, _CTableInfo*>** pArrTableInfo, bool bForceReread )
{
	bool bResult;
	if( bForceReread )
		bResult = CreateTableInfoCache();
	else
		if( IsDirtyTableInfo() )
			bResult = CreateTableInfoCache();
		else
			bResult = true;
	
	if( pArrTableInfo != NULL )
		*pArrTableInfo = &m_arrTableInfo;

	return bResult;
}

/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::CreateTableInfoCache()
{
	CWaitCursor wait;

	INamedDataPtr	ptrND( m_pDocument->GetControllingUnknown() );
	if( ptrND == 0 )
		return false;

	DestroyTableInfoCache();	

	if( !IsConnectionOK() )
		return false;

	if( !IsCatalogOK() )
		return false;
	
	try{
		//refresh... 		
		m_catalogPtr = 0;
		if( S_OK != m_catalogPtr.CreateInstance(__uuidof (ADOX::Catalog) ) )
		{
			AfxMessageBox( IDS_WARNING_CATALOG_OBJECT_FAILED, MB_ICONSTOP | MB_OK );		 
			return false;
		}					

		m_catalogPtr->PutActiveConnection( _variant_t( (IDispatch*)m_connPtr ) );
	}
	catch (_com_error &e)
	{
		dump_com_error( e );
		return false;
	}

	CreateTableCacheOnCatalog( m_catalogPtr, ptrND, m_arrTableInfo );

	SetDirtyTableInfo( false );


	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CDBaseEngine::DestroyTableInfoCache()
{
	for( int i=0;i<m_arrTableInfo.GetSize();i++ )
		delete m_arrTableInfo[i];

	m_arrTableInfo.RemoveAll();

	SetDirtyTableInfo( true );
}


/////////////////////////////////////////////////////////////////////////////
bool CDBaseEngine::CreateTableCacheOnCatalog( 
								IUnknown* punkCatalog, 
								IUnknown* punkNamedData,
								CArray<_CTableInfo*, _CTableInfo*>& arrTableInfo
								)
{
	CWaitCursor wait;

	try
	{

		ADOX::_CatalogPtr	catalogPtr = punkCatalog;

		if( catalogPtr == 0 )
			return false;

		_variant_t var;
		HRESULT hRes = catalogPtr->get_ActiveConnection( &var );
		if( hRes != S_OK || var.vt != VT_DISPATCH )
			return false;

		INamedDataPtr	ptrND( punkNamedData );

		ADOX::TablesPtr ptrTables = catalogPtr->Tables;
		if( ptrTables == 0 )
			return false;
		
		ptrTables->Refresh();

		int nTablesCount = ptrTables->Count;
		for( long i=0;i<nTablesCount;i++ )
		{
			ADOX::_TablePtr ptrTable = ptrTables->GetItem( i );
			if( ptrTable == 0 )
				continue;

			
			CString strType = (LPCSTR)ptrTable->Type;
			strType.MakeLower();
			if( strType != "table" )
				continue;				


			_CTableInfo* pti = new _CTableInfo( ::_MakeLower( (LPCSTR)ptrTable->Name ) );
			arrTableInfo.Add( pti );			

			ADOX::ColumnsPtr ptrColumns = ptrTable->Columns;
			if( ptrColumns == 0 )
				continue;
			

			long nColCount = ptrColumns->Count;

			for( long j=0;j<nColCount;j++ )
			{
				
				ADOX::_ColumnPtr ptrColumn = ptrColumns->GetItem( j );
				
				if( ptrColumn == 0 )
					continue;

				_CFieldInfo* pfi = new _CFieldInfo;
				pti->m_arrFieldInfo.Add( pfi );


				pfi->m_strFieldName			= ::_MakeLower( (LPCSTR)ptrColumn->Name );				

				pfi->m_strUserName			= _GetValueString( ptrND, SECTION_FIELD_CAPTIONS, 
												GetFieldEntry( pti->m_strTableName, pfi->m_strFieldName ),
												pfi->m_strFieldName );

				if( pfi->m_strUserName.IsEmpty() )
					pfi->m_strUserName = pfi->m_strFieldName;
				
				ADOX::DataTypeEnum	dt		= ptrColumn->Type;
				
				pfi->m_FieldType			= ftNotSupported;
				if( dt == ADOX::adDouble )
				{
					pfi->m_FieldType	= ftDigit;
				}				
				else if( dt == ADOX::adBSTR || dt == ADOX::adVarChar || dt == ADOX::adLongVarChar || 
					dt == ADOX::adWChar || dt == ADOX::adVarWChar || dt == ADOX::adLongVarWChar )
				{
					pfi->m_FieldType	= ftString;
				}
				else if( dt == ADOX::adDate || dt == ADOX::adDBDate || dt == ADOX::adDBTime || dt == ADOX::adDBTimeStamp )
				{
					pfi->m_FieldType	= ftDateTime;
				}
				else if( dt == ADOX::adLongVarBinary || dt == ADOX::adVarBinary )
				{
					pfi->m_FieldType	= ftVTObject;
				}


				pfi->m_strPrimaryTable		= "";

				pfi->m_strPrimaryFieldName	= "";

				pfi->m_bRequiredValue		= _GetValueInt( ptrND, SECTION_FIELD_REQUIRED_VALUES, 
												GetFieldEntry( pti->m_strTableName, pfi->m_strFieldName ),
												0 ) == 1;
				pfi->m_bDefaultValue		= _GetValueInt( ptrND, SECTION_FIELD_DEFAULT_VALUES_CHECK, 
												GetFieldEntry( pti->m_strTableName, pfi->m_strFieldName ),
												0 ) == 1;

				pfi->m_strDefaultValue		= _GetValueString( ptrND, SECTION_FIELD_DEFAULT_VALUES, 
												GetFieldEntry( pti->m_strTableName, pfi->m_strFieldName ),
												"" );

				pfi->m_strVTObjectType		= _GetValueString( ptrND, SECTION_FIELD_VTOBJECT_TYPE, 
												GetFieldEntry( pti->m_strTableName, pfi->m_strFieldName ),
												"" );

				pfi->m_bCanEditStructure	= FALSE;				
			}

			ADOX::KeysPtr	ptrKeys = ptrTable->Keys;
			if( ptrKeys == 0 )
				continue;

			int nKeysCount = ptrKeys->Count;
			for( j=0;j<nKeysCount;j++ )
			{
				ADOX::_KeyPtr ptrKey = ptrKeys->GetItem( j );
				if( ptrKey == 0 )
					continue;

				ADOX::ColumnsPtr ptrColumns = ptrKey->Columns;
				if( ptrColumns == 0 )
					continue;

				if( ptrColumns->Count < 1 )
					continue;

				ADOX::_ColumnPtr	ptrColumn = ptrColumns->GetItem( (long)0 );
				if( ptrColumn == 0 )
					continue;



				CString strKeyName		= ::_MakeLower( (LPCSTR)ptrColumn->Name );
				CString strPrimaryTable = ::_MakeLower( (LPCSTR)ptrKey->RelatedTable );
				CString strPrimaryField = ::_MakeLower( (LPCSTR)ptrColumn->RelatedColumn );
				
				ADOX::KeyTypeEnum	kte	= ptrKey->Type;

				//now found in our field cache
				for( j=0;j<pti->m_arrFieldInfo.GetSize();j++ )
				{
					_CFieldInfo* pfi = pti->m_arrFieldInfo[j];

					if( pfi->m_strFieldName == strKeyName )
					{
						if( kte == ADOX::adKeyPrimary || kte == ADOX::adKeyUnique )
						{
							pfi->m_FieldType = ftPrimaryKey;
						}
						else if( kte == ADOX::adKeyForeign )
						{
							pfi->m_FieldType			= ftForeignKey;
							pfi->m_strPrimaryTable		= strPrimaryTable;
							pfi->m_strPrimaryFieldName	= strPrimaryField;
						}
					}
				}
			}			
		}

	}
	catch (_com_error &e)
	{
		dump_com_error( e );
		return false;
	}

	return true;
}




/////////////////////////////////////////////////////////////////////////////
CString	CDBaseEngine::GetFieldEntry( CString strTable, CString strField )
{
	CString str;
	str.Format( "%s.%s", (LPCSTR)strTable, (LPCSTR)strField );

	return str;
}

/////////////////////////////////////////////////////////////////////////////
//
//		DDL
//
/////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseEngine::AddTable( BSTR bstrTableName )
{
	try
	{


		_bstr_t _bstrTableName = (LPCSTR)::_MakeLower( bstrTableName );

		if( !IsConnectionOK() )
			return S_FALSE;

		if( !::IsAccessDBase(m_pDocument->GetControllingUnknown() ) )
			return S_FALSE;

		if( !IsCatalogOK() )
			return S_FALSE;

		ADOX::TablesPtr ptrTables = m_catalogPtr->Tables;

		if( ptrTables == 0 )
			return S_FALSE;


		ADOX::_TablePtr ptrTable;
		if( S_OK != ptrTable.CreateInstance(__uuidof (ADOX::Table) ) )
			  return S_FALSE;

		ptrTable->Name = _bstrTableName;
		ptrTables->Append( _variant_t( (IDispatch*)ptrTable ) );

		SetDirtyTableInfo( true );


	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return E_FAIL;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseEngine::DeleteTable( BSTR bstrTableName )
{
	try
	{
		if( !IsConnectionOK() )
			return S_FALSE;

		if( !::IsAccessDBase(m_pDocument->GetControllingUnknown() ) )
			return S_FALSE;

		if( !IsCatalogOK() )
			return S_FALSE;

		ADOX::TablesPtr ptrTables = m_catalogPtr->Tables;

		if( ptrTables == 0 )
			return S_FALSE;

		ptrTables->Delete( _bstr_t( bstrTableName ) );

		SetDirtyTableInfo( true );

	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return E_FAIL;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseEngine::AddField(	BSTR bstrTableName, BSTR bstrFieldName, short nFieldType,
									BSTR bstrPrimaryTable, BSTR bstrPrimaryFieldName )
{
	try
	{
		if( !IsConnectionOK() )
			return S_FALSE;

		if( !::IsAccessDBase(m_pDocument->GetControllingUnknown() ) )
			return S_FALSE;

		if( !IsCatalogOK() )
			return S_FALSE;

		ADOX::TablesPtr ptrTables = m_catalogPtr->Tables;

		if( ptrTables == 0 )
			return S_FALSE;


		ADOX::_TablePtr ptrTable = ptrTables->GetItem( bstrTableName );
		if( ptrTable == 0 )
			return S_FALSE;

		ADOX::ColumnsPtr ptrColumns = ptrTable->Columns;
		if( ptrColumns == 0 )
			return S_FALSE;

		FieldType ft = (FieldType)nFieldType;


		ADOX::DataTypeEnum dt = ADOX::adEmpty;

		CString strFieldType;
		
		if( ft == ftDigit )
			strFieldType = "DOUBLE PRECISION";//dt = ADOX::adDouble;
		else if( ft == ftString )
			strFieldType = "MEMO";//dt = ADOX::adLongVarWChar;
		else if( ft == ftDateTime )
			strFieldType = "TIMESTAMP";//dt = ADOX::adDBTimeStamp;
		else if( ft == ftVTObject )
			strFieldType = "LONGBINARY";//dt = ADOX::adLongVarBinary;		
		else if( ft == ftPrimaryKey )
			strFieldType;
		else if( ft == ftForeignKey )
			strFieldType;
		else		
		{
			return S_FALSE;
		}

		SetDirtyTableInfo( true );

		
//ALTER TABLE Img ADD  column myids AUTOINCREMENT NOT NULL PRIMARY KEY UNIQUE		
/*
ALTER TABLE img2 
ADD  column id_for2 INTEGER ,
FOREIGN KEY ( id_for2 )  REFERENCES  img ( id_img )
*/
		if( !strFieldType.IsEmpty() )
		{
			CString strSQL;
			strSQL.Format( "alter table %s add %s %s ", 
							CString( bstrTableName ), 
							CString( bstrFieldName ),
							strFieldType );

			m_connPtr->Execute( _bstr_t( (LPCSTR)strSQL ), NULL, ADO::adCmdText );
		}
		else
		if( ft == ftPrimaryKey )
		{
			CString strSQL;
			strSQL.Format( "ALTER TABLE %s ADD  column %s AUTOINCREMENT NOT NULL PRIMARY KEY UNIQUE", 
							CString( bstrTableName ), 
							CString( bstrFieldName ) );

			m_connPtr->Execute( _bstr_t( (LPCSTR)strSQL ), NULL, ADO::adCmdText );
		}
		else if( ft == ftForeignKey )
		{
			CString strSQL;

			strSQL.Format( 
							"ALTER TABLE %s"
							"ADD  column %s INTEGER ,"
							"FOREIGN KEY ( %s )  REFERENCES  %s ( %s ) ",
							CString( bstrTableName ),
							CString( bstrFieldName ),
							CString( bstrFieldName ),
							CString( bstrPrimaryTable ),
							CString( bstrPrimaryFieldName )			
			);

			m_connPtr->Execute( _bstr_t( (LPCSTR)strSQL ), NULL, ADO::adCmdText );
		}		


	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return E_FAIL;
	}

	return S_OK;

}


/////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseEngine::DeleteField( BSTR bstrTableName, BSTR bstrFieldName )
{
	try
	{
		if( !IsConnectionOK() )
			return S_FALSE;

		if( !::IsAccessDBase(m_pDocument->GetControllingUnknown() ) )
			return S_FALSE;

		if( !IsCatalogOK() )
			return S_FALSE;

		SetDirtyTableInfo( true );

		CString strSQL;		
		strSQL.Format( "alter table %s drop %s", CString( bstrTableName ), CString( bstrFieldName ) );
		m_connPtr->Execute( _bstr_t( (LPCSTR)strSQL ), NULL, ADO::adCmdText );

		INamedDataPtr	ptrND( m_pDocument->GetControllingUnknown() );
		if( ptrND == 0 )
			return false;

		CString strEntry = GetFieldEntry( bstrTableName, bstrFieldName );

		_bstr_t bstr;
		bstr = _bstr_t( SECTION_FIELD_CAPTIONS ) + _bstr_t( "\\" ) + _bstr_t( (LPCSTR)strEntry );
		ptrND->DeleteEntry( bstr );

		bstr = _bstr_t( SECTION_FIELD_REQUIRED_VALUES ) + _bstr_t( "\\" ) + _bstr_t( (LPCSTR)strEntry );
		ptrND->DeleteEntry( bstr );

		bstr = _bstr_t( SECTION_FIELD_DEFAULT_VALUES_CHECK ) + _bstr_t( "\\" ) + _bstr_t( (LPCSTR)strEntry );
		ptrND->DeleteEntry( bstr );

		bstr = _bstr_t( SECTION_FIELD_DEFAULT_VALUES ) + _bstr_t( "\\" ) + _bstr_t( (LPCSTR)strEntry );
		ptrND->DeleteEntry( bstr );

		bstr = _bstr_t( SECTION_FIELD_VTOBJECT_TYPE ) + _bstr_t( "\\" ) + _bstr_t( (LPCSTR)strEntry );
		ptrND->DeleteEntry( bstr );


		/*
		ADOX::TablesPtr ptrTables = m_catalogPtr->Tables;

		if( ptrTables == 0 )
			return S_FALSE;


		ADOX::_TablePtr ptrTable = ptrTables->GetItem( _bstr_t( bstrTableName ) );
		if( ptrTable == 0 )
			return S_FALSE;

		ADOX::ColumnsPtr ptrColumns = ptrTable->Columns;
		if( ptrColumns == 0 )
			return S_FALSE;

		SetDirtyTableInfo( true );

		ptrColumns->Delete( _bstr_t( bstrFieldName ) );
		*/
	}
	catch (_com_error &e)
	{
		dump_com_error(e);
		return E_FAIL;
	}

	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CDBaseEngine::SetFieldInfo(	BSTR bstrTableName, BSTR bstrFieldName, BSTR bstrUserName,
									BOOL bRequiredValue, BOOL bDefaultValue,
									BSTR bstrDefaultValue, BSTR bstrVTObjectType )
{
	INamedDataPtr	ptrND( m_pDocument->GetControllingUnknown() );
	if( ptrND == 0 )
		return false;

	CString strTableName = bstrTableName;
	CString strFieldName = bstrFieldName;

	strTableName.MakeLower();
	strFieldName.MakeLower();	


	CString strEntry = GetFieldEntry( strTableName, strFieldName );


	CString strOldCaption = 
		::_GetValueString( ptrND, SECTION_FIELD_CAPTIONS,	strEntry, "" );
	

	::_SetValue( ptrND, SECTION_FIELD_CAPTIONS,				strEntry, CString( bstrUserName ) );
	::_SetValue( ptrND, SECTION_FIELD_REQUIRED_VALUES,		strEntry, (long)bRequiredValue );
	::_SetValue( ptrND, SECTION_FIELD_DEFAULT_VALUES_CHECK, strEntry, (long)bDefaultValue );
	::_SetValue( ptrND, SECTION_FIELD_DEFAULT_VALUES,		strEntry, CString( bstrDefaultValue ) );
	::_SetValue( ptrND, SECTION_FIELD_VTOBJECT_TYPE,		strEntry, CString( bstrVTObjectType ) );

	bool bFound = false;

	_CTableInfo* pti = 0;

	for( int i=0;i<m_arrTableInfo.GetSize();i++ )
	{		
		if( strTableName == m_arrTableInfo[i]->m_strTableName )
		{
			pti = m_arrTableInfo[i];						
			break;
		}
	}

	_CFieldInfo* pfi = 0;
	if( pti )
	{		
		for(int i=0;i<pti->m_arrFieldInfo.GetSize();i++ )
		{
			if( strFieldName == pti->m_arrFieldInfo[i]->m_strFieldName )
			{
				pfi = pti->m_arrFieldInfo[i];
				break;
			}
		}	
	}

	if( pfi )
	{
		pfi->m_strUserName		= bstrUserName;		

		if( pfi->m_strUserName.IsEmpty() )
			pfi->m_strUserName = pfi->m_strFieldName;

		pfi->m_bRequiredValue	= bRequiredValue;
		pfi->m_bDefaultValue	= bDefaultValue;
		pfi->m_strDefaultValue	= bstrDefaultValue;
		pfi->m_strVTObjectType	= bstrVTObjectType;
	}
	else
		SetDirtyTableInfo( true );

	if( m_pDocument )
	{
		IDBaseDocumentPtr ptrDoc( m_pDocument->GetControllingUnknown() );
		if( ptrDoc )
		{

			_variant_t var;
			CString strCaption = bstrUserName;
			if( strOldCaption != strCaption )
			{
				ptrDoc->FireEvent(	_bstr_t( szDBaseEventCaptionChange ), 0, ptrDoc, 
									_bstr_t( (LPCSTR)strTableName ),
									_bstr_t( (LPCSTR)strFieldName ),
									var
									);
			}

			INamedDataPtr ptrND( ptrDoc );
			if( ptrND )
				ptrND->NotifyContexts( ncRescan, 0, 0, 0 );
		}
	}


	return S_OK;
}


