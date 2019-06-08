#include "stdafx.h"
#include "com_main.h"
#include "win_main.h"
#include "action_main.h"
#include "RegHelp.h"
#include "Core5.h"
#include "Tchar.h"





//global funcs
DWORD RegDeleteKeyEx( HKEY hGlobalKey, const char *pszPath )
{
	HKEY	hKey = 0;
	DWORD dwRes = ::RegOpenKey( hGlobalKey, pszPath, &hKey );
	if( dwRes )return dwRes;

	char	szNewName[255];
	while( ::RegEnumKey( hKey, 0, szNewName, 255 ) == 0 )
	{
		char	szNewKey[512];
		strcpy( szNewKey, pszPath );
		strcat( szNewKey, "\\" );
		strcpy( szNewKey, szNewName );

		dwRes = ::RegDeleteKeyEx( hKey, szNewKey );
		if( dwRes )return dwRes;
	}
	::RegCloseKey( hKey );

	return RegDeleteKey( hGlobalKey, pszPath );


}

void dbg_assert( bool bExpression, const char *pszErrorDescription )
{
	if( !bExpression )
	{

#ifdef _DEBUG
	DebugBreak();
#endif

	}
}

static bool _UnregisterObject(GuidKey & guidKey, _bstr_t bstrProgID, _bstr_t bstrSection);
static bool _RegisterObject(GuidKey & guidKey, _bstr_t bstrProgID, _bstr_t bstrSectProgID, _bstr_t bstrSect);


//application gobal data
App::App()
{
	m_punkBitmapProvider = 0;//CBitmapProvider::get_instance()->Unknown();
	m_punkAppUnknown = 0;

	m_hInstance = 0;
	m_pszModuleFileName = 0;
	m_pFirstComInfo = 0;

	m_pclass = 0;
#ifdef _DEBUG
	m_bstrRegProgKey = "SOFTWARE\\VideoTest\\vt5\\add-ins(debug)";
#else
	m_bstrRegProgKey = "SOFTWARE\\VideoTest\\vt5\\add-ins";
#endif // _DEBUG

}


App::~App()
{
//	OutputDebugString( "\n~~~" );
//	OutputDebugString( module_filename() );
//	OutputDebugString( "\n" );


	delete[] m_pszModuleFileName;
}

const char *App::module_filename()
{
	if( !m_pszModuleFileName )
	{
		m_pszModuleFileName = new char[_MAX_PATH];
		::GetModuleFileName( m_hInstance, m_pszModuleFileName, _MAX_PATH );
		::GetLongPathName( m_pszModuleFileName, m_pszModuleFileName, _MAX_PATH ) ;
	}
	return m_pszModuleFileName;
}

void App::handle_error( dword dwErrorCode, char *pszErrorDescription, bool bCriticalError )
{
	char	sz[20];
	if( dwErrorCode == -1 )
		dwErrorCode = ::GetLastError();
	if( !pszErrorDescription )
		pszErrorDescription = "Generic Error";


	if( bCriticalError )
		::wsprintf( sz, "!Critical Error %d: ", dwErrorCode );
	else
		::wsprintf( sz, "Error %d: ", dwErrorCode );
	::OutputDebugString( sz );

	::OutputDebugString( pszErrorDescription );
	::OutputDebugString( "\n" );
}

bool App::Init( HINSTANCE h )
{
	m_hInstance = h;

	OutputDebugString( "!!!" );
	OutputDebugString( module_filename() );
	OutputDebugString( "\n" );

	
	OleInitialize( 0 );

	::CoGetMalloc( 1, &m_ptrMalloc );

	m_pclass = 0;
	
	
	WindowClass( new CWndClass( szDefClassName ) );
	WindowClass( new CWndClass( szAxContainerWinClassName, InitAxWindowProc ) );

	CWndClass	*p = m_pclass;
	while( p )
	{
		p->Register();
		p = p->m_pnext;
	}

	m_punkBitmapProvider = CBitmapProvider::get_instance()->Unknown();


	typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		if (pGetAppUnknown)
			m_punkAppUnknown = pGetAppUnknown(false);
	}

	if (!m_punkAppUnknown)
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
				m_punkAppUnknown = pGetAppUnknown(false);
		}
	}

	IVTApplicationPtr sptrA = m_punkAppUnknown;
	if (sptrA != 0)
	{
		BSTR bstr = 0;
		sptrA->GetRegKey(&bstr);
		_bstr_t bstrKey(bstr, false);

		m_bstrRegProgKey = bstrKey;
	}

	return true;
}

