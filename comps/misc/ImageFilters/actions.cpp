
#include "stdafx.h"
#include "actions.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"


#include "ImagePane.h"


/////////////////////////////////////////////////////////////////////////////
//
//
//		CAutomaticBCFilter
//
//
/////////////////////////////////////////////////////////////////////////////
#define szArgInputWhiteLevel	"InputWhiteLevel"
#define szArgInputBlackLevel	"InputBlackLevel"

#define szArgOutputWhiteLevel	"OutputWhiteLevel"
#define szArgOutputBlackLevel	"OutputBlackLevel"

_ainfo_base::arg	CAutomaticBCInfo::s_pargs[] = 
{	
	{szArgInputBlackLevel,	szArgumentTypeInt, "0", true, false },
	{szArgInputWhiteLevel,	szArgumentTypeInt, "0", true, false },
	{szArgOutputBlackLevel,	szArgumentTypeInt, "0", true, false },
	{szArgOutputWhiteLevel,	szArgumentTypeInt, "100", true, false },
	{"Img",				szArgumentTypeImage, 0, true, true },
	{"Result",			szArgumentTypeImage, 0, false, true },	
	{0, 0, 0, false, false },
};
		 

#define LAB_LIGHTNESS_INDEX  2

bool CAutomaticBCFilter::InvokeFilter()
{

	long nInputBlackLevel	= GetArgLong( szArgInputBlackLevel );
	long nInputWhiteLevel	= GetArgLong( szArgInputWhiteLevel );	

	if( nInputBlackLevel < 0 || nInputBlackLevel > 100 )
		return false;

	if( nInputWhiteLevel < 0 || nInputWhiteLevel > 100 )
		return false;

	if( nInputWhiteLevel + nInputBlackLevel > 100 )
		return false;


	long nOutputBlackLevel	= GetArgLong( szArgOutputBlackLevel );
	long nOutputWhiteLevel	= GetArgLong( szArgOutputWhiteLevel );	

	if( nOutputBlackLevel < 0 || nOutputBlackLevel > 100 )
		return false;

	if( nOutputWhiteLevel < 0 || nOutputWhiteLevel > 100 )
		return false;



	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );


	CImagePaneCache paneCache( ptrSrcImage );


	
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


	//Process Image

	int cx, cy;
	ptrSrcImage->GetSize( &cx, &cy );

	int nPanesSrc = ::GetImagePaneCount( ptrSrcImage );

	if( nPanesSrc < 1 )
		return false;

	int colors = nPanesSrc;

	DWORD* pHist = new DWORD[colorMax];
	_ptrKiller_t<DWORD>	pk6( pHist );

	int x, y;

	StartNotification( cy * 2, colors );

	for( int nPane=0;nPane<colors;nPane++ )
	{		
		if( !paneCache.IsPaneEnable( nPane ) ) 
		{			
			/*
			for( y = 0; y < cy; y++ )
			{
				color* pColorSrc = 0;
				ptrSrcImage->GetRow( y, nPane, &pColorSrc );

				color* pColorDest = 0;
				ptrDestImage->GetRow( y, nPane, &pColorDest );

				BYTE* pMask = 0;
				ptrDestImage->GetRowMask( y, &pMask );

				for( x = 0; x  < cx; x++ )
					if( pMask[x] == 255 )
						pColorDest[x] = pColorSrc[x]; 

				Notify( y * 2 );
			}
			*/

			continue;
		}

		::ZeroMemory( pHist, sizeof(DWORD) * colorMax );

		color Lwhite = colorMax;
		color Lblack = 0;


		color cMin = Lwhite;
		color cMax = Lblack;


		//fill histogram
		for( y = 0; y < cy; y++ )
		{
			color* pColorSrc = 0;
			ptrSrcImage->GetRow( y, nPane, &pColorSrc );

			for( x = 0; x  < cx; x++ )
			{
				color cValue = pColorSrc[ x ];//pColorSrc[ y * cx * colors + nPane * cx + x];			

				if( cValue > cMax ) 
					cMax = cValue;
				
				if( cValue < cMin ) 
					cMin = cValue;
				
				if( cValue >= 0 && cValue < colorMax )
				{
					pHist[cValue] += ((DWORD)1);
				}
			}
			
			Notify( y );
		}


		if(0) for( int i=0;i<colorMax;i++ )
		{
			if( !pHist[i] )
				continue;

			char sz[1024];
			sprintf( sz, "\n%d\t\t\t%d", i, pHist[i] );
			OutputDebugString( sz );
		}



		double fRange1 = ((double)( cMax - cMin )) / 100.0;
		double fRange2 = ((double)( Lwhite - Lblack )) / 100.0;

		
		color cMinInputLevel = 0;
		color cMaxInputLevel = 0;

		{
			DWORD dwMinReqArea = (DWORD)( (double)(nInputBlackLevel) / 100.0 * (double)cx * (double)cy );
			DWORD dwMaxReqArea = (DWORD)( (double)(nInputWhiteLevel) / 100.0 * (double)cx * (double)cy );

			DWORD dwMinArea = 0;
			DWORD dwMaxArea = 0;

			color colorIndex = 0;
			bool bEnough = false;
			while( !bEnough )
			{
				if( dwMinArea <= dwMinReqArea )
				{
					dwMinArea += pHist[colorIndex];			
					cMinInputLevel = colorIndex;
				}
				
				if( dwMaxArea <= dwMaxReqArea )
				{
					dwMaxArea += pHist[colorMax - colorIndex-1];
					cMaxInputLevel = colorMax - colorIndex-1;
				}
				
				if( dwMinArea > dwMinReqArea && dwMaxArea > dwMaxReqArea )
					bEnough = true;

				colorIndex++;

				if( colorIndex >= colorMax ) 	
					bEnough = true;				
			
			}
		}

		

		color cMinOutputLevel = Lblack + (color)( fRange2 * (double)(nOutputBlackLevel) );
		color cMaxOutputLevel = Lblack + (color)( fRange2 * (double)(nOutputWhiteLevel) );

		if( cMaxInputLevel < cMinInputLevel )
			continue;

		if( cMaxOutputLevel < cMinOutputLevel )
			continue;


		double fDevider = ( (double)(cMaxInputLevel - cMinInputLevel ) ) / ( (double)(Lwhite - Lblack ) ); 

		if( fDevider == 0 )
			continue;

		for( y = 0; y < cy; y++ )
		{
			color* pColorSrc = 0;
			ptrSrcImage->GetRow( y, nPane, &pColorSrc );

			color* pColorDest = 0;
			ptrDestImage->GetRow( y, nPane, &pColorDest );

			BYTE* pMask = 0;
			ptrDestImage->GetRowMask( y, &pMask );

			for( x = 0; x  < cx; x++ )
			{				
				color cValue = pColorSrc[ x ];
				
				color cNewValue = cValue;

				if( cValue < cMinInputLevel )
					cNewValue = cMinOutputLevel;
				else
				if( cValue > cMaxInputLevel )
				{
					cNewValue = cMaxOutputLevel;
				}
				else
				{
					cNewValue = (color)( (double)(cValue - cMinInputLevel ) / fDevider );
				}


				if( pMask[x] == 255 )
					pColorDest[x] = cNewValue;

			}

			Notify( cy + y );
		}		

		if( nPane != colors - 1 )
			NextNotificationStage();	
	}

	FinishNotification();

	return true;


}

