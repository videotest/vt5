#include "stdafx.h"
#include "storage_obj.h"
#include "propbag.h"


//////////////////////////////////////////////////////////////////////
//
// class storage_obj
//
//////////////////////////////////////////////////////////////////////
storage_obj::storage_obj() : IDispatchImpl<IStorageObject>( "StorageObj.tlb" )
{
	m_ipSuperStorage = 0;
	m_bLockExportRecords = FALSE;
	m_lpwstrFileName = 0;
	_reset( );
}

//////////////////////////////////////////////////////////////////////
storage_obj::~storage_obj()
{
	_close_storage_obj( );
	_reset( );
}

//////////////////////////////////////////////////////////////////////
IUnknown* storage_obj::DoGetInterface( const IID &iid )
{
	if( iid == __uuidof( IStorageObject ) ) 
		return (IStorageObject*)this;	
	else if( iid == __uuidof( IStorageObjectMisc ) ) 
		return (IStorageObjectMisc*)this;	
	else if( iid == IID_IDispatch ) 
		return (IDispatch *)this;
	else
		return CObjectBase::DoGetInterface( iid );
}

//ISerializableObject
/////////////////////////////////////////////////
HRESULT storage_obj::Load( IStream *pStream, SerializeParams *pparams )
{
	_close_storage_obj( );
	_reset( );

	HRESULT hr = CObjectBase::Load( pStream, pparams );
	if( S_OK != hr )
		return hr;

	ULONG nRead = 0;

    //At first version control
	long nVersion = 1;
	pStream->Read( &nVersion, sizeof(long), &nRead );
	
	int		nFileNameLength = 0;
	LPWSTR	lpwstFileName = 0;
	
	pStream->Read( &nFileNameLength, sizeof(int), &nRead );
	if( nFileNameLength < 0 )	// incorrect data
		return S_OK;	
	else if( nFileNameLength > 0 )
	{
		if( m_lpwstrFileName )
			delete[] m_lpwstrFileName; m_lpwstrFileName = 0;

		m_lpwstrFileName = new WCHAR[nFileNameLength + 1];
		pStream->Read( m_lpwstrFileName, sizeof(WCHAR) * (nFileNameLength + 1), &nRead );
	}
	
	if( nVersion >= 2 )
	{	// load export records
        long lrec_count = 0;
		pStream->Read( &lrec_count, sizeof( long ), &nRead );
		long lrec = 0;
		for( long lidx = 0; lidx < lrec_count; lidx++ )
		{
			pStream->Read( &lrec, sizeof( long ), &nRead );
			m_lExportRecords.add_tail( lrec );
		}
	}

	if( nVersion >= 3 )
	    pStream->Read( &m_dwObjectState, sizeof(m_dwObjectState), &nRead );

    return S_OK;
}

/////////////////////////////////////////////////
HRESULT storage_obj::Store( IStream *pStream, SerializeParams *pparams )
{
	HRESULT hr = CObjectBase::Store( pStream, pparams );
	if( hr != S_OK )
		return hr;
	
	ULONG nWritten = 0;
	
	//At second version control - add export record
	//At third version - support objects state
	long nVersion = 3;
	pStream->Write( &nVersion, sizeof(long), &nWritten );
	
	STATSTG stStat = {0};
	int		nFileNameLength = 0;
	
    hr = /*m_ipSuperStorage*/ (S_OK == _attach( )) ? m_ipSuperStorage->Stat( &stStat, STATFLAG_DEFAULT ) : S_FALSE;

	if( hr == S_OK )
		nFileNameLength = wcslen( stStat.pwcsName );
	else
		nFileNameLength = 0;
	pStream->Write( &nFileNameLength, sizeof(int), &nWritten );	

	if( hr == S_OK )
	{
		pStream->Write( stStat.pwcsName, sizeof(WCHAR) * (nFileNameLength + 1) , &nWritten );
        ::CoTaskMemFree( stStat.pwcsName );
	}
	
	// save export records
	long lrec_count = m_lExportRecords.count();
	pStream->Write( &lrec_count, sizeof( long ), &nWritten );
	for( long lpos = m_lExportRecords.head(); lpos; lpos = m_lExportRecords.next( lpos ) )
		pStream->Write( &m_lExportRecords.get( lpos ), sizeof( long ), &nWritten );	

	// save state
	pStream->Write( &m_dwObjectState, sizeof(m_dwObjectState), &nWritten );	

	return S_OK;
}

//INamedDataObject2
/////////////////////////////////////////////////
HRESULT storage_obj::IsBaseObject( BOOL* pbFlag )
{
	*pbFlag = TRUE;
	return S_OK;
}

/////////////////////////////////////////////////
//IStorageObject
HRESULT storage_obj::InitNew( /*[in]*/ BSTR bstr_file_name, VARIANT_BOOL bCreateTemp )
{
	if( !bstr_file_name )
		return S_FALSE;

    _close_storage_obj( );
	_reset( );
	_init_dib_size( );

	STGOPTIONS stSOpt = {0};
	stSOpt.ulSectorSize = 4096;
	stSOpt.usVersion = STGOPTIONS_VERSION;
	DWORD grfMode = STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT;
	
	if( bCreateTemp == VARIANT_TRUE )
	{
		m_dwObjectState |= osTemporaryFile;
		grfMode |= STGM_DELETEONRELEASE;
	}

    HRESULT hres = ::StgCreateDocfile( _bstr_t( bstr_file_name ), grfMode, 0, &m_ipSuperStorage );
    if( hres != S_OK )
		return hres;

	// set clsid
	m_ipSuperStorage->SetClass( CLSID_StorageObject );

	// set storage info
	m_stSOInfo.InitDefault( );
	if( !_save_storage_info( ) )
		return S_FALSE;

	return S_OK;
}

HRESULT storage_obj::LoadFile( /*[in]*/ BSTR bstr_file_name )
{
	BOOL bIsLoaded = FALSE;
	
	if( /*m_ipSuperStorage*/ S_OK == _attach( ) )
	{
		STATSTG stStat = {0};
		if( S_OK == m_ipSuperStorage->Stat( &stStat, STATFLAG_DEFAULT ) )
		{
			bIsLoaded = ( wcscmp( stStat.pwcsName, _bstr_t(bstr_file_name) ) == 0 );
			::CoTaskMemFree( stStat.pwcsName );
		}
	}

	if( bIsLoaded )
		return S_OK;
		
	_close_storage_obj( );
	HRESULT hres = ::StgOpenStorage( _bstr_t( bstr_file_name ), 0, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 0, 0, &m_ipSuperStorage );
	/*STGOPTIONS stSOpt = {0};
	stSOpt.ulSectorSize = 4096;
	stSOpt.usVersion = STGOPTIONS_VERSION;
	HRESULT hres = ::StgOpenStorageEx( _bstr_t( bstr_file_name ), 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, STGFMT_DOCFILE, 0, &stSOpt, 0,
		IID_IStorage, (void**) &m_ipSuperStorage );*/

	//if( FAILED( hres ) )
	if( hres != S_OK )	
		return hres;	

	if( !_init_storage_obj( ) )
	{
		_close_storage_obj( );
		_reset( );
		return S_FALSE;
	}
	
	return S_OK;
}

