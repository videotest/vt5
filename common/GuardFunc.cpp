// GuardFunc.cpp : 
//
#include "stdafx.h"


#include "GuardInt.h"
#include "GuardFunc.h"
//#include "TransTable.h"
#include "Utils.h"
#include "Factory.h"



// handled of kernel objects
export_data HANDLE	g_hMutex = 0;
export_data HANDLE	g_hEventEnd = 0;
export_data HANDLE	g_hEventError = 0;

// error code
export_data DWORD	g_dwErrorCode = 0;
export_data LPCTSTR	g_arrErrStrings[] = 
{
	_T("No error..."),										//guardNoError = 0,
	_T("Protection error during Initalization : unable to create Protected key's file"), //guardCreateFileMapping,
	_T("Protection error : unable to open Protected key's file "),			//guardOpenFileMapping,
	_T("Protection error : Map File"),			//guardMapFile,
	_T("Protection error : Novex Stealth Key requred"), //guardNotExistsNSK,
	_T("Protection error : invalid Novex Stealth Key"), //guardInvalidNSK,
	_T("Protection error : unable to read Novex Stealth Key "), //guardReadNSK,
	_T("Protection error : unable to write Novex Stealth Key "), //guardWriteNSK,

	_T("Protection error : bad CRC for protected data "), //guardInvalidPwd,
	_T("Protection error : invalid Protected key's file "), //guardInvalidGuardFile,
	_T("Unexpected error : unable to read file "), //guardReadGuardFile,
	_T("Unexpected error : unable to write file "), //guardWriteGuardFile,
	_T("Unexpected error : read bad CRC for protected data file "), //guardCryptFile,
	_T("Unexpected error : write bad CRC for protected data file  "), //guardDecryptFile,
	_T("Unexpected error : invalid protected data size "), //guardInvalidTableSize,
	_T("Unexpected error : invalid protected data "), //guardInvalidTable,
	_T("UnexpectedError : not enought memory...\n Close unused applications and try again"), //guardMemory

}; 

// return Application is alredy running
std_dll bool GuardAppIsRunning()
{
	SetLastError(0);
	HANDLE hMutex = OpenMutex(NULL, true, MUTEXNAME);
	if (!hMutex)
		return false;
	ReleaseMutex(hMutex);
	return true;
}

std_dll LPCTSTR GuardGetErrorMsg(guardErrorCode eCode)
{
	LPCTSTR szString = 0;
	HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
	if (hMutex)
	{
		WaitForSingleObject(hMutex, INFINITE);

		szString = g_arrErrStrings[(int)eCode];

		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return szString;
}

// return errorCode
std_dll DWORD GuardGetErrorCode()
{
	DWORD dwCode = -1;
	HANDLE hMutex = ::OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
	if (hMutex)
	{
		WaitForSingleObject(hMutex, INFINITE);

		dwCode = g_dwErrorCode;

		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}

	return dwCode;
}

// Set errorCode
std_dll void GuardSetErrorCode(DWORD dwCode)
{
	HANDLE hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
	if (hMutex)
	{
		WaitForSingleObject(hMutex, INFINITE);
		g_dwErrorCode = dwCode;

		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
	}
}

std_dll DWORD __stdcall ThreadFuncStealthError(LPVOID lpParam)
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

// global data
/*
BYTE __CreateInstanceOriginalCode[20]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE __GetClassObjectOriginalCode[28]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE __ProgIDFromCLSIDOriginalCode[22]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE __CLSIDFromProgIDOriginalCode[20]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
*/

BYTE __CreateInstanceOriginalCode[16]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE __GetClassObjectOriginalCode[16]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE __ProgIDFromCLSIDOriginalCode[16]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE __CLSIDFromProgIDOriginalCode[16]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };



std_dll BYTE* GetDataCreateInstance()
{
	return __CreateInstanceOriginalCode;
}

std_dll BYTE* GetDataGetClassObject()
{
	return __GetClassObjectOriginalCode;
}

std_dll BYTE* GetDataCLSIDFromProgID()
{
	return __CLSIDFromProgIDOriginalCode;
}

