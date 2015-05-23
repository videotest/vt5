#ifndef __misc_32bpp_h__
#define __misc_32bpp_h__

#ifndef __misc_drawex_h__
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
#endif //__misc_drawex_h__

///////////////////////////////////////////////////////////////////////////
//будем грузить иконки из ресурсов
typedef struct
{
   BITMAPINFOHEADER   icHeader;      // DIB header
   RGBQUAD         icColors[1];   // Color table
   BYTE            icXOR[1];      // DIB bits for XOR mask
   BYTE            icAND[1];      // DIB bits for AND mask
} ICONIMAGE, *LPICONIMAGE;

#pragma pack( push )
#pragma pack( 2 )
typedef struct
{
   BYTE   bWidth;               // Width, in pixels, of the image
   BYTE   bHeight;              // Height, in pixels, of the image
   BYTE   bColorCount;          // Number of colors in image (0 if >=8bpp)
   BYTE   bReserved;            // Reserved
   WORD   wPlanes;              // Color Planes
   WORD   wBitCount;            // Bits per pixel
   DWORD   dwBytesInRes;         // how many bytes in this resource?
   WORD   nID;                  // the ID
} GRPICONDIRENTRY, *LPGRPICONDIRENTRY;
#pragma pack( pop )

#pragma pack( push )
#pragma pack( 2 )
typedef struct 
{
   WORD            idReserved;   // Reserved (must be 0)
   WORD            idType;       // Resource type (1 for icons)
   WORD            idCount;      // How many images?
   GRPICONDIRENTRY   idEntries[1]; // The entries for each image
} GRPICONDIR, *LPGRPICONDIR;
#pragma pack( pop )


inline bool is_true_color()
{
	static int screen_bit_count = -1;

	if( screen_bit_count == -1 )
	{
		HDC	hdc = ::GetDC( 0 );
		screen_bit_count = ::GetDeviceCaps( hdc,  BITSPIXEL );
		::ReleaseDC( 0, hdc );
	}

	return screen_bit_count > 8;
}


struct xp_image
{
	int		m_cx, m_cy, m_count, m_inc, m_allocated;
	byte	*m_prgb;

	xp_image();
	void realloc_image( int size );
	long	get_image_data_size()	{return m_cx*m_cy*4;}
};

inline xp_image::xp_image()
{
	m_cx = m_cy = 0; 
	m_prgb = 0;
	m_count = 0;
	m_inc = 4;
	m_allocated = 0;
}

inline void xp_image::realloc_image( int size )
{
	if( m_allocated >= size )
		return;

	m_allocated = (size+m_inc-1)/m_inc*m_inc;
	
	int		cb_old = m_cx*m_cy*m_count*4;
	int		cb_new = m_cx*m_cy*m_allocated*4;

	byte	*pold = m_prgb;
	m_prgb = (byte*)::malloc( m_cx*m_cy*m_allocated*4 );

	int		cb_copy = min( cb_old, cb_new );

	if( cb_copy && pold && m_prgb )
		memcpy( m_prgb, pold, cb_copy );
	
	::free( pold );
}


inline void XPImage_Destroy( xp_image *pimage )
{
	::free( pimage->m_prgb );
	delete pimage;
}

inline xp_image *XPImage_Create( int cx, int cy, int count, int glow )
{
	xp_image *pxp = new xp_image;
	pxp->m_cx = cx;
	pxp->m_cy = cy;
	pxp->m_inc = glow;

	pxp->realloc_image( count );

	return pxp;
}

