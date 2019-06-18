#include "StdAfx.h"
#include "nvCommon.h"
#include "GuardFunc.h"
#include "GuardInt.h"
//#include "TransTable.h"
#include "Utils.h"


const WORD wSerNum = 5; // serial number of VideoTesT 5 programm
char const sPassword[nsars_Fast]   = "_THISis_string_PWD!@qq$52-bytes";

export_data DWORD dwNVDummy = 0xffffffff;	// fake number
export_data DWORD dwRDid = 0;				// indentifier of read
export_data DWORD dwWRid = 0;				// indentifier of write
export_data DWORD dwPublicID = 0;				// indentifier of write


// handled of kernel objects
export_data HANDLE	g_hMemFile = 0;

// return array of key's prog_numbers w/ gven serial_number
std_dll bool EnumStealthKeys(KeyInfo * pKeyInfo, int nNumKeys, WORD wSerialNum)
{
	bool bRet = true;
	if (!pKeyInfo || !nNumKeys)
		return false;

//	nvInitialize(0, 0, wSerialNum);

	DWORD dwID = 0;
	int nRet = nskFindFirst(nvGetPid(), &dwID);

	for (int i = 0; i < nNumKeys && nRet == nse_Ok; i++)
	{
		if (nse_Ok != nskRead(nvGetRDid(), sam_dwID, sizeof(pKeyInfo[0].dwID), &(pKeyInfo[0].dwID)))
			bRet = false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_bNProg, sizeof(pKeyInfo[0].bPN), &(pKeyInfo[0].bPN)))
			bRet = false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_bVer, sizeof(pKeyInfo[0].bVer), &(pKeyInfo[0].bVer)))
			bRet = false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_wMask, sizeof(pKeyInfo[0].wMask), &(pKeyInfo[0].wMask)))
			bRet = false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_wSN, sizeof(pKeyInfo[0].wSN), &(pKeyInfo[0].wSN)))
			bRet = false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_wType, sizeof(pKeyInfo[0].wType), &(pKeyInfo[0].wType)))
			bRet = false;

		// Find next specifed key
		nRet = nskFindNext(&dwID);
	}

	nvDeinitialize();
	
	return true;
}


// return number of keys w/ given serial_number
std_dll int	GetStealthKeysNumber()//WORD wSerialNum)
{
//	nvInitialize(0, 0, wSerialNum);
	int nCount = 0;
	// Find first specifed key
	DWORD dwID = 0;
	int nRet = nskFindFirst(nvGetPid(), &dwID);
	while (nRet == nse_Ok)
	{
		nCount++;
		// Find next specifed key
		nRet = nskFindNext(&dwID);
	}

//	nvDeinitialize();
	return nCount;
}

std_dll bool GetFirstStealthKey(KeyInfo * pInfo)
{
	if (!pInfo)
		return false;

	bool bRet = true;

	if (nse_Ok != nskFindFirst(nvGetPid(), &(pInfo->dwID)))
		return false;
	if (nse_Ok != nskRead(nvGetRDid(), sam_bNProg, sizeof(pInfo[0].bPN), &(pInfo[0].bPN)))
		return false;
	if (nse_Ok != nskRead(nvGetRDid(), sam_bVer, sizeof(pInfo[0].bVer), &(pInfo[0].bVer)))
		return false;
	if (nse_Ok != nskRead(nvGetRDid(), sam_wMask, sizeof(pInfo[0].wMask), &(pInfo[0].wMask)))
		return false;
	if (nse_Ok != nskRead(nvGetRDid(), sam_wSN, sizeof(pInfo[0].wSN), &(pInfo[0].wSN)))
		return false;
	if (nse_Ok != nskRead(nvGetRDid(), sam_wType, sizeof(pInfo[0].wType), &(pInfo[0].wType)))
		return false;

	return bRet;
}

