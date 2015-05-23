#include "stdafx.h"
#include "filteractions.h"
#include "resource.h"


#include "binimageint.h"
#include "binary.h"

#include "SelBinConv.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionCreateSelectionByBinary, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCreateBinaryBySelection, CCmdTargetEx);



// olecreate 
// {269EC437-574B-4175-81A8-CA70AC38AC11}
GUARD_IMPLEMENT_OLECREATE(CActionCreateSelectionByBinary, "FiltersMain.CreateSelectionByBinary", 
0x269ec437, 0x574b, 0x4175, 0x81, 0xa8, 0xca, 0x70, 0xac, 0x38, 0xac, 0x11);
// {204AD8B0-DFC5-4e19-B3A2-1F6BF6189A1D}
GUARD_IMPLEMENT_OLECREATE(CActionCreateBinaryBySelection, "FiltersMain.CreateBinaryBySelection", 
0x204ad8b0, 0xdfc5, 0x4e19, 0xb3, 0xa2, 0x1f, 0x6b, 0xf6, 0x18, 0x9a, 0x1d);

// guidinfo 

// {75695361-EDFE-11d4-9D41-009027982EB4}
static const GUID guidCreateSelectionByBinary = 
{ 0x75695361, 0xedfe, 0x11d4, { 0x9d, 0x41, 0x0, 0x90, 0x27, 0x98, 0x2e, 0xb4 } };

// {75695362-EDFE-11d4-9D41-009027982EB4}
static const GUID guidCreateBinaryBySelection = 
{ 0x75695362, 0xedfe, 0x11d4, { 0x9d, 0x41, 0x0, 0x90, 0x27, 0x98, 0x2e, 0xb4 } };


ACTION_INFO_FULL(CActionCreateSelectionByBinary, IDS_ACTION_CREATE_SELECTION_BY_BINARY, IDS_MENU_FILTERS, IDS_TB_MAIN, guidCreateSelectionByBinary);
ACTION_INFO_FULL(CActionCreateBinaryBySelection, IDS_ACTION_CREATE_BINARY_BY_SELECTION, IDS_MENU_FILTERS, IDS_TB_MAIN, guidCreateBinaryBySelection);


//[ag]7. targets
ACTION_TARGET(CActionCreateSelectionByBinary, szTargetAnydoc);
ACTION_TARGET(CActionCreateBinaryBySelection, szTargetAnydoc);
//[ag]8. arguments

#define szSourceImage	"Source"
#define szBinary		"BinImage"
#define szSelection		"Selection"


ACTION_ARG_LIST(CActionCreateSelectionByBinary)
	ARG_IMAGE( szSourceImage )
	ARG_BINIMAGE( szBinary )	
	RES_IMAGE( szSelection )	
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionCreateBinaryBySelection)
	ARG_IMAGE( szSelection )	
	ARG_IMAGE( szSourceImage )	
	RES_BINIMAGE( szBinary )	
END_ACTION_ARG_LIST();


//////////////////////////////////////////////////////////////////////
//CActionCreateSelectionByBinary implementation
CActionCreateSelectionByBinary::CActionCreateSelectionByBinary()
{
}

//////////////////////////////////////////////////////////////////////
CActionCreateSelectionByBinary::~CActionCreateSelectionByBinary()
{

}

//////////////////////////////////////////////////////////////////////
/*bool CActionCreateSelectionByBinary::CanDeleteArgument( CFilterArgument *pa )
{
	if( pa->m_strArgName == szSourceImage )
		return false;
	return CFilterBase::CanDeleteArgument( pa );
}*/


