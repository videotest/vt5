#include "stdafx.h"
#include "stat_filter.h"
#include "resource.h"

#include "statistics.h"
#include "\vt5\com_base\stream_misc.h"

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableObject
//
//////////////////////////////////////////////////////////////////////
CStatTableFilter::CStatTableFilter()
{
	m_bstrDocumentTemplate = _bstr_t("Image");
	char szFilterName[255];	szFilterName[0] = 0;
	::LoadString(App::handle(), IDS_STAT_TABLE_FILTERNAME, szFilterName, 255);
	m_bstrFilterName = szFilterName;//_bstr_t("JPG files");
	m_bstrFilterExt = _bstr_t(".stbl");
	AddDataObjectType( szTypeStatTable );	
}

//////////////////////////////////////////////////////////////////////
CStatTableFilter::~CStatTableFilter()
{
}



//////////////////////////////////////////////////////////////////////
bool CStatTableFilter::ReadFile( const char *pszFileName )
{
	if( !pszFileName )	return false;

	TCHAR sz_name[_MAX_PATH];	sz_name[0] = 0;
	TCHAR sz_drive[_MAX_DRIVE];	sz_drive[0] = 0;
	TCHAR sz_dir[_MAX_PATH];	sz_dir[0] = 0;
	TCHAR sz_ext[_MAX_PATH];	sz_ext[0] = 0;
	_splitpath( pszFileName, sz_drive, sz_dir, sz_name, sz_ext );
	
	IUnknown* punk = CreateNamedObject( szTypeStatTable );

	_bstr_t bstr_name( sz_name );
	long lname_exists = 0;
	if( m_sptrINamedData )
		m_sptrINamedData->NameExists( bstr_name, &lname_exists );

	//set name
	if( !lname_exists && punk )
	{
		INamedObject2Ptr ptr_no( punk );
		if( ptr_no )
			ptr_no->SetName( bstr_name );
	}

	sptrISerializableObject	ptr_so( punk );
	punk->Release();	punk = 0;

	if( ptr_so == 0 )	return false;

	stream_file file;
	if( S_OK != file.open( pszFileName ) )
		return false;

	SerializeParams	sp = {0};
	HRESULT hr = ptr_so->Load( &file, &sp );

    return ( hr == S_OK );
}

//////////////////////////////////////////////////////////////////////
bool CStatTableFilter::WriteFile( const char *pszFileName )
{
	IUnknown* punk = 0;
	
	// convert object type to BSTR
	_bstr_t bstrType( GetDataObjectType(0) );

	// get active object in which type == m_strObjKind	
	if( !(m_bObjectExists && m_sptrAttachedObject != NULL) && m_sptrIDataContext != NULL )
	{
		m_sptrIDataContext->GetActiveObject( bstrType, &punk );

	}
	else
	{
		punk = (IUnknown*)m_sptrAttachedObject.GetInterfacePtr();
		punk->AddRef();
	}

	if( !punk )			return false;

	sptrISerializableObject	ptr_so( punk );
	punk->Release();	punk = 0;

	if( ptr_so == 0 )	return false;

	stream_file file;
	if( S_OK != file.open( pszFileName ) )	return false;

	SerializeParams	sp = {0};
	HRESULT hr = ptr_so->Store( &file, &sp );

    return ( hr == S_OK );
}

//////////////////////////////////////////////////////////////////////
bool CStatTableFilter::_InitNew()
{
	return true;
}