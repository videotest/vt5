#include "stdafx.h"
#include "impl_help.h"
#include "com_main.h"
#include "misc_utils.h"
#include "action5.h"
#include <tchar.h>

BSTR GetDirectory( const char *pszRecord, const char *pszFolder )
{
	char	szPathName[_MAX_PATH];

	::GetModuleFileName( App::handle(), szPathName, _MAX_PATH );
	::GetLongPathName( szPathName, szPathName, _MAX_PATH ) ;

	char	*pszLast = strrchr( szPathName, '\\' );
	*pszLast = 0;
	strcat( szPathName, "\\" );
	strcat( szPathName, pszFolder );
	strcat( szPathName, "\\" );

	_bstr_t	bstrFileName = 
	::GetValueString( ::GetAppUnknown(), "Paths", pszRecord, szPathName );

	return bstrFileName.copy();
}



HRESULT CHelpInfoImpl::GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags )
{
	char	szModuleName[_MAX_PATH]; 
	const char	cszHelp[] = "Help";

	::GetModuleFileName( App::handle(), szModuleName, _MAX_PATH );
	::GetLongPathName( szModuleName, szModuleName, _MAX_PATH ) ;
	char	*pszFileName = strrchr( szModuleName, '\\' );
	if( pszFileName )pszFileName++;
	char	*pszExt = strrchr( pszFileName, '.' );
	if( pszExt )*pszExt = 0;

	/*_bstr_t	bstrConfigName = ::GetValueString( GetAppUnknown(), "\\Configuration", "LastFile", "" );
	_bstr_t	bstrColName;
	if( bstrConfigName.length() )
	{
		const char	*p = bstrConfigName;
		char	szName[_MAX_PATH];
		::_splitpath( p, 0, 0, szName, 0 );
		bstrColName = szName;
		bstrColName += ".col";
	}
	_bstr_t	bstrHtmlFileName( GetDirectory( "HelpPath", "Help" ), false );
	bstrHtmlFileName+=bstrColName;
	bstrHtmlFileName+="::";*/
	_bstr_t	bstrHtmlFileName = pszFileName;

//extract from named object
	if( m_bstrHelpTopic.length() == 0 )
	{
		BSTR	bstrName = 0;
		INamedObject2Ptr	ptrNamed( this );
		if( ptrNamed != 0 )ptrNamed->GetName( &bstrName );
		if( bstrName )m_bstrHelpTopic = bstrName;

		if( bstrName )	::SysFreeString( bstrName );	bstrName = 0;
	}
//extract from action
	if( m_bstrHelpTopic.length() == 0 )
	{
		BSTR	bstrName = 0;
		IActionInfo2Ptr	ptrAI( this );
		if( ptrAI != 0 )ptrAI->GetCommandInfo( &bstrName, 0, 0, 0 );
		if( bstrName ) m_bstrHelpTopic = bstrName;

		if( bstrName )	::SysFreeString( bstrName );	bstrName = 0;
	}
//extract from page
	if( m_bstrHelpTopic.length() == 0 )
	{
		BSTR	bstrName = 0;
		IOptionsPagePtr	ptrPage( this );
		if( ptrPage!= 0 )ptrPage->GetCaption( &bstrName );
		if( bstrName ) m_bstrHelpTopic = bstrName;

		if( bstrName )	::SysFreeString( bstrName );	bstrName = 0;
	}
//fault - exit
	if( m_bstrHelpTopic.length() == 0 )
		return S_FALSE;

	if( pbstrHelpFile )
		*pbstrHelpFile = bstrHtmlFileName.copy();
	if( pbstrHelpTopic ) 
		*pbstrHelpTopic = m_bstrHelpTopic.copy();
	if( pdwFlags )
		*pdwFlags = 0;

	return S_OK;
}

HRESULT CHelpInfoImpl::GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix )
{
	HRESULT hr = S_FALSE;
	if( S_OK == (hr  = GetHelpInfo( pbstrHelpFile, pbstrHelpTopic, pdwFlags )) )
	{
		if( pbstrHelpPrefix )
			*pbstrHelpPrefix = _bstr_t(_T("")).copy();
	}

	return hr;
}

#include "core5.h"
#include "htmlhelp.h"
#pragma comment(lib, "htmlhelp.lib")

