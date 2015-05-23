#include "stdafx.h"

#include "core5.h"
#include "data5.h"

#include "action_main.h"
#include "com_main.h"
#include "misc_templ.h"
#include "impl_misc.h"
#include "misc_utils.h"


#define RT_TOOLBAR  MAKEINTRESOURCE(241)


CBitmapProvider::CBitmapProvider()
{
	struct ToolBarData
	{
		WORD wVersion;
		WORD wWidth;
		WORD wHeight;
		WORD wItemCount;
		//WORD aItems[wItemCount]

		WORD* items()
			{ return (WORD*)(this+1); }
	};


//	HBITMAP hBitmap = ::LoadBitmap( App::handle(), MAKEINTRESOURCE(IDR_ALLRES)	 );
	HBITMAP	hBitmap = (HBITMAP)::LoadImage( App::handle(), MAKEINTRESOURCE(IDR_ALLRES), IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADMAP3DCOLORS );
	m_bitmap = hBitmap;

	if( hBitmap )
	{
		HRSRC hrsc = FindResource( App::handle(), MAKEINTRESOURCE(IDR_ALLRES), RT_TOOLBAR );
		if( hrsc )
		{
			HGLOBAL	htb = ::LoadResource( App::handle(),  hrsc );
			if( htb )
			{
				ToolBarData* pData = (ToolBarData*)LockResource( htb );
				if( pData )
				{
					m_cmds.alloc( pData->wItemCount );
					for (int i = 0; i < pData->wItemCount; i++)
						m_cmds[i] = pData->items()[i];
				}

				UnlockResource( htb );
				FreeResource( htb );
			}
		}
	}
}
CBitmapProvider::~CBitmapProvider()
{
}



int	CBitmapProvider::FindCommand( uint n )
{
	for( int i = 0; i < m_cmds.size(); i++ )
	{
		if( m_cmds[i] == n )
			return i;
	}
	return -1;
}

HRESULT CBitmapProvider::GetBitmapCount( long *piCount )
{
	*piCount = (m_bitmap == 0) ? 0:1;
	return S_OK;
}
HRESULT CBitmapProvider::GetBitmap( int idx, HANDLE *phBitmap )
{
	*phBitmap = m_bitmap;
	return S_OK;
}
HRESULT CBitmapProvider::GetBmpIndexFromCommand( UINT nCmd, DWORD dwDllCode, long *plIndex, long *plBitmap )
{
	dbg_assert( dwDllCode == (DWORD)App::handle() );

	int	nIndex = FindCommand( nCmd );
	int	nBitmap = (nIndex==-1)?-1:0;

	if( plIndex )
		*plIndex = nIndex;
	if( plBitmap )
		*plBitmap = nBitmap;

	return S_OK;
}


///////////////////////////////////////////////////////////////////////////////
//_ainfo_base
//base class for action information component
_ainfo_base::_ainfo_base()
{
	m_ready = false;
	m_id = 0;
	m_accl_key = -1;
	m_accl_virt = -1;
//	m_bitmap = -1;
	m_spunkBmpHelper = 0;
}

_ainfo_base::~_ainfo_base()
{
}

IUnknown *_ainfo_base::DoGetInterface( const IID &iid )
{
	if( iid == IID_IActionInfo3 )return (IActionInfo3*)this;
	else if( iid == IID_IActionInfo2 )return (IActionInfo2*)this;
	else if( iid == IID_IActionInfo )return (IActionInfo*)this;
	else if( iid == IID_INumeredObject)return (INumeredObject*)this;
	else if( iid == IID_IHelpInfo)return (IHelpInfo*)this;
	else return ComObjectBase::DoGetInterface( iid );
}


