#include "stdafx.h"
#include "compmanager.h"
#include "registry.h"
#include "utils.h"
#include "misc5.h"

CCompManager::CCompManager( const char *szGroupName, IUnknown *punkParent, bool bLoadComponents )
{
	m_punkOuterUnknown = 0;
	m_punkParent = 0;

	if( szGroupName )
		AttachComponentGroup( szGroupName );

	m_punkParent = punkParent;

	if( m_punkParent )
		m_punkParent->AddRef();

	Init();
}

CCompManager::~CCompManager()
{
	DeInit();
}

void CCompManager::AttachComponentGroup( const char *szGroupRecord )
{
	DeInit();

	m_sGroupName = szGroupRecord;
}

void CCompManager::SetRootUnknown( IUnknown * punkParent )
{
	if( punkParent )
		punkParent->AddRef();
	if( m_punkParent )
		m_punkParent->Release();

	m_punkParent = punkParent;
}

void CCompManager::SetOuterUnknown( IUnknown *punkOuterUnknown )
{
	/*if( punkOuterUnknown )
		punkOuterUnknown->AddRef();
	if( m_punkOuterUnknown )
		m_punkOuterUnknown->Release();*/
	m_punkOuterUnknown = punkOuterUnknown;
}

bool CCompManager::Init()
{
	CString			str = m_sGroupName;
	CStringArray	sa;

	while( !str.IsEmpty() )
	{
		int	idx = str.Find( '\n' );

		CString	strSection;

		if( idx == -1 )
		{
			strSection = str;
			str.Empty();
		}
		else
		{
			strSection = str.Left( idx );
			str = str.Right( str.GetLength()-idx-1 );
		}

		sa.Add( strSection );
	}

	for( int i = 0; i < sa.GetSize(); i++ )
	{
		CCompRegistrator	registrator( sa[i] );

		DWORD	dwSize = 255;
		char	sz[255];

		for( int i = 0; ::RegEnumValue( registrator.m_hKey, i, sz, &dwSize, 0, 0, 0, 0 )==0; i++ )
		{	
			dwSize = 255;
			CString	s = sz;

			BSTR	bstr = s.AllocSysString();
			CLSID	clsid;

			if( ::CLSIDFromProgID( bstr, &clsid  ) != S_OK )
				continue;

			IUnknown	*punk = 0;

			::SysFreeString( bstr );


			HRESULT hresult = ::CoCreateInstance( clsid, m_punkOuterUnknown, CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punk );
			
			if( !TraceIfFailed( hresult ) )
				continue;

	//		punk->AddRef();
			ASSERT( punk );
			m_ptrs.Add( punk );
			m_strs.Add( sz );

			IInitializeObjectVTPtr	ptrI( punk );
			if( ptrI != 0 )ptrI->Initialize();

			if( m_punkParent && CheckInterface( punk, IID_IRootedObject ) )
			{
				IRootedObject *punkR = 0;
				punk->QueryInterface( IID_IRootedObject, (void **)&punkR );

				punkR->AttachParent( m_punkParent );
				punkR->Release();
			}
		}
	}
	return true;

}

void CCompManager::DeInit()
{
	FreeComponents();

	if( m_punkParent )
		m_punkParent->Release();
	/*if( m_punkOuterUnknown )
		m_punkOuterUnknown->Release();*/
	m_punkOuterUnknown = 0;
	m_punkParent = 0;

	m_sGroupName.Empty();
}

void CCompManager::FreeComponents()
{
	for( int i = GetComponentCount()-1; i >= 0; i-- )
	{
		IUnknown	*punk = (IUnknown	*)m_ptrs[i];

//		TRACE( "%s ", GetObjectName( punk ) );
//		_ReportCounter( punk );
		punk->Release();
	}
	m_ptrs.RemoveAll();
}

void CCompManager::RemoveAllComponents()
{
	for( int i = GetComponentCount() - 1; i >= 0; i-- )
	{
		RemoveComponent(i);
	}
	m_ptrs.RemoveAll();
}