color CAutomaticBCFilter::GetLightnessValue( color B, color G, color R )
{
	IColorConvertor4Ptr ptrCCRGB = GetCCByName( "RGB" );		
	IColorConvertor4Ptr ptrCCHSB = GetCCByName( "HSB" );

	if( ptrCCRGB == NULL || ptrCCHSB == NULL )
		return 0;

	double pLab[3];		

	color	colorsRGB[3];
	colorsRGB[0] = B;
	colorsRGB[1] = G;
	colorsRGB[2] = R;

	color	*ppcolorRGB[3];
	for( int i = 0; i < 3; i++ )
	{			
		ppcolorRGB[i] = &colorsRGB[i];
	}

	ptrCCRGB->ConvertImageToLAB( ppcolorRGB, pLab, 1);
	ptrCCHSB->ConvertLABToImage(pLab, ppcolorRGB, 1);

	return (*ppcolorRGB[LAB_LIGHTNESS_INDEX]);
}

/////////////////////////////////////////////////////////////////////////////
//
//
//		CSelectColorFilter
//
//
/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CSelectColorInfo::s_pargs[] = 
{	
	{"ddd",		szArgumentTypeInt, "0", false, false },
	{"scwasc",	szArgumentTypeInt, "0", false, false },
	{"Img",				szArgumentTypeImage, 0, false, true },
	{"Result",			szArgumentTypeImage, 0, true, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////
bool CSelectColorFilter::InvokeFilter()
{

	/*
	long nBlackLevel	= GetArgLong( szArgBlackLevel );
	long nWhiteLevel	= GetArgLong( szArgWhiteLevel );	
	*/

	IImage3Ptr	ptrSrcImage( GetDataArgument() );
	IImage3Ptr	ptrDestImage( GetDataResult() );
	
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;

	return true;
}



/////////////////////////////////////////////////////////////////////////////
//
//
//		CApplyLookupFilter
//
//
/////////////////////////////////////////////////////////////////////////////
_ainfo_base::arg	CApplyLookupInfo::s_pargs[] = 
{	
	{"Img",				szArgumentTypeImage, 0, false, true },
	{"Result",			szArgumentTypeImage, 0, true, true },	
	{0, 0, 0, false, false },
};


/////////////////////////////////////////////////////////////////////////////
bool CApplyLookupFilter::InvokeFilter()
{	
	IImage2Ptr	ptrSrcImage (GetDataArgument());
	IImage2Ptr	ptrDestImage(GetDataResult());
		
	if( ptrSrcImage == NULL || ptrDestImage == NULL )
		return false;

	ICompatibleObjectPtr ptrCO( ptrDestImage );
	if( ptrCO == NULL )
		return false;
	if( S_OK != ptrCO->CreateCompatibleObject( ptrSrcImage, NULL, 0 ) )
		return false;

	IUnknown *punkCC = 0;
	ptrSrcImage->GetColorConvertor( &punkCC );
	
	_ptr_t2<byte>	pLookup;
	//byte* pLookup = 0;
	long  nLookupSize = 0;

	INamedDataObject2Ptr sptrNDO2(ptrSrcImage);
	if(sptrNDO2 != 0)
	{
		IUnknown* punkParent = 0;
		sptrNDO2->GetParent(&punkParent);
		if(punkParent)
		{
			sptrNDO2 = punkParent;
			punkParent->Release();
		}
		pLookup.attach( GetValuePtr(sptrNDO2, "LookupAction", "LookupTable", &nLookupSize) );
	}

	if(!pLookup)
		return false;

	int cx, cy;
	ptrSrcImage->GetSize( &cx, &cy );

	int nColors = ::GetImagePaneCount( ptrSrcImage );
	
	StartNotification(cy);

	for(long y = 0; y < cy; y++)
	{
		for(long c = 0; c < nColors; c++)
		{
			color* pColorSrc = 0;
			ptrSrcImage->GetRow( y, c, &pColorSrc );

			color* pColorDest = 0;
			ptrDestImage->GetRow( y, c, &pColorDest );

			BYTE* pMask = 0;
			ptrSrcImage->GetRowMask( y, &pMask );

			for(long x = 0; x < cx; x++)
			{
				if(pMask[x] == 255)
				{
					long nLookupPos = c*256 + ColorAsByte(pColorSrc[x]);
					if(nLookupPos < nLookupSize)
						pColorDest[x] = ByteAsColor(pLookup[nLookupPos]);
					else
						pColorDest[x] = pColorSrc[x];
				}
				else
					pColorDest[x] = pColorSrc[x];
			}
		}

		Notify(y);
	}

	FinishNotification();

	return true;
}