HRESULT _ainfo_base::GetArgsCount( int *piParamCount )
{
	int c = 0;
	
	arg	*p = args();
	if( p )for( ; p->pszArgName; p++ ) if( p->bArg )c++;

	*piParamCount = c;

	return S_OK;
}
HRESULT _ainfo_base::GetArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, BSTR *pbstrDefValue )
{
	return GetArgInfo2( iParamIdx, pbstrName, pbstrKind, pbstrDefValue, 0 );
}
HRESULT _ainfo_base::GetOutArgsCount( int *piParamCount )
{
	int c = 0;
	
	arg	*p = args();
	if( p )for( ; p->pszArgName; p++ ) if( !p->bArg )c++;

	*piParamCount = c;

	return S_OK;
}
HRESULT _ainfo_base::GetOutArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind )
{
	return GetOutArgInfo2( iParamIdx, pbstrName, pbstrKind, 0 );
}
HRESULT _ainfo_base::IsFunction( BOOL *pboolAction )
{
	*pboolAction = DoGetInterface( IID_IFilterAction ) != 0;
	return S_OK;
}
HRESULT _ainfo_base::GetCommandInfo( 
	//general information
	BSTR	*pbstrActionName,
	BSTR	*pbstrActionUserName,
	BSTR	*pbstrActionHelpString, //"Image rotation\nRotation
	BSTR	*pbstrGroupName	)
{
	ensure_initialized();

	if( pbstrActionName )*pbstrActionName = m_name.copy();
	if( pbstrActionUserName )*pbstrActionUserName = m_user_name.copy();
	if( pbstrActionHelpString )*pbstrActionHelpString = m_help.copy();
	if( pbstrGroupName )*pbstrGroupName = m_group.copy();

	return S_OK;
}
HRESULT _ainfo_base::GetTBInfo(
		//toolbar information
		BSTR	*pbstrDefTBName,	//default toolbar name (as "edit operaion")
		IUnknown **ppunkBitmapHelper )
{
	ensure_initialized();


	if( pbstrDefTBName )*pbstrDefTBName = 0;
	if( ppunkBitmapHelper )
	{
		// [vanek] : support IActionInfo3 interface - 13.12.2004
		if( m_spunkBmpHelper != 0 )
		{
			*ppunkBitmapHelper = m_spunkBmpHelper;
			(*ppunkBitmapHelper)->AddRef();
		}
		else
        	*ppunkBitmapHelper = CBitmapProvider::get_instance()->Unknown();
	}

	return S_OK;
}
	//menu information
HRESULT _ainfo_base::GetMenuInfo(
	BSTR	*pbstrMenuNames,	//"general\\File\\Open", "main\\Tools\\Image tranphormation\\Gamma correction", ...
	DWORD	*pdwMenuFlag )
{
	ensure_initialized();

	*pbstrMenuNames = 0;
	*pdwMenuFlag = 0;

	return S_OK;
}		//not used, must be zero. Will be used later, possible, 
HRESULT _ainfo_base::GetHotKey( DWORD *pdwKey, BYTE *pbVirt )
{
	ensure_initialized();

	if( m_accl_key != -1 )
	{
		*pdwKey = m_accl_key;
		*pbVirt = m_accl_virt;

		return S_OK;
	}
	else
		return S_FALSE;
}
HRESULT _ainfo_base::GetRuntimeInformaton( DWORD *pdwDllCode, DWORD *pdwCmdCode )
{
	*pdwDllCode = (DWORD)App::handle();
	*pdwCmdCode = resource();
	return S_OK;
}
HRESULT _ainfo_base::ConstructAction( IUnknown **ppunk )
{
	CAction	*pa = create();
	pa->init( this );
	*ppunk = pa->Unknown();
	return S_OK;
}

HRESULT _ainfo_base::GetTargetName( BSTR *pbstr )
{
	*pbstr = target().copy();
	return S_OK;
}
HRESULT _ainfo_base::GetLocalID( int *pnID )
{
	*pnID = m_id;
	return S_OK;
}
HRESULT _ainfo_base::SetLocalID( int nID )
{
	m_id = nID;
	return S_OK;
}
//IActionInfo2
HRESULT _ainfo_base::IsArgumentDataObject( int iParamIdx, BOOL bInArg, BOOL *pbIs )
{
	arg		*p = args();
	if( !p )return E_INVALIDARG;
	int idx;
	for( idx = 0; p->pszArgName; idx++, p++ )
	{
		if( (int)p->bArg != bInArg )continue;
		if( !iParamIdx )break;
		iParamIdx--;
	}

	if( idx == count() )return E_INVALIDARG;

	*pbIs = p->bDataObject;

	return S_OK;
}

