#include "stdafx.h"
#include "MenuRegistrator.h"
#include "GuardInt.h"


#ifdef _COMMON_LIB
#include "Registry.h"
#include "Utils.h"
#define __assert__( value )	ASSERT( value )
#else
#include "..\\common2\\RegHelp.h"
#include "..\\common2\\com_main.h"
#define __assert__( value )	dbg_assert( value )

#define CVTRegKey ::CRegKey
#define CVTRegValue CRegValue
#define CStringArray StringArray
#endif //_COMMON_LIB

#include "nameconsts.h"

/*
const char *_szMenuRegistryKey = 
#ifdef _DEBUG
"software\\VideoTest\\vt5\\add-ins(debug)\\menus";
#else
"software\\VideoTest\\vt5\\add-ins\\menus";
#endif //_DEBUG
*/

/////////////////////////////////////////////////////////////////////////////
//
//
//CMenuRegistrator class 
//
//
/////////////////////////////////////////////////////////////////////////////
CMenuRegistrator::CMenuRegistrator()
{
	m_hKey = NULL;
}

/////////////////////////////////////////////////////////////////////////////
CMenuRegistrator::~CMenuRegistrator()
{
	DeInit();
}


typedef void (*GAN)(char *, long *);

GAN GetFuncByName( const char* pszFuncName )
{
	HMODULE hModuleExe = GetModuleHandle(0);
	HINSTANCE hDll_Common = GetModuleHandle("common.dll");

	GAN pfn = (GAN)GetProcAddress( hModuleExe, pszFuncName );
	if( !pfn )
		pfn = (GAN)GetProcAddress( hDll_Common, pszFuncName );

	return pfn;	
}

/////////////////////////////////////////////////////////////////////////////
_bstr_t GetRegistryPath()
{
	//cos must support common, common 2, ( register && shell )
	
	bstr_t bstrRet("Software\\");
	bstr_t bstrAppName(DEF_APP_NAME);
	bstr_t bstrCompanyName(DEF_COMPANY_NAME);

	_bstr_t bstrRegKey = bstrRet + bstrCompanyName + _bstr_t("\\") + bstrAppName + _bstr_t("\\add-ins");

	IUnknown* punkAppUnknown = 0;
	typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef/* = false*/);
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		if (pGetAppUnknown)
			punkAppUnknown = pGetAppUnknown(false);
	}

	if (!punkAppUnknown)
	{
	#ifdef _DEBUG
		HINSTANCE hDll_Common = GetModuleHandle("common.dll");
	#else
		HINSTANCE hDll_Common = GetModuleHandle("common.dll");
	#endif
		if (hDll_Common)
		{
			PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hDll_Common, "GetAppUnknown"); 
			if (pGetAppUnknown)
				punkAppUnknown = pGetAppUnknown(false);
		}
	}

	IVTApplicationPtr sptrA = punkAppUnknown;
	if (sptrA != 0)
	{
		BSTR bstr = 0;
		sptrA->GetRegKey(&bstr);
		bstrRegKey = bstr;
		::SysFreeString( bstr );	bstr = 0;
	}	

	return bstrRegKey;
		
	/*

	char szName[1024];
	long lSize = 0;	

	//company name
	GAN pfn = GetFuncByName( "GetRegCompanyName" );
	if( !pfn )
		return "";
	
	lSize = 0;
	pfn(0, &lSize);
	if (lSize)
	{
		pfn( szName, &lSize);
		bstrCompanyName = szName;
	}

	//app name
	pfn = GetFuncByName( "GetRegAppName" );
	if( !pfn )
		return "";
	
	lSize = 0;
	pfn(0, &lSize);
	if (lSize)
	{
		pfn( szName, &lSize);
		bstrAppName = szName;
	}

	bstrRet += bstrCompanyName + _bstr_t("\\") + bstrAppName + _bstr_t("\\add-ins");
	*/

	
}

/////////////////////////////////////////////////////////////////////////////
_bstr_t	CMenuRegistrator::GetMenuRootName()
{
	_bstr_t bstrName = GetRegistryPath();
	if( !bstrName.length() )
		return bstrName;

	bstrName += "\\menus";

	return bstrName;
}


/////////////////////////////////////////////////////////////////////////////
_bstr_t CMenuRegistrator::GetKeyName( const char* szViewName, const char* szObjectName )
{
	if( szViewName == 0 )
		return "";

	_bstr_t bstrKey;

	_bstr_t bstrTemp = szViewName;
	if( !bstrTemp.length() )
		return "";	

	bstrKey += bstrTemp;	

	if( szObjectName )
	{
		bstrTemp = szObjectName;
		if( !bstrTemp.length() )
			return bstrKey;

		bstrKey += "_object_";

		bstrKey += bstrTemp;
	}

	return bstrKey;
}


