#include "stdafx.h"
#include "selectactions.h"
#include "resource.h"
#include "math.h"
#include "PropPage.h"
#include "alloc.h"
#include "units_const.h"


//common defines
const char szFixedSelectSizes[] = _T("SelectEditor");
const char szRectCX[] = _T("RectCX");
const char szRectCY[] = _T("RectCY");
const char szUseCalubration[] =  _T("UseCalibration");


const char szMagick[] = _T("ObjectEditor");
const char szMagickInt[] = _T("MagickInt");
const char szMagicFillHoles[] = _T("MagicFillHoles");
const char szDynamicBrightness[] = _T("DynamicBrightness");


bool GetActiveImage( IUnknown *punkC, IUnknown **ppunkI, IUnknown **ppunkS )
{
	(*ppunkI) = 0;
	(*ppunkS) = 0;

	IUnknown	*punkImage = 0;
	IUnknown	*punkOld = 0;

	if( CheckInterface( punkC, IID_IDocument ) )
		punkImage = GetActiveObjectFromDocument( punkC, szTypeImage );
	else
		punkImage = GetActiveObjectFromContext( punkC, szTypeImage );
	if( ::GetParentKey( punkImage ) != INVALID_KEY )
	{
		punkOld = punkImage;
		INamedDataObject2Ptr	ptrN( punkImage );
		ptrN->GetParent( &punkImage );
	}
	
	if( !punkImage )
	{
		if( punkOld )
			punkOld->Release();
		return false;
	}

	(*ppunkI) = punkImage;
	(*ppunkS) = punkOld;

	return true;
}


void SimplifyWalkInfo( CWalkFillInfo &wfi )
{
}


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionSelectAll, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMakeVirtual, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSelectSmooth, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSelectInvert, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSelectEllipse, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSelectRemove, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSelectAdd, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSelectArea, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionSelectRect, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionMagickStick, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFourierSelectCenter, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFourierSelectFree, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFourierSelectEllipse, CCmdTargetEx);
//	fixed selection
IMPLEMENT_DYNCREATE(CActionSelectFixedRect, CCmdTargetEx);


// olecreate 