inline xp_image *XPImage_Load( HMODULE hmodule, const _char *psz_res, int cx, int cy )
{
	HRSRC	hrsrc = ::FindResource( hmodule, psz_res, RT_GROUP_ICON );

	if( !hrsrc )return 0;
	HANDLE	hdata = ::LoadResource( hmodule, hrsrc );
	if( hdata == INVALID_HANDLE_VALUE )return 0;

	void *ptr = ::LockResource( hdata );

	GRPICONDIR	*pdir = (GRPICONDIR*)ptr;

	int	max_color  = 0;
	unsigned	max_id = 0;
//ищем иконку подходящего размера и цвета
	for( int i = 0; i < pdir->idCount; i++ )
	{
		GRPICONDIRENTRY	*pgrp = pdir->idEntries+i;

		if( (cx && pgrp->bWidth != cx) || (cy && pgrp->bHeight != cy) )
			continue;

	
		if( pgrp->wBitCount > max_color )
		{
			max_color = pgrp->wBitCount;
			max_id = pgrp->nID;
			cx = pgrp->bWidth;
			cy = pgrp->bHeight;
		}
	}

	hrsrc = ::FindResource( hmodule, MAKEINTRESOURCE(max_id), RT_ICON );
	if( !hrsrc )return 0;
	hdata = ::LoadResource( hmodule, hrsrc );
	if( hdata == INVALID_HANDLE_VALUE )return 0;

	ptr = ::LockResource( hdata );
	ICONIMAGE *pii = (ICONIMAGE *)ptr;

	xp_image	*pimage = new xp_image;
	pimage->m_cx = cx;
	pimage->m_cy = cy;
	pimage->realloc_image( 1 );
	
	memcpy( pimage->m_prgb, pii->icColors, pimage->get_image_data_size() );

	return pimage;
}

inline int XPImage_AddIcon( xp_image *pxp, HICON h )
{
	ICONINFO	ii;
	::GetIconInfo( h, &ii );

	byte	data[sizeof(BITMAPINFOHEADER)+1024];
	BITMAPINFOHEADER	&bi = (BITMAPINFOHEADER&)data;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biWidth = pxp->m_cx;
	bi.biHeight = pxp->m_cy;
	bi.biPlanes = 1;
	bi.biBitCount = 32;

	BITMAP	bm_ico;
	::GetObject( ii.hbmColor, sizeof( bm_ico ), &bm_ico );

	byte	*pdata = 0;
	HDC	hdc = ::GetDC( 0 );
	HDC	hdc_mem = ::CreateCompatibleDC( hdc );
	HDC	hdc_src = ::CreateCompatibleDC( hdc );
	HBITMAP	hbmp = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdata, 0, 0 );
	

	::SelectObject( hdc_src, ii.hbmColor );
	::SelectObject( hdc_mem, hbmp );

	if( bm_ico.bmWidth != pxp->m_cx || bm_ico.bmHeight != pxp->m_cy )
		::StretchBlt( hdc_mem, 0, 0, pxp->m_cx, pxp->m_cy, hdc_src, 0, 0, bm_ico.bmWidth, bm_ico.bmHeight, SRCCOPY );
	else
		::BitBlt( hdc_mem, 0, 0, pxp->m_cx, pxp->m_cy, hdc_src, 0, 0, SRCCOPY );



	int	index = pxp->m_count;
	pxp->realloc_image( index+1 );
	int	cb = pxp->get_image_data_size();
	pxp->m_count++;

	memcpy( pxp->m_prgb+index*cb, pdata, cb );

	bool	need_mask = true;
	int		i, j;

	if( is_true_color() )
	{
		for( i = 3; i < cb; i+=4 )
		{
			if( pdata[i] )
			{
				need_mask = false;
				break;
			}
		}
	}
	else
	{
		for( i = 3; i < cb; i+=4 )
			pdata[i] = 255;
	}

	HBITMAP	hmask = 0;
	if( need_mask )
	{
		byte	*p, *ps;
		int	cx4 = (pxp->m_cx+3)/4*4;

		for( p = data+bi.biSize, i = 0; i < 256; i++ )
		{
			*p = i;p++;
			*p = i;p++;
			*p = i;p++;
			*p = 0;p++;
		}
		bi.biBitCount = 8;
		byte	*pmask = 0;
		hmask = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pmask, 0, 0 );
		::SelectObject( hdc_src, ii.hbmMask );
		::SelectObject( hdc_mem, hmask );
		
		if( bm_ico.bmWidth != pxp->m_cx || bm_ico.bmHeight != pxp->m_cy )
			::StretchBlt( hdc_mem, 0, 0, pxp->m_cx, pxp->m_cy, hdc_src, 0, 0, bm_ico.bmWidth, bm_ico.bmHeight, SRCCOPY );
		else
			::BitBlt( hdc_mem, 0, 0, pxp->m_cx, pxp->m_cy, hdc_src, 0, 0, SRCCOPY );

		
		

		for( j = 0; j < pxp->m_cy; j++ )
			for( ps = pmask+j*cx4,
				p = pxp->m_prgb+index*cb+j*pxp->m_cx*4+3,
				i = 0; 
				i < pxp->m_cx; i++, p+=4, ps++ )

				*p = 255-*ps;
	}
	
	::DeleteDC( hdc_mem );
	::DeleteDC( hdc_src );
	::ReleaseDC( 0, hdc );
	
	::DeleteObject( ii.hbmColor );
	::DeleteObject( ii.hbmMask );
	::DeleteObject( hbmp );
	if( hmask )::DeleteObject( hmask );
	

	return index;
}


