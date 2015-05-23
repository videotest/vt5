#ifndef __action_main_h__
#define __action_main_h__

#include "action5.h"
#include "com_unknown.h"
#include "misc_list.h"
#include "impl_misc.h"
#include "misc_templ.h"
#include "impl_help.h"


///////////////////////////////////////////////////////////////////////////////
//CBitmapProvider
//class for getting toolbar information about action from resource
class CBitmapProvider: public ComObjectBase,
						public _static_t<CBitmapProvider>,
						public IBitmapProvider,
						public CNumeredObjectImpl
{
	public:																						
com_call_ref AddRef()									{return _addref();}					
com_call_ref Release()									{return _release();}				
com_call QueryInterface( const IID &iid, void **pret )	{return _qi( iid, pret );}
public:
	CBitmapProvider();
	~CBitmapProvider();
public:
	virtual IUnknown *DoGetInterface( const IID &iid )
	{
		if( iid == IID_IBitmapProvider )return	(IBitmapProvider*)this;
		if( iid == IID_INumeredObject)return	(INumeredObject*)this;
		else return ComObjectBase::DoGetInterface( iid );
	}

	com_call GetBitmapCount( long *piCount );
	com_call GetBitmap( int idx, HANDLE *phBitmap );
	com_call GetBmpIndexFromCommand( UINT nCmd, DWORD dwDllCode, long *plIndex, long *plBitmap );
	int	FindCommand( uint n );
protected:
	_ptr_t2<DWORD>	m_cmds;
	_gdi_t<HBITMAP>	m_bitmap;

	static CBitmapProvider	*s_pinst;
};

//the IDR_ALLRES is common resource id for toolbar and accellerator resources
#ifndef IDR_ALLRES
#define IDR_ALLRES	1000
#endif	//IDR_ALLRES

class CAction;
class CActionUI;

class _ainfo_base : public ComObjectBase,
					public IActionInfo3,
					public CNumeredObjectImpl,
					public CHelpInfoImpl
{
public:
	struct arg
	{	char	*pszArgName;	
		char	*pszArgType;
		char	*pszDefValue;
		bool	bArg;
		bool	bDataObject;
	};
public:
	_ainfo_base();
	virtual ~_ainfo_base();

	virtual IUnknown *DoGetInterface( const IID &iid );
//IActionInfo
	com_call GetArgsCount( int *piParamCount );
	com_call GetArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, BSTR *pbstrDefValue );
	com_call GetOutArgsCount( int *piParamCount );
	com_call GetOutArgInfo( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind );
	com_call IsFunction( BOOL *pboolAction );
	com_call GetCommandInfo( 
		//general information
		BSTR	*pbstrActionName,
		BSTR	*pbstrActionUserName,
		BSTR	*pbstrActionHelpString, //"Image rotation\nRotation
		BSTR	*pbstrGroupName	);
	com_call GetTBInfo(
			//toolbar information
			BSTR	*pbstrDefTBName,	//default toolbar name (as "edit operaion")
			IUnknown **ppunkBitmapHelper );
		//menu information
	com_call GetMenuInfo(
		BSTR	*pbstrMenuNames,	//"general\\File\\Open", "main\\Tools\\Image tranphormation\\Gamma correction", ...
		DWORD	*pdwMenuFlag );		//not used, must be zero. Will be used later, possible, 
	com_call GetHotKey( DWORD *pdwKey, BYTE *pbVirt );
	com_call GetRuntimeInformaton( DWORD *pdwDllCode, DWORD *pdwCmdCode );
	com_call ConstructAction( IUnknown **ppunk );

	com_call GetTargetName( BSTR *pbstr );
	com_call GetLocalID( int *pnID );
	com_call SetLocalID( int nID );
//IActionInfo2
	com_call IsArgumentDataObject( int iParamIdx, BOOL bInArg, BOOL *pbIs );
	com_call GetArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKnid, BSTR *pbstrDefValue, int* pNumber );
	com_call GetOutArgInfo2( int iParamIdx, BSTR *pbstrName, BSTR *pbstrKind, int* pNumber );
	com_call GetUIUnknown( IUnknown **ppunkinterface );	
	com_call SetActionString( BSTR bstr );
//  [vanek] : IActionInfo3 - 13.12.2004
	com_call SetBitmapHelper( IUnknown *punkBitmapHelper );
public:
	int		count();

	virtual arg	*args()					= 0;
	virtual _bstr_t target()			= 0;
	virtual CAction		*create()		= 0;
	virtual CActionUI	*create_ui()	= 0;
	virtual uint resource()				= 0;
	void	ensure_initialized();
	void	load( const char *psz, bool bAliase );

	uint	m_id;
	bool	m_ready;
	//int		m_bitmap;

	dword	m_accl_key;
	byte	m_accl_virt;

	_bstr_t	m_name;
	_bstr_t	m_user_name;
	_bstr_t	m_group;
	_bstr_t	m_help;
	_bstr_t	m_tooltip;

	// [vanek] : support IActionInfo3 interface - 13.12.2004
	IUnknownPtr	m_spunkBmpHelper;
};
							  
template<int resource_id, 
	void *(*pfnCA)(), 
	_ainfo_base::arg	*pargs = 0,	
	void *(*pfnUI)() = 0>
class _ainfo_t : public _ainfo_base
{
public:
	virtual _bstr_t target()			{return "app";}
	virtual uint resource()				{return resource_id;}
	virtual arg		*args()				{return pargs;}
	virtual CAction		*create()		{return (CAction*)pfnCA();}
	virtual CActionUI	*create_ui()	{if( !pfnUI )return 0;return (CActionUI*)pfnUI();}
};

struct arg
{
	_bstr_t		arg_name;
	_variant_t	value;
};


class CCustomizableActionImpl : public ICustomizableAction
{
	com_call CreateSettingWindow( HWND hWnd );
	com_call GetFlags( DWORD *pdwFlags );
};

inline void FreeArg( void *ptr )
{delete (arg*)ptr;}

class CAction : public ComObjectBase,
				public IAction,
				public CNumeredObjectImpl
{
public:
	CAction();
	virtual ~CAction();

	void	init( IActionInfo2 *pAI );
	arg*	find( BSTR bstrArgName );
public:
	virtual IUnknown *DoGetInterface( const IID &iid );

	double	GetArgDouble( const char *pszArgName );
	_bstr_t	GetArgString( const char *pszArgName );
	long	GetArgLong( const char *pszArgName );
//IAction
	com_call AttachTarget( IUnknown *punkTarget );
	com_call SetArgument( BSTR bstrArgName, VARIANT *pvarVal );
	com_call SetResult( BSTR bstrArgName, VARIANT *pvarVal );

	com_call Invoke();

	com_call GetActionState( DWORD *pdwState );
	com_call StoreParameters( BSTR *pbstr );

	com_call GetActionInfo(IUnknown** ppunkAI);
	com_call GetTarget( IUnknown **ppunkTarget );
//IActionArgumentHelper
	com_call SetArgumentHelper( BSTR bstrArgName, VARIANT *pvarArg );
	com_call GetResult( BSTR bstrResName, VARIANT *pvarResult );

	com_call DoInvoke() = 0;

	IUnknownPtr		m_ptrTarget;
	IActionInfo2Ptr	m_ptrAI;

	_list_ptr_t<arg*, FreeArg>m_args;
};

class CActionUI : public ComObjectBase
{
};


class export_data CFilterErrorInfo
{
};




#endif //__action_main_h__