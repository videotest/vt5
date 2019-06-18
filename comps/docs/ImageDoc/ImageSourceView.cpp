// ImageSourceView.cpp : implementation file
//

#include "stdafx.h"
#include "ImageDoc.h"
#include "ImageSourceView.h"
#include "image.h"
#include "Utils.h"
#include "math.h"
#include "idio_int.h"

#include "PropPage.h"
#include "MenuRegistrator.h"
#include "statusutils.h"
#include "ConvertorChooser.h"
#include "binary.h"
#include "editorint.h"
#include "\vt5\common2\class_utils.h"
#include "units_const.h"

#include "ObjectViews.h"
#include "BinaryView.h"
#include "chromosome.h"
#include "trajectory_int.h"
#include "statui_int.h"
#include "avi_int.h"
//#include "MainFrm.h"
//#include "CommandManager.h"

#include "CalibrMarker.h"

#include "drag_drop_defs.h"

#include "\vt5\AWIN\profiler.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


DWORD     g_pdwBinaryColors[256];
DWORD     g_clSyncColor;

GuidKey GetObjectBaseKey( IUnknown *punk_obj )
{
	GuidKey base_key;

	INamedDataObject2Ptr sptr = punk_obj;
	if( sptr == 0 )
		return base_key;

	sptr->GetBaseKey( &base_key );
	return base_key;
}

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

//ccmacro
bool m_ToDo = true;
void DrawBinaryRect( CBinImageWrp pbin, BITMAPINFOHEADER *pbi, BYTE *pdibBits, POINT ptBmpOfs, RECT rectDest, double fZoom, POINT ptScroll, DWORD dwFillColor, DWORD dwBackColor, BOOL bUseScrollZoom, DWORD dwShowFlags)
{
	if( !pdibBits )
		return;

	bool bFore = dwShowFlags & isfBinaryFore || dwShowFlags & isfBinary;
	bool bBack = dwShowFlags & isfBinaryBack || dwShowFlags & isfBinary;
	bool bContour = (dwShowFlags & isfBinaryContour) == isfBinaryContour;
	if(!bFore && !bBack && !bContour)
		return;

	COLORREF clrContour = 0;
	if(bContour)
		clrContour = ::GetValueColor(GetAppUnknown(), "Binary", "ContourColor", RGB(0, 0, 0));
	
	if( !pbi || pbin==0 )return;

	if(bUseScrollZoom == FALSE)
	{
		fZoom = 1;
		ptScroll.x = 0;
		ptScroll.y = 0;
	}

	int		cxImg, cyImg;
	pbin->GetSizes( &cxImg, &cyImg );

	POINT	pointImageOffest = {0, 0};
	pbin->GetOffset(&pointImageOffest);
	
	
//	if( dwFlags & cidrNoOffset )
//		pointImageOffest = CPoint( 0, 0 );

	int		x, y, xImg, yImg, yBmp;
	byte	fillR = GetRValue(dwFillColor);
	byte	fillG = GetGValue(dwFillColor);
	byte	fillB = GetBValue(dwFillColor);
	byte	backR = GetRValue(dwBackColor);
	byte	backG = GetGValue(dwBackColor);
	byte	backB = GetBValue(dwBackColor);
	byte	cntrR = GetRValue(clrContour);
	byte	cntrG = GetGValue(clrContour);
	byte	cntrB = GetBValue(clrContour);

	byte	*p, *pmask, *pmaskPrev, *pmaskNext;
	int		cx4Bmp = (pbi->biWidth*3+3)/4*4;
	int	xBmpStart = max( int(ceil(( pointImageOffest.x)*fZoom-ptScroll.x)), rectDest.left );				\
	int	xBmpEnd = min( int(floor((pointImageOffest.x+cxImg)*fZoom-ptScroll.x+.5)), rectDest.right );		\
	int	yBmpStart = max( int(ceil((pointImageOffest.y)*fZoom-ptScroll.y)), rectDest.top );					\
	int	yBmpEnd = min( int(floor((pointImageOffest.y+cyImg)*fZoom-ptScroll.y+.5)), rectDest.bottom );		\
	int	yImgOld = -1;
	int	yImgPrev = -1;
	int	yImgNext = -1;
	int	yImgPrevOld = -1;
	int	yImgNextOld = -1;

	int	t, cx = max(xBmpEnd-xBmpStart, 0), cy = max(yBmpEnd-yBmpStart, 0);									\
	int	*pxofs = new int[cx];																				\
	int	*pyofs = new int[cy];																				\
	for( t = 0; t < xBmpEnd-xBmpStart; t++ )																\
		pxofs[t] = int((t+xBmpStart+ptScroll.x)/fZoom-pointImageOffest.x);									\
	for( t = 0; t < yBmpEnd-yBmpStart; t++ )																\
		pyofs[t] = int((t+yBmpStart+ptScroll.y)/fZoom-pointImageOffest.y);									\

																											
	for( y = yBmpStart, yBmp = yBmpStart; y < yBmpEnd; y++, yBmp++ )
	{
		p = pdibBits+(pbi->biHeight-yBmp-1+ptBmpOfs.y)*cx4Bmp+(xBmpStart-ptBmpOfs.x)*3;
		yImg =pyofs[y-yBmpStart];
		if( yImgOld != yImg )
		{
			pbin->GetRow(&pmask, yImg, FALSE);
			yImgOld = yImg;
		}
		pmaskPrev = 0;
		pmaskNext = 0;
		if(bContour)
		{
			yImg =pyofs[y-yBmpStart-1];
			yImg =pyofs[y-yBmpStart+1];
			if(yImgPrev >= 0 && yImgPrev < cyImg && yImgPrevOld != yImgPrev)
			{
				pbin->GetRow(&pmaskPrev, yImgPrev, FALSE);
				yImgPrevOld = yImgPrev;
			}
			if(yImgNext >= 0 && yImgNext < cyImg && yImgNextOld != yImgNext)
			{
				pbin->GetRow(&pmaskNext, yImgNext, FALSE);
				yImgNextOld = yImgNext;
			}
		}
		for( x = xBmpStart; x < xBmpEnd; x++ )
		{
			xImg = pxofs[x-xBmpStart];

			if(pmask[xImg]<128)
			{
				if(bBack)
				{
					*p = backB; p++;
					*p = backG; p++;
					*p = backR; p++;
				}
				else if(!bContour)
				{
					p++;p++;p++;
				}
			}
			else 
			{
				if(bFore)
				{

					if( pmask[xImg] != 0xFF )
						dwFillColor = g_pdwBinaryColors[pmask[xImg]-1];
					else
						dwFillColor = g_clSyncColor;

					byte	fillR = GetRValue(dwFillColor);
					byte	fillG = GetGValue(dwFillColor);
					byte	fillB = GetBValue(dwFillColor);
          
					*p = fillB; p++;
					*p = fillG; p++;
					*p = fillR; p++;
				}
				else if(!bContour)
				{
					p++;p++;p++;
				}
			}

			if(bContour)
			{
				long nXPrev = x-1;
				xImg = pxofs[nXPrev-xBmpStart];
				long nXNext = x+1;
				xImg = pxofs[nXNext-xBmpStart];
				/*if((pmask[xImg/8] & (0x80>>(xImg%8)))!=0 && 
				  ((nXPrev >= 0 && (pmask[nXPrev/8] & (0x80>>(nXPrev%8)))==0)   ||
				   (nXNext < cxImg && (pmask[nXNext/8] & (0x80>>(nXNext%8)))==0)||
				   (pmaskPrev && (pmaskPrev[xImg/8] & (0x80>>(xImg%8)))==0)     || 
				   (pmaskNext && (pmaskNext[xImg/8] & (0x80>>(xImg%8)))==0)))*/
				if((pmask[xImg] !=0) && 
				  ((nXPrev >= 0 && (pmask[nXPrev] == 0)) ||
				   (nXNext < cxImg && (pmask[nXNext]==0))||
				   (pmaskPrev && (pmaskPrev[xImg]==0))   || 
				   (pmaskNext && (pmaskNext[xImg]==0))))
				{
					*p = cntrB; p++;
					*p = cntrG; p++;
					*p = cntrR; p++;
				}
				else
				{
					p++;p++;p++;
				}
			}
		}
	}
	delete pxofs;delete pyofs;
}


CSize	sizeNoImage( 400, 300 );

void DrawNoDataObject( CDC &dc, CRect rect )
{
	dc.SetBkMode( TRANSPARENT );
	LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );

	strcpy( lf.lfFaceName, "Times New Roman" );
	lf.lfCharSet = 1;
	lf.lfHeight = -32;

	CFont	fontLarge;
	fontLarge.CreateFontIndirect( &lf );

	strcpy( lf.lfFaceName, "Arial" );
	lf.lfHeight = -16;

	CFont	fontSmall;
	fontSmall.CreateFontIndirect( &lf );

	CString	strNoImage, strNoImageInfo;
	strNoImage.LoadString( IDS_NO_IMAGE );
	strNoImageInfo.LoadString( IDS_NO_IMAGE_INFO );

	rect.InflateRect( -10, -5 );

	CRect	rect1, rect2;
	rect1 = rect;
	rect2 = rect;

	HICON	hIcon = AfxGetApp()->LoadIcon( IDI_NOIMAGE );
	::DrawIcon( dc, rect1.left, rect1.top+30, hIcon );
	::DestroyIcon( hIcon );

	rect1.top += 10;
	rect1.left += 40;
	
	dc.SetTextColor( RGB( 0, 0, 0 ) );
	dc.SelectObject( &fontLarge );
	dc.DrawText( strNoImage, rect1, DT_CALCRECT );
	rect1.right = rect.right;
	rect1.bottom = min( rect1.bottom, rect.bottom );
	dc.DrawText( strNoImage, rect1, DT_END_ELLIPSIS );

	rect2.top = rect1.bottom+20;
	dc.SetTextColor( RGB( 0, 0, 255 ) );
	dc.SelectObject( &fontSmall );
	dc.DrawText( strNoImageInfo, rect2, DT_WORDBREAK|DT_END_ELLIPSIS );

	dc.SelectStockObject( SYSTEM_FONT );
}


COLORREF GetBackgroundColor()
{
	COLORREF	cr = ::GetValueColor( GetAppUnknown(), "\\Colors", "Background", RGB( 128, 128, 255 ) );
	return cr;
}
/*
COLORREF GetObjectClassColor( IUnknown *punkObject )
{
	return GetObjectClassColor(0, 0);
}
*/


/////////////////////////////////////////////////////////////////////////////////////////
// CImageSourceView

IMPLEMENT_DYNCREATE(CImageSourceView, CCmdTarget)

CImageSourceView::CImageSourceView()
: m_iSkip(0)
{
	m_fOldZoom = 1;
	m_bObjCountingOnly = false;
	m_bFromPrint = true;
	m_size_client = CSize( 0, 0 );
	m_sName = c_szCImageSourceView;
	m_sUserName.LoadString(IDS_SRCVIEW_NAME);

	m_bShowUnknown = ::GetPrivateProfileInt("Classes", "AppointUnknownClass", 0, full_classifiername(NULL))!=0;

	m_bPhases = false;
	if (::GetValueInt(::GetAppUnknown(), "ImageView", "EnableObjectResize", 1))
		m_dwFrameFlags = fcfResize|fcfRotate;
	else
		m_dwFrameFlags = 0;
	m_fZoom = 1;
	m_pointScroll = CPoint( 0, 0 );
	m_posLastSelectable =0;
	EnableAutomation();
	EnableAggregation();
	
	_OleLockApp( this );

	////m_location = CResizingFrame::None;
	m_bMouseImplWasEnabled = false;

	m_bObjectSelected = false;


	// Context menu for selection, drawing objects, etc...
	m_bShowChildContextMenu = true;
	
	m_pImagePane = 0;
	m_nImagePaneNum = -1;

	m_nActiveNumber = -1;
	m_bShowNumber = m_bShowName = false;
	m_nNumberPos = m_nNamePos = 0;

	m_dwShowFlags = isfSourceImage;

	m_pConvertPane = 0;

	m_bNeedSetAppropriateView = false;

	m_lenable_exended_move = -1;

	// center frame
	m_crCFLine = 0;
	m_lCFLineWidth = 0;
    m_CFSize.cx = m_CFSize.cy = 0;
	//
    
	_InitBinaryColorsTable();
}

CImageSourceView::~CImageSourceView()
{
	_attach_msg_listeners_from_list( m_drawobjects, false );

	ClearSelectableList();
	
	_OleUnlockApp( this );
}

void CImageSourceView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}

CString CImageSourceView::GetSectionName()
{
	if (m_bPhases)
	{
		CString s(szIVImageView);
		s += _T("\\Phases");
		return s; 
	}
	else
		return CString(szIVImageView);
}

void CImageSourceView::_InitBinaryColorsTable()
{
  DWORD dwDefColor = RGB(255,255,0);
  long nCounter = GetValueInt(GetAppUnknown(), "\\Binary", "IndexedColorsCount", 1);
  nCounter = max(0, min(256, nCounter));
  CString str;
  for(long i = 0; i < 256; i++)
  {
    if(i < nCounter) 
    {
		str.Format("Fore_%d", i);
		g_pdwBinaryColors[i] = GetValueColor(GetAppUnknown(), "\\Binary", str, dwDefColor);
    }
    else if( i > 255-nCounter && i<255 )
	{
		str.Format("Fore_%d", 255-i);
		g_pdwBinaryColors[i] = GetValueColor(GetAppUnknown(), "\\Binary", str, dwDefColor);
	}
	// начинаем со 127, т.к. цвет в бинари и индекс различаются на 1
	else if( ( i>=127 ) && ( i<127+nCounter ) )
	{
		str.Format("Fore_%d", i-127);
		g_pdwBinaryColors[i] = GetValueColor(GetAppUnknown(), "\\Binary", str, dwDefColor);
	}
	else if( ( i<127 ) && ( i>127-nCounter ) )
	{
		str.Format("Fore_%d", 127-i);
		g_pdwBinaryColors[i] = GetValueColor(GetAppUnknown(), "\\Binary", str, dwDefColor);
	}
	else
		g_pdwBinaryColors[i] = dwDefColor;
  }
  g_clSyncColor = GetValueColor(GetAppUnknown(), "\\Binary", "SyncColor", RGB( 255,255,0 ) );
}

BEGIN_MESSAGE_MAP(CImageSourceView, CViewBase)
	//{{AFX_MSG_MAP(CImageSourceView)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_CONTEXTMENU()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CImageSourceView, CViewBase)
	//{{AFX_DISPATCH_MAP(CImageSourceView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IImageView to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {2D8A4643-4323-11D3-A60F-0090275995FE}
static const IID IID_IImageViewDisp =
{ 0x2d8a4643, 0x4323, 0x11d3, { 0xa6, 0xf, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

BEGIN_INTERFACE_MAP(CImageSourceView, CViewBase)
	INTERFACE_PART(CImageSourceView, IID_IImageView, ImView)
	INTERFACE_PART(CImageSourceView, IID_IImageView2, ImView)
	INTERFACE_PART(CImageSourceView, IID_IImageViewDisp, Dispatch)
	INTERFACE_PART(CImageSourceView, IID_IEasyPreview, Preview)
	INTERFACE_PART(CImageSourceView, IID_IMenuInitializer, MenuInitializer)
	//INTERFACE_PART(CImageSourceView, IID_INamedPropBag, NamedPropBag)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CImageSourceView, ImView)

// {981F2EC9-256B-4a13-A87A-D038CADCFA72}
GUARD_IMPLEMENT_OLECREATE(CImageSourceView, szSourceViewProgID, 
0x981f2ec9, 0x256b, 0x4a13, 0xa8, 0x7a, 0xd0, 0x38, 0xca, 0xdc, 0xfa, 0x72);
/////////////////////////////////////////////////////////////////////////////
// CImageSourceView message handlers

void CImageSourceView::PostNcDestroy() 
{
	::UnRegisterMouseCtrlTarget( GetControllingUnknown() );
	if( ::IsWindow( GetSafeHwnd() ) )
		::KillTimer( GetSafeHwnd(), m_nIDTimerRepaintSelection );
}

int CImageSourceView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	//CreateImagePane();

	::RegisterMouseCtrlTarget( GetControllingUnknown() );

	INotifyControllerPtr ptr_nc(GetControllingUnknown());//GetAppUnknown();
	if( ptr_nc )
		ptr_nc->RegisterEventListener( 0, GetControllingUnknown() );

	/*
	CButton* pBtn = new CButton;
	// Create a push button.
	pBtn->Create(_T("My button"), WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON, 
		CRect(100,100,190,130), CWnd, 1);
	pBtn->SetWindowPos(&wndTopMost, 0, 0, 0, 0,
      SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE);
	  */

	WINDOWPLACEMENT wp;
	GetWindowPlacement(&wp);

	return 0;
}

//attach the active image							
void CImageSourceView::AttachActiveObjects()
{
//	m_pframe->EmptyFrame();
	_AttachObjects( GetControllingUnknown() );
	FillSelectableList();
	//paul 26.08.2003 if print - clear selection, BT 3377
	if( CheckInterface( GetControllingUnknown(), IID_IViewCtrl ) )
		ClearSelectableList();
	
	//attach images
	if( GetSafeHwnd() )
		Invalidate( TRUE );

	// [vanek]
	_UpdateAviPropPage( );
}


void CImageSourceView::DoPaint( CDC &dc, CRect rectPaint )
{
	dc.SetROP2( R2_COPYPEN );

}

bool CImageSourceView::_IsCalibrVisible()
{
	return ::_IsCalibrVisible(GetControllingUnknown());
}

void CImageSourceView::OnPaint() 
{
	m_bstr_class_name = "";

	//TIME_TEST( "CImageSourceView::OnPaint()" )

	m_clWhite = ::GetValueColor( ::GetAppUnknown(), "\\ImageView", "N_And_N_Color", RGB(255, 255, 255) );
	m_bAsClassColor = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "N_And_N_AsClassColor", 0 ) == 1;

	nInterpolationMethod=-1; //пусть его заново прочитает из shell.data

	m_fZoom = ::GetZoom( GetControllingUnknown() );
	m_pointScroll = ::GetScrollPos( GetControllingUnknown() );

	if( m_fZoom != m_fOldZoom )
	{
		m_object_coords.clear();
		m_object_coords_num.clear();
		m_fOldZoom = m_fZoom;
	}

	//get the image rectangle
	CSize	size_cur = m_size_client;

	if( size_cur.cx == 0 )
		size_cur = sizeNoImage;

	CRect	rectImage;

	rectImage.left = int(ceil( 0*m_fZoom-m_pointScroll.x));
	rectImage.right = int(floor((size_cur.cx)*m_fZoom-m_pointScroll.x + .5));
	rectImage.top = int(ceil( 0*m_fZoom-m_pointScroll.y));
	rectImage.bottom = int(floor((size_cur.cy)*m_fZoom-m_pointScroll.y + .5));

	bool bCenterView = _get_center_mode();
	
	POINT _pt = { 0 };
	CPoint ptMarkerOffset( 0, 0 );
	
	POINT ptLastImgOffset = {0};
	ptLastImgOffset = m_image.GetOffset();
  	if( bCenterView )
	{
		IScrollZoomSitePtr sptrZ = GetControllingUnknown();

		if( sptrZ )
		{
			CRect rcVisible = NORECT;

			sptrZ->GetVisibleRect( &rcVisible );
			
			if( rectImage.Width() < rcVisible.Width() )
				rectImage.OffsetRect( rcVisible.Width() / 2 - rectImage.Width() / 2, 0 );

			if( rectImage.Height() < rcVisible.Height() )
				rectImage.OffsetRect( 0, rcVisible.Height() / 2 - rectImage.Height() / 2 );
		}

		ptMarkerOffset = rectImage.TopLeft();

 		if( !m_image.IsEmpty() )
		{
			CPoint pt = rectImage.TopLeft();

			pt.x += m_pointScroll.x;
			pt.y += m_pointScroll.y;

			pt.x /= m_fZoom;
			pt.y /= m_fZoom;

			m_image.SetOffset( pt );
		}
	
	}


	//if( m_image )rectImage = m_image.GetRect();
	//rectImage = ConvertToWindow( GetControllingUnknown(), rectImage );
	

	CRect	rectPaint = NORECT;
	GetUpdateRect( &rectPaint );
	
	//create the paint and the DIB dc
	CPaintDC	dcPaint( this );


//	for( long n = 0; n < 500; n++ )
	{
	CDC			dcMemory;

 	if( GetFisrtVisibleObjectPosition() == 0 )
	{
//		rectImage = ::ConvertToWindow( GetControllingUnknown(), rectImage );

		rectImage.right		= rectImage.left + 300;
		rectImage.bottom	= rectImage.top + 300;
		
		dcPaint.FillRect( rectImage, &CBrush( GetSysColor( COLOR_WINDOW ) ) );
		DrawNoDataObject( dcPaint, rectImage );
		return;
	}


	//cache color convertor information

	
	IUnknown *punkColorCnv = 0;
	if( m_image != 0 )m_image->GetColorConvertor( &punkColorCnv );
	m_ptrCC = punkColorCnv;
	m_ptrCCLookup = punkColorCnv;
	if( punkColorCnv )punkColorCnv->Release();

//	ASSERT( m_ptrCC != 0 );
	//if( m_ptrCC == 0 )return;

	//set lookup table
	byte* pbyteLookup = 0;
	long nSize = 0;
	pbyteLookup = ::GetValuePtr(m_image, szLookup, szLookupTable, &nSize);
	if( m_ptrCCLookup != 0 )
	{
		int cx = 256;
		int cy = nSize/cx; 

		m_ptrCCLookup->SetLookupTable( pbyteLookup, cx, cy );

		if( pbyteLookup )delete pbyteLookup;
	}


	dcMemory.CreateCompatibleDC( &dcPaint );

	rectPaint.left = min( max( rectPaint.left, rectImage.left ), rectImage.right );
	rectPaint.right = min( max( rectPaint.right, rectImage.left ), rectImage.right );
	rectPaint.top = min( max( rectPaint.top, rectImage.top ), rectImage.bottom );
	rectPaint.bottom = min( max( rectPaint.bottom, rectImage.top ), rectImage.bottom );


	//init the DIB hearer
	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );

	bmih.biBitCount = 24;
	bmih.biHeight = rectPaint.Height();
	bmih.biWidth = rectPaint.Width();
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;
	bmih.biSizeImage=((int)(bmih.biWidth*3+3)/4)*4*bmih.biHeight;
	
	
	if( bmih.biHeight == 0 ||bmih.biWidth == 0 )
		return;

	//create a DIB section
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dcPaint, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );

	ASSERT( hDIBSection );

	CBitmap *psrcBitmap = new CBitmap;
	psrcBitmap->Attach( hDIBSection  );
	
	ASSERT( psrcBitmap );

	//propare the memory DC to drawing
	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );
	dcMemory.SetMapMode( MM_TEXT );
	dcMemory.SetViewportOrg( -rectPaint.TopLeft() );

	dcMemory.FillRect( rectPaint, &CBrush( ::GetBackgroundColor() ) );

	/////////////////////////////////////////////////////////////////////drawing
	//main paint cycle - draw the main image
	DWORD dwDrawFlags = 0;
	if((m_dwShowFlags & isfPseudoImage) || (m_dwShowFlags & isfFilledImageExceptObjects))
		dwDrawFlags = DrawMask;
	else
		dwDrawFlags = DrawImage;
	
	_DrawImage( dcMemory, m_image, rectPaint, dwDrawFlags, ::GetBackgroundColor(), &bmih, pdibBits );

	CPen	pen;
	pen.CreatePen( PS_SOLID, 0, RGB( 255, 0, 0 ) );
	dcMemory.SelectObject( &pen );

	bool	bDrawSelection = !m_imageSelection.IsEmpty() && !m_pframe->CheckObjectInSelection( (IUnknown*)m_imageSelection );

	//draw the "body" of selection. Draw here only if it is inactive and it doesn't match with preview image
	if( bDrawSelection &&((m_dwShowFlags & isfSourceImage)||
			(m_dwShowFlags & isfBinaryFore)||
			(m_dwShowFlags & isfContours)||
			(m_dwShowFlags & isfFilledOnlyObjects)||
			(m_dwShowFlags & isfBinaryContour)))
	{
		_DrawImage( dcMemory, m_imageSelection, rectPaint, DrawImage|DrawClip, ::GetBackgroundColor(), &bmih, pdibBits );
	}


	// [vanek] : set Arial font - 06.07.2004
	CFont* pOldFont = dcMemory.GetCurrentFont();
	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(LOGFONT) );
	if( pOldFont )
		pOldFont->GetLogFont( &lf );

	strcpy( lf.lfFaceName, "Arial" );
	CFont fontNew;
	fontNew.CreateFontIndirect( &lf );		
    dcMemory.SelectObject( &fontNew );

	m_bObjCountingOnly = true;
	DrawLayers(&dcMemory, &bmih, pdibBits, rectPaint, true);
	m_bObjCountingOnly = false;

	m_nDrawFrameStage = 0;
	m_pframe->Draw(dcMemory, rectPaint, &bmih, pdibBits);
	DrawLayers(&dcMemory, &bmih, pdibBits, rectPaint, true);
	m_nDrawFrameStage = 1;
	m_pframe->Draw(dcMemory, rectPaint, &bmih, pdibBits);
	m_nDrawFrameStage = -1;

    dcMemory.SelectObject( pOldFont );

	//draw the selection. Draw here only if it is inactive and it doesn't match with preview image
	if( bDrawSelection )
	{
		COLORREF	cr = GetValueColor( ::GetAppUnknown(), "\\Colors", "Selection", RGB( 255, 255, 0 ) );
		CPen	pen( PS_DOT, 0, cr );

		dcMemory.SetBkMode(OPAQUE);
		dcMemory.SelectObject( &pen );
		dcMemory.SelectStockObject( NULL_BRUSH );
		dcMemory.SetBkColor( RGB( 255, 255, 255 ) );

		_DrawImage( dcMemory, m_imageSelection, rectPaint, DrawContours|DrawSelFrame, ::GetBackgroundColor(), &bmih, pdibBits );

		dcMemory.SelectStockObject( BLACK_PEN );
	}


	
	//draw binary