bool App::Deinit()
{
	while( m_pFirstComInfo )
	{
		ComInfo	*pcomp = m_pFirstComInfo->m_pnext;
		delete m_pFirstComInfo;
		m_pFirstComInfo = pcomp;
	}


	while( m_pclass )
	{
		CWndClass	*p = m_pclass->m_pnext;
		UnregisterClass(m_pclass->GetClassName(),  handle());
		delete m_pclass;
		m_pclass = p;
	}

	m_punkBitmapProvider->Release();
	m_punkBitmapProvider = 0;
	m_ptrMalloc = 0;
	OleUninitialize();
	m_hInstance = 0;
	return true;
}

bool App::ObjectInfo( void *(*p)(), const CLSID &clsid, const char *psz, char *pszAddProgIDSect )
{
	ComInfo	*pci = new ComInfo;
	memset( pci, 0, sizeof( ComInfo ) );

	pci->m_pfn = p;
	memcpy( &pci->m_clsid, &clsid, sizeof( clsid ) );
	pci->m_bstrProgID = psz;


	//MessageBox( 0, psz, pci->m_bstrProgID, 0 );

	if (pszAddProgIDSect && lstrlen(pszAddProgIDSect) > 0)
	{
		_bstr_t bstrSect(m_bstrRegProgKey);
		bstrSect += "\\";
		bstrSect += pszAddProgIDSect;

		pci->m_bstrSect = bstrSect;
	}

	pci->m_pnext = m_pFirstComInfo;
	m_pFirstComInfo = pci;

	return 0;
}

CWndClass *App::FindWindowClass( const char *pszClassName )
{
	CWndClass	*pclass = m_pclass;

	while( pclass )
	{
		if( !strcmp( pclass->GetClassName(), pszClassName ) )
			return pclass;
		pclass = pclass->m_pnext;
	}
	return 0;
}

bool App::WindowClass( CWndClass *pclass )
{
	pclass->m_pnext = m_pclass;
	m_pclass = pclass;

	return true;
}

ComObjectBase	*App::CreateInstance( const CLSID &clsid )
{
	ComInfo	*pinfo = m_pFirstComInfo;
	while( pinfo )
	{
		if( pinfo->m_clsid == clsid )
			return (ComObjectBase*)pinfo->m_pfn();
		pinfo = pinfo->m_pnext;
	}

	return 0;
}

//class factory
ClassFactory::ClassFactory()
{
	ZeroMemory( &m_clsid, sizeof( m_clsid ) );
}

bool ClassFactory::RegisterAll( bool bRegister )
{
	App::ComInfo	*pi = App::instance()->FirstComInfo();
	bool bRet = true;
	while( pi )
	{
		if (!UpdateRegistry(bRegister, pi))
			bRet = false;

//		_register( pi, bRegister );
		pi = pi->m_pnext;
	}
	return true;
}

void ClassFactory::LoadObjectInfo()
{
	App::ComInfo * pi = App::instance()->FirstComInfo();
	while (pi)
	{
		LoadInfo(pi);
		pi = pi->m_pnext;
	}
}

