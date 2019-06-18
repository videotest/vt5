#include "stdafx.h"
#include "stat_types.h"


//////////////////////////////////////////////////////////////////////
//
//	class stat_value_ex
//
//////////////////////////////////////////////////////////////////////
stat_value_ex::stat_value_ex()
{
	init_stat_value( this );
	m_lkey = -1;
}

//////////////////////////////////////////////////////////////////////
stat_value_ex::~stat_value_ex(){}


//////////////////////////////////////////////////////////////////////
//
//	class stat_row_ex
//
//////////////////////////////////////////////////////////////////////
stat_row_ex::stat_row_ex()
{
	init_stat_row( this );
}

//////////////////////////////////////////////////////////////////////
stat_row_ex::~stat_row_ex()
{
	release_stat_row( this );
}

//////////////////////////////////////////////////////////////////////
IUnknown* stat_row_ex::DoGetInterface( const IID &iid )
{
	if( iid == IID_ICalcObject )return (ICalcObject*)this;
	if( iid == IID_INamedPropBag )return (INamedPropBag*)this;
	if( iid == IID_INamedPropBagSer )return (INamedPropBagSer*)this;
	if( iid == IID_INumeredObject )return (INumeredObject*)this;
	else return ComObjectBase::DoGetInterface( iid );	
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_row_ex::GetValue( long lParamKey, double *pfValue )
{
	long lpos_value = m_map_value.find( lParamKey );
	if( !lpos_value )
		return S_FALSE;

	stat_value_ex* pvalue = m_map_value.get( lpos_value );	
	if( !pvalue->bwas_defined )
		return S_FALSE;

	*pfValue = pvalue->fvalue;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT stat_row_ex::SetValue( long lParamKey, double fValue )
{
	long lpos_value = m_map_value.find( lParamKey );
	if( !lpos_value )
		return E_INVALIDARG;

	stat_value_ex* pvalue = m_map_value.get( lpos_value );	
	pvalue->bwas_defined	= true;
	pvalue->fvalue			= fValue;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
//
//	class stat_param_ex
//
//////////////////////////////////////////////////////////////////////
stat_param_ex::stat_param_ex()
{
	init_stat_param( this )	;
}

//////////////////////////////////////////////////////////////////////
stat_param_ex::~stat_param_ex(){}


//////////////////////////////////////////////////////////////////////
//
//	class stat_group_ex
//
//////////////////////////////////////////////////////////////////////
stat_group_ex::stat_group_ex()
{
	init_stat_group( this )	;
}

//////////////////////////////////////////////////////////////////////
stat_group_ex::~stat_group_ex()
{
	release_stat_group( this );
}


//stat_params helpers
//////////////////////////////////////////////////////////////////////
void init_stat_param( stat_param* pparam )
{
	pparam->lkey = -1;
}

//////////////////////////////////////////////////////////////////////
void copy_stat_param( stat_param* psrc, stat_param* ptarget )
{
	ptarget->lkey = psrc->lkey;
}

//stat_row helpers
//////////////////////////////////////////////////////////////////////
void init_stat_row( stat_row* prow )
{
	prow->punk_image	= 0;
	prow->guid_group	= INVALID_KEY;
	prow->guid_object	= INVALID_KEY;
}

//////////////////////////////////////////////////////////////////////
void copy_stat_row( stat_row* psrc, stat_row* ptarget )
{
	ptarget->punk_image		= psrc->punk_image;
	ptarget->guid_object	= psrc->guid_object;	
	ptarget->guid_group		= psrc->guid_group;	
	if( ptarget->punk_image )
		ptarget->punk_image->AddRef();
}

//////////////////////////////////////////////////////////////////////
void release_stat_row( stat_row* prow )
{
	if( prow->punk_image )
		prow->punk_image->Release();	prow->punk_image = 0;
}


//stat_value helpers
//////////////////////////////////////////////////////////////////////
void init_stat_value( stat_value* pvalue )
{
	pvalue->fvalue			= .0;
	pvalue->bwas_defined	= false;
}

//////////////////////////////////////////////////////////////////////
void copy_stat_value( stat_value* psrc, stat_value* ptarget )
{
	ptarget->fvalue			= psrc->fvalue;
	ptarget->bwas_defined	= psrc->bwas_defined;
}

//stat_group helpres
//////////////////////////////////////////////////////////////////////
void init_stat_group( stat_group* pgroup )
{
	pgroup->size					= sizeof(stat_group);
	pgroup->mask					= 0;
	//sending guid and time
	pgroup->guid_group				= INVALID_KEY;
	::ZeroMemory( &pgroup->time_send, sizeof(SYSTEMTIME) );
	//document
	pgroup->guid_doc				= INVALID_KEY;
	pgroup->bstr_doc_name			= 0;
	//image
	pgroup->guid_image				= INVALID_KEY;
	pgroup->bstr_image_name			= 0;
	//object_list
	pgroup->guid_object_list		= INVALID_KEY;
	pgroup->bstr_object_list_name	= 0;
	//user data
	pgroup->bstr_user_data			= 0;
}

//////////////////////////////////////////////////////////////////////
void copy_stat_group( stat_group* psrc, stat_group* ptarget )
{
	if( !psrc || !ptarget )	return;
	ptarget->size					= psrc->size;
	ptarget->mask					= psrc->mask;
	
	//sending guid and time
	ptarget->guid_group				= psrc->guid_group;
	ptarget->time_send				= psrc->time_send;
	
	//document
	ptarget->guid_doc				= psrc->guid_doc;
	if( psrc->bstr_doc_name )
		ptarget->bstr_doc_name		= _bstr_t( psrc->bstr_doc_name ).copy();
	else
		ptarget->bstr_doc_name		= 0;
	
	//image
	ptarget->guid_image				= psrc->guid_image;
	if( psrc->bstr_image_name )
		ptarget->bstr_image_name	= _bstr_t( psrc->bstr_image_name ).copy();
	else
		ptarget->bstr_image_name	= 0;
	
	//object_list
	ptarget->guid_object_list		= psrc->guid_object_list;
	if( psrc->bstr_object_list_name )
		ptarget->bstr_object_list_name	= _bstr_t( psrc->bstr_object_list_name ).copy();
	else
		ptarget->bstr_object_list_name	= 0;
	
	//user data
	if( psrc->bstr_user_data )
		ptarget->bstr_user_data	= _bstr_t( psrc->bstr_user_data ).copy();
	else
		ptarget->bstr_user_data	= 0;

}

//////////////////////////////////////////////////////////////////////
void release_stat_group( stat_group* pgroup )
{
	if( !pgroup )	return;

	if( pgroup->bstr_doc_name )
		::SysFreeString( pgroup->bstr_doc_name );			pgroup->bstr_doc_name = 0;

	if( pgroup->bstr_image_name )
		::SysFreeString( pgroup->bstr_image_name );			pgroup->bstr_image_name = 0;

	if( pgroup->bstr_object_list_name )
		::SysFreeString( pgroup->bstr_object_list_name );	pgroup->bstr_object_list_name = 0;

	if( pgroup->bstr_user_data )
		::SysFreeString( pgroup->bstr_user_data );			pgroup->bstr_user_data = 0;
}


//////////////////////////////////////////////////////////////////////
//
//	class stat_col_info
//
//////////////////////////////////////////////////////////////////////
stat_col_info::stat_col_info()
{
	m_width		= 100;
	m_bvisible	= true;	
	m_order = std::numeric_limits<long>::max();
}

//////////////////////////////////////////////////////////////////////
stat_col_info::~stat_col_info()
{

}

//////////////////////////////////////////////////////////////////////
bool stat_col_info::load( INamedData* pi_nd )
{
	if( !pi_nd )	return false;

	//width
	_variant_t var_width;
	pi_nd->GetValue( _bstr_t( ST_COL_WIDTH ), &var_width );
	if( var_width.vt == VT_I4 )
		m_width = var_width.lVal;

	//visible
	_variant_t var_visible;
	pi_nd->GetValue( _bstr_t( ST_COL_VISIBLE ), &var_visible );
	if( var_visible.vt == VT_I4 )
		m_bvisible = (var_visible.lVal == 1 );

	/*//order
	_variant_t var_order;
	pi_nd->GetValue( _bstr_t( ST_COL_ORDER ), &var_order );
	if( var_order.vt == VT_I4 )
		m_order = var_width.lVal;
		*/

	return true;
}

//////////////////////////////////////////////////////////////////////
bool stat_col_info::save( INamedData* pi_nd ) const
{
	if( !pi_nd )	return false;

	//width
	pi_nd->SetValue( _bstr_t( ST_COL_WIDTH ), _variant_t( (long)m_width ) );

	//visible	
	pi_nd->SetValue( _bstr_t( ST_COL_VISIBLE ), _variant_t( m_bvisible ? 1L : 0L ) );

	//order
	//pi_nd->SetValue( _bstr_t( ST_COL_ORDER ), _variant_t( (long)m_order ) );
	return true;
}
