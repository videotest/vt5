#include "stdafx.h"
#include "resource.h"
#include "idio_object.h"
#include "nameconsts.h"
#include "misc_utils.h"

#define SIMPLE_TYPES_DEFINED
#include "\vt5\awin\type_defs.h"
#include "stdio.h"

#include <atlstr.h>

void _get_parts( CString str, CString &sA, CString &sB, CString &sC, CString &sD )
{
	if( str.IsEmpty() ) 
		return;

	int nPos = str.Find( ".", 0 );
	CString token;
	
	if( nPos != -1 )
		token = str.Left( nPos );
	else
		token = str;

	sA = token.Left(1);
	
	sB = CString( token ).Right( strlen( token ) - 1 );

	if( nPos >= 0 )
		token = str.Tokenize( ".", nPos );
	else
		token = "";

	sC = token;

	if( nPos >= 0 )
		token = str.Tokenize( ".", nPos );
	else
		token = "";

	sD = token;
}

idio_info::idio_info() : CDataInfoBase( clsid_idio, sz_idio_type, szArgumentTypeIdioDBase, IDI_ICON_IDIO )
{}

idio::idio() : m_rectObject( this )
{
	clear();
	m_uFlags = IDIT_INSERTED;
}

HRESULT idio::clear()
{
	bands.deinit();
	centromere = .5;
	relative_length =  1;
	chromo_no = 0;
	m_bShowNums = true;
	return S_OK;
}

#define WINDOW_COORD_X( _x )	int((total->left+size.cx/2.+(_x)) + offsetx)
#define WINDOW_COORD_Y( _y )	int((total->top+(_y)+dbl_centromere_pos))