//	POSITION pos = m_listBinaries.GetHeadPosition();
//	while(pos)
//		DrawBinaryRect(m_listBinaries.GetNext(pos), &bmih, pdibBits, rectPaint.TopLeft(), rectPaint, m_fZoom, m_pointScroll, m_dwFillColor, m_dwBackColor, TRUE, m_dwShowFlags);
//	_DrawObjectList( dcMemory, rectPaint, &bmih, pdibBits );

	dcMemory.SelectStockObject( WHITE_PEN );

	_DrawObjects( dcMemory, rectPaint, &bmih, pdibBits );

//	m_pframe->Draw(dcMemory, rectPaint, &bmih, pdibBits);
	m_pframeDrag->Draw(dcMemory, rectPaint, &bmih, pdibBits);

	if( m_ptrCCLookup != 0 )
		m_ptrCCLookup->KillLookup();
	m_ptrCC = 0;
	m_ptrCCLookup = 0;
	
	DrawHilightedObjects( dcMemory, &bmih, pdibBits, rectPaint );

	//draw the plug-ins
	ProcessDrawing( dcMemory, rectPaint, &bmih, pdibBits );


	//draw the grid
 	_DrawGrid( dcMemory, rectPaint );

	 if(_IsCalibrVisible())
	{
 		_DrawCalibr( dcMemory, rectPaint, &bmih, pdibBits,
			bCenterView ? ptMarkerOffset + m_pointScroll : CPoint(0,0) );
	}

	// [vanek]
	bool bShowFrame = _IsCenterFrameVisible();
	if( bShowFrame )
		_DrawCenterFrame( dcMemory, TRUE );
    //

 	dcMemory.SetViewportOrg( 0, 0 );

	//paint to paint DC
	::SetDIBitsToDevice( dcPaint, rectPaint.left, rectPaint.top, rectPaint.Width(), rectPaint.Height(),
		0, 0, 0, rectPaint.Height(), pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS );

	if( !m_image.IsEmpty() )
		m_image.SetOffset( ptLastImgOffset  );

	// [vanek]
	if( bShowFrame )
		_DrawCenterFrame( dcPaint, FALSE );
	//

	//prepare to delete objects
	dcMemory.SelectObject( poldBmp );

	if( psrcBitmap )
	{
		psrcBitmap->DeleteObject();
		delete psrcBitmap;
	}
	}
	dcPaint.SetROP2( R2_COPYPEN );
	dcPaint.SelectStockObject( BLACK_PEN );
	m_pframe->CResizingFrame::Draw(dcPaint, rectPaint);

	//paint the mouse controller
	sptrIMouseController	sptrC( ::GetMouseController() );
	if( sptrC != 0 )sptrC->RepaintMouseCtrl( GetControllingUnknown(), dcPaint, 2 );

	m_ToDo = true;
}

BOOL CImageSourceView::OnEraseBkgnd(CDC* pDC) 
{
	if( GetFisrtVisibleObjectPosition() == 0 )
	{
		CRect rcUpdate;
		GetClientRect( rcUpdate );
		pDC->FillRect( rcUpdate, &CBrush( GetSysColor( COLOR_WINDOW ) ) );
		return TRUE;
	}

	return CViewBase::OnEraseBkgnd(pDC);
}

BOOL CImageSourceView::PreCreateWindow(CREATESTRUCT& cs) 
{
	//cs.style ^= CS_HREDRAW|CS_VREDRAW;
	return CViewBase::PreCreateWindow(cs);
}

