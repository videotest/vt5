void COleDispatchDriver::InvokeHelperV(DISPID dwDispID, WORD wFlags,
	VARTYPE vtRet, void* pvRet, const BYTE* pbParamInfo, va_list argList)
{
	if (m_lpDispatch == NULL)
	{
		TRACE(traceOle, 0, "Warning: attempt to call Invoke with NULL m_lpDispatch!\n");
		return;
	}

	COleDispParams dispparams;
	memset(&dispparams, 0, sizeof dispparams);

	// determine number of arguments
	if (pbParamInfo != NULL)
	{
		// Number of elements will never be biger than MAX_UINT thus casting
		dispparams.cArgs = static_cast<unsigned int>(strlen((LPCSTR)pbParamInfo));
	}

	DISPID dispidNamed = DISPID_PROPERTYPUT;
	if (wFlags & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF))
	{
		ASSERT(dispparams.cArgs > 0);
		dispparams.cNamedArgs = 1;
		dispparams.rgdispidNamedArgs = &dispidNamed;
	}
	CVariantBoolConverter tempArgs; //Used to convert VARIANT_BOOL | VT_BYREF --> BOOL*.
	if (dispparams.cArgs != 0)
	{
		// allocate memory for all VARIANT parameters
		VARIANT* pArg = new VARIANT[dispparams.cArgs];
		ASSERT(pArg != NULL);   // should have thrown exception
		dispparams.rgvarg = pArg;
		memset(pArg, 0, sizeof(VARIANT)* dispparams.cArgs);

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
				//CY is always passed by ref
				pArg->cyVal = *va_arg(argList, CY*);
				break;
			case VT_BSTR:
			{
							LPCOLESTR lpsz = va_arg(argList, LPOLESTR);
							pArg->bstrVal = ::SysAllocString(lpsz);
							if (lpsz != NULL && pArg->bstrVal == NULL)
							{
								AfxThrowMemoryException();
							}
			}
				break;
#if !defined(_UNICODE)
			case VT_BSTRA:
			{
							 LPCSTR lpsz = va_arg(argList, LPSTR);
							 CStringW strMBToUnicode(lpsz);
							 pArg->bstrVal = ::SysAllocString(static_cast<LPCWSTR>(strMBToUnicode));
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
				//VARIANT is always passed by ref
				*pArg = *va_arg(argList, VARIANT*);
				break;
			case VT_UNKNOWN:
				pArg->punkVal = va_arg(argList, LPUNKNOWN);
				break;

			case VT_UI1 | VT_BYREF:
				pArg->pbVal = va_arg(argList, BYTE*);
				break;
			case VT_UI2 | VT_BYREF:
				pArg->puiVal = va_arg(argList, USHORT*);
				break;
			case VT_UI4 | VT_BYREF:
				pArg->pulVal = va_arg(argList, ULONG*);
				break;
			case VT_UI8 | VT_BYREF:
				pArg->pullVal = va_arg(argList, ULONGLONG*);
				break;
			case VT_I1 | VT_BYREF:
				pArg->pcVal = va_arg(argList, char*);
				break;
			case VT_I2 | VT_BYREF:
				pArg->piVal = va_arg(argList, short*);
				break;
			case VT_I4 | VT_BYREF:
				pArg->plVal = va_arg(argList, long*);
				break;
			case VT_I8 | VT_BYREF:
				pArg->pllVal = va_arg(argList, LONGLONG*);
				break;
			case VT_R4 | VT_BYREF:
				pArg->pfltVal = va_arg(argList, float*);
				break;
			case VT_R8 | VT_BYREF:
				pArg->pdblVal = va_arg(argList, double*);
				break;
			case VT_DATE | VT_BYREF:
				pArg->pdate = va_arg(argList, DATE*);
				break;
			case VT_CY | VT_BYREF:
				pArg->pcyVal = va_arg(argList, CY*);
				break;
			case VT_BSTR | VT_BYREF:
				pArg->pbstrVal = va_arg(argList, BSTR*);
				break;
			case VT_DISPATCH | VT_BYREF:
				pArg->ppdispVal = va_arg(argList, LPDISPATCH*);
				break;
			case VT_ERROR | VT_BYREF:
				pArg->pscode = va_arg(argList, SCODE*);
				break;
			case VT_BOOL | VT_BYREF:
			{
									   // coerce BOOL into VARIANT_BOOL
									   BOOL* pboolVal = va_arg(argList, BOOL*);
									   *pboolVal = *pboolVal ? MAKELONG(0xffff, 0) : 0;
									   pArg->pboolVal = (VARIANT_BOOL*)pboolVal;
									   tempArgs.AddPair(CVariantBoolPair(pboolVal, pArg->pboolVal, FALSE));
			}
				break;
			case VT_VARIANT | VT_BYREF:
				pArg->pvarVal = va_arg(argList, VARIANT*);
				break;
			case VT_UNKNOWN | VT_BYREF:
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

	//When VT_BOOL | VT_BYREF is passed to com server, after Invoke returns - 
	//Convert VARIANT_TRUE[FALSE] --> BOOL TRUE[FALSE].	
	tempArgs.CopyVarBoolsIntoBOOLs();
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
	dispparams.rgvarg = NULL;

	// throw exception on failure
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
		if (::SysStringLen(excepInfo.bstrSource))
		{
			pException->m_strSource = excepInfo.bstrSource;
		}
		::SysFreeString(excepInfo.bstrSource);
		if (::SysStringLen(excepInfo.bstrDescription))
		{
			pException->m_strDescription = excepInfo.bstrDescription;
		}
		::SysFreeString(excepInfo.bstrDescription);
		if (::SysStringLen(excepInfo.bstrHelpFile))
		{
			pException->m_strHelpFile = excepInfo.bstrHelpFile;
		}
		::SysFreeString(excepInfo.bstrHelpFile);
		pException->m_dwHelpContext = excepInfo.dwHelpContext;
		pException->m_scError = excepInfo.scode;

		// then throw the exception
		THROW(pException);
	}

	if (vtRet != VT_EMPTY)
	{
		// convert return value
		if (vtRet != VT_VARIANT)
		{
			SCODE scChangeType = VariantChangeType(&vaResult, &vaResult, 0, vtRet);
			if (FAILED(scChangeType))
			{
				TRACE(traceOle, 0, "Warning: automation return value coercion failed.\n");
				VariantClear(&vaResult);
				AfxThrowOleException(scChangeType);
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
