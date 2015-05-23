#include "stdafx.h"
#include "PropPage.h"
#include "utils.h"
#include "window5.h"
#include "misc5.h"



/////////////////////////////////////////////////////////////////////////////
std_dll bool GetPropertyPageByName( CString strPageName, IUnknown** ppunkPage, int* pnPageNum )
{
	IUnknown	*punkPropertySheet = ::GetPropertySheet();
	if( punkPropertySheet == NULL )
		return false;
	
	
	IPropertySheetPtr	spPSheet( punkPropertySheet );
	punkPropertySheet->Release();

	if( spPSheet == NULL )
		return false;
								
	int nPagesCount = 0;		
	spPSheet->GetPagesCount( &nPagesCount );
																
	for( long nPageNo = nPagesCount-1; nPageNo >-1; nPageNo-- )
	{							
		IUnknown *punkPage = NULL;
		spPSheet->GetPage( nPageNo, &punkPage );						
																		
		IOptionsPagePtr	ptr( punkPage );								
		if( punkPage )													
			punkPage->Release();										
		else															
			continue;

		if( ptr == NULL )
			continue;
																		
		BSTR	bstrPageName;											
		ptr->GetCaption( &bstrPageName );								
		CString	strTestPageName = bstrPageName;								
		::SysFreeString( bstrPageName );								
																		
		if( strTestPageName == strPageName )									
		{
			ptr->AddRef();
			*ppunkPage = ptr;

			*pnPageNum = nPageNo;
			return true;			
		}																
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
std_dll bool GetPropertyPageByProgID( CString strProgID, IUnknown** ppunkPage, int* pnPageNum )
{
	IUnknown	*punkPropertySheet = ::GetPropertySheet();
	if( punkPropertySheet == NULL )
		return false;

	if( !ppunkPage )
		return false;
	
	
	IPropertySheetPtr	spPSheet( punkPropertySheet );
	punkPropertySheet->Release();

	if( spPSheet == NULL )
		return false;
								
	int nPagesCount = 0;		
	spPSheet->GetPagesCount( &nPagesCount );
																
	for( long nPageNo = nPagesCount-1; nPageNo >-1; nPageNo-- )
	{							
		IUnknown *punkPage = NULL;
		spPSheet->GetPage( nPageNo, &punkPage );						
																		
		IOptionsPagePtr	ptr( punkPage );								
		if( punkPage )													
			punkPage->Release();										
		else															
			continue;

		if( ptr == NULL )
			continue;
																		
		
		CString strTestPageProgID;
		IPersistPtr ptrPersist( ptr );
		if( ptrPersist == NULL )
			continue;

		CLSID ClassID;
		::ZeroMemory( &ClassID, sizeof(ClassID) );
		if( ptrPersist->GetClassID( &ClassID ) != S_OK )
			continue;

		LPWSTR psz;
		if( ::ProgIDFromCLSID( ClassID, &psz ) != S_OK )
			continue;
		
		strTestPageProgID = psz;
		::CoTaskMemFree(psz);		

		if( strTestPageProgID == strProgID )
		{
			ptr->AddRef();
			*ppunkPage = ptr;

			if( pnPageNum )
				*pnPageNum = nPageNo;

			return true;			
		}																
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
std_dll bool ShowPropertyPage(int nPageNum, bool bShow )
{

	IUnknown *punkPropertySheet = NULL;
	punkPropertySheet = ::GetPropertySheet();
	if( punkPropertySheet == NULL )
		return false;

	IPropertySheetPtr	spPSheet( punkPropertySheet );
	punkPropertySheet->Release();
	if( spPSheet == NULL )
		return false;

	if( bShow )	
		spPSheet->IncludePage( nPageNum );	
	else	
		spPSheet->ExcludePage( nPageNum );	

	return true;
}


