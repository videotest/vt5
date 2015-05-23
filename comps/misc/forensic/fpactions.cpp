#include "stdafx.h"
#include "fpactions.h"
#include "resource.h"


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionThresholdTrace, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionEluminateBackgr, CCmdTargetEx);

// olecreate 

// {C52B96CB-8031-4ccf-8BFF-B5295A96D9C3}
GUARD_IMPLEMENT_OLECREATE(CActionThresholdTrace, "forensic.ThresholdTrace",
0xc52b96cb, 0x8031, 0x4ccf, 0x8b, 0xff, 0xb5, 0x29, 0x5a, 0x96, 0xd9, 0xc3);
// {DAC9BCF6-FF0B-4291-98BE-C4BAE0052FAF}
GUARD_IMPLEMENT_OLECREATE(CActionEluminateBackgr, "forensic.EluminateBackgr",
0xdac9bcf6, 0xff0b, 0x4291, 0x98, 0xbe, 0xc4, 0xba, 0xe0, 0x5, 0x2f, 0xaf);

//guidinfo 

// {2FEF0C09-FD71-4fb9-8A88-1C356AA4E2AF}
static const GUID guidThresholdTrace =
{ 0x2fef0c09, 0xfd71, 0x4fb9, { 0x8a, 0x88, 0x1c, 0x35, 0x6a, 0xa4, 0xe2, 0xaf } };
// {FF70865F-A044-450b-B69E-78223180F3CE}
static const GUID guidEluminateBackgr =
{ 0xff70865f, 0xa044, 0x450b, { 0xb6, 0x9e, 0x78, 0x22, 0x31, 0x80, 0xf3, 0xce } };




//[ag]6. action info

ACTION_INFO_FULL(CActionThresholdTrace, IDS_ACTION_THRESHOLD, -1, -1, guidThresholdTrace);
ACTION_INFO_FULL(CActionEluminateBackgr, IDS_ACTION_ELUMINATE, -1, -1, guidEluminateBackgr);

//[ag]7. targets

ACTION_TARGET(CActionThresholdTrace, "anydoc");
ACTION_TARGET(CActionEluminateBackgr, "anydoc");

//[ag]8. arguments

ACTION_ARG_LIST(CActionThresholdTrace)
	ARG_IMAGE( _T("Source") )
	ARG_OBJECT( _T("Objects") )
	RES_IMAGE( _T("Thresholded") )
END_ACTION_ARG_LIST();


ACTION_ARG_LIST(CActionEluminateBackgr)
	ARG_IMAGE( _T("Source") )
	ARG_OBJECT( _T("Objects") )
	RES_IMAGE( _T("Eluminated") )
END_ACTION_ARG_LIST();



//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionThresholdTrace implementation
CActionThresholdTrace::CActionThresholdTrace()
{
}

CActionThresholdTrace::~CActionThresholdTrace()
{
}

bool CActionThresholdTrace::InvokeFilter()
{
	CObjectListWrp	objects = GetDataArgument( "Objects" );
	CImageWrp		image = GetDataArgument( "Source" );
	CImageWrp		imageNew = GetDataResult();
	imageNew.CreateCompatibleImage( image );

	StartNotification( image.GetHeight() );

	int	colors = image.GetColorsCount();

	color	*pcolorMin = new color[colors];
	color	*pcolorMax = new color[colors];
	color	**pcolors = new color*[colors];
	color	**pcolorsNew = new color*[colors];

	bool	bFirstInit = true;


	POSITION	pos = objects.GetFirstObjectPosition();

	while( pos )
	{
		CObjectWrp	object( objects.GetNextObject( pos ), false );
		CImageWrp	imageO( object.GetImage(), false );


		int	cx = imageO.GetWidth(), cy = imageO.GetHeight();
		int	x, y, c;

		if( imageO.GetColorsCount() != colors )
			continue;

		for( c = 0; c < colors; c++ )
			for( y = 0; y < cy; y++ )
			{
				color	*p = imageO.GetRow( y, c );
				byte	*pmask = imageO.GetRowMask( y );
				for( x = 0; x < cx; x++, p++, pmask++  )
				{
					if( !*pmask )
						continue;

					if( bFirstInit )
					{
						pcolorMin[c] = *p;
						pcolorMax[c] = *p;
						bFirstInit = false;
					}
					else
					{
						pcolorMin[c] = min( *p, pcolorMin[c] );
						pcolorMax[c] = max( *p, pcolorMax[c] );
					}
				}
			}
	}

	int	cx = image.GetWidth(), cy = image.GetHeight();
	int	x, y, c;


	for( y = 0; y < cy; y++ )
	{
		for( c = 0; c < colors; c++ )
		{
			pcolors[c] = image.GetRow( y, c );
			pcolorsNew[c]= imageNew.GetRow( y, c );
		}

		byte	*pmask = image.GetRowMask( y );

		for( x = 0; x < cx; x++, pmask++  )
		{
			if( !*pmask )
				continue;

			bool	bInside = true;
			for( c = 0; c < colors; c++ )
			{
				bInside &= pcolorMin[c] <= *pcolors[c] && pcolorMax[c] >= *pcolors[c];
				pcolors[c]++;
			}

			for( c = 0; c < colors; c++ )
			{
				if( bInside )
					*pcolorsNew[c] = ByteAsColor( 255 );
				else
					*pcolorsNew[c] = ByteAsColor( 0 );
				pcolorsNew[c]++;
			}
		}
	}

	FinishNotification();

	return true;
}

