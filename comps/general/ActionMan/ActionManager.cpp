// ActionManager.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ActionManager.h"
#include "easytokenizer.h"
#include "afxpriv.h"
#include "undoint.h"
#include "statusutils.h"
#include "ActionState.h"

extern CActionState	*g_pActionState;

//////////////////////////////////////////////////////////////////////
char* GetLogCurrentTime()
{
	static char sz_time[255];

	time_t ltime;
	time( &ltime );

	tm* today = localtime( &ltime );
	
	strftime( sz_time, sizeof(sz_time), _T("%d-%m-%y %H:%M:%S"), today );

	return sz_time;
}
//////////////////////////////////////////////////////////////////////
char* GetLogFileName()
{
	static char	sz_log_file[MAX_PATH];

	if( !strlen( sz_log_file ) )
	{
		::GetModuleFileName( 0, sz_log_file, sizeof(sz_log_file) );
		char* psz = strrchr( sz_log_file, '\\' );
		*psz = 0;
		strcat( sz_log_file, "\\action.log" );
	}

	return sz_log_file;
}

//////////////////////////////////////////////////////////////////////
void WriteActionLog( const char *psz_format, ... )
{	
	char* psz_log = GetLogFileName();

	va_list args;
	va_start( args, psz_format );

	char sz[2048];		sz[0]		= 0;
	char sz_pid[128];	sz_pid[0]	= 0;

	_vsnprintf( sz, sizeof(sz), psz_format, args );
	strcat( sz, _T("\r\n") );

	HANDLE	hFile = ::CreateFile( psz_log, GENERIC_WRITE, FILE_SHARE_READ, 0, OPEN_ALWAYS, 0, 0 );
	if( hFile != INVALID_HANDLE_VALUE )
	{
		::SetFilePointer( hFile, 0, 0, FILE_END );

		char sz_time[256];
		sprintf( sz_time, "%s\t", GetLogCurrentTime() );
		DWORD	dwSize = strlen( sz_time );
		::WriteFile( hFile, sz_time, dwSize, &dwSize, 0 );
		
		dwSize = strlen( sz );
		::WriteFile( hFile, sz, dwSize, &dwSize, 0 );

		::CloseHandle( hFile );	hFile = 0;
	}

	va_end(args);	
};


CActionManager	*CActionManager::s_pActionManager = 0;

IMPLEMENT_DYNCREATE(CActionManager, CCmdTargetEx)

CActionManager::CActionManager()
{
	m_bCacheEmpty = true;
	m_bLockInitInteractive = false;
	_OleLockApp( this );

	m_nInitRunningCount = 0;
	m_nRunningCount = 0;

	CActionManager::s_pActionManager = this;

	SetName( _T("ActionManager") );
	AttachComponentGroup( szPluginAction );

	m_bEnabled = TRUE;
	m_dwCurrentExecuteFlags = 0;
	m_bWriteToLogFlag = true;

	m_bold_enable			= true;
	m_bneed_set_enable_flag	= false;
	m_binside_interactive_loop = false;

}

CActionManager::~CActionManager()
{
	_OleUnlockApp( this );
}


void CActionManager::OnFinalRelease()
{
	_FlushCache();
 	CCmdTargetEx::OnFinalRelease();
}


BEGIN_INTERFACE_MAP(CActionManager, CCmdTargetEx)
	INTERFACE_PART(CActionManager, IID_IActionManager, ActionManager)
	INTERFACE_PART(CActionManager, IID_IActionManager2, ActionManager)
	INTERFACE_PART(CActionManager, IID_INamedObject2, Name)
	INTERFACE_PART(CActionManager, IID_ICompManager, CompMan)
	INTERFACE_PART(CActionManager, IID_ICompRegistrator, CompReg)
	INTERFACE_PART(CActionManager, IID_IRootedObject, Rooted)
	INTERFACE_PART(CActionManager, IID_IDispatch, Disp)
	INTERFACE_PART(CActionManager, IID_IMsgListener, MsgList)
	INTERFACE_PART(CActionManager, IID_IEventListener, EvList)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CActionManager, ActionManager);
IMPLEMENT_UNKNOWN(CActionManager, Disp);

GUARD_IMPLEMENT_OLECREATE(CActionManager, "ActionMan.ActionManager", 
0x810c7602, 0xf97a, 0x11d2, 0xa5, 0x96, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

DISPID CActionManager::GetDispID( const char *pszName )
{
	int	idx = _LookupCacheGetActionNo( pszName );
	//-1;
	//GetActionInfoByName( pszName, &idx );

	return idx+1;

}

DISPPARAMS	*CActionManager::PhraseArguments( const char *pszArgString )
{
	DISPPARAMS	*pdispParams = new DISPPARAMS;

	CEasyTokenizer	et( pszArgString );

	et.Tokenize();

	pdispParams->cArgs = et.GetVars().GetSize();
	pdispParams->cNamedArgs = 0;
	pdispParams->rgdispidNamedArgs = 0;
	pdispParams->rgvarg = 0;

	if( pdispParams->cArgs != 0 )
	{
		pdispParams->rgvarg = new VARIANTARG[pdispParams->cArgs];

		for( int i = 0; i < et.GetVars().GetSize(); i++ )
		{
			COleVariant	*p = (COleVariant *)et.GetVars()[i];

			::VariantInit( &pdispParams->rgvarg[i] );
			::VariantCopy( &pdispParams->rgvarg[i], (LPVARIANT)(*p) );
		}
	}

	return pdispParams;
}

HRESULT CActionManager::XActionManager::LockInitInteractive( BOOL bLock, BOOL *pbOldLock )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager);
	if( pbOldLock )*pbOldLock = pThis->m_bLockInitInteractive;
	pThis->m_bLockInitInteractive = bLock;
	return S_OK;
}

HRESULT CActionManager::XActionManager::GetActionFlags( BSTR bstrActionName, DWORD *pdwFlags )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager);
	{
		CString	sAction( bstrActionName );

		//paul 28.02.2003
		*pdwFlags = pThis->CheckActionState( sAction );

		if (pThis->m_bEnabled == FALSE)
		{
			if( *pdwFlags & afPrivileged )
				return S_OK;

			*pdwFlags = 0;
			return S_FALSE;
		}

		return S_OK;
	}
}


