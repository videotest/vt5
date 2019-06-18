#include "stdafx.h"
#include "mtd_utils.h"
#include "ScriptNotifyInt.h"

BOOL	get_method_by_name( BSTR bstr_mtd_name, IMethodMan *pmtd_man, IUnknown **ppunk_mtd, long *plpos_mtd /*= 0*/ )
{
	if( !bstr_mtd_name || (!ppunk_mtd && !plpos_mtd) )
		return FALSE;

	IMethodManPtr sptr_mtd_man;
	if( !pmtd_man )
	{
		IUnknown *punk_mtd_man = _GetOtherComponent( GetAppUnknown(), IID_IMethodMan ); 
		sptr_mtd_man = punk_mtd_man;
		if( punk_mtd_man )
			punk_mtd_man->Release(); punk_mtd_man = 0;
	}
	else
        sptr_mtd_man = pmtd_man;

	if( sptr_mtd_man == 0 )
		return FALSE;

	// search method by name
    long lpos_mtd = 0;
	
	// if method's name is empty return current active method
	if( !::SysStringLen( bstr_mtd_name ) )
		sptr_mtd_man->GetActiveMethodPos( &lpos_mtd );				
	else
	{
		sptr_mtd_man->GetFirstMethodPos( &lpos_mtd );
		while( lpos_mtd )
		{
			IUnknown  *punk_mtd = 0;
			long lpos_curr_mtd = lpos_mtd;
			if( S_OK != sptr_mtd_man->GetNextMethod( &lpos_mtd, &punk_mtd ) )
				break;

			IMethodPtr sptr_mtd = punk_mtd;		
			if( sptr_mtd != 0 )
			{
				_bstr_t bstrt_name;
				sptr_mtd->GetName( bstrt_name.GetAddress() );
				if( bstrt_name == _bstr_t(bstr_mtd_name) )
				{
					lpos_mtd = lpos_curr_mtd;
					/*if( ppunk_mtd ) 
					{
						*ppunk_mtd = punk_mtd;
						(*ppunk_mtd)->AddRef( );
					}

					if( plpos_mtd )
						*plpos_mtd = lpos_curr_mtd;*/

					break;
				}
			}

			if( punk_mtd )
				punk_mtd->Release(); punk_mtd = 0;
		}        		    
	}

	if( !lpos_mtd )
		return FALSE;

	if( plpos_mtd )
		*plpos_mtd = lpos_mtd;

	if( ppunk_mtd ) 
	{
		IUnknown  *punk_mtd = 0;
		sptr_mtd_man->GetNextMethod( &lpos_mtd, &punk_mtd );
		if( !punk_mtd )
			return FALSE;
		
		*ppunk_mtd = punk_mtd;
		punk_mtd = 0;
		// (*ppunk_mtd)->AddRef не вызываем, т.к. не вызываем punk_mtd->Release()
	}

	/*BOOL bret_val = FALSE;
	if( ppunk_mtd )
        bret_val = bret_val || ( (*ppunk_mtd) != 0 );

	if( plpos_mtd )	
		bret_val = bret_val || ( (*plpos_mtd) != 0 );

	return bret_val;								 */

	return TRUE;
}

HRESULT _invoke_func( IDispatch *pDisp, BSTR bstrFuncName, VARIANT* pargs, int nArgsCount, VARIANT* pvarResult )
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

_variant_t _exec_script_function( LPCTSTR lpctstrFunction, long nArgsCount, _variant_t *pvarArgs )
{
	if( !lpctstrFunction ) return _variant_t();

	IScriptSitePtr	sptrScriptSite = ::GetAppUnknown();
	if( sptrScriptSite == 0 ) return S_FALSE;

//	_variant_t args[] = {1,"zzz"};

	_variant_t varRes;
	HRESULT hRes = sptrScriptSite->Invoke( _bstr_t( lpctstrFunction ), pvarArgs, nArgsCount, &varRes, fwFormScript | fwAppScript );

	return varRes;
}