HRESULT storage_obj::InsertRecord()
{
	if( /*!m_ipSuperStorage*/ S_OK != _attach( ) )  
		return S_FALSE;

	WCHAR wRecName[32];
	swprintf( wRecName, L"Record_%u", m_stSOInfo.ulNextRecordNumber );
	IStorage *ipSubStorage = 0;
	HRESULT hres = m_ipSuperStorage->CreateStorage( wRecName, 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, 0, &ipSubStorage );

	_commit( );
    //if( FAILED(hres) )
	if( hres != S_OK )
        return hres;

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

    m_stSOInfo.ulNextRecordNumber ++;
	if( !_save_storage_info( ) )
	{
		m_stSOInfo.ulNextRecordNumber --;
		return S_FALSE;
	}

	_reload_records_names( );
	return S_OK;
}

HRESULT storage_obj::DeleteRecord( /*[in]*/ long lrecord )
{
	if( /*!m_ipSuperStorage*/ S_OK != _attach( ) )  
		return S_FALSE;

	if( ( lrecord < 1 ) || ( lrecord > m_ptRecordsNames.size( ) ) )
        return S_FALSE;    

	HRESULT hres = m_ipSuperStorage->DestroyElement( m_ptRecordsNames.ptr( )[ lrecord - 1 ] );
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;

    _reload_records_names( );

	// [vanek] BT2000: 3548
	// update current record
	int rec_count = m_ptRecordsNames.size();
	if( m_lCurrRecord > rec_count )
		m_lCurrRecord = (rec_count == 0) ? CURRENT_RECORD_NO_SET : rec_count;
	
	// updates export records
	long lrec_pos = m_lExportRecords.find( lrecord );
	if( lrec_pos )
		m_lExportRecords.remove( lrec_pos );
	for( long lpos = m_lExportRecords.head(); lpos; lpos = m_lExportRecords.next( lpos ) )
	{
		long lexport_rec = m_lExportRecords.get( lpos );
		if( lrecord < lexport_rec )
			m_lExportRecords.set( --lexport_rec, lpos );
	}

	return S_OK;
}

HRESULT storage_obj::GetCurrentRecord( /*[out, retval]*/ long* plrecord )
{
	if( !plrecord  ) // [vanek]: если текущей записи нет, вовращаем -1 || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;

	*plrecord = m_lCurrRecord;
	return S_OK;
}

HRESULT storage_obj::SetCurrentRecord( /*[in]*/ long lrecord, DWORD dwFlags )
{
	if( (lrecord > m_ptRecordsNames.size( )) || ( lrecord <= CURRENT_RECORD_NO_SET ) )
        return S_FALSE;    

	m_lCurrRecord = lrecord;
	if( !(dwFlags & asAbortObjectCountCalculation) )
		_calc_object_count( );

	return S_OK;    
}

HRESULT storage_obj::GetRecordCount( /*[out, retval]*/ long* plcount )
{
	if( !plcount )
        return S_FALSE;    

	*plcount = m_ptRecordsNames.size();
	return S_OK;
}

