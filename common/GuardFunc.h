#ifndef __GUARD_FUNCTION_H
#define __GUARD_FUNCTION_H

#include "Defs.h"
#include <objbase.h>
// helper functions used to defend apps from unauthorized using

// NOTE:
// before use one of this functions application must call CoInitialize(Ex)()

// CoCreateInstance
std_dll bool InstallCreateInstance(LPVOID lpvFuncAddr);
std_dll bool UninstallCreateInstance();
std_dll BYTE* GetDataCreateInstance();

// CoGetClassObject
std_dll bool InstallGetClassObject(LPVOID lpvFuncAddr);
std_dll bool UninstallGetClassObject();
std_dll BYTE* GetDataGetClassObject();

// CLSIDFromProgID
std_dll bool InstallCLSIDFromProgID(LPVOID lpvFuncAddr);
std_dll bool UninstallCLSIDFromProgID();
std_dll BYTE* GetDataCLSIDFromProgID();

// ProgIDFromCLSID
std_dll bool InstallProgIDFromCLSID(LPVOID lpvFuncAddr);
std_dll bool UninstallProgIDFromCLSID();
std_dll BYTE* GetDataProgIDFromCLSID();


// own functions to replace standard function in ole32.dll
#define EXPORT_STDAPI EXTERN_C std_dll HRESULT STDAPICALLTYPE

EXPORT_STDAPI CoCreateInstanceNew(REFCLSID rclsid, LPUNKNOWN pUnkOuter, DWORD dwClsContext, REFIID riid, LPVOID FAR* ppv);
EXPORT_STDAPI CoGetClassObjectNew(REFCLSID rclsid, DWORD dwClsContext, LPVOID pvReserved, REFIID riid, LPVOID FAR* ppv);
EXPORT_STDAPI ProgIDFromCLSIDNew(REFCLSID clsid, LPOLESTR FAR* lplpszProgID);
EXPORT_STDAPI CLSIDFromProgIDNew(LPCOLESTR lpszProgID, LPCLSID lpclsid);

#if (_WIN32_WINNT >= 0x0400 ) || defined(_WIN32_DCOM) // DCOM
// CoCreateInstanceEx
std_dll bool InstallCreateInstanceEx(LPVOID lpvFuncAddr);
std_dll bool UninstallCreateInstanceEx();
std_dll BYTE* GetDataCreateInstanceEx();
EXPORT_STDAPI CoCreateInstanceExNew(REFCLSID rclsid, LPUNKNOWN punkOuter, DWORD dwClsCtx, COSERVERINFO * pServerInfo, ULONG cmq, MULTI_QI * pResults);

#endif // DCOM


//std_dll bool TranslateProgID(BSTR bstrInnerProgID, BSTR * pbstrExtenProgID, BSTR bstrSuff = 0);



#define MUTEXNAME		_T("MyVTMutexSharing")
#define ENDEVENTNAME	_T("MyVTEvent1Sharing")
#define ERROREVENTNAME	_T("MyVTEvent2Sharing")

extern export_data HANDLE g_hMutex;
extern export_data HANDLE g_hEventEnd;
extern export_data HANDLE g_hEventError;

extern export_data const char lpszGuardFileSig[];

enum guardErrorCode
{
	guardNoError = 0,
	guardCreateFileMapping,
	guardOpenFileMapping,
	guardMapFile,

	guardNotExistsNSK,
	guardInvalidNSK,
	guardReadNSK,
	guardWriteNSK,

	guardInvalidPwd,
	guardInvalidGuardFile,
	guardReadGuardFile,
	guardWriteGuardFile,
	guardCryptFile,
	guardDecryptFile,
	guardInvalidTableSize,
	guardInvalidTable,
	guardMemory


};


// return Application is alredy running
std_dll bool GuardAppIsRunning();
// return errorCode
std_dll DWORD GuardGetErrorCode();
// Set errorCode
std_dll void GuardSetErrorCode(DWORD dwCode);
// get error msg
std_dll LPCTSTR GuardGetErrorMsg(guardErrorCode eCode);


// report error thread function
std_dll DWORD __stdcall ThreadFuncGuardError(LPVOID lpParam = 0);






#endif// __GUARD_FUNCTION_H