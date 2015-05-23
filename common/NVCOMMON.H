#ifndef __NVCOMMON_H__
#define __NVCOMMON_H__

#include "Defs.h"
#include "nvskey32.h"
//#include "GuardFunc.H"


/*
ключ:
для идентификации нашего ключа используем RDid и SerialNumber программы
для распознавания, какая программа установлена, используется ProgramNumber.

DWORD dwNVDummy - это переменная для конвертации идентификаторов в неправильный вид.
устанавлиывается при запуске программы в зависимости от времени

*/
#ifndef MAKEDWORD
#	define MAKEDWORD(l, h)	((DWORD)(((WORD)(l)) | ((DWORD)((WORD)(h))) << 16))
#endif // MAKEDWORD

#if !defined(LOWORD) || !defined(HIWORD)
#	define LOWORD(l)		((WORD)(l))
#	define HIWORD(h)		((WORD)(((DWORD)(h) >> 16) & 0xFFFF))
#endif // LOWORD & HIWORD

#define RDValue		(DWORD)1064537495	// идентификатор чтения
#define WRValue		(DWORD)-801426373	// идентификатор записи
#define PublicValue	(DWORD)0xb9ba3d83	// общий идентификатор

#define MAPPINGNAME		_T("MyVTFileSharing")

extern const WORD wSerNum; // serial number of VideoTesT 5 programm
extern char const sPassword[];

extern export_data DWORD dwNVDummy;	// fake number
extern export_data DWORD dwRDid;				// indentifier of read
extern export_data DWORD dwWRid;				// indentifier of write
extern export_data DWORD dwPublicID;				// indentifier of write

//extern const char	g_szMappingName[];
extern export_data HANDLE		g_hMemFile;


// helper functions
inline DWORD nvGetRDid()
{
	DWORD dwRes = MAKEDWORD(HIWORD(dwRDid), LOWORD(dwRDid));
	dwRes += dwNVDummy;
	return dwRes^dwNVDummy;
}

inline DWORD nvGetWRid()
{
	DWORD dwRes = MAKEDWORD(HIWORD(dwWRid), LOWORD(dwWRid));
	dwRes += dwNVDummy;
	return dwRes^dwNVDummy;
}

inline DWORD nvGetPid()
{
	DWORD dwRes = MAKEDWORD(HIWORD(dwPublicID), LOWORD(dwPublicID));
	dwRes += dwNVDummy;
	return dwRes^dwNVDummy;
}

inline DWORD nvConvert(DWORD dwRes)
{
	dwRes = MAKEDWORD(HIWORD(dwRes), LOWORD(dwRes));
	dwRes += dwNVDummy;
	return dwRes^dwNVDummy;
}

// Initialize functions
inline void nvInitialize(BYTE bProg = 0, DWORD dwID = 0, WORD wSerNum = 0, BYTE bVer = 0, WORD wMask = 0)
{
	_tzset();

	time_t tSave = 0;
	time_t tret = time(&tSave);
	// init fake number
	dwNVDummy = (DWORD)tSave;
	
	// convert and save read identifier
	dwRDid = ((RDValue)^dwNVDummy) - dwNVDummy;
	dwRDid = MAKEDWORD(HIWORD(dwRDid), LOWORD(dwRDid));

	// convert and save write identifier
	dwWRid = ((WRValue)^dwNVDummy) - dwNVDummy;
	dwWRid = MAKEDWORD(HIWORD(dwWRid), LOWORD(dwWRid));

	// convert and save public identifier
	dwPublicID = ((PublicValue)^dwNVDummy) - dwNVDummy;
	dwPublicID = MAKEDWORD(HIWORD(dwPublicID), LOWORD(dwPublicID));

	DWORD dwMode = nsf_SysAddrMode + nsf_NoFullAccess + 
					(bProg ? nsf_NProg : 0) + (dwID ? nsf_ID : 0) + 
					(wSerNum ? nsf_SN : 0)  + (bVer ? nsf_Ver : 0) + (wMask ? nsf_Mask : 0);
	// tune stealth API to needed mode
	nskSetMode(dwMode, bProg, dwID, wSerNum, (BYTE)nsf_Ver, wMask, (WORD)0);
}

