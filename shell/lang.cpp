#include "stdafx.h"
#include "tlhelp32.h"

HMODULE (WINAPI *g_pLoadLibraryA)(LPCSTR) = 0;
int (WINAPI *g_pMultiByteToWideChar)(UINT,DWORD,LPCSTR,int,LPWSTR,int) = 0;


/*//////////////////////////////////////////////////////////////////////
                        File Specific Prototypes
//////////////////////////////////////////////////////////////////////*/
typedef struct tag_HOOKFUNCDESCA
{
    // The name of the function to hook.
    LPCSTR	szFunc;
    // The procedure to blast in.
    PROC	pProc;
} HOOKFUNCDESCA , * LPHOOKFUNCDESCA ;

typedef struct tag_HOOKFUNCDESCW
{
    // The name of the function to hook.
    LPCWSTR szFunc   ;
    // The procedure to blast in.
    PROC	pProc;
} HOOKFUNCDESCW , * LPHOOKFUNCDESCW ;

#ifdef UNICODE
#define HOOKFUNCDESC   HOOKFUNCDESCW
#define LPHOOKFUNCDESC LPHOOKFUNCDESCW
#else
#define HOOKFUNCDESC   HOOKFUNCDESCA
#define LPHOOKFUNCDESC LPHOOKFUNCDESCA
#endif  // UNICODE


// A useful macro.
#define MakePtr( cast , ptr , AddValue ) \
                                 (cast)( (DWORD)(ptr)+(DWORD)(AddValue))
#ifdef _DEBUG
#define _assert(b)	if( !(b) )MessageBox( 0, #b, "_assertion!", MB_OK )
#define _trace(s)	OutputDebugString( #s )
#else
#define _assert(b)
#define _trace(s)
#endif//
#define _verify(b)	if( !(b) )MessageBox( 0, #b, "_verify!", MB_OK )





/*//////////////////////////////////////////////////////////////////////
                             Implementation
//////////////////////////////////////////////////////////////////////*/

/*----------------------------------------------------------------------
FUNCTION        :   GetNamedImportDescriptor
DISCUSSION      :
    Gets the import descriptor for the requested module.  If the module
is not imported in hModule, NULL is returned.
    This is a potential useful function in the future.
PARAMETERS      :
    hModule      - The module to hook in.
    szImportMod  - The module name to get the import descriptor for.
RETURNS         :
    NULL  - The module was not imported or hModule is invalid.
    !NULL - The import descriptor.
----------------------------------------------------------------------*/
PIMAGE_IMPORT_DESCRIPTOR
                     GetNamedImportDescriptor ( HMODULE hModule     ,
                                                LPCSTR  szImportMod  )
{
    // Always check parameters.
    _assert ( NULL != szImportMod ) ;
    _assert ( NULL != hModule     ) ;
    if ( ( NULL == szImportMod ) || ( NULL == hModule ) )
    {
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( NULL ) ;
    }

    PIMAGE_DOS_HEADER pDOSHeader = (PIMAGE_DOS_HEADER)hModule ;

    // Is this the MZ header?
    if ( ( TRUE == IsBadReadPtr ( pDOSHeader                  ,
                                 sizeof ( IMAGE_DOS_HEADER )  ) ) ||
         ( IMAGE_DOS_SIGNATURE != pDOSHeader->e_magic           )   )
    {
        _assert ( FALSE ) ;
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( NULL ) ;
    }

    // Get the PE header.
    PIMAGE_NT_HEADERS pNTHeader = MakePtr ( PIMAGE_NT_HEADERS       ,
                                            pDOSHeader              ,
                                            pDOSHeader->e_lfanew     ) ;

    // Is this a real PE image?
    if ( ( TRUE == IsBadReadPtr ( pNTHeader ,
                                  sizeof ( IMAGE_NT_HEADERS ) ) ) ||
         ( IMAGE_NT_SIGNATURE != pNTHeader->Signature           )   )
    {
        _assert ( FALSE ) ;
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( NULL ) ;
    }

    // If there is no imports section, leave now.
    if ( 0 == pNTHeader->OptionalHeader.
                         DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].
                                                      VirtualAddress   )
    {
        return ( NULL ) ;
    }

    // Get the pointer to the imports section.
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc
     = MakePtr ( PIMAGE_IMPORT_DESCRIPTOR ,
                 pDOSHeader               ,
                 pNTHeader->OptionalHeader.
                         DataDirectory[ IMAGE_DIRECTORY_ENTRY_IMPORT ].
                                                      VirtualAddress ) ;

    // Loop through the import module descriptors looking for the
    //  module whose name matches szImportMod.
    while ( NULL != pImportDesc->Name )
    {
        PSTR szCurrMod = MakePtr ( PSTR              ,
                                   pDOSHeader        ,
                                   pImportDesc->Name  ) ;
        if ( 0 == stricmp ( szCurrMod , szImportMod ) )
        {
            // Found it.
            break ;
        }
        // Look at the next one.
        pImportDesc++ ;
    }

    // If the name is NULL, then the module is not imported.
    if ( NULL == pImportDesc->Name )
    {
        return ( NULL ) ;
    }

    // All OK, Jumpmaster!
    return ( pImportDesc ) ;

}


