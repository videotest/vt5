// FlterViewImpl.cpp : implementation file
//

#include "stdafx.h"
#include "dbasedoc.h"
#include "FlterViewImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CFlterViewImpl

CFlterViewImpl::CFlterViewImpl()
{
}

CFlterViewImpl::~CFlterViewImpl()
{
}

IMPLEMENT_UNKNOWN_BASE(CFlterViewImpl, FilterVw);



/////////////////////////////////////////////////////////////////////////////
HRESULT CFlterViewImpl::XFilterVw::GetActiveField( BSTR* pbstrTable, BSTR* pbstrField )
{
	_try_nested_base(CFlterViewImpl, FilterVw, GetActiveField)
	{		

		CString strTable, strField;
		_variant_t var;
		if( !pThis->GetActiveField( strTable, strField ) )
			return S_FALSE;

		if( pbstrTable )
			*pbstrTable = strTable.AllocSysString( );

		if( pbstrField )
			*pbstrField = strField.AllocSysString( );

		return S_OK;
	}
	_catch_nested;
}


/////////////////////////////////////////////////////////////////////////////
HRESULT CFlterViewImpl::XFilterVw::GetActiveFieldValue( BSTR* pbstrTable, BSTR* pbstrField, BSTR* pbstrValue )
{
	_try_nested_base(CFlterViewImpl, FilterVw, GetActiveField)
	{		

		CString strTable, strField, strValue;
		_variant_t var;
		if( !pThis->GetActiveFieldValue( strTable, strField, strValue ) )
			return S_FALSE;

		if( pbstrTable )
			*pbstrTable = strTable.AllocSysString( );

		if( pbstrField )
			*pbstrField = strField.AllocSysString( );

		if( pbstrValue )
			*pbstrValue = strValue.AllocSysString( );

		return S_OK;
	}
	_catch_nested;
}