bool CActionThresholdTrace::CanDeleteArgument( CFilterArgument *pa )
{
	return CheckInterface( pa->m_punkArg, IID_IImage );
}

//////////////////////////////////////////////////////////////////////
//CActionEluminateBackgr implementation
CActionEluminateBackgr::CActionEluminateBackgr()
{
}

CActionEluminateBackgr::~CActionEluminateBackgr()
{
}

bool CActionEluminateBackgr::InvokeFilter()
{
	CObjectListWrp	objects = GetDataArgument( "Objects" );
	CImageWrp		image = GetDataArgument( "Source" );
	CImageWrp		imageNew = GetDataResult();
	imageNew.CreateCompatibleImage( image );

	StartNotification( image.GetHeight() );

	int	colors = image.GetColorsCount();

	color	*pcolorMin = new color[colors];
	color	*pcolorMax = new color[colors];
	color	**pcolors = new color*[colors];
	color	**pcolorsNew = new color*[colors];

	bool	bFirstInit = true;


	POSITION	pos = objects.GetFirstObjectPosition();

	while( pos )
	{
		CObjectWrp	object( objects.GetNextObject( pos ), false );
		CImageWrp	imageO( object.GetImage(), false );


		int	cx = imageO.GetWidth(), cy = imageO.GetHeight();
		int	x, y, c;

		if( imageO.GetColorsCount() != colors )
			continue;

		for( c = 0; c < colors; c++ )
			for( y = 0; y < cy; y++ )
			{
				color	*p = imageO.GetRow( y, c );
				byte	*pmask = imageO.GetRowMask( y );
				for( x = 0; x < cx; x++, p++, pmask++  )
				{
					if( !*pmask )
						continue;

					if( bFirstInit )
					{
						pcolorMin[c] = *p;
						pcolorMax[c] = *p;
						bFirstInit = false;
					}
					else
					{
						pcolorMin[c] = min( *p, pcolorMin[c] );
						pcolorMax[c] = max( *p, pcolorMax[c] );
					}
				}
			}
	}

	int	cx = image.GetWidth(), cy = image.GetHeight();
	int	x, y, c;


	for( y = 0; y < cy; y++ )
	{
		for( c = 0; c < colors; c++ )
		{
			pcolors[c] = image.GetRow( y, c );
			pcolorsNew[c]= imageNew.GetRow( y, c );
		}

		byte	*pmask = image.GetRowMask( y );

		for( x = 0; x < cx; x++, pmask++  )
		{
			if( !*pmask )
				continue;

			bool	bInside = true;
			bool	bBelow = true;
			for( c = 0; c < colors; c++ )
			{
				bInside &= pcolorMin[c] <= *pcolors[c] && pcolorMax[c] >= *pcolors[c];
			}

			for( c = 0; c < colors; c++ )
			{
				if( bInside )
					*pcolorsNew[c] = *pcolors[c];
				else
					*pcolorsNew[c] = ByteAsColor( 255 );
				pcolors[c]++;
				pcolorsNew[c]++;
			}
		}
	}

	FinishNotification();

	return true;
}


