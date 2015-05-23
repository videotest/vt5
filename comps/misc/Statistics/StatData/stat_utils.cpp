#include "stdafx.h"
#include "stat_utils.h"

#include "misc_utils.h"

//////////////////////////////////////////////////////////////////////
IUnknown* GetActiveObject( IUnknown* punk_target, const char* psz_type )
{
	if( !punk_target )
		return 0;

	IDataContext3Ptr ptr_dc;
	IDocumentSitePtr ptr_ds( punk_target );
	if( ptr_ds )
	{
		IUnknown* punk_view = 0;
		ptr_ds->GetActiveView( &punk_view );
		if( punk_view )
		{
			ptr_dc = punk_view;
			punk_view->Release();	punk_view = 0;
		}
	}
	else
		ptr_dc = punk_target;

	if( ptr_dc == 0 )
		return 0;

	IUnknown* punk_obj = 0;
	ptr_dc->GetActiveObject( _bstr_t( psz_type ), &punk_obj );

	return punk_obj;
}

//////////////////////////////////////////////////////////////////////
bool SetActiveObject( IUnknown* punk_target, IUnknown* punk_obj, const char* psz_type )
{
	if( !punk_target )
		return false;

	IDataContext3Ptr ptr_dc;
	IDocumentSitePtr ptr_ds( punk_target );
	if( ptr_ds )
	{
		IUnknown* punk_view = 0;
		ptr_ds->GetActiveView( &punk_view );
		if( punk_view )
		{
			ptr_dc = punk_view;
			punk_view->Release();	punk_view = 0;
		}
	}
	else
		ptr_dc = punk_target;

	if( ptr_dc == 0 )
		return false;

	ptr_dc->SetActiveObject( _bstr_t( psz_type ), punk_obj, 1 );

	return true;
}


//////////////////////////////////////////////////////////////////////
bool IsTableKeyVisible( IStatTable* pi_table, long lkey )
{
	if( !pi_table )		return false;
	INamedDataPtr ptr_nd( pi_table );
	if( ptr_nd == 0 )	return true;

	char sz_section[255];		
	sprintf( sz_section, "%s\\%s%d", STAT_TABLE_GRID, ST_COL_KEY, lkey );
	ptr_nd->SetupSection( _bstr_t( sz_section ) );
	
	_variant_t var_visible;
	ptr_nd->GetValue( _bstr_t( ST_COL_VISIBLE ), &var_visible );
	if( var_visible.vt == VT_I4 )
		return (var_visible.lVal == 1 );

	return true;
}

//////////////////////////////////////////////////////////////////////
void NotifyObjectChange( IUnknown* punk_table, const char* psz_event )
{
	INamedDataObject2Ptr ptr_obj( punk_table );
	if( ptr_obj == 0 )	return;

	IUnknown* punk_data = 0;
	ptr_obj->GetData( &punk_data );
	if( !punk_data )	return;

	INotifyControllerPtr ptr_nc(  punk_data );	
	punk_data->Release();	punk_data = 0;
	
	if( ptr_nc == 0  )	return;
	ptr_nc->FireEvent( _bstr_t( psz_event ), 0, punk_table, 0, 0 );

}

CString VTLoadString( LPCTSTR sz_id, LPCTSTR sz_sect ) 
{
	CString strResult( _T("") );

	// [vanek] : как и в CShellDocManagerDisp::LoadStringEx - 26.10.2004
	CString str_id( sz_id );
	str_id.TrimLeft();
	str_id.TrimRight();
	if( !str_id.IsEmpty() )
	{
		char	sz_string_file[MAX_PATH];
		GetModuleFileName( 0, sz_string_file, sizeof( sz_string_file ) );
		strcpy( strrchr( sz_string_file, '\\' )+1, "resource." );

		CString	strLang = (LPCTSTR)(GetValueString( GetAppUnknown(), "\\General", "Language", "en" ));
		strcat( sz_string_file, strLang );
		
		char	*pbuffer = strResult.GetBuffer( 4096 );
		char sz_default[] = "";
		::GetPrivateProfileString( sz_sect, sz_id, sz_default, pbuffer, 4096, sz_string_file );
		if(0==*pbuffer)
		{
			strcpy(pbuffer, sz_id);
			::WritePrivateProfileString( sz_sect, sz_id, pbuffer, sz_string_file );
		}

		char	*pout = pbuffer;

		while( *pbuffer )
		{
			if( *pbuffer == '\\' )
			{
				pbuffer++;
				if( *pbuffer == 'n' || *pbuffer == 'N' )
					*pbuffer = '\n';
				if( *pbuffer == 'r' || *pbuffer == 'R' )
					*pbuffer = '\r';
			}
			*pout = *pbuffer;
			pout++;pbuffer++;
		}
		*pout = 0;

		strResult.ReleaseBuffer();
	}

	return strResult;
}