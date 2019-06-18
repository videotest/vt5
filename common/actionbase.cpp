#include "stdafx.h"
#include "actionbase.h"
#include "argbase.h"
#include "nameconsts.h"
#include "resource.h"
#include "docviewbase.h"
#include "misc5.h"
#include "PropPage.h"
#include "ScriptNotifyInt.h"

#include "setupInt.h"



BOOL CALLBACK EnumResNameProc(  HANDLE hModule,   // module handle
				  LPCTSTR lpszType, // pointer to resource type
				  LPTSTR lpszName,  // pointer to resource name
				  LONG lParam )
{
	CDWordArray	*pa = (CDWordArray	*)lParam;
	pa->Add( (DWORD)lpszName );

	return TRUE;
}

BEGIN_INTERFACE_MAP(CActionBase, CCmdTargetEx)
	INTERFACE_PART(CActionBase, IID_IAction, Action)
	INTERFACE_PART(CActionBase, IID_INumeredObject, Num)
	INTERFACE_PART(CActionBase, IID_IRootedObject, Rooted)
	INTERFACE_PART(CActionBase, IID_IActionArgumentHelper, Args)
	INTERFACE_AGGREGATE(CActionBase, m_punkDlgHelper)
	INTERFACE_AGGREGATE(CActionBase, m_punkUndoHelper)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CActionBase, Action);
IMPLEMENT_UNKNOWN(CActionBase, Args);

CBitmapProvider *CActionInfoBase::s_pBmps = 0;

CActionBase::CActionBase()
{
	LOCAL_RESOURCE;

	m_pActionInfo = 0;
	m_punkTarget = 0;
	m_punkDlgHelper = 0;
	m_punkUndoHelper = 0;
	m_dwExecuteSettings = 0;

	//m_strErrorString.LoadString( IDS_ERR_OK );

	EnableAggregation();
}

CActionBase::~CActionBase()
{
	if( m_punkTarget )
		m_punkTarget->Release();
	if( m_pActionInfo )
		m_pActionInfo->Release();
	if( m_punkDlgHelper )
		m_punkDlgHelper->Release();
	if( m_punkUndoHelper )
		m_punkUndoHelper->Release();
}

bool CActionBase::PrepareUndoRedo()
{
	//get the target manager
	IUnknown *punkTargetMan = 0;
	sptrIApplication spApp( ::GetAppUnknown() );
	spApp->GetTargetManager( &punkTargetMan );

	if( !punkTargetMan )
		return false;

	sptrIActionTargetManager	sptrM( punkTargetMan );
	punkTargetMan->Release();

	if( m_punkTarget )
		m_punkTarget->Release();
	m_punkTarget = 0;

	sptrM->GetTargetByKey( m_lTargetKey, &m_punkTarget );

	if( !m_punkTarget )
		return false;
	return true;

}

bool CActionBase::UnPrepareUndoRedo()
{
	if( m_punkTarget )
		m_punkTarget->Release();
	m_punkTarget = 0;

	return true;
}

void CActionBase::Init()
{
	if( HasSettings() )
	{
		CActionDlgHelper *p = new CActionDlgHelper( this );
		p->m_pOuterUnknown = GetControllingUnknown();
		m_punkDlgHelper = (IUnknown*)&p->m_xInnerUnknown;
	}

	if( HasUndo() )
	{
		CActionUndoHelper *p = new CActionUndoHelper( this );
		p->m_pOuterUnknown = GetControllingUnknown();
		m_punkUndoHelper = (IUnknown*)&p->m_xInnerUnknown;
	}

	IActionManager *pam = GetActionManager();

	ASSERT( pam );

	if( pam )
	{
		pam->GetCurrentExecuteFlags( &m_dwExecuteSettings );
		pam->Release();
	}
}

void CActionBase::SetModified( bool bSet )
{
	if( bSet )
		m_modified.SetModified( m_punkTarget );
	else
		m_modified.ResetModified( m_punkTarget );
}

void CActionBase::MakeModified()
{
	//no undo - no helper
	if( !m_punkUndoHelper )
		return;

	IUnknown *punkTargetMan = 0;
	sptrIApplication spApp( ::GetAppUnknown() );
	spApp->GetTargetManager( &punkTargetMan );

	if( !punkTargetMan )
		return;

	sptrIActionTargetManager	sptrM( punkTargetMan );
	punkTargetMan->Release();

	IUnknown	*punkTarget = 0;
	sptrM->GetTargetByKey( m_lTargetKey, &punkTarget );

	if( !punkTarget )
		return;

	m_modified.SetModified( punkTarget );

	punkTarget->Release();
}


bool CActionBase::ForceShowDialog()
{
	return ( m_dwExecuteSettings & 0x03 ) == aefShowInterfaceAnyway;
}

bool CActionBase::ForceNotShowDialog()
{
	return ( m_dwExecuteSettings & 0x03 ) == aefNoShowInterfaceAnyway;
}

//!!!todo : image and other large information blocks should'n been copied
HRESULT CActionBase::XArgs::SetArgumentHelper( BSTR bstrArgName, VARIANT *pvarArg )
{
	METHOD_PROLOGUE_EX(CActionBase, Args)
	{
		CString	strArgName( bstrArgName );
		COleVariant var( *pvarArg );

		pThis->m_args.SetAt( strArgName, var );
		return S_OK;
	}
}

HRESULT CActionBase::XArgs::GetResultHelper( BSTR bstrResName, VARIANT *pvarResult )
{
	METHOD_PROLOGUE_EX(CActionBase, Args)
	{
		COleVariant	var;
		CString	strArgName( bstrResName );

		pThis->m_args.Lookup( strArgName, var );

		*pvarResult = var.Detach();

		return S_OK;
	}
}

HRESULT CActionBase::XAction::GetTarget( IUnknown **ppunkTarget )
{
	METHOD_PROLOGUE_EX(CActionBase, Action)

	*ppunkTarget = pThis->m_punkTarget;
	if( *ppunkTarget )(*ppunkTarget)->AddRef();

	return S_OK;
}


HRESULT CActionBase::XAction::Invoke()
{
	_try_nested(CActionBase, Action, Invoke)
	{
		if( !pThis->IsAvaible() )
			return S_FALSE;
		if( !pThis->CanInvoke() )
			return S_FALSE;
		if( !pThis->Invoke() )
		{
			BSTR	bstr;
			pThis->m_pActionInfo->GetCommandInfo( &bstr, 0, 0, 0 );
			CString	strActionName = bstr;
			::SysFreeString( bstr );

			CString	strMessage;

			if( !pThis->m_strErrorString.IsEmpty() )
			{
				strMessage.Format( "Action %s fault with following error:\n%s", (const char *)strActionName, (const char *)pThis->m_strErrorString );
				AfxMessageBox( strMessage );
			}
			
			return S_FALSE;
		}

		pThis->MakeModified();

		//detach target event (because UnPrepareUndo detach's target without notification
		//and bugs in interactive action's is possible
		pThis->OnAttachTarget( pThis->m_punkTarget, false );

		if( !pThis->UnPrepareUndoRedo() )
			return S_FALSE;
		return S_OK;

	}
	_catch_nested;
}

