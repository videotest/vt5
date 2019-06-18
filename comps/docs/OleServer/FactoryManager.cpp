// FactoryManager.cpp: implementation of the CFactoryManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "FactoryManager.h"
#include "misc_utils.h"
#include "registerserv.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CFactoryManager::CFactoryManager()
{
	m_bFactoryOpen			= false;

	Init();
}

//////////////////////////////////////////////////////////////////////
CFactoryManager::~CFactoryManager()
{
	DeInit();
}

//////////////////////////////////////////////////////////////////////
void CFactoryManager::Init()
{
	

	IVTApplicationPtr sptrApp = App::application();	
	BSTR bstr = 0;
	sptrApp->GetRegKey( &bstr );
	if( !bstr )
	{
		dbg_assert(false);
		return;
	}

	_bstr_t bstrSection = bstr;
	::SysFreeString( bstr );	bstr = 0;

	bstrSection += "\\";
	bstrSection += szServerFactorySection;


	HKEY	hKey = 0;
	if( ::RegOpenKey( HKEY_LOCAL_MACHINE, bstrSection, &hKey ) != ERROR_SUCCESS )
	{		
		return;
	}


	DWORD	dwSize = 255;
	char	sz[255];
	int	nCount = 0;

	DWORD dwType = REG_SZ;
	char  szValue[255];	
	DWORD dwValueSize = 255;



	_list_t2 <_bstr_t*> bstrProgIDList;	


	while( ::RegEnumValue( hKey, nCount, sz, &dwSize, 0, 
		(LPDWORD)&dwType, (LPBYTE)szValue, (LPDWORD)&dwValueSize ) == ERROR_SUCCESS )
	{
		
		bstrProgIDList.insert( new _bstr_t(sz) );

		dwSize = 255;
		dwType = REG_SZ;
		::ZeroMemory( &szValue, sizeof(szValue) );
		::ZeroMemory( &sz, sizeof(sz) );
		
		dwValueSize = 255;
		nCount++;		
	}

	::RegCloseKey( hKey );	hKey = 0;

	long lPos = bstrProgIDList.head();
	while( lPos )
	{

		_bstr_t* pbstrProgID = bstrProgIDList.next( lPos );

		//try to create instances		
		
		CLSID	clsid;
		::ZeroMemory( &clsid, sizeof(clsid) );

		if( ::CLSIDFromProgID( *pbstrProgID, &clsid  ) != S_OK )
			continue;

		IUnknown* punk = 0;

		HRESULT hres = E_FAIL;
		hres = ::CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punk );

		if( hres == S_OK && punk )
		{			
			if( !CheckInterface( punk, IID_IServerFactory ) )
				dbg_assert( false );
			m_comps.insert( punk );
		}
		


	}

	lPos = bstrProgIDList.head();
	while( lPos )
	{
		_bstr_t* pbstrProgID = bstrProgIDList.next( lPos );
		delete pbstrProgID;
	}

	bstrProgIDList.deinit();

	RegisterFactories();
	OpenFactories();

}


//////////////////////////////////////////////////////////////////////
void CFactoryManager::DeInit()
{

	CloseFactories();

	long lPos = m_comps.head();
	while( lPos )
	{
		IUnknown* punk = m_comps.next( lPos );
		IServerFactoryPtr ptr(punk);		
		punk->Release();
		
		if( ptr)
		{
			//long lCount = 0;
			//ptr->GetObjectRefCount( &lCount );
		}
		
	}

	m_comps.deinit();


}

//////////////////////////////////////////////////////////////////////
//IServerFactoryManager


HRESULT CFactoryManager::CanCloseServer( BOOL* pbCanClose )
{
	*pbCanClose = TRUE;

	long lObjectsCount = 0;
	long lServerLocks = 0;
	long lPos = m_comps.head();
	while( lPos )
	{
		IUnknown* punk = m_comps.next( lPos );
		IServerFactoryPtr ptr(punk);		
		if( ptr == 0 )
			continue;

		lObjectsCount = lServerLocks = 0;		
		
		ptr->GetObjectsCount( &lObjectsCount );
		ptr->GetServerLocks( &lServerLocks );

		if( lObjectsCount > 0 || lServerLocks > 0 )
		{
			*pbCanClose = FALSE;
			return S_OK;
		}

		
	}

	return S_OK;
}

