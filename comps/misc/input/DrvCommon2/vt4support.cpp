#include "StdAfx.h"
#include "vt4Support.h"
#include "utils.h"


void _addsubdir( char *pszResName, const char *szName, const char *szSubdirName )
{
	int	ilen = ::strlen( szName );
	if( !ilen )
	{
		::strcpy( pszResName, szSubdirName );
	}
	else
	{
		::strcpy( pszResName, szName );
		pszResName+=ilen;
		if( szName[ilen-1] != '\\' )
			*pszResName++ = '\\';
		::strcpy( pszResName, szSubdirName );
//		if( pszResName[::strlen( pszResName )-1] != '\\' )
//			*pszResName++ = '\\';
	}
}

void _deletesubdir( char *pszResName, const char *szName, const int iCount )
{
	int	ilen = ::strlen( szName );

	::strcpy( pszResName, szName );

	if( ilen > 0 && pszResName[ilen-1] == '\\' )
		pszResName[--ilen] = 0;


	for( int i = 0; i < iCount; i++ )
	{
		char	*posLastDir = ::strrchr( pszResName, '\\' );

		if( !posLastDir )	//no parent dir
			*pszResName = 0;
		else
			pszResName[posLastDir-pszResName] = 0;
	}
}

bool _isfileexist( const char *szFileName )
{
//
	HANDLE	h = ::CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,  
		FILE_ATTRIBUTE_NORMAL, 0 );

	if( h == INVALID_HANDLE_VALUE )
	{
		DWORD	dw = ::GetLastError();
		return false;
	}

	::CloseHandle( h );

	return true;

}


// Help adjusting
void AjustHelpFileName( CWinApp *papp )
{
	CString	s( papp->m_pszHelpFilePath );

	HKEY	hKey = 0;
	DWORD	dwType = REG_SZ;
	DWORD	dwSize = _MAX_PATH;
	BYTE	bytes[_MAX_PATH];

	::RegOpenKeyEx( HKEY_CURRENT_USER, "Software\\ISTA-VideoTesT\\VideoTesT\\Settings", 0, KEY_ALL_ACCESS, &hKey );
	::RegQueryValueEx( hKey, "HelpPath", 0, &dwType, &bytes[0], &dwSize );

	CString	sRegistryPath( (char *)bytes );

	//query file exist
	char	szDrv[_MAX_DRIVE], szPath[_MAX_DIR], szName[_MAX_FNAME], szExt[_MAX_EXT];
	char	szTryFileName[_MAX_PATH];
	char	szTempDir[_MAX_DIR], szTempDir1[_MAX_DIR];
	char	szResPath[_MAX_PATH];

	const char	cszHelp[] = "Help";

	szResPath[0] = 0;

	::_splitpath( papp->m_pszHelpFilePath, szDrv, szPath, szName, szExt );

//0. from registry
	if( !szResPath[0] && !sRegistryPath.IsEmpty() )
	{
		char	szDrv1[_MAX_DRIVE], szPath1[_MAX_DIR];

		::_splitpath( sRegistryPath, szDrv1, szPath1, 0, 0 );
		::_makepath( szTryFileName, szDrv1, szPath1, szName, szExt );
		if( ::_isfileexist( szTryFileName ) )
			::_makepath( szResPath, szDrv1, szPath1, 0, 0 );
	}

//1. test given filepath
	if( !szResPath[0] )
	{
		::_makepath( szTryFileName, szDrv, szPath, szName, szExt );
		if( ::_isfileexist( szTryFileName ) )
			::_makepath( szResPath, szDrv, szPath, 0, 0 );
	}
//2. try to find file in szDir\\Help
	if( !szResPath[0] )
	{
		::_addsubdir( szTempDir, szPath, cszHelp );
		::_makepath( szTryFileName, szDrv, szTempDir, szName, szExt );
		if( ::_isfileexist( szTryFileName ) )
			::_makepath( szResPath, szDrv, szTempDir, 0, 0 );
	}
//3. try to find it in szDir\\.. 
	if( !szResPath[0] )
	{
		::_deletesubdir( szTempDir, szPath, 1 );
		::_makepath( szTryFileName, szDrv, szTempDir, szName, szExt );
		if( ::_isfileexist( szTryFileName ) )
			::_makepath( szResPath, szDrv, szTempDir, 0, 0 );
	}
//4. try to find it in szDir\\..\\Help
	if( !szResPath[0] )
	{
		::_deletesubdir( szTempDir1, szPath, 1 );
		::_addsubdir( szTempDir, szTempDir1, cszHelp );
		::_makepath( szTryFileName, szDrv, szTempDir, szName, szExt );
		if( ::_isfileexist( szTryFileName ) )
			::_makepath( szResPath, szDrv, szTempDir, 0, 0 );
	}
//5 - finaly, if no help path exist, use from registry
	 if( !szResPath[0] && !sRegistryPath.IsEmpty() )
	{
		char	szDrv1[_MAX_DRIVE], szPath1[_MAX_DIR];

		::_splitpath( sRegistryPath, szDrv1, szPath1, 0, 0 );
		::_makepath( szTryFileName, szDrv1, szPath1, szName, szExt );
		::_makepath( szResPath, szDrv1, szPath1, 0, 0 );
	}
#ifdef __VT4__
//if it is VideoTesT project, setup "HelpPath" value
	if( szResPath[0] != 0 )
		::RegSetValueEx( hKey, "HelpPath", 0, REG_SZ, (const BYTE *)szResPath, ::strlen( szResPath ) );
#endif //__VT4__
	//::RegGet
	::RegCloseKey( hKey );

	if( szResPath[0] != 0 )
	{
		free( (void *)papp->m_pszHelpFilePath );
		papp->m_pszHelpFilePath = ::_tcsdup( szTryFileName );
	}
}