bool CCompManager::LoadComponentsDefault()
{
	CString			str = m_sGroupName;
	CStringArray	sa;

	while (!str.IsEmpty())
	{
		int	idx = str.Find('\n');

		CString	strSection;

		if (idx == -1)
		{
			strSection = str;
			str.Empty();
		}
		else
		{
			strSection = str.Left(idx);
			str = str.Right(str.GetLength() - idx - 1);
		}
		sa.Add(strSection);
	}

	for (int i = 0; i < sa.GetSize(); i++)
	{
		CCompRegistrator	registrator(sa[i]);

		DWORD	dwSize = 255;
		char	sz[255];

		for( int i = 0; ::RegEnumValue( registrator.m_hKey, i, sz, &dwSize, 0, 0, 0, 0 )==0; i++ )
		{	
			dwSize = 255;
			CString	s = sz;

			BSTR	bstr = s.AllocSysString();
			CLSID	clsid;

			if (::CLSIDFromProgID(bstr, &clsid) != S_OK)
				continue;

			IUnknown	*punk = 0;

			::SysFreeString(bstr);


			HRESULT hresult = ::CoCreateInstance(clsid, m_punkOuterUnknown, CLSCTX_INPROC_SERVER, IID_IUnknown, (void **)&punk);
			
			if (!TraceIfFailed(hresult))
				continue;

			AddComponent(punk);
			punk->Release();

			IInitializeObjectVTPtr	ptrI( punk );
			if( ptrI != 0 )ptrI->Initialize();

			if (m_punkParent && CheckInterface(punk, IID_IRootedObject))
			{
				IRootedObject *punkR = 0;
				punk->QueryInterface(IID_IRootedObject, (void **)&punkR);

				punkR->AttachParent(m_punkParent);
				punkR->Release();
			}
		}
	}
	return true;
}

const char *CCompManager::GetComponentName( int nCompIdx )
{
	return m_strs[nCompIdx];
}

IUnknown *CCompManager::GetComponent( int iCompIdx, bool bAddRef )
{
	IUnknown	*punk = (IUnknown	*)m_ptrs[ iCompIdx ];

	if( bAddRef )
		punk->AddRef();

	return punk;
}

IUnknown *CCompManager::GetComponent( const IID iid, bool bAddRef )
{
	for( int i = 0; i < GetComponentCount(); i++ )
	{
		IUnknown	*punk = (IUnknown	*)m_ptrs[i];

		if( CheckInterface( punk, iid ) )
		{
			if( bAddRef )
				punk->AddRef();
			return punk;
		}
	}
	return 0;
}

IUnknown *CCompManager::GetComponent( const char *szCompName, bool bAddRef )
{
	for( int i = 0; i < GetComponentCount(); i++ )
	{
		IUnknown	*punk = (IUnknown	*)m_ptrs[i];

		if( CheckInterface( punk, IID_INamedObject2 ) )
		{
			INamedObject2Ptr	sptrN( punk );

			BSTR	bstrName = 0;
			sptrN->GetName( &bstrName );

			CString	strName( bstrName );
			::SysFreeString( bstrName );

			if( strName == szCompName )
			{
				if( bAddRef )
					punk->AddRef();
				return punk;
			}
		}
	}
	return 0;
}


int CCompManager::GetComponentCount()
{
	return m_ptrs.GetSize();
}

int CCompManager::AddComponent( IUnknown *punk )
{
	if( !punk )
		return -1;
	punk->AddRef();
	int idx = m_ptrs.Add( punk );
	m_strs.Add( "qq");

	OnAddComponent( idx );

	return idx;
}

void CCompManager::RemoveComponent( int idx )
{
	if( idx < 0 || idx >= GetComponentCount() )
		return;

	OnRemoveComponent( idx );

	IUnknown	*p = (IUnknown	*)m_ptrs[idx];
	p->Release();

	m_ptrs.RemoveAt( idx );
	AfterRemoveComponent( idx );
}


IUnknown *CCompManager::operator[](int idx)
{
	if( idx < 0 || idx >= GetComponentCount() )
		return 0;
	return (IUnknown	*)m_ptrs[idx];
}

//CAggrManager

CAggrManager::CAggrManager()
{
	for( int i = 0; i < MAX_AGGR; i++ )
		m_pinterfaces[i] = 0;
}

CAggrManager::~CAggrManager()
{
	DeInit();
}

bool CAggrManager::Init()
{
	if( !CCompManager::Init() )
		return false;

	for( int i = 0; i < MAX_AGGR; i++ )
		m_pinterfaces[i] = (*this)[i];

	return true;
}

void CAggrManager::DeInit()
{
	for( int i = 0; i < MAX_AGGR; i++ )
		m_pinterfaces[i] = 0;

	CCompManager::DeInit();
}

void CAggrManager::OnAddComponent( int idx )
{
	if( idx < MAX_AGGR )
		m_pinterfaces[idx] = (*this)[idx];
}

void CAggrManager::AfterRemoveComponent( int idx )
{
	for( int i = 0; i < MAX_AGGR; i++ )
		m_pinterfaces[i] = NULL;

	for( i = 0; i < MAX_AGGR; i++ )
		m_pinterfaces[i] = (*this)[i];

	
}
