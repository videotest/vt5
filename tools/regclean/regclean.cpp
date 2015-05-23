// regclean.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "resource.h"
#include "commctrl.h"

#pragma comment(lib, "comctl32.lib" )

HWND	g_hwndProgress;
HWND	g_hwndG, g_hwndT;

struct	RegEntry
{
	char	szKeyNameCLSID[255];
	char	szKeyNameProgID[255];
	char	szFileName[255];
	bool	bDelete;

	RegEntry	*pnext;
};

struct FileEntry
{
	char	szFileName[255];

	FileEntry	*pnext;
};

RegEntry	*g_pRegEntries = 0;
FileEntry	*g_pFileEntries = 0;

void RegDeleteKeyEx( HKEY	hGlobalKey, const char *pszRegKey )
{
	if( !strlen( pszRegKey ) )
		return;
	int	idx= 0;

	HKEY	hKey;
	if( ::RegOpenKey( hGlobalKey, pszRegKey, &hKey ) == 0 )
	{
		DWORD	nSubKeysCount = 0;
		RegQueryInfoKey( hKey, 0, 0, 0, &nSubKeysCount, 0, 0, 0, 0, 0, 0, 0 );

		if( nSubKeysCount )
		{
			int	idx = nSubKeysCount;
			idx--;
			char	szKeyName[255];
			DWORD	dwSize;

			while( idx >= 0 )
			{
				dwSize = 255;
				::RegEnumKey( hKey, idx, szKeyName, dwSize );

				char	szFull[255];
				strcpy( szFull, pszRegKey );
				strcat( szFull, "\\" );
				strcat( szFull, szKeyName );
				::RegDeleteKeyEx( hGlobalKey, szFull );

				idx--;
			}
		}
		::RegCloseKey( hKey );
	}

	::RegDeleteKey( hGlobalKey, pszRegKey );
}

bool CheckFile( const char *pszFileName )
{
	if( !strlen( pszFileName ) )
		return false;
	FileEntry	*pf = g_pFileEntries;

	char	*psz = strrchr( (char*)pszFileName, '\\' );
	if( !psz )return false;
	psz++;

	for( char *p = psz; *p; p++ )
		if( *p >= 'A' && *p <= 'Z' )
			*p = *p-'A'+'a';


	while( pf )
	{
		if( !strncmp( pf->szFileName, psz, 6 ) )
			return true;
		pf = pf->pnext;
	}
	return false;
}


bool ScanRegistry( )
{
	HKEY	hKeyClasses = 0;
	if( ::RegOpenKey( HKEY_CLASSES_ROOT, "CLSID", &hKeyClasses ) != 0 )
		return false;
	

	DWORD	nSubKeysCount = 0;
	RegQueryInfoKey( hKeyClasses, 0, 0, 0, &nSubKeysCount, 0, 0, 0, 0, 0, 0, 0 );
	int	idx = 0;


	while( true )
	{
		char	szKeyName[255];
		DWORD	dwSize = 255;
		if( ::RegEnumKey( hKeyClasses, idx++, szKeyName, dwSize ) != 0 )
			break;

		RegEntry	*pe = new RegEntry;

		strcpy( pe->szKeyNameCLSID, "CLSID\\" );
		strcat( pe->szKeyNameCLSID, szKeyName );
		pe->szKeyNameProgID[0] = 0;
		pe->szFileName[0] = 0;
		pe->bDelete = false;

		HKEY	hKeyServer = 0;
		char	szInprocServer[255];
		strcpy( szInprocServer, pe->szKeyNameCLSID );
		strcat( szInprocServer, "\\InprocServer32" );

		DWORD	dwType;
		
		if( ::RegOpenKey( HKEY_CLASSES_ROOT, szInprocServer, &hKeyServer ) == 0 )
		{
			dwSize = 255;
			dwType = REG_SZ;
			::RegQueryValueEx( hKeyServer, 0, 0, &dwType, (BYTE*)pe->szFileName, &dwSize );
			::RegCloseKey( hKeyServer );
		}

		if( !CheckFile( pe->szFileName ) )
		{
			delete pe;
			continue;
		}

		HKEY	hKeyProgID = 0;
		char	szProgID[255];
		strcpy( szProgID, pe->szKeyNameCLSID );
		strcat( szProgID, "\\ProgID" );

		if( ::RegOpenKey( HKEY_CLASSES_ROOT, szProgID, &hKeyProgID ) == 0 )
		{
			dwSize = 255;
			dwType = REG_SZ;
			::RegQueryValueEx( hKeyServer, 0, 0, &dwType, (BYTE*)pe->szKeyNameProgID, &dwSize );
			::RegCloseKey( hKeyServer );
		}


		pe->pnext = g_pRegEntries;
		g_pRegEntries = pe;

		if( (idx % 100) == 0 )
		{
			double	fVal = 100.*idx/nSubKeysCount;
			::SendMessage( g_hwndG, PBM_SETPOS, (LONG)fVal, 0 );
		}

	}

	::RegCloseKey( hKeyClasses );

	return true;
}

