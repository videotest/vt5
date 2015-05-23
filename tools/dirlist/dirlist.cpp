// dirlist.cpp : Defines the entry point for the application.
//

#include "stdafx.h"


char	szOutFileName[MAX_PATH];
char	szFileToFind[MAX_PATH];
char	szRootPath[MAX_PATH];
char	g_szExtensions[1024];

bool write_line( const char *psz )
{
	HANDLE	hFile = ::CreateFile( szOutFileName, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );
	if( hFile == INVALID_HANDLE_VALUE )return false;

	DWORD	dwSize = strlen( psz );
	::SetFilePointer( hFile, 0, 0, FILE_END );
	::WriteFile( hFile, psz, dwSize, &dwSize, 0 );
	::CloseHandle( hFile );
	return true;
}

bool create_relative_path( char *pszto, const char *psz )
{
	GetModuleFileName( 0, pszto, MAX_PATH );
	char *p = strrchr( pszto, '\\' );
	p++;
	if( psz )
		strcpy( p, psz );
	else
		*p = 0;
	return true;
}

char *extract_path( char *psz, char *pszto, bool bAddRoot )
{
	char chEnd;
	if( !psz )return 0;
	if( *psz == '"' )
	{
		psz++;
		chEnd = '"';
	}
	else 
		chEnd = ' ';
	char *pend = strchr( psz, chEnd );
	long	cb = pend?((long)pend-(long)psz):strlen( psz );

	if( cb == 0 )return 0;
	
	char	sz[MAX_PATH];
	strncpy( sz, psz, cb );
	sz[cb] = 0;
	if( strchr( sz, '\\' ) || !bAddRoot )
		strcpy( pszto, sz );
	else
	{
		create_relative_path( pszto, sz );
	}
	return psz+cb;
}

void walk_dir( const char *pszRoot )
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
				write_line( pszRoot );
				write_line( ff.cFileName );
				write_line( "\r\n" );	
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
				walk_dir( sz );
			}
		}

		bContinue = ::FindNextFile( hFF, &ff )!=0;
	}
	::FindClose( hFF );
}

void _message( const char *psz )
{
	::MessageBox( 0, psz, "File List", 0 );
}



void parse_extension()
{
	char	szSep[] = "*;";
	char	*ps = strtok( szFileToFind, szSep );
	char	*pd = g_szExtensions;
	while( ps )
	{
		strcpy( pd, ps );
		pd+=strlen( ps )+1;
		ps = strtok( 0, szSep );
	}
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	if( strlen( lpCmdLine ) == 0 )
	{
		_message( "Ussage:\n /p:<path_to_scan> /o:<output_filename> /f:<file_to_find>" );
		return 0;
	}

	char	*p = strstr( lpCmdLine, "/p:" );
	if( p )	extract_path( p+3, szRootPath, true );
	else	create_relative_path( szRootPath, 0 );

	if( szRootPath[strlen(szRootPath)-1] != '\\' )
		strcat( szRootPath, "\\" );

	p = strstr( lpCmdLine, "/o:" );
	if( p ) extract_path( p+3, szOutFileName, true );
	else create_relative_path( szOutFileName, "list.dir" );

	p = strstr( lpCmdLine, "/f:" );
	if( p ) extract_path( p+3, szFileToFind, false );
	else strcpy( szFileToFind, "*.*" );

	parse_extension();

	::DeleteFile( szOutFileName );

	walk_dir( szRootPath );

	return 0;
}



