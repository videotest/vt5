// Image.cpp : Implementation of the CVTImage ActiveX Control class.

#include "stdafx.h"
#include "VTDesign.h"
#include "Image.h"
#include "ImagePpg.h"
#include <msstkppg.h>
#include <vfw.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CVTImage, COleControl)


BEGIN_INTERFACE_MAP(CVTImage, COleControl)
	INTERFACE_PART(CVTImage, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CVTImage, IID_IAXCtrlDataSite, AXCtrlDataSite)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTImage, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CVTImage)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTImage, COleControl)
	//{{AFX_MSG_MAP(CVTImage)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTImage, COleControl)
	//{{AFX_DISPATCH_MAP(CVTImage)	
	DISP_PROPERTY_EX(CVTImage, "TransparentColor", GetTransparentColor, SetTransparentColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTImage, "PermanentImage", GetPermanentImage, SetPermanentImage, VT_I2)
	DISP_PROPERTY_EX(CVTImage, "FileName", GetFileName, SetFileName, VT_BSTR)	
	DISP_PROPERTY_EX(CVTImage, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CVTImage, "BackgroundColor", GetBackgroundColor, SetBackgroundColor, VT_COLOR)
	DISP_FUNCTION(CVTImage, "ReloadImage", ReloadImage, VT_BOOL, VTS_NONE)	
	DISP_FUNCTION(CVTImage, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTImage, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTImage, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTImage, COleControl)
	//{{AFX_EVENT_MAP(CVTImage)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CVTImage, 1)
	PROPPAGEID(CVTImagePropPage::guid)
//	PROPPAGEID(CLSID_StockColorPage)
END_PROPPAGEIDS(CVTImage)
*/
static CLSID pages[1];

LPCLSID CVTImage::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CVTImagePropPage::guid;
	m_PropertyAXStatus=true;

	return pages;
}
/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTImage, "VTDESIGN.VTDImageCtrl.1",
	0xbc127b1b, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTImage, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTDImage =
		{ 0xbc127b19, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const IID BASED_CODE IID_DVTDImageEvents =
		{ 0xbc127b1a, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTDImageOleMisc =
//	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTImage, IDS_VTDIMAGE, _dwVTDImageOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTImage::CVTImageFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTImage

BOOL CVTImage::CVTImageFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_VTDIMAGE,
			IDB_VTDIMAGE,
			afxRegApartmentThreading,
			_dwVTDImageOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTImage::CVTImage - Constructor

CVTImage::CVTImage()
{
	InitializeIIDs(&IID_DVTDImage, &IID_DVTDImageEvents);
	InitDefaults();
	//added by akm 22_11_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTImage::~CVTImage - Destructor

CVTImage::~CVTImage()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTImage::DoPropExchange - Persistence support

void CVTImage::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	long nVersion = 1;
	PX_Long(pPX, "Version", nVersion );

	if( pPX->IsLoading() ){}

}


/////////////////////////////////////////////////////////////////////////////
// CVTImage::OnResetState - Reset control to default state

void CVTImage::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CVTImage::AboutBox - Display an "About" box to the user

void CVTImage::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTDIMAGE);
	dlgAbout.DoModal();
}



/////////////////////////////////////////////////////////////////////////////
HRESULT CVTImage::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CVTImage, PrintCtrl)			
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTImage::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CVTImage, PrintCtrl)		
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTImage::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CVTImage, PrintCtrl)	
	*pbUpdate = FALSE;
	return S_OK;

}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTImage::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CVTImage, PrintCtrl)	
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTImage::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CVTImage, PrintCtrl)	
	return S_OK;
}


class DrawDib
{
public:
	DrawDib()
	{	m_hDrawDib = ::DrawDibOpen();	}
	~DrawDib()
	{	::DrawDibClose( m_hDrawDib );	}
	operator HDRAWDIB()
	{	return m_hDrawDib;	}
protected:
	HDRAWDIB	m_hDrawDib;
};

static DrawDib	DrawDibInstance;

#define NEW_PRINT