HRESULT CActionManager::XActionManager::SetScriptRunningFlag( BOOL bSet )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		if( bSet )
			pThis->m_dwCurrentExecuteFlags |= aefScriptRunning;
		else
			if( pThis->m_dwCurrentExecuteFlags & aefScriptRunning )
				pThis->m_dwCurrentExecuteFlags ^= aefScriptRunning;

		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::SetInteractiveRunningFlag( BOOL bSet )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		if( bSet )
			pThis->m_dwCurrentExecuteFlags |= aefInteractiveRunning;
		else
			if( pThis->m_dwCurrentExecuteFlags & aefInteractiveRunning )
				pThis->m_dwCurrentExecuteFlags ^= aefInteractiveRunning;

		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::IsScriptRunning()
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		if( pThis->m_dwCurrentExecuteFlags & aefScriptRunning )
			return S_OK;
		else
			return S_FALSE;
	}
}

HRESULT CActionManager::XActionManager::TerminateInteractiveAction()
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		pThis->TerminateInteractive();		
		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::FinalizeInteractiveAction()
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		pThis->TerminateInteractive(true);
		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::ExecuteAction( BSTR bstrActionName, BSTR bstrExecuteParams, DWORD dwExecuteFlags )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		if (pThis->m_bEnabled == FALSE)
			return S_FALSE;

		if( dwExecuteFlags & aefDelayed/* && pThis->m_nRunningCount > 0*/ )
		{
			_bstr_t	bstr = bstrActionName;
			pThis->AddToDelayList( bstr, _bstr_t(bstrExecuteParams),  dwExecuteFlags, ( dwExecuteFlags & aefClearDelayedList ) != 0 );
			return S_OK;
			
		}
		DWORD dwOldExecuteFlags = pThis->m_dwCurrentExecuteFlags; 

		pThis->m_dwCurrentExecuteFlags = dwExecuteFlags;
		pThis->m_dwCurrentExecuteFlags |= aefInternalExecute;

		_bstr_t	bstr = "ActionManager.";
		bstr += bstrActionName;
		bstr += " ";
		bstr += bstrExecuteParams;



		::ExecuteScript( bstr, _bstr_t( bstrActionName ), true );

		//if( pThis->m_dwCurrentExecuteFlags & aefInternalExecute )
		//	pThis->m_dwCurrentExecuteFlags ^= aefInternalExecute;
		pThis->m_dwCurrentExecuteFlags = dwOldExecuteFlags;

		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::GetCurrentExecuteFlags( DWORD *pdwFlags )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		*pdwFlags = pThis->m_dwCurrentExecuteFlags;
		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::EnableManager(BOOL bEnable)
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		BOOL bCurEnableState = pThis->m_bEnabled;
		pThis->m_bEnabled = bEnable == TRUE;
		if (bCurEnableState == TRUE)
			return S_OK;
		else
			return S_FALSE;
	}
}

HRESULT CActionManager::XActionManager::ExecuteDelayedActions()
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		pThis->ExecuteNext();
		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::InitDelayExecuting()
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		pThis->m_nInitRunningCount = pThis->m_nRunningCount;
		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::GetActionInfo( UINT nDispID, IUnknown **ppunkInfo )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		*ppunkInfo = pThis->GetComponent( nDispID-1 );
		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::AddCompleteActionToUndoList( IUnknown *punkAction )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		if( !CheckInterface( punkAction, IID_IUndoneableAction ) )
			return S_FALSE;

		pThis->StoreToUndo( punkAction );

		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::GetActionInfo(BSTR bstrActionName, IUnknown **ppunkInfo)
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		CString strName = bstrActionName;

		if (strName.IsEmpty() || !ppunkInfo)
			return E_INVALIDARG;

		*ppunkInfo = pThis->_LookupCacheGetAction(strName);
		if (*ppunkInfo)
			(*ppunkInfo)->AddRef();

		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::GetRunningInteractiveActionInfo(IUnknown **ppunkInfo)
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		if (!ppunkInfo)
			return E_INVALIDARG;

		*ppunkInfo = 0;
		*ppunkInfo = pThis->m_sptrCurrentActionInfo;
		if (*ppunkInfo)
			(*ppunkInfo)->AddRef();

		return S_OK;
	}
}


HRESULT CActionManager::XActionManager::GetRunningInteractiveAction(IUnknown **ppunkAction )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		if (!ppunkAction)
			return E_INVALIDARG;

		*ppunkAction = pThis->m_sptrCurrentAction;
		if (*ppunkAction)
			(*ppunkAction)->AddRef();

		return S_OK;
	}
}


HRESULT CActionManager::XActionManager::ChangeInteractiveAction(IUnknown *punkInfo)
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		// check this pointer is IActionInfo
		if (punkInfo && !CheckInterface(punkInfo, IID_IActionInfo))
			return E_INVALIDARG;

		// check this Action info is not current Action Info
		if (::GetObjectKey(punkInfo) == ::GetObjectKey(pThis->m_sptrCurrentActionInfo))
			return S_OK;

		pThis->m_sptrCurrentActionInfo = 0;
		pThis->m_sptrChangeActionInfo = punkInfo;

		return S_OK;
	}
}


//override default action flags. The main purpose is setup custom states for actions VBScript
HRESULT CActionManager::XActionManager::SetActionFlags( BSTR bstrActionName, DWORD dwState )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		CString	str( bstrActionName );
		pThis->m_mapOverridedStates[str]=dwState;

		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::ResetActionFlags( BSTR bstrActionName )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		CString	str( bstrActionName );
		pThis->m_mapOverridedStates.RemoveKey( str );
		return S_OK;
	}
}

HRESULT CActionManager::XActionManager::ResetAllActionsFlags()
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager)
	{
		pThis->m_mapOverridedStates.RemoveAll();
		return S_OK;
	}
}

CString CActionManager::_ObfuscateName(CString strName)
{
	CString strObfuscated;
	if( ! m_NameToObfuscated.Lookup(strName, strObfuscated) )
	{ // если еще не замучена - замучить и внести в мапы
		strObfuscated = "InternalAction";
		do
		{
			strObfuscated += char( rand() % 10 + '0' );
		}
		while( m_ObfuscatedToName.PLookup(strObfuscated) );
		m_NameToObfuscated[strName] = strObfuscated;
		m_ObfuscatedToName[strObfuscated] = strName;
	}
	return strObfuscated;
}

