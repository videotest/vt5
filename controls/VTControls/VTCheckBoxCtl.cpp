// VTCheckBoxCtl.cpp : Implementation of the CVTCheckBoxCtrl ActiveX Control class.

#include "stdafx.h"
#include "VTControls.h"
#include "VTCheckBoxCtl.h"
#include "VTCheckBoxPpg.h"
#include "FramesPropPage.h"

#include <msstkppg.h>
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



void InvalidateCtrl( COleControl* pCtrl )
{
	if( !pCtrl )
		return;

	pCtrl->Refresh();	

	/*
	if( !pCtrl || !pCtrl->GetSafeHwnd() )
		return;

	((CWnd*)pCtrl)->Invalidate( );
	((CWnd*)pCtrl)->UpdateWindow( );

	CWnd* pWndParent = pCtrl->GetParent();

	if( !pWndParent || !pWndParent->GetSafeHwnd() )
		return;


	((CWnd*)pWndParent)->Invalidate( );
	((CWnd*)pWndParent)->UpdateWindow( );
	*/
	
	

}



IMPLEMENT_DYNCREATE(CVTCheckBoxCtrl, COleControl)

void InvalidateCtrl( COleControl* pCtrl );

/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CVTCheckBoxCtrl, COleControl)
	//{{AFX_MSG_MAP(CVTCheckBoxCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDBLCLK()		
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CVTCheckBoxCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CVTCheckBoxCtrl)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "TextFont", GetTextFont, SetTextFont, VT_FONT)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "Caption", GetCaption, SetCaption, VT_BSTR)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "UncheckTextColor", GetUncheckTextColor, SetUncheckTextColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "CheckTextColor", GetCheckTextColor, SetCheckTextColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "BackgroundColor", GetBackgroundColor, SetBackgroundColor, VT_COLOR)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "Disabled", GetDisabled, SetDisabled, VT_BOOL)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "Visible", GetVisible, SetVisible, VT_BOOL)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "Checked", GetChecked, SetChecked, VT_I2)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "InnerRaised", GetInnerRaised, SetInnerRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "InnerSunken", GetInnerSunken, SetInnerSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "OuterRaised", GetOuterRaised, SetOuterRaised, VT_BOOL)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "OuterSunken", GetOuterSunken, SetOuterSunken, VT_BOOL)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "NewDrawMode", GetNewDrawMode, SetNewDrawMode, VT_BOOL)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "Appearance3D", GetAppearance3D, SetAppearance3D, VT_BOOL)
	DISP_FUNCTION(CVTCheckBoxCtrl, "SetFocus", SetFocus, VT_EMPTY, VTS_NONE)
	DISP_PROPERTY_EX(CVTCheckBoxCtrl, "UseSystemFont", GetUseSystemFont, SetUseSystemFont, VT_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CVTCheckBoxCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CVTCheckBoxCtrl, COleControl)
	//{{AFX_EVENT_MAP(CVTCheckBoxCtrl)
	EVENT_CUSTOM("OnCheck", FireOnCheck, VTS_I2)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CVTCheckBoxCtrl, 4)
	PROPPAGEID(CVTCheckBoxPropPage::guid)
	PROPPAGEID(CFramesPropPage::guid)
	PROPPAGEID(CLSID_StockColorPage)
	PROPPAGEID(CLSID_StockFontPage)	
END_PROPPAGEIDS(CVTCheckBoxCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CVTCheckBoxCtrl, "VTCHECKBOX.VTCheckBoxCtrl.1",
	0xf6188dc0, 0x9f7f, 0x4021, 0xaa, 0x3c, 0x39, 0x59, 0x56, 0xd0, 0x21, 0xf1)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CVTCheckBoxCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DVTCheckBox =
		{ 0x9899e4f1, 0xa5e, 0x47f9, { 0x8a, 0xb4, 0x8f, 0x9f, 0x9, 0xf5, 0x7, 0xc1 } };