bool ClassFactory::_register( App::ComInfo *pi, bool bRegister )
{
	// get application
	IVTApplicationPtr sptrApp = App::application();
	if (!pi || sptrApp == 0)
		return false;

	BSTR	bstr = 0;
	::StringFromCLSID( pi->m_clsid, &bstr );
	_bstr_t	bstrRegGuid = "CLSID\\";
	_bstr_t	bstrCLSID =  bstr;
	bstrRegGuid += bstr;
	::CoTaskMemFree( bstr );
	
	if( bRegister )
	{
		_bstr_t	bstrRegInprocServer = bstrRegGuid+"\\InprocServer32";
		_bstr_t	bstrRegProgID = bstrRegGuid+"\\ProgID";
		_bstr_t	pszProgID_CLSID = pi->m_bstrProgID;

		pszProgID_CLSID +="\\CLSID";

		const char	*pszFileName = App::instance()->module_filename();
		const char	*pszProgID = pi->m_bstrProgID;
		const char  *pszThreadModel = "Apartment";
		const char	*pszCLSID = bstrCLSID;

		HKEY	hKey = 0;
		if( ::RegOpenKey( HKEY_CLASSES_ROOT, bstrRegGuid, &hKey ) != S_OK )
			if( ::RegCreateKey( HKEY_CLASSES_ROOT, bstrRegGuid, &hKey ) != S_OK )
			{
				App::instance()->handle_error();
				return false;
			}

		
		if( ::RegSetValueEx( hKey, 0, 0, REG_SZ, (byte*)pszProgID, strlen( pszProgID )+1 ) != 0 )
		{
			App::instance()->handle_error();
			::RegCloseKey( hKey );
			return false;
		}

		if( ::RegCloseKey( hKey ) != 0 )
		{
			App::instance()->handle_error();
			return false;
		}
		hKey = 0;
//register "InprocServer32
		if( ::RegOpenKey( HKEY_CLASSES_ROOT, bstrRegInprocServer, &hKey ) != S_OK )
			if( ::RegCreateKey( HKEY_CLASSES_ROOT, bstrRegInprocServer, &hKey ) != S_OK )
			{
				App::instance()->handle_error();
				return false;
			}
		
		if( ::RegSetValueEx( hKey, 0, 0, REG_SZ, (byte*)pszFileName, strlen( pszFileName )+1 ) != 0 )
		{
			App::instance()->handle_error();
			::RegCloseKey( hKey );
			return false;
		}

		if( ::RegSetValueEx( hKey, "ThreadingModel", 0, REG_SZ, (byte*)pszThreadModel, strlen( pszThreadModel	)+1 ) != 0 )
		{
			App::instance()->handle_error();
			::RegCloseKey( hKey );
			return false;
		}

		if( ::RegCloseKey( hKey ) != 0 )
		{
			App::instance()->handle_error();
			return false;
		}
//register prog id
		if( ::RegOpenKey( HKEY_CLASSES_ROOT, bstrRegProgID, &hKey ) != S_OK )
			if( ::RegCreateKey( HKEY_CLASSES_ROOT, bstrRegProgID, &hKey ) != S_OK )
			{
				App::instance()->handle_error();
				return false;
			}

		
		if( ::RegSetValueEx( hKey, 0, 0, REG_SZ, (byte*)pszProgID, strlen( pszProgID )+1 ) != 0 )
		{
			App::instance()->handle_error();
			::RegCloseKey( hKey );
			return false;
		}

		if( ::RegCloseKey( hKey ) != 0 )
		{
			App::instance()->handle_error();
			return false;
		}
		hKey = 0;
//registe in progid section
		char	szCompProgID[100];
		strcpy( szCompProgID, pszProgID );
		char	szCompName[100];
		char	*ppt1 = ::strchr( szCompProgID, '.' );
		char	*ppt2 = 0;
		if( ppt1 )
			ppt2 = ::strchr( ppt1+1, '.' );
		if( !ppt1 )ppt1 = szCompProgID;
		if( !ppt2 )ppt2 = ppt1 + strlen( szCompProgID );
		strncpy( szCompName, ppt1+1, min( 100, (long)ppt2-(long)ppt1-1 ) );

		if( ::RegOpenKey( HKEY_CLASSES_ROOT, pszProgID, &hKey ) != S_OK )
			if( ::RegCreateKey( HKEY_CLASSES_ROOT, pszProgID, &hKey ) != S_OK )
			{
				App::instance()->handle_error();
				return false;
			}
		
		if( ::RegSetValueEx( hKey, 0, 0, REG_SZ, (byte*)szCompName, strlen( szCompName )+1 ) != 0 )
		{
			App::instance()->handle_error();
			::RegCloseKey( hKey );
			return false;
		}

		if( ::RegCloseKey( hKey ) != 0 )
		{
			App::instance()->handle_error();
			return false;
		}
//progid\\clsid
		if( ::RegOpenKey( HKEY_CLASSES_ROOT, pszProgID_CLSID, &hKey ) != S_OK )
			if( ::RegCreateKey( HKEY_CLASSES_ROOT, pszProgID_CLSID, &hKey ) != S_OK )
			{
				App::instance()->handle_error();
				return false;
			}
		
		if( ::RegSetValueEx( hKey, 0, 0, REG_SZ, (byte*)pszCLSID, strlen( pszCLSID )+1 ) != 0 )
		{
			App::instance()->handle_error();
			::RegCloseKey( hKey );
			return false;
		}

		if( ::RegCloseKey( hKey ) != 0 )
		{
			App::instance()->handle_error();
			return false;
		}


//additional section
		if( pi->m_bstrSect.length() )
		{
			if( ::RegOpenKey( HKEY_LOCAL_MACHINE, pi->m_bstrSect, &hKey ) != S_OK )
				if( ::RegCreateKey( HKEY_LOCAL_MACHINE, pi->m_bstrSect, &hKey ) != S_OK )
			{
				App::instance()->handle_error();
				return false;
			}
		
			DWORD	dwCompFlag = 0;
			if( ::RegSetValueEx( hKey, pi->m_bstrProgID, 0, REG_DWORD, (byte*)&dwCompFlag, 4 ) != 0 )
			{
				App::instance()->handle_error();
				::RegCloseKey( hKey );
				return false;
			}

			if( ::RegCloseKey( hKey ) != 0 )
			{
				App::instance()->handle_error();
				return false;
			}
		}

		return true;
	}
	else
	{
		if( ::RegDeleteKeyEx( HKEY_CLASSES_ROOT, bstrRegGuid ) != S_OK ) 
		{
			App::instance()->handle_error();
			return false;
		}
		if( ::RegDeleteKeyEx( HKEY_CLASSES_ROOT, pi->m_bstrProgID ) != S_OK ) 
		{
			App::instance()->handle_error();
			return false;
		}

		if( pi->m_bstrSect.length() )
		{
			HKEY	hKey = 0;
			if( !::RegOpenKey( HKEY_LOCAL_MACHINE, pi->m_bstrSect, &hKey ) )
			{
				if( ::RegDeleteValue( hKey, pi->m_bstrProgID ) != S_OK )
				{
					::RegCloseKey( hKey );
					App::instance()->handle_error();
					return false;
				}
				if( ::RegCloseKey( hKey ) != S_OK )
				{
					App::instance()->handle_error();
					return false;
				}
			}
		}
	}

	return true;
}

