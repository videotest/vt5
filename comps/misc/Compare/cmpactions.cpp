#include "stdafx.h"
#include "cmpactions.h"
#include "resource.h"
#include "math.h"
#include "aphine.h"
#include "convert_help.h"
#include "statusutils.h"

bool CalcIntersect( CVector2D p1_1, CVector2D p1_2, CVector2D p2_1, CVector2D p2_2, CVector2D &p )
{
	double	a_1 = p1_2.y-p1_1.y;
	double	b_1 = p1_1.x-p1_2.x;
	double	c_1 = p1_1.x*a_1+p1_1.y*b_1;
	double	c_1_ = p1_2.x*a_1+p1_2.y*b_1;
	
	double	a_2 = p2_2.y-p2_1.y;
	double	b_2 = p2_1.x-p2_2.x;
	double	c_2 = p2_1.x*a_2+p2_1.y*b_2;

	double	dx, dy, d;
	dx = c_1*b_2-c_2*b_1;
	dy = a_1*c_2-a_2*c_1;
	d = a_1*b_2-a_2*b_1;


	if( ::fabs( d ) < 0.00001 )
		return false;
	if( ::fabs( d ) < 0.00001 )
		return false;


	p.x = dx/d;
	p.y = dy/d;

	return ( p.x >= 0 && p.y >= 0 &&
			p.x <= max(p1_1.x, p1_2.x)&&p.x >= min(p1_1.x, p1_2.x)&&
			p.y <= max(p1_1.y, p1_2.y)&&p.y >= min(p1_1.y, p1_2.y) );
}


void CopyCalibration( IUnknown *punkTo, IUnknown *punkFrom )
{
	ICalibrPtr	ptrFrom( punkFrom );
	ICalibrPtr	ptrTo( punkFrom );

	if( ptrFrom == 0 || ptrTo == 0 )return;

	GUID	g;
	double	d;
	ptrFrom->GetCalibration( &d, &g );
	ptrTo->SetCalibration( d, &g );
}

bool _callback_dummy(int, long)
{
	return(true);
}

bool _callback_rotate(int nPercent, long lParam)
{
	CLongOperationImpl	*p = (CLongOperationImpl	*)lParam;
	if( p ) return p->Notify( nPercent );
	return true;
}

	

IMPLEMENT_UNKNOWN_BASE(CFilterArgImpl, Filter)
CFilterArgImpl::~CFilterArgImpl()
{
	POSITION	pos = m_args.GetHeadPosition();
	while( pos )delete (Arg*)m_args.GetNext( pos );
}

HRESULT CFilterArgImpl::XFilter::GetFirstArgumentPosition(POSITION *pnPos)
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);
	*pnPos = pThis->m_args.GetHeadPosition();

	return S_OK;
}

HRESULT CFilterArgImpl::XFilter::GetNextArgument(IUnknown **ppunkDataObject, POSITION *pnPosNext)
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);

	CFilterArgImpl::Arg* p = (CFilterArgImpl::Arg*)pThis->m_args.GetNext( (POSITION&)*pnPosNext );
	*ppunkDataObject = p->punk;
	if( *ppunkDataObject )(*ppunkDataObject)->AddRef();

	return S_OK;
}

HRESULT CFilterArgImpl::XFilter::GetArgumentInfo(POSITION lPos, BSTR *pbstrArgType, BSTR *pbstrArgName, BOOL *pbOut)
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);
	CFilterArgImpl::Arg* p = (CFilterArgImpl::Arg*)pThis->m_args.GetAt(lPos );
	if( pbstrArgType )*pbstrArgType = CString( szTypeImage ).AllocSysString();
	if( pbstrArgName )*pbstrArgName = CString( "Arg" ).AllocSysString();
	if( pbOut )*pbOut = p->bOut;
	return S_OK;
}

HRESULT CFilterArgImpl::XFilter::SetArgument(POSITION lPos, IUnknown *punkDataObject, BOOL bDataChanged)
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);
	return S_OK;
}

HRESULT CFilterArgImpl::XFilter::InitArguments()
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);
	return S_OK;
}

HRESULT CFilterArgImpl::XFilter::LockDataChanging( BOOL bLock )
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);
	return S_OK;
}

HRESULT CFilterArgImpl::XFilter::SetPreviewMode( BOOL bEnter )
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);
	return S_OK;
}

HRESULT CFilterArgImpl::XFilter::CanDeleteArgument( IUnknown *punk, BOOL *pbCan )
{
	METHOD_PROLOGUE_BASE(CFilterArgImpl, Filter);
	*pbCan = false;
	return S_OK;
}


template	<class TYPE>
class _ptr_t
{
public:
	_ptr_t( TYPE *p )
	{
		ptr = p;
		nsize = -1;//size is unknown
	}
	_ptr_t( int nCount = 0 )
	{
		ptr = 0;
		nsize = 0;
		alloc( nCount );
	}

	~_ptr_t()
	{
		free();
	}

	void zero()
	{
		if( ptr )
			memset( ptr, 0, nsize*sizeof( TYPE ) );
	}
	
	TYPE	*alloc( int nCount )
	{
		free();

		nsize = nCount;
		if( nsize )ptr = new TYPE[nsize];
		return ptr;
	}

	int size()
	{
		return nsize;
	}

	void free()
	{
		delete [] detach();
	}
	TYPE	*detach()
	{
		TYPE	*pret = ptr;
		ptr = 0;
		nsize = 0;
		return pret;
	}
	operator	TYPE*()	{return ptr;}
public:
	TYPE	*ptr;
	int		nsize;
};


//ccmacro
void StrecthDIB( BITMAPINFOHEADER *pbiSrc, byte *pbitsSrc, const RECT rectSource, const CPoint pointImageOffest,
				BITMAPINFOHEADER *pbiDest, byte *pbitsDest, const RECT rectDest,
				double fZoom, const POINT &pointScroll )
{
	if( !pbiSrc || !pbitsSrc || !pbiDest|| !pbitsDest )return;
	if( rectSource.right == rectSource.left || rectSource.bottom == rectSource.top )return;

	int		cxImg, cyImg, cx4ImgSrc;
	cxImg = rectSource.right-rectSource.left;
	cyImg = rectSource.bottom-rectSource.top;
	cx4ImgSrc = (pbiSrc->biWidth*3+3)/4*4;

	int		x, y, xImg, yImg, yBmp;
	byte	*p, *psrc, *ps;
	CPoint	pointBmpOfs = CPoint( rectDest.left, rectDest.top );
	int		cx4Bmp = (pbiDest->biWidth*3+3)/4*4;

	int	xBmpStart = max( int(floor(( rectSource.left)*fZoom-pointScroll.x)), rectDest.left );		
	int	xBmpEnd = min( int(ceil((rectSource.left+cxImg)*fZoom-pointScroll.x+.5)), rectDest.right );
	int	yBmpStart = max( int(floor((rectSource.top)*fZoom-pointScroll.y)), rectDest.top );			
	int	yBmpEnd = min( int(ceil((rectSource.top+cyImg)*fZoom-pointScroll.y+.5)), rectDest.bottom );
	int	yImgOld = -1;																				
	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);							
	int	*pxofs = new int[cx];																		
	int	*pyofs = new int[cy];																		
																									
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )														
		pxofs[t] = max( 0, min( pbiSrc->biWidth-1, int((t+xBmpStart+pointScroll.x)/fZoom-pointImageOffest.x) ) );
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )														
		pyofs[t] = max( 0, min( pbiSrc->biHeight-1, int((t+yBmpStart+pointScroll.y)/fZoom-pointImageOffest.y) ) );

																											
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )
	{
		/*get the pointer to the BGR struct*/																
		p = pbitsDest+(pbiDest->biHeight-1-(yBmp-pointBmpOfs.y))*cx4Bmp+(xBmpStart-pointBmpOfs.x)*3;
		/*get the y coord of source image*/																	
		yImg =pyofs[y-yBmpStart];
		if( yImgOld != yImg )
		{
			psrc = pbitsSrc+(pbiSrc->biHeight-1-yImg)*cx4ImgSrc;
			yImgOld = yImg;
		}
		for( x = xBmpStart; x < xBmpEnd; x++ )
		{
			/*get the x coord of source image*/
			xImg = pxofs[x-xBmpStart];

			ps = psrc+xImg*3;

			*p = *ps;p++;ps++;
			*p = *ps;p++;ps++;
			*p = *ps;p++;ps++;
		}
	}
	delete 	pxofs;
	delete 	pyofs;

}

void FillDIB( COLORREF cr, const RECT rectSource, const CPoint pointImageOffest,
				BITMAPINFOHEADER *pbiDest, byte *pbitsDest, const RECT rectDest,
				double fZoom, const POINT &pointScroll )
{
	if( !pbiDest|| !pbitsDest )return;
	byte	r = GetRValue( cr );
	byte	g = GetGValue( cr );
	byte	b = GetBValue( cr );

	int		x, y, yBmp;
	byte	*p;
	CPoint	pointBmpOfs = CPoint( rectDest.left, rectDest.top );
	int		cx4Bmp = (pbiDest->biWidth*3+3)/4*4;
	int	nZoom = int( fZoom*256 );
	int	xBmpStart = max( int(floor( rectSource.left*nZoom/256.-pointScroll.x+.5)), rectDest.left );
	int	xBmpEnd = min( int(ceil( rectSource.right*nZoom/256.-pointScroll.x+.5)), rectDest.right );
	int	yBmpStart = max( int(floor( rectSource.top*nZoom/256.-pointScroll.y+.5)), rectDest.top );
	int	yBmpEnd = min( int(ceil( rectSource.bottom*nZoom/256.-pointScroll.y+.5)), rectDest.bottom );
	int	yImgOld = -1;
																											
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )
	{
		/*get the pointer to the BGR struct*/																
		p = pbitsDest+(pbiDest->biHeight-1-(yBmp-pointBmpOfs.y))*cx4Bmp+(xBmpStart-pointBmpOfs.x)*3;

		for( x = xBmpStart; x < xBmpEnd; x++ )
		{
			*p = b;p++;
			*p = g;p++;
			*p = r;p++;
		}
	}
}

//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionAphineByPoints, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionRotateImage, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionCompareImage, CCmdTargetEx);

// olecreate 


// {47F9FDDA-8968-4529-BA04-90C035569E9D}
GUARD_IMPLEMENT_OLECREATE(CActionAphineByPoints, "Compare.AphineByPoints",
0x47f9fdda, 0x8968, 0x4529, 0xba, 0x4, 0x90, 0xc0, 0x35, 0x56, 0x9e, 0x9d);
// {AF694381-1392-40af-A457-768379AA2468}
GUARD_IMPLEMENT_OLECREATE(CActionRotateImage, "Compare.Rotate",
0xaf694381, 0x1392, 0x40af, 0xa4, 0x57, 0x76, 0x83, 0x79, 0xaa, 0x24, 0x68);
// {80278473-95E7-49ac-8E00-97C148589FE6}
GUARD_IMPLEMENT_OLECREATE(CActionCompareImage, "Compare.CompareImage",
0x80278473, 0x95e7, 0x49ac, 0x8e, 0x0, 0x97, 0xc1, 0x48, 0x58, 0x9f, 0xe6);

BEGIN_INTERFACE_MAP(CActionCompareImage, CInteractiveActionBase)
	INTERFACE_PART(CActionCompareImage, IID_ILongOperation, Long)
	INTERFACE_PART(CActionCompareImage, IID_IFilterAction, Filter)
END_INTERFACE_MAP()