HRESULT CActionManager::XActionManager::ObfuscateName( BSTR* pbstrName )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager);
	if(!pbstrName) return E_INVALIDARG;

	CString strObfuscated = pThis->_ObfuscateName(*pbstrName);
	SysFreeString(*pbstrName);
	*pbstrName = strObfuscated.AllocSysString();
	return S_OK;
}

HRESULT CActionManager::XActionManager::DeobfuscateName( BSTR* pbstrName )
{
	METHOD_PROLOGUE_EX(CActionManager, ActionManager);
	if(!pbstrName) return E_INVALIDARG;
	CString strObfuscated = *pbstrName;
	CString strName = strObfuscated;
	if( ! pThis->m_ObfuscatedToName.Lookup(strObfuscated, strName) )
		strName = strObfuscated;
	*pbstrName = strName.AllocSysString();
	return S_OK;
}




void CActionManager::OnChangeParent()
{
	IUnknown *punk = GetParent();
	SetRootUnknown( punk );
	punk->Release();

	CCompManager::Init();

	sptrIApplication	spApp( ::GetAppUnknown() );
	IUnknown	*punkTargetMan = 0;
	spApp->GetTargetManager( &punkTargetMan );
	m_ptrTargetManager = punkTargetMan;
	punkTargetMan->Release();

	//cache all avaible actions
	//_FillCache();


	CEventListenerImpl::Register();
}

bool CActionManager::PreExecute( IUnknown *punkAction, DISPPARAMS *pDispParams )
{
//attach target to action
	IUnknown	*punk = 0;

	sptrIAction	sptrA( punkAction );
	sptrA->GetActionInfo( &punk );
	sptrIActionInfo	sptrAI( punk );

	if( !punk )
		return false;

	punk->Release();

	if(m_bEnabled == FALSE)
	{
//		if( !( dw & afPrivileged ) )
			return false;
	}

	sptrIApplication spApp( ::GetAppUnknown() );


	spApp->GetTargetManager( &punk );
	sptrIActionTargetManager	spTarget( punk );
	punk->Release();

	IUnknown *punkTarget = 0;
	BSTR	bstrTarget = 0;

	sptrAI->GetTargetName( &bstrTarget );
	spTarget->GetTarget( bstrTarget, &punkTarget );

	::SysFreeString( bstrTarget );

	if( !punkTarget )
		return false;
	sptrA->AttachTarget( punkTarget );
	punkTarget->Release();

/*
	move to "moving here" cos Filters init argument on GetActionState,
							before ActionMan set param to action
	DWORD	dw = 0;
	sptrA->GetActionState( &dw );
	if( (dw & afEnabled)==0 )
	{
		return false;
	}
	*/

//attach arguments to action
	int	iInArgCount = 0;
	int	iOutArgCount = 0;
	int	iParamsCounter = 0;

	sptrAI->GetArgsCount( &iInArgCount );
	sptrAI->GetOutArgsCount( &iOutArgCount );

	int	nArgCount = 0;

	if( pDispParams )
		nArgCount = pDispParams->cArgs;

	sptrIActionInfo2	sptrAI2( sptrAI );

	for( int i = 0; i < iInArgCount; i++ )
	{
		COleVariant var;
		BSTR	bstrName = 0, bstrDefVal = 0;

		if( sptrAI2 == 0 )
		{
			sptrAI->GetArgInfo( i, &bstrName, 0, &bstrDefVal );

			if( iParamsCounter < int( nArgCount ) )
				var = pDispParams->rgvarg[nArgCount-iParamsCounter-1];
			else
				var = bstrDefVal;
		}
		else
		{
			int nNumber = 0;
			sptrAI2->GetArgInfo2( i, &bstrName, 0, &bstrDefVal, &nNumber );

			if( nNumber < int( nArgCount ) )
				var = pDispParams->rgvarg[nArgCount-nNumber-1];
			else
				var = bstrDefVal;
		}


		sptrA->SetArgument( bstrName, var );

		iParamsCounter++;

		::SysFreeString( bstrName );
		::SysFreeString( bstrDefVal );

	}

	for( i = 0; i < iOutArgCount; i++ )
	{
		
		BSTR	bstrName = 0;

		if( sptrAI2 == 0 ) 
		{
			sptrAI->GetOutArgInfo( i, &bstrName, 0 );

			if( iParamsCounter < int( pDispParams->cArgs ) )
			{
				COleVariant var;
				var = pDispParams->rgvarg[pDispParams->cArgs-iParamsCounter-1];
				sptrA->SetArgument( bstrName, var );
			}
		}
		else
		{
			int nNumber = 0;
			sptrAI2->GetOutArgInfo2( i, &bstrName, 0, &nNumber );

			if( nNumber < int(nArgCount) )
			{
				COleVariant var;
				var = pDispParams->rgvarg[nArgCount-nNumber-1];
				sptrA->SetArgument( bstrName, var );
			}
		}
				

		iParamsCounter++;

		::SysFreeString( bstrName );
	}

	//moving here
	DWORD	dw = 0;
	sptrA->GetActionState( &dw );
	if( (dw & afEnabled)==0 )
	{
		return false;
	}

	if( CheckInterface( sptrA, IID_ICustomizableAction ) )
	{
		IApplicationPtr	ptrApp(GetAppUnknown() );
		HWND	hwnd;
		ptrApp->GetMainWindowHandle( &hwnd );
		sptrICustomizableAction	spCA( sptrA );

		if( spCA->CreateSettingWindow( hwnd ) != S_OK )
			return false;
	}

	if( CheckInterface( sptrA, IID_IInteractiveAction ) )
	{
		m_sptrCurrentTarget = punkTarget;
		//paul 24.04.2003
		//set focus to view
		{
			IWindowPtr ptr_wnd = m_sptrCurrentTarget;
			if( ptr_wnd )
			{
				HANDLE handle = 0;
				ptr_wnd->GetHandle( &handle );
				HWND hwnd = (HWND)handle;
				if( ::IsWindow( hwnd ) )
					::SetFocus( hwnd );
			}
		}
		return true;
	}
	
	return 	Execute( sptrA );
}


