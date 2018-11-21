// Register.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "Register.h"
#include "RegisterDlg.h"
#include "NameConsts.h"
#include "Registry.h"
#include "nvCommon.h"

#include "\vt5\shell\lang.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


char	szLogPath[_MAX_PATH] = "";

void WriteLogLine( const char *lpszFormat, ... )
{
	if( !strlen( szLogPath ) )return;

	va_list args;
	va_start(args, lpszFormat);

	HANDLE	hFile = ::CreateFile( szLogPath, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		
		char szBuffer[512];

		_vsntprintf(szBuffer, sizeof(szBuffer), lpszFormat, args);
		strcat( szBuffer, "\r\n" );

		DWORD	dwSize = strlen( szBuffer );
		::SetFilePointer( hFile, 0, 0, FILE_END );
		::WriteFile( hFile, szBuffer, dwSize, &dwSize, 0 );
		::CloseHandle( hFile );
	}
	va_end(args);
}


void RegClean();
UINT WM_FIND_APP = RegisterWindowMessage("SHELL_FIND_APP");


const char szAppGuardFilename[] = "shell.grd";

// rewritable data
// suffix to control's ProgIDs
#ifdef _DEBUG
static char szSuffix[10] = { '_', 'd', 'e', 'b', 'u', 'g', 0, 0, 0, 0};
#else
static char szSuffix[10] = { '_', 'r', 'e', 'l', 'e', 'a', 's', 'e', 0, 0};
#endif //_DEBUG

// guid for crypt translation table file
// {21DADB6D-382C-4a8a-B77A-FD5A60B5FD85}
static GUID guidData = 
{ 0x21dadb6d, 0x382c, 0x4a8a, { 0xb7, 0x7a, 0xfd, 0x5a, 0x60, 0xb5, 0xfd, 0x85 } };

// имитовставка для закриптованного файла
static DWORD dwImito = 0;


/////////////////////////////////////////////////////////////////////////////
// CRegisterApp

BEGIN_MESSAGE_MAP(CRegisterApp, CWinApp)
	//{{AFX_MSG_MAP(CRegisterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRegisterApp construction

CRegisterApp::CRegisterApp()
{
	_CrtSetDbgFlag(0);
	m_strSuffix = szSuffix;
	m_innerclsid = guidData;
	m_dwImito = dwImito;
	m_pTable = 0;//new CTranslateTable();

	m_strGuardAppName = DEF_APP_NAME;

	m_strGuardCompanyName = DEF_COMPANY_NAME;
	m_dwGuardKeyID = 0;
}

CRegisterApp::~CRegisterApp()
{
	if (m_pTable)
		delete m_pTable, m_pTable = 0;
}

void CRegisterApp::GuardGetAppName(char * szAppName, LONG * plSize)
{
	if (plSize)
		*plSize = m_strGuardAppName.GetLength();

	if (AfxIsValidAddress(szAppName, m_strGuardAppName.GetLength() + 1))
		lstrcpy(szAppName, m_strGuardAppName);
}

void CRegisterApp::GuardGetCompanyName(char * szCompanyName, long * plSize)
{
	if (plSize)
		*plSize = m_strGuardCompanyName.GetLength();

	if (AfxIsValidAddress(szCompanyName, m_strGuardCompanyName.GetLength() + 1))
		lstrcpy(szCompanyName, m_strGuardCompanyName);
}