HRESULT ClassFactory::CreateInstance( IUnknown *pUnkOuter, REFIID riid, void **ppvObject )
{
	ComObjectBase *pobj = App::instance()->CreateInstance( m_clsid );
	if( !pobj )
		return CLASS_E_CLASSNOTAVAILABLE;

	if( pUnkOuter )
	{
		if( !pobj->SetOuterUnknown( pUnkOuter ) || riid != IID_IUnknown )
		{
			pobj->Release();
			return CLASS_E_NOAGGREGATION;
		}
		*ppvObject = (IUnknown*)pobj->InnerUnknown();
		return S_OK;
	}

	HRESULT hRes = pobj->QueryInterface( riid, ppvObject );
	pobj->Release();

	return hRes;
}

HRESULT ClassFactory::LockServer( BOOL fLock )
{
	if( fLock )
		App::instance()->AddRef();
	else
		App::instance()->Release();

	return S_OK;
}

//IClassFactory2
HRESULT ClassFactory::GetLicInfo(LICINFO * pLicInfo)
{
	return E_NOTIMPL;
}

HRESULT ClassFactory::RequestLicKey(DWORD dwReserved, BSTR * pBstrKey)
{
	return E_NOTIMPL;
}

HRESULT ClassFactory::CreateInstanceLic(IUnknown * pUnkOuter, IUnknown * pUnkReserved, REFIID riid, BSTR bstrKey, PVOID * ppvObj)
{
	return VTCreateObject(pUnkOuter, pUnkReserved, riid, bstrKey, ppvObj);
}

