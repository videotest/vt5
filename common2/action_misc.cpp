#include "stdafx.h"
#include "action_misc.h"

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\misc_string.h"
#include "\vt5\awin\misc_list.h"

#include "misc_utils.h"
#include "data5.h"
#include "docview5.h"
#include "core5.h"

HRESULT CActionShowView::DoInvoke()
{
	if( m_ptrTarget == 0 )
		return S_FALSE;

	sptrIFrameWindow	sptrF( m_ptrTarget );

	IUnknown	*punkSplitter = 0;

	sptrF->GetSplitter( &punkSplitter );
	sptrISplitterWindow	sptrS( punkSplitter );

	punkSplitter->Release();
		
	int	nRow, nCol;

	_bstr_t	bstrViewProgID = GetViewProgID();
	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->ChangeViewType( nRow, nCol, bstrViewProgID );

	IUnknown	*punkView = 0;
	sptrS->GetViewRowCol( nRow, nCol, &punkView );

	if( !punkView )return S_FALSE;

	IViewPtr			ptrView( punkView );
	IDataContext2Ptr	ptrContext( punkView );

	punkView->Release();

	if( ptrView == 0 || ptrContext == 0 )
		return S_FALSE;


	//gathering all available types

	_list_t<_bstr_t> arTypes;
	_list_t<_bstr_t> arViewTypes;
	{
		sptrIApplication spApp( ::GetAppUnknown() );
		if( spApp )
		{
			sptrIDataTypeInfoManager	sptrT( spApp );
			if( sptrT )
			{
				long	nTypesCount = 0;
				sptrT->GetTypesCount( &nTypesCount );
				for( long nType = 0; nType < nTypesCount; nType++ )
				{
					BSTR	bstr = 0;
					sptrT->GetType( nType, &bstr );

					if( bstr )
					{
						bstr_t strType = bstr;
						
						DWORD dwMatch = 0;						
						ptrView->GetMatchType( bstr, &dwMatch );
						if( dwMatch >= mvFull )					
							arViewTypes.add_tail( strType );

						arTypes.add_tail( strType );
						::SysFreeString( bstr );
					}
				}
			}
		}
	}

	if( arTypes.count() < 1 || arViewTypes.count() < 1 )
		return S_OK;

	//ask view m.b. it have attached object?
	
	TPOS lpos_view;
	for( lpos_view=arViewTypes.head();lpos_view;lpos_view=arViewTypes.next(lpos_view) )
	{

		_bstr_t bstrType = arViewTypes.get( lpos_view );
		LONG_PTR lPos = 0;
		ptrContext->GetFirstSelectedPos( bstrType, &lPos );
		
		while( lPos )
		{
			IUnknown* punk = 0;
			ptrContext->GetNextSelected( bstrType, &lPos, &punk );
			if( punk )
			{
				punk->Release();	punk = 0;
				return S_OK;				
			}
		}
	}

	//View doesn't have active object, type supported by view. Try to activate last active type
	for( lpos_view=arViewTypes.head();lpos_view;lpos_view=arViewTypes.next(lpos_view) )
	{
		_bstr_t bstrType = arViewTypes.get( lpos_view );		
		IUnknown* punk = 0;
		ptrContext->GetLastActiveObject( bstrType, &punk );
		if( punk )
		{
			ptrContext->SetObjectSelect( punk, 1 );
			punk->Release();
			return S_OK;
		}
	}

	//Still here? No active, last active object, etc... Try to activate first object from context, supp by view
	for( lpos_view=arViewTypes.head();lpos_view;lpos_view=arViewTypes.next(lpos_view) )
	{
		_bstr_t bstrType = arViewTypes.get( lpos_view );		
		IUnknown* punk = 0;
		LONG_PTR lPos = 0;
		ptrContext->GetFirstObjectPos( bstrType, &lPos );

		while( lPos )
		{
			IUnknown* punk = 0;
			ptrContext->GetNextObject(bstrType, &lPos, &punk );
			if( punk )
			{
				ptrContext->SetObjectSelect( punk, 1 );
				punk->Release();
				return S_OK;
			}
		}
	}
	
	return S_OK;

}

HRESULT CActionShowView::GetActionState( DWORD *pdwState )
{
	if( !pdwState )	return E_POINTER;

	*pdwState = afEnabled;

	sptrIFrameWindow	sptrF( m_ptrTarget );

	IUnknown	*punkSplitter = 0;

	sptrF->GetSplitter( &punkSplitter );
	if( !punkSplitter )
		return S_OK;

	sptrISplitterWindow	sptrS( punkSplitter );

	punkSplitter->Release();	punkSplitter = 0;
		
	int	nRow, nCol;
	BSTR	bstrView = 0;

	_bstr_t	bstrViewProgID = GetViewProgID();
	sptrS->GetActivePane( &nRow, &nCol );
	sptrS->GetViewTypeRowCol( nRow, nCol, &bstrView );

	_bstr_t	bstrCurrentView = bstrView;
	::SysFreeString(bstrView );

	if( bstrViewProgID == bstrCurrentView )
		*pdwState |= afChecked;

	return S_OK;
}