std_dll BYTE* GetDataProgIDFromCLSID()
{
	return __ProgIDFromCLSIDOriginalCode;
}



#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
/*
BYTE __CreateInstanceExOriginalCode[20]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
*/
BYTE __CreateInstanceExOriginalCode[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

std_dll BYTE* GetDataCreateInstanceEx()
{
	return __CreateInstanceExOriginalCode;
}
#endif // DCOM

static void GetPageInfo(LPVOID lp, LPVOID * lpBase = 0, LONG * plSize = 0, bool * pbNeedChangeProtect = 0)
{
	if (lpBase)
		*lpBase = 0;
	
	if (plSize)
		*plSize = 0;

	if (pbNeedChangeProtect)
		*pbNeedChangeProtect = false;

	SYSTEM_INFO si;
	GetSystemInfo(&si);// get system information

	MEMORY_BASIC_INFORMATION mbi;
	if (VirtualQuery(lp, &mbi, sizeof(mbi)) == sizeof(mbi))
	{
		if (lpBase)
			*lpBase = mbi.AllocationBase;

		if (plSize)
			*plSize = mbi.RegionSize;

		if (pbNeedChangeProtect)
			*pbNeedChangeProtect = !(mbi.Protect == PAGE_EXECUTE_WRITECOPY || mbi.Protect == PAGE_EXECUTE_READWRITE);
	}
}

// implements of functions declared in Guard.h
// CoCreateInstance
std_dll bool InstallCreateInstance(LPVOID lpvFuncAddr)
{
//	LPVOID lpvFuncAddr = &CoCreateInstanceNew;
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CoCreateInstance")) : 0;

	if (!(lpvFuncAddr && hModuleO && lp))
		return false;

	// if original code is not saved
	if (__CreateInstanceOriginalCode[0] != 0x90)
	{
		// set all NOP to saved code
		memset(__CreateInstanceOriginalCode, 0x90, 16);
		// and copy piece of original code to this place
		memcpy(&__CreateInstanceOriginalCode[1], (BYTE*)lp, 7);
	
		DWORD dw = (DWORD)lp;
		memcpy(&__CreateInstanceOriginalCode[8], &dw, sizeof(DWORD));

		memcpy(&__CreateInstanceOriginalCode[12], &lpvFuncAddr, sizeof(DWORD));
	}

	// change protection for .dll mapping page if it's needed
	LPVOID lpBase = 0;
	LONG lSize = 0;
	bool bNeedChangeProtect = false;
	GetPageInfo(lp, &lpBase, &lSize, &bNeedChangeProtect);
	if (lpBase && lSize && bNeedChangeProtect)
	{
		SetLastError(0);
		DWORD dwOldProtect = 0;
		BOOL b = VirtualProtect(lpBase, lSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (ERROR_SUCCESS != GetLastError())
		{
			AfxMessageBox( "InstallCreateInstance:ERROR_SUCCESS != GetLastError()" );
			return false;
		}
	}

	// modify original code
	BYTE * ptr = (BYTE*)lp;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &lpvFuncAddr, sizeof(DWORD));
	ptr += 4;
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}


// implements of functions declared in Guard.h
// CoCreateInstance
std_dll bool InstallCustomHook(void *lpvFuncAddr, void *pfuncOld )
{
	LPVOID lp = pfuncOld;

	if (!(lpvFuncAddr && lp))
		return false;

	// change protection for .dll mapping page if it's needed
	LPVOID lpBase = 0;
	LONG lSize = 0;
	bool bNeedChangeProtect = false;
	GetPageInfo(lp, &lpBase, &lSize, &bNeedChangeProtect);
	if (lpBase && lSize && bNeedChangeProtect)
	{
		SetLastError(0);
		DWORD dwOldProtect = 0;
		VirtualProtect(lpBase, lSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (ERROR_SUCCESS != GetLastError())
		{
			AfxMessageBox( "InstallCustomHook:ERROR_SUCCESS != GetLastError()" );
			return false;
		}
	}

	// modify original code
	BYTE * ptr = (BYTE*)lp;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &lpvFuncAddr, sizeof(DWORD));
	ptr += 4;
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}


std_dll bool UninstallCreateInstance()
{
/*
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CoCreateInstance")) : 0;

	if (!(hModuleO && lp))
		return false;
*/
	if (__CreateInstanceOriginalCode[0] == 0x90)
	{
		DWORD dw = 0;
		memcpy(&dw, &__CreateInstanceOriginalCode[8], sizeof(DWORD));
		if (dw)
		{
			memcpy((BYTE*)dw, &__CreateInstanceOriginalCode[1], 7);
			// set all NOP to saved code
			memset(__CreateInstanceOriginalCode, 0, 16);
		}
	}	
	
	return true;
}



#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
// CoCreateInstanceEx
std_dll bool InstallCreateInstanceEx(LPVOID lpvFuncAddr)
{
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CoCreateInstanceEx")) : 0;

	if (!(lpvFuncAddr && hModuleO && lp))
		return false;

	// if original code is not saved
	if (__CreateInstanceExOriginalCode[0] != 0x90)
	{
		// set all NOP to saved code
		memset(__CreateInstanceExOriginalCode, 0x90, 16);
		// and copy piece of original code to this place
		memcpy(&__CreateInstanceExOriginalCode[1], (BYTE*)lp, 7);
	
		DWORD dw = (DWORD)lp;
		memcpy(&__CreateInstanceExOriginalCode[8], &dw, sizeof(DWORD));

		memcpy(&__CreateInstanceExOriginalCode[12], &lpvFuncAddr, sizeof(DWORD));
	}

	// change protection for .dll mapping page if it's needed
	LPVOID lpBase = 0;
	LONG lSize = 0;
	bool bNeedChangeProtect = false;
	GetPageInfo(lp, &lpBase, &lSize, &bNeedChangeProtect);
	if (lpBase && lSize && bNeedChangeProtect)
	{
		SetLastError(0);
		DWORD dwOldProtect = 0;
		VirtualProtect(lpBase, lSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (ERROR_SUCCESS != GetLastError())
		{
			AfxMessageBox( "InstallCreateInstanceEx:ERROR_SUCCESS != GetLastError()" );
			return false;
		}
	}

	// modify original code
	BYTE * ptr = (BYTE*)lp;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &lpvFuncAddr, sizeof(DWORD));
	ptr += 4;
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}

std_dll bool UninstallCreateInstanceEx()
{
/*
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CoCreateInstanceEx")) : 0;

	if (!(hModuleO && lp))
		return false;
*/
	if (__CreateInstanceExOriginalCode[0] == 0x90)
	{
		DWORD dw = 0;
		memcpy(&dw, &__CreateInstanceExOriginalCode[8], sizeof(DWORD));
		if (dw)
		{
			memcpy((BYTE*)dw, &__CreateInstanceExOriginalCode[1], 7);
			// set all NOP to saved code
			memset(__CreateInstanceExOriginalCode, 0, 16);
		}
	}	
	return true;
}
#endif // DCOM

// CoGetClassObject
std_dll bool InstallGetClassObject(LPVOID lpvFuncAddr)
{
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CoGetClassObject")) : 0;

	if (!(lpvFuncAddr && hModuleO && lp))
		return false;

	// if original code is not saved
	if (__GetClassObjectOriginalCode[0] != 0x90)
	{
		// set all NOP to saved code
		memset(__GetClassObjectOriginalCode, 0x90, 16);
		// and copy piece of original code to this place
		memcpy(&__GetClassObjectOriginalCode[1], (BYTE*)lp, 7);
	
		DWORD dw = (DWORD)lp;
		memcpy(&__GetClassObjectOriginalCode[8], &dw, sizeof(DWORD));

		memcpy(&__GetClassObjectOriginalCode[12], &lpvFuncAddr, sizeof(DWORD));
	}

	// change protection for .dll mapping page if it's needed
	LPVOID lpBase = 0;
	LONG lSize = 0;
	bool bNeedChangeProtect = false;
	GetPageInfo(lp, &lpBase, &lSize, &bNeedChangeProtect);
	if (lpBase && lSize && bNeedChangeProtect)
	{
		SetLastError(0);
		DWORD dwOldProtect = 0;
		VirtualProtect(lpBase, lSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (ERROR_SUCCESS != GetLastError())
			return false;
	}

	// modify original code
	BYTE * ptr = (BYTE*)lp;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &lpvFuncAddr, sizeof(DWORD));
	ptr += 4;
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}

std_dll bool UninstallGetClassObject()
{
/*
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CoGetClassObject")) : 0;

	if (!(hModuleO && lp))
		return false;
*/
	if (__GetClassObjectOriginalCode[0] == 0x90)
	{
		DWORD dw = 0;
		memcpy(&dw, &__GetClassObjectOriginalCode[8], sizeof(DWORD));
		if (dw)
		{
			memcpy((BYTE*)dw, &__GetClassObjectOriginalCode[1], 7);
			// set all NOP to saved code
			memset(__GetClassObjectOriginalCode, 0, 7);
		}
	}	
	
	return true;
}

// CLSIDFromProgID
std_dll bool InstallCLSIDFromProgID(LPVOID lpvFuncAddr)
{
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CLSIDFromProgID")) : 0;

	if (!(lpvFuncAddr && hModuleO && lp))
		return false;

	// if original code is not saved
	if (__CLSIDFromProgIDOriginalCode[0] != 0x90)
	{
		// set all NOP to saved code
		memset(__CLSIDFromProgIDOriginalCode, 0x90, 16);
		// and copy piece of original code to this place
		memcpy(&__CLSIDFromProgIDOriginalCode[1], (BYTE*)lp, 7);
	
		DWORD dw = (DWORD)lp;
		memcpy(&__CLSIDFromProgIDOriginalCode[8], &dw, sizeof(DWORD));

		memcpy(&__CLSIDFromProgIDOriginalCode[12], &lpvFuncAddr, sizeof(DWORD));
	}

	// change protection for .dll mapping page if it's needed
	LPVOID lpBase = 0;
	LONG lSize = 0;
	bool bNeedChangeProtect = false;
	GetPageInfo(lp, &lpBase, &lSize, &bNeedChangeProtect);
	if (lpBase && lSize && bNeedChangeProtect)
	{
		SetLastError(0);
		DWORD dwOldProtect = 0;
		VirtualProtect(lpBase, lSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (ERROR_SUCCESS != GetLastError())
			return false;
	}

	// modify original code
	BYTE * ptr = (BYTE*)lp;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &lpvFuncAddr, sizeof(DWORD));
	ptr += 4;
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}

std_dll bool UninstallCLSIDFromProgID()
{
/*
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("CLSIDFromProgID")) : 0;

	if (!(hModuleO && lp))
		return false;
*/
	if (__CLSIDFromProgIDOriginalCode[0] == 0x90)
	{
		DWORD dw = 0;
		memcpy(&dw, &__CLSIDFromProgIDOriginalCode[8], sizeof(DWORD));
		if (dw)
		{
			memcpy((BYTE*)dw, &__CLSIDFromProgIDOriginalCode[1], 7);
			// set all NOP to saved code
			memset(__CLSIDFromProgIDOriginalCode, 0, 16);
		}
	}	
	return true;
}

// ProgIDFromCLSID
std_dll bool InstallProgIDFromCLSID(LPVOID lpvFuncAddr)
{
	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("ProgIDFromCLSID")) : 0;

	if (!(lpvFuncAddr && hModuleO && lp))
		return false;

	// if original code is not saved
	if (__ProgIDFromCLSIDOriginalCode[0] != 0x90)
	{
		// set all NOP to saved code
		memset(__ProgIDFromCLSIDOriginalCode, 0x90, 16);
		// and copy piece of original code to this place
		memcpy(&__ProgIDFromCLSIDOriginalCode[1], (BYTE*)lp, 7);
	
		DWORD dw = (DWORD)lp;
		memcpy(&__ProgIDFromCLSIDOriginalCode[8], &dw, sizeof(DWORD));

		memcpy(&__ProgIDFromCLSIDOriginalCode[12], &lpvFuncAddr, sizeof(DWORD));
	}

	// change protection for .dll mapping page if it's needed
	LPVOID lpBase = 0;
	LONG lSize = 0;
	bool bNeedChangeProtect = false;
	GetPageInfo(lp, &lpBase, &lSize, &bNeedChangeProtect);
	if (lpBase && lSize && bNeedChangeProtect)
	{
		SetLastError(0);
		DWORD dwOldProtect = 0;
		VirtualProtect(lpBase, lSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);

		if (ERROR_SUCCESS != GetLastError())
			return false;
	}

	// modify original code
	BYTE * ptr = (BYTE*)lp;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &lpvFuncAddr, sizeof(DWORD));
	ptr += 4;
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}