inline void nvInitializeEx(BYTE bProg = 0, DWORD dwID = 0, WORD wSerNum = 0, BYTE bVer = 0, WORD wMask = 0)
{
	_tzset();

	time_t tSave = 0;
	time_t tret = time(&tSave);
	// init fake number
	dwNVDummy = (DWORD)tSave;
	
	// convert and save read identifier
	dwRDid = ((RDValue)^dwNVDummy) - dwNVDummy;
	dwRDid = MAKEDWORD(HIWORD(dwRDid), LOWORD(dwRDid));

	// convert and save write identifier
	dwWRid = ((WRValue)^dwNVDummy) - dwNVDummy;
	dwWRid = MAKEDWORD(HIWORD(dwWRid), LOWORD(dwWRid));

	// convert and save public identifier
	dwPublicID = ((PublicValue)^dwNVDummy) - dwNVDummy;
	dwPublicID = MAKEDWORD(HIWORD(dwPublicID), LOWORD(dwPublicID));

	DWORD dwMode = nsf_NoFullAccess + (bProg ? nsf_NProg : 0) + (dwID ? nsf_ID : 0) + 
					(wSerNum ? nsf_SN : 0)  + (bVer ? nsf_Ver : 0) + (wMask ? nsf_Mask : 0);
	// tune stealth API to needed mode
	nskSetMode(dwMode, bProg, dwID, wSerNum, nsf_Ver, wMask, 0);
}
inline void nvDeinitialize()
{
	time_t tSave = 0;
	time_t tret = time(&tSave);
	// init fake number
	dwNVDummy = (DWORD)tSave;
	
	// convert and save read identifier
	dwRDid = ((RDValue)^dwNVDummy) - dwNVDummy;
	dwRDid = MAKEDWORD(HIWORD(dwRDid), LOWORD(dwRDid));

	// convert and save write identifier
	dwWRid = ((WRValue)^dwNVDummy) - dwNVDummy;
	dwWRid = MAKEDWORD(HIWORD(dwWRid), LOWORD(dwWRid));

	// convert and save public identifier
	dwPublicID = ((PublicValue)^dwNVDummy) - dwNVDummy;
	dwPublicID = MAKEDWORD(HIWORD(dwPublicID), LOWORD(dwPublicID));

	nskSetMode(0, 0, 0, 0, 0, 0, 0);
}

inline BYTE nvGetProgVer()
{
	BYTE iRes = 0;
	VERIFY(nskRead(nvGetRDid(), 31, 1, &iRes) == nse_Ok);
	return iRes;
}

inline WORD nvGetProgMask()
{
	WORD iRes = 0;
	VERIFY(nskRead(nvGetRDid(), 34, 2, &iRes) == nse_Ok);
	return iRes;
}

inline DWORD nvGetKeyID()
{
	DWORD iRes = 0;
	VERIFY(nskRead(nvGetRDid(), 22, 4, &iRes) == nse_Ok);
	return iRes;
}

inline BOOL StealthTestMask(WORD wMask, BOOL bRandom = FALSE)
{
	if (bRandom)
		return (clock()%6 == 0) ? (wMask & nvGetProgMask()) == wMask : true;

	return (wMask & nvGetProgMask()) == wMask;
}

inline BOOL StealthTestVer(BYTE bVer, BOOL bRandom = FALSE)
{
	if (bRandom)
		return (clock()%6 == 0) ? nvGetProgVer() == bVer : true;

	return nvGetProgVer() == bVer;
}




struct std_dll KeyInfo
{
	DWORD	dwID;
	BYTE	bPN;
	WORD	wSN;
	BYTE	bVer;
	WORD	wMask;
	WORD	wType;

	KeyInfo() : dwID(0), bPN(0), wSN(0), bVer(0), wMask(0), wType(0) {}
};

// return array of key's prog_numbers w/ gven serial_number
std_dll bool EnumStealthKeys(KeyInfo * pKeyInfo, int nNumKeys, WORD wSerialNum = 0);
// return number of keys w/ given serial_number
std_dll int	 GetStealthKeysNumber();//WORD wSerialNum = 0);
std_dll bool GetFirstStealthKey(KeyInfo * pInfo);
std_dll bool GetNextStealthKey(KeyInfo * pInfo);
std_dll DWORD FindStealthKeyID(WORD wSerNum, BYTE bProg);

// initialize stealth API to given keyID
// load mem file, create sync kernel objects
std_dll bool StealthInit();//DWORD dwID = 0);
// deinitialize stealth API
// destroy sync kernel objects
std_dll bool StealthDeinit();
// stop 
std_dll void StealthEnd();

// initialize stealth API to given keyID
// update mem file
std_dll bool StealthUpdate(DWORD dwID);

// write indexes to key
std_dll void StealthWriteIndexes(DWORD dwID);
// check availability of given index
std_dll bool StealthIndexIsAvailable(int idx);
// make availability of given index
std_dll void StealthIndexMakeAvailable(int idx, bool bMake);

// write GUID to NSK
std_dll void StealthWriteGUID(GUID * pGuid);
// read GUID from NSK
std_dll void StealthReadGUID(GUID * pGuid);
// checl protection for GUID  by index in NSK
std_dll bool StealthCheckProtectionCLSID(REFCLSID rclsid);

// word thread function
std_dll DWORD __stdcall ThreadFuncStealthUpdate(LPVOID lpParam = 0);

#if 0 //#ifndef _WIN64 


//include .lib
#ifdef _DEBUG
#define _LIBNAME "dNovex.LIB"
#else
#define _LIBNAME "Novex.LIB"
#endif //_DEBUG

#pragma message( "Novex protection linking with " _LIBNAME )
#pragma comment(lib, _LIBNAME)
#endif

#endif //__NVCOMMON_H__
