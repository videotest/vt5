// shell.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "shell.h"

#include "MainFrm.h"
#include "ntfman.h"

#include "CommandManager.h"
#include "ActiveScript.h"
#include "docs.h"
#include "targetman.h"
#include "shelldockbar.h"
#include "shelldoc.h"
#include "misc.h"
#include "scriptdocint.h"
#include "shellbutton.h"
#include "Guard.h"
#include "NameConsts.h"
#include "Registry.h"
#include "MenuRegistrator.h"
#include "lang.h"
#include "\vt5\awin\misc_ini.h"
#include "SplashWindow.h"
#include "\vt5\awin\misc_dbg.h"
#include "\vt5\awin\profiler.h"
#include "\vt5\Archive\BuildCount.h"
#include "fileactions.h"

#include "\vt5\com_base\com_defs.h"
#include "\vt5\com_base\share_mem.h"

#include "\vt5\AWIN\misc_string.h"
#include "\vt5\AWIN\misc_map.h"
#include "\vt5\AWIN\misc_dbg.h"

#include "ImageSignature.h"

#include "Utils.h"

#include "APIHook.h"

bool	bReportErrors = true;
BOOL	g_bUseLanguageHooks = true;

UINT WM_FIND_APP = RegisterWindowMessage("SHELL_FIND_APP");
bool InstallHooks( HMODULE h );

const char AppDataFilename[] = "shell.data";
const char szAppGuardFilename[] = "shell.grd";


// rewritable data
// suffix to control's ProgIDs
static char szSuffix[10] = { '_', 'd', 'e', 'm', 'o', 0, 0, 0, 0, 0};

// guid for crypt translation table file
// {21DADB6D-382C-4a8a-B77A-FD5A60B5FD85}
static GUID guidData = 
{ 0x21dadb6d, 0x382c, 0x4a8a, { 0xb7, 0x7a, 0xfd, 0x5a, 0x60, 0xb5, 0xfd, 0x85 } };

// имитовставка для закриптованного файла
static DWORD dwImito = 0;

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