void FireScriptEvent(LPCTSTR lpszEventName, int nParams, VARIANT *pvarParams)
{
	IScriptSitePtr	sptrSS(::GetAppUnknown());
	if (sptrSS == 0) return;
	_bstr_t bstrEvent("MethodMan_");
	bstrEvent += lpszEventName;
	sptrSS->Invoke(bstrEvent, pvarParams, nParams, 0, fwAppScript);
}

DWORD	get_action_flags( BSTR bstr_name, DWORD dwDefault )
{
	if( !bstr_name )
		return dwDefault;

	CString str(bstr_name);
	if(str.FindOneOf("\r\n")>=0)
		return dwDefault; // если в имени есть CR или LF - не акция

	IUnknown *punkMan = 0;

	IApplicationPtr sptrApp = ::GetAppUnknown();
	sptrApp->GetActionManager( &punkMan );

	IActionManagerPtr sptrMan =	 punkMan;
	if( punkMan )
		punkMan->Release();	punkMan = 0;
	
	if( sptrMan == 0 )
		return dwDefault;
	
	DWORD dwFlag = dwDefault;
	sptrMan->GetActionFlags( bstr_name, &dwFlag );
	return dwFlag;
}

bool IsInteractiveAction(char *pszName)
{
	if( pszName[0] != _T('\n') && pszName[0] != _T('\r') )
	{
		// проверка на интерактивную акцию
		IApplicationPtr sptrApp = GetAppUnknown();
		IUnknown *punkActionMan = 0;
		sptrApp->GetActionManager( &punkActionMan );
		IActionManagerPtr sptrMan = punkActionMan;
		punkActionMan->Release(); punkActionMan = 0;

		IUnknownPtr ptrActionInfo;
		sptrMan->GetActionInfo( _bstr_t( pszName ), &ptrActionInfo );
		IActionInfoPtr sptrActionInfo = ptrActionInfo;

		if(sptrActionInfo)
		{
			IUnknownPtr	ptrAction;
			sptrActionInfo->ConstructAction( &ptrAction );
			IInteractiveActionPtr sptrInteractiveAction( ptrAction ); 
			if(sptrInteractiveAction) return true;
		}			
	}
	return false;
}

bool IsInteractiveRunning()
{
	IApplicationPtr sptrApp = ::GetAppUnknown();
	IUnknownPtr ptrActionMan = 0;
	if(sptrApp!=0) sptrApp->GetActionManager( &ptrActionMan );
	IActionManagerPtr sptrActionMan = ptrActionMan;
	IUnknownPtr ptr;
	if(sptrActionMan) sptrActionMan->GetRunningInteractiveAction(&ptr);
	return (ptr!=0);
}

_bstr_t GetActionGroup(char *pszName)
{
	if( pszName[0] != _T('\n') && pszName[0] != _T('\r') )
	{
		IApplicationPtr sptrApp = GetAppUnknown();
		IUnknown *punkActionMan = 0;
		sptrApp->GetActionManager( &punkActionMan );
		IActionManagerPtr sptrMan = punkActionMan;
		punkActionMan->Release(); punkActionMan = 0;

		IUnknownPtr ptrActionInfo;
		sptrMan->GetActionInfo( _bstr_t( pszName ), &ptrActionInfo );
		IActionInfoPtr sptrActionInfo = ptrActionInfo;

		if(sptrActionInfo)
		{
			_bstr_t bstrActionName, bstrActionUserName, bstrHelpString, bstrGroupName;
			sptrActionInfo->GetCommandInfo( bstrActionName.GetAddress(), bstrActionUserName.GetAddress(), bstrHelpString.GetAddress(), bstrGroupName.GetAddress() );
			return bstrGroupName;
		}
	}
	return "";
}

_map_t<int, const char*, cmp_string> _tracers_map; // карта трейсеров - со счетчиками
int _dbg_tracer::m_nDepth=0;