HRESULT storage_obj::AddObject( /*[in] long plrecord,*/ /*[in]*/ IDispatch* pdisp_object )
{
	if( /*!m_ipSuperStorage*/ (S_OK != _attach( )) || !pdisp_object || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    

    IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
	//if( SUCCEEDED( hres ) && ipSubStorage )
	if( ( hres == S_OK ) && ipSubStorage )
	{
		if( ( S_OK == (hres = _add_object( ipSubStorage, 0,pdisp_object )) ) && ( m_ulObjectCount != OBJECT_COUNT_NO_SET ) )
			_calc_object_count( ipSubStorage );
	}
	
	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	return hres;
}

HRESULT	storage_obj::GetObject( /*[in] long plrecord,*/ /*[in]*/ BSTR bstr_type, /*[out, retval]*/ IDispatch** pdisp_object )
{
	if( !bstr_type || !pdisp_object || /*!m_ipSuperStorage*/(S_OK == _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    
	
	IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;
    
	hres = _get_object( ipSubStorage, bstr_type, pdisp_object );				 

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	return hres;
}

HRESULT	storage_obj::GetObjectName( /*[in] long plrecord,*/ /*[in]*/ BSTR bstr_type, /*[out, retval]*/ BSTR* pbstr_name )
{
	if( !bstr_type || !pbstr_name || /*!m_ipSuperStorage*/(S_OK != _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    
	
	IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, 
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;
	
	hres = _get_object_name( ipSubStorage, bstr_type, pbstr_name );

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	return hres;    
}

HRESULT	storage_obj::GetObjectCount( /*[in] long plrecord,*/ /*[out, retval]*/ long* plcount )
{
	if( !plcount || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    

	*plcount = (long) m_ulObjectCount;
    return S_OK;
}

HRESULT	storage_obj::GetObjectType( /*[in] long plrecord,*/ /*[in]*/ long lidx, /*[out, retval]*/ BSTR* pbstr_type )
{
	if( /*!m_ipSuperStorage*/(S_OK == _attach( )) || !pbstr_type || (lidx < 0) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    
	
    IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, 
				STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
    //if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;

	IStorage *ipObjStorage = 0;
	hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipSubStorage, &ipObjStorage,
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
	if( hres == S_OK )
	{
		IEnumSTATSTG *penum = 0;
		STATSTG statstg = {0};

		hres = ipObjStorage->EnumElements( 0, 0, 0, &penum );
		//if( SUCCEEDED( hres ) )
		if( hres == S_OK )
		{
			penum->Skip( lidx );	///!!!!!!!!!!!!!!!!!!!!
			hres = penum->Next( 1, &statstg, 0 );
			//if( SUCCEEDED( hres ) )
			if( hres == S_OK )
			{
				*pbstr_type = ::SysAllocString( statstg.pwcsName );
				if( statstg.pwcsName )
					::CoTaskMemFree( statstg.pwcsName );
			}
		}

		if( penum )
		{
			penum->Release( );
			penum = 0;
		}
	}

	if( ipObjStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	return hres;
}

HRESULT storage_obj::DeleteObject( /*[in] long plrecord,*/ /*[in]*/ BSTR bstr_type )
{
	if( !bstr_type || /*!m_ipSuperStorage*/(S_OK != _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    

	IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
    //if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;

	IStorage *ipObjStorage = 0;
	hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipSubStorage, &ipObjStorage,
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, FALSE );
    if( hres == S_OK )
	{

		hres = ipObjStorage->DestroyElement( _bstr_t( bstr_type ) );
		_commit( );
		//if( SUCCEEDED( hres ) )
		if( hres == S_OK )
		{
			if( wcscmp( _bstr_t( bstr_type ), _bstr_t( szTypeImage ) ) == 0 )
			{
				// It's image - delete image DIB
				IStorage *ipPrivateStorage = 0;
				hres = _get_create_sub_storage( _bstr_t( sPrivateStorage ), ipSubStorage, 
					&ipPrivateStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
				if( hres == S_OK )
				{
					hres = ipPrivateStorage->DestroyElement( _bstr_t( sDIBObjTypeName ) );
                    _commit( );
				}

				if( ipPrivateStorage )
				{
					ipPrivateStorage->Release( );
					ipPrivateStorage = 0;
				}
			}
			
			if( (hres == S_OK) && (m_ulObjectCount != OBJECT_COUNT_NO_SET) )
				_calc_object_count( ipSubStorage, ipObjStorage);
		}
	}
	if( ipObjStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	return hres;    
}

HRESULT storage_obj::ExportRecord( /*[in] long lrecord,*/ /*[in]*/ IDispatch* pdisp_data )
{
	if( !pdisp_data || /*!m_ipSuperStorage*/(S_OK != _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    

	IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;

	IStorage *ipObjStorage = 0;
	hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipSubStorage, &ipObjStorage,
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
    if( hres == S_OK )
	{

		IEnumSTATSTG *penum = 0;
		STATSTG statstg = {0};

		hres = ipObjStorage->EnumElements( 0, 0, 0, &penum );
		//if( SUCCEEDED( hres ) )
		if( hres == S_OK )
		{
				
			HRESULT hr = penum->Next( 1, &statstg, 0 );
			while( hr == S_OK )
			{
				if( STGTY_STREAM == statstg.type )
				{
					IDispatch *pdisp_object = 0;
					BSTR bstr_type = 0;
					bstr_type = ::SysAllocString(statstg.pwcsName);
					hres = _get_object( ipObjStorage, bstr_type, &pdisp_object, FALSE );
					//if( SUCCEEDED(hres) )
					if( (hres == S_OK) && pdisp_object )
					{
						// add object to document					
						::SetValue( pdisp_data, 0, 0, _variant_t( (IUnknown*) pdisp_object ) );		
						IFileDataObject2Ptr ptrFDO2 = pdisp_data;
						if( ptrFDO2 )
							ptrFDO2->SetModifiedFlag( TRUE ); // set modified flag
					}

					if( pdisp_object )
					{
						pdisp_object->Release( );
						pdisp_object = 0;
					}

					if( bstr_type )
					{
						::SysFreeString( bstr_type );
						bstr_type = 0;
					}
				}	
				if( statstg.pwcsName )
					::CoTaskMemFree( statstg.pwcsName );
				hr = penum->Next( 1, &statstg, 0 );
			}
		
		}
		

		if( penum )
		{
			penum->Release( );
			penum = 0;
		}
	}

	if( ipObjStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}


	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	if( hres == S_OK )
	{
		long lpos = m_lExportRecords.find( m_lCurrRecord );
		if( !lpos )
		{
            m_lExportRecords.clear(); // for export only one record
			m_lExportRecords.add_tail( m_lCurrRecord );
		}
	}

	return hres;    
}

HRESULT storage_obj::ImportRecord( /*[in] long lrecord,*/ /*[in]*/ IDispatch* pdisp_data )
{
	if( !pdisp_data || /*!m_ipSuperStorage*/ (S_OK != _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
			return S_FALSE;    

	IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT);
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;

	IStorage *ipObjStorage = 0;
	hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipSubStorage, &ipObjStorage,
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
	if( hres == S_OK )
	{

		IEnumSTATSTG *penum = 0;
		STATSTG statstg = {0};

		hres = ipObjStorage->EnumElements( 0, 0, 0, &penum );
		//if( SUCCEEDED( hres ) )
		if( hres == S_OK )
		{
				
			HRESULT hr = penum->Next( 1, &statstg, 0 );
			while( hr == S_OK )
			{
				if( STGTY_STREAM == statstg.type )
				{
					IDispatch *pdisp_object = 0;
					BSTR bstr_type = 0, bstr_obj_name = 0;
					
					bstr_type = ::SysAllocString(statstg.pwcsName);	
					
					// get object name
					hres = _get_object_name( ipObjStorage, bstr_type, &bstr_obj_name, FALSE );

					//if( SUCCEEDED( hres ) && bstr_obj_name )
					if( ( hres == S_OK ) && bstr_obj_name && bstr_type )
					{
						// get object from document
						IUnknown* punk_obj = 0;

						//_variant_t var_t = ::GetValue( pdisp_data, 0, _bstr_t(bstr_obj_name), var );
						punk_obj = ::GetObjectByName( pdisp_data, bstr_obj_name, bstr_type );
						if( punk_obj )
						{
							hres = punk_obj->QueryInterface( IID_IDispatch, (void**) &pdisp_object );
							punk_obj->Release( ); punk_obj = 0;
						}
					}

					if( pdisp_object )
					{
						hres = _add_object( ipSubStorage, ipObjStorage, pdisp_object );
						pdisp_object->Release( );
						pdisp_object = 0;
					} 

					if( bstr_type )
					{
						::SysFreeString( bstr_type );
						bstr_type = 0;
					}

					if( bstr_obj_name )
					{
						::SysFreeString( bstr_obj_name );
						bstr_obj_name = 0;
					}
				}	
				if( statstg.pwcsName )
					::CoTaskMemFree( statstg.pwcsName );

				hr = penum->Next( 1, &statstg, 0 );
			}
		
		}
		

		if( penum )
		{
			penum->Release( );
			penum = 0;
		}
	}

	if( ipObjStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	if( hres == S_OK )
	{
		long lpos = m_lExportRecords.find( m_lCurrRecord );
		if( lpos )
			m_lExportRecords.remove( lpos );
	}

	return hres;    
}

HRESULT storage_obj::ClearDocument( /*[in]x long lrecord,*/ /*[in]*/ IDispatch* pdisp_data )
{
   if( !pdisp_data || /*!m_ipSuperStorage*/(S_OK != _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
			return S_FALSE;    

	IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, 
				STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;

	IStorage *ipObjStorage = 0;
	hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipSubStorage, &ipObjStorage,
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
	if( hres == S_OK )
	{

		IEnumSTATSTG *penum = 0;
		STATSTG statstg = {0};

		hres = ipObjStorage->EnumElements( 0, 0, 0, &penum );
		//if( SUCCEEDED( hres ) )
		if( hres == S_OK )
		{
				
			HRESULT hr = penum->Next( 1, &statstg, 0 );
			while( hr == S_OK )
			{
				if( STGTY_STREAM == statstg.type )
				{
					IDispatch *pdisp_object = 0;
					BSTR bstr_type = 0, bstr_obj_name = 0;
					bstr_type = ::SysAllocString(statstg.pwcsName);			
					hres = _get_object_name( ipObjStorage, bstr_type, &bstr_obj_name, FALSE );
					//if( SUCCEEDED( hres ) && bstr_obj_name )
					if( ( hres == S_OK ) && bstr_obj_name )
					{
						// get object from document
						IUnknown* punk_obj = 0;
						//punk_obj = (IUnknown*) ::GetValue( pdisp_data, 0, _bstr_t( bstr_obj_name ), _variant_t( (IUnknown*) 0 ) );
						punk_obj = ::GetObjectByName( pdisp_data, bstr_obj_name, bstr_type );
						if( punk_obj )
						{
							hres = punk_obj->QueryInterface( IID_IDispatch, (void**) &pdisp_object );
							punk_obj->Release( ); punk_obj = 0;
						}
					}

					if( pdisp_object )
					{	// delete object
						::DeleteObject( pdisp_data, pdisp_object );
						pdisp_object->Release( );
						pdisp_object = 0;
						hres = S_OK;
					}

					IFileDataObject2Ptr ptrFDO2 = pdisp_data;
					if( ptrFDO2 )
						ptrFDO2->SetModifiedFlag( TRUE );	// set modified flag
	                
					if( bstr_type )
					{
						::SysFreeString( bstr_type );
						bstr_type = 0;
					}

					if( bstr_obj_name )
					{
						::SysFreeString( bstr_obj_name );
						bstr_obj_name = 0;
					}
				}	
				if( statstg.pwcsName )
					::CoTaskMemFree( statstg.pwcsName );

				hr = penum->Next( 1, &statstg, 0 );
			}
		
		}
		

		if( penum )
		{
			penum->Release( );
			penum = 0;
		}
	}

	if( ipObjStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	if( hres == S_OK )
	{
		long lpos = m_lExportRecords.find( m_lCurrRecord );
		if( lpos )
			m_lExportRecords.remove( lpos );
	}

	return hres;    
}

HRESULT	storage_obj::GetExportRecord(  long *plrecord )
{
	if( !plrecord )
		return S_FALSE;

	// get last export record
	*plrecord = m_lExportRecords.count() ? m_lExportRecords.get( m_lExportRecords.tail() ) : -1;  
	return S_OK;
}

HRESULT	storage_obj::get_Name( BSTR *pbstr )
{
	if( !pbstr )
		return S_FALSE;

	*pbstr = m_bstrName.copy();
	return S_OK;
}

HRESULT	storage_obj::put_Name( BSTR bstr )
{
   return S_OK;	// Name - read only!!!
}

HRESULT	storage_obj::get_SerializeContent(VARIANT_BOOL *pvbVal)
{
    if( !pvbVal )
		return E_INVALIDARG;

	*pvbVal = m_dwObjectState & osSerializeContent ? VARIANT_TRUE : VARIANT_FALSE;
	return S_OK;
}

HRESULT	storage_obj::put_SerializeContent(VARIANT_BOOL vbNewVal)
{
	if( vbNewVal == VARIANT_TRUE )
        m_dwObjectState &= osSerializeContent;
	else
		m_dwObjectState &= ~osSerializeContent;

	return S_OK;
}

/////////////////////////////////////////////////
//IStorageObjectMisc
HRESULT storage_obj::GetRecordDib( /*[in] long pl_record,*/ /*[out]*/ BYTE** ppbi, /*[out]*/ DWORD* pdwSize )
{
	if( !ppbi || /*!m_ipSuperStorage*/(S_OK != _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
			return S_FALSE;    

	IStorage *ipSubStorage = 0;
	HRESULT hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT);
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return hres;

	IStorage *ipPrivateStorage = 0;
	hres = _get_create_sub_storage( _bstr_t( sPrivateStorage ), ipSubStorage, &ipPrivateStorage,
					STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
	if( hres == S_OK )
	{
	   	IStream *ipStreamObj = 0;
		hres = ipPrivateStorage->OpenStream( _bstr_t( sDIBObjTypeName ), 0, 
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 0, &ipStreamObj );

		//if( SUCCEEDED( hres ) )
		if( hres == S_OK )
		{
			XStreamObjInfo stInfo;
			stInfo.InitDefault( );
			if( !stInfo.Serialize( ipStreamObj, FALSE ) )
				hres = S_FALSE;
			else
				hres = LoadDIB( ipStreamObj, ppbi, pdwSize) ? S_OK : S_FALSE;
		}

		if( ipStreamObj )
		{
			ipStreamObj->Release( );
			ipStreamObj = 0;
		}
	}

	if( ipPrivateStorage )
	{
		ipPrivateStorage->Release( );
		ipPrivateStorage = 0;
	}

	if( ipSubStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}
	return hres;    
}

HRESULT storage_obj::GetObjectPropBag( /*[in]*/ BSTR bstr_type,  /*[out]*/ IUnknown **ppunkPropBag )
{
    if( !bstr_type && !ppunkPropBag || (S_OK != _attach( )) || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return S_FALSE;    

	IStoragePtr sptrSubStorage = 0;
	IStoragePtr sptrPrivateStorage = 0;
	IStoragePtr sptrPropBagStorage = 0;

	IStreamPtr sptrPropBagStream = 0;

	HRESULT hres = _get_record_storage( m_lCurrRecord, &sptrSubStorage, 
		STGM_READ | STGM_SHARE_EXCLUSIVE | STGM_DIRECT);
	if( hres != S_OK )
		return hres;

	hres = _get_create_sub_storage( _bstr_t( sPrivateStorage ), sptrSubStorage, &sptrPrivateStorage,
					STGM_READ | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, FALSE );
	if( hres != S_OK )
		return hres;

	hres = _get_create_sub_storage( _bstr_t( sPropBagsStorage ), sptrPrivateStorage, &sptrPropBagStorage,
					STGM_READ | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, FALSE );
	if( hres != S_OK )
		return hres;

    hres = sptrPropBagStorage->OpenStream( _bstr_t( bstr_type ), 0, STGM_READ | STGM_SHARE_EXCLUSIVE | STGM_DIRECT,
		0, &sptrPropBagStream );
	if( hres != S_OK )
		return hres;
    
	IUnknown *punk_obj = 0;
	punk_obj = ::CreateTypedObject( bstr_type );
	INamedPropBagSerPtr sptr_propbagser = punk_obj;
	if( punk_obj )
		punk_obj->Release( ); punk_obj = 0;

    if( sptr_propbagser == 0 )
		return S_FALSE;

	hres = sptr_propbagser->Load( sptrPropBagStream );
	if( hres != S_OK )
		return hres;

	*ppunkPropBag = sptr_propbagser;
	(*ppunkPropBag)->AddRef( );    	
	return S_OK;
}


//////////////////////////////////////////////////////
HRESULT	storage_obj::_attach( )
{
	if( m_ipSuperStorage )
		return S_OK;

	HRESULT hr = S_FALSE;
	if( m_lpwstrFileName )
	{
		if( m_dwObjectState & osTemporaryFile )
			hr = InitNew( _bstr_t( m_lpwstrFileName ), VARIANT_TRUE );	  // create temporary file
		else
		{
			m_bLockExportRecords = TRUE;	// lock - export records have already loaded
			hr = LoadFile( _bstr_t( m_lpwstrFileName ) );	// load from file
			m_bLockExportRecords = FALSE;	// unlock
		}

		// 06.10.2003
		/* delete[] m_lpwstrFileName;
		m_lpwstrFileName = 0; */
	}

	return hr;
}

//////////////////////////////////////////////////////
BOOL	storage_obj::_init_storage_obj( )
{
	if( !m_ipSuperStorage /*(S_OK != _attach( ))*/ )
		return FALSE;
	
	STATSTG stStat = {0};
    
	_reset( );
	_init_dib_size( );

	HRESULT hres = m_ipSuperStorage->Stat( &stStat, STATFLAG_NONAME );
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return FALSE;
	if( stStat.clsid != CLSID_StorageObject )
		return FALSE;	// чужой			
	
	if( _load_storage_info( ) )
		return _reload_records_names( );
	else
		return FALSE;
}

BOOL	storage_obj::_load_records_names( )
{
	if( !m_ipSuperStorage /*(S_OK != _attach( ))*/ )
		return FALSE;

	if( 0 != m_ptRecordsNames.size( ) )
		return FALSE;

	IEnumSTATSTG *penum = 0;
	HRESULT hres = S_OK;
	STATSTG statstg = {0};
	ULONG	ulRecCount = 0;
	LPWSTR lpwstrRecName = 0;

	hres = m_ipSuperStorage->EnumElements( 0, 0, 0, &penum );
	//if( FAILED( hres ) )
	if( hres != S_OK )
		return FALSE;

	hres = penum->Next( 1, &statstg, 0 );
	while( hres == S_OK )
    {
		if( STGTY_STORAGE == statstg.type )
		{
			ulRecCount ++;
			//m_lstRecordsNames.add_tail( statstg.pwcsName );
            //lpwstrRecName = new WCHAR[ sizeof(statstg.pwcsName) / sizeof(WCHAR) + 1 ];
			
			lpwstrRecName = new WCHAR[ wcslen( statstg.pwcsName ) + 1 ];
			wcscpy( lpwstrRecName, statstg.pwcsName );
			m_ptRecordsNames.alloc( ulRecCount );
			m_ptRecordsNames.ptr()[ulRecCount - 1] = lpwstrRecName;
			lpwstrRecName = 0;
		}
		if( statstg.pwcsName )
			::CoTaskMemFree( statstg.pwcsName );

		hres = penum->Next( 1, &statstg, 0 );
	}

	if( penum )
	{
		penum->Release( );
		penum = 0;
	}

	return TRUE;
}

BOOL	storage_obj::_reload_records_names( )
{
	_free_records_names( );
	return _load_records_names( );
}

void	storage_obj::_free_records_names( )
{
	LPWSTR lpwstr = 0;
	for( long l = 0; l < m_ptRecordsNames.size( ); l++ )
	{
		delete[] m_ptRecordsNames.ptr( )[ l ];
		m_ptRecordsNames.ptr( )[ l ] = 0;
	}
	m_ptRecordsNames.free( );
}

BOOL	storage_obj::_calc_object_count( IStorage *piRecStorage /*= 0*/, IStorage *piObjectStorage /*= 0*/ )
{
	m_ulObjectCount = OBJECT_COUNT_NO_SET;
	
	if( !m_ipSuperStorage /*(S_OK != _attach( ))*/ || (m_lCurrRecord == CURRENT_RECORD_NO_SET) )
		return FALSE;
    
	IStorage *ipSubStorage = 0;
	HRESULT hres = S_OK;
	if( piRecStorage )
		ipSubStorage = piRecStorage;
	else
	{
		hres = _get_record_storage( m_lCurrRecord, &ipSubStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT );
		//f( FAILED( hres ) )
		if( hres != S_OK )
			return FALSE;
	}

	IStorage *ipObjStorage = 0;
	if( piObjectStorage )
	  ipObjStorage = piObjectStorage;
	else
	{
		hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipSubStorage, &ipObjStorage,
				STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, FALSE );
	}
	
	BOOL bRetVal = FALSE;
	if( hres == S_OK )
	{
     	IEnumSTATSTG *penum = 0;
		STATSTG statstg = {0};
		
		hres = ipObjStorage->EnumElements( 0, 0, 0, &penum );
		//if( SUCCEEDED( hres ) )
		if( hres == S_OK )
		{
			hres = penum->Next( 1, &statstg, 0 );
			m_ulObjectCount = 0;
			while( hres == S_OK )
			{
				if( STGTY_STREAM == statstg.type )
					m_ulObjectCount ++;
				
				if( statstg.pwcsName )
					::CoTaskMemFree( statstg.pwcsName );

				hres = penum->Next( 1, &statstg, 0 );
			}
			bRetVal = TRUE;
		}
		else
			bRetVal = FALSE;

		if( penum )
		{
			penum->Release( );
			penum = 0;
		}
	}
	else if( hres == STG_E_FILENOTFOUND )
		m_ulObjectCount = 0;


	if( ipObjStorage && !piObjectStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}

	if( ipSubStorage && !piRecStorage )
	{
		ipSubStorage->Release( );
		ipSubStorage = 0;
	}

	
	return bRetVal;
}


BOOL	storage_obj::_load_storage_info( )
{
	if( !m_ipSuperStorage /*(S_OK != _attach( ))*/ )
		return FALSE;

	IStream *ipStreamInfo = 0;	
	HRESULT hres = m_ipSuperStorage->OpenStream( _bstr_t(sNameInfoStream), 0,
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 0, &ipStreamInfo );
	//if( FAILED(hres) )
	if( hres != S_OK )
		return FALSE;
	
	m_stSOInfo.Reset( );
	ULONG ulRead = m_stSOInfo.Serialize( ipStreamInfo, FALSE );

	if( ipStreamInfo )
	{
		ipStreamInfo->Release( );
		ipStreamInfo = 0;
	}

    return 	(ulRead != 0);
}

BOOL	storage_obj::_save_storage_info( )
{
	if( !m_ipSuperStorage /* (S_OK != _attach( ))*/ )
		return FALSE;

	IStream *ipInfoStream = 0;	
	HRESULT hres = m_ipSuperStorage->OpenStream( _bstr_t(sNameInfoStream), 0,
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 0, &ipInfoStream );

	if( hres == STG_E_FILENOTFOUND )
		hres = m_ipSuperStorage->CreateStream( _bstr_t( sNameInfoStream ), 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, 0, &ipInfoStream );
	
	
	//if( FAILED(hres) )
	if( hres != S_OK )
		return FALSE;

	m_stSOInfo.Serialize( ipInfoStream );

	if( ipInfoStream )
	{
		ipInfoStream->Release( );
		ipInfoStream = 0;
	}

	_commit( );
	return TRUE;
}

BOOL	storage_obj::_close_storage_obj( )
{
	BOOL bRetVal = FALSE;
    if( bRetVal = m_ipSuperStorage != 0 )
		m_ipSuperStorage->Release( );	m_ipSuperStorage = 0;	
	
	if( m_lpwstrFileName )
		delete[] m_lpwstrFileName; m_lpwstrFileName = 0;

	return bRetVal;
}

BOOL	storage_obj::_commit( )
{
	if( !m_ipSuperStorage /* (S_OK != _attach( ))*/ )
		return FALSE;

	m_ipSuperStorage->Commit( /*STGC_DEFAULT*/ STGC_OVERWRITE );
	return TRUE;
}

BOOL	storage_obj::_init_dib_size( )
{
	m_sizeDIB.cx = ::GetValueInt( ::GetAppUnknown( ), "\\StorageObject", "DibWidth", 50 );
	m_sizeDIB.cy = ::GetValueInt( ::GetAppUnknown( ), "\\StorageObject", "DibHeight", 50 );

	return TRUE;
}

BOOL	storage_obj::_create_dib( IDispatch *pdisp_object, BYTE **ppDIB, DWORD *pdwSize)
{
	if( !pdisp_object || !(ppDIB) )
		return FALSE;

	sptrIThumbnailManager spThumbMan( GetAppUnknown() );
	if( spThumbMan == NULL )
		return FALSE;
	
	IUnknown* punkRenderObject = 0;
    spThumbMan->GetRenderObject( pdisp_object, &punkRenderObject );
	if( punkRenderObject == 0 )
		return FALSE;
	
    sptrIRenderObject spRenderObject( punkRenderObject );
	punkRenderObject->Release();

	if( spRenderObject == NULL ) 
		return FALSE;

	short nSupported;
	spRenderObject->GenerateThumbnail( pdisp_object, 24, 0, 0, m_sizeDIB, 
			&nSupported, ppDIB );
	
	if( *ppDIB )
	{
		if( pdwSize )
			spThumbMan->ThumbnailGetSize( *ppDIB, pdwSize);

		return TRUE;
	}
	else
		return FALSE;
}

void	storage_obj::_reset( )
{
	m_dwObjectState = 0;
	m_lCurrRecord = CURRENT_RECORD_NO_SET;
	m_ulObjectCount = OBJECT_COUNT_NO_SET;
	m_stSOInfo.Reset( );
	::ZeroMemory( &m_sizeDIB, sizeof( m_sizeDIB ) );
	_free_records_names( );

	if( !m_bLockExportRecords )
		m_lExportRecords.clear( );
}

HRESULT	storage_obj::_add_object( IStorage *ipRecordStorage, IStorage *ipObjStorage, IDispatch* pdisp_object )
{
	if( !pdisp_object || !ipRecordStorage )
		return S_FALSE; 

	BOOL	bOpenObjectsStorage = (ipObjStorage == 0);

	// get object type
	_bstr_t bstrtObjType = ::GetObjectType( pdisp_object );

	// get object name
	INamedObject2Ptr sptrObj( pdisp_object );
	BSTR bstrObjName = 0;
	if( S_OK != sptrObj->GetName( &bstrObjName ) )
		return S_FALSE;

	HRESULT hres = S_OK;
	if( bOpenObjectsStorage )
	{
		hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipRecordStorage,
		&ipObjStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
	}
	
	if( hres == S_OK )
	{		
    	IStream *ipObjStream = 0;
		hres = ipObjStorage->CreateStream( bstrtObjType,
			STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, 0,
			&ipObjStream );

		_commit( );

		if( hres == S_OK )
		{
			XStreamObjInfo stInfo;
			stInfo.InitDefault( );
			stInfo.text_copy( _bstr_t( bstrObjName ) );
			stInfo.Serialize( ipObjStream );
			
			ISerializableObjectPtr sptrSrzObj( pdisp_object ); 
			if( sptrSrzObj == 0 )
				hres = S_FALSE;
			else
			{
				SerializeParams sp = {0};
				sp.flags = sf_DetachParent;	 // for correct storing the object
				if( ::GetValueInt( ::GetAppUnknown( ), "\\StorageObject", "CompressImage", 1 ) == 1L )
					sp.flags |= sf_ImageCompressed;
				
				sptrSrzObj->Store( ipObjStream, &sp );
			}
			_commit( );

			// vanek!
            // save private data: DIB, propbags
			IStorage *ipPrivateStorage = 0;
            hres = _get_create_sub_storage( _bstr_t( sPrivateStorage ), ipRecordStorage, &ipPrivateStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
			if( hres == S_OK )
			{
				// PropBag
				INamedPropBagSerPtr sptr_propbagser = pdisp_object;
				if( sptr_propbagser != 0 )
				{
                    IStorage *ipPropbagStorage = 0;                    
                    hres = _get_create_sub_storage( _bstr_t( sPropBagsStorage ), ipPrivateStorage, 
						&ipPropbagStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, TRUE );
                    if( hres == S_OK )
					{
						IStream *ipObjPropbagStream = 0;
						hres = ipPropbagStorage->CreateStream( _bstr_t( bstrtObjType ), 
							STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, 0,
							&ipObjPropbagStream );
						if( hres == S_OK  )
						    sptr_propbagser->Store( ipObjPropbagStream );

						if( ipObjPropbagStream )
							ipObjPropbagStream->Release( ); ipObjPropbagStream = 0;
					}
                    
					if( ipPropbagStorage )
						ipPropbagStorage->Release( ); ipPropbagStorage = 0;					
				}
				
				// DIB
				if( !wcscmp( bstrtObjType, _bstr_t( szTypeImage ) )	)
				{	// It's Image! - creating DIB
					IStream *ipObjDibStream = 0;
					hres = ipPrivateStorage->CreateStream( _bstr_t( sDIBObjTypeName ),
						STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0,
						0, &ipObjDibStream );
					if( hres == S_OK  )
					{
						BYTE *pDIB = 0;
						DWORD dwSize = 0;
						if( _create_dib( pdisp_object, &pDIB, &dwSize ) )
						{
							stInfo.Serialize( ipObjDibStream );
							StoreDIB( ipObjDibStream, pDIB, dwSize );
							FreeDIB( pDIB );
						}
					}

					if( ipObjDibStream )
						ipObjDibStream->Release( ); ipObjDibStream = 0;
				}
                
				_commit( );
			}
            
			if( ipPrivateStorage )
			{
				ipPrivateStorage->Release( );
				ipPrivateStorage = 0;
			}
		}

		if( ipObjStream )
		{
			ipObjStream->Release( );
			ipObjStream = 0;
		}
	}

	if( ipObjStorage && bOpenObjectsStorage )
	{
		ipObjStorage->Release();
		ipObjStorage = 0;
	}

	if( bstrObjName )
	{
		::SysFreeString( bstrObjName );
		bstrObjName = 0;
	}

	return hres;
}

HRESULT	storage_obj::_get_object( IStorage *ipStorage, BSTR bstr_type, IDispatch** ppdisp_object, BOOL bOpenObjectsStorage /*= TRUE*/ )
{
	if( !bstr_type || !ppdisp_object || (!bOpenObjectsStorage && !ipStorage) )
		return S_FALSE;    

	*ppdisp_object = 0;
	
	IStorage *ipObjStorage = 0;
	HRESULT hres = S_FALSE;
	if( bOpenObjectsStorage )
	{
		hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ), ipStorage,
		&ipObjStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, FALSE );
		if( hres != S_OK )
			return hres;
	}
	else
		ipObjStorage = ipStorage;
		
	IStream *ipStreamObj = 0;
	hres = ipObjStorage->OpenStream( _bstr_t( bstr_type ), 0, 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 0, &ipStreamObj );

	//if( SUCCEEDED( hres ) )
	if( hres == S_OK )
	{
		XStreamObjInfo stInfo;
		stInfo.InitDefault( );
		if( !stInfo.Serialize( ipStreamObj, FALSE ) )
			hres = S_FALSE;
		else
		{		
			IUnknown *punk_obj = 0;
			punk_obj = ::CreateTypedObject( bstr_type );
			if( punk_obj )
			{
				hres = punk_obj->QueryInterface( IID_IDispatch, (void**) ppdisp_object );
				punk_obj->Release( );
				punk_obj = 0;
				//if( SUCCEEDED(hres) )
				if( hres == S_OK )
				{
					ISerializableObjectPtr sptrSrzObj( *ppdisp_object );
					if( sptrSrzObj )
					{
						SerializeParams sp = {0};
						sp.flags |= sf_DontCheckSignature;
						sptrSrzObj->Load( ipStreamObj, &sp );
					}
					sptrSrzObj = 0;
					INumeredObjectPtr	sptrNumObject( *ppdisp_object );
					if( sptrNumObject )
						sptrNumObject->GenerateNewKey( 0 );
					sptrNumObject = 0;
				}
			}
			else
				hres = S_FALSE;
    	}
	}

	if( ipStreamObj )
	{
		ipStreamObj->Release( );
		ipStreamObj = 0;
	}

	if( ipObjStorage && bOpenObjectsStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}

	return hres;
}

HRESULT	storage_obj::_get_object_name(  IStorage *ipStorage, BSTR bstr_type, BSTR* pbstr_name, BOOL bOpenObjectsStorage /*= TRUE*/ )
{
	if( !bstr_type || !pbstr_name || (!bOpenObjectsStorage && !ipStorage) )
		return S_FALSE;    

	IStorage *ipObjStorage = 0;
	HRESULT hres = S_FALSE;
	if( bOpenObjectsStorage )
	{
		hres = _get_create_sub_storage( _bstr_t( sObjectsStorage ),
			ipStorage, &ipObjStorage, STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, FALSE );
		if( hres != S_OK )
			return hres;
	}
	else
		ipObjStorage = ipStorage;

	IStream *ipStreamObj = 0;
	hres = ipObjStorage->OpenStream( _bstr_t( bstr_type ), 0, 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_DIRECT, 0, &ipStreamObj );

	//if( SUCCEEDED( hres ) )
	if( hres == S_OK )
	{
		XStreamObjInfo stInfo;
		stInfo.InitDefault( );
		if( stInfo.Serialize( ipStreamObj, FALSE ) )
			*pbstr_name = ::SysAllocString( stInfo.lpwstrObjName );
		else
			hres = S_FALSE;
	}

	if( ipStreamObj )
	{
		ipStreamObj->Release( );
		ipStreamObj = 0;
	}
	
	if( ipObjStorage && bOpenObjectsStorage )
	{
		ipObjStorage->Release( );
		ipObjStorage = 0;
	}
	return hres;
}

HRESULT	storage_obj::_get_record_storage( long lrecord, IStorage** ppiStrorage, DWORD grfMode )
{
	if( /*!m_ipSuperStorage*/ (S_OK != _attach( )) || !ppiStrorage || ( lrecord > m_ptRecordsNames.size( ) ) || ( lrecord < 1 )  )
		return S_FALSE;
	
	return m_ipSuperStorage->OpenStorage( m_ptRecordsNames.ptr( )[ lrecord - 1 ], 0, grfMode, 0,
		0, ppiStrorage );
}

HRESULT storage_obj::_get_create_sub_storage( LPWSTR lpwstrSubStorageName, IStorage* piStorage, IStorage** ppiSubStorage, DWORD grfMode, BOOL bCreateIfNoExist )
{
	if( !lpwstrSubStorageName || !piStorage || !ppiSubStorage )
		return S_FALSE;

	HRESULT hr = piStorage->OpenStorage( lpwstrSubStorageName, 0, grfMode, 0,
		0, ppiSubStorage ); 

	if( (hr == STG_E_FILENOTFOUND) && bCreateIfNoExist )
	{	// create substorage
		hr = piStorage->CreateStorage( lpwstrSubStorageName, 
		STGM_READWRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE | STGM_DIRECT, 0, 0, ppiSubStorage );
		_commit( );
	}

	return hr;
}

HRESULT storage_obj::_serialize_content( IStream *pistream, BOOL bis_storing /*= TRUE*/  )
{
	return S_OK;
}


//////////////////////////// nested classes implementation /////////////////////////////

//////////////////////////////////////////////////////////////////////
//
// nested struct storage_obj::XStorageObjInfo
//
//////////////////////////////////////////////////////////////////////
storage_obj::XStorageObjInfo::XStorageObjInfo( )
{	
	InitDefault( );
}

storage_obj::XStorageObjInfo::~XStorageObjInfo( )
{	
	Reset( );
}

void	storage_obj::XStorageObjInfo::InitDefault( )
{
	nVersion = 1;
	ulNextRecordNumber = 1;
}

void	storage_obj::XStorageObjInfo::Reset( )
{
	nVersion = 0;
	ulNextRecordNumber = 0;
}

ULONG	storage_obj::XStorageObjInfo::Serialize( IStream *ipStream, BOOL bIsStoring /*= TRUE*/ )
{
	if( !ipStream )
		return 0;

	ULONG	ulReadWrite = 0;
	if( bIsStoring )
	{
		ULONG	ulWrite = 0;
		if( S_OK != ipStream->Write( &nVersion,  sizeof( nVersion ), &ulWrite ) )
			return 0;
		ulReadWrite += ulWrite;

		if( S_OK != ipStream->Write( &ulNextRecordNumber,  sizeof( ulNextRecordNumber ), &ulWrite ) )
			return 0;
		ulReadWrite += ulWrite;
	}
	else
	{	// loading
		ULONG	ulRead = 0;
		if( S_OK != ipStream->Read( &nVersion,  sizeof( nVersion ), &ulRead ) )
		{
			Reset( );
			return 0;
		}
		ulReadWrite += ulRead;

		if( S_OK != ipStream->Read( &ulNextRecordNumber,  sizeof( ulNextRecordNumber ), &ulRead ) )
		{
			Reset( );
			return 0;
		}
		ulReadWrite += ulRead;
	}

	return ulReadWrite;
}


//////////////////////////////////////////////////////////////////////
//
// nested struct storage_obj::XStreamObjInfo
//
//////////////////////////////////////////////////////////////////////
storage_obj::XStreamObjInfo::XStreamObjInfo(void)
{ 
	InitDefault( ); 
}

storage_obj::XStreamObjInfo::~XStreamObjInfo(void)
{	
	Reset( ); 
}

void	storage_obj::XStreamObjInfo::Reset(void)
{
	nVersion = 0;
	if( lpwstrObjName && m_bIAllocText )
	{
		delete[] lpwstrObjName;
		lpwstrObjName = 0;
		nSzObjName = 0;
		m_bIAllocText = false;
	}
}

void	storage_obj::XStreamObjInfo::InitDefault(void)
{
	nVersion = 1;
	lpwstrObjName = 0;
	nSzObjName = 0;
	m_bIAllocText = false;
}

void storage_obj::XStreamObjInfo::text_copy( LPCWSTR lpcwstr )
{
	if( !lpcwstr )
		return;
	if( lpwstrObjName && m_bIAllocText )
	{
		delete[] lpwstrObjName;
		lpwstrObjName = 0;
		nSzObjName = 0;
		m_bIAllocText = false;
	}
	if( !lpwstrObjName && !m_bIAllocText )
	{
		nSzObjName = wcslen( lpcwstr ) + 1;
		lpwstrObjName = new WCHAR[ nSzObjName ];
		m_bIAllocText = true;
		wcscpy( lpwstrObjName, lpcwstr );
	}
}

ULONG	storage_obj::XStreamObjInfo::Serialize( IStream *ipStream, BOOL bIsStoring /*= TRUE*/ )
{
	if( !ipStream )
		return 0;

	ULONG	ulReadWrite = 0;
	if( bIsStoring )	
	{
		ULONG	ulWrite = 0;
		if( S_OK != ipStream->Write( &nVersion,  sizeof( nVersion ), &ulWrite ) )
			return 0;
		ulReadWrite += ulWrite;

		long lObjNameLength = 0;
		if( lpwstrObjName )
			lObjNameLength = wcslen( lpwstrObjName );

		if( S_OK != ipStream->Write( &lObjNameLength, sizeof( lObjNameLength ), &ulWrite ) )
			return 0;
		ulReadWrite += ulWrite;

		if( S_OK != ipStream->Write( lpwstrObjName, sizeof(WCHAR) * (lObjNameLength + 1), &ulWrite ) )
			return 0;
		ulReadWrite += ulWrite;
	}
	else
	{	// loading

		Reset( );
		ULONG	ulRead = 0;
		if( S_OK != ipStream->Read( &nVersion,  sizeof( nVersion ), &ulRead ) )
		{
			Reset( );
			return 0;
		}
		ulReadWrite += ulRead;

		long lObjNameLength = 0;	
		if( S_OK != ipStream->Read( &lObjNameLength, sizeof( lObjNameLength ), &ulRead ) )
		{
			Reset( );
			return 0;
		}
		ulReadWrite += ulRead;

		nSzObjName = lObjNameLength + 1;
		lpwstrObjName = new WCHAR[ nSzObjName ];
		m_bIAllocText = true;

		if( S_OK != ipStream->Read( lpwstrObjName, sizeof(WCHAR) * (lObjNameLength + 1), &ulRead ) )
		{			
			Reset( );
			return 0;
		}
        ulReadWrite += ulRead;
	}
	return ulReadWrite;
}

