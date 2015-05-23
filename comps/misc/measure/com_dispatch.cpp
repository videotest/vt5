#include "stdafx.h"
#include "com_main.h"
#include "com_dispatch.h"


class _dispatch_map
{
};


void dbg_trace_typelib( ITypeLib *ptli )
{
	if( !ptli )
	{
		trace( "Zero typelib handle\n" );
		return;
	}

	int	nCount = ptli->GetTypeInfoCount();
	for( int i = 0; i < nCount; i++ )
	{
		ITypeInfo	*pti = 0;
		ptli->GetTypeInfo( i, &pti );

		BSTR	bstrTIName = 0;
		ptli->GetDocumentation( i, &bstrTIName, 0, 0, 0 );
		_bstr_t	bstrTI = bstrTIName;
		::SysFreeString( bstrTIName );

		trace( "Type Library:%s\n", (const char*)bstrTI );

		//IPersistPtr	ptr( pti );
		//CLSID	clsid;
		//if( ptr )ptr->GetClassID( &clsid );
		TYPEATTR	*pAttr = 0;
		pti->GetTypeAttr( &pAttr );


		if( pAttr->typekind == TKIND_COCLASS )
		{
			for( int nImpl = 0; nImpl < pAttr->cImplTypes; nImpl++ )
			{
				HREFTYPE hreftype;
				pti->GetRefTypeOfImplType( nImpl, &hreftype);  

				ITypeInfo	*ptiImpl = 0;
				pti->GetRefTypeInfo(hreftype, &ptiImpl);

				TYPEATTR	*pAttrImpl = 0;
				ptiImpl->GetTypeAttr( &pAttrImpl );


				ptiImpl->ReleaseTypeAttr( pAttrImpl );
				ptiImpl->Release();
			}
		}
		else if( pAttr->typekind == TKIND_DISPATCH )
		{
			int	n = 0;
			while( true )
			{
				FUNCDESC	*pfd;
				if( pti->GetFuncDesc( n, &pfd ) != S_OK )
					break;
				BSTR	bstrMethod = 0;
				pti->GetDocumentation(pfd->memid, &bstrMethod, NULL, NULL, NULL);
				_bstr_t	bstr = bstrMethod;
				::SysFreeString( bstrMethod );

				trace( "Member : %s\n", (const char*)bstr );

				n++;
			}
		}

		pti->ReleaseTypeAttr( pAttr );
	}

};


lib_provider::lib_provider()
{
	//simplify - load one type library
	m_ptl = 0;
	_bstr_t	filename = App::instance()->module_filename();
	::LoadTypeLib( filename, &m_ptl );

	dbg_trace_typelib( m_ptl );
}

lib_provider::~lib_provider()
{
	if( m_ptl )m_ptl->Release();
	m_ptl = 0;
}


ITypeLib		*lib_provider::get_tl()
{
	if( m_ptl )m_ptl->AddRef();
	return m_ptl;
}

ITypeInfo		*lib_provider::get_ti( const GUID &guid )
{
	if( !m_ptl )return 0;
	ITypeInfo	*pti = 0;
	m_ptl->GetTypeInfoOfGuid( guid, &pti );

	if( pti )
	{
		int	n = 0;
		while( true )
		{
			FUNCDESC	*pfd;
			if( pti->GetFuncDesc( n, &pfd ) != S_OK )
				break;
			BSTR	bstrMethod = 0;
			pti->GetDocumentation(pfd->memid, &bstrMethod, NULL, NULL, NULL);
			_bstr_t	bstr = bstrMethod;
			::SysFreeString( bstrMethod );

			n++;
		}
	}
	return pti;
}
