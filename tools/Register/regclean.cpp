#include "stdafx.h"
#include "resource.h"
#include "RegisterDlg.h"

HWND	g_hwndProgress;
HWND	g_hwndG, g_hwndT;

void WriteLogLine( const char *lpszFormat, ... );

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
char g_strDir[MAX_PATH];
char g_strDir1[MAX_PATH];

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

//* Is file in ./ or in ./comps directory
bool CheckFile( const char *pszFileName )
{
	if( !strlen( pszFileName ) )
		return false;

	FileEntry	*pf = g_pFileEntries;

	char	*psz = strrchr( (char*)pszFileName, '\\' );
	if( !psz )
		return false;
	psz++;

	for( char *p = psz; *p; p++ )
		if( *p >= 'A' && *p <= 'Z' )
			*p = *p-'A'+'a';

	size_t nSz = strlen( pszFileName ) - strlen( psz ) - 1;
	
	if( nSz <= 0 )
		return false;

	char *pPath = new char[nSz + 1];

	if( !pPath )
		return false;

	strncpy( pPath, pszFileName, nSz );
	pPath[nSz] = 0;

	for(  p = pPath; *p; p++ )
		if( *p >= 'A' && *p <= 'Z' )
			*p = *p-'A'+'a';

	if( strlen( g_strDir ) == nSz || strlen( g_strDir1 ) == nSz )
	{
		if( !strncmp( pPath, g_strDir, nSz ) || !strncmp( pPath, g_strDir1, nSz ) )
		{
			while( pf )
			{
				if( !strncmp( pf->szFileName, psz, 6 ) )
				{
					delete []pPath;
					return true;
				}
				pf = pf->pnext;
			}
		}
	}

	delete []pPath;
	return false;
}


bool ScanRegistry( char* szDir)
{
	strcpy (g_strDir,szDir);
	strcpy (g_strDir1,szDir);
	
	strcat( g_strDir1, "\\comps" );

	for( char *p = g_strDir; *p; p++ )
		if( *p >= 'A' && *p <= 'Z' )
			*p = *p-'A'+'a';

	for( p = g_strDir1; *p; p++ )
		if( *p >= 'A' && *p <= 'Z' )
			*p = *p-'A'+'a';

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
			::RegQueryValueEx( hKeyProgID, 0, 0, &dwType, (BYTE*)pe->szKeyNameProgID, &dwSize );
			::RegCloseKey( hKeyProgID );
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
	while( g_pRegEntries )
	{
		RegEntry	*pe = g_pRegEntries;
		if(0!=*pe->szKeyNameCLSID){
			RegDeleteKeyEx( HKEY_CLASSES_ROOT, pe->szKeyNameCLSID);
				WriteLogLine( "   delete CLSID entry %s", pe->szKeyNameCLSID );
		}
		if(0!=*pe->szKeyNameProgID)
		{
			RegDeleteKeyEx( HKEY_CLASSES_ROOT, pe->szKeyNameProgID);
				WriteLogLine( "   delete ProgID entry %s", pe->szKeyNameProgID );
		}
		g_pRegEntries = pe->pnext;
		delete pe;
	}
}

void CRegisterDlg::RegClean(char* szDir)
{
	WriteLogLine( "Clearing registry..." );
	g_hwndProgress = ::CreateDialog( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDD_PROGRESS), 0, 0 );
	::UpdateWindow( g_hwndProgress );

	g_hwndT = ::GetDlgItem( g_hwndProgress, IDC_TEXT );
	g_hwndG= ::GetDlgItem( g_hwndProgress, IDC_PROGRESS );
	::SendMessage( g_hwndG, PBM_SETRANGE, 0, MAKELPARAM( 0, 100 ) );

	char *psz = ::strrchr( szDir, '\\' );
	*psz = 0;

	::SetWindowText( g_hwndT, "Scaning files..." );
	WriteLogLine( "Scaning files..." );

	::ScanFile( szDir, "shell.exe" );
	::ScanFile( szDir, "comps\\*.ocx" );
	::ScanFile( szDir, "comps\\*.dll" );

	::SetWindowText( g_hwndT, "Scaning registry..." );
	WriteLogLine( "Scaning registry..." );
	
	::ScanRegistry(szDir);

	::SetWindowText( g_hwndT, "Removing from registry..." );
	WriteLogLine( "Removing from registry..." );

	::RemoveFromRegistry();

	::SetWindowText( g_hwndT, "OK" );

	::EndDialog( g_hwndProgress, 1 );
	WriteLogLine( "Clearing done" );
}

