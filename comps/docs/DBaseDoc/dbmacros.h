#ifndef __dbmacros_h
#define __dbmacros_h

#include "PropPage.h"

const char
 c_szCBlankView[] = "BlankView",
 c_szCFilterPage[] = "FilterPage";

//Filter Property page

inline void ShowDBPage( bool bShow )
{

	IUnknown* punkPage = 0;
	int nPage = 0;
	GetPropertyPageByProgID( szFilterPropPageProgID, &punkPage, &nPage );
	if( !punkPage )
		return;

	sptrIDBFilterPage  sptrPP( punkPage );
	punkPage->Release();	punkPage = 0;

	if( sptrPP == 0 )
		return;


	IUnknown	*punkPropertySheet = ::GetPropertySheet();		
	IPropertySheetPtr	spPSheet( punkPropertySheet );
	punkPropertySheet->Release();	punkPropertySheet = 0;

	if( spPSheet == 0 )
		return;

	if( bShow )
	{		
		//sptrPP->BuildAppearance();//call on DBaseDoc::OnNotify .. ActivateView
		spPSheet->IncludePage( nPage );
	}
	else
		spPSheet->ExcludePage( nPage );	
}


#endif