std_dll bool GetNextStealthKey(KeyInfo * pInfo)
{
	if (!pInfo)
		return false;
	bool bRet = true;

	if (nse_Ok == nskFindNext(&(pInfo->dwID)))
	{
		if (nse_Ok != nskRead(nvGetRDid(), sam_bNProg, sizeof(pInfo[0].bPN), &(pInfo[0].bPN)))
			return false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_bVer, sizeof(pInfo[0].bVer), &(pInfo[0].bVer)))
			return false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_wMask, sizeof(pInfo[0].wMask), &(pInfo[0].wMask)))
			return false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_wSN, sizeof(pInfo[0].wSN), &(pInfo[0].wSN)))
			return false;
		if (nse_Ok != nskRead(nvGetRDid(), sam_wType, sizeof(pInfo[0].wType), &(pInfo[0].wType)))
			return false;
	}
	else
		bRet = false;

	return bRet;
}

// find appropriate stealth key w /given ser_num & prog
// and return stealth key ID
std_dll DWORD FindStealthKeyID(WORD wSerNum, BYTE bProg)
{
	nvInitialize(0, 0, wSerNum, bProg);

	DWORD dwID = 0;
	int nRet = nskFindFirst(nvGetPid(), &dwID);
	ASSERT (nRet == nse_Ok);

	nvDeinitialize();
	return dwID;
}




// initialize stealth API to given keyID
// load mem file
// Create all kernel objects
std_dll bool StealthInit()//DWORD dwID)
{
	SetLastError(0);

	if (!g_hMutex && !(g_hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME)))
	{
		g_hMutex = CreateMutex(NULL, true, MUTEXNAME);
		if (ERROR_ALREADY_EXISTS == GetLastError())
		{
			ReleaseMutex(g_hMutex);
//			return true;
		}
		if (!g_hMutex)
			return false;
	}

	if (!g_hEventEnd && !(g_hEventEnd = OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME)))
	{
		g_hEventEnd = CreateEvent(NULL, true, false, ENDEVENTNAME);
		if (!g_hEventEnd)
		{
			ReleaseMutex(g_hMutex);
			StealthDeinit();
			return false;
		}
	}

	if (!g_hEventError && !(g_hEventError = OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME)))
	{
		g_hEventError = CreateEvent(NULL, true, false, ERROREVENTNAME);
		if (!g_hEventError)
		{
			ReleaseMutex(g_hMutex);
			StealthDeinit();
			return false;
		}
	}

	// create mem file
	if (!g_hMemFile && !(g_hMemFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, MAPPINGNAME)))
	{
		g_hMemFile = ::CreateFileMapping((HANDLE)-1, 0, PAGE_READWRITE, 0, 2048, MAPPINGNAME);
		if (!g_hMemFile)
		{
			GuardSetErrorCode(guardCreateFileMapping); // error create file mapping
			SetEvent(g_hEventError);
			ReleaseMutex(g_hMutex);
	//		StealthDeinit();
			return false;
		}
	}

	BYTE * pdata = (BYTE*)::MapViewOfFile(g_hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 2048);
	if (!pdata)
	{
		GuardSetErrorCode(guardMapFile); // error file mapping
		SetEvent(g_hEventError);
		ReleaseMutex(g_hMutex);
//		StealthDeinit();
		return false;
	}


	// init file
	// 03.05.2006 build 104 - переписал кошмарный код случайного упорядочения массива
	for (int i = 0; i < 256; i++) pdata[i] = i;
	for (i = 0; i < 256; i++)
	{
		int j = rand()%256;
		BYTE tmp = pdata[i];
		pdata[i] = pdata[j];
		pdata[j] = tmp;
	}
	for (i = 256; i < 1024; i++)
		pdata[i] = rand()%256;