std_dll bool UninstallProgIDFromCLSID()
{
//	HMODULE	hModuleO = ::GetModuleHandle("ole32.dll");
//	LPVOID lp = hModuleO ? (LPVOID)GetProcAddress(hModuleO, _T("ProgIDFromCLSID")) : 0;

//	if (!(hModuleO && lp))
//		return false;

	if (__ProgIDFromCLSIDOriginalCode[0] == 0x90)
	{
		DWORD dw = 0;
		memcpy(&dw, &__ProgIDFromCLSIDOriginalCode[8], sizeof(DWORD));
		if (dw)
		{
			memcpy((BYTE*)dw, &__ProgIDFromCLSIDOriginalCode[1], 7);
			// set all NOP to saved code
			memset(__ProgIDFromCLSIDOriginalCode, 0, 16);
		}
	}	
	return true;
}

EXPORT_STDAPI CoCreateInstanceNew(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;
	DWORD dw = 0;
	memcpy(&dw, &__CreateInstanceOriginalCode[8], sizeof(DWORD));
	if (dw)
	{
		// restore original handler
		memcpy((BYTE*)dw, &__CreateInstanceOriginalCode[1], 7);

		// simply call CoCreateInstance function
		typedef HRESULT (_stdcall *PFN)(REFCLSID, LPUNKNOWN, DWORD, REFIID, LPVOID FAR* );
		PFN pfn = (PFN)dw;
		hr = pfn(rclsid, pUnkOuter, dwClsContext, riid, ppv);

		// set new handler again
		BYTE * ptr = (BYTE*)dw;
		*ptr = 0xB8;ptr++; // mov eax, &data
		memcpy(ptr, &__CreateInstanceOriginalCode[12], sizeof(DWORD));
		ptr += 4;
		*ptr = 0xFF;ptr++; // jmp eax
		*ptr = 0xE0;ptr++; // 
	}

/*
	__asm 
	{
		mov		esi, esp

		mov		eax, DWORD PTR ppv
		push	eax
		
		mov		ecx, DWORD PTR riid
		push	ecx
		
		mov		edx, dwClsContext
		push	edx
		
		mov		eax, DWORD PTR pUnkOuter
		push	eax
		
		mov		ecx, DWORD PTR intGuid
		push	ecx

		call	dwCode
		
		cmp		esi, esp
		mov		DWORD PTR hr, eax
	}
*/
	return hr;
}

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
EXPORT_STDAPI CoCreateInstanceExNew(REFCLSID rclsid, LPUNKNOWN punkOuter, DWORD dwClsCtx, COSERVERINFO * pServerInfo, ULONG cmq, MULTI_QI * pResults)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;
	DWORD dw = 0;
	memcpy(&dw, &__CreateInstanceExOriginalCode[8], sizeof(DWORD));
	if (dw)
	{
		// restore original handler
		memcpy((BYTE*)dw, &__CreateInstanceExOriginalCode[1], 7);

		// simply call CoCreateInstanceEx function
		typedef HRESULT (_stdcall *PFN)(REFCLSID, LPUNKNOWN, DWORD, COSERVERINFO*, ULONG, MULTI_QI*);
		PFN pfn = (PFN)dw;
		hr = pfn(rclsid, punkOuter, dwClsCtx, pServerInfo, cmq, pResults);

		// set new handler again
		BYTE * ptr = (BYTE*)dw;
		*ptr = 0xB8;ptr++; // mov eax, &data
		memcpy(ptr, &__CreateInstanceExOriginalCode[12], sizeof(DWORD));
		ptr += 4;
		*ptr = 0xFF;ptr++; // jmp eax
		*ptr = 0xE0;ptr++; // 
	}
	
	return hr;