//implememntation of IImageView
HRESULT CImageSourceView::XImView::SetSelectionMode( long lSelectType )
{
	_try_nested(CImageSourceView, ImView, SetSelectionMode)
	{
		//pThis->_ChangeSelectionMode( lSelectType );
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImageSourceView::XImView::GetSelectionMode( long *plSelectType )
{
	_try_nested(CImageSourceView, ImView, GetSelectionMode)
	{
		//*plSelectType = pThis->m_SelectionMode;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImageSourceView::XImView::SetActiveImageDuringPreview( IUnknown *punkImage, IUnknown *punkSelection )
{
	_try_nested(CImageSourceView, ImView, SetActiveImage)
	{
		if( pThis->IsPreviewModeActive() )
		{
			pThis->m_imageSource = punkImage;
			pThis->m_imageSourceSelection = punkSelection;
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImageSourceView::XImView::GetActiveImage( IUnknown **ppunkImage, IUnknown **ppunkSelection )
{
	_try_nested(CImageSourceView, ImView, GetActiveImage)
	{
		if( ppunkImage )
		{
			*ppunkImage = pThis->IsPreviewModeActive()?pThis->m_imageSource:pThis->m_image;
			if( *ppunkImage )
				(*ppunkImage)->AddRef();
		}
		if( ppunkSelection )
		{
			*ppunkSelection = pThis->IsPreviewModeActive()?pThis->m_imageSourceSelection:pThis->m_imageSelection;
			if( *ppunkSelection )
				(*ppunkSelection)->AddRef();
		}
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImageSourceView::XImView::GetActiveImage2( IUnknown **ppunkImage, IUnknown **ppunkSelection, BOOL bPreview )
{
	_try_nested(CImageSourceView, ImView, GetActiveImage2)
	{
		if( ppunkImage )
		{
			*ppunkImage = pThis->IsPreviewModeActive()&&!bPreview?pThis->m_imageSource:pThis->m_image;
			if( *ppunkImage )
				(*ppunkImage)->AddRef();
		}
		if( ppunkSelection )
		{
			*ppunkSelection = pThis->IsPreviewModeActive()&&!bPreview?pThis->m_imageSourceSelection:pThis->m_imageSelection;
			if( *ppunkSelection )
				(*ppunkSelection)->AddRef();
		}
		return S_OK;
	}
	_catch_nested;
}


HRESULT CImageSourceView::XImView::EnableControlFrame( DWORD dwFrameControlsFlags )
{
	_try_nested(CImageSourceView, ImView, GetActiveImage)
	{
		pThis->m_dwFrameFlags = dwFrameControlsFlags;
		return S_OK;
	}
	_catch_nested;
}

HRESULT CImageSourceView::XImView::GetImageShowFlags( DWORD *pdwFlags )
{
	_try_nested(CImageSourceView, ImView, GetImageShowFlags)
	{
		*pdwFlags = 0;

		*pdwFlags = pThis->m_dwShowFlags;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CImageSourceView::XImView::SetImageShowFlags( DWORD dwFlags )
{
	_try_nested(CImageSourceView, ImView, GetImageBinaryFlags)
	{
		pThis->m_dwShowFlags = dwFlags;

		return S_OK;
	}
	_catch_nested;
}


void CImageSourceView::OnTimer(UINT_PTR nIDEvent) 
{
	::CheckMouseInWindow( GetControllingUnknown() );

	_CheckFrameState();

	CViewBase::OnTimer(nIDEvent);
}


DWORD CImageSourceView::GetMatchType( const char *szType )
{
	if( !strcmp( szType, szTypeImage ) )
		return mvFull;
	else
	if( !strcmp( szType, szDrawingObjectList ) )
		return /*( m_image == 0 )?mvNone:*/mvPartial;
	else
		return mvNone;
}


ObjectFlags CImageSourceView::GetObjectFlags( const char *szType )
{				 
	if( !strcmp( szType, szTypeImage ) )
		return ofOnlyOneChild;
	if( !strcmp( szType, szTypeObjectList ) ||
		!strcmp( szType, szTypeObject  ) ||
		!strcmp( szType, szDrawingObject )  )
		return ofNormal;

	return CViewBase::GetObjectFlags( szType );
}

bool CImageSourceView::DoDrag( CPoint point )
{
	bool ballow_object_drag = ( 1L == ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "EnableObjectDrag", 1L ) );
	bool ballow_drawing_drag = ( 1L == ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "EnableDrawingDrag", 1L ) );

	if( m_pframeDrag )
	{
		bool bneed_kill = false;
		int ncount = m_pframeDrag->GetObjectsCount();
		if( !ncount )
			return false;

		for( int i=0;i<ncount;i++ )
		{
			IUnknown* punk = m_pframeDrag->GetObjectByIdx( i );
			if( punk )
			{
				IMeasureObjectPtr	ptrMeasure( punk );
				ICompositeSupportPtr cs(punk);
					if(cs!=0)
					{
						long pCount;// 
						cs->GetPixCount(&pCount);
						ballow_object_drag = (pCount==0);
					}
				
				IDrawObjectPtr		ptrDrawing( punk );
				ITrajectoryPtr		ptrTrajectory( punk );
				punk->Release();	punk = 0;				
				
				if( ptrTrajectory != 0 )
				{ // для траекторий таскание не реализовано. Плохо, что проверка здесь, а не в Trajectory :-(
					bneed_kill = true;
					break;
				}
				if( ptrMeasure != 0 && !ballow_object_drag )
				{
					bneed_kill = true;
					break;
				}
				if( ptrDrawing != 0 && !ballow_drawing_drag )
				{
					bneed_kill = true;
					break;
				}

			}
		}

		if( bneed_kill )
		{
			m_pframeDrag->EmptyFrame();
			return false;
		}

	}


	return CViewBase::DoDrag( point );
}

bool CImageSourceView::_DrawImage( CDC &dc, CImageWrp &image, CRect rectClip, DWORD dwFlags, COLORREF cr, BITMAPINFOHEADER *pbih, byte *pdibBits, bool bScrollZoom )
{
	if( image.IsEmpty() )
		return false;

	int		nFrameSize = 100;

	if(dwFlags & DrawImage || dwFlags & DrawMask)
	{
		IUnknown *punkColorCnv = 0;
		image->GetColorConvertor( &punkColorCnv );

		if( punkColorCnv )
		{

			IColorConvertorExPtr	ptrCC( punkColorCnv );
			IColorConvertor2Ptr ptrCC2( punkColorCnv );

			punkColorCnv->Release();

			CRect	rectDraft = image.GetRect();

			rectDraft.left = int( rectDraft.left*m_fZoom-m_pointScroll.x-2 );
			rectDraft.right = int( rectDraft.right*m_fZoom-m_pointScroll.x+2 );
			rectDraft.top = int( rectDraft.top*m_fZoom-m_pointScroll.y-2 );
			rectDraft.bottom = int( rectDraft.bottom*m_fZoom-m_pointScroll.y+2 );



			DWORD	dwDrawFlags = cidrUseImageOffset|((dwFlags & DrawMask) ? cidrFillBodyByColor : 0);

			if( pdibBits)
			{
				rectDraft.IntersectRect( rectClip, rectDraft );


				if( (rectDraft.right <= rectDraft.left||	rectDraft.bottom <= rectDraft.top) && bScrollZoom )
					return true;

				ptrCC->ConvertImageToDIBRect( pbih, pdibBits, rectClip.TopLeft(), (IImage2*)image, 
					rectClip,  CPoint( 0, 0 ),
					bScrollZoom ? m_fZoom : 1, bScrollZoom ? m_pointScroll : CPoint(0, 0), cr, dwDrawFlags,
					GetControllingUnknown());
			}
		}
	}

 		
	if( m_bFromPrint && dwFlags & DrawContours )
	{
		CPen	pen;
		CPen*	pOldPen = 0;
		bool bRestore = false;

		if( (dwFlags & DrawSelFrame) ==0 )
		{
			pen.CreatePen( PS_SOLID, 0, cr );
			pOldPen = dc.SelectObject( &pen );
			bRestore = true;
		}
		int	nCount = image.GetContoursCount();
		for( int nPos = 0; nPos < nCount; nPos++ )
		{
			Contour	*pc = image.GetContour( nPos );
			::ContourDraw( dc, pc, bScrollZoom ? m_fZoom : 1, bScrollZoom ? m_pointScroll : CPoint(0, 0), cdfClosed );
		}

		if (bRestore)
			dc.SelectObject(pOldPen);
	}

	return true;
}

bool CImageSourceView::_DrawGrid( CDC &dc, CRect rectClip )
{
	if( m_image.IsEmpty() )
		return true;

	CRect	rectClient;
	GetClientRect( rectClient );
	rectClip.left = max( rectClip.left, rectClient.left );
	rectClip.top = max( rectClip.top, rectClient.top );
	rectClip.right = min( rectClip.right, rectClient.right );
	rectClip.bottom = min( rectClip.bottom, rectClient.bottom );

	CRect	rectImage;
	rectImage.left = (int)::floor( (rectClip.left+m_pointScroll.x)/m_fZoom );
	rectImage.top = (int)::floor( (rectClip.top+m_pointScroll.y)/m_fZoom );
	rectImage.right = (int)::ceil( (rectClip.right+m_pointScroll.x)/m_fZoom );
	rectImage.bottom = (int)::ceil( (rectClip.bottom+m_pointScroll.y)/m_fZoom );

	CRect	rectI = m_image.GetRect();

	rectImage.left = max( rectI.left, min( rectI.right, rectImage.left ) );
	rectImage.right = max( rectI.left, min( rectI.right, rectImage.right ) );
	rectImage.top = max( rectI.top, min( rectI.bottom, rectImage.top ) );
	rectImage.bottom = max( rectI.top, min( rectI.bottom, rectImage.bottom ) );

	if( rectClip == NORECT )
		return true;

	/*CRgn	rgn;
	rgn.CreateRectRgn( rectClip.left, rectClip.top, rectClip.right, rectClip.bottom );
	dc.SelectClipRgn( &rgn );*/

	CPoint	pt1, pt2;

	if( m_fZoom > 10 )
	{
		dc.SetROP2( R2_COPYPEN );
		dc.SelectStockObject( WHITE_PEN );

		for( int i = rectImage.left; i < rectImage.right; i++ )
		{
			pt1.x = int( i*m_fZoom-m_pointScroll.x+.5 );
			pt1.y = int( rectI.top*m_fZoom-m_pointScroll.y+.5 );
			pt2.x = int( i*m_fZoom-m_pointScroll.x+.5 );
			pt2.y = int( rectI.bottom*m_fZoom-m_pointScroll.y+.5 );

			dc.MoveTo( pt1 );
			dc.LineTo( pt2 );
		}

		for( i = rectImage.top; i < rectImage.bottom; i++ )
		{
			pt1.x = int( rectI.left*m_fZoom-m_pointScroll.x+.5 );
			pt1.y = int( i*m_fZoom-m_pointScroll.y+.5 );
			pt2.x = int( rectI.right*m_fZoom-m_pointScroll.x+.5 );
			pt2.y = int( i*m_fZoom-m_pointScroll.y+.5 );

			dc.MoveTo( pt1 );
			dc.LineTo( pt2 );
		}
	}

	//dc.SelectClipRgn( 0 );

	return true;
}

bool CImageSourceView::_DrawObjects( CDC &dc, CRect rectClip, BITMAPINFOHEADER* pbmih, byte* pdibBits )
{
	 //draw comment objects
	sptrIDrawObject sptrObj(m_drawobjects);
	if(sptrObj != 0)
		sptrObj->Paint(dc.GetSafeHdc(), rectClip, GetControllingUnknown(), pbmih, pdibBits );

	return true;
}

bool CImageSourceView::_DrawCalibr( CDC &dc, CRect rectClip, BITMAPINFOHEADER* pbmih, byte* pdibBits, CPoint ptOffset, bool bPrint, double fPrintZoom, bool bPrintPreview)
{
	double	fCalibr = 0.0025;
	GUID guidC;
	::GetDefaultCalibration( &fCalibr, &guidC );

	ICalibrPtr	ptrCalibr = m_image;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, 0 );

	::_DrawCalibr(dc, rectClip, pbmih, pdibBits, bPrint, fPrintZoom, bPrintPreview,
		m_fZoom, fCalibr, m_pointScroll, m_size_client, this, ptOffset );

	/*
	CRect	rect;
	GetClientRect( rect );

	double fPrintMarkerScale = 1;
	if(bPrint) 
		fPrintMarkerScale = ::GetValueDouble(::GetAppUnknown(), "ImageView", "PrintMarkerScale", 1.0);

	rect.left = PrtScale(rect.left);
	rect.right = PrtScale(rect.right);
	rect.top = PrtScale(rect.top);
	rect.bottom = PrtScale(rect.bottom);

	//get the image rectangle
	CSize	size_cur = m_size_client;

	if( size_cur.cx<50 ) return false; // не рисуем на слишком мелких

	int nDPI = dc.GetDeviceCaps(LOGPIXELSX);
	//double fMeterPerPixelDC = 0.0254/max(nDPI,1);
	int nHorzSize = dc.GetDeviceCaps(HORZSIZE);
	int nHorzRes = dc.GetDeviceCaps(HORZRES);
	double fMeterPerPixelDC = nHorzSize*0.001/max(1,nHorzRes);

	int nPhysicalWidth = dc.GetDeviceCaps(PHYSICALWIDTH);
	int nLogPixelsX = dc.GetDeviceCaps(LOGPIXELSX);
	int nScalingFactorX = dc.GetDeviceCaps(SCALINGFACTORX);

	double	fCalibr = 0.0025;
	GUID guidC;
	::GetDefaultCalibration( &fCalibr, &guidC );

	ICalibrPtr	ptrCalibr = m_image;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, 0 );

	double fMeterPerPixel = fCalibr/m_fZoom;

	double fUnitPerMeter = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );

	CString strType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
	double fPixelPerUnit = 1/fUnitPerMeter/fMeterPerPixel;

	double fScreenSize = 15.5 * 2.54e-2; // диагональ экрана в метрах
	int cxScreen = ::GetSystemMetrics(SM_CXSCREEN);
	int cyScreen = ::GetSystemMetrics(SM_CYSCREEN);
	double fScreenMetersPerPixel = fScreenSize / _hypot(cxScreen,cyScreen);
	double fVisibleScale = fMeterPerPixel / fScreenMetersPerPixel;
	if(bPrint)
	{
		fVisibleScale = fMeterPerPixelDC/fMeterPerPixel;
		if(bPrintPreview) fVisibleScale /= fPrintZoom;
	}

	int bi_w34=(pbmih->biWidth*3+3)/4*4;

	CRect	rectImage;

	rectImage.left = int(ceil( 0*m_fZoom-m_pointScroll.x));
	rectImage.right = int(floor((size_cur.cx)*m_fZoom-m_pointScroll.x + .5));
	rectImage.top = int(ceil( 0*m_fZoom-m_pointScroll.y));
	rectImage.bottom = int(floor((size_cur.cy)*m_fZoom-m_pointScroll.y + .5));

	rect &= rectImage;

	if( rect.Width()<100 ) return false; // не рисуем на слишком мелких

	double fCalUnits=1;
	int lim=20;	// ограничим циклы - чтобы не было бесконечного
	while(fPixelPerUnit*fCalUnits > rect.Width()/2 && lim-->0) fCalUnits /= 10;
	while(fPixelPerUnit*fCalUnits < rect.Width()/2 && lim-->0) fCalUnits *= 10;
	if(fPixelPerUnit*fCalUnits > rect.Width()/2) fCalUnits /= 2;
	if(fPixelPerUnit*fCalUnits > rect.Width()/2) fCalUnits /= 2.5;
	if(fPixelPerUnit*fCalUnits > rect.Width()/2) fCalUnits /= 2;

	int x1 = rect.right - PrtScaleSize(10);
	int y1 = rect.bottom - PrtScaleSize(16);
	int w = int(fPixelPerUnit*fCalUnits);
		//rect.Width() / 4;
	double dw=w/5.0;
	int h = PrtScaleSize(8);
	int x0 = x1-w, y0 = y1-h;

	rect.top = max(rect.top, rect.bottom-PrtScaleSize(30));

	COLORREF clBack = ::GetValueColor(GetAppUnknown(), "\\ImageView", "MarkerBackground", RGB(255,255,255));
	int backR = GetRValue(clBack);
	int backG = GetGValue(clBack);
	int backB = GetBValue(clBack);

	if(pdibBits!=0)
	{
		CRect rectHighlight = rect & rectClip;
		for(int y=rectHighlight.top; y<rectHighlight.bottom; y++)
		{
			int yy=y-rectClip.top;
			yy=pbmih->biHeight-1-yy;
			byte *pb = pdibBits + bi_w34*yy;
			for(int x=rectHighlight.left; x<rectHighlight.right; x++)
			{
				int xx=x-rectClip.left;
				(*pb) = (*pb+backR)/2; pb++;
				(*pb) = (*pb+backG)/2; pb++;
				(*pb) = (*pb+backB)/2; pb++;
			}
		}
	}

	COLORREF color = ::GetValueColor(GetAppUnknown(), "\\ImageView", "MarkerColor", RGB(0,0,0));	
	COLORREF color2 = ::GetValueColor(GetAppUnknown(), "\\ImageView", "MarkerColor2", RGB(255,255,255));	
	CPen	pen(PS_SOLID, 0, color);
	CBrush	brush(color);
	CBrush	brush2(color2);


	//dc.SelectStockObject( BLACK_PEN );
	CPen	*pOldPen = (CPen*)dc.SelectObject( &pen );
	//dc.SelectStockObject( WHITE_BRUSH );
	CBrush* pOldBrush = (CBrush*)dc.SelectObject( brush2 );
	dc.Rectangle(x0,y0,x1,y1);
	dc.SelectObject( brush );
	for(int i=0; i<5; i+=2)
	{
		dc.Rectangle(int(x0+dw*i+0.5),y0,int(x0+dw*i+dw+0.5),y1);
	}

	dc.SetBkMode( TRANSPARENT );
	LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );

	strcpy( lf.lfFaceName, "Arial" );
	lf.lfCharSet = 1;
	lf.lfHeight = -PrtScaleSize(11);
	lf.lfWeight = FW_BOLD;

	CFont	font;
	font.CreateFontIndirect( &lf );

	dc.SelectObject( &font );
	char szbuf[256];
	format3(szbuf,fCalUnits);

	dc.SetTextColor(color);
	CString str = CString(szbuf) + " " + strType;
	CRect rect1 = CRect(x0, rect.top+PrtScaleSize(15), x1, rect.bottom);
	CRect rect2 = rect1;
	dc.DrawText( str, rect2, DT_CALCRECT );
	if(rect2.Width() <= rect1.Width())
	{
		rect1.left = (rect1.left + rect1.right)/2 - rect2.Width()/2;
		dc.DrawText( str, rect1, 0);
	}

	rect1 = CRect(rect.left+PrtScaleSize(8), rect.top+PrtScaleSize(8), x0-PrtScaleSize(5), rect.bottom);
	rect2 = rect1;

	if(fVisibleScale>1)
	{
		format3(szbuf,fVisibleScale);
		str = CString("1 : ") + szbuf;
	}
	else
	{
		format3(szbuf,1/fVisibleScale);
		str = CString(szbuf) + " : 1";
	}
	dc.DrawText( str, rect2, DT_CALCRECT );
	if(rect2.Width() <= rect1.Width())
	{
		//rect1.left = (rect1.left + rect1.right)/2 - rect2.Width()/2;
		dc.DrawText( str, rect1, 0);
	}

	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);
	*/

	return true;
}

// [vanek] 23.10.2003
bool CImageSourceView::_IsCenterFrameVisible( ) 
{ 
	return 1 == ::GetValueInt( m_image, CF_SECTION, CF_SHOWFRAME, 0 ); 
}

bool	CImageSourceView::_DrawCenterFrame( CDC &dc, BOOL	bUpdateData /*= FALSE*/ )
{
	if( bUpdateData )
	{
		m_lCFLineWidth = ::GetValueInt( m_image, CF_SECTION, CF_LINEWIDTH, 1 );
		m_crCFLine = ::GetValueColor( m_image, CF_SECTION, CF_LINECOLOR, RGB( 255, 255, 255 ) );

		m_CFSize.cx = ::GetValueInt( m_image, CF_SECTION, CF_FRAMEWIDTH, 50 );
		m_CFSize.cy = ::GetValueInt( m_image, CF_SECTION, CF_FRAMEHEIGHT, 50 );
	}
	
	CSize size = m_CFSize;
	size.cx = int( m_CFSize.cx * m_fZoom );
	size.cy = int( m_CFSize.cy * m_fZoom );

	CRect rcCenter( 0, 0, size.cx + m_lCFLineWidth + 1, size.cy + m_lCFLineWidth + 1 ), rcClient( 0, 0, 0, 0 );
	GetClientRect( &rcClient );
    
	CPen pen( PS_SOLID, m_lCFLineWidth, m_crCFLine );
	CPen* pOldPen = 0;
	pOldPen = dc.SelectObject( &pen );
	
	CGdiObject *poldbrush = 0;
	poldbrush = dc.SelectStockObject( NULL_BRUSH );

	rcCenter.OffsetRect( rcClient.Width() / 2 - rcCenter.Width() / 2, rcClient.Height() / 2 - rcCenter.Height() / 2 );

    int nOldROP2 = dc.SetROP2( R2_COPYPEN );
	dc.Rectangle( &rcCenter );
	dc.SetROP2( nOldROP2 );

	dc.SelectObject( poldbrush );		
	dc.SelectObject( pOldPen );
	
    return true;        
}
//

void CImageSourceView::CalcClientSize()
{
	if( m_image )
		m_size_client = CSize( m_image.GetWidth(), m_image.GetHeight() );
	else
		m_size_client = CSize( 0, 0 );
}


void CImageSourceView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CViewBase::OnNotify(pszEvent, punkFrom, punkHit, pdata, cbSize );

	if( strcmp(pszEvent, szNeedUpdateView) == 0)
	{
		IUnknown * punkDoc = GetDocument();
		if (!punkDoc)
			return;
		if( ::GetObjectKey(punkDoc) != ::GetObjectKey(punkFrom) )
		{
			punkDoc->Release();
			return;
		}
		punkDoc->Release();

		sptrIPaintInfo sptrInfo(punkHit);
		if(sptrInfo == 0)
			return;
		CRect rc = NORECT;
		sptrInfo->GetInvalidRect( &rc );
		rc = ConvertToWindow( GetWindow()->GetControllingUnknown(), rc );
		InvalidateRect(rc);
//			UpdateWindow();
	}
	else if( !strcmp( pszEvent, szEventChangeObjectList ) )
	{
		long	lHint= *(long*)pdata;
	
		DoNotifySelectableObjects( lHint, punkHit, punkFrom );

		CalcClientSize();

		_recalc_rect( true, ( lHint == cncRemove ) ? punkHit : 0   );

		//грубо
		if( lHint == cncRemove )
		{
//			m_object_coords.remove_key( (long)punkHit );
//			m_object_coords_num.remove_key( (long)punkHit );
			m_object_coords.clear();
			m_object_coords_num.clear();
		}

		if( ( lHint == cncReset || lHint == cncChange ) && ( m_class_names.count() > 0 || m_class_names2.count() > 0 ) )
		{
			m_sptrAttached = 0;

			m_class_names.clear();
			m_class_colors.clear();
			m_object_coords.clear();
			m_object_coords_num.clear();
			
			m_class_names2.clear();
			m_class_colors2.clear();
			
			m_ToDo = false;
		}

		if( lHint == cncChange )
		{
			if( m_pframe->CheckObjectInSelection( punkHit ) )
			{
				m_pframe->EmptyFrame();
				m_pframe->SelectObject( punkHit );
				m_object_coords.clear();
				m_object_coords_num.clear();
			}
		}

		if(::GetObjectKey( punkFrom ) == ::GetObjectKey( m_drawobjects ))
		{
			if( lHint == cncAdd )
			{
				_attach_msg_listener( punkHit, true );
			}
			else if( lHint == cncRemove )
			{
				_attach_msg_listener( punkHit, false );
			}
			if( lHint == cncReset && GetSafeHwnd() )
				Invalidate();			
		}
	}
	else if( !strcmp( pszEvent, szEventNewSettings ) )
	{
		m_sptrAttached = 0;

		m_class_names.clear();
		m_class_colors.clear();
		m_object_coords.clear();
		m_object_coords_num.clear();

		m_class_names2.clear();
		m_class_colors2.clear();

		InvalidateRect( 0 );

	}
/*	else if( !strcmp( pszEvent, szEventChangeObjectList ) )
	{
		long	lHint= *(long*)pdata;
		DoNotifySelectableObjects( lHint, punkHit, punkFrom );
	
		if(::GetObjectKey( punkFrom ) == ::GetObjectKey( m_drawobjects ))
		{
			if( lHint == cncReset )
			{
				if( GetSafeHwnd() )
					Invalidate();
			}
			if( lHint == cncAdd||lHint == cncActivate)
			{
				if(::GetObjectKey( punkFrom ) == ::GetObjectKey( m_drawobjects )  )
				{
					m_pframe->SelectObject( punkHit, false );
				}
				CRect	rect = GetObjectRect( punkHit );
				InvalidateRect( ::ConvertToWindow( GetControllingUnknown(), rect ) );

			}

		}
	}
*/	

	else if( !strcmp( pszEvent, szEventChangeObject ) )
	{
		if( ::GetObjectKey( punkFrom ) == ::GetObjectKey( m_image ) )
		{
			//paul 12.09.2001. 
			_UpdateImagePane();		//Cos image CC can change after serialization
			//paul 12.09.2001 

			CalcClientSize();

			_UpdateScrollSizes();			
		}
	}
    /*else if(!strcmp( pszEvent, szEventNewSettings ))
    {
      _InitBinaryColorsTable();
	  Invalidate();
    }*/
    else if(!strcmp( pszEvent, szEventScrollPosition ))
    {
		CRect rect(NORECT);
		GetClientRect( rect );

		//get the image rectangle
		CSize	size_cur = m_size_client;

		if( size_cur.cx == 0 )
			size_cur = sizeNoImage;

		CRect	rectImage;

		rectImage.left = int(ceil( 0*m_fZoom-m_pointScroll.x));
		rectImage.right = int(floor((size_cur.cx)*m_fZoom-m_pointScroll.x + .5));
		rectImage.top = int(ceil( 0*m_fZoom-m_pointScroll.y));
		rectImage.bottom = int(floor((size_cur.cy)*m_fZoom-m_pointScroll.y + .5));

		rect &= rectImage;
		int nTop = max(rect.top, rect.bottom-30);
		CPoint ptScroll(0,0);
		IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
		if(ptrZ!=0)	ptrZ->GetScrollPos( &ptScroll );
		rect.top = max(rect.top, min(nTop,m_nLastCalibrTop-ptScroll.y));
		InvalidateRect( &rect );
		m_nLastCalibrTop = ptScroll.y + nTop;
	}	
    else if(!strcmp( pszEvent, "EventGetScrollableRect" ))
    {
		if(pdata!=0 && cbSize==sizeof(CRect))
		{
			if(_IsCalibrVisible())
			{
				// дублирующийся код определения image rect - вынести в ф-ю
				CRect	rect;
				GetClientRect( rect );

				//get the image rectangle
				CSize	size_cur = m_size_client;

				CRect	rectImage;

				rectImage.left = int(ceil( 0*m_fZoom-m_pointScroll.x));
				rectImage.right = int(floor((size_cur.cx)*m_fZoom-m_pointScroll.x + .5));
				rectImage.top = int(ceil( 0*m_fZoom-m_pointScroll.y));
				rectImage.bottom = int(floor((size_cur.cy)*m_fZoom-m_pointScroll.y + .5));

				rect &= rectImage;
				CRect *pRect = (CRect*)pdata;
				rect.bottom -= 30;
				pRect->bottom = min(pRect->bottom, rect.bottom);
				pRect->right = min(pRect->right, rect.right);
			}
			if( _IsCenterFrameVisible( ) )
			{
				CRect *pRect = (CRect*)pdata;
				pRect->right = pRect->left;
				pRect->bottom = pRect->top;
			}
		}		
	}
	else if( !strcmp( pszEvent, szEventNewSettingsView ) )
	{	// [vanek] 24.10.2003
		InvalidateRect( 0 );
	}

	DoPocessNotify(pszEvent, punkHit, punkFrom, pdata, cbSize );
}



HBRUSH CImageSourceView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CViewBase::OnCtlColor(pDC, pWnd, nCtlColor);
	OutputDebugString("CImageSourceView::OnCtlColor\n");
	// TODO: Change any attributes of the DC here
	
	// TODO: Return a different brush if the default is not desired
	//pDC->SelectStockObject( NULL_BRUSH );
	if(nCtlColor != CTLCOLOR_EDIT)
	{
		return hbr; 
	}

	int transparent = ::GetValueInt(GetAppUnknown(), "Editor", "EditTransparentCommentText", 0);


//	transparent = 0;

	COLORREF color;
	color = ::GetValueColor(GetAppUnknown(), "Editor", "EditCommentText_Fore", RGB(0,255,0));	
	m_colors[0] = GetRValue(color);
	m_colors[1] = GetGValue(color);
	m_colors[2] = GetBValue(color);
	color = ::GetValueColor(GetAppUnknown(), "Editor", "EditCommentText_Back", RGB(127,255,127));	
	m_backcolors[0] = GetRValue(color);
	m_backcolors[1] = GetGValue(color);
	m_backcolors[2] = GetBValue(color);
			
	pDC->SetTextColor(RGB(m_colors[0], m_colors[1], m_colors[2]));

	//pDC->SelectObject(&m_fontEdit);
	
	if(transparent == 1)
	{
		pDC->SetBkMode( TRANSPARENT);
		return (HBRUSH)::GetStockObject( NULL_BRUSH );
	}
	else
	{
		pDC->SetBkMode(OPAQUE);
		pDC->SetBkColor( RGB( m_backcolors[0], m_backcolors[1], m_backcolors[2] ) );
		return (HBRUSH)::GetStockObject( NULL_BRUSH );
	}
}

void CImageSourceView::_SetSelection( IUnknown *punkObjectSelection )
{
	if( !m_pframe )return;
	m_pframe->EmptyFrame();

	IViewCtrlPtr ptr = GetControllingUnknown();
	if( ptr == 0 )
	{
		if( ::GetObjectKey( m_imageSelection ) == 
			::GetObjectKey( punkObjectSelection ) )
			return;

		CRect	rect = m_imageSelection.GetRect();
		rect = ConvertToWindow( GetControllingUnknown(), rect );
 		if( GetSafeHwnd() )InvalidateRect( rect );

		m_imageSelection = punkObjectSelection;

//		if((m_dwShowFlags & isfSourceImage) == isfSourceImage)
		  m_pframe->SelectObject( m_imageSelection );
		rect = m_imageSelection.GetRect();
		rect = ConvertToWindow( GetControllingUnknown(), rect );
 		if( GetSafeHwnd() )InvalidateRect( rect );
	}
}


void CImageSourceView::OnMouseMove(UINT nFlags, CPoint point) 
{
	::DispatchMouseToCtrl( GetControllingUnknown(), ::ConvertToClient( GetControllingUnknown(), point ) );
	if( m_pImagePane )
	{
		CPoint	pointClient = ::ConvertToClient( GetControllingUnknown(), point );	
		m_pImagePane->SetPosition( pointClient );
	}
}

bool CImageSourceView::_DrawObjectClass(CDC &dc, IUnknown * punkObject, CRect & rcInvalidate, CRect & rcObj, COLORREF cr)
{
	if (!punkObject || !m_bShowName)
		return true;

//	if (!rcInvalidate.IsRectEmpty() && !rcInvalidate.IntersectRect(rcInvalidate, rcObj))
//		return true;

	COLORREF OldColor = dc.SetTextColor(cr);
	COLORREF clBlack = RGB(0,0,0);

	if( m_bAsClassColor )
		m_clWhite = cr;

	int nOldMode = dc.SetBkMode(TRANSPARENT);
	long	lclass = _get_object_class( ICalcObjectPtr( punkObject ) );
	CString strClass = _get_class_name( lclass );

	if (strClass.IsEmpty())
		return false;

	CPoint ptc;
	CRect textRc = NORECT;
	CRect outRect;

	bool bNeedRestore = false;
	int nNamePos = m_nNamePos;
	int nNumPos = m_nNumberPos;
	dc.DrawText(strClass, &textRc, DT_CALCRECT|DT_CENTER|DT_TOP|DT_SINGLELINE);

	double Del = m_bFromPrint ? m_fZoom : 1;

	long	lpos = m_object_coords.find( (long)punkObject );

	if( !lpos )
	{
		CObjectWrp object(punkObject, true);
		CImageWrp  image(object.GetImage(), false);
		CRect imageRc = image.GetRect();

		if (imageRc.IsRectEmpty())
		{
			bNeedRestore = true;
			m_nNamePos = 0;
			m_nNumberPos = 0;
		}

		long nSz = 0;
		LPPOINT Pts = 0;

		{				 
			IMeasureObjectPtr sptrMO(punkObject);
			INamedDataObject2Ptr ptrNDO = sptrMO;
			if( ptrNDO == 0 )		return 0;


			long lPos = 0;
			ptrNDO->GetFirstChildPosition( &lPos );
			while( lPos )
			{
				IUnknown* punkChromo = 0;
				ptrNDO->GetNextChild( &lPos, &punkChromo );
				if( !punkChromo )
					continue;

				IChromosomePtr ptrChromo = punkChromo;
				punkChromo->Release();
				punkChromo = 0;

				if( ptrChromo != 0 )
				{
					ptrChromo->GetAxisSize( AT_LongAxis, &nSz, 0 );

					if( nSz != 0 )
					{
						Pts = new POINT[nSz];

						ptrChromo->GetAxis( AT_LongAxis, Pts );
					}
					break;
				}
			}
		}
		
		switch (m_nNamePos)
		{
		case 1: // left-top
			ptc = CPoint(rcObj.left, rcObj.top);
			outRect.left = long( ptc.x - textRc.Width() / Del );
			break;

		case 2: // right-top
			ptc = CPoint(rcObj.right, rcObj.top);
			outRect.left = ptc.x;
			break;

		case 3: // left-bottom
			ptc = CPoint(rcObj.left, rcObj.bottom);
			outRect.left = long( ptc.x - textRc.Width() / Del );
			break;

		case 4: // right-bottom
			ptc = CPoint(rcObj.right, rcObj.bottom);
			outRect.left = ptc.x;
			break;

		case 5: // chromo head
			if( Pts )
			{
				int x = Pts[int(nSz*0.5f)].x - Pts[int(nSz*0.05f)].x;
				int y = Pts[int(nSz*0.5f)].y - Pts[int(nSz*0.05f)].y;

				ptc = CPoint( rcObj.left + Pts[int(nSz*0.05f)].x/* * m_fZoom*/, 
				rcObj.top + Pts[int(nSz*0.05f)].y/*  * m_fZoom */);
				outRect.left = long( ptc.x + ( ( abs( x ) > abs( y ) ? x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2 / Del );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
			}
			break;

		case 6: // chromo tail
			if( Pts )
			{
				int x = Pts[int(nSz*0.95f)].x - Pts[int(nSz*0.7f)].x;
				int y = Pts[int(nSz*0.95f)].y - Pts[int(nSz*0.7f)].y;

				ptc = CPoint( rcObj.left + Pts[int(nSz*0.95f)].x, 
				rcObj.top + Pts[int(nSz*0.95f)].y );
				outRect.left = long( ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2 / Del );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
			}
			break;

		case 7: // chromo center
			if( Pts )
			{
				int x = Pts[int(nSz*0.3f)].x - Pts[int(nSz*0.05f)].x;
				int y = Pts[int(nSz*0.3f)].y - Pts[int(nSz*0.05f)].y;

				ptc = CPoint( rcObj.left + Pts[int(nSz*0.5f)].x/* * m_fZoom*/, 
				rcObj.top + Pts[int(nSz*0.5f)].y/* * m_fZoom*/);
				outRect.left = long( ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2 / Del );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
			}
			break;
		case 8: // chromo right
			if( Pts )
			{
				int nID = 0;
				int x = Pts[0].x;

				for( int i = nSz - 1; i < nSz; i++ )
				{
					if( Pts[i].x > x )
					{
						nID = i;
						x = Pts[i].x;
					}
				}

				int dy = 0;

				if( nID )
					dy = Pts[nID].y - Pts[nID-1].y;
				else
					dy = Pts[nID + 1].y - Pts[nID].y;

				ptc = CPoint( rcObj.left + Pts[nID].x, 
							  rcObj.top  + Pts[nID].y);
				
				outRect.left = ptc.x;

				if( dy < 0 )
					outRect.top = ptc.y;
				else
					outRect.top = long( ptc.y - textRc.Height() / Del );

				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();
				_fix_letter( &outRect, textRc, punkObject );

			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
				outRect.top = ptc.y;
			}
			break;

		case 9: // chromo left
			if( Pts )
			{
				int nID = 0;
				int x = Pts[0].x;

				for( int i = nSz - 1; i < nSz; i++ )
				{
					if( Pts[i].x < x )
					{
						nID = i;
						x = Pts[i].x;
					}
				}

				int dy = 0;

				if( nID )
					dy = Pts[nID].y - Pts[nID-1].y;
				else
					dy = Pts[nID + 1].y - Pts[nID].y;

				ptc = CPoint( rcObj.left + Pts[nID].x , 
							  rcObj.top  + Pts[nID].y );

				outRect.left = long( ptc.x - textRc.Width() / Del );

				if( dy < 0 )
					outRect.top = ptc.y;
				else
					outRect.top = long( ptc.y - textRc.Height() / Del );

				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();
				_fix_letter( &outRect, textRc, punkObject );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
				outRect.top = ptc.y;
			}
			break;

		default: //case 0: // center
			ptc = CPoint(int( rcObj.left + rcObj.Width() / 2), int(rcObj.top + rcObj.Height() / 2  - textRc.Height() / 2 / Del));
			outRect.left = long( ptc.x - textRc.Width() / 2 / Del );
			break;
		}

		if( Pts )
			delete []Pts;

		if( m_nNamePos != 8 && m_nNamePos != 9 )
			outRect.top = ptc.y;

//		if( m_nNamePos > 0 && m_nNamePos < 5 )
//			_fix_letter( &outRect, textRc, punkObject );

		outRect.right = outRect.left + textRc.Width();
		outRect.bottom = outRect.top + textRc.Height();

		m_object_coords.set( outRect, (long)punkObject );


	}
	else
		outRect = m_object_coords.get( lpos );

	outRect.left   = long( outRect.left * m_fZoom	- m_pointScroll.x );
	outRect.top    = long( outRect.top  * m_fZoom	- m_pointScroll.y );
	
	outRect.right  = outRect.left + textRc.Width();
	outRect.bottom = outRect.top  + textRc.Height();

	// draw black border for text 
//	dc.SetTextColor(clBlack);

	// draw text w/ white color
	dc.SetTextColor(m_clWhite);
	dc.DrawText(strClass, &outRect, DT_CENTER|DT_TOP|DT_SINGLELINE);

	nOldMode = dc.SetBkMode(nOldMode);
	dc.SetTextColor(OldColor);
	if (bNeedRestore)
	{	
		m_nNamePos = nNamePos;
		m_nNumberPos = nNumPos;
	}
	return true;
}


/*void CalcObjectExtraRect( IUnknown *punkObject, CRect &rectObject, CRect *prectText, CRect *prectClass )
{
	if( prectText )*prectText = rectObject;
	if( prectClass )*prectText = prectClass;


}*/

bool CImageSourceView::_DrawObjectNumber(CDC &dc, IUnknown * punkObject, CRect & rcInvalidate, CRect & rcObj, COLORREF cr)
{
	if (!punkObject || !m_bShowNumber)
		return true;

	long   nNumber = 1;
	IMeasureObjectPtr sptrMO(punkObject);
	if(sptrMO != 0)sptrMO->GetDrawingNumber(&nNumber);

	// [vanek] - 23.12.2003 : set "Arial" font
	CFont* pOldFont = dc.GetCurrentFont();
	LOGFONT lf;
	::ZeroMemory( &lf, sizeof(LOGFONT) );
	if( pOldFont )
		pOldFont->GetLogFont( &lf );
	else
		::GetObject( ::GetStockObject( ANSI_VAR_FONT ), sizeof( lf ), &lf );
	
	strcpy( lf.lfFaceName, "Arial" );
	// [vanek] BT2000: 3560 - 12.01.2004
	{
		// [vanek] BT2000:3814 используем экранное dc для пересчета - 06.07.2004
		CClientDC	dc_display( 0 );
        lf.lfHeight = -MulDiv( ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "N_And_N_FontSize", 10 ),
								dc_display.GetDeviceCaps( LOGPIXELSY ), 72);

        lf.lfHeight = long( lf.lfHeight * m_fZoom / m_fOldZoom );
        if( !lf.lfHeight )
			lf.lfHeight = -1;
	}
		
	// [vanek] SBT: 755 - 05.03.2004
	lf.lfWidth = 0;

    CFont fontNew;
	fontNew.CreateFontIndirect( &lf );		
    dc.SelectObject( &fontNew );

//	if (!rcInvalidate.IsRectEmpty() && !rcInvalidate.IntersectRect(rcInvalidate, rcObj))
//		return true;
	
	double Del = m_bFromPrint ? m_fZoom : 1;

	COLORREF OldColor = dc.SetTextColor(cr);
	COLORREF clBlack = RGB(0,0,0);

	if( m_bAsClassColor )
		m_clWhite = cr;

	
	int nOldMode = dc.SetBkMode(TRANSPARENT);
	CString strNumber;
	strNumber.Format("%d", nNumber);

	CPoint ptc;
	CRect textRc = NORECT;
	CRect outRect;

	bool bNeedRestore = false;
	int nNumPos = m_nNumberPos;
	int nNamePos = m_nNamePos;

	dc.DrawText(strNumber, &textRc, DT_CALCRECT|DT_CENTER|DT_TOP|DT_SINGLELINE);

	long	lpos = m_object_coords_num.find( (long)punkObject );

	if( !lpos )
	{
		CObjectWrp object(punkObject, true);
		CImageWrp  image(object.GetImage(), false);
		CRect imageRc = image.GetRect();

		if (imageRc.IsRectEmpty())
		{
			bNeedRestore = true;
			m_nNamePos = 0;
			m_nNumberPos = 0;
		}

		long nSz = 0;
		LPPOINT Pts = 0;

		{
			IMeasureObjectPtr sptrMO(punkObject);
			INamedDataObject2Ptr ptrNDO = sptrMO;
			if( ptrNDO == 0 )		return 0;


			long lPos = 0;
			ptrNDO->GetFirstChildPosition( &lPos );
			while( lPos )
			{
				IUnknown* punkChromo = 0;
				ptrNDO->GetNextChild( &lPos, &punkChromo );
				if( !punkChromo )
					continue;

				IChromosomePtr ptrChromo = punkChromo;
				punkChromo->Release();
				punkChromo = 0;

				if( ptrChromo != 0 )
				{
					ptrChromo->GetAxisSize( AT_LongAxis, &nSz, 0 );

					if( nSz != 0 )
					{
						Pts = new POINT[nSz];

						ptrChromo->GetAxis( AT_LongAxis, Pts );
					}
					break;
				}
			}
		}
		
		switch ( m_nNumberPos )
		{
		case 1: // left-top
			ptc = CPoint(rcObj.left, rcObj.top);
			outRect.left = long( ptc.x - textRc.Width() / Del );
			break;

		case 2: // right-top
			ptc = CPoint(rcObj.right, rcObj.top);
			outRect.left = ptc.x;
			break;

		case 3: // left-bottom
			ptc = CPoint(rcObj.left, rcObj.bottom);
			outRect.left = long( ptc.x - textRc.Width() / Del );
			break;

		case 4: // right-bottom
			ptc = CPoint(rcObj.right, rcObj.bottom);
			outRect.left = ptc.x;
			break;

		case 5: // chromo head
			if( Pts )
			{
				int x = Pts[int(nSz*0.5f)].x - Pts[int(nSz*0.05f)].x;
				int y = Pts[int(nSz*0.5f)].y - Pts[int(nSz*0.05f)].y;

				ptc = CPoint( rcObj.left + Pts[int(nSz*0.05f)].x/* * m_fZoom*/, 
				rcObj.top + Pts[int(nSz*0.05f)].y/*  * m_fZoom */);
				outRect.left = long( ptc.x + ( ( abs( x ) > abs( y ) ? x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2 / Del );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
			}
			break;

		case 6: // chromo tail
			if( Pts )
			{
				int x = Pts[int(nSz*0.95f)].x - Pts[int(nSz*0.7f)].x;
				int y = Pts[int(nSz*0.95f)].y - Pts[int(nSz*0.7f)].y;

				ptc = CPoint( rcObj.left + Pts[int(nSz*0.95f)].x, 
				rcObj.top + Pts[int(nSz*0.95f)].y );
				outRect.left = long( ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2  / Del );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
			}
			break;

		case 7: // chromo center
			if( Pts )
			{
				int x = Pts[int(nSz*0.3f)].x - Pts[int(nSz*0.05f)].x;
				int y = Pts[int(nSz*0.3f)].y - Pts[int(nSz*0.05f)].y;

				ptc = CPoint( rcObj.left + Pts[int(nSz*0.5f)].x/* * m_fZoom*/, 
				rcObj.top + Pts[int(nSz*0.5f)].y/* * m_fZoom*/);
				outRect.left = long( ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2 / Del );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del ) ;
			}
			break;
		case 8: // chromo right
			if( Pts )
			{
				int nID = 0;
				int x = Pts[0].x;

				for( int i = nSz - 1; i < nSz; i++ )
				{
					if( Pts[i].x > x )
					{
						nID = i;
						x = Pts[i].x;
					}
				}

				int dy = 0;

				if( nID )
					dy = Pts[nID].y - Pts[nID-1].y;
				else
					dy = Pts[nID + 1].y - Pts[nID].y;

				ptc = CPoint( rcObj.left + Pts[nID].x, 
							  rcObj.top  + Pts[nID].y);
				
				outRect.left = ptc.x;

				if( dy < 0 )
					outRect.top = ptc.y;
				else
					outRect.top = long( ptc.y - textRc.Height() / Del );

				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();
				_fix_letter( &outRect, textRc, punkObject );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
				outRect.top = ptc.y;
			}
			break;

		case 9: // chromo left
			if( Pts )
			{
				int nID = 0;
				int x = Pts[0].x;

				for( int i = nSz - 1; i < nSz; i++ )
				{
					if( Pts[i].x < x )
					{
						nID = i;
						x = Pts[i].x;
					}
				}

				int dy = 0;

				if( nID )
					dy = Pts[nID].y - Pts[nID-1].y;
				else
					dy = Pts[nID + 1].y - Pts[nID].y;

				ptc = CPoint( rcObj.left + Pts[nID].x , 
							  rcObj.top  + Pts[nID].y );

				
				outRect.left = long( ptc.x - textRc.Width() / Del );

				if( dy < 0 )
					outRect.top = ptc.y;
				else
					outRect.top = long( ptc.y - textRc.Height() / Del );

				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();
				_fix_letter( &outRect, textRc, punkObject );
			}
			else
			{
				ptc = CPoint(rcObj.left, rcObj.top);
				outRect.left = long( ptc.x - textRc.Width() / Del );
				outRect.top = ptc.y;
			}
			break;

		default: //case 0: // center
			ptc = CPoint( int( rcObj.left + rcObj.Width() / 2 ),  int( rcObj.top + rcObj.Height() / 2  - textRc.Height() / 2 / Del ) );
			outRect.left = long( ptc.x - textRc.Width() / 2 / Del );
			break;
		}

		if( Pts )
			delete []Pts;

		if( m_nNumberPos != 8 && m_nNumberPos != 9 )
			outRect.top = ptc.y;

//		if( m_nNumberPos > 0 && m_nNumberPos < 5 )
//			_fix_letter( &outRect, textRc, punkObject );

		outRect.right = outRect.left + textRc.Width();
		outRect.bottom = outRect.top + textRc.Height();
	
		m_object_coords_num.set( outRect, (long)punkObject );
	}
	else
		outRect = m_object_coords_num.get( lpos );
	
	outRect.left   = long(  outRect.left * m_fZoom - m_pointScroll.x );
	outRect.top    = long( outRect.top  * m_fZoom - m_pointScroll.y );
	outRect.right  = outRect.left           + textRc.Width();
	outRect.bottom = outRect.top            + textRc.Height();
	

	// draw black border for text 
	// draw text w/ white color
	dc.SetTextColor(m_clWhite);
	dc.DrawText(strNumber, &outRect, DT_CENTER|DT_TOP|DT_SINGLELINE);

	if( pOldFont )
		dc.SelectObject( pOldFont );

	dc.SetTextColor(OldColor);
	nOldMode = dc.SetBkMode(nOldMode);

//	dc.SetPixel( ptc, RGB(255,0,255) );
	if (bNeedRestore)
	{	
		m_nNamePos = nNamePos;
		m_nNumberPos = nNumPos;
	}
	return true;
}

CRect CImageSourceView::GetObjectAdditionRect(IUnknown * punkObject)
{
	if (!CheckInterface(punkObject, IID_IMeasureObject))
		return GetObjectRect(punkObject);

	return _GetMeasObjectRect(punkObject, true);
}

CRect CImageSourceView::_GetObjectClassAndNumberRect(IUnknown * punkObject, CRect & rc, BOOL bFromSetOffset )
{
	CRect rect = rc;
	if (!punkObject)
		return rect;

	if( !GetSafeHwnd() )
		return rect;

	bool bNeedRestore = false;
	int nNamePos = m_nNamePos;
	int nNumPos = m_nNumberPos;

	CObjectWrp object(punkObject, true);
	CImageWrp  image(object.GetImage(), false);

	if (image == 0)
	{
		bNeedRestore = true;
		m_nNamePos = 0;
		m_nNumberPos = 0;
	}

	CWindowDC dc(this);

	double Del = m_bFromPrint ? m_fZoom : 1;
	if (m_bShowName)
	{
		long	lclass = _get_object_class( ICalcObjectPtr( punkObject ) );
		CString strClass = _get_class_name( lclass );

		if (!strClass.IsEmpty())
		{
			// A.M. fix SBT 1372. Fonts was different in _DrawObjectClass, called from CImageSourceView::OnPaint
			// and in this case.
			CFont* pOldFont = dc.GetCurrentFont();
			LOGFONT lf;
			::ZeroMemory( &lf, sizeof(LOGFONT) );
			if( pOldFont )
				pOldFont->GetLogFont( &lf );

			strcpy( lf.lfFaceName, "Arial" );
			CFont fontNew;
			fontNew.CreateFontIndirect( &lf );		
			dc.SelectObject( &fontNew );


			CPoint ptc;
			CRect textRc = NORECT;
			CRect outRect;
			dc.DrawText(strClass, &textRc, DT_CALCRECT|DT_CENTER|DT_TOP|DT_SINGLELINE);
			CRect rcObj = rc;

			dc.SelectObject( pOldFont );


			long	lpos = m_object_coords.find( (long)punkObject );
			
			if( !lpos )
			{
				CObjectWrp object(punkObject, true);
				CImageWrp  image(object.GetImage(), false);
				CRect imageRc = rc;

				if (imageRc.IsRectEmpty())
				{
					bNeedRestore = true;
					m_nNamePos = 0;
					m_nNumberPos = 0;
				}

				long nSz = 0;
				LPPOINT Pts = 0;

				{
					IMeasureObjectPtr sptrMO(punkObject);
					INamedDataObject2Ptr ptrNDO = sptrMO;
					if( ptrNDO == 0 )		return 0;


					long lPos = 0;
					ptrNDO->GetFirstChildPosition( &lPos );
					while( lPos )
					{
						IUnknown* punkChromo = 0;
						ptrNDO->GetNextChild( &lPos, &punkChromo );
						if( !punkChromo )
							continue;

						IChromosomePtr ptrChromo = punkChromo;
						punkChromo->Release();
						punkChromo = 0;

						if( ptrChromo != 0 )
						{
							ptrChromo->GetAxisSize( AT_LongAxis, &nSz, 0 );

							if( nSz != 0 )
							{
								Pts = new POINT[nSz];

								ptrChromo->GetAxis( AT_LongAxis, Pts );
							}
							break;
						}
					}
				}
				
				switch (m_nNamePos)
				{
				case 1: // left-top
					ptc = CPoint(rcObj.left, rcObj.top);
					outRect.left = ptc.x - textRc.Width();
					break;

				case 2: // right-top
					ptc = CPoint(rcObj.right, rcObj.top);
					outRect.left = ptc.x;
					break;

				case 3: // left-bottom
					ptc = CPoint(rcObj.left, rcObj.bottom);
					outRect.left = ptc.x - textRc.Width();
					break;

				case 4: // right-bottom
					ptc = CPoint(rcObj.right, rcObj.bottom);
					outRect.left = ptc.x;
					break;

				case 5: // chromo head
					if( Pts )
					{
						int x = Pts[int(nSz*0.5f)].x - Pts[int(nSz*0.05f)].x;
						int y = Pts[int(nSz*0.5f)].y - Pts[int(nSz*0.05f)].y;

						ptc = CPoint( rcObj.left + Pts[int(nSz*0.05f)].x/* * m_fZoom*/, 
						rcObj.top + Pts[int(nSz*0.05f)].y/*  * m_fZoom */);
						outRect.left = ptc.x + ( ( abs( x ) > abs( y ) ? x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2/* * m_fZoom*/;
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = ptc.x - textRc.Width();
					}
					break;

				case 6: // chromo tail
					if( Pts )
					{
						int x = Pts[int(nSz*0.95f)].x - Pts[int(nSz*0.7f)].x;
						int y = Pts[int(nSz*0.95f)].y - Pts[int(nSz*0.7f)].y;

						ptc = CPoint( rcObj.left + Pts[int(nSz*0.95f)].x, 
						rcObj.top + Pts[int(nSz*0.95f)].y );
						outRect.left = ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2;
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = ptc.x - textRc.Width();
					}
					break;

				case 7: // chromo center
					if( Pts )
					{
						int x = Pts[int(nSz*0.3f)].x - Pts[int(nSz*0.05f)].x;
						int y = Pts[int(nSz*0.3f)].y - Pts[int(nSz*0.05f)].y;

						ptc = CPoint( rcObj.left + Pts[int(nSz*0.5f)].x/* * m_fZoom*/, 
						rcObj.top + Pts[int(nSz*0.5f)].y/* * m_fZoom*/);
						outRect.left = ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2/* * m_fZoom*/;
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = ptc.x - textRc.Width();
					}
					break;
				case 8: // chromo right
					if( Pts )
					{
						int nID = 0;
						int x = Pts[0].x;

						for( int i = nSz - 1; i < nSz; i++ )
						{
							if( Pts[i].x > x )
							{
								nID = i;
								x = Pts[i].x;
							}
						}

						int dy = 0;

						if( nID )
							dy = Pts[nID].y - Pts[nID-1].y;
						else
							dy = Pts[nID + 1].y - Pts[nID].y;

						ptc = CPoint( rcObj.left + Pts[nID].x/* * m_fZoom*/, 
									  rcObj.top + Pts[nID].y/* * m_fZoom */);

						ptc = CPoint( rcObj.left + Pts[nID].x, 
									  rcObj.top  + Pts[nID].y);
						
						outRect.left = ptc.x;

						if( dy < 0 )
							outRect.top = ptc.y;
						else
							outRect.top = long( ptc.y - textRc.Height() / Del );

						outRect.right = outRect.left + textRc.Width();
						outRect.bottom = outRect.top + textRc.Height();
						_fix_letter( &outRect, textRc, punkObject );
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = ptc.x - textRc.Width();
						outRect.top = ptc.y;
					}
					break;

				case 9: // chromo left
					if( Pts )
					{
						int nID = 0;
						int x = Pts[0].x;

						for( int i = nSz - 1; i < nSz; i++ )
						{
							if( Pts[i].x < x )
							{
								nID = i;
								x = Pts[i].x;
							}
						}

						int dy = 0;

						if( nID )
							dy = Pts[nID].y - Pts[nID-1].y;
						else
							dy = Pts[nID + 1].y - Pts[nID].y;

						ptc = CPoint( rcObj.left + Pts[nID].x , 
									  rcObj.top  + Pts[nID].y );

						
						outRect.left = long( ptc.x - textRc.Width() / Del );

						if( dy < 0 )
							outRect.top = ptc.y;
						else
							outRect.top = long( ptc.y - textRc.Height() / Del );

						outRect.right = outRect.left + textRc.Width();
						outRect.bottom = outRect.top + textRc.Height();

						_fix_letter( &outRect, textRc, punkObject );
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = long( ptc.x - textRc.Width() / Del );
						outRect.top = ptc.y;
					}
					break;

				default: //case 0: // center
					ptc = CPoint( int( rcObj.left + rcObj.Width() / 2 ),  int( rcObj.top + rcObj.Height() / 2  - textRc.Height() / 2 / Del));
					outRect.left = ptc.x - textRc.Width() / 2;
					break;
				}

				if( Pts )
				{ 
					delete []Pts; 
					Pts = 0; 
				}

//				if( m_nNamePos > 0 && m_nNamePos < 5 )
				if( m_nNamePos != 8 && m_nNamePos != 9 )
					outRect.top = ptc.y;

//				if( m_nNamePos > 0 && m_nNamePos < 5 )
//					_fix_letter( &outRect, textRc, punkObject );

				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();

				m_object_coords.set( outRect, (long)punkObject );
			}
			else 
				outRect = m_object_coords.get( lpos );

			if( !bFromSetOffset )
			{
				outRect.left   = long( outRect.left   * m_fZoom - m_pointScroll.x );
				outRect.top    = long( outRect.top    * m_fZoom - m_pointScroll.y );
				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();
		
			}
			rect.UnionRect(rect, outRect);
		}
	}

	if (m_bShowNumber)
	{
		long nNumber = 1;
		IMeasureObjectPtr sptrMO(punkObject);
		if(sptrMO != 0)sptrMO->GetDrawingNumber(&nNumber);

		CString strNumber;
		strNumber.Format("%d", (int)nNumber);

		// [vanek] SBT: 754 - same as in _DrawObjectNumber
		CFont* pOldFont = dc.GetCurrentFont();
		LOGFONT lf;
		::ZeroMemory( &lf, sizeof(LOGFONT) );
		if( pOldFont )
			pOldFont->GetLogFont( &lf );
		else
			::GetObject( ::GetStockObject( ANSI_VAR_FONT ), sizeof( lf ), &lf );
		
		strcpy( lf.lfFaceName, "Arial" );
		// [vanek] BT2000:3814 используем экранное dc для пересчета - 06.07.2004
		{
			CClientDC	dc_display( 0 );
			lf.lfHeight = -MulDiv( ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "N_And_N_FontSize", 10 ),
									dc_display.GetDeviceCaps( LOGPIXELSY ), 72);	
		}

		lf.lfWidth = 0;
        
		CFont fontNew;
		fontNew.CreateFontIndirect( &lf );		
		dc.SelectObject( &fontNew );

		if (!strNumber.IsEmpty())
		{
			CPoint ptc;
			CRect textRc = NORECT;
			CRect outRect;
			CRect rcObj = rc;

			dc.DrawText(strNumber, &textRc, DT_CALCRECT|DT_CENTER|DT_TOP|DT_SINGLELINE);

			long	lpos = m_object_coords_num.find( (long)punkObject );

			if( !lpos )
			{
				CObjectWrp object(punkObject, true);
				CImageWrp  image(object.GetImage(), false);
				CRect imageRc = rc;

				if (imageRc.IsRectEmpty())
				{
					bNeedRestore = true;
					m_nNamePos = 0;
					m_nNumberPos = 0;
				}

				long nSz = 0;
				LPPOINT Pts = 0;

				{
					IMeasureObjectPtr sptrMO(punkObject);
					INamedDataObject2Ptr ptrNDO = sptrMO;
					if( ptrNDO == 0 )		return 0;


					long lPos = 0;
					ptrNDO->GetFirstChildPosition( &lPos );
					while( lPos )
					{
						IUnknown* punkChromo = 0;
						ptrNDO->GetNextChild( &lPos, &punkChromo );
						if( !punkChromo )
							continue;

						IChromosomePtr ptrChromo = punkChromo;
						punkChromo->Release();
						punkChromo = 0;

						if( ptrChromo != 0 )
						{
							ptrChromo->GetAxisSize( AT_LongAxis, &nSz, 0 );

							if( nSz != 0 )
							{
								Pts = new POINT[nSz];

								ptrChromo->GetAxis( AT_LongAxis, Pts );
							}
							break;
						}
					}
				}
				
				switch (m_nNumberPos)
				{
				case 1: // left-top
					ptc = CPoint(rcObj.left, rcObj.top);
					outRect.left = ptc.x - textRc.Width();
					outRect.top = ptc.y;
					break;

				case 2: // right-top
					ptc = CPoint(rcObj.right, rcObj.top);
					outRect.left = ptc.x;
					outRect.top = ptc.y;
					break;

				case 3: // left-bottom
					ptc = CPoint(rcObj.left, rcObj.bottom);
					outRect.left = ptc.x - textRc.Width();
					outRect.top = ptc.y;
					break;

				case 4: // right-bottom
					ptc = CPoint(rcObj.right, rcObj.bottom);
					outRect.left = ptc.x;
					outRect.top = ptc.y;
					break;

				case 5: // chromo head
					if( Pts )
					{
						int x = Pts[int(nSz*0.5f)].x - Pts[int(nSz*0.05f)].x;
						int y = Pts[int(nSz*0.5f)].y - Pts[int(nSz*0.05f)].y;

						ptc = CPoint( rcObj.left + Pts[int(nSz*0.05f)].x/* * m_fZoom*/, 
						rcObj.top + Pts[int(nSz*0.05f)].y/*  * m_fZoom */);
						outRect.left = ptc.x + ( ( abs( x ) > abs( y ) ? x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2/* * m_fZoom*/;
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = ptc.x - textRc.Width();
					}
					break;

				case 6: // chromo tail
					if( Pts )
					{
						int x = Pts[int(nSz*0.95f)].x - Pts[int(nSz*0.7f)].x;
						int y = Pts[int(nSz*0.95f)].y - Pts[int(nSz*0.7f)].y;

						ptc = CPoint( rcObj.left + Pts[int(nSz*0.95f)].x, 
						rcObj.top + Pts[int(nSz*0.95f)].y );
						outRect.left = ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2;
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = ptc.x - textRc.Width();
					}
					break;

				case 7: // chromo center
					if( Pts )
					{
						int x = Pts[int(nSz*0.3f)].x - Pts[int(nSz*0.05f)].x;
						int y = Pts[int(nSz*0.3f)].y - Pts[int(nSz*0.05f)].y;

						ptc = CPoint( rcObj.left + Pts[int(nSz*0.5f)].x/* * m_fZoom*/, 
						rcObj.top + Pts[int(nSz*0.5f)].y/* * m_fZoom*/);
						outRect.left = ptc.x + ( ( abs( x ) > abs( y ) ? -x : -y ) > 0 ? 1 : -1 ) * textRc.Width() / 2/* * m_fZoom*/;
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = ptc.x - textRc.Width();
					}
					break;
				case 8: // chromo right
					if( Pts )
					{
						int nID = 0;
						int x = Pts[0].x;

						for( int i = nSz - 1; i < nSz; i++ )
						{
							if( Pts[i].x > x )
							{
								nID = i;
								x = Pts[i].x;
							}
						}

						int dy = 0;

						if( nID )
							dy = Pts[nID].y - Pts[nID-1].y;
						else
							dy = Pts[nID + 1].y - Pts[nID].y;

						ptc = CPoint( rcObj.left + Pts[nID].x, 
									  rcObj.top  + Pts[nID].y);
						
						outRect.left = ptc.x;

						if( dy < 0 )
							outRect.top = ptc.y;
						else
							outRect.top = long( ptc.y - textRc.Height() / Del );

						outRect.right = outRect.left + textRc.Width();
						outRect.bottom = outRect.top + textRc.Height();
						_fix_letter( &outRect, textRc, punkObject );
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = long( ptc.x - textRc.Width() / Del );
						outRect.top = ptc.y;
					}
					break;

				case 9: // chromo left
					if( Pts )
					{
						int nID = 0;
						int x = Pts[0].x;

						for( int i = nSz - 1; i < nSz; i++ )
						{
							if( Pts[i].x < x )
							{
								nID = i;
								x = Pts[i].x;
							}
						}

						int dy = 0;

						if( nID )
							dy = Pts[nID].y - Pts[nID-1].y;
						else
							dy = Pts[nID + 1].y - Pts[nID].y;

						ptc = CPoint( rcObj.left + Pts[nID].x , 
									  rcObj.top  + Pts[nID].y );

						
						outRect.left = long( ptc.x - textRc.Width() / Del );

						if( dy < 0 )
							outRect.top = ptc.y;
						else
							outRect.top = long( ptc.y - textRc.Height() / Del );

						outRect.right = outRect.left + textRc.Width();
						outRect.bottom = outRect.top + textRc.Height();
						_fix_letter( &outRect, textRc, punkObject );
					}
					else
					{
						ptc = CPoint(rcObj.left, rcObj.top);
						outRect.left = long( ptc.x - textRc.Width() / Del );
						outRect.top = ptc.y;
					}
					break;

				default: //case 0: // center
					ptc = CPoint( int( rcObj.left + rcObj.Width() / 2),  int( rcObj.top + rcObj.Height() / 2  - textRc.Height() / 2 / Del));
					outRect.left = long( ptc.x - textRc.Width() / 2 / Del );
					break;
				}

				if( Pts )
				{
					delete []Pts;
					Pts = 0;
				}

				if( m_nNumberPos != 8 && m_nNumberPos != 9 )
					outRect.top = ptc.y;

//				if( m_nNumberPos > 0 && m_nNumberPos < 5 )
//					_fix_letter( &outRect, textRc, punkObject );

				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();
			
				m_object_coords_num.set( outRect, (long)punkObject );
			}
			else 
				outRect = m_object_coords_num.get( lpos );

			if( !bFromSetOffset )
			{
				outRect.left = long( outRect.left * m_fZoom- m_pointScroll.x );
				outRect.top = long( outRect.top * m_fZoom - m_pointScroll.y );
				outRect.right = outRect.left + textRc.Width();
				outRect.bottom = outRect.top + textRc.Height();
			}
			rect.UnionRect(rect, outRect);
		}

		if( pOldFont )
			dc.SelectObject( pOldFont );
	}


	if (bNeedRestore)
	{	
		m_nNamePos = nNamePos;
		m_nNumberPos = nNumPos;
	}

	return rect;
}

CRect CImageSourceView::_GetMeasObjectRect(IUnknown * punkObject, bool bEx)
{
	CRect rc = NORECT;
	if (!punkObject)
		return rc;

	// get measure object rect
	CObjectWrp object(punkObject, true);
	CImageWrp  image(object.GetImage(), false);
	rc = image.GetRect();

	if (bEx)rc = _GetObjectClassAndNumberRect(punkObject, rc);

	return rc;
}

bool CImageSourceView::_DrawMeasObjects(CDC &dc, CRect rectClip, COLORREF color, IUnknown * punkParent, ObjectDrawState state )
{
	if( state == odsActive || state == odsAnimation )
		return true;

	INamedDataObject2Ptr	ptrNamed = punkParent;

	if( ptrNamed == 0 )return false;

	long lNumber = 0;
	IMeasureObjectPtr	ptrMeasObject( ptrNamed );
	if( ptrMeasObject != 0 )ptrMeasObject->GetDrawingNumber(&lNumber);


	// for all manual meas objects
	long	lpos;
	ptrNamed->GetFirstChildPosition( &lpos );

	CPen	pen(PS_SOLID, 0, color);


	CBrush* pOldBrush = (CBrush*)dc.SelectStockObject(NULL_BRUSH);
	CPen	*pOldPen = (CPen*)dc.SelectObject( &pen );
	/*LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );
	lf.lfCharSet = 1;
	lf.lfHeight = 16*m_fZoom;
	lf.lfWeight = FW_BOLD;
	strcpy( lf.lfFaceName, "Arial" );

	CFont	font;
	font.CreateFontIndirect( &lf );
	CFont	*pOldFont = dc.SelectObject( &font );
	dc.SetBkMode( TRANSPARENT );*/

	//during anmimation paint we shodn't paint every object
	while( lpos )
	{
		dc.SetTextColor( m_clWhite );
		// get next object
		IUnknown * punkObj = 0;
		ptrNamed->GetNextChild( &lpos, &punkObj );

		if( !punkObj )
			continue;

		IDrawObjectPtr ptrDraw = punkObj;
		punkObj->Release();

		if( ptrDraw == 0 )
			continue;

		IDrawObject2Ptr sptrDraw2 = ptrDraw;
		if( sptrDraw2 != 0 )
		{
            // [vanek] BT2000: 3560 - отрисовка ч-з PaintEx, чтобы передавать результирующий зум
			DRAWOPTIONS stDrawOptions = {0};

			// [vanek] BT2000: 3680 - use mask
            stDrawOptions.dZoomDC = m_fZoom / m_fOldZoom;
			stDrawOptions.dwMask = domZoomDC;
			sptrDraw2->PaintEx(dc.GetSafeHdc(), rectClip, GetControllingUnknown(), 0, 0, &stDrawOptions);
		}
		else
			ptrDraw->Paint(dc.GetSafeHdc(), rectClip, GetControllingUnknown(), 0, 0);	

		

		/*if( !lNumber )
			continue;

		//draw Parameter/Number
		IRectObjectPtr	ptrRect = punkObj;
		CRect	rectTotal;
		ptrRect->GetRect( &rectTotal );
		rectTotal = ::ConvertToWindow( GetControllingUnknown(), rectTotal );

		IManualMeasureObjectPtr	ptrM( punkObj );
		if( ptrM == 0 )continue;
		long	lkey = -1;
		ptrM->GetParamInfo( &lkey, 0 );

		ParameterContainer	*pc;
		ptrCont->ParamDefByKey( lkey, &pc );

		if( !pc )continue;



		CString	str;
		CString	str1 = pc->pDescr->bstrUserName;
		str.Format( "%d/%s", lNumber, (const char*)str1 );

		dc.SetTextColor( RGB( 0, 0, 0 ) );
		rectTotal.OffsetRect( -1, -1 );
		dc.DrawText( str, rectTotal, DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );
		rectTotal.OffsetRect( 0, 2 );
		dc.DrawText( str, rectTotal, DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );
		rectTotal.OffsetRect( 2, 0 );
		dc.DrawText( str, rectTotal, DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );
		rectTotal.OffsetRect( 0, -2 );
		dc.DrawText( str, rectTotal, DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );

		dc.SetTextColor( RGB( 255, 255, 255 ) );
		rectTotal.OffsetRect( -1, 1 );
		dc.DrawText( str, rectTotal, DT_SINGLELINE|DT_END_ELLIPSIS|DT_VCENTER|DT_CENTER );*/
	}

	//dc.SelectObject(pOldFont);
	dc.SelectObject(pOldPen);
	dc.SelectObject(pOldBrush);

	return true;
}

COLORREF CImageSourceView::_get_class_color( long lClass )
{
	INamedPropBagPtr sptrBag = m_sptrAttached;
	if( m_sptrAttached == 0 )
	{
		IUnknown *punkList = ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeObjectList );
		m_sptrAttached = sptrBag = punkList;
		if( punkList )
			punkList->Release();
	}
	
	if( sptrBag )
	{
		_variant_t var, var2;
		sptrBag->GetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), &var );
		sptrBag->GetProperty( _bstr_t( VIRTUAL_CLASS_COUNT ), &var2 );

		if( var.vt == VT_I4 && var.lVal == 1L && var2.vt == VT_I4 )
		{
			long	lpos = m_class_colors2.find( lClass );
			if( !lpos )
			{
				COLORREF cr = ::get_smooth_class_color( lClass, var2.lVal, VIRTUAL_STAT_CLASSIFIER );
				m_class_colors2.set( cr, lClass );
				return cr;
			}
			return m_class_colors2.get( lpos );
		}
	}

	long	lpos = m_class_colors.find( lClass );
	if( !lpos )
	{
		COLORREF cr = ::get_class_color( lClass, m_bstr_class_name );
		m_class_colors.set( cr, lClass );
		return cr;
	}
	return m_class_colors.get( lpos );
}

const char	*CImageSourceView::_get_class_name( long nclass )
{
	INamedPropBagPtr sptrBag = m_sptrAttached;
	if( m_sptrAttached == 0 )
	{
		IUnknown *punkList = ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeObjectList );
		m_sptrAttached = sptrBag = punkList;
		if( punkList )
			punkList->Release();
	}

	if( sptrBag )
	{
		_variant_t var;
		sptrBag->GetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), &var );

		if( var.vt == VT_I4 && var.lVal == 1L )
		{
			if( nclass != -1 )
			{
				long	lpos = m_class_names2.find( nclass );
				if( !lpos )
				{
					char *str = _class_buffer();
					sprintf( str, "%ld", nclass + 1 );
					m_class_names2.set( _strdup(str), nclass );
					return str;
				}
				return m_class_names2.get( lpos );
			}
		}
	}

	long	lpos = m_class_names.find( nclass );
	if( !lpos )
	{
		const char *psz = ::get_class_name( nclass, m_bstr_class_name );
		m_class_names.set( _strdup(psz), nclass );
		return psz;
	}
	return m_class_names.get( lpos );
}

long CImageSourceView::_get_object_class( ICalcObject *pcalc )
{
	INamedPropBagPtr sptrBag = m_sptrAttached;
	if( m_sptrAttached == 0 )
	{
		IUnknown *punkList = ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeObjectList );
		m_sptrAttached = sptrBag = punkList;
		if( punkList )
			punkList->Release();
	}

	if( sptrBag )
	{
		_variant_t var, var2;
		sptrBag->GetProperty( _bstr_t( VIRTUAL_STAT_CLASSIFIER ), &var );

		if( var.vt == VT_I4 && var.lVal == 1L )
			return ::get_object_class( pcalc, VIRTUAL_STAT_CLASSIFIER );
	}

	if( !m_bstr_class_name.length() )
		m_bstr_class_name = short_classifiername( 0 );

	return ::get_object_class( pcalc, m_bstr_class_name );
}



void CImageSourceView::DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state )
{
	//draw flags for image
	DWORD	dwDrawFlags = m_nDrawFrameStage==0?0:DrawContours;
	
	if( state == odsActive||state == odsAnimation )
		dwDrawFlags|=DrawSelFrame;

	if( m_bObjectSelected && ((m_dwShowFlags & isfSourceImage) == isfSourceImage) )
		return;


	if( state != odsAnimation && m_nDrawFrameStage != 1)
	{
		if( CheckInterface(punkObject, IID_IMeasureObject) )
		{
			if((m_dwShowFlags & isfPseudoImage) || (m_dwShowFlags & isfFilledOnlyObjects))
				dwDrawFlags |= DrawMask;
			else
				dwDrawFlags |= DrawImage;
		}
		else
		{
			if((m_dwShowFlags & isfPseudoImage) || (m_dwShowFlags & isfFilledImageExceptObjects))
				dwDrawFlags |= DrawMask;
			else
				dwDrawFlags |= DrawImage;
		}
		
	}


	CPen	pen;

	if( state == odsSelected )
		pen.CreatePen( PS_SOLID, 0, RGB( 0, 0, 0 )  );
	
	else if( state == odsActive )
		pen.CreatePen( PS_DOT, 0, RGB( 0, 0, 0 )  );
	else if( state == odsAnimation )
		pen.CreatePen( PS_SOLID, 0, RGB( 255, 255, 255 )  );
	else
		return;

	dc.SetBkMode(OPAQUE);
	dc.SelectObject( &pen );
	dc.SelectStockObject( NULL_BRUSH );
	dc.SetBkColor( RGB( 255, 255, 255 ) );
	
	
	if( CheckInterface(punkObject, IID_IMeasureObject) )
	{
		long	lclass = _get_object_class( ICalcObjectPtr( punkObject ) );
		COLORREF	cr = _get_class_color( lclass );

		CObjectWrp	object( punkObject, true);
		CImageWrp	imageSelection( object.GetImage(), false );

		_DrawImage( dc, imageSelection, rectInvalidate, dwDrawFlags, cr, pbih, pdibBits);
		CRect rcObj = _GetMeasObjectRect(punkObject);
//		CRect rcObj = imageSelection.GetRect();
//		rcObj = ::ConvertToWindow(GetControllingUnknown(), rcObj);

		if (state != odsAnimation && m_nActiveNumber != -1)
		{
			int nNamePos = m_nNamePos;
			int nNumPos = m_nNumberPos;
			bool bNeedRestore = false;
			if (imageSelection == 0)//rcObj.IsRectEmpty())
			{
				rcObj = _GetMeasObjectRect(punkObject);
//				rcObj = ::ConvertToWindow(GetControllingUnknown(), rcObj);
				m_nNamePos = 0;
				m_nNumberPos = 0;
				bNeedRestore = true;
			}

			_DrawObjectNumber(dc, punkObject, rectInvalidate, rcObj, cr);
			_DrawObjectClass(dc, punkObject, rectInvalidate, rcObj, cr);

			if (bNeedRestore)
			{	
				m_nNamePos = nNamePos;
				m_nNumberPos = nNumPos;
			}
		}
	}
	else if( CheckInterface(punkObject, IID_IImage) )
	{
		COLORREF	cr = GetBackgroundColor();
		CImageWrp	imageSelection( punkObject, true );
		_DrawImage( dc, imageSelection, rectInvalidate, dwDrawFlags, cr, pbih, pdibBits);
	}
	else if(CheckInterface(punkObject, IID_IBinaryImage))
	{
		/*long nFillColor = ::GetValueInt( GetAppUnknown(), "Binary", "FillColor", 0);
		nFillColor = max(0, min(255, nFillColor));*/
		DWORD dwBackColor = ::GetValueColor( GetAppUnknown(), "Binary", "BackColor", RGB( 0, 0, 0 ) );
		DrawBinaryRect(punkObject, pbih, pdibBits, rectInvalidate.TopLeft(), rectInvalidate, m_fZoom, m_pointScroll, 0, dwBackColor, TRUE, isfBinaryFore);

		if( state == odsActive || state == odsAnimation )
		{
			CRect	rect = GetObjectRect( punkObject );
			rect = ConvertToWindow( GetControllingUnknown(), rect );
			dc.Rectangle( rect );
		}

	}
	else if( CheckInterface( punkObject, IID_IManualMeasureObject ) )
	{
		IDrawObjectPtr	ptrDraw = punkObject;
		ptrDraw->Paint( dc, rectInvalidate, GetControllingUnknown(), pbih, pdibBits );
	}
	else if( CheckInterface( punkObject, IID_ITrajectory ) )
	{
		IDrawObjectPtr	ptrDraw = punkObject;
		ptrDraw->Paint( dc, rectInvalidate, GetControllingUnknown(), pbih, pdibBits );
	}
	else if( CheckInterface(punkObject, IID_IRectObject ) )
	{
		if( state == odsActive || state == odsAnimation )
		{
			CRect	rect = GetObjectRect( punkObject );
			rect = ConvertToWindow( GetControllingUnknown(), rect );
			dc.Rectangle( rect );
		}
	}
	
	
	if( state == odsSelected )
	{
		CRect	rect = GetObjectRect( punkObject ), rc;

		if( m_bShowName )
		{
			long	lpos = m_object_coords.find( (long)punkObject );
			if( lpos )
			{
				rc = m_object_coords.get( lpos );


				switch (m_nNamePos)
				{
				case 1: // left-top
					rect.left = rc.right;
					break;

				case 2: // right-top
					rect.right = rc.left;
					break;

				case 3: // left-bottom
					rect.left = rc.right;
					rect.bottom = rc.top;
					break;

				case 4: // right-bottom
					rect.right = rc.left;
					rect.bottom = rc.top;
					break;
				}
			}
		}
		if( m_bShowNumber )
		{
			long	lpos = m_object_coords_num.find( (long)punkObject );
			if( lpos )
			{
				rc = m_object_coords_num.get( lpos );

				switch (m_nNumberPos)
				{
				case 1: // left-top
					rect.left = rc.right;
					break;

				case 2: // right-top
					rect.right = rc.left;
					break;

				case 3: // left-bottom
					rect.left = rc.right;
					rect.bottom = rc.top;
					break;

				case 4: // right-bottom
					rect.right = rc.left;
					rect.bottom = rc.top;
					break;
				}
			}
		}

		rect = ConvertToWindow( GetControllingUnknown(), rect );
		dc.DrawFocusRect( rect );
	}

	dc.SelectStockObject( BLACK_PEN );
	dc.SelectStockObject( BLACK_BRUSH );
}

bool CImageSourceView::PtInObject( IUnknown *punkObject, CPoint point )
{
	if( CheckInterface( punkObject, IID_IImage ) )
	{
		CImageWrp	image( punkObject );
		return image.PtInImage( point );
	}
	else
	if( CheckInterface( punkObject, IID_IMeasureObject ) )
	{
		CObjectWrp	object( punkObject );
		CImageWrp	image( object.GetImage(), false );
		return image.PtInImage( point );
	}
	else
		return false;
}

bool CImageSourceView::CanStartTracking( CPoint pt )
{
	if( m_ptrPreviewData == 0 )return true;
	return false;
}


void CImageSourceView::OnDeselect( INamedDataObject2 *punkWillBeDeselectedNow )
{
  if(m_bNeedSetAppropriateView)
    return;


  INamedDataObject2Ptr	sptrN(punkWillBeDeselectedNow);
  if(sptrN == 0) return;
	DWORD	dwFlags = 0;
	sptrN->GetObjectFlags(&dwFlags );

	if((dwFlags & nofStoreByParent) == nofStoreByParent)
  {
    IUnknown* punkParent = 0;
    sptrN->GetParent(&punkParent);
    sptrN = punkParent;
    if(punkParent)
      punkParent->Release();
  }

  CString strKind = ::GetObjectKind(sptrN);
  if(GetMatchType(strKind) == mvNone)
  {
    if(AfxMessageBox(IDS_PROMT_CHANGE_VIEW, MB_YESNO) == IDYES)
    {
      m_bNeedSetAppropriateView = true;
      CString strShowActionParam;
      if(strKind == szTypeObjectList)
        strShowActionParam = ::GetValueString(GetAppUnknown(), "General", "LastUsedObjectView", szImageViewProgID);
      else if(strKind == szTypeBinaryImage)
        strShowActionParam = ::GetValueString(GetAppUnknown(), "General", "LastUsedBinaryView", szBinaryForeViewProgID);
      
      if(!strShowActionParam.IsEmpty())
      {
        IUnknown	*punk = _GetOtherComponent(GetAppUnknown(false), IID_IActionManager);
    	  IActionManagerPtr sptrAM(punk);
        if(punk)
          punk->Release();
        if(sptrAM != 0)
        {
          sptrAM->ExecuteAction(_bstr_t("ShowView"), _bstr_t(CString("\"")+strShowActionParam+CString("\"")), aefDelayed);
          AfxGetMainWnd()->PostMessage(WM_SETFOCUS); //for execute delay action
        }
      }
    }
  }
}

IUnknown *CImageSourceView::GetObjectByPoint( CPoint point)
{
	IUnknown *punk = 0;
//	CWaitCursor wc;

	//check the key state 
	if( (::GetAsyncKeyState(VK_MENU) & 0x8000) != 0 ||
		(::GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0 )
		return 0;


	POSITION	posStart, posEnd;
	posStart = posEnd = m_posLastSelectable;

	if( !posEnd )posEnd = m_selectableObjects.GetTailPosition();

	do
	{
		//next object
		if( m_posLastSelectable )m_selectableObjects.GetNext( m_posLastSelectable );
		if( !m_posLastSelectable )m_posLastSelectable = m_selectableObjects.GetHeadPosition();
		if( !m_posLastSelectable )return 0;	//no available object


		INamedDataObject2	*pNamedObject = m_selectableObjects.GetAt( m_posLastSelectable );

		if( HitTest( point, pNamedObject ) )
		{
			pNamedObject->AddRef();
			return pNamedObject;
		}
	}
	while( m_posLastSelectable != posEnd );
//no selected object
	m_posLastSelectable = 0;
	
	return 0;
}

CRect CImageSourceView::GetObjectRect( IUnknown *punkObj )
{
	if(CheckInterface(punkObj, IID_ICommentObj))
	{
		CRect rc(NORECT);
		sptrICommentObj sptrComm(punkObj);
		if(sptrComm)
		{
			CRect	rcText = NORECT;
			CPoint	ptArr = CPoint(0,0);
			sptrComm->GetCoordinates(&rcText, &ptArr );
			if(ptArr == CPoint(-1,-1))
				ptArr = rcText.TopLeft();
			CRect rcArr(ptArr.x-1, ptArr.y-1, ptArr.x+1, ptArr.y+1);
			rcArr.NormalizeRect();
			rc.UnionRect(&rcText, &rcArr);
			rc.NormalizeRect();
			rc.InflateRect( 5, 5, 5, 5 );
			rc.right += 3;
			rc.bottom += 3;
		}
		return rc;
	}

	IRectObjectPtr	ptrRectObject( punkObj );
	CRect rect = NORECT;
	if( ptrRectObject ) ptrRectObject->GetRect( &rect );

	if( rect == NORECT )
		return rect;

	if( m_bShowNumber || m_bShowName )
	{
		IMeasureObjectPtr	ptrM( punkObj );
		//false - иначе постоянно пересчитывается
		if( ptrM )rect = _GetObjectClassAndNumberRect( punkObj, rect, true );
	}
	return rect;
}

CRect CImageSourceView::GetObjectRectFull( IUnknown *punkObj )
{
	if(CheckInterface(punkObj, IID_ICommentObj))
	{
		CRect rc(NORECT);
		sptrICommentObj sptrComm(punkObj);
		if(sptrComm)
		{
			CRect	rcText = NORECT;
			CPoint	ptArr = CPoint(0,0);
			sptrComm->GetCoordinates(&rcText, &ptArr, 1 );
			if(ptArr == CPoint(-1,-1))
				ptArr = rcText.TopLeft();
			CRect rcArr(ptArr.x-1, ptArr.y-1, ptArr.x+1, ptArr.y+1);
			rcArr.NormalizeRect();
			rc.UnionRect(&rcText, &rcArr);
			rc.NormalizeRect();
			rc.InflateRect( 5, 5, 5, 5 );
			rc.right += 3;
			rc.bottom += 3;
		}
		return rc;
	}

	IRectObjectPtr	ptrRectObject( punkObj );
	CRect rect = NORECT;
	if( ptrRectObject ) ptrRectObject->GetRect( &rect );

	if( rect == NORECT )
		return rect;

	if( m_bShowNumber || m_bShowName )
	{
		IMeasureObjectPtr	ptrM( punkObj );
		//false - иначе постоянно пересчитывается
		if( ptrM )rect = _GetObjectClassAndNumberRect( punkObj, rect, true );
	}
	return rect;
}


void CImageSourceView::SetObjectOffset( IUnknown *punkObj, CPoint ptOffset )
{
	//[ay] раскешируем нафиг координаты
	sptrICommentObj sptrComm(punkObj);
	if(sptrComm)
	{
		CRect	rcText = NORECT;
		CPoint	ptArr = CPoint(0,0);
		sptrComm->GetCoordinates(&rcText, &ptArr);
		CPoint ptObjOffset(0,0);
		if(ptArr != CPoint(-1,-1))
		{
			CPoint ptLeftTop = ptArr;
			if(ptArr.x > rcText.left)
				ptLeftTop.x = rcText.left;
			if(ptArr.y > rcText.top)
				ptLeftTop.y = rcText.top;

			ptObjOffset = ptOffset - ptLeftTop;
			
			CRect rc_old = GetObjectRect( punkObj );
			if( !is_enable_exended_move() )
			{
  				if( ptObjOffset.x < 0 )
				{
					if( rc_old.left > 0 )
					{
						if( rc_old.left + ptObjOffset.x < 0 )
							ptObjOffset.x -= rc_old.left + ptObjOffset.x;
					}
					else
						ptObjOffset.x = 0;
				}

  				if( ptObjOffset.y < 0 )
				{
					if( rc_old.top > 0 )
					{
						if( rc_old.top + ptObjOffset.y < 0 )
							ptObjOffset.y -= rc_old.top + ptObjOffset.y;
					}
					else
						ptObjOffset.y = 0;
				}
			}
			
			if( !is_enable_exended_move() )
			{
 				if( ptObjOffset.x > 0 )
				{
					if( rc_old.right < m_size_client.cx )
					{
						if( rc_old.right + ptObjOffset.x > m_size_client.cx )
							ptObjOffset.x -= ( rc_old.right + ptObjOffset.x ) - m_size_client.cx;
					}
					else
						ptObjOffset.x = 0;
				}

 				if( ptObjOffset.y > 0 )
				{
					if( rc_old.bottom < m_size_client.cy )
					{
						if( rc_old.bottom + ptObjOffset.y > m_size_client.cy )
							ptObjOffset.y -= ( rc_old.bottom + ptObjOffset.y ) - m_size_client.cy;
					}
					else
						ptObjOffset.y = 0;
				}
			}

			ptArr.Offset( ptObjOffset );
			rcText.OffsetRect( ptObjOffset );
		}
		else
		{
			CRect rc_old = GetObjectRect( punkObj );
			ptObjOffset = ptOffset - rcText.TopLeft();

			if( !is_enable_exended_move() )
			{
  				if( ptObjOffset.x < 0 )
				{
					if( rc_old.left > 0 )
					{
						if( rc_old.left + ptObjOffset.x < 0 )
							ptObjOffset.x -= rc_old.left + ptObjOffset.x;
					}
					else
						ptObjOffset.x = 0;
				}

  				if( ptObjOffset.y < 0 )
				{
					if( rc_old.top > 0 )
					{
						if( rc_old.top + ptObjOffset.y < 0 )
							ptObjOffset.y -= rc_old.top + ptObjOffset.y;
					}
					else
						ptObjOffset.y = 0;
				}
			}
			
			if( !is_enable_exended_move() )
			{
 				if( ptObjOffset.x > 0 )
				{
					if( rc_old.right < m_size_client.cx )
					{
						if( rc_old.right + ptObjOffset.x > m_size_client.cx )
							ptObjOffset.x -= ( rc_old.right + ptObjOffset.x ) - m_size_client.cx;
					}
					else
						ptObjOffset.x = 0;
				}

 				if( ptObjOffset.y > 0 )
				{
					if( rc_old.bottom < m_size_client.cy )
					{
						if( rc_old.bottom + ptObjOffset.y > m_size_client.cy )
							ptObjOffset.y -= ( rc_old.bottom + ptObjOffset.y ) - m_size_client.cy;
					}
					else
						ptObjOffset.y = 0;
				}
			}

			rcText.OffsetRect(ptObjOffset);
		}
		

		sptrComm->SetCoordinates(rcText, ptArr);

		return;
	}

	IRectObjectPtr	ptrRectObject( punkObj );

	if( ptrRectObject )
	{

		if( m_bShowNumber || m_bShowName )
		{
			IMeasureObjectPtr	ptrM( punkObj );
			if( ptrM )
			{
				CRect	rect;
				ptrRectObject->GetRect( &rect );
				CRect	rectAll = _GetObjectClassAndNumberRect( punkObj, rect, true );

//				ptOffset-=(rectAll.TopLeft()-rect.TopLeft());
			}
		}
		ptrRectObject->Move( ptOffset );
/*********************** Maxim [26.06.2002] - Don't move object out work area ***********************/
		if( !is_enable_exended_move() )
		{

			RECT rcClient;
			IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
			if( ptrZ )
			{
				CSize size;
				ptrZ->GetClientSize( &size );
				rcClient = CRect( 0, 0, size.cx, size.cy );
			}

			for( int i = 0; i < 2; i++ )
			{
				RECT rcOb;

				ptrRectObject->GetRect( &rcOb );

				POINT Pts[2];

				Pts[0].x = rcOb.left;
				Pts[0].y = rcOb.top;


				Pts[1].x = rcOb.right;
				Pts[1].y = rcOb.bottom;

				if( !::PtInRect( &rcClient, Pts[i] ) )
				{
					int dx = 0, dy = 0;
					
					if( Pts[i].x >= rcClient.right )
						dx = rcClient.right - Pts[i].x;
					else if( Pts[i].x < 0 )
						dx = -Pts[i].x;

					if( Pts[i].y >= rcClient.bottom )
						dy = rcClient.bottom - Pts[i].y;
					else if( Pts[i].y < 0 )
						dy = -Pts[i].y;

					ptOffset = Pts[0];
					ptOffset.Offset( dx, dy );

					ptrRectObject->Move( ptOffset );
				}
			}
		}
/****************************************************************************************************/
	}

/*******************
		if( !m_rcPrevClass.IsRectEmpty() )
		{
			outRect.UnionRect( &outRect, &m_rcPrevClass );
			m_rcPrevClass.SetRectEmpty();
		}

	long lPos = m_object_coords.find( (long)punkObj );
	if( lPos )
		m_rcPrevClass = m_object_coords.get( lPos );

	lPos = m_object_coords_num.find( (long)punkObj );
	if( lPos )
		m_rcPrevNum = m_object_coords_num.get( lPos );
******************/

	m_object_coords.remove_key( (long)punkObj );
	m_object_coords_num.remove_key( (long)punkObj );
}

void CImageSourceView::DoResizeRotateObject( IUnknown *punkObject, CFrame *pframe )
{
	double fZoomX = pframe->GetZoomX();
	double fZoomY = pframe->GetZoomY();
	double fAngle = pframe->GetAngle();

	CRect	rect = pframe->CalcBoundsRect();

	CString	strParams;
	strParams.Format( "%f, %f, %f, %d, %d", fZoomX, fZoomY, fAngle, rect.left, rect.top );
	::ExecuteAction( _T("ImageTransform"), strParams );
}

bool CImageSourceView::CanResizeObject( IUnknown *punkObject )
{
	if( CheckInterface( punkObject, IID_IImage ) )
		return true;
	if( CheckInterface( punkObject, IID_IMeasureObject ) )
		return true;

	return false;
}

bool CImageSourceView::CanRotateObject( IUnknown *punkObject )
{
	if( CheckInterface( punkObject, IID_IImage ) )
		return true;
	if( CheckInterface( punkObject, IID_IMeasureObject ) )
		return true;

	return false;
}

//switch between modes 
void CImageSourceView::_ChangeSelectionMode( long nMode )
{
	/*if(m_SelectionMode == nMode)
		return;

	m_SelectionMode = (SelectionMode)nMode;
	
	::SetValue( GetAppUnknown(), "ImageView", "SelectionMode", nMode );
	
	m_pframe->EmptyFrame();
	if( m_SelectionMode == SelectFrame )
	{
		if( m_imageSelection != 0 )
			m_pframe->SelectObject( m_imageSelection );
	}
	else if( m_SelectionMode == 0 )
	{		
		POSITION	posActive = m_objects.GetCurPosition();
		if( posActive )
		{
			CObjectWrp	object( m_objects.GetNextObject( posActive ), false );
			m_pframe->SelectObject( object );
		}
	}
	else if( m_SelectionMode == 2 )
	{
		if( m_binary != 0 )
			m_pframe->SelectObject( m_binary );
	}*/
}

void CImageSourceView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	_CheckFrameState();
	
	CViewBase::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CImageSourceView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	_CheckFrameState();
	
	CViewBase::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CImageSourceView::_CheckFrameState()
{
	DWORD	dwFrameFlags = m_pframe->GetFlags();
	DWORD	dwNewFlags = 0;

	if( ( ::GetAsyncKeyState(VK_MENU) & 0x8000) != 0 )
	{
		
		IUnknown *punk = m_pframe->GetActiveObject();
		if( punk )
		{
			if( CheckInterface( punk, IID_IManualMeasureObject ) )
				dwNewFlags = 0;
			else if( CheckInterface( punk, IID_IImage ) || CheckInterface( punk, IID_IMeasureObject ) )
				dwNewFlags = m_dwFrameFlags;

			punk->Release();	punk = 0;
		}
	}

	if( dwFrameFlags != dwNewFlags )
	{
		m_pframe->Redraw( false );
		m_pframe->SetFlags( dwNewFlags );
		m_pframe->Redraw( false );
		UpdateWindow();
	}
}

void CImageSourceView::OnPreviewReady()
{
	_AttachObjects( m_ptrPreviewData );

}

void CImageSourceView::OnEnterPreviewMode()
{
//	m_pframe->LockUpdate( true );
	m_imageSource = m_image;
	m_imageSourceSelection = m_imageSelection;

	_AttachObjects( m_ptrPreviewData );
}

void CImageSourceView::OnLeavePreviewMode()
{
	m_imageSource = 0;
	m_imageSourceSelection = 0;
//	m_pframe->LockUpdate( false );

	_AttachObjects( GetControllingUnknown() );
}

void CImageSourceView::_AttachObjects( IUnknown *punkContextFrom )
{
	m_size_client = CSize( 0, 0 );
	m_bNeedSetAppropriateView = false;

	bool bChangeList = ::GetObjectKey( punkContextFrom ) == ::GetObjectKey( GetControllingUnknown() );

	if( bChangeList )m_listObjects.RemoveAll();
	IUnknown* punkObject	= ::GetActiveObjectFromContext( punkContextFrom, szTypeImage );

	sptrINamedDataObject2	sptrN( punkObject );
	if( punkObject )
		punkObject->Release();

	if( ::GetParentKey( sptrN ) != INVALID_KEY )
	{
//		if( ::GetObjectKey(m_imageSelection) != ::GetObjectKey(punkObject) )//to avoid "deselection"
		{
			if( m_pframe )
				m_pframe->UnselectObject( m_imageSelection );

			_SetSelection( sptrN );
		
			IUnknown	*punk = 0;
			sptrN->GetParent( &punk );
			sptrN = punk;
			punk->Release();
		}
	}
	else
	{
		_SetSelection( 0 );
	}


	RECT rcImage = NORECT;

	if( ::GetObjectKey( sptrN ) != GetObjectKey( m_image ) )
	{
		if( m_pframe )
			m_pframe->UnselectObjectsWithSpecifiedParent( m_image );
      
		// [vanek] BT2000:3971 - 17.09.2004
		bool bsame_group = false;
		bsame_group = GetObjectBaseKey( sptrN ) == GetObjectBaseKey( m_image );

		m_image = sptrN;

		if( m_image )
		{
			m_image.SetOffset( CPoint( 0, 0 ) );
            
			if( !bsame_group )
				_UpdateScrollSizes();	
		}
	}
	else
	{
		IRectObjectPtr ptrROb = punkObject;
		if( ptrROb != 0 )
			ptrROb->GetRect( &rcImage );
	}


	if( m_image != 0 )
	{
		m_size_client = CSize( m_image.GetWidth(), m_image.GetHeight() );
		m_size_client = CSize( max( m_size_client.cx, rcImage.right ), max( m_size_client.cy, rcImage.bottom ) );

		//paul 17.04.2003
		if( !is_enable_exended_move() )
			m_size_client = CSize( m_image.GetWidth(), m_image.GetHeight() );

		//store image size to shell.data
		::SetValue( GetAppUnknown(), "\\Information", "LastImageCX", (long)m_image.GetWidth() );
		::SetValue( GetAppUnknown(), "\\Information", "LastImageCY", (long)m_image.GetHeight() );
	}
	else
		m_size_client = CSize( 0, 0 );

	if( bChangeList )
		_recalc_rect();

	_UpdateScrollSizes();

//attach the drawing objects list
	//m_sptrCommentObjList = 0;
	//m_sptrCommentObjList = ::GetActiveObjectFromContext( GetControllingUnknown(), szDrawingObjectList );
	punkObject = ::GetActiveObjectFromContext( punkContextFrom, szDrawingObjectList );

	if( ::GetObjectKey( m_drawobjects ) != ::GetObjectKey( punkObject ) )
	{
		if( m_pframe )
			m_pframe->UnselectObjectsWithSpecifiedParent( m_drawobjects );

		_attach_msg_listeners_from_list( m_drawobjects, false );
		m_drawobjects = punkObject;
		_attach_msg_listeners_from_list( m_drawobjects, true );

		if( IsWindow(GetSafeHwnd()) )
			Invalidate();
	}

	if( punkObject )
		punkObject->Release();

	
	if( bChangeList )
	{
		if( m_image!=0 )m_listObjects.AddTail( m_image );
		if( m_imageSelection!=0 )m_listObjects.AddTail( m_imageSelection );
		//if( m_objects!=0 && bAddObjects)m_listObjects.AddTail( m_objects );
		if( m_drawobjects!=0 )m_listObjects.AddTail( m_drawobjects );
	}

	_UpdateImagePane();
												
	m_class_names.clear();
	m_class_colors.clear();
	m_object_coords.clear();
	m_object_coords_num.clear();

	m_class_names2.clear();
	m_class_colors2.clear();

}

bool CImageSourceView::is_have_print_data()
{
	if( m_image.IsEmpty() && m_imageSelection.IsEmpty() && m_drawobjects == 0 )
		return false;

	return true;
}

CPoint ptOrg(0,0);
CPoint ptCorr(0,0);
void CImageSourceView::Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags )
{
	m_bstr_class_name = "";

	m_clWhite = ::GetValueColor( ::GetAppUnknown(), "\\ImageView", "N_And_N_Color", RGB(255, 255, 255) );
	m_bAsClassColor = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "N_And_N_AsClassColor", 0 ) == 1;
	//[paul]20.03.2002
	if( !is_have_print_data() )//nothing to print
//	{
//		CDC::FromHandle(hdc)->FillSolidRect(rectTarget, GetSysColor( COLOR_WINDOW ));
//		DrawNoDataObject(*CDC::FromHandle(hdc), rectTarget);
		return;
//	}

	//CTimeTest test(true, "PRINT");
	m_fZoom = ::GetZoom( GetControllingUnknown() );
	m_pointScroll = ::GetScrollPos( GetControllingUnknown() );
								 
	if( m_fZoom != m_fOldZoom )
	{
		m_object_coords.clear();
		m_object_coords_num.clear();
		m_fOldZoom = m_fZoom;
	}

	CDC* pdc = CDC::FromHandle(hdc);

	CRect rectPaint(nUserPosX, nUserPosY, nUserPosX+nUserPosDX, nUserPosY+nUserPosDY);

	if(rectPaint == NORECT)
		return;

	pdc->SelectClipRgn(0);
	CRgn rgnClip;
	rgnClip.CreateRectRgnIndirect(&rectTarget);
	pdc->SelectClipRgn(&rgnClip);

	if(nUserPosX == 0 && nUserPosY == 0)
	{
		ptOrg = rectTarget.TopLeft();
	}

	CPoint ptOrgCur;
	CSize s1;
	s1 = (rectTarget.TopLeft()-ptOrg) - CPoint(rectPaint.left*rectTarget.Width()/rectPaint.Width(), rectPaint.top*rectTarget.Height()/rectPaint.Height());
	ptOrgCur = ptOrg + CSize(abs(s1.cx), abs(s1.cy));

	//cache color convertor information
	IUnknown *punkColorCnv = 0;

	CRect rcImage = CRect(0,0,0,0);
	if( !m_image.IsEmpty() )
	{
		rcImage = m_image.GetRect();

		m_image->GetColorConvertor( &punkColorCnv );
		m_ptrCC = punkColorCnv;
		m_ptrCCLookup = punkColorCnv;
		if( punkColorCnv )punkColorCnv->Release();

		ASSERT( m_ptrCC != 0 );

		if( m_ptrCC == 0 )
		{
			pdc->SelectClipRgn( 0 );
			return;
		}

		//set lookup table
		byte* pbyteLookup = 0;
		long nSize = 0;
		pbyteLookup = ::GetValuePtr(m_image, szLookup, szLookupTable, &nSize);
		if(pbyteLookup)
		{
			if( m_ptrCCLookup != 0 )
			{
				int cx = 256;
				int cy = nSize/cx; 

				if( m_ptrCCLookup->SetLookupTable( pbyteLookup, cx, cy ) != S_OK )
					delete pbyteLookup;
			}
			else
				delete pbyteLookup;
		}
	}
	else
	{
		IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
		CSize size;
		ptrZ->GetClientSize( &size );
		rcImage.right = size.cx;
		rcImage.bottom = size.cy;
	}


	CDC dcMemory;
	dcMemory.CreateCompatibleDC(pdc);

	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );
	bmih.biBitCount = 24;
	
	bmih.biWidth = max( rcImage.Width(), m_size_client.cx );
	bmih.biHeight = max( rcImage.Height(), m_size_client.cy );

	CRect RealSize( 0,0, bmih.biWidth, bmih.biHeight );

	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;

	if( bmih.biHeight == 0 ||bmih.biWidth == 0 )
		return;

	//create a DIB section
	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( *pdc, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0 );
	
	CFont fnt;
	fnt.CreatePointFont( 100, ::GetValueString(GetAppUnknown(), "Editor", "CommentText_Name", "Arial") );
	CFont	*pold_font = (CFont*)dcMemory.SelectObject( &fnt );


	ASSERT( hDIBSection );

	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );

	ASSERT( psrcBitmap );

	//propare the memory DC to drawing
	CBitmap *poldBmp = dcMemory.SelectObject( psrcBitmap );
	dcMemory.SetMapMode( MM_TEXT );

//	if( m_image.IsEmpty() || RealSize != rcImage )		 
		dcMemory.FillRect( &RealSize, &CBrush( RGB( 255, 255, 255 ) ) );

	//draw main image
	DWORD dwDrawFlags = 0;
	if((m_dwShowFlags & isfPseudoImage) || (m_dwShowFlags & isfFilledImageExceptObjects))
		dwDrawFlags = DrawMask;
	else
		dwDrawFlags = DrawImage;
	
	double fOldZoom = m_fZoom;
	m_fZoom = 1;

	{
		IScrollZoomSitePtr	site( GetControllingUnknown() );
		site->SetZoom( 1 );
	}

	_DrawImage(dcMemory, m_image, RealSize, dwDrawFlags, ::GetBackgroundColor(), &bmih, pdibBits, bUseScrollZoom);	

	CRect rcDraw( nUserPosX, nUserPosY, nUserPosX + nUserPosDX, nUserPosY + nUserPosDY );

	//draw the selection. Draw here only if it is inactive and it doesn't match with preview image
	if( !m_imageSelection.IsEmpty() )
	{
		DWORD dwDrawFlags = DrawClip|DrawContours|DrawSelFrame;
		COLORREF	cr = GetValueColor( ::GetAppUnknown(), "\\Colors", "Selection", ::GetBackgroundColor() );
		CPen	pen( PS_DOT, 0, cr );

		dcMemory.SetBkMode(OPAQUE);
		dcMemory.SelectObject( &pen );
		dcMemory.SelectStockObject( NULL_BRUSH );
		dcMemory.SetBkColor( RGB( 255, 255, 255 ) );

		if((m_dwShowFlags & isfPseudoImage) || (m_dwShowFlags & isfFilledImageExceptObjects))
			dwDrawFlags |= DrawMask;
		else
			dwDrawFlags |= DrawImage;

		_DrawImage( dcMemory, m_imageSelection, RealSize, dwDrawFlags, ::GetBackgroundColor(), &bmih, pdibBits, bUseScrollZoom );
		
		
		dcMemory.SelectStockObject( BLACK_PEN );
	}

	m_bFromPrint = false;
	{

		//paul 27.06.2001 - correct font size
		CFont* pOldFont = dcMemory.GetCurrentFont();
		
		LOGFONT lf;
		::ZeroMemory( &lf, sizeof(LOGFONT) );

		if( pOldFont )
			pOldFont->GetLogFont( &lf );

		strcpy( lf.lfFaceName, "Arial" );

 		lf.lfWidth = 0;//long( lf.lfWidth * (double)rectTarget.Width() / (double)rcImage.Width() );
//		lf.lfHeight = LONG( (double)lf.lfWidth * (double)rectTarget.Width() / (double)rcImage.Width() );
		double fTmp = lf.lfHeight * (double)rectTarget.Width() / (double)nUserPosDX;
		lf.lfHeight = long( fTmp * GetDeviceCaps( dcMemory, LOGPIXELSY) / 72.0 - 0.5 );
		
		CFont fontNew;
		fontNew.CreateFontIndirect( &lf );		

		dcMemory.SelectObject( &fontNew );

		DrawLayers(&dcMemory, &bmih, pdibBits, RealSize, bUseScrollZoom==TRUE);

		dcMemory.SelectObject( pOldFont );

	}

	if( dwFlags == 1 )	
	{
		m_fZoom = fOldZoom;
		_DrawGrid( dcMemory, rectPaint );
		m_fZoom = 1;
	}

	CPoint pt = pdc->GetViewportOrg();
	BOOL bRes = FALSE;
	if( dwFlags == 1 )	
	{	
		bRes = ::DrawDibDraw(DrawDibInstance, *pdc, rectTarget.left, rectTarget.top, rectTarget.Width(), rectTarget.Height(),
		&bmih, pdibBits, nUserPosX, nUserPosY, nUserPosDX , nUserPosDY , DDF_HALFTONE );
	}
	else
	{
		m_fZoom = max(rectTarget.Width() / (double)rectPaint.Width(), rectTarget.Height() / (double)rectPaint.Height() );
		bRes = StretchDIBits( *pdc, rectTarget.left, rectTarget.top, int(rectPaint.Width() * m_fZoom), int(rectPaint.Height() * m_fZoom),
		nUserPosX, bmih.biHeight - nUserPosDY - nUserPosY, rectPaint.Width() , rectPaint.Height(), pdibBits, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, SRCCOPY );
	}

	// [vanek] SBT: 756 - 09.03.2004
	m_object_coords.clear();
	m_object_coords_num.clear();

	{
		m_fZoom = max(rectTarget.Width() / (double)rectPaint.Width(), rectTarget.Height() / (double)rectPaint.Height() );
		IScrollZoomSitePtr	site( GetControllingUnknown() );
		site->SetZoom( m_fZoom  );//Was -m_fZoom 

	}


	m_bFromPrint = true;

	pdc->SetViewportOrg( int(rectTarget.left - (nUserPosX)*m_fZoom) , int(rectTarget.top - nUserPosY*m_fZoom) );

	{
		//paul 27.06.2001 - correct font size
		CFont* pOldFont = pdc->GetCurrentFont();
		
		LOGFONT lf;
		::ZeroMemory( &lf, sizeof(LOGFONT) );

		if( pOldFont )
			pOldFont->GetLogFont( &lf );

		strcpy( lf.lfFaceName, "Arial" );

		// [vanek] - 23.12.2003: учитываем суммарный зум
		lf.lfWidth = 0;
		lf.lfHeight = long( lf.lfHeight * m_fZoom / m_fOldZoom );
		if( !lf.lfHeight ) 
			lf.lfHeight = -1;
		
		CFont fontNew;
		fontNew.CreateFontIndirect( &lf );		

		pdc->SelectObject( &fontNew );
		
		DrawLayers( pdc, &bmih, NULL, rcImage, bUseScrollZoom==TRUE);

		dcMemory.SelectStockObject( BLACK_PEN );

		pdc->SelectObject( pOldFont );

	}


	if( m_ptrCCLookup != 0 )
		m_ptrCCLookup->KillLookup(); 
	m_ptrCC = 0;
	m_ptrCCLookup = 0;



	INamedPropBagPtr ptr_bag(GetControllingUnknown());  
	_variant_t var;
	if(dwFlags == 1)
		var=long(1);
	else
		var=long(0);
	if(ptr_bag)	
	{
		ptr_bag->SetProperty(_bstr_t("PrintPreview"),var);
		_variant_t varZoom=double(m_fZoom/fOldZoom);
		ptr_bag->SetProperty(_bstr_t("PrintPreviewZoom"),varZoom);
	}
	IUnknownPtr ptr = ptr_bag; //!!! debug

	_DrawObjects( *pdc, rectPaint, &bmih, NULL );
	//if(_IsCalibrVisible()) // печатаем независимо от видимости маркера на view
	{
		if (::GetValueInt(::GetAppUnknown(), "ImageView", "PrintMarker", 0))
		{
			CPoint pt(0,0);
			_DrawCalibr( *pdc, rectPaint, &bmih, NULL, pt, true, m_fZoom/fOldZoom, dwFlags == 1 );
		}
	}

	dcMemory.SetViewportOrg( 0, 0 );

	{
		IScrollZoomSitePtr	site( GetControllingUnknown() );
		site->SetZoom( fOldZoom );
		m_fZoom = fOldZoom;
	}

	pdc->SetViewportOrg( pt );

	ASSERT( bRes > 0 );

	//prepare to delete objects
	dcMemory.SelectObject( poldBmp );
	dcMemory.SelectObject( pold_font );

	::DeleteObject(hDIBSection);	

}

