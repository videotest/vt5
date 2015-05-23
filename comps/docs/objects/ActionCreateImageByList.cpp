// ActionCreateImageByList.cpp: implementation of the CActionCreateImageByList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ActionCreateImageByList.h"
#include "resource.h"
#include "misc_utils.h"
#include "PropBag.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CActionCreateImageByList,	CCmdTargetEx);

// {77AA2071-F2DD-4194-A43B-DACB2F5CDC54}
GUARD_IMPLEMENT_OLECREATE(CActionCreateImageByList, "FiltersMain.CreateImageByList", 
0x77aa2071, 0xf2dd, 0x4194, 0xa4, 0x3b, 0xda, 0xcb, 0x2f, 0x5c, 0xdc, 0x54);


// Action info

// {449BE642-6C14-4f0f-AC2D-7005A2363721}
static const GUID guidCreateImageByList = 
{ 0x449be642, 0x6c14, 0x4f0f, { 0xac, 0x2d, 0x70, 0x5, 0xa2, 0x36, 0x37, 0x21 } };


ACTION_INFO_FULL(CActionCreateImageByList, IDS_ACTION_CREATE_IMAGE_BY_OBJECT_LIST, -1, -1, guidCreateImageByList );
ACTION_TARGET(CActionCreateImageByList, szTargetAnydoc );


ACTION_ARG_LIST(CActionCreateImageByList)
	ARG_IMAGE( _T("Image") )
	ARG_OBJECT( _T("Objects") )
	RES_IMAGE( _T("Generated") )
END_ACTION_ARG_LIST();

_bstr_t _GetCCName(IUnknown *pImage)
{
	IImage3Ptr pimg = pImage;

	if( pimg == 0 )
		return _bstr_t("");


	IUnknown	*punkCC1 = 0;
	pimg->GetColorConvertor(&punkCC1);

	if (punkCC1 == 0)
		return _bstr_t("");

	IColorConvertor2Ptr sptrCC1 = punkCC1;
	punkCC1->Release();

	if (sptrCC1 == 0)
		return _bstr_t("");
	
	BSTR bstr;
	sptrCC1->GetCnvName(&bstr);
	_bstr_t bstrN1(bstr);
	::SysFreeString(bstr);
	return bstrN1;
}


CActionCreateImageByList::CActionCreateImageByList()
{
	m_bUseColor = true;
}

CActionCreateImageByList::~CActionCreateImageByList()
{

}

bool CActionCreateImageByList::InvokeFilter()
{
	IUnknown *punkArgument = GetDataArgument("Objects");
	IUnknown *punkResult   = GetDataResult();

	if( !punkResult || !punkArgument )
		return false;

	INamedDataObject2Ptr	ptrListObject = punkArgument;
	
	if( ptrListObject == 0 )
		return false;

	long nCount = 0;
	ptrListObject->GetChildsCount( &nCount );

	StartNotification(nCount + 2, 1);

	nCount = 0;

	if( !_create_image_copy( punkResult, punkArgument ) )
		return false;

	Notify( nCount++ );

	if( !_fill_bk_color( punkResult ) )
		return false;

//	punkArgument->Release(); punkArgument = 0;
	Notify( nCount++ );

				    
	TPOS lPos = 0;
	ptrListObject->GetFirstChildPosition( &lPos );

	while( lPos )
	{
		IUnknown *punkChild = 0;
		ptrListObject->GetNextChild( &lPos, &punkChild );
		
		if( !punkChild )
			continue;

		IMeasureObjectPtr ptrMeasObject = punkChild;
		punkChild->Release(); punkChild = 0;

		if( ptrMeasObject == 0 )
			continue;
	
		IUnknown *punkImage = 0;
		ptrMeasObject->GetImage( &punkImage );

		if( !punkImage )
			continue;

		_copy_image( punkResult, punkImage );
		punkImage->Release();	punkImage = 0;

		Notify( nCount++ );
	}

	FinishNotification();

	return true;
}

bool CActionCreateImageByList::_copy_image(IUnknown *punkResult, IUnknown *punkObject)
{
	IImage3Ptr ptrResult = punkResult;
	IImage3Ptr ptrObject = punkObject;

	if( ptrResult == 0 || ptrObject == 0 )
		return false;
	
	int nSzX = 0, nSzY = 0;
	POINT ptOffset;

	ptrObject->GetOffset( &ptOffset );
	ptrObject->GetSize( &nSzX, &nSzY );

	int nPanes = ::GetImagePaneCount( ptrResult );

	for(int j = 0; j < nSzY; j++ )
	{
		LPBYTE NewImgRow = 0;
		ptrResult->GetRowMask( j + ptOffset.y, &NewImgRow );

		for (int k = 0; k < nPanes; k++)
		{
			color *SrcRowColor = 0;
			color *DestRowColor = 0;

			ptrObject->GetRow( j, k, &SrcRowColor );
			ptrResult->GetRow( j + ptOffset.y, k, &DestRowColor );

			LPBYTE SrcImgRow = 0;
			ptrObject->GetRowMask( j, &SrcImgRow );

			if( !SrcRowColor || !DestRowColor || !SrcImgRow )
				return false;
					 
			for(int i = 0; i < nSzX; i++ )
				if( SrcImgRow[i] == 0xFF )
					DestRowColor[i + ptOffset.x] = SrcRowColor[i];
		}
	}
	return true;
}

