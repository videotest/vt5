#include "stdafx.h"
#include "fastactions.h"
#include "resource.h"


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionFastGradient, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFastUnsharpenMask, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFastAverage, CCmdTargetEx);

// olecreate 

// {1EF3C8B0-20EA-498b-8FA3-CE53AE080A2E}
GUARD_IMPLEMENT_OLECREATE(CActionFastGradient, "FiltersMain.FastGradient",
0x1ef3c8b0, 0x20ea, 0x498b, 0x8f, 0xa3, 0xce, 0x53, 0xae, 0x8, 0xa, 0x2e);
// {E31F4A4F-2D0D-4604-9BA5-37AFC7E149A3}
GUARD_IMPLEMENT_OLECREATE(CActionFastUnsharpenMask, "FiltersMain.FastUnsharpenMask",
0xe31f4a4f, 0x2d0d, 0x4604, 0x9b, 0xa5, 0x37, 0xaf, 0xc7, 0xe1, 0x49, 0xa3);
// {EEB4AAD7-5C62-4fbf-A7FE-82824ECD600F}
GUARD_IMPLEMENT_OLECREATE(CActionFastAverage, "FiltersMain.FastAverage",
0xeeb4aad7, 0x5c62, 0x4fbf, 0xa7, 0xfe, 0x82, 0x82, 0x4e, 0xcd, 0x60, 0xf);
// guidinfo 

// {B8000D7C-4B6B-46d5-A588-94261AF62248}
static const GUID guidFastGradient =
{ 0xb8000d7c, 0x4b6b, 0x46d5, { 0xa5, 0x88, 0x94, 0x26, 0x1a, 0xf6, 0x22, 0x48 } };
// {14140509-DAEF-48cd-BC52-BC03417A104F}
static const GUID guidFastUnsharpenMask =
{ 0x14140509, 0xdaef, 0x48cd, { 0xbc, 0x52, 0xbc, 0x3, 0x41, 0x7a, 0x10, 0x4f } };
// {7BE2F380-2439-4172-9712-E647C1A82892}
static const GUID guidFastAverage =
{ 0x7be2f380, 0x2439, 0x4172, { 0x97, 0x12, 0xe6, 0x47, 0xc1, 0xa8, 0x28, 0x92 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionFastGradient, IDS_ACTION_FASTGRADIENT, -1, -1, guidFastGradient);
ACTION_INFO_FULL(CActionFastUnsharpenMask, IDS_ACTION_FASTUNSHARPENMASK, -1, -1, guidFastUnsharpenMask);
ACTION_INFO_FULL(CActionFastAverage, IDS_ACTION_FASTAVERAGE, -1, -1, guidFastAverage);
//[ag]7. targets

ACTION_TARGET(CActionFastGradient, szTargetAnydoc);
ACTION_TARGET(CActionFastUnsharpenMask, szTargetAnydoc);
ACTION_TARGET(CActionFastAverage, szTargetAnydoc);

//[ag]8. arguments

ACTION_ARG_LIST(CActionFastGradient)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_INT(_T("Power"), 3 )
	ARG_INT(_T("Base"), 32000 )
	ARG_IMAGE( _T("SourceImage") )
	RES_IMAGE( _T("Gradient") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionFastAverage)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("SourceImage") )
	RES_IMAGE( _T("Average") )
	ARG_INT(_T("MaskSizeY"), -1 )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionFastUnsharpenMask)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_INT(_T("Threshold"), 0 )
	ARG_INT(_T("Strength"), 100 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("UnsharpenMask") )
END_ACTION_ARG_LIST();




//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionFastGradient implementation
CActionFastGradient::CActionFastGradient()
{
}

CActionFastGradient::~CActionFastGradient()
{
}