inline long XPImage_AddResource( xp_image *pxp_image, HMODULE hmodule, const _char *psz_res )
{
	HRSRC	hrsrc = ::FindResource( hmodule, psz_res, RT_GROUP_ICON );

	if( !hrsrc )return 0;
	HANDLE	hdata = ::LoadResource( hmodule, hrsrc );
	if( hdata == INVALID_HANDLE_VALUE )return 0;

	void *ptr = ::LockResource( hdata );

	GRPICONDIR	*pdir = (GRPICONDIR*)ptr;

	int	cx = pxp_image->m_cx;
	int	cy = pxp_image->m_cy;

//ищем иконку подходящего размера и цвета
	if( !pdir->idCount )return -1;

	GRPICONDIRENTRY	*pgrp_f = pdir->idEntries;

	for( int i = 0; i < pdir->idCount; i++ )
	{
		GRPICONDIRENTRY	*pgrp = pdir->idEntries+i;

		if( (cx && pgrp->bWidth != cx) || (cy && pgrp->bHeight != cy) )
			continue;

	
		if( pgrp->wBitCount >= pgrp_f->wBitCount )
			pgrp_f = pgrp;
	}


	hrsrc = ::FindResource( hmodule, MAKEINTRESOURCE(pgrp_f->nID), RT_ICON );
	if( !hrsrc )return -1;
	hdata = ::LoadResource( hmodule, hrsrc );
	if( hdata == INVALID_HANDLE_VALUE )return -1;

	ptr = ::LockResource( hdata );
	ICONIMAGE *pii = (ICONIMAGE *)ptr;

	if( pgrp_f->wBitCount == 32 && 
		pxp_image->m_cx == pii->icHeader.biWidth &&
		pxp_image->m_cy == pii->icHeader.biHeight/2 )
	{
		int	index = pxp_image->m_count;
		pxp_image->realloc_image( index+1 );
		int	cb = pxp_image->get_image_data_size();
		pxp_image->m_count++;
		memcpy( pxp_image->m_prgb+index*cb, pii->icColors, cb );

		return index;
	}
	else
	{
		HICON	h = CreateIconFromResourceEx( (byte*)pii, pgrp_f->dwBytesInRes, true, 0x00030000,  pxp_image->m_cx, pxp_image->m_cy, 0 );
		if( !h )return -1;

		int	nret = XPImage_AddIcon( pxp_image, h );

		::DestroyIcon( h );

		return nret;
	}
}

inline int	XPImage_GetSize( xp_image *pxp, int *pcx, int *pcy )
{
	*pcx = pxp->m_cx;
	*pcy = pxp->m_cy;
}