bool CActionManager::Execute( IUnknown *punkAction )
{
	if( !punkAction )
		return false;

	if(m_bEnabled == FALSE)
		return false;

	sptrIAction	sptrA = punkAction;
	IUnknown	*punkAI = 0;
	sptrA->GetActionInfo( &punkAI );
	sptrIActionInfo	sptrAI = punkAI;
	if( !punkAI )return false;
	punkAI->Release();

//write record to log file
	BSTR	bstrName = 0;
	sptrAI->GetCommandInfo( &bstrName, 0, 0, 0 );
	CString	strName  = bstrName;
	::SysFreeString( bstrName );

//store tick count for timing
	DWORD	dwTickCount = ::GetTickCount();

//time log
	CString	strTime;
	CString	strActionName;
	
	try
	{
		::FireEvent( GetAppUnknown(), szEventBeforeActionExecute, GetControllingUnknown(), sptrA, 0 );
	}
	catch( CException *pe )
	{
		pe->ReportError();
		pe->Delete();
	}
	
//undo support
	if( CheckInterface( sptrA, IID_IUndoneableAction ) )
		InitUndoInfo( sptrA );

	m_dwCurrentExecuteFlags |= aefRunning;
	m_nRunningCount++;

	bool bReturn = false;
	_try(CActionManager,Execute)
	{
		bReturn = sptrA->Invoke()==S_OK;
	}
	_catch
	{
		HWND	hwndMain;
		IApplication	*papp = GetAppUnknown();
		papp->GetMainWindowHandle( &hwndMain );
		::SendMessage( hwndMain, WM_NOTIFYLONGOPERATION, loaTerminate, (LPARAM)punkAction);
	};

	m_nRunningCount--;
	if( m_dwCurrentExecuteFlags & aefRunning )
		m_dwCurrentExecuteFlags ^= aefRunning;

	if( bReturn && CheckInterface( sptrA, IID_IUndoneableAction ) && !(m_dwCurrentExecuteFlags & aefNoUndo ) )
		StoreToUndo( sptrA );

	try
	{	
		dwTickCount = ::GetTickCount()-dwTickCount;
		::FireEvent( GetAppUnknown(), szEventAfterActionExecute, GetControllingUnknown(), punkAction, &dwTickCount, sizeof(dwTickCount) );
		
		g_pActionState->FireOnActionComplete( strName, bReturn?1:2 );

	}
	catch( CException *pe )
	{
		pe->ReportError();
		pe->Delete();
	}

	//IApplication	*papp = GetAppUnknown();
	//papp->IdleUpdate();
	return bReturn;
}

//if action "active" or "disabled"
DWORD	CActionManager::CheckActionState( const char *pszAction )
{
	DWORD	dwFlags = 0;
//get the action info
	//IUnknown *punk = GetActionInfoByName( pszAction );
	IUnknown *punk = _LookupCacheGetAction( pszAction );

	if( !punk )
		return dwFlags;

	sptrIActionInfo	sptrAI( punk );
//	punk->Release();
//if the action is current interactive action, check it and get it state
	if( m_sptrCurrentActionInfo != 0 && m_sptrCurrentAction != 0 )
	{	
		BSTR	bstr;			   

		m_sptrCurrentActionInfo->GetCommandInfo( &bstr, 0, 0, 0 );

		CString	str = bstr;
		::SysFreeString( bstr );

		if( str == pszAction )
		{
			sptrIAction sptrA( m_sptrCurrentAction );
			sptrA->GetActionState( &dwFlags );
			dwFlags |= afChecked;
			return dwFlags;
		}
	}
//get the target manager	

	DWORD	dwState;
	if( m_mapOverridedStates.Lookup( pszAction, dwState ) )
		return dwState;

//get the action target
	BSTR	bstrTarget = 0;
	IUnknown *punkTarget = 0;
	sptrAI->GetTargetName( &bstrTarget );
	m_ptrTargetManager->GetTarget( bstrTarget, &punkTarget );
	::SysFreeString( bstrTarget );

//if no avaible targets, action is disabled
	if( !punkTarget )
		return dwFlags;

	IUnknownPtr	sptrTarget( punkTarget );
	punkTarget->Release();

//create an action object and check it state
	IUnknown *punkA = 0;
	sptrAI->ConstructAction( &punkA );

	if( !punkA )
		return dwFlags;

	sptrIAction	sptrA( punkA );
	punkA->Release();


	sptrA->AttachTarget( sptrTarget );
	sptrA->GetActionState( &dwFlags );

	return dwFlags;
}