bool ServerContains(char* server, const CString& szDir, RegEntry	*pe)
{
	HKEY	hKeyServer = 0;
	char	szServer[255];

	DWORD	dwType;

	strcpy(szServer,pe->szKeyNameCLSID);
	strcat(szServer,server);
	if(ERROR_SUCCESS == ::RegOpenKey(HKEY_CLASSES_ROOT, szServer, &hKeyServer))
	{
		DWORD dwSize = 255;
		dwType = REG_SZ;
		::RegQueryValueEx( hKeyServer, 0, 0, &dwType, (BYTE*)pe->szFileName, &dwSize );
		::RegCloseKey( hKeyServer );

		strcpy( szServer, pe->szKeyNameCLSID );
		strcat( szServer, "\\InprocServer32" );
		return !_strnicmp( &pe->szFileName[0],szDir,szDir.GetLength() );
	}
	return false;
}

bool recursSearch(CString keyPath, const CString& szDir, RegEntry *pe)
{
	bool bFound=false;
	HKEY	hKey = 0;

	if(ERROR_SUCCESS==::RegOpenKey( HKEY_CLASSES_ROOT, keyPath, &hKey))
	{
		DWORD nSubKeysCount=0;
		{
			TCHAR szKeyName[255];
			bool bFoundAny=false;
			int nFound=0;
			CString keyFound;
			::RegQueryInfoKey( hKey, 0, 0, 0, &nSubKeysCount, 0, 0, 0, 0, 0, 0, 0 );
			int	idx = 0;
			for( ;ERROR_SUCCESS == RegEnumKey( hKey, idx, szKeyName, 255 ); ++idx)
			{
				bFoundAny = recursSearch(keyPath+"\\"+szKeyName,szDir,pe);
				if(bFoundAny)
					++nFound;
			}
			if(nFound>0 && idx==nFound)
			{
				pe->bDelete=true;
			}
		}
		{
			DWORD dwSize = 255;	
			BYTE szValue[255];
			DWORD	dwType = REG_SZ;

			if(ERROR_SUCCESS==::RegQueryValueEx( hKey, 0, 0, &dwType, szValue, &dwSize ))
			{
				if(REG_SZ==dwType)
				{
					bFound = !_strnicmp( (LPTSTR)szValue,szDir,szDir.GetLength() );
					if(bFound){
						strcpy(pe->szKeyNameCLSID,keyPath);
					}
				}
			}
		}
		::RegCloseKey( hKey );
	}
	return bFound;
}

