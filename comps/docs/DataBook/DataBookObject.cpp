/////////////////////////////////////////////////////
// CDataBook - таблица для хранения данных отчета

#include "stdafx.h"
#include "DataBook.h"
#include "DataBookObject.h"

CDataBook::CDataBook()
{

}

CDataBook::~CDataBook()
{

}

//////////////////////////////////////////////////////////////////////
HRESULT CDataBook::AttachData( IUnknown *punkNamedData )	//called from NamedData
{
	HRESULT hr = CObjectBase::AttachData( punkNamedData );
	//m_ptrDocument = punkNamedData;

	return hr;
}


HRESULT CDataBookInfo::GetUserName( BSTR *pbstr )
{
	if( !pbstr ) 
		return E_POINTER;

	if( !m_bstrUserName.length() )
	{
		char sz_buf[256];
		sz_buf[0] = 0;

		::LoadString( App::handle(), IDS_DATABOOK_OBJECT_TYPE_NAME, sz_buf, sizeof(sz_buf) );
		
		m_bstrUserName	= sz_buf;
	}

	*pbstr = m_bstrUserName.copy();

	return S_OK;//CDataInfoBase::GetUserName( pbstr );
}


