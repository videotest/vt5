#include "stdafx.h"
#include "helpbase.h"
#include "window5.h"
#include "htmlhelp.h"
#include "core5.h"

void HelpRemoveSpaces( char *psz )
{
	while( 1 ) 
	{
		char	*pSpace = strchr( psz, ' ' );
		if( pSpace )*pSpace = '_';
		else return;
	}
}


bool HelpRegisterWindow( const char *pszCaption, const char *pszType )
{
	//get main window handle 
	HWND	hwndMain = 0;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwndMain );
	hwndMain = ::GetDesktopWindow();

	//register wintype
	HH_WINTYPE	hhwin;
	ZeroMemory( &hhwin, sizeof( hhwin ) );

	hhwin.cbStruct = sizeof( hhwin );
	hhwin.pszType = pszType;
	hhwin.pszCaption = pszCaption;
	hhwin.fsWinProperties = HHWIN_PROP_NOTITLEBAR|HHWIN_PROP_ONTOP;

	::HtmlHelp( hwndMain, 0, HH_SET_WIN_TYPE, (DWORD)&hhwin );
	
	return true;
}

bool HelpLoadCollection( const char *pszWinType )
{
	static HWND	hwndCollection = 0;

	if( IsWindow( hwndCollection ) )return true;

	CString strHtmlFileName = GetDirectory( "HelpPath", "Help" );

	//collection support
	CString	strConfigName = ::GetValueString( GetAppUnknown(), "\\Configuration", "LastFile", "" );
	CString	strColName;
	if( strConfigName.IsEmpty() )
		strConfigName = "shell.config";

	char	*p = strConfigName.GetBuffer( 0 );
	char	szName[_MAX_PATH];
	::_splitpath( p, 0, 0, szName, 0 );
	strColName = strHtmlFileName;
	strColName += szName;
	strColName += ".col";
/*	if( pszWinType )
	{
		strColName+=">";
		strColName+=pszWinType;
	}*/

	HWND	hwndMain = 0;
	IApplicationPtr	ptrA( GetAppUnknown() );
	ptrA->GetMainWindowHandle( &hwndMain );
	hwndMain = ::GetDesktopWindow();

	hwndCollection = ::HtmlHelp( hwndMain, strColName, HH_DISPLAY_TOPIC, 0 );
	if( hwndCollection == 0 )
	{
		CString	str;
		str.Format( "Error displaying collection %s", strColName );
		AfxMessageBox( str );
		return	 false;
	}

	ShowWindow( hwndCollection, SW_HIDE );


	return true;
}


bool HelpDisplayPopup( const char *pszFileName, const char *pszPrefix, const char *pszName, HWND hwnd )
{
	if (::GetCapture() != NULL)
		ReleaseCapture();


	CString	strHtmlFileName = pszPrefix;
	strHtmlFileName += pszFileName;
	strHtmlFileName += ".chm";


	strHtmlFileName+="::/";
	if( pszPrefix )strHtmlFileName+=pszPrefix;
	strHtmlFileName+=pszName;
	strHtmlFileName+="/ctrltext.txt";

	HH_POPUP	popup;
	ZeroMemory( &popup, sizeof( popup ) );

	popup.cbStruct = sizeof( popup );
	popup.clrBackground = -1;
	popup.clrForeground = -1;
	popup.hinst = 0;
	popup.idString = ::GetWindowContextHelpId( hwnd );
	popup.pszFont = "Arial, 8, 1";
	popup.pszText = 0;
	popup.pt.x = -1;
	popup.pt.y = -1;
	popup.rcMargins.left = -1;
	popup.rcMargins.right = -1;
	popup.rcMargins.top = -1;
	popup.rcMargins.bottom = -1;


	::HelpRemoveSpaces( strHtmlFileName.GetBuffer(0) );
	strHtmlFileName.ReleaseBuffer();

	if( ::HtmlHelp( hwnd, strHtmlFileName, HH_DISPLAY_TEXT_POPUP, (DWORD_PTR)(LPVOID)&popup ) == 0 )
	{
		CString	str;
		str.Format( "Error displaying help %s", strHtmlFileName );
		AfxMessageBox( str );
		return false;
	}

	return true;
}

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
	CString	strHtmlFileName(_T(""));
    CString str_help_path( _T("") );
	str_help_path = GetDirectory( "HelpPath", "Help" );
	if( ::GetValueInt( GetAppUnknown(), "\\Help", "UseCollection", 1 ) != 0)
	{	// используем коллекцию - страрый выриант

		//load collection
		CString strColFileName = str_help_path ;

		//collection support
		CString	strConfigName = ::GetValueString( GetAppUnknown(), "\\Configuration", "LastFile", "" );
		CString	strColName;
		if( strConfigName.IsEmpty() )
			strConfigName = "shell.config";

		char	*p = strConfigName.GetBuffer( 0 );
		char	szName[_MAX_PATH];
		::_splitpath( p, 0, 0, szName, 0 );
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

		::HelpRemoveSpaces( strHtmlFileName.GetBuffer( 0 )  );
		strHtmlFileName.ReleaseBuffer();

		hwndHelp = ::HtmlHelp( hwndMain, strColName, HH_DISPLAY_TOPIC, (DWORD)(const char *)strHtmlFileName  );
		
	}
	else
	{						  
		CString str_chm_name( pszFileName );		
		if( pszPrefix )
		{
			if( !strcmp( pszPrefix, "ax" ) )
				str_chm_name = _T("Forms");
			else if( !strcmp( pszPrefix, "as" ) )
				str_chm_name = _T("Scripts");
		}
		
		// вначале пытаемся найти и загрузить топик из файла <str_chm_file> = "<help_path>\<str_chm_name>.chm"
		CString str_chm_file(str_help_path);
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

			::HelpRemoveSpaces( strHtmlFileName.GetBuffer( 0 )  );
			strHtmlFileName.ReleaseBuffer();

			hwndHelp = ::HtmlHelp( hwndMain, strHtmlFileName, HH_DISPLAY_TOPIC, 0 );
		}
		
		if( !hwndHelp )
		{	// топик не создался, тогда пытаемся загрузить его из основного файла
			CString str_folder_name( str_chm_name );
			str_chm_name = ::GetValueString( GetAppUnknown(), "\\General", "ProgramName", "Main" );			
			
			strHtmlFileName = str_help_path;
			strHtmlFileName	+= str_chm_name;
			strHtmlFileName	+= _T(".chm");
			strHtmlFileName	+=	_T("::/");
			CString strHtmlSubName;
			strHtmlSubName	+=	str_folder_name;
			strHtmlSubName	+=	_T("/");
			strHtmlSubName	+=	pszName;
			strHtmlSubName	+=	_T(".html");
			strHtmlSubName	+=	_T(">");
			strHtmlSubName	+=	pszWinType;
			::HelpRemoveSpaces( strHtmlSubName.GetBuffer( 0 )  );
			strHtmlSubName.ReleaseBuffer();
			strHtmlFileName += strHtmlSubName;

			hwndHelp = ::HtmlHelp( hwndMain, strHtmlFileName, HH_DISPLAY_TOPIC, 0 );        
		}
	}

	if( !hwndHelp )
	{
		CString	str;
		str.Format( "Error displaying topic %s", strHtmlFileName );
		AfxMessageBox( str );
		return	 false;
	}
	
	::ShowWindow( hwndHelp, SW_SHOW );
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//CHelpInfoImpl
IMPLEMENT_UNKNOWN_BASE(CHelpInfoImpl, Help);

