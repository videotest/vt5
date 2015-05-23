#ifndef __misc_drawex_h__
#define __misc_drawex_h__

inline long GradientFillRect( HDC hdc, RECT *prect, COLORREF cr1, COLORREF cr2 )
{
	int	cx = prect->right-prect->left;
	int	cy = prect->bottom-prect->top;

	int	start_x = prect->left;
	int	start_y = prect->top;


	if( cx <= 0 || cy <= 0 )return false;

	int	start_r=GetRValue( cr1 );
	int	start_g=GetGValue( cr1 );
	int	start_b=GetBValue( cr1 );

	int	end_r=GetRValue( cr2 );
	int	end_g=GetGValue( cr2 );
	int	end_b=GetBValue( cr2 );


	int	cx4=(cx*3+3)/4*4;

	if( ::GetDeviceCaps( hdc,  BITSPIXEL ) > 8 )
	{
		BITMAPINFOHEADER	bi;
		ZeroMemory( &bi, sizeof( bi ) );

		bi.biSize = sizeof( bi );
		bi.biWidth = cx;
		bi.biHeight = cy;
		bi.biPlanes = 1;
		bi.biBitCount = 24;

		byte	*pdib = 0;
		HBITMAP hbmp = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdib, 0, 0 );
		if( !hbmp )return false;

		int		x, y;
		byte	*p;		
		
		for( y = 0; y < cy; y++ )
		{
			p = pdib+(cy-y-1)*cx4;

			for( x = 0; x < cx; x++ )
			{
				*p=start_b+((end_b-start_b)*(x-start_x)/cx+(end_b-start_b)*(y-start_y)/cy)/2;p++;		
				*p=start_g+((end_g-start_g)*(x-start_x)/cx+(end_g-start_g)*(y-start_y)/cy)/2;p++;		
				*p=start_r+((end_r-start_r)*(x-start_x)/cx+(end_r-start_r)*(y-start_y)/cy)/2;p++;		
			}
		}

		HDC	hdc_mem = ::CreateCompatibleDC( hdc );
		::SelectObject( hdc_mem, hbmp );
		::BitBlt( hdc, prect->left, prect->top, cx, cy, hdc_mem, 0, 0, SRCCOPY );
		::DeleteDC( hdc_mem );
		::DeleteObject( hbmp );
	}
	else
	{
		COLORREF	cr = RGB((end_r+start_r)/2, (end_g+start_g)/2, (end_b+start_b)/2 );
		HBRUSH		hbr = ::CreateSolidBrush( cr );
		::FillRect( hdc, prect, hbr );
		::DeleteObject( hbr );
	}


	

	return true;
}

#ifndef __misc_32bpp_h__
#define DI_LEFT			0x000
#define DI_RIGHT		0x001
#define DI_CENTER		0x002
#define DI_HORZ_MASK	0x003

#define DI_TOP			0x000
#define DI_BOTTOM		0x004
#define DI_VCENTER		0x008
#define DI_VERT_MASK	0x00C
#define DI_DISABLED		0x010
#define DI_HILIGHT		0x020
#endif //__misc_32bpp_h__



inline long DrawXPIcon( HDC hdc, HICON h, RECT *prect, unsigned flags )
{
	ICONINFO	ii;
	if( !::GetIconInfo( h, &ii ) )
		return false;

	BITMAP	bm;

	if( !::GetObject( ii.hbmColor, sizeof( bm ), &bm ) )
	{
		::DeleteObject( ii.hbmColor );
		::DeleteObject( ii.hbmMask );
		return false;
	}

	int	cx = bm.bmWidth;
	int	cy = bm.bmHeight;
	int x, y, cx_draw, cy_draw;

	
	if( prect->right-prect->left < cx ||
		prect->bottom-prect->top < cy )
	{
		cx_draw = cx/2;
		cy_draw = cy/2;
	}
	else
	{
		cx_draw = cx;
		cy_draw = cy;
	}

	if( (flags & DI_HORZ_MASK) == DI_LEFT )
		x = prect->left;
	else if( (flags & DI_HORZ_MASK) == DI_RIGHT )
		x = prect->right-cx_draw;
	else if( (flags & DI_HORZ_MASK) == DI_CENTER )
		x = (prect->left+prect->right-cx_draw)/2;

	if( (flags & DI_VERT_MASK) == DI_TOP )
		y = prect->top;
	else if( (flags & DI_VERT_MASK) == DI_BOTTOM )
		y = prect->bottom-cy_draw;
	else if( (flags & DI_VERT_MASK) == DI_VCENTER )
		y = (prect->top+prect->bottom-cy_draw)/2;


	
	byte				data[sizeof(BITMAPINFOHEADER)+1024];
	byte	*prgbq=data+sizeof(BITMAPINFOHEADER);
	for( int idx=0; idx < 256; idx++ )
	{
		*prgbq=idx;prgbq++;
		*prgbq=idx;prgbq++;
		*prgbq=idx;prgbq++;
		*prgbq=0;prgbq++;
	}


	HDC	hdc_src = ::CreateCompatibleDC( hdc );
	HDC	hdc_mem = ::CreateCompatibleDC( hdc );
//иконка
	BITMAPINFOHEADER	&bi=(BITMAPINFOHEADER	&)data;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biWidth = cx;
	bi.biHeight = cy;

	byte	*picon_mem = 0;
	HBITMAP hbmp_image = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&picon_mem, 0, 0 );
	::SelectObject( hdc_src, ii.hbmColor );
	::SelectObject( hdc_mem, hbmp_image );
	::BitBlt( hdc_mem, 0, 0, cx, cy, hdc_src, 0, 0, SRCCOPY );

