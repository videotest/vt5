//#include "StdAfx.h"
//#include "PropBagImpl.h"
#include "alloc.h"

//#include "\vt5\awin\profiler.h"


//IMPLEMENT_UNKNOWN_BASE(CNamedPropBagImpl, NamedPropBag);

inline int CNamedPropBagImpl::CompareItem(LPOS lpos, DWORD h2, BSTR bstr2)
{ // сравнение двух пар хэш/строка
	CNamedProperty *p = m_bag.ptr()+lpos;
	dword h1 = p->dwHash;

	if(h1<h2) return -1;
	if(h1>h2) return 1;

	BSTR bstr1 = p->bstrName;

	if (bstr1 == NULL) {
        return bstr2 ? -1 : 0;
    }

    if (bstr2 == NULL) {
        return 1;
    }

    const unsigned int l1 = ::SysStringLen(bstr1);
    const unsigned int l2 = ::SysStringLen(bstr2);

    unsigned int len = l1;
    if (len > l2) {
        len = l2;
    }

    while (len-- > 0) {
        if (*bstr1++ != *bstr2++) {
            return bstr1[-1] - bstr2[-1];
        }
    }

    return (l1 < l2) ? -1 : (l1 == l2) ? 0 : 1;
}

inline bool CNamedPropBagImpl::Search(BSTR bstr, long *ppos)
{	
	//PROFILE_TEST("CNamedPropBagImpl::Search")
	//найти элемент в массиве;
	// Если найден - true, его номер в ppos
	// Если не найден - false, в ppos - номер первого большего
	int size=m_bag.size();
	if(size==0) { *ppos=0; return false; } //no data

	DWORD h=CNamedProperty::hash_f(bstr);

	int i0=0, i1=size-1; // диапазон для поиска

	int cmp0 = CompareItem(i0, h, bstr);
	if(cmp0==0) { *ppos=i0; return true; }
	if(cmp0>0) { *ppos=i0; return false; } //[i0]>h

	int cmp1 = CompareItem(i1, h, bstr);
	if(cmp1==0) { *ppos=i1; return true; }
	if(cmp1<0) { *ppos=i1+1; return false; } //[i1]<h

	while(i1-i0>1)
	{
		int i=(i1+i0)/2;
		int cmp = CompareItem(i, h, bstr);
		if(cmp==0) { *ppos=i; return true; }
		if(cmp<0) i0=i; //[i]<h
		else i1=i;
	}
	*ppos=i1; return false;
}

inline int CNamedPropBagImpl::GetPropertyPos( BSTR bstrName, LPOS* plos )
{
	long pos;
	if( Search(bstrName, &pos) )
		*plos=pos+1; // Внимание! +1 - чтобы не было 0.
	else
		*plos=0;
	return 0;
}



inline HRESULT CNamedPropBagImpl::SetProperty( BSTR bstrName, VARIANT var )
{
	if(var.vt & VT_BYREF)
	{
		if(var.vt == (VT_VARIANT|VT_BYREF))
			VariantChangeType(&var, &var, 0, var.pvarVal->vt&(~VT_BYREF));
		else
			VariantChangeType(&var, &var, 0, var.vt&(~VT_BYREF));
	}
	switch(var.vt)
	{
	case VT_I4:
	case VT_I2:
	case VT_I1:
	case VT_UI1:
	case VT_UI2:
	case VT_UI4:
	case VT_I8:
	case VT_UI8:
	case VT_INT:
	case VT_UINT:
	case VT_BOOL:
	case VT_HRESULT:
		VariantChangeType(&var, &var, 0, VT_I4);
		break;

	case VT_R8:
	case VT_R4:
	case VT_DECIMAL:
	case VT_CY:
		VariantChangeType(&var, &var, 0, VT_R8);
		break;

	case VT_BSTR:
		break;

	default:
		VariantClear(&var);
	}
	long pos=0;
	//PROFILE_TEST("CNamedPropBagImpl::SetProperty")
	int size;
	if(!Search(bstrName,&pos))
	{	// Если не нашли - раздвинем массив
		{
			//PROFILE_TEST("CNamedPropBagImpl::Alloc")
		size=m_bag.size();
		m_bag.alloc(size+1,10);
		}

		{
			//PROFILE_TEST("CNamedPropBagImpl::Copy")
		for(int i=size-1; i>=pos; i--) m_bag.ptr()[i+1] = m_bag.ptr()[i];
		}

	}

	{
	//PROFILE_TEST("CNamedPropBagImpl::Copy1")
	m_bag.ptr()[pos]=CNamedProperty(bstrName, var);
	}
	{
	//PROFILE_TEST("CNamedPropBagImpl::OnChange")
	OnChange();
	}
	return S_OK;
}

inline HRESULT CNamedPropBagImpl::DeleteProperty(long lPos)
{
	int size=m_bag.size();
	long pos=lPos-1;
	if(pos>=0 && pos<size)
	{	// Если нашли - сдвинем массив
		for(int i=pos+1; i<size; i++) m_bag.ptr()[i-1] = m_bag.ptr()[i];
		m_bag.ptr()[size-1].bstrName="";
		m_bag.ptr()[size-1].var.Clear();
		m_bag.alloc(size-1);
	}

	OnChange();
	
	return S_OK;
}

