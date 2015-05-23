#ifndef  __chromo_misc_h__

#include "\vt5\ifaces\data5.h"
#include "\vt5\ifaces\carioint.h"
#include "\vt5\ifaces\Chromosome.h"
#include "\vt5\ifaces\docview5.h"
#include "\vt5\common\nameconsts.h"

inline bool IsObjectListWasBuild( IUnknown* punkObjectList )
{
	INamedDataPtr ptr_data( punkObjectList );
	if( ptr_data == 0 )	return false;

	_variant_t var;
	_bstr_t bstr_path = CARIO_ROOT;
	bstr_path += "\\";
	bstr_path += CARIO_WAS_BUILD;

	ptr_data->GetValue( bstr_path, &var );

	return ( var.vt == VT_I4 && var.lVal == 1L );
}


inline bool IsObjectListHasChromoObject( IUnknown* punkObjectList )
{
	INamedDataObject2Ptr ptr_list( punkObjectList );
	if( ptr_list == 0 )	return false;

	POSITION lpos_measure = 0;
	ptr_list->GetFirstChildPosition( &lpos_measure );
	while( lpos_measure )
	{
		IUnknown* punk_measure = 0;
		ptr_list->GetNextChild( &lpos_measure, &punk_measure );

		if( !punk_measure )
			continue;

		INamedDataObject2Ptr ptr_child( punk_measure );
		punk_measure->Release();	punk_measure = 0;

		if( ptr_child == 0 )
			continue;

		POSITION lPosCromo = 0;
		ptr_child->GetFirstChildPosition( &lPosCromo );
		while( lPosCromo )
		{
			IUnknown* punkChromo = 0;
			ptr_child->GetNextChild( &lPosCromo, &punkChromo );
			if( !punkChromo )
				continue;

			IChromosomePtr ptrChromo( punkChromo );
			punkChromo->Release();	punkChromo = 0;

			if( ptrChromo != 0 )
				return true;
		}
	}

	return false;
}

inline IUnknown* GetActiveObjectList( IUnknown* punkTarget )
{
	IViewPtr ptr_view( punkTarget );

	if( ptr_view == 0 )
	{
		IDocumentSitePtr ptr_doc_site( punkTarget );
		if( ptr_doc_site == 0 )
			return 0;

		IUnknown* punk = 0;
		ptr_doc_site->GetActiveView( &punk );
		ptr_view = punk;
		punk->Release();	punk = 0;
	}

	if( ptr_view == 0 )
		return 0;

	IDataContext3Ptr ptr_dc( ptr_view );
	if( ptr_dc == 0 )
		return 0;

	IUnknown* punk_ol = 0;
	ptr_dc->GetActiveObject( _bstr_t( szTypeObjectList ), &punk_ol );

	return punk_ol;
}

inline bool IsActiveObjectListWasBuild( IUnknown* punkTarget )
{
	IUnknown* punk_ol = GetActiveObjectList( punkTarget );
	if( !punk_ol )
		return false;

	bool b = IsObjectListWasBuild( punk_ol );
	punk_ol->Release();	punk_ol = 0;

	return b;
}

inline bool IsActiveObjectListHasChromoObject( IUnknown* punkTarget )
{
	IUnknown* punk_ol = GetActiveObjectList( punkTarget );
	if( !punk_ol )
		return false;

	bool b = IsObjectListHasChromoObject( punk_ol );	
	punk_ol->Release();	punk_ol = 0;

	return b;
}


#define __chromo_misc_h__
#endif//__chromo_misc_h__