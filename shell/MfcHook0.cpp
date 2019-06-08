#include "stdafx.h"
#include "\vt5\awin\misc_map.h"
#include "MfcFix.h"

class __CCmdTarget__ : public CCmdTarget{
public:
	LPUNKNOWN __GetInterface__(const void* iid);
};

#define __GetInterfacePtr__(pTarget, pEntry) \
	((LPUNKNOWN)((BYTE*)pTarget + pEntry->nOffset))


LPUNKNOWN __CCmdTarget__::__GetInterface__(const void* iid)
{
	// allow general hook first chance
	LPUNKNOWN lpUnk;
	if ((lpUnk = GetInterfaceHook(iid)) != NULL)
	{
/*#ifdef _DEBUG
		LPCTSTR strIID = AfxGetIIDString(*(IID*)(iid));
		TRACE(traceOle, 1, _T("QueryInterface(%s) succeeded\n"), strIID);
#endif*/
		return lpUnk;
	}

	const AFX_INTERFACEMAP* pMap = GetInterfaceMap();
	ASSERT(pMap != NULL);
	DWORD lData1 = ((IID*)iid)->Data1;

	// IUnknown is a special case since nobody really implements *only* it!
	BOOL bUnknown = ((DWORD*)&IID_IUnknown)[0] == lData1 &&
		((DWORD*)iid)[1] == ((DWORD*)&IID_IUnknown)[1] &&
		((DWORD*)iid)[2] == ((DWORD*)&IID_IUnknown)[2] &&
		((DWORD*)iid)[3] == ((DWORD*)&IID_IUnknown)[3];
	if (bUnknown)
	{
		do
		{
			const AFX_INTERFACEMAP_ENTRY* pEntry = pMap->pEntry;
			ASSERT(pEntry != NULL);
			while (pEntry->piid != NULL)
			{
				// check INTERFACE_ENTRY macro
				LPUNKNOWN lpUnk = __GetInterfacePtr__(this, pEntry);

				// check vtable pointer (can be NULL)
				if (*(DWORD*)lpUnk != 0)
				{
/*#ifdef _DEBUG
					LPCTSTR strIID = AfxGetIIDString(*(IID*)(iid));
					TRACE(traceOle, 1, _T("QueryInterface(%s) succeeded\n"), strIID);
#endif*/
					return lpUnk;
				}

				// entry did not match -- keep looking
				++pEntry;
			}
#ifdef _AFXDLL
			if (pMap->pfnGetBaseMap == NULL)
				break;
			pMap = (*pMap->pfnGetBaseMap)();
		} while (1);
#else
		} while ((pMap = pMap->pBaseMap) != NULL);
#endif

/*#ifdef _DEBUG
		LPCTSTR strIID = AfxGetIIDString(*(IID*)(iid));
		TRACE(traceOle, 1, _T("QueryInterface(%s) failed\n"), strIID);
#endif*/
		// interface ID not found, fail the call
		return NULL;
	}

	// otherwise, walk the interface map to find the matching IID
	do
	{
		const AFX_INTERFACEMAP_ENTRY* pEntry = pMap->pEntry;
		ASSERT(pEntry != NULL);
		while (pEntry->piid != NULL)
		{
			if (((DWORD*)pEntry->piid)[0] == lData1 &&
				((DWORD*)pEntry->piid)[1] == ((DWORD*)iid)[1] &&
				((DWORD*)pEntry->piid)[2] == ((DWORD*)iid)[2] &&
				((DWORD*)pEntry->piid)[3] == ((DWORD*)iid)[3])
			{
				// check INTERFACE_ENTRY macro
				LPUNKNOWN lpUnk = __GetInterfacePtr__(this, pEntry);

				// check vtable pointer (can be NULL)
				if (*(DWORD*)lpUnk != 0)
				{
/*#ifdef _DEBUG
					LPCTSTR strIID = AfxGetIIDString(*(IID*)(iid));
					TRACE(traceOle, 1, _T("QueryInterface(%s) succeeded\n"), strIID);
#endif*/
					return lpUnk;
				}
			}

			// entry did not match -- keep looking
			++pEntry;
		}
#ifdef _AFXDLL
		if (pMap->pfnGetBaseMap == NULL)
			break;
		pMap = (*pMap->pfnGetBaseMap)();
	} while (1);
#else
	} while ((pMap = pMap->pBaseMap) != NULL);
#endif

/*#ifdef _DEBUG
	LPCTSTR strIID = AfxGetIIDString(*(IID*)(iid));
	TRACE(traceOle, 1, _T("QueryInterface(%s) failed\n"), strIID);
#endif*/
	// interface ID not found, fail the call
	return NULL;
}

