#include "StdAfx.h"
#include "TestDataArray.h"
#include "data5.h"

//#include <ActivScp.h>
#include "script_int.h"
#include "misc_utils.h"
#include "core5.h"

CTestDataArray::CTestDataArray(void) : IDispatchImpl<ITestDataArrayDisp>( "TestMan.tlb" )
{
}

CTestDataArray::~CTestDataArray(void)
{
}

IUnknown *CTestDataArray::DoGetInterface( const IID &iid )
{
	if( iid == __uuidof( ITestDataArray ) )
		return (ITestDataArray *)this;
	else if( iid == IID_IDispatch ) 
		return (IDispatch *)this;	
	else if( iid == IID_INamedPropBag || iid == IID_INamedPropBagSer ) 
		return (INamedPropBag *)this;

	return CObjectBase::DoGetInterface( iid );
}

HRESULT CTestDataArray::AddItem( /*[in]*/ VARIANT vData, /*[in]*/ BSTR bstrName )
{
	XTestArrayItemInfo *pii = new XTestArrayItemInfo;
	pii->m_bstrName = bstrName;
	pii->m_varData = vData;
	m_items.add_tail( pii );
    return S_OK;
}

HRESULT CTestDataArray::GetItem( /*[in]*/ UINT nID, /*[out]*/ VARIANT *lpvRetVal)
{
	if(!lpvRetVal) 
		return E_INVALIDARG;

	TPOS pos = FindItem(nID);
	if(!pos) 
		return E_FAIL;

	XTestArrayItemInfo *pii = 0;
    pii = m_items.get( pos );
	if( !pii )
		return E_FAIL;

	::VariantCopy( lpvRetVal, &(pii->m_varData) );
	return S_OK;
}

HRESULT CTestDataArray::GetItemName( /*[in]*/ UINT nID, /*[out]*/ BSTR *pbstrName)
{
	if(!pbstrName) 
		return E_INVALIDARG;

	TPOS pos = FindItem(nID);
	if(!pos) 
		return E_FAIL;

	XTestArrayItemInfo *pii = 0;
    pii = m_items.get( pos );
	if( !pii )
		return E_FAIL;

	*pbstrName = pii->m_bstrName.copy( );
	return S_OK;
}

HRESULT CTestDataArray::SetItemCmpExpr( /*[in]*/ UINT nID, /*[in]*/ BSTR bstrCmpExpr)
{
	TPOS pos = FindItem(nID);
	if(!pos) 
		return E_FAIL;

	XTestArrayItemInfo *pii = 0;
    pii = m_items.get( pos );
	if( !pii )
		return E_FAIL;
	
	pii->m_bstrScriptCmp = bstrCmpExpr;
    return S_OK;
}

HRESULT CTestDataArray::GetItemCmpExpr( /*[in]*/ UINT nID, /*[out]*/ BSTR *pbstrCmpExpr)
{
	if( !pbstrCmpExpr )
		return E_INVALIDARG;

	TPOS pos = FindItem(nID);
	if(!pos) 
		return E_FAIL;

	XTestArrayItemInfo *pii = 0;
    pii = m_items.get( pos );
	if( !pii )
		return E_FAIL;
	
	*pbstrCmpExpr = pii->m_bstrScriptCmp.copy( );
	return S_OK;
}

HRESULT CTestDataArray::CompareItem( /*[in]*/ UINT nID, /*[in]*/ VARIANT vCmpData, /*[out]*/ long *plEqual )
{
	if( !plEqual )   
		return E_INVALIDARG;

	*plEqual = 0;
	TPOS pos = FindItem(nID);
	if(!pos) 
		return E_FAIL;

	XTestArrayItemInfo *pii = 0;
    pii = m_items.get( pos );
	if( !pii )
		return E_FAIL;

	*plEqual = pii->Compare( _variant_t(vCmpData) ) ? 1 : 0;
	return S_OK;
}

HRESULT CTestDataArray::GetItemCount( /*[out]*/ long *lplRet)
{
	if(!lplRet) 
		return E_INVALIDARG;

	*lplRet = m_items.count( );
	return S_OK;
}

HRESULT CTestDataArray::RemoveItem( /*[in]*/ UINT nID)
{
	TPOS	lpos = 0;
	lpos = FindItem(nID);
	if( !lpos )
		return E_FAIL;

	m_items.remove( lpos );
	return S_OK;
}

HRESULT CTestDataArray::RemoveAll()
{
	m_items.clear( );
	return S_OK;
}

TPOS CTestDataArray::FindItem(UINT nID)
{
	TPOS pos = m_items.head();
	for(UINT i=0; (i<nID) && pos; i++)
		pos = m_items.next(pos);
	return pos;        
}