//маска
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biPlanes = 1;
	bi.biBitCount = 8;
	bi.biWidth = cx;
	bi.biHeight = cy;

	byte	*pmask_mem = 0;
	HBITMAP hbmp_mask = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pmask_mem, 0, 0 );
	::SelectObject( hdc_src, ii.hbmMask );
	::SelectObject( hdc_mem, hbmp_mask );
	::BitBlt( hdc_mem, 0, 0, cx, cy, hdc_src, 0, 0, SRCCOPY );

	::DeleteDC( hdc_src );
	::DeleteObject( ii.hbmColor );
	::DeleteObject( ii.hbmMask );
//фон	
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biWidth = cx;
	bi.biHeight = cy;
	bi.biPlanes = 1;
	bi.biBitCount = 24;

	byte	*pdib = 0;
	HBITMAP hbmp = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdib, 0, 0 );
	::SelectObject( hdc_mem, hbmp );
	::BitBlt( hdc_mem, 0, 0, cx, cy, hdc, x, y, SRCCOPY );


	int		i, j, t;
	byte	*p, r, g, b;

	int cx4 = (cx*3+3)/4*4;
	int	cx4_icon = cx*4, cx4_mask = (cx+3)/4*4;
	byte	*picon, *pmask;

	bool	use_mask = true;

	p = picon_mem+3;
	for( j = 0; j < cy; j++ )
		for( i = 0; i < cx; i++ )
			if( *p )
			{
				use_mask = false;
				break;
			}
			else p+=4;

	for( j = 0; j < cy; j++ )
	{
		p = pdib+j*cx4;
		picon = picon_mem+j*cx4_icon;
		pmask = pmask_mem+j*cx4_mask;

		for( i = 0; i < cx; i++ )
		{
			t = picon[3];

			if( use_mask )
				t=*pmask?0:255;

			b = *picon;picon++;
			g = *picon;picon++;
			r = *picon;picon++;

			if( flags & DI_DISABLED )
				b=g=r = (r+g+b)/6+128;
			else if( flags & DI_HILIGHT )
			{
				r=max( 0, min( 255, 128+(r-128)*3/2 ) );
				g=max( 0, min( 255, 128+(g-128)*3/2 ) );
				b=max( 0, min( 255, 128+(b-128)*3/2 ) );
			}

			*p = (b*t + *p*(255-t))/256;p++;
			*p = (g*t + *p*(255-t))/256;p++;
			*p = (r*t + *p*(255-t))/256;p++;

			picon++;
			pmask++;
		}
	}



	if( cx != cx_draw || cy != cy_draw )
		StretchBlt( hdc, x, y, cx_draw, cy_draw, hdc_mem, 0, 0, cx, cy, SRCCOPY );
	else
		::BitBlt( hdc, x, y, cx, cy, hdc_mem, 0, 0, SRCCOPY );
	::DeleteDC( hdc_mem );
	::DeleteObject( hbmp );
	::DeleteObject( hbmp_image );
	::DeleteObject( hbmp_mask );

	

	return true;
}