bool CActionFastGradient::InvokeFilter()
{
	CTimeTest	timeTest( true, "CActionFastAverage" );
	int	nMaskSize = GetArgumentInt( "MaskSize" );
	int	nPower = GetArgumentInt( "Power" );
	int	nBase = GetArgumentInt( "Base" );

	if( nMaskSize  < 1 )
		return false;


	CImageWrp	image = GetDataArgument();
	CImageWrp	imageNew = GetDataResult();
	CImageWrp	imageWork;

	if( image.IsEmpty()) return false;

	int	cx = image.GetWidth();
	int	cy = image.GetHeight();
	int	colors = image.GetColorsCount();

	StartNotification( cy+cx, colors, 100 );

	ICompatibleObjectPtr ptrCO( imageNew );
	ptrCO->CreateCompatibleObject( image, NULL, 0 );
	//imageNew.CreateCompatibleImage( image );

	if( !imageWork.CreateNew( cx, cy, image.GetColorConvertor() ) )
		return false;

	int	x, y, c, i;

	color	*pwork;
	color	*pnew;
	color	*psrc;
	byte	*pmask;

	color	**ppsrc = new color*[cy];
	color	**ppnew = new color*[cy];
	color	**ppwork = new color*[cy];
	byte	**ppmask = new byte*[cy];
//init masks
	for( y = 0; y < cy; y++ )
		ppmask[y] = image.GetRowMask( y );

	int	nMaskMin = nMaskSize/2;
	int	nMaskMax = nMaskSize-nMaskSize/2;

	long	nCurrentSum;

	bool bRet=true;
	try{

	CImagePaneCache paneCache( imageNew );

	for( c = 0; c <  colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		for( y = 0; y < cy; y++ )
		{
			ppsrc[y] = image.GetRow( y, c );
			ppnew[y] = imageNew.GetRow( y, c );
			ppwork[y] = imageWork.GetRow( y, c );
		}
//first step - average columns. Write the column sum to work image
		for( x = 0; x < cx; x++ )
		{
			nCurrentSum = 0;
			for( i = -nMaskMin; i < +nMaskMax; i++ )
				nCurrentSum+=ppsrc[max(0, min(cy-1, i))][x];

			for( y = 0; y < cy; y++ )
			{
				ppwork[y][x] = nCurrentSum/nMaskSize;
				nCurrentSum-=ppsrc[max(0, /*min(cy-1,*/ y-nMaskMin/*)*/)][x];
				nCurrentSum+=ppsrc[/*max(0,*/ min(cy-1, y+nMaskMax)/*)*/][x];

				//ASSERT(nCurrentSum>=0);
			}
			//if( x%2 )
				Notify( x );
		}

//second step. average rows
		for( y = 0; y < cy; y++, pwork++ )
		{
			psrc = ppsrc[y];
			pwork = ppwork[y];
			nCurrentSum = 0;
			for( i = -nMaskMin; i< +nMaskMax; i++ )
				nCurrentSum+=pwork[max(0, min(cx-1, i))];

			for( pmask = ppmask[y], pnew = ppnew[y], x = 0; x < cx; x++, pnew++, pmask++, psrc++ )
			{
				if( *pmask )
					*pnew = max( 0, min( colorMax, nBase+(*psrc-nCurrentSum/nMaskSize)*nPower ) );	

				nCurrentSum-=pwork[max(0, /*min(cx-1, */x-nMaskMin/*)*/)];
				nCurrentSum+=pwork[/*max(0,*/ min(cx-1, x+nMaskMax)/*)*/];
			}
			//if( y%2 )
				Notify( y+cx );
		}

		if( c != colors-1 )
			NextNotificationStage();
			
	}
	}
	catch( CException *pe )
	{
		pe->Delete();
		bRet = false;
	}

	delete ppsrc;
	delete ppnew;
	delete ppwork;
	delete ppmask;

	return bRet;
}

//////////////////////////////////////////////////////////////////////
//CActionFastUnsharpenMask implementation
CActionFastUnsharpenMask::CActionFastUnsharpenMask()
{
}

CActionFastUnsharpenMask::~CActionFastUnsharpenMask()
{
}

