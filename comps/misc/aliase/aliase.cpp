// aliase.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "misc_const.h"
#include "aliaseint.h"
#include "misc_utils.h"
#include "action5.h"
#include "data5.h"

//#ifdef _DEBUG
//#pragma comment (lib, "\\vt5\\common2\\debug\\common2.lib")
//#else
//#pragma comment (lib, "\\vt5\\common2\\release\\common2.lib")
//#endif //_DEBUG#if 


//using namespace std;


class CAliaseMan : public ComAggregableBase,
					public _dyncreate_t<CAliaseMan>,
					public IAliaseMan2
{
	bool m_bReadOnly;
	route_unknown();
public:
	CAliaseMan();
	virtual ~CAliaseMan();

	void Init();
	void DeInit();
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	com_call LoadFile( BSTR bstrFileName );
	com_call StoreFile( BSTR bstrFileName );

	com_call GetActionString( BSTR bstrAction, BSTR *pbstrString );
	com_call GetObjectString( BSTR bstrObject, BSTR *pbstrString );

	com_call IsReadOnly()
	{
		if( m_bReadOnly )
			return S_OK;

		return S_FALSE;
	}

	com_call GetActionArgumentString( BSTR bstrAction, BSTR bstrArgument, BSTR *pbstrString );

	typedef std::map<_bstr_t, _bstr_t>	bstr2bstr;
	bstr2bstr	m_aliaseActions;
	bstr2bstr	m_aliaseObjects;
	bstr2bstr	m_aliaseArguments;
	bool		m_bInitialized;
};

/////////////////////////////////////////////////////////////////////////////////////////
CAliaseMan::CAliaseMan()
{
	m_bInitialized = false;
	m_bReadOnly = false;
}

CAliaseMan::~CAliaseMan()
{
	DeInit();
}

IUnknown *CAliaseMan::DoGetInterface( const IID &iid )
{
	if( iid == IID_IAliaseMan2 )return (IAliaseMan2*)this;
	else if( iid == IID_IAliaseMan )return (IAliaseMan*)this;
	else return ComObjectBase::DoGetInterface( iid );
}

void CAliaseMan::Init()
{
	DeInit();

	m_bInitialized = true;

	if( !m_bReadOnly )
	{
		IUnknown	*punk = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
		ICompManagerPtr	ptrCompMan( punk );
		::dbg_assert( ptrCompMan!=0 );
		punk->Release();

		int	nCount, n;
		ptrCompMan->GetCount( &nCount );

		for( n = 0; n < nCount; n++ )
		{
			IUnknown	*punkAction = 0;
			ptrCompMan->GetComponentUnknownByIdx( n, &punkAction );
			IActionInfoPtr	ptrAI( punkAction );
			if( punkAction )punkAction->Release();


			if( ptrAI == 0 )continue;

			BSTR	bstrAction, bstrUserName, bstrActionHelpString, bstrGroupName;
			ptrAI->GetCommandInfo( &bstrAction, &bstrUserName, &bstrActionHelpString, &bstrGroupName );

			_bstr_t	bstrA = bstrAction;
			_bstr_t	bstrS = bstrUserName;

			bstrS+="#";
			bstrS+=bstrGroupName;
			bstrS+="#";
			bstrS+=bstrActionHelpString;
			

			m_aliaseActions[bstrA]=bstrS;

			::SysFreeString( bstrAction );
			::SysFreeString( bstrUserName );
			::SysFreeString( bstrActionHelpString );
			::SysFreeString( bstrGroupName );
		}
	}

	long	lCount, l;
	IDataTypeInfoManagerPtr	ptrTypeMan( GetAppUnknown() );
	ptrTypeMan->GetTypesCount( &lCount );

	for( l = 0; l < lCount; l++ )
	{
		
		INamedDataInfo *pi = 0;
		ptrTypeMan->GetTypeInfo( l, &pi );
		if( pi )
		{
			INamedObject2Ptr	ptrNamed( pi );
			pi->Release();

			BSTR	bstr, bstrU;
			ptrNamed->GetUserName( &bstrU );
			ptrNamed->GetName( &bstr );
			m_aliaseObjects[bstr] = bstrU;
			::SysFreeString( bstr );
			::SysFreeString( bstrU );
		}

		
	}

}

void CAliaseMan::DeInit()
{
	m_aliaseObjects.empty();
	m_aliaseActions.empty();	
	m_aliaseArguments.empty();	

	m_bInitialized = false;
}