class __COleDispatchDriver__ : public COleDispatchDriver
{
public:
	void __InvokeHelperV__( DISPID dwDispID, WORD wFlags,
		VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, va_list argList );
};

void __COleDispatchDriver__::__InvokeHelperV__(DISPID dwDispID, WORD wFlags,
	VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, va_list argList)
{
	USES_CONVERSION;

	if (m_lpDispatch == NULL)
	{
		TRACE(traceOle, 0, "Warning: attempt to call Invoke with NULL m_lpDispatch!\n");
		return;
	}

	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof dispparams);

	// determine number of arguments
	if (pbParamInfo != NULL)
		dispparams.cArgs = lstrlenA((LPCSTR)pbParamInfo);

	DISPID dispidNamed = DISPID_PROPERTYPUT;
	if (wFlags & (DISPATCH_PROPERTYPUT|DISPATCH_PROPERTYPUTREF))
	{
		ASSERT(dispparams.cArgs > 0);
		dispparams.cNamedArgs = 1;
		dispparams.rgdispidNamedArgs = &dispidNamed;
	}

	if (dispparams.cArgs != 0)
	{
		// allocate memory for all VARIANT parameters
		VARIANT* pArg = new VARIANT[dispparams.cArgs];
		ASSERT(pArg != NULL);   // should have thrown exception
		dispparams.rgvarg = pArg;
		memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);

		// get ready to walk vararg list
		const BYTE* pb = pbParamInfo;
		pArg += dispparams.cArgs - 1;   // params go in opposite order

		while (*pb != 0)
		{
			ASSERT(pArg >= dispparams.rgvarg);

			pArg->vt = *pb; // set the variant type
			if (pArg->vt & VT_MFCBYREF)
			{
				pArg->vt &= ~VT_MFCBYREF;
				pArg->vt |= VT_BYREF;
			}
			switch (pArg->vt)
			{
			case VT_UI1:
				pArg->bVal = va_arg(argList, BYTE);
				break;
			case VT_UI2:
				pArg->uiVal = va_arg(argList, USHORT);
				break;
			case VT_UI4:
				pArg->ulVal = va_arg(argList, ULONG);
				break;
			case VT_UI8:
				pArg->ullVal = va_arg(argList, ULONGLONG);
				break;
			case VT_I1:
				pArg->cVal = va_arg(argList, char);
				break;
			case VT_I2:
				pArg->iVal = va_arg(argList, short);
				break;
			case VT_I4:
				pArg->lVal = va_arg(argList, long);
				break;
			case VT_I8:
				pArg->llVal = va_arg(argList, LONGLONG);
				break;
			case VT_R4:
				pArg->fltVal = (float)va_arg(argList, double);
				break;
			case VT_R8:
				pArg->dblVal = va_arg(argList, double);
				break;
			case VT_DATE:
				pArg->date = va_arg(argList, DATE);
				break;
			case VT_CY:
				pArg->cyVal = *va_arg(argList, CY*);
				break;
			case VT_BSTR:
				{
					LPCOLESTR lpsz = va_arg(argList, LPOLESTR);
					pArg->bstrVal = ::SysAllocString(lpsz);
					if (lpsz != NULL && pArg->bstrVal == NULL)
						AfxThrowMemoryException();
				}
				break;
#if !defined(_UNICODE)
			case VT_BSTRA:
				{
					LPCSTR lpsz = va_arg(argList, LPSTR);
					pArg->bstrVal = ::SysAllocString(T2COLE(lpsz));
					if (lpsz != NULL && pArg->bstrVal == NULL)
						AfxThrowMemoryException();
					pArg->vt = VT_BSTR;
				}
				break;
#endif
			case VT_DISPATCH:
				pArg->pdispVal = va_arg(argList, LPDISPATCH);
				break;
			case VT_ERROR:
				pArg->scode = va_arg(argList, SCODE);
				break;
			case VT_BOOL:
				V_BOOL(pArg) = (VARIANT_BOOL)(va_arg(argList, BOOL) ? -1 : 0);
				break;
			case VT_VARIANT:
				*pArg = *va_arg(argList, VARIANT*);
				break;
			case VT_UNKNOWN:
				pArg->punkVal = va_arg(argList, LPUNKNOWN);
				break;

			case VT_UI1|VT_BYREF:
				pArg->pbVal = va_arg(argList, BYTE*);
				break;
			case VT_UI2|VT_BYREF:
				pArg->puiVal = va_arg(argList, USHORT*);
				break;
			case VT_UI4|VT_BYREF:
				pArg->pulVal = va_arg(argList, ULONG*);
				break;
			case VT_UI8|VT_BYREF:
				pArg->pullVal = va_arg(argList, ULONGLONG*);
				break;
			case VT_I1|VT_BYREF:
				pArg->pcVal = va_arg(argList, char*);
				break;
			case VT_I2|VT_BYREF:
				pArg->piVal = va_arg(argList, short*);
				break;
			case VT_I4|VT_BYREF:
				pArg->plVal = va_arg(argList, long*);
				break;
			case VT_I8|VT_BYREF:
				pArg->pllVal = va_arg(argList, LONGLONG*);
				break;
			case VT_R4|VT_BYREF:
				pArg->pfltVal = va_arg(argList, float*);
				break;
			case VT_R8|VT_BYREF:
				pArg->pdblVal = va_arg(argList, double*);
				break;
			case VT_DATE|VT_BYREF:
				pArg->pdate = va_arg(argList, DATE*);
				break;
			case VT_CY|VT_BYREF:
				pArg->pcyVal = va_arg(argList, CY*);
				break;
			case VT_BSTR|VT_BYREF:
				pArg->pbstrVal = va_arg(argList, BSTR*);
				break;
			case VT_DISPATCH|VT_BYREF:
				pArg->ppdispVal = va_arg(argList, LPDISPATCH*);
				break;
			case VT_ERROR|VT_BYREF:
				pArg->pscode = va_arg(argList, SCODE*);
				break;
			case VT_BOOL|VT_BYREF:
				{
					// coerce BOOL into VARIANT_BOOL
					BOOL* pboolVal = va_arg(argList, BOOL*);
					*pboolVal = *pboolVal ? MAKELONG(0xffff, 0) : 0;
					pArg->pboolVal = (VARIANT_BOOL*)pboolVal;
				}
				break;
			case VT_VARIANT|VT_BYREF:
				pArg->pvarVal = va_arg(argList, VARIANT*);
				break;
			case VT_UNKNOWN|VT_BYREF:
				pArg->ppunkVal = va_arg(argList, LPUNKNOWN*);
				break;

			default:
				ASSERT(FALSE);  // unknown type!
				break;
			}

			--pArg; // get ready to fill next argument
			++pb;
		}
	}

	// initialize return value
	VARIANT* pvarResult = NULL;
	VARIANT vaResult;
	AfxVariantInit(&vaResult);
	if (vtRet != VT_EMPTY)
		pvarResult = &vaResult;

	// initialize EXCEPINFO struct
	EXCEPINFO excepInfo;
	memset(&excepInfo, 0, sizeof excepInfo);

	UINT nArgErr = (UINT)-1;  // initialize to invalid arg

	// make the call
	SCODE sc = m_lpDispatch->Invoke(dwDispID, IID_NULL, 0, wFlags,
		&dispparams, pvarResult, &excepInfo, &nArgErr);

	// cleanup any arguments that need cleanup
	if (dispparams.cArgs != 0)
	{
		VARIANT* pArg = dispparams.rgvarg + dispparams.cArgs - 1;
		const BYTE* pb = pbParamInfo;
		while (*pb != 0)
		{
			switch ((VARTYPE)*pb)
			{
#if !defined(_UNICODE)
			case VT_BSTRA:
#endif
			case VT_BSTR:
				VariantClear(pArg);
				break;
			}
			--pArg;
			++pb;
		}
	}
	delete[] dispparams.rgvarg;

	/*// throw exception on failure
	if (FAILED(sc))
	{
		VariantClear(&vaResult);
		if (sc != DISP_E_EXCEPTION)
		{
			// non-exception error code
			AfxThrowOleException(sc);			
		}

		// make sure excepInfo is filled in
		if (excepInfo.pfnDeferredFillIn != NULL)
			excepInfo.pfnDeferredFillIn(&excepInfo);

		// allocate new exception, and fill it
		COleDispatchException* pException =
			new COleDispatchException(NULL, 0, excepInfo.wCode);
		ASSERT(pException->m_wCode == excepInfo.wCode);
		if (excepInfo.bstrSource != NULL)
		{
			pException->m_strSource = excepInfo.bstrSource;
			SysFreeString(excepInfo.bstrSource);
		}
		if (excepInfo.bstrDescription != NULL)
		{
			pException->m_strDescription = excepInfo.bstrDescription;
			SysFreeString(excepInfo.bstrDescription);
		}
		if (excepInfo.bstrHelpFile != NULL)
		{
			pException->m_strHelpFile = excepInfo.bstrHelpFile;
			SysFreeString(excepInfo.bstrHelpFile);
		}
		pException->m_dwHelpContext = excepInfo.dwHelpContext;
		pException->m_scError = excepInfo.scode;

		// then throw the exception
		THROW(pException);
		ASSERT(FALSE);  // not reached
	}
	*/

	if (vtRet != VT_EMPTY)
	{
		// convert return value
		if (vtRet != VT_VARIANT)
		{
			SCODE sc = VariantChangeType(&vaResult, &vaResult, 0, vtRet);
			if (FAILED(sc))
			{
				TRACE(traceOle, 0, "Warning: automation return value coercion failed.\n");
				VariantClear(&vaResult);
				AfxThrowOleException(sc);
			}
			ASSERT(vtRet == vaResult.vt);
		}

		// copy return value into return spot!
		switch (vtRet)
		{
		case VT_UI1:
			*(BYTE*)pvRet = vaResult.bVal;
			break;
		case VT_UI2:
			*(USHORT*)pvRet = vaResult.uiVal;
			break;
		case VT_UI4:
			*(ULONG*)pvRet = vaResult.ulVal;
			break;
		case VT_UI8:
			*(ULONGLONG*)pvRet = vaResult.ullVal;
			break;
		case VT_I1:
			*(char*)pvRet = vaResult.cVal;
			break;
		case VT_I2:
			*(short*)pvRet = vaResult.iVal;
			break;
		case VT_I4:
			*(long*)pvRet = vaResult.lVal;
			break;
		case VT_I8:
			*(LONGLONG*)pvRet = vaResult.llVal;
			break;
		case VT_R4:
			*(float*)pvRet = vaResult.fltVal;
			break;
		case VT_R8:
			*(double*)pvRet = vaResult.dblVal;
			break;
		case VT_DATE:
			*(double*)pvRet = *(double*)&vaResult.date;
			break;
		case VT_CY:
			*(CY*)pvRet = vaResult.cyVal;
			break;
		case VT_BSTR:
			AfxBSTR2CString((CString*)pvRet, vaResult.bstrVal);
			SysFreeString(vaResult.bstrVal);
			break;
		case VT_DISPATCH:
			*(LPDISPATCH*)pvRet = vaResult.pdispVal;
			break;
		case VT_ERROR:
			*(SCODE*)pvRet = vaResult.scode;
			break;
		case VT_BOOL:
			*(BOOL*)pvRet = (V_BOOL(&vaResult) != 0);
			break;
		case VT_VARIANT:
			*(VARIANT*)pvRet = vaResult;
			break;
		case VT_UNKNOWN:
			*(LPUNKNOWN*)pvRet = vaResult.punkVal;
			break;

		default:
			ASSERT(FALSE);  // invalid return type specified
		}
	}
}