static BOOL g_bHasVersion = FALSE ;
// Indicates NT or 95/98.
static BOOL g_bIsNT = TRUE ;

BOOL __stdcall IsNT ( void )
{
    if ( TRUE == g_bHasVersion )
    {
        return ( TRUE == g_bIsNT ) ;
    }

    OSVERSIONINFO stOSVI ;

    memset ( &stOSVI , NULL , sizeof ( OSVERSIONINFO ) ) ;
    stOSVI.dwOSVersionInfoSize = sizeof ( OSVERSIONINFO ) ;

    BOOL bRet = GetVersionEx ( &stOSVI ) ;
    _assert ( TRUE == bRet ) ;
    if ( FALSE == bRet )
    {
        _trace ( "GetVersionEx failed!\n" ) ;
        return ( FALSE ) ;
    }

    // Check the version and call the appropriate thing.
    if ( VER_PLATFORM_WIN32_NT == stOSVI.dwPlatformId )
    {
        g_bIsNT = TRUE ;
    }
    else
    {
        g_bIsNT = FALSE ;
    }
    g_bHasVersion = TRUE ;
    return ( TRUE == g_bIsNT ) ;
}



BOOL __stdcall HookImportedFunctionsByName ( HMODULE         hModule     ,
                                      LPCSTR          szImportMod ,
                                      UINT            uiCount     ,
                                      LPHOOKFUNCDESCA paHookArray ,
                                      LPDWORD         pdwHooked    )
{
    // Double check the parameters.
    _assert ( NULL != szImportMod ) ;
    _assert ( 0 != uiCount ) ;
    _assert ( FALSE == IsBadReadPtr ( paHookArray ,
                                     sizeof (HOOKFUNCDESC) * uiCount ));
#ifdef _DEBUG
    if ( NULL != pdwHooked )
    {
        _assert ( FALSE == IsBadWritePtr ( pdwHooked , sizeof ( UINT )));
    }

    // Check each function name in the hook array.
    {
        for ( UINT i = 0 ; i < uiCount ; i++ )
        {
            _assert ( NULL != paHookArray[ i ].szFunc  ) ;
            _assert ( '\0' != *paHookArray[ i ].szFunc ) ;
            // If the proc is not NULL, then it is checked.
            if ( NULL != paHookArray[ i ].pProc )
            {
                _assert ( FALSE == IsBadCodePtr ( paHookArray[i].pProc));
            }
        }
    }
#endif
    // Do the parameter validation for real.
    if ( ( 0    == uiCount      )                                 ||
         ( NULL == szImportMod  )                                 ||
         ( TRUE == IsBadReadPtr ( paHookArray ,
                                  sizeof (HOOKFUNCDESC) * uiCount ) ) )
    {
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( FALSE ) ;
    }
    if ( ( NULL != pdwHooked )                                    &&
         ( TRUE == IsBadWritePtr ( pdwHooked , sizeof ( UINT ) ) )  )
    {
        SetLastErrorEx ( ERROR_INVALID_PARAMETER , SLE_ERROR ) ;
        return ( FALSE ) ;
    }

    // Is this a system DLL, which Windows95 will not let you patch
    //  since it is above the 2GB line?
    if ( ( FALSE == IsNT ( ) ) && ( (DWORD)hModule >= 0x80000000 ) )
    {
        SetLastErrorEx ( ERROR_INVALID_HANDLE , SLE_ERROR ) ;
        return ( FALSE ) ;
    }


    // TODO TODO
    //  Should each item in the hook array be checked in release builds?
    if ( NULL != pdwHooked )
    {
        // Set the number of functions hooked to zero.
        *pdwHooked = 0 ;
    }

    // Get the specific import descriptor.
    PIMAGE_IMPORT_DESCRIPTOR pImportDesc =
                     GetNamedImportDescriptor ( hModule , szImportMod );
    if ( NULL == pImportDesc )
    {
        // The requested module was not imported.  This is not an error.
        return ( TRUE ) ;
    }

    // Get the original thunk information for this DLL.  I cannot use
    //  the thunk information stored in the pImportDesc->FirstThunk
    //  because the that is the array that the loader has already
   //  bashed to fix up all the imports.  This pointer gives us acess
    //  to the function names.
    PIMAGE_THUNK_DATA pOrigThunk =
                        MakePtr ( PIMAGE_THUNK_DATA       ,
                                  hModule                 ,
                                  pImportDesc->OriginalFirstThunk  ) ;
    // Get the array pointed to by the pImportDesc->FirstThunk.  This is
    //  where I will do the actual bash.
    PIMAGE_THUNK_DATA pRealThunk = MakePtr ( PIMAGE_THUNK_DATA       ,
                                             hModule                 ,
                                             pImportDesc->FirstThunk  );

    // Loop through and look for the one that matches the name.
    while ( NULL != pOrigThunk->u1.Function )
    {
        // Only look at those that are imported by name, not ordinal.
        if (  IMAGE_ORDINAL_FLAG !=
                        ( pOrigThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG ))
        {
            // Look get the name of this imported function.
            PIMAGE_IMPORT_BY_NAME pByName ;

            pByName = MakePtr ( PIMAGE_IMPORT_BY_NAME    ,
                                hModule                  ,
                                pOrigThunk->u1.AddressOfData  ) ;

            // If the name starts with NULL, then just skip out now.
            if ( '\0' == pByName->Name[ 0 ] )
            {
				//paul 6.09.2001. Kill line:
				//continue ; - dead while
				//change to:
				break;
				
            }

            // Determines if we do the hook.
            BOOL bDoHook = FALSE ;

            // TODO TODO
            //  Might want to consider bsearch here.

            // See if the particular function name is in the import
            //  list.  It might be good to consider requiring the
            //  paHookArray to be in sorted order so bsearch could be
            //  used so the lookup will be faster.  However, the size of
            //  uiCount coming into this function should be rather
            //  small but it is called for each function imported by
            //  szImportMod.
            for ( UINT i = 0 ; i < uiCount ; i++ )
            {
                if ( ( paHookArray[i].szFunc[0] ==
                                                pByName->Name[0] ) &&
                     ( 0 == strcmpi ( paHookArray[i].szFunc ,
                                      (char*)pByName->Name   )   )    )
                {
                    // If the proc is NULL, kick out, otherwise go
                    //  ahead and hook it.
                    if ( NULL != paHookArray[ i ].pProc )
                    {
                        bDoHook = TRUE ;
                    }
                    break ;
                }
            }

            if ( TRUE == bDoHook )
            {
                // I found it.  Now I need to change the protection to
                //  writable before I do the blast.  Note that I am now
                //  blasting into the real thunk area!
                MEMORY_BASIC_INFORMATION mbi_thunk ;

                VirtualQuery ( pRealThunk                          ,
                               &mbi_thunk                          ,
                               sizeof ( MEMORY_BASIC_INFORMATION )  ) ;

                _verify ( VirtualProtect ( mbi_thunk.BaseAddress ,
                                          mbi_thunk.RegionSize  ,
                                          PAGE_READWRITE        ,
                                          &mbi_thunk.Protect     ) ) ;

                // Save the original address if requested.
#if(WINVER >= 0x500)
                pRealThunk->u1.Function = (DWORD)(PDWORD)paHookArray[i].pProc ;
#else
				 pRealThunk->u1.Function = (PDWORD)paHookArray[i].pProc ;
#endif//
                DWORD dwOldProtect ;

                // Change the protection back to what it was before I
                //  blasted.
                _verify ( VirtualProtect ( mbi_thunk.BaseAddress ,
                                          mbi_thunk.RegionSize  ,
                                          mbi_thunk.Protect     ,
                                          &dwOldProtect          ) ) ;

                if ( NULL != pdwHooked )
                {
                    // Increment the total number hooked.
                    *pdwHooked += 1 ;
                }
            }
        }
        // Increment both tables.
        pOrigThunk++ ;
        pRealThunk++ ;
    }

    // All OK, JumpMaster!
    SetLastError ( ERROR_SUCCESS ) ;
    return ( TRUE ) ;
}


