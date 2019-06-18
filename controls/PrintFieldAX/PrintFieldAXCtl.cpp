// PrintFieldAXCtl.cpp : Implementation of the CPrintFieldAXCtrl ActiveX Control class.

#include "stdafx.h"

#include "PrintFieldAX.h"
#include "PrintFieldAXCtl.h"
#include "PrintFieldAXPpg.h"
#include <msstkppg.h>

#include "\vt5\ifaces\data5.h"
#include "\vt5\awin\misc_utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CPrintFieldAXCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CPrintFieldAXCtrl, COleControl)
	//{{AFX_MSG_MAP(CPrintFieldAXCtrl)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CPrintFieldAXCtrl, COleControl)
	INTERFACE_PART(CPrintFieldAXCtrl, IID_IPrintField, PrintField)
	INTERFACE_PART(CPrintFieldAXCtrl, IID_IVtActiveXCtrl, ActiveXCtrl)
	INTERFACE_PART(CPrintFieldAXCtrl, IID_IAXCtrlDataSite, AXCtrlDataSite)
END_INTERFACE_MAP()

IMPLEMENT_AX_DATA_SITE(CPrintFieldAXCtrl)

/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CPrintFieldAXCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CPrintFieldAXCtrl)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "Group", GetGroup, SetGroup, VT_BSTR)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "EditControlStyle", GetEditControlStyle, SetEditControlStyle, VT_I4)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "CaptionWidth", GetCaptionWidth, SetCaptionWidth, VT_I4)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "CaptioAutoWidth", GetCaptioAutoWidth, SetCaptioAutoWidth, VT_BOOL)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "Caption", GetCaption, SetCaption, VT_BSTR)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "KeyToRead", GetKeyToRead, SetKeyToRead, VT_BSTR)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "UseKeyToRead", GetUseKeyToRead, SetUseKeyToRead, VT_BOOL)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "CaptionAlignment", GetCaptionAlignment, SetCaptionAlignment, VT_I4)
	DISP_PROPERTY_EX(CPrintFieldAXCtrl, "TextColor", GetTextColor, SetTextColor, VT_COLOR )
	DISP_STOCKPROP_TEXT()
	DISP_STOCKPROP_FONT()
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CPrintFieldAXCtrl, COleControl)
	//{{AFX_EVENT_MAP(CPrintFieldAXCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CPrintFieldAXCtrl, 3)
	PROPPAGEID(CPrintFieldAXPropPage::guid)
	PROPPAGEID(CLSID_StockFontPage)
	PROPPAGEID(CLSID_StockColorPage)
END_PROPPAGEIDS(CPrintFieldAXCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CPrintFieldAXCtrl, "PRINTFIELDAX.PrintFieldAXCtrl.1",
	0x38e8c2f4, 0xe97a, 0x44c9, 0xb0, 0xac, 0xe9, 0x59, 0xe8, 0x5, 0xf5, 0x3d)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CPrintFieldAXCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DPrintFieldAX =
		{ 0x3851e866, 0xc6d3, 0x46c4, { 0xbd, 0x74, 0xeb, 0x3b, 0x16, 0xe6, 0x6, 0x92 } };
