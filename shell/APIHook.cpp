/******************************************************************************
Module:  APIHook.cpp
Notices: Copyright (c) 2000 Jeffrey Richter
******************************************************************************/

#include "stdafx.h"

#include "CmnHdr.h"
#include <ImageHlp.h>
#pragma comment(lib, "ImageHlp")

#include "APIHook.h"
#include "Toolhelp.h"
#include <crtdbg.h>

///////////////////////////////////////////////////////////////////////////////


// When an application runs on Windows 98 under a debugger, the debugger
// makes the module's import section point to a stub that calls the desired 
// function. To account for this, the code in this module must do some crazy 
// stuff. These variables are needed to help with the crazy stuff.


// The highest private memory address (used for Windows 98 only)
//PVOID CAPIHook::sm_pvMaxAppAddr = NULL;
const BYTE cPushOpCode = 0x68;   // The PUSH opcode on x86 platforms


///////////////////////////////////////////////////////////////////////////////


// The head of the linked-list of CAPIHook objects
CAPIHook* CAPIHook::sm_pHead = NULL;


///////////////////////////////////////////////////////////////////////////////


CAPIHook::CAPIHook(PSTR pszCalleeModName, PSTR pszFuncName, PROC pfnHook, 
   BOOL fExcludeAPIHookMod) {

   //if (sm_pvMaxAppAddr == NULL) {
   //   // Functions with address above lpMaximumApplicationAddress require
   //   // special processing (Windows 98 only)
   //   SYSTEM_INFO si;
   //   GetSystemInfo(&si);
   //   sm_pvMaxAppAddr = si.lpMaximumApplicationAddress;
   //}

   m_pNext  = sm_pHead;    // The next node was at the head
   sm_pHead = this;        // This node is now at the head

   // Save information about this hooked function
   m_pszCalleeModName   = pszCalleeModName;
   m_pszFuncName        = pszFuncName;
   m_pfnHook            = pfnHook;
   m_fExcludeAPIHookMod = fExcludeAPIHookMod;
   m_pfnOrig            = GetProcAddressRaw(
      GetModuleHandleA(pszCalleeModName), m_pszFuncName);
   chASSERT(m_pfnOrig != NULL);  // Function doesn't exist

   //if (m_pfnOrig > sm_pvMaxAppAddr) {
   //   // The address is in a shared DLL; the address needs fixing up 
   //   PBYTE pb = (PBYTE) m_pfnOrig;
   //   if (pb[0] == cPushOpCode) {
   //      // Skip over the PUSH op code and grab the real address
   //      PVOID pv = * (PVOID*) &pb[1];
   //      m_pfnOrig = (PROC) pv;
   //   }
   //}

   // Hook this function in all currently loaded modules
   ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnOrig, m_pfnHook, 
      m_fExcludeAPIHookMod);
}

///////////////////////////////////////////////////////////////////////////////


CAPIHook::~CAPIHook() {

   // Unhook this function from all modules
   ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnHook, m_pfnOrig, 
      m_fExcludeAPIHookMod);

   // Remove this object from the linked list
   CAPIHook* p = sm_pHead; 
   if (p == this) {     // Removing the head node
      sm_pHead = p->m_pNext; 
   } else {

      BOOL fFound = FALSE;

      // Walk list from head and fix pointers
      for (; !fFound && (p->m_pNext != NULL); p = p->m_pNext) {
         if (p->m_pNext == this) { 
            // Make the node that points to us point to the our next node
            p->m_pNext = p->m_pNext->m_pNext; 
            break; 
         }
      }
      chASSERT(fFound);
   }
}


///////////////////////////////////////////////////////////////////////////////

//typedef WINBASEAPI FARPROC WINAPI GetProcAddress(IN HMODULE hModule,IN LPCSTR lpProcName);
//FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hmod, PCSTR pszProcName);

// NOTE: This function must NOT be inlined
FARPROC CAPIHook::GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName) {
	FARPROC (WINAPI *pfnGetProcAddress)(IN HMODULE hModule,IN LPCSTR lpProcName)=&::GetProcAddress;
	if(&CAPIHook::GetProcAddress==pfnGetProcAddress){
		pfnGetProcAddress=(FARPROC (WINAPI*)(IN HMODULE hModule,IN LPCSTR lpProcName))sm_GetProcAddress.m_pfnOrig;
	}
	return pfnGetProcAddress(hmod, pszProcName);
}


///////////////////////////////////////////////////////////////////////////////


