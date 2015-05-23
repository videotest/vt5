// ColorRangeCtrlCtl.cpp : Implementation of the CColorRangeCtrl ActiveX Control class.

#include "stdafx.h"
#include "vtcontrols2.h"
#include "ColorRangeCtrl.h"
#include "ColorRangePpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SIZE_PRECISION	5
#define MIN_WIDTH_PIXEL	5


IMPLEMENT_DYNCREATE(CColorRangeCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CColorRangeCtrl, COleControl)
	//{{AFX_MSG_MAP(CColorRangeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CColorRangeCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CColorRangeCtrl)
	DISP_PROPERTY_EX(CColorRangeCtrl, "MinValue", GetMinValue, SetMinValue, VT_R8)
	DISP_PROPERTY_EX(CColorRangeCtrl, "MaxValue", GetMaxValue, SetMaxValue, VT_R8)
	DISP_PROPERTY_EX(CColorRangeCtrl, "RangesCount", GetRangesCount, SetRangesCount, VT_I4)
	DISP_PROPERTY_EX(CColorRangeCtrl, "BackColor", GetBackColor, SetBackColor, VT_COLOR)
	DISP_FUNCTION(CColorRangeCtrl, "GetRangeColor", GetRangeColor, VT_COLOR, VTS_I4)
	DISP_FUNCTION(CColorRangeCtrl, "SetRangeColor", SetRangeColor, VT_EMPTY, VTS_I4 VTS_COLOR)
	DISP_FUNCTION(CColorRangeCtrl, "GetRangeLen", GetRangeLen, VT_R8, VTS_I4)
	DISP_FUNCTION(CColorRangeCtrl, "SetRangeLen", SetRangeLen, VT_EMPTY, VTS_I4 VTS_R8)
	DISP_FUNCTION(CColorRangeCtrl, "InvalidateCtrl", InvalidateCtrl, VT_EMPTY, VTS_NONE)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CColorRangeCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CColorRangeCtrl, COleControl)
	//{{AFX_EVENT_MAP(CColorRangeCtrl)
	EVENT_CUSTOM("RangeLenChange", FireRangeLenChange, VTS_NONE)
	EVENT_CUSTOM("RangeColorChange", FireRangeColorChange, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CColorRangeCtrl, 1)
	PROPPAGEID(CColorRangePropPage::guid)
END_PROPPAGEIDS(CColorRangeCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CColorRangeCtrl, "VTCONTROLS2.ColorRangeCtrl.1",
	0xa02750e3, 0xb2cc, 0x49db, 0xa8, 0x4b, 0x2c, 0x9c, 0x96, 0x8, 0x1c, 0x8f)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CColorRangeCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DColorRangeCtrl =
		{ 0xc21b81bb, 0x7e16, 0x4630, { 0xa7, 0x13, 0x1a, 0x4, 0x4a, 0xb9, 0xe, 0x58 } };