HRESULT CActionBase::XAction::GetActionState( DWORD *pdwState )
{
	METHOD_PROLOGUE_EX(CActionBase, Action)
	*pdwState = pThis->GetActionState();
	return S_OK;
}

HRESULT CActionBase::XAction::StoreParameters( BSTR *pbstr )
{
	METHOD_PROLOGUE_EX(CActionBase, Action)
	{
		CString s( pThis->GetTextParams() );
		*pbstr = s.AllocSysString();
		return S_OK;
	}
}

HRESULT CActionBase::XAction::AttachTarget( IUnknown *punkTarget )
{
	METHOD_PROLOGUE_EX(CActionBase, Action)
	{
		if( pThis->m_lTargetKey == GetObjectKey( punkTarget ) &&
			(pThis->m_punkTarget == 0) == (punkTarget == 0) )
			return S_OK;

		pThis->OnAttachTarget( pThis->m_punkTarget, false );
		if(pThis->m_punkTarget)pThis->m_punkTarget->Release();
		if(punkTarget)punkTarget->AddRef();
		pThis->m_punkTarget = punkTarget;
		pThis->OnAttachTarget( pThis->m_punkTarget, true );

		pThis->m_lTargetKey = ::GetObjectKey( pThis->m_punkTarget );

		return S_OK;
	}
}

HRESULT CActionBase::XAction::SetArgument( BSTR bstrArgName, VARIANT *pvarVal )
{
	METHOD_PROLOGUE_EX(CActionBase, Action)
	{
		CString	str( bstrArgName );
		
		COleVariant	var( pvarVal );
		if( var.vt & VT_BYREF )
			var.ChangeType( VT_BSTR );

		pThis->m_args[str] = var;
		return S_OK;
	}
}


HRESULT CActionBase::XAction::SetResult( BSTR bstrArgName, VARIANT *pvarVal )
{
	METHOD_PROLOGUE_EX(CActionBase, Action)
	{
		CString	str( bstrArgName );

		pThis->m_args[str] = COleVariant( pvarVal );
		return S_OK;
	}
}

HRESULT CActionBase::XAction::GetActionInfo(IUnknown** ppunkAI)
{
	METHOD_PROLOGUE_EX(CActionBase, Action)
	{
		if(ppunkAI)
		{
			*ppunkAI = (IUnknown*)pThis->m_pActionInfo;
			if(*ppunkAI)
				(*ppunkAI)->AddRef();
		}
		return S_OK;
	}
}


bool CActionBase::Invoke()
{
	AfxMessageBox( GetName() );
	return true;
}

CString CActionBase::GetTextParams()
{
	StoreCurrentExecuteParams();

	CString	strParam;
	int	iParamCount = 0;

	m_pActionInfo->GetArgsCount( &iParamCount );

	for( int i = 0; i < iParamCount; i++ )
	{
		BSTR	bstrName;
		BSTR	bstrKind;
		BSTR	bstrDefVal;

		m_pActionInfo->GetArgInfo( i, &bstrName, &bstrKind, &bstrDefVal );

		CString	strName( bstrName );
		CString	strValue( bstrDefVal );
		CString	strKind( bstrKind );

		::SysFreeString( bstrName );
		::SysFreeString( bstrDefVal );
		::SysFreeString( bstrKind );

		strValue = GetArgument(strName);
		//VariantChangeType(
		//if(strName.GetLength() > 0)
		//{
		//	_VarChangeType( var, VT_BSTR );
		//	strValue = var.bstrVal;
		//}

		if( strKind == szArgumentTypeString )
			strValue = "\""+strValue+"\"";

		if( !strParam.IsEmpty() )
			strParam += ", ";

		strParam += strValue;
	}
	return strParam;
}

bool CActionBase::CanInvoke()
{
	return true;
}

//arguments helper
void CActionBase::SetArgument( const char *szArgName, const VARIANT var )
{
	m_args[szArgName] = var;
}

void CActionBase::SetResult( const char *szArgName, VARIANT *pvarRes )
{
	COleVariant	&variant = m_args[szArgName];

	if( variant.vt == VT_EMPTY )
		return;
	if( (variant.vt & VT_BYREF ) == 0 )
		return;

	variant.vt = VT_BYREF|VT_VARIANT;
	VariantCopy( variant.pvarVal, pvarRes );
}

VARIANT CActionBase::GetArgument( const char *szArgName )
{
	COleVariant	var = m_args[szArgName];
	return var.Detach();//Win 98 problem 7.05.2002
}

CString CActionBase::GetArgumentString( const char *szArgName )
{
//	_try(CActionBase, GetArgumentString)
	{
		_variant_t	var  = m_args[szArgName];
		_VarChangeType( var, VT_BSTR );
		CString	s = var.bstrVal;
		return s;
	}
//	_catch;
//	return "";
}

//returns the integer value
long CActionBase::GetArgumentInt( const char *szArgName )
{
//	_try(CActionBase, GetArgumentInt)
	{
		_variant_t	var = m_args[szArgName];
		_VarChangeType( var, VT_I4 );
		return var.lVal;
	}
//	_catch;
//	return 0;
}

double CActionBase::GetArgumentDouble( const char *szArgName )
{
	//_try(CActionBase, GetArgumentDouble)
	{
		_variant_t	var = m_args[szArgName];
		_VarChangeType( var, VT_R8 );
			return var.dblVal;
	}
//	return 0;
}

CString CActionBase::GetName()
{
	IUnknown	*punk = GetParent();
	ASSERT( punk );

	sptrIActionInfo	sp( punk );

	punk->Release();

	BSTR	bstr;

	sp->GetCommandInfo( &bstr, 0, 0, 0 );

	CString	s = bstr;

	::SysFreeString( bstr );

	return s;
}

IActionManager *CActionBase::GetActionManager()
{
	IApplication	*pa = GetApplication();

	if( !pa )
		return 0;

	IUnknown *punk = 0;

	pa->GetActionManager( &punk );

	pa->Release();

	if( !punk )
		return 0;

	IActionManager	*pam = 0;
	
	punk->QueryInterface( IID_IActionManager, (void **)&pam );

	punk->Release();

	return pam;
}

