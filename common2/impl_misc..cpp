#include "stdafx.h"
#include "impl_misc.h"


CNumeredObjectImpl::CNumeredObjectImpl()
{
	m_nCreateNo = -1;
	::CoCreateGuid( &m_key );
}

HRESULT CNumeredObjectImpl::AssignKey( GUID Key )
{
	m_key = Key;
	return S_OK;
}
HRESULT CNumeredObjectImpl::GetKey( GUID *pKey )
{
	*pKey = m_key;
	return S_OK;
}
HRESULT CNumeredObjectImpl::GenerateNewKey()
{
	::CoCreateGuid( &m_key );
	return S_OK;

}
HRESULT CNumeredObjectImpl::GetCreateNum( long *plNum )
{
	if( m_nCreateNo == -1 )
	{
		HKEY	hKey = 0;
		if( ::RegOpenKey( HKEY_LOCAL_MACHINE, "Software\\VideoTest\\ObjectCounter", &hKey ) )
			if( ::RegCreateKey( HKEY_LOCAL_MACHINE, "Software\\VideoTest\\ObjectCounter", &hKey ) )
				return E_INVALIDARG;
		DWORD	dwSize = 4;
		::RegQueryValueEx( hKey, 0, 0, 0, (byte*)&m_nCreateNo, &dwSize );
		m_nCreateNo++;
		::RegSetValueEx( hKey, 0, 0, REG_DWORD, (byte*)&m_nCreateNo, 4 );
		::RegCloseKey( hKey );
	}
	*plNum = m_nCreateNo;
	return S_OK;
}

HRESULT CNumeredObjectImpl::GenerateNewKey( GUID * pKey )
{
	::CoCreateGuid( &m_key );
	*pKey = m_key;
	return S_OK;
}
//named object
CNamedObjectImpl::CNamedObjectImpl()
{

}

HRESULT CNamedObjectImpl::GetName( BSTR *pbstr )
{
	*pbstr = m_bstrName.copy();
	return S_OK;
}

HRESULT CNamedObjectImpl::GetUserName( BSTR *pbstr )
{
	*pbstr = m_bstrUserName.copy();
	return S_OK;
}

HRESULT CNamedObjectImpl::SetName( BSTR bstr )
{
	m_bstrName = bstr;
	return S_OK;
}

HRESULT CNamedObjectImpl::SetUserName( BSTR bstr )
{
	m_bstrUserName = bstr;
	return S_OK;
}

HRESULT CNamedObjectImpl::GenerateUniqueName( BSTR bstrBase )
{
	_bstr_t	bstrObjectSection = bstrBase;
	long	lCount = 0;
	HKEY	hKey = 0;
	if( ::RegOpenKey( HKEY_LOCAL_MACHINE, "Software\\VideoTest\\ObjectCounter", &hKey ) )
		if( ::RegCreateKey( HKEY_LOCAL_MACHINE, "Software\\VideoTest\\ObjectCounter", &hKey ) )
			return E_INVALIDARG;
	DWORD	dwSize = 4;
	::RegQueryValueEx( hKey, bstrObjectSection, 0, 0, (byte*)&lCount, &dwSize );
	lCount++;
	::RegSetValueEx( hKey, 0, 0, REG_DWORD, (byte*)&lCount, 4 );
	::RegCloseKey( hKey );

	char	szName[100];
	::wsprintf( szName, "%s%d", (const char *)bstrObjectSection, lCount );
	m_bstrName = szName;

	return S_OK;
}
