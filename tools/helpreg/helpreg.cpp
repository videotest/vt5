// helpreg.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "time.h"
#pragma warning(disable:4786)
#include "map"
#include "string"
#include "list"

using namespace std;

typedef	map<string, string>	maps2s;
typedef	list<string>		lists;

char	g_szPrefix[100];
char	g_szLang[10] = "1033";
bool	g_bRemove = false;

void _error( const char *psz )
{	MessageBox( 0, psz, "Help Registrator", MB_OK );	}


bool load_file( const char *pszFile, lists	&strings )
{
	FILE	*pfile = ::fopen( pszFile, "rt" );
	if( !pfile )
	{
		char	sz[280];
		sprintf( sz, "Can't open file\n%s", pszFile );
		_error( sz );
		return false;
	}

	strings.clear();
	char	sz[255];
	while( ::fgets( sz, 255, pfile ) )
	{
		char	*p = strchr( sz, '\n' );
		if( p )*p = 0;
		strings.push_back( sz );
	}
	::fclose( pfile );
	return true;
}

bool backup_file( const char *pszFile )
{
	char	sz[255];
	strcpy( sz, pszFile );
	char	*pszE = strrchr( sz, '.' );
	if( pszE )
		strcpy( pszE, ".old" );
	else
		strcat( sz, ".old" );

	::MoveFile( pszFile, sz );
	return true;
}

bool store_file( const char *pszFile, lists	&strings )
{
	FILE	*pfile = ::fopen( pszFile, "wt" );
	if( !pfile )
	{
		char	sz[280];
		sprintf( sz, "Can't store file\n%s", pszFile );
		_error( sz );
		return false;
	}


	lists::iterator	rs = strings.begin();

	while( rs != strings.end() )
	{
		const char	*psz = (*rs).c_str();
		fputs( psz, pfile );
		fputs( "\n", pfile );
		rs++;
	}

	::fclose( pfile );
	return true;
}

/*<DocCompilations>
<DocCompilation>
	<DocCompId value="FP"/>
	<DocCompLanguage value=1049/>
	<LocationHistory>
		<ColNum value=1/>
		<TitleLocation value="D:\vt5\help\FP.chm"/>
		<IndexLocation value="D:\vt5\help\FP.chi"/>
		<QueryLocation value=""/>
		<LocationRef value=""/>
		<Version value=29227493/>
		<LastPromptedVersion value=0/>
		<TitleSampleLocation value=""/>
		<TitleQueryLocation value=""/>
		<SupportsMerge value=0/>
	</LocationHistory>
</DocCompilation>*/