const IID BASED_CODE IID_DPrintFieldAXEvents =
		{ 0x453ad094, 0xd102, 0x42cd, { 0x86, 0x6d, 0xef, 0x4e, 0x80, 0xda, 0xa2, 0x43 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information


static const DWORD BASED_CODE _dwPrintFieldAXOleMisc =
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CPrintFieldAXCtrl, IDS_PRINTFIELDAX, _dwPrintFieldAXOleMisc)

IMPLEMENT_UNKNOWN(CPrintFieldAXCtrl, ActiveXCtrl);
IMPLEMENT_UNKNOWN(CPrintFieldAXCtrl, PrintField);

HRESULT CPrintFieldAXCtrl::XActiveXCtrl::SetSite( IUnknown *punkVtApp, IUnknown *punkSite )
{
	METHOD_PROLOGUE_EX(CPrintFieldAXCtrl, ActiveXCtrl)
	pThis->m_ptrApp = punkVtApp;
	pThis->m_ptrSite = punkSite;
	return S_OK;
}

HRESULT CPrintFieldAXCtrl::XActiveXCtrl::GetName( BSTR *pbstrName )
{
	METHOD_PROLOGUE_EX(CPrintFieldAXCtrl, ActiveXCtrl)
	*pbstrName = CString( "Print Field" ).AllocSysString();
	return S_OK;
}

HRESULT CPrintFieldAXCtrl::XPrintField::GetParams( print_field_params * pparams )
{
	METHOD_PROLOGUE_EX(CPrintFieldAXCtrl, PrintField)
	memcpy( pparams, &pThis->m_params, sizeof( print_field_params ) );
	return S_OK;
}

HRESULT CPrintFieldAXCtrl::XPrintField::SetParams( print_field_params * pparams )
{
	METHOD_PROLOGUE_EX(CPrintFieldAXCtrl, PrintField)
	memcpy( &pThis->m_params, pparams, sizeof( print_field_params ) );
	return S_OK;
}

HRESULT CPrintFieldAXCtrl::XPrintField::GetNames( BSTR *pbstrCaption, BSTR *pbstrGroup )
{
	METHOD_PROLOGUE_EX(CPrintFieldAXCtrl, PrintField)
	*pbstrCaption = pThis->m_caption.copy();
	*pbstrGroup = pThis->m_group.copy();
	return S_OK;
}

HRESULT CPrintFieldAXCtrl::XPrintField::GetCaptionAlingment( long *plAligment/*PrintFieldAligment enum*/ )
{
	METHOD_PROLOGUE_EX(CPrintFieldAXCtrl, PrintField)
	*plAligment = pThis->m_lAlingment;
	return S_OK;
}

HRESULT CPrintFieldAXCtrl::XPrintField::GetCaptionWidth( long *plWidth/*if aligment == pfa_width*/)
{
	METHOD_PROLOGUE_EX(CPrintFieldAXCtrl, PrintField)
	*plWidth = pThis->m_caption_width;
	return S_OK;
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl::CPrintFieldAXCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CPrintFieldAXCtrl

BOOL CPrintFieldAXCtrl::CPrintFieldAXCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_PRINTFIELDAX,
			IDB_PRINTFIELDAX,
			afxRegApartmentThreading,
			_dwPrintFieldAXOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl::CPrintFieldAXCtrl - Constructor

CPrintFieldAXCtrl::CPrintFieldAXCtrl()
{
	InitializeIIDs(&IID_DPrintFieldAX, &IID_DPrintFieldAXEvents);

	m_clrText = COLOR_WINDOWTEXT;
	OnResetState();
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl::~CPrintFieldAXCtrl - Destructor

CPrintFieldAXCtrl::~CPrintFieldAXCtrl()
{
	// TODO: Cleanup your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl::OnDraw - Drawing function
#define DRAW_3DFRAME	1
#define DRAW_TEXTONLY	0
#define DRAW_ADVANCED	2


void DrawMultiLineText( CDC &dc, const char *psz, size_t cbSize, RECT *prect, int xOff = 0, int yOff = 0 )
{
	const char	*pszSpace = 0;
	const char	*pszCurrent = psz;
	CSize	size = dc.GetTextExtent( "W", 1 );
	long	cyText = size.cy;
	long	cxSpace = size.cx;
	long	nCurrentWidth = prect->right - prect->left;
	long	nWordsCount = 0, xPos = prect->left, yPos = prect->top;

	bool	bDraw;

	while( true )
	{
		for( pszSpace = pszCurrent; *pszSpace; pszSpace++ )
		{
			long	nCurrentLen = (long)pszSpace-(long)pszCurrent;
			size = dc.GetTextExtent( pszCurrent, nCurrentLen );

			if( *pszSpace == ' ' && size.cx < ( prect->right - prect->left ) )
			{
				const char	*pszSpace2 = 0;
				bool bBreak = false;
				for( pszSpace2 = pszSpace + 1; *pszSpace2; pszSpace2++ )
				{
					if( *pszSpace2 == ' ' || *pszSpace2 == '\n' || *pszSpace2 == '\r' )
					{
						long	nCurrentLen2 = (long)pszSpace2-(long)pszCurrent;
						CSize	size2 = dc.GetTextExtent( pszCurrent, nCurrentLen2 );

						if( size2.cx >= ( prect->right - prect->left ) )
							bBreak = true;
						break;
					}
				}

				if( !bBreak && !( *pszSpace2 ) )
				{
					long	nCurrentLen2 = (long)pszSpace2-(long)pszCurrent;
					CSize	size2 = dc.GetTextExtent( pszCurrent, nCurrentLen2 );

					if( size2.cx >= ( prect->right - prect->left ) )
						break;
				}
				else if( bBreak )
					break;
			}

			if( *pszSpace == '\n' )
				break;
			if( *pszSpace == '\r' )
				break;
		}
		
		long	nCurrentLen = (long)pszSpace-(long)pszCurrent;
		size = dc.GetTextExtent( pszCurrent, nCurrentLen );

		nCurrentWidth-=size.cx;
		bDraw = false;

		//на этой строке нет слов - рисуем
		if( nWordsCount == 0 || nCurrentWidth >= 0 )
		{
			::ExtTextOut( dc, 
				xPos + xOff, 
				yPos + yOff,
				ETO_CLIPPED, 
				prect, 
				pszCurrent, 
				nCurrentLen, 0 );
			bDraw = true;
			nWordsCount++;
			xPos += size.cx;
			if( *pszSpace == ' ' )
			{
				xPos+=cxSpace;
				nCurrentWidth-=cxSpace;
			}
		}

		//переходим на следующую строку
		if( nCurrentWidth < 0 || *pszSpace=='\n' )
		{
			yPos += cyText;
			xPos = prect->left;
			nCurrentWidth = prect->right - prect->left;
			nWordsCount = 0;
		}

		//если нерисовали раньше, рисуем
		if( !bDraw )
		{
			::ExtTextOut( dc, 
				xPos + xOff, 
				yPos + yOff,
				ETO_CLIPPED, 
				prect, 
				pszCurrent, 
				nCurrentLen, 0 );			
			nWordsCount++;
			xPos += size.cx;
			if( *pszSpace == ' ' )
			{
				xPos+=cxSpace;
				nCurrentWidth-=cxSpace;
			}
		}

		// vanek - 06.10.2003
		//nCurrentWidth = 0;



		if( !*pszSpace )break;
		pszCurrent = pszSpace+1;
	}


}

void CPrintFieldAXCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	long	lDrawStyle = ::GetValueInt( GetAppUnknown(), "\\Interface", "PrintFieldStyle", 0 );
	LoadValue( );

	if( lDrawStyle == DRAW_3DFRAME )
		pdc->FillRect( (RECT*)&rcBounds, &CBrush( GetSysColor( COLOR_3DFACE ) ) );
	else
		pdc->FillRect( (RECT*)&rcBounds, &CBrush( RGB( 255, 255, 255 ) ) );

	if( lDrawStyle == DRAW_3DFRAME )
		pdc->DrawEdge( (RECT*)&rcBounds, EDGE_RAISED, BF_RECT );
	
	CRect	rectCaption = rcBounds, rectText = rcBounds;

	char	sz[200];

	if( CString( (char *)m_caption ).Find( "%3a" ) > 0 )
		strdecode( sz, m_caption, sizeof( sz ) );
	else
		::strcpy( sz, m_caption );

	//Can't use SelectFontObject( pdc, font_tmp ); direct, cos this change font content
	//depend of m_rcBounds. Must clone font and use its.
	IFont* pi_font = 0;
	if( m_font.m_pFont )
		m_font.m_pFont->Clone( &pi_font );
	
	if( pi_font ) 
	{
		CFontHolder font_tmp( 0 );
		font_tmp.SetFont( pi_font );
		SelectFontObject( pdc, font_tmp );
		font_tmp.ReleaseFont();
		pi_font = 0;
	}

	HFONT	hFont = (HFONT)::SelectObject( *pdc, GetStockObject( SYSTEM_FONT ) );

	LOGFONT	lf;
	ZeroMemory( &lf, sizeof( lf ) );

	::GetObject( hFont, sizeof( LOGFONT), &lf );

/*
	lf.lfWeight= FW_NORMAL;
	lf.lfUnderline = 0;
*/
	lf.lfCharSet = DEFAULT_CHARSET;

	HFONT	hFontNormal = ::CreateFontIndirect( &lf );

	lf.lfWeight= FW_BOLD;
	lf.lfUnderline = 0;

	HFONT	hFontBold = ::CreateFontIndirect( &lf );


	//::SelectObject( *pdc, hFont );
	::SelectObject( *pdc, hFontNormal );


	CSize	size_caption = pdc->GetTextExtent( sz, strlen( sz ) );
	{
/*		CRect	rect1;
		rect1.SetRectEmpty();
		pdc->DrawText( sz, strlen( sz ),  &rect1, DT_CALCRECT|DT_SINGLELINE );
		size_caption = rect1.Size();*/
	}


	if( m_params.flags & PFF_TEXT_STYLE_MULTYLINE )
	{
		rectCaption.bottom = min(rectCaption.top+size_caption.cy+5,rcBounds.bottom);
		rectText.top = rectCaption.bottom;
	}
	else
	{
		// vanek - 06.10.2003
		long w = m_caption_width > 0 ? m_caption_width : -m_caption_width;
		if( w >= 0 )
			rectCaption.right = rectCaption.left+rectCaption.Width()*w/100;	
		else
			rectCaption.right = rectCaption.left+size_caption.cx+10;
		rectText.left = rectCaption.right+10;
	}

	rectText.InflateRect( -2, -2 );

	if( lDrawStyle == DRAW_3DFRAME )
	{
		pdc->FillRect( rectText, &CBrush( RGB( 255, 255, 255 ) ) );
		pdc->DrawEdge( rectText, EDGE_SUNKEN, BF_RECT );
		rectText.InflateRect( -2, -2 );
	}
	else if( lDrawStyle == DRAW_ADVANCED )
	{
		pdc->SelectStockObject( NULL_BRUSH );
		pdc->SelectStockObject( BLACK_PEN );
		pdc->Rectangle( rectText );
		rectText.InflateRect( -2, -2 );
	}
	pdc->SetBkMode( TRANSPARENT ); 
	pdc->SetTextAlign( TA_LEFT|TA_TOP ); 
	pdc->SetTextColor( m_clrText ); 

	if( lDrawStyle == DRAW_ADVANCED )
		::SelectObject( *pdc, hFontNormal );

	::ExtTextOut( *pdc, rectCaption.left+5, rectCaption.top+rectCaption.Height()/2-
		pdc->GetTextExtent( sz, strlen(sz) ).cy/2,
		ETO_CLIPPED, rectCaption, sz, strlen( sz ), 0 );

	if( lDrawStyle == DRAW_ADVANCED )
		::SelectObject( *pdc, hFontBold );

	if( m_params.flags & PFF_TEXT_STYLE_MULTYLINE )
	{
		int x = 0;

		if( m_lAlingment == 0 )
			pdc->SetTextAlign( TA_LEFT|TA_TOP ); 
		else if( m_lAlingment == 1 )
		{
			pdc->SetTextAlign( TA_CENTER | TA_TOP ); 
			x = rectText.Width() / 2;
		}
		else if( m_lAlingment == 2 )
		{
			pdc->SetTextAlign( TA_RIGHT | TA_TOP ); 
			x = rectText.Width();
		}
		else if( m_lAlingment == -1 )
		{
			pdc->SetTextAlign( TA_LEFT | TA_TOP ); 
			int w = m_caption_width > 0 ? m_caption_width : -m_caption_width;
			x = rcBounds.Width() * w / 100. - rectText.left;
			if( x < 0 )
				x = 0;
		}

		::DrawMultiLineText( *pdc, (char*)m_text, CString( (char*)m_text).GetLength(), rectText, x );
	}
	else
	{
		int x = 0;

		if( m_lAlingment == 0 )
			pdc->SetTextAlign( TA_LEFT|TA_TOP ); 
		else if( m_lAlingment == 1 )
		{
			pdc->SetTextAlign( TA_CENTER | TA_TOP ); 
			x = rectText.Width() / 2;
		}
		else if( m_lAlingment == 2 )
		{
			pdc->SetTextAlign( TA_RIGHT | TA_TOP ); 
			x = rectText.Width();
		}
		else if( m_lAlingment == -1 )
		{
			pdc->SetTextAlign( TA_LEFT | TA_TOP ); 
			int w = m_caption_width > 0 ? m_caption_width : -m_caption_width;
			x = rcBounds.Width() * w / 100.- rectText.left;
			if( x < 0 )
				x = 0;
		}

		::ExtTextOut( *pdc, rectText.left + x, rectText.top+rectText.Height()/2-pdc->GetTextExtent( m_text, m_text.length() ).cy/2,
				ETO_CLIPPED, rectText, (char*)m_text, CString( (char*)m_text).GetLength(), 0 );
	}


	//::SelectObject( *pdc, hfont_old );
	::SelectObject( *pdc, ::GetStockObject( SYSTEM_FONT ) );	

	::DeleteObject( hFontNormal );
	::DeleteObject( hFontBold );
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl::DoPropExchange - Persistence support

void CPrintFieldAXCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl::OnResetState - Reset control to default state

static FONTDESC _fontdesc =
  { sizeof(FONTDESC), OLESTR("Arial"), FONTSIZE( 8 ), FW_THIN, // vanek - 17.10.2003
     ANSI_CHARSET, FALSE, FALSE, FALSE };

_bstr_t g_bstr_font_name;


void CPrintFieldAXCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	ZeroMemory( &m_params, sizeof( m_params ) );

	CString str;	
	
	str.LoadString( IDS_CTRL_CAPTION );
	m_caption = (LPCSTR)str;

	str.LoadString( IDS_CTRL_TEXT );
	m_text = (LPCSTR)str;

	str.LoadString( IDS_CTRL_GROUP );
	m_group = (LPCSTR)str;

	m_caption_width = -40;
	m_bUseKeyToRead = false;
	m_lAlingment = 0; // Left
	
	m_font.ReleaseFont();

	const char* psz_sect = "\\PrintFieldAX\\Font";
	IUnknown* punk_app = ::GetAppUnknown();
	g_bstr_font_name = ::GetValueString( punk_app, psz_sect, "Face", "Arial" );
	//read from shell.data
    _fontdesc.lpstrName			= g_bstr_font_name;
    _fontdesc.cySize.Lo			= 10000 * ::GetValueInt( punk_app, psz_sect, "Size", 8 );
	_fontdesc.cySize.Hi			= 0;
    _fontdesc.sWeight			= ::GetValueInt( punk_app, psz_sect, "Weight", FW_THIN );
    _fontdesc.fItalic			= ::GetValueInt( punk_app, psz_sect, "Italic", FALSE );
    _fontdesc.fUnderline		= ::GetValueInt( punk_app, psz_sect, "Underline", FALSE );
    _fontdesc.fStrikethrough	= ::GetValueInt( punk_app, psz_sect, "Strikethrough", FALSE );
	
	m_font.InitializeFont( &_fontdesc );	
}


/////////////////////////////////////////////////////////////////////////////
// CPrintFieldAXCtrl message handlers

BSTR CPrintFieldAXCtrl::GetGroup() 
{	return m_group.copy();}
void CPrintFieldAXCtrl::SetGroup(LPCTSTR lpszNewValue) 
{	m_group=lpszNewValue;SetModifiedFlag();}

long CPrintFieldAXCtrl::GetEditControlStyle() 
{	return m_params.flags & PFF_TYPE_MASKS;}

void CPrintFieldAXCtrl::SetEditControlStyle(long nNewValue) 
{	m_params.flags &= PFF_TYPE_MASKS; m_params.flags |= nNewValue; SetModifiedFlag();}

long CPrintFieldAXCtrl::GetCaptionWidth() 
{	return (m_caption_width>0)?m_caption_width:-m_caption_width;}

void CPrintFieldAXCtrl::SetCaptionWidth(long nNewValue) 
{	m_caption_width = (m_caption_width > 0) ? nNewValue:-nNewValue; LoadValue(); SetModifiedFlag();}


BSTR CPrintFieldAXCtrl::GetCaption() 
{
	char	sz[1000];

	if( CString( (char *)m_caption ).Find( "%3a" ) > 0 )
		strdecode( sz, m_caption, sizeof( sz ) );
	else
		::strcpy( sz, m_caption );

	return _bstr_t( sz ).copy();
}

void CPrintFieldAXCtrl::SetCaption(LPCTSTR lpszNewValue) 
{	
	char	sz[1000];
//	strencode( sz, lpszNewValue, ":\n\r\\", sizeof( sz ) );
	::strcpy( sz, lpszNewValue );
	m_caption = sz; SetModifiedFlag();
}

void CPrintFieldAXCtrl::Serialize( CArchive &ar )
{
	COleControl::Serialize( ar );

	long	lversion = 4;

	if( ar.IsStoring() )
	{
		ar<<lversion;
		CString	s1 = (const char*)m_caption, s2 = (const char*)m_group;
		ar<<s1<<s2<<m_caption_width;
		ar.Write( &m_params, sizeof( m_params ) );
		ar << m_strKeyToRead;
		ar << m_bUseKeyToRead;
		ar << m_lAlingment;
		ar << m_clrText;
	}
	else
	{
		ar>>lversion;
		CString	s1, s2;
		ar>>s1>>s2>>m_caption_width;
		m_caption = s1;
		m_group = s2;
		ar.Read( &m_params, sizeof( m_params ) );

		if( lversion >= 2 )
		{
			ar >> m_strKeyToRead;
			ar >> m_bUseKeyToRead;
		}
		if( lversion >= 3 )
			ar >> m_lAlingment;
		if( lversion >= 4 )
			ar >> m_clrText;
	}
}

extern CString CopyValueName(LPCTSTR lpstrSrc);
void CPrintFieldAXCtrl::LoadValue( )
{
	INamedDataPtr	ptrNamedData( GetAppUnknown() );
	if( ptrNamedData == 0 )return;

	_bstr_t	bstr_path = "\\Values\\";

	CString sValueName;
	if( !m_bUseKeyToRead )
	{
		sValueName = CopyValueName(m_caption);
		bstr_path+=_bstr_t((LPCTSTR)sValueName);
	}
	else
		bstr_path += _bstr_t( m_strKeyToRead );

	variant_t	var;

	long lCount = 0;
	
	bool bOK = false;

	if( !m_bUseKeyToRead )
	{
		ptrNamedData->SetupSection( _bstr_t( "\\Values" )  );
		
		ptrNamedData->GetEntriesCount( &lCount );
		for( long l = 0; l < lCount; l++ )
		{
			_bstr_t bstr;
			ptrNamedData->GetEntryName( l, bstr.GetAddress() ) ;
			if( bstr == _bstr_t( sValueName ) )
			{
				bOK = true;
				break;
			}
		}
	}
	else
		bOK = true;

	if( bOK )
		ptrNamedData->GetValue( bstr_path, &var );

	m_text = var;
}

BSTR CPrintFieldAXCtrl::GetKeyToRead() 
{
	return m_strKeyToRead.AllocSysString();
}

void CPrintFieldAXCtrl::SetKeyToRead(LPCTSTR lpszNewValue) 
{	
	m_strKeyToRead = lpszNewValue; 
	SetModifiedFlag();
}

BOOL CPrintFieldAXCtrl::GetUseKeyToRead()
{
	return m_bUseKeyToRead;
}

void CPrintFieldAXCtrl::SetUseKeyToRead(BOOL bNewValue)
{
	m_bUseKeyToRead = bNewValue;
}

long CPrintFieldAXCtrl::GetCaptionAlignment()
{
	return m_lAlingment;
}

void CPrintFieldAXCtrl::SetCaptionAlignment(long nNewValue)
{
	m_lAlingment = nNewValue;
}

BOOL CPrintFieldAXCtrl::GetCaptioAutoWidth() 
{	return 0;}

void CPrintFieldAXCtrl::SetCaptioAutoWidth(BOOL bNewValue) 
{
}

OLE_COLOR CPrintFieldAXCtrl::GetTextColor() 
{
	return m_clrText;
}

void CPrintFieldAXCtrl::SetTextColor(OLE_COLOR nNewValue) 
{
	m_clrText = nNewValue;
	SetModifiedFlag();
	InvalidateControl();
}