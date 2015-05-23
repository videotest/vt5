#ifndef __com_dispatch_h__
#define __com_dispatch_h__

typedef	std::map<GuidKey, ITypeLib*>	mapGuid2TypeLib;
typedef	std::map<GuidKey, ITypeInfo*>	mapGuid2ITypeInfo;

class lib_provider : public ComObjectBase,
					public _static_t<lib_provider>
{
	route_unknown();
public:
	lib_provider();
	virtual ~lib_provider();

	ITypeLib		*get_tl();
	ITypeInfo		*get_ti( const GUID &guid );

public:
	mapGuid2TypeLib		TypeLibs;
	mapGuid2ITypeInfo	TypeInfos;

	//tempo
	ITypeLib		*m_ptl;
};

template <class Disp, const GUID *__iid>
class CDispatchImpl : public Disp
{
public:
	CDispatchImpl();
	~CDispatchImpl();

	const GUID &disp_guid()	{return *__iid;/*__uuidof(Disp);*/}

	com_call GetTypeInfoCount(  UINT *pctinfo);
    com_call GetTypeInfo(  UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo);
    com_call GetIDsOfNames(  REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
    com_call Invoke(  DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);

	ITypeInfo	*m_pti;
	ITypeLib	*m_ptl;
};

template <class Disp, const GUID *__iid>
CDispatchImpl<Disp, __iid>::CDispatchImpl()
{
	m_pti = 0;
	m_ptl = 0;
}

template <class Disp, const GUID *__iid>
CDispatchImpl<Disp, __iid>::~CDispatchImpl()
{
	if( m_ptl )m_ptl->Release();
	if( m_pti )m_pti->Release();
}

template <class Disp, const GUID *__iid>
HRESULT CDispatchImpl<Disp, __iid>::GetTypeInfoCount(  UINT *pctinfo)
{
	*pctinfo = 1;
	return S_OK;
}
template <class Disp, const GUID *__iid>
HRESULT CDispatchImpl<Disp, __iid>::GetTypeInfo(  UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
{
	if( !m_ptl )
	{
		lib_provider	*pholder = lib_provider::get_instance();
		m_ptl = pholder->get_tl();
		pholder->Release();
		if( !m_ptl )return E_FAIL;
	}
	return m_ptl->GetTypeInfo( iTInfo, ppTInfo );
}
template <class Disp, const GUID *__iid>
HRESULT CDispatchImpl<Disp, __iid>::GetIDsOfNames(  REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
{
	if( !m_pti )
	{
		lib_provider	*pholder = lib_provider::get_instance();
		m_pti = pholder->get_ti( disp_guid() );
		pholder->Release();
		if( !m_pti )return E_FAIL;
	}

	_bstr_t	bstr = rgszNames[0];
	HRESULT	hr =  m_pti->GetIDsOfNames( rgszNames, cNames, rgDispId );
	return hr;
}
template <class Disp, const GUID *__iid>
HRESULT CDispatchImpl<Disp, __iid>::Invoke(  DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
{
	if( !m_pti )
	{
		lib_provider	*pholder = lib_provider::get_instance();
		m_pti = pholder->get_ti( disp_guid() );
		pholder->Release();
		if( !m_pti )return E_FAIL;
	}
	HRESULT hr = m_pti->Invoke( (Disp*)this, dispIdMember, wFlags,
            pDispParams, pVarResult, pExcepInfo, puArgErr );
	return hr;
}

#endif //__com_dispatch_h__