//!!!todo : add map string to ptr for better performance
IUnknown *CActionManager::GetActionInfoByName( const char *szName, int *pidx )
{
	for( int i = 0; i < GetComponentCount(); i++ )
	{
		IUnknown *punk  = GetComponent( i );
		sptrIActionInfo	sp( punk );
		punk->Release();

		BSTR	bstr;
		sp->GetCommandInfo( &bstr, 0, 0, 0 );

		CString	sAction = bstr;
		::SysFreeString( bstr );

		if( sAction == szName )
		{
			if( pidx )*pidx = i;
			return punk;
		}
	}

	if( pidx )*pidx = -1;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
//IDispatch implementation. Handy made!!!
HRESULT CActionManager::XDisp::GetTypeInfoCount( 
            /* [out] */ UINT *pctinfo)
{
	_try_nested(CActionManager, Disp, GetTypeInfoCount)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}
        
HRESULT CActionManager::XDisp::GetTypeInfo( 
    /* [in] */ UINT iTInfo,
    /* [in] */ LCID lcid,
    /* [out] */ ITypeInfo **ppTInfo)
{
	_try_nested(CActionManager, Disp, GetTypeInfo)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CActionManager::XDisp::GetIDsOfNames( 
    /* [in] */ REFIID riid,
    /* [size_is][in] */ LPOLESTR *rgszNames,
    /* [in] */ UINT cNames,
    /* [in] */ LCID lcid,
    /* [size_is][out] */ DISPID *rgDispId)
{
	_try_nested(CActionManager, Disp, GetIDsOfNames)
	{															
		//we don't support locale language
		lcid;
		//we don't know abour riid 
		riid;

		HRESULT	hRes = S_OK;

		for( int i = 0; i < (int)cNames; i++ )
		{
			CString	strRequestName = rgszNames[i];

			//int	idx = 0;
			
			
			IUnknown *punk = pThis->_LookupCacheGetAction( strRequestName );
			int	idx = pThis->_LookupCacheGetActionNo( strRequestName );

			//if( !punk )
			//	punk = pThis->GetActionInfoByName( strRequestName, &idx );
			

			if( !punk )
			{
				hRes = DISP_E_UNKNOWNNAME;
				rgDispId[i] = (DISPID)-1;
				continue;
			}

			rgDispId[i] = idx+1;
		}

		return hRes;
	}
	_catch_nested;
}

HRESULT CActionManager::XDisp::Invoke( 
    /* [in] */ DISPID dispIdMember,
    /* [in] */ REFIID riid,
    /* [in] */ LCID lcid,
    /* [in] */ WORD wFlags,
    /* [out][in] */ DISPPARAMS *pDispParams,
    /* [out] */ VARIANT *pVarResult,
    /* [out] */ EXCEPINFO *pExcepInfo,
    /* [out] */ UINT *puArgErr)
{
	_try_nested(CActionManager, Disp, Invoke)
	{

		riid;
		lcid;

		GetLogFile()<<"wFlags=";
		GetLogFile()<<wFlags;
		GetLogFile()<<"\n";

		/*if( wFlags != DISPATCH_METHOD )
		{
			return DISP_E_MEMBERNOTFOUND;
		}*/

		bool	bFromScript = (pThis->m_dwCurrentExecuteFlags & aefScriptRunning) != 0;
		
		long	lResult = pThis->ExecuteActionCommand( dispIdMember, pDispParams, bFromScript );
		
		pThis->m_ptr_running_action = 0;

		/*if( !pThis->ExecuteActionCommand( dispIdMember, pDispParams, bFromScript ) )
			return DISP_E_PARAMNOTFOUND;*/

		if( pVarResult )
			*pVarResult = COleVariant( (long)lResult ).Detach();

		return S_OK;
	}
	_catch_nested;
}

void CActionManager::OnAddComponent( int idx )
{
	IUnknown	*punk = GetComponent( idx, false );

	IUnknown	*punkCommandManager = 0;
	sptrIApplication sptrA( ::GetAppUnknown() );
	sptrA->GetCommandManager( &punkCommandManager );

	if( !punkCommandManager )return;

	sptrICommandManager	sptrCM( punkCommandManager );

	punkCommandManager->Release();

	_CacheAction( punk, idx );

	sptrCM->AddAction( punk );
}

void CActionManager::OnRemoveComponent( int idx )
{
	IUnknown	*punk = GetComponent( idx, false );

	IUnknown	*punkCommandManager = 0;
	sptrIApplication sptrA( GetAppUnknown() );
	sptrA->GetCommandManager( &punkCommandManager );

	if( !punkCommandManager )return;

	sptrICommandManager	sptrCM( punkCommandManager );

	punkCommandManager->Release();

	sptrCM->RemoveAction( punk );

	_UncacheAction( punk );
}


///////interactive actions support
BOOL CActionManager::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	//if interactive action is executing now
	if( m_sptrCurrentAction != 0 )	//and it is complete
	{
		HRESULT	hr = m_sptrCurrentAction->IsComplete();
		if( hr != S_FALSE )
		{
			if( hr == S_OK )
				FinalizeInteractive();	//terminate it!
//[ay] - проблема с print manager в карио
/*			if( m_dwCurrentExecuteFlags & aefScriptRunning )
			{
				m_sptrCurrentActionInfo = 0;
				return FALSE;
			}*/

			if (m_sptrChangeActionInfo != 0)
			{
				m_sptrCurrentAction = 0;
				m_sptrCurrentActionInfo = m_sptrChangeActionInfo;
				m_sptrChangeActionInfo = 0;
			}

			//проблема выхода из интерактивной акции по ESC - принудитльно обновляем толбар
			IApplication	*pApp = GetAppUnknown();

			if( m_sptrCurrentActionInfo == 0 )
			{
				CString str_action_name;
				if( m_sptrCurrentAction != 0 )
				{
					IActionPtr	ptrA( m_sptrCurrentAction );
					IUnknown	*punk = 0;
					ptrA->GetActionInfo( &punk );
					IActionInfoPtr	ptrAI( punk );
					punk->Release();

					BSTR	bstrActionName;
					ptrAI->GetCommandInfo( &bstrActionName, 0, 0, 0 );
					str_action_name = bstrActionName;
					::SysFreeString( bstrActionName );					
				}

				m_sptrCurrentAction = 0;
				m_sptrCurrentTarget = 0;

				if( str_action_name.GetLength() )
					g_pActionState->FireOnActionComplete( str_action_name, false );

				pApp->IdleUpdate();

				//paul 30.04.2003
				SetCursor( ::LoadCursor( 0, IDC_ARROW ) );

				return FALSE;
			}

			if( hr == S_OK )
				InitInteractive();		//and init next
			else
				TerminateInteractive();

			pApp->IdleUpdate();

		}
	}

	return FALSE;
}

bool CActionManager::ExecuteActionCommand( DISPID dispIdMember, DISPPARAMS *pDispParams, bool bFromScript )
{
	_try(CActionManager,ExecuteActionCommand)
	{
	m_bWriteToLogFlag = !bFromScript;

	if( dispIdMember <= 0 ||dispIdMember > GetComponentCount() )
	//index out of bonds
		return false;

//get action info and create action
	GetLogFile()<<"Getting ActionInfo\n";

	IUnknown	*punk = GetComponent( dispIdMember-1 );
	sptrIActionInfo	sptrAI( punk );
	punk->Release();

	

//if current interactive action executed
	bool	bTerminateInteractive = false;
	if( !bFromScript && GetObjectKey( m_sptrCurrentActionInfo ) ==
		GetObjectKey( sptrAI ) )
		bTerminateInteractive = true;

	DWORD dw_tick_count = ::GetTickCount();

//create an action
	GetLogFile()<<"Constructing action\n";

	punk = 0;
	if( sptrAI->ConstructAction( &punk ) )
	//can't create action object
		return false;

	sptrIAction	sptrA( punk );
	punk->Release();

	m_ptr_running_action = sptrA;

	CString	str_action_name;
	//paul 28.01.2003 Fire event, then action is about run
	{
		BSTR	bstrName = 0;
		sptrAI->GetCommandInfo( &bstrName, 0, 0, 0 );		
		str_action_name = bstrName;		
		::SysFreeString( bstrName );
	}

	GetLogFile()<<"Checking interactive action\n";
//if action is interactive
	if( CheckInterface( sptrA, IID_IInteractiveAction ) )
	{
		TerminateInteractive();
		if( bTerminateInteractive && !bFromScript )
			return true;
		m_sptrCurrentActionInfo = sptrAI;
		m_sptrCurrentAction = sptrA;

		if( !PreExecute( sptrA, pDispParams ) )
		{
			m_sptrCurrentActionInfo = 0;
			m_sptrCurrentAction = 0;

			return false;
		}

		if( !InitInteractive() )
			return false;

		bool	fResult = false;

		if( bFromScript )
		{
			//paul 27.09.2002
			//to avoid dead lock
			m_bold_enable			= m_bEnabled;
			m_bneed_set_enable_flag	= true;


			BOOL	bOldEnabled = m_bEnabled;
			m_bEnabled = false;

			//here - alternative message loop (comming soon)
			IApplicationPtr	ptrApp( GetAppUnknown() );

			ptrApp->IdleUpdate();

			m_binside_interactive_loop = true;

			BOOL	bOld = false;
			ptrApp->SetNestedMessageLoop( true, &bOld );

			fResult = false;
			for( ;; )
			{
				ptrApp->ProcessMessage();
				if( m_sptrCurrentAction==0 )
				{
					break;
				}
				
				HRESULT	hr =m_sptrCurrentAction->IsComplete();
				if( hr != S_FALSE )
				{
					if( hr == S_OK )
					{
						fResult = true;
						m_bEnabled = bOldEnabled;
						FinalizeInteractive();
					}
					
					//[vanek] BT2000:3573 - 14.01.2004
					SetCursor( ::LoadCursor( 0, IDC_ARROW ) );
					break;
				}
			}
			m_binside_interactive_loop = false;

			TerminateInteractive();
			ptrApp->SetNestedMessageLoop( bOld, 0 );
		
			m_bEnabled = bOldEnabled;
			
			m_bneed_set_enable_flag = false;
		}
		else
			fResult = true;
		return fResult;
	}
	
	//if action is not interactive	
	g_pActionState->FireOnActionRunning( str_action_name );

	static int g_nlevel = 0;

	g_nlevel++;

	static long lenable_time_log = -1;
	if( lenable_time_log == -1 )
	{
		lenable_time_log = ::GetValueInt( ::GetAppUnknown(), "\\Logging", "FunctionTiming", 0L );
		::DeleteFile( GetLogFileName() );
	}
	if( lenable_time_log == 1 )
	{
		CString str_spaces = "";
		for(int i=0;i<g_nlevel;i++ )
			str_spaces += " ";
		WriteActionLog( "%s%s", (const char*)str_spaces, (const char*)str_action_name );
	}
	
	//[Max] Bt: 3232
	bool bOldState = (m_bEnabled == TRUE);
//	if( bFromScript)
	{
		DWORD dw = CheckActionState( str_action_name );

		if( !m_bEnabled )
		{
			if( dw & afPrivileged )
				m_bEnabled = true;
		}
	}

	GetLogFile()<<"Preexecuting action\n";
	bool bres = PreExecute( sptrA, pDispParams );
	m_bEnabled = bOldState;
	
	if( lenable_time_log == 1 )
	{
		BSTR	bstrParams = 0;
		sptrA->StoreParameters( &bstrParams );
		CString	str_params = bstrParams;
		::SysFreeString( bstrParams );

		//store tick count for timing
		dw_tick_count = ::GetTickCount() - dw_tick_count;
		CString	strCommandLine = str_action_name + " " + str_params;

		CString str_spaces = "";
		for(int i=0;i<g_nlevel;i++ )
			str_spaces += " ";

		WriteActionLog( "%s%dms %s", (const char*)str_spaces, dw_tick_count, (const char*)strCommandLine );
	}
	g_nlevel--;

	return bres;
	}
	_catch;
	return false;
}

void CActionManager::TerminateInteractive(bool bFinalize)
{
	CString str_action_name = "";

	if (bFinalize && m_bneed_set_enable_flag )
		// m_bEnabled now false and Execute wiil no be called in FinalizeInteractive
		m_bEnabled = m_bold_enable; 

	if( m_sptrCurrentAction != 0 &&
		m_sptrCurrentActionInfo != 0 )
	{
		DWORD	dwState = 0;

		sptrIAction	sptrA( m_sptrCurrentAction );
		sptrA->GetActionState( &dwState );

		m_sptrCurrentAction->TerminateInteractive();
		if( dwState & afRequiredInvokeOnTerminate )
		{
			str_action_name = "";
			FinalizeInteractive();
		}

		
		// [Max] GPF on shell exit if have current running interactive action
		// After FinalizeInteractive() variable m_sptrCurrentActionInfo is NULL

		if( m_sptrCurrentActionInfo != 0 ) 
		{
			BSTR	bstrActionName;
			m_sptrCurrentActionInfo->GetCommandInfo( &bstrActionName, 0, 0, 0 );
			str_action_name = bstrActionName;
			::SysFreeString( bstrActionName );			
		}
	}
	// A.M. fix, BT 3618
	else if( m_sptrCurrentAction != 0 && m_sptrCurrentActionInfo == 0 )
	{
		IActionPtr	ptrA( m_sptrCurrentAction );
		IUnknownPtr punk;
		ptrA->GetActionInfo(&punk);
		IActionInfoPtr	ptrAI(punk);
		BSTR	bstrActionName;
		ptrAI->GetCommandInfo( &bstrActionName, 0, 0, 0 );
		str_action_name = bstrActionName;
		::SysFreeString( bstrActionName );					
	}

	m_sptrCurrentActionInfo = 0;
	m_sptrCurrentAction = 0;
	m_sptrCurrentTarget = 0;

	if( str_action_name.GetLength() )
		g_pActionState->FireOnActionComplete( str_action_name, false );

	_SetStatusCurrentAction();

	if( m_bneed_set_enable_flag )
		m_bEnabled = m_bold_enable;

	//23.04.2003 BT 3076
	{
		IApplication	*papp = GetAppUnknown();
		papp->IdleUpdate();
	}

}


bool CActionManager::InitInteractive()
{
//	ASSERT( m_sptrCurrentTarget == 0 );
	ASSERT( m_sptrCurrentActionInfo != 0 );

	if( m_sptrCurrentActionInfo == 0 )
		return false;

	if( m_sptrCurrentAction == 0 )
	{
		IUnknown	*punkA = 0;
		if( m_sptrCurrentActionInfo->ConstructAction( &punkA ) != S_OK )
		//can't create action object
			return false;

		m_sptrCurrentAction = punkA;
		punkA->Release();

		if( !PreExecute( m_sptrCurrentAction ) )
			return false;
	}

	if( !CheckInterface( m_sptrCurrentAction, IID_IInteractiveAction ) )
	{
		AfxMessageBox( IDS_NO_INTERACTIVE_INTERAFCE );
		return false;
	}
	if( !CheckInterface( m_sptrCurrentAction, IID_IMsgListener ) )
	{
		AfxMessageBox( IDS_NO_MSG_LISTENER );
		return false;
	}
	/*if( !CheckInterface( m_sptrCurrentTarget, IID_IWindow2 ) )
	{
		AfxMessageBox( IDS_NO_WINDOW_TARGET );
		return false;
	}						  */
//attach event listener to the target window
	//protect from invalid invoke
	IUnknownPtr	ptrActionKeeper = m_sptrCurrentAction;	

	if( m_sptrCurrentAction->BeginInteractiveMode() != S_OK )
	{
		m_sptrCurrentAction = 0;
		m_sptrCurrentTarget = 0;
		m_sptrCurrentActionInfo = 0;
	}

	//paul 7.08.2003
	{
		CString	str_action_name;	
		if( m_sptrCurrentActionInfo )
		{
			BSTR	bstrName = 0;
			m_sptrCurrentActionInfo->GetCommandInfo( &bstrName, 0, 0, 0 );		
			str_action_name = bstrName;		
			::SysFreeString( bstrName );
		}
		g_pActionState->FireOnActionRunning( str_action_name );
	}


	if( m_sptrCurrentAction != 0 )
	{
		HRESULT	hr = m_sptrCurrentAction->IsComplete();
		if( hr != S_FALSE )
		{
			if( hr == S_OK )
				FinalizeInteractive();
			m_sptrCurrentActionInfo = 0;
		}
	}

	_SetStatusCurrentAction();


	return true;
}

bool CActionManager::FinalizeInteractive()
{
	ASSERT( m_sptrCurrentTarget!=0 );
	ASSERT( m_sptrCurrentAction!=0 );

	Execute( m_sptrCurrentAction );

	m_sptrCurrentAction = 0;
	m_sptrCurrentTarget = 0;
	_SetStatusCurrentAction();

	return true;
}

void CActionManager::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( pszEvent, szActiveTargetChanged ) )
	{
 		if( m_sptrCurrentAction != 0 &&
			m_sptrCurrentActionInfo != 0 )
		{
			BSTR	bstrTargetName = 0;
			m_sptrCurrentActionInfo->GetTargetName( &bstrTargetName );

			_bstr_t	bstrCurrentTarget = bstrTargetName;
			::SysFreeString( bstrTargetName );

			IUnknown	*punkNewTarget = 0;
			m_ptrTargetManager->GetTarget( bstrCurrentTarget, &punkNewTarget );
			IUnknownPtr	ptrNewTarget( punkNewTarget, false );

			IActionPtr	ptrA = m_sptrCurrentAction;
			IUnknown	*punkOldTarget = 0;
			ptrA->GetTarget( &punkOldTarget );
			IUnknownPtr	ptrOldTarget( punkOldTarget, false );

			if( GetObjectKey( ptrNewTarget ) == ::GetObjectKey( ptrOldTarget ) )
				return;

			/*_bstr_t	bstrActivatedTarget = var;

			if( bstrActivatedTarget != bstrCurrentTarget )
				return;*/

			DWORD	dwState = 0;
			sptrIAction	sptrA( m_sptrCurrentAction );
			sptrA->GetActionState( &dwState );

			if( !m_bLockInitInteractive )
				m_sptrCurrentAction->TerminateInteractive();
			if( dwState & afRequiredInvokeOnTerminate )
			{
				FinalizeInteractive();
/*				InitInteractive();
				return;*/
			}
			if( dwState & afLeaveModeOnChangeTarget )
			{
				m_sptrCurrentActionInfo = 0;
				m_sptrCurrentAction = 0;
				m_sptrCurrentTarget = 0;
				_SetStatusCurrentAction();

				return;
			}
			

			sptrA->AttachTarget( ptrNewTarget );
			sptrA->GetActionState( &dwState );

			if( ( dwState & afEnabled ) == 0 || ( dwState & afTargetCompatibility ) == 0 )
			{
				m_sptrCurrentActionInfo = 0;
				m_sptrCurrentAction = 0;
				m_sptrCurrentTarget = 0;
				_SetStatusCurrentAction();

				return;
			}
			m_sptrCurrentTarget = ptrNewTarget;
			if( !m_bLockInitInteractive && m_sptrCurrentAction != 0 )
				m_sptrCurrentAction->BeginInteractiveMode();
		}
	}
}

