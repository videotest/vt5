#include "stdafx.h"
#include "DllInfo.h"
#include "FgDrv.h"
#include "Pixera.h"
#include "DriverPixera.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define std_dll_c	extern "C" __declspec(dllexport) 


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


///////////////////////////////////////////////////////////////////////////////
//
// Information - retrieving functions


const CIntFuncInfo FInfo=
	{0, 0, 6, 0,"PixeraGrabImage","PixeraGrabImage",0};

HANDLE hInst;

CImageArg res1 =
{
	{sizeof(CImageArg),
	iptImage,
	"Result image of function PixeraGrabImage",
	"Image on output"},
	NULL
};


std_dll_c	unsigned DllGetFunctionsCount()
{
	return 1;
}

std_dll_c	unsigned DllGetFunctionInfo(unsigned nFuncNo, CIntFuncInfo *pBuffer)
{
	if (nFuncNo>=1) return 0xFFFFFFFF;
	if (!pBuffer||pBuffer->m_nSize!=sizeof(CIntFuncInfo)) return 0;

	memcpy(pBuffer, &FInfo, pBuffer->m_nSize );

	pBuffer->m_nSize = sizeof(CIntFuncInfo);
	return 0;
}

std_dll_c	unsigned DllGetFuncArgsCount(unsigned nFuncNo)
{
	return 0;
}

std_dll_c	CArgInfo *DllGetFuncArgInfo(unsigned nFuncNo, unsigned nArgNo)
{
//	if (nFuncNo>=FuncNo) return NULL;
//	if (nArgNo>=arrArgsCount[nFuncNo]) return NULL;
//	CArgInfo **ppArgInfo = args[nFuncNo];
//	return ppArgInfo[nArgNo];
	return NULL;
}

std_dll_c	CArgInfo *DllGetFuncResultInfo(unsigned nFuncNo)
{
	return (CArgInfo *)&res1;
}

// Work functions

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


