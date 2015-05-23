// System.cpp : Implementation of the CVTSystem ActiveX Control class.

#include "stdafx.h"
#include "VTDesign.h"
#include "System.h"
#include "SystemPpg.h"
#include <msstkppg.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CVTSystem, COleControl)

BEGIN_INTERFACE_MAP(CVTSystem, COleControl)
	INTERFACE_PART(CVTSystem, IID_IVTPrintCtrl, PrintCtrl)
	INTERFACE_PART(CVTSystem, IID_IAXCtrlDataSite, AXCtrlDataSite)	
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CVTSystem, PrintCtrl)
IMPLEMENT_AX_DATA_SITE(CVTSystem)

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTSystem, COleControl)
	//{{AFX_MSG_MAP(CVTSystem)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTSystem, COleControl)
	//{{AFX_DISPATCH_MAP(CVTSystem)
	DISP_PROPERTY_EX(CVTSystem, "BackColor", GetBackColor, SetBackColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTSystem, "TransparentBack", GetTransparentBack, SetTransparentBack, VT_BOOL)
	DISP_PROPERTY_EX(CVTSystem, "FontColor", GetFontColor, SetFontColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTSystem, "DataType", GetDataType, SetDataType, VT_I2)		
	DISP_PROPERTY_EX(CVTSystem, "TextFont", GetTextFont, SetTextFont, VT_FONT)	
	DISP_PROPERTY_EX(CVTSystem, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CVTSystem, "HorzAlign", GetHorzAlign, SetHorzAlign, VT_I2)	
	DISP_PROPERTY_EX(CVTSystem, "VertAlign", GetVertAlign, SetVertAlign, VT_I2)	
	DISP_FUNCTION(CVTSystem, "GetPropertyAXStatus", GetPropertyAXStatus, VT_BOOL, VTS_NONE)
	DISP_FUNCTION(CVTSystem, "SetPropertyAXStatus", SetPropertyAXStatus, VT_EMPTY, VTS_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTSystem, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTSystem, COleControl)
	//{{AFX_EVENT_MAP(CVTSystem)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
/*BEGIN_PROPPAGEIDS(CVTSystem, 2)
	PROPPAGEID(CVTSystemPropPage::guid)
	//PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTSystem)
*/
static CLSID pages[2];

LPCLSID CVTSystem::GetPropPageIDs( ULONG& cPropPages )
{
	cPropPages = 0;
	pages[cPropPages++]=CVTSystemPropPage::guid;
	pages[cPropPages++]=CLSID_StockFontPage;
	m_PropertyAXStatus=true;

	return pages;
}
/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTSystem, "VTDESIGN.VTDSystemCtrl.1",
	0xbc127b23, 0x26c, 0x11d4, 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTSystem, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTDSystem =
		{ 0xbc127b21, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };
const IID BASED_CODE IID_DVTDSystemEvents =
		{ 0xbc127b22, 0x26c, 0x11d4, { 0x81, 0x24, 0, 0, 0xe8, 0xdf, 0x68, 0xc3 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTDSystemOleMisc =
//	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTSystem, IDS_VTDSYSTEM, _dwVTDSystemOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTSystem::CVTSystemFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTSystem

BOOL CVTSystem::CVTSystemFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTDSYSTEM,
			IDB_VTDSYSTEM,
			afxRegApartmentThreading,
			_dwVTDSystemOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystem::CVTSystem - Constructor

CVTSystem::CVTSystem() : m_fontHolder( &m_xFontNotification )
{
	InitializeIIDs(&IID_DVTDSystem, &IID_DVTDSystemEvents);

	InitDefaults();
	//added by akm 22_11_k5
	m_PropertyAXStatus=false;
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystem::~CVTSystem - Destructor

CVTSystem::~CVTSystem()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystem::OnDraw - Drawing function



/////////////////////////////////////////////////////////////////////////////
// CVTSystem::DoPropExchange - Persistence support

void CVTSystem::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	//COleControl::DoPropExchange(pPX);

	long nVersion = 1;
	PX_Long(pPX, "Version", nVersion );

	if( pPX->IsLoading() ){}

	PX_Font(pPX, _T("TextFont"), m_fontHolder, &m_fontDesc);

}


/////////////////////////////////////////////////////////////////////////////
// CVTSystem::OnResetState - Reset control to default state

void CVTSystem::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystem::AboutBox - Display an "About" box to the user

void CVTSystem::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTDSYSTEM);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystem message handlers
DWORD CVTSystem::GetControlFlags() 
{
	return COleControl::GetControlFlags();// | windowlessActivate;
}

BOOL CVTSystem::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.dwExStyle |= WS_EX_TRANSPARENT;	
	return COleControl::PreCreateWindow(cs);
}