const char szActions[] = "[Actions]";
const char szObjects[] = "[Objects]";
const char szActionArguments[] = "[ActionArguments]";
HRESULT CAliaseMan::LoadFile( BSTR bstrFileName )
{
	_bstr_t	strFileName( bstrFileName );

	{
		FILE	*_pfile = ::fopen( strFileName, "at" );

		if( !_pfile )
			m_bReadOnly = true;
		else
			fclose( _pfile );
	}

	Init();

	FILE	*pfile = ::fopen( strFileName, "rt" );
	char	sz[255];

	if( !pfile )return S_FALSE;


	int	nLoadMode = 0;

	while( !feof( pfile ) )
	{
		sz[0] = 0;
		::fgets( sz, 255, pfile );

		int	nLen = strlen( sz );

		if( !nLen )continue;
		if( sz[nLen-1] == '\n' )sz[nLen-1] = 0;
		if( sz[0] == ';' )continue;
		if( sz[0] == '[' )
		{
			if( !strcmp( sz, szActions ) )nLoadMode = 1;
			else if( !strcmp( sz, szObjects ) )nLoadMode = 2;
			else if( !strcmp( sz, szActionArguments ) )nLoadMode = 3;
			else nLoadMode = 0;
			continue;
		}

		if( !nLoadMode )
			continue;
		char	*pszVal = strchr( sz, '=' );
		if( !pszVal )
		{
			if( nLoadMode == 1 )
				m_aliaseActions[sz] = "";
			if( nLoadMode == 3 )
				m_aliaseArguments[sz] = "";
			
			continue;
		}

		*pszVal = 0;
		pszVal++;

		if( nLoadMode == 1 )
			m_aliaseActions[sz]=pszVal;
		else if( nLoadMode == 2 )
			m_aliaseObjects[sz]=pszVal;
		else if( nLoadMode == 3 )
			m_aliaseArguments[sz]=pszVal;
	}

	::fclose( pfile );


	long	lCount, l;
	IDataTypeInfoManagerPtr	ptrTypeMan( GetAppUnknown() );
	ptrTypeMan->GetTypesCount( &lCount );

	for( l = 0; l < lCount; l++ )
	{
		
		INamedDataInfo *pi = 0;
		ptrTypeMan->GetTypeInfo( l, &pi );
		if( pi )
		{
			INamedObject2Ptr	ptrNamed( pi );
			pi->Release();

			BSTR	bstr;

			ptrNamed->GetName( &bstr );
			bstr2bstr::iterator r = m_aliaseObjects.find( bstr );
			if( r != m_aliaseObjects.end() )
				ptrNamed->SetUserName( (*r).second );
			::SysFreeString( bstr );
		}
	}

	return S_OK;
}

HRESULT CAliaseMan::StoreFile( BSTR bstrFileName )
{
	_bstr_t	strFileName( bstrFileName );

	FILE	*pfile = ::fopen( strFileName, "wt" );
	char	sz[255];

	if( !pfile )return S_OK;

	fputs( "VideoTest aliases file\n", pfile );
	fputs( "Format:\n", pfile );
	fputs( "Entry=Value\n", pfile );
	fputs( "New line delimiter is '#'\n", pfile );

	fputs( "\n", pfile );

	fputs( szActions, pfile );

	fputs( "\n", pfile );

	char	*pszNewLine;

	bstr2bstr::iterator r = m_aliaseActions.begin();
	while( r != m_aliaseActions.end() )
	{
		if( ( (*r).first ).length() )
		{
			if( ( (*r).second ).length() )
				_snprintf( sz, sizeof( sz ), "%s=%s", (const char*)(*r).first, (const char*)(*r).second );
			else
				_snprintf( sz, sizeof( sz ), "%s", (const char*)(*r).first );

			while( pszNewLine=strchr( sz, '\n' ) )*pszNewLine = '#';
			fputs( sz, pfile );
			fputs( "\n", pfile );
		}
		r++;
	}
	
	fputs( "\n", pfile );
	fputs( szObjects, pfile );
	fputs( "\n", pfile );

	r = m_aliaseObjects.begin();
	while( r != m_aliaseObjects.end() )
	{
		_snprintf( sz, sizeof( sz ), "%s=%s", (const char*)(*r).first, (const char*)(*r).second );
		fputs( sz, pfile );
		fputs( "\n", pfile );
		r++;
	}

	fputs( "\n", pfile );
	fputs( szActionArguments, pfile );
	fputs( "\n", pfile );

	r = m_aliaseArguments.begin();
	while( r != m_aliaseArguments.end() )
	{
		_snprintf( sz, sizeof( sz ), "%s=%s", (const char*)(*r).first, (const char*)(*r).second );
		fputs( sz, pfile );
		fputs( "\n", pfile );
		r++;
	}

	::fclose( pfile );

	return S_OK;
}

