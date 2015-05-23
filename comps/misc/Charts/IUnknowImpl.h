#pragma once

#include <unknwn.h>

template <class TMain >class IUnknownImpl : public IUnknown 
{
	ULONG m_lRef;
public:
	IUnknownImpl() { m_lRef = 1; }
	~IUnknownImpl(){}
	IUnknown * __stdcall GetControllingUnknown()
	{
		IUnknown *punk = 0; 
		QueryInterface( IID_IUnknown, (LPVOID*)&punk ); 
		return punk; 
	} 
	ULONG __stdcall _addref() { return ++m_lRef; }
	ULONG __stdcall _release()
	{
		m_lRef--;

		if( !m_lRef )
		{
			delete (TMain*)this;
			return 0;
		}

		return m_lRef;
	}
};

#define BEGIN_INTERFACE_SUPPORT_MAP(_TMain_) \
public: \
ULONG __stdcall AddRef(void) \
{ return _addref(); } \
ULONG __stdcall Release(void) \
{ return _release(); }\
HRESULT __stdcall QueryInterface ( REFIID riid, void __RPC_FAR *__RPC_FAR *ppvObject) { \
	if( !ppvObject ) \
		return S_FALSE; \
	if( riid == IID_IUnknown )  \
		*ppvObject = (IUnknown*)((IUnknownImpl<_TMain_>*)this);  \

#define INTERFACE_SUPPORT_ENTRY( _iface_ ) \
	else if( riid == __uuidof( _iface_ ) ) \
		*ppvObject = (_iface_ *)this; \

#define CHAIN_INTERFACE_SUPPORT_ENTRY( _base_ ) \
	else if( _base_::QueryInterface( riid, ppvObject ) ==  S_OK );   \

#define END_INTERFACE_SUPPORT_MAP() \
	else \
	{  \
		*ppvObject = 0; \
		return E_NOINTERFACE; \
	} \
	((IUnknown*)(*ppvObject))->AddRef(); \
	return S_OK; \
}