CHelpInfoImpl::CHelpInfoImpl()
{
	m_hHelpInstance = AfxGetApp()->m_hInstance;
}

const char *CHelpInfoImpl::GetHelpTopic()
{
	return 0;
}

HRESULT CHelpInfoImpl::XHelp::GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags )
{
	METHOD_PROLOGUE_BASE(CHelpInfoImpl, Help)

	char	szModuleName[_MAX_PATH]; 

	const char	cszHelp[] = "Help";
	CString	strHelpFileName;

	::GetModuleFileName( pThis->m_hHelpInstance, szModuleName, _MAX_PATH );
	::GetLongPathName( szModuleName, szModuleName, _MAX_PATH ) ;
	char	*pszFileName = strrchr( szModuleName, '\\' );
	if( pszFileName )pszFileName++;
	char	*pszExt = strrchr( pszFileName, '.' );
	if( pszExt )*pszExt = 0;

	strHelpFileName = pszFileName;
	//strHelpFileName+= ".chm";

	CString	strTopicName = pThis->GetHelpTopic();
	if( strTopicName.IsEmpty() )
		strTopicName = ::GetObjectName( this );
	if( pbstrHelpFile )
		*pbstrHelpFile = strHelpFileName.AllocSysString();
	if( pbstrHelpTopic ) 
		*pbstrHelpTopic = strTopicName.AllocSysString();
	if( pdwFlags )
		*pdwFlags = 0;

	return S_OK;
}

HRESULT CHelpInfoImpl::XHelp::GetHelpInfo2( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags, BSTR *pbstrHelpPrefix )
{
    METHOD_PROLOGUE_BASE(CHelpInfoImpl, Help)
	HRESULT hr = S_FALSE;
	if( S_OK == (hr = GetHelpInfo( pbstrHelpFile, pbstrHelpTopic, pdwFlags )) )
	{
        if( pbstrHelpPrefix )
			*pbstrHelpPrefix = _bstr_t( _T("") ).copy();
	}
    return hr;
}

std_dll void HelpDisplay( IUnknown *punkHelpInfo )
{
	/*if( !CheckInterface( punkHelpInfo, IID_IHelpInfo ) )
		return;*/

	CString	strHtmlFileName( _T("") ),
			strTopicName( _T("") ),
			strPrefix( _T("") );

	IHelpInfoPtr	sptrH( punkHelpInfo );
	if( sptrH == 0 ) 
		return;

	// [vanek]  - 04.11.2004
	BSTR bstrFileName = 0, bstrTopicName = 0, bstrPrefix = 0;
	IHelpInfo2Ptr	sptrH2 = sptrH;
	if( sptrH2 != 0 )
    {
        sptrH2->GetHelpInfo2( &bstrFileName, &bstrTopicName, 0, &bstrPrefix );
	}
	else
		sptrH->GetHelpInfo( &bstrFileName, &bstrTopicName, 0 );
	
	if( bstrFileName )
	{
		strHtmlFileName = bstrFileName;
		::SysFreeString( bstrFileName ); bstrFileName = 0;
	}

	if( bstrTopicName )
	{
		strTopicName = bstrTopicName;
		::SysFreeString( bstrTopicName ); 	bstrTopicName = 0;
	}

	if( bstrPrefix )
	{	
		strPrefix = bstrPrefix;
		::SysFreeString( bstrPrefix ); bstrPrefix = 0;
	}

//execute HtmlHelp
	// A.M.
	if (!strHtmlFileName.IsEmpty())
		::HelpDisplayTopic( strHtmlFileName, strPrefix, strTopicName, "$GLOBAL_main" );
}