//IVTClass
HRESULT ClassFactory::VTCreateObject(LPUNKNOWN pUnkOuter, LPUNKNOWN pUnkReserved, REFIID riid, BSTR bstrKey, LPVOID* ppvObject)
{
	if (!ppvObject)
		return E_POINTER;

	if (m_clsid == CLSID_NULL)
		return E_FAIL;

	App::ComInfo * pInfo = App::instance()->FirstComInfo();
	while (pInfo)
	{
		if (pInfo->m_clsid == m_clsid)
		{
			break;
		}
		pInfo = pInfo->m_pnext;
	}
	if (!pInfo)
		return CLASS_E_CLASSNOTAVAILABLE;

	ComObjectBase * pobj = (ComObjectBase*)pInfo->m_pfn();;
	if (!pobj)
		return CLASS_E_CLASSNOTAVAILABLE;

	if (pUnkOuter)
	{
		if (!pobj->SetOuterUnknown(pUnkOuter) || riid != IID_IUnknown)
		{
			pobj->Release();
			return CLASS_E_NOAGGREGATION;
		}
		*ppvObject = (IUnknown*)pobj->InnerUnknown();
		return S_OK;
	}

	HRESULT hRes = pobj->QueryInterface(riid, ppvObject);
	pobj->Release();

	return hRes;
}

HRESULT ClassFactory::VTRegister(BOOL bRegister)
{
	if (m_clsid == CLSID_NULL)
		return E_FAIL;

	App::ComInfo * pInfo = App::instance()->FirstComInfo();
	while (pInfo)
	{
		if (pInfo->m_clsid == m_clsid)
		{
			break;
		}
		pInfo = pInfo->m_pnext;
	}
	if (!pInfo)
		return CLASS_E_CLASSNOTAVAILABLE;

	return UpdateRegistry(bRegister ? true : false, pInfo) ? S_OK : E_FAIL;
}

HRESULT ClassFactory::VTGetProgID(BSTR * pbstrProgID)
{
	if (!pbstrProgID)
		return E_POINTER;

	if (m_clsid == CLSID_NULL)
		return E_FAIL;

	App::ComInfo * pInfo = App::instance()->FirstComInfo();
	while (pInfo)
	{
		if (pInfo->m_clsid == m_clsid)
		{
			break;
		}
		pInfo = pInfo->m_pnext;
	}
	if (!pInfo)
		return CLASS_E_CLASSNOTAVAILABLE;

	*pbstrProgID = pInfo->m_bstrProgID.copy();
	return S_OK;
}


void ClassFactory::SetCLSID( const CLSID &clsid )
{
	m_clsid = clsid;
}

void ClassFactory::LoadInfo(App::ComInfo * pInfo)
{
	IVTApplicationPtr sptrApp = App::application();
	if (sptrApp == 0 || !pInfo)
		return;

	pInfo->m_clsidExt = CLSID_NULL;
	sptrApp->GetModeData(0, (DWORD*)&(pInfo->m_clsidExt), pInfo->m_bstrProgID);
}

bool ClassFactory::UpdateRegistry(bool bRegister, App::ComInfo * pInfo)
{
	IVTApplicationPtr sptrApp = App::application();
	if (!pInfo || sptrApp == 0)
	{
		App::instance()->handle_error();
		return false;
	}

	BSTR bstrsuff = 0;
	if (FAILED(sptrApp->GetData(0, 0, &bstrsuff, 0)))
	{
		App::instance()->handle_error();
		return false;
	}

	// suffix
	_bstr_t bstrSuff(bstrsuff, false);

	// get old extern guid && old data
	GuidKey	guidExtern;
	DWORD	dwData = 0;
	sptrApp->GetEntry(1, (DWORD*)&(pInfo->m_clsid), (DWORD*)&guidExtern, &dwData);

	// if not found try again by progid
	if (guidExtern == INVALID_KEY)
		sptrApp->GetModeData(0, (DWORD*)&guidExtern, pInfo->m_bstrProgID);

	// update extern ComInfo->clsidExtern if needed
	if (guidExtern != INVALID_KEY && guidExtern != GuidKey(pInfo->m_clsidExt))
		pInfo->m_clsidExt = guidExtern;

	// Create extern ProgID
	_bstr_t bstrProgIDEx(pInfo->m_bstrProgID);
	bstrProgIDEx += bstrSuff;

	// if this call from fastreg application
	BOOL bModeRegister = false;
	sptrApp->GetMode(&bModeRegister);
	if (!bModeRegister)
	{
		IApplicationPtr strA = sptrApp;
		bModeRegister = strA != 0;
	}
	return bRegister ?	RegisterThis(sptrApp, bModeRegister, pInfo, bstrProgIDEx, dwData) :  
						UnregisterThis(sptrApp, bModeRegister, pInfo, bstrProgIDEx);
}