void CImageSourceView::_UpdateScrollSizes()
{
	IScrollZoomSitePtr	site( GetControllingUnknown() );
	IScrollZoomSite2Ptr	site2( GetControllingUnknown() );
	//update scroll size
	CSize	size;
	site->GetClientSize( &size );
	CSize	sizeNew = m_size_client;

	if( m_size_client.cx == 0 && m_size_client.cy == 0 )
		sizeNew = sizeNoImage;

	if( size != sizeNew )
	{
		site->SetClientSize( sizeNew );
//проблемы при выходе из режима яркость-контраст в хромосомном анализе
//		if( m_image == 0 )
//			site->SetZoom( 1 );
	}

	if( GetSafeHwnd() )
		Invalidate();
}

void CImageSourceView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	_ShowImagePage( bActivate );
	_ShowImagePane( bActivate );
}

bool CImageSourceView::_ShowImagePage( bool bShow )
{
	IUnknown* punkPage = NULL;
	int nPageNum = -1;

	if( ::GetPropertyPageByProgID( szZoomNavigatorPropPageProgID, &punkPage, &nPageNum) )
	{
		::ShowPropertyPage( nPageNum, bShow );
		if( punkPage )
			punkPage->Release();
	}

	// [vanek]
	_UpdateAviPropPage( bShow );

	return true;

}