bool CActionFastUnsharpenMask::InvokeFilter()
{
	CTimeTest	timeTest( true, "CActionFastAverage" );
	int	nMaskSize = GetArgumentInt( "MaskSize" );
	int	nThreshold = GetArgumentInt( "Threshold" )*256;
	double fStrength = GetArgumentInt( "Strength" )/100.;

	if( nMaskSize  < 1 )
		return false;

	nThreshold = max(nThreshold, 0);
	fStrength = max(fStrength, 0);


	CImageWrp	image = GetDataArgument();
	CImageWrp	imageNew = GetDataResult();
	CImageWrp	imageWork;

	if( image.IsEmpty()) return false;

	int	cx = image.GetWidth();
	int	cy = image.GetHeight();
	int	colors = image.GetColorsCount();

	StartNotification( cy+cx, colors, 100 );

	ICompatibleObjectPtr ptrCO( imageNew );
	ptrCO->CreateCompatibleObject( image, NULL, 0 );

	//imageNew.CreateCompatibleImage( image );
	if( !imageWork.CreateNew( cx, cy, image.GetColorConvertor() ) )
		return false;

	int	x, y, c, i;

	color	*pwork;
	color	*pnew;
	color	*psrc;
	byte	*pmask;

	color	**ppsrc = new color*[cy];
	color	**ppnew = new color*[cy];
	color	**ppwork = new color*[cy];
	byte	**ppmask = new byte*[cy];
//init masks
	for( y = 0; y < cy; y++ )
		ppmask[y] = image.GetRowMask( y );

	int	nMaskMin = nMaskSize/2;
	int	nMaskMax = nMaskSize-nMaskSize/2;
	int	nHistArea = nMaskSize*nMaskSize;

	long	nCurrentSum;
	long	nRes, nCurrentHist;

	bool bRet=true;
	try{

	CImagePaneCache paneCache( imageNew );


	for( c = 0; c <  colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		for( y = 0; y < cy; y++ )
		{
			ppsrc[y] = image.GetRow( y, c );
			ppnew[y] = imageNew.GetRow( y, c );
			ppwork[y] = imageWork.GetRow( y, c );
		}
//first step - average columns. Write the column sum to work image
		for( x = 0; x < cx; x++ )
		{
			nCurrentSum = 0;
			for( i = -nMaskMin; i < +nMaskMax; i++ )
				nCurrentSum+=ppsrc[max(0, min(cy-1, i))][x];

			for( y = 0; y < cy; y++ )
			{
				ppwork[y][x] = nCurrentSum/nMaskSize;
				nCurrentSum-=ppsrc[max(0 /*min(cy-1*/, y-nMaskMin/*)*/)][x];
				nCurrentSum+=ppsrc[/*max(0, */min(cy-1, y+nMaskMax)/*)*/][x];

				//ASSERT(nCurrentSum>=0);
			}
			Notify( x );
		}

//second step. average rows
		for( y = 0; y < cy; y++, pwork++ )
		{
			pwork = ppwork[y];
			nCurrentSum = 0;
			for( i = -nMaskMin; i< +nMaskMax; i++ )
				nCurrentSum+=pwork[max(0, min(cx-1, i))];

			for( pmask = ppmask[y], pnew = ppnew[y], psrc = ppsrc[y], x = 0; x < cx; x++, pnew++, pmask++, psrc++ )
			{
				if( *pmask )
				{
					nCurrentHist = nCurrentSum*nMaskSize;
					nRes = *psrc*nHistArea - nCurrentHist;
					if(nRes >= nThreshold||nRes <= -nThreshold)
						*pnew = max(0, min((color)-1, 
							((*psrc)*nHistArea + fStrength*nRes)/nHistArea) );
					else
						*pnew = *psrc;
				}

				nCurrentSum-=pwork[max(0, /*min(cx-1, */x-nMaskMin/*)*/)];
				nCurrentSum+=pwork[/*max(0, */min(cx-1, x+nMaskMax/*)*/)];
			}
			Notify( y+cx );
		}

		if( c != colors-1 )
			NextNotificationStage();
			
	}
	}
	catch( CException *pe )
	{
		pe->Delete();
		bRet = false;
	}

	delete ppsrc;
	delete ppnew;
	delete ppwork;
	delete ppmask;

	return bRet;
}