void _message(const char *psz )
{		MessageBox( 0, psz, "mlang", MB_OK );	}

LANGID		g_lid;
int			g_codepage;

HRSRC WINAPI LanguageFindResource(
    HMODULE hModule,
    LPCSTR lpName,
    LPCSTR lpType )
{
	HRSRC hrsrc = ::FindResourceEx( hModule, lpType, lpName, g_lid );
	if( hrsrc )return hrsrc;
	hrsrc = ::FindResourceEx( hModule, lpType, lpName, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US) );
	if( hrsrc )return hrsrc;
	hrsrc = ::FindResourceEx( hModule, lpType, lpName, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL) );
	if( hrsrc )return hrsrc;
	return 0;
}

HMENU WINAPI LanguageLoadMenu(
    HINSTANCE hInstance,
    LPCSTR lpMenuName)
{
	HRSRC	hrsrc = ::LanguageFindResource( hInstance, lpMenuName, RT_MENU );
	if( !hrsrc )return 0;
	HGLOBAL	hrc = ::LoadResource( hInstance, hrsrc );
	if( !hrc )return 0;

	MENUTEMPLATE	*pTemplate = (MENUTEMPLATE*)::LockResource( hrc );
	HMENU hmenu = LoadMenuIndirect( pTemplate );
	::FreeResource( hrsrc );

	return hmenu;
}