const IID BASED_CODE IID_DVTCheckBoxEvents =
		{ 0xb8c07e38, 0x3c8f, 0x41a5, { 0x9e, 0x3a, 0x45, 0xb4, 0xf0, 0x50, 0xda, 0xc4 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwVTCheckBoxOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CVTCheckBoxCtrl, IDS_VTCHECKBOX, _dwVTCheckBoxOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl::CVTCheckBoxCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CVTCheckBoxCtrl

BOOL CVTCheckBoxCtrl::CVTCheckBoxCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_VTCHECKBOX,
			IDB_VTCHECKBOX,
			afxRegApartmentThreading,
			_dwVTCheckBoxOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl::CVTCheckBoxCtrl - Constructor

CVTCheckBoxCtrl::CVTCheckBoxCtrl() : m_fontHolder( &m_xFontNotification )
{
	InitializeIIDs(&IID_DVTCheckBox, &IID_DVTCheckBoxEvents);
	m_bPressed =  false;

	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl::~CVTCheckBoxCtrl - Destructor

CVTCheckBoxCtrl::~CVTCheckBoxCtrl()
{
	if( m_hAccel ) ::DestroyAcceleratorTable( m_hAccel );
}




const int nCheckImageSize	= 20;
const int nCheckBoxSize		= 16;

/////////////////////////////////////////////////////////////////////////////
CRect CVTCheckBoxCtrl::GetBorderRect( CRect rcClient )
{
	CRect rc = rcClient;
	if( m_bInnerRaised || m_bInnerSunken || m_bOuterRaised || m_bOuterSunken )
	{		
		rc.left		+= 2;
		rc.top		+= 2;
		rc.right	-= 2;
		rc.bottom	-= 2;
	}

	return rc;
}

/////////////////////////////////////////////////////////////////////////////
CRect CVTCheckBoxCtrl::GetCheckRect( CRect rcClient )
{
	CRect rcCheck = GetBorderRect( rcClient );

	rcCheck.left	= rcCheck.left + 2;
	rcCheck.top		= rcCheck.top  + 2;

	rcCheck.right	= rcCheck.left   + nCheckBoxSize;
	rcCheck.bottom	= rcCheck.top	 + nCheckBoxSize;

	return rcCheck;
}

/////////////////////////////////////////////////////////////////////////////
CRect CVTCheckBoxCtrl::GetTextRect( CRect rcClient, CDC* pdc )
{

	bool bOneLine = ( m_strCaption.Find( '\n' ) == -1 );

	CRect rcCheck = GetCheckRect( rcClient );
	int nTextWidth, nTextHeight;
	nTextWidth = nTextHeight = 0;
	{
		CRect rcTextDraw;
		pdc->DrawText( (LPCTSTR)m_strCaption,  &rcTextDraw, DT_CALCRECT );
		nTextWidth  = rcTextDraw.Width() + 1;
		nTextHeight = rcTextDraw.Height() + 1;
	}

	CRect rcText;
	
	if( bOneLine )
		rcText.top		= rcCheck.top + rcCheck.Height() / 2 - nTextHeight/2;
	else
		rcText.top		= rcCheck.top;

	rcText.left		= rcCheck.right + 5;
	rcText.right	= rcText.left + nTextWidth;
	rcText.bottom	= rcText.top  + nTextHeight;

	return rcText;
}

/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl::OnDraw - Drawing function
void CVTCheckBoxCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rc = rcBounds;

	pdc->FillRect( &rc, &CBrush( TranslateColor( m_backgroundColor )) );


	int nOldMode = pdc->SetBkMode( TRANSPARENT );

	CRect rcBorder = GetBorderRect( rc );
	if( rcBorder != rc )
	{
		UINT	edgeType = 0;
		if(m_bInnerRaised)
			edgeType |= BDR_RAISEDINNER;
		if(m_bInnerSunken)
			edgeType |= BDR_SUNKENINNER;
		if(m_bOuterRaised)
			edgeType |= BDR_RAISEDOUTER;
		if(m_bOuterSunken)
			edgeType |= BDR_SUNKENOUTER;

		pdc->DrawEdge(CRect(0,0,rc.Width(),rc.Height()), edgeType, BF_RECT);
	}

	CRect rcCheck	= GetCheckRect( rc );
	{
		CRect rcTemp = rcCheck;
		
		if( !m_bDisabled )
		{
			pdc->FillRect( &rcCheck, &CBrush( RGB(255,255,255) ) );
		}

		if( !m_bNewDrawMode )
		{
			pdc->DrawFrameControl( rcCheck, DFC_BUTTON, DFCS_BUTTON3STATE | 
				( m_bDisabled ? DFCS_INACTIVE : 0 ) );
		}
		else
		{	
			if( !m_bAppearance3D )
			{
				pdc->FrameRect( &rcTemp,  &CBrush( RGB(0,0,0) ) );
				rcTemp.DeflateRect( 1, 1, 1, 1 );
				pdc->FrameRect( &rcTemp,  &CBrush( RGB(0,0,0) ) );		
			}
			else
			{
				CRect rcb = rcTemp;
				pdc->Draw3dRect (&rcb, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
				rcb.DeflateRect( 1, 1, 1, 1 );
				pdc->Draw3dRect (&rcb, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			}
		}
	}


	COLORREF clrOldText = pdc->SetTextColor( ( m_bDisabled ? ::GetSysColor( COLOR_GRAYTEXT ) :
										(m_nChecked ? 
										TranslateColor(m_textCheckColor) :
										TranslateColor(m_textUncheckColor) ) )
										);

	CFont fontCaption;
	if( CreateFont( &fontCaption, pdc->m_hDC ) )
	{
		
		CFont* pOldFont = (CFont*)pdc->SelectObject( &fontCaption );

		CRect rcText	= GetTextRect( rc, pdc );	
		pdc->DrawText( (LPCTSTR)m_strCaption,  &rcText, DT_LEFT );	

		pdc->SelectObject( pOldFont );
		

		if( !m_bDisabled && GetSafeHwnd() && GetFocus() == (CWnd*)this )
		{
			rcText.InflateRect( 1, 1, 1, 1 );
			pdc->DrawFocusRect( rcText );
		}
	}	

	pdc->SetTextColor( clrOldText );

	CPoint ptCheck = CPoint( rcCheck.left, 
							rcCheck.top - ( nCheckImageSize - rcCheck.Height() ) );
	
	if( m_nChecked == 1 )
	{
		if( m_bDisabled )		
			m_ImageList.Draw( pdc, 1, ptCheck, ILD_NORMAL );			
		else
			m_ImageList.Draw( pdc, 0, ptCheck, ILD_NORMAL );	
	}
	else
	if( m_nChecked == 2 )
	{		
		m_ImageList.Draw( pdc, 1, ptCheck, ILD_NORMAL );			
	}
	

	pdc->SetBkMode( nOldMode );
	
}


/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl::DoPropExchange - Persistence support

void CVTCheckBoxCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));	
	PX_Font(pPX, _T("TextFont"), m_fontHolder, &m_fontDesc);
}

/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl::OnResetState - Reset control to default state
void CVTCheckBoxCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::InitDefaults()
{
	m_textUncheckColor	= 0x80000000 + COLOR_WINDOWTEXT;
	m_textCheckColor	= 0x80000000 + COLOR_WINDOWTEXT;
	m_backgroundColor	= 0x80000000 + COLOR_BTNFACE;

	m_nChecked			= 0;

	m_strCaption		= "Caption";

	m_fontDesc.cbSizeofstruct = sizeof( FONTDESC );    
    m_fontDesc.lpstrName	= OLESTR("MS Sans Serif");

    m_fontDesc.cySize.Lo	= 80000;
	m_fontDesc.cySize.Hi	= 0;

    m_fontDesc.sWeight		= FW_THIN;
    m_fontDesc.sCharset		= DEFAULT_CHARSET;
    m_fontDesc.fItalic		= FALSE;
    m_fontDesc.fUnderline	= FALSE;
    m_fontDesc.fStrikethrough = FALSE;

	m_fontHolder.ReleaseFont();
	m_fontHolder.InitializeFont( &m_fontDesc );	

	m_bInnerRaised		= FALSE;
	m_bInnerSunken		= FALSE;
	m_bOuterRaised		= FALSE;
	m_bOuterSunken		= FALSE;


	m_bDisabled			= FALSE;	

	if( m_ImageList.m_hImageList)
		m_ImageList.DeleteImageList();

	VERIFY( m_ImageList.Create( IDB_CHECKED, 20, 0, RGB(255,255,255) ) );	

	m_bNewDrawMode = FALSE;

	m_bAppearance3D = FALSE;

	m_hAccel = 0;
	m_cAccelKey = 0;

	m_bUseSystemFont = TRUE;

}


/////////////////////////////////////////////////////////////////////////////
bool CVTCheckBoxCtrl::IsAccelChar( char ch )
{
	CString strCh = ch;	
	
	strCh.MakeLower();

	CString str = m_cAccelKey;
	str.MakeLower();

	if( str == strCh )
		return true;

	return false;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::FindAccelKey()
{
	m_cAccelKey = 0;

	int nPos = m_strCaption.Find( "&" );
	if( (nPos != -1) && nPos < (m_strCaption.GetLength() - 1) )
	{
		m_cAccelKey = (char)m_strCaption[nPos+1];
		ControlInfoChanged( );
		return TRUE;
	}
	return FALSE;

}


/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnMnemonic(LPMSG pMsg)
{ 
	if( !CanProcessInput() )
		return;
	
	if ((pMsg->message == WM_SYSKEYDOWN)||
		(pMsg->message == WM_SYSCHAR))
	{		
		if( IsAccelChar(pMsg->wParam) )
		{
			SetFocus( );
			UserChangeValue( !GetChecked() );
			return;
		}
	}
	
	COleControl::OnMnemonic(pMsg);

} 

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnGetControlInfo(LPCONTROLINFO pControlInfo)
{

	if( m_hAccel ) ::DestroyAcceleratorTable( m_hAccel );
	
	TCHAR ch = (TCHAR)m_cAccelKey;
                    

	ACCEL accKey[1];

	accKey[0].fVirt	= FVIRTKEY | FALT;	
	accKey[0].key	= LOBYTE(VkKeyScan(ch));
	accKey[0].cmd	= 1;

	// Create the accel table.
	m_hAccel = CreateAcceleratorTable(accKey, 1);	

	if (m_hAccel != NULL)
	{
		// Fill in the CONTROLINFO structure passed in.
		pControlInfo ->hAccel = m_hAccel;
		pControlInfo ->cAccel = 1;
		pControlInfo ->dwFlags = 0;
	}
	else
	{
		COleControl::OnGetControlInfo(pControlInfo);
	}

}




/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl::AboutBox - Display an "About" box to the user

void CVTCheckBoxCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_VTCHECKBOX);
	dlgAbout.DoModal();
}



