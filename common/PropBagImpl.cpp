#include "StdAfx.h"
#include "PropBagImpl.h"
#include "alloc.h"


IMPLEMENT_UNKNOWN_BASE(CNamedPropBagImpl, NamedPropBag);

int CNamedPropBagImpl::GetPropertyPos( BSTR bstrName, long *plos )
{
	DWORD h=CNamedProperty::hash_f(bstrName);
	for(long lPos = bag.head(); lPos; lPos=bag.next(lPos))
	{
		CNamedProperty p=bag.get(lPos);		
		if(p.dwHash==h && p.bstrName==_bstr_t(bstrName))
		{
			*plos=lPos;
			return 0;
		}
	}
	return 0;
}


HRESULT CNamedPropBagImpl::XNamedPropBag::SetProperty( BSTR bstrName, VARIANT var )
{
	_try_nested_base(CNamedPropBagImpl, NamedPropBag, SetProperty)
	{
		long lPos=0;
		pThis->GetPropertyPos( bstrName, &lPos );
		if(lPos) pThis->bag.remove(lPos);


		CNamedProperty prop(bstrName, var);
		pThis->bag.add_tail(prop);

		pThis->OnChange();

		return true;
	}
	_catch_nested;
}

HRESULT CNamedPropBagImpl::XNamedPropBag::DeleteProperty(long lPos)
{
	_try_nested_base(CNamedPropBagImpl, NamedPropBag, DeleteProperty)
	{
		//long lPos=0;
		//pThis->GetPropertyPos( bstrName, &lPos );
		if(lPos) pThis->bag.remove(lPos);

		pThis->OnChange();
		
		return true;
	}
	_catch_nested;
}

HRESULT CNamedPropBagImpl::XNamedPropBag::GetProperty( BSTR bstrName, VARIANT *pvar )
{
	_try_nested_base(CNamedPropBagImpl, NamedPropBag, GetProperty)
	{
		VariantInit(pvar);
		long lPos=0;
		pThis->GetPropertyPos( bstrName, &lPos );
		if(lPos)
		{
			CNamedProperty p = pThis->bag.get(lPos);
			*pvar=p.var;
		}
		return true;
	}
	_catch_nested;
}

HRESULT CNamedPropBagImpl::XNamedPropBag::GetFirstPropertyPos( long* plos )
{
	_try_nested_base(CNamedPropBagImpl, NamedPropBag, GetFirstPropertyPos)
	{
		*plos = pThis->bag.head();
		return true;
	}
	_catch_nested;
}

HRESULT CNamedPropBagImpl::XNamedPropBag::GetNextProperty(BSTR* pbstrName, VARIANT* pvar, long* plos )
{
	_try_nested_base(CNamedPropBagImpl, NamedPropBag, GetNextProperty)
	{
		CNamedProperty p = pThis->bag.get(*plos);
		*plos = pThis->bag.next(*plos);
		if(pbstrName) *pbstrName = p.bstrName.copy();
		if(pvar) *pvar=p.var;
		return true;
	}
	_catch_nested;
}

bool SerializeBSTR( BSTR* pbstr, IStream &ar, bool bIsStoring )
{
	if( !pbstr )
		return false;

	if( bIsStoring )
	{
		ULONG len1=0;
		UINT uiLen = SysStringByteLen( *pbstr );
		ar.Write(&uiLen, sizeof(uiLen), &len1);
		if(len1!=sizeof(uiLen)) return false;
		if( uiLen > 0 )
		{
			ar.Write( (BYTE*)(*pbstr), uiLen, &len1 );
			if(len1!=uiLen) return false;
		}
	}
	else
	{
		*pbstr = 0;
		ULONG len1=0;
		UINT uiLen = 0;
		ar.Read(&uiLen, sizeof(uiLen), &len1);
		if(len1!=sizeof(uiLen)) return false;
		if( uiLen > 0 )
		{			
			BYTE *pBuf = new BYTE[uiLen];
			ar.Read( pBuf, uiLen, &len1 );
			*pbstr = SysAllocStringByteLen( (LPCSTR)pBuf, uiLen );
			delete [] pBuf;	pBuf = 0;
			
		}
		
	}

	return true;
}