HRESULT idio::draw( HDC hdc, SIZE size, RECT *total, int centromere_pos, unsigned flags, double fZoom )
{
	_rect	rect_calc;

	rect_calc.left = rect_calc.right = total->left;
	rect_calc.top = rect_calc.bottom = total->top;

	int offsetx = int( 35 * fZoom ) ;

	if( !m_bShowNums )
		offsetx = 0;

	COLORREF	color = 0;

	if(flags & IDF_BLUE )
		color = ::GetValueColor( ::GetAppUnknown(), "\\Colors", "IdioBlue", RGB( 0, 0, 255 ) );
	else
		color = ::GetValueColor( ::GetAppUnknown(), "\\Colors", "IdioBlack", RGB( 0, 0, 0 ) );

	if(flags & IDF_RED )
		color = ::GetValueColor( ::GetAppUnknown(), "\\Colors", "IdioEdit", RGB( 255, 0, 0 ) );

	HPEN	hpen = CreatePen( PS_SOLID, 0, color );
	HPEN	hpen_frame = hpen;
	HBRUSH	hbrush_fill = (HBRUSH)::GetStockObject( WHITE_BRUSH );

	double	length = relative_length;
	if( ( flags & IDF_IGNORELENGTH ) == IDF_IGNORELENGTH )length = 1;
	double	dbl_centromere_pos = centromere_pos;
	if( ( flags & IDF_IGNORECENTROMERE ) == IDF_IGNORECENTROMERE )dbl_centromere_pos = size.cy*length*centromere;


	//calc rect for this idiogram - no scroll and zoom
	double	abs_length = size.cy*length;		//total length of this idiogtam
	double	p_length = abs_length*(1.0-centromere);		//p_hand length 
	double	q_length = abs_length*centromere;			//q_
	double	width2 = size.cx/2.;						//a half of idiogram width


	HPEN	hpen_old = (HPEN)::SelectObject( hdc, hpen_frame );
	HBRUSH	hbr_old = (HBRUSH)::SelectObject( hdc, hbrush_fill );

	HBRUSH	hbr_white = _create_brush( IDIC_WHITE );
	HBRUSH	hbr_black = _create_brush( IDIC_BLACK );
	HBRUSH	hbr_cross = _create_brush( IDIC_CROSS );
	HBRUSH	hbr_dcross = _create_brush( IDIC_DCROSS );
	HBRUSH	hbr_other = _create_brush( IDIC_OTHER );
	HBRUSH	hbr_lt = _create_brush( IDIC_LT );
	HBRUSH	hbr_gr = _create_brush( IDIC_GR );
	HBRUSH	hbr_dk = _create_brush( IDIC_DK );

//	HBRUSH	hbr_sp = _create_brush( IDIC_SP );
	HBRUSH	hbr_link = _create_brush( IDIC_LINK );

	struct	fpoint
	{double	x, y;};
	fpoint	fpoints[100];	
	int	counter = 0;
 	int	delta	= (size.cx / 50 ) + 1;
//#pragma warning(off:4244)
	//left bottom corner
	int nn = 2;
	int mm = 1;
	fpoints[counter].x = -width2+nn*delta;
	fpoints[counter].y = p_length;
	counter++;
	fpoints[counter].x = -width2+mm*delta;
	fpoints[counter].y = p_length-delta;
	counter++;
	fpoints[counter].x = -width2+delta;
	fpoints[counter].y = p_length-mm*delta;
	counter++;
	fpoints[counter].x = -width2;
	fpoints[counter].y = p_length-nn*delta;
	counter++;

	for( TPOS	lpos = bands.head(); lpos; bands.next( lpos ) )
	//centromere
	{
		band	*pband = bands.get( lpos );

		if( (pband->flags & IDIB_SPUTNIC) != IDIB_SPUTNIC )
		{
			bool bDownTo = true;

			{
				TPOS _lpos = bands.head();

				if( _lpos )
				{
					band	*_pband = bands.get( _lpos );

					if( _pband->pos1 != 0 )
						bDownTo = false;
				}
			}
		}


		if( (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE )
		{
 			double center = -q_length+abs_length*pband->pos1;
			
		//bottom right
			fpoints[counter].x = -width2;
			fpoints[counter].y = center+nn*delta;
			counter++;
			fpoints[counter].x = -width2+delta;
			fpoints[counter].y = center+mm*delta;
			counter++;
			fpoints[counter].x = -width2+mm*delta;
			fpoints[counter].y = center+delta;
			counter++;
			fpoints[counter].x = -width2+nn*delta;
			fpoints[counter].y = center;
			counter++;
		//top right corner
			fpoints[counter].x = -width2+nn*delta;
			fpoints[counter].y = center;
			counter++;
			fpoints[counter].x = -width2+mm*delta;
			fpoints[counter].y = center-delta;
			counter++;
			fpoints[counter].x = -width2+delta;
			fpoints[counter].y = center-mm*delta;
			counter++;
			fpoints[counter].x = -width2;
			fpoints[counter].y = center-nn*delta;
			counter++;
		}
	}
	

	//top left corner
	fpoints[counter].x = -width2;
	fpoints[counter].y = -q_length+nn*delta;
	counter++;
	fpoints[counter].x = -width2+delta;
	fpoints[counter].y = -q_length+mm*delta;
	counter++;
	fpoints[counter].x = -width2+mm*delta;
	fpoints[counter].y = -q_length+delta;
	counter++;
	fpoints[counter].x = -width2+nn*delta;
	fpoints[counter].y = -q_length;
	counter++;

	//finalize and link contour
	for( int idx = counter-1; idx>=0; idx-- )
	{
		fpoints[counter].x = -fpoints[idx].x;
		fpoints[counter].y = fpoints[idx].y;
		counter++;
	}

	fpoints[counter].x = fpoints[0].x;
	fpoints[counter].y = fpoints[0].y;
	counter++;


	_point	points[100];
	//convert idio shape using zoom and scroll
	for( int i = 0; i < counter; i++ )
	{
		points[i].x = WINDOW_COORD_X( fpoints[i].x );
		points[i].y = WINDOW_COORD_Y( fpoints[i].y );

		if( ( flags & IDF_CALCRECT ) == IDF_CALCRECT )
		{
			rect_calc.left = min( rect_calc.left, points[i].x );
			rect_calc.right = max( rect_calc.right, points[i].x );
			rect_calc.top = min( rect_calc.top, points[i].y );
			rect_calc.bottom = max( rect_calc.bottom, points[i].y );
		}
	}

	::LPtoDP( hdc, points, counter );
	::LPtoDP( hdc, (POINT*)total, 2 );

	HRGN	hrgn_idio = ::CreatePolygonRgn( points, counter, 1 );
	HRGN	hrgn_idio2 = ::CreatePolygonRgn( points, counter, 1 );
	HRGN	hrgn_total = ::CreateRectRgn( 
total->left - int( 2 * fZoom ), total->top - int( 2 * fZoom ), 
 		total->right + int( 2 * fZoom ), total->bottom + int( 2 * fZoom ) );
	HRGN	hrgn_total2 = ::CreateRectRgn( 
		total->left - int( 2 * fZoom ), total->top - int( 2 * fZoom ), 
 		total->right + int( 2 * fZoom ), total->bottom + int( 2 * fZoom ) );

	::DPtoLP( hdc, points, counter );
	::DPtoLP( hdc, (POINT*)total, 2 );

	RECT rcClip = {0};
	::GetClipBox( hdc, &rcClip );
	
	SIZE _vpExt = {0};
	POINT _vpOrg = {0};
	::GetViewportExtEx( hdc, &_vpExt );
//	::GetViewportOrgEx( hdc, &_vpOrg );

	HRGN hRgnOld = ::CreateRectRgn( _vpOrg.x, _vpOrg.y, _vpOrg.x + _vpExt.cx, _vpOrg.y + _vpExt.cy );
	HRGN hRgnOld2 = ::CreateRectRgn( _vpOrg.x, _vpOrg.y, _vpOrg.x + _vpExt.cx, _vpOrg.y + _vpExt.cy );

	int tRetRes = ::GetClipRgn( hdc, hRgnOld );

	if( tRetRes )
	{
		CombineRgn( hrgn_idio2, hRgnOld, hrgn_idio, RGN_AND );
		::SelectClipRgn( hdc, hrgn_idio2 );
	}
	else
		::SelectClipRgn( hdc, hrgn_idio );

	::SetBkColor( hdc, RGB( 255, 255, 255 ) );
	::SetBkMode( hdc, OPAQUE );
//	::SetBrushOrgEx( hdc, -scroll.x %8, -scroll.y %8, 0 ); 

	HRGN rgn = 0;

	for( lpos = bands.head(); lpos; bands.next( lpos ) )
	{
		band	*pband = bands.get( lpos );

		if( pband->color == IDIC_LINK || (pband->flags & IDIB_LINK ) == IDIB_LINK )
			::SelectObject( hdc, hbr_link );
		else if( pband->color == IDIC_BLACK )
			::SelectObject( hdc, hbr_black );
		else if( pband->color == IDIC_OTHER )
			::SelectObject( hdc, hbr_other );
		else if( pband->color == IDIC_DCROSS )
			::SelectObject( hdc, hbr_dcross );
		else if( pband->color == IDIC_CROSS )
			::SelectObject( hdc, hbr_cross );
		else if( pband->color == IDIC_LT )
			::SelectObject( hdc, hbr_lt );
		else if( pband->color == IDIC_GR )
			::SelectObject( hdc, hbr_gr );
		else if( pband->color == IDIC_DK )
			::SelectObject( hdc, hbr_dk );
		else 
			::SelectObject( hdc, hbr_white );		

		if( (pband->flags & IDIB_LINK ) == IDIB_LINK )
		{
			POINT _pts[2] = { { WINDOW_COORD_X( -width2 ), WINDOW_COORD_Y( -q_length+abs_length*pband->pos1 )}, 
							  { WINDOW_COORD_X( width2 ) + 1, WINDOW_COORD_Y( -q_length+abs_length*pband->pos2 ) } };

			::LPtoDP( hdc, _pts, 2 );

			HRGN _rgn = ::CreateRectRgn( _pts[0].x, _pts[0].y, _pts[1].x, _pts[1].y );

			if( !rgn )
				rgn = _rgn;
			else
			{
				HRGN __rgn = ::CreateRectRgn( total->left, total->top, total->right+1, total->bottom+1 );
				CombineRgn( __rgn, rgn, _rgn, RGN_OR );
				
				::DeleteObject( rgn );
				::DeleteObject( _rgn );
				rgn = __rgn;
			}

		}
		if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
		{
 			RECT rc = { WINDOW_COORD_X( -width2 ) + 1, 
					WINDOW_COORD_Y( -q_length+abs_length*pband->pos1 ),
					WINDOW_COORD_X( width2 ), 
					WINDOW_COORD_Y( -q_length+abs_length*pband->pos2 ) };
				::FillRect( hdc, &rc, 0 );

				if( (pband->flags & IDIB_LINK ) != IDIB_LINK )
				{
					::MoveToEx( hdc, rc.left, rc.top, 0 );
					::LineTo( hdc, rc.right, rc.top);

					::MoveToEx( hdc, rc.left, rc.bottom, 0 );
					::LineTo( hdc, rc.right, rc.bottom );
				}
		}
	}

	if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
	{
		bool bDownTo = true;

		{
			lpos = bands.head();
			if( lpos )
			{
				band	*pband = bands.get( lpos );

				if( pband->pos1 != 0 )
					bDownTo = false;
			}
		}

		if( bDownTo )
			lpos = bands.head();
		else
			lpos = bands.tail();

		double fPrev = 0;
		for( ;lpos; )
		{
			band	*pband = bands.get( lpos );

			if( bDownTo && lpos == bands.head() )
			{
					bands.next( lpos );
					fPrev = pband->pos2;
					continue;
			}
			else if( !bDownTo && lpos == bands.tail() )
			{
					bands.prev( lpos );
					fPrev = pband->pos2;
					continue;
			}

			if( fPrev != pband->pos1 )
			{
				POINT _pts[2] = { { WINDOW_COORD_X( -width2 ) - long( 2 * fZoom), WINDOW_COORD_Y( -q_length+abs_length*fPrev ) + 1 }, 
								{ WINDOW_COORD_X( width2 ) + long( 2 * fZoom), WINDOW_COORD_Y( -q_length+abs_length*pband->pos1 ) } };

				::LPtoDP( hdc, _pts, 2 );

				HRGN _rgn = ::CreateRectRgn( _pts[0].x, _pts[0].y, _pts[1].x, _pts[1].y );

				if( !rgn )
					rgn = _rgn;
				else
				{
					HRGN __rgn = ::CreateRectRgn( total->left, total->top, total->right, total->bottom );
					CombineRgn( __rgn, rgn, _rgn, RGN_OR );
						 
					::DeleteObject( rgn );
					::DeleteObject( _rgn );
					rgn = __rgn;
				}
			}

			fPrev = pband->pos2;
			if( bDownTo )
				bands.next( lpos );
			else
				bands.prev( lpos );
		}
	}


	LOGFONT	lf = {0};
	strcpy( lf.lfFaceName, "Arial" );
	lf.lfHeight = long( fZoom >= 1 ? -8: fZoom * -8 );

	SIZE vpExt = {0}, wExt = {0};
	::GetViewportExtEx( hdc, &vpExt );
	::GetWindowExtEx( hdc, &wExt );

	if( wExt.cx != 0 ) 
	{
		if( vpExt.cx / double ( wExt.cx ) != 1 )
			lf.lfHeight = long( -8 / ( vpExt.cx / double ( wExt.cx ) ) );

 		if( lf.lfHeight > -4 )
			lf.lfHeight = -4;

	}

	HFONT hfont = ::CreateFontIndirect( &lf );

	HFONT	hfont_old = (HFONT)::SelectObject( hdc, hfont );
	::SetBkMode( hdc, TRANSPARENT );
	::SetTextColor( hdc, RGB( 0, 0, 0 ) );

	if( tRetRes )
	{
		CombineRgn( hrgn_total2, hRgnOld, hrgn_total, RGN_AND );
		::SelectClipRgn( hdc, hrgn_total2 );
	}
	else
		::SelectClipRgn( hdc, hrgn_total );

	HRGN _rgn = 0;
	
	if( rgn )
	{
		_rgn = ::CreateRectRgn( 
			total->left, total->top, 
 			total->right, total->bottom );

		CombineRgn( _rgn, hrgn_total, rgn, RGN_DIFF );

		::DeleteObject( rgn );
	}


	if( (flags & IDF_NOTEXT)==0 )
	if( m_bShowNums )
	{
		long nPrev = -1, nPrev2 = -1, nPrev3 = -1, nPrev4 = -1;
		double fPrev = -1, fPrev2 = -1, fPrev3 = -1, fPrev4 = -1;

		double _fPrev = -1;

		bool bDownTo = true;

		{
			lpos = bands.head();
			if( lpos  )
			{
				band	*pband = bands.get( lpos );

				if( pband->pos1 != 0 )
					bDownTo = false;
			}
		}

		if( bDownTo )
			lpos = bands.head();
		else
			lpos = bands.tail();

 		int aval = WINDOW_COORD_X( 0 ) - WINDOW_COORD_X( -width2 ) + ( ( (size.cx / fZoom ) >= 10 && (size.cx / fZoom ) <= 15 ) ? 5 : 0 ); 
		int _max_len = int( -45 * fZoom ) + aval;
		int _simple_len = int( -35 * fZoom ) + aval;
		int _first_line = int( -25 * fZoom ) + aval;
		int _short_len = int( -30 * fZoom )  + aval;
		int _short_len_2 = int( -5 * fZoom );

		int _third_line = int( -17 * fZoom ) + aval;
		int _fourth_line = int( -14 * fZoom )  + aval;
		

		int xpos_t_l = WINDOW_COORD_X( -width2 );
		int xpos_t_r = xpos_t_l + _simple_len;

		int xpos_b_l = xpos_t_l;
		int xpos_b_r = xpos_b_l + _simple_len;					 

		for( ;lpos; )
		{
			band	*pband = bands.get( lpos );

			int		ypos_t = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );
			int		ypos_b = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );

			bool bUp = false, bDown = false;						  
			bool bShort = false, bSputnic = false;

			if( pband->pos1 == 0 )
				bUp = true;

			if( pband->pos2 == 1 )
				bDown = true;

			if( nPrev == -1 )
			{									  
				nPrev = pband->A;
				nPrev2 = pband->b;
				nPrev3 = pband->c;
				nPrev4 = pband->d;

				fPrev = pband->pos1;
				fPrev2 = pband->pos1;
				fPrev3 = pband->pos1;
				fPrev4 = pband->pos1;

				_fPrev = pband->pos2;
			}

			if( !pband->pos1 || (pband->flags & IDIB_CENTROMERE )== IDIB_CENTROMERE )
			{
				xpos_t_r = xpos_t_l + _max_len;
				bUp = true;
			}

			if( (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE )
			{
				RECT rcText = {0}, rcText2 = {0};
				char szText[10] = {0}, szText2[10] = {0};
				
				rcText.left = xpos_t_l + _simple_len;
				rcText.right = xpos_t_l + _max_len;

				rcText2.left = xpos_t_l + _simple_len;
				rcText2.right = xpos_t_l + _max_len;

				rcText.top = WINDOW_COORD_Y( -q_length );
				rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );

				rcText2.top = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );
				rcText2.bottom = WINDOW_COORD_Y( -q_length + abs_length );

				szText[0] = 'p';
				szText2[0] = 'q';

				if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
				{
					::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
					::DrawText( hdc, szText2, -1, &rcText2, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
				}
			}									 

			if( pband->pos2 == 1 )
				xpos_b_r = xpos_b_l + _max_len;

			if( pband->pos1 > _fPrev )
				bShort = true;

			{
				TPOS _pos = lpos;
				bands.next( _pos );

				if( _pos )
				{
					band	*_pband = bands.get( _pos );

					if( _pband->pos1 > pband->pos2 )
						bSputnic = true;
				}
			}

			if( bSputnic || nPrev4 != pband->d || nPrev3 != pband->c || nPrev2 != pband->b || nPrev != pband->A || (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE || pband->pos2 == 1 || pband->pos1 == 0 )
			{
				RECT rcText = {0};
				char szText[10] = {0};

				if( bSputnic || ( nPrev3 != pband->c || nPrev2 != pband->b || nPrev != pband->A || nPrev4 != pband->d || pband->pos2 == 1 || (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE ) )
				{
					szText[0] = '.';
					szText[1] = ' ';
					_itoa( nPrev4, szText + 2, 10 );

					rcText.left = xpos_t_l + _fourth_line;
					rcText.right = rcText.left + _short_len_2;

					rcText.top = WINDOW_COORD_Y( -q_length + abs_length * fPrev4  );

					if(  bSputnic || pband->pos2 == 1 && ( ( nPrev3 != pband->c || nPrev2 != pband->b || nPrev != pband->A ) && nPrev4 == pband->d ) )
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );
					else if( pband->pos2 == 1 && ( nPrev3 != pband->c || nPrev2 != pband->b || nPrev != pband->A || nPrev4 == pband->d ) )
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );
					else
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );

					if( nPrev4 && ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
						::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

					if( ( bSputnic && pband->d != nPrev4 ) || pband->pos2 == 1 && ( nPrev3 != pband->c || nPrev2 != pband->b || nPrev != pband->A || nPrev4 != pband->d ) && pband->d > 0 )
					{
						szText[0] = '.';
						szText[1] = ' ';
						_itoa( pband->d, szText + 2, 10 );

						rcText.left = xpos_t_l + _fourth_line;
						rcText.right = rcText.left + _short_len_2;

						rcText.top = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1  );
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );

						if( pband->d && ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
							::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
					}

					fPrev4 = pband->pos1;
				}
				if( nPrev3 != pband->c || nPrev2 != pband->b || nPrev != pband->A || (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE )
					fPrev4 = pband->pos1;

				nPrev4 = pband->d;
				if( bSputnic )
				{
//						if( pband->pos1 > _fPrev )
//							fPrev2 = pband->pos2;

					TPOS _pos = lpos;
					bands.next( _pos );

					if( _pos )
					{
						band	*_pband = bands.get( _pos );

						if( _pband->pos1 > pband->pos2 )
						{
							nPrev4 = _pband->d;
							fPrev4 = _pband->pos1;
						}
					}
				}
			}


			if( bSputnic || nPrev3 != pband->c || nPrev2 != pband->b || nPrev != pband->A || (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE || pband->pos2 == 1 || pband->pos1 == 0 )
			{
				RECT rcText = {0};
				char szText[10] = {0};

				if(  bSputnic || /*nPrev3 && */( nPrev2 != pband->b || nPrev != pband->A || nPrev3 != pband->c || pband->pos2 == 1 || (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE ) )
				{
					szText[0] = '.';
					szText[1] = ' ';
					_itoa( nPrev3, szText + 2, 10 );

					rcText.left = xpos_t_l + _third_line;
					rcText.right = rcText.left + _short_len_2;

					rcText.top = WINDOW_COORD_Y( -q_length + abs_length * fPrev3  );

					if(  bSputnic || pband->pos2 == 1 && ( ( nPrev2 != pband->b || nPrev != pband->A ) && nPrev3 == pband->c ) )
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );
					else if( pband->pos2 == 1 && ( nPrev2 != pband->b || nPrev != pband->A || nPrev3 == pband->c ) )
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );
					else
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );

					if( nPrev3 && ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
						::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

					if( ( bSputnic && pband->c != nPrev3 ) || pband->pos2 == 1 && (nPrev2 != pband->b || nPrev != pband->A || nPrev3 != pband->c) && pband->c > 0 )
					{
						szText[0] = '.';
						szText[1] = ' ';
						_itoa( pband->c, szText + 2, 10 );

						rcText.left = xpos_t_l + _third_line;
						rcText.right = rcText.left + _short_len_2;

						rcText.top = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1  );
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );

						if( pband->c && ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
							::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
					}

					fPrev3 = pband->pos1;
				}
				if( nPrev2 != pband->b || nPrev != pband->A || (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE )
					fPrev3 = pband->pos1;

				nPrev3 = pband->c;

				if( bSputnic )
				{
//						if( pband->pos1 > _fPrev )
//							fPrev2 = pband->pos2;

					TPOS _pos = lpos;
					bands.next( _pos );

					if( _pos )
					{
						band	*_pband = bands.get( _pos );

						if( _pband->pos1 > pband->pos2 )
						{
							nPrev3 = _pband->c;
							fPrev3 = _pband->pos1;
						}
					}
				}
			}

			if( bSputnic || nPrev2 != pband->b || nPrev != pband->A || pband->flags == IDIB_CENTROMERE || pband->pos2 == 1 || pband->pos1 == 0 )
			{
				RECT rcText = {0};
				char szText[10] = {0};

				if( bSputnic || nPrev2 != pband->b  || ( pband->flags != IDIB_CENTROMERE && pband->pos2 != 1 && pband->pos1 != 0 ) )
					bShort = true;

				if( bSputnic && nPrev2 == pband->b )
					bShort = false;


				if( bSputnic || nPrev != pband->A ||nPrev2 != pband->b || pband->pos2 == 1 || pband->flags == IDIB_CENTROMERE )
				{

					_itoa( nPrev2, szText, 10 );

					rcText.left = xpos_t_l + _first_line - _short_len_2;
					rcText.right = rcText.left + _short_len_2;

					rcText.top = WINDOW_COORD_Y( -q_length + abs_length * fPrev2  );

					if( (bSputnic && nPrev2 == pband->b ) || nPrev == pband->A && pband->pos2 == 1 && nPrev2 == pband->b && pband->flags != IDIB_CENTROMERE )
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );
					else
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );

					if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
						::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

					if( ( bSputnic && nPrev2 != pband->b ) || !pband->pos2 == 1 && ( nPrev != pband->A || nPrev2 != pband->b || pband->flags == IDIB_CENTROMERE ) )
					{
						_itoa( pband->b, szText, 10 );

						rcText.left = xpos_t_l + _first_line - _short_len_2;
						rcText.right = rcText.left + _short_len_2;

						rcText.top = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1  );
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );

						if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
							::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
					}
					else if( pband->pos2 == 1 && ( nPrev != pband->A || nPrev2 != pband->b || pband->flags == IDIB_CENTROMERE ) )
					{
						_itoa( pband->b, szText, 10 );

						rcText.left = xpos_t_l + _first_line - _short_len_2;
						rcText.right = rcText.left + _short_len_2;

						rcText.top = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1  );
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );

						if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
							::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
					}

					nPrev2 = pband->b;
					fPrev2 = pband->pos1;

					if( bSputnic )
					{
//						if( pband->pos1 > _fPrev )
//							fPrev2 = pband->pos2;

						TPOS _pos = lpos;
						bands.next( _pos );

						if( _pos )
						{
							band	*_pband = bands.get( _pos );

							if( _pband->pos1 > pband->pos2 )
							{
								nPrev2 = _pband->b;
								fPrev2 = _pband->pos1;
							}
						}
					}
				}
			}

			_fPrev = pband->pos2;

			if( nPrev != pband->A || pband->flags == IDIB_CENTROMERE || pband->pos1 == 0 || pband->pos2 == 1 )
			{
				RECT rcText = {0};
				char szText[10] = {0};

				szText[0] = (char)nPrev;
				if( pband->flags != IDIB_CENTROMERE && pband->pos1 != 0 && ( pband->pos2 != 1 || nPrev != pband->A ) )
				{
					xpos_t_r = xpos_t_l + _short_len;
					bUp = true;
				}

				rcText.left = xpos_t_l + _first_line;
				rcText.right = xpos_t_l + _simple_len;

				if( pband->flags == IDIB_CENTROMERE && pband->pos2 == 1 )
				{
					rcText.top = WINDOW_COORD_Y( -q_length + abs_length * fPrev  );
					rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );

					if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
						::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

					szText[0] = (char)pband->A;

					rcText.top = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1  );
					rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );
				}
				else
				{
					if( (pband->flags & IDIB_CENTROMERE) == IDIB_CENTROMERE )
					{
						rcText.top = WINDOW_COORD_Y( -q_length + abs_length * fPrev  );
						rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );
					}
					else if( pband->pos2 == 1 || nPrev != pband->A )
					{
						rcText.top = WINDOW_COORD_Y( -q_length + abs_length * fPrev  );
						if( pband->pos2 != 1 )
							rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );
						else
						{
							if( nPrev != pband->A )
								rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1 );
							else
								rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );
						}

						if( pband->pos2 == 1 && nPrev != pband->A )
						{
							if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
								::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );

							rcText.top = WINDOW_COORD_Y( -q_length + abs_length * pband->pos1  );
							rcText.bottom = WINDOW_COORD_Y( -q_length + abs_length * pband->pos2 );

							szText[0] = (char)pband->A;
						}

					}
				}
				if( rcText.top != rcText.bottom )
				{
					nPrev = pband->A;
					fPrev = pband->pos1;

					if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
						::DrawText( hdc, szText, -1, &rcText, DT_VCENTER | DT_CENTER | DT_SINGLELINE );
				}
			}

			if( ( flags & IDF_CALCRECT ) == IDF_CALCRECT )
			{
				rect_calc.left = min( rect_calc.left, xpos_t_l );
				rect_calc.right = max( rect_calc.right, xpos_t_r );
				rect_calc.top = min( rect_calc.top, ypos_t );
				rect_calc.bottom = max( rect_calc.bottom, ypos_b );

			}
			else
			{
				if( bUp )
				{
					::MoveToEx( hdc, xpos_t_l, ypos_t, 0 );
					::LineTo( hdc, xpos_t_r, ypos_t );
				}

				if( bDown )
				{
					::MoveToEx( hdc, xpos_b_l, ypos_b, 0 );
					::LineTo( hdc, xpos_b_r, ypos_b );
				}

 				if( bShort || bSputnic )
				{
					int xpos_b_l = WINDOW_COORD_X( -width2 ) + _first_line - _short_len_2;
					int xpos_b_r = xpos_b_l + _short_len_2;

					if( bShort )
					{
						::MoveToEx( hdc, xpos_b_l, ypos_t, 0 );
						::LineTo( hdc, xpos_b_r, ypos_t );
					}

					if( bSputnic )
					{
						::MoveToEx( hdc, xpos_b_l, ypos_b, 0 );
						::LineTo( hdc, xpos_b_r, ypos_b );
					}
				}
			}
			if( bDownTo )
				bands.next( lpos );
			else
				bands.prev( lpos );
		}


		int		ypos_t = WINDOW_COORD_Y( -q_length );
		int		ypos_b = WINDOW_COORD_Y( -q_length + abs_length );

		if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
		{
			::MoveToEx( hdc, xpos_t_l + _first_line, ypos_t, 0 );
			::LineTo(   hdc, xpos_t_l + _first_line, ypos_b );

			::MoveToEx( hdc, xpos_t_l + _simple_len, ypos_t, 0 );
			::LineTo(   hdc, xpos_t_l + _simple_len, ypos_b );
		}
	}
  	::SelectObject( hdc, GetStockObject( NULL_BRUSH ) );

	if( _rgn && tRetRes )
	{
		::CombineRgn( hRgnOld2, hRgnOld, _rgn, RGN_AND );
		::SelectClipRgn( hdc, hRgnOld2 );
	}
	else
	{
		if( tRetRes )
			::SelectClipRgn( hdc, hRgnOld );
		else
			::SelectClipRgn( hdc, _rgn );
	}

	if( ( flags & IDF_CALCRECT ) != IDF_CALCRECT )
		::Polygon( hdc, points, counter );

	if( _rgn )
		DeleteObject( _rgn );

	if( tRetRes )
		::SelectClipRgn( hdc, hRgnOld );
	else
		::SelectClipRgn( hdc, 0 );

	::DeleteObject( hRgnOld );
	::DeleteObject( hRgnOld2 );

	::SelectObject( hdc, hfont_old );
	::SelectObject( hdc, hbr_old );
	::SelectObject( hdc, hpen_old );

	::DeleteObject( hbr_white );
	::DeleteObject( hbr_black );
	::DeleteObject( hbr_cross );
	::DeleteObject( hbr_dcross );
	::DeleteObject( hbr_other );
	::DeleteObject( hbr_dk );
	::DeleteObject( hbr_gr );
	::DeleteObject( hbr_lt );