IApplication *CActionBase::GetApplication()
{
	IApplication *pa = 0;
	IUnknown	*punk = GetParent();

	ASSERT( punk );

	IRootedObjectPtr	sp( punk );

	punk->Release();

	IUnknown *punk1 = 0;

	sp->GetParent( &punk1 );

	if( !punk1 )
		return 0;

	punk1->QueryInterface( IID_IApplication, (void**)&pa );

	punk1->Release();

	return pa;
}
///////////////////////////////////////////////////////////////////////////////////////
//CActionInfoBase
BEGIN_INTERFACE_MAP(CActionInfoBase, CCmdTargetEx)
	INTERFACE_PART(CActionInfoBase, IID_IActionInfo, Info)
	INTERFACE_PART(CActionInfoBase, IID_IActionInfo2, Info)
	INTERFACE_PART(CActionInfoBase, IID_IActionInfo3, Info)
	INTERFACE_PART(CActionInfoBase, IID_IRootedObject, Rooted)
	INTERFACE_PART(CActionInfoBase, IID_INumeredObject, Num)
	INTERFACE_PART(CActionInfoBase, IID_IHelpInfo, Help)
END_INTERFACE_MAP();

IMPLEMENT_DYNAMIC(CActionInfoBase,CCmdTargetEx);
IMPLEMENT_UNKNOWN(CActionInfoBase, Info);

CActionInfoBase::CActionInfoBase()
{
	_OleLockApp( this );

	m_bInitialized = false;
	m_bShouldReleaseBmpProvider = false;
	m_nToolBarResourceID = IDS_DEFAULT_TOOLBAR;
	m_nMenuResourceID = IDS_DEFAULT_MENU;
	m_nLocalID = -1;
    m_spunkBmpHelper = 0;
}

CActionInfoBase::~CActionInfoBase()
{
	if( m_bShouldReleaseBmpProvider )
		s_pBmps->GetControllingUnknown()->Release();
	_OleUnlockApp( this );
	//TRACE( "%s Released!\n", (LPCSTR)m_strName );
}

void CActionInfoBase::Init()
{
	if( m_bInitialized )
		return;

	m_bInitialized = true;

	ASSERT(GetCommand());
	//load and phrase action string

	CString strAction, strToken;
	strAction.LoadString( GetCommand() );

	LoadString( strAction, false );
	m_strMenuNames = GetDefMenuString();
	m_strToolBarName = GetDefToolBarString();
}

CString CActionInfoBase::GetDefMenuString()
{
	LOCAL_RESOURCE;
	CString s;
	s.LoadString( m_nMenuResourceID );

	return s;
}

HRESULT CActionInfoBase::XInfo::GetHotKey( DWORD *pdwKey, BYTE *pbVirt )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		//
		*pdwKey = 0;
		*pbVirt = 0;

		CDWordArray	dwa;
		::EnumResourceNames( AfxGetApp()->m_hInstance, RT_ACCELERATOR, (ENUMRESNAMEPROC)EnumResNameProc, (LONG)&dwa );

		for( int i = 0; i < dwa.GetSize(); i++ )
		{
			HACCEL	hAccel = ::LoadAccelerators( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(dwa[i]) );
			int nAccelSize = ::CopyAcceleratorTable( hAccel, NULL, 0 );

			LPACCEL lpAccel = new ACCEL [nAccelSize];
			ASSERT (lpAccel != NULL);

			::CopyAcceleratorTable( hAccel, lpAccel, nAccelSize );

			for( int nAccel = 0; nAccel < nAccelSize; nAccel++ )
			{
				if( lpAccel[nAccel].cmd == pThis->GetCommand() )
				{
					*pdwKey = lpAccel[nAccel].key;
					*pbVirt = lpAccel[nAccel].fVirt;


					delete lpAccel;

					return S_OK;
				}
			}

			delete lpAccel;
		}
		return S_FALSE;
	}
}



CString CActionInfoBase::GetDefToolBarString()
{
	LOCAL_RESOURCE;
	CString s;
	s.LoadString( m_nToolBarResourceID );

	return s;
}

HRESULT CActionInfoBase::XInfo::GetLocalID( int *pn )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	*pn = pThis->m_nLocalID;
	return S_OK;
}

HRESULT CActionInfoBase::XInfo::SetLocalID( int n )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	pThis->m_nLocalID = n;
	return S_OK;
}


HRESULT CActionInfoBase::XInfo::IsArgumentDataObject( int iParamIdx, BOOL bInArg, BOOL *pbIs )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		ArgumentInfo	*pai = pThis->GetArgInfo( iParamIdx, bInArg == TRUE );
		*pbIs = pai->bDataObject;

		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::GetArgsCount( int *piParamCount )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		*piParamCount = pThis->GetArgCount( true );;
		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::GetArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		ArgumentInfo	*pai = pThis->GetArgInfo( iParamIdx, true );

		ASSERT( pai );

		if( pbstrName )
			*pbstrName = CString( pai->pszArgName ).AllocSysString();
		if( pbstrKnid )
			*pbstrKnid = CString( pai->pszArgType ).AllocSysString();
		if( pbstrDefValue )
			if( pai->pszDefValue )
				*pbstrDefValue = CString( pai->pszDefValue ).AllocSysString();
			else
				*pbstrDefValue = 0;

		return S_OK;
	}
}


HRESULT CActionInfoBase::XInfo::GetArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue, int* pNumber )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		ArgumentInfo	*pai = pThis->GetArgInfo( iParamIdx, true,  pNumber);

		ASSERT( pai );

		if( pbstrName )
			*pbstrName = CString( pai->pszArgName ).AllocSysString();
		if( pbstrKnid )
			*pbstrKnid = CString( pai->pszArgType ).AllocSysString();
		if( pbstrDefValue )
			if( pai->pszDefValue )
				*pbstrDefValue = CString( pai->pszDefValue ).AllocSysString();
			else
				*pbstrDefValue = 0;

		return S_OK;
	}
}


HRESULT CActionInfoBase::XInfo::GetOutArgsCount( int *piParamCount )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		*piParamCount = pThis->GetArgCount( false );
		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::GetOutArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		ArgumentInfo	*pai = pThis->GetArgInfo( iParamIdx, false );

		ASSERT( pai );

		if( pbstrName )
			*pbstrName = CString( pai->pszArgName ).AllocSysString();
		if( pbstrKind )
			*pbstrKind = CString( pai->pszArgType ).AllocSysString();

		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::GetOutArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, int* pNumber )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		ArgumentInfo	*pai = pThis->GetArgInfo( iParamIdx, false,  pNumber);

		ASSERT( pai );

		if( pbstrName )
			*pbstrName = CString( pai->pszArgName ).AllocSysString();
		if( pbstrKind )
			*pbstrKind = CString( pai->pszArgType ).AllocSysString();

		return S_OK;
	}
}


HRESULT CActionInfoBase::XInfo::IsFunction( BOOL *pboolAction )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		pThis->Init();
		*pboolAction = false;
		return S_OK;
	}
}

//user interface
HRESULT CActionInfoBase::XInfo::GetCommandInfo( 
			//general information
			BSTR	*pbstrActionName,
			BSTR	*pbstrActionUserName,
			BSTR	*pbstrActionHelpString, //"Image rotation\nRotation
			BSTR	*pbstrGroupName	)
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		pThis->Init();
		if( pbstrActionName )
			*pbstrActionName = pThis->m_strName.AllocSysString();
		if( pbstrActionUserName )
			*pbstrActionUserName = pThis->m_strUserName.AllocSysString();
		if( pbstrActionHelpString )
			*pbstrActionHelpString = pThis->m_strHelpStringTooltip.AllocSysString();
		if( pbstrGroupName )
			*pbstrGroupName = pThis->m_strGroupName.AllocSysString();
		return S_OK;
	}
}


