// Gradient.cpp: implementation of the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Gradient.h"
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
//		Gradient
//
//
/////////////////////////////////////////////////////////////////////////////

_ainfo_base::arg	CGradientInfo::s_pargs[] = 
{	
	{"AddToSource",		szArgumentTypeInt, "0", true, false },
	{"Img",		szArgumentTypeImage, 0, true, true },
	{"Result",	szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////

bool CGradientFilter::InvokeFilter()
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

	double	fVal;
	

	StartNotification( size.cy, nPaneCount );
	for( int nPane=0;nPane<nPaneCount;nPane++ )
	{
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			continue;
		}
		bool bEnabled =IsPaneEnable(nPane);

		//[AY]
		long	x, y;
		byte *pmask;
		color *pSrc1,*pSrc2,*pSrc3,*pDst;
		ptrSrcImage->GetRowMask( 0 ,&pmask);
		ptrSrcImage->GetRow( 0, nPane ,&pSrc1 );
		ptrDestImage->GetRow( 0, nPane ,&pDst );
		
		for( x = 0; x < size.cx; x++, pmask++, pSrc1++, pDst++  )
			if( !bEnabled||*pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }


		ptrDestImage->GetRow( size.cy-1, nPane ,&pDst );
		ptrSrcImage->GetRowMask( size.cy-1 ,&pmask);
		ptrSrcImage->GetRow( size.cy-1, nPane ,&pSrc1 );

		for( x = 0; x < size.cx; x++, pmask++, pSrc1++, pDst++  )
			if( !bEnabled||*pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }

		for( y=1;y<size.cy-1;y++ )
		{
			ptrSrcImage->GetRowMask( y ,&pmask);

			ptrDestImage->GetRow( y, nPane ,&pDst );
			ptrSrcImage->GetRow( y-1, nPane ,&pSrc3 );
			ptrSrcImage->GetRow( y, nPane ,&pSrc1 );
			ptrSrcImage->GetRow( y+1, nPane ,&pSrc2 );


			if( !bEnabled||*pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }
			
			for( x = 1; x < size.cx-1; x++, pmask++, pSrc1++, pDst++,pSrc2++,pSrc3++  )
			{
				if( *pmask==255 )
				{
					if(bEnabled)
					{							
						//double _fval = (*(pSrc1+1)-*(pSrc1-1))*(*(pSrc1+1)-*(pSrc1-1)) + (*pSrc2-*pSrc3)*(*pSrc2-*pSrc3);
						//fVal = (color)(sqrt( _fval ));						
						fVal = _hypot(*(pSrc1+1)-*(pSrc1-1), *pSrc2-*pSrc3 );

						if( nAdd )
							*pDst = ToColor(fVal+*pSrc1);
						else
							*pDst = ToColor(fVal);
					}
					else
						*pDst=*pSrc1;
				}
			}			

			if( !bEnabled||*pmask ){ if( nAdd )*pDst = ToColor(*pSrc1);else *pDst = ToColor(0); }

			Notify( y );
		}

		NextNotificationStage( );
	}
	FinishNotification();

	return true;
}