bool HelpDisplayTopic( const char *pszFileName, const char *pszPrefix, const char *pszName, const char *pszWinType )
{
	if (::GetCapture() != NULL)
		ReleaseCapture();
	//get main window handle 
	HWND	hwndMain = 0;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwndMain );
	hwndMain = ::GetDesktopWindow();

	// [vanek] : загрузка нового хелпа - нет коллекций *.chm(+ *.chi) файлов, вместо этого используется один файл
	// *.chm на всю программу - 02.11.2004
	HWND hwndHelp = 0;
	_bstr_t	strHtmlFileName(_T(""));
    _bstr_t str_help_path( _T("") );
	str_help_path = GetDirectory( "HelpPath", "Help" );

	if( ::GetValueInt( GetAppUnknown(), "\\Help", "UseCollection", 1 ) != 0)
	{	// используем коллекцию - страрый выриант

		//load collection
		_bstr_t	strColFileName = GetDirectory( "HelpPath", "Help" );

		//collection support
		_bstr_t	strConfigName = ::GetValueString( GetAppUnknown(), "\\Configuration", "LastFile", "" );
		_bstr_t	strColName;
		if( strConfigName.length()==0 )
			strConfigName = "shell.config";

		char	szName[_MAX_PATH];
		::_splitpath( strConfigName, 0, 0, szName, 0 );
		strColName = strColFileName;
		strColName += szName;
		strColName += ".col";

		if( pszWinType )
		{
			strColName+=">";
			strColName+=pszWinType;
		}


		strHtmlFileName = pszPrefix;
		strHtmlFileName+=pszFileName;
		strHtmlFileName+=".chm";
		strHtmlFileName+="::/";
		strHtmlFileName+=pszPrefix;
		strHtmlFileName+=pszFileName;
		strHtmlFileName+="/";
		strHtmlFileName+=pszName;
		strHtmlFileName+=".html";

		//::HelpRemoveSpaces( strHtmlFileName.GetBuffer( 0 )  );
		//strHtmlFileName.ReleaseBuffer();

		hwndHelp = ::HtmlHelp( hwndMain, strColName, HH_DISPLAY_TOPIC, (DWORD)(const char *)strHtmlFileName  );
	}
	else
	{
		_bstr_t str_chm_name( pszFileName );		
		if( pszPrefix )
		{
			if( !strcmp( pszPrefix, "ax" ) )
				str_chm_name = _T("Forms");
			else if( !strcmp( pszPrefix, "as" ) )
				str_chm_name = _T("Scripts");
		}
		
		// вначале пытаемся найти и загрузить топик из файла <str_chm_file> = "<help_path>\<str_chm_name>.chm"
		_bstr_t str_chm_file(str_help_path);
		str_chm_file += str_chm_name;
		str_chm_file += _T(".chm"); 
		HANDLE	hfile = ::CreateFile( str_chm_file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,  
						FILE_ATTRIBUTE_NORMAL, 0 );

		if( hfile != INVALID_HANDLE_VALUE )
        {   // файл "<help_path>\<pszFileName>.chm" есть - грузим топик из него         
			::CloseHandle( hfile ); hfile = 0;

			strHtmlFileName = str_chm_file;
			strHtmlFileName	+=	_T("::/");
			strHtmlFileName	+=	str_chm_name;
			strHtmlFileName	+=	_T("/");
			strHtmlFileName	+=	pszName;
			strHtmlFileName	+=	_T(".html");
			strHtmlFileName	+=	_T(">");
			strHtmlFileName	+=	pszWinType;

			hwndHelp = ::HtmlHelp( hwndMain, strHtmlFileName, HH_DISPLAY_TOPIC, 0 );
		}
		
		if( !hwndHelp )
		{	// топик не создался, тогда пытаемся загрузить его из основного файла
			_bstr_t str_folder_name( str_chm_name );
			str_chm_name = ::GetValueString( GetAppUnknown(), "\\General", "ProgramName", "Main" );			
			
			strHtmlFileName = str_help_path;
			strHtmlFileName	+= str_chm_name;
			strHtmlFileName	+= _T(".chm");
			strHtmlFileName	+=	_T("::/");
			strHtmlFileName	+=	str_folder_name;
			strHtmlFileName	+=	_T("/");
			strHtmlFileName	+=	pszName;
			strHtmlFileName	+=	_T(".html");
			strHtmlFileName	+=	_T(">");
			strHtmlFileName	+=	pszWinType;

			hwndHelp = ::HtmlHelp( hwndMain, strHtmlFileName, HH_DISPLAY_TOPIC, 0 );        
		}
	}


	if( !hwndHelp )
	{
		char	sz[200];
		sprintf( sz, "Error displaying topic %s", (const char *)strHtmlFileName );
		::MessageBox( 0, sz, "Error", MB_OK );
		return	 false;
	}

	::ShowWindow( hwndHelp, SW_SHOW );
    return true;
}

bool HelpDisplay( const char *pszFileName )
{
	char	sz[MAX_PATH];
	::GetModuleFileName( App::handle(), sz, sizeof( sz ) );
	char	szChm[MAX_PATH];
	::_splitpath( sz, 0, 0, szChm, 0 );

	return HelpDisplayTopic( szChm, "", pszFileName, "$GLOBAL_main" );
}

