#include "stdafx.h"
#include "storage_actions.h"
#include "StorageObj_int.h"
#include "\vt5\common2\misc_utils.h"
#include <Commdlg.h>


//////////////////////////////////////////////////////////////////////
//
//	class CCreateStorageObjInfo
//
//////////////////////////////////////////////////////////////////////
_ainfo_base::arg CCreateStorageObjInfo::s_pargs[] = 
{	
	{"ObjectName",		szArgumentTypeString, "", true, false },
	{"FileName",		szArgumentTypeString, "", true, false },
    {"Temporary",		szArgumentTypeInt, "0", true, false },
	{"OverwritePrompt",	szArgumentTypeInt, "1", true, false },
	{0, 0, 0, false, false },
};

//////////////////////////////////////////////////////////////////////
//
//	class CCreateStorageObj
//
//////////////////////////////////////////////////////////////////////
CCreateStorageObj::CCreateStorageObj()
{
}

//////////////////////////////////////////////////////////////////////
CCreateStorageObj::~CCreateStorageObj()
{
}

//////////////////////////////////////////////////////////////////////
IUnknown* CCreateStorageObj::DoGetInterface( const IID &iid )
{
	if( iid == IID_IUndoneableAction )return (IUndoneableAction*)this;
	return CAction::DoGetInterface( iid );
}

//////////////////////////////////////////////////////////////////////
HRESULT	CCreateStorageObj::_get_action_user_name( BSTR *pbstrName )
{
	if( !pbstrName )
		return S_FALSE;

	IUnknown *punkInfo = 0;
	HRESULT hres = GetActionInfo( &punkInfo );
	if( hres == S_OK )
	{
		IActionInfoPtr sptrActionInfo(punkInfo);

		punkInfo->Release( );
		punkInfo = 0;

		hres = sptrActionInfo->GetCommandInfo( 0, pbstrName, 0 ,0 );            
		sptrActionInfo = 0;
	}

	return hres;
}

//////////////////////////////////////////////////////////////////////
HRESULT CCreateStorageObj::DoInvoke()
{
	_bstr_t		bstrt_object_name = GetArgString( "ObjectName" ),
				bstrt_file_name = GetArgString( "FileName" );

	VARIANT_BOOL	vbCreateTemp = (GetArgLong( "Temporary" ) == 0) ? VARIANT_FALSE : VARIANT_TRUE;
	BOOL			bOverwritePrompt = ( GetArgLong( "OverwritePrompt" ) != 0 );

	IStorageObjectPtr sptrStorageObj;
    
	// create new object
	IUnknown* punk_new_object = 0;		
	punk_new_object = ::CreateTypedObject( _bstr_t(szTypeStorageObject) );
	if( !punk_new_object )
		return S_OK;	// return to script
    
	sptrStorageObj = punk_new_object;
	punk_new_object->Release();
    
	if( sptrStorageObj == 0 )
		return S_OK;	// return to script

	// is filename empty?
	if( !bstrt_file_name.length() )
	{	
		OPENFILENAME ofn = {0};
		//::ZeroMemory( &ofn, sizeof(OPENFILENAME) );

		char	szFile[MAX_PATH],
				szTitle[100],
				stFilter[100];

		::LoadString( App::handle(), IDS_SAVEAS_DIALOG_TITLE, szTitle, sizeof( szTitle ) );
		::LoadString( App::handle(), IDS_SAVEAS_DIALOG_FILTER, stFilter, sizeof( stFilter ) );

		int nFilterLength = strlen( stFilter );
		for(int i = 0; i < nFilterLength; i ++ )
		{
			if( stFilter[i] == '\n' )
				stFilter[i] = 0;
		}

		szFile[0] = '\0';
		
		ofn.lStructSize = CDSIZEOF_STRUCT(OPENFILENAMEA,lpTemplateName);//sizeof(ofn);
		ofn.hwndOwner   = ::GetActiveWindow();
		ofn.lpstrFilter = stFilter;
		ofn.lpstrFile   = szFile;
		ofn.nMaxFile    = sizeof( szFile );
		ofn.lpstrTitle  = szTitle;
		ofn.lpstrDefExt = "storage";
		ofn.Flags       =	OFN_NONETWORKBUTTON | OFN_NOCHANGEDIR  |OFN_LONGNAMES | OFN_ENABLESIZING |
							OFN_EXTENSIONDIFFERENT;

		if( bOverwritePrompt )
			ofn.Flags |= OFN_OVERWRITEPROMPT;

		if( !::GetSaveFileNameA( &ofn ) )
			return S_OK; // return to script

		bstrt_file_name = ofn.lpstrFile;
	}
	else
	{
		OFSTRUCT stOpenFile = {0};
		if( HFILE_ERROR != OpenFile( bstrt_file_name, &stOpenFile, OF_EXIST) && bOverwritePrompt )
		{	// file exist
			char	szText[ MAX_PATH + 100] = {0},
					*pszTitle = 0;

			BSTR	bstr_action_user_name = 0;
			_get_action_user_name( &bstr_action_user_name );
			int nTitleLen = SysStringLen( bstr_action_user_name );
			pszTitle = new char[ nTitleLen + 1];
			*(pszTitle + nTitleLen) = 0;
					            		
			if( bstr_action_user_name )
			{
				strncpy( pszTitle, _bstr_t( bstr_action_user_name, false ), nTitleLen);
				::SysFreeString( bstr_action_user_name );
				bstr_action_user_name = 0;
			}

			strcpy( szText, bstrt_file_name);
			char strBuff[100];
			::LoadString( App::handle(), IDS_EXIST_FILE, strBuff, sizeof( strBuff ) );
			strcat( szText, strBuff );
			
			BOOL bReCreate = (IDYES == ::MessageBox( ::GetActiveWindow(), szText, pszTitle, MB_YESNO |
							MB_ICONEXCLAMATION ) );

			if(pszTitle)
			{
				delete[] pszTitle;
				pszTitle = 0;
			}

			if( !bReCreate )
				return S_OK;	// return to script 
		}
	}

	HRESULT hr = sptrStorageObj->InitNew( bstrt_file_name, vbCreateTemp ); 
	if( hr != S_OK )
	{
		BSTR bstr_action_user_name = 0;
		_get_action_user_name( &bstr_action_user_name );
		if( bstr_action_user_name ) 
		{
			DislayWin32Error( hr, ::GetActiveWindow( ), _bstr_t( bstr_action_user_name ) );
			::SysFreeString( bstr_action_user_name );
			bstr_action_user_name = 0;
		}
		else
		   DislayWin32Error( hr, ::GetActiveWindow( ), 0 );
		return /*hr*/ S_OK;	// return to script
	}

	//is object exist?
	if( bstrt_object_name.length() )
	{
		IUnknown* punk_object_exist = GetObjectByName( m_ptrTarget, bstrt_object_name, _bstr_t(szTypeStorageObject) );
		if( punk_object_exist )
		{
			//remove old object from document
			RemoveFromDocData( m_ptrTarget, punk_object_exist );
            punk_object_exist->Release( );
			punk_object_exist = 0;
		}

		//set table name
		INamedObject2Ptr ptr_no2( sptrStorageObj );
		if( ptr_no2 )
			ptr_no2->SetName( bstrt_object_name );
	}

	//add new object to document
	SetToDocData( m_ptrTarget, sptrStorageObj );
	return S_OK;
}