/*
	// simply call old CoCreateInstanceEx function
	DWORD dwCode = (DWORD)::GetDataCreateInstanceEx(); 
	__asm 
	{
		mov		esi, esp
	
		mov		eax, DWORD PTR pResults
		push	eax
		
		mov		ecx, cmq
		push	ecx
		
		mov		edx, DWORD PTR pServerInfo
		push	edx
		
		mov		eax, dwClsCtx
		push	eax
		
		mov		ecx, DWORD PTR punkOuter
		push	ecx
		
		mov		edx, DWORD PTR intGuid
		push	edx

		call	dwCode
		
		cmp		esi, esp
		mov		DWORD PTR hr, eax
	}
	
	return hr;
*/
}
#endif // DCOM

EXPORT_STDAPI CoGetClassObjectNew(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;
	DWORD dw = 0;
	memcpy(&dw, &__GetClassObjectOriginalCode[8], sizeof(DWORD));
	if (dw)
	{
		// restore original handler
		memcpy((BYTE*)dw, &__GetClassObjectOriginalCode[1], 7);

		// simply call CoGetClassObject function
		typedef HRESULT (_stdcall *PFN)(REFCLSID, DWORD, LPVOID, REFIID, LPVOID*) ;
		PFN pfn = (PFN)dw;
		hr = pfn(rclsid, dwClsContext, pvReserved, riid, ppv);

		// set new handler again
		BYTE * ptr = (BYTE*)dw;
		*ptr = 0xB8;ptr++; // mov eax, &data
		memcpy(ptr, &__GetClassObjectOriginalCode[12], sizeof(DWORD));
		ptr += 4;
		*ptr = 0xFF;ptr++; // jmp eax
		*ptr = 0xE0;ptr++; // 
	}
	
	return hr;
/*
	// simply call old CoGetClassObject function
	DWORD dwCode = (DWORD)::GetDataGetClassObject(); 
	__asm 
	{
		mov		esi, esp
		mov		eax, DWORD PTR ppv
		push	eax

		mov		ecx, DWORD PTR riid
		push	ecx
		
		mov		edx, pvReserved
		push	edx
		
		mov		eax, dwClsContext
		push	eax

		mov		ecx, DWORD PTR intGuid
		push	ecx

		call	dwCode

		cmp		esi, esp
		mov		DWORD PTR hr, eax
	}
	
	return hr;
*/
}