extern "C" _declspec(dllexport) void GuardGetNSKInfo(DWORD * pwKeyID)
{
	theApp.GuardGetNSKInfo(pwKeyID);
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


extern "C" _declspec(dllexport) void GuardSetNSKInfo(DWORD * pwKeyID)
{
	theApp.GuardSetNSKInfo(pwKeyID);
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

///////////////////////////////////////////////////////////////////////////////
//Type info cache
class vt_type_info
{
public:
	vt_type_info()
	{
		pi_type_info = 0;
	}
	virtual ~vt_type_info()
	{
		if( pi_type_info )
			pi_type_info->Release();
	}

	INamedDataInfo*	pi_type_info;
	_string			str_name;
};

void func_free_vt_type_info( void* pdata )
{
	delete (vt_type_info*)pdata;
}

_map_t<vt_type_info*, const char*, cmp_string_nocase, func_free_vt_type_info>	g_map_types;

namespace{
	DWORD Index_HeapAllocRecursion=TlsAlloc();
	BOOL bGetRecursion(){return (BOOL)TlsGetValue(Index_HeapAllocRecursion);}
	void SetRecursion(BOOL b){
		TlsSetValue(Index_HeapAllocRecursion,(LPVOID)b);
	}
}

extern "C" _declspec(dllexport) INamedDataInfo* VTGetTypeInfo( BSTR bstrType )
{
	TPOS lpos = g_map_types.find( (char*)_bstr_t( bstrType ) );
	if( lpos )
	{
		vt_type_info* pti = g_map_types.get( lpos );
		if( pti->pi_type_info )
		{
			pti->pi_type_info->AddRef();
			return pti->pi_type_info;
		}
	}

	sptrIDataTypeInfoManager	sptrM( GetAppUnknown() );

	long	lCount, n;
	sptrM->GetTypesCount( &lCount );

	CString	strType = bstrType;
	strType.MakeLower();

	for( n = 0; n < lCount; n++ )
	{
		INamedDataInfo *punkType = 0;
		sptrM->GetTypeInfo( n, &punkType );

		CString	strTest =GetObjectName( punkType );
		strTest.MakeLower();

		if( strTest == strType )
		{
			vt_type_info* pinfo = new vt_type_info;
			pinfo->str_name = strType;
			pinfo->pi_type_info = punkType;
			punkType->AddRef();
			g_map_types.set( pinfo, (char*)pinfo->str_name );

			return punkType;
		}

		if (punkType)
			punkType->Release();
	}

	return 0;	
}


int WINAPI LanguageLoadString(
  HINSTANCE hInstance,  // handle to resource module
  UINT uID,             // resource identifier
  LPTSTR lpBuffer,      // resource buffer
  int nBufferMax        // size of buffer
);

CString LanguageLoadCString( UINT ui_id )
{
	CString str;	
	char sz_buf[1024];	sz_buf[0] = 0;

	if( g_bUseLanguageHooks )
	{
		LoadString( AfxGetApp()->m_hInstance, ui_id, sz_buf, sizeof(sz_buf) );
		str = sz_buf;
	}
	else
		str.LoadString( ui_id );

	return str;
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

///////////////////////////////////////////////////////////////////////////////
//Factory cache
class vt_factory_info
{
public:
	vt_factory_info()
	{
		pi_vt_class			= 0;
		pi_class_factory	= 0;
		::ZeroMemory( &clsid, sizeof(CLSID) );
	}
	virtual ~vt_factory_info()
	{
		if( pi_vt_class )
			pi_vt_class->Release();			pi_vt_class = 0;

		if( pi_class_factory )
			pi_class_factory->Release();	pi_vt_class = 0;		
	}

	IVTClass*		pi_vt_class;
	IClassFactory*	pi_class_factory;
	GUID			clsid;
};

void free_vt_factory_info( void* pdata )
{
	delete (vt_factory_info*)pdata;
}

long cmp_guid( const GUID *p1, const GUID *p2 )
{
	return memcmp( p1, p2, sizeof( GUID ) );
}

_map_t<vt_factory_info*, const GUID*, cmp_guid, free_vt_factory_info>	g_map_fi;

HOOKMACRO("OLEAUT32.DLL", BSTR __stdcall, SysAllocString, (const OLECHAR * oleChar))
{
	BSTR bstr;
	BOOL b_HeapAllocRecursion=bGetRecursion();
	if(!b_HeapAllocRecursion){
		SetRecursion(TRUE);
		bstr=((PFN_SysAllocString)(PROC)g_SysAllocString)(oleChar);
		if(bstr){
//			add_mem(MI_BSTR, bstr, wcslen(oleChar)+2);
		}
		SetRecursion(FALSE);
	}else{
		bstr=((PFN_SysAllocString)(PROC)g_SysAllocString)(oleChar);
	}
	return bstr;
}

//WINOLEAPI CoCreateInstance(IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
//                    IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv);
//HANDLER_STDAPI CoCreateInstanceShell(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)

HOOKMACRO("OLE32.DLL", HRESULT STDAPICALLTYPE, CoCreateInstance, (IN REFCLSID rclsid, IN LPUNKNOWN pUnkOuter,
                    IN DWORD dwClsContext, IN REFIID riid, OUT LPVOID FAR* ppv))
{
	PROFILE_TEST( "shell.cpp\\CoCreateInstanceShell" )
	
	TPOS lpos = g_map_fi.find( &rclsid );
	if( lpos )
	{
		vt_factory_info* pfi = g_map_fi.get( lpos );
		if( pfi->pi_class_factory )
		{			
			return pfi->pi_class_factory->CreateInstance( pUnkOuter, riid,  ppv );
		}
		else if( pfi->pi_vt_class )
			return pfi->pi_vt_class->VTCreateObject( pUnkOuter, 0, riid, 0, ppv);
	}


	HRESULT hr = REGDB_E_CLASSNOTREG;
	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	IVTApplication * pApp = 0; 
	if (FAILED(GetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
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

// NOTE : for all objects that has index in Novex Stealth Key
// if NSK does not contains given index -> return with REGDB_E_CLASSNOTREG
	if (pEntry && pEntry->dwData && CShellApp::m_dwGuardKeyID && !::StealthIndexIsAvailable(pEntry->dwData))
	{
		return CO_E_ERRORINDLL;
	}

// NOTE : for demo-version this peace of code must be removed
////////////////////{
	// if entry not founded
	if (!pEntry || FAILED(hr)) // not own control  // 
	{
		//BYTE * pCode = (BYTE*)GetDataCreateInstance();
		//DWORD dw = 0;
		//memcpy(&dw, &pCode[8], sizeof(DWORD));
		//if (dw)
		//{
		//	// restore original handler
		//	memcpy((BYTE*)dw, &pCode[1], 7);

		//	// simply call CoCreateInstance function
		//	typedef HRESULT (_stdcall *PFN)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR* );
		//	PFN pfn = (PFN)dw;
		//	hr = pfn(rclsid, pUnkOuter, dwClsContext, riid, ppv);

		//	// set new handler again
		//	BYTE * ptr = (BYTE*)dw;
		//	*ptr = 0xB8;ptr++; // mov eax, &data
		//	memcpy(ptr, &pCode[12], sizeof(DWORD));
		//	ptr += 4;
		//	*ptr = 0xFF;ptr++; // jmp eax
		//	*ptr = 0xE0;ptr++; // 
		//}

		hr = ((PFN_CoCreateInstance)(PROC)g_CoCreateInstance)(rclsid, pUnkOuter, dwClsContext, riid, ppv);
	
		if (SUCCEEDED(hr))
			return hr;
		extGuid = rclsid;
	}
/////////////////////}
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

	HINSTANCE hDll = ::LoadLibraryExW(bstrModule, 0, 0);
	if (!hDll)
		return TYPE_E_CANTLOADLIBRARY;

	if( g_bUseLanguageHooks )
		InstallHooks( hDll );

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
						//add to cache
						{
							vt_factory_info* pfi = new vt_factory_info;
							pfi->pi_vt_class = ptrVT;
							pfi->pi_vt_class->AddRef();
							pfi->clsid = rclsid;
							g_map_fi.set( pfi, &pfi->clsid );
						}
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
				//add to cache
				{
					vt_factory_info* pfi = new vt_factory_info;
					pfi->pi_vt_class = ptrVT;
					pfi->pi_vt_class->AddRef();
					pfi->clsid = rclsid;
					g_map_fi.set( pfi, &pfi->clsid );
				}
				ptrVT->Release();
			}
			else
			{
				IClassFactory * pFact = 0;
				if (SUCCEEDED(pfn(intGuid, IID_IClassFactory, (LPVOID*)&pFact)) && pFact)
				{
					hr = pFact->CreateInstance(pUnkOuter, riid,  ppv);					
					//add to cache
					{
						vt_factory_info* pfi = new vt_factory_info;
						pfi->pi_class_factory = pFact;
						pfi->pi_class_factory->AddRef();
						pfi->clsid = rclsid;
						g_map_fi.set( pfi, &pfi->clsid );
					}
					pFact->Release();
				}
			}
		}
	}		


	if( hr == 0 && *ppv==0 )
		return E_NOINTERFACE;
//	::CoFreeLibrary(hDll);

	return hr;

}

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
//WINOLEAPI CoCreateInstanceEx(
//    IN REFCLSID                    Clsid,
//    IN IUnknown    *               punkOuter, // only relevant locally
//    IN DWORD                       dwClsCtx,
//    IN COSERVERINFO *              pServerInfo,
//    IN DWORD                       dwCount,
//    IN OUT MULTI_QI    *           pResults );
//HANDLER_STDAPI CoCreateInstanceExShell(REFCLSID rclsid, LPUNKNOWN punkOuter, DWORD dwClsCtx, COSERVERINFO * pServerInfo, ULONG cmq, MULTI_QI * pResults)
HOOKMACRO("OLE32.DLL", HRESULT STDAPICALLTYPE, CoCreateInstanceEx, (REFCLSID rclsid, LPUNKNOWN punkOuter, DWORD dwClsCtx, COSERVERINFO * pServerInfo, ULONG cmq, MULTI_QI* pResults))
{
	//PROFILE_TEST( "shell.cpp\\CoCreateInstanceExShell" )

	HRESULT hr = REGDB_E_CLASSNOTREG;

	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	IVTApplication * pApp = 0; 
	if (FAILED(GetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
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
// NOTE : for all objects that has index in Novex Stealth Key
// if NSK does not contains given index -> return with REGDB_E_CLASSNOTREG
	if (pEntry && pEntry->dwData && CShellApp::m_dwGuardKeyID && !::StealthIndexIsAvailable(pEntry->dwData))
	{
		return CO_E_ERRORINDLL;
	}

// NOTE : for demo-version this peace of code must be removed
////////////////////{
	// if entry not founded
	if (!pEntry || FAILED(hr)) // not own control
	{
		//BYTE * pCode = (BYTE*)GetDataCreateInstanceEx();
		//DWORD dw = 0;
		//memcpy(&dw, &pCode[8], sizeof(DWORD));
		//if (dw)
		//{
		//	// restore original handler
		//	memcpy((BYTE*)dw, &pCode[1], 7);

		//	// simply call CoCreateInstanceEx function
		//	typedef HRESULT (_stdcall *PFN)(REFCLSID, LPUNKNOWN, DWORD, COSERVERINFO*, ULONG, MULTI_QI*);
		//	PFN pfn = (PFN)dw;
		//	hr = pfn(rclsid, punkOuter, dwClsCtx, pServerInfo, cmq, pResults);

		//	// set new handler again
		//	BYTE * ptr = (BYTE*)dw;
		//	*ptr = 0xB8;ptr++; // mov eax, &data
		//	memcpy(ptr, &pCode[12], sizeof(DWORD));
		//	ptr += 4;
		//	*ptr = 0xFF;ptr++; // jmp eax
		//	*ptr = 0xE0;ptr++; // 
		//}

		hr = ((PFN_CoCreateInstanceEx)(PROC)g_CoCreateInstanceEx)(rclsid, punkOuter, dwClsCtx, pServerInfo, cmq, pResults);
		return hr;
	}
//////////////////////}
	if (pServerInfo || cmq <= 0 || !pResults)
		return E_INVALIDARG;

	IUnknown * punk = 0;
//	hr = CoCreateInstanceShell(rclsid, punkOuter, dwClsCtx, IID_IUnknown, (LPVOID*)&punk);
	hr = CoCreateInstance(rclsid, punkOuter, dwClsCtx, IID_IUnknown, (LPVOID*)&punk);
	if (FAILED(hr) || !punk)
		return hr;

	for (ULONG i = 0; i < cmq; i++)
#if _MSC_VER >= 1300
		pResults->hr = punk->QueryInterface(*pResults->pIID, (LPVOID*)&(pResults->pItf));
#else
		pResults->hr = punk->QueryInterface(pResults->pIID, (LPVOID*)&(pResults->pItf));
#endif

	punk->Release();
	return hr;

}
#endif // DCOM

//HANDLER_STDAPI CoGetClassObjectShell(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv)
HOOKMACRO("OLE32.DLL", HRESULT STDAPICALLTYPE, CoGetClassObject, (REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv))
{
	//PROFILE_TEST( "shell.cpp\\CoGetClassObjectShell" )

	HRESULT hr = REGDB_E_CLASSNOTREG;
	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	IVTApplication * pApp = 0; 
	if (FAILED(GetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
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

// NOTE : for all objects that has index in Novex Stealth Key
// if NSK does not contains given index -> return with REGDB_E_CLASSNOTREG
	if (pEntry && pEntry->dwData && CShellApp::m_dwGuardKeyID && !::StealthIndexIsAvailable(pEntry->dwData))
	{
		return CO_E_ERRORINDLL;
	}


// NOTE : for demo-version this peace of code must be removed
////////////////////{
	// if entry not founded
	if (!pEntry || FAILED(hr)) // not own control
	{
		//BYTE * pCode = (BYTE*)GetDataGetClassObject();
		//DWORD dw = 0;
		//memcpy(&dw, &pCode[8], sizeof(DWORD));
		//if (dw)
		//{
		//	// restore original handler
		//	memcpy((BYTE*)dw, &pCode[1], 7);

		//	// simply call CoGetClassObject function
		//	typedef HRESULT (_stdcall *PFN)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*) ;
		//	PFN pfn = (PFN)dw;
		//	hr = pfn(rclsid, dwClsContext, pvReserved, riid, ppv);

		//	// set new handler again
		//	BYTE * ptr = (BYTE*)dw;
		//	*ptr = 0xB8;ptr++; // mov eax, &data
		//	memcpy(ptr, &pCode[12], sizeof(DWORD));
		//	ptr += 4;
		//	*ptr = 0xFF;ptr++; // jmp eax
		//	*ptr = 0xE0;ptr++; // 
		//}

		hr = ((PFN_CoGetClassObject)(PROC)g_CoGetClassObject)(rclsid, dwClsContext, pvReserved, riid, ppv);

		return hr;
	}
//////////////////////////}
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
#if 0
		if( g_bUseLanguageHooks )
			InstallHooks( hDll );
#endif

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

//HANDLER_STDAPI ProgIDFromCLSIDShell(REFCLSID rclsid, LPOLESTR FAR* lplpszProgID)
HOOKMACRO("OLE32.DLL", HRESULT STDAPICALLTYPE, ProgIDFromCLSID, (REFCLSID rclsid, LPOLESTR FAR* lplpszProgID))
{
	//PROFILE_TEST( "shell.cpp\\ProgIDFromCLSIDShell" )

	// interaction w/ translate_table
	GuidKey extGuid(rclsid);
	GuidKey intGuid(rclsid);

	if (!lplpszProgID)
		return E_POINTER;

	IVTApplication * pApp = 0; 
	if (FAILED(GetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) || !pApp)
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

//HANDLER_STDAPI CLSIDFromProgIDShell(LPCOLESTR lpszProgID, LPCLSID lpclsid)
HOOKMACRO("OLE32.DLL", HRESULT STDAPICALLTYPE, CLSIDFromProgID, (LPCOLESTR lpszProgID, LPCLSID lpclsid))
{
	//PROFILE_TEST( "shell.cpp\\CLSIDFromProgIDShell" )

	HRESULT hr = REGDB_E_CLASSNOTREG;

	// interaction w/ translate_table
	if (lpclsid)
	{
		IVTApplication * pApp = 0; 
		if (SUCCEEDED(GetAppUnknown(false)->QueryInterface(IID_IVTApplication, (void**)&pApp)) && pApp)
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

	hr=((PFN_CLSIDFromProgID)(PROC)g_CLSIDFromProgID)(lpszProgID, lpclsid);

//	hr = REGDB_E_CLASSNOTREG;
//	BYTE * pCode = (BYTE*)GetDataCLSIDFromProgID();
//	DWORD_PTR dw = 0;
//	memcpy(&dw, &pCode[8], sizeof(DWORD_PTR));
//	if (dw)
//	{
//		// restore original handler
//		memcpy((BYTE*)dw, &pCode[1], 7);
//
//		// simply call CLSIDFromProgID function
//		typedef HRESULT (_stdcall *PFN)(LPCOLESTR, LPCLSID);
//		PFN pfn = (PFN)dw;
//
//
////		hr = pfn(lpszProgID, lpclsid);
//
//
//		// set new handler again
//		BYTE * ptr = (BYTE*)dw;
//		*ptr = 0xB8;ptr++; // mov eax, &data
//		memcpy(ptr, &pCode[12], sizeof(DWORD));
//		ptr += 4;
//		*ptr = 0xFF;ptr++; // jmp eax
//		*ptr = 0xE0;ptr++; // 
//	}
	
	return hr;
}


DWORD __stdcall ThreadFuncShellError(LPVOID lpParam)
{
	HANDLE hEvents[2];
	hEvents[0] = ::OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME);
	hEvents[1] = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);
	while (1)
	{
		DWORD dwRet = WaitForMultipleObjects(2, hEvents, false, INFINITE);
		if (dwRet == WAIT_FAILED)
		{
			CloseHandle(hEvents[0]);
			CloseHandle(hEvents[1]);
			return -1;
		}

		if (dwRet - WAIT_OBJECT_0 == 0) // report error
		{
			guardErrorCode eCode = (guardErrorCode)GuardGetErrorCode();
			if (guardNoError != eCode)
			{
				CString strError = GuardGetErrorMsg(eCode);
				::AfxMessageBox(strError, MB_OK);
				
				SetEvent(hEvents[1]);
				
				CloseHandle(hEvents[0]);
				CloseHandle(hEvents[1]);

				
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
				UninstallCreateInstanceEx();
#endif // DCOM

				UninstallProgIDFromCLSID();
				UninstallCLSIDFromProgID();
				UninstallGetClassObject();
				UninstallCreateInstance();

				//StealthDeinit();
				// simulate GPF
				


				//::ExitProcess((DWORD)eCode);

				return (DWORD)eCode;
			}
			GuardSetErrorCode(guardNoError);
			::ResetEvent(hEvents[0]);
		}
		else if ((dwRet - WAIT_OBJECT_0) == 1)
		{
			CloseHandle(hEvents[0]);
			CloseHandle(hEvents[1]);
			return 0;
		}
		Sleep(500);
	}

	return 0;
}


//id of typelib
GUID IID_ITypeLibID = 
{ 0xa23423c1, 0x69d5, 0x11d3, { 0xa6, 0x4d, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

/////////////////////////////////////////////////////////////////////////////
// CShellApp

BEGIN_MESSAGE_MAP(CShellApp, CWinApp)
	//{{AFX_MSG_MAP(CShellApp)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CShellApp construction

extern LANGID g_lid;

void ShellInitLanguages()
{
//try to get value from shell.data using windows api
	char	szIniFileName[MAX_PATH];
	::GetModuleFileName( 0, szIniFileName, MAX_PATH );
	char	*p = strrchr( szIniFileName, '\\' );
	strcpy( p, "\\shell.data" );

	char	sz[10] = "ru";
	::GetPrivateProfileString( "General", "Language:String", sz, sz, 10, szIniFileName );
	::WritePrivateProfileString( "General", "Language:String", sz, szIniFileName );

	// 18.11.2004 build 72 - прикручиваю третий вариант локализации (только для 2000/xp)
	// - если \General\UseLanguage=2 - то локализуемся по SetThreadLocale
	//g_bUseLanguageHooks = ::GetPrivateProfileInt( "General", "UseLanguage:Long", 1, szIniFileName );
	//::WritePrivateProfileInt( "General", "UseLanguage:Long", g_bUseLanguageHooks, szIniFileName );
	long nUseLanguage = ::GetPrivateProfileInt( "General", "UseLanguage:Long", 2, szIniFileName );
	::WritePrivateProfileInt( "General", "UseLanguage:Long", nUseLanguage, szIniFileName );

	LANGID lid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);

	if( !stricmp( sz, "ru" ) )lid = MAKELANGID(LANG_RUSSIAN, SUBLANG_DEFAULT);
	else if( !stricmp( sz, "en" ) )lid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
	else if( !stricmp( sz, "ch" ) )lid = MAKELANGID(LANG_CHINESE, SUBLANG_CHINESE_SIMPLIFIED );

	g_lid = lid;

	// 18.11.2004 build 72
	if(nUseLanguage==0) g_bUseLanguageHooks = FALSE;
	else if(nUseLanguage==2)
	{
		g_bUseLanguageHooks = FALSE;
		SetThreadLocale(lid);
	}
	else g_bUseLanguageHooks = TRUE;

	if( g_bUseLanguageHooks )
	{
//		InitLanguageSupport( lid );
	}
	/*
	else
	{
		OSVERSIONINFO  osver;
		osver.dwOSVersionInfoSize = sizeof( osver ) ;
		GetVersionEx( &osver );
		// If Windows NT:
		if ( osver.dwPlatformId == VER_PLATFORM_WIN32_NT )		
			::SetThreadLocale(MAKELCID(MAKELANGID(LANG_RUSSIAN,SUBLANG_DEFAULT),SORT_DEFAULT));
			//::SetThreadLocale( g_lid );	
	}
	*/

	//CWinApp
	//SetResourceHandle( 
}

DWORD CShellApp::m_dwGuardKeyID;

bool InstallMfcHook();

HMODULE hmod_dump = 0;

CShellApp::CShellApp()
{
	m_nPumpMessageDepth = 0;

	//m_benable_map	= false;

	ShellInitLanguages();	

	m_hWndHook = 0;

	m_bNestedLoop = false;
	m_strSuffix = szSuffix;
	m_innerclsid = guidData;
	m_dwImito = dwImito;
	m_pTable = 0;//new CTranslateTable();

	m_strGuardAppName = DEF_APP_NAME;
	m_strGuardCompanyName = DEF_COMPANY_NAME;
	m_dwGuardKeyID = 0;
 
	m_pDocManager = new CShellDocManager();
	m_nCurrentHelpCommand = 0;
	m_dwCurrentHelpTopicID = 0;

	m_bCreateInstance = m_bGetClassObject = m_bCLSIDFromProgID = m_bProgIDFromCLSID = false;
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	m_bCreateInstanceEx = false;
#endif // DCOM

	m_hErrorThread		= 0;
	m_hmutex_app		= 0;

//idle optimize
	m_last_idle_tick	= 0;
	m_last_idle_count	= 0;

//	bool InstallMfcFixs(void); InstallMfcFixs();

	#ifdef _DEBUG
	//InstallMfcHook();
	#endif _DEBUG

	hmod_dump = ::LoadLibrary( "dump.dll" );
}

CShellApp::~CShellApp()
{
#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	if (m_bCreateInstanceEx)
		UninstallCreateInstanceEx();
	m_bCreateInstanceEx = false;
#endif // DCOM

	if (m_bProgIDFromCLSID)
		UninstallProgIDFromCLSID();

	if (m_bCLSIDFromProgID)
		UninstallCLSIDFromProgID();

	if (m_bGetClassObject)
		UninstallGetClassObject();

	if (m_bCreateInstance)
		UninstallCreateInstance();

	m_bCreateInstance = m_bGetClassObject = m_bCLSIDFromProgID = m_bProgIDFromCLSID = false;
	

	if( m_hmutex_app )
		CloseHandle( m_hmutex_app );	m_hmutex_app = 0;

	g_script.DeInit();

	if( hmod_dump )
		::FreeLibrary( hmod_dump );
}

void CShellApp::GuardGetAppName(char * szAppName, LONG * plSize)
{
	if (plSize)
		*plSize = m_strGuardAppName.GetLength();

	if (AfxIsValidAddress(szAppName, m_strGuardAppName.GetLength() + 1))
		lstrcpy(szAppName, m_strGuardAppName);
}

void CShellApp::GuardGetCompanyName(char * szCompanyName, long * plSize)
{
	if (plSize)
		*plSize = m_strGuardCompanyName.GetLength();

	if (AfxIsValidAddress(szCompanyName, m_strGuardCompanyName.GetLength() + 1))
		lstrcpy(szCompanyName, m_strGuardCompanyName);
}

void CShellApp::GuardGetSuffix(char * szSuffix, long * plSize)
{
	if (plSize)
		*plSize = m_strSuffix.GetLength();

	if (AfxIsValidAddress(szSuffix, m_strSuffix.GetLength() + 1))
		lstrcpy(szSuffix, m_strSuffix);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CShellApp object

CShellApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.


/////////////////////////////////////////////////////////////////////////////
// CShellApp initialization



long __stdcall __ecx_filter( EXCEPTION_POINTERS *ExceptionInfo )
{
	static long	call_counter = 0;
	char	sz[MAX_PATH];
	char	sz_args[MAX_PATH+20];

	if( call_counter )return EXCEPTION_EXECUTE_HANDLER;

/*	::GetModuleFileName( 0, sz, sizeof(sz) );
	strcpy( strrchr( sz, '\\' ), "\\dumps" );
	::CreateDirectory( sz, 0 );*/

	char	sz_fbase[30];
	::sprintf( sz_fbase, "dump%03d", BUILD_NO );

	GetModuleFileName( 0, sz, sizeof(sz) );
	strcpy( strrchr( sz, '\\' ), "\\write_dump.exe" );
	sprintf( sz_args, "\"%s\"/pid:%d /tid:%d /pex:%p /fb:%s", sz, ::GetCurrentProcessId(), ::GetCurrentThreadId(), ExceptionInfo, sz_fbase );
	//::MessageBox( AfxGetMainWnd()->GetSafeHwnd(), sz_args, "Outpost Error", MB_OK );
 
	PROCESS_INFORMATION	pi;
	ZeroMemory( &pi, sizeof( pi ) );
	STARTUPINFO	si;
	ZeroMemory( &si, sizeof( si ) );
	if( ::CreateProcess( sz, sz_args, 0, 0, 0, 0, 0, 0, &si, &pi ) )
	{
		::WaitForSingleObject( pi.hProcess, 1000000 );
		::CloseHandle( pi.hProcess );
		::CloseHandle( pi.hThread );

		
		if( call_counter == 0 )
		{
			char	sz_text[1000];
			wsprintf( sz_text, "Unhandled Exception %X. Minidump created", ExceptionInfo->ExceptionRecord->ExceptionCode, sz );
			::MessageBox( AfxGetMainWnd()->GetSafeHwnd(), sz_text, "VideoTesT Stability System", MB_OK );
		}
	}
	else
	{
		if( call_counter == 0 )
		{
			char	sz_text[1000];
			wsprintf( sz_text, "Unhandled Exception %X. Minidump can't be created", ExceptionInfo->ExceptionRecord->ExceptionCode, sz );
			::MessageBox( AfxGetMainWnd()->GetSafeHwnd(), sz_text, "VideoTesT Stability System", MB_OK );
		}
	}
	call_counter++;

	return EXCEPTION_EXECUTE_HANDLER;
}

//fire simple event
void FireScriptEvent( const char* psz_event )
{
	IScriptSitePtr	ptr_ss = ::GetAppUnknown();
	if( ptr_ss == 0 )	return;

	ptr_ss->Invoke( _bstr_t( psz_event ), 0, 0, 0, fwAppScript );
}

//#include "uxtheme.h"
#include "\vt5\awin\misc_module.h"

//cheking for available disk space with current user quotas
BOOL check_disk_space(CMainFrame* mf)
{
	ULARGE_INTEGER total ={0}, free_for_user ={0}, total_free ={0};
	CString str = ::GetValueString(::GetAppUnknown(), "Paths", "MethodCache","");
	TCHAR* path = str.IsEmpty()? 0 : str.GetBuffer();
	long fp =0, min_free =0;

	if(GetDiskFreeSpaceEx(path, &free_for_user, &total, &total_free))
	{
		fp = long(free_for_user.QuadPart/(1<<20));
		min_free = ::GetValueInt(::GetAppUnknown(), "General", "MinimumDiskAvailableMb",100);
		if( fp >= min_free) return TRUE;
	}
	else return TRUE;
	str.ReleaseBuffer();
	str.Format(IDS_NOT_ENOUGH_FREE_DISK_SPACE, fp, min_free); 
	/*mf->HideSplash();*/
	HWND logo_hwnd = CLogoWnd::m_pLogoWnd->GetSafeHwnd(); 
	SendMessage(logo_hwnd, WM_KILLFOCUS, 0,0);
	AfxMessageBox(str, MB_ICONERROR | MB_OK | MB_SYSTEMMODAL, 0);
	return FALSE;
}

BOOL CShellApp::InitInstance()
{
//	m_hLangResourceDLL = ::LoadLibrary( "mfc70rus.dll" );
//	AfxSetResourceHandle(m_hLangResourceDLL);
	//paul hw

	static module	_module( 0 );
	_module.init( (HINSTANCE)m_hInstance );

//	SetThemeAppProperties(STAP_ALLOW_CONTROLS|STAP_ALLOW_WEBCONTENT);

#ifdef _DEBUG
	//_CrtSetDbgFlag( 0/*_CRTDBG_ALLOC_MEM_DF*/ );
#endif _DEBUG

	SetUnhandledExceptionFilter( __ecx_filter );

	int nDisableSteal = 
		GetPrivateProfileInt("Shell", "DisableStealFocus", 0, "vt5_monster.ini");
	if(nDisableSteal)
	{
		SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, LPVOID(10000), 0);
		LockSetForegroundWindow(LSFW_LOCK);
	}

	//is app running?
	{
		char* psz_mutex = "VT5-56AF69D5-4C40-4976-AB36-97BF04A18BAF";
		m_hmutex_app = OpenMutex( MUTEX_ALL_ACCESS, false, psz_mutex );
		if( m_hmutex_app )
		{
			DWORD dwRecipients = BSM_APPLICATIONS;			

			_bstr_t bstr_line = m_lpCmdLine;					
			if( bstr_line.length() && GetValueInt(::GetAppUnknown(), "\\MainFrame", "EnableRegisterExtensions", 1))
			{
				int nfile_len = bstr_line.length() * 2 + 2;
				_char sz_file_name[256];
				strcpy( sz_file_name, "SHEL_COMMAND_LINE_MMF" );
				share_mem mem;
				mem.create_new( sz_file_name, nfile_len );
				BYTE* pfile_buf = (BYTE*)mem.lock();
				if( pfile_buf )
				{
					::ZeroMemory( pfile_buf, nfile_len );
					memcpy( pfile_buf, (wchar_t*)bstr_line, 2 * bstr_line.length() );
				}
				mem.unlock();
				::BroadcastSystemMessage( BSF_IGNORECURRENTTASK, &dwRecipients, WM_FIND_APP, (WPARAM)nfile_len, NULL );
			}

			return FALSE;
		}

		m_hmutex_app = CreateMutex( 0, false, psz_mutex );
	}

	_try(CShellApp, InitInstance)
	{
		if (!IsUserAnAdmin())
		{
			int r = AfxMessageBox(IDS_USER_NOT_ADMIN, MB_YESNO);
			if (r != IDYES)
				return FALSE;
		}

		//регистрируем dll из списка		
		char	sz_file[MAX_PATH];
		::GetModuleFileName( 0, sz_file, sizeof( sz_file ) );

		strcpy( strrchr( sz_file, '\\' ), "\\register.lst" );

		FILE	*pfile = ::fopen( sz_file, "rt" );

		if( pfile )
		{
			while( !feof( pfile ) )
			{
				char	sz_module[MAX_PATH];	sz_module[0] = 0;
				fgets( sz_module, sizeof( sz_module ), pfile );

				int nlen = (int)strlen( sz_module );
				if( !nlen )continue;

				if( sz_module[nlen-1] == '\n' )
					sz_module[nlen-1] = 0;				

				HMODULE	h = ::LoadLibrary( sz_module );
				if( !h )continue;

				FARPROC	fp = ::GetProcAddress( h, "DllRegisterServer" );
				if( fp )fp();

				::FreeLibrary( h );
			}

			::fclose( pfile );
		}
		//едем дальше


		//set current app path to shell.exe path
		{
			//need for ole server running
			TCHAR szModulePath[MAX_PATH];
			TCHAR szCurrentPath[MAX_PATH];

			// Obtain the path to this module's executable file for later use.
			GetModuleFileName(
				GetModuleHandle( NULL ),
				szModulePath,
				sizeof(szModulePath)/sizeof(TCHAR));

			GetCurrentDirectory( sizeof(szCurrentPath)/sizeof(TCHAR), szCurrentPath );

			CString strModuleName = szModulePath;
			int nIndex = strModuleName.ReverseFind( '\\' );
			if( nIndex != -1 )
			{
				CString strNeedPath = strModuleName.Left( nIndex );
				if( strNeedPath != szCurrentPath )
					SetCurrentDirectory( strNeedPath );
				
			}			
			
		}

	#if _MSC_VER >= 1300
	{
		AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
		pModuleState->m_classList.RemoveAll();

		RUNTIME_CLASS(CShellToolbarButton)->m_pNextClass = 0;
		AFX_CLASSINIT _init_CShellToolbarButton(RUNTIME_CLASS(CShellToolbarButton));
		RUNTIME_CLASS(CShellChooseToolbarButton)->m_pNextClass = 0;
		AFX_CLASSINIT _init_CShellChooseToolbarButton(RUNTIME_CLASS(CShellChooseToolbarButton));
		RUNTIME_CLASS(CShellMenuButton)->m_pNextClass = 0;
		AFX_CLASSINIT _init_CShellMenuButton(RUNTIME_CLASS(CShellMenuButton));
		RUNTIME_CLASS(CShellMenuComboBoxButton)->m_pNextClass = 0;
		AFX_CLASSINIT _init_CShellMenuComboBoxButton(RUNTIME_CLASS(CShellMenuComboBoxButton));
		RUNTIME_CLASS(CShellToolBar)->m_pNextClass = 0;
		AFX_CLASSINIT _init_CShellToolBar(RUNTIME_CLASS(CShellToolBar));
		RUNTIME_CLASS(CShellFrameToolBar)->m_pNextClass = 0;
		AFX_CLASSINIT _init_CShellFrameToolBar(RUNTIME_CLASS(CShellFrameToolBar));

	}
	#endif



	//Logo support
	CreateLogoWnd();

	/*
		// check app is running
	if (GuardAppIsRunning())
	{
		// send broadcast message to all apps 
		DWORD dwRecipients = BSM_APPLICATIONS;   
		::BroadcastSystemMessage(BSF_POSTMESSAGE|BSF_IGNORECURRENTTASK, &dwRecipients, WM_FIND_APP, NULL, NULL);
		// and exit
		return FALSE;
	}
	*/
	if (!ReadGuardInfo(szAppGuardFilename))
	{
		AfxMessageBox(IDS_GUARD_FILE_READ_FAILED);
		return FALSE;
	}

	CoInitialize(0);
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		CoUninitialize();
		return FALSE;
	}
	//register
	AfxOleRegisterTypeLib(AfxGetInstanceHandle(), IID_ITypeLibID, _T("shell.tlb") );

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


	if (!::StealthInit())
	{
		CoUninitialize();
		return false;
	}
	DWORD dwErrorThreadID = 0;
	//m_hErrorThread = ::CreateThread(NULL, 0, ThreadFuncShellError, 0, 0, &dwErrorThreadID);

	//if (m_dwGuardKeyID)
	{
		if (!StealthUpdate(m_dwGuardKeyID))
		{
			::StealthDeinit();
			CoUninitialize();
			return false;
		}
		StealthReadGUID(&m_innerclsid);
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
	m_pTable->LoadText("shell.grd.txt");
	m_pTable->SaveText("shell.grd3.txt");


	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(m_strGuardCompanyName);

	m_bCreateInstance = false;//InstallCreateInstance((LPVOID)&CoCreateInstanceShell);

	m_bGetClassObject = false;//InstallGetClassObject((LPVOID)&CoGetClassObjectShell);

	m_bCLSIDFromProgID = false;//InstallCLSIDFromProgID((LPVOID)&CLSIDFromProgIDShell);

	m_bProgIDFromCLSID = false;//InstallProgIDFromCLSID((LPVOID)&ProgIDFromCLSIDShell);

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	m_bCreateInstanceEx = false;//InstallCreateInstanceEx((LPVOID)&CoCreateInstanceExShell);
#endif // DCOM

	AddOwnComponents();

	// update registry and add controls to table
	COleObjectFactory::UpdateRegistryAll(true);
	
	//register internal actions
	{
		CActionRegistrator	r;
		r.RegisterActions();

		CCompRegistrator rs( szPluginScriptNamespace );
		rs.RegisterComponent( "Shell.App" );
	}


	LoadStdProfileSettings( 10 );  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.
//create a freame window object 
	CMainFrame* pMainFrame = new CMainFrame;
//init the general component manager
	m_aggrs.AttachComponentGroup( szPluginApplicationAggr );
	m_aggrs.SetOuterUnknown( GetControllingUnknown() );
	m_aggrs.Init();
//ensure required components was loaded
	if( !CheckInterface( GetControllingUnknown(), IID_INamedData ) )
	{
		AfxMessageBox( IDS_NONAMEDDATA );
		delete pMainFrame;
		CoUninitialize();
		return false;
	}
	if( !CheckInterface( GetControllingUnknown(), IID_INotifyController ) )
	{
		AfxMessageBox( IDS_NONTFCONTR );
		delete pMainFrame;
		CoUninitialize();
		return false;
	}
//load the application NamedData
	EnableActionRegistartion( false );
	{
		sptrIFileDataObject2	sptrF( GetControllingUnknown() );
		_bstr_t	bstrFileName = MakeAppPathName( AppDataFilename );
		sptrF->LoadTextFile( bstrFileName );
		/*
		//!!! debug
		{
			sptrINamedDataLogger sptrLogger( GetAppUnknown() );
			sptrIFileDataObject	sptrF1;
			sptrF1.CreateInstance( szNamedDataProgID );
			sptrLogger->SetLog(ndlkRead, sptrF1);
		}
		{
			sptrINamedDataLogger sptrLogger( GetAppUnknown() );
			sptrIFileDataObject	sptrF1;
			sptrF1.CreateInstance( szNamedDataProgID );
			sptrLogger->SetLog(ndlkWrite, sptrF1);
		}
		*/
	}

	if(!check_disk_space(pMainFrame)) 
	{
		delete pMainFrame;
		CoUninitialize();
		return false;
	}

	char	szPathName[_MAX_PATH];
	::GetModuleFileName( AfxGetInstanceHandle(), szPathName, _MAX_PATH );
	::GetLongPathName( szPathName, szPathName, _MAX_PATH ) ;
	char	*pLastSlash = strrchr( szPathName, '\\' );
	if( pLastSlash )*(pLastSlash+1) = 0;
	::SetValue( GetAppUnknown(), "\\Paths", "CurrentPath", szPathName );


	CSize	sizeButton;
	int		nButtonStyle = 0;//flat, semi-flat, ordinary, round
	sizeButton.cx = GetValueInt( GetAppUnknown(), "General", "SizeButtonCX", 23 );
	sizeButton.cy = GetValueInt( GetAppUnknown(), "General", "SizeButtonCY", 22 );
	nButtonStyle = GetValueInt( GetAppUnknown(), "General", "ButtonStyle", 1 );

	CShellToolbarButton::SetButtonsStyle( nButtonStyle );
	CShellToolbarButton::SetButtonSize( sizeButton );

	

	CString	strComps = CString()+szPluginApplication+"\n"+szPluginScriptNamespace;
	AttachComponentGroup( strComps );
	SetRootUnknown( GetControllingUnknown() );
//fill script namespace
	CCompManager::Init();
//create...
	CShellDocManagerDisp	*pDocManDisp = new CShellDocManagerDisp();	//ref counter == 1
//...add local components - namespace for script engeene
	AddComponent( g_CmdManager.GetControllingUnknown() );
	AddComponent( g_NtfManager.GetControllingUnknown() );
	AddComponent( pMainFrame->GetControllingUnknown() );
	AddComponent( pDocManDisp->GetControllingUnknown() );

	pDocManDisp->GetControllingUnknown()->Release();
//check the loaded comps's
	IUnknown *punkAM =GetComponent( IID_IActionManager, false );
	m_ptrAM = punkAM;
	if( !punkAM )
	{
		AfxMessageBox( IDS_NOACTMAN );
		delete pMainFrame;
		CoUninitialize();
		return false;
	}
//attach ActionManager as EventListener to the main window
	sptrIWindow2	sptrW2( pMainFrame->GetControllingUnknown() );
	sptrW2->AttachMsgListener( punkAM );

	//turn on/off script debugger
	::SetValue( ::GetAppUnknown(), DEBUGGER_SECTION, DEBUGGER_ENABLE, strstr( m_lpCmdLine, "/Debug" ) ? 1L : 0L );	
	
	g_script.Init();

	g_TargetManager.Init();	//initialize application target
//initialize the document manager
	((CShellDocManager*)m_pDocManager)->Init();
	
	g_CmdManager.Init();	//...and command manager

	IScriptSitePtr sptrSSite = GetControllingUnknown();
	if( sptrSSite != 0 )
	{
		IActiveScript* pi_script = g_script.GetActiveScript();
		if( pi_script )
		{
			sptrSSite->RegisterScript( pi_script, fwAppScript );
			pi_script->Release();
		}
	}


	g_script.LoadScripts();

	//register global scripts
	{
		EnableActionRegistartion( true );

		sptrIDataTypeManager	sptrN( GetControllingUnknown() );

		long	nType = -1;

		long nTypesCount = 0;

		sptrN->GetTypesCount( &nTypesCount );

		for( long n = 0; n < nTypesCount; n++ )
		{
			BSTR	bstr;
			sptrN->GetType( n, &bstr );

			CString	strType =bstr;

			::SysFreeString( bstr );

			if( strType == _T("Script") )
				nType = n;

		}
		
		if( nType != -1 )
		{
			LONG_PTR	lpos = 0;

			sptrN->GetObjectFirstPosition( nType, &lpos );

			while( lpos )
			{
				IUnknown *punkObj = 0;
				sptrN->GetNextObject( nType, &lpos, &punkObj );

				ASSERT( punkObj );
				ASSERT( ::CheckInterface( punkObj, IID_IScriptDataObject ) );

				sptrIScriptDataObject	sptrS( punkObj );

				punkObj->Release();

				sptrS->RegisterScriptAction();

				if( GetObjectName( sptrS )==_T("AutoExec") )
				{
					BSTR	bstrScriptText = 0;
					sptrS->GetText( &bstrScriptText );
					::ExecuteScript( bstrScriptText, "Doc Autoexec", false, m_key );
					::SysFreeString( bstrScriptText );
				}
			}
		} 

	}


	//[paul] 12.12.2001. reason - notify plugins, all done
	::FireEvent( GetControllingUnknown(), "AppInit" );


	//store aliases with scripts
	IAliaseManPtr	ptrAliases = GetAppUnknown();
	if( ptrAliases != 0 )
	{
		_bstr_t	bstrAliaseFileName = ::MakeAppPathName( "shell.aliases" );
		ptrAliases->StoreFile( bstrAliaseFileName );
	}

	// Register all OLE server factories as running.  This enables the
	//  OLE libraries to create objects from other applications.
	COleTemplateServer::RegisterAll();

		// Note: MDI applications register all server objects without regard
		//  to the /Embedding or /Automation on the command line.

	// create main MDI Frame window

	CLogoWnd::CanDestroyByClick( 1 );

	bool bold = pMainFrame->GetLockRecalc( );
	pMainFrame->SetLockRecalc( true );

	LoadLibrary( "xpbar_ctrl.dll" );
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	if( !g_CmdManager.IsStateReady() && 
		GetValueInt( ::GetAppUnknown(), "\\General", "LoadStateOnStartup", 1 ) == 1 )
	{
		// [vanek] : restore main frame's state - 15.10.2004
		::ExecuteScript( _bstr_t( _T("MainWnd.RestoreState(\"\")") ), "CShellApp::InitInstance" );

		CString	strLastState = ::GetValueString( ::GetAppUnknown(), "\\General", "CurrentState", "default.state" );
		//if( !strLastState.IsEmpty() )
		//	::ExecuteAction( "ToolsLoadState", CString( "\"" )+strLastState+CString( "\"" ), aefNoShowInterfaceAnyway );
	}

	//Execute AutoExec script	
	//g_script.ExecuteAppScript( ENTRY_AUTOEXEC );
	
//	if( ::GetValueInt( ::GetAppUnknown(), "\\MainFrame", "EnableDragDropOpen", 1 ) != 0 )
		m_pMainWnd->DragAcceptFiles();

	((CShellDocManager*)m_pDocManager)->ReloadFiles();
	


	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);


	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		//return TRUE;
	}

	//Register file types:
	if (GetValueInt(::GetAppUnknown(), "\\MainFrame", "EnableRegisterExtensions", 1))
		CFileRegistrator::RegisterInSystem();
 
	pMainFrame->SetLockRecalc( bold );
	pMainFrame->RecalcLayout( );	
	
	// The main window has been initialized, so show and update it.	

	CLogoWnd::DelayDestroy();

	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();
	pMainFrame->UpdateClientStyle();
	// A.M. fix, BT 3732, use AttachThreadInput... instead SetFocus in CLogoWnd::OnCreate
	pMainFrame->MakeForeground();

	//force idle update
	{
		ForceIdleUpdate();
		// AAM: SBT1001. Возможно, избыточно
		// (см. также CActionToolsLoadSave::Invoke, CCommandManager::LoadState)
		CMainFrame	*pmain = (CMainFrame	*)AfxGetMainWnd();

		pmain->GetDockSite( AFX_IDW_DOCKBAR_LEFT )->CalcFixedLayout( false, false );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_TOP )->CalcFixedLayout( false, true );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_RIGHT )->CalcFixedLayout( false, false );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_BOTTOM )->CalcFixedLayout( false, true );

		// [vanek] - 15.10.2004
		pMainFrame->RecalcLayout();
	}

	// AAM: проверяем несколько DLL на вшивость
#ifdef _DEBUG
	CheckDllSignature("common.dll");
#else
	CheckDllSignature("common.dll");
#endif

	CLSID	clsidAvi = {0};
	if( ::CLSIDFromProgID( _bstr_t( "AviBase.AviObject" ), &clsidAvi ) == S_OK  )
	{
		IUnknown *punk=0;
		if( ::CoCreateInstance( clsidAvi, 0, CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&punk ) == S_OK )
		{
			CheckDllSignature("comps\\AviBase.dll");
			if( punk ) punk->Release();
			punk = 0;
		}
	}
	CheckDllSignature("comps\\Data.dll");
	CheckDllSignature("comps\\FileFilters2.dll");
	CheckDllSignature("comps\\ImageDoc.dll");

	CString str_help_path = ::GetDirectory( "HelpPath", "Help" );
	str_help_path += "shell.chm::/Shell/HelpContext.html";
	//First free the string allocated by MFC at CWinApp startup.
	//The string is allocated before InitInstance is called.
	free((void*)m_pszHelpFilePath);
	//Change the name of the .HLP file.
	//The CWinApp destructor will free the memory.
	m_pszHelpFilePath=_tcsdup(str_help_path);
	EnableHtmlHelp();

	if( cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen &&
		GetValueInt(::GetAppUnknown(), "\\MainFrame", "EnableRegisterExtensions", 1))
	{
		CString strParam = CString( "\"" )+cmdInfo.m_strFileName+CString( "\"" );
		::ExecuteAction( "FileOpen", strParam );
	}

	//paul 01.08.2003
	//fire app start
	FireScriptEvent( "Application_OnStart" );


	return TRUE;
	}
	_catch_report;
	return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
// CShellApp message handlers
BEGIN_INTERFACE_MAP(CShellApp, CWinApp)
  INTERFACE_PART(CShellApp, IID_IApplication, App )
  INTERFACE_PART(CShellApp, IID_ICompManager, CompMan )
  INTERFACE_PART(CShellApp, IID_ICompRegistrator, CompReg )
  INTERFACE_PART(CShellApp, IID_INumeredObject, Num )
  INTERFACE_PART(CShellApp, IID_IVTApplication, Guard)
  INTERFACE_PART(CShellApp, IID_IHwndHook, HwndHook)
  INTERFACE_PART(CShellApp, IID_IFormNamesData, FormNames)
  INTERFACE_PART(CShellApp, IID_IScriptSite, ScriptSite)

  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[0])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[1])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[2])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[3])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[4])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[5])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[6])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[7])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[8])
  INTERFACE_AGGREGATE(CShellApp, m_aggrs.m_pinterfaces[9])
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CShellApp, App)
IMPLEMENT_UNKNOWN(CShellApp, Guard)
IMPLEMENT_UNKNOWN(CShellApp, HwndHook)
IMPLEMENT_UNKNOWN(CShellApp, FormNames)
IMPLEMENT_UNKNOWN(CShellApp, ScriptSite)