POSITION CImageSourceView::GetFisrtVisibleObjectPosition()
{
	return m_listObjects.GetHeadPosition();
}

IUnknown *CImageSourceView::GetNextVisibleObject( POSITION &rPos )
{
	IUnknown * p = 0;
	if (rPos)
		p = m_listObjects.GetNext( rPos );
	if (p)
		p->AddRef();
	return p;
}

const char* CImageSourceView::GetViewMenuName()
{
	return szSourceView;
}

//#define ID_CLASSMENU_BASE (ID_CMDMAN_BASE + 1200)

void CImageSourceView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	if( ::GetValueInt( ::GetAppUnknown(), get_section(), "ShowContext", 1 ) == 0 )
		return;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;

	HRESULT hr=sptrCM->GetActionID(_bstr_t(L"SetClass"),&m_iSkip);

	CMenuRegistrator rcm;	

	CString strMenuName;

	IUnknownPtr ptrObject;

	if( strMenuName.IsEmpty() )
	{
		CString strViewName = GetViewMenuName();
		strMenuName = (LPCSTR)rcm.GetMenu( strViewName, 0 );
	}		

	if( strMenuName.IsEmpty() )
		return;
	_bstr_t	bstrMenuName = strMenuName;

	// Context menu for selection, drawing objects, etc...
	

	if( m_bShowChildContextMenu )
	{
		IUnknownPtr punkObject;
		if(m_pframe!=0) punkObject = m_pframe->GetObjectByIdx( 0 );

		if( (IUnknown*)punkObject )
		{
			ptrObject = punkObject;

			CString strObjectType = ::GetObjectKind( ptrObject );
			
			CString strObjectMenuName = (LPCSTR)rcm.GetMenu( szImageCloneView, strObjectType );

			if( !strObjectMenuName.IsEmpty() )
			{
				bstrMenuName = strObjectMenuName;
		}
	}

		// register context menu initializer
		sptrCM->RegisterMenuCreator(&m_xMenuInitializer);
		// execute context menu (return value is action's ID which selected by user)
		UINT uCMD=0;
		sptrCM->ExecuteContextMenu3(bstrMenuName, point, TPM_RETURNCMD, &uCMD);
		sptrCM->UnRegisterMenuCreator(&m_xMenuInitializer);
		{
			long nClassCount = class_count();
			if(uCMD>=ID_CLASSMENU_BASE && uCMD<ID_CLASSMENU_BASE+UINT(nClassCount)+1)
			{
				ICalcObjectPtr sptrCO(punkObject);
				if(sptrCO != 0)
				{
					bool bDeselect = ::GetValueInt( ::GetAppUnknown(), szIVImageView, "DeselectOnSetClass", 1L ) != 0;
					int nClass = uCMD - ID_CLASSMENU_BASE - 1;
					char sz_cl[60];
					_itoa(nClass, sz_cl, 10);
					if(bDeselect) strcat(sz_cl,",\"\", 1");
					::ExecuteAction("SetClass",sz_cl,0);
				}
			}
			else
	{
				// set selected action arguments
				bool bExec = true;

				BSTR	bstrActionName;
				_bstr_t strArgs = "";
				DWORD	dwFlag = 0;

				if (!SUCCEEDED(sptrCM->GetActionName(&bstrActionName, uCMD)))
		return;
	
				_bstr_t strActionName = bstrActionName;
				::SysFreeString(bstrActionName);

				//AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
				::ExecuteAction(strActionName, strArgs, dwFlag);
			}
		}
	}else
	sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
}