EXPORT_STDAPI ProgIDFromCLSIDNew(REFCLSID rclsid, LPOLESTR FAR* lplpszProgID)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;
	DWORD dw = 0;
	memcpy(&dw, &__ProgIDFromCLSIDOriginalCode[8], sizeof(DWORD));
	if (dw)
	{
		// restore original handler
		memcpy((BYTE*)dw, &__ProgIDFromCLSIDOriginalCode[1], 7);

		// simply call ProgIDFromCLSID function
		typedef HRESULT (_stdcall *PFN)(REFCLSID, LPOLESTR FAR*);
		PFN pfn = (PFN)dw;
		hr = pfn(rclsid, lplpszProgID);

		// set new handler again
		BYTE * ptr = (BYTE*)dw;
		*ptr = 0xB8;ptr++; // mov eax, &data
		memcpy(ptr, &__ProgIDFromCLSIDOriginalCode[12], sizeof(DWORD));
		ptr += 4;
		*ptr = 0xFF;ptr++; // jmp eax
		*ptr = 0xE0;ptr++; // 
	}
	
	return hr;
/*
	// simply call old ProgIDFromCLSID function
	DWORD dwCode = (DWORD)::GetDataProgIDFromCLSID(); 
	__asm 
	{
		mov		esi, esp

		mov		eax, DWORD PTR lplpszProgID
		push	eax

		mov		ecx, DWORD PTR rclsid
		push	ecx

		call	dwCode
	
		cmp		esi, esp
		mov		DWORD PTR hr, eax
	}
	
	return hr;
*/
}