HRESULT CFactoryManager::GetServerObjectsInfo( BSTR* pbstr )
{
	_bstr_t bstr_t;
	char szBuf[30];
	long lPos = m_comps.head();
	while( lPos )
	{
		IUnknown* punk = m_comps.next( lPos );
		IServerFactoryPtr ptr( punk );
		if( ptr == 0 )
			continue;		

		BSTR bstrComponentName = 0;
		BSTR bstrProgID = 0;
		ptr->GetRegisterInfo( NULL, &bstrComponentName, &bstrProgID, NULL );

		long lCount = 0;
		ptr->GetObjectsCount( &lCount );
		if( lCount > 0 )
		{

			bstr_t += "\nComponent:";	
			bstr_t += bstrComponentName;
			bstr_t += " ProgID:";
			bstr_t += bstrProgID;
			bstr_t += " Objects count:";
			::wsprintf( szBuf, "%d", lCount );
			bstr_t += szBuf;
			bstr_t += "\n\tReference list:";

			for( int i=0;i<lCount;i++ )
			{
				long lRef = 0;
				ptr->GetObjectRefCount( i, &lRef );
				::wsprintf( szBuf, "%d", lRef );
				bstr_t += szBuf;
				if( i != lCount - 1 )
					bstr_t += "; ";
			}
			
			ptr->GetServerLocks( &lCount );
			bstr_t += "\n\tLocks count:";
			bstr_t += szBuf;
			::wsprintf( szBuf, "%d", lCount );
		}

		::SysFreeString( bstrComponentName );	bstrComponentName = 0;
		::SysFreeString( bstrProgID );			bstrProgID = 0;		

		

	}

	*pbstr = bstr_t.copy();

	return S_OK;
}



//////////////////////////////////////////////////////////////////////
HRESULT CFactoryManager::InitApp( )
{
	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CFactoryManager::ShowApp( BOOL bShow )
{
	return S_OK;
}


//////////////////////////////////////////////////////////////////////
HRESULT CFactoryManager::OpenFactories()
{
	if( m_bFactoryOpen )
	{
		dbg_assert( false );
		return S_FALSE;
	}

	long lPos = m_comps.head();
	while( lPos )
	{
		IUnknown* punk = m_comps.next( lPos );

		IServerFactoryPtr ptr( punk );	

		if( ptr )
		{			
			ptr->OpenFactory();
		}
	}

	m_bFactoryOpen = true;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CFactoryManager::CloseFactories()
{
	if( !m_bFactoryOpen )
	{		
		return S_FALSE;
	}

	long lPos = m_comps.head();
	while( lPos )
	{
		IUnknown* punk = m_comps.next( lPos );
		IServerFactoryPtr ptr( punk );		

		if( ptr )
		{			
			ptr->CloseFactory();			
		}
	}

	m_bFactoryOpen = false;

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CFactoryManager::RegisterFactories()
{
	long lPos = m_comps.head();
	while( lPos )
	{
		IUnknown* punk = m_comps.next( lPos );
		IServerFactoryPtr ptr( punk );		

		if( ptr )
		{
			CLSID clsid;
			::ZeroMemory( &clsid, sizeof(CLSID) );
			BSTR bstrComponentName = 0;
			BSTR bstrProgID = 0;
			BSTR bstrProgIDVer = 0;
			if( S_OK == ptr->GetRegisterInfo( &clsid, &bstrComponentName, &bstrProgID, &bstrProgIDVer ) )
				dbg_assert( RegisterServer( clsid, bstrComponentName, bstrProgID, bstrProgIDVer) == TRUE );

			if( bstrComponentName )
				::SysFreeString( bstrComponentName );	bstrComponentName = 0;

			if( bstrProgID )
				::SysFreeString( bstrProgID );			bstrProgID = 0;

			if( bstrProgIDVer )
				::SysFreeString( bstrProgIDVer );		bstrProgIDVer = 0;


			//ptr->RegisterFactory();
		}
	}

	return S_OK;
}

//////////////////////////////////////////////////////////////////////
HRESULT CFactoryManager::UnRegisterFactories()
{
	long lPos = m_comps.head();
	while( lPos )
	{
		IUnknown* punk = m_comps.next( lPos );
		IServerFactoryPtr ptr( punk );		

		if( ptr )
		{
			CLSID clsid;
			::ZeroMemory( &clsid, sizeof(CLSID) );
			BSTR bstrComponentName = 0;
			BSTR bstrProgID = 0;
			BSTR bstrProgIDVer = 0;
			ptr->GetRegisterInfo( &clsid, &bstrComponentName, &bstrProgID, &bstrProgIDVer );

			dbg_assert( UnRegisterServer( clsid, bstrComponentName, bstrProgID, bstrProgIDVer) == TRUE );

			if( bstrComponentName )
				::SysFreeString( bstrComponentName );	bstrComponentName = 0;

			if( bstrProgID )
				::SysFreeString( bstrProgID );			bstrProgID = 0;

			if( bstrProgIDVer )
				::SysFreeString( bstrProgIDVer );		bstrProgIDVer = 0;
		}
	}

	return S_OK;
}
					  

// Server Registrator
//////////////////////////////////////////////////////////////////////
BOOL CFactoryManager::RegisterServer( CLSID clsid, BSTR bstrComponentName, BSTR bstrProgID, BSTR bstrProgIDVer )
{
	return ::_RegisterServer( clsid, _bstr_t(bstrComponentName), _bstr_t(bstrProgID), _bstr_t(bstrProgIDVer) );	 
}

//////////////////////////////////////////////////////////////////////
BOOL CFactoryManager::UnRegisterServer( CLSID clsid, BSTR bstrComponentName, BSTR bstrProgID, BSTR bstrProgIDVer )
{
	return ::_UnRegisterServer( clsid, _bstr_t(bstrComponentName), _bstr_t(bstrProgID), _bstr_t(bstrProgIDVer) );	
}