int WINAPI LanguageMultiByteToWideChar(
    UINT CodePage, DWORD dwFlags,
    LPCSTR lpMultiByteStr, int cchMultiByte,
    LPWSTR lpWideCharStr, int cchWideChar )
{
	long lret = g_pMultiByteToWideChar( CodePage, dwFlags, lpMultiByteStr, 
		cchMultiByte, lpWideCharStr, cchWideChar );

/*	OutputDebugString( lpMultiByteStr );
	OutputDebugString( "\n" );

	long lret = g_pMultiByteToWideChar( g_codepage, dwFlags, lpMultiByteStr, 
		cchMultiByte, lpWideCharStr, cchWideChar );
	if( !lret )return 0;
	if( !lpMultiByteStr || !lpWideCharStr )return lret;

	while( *lpMultiByteStr && *lpWideCharStr )
	{
		if( *lpMultiByteStr == '"' ||
			*lpMultiByteStr == '.' ||
			*lpMultiByteStr == ';' ||
			*lpMultiByteStr == ':' )
			*lpWideCharStr = *lpMultiByteStr;
		
		lpMultiByteStr++;
		lpWideCharStr++;
	}*/

	return lret;

}




HWND WINAPI LanguageCreateDialogParam(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam)
{
	HRSRC	hrsrc = ::LanguageFindResource( hInstance, lpTemplateName, RT_DIALOG );
	if( !hrsrc )return 0;
	HGLOBAL	hrc = ::LoadResource( hInstance, hrsrc );
	if( !hrc )return 0;
	DLGTEMPLATE *pTemplate = (DLGTEMPLATE*)::LockResource( hrc );

	HWND hwnd = CreateDialogIndirectParam( hInstance, pTemplate, hWndParent,
			lpDialogFunc, dwInitParam );

	::FreeResource( hrsrc );

	return hwnd;
}