void CImageSourceView::OnDestroy() 
{
	//::SetValue( GetAppUnknown(), "ImageView", "SelectionMode", m_SelectionMode );
	//INotifyControllerPtr ptr_nc = GetAppUnknown();
	INotifyControllerPtr ptr_nc(GetControllingUnknown());//GetAppUnknown();
	if( ptr_nc )
		ptr_nc->UnRegisterEventListener( 0, GetControllingUnknown() );
	
	CViewBase::OnDestroy();
	
	// TODO: Add your message handler code here	
}



////////////////////////////////////////////////////////////////
//
//	Status Pane support
//
////////////////////////////////////////////////////////////////
bool	CImageSourceView::_ShowImagePane( bool bCreate )
{
	INewStatusBarPtr	ptrStatus( ::StatusGetBar(), false );
	if( ptrStatus == 0 )return false;
	HWND	hwnd = ::GetWindow( ptrStatus );

	ptrStatus->RemovePane( guidPaneColorSystem );

	if( m_pImagePane )
	{
		m_pImagePane->DestroyWindow();
		delete m_pImagePane;
		m_pImagePane = 0;
	}


	if( bCreate && GetValueInt( ::GetAppUnknown(), szIVImageView, "ShowColorSysPane", 1L ) == 1L )
	{
		m_pImagePane = new CImagePane; 
		m_pImagePane->Create(NULL, _T(""), WS_CHILD|WS_VISIBLE, NORECT, CWnd::FromHandle(hwnd), 65535);
		ptrStatus->AddPane( guidPaneColorSystem, 170, m_pImagePane->GetSafeHwnd(), 0 );

		_UpdateImagePane();
	}
	return true;
}