//	::DeleteObject( hbr_sp );
	::DeleteObject( hbr_link );

	::DeleteObject( hfont );
	::DeleteObject( hrgn_idio );
	::DeleteObject( hrgn_total );
	::DeleteObject( hrgn_idio2 );
	::DeleteObject( hrgn_total2 );
	::DeleteObject( hpen );

	if( ( flags & IDF_CALCRECT ) == IDF_CALCRECT )
	{
		total->left = rect_calc.left;
		total->top = rect_calc.top;
		total->right = rect_calc.right + 1;
		total->bottom = rect_calc.bottom;
	}
	
	m_IdioRect = *total;

	return S_OK;
}

int strcmpr( const char *psz1, const char *psz2 )
{
	int	len = strlen( psz2 );
	return strnicmp( psz1, psz2, len );
}

HRESULT idio::open_text( BSTR filename )
{
	FILE	*pfile = ::fopen( _bstr_t(filename), "rt" );
	if( !pfile )return E_INVALIDARG;

	clear();

	char	sz[200];
	bool	reading_bands = false;
	long	bands_count = 0;

	while( fgets( sz, sizeof( sz ), pfile ) )
	{
		if( reading_bands )
		{
			if( !bands_count )break;
			band	*pnew = new band();
			
			char *p = strchr( sz, '=' );
			if( p )
			{
				*p=0;p++;			
			}

			strcpy( pnew->sz_number, strtok( sz, "=" ) );
			sscanf( p, "[%lf %lf], %X %X", &pnew->pos1, &pnew->pos2, &pnew->flags, &pnew->color );

			_calc_level( pnew->sz_number, pnew );

			bands.insert_before( pnew, 0 );
			bands_count--;
			continue;
		}
		else if( !strcmpr( sz, "[Ch" ) )
		{
			sscanf( sz, "[Ch%d]", &chromo_no );
			continue;
		}
		else if( !strcmpr( sz, "RelativeLength" ) )
		{
			sscanf( sz, "RelativeLength=%lf", &relative_length );
			continue;
		}
		else if( !strcmpr( sz, "Centromere" ) )
		{
			sscanf( sz, "Centromere=%lf", &centromere );
			continue;
		}
		else if( !strcmpr( sz, "BandsCount" ) )
		{
			sscanf( sz, "BandsCount=%d", &bands_count );
			reading_bands = true;
		}

	}

	fclose( pfile );

	return S_OK;
}

