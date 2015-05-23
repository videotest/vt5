#include "StdAfx.h"
#include "resource.h"
#include "statisticobjectfilter.h"

#include "statistics.h"
#include "\vt5\com_base\stream_misc.h"

namespace ObjectSpace
{
CStatisticObjectFilter::CStatisticObjectFilter(void)
{
	m_bstrDocumentTemplate = _bstr_t( "Image" );
	char szFilterName[255];

	::LoadString(App::handle(), IDS_STAT_OBJECT_FILTERNAME, szFilterName, 255);
	
	m_bstrFilterName = szFilterName;
	m_bstrFilterExt = _bstr_t( ".stdata" );
	
	AddDataObjectType( szTypeStatObject );	
}

CStatisticObjectFilter::~CStatisticObjectFilter(void)
{
}

//////////////////////////////////////////////////////////////////////
bool CStatisticObjectFilter::ReadFile( const char *pszFileName )
{
	if( !pszFileName )	
		return false;

	TCHAR sz_name[_MAX_PATH] = {0};
	TCHAR sz_drive[_MAX_DRIVE] = {0};
	TCHAR sz_dir[_MAX_PATH] = {0};
	TCHAR sz_ext[_MAX_PATH] = {0};

	_splitpath( pszFileName, sz_drive, sz_dir, sz_name, sz_ext );
	
	IUnknown* punk = CreateNamedObject( szTypeStatObject );

	_bstr_t bstr_name( sz_name );
	long lname_exists = 0;
	
	if( m_sptrINamedData )
		m_sptrINamedData->NameExists( bstr_name, &lname_exists );

	//set name
	if( !lname_exists && punk )
	{
		INamedObject2Ptr ptr_no = punk;
		if( ptr_no )
			ptr_no->SetName( bstr_name );
	}

	sptrISerializableObject	ptr_so = punk;
	punk->Release();
	punk = 0;

	if( ptr_so == 0 )
		return false;

	stream_file file;
	
	if( file.open( pszFileName ) != S_OK )
		return false;

	SerializeParams	sp = {0};
    return ( ptr_so->Load( &file, &sp ) == S_OK );
}

//////////////////////////////////////////////////////////////////////
bool CStatisticObjectFilter::WriteFile( const char *pszFileName )
{
	IUnknown* punk = 0;
	
	// convert object type to BSTR
	_bstr_t bstrType( GetDataObjectType(0) );

	// get active object in which type == m_strObjKind	
	if( !( m_bObjectExists && m_sptrAttachedObject != 0 ) && m_sptrIDataContext != 0 )
		m_sptrIDataContext->GetActiveObject( bstrType, &punk );
	else
	{
		punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
		punk->AddRef();
	}

	if( !punk )
		return false;

	sptrISerializableObject	ptr_so = punk;
	punk->Release();
	punk = 0;

	if( ptr_so == 0 )
		return false;

	stream_file file;
	if( file.open( pszFileName ) != S_OK )
		return false;

	SerializeParams	sp = {0};
    return ( ptr_so->Store( &file, &sp ) == S_OK );
}

//////////////////////////////////////////////////////////////////////
bool CStatisticObjectFilter::_InitNew()
{
	return true;
}
}