////////////////////////////////////////////////////////////////
IUnknown* CImageSourceView::GetImageByPoint( CPoint pt )
{	
	//GetActiveObjectFromContext( GetControllingUnknown(), szTypeImage );	
	if( m_image.IsEmpty() )
		return NULL;

	if( m_imageSelection.IsEmpty() )
	{
		m_image->AddRef();
		return m_image;
	}

	CRect rcImage = NORECT;
	CRect rcSelection = NORECT;

	
	CPoint ptOffset(0,0);

	//Fill image rect
	ptOffset = m_image.GetOffset();
	rcImage.left	= ptOffset.x;
	rcImage.top		= ptOffset.y;
	rcImage.right	= rcImage.left + m_image.GetWidth();
	rcImage.bottom	= rcImage.top + m_image.GetHeight();

	//Fill selection rect
	ptOffset = m_imageSelection.GetOffset();
	rcSelection.left	= ptOffset.x;
	rcSelection.top		= ptOffset.y;
	rcSelection.right	= rcSelection.left + m_imageSelection.GetWidth();
	rcSelection.bottom	= rcSelection.top + m_imageSelection.GetHeight();

	if( rcSelection.PtInRect( pt ) )
	{
		m_imageSelection->AddRef();
		return m_imageSelection;
	}

	if( rcImage.PtInRect( pt ) )
	{
		m_image->AddRef();
		return m_image;
	}

	return NULL;
}

////////////////////////////////////////////////////////////////
void CImageSourceView::_UpdateImagePane()
{
	if( !GetSafeHwnd() || !m_pImagePane )
		return;

	CPoint pointScreen, pointClient;

	::GetCursorPos( &pointScreen );
	ScreenToClient( &pointScreen );

	pointClient = ::ConvertToClient( GetControllingUnknown(), pointScreen );	

	m_pImagePane->SetImage( m_image, GetControllingUnknown() );
	m_pImagePane->SetPosition( pointClient );
}


bool	CImageSourceView::HitTest( CPoint point, INamedDataObject2 *pNamedObject )
{
	IBinaryImagePtr	ptrBin = pNamedObject;
	if( ptrBin != 0 )		
		if( !GetValueInt( GetAppUnknown(), "\\ImageView", "EnableBinarySelection", 0 ) )
			return false;

	// [vanek] BT2000: 3543
	if( CheckInterface( pNamedObject, IID_ICommentObj ) )
	{
		if( !GetValueInt( GetAppUnknown(), "\\ImageView", "EnableDrawingSelection", 1 ) )
			return false;
	}
	else if( !GetValueInt( GetAppUnknown(), "\\ImageView", "EnableSelection", 1 ) )	
		return false;

	IRectObjectPtr	ptrObject( pNamedObject );
	if( ptrObject == 0 )return 0;

	IIdiogramPtr sptrI = ptrObject;
	if( sptrI )
		return 0;


	long	lHitTest = 0;
	ptrObject->HitTest( point, &lHitTest );
	return lHitTest != 0;
}

void	CImageSourceView::ClearSelectableList()
{
	POSITION	pos = m_selectableObjects.GetHeadPosition();
	while( pos )m_selectableObjects.GetNext( pos )->Release();
	m_selectableObjects.RemoveAll();
	m_posLastSelectable = 0;
	m_object2position.RemoveAll();
}

void	CImageSourceView::FillSelectableList()
{
	ClearSelectableList();

	POSITION	posVisibleObject = m_listObjects.GetHeadPosition();
	while( posVisibleObject )
	{
		IUnknown *punk = m_listObjects.GetNext( posVisibleObject );

		if( CheckInterface( punk, IID_IImage ) )
		{
			if( ::GetValueInt(::GetAppUnknown(), "\\ImageView", "ImageSelectionOnSourceOnly", 1) )
			{
				if((m_dwShowFlags & isfSourceImage) != isfSourceImage)
					continue;
			}
			if( GetParentKey( punk ) == INVALID_KEY )
				continue;
		}

		INamedDataObject2Ptr	ptrNamedList( punk );
		AddSelectableObject( ptrNamedList, false );
	}
	m_posLastSelectable = 0;
}

void	CImageSourceView::OnChangeObject( INamedDataObject2 *pNamedObject )
{
	RemoveSelectableObject( pNamedObject );
	AddSelectableObject( pNamedObject );
}

void	CImageSourceView::OnSelect( INamedDataObject2 *pNamedObject )
{
	IUnknown	*punkParent = 0;
	pNamedObject->GetParent( &punkParent );
	INamedDataObject2Ptr	ptrNamed( punkParent );
	if( punkParent )punkParent->Release();
	long	lpos = 0;
	pNamedObject->GetObjectPosInParent( &lpos );
	if( ptrNamed != 0 )ptrNamed->SetActiveChild( lpos );
	OnSelect( ptrNamed );

	m_posLastSelectable = 0;
	m_object2position.Lookup( pNamedObject, m_posLastSelectable );
}

void	CImageSourceView::AddSelectableObject( INamedDataObject2 *pNamedObject, bool bInvalidate )
{
	//TRACE("Add %p, Count %d\n", pNamedObject, m_selectableObjects.GetCount() );
	POSITION	pos;
	if(m_object2position.Lookup( pNamedObject, pos )!=0 ) return;
//#ifdef _DEBUG
//
//#endif //_DEBUG
	m_object2position[pNamedObject]=m_selectableObjects.AddTail( pNamedObject );
	pNamedObject->AddRef();

	long	lpos = 0;
	pNamedObject->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown *punk = 0;
		pNamedObject->GetNextChild( &lpos, &punk );
		INamedDataObject2Ptr	ptrNamed( punk );
		punk->Release();
		if( ptrNamed != 0 )AddSelectableObject( ptrNamed );
	}

	if( bInvalidate && m_hWnd )
	{
		CRect	rect = GetObjectRect( pNamedObject );

		if( !rect.IsRectEmpty() )
			InvalidateRect( ::ConvertToWindow( GetControllingUnknown(), rect ) );
	}
}

void	CImageSourceView::RemoveSelectableObject( INamedDataObject2 *pNamedObject, POSITION pos )
{
	//TRACE("Remove %p, Count %d\n", pNamedObject, m_selectableObjects.GetCount() );
	if( !pos )pos = m_object2position[pNamedObject];
	ASSERT(pos);

	m_selectableObjects.RemoveAt( pos );
	m_object2position.RemoveKey( pNamedObject );
	if( m_posLastSelectable == pos )m_posLastSelectable = 0;
	

	long	lpos = 0;
	pNamedObject->GetFirstChildPosition( &lpos );

	while( lpos )
	{
		IUnknown *punk = 0;
		pNamedObject->GetNextChild( &lpos, &punk );
		INamedDataObject2Ptr	ptrNamed( punk );
		punk->Release();

		if( ptrNamed != 0 )RemoveSelectableObject( ptrNamed );
	}

	CRect	rect = GetObjectRect( pNamedObject );
	InvalidateRect( ::ConvertToWindow( GetControllingUnknown(), rect ) );

	pNamedObject->Release();
}

void CImageSourceView::_select_active_object(IUnknown *punkObjectList)
{
	// SBT1742(12.04.2006). Do not remove object selection during notification with cncReset code.
	INamedDataObject2Ptr	ptrOL( punkObjectList );
	if (ptrOL == 0) return;
	long lpos = 0;
	ptrOL->GetActiveChild(&lpos);
	IUnknownPtr punkActiveObj;
	if (lpos != 0)
		ptrOL->GetNextChild(&lpos, &punkActiveObj);
	if (punkActiveObj != 0)
		m_pframe->SelectObject( punkActiveObj, false );
	else
		m_pframe->EmptyFrame();
}

void	CImageSourceView::DoNotifySelectableObjects( long lHint, IUnknown *punkObject, IUnknown *punkObjectList )
{
	ICompositeObjectPtr co(punkObjectList);
	long bC;
	IUnknown* unkGrand =0;
	if(co)
	{
		co->IsComposite(&bC);
		if(bC)
		{
			ICompositeSupportPtr cs(punkObject);
			if(cs)
			{
				IUnknown* unk =0;
				cs->GetGrandParent(&unkGrand);
				if(unkGrand) punkObject =unkGrand;
			}
		}
	}
	if( lHint == cncAdd )
	{
		//parent should be in list
		INamedDataObject2Ptr	ptrObject( punkObject );
		INamedDataObject2Ptr	ptrObjectList( punkObjectList );
		POSITION	pos;
		if( !m_object2position.Lookup( ptrObjectList, pos ) )
		{
			if(unkGrand) unkGrand->Release();
			return;
		}

		POSITION	posOld;
		if( m_object2position.Lookup( ptrObject, posOld ) )
			RemoveSelectableObject( ptrObject, posOld );
		AddSelectableObject( ptrObject );
	}
	else if( lHint == cncRemove )
	{
		INamedDataObject2Ptr	ptrObject( punkObject );
		POSITION	posOld;
		if( m_object2position.Lookup( ptrObject, posOld ) )
		{
			RemoveSelectableObject( ptrObject, posOld );
			m_pframe->UnselectObject( punkObject );

			if( m_bShowNumber && CheckInterface( ptrObject, IID_IMeasureObject ) )
				Invalidate();
		}
	}
	else if( lHint == cncChange )
	{
		INamedDataObject2Ptr	ptrObject( punkObject );
		POSITION	posOld;
		if( m_object2position.Lookup( ptrObject, posOld ) )
		{
			//[Max] - Пиши коментарии! Если не вызывать функцию GetObjectRectFull, 
			//то не инвалидейтится старая область при уменьшении шрифта в комментарии.
			InvalidateRect( ConvertToWindow( GetControllingUnknown(), GetObjectRectFull( ptrObject ) ) );
			//InvalidateRect( ConvertToWindow( GetControllingUnknown(), GetObjectRect( ptrObject ) ) );
		}
	}
	else if( lHint == cncActivate )
	{
		INamedDataObject2Ptr	ptrObject( punkObject );
		POSITION	posOld;
		if( m_object2position.Lookup( ptrObject, posOld ) )
		{
			m_pframe->SelectObject( ptrObject, false );
		}
		else
			m_pframe->EmptyFrame();
	}
	else if( lHint == cncReset )
	{
		_select_active_object(punkObjectList); // SBT1742(12.04.2006)
	}

	if(unkGrand) unkGrand->Release();
}