/*
	// init stealth API
	::nvInitializeEx(0, dwID);

	if (nskCheck(::nvGetRDid()) != nse_Ok)
	{
		GuardSetErrorCode(guardNotExistsNSK);
		SetEvent(g_hEventError);
	}

	{
		// read key data
		BYTE data[122];
		GUID guidkey;
///////////////////////////////////////////////
		if (::nskCheck(nvGetRDid()) == nse_Ok)
		{
			// read indexes
			if (::nskRead(nvGetRDid(), 192, 24, &data[0]) != nse_Ok)
			{
				GuardSetErrorCode(guardReadNSK);
				SetEvent(g_hEventError);
			}

			if (::nskRead(nvGetRDid(), 94, 98, &data[24]) != nse_Ok)
			{
				GuardSetErrorCode(guardReadNSK);
				SetEvent(g_hEventError);
			}

			// read GUID
			if (::nskRead(nvGetRDid(), 78, 16, &guidkey) != nse_Ok)
			{
				GuardSetErrorCode(guardReadNSK);
				SetEvent(g_hEventError);
			}
		}
		else
			::memset(data, 255, sizeof(data));
		
		// save data to file
		for (i = 0; i < 122; i++)
			pdata[i+258+pdata[256]] = pdata[data[i]];

		// save guid to file
		BYTE * pGuidPtr = (BYTE*)&guidkey;
		for (i = 0; i < 16; i++)
			pdata[i+128+258+pdata[256]] = pdata[pGuidPtr[i]];
	}

	::nvDeinitialize();
*/
	::UnmapViewOfFile(pdata);

	ReleaseMutex(g_hMutex);

	return true;
}

// initialize stealth API to given keyID
// load mem file
std_dll bool StealthUpdate(DWORD dwID)
{
	HANDLE hMutex = 0;
	HANDLE hEventError = 0;
	HANDLE hEventEnd = 0;
	HANDLE hMemFile = 0;
	BYTE * pdata = 0;
	__try
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
		if (!hMutex)
			__leave;

		WaitForSingleObject(hMutex, INFINITE);

		hEventError = ::OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME);

		hEventEnd = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);

		// Open mem file
		hMemFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, MAPPINGNAME);
		if (!hMemFile)
		{
			GuardSetErrorCode(guardOpenFileMapping); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		pdata = (BYTE*)::MapViewOfFile(hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 2048);
		if (!pdata)
		{
			GuardSetErrorCode(guardMapFile); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		// init file
		// 03.05.2006 build 104 - переписал кошмарный код случайного упорядочения массива
		for (int i = 0; i < 256; i++) pdata[i] = i;
		for (i = 0; i < 256; i++)
		{
			int j = rand()%256;
			BYTE tmp = pdata[i];
			pdata[i] = pdata[j];
			pdata[j] = tmp;
		}
		for (i = 256; i < 1024; i++)
			pdata[i] = rand()%256;

		// init stealth API
		::nvInitializeEx(0, dwID);

		if (nskCheck(::nvGetRDid()) != nse_Ok)
		{
			GuardSetErrorCode(guardNotExistsNSK);
			SetEvent(hEventError);
		}
/*
	guardCreateFileMapping = 0,
	guardOpenFileMapping,
	guardMapFile,

	guardNotExistsNSK,
	guardInvalidNSK,
	guardReadNSK,
	guardWriteNSK

*/
		{
			// read key data
			BYTE data[122];
			GUID guidkey;

			IVTApplication * ptrApp = 0;
			HMODULE hMod = GetModuleHandle(0);
			if (hMod)
			{
				typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
				PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
				IUnknown * punk = pGetAppUnknown ? pGetAppUnknown(false) : 0;
				if (punk && SUCCEEDED(punk->QueryInterface(IID_IVTApplication, (void**)&ptrApp)))
				{
					ptrApp->GetData((DWORD*)&guidkey, 0, 0, 0);
					ptrApp->Release();
				}
			}

			if (::nskCheck(nvGetRDid()) == nse_Ok)
			{
				// read indexes
				if (::nskRead(nvGetRDid(), 192, 24, &data[0]) != nse_Ok)
				{
					GuardSetErrorCode(guardReadNSK);
					SetEvent(hEventError);
				}

				if (::nskRead(nvGetRDid(), 94, 98, &data[24]) != nse_Ok)
				{
					GuardSetErrorCode(guardReadNSK);
					SetEvent(hEventError);
				}

				// read GUID
				if (::nskRead(nvGetRDid(), 78, 16, &guidkey) != nse_Ok)
				{
					GuardSetErrorCode(guardReadNSK);
					SetEvent(hEventError);
				}
			}
			else
				::memset(data, 122, sizeof(data));
			
			// save data to file
			for (i = 0; i < 122; i++)
				pdata[i+258+pdata[256]] = pdata[data[i]];

			// save guid to file
			BYTE * pGuidPtr = (BYTE*)&guidkey;
			for (i = 0; i < 16; i++)
				pdata[i+128+258+pdata[256]] = pdata[pGuidPtr[i]];

		}
		::nvDeinitialize();
	}
	__finally
	{
		if (pdata)
			::UnmapViewOfFile(pdata);

		if (hMemFile)
			CloseHandle(hMemFile);

		if (hEventError)
			CloseHandle(hEventError);

		if (hEventEnd)
			CloseHandle(hEventEnd);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}

	return true;
}