HRESULT CAliaseMan::GetActionString( BSTR bstrAction, BSTR *pbstrString )
{
	if( !m_bInitialized )
		Init();

	_bstr_t	strAction = bstrAction;
	bstr2bstr::iterator r = m_aliaseActions.find( strAction );

	if( r != m_aliaseActions.end() )
	{
		_bstr_t	bstr = (*r).second;
		if( bstr.length() )
		{
			*pbstrString = bstr.copy();
			return S_OK;
		}
		return S_FALSE;
	}
	else
	{
		IUnknown	*punk = _GetOtherComponent( GetAppUnknown(), IID_IActionManager );
		IActionManagerPtr	ptrAM( punk );
		::dbg_assert( ptrAM!=0 );
		punk->Release();

		IUnknown	*punkAction = 0;
		ptrAM->GetActionInfo( strAction, &punkAction );

		if( punkAction != 0 )
		{
			IActionInfoPtr	ptrAI( punkAction );

			BSTR	bstrAction, bstrUserName, bstrActionHelpString, bstrGroupName;
			ptrAI->GetCommandInfo( &bstrAction, &bstrUserName, &bstrActionHelpString, &bstrGroupName );

			_bstr_t	bstrA = bstrAction;
			_bstr_t	bstrS = bstrUserName;

			bstrS+="#";
			
			if( !m_bReadOnly )
				bstrS += bstrGroupName;
			else
				bstrS += _bstr_t( "---" );

			bstrS+="#";
			bstrS+=bstrActionHelpString;
			

			m_aliaseActions[bstrA]=bstrS;
			*pbstrString = bstrS.copy();

			::SysFreeString( bstrAction );
			::SysFreeString( bstrUserName );
			::SysFreeString( bstrActionHelpString );
			::SysFreeString( bstrGroupName );

			punkAction->Release();
		}
		return S_OK;
	}
}

HRESULT CAliaseMan::GetObjectString( BSTR bstrObject, BSTR *pbstrString )
{
	if( !m_bInitialized )
		Init();

	bstr2bstr::iterator r = m_aliaseObjects.find( bstrObject );

	if( r != m_aliaseObjects.end() )
	{
		*pbstrString = (*r).second.copy();
		return S_OK;
	}
	else
	{
		*pbstrString = 0;
		return E_INVALIDARG;
	}
}

HRESULT CAliaseMan::GetActionArgumentString( BSTR bstrAction, BSTR bstrArgument, BSTR *pbstrString )
{
	if( !m_bInitialized )
		Init();

	_bstr_t bstr = bstrAction;
	bstr += ":";
	bstr += bstrArgument;

	bstr2bstr::iterator r = m_aliaseArguments.find( bstr );

	if( r != m_aliaseArguments.end() )
	{
		*pbstrString = (*r).second.copy();
		return S_OK;
	}
	else
	{
		*pbstrString = 0;
		return E_INVALIDARG;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////
#define pszAliaseManProgID	"Aliases.AliaseMan"
// {E5B1BA04-E5AE-4839-83C0-2798561EF14A}
static const GUID clsidAliaseMan = 
{ 0xe5b1ba04, 0xe5ae, 0x4839, { 0x83, 0xc0, 0x27, 0x98, 0x56, 0x1e, 0xf1, 0x4a } };

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if( ul_reason_for_call == DLL_PROCESS_ATTACH )
	{
		_dyncreate_t<App>::CreateObject();
		App::instance()->Init( (HINSTANCE)hModule );

		App::instance()->ObjectInfo( _dyncreate_t<CAliaseMan>::CreateObject, clsidAliaseMan, pszAliaseManProgID, pszAppAggrSection );

		ClassFactory * pfactory = (ClassFactory*)ClassFactory::CreateObject();
		pfactory->LoadObjectInfo();
		pfactory->Release();
	}
	else if( ul_reason_for_call == DLL_PROCESS_DETACH )
	{
		App::instance()->Deinit();
		App::instance()->Release();
	}
    return TRUE;
}


implement_com_exports();