bool CActionManager::StoreToUndo( IUnknown *punkAction )		//store the action to the UNDO list
{
	//new version
	IUndoneableActionPtr	ptrUA( punkAction );
	if( ptrUA == 0 )return false;

	GuidKey	key;
	ptrUA->GetTargetKey( &key );

	if( key == INVALID_KEY )
		return false;


	IUnknown	*punkTarget = 0;

	m_ptrTargetManager->GetTargetByKey( key, &punkTarget );

	if( !punkTarget )
		return false;

	IUndoListPtr	ptrUndoList( punkTarget );
	punkTarget->Release();

	if( ptrUndoList != 0 )
		return ptrUndoList->AddAction( punkAction ) == S_OK;

	return false;
}
												

//init undo info
bool CActionManager::InitUndoInfo( IUnknown *punkAction )		
{
	sptrIUndoneableAction	sptrUA( punkAction );
	return sptrUA->StoreState() == S_OK;
}


void CActionManager::_FillCache()
{
	/*
	FILE *fl = fopen( ::MakeAppPathName( "fn.log" ) , "rt" );
	if( fl )
	{
		fclose( fl );
		::DeleteFile( ::MakeAppPathName( "fn.log" ) );
	}
	*/

	if( !m_bCacheEmpty )
		return;
	
	for( int i = 0; i < GetComponentCount(); i++  )
		_CacheAction( GetComponent( i, false ), i );
	m_bCacheEmpty = false;
}