HRESULT idio::store_text( BSTR filename )
{
	FILE	*pfile = ::fopen( _bstr_t(filename), "wt" );


	if( !pfile )return E_INVALIDARG;
	
	fprintf( pfile, "[Ch%d]\n", chromo_no );
	fprintf( pfile, "RelativeLength=%g\n", relative_length );
	fprintf( pfile, "Centromere=%g\n", centromere );
	fprintf( pfile, "BandsCount=%d\n", bands.Count() );


	for (TPOS lpos = bands.head(); lpos; bands.next(lpos))
	{
		band	*pband = bands.get( lpos );
		//0.0.1=[0.01 0.02], 0, 255
		fprintf( pfile, "%s=[%g %g], %X %X\n", pband->sz_number, pband->pos1, pband->pos2, pband->flags, pband->color );
	}


	::fclose( pfile );
	return S_OK;
}

HRESULT idio::Add( double pos1, double pos2, int color, BSTR sz_number, int flags )
{
	band	*pnew = new band();
	
	TPOS lPos = bands.tail();
	bands.insert( pnew, lPos );
	lPos = bands.tail();

	return _set_bend_params( lPos, &pos1, &pos2, &color, &sz_number, &flags );
}

HRESULT idio::AddAfter( int nID, double pos1, double pos2, int color, BSTR sz_number, int flags )
{
	TPOS lPos = _get_pos_by_id(nID);

	band	*pnew = new band();
	bands.insert( pnew, lPos );

	lPos = _get_pos_by_id( nID + 1 );

	return _set_bend_params( lPos, &pos1, &pos2, &color, &sz_number, &flags );
}