EXPORT_STDAPI CLSIDFromProgIDNew(LPCOLESTR lpszProgID, LPCLSID lpclsid)
{
	HRESULT hr = REGDB_E_CLASSNOTREG;
	DWORD dw = 0;
	memcpy(&dw, &__CLSIDFromProgIDOriginalCode[8], sizeof(DWORD));
	if (dw)
	{
		// restore original handler
		memcpy((BYTE*)dw, &__CLSIDFromProgIDOriginalCode[1], 7);

		// simply call CLSIDFromProgID function
		typedef HRESULT (_stdcall *PFN)(LPCOLESTR, LPCLSID);
		PFN pfn = (PFN)dw;
		hr = pfn(lpszProgID, lpclsid);

		// set new handler again
		BYTE * ptr = (BYTE*)dw;
		*ptr = 0xB8;ptr++; // mov eax, &data
		memcpy(ptr, &__CLSIDFromProgIDOriginalCode[12], sizeof(DWORD));
		ptr += 4;
		*ptr = 0xFF;ptr++; // jmp eax
		*ptr = 0xE0;ptr++; // 
	}
	
	return hr;
/*
	// simply call old CLSIDFromProgID function
	DWORD dwCode = (DWORD)::GetDataCLSIDFromProgID(); 
	__asm 
	{
		mov		esi, esp

		mov		eax, DWORD PTR lpclsid
		push	eax

		mov		ecx, DWORD PTR lpszProgID
		push	ecx

		call	dwCode
	
		cmp		esi, esp
		mov		DWORD PTR hr, eax
	}
	return hr;
*/
}

