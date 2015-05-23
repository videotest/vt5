#include "stdafx.h"
#include "stat_actions.h"
#include "stat_utils.h"

#include "misc_calibr.h"
#include "action_dlg.h"

#include "\vt5\com_base\stream_misc.h"

//#include "\vt5\awin\misc_ptr.h"
//#include "\vt5\awin\misc_list.h"
//#include "\vt5\awin\misc_map.h"
#include "stat_object.h"

class CSaveSetupSection_
{ // класс, занимающийся исключительно тем, что сохраняет текущую секцию, а при смерти - восстанавливает ее
public:
	CSaveSetupSection_(IUnknown* punkND)
	{
		m_sptrND = punkND;
		if(m_sptrND!=0) m_sptrND->GetCurrentSection(&m_bstrSection.GetBSTR());
	}
	~CSaveSetupSection_()
	{
		if(m_sptrND!=0) m_sptrND->SetupSection(m_bstrSection);
	}
private:
	INamedDataPtr m_sptrND;
	_bstr_t m_bstrSection;
};

bool IsUndoUnable()
{
	return ( ::GetValueInt( ::GetAppUnknown(), STAT_TABLE_SECTION, ENABLE_UNDO, 1L ) == 1L );
}

IStreamPtr CreateMemStream()
{
	IStream* pi_stream = 0;	
	::CreateStreamOnHGlobal( 0, TRUE, &pi_stream );
	if( !pi_stream )	return 0;

	IStreamPtr ptrStream = pi_stream;
	pi_stream->Release();

	return ptrStream;
}



bool copy_object_classes( IUnknown* punk_src, IUnknown* punk_target, IStream* pi_stream )
{
	if( !punk_src || !punk_target || !pi_stream ) return false;

	INamedPropBagSerPtr ptr_src( punk_src );
	INamedPropBagSerPtr ptr_target( punk_target );

	if( ptr_src == 0 || ptr_target == 0 )
	{
		_ASSERTE( false );
		return false;
	}
	
	LARGE_INTEGER dlibMove = {0};
	pi_stream->Seek( dlibMove, STREAM_SEEK_SET, 0 );
	ptr_src->Store( pi_stream );
	pi_stream->Seek( dlibMove, STREAM_SEEK_SET, 0 );
	ptr_target->Load( pi_stream );

	return true;
}
			


//////////////////////////////////////////////////////////////////////
IStatTablePtr CreateCompatibleTable( IUnknown* punk_table_src )
{
	if( !punk_table_src )		return 0;

	IStatTablePtr ptr_new_table;

	{
		IUnknown* punk_new_table = 0;		
		punk_new_table = CreateTypedObject( _bstr_t(szTypeStatTable) );
		if( !punk_new_table )	return 0;

		ptr_new_table = punk_new_table;
		punk_new_table->Release();
	}

	if( ptr_new_table == 0 )	return 0;

	//clone table
	ICompatibleObjectPtr ptr_clone( ptr_new_table );
	if( ptr_clone == 0 )		return 0;

	HRESULT hr=ptr_clone->CreateCompatibleObject(punk_table_src, 0, 0);

	if( FAILED(hr)){
		_com_raise_error(hr);
	}

	return ptr_new_table;
}



//////////////////////////////////////////////////////////////////////
//
//	class CCreateStatTable
//
//////////////////////////////////////////////////////////////////////

_ainfo_base::arg CCreateStatTableInfo::s_pargs[] = 
{	
	{"ObjectList",		szArgumentTypeString, "", true, false },
	{"TableName",		szArgumentTypeString, "", true, false },
	{"ActivateTable",	szArgumentTypeInt, "1", true, false },
	{"SendImages",		szArgumentTypeInt, "0", true, false },
	{"VerifyType",		szArgumentTypeInt, "0", true, false },
	{"AskOverWrite",	szArgumentTypeInt, "1", true, false },
	{"EnableUndo",		szArgumentTypeInt, "1", true, false },
	{0, 0, 0, false, false },
};

//VerifyType 
#define VER_TYPE_NONE			0
#define VER_TYPE_IMAGE			1
#define VER_TYPE_OBJECT_LIST	2
#define VER_TYPE_OBJECT			3


//////////////////////////////////////////////////////////////////////
CCreateStatTable::CCreateStatTable()
{
}