//////////////////////////////////////////////////////////////////////
/*bool CActionCreateSelectionByBinary::InvokeFilter()
{

	CImageWrp		image( GetDataArgument( szSourceImage ) );
	CBinImageWrp	binary( GetDataArgument( szBinary ) );
	
	CImageWrp		imageNew( GetDataResult( szSelection ) );

	if( image == NULL || binary == NULL || imageNew == NULL )
		return false;
		

	//byte* m_pImageMask;
	CRect rcSelection = NORECT;

	long cxImage = image.GetWidth();
	long cyImage = image.GetHeight();
	CPoint ptOffsetImage = image.GetOffset();

	long cxBinary = binary.GetWidth();
	long cyBinary = binary.GetHeight();

	

	CPoint ptOffsetBinary;
	binary->GetOffset( &ptOffsetBinary );


	if( cxImage < 1 || cyImage < 1 || cxBinary < 1 || cyBinary < 1)
		return false;
	

	
	byte* pImageMask = NULL;

	

	//Find rect on image
	bool bFirstEntry = true;
	for( int y = 0; y < cyBinary; y++ )
	{		
		byte* binptr = 0;
		binary->GetRow(&binptr, y, FALSE);

		for( long x = 0; x < cxBinary; x++ )
		{			
			//if( binptr[x/8] && ( 0x80>>( x % 8 ) ) )
      if( binptr[x] != 0 )
			{
				if( bFirstEntry )
				{
					bFirstEntry = false;
					rcSelection.left = rcSelection.right = x;
					rcSelection.top = rcSelection.bottom = y;
				}

				rcSelection.left	= min( rcSelection.left, x );
				rcSelection.right	= max( rcSelection.right, x );
				
				rcSelection.top		= min( rcSelection.top, y );
				rcSelection.bottom	= max( rcSelection.bottom, y );				
			}
		}		
	}	

	CPoint ptSelBinOffset = rcSelection.TopLeft();

	
	//Note: rcSelection - in binary local coordinates
	//Convert to absolute coordinate
	rcSelection.left	+= ptOffsetBinary.x;
	rcSelection.right	+= ptOffsetBinary.x;

	rcSelection.top		+= ptOffsetBinary.y;
	rcSelection.bottom	+= ptOffsetBinary.y;
	//Convert to image coordinates
	rcSelection.left	-= ptOffsetImage.x;
	rcSelection.right	-= ptOffsetImage.x;

	rcSelection.top		-= ptOffsetImage.y;
	rcSelection.bottom	-= ptOffsetImage.y;


	if( rcSelection.Width() < 1 || rcSelection.Height() < 1 )
	{		
		return false;
	}

	{
		INamedDataObject2Ptr ptrSel( imageNew );
		if( ptrSel )
			ptrSel->SetParent( image, 0 );

	}

	imageNew->FreeImage();
	imageNew->CreateVirtual( rcSelection );

	
	
	StartNotification( cyImage );

	//Now Create VImage
	//Copy image mask	
	imageNew.SetOffset( CPoint( ptOffsetImage.x + rcSelection.left, ptOffsetImage.y + rcSelection.top), TRUE );
	imageNew.InitRowMasks();

	int nSelWidth  = rcSelection.Width();
	int nSelHeight = rcSelection.Height();

	int xBin, yBin;
	
	for( y = 0; y < nSelHeight; y++ )
	{
		byte * pDst = imageNew.GetRowMask( y );

		yBin = y + ptSelBinOffset.y;

		byte* binptr = 0;
		binary->GetRow(&binptr, yBin, FALSE);

		for( long x = 0; x < nSelWidth; x++ )
		{
			xBin = ptSelBinOffset.x + x;
			//if( binptr[xBin/8] & ( 0x80>>( xBin % 8 ) ) )			
      if( binptr[xBin] != 0 )			
				pDst[x] = 0xFF;			
			else			
				pDst[x] = 0x00;			
		}
		int nIndex = (int)((double)y / (double)nSelHeight * (double)cyImage);
		Notify( nIndex );
	}

	imageNew.InitContours( );	

	FinishNotification();

	
	return true;
}*/