// deinitialize stealth API
// release all kernel objects
std_dll bool StealthDeinit()
{
	if (!g_hMutex)
		return false;

	WaitForSingleObject(g_hMutex, INFINITE);
	::nvDeinitialize();

	if (g_hMemFile)
	{
		CloseHandle(g_hMemFile);
		g_hMemFile = 0;
	}

	if (g_hEventError)
	{
		CloseHandle(g_hEventError);
		g_hEventError = 0;
	}

	if (g_hEventEnd)
	{
		CloseHandle(g_hEventEnd);
		g_hEventEnd = 0;
	}

	ReleaseMutex(g_hMutex);
	CloseHandle(g_hMutex);
	g_hMutex = 0;

	return true;
}
// stop 
std_dll void StealthEnd()
{
	HANDLE hMutex = 0;
	HANDLE hEventEnd = 0;
	__try
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
		if (!hMutex)
			__leave;

		WaitForSingleObject(hMutex, INFINITE);

		hEventEnd = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);
		::SetEvent(hEventEnd);

	}	
	__finally
	{
		if (hEventEnd)
			CloseHandle(hEventEnd);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}
}

std_dll bool StealthIndexIsAvailable(int idx)
{
	HANDLE hMutex = 0;
	HANDLE hEventError = 0;
	HANDLE hEventEnd = 0;
	HANDLE hMemFile = 0;
	BYTE * pdata = 0;
	bool bAccessible = true;
	
	__try
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
		if (!hMutex)
			__leave;

		WaitForSingleObject(hMutex, INFINITE);

		hEventError = ::OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME);

		hEventEnd = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);

		// Open mem file
		hMemFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, MAPPINGNAME);
		if (!hMemFile)
		{
			GuardSetErrorCode(guardOpenFileMapping); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		pdata = (BYTE*)::MapViewOfFile(hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 2048);
		if (!pdata)
		{
			GuardSetErrorCode(guardMapFile); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		int	inumByte = idx / 8;
		int	inumBit = idx % 8;

		BYTE byteVal = pdata[inumByte+258+pdata[256]];

		int i;
		for (i = 0; i < 256; i++)
			if (byteVal == pdata[i])
				break;

		if (i != 256)
			bAccessible = (i & (128 >> inumBit)) != 0;

	}	
	__finally
	{
		if (pdata)
			::UnmapViewOfFile(pdata);

		if (hMemFile)
			CloseHandle(hMemFile);

		if (hEventError)
			CloseHandle(hEventError);

		if (hEventEnd)
			CloseHandle(hEventEnd);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}
	return bAccessible;
}

std_dll void StealthIndexMakeAvailable(int idx, bool bMake)
{
	HANDLE hMutex = 0;
	HANDLE hEventError = 0;
	HANDLE hEventEnd = 0;
	HANDLE hMemFile = 0;
	BYTE * pdata = 0;
	
	__try
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
		if (!hMutex)
			__leave;

		WaitForSingleObject(hMutex, INFINITE);

		hEventError = ::OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME);

		hEventEnd = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);

		// Open mem file
		hMemFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, MAPPINGNAME);
		if (!hMemFile)
		{
			GuardSetErrorCode(guardOpenFileMapping); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		pdata = (BYTE*)::MapViewOfFile(hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 2048);
		if (!pdata)
		{
			GuardSetErrorCode(guardMapFile); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		int	inumByte = idx / 8;
		int	inumBit = idx % 8;

		BYTE byteVal = pdata[inumByte+258+pdata[256]];
		int i;
		for (i = 0; i < 256; i++)
			if (byteVal == pdata[i])
				break;

		byteVal = i;

		if (bMake)
			byteVal |= (128 >> inumBit);
		else
			byteVal &= ~(128 >> inumBit);

		byteVal = pdata[byteVal];
		pdata[inumByte+258+pdata[256]] = byteVal;

	}
	__finally
	{
		if (pdata)
			::UnmapViewOfFile(pdata);

		if (hMemFile)
			CloseHandle(hMemFile);

		if (hEventError)
			CloseHandle(hEventError);

		if (hEventEnd)
			CloseHandle(hEventEnd);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}
}