HRESULT _ainfo_base::GetArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue, int* pNumber )
{
	arg		*p = args();
	if( !p )return E_INVALIDARG;
	int idx;
	for( idx = 0; p->pszArgName; idx++, p++ )
	{
		if( !p->bArg )continue;
		if( !iParamIdx )break;
		iParamIdx--;
	}

	if( idx == count() )return E_INVALIDARG;

	if( pbstrName )*pbstrName = _bstr_t( p->pszArgName ).copy();
	if( pbstrKnid )*pbstrKnid = _bstr_t( p->pszArgType ).copy();
	if( pbstrDefValue )*pbstrDefValue = _bstr_t( p->pszDefValue ).copy();

	if( pNumber )*pNumber = idx;

	return S_OK;
}

HRESULT _ainfo_base::GetOutArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, int* pNumber )
{
	arg	*p = args();
	if( !p )return E_INVALIDARG;

	int idx;
	for( idx = 0; p->pszArgName; idx++, p++ )
	{
		if( p->bArg )continue;
		if( !iParamIdx )break;
		iParamIdx--;
	}

	if( idx == count() )return E_INVALIDARG;

	if( pbstrName )*pbstrName = _bstr_t( p->pszArgName ).copy();
	if( pbstrKind )*pbstrKind = _bstr_t( p->pszArgType ).copy();

	if( pNumber )*pNumber = idx;

	return S_OK;
}
HRESULT _ainfo_base::GetUIUnknown( IUnknown **ppunkinterface )
{
	CActionUI	*pui = create_ui();
	if( pui )
		*ppunkinterface = pui->Unknown();
	else
		*ppunkinterface = 0;
	return S_OK;
}	

HRESULT _ainfo_base::SetActionString( BSTR bstr )
{
	_bstr_t	bstrT( bstr );
	load( bstrT, true );

	return S_OK;
}

HRESULT _ainfo_base::SetBitmapHelper( IUnknown *punkBitmapHelper )
{
	if( !punkBitmapHelper )
		return E_INVALIDARG;
    
	m_spunkBmpHelper = punkBitmapHelper;
	return S_OK;
}

int _ainfo_base::count()
{
	arg	*p = args();
	if( !p )return 0;
	int idx;
	for( idx = 0; p->pszArgName; idx++, p++ );
	return idx;
}

void _ainfo_base::load( const char *psz, bool bAliase )
{

	/*long	len = strlen( psz );
	if( len >= 256 )
	{
		DebugBreak();
	}*/
	char	sz[512];
	char	szDelimiter[] = "\n#";
	long	len = strlen( psz );
	strncpy( sz, psz, sizeof( sz ) );
	sz[sizeof(sz)-1] = 0;

	if( !bAliase )m_name = strtok( sz, szDelimiter );

	if( !bAliase )m_user_name = strtok( 0, szDelimiter );
	else m_user_name = strtok( sz, szDelimiter );

	m_group = strtok( 0, szDelimiter );
	m_help = strtok( 0, szDelimiter );
	m_tooltip = strtok( 0, szDelimiter );
}

void _ainfo_base::ensure_initialized()
{
	if( m_ready )return;
	m_ready = true;

	HMODULE	h = App::handle();
	uint	id = resource();

	char	sz[255];
	sz[0] = 0;
	::LoadString( h, id, sz, 255 );

	load( sz, false );

//	CBitmapProvider	*p = CBitmapProvider::get_instance();
//	m_bitmap = p->FindCommand( id );
//	p->_release();

	HACCEL	haccl = LoadAccelerators( App::handle(), MAKEINTRESOURCE(IDR_ALLRES));
	if( haccl )
	{
		ACCEL	accl[255];
		ZeroMemory( accl, sizeof(accl) );

		int nc = ::CopyAcceleratorTable(  haccl, accl, 255 );

		for( int i = 0; i < nc; i++ )
		{
			if( accl[i].cmd == id )
			{
				m_accl_key = accl[i].key;
				m_accl_virt = accl[i].fVirt;
			}
		}
		::DestroyAcceleratorTable( haccl );
	}
}


////////////////////////////////////////////////////////////////////////////////////////////////////////
//