/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	//cs.dwExStyle |= WS_EX_TRANSPARENT;	
	cs.style		|= WS_TABSTOP;	
	return COleControl::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
DWORD CVTCheckBoxCtrl::GetControlFlags() 
{
	return COleControl::GetControlFlags();/* | windowlessActivate;*/
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::OnEraseBkgnd(CDC* pDC) 
{	
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::PreTranslateMessage(MSG* pMsg) 
{	
		
	if(pMsg->message == WM_SYSKEYDOWN)
	{
		if( CanProcessInput() && IsAccelChar(pMsg->wParam) )
		{
			SetFocus( );
			UserChangeValue( !GetChecked() );
			return TRUE;		
		}
	}

	return COleControl::PreTranslateMessage(pMsg);
}

/////////////////////////////////////////////////////////////////////////////
LRESULT CVTCheckBoxCtrl::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{

	if( message == WM_SYSKEYDOWN )
	{
		CString str = (char)wParam;
		if( CanProcessInput() && IsAccelChar(wParam) )
		{
			SetFocus( );
			UserChangeValue( !GetChecked() );
			return TRUE;		
		}
	}
	else
	if( message == WM_KEYDOWN )
	{
		if( CanProcessInput() && (int)wParam == VK_SPACE )
			UserChangeValue( !GetChecked() );
	}
	else
	if( message == WM_GETTEXT )
	{
		lstrcpyn((LPTSTR)lParam, (LPCTSTR)m_strCaption, wParam);
		if ((int)wParam > m_strCaption.GetLength())
		wParam = m_strCaption.GetLength();
		return TRUE;    
	}

	if( message == WM_SETTEXT )
	{
		if (lParam == NULL)
		{
			// NULL lParam means set caption to nothing
			m_strCaption.Empty();
		}
		else
		{
			// non-NULL sets caption to that specified by lParam
			lstrcpy(m_strCaption.GetBufferSetLength(lstrlen((LPCTSTR)lParam)),
				(LPCTSTR)lParam);
		}
		
		InvalidateAndSetModifiedFlag( );		
		return TRUE;
	}

	
	return COleControl::DefWindowProc(message, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::InvalidateAndSetModifiedFlag()
{
	SetModifiedFlag();
	InvalidateCtrl( this );
}

/////////////////////////////////////////////////////////////////////////////
bool CVTCheckBoxCtrl::CreateFont( CFont* pFont, HDC hDC )
{
	if( !pFont )
		return false;


	IFontPtr fontPtr( m_fontHolder.m_pFont );
	if( fontPtr == NULL )
		return false;


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

	int nHeight = (int)(((double)sizeCY.Lo)/(10000.0) );

	nHeight = -MulDiv( nHeight, GetDeviceCaps( hDC, LOGPIXELSY), 72);
 

	pFont->CreateFont( 
		nHeight , 0, 0, 0,
		nWeight, (BYTE)bItalic, 
		(BYTE)bUnderline, (BYTE)bStrikethrough, 
		(BYTE)DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY,DEFAULT_PITCH,						
		strFontName 
		);


	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CVTCheckBoxCtrl serialization
void CVTCheckBoxCtrl::Serialize(CArchive& ar) 
{
	long nVersion = 4;
	//Back compatibly
	BOOL bVisible = FALSE;

	SerializeStockProps(ar);

	if (ar.IsStoring())
	{	// storing code
		ar<<nVersion;
		BOOL b = (BOOL)m_nChecked;
		ar<<b;
		ar<<m_strCaption;
		ar<<m_textUncheckColor;
		ar<<m_backgroundColor;
		ar<<m_textCheckColor;
		
		ar<<m_bDisabled;
		ar<<bVisible;

		ar<<b;
		ar<<m_bInnerRaised;
		ar<<m_bInnerSunken;
		ar<<m_bOuterRaised;
		ar<<m_bOuterSunken;

		ar<<m_bNewDrawMode;

		ar<<m_bAppearance3D;
		
		ar<<m_bUseSystemFont;


	}
	else
	{	// loading code
		ar>>nVersion;
		BOOL b = FALSE;
		ar>>b;
		m_nChecked = (short)b;

		ar>>m_strCaption;
		ar>>m_textUncheckColor;
		ar>>m_backgroundColor;
		ar>>m_textCheckColor;

		ar>>m_bDisabled;
		ar>>bVisible;

		ar>>b;
		ar>>m_bInnerRaised;
		ar>>m_bInnerSunken;
		ar>>m_bOuterRaised;
		ar>>m_bOuterSunken;

		m_bDisabled			= FALSE;		

		if( nVersion >= 2 )
			ar>>m_bNewDrawMode;

		if( nVersion >= 3 )
			ar>>m_bAppearance3D;

		if( nVersion >= 4 )
			ar>>m_bUseSystemFont;

		FindAccelKey();

	}

	COleControl::Serialize( ar );

}

/////////////////////////////////////////////////////////////////////////////
bool CVTCheckBoxCtrl::IsPointInTarget( CPoint point )
{
	CRect rcClient;
	GetRectInContainer( &rcClient );	

	rcClient.DeflateRect( -rcClient.left, -rcClient.top, rcClient.left, rcClient.top );

	CRect rcCheck = GetCheckRect( rcClient );
	CRect rcText;
	if( GetSafeHwnd() )
	{		
		CClientDC dc(this);

		CFont fontCaption;
		if( CreateFont( &fontCaption, dc.m_hAttribDC ) )
		{			
			CFont* pOldFont = (CFont*)dc.SelectObject( &fontCaption );
			rcText = GetTextRect( rcClient, &dc );
			dc.SelectObject( pOldFont );
		}		
	}
	else
	{
		ASSERT( FALSE );
	}

	return ( rcCheck.PtInRect( point ) || rcText.PtInRect( point ) );	
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{	
	//m_bPressed = false;
	COleControl::OnLButtonDblClk(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{	
	if( !m_bDisabled )
		m_bPressed = IsPointInTarget( point );

	COleControl::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( !m_bDisabled && m_bPressed )
	{
		if( IsPointInTarget( point ) )
		{
			UserChangeValue( !GetChecked() );
			InvalidateCtrl( this );	
		}
	}
	m_bPressed = false;
	COleControl::OnLButtonUp(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{	
	COleControl::OnMouseMove(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	COleControl::OnSetFocus(pOldWnd);
	InvalidateCtrl( this );	
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	COleControl::OnKillFocus(pNewWnd);
	InvalidateCtrl( this );
	
}


/////////////////////////////////////////////////////////////////////////////
LPFONTDISP CVTCheckBoxCtrl::GetTextFont() 
{	
	return m_fontHolder.GetFontDispatch( );
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetTextFont(LPFONTDISP newValue) 
{	
	m_fontHolder.InitializeFont( &m_fontDesc, newValue);	
	InvalidateAndSetModifiedFlag();	
}




/////////////////////////////////////////////////////////////////////////////
BSTR CVTCheckBoxCtrl::GetCaption() 
{
	CString strResult = m_strCaption;	
	return strResult.AllocSysString();
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetCaption(LPCTSTR lpszNewValue) 
{
	m_strCaption = lpszNewValue;
	InvalidateAndSetModifiedFlag();	
	FindAccelKey();
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTCheckBoxCtrl::GetUncheckTextColor() 
{	
	return m_textUncheckColor;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetUncheckTextColor(OLE_COLOR nNewValue) 
{
	m_textUncheckColor = nNewValue;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTCheckBoxCtrl::GetCheckTextColor() 
{		
	return m_textCheckColor;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetCheckTextColor(OLE_COLOR nNewValue) 
{
	m_textCheckColor = nNewValue;
	InvalidateAndSetModifiedFlag();		
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CVTCheckBoxCtrl::GetBackgroundColor() 
{	
	return m_backgroundColor;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetBackgroundColor(OLE_COLOR nNewValue) 
{	
	m_backgroundColor = nNewValue;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetDisabled() 
{	
	return m_bDisabled;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetDisabled(BOOL bNewValue) 
{
	m_bDisabled = bNewValue;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetVisible() 
{	
	return ::_IsWindowVisible( this );
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetVisible(BOOL bNewValue) 
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;

	if( bNewValue )
	{			
		ShowWindow( SW_SHOWNA );			
	}
	else
	{				
		ShowWindow( SW_HIDE );			
	}

	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
short CVTCheckBoxCtrl::GetChecked() 
{	
	return m_nChecked;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::UserChangeValue( short bNewValue )
{
	m_nChecked = bNewValue;
	InvalidateAndSetModifiedFlag();	
	FireOnCheck( m_nChecked );
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetChecked(short bNewValue) 
{
	m_nChecked = bNewValue;
	InvalidateAndSetModifiedFlag();		
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetInnerRaised() 
{
	return m_bInnerRaised;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetInnerRaised(BOOL bNewValue) 
{
	m_bInnerRaised = bNewValue;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetInnerSunken() 
{
	return m_bInnerSunken;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetInnerSunken(BOOL bNewValue) 
{
	m_bInnerSunken = bNewValue;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetOuterRaised() 
{
	return m_bOuterRaised;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetOuterRaised(BOOL bNewValue) 
{
	m_bOuterRaised = bNewValue;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetOuterSunken() 
{
	return m_bOuterSunken;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetOuterSunken(BOOL bNewValue) 
{
	m_bOuterSunken = m_bOuterSunken;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetNewDrawMode()
{
	return m_bNewDrawMode;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetNewDrawMode(BOOL bNewValue)
{
	m_bNewDrawMode = bNewValue;
	InvalidateAndSetModifiedFlag();	
}

/////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetAppearance3D()
{
	return m_bAppearance3D;
}

/////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetAppearance3D(BOOL bNewValue)
{
	m_bAppearance3D = bNewValue;
	InvalidateAndSetModifiedFlag();	
}



/////////////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetFocus()
{
	if( !::IsWindow( GetSafeHwnd() ) )
		return;
	
	CWnd::SetFocus( );
}

/////////////////////////////////////////////////////////////////////////////////////
BOOL CVTCheckBoxCtrl::GetUseSystemFont() 
{
	return m_bUseSystemFont;
}

/////////////////////////////////////////////////////////////////////////////////////
void CVTCheckBoxCtrl::SetUseSystemFont(BOOL bNewValue)
{
	m_bUseSystemFont = bNewValue;
	SetModifiedFlag();
	InvalidateControl();
}