inline HRESULT CNamedPropBagImpl::GetProperty( BSTR bstrName, VARIANT *pvar )
{
	VariantInit(pvar);
	long pos=0;
	if( Search(bstrName, &pos) )
	{
		CNamedProperty *p = m_bag.ptr()+pos;
		VariantCopy(pvar,&p->var); //*pvar=p->var;
	}
	return S_OK;
}

inline HRESULT CNamedPropBagImpl::GetFirstPropertyPos( long* plos )
{
	if(m_bag.size()) *plos = 1;
	else *plos = 0;
	return S_OK;
}

inline HRESULT CNamedPropBagImpl::GetNextProperty(BSTR* pbstrName, VARIANT* pvar, long* plos )
{
	int pos=*plos-1;
	if(pos<0 || pos>=m_bag.size())
	{
		*plos=0;
		return E_INVALIDARG;
	}
	CNamedProperty *p = m_bag.ptr()+pos;
	(*plos)++; if(pos>=m_bag.size()-1) (*plos)=0;
	if(pbstrName) *pbstrName = p->bstrName.copy();
	if(pvar) VariantCopy(pvar,&p->var); //*pvar=p->var;
	return S_OK;
}

inline bool SerializeBSTR( BSTR* pbstr, IStream &ar, bool bIsStoring )
{
	//PROFILE_TEST("CNamedPropBagImpl::SerializeBSTR)")

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
			static BYTE buf_stat[255];
			if( uiLen < sizeof(buf_stat) )
			{
				ar.Read( buf_stat, uiLen, &len1 );
				*pbstr = SysAllocStringByteLen( (LPCSTR)buf_stat, uiLen );
			}
			else
			{
				BYTE *pBuf = new BYTE[uiLen];
				ar.Read( pBuf, uiLen, &len1 );
				*pbstr = SysAllocStringByteLen( (LPCSTR)pBuf, uiLen );
				delete [] pBuf;	pBuf = 0;
			}			
		}
		
	}

	return true;
}


inline HRESULT CNamedPropBagImpl::Load( IStream *pStream )
{
	//PROFILE_TEST("CNamedPropBagImpl::Load")

	ULONG len1=0;
	long version=0;
	pStream->Read(&version, sizeof(version), &len1);
	if(len1!=sizeof(version)) return E_FAIL;
	if(version!=1) return E_FAIL;


	len1=0;
	long count = 0;
	pStream->Read(&count, sizeof(count), &len1);
	if(len1!=sizeof(count)) return E_FAIL;

	//m_bag.alloc(0);
	if( m_bag.size() )
		m_bag.free();

	for( ; count; count--)
	{
		BSTR b_raw;
		if(!SerializeBSTR(&b_raw, *pStream, false)) return E_FAIL;
		_bstr_t b(b_raw);
		SysFreeString(b_raw);

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
				BSTR tmp_raw;
				if(!SerializeBSTR(&tmp_raw, *pStream, false)) return E_FAIL;
				_bstr_t tmp(tmp_raw);
				SysFreeString(tmp_raw);
				SetProperty(b, _variant_t(tmp));
				break;	}
		case VT_EMPTY:		{
				SetProperty(b, _variant_t());
				break;	}
		default: ;
			return E_FAIL; //неизвестный тип - мы такого не писали
		}
	}
	return S_OK;
}



inline HRESULT CNamedPropBagImpl::Store( IStream *pStream )
{
	ULONG len1=0;
	long version=1;
	pStream->Write(&version, sizeof(version), &len1);
	if(len1!=sizeof(version)) return E_FAIL;

	len1=0;
	long count = m_bag.size();
	pStream->Write(&count, sizeof(count), &len1);
	if(len1!=sizeof(count)) return E_FAIL;

	for(LPOS lPos = 0; lPos<count; lPos++)
	{
		CNamedProperty* p = m_bag.ptr()+lPos;
		BSTR b=p->bstrName;
		if(!SerializeBSTR(&b, *pStream, true)) return E_FAIL;

		ULONG len1=0;
		
		switch(p->var.vt)
		{
		case VT_I4:		{
				pStream->Write(&p->var.vt, sizeof(p->var.vt), &len1);
				if(len1!=sizeof(p->var.vt)) return E_FAIL;
				long tmp(p->var);
				pStream->Write(&tmp, sizeof(tmp), &len1);
				if(len1!=sizeof(tmp)) return E_FAIL;
				break;	}
		case VT_R8:		{
				pStream->Write(&p->var.vt, sizeof(p->var.vt), &len1);
				if(len1!=sizeof(p->var.vt)) return E_FAIL;
				double tmp(p->var);
				pStream->Write(&tmp, sizeof(tmp), &len1);
				if(len1!=sizeof(tmp)) return E_FAIL;
				break;	}
		case VT_BSTR:		{
				pStream->Write(&p->var.vt, sizeof(p->var.vt), &len1);
				if(len1!=sizeof(p->var.vt)) return E_FAIL;
				BSTR tmp(p->var.bstrVal);
				if(!SerializeBSTR(&tmp, *pStream, true)) return E_FAIL;
				break;	}
		case VT_EMPTY:		{
				pStream->Write(&p->var.vt, sizeof(p->var.vt), &len1);
				if(len1!=sizeof(p->var.vt)) return E_FAIL;
				break;	}
		}
	}
	return S_OK;
}