std_dll void StealthWriteIndexes(DWORD dwID)
{
	HANDLE hMutex = 0;
	HANDLE hEventError = 0;
	HANDLE hEventEnd = 0;
	HANDLE hMemFile = 0;
	BYTE * pdata = 0;
	bool bAccessible = true;
	
	__try
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
		if (!hMutex)
			__leave;

		WaitForSingleObject(hMutex, INFINITE);

		hEventError = ::OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME);

		hEventEnd = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);

		// Open mem file
		hMemFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, MAPPINGNAME);
		if (!hMemFile)
		{
			GuardSetErrorCode(guardOpenFileMapping); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		pdata = (BYTE*)::MapViewOfFile(hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 2048);
		if (!pdata)
		{
			GuardSetErrorCode(guardMapFile); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}
		byte	data[122];

		for (int inumByte = 0; inumByte < 122; inumByte++)
		{
			byte byteVal = pdata[inumByte+258+pdata[256]];
			int i;
			for (i = 0; i < 256; i++)
				if (byteVal == pdata[i])
					break;

			data[inumByte] = i;
		}

		GUID guid;
		BYTE * pGuidPtr = (BYTE*)&guid;
		// Read Guid from file
		for (int inumByte = 0; inumByte < 16; inumByte++)
		{
			byte byteVal = pdata[inumByte+128+258+pdata[256]];
			for (int i = 0; i < 256; i++)
				if (byteVal == pdata[i])
					break;

			pGuidPtr[inumByte] = i;
		}

		// init stealth API
		::nvInitializeEx(0, dwID);
		
		if (nskCheck(::nvGetRDid()) != nse_Ok)
		{
			GuardSetErrorCode(guardNotExistsNSK);
			SetEvent(hEventError);
		}

		{
			// write indexes
			if (::nskWrite(nvGetWRid(), 192, 24, &data[0]) != nse_Ok)
			{
				GuardSetErrorCode(guardWriteNSK);
				SetEvent(hEventError);
			}
			if (::nskWrite(nvGetWRid(), 94, 98, &data[24]) != nse_Ok)
			{
				GuardSetErrorCode(guardWriteNSK);
				SetEvent(hEventError);
			}

			// Write GUID
			if (::nskWrite(nvGetWRid(), 78, 16, pGuidPtr) != nse_Ok)
			{
				GuardSetErrorCode(guardWriteNSK);
				SetEvent(hEventError);
			}
		}
		::nvDeinitialize();
	}
	__finally
	{
		if (pdata)
			::UnmapViewOfFile(pdata);

		if (hMemFile)
			CloseHandle(hMemFile);

		if (hEventError)
			CloseHandle(hEventError);

		if (hEventEnd)
			CloseHandle(hEventEnd);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}
}

std_dll void StealthWriteGUID(GUID * pGuid)
{
	if (!pGuid)
		return;

	HANDLE hMutex = 0;
	HANDLE hEventError = 0;
	HANDLE hEventEnd = 0;
	HANDLE hMemFile = 0;
	BYTE * pdata = 0;
	
	__try
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
		if (!hMutex)
			__leave;

		WaitForSingleObject(hMutex, INFINITE);

		hEventError = ::OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME);

		hEventEnd = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);

		// Open mem file
		hMemFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, MAPPINGNAME);
		if (!hMemFile)
		{
			GuardSetErrorCode(guardOpenFileMapping); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		pdata = (BYTE*)::MapViewOfFile(hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 2048);
		if (!pdata)
		{
			GuardSetErrorCode(guardMapFile); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		// save guid to file
		BYTE * pGuidPtr = (BYTE*)pGuid;
		for (int i = 0; i < 16; i++)
			pdata[i+128+258+pdata[256]] = pdata[pGuidPtr[i]];

	}
	__finally
	{
		if (pdata)
			::UnmapViewOfFile(pdata);

		if (hMemFile)
			CloseHandle(hMemFile);

		if (hEventError)
			CloseHandle(hEventError);

		if (hEventEnd)
			CloseHandle(hEventEnd);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}
}