inline long ImageList_DrawXP( HDC hdc, HIMAGELIST hil, int image, RECT *prect, unsigned flags )
{
	IMAGEINFO	ii;
	ZeroMemory( &ii, sizeof( ii ) );
	ImageList_GetImageInfo( hil, image, &ii );
	HBITMAP	hbmImage = (HBITMAP)::CopyImage( ii.hbmImage, IMAGE_BITMAP, 0, 0, LR_COLOR|LR_COPYRETURNORG|LR_CREATEDIBSECTION );
	HBITMAP	hbmMask = (HBITMAP)::CopyImage( ii.hbmMask, IMAGE_BITMAP, 0, 0, LR_MONOCHROME );

	DIBSECTION	ds_image, ds_mask;
	ZeroMemory( &ds_image, sizeof( ds_image ) );
	ZeroMemory( &ds_mask, sizeof( ds_mask ) );

	::GetObject( ii.hbmImage, sizeof( ds_image ), &ds_image );
	::GetObject( ii.hbmMask, sizeof( ds_mask ), &ds_mask );


	int	cx = ii.rcImage.right-ii.rcImage.left;
	int	cy = ii.rcImage.bottom-ii.rcImage.top;
	int x, y;

	if( (flags & DI_HORZ_MASK) == DI_LEFT )
		x = prect->left;
	else if( (flags & DI_HORZ_MASK) == DI_RIGHT )
		x = prect->right-cx;
	else if( (flags & DI_HORZ_MASK) == DI_CENTER )
		x = (prect->left+prect->right-cx)/2;

	if( (flags & DI_VERT_MASK) == DI_TOP )
		y = prect->top;
	else if( (flags & DI_VERT_MASK) == DI_BOTTOM )
		y = prect->bottom-cy;
	else if( (flags & DI_VERT_MASK) == DI_VCENTER )
		y = (prect->top+prect->bottom-cy)/2;



//создадим в памяти dc
	HDC	hdc_mem = ::CreateCompatibleDC( hdc );
	HDC	hdc_src = ::CreateCompatibleDC( hdc );
//выберем в нем DIBSECTION
	byte				*psrc_mem = 0;
	byte				data[sizeof(BITMAPINFOHEADER)+1024];
	BITMAPINFOHEADER	&bi=(BITMAPINFOHEADER	&)data;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biWidth = cx;
	bi.biHeight = cy;
	HBITMAP	hbmp_src = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&psrc_mem, 0, 0 );
	
	::SelectObject( hdc_mem, hbmp_src );
	::SelectObject( hdc_src, hbmImage );
	::BitBlt( hdc_mem, 0, 0, cx, cy, hdc_src, ii.rcImage.left, ii.rcImage.top, SRCCOPY );

//теперь в DIBSECTION у нас лежит картинка иконы

	byte	*pmask_mem = 0;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biPlanes = 1;
	bi.biBitCount = 8;
	bi.biWidth = cx;
	bi.biHeight = cy;
	byte	*prgbq=data+sizeof(BITMAPINFOHEADER);
	for( int idx=0; idx < 256; idx++ )
	{
		*prgbq=idx;prgbq++;
		*prgbq=idx;prgbq++;
		*prgbq=idx;prgbq++;
		*prgbq=0;prgbq++;
	}

	HBITMAP	hbmp_mask = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pmask_mem, 0, 0 );
	
	::SelectObject( hdc_mem, hbmp_mask );
	::SelectObject( hdc_src, hbmMask );
	::BitBlt( hdc_mem, 0, 0, cx, cy, hdc_src, ii.rcImage.left, ii.rcImage.top, SRCCOPY );


//теперь в DIBSECTION у нас лежит картинка маска
	::DeleteDC( hdc_src );
	::DeleteObject( hbmImage );
	::DeleteObject( hbmMask );


	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biWidth = cx;
	bi.biHeight = cy;
	bi.biPlanes = 1;
	bi.biBitCount = 24;

	byte	*pdib = 0;
	HBITMAP hbmp = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdib, 0, 0 );

	::SelectObject( hdc_mem, hbmp );
	::BitBlt( hdc_mem, 0, 0, cx, cy, hdc, x, y, SRCCOPY );

	int		i, j, t;
	byte	*p;
	int		r, g, b;

	int cx4 = (cx*3+3)/4*4;

	
	byte	*psrc, *pmask;
	int	cx4_icon = cx*4, cx4_mask = (cx+3)/4*4;

	//bool	use_mask = false;
	
	bool	use_mask = true;
	p = psrc_mem+3;
	for( j = 0; j < cy; j++ )
		for( i = 0; i < cx; i++ )
			if( *p )
			{
				use_mask = false;
				break;
			}
			else p+=4;

	use_mask = true;
	for( j = 0; j < cy; j++ )
	{
		p = pdib+j*cx4;
		psrc = psrc_mem+j*cx4_icon;
		pmask = pmask_mem+j*cx4_mask;

		for( i = 0; i < cx; i++ )
		{
			t = psrc[3];

			if( use_mask )t=255-*pmask;
			if( *pmask )t = 0;
			else if( use_mask )t=255;

			*pmask = psrc[3];
				

			b = *psrc;psrc++;
			g = *psrc;psrc++;
			r = *psrc;psrc++;

			if( flags & DI_DISABLED )
				b=g=r = (r+g+b)/6+128;
			else if( flags & DI_HILIGHT )
			{
				r=128+(r-128)*3/2;
				g=128+(g-128)*3/2;
				b=128+(b-128)*3/2;
			}

			*p = max( 0, min( 255, (b*t + *p*(255-t))/255));p++;
			*p = max( 0, min( 255, (g*t + *p*(255-t))/255));p++;
			*p = max( 0, min( 255, (r*t + *p*(255-t))/255));p++;

			psrc++;
			pmask++;
		}
	}


	::BitBlt( hdc, x, y, cx, cy, hdc_mem, 0, 0, SRCCOPY );
	::DeleteDC( hdc_mem );

	::DeleteObject( hbmp );
	::DeleteObject( hbmp_src );
	::DeleteObject( hbmp_mask );
	

	return true;
}

#endif //__misc_drawex_h__