//ISerializableObject
/////////////////////////////////////////////////
HRESULT CTestDataArray::Load( IStream *pStream, SerializeParams *pparams )
{
	HRESULT hr = CObjectBase::Load( pStream, pparams );
	if( S_OK != hr )
		return hr;

	ULONG len1=0;
	long version=0;
	pStream->Read(&version, sizeof(version), &len1);
	if(len1!=sizeof(version)) return E_FAIL;

	// нет поддержки для версий ниже 3!
	if( version < 3 || version > 3 ) return E_FAIL;

	len1=0;

	// items count
	long count = 0;
	pStream->Read(&count, sizeof(count), &len1);
	if(len1!=sizeof(count)) return E_FAIL;

	m_items.clear( );
	// load items
	for( long lcount_items = count; lcount_items; lcount_items-- )
	{
		XTestArrayItemInfo *pii = new XTestArrayItemInfo;
		if( S_OK != pii->Serialize( pStream, false ) )
			break;
		m_items.add_tail( pii );
	}
	if( lcount_items )
	{
		// не все успешно загрузились
		m_items.clear( );
		return E_FAIL;
	}

	CNamedPropBagImpl::Load(pStream);
    return S_OK;
}

/////////////////////////////////////////////////
HRESULT CTestDataArray::Store( IStream *pStream, SerializeParams *pparams )
{
	HRESULT hr = CObjectBase::Store( pStream, pparams );
	if( hr != S_OK )
		return hr;

	ULONG nWritten = 0;

	long nVersion  = 3;
	pStream->Write( &nVersion, sizeof(long), &nWritten );

	long lrec_count = m_items.count( );
	pStream->Write( &lrec_count, sizeof( long ), &nWritten );

	for( TPOS lpos = m_items.head(); lpos; lpos = m_items.next( lpos ) )
	{
		XTestArrayItemInfo *pii = 0;
		pii = m_items.get( lpos );
		if( !pii )
			return E_FAIL;

		pii->Serialize( pStream, true );
	}

	// store PropBag
	CNamedPropBagImpl::Store(pStream);
    return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
// XTestArrayItemInfo implementation
/////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
CTestDataArray::XTestArrayItemInfo::XTestArrayItemInfo(void)
{
}

/////////////////////////////////////////////////////////////////////////////////
CTestDataArray::XTestArrayItemInfo::~XTestArrayItemInfo(void)
{
}

/////////////////////////////////////////////////////////////////////////////////
HRESULT	CTestDataArray::XTestArrayItemInfo::Serialize( IStream *pStream, bool bstoring )
{
	if( !pStream )
		return E_INVALIDARG;

	long lcurr_version = 1;
	
	if( bstoring )
	{
        ULONG len1=0;

		// version
		long lversion = lcurr_version;
		pStream->Write( &lversion, sizeof(lversion), &len1 );
		
		// value
		switch( m_varData.vt )
		{
		case VT_I4:		{
			pStream->Write( &m_varData.vt, sizeof(m_varData.vt), &len1 );
			if( len1 != sizeof(m_varData.vt) ) return E_FAIL;
			long tmp(m_varData);
			pStream->Write( &tmp, sizeof(tmp), &len1 );
			if( len1 != sizeof(tmp) ) return E_FAIL;
			break;	}
		case VT_R8:		{
			pStream->Write( &m_varData.vt, sizeof(m_varData.vt), &len1 );
			if( len1 != sizeof(m_varData.vt) ) return E_FAIL;
			double tmp(m_varData);
			pStream->Write( &tmp, sizeof(tmp), &len1 );
			if( len1 != sizeof(tmp) ) return E_FAIL;
			break;	}
		case VT_BSTR:		{
			pStream->Write( &m_varData.vt, sizeof(m_varData.vt), &len1 );
			if( len1 != sizeof(m_varData.vt) ) return E_FAIL;
			BSTR tmp(m_varData.bstrVal);
			bool bRes = SerializeBSTR( &tmp, *pStream, true );
			if( !bRes ) return E_FAIL;
			break;	}
		}

		// name
		if( !SerializeBSTR( &m_bstrName.GetBSTR(), *pStream, true ) )
			return E_FAIL;

        // compare expression
		if( !SerializeBSTR( &m_bstrScriptCmp.GetBSTR(), *pStream, true ) )
			return E_FAIL;
	}
	else
	{
        ULONG len1 = 0;

		// version
		long lversion = 0;
		pStream->Read( &lversion, sizeof(lversion), &len1);
		if( len1 != sizeof(lversion) ) return E_FAIL;
		if( lversion > lcurr_version ) return E_FAIL;
        
		// value
		VARTYPE vt=0;
		pStream->Read(&vt, sizeof(vt), &len1);
		if(len1!=sizeof(vt)) return E_FAIL;

		switch(vt)
		{
		case VT_I4:		{
			long tmp;
			pStream->Read(&tmp, sizeof(tmp), &len1);
			if(len1!=sizeof(tmp)) return E_FAIL;
			m_varData = _variant_t(tmp);
			break;	}
		case VT_R8:		{
			double tmp;
			pStream->Read(&tmp, sizeof(tmp), &len1);
			if(len1!=sizeof(tmp)) return E_FAIL;
			m_varData = _variant_t(tmp);
			break;	}
		case VT_BSTR:		{
			BSTR tmp_raw;
			bool bRes = SerializeBSTR(&tmp_raw, *pStream, false);
			if(!bRes) return E_FAIL;
			_bstr_t tmp(tmp_raw);
			SysFreeString(tmp_raw);
			m_varData = _variant_t(tmp);
			break;	}
		default: ;
			return E_FAIL; //неизвестный тип - мы такого не писали
		}

		// name
		if( !SerializeBSTR( &m_bstrName.GetBSTR(), *pStream, false ) )
			return E_FAIL;

        // compare expression
		if( !SerializeBSTR( &m_bstrScriptCmp.GetBSTR(), *pStream, false ) )
			return E_FAIL;
	}

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////////
bool	CTestDataArray::XTestArrayItemInfo::Compare( _variant_t var_new_val  )
{
	if( !m_bstrScriptCmp.length() )
		return m_varData == var_new_val;

	//get the ActiveScript
	IUnknown	*punkScript = 0;
	IUnknown	*papp = GetAppUnknown();
	IApplicationPtr	sptrA = papp;

	sptrA->GetActiveScript( &punkScript );
	if( !punkScript )
		return false;

	IScriptEnginePtr sptr_se = punkScript;
	if( punkScript )
		punkScript->Release();	punkScript = 0;

	if( sptr_se == 0 )
		return false;

	IUnknown* punk_as = 0;
	sptr_se->GetActiveScript( &punk_as );
	IActiveScriptPtr sptr_as = punk_as;
	if( punk_as )	
		punk_as->Release();		punk_as = 0;

	// get active script site
	IUnknown* punk_app_ss = 0;
	if( S_OK != sptr_as->GetScriptSite( IID_IActiveScriptSite, (void**)&punk_app_ss ) )
		return false;

	IActiveScriptSitePtr  sptr_app_ss = punk_app_ss;
	if( punk_app_ss  )
		punk_app_ss->Release( );	punk_app_ss  = 0;

	if( sptr_app_ss == 0 )
		return false;

	// create own active script
	CLSID	clsid_se;
	_bstr_t bstrt_se( _T("VBScript") ); 
	if( ::CLSIDFromProgID( bstrt_se, &clsid_se ) )
		return false;

	// Create the ActiveX Scripting Engine
	IActiveScriptPtr sptr_own_as;
	if ( S_OK != CoCreateInstance( clsid_se, NULL, CLSCTX_INPROC_SERVER, IID_IActiveScript, (void **)&sptr_own_as ) )
		return false;

	// Script Engine must support IActiveScriptParse for us to use it
	IActiveScriptParsePtr sptr_own_asp = sptr_own_as;
	if( sptr_own_asp == 0 )
		return false;

	// set application's script site
	if( S_OK != sptr_own_as->SetScriptSite( sptr_app_ss ) )
		return false;

	if( S_OK != sptr_own_asp->InitNew( ) )
		return false;

	if( S_OK != sptr_own_as->SetScriptState(SCRIPTSTATE_CONNECTED) )
		return false;


	_bstr_t bstr_func;
	bstr_func = _bstr_t( FUNC_CMP_BEGIN FUNC_CMP_NAME "=" ) + m_bstrScriptCmp + _bstr_t( FUNC_CMP_END );
	if( sptr_own_asp->ParseScriptText( bstr_func, NULL, NULL, NULL, SCRIPTTEXT_ISVISIBLE, 0, 0L, 0, 0 ) )
		return false;

	_variant_t var_args[2];
	// [vanek] SBT:1060 - 05.07.2004
	var_args[0] = var_new_val;
	var_args[1] = m_varData;

	DISPPARAMS dp = { var_args, 0, 2, 0 };
	DISPID id = 0;
	IDispatchPtr sptr_script_disp;
	if( S_OK != sptr_own_as->GetScriptDispatch( 0, &sptr_script_disp ) )
		return false;

	_bstr_t bstr_func_name( FUNC_CMP_NAME );
	if( S_OK != sptr_script_disp->GetIDsOfNames( IID_NULL, &bstr_func_name.GetBSTR(), 1, 0, &id ) )
		return false;

	_variant_t var_res;
	if( S_OK != sptr_script_disp->Invoke( id,IID_NULL,0,DISPATCH_METHOD, &dp, &var_res, 0, 0 ) )
		return false;

	sptr_own_as->Close( );
	
	if( var_res.vt != VT_BOOL )
		return false;

	return var_res.boolVal == VARIANT_TRUE;
}