std_dll void StealthReadGUID(GUID * pGuid)
{
	if (!pGuid)
		return;

	HANDLE hMutex = 0;
	HANDLE hEventError = 0;
	HANDLE hEventEnd = 0;
	HANDLE hMemFile = 0;
	BYTE * pdata = 0;
	
	__try
	{
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, true, MUTEXNAME);
		if (!hMutex)
			__leave;

		WaitForSingleObject(hMutex, INFINITE);

		hEventError = ::OpenEvent(EVENT_ALL_ACCESS, true, ERROREVENTNAME);

		hEventEnd = ::OpenEvent(EVENT_ALL_ACCESS, true, ENDEVENTNAME);

		// Open mem file
		hMemFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, MAPPINGNAME);
		if (!hMemFile)
		{
			GuardSetErrorCode(guardOpenFileMapping); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		pdata = (BYTE*)::MapViewOfFile(hMemFile, FILE_MAP_ALL_ACCESS, 0, 0, 2048);
		if (!pdata)
		{
			GuardSetErrorCode(guardMapFile); // error Open file mapping
			SetEvent(hEventError);
			__leave;
		}

		// save guid to file
		BYTE * pGuidPtr = (BYTE*)pGuid;
		// Read Guid from file
		for (int inumByte = 0; inumByte < 16; inumByte++)
		{
			byte byteVal = pdata[inumByte+128+258+pdata[256]];
			for (int i = 0; i < 256; i++)
				if (byteVal == pdata[i])
					break;

			pGuidPtr[inumByte] = i;
		}
	}
	__finally
	{
		if (pdata)
			::UnmapViewOfFile(pdata);

		if (hMemFile)
			CloseHandle(hMemFile);

		if (hEventError)
			CloseHandle(hEventError);

		if (hEventEnd)
			CloseHandle(hEventEnd);

		if (hMutex)
		{
			ReleaseMutex(hMutex);
			CloseHandle(hMutex);
		}
	}
}


std_dll DWORD __stdcall ThreadFuncStealthUpdate(LPVOID lpParam)
{
	if (!lpParam)
		return -1;

	bool bRet = StealthUpdate((DWORD)lpParam);

	Sleep(200);

	return bRet ? 0 : -1;
}

std_dll bool StealthCheckProtectionCLSID(REFCLSID rclsid)
{
	IVTApplicationPtr sptrApp;
	HMODULE hMod = GetModuleHandle(0);
	if (hMod)
	{
		typedef  IUnknown* (*PGET_APP_UNKNOWN)(bool bAddRef /*= false*/);
		PGET_APP_UNKNOWN pGetAppUnknown = (PGET_APP_UNKNOWN)GetProcAddress(hMod, "GuardGetAppUnknown"); 
		pGetAppUnknown ? pGetAppUnknown(false) : 0;
		if (pGetAppUnknown)
			sptrApp = pGetAppUnknown(false);
	}
	if (sptrApp == 0)
		sptrApp = GetAppUnknown();

	if (sptrApp == 0)
		return false;
/*
	// return ptr on KeyGuid, ptr on TranslationTable, szSuffix
	GuidKey Key;
	BYTE * pbTable = 0;
	if (FAILED(sptrApp->GetData((DWORD*)&Key, pbTable, 0)) || !pbTable)
		return false;

	CTranslateTable * pTable = (CTranslateTable*)pbTable;

	return StealthIndexIsAvailable((int)pTable->GetData(GuidKey(rclsid)));
*/
	return true;
}



/*

До 107 адреса включительно все остается, как и было.
С 108 по 123 включительно записывается пароль - 16 байт (GUID).
C 222 по 245 включительно - 22 байт и с 124 по 221 включительно - 98 байт  - 
*/