HRESULT idio::SetBendParams( int nID, double pos1, double pos2, int color, BSTR sz_number, int flags )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _set_bend_params( lPos, &pos1, &pos2, &color, &sz_number, &flags );
}

HRESULT idio::GetBendParams( int nID, double *pos1, double *pos2, int *color, BSTR *sz_number, int *flags )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _get_bend_params( lPos, pos1, pos2, color, sz_number, flags );
}

HRESULT idio::SetBendFillStyle( int nID, int color )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _set_bend_params( lPos, 0, 0, &color );
}

HRESULT idio::GetBendFillStyle( int nID, int *color )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _get_bend_params( lPos, 0, 0, color );
}

HRESULT idio::SetBendRange( int nID, double pos1, double pos2 )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _set_bend_params( lPos, &pos1, &pos2 );
}

HRESULT idio::GetBendRange( int nID, double *pos1, double *pos2 )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _get_bend_params( lPos, pos1, pos2 );
}

HRESULT idio::SetBendNumber( int nID, BSTR sz_number )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _set_bend_params( lPos, 0, 0, 0, &sz_number );
}

HRESULT idio::GetBendNumber( int nID, BSTR *sz_number )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _get_bend_params( lPos, 0, 0, 0, sz_number );
}

HRESULT idio::SetBendFlag( int nID, int flag )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _set_bend_params( lPos, 0, 0, 0, 0, &flag );
}