const IID BASED_CODE IID_DColorRangeCtrlEvents =
		{ 0x5264ad94, 0xc1d2, 0x4afe, { 0x93, 0x59, 0xdb, 0xe4, 0x7f, 0, 0xf9, 0x2f } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwColorRangeCtrlOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CColorRangeCtrl, IDS_COLORRANGE, _dwColorRangeCtrlOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl::CColorRangeCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CColorRangeCtrl

BOOL CColorRangeCtrl::CColorRangeCtrlFactory::UpdateRegistry(BOOL bRegister)
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
			IDS_COLORRANGE,
			IDB_COLORRANGE,
			afxRegApartmentThreading,
			_dwColorRangeCtrlOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl::CColorRangeCtrl - Constructor

CColorRangeCtrl::CColorRangeCtrl()
{
	InitializeIIDs(&IID_DColorRangeCtrl, &IID_DColorRangeCtrlEvents);
	
	m_hcursor_normal	= 0;
	m_hcursor_size		= 0;

	m_hicon_slider		= 0;

	InitDefaults();
}


/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl::~CColorRangeCtrl - Destructor

CColorRangeCtrl::~CColorRangeCtrl()
{
	deinit_array();
}


/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl::OnDraw - Drawing function

void CColorRangeCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	CRect rc_bounds = rcBounds;

	CDC dcImage;
	dcImage.CreateCompatibleDC( pdc );

	CBitmap Bitmap;
	Bitmap.CreateCompatibleBitmap( pdc, rc_bounds.Width(), rc_bounds.Height() );
	CBitmap* pOldBitmap = dcImage.SelectObject( &Bitmap );

	dcImage.SetWindowOrg( CPoint( rc_bounds.left, rc_bounds.top ) );


	dcImage.FillRect( &rcBounds, &CBrush( TranslateColor( m_color_back ) ) );

	dcImage.DrawEdge( &rc_bounds, EDGE_SUNKEN, BF_RECT );

	CArray<CRect,CRect>	ar_rect;

	for( int i=0;i<m_ar_range.GetSize();i++ )
	{
		range_info* prange = m_ar_range[i];
		CRect rc_range = get_range_rect( i );
		ar_rect.Add( rc_range );
		dcImage.FillRect( &rc_range, &CBrush( TranslateColor( prange->m_color ) ) );
	}

	//draw center line
	{
		CPen pen( PS_SOLID, 1, ::GetSysColor( COLOR_BTNFACE ) );
		CPen* pold_pen = dcImage.SelectObject( &pen );		
		int nx_center = rc_bounds.left + rc_bounds.Width() / 2;
		int ny1 = rc_bounds.top + 1;
		int ny2 = rc_bounds.bottom - 1;
		dcImage.MoveTo( nx_center, ny1 );
		dcImage.LineTo( nx_center, ny2 );
		dcImage.SelectObject( pold_pen );
	}

	for( int i=0;i<ar_rect.GetSize();i++ )
	{
		if( i == ar_rect.GetSize() - 1 )
			break;
		CRect rc_item = ar_rect[i];

		if( m_hicon_slider )
		{
			int x = rc_item.right - 8;
			int y = rc_item.top + rc_item.Height() / 2 - 8;
			DrawIconEx( dcImage.m_hDC, x, y, m_hicon_slider, 16, 16, 0, 0, DI_NORMAL );
		}
	}
	ar_rect.RemoveAll();	

	pdc->BitBlt( rc_bounds.left, rc_bounds.top, rc_bounds.Width(), rc_bounds.Height(),
					&dcImage, rc_bounds.left, rc_bounds.top, SRCCOPY );

	dcImage.SelectObject( pOldBitmap );

}


/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl::DoPropExchange - Persistence support

void CColorRangeCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	long lversion = 1;
	PX_Long( pPX, "Version", lversion, 1 );
	// TODO: Call PX_ functions for each persistent custom property.

}


/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl::OnResetState - Reset control to default state

void CColorRangeCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	InitDefaults();
	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CColorRangeCtrl::AboutBox - Display an "About" box to the user
