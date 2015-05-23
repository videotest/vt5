#include "stdafx.h"
#include "morphology.h"
#include "resource.h"
#include "ImagePane.h"

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionBoundary, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionClose, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionOpen, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionDliate, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionErode, CCmdTargetEx);

// olecreate 

// {9F1EE000-059E-4287-8065-3BC7BED6B62A}
GUARD_IMPLEMENT_OLECREATE(CActionBoundary, "FiltersMain.Boundary",
0x9f1ee000, 0x59e, 0x4287, 0x80, 0x65, 0x3b, 0xc7, 0xbe, 0xd6, 0xb6, 0x2a);
// {195CE12F-112A-4fee-B82C-E21E7A6ADB2B}
GUARD_IMPLEMENT_OLECREATE(CActionClose, "FiltersMain.Close",
0x195ce12f, 0x112a, 0x4fee, 0xb8, 0x2c, 0xe2, 0x1e, 0x7a, 0x6a, 0xdb, 0x2b);
// {0442FD20-7D5D-457d-A552-2D9306B34BB8}
GUARD_IMPLEMENT_OLECREATE(CActionOpen, "FiltersMain.Open",
0x442fd20, 0x7d5d, 0x457d, 0xa5, 0x52, 0x2d, 0x93, 0x6, 0xb3, 0x4b, 0xb8);
// {FFAF4B20-3F13-4370-A995-4C93B8147979}
GUARD_IMPLEMENT_OLECREATE(CActionDliate, "FiltersMain.Dliate",
0xffaf4b20, 0x3f13, 0x4370, 0xa9, 0x95, 0x4c, 0x93, 0xb8, 0x14, 0x79, 0x79);
// {183C2178-C5D5-494f-AA3D-4633E4F44618}
GUARD_IMPLEMENT_OLECREATE(CActionErode, "FiltersMain.Erode",
0x183c2178, 0xc5d5, 0x494f, 0xaa, 0x3d, 0x46, 0x33, 0xe4, 0xf4, 0x46, 0x18);

// guidinfo 

// {89081419-296D-488b-A7D1-FE70128CF4F5}
static const GUID guidBoundary =
{ 0x89081419, 0x296d, 0x488b, { 0xa7, 0xd1, 0xfe, 0x70, 0x12, 0x8c, 0xf4, 0xf5 } };
// {2FC065DA-613F-45e3-B80B-F23736AA8572}
static const GUID guidClose =
{ 0x2fc065da, 0x613f, 0x45e3, { 0xb8, 0xb, 0xf2, 0x37, 0x36, 0xaa, 0x85, 0x72 } };
// {6B6DB382-EB8D-4b8e-BD73-93AA792C9D8E}
static const GUID guidOpen =
{ 0x6b6db382, 0xeb8d, 0x4b8e, { 0xbd, 0x73, 0x93, 0xaa, 0x79, 0x2c, 0x9d, 0x8e } };
// {A2577371-9324-4bf3-9C70-8CD21329A8FC}
static const GUID guidDliate =
{ 0xa2577371, 0x9324, 0x4bf3, { 0x9c, 0x70, 0x8c, 0xd2, 0x13, 0x29, 0xa8, 0xfc } };
// {AAB4AC1F-2809-4a87-B8BE-D321869BA25A}
static const GUID guidErode =
{ 0xaab4ac1f, 0x2809, 0x4a87, { 0xb8, 0xbe, 0xd3, 0x21, 0x86, 0x9b, 0xa2, 0x5a } };



//[ag]6. action info

ACTION_INFO_FULL(CActionBoundary, IDS_ACTION_BOUNDARY, IDS_MENU_FILTERS, IDS_TB_MAIN, guidBoundary);
ACTION_INFO_FULL(CActionClose, IDS_ACTION_CLOSE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidClose);
ACTION_INFO_FULL(CActionOpen, IDS_ACTION_OPEN, IDS_MENU_FILTERS, IDS_TB_MAIN, guidOpen);
ACTION_INFO_FULL(CActionDliate, IDS_ACTION_DILATE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidDliate);
ACTION_INFO_FULL(CActionErode, IDS_ACTION_ERODE, IDS_MENU_FILTERS, IDS_TB_MAIN, guidErode);