void CRegisterApp::GuardGetSuffix(char * szSuffix, long * plSize)
{
	if (plSize)
		*plSize = m_strSuffix.GetLength();

	if (AfxIsValidAddress(szSuffix, m_strSuffix.GetLength() + 1))
		lstrcpy(szSuffix, m_strSuffix);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRegisterApp object

CRegisterApp theApp;

// export functions that return addreses and sizes of suffix string and guid
// return addreses is image_base(from header) + offset to data
extern "C" _declspec(dllexport) void STDAPICALLTYPE SetData(long * plData, long * plSize)
{
	if (plData) *plData = (long)szSuffix;
	if (plSize) *plSize = 10;
}

extern "C" _declspec(dllexport) void STDAPICALLTYPE GetData(long * plData, long * plSize)
{
	if (plData) *plData = (long)&guidData;
	if (plSize) *plSize = sizeof(guidData);
}

extern "C" _declspec(dllexport) void STDAPICALLTYPE InitData(long * plData, long * plSize)
{
	if (plData) *plData = (long)&dwImito;
	if (plSize) *plSize = sizeof(dwImito);
}

extern "C" _declspec(dllexport) void GuardGetNSKInfo(DWORD * pdwKeyID)
{
	theApp.GuardGetNSKInfo(pdwKeyID);
}

extern "C" _declspec(dllexport) void GuardGetAppName(char * szAppName, LONG * plSize)
{
	theApp.GuardGetAppName(szAppName, plSize);
}

extern "C" _declspec(dllexport) void GuardGetCompanyName(char * szCompanyName, long * plSize)
{
	theApp.GuardGetCompanyName(szCompanyName, plSize);
}

extern "C" _declspec(dllexport) void GuardGetSuffix(char * szSuffix, long * plSize)
{
	theApp.GuardGetSuffix(szSuffix, plSize);
}


extern "C" _declspec(dllexport) void GuardSetNSKInfo(DWORD * pdwKeyID)
{
	theApp.GuardSetNSKInfo(pdwKeyID);
}

extern "C" _declspec(dllexport) void GuardSetAppName(const char * szAppName)
{
	theApp.GuardSetAppName(szAppName);
}

extern "C" _declspec(dllexport) void GuardSetCompanyName(const char * szCompanyName)
{
	theApp.GuardSetCompanyName(szCompanyName);
}

extern "C" _declspec(dllexport) void GuardSetSuffix(const char * szSuffix)
{
	theApp.GuardSetSuffix(szSuffix);
}

extern "C" _declspec(dllexport) IUnknown * GuardGetAppUnknown(bool bAddRef)
{
	IUnknown * punk = theApp.GetControllingUnknown();
	if (punk && bAddRef)
		punk->AddRef();

	return punk;
}



#ifndef EXTERN_C
#define EXTERN_C extern "C"
#endif// EXTERN_C

// own functions to replace standard function in ole32.dll
#define HANDLER_STDAPI EXTERN_C HRESULT STDAPICALLTYPE
/*
HANDLER_STDAPI CoCreateInstanceNew(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv);
HANDLER_STDAPI CoGetClassObjectNew(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv);
HANDLER_STDAPI ProgIDFromCLSIDNew(REFCLSID clsid, LPOLESTR FAR* lplpszProgID);
HANDLER_STDAPI CLSIDFromProgIDNew(LPCOLESTR lpszProgID, LPCLSID lpclsid);

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
// CoCreateInstanceEx
HANDLER_STDAPI CoCreateInstanceExNew(REFCLSID rclsid, LPUNKNOWN punkOuter, DWORD dwClsCtx, COSERVERINFO * pServerInfo, ULONG cmq, MULTI_QI * pResults);
#endif
*/

HANDLER_STDAPI CoCreateInstanceShell(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;
	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	IVTApplication * pApp = 0; 
	IUnknown	*pAppUnknown = GuardGetAppUnknown(false);

	if( !pAppUnknown )
		return E_INVALIDARG;

	if (FAILED(GuardGetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
		return E_INVALIDARG;

	// get table
	BYTE * pbTable = 0;
	hr = pApp->GetData(0, &pbTable, 0, 0);
	pApp->Release();

	CTranslateTable * pTable = (CTranslateTable*)pbTable;
	TTranslateEntry * pEntry = 0;
	if (pTable)
	{
		// get entry by extern
		pEntry = pTable->GetEntry(extGuid);
		if (pEntry) // if not found -> try to get entry by internal
		{
			intGuid = pTable->DecryptEntry(pEntry);
		}
		else
		{
			extGuid = pTable->GetExtern(intGuid);
			pEntry = extGuid != INVALID_KEY ? pTable->GetEntry(extGuid) : 0;
		}
	}
	// if entry not founded
	if (!pEntry || FAILED(hr)) // not own control
	{
		BYTE * pCode = (BYTE*)GetDataCreateInstance();
		DWORD dw = 0;
		memcpy(&dw, &pCode[8], sizeof(DWORD));
		if (dw)
		{
			// restore original handler
			memcpy((BYTE*)dw, &pCode[1], 7);

			// simply call CoCreateInstance function
			typedef HRESULT (_stdcall *PFN)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR* );
			PFN pfn = (PFN)dw;
			hr = pfn(rclsid, pUnkOuter, dwClsContext, riid, ppv);

			// set new handler again
			BYTE * ptr = (BYTE*)dw;
			*ptr = 0xB8;ptr++; // mov eax, &data
			memcpy(ptr, &pCode[12], sizeof(DWORD));
			ptr += 4;
			*ptr = 0xFF;ptr++; // jmp eax
			*ptr = 0xE0;ptr++; // 
		}
	
		return hr;
	}

	BSTR bstrCLSID = 0;
	if (FAILED(StringFromCLSID(extGuid, &bstrCLSID)) || !bstrCLSID)
		return REGDB_E_CLASSNOTREG;

	CString strCLSID = bstrCLSID;
	::CoTaskMemFree(bstrCLSID);
	strCLSID = _T("CLSID\\") + strCLSID + _T("\\InprocServer32");

	CVTRegKey regKey;
	if (!regKey.Open(HKEY_CLASSES_ROOT, strCLSID))
		return REGDB_E_CLASSNOTREG;

	CVTRegValue val;
	if (!regKey.GetValue(val))
		return REGDB_E_CLASSNOTREG;

	_bstr_t bstrModule(val.GetString());
	if (!bstrModule.length())
		return REGDB_E_READREGDB;

	HINSTANCE hDll = ::CoLoadLibrary(bstrModule, true);
	if (!hDll)
		return TYPE_E_CANTLOADLIBRARY;

	{
		if (hDll == GetModuleHandle(0)) // own factory
		{
			hr = REGDB_E_CLASSNOTREG;

			AFX_MODULE_STATE* pModuleState = AfxGetAppModuleState();
			// for all factories
			for (COleObjectFactory* pFactory = pModuleState->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
			{
				// if founded needed factory
				if (pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)) && ((CVTFactory*)pFactory)->GetClassID() == intGuid)
				{
					IUnknown * punk = pFactory->GetControllingUnknown();
					if (punk)
					{
						IVTClass * ptrVT = 0;
						if (SUCCEEDED(punk->QueryInterface(IID_IVTClass, (LPVOID*)&ptrVT)) && ptrVT)
						{
							hr = ptrVT->VTCreateObject(pUnkOuter, 0, riid, 0, ppv);
							ptrVT->Release();
							break;
						}
					}
				}
			}
		}
		else
		{
			typedef HRESULT (_stdcall *PFN)(REFCLSID, REFIID, LPVOID*) ;
			PFN pfn = (PFN)GetProcAddress(hDll, _T("DllGetClassObject"));
			if (pfn)
			{
				IVTClass * ptrVT = 0;
				if (SUCCEEDED(pfn(intGuid, IID_IVTClass, (LPVOID*)&ptrVT)) && ptrVT)
				{
	//				rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv
					hr = ptrVT->VTCreateObject(pUnkOuter, 0, riid, 0, ppv);
					ptrVT->Release();
				}
				else
				{
					IClassFactory * pFact = 0;
					if (SUCCEEDED(pfn(intGuid, IID_IClassFactory, (LPVOID*)&pFact)) && pFact)
					{
						hr = pFact->CreateInstance(pUnkOuter, riid,  ppv);
						pFact->Release();
					}
				}
			}
		}		
//		::CoFreeLibrary(hDll);
	}
	return hr;

}

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
HANDLER_STDAPI CoCreateInstanceExShell(REFCLSID rclsid, LPUNKNOWN punkOuter, DWORD dwClsCtx, COSERVERINFO * pServerInfo, ULONG cmq, MULTI_QI * pResults)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;

	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	IVTApplication * pApp = 0; 
	if (FAILED(GuardGetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
		return E_INVALIDARG;

	// get table
	BYTE * pbTable = 0;
	hr = pApp->GetData(0, &pbTable, 0, 0);
	pApp->Release();

	CTranslateTable * pTable = (CTranslateTable*)pbTable;
	TTranslateEntry * pEntry = 0;
	if (pTable)
	{
		// get entry by extern
		pEntry = pTable->GetEntry(extGuid);
		if (pEntry) // if not found -> try to get entry by internal
		{
			intGuid = pTable->DecryptEntry(pEntry);
		}
		else
		{
			extGuid = pTable->GetExtern(intGuid);
			pEntry = extGuid != INVALID_KEY ? pTable->GetEntry(extGuid) : 0;
		}
	}
	// if entry not founded
	if (!pEntry || FAILED(hr)) // not own control
	{
		BYTE * pCode = (BYTE*)GetDataCreateInstanceEx();
		DWORD dw = 0;
		memcpy(&dw, &pCode[8], sizeof(DWORD));
		if (dw)
		{
			// restore original handler
			memcpy((BYTE*)dw, &pCode[1], 7);

			// simply call CoCreateInstanceEx function
			typedef HRESULT (_stdcall *PFN)(REFCLSID, LPUNKNOWN, DWORD, COSERVERINFO*, ULONG, MULTI_QI*);
			PFN pfn = (PFN)dw;
			hr = pfn(rclsid, punkOuter, dwClsCtx, pServerInfo, cmq, pResults);

			// set new handler again
			BYTE * ptr = (BYTE*)dw;
			*ptr = 0xB8;ptr++; // mov eax, &data
			memcpy(ptr, &pCode[12], sizeof(DWORD));
			ptr += 4;
			*ptr = 0xFF;ptr++; // jmp eax
			*ptr = 0xE0;ptr++; // 
		}
		return hr;
	}
	if (pServerInfo || cmq <= 0 || !pResults)
		return E_INVALIDARG;

	IUnknown * punk = 0;
	hr = CoCreateInstanceShell(rclsid, punkOuter, dwClsCtx, IID_IUnknown, (LPVOID*)&punk);
	if (FAILED(hr) || !punk)
		return hr;

#if _MSC_VER >= 1300
	for (ULONG i = 0; i < cmq; i++)
		pResults->hr = punk->QueryInterface(*pResults->pIID, (LPVOID*)&(pResults->pItf));
#else
	for (ULONG i = 0; i < cmq; i++)
		pResults->hr = punk->QueryInterface(pResults->pIID, (LPVOID*)&(pResults->pItf));
#endif

	punk->Release();
	return hr;

}
#endif // DCOM

HANDLER_STDAPI CoGetClassObjectShell(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;
	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	IVTApplication * pApp = 0; 
	if (FAILED(GuardGetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
		return E_INVALIDARG;

	// get table
	BYTE * pbTable = 0;
	hr = pApp->GetData(0, &pbTable, 0, 0);
	pApp->Release();

	CTranslateTable * pTable = (CTranslateTable*)pbTable;
	TTranslateEntry * pEntry = 0;
	if (pTable)
	{
		// get entry by extern
		pEntry = pTable->GetEntry(extGuid);
		if (pEntry) // if not found -> try to get entry by internal
		{
			intGuid = pTable->DecryptEntry(pEntry);
		}
		else
		{
			extGuid = pTable->GetExtern(intGuid);
			pEntry = extGuid != INVALID_KEY ? pTable->GetEntry(extGuid) : 0;
		}
	}

	// if entry not founded
	if (!pEntry || FAILED(hr)) // not own control
	{
		BYTE * pCode = (BYTE*)GetDataGetClassObject();
		DWORD dw = 0;
		memcpy(&dw, &pCode[8], sizeof(DWORD));
		if (dw)
		{
			// restore original handler
			memcpy((BYTE*)dw, &pCode[1], 7);

			// simply call CoGetClassObject function
			typedef HRESULT (_stdcall *PFN)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*) ;
			PFN pfn = (PFN)dw;
			hr = pfn(rclsid, dwClsContext, pvReserved, riid, ppv);

			// set new handler again
			BYTE * ptr = (BYTE*)dw;
			*ptr = 0xB8;ptr++; // mov eax, &data
			memcpy(ptr, &pCode[12], sizeof(DWORD));
			ptr += 4;
			*ptr = 0xFF;ptr++; // jmp eax
			*ptr = 0xE0;ptr++; // 
		}
		return hr;
	}

	BSTR bstrCLSID = 0;
	if (FAILED(StringFromCLSID(extGuid, &bstrCLSID)) || !bstrCLSID)
		return REGDB_E_CLASSNOTREG;

	CString strCLSID = bstrCLSID;
	::CoTaskMemFree(bstrCLSID);
	strCLSID = _T("CLSID\\") + strCLSID + _T("\\InprocServer32");

	CVTRegKey regKey;
	if (!regKey.Open(HKEY_CLASSES_ROOT, strCLSID))
		return REGDB_E_CLASSNOTREG;

	CVTRegValue val;
	if (!regKey.GetValue(val))
		return REGDB_E_CLASSNOTREG;

	_bstr_t bstrModule(val.GetString());
	if (!bstrModule.length())
		return REGDB_E_READREGDB;

	HINSTANCE hDll = ::CoLoadLibrary(bstrModule, true);
	if (hDll)
	{
		if (hDll == GetModuleHandle(0)) // own factory
		{
			hr = REGDB_E_CLASSNOTREG;

			AFX_MODULE_STATE* pModuleState = AfxGetAppModuleState();
			// for all factories
			for (COleObjectFactory* pFactory = pModuleState->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
			{
				// if founded needed factory
				if (pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)) && ((CVTFactory*)pFactory)->GetClassID() == intGuid)
				{
					IUnknown * punk = pFactory->GetControllingUnknown();
					if (punk)
						return punk->QueryInterface(riid, ppv);
				}
			}
		}
		else
		{
			typedef HRESULT (_stdcall *PFN)(REFCLSID, REFIID, LPVOID*) ;
			PFN pfn = (PFN)GetProcAddress(hDll, _T("DllGetClassObject"));
			if (pfn)
				return pfn(intGuid, riid, ppv);
		}		
//		::CoFreeLibrary(hDll);
	}
	return REGDB_E_CLASSNOTREG;
}

HANDLER_STDAPI ProgIDFromCLSIDShell(REFCLSID rclsid, LPOLESTR FAR* lplpszProgID)
{
	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	if (!lplpszProgID)
		return E_POINTER;

	IVTApplication * pApp = 0; 
	if (FAILED(GuardGetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
		return E_INVALIDARG;

	// get table
	BYTE * pbTable = 0;
	HRESULT hr = pApp->GetData(0, &pbTable, 0, 0);
	pApp->Release();

	CTranslateTable * pTable = (CTranslateTable*)pbTable;
	if (pTable)
	{
		// get entry by extern
		TTranslateEntry * pEntry = pTable->GetEntry(extGuid);
		if (!pEntry) // if not found -> try to get entry by internal
		{
			extGuid = pTable->GetExtern(intGuid);
			pEntry = extGuid != INVALID_KEY ? pTable->GetEntry(extGuid) : 0;
		}
		// if entry founded
		if (pEntry)
		{
			// we want to convert an MBCS string in lpszA
			int nLen = MultiByteToWideChar(CP_ACP, 0, pEntry->szProgID, -1, NULL, NULL);
			*lplpszProgID = (LPOLESTR)::CoTaskMemAlloc(nLen * sizeof(WCHAR));
			MultiByteToWideChar(CP_ACP, 0, pEntry->szProgID, -1, *lplpszProgID, nLen);
			return S_OK;
		}
	}

	hr = REGDB_E_CLASSNOTREG;
	BYTE * pCode = (BYTE*)GetDataProgIDFromCLSID();
	DWORD dw = 0;
	memcpy(&dw, &pCode[8], sizeof(DWORD));
	if (dw)
	{
		// restore original handler
		memcpy((BYTE*)dw, &pCode[1], 7);

		// simply call ProgIDFromCLSID function
		typedef HRESULT (_stdcall *PFN)(REFCLSID, LPOLESTR FAR*);
		PFN pfn = (PFN)dw;
		hr = pfn(rclsid, lplpszProgID);

		// set new handler again
		BYTE * ptr = (BYTE*)dw;
		*ptr = 0xB8;ptr++; // mov eax, &data
		memcpy(ptr, &pCode[12], sizeof(DWORD));
		ptr += 4;
		*ptr = 0xFF;ptr++; // jmp eax
		*ptr = 0xE0;ptr++; // 
	}
	
	return hr;
}

HANDLER_STDAPI CLSIDFromProgIDShell(LPCOLESTR lpszProgID, LPCLSID lpclsid)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;

	// interaction w/ translate_table
	if (lpclsid)
	{
		IVTApplication * pApp = 0; 
		if (SUCCEEDED(GuardGetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) && pApp)
		{
			_bstr_t bstr(lpszProgID);
			GuidKey extGuid;
			hr = pApp->GetModeData(0, (DWORD*)&extGuid, bstr);
			pApp->Release();
			if (SUCCEEDED(hr) && extGuid != INVALID_KEY)
			{
				*lpclsid = extGuid;
				return S_OK;
			}
		}
	}

	hr = REGDB_E_CLASSNOTREG;
	BYTE * pCode = (BYTE*)GetDataCLSIDFromProgID();
	DWORD dw = 0;
	memcpy(&dw, &pCode[8], sizeof(DWORD));
	if (dw)
	{
		// restore original handler
		memcpy((BYTE*)dw, &pCode[1], 7);

		// simply call CLSIDFromProgID function
		typedef HRESULT (_stdcall *PFN)(LPCOLESTR, LPCLSID);
		PFN pfn = (PFN)dw;
		hr = pfn(lpszProgID, lpclsid);

		// set new handler again
		BYTE * ptr = (BYTE*)dw;
		*ptr = 0xB8;ptr++; // mov eax, &data
		memcpy(ptr, &pCode[12], sizeof(DWORD));
		ptr += 4;
		*ptr = 0xFF;ptr++; // jmp eax
		*ptr = 0xE0;ptr++; // 
	}
	
	return hr;
}




/////////////////////////////////////////////////////////////////////////////
// CRegisterApp initialization
bool	bClearReg=  true;
bool	bReportErrors = false;
bool	bRegister = true;
bool	bShowInterface = false;
extern char	g_szTaskFileName[];

BOOL CRegisterApp::InitInstance()
{
	AfxEnableControlContainer();

	//set current directory
	LANGID	langid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
	char	sz[_MAX_PATH];
	::GetModuleFileName( m_hInstance, sz, _MAX_PATH );
	*strrchr( sz, '\\' ) = 0;
	::SetCurrentDirectory( sz );

	strcpy( szLogPath, sz );
	strcat( szLogPath, "\\register.log" );

	::DeleteFile( szLogPath );

	//parse command line
	const char *psz = m_lpCmdLine;

	if( strstr( psz, "/NoClearReg" ) )bClearReg = false;
	if( strstr( psz, "/ReportError" ) )bReportErrors = true;
	if( strstr( psz, "/NoRegister" ) )bRegister = false;
	if( strstr( psz, "/ShowDialog" ) )bShowInterface = true;
	if( strstr( psz, "/help" )||strstr( psz, "/?" )||strstr( psz, "-?" ) )
	{
		AfxMessageBox( "Command line parameters:\nNoClearReg\t-Don't clear registry before register\n\
ReportError\t-Display modal error report\n\
NoRegister\t-Don't register components, only clear registry\n\
ShowDialog\t-Show modal dialog with register settings\n\
Lang:XXXX-Language. Example.Lang:1049-RUSSIAN. Localization for .ocx only\n\
Task:<filename>\t-Specify the file with list of dlls to register" );
		return false;
	}
	char *plang = strstr( (char*)psz, "/Lang:" );
	if( plang )
	{
		plang+=6;
		sscanf( plang, "%d", &langid );
	}
	else
	{
		char	szIniFileName[MAX_PATH];
		::GetModuleFileName( 0, szIniFileName, MAX_PATH );
		char	*p = strrchr( szIniFileName, '\\' );
		strcpy( p, "\\shell.data" );

		char	sz[10] = "en";
		::GetPrivateProfileString( "General", "Language:String", sz, sz, 10, szIniFileName );		
		
		if( ::GetPrivateProfileInt( "General", "UseLanguage:Long", 2, szIniFileName ) )
		{		
			if( !stricmp( sz, "ru" ) )langid = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
			else if( !stricmp( sz, "en" ) )langid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
			else if( !stricmp( sz, "ch" ) ) langid = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED );
		}
	}

	char	*p;
	if( p = strstr( (char*)psz, "/Task:" ) )
	{
		p+= 6;
		char	*pe = strchr( p, ' ' );
		if( !pe )pe = strchr( p, '/' );
		if( !pe )pe = p+strlen( p );

		long	len = (long)pe-long(p);
		strncpy( g_szTaskFileName, p, len );
		g_szTaskFileName[len] = 0;
	}

	WriteLogLine( "Registering components using settings" );
	WriteLogLine( "  bClearReg=%d", bClearReg );
	WriteLogLine( "  bReportErrors=%d", bReportErrors );
	WriteLogLine( "  bRegister=%d", bRegister );
	WriteLogLine( "  bShowInterface=%d", bShowInterface );


//	InitLanguageSupport( langid );
	
	

	if( bClearReg )
	{
		WriteLogLine( "Clearing registry..." );
		RegClean();
		WriteLogLine( "Clearing done" );
	}
	if( !bRegister )
		return false;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
		// check app is running
	if (GuardAppIsRunning())
	{
		// send broadcast message to all apps 
		DWORD dwRecipients = BSM_APPLICATIONS;   
		::BroadcastSystemMessage(BSF_POSTMESSAGE|BSF_IGNORECURRENTTASK, &dwRecipients, WM_FIND_APP, NULL, NULL);
		// and exit
		return FALSE;
	}
	if (!::StealthInit())
		return false;


	DefaultInit();
	// load saved last info from  registry
	//LoadRegisterData(m_regData);

	CString strFilePath(szAppGuardFilename);

	{
		CString str = m_regData.strPath;
		str.MakeUpper();
		// set variables
		int nPos = str.Find("\\COMPS");
		strFilePath = m_regData.strPath;
		if (nPos != -1)
		{
			strFilePath = m_regData.strPath.Left(nPos);
			m_regData.strPath = strFilePath;
		}

		if (strFilePath.GetLength() && strFilePath[strFilePath.GetLength() - 1] != '\\')
			strFilePath += '\\';

		strFilePath += szAppGuardFilename;
	}
	if (!ReadGuardInfo(strFilePath))
	{
		if( bReportErrors )
			AfxMessageBox(IDS_GUARD_FILE_READ_FAILED);
	}

	CoInitialize(0);
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(m_strGuardCompanyName);

	//initialize the global reference to the application's unknown
//	InitAppUnknown(GetControllingUnknown());

	// load saved last info from  registry

//	LoadRegisterData(m_regData);
	if (!m_dwGuardKeyID)
	{
		nvInitialize(0, 0, 5);
		KeyInfo Info;
		if (GetFirstStealthKey(&Info))
			m_dwGuardKeyID = Info.dwID;
	}

	// try to find key
	if (m_dwGuardKeyID)
	{
		// update key info
		if (!StealthUpdate(m_dwGuardKeyID))
		{
			CoUninitialize();
			::StealthDeinit();
			return false;
		}

		GUID guid = INVALID_KEY;
		StealthReadGUID(&guid);
		if( guid != INVALID_KEY )
			m_innerclsid = guid;
		else
		{
			m_dwGuardKeyID = 0;
			WriteLogLine( "Error:Found not our Stealth Key." );
		}

		

	}

	// create table
	m_pTable = new CTranslateTable;
	if (!m_pTable)
	{
		CoUninitialize();
		::StealthDeinit();
		return FALSE;
	}
	m_pTable->SetKey(GuidKey(m_innerclsid), 0);
	m_pTable->Load(strFilePath);


	// set our functions instead standard

	bool bCreateInstance = false;//InstallCreateInstance((LPVOID)&CoCreateInstanceShell);

	bool bGetClassObject = false;//InstallGetClassObject((LPVOID)&CoGetClassObjectShell);

	bool bCLSIDFromProgID = false;//InstallCLSIDFromProgID((LPVOID)&CLSIDFromProgIDShell);

	bool bProgIDFromCLSID = false;//InstallProgIDFromCLSID((LPVOID)&ProgIDFromCLSIDShell);


#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	bool bCreateInstanceEx = false;//InstallCreateInstanceEx((LPVOID)&CoCreateInstanceExShell);

#endif // DCOM

	// and set table to dlg

	if( bShowInterface )
	{
		CRegisterDlg dlg(&m_regData);
		m_pMainWnd = &dlg;
		int nResponse = dlg.DoModal();
	}
	else
	{
		WriteLogLine( "Registering components..." );

		{
			AFX_MANAGE_STATE(AfxGetModuleState());
		}
		CRegisterDlg* pdlg = new CRegisterDlg( &m_regData );
		m_pMainWnd = pdlg;
		pdlg->Create( IDD_REGISTER_LITE, 0 );
		pdlg->UpdateWindow();
		pdlg->OnRegister();		

		pdlg->EndDialog( IDOK );

		//try{
		//if( ::IsWindow( pdlg->m_hWnd ) )
		//	pdlg->DestroyWindow( );
		//}
		//catch(...){}

		WriteLogLine( "Registering done" );
		
		AFX_MANAGE_STATE(AfxGetModuleState());

		delete pdlg;

		//m_pMainWnd = 0;
	}


	WriteLogLine( "Saving configuration..." );

	// set valiables
	{
		CString str = m_regData.strPath;
		str.MakeUpper();
		// set variables
		int nPos = str.Find("\\COMPS");
		strFilePath = m_regData.strPath;
		if (nPos != -1)
		{
			strFilePath = m_regData.strPath.Left(nPos);
			m_regData.strPath = strFilePath;
		}

		if (strFilePath.GetLength() && strFilePath[strFilePath.GetLength() - 1] != '\\')
			strFilePath += '\\';

		strFilePath += szAppGuardFilename;
	}
	SaveRegisterData(m_regData);
	// save table
	VERIFY(m_pTable->Save(strFilePath));
	VERIFY(m_pTable->SaveText("shell.grd.txt"));

	// remove table 
	delete m_pTable, m_pTable = 0;

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	if (bCreateInstanceEx)
		UninstallCreateInstanceEx();

#endif // DCOM

	if (bProgIDFromCLSID)
		UninstallProgIDFromCLSID();

	if (bCLSIDFromProgID)
		UninstallCLSIDFromProgID();

	if (bGetClassObject)
		UninstallGetClassObject();

	if (bCreateInstance)
		UninstallCreateInstance();

	::StealthDeinit();
	CoUninitialize();
	
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CRegisterApp message handlers
BEGIN_INTERFACE_MAP(CRegisterApp, CWinApp)
  INTERFACE_PART(CRegisterApp, IID_IVTApplication, Guard)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CRegisterApp, Guard)

// IVTApplication

HRESULT CRegisterApp::XGuard::InitApp( BYTE* pBuf, DWORD dwData )
{
	_try_nested(CRegisterApp, Guard, InitApp)
	{
		return S_OK;
	}
	_catch_nested;
}

// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
HRESULT CRegisterApp::XGuard::GetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito)
{
	_try_nested(CRegisterApp, Guard, GetData)
	{
		if (pKeyGUID)
			*((GUID*)pKeyGUID) = pThis->m_innerclsid;	

		if (pTable)
			*pTable = (BYTE*)pThis->m_pTable;
		
		if (pbstrSuffix)
		{
			CString str_suffix = "_";
			str_suffix += pThis->m_strGuardAppName;
			*pbstrSuffix = str_suffix.AllocSysString();/*m_strSuffix*/
		}

		if (pdwImito)
			*pdwImito = pThis->m_dwImito;

		return S_OK;
	}
	_catch_nested;
}

// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
HRESULT CRegisterApp::XGuard::SetData(DWORD * pKeyGUID, BYTE ** pTable, BSTR * pbstrSuffix, DWORD * pdwImito)
{
	_try_nested(CRegisterApp, Guard, SetData)
	{
		if (pKeyGUID)
			pThis->m_innerclsid = *((GUID*)pKeyGUID);	

//		if (pTable)
//			pTable = (BYTE*)pThis->m_pTable;

		if (pbstrSuffix)
			pThis->m_strSuffix = *pbstrSuffix;

		if (pdwImito)
			pThis->m_dwImito = *pdwImito;

		return S_OK;
	}
	_catch_nested;
}

// get mode (register or not)
HRESULT CRegisterApp::XGuard::GetMode(BOOL * pbRegister)
{
	_try_nested(CRegisterApp, Guard, GetMode)
	{
		if (!pbRegister)
			return E_POINTER;

		*pbRegister = true;
		return S_OK;
	}
	_catch_nested;
}


// Add guid to table
HRESULT CRegisterApp::XGuard::AddEntry(DWORD * pIntGUID, DWORD * pExtGUID, BSTR bstrIntProgID, DWORD dwData)
{
	_try_nested(CRegisterApp, Guard, AddEntry)
	{
		CString str = bstrIntProgID;
		if (!pIntGUID || !pExtGUID || str.IsEmpty())
			return E_POINTER;

		CTranslateTable * pTable  = pThis->m_pTable;
		if (!pTable)
			return E_FAIL;

		return pTable->Add(GuidKey(*((GUID*)pExtGUID)), GuidKey(*((GUID*)pIntGUID)), str, dwData) ? S_OK : S_FALSE;
	}
	_catch_nested;
}

// Remove guid from table
HRESULT CRegisterApp::XGuard::RemoveEntry(DWORD * pExtGUID, BSTR bstrIntProgID)
{
	_try_nested(CRegisterApp, Guard, RemoveEntry)
	{
		CString str = bstrIntProgID;
		CTranslateTable * pTable  = pThis->m_pTable;
		if (!pTable)
			return E_FAIL;

		bool bRet = (pExtGUID) ? pTable->Remove(GuidKey(*((GUID*)pExtGUID))) : (!str.IsEmpty() ? pTable->Remove(str) : false);

		return bRet ? S_OK : E_INVALIDARG;
	}
	_catch_nested;
}

// get inner guid from table by extern guid	: mode 0
// get extern guid by inner guid			: mode 1
HRESULT CRegisterApp::XGuard::GetEntry(DWORD dwMode, DWORD * pSrcGUID, DWORD * pDstGUID, DWORD * pdwData)
{
	_try_nested(CRegisterApp, Guard, GetEntry)
	{
		if (!pSrcGUID)
			return E_POINTER;

		CTranslateTable * pTable  = pThis->m_pTable;
		if (!pTable)
			return E_FAIL;

		TTranslateEntry * pEntry = 0;
		if (dwMode == 0)
			pEntry =  pTable->GetEntry(*((GUID*)pSrcGUID));

		else if (dwMode == 1)
		{
			GuidKey guidExtern = pTable->GetExtern(*((GUID*)pSrcGUID));
			pEntry =  pTable->GetEntry(guidExtern);
		}

		if (!pEntry)
			return E_INVALIDARG;

		if (pDstGUID)
		{
			GuidKey key = dwMode == 0 ? pTable->DecryptEntry(pEntry) : 
						  dwMode == 1 ? pEntry->GuidExtern : INVALID_KEY;

			memcpy(pDstGUID, (DWORD*)&key, sizeof(GUID));
		}

		if (pdwData)
			*pdwData = pEntry->dwData;

		return S_OK;
	}
	_catch_nested;
}


// Get External GUID from internal/external ProgID : mode 0
// Get internal GUID from internal/external ProgID : mode 1
HRESULT CRegisterApp::XGuard::GetModeData(DWORD dwMode, DWORD * pGUID, BSTR bstrProgID)
{
	_try_nested(CRegisterApp, Guard, GetModeData)
	{
		CString strProgID(bstrProgID);

		if (strProgID.IsEmpty() || !pGUID)
			return E_INVALIDARG;
	
		CTranslateTable * pTable = pThis->m_pTable;
		if (!pTable)
			return E_FAIL;

		CString strSuff(pThis->m_strSuffix);

		// calc diff of given progID length & suffix length
		int nDiff = strProgID.GetLength() - strSuff.GetLength();
		if (nDiff > 0)
		{
			// try to find suffix in given string
			int nPos = strProgID.Find(strSuff, nDiff);
			// if object has suffix we need to remove suffix from it
			if (nPos != -1)
				strProgID.Delete(nPos, strProgID.GetLength() - nPos);
		}

		// in this place we have inner ProgID
		GuidKey guid;
		switch (dwMode)
		{
			case 0:
				guid = pTable->GetExtern(strProgID);
				break;
			case 1:
				guid = pTable->Get(strProgID);
				break;
		}
	
		*((GUID*)pGUID) = guid;

		return S_OK;
	}
	_catch_nested;
}


// translations	
// get external ProgID from internal ProgID : mode 0
// get internal ProgID from external ProgID : mode 1
HRESULT CRegisterApp::XGuard::GetDataValue(DWORD dwMode, BSTR bstrSrcProgID, BSTR * pbstrDstProgID)
{
	_try_nested(CRegisterApp, Guard, GetDataValue)
	{
		CString strProgID(bstrSrcProgID);
		CString strRet(bstrSrcProgID); // return string

		if (strProgID.IsEmpty() || !pbstrDstProgID)
			return E_INVALIDARG;
	
		CString strSuff(pThis->m_strSuffix);

		// calc diff of given progID length & suffix length
		int nDiff = strProgID.GetLength() - strSuff.GetLength();
		if (nDiff > 0)
		{
			// try to find suffix in given string
			int nPos = strProgID.Find(strSuff, nDiff);
			// if object has suffix we need to remove suffix from it
			if (nPos != -1)
				strProgID.Delete(nPos, strProgID.GetLength() - nPos);
		}

		CTranslateTable * pTable = pThis->m_pTable;
		if (!pTable)
			return E_FAIL;

		if (INVALID_KEY != pTable->GetExtern(strProgID))
		{
			// found

			if (dwMode == 0)// need add suffix 
				strRet += strSuff;
		}
		*pbstrDstProgID = strRet.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CRegisterApp::XGuard::GetNames(BSTR * pbstrAppName, BSTR * pbstrCompanyName)
{
	_try_nested(CRegisterApp, Guard, GetNames)
	{
		if (pbstrAppName)
			*pbstrAppName = pThis->m_strGuardAppName.AllocSysString();
		if (pbstrCompanyName)
			*pbstrCompanyName = pThis->m_strGuardCompanyName.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CRegisterApp::XGuard::GetRegKey(BSTR * pbstrRegKey)
{
	_try_nested(CRegisterApp, Guard, GetRegKey)
	{
		if (!pbstrRegKey)
			return E_POINTER;

		CString strRet("SOFTWARE\\");
		strRet += pThis->m_strGuardCompanyName + _T("\\");
		strRet += pThis->m_strGuardAppName + _T("\\add-ins");

		*pbstrRegKey = strRet.AllocSysString();
		return S_OK;
	}
	_catch_nested;
}



////

bool CRegisterApp::ReadGuardInfo(LPCTSTR szFile)
{
	CString strFile(szFile);
	if (strFile.IsEmpty())
		strFile = szAppGuardFilename;

	try
	{
		CFile file(strFile, CFile::modeRead|CFile::shareDenyRead);
	
		// read signature
		int nSigSize = lstrlen(szGUARDFILESIG);
		CString strSig;

		file.Read(strSig.GetBuffer(nSigSize + 1), nSigSize);
		strSig.ReleaseBuffer( nSigSize );
		if (strSig.GetLength() > nSigSize)
			strSig.SetAt(nSigSize, '\0');

		if (lstrcmp(strSig, szGUARDFILESIG))
		{
			GuardSetErrorCode(guardInvalidGuardFile);
			return false;
		}
		// read size of app_name string
		LONG lNameSize = 0;
		file.Read(&lNameSize, sizeof(LONG));

		CString strAppName(m_strGuardAppName);
		if (lNameSize)
		{
			// save app_name_string
			file.Read(strAppName.GetBuffer(lNameSize + 1), lNameSize);
			strAppName.ReleaseBuffer( lNameSize );
			if (strAppName.GetLength() > lNameSize)
				strAppName.SetAt(lNameSize, '\0');

		}
		GuardSetAppName(strAppName);

		// Read NSK info 
		DWORD dwKeyId = m_dwGuardKeyID;

		file.Read(&dwKeyId, sizeof(DWORD));
		GuardSetNSKInfo(&dwKeyId);

		// read size of app_suffix string
		LONG lSufSize = 0;
		file.Read(&lSufSize, sizeof(LONG));

		if (lSufSize)
		{
			CString strSuff(m_strSuffix);
			// save app_name_string
			file.Read(strSuff.GetBuffer(lSufSize + 1), lSufSize);
			strSuff.ReleaseBuffer( lSufSize );
			if (strSuff.GetLength() > lSufSize)
				strSuff.SetAt(lSufSize, '\0');

			GuardSetSuffix(strSuff);
		}
	}
	catch (CFileException * e)
	{
		CString strError;
		e->GetErrorMessage(strError.GetBuffer(1024), 1024);
		TRACE ("%s\n", strError);
		switch (e->m_cause)
		{
		case CFileException::tooManyOpenFiles:

		case CFileException::genericException:
		case CFileException::hardIO:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::fileNotFound:
		case CFileException::badPath:
		case CFileException::invalidFile:
			GuardSetErrorCode(guardInvalidGuardFile);
			break;

		case CFileException::accessDenied:
		case CFileException::sharingViolation:
		case CFileException::lockViolation:
		case CFileException::badSeek:
		case CFileException::endOfFile:
			GuardSetErrorCode(guardReadGuardFile);
			break;

		}
		if( bReportErrors )e->ReportError();
		e->Delete();

		return false;
	}
	return true;
}

void CRegisterApp::DefaultInit()
{
	CString strPath;
	GetCurrentDirectory(1024, strPath.GetBuffer(1024));
	strPath.ReleaseBuffer();

	m_regData.strSuffix = m_strSuffix;
	m_regData.strProgramName = m_strGuardAppName + m_regData.strSuffix;

	m_regData.strPath = strPath;
	m_regData.arrFiles.RemoveAll();


	//add all dlls from current directory
	CFileFind finder;
	BOOL bWorking = finder.FindFile(strPath + _T("\\*.dll"));

	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		if (finder.IsDirectory() || finder.IsDots())
			continue;

		m_regData.arrFiles.Add(finder.GetFilePath());
	}
}

bool CRegisterApp::SaveRegisterData(CRegisterVersion & regData)
{
	CString strRootKey("Software\\");
	strRootKey += m_strGuardCompanyName + _T("\\Register");
#ifdef _DEBUG
	strRootKey += _T("\\Debug");
#else
	strRootKey += _T("\\Release");
#endif

	CVTRegKey reg;
	if (!reg.Create(HKEY_CURRENT_USER, strRootKey))
		return false;
	
	CVTRegValue val;

	// set prog name
	val.SetName("ProgName");
	val.SetString(regData.strProgramName);
	if (!reg.SetValue(val))
		return false;

	// set prog path
	val.SetName("ProgPath");
	val.SetString(regData.strPath);
	if (!reg.SetValue(val))
		return false;

	// set prog suffix
	val.SetName("Suffix");
	val.SetString(regData.strSuffix);
	if (!reg.SetValue(val))
		return false;

	// Create sub key "files"
	if (!reg.CreateSubKey("Files"))
		return false;

	if (!reg.Open(HKEY_CURRENT_USER, strRootKey + _T("\\Files")))
		return false;

	// set files count
	val.SetName("");
	val.SetDword(regData.arrFiles.GetSize());
	if (!reg.SetValue(val))
		return false;

	bool bRet = true;
	for (int i = 0; i < regData.arrFiles.GetSize(); i++)
	{
		CString strName;
		strName.Format("File%d", i);
	
		val.SetName(strName);
		val.SetString(regData.arrFiles[i]);
		
		if (!reg.SetValue(val))
			bRet = false;
	}


	return bRet;
}

bool CRegisterApp::LoadRegisterData(CRegisterVersion & regData)
{
	CString strRootKey("Software\\");
	strRootKey += m_strGuardCompanyName + _T("\\Register");
#ifdef _DEBUG
	strRootKey += _T("\\Debug");
#else
	strRootKey += _T("\\Release");
#endif

	CVTRegKey reg;
	if (!reg.Open(HKEY_CURRENT_USER, strRootKey))
		return false;
	
	CVTRegValue val;
	val.SetName("ProgName");
	if (reg.GetValue(val))
		regData.strProgramName = val.GetString();

	// get prog path
	val.SetName("ProgPath");
	if (reg.GetValue(val))
		regData.strPath = val.GetString();

	// set prog suffix
	val.SetName("Suffix");
	if (reg.GetValue(val))
		regData.strSuffix = val.GetString();
	
	regData.arrFiles.RemoveAll();
	if (!reg.Open(HKEY_CURRENT_USER, strRootKey + _T("\\Files")))
		return false;

	// Get files count
	int nCount = 0;
	val.SetName("");
	if (reg.GetValue(val))
		nCount = val.GetDword();

	CStringArray saValues;
	if (reg.EnumValues(saValues))
	{
		ASSERT (saValues.GetSize() == nCount);
		for (int i = 0; i < saValues.GetSize(); i++)
		{
			val.SetName(saValues[i]);
			if (reg.GetValue(val))
				regData.arrFiles.Add(val.GetString());
		}
	}

	return true;
}