//Dialog box replacement
int WINAPI LanguageDialogBoxParam(
    HINSTANCE hInstance,
    LPCSTR lpTemplateName,
    HWND hWndParent ,
    DLGPROC lpDialogFunc,
    LPARAM dwInitParam )
{
	int nResult = -1;
	HRSRC	hrsrc = ::LanguageFindResource( hInstance, lpTemplateName, RT_DIALOG );
	if( !hrsrc )return nResult;
	HGLOBAL	hrc = ::LoadResource( hInstance, hrsrc );
	if( !hrc )return nResult;
	DLGTEMPLATE *pTemplate = (DLGTEMPLATE*)::LockResource( hrc );

	nResult = DialogBoxIndirectParam( hInstance, pTemplate, hWndParent,
			lpDialogFunc, dwInitParam );

	::FreeResource( hrsrc );

	return nResult;
}

static char sz_app_title[255];

class __init_title
{
public:
	__init_title()
	{
		char	szIniFileName[MAX_PATH];	szIniFileName[0] = 0;
		::GetModuleFileName( 0, szIniFileName, MAX_PATH );
		char	*p = strrchr( szIniFileName, '\\' );
		strcpy( p, "\\shell.data" );			
		::GetPrivateProfileString( "General", "DefaultTitle:String", "VideoTesT 5.0", sz_app_title, sizeof(sz_app_title), szIniFileName );
	}
};

__init_title __init_title__;


int WINAPI LanguageLoadString(
  HINSTANCE hInstance,  // handle to resource module
  UINT uID,             // resource identifier
  LPTSTR lpBuffer,      // resource buffer
  int nBufferMax        // size of buffer
)
{
	if( uID == AFX_IDS_APP_TITLE )
	{
		if( strlen( sz_app_title ) )
		{
			strncpy( lpBuffer, sz_app_title, nBufferMax );
			lpBuffer[nBufferMax-1] = 0;
			return strlen( lpBuffer );
		}
	}

	if( !nBufferMax )return 0;

	int	nResourceID = uID/16+1;
	int	nIndex = uID % 16;

	HRSRC	hrsrc = ::LanguageFindResource( hInstance, MAKEINTRESOURCE(nResourceID), RT_STRING );
	if( !hrsrc ){ASSERT(false);return 0;}
	HGLOBAL	hrc = ::LoadResource( hInstance, hrsrc );
	if( !hrc ){ASSERT(false);return 0;}

	//pointer to string table
	wchar_t *ptsz = (wchar_t *)::LockResource( hrc );

	for( int idx = 0/*!!!*/; idx < nIndex; idx++ )
	{
		int	nLen = *ptsz;
		ptsz++;
		ptsz+=nLen;
	}


	//extract string
	int	nLen = *ptsz;
	if( !nLen )
		{return 0;};
	ptsz++;

	long length = ::WideCharToMultiByte( CP_ACP, 0, ptsz, nLen+1, 0, 0, 0, 0 );
	char	*p =  new char[length+1];
	if( !p ){ASSERT(false);return 0;}

	int 
	nResult = ::WideCharToMultiByte( CP_ACP, 0, ptsz, nLen+1, p, length, 0, 0 );
	nResult = min( nBufferMax, max( nResult, 0 ) );
	memcpy( lpBuffer, p, nResult );
	delete p;

	if( nResult == 0 )
	{
		DWORD	dw = GetLastError();
		ASSERT(false);return 0;
	}

	if( uID == AFX_IDS_APP_TITLE )
		strncpy( sz_app_title, lpBuffer, nBufferMax );

	if( nResult > 0 )
		lpBuffer[nResult-1]=0;

	lpBuffer[nBufferMax-1]=0;
	lpBuffer[nBufferMax-2]=0;
	lpBuffer[nBufferMax-3]=0;
	lpBuffer[nBufferMax-4]=0;
	::FreeResource( hrsrc );


	return nResult;
}