bool CActionCreateSelectionByBinary::Invoke()
{
	IUnknownPtr punkImage(GetActiveObjectFromDocument(m_punkTarget, szArgumentTypeImage), false);
	CImageWrp		image(punkImage);
	IUnknownPtr punkBin(GetActiveObjectFromDocument(m_punkTarget, szArgumentTypeBinaryImage), false);
	CBinImageWrp	binary(punkBin);
	
	CImageWrp		imageNew;

	if( image == NULL || binary == NULL )
		return false;


	CImageWrp	imageMask = image;
	CRect rcImage(image.GetRect()); // Image rectangle in document coordinates
	INamedDataObject2Ptr sptrNDO2(punkImage);
	if (sptrNDO2 == 0) return false;
	IUnknownPtr punkImageParent;
	sptrNDO2->GetParent(&punkImageParent);
	if (punkImageParent != 0)
		image = punkImageParent;


	//byte* m_pImageMask;
	CRect rcSelection = NORECT;

	long cxImage = image.GetWidth();
	long cyImage = image.GetHeight();
	CPoint ptOffsetImage = image.GetOffset();

	long cxBinary = binary.GetWidth();
	long cyBinary = binary.GetHeight();

	

	CPoint ptOffsetBinary;
	binary->GetOffset( &ptOffsetBinary );


	if( cxImage < 1 || cyImage < 1 || cxBinary < 1 || cyBinary < 1)
		return false;
	

	
	byte* pImageMask = NULL;

	

	//Find rect on image
	bool bFirstEntry = true;
	for( int y = 0; y < cyBinary; y++ )
	{		
		byte* binptr = 0;
		binary->GetRow(&binptr, y, FALSE);

		for( long x = 0; x < cxBinary; x++ )
		{			
			//if( binptr[x/8] && ( 0x80>>( x % 8 ) ) )
      if( binptr[x] != 0 )
			{
				if( bFirstEntry )
				{
					bFirstEntry = false;
					rcSelection.left = rcSelection.right = x;
					rcSelection.top = rcSelection.bottom = y;
				}

				rcSelection.left	= min( rcSelection.left, x );
				rcSelection.right	= max( rcSelection.right, x );
				
				rcSelection.top		= min( rcSelection.top, y );
				rcSelection.bottom	= max( rcSelection.bottom, y );				
			}
		}		
	}	

//	CPoint ptSelBinOffset = rcSelection.TopLeft();
	
	//Note: rcSelection - in binary local coordinates
	//Convert to absolute coordinate
	rcSelection.left	+= ptOffsetBinary.x;
	rcSelection.right	+= ptOffsetBinary.x;
	rcSelection.top		+= ptOffsetBinary.y;
	rcSelection.bottom	+= ptOffsetBinary.y;

	// Clip area outside image rectangle.
	if (rcSelection.left < rcImage.left)
		rcSelection.left = rcImage.left;
	if (rcSelection.top < rcImage.top)
		rcSelection.top = rcImage.top;
	if (rcSelection.right > rcImage.right)
		rcSelection.right = rcImage.right;
	if (rcSelection.bottom > rcImage.bottom)
		rcSelection.bottom = rcImage.bottom;
	// New selection offset in binary coordinates.
	CPoint ptSelBinOffset(rcSelection.left-ptOffsetBinary.x, rcSelection.top-ptOffsetBinary.y);

	//Convert to main image coordinates
	rcSelection.left	-= ptOffsetImage.x;
	rcSelection.right	-= ptOffsetImage.x;
	rcSelection.top		-= ptOffsetImage.y;
	rcSelection.bottom	-= ptOffsetImage.y;


	if( rcSelection.Width() < 1 || rcSelection.Height() < 1 )
	{		
		return false;
	}

	IUnknownPtr punkNew(image.CreateVImage(rcSelection), false);
	imageNew = punkNew;


	
	
	//Now Create VImage
	//Copy image mask	
	imageNew.SetOffset( CPoint( ptOffsetImage.x + rcSelection.left, ptOffsetImage.y + rcSelection.top), TRUE );
	imageNew.InitRowMasks();

	int nSelWidth  = rcSelection.Width();
	int nSelHeight = rcSelection.Height();

	int xBin, yBin;
	
	for( y = 0; y < nSelHeight; y++ )
	{
		byte * pDst = imageNew.GetRowMask( y );

		yBin = y + ptSelBinOffset.y;

		byte* binptr = 0;
		binary->GetRow(&binptr, yBin, FALSE);

		byte* pRowMask = imageMask.GetRowMask(y+rcSelection.top-rcImage.top);
		pRowMask += rcSelection.left-rcImage.left;


		for( long x = 0; x < nSelWidth; x++ )
		{
			xBin = ptSelBinOffset.x + x;
			//if( binptr[xBin/8] & ( 0x80>>( xBin % 8 ) ) )			
		    if( binptr[xBin] != 0 && pRowMask[x] != 0 )
				pDst[x] = 0xFF;			
			else			
				pDst[x] = 0x00;			
		}
//		int nIndex = (int)((double)y / (double)nSelHeight * (double)cyImage);
	}

	imageNew.InitContours( );	

	m_changes.SetToDocData(m_punkTarget, imageNew);

	
	return true;
}

bool CActionCreateSelectionByBinary::DoUndo()
{
	m_changes.DoUndo(m_punkTarget);
	return true;
}

bool CActionCreateSelectionByBinary::DoRedo()
{
	m_changes.DoRedo(m_punkTarget);
	return true;
}



//////////////////////////////////////////////////////////////////////
//CActionCreateBinaryBySelection implementation
CActionCreateBinaryBySelection::CActionCreateBinaryBySelection()
{
}

//////////////////////////////////////////////////////////////////////
CActionCreateBinaryBySelection::~CActionCreateBinaryBySelection()
{

}

//////////////////////////////////////////////////////////////////////
bool CActionCreateBinaryBySelection::CanDeleteArgument( CFilterArgument *pa )
{
	if( pa->m_strArgName == szSourceImage )
		return false;
	return CFilterBase::CanDeleteArgument( pa );
}

//////////////////////////////////////////////////////////////////////
bool CActionCreateBinaryBySelection::CheckSelection()
{	

	CImageWrp		image( GetDataArgument( szSourceImage ) );
	CImageWrp		imageSel( GetDataArgument( szSelection ) );

	if( imageSel == NULL || image == NULL )
		return false;

	INamedDataObject2Ptr ptrNDO( imageSel );
	if( ptrNDO == NULL )
		return false;

	IUnknown* punkParent = NULL;
	ptrNDO->GetParent( &punkParent );
	if( !punkParent )
		return false;

	bool bRes = ( ::GetObjectKey(punkParent) == ::GetObjectKey(image) );

	punkParent->Release();
	punkParent = NULL;

	return bRes;

}