CAction::CAction()
{
}
CAction::~CAction()
{
}

void CAction::init( IActionInfo2 *pAI )
{
	m_ptrAI = pAI;
}

arg* CAction::find( BSTR bstrArgName )
{
	TPOS	lpos = m_args.head();
	_bstr_t	bstr = bstrArgName;

	while( lpos )
	{
		arg	*parg = m_args.next( lpos );
		if( parg->arg_name == bstr )
			return parg;
	}

	arg	*parg = new arg;
	parg->arg_name = bstrArgName;
	m_args.insert( parg );

	return parg;
}

IUnknown *CAction::DoGetInterface( const IID &iid )
{
	if( iid == IID_IAction )return (IAction*)this;
	else if( iid == IID_IActionArgumentHelper )return (IActionArgumentHelper*)this;
	else if( iid == IID_INumeredObject )return (INumeredObject*)this;
	else return ComObjectBase::DoGetInterface( iid );
}
//IAction
HRESULT CAction::AttachTarget( IUnknown *punkTarget )
{
	m_ptrTarget = punkTarget;

	return S_OK;
}

HRESULT CAction::SetArgument( BSTR bstrArgName, VARIANT *pvarVal )
{
	arg	*parg = find( bstrArgName );
	parg->value = *pvarVal;

	if( parg->value.vt & VT_BYREF )
		::VariantChangeType( &parg->value, &parg->value, 0, VT_BSTR );

	return S_OK;
}
HRESULT CAction::SetResult( BSTR bstrArgName, VARIANT *pvarVal )
{
	arg	*parg = find( bstrArgName );
	parg->value = *pvarVal;

	return S_OK;
}

HRESULT CAction::Invoke()
{
	HRESULT hr = DoInvoke();

	m_ptrTarget = 0;
	return hr;
}

HRESULT CAction::GetActionState( DWORD *pdwState )
{
	*pdwState = 1;
	return S_OK;
}
HRESULT CAction::StoreParameters( BSTR *pbstr )
{
	*pbstr = 0;
	return S_OK;
}

HRESULT CAction::GetActionInfo(IUnknown** ppunkAI)
{
	*ppunkAI = m_ptrAI;
	if( *ppunkAI )(*ppunkAI)->AddRef();

	return S_OK;
}
HRESULT CAction::GetTarget( IUnknown **ppunkTarget )
{
	*ppunkTarget = m_ptrTarget;
	if( *ppunkTarget )(*ppunkTarget)->AddRef();

	return S_OK;
}
//IActionArgumentHelper
HRESULT CAction::SetArgumentHelper( BSTR bstrArgName, VARIANT *pvarVal )
{
	arg	*parg = find( bstrArgName );
	parg->value = *pvarVal;

	return S_OK;
}

HRESULT CAction::GetResult( BSTR bstrResName, VARIANT *pvarResult )
{
	arg	*parg = find( bstrResName );
	::VariantCopy( pvarResult, &parg->value );

	return S_OK;
}

double	CAction::GetArgDouble( const char *pszArgName )
{
	arg	*parg = find( _bstr_t( pszArgName ) );
	if( !parg )
		return 0;
	_variant_t	var;
	if( _VarChangeType( var, VT_R8, &parg->value ) != S_OK )
		return 0;
	return var.dblVal;
}

_bstr_t	CAction::GetArgString( const char *pszArgName )
{
	arg	*parg = find( _bstr_t( pszArgName ) );
	if( !parg )
		return "";
	_variant_t	var;
	if( _VarChangeType( var, VT_BSTR, &parg->value ) != S_OK )
		return "";
	return var.bstrVal;
}

long	CAction::GetArgLong( const char *pszArgName )
{
	arg	*parg = find( _bstr_t( pszArgName ) );
	if( !parg )
		return 0;
	_variant_t	var;
	if( _VarChangeType( var, VT_I4, &parg->value ) != S_OK )
		return 0;
	return var.lVal;
}

HRESULT CCustomizableActionImpl::CreateSettingWindow( HWND hWnd )
{
	return S_OK;
}

HRESULT CCustomizableActionImpl::GetFlags( DWORD *pdwFlags )
{
	return S_OK;
}
