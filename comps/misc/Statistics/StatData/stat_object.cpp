#include "stdafx.h"
#include "resource.h"
#include "stat_object.h"
#include "stat_utils.h"
#import "msado15.dll" rename_namespace("ADO") rename("EOF", "ADOEOF")
#include "statui_int.h"

#include "\vt5\awin\profiler.h"

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableObject
//
//////////////////////////////////////////////////////////////////////
CStatTableObject::CStatTableObject()
: lNextManMeasure(MANUAL_PARAMS_FIRST_KEY)
, lNextUserMeasure(MEAS_USER_PARAMS_FIRST_KEY)
{
	default_init();

	//load type lib && type info
	m_pi_type_lib	= 0;
	m_pi_type_info	= 0;
	{
		char sz_path[MAX_PATH];	sz_path[0] = 0;
		::GetModuleFileName( 0, sz_path, sizeof(sz_path) );
		char* psz_last = strrchr( sz_path, '\\' );
		if( psz_last )
		{
			psz_last++;
			strcpy( psz_last, "comps\\statdata.tlb" );
			HRESULT hr = ::LoadTypeLib( _bstr_t( sz_path ), &m_pi_type_lib );
			_ASSERTE( S_OK == hr );

			if( m_pi_type_lib )
			{
				hr = m_pi_type_lib->GetTypeInfoOfGuid( IID_IStatTableDisp, &m_pi_type_info );
				_ASSERTE( S_OK == hr );
			}
		}		
	}
	Register( ::GetAppUnknown(), true, szEventNewSettings );
//	Register( ::GetAppUnknown(), true, szEventNewSettingsView );
}


//////////////////////////////////////////////////////////////////////
CStatTableObject::~CStatTableObject()
{
	Register( ::GetAppUnknown(), false, szEventNewSettings );
//	Register( ::GetAppUnknown(), false, szEventNewSettingsView );
	deinit();

	if( m_pi_type_info )	m_pi_type_info->Release();	m_pi_type_info = 0;
	if( m_pi_type_lib )		m_pi_type_lib->Release();	m_pi_type_lib = 0;
}

//////////////////////////////////////////////////////////////////////
void CStatTableObject::default_init()
{
	m_str_class_name = "";
}

//////////////////////////////////////////////////////////////////////
void CStatTableObject::deinit()
{
	//deinit params
	m_map_params.clear();
	m_list_params.clear();

	//deinit row
	m_list_row.clear();

	//deinit groups
	m_list_group.clear();
}

