#include "stdafx.h"
#include "figurebutton.h"

#define ID_FBTN_TOOLTIP	1

/////////////////////////////////////////////////////////////////////////////
// CFigureButton

CGDIPlusManager *CFigureButton::m_pGDIPlusManager = 0;

CImageDrawer CFigureButton::m_ImgDrawer;

/////////////////////////////////////////////////////////////
CFigureButton::CFigureButton(void) : m_eftype( eftRectangle )
{
	init( );
	init_gdi_plus( );
}

/////////////////////////////////////////////////////////////
CFigureButton::CFigureButton(FigureType ft) : m_eftype( ft )
{
	init( );
	init_gdi_plus( );
}

/////////////////////////////////////////////////////////////
CFigureButton::~CFigureButton(void)
{
	if( m_hbitmap_pushed )
		::DeleteObject( m_hbitmap_pushed ); m_hbitmap_pushed = 0;

	if( m_hicon_pushed )
		::DestroyIcon( m_hicon_pushed ); m_hicon_pushed = 0;

	deinit_gdi_plus( );
}

/////////////////////////////////////////////////////////////
void	CFigureButton::SetThreeState( BOOL buse_three_state /*= TRUE*/ )
{
	m_bthree_state_btn = buse_three_state;
}

/////////////////////////////////////////////////////////////
BOOL	CFigureButton::GetThreeState( )
{
	return m_bthree_state_btn;
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::SetPressedState( BOOL bnew_state /*= TRUE*/ )
{
	if( bnew_state == m_bpushed )
		return m_bpushed;

	BOOL	prev_state = m_bpushed;
	m_bpushed = bnew_state;
	if( GetSafeHwnd() )	Invalidate( );
	return prev_state;
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::IsPressed()
{
	return m_bpushed;
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::SetPushedBitmap( HBITMAP hbitmap )
{
	if( hbitmap && (GetStyle() & BS_BITMAP) )
	{
		if( m_hbitmap_pushed )		
			::DeleteObject( m_hbitmap_pushed ); m_hbitmap_pushed = 0;

		m_hbitmap_pushed = hbitmap;
		Invalidate();
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::SetPushedIcon( HICON hicon )
{
	if( hicon && (GetStyle() & BS_ICON) )
	{
		if( m_hicon_pushed )		
			::DeleteObject( m_hicon_pushed ); m_hicon_pushed = 0;

		m_hicon_pushed = hicon;
		Invalidate();
		return TRUE;
	}

	return FALSE;    
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::SetToolTipText( CString str_toolip )
{
	if( !m_tooltip.GetSafeHwnd() )
	{
		// создаем тултип с родителем 0 (desktop)
		m_tooltip.Create( 0, TTS_ALWAYSTIP );
		m_tooltip.SendMessage( TTM_SETMAXTIPWIDTH, 0,			 SHRT_MAX);
		m_tooltip.SendMessage( TTM_SETDELAYTIME,   TTDT_AUTOPOP, SHRT_MAX);
		m_tooltip.SendMessage( TTM_SETDELAYTIME,   TTDT_INITIAL, 200);
		m_tooltip.SendMessage( TTM_SETDELAYTIME,   TTDT_RESHOW,  200);
	}

	// delete
	if( m_tooltip.GetToolCount() )
		m_tooltip.DelTool( this, ID_FBTN_TOOLTIP );

	// add
	if( !str_toolip.IsEmpty() )
	{
		CRect rect;
		GetClientRect( rect);
		m_tooltip.AddTool( this, str_toolip, rect, ID_FBTN_TOOLTIP );
		m_tooltip.Activate( TRUE );
	}
	else
		m_tooltip.Activate( FALSE );

	return TRUE;
}

/////////////////////////////////////////////////////////////
void	CFigureButton::EnableButton( BOOL benable /*= TRUE*/ )
{
	if( m_bdisable == !benable )
		return;

	m_bdisable = !benable;
	if( GetSafeHwnd() )
		Invalidate( TRUE );
}

/////////////////////////////////////////////////////////////
BOOL	CFigureButton::IsEnableButton( )
{
	return !m_bdisable;
}

/////////////////////////////////////////////////////////////
void	CFigureButton::SetSemiFlat( BOOL bnew )
{
	if( m_bsemiflat == bnew )  
		return;

	m_bsemiflat = bnew;
	if( GetSafeHwnd() )
		Invalidate();
}

/////////////////////////////////////////////////////////////
BOOL	CFigureButton::GetSemiFlat( )
{
	return m_bsemiflat;
}

/////////////////////////////////////////////////////////////
void	CFigureButton::SetFlatButton( BOOL bnew )
{
    if( m_bflat_button== bnew )  
		return;

	m_bflat_button= bnew;
	if( GetSafeHwnd() )
		Invalidate();
}

/////////////////////////////////////////////////////////////
BOOL	CFigureButton::GetFlatButton( )
{
	return m_bflat_button;
}

/////////////////////////////////////////////////////////////
void	CFigureButton::SetDrawBorder( BOOL bnew )
{
    if( m_bdraw_border == bnew )  
		return;

	m_bdraw_border = bnew;
	if( GetSafeHwnd() )
		Invalidate();
}

/////////////////////////////////////////////////////////////
BOOL	CFigureButton::GetDrawBorder( )
{
	return m_bdraw_border;
}



/////////////////////////////////////////////////////////////
void	CFigureButton::init()
{
	m_bthree_state_btn = FALSE;
	m_bpushed = FALSE;

	m_hbitmap_pushed = 0;
	m_hicon_pushed = 0;

	m_bdisable = FALSE;
	m_bmouse_on_button = FALSE;
	m_bsemiflat = FALSE;
	m_bdraw_border = FALSE;
}

/////////////////////////////////////////////////////////////
void	CFigureButton::init_gdi_plus(void)
{
	if( !m_pGDIPlusManager )
		m_pGDIPlusManager = new CGDIPlusManager;
}

/////////////////////////////////////////////////////////////
void	CFigureButton::deinit_gdi_plus(void)
{
	if( m_pGDIPlusManager )
	{
		delete m_pGDIPlusManager;
		m_pGDIPlusManager = 0;
	}        
}

BEGIN_MESSAGE_MAP(CFigureButton, CButton)
	ON_WM_SIZE()
	ON_WM_NCHITTEST()
	ON_CONTROL_REFLECT_EX(BN_CLICKED, OnClicked)
	ON_CONTROL_REFLECT_EX(BN_DBLCLK, OnDblClicked)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////
void CFigureButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	if( !lpDrawItemStruct )
		return;

	if( !lpDrawItemStruct->hDC )
		return;

	Graphics graphics( lpDrawItemStruct->hDC );

	BOOL	bpressed_button = FALSE,
			bdisable_button = FALSE;


	bdisable_button = lpDrawItemStruct->itemState & ODS_DISABLED || m_bdisable;
	
	if( m_bthree_state_btn )
		bpressed_button = m_bpushed;

	if( !bpressed_button )
		bpressed_button = lpDrawItemStruct->itemState & ODS_SELECTED;

	RectF rc;
	RectF rect_inner;
	rc.X = (REAL)(lpDrawItemStruct->rcItem.left);
	rc.Y = (REAL)(lpDrawItemStruct->rcItem.top);
	rc.Width = (REAL)(lpDrawItemStruct->rcItem.right - lpDrawItemStruct->rcItem.left);
	rc.Height = (REAL)(lpDrawItemStruct->rcItem.bottom - lpDrawItemStruct->rcItem.top);

	rect_inner = rc;

	if( m_eftype == eftCircle )
	{
		// 1 fill background
		Color	color0;
		color0.SetFromCOLORREF( ::GetSysColor(COLOR_BTNFACE) );
		SolidBrush mySolidBrush(color0);
		graphics.FillRectangle( &mySolidBrush, rc.X, rc.Y, rc.Width, rc.Height );

		// 2 draw border
		Color	color1 = 0,
			color2 = 0;

		color1.SetFromCOLORREF( ::GetSysColor(COLOR_3DHIGHLIGHT) );
		color2.SetFromCOLORREF( ::GetSysColor(COLOR_3DDKSHADOW) );

		// vanek!!!
		if( bpressed_button )
		{	
			draw_circle( &graphics, rc, color2, color1, 1.f);
			rc.Inflate( -1.f, -1.f);
			rect_inner = draw_circle( &graphics, rc, color1, color2, 1.f );
		}
		else
		{
			if( m_bflat_button && !m_bmouse_on_button )
				color1 = color2 = color0;

			rc.Width -= 1.f;
			rc.Height -= 1.f;
			draw_circle( &graphics, rc, color1, color2, 1.f);

			if( !m_bflat_button && m_bsemiflat && !m_bmouse_on_button )
				color2.SetValue( color2.MakeARGB( color2.GetA() / 3, color2.GetR(), color2.GetG(), color2.GetB()) );				

			rc.Width -= 1.f;
			rc.Height -= 1.f;
			rect_inner = draw_circle( &graphics, rc, color1, color2, 1.f);			
		}

		rect_inner.Inflate( -1.f, -1.f );
	}
	else if( m_eftype == eftRectangle )
	{
		//::DrawFrameControl( lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem), DFC_BUTTON, DFCS_BUTTONPUSH | (bpressed_button ? DFCS_PUSHED : 0) );

		// like as the PushButton control

		// 1. Fill background
		Color	color_back;		
		if( m_bsemiflat && !m_bmouse_on_button && bpressed_button && !bdisable_button )
		{
			COLORREF clrBk = ::GetSysColor(COLOR_3DFACE);
			COLORREF clrHighlight = ::GetSysColor(COLOR_3DHILIGHT);
			COLORREF clrDither = RGB(
				GetRValue(clrBk) + ((GetRValue(clrHighlight) - GetRValue(clrBk)) / 2),
				GetGValue(clrBk) + ((GetGValue(clrHighlight) - GetGValue(clrBk)) / 2),
				GetBValue(clrBk) + ((GetBValue(clrHighlight) - GetBValue(clrBk)) / 2));

			color_back.SetFromCOLORREF( clrDither );
		}
		else
			color_back.SetFromCOLORREF( ::GetSysColor(COLOR_BTNFACE) );

		SolidBrush mySolidBrush(color_back);
		graphics.FillRectangle( &mySolidBrush, rc.X, rc.Y, rc.Width, rc.Height );

		// 2. Draw pressed button
		if( bpressed_button && !bdisable_button )
		{
			if( m_bflat_button)
			{
				if( m_bdraw_border )
				{
					Color clr_pen;
					clr_pen.SetFromCOLORREF( GetSysColor(COLOR_BTNHILIGHT) );
					Pen penBtnHiLight( clr_pen );

                    clr_pen.SetFromCOLORREF( GetSysColor(COLOR_BTNSHADOW) );
					Pen penBtnShadow( clr_pen );

                    // Dark gray line
					graphics.DrawLine( &penBtnShadow, rc.X, rc.Y + rc.Height - 1, 
						rc.X, rc.Y);
					graphics.DrawLine( &penBtnShadow, rc.X, rc.Y, rc.X + rc.Width, rc.Y );
					/*pOldPen = pDC->SelectObject(&penBtnShadow);
					pDC->MoveTo(itemRect.left, itemRect.bottom-1);
					pDC->LineTo(itemRect.left, itemRect.top);
					pDC->LineTo(itemRect.right, itemRect.top);*/

                    // White line
					graphics.DrawLine( &penBtnHiLight, rc.X, rc.Y + rc.Height - 1, 
						rc.X + rc.Width - 1, rc.Y + rc.Height - 1 );
					graphics.DrawLine( &penBtnHiLight, rc.X + rc.Width - 1, rc.Y + rc.Height - 1,
						rc.X + rc.Width - 1, rc.Y - 1 );
				}
			}
			else    
			{
				if( !m_bsemiflat )
					::DrawFrameControl(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem), DFC_BUTTON, DFCS_PUSHED|DFCS_BUTTONPUSH);
				else
				{
					Color clr_pen;
					clr_pen.SetFromCOLORREF( GetSysColor(COLOR_BTNHILIGHT) );
					Pen penBtnHiLight( clr_pen );	// White			

                    clr_pen.SetFromCOLORREF( GetSysColor(COLOR_BTNSHADOW) );
					Pen penBtnShadow( clr_pen );	// Dark gray			

                    // Dark gray line
					graphics.DrawLine( &penBtnShadow, rc.X, rc.Y + rc.Height - 1, 
						rc.X, rc.Y);
					graphics.DrawLine( &penBtnShadow, rc.X, rc.Y, rc.X + rc.Width, rc.Y);

					// White line
					graphics.DrawLine( &penBtnHiLight, rc.X, rc.Y + rc.Height - 1, 
						rc.X + rc.Width - 1, rc.Y + rc.Height - 1 );
					graphics.DrawLine( &penBtnHiLight, rc.X + rc.Width - 1, rc.Y + rc.Height - 1,
						rc.X + rc.Width - 1, rc.Y - 1 );					
				}
			}
		}
		else // ...else draw non pressed button
		{
			Color clr_pen;
			clr_pen.SetFromCOLORREF( GetSysColor(COLOR_BTNHILIGHT) );
			Pen penBtnHiLight( clr_pen );	// White			

			clr_pen.SetFromCOLORREF( GetSysColor(COLOR_3DLIGHT) );
			Pen pen3DLight( clr_pen );	// Light gray			

            clr_pen.SetFromCOLORREF( GetSysColor(COLOR_BTNSHADOW) );
			Pen penBtnShadow( clr_pen );	// Dark gray			

			clr_pen.SetFromCOLORREF( GetSysColor(COLOR_3DDKSHADOW) );
			Pen pen3DDKShadow( clr_pen );	// Black

			if( m_bflat_button)
			{
				if (m_bmouse_on_button && m_bdraw_border && !bdisable_button)
				{
					// White line
					graphics.DrawLine( &penBtnHiLight, rc.X, rc.Y + rc.Height - 1, 
						rc.X, rc.Y);
					graphics.DrawLine( &penBtnHiLight, rc.X, rc.Y,
						rc.X + rc.Width, rc.Y );

					// Dark gray line
					graphics.DrawLine( &penBtnShadow, rc.X, rc.Y + rc.Height - 1, 
						rc.X + rc.Width - 1, rc.Y + rc.Height - 1 );
					graphics.DrawLine( &penBtnShadow, rc.X + rc.Width - 1, rc.Y + rc.Height - 1,
						rc.X + rc.Width - 1, rc.Y - 1 );

				}
			}
			else
			{
				// White line
				graphics.DrawLine( &penBtnHiLight, rc.X, rc.Y + rc.Height - 1, rc.X, rc.Y );
				graphics.DrawLine( &penBtnHiLight, rc.X, rc.Y, rc.X + rc.Width, rc.Y);

				// Light gray line
				graphics.DrawLine( &pen3DLight, rc.X + 1, rc.Y + rc.Height - 1, rc.X + 1,
					rc.Y + 1 );
				graphics.DrawLine( &pen3DLight, rc.X + 1, rc.Y + 1, rc.X + rc.Width, 
					rc.Y + 1);

				// Black line
				graphics.DrawLine( &pen3DDKShadow, rc.X, rc.Y + rc.Height - 1, 
					rc.X + rc.Width- 1,	rc.Y + rc.Height - 1 );
				graphics.DrawLine( &pen3DDKShadow, rc.X + rc.Width- 1, 
					rc.Y + rc.Height - 1, rc.X + rc.Width - 1, 	rc.Y - 1);

				if( !m_bsemiflat)
				{
					// Dark gray line
					graphics.DrawLine( &penBtnShadow, rc.X + 1, rc.Y + rc.Height - 2, 
								rc.X + rc.Width - 2,	rc.Y + rc.Height - 2 );
					graphics.DrawLine( &penBtnShadow, rc.X + rc.Width - 2,	
						rc.Y + rc.Height - 2, rc.X + rc.Width - 2, 	rc.Y);
				}
			}
		}


		if( m_bsemiflat && m_bmouse_on_button && !bpressed_button && !bdisable_button)
		{
			Color clr_pen;
			clr_pen.SetFromCOLORREF( GetSysColor(COLOR_BTNSHADOW) );
			Pen penBtnShadow( clr_pen );	// Dark gray

			// Dark gray line
			graphics.DrawLine( &penBtnShadow, rc.X, rc.Y + rc.Height - 2, 
							rc.X + rc.Width - 2, rc.Y + rc.Height - 2 );
			graphics.DrawLine( &penBtnShadow, rc.X + rc.Width - 2,	
						rc.Y + rc.Height - 2, rc.X + rc.Width - 2, 	rc.Y);
		}

		if( bpressed_button )
			rect_inner.Offset( 1.f, 1.f );

		rect_inner.Inflate( -3.f, -3.f );
	}

	// 3 draw icon and text if need
	Bitmap *pbitmap = 0; 

	HICON	hicon = 0;
	hicon = (bpressed_button && m_hicon_pushed) ? m_hicon_pushed : GetIcon( );

	if( hicon )
		pbitmap = new Bitmap( hicon );
	else
	{
		HBITMAP hbitmap = 0;
		hbitmap = (bpressed_button && m_hbitmap_pushed) ? m_hbitmap_pushed : GetBitmap( );
		if( hbitmap )
			pbitmap = new Bitmap( hbitmap, 0 );
	}


	if( pbitmap )
	{
		// draw image
		ImageAttributes imAtt;

		if( ImageTypeBitmap == pbitmap->GetType() )
		{	
			// set transparent colors for bitmap
			Color clr_transp1, clr_transp2;
			pbitmap->GetPixel( 0, 0, &clr_transp1 );

			clr_transp2.SetValue( Color::MakeARGB( clr_transp1.GetA(), clr_transp1.GetR()+5, clr_transp1.GetG()+5,
				clr_transp1.GetB() + 5) );

			clr_transp1.SetValue( Color::MakeARGB( clr_transp1.GetA(), clr_transp1.GetR()-5, clr_transp1.GetG()-5,
				clr_transp1.GetB()-5) );
			imAtt.SetColorKey( clr_transp1, clr_transp2, ColorAdjustTypeDefault);
		}

		if( bdisable_button )
		{
			// set color matrix for draw "disable" image
			REAL r = 0.f, g = 0.f, b = 0.f;
			COLORREF clr_gray = ::GetSysColor( COLOR_GRAYTEXT );
			r = (REAL)GetRValue( clr_gray ) / 255.f;
			g = (REAL)GetGValue( clr_gray ) / 255.f;
			b = (REAL)GetBValue( clr_gray ) / 255.f;

			ColorMatrix color_matrix = {  
				0.1f,	0.1f,	0.1f,	0.0f,	0.0f,
					0.1f,	0.1f,	0.1f,	0.0f,	0.0f,
					0.1f,	0.1f,	0.1f,	0.0f,	0.0f,
					0.0f,	0.0f,	0.0f,	1.0f,	0.0f,
					r,		g,		b,		0.0f,	1.0f};

			imAtt.SetColorMatrix( &color_matrix, ColorMatrixFlagsDefault, ColorAdjustTypeDefault );
		}

		// vanek : drawing disabled icons - 27.02.2005
		if( !m_ImgDrawer.DrawImage( &graphics, pbitmap, rect_inner, 
			(bdisable_button ? idfUseAlpha : 0) | idfScalingImage, &imAtt ) )
             graphics.DrawImage( pbitmap, rect_inner, 0.f, 0.f, (REAL)pbitmap->GetWidth(), (REAL)pbitmap->GetHeight(),
			UnitPixel, &imAtt, 0, 0);           

		delete pbitmap;
		pbitmap = 0;
	}

	return;
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::PreTranslateMessage(MSG* pMsg)
{
	if( m_tooltip.GetSafeHwnd() )
		m_tooltip.RelayEvent(pMsg);

	return __super::PreTranslateMessage( pMsg );
}

/////////////////////////////////////////////////////////////
RectF	CFigureButton::draw_circle( Graphics *pgraphics, RectF rc, Color color1, Color color2, REAL fwidth )
{
	RectF rc_out;
	rc_out.X = rc_out.Y = rc_out.Width = rc_out.Height = 0;

	if( !pgraphics )
		return rc_out;

	RectF rect( (rc.X + fwidth / 2.f), (rc.Y + fwidth / 2.f), 
		(rc.Width - fwidth - 1.f), (rc.Height - fwidth - 1.f) );

	REAL fradius = 0;
	fradius = min( rect.Width, rect.Height ) / 2.f;

	PointF pt1, pt2, pt3, pt4;
	pt1.X = rect.GetLeft() + fradius;
	pt1.Y = rect.GetTop() + fradius;


	pt2.X = rect.GetRight() - fradius;
	pt2.Y = rect.GetTop() + fradius;

	pt3.X = rect.GetRight() - fradius;
	pt3.Y = rect.GetBottom() - fradius;

	pt4.X = rect.GetLeft() + fradius;
	pt4.Y = rect.GetBottom() - fradius;


	LinearGradientBrush linGrBrush( rect, color1, color2, LinearGradientModeForwardDiagonal );  
	//linGrBrush.SetGammaCorrection( TRUE );

	// Set the smoothing mode to SmoothingModeHighQuality
	pgraphics->SetSmoothingMode(SmoothingModeHighQuality);

	Pen pen( &linGrBrush, fwidth );
	RectF rect_ellipse;

	// draw arc 1
	rect_ellipse.X = pt1.X - fradius;
	rect_ellipse.Y = pt1.Y - fradius;
	rect_ellipse.Width = rect_ellipse.Height = 2.f * fradius;
	pgraphics->DrawArc( &pen, rect_ellipse, -180.0f, 90.0f );

	// draw arc 2
	rect_ellipse.X = pt2.X - fradius;
	rect_ellipse.Y = pt2.Y - fradius;
	rect_ellipse.Width = rect_ellipse.Height = 2.f * fradius;
	pgraphics->DrawArc( &pen, rect_ellipse, -90.0f, 90.0f );

	// draw line between arcs 1-2
	pgraphics->DrawLine( &pen, pt1.X, pt1.Y - fradius, pt2.X, pt2.Y - fradius );

	// draw arc 3
	rect_ellipse.X = pt3.X - fradius;
	rect_ellipse.Y = pt3.Y - fradius;
	rect_ellipse.Width = rect_ellipse.Height = 2.f * fradius;
	pgraphics->DrawArc( &pen, rect_ellipse, 0.0f, 90.0f );

	// draw line between arcs 2-3
	pgraphics->DrawLine( &pen, pt2.X + fradius, pt2.Y, pt3.X + fradius, pt3.Y );

	// draw arc 4
	rect_ellipse.X = pt4.X - fradius;
	rect_ellipse.Y = pt4.Y - fradius;
	rect_ellipse.Width = rect_ellipse.Height = 2.f * fradius;
	pgraphics->DrawArc( &pen, rect_ellipse, 90.0f, 90.0f );

	// draw line between arcs 3-4
	pgraphics->DrawLine( &pen, pt3.X, pt3.Y + fradius, pt4.X, pt4.Y + fradius );

	// draw line between arcs 4-1
	pgraphics->DrawLine( &pen, pt4.X - fradius, pt4.Y, pt1.X - fradius, pt1.Y );

	// calc out rect
	rc_out = rc;
	rc_out.Inflate( -fwidth, -fwidth );
	return rc_out;
}

/////////////////////////////////////////////////////////////
void CFigureButton::PreSubclassWindow()
{
	CButton::PreSubclassWindow( );
	ModifyStyle(0, BS_OWNERDRAW );

	EnableToolTips(TRUE);
	return;
}
/////////////////////////////////////////////////////////////
void	CFigureButton::OnSize( UINT nType, int cx, int cy )
{
	__super::OnSize( nType, cx, cy );
	update_region();

	if( m_tooltip.GetSafeHwnd() )
	{
		CRect rect;
		GetClientRect( rect);
		m_tooltip.SetToolRect( this, ID_FBTN_TOOLTIP , rect );
	}
}

/////////////////////////////////////////////////////////////
LRESULT CFigureButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch( message )	
	{
	case WM_MOUSEMOVE:
		{
			TRACKMOUSEEVENT		csTME = {0};
			csTME.cbSize = sizeof( csTME );
			csTME.dwFlags = TME_LEAVE;
			csTME.hwndTrack = GetSafeHwnd();
			::_TrackMouseEvent( &csTME );

			bool bold_value = m_bmouse_on_button;
			POINT pt = {0};
			pt.x = LOWORD(lParam); 
			pt.y = HIWORD(lParam);
			ClientToScreen( &pt );
			CWnd	*pwnd_under_mouse = 0,
					*pwnd_active = 0;

			pwnd_under_mouse = WindowFromPoint(pt);
			pwnd_active = GetActiveWindow();

			m_bmouse_on_button = pwnd_under_mouse == this;
			if(wParam & MK_LBUTTON)
				m_bmouse_on_button = pwnd_active == this;

			if( m_bmouse_on_button != bold_value )
				Invalidate( );
		}
		break;

	case WM_MOUSELEAVE:
		if( m_tooltip.GetSafeHwnd() )
		{
			m_tooltip.Activate( FALSE );
			m_tooltip.Activate( TRUE );
		}

		if( m_bmouse_on_button )
		{
			m_bmouse_on_button = FALSE;
			Invalidate( );
		}
		break;
	}

	if( m_bdisable && ( (message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) || (message >= WM_KEYFIRST && message <= WM_KEYLAST) ) )
		return 1L;	// block mouse and keyboard input for disabled button

	return __super::WindowProc( message, wParam, lParam );
}


/////////////////////////////////////////////////////////////
void	CFigureButton::update_region( )
{
	if( !GetSafeHwnd( ) )    
		return;

	CRect rect_client;
	GetClientRect( rect_client );

	//SetWindowRgn(NULL, FALSE);

	m_current_region.DeleteObject( );
	switch( m_eftype )
	{
	case eftCircle:
		{
			int nradius = 0;
			nradius = min( rect_client.Width( ), rect_client.Height( ) );
			m_current_region.CreateRoundRectRgn( rect_client.left, rect_client.top, rect_client.right, rect_client.bottom,
				nradius-2, nradius-2 );
		}
		break;

	case eftRectangle:
	default:
		m_current_region.CreateRectRgn( rect_client.left, rect_client.top, rect_client.right, rect_client.bottom );
	}

	//SetWindowRgn( m_current_region, TRUE);
}

/////////////////////////////////////////////////////////////
LRESULT CFigureButton::OnNcHitTest(CPoint point) 
{
	ScreenToClient(&point);
	return m_current_region.PtInRegion(point) ? HTCLIENT : HTNOWHERE;
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::OnClicked()
{
	if( m_bthree_state_btn )
		SetPressedState( !m_bpushed );

	return FALSE;
}

/////////////////////////////////////////////////////////////
BOOL CFigureButton::OnDblClicked()
{
	if( m_bthree_state_btn )
		SetPressedState( !m_bpushed );

	return FALSE;
}