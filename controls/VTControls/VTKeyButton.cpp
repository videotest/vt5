// VTKeyButton.cpp : Implementation of the CVTKeyButton ActiveX Control class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTKeyButton.h"
#include "VTKeyButtonPpg.h"
#include <msstkppg.h>
#include "VTKeyButtonImagePropPage.h"
#include "\vt5\common\utils.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif




IMPLEMENT_DYNCREATE(CVTKeyButton, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTKeyButton, COleControl)
	//{{AFX_MSG_MAP(CVTKeyButton)
	ON_WM_CREATE()	
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTKeyButton, COleControl)
	//{{AFX_DISPATCH_MAP(CVTKeyButton)
	DISP_PROPERTY_EX(CVTKeyButton, "VirtKey", GetVirtKey, SetVirtKey, VT_I4)
	DISP_PROPERTY_EX(CVTKeyButton, "ImageWidth", GetImageWidth, SetImageWidth, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "ImageHeight", GetImageHeight, SetImageHeight, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "BmpFileName", GetBmpFileName, SetBmpFileName, VT_BSTR)	
	DISP_PROPERTY_EX(CVTKeyButton, "ImageMask", GetImageMask, SetImageMask, VT_COLOR)	
	DISP_PROPERTY_EX(CVTKeyButton, "ImageIndex", GetImageIndex, SetImageIndex, VT_I2)			
	DISP_PROPERTY_EX(CVTKeyButton, "ImageAlignHorz", GetImageAlignHorz, SetImageAlignHorz, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "ImageAlignVert", GetImageAlignVert, SetImageAlignVert, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "ImageDeltaX", GetImageDeltaX, SetImageDeltaX, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "ImageDeltaY", GetImageDeltaY, SetImageDeltaY, VT_I2)	
	DISP_FUNCTION(CVTKeyButton, "SetImageList", SetImageList, VT_BOOL, VTS_BSTR VTS_I2 VTS_I2 VTS_COLOR)
	DISP_FUNCTION(CVTKeyButton, "ReloadImageList", ReloadImageList, VT_BOOL, VTS_NONE)		
	DISP_PROPERTY_EX(CVTKeyButton, "ControlInfo", GetControlInfo, SetControlInfo, VT_BSTR)	
	DISP_PROPERTY_EX(CVTKeyButton, "IconDeltaX", GetIconDeltaX, SetIconDeltaX, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "IconDeltaY", GetIconDeltaY, SetIconDeltaY, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "TextDeltaX", GetTextDeltaX, SetTextDeltaX, VT_I2)
	DISP_PROPERTY_EX(CVTKeyButton, "TextDeltaY", GetTextDeltaY, SetTextDeltaY, VT_I2)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTKeyButton, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTKeyButton, COleControl)
	//{{AFX_EVENT_MAP(CVTKeyButton)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTKeyButton, 3)
	PROPPAGEID(CVTKeyButtonPropPage::guid)
	PROPPAGEID(CVTKeyButtonImagePropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
END_PROPPAGEIDS(CVTKeyButton)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTKeyButton, "VTCONTROLS.VTKeyButton.1",
	0x6743979d, 0xd00f, 0x48a0, 0xa0, 0x36, 0xdd, 0x7e, 0xe3, 0x3a, 0x30, 0xb3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTKeyButton, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTKeyButton =
		{ 0xadd6255f, 0x8137, 0x4eaa, { 0x96, 0x7f, 0x3a, 0xc5, 0xca, 0x9a, 0x76, 0x7 } };
const IID BASED_CODE IID_DVTKeyButtonEvents =
		{ 0xd6b94f54, 0xfe1e, 0x4d28, { 0x9b, 0x84, 0x85, 0xf8, 0xba, 0x85, 0x38, 0x63 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTKeyButtonOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTKeyButton, IDS_VTKEYBUTTON, _dwVTKeyButtonOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton::CVTKeyButtonFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTKeyButton

BOOL CVTKeyButton::CVTKeyButtonFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTKEYBUTTON,
			IDB_VTKEYBUTTON,
			afxRegApartmentThreading,
			_dwVTKeyButtonOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton::CVTKeyButton - Constructor

CVTKeyButton::CVTKeyButton()
{
	InitializeIIDs(&IID_DVTKeyButton, &IID_DVTKeyButtonEvents);
	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::InitDefaults()
{
	m_bPressed = false;
	m_nVKey = 0;
	m_szText[0] = 0;
	m_hIcon = 0;	
	
	m_nTimerID = -1;

	CreateDefaultImageList();	

	m_nImageWidth	= -1;
	m_nImageHeight	= -1;
	m_clrImageMask	= 0x80000000 + COLOR_BTNFACE;

	m_nImageIndex	= -1;


	m_imageAlignHorz	= iahRight;
	m_imageAlignVert	= iavTop;
	m_imageDeltaX		= 0;
	m_imageDeltaY		= 0;


	m_iconDeltaX		= 0;
	m_iconDeltaY		= 0;
	m_textDeltaX		= 0;
	m_textDeltaY		= 0;

	m_rcAvailable = NORECT;
	

}

void CVTKeyButton::CreateDefaultImageList()
{
	if( m_ImageList.m_hImageList)
		m_ImageList.DeleteImageList();

	VERIFY( m_ImageList.Create( 1, 1, ILC_COLOR16|ILC_MASK, 1, 0 ) );
	//m_ImageList.SetBkColor( COLOR_BTN_FACE );	
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton::~CVTKeyButton - Destructor

CVTKeyButton::~CVTKeyButton()
{	
	// TODO: Cleanup your control's instance data here.
}

/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton::DoPropExchange - Persistence support
void CVTKeyButton::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	long nVersion = 1;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::Serialize( CArchive& ar )
{	
	long nVersion = 4;

	if( ar.IsStoring() )
	{
		ar<<nVersion;
		ar<<m_nVKey;
		ar<<m_strBmpFileName;
		m_ImageList.Write( &ar );		
		ar<<m_nImageWidth;
		ar<<m_nImageHeight;
		ar<<m_clrImageMask;
		ar<<m_nImageIndex;

		ar<<(short)m_imageAlignHorz;
		ar<<(short)m_imageAlignVert;
		ar<<m_imageDeltaX;
		ar<<m_imageDeltaY;

		ar<<m_iconDeltaX;
		ar<<m_iconDeltaY;
		ar<<m_textDeltaX;
		ar<<m_textDeltaY;

		
	}
	else//Loading
	{
		ar>>nVersion;
		ar>>m_nVKey;
		SetVirtKey( m_nVKey );

		if( nVersion >= 2 )
		{
			ar>>m_strBmpFileName;
			m_ImageList.DeleteImageList();
			m_ImageList.Read( &ar );			
			ar>>m_nImageWidth;
			ar>>m_nImageHeight;
			ar>>m_clrImageMask;
			ar>>m_nImageIndex;
		}

		if( nVersion >= 3 )
		{
			short n = 0;
			ar>>n;
			m_imageAlignHorz = (ImageAlignHorzEnum)n;

			n = 0;
			ar>>n;
			m_imageAlignVert = (ImageAlignVertEnum)n;

			ar>>m_imageDeltaX;
			ar>>m_imageDeltaY;
		}

		if( nVersion >= 4 )
		{
			ar>>m_iconDeltaX;
			ar>>m_iconDeltaY;
			ar>>m_textDeltaX;
			ar>>m_textDeltaY;
		}



	}

	SerializeStockProps( ar	);
	
}

/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton::OnResetState - Reset control to default state
void CVTKeyButton::OnResetState()
{
	COleControl::OnResetState();
	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton::AboutBox - Display an "About" box to the user

void CVTKeyButton::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTKEYBUTTON);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton properties
/////////////////////////////////////////////////////////////////////////////
long CVTKeyButton::GetVirtKey()
{
	return m_nVKey;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetVirtKey(long nNewValue)
{
	m_nVKey = nNewValue;
	m_szText[0] = 0;
	if( m_hIcon )::DestroyIcon( m_hIcon );m_hIcon = 0;
	

	if( m_nVKey == VK_CONTROL )
		strcpy( m_szText, "Ctrl" );
	else if( m_nVKey == VK_MENU )
	{
		strcpy( m_szText, "Alt" );
	}
	else if( m_nVKey == VK_SHIFT )
	{
		strcpy( m_szText, "Shift" );
		m_hIcon = ::LoadIcon( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDI_SHIFT) );
	}
	else if( m_nVKey ==  VK_END )
	{
		strcpy( m_szText, "End" );
	}
	else if( m_nVKey == VK_HOME )
	{
		strcpy( m_szText, "Home" );
	}
	else if( m_nVKey ==  VK_INSERT )
	{
		strcpy( m_szText, "Insert" );
	}
	else if( m_nVKey == VK_DELETE )
	{
		strcpy( m_szText, "Del" );
	}
	else if( m_nVKey == VK_ESCAPE )
	{
		strcpy( m_szText, "Esc" );
	}
	else if( m_nVKey == VK_BACK )
	{
		strcpy( m_szText, "Back" );
	}
	else if( m_nVKey == VK_TAB )
	{
		strcpy( m_szText, "Tab" );
	}
	else if( m_nVKey == 33 )
	{
		strcpy( m_szText, "" );
	}
	else if( m_nVKey == 34 )
	{
		strcpy( m_szText, "" );
	}
	else if( m_nVKey == VK_UP )
		m_hIcon = ::LoadIcon( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDI_UP) );
	else if( m_nVKey == VK_DOWN )
		m_hIcon = ::LoadIcon( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDI_DOWN) );
	else if( m_nVKey == VK_LEFT )
		m_hIcon = ::LoadIcon( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDI_LEFT) );
	else if( m_nVKey == VK_RIGHT )
		m_hIcon = ::LoadIcon( AfxGetInstanceHandle( ), MAKEINTRESOURCE(IDI_RIGHT) );
	else if( ::isalnum_ru( m_nVKey ) )
	{
		m_szText[0] = (char)m_nVKey;
		m_szText[1] = 0;
	}
				
	SetModifiedFlag( );
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetImageWidth()
{
	return m_nImageWidth;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageWidth(short nNewValue)
{
	m_nImageWidth = nNewValue;
	SetModifiedFlag();
	SetImageList( (LPCTSTR)m_strBmpFileName, m_nImageWidth, m_nImageHeight, m_clrImageMask );
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetImageHeight()
{
	return m_nImageHeight;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageHeight(short nNewValue)
{
	m_nImageHeight = nNewValue;
	SetModifiedFlag();
	SetImageList( (LPCTSTR)m_strBmpFileName, m_nImageWidth, m_nImageHeight, m_clrImageMask );
}

/////////////////////////////////////////////////////////////////////////////
BSTR CVTKeyButton::GetBmpFileName()
{
	CString strResult = m_strBmpFileName;
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetBmpFileName(LPCTSTR strNewValue)
{
	m_strBmpFileName = strNewValue;
	SetModifiedFlag();
	SetImageList( (LPCTSTR)m_strBmpFileName, m_nImageWidth, m_nImageHeight, m_clrImageMask );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTKeyButton::GetImageMask()
{
	return m_clrImageMask;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageMask(OLE_COLOR nNewValue)
{
	m_clrImageMask = nNewValue;
	SetModifiedFlag();
	SetImageList( (LPCTSTR)m_strBmpFileName, m_nImageWidth, m_nImageHeight, m_clrImageMask );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTKeyButton::SetImageList(LPCTSTR lpstrFileName, short cx, short cy, OLE_COLOR clrMask)
{	
	if( cx < 1 || cy < 1 )
		return FALSE;

	CString strFileName = lpstrFileName;

	CFileFind ff;
	if( !ff.FindFile( strFileName  ) )
		return FALSE;

	//Destroy prev created
	CreateDefaultImageList();

	HBITMAP hBitmap = NULL;
	hBitmap = (HBITMAP)LoadImage( NULL, strFileName, IMAGE_BITMAP, 0, 0,
					 LR_DEFAULTSIZE | LR_LOADFROMFILE );
	if( !hBitmap )
		return FALSE;

	CBitmap bmp;
	bmp.Attach (hBitmap);
	BITMAP bitmap;
	bmp.GetBitmap (&bitmap);

	int nImageWidth = bitmap.bmWidth;
	int nImageHeight = bitmap.bmHeight;

	m_ImageList.DeleteImageList();

	
	
	if( !m_ImageList.Create( cx, cy, ILC_COLOR16|ILC_MASK, 2, 1 ) )
	{
		if( hBitmap ) DeleteObject( hBitmap );		
		CreateDefaultImageList();
		return FALSE;
	}			

	//m_ImageList.SetBkColor( COLOR_FRONT  );	
	int nIndex = m_ImageList.Add( &bmp, TranslateColor(clrMask) );
	//m_ImageList.SetBkColor( COLOR_FRONT );	
	
	DeleteObject( hBitmap );


	m_strBmpFileName	= lpstrFileName;
	m_nImageWidth		= cx;
	m_nImageHeight		= cy;
	m_clrImageMask		= clrMask;

	Refresh();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetImageAlignHorz()
{
	return (short)m_imageAlignHorz;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageAlignHorz(short nNewValue)
{
	m_imageAlignHorz = (ImageAlignHorzEnum)nNewValue;
	SetModifiedFlag();
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetImageAlignVert()
{
	return (short)m_imageAlignVert;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageAlignVert(short nNewValue)
{
	m_imageAlignVert = (ImageAlignVertEnum)nNewValue;
	SetModifiedFlag();
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetImageDeltaX()
{
	return m_imageDeltaX;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageDeltaX(short nNewValue)
{
	m_imageDeltaX = nNewValue;
	SetModifiedFlag();
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetImageDeltaY()
{
	return m_imageDeltaY;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageDeltaY(short nNewValue)
{
	m_imageDeltaY = nNewValue;
	SetModifiedFlag();
	Refresh();
}


/////////////////////////////////////////////////////////////////////////////
BOOL CVTKeyButton::ReloadImageList()
{
	return SetImageList( (LPCTSTR)m_strBmpFileName, m_nImageWidth, m_nImageHeight, m_clrImageMask );
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetImageIndex(short nNewValue)
{
	m_nImageIndex = nNewValue;
	SetModifiedFlag();
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetImageIndex()
{
	return m_nImageIndex;
}


/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetIconDeltaX(short nNewValue)
{
	m_iconDeltaX = nNewValue;
	SetModifiedFlag();
	Refresh();
}


/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetIconDeltaX()
{
	return m_iconDeltaX;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetIconDeltaY(short nNewValue)
{
	m_iconDeltaY = nNewValue;
	SetModifiedFlag();
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetIconDeltaY()
{
	return m_iconDeltaY;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetTextDeltaX(short nNewValue)
{
	m_textDeltaX = nNewValue;
	SetModifiedFlag();
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetTextDeltaX()
{
	return m_textDeltaX;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetTextDeltaY(short nNewValue)
{
	m_textDeltaY = nNewValue;
	SetModifiedFlag();
	Refresh();
}

/////////////////////////////////////////////////////////////////////////////
short CVTKeyButton::GetTextDeltaY()
{
	return m_textDeltaY;
}


/////////////////////////////////////////////////////////////////////////////
int CVTKeyButton::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;		
	
	SetVirtKey( m_nVKey );	

	m_nTimerID = 787;
	m_nTimerID = SetTimer( m_nTimerID, 100, NULL );	

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::OnTimer(UINT_PTR nIDEvent) 
{
	bool	bPressed = (::GetAsyncKeyState( m_nVKey ) & 0x8000) == 0x8000;
	if( bPressed != m_bPressed )
	{
		m_bPressed = bPressed;
		Invalidate();
	}

	COleControl::OnTimer(nIDEvent);
}


/////////////////////////////////////////////////////////////////////////////
BOOL CVTKeyButton::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::OnDestroy() 
{
	if( m_nTimerID != -1 )
		KillTimer( m_nTimerID );

	m_nTimerID = -1;

	COleControl::OnDestroy();		
}


const int	cyFrontHeight = 10;
const int	cyBackHeight = 2;
const int	cxShiftFront = 7;
const int	cxShiftBack = 7;
const int	cxPersp = 1;
const int	cxSmooth = 2;


/////////////////////////////////////////////////////////////////////////////
// CVTKeyButton::OnDraw - Drawing function
void CVTKeyButton::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{

	CRect rc = rcBounds;
	HDC	hdcPaint = pdc->m_hDC;

	HDC	hdc = ::CreateCompatibleDC( hdcPaint );
	HBITMAP	hbmpOld, hbmp = ::CreateCompatibleBitmap( hdcPaint, rc.right-rc.left, rc.bottom-rc.top );
	hbmpOld = (HBITMAP)::SelectObject( hdc, hbmp );

	::SetWindowOrgEx( hdc, -rc.left, -rc.top, 0 );
	
	POINT	pointLeftSide[4];
	POINT	pointRightSide[4];
	POINT	pointTopSide[8];
	POINT	pointFrontSide[4];
	POINT	pointBackLeft[4];
	POINT	pointFrontLeft[4];
	POINT	pointBackRight[4];
	POINT	pointFrontRight[4];

	POINT	pointBack[16];

	RECT	rcBtn;
	memcpy( &rcBtn, &rc, sizeof( rc ) );
	rcBtn.top++;
	rcBtn.right-=2;
	rcBtn.bottom-=2;


	int	cxPressed = 1, cyPressed = 0;
	if( m_bPressed )
	{
		cxPressed = 4;
		cyPressed  = 3;
	}
	int	cxPressedTop = 0;
	if( m_bPressed )cxPressedTop = 0;

	pointFrontSide[0].x = rcBtn.left+cxSmooth;
	pointFrontSide[0].y = rcBtn.bottom+cyPressed;
	pointFrontSide[1].x = rcBtn.left+cxShiftFront+cxSmooth;
	pointFrontSide[1].y = rcBtn.bottom-cyFrontHeight+cyPressed;
	pointFrontSide[2].x = rcBtn.right-cxShiftFront-cxSmooth;
	pointFrontSide[2].y = rcBtn.bottom-cyFrontHeight+cyPressed;
	pointFrontSide[3].x = rcBtn.right-cxSmooth;
	pointFrontSide[3].y = rcBtn.bottom+cyPressed;

	pointLeftSide[0].x = rcBtn.left;
	pointLeftSide[0].y = rcBtn.bottom-cxSmooth+cyPressed;
	pointLeftSide[1].x = rcBtn.left+cxPersp;
	pointLeftSide[1].y = rcBtn.top+cyBackHeight+cxSmooth+cyPressed;
	pointLeftSide[2].x = rcBtn.left+cxPersp+cxShiftBack;
	pointLeftSide[2].y = rcBtn.top+cxSmooth+cyPressed;
	pointLeftSide[3].x = rcBtn.left+cxShiftFront;
	pointLeftSide[3].y = rcBtn.bottom-cyFrontHeight-cxSmooth+cyPressed;

	pointRightSide[0].x = rcBtn.right;
	pointRightSide[0].y = rcBtn.bottom-cxSmooth+cyPressed;
	pointRightSide[1].x = rcBtn.right-cxPersp;
	pointRightSide[1].y = rcBtn.top+cyBackHeight+cxSmooth+cyPressed;
	pointRightSide[2].x = rcBtn.right-cxPersp-cxShiftBack;
	pointRightSide[2].y = rcBtn.top+cxSmooth+cyPressed;
	pointRightSide[3].x = rcBtn.right-cxShiftFront;
	pointRightSide[3].y = rcBtn.bottom-cyFrontHeight-cxSmooth+cyPressed;

	pointTopSide[0].x = rcBtn.left+cxShiftFront;
	pointTopSide[0].y = rcBtn.bottom-cyFrontHeight-cxSmooth+cyPressed;
	pointTopSide[1].x = rcBtn.left+cxShiftBack+cxPersp;
	pointTopSide[1].y = rcBtn.top+cxSmooth+cyPressed;
	pointTopSide[2].x = rcBtn.left+cxShiftBack+cxPersp+cxSmooth;
	pointTopSide[2].y = rcBtn.top+cyPressed;
	pointTopSide[3].x = rcBtn.right-cxShiftBack-cxPersp-cxSmooth;
	pointTopSide[3].y = rcBtn.top+cyPressed;
	pointTopSide[4].x = rcBtn.right-cxShiftBack-cxPersp;
	pointTopSide[4].y = rcBtn.top+cxSmooth+cyPressed;
	pointTopSide[5].x = rcBtn.right-cxShiftFront;
	pointTopSide[5].y = rcBtn.bottom-cyFrontHeight-cxSmooth+cyPressed;
	pointTopSide[6].x = rcBtn.right-cxShiftFront-cxSmooth;
	pointTopSide[6].y = rcBtn.bottom-cyFrontHeight+cyPressed;
	pointTopSide[7].x = rcBtn.left+cxShiftFront+cxSmooth;
	pointTopSide[7].y = rcBtn.bottom-cyFrontHeight+cyPressed;

	pointBackLeft[0].x = rcBtn.left+cxPersp;
	pointBackLeft[0].y = rcBtn.top+cyBackHeight+cxSmooth+cyPressed;
	pointBackLeft[1].x = rcBtn.left+cxPersp+cxShiftBack;
	pointBackLeft[1].y = rcBtn.top+cxSmooth+cyPressed;
	pointBackLeft[2].x = rcBtn.left+cxPersp+cxShiftBack+cxSmooth;
	pointBackLeft[2].y = rcBtn.top+cyPressed;
	pointBackLeft[3].x = rcBtn.left+cxPersp+cxSmooth;
	pointBackLeft[3].y = rcBtn.top+cyPressed;

	pointBackRight[0].x = rcBtn.right-cxPersp;
	pointBackRight[0].y = rcBtn.top+cyBackHeight+cxSmooth+cyPressed;
	pointBackRight[1].x = rcBtn.right-cxPersp-cxShiftBack;
	pointBackRight[1].y = rcBtn.top+cxSmooth+cyPressed;
	pointBackRight[2].x = rcBtn.right-cxPersp-cxShiftBack-cxSmooth;
	pointBackRight[2].y = rcBtn.top+cyPressed;
	pointBackRight[3].x = rcBtn.right-cxPersp-cxSmooth;
	pointBackRight[3].y = rcBtn.top+cyPressed;

	pointFrontLeft[0].x = rcBtn.left+cxSmooth;
	pointFrontLeft[0].y = rcBtn.bottom+cyPressed;
	pointFrontLeft[1].x = rcBtn.left+cxShiftFront+cxSmooth;
	pointFrontLeft[1].y = rcBtn.bottom-cyFrontHeight+cyPressed;
	pointFrontLeft[2].x = rcBtn.left+cxShiftFront;
	pointFrontLeft[2].y = rcBtn.bottom-cyFrontHeight-cxSmooth+cyPressed;
	pointFrontLeft[3].x = rcBtn.left;
	pointFrontLeft[3].y = rcBtn.bottom-cxSmooth+cyPressed;

	pointFrontRight[0].x = rcBtn.right-cxSmooth;
	pointFrontRight[0].y = rcBtn.bottom+cyPressed;
	pointFrontRight[1].x = rcBtn.right-cxShiftFront-cxSmooth;
	pointFrontRight[1].y = rcBtn.bottom-cyFrontHeight+cyPressed;
	pointFrontRight[2].x = rcBtn.right-cxShiftFront;
	pointFrontRight[2].y = rcBtn.bottom-cyFrontHeight-cxSmooth+cyPressed;
	pointFrontRight[3].x = rcBtn.right;
	pointFrontRight[3].y = rcBtn.bottom-cxSmooth+cyPressed;

	pointBack[0].x = rc.left;
	pointBack[0].y = rc.top;
	pointBack[1].x = rc.right;
	pointBack[1].y = rc.top;
	pointBack[2].x = rc.right;
	pointBack[2].y = rc.bottom;
	pointBack[3].x = rc.left;
	pointBack[3].y = rc.bottom;
	pointBack[4].x = rc.left+cxPressed;
	pointBack[4].y = rc.top-cxPressedTop;
	pointBack[5].x = rc.left+cxSmooth+cxPressed;
	pointBack[5].y = rc.top-cxPressedTop;
	pointBack[6].x = rc.right-cxSmooth-cxPressed;
	pointBack[6].y = rc.top-cxPressedTop;
	pointBack[7].x = rc.right-cxPressed;
	pointBack[7].y = rc.top-cxSmooth-cxPressedTop;
	pointBack[8].x = rc.right-cxPressed;
	pointBack[8].y = rc.bottom-cxSmooth-cxPressed;
	pointBack[9].x = rc.right-cxSmooth-cxPressed;
	pointBack[9].y = rc.bottom-cxPressed;
	pointBack[10].x = rc.left+cxSmooth+cxPressed;
	pointBack[10].y = rc.bottom-cxPressed;
	pointBack[11].x = rc.left+cxPressed;
	pointBack[11].y = rc.bottom-cxSmooth-cxPressed;
	pointBack[12].x = rc.left+cxPressed;
	pointBack[12].y = rc.top+cxSmooth-cxPressedTop;


	

	COLORREF	crLeft = RGB(200, 200, 200), crRight = RGB( 160, 160, 160 ), 
				crTop = RGB(230, 230, 230), crFront = RGB(128, 128, 128),
				crBackLeft = RGB(200, 200, 200), crBackRight = RGB(170, 170, 170), 
				crFrontLeft = RGB(220, 220, 220), crFrontRight = RGB(140, 140, 140);

	HBRUSH	hOldBrush, 
		hTopBrush = ::CreateSolidBrush( crTop ), 
		hLeftBrush = ::CreateSolidBrush( crLeft ), 
		hFrontBrush = ::CreateSolidBrush( crFront ), 
		hRightBrush = ::CreateSolidBrush( crRight ),
		hBackLeft =  ::CreateSolidBrush( crBackLeft ),
		hBackRight = ::CreateSolidBrush( crBackRight ),
		hFrontLeft = ::CreateSolidBrush( crFrontLeft ),
		hFrontRight = ::CreateSolidBrush( crFrontRight ),
		hBack = ::CreateSolidBrush( RGB( 0, 0, 0 ) );

	


	::SelectObject( hdc, ::GetStockObject( NULL_PEN ) );
	hOldBrush = (HBRUSH)
	::SelectObject( hdc, hBack );

	::Rectangle( hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );
	::SelectObject( hdc, hTopBrush );
	::Polygon( hdc, pointTopSide, 8 );
	::SelectObject( hdc, hLeftBrush );
	::Polygon( hdc, pointLeftSide, 4 );
	::SelectObject( hdc, hRightBrush );
	::Polygon( hdc, pointRightSide, 4 );
	::SelectObject( hdc, hFrontBrush );
	::Polygon( hdc, pointFrontSide, 4 );

	::SelectObject( hdc, hBackLeft );
	::Polygon( hdc, pointBackLeft, 4 );
	::SelectObject( hdc, hBackRight );
	::Polygon( hdc, pointBackRight, 4 );
	::SelectObject( hdc, hFrontLeft );
	::Polygon( hdc, pointFrontLeft, 4 );
	::SelectObject( hdc, hFrontRight );
	::Polygon( hdc, pointFrontRight, 4 );

	::SelectObject( hdc, GetStockObject( BLACK_PEN ) );
	::SelectObject( hdc, GetStockObject( NULL_BRUSH ) );
	::Rectangle( hdc, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );

	::SetBkMode( hdc, TRANSPARENT );

	RECT	rectText;
	ZeroMemory( &rectText, sizeof( rectText ) );
	::DrawText( hdc, m_szText, strlen( m_szText ), &rectText, DT_CALCRECT );
	int	nTextWidth = rectText.right-rectText.left;

	int	cx = 0, cy = 0;

	for( int i = 0; i < 8; i++ )
	{
		cx+=pointTopSide[i].x;
		cy+=pointTopSide[i].y;
	}
	cx/=8;
	cy/=8;

	LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );

	lf.lfHeight = 12;
	lf.lfWeight = FW_BOLD;
	strcpy( lf.lfFaceName, "Arial" );

	HFONT	hFont = ::CreateFontIndirect( &lf ), hOldFont;

	hOldFont = (HFONT)
	::SelectObject( hdc, hFont );



	int	nDrawPos = cx-nTextWidth/2;

	if( m_hIcon )
	{
		::DrawIcon( hdc, nDrawPos-16+m_iconDeltaX, cy-16+m_iconDeltaY, m_hIcon );
		nDrawPos+=8;
	}
	if( strlen( m_szText ) )
	{
		RECT	rect;
		rect.left = nDrawPos + m_textDeltaX;
		rect.right = nDrawPos+nTextWidth + m_textDeltaX;
		rect.top = cy-(rectText.bottom-rectText.top)/2 + m_textDeltaY;
		rect.bottom = rect.top+(rectText.bottom-rectText.top) + m_textDeltaY;		

		::DrawText( hdc, m_szText, strlen( m_szText ), &rect, DT_CENTER|DT_VCENTER );
	}

	m_rcAvailable = CRect( pointTopSide[2].x, pointTopSide[1].y,
									pointTopSide[3].x, pointTopSide[5].y );
	//Drawing image list
	if( m_ImageList.m_hImageList && //Content 
			m_nImageIndex >= 0 && m_nImageIndex < m_ImageList.GetImageCount() //Size
			)
	{
		
		CRect rcAvailable = m_rcAvailable;
		
		CPoint pt = rcAvailable.TopLeft();

		//Horz
		if( m_imageAlignHorz == iahLeft )
		{
			pt.x = rcAvailable.left + m_imageDeltaX;
		}
		else
		if( m_imageAlignHorz == iahCenter )
		{
			pt.x = rcAvailable.left + rcAvailable.Width() / 2 - m_nImageWidth / 2 + m_imageDeltaX;
		}
		else
		if( m_imageAlignHorz == iahRight )
		{
			pt.x = rcAvailable.right - m_imageDeltaX - m_nImageWidth;
		}


		//Vert
		if( m_imageAlignVert == iavTop )
		{
			pt.y = rcAvailable.top + m_imageDeltaY;
		}
		else
		if( m_imageAlignVert == iavCenter )
		{
			pt.y = rcAvailable.top + rcAvailable.Height() / 2 - m_nImageHeight / 2 + m_imageDeltaY;
		}
		else
		if( m_imageAlignVert == iavBottom )
		{
			pt.y = rcAvailable.bottom - m_imageDeltaY - m_nImageHeight;
		}

		//Draw trnsparency
		{

			CDC* pdcTemp = CDC::FromHandle( hdc );
			//m_ImageList.SetBkColor( COLOR_FRONT  );	
			m_ImageList.Draw( pdcTemp, m_nImageIndex, pt, ILD_NORMAL/*|ILD_MASK*/ );
			//m_ImageList.DrawIndirect( pdcTemp, m_nImageIndex, 
			//	pt, CSize(m_nImageWidth, m_nImageHeight), CPoint(0,0),
			//	ILD_NORMAL, NOTSRCCOPY, RGB(125,0,255), RGB(125,1,255)  );
				
		}

	}	


	/*
	{
		CRect rcContainer;
		int cx, cy;
		GetControlSize( &cx, &cy );
		rcContainer = CRect( 0, 0, cx, cy );

		double fOuterWidth  = (double)rcContainer.Width()  + 1;
		double fOuterHeigth = (double)rcContainer.Height() + 1;


		double fInnerWidth  = (double)rcBounds.Width()  + 1;
		double fInnerHeight = (double)rcBounds.Height()  + 1;
		
		double fZoom = fOuterWidth / fInnerWidth;
		
		m_rcAvailable.left = 0;
		m_rcAvailable.top  = 0;

		m_rcAvailable.right  = m_rcAvailable.left + (int)(fInnerWidth / fZoom);
		m_rcAvailable.bottom = m_rcAvailable.top  + (int)(fInnerHeight / fZoom);


		m_rcAvailable = rcContainer;

		
		//m_rcAvailable		
		
	}
	*/

	

	//::SelectObject( hdc, hBack );
	//::Polygon( hdc, pointBack, 13 );

	::BitBlt( hdcPaint, rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top, hdc, rc.left, rc.top, SRCCOPY );

	::SelectObject( hdc, hOldBrush );
	::SelectObject( hdc, hbmpOld );
	::SelectObject( hdc, hOldFont );

	::DeleteObject( hbmp );

	::DeleteObject( hTopBrush );
	::DeleteObject( hFrontBrush );
	::DeleteObject( hLeftBrush );
	::DeleteObject( hRightBrush );
	::DeleteObject( hBackLeft );
	::DeleteObject( hBackRight );
	::DeleteObject( hFrontLeft );
	::DeleteObject( hFrontRight );
	::DeleteObject( hBack );
	::DeleteObject( hFont );	
	
	

	::DeleteDC( hdc );

}


/////////////////////////////////////////////////////////////////////////////
BSTR CVTKeyButton::GetControlInfo()
{
	CString strResult;

	if( m_rcAvailable != NORECT )
	{
		strResult.Format( "Available Rect:left=%d, top=%d, right=%d, bottom=%d, Width=%d, Height=%d",
			m_rcAvailable.left, m_rcAvailable.top, m_rcAvailable.right, m_rcAvailable.bottom,
			m_rcAvailable.Width(), m_rcAvailable.Height()
				);
	}
	else
	{
		strResult = "No info";
	}

	return strResult.AllocSysString();

}

/////////////////////////////////////////////////////////////////////////////
void CVTKeyButton::SetControlInfo(LPCTSTR strNewValue)
{

}


void CVTKeyButton::OnSetFocus(CWnd* pOldWnd) 
{
	//COleControl::OnSetFocus(pOldWnd);

	CWnd	*pwnd = GetParent();
	if( pwnd  )pwnd->SetFocus();
}