//IPersist helper
//////////////////////////////////////////////////////////////////////
GuidKey CStatTableObject::GetInternalClassID()
{
	return clsidStatTableObject;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CStatTableObject::DoGetInterface( const IID &iid )
{
	if( iid == IID_IStatTable )return (IStatTable*)this;
	if( iid == IID_ICompatibleObject )return (ICompatibleObject*)this;	
	if( iid == IID_IStatTablePersist )return (IStatTablePersist*)this;
	if( iid == IID_IStatTableDisp )return (IStatTableDisp*)this;
	if( iid == IID_IDispatch )return (IDispatch*)this;
	if( iid == IID_IEventListener ) return (IEventListener*)this;
	else return CObjectBase::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
//interface ISerializableObject
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::Load( IStream *pStream, SerializeParams *pparams )
{
	//TIME_TEST( "CStatTableObject::Load()" );

	deinit();

	HRESULT hr = CObjectBase::Load( pStream, pparams );
	if( S_OK != hr )
		return hr;

	LoadData( pStream, 0 );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::Store( IStream *pStream, SerializeParams *pparams )
{
	//TIME_TEST( "CStatTableObject::Store()" );
	//PROFILE_TEST( "CStatTableObject::Store()" );

	HRESULT hr = CObjectBase::Store( pStream, pparams );
	if( S_OK != hr )
		return hr;

	StoreData( pStream, 0 );

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//interface INamedDataObject2
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::IsBaseObject( BOOL * pbFlag )
{
	if( !pbFlag )	return E_POINTER;

	*pbFlag = TRUE;
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetFirstChildPosition(TPOS *plPos)
{
	return GetFirstRowPos( plPos );
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetNextChild(TPOS *plPos, IUnknown **ppunkChild)
{
	stat_row* prow = 0;
	HRESULT hr = GetNextRow( plPos, &prow );
	if( hr != S_OK )	return hr;

	if( !ppunkChild )
		return E_POINTER;

	stat_row_ex* prow_ex = (stat_row_ex*)prow;

	*ppunkChild = prow_ex->Unknown();
	prow_ex->AddRef();
	return S_OK;	
}


//////////////////////////////////////////////////////////////////////
//interface IStatTable
//////////////////////////////////////////////////////////////////////
//parameter info
HRESULT CStatTableObject::GetFirstParamPos( TPOS* plpos )
{
	if( !plpos )	return E_POINTER;

	*plpos = (TPOS)m_list_params.head();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetNextParam( TPOS* plpos, stat_param** ppparam )
{	
	if( !plpos || !ppparam )	return E_POINTER;

	*ppparam = (stat_param*)m_list_params.get(*plpos);
	*plpos = m_list_params.next(*plpos);
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetParamsCount( long* pl_count )
{
	if( !pl_count )	return E_POINTER;

	*pl_count = m_list_params.count();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::AddParam( TPOS lpos_insert_after, stat_param* pparam, TPOS* pl_pos_new )
{	
	if( !pparam )	return E_POINTER;

	//add to list
	stat_param_ex* pnew_param = new stat_param_ex;
	copy_stat_param( pparam, pnew_param );

	//add param to list
	TPOS lnew_pos = 0;
	if( lpos_insert_after )
		lnew_pos = m_list_params.insert_after( pnew_param, lpos_insert_after );
	else
		lnew_pos = m_list_params.add_tail( pnew_param );

	//add to map
	m_map_params.set( lnew_pos, pnew_param->lkey );

	if( pl_pos_new )
		*pl_pos_new = lnew_pos;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::DeleteParam( TPOS lpos )
{
	if( !lpos )		return E_INVALIDARG;
	
	//get param info
	stat_param_ex* psp = m_list_params.get(lpos );

	//remove this param from all row
	{
		for (TPOS lpos_row = m_list_row.head(); lpos_row; lpos = m_list_row.next(lpos_row))
		{
			stat_row_ex* prow = m_list_row.get( lpos_row );
			TPOS lpos_map = prow->m_map_value.find(psp->lkey);
			if( lpos_map )
				prow->m_map_value.remove( lpos_map );
		}
	}


	//remove from map
	TPOS lmap_pos = m_map_params.find(psp->lkey);
	_ASSERTE( lmap_pos );
	m_map_params.remove( lmap_pos );

	//remove from param
	m_list_params.remove(lpos);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetParamPosByKey( long lkey, TPOS* pl_pos )
{
	if( !pl_pos )	return E_POINTER;
    
	TPOS lmap_pos = m_map_params.find(lkey);
	if( !lmap_pos )	return S_FALSE;

	*pl_pos = m_map_params.get( lmap_pos );
	return S_OK;
}

//group
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetFirstGroupPos(TPOS* plpos)
{
	if( !plpos )	return E_POINTER;

	*plpos = (TPOS)m_list_group.head();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetNextGroup(TPOS* plpos, stat_group** ppgroup)
{
	if( !plpos || !ppgroup )	return E_POINTER;

	*ppgroup = m_list_group.get( *plpos );
	*plpos = (TPOS)m_list_group.next(*plpos);
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::AddGroup(TPOS lpos_insert_after, stat_group* pgroup, TPOS* pl_pos_new)
{
	//add to list
	stat_group_ex* pnew_group = new stat_group_ex;
	copy_stat_group( pgroup, pnew_group );

	//add param to list
	TPOS lnew_pos = 0;
	if( lpos_insert_after )
		lnew_pos = m_list_group.insert_after(pnew_group, lpos_insert_after);
	else
		lnew_pos = m_list_group.add_tail( pnew_group );

	if( pl_pos_new )
		*pl_pos_new = lnew_pos;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetGroupCount( long* pl_count )
{
	if( !pl_count )	return E_INVALIDARG;
	*pl_count = m_list_group.count();

	return S_OK;
}

HRESULT CStatTableObject::getGroup( /*[in]*/long posIndex, /* [out, retval]*/VARIANT* pv_group )
{
	if( posIndex<0 || posIndex>= m_list_group.count() )
	{
		return E_INVALIDARG;
	}
	TPOS lpos = m_list_group.head();
	for(int index=0; index<posIndex; ++index)
	{
		lpos = m_list_group.next( lpos );
	}
	stat_group& groupStat = *m_list_group.get(lpos);

	
	CComSafeArray<VARIANT> array(9);
	array[0]=groupStat.guid_group;

	array[1]=COleDateTime(groupStat.time_send);
	VARIANT& var=array[1];
	VariantChangeType(&var,&var,0,VT_DATE);

	array[2]=groupStat.guid_doc;
	array[3]=groupStat.bstr_doc_name;
	array[4]=groupStat.guid_image;
	array[5]=groupStat.bstr_image_name;
	array[6]=groupStat.guid_object_list;
	array[7]=groupStat.bstr_object_list_name;
	array[8]=groupStat.bstr_user_data;

	//CComVariant var=array;
	//var.Detach(pv_group);
	pv_group->vt = VT_ARRAY | VT_VARIANT;
	pv_group->parray=array.Detach();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::DeleteGroup(TPOS lpos, BOOL bsync_rows)
{
	if( !lpos )	return E_INVALIDARG;

	stat_group_ex* pgroup = m_list_group.get( lpos );

	//delete row, associate with this group
	if( bsync_rows )
	{
		TPOS lpos_row = 0;
		GetFirstRowPos( &lpos_row );
		while( lpos_row )
		{
			stat_row* prow = 0;
			TPOS lpos_save = lpos_row;
			GetNextRow( &lpos_row, &prow );

			if( pgroup->guid_group == prow->guid_group )
				DeleteRow( lpos_save );
		}
	}

	m_list_group.remove( lpos );

	return S_OK;
}


//row info
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetFirstRowPos(TPOS* pl_pos)
{
	if( !pl_pos )	return E_POINTER;
	
	*pl_pos = (TPOS)m_list_row.head();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetNextRow(TPOS* pl_pos, stat_row** pprow)
{
	if( !pl_pos || !pprow )	return E_POINTER;

	*pprow = m_list_row.get(*pl_pos);
	*pl_pos = m_list_row.next(*pl_pos);
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetRowCount( long* pl_count )
{
	if( !pl_count )		return E_POINTER;

	*pl_count = m_list_row.count();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::AddRow(TPOS lpos_insert_after, stat_row* prow, TPOS* pl_pos_new)
{
	if( !prow )	return E_POINTER;

	stat_row_ex* pnew_row = new stat_row_ex;
	copy_stat_row( prow, pnew_row );

	TPOS lpos_new = 0;

	if( lpos_insert_after )
		lpos_new = m_list_row.insert_after( pnew_row, lpos_insert_after );
	else
		lpos_new = m_list_row.add_tail( pnew_row );

	if( pl_pos_new )
		LPOS lpos_new = 0;
	*pl_pos_new = lpos_new;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::DeleteRow( TPOS lpos_row )
{
	if( !lpos_row )	return E_INVALIDARG;

	m_list_row.remove(lpos_row);
	return S_OK;
}

//value by param
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetValue(TPOS lpos_row, TPOS lpos_param, stat_value** ppvalue)
{
	if( !lpos_row || !lpos_param || !ppvalue )	return E_INVALIDARG;

	//get row
	stat_row_ex* prow = m_list_row.get(lpos_row );	

	//get param
	stat_param_ex* pparam = m_list_params.get(lpos_param);

	TPOS lpos_value = prow->m_map_value.find(pparam->lkey);
	if( !lpos_value )
		return S_FALSE;

	stat_value_ex* pvalue_ex = prow->m_map_value.get( lpos_value );
	//if class replace value
	if( pparam->lkey == KEY_CLASS )
	{
		ICalcObjectPtr ptr_calc( prow->Unknown() );
		if( ptr_calc )
		{
			pvalue_ex->bwas_defined = true;
			//if( !m_str_class_name.length() )
			//	m_str_class_name = short_classifiername( 0 );
			pvalue_ex->fvalue = get_object_class( ptr_calc, m_str_class_name );
		}
	}

	*ppvalue = pvalue_ex;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::SetValue(TPOS lpos_row, TPOS lpos_param, stat_value* pvalue)
{
	if( !lpos_row || !lpos_param )	return E_INVALIDARG;
	
	//get row
	stat_row_ex* prow = m_list_row.get(lpos_row);

	//get param
	stat_param_ex* pparam = m_list_params.get(lpos_param);

	//set value
	{
		stat_value_ex* pvi_map = 0;
		TPOS lpos_value = prow->m_map_value.find(pparam->lkey);

		if( pvalue )//add/edit new value
		{
			if( lpos_value )
				pvi_map = prow->m_map_value.get( lpos_value );
			else
			{
				pvi_map = new stat_value_ex;
				lpos_value = prow->m_map_value.set( pvi_map, pparam->lkey );
			}
			
			copy_stat_value( pvalue, pvi_map );
			pvi_map->m_lkey	= pparam->lkey;
		}
		else//delete
		{
			if( lpos_value )
				prow->m_map_value.remove( lpos_value );
		}
	}
	
	return S_OK;
}


//value by key
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetValueByKey(TPOS lpos_row, long lkey, stat_value** ppvalue)
{
	TPOS lpos_param = 0;
	get_param_by_key( lkey, &lpos_param );
	return GetValue( lpos_row, lpos_param, ppvalue );
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::SetValueByKey(TPOS lpos_row, long lkey, stat_value* pvalue)
{
	TPOS lpos_param = 0;
	get_param_by_key( lkey, &lpos_param );
	return SetValue( lpos_row, lpos_param, pvalue );
}

HRESULT CStatTableObject::ClearCache( )
{
	m_str_class_name = "";
	return S_OK;
}

//helper func
//////////////////////////////////////////////////////////////////////
stat_param_ex* CStatTableObject::get_param_by_key(long lkey, TPOS* pl_pos)
{
	TPOS lpos_map = m_map_params.find(lkey);
	if( !lpos_map )	return 0;

	TPOS lpos_param = m_map_params.get(lpos_map);
	stat_param_ex* pparam = m_list_params.get( lpos_param );
	if( pl_pos )
		*pl_pos = lpos_param;

	return pparam;
}

//ICompatibleObject
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize )
{
	PROFILE_TEST( "CStatTableObject::CreateCompatibleObject" );
	TIME_TEST( "CStatTableObject::CreateCompatibleObject" );

	IStatTablePersistPtr ptr_src( punkObjSource );
	if( ptr_src == 0 )	return HRESULT_FROM_WIN32(ERROR_INVALID_PARAMETER);

	IDispatchPtr pDispStream;//(__uuidof(ADO::_Stream));
	HRESULT hr=pDispStream.CreateInstance(L"ADODB.Stream");//,0,CLSCTX_INPROC_SERVER);
//	adoStream->Mode = ADO::adModeReadWrite; //read/write mode
	DISPID dispId;
	LPWSTR pOpen=L"Open";
	hr=pDispStream->GetIDsOfNames(IID_NULL,&pOpen,1,0,&dispId);
	DISPPARAMS dispParams={0};
	_variant_t	var( 0l);
	hr=pDispStream->Invoke(dispId,IID_NULL,0,DISPATCH_METHOD,&dispParams,&var,NULL,NULL);

	ADO::_StreamPtr adoStream=pDispStream;
	if( adoStream == 0 ){
		::MessageBox(NULL,_T("ADODB stream wasn't created!"),_T("Error"),MB_ICONWARNING);
		return hr;
	}
	adoStream->Type = ADO::adTypeBinary ;//- Set it as binary
	_ASSERTE(!hr);
	ADO::StreamTypeEnum streamType;
	hr=adoStream->get_Type(&streamType);

	IStreamPtr ptr_stream=adoStream;
	
	//{
	//	IStream* pi_stream = 0;
	//	::CreateStreamOnHGlobal( 0, TRUE, &pi_stream );
	//	if( !pi_stream )
	//		return S_FALSE;

	//	ptr_stream = pi_stream;
	//	pi_stream->Release();	pi_stream = 0;
	//}

	
//	ULARGE_INTEGER ul={0x02000000,0};
//	hr=ptr_stream->SetSize(ul);

	ptr_src->StoreData( ptr_stream, SF_SERIALIZE_NAMED_DATA );	
	LARGE_INTEGER dlibMove = {0};
	ptr_stream->Seek( dlibMove, STREAM_SEEK_SET, 0 );
	LoadData( ptr_stream, SF_SERIALIZE_NAMED_DATA );

	return S_OK;
}

//interface IStatTablePersist
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::LoadData( IStream *pStream, long lparam )
{
	PROFILE_TEST( "CStatTableObject::LoadData( IStream *pStream, long lparam )" );
	TIME_TEST( "CStatTableObject::LoadData" );
	
	ULONG nRead = 0;
	//At first version control
	long nVersion  = 1;
	pStream->Read( &nVersion, sizeof(long), &nRead );

	// Synchronize Params keys
	MapPrivateKeys mapPrivateKeys;
	HRESULT hrAppend=AppendParamsFromND(Unknown(),mapPrivateKeys);

	//load params
	long count = 0;
	long idx = 0;
	{
		//TIME_TEST( "CStatTableObject::LoadData->LoadParams" );

		pStream->Read( &count, sizeof(long), &nRead );
		for( idx=0; idx<count; idx++ )
		{
			stat_param_ex* pparam = new stat_param_ex;
			TPOS lnew_pos = m_list_params.add_tail(pparam);
			//load param key
			pStream->Read( &pparam->lkey, sizeof(long), &nRead );
			pparam->lkey=mapPrivateKeys.replace(pparam->lkey);
			//add to map
			m_map_params.set( lnew_pos, pparam->lkey );		
		}
	}

	//load groups
	count = 0;
	{
		//TIME_TEST( "CStatTableObject::LoadData->LoadGroups" );

		pStream->Read( &count, sizeof(long), &nRead );
		for( idx=0; idx<count; idx++ )
		{
			stat_group_ex* pgroup = new stat_group_ex;
			m_list_group.add_tail( pgroup );
			pStream->Read( &pgroup->size, sizeof(unsigned), &nRead );
			//sending guid and time
			pStream->Read( &pgroup->guid_group, sizeof(GUID), &nRead );
			pStream->Read( &pgroup->time_send, sizeof(SYSTEMTIME), &nRead );
			//document
			pStream->Read( &pgroup->guid_doc, sizeof(GUID), &nRead );
			ReadBSTR( pStream, &pgroup->bstr_doc_name );
			//image
			pStream->Read( &pgroup->guid_image, sizeof(GUID), &nRead );
			ReadBSTR( pStream, &pgroup->bstr_image_name );
			//object_list
			pStream->Read( &pgroup->guid_object_list, sizeof(GUID), &nRead );
			ReadBSTR( pStream, &pgroup->bstr_object_list_name );
			//user data
			ReadBSTR( pStream, &pgroup->bstr_user_data  );
		}
	}

	{
		//TIME_TEST( "CStatTableObject::LoadData->LoadRows" );
		//__int64 ltime = 0;
		//load rows
		count = 0;
		pStream->Read( &count, sizeof(long), &nRead );
		for( idx=0; idx<count; idx++ )
		{
			stat_row_ex* prow = new stat_row_ex;
			m_list_row.add_tail( prow );
			//load image
			BOOL bimage = FALSE;
			pStream->Read( &bimage, sizeof(BOOL), &nRead );
			if( bimage )
			{
				SerializeParams sp = {0};
				sp.flags = sf_ImageCompressed;

				prow->punk_image = ::CreateTypedObject( _bstr_t(szTypeImage) );
				ISerializableObjectPtr ptr_so( prow->punk_image );
				if( ptr_so )
					ptr_so->Load( pStream, &sp );
			}
			//read group guid
			pStream->Read( &prow->guid_group, sizeof(GUID), &nRead );
			//read object guid
			pStream->Read( &prow->guid_object, sizeof(GUID), &nRead );
			//read property bag
			{
				//PROFILE_TEST( "prow->Load( pStream )" );
				//__int64 l1 = __cputick__();
				prow->Load( pStream );
				//ltime += __cputick__() - l1;
			}
			
			//read stat_values
			long value_count = 0;
			pStream->Read( &value_count, sizeof(long), &nRead );
			for( long idx_value=0; idx_value<value_count; idx_value++ )
			{
				stat_value_ex* pvalue = new stat_value_ex;			
				//save value was defined
				pStream->Read( &pvalue->bwas_defined, sizeof(bool), &nRead );
				//save value
				pStream->Read( &pvalue->fvalue, sizeof(double), &nRead );
				//save param key
				pStream->Read( &pvalue->m_lkey, sizeof(long), &nRead );
				//add to map
				prow->m_map_value.set( pvalue, mapPrivateKeys.replace(pvalue->m_lkey) );
			}		
		}
		//__int64 time_ms = ltime / __cpuspeed__();
		//char sz_trace[200];
		//sprintf( sz_trace, "CStatTableObject::LoadData->LoadPropBag: %d\n", (int)time_ms );
		//OutputDebugString( sz_trace );
	}

	if( nVersion >=2 )
	{
		//TIME_TEST( "CStatTableObject::LoadData->LoadND" );

		if( lparam == SF_SERIALIZE_NAMED_DATA )
		{
			long ldata_save = 0;
			pStream->Read( &ldata_save, sizeof(ldata_save), &nRead );		

			if( ldata_save == 1 )
			{
				if( !m_punkAggrNamedData )
					InitAttachedData();

				ISerializableObjectPtr	ptr_s( m_punkAggrNamedData );
				if( ptr_s )
				{
					SerializeParams	params;
					params.flags = 0;
					params.punkNamedData = (INamedData*)m_punkAggrNamedData;
					params.punkCurrentParent = 0;
					ptr_s->Load( pStream, &params );
				}
			}
		}
	}
	if(hrAppend!=S_OK)
		hrAppend=AppendParamsFromND(Unknown(),mapPrivateKeys);
	if(!hrAppend){
		hrAppend=SyncParamKeys(mapPrivateKeys);
	}
	
	if(INamedDataPtr pND=Unknown())
	{
		LoadColorsNames(pND);
	}
	else{
		_ASSERTE(!"No Colors & Names for classes");
	}
		//CComVariant vClassifierShortName;
		//pND->GetValue(CComBSTR("\\ClassifierName"), &vClassifierShortName);
		//if(VT_BSTR==vClassifierShortName.vt)
		//{
		//	UpdateColorsNames(vClassifierShortName.bstrVal);
		//}

	return hrAppend;
}

bool CStatTableObject::SyncParamKeys(MapPrivateKeys& mapPrivateKeys)
{
	if(INamedDataPtr pND=Unknown())
	{
		long keyPrivate=::GetValueInt(Unknown(), SECT_DEPENDENCE_CHART_AXIS_ROOT, X_PARAM_KEY,0);
		long keyShell=mapPrivateKeys.replace(keyPrivate);
		if(keyPrivate!=keyShell){
			::SetValue(Unknown(), SECT_DEPENDENCE_CHART_AXIS_ROOT, X_PARAM_KEY, keyShell);
		}
		keyPrivate=::GetValueInt(Unknown(), SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_PARAM_KEY,0);
		keyShell=mapPrivateKeys.replace(keyPrivate);
		if(keyPrivate!=keyShell){
			::SetValue(Unknown(), SECT_DEPENDENCE_CHART_AXIS_ROOT, Y_PARAM_KEY, keyShell);
		}

		pND->SetupSection(_bstr_t(_T(STAT_TABLE_GRID)));
		long cnt; pND->GetEntriesCount(&cnt);
		for(long idx=0; idx<cnt; ++idx)
		{
			_bstr_t sEntry;
			pND->GetEntryName(idx,sEntry.GetAddress());
			if(!wcsncmp((wchar_t*)sEntry,L"Key",3)){
				keyPrivate=-333L;
				keyPrivate=_wtol((wchar_t*)sEntry+3);
				if(keyPrivate!=-333L){
					keyShell=mapPrivateKeys.replace(keyPrivate);
					if(keyPrivate!=keyShell){
						pND->RenameEntry(sEntry,_bstr_t(L"Key")+_bstr_t(keyShell));
					}
				}
			}
		}

		_variant_t varOrder;
		pND->GetValue(_bstr_t(_T(ST_KEY_ORDER)), &varOrder);
		if(VT_BSTR==varOrder.vt){
			_bstr_t keyOrders;
			bool bReplace=false;
			wchar_t* token = wcstok(varOrder.bstrVal, L",");
			for(int i=0; token != NULL; ++i)
			{
				keyPrivate=_wtol(token);
				keyShell=mapPrivateKeys.replace(keyPrivate);
				if(keyPrivate!=keyShell){
					bReplace=true;
				}
				if(0==i){
					keyOrders += _bstr_t(keyShell);
				}else{
					keyOrders += L","+_bstr_t(keyShell);
				}
				token = wcstok( NULL, L",");
			}
			if(bReplace)
				pND->SetValue(_bstr_t(_T(ST_KEY_ORDER)), _variant_t(keyOrders));
		}

		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::StoreData( IStream *pStream, long lparam )
{
	PROFILE_TEST( "CStatTableObject::StoreData" );
	TIME_TEST( "CStatTableObject::StoreData" );
	bool benable_compress = ( 1L == ::GetValueInt( ::GetAppUnknown(), STAT_TABLE_SECTION, COMPRESS_IMAGE, 1 ) );

	ULONG nWritten = 0;

	//At first version control
	long nVersion  = 2;
	pStream->Write( &nVersion, sizeof(long), &nWritten );

	//store params
	TPOS lpos = 0;
	long count = m_list_params.count();
	{
		//TIME_TEST( "CStatTableObject::StoreData->StoreParams" );
		pStream->Write( &count, sizeof(long), &nWritten );
		for( lpos=m_list_params.head(); lpos; lpos=m_list_params.next( lpos ) )
		{
			stat_param_ex* pparam = m_list_params.get( lpos );
			//save param key
			pStream->Write( &pparam->lkey, sizeof(long), &nWritten );
		}
	}

	//store groups
	count = m_list_group.count();
	{
		//TIME_TEST( "CStatTableObject::StoreData->StoreGroups" );
		pStream->Write( &count, sizeof(long), &nWritten );
		for( lpos=m_list_group.head(); lpos; lpos=m_list_group.next( lpos ) )
		{
			stat_group_ex* pgroup = m_list_group.get( lpos );
			pStream->Write( &pgroup->size, sizeof(unsigned), &nWritten );
			//sending guid and time
			pStream->Write( &pgroup->guid_group, sizeof(GUID), &nWritten );
			pStream->Write( &pgroup->time_send, sizeof(SYSTEMTIME), &nWritten );
			//document
			pStream->Write( &pgroup->guid_doc, sizeof(GUID), &nWritten );
			WriteBSTR( pStream, pgroup->bstr_doc_name );
			//image
			pStream->Write( &pgroup->guid_image, sizeof(GUID), &nWritten );
			WriteBSTR( pStream, pgroup->bstr_image_name );
			//object_list
			pStream->Write( &pgroup->guid_object_list, sizeof(GUID), &nWritten );
			WriteBSTR( pStream, pgroup->bstr_object_list_name );
			//user data
			WriteBSTR( pStream, pgroup->bstr_user_data  );
		}
	}

	//store rows
	count = m_list_row.count();
	{
		char sz_trace[200];
		sprintf( sz_trace, "CStatTableObject::StoreData->ObjectCount: %d\n", (int)count );
		OutputDebugString( sz_trace );

		//TIME_TEST( "CStatTableObject::StoreData->StoreRows" );
		//__int64 ltime = 0;
		pStream->Write( &count, sizeof(long), &nWritten );
		for( lpos=m_list_row.head(); lpos; lpos=m_list_row.next( lpos ) )
		{
			stat_row_ex* prow = m_list_row.get( lpos );
			//store image
			BOOL bimage = ( prow->punk_image != 0 );
			pStream->Write( &bimage, sizeof(BOOL), &nWritten );
			if( bimage )
			{
				SerializeParams sp = {0};
				if( benable_compress )
					sp.flags = sf_ImageCompressed;

				ISerializableObjectPtr ptr_so( prow->punk_image );
				if( ptr_so )
					ptr_so->Store( pStream, &sp );
			}
			//Write group GUID
			pStream->Write( &prow->guid_group, sizeof(GUID), &nWritten );
			//Write object guid
			pStream->Write( &prow->guid_object, sizeof(GUID), &nWritten );
			//Write property bag
			{
				//__int64 l1 = __cputick__();
				prow->Store( pStream );
				//ltime += __cputick__() - l1;
			}
			
			//store stat_values
			long value_count = prow->m_map_value.count();
			pStream->Write( &value_count, sizeof(long), &nWritten );
			for (TPOS lpos_value = prow->m_map_value.head(); lpos_value; lpos_value = prow->m_map_value.next(lpos_value))
			{
				stat_value_ex* pvalue = prow->m_map_value.get( lpos_value );
				//save value was defined
				pStream->Write( &pvalue->bwas_defined, sizeof(bool), &nWritten );
				//save value
				pStream->Write( &pvalue->fvalue, sizeof(double), &nWritten );
				//save param key
				pStream->Write( &pvalue->m_lkey, sizeof(long), &nWritten );
			}		
		}
		//__int64 time_ms = ltime / __cpuspeed__();
		//sz_trace[200];
		//sprintf( sz_trace, "CStatTableObject::StoreData->StorePropBag: %d\n", (int)time_ms );
		//OutputDebugString( sz_trace );
	}

	if( lparam == SF_SERIALIZE_NAMED_DATA )
	{
		//TIME_TEST( "CStatTableObject::StoreData->StoreND" );

		ISerializableObjectPtr	ptr_s( m_punkAggrNamedData );
		long ldata_save = ( ptr_s != 0 ? 1 : 0 );
		pStream->Write( &ldata_save, sizeof(ldata_save), &nWritten );		
		if( ptr_s )
		{
			SerializeParams	params;
			params.flags = 0;
			params.punkNamedData = (INamedData*)m_punkAggrNamedData;
			params.punkCurrentParent = 0;
			ptr_s->Store( pStream, &params );
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo)
{
	if( !pctinfo )
		return E_POINTER;

	*pctinfo = 1;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo)
{
	if( !ppTInfo )
		return E_POINTER;

	if( !iTInfo )
		return E_INVALIDARG;

	if( !m_pi_type_info )
		return E_FAIL;

	m_pi_type_info->AddRef();

	(*ppTInfo) = m_pi_type_info;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->GetIDsOfNames( rgszNames, cNames, rgDispId );
}


//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
{
	if( !m_pi_type_info )
		return E_FAIL;

	return m_pi_type_info->Invoke(	(IUnknown*)(IStatTableDisp*)this, dispIdMember, wFlags, 
									pDispParams, pVarResult, pExcepInfo, puArgErr );
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getFirstParamPos (
        /*[out,retval]*/ VARIANT * pv_pos )
{
	if( !pv_pos )		return E_POINTER;

	TPOS lpos = 0;
	GetFirstParamPos( &lpos );
	_variant_t var	= (LONG_PTR)lpos;
	*pv_pos	= var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getNextParamPos (
	/*[in]*/ LONG_PTR lpos,
    /*[out,retval]*/ VARIANT * pv_pos )
{
	if( !pv_pos || !lpos )		return E_POINTER;

	TPOS lpos_param = (TPOS)lpos;
	stat_param* pparam = 0;
	GetNextParam( &lpos_param, &pparam );
	_variant_t var = (LONG_PTR)lpos_param;
	*pv_pos	= var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getParamKey (
	/*[in]*/ LONG_PTR lpos,
  /*[out,retval]*/ VARIANT * pv_key )
{
	if( !pv_key || !lpos )		return E_POINTER;

	TPOS lpos_param = (TPOS)lpos;
	stat_param* pparam = 0;
	GetNextParam( &lpos_param, &pparam );
	_variant_t var	= pparam->lkey;
	*pv_key	= var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getFirstRowPos (
        /*[out,retval]*/ VARIANT * pv_pos )
{
	if( !pv_pos )		return E_POINTER;

	LONG_PTR lpos = 0;
	GetFirstRowPos( (TPOS*)&lpos );
	_variant_t var	= lpos;
	*pv_pos	= var.Detach();

	return S_OK;
}
    
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getNextRowPos (
        /*[in]*/ LONG_PTR lpos,
        /*[out,retval]*/ VARIANT * pv_pos )
{
	if( !pv_pos || !lpos )		return E_POINTER;

	LONG_PTR lpos_row = lpos;
	stat_row* prow = 0;
	GetNextRow( (TPOS*)&lpos_row, &prow );
	_variant_t var	= lpos_row;
	*pv_pos	= var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getRowCount (
        /*[out,retval]*/ VARIANT * pv_count )
{
	if( !pv_count )	return E_POINTER;
	long lcount = 0;
	GetRowCount( &lcount );
	_variant_t var	= lcount;
	*pv_count	= var.Detach();
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getRowGroup (
	/*[in]*/ LONG_PTR lpos,
        /*[out,retval]*/ VARIANT * pv_group )
{
	if( !pv_group || !lpos )		return E_POINTER;

	LONG_PTR lpos_row = lpos;
	stat_row* prow = 0;
	GetNextRow( (TPOS*)&lpos_row, &prow );	

	BSTR bstr = 0;
	StringFromCLSID( prow->guid_group, &bstr );
	_bstr_t bstr_group = bstr;
	::CoTaskMemFree( bstr );	bstr = 0;
	
	_variant_t var	= bstr_group;
	*pv_group	= var.Detach();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getRowClass (
	/*[in]*/ LONG_PTR lpos,
  /*[in]*/ BSTR class_file,
  /*[out,retval]*/ VARIANT * pv_class )
{
	if( !lpos || !class_file || !pv_class )	return E_POINTER;

	long lclass = -1;

	bstr_t bstr_classes_file = class_file;
	TPOS lpos_calc = (TPOS)lpos;

	IUnknown* punk_child = 0;
	GetNextChild( &lpos_calc, &punk_child );
	if( punk_child )
	{
		ICalcObjectPtr ptr_calc = punk_child;
		punk_child->Release();	punk_child = 0;

		if( ptr_calc )
			lclass = get_object_class( ptr_calc, bstr_classes_file.length() ? (char*)bstr_classes_file : 0 );
	}	

	_variant_t var	= lclass;
	*pv_class	= var.Detach();
	
	return S_OK;
}
    
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::deleteRow (
	/*[in]*/ LONG_PTR lpos)
{
	if( !lpos )	return E_POINTER;
	DeleteRow( (TPOS)lpos );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getValueByKey (
	/*[in]*/ LONG_PTR lpos,
        /*[in]*/ long lkey,
        /*[out,retval]*/ VARIANT * pv_value )
{
	if( !pv_value )	return E_POINTER;

	double fvalue = 0;
	stat_value* psv = 0;
	GetValueByKey( (TPOS)lpos, lkey, &psv );
	if( psv )
		fvalue = psv->fvalue;

	_variant_t var	= fvalue;
	*pv_value		= var.Detach();	

	return S_OK;
}

static double GetCoeffToUnit(long lKey)
{
	IUnknownPtr punk_man(::_GetOtherComponent( ::GetAppUnknown(), IID_IMeasureManager ), false);
	if( punk_man==0 )			return 1.;

	ICompManagerPtr ptr_meas_man = punk_man;
	if( ptr_meas_man == 0 )	return false;

	int ncount = 0;
	ptr_meas_man->GetCount( &ncount );
	for( int i=0;i<ncount;i++ )
	{
		IUnknownPtr punk = 0;
		ptr_meas_man->GetComponentUnknownByIdx( i, &punk );
		if( punk==0 )	continue;

		IMeasParamGroupPtr ptr_group = punk;
		if( ptr_group==0 ) continue;

		LONG_PTR lparam_pos = 0;
		ptr_group->GetFirstPos( &lparam_pos );
		while( lparam_pos )
		{
			ParameterDescriptor* pdescr = 0;
			ptr_group->GetNextParam( &lparam_pos, &pdescr );
			if( pdescr->lKey == lKey )
				return pdescr->fCoeffToUnits;
		}
	}
	return 1.;
};


//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getValueByKeyInUnit (
	/*[in]*/ LONG_PTR lpos,
        /*[in]*/ long lkey,
        /*[out,retval]*/ VARIANT * pv_value )
{
	if( !pv_value )	return E_POINTER;

	double fvalue = 0;
	stat_value* psv = 0;
	GetValueByKey( (TPOS)lpos, lkey, &psv );
	if( psv )
		fvalue = psv->fvalue;
	fvalue *= GetCoeffToUnit(lkey);

	_variant_t var	= fvalue;
	*pv_value		= var.Detach();	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getValueByParamPos (
	/*[in]*/ LONG_PTR lpos,
	/*[in]*/ LONG_PTR lparam_pos,
  /*[out,retval]*/ VARIANT * pv_value )
{
	if( !pv_value )	return E_POINTER;

	double fvalue = 0;
	stat_value* psv = 0;
	GetValue((TPOS)lpos, (TPOS)lparam_pos, &psv);
	if( psv )
		fvalue = psv->fvalue;

	_variant_t var	= fvalue;
	*pv_value		= var.Detach();	

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::getGroupCount (
        /*[out,retval]*/ VARIANT * pv_count )
{
	if( !pv_count )	return E_POINTER;
	long lcount = m_list_group.count();	
	_variant_t var	= lcount;
	*pv_count	= var.Detach();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::updateObject( long lflags )
{
	NotifyObjectChange( Unknown(), szEventChangeObject );
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObject::relloadSettings( long lflags )
{
	NotifyObjectChange( Unknown(), szEventObjectOptionsChange  );
	return S_OK;
}

HRESULT CStatTableObject::GetPrivateNamedData( /*[out,retval]*/ IDispatch **ppDisp )
{
	if( !ppDisp )
		return S_FALSE;

	INamedDataPtr sptrND = Unknown();

	if( sptrND == 0 )
	{
		INamedDataObject2Ptr sptrND2 = Unknown();
		sptrND2->InitAttachedData();
		sptrND = Unknown();
	}

	if( m_punkAggrNamedData )
		m_punkAggrNamedData->QueryInterface( IID_IDispatch, (LPVOID *)ppDisp );

	return S_OK;
}

// [vanek] - 01.12.2003
HRESULT CStatTableObject::get_Name(/*[out,retval]*/BSTR *pbstr )
{
    if( !pbstr )
		return S_FALSE;

	*pbstr = m_bstrName.copy();
	return S_OK;    
}

HRESULT	CStatTableObject::put_Name( BSTR bstr )
{
   return S_OK;	// Name - read only!!!
}

HRESULT	CStatTableObject::IsEmpty( /*[out, retval]*/ VARIANT_BOOL *pvbIsEmpty )
{
	if( !pvbIsEmpty )
		return E_INVALIDARG;

	*pvbIsEmpty = m_list_row.count() ? VARIANT_FALSE : VARIANT_TRUE;
	return S_OK;
}

HRESULT	CStatTableObject::GetType( /*[out, retval]*/ BSTR *pbstrType )
{
	return CObjectBase::GetType( pbstrType );
}

void CStatTableObject::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
 {
	//if( !strcmp( pszEvent, szEventNewSettings )
	//	|| !strcmp( pszEvent, szEventNewSettingsView) )
	//{
	//		UpdateColorsNames(short_classifiername(0));
	//}
 	__super::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );
}

STDMETHODIMP CStatTableObject::UpdateClassNames(void)
{
	try{
		m_str_class_name=short_classifiername(0);
		INamedDataPtr pND=Unknown();
		const CComBSTR CLASSIFIER(L"\\Classifier");
		pND->SetValue(CLASSIFIER,CComVariant(CComBSTR(m_str_class_name)));
		UpdateColorsNames(m_str_class_name);
		return S_OK;
	}catch(_com_error& err){
		return err.Error();
	}
}

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableObjectInfo
//
//////////////////////////////////////////////////////////////////////
CStatTableObjectInfo::CStatTableObjectInfo() : CDataInfoBase( clsidStatTableObject, szTypeStatTable, 0, IDI_STAT_TABLE )
{}

//////////////////////////////////////////////////////////////////////
HRESULT CStatTableObjectInfo::GetUserName( BSTR *pbstr )
{
	if( !pbstr ) return E_POINTER;

	if( !m_bstrUserName.length() )
	{
		char sz_buf[256];	sz_buf[0] = 0;
		::LoadString( App::handle(), IDS_STAT_TABLE_TYPE_NAME, sz_buf, sizeof(sz_buf) );		
		m_bstrUserName	= sz_buf;
	}

	*pbstr = m_bstrUserName.copy();

	return S_OK;
}