//[ag]7. targets

ACTION_TARGET(CActionBoundary, "anydoc");
ACTION_TARGET(CActionClose, "anydoc");
ACTION_TARGET(CActionOpen, "anydoc");
ACTION_TARGET(CActionDliate, "anydoc");
ACTION_TARGET(CActionErode, "anydoc");

//[ag]8. arguments

ACTION_ARG_LIST(CActionBoundary)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_INT(_T("Power"), 3 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Boundary") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionClose)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Close") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionOpen)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Open") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionDliate)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Dilate") )
END_ACTION_ARG_LIST();

ACTION_ARG_LIST(CActionErode)
	ARG_INT(_T("MaskSize"), 5 )
	ARG_IMAGE( _T("Source") )
	RES_IMAGE( _T("Erode") )
END_ACTION_ARG_LIST();

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionBoundary implementation
CActionBoundary::CActionBoundary()
{
}

CActionBoundary::~CActionBoundary()
{
}

bool CActionBoundary::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );
	int	nPower = GetArgumentInt( "Power" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter() )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;


	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = max( 0, min( colorMax, nPower*(CalcMax()-CalcMin()) ) );

				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}
		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionClose implementation
CActionClose::CActionClose()
{
}

CActionClose::~CActionClose()
{
}

bool CActionClose::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter( 2 ) )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;

	CImageWrp	imageWork, imageNew;
				 
	imageWork.CreateCompatibleImage( m_imageNew, false );
	//{
	//	ICompatibleObjectPtr ptrCO( imageWork );
	//	ptrCO->CreateCompatibleObject( m_imageNew, NULL, 0 );
	//
	//}
	

	CImagePaneCache paneCache( m_imageNew );


	imageNew = m_imageNew;
	m_imageNew = imageWork;

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = CalcMax();

				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}

	NextNotificationStage();

	m_image = imageWork;
	m_imageNew = imageNew;

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = CalcMin();

				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}
		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionOpen implementation
CActionOpen::CActionOpen()
{
}

CActionOpen::~CActionOpen()
{
}

bool CActionOpen::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter( 2 ) )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;

	CImageWrp	imageWork, imageNew;

	imageWork.CreateCompatibleImage( m_imageNew, false );
	//{
	//	ICompatibleObjectPtr ptrCO( imageWork );
	//	ptrCO->CreateCompatibleObject( m_imageNew, NULL, 0 );
	//
	//}

	CImagePaneCache paneCache( m_imageNew );

	

	imageNew = m_imageNew;
	m_imageNew = imageWork;

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = CalcMin();

				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}

	NextNotificationStage();

	m_image = imageWork;
	m_imageNew = imageNew;

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = CalcMax();

				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}
		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionDliate implementation
CActionDliate::CActionDliate()
{
}

CActionDliate::~CActionDliate()
{
}

bool CActionDliate::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter() )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;

	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = CalcMax();

				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}
		
	FinalizeImageFilter();
	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionErode implementation
CActionErode::CActionErode()
{
}

CActionErode::~CActionErode()
{
}

bool CActionErode::InvokeFilter()
{
	int	nMaskSize = GetArgumentInt( "MaskSize" );

	if( nMaskSize  < 2 )
		return false;

	if( !InitImageFilter() )return false;
	if( !InitHistMask( nMaskSize ) )return false;

	int	x, y, c;
	color	*pdest;
	byte	*pmask;

	CImagePaneCache paneCache( m_imageNew );

	for( c = 0; c <  m_colors; c++ )
	{
		if( !paneCache.IsPaneEnable( c ) ) 
			continue;

		InitColorCache( c );
		for( y = 0; y < m_cy; y++ )
		{
			pmask = m_image.GetRowMask( y );
			pdest = m_imageNew.GetRow( y, c );


			InitApprHistMask( y );

			for( x = 0; x < m_cx; x++, pmask++, pdest++ )
			{
				if( *pmask )
					*pdest = CalcMin();

				MoveApprHistMaskRight();
			}

			NotifyImage( y, c );
		}
	}
		
	FinalizeImageFilter();
	return true;
}