void CActionManager::_FlushCache()
{
	m_bCacheEmpty = true;
	
	POSITION pos = m_cacheByName.GetStartPosition();

	while( pos )
	{
		void	*pval;
		CString	str;
		m_cacheByName.GetNextAssoc( pos, str, pval );

		CacheAIRecord	*prec =(CacheAIRecord	*)pval;

		delete prec;
	}
	
	m_cacheByName.RemoveAll();
}

void WriteToLog( CString str )
{
	FILE *fl = fopen( ::MakeAppPathName( "fn.log" ), "at" );
	
 	if( fl )
		fputs( str + "\n", fl );

	fclose( fl );
}

void CActionManager::_CacheAction( IUnknown *punkAI, int idx )
{
	sptrIActionInfo sptrI( punkAI );
	if( sptrI == 0 )return;

	BSTR	bstrName = 0;
	sptrI->GetCommandInfo( &bstrName, 0, 0, 0 );

	CString	strName = bstrName;
	::SysFreeString( bstrName );
	
	//WriteToLog( strName );

	CacheAIRecord	*prec = new CacheAIRecord;
	prec->punkAI = punkAI;
	prec->idx = idx;
	prec->strName = strName;


	void *pold = m_cacheByName[strName];
	if( pold )
		delete (CacheAIRecord	*)pold;

	m_cacheByName[strName] = prec;
	//m_cacheByAction[punkAI] = prec;

	{ // присваивание акциям парных имен для обфускатора
		CString strObfuscated = _ObfuscateName(strName);

		CacheAIRecord	*prec = new CacheAIRecord;
		prec->punkAI = punkAI;
		prec->idx = idx;
		prec->strName = strObfuscated;

		void *pold = m_cacheByName[strObfuscated];
		if( pold )
			delete (CacheAIRecord	*)pold;

		m_cacheByName[strObfuscated] = prec;

		m_NameToObfuscated[strName] = strObfuscated;
		m_ObfuscatedToName[strObfuscated] = strName;
	}

}
void CActionManager::_UncacheAction( IUnknown *punkAI )
{
	_FlushCache();
	/*POSITION pos = m_cacheByName.GetStartPosition();

	while( pos )
	{
		void	*pval;
		CString	str;
		m_cacheByName.GetNextAssoc( pos, str, pval );

		CacheAIRecord	*prec =(CacheAIRecord	*)pval;

		if( prec && prec->punkAI == punkAI )
		{
			delete prec;
			m_cacheByName.RemoveKey( str );

			break;
		}
	} */

/*	bool	bDecNo = false;

	for( int i = 0; i < GetComponentCount(); i++ )
	{
		IUnknown *punk  = GetComponent( i );
		sptrIActionInfo	sp( punk );
		punk->Release();

		BSTR	bstr;
		sp->GetCommandInfo( &bstr, 0, 0, 0 );

		CString	sAction = bstr;
		::SysFreeString( bstr );


		if( punk == punkAI )
		{
			bDecNo = true;
			continue;
		}


		if( bDecNo )
		{
			CacheAIRecord	*prec =(CacheAIRecord	*)m_cacheByName[sAction];
			if( prec )
				prec->idx--;
		}
	}*/
}