HRESULT CActionInfoBase::XInfo::GetRuntimeInformaton( DWORD *pdwDllCode, DWORD *pdwCmdCode )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		if( pdwDllCode )
			*pdwDllCode = (DWORD)pThis->GetHinstance();
		if( pdwCmdCode )
			*pdwCmdCode = pThis->GetCommand();

		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::GetTBInfo(
			//toolbar information
			BSTR	*pbstrDefTBName,	//default toolbar name (as "edit operaion")
			IUnknown **ppunkBitmapHelper )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
//ensure initialised state firstly...
		pThis->Init();
//store toolbar name
		if( pbstrDefTBName )
			*pbstrDefTBName = pThis->m_strToolBarName.AllocSysString();

//ensure bitmap provider allready initialized here
		if( ppunkBitmapHelper )
		{
			// [vanek] : support IActionInfo3 interface - 13.12.2004
			if( pThis->m_spunkBmpHelper != 0 )
			{
				*ppunkBitmapHelper = pThis->m_spunkBmpHelper;
				(*ppunkBitmapHelper)->AddRef();
			}
			else
			{
				if( !pThis->s_pBmps )
				{
					pThis->s_pBmps = new CBitmapProvider();
					pThis->m_bShouldReleaseBmpProvider = true;
				}

				*ppunkBitmapHelper = pThis->s_pBmps->GetControllingUnknown();
				(*ppunkBitmapHelper)->AddRef();
				pThis->s_pBmps->EnsureInstanceWasLooked( pThis->GetHinstance() );
			}
		}

		return S_OK;
	}
}

	//menu information
HRESULT CActionInfoBase::XInfo::GetMenuInfo(
	BSTR	*pbstrMenus,
	DWORD	*pdwMenuFlag )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		pThis->Init();
		*pbstrMenus = pThis->m_strMenuNames.AllocSysString();

		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::ConstructAction( IUnknown **ppunk )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		CRuntimeClass *pclass = pThis->GetActionRuntime();
		CActionBase *pobj = (CActionBase *)pclass->CreateObject();
		IUnknown *punk = pobj->GetControllingUnknown();

		pobj->SetParent( pThis->GetControllingUnknown() );
		pobj->m_pActionInfo = this;
		pobj->m_pActionInfo->AddRef();

		pobj->Init();

		*ppunk = punk;

		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::GetTargetName( BSTR *pbstr )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		CString	str = pThis->GetTarget();
		*pbstr = str.AllocSysString();

		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::GetUIUnknown( IUnknown **ppunkinterface )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)
	{
		(*ppunkinterface) = 0;

		CRuntimeClass	*pclass = pThis->GetUIRuntime();
		if( !pclass )return S_OK;

		CObject	*pobject = pclass->CreateObject();
		ASSERT( pobject->IsKindOf( RUNTIME_CLASS( CCmdTarget ) ) );

		(*ppunkinterface) = ((CCmdTarget*)pobject)->GetControllingUnknown();
		//(*ppunkinterface)->AddRef();

		return S_OK;
	}
}

HRESULT CActionInfoBase::XInfo::SetActionString( BSTR bstr )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)

	_bstr_t	bstrT( bstr );
	pThis->LoadString( bstrT, true );

	return S_OK;
}

HRESULT CActionInfoBase::XInfo::SetBitmapHelper( IUnknown *punkBitmapHelper )
{
	METHOD_PROLOGUE_EX(CActionInfoBase, Info)

	if( !punkBitmapHelper )
		return E_INVALIDARG;

	pThis->m_spunkBmpHelper = punkBitmapHelper;
	return S_OK;
}

void CActionInfoBase::LoadString( const char *psz, bool bAliase )
{
//1. get the first token - Name

	if( !bAliase )
	{
		CString	strAction  = psz;
		CString	strToken;
		int	idx = strAction.Find( "\n" );

		if( idx == -1 )
		{
			strToken = strAction;
			strAction = "";
		}
		else
		{
			strToken = strAction.Left( idx );
			strAction = strAction.Right( strAction.GetLength()-idx-1 );
		}
		if( !bAliase )m_strName = strToken;
	//check for valid action name
		ASSERT( !m_strName.IsEmpty() );
		ASSERT( m_strName.Find( " " ) == -1 );
	//2. load the user name
		idx = strAction.Find( "\n" );

		if( idx == -1 )
		{
			strToken = strAction;
			strAction = "";
		}
		else
		{
			strToken = strAction.Left( idx );
			strAction = strAction.Right( strAction.GetLength()-idx-1 );
		}

		m_strUserName = strToken;
		if( m_strUserName.IsEmpty() )
			m_strUserName = m_strName;
	//3. Load the action group name
		idx = strAction.Find( "\n" );

		if( idx == -1 )
		{
			strToken = strAction;
			strAction = "";
		}
		else
		{
			strToken = strAction.Left( idx );
			strAction = strAction.Right( strAction.GetLength()-idx-1 );
		}

		m_strGroupName = strToken;
		if( m_strGroupName.IsEmpty() )
		{
			LOCAL_RESOURCE;
			m_strGroupName.LoadString( IDS_DEFAULT_GROUP );
		}
	//4. Load the help string
		CString	strHelpString, strToolTip;

		idx = strAction.Find( "\n" );

		if( idx == -1 )
		{
			strToken = strAction;
			strAction = "";
		}
		else
		{
			strToken = strAction.Left( idx );
			strAction = strAction.Right( strAction.GetLength()-idx-1 );
		}

		strHelpString = strToken;
		if( strHelpString.IsEmpty() )
			strHelpString = m_strUserName;
	//5. Load the tooltip
		idx = strAction.Find( "\n" );

		if( idx == -1 )
		{
			strToken = strAction;
			strAction = "";
		}
		else
		{
			//possible, the next version of ActionInfo string
			strToken = strAction.Left( idx );
			strAction = strAction.Right( strAction.GetLength()-idx-1 );
		}

		strToolTip = strToken;
		if( strToolTip.IsEmpty() )
			strToolTip = strHelpString;

		m_strHelpStringTooltip = strHelpString+"\n"+strToolTip;
	}
	else
	{
		_bstr_t	bstrT( psz );
		char	szDelimiter[] = "\n#";
		m_strUserName = strtok( bstrT, szDelimiter );
		m_strGroupName = strtok( 0, szDelimiter );
		m_strHelpStringTooltip = strtok( 0, szDelimiter );
		m_strHelpStringTooltip +="\n";
		m_strHelpStringTooltip +=strtok( 0, szDelimiter );
	}
}