void CColorRangeCtrl::AboutBox()
{
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::InitDefaults()
{
	deinit_array();

	m_fmin			= 0.0;
	m_fmax			= 10.0;

	m_color_back	= 0x80000000 + COLOR_BTNFACE;

	range_info* p	= new range_info;
	p->m_color		= RGB(255,0,0);
	p->m_flen		= 2;
	m_ar_range.Add( p );

	p				= new range_info;
	p->m_color		= RGB(0,255,0);
	p->m_flen		= 6;
	m_ar_range.Add( p );

	p				= new range_info;
	p->m_color		= RGB(0,0,255);
	p->m_flen		= 2;
	m_ar_range.Add( p );

	m_bdraging			= false;
	m_ndrag_range_left	= -1;
	m_ndrag_range_right	= -1;
	m_pt_drag_first		= CPoint( 0, 0 );
}

/////////////////////////////////////////////////////////////////////////////
int CColorRangeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_hcursor_normal	= ::LoadCursor( 0, MAKEINTRESOURCE(IDC_ARROW) );
	m_hcursor_size		= ::LoadCursor( 0, MAKEINTRESOURCE(IDC_SIZEWE) );

	m_hicon_slider		= (HICON)::LoadImage( AfxGetApp()->m_hInstance, MAKEINTRESOURCE(IDI_COLOR_SLIDER), IMAGE_ICON, 16, 16, 0 );

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::OnDestroy() 
{
	if( m_hcursor_normal )
		::DestroyCursor( m_hcursor_normal );	m_hcursor_normal = 0;

	if( m_hcursor_size )
		::DestroyCursor( m_hcursor_size );		m_hcursor_size = 0;

	if( m_hicon_slider )
		::DestroyIcon( m_hicon_slider );	m_hicon_slider = 0;

	COleControl::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
BOOL CColorRangeCtrl::OnEraseBkgnd(CDC* pDC) 
{
	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::Serialize( CArchive& ar )
{
	long lversion = 1;
	if( ar.IsStoring() )
	{
		ar << lversion;
		ar << m_fmin;
		ar << m_fmax;
		ar << m_color_back;

		ar << (long)m_ar_range.GetSize();
		for( int i = 0;i<m_ar_range.GetSize();i++ )
		{
			range_info* p = m_ar_range.GetAt( i );
			ar << p->m_color;
			ar << p->m_flen;
		}
	}
	else
	{
		deinit_array();

		ar >> lversion;
		ar << m_fmin;
		ar << m_fmax;
		ar >> m_color_back;

		long lcount = 0;
		ar >> lcount;
		for( int i=0;i<lcount;i++ )
		{
			range_info* p = new range_info;
			ar >> p->m_color;
			ar >> p->m_flen;

			m_ar_range.Add( p );
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
range_info* CColorRangeCtrl::get_range( int nidx )
{
	if( nidx < 0 || nidx >= m_ar_range.GetSize() )
		return 0;

	return m_ar_range[nidx];
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::deinit_array()
{
	for( int i=0;i<m_ar_range.GetSize();i++ )
		delete m_ar_range[i];

	m_ar_range.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
CRect CColorRangeCtrl::get_range_rect( int nidx )
{
	double funit_in_pixel = get_unit_in_pixel( );
	if( funit_in_pixel <= 0 )
		return NORECT;

	CRect rc_image = get_image_rect();
	double foffset = 0;

	for( int i=0;i<m_ar_range.GetSize();i++ )
	{
		range_info* prange = m_ar_range[i];

		if( i == nidx )
		{
			CRect rc_range = rc_image;
			rc_range.left	= rc_image.left + (long)(foffset / funit_in_pixel - 0.5 );
			rc_range.right	= rc_range.left + (long)( prange->m_flen / funit_in_pixel );
			return rc_range;
		}
		
		foffset += prange->m_flen;
	}

	return NORECT;

}

/////////////////////////////////////////////////////////////////////////////
bool CColorRangeCtrl::get_drag_range( CPoint pt_cur, int* pleft_range, int* pright_range )
{
	if( !GetSafeHwnd() )
		return false;

	int ncount = m_ar_range.GetSize();
	for( int i=0;i<ncount;i++ )
	{
		CRect rc_range = get_range_rect( i );
		if( pt_cur.x > rc_range.right - SIZE_PRECISION && 
			pt_cur.x <= rc_range.right &&
			i != ncount -1 ) 
		{
			if( pleft_range )
				*pleft_range = i;

			if( pright_range )
				*pright_range = i + 1;

			return true;
		}

		if(	pt_cur.x >= rc_range.left && 
			pt_cur.x < rc_range.left +  SIZE_PRECISION  &&
			i != 0 )
			
		{			
			if( pleft_range )
				*pleft_range = i - 1;

			if( pright_range )
				*pright_range = i;

			return true;
		}
	}

	return false;	
}

/////////////////////////////////////////////////////////////////////////////
double CColorRangeCtrl::get_unit_in_pixel( )
{
	CRect rc_image = get_image_rect( );

	double frange = m_fmax - m_fmin;
	if( frange <= 0 || !rc_image.Width() )
		return 0; 

	double foffset = 0;
	double funit_in_pixel = frange / (double)rc_image.Width();
	
	return funit_in_pixel;
}

/////////////////////////////////////////////////////////////////////////////
CRect CColorRangeCtrl::get_image_rect( )
{
	CRect rc_image = NORECT;

	if( !GetSafeHwnd() )
		return NORECT;

	GetClientRect( &rc_image );

	rc_image.DeflateRect( 2, 2, 2, 2 );

	return rc_image;

}

/////////////////////////////////////////////////////////////////////////////
double CColorRangeCtrl::GetMinValue() 
{
	return m_fmin;
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::SetMinValue(double newValue) 
{
	m_fmin = newValue;
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
double CColorRangeCtrl::GetMaxValue() 
{
	return m_fmax;
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::SetMaxValue(double newValue) 
{
	m_fmax = newValue;
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
long CColorRangeCtrl::GetRangesCount() 
{
	return m_ar_range.GetSize();
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::SetRangesCount(long newValue) 
{
	if( newValue < 0 )
		return;

	if( newValue < m_ar_range.GetSize() )
	{
		int nremove = m_ar_range.GetSize() - newValue;
		while( nremove )
		{	
			range_info* p = m_ar_range[ m_ar_range.GetSize() - 1 ];
			delete p;
			m_ar_range.RemoveAt( m_ar_range.GetSize() - 1 );
		}		
	}
	else if( newValue > m_ar_range.GetSize() )
	{
		int nadd = newValue - m_ar_range.GetSize();
		for( int i=0;i<nadd;i++ )
		{
			range_info* p = new range_info;
			m_ar_range.Add( p );
		}
	}
	

	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CColorRangeCtrl::GetRangeColor(long nidx) 
{
	range_info* p = get_range( nidx );
	if( !p )	
	{
		AfxMessageBox( "ColorRangeCtrl.GetRangeColor( idx ). Idx out of range.", MB_ICONERROR );
		return RGB(0,0,0);
	}

	return p->m_color;
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::SetRangeColor(long nidx, OLE_COLOR color) 
{
	range_info* p = get_range( nidx );
	if( !p )	
	{
		AfxMessageBox( "ColorRangeCtrl.SetRangeColor( idx ). Idx out of range.", MB_ICONERROR );
		return;
	}

	p->m_color = color;
}

/////////////////////////////////////////////////////////////////////////////
double CColorRangeCtrl::GetRangeLen(long nidx) 
{
	range_info* p = get_range( nidx );
	if( !p )	
	{
		AfxMessageBox( "ColorRangeCtrl.GetRangeLen( idx ). Idx out of range.", MB_ICONERROR );
		return 0.;
	}

	return p->m_flen;
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::SetRangeLen(long nidx, double flen ) 
{
	range_info* p = get_range( nidx );
	if( !p )	
	{
		AfxMessageBox( "ColorRangeCtrl.SetRangeLen( idx ). Idx out of range.", MB_ICONERROR );
		return;
	}

	p->m_flen = flen;
}

/////////////////////////////////////////////////////////////////////////////
OLE_COLOR CColorRangeCtrl::GetBackColor() 
{
	return m_color_back;
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::SetBackColor(OLE_COLOR nNewValue) 
{
	m_color_back = nNewValue;
	SetModifiedFlag();
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::InvalidateCtrl() 
{
	if( GetSafeHwnd() )
		Invalidate();
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_ndrag_range_left = m_ndrag_range_right = -1;
	get_drag_range( point, &m_ndrag_range_left, &m_ndrag_range_right );
	if( m_ndrag_range_left != -1 && m_ndrag_range_right )
	{
		m_bdraging		= true;
		m_pt_drag_first	= point;
		SetCapture();
	}
	
	COleControl::OnLButtonDown( nFlags, point );
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if( m_bdraging )
	{
		int ncount = m_ar_range.GetSize();
		range_info* prange_left		= get_range( m_ndrag_range_left );
		range_info* prange_right	= get_range( m_ndrag_range_right );

		double funit_in_pixel = get_unit_in_pixel( );	

		if( prange_left && prange_right && funit_in_pixel > 0. )
		{
				int ndelta			= m_pt_drag_first.x - point.x;
				double funit_delta	= ndelta * funit_in_pixel;
				if( ( prange_left->m_flen - funit_delta ) / funit_in_pixel > MIN_WIDTH_PIXEL && 
					( prange_right->m_flen + funit_delta ) / funit_in_pixel > MIN_WIDTH_PIXEL
					)
				{
					prange_left->m_flen		-= funit_delta;
					prange_right->m_flen	+= funit_delta;
					
					Invalidate( );

					FireRangeLenChange();
				}
		}		
	}

	m_pt_drag_first = point;

	COleControl::OnMouseMove( nFlags, point );
}

/////////////////////////////////////////////////////////////////////////////
void CColorRangeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if( m_bdraging )
	{
		m_bdraging = false;
		ReleaseCapture();
	}
	COleControl::OnLButtonUp( nFlags, point );
}


/////////////////////////////////////////////////////////////////////////////
BOOL CColorRangeCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CPoint pt_cur;
	::GetCursorPos( &pt_cur );
	ScreenToClient( &pt_cur );
	if( get_drag_range( pt_cur, 0, 0 ) )
	{
		::SetCursor( m_hcursor_size );	
		return true;
	}
	
	return COleControl::OnSetCursor( pWnd, nHitTest, message );
}