bool ClassFactory::RegisterThis(IVTApplication * pApp, BOOL bModeRegister, App::ComInfo * pInfo, _bstr_t bstrProgIDEx, DWORD dwData)
{
	GuidKey guidExtern(pInfo->m_clsidExt);
	if (bModeRegister)
	{
		// first we need to unregister this component
		UnregisterThis(pApp, bModeRegister, pInfo, bstrProgIDEx);
	
		// second : we need remove old entry from table and reuse extern clsid
		if (guidExtern != INVALID_KEY)
			pApp->RemoveEntry((DWORD*)&guidExtern, pInfo->m_bstrProgID);

		else // we need to generate new clsid
		{
			// and create new CLSID if needed 
			guidExtern = (pInfo->m_clsid);
		}
		pInfo->m_clsidExt = guidExtern;
	}

	// and register object		
	if (bModeRegister && FAILED(pApp->AddEntry((DWORD*)&(pInfo->m_clsid), (DWORD*)&guidExtern, pInfo->m_bstrProgID, dwData)))
	{
		App::instance()->handle_error();
		return false;
	}

	return guidExtern != INVALID_KEY ? _RegisterObject(guidExtern, bstrProgIDEx, pInfo->m_bstrProgID, pInfo->m_bstrSect) : false;
}

bool _RegisterObject(GuidKey & guidKey, _bstr_t bstrProgID, _bstr_t bstrSectProgID, _bstr_t bstrSect)
{
	if (!bstrProgID.length() || guidKey == INVALID_KEY)
	{
		App::instance()->handle_error();
		return false;
	}

	BSTR bstr = 0;
	if (FAILED(::StringFromCLSID(guidKey, &bstr)) || !bstr)
	{
		App::instance()->handle_error();
		return false;
	}

	// string {CLSID}
	_bstr_t bstrCLSID = bstr;
	::CoTaskMemFree(bstr);
	// string CLSID\\{CLSID}
	_bstr_t bstrRegGuid = "CLSID\\";
	bstrRegGuid += bstrCLSID;
	// string CLSID\\{CLSID}\\InprocServer32
	_bstr_t bstrRegInprocServer(bstrRegGuid);
	bstrRegInprocServer += "\\InprocServer32";
	// string CLSID\\{CLSID}\\ProgID
	_bstr_t bstrRegProgID(bstrRegGuid);
	bstrRegProgID += "\\ProgID";
	// string {ProgID}\\CLSID
	_bstr_t bstrProgID_CLSID(bstrProgID);
	bstrProgID_CLSID += "\\CLSID";

	// get module name
	char szShortPath[_MAX_PATH];
	GetShortPathName(App::instance()->module_filename(), szShortPath, _MAX_PATH);
 
	_bstr_t bstrModuleName(szShortPath);
	if (!bstrModuleName.length())
	{
		App::instance()->handle_error();
		return false;
	}

// string CLSID\\{CLSID}
	::CRegKey reg;
	if (reg.Create(HKEY_CLASSES_ROOT, bstrRegGuid))
	{
		CRegValue valProgID;
		valProgID.SetString(bstrProgID);
		if (!reg.SetValue(valProgID))
		{
			App::instance()->handle_error();
			return false;
		}
	}
	else
	{
		App::instance()->handle_error();
		return false;
	}

// string CLSID\\{CLSID}\\InprocServer32
	::CRegKey regInProcServer;
	if (regInProcServer.Create(HKEY_CLASSES_ROOT, bstrRegInprocServer))
	{
		// set module name
		CRegValue val;
		val.SetString(bstrModuleName);
		if (!regInProcServer.SetValue(val))
		{
			App::instance()->handle_error();
			return false;
		}

// threadingmodel
		val.SetName("ThreadingModel");
		val.SetString("Apartment");
		if (!regInProcServer.SetValue(val))
		{
			App::instance()->handle_error();
			return false;
		}
	}
	else
	{
		App::instance()->handle_error();
		return false;
	}

// string CLSID\\{CLSID}\\ProgID
	::CRegKey regProgID;
	if (regProgID.Create(HKEY_CLASSES_ROOT, bstrRegProgID))
	{
		// set value
		CRegValue valProgID;
		valProgID.SetString(bstrProgID);
		if (!regProgID.SetValue(valProgID))
		{
			App::instance()->handle_error();
			return false;
		}
	}
	else
	{
		App::instance()->handle_error();
		return false;
	}

// string {ProgID}\\CLSID
	if (regProgID.Create(HKEY_CLASSES_ROOT, bstrProgID_CLSID))
	{
		CRegValue valCLSID;
		valCLSID.SetString(bstrCLSID);
		if (!regProgID.SetValue(valCLSID))
		{
			App::instance()->handle_error();
			return false;
		}
	}
	else
	{
		App::instance()->handle_error();
		return false;
	}

	if (bstrSect.length())
	{
		::CRegKey regSect;
		if (regSect.Create(HKEY_LOCAL_MACHINE, bstrSect))
		{
			CRegValue val(bstrSectProgID);
			val.SetDword(0);
			regSect.SetValue(val);
		}
	}

	return true;
}

