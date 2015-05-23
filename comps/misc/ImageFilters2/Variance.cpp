// Variance.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Variance.h"
#include "misc_utils.h"
#include <math.h>

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "ImagePane.h"

#define MaxColor color(-1)
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//
//
//		Variance
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CVarianceInfo::s_pargs[] = 
{	
	{"AddToSource",		szArgumentTypeInt, "0", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

bool CVarianceFilter::InvokeFilter()
{
	int	nAdd = GetArgLong( "AddToSource" );
	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;
	{	
		ICompatibleObjectPtr ptrCO( ptrDestImage );
		
		if( ptrCO == NULL )
			return false;

		if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
		{			
			return false;
		}		
	}

	// [vanek] 12.12.2003: works with color convertors panes
	CImagePaneCache paneCache( ptrSrcImage );
    //int nPaneCount = ::GetImagePaneCount( ptrSrcImage );
	int nPaneCount = paneCache.GetPanesCount( );
	SIZE size;
	
	ptrSrcImage->GetSize((int*)&size.cx,(int*)&size.cy);
	
	StartNotification( size.cy, 3 );
	double	fVal;
	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}
		bool bEnabled =IsPaneEnable(nPane);

		//[AY]
		long	x, y;
		color *pSrc1,*pSrc2,*pSrc3,*pDst;
		byte *pmask;

		ptrSrcImage->GetRowMask( 0 ,&pmask);
		ptrDestImage->GetRow( 0, nPane ,&pDst );
		ptrSrcImage->GetRow( 0, nPane ,&pSrc1 );

		for( x = 0; x < size.cx; x++, pmask++, pSrc1++, pDst++  )
			if( *pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }


		ptrSrcImage->GetRowMask( size.cy-1 ,&pmask);
		ptrDestImage->GetRow( size.cy-1, nPane ,&pDst );
		ptrSrcImage->GetRow( size.cy-1, nPane ,&pSrc1 );

		for( x = 0; x < size.cx; x++, pmask++, pSrc1++, pDst++  )
			if( *pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }

		for( y=1;y<size.cy-1;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask);
			ptrDestImage->GetRow( y, nPane ,&pDst );
			ptrSrcImage->GetRow( y-1, nPane ,&pSrc3 );
			ptrSrcImage->GetRow( y, nPane ,&pSrc1 );
			ptrSrcImage->GetRow( y+1, nPane ,&pSrc2 );

			if( *pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }

			for( x = 1; x < size.cx-1; x++, pmask++, pSrc1++, pSrc2++,pSrc3++,pDst++  )
			{
				if( *pmask==255 )
				{
					if(bEnabled)
					{
						fVal = 0;
						fVal+=(*(pSrc1)-*(pSrc3-1))*(*(pSrc1)-*(pSrc3-1));
						fVal+=(*(pSrc1)-*(pSrc3))*(*(pSrc1)-*(pSrc3));
						fVal+=(*(pSrc1)-*(pSrc3+1))*(*(pSrc1)-*(pSrc3+1));
						fVal+=(*(pSrc1)-*(pSrc2-1))*(*(pSrc1)-*(pSrc2-1));
						fVal+=(*(pSrc1)-*(pSrc2))*(*(pSrc1)-*(pSrc2));
						fVal+=(*(pSrc1)-*(pSrc2+1))*(*(pSrc1)-*(pSrc2+1));
						fVal+=(*(pSrc1)-*(pSrc1-1))*(*(pSrc1)-*(pSrc1-1));
						fVal+=(*(pSrc1)-*(pSrc1+1))*(*(pSrc1)-*(pSrc1+1));

						fVal = sqrt(fVal);
						if( nAdd )
							*pDst = ToColor(fVal+*pSrc1);
						else
							*pDst = ToColor(fVal);
					}
					else
						*pDst = *pSrc1;
				}
			}
			if( *pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }

			Notify( y );
		}

		NextNotificationStage( );
	}
	FinishNotification();
	return true;
}