BEGIN_INTERFACE_MAP(CActionAphineByPoints, CInteractiveActionBase)
	INTERFACE_PART(CActionCompareImage, IID_ILongOperation, Long)
	INTERFACE_PART(CActionCompareImage, IID_IFilterAction, Filter)
END_INTERFACE_MAP()

// guidinfo 

// {4BF5CE28-2E53-4b0d-96A4-340503C53201}
static const GUID guidRotate =
{ 0x4bf5ce28, 0x2e53, 0x4b0d, { 0x96, 0xa4, 0x34, 0x5, 0x3, 0xc5, 0x32, 0x1 } };
// {E3F4924E-12CA-4917-A0EA-3C42A2AF129C}
static const GUID guidCompareImage =
{ 0xe3f4924e, 0x12ca, 0x4917, { 0xa0, 0xea, 0x3c, 0x42, 0xa2, 0xaf, 0x12, 0x9c } };
// {CB986638-80C5-46da-935F-9F053DD99C76}
static const GUID guidAphineByPoints =
{ 0xcb986638, 0x80c5, 0x46da, { 0x93, 0x5f, 0x9f, 0x5, 0x3d, 0xd9, 0x9c, 0x76 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionAphineByPoints, IDS_ACTION_BYPOINT, -1, -1, guidAphineByPoints);
ACTION_INFO_FULL(CActionRotateImage, IDS_ACTION_ROTATE, -1, -1, guidRotate);
ACTION_INFO_FULL(CActionCompareImage, IDS_ACTION_COMPARE, -1, -1, guidCompareImage);

//[ag]7. targets

ACTION_TARGET(CActionAphineByPoints, szTargetViewSite);
ACTION_TARGET(CActionRotateImage, szTargetViewSite);
ACTION_TARGET(CActionCompareImage, szTargetViewSite);

//[ag]8. arguments


//[ag]9. implementation


//////////////////////////////////////////////////////////////////////
//CActionRotateImage implementation

void CActionCompareImage::_InstallDrawing( bool bInstall )
{
	IWindow2Ptr	ptr( m_punkTarget );
	if( ptr == 0 )return;

	if( bInstall )ptr->AttachMsgListener( GetControllingUnknown() );
	else ptr->DetachMsgListener( GetControllingUnknown() );
}
bool CActionCompareImage::Notify( int nPos )
{
	return CLongOperationImpl::Notify( nPos );
}

CActionRotateImage::CActionRotateImage()
{
	m_bSingleImage = true;
}

CActionRotateImage::~CActionRotateImage()
{
}


bool CActionRotateImage::DoVirtKey( UINT nVirtKey )
{
	if( nVirtKey == 27 )
	{
		m_bManualTerminated = true;
		Terminate();
		LeaveMode();
		return true;
	}

	return CActionCompareImage::DoVirtKey( nVirtKey );
}

bool CActionRotateImage::Invoke()
{
	m_bManualTerminated = true;
	//init aphine transform params
	double	fZoomX = m_nWidth/100., fZoomY = m_nHeight/100.;
	double	fAngle = m_nAngle/180.*PI;
	double co=::cos(fAngle), si=::sin(fAngle);
	CPoint	ptOldCenter = m_imageOther.GetRect().CenterPoint();

	CAphineParams params=
		CAphineParams(
			CMatrix2x2(co, -si, si, co)*CMatrix2x2(fZoomX, 0, 0, fZoomY ),
			CVector2D(ptOldCenter.x,ptOldCenter.y),
			CVector2D(ptOldCenter.x+m_nXPos,ptOldCenter.y+m_nYPos)/*new*/);

	_size new_size;
	_point new_offset;
	atCalcNewSizeAndOffsetMasked(IImage2Ptr(m_imageOther),  params,
		new_size, new_offset );

	CRect rect=CRect(new_offset.x, new_offset.y,
		new_offset.x+new_size.cx, new_offset.y+new_size.cy);

	CImageWrp image;

	IUnknown	*punkParent = 0;

	{
		INamedDataObject2Ptr	ptrN( m_imageOther );
		ptrN->GetParent( &punkParent );
	}

	bool	bFullImage = true;
	if( punkParent )
	{
		if( !image.CreateNew( rect.Width(), rect.Height(), m_imageOther.GetColorConvertor() ) )
		{
			AfxMessageBox( IDS_CANT_CREATE_DESTINATION );
			return false;
		}

		image.SetOffset( rect.TopLeft() );

		INamedDataObject2Ptr	ptrN( image );
		ptrN->SetParent( punkParent, 0 );
		punkParent->Release();

		
		bFullImage = false;
	}
	else
	{
		if (!image.CreateCompatibleImage( m_imageOther, false, rect))
			return(false);
		bFullImage = true;
	}

	image->InitRowMasks();	


	try{
	_InstallDrawing( true );
	StartNotification( 101, 1, 1 );
	atExecute( IImage2Ptr(m_imageOther), IImage2Ptr(image), params, 
		_callback_rotate, (long)(CLongOperationImpl*)this);
	FinishNotification();
	_InstallDrawing( false );
	}
	catch( CException *pe )
	{
		_InstallDrawing( false );
		throw pe;
	}


	if( bFullImage )
	{
		_convert	convert;

		image.SetOffset( CPoint(0, 0) );
		CRect	rect = image.GetRect();
		convert.init( image );
		convert.set_rgb( ::GetValueColor(GetAppUnknown(), "\\Image", "Background", RGB(255,255,255) ) );

		int	x, c, c_max = convert.get_panes();
		color	*p;
		byte	*pmask;
		color	col;

		for( int i = 0; i < rect.bottom; i++ ) 
		{
			image->GetRowMask( i, &pmask );

			for( c = 0; c < c_max; c++ )
			{
				col = convert.get_color( c );
				image->GetRow( i, c, &p );
				for( x = 0; x < rect.right; x++ )
					if( !pmask[x] )p[x] = col;
			}

			memset( pmask, 0xff, rect.Width() );
		}
	}

	IViewPtr	ptrV( m_punkTarget );
	IUnknown	*punkDoc = 0;
	ptrV->GetDocument( &punkDoc );
	if( !punkDoc )return false;
	m_lTargetKey = ::GetObjectKey( punkDoc );
	IUnknownPtr	ptrDoc = punkDoc;
	punkDoc->Release();

	if( !bFullImage )
		image->InitContours();

	CopyCalibration( image, m_imageOther );
	{
		INamedDataObject2Ptr	ptr1( image ), ptr2( m_imageOther );

		GUID	baseKey;
		ptr2->GetBaseKey( &baseKey );
		ptr1->SetBaseKey( &baseKey );

		SetObjectName( image, CreateUniqueName( punkDoc, GetObjectName( m_imageOther ) ) );
	}

	m_changes.SetToDocData( ptrDoc, image );
	if( GetValueInt( GetAppUnknown(), "\\General", "DeleteFilterArgs", 0 ) )
		m_changes.RemoveFromDocData( ptrDoc, m_imageOther, false );



	AddArgumentToFilter( image, true );


	DeleteBitmap();
	FreeDIBCache();

	LeaveMode();

	return true;
}

bool CActionRotateImage::Initialize()
{
	::SetValue( GetAppUnknown(), "Compare", "Angle", 0l );
	::SetValue( GetAppUnknown(), "Compare", "Width", 100l );
	::SetValue( GetAppUnknown(), "Compare", "Height", 100l );
	::SetValue( GetAppUnknown(), "Compare", "XPos", 0l );
	::SetValue( GetAppUnknown(), "Compare", "YPos", 0l );	
	::SetValue( GetAppUnknown(), "Compare", "Compare", 0l );	

	
	InitBackground();

	AddPropertyPageDescription( c_szCCmpPage );
	UpdatePropertyPages();

	m_imageOther.Attach( GetActiveObjectFromContext( m_punkTarget, szTypeImage ), false );

	if( m_imageOther == 0 )
	{
		Terminate();
		LeaveMode();
		AfxMessageBox( IDS_IMAGE_REQUIRED );
		return false;
	}


	IUnknown	*punkParent = 0;

	{
		INamedDataObject2Ptr	ptrN( m_imageOther );
		ptrN->GetParent( &punkParent );
	}
	if( punkParent )
	{
		m_image = punkParent;
		punkParent->Release();
	}

	if( m_image != 0 )
	{
		AddArgumentToFilter( m_image, false );
		m_rectAll = m_image.GetRect();
	}
	else
	{
		AddArgumentToFilter( m_imageOther, false );
		m_rectAll = m_imageOther.GetRect();
	}

	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );

	CRect	rect = m_imageOther.GetRect();
	m_frame.Init( m_punkTarget, pwnd->GetSafeHwnd() );
	m_frame.SetRect( rect );
	m_frame.SetFlags( fcfRotate|fcfResize );
	m_frame.Redraw();

	
	InitDIBCache();
	MakeBitmap();
	MakeContours();

	if( !CInteractiveActionBase::Initialize() )return false;


	
	if( pwnd )pwnd->SetFocus( );




	return true;
}

/*bool CActionRotateImage::Invoke()
{
	return true;
}*/


//////////////////////////////////////////////////////////////////////
//CActionCompareImage implementation
class DrawDib
{
public:
	DrawDib()
	{		m_hDrawDib = ::DrawDibOpen();	}
	~DrawDib()
	{		::DrawDibClose( m_hDrawDib );	}
	operator HDRAWDIB()
	{	return m_hDrawDib;}
protected:
	HDRAWDIB	m_hDrawDib;
};
DrawDib	DrawDibInstance;


CActionCompareImage::CActionCompareImage()
{
	m_location = CResizingFrame::None;
	m_bManualTerminated = false;
	m_bSingleImage = false;

	m_pbiIntersect = 0;
	m_pdibIntersect = 0;
	m_rectDIB = NORECT;
	m_pdib1 =
	m_pdib2 = 0;
	m_pbi1 = 
	m_pbi2 = 0;

	m_pmask = 0;
	m_pmaskNoRotate = 0;
	m_nOverlayMode = 0;

	m_nCurrentWidth = 100;
	m_nCurrentHeight = 100;
	m_nCurrentXPos = 0;
	m_nCurrentYPos = 0;
	m_nCurrentAngle = 0;
	m_pointStart = CPoint( 0, 0 );

	m_nWidth = 100;
	m_nHeight = 100;
	m_nXPos = 0;
	m_nYPos = 0;
	m_nAngle = 0;
	m_pointStart = CPoint( 0, 0 );
	m_pbiNoRotate = 0;
	m_pdibNoRotate = 0;
	m_rectOther = NORECT;
}

CActionCompareImage::~CActionCompareImage()
{
	DeleteBitmap();
	FreeDIBCache();

}