inline void XPImage_Draw( HDC hdc, xp_image *pxp_image, int image, RECT *prect, unsigned flags )
{
	int x, y, cx_draw, cy_draw;
	int	cx = pxp_image->m_cx;
	int	cy = pxp_image->m_cy;
	int	cx4=(cx*3+3)/4*4;
	
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


	BITMAPINFOHEADER	bi;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biWidth = cx;
	bi.biHeight = cy;
	bi.biBitCount = 24;
	bi.biPlanes = 1;

	byte	*pdib_screen;
	HBITMAP	hbmp_screen = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdib_screen, 0, 0 );
	HDC	hdc_mem = ::CreateCompatibleDC( hdc );
	::SelectObject( hdc_mem, hbmp_screen );
	::BitBlt( hdc_mem, 0, 0, cx, cy, hdc, x, y, SRCCOPY );

	int	i, j, t, r, g, b;

	for( j = 0; j < cy; j++ )
	{
		byte	*pscreen = pdib_screen+cx4*j; 
		byte	*pimage = pxp_image->m_prgb+pxp_image->get_image_data_size()*image+pxp_image->m_cx*j*4;

		for( i = 0; i < cx; i++ )
		{
			b = *pimage;pimage++;
			g = *pimage;pimage++;
			r = *pimage;pimage++;
			t = *pimage;pimage++;

			if( flags & DI_DISABLED )
				b=g=r = (r+g+b)/6+128;
			else if( flags & DI_HILIGHT )
			{
				r=128+(r-128)*3/2;
				g=128+(g-128)*3/2;
				b=128+(b-128)*3/2;
			}

			*pscreen = max( 0, min( 255, (b*t+*pscreen*(255-t))/255));pscreen++;
			*pscreen = max( 0, min( 255, (g*t+*pscreen*(255-t))/255));pscreen++;
			*pscreen = max( 0, min( 255, (r*t+*pscreen*(255-t))/255));pscreen++;
		}
	}

	if( cx != cx_draw || cy != cy_draw )
		StretchBlt( hdc, x, y, cx_draw, cy_draw, hdc_mem, 0, 0, cx, cy, SRCCOPY );
	else
		::BitBlt( hdc, x, y, cx, cy, hdc_mem, 0, 0, SRCCOPY );


	::DeleteDC( hdc_mem );
	::DeleteObject( hbmp_screen );
}