/*
interface IVTApplication : public IUnknown
{
	// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
	com_call GetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito) = 0;
	// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
	com_call SetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito) = 0;
	// get mode (register or not)
	com_call GetMode(BOOL * pbRegister) = 0;

	// Add guid to table
	com_call AddEntry(DWORD * pIntGUID, DWORD * pExtGUID, BSTR bstrIntProgID, DWORD dwData) = 0;
	// Remove guid from table
	com_call RemoveEntry(DWORD * pExtGUID, BSTR bstrIntProgID) = 0;
	// get inner guid from table by extern guid	: mode 0
	// get extern guid by inner guid			: mode 1
	com_call GetEntry(DWORD dwMode, DWORD * pSrcGUID, DWORD * pDstGUID, DWORD * pdwData) = 0;

	// Get External GUID from internal/external ProgID : mode 0
	// Get internal GUID from internal/external ProgID : mode 1
	com_call GetModeData(DWORD dwMode, DWORD * pGUID, BSTR bstrProgID) = 0;
	// translations	
	// get external ProgID from internal ProgID : mode 0
	// get internal ProgID from external ProgID : mode 1
	com_call GetDataValue(DWORD dwMode, BSTR bstrSrcProgID, BSTR * pbstrDstProgID) = 0;

	com_call GetNames(BSTR * pbstrAppName, BSTR * pbstrCompanyName) = 0;
	com_call GetRegKey(BSTR * pbstrRegKey) = 0;
};
*/

bool ClassFactory::UnregisterThis(IVTApplication * pApp, BOOL bModeRegister, App::ComInfo * pInfo, _bstr_t bstrProgIDEx)
{
	if (!pInfo || !pApp || !bModeRegister)
	{
		App::instance()->handle_error();
		return false;
	}
	
	// really unregister ? 
	if (!bModeRegister)
		return true;

	// addition clean up
	_UnregisterObject(GuidKey(pInfo->m_clsid), pInfo->m_bstrProgID, pInfo->m_bstrSect);

	GuidKey guidExtern = (pInfo->m_clsidExt);
	// Remove guid from table
	if (guidExtern != INVALID_KEY)
	{
		pApp->RemoveEntry((DWORD*)&guidExtern, pInfo->m_bstrProgID);
		return _UnregisterObject(guidExtern, bstrProgIDEx, pInfo->m_bstrSect);
	}

	return true;
}