bool CActionCompareImage::Invoke()
{
	CImageWrp	image, image1;

	if( m_pbi1 )
	{
		image.AttachDIBBits( m_pbi1, m_pdib1 );
	}
	else
	{
		IScrollZoomSitePtr	ptrZS( m_punkTarget );
		CSize	size;
		ptrZS->GetClientSize( &size );
		if( !image.CreateNew( size.cx, size.cy, "RGB" ) )
			return false;

		//fill it
		int	x, y;
		color	*p, c;

		for( y = 0; y < size.cy; y++ )
		{
			c = ByteAsColor( m_r );
			p = image.GetRow( y, 0 );

			for( x = 0; x < size.cx; x++, p++ )
				*p = c;

			c = ByteAsColor( m_g );
			p = image.GetRow( y, 1 );

			for( x = 0; x < size.cx; x++, p++ )
				*p = c;

			c = ByteAsColor( m_b );
			p = image.GetRow( y, 2 );
			for( x = 0; x < size.cx; x++, p++ )
				*p = c;
		}
	}

	//init aphine transform params
	double	fZoomX = m_nWidth/100., fZoomY = m_nHeight/100.;
	double	fAngle = m_nAngle/180.*PI;
	double co=::cos(fAngle), si=::sin(fAngle);
	CPoint	ptOldCenter = m_imageOther.GetRect().CenterPoint();

	CAphineParams params=
		CAphineParams(
			CMatrix2x2(co, -si, si, co)*CMatrix2x2(fZoomX, 0, 0, fZoomY ),
			CVector2D(ptOldCenter.x,ptOldCenter.y),
			CVector2D(ptOldCenter.x+m_nXPos,ptOldCenter.y+m_nYPos)/*new*/);

	_size new_size;
	_point new_offset;
	atCalcNewSizeAndOffset(IImage2Ptr(m_imageOther),  params,
		new_size, new_offset );

	CRect rect=CRect(new_offset.x, new_offset.y,
		new_offset.x+new_size.cx, new_offset.y+new_size.cy);

	CImageWrp imageWork;

	if (!imageWork.CreateCompatibleImage( m_imageOther, false, rect))
		return(false);
	
	imageWork->InitRowMasks();


	try{
	_InstallDrawing( true );
	StartNotification( 101, 1, 1 );
	atExecute( IImage2Ptr(m_imageOther), IImage2Ptr(imageWork), params, 
		_callback_rotate, (long)(CLongOperationImpl*)this);
	FinishNotification();
	_InstallDrawing( false );
	}
	catch( CException *pe )
	{
		_InstallDrawing( false );
		throw pe;
	}
	

	IViewPtr	ptrV( m_punkTarget );
	IUnknown	*punkDoc = 0;
	ptrV->GetDocument( &punkDoc );
	if( !punkDoc )return false;
	m_lTargetKey = ::GetObjectKey( punkDoc );
	IUnknownPtr	ptrDoc = punkDoc;
	punkDoc->Release();

//Kir - BT5345 - преобразование изображения в формат исходных
	CString cc1 = m_image.GetColorConvertor();
	CString cc2 = m_imageOther.GetColorConvertor();

	bool bothHSB = false;
	if(cc1 == cc2 && cc1!="RGB" && cc1!="HSB")
	{
		image = ConvertImage(image,cc1); 
	}
	else if(cc1 != cc2)
	{
		imageWork = ConvertImage(imageWork,"RGB");
	}
	else if(cc1 == cc2 && cc1=="HSB")
	{
		imageWork = ConvertImage(imageWork,"RGB");
		bothHSB = true;
	}

//Kir - BT5345 -end



	//Теперь надо совместить новый image со старым.
	int	cx = image.GetWidth();
	int	cy = image.GetHeight();

	rect &=  image.GetRect();

	int colors = image.GetColorsCount(), y, c;

	for( c = 0; c < colors; c++ )
	{
		for( y = rect.top
			; y < rect.bottom; y++ )
		{
			color *p = image.GetRow( y, c )+rect.left;
			color *p1 = imageWork.GetRow( y-new_offset.y, c )+rect.left-new_offset.x;
			byte *pMask=imageWork.GetRowMask( y-new_offset.y)+rect.left-new_offset.x;

			if( m_nOverlayMode == 0 )
			{
				for(int x = rect.left; x < rect.right; x++, p++, p1++, pMask++) 
				{
					if(*pMask) *p=*p1;
				}			
			}
			else if( m_nOverlayMode == 1 )
			{
				for(int x = rect.left; x < rect.right; x++, p++, p1++, pMask++) 
				{
					if(*pMask) *p=(*p+*p1)>>1;;
				}			
			}
			else if( m_nOverlayMode == 2 )
			{
				for(int x = rect.left; x < rect.right; x++, p++, p1++, pMask++) 
				{
					if(*pMask) *p=max( 0, min( colorMax, colorMax/2+*p-*p1));
				}			
			}
		}
	}

	if(bothHSB) image =ConvertImage(image, "HSB");
	CopyCalibration( image, m_imageOther );

	m_changes.SetToDocData( ptrDoc, image );
	if( image != 0 )AddArgumentToFilter( image, true );


	DeleteBitmap();
	FreeDIBCache();

	LeaveMode();

	

	return true;
}

CImageWrp CActionCompareImage::ConvertImage(CImageWrp& image,CString cc)
{
		long	cx = image.GetWidth(), cy = image.GetHeight();

		CImageWrp imCnv;
		imCnv.CreateCompatibleImage( image,  true, NORECT, cc);

		long	colors =max(imCnv.GetColorsCount(),image.GetColorsCount());

		double*		pLab;
		pLab = new double[cx*3];

		IUnknown* punk;
		imCnv->GetColorConvertor(&punk);
		sptrIColorConvertor3 sptrCCDest(punk);
		punk->Release();

		image->GetColorConvertor(&punk);
		sptrIColorConvertor3 sptrCCScr(punk);
		punk->Release();

		color	**ppcolorscr = new color*[colors];
		color	**ppcolordest = new color*[colors];
		for( long y = 0; y < cy; y++ )
		{
			for( long c = 0; c  < colors; c++ )
			{
				ppcolorscr[c] = image.GetRow( y, c );
				ppcolordest[c] = imCnv.GetRow( y, c );
			}
			sptrCCScr->ConvertImageToLAB(ppcolorscr, pLab, cx);
			sptrCCDest->ConvertLABToImage(pLab, ppcolordest, cx);
		}

		delete pLab;
		delete ppcolorscr;
		delete ppcolordest;

		return (IUnknown*)imCnv;
}
bool CActionCompareImage::IsAvaible()
{
	return CheckInterface( m_punkTarget, IID_IImageView );
}


void CActionCompareImage::MakeBitmap()
{
	if( !m_pbi2 )
		return;

	InitBackground();

	if( m_nCurrentWidth != m_nWidth ||
		m_nCurrentHeight != m_nHeight ||
		m_nCurrentAngle != m_nAngle)
	{
		ResizeRotate();
	}


	CRect	rect1 = m_image.GetRect();
	CRect	rect2 = m_rectOther;//m_imageOther.GetRect();//+CPoint( m_nCurrentXPos, m_nCurrentYPos );
	if( rect1 == NORECT )rect1 = m_rectAll;

	ASSERT( m_pbi2->biHeight == m_rectOther.Height() );
	ASSERT( m_pbi2->biWidth == m_rectOther.Width() );

	m_rectDIB.left = max( rect1.left, min( rect1.right, rect2.left+m_nXPos ) );
	m_rectDIB.right = max( rect1.left, min( rect1.right, rect2.right+m_nXPos ) );
	m_rectDIB.top = max( rect1.top, min( rect1.bottom, rect2.top+m_nYPos ) );
	m_rectDIB.bottom = max( rect1.top, min( rect1.bottom, rect2.bottom+m_nYPos ) );

	if( m_rectDIB.left >= m_rectDIB.right ||
		m_rectDIB.top >= m_rectDIB.bottom )
	{
		m_rectDIB = NORECT;
		return;
	}

	if( m_pbiIntersect )
	{
		if( m_rectDIB.Width() != m_pbiIntersect->biWidth ||
			m_rectDIB.Height() != m_pbiIntersect->biHeight )
		{
			delete m_pbiIntersect;
			m_pbiIntersect = 0;
			m_pdibIntersect = 0;
		}
	}
			



	int	cx = m_rectDIB.Width();
	int	cy = m_rectDIB.Height();
	int	cbRow = (cx*3+3)/4*4;

	int	cbRow1 = (rect1.Width()*3+3)/4*4;
	int	cbRow2 = (rect2.Width()*3+3)/4*4;


	if( !m_pbiIntersect )
	{
		DWORD	dwDIBSize = sizeof( BITMAPINFOHEADER ) + cbRow*cy;
		m_pdibIntersect = new byte[dwDIBSize];
		m_pbiIntersect = (BITMAPINFOHEADER*)m_pdibIntersect;
		m_pdibIntersect+=sizeof(BITMAPINFOHEADER);
		ZeroMemory( m_pbiIntersect, sizeof( BITMAPINFOHEADER ) );
		m_pbiIntersect->biBitCount = 24;
		m_pbiIntersect->biHeight = cy;
		m_pbiIntersect->biWidth = cx;
		m_pbiIntersect->biSize = sizeof( BITMAPINFOHEADER );
		m_pbiIntersect->biPlanes = 1;
	}

	int	x, y;

	int	xSource1 = m_rectDIB.left,
					xSource2 = m_rectDIB.left-rect2.left-m_nXPos,
					xDest = 0,
					ySource1 = m_rectDIB.top,
					ySource2 = m_rectDIB.top-rect2.top-m_nYPos,
					yDest = 0;

	
	for( y = 0; y < cy; y++ )
	{
		byte	*prow = m_pdibIntersect+(m_pbiIntersect->biHeight-1-(y+yDest))*cbRow+xDest*3;
		byte	*prowSrc1 = m_pdib1?(m_pdib1+(m_pbi1->biHeight-1-(y+ySource1))*cbRow1+xSource1*3):0;
		byte	*prowSrc2 = m_pdib2+(m_pbi2->biHeight-1-(y+ySource2))*cbRow2+xSource2*3;
		byte	*pmask = m_pmask+(m_pbi2->biHeight-1-(y+ySource2))*m_pbi2->biWidth+xSource2;

		for( x = 0; x < cx; x++ )
		{
			if( prowSrc1 )
			{
				if( *pmask )
				{
					if( m_nOverlayMode == 0 )
					{
						*prow = *prowSrc2;prowSrc2++;prow++;prowSrc1++;
						*prow = *prowSrc2;prowSrc2++;prow++;prowSrc1++;
						*prow = *prowSrc2;prowSrc2++;prow++;prowSrc1++;
					}
					else if( m_nOverlayMode == 1 )
					{
						*prow = (*prowSrc1+*prowSrc2)>>1;prowSrc1++;prowSrc2++;prow++;
						*prow = (*prowSrc1+*prowSrc2)>>1;prowSrc1++;prowSrc2++;prow++;
						*prow = (*prowSrc1+*prowSrc2)>>1;prowSrc1++;prowSrc2++;prow++;
					}
					else if( m_nOverlayMode == 2 )
					{
						*prow = max( 0, min( 255, 128+*prowSrc1-*prowSrc2));prowSrc1++;prowSrc2++;prow++;
						*prow = max( 0, min( 255, 128+*prowSrc1-*prowSrc2));prowSrc1++;prowSrc2++;prow++;
						*prow = max( 0, min( 255, 128+*prowSrc1-*prowSrc2));prowSrc1++;prowSrc2++;prow++;
					}
				}
				else
				{
					*prow = (*prowSrc1);prow++;prowSrc1++;prowSrc2++;
					*prow = (*prowSrc1);prow++;prowSrc1++;prowSrc2++;
					*prow = (*prowSrc1);prow++;prowSrc1++;prowSrc2++;
				}
			}
			else
			{
				if( *pmask )
				{
					*prow = *prowSrc2;prow++;prowSrc2++;
					*prow = *prowSrc2;prow++;prowSrc2++;
					*prow = *prowSrc2;prow++;prowSrc2++;
				}
				else
				{
					*prow = m_b;prow++;
					*prow = m_g;prow++;
					*prow = m_r;prow++;
					prowSrc2++;
					prowSrc2++;
					prowSrc2++;
				}
			}
			pmask++;
		}
	}
}

void CActionCompareImage::DeleteBitmap()
{
	if( m_pbiIntersect )
	{
		delete m_pbiIntersect;
	}
	m_pbiIntersect = 0;
	m_pdibIntersect = 0;
	m_rectDIB = NORECT;
}