CImageArg res1 =
{
	{sizeof(CImageArg),
	iptImage,
	"Result image",
	"Image on output"},
	NULL
};

CErrorInfo errUserBreak =
{
	sizeof(CErrorInfo),
	3,
	svFatal,
	"User break"
};

class CDIBProvider : public IDIBProvider
{
public:
	void *m_pImg;
	CDIBProvider()
	{
		m_pImg = NULL;
	}
	virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject) {return E_NOTIMPL;}
	virtual ULONG STDMETHODCALLTYPE AddRef( void) {return 0;}
	virtual ULONG STDMETHODCALLTYPE Release( void) {return 0;}
	virtual void *__stdcall AllocMem(DWORD dwSize)
	{
		m_pImg = malloc(dwSize);
		return m_pImg;
	}
};


void DoGrabImage(CDriverPreview *pDrv, CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDIBArgument *pOutImage = (CDIBArgument *)pResult;
	pOutImage->m_fsOpts = 0;//CDIBArgument::Temp;
	BOOL bSilent = false;
	if (pInvokeStruct->m_pszTextParams)
	{
		TCHAR szProfile[200];
		_stscanf(pInvokeStruct->m_pszTextParams, "%d %s", &bSilent, szProfile);
	}
	if (!bSilent)
	{
		if (pDrv->ExecuteSettings(NULL, NULL, 0, 1, 0, TRUE) != S_OK)
		{
//			delete pDrv;
			return;
		}
	}
	CDIBProvider prov;
	HRESULT hr = pDrv->InputDIB(&prov, NULL, 0, TRUE);
	if (SUCCEEDED(hr) && prov.m_pImg)
	{
		pOutImage->m_lpbi = (LPBITMAPINFOHEADER)prov.m_pImg;
		pInvokeStruct->NotifyFunc(0x80000000, pInvokeStruct->NotifyLParam);
	}
//	delete pDrv;
}

FGFormatDescr fds[] =
{
	{0,0,FALSE,""},
};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVT4Drv *CVT4Drv::s_pDrv = NULL;
BOOL CVT4Drv::s_bVT5Settings = FALSE;