HRESULT CShellApp::XFormNames::GetFormName( BSTR bstr_label, BSTR *bstr_name )
{
	_try_nested( CShellApp, FormNames, GetFormName )
	{
		CShellDocManagerDisp *dispImpl = ( ( CShellDocManager * ) pThis->m_pDocManager)->GetDispatchImpl();

		if( dispImpl )
		{
			*bstr_name = dispImpl->LoadString( _bstr_t( bstr_label ) );
			return S_OK;
		}
		return S_FALSE;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::SetNestedMessageLoop( BOOL bSet, BOOL *pbOld )
{
	METHOD_PROLOGUE_EX(CShellApp, App);
	if( pbOld )*pbOld = pThis->m_bNestedLoop;
	pThis->m_bNestedLoop = bSet;
	return S_OK;
}
HRESULT CShellApp::XApp::IsNestedMessageLoop()
{
	METHOD_PROLOGUE_EX(CShellApp, App);
	return pThis->m_bNestedLoop ? S_OK : S_FALSE;
}

HRESULT CShellApp::XApp::IdleUpdate()
{
	METHOD_PROLOGUE_EX(CShellApp, App);
	pThis->OnIdle( 0 );
	return S_OK;
}

HRESULT CShellApp::XApp::GetMainWindowHandle( HWND *phWnd )
{
	_try_nested(CShellApp, App, ProcessMessage)
	{
		if( pThis->m_hWndHook )
			*phWnd = pThis->m_hWndHook;
		else
			*phWnd = AfxGetMainWnd()->GetSafeHwnd();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XHwndHook::EnterHook( HWND hWnd )
{
	_try_nested(CShellApp, HwndHook, EnterHook)
	{
		pThis->m_hWndHook = hWnd;		
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XHwndHook::LeaveHook( )
{
	_try_nested(CShellApp, HwndHook, LeaveHook)
	{
		pThis->m_hWndHook = 0;		
		return S_OK;
	}
	_catch_nested;
}


HRESULT CShellApp::XApp::ProcessMessage()
{
	_try_nested(CShellApp, App, ProcessMessage)
	{

		BOOL	bIdle = FALSE;
		long	lIdleCount = 0;
			// acquire and dispatch messages until a WM_QUIT message is received.

		MSG *msgCur = 0;

#if _MSC_VER >= 1300
		msgCur = AfxGetCurrentMessage();
#else
		msgCur = &AfxGetApp()->m_msgCur;
#endif
		//[ay] тут фигня какая-то была - зачем на каждый мессадж запускать idle цикл	
		// phase2: pump messages while available
		do
		{
			if (!::GetMessage(msgCur, NULL, NULL, NULL))
				return 0;

			if (msgCur->message != WM_KICKIDLE && !AfxGetApp()->PreTranslateMessage(msgCur))
			{
				::TranslateMessage(msgCur);
				::DispatchMessage(msgCur);
			}

			// reset "no idle" state after pumping "normal" message
			if (AfxGetApp()->IsIdleMessage(msgCur)) 
			{
				bIdle = TRUE;
				lIdleCount = 0;
			}

			if( !GetMainFrame()->m_bHelpMode )
				break;

		} while (::PeekMessage(msgCur, NULL, NULL, NULL, PM_NOREMOVE));

		//[ay] а вот тут попробуем
		// phase1: check to see if we can do idle work
		while (bIdle && !::PeekMessage(msgCur, NULL, NULL, NULL, PM_NOREMOVE))
		{
			// call OnIdle while in bIdle state
			if (!AfxGetApp()->OnIdle(lIdleCount++))
				bIdle = FALSE; // assume "no idle" state
		}

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetActionManager( IUnknown **punk )
{
	_try_nested(CShellApp, App, GetActionManager)
	{
		*punk = pThis->m_ptrAM;
		if( *punk )(*punk)->AddRef();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetCommandManager( IUnknown **punk )
{
	_try_nested(CShellApp, App, GetCommandManager)
	{
		*punk = pThis->GetComponent( IID_ICommandManager );
		return S_OK;
	}
	_catch_nested;
}


HRESULT CShellApp::XApp::GetActiveScript( IUnknown **punk )
{
	_try_nested(CShellApp, App, GetActiveScript)
	{
		*punk = g_script.GetControllingUnknown();
		(*punk)->AddRef();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetTargetManager( IUnknown **punk )
{
	_try_nested(CShellApp, App, GetTargetManager)
	{
		*punk = g_TargetManager.GetControllingUnknown();
		(*punk)->AddRef();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetFirstDocTemplPosition( LONG_PTR *plPos )
{
	_try_nested(CShellApp, App, GetFirstDocTemplPosition)
	{
		*plPos = (LONG_PTR)theApp.GetFirstDocTemplatePosition();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetNextDocTempl( LONG_PTR *plPos, BSTR *pbstrName, IUnknown **punk )
{
	_try_nested(CShellApp, App, GetFirstNextTempl)
	{
		CDocTemplate	*ptemplate;
		ptemplate = theApp.GetNextDocTemplate( (POSITION&)*plPos );

		ASSERT( ptemplate );

		CString	strTemplate;

		ptemplate->GetDocString( strTemplate, CDocTemplate::docName );

		if( pbstrName )
			*pbstrName = strTemplate.AllocSysString();

		if (punk)
		{
			*punk = ptemplate->GetControllingUnknown();
			(*punk)->AddRef();
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetFirstDocPosition( LONG_PTR lPosTemplate, LONG_PTR *plPosDoc )
{
	_try_nested(CShellApp, App, GetFirstDocPosition)
	{
		CDocTemplate	*ptemplate;
		
		if(lPosTemplate == 0)
			return E_INVALIDARG;

		ptemplate = theApp.GetNextDocTemplate( (POSITION&)lPosTemplate );
		ASSERT( ptemplate );

		*plPosDoc = (long)ptemplate->GetFirstDocPosition();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetNextDoc( LONG_PTR lPosTemplate, LONG_PTR *plPosDoc, IUnknown **punkDoc )
{
	_try_nested(CShellApp, App, GetNextDoc)
	{
		CDocTemplate	*ptemplate;
		ptemplate = theApp.GetNextDocTemplate( (POSITION&)lPosTemplate );
		ASSERT( ptemplate );

		CDocument *pdoc = ptemplate->GetNextDoc( (POSITION&)*plPosDoc );

		ASSERT( pdoc );
		ASSERT_KINDOF(CShellDoc, pdoc);
		IUnknown	*punk = ((CShellDoc*)pdoc)->GetDocumentInterface();

		if( punkDoc )
		{
			*punkDoc = punk;
		}
		else		
			punk->Release();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetActiveDocument( IUnknown **punkDoc )
{
	_try_nested(CShellApp, App, GetActiveDocument)
	{
		*punkDoc =0;

		CWnd	*pwnd = AfxGetMainWnd();
		if(!pwnd || !pwnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
			return S_OK;

		ASSERT_KINDOF(CMainFrame, pwnd);
		CMainFrame* pFrame = (CMainFrame*)pwnd;

		CMDIChildWnd* pmdi = pFrame->_GetActiveFrame( );

		if( !pmdi )
			return S_OK;

		CDocument	*pdoc = pmdi->GetActiveDocument();
		if( !pdoc )
			return S_OK;

		/*
		CMDIChildWnd	*pmdi = ((CMDIFrameWnd*)pwnd)->MDIGetActive();

		if( !pmdi )
			return S_OK;


		CDocument	*pdoc = pmdi->GetActiveDocument();
		if( !pdoc )
			return S_OK;

		*/


		ASSERT_KINDOF(CShellDoc, pdoc);
		IUnknown	*punk = ((CShellDoc*)pdoc)->GetDocumentInterface();//with addref

		*punkDoc = punk;

		//why??
		//punk->AddRef();
		//
		//punk->Release();

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetMatchDocTemplate( BSTR bstrFileName, IUnknown **punk )
{
	_try_nested(CShellApp, App, GetMatchDocTemplate)
	{
		*punk = 0;
		CString strFileName(bstrFileName);
		CShellDocManager* pManager = pThis->GetDocManager();
		CShellDocTemplate* pTemplate;
		if (pManager)
			pTemplate = pManager->GetMatchDocTemplate(strFileName);
		if (punk && pTemplate)
		{
			*punk = pTemplate->GetControllingUnknown();
			(*punk)->AddRef();
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XApp::GetStatusBar( IUnknown **punk )
{
	_try_nested(CShellApp, App, GetStatusBar)
	{
		if( !punk )
			return E_INVALIDARG;

		CMainFrame* pFrame = (CMainFrame*)pThis->GetMainWnd();
		if(  pFrame )
		{
			*punk = pFrame->m_wndStatusBar.GetControllingUnknown();
			if( *punk )(*punk)->AddRef();
		}
		else
			*punk = 0;


		return S_OK;
	}
	_catch_nested;
}


// IVTApplication

HRESULT CShellApp::XGuard::InitApp( BYTE* pBuf, DWORD dwData )
{
	_try_nested(CShellApp, Guard, InitApp)
	{
		return S_OK;
	}
	_catch_nested;
}

// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
HRESULT CShellApp::XGuard::GetData(DWORD * pKeyGUID, BYTE ** ppTable, BSTR * pbstrSuffix, DWORD * pdwImito)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
	{
		if (pKeyGUID)
			*((GUID*)pKeyGUID) = pThis->m_innerclsid;	

		if (ppTable)
			*ppTable = (BYTE*)pThis->m_pTable;
		
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
}

// return ptr on KeyGuid, ptr on TranslationTable, szSuffix, imito
HRESULT CShellApp::XGuard::SetData(DWORD * pKeyGUID, BYTE ** ppTable, BSTR * pbstrSuffix, DWORD * pdwImito)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
	{
		if (pKeyGUID)
			pThis->m_innerclsid = *((GUID*)pKeyGUID);	

		if (ppTable)
			*ppTable = (BYTE*)pThis->m_pTable;
		{
			pThis->m_pTable->SetKey(GuidKey(pThis->m_innerclsid), 0);
		}

		if (pbstrSuffix)
			pThis->m_strSuffix = *pbstrSuffix;

		if (pdwImito)
			pThis->m_dwImito = *pdwImito;

		return S_OK;
	}
}

// get mode (register or not)
HRESULT CShellApp::XGuard::GetMode(BOOL * pbRegister)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
	{
		if (!pbRegister)
			return E_POINTER;

		*pbRegister = false;
		return S_OK;
	}
}


// Add guid to table
HRESULT CShellApp::XGuard::AddEntry(DWORD * pIntGUID, DWORD * pExtGUID, BSTR bstrIntProgID, DWORD dwData)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
	{
		CString str = bstrIntProgID;
		if (!pIntGUID || !pExtGUID || str.IsEmpty())
			return E_POINTER;

		CTranslateTable * pTable  = pThis->m_pTable;
		if (!pTable)
			return E_FAIL;

		return pTable->Add(GuidKey(*((GUID*)pExtGUID)), GuidKey(*((GUID*)pIntGUID)), str, dwData) ? S_OK : S_FALSE;
	}
}

// Remove guid from table
HRESULT CShellApp::XGuard::RemoveEntry(DWORD * pExtGUID, BSTR bstrIntProgID)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
	{
		CString str = bstrIntProgID;
		CTranslateTable * pTable  = pThis->m_pTable;
		if (!pTable)
			return E_FAIL;

		bool bRet = (pExtGUID) ? pTable->Remove(GuidKey(*((GUID*)pExtGUID))) : (!str.IsEmpty() ? pTable->Remove(str) : false);

		return bRet ? S_OK : E_INVALIDARG;
	}
}

// get inner guid from table by extern guid	: mode 0
// get extern guid by inner guid			: mode 1
HRESULT CShellApp::XGuard::GetEntry(DWORD dwMode, DWORD * pSrcGUID, DWORD * pDstGUID, DWORD * pdwData)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
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
}


// Get External GUID from internal/external ProgID : mode 0
// Get internal GUID from internal/external ProgID : mode 1
HRESULT CShellApp::XGuard::GetModeData(DWORD dwMode, DWORD * pGUID, BSTR bstrProgID)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
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
}


// translations	
// get external ProgID from internal ProgID : mode 0
// get internal ProgID from external ProgID : mode 1
HRESULT CShellApp::XGuard::GetDataValue(DWORD dwMode, BSTR bstrSrcProgID, BSTR * pbstrDstProgID)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
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
}

HRESULT CShellApp::XGuard::GetNames(BSTR * pbstrAppName, BSTR * pbstrCompanyName)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
	{
		if (pbstrAppName)
			*pbstrAppName = pThis->m_strGuardAppName.AllocSysString();
		if (pbstrCompanyName)
			*pbstrCompanyName = pThis->m_strGuardCompanyName.AllocSysString();
		return S_OK;
	}
}

HRESULT CShellApp::XGuard::GetRegKey(BSTR * pbstrRegKey)
{
	METHOD_PROLOGUE_EX(CShellApp, Guard)
	{
		if (!pbstrRegKey)
			return E_POINTER;

		CString strRet("SOFTWARE\\");
		strRet += pThis->m_strGuardCompanyName + _T("\\");
		strRet += pThis->m_strGuardAppName + _T("\\add-ins");

		*pbstrRegKey = strRet.AllocSysString();
		return S_OK;
	}
}



////

bool CShellApp::ReadGuardInfo(LPCTSTR szFile)
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
		DWORD dwKeyId = 0;

		file.Read(&dwKeyId, sizeof(DWORD));
		GuardSetNSKInfo(&dwKeyId);

		// read size of app_suffix string
		LONG lSufSize = 0;
		file.Read(&lSufSize, sizeof(LONG));

		CString strSuff(m_strSuffix);
		if (lSufSize)
		{
			// save app_name_string
			file.Read(strSuff.GetBuffer(lSufSize + 1), lSufSize);
			strSuff.ReleaseBuffer( lSufSize );
			if (strSuff.GetLength() > lSufSize)
				strSuff.SetAt(lSufSize, '\0');
		}
		GuardSetSuffix(strSuff);
	}
	catch (CFileException * e)
	{
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
		ASSERT(e->ReportError()== IDOK);
		e->Delete();

		return false;
	}
	return true;
}


bool CShellApp::AddOwnComponents()
{
	IVTApplicationPtr ptr_app( ::GetAppUnknown() );
	if( ptr_app == 0 )
		return false;
	//paul 25.02.2003. Append translate table to avoid register new guid at each start
	{
		AFX_MODULE_STATE* pModuleState = AfxGetModuleState();
		for (COleObjectFactory* pFactory = pModuleState->m_factoryList;
			pFactory != NULL; pFactory = pFactory->m_pNextFactory)
		{
			if( pFactory->IsKindOf( RUNTIME_CLASS(CVTFactory) ) )
			{
				CVTFactory* pvtf	= (CVTFactory*)pFactory;
				CLSID clsid			= pvtf->GetClassID();
				CString str_progid	= pvtf->GetProgID();

				ptr_app->AddEntry( (DWORD*)&clsid, (DWORD*)&clsid, _bstr_t(str_progid), 0 );
			}
		}

	}

	return true;
}


int CShellApp::ExitInstance() 
{
//	COleObjectFactory::UpdateRegistryAll(false);

	if (m_hErrorThread)
		WaitForSingleObject(m_hErrorThread, INFINITE);
	::StealthDeinit();

//	InitAppUnknown(0);
	delete m_pTable;
	m_pTable = 0;

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
	if (m_bCreateInstanceEx)
		UninstallCreateInstanceEx();
	m_bCreateInstanceEx = false;
#endif // DCOM

	if (m_bProgIDFromCLSID)
		UninstallProgIDFromCLSID();

	if (m_bCLSIDFromProgID)
		UninstallCLSIDFromProgID();

	if (m_bGetClassObject)
		UninstallGetClassObject();

	if (m_bCreateInstance)
		UninstallCreateInstance();

	m_bCreateInstance = m_bGetClassObject = m_bCLSIDFromProgID = m_bProgIDFromCLSID = false;


	return CWinApp::ExitInstance();
}


void CShellApp::FreeComponents()
{
	//clear cache map for interface
	g_map_types.clear();
	g_map_fi.clear();	
	m_benable_map = false;

	//m_ptrAM = 0; // переносим - чтобы при закрытии окна были доступны акции

	POSITION	pos = GetFirstDocTemplatePosition();
	while( pos )
	{
		CShellDocTemplate	*ptempl = (CShellDocTemplate*)GetNextDocTemplate( pos );
		ptempl->SaveState();
	}
	//terminate the current interactive acition
	/*it's moved to CMainFrame::OnClose()
	{
		sptrIApplication	sptrA( GetAppUnknown() );
		IUnknown		*punkAM = 0;
		sptrA->GetActionManager( &punkAM );
		sptrIActionManager	sptrAM( punkAM );
		punkAM->Release();

		sptrAM->TerminateInteractiveAction();
	}*/

	if( m_pDocManager )//calling FreeComponents first time
	{
		::FireEvent( GetControllingUnknown(), szBeforeAppTerminate );
		((CShellDocManager*)m_pDocManager)->DeInit();

		//store the application NamedData
		/*
		//!!! debug
		{
			sptrINamedDataLogger sptrLogger( GetControllingUnknown() );
			IUnknownPtr sptr1;
			sptrLogger->GetLog(ndlkRead, &sptr1);
			sptrIFileDataObject2 sptrF1(sptr1);
			sptrF1->SaveTextFile( _bstr_t("C:\\used.data") );
		}
		{
			sptrINamedDataLogger sptrLogger( GetControllingUnknown() );
			IUnknownPtr sptr1;
			sptrLogger->GetLog(ndlkWrite, &sptr1);
			sptrIFileDataObject2 sptrF1(sptr1);
			sptrF1->SaveTextFile( _bstr_t("C:\\written.data") );
		}
		*/

		sptrIFileDataObject2	sptrF( GetControllingUnknown() );
		_bstr_t	bstrFileName = MakeAppPathName( AppDataFilename );
		sptrF->SaveTextFile( bstrFileName );
	}
	
	m_ptrAM = 0; // переносим - чтобы при закрытии окна были доступны акции

	delete m_pDocManager;
	m_pDocManager = 0;

	g_TargetManager.DeInit();

//remove all entries
	DeleteEntry( GetAppUnknown(), 0 );
	
	g_CmdManager.DeInit();
	g_script.DeInit();
//delete plug-in windows
	CMainFrame	*pmain = (CMainFrame*)m_pMainWnd;

	pos = pmain->GetFirstDockBarPos();

	while( pos )
	{
		CControlBar	*pbar = pmain->GetNextDockBar( pos );
		if(pbar && pbar->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
		{
			delete pbar;
			pbar = 0;
		}
	}

//deinit application component manager
	CCompManagerImpl::DeInit();

	m_aggrs.DeInit();
	

	CCmdTargetEx::Dump();
}

	

//return application document manager
CShellDocManager *CShellApp::GetDocManager() const
{
	return dynamic_cast<CShellDocManager*>(m_pDocManager);
}

void CShellApp::OnFinalRelease()
{
	//...
}

BOOL CShellApp::PreTranslateMessage(MSG* pMsg) 
{
//	time_test	t( "CShellApp::PreTranslateMessage" );
	/*if( !IsWindow( pMsg->hwnd ) )
		return true;*/
	// TODO: Add your specialized code here and/or call the base class
	if( (pMsg->message == WM_KEYDOWN) || // && pMsg->wParam == VK_DELETE ) ||
		(pMsg->message == WM_SYSKEYDOWN))// && pMsg->wParam == VK_MENU ))
	{
		char	sz[255];
		::GetClassName( pMsg->hwnd, sz, 255 );
		int ncode = (int) pMsg->wParam;
		if (ncode == VK_RETURN)
		{
			ncode = VK_RETURN;
//			return false;
		}
		if( !strcmp( sz, "Edit" ))
		{
			return false;
		}
//		if (!strcmp(sz, TITLETIP_CLASSNAME))
//		{
//			return false;
//		}

	}
/*	else if (pMsg->message == WM_LBUTTONDOWN ||
			 pMsg->message == WM_RBUTTONDOWN ||
			 pMsg->message == WM_MBUTTONDOWN)

	{
		char	sz[255];
		::GetClassName(pMsg->hwnd, sz, 255);

		if (!strcmp(sz, TITLETIP_CLASSNAME))
		{
			CWnd * pWnd = CWnd::FromHandlePermanent(pMsg->hwnd);

//			return pWnd ? pWnd->PreTranslateMessage(pMsg) : false;;
		}
	}*/
	if( m_pMainWnd )
	{
		if( pMsg->message == WM_NCLBUTTONDOWN ||
			pMsg->message == WM_NCLBUTTONUP ||
			pMsg->message == WM_NCRBUTTONDOWN ||
			pMsg->message == WM_NCRBUTTONUP )
		{
			((CMainFrame*)m_pMainWnd)->KillActivePopup();
		}
	}
	if( pMsg->message == WM_KEYDOWN && pMsg->wParam == 'D' && 
		( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0 ) && 
		( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )
		)
	{
		HMODULE hmod = GetModuleHandle("dump.dll");
		if( hmod )
		{
			typedef  void (*PF_DUMP_OPTIONS)( HWND hwnd_main );
			PF_DUMP_OPTIONS pfunc = (PF_DUMP_OPTIONS)GetProcAddress( hmod, "hook_options"); 
			if (pfunc)
				pfunc( 0 );
		}

	}


	return CWinApp::PreTranslateMessage(pMsg);
}

//ovverided for prevent inializing idle mode by WM_TIMER message. Decrease dummy cpu ussage when 
//timer is set
BOOL CShellApp::IsIdleMessage(MSG* pMsg)
{
	if( !CWinApp::IsIdleMessage( pMsg ) )
		return false;

	//[ay] - нельзя совсем запрещать переход в состояние idle по WM_TIMER - 
	//в активном состоянии не приходит больше никаких сообщений, и поэтому система
	//не переходит в idle mode вообще.

	//переходить после каждого таймера в идле тоже не круто, поэтому сохраняем последнее
	//сообщение, если это таймер, или другое незначимое событие, то в идлю не идем

	if( pMsg->message == WM_NCPAINT )return false;
	if( pMsg->message >= WM_USER )return false;
	if( pMsg->message == 674 )return false;

	static unsigned last_message = 0;
	bool	last_message_idle = 
		last_message == WM_TIMER ||
		last_message == WM_MOUSEMOVE ||
/*		last_message == WM_LBUTTONDOWN ||
		last_message == WM_LBUTTONUP ||
		last_message == WM_RBUTTONDOWN ||
		last_message == WM_RBUTTONUP ||
		last_message == WM_NCLBUTTONDOWN ||
		last_message == WM_NCLBUTTONUP ||
		last_message == WM_NCRBUTTONDOWN ||
		last_message == WM_NCRBUTTONUP ||*/
		last_message == WM_NCMOUSEMOVE /*||
		last_message == WM_KEYDOWN ||
		last_message == WM_KEYUP ||
		last_message == WM_CHAR*/;

	last_message = pMsg->message;

	if( last_message == WM_TIMER ||
		last_message == WM_MOUSEMOVE ||
/*		last_message == WM_LBUTTONDOWN ||
		last_message == WM_LBUTTONUP ||
		last_message == WM_RBUTTONDOWN ||
		last_message == WM_RBUTTONUP ||
		last_message == WM_NCLBUTTONDOWN ||
		last_message == WM_NCLBUTTONUP ||
		last_message == WM_NCRBUTTONDOWN ||
		last_message == WM_NCRBUTTONUP ||*/
		last_message == WM_NCMOUSEMOVE /*||
		last_message == WM_KEYDOWN ||
		last_message == WM_KEYUP ||
		last_message == WM_CHAR*/ )
	{
		if (last_message == WM_TIMER || last_message == WM_MOUSEMOVE || last_message == WM_NCMOUSEMOVE)
			return !last_message_idle;
		else
			return true;
	}
	if( pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN )
	{
		DWORD	flags = 0;
		m_ptrAM->GetCurrentExecuteFlags( &flags );
		return (aefInteractiveRunning & flags)==0;
	
	}

	return true;
}

class CIncLongCounter
{
public:
	CIncLongCounter(long* pnCounter) { m_pnCounter=pnCounter; (*m_pnCounter)++; }
	~CIncLongCounter() { (*m_pnCounter)--; }
private:
	long* m_pnCounter;
};

class CTimeLimiter
{
public:
	inline CTimeLimiter(DWORD dwPeriod)
	{
		m_dwPeriod = dwPeriod;
		m_dwLastTime = GetTickCount();
	}
	inline bool Ready()
	{
		DWORD t = GetTickCount();
		if( t - m_dwLastTime >= m_dwPeriod)
		{
			m_dwLastTime = t;
			return true;
		}
		return false;
	}
protected:
	DWORD m_dwPeriod, m_dwLastTime;
};

BOOL CShellApp::PumpMessage()
{
	// увеличим глубину на время вызова; теперь =1 - внешний цикл
	CIncLongCounter cnt1(&m_nPumpMessageDepth);

	if(m_nPumpMessageDepth==1)
	{
		static CTimeLimiter TimeLimiter(20);
		if(TimeLimiter.Ready())
		{
			if( GetValueInt(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", 0) )
			{
				SetValue(::GetAppUnknown(), "\\MainFrame", "FireEventInMainLoop", long(0));
				::FireEvent( GetControllingUnknown(), szEventAppOnMainLoop);
			}
		}
		if( GetValueInt(::GetAppUnknown(), "\\MainFrame", "NotifyInMainLoop", 0) )
		{
			SetValue(::GetAppUnknown(), "\\MainFrame", "NotifyInMainLoop", long(0));
			FireScriptEvent("Application_OnMainLoop");
		}
	}

	MSG *msgCur = 0;
#if _MSC_VER >= 1300
		msgCur = AfxGetCurrentMessage();
#else
		msgCur = &AfxGetApp()->m_msgCur;
#endif

		//ASSERT( msgCur->message != WM_LBUTTONDOWN );
//	TRACE( "CShellApp::PumpMessage %d\n", msgCur->message );
//	if( msgCur->message == WM_TIMER  )
//		return GetMessage(msgCur, NULL, NULL, NULL);

/*	char	sz[100];
	sprintf( sz, "message %04x - %04x %04x %04x", msgCur->hwnd, msgCur->message, msgCur->wParam, msgCur->lParam );
	time_test	t( sz, false );

	if( msgCur->message == WM_TIMER &&
		(msgCur->wParam == 0x200 ||
		 msgCur->wParam == 0x309 ) )
	{
		GetMessage( msgCur, 0, 0, 0 );
		return true;
	}*/

	if( CBCGToolBar::IsCustomizeMode() )
	{
		if (!::GetMessage(msgCur, NULL, NULL, NULL))
			return FALSE;

		HWND	hwndDlg = msgCur->hwnd;
		while( hwndDlg != 0 )
		{
			if( ::GetWindowLong( hwndDlg, GWL_STYLE  ) & WS_POPUP )
			{
				if( ::IsDialogMessage(hwndDlg, msgCur) )
					return true;
				break;
			}
			hwndDlg = ::GetParent( hwndDlg );
		}

		if( msgCur->message != WM_KICKIDLE )
		{
			::TranslateMessage(msgCur);
			::DispatchMessage(msgCur);
		}

		return TRUE;
	}
	else
	{
		BOOL ret = CWinApp::PumpMessage();
		return ret;
	}
}

//help support
void CShellApp::WinHelp(DWORD dwData, UINT nCmd) 
{
	m_nCurrentHelpCommand = nCmd;
	m_dwCurrentHelpTopicID = dwData;
}

UINT	CShellApp::GetCurrentHelpCommand() const		//return a current help command
{	return m_nCurrentHelpCommand;	}
DWORD	CShellApp::GetCurrentTopicID() const			//return a current topic id
{	return m_dwCurrentHelpTopicID;	}


void CShellApp::OnTimerFire()
{
	return;
	long ltick = ::GetTickCount();
	
	//not need
	if( m_last_idle_tick == -1 )
		return;
	
	if( m_last_idle_tick > ltick || ltick - m_last_idle_tick > 1000 )
	{
		CWinApp::OnIdle( m_last_idle_count );
		m_last_idle_tick = -1;
		return;
	}

	//m_last_idle_tick = ltick;
}

void CShellApp::ForceIdleUpdate()
{
	m_last_idle_tick = 0;
	//OnTimerFire();
	OnIdle( 0 );
}

BOOL CShellApp::OnIdle(LONG lCount) 
{
	DO_LOCK_R(FALSE);
	//TIME_TEST( "CShellApp::OnIdle" )
	//это - контроль актиавизации окна после выхода.
	//тоже ситуация из бредовых, однако...
	if( lCount == 0 && m_ptrAM == 0 )
		return false;

	m_last_idle_tick	= ::GetTickCount();
	m_last_idle_count	= lCount;

	//FireScriptEvent( "Application_OnIdle" );
	//::FireEvent( GetControllingUnknown(), szEventAppOnIdle);

	return CWinApp::OnIdle(lCount);

	//CTimeTest time( true, "-----------------------OnIdle" );

	/*
	//это - контроль актиавизации окна после выхода.
	//тоже ситуация из бредовых, однако...
	if( lCount == 0 && m_ptrAM == 0 )
		return false;

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
	// check MFC's allocator (before idle)
	if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
		ASSERT(AfxCheckMemory());
#endif


	if (lCount <= 0)
	{
		// send WM_IDLEUPDATECMDUI to the main window
		CWnd* pMainWnd = m_pMainWnd;
		if (pMainWnd != NULL && pMainWnd->m_hWnd != NULL &&
			pMainWnd->IsWindowVisible())
		{
			AfxCallWndProc(pMainWnd, pMainWnd->m_hWnd,
				WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
			pMainWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
				(WPARAM)TRUE, 0, TRUE, TRUE);
		
			
//We've trouble with Idle , so can't have calling CFrameWnd::OnIdleUpdateCmdUI()
		}
		// send WM_IDLEUPDATECMDUI to all frame windows

		AFX_MODULE_THREAD_STATE* pState = _AFX_CMDTARGET_GETSTATE()->m_thread;
		
		CFrameWnd* pFrameWnd = pState->m_frameList;
		while (pFrameWnd != NULL)
		{
			if (pFrameWnd->m_hWnd != NULL && pFrameWnd != pMainWnd)
			{
				if (pFrameWnd->m_nShowDelay == SW_HIDE)
					pFrameWnd->ShowWindow(pFrameWnd->m_nShowDelay);
				if (pFrameWnd->IsWindowVisible() ||
					pFrameWnd->m_nShowDelay >= 0)
				{
					AfxCallWndProc(pFrameWnd, pFrameWnd->m_hWnd,
						WM_IDLEUPDATECMDUI, (WPARAM)TRUE, 0);
					pFrameWnd->SendMessageToDescendants(WM_IDLEUPDATECMDUI,
						(WPARAM)TRUE, 0, TRUE, TRUE);
						
				}
				if (pFrameWnd->m_nShowDelay > SW_HIDE)
					pFrameWnd->ShowWindow(pFrameWnd->m_nShowDelay);
				pFrameWnd->m_nShowDelay = -1;
			}
			pFrameWnd = pFrameWnd->m_pNextFrameWnd;
		}		

		//sptrAM->FlushActionStateCache( 0 );
	}
	else if (lCount >= 0)
	{
		AFX_MODULE_THREAD_STATE* pState = _AFX_CMDTARGET_GETSTATE()->m_thread;

		if (pState->m_nTempMapLock == 0)
		{
			// free temp maps, OLE DLLs, etc.
			AfxLockTempMaps();
			AfxUnlockTempMaps();
		
		}		
	}

#if defined(_DEBUG) && !defined(_AFX_NO_DEBUG_CRT)
	// check MFC's allocator (after idle)
	if (_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) & _CRTDBG_CHECK_ALWAYS_DF)
		ASSERT(AfxCheckMemory());
#endif

	return lCount < 0;  // nothing more to do if lCount >= 0
	*/
}

HRESULT CShellApp::XScriptSite::RegisterScript( IUnknown *punk, DWORD dwFlags )
{
	_try_nested( CShellApp, ScriptSite, RegisterScript )
	{
		if( !punk )
			return S_FALSE;

		CScriptDesc desc( punk, dwFlags );
		pThis->m_arrScriptPUNK.add_tail( desc );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XScriptSite::UnregisterScript( IUnknown *punk, DWORD dwFlags  )
{
	_try_nested( CShellApp, ScriptSite, UnregisterScript )
	{
		if( !punk )
			return S_FALSE;

		TPOS lPos = pThis->m_arrScriptPUNK.find( CScriptDesc( punk, dwFlags ) );
		if( lPos )
			pThis->m_arrScriptPUNK.remove( lPos );
		else
			return S_FALSE;

		return S_OK;
	}
	_catch_nested;
}


HRESULT CShellApp::XScriptSite::Invoke( BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult, DWORD dwFlags )
{
	_try_nested( CShellApp, ScriptSite, Invoke )
	{
		static long lenable_script_log = -1;
		if( lenable_script_log == -1 )
		{
			lenable_script_log = ::GetValueInt( ::GetAppUnknown(), "\\Logging", "ScriptLogging", 0L );
			//::DeleteFile( GetLogFileName() );
		}
		if( lenable_script_log == 1 )
		{
			CString strFuncName = bstrFuncName;
			_trace_file( "Script.log", "%s", (const char*)strFuncName );
		}

		for( TPOS lPos = pThis->m_arrScriptPUNK.head(); lPos != 0;  lPos = pThis->m_arrScriptPUNK.next( lPos ) )
		{
			CScriptDesc desc = pThis->m_arrScriptPUNK.get( lPos );
			IActiveScriptPtr ptrAct = desc.GetScript();
			DWORD dwFlag = desc.GetFlag();

			if( dwFlag & dwFlags )
			{
				if( ptrAct != 0 )
				{
					IDispatch *pDisp = 0;
					ptrAct->GetScriptDispatch( 0, &pDisp );
					if( pDisp )
					{
						HRESULT hr = pThis->_invoke_script_func( pDisp, bstrFuncName, pargs, nArgsCount, pvarResult );
						pDisp->Release();
						if( hr == S_OK )
							return S_OK;
					}
				}
			}
		}
		return S_FALSE;
	}
	_catch_nested;
}



HRESULT CShellApp::XScriptSite::GetFirstPos(TPOS *dwPos )
{
	_try_nested( CShellApp, ScriptSite, GetFirstPos )
	{
		if( !dwPos )
			return S_FALSE;

		*dwPos = pThis->m_arrScriptPUNK.head();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XScriptSite::GetNextPos( TPOS *dwPos, IUnknown **punk, DWORD *dwFlags )
{
	_try_nested( CShellApp, ScriptSite, GetNextPos )
	{
		if( !punk || !dwPos || !dwFlags )
			return S_FALSE;

		CScriptDesc desc = pThis->m_arrScriptPUNK.get( *dwPos );

		*punk = desc.GetScript( true );
		*dwFlags = desc.GetFlag();

		*dwPos = pThis->m_arrScriptPUNK.next( *dwPos );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::XScriptSite::UnregisterAll()
{
	_try_nested( CShellApp, ScriptSite, GetPrevDispatchPos )
	{
		pThis->m_arrScriptPUNK.clear();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CShellApp::_invoke_script_func( IDispatch *pDisp, BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult )
{
		CString szFuncName( bstrFuncName );

		if( szFuncName.IsEmpty() )
			return S_FALSE;

		CString str = szFuncName;
		BSTR bstr = str.AllocSysString( );
		DISPID id = -1;
		HRESULT hr = pDisp->GetIDsOfNames( IID_NULL, &bstr, 1, LOCALE_USER_DEFAULT, &id );
		if( hr != S_OK )
		{
			::SysFreeString( bstr );	bstr = 0;
			return S_FALSE;
		}
		
		::SysFreeString( bstr );	bstr = 0;

		DISPPARAMS dispparams;	    

		ZeroMemory( &dispparams, sizeof(dispparams) );	
		
		dispparams.rgvarg			= pargs;
		dispparams.cArgs			= nArgsCount;
		dispparams.cNamedArgs		= 0;
			
		VARIANT vt;
		ZeroMemory( &vt, sizeof(vt) );

		hr = pDisp->Invoke( 
						id, 
						IID_NULL, 
						LOCALE_SYSTEM_DEFAULT,
						DISPATCH_METHOD,
						&dispparams,
						&vt,
						NULL,
						NULL
						);

		if( pvarResult )
			*pvarResult = _variant_t( vt );

		return S_OK;
}

IUnknown* CShellApp::GetInterfaceHook( const void* p )
{	
	return get_fast_interface( (const GUID*)p );
}

IUnknown* CShellApp::raw_get_interface( const GUID* piid )
{	
	IUnknown* punk = GetInterface( piid );
	if( !punk )
	{
		punk = QueryAggregates( piid );
		if( punk )
			punk->Release();
	}		
	return punk;
}