bool HookSystemFuncPtr( void *ptrOldFuncAddress, void *ptrNewFuncAddress )
{
	void	*pBasePage = 0;
	long	cbPageSize = 0;
	bool	bNeedChangeProtect = false;


	//get memory status
	MEMORY_BASIC_INFORMATION mbi;
	if( VirtualQuery(ptrOldFuncAddress, &mbi, sizeof(mbi)) != sizeof(mbi) )
	{
		_message( "VirtualQuery" );
		return false;
	}

	pBasePage = mbi.AllocationBase;
	cbPageSize = mbi.RegionSize;
	bNeedChangeProtect = !(mbi.Protect == PAGE_EXECUTE_WRITECOPY || mbi.Protect == PAGE_EXECUTE_READWRITE);

	if( !pBasePage || !cbPageSize )
	{
		_message( "!pBasePage || !cbPageSize" );
		return false;
	}

	if( bNeedChangeProtect )
	{
		SetLastError(0);

		DWORD dwOldProtect = 0;
		VirtualProtect( pBasePage, cbPageSize, PAGE_EXECUTE_READWRITE, &dwOldProtect );
		if( !VirtualProtect( pBasePage, cbPageSize, PAGE_EXECUTE_READWRITE, &dwOldProtect ) )
		{
			_message( "!VirtualProtect" );
			return false;
		}

		if(GetLastError() != ERROR_SUCCESS )
		{
			_message( "GetLastError() != ERROR_SUCCESS" );
			return false;
		}
	}

	// modify original code
	BYTE * ptr = (BYTE*)ptrOldFuncAddress;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &ptrNewFuncAddress, sizeof(ptrNewFuncAddress) );
	ptr += sizeof(ptrNewFuncAddress);
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}

bool HookSystemFunc( HMODULE hModule, const char *pszExportFuncName, void *ptrNewFuncAddress )
{
	//get old proc address
	void *pfn = ::GetProcAddress( hModule, pszExportFuncName );

	if( !pfn )
		return false;

	return HookSystemFuncPtr( pfn, ptrNewFuncAddress );
}

extern HOOKFUNCDESC	hookKernel[], hookUser[];



bool InstallHooks( HMODULE h )
{
	BOOL	bResult = TRUE;
	DWORD	dwHooked = 0;

	char	sz[MAX_PATH],szFName[_MAX_FNAME];
	::GetModuleFileName( h, sz, MAX_PATH );
	::strlwr( sz );
	::splitpath(sz, NULL, NULL, szFName, NULL);
	if( strstr( sz, "oledlg.dll" ) )	//Win98 problem( File Open Dlg( Create Link to file, filter) )
		return true;
	if (strcmp(sz+strlen(sz)-3,".ds")==0) // Problems with some TWAIN drivers
		return true;
	if (strcmp(szFName, "tbmapi") == 0) // Problem with some versions of the BAT.
		return true;
	

	
	
	bResult &= HookImportedFunctionsByName( h, "User32.dll", 
		4, hookUser, &dwHooked );
	bResult &= HookImportedFunctionsByName( h, "Kernel32.dll", 
		2, hookKernel, &dwHooked );

	//if( !bResult )
	//	_message( sz );

	return true;
}