std_dll_c void PixeraGrabImage( CInvokeStruct *pInvokeStruct, void **pArguments, void *pResult, void **pErrInfo )
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDIBArgument *pOutImage = (CDIBArgument *)pResult;
	pOutImage->m_fsOpts = CDIBArgument::Temp;
	bool bSilent = false;
	if (pInvokeStruct->m_pszTextParams)
	{
		TCHAR szProfile[200];
		_stscanf(pInvokeStruct->m_pszTextParams, "%d %s", &bSilent, szProfile);
		/*if (((CBaumerApp *)AfxGetApp())->m_bDiffProf)
		{
			g_BaumerProfile.InitMethodic(szProfile);
			g_CxLibWork.InitCameraSettings(false);
		}*/
	}
	CDriverPixera *pDrv = new CDriverPixera;
	if (!bSilent)
	{
//		_AFX_THREAD_STATE *pThreadState = AfxGetThreadState();
//		pThreadState->m_pRoutingFrame = NULL;
		if (pDrv->ExecuteSettings(NULL, NULL, 0, 1, 0, TRUE) != S_OK)
		{
			delete pDrv;
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
	delete pDrv;
}


class CPixeraDrv : public CFGDrvBase  
{
public:
	CPixeraDrv();
	virtual ~CPixeraDrv();

	virtual BOOL fgInit();
	virtual const char *GetFunctionName();
	virtual BOOL fgIsHdwPresent();
	virtual UINT fgGetHdwFlags();
	// Returns information, saved during capability negotiation.
	virtual int fgGetFormatDescr( FGFormatDescr **fd ); // returns number of formats
	virtual int fgGetFormat();
	virtual void fgSetFormat( int iNum );
	virtual BYTE * fgGetLine( short LineNum );
	virtual BOOL fgGetBrContrLimits( int *iBrMin, int *iBrMax, int *iCntrMin, int *iCntrMax );
	virtual BOOL fgSetBrContr( int iBr, int iContr, BOOL bAuto );
	virtual BOOL fgGetBrContr( int *iBr, int *iContr );
	virtual void fgSetExposTime( int iTime );
	virtual int fgGetExposTime();
	virtual void fgForse8bit( BOOL bForce );
	virtual BOOL fgIsForsed8bit();
	virtual LPCSTR fgGetDeviceName();
	virtual void fgSetUIType(int nType);
	virtual void fgSetXferType(int nType);
	virtual BOOL fgIsReverseBits();

	// Only if GetAquisMech has return Permanently, else setup
	// dialog box already will be created on begin of function.
	// Data source select dialog box will be created inside fgInit.
	virtual HWND fgCreateSetupDialog( HWND hWndParent, BOOL bModal );
	// Here both DS and DSM will be closed.
	virtual void fgExit();
	virtual void fgRelease();
};


FGFormatDescr fds[] =
{
	{1296,1034,TRUE,"Standard"},
	{0,0,FALSE,""},
};

CPixeraDrv g_PixeraDrv;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPixeraDrv::CPixeraDrv()
{

}

CPixeraDrv::~CPixeraDrv()
{

}


extern "C" BOOL _declspec(dllexport) fgInit(HWND hWnd, CFGDrvBase **ptab, int *num,	int mx)
{
	if (*num >= mx) return FALSE;
	ptab[*num] = &g_PixeraDrv;
	(*num)++;
	return TRUE;
}

extern "C" void _declspec(dllexport) fgExit()
{
}


BOOL CPixeraDrv::fgInit()
{
	return TRUE;
}

void CPixeraDrv::fgExit()
{
//	DeinitHardware();
}

BOOL CPixeraDrv::fgIsHdwPresent()
{
//	InitHardware();
	return TRUE;//m_bHdwOK;
}

UINT CPixeraDrv::fgGetHdwFlags()
{
	return FG_HASSETUPDLG|FG_UICONTROLLABLE;
}

int CPixeraDrv::fgGetFormatDescr(FGFormatDescr **fd)
{
	*fd = fds;
	return 1;
}

void CPixeraDrv::fgSetFormat(int iNum)
{
}

BYTE *CPixeraDrv::fgGetLine(short LineNum)
{
	return NULL;
}

HWND CPixeraDrv::fgCreateSetupDialog(HWND hWndParent, BOOL bModal)
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	CDriverPixera *pDrv = new CDriverPixera;
//	_AFX_THREAD_STATE *pThreadState = AfxGetThreadState();
//	pThreadState->m_pRoutingFrame = NULL;
	pDrv->ExecuteSettings(NULL, NULL, 0, 1, 0, TRUE);
	delete pDrv;
	return NULL;
}

BOOL CPixeraDrv::fgGetBrContrLimits( int *iBrMin, int *iBrMax, int *iCntrMin, int *iCntrMax )
{
	*iBrMin = *iCntrMin = 0;
	*iBrMax = *iCntrMax = 0;
	return TRUE;
}

BOOL CPixeraDrv::fgGetBrContr( int *iBr, int *iContr )
{
	*iBr = 0;
	*iContr = 0;
	return FALSE;
}

BOOL CPixeraDrv::fgSetBrContr( int iBr, int iContr, BOOL bAuto )
{
	return FALSE;
}

LPCSTR CPixeraDrv::fgGetDeviceName()
{
	return "Pixera";
}

int CPixeraDrv::fgGetFormat()
{
	return 0;
}

void CPixeraDrv::fgSetExposTime( int iTime )
{
}

int CPixeraDrv::fgGetExposTime()
{
	return 0;
}

void CPixeraDrv::fgForse8bit( BOOL bForce )
{
}

BOOL CPixeraDrv::fgIsForsed8bit()
{
	return TRUE;
}

void CPixeraDrv::fgSetUIType(int nType)
{
}

void CPixeraDrv::fgSetXferType(int nType)
{
}

BOOL CPixeraDrv::fgIsReverseBits()
{
	return FALSE;
}

void CPixeraDrv::fgRelease()
{
}

const char *CPixeraDrv::GetFunctionName()
{
	return "PixeraGrabImage";
}