void CVTImage::DrawContext( HDC hDC, SIZE size/* in himetrics*/ )	
{
	const int n = 3;

	CDC* pdc = CDC::FromHandle( hDC );
	VERIFY( pdc );

	if( m_nImageIndex < 0 )
	{
		pdc->FillRect( &CRect( 0, 0, size.cx, size.cy ), &CBrush( ::GetSysColor( COLOR_APPWORKSPACE ) ) );
		return;
	}

	CDC dcImage;
	dcImage.CreateCompatibleDC( pdc );	

	BITMAPINFOHEADER	bmih = {0};
	bmih.biBitCount = 24;
	// vanek
	bmih.biHeight = m_nImageHeight + 2;
	bmih.biWidth = m_nImageWidth + 2;
	bmih.biSize = sizeof( bmih );
	bmih.biPlanes = 1;		
	
	if( bmih.biHeight == 0 ||bmih.biWidth == 0 )
		return;

	//vanek - 13.10.2003
	BITMAPINFOHEADER	bmih_big = {0};  
	bmih_big.biBitCount = 24;
	bmih_big.biHeight = bmih.biHeight * n - (n-1);
	bmih_big.biWidth = bmih.biWidth * n - (n-1);
	bmih_big.biSize = sizeof( bmih_big );
	bmih_big.biPlanes = 1;		
	
	if( bmih_big.biHeight == 0 || bmih_big.biWidth == 0 )
		return;

	byte	*pdibBits = 0;
	HBITMAP	hDIBSection = ::CreateDIBSection( dcImage, (BITMAPINFO*)&bmih, DIB_PAL_COLORS, (void**)&pdibBits, 0, 0 );
	ASSERT( hDIBSection );

	byte	*pdibBitsBig = 0;
	HBITMAP	hDIBSectionBig = ::CreateDIBSection( dcImage, (BITMAPINFO*)&bmih_big, DIB_PAL_COLORS, (void**)&pdibBitsBig, 0, 0 );
    ASSERT( hDIBSectionBig );
	////////////

	CBitmap *psrcBitmap = CBitmap::FromHandle( hDIBSection  );
	ASSERT( psrcBitmap );

	//propare the memory DC to drawing
	CBitmap *poldBmp = dcImage.SelectObject( psrcBitmap );

	dcImage.FillRect( &CRect( 0, 0, size.cx, size.cy ), &CBrush( TranslateColor( m_BackgroundColor ) ) );

	// vanek
	m_ImageList.Draw(   &dcImage,
						m_nImageIndex,
						CPoint(1,1),
						ILD_NORMAL
						);

	// билинейна€ интерпол€ци€ - monster
	int cx4 = ( bmih.biWidth * 3 + 3 ) / 4 * 4;
 	int cx4Big = ( bmih_big.biWidth * 3 + 3 ) / 4 * 4;


	for( int y = 0; y < bmih.biHeight; y ++ )
	{
		pdibBits[y * cx4] = pdibBits[y * cx4 + 3];
		pdibBits[y*cx4 + (bmih.biWidth - 1) * 3] = pdibBits[ y * cx4 + max(bmih.biWidth - 2, 1) * 3];

		pdibBits[y * cx4 + 1] = pdibBits[y * cx4 + 3 + 1];
		pdibBits[y * cx4 + (bmih.biWidth - 1) * 3 + 1] = pdibBits[ y * cx4 + max(bmih.biWidth - 2, 1) * 3 + 1];

		pdibBits[y * cx4 + 2] = pdibBits[y * cx4 + 3 + 2];
		pdibBits[y * cx4 + (bmih.biWidth - 1) * 3 + 2] = pdibBits[ y * cx4 + max(bmih.biWidth - 2, 1) * 3 + 2];
	}  	

	for( int x = 0; x < bmih.biWidth; x ++ )
	{
		pdibBits[x * 3] = pdibBits[cx4 + x * 3];
		pdibBits[ (bmih.biHeight - 1) * cx4 + x * 3] = pdibBits[ max(bmih.biHeight - 2, 1) * cx4 + x * 3];

		pdibBits[x * 3 + 1] = pdibBits[cx4 + x * 3 + 1];
		pdibBits[(bmih.biHeight - 1) * cx4 + x * 3 + 1] = pdibBits[ max(bmih.biHeight - 2, 1) * cx4 + x * 3 + 1];

		pdibBits[x * 3 + 2] = pdibBits[cx4 + x * 3 + 2];
		pdibBits[(bmih.biHeight - 1) * cx4 + x * 3 + 2] = pdibBits[ max(bmih.biHeight - 2, 1) * cx4 + x * 3 + 2];
	}  	

	int y;
	for(y = 0; y < bmih.biHeight - 1; y ++)
	{
		for(int x = 0; x < bmih.biWidth; x ++)
		{
			for(int yy = 0; yy < n; yy ++) 
			{
				pdibBitsBig[ (y * n + yy) * cx4Big + x * n * 3] =
					(pdibBits[ y * cx4 + x * 3 ] * (n - yy) + pdibBits[ (y + 1) * cx4 + x * 3 ] * yy) / n;
				pdibBitsBig[ (y * n + yy) * cx4Big + x * 3 * n + 1] =
					(pdibBits[ y * cx4 + x * 3 + 1 ] * (n - yy) + pdibBits[ (y + 1) * cx4 + x * 3 + 1 ] * yy) / n;
				pdibBitsBig[ (y * n + yy) * cx4Big + x * 3 * n + 2 ] =
					(pdibBits[ y * cx4 + x * 3 + 2 ] * (n - yy) + pdibBits[ (y + 1) * cx4 + x * 3 + 2 ] * yy) / n;
			}

		}
	}
	y = bmih.biHeight - 1;
	for(int x = 0; x < bmih.biWidth; x ++)
	{
		pdibBitsBig[ (y * n) * cx4Big + x * n * 3 ] =
			pdibBits[ y * cx4 + x * 3 ];
		pdibBitsBig[ (y * n) * cx4Big + x * 3 * n + 1 ] =
			pdibBits[ y * cx4 + x * 3 + 1 ];
		pdibBitsBig[ (y * n) * cx4Big + x * 3 * n + 2 ] =
			pdibBits[ y * cx4 + x * 3 + 2 ];
	}  	

    for(int y = 0; y <= (bmih.biHeight - 1) * n; y ++)
	{
		for(int x = 0; x < bmih.biWidth - 1; x ++)
		{
			for(int xx = 1; xx < n; xx ++)
			{
				pdibBitsBig[ y * cx4Big + (x * n + xx) * 3 ] = 
					(pdibBitsBig[y*cx4Big+x*n*3]*(n-xx) + pdibBitsBig[y*cx4Big+(x+1)*n*3]*xx)/n;
				pdibBitsBig[ y * cx4Big + (x * n + xx) * 3 + 1 ] =
					(pdibBitsBig[y*cx4Big+x*n*3+1]*(n-xx) + pdibBitsBig[y*cx4Big+(x+1)*n*3+1]*xx)/n;
				pdibBitsBig[ y *cx4Big + (x * n + xx) * 3 + 2 ] =
					(pdibBitsBig[y*cx4Big+x*n*3+2]*(n-xx) + pdibBitsBig[y*cx4Big+(x+1)*n*3+2]*xx)/n;
			}

		}
	}

	CSize sizeMM( size );
	pdc->LPtoDP( &sizeMM );

	int nOldMode = pdc->SetMapMode( MM_TEXT );
	
	pdc->SetWindowExt( CSize( sizeMM.cx, -sizeMM.cy ) );
	pdc->SetWindowOrg( CPoint( 0, 0 ) );
	
#ifdef NEW_PRINT
	if( nOldMode == MM_ANISOTROPIC )	 //[max] sbt:725
		::DrawDibDraw( DrawDibInstance, hDC, 0, -sizeMM.cy, sizeMM.cx, sizeMM.cy, &bmih_big, pdibBitsBig, 0, 0, -1, -1, 0 );
	else
	::DrawDibDraw( DrawDibInstance, hDC, 0, 0, sizeMM.cx, sizeMM.cy, &bmih_big, pdibBitsBig, 0, 0, -1, -1, 0 );
#else
	::DrawDibDraw( DrawDibInstance, hDC, 0, 0, sizeMM.cx, sizeMM.cy, &bmih_big, pdibBitsBig, 0, 0, -1, -1, 0 );
#endif//NEW_PRINT
	
	pdc->SetMapMode( nOldMode );

	dcImage.SelectObject( poldBmp );

	if( hDIBSection )
	{
		::DeleteObject( hDIBSection );
		hDIBSection = 0;
	}

	if( hDIBSectionBig )
	{
		::DeleteObject( hDIBSectionBig );
		hDIBSectionBig = 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CVTImage message handlers

DWORD CVTImage::GetControlFlags() 
{		
	return COleControl::GetControlFlags();// | windowlessActivate;
}

BOOL CVTImage::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_TRANSPARENT;	
	return COleControl::PreCreateWindow(cs);
}

BOOL CVTImage::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CVTImage::InitDefaults()
{
	//m_BackColor			= 0x80000000 + COLOR_WINDOW;	
	m_TransparentColor	= 0x80000000 + COLOR_WINDOW;		
	m_BackgroundColor	= 0x80000000 + COLOR_WINDOW;

	m_nImageIndex		= -1;

	m_bUsePermanentImage = TRUE;
	//m_bTransparentBack   = FALSE;

	CreateDefaultImageList();
}

void CVTImage::CreateDefaultImageList()
{
	m_nImageIndex			= -1;
	m_nImageWidth = m_nImageHeight = 0;

	m_ImageList.DeleteImageList();
	VERIFY( m_ImageList.Create( 1, 1, ILC_COLOR16, 1, 0 ) );
	/*
	CBitmap bitmap;
	bitmap.LoadBitmap( IDB_VTDIMAGE );
	m_nImageWidth = m_nImageHeight = 16;
	m_nImageIndex = m_ImageList.Add( &bitmap, RGB(202,5,25) );
	*/


	/*
	HICON hIcon = NULL;

	//AFX_MANAGE_STATE(AfxGetStaticModuleState()); 	
	hIcon = LoadIcon(AfxGetInstanceHandle( ), "IDI_ICON_LOAD" );
	
	m_nImageIndex = m_ImageList.Add( hIcon );
	::DestroyIcon( hIcon );
	*/
}


BOOL CVTImage::ReadPictureFromFile()
{
	HBITMAP hBitmap = NULL;
	try
	{	    
		CreateDefaultImageList();		

		if( m_strFileName.IsEmpty() )
			return FALSE;

		hBitmap = NULL;
		hBitmap = (HBITMAP)LoadImage( NULL, m_strFileName, IMAGE_BITMAP, 0, 0,
					 /*LR_CREATEDIBSECTION | LR_DEFAULTSIZE | */LR_LOADFROMFILE );

		if( hBitmap )
		{
			CBitmap bmp;
			bmp.Attach (hBitmap);
			BITMAP bitmap;
			bmp.GetBitmap (&bitmap);

			m_nImageWidth = bitmap.bmWidth;
			m_nImageHeight = bitmap.bmHeight;

			m_ImageList.DeleteImageList();


			if( !m_ImageList.Create( m_nImageWidth, m_nImageHeight, ILC_COLOR16|ILC_MASK, 2, 0 ) )
			{
				if( hBitmap ) DeleteObject( hBitmap );

				m_strFileName.Empty( );
				CreateDefaultImageList();
				return FALSE;
			}

			m_nImageIndex = m_ImageList.Add( &bmp, TranslateColor(m_TransparentColor) );

			DeleteObject( hBitmap );
		}
	}
	catch(CException* pExc)
	{
		pExc->ReportError();
		pExc->Delete();

		if( hBitmap ) DeleteObject( hBitmap );

		m_strFileName.Empty();
		CreateDefaultImageList();		

		return FALSE;

	}

	return TRUE;
}


void CVTImage::Serialize(CArchive& ar)
{	
    SerializeStockProps(ar);
	long nVersion = 3;

	OLE_COLOR m_BackColor = 0;
	BOOL m_bTransparentBack = 0;

	if( ar.IsLoading() )
	{		
		ar >> nVersion;
		ar >> m_BackColor;
		ar >> m_TransparentColor;
		ar >> m_bUsePermanentImage;
		ar >> m_strFileName;
		ar >> m_nImageIndex;
		ar >> m_nImageWidth;
		ar >> m_nImageHeight;

		m_ImageList.DeleteImageList( );
		m_ImageList.Read( &ar );

		if( nVersion >= 2 )
			ar >> m_bTransparentBack;

		if( nVersion >= 3 )
			ar >> m_BackgroundColor;


		if( !m_bUsePermanentImage )
			ReadPictureFromFile();		

		InvalidateCtrl( this, m_pInPlaceSiteWndless );


	}
	else
	{
		ar << nVersion;

		ar << m_BackColor;
		ar << m_TransparentColor;
		ar << m_bUsePermanentImage;
		ar << m_strFileName;
		ar << m_nImageIndex;
		ar << m_nImageWidth;
		ar << m_nImageHeight;

		m_ImageList.Write( &ar );
		ar << m_bTransparentBack;

		ar << m_BackgroundColor;


	}

	

}

/////////////////////////////////////////////////////////////////////////////
// CVTImage::OnDraw - Drawing function

void CVTImage::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rcArea = rcBounds;
	rcArea.DeflateRect( 0, 0, 1, 1 );

	if( m_nImageIndex < 0 )
	{
		pdc->FillRect( &rcArea, &CBrush( ::GetSysColor( COLOR_APPWORKSPACE ) ) );
		return;
	}


	SIZE sizeHM;
	sizeHM.cx = FROM_DOUBLE_TO_VTPIXEL( rcArea.Width() );
	sizeHM.cy = FROM_DOUBLE_TO_VTPIXEL( rcArea.Height() );

	int nOldMapMode = pdc->SetMapMode( MM_ANISOTROPIC );
	pdc->SetWindowExt( sizeHM.cx, -sizeHM.cy );
	pdc->SetViewportExt( rcArea.Width(), rcArea.Height() );	

#ifdef NEW_PRINT
	pdc->SetViewportOrg( rcArea.left, rcArea.Height() + rcArea.top );
#else
	pdc->SetViewportOrg( rcArea.left, 0/*rcArea.Height() + rcArea.top*/ );
#endif//NEW_PRINT

	DrawContext( pdc->GetSafeHdc(), sizeHM );

	pdc->SetMapMode( nOldMapMode );
}