HRESULT idio::GetBendFlag( int nID, int *flag )
{
	TPOS lPos = _get_pos_by_id(nID);
	return _get_bend_params( lPos, 0, 0, 0, 0, flag );
}

HRESULT idio::GetBengsCount( long *lSz )
{
	if( !lSz )
		return S_FALSE;

	*lSz = bands.Count();
	return S_OK;
}

TPOS idio::_get_pos_by_id(int nID)
{
	int i = 0;
	for (TPOS lpos = bands.head(); lpos; bands.next(lpos), i++)
	{
		if( i == nID )
			return lpos;
	}
	return 0;
}

HRESULT idio::_set_bend_params( TPOS lPos, const double *pos1, const double *pos2, const int *color, const BSTR *sz_number, const int *flags )
{
	if( !lPos )
		return S_FALSE; 

	band	*pband = bands.get( lPos );

	if( !pband || ( !pos1 && !pos2 && !color && !sz_number && !flags ) )
		return S_FALSE;

	if( pos1 )
		pband->pos1 = *pos1;

	if( pos2 )
		pband->pos2 = *pos2;

	if( color )
	{
		if( (pband->flags & IDIB_CENTROMERE) != IDIB_CENTROMERE || *color != IDIC_LINK )
		{
			pband->color = *color;

			if( pband->color == IDIC_LINK )
				pband->flags &= ~IDIB_LINK;
			else if( (pband->flags & IDIB_CENTROMERE) != IDIB_CENTROMERE )
				pband->flags = 0;
		}

	}

	if( flags )
	{
		pband->flags = *flags;
		if( (pband->flags & IDIB_LINK) == IDIB_LINK )
			pband->color = IDIC_LINK;
	}

	if( sz_number && *sz_number )
	{
		bstr_t bstr( *sz_number );

		CString a, b, c, d, res;
		_get_parts( (char*)bstr, a, b, c, d );

		if( !isdigit_ru( a[0] ) )
		{
			if( a[0] > 'D' )
				a = "D";
			else if( a[0] < 'A' )
				a = "A";
		}
		else
		{
			if( atoi(a) > 4 )
				a = "4";
			else if( atoi(a) < 1 )
				a = "1";
		}

		{
			if( atoi(b) < 1 )
				b = "1";
		}

		if( !c.IsEmpty() )
		{
			if( atoi(c) < 1 )
				c = "1";
		}

		if( !d.IsEmpty() )
		{
			if( atoi(d) < 1 )
				d = "1";
		}

		res = a + b;

		if( !c.IsEmpty() )
		{
			res += "." + c;
			
			if( !d.IsEmpty() )
				res += "." + d;
		}

		bstr = res;
		strncpy(pband->sz_number,(char *)bstr, _NUM_SIZE );
		_calc_level( pband->sz_number, pband );
	}

	return S_OK;

}