/*void CActionCompareImage::DoDraw( CDC &dc )
{
	if( !m_pbiIntersect )
		return;


	//calc 5 rects 
	//|-----------------------------------|
	//|rectTop							  |
	//|-----------------------------------|
	//|		   |		 |				  |
	//|rectLeft|rectInter|rectRight       |
	//|-----------------------------------|
	//|	rectBottom           			  |
	//|-----------------------------------|
	//and intersect it with update rectangle


	CPoint	pointScroll = ::GetScrollPos( m_punkTarget );
	double fZoom = ::GetZoom( m_punkTarget );
	CRect	rectUpdate;

	rectUpdate.left = (int)ceil( (m_rcInvalid.left+pointScroll.x)/fZoom );
	rectUpdate.right = (int)floor( (m_rcInvalid.right+pointScroll.x)/fZoom );
	rectUpdate.top = (int)ceil( (m_rcInvalid.top+pointScroll.y)/fZoom );
	rectUpdate.bottom = (int)floor( (m_rcInvalid.bottom+pointScroll.y)/fZoom );

	CRect	rectLeft, rectTop, rectRight, rectBottom, rectWork;
	int	biWidth = m_pbi1?m_pbi1->biWidth:m_pbi2->biWidth;
	int	biHeight = m_pbi1?m_pbi1->biHeight:m_pbi2->biHeight;
	
	rectTop.left = max( rectUpdate.left, min( rectUpdate.right, 0 ) );
	rectTop.right = max( rectUpdate.left, min( rectUpdate.right, biWidth ) );
	rectTop.top = max( rectUpdate.top, min( rectUpdate.bottom, 0 ) );
	rectTop.bottom = max( rectUpdate.top, min( rectUpdate.bottom, m_rectDIB.top ) );

	rectBottom.left = max( rectUpdate.left, min( rectUpdate.right, 0 ) );
	rectBottom.right = max( rectUpdate.left, min( rectUpdate.right, biWidth ) );
	rectBottom.top = max( rectUpdate.top, min( rectUpdate.bottom, m_rectDIB.bottom ) );
	rectBottom.bottom = max( rectUpdate.top, min( rectUpdate.bottom, biHeight ) );

	rectLeft.top = rectTop.bottom;
	rectLeft.bottom = rectBottom.top;
	rectLeft.left = max( rectUpdate.left, min( rectUpdate.right, 0 ) );
	rectLeft.right = max( rectUpdate.left, min( rectUpdate.right, m_rectDIB.left ) );

	rectRight.top = rectTop.bottom;
	rectRight.bottom = rectBottom.top;
	rectRight.left = max( rectUpdate.left, min( rectUpdate.right, m_rectDIB.right ) );
	rectRight.right = max( rectUpdate.left, min( rectUpdate.right, biWidth ) );

	rectWork.left = rectLeft.right;
	rectWork.right = rectRight.left;
	rectWork.top = rectTop.bottom;
	rectWork.bottom = rectBottom.top;


	CRect	rectDraw;
	CBrush	brush( m_crBack );
	CPen	pen( PS_SOLID, 0, RGB( 0, 0, 0 ) );
	CBrush	*poldBrush = dc.SelectObject( &brush );
	CPen	*poldPen = dc.SelectObject( &pen );
	dc.SetROP2( R2_COPYPEN );

	if( rectTop.left < rectTop.right && rectTop.top < rectTop.bottom )
	{
		rectDraw.top = (int)ceil(rectTop.top*fZoom-pointScroll.y);
		rectDraw.bottom = (int)floor(rectTop.bottom*fZoom-pointScroll.y);
		rectDraw.left = (int)ceil(rectTop.left*fZoom-pointScroll.x);
		rectDraw.right = (int)floor(rectTop.right*fZoom-pointScroll.x);

		if( m_pdib1 )
			::DrawDibDraw( DrawDibInstance, dc, rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(),
			m_pbi1, m_pdib1, rectTop.left, rectTop.top, rectTop.Width(), rectTop.Height(), 0 );
		else
			dc.FillRect( rectDraw, &brush );
	}

	if( rectLeft.left < rectLeft.right && rectLeft.top < rectLeft.bottom )
	{
		rectDraw.top = (int)ceil(rectLeft.top*fZoom-pointScroll.y);
		rectDraw.bottom = (int)floor(rectLeft.bottom*fZoom-pointScroll.y);
		rectDraw.left = (int)ceil(rectLeft.left*fZoom-pointScroll.x);
		rectDraw.right = (int)floor(rectLeft.right*fZoom-pointScroll.x);

		if( m_pdib1 )
			::DrawDibDraw( DrawDibInstance, dc, rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(),
				m_pbi1, m_pdib1, rectLeft.left, rectLeft.top, rectLeft.Width(), rectLeft.Height(), 0 );
		else
			dc.FillRect( rectDraw, &brush );
	}

	if( rectWork.left < rectWork.right && rectWork.top < rectWork.bottom )
	{
		rectDraw.top = (int)ceil(rectWork.top*fZoom-pointScroll.y);
		rectDraw.bottom = (int)floor(rectWork.bottom*fZoom-pointScroll.y);
		rectDraw.left = (int)ceil(rectWork.left*fZoom-pointScroll.x);
		rectDraw.right = (int)floor(rectWork.right*fZoom-pointScroll.x);

		rectWork-=m_rectDIB.TopLeft();

		::DrawDibDraw( DrawDibInstance, dc, rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(),
			m_pbiIntersect, m_pdibIntersect, rectWork.left, rectWork.top, rectWork.Width(), rectWork.Height(), 0 );
		//dc.FillRect( rectDraw, &brush );
	}

	if( rectRight.left < rectRight.right && rectRight.top < rectRight.bottom )
	{
		rectDraw.top = (int)ceil(rectRight.top*fZoom-pointScroll.y);
		rectDraw.bottom = (int)floor(rectRight.bottom*fZoom-pointScroll.y);
		rectDraw.left = (int)ceil(rectRight.left*fZoom-pointScroll.x);
		rectDraw.right = (int)floor(rectRight.right*fZoom-pointScroll.x);

		if( m_pdib1 )
			::DrawDibDraw( DrawDibInstance, dc, rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(),
				m_pbi1, m_pdib1, rectRight.left, rectRight.top, rectRight.Width(), rectRight.Height(), 0 );
		else
			dc.FillRect( rectDraw, &brush );
	}

	if( rectBottom.left < rectBottom.right && rectBottom.top < rectBottom.bottom )
	{
		rectDraw.top = (int)ceil(rectBottom.top*fZoom-pointScroll.y);
		rectDraw.bottom = (int)floor(rectBottom.bottom*fZoom-pointScroll.y);
		rectDraw.left = (int)ceil(rectBottom.left*fZoom-pointScroll.x);
		rectDraw.right = (int)floor(rectBottom.right*fZoom-pointScroll.x);

		if( m_pdib1 )
			::DrawDibDraw( DrawDibInstance, dc, rectDraw.left, rectDraw.top, rectDraw.Width(), rectDraw.Height(),
				m_pbi1, m_pdib1, rectBottom.left, rectBottom.top, rectBottom.Width(), rectBottom.Height(), 0 );
		else
			dc.FillRect( rectDraw, &brush );
	}


	dc.SelectObject( poldPen );
	dc.SelectObject( poldBrush );
}*/

bool CActionCompareImage::Initialize()
{
	//_ReportCounter( m_punkTarget );
	::SetValue( GetAppUnknown(), "Compare", "Angle", 0l );
	::SetValue( GetAppUnknown(), "Compare", "Width", 100l );
	::SetValue( GetAppUnknown(), "Compare", "Height", 100l );
	::SetValue( GetAppUnknown(), "Compare", "XPos", 0l );
	::SetValue( GetAppUnknown(), "Compare", "YPos", 0l );	
	::SetValue( GetAppUnknown(), "Compare", "Compare", 1l );	

	UpdatePropertyPages();

	InitBackground();

	AddPropertyPageDescription( c_szCCmpPage );

	if( !CInteractiveActionBase::Initialize() )return false;


	_bstr_t	bstr( szTypeImage );
	LONG_PTR	pos = 0;

	IDataContext3Ptr	ptrC( m_punkTarget );
	ptrC->GetFirstSelectedPos( bstr, &pos );

	IUnknown	*punk1 = 0, *punk2 = 0;

	if( pos )ptrC->GetNextSelected( bstr, &pos, &punk1 );
	if( pos )ptrC->GetNextSelected( bstr, &pos, &punk2 );

	m_image = punk1;
	m_imageOther = punk2;

	if( punk1 )punk1->Release();
	if( punk2 )punk2->Release();

	if( m_imageOther==0 )
	{
		AfxMessageBox( IDS_NOIMAGE );
		LeaveMode();
		Terminate();
		return false;
	}

	if( m_image.IsEmpty() ||
		m_imageOther.IsEmpty() )
	{
		AfxMessageBox( IDS_CANT_COMPARE_EMPTY );
		LeaveMode();
		Terminate();
		return false;
	}

	if( ::GetParentKey( m_image ) != INVALID_KEY &&
		::GetParentKey( m_imageOther ) == INVALID_KEY )
	{
		IUnknown	*punkTemp = m_image;
		m_image = m_imageOther;
		m_imageOther = punkTemp;
	}

	if( ::GetParentKey( m_image ) != INVALID_KEY )
	{
		AfxMessageBox( IDS_NOSELECTIONIMAGE );
		LeaveMode();
		Terminate();
		return false;
	}

	if( m_image != 0 )AddArgumentToFilter( m_image, false );
	if( m_imageOther != 0 )AddArgumentToFilter( m_imageOther, false );

	m_rectAll = m_image.GetRect();


	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );

	CRect	rect = m_imageOther.GetRect();
	m_frame.Init( m_punkTarget, pwnd->GetSafeHwnd() );
	m_frame.SetRect( rect );
	m_frame.SetFlags( fcfRotate|fcfResize );
	m_frame.Redraw();

	InitDIBCache();
	MakeBitmap();
	MakeContours();
	Invalidate();

	if( pwnd )pwnd->SetFocus( );
	return true;
}

void CActionCompareImage::Finalize()
{
	InvalidateIntersect();
	m_frame.Init( 0, 0 );

	::SetValue(GetAppUnknown(), "\\IntelliMouse", "Enable key scroll", m_bEnableScroll);
	if(m_punkTarget)
	{
		IViewSitePtr vs(m_punkTarget);
		if(vs)
		{
			vs->InitView(0);
		}
	}

	CInteractiveActionBase::Finalize();
}

void CActionCompareImage::Invalidate( CRect rect )
{
	CWnd	*pwnd = GetWindowFromUnknown( m_punkTarget );

	if( !pwnd )
		return;
	if( rect == NORECT )
		pwnd->Invalidate();
	else
		pwnd->InvalidateRect( rect );
}

void CActionCompareImage::InitDIBCache()
{
	FreeDIBCache();

	if( m_image != 0 )_InitCache( m_image, &m_pbi1, &m_pdib1, false );
	if( m_imageOther != 0 )_InitCache( m_imageOther, &m_pbi2, &m_pdib2, true );

	m_rectOther = m_imageOther.GetRect();
}

void CActionCompareImage::FreeDIBCache()
{
	if( m_pbi1 )delete m_pbi1;
	if( m_pbi2 )delete m_pbi2;
	if( m_pbiNoRotate )delete m_pbiNoRotate;
	if( m_pmask )delete m_pmask;
	if( m_pmaskNoRotate )delete m_pmaskNoRotate;

	m_pmask = 0;
	m_pmaskNoRotate = 0;

	m_pbi1 = 0;
	m_pbi2 = 0;
	m_pbiNoRotate = 0;

	m_pdib1 = 0;
	m_pdib2 = 0;
	m_pdibNoRotate = 0;
}