IUnknown *CActionManager::_LookupCacheGetAction( const char *szActionName )
{
	if( m_bCacheEmpty )
		_FillCache();

	CacheAIRecord	*prec =(CacheAIRecord	*)m_cacheByName[szActionName];

	if( !prec )
		return 0;

	return prec->punkAI;
}

int CActionManager::_LookupCacheGetActionNo( const char *szActionName )
{
	CacheAIRecord	*prec =(CacheAIRecord	*)m_cacheByName[szActionName];
	if( !prec )
		return -1;

	return prec->idx;
}

void CActionManager::ExecuteNext()
{
	if( m_nRunningCount > m_nInitRunningCount )
		return;
	if( m_listDelayActions.GetCount() == 0 )
		return;
	CString	strAction = m_listDelayActions.GetHead();
  CString	strActionParams = m_listDelayActionsParams.GetHead();
	bstr_t bstr = strAction;
	DWORD	dwFlags = m_listDelayExecFlags.GetHead();

	m_listDelayActions.RemoveHead();
  m_listDelayActionsParams.RemoveHead();
	m_listDelayExecFlags.RemoveHead();

	m_xActionManager.ExecuteAction( bstr, _bstr_t(strActionParams), dwFlags );
}


void CActionManager::AddToDelayList( const char *szAction, const char *szActionParams, DWORD dwFlags, bool bClearList )
{
	if( bClearList )
	{
		m_listDelayActions.RemoveAll();
    m_listDelayActionsParams.RemoveAll();
		m_listDelayExecFlags.RemoveAll();
	}

	dwFlags ^= aefDelayed;

	m_listDelayActions.AddTail( szAction );
  m_listDelayActionsParams.AddTail( szActionParams );
	m_listDelayExecFlags.AddTail( dwFlags );
}
	

void CActionManager::_SetStatusCurrentAction()
{

	HWND	hwndMain;
	IApplication	*papp = GetAppUnknown();
	papp->GetMainWindowHandle( &hwndMain );

	if( m_sptrCurrentActionInfo == 0 )
	{
		::StatusSetPaneText( guidPaneAction, 0 );
		::StatusSetPaneIcon( guidPaneAction, 0 );
		::SendMessage( hwndMain, WM_SETMESSAGESTRING, 57345, 0 );
	}
	else
	{
		BSTR	bstrCommand = 0;
		m_sptrCurrentActionInfo->GetCommandInfo( 0, &bstrCommand,  0, 0 );
		CString	str = bstrCommand;
		::SysFreeString( bstrCommand );

		//paul 27.04.2002, tech ask
		//::StatusSetPaneText( guidPaneAction, str );

		int	id;
		m_sptrCurrentActionInfo->GetLocalID( &id );
		::SendMessage( hwndMain, WM_SETMESSAGESTRING, id, 0 );
	}
	
}