//works with arguments		
int CActionInfoBase::GetArgCount( bool bIn )
{
	ArgumentInfo *pinfo = GetArgumentInfos();

	if( !pinfo )
		return 0;

	int	iCounter = 0;

	for( int i = 0; pinfo[i].pszArgName != 0; i++ )
	{
		if( bIn == pinfo[i].bArg )
			iCounter++;
	}
	return iCounter;
}

ArgumentInfo *CActionInfoBase::GetArgInfo( int idx, bool bIn, int* pNumber )
{
	ArgumentInfo *pinfo = GetArgumentInfos();

	ASSERT(pinfo);

	int	iCounter = 0;

	for( int i = 0; pinfo[i].pszArgName != 0; i++ )
	{
		if(pNumber)
			*pNumber = i;
		if( bIn == pinfo[i].bArg )
		{
			if( iCounter == idx )
				return &pinfo[i];
			iCounter++;
		}
	}

	return 0;
}



	
//action registrator and file namse helper

bool CanRegisterAction( CVTFactory* pf )
{
	if( !pf )
		return false;

	IUnknown * punkApp = 0;
	typedef IUnknown* (*PGUARDGETAPPUNKNOWN)(bool bAddRef /*= false*/);
	HMODULE hModule = GetModuleHandle( 0 );
	if (hModule)
	{
		PGUARDGETAPPUNKNOWN pfn = (PGUARDGETAPPUNKNOWN)GetProcAddress(hModule, "GuardGetAppUnknown");
		if( !pfn )
			return false;

		punkApp = pfn(false);
	}

	ISetupPtr ptrSetup( punkApp );
	if( ptrSetup == 0 )
		return true;


	BOOL bCanRegister = FALSE;
	ptrSetup->CanRegisterAction( pf->GetClassID(), &bCanRegister );	

	return ( bCanRegister == TRUE );
}

CActionRegistrator::CActionRegistrator() 
	: CCompRegistrator( szPluginAction )
{
}	


void CActionRegistrator::RegisterActions( bool bRegister )
{	
	AFX_MODULE_STATE* pModuleState = AfxGetModuleState();

	for (COleObjectFactory* pFactory = pModuleState->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
	{
		// register any non-registered, non-doctemplate factories
		if (!pFactory->IsRegistered() && pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)) && 
			((CVTFactory*)pFactory)->GetClassID() != CLSID_NULL)
		{
			if( !CanRegisterAction( (CVTFactory*)pFactory ) )
				continue;

			if (!pFactory->Register())
				continue;

			CString	sTest = ((CVTFactory*)pFactory)->GetProgID();

			if (IsAction(sTest))
			{
				if (bRegister)
					RegisterComponent(sTest);
				else
					UnRegisterComponent(sTest);
			}
		}
	}


	for (CDynLinkLibrary* pDLL = pModuleState->m_libraryList; pDLL != NULL; pDLL = pDLL->m_pNextDLL)
	{
		for (COleObjectFactory *pFactory = pDLL->m_factoryList; pFactory != NULL; pFactory = pFactory->m_pNextFactory)
		{
			if (pFactory->IsKindOf(RUNTIME_CLASS(CVTFactory)))
			{
				if( !CanRegisterAction( (CVTFactory*)pFactory ) )
					continue;

				CString	sTest = ((CVTFactory*)pFactory)->GetProgID();

				if (IsAction(sTest))
				{
					if (bRegister)
						RegisterComponent(sTest);
					else
						UnRegisterComponent(sTest);
				}
			}
		}
	}

//	COleObjectFactory::UpdateRegistryAll( bRegister );
}	


bool CActionRegistrator::IsAction(const char * szProgID)
{
	CString sProgID("VideoTesT5Action.");
	CString sTest = szProgID;

	sTest = sTest.Left(sProgID.GetLength());
	return sTest == sProgID;
}

///////////////////////////////////////////////////////////////////////////////
//CBitmapProvider
CBitmapProvider::CBitmapProvider()
{
	Init();
}

CBitmapProvider::~CBitmapProvider()
{
	DeInit();
}

BEGIN_INTERFACE_MAP(CBitmapProvider, CCmdTargetEx)
	INTERFACE_PART(CBitmapProvider, IID_IBitmapProvider, Bmps)
	INTERFACE_PART(CBitmapProvider, IID_INumeredObject, Num)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CBitmapProvider, Bmps)


void CBitmapProvider::EnsureInstanceWasLooked( HINSTANCE hInstance )
{
	for( int i = 0; i < m_arrLookedInstances.GetSize(); i++ )
		if( ( HINSTANCE )m_arrLookedInstances[i] == hInstance )
			return;

	m_arrLookedInstances.Add( (void*)hInstance );
	
	struct CToolBarData
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;
		//WORD aItems[wItemCount]

		WORD* items()
			{ return (WORD*)(this+1); }
	};

	CDWordArray	dwa;
	::EnumResourceNames( hInstance, RT_TOOLBAR, (ENUMRESNAMEPROC)EnumResNameProc, (LONG)&dwa );

	for( i = 0; i < dwa.GetSize(); i++ )
	{
		DWORD	dw = dwa[i];

		//load the bitmap resource
		HBITMAP	hbmp = (HBITMAP)::LoadImage( hInstance, MAKEINTRESOURCE(dw), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADMAP3DCOLORS );
		CBitmap	bitmap;

		//bitmap.LoadMappedBitmap
		//bitmap.LoadBitmap( MAKEINTRESOURCE(dw) );
		bitmap.Attach( hbmp );
		m_arrBmps.Add( bitmap.Detach() );
		m_iCounter++;

		//load the toolbar resource
//find resource
		//HINSTANCE hInstance = AfxFindResourceHandle( MAKEINTRESOURCE(dw), RT_TOOLBAR );
		HRSRC hRsrc = ::FindResource( hInstance, MAKEINTRESOURCE(dw), RT_TOOLBAR );
		if( hInstance == NULL )
			continue;

		HGLOBAL hGlobal = LoadResource(hInstance, hRsrc);
		if( hGlobal == NULL )
			continue;

		CToolBarData* pData = (CToolBarData*)LockResource(hGlobal);
		if (pData == NULL)
			continue;

		ASSERT(pData->wVersion == 1);

		int	iPictureCounter = 0;

		for (int j = 0; j < pData->wItemCount; j++)
		{
			UINT	nLocalCmd = pData->items()[j];

			if( nLocalCmd == 0 )//separator 
				continue;

			CmdInfo	*pinfo = new CmdInfo;
			pinfo->lBmpNo = m_iCounter-1;
			pinfo->lPictNo = iPictureCounter++;
			pinfo->nCmd = nLocalCmd;
			pinfo->dwDllCode = (DWORD)hInstance;

			m_arrCmdInfos.Add( pinfo );
		}
//		CSize sizeImage(pData->wWidth, pData->wHeight);
//		CSize sizeButton(pData->wWidth + 7, pData->wHeight + 7);

		UnlockResource(hGlobal);
		FreeResource(hGlobal);
	}
}