void CActionCompareImage::_InitCache( CImageWrp &image, BITMAPINFOHEADER **ppbi, byte **ppbyte, bool bInitMask )
{
	int	cx = image.GetWidth();
	int	cy = image.GetHeight();
	int	cbRow = (cx*3+3)/4*4;
	BITMAPINFOHEADER	*pbi;
	byte	*pbyte = new byte[cy*cbRow+sizeof(BITMAPINFOHEADER)];

	pbi = (BITMAPINFOHEADER*)pbyte;
	pbyte = (byte*)(pbi+1);

	ASSERT( *ppbi == 0 );
	ASSERT( *ppbyte == 0 );

	*ppbi = pbi;
	*ppbyte = pbyte;

	ZeroMemory( pbi, sizeof(BITMAPINFOHEADER) );
	pbi->biSize = sizeof(BITMAPINFOHEADER);
	pbi->biBitCount = 24;
	pbi->biHeight = cy;
	pbi->biPlanes = 1;
	pbi->biWidth = cx;
	pbi->biSizeImage = cy*cbRow;

	IUnknown	*punkCC = 0;
	image->GetColorConvertor( &punkCC );
	ASSERT( punkCC );
	IColorConvertorPtr	ptrCC( punkCC );
	punkCC->Release();

	int	colors = image.GetColorsCount();
	color	**ppColors = new color*[colors];

	if( bInitMask )
	{
		ASSERT(m_pmask == 0 );
		m_pmask = new byte[cx*cy];
	}


	int	row, c;
	for( row = 0; row < cy; row++ )
	{
		for( c = 0; c < colors; c++ )
		{
			ppColors[c] = image.GetRow( row, c );
		}
		ptrCC->ConvertImageToDIB( ppColors, pbyte+(cy-row-1)*cbRow, cx, true );

		if( bInitMask )
		{
			byte	*pmask = image.GetRowMask( row );
			memcpy( m_pmask+(cy-1-row)*cx, pmask, cx );
		}
	}

	delete ppColors;
}

bool CActionCompareImage::DoSetCursor( CPoint pt )
{
	if( m_frame.DoSetCursor( pt, m_location ) )
		return true;
	return CInteractiveActionBase::DoSetCursor( pt );
}

bool CActionCompareImage::DoUpdateSettings()
{
	m_nAngle = ::GetValueDouble( GetAppUnknown(), "Compare", "Angle", m_nAngle );
	m_nWidth = ::GetValueInt( GetAppUnknown(), "Compare", "Width", m_nWidth );
	m_nHeight = ::GetValueInt( GetAppUnknown(), "Compare", "Height", m_nHeight );
	m_nXPos = ::GetValueInt( GetAppUnknown(), "Compare", "XPos", m_nXPos );
	m_nYPos = ::GetValueInt( GetAppUnknown(), "Compare", "YPos", m_nYPos );	
	m_nOverlayMode = ::GetValueInt( GetAppUnknown(), "\\Compare", "OverlayMode", m_nOverlayMode );
	m_nAngleStep = ::GetValueDouble( GetAppUnknown(), "\\Compare", "AngleStep", 1 );
	m_bEnableScroll = ::GetValueInt( GetAppUnknown(), "\\IntelliMouse", "Enable key scroll", 0);
	::SetValue(GetAppUnknown(), "\\IntelliMouse", "Enable key scroll", 0L);

	if(m_punkTarget)
	{
		IViewSitePtr vs(m_punkTarget);
		if(vs)
		{
			vs->InitView(0);
		}
	}

	if( m_bSingleImage )
		m_nOverlayMode = 0;

	InvalidateIntersect();
	MakeBitmap();
	MakeContours();
	InvalidateIntersect( true );

	return true;
}

bool CActionCompareImage::DoLButtonDown( CPoint point )
{
	return StartTracking( point );
}

bool CActionCompareImage::DoStartTracking( CPoint point )
{
	m_location = m_frame.HitTest( point );
	if( m_location != CResizingFrame::None )
	{
		m_frame.RecalcChanges( point, m_location );
		return true;
	}
	m_pointStart = point;
	m_pointStart.x -= m_nXPos;
	m_pointStart.y -= m_nYPos;
	return true;
}
bool CActionCompareImage::DoTrack( CPoint point )
{
	if( m_location != CResizingFrame::None )
	{
		m_frame.Redraw();
		m_frame.RecalcChanges( point, m_location );

		m_nWidth = m_frame.GetZoomX()*100;
		m_nHeight = m_frame.GetZoomY()*100;
		m_nAngle = -m_frame.GetAngle()*180./PI;

		DoSetCursor( point );

		MakeBitmap();
		MakeContours( false );
		InvalidateIntersect( true );


		::SetValue( GetAppUnknown(), "Compare", "Angle", m_nAngle );
		::SetValue( GetAppUnknown(), "Compare", "Width", m_nWidth );
		::SetValue( GetAppUnknown(), "Compare", "Height", m_nHeight );
		::SetValue( GetAppUnknown(), "Compare", "XPos", m_nXPos );
		::SetValue( GetAppUnknown(), "Compare", "YPos", m_nYPos );	

		UpdatePropertyPages();

		

		return true;
	}

	if( m_nXPos == point.x-m_pointStart.x &&
		m_nYPos == point.y-m_pointStart.y )
		return true;
	m_nXPos = point.x-m_pointStart.x;
	m_nYPos = point.y-m_pointStart.y;

	::SetValue( GetAppUnknown(), "Compare", "XPos", m_nXPos );
	::SetValue( GetAppUnknown(), "Compare", "YPos", m_nYPos );	


	UpdatePropertyPages();

	InvalidateIntersect();
	MakeBitmap();
	MakeContours();
	InvalidateIntersect( true );

	return true;
}

bool CActionCompareImage::DoFinishTracking( CPoint point )
{
	if( m_location != CResizingFrame::None )
	{
		m_frame.Redraw();
		m_frame.RecalcChanges( point, m_location );

		m_nWidth = m_frame.GetZoomX()*100;
		m_nHeight = m_frame.GetZoomY()*100;
		m_nAngle = -m_frame.GetAngle()*180./PI;

		m_location = CResizingFrame::None;

		MakeBitmap();
		MakeContours( false );
		InvalidateIntersect();


		::SetValue( GetAppUnknown(), "Compare", "Angle", m_nAngle );
		::SetValue( GetAppUnknown(), "Compare", "Width", m_nWidth );
		::SetValue( GetAppUnknown(), "Compare", "Height", m_nHeight );
		::SetValue( GetAppUnknown(), "Compare", "XPos", m_nXPos );
		::SetValue( GetAppUnknown(), "Compare", "YPos", m_nYPos );	


		UpdatePropertyPages();

		return true;
	}
	return false;
}

void CActionCompareImage::InvalidateIntersect( bool bUpdate )
{
	m_frame.Redraw( false, bUpdate );
	CRect	rect = ::ConvertToWindow( m_punkTarget, m_rectDIB );
	::InvalidateRect( m_hwndTarget, &rect, false );
}

bool CActionCompareImage::DoChar( UINT nChar )
{
	if( nChar == VK_RETURN )
	{
		Finalize();
		return true;
	}
	else if( nChar == VK_ESCAPE )
	{
		m_bManualTerminated = true;
		Terminate();
		LeaveMode();
		return true;
	}
	int		n = m_nOverlayMode;

	switch( nChar )
	{
	case '=':n = 0;break;
	case '+':n = 1;break;
	case '-':n = 2;break;
	default:
		return false;
	};

	
	if( m_nOverlayMode != n )
	{
		m_nOverlayMode = n;
		::SetValue( GetAppUnknown(), "\\Compare", "OverlayMode", (long)m_nOverlayMode );
		UpdatePropertyPages();

		InvalidateIntersect();
		MakeBitmap();
		MakeContours();
		InvalidateIntersect( true );
	}

	return true;
}

void CActionCompareImage::ResizeRotate()
{
	if( !m_pbi2 )
		return;

	InitBackground();

	m_nCurrentWidth	= m_nWidth;
	m_nCurrentHeight = m_nHeight;
	m_nCurrentAngle = m_nAngle;


	int	cx, cy, cx4;
	int	cxn, cyn, cx4n;

	if( !m_pbiNoRotate )
	{
		m_pbiNoRotate = (BITMAPINFOHEADER*)new byte[m_pbi2->biSizeImage+m_pbi2->biSize];
		memcpy( m_pbiNoRotate, m_pbi2, m_pbi2->biSizeImage+m_pbi2->biSize );
		m_pdibNoRotate = (byte*)(m_pbiNoRotate+1);

		ASSERT(!m_pmaskNoRotate);
		ASSERT(m_pmask);
		DWORD	dwSize = m_pbi2->biHeight*m_pbi2->biWidth;
		m_pmaskNoRotate = new byte[dwSize];
		memcpy( m_pmaskNoRotate, m_pmask, dwSize );
	}


	cx = m_pbiNoRotate->biWidth;
	cy = m_pbiNoRotate->biHeight;
	cx4 = (cx*3+3)/4*4;

	double	fZoomX = m_nWidth/100., fZoomY = m_nHeight/100.;
	double	fAngle = m_nAngle/180.*PI;


	double	point1x, point2x, point3x, point4x;
	double	point1y, point2y, point3y, point4y;
	CRect	rect = m_imageOther.GetRect();

	point1x = ( rect.left*::cos( fAngle )*fZoomX+rect.top*::sin( fAngle )*fZoomY );
	point1y = ( -rect.left*::sin( fAngle )*fZoomX+rect.top*::cos( fAngle )*fZoomY );
	point2x = ( rect.left*::cos( fAngle )*fZoomX+rect.bottom*::sin( fAngle )*fZoomY );
	point2y = ( -rect.left*::sin( fAngle )*fZoomX+rect.bottom*::cos( fAngle )*fZoomY );
	point3x = ( rect.right*::cos( fAngle )*fZoomX+rect.bottom*::sin( fAngle )*fZoomY );
	point3y = ( -rect.right*::sin( fAngle )*fZoomX+rect.bottom*::cos( fAngle )*fZoomY );
	point4x = ( rect.right*::cos( fAngle )*fZoomX+rect.top*::sin( fAngle )*fZoomY );
	point4y = ( -rect.right*::sin( fAngle )*fZoomX+rect.top*::cos( fAngle )*fZoomY );

	cxn = (int)floor( max( max( point1x, point2x ), max( point3x, point4x ) ) )-(int)ceil( min( min( point1x, point2x ), min( point3x, point4x ) ) );
	cyn = (int)floor( max( max( point1y, point2y ), max( point3y, point4y ) ) )-(int)ceil( min( min( point1y, point2y ), min( point3y, point4y ) ) );
	cx4n = (cxn*3+3)/4*4;

	if( cxn != m_pbi2->biWidth ||
		cyn != m_pbi2->biHeight )
	{
		delete m_pbi2;
		delete m_pmask;
		m_pbi2 = 0;
		m_pdib2 = 0;
		m_pmask = 0;

		m_pbi2 = (BITMAPINFOHEADER*)new byte[cx4n*cyn+sizeof( BITMAPINFOHEADER)];
		m_pmask = new byte[cxn*cyn];
		ZeroMemory( m_pbi2, sizeof( BITMAPINFOHEADER ) );
		m_pbi2->biSize = sizeof( BITMAPINFOHEADER );
		m_pbi2->biSizeImage = cyn*cx4n;
		m_pbi2->biWidth = cxn;
		m_pbi2->biHeight = cyn;
		m_pbi2->biBitCount = 24;
		m_pbi2->biPlanes = 1;

		m_pdib2 = (byte*)(m_pbi2+1);

		CPoint	ptOldCenter = m_imageOther.GetRect().CenterPoint();
		m_rectOther.left = ptOldCenter.x - cxn/2;
		m_rectOther.right = m_rectOther.left+cxn;
		m_rectOther.top = ptOldCenter.y - cyn/2;
		m_rectOther.bottom = m_rectOther.top+cyn;
	}





	double	a11, a12, a13,
			a21, a22, a23;

	int		x, x3, y;
	int		xs, ys;


	CPoint	ptCenter( cx/2, cy/2 );
	CPoint	ptCenterNew( cxn/2, cyn/2 );

	a11 = cos( -fAngle )/(max(fZoomX, 1e-10));
	a12 = sin( -fAngle )/(max(fZoomX, 1e-10));
	a13 = -a11*ptCenterNew.x-a12*ptCenterNew.y+ptCenter.x;

	a21 = -sin( -fAngle )/(max(fZoomY, 1e-10));
	a22 = cos( -fAngle )/(max(fZoomY, 1e-10));
	a23 = -a21*ptCenterNew.x-a22*ptCenterNew.y+ptCenter.y;

	byte	*pbyteCurrentRow = 0;
	byte	*pbyteSourceRow = 0;
	byte	*pmask = 0;

	_ptr_t<int> smartSrc_xx(cxn);
	int* src_xx=smartSrc_xx.ptr;
	_ptr_t<int> smartSrc_yx(cxn);
	int* src_yx=smartSrc_yx.ptr;

	for( x = 0; x<cxn; x++)
	{
		src_xx[x]=a11*x*256;
		src_yx[x]=a21*x*256;
	}
	
	for( y = 0; y<cyn; y++ )
	{
		pbyteCurrentRow = m_pdib2+y*cx4n;
		pmask = m_pmask+y*cxn;
		int xs256 = int((a12*y+a13)*256 );
		int ys256 = int((a22*y+a23)*256 ); 
		for( x = 0, x3 = 0; x<cxn; x++, x3+=3 )
		{
			xs = (xs256 + src_xx[x])/256;
			ys = (ys256 + src_yx[x])/256;

			if( xs < 0 || ys < 0 || xs >= m_pbiNoRotate->biWidth ||ys >= m_pbiNoRotate->biHeight )
			{
				pbyteCurrentRow[x3] = 0;
				pbyteCurrentRow[x3+1] = 0;
				pbyteCurrentRow[x3+2] = 0;
				pmask[x] = 0;
			}
			else
			{
				pbyteSourceRow = m_pdibNoRotate+ys*cx4;

				register int xs3=xs*3;
				pbyteCurrentRow[x3] = pbyteSourceRow[xs3];
				pbyteCurrentRow[x3+1] = pbyteSourceRow[xs3+1];
				pbyteCurrentRow[x3+2] = pbyteSourceRow[xs3+2];
				pmask[x] = m_pmaskNoRotate[ys*cx+xs];
			}
		}
	}
}