HRESULT CNamedPropBagImpl::XNamedPropBag::Load( IStream *pStream )
{
	_try_nested_base(CNamedPropBagImpl, NamedPropBag, Load)
	{
		ULONG len1=0;
		long version=0;
		pStream->Read(&version, sizeof(version), &len1);
		if(len1!=sizeof(version)) return E_FAIL;
		if(version!=1) return E_FAIL;


		len1=0;
		long count = 0;
		pStream->Read(&count, sizeof(count), &len1);
		if(len1!=sizeof(count)) return E_FAIL;

		pThis->bag.clear();

		for( ; count; count--)
		{
			BSTR b;
			if(!SerializeBSTR(&b, *pStream, false)) return E_FAIL;

			ULONG len1=0;
			VARTYPE vt=0;
			pStream->Read(&vt, sizeof(vt), &len1);
			if(len1!=sizeof(vt)) return E_FAIL;
			
			switch(vt)
			{
			case VT_I4:		{
					long tmp;
					pStream->Read(&tmp, sizeof(tmp), &len1);
					if(len1!=sizeof(tmp)) return E_FAIL;
					SetProperty(b, _variant_t(tmp));
					break;	}
			case VT_R8:		{
					double tmp;
					pStream->Read(&tmp, sizeof(tmp), &len1);
					if(len1!=sizeof(tmp)) return E_FAIL;
					SetProperty(b, _variant_t(tmp));
					break;	}
			case VT_BSTR:		{
					BSTR tmp;
					SerializeBSTR(&tmp, *pStream, false);
					if(len1!=sizeof(tmp)) return E_FAIL;
					SetProperty(b, _variant_t(tmp));
					break;	}
			default: ;
				return E_FAIL; //неизвестный тип - мы такого не писали
			}
		}
		return true;
	}
	_catch_nested;
}



HRESULT CNamedPropBagImpl::XNamedPropBag::Store( IStream *pStream )
{
	_try_nested_base(CNamedPropBagImpl, NamedPropBag, Store)
	{
		ULONG len1=0;
		long version=1;
		pStream->Write(&version, sizeof(version), &len1);
		if(len1!=sizeof(version)) return E_FAIL;

		len1=0;
		long count = pThis->bag.count();
		pStream->Write(&count, sizeof(count), &len1);
		if(len1!=sizeof(count)) return E_FAIL;


		for(long lPos = pThis->bag.head(); lPos; lPos=pThis->bag.next(lPos))
		{
			CNamedProperty p=pThis->bag.get(lPos);
			BSTR b=p.bstrName;
			if(!SerializeBSTR(&b, *pStream, true)) return E_FAIL;

			ULONG len1=0;
			
			switch(p.var.vt)
			{
			case VT_I4:		{
					pStream->Write(&p.var.vt, sizeof(p.var.vt), &len1);
					if(len1!=sizeof(p.var.vt)) return E_FAIL;
					long tmp(p.var);
					pStream->Write(&tmp, sizeof(tmp), &len1);
					if(len1!=sizeof(tmp)) return E_FAIL;
					break;	}
			case VT_R8:		{
					pStream->Write(&p.var.vt, sizeof(p.var.vt), &len1);
					if(len1!=sizeof(p.var.vt)) return E_FAIL;
					double tmp(p.var);
					pStream->Write(&tmp, sizeof(tmp), &len1);
					if(len1!=sizeof(tmp)) return E_FAIL;
					break;	}
			case VT_BSTR:		{
					pStream->Write(&p.var.vt, sizeof(p.var.vt), &len1);
					if(len1!=sizeof(p.var.vt)) return E_FAIL;
					BSTR tmp(p.var.bstrVal);
					SerializeBSTR(&tmp, *pStream, true);
					if(len1!=sizeof(tmp)) return E_FAIL;
					break;	}
			}
		}
		return S_OK;
	}
	_catch_nested;
}
