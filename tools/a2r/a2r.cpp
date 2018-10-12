// a2r.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#pragma warning(disable:4786)
#include "map"
#include "string"
#include "list"

using namespace std;

typedef	map<string, string>	maps2s;
typedef	list<string>		lists;

maps2s	aliases;

void _error( const char *psz )
{	MessageBox( 0, psz, "Aliase to Resource Mapper", MB_OK );	}

bool load_aliases( const char *psz )
{
	FILE	*pfile = ::fopen( psz, "rt" );

	if( !pfile )
	{
		char	sz[280];
		sprintf( sz, "Can't open aliases file\n%s", psz );
		_error( sz );
		return false;
	}

	char	sz[255];
	bool	bRead = false;

	while( ::fgets( sz, 255, pfile ) )
	{
		{
			char	*psz = strrchr( sz, '\n' );
			if( psz )*psz = 0;
		}
		
		if( !bRead )
		{
			if( !strcmp( sz, "[Actions]" ) )
				bRead =true;
			continue;
		}
		
		if( !strlen( sz ) || sz[0] == '[' )
		{
			bRead = false;
			continue;
		}


		char	*pszS = strchr( sz, '=' ), *pszA = sz;
		if( !pszS )continue;
		*pszS = 0;pszS++;

		char	szAliase[512];
		for( char *p = pszS, *pa = szAliase; *p; p++, pa++ )
		{
			if( *p == '#' )
			{
				*pa = '\\';pa++;*pa='n';
			}
			else if( *p != '\n' )
			{
				*pa = *p;
			}
		}
		*pa = 0;
		aliases[pszA] = szAliase;
	}
	::fclose( pfile );

	return true;
}

bool load_file( const char *pszFile, lists	&resources )
{
	FILE	*pfile = ::fopen( pszFile, "rt" );
	if( !pfile )
	{
		char	sz[280];
		sprintf( sz, "Can't open resource file\n%s", pszFile );
		_error( sz );
		return false;
	}

	resources.clear();
	char	sz[255];
	while( ::fgets( sz, 255, pfile ) )
	{
		char	*p = strchr( sz, '\n' );
		if( p )*p = 0;
		resources.push_back( sz );
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

bool store_file( const char *pszFile, lists	&resources )
{
	FILE	*pfile = ::fopen( pszFile, "wt" );
	if( !pfile )
	{
		char	sz[280];
		sprintf( sz, "Can't store resource file\n%s", pszFile );
		_error( sz );
		return false;
	}


	lists::iterator	rs = resources.begin();

	while( rs != resources.end() )
	{
		const char	*psz = (*rs).c_str();
		fputs( psz, pfile );
		fputs( "\n", pfile );
		rs++;
	}

	::fclose( pfile );
	return true;
}

bool replace_aliases( lists	&resoucers )
{
	bool	bStringTable = false;
	bool	bWaitForStringTable = false;
	bool	bWaitingForString = false;

	for( lists::iterator rs = resoucers.begin(); rs != resoucers.end(); rs++ )
	{
		char	*psz = (char*)(*rs).c_str();
		if( !bStringTable && !bWaitForStringTable )
		{
			if( !strcmp( psz, "STRINGTABLE DISCARDABLE " ) )
				bWaitForStringTable = true;
			continue;
		}
		if( bWaitForStringTable )
			if( !strcmp( psz, "BEGIN" ) )
			{
				bWaitForStringTable = false;
				bStringTable = true;
				continue;
			}
		if( !strcmp( psz, "END" ) )
		{
			bWaitForStringTable = false;
			bStringTable = false;
			continue;
		}


		//here string table
		if( bStringTable )
		{
			char	*pszB, *pszE;
			pszB = strchr( psz, '\"' );
			pszE = strrchr( psz, '\"' );
			bool	bWriteIDS = true;

			if( bWaitingForString )
			{
				bWriteIDS = false;
				bWaitingForString = false;
				if( !pszB || !pszE )continue;
			}

			if( !pszB || !pszE )
			{
				bWaitingForString = true;
				continue;
			}


			char	sz[512];
			strncpy( sz, pszB+1, (long)pszE-(long)pszB-2 );
			
			char	*pszAS = strstr( sz, "\\n" );
			if( !pszAS )continue;
			
			*pszAS = 0;
			
			maps2s::iterator	rm = aliases.find( sz );
			if( rm != aliases.end() )
			{
				*pszB = 0;
				const char	*pszA=(*rm).second.c_str();

				char	sz1[512];

				if( bWriteIDS )
				{
					strcpy( sz1, psz );
					strcat( sz1, "\"" );
					strcat( sz1, sz );
				}
				else
				{
					strcpy( sz1, "\"" );
					strcat( sz1, sz );
				}
				
				strcat( sz1, "\\n" );
				strcat( sz1, pszA );
				strcat( sz1, "\"" );

				(*rs) = sz1;
			}
		}
	}
	return true;
}

bool process_resource( const char *psz )
{
	lists	resources;
	
	if( !load_file( psz, resources ) )
		return false;
	if( !replace_aliases( resources ) )
		return false;
	if( !backup_file( psz ) )
		return false;
	if( !store_file( psz, resources ) )
		return false;

	return true;
}



int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
 	//1. read aliase file and store it to map
	if( !load_aliases( "shell.aliases" ) )
		return 0;

	lists	files;

	if( !load_file( "a2r.task", files ) )
		return 0;

	for( lists::iterator rf = files.begin(); rf != files.end(); rf++ )
		process_resource( (*rf).c_str() );

	return 0;
}