/*
// трансляция внутренного ProgID во внешний ProgID
std_dll bool TranslateProgID(BSTR bstrInnerProgID, BSTR * pbstrExtenProgID, BSTR bstrSuff)
{
	CString strI(bstrInnerProgID);
	CString strS(bstrSuff);

	if (strI.IsEmpty() || !pbstrExtenProgID)
		return false;

	*pbstrExtenProgID = 0;

	IVTApplicationPtr sptrApp = GetAppUnknown(false);
	if (sptrApp == 0)
		return false;

	BSTR bSuff = 0;
	BYTE * pbTable = 0;
	if (FAILED(sptrApp->GetData(0, pbTable, &bSuff, 0)))
		return false;

	if (strS.IsEmpty())
		strS = bSuff;
	
	::CoTaskMemFree(bSuff);

	// now we check given progid is contains suffix
	int nDiff = strI.GetLength() - strS.GetLength();
	if (nDiff > 0)
	{
		// if so we need to remove suffix from progid
		int nPos = strI.Find(strS, nDiff);
		if (nPos != -1)
			strI.Delete(nPos, strI.GetLength() - nPos);
	}

	CTranslateTable * pTable = (CTranslateTable*)pbTable;
	if (!pTable)
		return false;

	// if clsid of given progID is valid && equal with extern -> do nothing
	// else -> add suffix to progid
	GuidKey guid = pTable->Get(strI);
	if (guid != INVALID_KEY && pTable->GetExtern(strI) != guid)
		strI += strS;

	*pbstrExtenProgID = strI.AllocSysString();

	return true;
}
*/

std_dll bool LoadVTClass(CWinApp * pApp)
{
	if (!pApp)
		return false;

//	AFX_MANAGE_STATE(AfxGetStaticModuleState());

	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();

	for (COleObjectFactory* pFactory = pModuleState->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
	{
		if (pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)))
		{
			CVTFactory* pF = (CVTFactory*)pFactory;
			pF->LoadInfo();
		}
	}

	// register extension DLL factories
	for (CDynLinkLibrary* pDLL = pModuleState->m_libraryList; pDLL != NULL; pDLL = pDLL->m_pNextDLL)
	{
		for (pFactory = pDLL->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
		{
			if (pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)))
			{
				CVTFactory* pF = (CVTFactory*)pFactory;
				pF->LoadInfo();
			}
		}
	}

	return true;
}