void CBitmapProvider::Init()
{
	m_iCounter = 0;
}

void CBitmapProvider::DeInit()
{
	for( int i = 0; i < m_arrBmps.GetSize(); i++ )
		::DeleteObject( (HBITMAP)m_arrBmps[i] );
	m_arrBmps.RemoveAll();

	for( i = 0; i < m_arrCmdInfos.GetSize(); i++ )
		delete (CmdInfo*)m_arrCmdInfos[i];
	m_arrCmdInfos.RemoveAll();
}

//dummy implementation - call main class members
HRESULT CBitmapProvider::XBmps::GetBitmapCount( long *piCount )
{
	_try_nested(CBitmapProvider, Bmps, GetBitmapCount)
	{
		*piCount = pThis->GetBitmapCount();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CBitmapProvider::XBmps::GetBitmap( int idx, HANDLE *phBitmap )
{
	_try_nested(CBitmapProvider, Bmps, GetBitmap)
	{
		if( idx == -1 )
			*phBitmap = 0;
		else
			*phBitmap = pThis->GetBitmap( idx );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CBitmapProvider::XBmps::GetBmpIndexFromCommand( UINT nCmd, DWORD dwDllCode, long *plIndex, long *plBitmap )
{
	_try_nested(CBitmapProvider, Bmps, GetBitmapCount)
	{
		long	lBmpIndex = -1;
		long	lIndex = -1;

		CBitmapProvider::CmdInfo *pi = 
				pThis->GetCmdInfo( nCmd, dwDllCode );

		if( pi )
		{
			lBmpIndex = pi->lBmpNo;
			lIndex = pi->lPictNo;
		}

		if( plIndex )
			*plIndex = lIndex;

		if( plBitmap )
			*plBitmap = lBmpIndex;

		return S_OK;
	}
	_catch_nested;
}

long CBitmapProvider::GetBitmapCount()
{
	return m_arrBmps.GetSize();
}

HANDLE CBitmapProvider::GetBitmap( int idx )
{
	return (HANDLE)m_arrBmps[idx];
}

CBitmapProvider::CmdInfo *CBitmapProvider::GetCmdInfo( UINT nCmd, DWORD dwDllCode )
{
	for( int i = 0; i < m_arrCmdInfos.GetSize(); i++ )
	{
		CmdInfo	*pcmd = (CmdInfo *)m_arrCmdInfos[i];
		if( pcmd->nCmd == nCmd &&
			pcmd->dwDllCode == dwDllCode	)
			return pcmd;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////
BEGIN_INTERFACE_MAP(CActionDlgHelper, CCmdTargetEx)
	INTERFACE_PART(CActionDlgHelper, IID_ICustomizableAction, Dlg)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CActionDlgHelper, Dlg);

CActionDlgHelper::CActionDlgHelper( CActionBase *paction )
{
	EnableAggregation();

	m_pa = paction;
}

CActionDlgHelper::~CActionDlgHelper()
{
}

HRESULT CActionDlgHelper::XDlg::CreateSettingWindow( HWND hWnd )
{
	_try_nested(CActionDlgHelper, Dlg, CreateSettingWindow)
	{
		CWnd	*pwnd = CWnd::FromHandle( hWnd );
		if( pThis->m_pa->ExecuteSettings( pwnd ) )
			return S_OK;
		else
			return S_FALSE;
	}
	_catch_nested;
}

HRESULT CActionDlgHelper::XDlg::GetFlags( DWORD *pdwFlags )
{
	_try_nested(CActionDlgHelper, Dlg, GetFlags)
	{
		*pdwFlags = 0;
		return S_OK;
	}
	_catch_nested;
}

/////////////////////////////////////////////////////////////////////////////////////////
//CActionUndoHelper
BEGIN_INTERFACE_MAP(CActionUndoHelper, CCmdTargetEx)
	INTERFACE_PART(CActionUndoHelper, IID_IUndoneableAction, Undo)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CActionUndoHelper, Undo);

CActionUndoHelper::CActionUndoHelper( CActionBase *paction )
{
	EnableAggregation();
	m_pa = paction;
}

CActionUndoHelper::~CActionUndoHelper()
{
}

HRESULT CActionUndoHelper::XUndo::StoreState()
{
	_try_nested(CActionUndoHelper, Undo, StoreState)
	{
		return pThis->m_pa->StoreState()?S_OK:S_FALSE;
	}
	_catch_nested;
}

HRESULT CActionUndoHelper::XUndo::Undo()
{
	_try_nested(CActionUndoHelper, Undo, Undo)
	{
		if( !pThis->m_pa->PrepareUndoRedo() )
			return S_FALSE;
		
		pThis->m_pa->DoUndo();
		pThis->m_pa->SetModified( false );

		if( !pThis->m_pa->UnPrepareUndoRedo() )
			return S_FALSE;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CActionUndoHelper::XUndo::Redo()
{
	_try_nested(CActionUndoHelper, Undo, Redo)
	{
		if( !pThis->m_pa->PrepareUndoRedo() )
			return S_FALSE;
		
		pThis->m_pa->DoRedo();
		pThis->m_pa->SetModified( true );

		if( !pThis->m_pa->UnPrepareUndoRedo() )
			return S_FALSE;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CActionUndoHelper::XUndo::GetTargetKey( GUID *plKey )
{
	_try_nested(CActionUndoHelper, Undo, GetTargetKey)
	{
		*plKey = pThis->m_pa->m_lTargetKey;
		return S_OK;
	}
	_catch_nested;
}




/////////////////////////////////////////////////////////////////////////////////////////
//interactive action implementation
BEGIN_INTERFACE_MAP(CInteractiveActionBase, CActionBase)
	INTERFACE_PART(CInteractiveActionBase, IID_IInteractiveAction, IntAction)
	INTERFACE_PART(CInteractiveActionBase, IID_IMsgListener, MsgList)
	INTERFACE_PART(CInteractiveActionBase, IID_IDrawObject, Draw)
	INTERFACE_PART(CInteractiveActionBase, IID_IEventListener, EvList)
END_INTERFACE_MAP();

IMPLEMENT_UNKNOWN(CInteractiveActionBase, IntAction);


BEGIN_MESSAGE_MAP(CInteractiveActionBase, CActionBase)
	//{{AFX_MSG_MAP(CInteractiveActionBase)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP();

CInteractiveActionBase::CInteractiveActionBase()
{
	m_state = asInactive;
	m_bStoreInteractiveFlag = false;
	m_hwndTarget = 0;

	m_hcurActive = 0;

	m_bOldEnable = true;
	_OleLockApp( this );
}
CInteractiveActionBase::~CInteractiveActionBase()
{
	_DeInit();
	_OleUnlockApp( this );
}

//implementation of interface
HRESULT CInteractiveActionBase::XIntAction::BeginInteractiveMode()
{
	_try_nested(CInteractiveActionBase, IntAction, BeginInteractiveMode)
	{
		pThis->_Init();
		return (pThis->m_state == asActive||pThis->m_state == asReady)?S_OK:S_FALSE;
	}
	_catch_nested;
}

HRESULT CInteractiveActionBase::XIntAction::TerminateInteractive()
{
	_try_nested(CInteractiveActionBase, IntAction, TerminateInteractive)
	{
		pThis->Terminate();
		//pThis->Finalize();
		return S_OK;
	}
	_catch_nested;
}

HRESULT CInteractiveActionBase::XIntAction::IsComplete()
{
	_try_nested(CInteractiveActionBase, IntAction, IsComplete)
	{
		if( pThis->GetState() == CInteractiveActionBase::asReady )
			return S_OK;
		if( pThis->GetState() == CInteractiveActionBase::asTerminate )
			return ERROR_CANCELLED;
			
		return S_FALSE;
	}
	_catch_nested;
}


bool CInteractiveActionBase::_Init()
{
	if( m_state == asActive )
		return true;

	if( !Initialize() )
		return false;

	if( m_state != asReady )
		m_state = asActive;

	return true;
}

void CInteractiveActionBase::_DeInit()
{
	if( m_hwndTarget )
	{
		IWindow2Ptr		ptrWindow2( m_punkTarget );
		if( ptrWindow2 != 0 )
		{
			ptrWindow2->DetachDrawing( GetControllingUnknown() );
			ptrWindow2->DetachMsgListener( GetControllingUnknown() );
			m_hwndTarget = 0;
		}
	}

	if( m_state == asInactive || m_state == asReady || m_state == asTerminate )
		return;


//	m_state = asInactive;
	
}

void CInteractiveActionBase::_Draw( CDC *pdc )
{
	bool	bOwnDC = pdc == 0;

	CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );

	if( bOwnDC )
	{
		pdc = new CClientDC( pwnd );
	}

	int nOldROP = 
	
	pdc->SetROP2( R2_NOTXORPEN );
	pdc->SelectStockObject( BLACK_PEN );
	pdc->SelectStockObject( NULL_BRUSH );
	
	if( m_state == asActive )
		DoDraw( *pdc );

	pdc->SetROP2( nOldROP );

	if( bOwnDC )
	{
		delete pdc;
	}
}

//add the name of property page as NC
void CInteractiveActionBase::AddPropertyPageDescription( const char *szPageName )
{
	for( int i = 0; i < m_pages.GetSize(); i++ )
		if( m_pages[i] == szPageName )
			return;
	m_pages.Add( szPageName );
}


//resource-based version
void CInteractiveActionBase::AddPropertyPageDescription( UINT nID )
{
	CString	str;
	str.LoadString( nID );

	AddPropertyPageDescription( str );
}


void CInteractiveActionBase::AddPropertyPageDescriptionByProgID( const char *szPageProgID )
{
	m_pagesProgID.Add( szPageProgID );
}


void CInteractiveActionBase::Finalize()
{
	LeaveInteractiveMode();
	//clear "interactive running" flag
	/*{
		sptrIApplication	sptrA( GetAppUnknown() );

		IUnknown	*punkAM = 0; 
		sptrA->GetActionManager( &punkAM );
		sptrIActionManager	sptrAM( punkAM );
		punkAM->Release();

		sptrAM->SetInteractiveRunningFlag( false );
	}*/
	//remove from page's NC

	
	long nPagesCount = GetPropertyPagesCount();

	IUnknown	*punkPropertySheet = ::GetPropertySheet();

	if( punkPropertySheet )
	{
		IPropertySheetPtr	sptrO( punkPropertySheet );
		punkPropertySheet->Release();

		for( long n = 0; n < nPagesCount; n++ )
		{
			CString	strPage = GetPropertyPageName( n );
			long	nPageNo = 0;
			IUnknown *punkPage = ::GetPropertyPageByTitle( strPage, &nPageNo );

			if( !punkPage )
				continue;
			CEventListenerImpl::UnRegister( punkPage );

			sptrO->ExcludePage( nPageNo );

			punkPage->Release();
		}

		//UnRegister & hide prop pages, was incude by prog ID
		for( int i=0;i<m_pagesProgID.GetSize();i++ )
		{
			IUnknown* punkPP = NULL;
			int nPageNum = -1;
			if( ::GetPropertyPageByProgID( m_pagesProgID[i], &punkPP, &nPageNum ) )
			{
				CEventListenerImpl::UnRegister( punkPP );

				sptrO->ExcludePage( nPageNum );

				punkPP->Release();

			}
		}

	}

	CEventListenerImpl::UnRegister( GetAppUnknown() );

	_DeInit();

	// A.M. BT 4047. If non-interacive action, called from ActionHelper, was called during work of the
	// interactive action, user has finalized interactive action before before completion of
	// non-interactive action (i.e. inside CActiveXSite::OnTimer), ActionState_OnActionComplete
	// will be fired only after completion of the non-interactive action.
	// In some circumstances it's needed to terminate non-interactive action immediately.
	IScriptSitePtr	sptrSS(::GetAppUnknown());
	if (sptrSS != 0)
	{
		_bstr_t bstrEvent("ActionState_OnFinalizeInteractive");
		_variant_t vars[2];
		vars[0] = _variant_t((long)0);
		vars[1] = _variant_t(GetName());
		sptrSS->Invoke(bstrEvent, vars, 2, 0, fwFormScript);
		sptrSS->Invoke(bstrEvent, vars, 2, 0, fwAppScript);
	}

	if( m_state == asActive || m_state == asInactive )
		m_state = asReady;
}

bool CInteractiveActionBase::Initialize()
{
	//set "interactive running" flag
	/*{
		sptrIApplication	sptrA( GetAppUnknown() );

		IUnknown	*punkAM = 0; 
		sptrA->GetActionManager( &punkAM );
		sptrIActionManager	sptrAM( punkAM );
		punkAM->Release();

		sptrAM->SetInteractiveRunningFlag( true );
	}*/
	//place to the page's notify controller
	long nPagesCount = GetPropertyPagesCount();

	IUnknown	*punkPropertySheet = ::GetPropertySheet();

	if( punkPropertySheet )
	{
		IPropertySheetPtr	sptrO( punkPropertySheet );
		punkPropertySheet->Release();

		for( long n = nPagesCount-1; n >-1; n-- )
		{
			CString	strPage = GetPropertyPageName( n );
			long	nPageNo = 0;
			IUnknown *punkPage = ::GetPropertyPageByTitle( strPage, &nPageNo );

			if( !punkPage )
				continue;

			sptrO->IncludePage( nPageNo );

			CEventListenerImpl::Register( punkPage );

			punkPage->Release();
		}

		//Register & show prop pages, was incude by prog ID
		for( int i=0;i<m_pagesProgID.GetSize();i++ )
		{
			IUnknown* punkPP = NULL;
			int nPageNum = -1;
			if( ::GetPropertyPageByProgID( m_pagesProgID[i], &punkPP, &nPageNum ) )
			{
				sptrO->IncludePage( nPageNum );
				
				CEventListenerImpl::Register( punkPP );

				punkPP->Release();

			}
		}


	}

	CEventListenerImpl::Register( GetAppUnknown() );

	IWindow2Ptr	ptrWindow( m_punkTarget );
	if( ptrWindow != 0 )
	{
		ASSERT( m_hwndTarget == 0 );
		ptrWindow->AttachMsgListener( GetControllingUnknown() );
		ptrWindow->AttachDrawing( GetControllingUnknown() );
		ptrWindow->GetHandle( (HANDLE*)&m_hwndTarget );
		ASSERT( m_hwndTarget != 0 );
	}


	EnterInteractiveMode();
	DoUpdateSettings();

	return true;
}

IUnknown *CInteractiveActionBase::GetTargetWindow()
{
	/*if( m_punkTarget )
		m_punkTarget->AddRef();*/
	return m_punkTarget;
}

void CInteractiveActionBase::OnAttachTarget( IUnknown *punkTarget, bool bAttach )
{
	if( bAttach )
	{
		if( GetState() != asActive )return;

		IWindow2Ptr	ptrWindow( punkTarget );
		if( ptrWindow != 0 )
		{
			ASSERT( m_hwndTarget == 0 );
			ptrWindow->AttachMsgListener( GetControllingUnknown() );
			ptrWindow->AttachDrawing( GetControllingUnknown() );
			ptrWindow->GetHandle( (HANDLE*)&m_hwndTarget );
			ASSERT( m_hwndTarget != 0 );
		}
		_Draw();
	}
	else
	{
		if( GetState() == asActive )
			_Draw();
		
		if( m_hwndTarget != 0 )
		{
			IWindow2Ptr	ptrWindow( m_punkTarget );

			if( ptrWindow != 0 )
			{
				ptrWindow->DetachDrawing( GetControllingUnknown() );
				ptrWindow->DetachMsgListener( GetControllingUnknown() );
				m_hwndTarget = 0;
			}
		}
	}
}

void CInteractiveActionBase::LeaveMode()
{
	sptrIApplication	sptrA( GetAppUnknown() );

	IUnknown	*punkAM = 0; 
	sptrA->GetActionManager( &punkAM );
	sptrIActionManager	sptrAM( punkAM );

	if( !punkAM )
		return;
	punkAM->Release();

	sptrAM->ChangeInteractiveAction( 0 );

}

void CInteractiveActionBase::_MouseEvent( MouseEvent me, CPoint point )
{
	if( GetState() != asActive )
		return;
	CMouseImpl::_MouseEvent( me, point );
}

void CInteractiveActionBase::EnterInteractiveMode()
{
	sptrIApplication	sptrA( GetAppUnknown() );

	IUnknown	*punkAM = 0; 
	sptrA->GetActionManager( &punkAM );
	sptrIActionManager	sptrAM( punkAM );

	if( !punkAM )
		return;
	punkAM->Release();


	DWORD	dwFlags = 0;
	sptrAM->GetCurrentExecuteFlags( &dwFlags );

	m_bStoreInteractiveFlag = (dwFlags & aefInteractiveRunning) != 0;

	sptrAM->SetInteractiveRunningFlag( true );

	m_bOldEnable = CMouseImpl::Enable( false );
	ForceEnabled( true );
}

void CInteractiveActionBase::LeaveInteractiveMode()
{
	sptrIApplication	sptrA( GetAppUnknown() );

	IUnknown	*punkAM = 0; 
	sptrA->GetActionManager( &punkAM );
	sptrIActionManager	sptrAM( punkAM );

	if( !punkAM )
		return;
	punkAM->Release();

	if( m_bStoreInteractiveFlag )
		sptrAM->SetInteractiveRunningFlag( true );
	else
		sptrAM->SetInteractiveRunningFlag( false );

	CMouseImpl::Enable( m_bOldEnable );
	ForceEnabled( false );
}

bool CInteractiveActionBase::DoSetCursor( CPoint point )
{
	if( m_state == asActive && m_hcurActive )
	{
		::SetCursor( m_hcurActive );
		return true;
	}
	return CMouseImpl::DoSetCursor( point );
}

bool CInteractiveActionBase::DoUpdateSettings()
{
	return true;
}

void CInteractiveActionBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !::strcmp( pszEvent, szEventChangeProperties ) )
		DoUpdateSettings();
	else if( !strcmp( pszEvent, szEventActivateObject ) )
	{
		if( GetObjectKey( punkFrom ) == GetObjectKey( m_punkTarget ) )
		{
			TRACE( "ActiveObjectChange %s\n", GetObjectName( punkHit ) );
			DoActivateObjects( punkHit );
		}
	}
	else if (!strcmp(pszEvent, szEventNamedPropsChanged))
		UpdatePropertyPages();
}

bool CInteractiveActionBase::DoChar( UINT nChar )
{
	if( nChar == 27 )
	{
		Terminate();
		LeaveMode();
		return true;
	}
	return false;
}

void CInteractiveActionBase::Terminate()
{
	if( m_bInTrackMode )
		DoLeaveMode();
	/*if( GetState() != asActive )
	{
		LeaveMode();
		return;
	}*/
	CWnd	*pwnd = 0;
	CRect	rect = NORECT;

	if( m_punkTarget )
	{
		rect = ::ConvertToWindow( m_punkTarget, GetInvalidateRect() );
		pwnd = GetWindowFromUnknown( m_punkTarget );
	}

	m_state = asTerminate;
	Finalize();

	if(IsRequiredInvokeOnTerminate())
		m_state = asReady;

	if( DoTerminate() )
	{
		if(pwnd)
			pwnd->InvalidateRect( rect );
	}
}

CRect CInteractiveActionBase::GetInvalidateRect()
{
	if( m_punkTarget )
	{
		CSize	size;
		sptrIScrollZoomSite	site( m_punkTarget );
		site->GetClientSize( &size );

		return CRect( 0, 0, size.cx, size.cy );
	}
	return NORECT;
}

bool CInteractiveActionBase::DoTerminate()
{
	return true;
}

bool CInteractiveActionBase::CanInvoke()
{
	return m_state == asReady;
}

void CInteractiveActionBase::UpdatePropertyPages()
{
	long nPagesCount = GetPropertyPagesCount();

	IUnknown	*punkPropertySheet = ::GetPropertySheet();

	if( punkPropertySheet )
	{
		IPropertySheetPtr	sptrO( punkPropertySheet );
		punkPropertySheet->Release();
		long	nPageNo;

		for( long n = nPagesCount-1; n >-1; n-- )
		{
			CString	strPage = GetPropertyPageName( n );
			IUnknown *punkPage = ::GetPropertyPageByTitle( strPage, &nPageNo );
			if( !punkPage )
				continue;

			IOptionsPagePtr	page( punkPage );
			punkPage->Release();
			page->LoadSettings();
		}
	}
}