//////////////////////////////////////////////////////////////////////
bool CActionCreateBinaryBySelection::IsAvaible()
{
	return /*( CheckSelection() && */CFilterBase::IsAvaible() ;
}

//////////////////////////////////////////////////////////////////////
bool CActionCreateBinaryBySelection::InvokeFilter()
{

	CImageWrp		image( GetDataArgument( szSourceImage ) );
	CImageWrp		imageSel( GetDataArgument( szSelection ) );

	CBinImageWrp	binary( GetDataResult( szBinary ) );
	

	if( image == NULL || binary == NULL || imageSel == NULL )
		return false;
		

	//byte* m_pImageMask;
	CRect rcSelection = NORECT;

	long cxImage = image.GetWidth();
	long cyImage = image.GetHeight();

	long cxSel = imageSel.GetWidth();
	long cySel = imageSel.GetHeight();

	CPoint ptSelOffset = imageSel.GetOffset();


	if( cxImage < 1 || cyImage < 1 || cxSel < 1 || cySel < 1)
		return false;

	if( !CheckSelection() )
		return false;


	if( !binary.CreateNew( cxSel, cySel /*cxImage, cyImage*/ ) )
		return false;


	StartNotification( cySel );

	long xBin;//yBin;

	for( long y = 0; y < cySel; y++ )
	{		
		byte* pMask = imageSel.GetRowMask( y );

		byte* binptr = 0;
		binary->GetRow(&binptr, y, FALSE);

		for( long x = 0; x < cxSel; x++ )
		{
			xBin = x;// + ptSelOffset.x;
			if( pMask[ x ] == 0xFF )
			{
				//binptr[xBin/8] |= 0x80>>( xBin % 8 );
        binptr[xBin] = 0xFF;
			}			
		}
		Notify( y );
	}

	binary->SetOffset( ptSelOffset, TRUE );

/*
	for( int y = 0; y < cyImage; y++ )
	{
		byte* binptr = pbin+y*nBinaryRowSize;
		for( long x = 0; x < cxImage; x++ )
		{
			byte bMask = m_pbyteMark[ cx*y + x ];
			if( bMask == 0xFF )
			{
				binptr[x/8] |= 0x80>>( x % 8 );
			}
		}

		Notify( y );
	}
	*/



	FinishNotification();



	return true;

	

	/*
	int nBinaryRowSize = 0;
	byte* pbin = binary.GetData(nBinaryRowSize);
	if( pbin == NULL ) 
		return false;


	byte* pImageMask = NULL;

	

	//Create image mask
	bool bFirstEntry = true;
	for( int y = 0; y < cyBinary && y < cyImage; y++ )
	{		
		byte* binptr = pbin+y*nBinaryRowSize;
		pImageMask = image.GetRowMask( y );
		for( long x = 0; x < cxBinary && x < cxImage; x++ )
		{			
			if( binptr[x/8] && ( 0x80>>( x % 8 ) ) )
			{
				if( bFirstEntry )
				{
					bFirstEntry = false;
					rcSelection.left = rcSelection.right = x;
					rcSelection.top = rcSelection.bottom = y;
				}

				rcSelection.left	= min( rcSelection.left, x );
				rcSelection.right	= max( rcSelection.right, x );
				
				rcSelection.top		= min( rcSelection.top, y );
				rcSelection.bottom	= max( rcSelection.bottom, y );				
			}
		}		
	}	


	if( rcSelection.Width() < 1 || rcSelection.Height() < 1 )
	{		
		return false;
	}

	{
		INamedDataObject2Ptr ptrSel( imageNew );
		if( ptrSel )
			ptrSel->SetParent( image, 0 );

	}

	imageNew->FreeImage();
	imageNew->CreateVirtual( rcSelection );

	
	
	StartNotification( cyImage );



	//Now Create VImage
	//Copy image mask
	imageNew.InitRowMasks();

	int nSelWidth  = rcSelection.Width();
	int nSelHeight = rcSelection.Height();
	
	for( y = 0; y < cyBinary, y < nSelHeight; y++ )
	{
		byte * pDst = imageNew.GetRowMask( y );
		byte* binptr = pbin+y*nBinaryRowSize;

		for( long x = 0; x < cxBinary && x < nSelWidth; x++ )
		{
			
			if( binptr[x/8] && ( 0x80>>( x % 8 ) ) )
			{
				pDst[x] = 0xFF;
			}
			else
			{
				pDst[x] = 0x00;
			}
		}
		int nIndex = (int)((double)y / (double)nSelHeight * (double)cyImage);
		Notify( nIndex );
	}

	imageNew.InitContours();	

	FinishNotification();

	
	return true;
	*/
}