// {D12F9D6E-1E79-47e0-8B16-BC0F0F555CC9}
GUARD_IMPLEMENT_OLECREATE(CActionSelectAll, "Selection.SelectAll",
0xd12f9d6e, 0x1e79, 0x47e0, 0x8b, 0x16, 0xbc, 0xf, 0xf, 0x55, 0x5c, 0xc9);
// {7C200DC1-03E7-47d8-A769-8813CBC79B85}
GUARD_IMPLEMENT_OLECREATE(CActionMakeVirtual, "Selection.MakeVirtual",
0x7c200dc1, 0x3e7, 0x47d8, 0xa7, 0x69, 0x88, 0x13, 0xcb, 0xc7, 0x9b, 0x85);
// {653E1407-853C-11d3-A679-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectSmooth, "selection.SelectSmooth",
0x653e1407, 0x853c, 0x11d3, 0xa6, 0x79, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {653E1403-853C-11d3-A679-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectInvert, "selection.SelectInvert",
0x653e1403, 0x853c, 0x11d3, 0xa6, 0x79, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {699ACD93-7642-11d3-A663-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectEllipse, "Selection.SelectEllipse",
0x699acd93, 0x7642, 0x11d3, 0xa6, 0x63, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {B4695567-7589-11d3-A661-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectRemove, "Selection.SelectRemove",
0xb4695567, 0x7589, 0x11d3, 0xa6, 0x61, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {B4695563-7589-11d3-A661-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectAdd, "Selection.SelectAdd",
0xb4695563, 0x7589, 0x11d3, 0xa6, 0x61, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {120CD473-725A-11d3-A65D-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectArea, "Selection.SelectArea",
0x120cd473, 0x725a, 0x11d3, 0xa6, 0x5d, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {03F678D3-53B4-11d3-A626-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectRect, "Selection.SelectRect",
0x3f678d3, 0x53b4, 0x11d3, 0xa6, 0x26, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);
// {299E9B2B-69F5-4875-A76C-89E38D5F2C70}
GUARD_IMPLEMENT_OLECREATE(CActionMagickStick, "Selection.MagickStick", 
0x299e9b2b, 0x69f5, 0x4875, 0xa7, 0x6c, 0x89, 0xe3, 0x8d, 0x5f, 0x2c, 0x70);
// {5FB8A6D6-A342-4116-BD3F-B2DE12D1015E}
GUARD_IMPLEMENT_OLECREATE(CActionFourierSelectCenter, "fft.FourierSelectCenter",
0x5fb8a6d6, 0xa342, 0x4116, 0xbd, 0x3f, 0xb2, 0xde, 0x12, 0xd1, 0x1, 0x5e);
// {5F71E858-2795-4b01-ABB6-BA8CFF13C579}
GUARD_IMPLEMENT_OLECREATE(CActionFourierSelectFree, "fft.FourierSelectFree",
0x5f71e858, 0x2795, 0x4b01, 0xab, 0xb6, 0xba, 0x8c, 0xff, 0x13, 0xc5, 0x79);
// {DA0E9380-4210-419b-814D-ACBB55B69674}
GUARD_IMPLEMENT_OLECREATE(CActionFourierSelectEllipse, "fft.FourierSelectEllipse",
0xda0e9380, 0x4210, 0x419b, 0x81, 0x4d, 0xac, 0xbb, 0x55, 0xb6, 0x96, 0x74);
//	fixed selection
// {CCCA3B3A-235E-41a5-9022-7D7DD14A45EE}
GUARD_IMPLEMENT_OLECREATE(CActionSelectFixedRect, "Selection.SelectFixedRect",
0xccca3b3a, 0x235e, 0x41a5, 0x90, 0x22, 0x7d, 0x7d, 0xd1, 0x4a, 0x45, 0xee);

// guidinfo 

// {D5105461-DE1A-429f-A039-970B2C7A80DE}
static const GUID guidSelectAll =
{ 0xd5105461, 0xde1a, 0x429f, { 0xa0, 0x39, 0x97, 0xb, 0x2c, 0x7a, 0x80, 0xde } };
// {81666765-8025-4ed6-9EB0-6FC50A8305B2}
static const GUID guidMakeVirtual =
{ 0x81666765, 0x8025, 0x4ed6, { 0x9e, 0xb0, 0x6f, 0xc5, 0xa, 0x83, 0x5, 0xb2 } };
// {653E1405-853C-11d3-A679-0090275995FE}
static const GUID guidSelectSmooth =
{ 0x653e1405, 0x853c, 0x11d3, { 0xa6, 0x79, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {653E1401-853C-11d3-A679-0090275995FE}
static const GUID guidSelectInvert =
{ 0x653e1401, 0x853c, 0x11d3, { 0xa6, 0x79, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {699ACD91-7642-11d3-A663-0090275995FE}
static const GUID guidSelectEllipse =
{ 0x699acd91, 0x7642, 0x11d3, { 0xa6, 0x63, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {B4695565-7589-11d3-A661-0090275995FE}
static const GUID guidSelectRemove =
{ 0xb4695565, 0x7589, 0x11d3, { 0xa6, 0x61, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {B4695561-7589-11d3-A661-0090275995FE}
static const GUID guidSelectAdd =
{ 0xb4695561, 0x7589, 0x11d3, { 0xa6, 0x61, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {120CD471-725A-11d3-A65D-0090275995FE}
static const GUID guidSelectArea =
{ 0x120cd471, 0x725a, 0x11d3, { 0xa6, 0x5d, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {03F678D1-53B4-11d3-A626-0090275995FE}
static const GUID guidSelectRect =
{ 0x3f678d1, 0x53b4, 0x11d3, { 0xa6, 0x26, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };
// {BC7446C1-EDFE-11d4-9D41-009027982EB4}
static const GUID guidMagickStick = 
{ 0xbc7446c1, 0xedfe, 0x11d4, { 0x9d, 0x41, 0x0, 0x90, 0x27, 0x98, 0x2e, 0xb4 } };
// {51E02B74-96D7-4586-9760-DFE702293AC6}
static const GUID guidFourierSelectCenter =
{ 0x51e02b74, 0x96d7, 0x4586, { 0x97, 0x60, 0xdf, 0xe7, 0x2, 0x29, 0x3a, 0xc6 } };
// {F22BFC1C-90D3-45b6-9AEA-FB697C3DD467}
static const GUID guidFourierSelectFree =
{ 0xf22bfc1c, 0x90d3, 0x45b6, { 0x9a, 0xea, 0xfb, 0x69, 0x7c, 0x3d, 0xd4, 0x67 } };
// {0FEA6A67-59B4-44c6-A1BC-574DA35AB8CA}
static const GUID guidFourierSelectEllipse =
{ 0xfea6a67, 0x59b4, 0x44c6, { 0xa1, 0xbc, 0x57, 0x4d, 0xa3, 0x5a, 0xb8, 0xca } };
//	fixed selection
// {42781A8E-7B8F-4f45-98D2-425B9952E69D}
static const GUID guidSelectFixedRect =
{ 0x42781a8e, 0x7b8f, 0x4f45, { 0x98, 0xd2, 0x42, 0x5b, 0x99, 0x52, 0xe6, 0x9d } };

//[ag]6. action info

ACTION_INFO_FULL(CActionSelectAll, ID_ACTION_SELECT_ALL, -1, -1, guidSelectAll);
ACTION_INFO_FULL(CActionMakeVirtual, ID_ACTION_MAKE_VIRTUAL, -1, -1, guidMakeVirtual);
ACTION_INFO_FULL(CActionSelectSmooth, ID_ACTION_SEL_SMOOTH, -1, -1, guidSelectSmooth);
ACTION_INFO_FULL(CActionSelectInvert, ID_ACTION_SEL_INVERT, -1, -1, guidSelectInvert);
ACTION_INFO_FULL(CActionSelectEllipse, ID_ACTION_SEL_ELLIPSE, -1, -1, guidSelectEllipse);
ACTION_INFO_FULL(CActionSelectRemove, ID_ACTION_SEL_REMOVE, -1, -1, guidSelectRemove);
ACTION_INFO_FULL(CActionSelectAdd, ID_ACTION_SEL_ADD, -1, -1, guidSelectAdd);
ACTION_INFO_FULL(CActionSelectArea, ID_ACTION_SEL_AREA, -1, -1, guidSelectArea);
ACTION_INFO_FULL(CActionSelectRect, ID_ACTION_SEL_RECT, -1, -1, guidSelectRect);
ACTION_INFO_FULL(CActionMagickStick, ID_ACTION_MAGICK_STICK, -1, -1, guidMagickStick);
ACTION_INFO_FULL(CActionFourierSelectCenter, IDS_ACTION_FFT_CENTER, -1, -1, guidFourierSelectCenter);
ACTION_INFO_FULL(CActionFourierSelectFree, IDS_ACTION_FFT_FREE, -1, -1, guidFourierSelectFree);
ACTION_INFO_FULL(CActionFourierSelectEllipse, IDS_ACTION_FFT_ELPS, -1, -1, guidFourierSelectEllipse);
//	fixed selection
ACTION_INFO_FULL(CActionSelectFixedRect, ID_ACTION_SEL_FIXED_RECT, -1, -1, guidSelectFixedRect);

//[ag]7. targets

ACTION_TARGET(CActionSelectAll, szTargetAnydoc);
ACTION_TARGET(CActionMakeVirtual, szTargetAnydoc);
ACTION_TARGET(CActionSelectSmooth, szTargetViewSite);
ACTION_TARGET(CActionSelectInvert, szTargetViewSite);
ACTION_TARGET(CActionSelectEllipse, szTargetViewSite);
ACTION_TARGET(CActionSelectRemove, szTargetViewSite);
ACTION_TARGET(CActionSelectAdd, szTargetViewSite);
ACTION_TARGET(CActionSelectArea, szTargetViewSite);
ACTION_TARGET(CActionSelectRect, szTargetViewSite);
ACTION_TARGET(CActionMagickStick, szTargetViewSite);
ACTION_TARGET(CActionFourierSelectCenter, szTargetViewSite);
ACTION_TARGET(CActionFourierSelectFree, szTargetViewSite);
ACTION_TARGET(CActionFourierSelectEllipse, szTargetViewSite);
//	fixed selection
ACTION_TARGET(CActionSelectFixedRect, szTargetViewSite);

//[ag]8. arguments

ACTION_ARG_LIST(CActionSelectRect)
	ARG_INT(_T("Left"), 0)
	ARG_INT(_T("Top"), 0)
	ARG_INT(_T("Right"), 0)
	ARG_INT(_T("Bottom"), 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSelectEllipse)
	ARG_INT(_T("X"), -1)
	ARG_INT(_T("Y"), -1)
	ARG_INT(_T("R"), -1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSelectRemove)
	ARG_INT(_T("Select"), -1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionSelectAdd)
	ARG_INT(_T("Select"), -1)
END_ACTION_ARG_LIST()

//	fixed selection
/*ACTION_ARG_LIST(CActionSelectFixedRect)
	ARG_INT(_T("Left"), 0)
	ARG_INT(_T("Top"), 0)
	ARG_INT(_T("Right"), 0)
	ARG_INT(_T("Bottom"), 0)
END_ACTION_ARG_LIST()*/
ACTION_ARG_LIST(CActionSelectFixedRect)
	ARG_DOUBLE(_T("X"), -1)
	ARG_DOUBLE(_T("Y"), -1)
	ARG_DOUBLE(_T("Width"), 0)
	ARG_DOUBLE(_T("Height"), 0)
	ARG_INT(_T("UseCalibration"), 0)
END_ACTION_ARG_LIST()

CActionSelectBase::FrameDefMode	
	CActionSelectBase::s_defMode = CActionSelectBase::fdmNew;

bool CActionSelectBase::s_bMagick   = false;
bool CActionSelectBase::s_bFreehand = false;


//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionSelectAll implementation
CActionSelectAll::CActionSelectAll()
{
}

CActionSelectAll::~CActionSelectAll()
{
}

bool CActionSelectAll::Invoke()
{
	//get the target image
	IUnknown	*punkSelection = 0;
	IUnknown	*punkImage = 0;
	if( !::GetActiveImage( m_punkTarget, &punkImage, &punkSelection ) )
		return false;

	
	CImageWrp	image( punkImage, false );
	CImageWrp	imageSel( punkSelection, false );

	if( !punkImage )
		return false;

	if( imageSel != 0 )
		m_changes.RemoveFromDocData( m_punkTarget, imageSel );

	CRect	rect = image.GetRect();
	CImageWrp	imageNew;
	imageNew.Attach( image.CreateVImage( rect ), false );
	imageNew.InitContours();

	m_changes.SetToDocData( m_punkTarget, imageNew );

	return true;
}
							 
bool CActionSelectAll::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionSelectAll::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionSelectAll::IsAvaible()
{
	IUnknownPtr punkSelection;
	IUnknownPtr punkImage;
	if( !::GetActiveImage( m_punkTarget, &punkImage, &punkSelection ) )
		return false;
	if (punkImage == 0)
		return false;
	return true;
}


//////////////////////////////////////////////////////////////////////
//CActionMakeVirtual implementation
CActionMakeVirtual::CActionMakeVirtual()
{
}

CActionMakeVirtual::~CActionMakeVirtual()
{
}

bool CActionMakeVirtual::Invoke()
{
	//get the target image
	IUnknown	*punkImage = GetActiveObjectFromDocument( m_punkTarget, szTypeImage );
	if( !punkImage )return false;

	CImageWrp	imageOld( punkImage );
	punkImage->Release();

	CRect	rect = imageOld.GetRect();
	IUnknown	*punkParent = 0;
	INamedDataObject2Ptr	ptrOld( imageOld );
	ptrOld->GetParent( &punkParent );
	if( punkParent == 0 )return false;
	CImageWrp	imageParent = punkParent;
	punkParent->Release();	

	CRect	_rect_parent = imageParent.GetRect();		

	if( rect.left < 0 )
		rect.left = 0;

	if( rect.top < 0 )
		rect.top = 0;

	if( rect.bottom > _rect_parent.bottom )
		rect.bottom = _rect_parent.bottom;

	if( rect.right > _rect_parent.right )
		rect.right = _rect_parent.right;

	CImageWrp	image;
	image.Attach( imageParent.CreateVImage( rect ), false );
	image.InitRowMasks();

	if( image.GetWidth() <= 0 || image.GetHeight() <= 0 )
		return false;

	for( int y = 0; y<rect.Height(); y++ )
	{
		byte* p1 = image.GetRowMask( y );
		byte* p2 = imageOld.GetRowMask( y );

		if( !p1 || !p2 )
		{
			ASSERT( false );
		}

		memcpy( image.GetRowMask( y ), imageOld.GetRowMask( y ), rect.Width() );
	}

	m_changes.RemoveFromDocData( m_punkTarget, ptrOld );

	image.InitContours();
	m_changes.SetToDocData( m_punkTarget, image );

	return true;
}

bool CActionMakeVirtual::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionMakeVirtual::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}


class CRectCtrl: public CDrawControllerBase
{
public:
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		sptrIScrollZoomSite	sptrS( punkVS );
		CSize	size;

		sptrS->GetClientSize( &size );

		CPoint	point1h( 0, point.y );
		CPoint	point2h( size.cx, point.y );
		CPoint	point1v( point.x, 0 );
		CPoint	point2v( point.x, size.cy );

		point1h = ::ConvertToWindow( punkVS, point1h );
		point2h = ::ConvertToWindow( punkVS, point2h );
		point1v = ::ConvertToWindow( punkVS, point1v );
		point2v = ::ConvertToWindow( punkVS, point2v );

		dc.MoveTo( point1h );
		dc.LineTo( point2h );
		dc.MoveTo( point1v );
		dc.LineTo( point2v );
		
	}
};

//////////////////////////////////////////////////////////////////////
//CActionSelectSmooth implementation
CActionSelectSmooth::CActionSelectSmooth()
{
}

CActionSelectSmooth::~CActionSelectSmooth()
{
}

bool CActionSelectSmooth::ExecuteSettings( CWnd *pwndParent )
{
	return true;
}

bool CActionSelectSmooth::Invoke()
{
	//get the target image

	IUnknown	*punkSelection = 0;
	IUnknown	*punkImage = 0;

	if( !::GetActiveImage( m_punkTarget, &punkImage, &punkSelection ) )
		return false;

	CImageWrp	image( punkImage, false );
	CImageWrp	imageSel( punkSelection, false );

	if( !punkImage || !punkSelection )
		return false;

	//get the target document
	IUnknown	*punkDoc = 0;
	sptrIView	sptrV( m_punkTarget );
	sptrV->GetDocument( &punkDoc );
	sptrIDocument	sptrD( punkDoc );
	if( !punkDoc )
		return false;
	punkDoc->Release();


	//create a walk info for image
	CWalkFillInfo	info( image.GetRect() );

	//map the selection info to the global info

	CDC	*pdc = info.GetDC();
	pdc->SetPolyFillMode( WINDING );

	int	nContour, nContoursCount = imageSel.GetContoursCount();
	for( nContour = 0; nContour < nContoursCount; nContour++ )
	{
		Contour	*pContour = imageSel.GetContour( nContour );

		if( !pContour || pContour->nContourSize == 0 )
			continue;

		__SmoothContour(pdc, pContour, 20);

	}

	//find the contours
	info.InitContours();

	//calc the new selection rectangle
	CRect	rect = info.CalcContoursRect();

	//create a new image and attach image masks
	IUnknown	*punkNewSelection = image.CreateVImage( rect );
	CImageWrp	imageNew = punkNewSelection;
	if( !punkNewSelection )
		return false;
	punkNewSelection->Release();

	//attach data to the new image
	info.AttachMasksToImage( imageNew );

	imageNew.InitContours();

	//store changes
	m_changes.SetToDocData( sptrD, imageNew );
	m_changes.RemoveFromDocData( sptrD, imageSel );

	//change the target - now it is document
	m_lTargetKey = ::GetObjectKey( sptrD );
	return true;
}

//undo/redo
bool CActionSelectSmooth::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionSelectSmooth::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionSelectSmooth::IsAvaible()
{
	return CheckInterface( m_punkTarget, IID_IImageView );
}

//////////////////////////////////////////////////////////////////////
//CActionSelectInvert implementation
CActionSelectInvert::CActionSelectInvert()
{
}

CActionSelectInvert::~CActionSelectInvert()
{
}

bool CActionSelectInvert::Invoke()
{
	//get the target image

	IUnknown	*punkSelection = 0;
	IUnknown	*punkImage = 0;
	if( !::GetActiveImage( m_punkTarget, &punkImage, &punkSelection ) )
		return false;

	CImageWrp	image( punkImage, false );
	CImageWrp	imageSel( punkSelection, false );

	if( !punkImage )
		return false;

	//get the target document
	IUnknown	*punkDoc = 0;
	sptrIView	sptrV( m_punkTarget );
	sptrV->GetDocument( &punkDoc );
	sptrIDocument	sptrD( punkDoc );
	if( !punkDoc )
		return false;
	punkDoc->Release();


	//create a walk info for image
	CWalkFillInfo	info( image.GetRect() );

	//map the selection info to the global info
	if( !imageSel.IsEmpty() ) 
		info.AddImageMasks( imageSel );

	//negative the info
	int	x, y;
	int	cx = image.GetWidth(), cy = image.GetHeight();
	byte	byteFill = info.GetRawBody();

	for( y = 0; y < cy; y++ )
		for( x = 0; x < cx; x++ )
			if( info.GetPixel( x, y ) == 0 )
				info.SetPixel( x, y, byteFill );
			else
				info.SetPixel( x, y, 0 );
		

	//find the contours
	info.InitContours();

	//calc the new selection rectangle
	CRect	rect = info.CalcContoursRect();

	//create a new image and attach image masks
	IUnknown	*punkNewSelection = image.CreateVImage( rect );
	CImageWrp	imageNew = punkNewSelection;
	if( !punkNewSelection )
		return false;
	punkNewSelection->Release();

	//attach data to the new image
	info.AttachMasksToImage( imageNew );

	imageNew.InitContours();

	//store changes
	if (!imageNew.IsEmpty())
	m_changes.SetToDocData( sptrD, imageNew );

	if( !imageSel.IsEmpty() )
		m_changes.RemoveFromDocData( sptrD, imageSel );

	//change the target - now it is document
	m_lTargetKey = ::GetObjectKey( sptrD );
	return true;
}

//undo/redo
bool CActionSelectInvert::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionSelectInvert::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

bool CActionSelectInvert::IsAvaible()
{
	bool bOK = CheckInterface( m_punkTarget, IID_IImageView );

	if( bOK )
	{
		IUnknown	*punkSelection = 0;
		IUnknown	*punkImage = 0;
		if( !::GetActiveImage( m_punkTarget, &punkImage, &punkSelection ) )
			return false;

		if( punkImage )
			punkImage->Release();

		if( punkSelection )
		{
			punkSelection->Release();
			return true;
		}
		return false;
	}
	return bOK;
}

//////////////////////////////////////////////////////////////////////
//CActionSelectRemove implementation
CActionSelectModeBase::CActionSelectModeBase()
{
	m_bSelect = false;
}

bool CActionSelectModeBase::ExecuteSettings( CWnd *pwndParent )
{
	int	nSelect = GetArgumentInt( _T("Select") );

	if( nSelect != -1 && nSelect != 0 && nSelect != 1 )
		return false;

	if( nSelect == -1 )
	{
		if( IsChecked() )
			nSelect = 0;
		else
			nSelect = 1;
	}

	m_bSelect = nSelect == 1;

	SetArgument( _T("Select"), _variant_t( (long)nSelect ) );

	return true;
}

bool CActionSelectModeBase::Invoke()
{
	if( m_bSelect )
		CActionSelectBase::s_defMode = GetActionMode();
	else
		CActionSelectBase::s_defMode = CActionSelectBase::fdmNew;
	SetValue( GetAppUnknown(), "\\Selection", "DefMode", (long)(CActionSelectBase::s_defMode) );

	return true;
}

//extended UI
bool CActionSelectModeBase::IsAvaible()
{
	return CheckInterface( m_punkTarget, IID_IImageView );
}

bool CActionSelectModeBase::IsChecked()
{
	return CActionSelectBase::s_defMode == GetActionMode();
}

//////////////////////////////////////////////////////////////////////
//CActionSelectArea implementation
CActionSelectArea::CActionSelectArea()
{
	m_pcontour = 0;
	m_bTracking = false;
}

CActionSelectArea::~CActionSelectArea()
{
	::ContourDelete( m_pcontour );
}

void CActionSelectArea::DoDraw( CDC &dc )
{
	::ContourDraw( dc, m_pcontour, m_punkTarget, 0 );
}

bool CActionSelectArea::DoLButtonDown( CPoint pt )
{
	_Draw();
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();

	return false;
}

bool CActionSelectArea::DoLButtonUp( CPoint pt )
{
	_Draw();
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();

	return true;
}

bool CActionSelectArea::DoStartTracking( CPoint pt )
{
	m_bTracking = true;
	return true;
}

bool CActionSelectArea::DoTrack( CPoint pt )
{
	ASSERT(m_pcontour);
	ASSERT(m_pcontour->nContourSize > 0);

	ContourPoint	cpt = m_pcontour->ppoints[m_pcontour->nContourSize-1];

	if( cpt.x  == pt.x && cpt.y == pt.y )
		return true;

	_Draw();
	::ContourAddPoint( m_pcontour, pt.x, pt.y );
	_Draw();

	return true;
}

bool CActionSelectArea::DoFinishTracking( CPoint pt )
{
	ASSERT(m_pcontour);
	ASSERT(m_pcontour->nContourSize > 0);

	ContourPoint	cpt = m_pcontour->ppoints[m_pcontour->nContourSize-1];

	if( cpt.x != pt.x && cpt.y != pt.y )
	{
		_Draw();
		::ContourAddPoint( m_pcontour, pt.x, pt.y );
		_Draw();
	}

	m_bTracking = false;


	if( GetValueInt( GetAppUnknown(), "\\Selection", "FinalizeByButtonUp", true ) )
	{
		Finalize();
	}

	return true;
}

bool CActionSelectArea::DoLButtonDblClick( CPoint pt )
{
	Finalize();
	return true;
}

bool CActionSelectArea::DoMouseMove( CPoint pt )
{
	if( m_pcontour->nContourSize > 0 && !m_bTracking )
	{
		_Draw();

		m_pcontour->ppoints[m_pcontour->nContourSize-1].x = pt.x;
		m_pcontour->ppoints[m_pcontour->nContourSize-1].y = pt.y;

		_Draw();
	}
	return true;
}

void CActionSelectArea::StoreCurrentExecuteParams()
{
}

IUnknown *CActionSelectArea::DoCreateSelectionImage( IUnknown *punkImage )
{
	//test contour
	/*{
		::ContourDelete( m_pcontour );
		m_pcontour = ::ContourCreate();
		::ContourAddPoint( m_pcontour, 100, 50 );
		::ContourAddPoint( m_pcontour, 101, 50 );
		::ContourAddPoint( m_pcontour, 102, 51 );
		::ContourAddPoint( m_pcontour, 101, 52 );
		::ContourAddPoint( m_pcontour, 100, 52 );
	}*/

	CRect rect = ::ContourCalcRect( m_pcontour );

	if( rect == NORECT )
		return 0;
	if( !m_pcontour )
		return 0;
	
	CImageWrp	image( punkImage );
	CImageWrp	imageNew( image.CreateVImage( rect ), false );
	
	rect = CRect( imageNew.GetOffset(), CSize( imageNew.GetWidth(), imageNew.GetHeight() ) );

	CWalkFillInfo	wfi( rect );

	CDC	*pDC = wfi.GetDC();
	::ContourDraw( *pDC, m_pcontour, 0, cdfClosed|cdfFill );

	wfi.AttachMasksToImage( imageNew );

	return imageNew.Detach();
}
	

bool CActionSelectArea::Initialize()
{
	if( !CActionSelectBase::Initialize() )return false;
	s_bFreehand = true;
	UpdatePropertyPages();

	m_pcontour = ::ContourCreate();

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_FREE );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_FREE_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_FREE_SUB );

	return true;
}


CRect CActionSelectArea::CalcBoundsRect()
{
	ASSERT(m_pcontour);

	CRect	rect = NORECT;

	bool	bFoundPoint = false;

	for( int nPoint = 0; nPoint < m_pcontour->nContourSize; nPoint++ )
	{
		if( !bFoundPoint )
		{
			rect.left = m_pcontour->ppoints[nPoint].x;
			rect.top = m_pcontour->ppoints[nPoint].y;
			rect.right = m_pcontour->ppoints[nPoint].x+1;
			rect.bottom = m_pcontour->ppoints[nPoint].y+1;

			bFoundPoint = true;
		}
		else
		{
			rect.left = min( rect.left, m_pcontour->ppoints[nPoint].x );
			rect.top = min( rect.top, m_pcontour->ppoints[nPoint].y );
			rect.right = max( rect.right, m_pcontour->ppoints[nPoint].x+1 );
			rect.bottom = max( rect.bottom, m_pcontour->ppoints[nPoint].y+1 );
		}
	}

	return rect;
}
//////////////////////////////////////////////////////////////////////
//CActionSelectRect implementation

CActionSelectRect::CActionSelectRect()
{
	m_rect = NORECT;
	m_pointStart = CPoint( -1, -1 );
	m_pctrl = 0;
}

CActionSelectRect::~CActionSelectRect()
{
	ASSERT(m_pctrl == 0 );
}

void CActionSelectRect::DoDraw( CDC &dc )
{
	CWnd	*pwnd = GetWindowFromUnknown(m_punkTarget);
	CRect	rectClient;
	

	CRect	rc = ::ConvertToWindow( m_punkTarget, m_rect );
	dc.Rectangle( rc );
}

bool CActionSelectRect::DoVirtKey( UINT nVirtKey )
{
	CheckController();
	return CActionSelectBase::DoVirtKey( nVirtKey );
}

bool CActionSelectRect::DoSetCursor( CPoint pt )
{
	///CheckController();
	return CActionSelectBase::DoSetCursor( pt );
}

BOOL CActionSelectRect::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_TIMER || message == WM_KEYDOWN )CheckController();
	return CActionSelectBase::OnWndMsg( message, wParam, lParam, pResult );
}

void CActionSelectRect::CheckController()
{
	if( !m_bInTrackMode )
	{
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
		{
			if( !m_pctrl )
			{
				m_pctrl = new CRectCtrl();
				m_pctrl->Install();
			}
		}
		else
		{
			if( m_pctrl )
			{
				m_pctrl->Uninstall();
				m_pctrl->GetControllingUnknown()->Release();
				m_pctrl = 0;
			}
		}
	}
}

bool CActionSelectRect::DoStartTracking( CPoint pt )
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}

	_Draw();

	m_rect.left = pt.x;
	m_rect.right = pt.x;
	m_rect.top = pt.y;
	m_rect.bottom = pt.y;
	m_pointStart = pt;

	_Draw();
	return true;
}

bool CActionSelectRect::DoTrack( CPoint pt )
{
	_Draw();
	m_rect.left = min( m_pointStart.x, pt.x );
	m_rect.right = max( m_pointStart.x, pt.x );
	m_rect.top = min( m_pointStart.y, pt.y );
	m_rect.bottom = max( m_pointStart.y, pt.y );
	_Draw();
	return true;
}

bool CActionSelectRect::DoFinishTracking( CPoint pt )
{
	Finalize();
	return true;
}

void CActionSelectRect::StoreCurrentExecuteParams()
{
	SetArgument( _T("Left"), _variant_t( m_rect.left ) );
	SetArgument( _T("Top"), _variant_t( m_rect.top ) );
	SetArgument( _T("Right"), _variant_t( m_rect.right ) );
	SetArgument( _T("Bottom"), _variant_t( m_rect.bottom ) );
}

IUnknown *CActionSelectRect::DoCreateSelectionImage( IUnknown *punkImage )
{
	CImageWrp	image( punkImage );

	m_rect.left = max( min( m_rect.left, image.GetWidth() ), 0 );
	m_rect.right = max( min( m_rect.right, image.GetWidth() ), 0 );
	m_rect.top = max( min( m_rect.top, image.GetHeight() ), 0 );
	m_rect.bottom = max( min( m_rect.bottom, image.GetHeight() ), 0 );

	IUnknown *punk = image.CreateVImage( m_rect );
	return punk;
}
	

bool CActionSelectRect::Initialize()
{
	//view manipulation
	m_rect.left = GetArgumentInt( _T("Left") );
	m_rect.top = GetArgumentInt( _T("Top") );
	m_rect.right = GetArgumentInt( _T("Right") );
	m_rect.bottom = GetArgumentInt( _T("Bottom") );

	if( m_rect != NORECT )
	{
		Finalize();
		return false;
	}

	CheckController();

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_SELECT_RECT );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_SELECT_RECT_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_SELECT_RECT_SUB );

	return CActionSelectBase::Initialize();
}

void CActionSelectRect::Finalize()
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}

	CActionSelectBase::Finalize();
}


//////////////////////////////////////////////////////////////////////
//CActionSelectEllipse implementation
CActionSelectEllipse::CActionSelectEllipse()
{
	m_point1 = CPoint( -1, -1 );
	m_point2 = CPoint( -1, -1 );

	m_pointCenter = CPoint( -1, -1 );
	m_nRadius = 0;

}

CActionSelectEllipse::~CActionSelectEllipse()
{
}

void CActionSelectEllipse::DoDraw( CDC &dc )
{
	CRect	rect;
	
	rect.left = int( m_pointCenter.x - m_nRadius+.5 );
	rect.right = int( m_pointCenter.x + m_nRadius+.5 );
	rect.top = int( m_pointCenter.y - m_nRadius+.5 );
	rect.bottom = int( m_pointCenter.y + m_nRadius+.5 );

	rect = ::ConvertToWindow( m_punkTarget, rect );
	dc.Ellipse( rect );

	DrawPoint( dc, m_point1 );
	DrawPoint( dc, m_point2 );
}

IUnknown *CActionSelectEllipse::DoCreateSelectionImage( IUnknown *punkImage )
{
	CImageWrp	image( punkImage );

	CRect	rect;
	
	rect.left = int( m_pointCenter.x - m_nRadius+.5 );
	rect.right = int( m_pointCenter.x + m_nRadius+.5 );
	rect.top = int( m_pointCenter.y - m_nRadius+.5 );
	rect.bottom = int( m_pointCenter.y + m_nRadius+.5 );

	CRect	rectInfo = rect;

	rectInfo.left = max( min( rectInfo.left-1, image.GetWidth() ), 0 );
	rectInfo.right = max( min( rectInfo.right, image.GetWidth() ), 0 );
	rectInfo.top = max( min( rectInfo.top-1, image.GetHeight() ), 0 );
	rectInfo.bottom = max( min( rectInfo.bottom, image.GetHeight() ), 0 );

	CWalkFillInfo	info( rectInfo );

	CDC	*pDC = info.GetDC();
	pDC->SelectStockObject( WHITE_BRUSH );
	pDC->SelectStockObject( WHITE_PEN );
	pDC->Ellipse( rect );
	//info.FinalizeDC();

	IUnknown *punk = image.CreateVImage( rectInfo );

	info.AttachMasksToImage( punk );
	
	return punk;
}

bool CActionSelectEllipse::DoStartTracking( CPoint pt )
{
	_Draw();
	m_point1 = pt;
	m_point2 = pt;
	m_pointCenter = pt;
	m_nRadius = 0;
	_Draw();
	return true;
}

bool CActionSelectEllipse::DoTrack( CPoint pt )
{
	_Draw();
	
	m_point2 = pt;

	m_pointCenter.x = ( m_point1.x + m_point2.x )>>1;
	m_pointCenter.y = ( m_point1.y + m_point2.y )>>1;
	m_nRadius = ::sqrt(double((m_point1.x - m_point2.x)*(m_point1.x - m_point2.x)+
						(m_point1.y - m_point2.y)*(m_point1.y - m_point2.y) ))/2;

	_Draw();
	return true;
}

bool CActionSelectEllipse::DoFinishTracking( CPoint pt )
{
	Finalize();
	return true;
}

void CActionSelectEllipse::StoreCurrentExecuteParams()
{
	SetArgument( _T("X"), _variant_t(m_pointCenter.x) );
	SetArgument( _T("Y"), _variant_t(m_pointCenter.y) );
	SetArgument( _T("R"), _variant_t(m_nRadius) );
}

bool CActionSelectEllipse::Initialize()
{
	if( !CActionSelectBase::Initialize() )return false;

	m_pointCenter.x = GetArgumentInt( _T("X") );
	m_pointCenter.y = GetArgumentInt( _T("Y") );
	m_nRadius = GetArgumentInt( _T("R") );

	if( m_pointCenter.x != -1 &&
		m_pointCenter.y != -1 &&
		m_nRadius != 0 )
	{
		Finalize();
		return false;
	}

	m_hcurActive = AfxGetApp()->LoadCursor( IDC_ELLIPSE );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_ELLIPSE_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_ELLIPSE_SUB );

	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////
//the base class implementation
//undo/redo

bool CActionSelectBase::DoSetCursor( CPoint point )
{
	int	mode = s_defMode;

	if( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 ) mode = fdmAdd;
	if( ::GetAsyncKeyState( VK_MENU ) & 0x8000 ) mode = fdmRemove;

	if(m_bUseAddCursor)
	{
		if( mode == fdmAdd && m_hcurAdd != 0 )
		{		::SetCursor( m_hcurAdd );		return true;	}

		if( mode == fdmRemove && m_hcurSub != 0 )
		{		::SetCursor( m_hcurSub );		return true;	}
	}

	return CInteractiveActionBase::DoSetCursor( point );
}


CActionSelectBase::CActionSelectBase()
{
	s_defMode = 
		(FrameDefMode)
		(GetValueInt( GetAppUnknown(), "\\Selection", "DefMode", s_defMode ));
	int n = GetValueInt( GetAppUnknown(), "\\Selection", "DefMode", s_defMode );
	m_nOldSelectionType = 0;
	m_bOldEnable = false;
	m_rectImage = NORECT;
	m_hcurAdd = 0;
	m_hcurSub = 0;
}

CActionSelectBase::~CActionSelectBase()
{
}

bool CActionSelectBase::IsCompatibleTarget()
{
	DWORD dwFlags = 0;
	IImageViewPtr sptrIV(m_punkTarget);
	if(sptrIV != 0)sptrIV->GetImageShowFlags(&dwFlags);
	if (::GetValueInt(::GetAppUnknown(), "Selection", "SelectionActionsWorkOnBinary", FALSE))
		return (dwFlags&(isfSourceImage|isfBinaryFore|isfBinaryBack|isfBinary))!=0;
	else
		return (dwFlags & isfSourceImage) == isfSourceImage;
}

bool CActionSelectBase::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionSelectBase::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}

//some global funcs
IUnknown *CreateSumMask( IUnknown *punk1, IUnknown *punk2 )
{
	CImageWrp	image1( punk1 );
	CImageWrp	image2( punk2 );

	ASSERT( image1!=0 );
	ASSERT( image2!=0 );

	CRect	rect1 = CRect( image1.GetOffset(), CSize( image1.GetWidth(), image1.GetHeight() ) );
	CRect	rect2 = CRect( image2.GetOffset(), CSize( image2.GetWidth(), image2.GetHeight() ) );

	CRect	rect;
	rect.left = min( rect1.left, rect2.left );
	rect.top = min( rect1.top, rect2.top );
	rect.right = max( rect1.right, rect2.right );
	rect.bottom = max( rect1.bottom, rect2.bottom );

	//
	CImageWrp	image;
	image.CreateCompatibleImage( image1, false, rect );
	image.InitRowMasks();
	image.ResetMasks();

	bool	bCopyData = !image.IsVirtual();

	int	x, y, c;

	for( y = 0; y < rect2.Height(); y++ )
	{
		byte	*psource = image2.GetRowMask( y );
		byte	*pdest = image.GetRowMask( y-rect.top+rect2.top )-rect.left+rect2.left;

		for( x = 0; x < rect2.Width(); x++ )
			if( psource[x]==0xFF )
				pdest[x] = psource[x];

		if( bCopyData )
		{
			for( c = 0; c < image.GetColorsCount(); c++ )
			{
				color	*pcolorSource = image2.GetRow( y, c );
				color	*pcolorDest = image.GetRow( y-rect.top+rect2.top, c )-rect.left+rect2.left;

				for( x = 0; x < rect2.Width(); x++ )
					if( psource[x]==0xFF )
						pcolorDest[x] = pcolorSource[x];
			}
		}
	}


	for( y = 0; y < rect1.Height(); y++ )
	{
		byte	*psource = image1.GetRowMask( y );
		byte	*pdest = image.GetRowMask( y-rect.top+rect1.top )-rect.left+rect1.left;

		for( x = 0; x < rect1.Width(); x++ )
			if( psource[x]==0xFF )
				pdest[x] = psource[x];

		if( bCopyData )
		{
			for( c = 0; c < image.GetColorsCount(); c++ )
			{
				color	*pcolorSource = image1.GetRow( y, c );
				color	*pcolorDest = image.GetRow( y-rect.top+rect1.top, c )-rect.left+rect1.left;

				for( x = 0; x < rect1.Width(); x++ )
					if( psource[x]==0xFF )
						pcolorDest[x] = pcolorSource[x];
			}
		}
	}

	return image.Detach();
}

//some global funcs
IUnknown *CreateDiffMask( IUnknown *punk1, IUnknown *punk2 )
{
	CImageWrp	image1( punk1 );
	CImageWrp	image2( punk2 );

	ASSERT( image1!=0 );
	ASSERT( image2!=0 );

	CRect	rect1 = CRect( image1.GetOffset(), CSize( image1.GetWidth(), image1.GetHeight() ) );
	CRect	rect2 = CRect( image2.GetOffset(), CSize( image2.GetWidth(), image2.GetHeight() ) );

	CImageWrp	image;
	image.CreateCompatibleImage( image1, true );
	image.InitRowMasks();
	image.ResetContours();

	bool	bCopyData = !image.IsVirtual();

	int	x, y, c;

	if( bCopyData )
	{
		for( y = 0; y < rect1.Height(); y++ )
		{
			byte	*psource = image1.GetRowMask( y );
			for( c = 0; c < image.GetColorsCount(); c++ )
			{
				color	*pcolorSource = image1.GetRow( y, c );
				color	*pcolorDest = image.GetRow( y, c );

				for( x = 0; x < rect1.Width(); x++ )
					if( psource[x]==0xFF )
						pcolorDest[x] = pcolorSource[x];
			}
		}
	}

	CRect	rectIntersect;

	rectIntersect.left = max( rect1.left, rect2.left );
	rectIntersect.right = min( rect1.right, rect2.right );
	rectIntersect.top = max( rect1.top, rect2.top );
	rectIntersect.bottom = min( rect1.bottom, rect2.bottom );

	if( rectIntersect.left >= rectIntersect.right ||
		rectIntersect.top >= rectIntersect.bottom )
		return image.Detach();

	for( y = 0; y < rectIntersect.Height(); y++ )
	{
		byte	*psource = image2.GetRowMask( y-rect2.top+rectIntersect.top )-rect2.left+rectIntersect.left;
		byte	*pdest = image.GetRowMask( y-rect1.top+rectIntersect.top )-rect1.left+rectIntersect.left;

		for( x = 0; x < rectIntersect.Width(); x++ )
			if( psource[x] == 0xFF )
				pdest[x] = 0;
	}

	return image.Detach();
}


bool CActionSelectBase::Invoke()
{
	//get NameData
	IUnknownPtr	sptrD;

	{
		sptrIView	sptrV( m_punkTarget );
		sptrV->GetDocument( &sptrD );
	}

	bool	bForceAdd = ( ::GetAsyncKeyState( VK_SHIFT ) & 0x8000 )!=0;
	bool	bForceSub = ( ::GetAsyncKeyState( VK_MENU ) & 0x8000 )!=0;
	IUnknown	*punkNew = 0;

	int	mode = s_defMode;

	if( bForceAdd )mode = fdmAdd;
	if( bForceSub )mode = fdmRemove;
	if( m_imageOld.IsEmpty() )mode = fdmNew;

	if( mode == fdmNew )
	{
		punkNew = DoCreateSelectionImage( m_image );
	}
	else
	{
		IUnknown *punk = DoCreateSelectionImage( m_image );

		if( !punk )
			return false;

		if( mode == fdmAdd )
			punkNew = ::CreateSumMask( m_imageOld, punk );
		else
		if( mode == fdmRemove )
			punkNew = ::CreateDiffMask( m_imageOld, punk );

		punk->Release();
	}

	CImageWrp	image(  punkNew );
	image.InitContours();


	if( image.GetContoursCount() == 0 )
		if( punkNew )	
		{
			punkNew->Release();
			punkNew = 0;
		}


	//smooth
	long nSmoothStrength = max(0, GetValueInt(GetAppUnknown(), szMagick, "SmoothStrength", 30));

	if( punkNew )
	if((s_bFreehand || s_bMagick) && nSmoothStrength > 1)
	{
		

		//create a walk info for image
		CWalkFillInfo	info( m_image.GetRect() );
		//map the selection info to the global info
		CDC	*pdc = info.GetDC();
		pdc->SetPolyFillMode( WINDING );

		int	nContour, nContoursCount = image.GetContoursCount();
		for( nContour = 0; nContour < nContoursCount; nContour++ )
		{
			Contour	*pContour = image.GetContour( nContour );

			if( !pContour || pContour->nContourSize == 0 )
				continue;

			__SmoothContour(pdc, pContour, nSmoothStrength);

		}

		//find the contours
		info.InitContours();
		//calc the new selection rectangle
		CRect	rect = info.CalcContoursRect();

		//create a new image and attach image masks
		punkNew->Release();
		punkNew = m_image.CreateVImage( rect );
		image = punkNew;
		//attach data to the new image
		info.AttachMasksToImage(image);
		image.InitContours();
	}

	//simplification
	if( punkNew )
	if(s_bMagick)
	{
		//create a walk info for image
		CWalkFillInfo	info( m_image.GetRect() );
		//map the selection info to the global info
		CDC	*pdc = info.GetDC();
		pdc->SetPolyFillMode( WINDING );


		CWalkFillInfo	infoArea(m_image.GetRect());
		CDC	*pdcArea = infoArea.GetDC();
		pdcArea->SetPolyFillMode( WINDING );
		long nL = infoArea.GetRect().left;
		long nR = infoArea.GetRect().right;
		long nT = infoArea.GetRect().top;
		long nB = infoArea.GetRect().bottom;

		double fSimplificationCoeff = GetValueDouble(GetAppUnknown(), szMagick, "SimplificationCoeff", 0.1);
		long nMaxContour = GetValueInt(GetAppUnknown(), szMagick, "MaxContours", 30);
		long nSimplificationStrength = GetValueInt(GetAppUnknown(), szMagick, "SimplificationStrength", 30);

		int	nContour, nContoursCount = image.GetContoursCount();

		HWND hwnd = 0;
		{
			IApplicationPtr ptr_app( GetAppUnknown() );
			if( ptr_app )
				ptr_app->GetMainWindowHandle( &hwnd );
		}

		bool bNeedToSimplify = false;
		bool bSkipSimplify = false;
		if(nContoursCount > nMaxContour)
		{
			CString strText, strCaption;
			strText.LoadString(IDS_PROMT_SIMPLIFY);
			strCaption.LoadString(IDS_PROMT_SIMPLIFY_CAPTION);
			if(MessageBox( hwnd, strText, strCaption, MB_YESNO ) == IDYES)
				bNeedToSimplify = true;
			else
				bSkipSimplify = true;
		}

		if(!bSkipSimplify)
		{
			for(nContour = 0; nContour < nContoursCount; nContour++)
			{
				Contour	*pContour = image.GetContour(nContour);

				if(!pContour || pContour->nContourSize == 0)
					continue;

				if(pContour->lFlags == cfExternal && !bNeedToSimplify)
				{
					double fPerimeter = ContourMeasurePerimeter(pContour);

					__int64 nArea = 0;
					::ContourDraw(*pdcArea, pContour, 0, cdfFill|cdfClosed);
					for(long y = nT; y < nB; y++)
					{
						for(long x = nL; x < nR; x++)
						{
							if(infoArea.GetPixel(x,y) != 0)
								nArea++;
						}
					}

					double ffCircle = 4.0*PI*nArea/(fPerimeter*fPerimeter);
					if(ffCircle < fSimplificationCoeff)
					{

						HWND hwnd = 0;
						{
							IApplicationPtr ptr_app( GetAppUnknown() );
							if( ptr_app )
								ptr_app->GetMainWindowHandle( &hwnd );
						}
						CString strText, strCaption;
						strText.LoadString(IDS_PROMT_SIMPLIFY);
						strCaption.LoadString(IDS_PROMT_SIMPLIFY_CAPTION);
						if(MessageBox( hwnd, strText, strCaption, MB_YESNO ) == IDYES)
							bNeedToSimplify = true;
						else
							break;
					}
					//pdcArea->FillRect(infoArea.GetRect(), (CBrush*)GetStockObject(WHITE_BRUSH));
				}

				__SmoothContour(pdc, pContour, nSimplificationStrength);
			}

			if(bNeedToSimplify)
			{
				//find the contours
				info.InitContours();
				//calc the new selection rectangle
				CRect	rect = info.CalcContoursRect();
				//create a new image and attach image masks
				punkNew->Release();
				punkNew = m_image.CreateVImage( rect );
				image = punkNew;
				//attach data to the new image
				info.AttachMasksToImage(image);
				image.InitContours();
			}
		}
	}

	m_lTargetKey = ::GetObjectKey( sptrD );

	if( image.GetContoursCount() == 0 )
		if( punkNew )	
		{
			punkNew->Release();
			punkNew = 0;
		}


	if( m_imageOld )m_changes.RemoveFromDocData( sptrD, m_imageOld, true );
	if( punkNew )m_changes.SetToDocData( sptrD, punkNew );
	if( punkNew )	punkNew->Release();


  	return punkNew != 0 || m_imageOld != 0;
}


//extended UI
bool CActionSelectBase::IsAvaible()
{
	IUnknown *punk = GetTargetWindow();

	if (CheckInterface( m_punkTarget, IID_IImageView ))
	{
		IUnknown* punk_img = ::GetActiveObjectFromContext( punk, szTypeImage );
		if( punk_img )
		{
			punk_img->Release();
			return true;
		}
	}
	return false;
}

bool CActionSelectBase::IsChecked()
{
	return false;
}


void CActionSelectBase::DrawPoint( CDC &dc, CPoint point )
{
	if( point.x == -1 && point.y == -1 )
		return;
	CPoint	pointDraw = ::ConvertToWindow( m_punkTarget, point );

	int	dx = 10;
	int	dy = 10;

	dc.MoveTo( pointDraw.x - dx, pointDraw.y - dy );
	dc.LineTo( pointDraw.x + dx, pointDraw.y + dy );
	dc.MoveTo( pointDraw.x + dx, pointDraw.y - dy );
	dc.LineTo( pointDraw.x - dx, pointDraw.y + dy );

}

bool CActionSelectBase::Initialize()
{
	AddPropertyPageDescription(c_szCSelPropPage);


	if( !IsCompatibleTarget())
		if(/*!SetAppropriateView(m_punkTarget, avtSource) ||*/ m_state == asTerminate )
			return false;

	s_bMagick = false;
	s_bFreehand = false;


	sptrIImageView	sptrIV( m_punkTarget );
	sptrIV->EnableControlFrame( fcfNone );

	IUnknown	*punkImage = 0;
	IUnknown	*punkOld = 0;

	//sptrIImageView sptrIV( m_punkTarget );
	//get the image
	if( !::GetActiveImage( m_punkTarget, &punkImage, &punkOld ) )
		return false;

	m_image = punkImage;
	m_imageOld = punkOld;

	if( punkImage )punkImage->Release();
	if( punkOld )punkOld->Release();
	m_rectImage = m_image.GetRect();

	m_bUseAddCursor = ::GetValueInt(::GetAppUnknown(), "Selection", "UseAddCursor", 1) != 0;
	

	if( !CInteractiveActionBase::Initialize() )return false;

	LockMovement( true, m_rectImage );
	UpdatePropertyPages();

	return true;
}

void CActionSelectBase::Finalize()
{
	_Draw();
	sptrIImageView	sptrIV( m_punkTarget );
	sptrIV->EnableControlFrame( fcfRotate|fcfResize );

	SetValue( GetAppUnknown(), "\\Selection", "DefMode", (long)(CActionSelectBase::s_defMode) );

	CInteractiveActionBase::Finalize();
}



//////////////////////////////////////////////////////////////////////
//CActionMagickStick implementation

CActionMagickStick::CActionMagickStick()
{
	m_colorDelta = ::ByteAsColor( 5 );
	m_point = CPoint( -1, -1 );

	m_cx = 0;
	m_cy = 0;
	m_colors = 0;
	m_ppoints = 0;
	m_pbyteMark = 0;
	m_prows = 0;
	m_nPointCount = 0;
	m_rect = NORECT;

	m_bDynamicBrightness	= false;	
}

CActionMagickStick::~CActionMagickStick()
{

}


void CActionMagickStick::DoDraw( CDC &dc )
{

}

bool CActionMagickStick::DoLButtonDown( CPoint pt )
{
	m_point = pt;
	DoUpdateSettings();
	Finalize();
	return true;
}


IUnknown* CActionMagickStick::DoCreateSelectionImage( IUnknown *punkImage )
{	
	return GetResult( punkImage );
}

static void FillHoles(byte *mask, int cx, int cy, CRect r)
{ //заполнить дыры в изображении, работаем только в пределах rect
	if(r.Width()<3) return;
	if(r.Height()<3) return;

	int n;
	smart_alloc(pts, int, cx*cy);
	int pos0=0, pos1=0;

	int x,y;

	for (y=r.top+1;y<r.bottom-1;y++)
	{
		x=r.left+1; n=y*cx+x;
		if(mask[n-1]==0)
		{
			mask[n-1]=1;
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
		x=r.right-2; n=y*cx+x;
		if(mask[n+1]==0)
		{
			mask[n+1]=1;
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
	}
	for (x=r.left+1;x<r.right-1;x++)
	{
		y=r.top+1; n=y*cx+x;
		if(mask[n-cx]==0)
		{
			mask[n-cx]=1;
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
		y=r.bottom-2; n=y*cx+x;
		if(mask[n+cx]==0)
		{
			mask[n+cx]=1; 
			if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		}
	}
	// края изображения заполнили 1, точки на расстоянии шага от края внесли в список
	
	y=r.top; x=r.left; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	y=r.top; x=r.right-1; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	y=r.bottom-1; x=r.left; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	y=r.bottom-1; x=r.right-1; n=y*cx+x; if(mask[n]==0) mask[n]=1;
	//заткнули уголки

	/*
	// debug - output mask to file
	FILE *f=fopen("e:\\mylog.log","a");
	fputs("\n\r",f);
	for (y=0;y<cy;y++)
	{
		byte *imgy=mask+y*cx;
		for (x=0;x<cx;x++)
		{
			char c='.';
			if(*imgy==1) c='*';
			if(*imgy==255) c='#';
			fputc(c,f);
			imgy++;
		}
		fputs("\n\r",f);
	}
	fputs("\n\r",f);
	fclose(f);
	*/

	while(pos0<pos1)
	{
		n=pts[pos0]+1; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		n=pts[pos0]-1; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		n=pts[pos0]+cx; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		n=pts[pos0]-cx; if(mask[n]==0) {mask[n]=1; pts[pos1++]=n;}
		pos0++;
	}

	for (y=r.top;y<r.bottom;y++)
	{
		for (x=r.left;x<r.right;x++)
		{
			n=y*cx+x;
			switch(mask[n])
			{
			case 0: mask[n]=255; break;
			case 1: mask[n]=0; break;
			}
			
		}
	}
}

IUnknown* CActionMagickStick::GetResult( IUnknown* punkImage )
{
	
	CImageWrp image = punkImage;
	if( image.IsEmpty() )
		return NULL;	

	GetSettings();

	CWaitCursor	wait;
	
	m_cx = image.GetWidth();
	m_cy = image.GetHeight();
	m_colors = image.GetColorsCount();


	if( !image.GetRect().PtInRect( m_point ) )
	{
		//SetError( IDS_ERR_NOTINIMAGE );
		return NULL;
	}
	
	m_ppoints = new CPoint[m_cx*m_cy];
	m_pbyteMark = new byte[m_cx*m_cy];
	m_prows = new color*[m_cy*m_colors];

	for( int n = 0; n < m_cy; n++ )
		for( int c = 0; c < m_colors; c++ )
			m_prows[n*m_colors+c] = image.GetRow( n, c );

	ZeroMemory( m_pbyteMark, m_cx*m_cy );

	m_nPointCount = 0;
	int	nPointPos = 0;

	m_ppoints[m_nPointCount].x = m_point.x;
	m_ppoints[m_nPointCount].y = m_point.y;
	m_nPointCount++;

	m_pbyteMark[m_point.y*m_cx+m_point.x] = 0xFF;

	m_rect.left = m_point.x;
	m_rect.top = m_point.y;
	m_rect.right = m_point.x+1;
	m_rect.bottom = m_point.y+1;
	
	//TestPoint( m_ppoints[0].x, m_ppoints[0].y );

	
	while( nPointPos < m_nPointCount )
	{
		int	x = m_ppoints[nPointPos].x;
		int	y = m_ppoints[nPointPos].y;

		CheckAddArea( m_point.x, m_point.y, x-1, y, x, y );
		CheckAddArea( m_point.x, m_point.y, x+1, y, x, y );
		CheckAddArea( m_point.x, m_point.y, x, y-1, x, y );
		CheckAddArea( m_point.x, m_point.y, x, y+1, x, y );

		CheckAddArea( m_point.x, m_point.y, x-1, y-1, x, y );
		CheckAddArea( m_point.x, m_point.y, x+1, y+1, x, y );
		CheckAddArea( m_point.x, m_point.y, x+1, y-1, x, y );
		CheckAddArea( m_point.x, m_point.y, x-1, y+1, x, y );

		nPointPos++;
	}
	
	//удаление дырок в m_pbyteMark - в пределах m_rect
	if(m_bFillHoles) FillHoles(m_pbyteMark, m_cx, m_cy, m_rect);	
	
	CImageWrp	imageNew( image.CreateVImage( m_rect ), false );

	imageNew.InitRowMasks();
	for( n = 0; n < m_rect.Height(); n++ )
		memcpy( imageNew.GetRowMask( n ), m_pbyteMark+(n+m_rect.top)*m_cx+m_rect.left, m_rect.Width() );


	delete m_ppoints;
	delete m_pbyteMark;
	delete m_prows;

	m_cx = 0;
	m_cy = 0;
	m_colors = 0;
	m_ppoints = 0;
	m_pbyteMark = 0;
	m_prows = 0;

	return imageNew.Detach();
	
}


void CActionMagickStick::CheckAddArea( int x, int y, int x1, int y1, int nNearX , int nNearY )
{

	if( x1 < 0 || y1 < 0 || x1 >= m_cx || y1 >= m_cy )
		return;

	if( m_pbyteMark[y1*m_cx+x1] )
		return;

	for( int c = 0; c < m_colors; c++ )
	{
		color	color1;

		if( m_bDynamicBrightness )
			color1 = m_prows[nNearY*m_colors+c][nNearX];
		else
			color1 = m_prows[y*m_colors+c][x];

		
		color	color2 = m_prows[y1*m_colors+c][x1];

		if( ::abs( color2-color1 ) > m_colorDelta )
			return;
	}

	ASSERT( m_nPointCount < m_cx*m_cy  );
	
	m_ppoints[m_nPointCount].x = x1;
	m_ppoints[m_nPointCount].y = y1;
	m_nPointCount++;

	m_rect.left = min( x1, m_rect.left );
	m_rect.top = min( y1, m_rect.top );
	m_rect.right = max( x1+1, m_rect.right );
	m_rect.bottom = max( y1+1, m_rect.bottom );

	m_pbyteMark[y1*m_cx+x1] = 0xFF;
}


bool CActionMagickStick::Initialize()
{
	if( !CActionSelectBase::Initialize() )return false;
	s_bMagick = true;
	UpdatePropertyPages();
	m_hcurActive = AfxGetApp()->LoadCursor( IDC_MAGICK_OBJECT );
	m_hcurAdd = AfxGetApp()->LoadCursor( IDC_MAGICK_ADD );
	m_hcurSub = AfxGetApp()->LoadCursor( IDC_MAGICK_SUB );

	DoUpdateSettings();
	return true;
}

bool CActionMagickStick::DoUpdateSettings()
{
	GetSettings();
	return CActionSelectBase::DoUpdateSettings();
}

void CActionMagickStick::GetSettings()
{
	m_colorDelta = ByteAsColor( GetValueInt( GetAppUnknown(), szMagick, szMagickInt, 5 ) );

	m_bFillHoles = ::GetValueInt( ::GetAppUnknown(), szMagick, szMagicFillHoles, 0 ) != 0;

	m_bDynamicBrightness = 
		( ::GetValueInt( ::GetAppUnknown(), szMagick, szDynamicBrightness, 0 ) == 1 ? true : false );

}


//////////////////////////////////////////////////////////////////////
//CActionFourierSelectEllipse implementation
CActionFourierSelectEllipse::CActionFourierSelectEllipse()
{
	m_pointCenter = CPoint( 0, 0 );
}

CActionFourierSelectEllipse::~CActionFourierSelectEllipse()
{
}


IUnknown *CActionFourierSelectEllipse::DoCreateSelectionImage( IUnknown *punkImage )
{
	CImageWrp	imageBig = punkImage;
	CRect	rectImage = 	imageBig.GetRect();

	CPoint	pt1 = m_pointCenter;
	CPoint	pt2 = CPoint( rectImage.left+rectImage.Width()-m_pointCenter.x,
						rectImage.top+rectImage.Height()-m_pointCenter.y );

	int	cx = abs( pt1.x - m_point.x );
	int	cy = abs( pt1.y - m_point.y );

	CRect	rect1( pt1.x-cx, pt1.y - cy, pt1.x+cx, pt1.y+cy );
	CRect	rect2( pt2.x-cx, pt2.y - cy, pt2.x+cx, pt2.y+cy );


	CRect	rect( min( rect1.left, rect2.left ), min( rect1.top, rect2.top ),
					max( rect1.right, rect2.right ), max( rect1.bottom, rect2.bottom ) );
				

	CWalkFillInfo	info( rect );

	{
		CDC *pdc = info.GetDC();
		pdc->SelectStockObject( WHITE_BRUSH );
		pdc->SelectStockObject( WHITE_PEN );
		pdc->Ellipse( rect1 );
		pdc->Ellipse( rect2 );
	}


	CImageWrp	image( imageBig.CreateVImage( rect ), false );
	image.InitRowMasks();
	info.AttachMasksToImage( image );
	image.InitContours();

	return image.Detach();
}

void CActionFourierSelectEllipse::DoDraw( CDC &dc )
{
	dc.SelectStockObject( NULL_BRUSH );


	CPoint	pt1 = m_pointCenter;
	CPoint	pt2 = CPoint( m_rectImage.left+m_rectImage.Width()-m_pointCenter.x,
						m_rectImage.top+m_rectImage.Height()-m_pointCenter.y );

	int	cx = abs( pt1.x - m_point.x );
	int	cy = abs( pt1.y - m_point.y );

	CRect	rect1( pt1.x-cx, pt1.y - cy, pt1.x+cx, pt1.y+cy );
	CRect	rect2( pt2.x-cx, pt2.y - cy, pt2.x+cx, pt2.y+cy );

	rect1 = ConvertToWindow( m_punkTarget, rect1 );
	rect2 = ConvertToWindow( m_punkTarget, rect2 );

	dc.Ellipse( rect1 );
	dc.Ellipse( rect2 );
}

bool CActionFourierSelectEllipse::DoStartTracking( CPoint point )
{
	m_pointCenter = point;
	return CActionFourierSelectCenter::DoStartTracking( point );

}

//////////////////////////////////////////////////////////////////////
//CActionFourierSelectFree implementation
CActionFourierSelectFree::CActionFourierSelectFree()
{
	m_pcntr1 = ContourCreate();
	m_pcntr2 = ContourCreate();
}

CActionFourierSelectFree::~CActionFourierSelectFree()
{
	::ContourDelete( m_pcntr1 );
	::ContourDelete( m_pcntr2 );
}

IUnknown	*CActionFourierSelectFree::DoCreateSelectionImage( IUnknown *punkImage )
{
	CRect	rect1 = ::ContourCalcRect( m_pcntr1 );
	CRect	rect2 = ::ContourCalcRect( m_pcntr2 );

	CRect	rect( min( rect1.left, rect2.left ), min( rect1.top, rect2.top ),
					max( rect1.right, rect2.right ), max( rect1.bottom, rect2.bottom ) );

	CWalkFillInfo	info( rect );

	{
		CDC *pdc = info.GetDC();
		pdc->SelectStockObject( WHITE_BRUSH );
		pdc->SelectStockObject( WHITE_PEN );

		::ContourDraw( *pdc, m_pcntr1, 0, cdfClosed|cdfFill );
		::ContourDraw( *pdc, m_pcntr2, 0, cdfClosed|cdfFill );
	}
	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();
	info.AttachMasksToImage( image );
	image.InitContours();

	return image.Detach();
}

void CActionFourierSelectFree::DoDraw( CDC &dc )
{
	::ContourDraw( dc, m_pcntr1, m_punkTarget, cdfClosed );
	::ContourDraw( dc, m_pcntr2, m_punkTarget, cdfClosed );
}

bool CActionFourierSelectFree::DoTrack( CPoint point )
{
	_Draw();
	_AddPoint( point );
	_Draw();
	return true;
}
bool CActionFourierSelectFree::DoStartTracking( CPoint point )
{
	if( !CActionFourierSelectCenter::DoStartTracking( point ) )
		return false;

	_Draw();
	_AddPoint( point );
	_Draw();

	return true;
}

void CActionFourierSelectFree::_AddPoint( CPoint pt1 )
{
	CPoint	pt2 = CPoint( m_rectImage.left+m_rectImage.Width()-pt1.x,
						m_rectImage.top+m_rectImage.Height()-pt1.y );

	if( m_pcntr1 && m_pcntr1->nContourSize )
		if( m_pcntr1->ppoints[m_pcntr1->nContourSize-1].x == pt1.x &&
			m_pcntr1->ppoints[m_pcntr1->nContourSize-1].y == pt1.y )
			return;

	ContourAddPoint( m_pcntr1, pt1.x, pt1.y );
	ContourAddPoint( m_pcntr2, pt2.x, pt2.y );

}
//////////////////////////////////////////////////////////////////////
//CActionFourierSelectCenter implementation
CActionFourierSelectCenter::CActionFourierSelectCenter()
{
	m_point = CPoint( -1000, -1000 );
}

CActionFourierSelectCenter::~CActionFourierSelectCenter()
{
}

IUnknown *CActionFourierSelectCenter::DoCreateSelectionImage( IUnknown *punkImage )
{
	CPoint	pointCenter = m_rectImage.CenterPoint();
	int	cx = abs( pointCenter.x - m_point.x );
	int	cy = abs( pointCenter.y - m_point.y );

	CRect	rect( pointCenter.x - cx, pointCenter.y - cy,
				pointCenter.x + cx, pointCenter.y + cy );

	CWalkFillInfo	info( rect );

	{
		CDC *pdc = info.GetDC();
		pdc->SelectStockObject( WHITE_BRUSH );
		pdc->SelectStockObject( WHITE_PEN );
		pdc->Ellipse( rect );
	}


	CImageWrp	image( m_image.CreateVImage( rect ), false );
	image.InitRowMasks();
	info.AttachMasksToImage( image );
	image.InitContours();

	return image.Detach();
}

//extended UI


void CActionFourierSelectCenter::DoDraw( CDC &dc )
{
	if( m_point.x == -1000 && m_point.y == -1000 ) return;

	CPoint	pointCenter = m_rectImage.CenterPoint();
	int	cx = abs( pointCenter.x - m_point.x );
	int	cy = abs( pointCenter.y - m_point.y );

	CRect	rect( pointCenter.x - cx, pointCenter.y - cy,
				pointCenter.x + cx, pointCenter.y + cy );

	rect = ConvertToWindow( m_punkTarget, rect );

	dc.SelectStockObject( NULL_BRUSH );
	dc.Ellipse( rect );

}

bool CActionFourierSelectCenter::DoStartTracking( CPoint point )
{
	if( m_image == 0 )
		return false;

	m_point = point;

	_Draw();

	return true;
}

bool CActionFourierSelectCenter::DoFinishTracking( CPoint point )
{
	//m_point = point;
	Finalize();
	return true;
}

bool CActionFourierSelectCenter::DoTrack( CPoint point )
{
	if( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	{
		double	dx = ::fabs( (double)(point.x - m_rectImage.CenterPoint().x) );
		double	dy = ::fabs( (double)(point.y - m_rectImage.CenterPoint().y) );

		double	fZoomX = 1.*dx/m_rectImage.Width();
		double	fZoomY = 1.*dy/m_rectImage.Height();

		double	fZoom = (fZoomX+fZoomY)/2;

		point.x = int( m_rectImage.CenterPoint().x-m_rectImage.Width()*fZoom );
		point.y = int( m_rectImage.CenterPoint().y-m_rectImage.Height()*fZoom );
	}

	if( point == m_point )
		return true;
	_Draw();
	m_point = point;
	_Draw();

	return true;
}

bool CChechAvailable::IsAvaible( IUnknown *punkTarget )
{
	IUnknown *punk = ::GetActiveObjectFromContext( punkTarget, szTypeImage );

	if( !punk )
		return 0;

	CImageWrp image( punk );
	punk->Release();

	if( image == 0 )
		return 0;
	

	if( image.GetColorConvertor().CompareNoCase( "Complex" ) )
		return 0;

	return true;
}

//	fixed selection
//////////////////////////////////////////////////////////////////////
class CFixedSelectRectCtrl: public CDrawControllerBase
{
public:
	CFixedSelectRectCtrl()
	{
		m_size.cx = 0;
		m_size.cy = 0;
	}
	void DoDraw( CDC &dc, CPoint point, IUnknown *punkVS,  bool bErase )
	{
		CRect	rect;

		rect.left = point.x-m_size.cx/2;
		rect.top = point.y-m_size.cy/2;
		rect.right = rect.left + m_size.cx;
		rect.bottom = rect.top + m_size.cy;

		rect = ::ConvertToWindow( punkVS, rect );
		dc.Rectangle( rect );
		
	}
	void SetSize( CSize	size )
	{
		m_size = size;
	}
protected:
	CSize	m_size;
};

//CActionSelectFixedRect implementation
CActionSelectFixedRect::CActionSelectFixedRect()
{
	m_left = m_top = m_width = m_height = -1;
	m_calibr = 1;
	m_bCalibr = false;
	m_pctrl = 0;
}

CActionSelectFixedRect::~CActionSelectFixedRect()
{
	ASSERT(m_pctrl==0);
}

void CActionSelectFixedRect::StoreCurrentExecuteParams()
{
	SetArgument( _T("X"), _variant_t(m_left) );
	SetArgument( _T("Y"), _variant_t(m_top) );
	SetArgument( _T("Width"), _variant_t(m_width) );
	SetArgument( _T("Height"), _variant_t(m_height) );
	SetArgument( _T("UseCalibration"), _variant_t((long)m_bCalibr) );
}

bool CActionSelectFixedRect::Initialize()
{
	AddPropertyPageDescription(c_szCSelPropPage);
	AddPropertyPageDescription( c_szCSelFixedRectPropPage );

	if( !CActionSelectBase::Initialize() )return false;

	m_left = GetArgumentDouble( _T("X") );
	m_top = GetArgumentDouble( _T("Y") );
	m_width = GetArgumentDouble( _T("Width") );
	m_height = GetArgumentDouble( _T("Height") );
	m_bCalibr = GetArgumentInt( _T("UseCalibration") )!=0;
	UpdateCalibration();

	m_pctrl = new CFixedSelectRectCtrl();
	m_pctrl->SetSize( CalcPixelSize() );
	m_pctrl->Install();

	if( m_width>0 )
	{
		Finalize();
		return false;
	}
	else
		DoUpdateSettings();
	return true;
}

bool CActionSelectFixedRect::DoLButtonDown( CPoint pt )
{
	m_left = pt.x*m_calibr-m_width/2;
	m_top = pt.y*m_calibr-m_height/2;

	Finalize();
	return true;
}

bool CActionSelectFixedRect::DoUpdateSettings()
{
	m_width = GetValueDouble( GetAppUnknown(), szFixedSelectSizes, szRectCX, 100 );
	m_height = GetValueDouble( GetAppUnknown(), szFixedSelectSizes, szRectCY, 100 );
	m_bCalibr = GetValueInt( GetAppUnknown(), szFixedSelectSizes, szUseCalubration, 0 )!=0;
	UpdateCalibration();

	if( m_pctrl )
	{
		m_pctrl->Redraw( false );
		m_pctrl->SetSize( CalcPixelSize() );
		m_pctrl->Redraw( true );
	}

	return CActionSelectBase::DoUpdateSettings();
}

void CActionSelectFixedRect::Finalize()
{
	if( m_pctrl )
	{
		m_pctrl->Uninstall();
		m_pctrl->GetControllingUnknown()->Release();
		m_pctrl = 0;
	}

	CActionSelectBase::Finalize();
}
IUnknown *CActionSelectFixedRect::DoCreateSelectionImage( IUnknown *punkImage )
{
	if( m_width < 0. || m_height < 0. )	return false;
	if( m_width < 2. && m_height < 2. ) return false;

	CRect	m_rect = CalcPixelRect();

	CImageWrp	image( punkImage );

	IUnknown *punk = image.CreateVImage( m_rect );
	return punk;
}

void CActionSelectFixedRect::UpdateCalibration()
{
	m_calibr = 1;
	if( m_bCalibr ) 
	{
		ICalibrPtr	ptrC = m_image;
		if( ptrC!= 0 )
		{
			ptrC->GetCalibration( &m_calibr, 0 );
			double	fUnitC = ::GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
			m_calibr*=fUnitC;
		}
	}
}

CRect CActionSelectFixedRect::CalcPixelRect()
{
	CRect	rect( int( m_left/m_calibr ), int( m_top/m_calibr ),
		int( (m_left+m_width)/m_calibr ), int( (m_top+m_height)/m_calibr ) );
	return rect;
}