bool CActionCreateImageByList::_fill_bk_color( IUnknown *punkImage )
{
	IImage3Ptr ptrImage = punkImage;
	if( ptrImage == 0 )
		return false;

	DWORD dw_flags = 0;
	ptrImage->GetImageFlags( &dw_flags );
	if( dw_flags & ifVirtual )
		return false;

	IColorConvertor5Ptr ptrCC;
	{
		IUnknown* punkCC = 0;
		ptrImage->GetColorConvertor( &punkCC );
		ptrCC = punkCC;
		if( punkCC )
			punkCC->Release();
	}

	if( ptrCC == 0 )
		return false;

	int nPanes = ::GetImagePaneCount( ptrImage );
	if( nPanes < 1 )
		return false;

	int cx = 0, cy = 0;
	ptrImage->GetSize( &cx, &cy );

	m_bUseColor = nPanes > 1;
	color		*colorsBacks = new color[nPanes];
	COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Editor", m_bUseColor ? "Back" : "Back gray", RGB( 0, 0, 0 ) );

	{
		byte	dib[3];
		dib[0] = (byte)GetBValue(cr); 
		dib[1] = (byte)GetGValue(cr);
		dib[2] = (byte)GetRValue(cr);

		color	**ppcolor = new color*[nPanes];
		for( int i = 0; i < nPanes; i++ )
			ppcolor[i] = &colorsBacks[i];

		ptrCC->ConvertDIBToImageEx( dib, ppcolor, 1, TRUE, nPanes );
	}

	for( int k = 0; k <nPanes; k++ )
	{
		color	colorBack = colorsBacks[k];

		for( int j = 0; j < cy; j++ )
		{			
			color* pColor = 0;
			ptrImage->GetRow( j, k, &pColor );
			for( int i = 0; i < cx; i++ )
				pColor[i] = colorBack;
		}
	}
	delete []colorsBacks;

	return true;

}

bool CActionCreateImageByList::_create_image_copy(IUnknown *punkResult, IUnknown *punkSource)
{
	IImage3Ptr ptrResult = punkResult;

	INamedDataObject2Ptr	ptrListObject = punkSource;
	
	if( ptrListObject == 0 )
		return false;


	TPOS lPos = 0;
	ptrListObject->GetFirstChildPosition( &lPos );

	int cx = 0, cy = 0;

	_bstr_t bstr;

	IUnknown *punkImage = GetDataArgument("Image");
	IImage3Ptr ptrImage = punkImage;

	if( ptrImage == 0 )
	{
		while( lPos )
		{
			IUnknown *punkChild = 0;
			ptrListObject->GetNextChild( &lPos, &punkChild );
			
			if( !punkChild )
				continue;

			IMeasureObjectPtr ptrMeasObject = punkChild;
			punkChild->Release(); punkChild = 0;

			if( ptrMeasObject == 0 )
				continue;
		
			IUnknown *punkImage = 0;
			ptrMeasObject->GetImage( &punkImage );

			if( !punkImage )
				continue;

			IImage3Ptr ptrChild = punkImage;
			punkImage->Release(); punkImage = 0;

			if( !bstr )
				bstr = _GetCCName( ptrChild );

			if( ptrChild == 0 )
				continue;

			POINT ptOffset;
			int nX = 0, nY = 0;

			ptrChild->GetOffset( &ptOffset );
			ptrChild->GetSize( &nX, &nY  );

			if( ptOffset.x + nX > cx )
				cx = ptOffset.x + nX;

			if( ptOffset.y + nY > cy )
				cy = ptOffset.y + nY;
		}
	}
	else	
	{	
		ptrImage->GetSize( &cx, &cy );
		bstr = _GetCCName( ptrImage );
	}

	if( !cx || !cy )
		return false;

	ptrResult->CreateImage( cx, cy, bstr, ::GetImagePaneCount( ptrImage ) );
	ptrResult->InitRowMasks();

	// [vanek] BT2000:3859 - 06.07.2004
	if( ptrImage != 0 && CheckInterface( ptrImage, IID_INamedData ) )
	{
        INamedDataObject2Ptr	ptr = ptrResult;
		ptr->InitAttachedData();
    	CopyNamedData( ptr, ptrImage );
	}

	// [vanek] BT2000:4015 copy propbag - 20.09.2004
	INamedPropBagPtr	sptr_pb_source = ptrImage,
						sptr_pb_result = ptrResult;
	if(sptr_pb_source != 0 && sptr_pb_result != 0 )
	{
		long lpos_prop = 0;
		sptr_pb_source->GetFirstPropertyPos( &lpos_prop );
		while( lpos_prop ) 
		{
			_bstr_t bstr_name;
			_variant_t var_value; 
			if( S_OK != sptr_pb_source->GetNextProperty( bstr_name.GetAddress(), var_value.GetAddress( ), &lpos_prop ) )
				break;

            sptr_pb_result->SetProperty( bstr_name, var_value );
		}
	}

	return true;
}