bool _UnregisterObject(GuidKey & guidKey, _bstr_t bstrProgID, _bstr_t bstrSection)
{
	if (bstrProgID.length())
	{
		_bstr_t bstrProgIDCLSID(bstrProgID);
		bstrProgIDCLSID += "\\CLSID";

		::CRegKey regProg;
		if (regProg.Open(HKEY_CLASSES_ROOT, bstrProgIDCLSID))
		{
			_bstr_t bstrCLSID;
			CRegValue val;
			val.SetType(REG_SZ);
			if (regProg.GetValue(val))
				bstrCLSID = val.GetString();

			if (bstrCLSID.length())
			{
				_bstr_t bstrKey("CLSID\\");
				bstrKey += bstrCLSID;

				::CRegKey regCLSID;
				if (regCLSID.Open(HKEY_CLASSES_ROOT, bstrKey))
					regCLSID.DeleteKey();
			}
		}
		if (regProg.Open(HKEY_CLASSES_ROOT, bstrProgID))
			regProg.DeleteKey();
	}

	if (guidKey != INVALID_KEY)
	{
		// create string key
		BSTR bstr = 0;
		if (SUCCEEDED(::StringFromCLSID(guidKey, &bstr)) && bstr)
		{
			_bstr_t bstrCLSID(bstr);

			::CoTaskMemFree(bstr);

			_bstr_t bstrCLSIDKey("CLSID\\");
			bstrCLSIDKey += bstrCLSID;
			_bstr_t bstrCLSIDProgIDKey(bstrCLSIDKey);
			bstrCLSIDProgIDKey += "\\ProgID";

			// get real progID
			_bstr_t bstrRealCLSIDProgID;
			::CRegKey regProg;
			if (regProg.Open(HKEY_CLASSES_ROOT, bstrCLSIDProgIDKey))
			{
				// get default value (registered extern ProgID)
				CRegValue val;
				val.SetType(REG_SZ);
				if (regProg.GetValue(val))
					bstrRealCLSIDProgID = val.GetString();

				regProg.Close();
			}

			// delete CLSID entry
			::CRegKey regCLSID;
			if (regCLSID.Open(HKEY_CLASSES_ROOT, bstrCLSIDKey))
				regCLSID.DeleteKey();

			// if real progID is Valid
			if (bstrRealCLSIDProgID.length())
			{
				// delete real ProgID 
				_UnregisterObject(INVALID_KEY, bstrRealCLSIDProgID, bstrSection);
			}
		}
	}
	if (bstrSection.length())
	{
		::CRegKey reg;
		if (reg.Open(HKEY_LOCAL_MACHINE, bstrSection))
		{
			CRegValue val(bstrProgID);
			reg.DeleteValue(val);
		}
	}
	return true;
}


// {ffffffff-ffff-ffff-ffff-ffffffffffff}
static GUID cmp_guid1 = { 0xffffffff, 0xffff, 0xffff, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } };
// {00000000-0000-0000-0000-000000000000}
static GUID cmp_guid2 = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };

bool NeedComponentInfo(const CLSID &rclsid, const IID &iid)
{
	return GuidKey(rclsid) == GuidKey(cmp_guid1) && GuidKey(iid) == GuidKey(cmp_guid2);
}

HRESULT GetComponentInfo(void ** ppv)
{
	__GuardComponentInfo * pInfo = *((__GuardComponentInfo**)ppv);
	if (!pInfo || pInfo->nSize != sizeof(__GuardComponentInfo))
		return E_INVALIDARG;
		
	pInfo->bstrName = 0;
	pInfo->guid = GUID_NULL;
	pInfo->bIsAction = false;

	App::ComInfo * pComInfo = 0;

	if (!pInfo->lpos)
	{
		return E_FAIL;
	}
	else if (pInfo->lpos == (TPOS)-1) // get first component
	{
		pComInfo = App::instance()->FirstComInfo();
		pInfo->lpos = (TPOS)(pComInfo ? pComInfo->m_pnext : 0);
	}
	else // get next component
	{
		pComInfo = (App::ComInfo*)pInfo->lpos;
		pInfo->lpos = (TPOS)(pComInfo ? pComInfo->m_pnext : 0);
	}

	if (!pComInfo)
	{
		pInfo->lpos = 0;
		return E_FAIL;
	}

	pInfo->bstrName = pComInfo->m_bstrProgID.copy();
	pInfo->guid = pComInfo->m_clsid;

	_bstr_t bstrPrefix(ACTION_NAME_PREFIX);
	pInfo->bIsAction = _tcsncmp(pComInfo->m_bstrProgID, bstrPrefix, bstrPrefix.length()) == 0;

	return S_OK;
}


long	lAlloc12Counter = 0;