/////////////////////////////////////////////////////////////////////////////
bool CMenuRegistrator::RegisterMenu( const char* szViewName, const char* szObjectName, const char* szUserMenuName )
{
	if( !Init() )
		return false;
	
	bstr_t bstrKeyName  = GetKeyName( szViewName, szObjectName );
	bstr_t bstrKeyValue = szUserMenuName;

	if( ::RegSetValueEx( m_hKey, (LPCSTR)bstrKeyName, 0, REG_SZ, 
					(byte *)(LPCSTR)bstrKeyValue, bstrKeyValue.length() ) != ERROR_SUCCESS )
	{	
		__assert__( FALSE );
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMenuRegistrator::UnRegisterMenu( const char* szViewName, const char* szObjectName )
{
	if( !Init() )
		return false;
	

	_bstr_t bstrKeyName  = GetKeyName( szViewName, szObjectName );
	if( bstrKeyName.length() == 0 )
	{
		__assert__( FALSE );
		return false;
	}		

	if( ::RegDeleteValue( m_hKey, (LPCTSTR)bstrKeyName ) != ERROR_SUCCESS )
	{
		__assert__( FALSE );
		return false;
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
bool CMenuRegistrator::Init( )
{
	DeInit();

	_bstr_t bstrMenuKey = GetMenuRootName();
	
	if( !bstrMenuKey.length() )
		return false;

	{
		
		CVTRegKey reg;
		if (!reg.Create(HKEY_LOCAL_MACHINE, (LPCSTR)bstrMenuKey))
			return false;

	}	


	if (::RegOpenKey(HKEY_LOCAL_MACHINE, (LPCSTR)bstrMenuKey, &m_hKey) == ERROR_SUCCESS)
		return true;



	__assert__ (FALSE);
	return false;
}

/////////////////////////////////////////////////////////////////////////////
void CMenuRegistrator::DeInit()
{
	if( m_hKey )
		::RegCloseKey( m_hKey );
	
	m_hKey = NULL;	
}

/////////////////////////////////////////////////////////////////////////////
bool CMenuRegistrator::GetMenuSectionContent( long* plCount, _bstr_t* pNames, _bstr_t* pUserNames )
{
	if( !Init() )
		return false;

	DWORD	dwSize = 255;
	char	sz[1024];
	int	nCount = 0;

	DWORD dwType = REG_SZ;
	char  szValue[1024];	
	DWORD dwValueSize = 255;

	DWORD dwSubKeysCount = 0;
	if( ERROR_SUCCESS != ::RegQueryInfoKey( m_hKey, 0, 0, 0, 0, 0, 0, &dwSubKeysCount, 0, 0, 0, 0 ) )
		return false;

	if( dwSubKeysCount <= 0 )
		return false;

	if( plCount )
		*plCount = dwSubKeysCount;

	if( !pNames && !pUserNames )
		return true;

	while( ::RegEnumValue( m_hKey, nCount, sz, &dwSize, 0, 
		(LPDWORD)&dwType, (LPBYTE)szValue, (LPDWORD)&dwValueSize ) == ERROR_SUCCESS )
	{
		//arMenuSectionContent.Add( CString(szValue) );
		if( pNames )
			pNames[nCount] = sz;

		if( pUserNames )
			pUserNames[nCount] = szValue;

		dwSize = 1024;
		dwType = REG_SZ;
		::ZeroMemory( &szValue, sizeof(szValue) );
		dwValueSize = 1024;
		nCount++;		
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
_bstr_t CMenuRegistrator::GetMenu( const char* szViewName, const char* szObjectName )
{
	if( !Init() )
		return "";
	
	bstr_t bstrKeyName  = GetKeyName( szViewName, szObjectName );

	char  szValue[255];	
	DWORD dwSize = 255;
	DWORD dwType = 0;
												   
	if( RegQueryValueEx( m_hKey, (LPCSTR)bstrKeyName, 0, &dwType, (LPBYTE)szValue, &dwSize ) != ERROR_SUCCESS )
		return "";

	return bstr_t( szValue );
}



/////////////////////////////////////////////////////////////////////////////
//
//
//CFileRegistrator class 
//
//
/////////////////////////////////////////////////////////////////////////////
#define MAX_STRING_LENGTH	1024
BOOL SetRegKeyValue(
       LPCSTR pszKey,
       LPCSTR pszSubkey,
       LPCSTR pszValue)
{
  BOOL bOk = FALSE;
  LONG ec;
  HKEY hKey;
  TCHAR szKey[MAX_STRING_LENGTH];

  lstrcpy(szKey, pszKey);

  if (NULL != pszSubkey)
  {
    lstrcat(szKey, TEXT("\\"));
    lstrcat(szKey, pszSubkey);
  }

  ec = RegCreateKeyEx(
         HKEY_CLASSES_ROOT,
         szKey,
         0,
         NULL,
         REG_OPTION_NON_VOLATILE,
         KEY_ALL_ACCESS,
         NULL,
         &hKey,
         NULL);

  if (ERROR_SUCCESS == ec)
  {
    if (NULL != pszValue)
    {
      ec = RegSetValueEx(
             hKey,
             NULL,
             0,
             REG_SZ,
             (BYTE *)pszValue,
             (lstrlen(pszValue)+1)*sizeof(TCHAR));
    }
    if (ERROR_SUCCESS == ec)
      bOk = TRUE;
    RegCloseKey(hKey);
  }

  return bOk;
}

void RegisterExt( _bstr_t bstrAppPath, _bstr_t bstrIconFileName, _bstr_t bstrExt, _bstr_t bstrShortName, _bstr_t bstrLongName, int nIcon )
{
	char szIconPath[1024];
	sprintf( szIconPath, "%s,%d", (LPCSTR)bstrIconFileName, nIcon ); 	

	_bstr_t bstrOpenPath;
	bstrOpenPath = "\"";
	bstrOpenPath += bstrAppPath;
	bstrOpenPath += "\" \"%1\"";


	SetRegKeyValue(
		TEXT( bstrExt ),
		NULL,
		TEXT( bstrShortName ) );

	SetRegKeyValue(
		TEXT( bstrShortName ),
		NULL,
		TEXT( bstrLongName ) );

	SetRegKeyValue(
		TEXT( bstrShortName ),
		TEXT( "DefaultIcon" ),
		TEXT( (LPCSTR)szIconPath ) );

	SetRegKeyValue(
		TEXT( bstrShortName ),
		TEXT("Shell"),
		TEXT("Open") );

	_bstr_t bstrTemp = bstrShortName + _bstr_t( "\\Shell" );
	
	SetRegKeyValue(
		TEXT( bstrTemp ),
		TEXT( "Open" ),
		NULL );

	bstrTemp = bstrShortName + _bstr_t( "\\Shell\\Open" );

	SetRegKeyValue(
		TEXT( bstrTemp ),
		TEXT( "Command" ),
		TEXT( bstrOpenPath ));

}

/////////////////////////////////////////////////////////////////////////////
CFileRegistrator::CFileRegistrator(  )
{

}

/////////////////////////////////////////////////////////////////////////////
CFileRegistrator::~CFileRegistrator()
{

}

/////////////////////////////////////////////////////////////////////////////
_bstr_t CFileRegistrator::GetShortName( _bstr_t bstrExt )
{
	_bstr_t bstr = bstrExt;		
	
	if( bstrExt.length() < 1 )
	{
		__assert__( false );
		return "";
	}

	char *s = bstrExt;

	if( s[0] != '.' )
		return "";

	char szBuf[255];
	
	memcpy( szBuf, (s+1), bstrExt.length() - 1 );
	szBuf[ bstrExt.length() ] = 0;	
	
	bstr = szBuf;	

	_bstr_t bstrShortName;
	bstrShortName += "VT5_";
	bstrShortName += bstr;
	bstrShortName += "_file";

	return bstrShortName;

}

#define	szFileTypesKey	"FileTypes"

#define	szShortName		"ShortName"
#define	szRegisterName	"RegisterName"
#define	szIconFileName	"IconFileName"
#define	szIconIndex		"IconIndex"

/////////////////////////////////////////////////////////////////////////////
void CFileRegistrator::Register( _bstr_t bstrExt, _bstr_t bstrRegisterName, int nIconIndex, HINSTANCE hIconInst )
{
	_bstr_t bstrShortName = GetShortName( bstrExt );
	if( !bstrShortName.length() )
		return;

	
	_bstr_t bstrIconFileName;

	if( hIconInst )
	{
		TCHAR szModulePath[MAX_PATH];
		GetModuleFileName( hIconInst, szModulePath,
			sizeof(szModulePath)/sizeof(TCHAR));
		bstrIconFileName = szModulePath;		
	}



	_bstr_t bstrFileExtKey = GetRegistryPath();
	if( !bstrFileExtKey.length() )
		return;
	
	bstrFileExtKey += "\\";
	bstrFileExtKey += szFileTypesKey;
	bstrFileExtKey += "\\";
	bstrFileExtKey += bstrExt;


	{
		CVTRegKey reg;

		if( !reg.Create(HKEY_LOCAL_MACHINE, bstrFileExtKey) )
		{
			__assert__( false );
			return;
		}

		if( !reg.Open() )
		{
			__assert__( false );
			return;
		}

		{
			CVTRegValue val( szShortName );
			val.SetString( bstrShortName );
			reg.SetValue( val );
		}

		{
			CVTRegValue val( szRegisterName );
			val.SetString( bstrRegisterName );
			reg.SetValue( val );
		}

		{
			CVTRegValue val( szIconFileName );
			val.SetString( bstrIconFileName );
			reg.SetValue( val );
		}

		{
			CVTRegValue val( szIconIndex );
			val.SetDword( nIconIndex );
			reg.SetValue( val );
		}

		reg.Close();
	}

}

/////////////////////////////////////////////////////////////////////////////
void CFileRegistrator::Unregister( 	_bstr_t bstrExt )
{
	bstr_t bstrShortName = GetShortName( bstrExt );
	if( !bstrShortName.length() )
		return;


	bstr_t bstrFileExtKey = GetRegistryPath();
	if( !bstrFileExtKey.length() )
		return;


	bstrFileExtKey += "\\";
	bstrFileExtKey += szFileTypesKey;
	bstrFileExtKey += "\\";
	bstrFileExtKey += bstrExt;

	{
		CVTRegKey reg;
		reg.Open( HKEY_LOCAL_MACHINE, bstrFileExtKey );

		reg.DeleteKey();
		
		reg.Close();
	}


	RegDeleteKey( HKEY_CLASSES_ROOT, TEXT( bstrExt ) );
	RegDeleteKey( HKEY_CLASSES_ROOT, TEXT( bstrShortName ) );
}

bool CFileRegistrator::RegisterInSystem()
{
	TCHAR szModulePath[MAX_PATH];
	GetModuleFileName( GetModuleHandle( NULL ), szModulePath,
		sizeof(szModulePath)/sizeof(TCHAR));

	_bstr_t bstrVT5Path = szModulePath;


	_bstr_t bstrFileTypesKey = GetRegistryPath();
	if( !bstrFileTypesKey.length() )
		return false;


	bstrFileTypesKey += "\\";
	bstrFileTypesKey += szFileTypesKey;


	
	CVTRegKey reg;
	if( !reg.Open( HKEY_LOCAL_MACHINE, bstrFileTypesKey ) )
		return false;

	CStringArray arStrFilesType;
	if( reg.EnumSubKeys( arStrFilesType ) )
	{
		for( int i=0;i<arStrFilesType.GetSize();i++ )
		{
			bstr_t bstrFileTypeKey = bstrFileTypesKey;
			bstrFileTypeKey += "\\";
			bstrFileTypeKey += ( (LPCSTR)(arStrFilesType[i]) );
			CVTRegKey regType;
			if( regType.Open( HKEY_LOCAL_MACHINE, bstrFileTypeKey ) )
			{

				//strVT5Path
				bstr_t bstrExt = arStrFilesType[i];
				bstr_t bstrIconFileName;
				bstr_t bstrShortName;
				bstr_t bstrLongName;
				int nIcon = -1;
				{
					CVTRegValue val( szShortName );
					regType.GetValue( val ); 
					bstrShortName = val.GetString();
				}

				{
					CVTRegValue val( szRegisterName );
					regType.GetValue( val );
					bstrLongName = val.GetString();
				}

				{
					CVTRegValue val( szIconFileName );
					regType.GetValue( val );
					if(val.GetString()!=0)
						bstrIconFileName = _bstr_t(val.GetString());
					else
						bstrIconFileName = bstrVT5Path;
					if( !bstrIconFileName.length() )
						bstrIconFileName = bstrVT5Path;
				}
				
				{
					CVTRegValue val( szIconIndex );
					regType.GetValue( val );
					nIcon = (int)val.GetDword();
				}

				RegisterExt( bstrVT5Path, bstrIconFileName, bstrExt, bstrShortName, bstrLongName, nIcon );

				regType.Close();

			}
			
		}		
	}


	reg.Close();

	return true;

}