bool add_collections( lists &colreg, lists &files )
{
	//1. remove old definition
	lists::iterator beginDocC = 0;
	lists::iterator endDocDocC = 0;
	bool	bRemove = false;

	for( lists::iterator  il = colreg.begin(); il != colreg.end(); il++ )
	{
		const char	*psz = (*il).c_str();

		if( !strcmp( psz, "</DocCompilation>" ) )
		{
			endDocDocC = il;
			if( beginDocC == 0 )
				continue;
			if( bRemove )
			{
				endDocDocC++;
				il = colreg.erase( beginDocC, endDocDocC );
				psz = (*il).c_str();
				bRemove = false;
			}
			endDocDocC = 0;
			beginDocC = 0;
		}
		if( !strcmp( psz, "<DocCompilation>" ) )
			beginDocC = il;
		if( beginDocC == 0 )
			continue;

/******* Maxim [15.05.2002 ] ***********************************************************************/
		if( strstr( psz, "DocCompId" ) )
		{
			char	*pszF = strrchr( psz, '=' );

			if( !pszF )
				continue;

			int nSz = strlen( pszF );
			char ch = *(pszF + nSz - 3);
			*(pszF + strlen( pszF ) - 3) = '\0';
			char	*pszRight = pszF + strlen( pszF ) - strlen( g_szPrefix );

			if( !pszRight )
				continue;

			if( strcmp( pszRight, g_szPrefix ) )
				beginDocC = 0;

			*(pszF + nSz - 3) = ch;
		}

		if( beginDocC == 0 )
			continue;
/******* Maxim [15.05.2002 ] ***********************************************************************/

		if( strstr( psz, "TitleLocation" ) )
		{
			char	*pszF = strrchr( psz, '\\' );
			if( !pszF )continue;
			char	szFileName[_MAX_PATH];
			strcpy( szFileName, pszF+1 );
			szFileName[strlen( szFileName )-3] = 0;

			for( lists::iterator im = files.begin(); im != files.end(); im++ )
			{
				char	*ptest = strrchr( (*im).c_str(), '\\' );
				if( !ptest )continue;
				ptest++;
				if( !stricmp( szFileName, ptest ) )
					bRemove = true;
			}
		}
	}

	if( g_bRemove )
		return true;
		//find <DocCompilations>
	lists::iterator	iInsert = 0;
	for( il = colreg.begin(); il != colreg.end(); il++ )
	{
		if( !strcmp( (*il).c_str(), "<DocCompilations>" ) )
			iInsert = il;
	}

	if( iInsert == 0 )
	{
		_error( "DocCompilations section not found" );
		return false;
	}
	iInsert++;


	for( lists::iterator i = files.begin(); i != files.end(); i++ )
	{
		//process file
		const char	*pszFile = (*i).c_str();
		char	szTitle[_MAX_PATH];
		char	szCHM[_MAX_PATH], szCHI[_MAX_PATH];
		::_splitpath( pszFile, 0, 0, szTitle, 0 );
		strcat( szTitle, g_szPrefix );
		strcpy( szCHM, pszFile );
		strcpy( szCHI, pszFile );

		char	*pd = strrchr( szCHI, '.' );
		if( pd )strcpy( pd, ".chi" );
		char	sz[_MAX_PATH+100];
//<DocCompilation>
		colreg.insert( iInsert, "<DocCompilation>" );
//<DocCompId value="FP"/>
		strcpy( sz, "\t<DocCompId value=\"" );strcat( sz, szTitle ); strcat( sz, "\"/>" );
		colreg.insert( iInsert, sz );
//<DocCompLanguage value=1049/>
		sprintf( sz, "\t<DocCompLanguage value=%s/>", g_szLang );
		colreg.insert( iInsert, sz );
//<LocationHistory>
		colreg.insert( iInsert, "\t<LocationHistory>" );
//<ColNum value=1/>
		colreg.insert( iInsert, "\t\t<ColNum value=1/>" );
//<TitleLocation value="D:\vt5\help\FP.chm"/>
		sprintf( sz, "\t\t<TitleLocation value=\"%s\"/>", szCHM );
		colreg.insert( iInsert, sz );
//<IndexLocation value="D:\vt5\help\FP.chi"/>
		sprintf( sz, "\t\t<IndexLocation value=\"%s\"/>", szCHI );
		colreg.insert( iInsert, sz );
//<QueryLocation value=""/>
		colreg.insert( iInsert, "\t\t<QueryLocation value=\"\"/>" );
//<LocationRef value=""/>
		colreg.insert( iInsert, "\t\t<LocationRef value=\"\"/>" );
//<Version value=29227493/>
		time_t	timet;
		time( &timet );
		tm *ptm = gmtime( &timet );
		sprintf( sz, "\t\t<Version value=%04d%03d%02d%02d/>", ptm->tm_year, ptm->tm_yday, ptm->tm_hour, ptm->tm_min );
		colreg.insert( iInsert, sz );
//<LastPromptedVersion value=0/>
		colreg.insert( iInsert, "\t\t<LastPromptedVersion value=0/>" );
//<TitleSampleLocation value=""/>
		colreg.insert( iInsert, "\t\t<TitleSampleLocation value=\"\"/>" );
//<TitleQueryLocation value=""/>
		colreg.insert( iInsert, "\t\t<TitleQueryLocation value=\"\"/>" );
//<SupportsMerge value=0/>
		colreg.insert( iInsert, "\t\t<SupportsMerge value=0/>" );
//</LocationHistory>
		colreg.insert( iInsert, "\t</LocationHistory>" );
//</DocCompilation>
		colreg.insert( iInsert, "</DocCompilation>" );
	}
	return true;
}