//////////////////////////////////////////////////////////////////////
//CActionFastAverage implementation
CActionFastAverage::CActionFastAverage()
{
}

CActionFastAverage::~CActionFastAverage()
{
}

bool CActionFastAverage::InvokeFilter()
{
	CTimeTest	timeTest( true, "CActionFastAverage" );
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 1 )
		return false;

	int	nMaskSizeY = GetArgumentInt( "MaskSizeY" );
	if(nMaskSizeY<=0) nMaskSizeY = nMaskSize;

	CImageWrp	image = GetDataArgument();
	CImageWrp	imageNew = GetDataResult();
	CImageWrp	imageWork;

	if( image.IsEmpty()) return false;

	int	cx = image.GetWidth();
	int	cy = image.GetHeight();
	int	colors = image.GetColorsCount();

	StartNotification( 2*cy, colors, 100 );

	//imageNew.CreateCompatibleImage( image );

	ICompatibleObjectPtr ptrCO( imageNew );
	ptrCO->CreateCompatibleObject( image, NULL, 0 );

	if( !imageWork.CreateNew( cx, cy, image.GetColorConvertor() ) )
		return false;

	int	x, y, c, i;

	color	*pwork;
	color	*pnew;
	byte	*pmask;

	color	**ppsrc = new color*[cy];
	color	**ppnew = new color*[cy];
	color	**ppwork = new color*[cy];
	byte	**ppmask = new byte*[cy];
//init masks
	for( y = 0; y < cy; y++ )
		ppmask[y] = image.GetRowMask( y );

	long	nCurrentSum;

	bool bRet=true;
	try{

	CImagePaneCache paneCache( imageNew );

	long *pnCurrentSum = new long[cx];
	for( c = 0; c <  colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;


		for( y = 0; y < cy; y++ )
		{
			ppsrc[y] = image.GetRow( y, c );
			ppnew[y] = imageNew.GetRow( y, c );
			ppwork[y] = imageWork.GetRow( y, c );
		}
//first step - average columns. Write the column sum to work image
		int	nMaskMin = nMaskSizeY/2;
		int	nMaskMax = nMaskSizeY-nMaskSizeY/2;

		for( x = 0; x < cx; x++ )
			pnCurrentSum[x] = 0;
		for( i = -nMaskMin; i < +nMaskMax; i++ )
			for( x = 0; x < cx; x++ )
				pnCurrentSum[x]+=ppsrc[max(0, min(cy-1, i))][x];
		for( y = 0; y < cy; y++ )
		{
			for( x = 0; x < cx; x++ )
			{
				ppwork[y][x] = pnCurrentSum[x]/nMaskSizeY;
				pnCurrentSum[x]-=ppsrc[max(0, y-nMaskMin)][x];
				pnCurrentSum[x]+=ppsrc[min(cy-1, y+nMaskMax)][x];
			}
			Notify( y );
		}

		nMaskMin = nMaskSize/2;
		nMaskMax = nMaskSize-nMaskSize/2;

//second step. average rows
		for( y = 0; y < cy; y++, pwork++ )
		{
			pwork = ppwork[y];
			nCurrentSum = 0;
			for( i = -nMaskMin; i< +nMaskMax; i++ )
				nCurrentSum+=pwork[max(0, min(cx-1, i))];

			for( pmask = ppmask[y], pnew = ppnew[y], x = 0; x < cx; x++, pnew++, pmask++ )
			{
				if( *pmask )
					*pnew = nCurrentSum/nMaskSize;	

				nCurrentSum-=pwork[max(0, x-nMaskMin)];
				nCurrentSum+=pwork[min(cx-1, x+nMaskMax)];
			}
			Notify( y+cy );
		}

		if( c != colors-1 )
			NextNotificationStage();
			
	}
	}
	catch( CException *pe )
	{
		pe->Delete();
		bRet = false;
	}

	delete ppsrc;
	delete ppnew;
	delete ppwork;
	delete ppmask;

	return bRet;
}