HMODULE WINAPI HookLoadLibrary( LPCSTR lpLibFileName )
{
	_verify(g_pLoadLibraryA);

	HMODULE	h, h1 = ::GetModuleHandle( lpLibFileName );
	
	h = g_pLoadLibraryA( lpLibFileName );
	if( !h )return 0;
	if( h1 )return h;

	InstallHooks( h );

	return h;
}


HOOKFUNCDESC	hookUser[]=
{
	{"DialogBoxParamA", (PROC)LanguageDialogBoxParam},
	{"CreateDialogParamA", (PROC)LanguageCreateDialogParam},
	{"LoadStringA", (PROC)LanguageLoadString},
	{"LoadMenuA", (PROC)LanguageLoadMenu},
};

HOOKFUNCDESC	hookKernel[]=
{
	{"FindResourceA", (PROC)LanguageFindResource},
	{"LoadLibraryA", (PROC)HookLoadLibrary},
//	{"MultiByteToWideChar", (PROC)LanguageMultiByteToWideChar}
};


bool InitLanguageSupport( LANGID langid )
{
	g_lid = langid;
	g_codepage = CP_ACP;

	//if( g_lid == 0x0419 )g_codepage = 1251;




	bool	bResult = true;
	g_pLoadLibraryA = 
		(HMODULE  (WINAPI*)(LPCSTR))
		::GetProcAddress( GetModuleHandle( "Kernel32.dll" ), "LoadLibraryA" );
	g_pMultiByteToWideChar= 
		(int (WINAPI*)(UINT,DWORD,LPCSTR,int,LPWSTR,int))
		::GetProcAddress( GetModuleHandle( "Kernel32.dll" ), "MultiByteToWideChar" );

	if( IsNT() )
	{
		HINSTANCE	hLib = 0;
		_verify(hLib = ::LoadLibrary( "psapi.dll" ) );

		if( !hLib )return false;

		BOOL (WINAPI *pEnumProcessModules)( HANDLE , HMODULE *, DWORD , LPDWORD ) = 
			(BOOL (WINAPI *)( HANDLE , HMODULE *, DWORD , LPDWORD ))GetProcAddress( hLib, "EnumProcessModules" );

		

		HMODULE	modules[100];
		DWORD	cb;
		pEnumProcessModules( GetCurrentProcess(), modules, sizeof(modules), &cb );

		long	count = cb/4, n;

		for(n=0;n<count;n++)
			bResult &= InstallHooks(modules[n]);

		::FreeLibrary( hLib );
	}
	else
	{
		HINSTANCE	hLib = GetModuleHandle( "Kernel32.dll" );

		BOOL (WINAPI *pModule32First)( HANDLE, LPMODULEENTRY32 )=
			(BOOL (WINAPI *)( HANDLE, LPMODULEENTRY32 ))
			::GetProcAddress( hLib, "Module32First" );
		BOOL (WINAPI *pModule32Next)( HANDLE, LPMODULEENTRY32 )=
			(BOOL (WINAPI *)( HANDLE, LPMODULEENTRY32 ))
			::GetProcAddress( hLib, "Module32Next" );
		HANDLE (WINAPI *pCreateToolhelp32Snapshot)( DWORD, DWORD )=
			(HANDLE (WINAPI *)( DWORD, DWORD ))
			::GetProcAddress( hLib, "CreateToolhelp32Snapshot" );

		_verify(pModule32First);
		_verify(pModule32Next);
		_verify(pCreateToolhelp32Snapshot);
		

		HANDLE h = pCreateToolhelp32Snapshot( TH32CS_SNAPMODULE, 0 );
		MODULEENTRY32	me;
		ZeroMemory( &me, sizeof(me));
		me.dwSize = sizeof(me);
		BOOL bContinue = pModule32First( h, &me );

		while( bContinue )
		{
			bResult &= InstallHooks(me.hModule);
			bContinue = pModule32Next( h, &me );
		}

		::CloseHandle( h );
		::FreeLibrary( hLib );
	}


	return bResult;
}