// Returns the HMODULE that contains the specified memory address
static HMODULE ModuleFromAddress(PVOID pv) {

   MEMORY_BASIC_INFORMATION mbi;
   return((VirtualQuery(pv, &mbi, sizeof(mbi)) != 0) 
      ? (HMODULE) mbi.AllocationBase : NULL);
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceIATEntryInAllMods(PCSTR pszCalleeModName, 
   PROC pfnCurrent, PROC pfnNew, BOOL fExcludeAPIHookMod) {

   HMODULE hmodThisMod = fExcludeAPIHookMod 
      ? ModuleFromAddress(ReplaceIATEntryInAllMods) : NULL;

   // Get the list of modules in this process
   CToolhelp th(TH32CS_SNAPMODULE, GetCurrentProcessId());

   MODULEENTRY32 me = { sizeof(me) };
   for (BOOL fOk = th.ModuleFirst(&me); fOk; fOk = th.ModuleNext(&me)) {

      // NOTE: We don't hook functions in our own module
      if (me.hModule != hmodThisMod) {

         // Hook this function in this module
         ReplaceIATEntryInOneMod(
            pszCalleeModName, pfnCurrent, pfnNew, me.hModule);
      }
   }
}


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::ReplaceIATEntryInOneMod(PCSTR pszCalleeModName, 
   PROC pfnCurrent, PROC pfnNew, HMODULE hmodCaller) {

   // Get the address of the module's import section
   ULONG ulSize;
   PIMAGE_IMPORT_DESCRIPTOR pImportDesc = (PIMAGE_IMPORT_DESCRIPTOR)
      ImageDirectoryEntryToData(hmodCaller, TRUE, 
      IMAGE_DIRECTORY_ENTRY_IMPORT, &ulSize);

   if (pImportDesc == NULL)
      return;  // This module has no import section


   // Find the import descriptor containing references to callee's functions
   for (; pImportDesc->Name; pImportDesc++) {
      PSTR pszModName = (PSTR) ((PBYTE) hmodCaller + pImportDesc->Name);
      if (lstrcmpiA(pszModName, pszCalleeModName) == 0) 
         break;   // Found
   }

   if (pImportDesc->Name == 0)
      return;  // This module doesn't import any functions from this callee

   // Get caller's import address table (IAT) for the callee's functions
   PIMAGE_THUNK_DATA pThunk = (PIMAGE_THUNK_DATA) 
      ((PBYTE) hmodCaller + pImportDesc->FirstThunk);

   // Replace current function address with new function address
   for (; pThunk->u1.Function; pThunk++) {

      // Get the address of the function address
      PROC* ppfn = (PROC*) &pThunk->u1.Function;

      // Is this the function we're looking for?
      BOOL fFound = (*ppfn == pfnCurrent);

      //if (!fFound && (*ppfn > sm_pvMaxAppAddr)) {

      //   // If this is not the function and the address is in a shared DLL, 
      //   // then maybe we're running under a debugger on Windows 98. In this 
      //   // case, this address points to an instruction that may have the 
      //   // correct address.

      //   PBYTE pbInFunc = (PBYTE) *ppfn;
      //   if (pbInFunc[0] == cPushOpCode) {
      //      // We see the PUSH instruction, the real function address follows
      //      ppfn = (PROC*) &pbInFunc[1];

      //      // Is this the function we're looking for?
      //      fFound = (*ppfn == pfnCurrent);
      //   }
      //}

      if (fFound) {
         // The addresses match, change the import section address
				DWORD dwDummy;
				::VirtualProtect(ppfn,sizeof(ppfn),PAGE_EXECUTE_READWRITE,&dwDummy);
        BOOL b=WriteProcessMemory(GetCurrentProcess(), ppfn, &pfnNew, 
            sizeof(pfnNew), NULL);
				if(!b)
					_ASSERTE(!"Not hooked");
         return;  // We did it, get out
      }
   }

   // If we get to here, the function is not in the caller's import section
}


///////////////////////////////////////////////////////////////////////////////


// Hook LoadLibrary functions and GetProcAddress so that hooked functions
// are handled correctly if these functions are called.

CAPIHook CAPIHook::sm_LoadLibraryA  ("Kernel32.dll", "LoadLibraryA",   
   (PROC) CAPIHook::LoadLibraryA, FALSE);

CAPIHook CAPIHook::sm_LoadLibraryW  ("Kernel32.dll", "LoadLibraryW",   
   (PROC) CAPIHook::LoadLibraryW, FALSE);

CAPIHook CAPIHook::sm_LoadLibraryExA("Kernel32.dll", "LoadLibraryExA", 
   (PROC) CAPIHook::LoadLibraryExA, FALSE);

CAPIHook CAPIHook::sm_LoadLibraryExW("Kernel32.dll", "LoadLibraryExW", 
   (PROC) CAPIHook::LoadLibraryExW, FALSE);

CAPIHook CAPIHook::sm_GetProcAddress("Kernel32.dll", "GetProcAddress", 
   (PROC) CAPIHook::GetProcAddress, FALSE);


///////////////////////////////////////////////////////////////////////////////


void CAPIHook::FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags) {

   // If a new module is loaded, hook the hooked functions
   if ((hmod != NULL) && ((dwFlags & LOAD_LIBRARY_AS_DATAFILE) == 0)) {

      for (CAPIHook* p = sm_pHead; p != NULL; p = p->m_pNext) {
         ReplaceIATEntryInOneMod(p->m_pszCalleeModName, 
            p->m_pfnOrig, p->m_pfnHook, hmod);
      }
   }
}