bool DirScanRegistry(const CString& szDir)
{
	size_t lDir=strlen(szDir);

	HKEY	hKeyClasses = 0;
	if( ::RegOpenKey( HKEY_CLASSES_ROOT, "CLSID", &hKeyClasses ) != 0 )
		return false;

	char	szKeyName[255];
	DWORD	dwSize = 255;
	DWORD	nSubKeysCount = 0;
	RegQueryInfoKey( hKeyClasses, 0, 0, 0, &nSubKeysCount, 0, 0, 0, 0, 0, 0, 0 );
	for(int	idx = 0; ERROR_SUCCESS == RegEnumKey( hKeyClasses, idx, szKeyName, dwSize ); ++idx)
	{
		RegEntry re;

		if(!strnicmp(szKeyName,"{511ED0B9-BAB5-47A3-A497-3D3789E6CFE0}",3))
		{
			"{511ED0B9-BAB5-47A3-A497-3D3789E6CFE0}";	
			strcpy( re.szKeyNameCLSID, "CLSID\\" );
		}
		strcpy( re.szKeyNameCLSID, "CLSID\\" );
		strcat( re.szKeyNameCLSID, szKeyName );
		re.szKeyNameProgID[0] = 0;
		re.szFileName[0] = 0;
		re.bDelete = false;
		if( ServerContains("\\InprocServer32", szDir, &re )
				|| ServerContains("\\LocalServer32", szDir, &re))
		{
			HKEY	hKeyProgID = 0;
			char	szProgID[255];
			DWORD	dwType = REG_SZ;
			RegEntry *pe = new RegEntry;

			*pe=re;
			strcpy( szProgID, pe->szKeyNameCLSID );
			strcat( szProgID, "\\ProgID" );

			if( ::RegOpenKey( HKEY_CLASSES_ROOT, szProgID, &hKeyProgID ) == 0 )
			{
				dwSize = 255;
				::RegQueryValueEx( hKeyProgID, 0, 0, &dwType, (BYTE*)pe->szKeyNameProgID, &dwSize );
				::RegCloseKey( hKeyProgID );
			}
			pe->pnext = g_pRegEntries;
			g_pRegEntries = pe;
		}
		if( (idx % 100) == 0 )
		{
			::SendMessage( g_hwndG, PBM_SETPOS, 100*idx/nSubKeysCount, 0 );
		}
	}
	::RegCloseKey( hKeyClasses );

	{
		TCHAR szLibUid[255];
		CString keyPath ="TypeLib";
		if( ::RegOpenKey( HKEY_CLASSES_ROOT, "TypeLib", &hKeyClasses ) != 0 )
			return false;
		RegQueryInfoKey( hKeyClasses, 0, 0, 0, &nSubKeysCount, 0, 0, 0, 0, 0, 0, 0 );
		for(int	idx = 0; ERROR_SUCCESS == RegEnumKey( hKeyClasses, idx, szLibUid, sizeof(szLibUid) ); ++idx)
		{
			RegEntry re={""};
			CString sLibUid = keyPath + CString("\\") + szLibUid;
			recursSearch(sLibUid,szDir,&re);
			if(re.szKeyNameCLSID[0])
			{
				RegEntry* pe=new RegEntry(re);
				if(re.bDelete)
				{
					strcpy(pe->szKeyNameCLSID,sLibUid);
				}
				pe->pnext = g_pRegEntries;
				g_pRegEntries = pe;
			}
		}	
	}

	return true;
}

// Clean Registry in CLSID where refence to location dir
void CRegisterDlg::RegCleanLoc(const char* szLocDir)
{
	WriteLogLine( "Clearing registry..." );
	g_hwndProgress = ::CreateDialog( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDD_PROGRESS), 0, 0 );
	::UpdateWindow( g_hwndProgress );

	g_hwndT = ::GetDlgItem( g_hwndProgress, IDC_TEXT );
	g_hwndG= ::GetDlgItem( g_hwndProgress, IDC_PROGRESS );
	::SendMessage( g_hwndG, PBM_SETRANGE, 0, MAKELPARAM( 0, 100 ) );

	::SetWindowText( g_hwndT, "Scaning registry..." );
	WriteLogLine( "Scaning registry..." );

	::DirScanRegistry(szLocDir);

	::SetWindowText( g_hwndT, "Removing from registry..." );
	WriteLogLine( "Removing from registry..." );

	::RemoveFromRegistry();

	::SetWindowText( g_hwndT, "OK" );

	::EndDialog( g_hwndProgress, 1 );
	WriteLogLine( "Clearing done" );
}