void CActionCompareImage::MakeContours( bool bUpdateFrame  )
{
	CRect	rect_i = m_imageOther.GetRect();

	double	fZoomX = m_nWidth/100., fZoomY = m_nHeight/100.;
	double	fAngle = m_nAngle/180.*PI;

	int	xo = rect_i.Width()/2+rect_i.left;
	int	yo = rect_i.Height()/2+rect_i.top;
	int	xn = m_rectOther.CenterPoint().x+m_nXPos;
	int	yn = m_rectOther.CenterPoint().y+m_nYPos;

	double a11 = cos( fAngle )*fZoomX;
	double a12 = -sin( fAngle )*fZoomY;
	double a13 = -a11*xo-a12*yo+xn;

	double a21 = sin( fAngle )*fZoomX;
	double a22 = cos( fAngle )*fZoomY;
	double a23 = -a21*xo-a22*yo+yn;

	m_contours.clear();
	int	c = 0;
	

	if( m_imageOther == 0 )
		return;
	m_imageOther->GetContoursCount( &c );
	for( int i = 0; i < c; i++ )
	{
		Contour	*pcs = 0;
		m_imageOther->GetContour( i, &pcs );
		Contour	*pcd = ContourCreate();

		for( int p = 0; p < pcs->nContourSize; p++ )
		{
			int	xs = pcs->ppoints[p].x;
			int	ys = pcs->ppoints[p].y;
			int xs256 = int((a11*xs+a12*ys+a13)*256 );
			int ys256 = int((a21*xs+a22*ys+a23)*256 );
			::ContourAddPoint( pcd, xs256/256, ys256/256, false );
		}
		m_contours.insert_before( pcd, 0 );
	}

	//m_frame.SetRect( m_rectOther );

	if( bUpdateFrame )
	{
		CRect	rect_f = rect_i;
		rect_f.OffsetRect( m_nXPos, m_nYPos );

		m_frame.SetRect( rect_f );
		m_frame.SetAngle( fAngle );
		m_frame.SetZooms( fZoomX, fZoomY );
	}
	//m_frame.Redraw();
}

bool CActionCompareImage::DoVirtKey( UINT nVirtKey )
{
	bool	bChange = false;

	/*if( nVirtKey == VK_SPACE )
	{
		FindProper
		SendMessage( 
	}*/

	if( GetAsyncKeyState( VK_SHIFT ) & 0x8000 )
	{
		if( nVirtKey == VK_LEFT || nVirtKey == VK_RIGHT )
		{
			double nOldW = m_nWidth;
			if( nVirtKey == VK_LEFT )m_nWidth=max( m_nWidth-1, 1 );
			if( nVirtKey == VK_RIGHT )m_nWidth=min( m_nWidth+1, 200 );

			if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
//				m_nHeight = m_nWidth*m_nCurrentHeight/double( m_nCurrentWidth );
				if( m_nWidth <= m_nHeight)
					m_nHeight = m_nHeight*m_nWidth/nOldW;
				else
				{
					double nOldH = m_nHeight;
					if( nVirtKey == VK_LEFT )m_nHeight=max( m_nHeight-1, 1 );
					if( nVirtKey == VK_RIGHT )m_nHeight=min( m_nHeight+1, 200 );

					if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
						m_nWidth = m_nWidth*m_nHeight/nOldH;

				}
			}

			
		}
		if( nVirtKey == VK_UP || nVirtKey == VK_DOWN )
		{
			double nOldH = m_nHeight;
			if( nVirtKey == VK_DOWN )m_nHeight=max( m_nHeight-1, 1 );
			if( nVirtKey == VK_UP )m_nHeight=min( m_nHeight+1, 200 );

			if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			{
//				m_nWidth = m_nHeight*(m_nCurrentWidth/double( m_nCurrentHeight ));
				if( m_nWidth >= m_nHeight )
					m_nWidth = m_nWidth*m_nHeight/nOldH;
				else
				{
					double nOldW = m_nWidth;
					if( nVirtKey == VK_DOWN )m_nWidth=max( m_nWidth-1, 1 );
					if( nVirtKey == VK_UP )m_nWidth=min( m_nWidth+1, 200 );

					if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
						m_nHeight = m_nHeight*m_nWidth/nOldW;
				}
			}
		}

		m_nHeight= max( 1, min( 200, m_nHeight ) );
		m_nWidth= max( 1, min( 200, m_nWidth ) );
	}
	else
	{
		int	nDelta = 1;
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			nDelta = 10;
		double	fDeltaA = m_nAngleStep;
		if( GetAsyncKeyState( VK_CONTROL ) & 0x8000 )
			fDeltaA = m_nAngleStep*10;

		if( nVirtKey == VK_LEFT )
			m_nXPos -=nDelta;
		else if( nVirtKey == VK_RIGHT )
			m_nXPos +=nDelta;
		else if( nVirtKey == VK_UP )
			m_nYPos -=nDelta;
		else if( nVirtKey == VK_DOWN )
			m_nYPos +=nDelta;
		else if( nVirtKey == VK_PRIOR )
			m_nAngle -= fDeltaA;
		else if( nVirtKey == VK_NEXT )
			m_nAngle += fDeltaA;

		while( m_nAngle >= 360 )m_nAngle-=360;
		while( m_nAngle < 0 )m_nAngle+=360;
	}

	if( m_nAngle != m_nCurrentAngle ||
		m_nHeight != m_nCurrentHeight ||
		m_nWidth != m_nCurrentWidth ||
		m_nXPos != m_nCurrentXPos ||
		m_nYPos != m_nCurrentYPos )
	{
		::SetValue( GetAppUnknown(), "Compare", "Angle", m_nAngle );
		::SetValue( GetAppUnknown(), "Compare", "Width", m_nWidth );
		::SetValue( GetAppUnknown(), "Compare", "Height", m_nHeight );
		::SetValue( GetAppUnknown(), "Compare", "XPos", m_nXPos );
		::SetValue( GetAppUnknown(), "Compare", "YPos", m_nYPos );	

		UpdatePropertyPages();
		InvalidateIntersect();
		MakeBitmap();
		MakeContours();
		InvalidateIntersect( true );

		m_nCurrentXPos = m_nXPos;
		m_nCurrentYPos = m_nYPos;
	}

	return true;
}

bool CActionCompareImage::DoLButtonDblClick( CPoint pt )
{
	Finalize();
	return true;
}

bool CActionCompareImage::DoUndo()
{
	m_changes.DoUndo( m_punkTarget );
	return true;
}

bool CActionCompareImage::DoRedo()
{
	m_changes.DoRedo( m_punkTarget );
	return true;
}


void CActionCompareImage::InitBackground()
{
	m_crBack = ::GetValueColor( GetAppUnknown(), "\\Image", "Background", RGB(255,255,255) );
	m_r = GetRValue( m_crBack );
	m_g = GetGValue( m_crBack );
	m_b = GetBValue( m_crBack );
}