HRESULT idio::_get_bend_params(TPOS lPos, double *pos1, double *pos2, int *color, BSTR *sz_number, int *flags)
{
	if( !lPos )
		return S_FALSE; 

	band	*pband = bands.get( lPos );

	if( !pband || ( !pos1 && !pos2 && !color && !sz_number && !flags ) )
		return S_FALSE;

	if( pos1 )
		*pos1 = pband->pos1;

	if( pos2 )
		*pos2 = pband->pos2;

	if( color )
		*color = pband->color;

	if( flags )
		*flags = pband->flags;


	if( sz_number )
	{
		bstr_t bstr( pband->sz_number );
		*sz_number = bstr.copy();
	}

	return S_OK;

}

/***************************************************************************************************/
HBRUSH idio::_create_brush( int nType )
{
	if( nType == IDIC_BLACK )
		return ::CreateSolidBrush( 0 );
	else if( nType == IDIC_OTHER )
		return ::CreateHatchBrush( HS_CROSS, 0 );
	else if( nType == IDIC_DCROSS )
		return ::CreateHatchBrush( HS_DIAGCROSS, 0 );
	else if( nType == IDIC_CROSS )
		return ::CreateHatchBrush( HS_FDIAGONAL, 0 );
	else if( nType == IDIC_LT )
		return ::CreateSolidBrush( RGB( 192, 192, 192 ) );
	else if( nType == IDIC_GR )
		return ::CreateSolidBrush( RGB( 128, 128, 128 ) );
	else if( nType == IDIC_DK )
		return ::CreateSolidBrush( RGB( 96, 96, 96 ) );
	else if( nType == IDIC_WHITE )
		return ::CreateSolidBrush( RGB( 255, 255, 255 ) );
	
//	else if( nType == IDIC_SP )
//		return (HBRUSH)::CreateHatchBrush( HS_HORIZONTAL, 0 );
	else if( nType == IDIC_LINK )
		return (HBRUSH)::CreateHatchBrush( HS_HORIZONTAL, 0 );

	return 0;
}