BOOL CVTSystem::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}

void CVTSystem::InitDefaults()
{
	m_BackColor			= 0x80000000 + COLOR_WINDOW;
	m_bTransparentBack	= TRUE;	
	m_FontColor			= 0x80000000 + COLOR_WINDOWTEXT;
	
	m_DataType	= stDate;	


	m_nCurPage = 0;
	m_nPageCount = 0;
	

	m_fontDesc.cbSizeofstruct = sizeof( FONTDESC );    
    m_fontDesc.lpstrName = OLESTR("Arial");

    m_fontDesc.cySize.Lo = 80000;
	m_fontDesc.cySize.Hi = 0;

    m_fontDesc.sWeight = FW_THIN;
    m_fontDesc.sCharset = NULL;
    m_fontDesc.fItalic	= FALSE;
    m_fontDesc.fUnderline = FALSE;
    m_fontDesc.fStrikethrough = FALSE;

	m_fontHolder.ReleaseFont();
	m_fontHolder.InitializeFont( &m_fontDesc );	

	m_horzAlign = ahLeft;
	m_vertAlign = avTop;

	m_fFontRatio = 1.0;

}

void CVTSystem::Serialize(CArchive& ar)
{
	long nVersion = 2;


	SerializeStockProps(ar);

	if( ar.IsStoring( ) )
	{				
		ar<<nVersion;		
		ar<<(SHORT)m_DataType;		
		ar<<m_BackColor;
		ar<<m_bTransparentBack;
		ar<<m_FontColor;
		//ar.Write( &m_fontDesc, sizeof(FONTDESC) );
		ar<<(SHORT)m_horzAlign;
		ar<<(SHORT)m_vertAlign;

		
	}
	else
	{			
		
		ar>>nVersion;
		SHORT nVal;		
		ar>>nVal; m_DataType	= (SystemType)nVal;		
		ar>>m_BackColor;
		ar>>m_bTransparentBack;
		ar>>m_FontColor;		

		if( nVersion >= 2 )
		{			
			ar>>nVal; m_horzAlign = (AlignHorizontal)nVal;		
			ar>>nVal; m_vertAlign = (AlignVertical)nVal;		
		}

		InvalidateCtrl( this, m_pInPlaceSiteWndless );
	}

	COleControl::Serialize( ar );
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTSystem::XPrintCtrl::FlipHorizontal( )
{
	METHOD_PROLOGUE_EX(CVTSystem, PrintCtrl)			
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTSystem::XPrintCtrl::FlipVertical( )
{
	METHOD_PROLOGUE_EX(CVTSystem, PrintCtrl)		
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTSystem::XPrintCtrl::Draw( HDC hDC, SIZE sizeVTPixel )
{
	METHOD_PROLOGUE_EX(CVTSystem, PrintCtrl)
	pThis->m_fFontRatio = 1.0;
	pThis->DrawContext( hDC, sizeVTPixel );
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTSystem::XPrintCtrl::SetPageInfo( int nCurPage, int nPageCount )
{
	METHOD_PROLOGUE_EX(CVTSystem, PrintCtrl)	

	pThis->m_nCurPage = nCurPage;
	pThis->m_nPageCount = nPageCount;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
HRESULT CVTSystem::XPrintCtrl::NeedUpdate( BOOL* pbUpdate )
{
	METHOD_PROLOGUE_EX(CVTSystem, PrintCtrl)	
	*pbUpdate = TRUE;
	return S_OK;

}


/////////////////////////////////////////////////////////////////////////////
void CVTSystem::DrawContext( HDC hDC, SIZE size/* in himetrics*/ )	
{
	CRect rcBounds(0, 0, size.cx, size.cy );
	
	CDC* pdc = CDC::FromHandle( hDC );
	VERIFY(pdc);

	int nOldBkMode = pdc->SetBkMode( TRANSPARENT );

	//BackGround
	if( !m_bTransparentBack )
	{
		CBrush brush( TranslateColor(m_BackColor) );
		CBrush* pOldBrush = (CBrush*)pdc->SelectObject( &brush );
		pdc->FillRect( rcBounds, &brush );
		pdc->SelectObject( pOldBrush );
	}

	//Draw text

	int nTextWidth, nTextHeight;
	nTextWidth = nTextHeight = 0;

	if( m_DataType == stDate )
	{
		COleDateTime time = COleDateTime::GetCurrentTime();
		m_strText = time.Format( VAR_DATEVALUEONLY );
	}

	if( m_DataType == stDateTime )
	{
		COleDateTime time = COleDateTime::GetCurrentTime();
		m_strText = time.Format( );
	}
	
	if( m_DataType == stPageNum )	
		m_strText.Format( "Page %d of %d", m_nCurPage+1, m_nPageCount );	


	if( !m_strText.IsEmpty() )
	{
		COLORREF clrOldText = pdc->SetTextColor( TranslateColor(m_FontColor) );
		
		IFontPtr fontPtr( m_fontHolder.m_pFont );

		if( fontPtr )
		{
			BSTR	bstrName;
			CY		sizeCY;
			BOOL	bBold;
			BOOL	bItalic;
			BOOL	bUnderline;
			BOOL	bStrikethrough;
			SHORT	nWeight;
			SHORT	nCharset;

			fontPtr->get_Name( &bstrName );
			fontPtr->get_Size( &sizeCY );
			fontPtr->get_Bold( &bBold );
			fontPtr->get_Italic( &bItalic );
			fontPtr->get_Underline( &bUnderline );
			fontPtr->get_Strikethrough( &bStrikethrough );
			fontPtr->get_Weight( &nWeight );
			fontPtr->get_Charset( &nCharset );                

			CString strFontName = bstrName;
			::SysFreeString( bstrName );

			int nAngle;
			
			nAngle = 0;
			
			double fAspect = 1.0 / 72.0 * 25.2 * 10.0 * m_fFontRatio;
			CFont font;

			font.CreateFont( 
				(int)(((double)sizeCY.Lo)/(1000.0) * fAspect), 0, 
				nAngle, 0, 
				nWeight, (BYTE)bItalic, 
				(BYTE)bUnderline, (BYTE)bStrikethrough, 
				(BYTE)DEFAULT_CHARSET,
				OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
				DEFAULT_QUALITY,DEFAULT_PITCH,						
				strFontName 
				);


			CFont* pOldFont = (CFont*)pdc->SelectObject( &font );

			CRect rcCalc(0,0,0,0);
			VERIFY( pdc->DrawText( m_strText, &rcCalc , DT_CALCRECT ) );			
			rcCalc.NormalizeRect();

			nTextWidth  = rcCalc.Width();
			nTextHeight = rcCalc.Height();

			
			int nBoundsWidth  = rcBounds.Width();
			int nBoundsHeight = rcBounds.Height();

			CRect rcOut;

			CPoint ptAdd( 0, 0 );
			if( m_horzAlign == ahLeft )
			{
				ptAdd.x = rcBounds.left;
			}
			else
			if( m_horzAlign == ahCenter )
			{
				ptAdd.x = rcBounds.left + (nBoundsWidth - nTextWidth) / 2;
			}
			else
			if( m_horzAlign == ahRight )
			{
				ptAdd.x = rcBounds.right - nTextWidth;
			}

			if( m_vertAlign == avBottom )
			{
				ptAdd.y = rcBounds.top;
			}
			else
			if( m_vertAlign == avCenter )
			{
				ptAdd.y = rcBounds.top + (nBoundsHeight - nTextHeight) / 2;
			}
			else
			if( m_vertAlign == avTop )
			{
				ptAdd.y = rcBounds.bottom - nTextHeight;
			}


			rcOut.left = ptAdd.x;
			rcOut.right = rcOut.left + nTextWidth;

			rcOut.bottom = ptAdd.y;
			rcOut.top = rcOut.bottom + nTextHeight;

			CRgn rgnClip;
			pdc->BeginPath();
			pdc->MoveTo( 0, 0 );
			pdc->LineTo( size.cx, 0 );
			pdc->LineTo( size.cx, size.cy );
			pdc->LineTo( 0, size.cy );
            pdc->LineTo( 0, 0 );
			pdc->EndPath( );
			rgnClip.CreateFromPath( pdc );
			pdc->SelectClipRgn( &rgnClip, RGN_AND );
		
			VERIFY( pdc->DrawText(m_strText, &rcOut, DT_NOCLIP ) );

			pdc->SelectClipRgn( &rgnClip, RGN_DIFF );
			pdc->SelectObject( pOldFont );
		}

		pdc->SetTextColor( clrOldText );
	}



	pdc->SetBkMode( nOldBkMode );
}

/////////////////////////////////////////////////////////////////////////////
// CVTSystem::OnDraw - Drawing function
void CVTSystem::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	m_fFontRatio = DISPLAY_FONT_RATIO;

	CRect rcArea = rcBounds;
	rcArea.DeflateRect( 0, 0, 1, 1 );
	SIZE sizeHM;
	sizeHM.cx = FROM_DOUBLE_TO_VTPIXEL( rcArea.Width() ) / 10;
	sizeHM.cy = FROM_DOUBLE_TO_VTPIXEL( rcArea.Height() ) / 10;

	int nOldMapMode = pdc->SetMapMode( MM_ANISOTROPIC );
	pdc->SetWindowExt( sizeHM.cx, -sizeHM.cy );
	pdc->SetViewportExt( rcArea.Width(), rcArea.Height() );	
	pdc->SetViewportOrg( rcArea.left, rcArea.Height() + rcArea.top );


	double fOldFontRatio = m_fFontRatio;
	m_fFontRatio /= 10.0;

	DrawContext( pdc->GetSafeHdc(), sizeHM );

	m_fFontRatio = fOldFontRatio;

	pdc->SetMapMode( nOldMapMode );
}


/////////////////////////////////////////////////////////////////////////////
// CVTSystem  Properties Access
/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTSystem::GetBackColor() 
{
	return m_BackColor;
}

void CVTSystem::SetBackColor(OLE_COLOR nNewValue) 
{
	m_BackColor = nNewValue;
	BoundPropertyChanged( 1 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTSystem::GetTransparentBack() 
{
	return m_bTransparentBack;
}

void CVTSystem::SetTransparentBack(BOOL bNewValue) 
{
	m_bTransparentBack = bNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTSystem::GetFontColor() 
{
	return m_FontColor;
}

void CVTSystem::SetFontColor(OLE_COLOR nNewValue) 
{
	m_FontColor = nNewValue;
	BoundPropertyChanged( 3 );
	SetModifiedFlag();	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}

/////////////////////////////////////////////////////////////////////////////
short CVTSystem::GetDataType() 
{	
	return (short)m_DataType;
}

void CVTSystem::SetDataType(short nNewValue) 
{
	m_DataType = (SystemType)nNewValue;	
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}
					   
/////////////////////////////////////////////////////////////////////////////
LPFONTDISP CVTSystem::GetTextFont() 
{	
	return m_fontHolder.GetFontDispatch( );
}

void CVTSystem::SetTextFont(LPFONTDISP newValue) 
{	
	m_fontHolder.InitializeFont( &m_fontDesc, newValue);
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();

	FireCtrlPropChange( GetControllingUnknown() );
}


BOOL CVTSystem::GetVisible() 
{
	return ::_IsWindowVisible( this );
}

void CVTSystem::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( !bNewValue )
		ShowWindow( SW_HIDE );
	else
		ShowWindow( SW_SHOWNA );
}

/////////////////////////////////////////////////////////////////////////////
short CVTSystem::GetHorzAlign()
{
	return (short)m_horzAlign;
}

/////////////////////////////////////////////////////////////////////////////
void CVTSystem::SetHorzAlign(short nNewValue)
{
	m_horzAlign = (AlignHorizontal)nNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
short CVTSystem::GetVertAlign()
{
	return (short)m_vertAlign;
}

/////////////////////////////////////////////////////////////////////////////
void CVTSystem::SetVertAlign(short nNewValue)
{
	m_vertAlign = (AlignVertical)nNewValue;
	InvalidateCtrl( this, m_pInPlaceSiteWndless );
	SetModifiedFlag();
}

//added by akm 22_11_k5
BOOL CVTSystem::GetPropertyAXStatus() 
{
	return m_PropertyAXStatus;
}

void CVTSystem::SetPropertyAXStatus(BOOL val) 
{		
	m_PropertyAXStatus=val;
}