void ScanFile( const char *pszDirToScan, const char *pszExt )
{
	char	sz[255];
	strcpy( sz, pszDirToScan );
	strcat( sz, "\\" );
	strcat( sz, pszExt );

	WIN32_FIND_DATA	ff;
	HANDLE	hFF = ::FindFirstFile( sz, &ff );

	char	*psz = strrchr( sz, '\\' );
	*(psz+1) = 0;

	while( hFF != INVALID_HANDLE_VALUE )
	{
		FileEntry	*pf = new FileEntry;
		pf->pnext = g_pFileEntries;
		g_pFileEntries = pf;
		
		//strcpy( pf->szFileName, sz );
		strcpy( pf->szFileName, ff.cFileName );

		for( char *p = pf->szFileName; *p; p++ )
		if( *p >= 'A' && *p <= 'Z' )
			*p = *p-'A'+'a';

		if( !::FindNextFile( hFF, &ff ) )
			break;
	}

	::FindClose( hFF );
}


void RemoveFromRegistry()
{
	RegEntry	*pe = g_pRegEntries;

	while( pe )
	{
		::RegDeleteKeyEx( HKEY_CLASSES_ROOT, pe->szKeyNameCLSID );
		::RegDeleteKeyEx( HKEY_CLASSES_ROOT, pe->szKeyNameProgID );
		pe = pe->pnext;
	}
}

int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	InitCommonControls();
	g_hwndProgress = ::CreateDialog( hInstance, MAKEINTRESOURCE(IDD_PROGRESS), 0, 0 );
	::UpdateWindow( g_hwndProgress );

	g_hwndT = ::GetDlgItem( g_hwndProgress, IDC_TEXT );
	g_hwndG= ::GetDlgItem( g_hwndProgress, IDC_PROGRESS );
	::SendMessage( g_hwndG, PBM_SETRANGE, 0, MAKELPARAM( 0, 100 ) );

 	char	szDir[255];

	::GetModuleFileName( hInstance, szDir, 255 );
	::GetLongPathName( szDir, szDir, 255 ) ;
	char	*psz = ::strrchr( szDir, '\\' );
	*psz = 0;

	SetWindowText( g_hwndT, "Scaning files..." );

	::ScanFile( szDir, "shell.exe" );
	::ScanFile( szDir, "comps\\*.ocx" );
	::ScanFile( szDir, "comps\\*.dll" );

	SetWindowText( g_hwndT, "Scaning registry..." );
	
	::ScanRegistry();

	SetWindowText( g_hwndT, "Removing from registry..." );

	::RemoveFromRegistry();

	SetWindowText( g_hwndT, "OK" );

	::EndDialog( g_hwndProgress, 1 );

	return 0;
}