inline HICON XPLoadIcon( HMODULE hmodule, const char *psz_res, int cx, int cy, int nBitCount )
{
	if( !is_true_color() )
		return (HICON)::LoadImage( hmodule, psz_res, IMAGE_ICON, cx, cy, 0 );
	
	if( !cx )cx = 32;
	if( !cy )cy = 32;

	HRSRC	hrsrc = ::FindResource( hmodule, psz_res, RT_GROUP_ICON );

	if( !hrsrc )return 0;
	HANDLE	hdata = ::LoadResource( hmodule, hrsrc );
	if( hdata == INVALID_HANDLE_VALUE )return 0;
	void *ptr = ::LockResource( hdata );

	GRPICONDIR	*pdir = (GRPICONDIR*)ptr;


//ищем иконку подходящего размера и цвета
	if( !pdir->idCount )return 0;

	GRPICONDIRENTRY	*pgrp_f = pdir->idEntries;

	for( int i = 0; i < pdir->idCount; i++ )
	{
		GRPICONDIRENTRY	*pgrp = pdir->idEntries+i;

		if( pgrp->bWidth != cx || pgrp->bHeight != cy )
			continue;

		if( pgrp->wBitCount >= pgrp_f->wBitCount )
			pgrp_f = pgrp;
	}

	hrsrc = ::FindResource( hmodule, MAKEINTRESOURCE(pgrp_f->nID), RT_ICON );
	if( !hrsrc )return 0;
	hdata = ::LoadResource( hmodule, hrsrc );
	if( hdata == INVALID_HANDLE_VALUE )return 0;

	ptr = ::LockResource( hdata );
	ICONIMAGE *pii = (ICONIMAGE *)ptr;


	if( pii->icHeader.biBitCount != 32 )
		return CreateIconFromResourceEx( (byte*)ptr, pgrp_f->dwBytesInRes, true, 0x00030000, cx, cy, 0 );


	ICONINFO	ii;
	ZeroMemory( &ii, sizeof( ii ) );
	ii.fIcon = true;

	HDC	hdc = ::GetDC( 0 );


	byte	data[sizeof(BITMAPINFOHEADER)+1024];
	BITMAPINFOHEADER	&bi = (BITMAPINFOHEADER	&)data;
	ZeroMemory( &bi, sizeof( bi ) );
	bi.biSize = sizeof( bi );
	bi.biWidth = cx;
	bi.biHeight = cy;
	bi.biBitCount = 32;
	bi.biPlanes = 1;
	byte	*pdata = 0;
	
	ii.hbmColor = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdata, 0, 0 );
	int	cx4=(cx*bi.biBitCount/8+3)/4*4;
	int	cb = cx4*cy;
	memcpy( pdata, pii->icColors, cb );


	//внимание! под windows 2000 в pii->icAND какая-то фигня.
	//поэтому восстанавливаем прозрачность исходя из a-канала
	bool	fill_alpha = true;
	for( i = 3; i < cb; i+=4 )
		if( pdata[i] )
		{
			fill_alpha = false;
			break;
		}

	int	cb_pal = 8;

	{
		//у нас битмап с палитрой
		byte	*ppal_dest = data+bi.biSize;
		*ppal_dest = 0;ppal_dest++;
		*ppal_dest = 0;ppal_dest++;
		*ppal_dest = 0;ppal_dest++;
		*ppal_dest = 0;ppal_dest++;
		*ppal_dest = 255;ppal_dest++;
		*ppal_dest = 255;ppal_dest++;
		*ppal_dest = 255;ppal_dest++;
		*ppal_dest = 0;ppal_dest++;
	}

	bi.biBitCount = 1;
	
	byte	*pdata_mask = 0;
	ii.hbmMask = ::CreateDIBSection( hdc, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void**)&pdata_mask, 0, 0 );
	
	cx4=(cx*bi.biBitCount/8+3)/4*4;
	cb = cx4*cy;

	ZeroMemory( pdata_mask, cb );

	if( !fill_alpha )
	{
		int	x, y, byte_no, bit_no;
		byte	*p, *pi;

		for( y = 0; y < cy; y++ )
		{
			p = pdata_mask+y*cx4;
			pi = pdata+y*cx*4+3;


			for( x = 0; x < cx; x++, pi+=4)
			{
				byte_no = x/8;
				bit_no = x%8;

				if( !*pi )
					p[byte_no] |= (1<<(7-bit_no) );
			}
		}
	}
	else
		memcpy( pdata_mask, pii->icAND, cb );

	HICON	h = ::CreateIconIndirect( &ii ); 
	::DeleteObject( ii.hbmColor );
	::DeleteObject( ii.hbmMask );
	::ReleaseDC( 0, hdc );

	return h;
}

inline HIMAGELIST	XPImage_DummyList( bool is_small )
{
	static	HIMAGELIST	hil_small = 0;
	static	HIMAGELIST	hil_large = 0;

	if( is_small )
	{
		if( !hil_small )hil_small = ImageList_Create( 16, 16, 0, 1, 1 );
		return hil_small;
	}
	else 
	{
		if( !hil_large )hil_large = ImageList_Create( 32, 32, 0, 1, 1 );
		return hil_large;
	}

}



typedef	xp_image*	XPIMAGE;
typedef xp_image*	XPIMAGELIST;

/////////////////////////////////////////////////////////////////////////////
//set the XPImageList
//wParam is a new XPlisy
//lParam - deletion flag, if 1, window havel to destroy image list when it unnessesф
#define WM_SETXPIMAGELIST	(WM_USER+100)
//return value is XPIMAGELIST associated with specified window

//get the XPImageList
//wParam = 0
//lParam = 0
#define WM_GETXPIMAGELIST	(WM_USER+101)



#endif //__misc_32bpp_h__