CVT4Drv::CVT4Drv()
{
	s_pDrv = this;
	m_pDrvVT5 = NULL;
}

CVT4Drv::~CVT4Drv()
{
	s_pDrv = NULL;
	if (m_pDrvVT5)
		m_pDrvVT5->Release();
}

BOOL CVT4Drv::fgInit()
{
	return TRUE;
}

void CVT4Drv::fgExit()
{
}

BOOL CVT4Drv::fgIsHdwPresent()
{
	return TRUE;
}

UINT CVT4Drv::fgGetHdwFlags()
{
	DWORD dwFlags = FG_HASSETUPDLG|FG_UICONTROLLABLE;
	CDriver *pDrv = GetDriverVT5();
	DWORD dwFlags5;
	if (pDrv != NULL && SUCCEEDED(pDrv->GetFlags(&dwFlags5)) && (dwFlags5&FG_SUPPORTSVIDEO))
		dwFlags |= FG_SUPPORTSAVI;
	return dwFlags;
}

int CVT4Drv::fgGetFormatDescr(FGFormatDescr **fd)
{
	*fd = fds;
	return 1;
}

void CVT4Drv::fgSetFormat(int iNum)
{
}

BYTE *CVT4Drv::fgGetLine(short LineNum)
{
	return NULL;
}

HWND CVT4Drv::fgCreateSetupDialog(HWND hWndParent, BOOL bModal)
{
	CDriver *pDrv = GetDriverVT5();
	s_bVT5Settings = TRUE;
	pDrv->ExecuteSettings(hWndParent, NULL, 0, 1, 0, TRUE); 
	s_bVT5Settings = FALSE;
	return NULL;
}

BOOL CVT4Drv::fgGetBrContrLimits( int *iBrMin, int *iBrMax, int *iCntrMin, int *iCntrMax )
{
	*iBrMin = *iCntrMin = 0;
	*iBrMax = *iCntrMax = 0;
	return TRUE;
}

BOOL CVT4Drv::fgGetBrContr( int *iBr, int *iContr )
{
	*iBr = 0;
	*iContr = 0;
	return FALSE;
}

BOOL CVT4Drv::fgSetBrContr( int iBr, int iContr, BOOL bAuto )
{
	return FALSE;
}

LPCSTR CVT4Drv::fgGetDeviceName()
{
	return "UnknownDevice";
}

int CVT4Drv::fgGetFormat()
{
	return 0;
}

void CVT4Drv::fgSetExposTime( int iTime )
{
}

int CVT4Drv::fgGetExposTime()
{
	return 0;
}

void CVT4Drv::fgForse8bit( BOOL bForce )
{
}

BOOL CVT4Drv::fgIsForsed8bit()
{
	return TRUE;
}

void CVT4Drv::fgSetUIType(int nType)
{
}

void CVT4Drv::fgSetXferType(int nType)
{
}

BOOL CVT4Drv::fgIsReverseBits()
{
	return FALSE;
}

void CVT4Drv::fgRelease()
{
	if (m_pDrvVT5)
	{
		m_pDrvVT5->Release();
		m_pDrvVT5 = NULL;
	}
}

const char *CVT4Drv::GetFunctionName()
{
	return "GrabImage";
}

CDriver *CVT4Drv::GetDriverVT5()
{
	if (m_pDrvVT5 == NULL)
		m_pDrvVT5 = CreateDriverVT5();
	return m_pDrvVT5;
}

int CVT4Drv::CaptureAVI(char *pszNameBuffer, int nBuffSize)
{
	CDriver *pDrv = GetDriverVT5();
	_bstr_t bstrName;
	if (pDrv->InputVideoFile(bstrName.GetAddress(), 0, 1) == S_OK)
	{
		strncpy(pszNameBuffer, (const char *)bstrName, nBuffSize);
		return 0;
	}
	else
		return -1;
}
