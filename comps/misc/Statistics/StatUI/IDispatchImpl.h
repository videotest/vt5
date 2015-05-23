#pragma once
template <class TMain>class IDispatchImpl : public TMain
{
	ITypeInfo*	m_type_info;
public:
	IDispatchImpl(){ m_type_info = 0; }
	IDispatchImpl(LPCTSTR lpStr)
	{
		m_type_info = 0;
		ITypeLib *pi_type_lib = 0;
		char sz_path[MAX_PATH];	sz_path[0] = 0;
		::GetModuleFileName( 0, sz_path, sizeof(sz_path) );
		char* psz_last = strrchr( sz_path, '\\' );
		if( psz_last )
		{
			psz_last++;
			strcpy( psz_last, "comps\\" );
			strcat( sz_path, lpStr );
			
			HRESULT hr = ::LoadTypeLib( _bstr_t( sz_path ), &pi_type_lib );
			dbg_assert( S_OK == hr );

			if( pi_type_lib )
			{
				hr = pi_type_lib->GetTypeInfoOfGuid( __uuidof( TMain ), &m_type_info );
				dbg_assert( S_OK == hr );

				pi_type_lib->Release();

			}
		}		
	}
	~IDispatchImpl(){ if( m_type_info ) m_type_info->Release(); m_type_info = 0; }


    com_call GetTypeInfoCount(/* [out] */ UINT *pctinfo)
	{
		if( !pctinfo )
			return E_POINTER;

		*pctinfo = 1;

		return S_OK;
	}
	com_call GetTypeInfo( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo)
	{
		if( !ppTInfo )
			return E_POINTER;

		if( !iTInfo )
			return E_INVALIDARG;

		if( !m_type_info )
			return E_FAIL;

		m_type_info->AddRef();

		(*ppTInfo) = m_type_info;

		return S_OK;
	}

	com_call GetIDsOfNames( 
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId)
	{
		if( !m_type_info )
			return E_FAIL;

		return m_type_info->GetIDsOfNames( rgszNames, cNames, rgDispId );
	}
	com_call Invoke( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr)
	{
		if( !m_type_info )
			return E_FAIL;

		return m_type_info->Invoke(	(IUnknown*)(TMain *)this, dispIdMember, wFlags, 
										pDispParams, pVarResult, pExcepInfo, puArgErr );
	}

};