char	g_szExtensions[] = ".chm\0\0";

void walk_dir( const char *pszRoot, lists	&strings )
{
	char	sz[MAX_PATH];
	strcpy( sz, pszRoot );
	strcat( sz, "*.*" );

	WIN32_FIND_DATA	ff;
	HANDLE hFF = 
	::FindFirstFile( sz, &ff );

	bool	bContinue = hFF != INVALID_HANDLE_VALUE;

	while( bContinue )
	{
		if((ff.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0 )
		{
			bool	bAdd = false;

			char	*p = g_szExtensions;
			while( *p )
			{
				if( strstr( ff.cFileName, p ) )
					bAdd = true;
				p+= strlen( p )+1;
			}
			if( bAdd )
			{
				std::string	str = pszRoot;
				str+=ff.cFileName;
				strings.push_back( str );
				/*write_line( pszRoot );
				write_line( ff.cFileName );
				write_line( "\r\n" );	*/
			}
		}
		else
		{
			if( strcmp( ff.cFileName, "." ) && strcmp( ff.cFileName, ".." ) )
			{
				char	sz[MAX_PATH];
				strcpy( sz, pszRoot );
				strcat( sz, ff.cFileName );
				strcat( sz, "\\" );
				walk_dir( sz, strings );
			}
		}

		bContinue = ::FindNextFile( hFF, &ff )!=0;
	}
	::FindClose( hFF );
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{

	char	*pszPrefix = strstr( lpCmdLine, "/prefix:" );
	if( pszPrefix )
	{
		pszPrefix+=8;
		strcpy( g_szPrefix, pszPrefix );
		char	*p = strrchr( g_szPrefix, ' ' );
		if( p )*p = 0;
	}
	char	*pszLang = strstr( lpCmdLine, "/lang:" );
	if( pszLang )
	{
		pszLang+=6;
		strncpy( g_szLang, pszLang, 6 );
		g_szLang[6] = 0;
		char	*p = strrchr( g_szLang, ' ' );
		if( p )*p = 0;
	}

	if( strstr( lpCmdLine, "/remove" ) )
		g_bRemove = true;

	char	szPath[MAX_PATH];szPath[0]=0;

	char	*pszScanPath = strstr( lpCmdLine, "/scan:" );
	if( pszScanPath && strlen(pszScanPath)>6 )
	{
		bool	bFlag = pszScanPath[6] == '"';
		strcpy( szPath, pszScanPath+(bFlag?7:6) );

		
		char	*p = 0;
		if( bFlag )p = strchr( szPath, '"' );
		else p = strchr( szPath, ' ' );
		if( p ) *p = 0;
		if( szPath[strlen(szPath)-1]!='\\' )
			strcat( szPath, "\\" );
	}

	bool	bScan = false;
	

 	char	szColFile[_MAX_PATH];

	::GetWindowsDirectory( szColFile, _MAX_PATH );
	strcat( szColFile, "\\help\\hhcolreg.dat" );

	lists	files;

	if( strlen( szPath ) == 0 )
	{
		if( !load_file( "helpreg.task", files ) )
			return 0;
	}
	else
		walk_dir( szPath, files );
	lists	colreg;
	if( !load_file( szColFile, colreg ) )return 0;
	if( !add_collections( colreg, files ) )return 0;
	if( !backup_file( szColFile ) )return 0;
	if( !store_file( szColFile, colreg ) )return 0;

	return 0;
}