///////////////////////////////////////////////////////////////////////////////
namespace{
	DWORD Index_LoadLibrary=TlsAlloc();
	BOOL bGetRecursion(){return (BOOL)TlsGetValue(Index_LoadLibrary);}
	void SetRecursion(BOOL b){
		TlsSetValue(Index_LoadLibrary,(LPVOID)b);
	}
}
typedef	HMODULE (WINAPI *PFN_LoadLibraryA)(PCSTR pszModulePath);

HMODULE WINAPI CAPIHook::LoadLibraryA(PCSTR pszModulePath) {
	PFN_LoadLibraryA pfn_LoadLibraryA=&::LoadLibraryA;
	if(&CAPIHook::LoadLibraryA==pfn_LoadLibraryA){
		pfn_LoadLibraryA=(PFN_LoadLibraryA)sm_LoadLibraryA.m_pfnOrig;
	}
	HMODULE hmod=0;
	BOOL b_LoadLibraryRecursion=bGetRecursion();
	if(!b_LoadLibraryRecursion){
			SetRecursion(TRUE);
			hmod = pfn_LoadLibraryA(pszModulePath);
			int err;
			if(0==hmod){
				err=GetLastError();
			}
			FixupNewlyLoadedModule(hmod, 0);
			SetRecursion(FALSE);
	}else{
		hmod=((PFN_LoadLibraryA)sm_LoadLibraryA.m_pfnOrig)(pszModulePath);
	}
   return(hmod);
}


///////////////////////////////////////////////////////////////////////////////
typedef	HMODULE (WINAPI *PFN_LoadLibraryW)(PCWSTR pszModulePath);

HMODULE WINAPI CAPIHook::LoadLibraryW(PCWSTR pszModulePath) {
	PFN_LoadLibraryW pfn_LoadLibraryW=&::LoadLibraryW;
	if(&CAPIHook::LoadLibraryW==pfn_LoadLibraryW){
		pfn_LoadLibraryW=(PFN_LoadLibraryW)sm_LoadLibraryW.m_pfnOrig;
	}
  HMODULE hmod = pfn_LoadLibraryW(pszModulePath);
   FixupNewlyLoadedModule(hmod, 0);
   return(hmod);
}


///////////////////////////////////////////////////////////////////////////////
typedef	HMODULE (WINAPI *PFN_LoadLibraryExA)(PCSTR pszModulePath, HANDLE hFile, DWORD dwFlags);

HMODULE WINAPI CAPIHook::LoadLibraryExA(PCSTR pszModulePath, HANDLE hFile, DWORD dwFlags) {
	PFN_LoadLibraryExA pfn_LoadLibraryExA=&::LoadLibraryExA;
	if(&CAPIHook::LoadLibraryExA==pfn_LoadLibraryExA){
		pfn_LoadLibraryExA=(PFN_LoadLibraryExA)sm_LoadLibraryExA.m_pfnOrig;
	}
	HMODULE hmod = pfn_LoadLibraryExA(pszModulePath, hFile, dwFlags);
   FixupNewlyLoadedModule(hmod, dwFlags);
   return(hmod);
}


///////////////////////////////////////////////////////////////////////////////
typedef	HMODULE (WINAPI *PFN_LoadLibraryExW)(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags);

HMODULE WINAPI CAPIHook::LoadLibraryExW(PCWSTR pszModulePath, HANDLE hFile, DWORD dwFlags) {
	PFN_LoadLibraryExW pfn_LoadLibraryExW=&::LoadLibraryExW;
	if(&CAPIHook::LoadLibraryExW==pfn_LoadLibraryExW){
		pfn_LoadLibraryExW=(PFN_LoadLibraryExW)sm_LoadLibraryExW.m_pfnOrig;
	}
	HMODULE hmod = pfn_LoadLibraryExW(pszModulePath, hFile, dwFlags);
   FixupNewlyLoadedModule(hmod, dwFlags);
   return(hmod);
}


///////////////////////////////////////////////////////////////////////////////


FARPROC WINAPI CAPIHook::GetProcAddress(HMODULE hmod, PCSTR pszProcName) {

   // Get the true address of the function
   FARPROC pfn = GetProcAddressRaw(hmod, pszProcName);
	{
   // Is it one of the functions that we want hooked?
   CAPIHook* p = sm_pHead;
   for (; (pfn != NULL) && (p != NULL); p = p->m_pNext) {

      if (pfn == p->m_pfnOrig) {

         // The address to return matches an address we want to hook
         // Return the hook function address instead
         pfn = p->m_pfnHook;
         break;
      }
   }
	}
   return(pfn);
}


//////////////////////////////// End of File //////////////////////////////////