//////////////////////////////////////////////////////////////////////
CCreateStatTable::~CCreateStatTable()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CCreateStatTable::DoGetInterface( const IID &iid )
{
	if( IsUndoUnable() )
		if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;

	return CAction::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT CCreateStatTable::DoInvoke()
{
	//OutputDebugString( "==================================================\n" );
	//PROFILE_TEST( "CCreateStatTable::DoInvoke" );
	//TIME_TEST( "CCreateStatTable::DoInvoke" );

	//get active object list
	INamedDataObject2Ptr ptr_ol;
	{
		IUnknown* punk = get_object_list();
		if( punk )
		{
			ptr_ol = punk;
			punk->Release();	punk = 0;
		}
	}
	if( ptr_ol == 0 )		return S_FALSE;
	
	//create, get table object
	IStatTablePtr ptr_new_table;
	IStatTablePtr ptr_exist_table;

	_bstr_t bstr_table_name = GetArgString( "TableName" );

	bool buse_undo = true;

	//is table with the same name exist?
	if( bstr_table_name.length() )
	{	
		IUnknown* punk_table_exist = ::GetObjectByName( m_ptrTarget, bstr_table_name, _bstr_t(szTypeStatTable) );
		if( punk_table_exist )
		{
			ptr_exist_table = punk_table_exist;
			punk_table_exist->Release();	punk_table_exist = 0;
		}

		//reuse table if undo is off
		if( !IsUndoUnable() && bstr_table_name == ::GetName( ptr_exist_table ) )
		{
			ptr_new_table = ptr_exist_table;
			buse_undo = false;
		}
	}

	if( buse_undo )
	{
		if( ptr_exist_table != 0 )
		{
			ptr_new_table = CreateCompatibleTable( ptr_exist_table );
		}
		else
		{
			IUnknown* punk = CreateTypedObject( _bstr_t(szTypeStatTable) );
			if( punk )
			{
				ptr_new_table = punk;
				punk->Release();	punk = 0;
			}
		}
	}

	if( ptr_new_table == 0 )	return S_FALSE;

	//set table name
	if( bstr_table_name.length() )
	{
		INamedObject2Ptr ptr_ndo2( ptr_new_table );
		if( ptr_ndo2 )
			ptr_ndo2->SetName( bstr_table_name );
	}

	//process table
	if( !process_table( ptr_new_table, ptr_ol ) )
		return false;

	if( buse_undo )
	{
		//remove old table from document
		if( ptr_exist_table )
			RemoveFromDocData( m_ptrTarget, ptr_exist_table );
		//add new table to document
		SetToDocData( m_ptrTarget, ptr_new_table );	
	}
	else
	{
		NotifyObjectChange( m_ptrTarget, szEventChangeObject );
	}

	//activate table if need
	if( GetArgLong( "ActivateTable" ) == 1 )
	{
		ExecuteAction( "ShowStatTable", 0, 0 );
		// [vanek]: activate table - 06.04.2004
		SetActiveObject( m_ptrTarget, ptr_new_table, szTypeStatTable );
	}

	return S_OK;
}

bool copy_base_params( INamedData* pi_nd_src, const char* psz_src_path, INamedData* pi_nd_target, const char* psz_target_path )
{
	if( !pi_nd_src || !psz_src_path || !pi_nd_target || !psz_target_path )	return false;
	CSaveSetupSection_ ss(pi_nd_src);//Kir SBT2085
	pi_nd_src->SetupSection( _bstr_t( psz_src_path ) );
	pi_nd_target->SetupSection( _bstr_t( psz_target_path ) );

	long EntryCount = 0;
	pi_nd_src->GetEntriesCount( &EntryCount );

	// for all entries
	for( int i = 0;i<(int)EntryCount;i++ )
	{
		BSTR bstr = NULL;
		// get entry name		
		pi_nd_src->GetEntryName( i, &bstr );
		_bstr_t bstr_entry_name = bstr;
		::SysFreeString( bstr );	bstr = 0;		

		_variant_t var;
		pi_nd_src->GetValue( bstr_entry_name, &var );
		pi_nd_target->SetValue( bstr_entry_name, var );
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CCreateStatTable::create_stat_group( IStatTable* pi_table, INamedDataObject2* pi_ol, stat_group_ex* pgroup )
{
	if( !pgroup )	return false;
	//fil stat_send_info
	//sending guid and time
	::CoCreateGuid( &pgroup->guid_group );
	::GetLocalTime( &pgroup->time_send );

	//document
	pgroup->guid_doc = ::GetKey( m_ptrTarget );
	IDocumentSitePtr ptr_ds( m_ptrTarget );
	if( ptr_ds )
		ptr_ds->GetPathName( &pgroup->bstr_doc_name );

	IUnknownPtr pUnkImage;
	if( INamedDataPtr ptr_ndDoc=m_ptrTarget)
	{
		GUID guid_base = {0};
		pi_ol->GetBaseKey( &guid_base );
		ptr_ndDoc->GetBaseGroupBaseObject( &guid_base, &pUnkImage );
	}

	//object_list
	pgroup->guid_object_list = ::GetKey( pi_ol );
	INamedObject2Ptr ptr_named( pi_ol );
	if( ptr_named )
		ptr_named->GetName( &pgroup->bstr_object_list_name );


	//copy image & object list named data base params
	{
		//init attached data if need
		INamedDataPtr ptr_nd_tbl( pi_table );
		if( ptr_nd_tbl == 0 )
		{
			INamedDataObject2Ptr ptr_ndo( pi_table );
			if( ptr_ndo )
				ptr_ndo->InitAttachedData();

			ptr_nd_tbl = ptr_ndo;
		}
		if( ptr_nd_tbl )
		{
			_bstr_t bstr_src_path = "\\";
			bstr_src_path += BASE_PARAMS_SECTION;

			INamedDataPtr ptr_nd_ol = pi_ol;

			if( ptr_nd_ol )
			{
				_bstr_t bstr_guid = "";
				{
					BSTR bstr = 0;
					::StringFromCLSID( pgroup->guid_object_list, &bstr );
					if( bstr )
					{
						bstr_guid = bstr;
						CoTaskMemFree(bstr); bstr = 0;
					}
				}					
				_bstr_t bstr_ol_path = _T("\\"BASE_PARAMS_SECTION"\\"BASE_PARAMS_OBJECTLIST"\\");
				bstr_ol_path += bstr_guid;
				copy_base_params( ptr_nd_ol, bstr_src_path, ptr_nd_tbl, bstr_ol_path );
			}
			_bstr_t bstr_guid = "";
			{
				BSTR bstr = 0;
				::StringFromCLSID( pgroup->guid_image, &bstr );
				if( bstr )
				{
					bstr_guid = bstr;
					CoTaskMemFree(bstr); bstr = 0;
				}
			}					


			int cx = 0, cy = 0;
			double	fCalibr = 1.;

			_bstr_t bstr_image_path = _T("\\"BASE_PARAMS_SECTION"\\"BASE_PARAMS_IMAGE"\\");
			_bstr_t sImageGuid;
			if(IImagePtr image=pUnkImage)
			{
				image->GetSize( &cx, &cy );

				::GetDefaultCalibration( &fCalibr, 0 );

				ICalibrPtr	ptrCalibr = image;
				if( ptrCalibr != 0 )
					ptrCalibr->GetCalibration( &fCalibr, 0 );

				GUID guidImage=GUID_NULL;
				guidImage	= ::GetKey(pUnkImage);
				if(GUID_NULL!=guidImage){
					pgroup->guid_image=guidImage;
					LPOLESTR olestrImageGuid=0;
					HRESULT hr=::StringFromCLSID(pgroup->guid_image,&olestrImageGuid);
					if(SUCCEEDED(hr)){
						sImageGuid = olestrImageGuid;
						bstr_image_path += sImageGuid;
					}
					if(olestrImageGuid)
						CoTaskMemFree(olestrImageGuid);
				}

				if(INamedDataPtr ptr_nd_image=pUnkImage)
					copy_base_params( ptr_nd_image, bstr_src_path, ptr_nd_tbl, bstr_image_path );

				if( INamedObject2Ptr ptr_named=pUnkImage)
					ptr_named->GetName( &pgroup->bstr_image_name );


			}else if(ptr_nd_ol){
				ptr_nd_ol->SetupSection(bstr_image_path);
				long cntEntries=0;
				ptr_nd_ol->GetEntriesCount(&cntEntries);
				for(int iEntry=0; iEntry<cntEntries; ++iEntry)
				{
					_bstr_t sEntry;
					ptr_nd_ol->GetEntryName(iEntry,sEntry.GetAddress());
					GUID guidImage=GUID_NULL;
					HRESULT hr=CLSIDFromString(sEntry,&guidImage);
					if(SUCCEEDED(hr) && GUID_NULL!=guidImage){
						pgroup->guid_image=guidImage;
						sImageGuid=sEntry;
						bstr_image_path += sImageGuid;
						cx=::GetValueInt( ptr_nd_ol, bstr_image_path, _bstr_t((long)BASE_PARAMS_IMAGE_WIDTH), 0 );
						cy=::GetValueInt( ptr_nd_ol, bstr_image_path, _bstr_t((long)BASE_PARAMS_IMAGE_HEIGHT), 0 );
						fCalibr=::GetValueDouble( ptr_nd_ol, bstr_image_path, _bstr_t((long)BASE_PARAMS_IMAGE_CALIBR), fCalibr);
					}
				}
			}else{
				_ASSERT(!"Image GUID Invalid!");
			}
			if(sImageGuid.length()>0){
				char sz_buf[20];
				if(0!=cx){
					sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_WIDTH );
					::SetValue( ptr_nd_tbl, bstr_image_path, sz_buf, (long)cx );
				}
				if(0!=cy){
					sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_HEIGHT );
					::SetValue( ptr_nd_tbl, bstr_image_path, sz_buf, (long)cy );
				}
				if(1.!=fCalibr){
					sprintf( sz_buf, "%d", BASE_PARAMS_IMAGE_CALIBR );
					::SetValue( ptr_nd_tbl, bstr_image_path, sz_buf, (double)fCalibr );
				}
			}else{
				_ASSERT(!"Image GUID Invalid!");
			}
		}
	}

	return true;
}


//////////////////////////////////////////////////////////////////////
bool CCreateStatTable::test_exist( IStatTable* pi_table, INamedDataObject2* pi_ol, stat_group_ex* pgroup_new )
{
	if( !pi_table || !pi_ol || !pgroup_new )
		return false;

	int nverify			= GetArgLong( "VerifyType" );
	int nask_overwrite	= GetArgLong( "AskOverWrite" );		

	if( nverify == VER_TYPE_NONE )
		return true;

	//test fof image and object list
	if( nverify == VER_TYPE_IMAGE || nverify == VER_TYPE_OBJECT_LIST )
	{
		//gather info
		_list_t<TPOS> list_group_delete;

		TPOS lpos_group = 0;
		pi_table->GetFirstGroupPos( &lpos_group );
		while( lpos_group )
		{
			stat_group* pgroup = 0;
			TPOS lpos_save = lpos_group;
			pi_table->GetNextGroup(&lpos_group, &pgroup );
			if( nverify == VER_TYPE_IMAGE )
			{
				if( pgroup->guid_image == pgroup_new->guid_image )
					list_group_delete.add_head( lpos_save );
			}
			else if( nverify == VER_TYPE_OBJECT_LIST )
			{
				if( pgroup->guid_object_list == pgroup_new->guid_object_list )
					list_group_delete.add_head( lpos_save );
			}
		}

		//is exist
		if( list_group_delete.count() )
		{
			if( nask_overwrite == 1L )
			{
				char sz_quest[255];
				if( nverify == VER_TYPE_IMAGE )
					::LoadString( App::handle(), IDS_OVEWRITE_RESULT_IMAGE, sz_quest, sizeof(sz_quest) );
				else
					::LoadString( App::handle(), IDS_OVEWRITE_RESULT_OBJECT_LIST, sz_quest, sizeof(sz_quest) );

				if( IDNO == VTMessageBox( sz_quest, 0, MB_YESNO ) )
					return false;
			}

			//delete group and row, associated with this group
			for( TPOS lpos_group=list_group_delete.head(); lpos_group; lpos_group=list_group_delete.next(lpos_group) ) 
				pi_table->DeleteGroup( list_group_delete.get( lpos_group ), true );
		}
	}
	else if( nverify == VER_TYPE_OBJECT )
	{
		_list_t<LONG_PTR> list_row_delete;

		TPOS lpos_row = 0;
		pi_table->GetFirstRowPos( &lpos_row );
		while( lpos_row )
		{
			stat_row* prow = 0;
			TPOS lpos_row_save = lpos_row;
			pi_table->GetNextRow( &lpos_row, &prow );

			POSITION lpos_child = 0;
			pi_ol->GetFirstChildPosition( (POSITION*)&lpos_child );
			while( lpos_child )
			{
				IUnknown* punk_child = 0;
				pi_ol->GetNextChild( &lpos_child, &punk_child );
				GUID guid_object = ::GetKey( punk_child );
				punk_child->Release();	punk_child = 0;

				if( guid_object == prow->guid_object )
				{
					list_row_delete.add_tail( (LONG_PTR)lpos_row_save );
					break;
				}
			}
		}
		//is exist
		if( list_row_delete.count() )
		{
			if( nask_overwrite == 1L )
			{
				char sz_quest[255];
				::LoadString( App::handle(), IDS_OVEWRITE_RESULT_OBJECT, sz_quest, sizeof(sz_quest) );
				if( IDNO == VTMessageBox( sz_quest, 0, MB_YESNO ) )
					return false;
			}

			//delete group and row, associated with this group
			for (TPOS lpos_row = list_row_delete.head(); lpos_row; lpos_row = list_row_delete.next(lpos_row))
				pi_table->DeleteRow( (TPOS&)list_row_delete.get( lpos_row ) );
		}

	}

	return true;
}

//////////////////////////////////////////////////////////////////////
bool CCreateStatTable::process_table( IStatTable* pi_table, INamedDataObject2* pi_ol )
{
	//TIME_TEST( "CCreateStatTable::process_table" );
	//PROFILE_TEST( "CCreateStatTable::process_table" );
		
	if( !pi_table || !pi_ol )			return false;

	CStatTableObject* pStatTable=(CStatTableObject*)(IStatTable*)pi_table;
		
	ICalcObjectContainerPtr ptr_cont = pi_ol;
	if( ptr_cont == 0 )	return false;

	_bstr_t bstr_class_name = short_classifiername( 0 );
	IStreamPtr ptr_stream_tmp = CreateMemStream();
	if( ptr_stream_tmp == 0 )			return false;

	INamedDataObject2Ptr ptr_table_nd( pi_table );
	if( ptr_table_nd == 0 )				return false;

	//generate new group guid
	stat_group_ex	sg;
	if( !create_stat_group( pi_table, pi_ol, &sg ) )
		return false;

	{
		//PROFILE_TEST( "CCreateStatTable::process_table->test_exist" );
	//at first test for verify type
	if( !test_exist( pi_table, pi_ol, &sg ) )
		return false;
	}
	
	//need send images?
	bool bsend_image = ( 1L == GetArgLong( "SendImages" ) );


	//add statistic group
	pi_table->AddGroup( 0, &sg, 0 );

	//add class param
	TPOS lpos_class_param = 0;
	{		
		pi_table->GetParamPosByKey( KEY_CLASS, &lpos_class_param );
		if( !lpos_class_param )
		{
			stat_param sp_class;			
			init_stat_param( &sp_class );

			sp_class.lkey = KEY_CLASS;
			pi_table->AddParam( 0, &sp_class, &lpos_class_param );
			if( !lpos_class_param )
				return false;
		}
	}

	map<long,TPOS> mapKey_ParamPos;
	{
		INamedDataPtr pND=pi_table;
		INamedDataPtr ndApp=::GetAppUnknown();
		// get man & users params from shell
		map_meas_params mapMeasParams; 
		CreateNDParamDescrCache(mapMeasParams, ndApp);

		// find free keys for manual measurement and user measurement params
		long lNextManualParamKey=MANUAL_PARAMS_FIRST_KEY;
		long lNextMeasUserParamKey=MEAS_USER_PARAMS_FIRST_KEY;
		for(map_meas_params::const_iterator lpos=mapMeasParams.begin(); lpos!=mapMeasParams.end(); ++lpos)
		{
			ParameterDescriptor_ex* pShell=lpos->second;
			long lKey=pShell->lKey;
			if( MANUAL_PARAMS_FIRST_KEY<=lKey && lKey <= MANUAL_PARAMS_LAST_KEY){
				lNextManualParamKey=lKey+1;
			}else if(MEAS_USER_PARAMS_FIRST_KEY<=lKey && lKey<=MEAS_USER_PARAMS_LAST_KEY){
				lNextMeasUserParamKey=lKey+1;
			}
		}	

		LONG_PTR lpos_param = 0;
		ptr_cont->GetFirstParameterPos( &lpos_param );
		while( lpos_param )
		{
			ParameterContainer* ppc = 0;
			ptr_cont->GetNextParameter( &lpos_param, &ppc );
			if( ppc && ppc->pDescr )
			{
				ParameterDescriptor* pDescr=ppc->pDescr;
				long lkey_ol = pDescr->lKey;

				long keyShell;
				ParameterDescriptor_ex* pShell=0;
				if(MANUAL_PARAMS_FIRST_KEY <=lkey_ol && lkey_ol<=MANUAL_PARAMS_LAST_KEY){
					CStatTableObject::ManMeasParDscr MyPD(lkey_ol,ppc);
					pShell=MyPD.match(mapMeasParams);
					if(pShell){
						keyShell=pShell->lKey;
					}else{
						keyShell=lNextManualParamKey++;
						MyPD.lKey=keyShell;
						MyPD.Store(ndApp);
					}
				}else if(MEAS_USER_PARAMS_FIRST_KEY<=lkey_ol && lkey_ol<MEAS_USER_PARAMS_LAST_KEY){
					CStatTableObject::UserMeasParDscr MyPD(lkey_ol,ppc);
					pShell=MyPD.match(mapMeasParams);
					if(pShell){
						keyShell=pShell->lKey;
					}else{
						keyShell=lNextMeasUserParamKey++;
						MyPD.lKey=keyShell;
						MyPD.Store(ndApp);
					}
				}else{
					keyShell=lkey_ol;
				}

				TPOS lpos_tbl_param;
				HRESULT hr=pi_table->GetParamPosByKey( keyShell, &lpos_tbl_param );
				if(hr){
					// Add column to table
					stat_param sp;
					init_stat_param( &sp );
					sp.lkey = keyShell;
					pi_table->AddParam( 0, &sp, &lpos_tbl_param );

					// Add param description to named data
					if(MANUAL_PARAMS_FIRST_KEY <=keyShell && keyShell<=MANUAL_PARAMS_LAST_KEY){
						CStatTableObject::ManMeasParDscr MyPD(keyShell,ppc);
						MyPD.Store(pND);
					}else if(MEAS_USER_PARAMS_FIRST_KEY<=keyShell && keyShell<MEAS_USER_PARAMS_LAST_KEY){
						CStatTableObject::UserMeasParDscr MyPD(keyShell,ppc);
						MyPD.Store(pND);
					}
				}
				mapKey_ParamPos[lkey_ol]=lpos_tbl_param;
			}else{
				_ASSERT(!"Unknown Parameter");
				return false;
			}
		}		
		::SetValue(ndApp,secMeasurement,MANUAL_NEXT_KEY, lNextManualParamKey);
		::SetValue(ndApp,secMeasurement,MEASUSER_NEXT_KEY, lNextMeasUserParamKey);
	}

	//char sz_trace[200];
	//long lcount_childs = 0;
	//pi_ol->GetChildsCount( &lcount_childs );
	//sprintf( sz_trace, "CStatTableObject::process_table->object:%d\n", (int)lcount_childs );
	//OutputDebugString( sz_trace );

	//fill table by object list

	//Composite support
	long bC = FALSE;
	ICompositeObjectPtr co(pi_ol);
	if(co!=0)	co->IsComposite(&bC);
	
	POSITION lpos_child = 0;
	pi_ol->GetFirstChildPosition( &lpos_child );
	while( lpos_child )
	{
		//PROFILE_TEST( "CCreateStatTable::process_table->enum" );

		IUnknown* punk_child = 0;
		pi_ol->GetNextChild( &lpos_child, &punk_child );
		if( !punk_child )	continue;
		
		//Composite support
		if(bC)
		{
			ICompositeSupportPtr cs(punk_child);
			if(cs)
			{
				GUID key;
				cs->GetParentKey(&key);
				if(key !=GUID_NULL) 
					continue;
			}
		}

		//----------------cs

		ICalcObjectPtr   ptr_calc( punk_child );
		punk_child->Release();	punk_child = 0;

		if( ptr_calc == 0 )	continue;

		//add row to table
		TPOS lpos_tbl_row = 0;
		stat_row sr;
		init_stat_row( &sr );

		//set group guid
		sr.guid_group	= sg.guid_group;
		//object guig
		sr.guid_object	= ::GetKey( ptr_calc );

		//set image if need
		if( bsend_image )
		{
			IMeasureObjectPtr ptr_measure( ptr_calc );
			if( ptr_measure )
			{
				IUnknown* punk_image = 0;
				ptr_measure->GetImage( &punk_image );
				if( punk_image )
				{
					sr.punk_image = ::CreateTypedObject( _bstr_t(szTypeImage) );
					ICompatibleObjectPtr ptr_co( sr.punk_image );
					if( ptr_co )
						ptr_co->CreateCompatibleObject( punk_image, NULL, 0 );
					punk_image->Release();	punk_image = 0;
				}
			}
		}

		pi_table->AddRow( 0, &sr, &lpos_tbl_row );

		release_stat_row( &sr );
		if( !lpos_tbl_row )
			continue;

		//save classes		
		{
			//PROFILE_TEST( "CCreateStatTable::process_table->save classes" );

			IUnknown* punk_row = 0;
			POSITION lpos_temp = lpos_tbl_row;
			ptr_table_nd->GetNextChild(&lpos_temp, &punk_row );
			if( punk_row )
			{
				{
					//PROFILE_TEST( "CCreateStatTable::process_table->copy_object_classes" );
					copy_object_classes( ptr_calc, punk_row, ptr_stream_tmp );
				}

				punk_row->Release();	punk_row = 0;
				//at first add key		
				{
					stat_value sv;
					init_stat_value( &sv );
					sv.bwas_defined = true;
					sv.fvalue = ::get_object_class(ptr_calc);
					{

						//PROFILE_TEST( "CCreateStatTable::process_table->set_value" );
						pi_table->SetValue( lpos_tbl_row, lpos_class_param, &sv );
					}
				}
			}
			else
			{
				_ASSERTE( false );
			}
		}

		//add values
		LONG_PTR lpos_param = 0;
		ptr_cont->GetFirstParameterPos( &lpos_param );
		while( lpos_param )
		{
			ParameterContainer* ppc = 0;
			ptr_cont->GetNextParameter( &lpos_param, &ppc );
			if( ppc && ppc->pDescr )
			{
				ParameterDescriptor* pDescr=ppc->pDescr;
				long lkey = pDescr->lKey;

				TPOS lpos_tbl_param;
				try{
					lpos_tbl_param=mapKey_ParamPos[lkey];
				}catch(exception&){
					_ASSERTE(!"Invalid parameter key");
					continue;
				}

				//add value
				stat_value sv;
				init_stat_value( &sv );
				sv.bwas_defined = false;

				if( S_OK == ptr_calc->GetValue( lkey, &sv.fvalue ) )
					sv.bwas_defined = true;

				pi_table->SetValue( lpos_tbl_row, lpos_tbl_param, &sv );
			}
		}		
	}

	pStatTable->UpdateClassNames();
	return true;
}

//////////////////////////////////////////////////////////////////////
HRESULT CCreateStatTable::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;

	IUnknown* punk_ol = get_object_list();
	*pdwState = punk_ol ? afEnabled : 0;

	if( punk_ol )
		punk_ol->Release();	punk_ol = 0;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CCreateStatTable::get_object_list()
{
	IUnknown* punk_ol = 0;
	_bstr_t bstr_name = GetArgString( "ObjectList" );
	if( bstr_name.length() )
		punk_ol = ::GetObjectByName( m_ptrTarget, bstr_name, _bstr_t(szTypeObjectList) );
	else
		punk_ol = GetActiveObject( m_ptrTarget, szTypeObjectList );		

	return punk_ol;
}


//////////////////////////////////////////////////////////////////////
//
//	class CShowStatTable
//
//////////////////////////////////////////////////////////////////////
_bstr_t CShowStatTable::GetViewProgID()
{
	return _bstr_t( szStatTableViewProgID );
}

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableEditBase
//
//////////////////////////////////////////////////////////////////////
HRESULT CStatTableEditBase::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;

	IUnknown* punk_st = get_stat_table();
	*pdwState = punk_st ? afEnabled : 0;

	if( punk_st )
		punk_st->Release();	punk_st = 0;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
IUnknown* CStatTableEditBase::get_stat_table()
{
	IUnknown* punk_st = 0;
	_bstr_t bstr_name = GetArgString( GetTableNameTarget() );
	if( bstr_name.length() )
		punk_st = ::GetObjectByName( m_ptrTarget, bstr_name, _bstr_t(szTypeStatTable) );
	else
		punk_st = GetActiveObject( m_ptrTarget, szTypeStatTable );		

	return punk_st;
}

//////////////////////////////////////////////////////////////////////
bstr_t CStatTableEditBase::GetTableNameTarget()
{
	return "StatTable";
}


//////////////////////////////////////////////////////////////////////
//
//	class CExportStatTable
//
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CExportStatTableInfo::s_pargs[] = 
{	
	{"StatTable",		szArgumentTypeString, "", true, false },
	{"FileName",		szArgumentTypeString, "", true, false },
	{"ExportType",		szArgumentTypeInt, "0", true, false },
	{"AddParamNames",	szArgumentTypeInt, "1", true, false },
	{"DecimalSeparator",	szArgumentTypeString, "", true, false },
	{0, 0, 0, false, false },
};

#define EXP_TYPE_TEXT				0
#define EXP_TYPE_CLIPBOARD			1
//#define EXP_TYPE_STATISTICA			2

//////////////////////////////////////////////////////////////////////
CExportStatTable::CExportStatTable()
{
}

//////////////////////////////////////////////////////////////////////
CExportStatTable::~CExportStatTable()
{
}

//////////////////////////////////////////////////////////////////////
HRESULT CExportStatTable::DoInvoke()
{
	IUnknown* punk_st = get_stat_table();
	if( !punk_st )	return S_FALSE;

	IStatTablePtr ptr_table = punk_st;
	punk_st->Release();	punk_st = 0;

	if( ptr_table == 0 )	return S_FALSE;

	int nexp_type		= GetArgLong( "ExportType" );
	_bstr_t bstr_file	= GetArgString( "FileName" );
	_bstr_t strDecimalSeparator = GetArgString( "DecimalSeparator" );
	
	if( nexp_type == EXP_TYPE_TEXT )
	{
		if( !bstr_file.length()  )
		{
			OPENFILENAME ofn;
			::ZeroMemory( &ofn, sizeof(OPENFILENAME) );

			char sz_file[MAX_PATH];	sz_file[0] = 0;		
			
			char sz_title[255];		sz_title[0] = 0;
			::LoadString( App::handle(), IDS_EXPORT_TITLE, sz_title, sizeof(sz_title) );		

			char sz_filter[255];		sz_filter[0] = 0;
			::LoadString( App::handle(), IDS_EXPORT_FILTER, sz_filter, sizeof(sz_filter) );
			int nlen = strlen(sz_filter);
			for( int i=0;i<nlen;i++ )
				if( sz_filter[i] == '\n' )
					sz_filter[i] = 0;		

			ofn.lStructSize = CDSIZEOF_STRUCT(OPENFILENAMEA,lpTemplateName);
			ofn.Flags = OFN_OVERWRITEPROMPT|OFN_EXPLORER|OFN_LONGNAMES|OFN_ENABLESIZING;	
			ofn.hwndOwner   = ::GetActiveWindow();
			ofn.lpstrFilter = sz_filter;
			ofn.lpstrFile   = sz_file;
			ofn.nMaxFile    = MAX_PATH;
			ofn.lpstrTitle  = sz_title;
			ofn.lpstrDefExt = _T("txt");

			if( !::GetSaveFileNameA( &ofn ) )
				return S_FALSE;

			bstr_file = ofn.lpstrFile;
		}

		stream_file file;
		HRESULT hr = file.open( bstr_file, GENERIC_WRITE, FILE_SHARE_READ, CREATE_ALWAYS );		
		if( hr != S_OK )
		{
			DislayWin32Error( hr, ::GetActiveWindow(), 0 );
			return S_FALSE;
		}

		if( !export_table_to_stream( ptr_table, &file ) )
		{
			//DislayWin32Error( GetLastError(), ::GetActiveWindow(), 0 );
			return S_FALSE;
		}

		return S_OK;		
	}
	else if( nexp_type == EXP_TYPE_CLIPBOARD )
	{
		if( !OpenClipboard( ::GetActiveWindow() ) )
		{
			DislayWin32Error( GetLastError(), ::GetActiveWindow(), 0 );
			return S_FALSE;
		}

		if( !EmptyClipboard() )
		{
			DislayWin32Error( GetLastError(), ::GetActiveWindow(), 0 );
			return S_FALSE;
		}

		IStreamPtr	pi_stream;
		::CreateStreamOnHGlobal( 0, FALSE, &pi_stream );
		if( !(bool)pi_stream )
		{			
			DislayWin32Error( GetLastError(), ::GetActiveWindow(), 0 );
			CloseClipboard();
			return S_FALSE;
		}

		if( !export_table_to_stream( ptr_table, pi_stream ) )
		{
			//DislayWin32Error( GetLastError(), ::GetActiveWindow(), 0 );
			return S_FALSE;
		}		

		//write "zero" char
		{
			_char sz_zero = 0;
			DWORD dw_size = sizeof( sz_zero );
			pi_stream->Write( &sz_zero, dw_size, &dw_size );
		}

		HGLOBAL h = 0;
		GetHGlobalFromStream( pi_stream, &h );
		if( ::SetClipboardData( CF_UNICODETEXT, h ) == 0 )
		{
			DislayWin32Error( GetLastError(), ::GetActiveWindow(), 0 );
			CloseClipboard();
			return S_FALSE;
		}
		
		CloseClipboard();

		return S_OK;
	}
	else
		return S_FALSE;
}

//////////////////////////////////////////////////////////////////////
bool CExportStatTable::export_table_to_stream( IStatTable* pi_table, IStream* pi_stream )
{
	if( !pi_table || !pi_stream )
		return false;

	_string str_class_name = short_classifiername( 0 );

	//cache meas params
	map_meas_params		map_meas_params;
	CreateAllParamDescrCache( map_meas_params, pi_table );
	
	//gather table param keys
	_list_map_t<long/*key*/, long/*lidx*/, cmp_long>	table_keys;
	
	//allock memory for table params
	long lparam_count = 0;
	pi_table->GetParamsCount( &lparam_count );
	if( !lparam_count )	return false;

	std::valarray<long> pparams(lparam_count);
	
	//get params from table
	lparam_count = 0;
	TPOS lParamPos = 0;
	pi_table->GetFirstParamPos( &lParamPos );
	while( lParamPos )
	{
		stat_param* pparam = 0;
		pi_table->GetNextParam( &lParamPos, &pparam );

		long lkey = pparam->lkey;
		if( !IsTableKeyVisible( pi_table, lkey ) )
			continue;
		
		pparams[lparam_count++] = lkey;
	}

	if( !lparam_count )	return false;
	
///////////////
	//CStatView* pStatView;
	//IApplicationPtr sptrApp(GetAppUnknown());
	//IUnknown* pDoc = 0;
	//sptrApp->GetActiveDocument(&pDoc);
	//IUnknown* punkBaseImage = 0;
	//if(pDoc)
	//{
	//	IDocumentSitePtr sptrDoc(pDoc);
	//	IUnknown* punkView  = 0;
	//	sptrDoc->GetActiveView(&punkView);
	//	IStatTableViewPtr pStatTableView=punkView;
	//	pStatView=(CStatView*)(IStatTableView*)pStatTableView;
	//}
	CStatTableObject* pStatObject=(CStatTableObject*)(IStatTable*)pi_table;
	CMapColInfo& map_col = pStatObject->m_map_col;
	_ASSERTE(map_col.size()==lparam_count);
////////////////
	//sort table key by order
	long idx=0;
	for(CMapColInfo::const_iterator it=map_col.begin(); 
		it!=map_col.end(); ++it,++idx)
	{
		stat_col_info* pStatColInfo= it->second;
		pparams[idx]=pStatColInfo->m_lkey;
	}

	wchar_t sz_buf[1024];	sz_buf[0] = 0;
	DWORD dw_size = 0;
	HRESULT hr = S_OK;

	//write header
	int nadd_params = GetArgLong( "AddParamNames" );
	if( nadd_params == 1 )
	{
		for( int i=0;i<lparam_count;i++ )
		{
 			long lkey = pparams[i];
			if( lkey == KEY_CLASS )
			{
				char sz_cbuf[200];	sz_cbuf[0] = 0;
				::LoadString( app::handle(), IDS_CLASS_CAPTION, sz_cbuf, sizeof(sz_cbuf) );
				::MultiByteToWideChar( CP_ACP, 0, sz_cbuf, -1, sz_buf, sizeof(sz_buf));
			}
			else
			{
				if(ParameterDescriptor_ex* pmp=map_meas_params(lkey))
				{
					if(pmp->bstrUserName && *pmp->bstrUserName)
						wcscpy( sz_buf, pmp->bstrUserName );
					else if(pmp->bstrName && *pmp->bstrName)
						wcscpy( sz_buf, pmp->bstrName );
					if( pmp->bstrUnit && *pmp->bstrUnit){
						wcscat( sz_buf, L", ");
						wcscat( sz_buf, pmp->bstrUnit);
					}
				}				
			}
			

			dw_size = wcslen( sz_buf );
			
			hr = pi_stream->Write( sz_buf, dw_size*2, &dw_size );
			if( hr != S_OK )
			{
				DislayWin32Error( hr, ::GetActiveWindow(), 0 );
				return false;
			}

			if( i != lparam_count - 1 )
			{
				wcscpy( sz_buf, L"\t" );
				dw_size = wcslen( sz_buf );
				hr = pi_stream->Write( sz_buf, dw_size*2, &dw_size );
				if( hr != S_OK )
				{
					DislayWin32Error( hr, ::GetActiveWindow(), 0 );
					return false;
				}
			}
		}

		//write new line
		wcscpy( sz_buf, L"\r\n" );
		dw_size = wcslen( sz_buf );
		hr = pi_stream->Write( sz_buf, dw_size*2, &dw_size );
		if( hr != S_OK )
		{
			DislayWin32Error( hr, ::GetActiveWindow(), 0 );
			return false;
		}
	}

	wchar_t DecimalSeparator=((wchar_t*)_bstr_t(_variant_t(1.1)))[1];
	{
		_bstr_t strDecimalSeparator = GetArgString( "DecimalSeparator" );
		if(strDecimalSeparator.length())
			DecimalSeparator=((wchar_t*)strDecimalSeparator)[0];
	}

	TPOS lrow_pos = 0;
	pi_table->GetFirstRowPos( &lrow_pos );
	while( lrow_pos )
	{
		stat_row* prow = 0;
		TPOS lsave_row_pos = lrow_pos;
		pi_table->GetNextRow( &lrow_pos, &prow );

		//write values
		for( int i=0;i<lparam_count;i++ )
		{
			long lkey = pparams[i];

			stat_value* pvalue = 0;
			pi_table->GetValueByKey( lsave_row_pos, lkey, &pvalue );
			
			sz_buf[0] = 0;			
			
			if( pvalue )
			{
				if( lkey == KEY_CLASS )
				{
					wcscpy( sz_buf, _bstr_t(((CStatTableObject*)(IStatTable*)(pi_table))
						->get_class_name((long)pvalue->fvalue)));
				}
				else
				{
					_bstr_t bstr_format = "%f";
					double funit = 1.;
					map_meas_params::const_iterator it=map_meas_params.find( lkey );
					if( it!=map_meas_params.end() )
					{
						ParameterDescriptor_ex* pmp = it->second;
						if( pmp->bstrDefFormat && *pmp->bstrDefFormat)
							bstr_format = pmp->bstrDefFormat;
						funit = pmp->fCoeffToUnits;
					}

					if( pvalue && pvalue->bwas_defined )
					{
						sprintf( (char*)sz_buf, "%g", pvalue->fvalue * funit );
						for(wchar_t* p=sz_buf; *p; ++p){
							if(L'.'==*p || L','==*p)
								*p=DecimalSeparator;
						}
						wcscpy( sz_buf, _bstr_t((char*)sz_buf));
					}
					else
						wcscpy( sz_buf, L"-" );						
				}
			}
			
			dw_size = wcslen( sz_buf );
				
			hr = pi_stream->Write( sz_buf, dw_size*2, &dw_size );
			if( hr != S_OK )
			{
				DislayWin32Error( hr, ::GetActiveWindow(), 0 );
				return false;
			}

			if( i != lparam_count - 1 )
			{
				wcscpy( sz_buf, L"\t" );
				dw_size = wcslen( sz_buf );
				hr = pi_stream->Write( sz_buf, dw_size*2, &dw_size );
				if( hr != S_OK )
				{
					DislayWin32Error( hr, ::GetActiveWindow(), 0 );
					return false;
				}
			}
		}

		//write new line
		if( lrow_pos )
		{			
			wcscpy( sz_buf, L"\r\n" );
			dw_size = wcslen( sz_buf );
			hr = pi_stream->Write( sz_buf, dw_size*2, &dw_size );
			if( hr != S_OK )
			{
				DislayWin32Error( hr, ::GetActiveWindow(), 0 );
				return false;
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////
//
//	class CDeleteObjectFromStatTable
//
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CDeleteObjectFromStatTableInfo::s_pargs[] = 
{	
	{"StatTable",		szArgumentTypeString, "", true, false },		
	{"ObjectListName",	szArgumentTypeString, "", true, false },
	{"ImageName",		szArgumentTypeString, "", true, false },	
	{0, 0, 0, false, false },
};

//////////////////////////////////////////////////////////////////////
CDeleteObjectFromStatTable::CDeleteObjectFromStatTable()
{
}

//////////////////////////////////////////////////////////////////////
CDeleteObjectFromStatTable::~CDeleteObjectFromStatTable()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CDeleteObjectFromStatTable::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT CDeleteObjectFromStatTable::DoInvoke()
{
	//get active table
	IUnknown* punk_table = get_stat_table();
	if( !punk_table )		return S_FALSE;
	    
	IStatTablePtr ptr_table = punk_table;
	punk_table->Release();	punk_table = 0;
	if( ptr_table == 0 )	return S_FALSE;

	_bstr_t bstr_ol		= GetArgString( "ObjectListName" );
	_bstr_t bstr_image	= GetArgString( "ImageName" );

	//show dialog if need
	if( !bstr_ol.length() && !bstr_image.length() )
	{			
		CDeleteObjectDlg dlg;
		dlg.m_ptr_table = ptr_table;

		if( dlg.do_modal( ::GetActiveWindow() ) != IDOK )	return S_FALSE;

		if( !bstr_ol.length() )
			bstr_ol = dlg.m_str_object_list;
		
		if( !bstr_image.length() )
			bstr_image = dlg.m_str_image;
	}

	//create new table
	_bstr_t bstr_table_name = ::GetName( ptr_table );	
	IStatTablePtr ptr_new_table = CreateCompatibleTable( ptr_table );
	if( ptr_new_table == 0 )	return S_FALSE;

	//gather group positions
	_list_t<TPOS> list_group;
	
	TPOS lpos = 0;
	ptr_new_table->GetFirstGroupPos( &lpos );
	while( lpos )
	{
		TPOS lpos_save = lpos;
		stat_group* pgroup = 0;
		ptr_new_table->GetNextGroup( &lpos, &pgroup );
		if( bstr_ol.length() )
		{
			if( pgroup->bstr_object_list_name )
			{
				_bstr_t bstr = pgroup->bstr_object_list_name;
				if( !strcmp( (char*)bstr, (char*)bstr_ol ) )
					list_group.add_tail( lpos_save );
			}
		}
		else if( bstr_image.length() )
		{
			if( pgroup->bstr_image_name )
			{
				_bstr_t bstr = pgroup->bstr_image_name;
				if( !strcmp( (char*)bstr, (char*)bstr_image ) )
					list_group.add_tail( lpos_save );
			}
		}
	}

	if( !list_group.count() )	return S_FALSE;
	

	//remove groups
	for (TPOS lpos = list_group.head(); lpos; lpos = list_group.next(lpos))
	{
		TPOS lgroup_pos = list_group.get( lpos );
        //remove group
		ptr_new_table->DeleteGroup( lgroup_pos, true );
	}

	//set table name
	INamedObject2Ptr ptr_no2( ptr_new_table );
	if( ptr_no2 )
		ptr_no2->SetName( bstr_table_name );

	//remove old table from document
	RemoveFromDocData( m_ptrTarget, ptr_table );

	//add new table to document
	SetToDocData( m_ptrTarget, ptr_new_table );
	
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	class CFilterStatTable
//
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CFilterStatTableInfo::s_pargs[] = 
{	
	{"StatTable",			szArgumentTypeString,	"",		true,	false },
	{"StatTableNew",		szArgumentTypeString,	"",		true,	false },
	{"ActivateTable",		szArgumentTypeInt,		"1",	true,	false },
	{"UseParamsFilter",		szArgumentTypeInt,		"1",	true,	false },
	{"Classes",				szArgumentTypeString,	"",		true,	false },
	{"ClassesFile",			szArgumentTypeString,	"",		true,	false },	
	{0, 0, 0, false, false },
};

//////////////////////////////////////////////////////////////////////
CFilterStatTable::CFilterStatTable()
{
}

//////////////////////////////////////////////////////////////////////
CFilterStatTable::~CFilterStatTable()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CFilterStatTable::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT CFilterStatTable::DoInvoke()
{
	//TIME_TEST("CFilterStatTable::DoInvoke");
	//PROFILE_TEST("CFilterStatTable::DoInvoke");
	//get active table
	IUnknown* punk_table = get_stat_table();
	if( !punk_table )		return S_FALSE;
	    
	IStatTablePtr ptr_table = punk_table;
	punk_table->Release();	punk_table = 0;
	if( ptr_table == 0 )	return S_FALSE;

	//create new table	
	IStatTablePtr ptr_new_table = 0;
	{
		IUnknown* punk_new_table = 0;		
		punk_new_table = CreateTypedObject( _bstr_t(szTypeStatTable) );
		if( !punk_new_table )	return S_FALSE;

		ptr_new_table = punk_new_table;
		punk_new_table->Release();
		//ptr_new_table = CreateCompatibleTable( ptr_table );
	}
	if( ptr_new_table == 0 )	return S_FALSE;

	_bstr_t bstr_new_table_name	= GetArgString( "StatTableNew" );
	if( bstr_new_table_name.length() )
	{
		//set table name
		INamedObject2Ptr ptr_no2( ptr_new_table );
		if( ptr_no2 )
			ptr_no2->SetName( bstr_new_table_name );
	}

	//filter table
	filter_table( ptr_table, ptr_new_table );

	//remove from data if need
	if( bstr_new_table_name == ::GetName( ptr_table ) )
		RemoveFromDocData( m_ptrTarget, ptr_table );	

	//add new table to document
	SetToDocData( m_ptrTarget, ptr_new_table );	

	if( GetArgLong( "ActivateTable" ) == 1 )
	{
		ExecuteAction( "ShowStatTable", 0, 0 );
		SetActiveObject( m_ptrTarget, ptr_new_table, szTypeStatTable );
	}

	return S_OK;
}

class filter_param
{
public:
	filter_param()
	{
		flo		= 0.;
		fhi		= 100.;
		lkey	= -1;
	}

	double	flo;
	double	fhi;
	long	lkey;
};

void filter_param_free( void* pdata )
{
	delete (filter_param*)pdata;
};

//////////////////////////////////////////////////////////////////////
bool CFilterStatTable::filter_table( IStatTable* pi_table, IStatTable* pi_table_new )
{
	//TIME_TEST("CFilterStatTable::filter_table");
	//PROFILE_TEST("CFilterStatTable::filter_table");

	IStreamPtr ptr_stream_tmp = CreateMemStream();
	if( ptr_stream_tmp == 0 )	return false;

	if( !pi_table )		return false;

	INamedDataObject2Ptr ptr_ndo( pi_table );
	if( ptr_ndo == 0 )	return false;

	INamedDataObject2Ptr ptr_ndo_new( pi_table_new );
	if( ptr_ndo_new == 0 )	return false;

	int nuse_params				= GetArgLong( "UseParamsFilter" );

	_bstr_t bstr_classes		= GetArgString( "Classes" );
	_bstr_t bstr_classes_file	= GetArgString( "ClassesFile" );
	if( !bstr_classes_file.length() )
		bstr_classes_file = short_classifiername( 0 );

	_list_t<filter_param*, filter_param_free> list_params;
	_list_t<long>	list_classes;

	if( nuse_params == 1 )
	{
		INamedDataPtr ptr_nd( ::GetAppUnknown() );
		ptr_nd->SetupSection( _bstr_t( STAT_TABLE_FILTER ) );
		long lcount = 0;
		ptr_nd->GetEntriesCount( &lcount );
		for( long i=0;i<lcount;i++ )
		{
  		    ptr_nd->SetupSection( _bstr_t( STAT_TABLE_FILTER ) );

			BSTR bstr = 0;
			ptr_nd->GetEntryName( i, &bstr );
			if( !bstr )	continue;

			_bstr_t bstr_section = bstr;
			::SysFreeString( bstr );	bstr = 0;

			long lkey = atoi( (char*)bstr_section );

			_bstr_t bstr_new_param_section = STAT_TABLE_FILTER;
			bstr_new_param_section += "\\";
			bstr_new_param_section += bstr_section;
			ptr_nd->SetupSection( bstr_new_param_section );

			_variant_t var_hi;
			ptr_nd->GetValue( _bstr_t("hi"), &var_hi );
			_variant_t var_lo;
			ptr_nd->GetValue( _bstr_t("lo"), &var_lo );

			_variant_t var_hi_c;
			_variant_t var_lo_c;

			if( S_OK == ::VariantChangeType( &var_hi_c, &var_hi, 0, VT_R8 ) && 
				S_OK == ::VariantChangeType( &var_lo_c, &var_lo, 0, VT_R8 ) )
			{
				filter_param* pparam = new filter_param;
				list_params.add_tail( pparam );
				pparam->fhi		= var_hi_c.dblVal;
				pparam->flo		= var_lo_c.dblVal;
				pparam->lkey	= lkey;
			}
		}
	}

	char sz_buf[1024];
	if( bstr_classes.length() >= sizeof(sz_buf) )
		return false;
	
	strcpy( sz_buf, (char*)bstr_classes );

	char* psz_class = strtok( sz_buf, ";" );
	while( psz_class )
	{
		long lclass = atoi( psz_class );
		psz_class = strtok( 0, ";" );
		
		list_classes.add_tail( lclass );
	}

	{
		TPOS lpos = 0;
		//copy params
		pi_table->GetFirstParamPos(&lpos);
		while (lpos)
		{
			stat_param* pparam = 0;
			pi_table->GetNextParam(&lpos, &pparam);
			pi_table_new->AddParam(0, pparam, 0);
		}
	}
	//copy groups
	TPOS lpos = 0;
	{
		pi_table->GetFirstGroupPos( &lpos );
		while( lpos )
		{
			stat_group* ppgroup = 0;
			pi_table->GetNextGroup( &lpos, &ppgroup );
			pi_table_new->AddGroup( 0, ppgroup, 0 );
		}
	}

	pi_table->GetFirstRowPos(&lpos);
	while( lpos )
	{
		bool bneed_remove = false;

		TPOS lpos_save = lpos;
		
		stat_row* prow = 0;
		pi_table->GetNextRow( &lpos, &prow );
		
		//lookup in params
		if( list_params.count() )
		{
			for (TPOS lpos_param = list_params.head(); lpos_param; lpos_param = list_params.next(lpos_param))
			{
				filter_param* pparam = list_params.get( lpos_param );
				stat_value* pvalue = 0;
				if( S_OK == pi_table->GetValueByKey( lpos_save, pparam->lkey, &pvalue ) )
				{
					if( pvalue->bwas_defined )
					{
						if( pvalue->fvalue < pparam->flo || pvalue->fvalue > pparam->fhi )
						{
							bneed_remove = true;
							break;
						}
					}
				}				
			}
			
		}

		//lookup in classes
		if( list_classes.count() && !bneed_remove )
		{			
			TPOS lpos_calc = lpos_save;
			IUnknown* punk_child = 0;
			ptr_ndo->GetNextChild( (POSITION*)&lpos_calc, &punk_child );
			if( punk_child )
			{
				ICalcObjectPtr ptr_calc = punk_child;
				punk_child->Release();	punk_child = 0;

				if( ptr_calc )
				{
					bool bfound_class = false;
					long lclass = get_object_class( ptr_calc, bstr_classes_file.length() ? (char*)bstr_classes_file : 0 );
					for (TPOS lpos_class = list_classes.head(); lpos_class; lpos_class = list_classes.next(lpos_class))
					{
						if( lclass == list_classes.get( lpos_class ) )
						{
							bfound_class = true;
							break;
						}
					}
					bneed_remove = !bfound_class;
				}
			}
		}

		if( !bneed_remove )
		{
			TPOS lpos_new_row = 0;
			pi_table_new->AddRow( 0, prow, &lpos_new_row );

			//copy classes
			{
				TPOS lpos_temp = lpos_save;
				IUnknown* punk_src = 0;
				ptr_ndo->GetNextChild( &lpos_temp, &punk_src );
				
				lpos_temp = lpos_new_row;
				IUnknown* punk_target = 0;
				ptr_ndo_new->GetNextChild( &lpos_temp, &punk_target );

				copy_object_classes( punk_src, punk_target, ptr_stream_tmp );

				if( punk_src )		punk_src->Release();	punk_src = 0;
				if( punk_target )	punk_target->Release();	punk_target = 0;
			}

			//copy values
			TPOS lpos_param = 0;
			pi_table->GetFirstParamPos( &lpos_param );
			while( lpos_param )
			{
				stat_param* pparam = 0;
				pi_table->GetNextParam( &lpos_param, &pparam );
				stat_value* pvalue = 0;
				if( S_OK == pi_table->GetValueByKey( lpos_save, pparam->lkey, &pvalue ) )
					pi_table_new->SetValueByKey( lpos_new_row, pparam->lkey, pvalue );
			}
		}			
	}
	
	// [vanek] BT2000: 3700 - copy base parameters section
	INamedDataPtr	sptr_nd_tbl_src = ptr_ndo,
					sptr_nd_tbl_target = ptr_ndo_new;

	if( sptr_nd_tbl_src != 0 )
	{
     	if( sptr_nd_tbl_target == 0 )
		{
			ptr_ndo_new->InitAttachedData();
			sptr_nd_tbl_target = ptr_ndo_new;
		}
            
        RecursiveWriteEntry( sptr_nd_tbl_src, sptr_nd_tbl_target , "\\" BASE_PARAMS_SECTION, true );
	}

	return true;
}

/*
//////////////////////////////////////////////////////////////////////
bool CFilterStatTable::filter_table( IStatTable* pi_table )
{
	TIME_TEST("CFilterStatTable::filter_table");
	PROFILE_TEST("CFilterStatTable::filter_table");

	if( !pi_table )		return false;

	INamedDataObject2Ptr ptr_ndo( pi_table );
	if( ptr_ndo == 0 )	return false;

	int nuse_params				= GetArgLong( "UseParamsFilter" );

	_bstr_t bstr_classes		= GetArgString( "Classes" );
	_bstr_t bstr_classes_file	= GetArgString( "ClassesFile" );
	if( !bstr_classes_file.length() )
		bstr_classes_file = short_classifiername( 0 );

	_list_t<filter_param*, filter_param_free> list_params;
	_list_t<long>	list_classes;

	if( nuse_params == 1 )
	{
		INamedDataPtr ptr_nd( ::GetAppUnknown() );
		ptr_nd->SetupSection( _bstr_t( STAT_TABLE_FILTER ) );
		long lcount = 0;
		ptr_nd->GetEntriesCount( &lcount );
		for( long i=0;i<lcount;i++ )
		{
			BSTR bstr = 0;
			ptr_nd->GetEntryName( i, &bstr );
			if( !bstr )	continue;

			_bstr_t bstr_section = bstr;
			::SysFreeString( bstr );	bstr = 0;

			long lkey = atoi( (char*)bstr_section );

			_bstr_t bstr_new_param_section = STAT_TABLE_FILTER;
			bstr_new_param_section += "\\";
			bstr_new_param_section += bstr_section;
			ptr_nd->SetupSection( bstr_new_param_section );

			_variant_t var_hi;
			ptr_nd->GetValue( _bstr_t("hi"), &var_hi );
			_variant_t var_lo;
			ptr_nd->GetValue( _bstr_t("lo"), &var_lo );

			_variant_t var_hi_c;
			_variant_t var_lo_c;

			if( S_OK == ::VariantChangeType( &var_hi_c, &var_hi, 0, VT_R8 ) && 
				S_OK == ::VariantChangeType( &var_lo_c, &var_lo, 0, VT_R8 ) )
			{
				filter_param* pparam = new filter_param;
				list_params.add_tail( pparam );
				pparam->fhi		= var_hi_c.dblVal;
				pparam->flo		= var_lo_c.dblVal;
				pparam->lkey	= lkey;
			}
		}
	}

	char sz_buf[1024];
	if( bstr_classes.length() >= sizeof(sz_buf) )
		return false;
	
	strcpy( sz_buf, (char*)bstr_classes );

	char* psz_class = strtok( sz_buf, ";" );
	while( psz_class )
	{
		long lclass = atoi( psz_class );
		psz_class = strtok( 0, ";" );
		
		list_classes.add_tail( lclass );
	}

	long lpos = 0;
	pi_table->GetFirstRowPos( &lpos );
	while( lpos )
	{
		bool bneed_remove = false;

		long lpos_save = lpos;
		
		stat_row* prow = 0;
		pi_table->GetNextRow( &lpos, &prow );
		
		//lookup in params
		if( list_params.count() )
		{
			for( long lpos_param=list_params.head(); lpos_param; lpos_param=list_params.next(lpos_param) )
			{
				filter_param* pparam = list_params.get( lpos_param );
				stat_value* pvalue = 0;
				if( S_OK == pi_table->GetValueByKey( lpos_save, pparam->lkey, &pvalue ) )
				{
					if( pvalue->bwas_defined )
					{
						if( pvalue->fvalue < pparam->flo || pvalue->fvalue > pparam->fhi )
						{
							bneed_remove = true;
							break;
						}
					}
				}				
			}
			
		}

		//lookup in classes
		if( list_classes.count() && !bneed_remove )
		{			
			long lpos_calc = lpos_save;
			IUnknown* punk_child = 0;
			ptr_ndo->GetNextChild( &lpos_calc, &punk_child );
			if( punk_child )
			{
				ICalcObjectPtr ptr_calc = punk_child;
				punk_child->Release();	punk_child = 0;

				if( ptr_calc )
				{
					bool bfound_class = false;
					long lclass = get_object_class( ptr_calc, bstr_classes_file.length() ? (char*)bstr_classes_file : 0 );
					for( long lpos_class=list_classes.head(); lpos_class; lpos_class=list_classes.next(lpos_class) )
					{
						if( lclass == list_classes.get( lpos_class ) )
						{
							bfound_class = true;
							break;
						}
					}
					bneed_remove = !bfound_class;
				}
			}
		}

		if( bneed_remove )
			pi_table->DeleteRow( lpos_save );

	}

	return true;
}
*/
//////////////////////////////////////////////////////////////////////
//
//	class CSewStatTable
//
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CSewStatTableInfo::s_pargs[] = 
{	
	{"StatTable1",			szArgumentTypeString,	"",		true,	false },
	{"StatTable2",			szArgumentTypeString,	"",		true,	false },
	{"StatTableNew",		szArgumentTypeString,	"",		true,	false },
	{"ActivateTable",		szArgumentTypeInt,		"1",	true,	false },
	{"OvewriteType",		szArgumentTypeInt,		"0",	true,	false },
	{0, 0, 0, false, false },
};

#define OVEWRITE_ASK			0
#define OVEWRITE_APPEND_PARAMS	1
#define OVEWRITE_SKIP_PARAMS	2

//////////////////////////////////////////////////////////////////////
CSewStatTable::CSewStatTable()
{
}

//////////////////////////////////////////////////////////////////////
CSewStatTable::~CSewStatTable()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CSewStatTable::DoGetInterface( const IID &iid )
{
	if( IsUndoUnable() )
		if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT CSewStatTable::DoInvoke()
{
	//TIME_TEST( "CSewStatTable::DoInvoke()" );
	//PROFILE_TEST( "CSewStatTable::DoInvoke()" );

	//get active table
	IUnknown* punk_table1 = 0;
	IUnknown* punk_table2 = 0;
	get_stat_tables( &punk_table1, &punk_table2 );	
	    
	IStatTablePtr ptr_table1 = punk_table1;
	IStatTablePtr ptr_table2 = punk_table2;

	if( punk_table1 )
		punk_table1->Release();	punk_table1 = 0;

	if( punk_table2 )
		punk_table2->Release();	punk_table2 = 0;
	
	if( ptr_table1 == 0 || ptr_table2 == 0 )	return S_FALSE;

	_bstr_t bstr_new_table_name	= GetArgString( "StatTableNew" );

	//create new table	
	IStatTablePtr ptr_new_table;

	bool buse_undo = true;
	if( !IsUndoUnable() )
	{
		if( bstr_new_table_name == ::GetName( ptr_table1 ) )	
		{
			ptr_new_table = ptr_table1;
			buse_undo = false;
		}
	}

	if( buse_undo )
		ptr_new_table = CreateCompatibleTable( ptr_table1 );

	if( ptr_new_table == 0 )	return S_FALSE;

	if( bstr_new_table_name.length() )
	{
		//set table name
		INamedObject2Ptr ptr_no2( ptr_new_table );
		if( ptr_no2 )
			ptr_no2->SetName( bstr_new_table_name );
	}

	//filter table
	process( ptr_new_table, ptr_table2 );

	
	if( buse_undo )
	{
		//remove from data if need
		if( bstr_new_table_name == ::GetName( ptr_table1 ) )
			RemoveFromDocData( m_ptrTarget, ptr_table1 );	
		else if( bstr_new_table_name == ::GetName( ptr_table2 ) )
			RemoveFromDocData( m_ptrTarget, ptr_table2 );	

		//add new table to document
		SetToDocData( m_ptrTarget, ptr_new_table );
	}
	else
	{
		NotifyObjectChange( ptr_new_table, szEventChangeObject );
	}

	//activate table
	if( GetArgLong( "ActivateTable" ) == 1 )
	{
		ExecuteAction( "ShowStatTable", 0, 0 );
		SetActiveObject( m_ptrTarget, ptr_new_table, szTypeStatTable );
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
bool CSewStatTable::process( IStatTable* pi_table, IStatTable* pi_table_add )
{
	//TIME_TEST( "CSewStatTable::process()" );
	//PROFILE_TEST( "CSewStatTable::process()" );

	if( !pi_table || !pi_table_add )	return false;

	IStreamPtr ptr_stream_tmp = CreateMemStream();
	if( ptr_stream_tmp == 0 )			return false;

	INamedDataObject2Ptr ptr_table_ndo = pi_table;
	INamedDataObject2Ptr ptr_table_add_ndo = pi_table_add;
	if( ptr_table_ndo == 0 || ptr_table_add_ndo == 0 )	return false;
    
	if( !pi_table || !pi_table_add )	return false;

	CopyObjectNamedData( pi_table_add, pi_table, "\\", true );

	//gather keys from 2 tables
	_list_t<long> list_key;
	_list_t<long> list_key_add;

	TPOS lpos = 0;
	pi_table->GetFirstParamPos( &lpos );
	while( lpos )
	{
		stat_param* pparam = 0;
		pi_table->GetNextParam( &lpos, &pparam );
		list_key.add_tail( pparam->lkey );
	}

	lpos = 0;
	pi_table_add->GetFirstParamPos( &lpos );
	while( lpos )
	{
		stat_param* pparam = 0;
		pi_table_add->GetNextParam( &lpos, &pparam );
		list_key_add.add_tail( pparam->lkey );
	}

	//calc new key for append
	_list_t<long> list_new_key;
	for (TPOS lpos1 = list_key_add.head(); lpos1; lpos1 = list_key_add.next(lpos1))
	{
		long lkey_add=list_key_add.get( lpos1 );
		bool bfound = false;
		for (TPOS lpos2 = list_key.head(); lpos2; lpos2 = list_key.next(lpos2))
		{
			long lkey=list_key.get( lpos2 );
			if( lkey_add == lkey )
			{
				bfound = true;
				break;
			}
		}
		if( !bfound )
			list_new_key.add_tail( lkey_add );
	}

	//ask for append
	if( list_new_key.count() )
	{
		long lovewrite = GetArgLong( "OvewriteType" );

		bool bneed_append_keys = false;
		if( lovewrite == OVEWRITE_ASK )
		{
			char sz_buf[255];	sz_buf[0] = 0;
			::LoadString( App::handle(), IDS_SEW_OVEWRITE, sz_buf, sizeof(sz_buf) );
			bneed_append_keys = ( IDYES == VTMessageBox( sz_buf, 0, MB_YESNO ) );
		}
		else if( lovewrite == OVEWRITE_APPEND_PARAMS )
			bneed_append_keys = true;
		else if( lovewrite == OVEWRITE_SKIP_PARAMS )
			bneed_append_keys = false;

		//append keys
		if( bneed_append_keys )
		{
			for (TPOS lpos = list_new_key.head(); lpos; lpos = list_new_key.next(lpos))
			{
				long lkey = list_new_key.get( lpos );

				stat_param param = {0};
				param.lkey = lkey;
				pi_table->AddParam( 0, &param, 0 );				
			}
		}
	}

	//copy groups from tbl2 to tbl1
	
	long lgroup_count = 0;
	pi_table_add->GetGroupCount( &lgroup_count );
	long lidx = 0;
	TPOS lpos_group_add = 0;
	pi_table_add->GetFirstGroupPos( &lpos_group_add );

	while( lpos_group_add )
	{
		stat_group* pgroup_add = 0;
		pi_table_add->GetNextGroup( &lpos_group_add, &pgroup_add );
		
		stat_group group;
		init_stat_group( &group );
		copy_stat_group( pgroup_add, &group );
		pi_table->AddGroup( 0, &group, 0 );
		release_stat_group( &group );

		lidx++;
		if( lidx == lgroup_count )	break;
	}

	//copy keys from tbl2 to 1
	long lrow_count = 0;
	pi_table_add->GetRowCount( &lrow_count );
	lidx = 0;

	TPOS lpos_row_add = 0;
	pi_table_add->GetFirstRowPos( &lpos_row_add );

	while( lpos_row_add )
	{
		stat_row* prow_add = 0;
		TPOS lpos_row_add_save = lpos_row_add;
		pi_table_add->GetNextRow( &lpos_row_add, &prow_add );

		stat_row row_new = {0};
		row_new.guid_group	= prow_add->guid_group;
		row_new.guid_object	= prow_add->guid_object;

		//add image
		if( prow_add->punk_image )
		{
			row_new.punk_image = ::CreateTypedObject( _bstr_t(szTypeImage) );
			ICompatibleObjectPtr ptr_co( row_new.punk_image );
			if( ptr_co )
				ptr_co->CreateCompatibleObject( prow_add->punk_image, NULL, 0 );
		}

		//add row
		TPOS lpos_row_new = 0;
		pi_table->AddRow( 0, &row_new, &lpos_row_new );
		release_stat_row( &row_new );

		if( !lpos_row_new )
			continue;

		//add classes
		{
			TPOS lpos_add_temp = (TPOS)lpos_row_add_save;
			IUnknown* punk_src = 0;
			ptr_table_add_ndo->GetNextChild( &lpos_add_temp, &punk_src );
			
			TPOS lpos_new_temp = (TPOS)lpos_row_new;
			IUnknown* punk_target = 0;
			ptr_table_ndo->GetNextChild( &lpos_new_temp, &punk_target );

			copy_object_classes( punk_src, punk_target, ptr_stream_tmp );

			if( punk_src )		punk_src->Release();	punk_src = 0;
			if( punk_target )	punk_target->Release();	punk_target = 0;

		}

		//set values
		for (TPOS lpos = list_key_add.head(); lpos; lpos = list_key_add.next(lpos))
		{
			long lkey = list_key_add.get( lpos );
			
			stat_value* pvalue = 0;
			if( S_OK == pi_table->GetValueByKey( lpos_row_add_save, lkey, &pvalue ) )
				pi_table_add->SetValueByKey( lpos_row_new, lkey, pvalue );
		}

		lidx++;
		if( lidx == lrow_count )	break;
	}	

	return true;
}

//////////////////////////////////////////////////////////////////////
HRESULT CSewStatTable::GetActionState( DWORD *pdwState )
{
	IUnknown* punk_table1 = 0;
	IUnknown* punk_table2 = 0;
	get_stat_tables( &punk_table1, &punk_table2 );	
	    
	IStatTablePtr ptr_table1 = punk_table1;
	IStatTablePtr ptr_table2 = punk_table2;

	if( punk_table1 )
		punk_table1->Release();	punk_table1 = 0;

	if( punk_table2 )
		punk_table2->Release();	punk_table2 = 0;

	if( !pdwState )	return E_POINTER;

	*pdwState = ( ptr_table1 != 0 && ptr_table2 != 0 ) ? afEnabled : 0;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
void CSewStatTable::get_stat_tables( IUnknown** ppunk1, IUnknown** ppunk2 )
{
	if( !ppunk1 || !ppunk2 )	return;

	IUnknown* punk_st1 = 0;
	IUnknown* punk_st2 = 0;
	_bstr_t bstr_name1 = GetArgString( "StatTable1" );
	_bstr_t bstr_name2 = GetArgString( "StatTable2" );
	if( bstr_name1.length() && bstr_name2.length() )
	{
		punk_st1 = ::GetObjectByName( m_ptrTarget, bstr_name1, _bstr_t(szTypeStatTable) );
		punk_st2 = ::GetObjectByName( m_ptrTarget, bstr_name2, _bstr_t(szTypeStatTable) );
	}
	else	
	{
		IDataContext3Ptr ptr_dc;
		IDocumentSitePtr ptr_ds( m_ptrTarget );
		if( ptr_ds )
		{
			IUnknown* punk_view = 0;
			ptr_ds->GetActiveView( &punk_view );
			if( punk_view )
			{
				ptr_dc = punk_view;
				punk_view->Release();	punk_view = 0;

				
			}

			if( ptr_dc )
			{
				LONG_PTR lpos = 0;
				ptr_dc->GetFirstSelectedPos( _bstr_t(szTypeStatTable), &lpos );
				if( lpos )
				{
					ptr_dc->GetNextSelected( _bstr_t(szTypeStatTable), &lpos, &punk_st1 );
					if( lpos )
						ptr_dc->GetNextSelected( _bstr_t(szTypeStatTable), &lpos, &punk_st2 );

				}
			}
		}
	}

	*ppunk1 = punk_st1;
	*ppunk2 = punk_st2;
}