/////////////////////////////////////////////////////////////////////////////
// CVTLine  Properties Access
/////////////////////////////////////////////////////////////////////////////

OLE_COLOR CVTImage::GetBackgroundColor() 
{	
	return m_BackgroundColor;
}

void CVTImage::SetBackgroundColor(OLE_COLOR nNewValue) 
{
	m_BackgroundColor = nNewValue;
	BoundPropertyChanged( 5 );
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}


/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTImage::GetTransparentColor() 
{	
	return m_TransparentColor;
}

void CVTImage::SetTransparentColor(OLE_COLOR nNewValue) 
{
	m_TransparentColor = nNewValue;
	BoundPropertyChanged( 1 );
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
short CVTImage::GetPermanentImage() 
{	
	return (short)m_bUsePermanentImage;
}

void CVTImage::SetPermanentImage(short bNewValue) 
{
	m_bUsePermanentImage = (BOOL)bNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BSTR CVTImage::GetFileName() 
{
	CString strResult = m_strFileName;	
	return strResult.AllocSysString();
}

void CVTImage::SetFileName(LPCTSTR lpszNewValue) 
{
	if( m_strFileName == lpszNewValue )
		return;

	m_strFileName = lpszNewValue;
	ReadPictureFromFile();
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTImage::ReloadImage() 
{
	ReadPictureFromFile();
	return TRUE;
}

/*
BOOL CVTImage::GetTransparentBack() 
{
	return m_bTransparentBack;
}

void CVTImage::SetTransparentBack(BOOL bNewValue) 
{
	m_bTransparentBack = bNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}
*/

BOOL CVTImage::GetVisible() 
{
	return ::_IsWindowVisible( this );
}

void CVTImage::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}

//added by akm 22_11_k5
BOOL CVTImage::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CVTImage::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}