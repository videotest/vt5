#include "stdafx.h"
#include "actions.h"
#include <Commdlg.h>
#include "alloc.h"
#include "measure5.h"
#include "core5.h"
#include <stdio.h>
#include "misc_utils.h"


void	_convert_rgb_to_gray( color **ppColor, byte *pGray, int cx )
{
    color	*pr, *pg, *pb;
	pr = ppColor[0];
	pg = ppColor[1];
	pb = ppColor[2];

	for( long n = 0; n < cx; n++ )
	{
		pGray[n] = Bright( ColorAsByte( pb[n] ), 
						ColorAsByte( pg[n] ), 
						ColorAsByte( pr[n] ) );

	}
}

//////////////////////////////////////////////////////////////////////
// CActionExportBrightness
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CActionExportBrightnessInfo ::s_pargs[] =
{
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////
CActionExportBrightness::CActionExportBrightness()
{
}

/////////////////////////////////////////////////////////////////////
CActionExportBrightness::~CActionExportBrightness()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CActionExportBrightness::DoGetInterface( const IID &iid )
{
	if( iid == IID_ILongOperation )
		return (ILongOperation*)this;
	
	return __super::DoGetInterface( iid );
}

/////////////////////////////////////////////////////////////////////
HRESULT	CActionExportBrightness::GetActionState(DWORD *pdwState)
{
	HRESULT hr = S_OK;
	hr = __super::GetActionState( pdwState );
    
	*pdwState &= ~afEnabled;
	IUnknown *punk_objlist = 0;
	punk_objlist = GetActiveObjectFromDocument( m_ptrTarget, szTypeObjectList );
	INamedDataObject2Ptr sptr_ndo = punk_objlist;
	if( punk_objlist )
		punk_objlist->Release(); punk_objlist = 0;

	if( sptr_ndo != 0 )
	{
		POSITION lpos_chld = 0; 
        sptr_ndo->GetActiveChild( &lpos_chld );
		if( lpos_chld )
				*pdwState |= afEnabled;
	}

    return hr;
}

/////////////////////////////////////////////////////////////////////
HRESULT	CActionExportBrightness::DoInvoke()
{
	IDataContextPtr sptr_context = m_ptrTarget;
	if( sptr_context == 0 )
		return E_FAIL;
									   
	IUnknown *punk_objlist = 0;
	punk_objlist = GetActiveObjectFromDocument( m_ptrTarget, szTypeObjectList );
	INamedDataObject2Ptr sptr_ndo = punk_objlist;
	if( punk_objlist )
		punk_objlist->Release(); punk_objlist = 0;

	if( sptr_ndo == 0 )
		return E_FAIL;
	
	POSITION lpos_chld = 0; 
    sptr_ndo->GetActiveChild( &lpos_chld );
	if( !lpos_chld )
		return E_FAIL;

	// get image
	IUnknown *punk_child = 0;
	sptr_ndo->GetNextChild( &lpos_chld, &punk_child );
	IMeasureObjectPtr sptr_obj = punk_child;
	if( punk_child )
		punk_child->Release(); punk_child = 0;

	if( sptr_obj == 0 )
		return E_FAIL;

	IUnknown *punk_image = 0;
	sptr_obj->GetImage( &punk_image );
	IImage2Ptr sptr_image = punk_image;
	if( punk_image )
		punk_image->Release(); punk_image = 0;

    if( sptr_image == 0 )
		return E_FAIL;

	// get color convertor
	IUnknown *punk_cc = 0;
	sptr_image->GetColorConvertor( &punk_cc );
	IColorConvertor2Ptr sptr_cc = punk_cc;
    if( punk_cc )
		punk_cc->Release(); punk_cc = 0;

	if( sptr_cc == 0 )
		return E_FAIL;

	int npanes_count = -1;
	_bstr_t bstr_ccname( _T("") );
	sptr_cc->GetCnvName( bstr_ccname.GetAddress() );
	if( bstr_ccname != _bstr_t(_T("GRAY")) && bstr_ccname != _bstr_t(_T("RGB")) )
	{
		HWND hwnd__main = 0;
		IApplicationPtr sptr_app = ::GetAppUnknown();
		sptr_app->GetMainWindowHandle( &hwnd__main );

		TCHAR	szmessage[150] = {0};
		::LoadString( App::handle(), IDS_WRONG_COLOR_MODEL_STR, szmessage, sizeof( szmessage ) );

		_bstr_t bstr_title( _T("") );
		m_ptrAI->GetCommandInfo( 0, bstr_title.GetAddress(), 0, 0 );

		::MessageBox( hwnd__main, szmessage, bstr_title, MB_OK | MB_ICONERROR );
        return true;                
	}
	sptr_cc->GetColorPanesDefCount( &npanes_count );

	_bstr_t bstrt_file_name(_T(""));
	bstrt_file_name = ::GetValueString( GetAppUnknown(), "\\ExportBrightness", "LastPath", "" );
    
	// get filename
	OPENFILENAME ofn = {0};
	TCHAR	szFile[MAX_PATH]={0},
			szTitle[100],
			stFilter[100];

	_tcsnccpy( szFile, (TCHAR *)(bstrt_file_name), max(0, min( bstrt_file_name.length(), sizeof(szFile)/sizeof(TCHAR) - 1 )) ); 

	::LoadString( App::handle(), IDS_SAVEAS_DIALOG_TITLE, szTitle, sizeof( szTitle ) );
	::LoadString( App::handle(), IDS_SAVEAS_DIALOG_FILTER, stFilter, sizeof( stFilter ) );

	int nFilterLength = _tcslen( stFilter );
	for(int i = 0; i < nFilterLength; i ++ )
	{
		if( stFilter[i] == _T('\n') )
			stFilter[i] = 0;
	}

	ofn.lStructSize = CDSIZEOF_STRUCT(OPENFILENAMEA,lpTemplateName);
	ofn.hwndOwner   = ::GetActiveWindow();
	ofn.lpstrFilter = stFilter;
	ofn.lpstrFile   = szFile;
	ofn.nMaxFile    = sizeof( szFile );
	ofn.lpstrTitle  = szTitle;
	ofn.lpstrDefExt = _T("txt");
	ofn.Flags       =	OFN_NONETWORKBUTTON | OFN_NOCHANGEDIR  |OFN_LONGNAMES | OFN_ENABLESIZING |
						OFN_EXTENSIONDIFFERENT | OFN_OVERWRITEPROMPT;

	if( !::GetSaveFileNameA( &ofn ) )
		return S_OK; // return to script

	bstrt_file_name = ofn.lpstrFile;

	// open file
	FILE *pf = 0;
    pf = _tfopen( bstrt_file_name, _T("w") );
	if( !pf )
		return E_FAIL;

	_bstr_t bstr_separator( _T("") );
	bstr_separator = ::GetValueInt( ::GetAppUnknown(), "\\ExportBrightness", "UseTab", 1 ) != 0 ? _T("\t") : _T(" ");

	_bstr_t bstr_intensity( _T("Intensity") );
	//TCHAR	szIntensity[100] = {0};
	//::LoadString( App::handle(), IDS_INTENSITY_STR, szIntensity, sizeof( szIntensity ) );
	bstr_intensity = ::GetValueString( ::GetAppUnknown(), "\\ExportBrightness", "IntensityColumnName", bstr_intensity );
	_ftprintf( pf, _T("X%sY%s%s\n"), (TCHAR *)(bstr_separator), (TCHAR *)(bstr_separator), (TCHAR *)(bstr_intensity) );

	int cx = 0, cy = 0;
    sptr_image->GetSize( &cx, &cy );
	/* npanes_count = GetImagePaneCount( sptr_image );
    if( npanes_count == -1 )
	{	// опа, работаем со старой версией - нет фишового конвертера,
		// но на всякий случай проверим
		_bstr_t bstr_ccname( _T("") );
		sptr_cc->GetCnvName( bstr_ccname.GetAddress() );
        if( bstr_ccname != _bstr_t(_T("FISH")) )
			sptr_cc->GetColorPanesDefCount( &npanes_count );
	}

	// [vanek] : определение RGB-конвертора - 05.11.2004
	bool bis_rgb_cc = false;
	if( npanes_count == 3 )
	{
        _bstr_t bstr_ccname( _T("") );
		sptr_cc->GetCnvName( bstr_ccname.GetAddress() );
        bis_rgb_cc  = bstr_ccname == _bstr_t(_T("RGB"));
	}*/

	unsigned long ulpixels_processed = 0;
	if( npanes_count > 0 )
	{
		StartNotification( cy, 1, 1 );

		for( int y = 0; y < cy; y ++ )
		{
			Notify(y);
			smart_alloc( spcolors, color*, npanes_count);
			for( int npane = 0; npane < npanes_count; npane ++ )
				sptr_image->GetRow( y, npane, &spcolors[npane] );
			
			smart_alloc( spBrightness, byte, cx);
			if( /*bis_rgb_cc*/npanes_count == 3 )
				_convert_rgb_to_gray( spcolors, spBrightness, cx );
			else
			{
				IColorConvertor5Ptr sptr_cc5 = sptr_cc;
				if( sptr_cc5 != 0 )
					sptr_cc5->ConvertImageToDIBEx( spcolors, spBrightness, cx, FALSE, npanes_count );
				else
					sptr_cc->ConvertImageToDIB( spcolors, spBrightness, cx, FALSE );
			}

			byte *pbmask = 0;
			sptr_image->GetRowMask(y, &pbmask );                        
			for( int x = 0; x < cx; x ++ )
			{
				if( pbmask[ x ] & 128 ) // 7-й бит - это объект
				{
                    _ftprintf( pf, _T("%d%s%d%s%d\n"), x, (TCHAR *)(bstr_separator), y, (TCHAR *)(bstr_separator), spBrightness[x] );
					ulpixels_processed ++;
				}
			}
		}   

		FinishNotification();
	}

	// close file
	fclose( pf );
	pf = 0;

	// show message if need
	if( ::GetValueInt( GetAppUnknown(), "\\ExportBrightness", "ShowMessage", 1 ) != 0 )
	{
		TCHAR	szformat[100] = {0},
				szmessage[150] = {0};
		::LoadString( App::handle(), IDS_PROCESSED_PIXELS, szformat, sizeof( szformat ) );
		_stprintf( szmessage, szformat, ulpixels_processed );

		_bstr_t bstr_title( _T("") );
		m_ptrAI->GetCommandInfo( 0, bstr_title.GetAddress(), 0, 0 );

		HWND hwnd__main = 0;
		IApplicationPtr sptr_app = ::GetAppUnknown();
		sptr_app->GetMainWindowHandle( &hwnd__main );
		::MessageBox( hwnd__main, szmessage, (TCHAR *)(bstr_title), MB_OK | MB_ICONINFORMATION );
	}

	// store path
	::SetValue( GetAppUnknown(), "\\ExportBrightness", "LastPath", (char *)bstrt_file_name );

	return S_OK;
}


//////////////////////////////////////////////////////////////////////
// CActionSetActiveImageDB
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CActionSetActiveImageDBInfo ::s_pargs[] =
{
	{"strNameField",szArgumentTypeString,0,true,false},
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////
CActionSetActiveImageDB::CActionSetActiveImageDB()
{
}

/////////////////////////////////////////////////////////////////////
CActionSetActiveImageDB::~CActionSetActiveImageDB()
{
}

HRESULT	CActionSetActiveImageDB::DoInvoke()
{
	sptrIDocumentSite	spDocSite( m_ptrTarget );	
	if( spDocSite == NULL )						
		return false;								 
														
	sptrIDBaseDocument spDBDoc( spDocSite );			
	if( spDBDoc == NULL )
		return false;
	IUnknown* pUnkQuery = NULL;
	if( S_OK != spDBDoc->GetActiveQuery( &pUnkQuery ) )
		return false;
					 
	if( pUnkQuery == NULL )
		return false;
					  
	sptrIQueryObject spQueryData( pUnkQuery );
	sptrISelectQuery spSelectQuery( pUnkQuery );
																	
	pUnkQuery->Release();
																
	if( spQueryData == NULL || spSelectQuery == NULL )
		return false;	

	sptrINamedDataObject	sptrO( spQueryData );

	IDocumentSitePtr ptr_ds( spDocSite );
	if( ptr_ds == 0 )
		return false;

	IUnknownPtr ptr_view;
	ptr_ds->GetActiveView( &ptr_view );
	if( ptr_view == 0 )
		return false;

	if( !CheckInterface( ptr_view, IID_IBlankView ) )
		return false;

	//Смотрим активное поле в запросе
	IQueryObjectPtr ptr_query = pUnkQuery;
	if( ptr_query == 0 )
		return false;

	_bstr_t		bstrt_object_name = GetArgString( "strNameField" );
	CString strNF;
	strNF.Format("%s",(char*)bstrt_object_name);
	int nidx = 0;
	ptr_query->GetFieldsCount(&nidx);
	for(int i=0; i<nidx; i++)
	{
		_bstr_t bstr_table, bstr_field;
		ptr_query->GetField( i, 0, bstr_table.GetAddress(), bstr_field.GetAddress() ); 

		if( !bstr_table.length() || !bstr_field.length() )
			return false;

		CString str_obj_name;
		str_obj_name.Format( "%s.%s", (char*)bstr_table, (char*)bstr_field );
		if(strNF==str_obj_name)
		{
			ptr_query->SetActiveField(i);
			break;
		}
	}		
	return S_OK;
}