void CImageSourceView::_attach_msg_listeners_from_list( IUnknown* punkObjectList, bool bAttach )
{
	INamedDataObject2Ptr ptrList( punkObjectList );
	if( ptrList == 0 )
		return;

	long lpos = 0;
	ptrList->GetFirstChildPosition( &lpos );
	while( lpos )
	{
		IUnknown* punk = 0;
		ptrList->GetNextChild( &lpos, &punk );
		if( !punk ) continue;

		if( !CheckInterface( punk, IID_IMsgListener ) )
		{
			punk->Release();	punk = 0;
			continue;
		}

		if( bAttach )
			_AttachListener( punk );
		else
			_DetachListener( punk );
		
		punk->Release();	punk = 0;
	}	
}

void CImageSourceView::_attach_msg_listener( IUnknown* punkObject, bool bAttach )
{
	if( !CheckInterface( punkObject, IID_IMsgListener ) )
		return;

	if( bAttach )
		_AttachListener( punkObject );
	else
		_DetachListener( punkObject );

}


bool CImageSourceView::GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX )
{
	CRect rc = NORECT;

	GetWindowRect(rc);
	
	IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
	double fZoom = 1.0;
	if( ptrZ )
	{
		CSize size;
		ptrZ->GetClientSize( &size );
		ptrZ->GetZoom( &fZoom );
		rc = CRect( 0, 0, int((double)size.cx * fZoom), int((double)size.cy * fZoom) );

	}
	bool bContinue = false;

	long nWidth = rc.Width();

	if(nWidth > nUserPosX*fZoom + nMaxWidth)
	{
		nReturnWidth = nMaxWidth;
		nNewUserPosX =  int( nNewUserPosX + nReturnWidth / fZoom );
		bContinue = true;
	}
	else
	{
		nReturnWidth = int( nWidth - nUserPosX * fZoom );
		nNewUserPosX = int( nWidth / fZoom );
	}

	return bContinue;
}

bool CImageSourceView::GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY )
{
	CRect rc = NORECT;
	GetWindowRect(rc);
	
	IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
	double fZoom = 1.0;
	if( ptrZ )
	{
		CSize size;
		ptrZ->GetClientSize( &size );
		ptrZ->GetZoom( &fZoom );
		rc = CRect( 0, 0, int((double)size.cx * fZoom), int((double)size.cy * fZoom) );

	}
	

	long nHeight = rc.Height();
	bool bContinue = false;
	if(nHeight > nUserPosY*fZoom + nMaxHeight)
	{
		nReturnHeight = nMaxHeight;
		nNewUserPosY =  int( nNewUserPosY + nReturnHeight / fZoom );
		bContinue = true;
	}
	else
	{
		nReturnHeight = int( nHeight - nUserPosY * fZoom );
		nNewUserPosY = int( nHeight / fZoom );
	}

	return bContinue;
}


void CImageSourceView::_fix_letter( CRect *out, CRect textRc, IUnknown *punkObject )
{
	CRect outRect = *out;

	double Del = m_bFromPrint ? m_fZoom : 1;

	int w = textRc.Width(), h = textRc.Height();

	outRect.left = long( outRect.left * Del );
	outRect.top	 = long( outRect.top * Del );

	outRect.right = outRect.left + w;
	outRect.bottom = outRect.top + h;

	CRect rcClient;
	IScrollZoomSitePtr ptrZ( GetControllingUnknown() );
	if( ptrZ != 0 )
	{
		CSize size;
		ptrZ->GetClientSize( &size );
		rcClient = CRect( 0, 0, int(size.cx*Del), int(size.cy*Del) );
	}


	CObjectWrp object(punkObject, true);

	IUnknown *punk = object.GetImage();

	if( punk == 0 )
		return;

	IImage3Ptr ptrI = punk;
	punk->Release();

	if( ptrI == 0 )
		return;

	int W = 0, H = 0;

	POINT pt;

	ptrI->GetSize( &W, &H );
	ptrI->GetOffset( &pt );

	LPBYTE *pSourceImgMask = new LPBYTE[H];
	
	for( int y = 0; y < H; y++ )
	{
		pSourceImgMask[y]  = new BYTE[W];

		LPBYTE tmp;
		ptrI->GetRowMask( y, &tmp );
		
		for( int x = 0; x < W; x++ )
			pSourceImgMask[y][x] = tmp[x];
	}

	int ptx = outRect.left, pty = outRect.top;
	bool bCh = false;
	if( outRect.left < rcClient.left || outRect.top < rcClient.top || 
		outRect.left + w > rcClient.right || 
		outRect.top + h > rcClient.bottom || 
		!_check_cross( outRect.left, outRect.top, w, h, punkObject ) || 
		!_check_corners( outRect, rcClient, pSourceImgMask, int(W*Del), int(H*Del), CPoint( int(pt.x * Del), int(pt.y * Del) ), &ptx, &pty, punkObject ) )
	{
		bCh = true;
		CObjectWrp object(punkObject, true);
		IUnknown *punk = object.GetImage();

		IImage3Ptr ptrI = punk;

		if( punk )
			punk->Release();

		if( ptrI != 0 )
		{
			int nSz = 0;
			ptrI->GetContoursCount( &nSz );

			for(int j = 0; j < nSz; j++ )
			{
				Contour *imgContour = 0;
				ptrI->GetContour( j, &imgContour );

				if( imgContour && imgContour->lFlags == cfExternal )
				{
					double lenMin = 10000;
					int nContSize = imgContour->nContourSize;

					int nID = 0;

					for( int i = 0; i < nContSize; i++ )
					{
						int ix = int(imgContour->ppoints[i].x*Del);
						int iy =  int(imgContour->ppoints[i].y*Del);

						double len = _hypot( outRect.left - ix, outRect.top - iy );
						if( len < lenMin ) 
						{
							lenMin = len;
							nID = i;
						}
					}

					BOOL bOK = false;
					for( i = nID; i < nContSize; i++ )
					{
						int ix = int(imgContour->ppoints[i].x*Del);
						int iy = int(imgContour->ppoints[i].y*Del);

						CRect rcObj( ix, iy, ix + w, iy + h );

						if( _check_corners( rcObj, rcClient, pSourceImgMask, int(W*Del), int(H*Del), CPoint( int(pt.x*Del), int(pt.y*Del)), &ix, &iy, punkObject ) )
						{
							outRect.left = ix;
							outRect.top  = iy;

							bOK = true;
							break;
						}
					}

					if( !bOK )
					{
						for( i = 0; i < nID; i++ )
						{
							int ix = int(imgContour->ppoints[i].x*Del);
							int iy =  int(imgContour->ppoints[i].y*Del);

							CRect rcObj( ix, iy, ix + w, iy + h );

							if( _check_corners( rcObj, rcClient, pSourceImgMask, int(W*Del), int(H*Del), CPoint( int(pt.x*Del), int(pt.y*Del) ), &ix, &iy, punkObject ) )
							{
								outRect.left = ix;
								outRect.top  = iy;
								break;
							}
						}
					}
					break;
				}
			}
		}
	}
	if( !bCh  )
	{
		outRect.left = ptx;
		outRect.top  = pty;
	}

		 
	for( y = 0; y < H;y++ )
		delete []pSourceImgMask[y];
	delete []pSourceImgMask;

	outRect.left = long( outRect.left / Del );
	outRect.top  = long( outRect.top / Del );

	*out = outRect;
}


bool CImageSourceView::_check_corners( RECT rcObj, RECT rcClient, BYTE **pSourceImgMask, int W, int H, POINT pt, int *pX, int *pY, IUnknown *punkObject )
{
	int ix = *pX, iy = *pY;
	double Del = m_bFromPrint ? m_fZoom : 1;


	CRect rc = rcObj;

	for( int j = 0; j < 4;j++ )
	{
		bool bOK = true; 


		int nCol = 17;
		CPoint pts[17] = { rc.TopLeft(), CPoint( rc.right, rc.top ), rc.BottomRight(), CPoint( rc.left, rc.bottom ), rc.CenterPoint(),
						  CPoint( rc.left + (rc.right - rc.left)/4, rc.top ), CPoint( rc.left + (rc.right - rc.left)/2, rc.top ), CPoint( rc.left + (rc.right - rc.left)*3/4, rc.top ),
						  CPoint( rc.left + (rc.right - rc.left)/4, rc.bottom ), CPoint( rc.left + (rc.right - rc.left)/2, rc.bottom ), CPoint( rc.left + (rc.right - rc.left)*3/4, rc.bottom ),
						  CPoint( rc.left, rc.top + (rc.bottom - rc.top)/4 ), CPoint( rc.left, rc.top + (rc.bottom - rc.top)/2 ), CPoint( rc.left, rc.top + (rc.bottom - rc.top)*3/4 ),
						  CPoint( rc.right, rc.top + (rc.bottom - rc.top)/4 ), CPoint( rc.right, rc.top + (rc.bottom - rc.top)/2 ), CPoint( rc.right, rc.top + (rc.bottom - rc.top)*3/4 )};

		if( bOK )
		{
			int nSz = 0;

			for( int i = 0; i < 4; i++ )
			{
				if( !( pts[i].x >= rcClient.left && pts[i].y >= rcClient.top && 
				  	   pts[i].x < rcClient.right && 
				 	   pts[i].y < rcClient.bottom ) )
				{
					bOK = false;
					break;
				}
			}

			if( bOK )
			{
				int nMax = 1;
				if( Del >= 5 )
					nMax = 2;
				if( Del >= 9 )
					nMax = 3;
				if( Del >= 9 )
					nMax = 4;

				for( i = 0; i < nCol; i++ )
				{

					int py = pts[i].y - pt.y, px = pts[i].x - pt.x;

					if( py >= 0 && py < H && px >= 0 && px < W )
					{
						if( pSourceImgMask[int(py/Del)][int(px/Del)] == 0xFF ) 
						{
							nSz++;
							if( nSz > nMax )
								break;
						}
					}
				}

				if( nSz > nMax || !nSz )
					bOK = false;
			}
		}

		if( bOK )
			bOK = _check_cross( rc, punkObject );

		if( bOK )
		{
			*pX = rc.left;
			*pY = rc.top;

			return true;
		}

		rc = rcObj;
		switch( j )
		{
		case 0: rc.OffsetRect( -rc.Width(), -rc.Height() );
				break;
		case 1: rc.OffsetRect( 0, -rc.Height());
				break;
		case 2: rc.OffsetRect( -rc.Width(), 0 );
				break;
		}
	}

	return false;
}

bool CImageSourceView::_check_cross( CRect rc, IUnknown *punkObject )
{
	bool bOK = true;
	double Del = m_bFromPrint ? m_fZoom : 1;

	IUnknown *ptrDocList = ::GetActiveObjectFromContext( GetControllingUnknown(), szTypeObjectList );

	if( !ptrDocList )
		return true;

	INamedDataObject2Ptr	ptrListObject = ptrDocList;
	ptrDocList->Release();

	if( ptrListObject == 0 )
		return true;
	if( bOK )
	{
		long Pos = 0;
		ptrListObject->GetFirstChildPosition( &Pos );

		while( Pos )
		{
			IUnknown *pChild = 0;
			ptrListObject->GetNextChild( &Pos, &pChild );

			if( pChild )
			{
				CObjectWrp object(pChild, true);

				IUnknown *punk = object.GetImage();
				
				IImage3Ptr ptrI = punk;

				if( punk )
					punk->Release();

				if( ptrI != 0 && GetObjectKey( punkObject ) != GetObjectKey( pChild ) )
				{
					int W = 0, H = 0;
					POINT pt;

					ptrI->GetSize( &W, &H );
					ptrI->GetOffset( &pt );
					pt = CPoint( int(pt.x * Del), int(pt.y * Del) );

					CRect imageRc( pt, CSize( int(W * Del), int(H * Del) ) );
					
//					rc.DeflateRect( 2,2, 2, 2 );
					BOOL bOk = imageRc.IntersectRect( imageRc, rc );
					if( bOk )
					{
						int nCol = 17;
						CPoint pts[17] = { rc.TopLeft(), CPoint( rc.right, rc.top ), rc.BottomRight(), CPoint( rc.left, rc.bottom ), rc.CenterPoint(),
										  CPoint( rc.left + (rc.right - rc.left)/4, rc.top ), CPoint( rc.left + (rc.right - rc.left)/2, rc.top ), CPoint( rc.left + (rc.right - rc.left)*3/4, rc.top ),
										  CPoint( rc.left + (rc.right - rc.left)/4, rc.bottom ), CPoint( rc.left + (rc.right - rc.left)/2, rc.bottom ), CPoint( rc.left + (rc.right - rc.left)*3/4, rc.bottom ),
										  CPoint( rc.left, rc.top + (rc.bottom - rc.top)/4 ), CPoint( rc.left, rc.top + (rc.bottom - rc.top)/2 ), CPoint( rc.left, rc.top + (rc.bottom - rc.top)*3/4 ),
										  CPoint( rc.right, rc.top + (rc.bottom - rc.top)/4 ), CPoint( rc.right, rc.top + (rc.bottom - rc.top)/2 ), CPoint( rc.right, rc.top + (rc.bottom - rc.top)*3/4 )};

						for( int i = 0; i < nCol; i++ )
						{

							int py = pts[i].y - pt.y, px = pts[i].x - pt.x;

							if( py >= 0 && py < H * Del && px >= 0 && px < W * Del )
							{
								LPBYTE pRow = 0;
								ptrI->GetRowMask( int(py / Del), &pRow );
								
								if( pRow && pRow[int(px / Del)] == 0xFF ) 
								{
									bOK = false;
									break;
								}
							}
						}
					}
				}

				if( bOK )
				{
					if( GetObjectKey( punkObject ) != GetObjectKey( pChild ) )
					{
						long lPos = m_object_coords.find( (long)pChild );

						if( lPos )
						{
							CRect rcTxt = m_object_coords.get( lPos );

							if( rcTxt.IntersectRect( rcTxt, rc ) )
								bOK = false;
						}
					}
				}

				pChild->Release();

				if( !bOK )
					break;
			}
		}
	}

	if( bOK )
	{
		long Pos = 0;
		ptrListObject->GetFirstChildPosition( &Pos );

		while( Pos )
		{
			IUnknown *pChild = 0;
			ptrListObject->GetNextChild( &Pos, &pChild );

			if( pChild)
			{
				if( GetObjectKey( punkObject ) != GetObjectKey( pChild ) )
				{
					long lPos = m_object_coords_num.find( (long)pChild );

					if( lPos )
					{
						CRect rcTxt = m_object_coords_num.get( lPos );

						if( rcTxt.IntersectRect( rcTxt, rc ) )
							bOK = false;
					}
				}
				pChild->Release();

				if( !bOK )
					break;
			}
		}
	}

	return bOK;
}

void CImageSourceView::_recalc_rect(BOOL bUpdate/* = false*/, IUnknown *punkExclude/* = 0*/)
{
	IUnknown* punk_drawing = ::GetActiveObjectFromContext( GetControllingUnknown(), szDrawingObjectList );

	INamedDataObject2Ptr	ptr_objects = punk_drawing;
	if( punk_drawing )
		punk_drawing->Release();	punk_drawing = 0;

	if( ptr_objects != 0 )
	{
		long lpos = 0;
		ptr_objects->GetFirstChildPosition( &lpos );

		while( lpos ) 
		{
			IUnknown	*punk = 0;
			ptr_objects->GetNextChild( &lpos, &punk );
			
			if( bUpdate && punkExclude && ::GetObjectKey( punkExclude ) == ::GetObjectKey( punk ) )
			{
				punk->Release();	punk = 0;
				continue;
			}

			if( punk )
			{
				CRect	rect = GetObjectRect( punk );
				
				m_size_client.cx = max( rect.right, m_size_client.cx );
				m_size_client.cy = max( rect.bottom, m_size_client.cy );
				punk->Release();	punk = 0;
			}
		}
	}

	//paul 17.04.2003
	if( m_image != 0 && !is_enable_exended_move() )
		m_size_client = CSize( m_image.GetWidth(), m_image.GetHeight() );


	if( bUpdate )
		_UpdateScrollSizes();
}

//allow move object outside image rect
bool CImageSourceView::is_enable_exended_move()
{
	if( m_lenable_exended_move == -1 )
		m_lenable_exended_move = ::GetValueInt( ::GetAppUnknown(), "\\ImageView", "EnableMoveOutside", 0 );

	return ( m_lenable_exended_move == 1L );
}

void CImageSourceView::OnSize(UINT nType, int cx, int cy) 
{
	bool bCenterView = _get_center_mode();
	
	__super::OnSize(nType, cx, cy);
	if(_IsCalibrVisible() || bCenterView )
	{
		InvalidateRect( 0 ); // обновим все окно - хотя лучше бы ректы посчитать...
	}
}

CRect CImageSourceView::_VisibleImageRect()
{	//возвращает прямоугольник, содержащий видимую в окне часть image
	// дублирующийся код определения image rect - вынести в ф-ю
	CRect	rect;
	GetClientRect( rect );

	//get the image rectangle
	CSize	size_cur = m_size_client;

	CRect	rectImage;

	rectImage.left = int(ceil( 0*m_fZoom-m_pointScroll.x));
	rectImage.right = int(floor((size_cur.cx)*m_fZoom-m_pointScroll.x + .5));
	rectImage.top = int(ceil( 0*m_fZoom-m_pointScroll.y));
	rectImage.bottom = int(floor((size_cur.cy)*m_fZoom-m_pointScroll.y + .5));

	rect &= rectImage;

	return rect;
}

BOOL CImageSourceView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_MESSAGE_ENABLE_CENTER )
	{
		*pResult = _get_center_mode();
		return TRUE;
	}

	return __super::OnWndMsg( message, wParam, lParam, pResult);
}

void	CImageSourceView::_UpdateAviPropPage( bool bShow /*= true*/ )
{
	if( bShow )
	{
		IAviImagePtr sptr_avi = IsPreviewModeActive() ? m_imageSource : m_image;        		
		bShow = (sptr_avi != 0);
	}

	IUnknown *punkPage = 0;
	int nPageNum = -1;
	if( ::GetPropertyPageByProgID( _T(szAviPlayPageProgID), &punkPage, &nPageNum) )
	{
		::ShowPropertyPage( nPageNum, bShow );
		IAviPlayPropPagePtr sptr_avipage = punkPage;
		if( punkPage )
			punkPage->Release();

		if( sptr_avipage != 0 )
			sptr_avipage->UpdateCtrls( );
	}        
}

IMPLEMENT_UNKNOWN(CImageSourceView, MenuInitializer)

HRESULT CImageSourceView::XMenuInitializer::OnInitPopup(
	BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu )
{
	_try_nested(CImageSourceView, MenuInitializer, OnInitPopup)
	{
		UINT posSetClass;
		*phOutMenu = ::CopyMenuSkipSubMenu(hMenu,pThis->m_iSkip, posSetClass);
		if(posSetClass>=0)
		{
			if (!*phOutMenu) return S_FALSE;

			IUnknown* punkObject = pThis->m_pframe->GetObjectByIdx( 0 );
			ICalcObjectPtr sptrCO(punkObject);
			if(punkObject) punkObject->Release();
			long nClass = get_object_class( sptrCO );

			IUnknownPtr punkAM(::_GetOtherComponent(::GetAppUnknown(), IID_IActionManager), false);
			sptrIActionManager sptrAM(punkAM);
			DWORD	dwFlags = 0;
			sptrAM->GetActionFlags( _bstr_t("SetClass"), &dwFlags );


			if(sptrCO != 0 && (dwFlags&afEnabled))
			{
				UINT uState = (sptrCO != 0) && (dwFlags&afEnabled) ? MF_ENABLED : (MF_DISABLED | MF_GRAYED);

				::InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

				long nClassCount = class_count();
				for(int i=0; i<nClassCount; i++)
				{
					_bstr_t bstr = get_class_name( i );
					UINT uState2 = (i==nClass) ? MF_GRAYED | MF_CHECKED : MF_ENABLED;
					::InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_STRING | uState | uState2,
						ID_CLASSMENU_BASE+1+i, bstr);
				}
				if(pThis->m_bShowUnknown)
				{
					_bstr_t bstr = get_class_name( -1 );
					UINT uState2 = (-1==nClass) ? MF_GRAYED | MF_CHECKED : MF_ENABLED;
					::InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_STRING | uState | uState2,
						ID_CLASSMENU_BASE, bstr);
				}
			}
		}
		//pThis->_ChangeSelectionMode( lSelectType );

		return S_OK;
	/////////////////////////////
#if 0
		DestroyPopup();

		if( m_ptrSubMenu == NULL )
			return;


		//At first clear
		CreateFromMenu( NULL );

		m_ptrSubMenu->SetStartItemID( ID_CMD_POPUP_MIN );

		m_ptrSubMenu->SetSingleObjectName( _bstr_t( m_strText ) );

		long lPos = 0;	
		m_ptrSubMenu->GetFirstItemPos( &lPos );
		
		
		//For destroy
		CArray<CMenuInfo*,CMenuInfo*> arMenu;

		CMenuInfo* pRootMenu = new CMenuInfo( -1 );	
		arMenu.Add( pRootMenu );	

		while( lPos )
		{
			UINT uiFlags	= 0;
			BSTR bstrText	= 0;
			UINT uiItemID	= 0;
			UINT uiParentID	= -1;
			//long lCurPos	= lPos;

			m_ptrSubMenu->GetNextItem( &uiFlags, &uiItemID, &bstrText, &uiParentID, &lPos );
			CString strText = bstrText;
			::SysFreeString( bstrText );		

			bool bPopupMenu = false;
			if( uiFlags == MF_POPUP )
			{
				CMenuInfo* pMenu = new CMenuInfo( uiItemID );			
				arMenu.Add( pMenu );	
				bPopupMenu = true;
			}
			

			HMENU hMenuAppend = FindMenuByPos( uiParentID, arMenu );

			::AppendMenu( hMenuAppend, bPopupMenu ? MF_POPUP : uiFlags, 
				bPopupMenu ? (UINT)FindMenuByPos( uiItemID, arMenu ) : uiItemID, (LPCTSTR)strText );


			//menu.AppendMenu( uiFlags, uiItemID, strText );		
		}	
		

		CreateFromMenu( FindMenuByPos( -1/*Root*/, arMenu ) );

		for( int i=0;i<arMenu.GetSize();i++ )
		{
			delete arMenu[i];		
		}

		arMenu.RemoveAll();
#endif
	/////////////////////////////////

	}
	_catch_nested;
}