BYTE __GetInterfaceOriginalCode[16]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
BYTE __InvokeDispHelperCode[16]	= { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


#define C1(code,ftype)						\
BYTE * pCode = (BYTE*)code;					\
DWORD dw = 0;								\
memcpy(&dw, &pCode[8], sizeof(DWORD));		\
memcpy((BYTE*)dw, &pCode[1], 7);			\
ftype pfn = (ftype)dw;						\

#define C2()								\
BYTE * ptr = (BYTE*)dw;						\
*ptr = 0xB8;ptr++;							\
memcpy(ptr, &pCode[12], sizeof(DWORD));		\
ptr += 4;									\
*ptr = 0xFF;ptr++;							\
*ptr = 0xE0;ptr++; 							\



bool _install_function_hook( BYTE* orig_code, LPVOID pfunc_addr_old, LPVOID pfunc_addr_new );

bool InstallMfcHook()
{
	
	//install CCmdTarget::GetInterface	
	{
		//old address
		LPUNKNOWN (CCmdTarget::*  pint_old)(const void *) = &CCmdTarget::GetInterface;	
		LPBYTE pfunc_addr_old = (LPBYTE)*(void**)&pint_old;
		if(0xe9 == *pfunc_addr_old){
			pfunc_addr_old += *((int*)(pfunc_addr_old+1))  + 5;
		}
		if(0xFF == *pfunc_addr_old && 0x25 == *(pfunc_addr_old+1) ){
			int pb=*(int*)(pfunc_addr_old+2);
			pfunc_addr_old = (LPBYTE)(*(int*)pb);
		}

		//new address
		LPUNKNOWN (__CCmdTarget__::*  pint_new)(const void *) = &__CCmdTarget__::__GetInterface__;	
		LPVOID pfunc_addr_new = *(void**)&pint_new;

		HMODULE hmod = ::GetModuleHandle( MFCXXD_DLL );
		if( hmod )
		{
//			pfunc_addr_old = ::GetProcAddress( hmod, (LPCSTR)3788 );	
//			if( pfunc_addr_old )
//				VERIFY( _install_function_hook( __GetInterfaceOriginalCode, pfunc_addr_old, pfunc_addr_new ) );
		}
	}
	
	//install __AfxThrowOleException	
	if( 1 )
	{
		//old address
		void (COleDispatchDriver::*pint_old)( DISPID dwDispID, WORD wFlags,
			VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, va_list argList ) = &COleDispatchDriver::InvokeHelperV;	
		LPBYTE pfunc_addr_old = (LPBYTE)*(void**)&pint_old;
		if(0xe9 == *pfunc_addr_old){
			pfunc_addr_old += *((int*)(pfunc_addr_old+1))  + 5;
		}
		if(0xFF == *pfunc_addr_old && 0x25 == *(pfunc_addr_old+1) ){
			int pb=*(int*)(pfunc_addr_old+2);
			pfunc_addr_old = (LPBYTE)(*(int*)pb);
		}

		//new address
		void (__COleDispatchDriver__::*pint_new)( DISPID dwDispID, WORD wFlags,
			VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, va_list argList ) = &__COleDispatchDriver__::__InvokeHelperV__;	
		LPVOID pfunc_addr_new = *(void**)&pint_new;

		HMODULE hmod = ::GetModuleHandle( MFCXXD_DLL );
		if( hmod )
		{
			if( pfunc_addr_old )
				VERIFY( _install_function_hook( __InvokeDispHelperCode, pfunc_addr_old, pfunc_addr_new ) );
		}
	}

	return true;
	
}

bool _install_function_hook( BYTE* orig_code, LPVOID pfunc_addr_old, LPVOID pfunc_addr_new )
{
	//original address
	/*
	{
		LPVOID paddr_jmp = *(void**)&pint_old;
		
		//test for jump
		{
			BYTE* pbuf = (BYTE*)paddr_jmp;
			if( pbuf[0] != 0xE9 )
			{
				ASSERT(false);
				return false;
			}

			BYTE* pbuf_offest = pbuf + 1;
			DWORD* pdw_offset = (DWORD*)pbuf_offest;

			DWORD dw_address = (DWORD)pbuf + *pdw_offset + 5;
			pfunc_addr_old = (void*)dw_address;
		}
	}	
	*/

	if( !orig_code || !pfunc_addr_old || !pfunc_addr_new )
	{
		ASSERT( false );
		return false;
	}

	if (orig_code[0] != 0x90)
	{
		// set all NOP to saved code
		memset(orig_code, 0x90, 16);
		// and copy piece of original code to this place
		memcpy(&orig_code[1], (BYTE*)pfunc_addr_old, 7);
	
		DWORD dw = (DWORD)pfunc_addr_old;
		memcpy(&orig_code[8], &dw, sizeof(DWORD));

		memcpy(&orig_code[12], &pfunc_addr_new, sizeof(DWORD));
	}

	// change protection for .dll mapping page if it's needed
	/*LPVOID lpBase = 0;
	LONG lSize = 0;
	bool bNeedChangeProtect = false;
	GetPageInfo(pfunc_addr_old, &lpBase, &lSize, &bNeedChangeProtect);
	if (lpBase && lSize && bNeedChangeProtect)*/
	{
		SetLastError(0);
		DWORD dwOldProtect = 0;
		//BOOL b = VirtualProtect(lpBase, lSize, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		BOOL b = VirtualProtect(pfunc_addr_old, 32, PAGE_EXECUTE_READWRITE, &dwOldProtect);
		//__GetPageInfo(lp, &lpBase, &lSize, &bNeedChangeProtect);

		if (ERROR_SUCCESS != GetLastError())
		{
			AfxMessageBox( "InstallMfcHook:ERROR_SUCCESS != GetLastError()" );
			return false;
		}
	}

	// modify original code
	BYTE * ptr = (BYTE*)pfunc_addr_old;
	*ptr = 0xB8;ptr++; // mov eax, &data
	memcpy(ptr, &pfunc_addr_new, sizeof(DWORD));
	ptr += 4;
	*ptr = 0xFF;ptr++; // jmp eax
	*ptr = 0xE0;ptr++; // 

	return true;
}


