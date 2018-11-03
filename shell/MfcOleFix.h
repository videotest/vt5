namespace{
template<int Ordinal
	, typename TMethodOrig, typename TMethodHook>
class CAPIHookMethod {
public:

	// Hook a function in all modules
  CAPIHookMethod(PSTR pszCalleeModName, TMethodHook hook, BOOL fExcludeAPIHookMod);

  // Unhook a function from all modules
   ~CAPIHookMethod();

	union PMethod 
	{
			 TMethodHook  m_MethPtr;
			 PROC m_pVoid[4];
	};
   // Returns the original address of the hooked function
   operator TMethodOrig() {
		 PMethod pMeth={m_MethPtr};
		 pMeth.m_pVoid[0]=m_pfnOrig;
		 return(pMeth.m_MethPtr); 
	 }

public:
   // Calls the real GetProcAddress 
   static FARPROC WINAPI GetProcAddressRaw(HMODULE hmod, PCSTR pszProcName);

private:
   //static PVOID sm_pvMaxAppAddr; // Maximum private memory address
   //static CAPIHookMethod* sm_pHead;    // Address of first object
   CAPIHook* m_pNext;            // Address of next  object

   PCSTR m_pszCalleeModName;     // Module containing the function (ANSI)
   PCSTR m_pszFuncName;          // Function name in callee (ANSI)
   PROC  m_pfnOrig;              // Original function address in callee
	 PMethod m_pfnHook;						// Hook function address
   BOOL  m_fExcludeAPIHookMod;   // Hook module w/CAPIHook implementation?

private:
   // Replaces a symbol's address in a module's import section
   static void WINAPI ReplaceIATEntryInAllMods(PCSTR pszCalleeModName, 
      PROC pfnOrig, PROC pfnHook, BOOL fExcludeAPIHookMod);

   // Replaces a symbol's address in all module's import sections
   static void WINAPI ReplaceIATEntryInOneMod(PCSTR pszCalleeModName, 
      PROC pfnOrig, PROC pfnHook, HMODULE hmodCaller);

private:
   // Used when a DLL is newly loaded after hooking a function
   static void    WINAPI FixupNewlyLoadedModule(HMODULE hmod, DWORD dwFlags);
};

template<int Ordinal, typename TMethodOrig, typename TMethodHook>
CAPIHookMethod<Ordinal, TMethodOrig, TMethodHook>::CAPIHookMethod
	(PSTR pszCalleeModName, TMethodHook hook, BOOL fExcludeAPIHookMod)
 {
   m_pNext  = CAPIHook::sm_pHead;    // The next node was at the head
   CAPIHook::sm_pHead = (CAPIHook*)this;        // This node is now at the head

   // Save information about this hooked function
   m_pszCalleeModName   = pszCalleeModName;
   m_pszFuncName        = (PSTR)Ordinal;
	 m_pfnHook.m_MethPtr	= hook;
   m_fExcludeAPIHookMod = fExcludeAPIHookMod;
   m_pfnOrig            = CAPIHook::GetProcAddressRaw(
      GetModuleHandleA(pszCalleeModName), m_pszFuncName);

   chASSERT(m_pfnOrig != NULL);  // Function doesn't exist

   // Hook this function in all currently loaded modules
	 CAPIHook::ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnOrig, m_pfnHook.m_pVoid[0], 
      m_fExcludeAPIHookMod);
}

template<int Ordinal, typename TMethodOrig, typename TMethodHook>
CAPIHookMethod<Ordinal, TMethodOrig, TMethodHook>::~CAPIHookMethod() {

   // Unhook this function from all modules
   CAPIHook::ReplaceIATEntryInAllMods(m_pszCalleeModName, m_pfnHook.m_pVoid[0], m_pfnOrig, 
      m_fExcludeAPIHookMod);

   // Remove this object from the linked list
	 CAPIHook* p = CAPIHook::sm_pHead; 
   if (p == (CAPIHook*)this) {     // Removing the head node
      CAPIHook::sm_pHead = p->m_pNext; 
   } else {

      BOOL fFound = FALSE;

      // Walk list from head and fix pointers
      for (; !fFound && (p->m_pNext != NULL); p = p->m_pNext) {
         if (p->m_pNext == (CAPIHook*)this) { 
            // Make the node that points to us point to the our next node
            p->m_pNext = p->m_pNext->m_pNext; 
            break; 
         }
      }
      chASSERT(fFound);
   }
}

#define HOOK_METHOD(dll,ord,ret,class_orig,method_orig,params) \
struct T##class_orig##method_orig##ord: public class_orig \
{ \
	typedef ret (T##class_orig##method_orig##ord::*TMethodHook)params; \
	ret Hook params; \
}; \
 \
CAPIHookMethod<ord,ret (class_orig::*) params, T##class_orig##method_orig##ord::TMethodHook>* \
	p##class_orig##method_orig##ord = new \
CAPIHookMethod<ord,ret (class_orig::*) params, T##class_orig##method_orig##ord::TMethodHook> \
(dll, &T##class_orig##method_orig##ord::Hook, FALSE); \
ret T##class_orig##method_orig##ord::Hook params

//	typedef ret (class_orig::*PFN_ODD_Orig) params;  
}