HRESULT idio::GetBrush( int nType, HBRUSH *phBrush )
{
	*phBrush = _create_brush( nType );

	if( *phBrush == 0 )
		return S_FALSE;

	return S_OK;
}

HRESULT idio::GetRect( RECT *rc )
{
	if( !rc )
		return S_FALSE;

	*rc = m_IdioRect;

	return S_OK;

}
/***************************************************************************************************/
HRESULT idio::Remove( int nID )
{
	if( nID != -1 )
	{
		TPOS lPos = _get_pos_by_id(nID);
		bands.remove( lPos );
	}
	else
	{
		TPOS lPos = bands.head();
		while( lPos )
		{
			bands.remove( lPos );
			lPos = bands.head();
		}
	}
	return S_OK;
}
/***************************************************************************************************/


HRESULT idio::Load( IStream *pStream, SerializeParams *pparams )
{
	ULONG		cb = 0;
	long		ver = 1;
	pStream->Read( &ver, sizeof( ver ), &cb );
	pStream->Read( &chromo_no, sizeof( chromo_no ), &cb );
	pStream->Read( &relative_length, sizeof( relative_length ), &cb );
	pStream->Read( &centromere, sizeof( centromere ), &cb );
	long	c = bands.Count();
	pStream->Read( &c, sizeof( c ), &cb );


	for( ; c > 0; c-- )
	{
		band	*pband = new band;
		
		pStream->Read( pband->sz_number, sizeof( pband->sz_number ), &cb );
		pStream->Read( &pband->pos1, sizeof( pband->pos1 ), &cb );
		pStream->Read( &pband->pos2, sizeof( pband->pos2 ), &cb );
		pStream->Read( &pband->flags, sizeof( pband->flags ), &cb );
		pStream->Read( &pband->color, sizeof( pband->color ), &cb );

		_calc_level( pband->sz_number, pband );

		bands.insert( pband );
	}

	if( ver >= 2 )
		pStream->Read( &m_uFlags, sizeof( m_uFlags ), &cb );

	if( ver >= 4 )
		pStream->Read( &m_rectObject.get_rect(), sizeof( RECT ), &cb );

	if( ver >= 5 )
		__super::Load( pStream, pparams );


	return S_OK;
}

HRESULT idio::Store( IStream *pStream, SerializeParams *pparams )
{
	ULONG	cb = 0;
	long		ver = 5;
	pStream->Write( &ver, sizeof( ver ), &cb );
	pStream->Write( &chromo_no, sizeof( chromo_no ), &cb );
	pStream->Write( &relative_length, sizeof( relative_length ), &cb );
	pStream->Write( &centromere, sizeof( centromere ), &cb );
	long	c = bands.Count();
	pStream->Write( &c, sizeof( c ), &cb );


	for (TPOS lpos = bands.head(); lpos; bands.next(lpos))
	{
		band	*pband = bands.get( lpos );

		pStream->Write( pband->sz_number, sizeof( pband->sz_number ), &cb );
		pStream->Write( &pband->pos1, sizeof( pband->pos1 ), &cb );
		pStream->Write( &pband->pos2, sizeof( pband->pos2 ), &cb );
		pStream->Write( &pband->flags, sizeof( pband->flags ), &cb );
		pStream->Write( &pband->color, sizeof( pband->color ), &cb );
	}
	
	pStream->Write( &m_uFlags, sizeof( m_uFlags ), &cb );
	pStream->Write( &m_rectObject.get_rect(), sizeof( RECT ), &cb );

	__super::Store( pStream, pparams );

	return S_OK;
}

HRESULT idio::set_flags( unsigned flags )
{
	m_uFlags = flags;
	return S_OK;
}

HRESULT idio::get_flags( unsigned *flags )
{
	if( !flags )
		return S_FALSE;

	*flags = m_uFlags;

	return S_OK;
}
//	IRectObject
HRESULT idio::XRect::SetRect(RECT rc)
{
	m_rcObject = rc;
	return S_OK;
}
HRESULT idio::XRect::GetRect(RECT* prc)
{
	if( !prc )
		return S_FALSE;

	*prc = m_rcObject;

	return S_OK;
}
HRESULT idio::XRect::HitTest( POINT	point, long *plHitTest )
{
	if( !plHitTest )
		return E_INVALIDARG;

	if( point.x >= m_rcObject.left && point.x < m_rcObject.right &&
		point.y >= m_rcObject.top && point.y < m_rcObject.bottom )
			*plHitTest = 1;
	else
		*plHitTest = 0;

	return S_OK;
}

HRESULT idio::XRect::Move( POINT point )
{
	int w = m_rcObject.right - m_rcObject.left;
	int h = m_rcObject.bottom - m_rcObject.top;
	
	m_rcObject.left = point.x;
	m_rcObject.top  = point.y;

	m_rcObject.right = m_rcObject.left + w;
	m_rcObject.bottom = m_rcObject.top + h;
	return S_OK;
}

void idio::_calc_level( char *sz_number, band *pbend )
{
	if( !sz_number )
		return;
	
	if( !pbend )
		return;

	int nLen = strlen( sz_number );

	if( !nLen )
		return;

	pbend->lLevel = 1;
	for( int i = 0; i < nLen; i++ )
	{
		if( sz_number[i] == '.' )
			pbend->lLevel++;
	}

	pbend->A = sz_number[0];

	char tmp[10];
	
	i = 1;
	int nCh = 0;
	while( i < nLen && sz_number[i] != '.' )
		tmp[nCh++] = sz_number[i++];
	tmp[nCh] = 0;
	pbend->b = atoi( tmp );

	i++;
	if( i < nLen )
	{
		nCh = 0;
		while(  i < nLen && sz_number[i] != '.' )
			tmp[nCh++] = sz_number[i++];
		tmp[nCh] = 0;
		pbend->c = atoi( tmp );
	}
	else
		pbend->c = 0;

	i++;
	if( i < nLen )
	{
		nCh = 0;
		while(  i < nLen && sz_number[i] != '.' )
			tmp[nCh++] = sz_number[i++];
		tmp[nCh] = 0;
		pbend->d = atoi( tmp );
	}
	else
		pbend->d = 0;
}