BOOL CActionCompareImage::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_PAINT )
	{
		double	fZoom;
		POINT	ptScroll;
		CRect	rectAll = NORECT;
		CSize	size;

		{
			IScrollZoomSitePtr	ptrSZ( m_punkTarget );
			ptrSZ->GetZoom( &fZoom );
			ptrSZ->GetScrollPos( &ptScroll );
			ptrSZ->GetClientSize( &size );
		}

		rectAll.right = size.cx;
		rectAll.bottom = size.cy;


		CWnd	*pwnd = ::GetWindowFromUnknown( m_punkTarget );
		CRect	rectInvalid;
		pwnd->GetUpdateRect( &rectInvalid );
		rectInvalid.right = min( rectInvalid.right, (int)ceil( rectAll.right*fZoom-ptScroll.x ) );
		rectInvalid.bottom = min( rectInvalid.bottom, (int)ceil( rectAll.bottom*fZoom-ptScroll.y ) );


		CPaintDC	dc( pwnd );

		BITMAPINFOHEADER	bi;
		ZeroMemory( &bi, sizeof( bi ) );
		bi.biSize = sizeof( bi );
		bi.biWidth = rectInvalid.Width();
		bi.biHeight = rectInvalid.Height();
		bi.biBitCount = 24;
		bi.biPlanes = 1;

		byte	*pbytes = new byte[ (bi.biWidth*3+3)/4*4*bi.biHeight ];


		CRect	rectTop, rectBottom, rectLeft, rectRight;
		rectTop.left = rectAll.left;
		rectTop.right = rectAll.right;
		rectTop.top = rectAll.top;
		rectTop.bottom = m_rectDIB.top+1;

		rectBottom.left = rectAll.left;
		rectBottom.right = rectAll.right;
		rectBottom.top = m_rectDIB.bottom-1;
		rectBottom.bottom = rectAll.bottom;

		rectLeft.left = rectAll.left;
		rectLeft.right = m_rectDIB.left+1;
		rectLeft.top = rectTop.bottom-1;
		rectLeft.bottom = rectBottom.top+1;

		rectRight.left = m_rectDIB.right-1;
		rectRight.right = rectAll.right;
		rectRight.top = rectTop.bottom-1;
		rectRight.bottom = rectBottom.top+1;

#ifdef _DEBUG
		try{
#endif //_DEBUG		

		if( m_pbi1 )
		{
			StrecthDIB( m_pbi1, m_pdib1, rectTop, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
			StrecthDIB( m_pbi1, m_pdib1, rectLeft, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
			StrecthDIB( m_pbi1, m_pdib1, rectRight, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
			StrecthDIB( m_pbi1, m_pdib1, rectBottom, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
		}
		else
		{
			COLORREF	cr = m_crBack;
			FillDIB( cr, rectTop, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
			FillDIB( cr, rectLeft, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
			FillDIB( cr, rectRight, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
			FillDIB( cr, rectBottom, CPoint( 0, 0 ),
				&bi, pbytes, rectInvalid, fZoom, ptScroll );
		}

		
		StrecthDIB( m_pbiIntersect, m_pdibIntersect, m_rectDIB, m_rectDIB.TopLeft(),
			&bi, pbytes, rectInvalid, fZoom, ptScroll );
#ifdef _DEBUG
		}
		catch( ... )
		{
		}
#endif //_DEBUG		

		::SetDIBitsToDevice( dc, rectInvalid.left, rectInvalid.top, bi.biWidth, bi.biHeight, 
			0, 0, 0, bi.biHeight, pbytes, (BITMAPINFO*)&bi, DIB_RGB_COLORS );

		dc.SelectStockObject( WHITE_PEN );
		dc.SelectStockObject( NULL_BRUSH );
		dc.SetROP2( R2_COPYPEN );
		dc.SetBoundsRect( rectAll, DCB_ENABLE );
		//dc.Rectangle( ConvertToWindow( m_punkTarget, m_rectDIB ) );
		for (TPOS lpos = m_contours.head(); lpos; lpos = m_contours.next(lpos))
			::ContourDraw( dc, m_contours.get( lpos ), m_punkTarget, 0 );
		m_frame.Draw( dc );



		delete pbytes;

		*pResult = 1;

		return true;
	}
	if( message == WM_TIMER )
	{
		return true;
	}

	if( CInteractiveActionBase::OnWndMsg( message, wParam, lParam, pResult ) )
		return true;

	return false;
}

void CActionCompareImage::DoActivateObjects( IUnknown *punkObject )
{
	m_bManualTerminated = true;
	Terminate();
	LeaveMode();
}


//////////////////////////////////////////////////////////////////////
//CActionAphineByPoints implementation
class COtherViewHandler : public CCmdTargetEx,
						public CMouseImpl
{
	DECLARE_INTERFACE_MAP();
public:
	COtherViewHandler( CActionAphineByPoints* p )
	{
		m_pAction = p;
		m_punkDrawTarget = 0;
		ForceEnabled( true );
	}
	void AttachTarget( IUnknown *punk )
	{		m_punkDrawTarget = punk;	}
protected:
	BEGIN_INTERFACE_PART(Draw, IDrawObject)
		com_call Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache );
		com_call GetRect( RECT *prect );
		com_call SetTargetWindow( IUnknown *punkTarget );
	END_INTERFACE_PART(Draw)

	BEGIN_INTERFACE_PART(List, IMsgListener)
		com_call OnMessage( MSG *pmsg, LRESULT *plReturn );
	END_INTERFACE_PART(List)

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonDblClick( CPoint pt );

	


	//mouseimpl
	virtual IUnknown *GetTarget()
	{		return m_punkDrawTarget;	}

	CActionAphineByPoints	*m_pAction;
	IUnknown				*m_punkDrawTarget;

};

IMPLEMENT_UNKNOWN(COtherViewHandler, Draw)
IMPLEMENT_UNKNOWN(COtherViewHandler, List)

BEGIN_INTERFACE_MAP(COtherViewHandler, CCmdTargetEx)
	INTERFACE_PART(COtherViewHandler, IID_IDrawObject, Draw)
	INTERFACE_PART(COtherViewHandler, IID_IMsgListener, List)
END_INTERFACE_MAP();

HRESULT COtherViewHandler::XDraw::Paint( HDC hDC, RECT rectDraw, IUnknown *punkTarget, BITMAPINFOHEADER *pbiCache, BYTE *pdibCache )
{
	METHOD_PROLOGUE_EX(COtherViewHandler, Draw);
	pThis->m_pAction->DrawPoints( hDC, punkTarget, true );
	return S_OK;
}

HRESULT COtherViewHandler::XDraw::GetRect( RECT *prect )
{
	METHOD_PROLOGUE_EX(COtherViewHandler, Draw);
	IScrollZoomSitePtr	ptrSite( pThis->m_punkDrawTarget );
	_size	size;
	ptrSite->GetClientSize( &size );

	prect->left = prect->top = 0;
	prect->right = size.cx;
	prect->bottom = size.cy;


	return S_OK;
}

HRESULT COtherViewHandler::XDraw::SetTargetWindow( IUnknown *punkTarget )
{
	METHOD_PROLOGUE_EX(COtherViewHandler, Draw);
	return S_OK;
}

HRESULT COtherViewHandler::XList::OnMessage( MSG *pmsg, LRESULT *plReturn )
{
	METHOD_PROLOGUE_EX(COtherViewHandler, List);

	*plReturn = 0;
	pThis->CMouseImpl::OnWndMsg( pmsg->message, pmsg->wParam, pmsg->lParam, plReturn );

	if( pmsg->message == WM_PAINT||
		pmsg->message == WM_ERASEBKGND )
		return S_OK;

	if( pmsg->message == WM_MOUSEACTIVATE )
	{
		*plReturn = 0;
		return S_FALSE;
	}

	if( (pmsg->message >= WM_MOUSEFIRST && pmsg->message <= WM_MOUSELAST) ||
		pmsg->message == WM_SETFOCUS  )
	{
		return S_FALSE;
	}

	return S_OK;
}

bool COtherViewHandler::DoLButtonDown( CPoint pt )
{
	m_pAction->AddPoint( pt, true );
	return true;
}

bool COtherViewHandler::DoLButtonDblClick( CPoint pt )
{
	if( m_pAction->m_nPointsCountOther == m_pAction->m_nPointsCount &&
		m_pAction->m_nPointsCountOther > 0 )
		m_pAction->Finalize();
	else
	{
		AfxMessageBox( IDS_DIFFERENT_POINTS_COUNT );
	}
	return true;
}



CActionAphineByPoints::CActionAphineByPoints()
{
	m_nPointsCount = m_nPointsCountOther = 0;
}

CActionAphineByPoints::~CActionAphineByPoints()
{
}


bool CalcParamsPoints( CVector2D *points_from, CVector2D *points_to,
                      int n_points,
                      CAphineParams &params)
{
        if(n_points<1 || n_points>3) return(false);
        params.center=points_from[0];
        params.center_new=points_to[0];

        CVector2D dir1_from=CVector2D(1,0), dir1_to=CVector2D(1,0);
        if(n_points>=2)
        {
                dir1_from=points_from[1]-points_from[0];
                dir1_to=points_to[1]-points_to[0];
        }

        CVector2D dir2_from=CVector2D(dir1_from.y,-dir1_from.x);
        CVector2D dir2_to=CVector2D(dir1_to.y,-dir1_to.x);
        if(n_points>=3)
        {
                dir2_from=points_from[2]-points_from[0];
                dir2_to=points_to[2]-points_to[0];
        }

        CMatrix2x2 from_basis=
                CMatrix2x2(dir1_from.x, dir2_from.x, dir1_from.y, dir2_from.y);
        CMatrix2x2 to_basis=
                CMatrix2x2(dir1_to.x, dir2_to.x, dir1_to.y, dir2_to.y);

        double d_from = from_basis.a11*from_basis.a22-from_basis.a12*from_basis.a21;
        double d_to = to_basis.a11*to_basis.a22-to_basis.a12*to_basis.a21;

        if(fabs( d_from) <1e-20 || fabs( d_to ) <1e-20) return(false);

        params.a=to_basis * ~from_basis; //добавить проверку на вырожденность

        return(true);
}

bool CActionAphineByPoints::Invoke()
{
	LeaveMode();

	if( m_nPointsCount != m_nPointsCountOther )
	{
		AfxMessageBox( IDS_DIFFERENT_COUNT_OF_POINTS );
		return false;
	}
	CVector2D points_from[MAX_POINTS],
				points_to[MAX_POINTS];

	/*len_top to paul
	m_points[0].x = 210;	m_points[0].y = 138;
	m_points[1].x = 197;	m_points[1].y = 102;
	m_points[2].x = 227;	m_points[2].y = 103;

	m_pointsOther[0].x = 412;	m_pointsOther[0].y = 338;
	m_pointsOther[1].x = 323;	m_pointsOther[1].y = 229;
	m_pointsOther[2].x = 450;	m_pointsOther[2].y = 193;*/

	for( int n = 0; n < m_nPointsCount; n++ )
	{
		points_from[n] = CVector2D( m_points[n].x, m_points[n].y );
		points_to[n] = CVector2D( m_pointsOther[n].x, m_pointsOther[n].y );
	}

	CAphineParams	params;
	if( !CalcParamsPoints( points_from, points_to, m_nPointsCount, params ) )
	{
		AfxMessageBox( IDS_CANT_TRANSPHORM );
		return false;
	}

	CPoint	point;
	CSize	size;
	m_image->GetSize( (int*)&size.cx, (int*)&size.cy );
	m_image->GetOffset( &point );


	//expand it
	CRect	rect( point, size );
	
	CVector2D	corners[4], rects[4], pt;
	corners[0].x = rect.left;
	corners[0].y = rect.top;
	corners[1].x = rect.right;
	corners[1].y = rect.top;
	corners[2].x = rect.right;
	corners[2].y = rect.bottom;
	corners[3].x = rect.left;
	corners[3].y = rect.bottom;		   

	double	minX, minY, maxX, maxY;

	for( n = 0; n < 4; n++ )
	{
		corners[n] = params*corners[n];

		if( n )
		{
			minX = min( minX, corners[n].x );
			maxX = max( maxX, corners[n].x );
			minY = min( minY, corners[n].y );
			maxY = max( maxY, corners[n].y );
		}
		else
		{
			minX = corners[n].x;
			maxX = corners[n].x;
			minY = corners[n].y;
			maxY = corners[n].y;
		}
	}
	
	rects[0].x = 0;
	rects[0].y = 0;
	rects[1].x = maxX;
	rects[1].y = 0;
	rects[2].x = maxX;
	rects[2].y = maxY;
	rects[3].x = 0;
	rects[3].y = maxY;

	if( minX < 0 )maxX+=minX;
	if( minY < 0 )maxY+=minY;

	for( int n1 = 0; n1 < 4; n1++ )
		for( int n2 = 0; n2 < 4; n2++ )
			if( CalcIntersect( corners[n1], corners[n1?(n1-1):3], rects[n2], rects[n2?(n2-1):3], pt ) )
			{
				maxX = max( maxX, pt.x );
				maxY = max( maxY, pt.y );
			}
	rect.left = rect.top = 0;
	rect.right = int( maxX );
	rect.bottom = int( maxY );


	if( rect.right*rect.bottom >= 3000*3000 )
	{
		CString	str;
		str.Format( IDS_VERYBIGIMAGE, rect.right, rect.bottom );

		if( AfxMessageBox( str, MB_YESNO ) != IDYES )
			return false;
	}


	CImageWrp image;
	if (!image.CreateCompatibleImage( CImageWrp(m_image), false, rect))
		return(false);
	image->InitRowMasks();


	StartNotification( 101, 1, 1 );
	atExecute( m_image, IImage2Ptr(image), params, 
		_callback_rotate, (long)(CLongOperationImpl*)this);
	FinishNotification();

	//fill row masks
	{
		_convert	convert;

		convert.init( image );
		convert.set_rgb( ::GetValueColor(GetAppUnknown(), "\\Image", "Background", RGB(255,255,255) ) );

		int	x, c, c_max = convert.get_panes();
		color	*p;
		byte	*pmask;
		color	col;

		for( int i = 0; i < rect.bottom; i++ ) 
		{
			image->GetRowMask( i, &pmask );

			for( c = 0; c < c_max; c++ )
			{
				col = convert.get_color( c );
				image->GetRow( i, c, &p );
				for( x = 0; x < rect.right; x++ )
					if( !pmask[x] )p[x] = col;
			}

			memset( pmask, 0xff, rect.right );
		}
	}

	//set to document

	IUnknown	*pdoc = 0;
	IViewPtr	ptrView( m_punkTarget );
	ptrView->GetDocument( &pdoc );

	CopyCalibration( image, m_ptrOtherImage );
	m_lTargetKey = ::GetObjectKey( pdoc );
	pdoc->Release();

	AddArgumentToFilter( m_image, false );
	AddArgumentToFilter( image, true );

	{
		INamedDataObject2Ptr	ptr1( image ), ptr2( m_image );

		GUID	baseKey;
		ptr2->GetBaseKey( &baseKey );
		ptr1->SetBaseKey( &baseKey );


		SetObjectName( image, CreateUniqueName( pdoc, GetObjectName( m_image ) ) );
	}

	m_changes.SetToDocData( pdoc, image );
	if( GetValueInt( GetAppUnknown(), "\\General", "DeleteFilterArgs", 0 ) )
		m_changes.RemoveFromDocData( pdoc, m_image, false );

	m_ptrOtherImage = 0;

	return true;
}

//extended UI
bool CActionAphineByPoints::IsAvaible()
{
	return CheckInterface( m_punkTarget, IID_IImageView );
}

bool CActionAphineByPoints::Initialize()
{
	//1. find second view - get it from splitter
	IViewSitePtr	ptrView( m_punkTarget );

	if( ptrView == 0 )
	{
		AfxMessageBox( IDS_INVALID_TARGET_WINDOW );
		return false;
	}
	IUnknown	*punkFrame = 0;
	ptrView->GetFrameWindow( &punkFrame );

	if( !punkFrame )
	{
		AfxMessageBox( IDS_CANT_GET_FRAME );
		return false;
	}

	IFrameWindowPtr	ptrFrameWindow( punkFrame );
	punkFrame->Release();

	if( ptrFrameWindow == 0 )
	{
		AfxMessageBox( IDS_WRONG_FRAME );
		return false;
	}

	IUnknown	*punkSplitter = 0;
	ptrFrameWindow->GetSplitter( &punkSplitter );

	if( !punkSplitter )
	{
		AfxMessageBox( IDS_NO_SPLITTER );
		return false;
	}

	ISplitterWindowPtr	ptrSplitter( punkSplitter );
	punkSplitter->Release();

	if( ptrSplitter == 0 )
	{
		AfxMessageBox( IDS_WRONG_SPLITTER );
		return false;
	}

	int	nRows, nCols;
	int	nColActive, nRowActive;

	ptrSplitter->GetRowColCount( &nRows, &nCols );
	ptrSplitter->GetActivePane( &nRowActive, &nColActive );
	
	int	row, col;

	for( row = 0; row < nRows; row++ )
		for( col = 0; col < nCols; col++ )
		{
			if( nRowActive == row && nColActive == col )
				continue;	//skip current view
			IUnknown	*punkSplitterView = 0;
			ptrSplitter->GetViewRowCol( row, col, &punkSplitterView );

			if( !punkSplitterView )
				continue;

			IImageViewPtr	ptrImageView( punkSplitterView );
			punkSplitterView->Release();

			if( ptrImageView == 0 )
				continue;

			if( m_ptrOtherView != 0 )
			{
				AfxMessageBox( IDS_MORE2VIEW );
				return false;
			}
			m_ptrOtherView = ptrImageView;
		}
	

	if( m_ptrOtherView == 0 )
	{
		AfxMessageBox( IDS_SPLIT );
		return false;
	}


	//2. create handler
	COtherViewHandler	*p = new COtherViewHandler( this );
	m_ptrOtherHandler = p->GetControllingUnknown();
	p->AttachTarget( m_ptrOtherView );
	p->GetControllingUnknown()->Release();

	//3. init images
	IUnknown	*punkImage = GetActiveObjectFromContext( m_punkTarget, szTypeImage );
	if( !punkImage )
	{
		AfxMessageBox( IDS_CANT_GET_ACTIVE );
		return false;
	}
	m_image = punkImage;
	punkImage->Release();

	punkImage = GetActiveObjectFromContext( m_ptrOtherView, szTypeImage );
	if( !punkImage )
	{
		AfxMessageBox( IDS_CANT_GET_SECOND );
		return false;
	}
	m_ptrOtherImage = punkImage;
	punkImage->Release();

	CSize	size;
	CPoint	offset;

	m_image->GetSize( (int*)&size.cx, (int*)&size.cy );
	m_image->GetOffset( &offset );
	//LockMovement( true, CRect( offset, size ) );

	m_ptrOtherImage->GetSize( (int*)&size.cx, (int*)&size.cy );
	m_ptrOtherImage->GetOffset( &offset );
	//p->LockMovement( true, CRect( offset, size ) );


	//4. register message handler in other view
	IWindow2Ptr	ptrOtherWindow = m_ptrOtherView;

	if( ptrOtherWindow == 0 )
	{
		AfxMessageBox( IDS_INVALID_SECOND );
		return false;
	}

	ptrOtherWindow->AttachMsgListener( m_ptrOtherHandler );
	ptrOtherWindow->AttachDrawing( m_ptrOtherHandler );

	return CInteractiveActionBase::Initialize();
}

void CActionAphineByPoints::Finalize()
{
	IWindow2Ptr	ptrOtherWindow = m_ptrOtherView;

	if( ptrOtherWindow != 0 && m_ptrOtherHandler != 0 )
	{
		ptrOtherWindow->DetachMsgListener( m_ptrOtherHandler );
		ptrOtherWindow->DetachDrawing( m_ptrOtherHandler );

		CWnd	*
		pwnd = GetWindowFromUnknown( m_punkTarget );
		if( pwnd )pwnd->Invalidate();
		pwnd = GetWindowFromUnknown( ptrOtherWindow );
		if( pwnd )pwnd->Invalidate();

		IScrollZoomSitePtr	ptrS( m_punkTarget );
		ptrS->SetZoom( GetZoom( m_ptrOtherView ) );
	}

	m_ptrOtherView = 0;
	m_ptrOtherHandler = 0;

	CInteractiveActionBase::Finalize();
}

bool CActionAphineByPoints::DoLButtonDown( CPoint pt )
{
	AddPoint( pt, false );
	return true;
}

bool CActionAphineByPoints::DoLButtonDblClick( CPoint pt )
{
	if( m_nPointsCountOther == m_nPointsCount &&
		m_nPointsCountOther > 0 )
		Finalize();
	else
		AfxMessageBox( IDS_DIFFERENT_POINTS_COUNT );
	return true;
}

void CActionAphineByPoints::Redraw( bool bOther )
{
	IUnknown	*punk = bOther?m_ptrOtherView:m_punkTarget;
	IWindowPtr		ptrWindow( punk );
	HWND	hwnd;
	ptrWindow->GetHandle( (HANDLE*)&hwnd );

/*	if( bOther )
		hwnd = ::GetWindow( hwnd, GW_CHILD );*/

	HDC	hdc = ::GetDC( hwnd );

	DrawPoints( hdc, punk, bOther );

	::ReleaseDC( hwnd, hdc );
}

void CActionAphineByPoints::_Draw( CDC *pdc )
{
	bool	bOwnDC = pdc == 0;
	if( bOwnDC )pdc = new CClientDC( GetWindowFromUnknown( m_punkTarget ) );

	DrawPoints( *pdc, m_punkTarget, false );

	if( bOwnDC )delete pdc;
}

void CActionAphineByPoints::DrawPoints( HDC hdc, IUnknown *punkTarget, bool bOther )
{
	int	delta = ::GetValueInt( ::GetAppUnknown(false), szManualMeasure, szMMCrossOffset, 5 );
	COLORREF	color = bOther?
		GetValueColor( GetAppUnknown(), "\\Colors", "AphinePassive", RGB( 255, 0, 0 ) ):
		GetValueColor( GetAppUnknown(), "\\Colors", "AphineActive", RGB( 255, 255, 0 ) );
	int	&c = bOther?m_nPointsCountOther:m_nPointsCount;
	CPoint	*p = bOther?m_pointsOther:m_points;


	HPEN	hpen = ::CreatePen( PS_SOLID, 2, color );
	HPEN	hOldPen = (HPEN)::SelectObject( hdc, hpen );
	HFONT	hfont = ::CreateFont( 3*delta+1, 0, 0, 0, FW_BOLD, 1, 0, 0, 0, 0, 0, 0, 0, "Tahoma" );
	HFONT	hOldFont = (HFONT)::SelectObject( hdc, hfont );

	::SetTextColor( hdc, color );
	::SetBkMode( hdc, TRANSPARENT );
	::SetROP2( hdc, R2_COPYPEN );

	char	sz[10];
	

	for( int i = 0; i < c; i++, p++ )
	{
		sprintf( sz, "%d", i+1 );
		CPoint	point = ConvertToWindow( punkTarget, *p );
		::MoveToEx( hdc, point.x-delta, point.y-delta, 0 );
		::LineTo( hdc, point.x+delta, point.y+delta );
		::MoveToEx( hdc, point.x-delta, point.y+delta, 0 );
		::LineTo( hdc, point.x+delta, point.y-delta );

		::TextOut( hdc, point.x+delta, point.y, sz, (int)strlen( sz ) );
	}

	::SelectObject( hdc, hOldPen );
	::SelectObject( hdc, hOldFont );

	::DeleteObject( hpen );
	::DeleteObject( hfont );
	
}
void CActionAphineByPoints::AddPoint( CPoint point, bool bOther )
{
	int	&c = bOther?m_nPointsCountOther:m_nPointsCount;
	CPoint	*p = bOther?m_pointsOther:m_points;

	if( c >= MAX_POINTS )
	{
		AfxMessageBox( IDS_MANY_POINTS );
		return;
	}

	
	p[c++] = point;
	Redraw( bOther );

	CString	str;
	str.Format( IDS_POINTCOUNTS, m_nPointsCount, m_nPointsCountOther );
	StatusSetPaneText( guidPaneAction, str );

	if( m_nPointsCountOther == m_nPointsCount &&
		m_nPointsCountOther == MAX_POINTS )
		Finalize();
}