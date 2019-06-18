#include "StdAfx.h"
#include "hot_btn.h"
#include "misc_utils.h"
#include "resource.h"

hot_btn::hot_btn():m_hhot_zone(0),m_hcursor(::LoadCursor(0,IDC_HAND))
{
	::ZeroMemory( m_szscript_text, sizeof( m_szscript_text ) );
	m_lindex = -1;
}

hot_btn::~hot_btn()
{
	if( m_hhot_zone )
		::DestroyIcon( m_hhot_zone );
	m_hhot_zone = 0;

	if( m_hcursor )
		::DeleteObject( m_hcursor );
	m_hcursor = 0;
}

bool hot_btn::create_ex( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass, DWORD ex_style )
{
	return __super::create_ex( style | BS_ICON, rect, pszTitle, parent, hmenu, pszClass, ex_style );
}

void hot_btn::set_index( long lindex  ) 
{
	m_lindex = lindex; 

	HBITMAP hBitMap = ::LoadBitmap( App::handle(), MAKEINTRESOURCE( IDB_HOTZONE) );
	BITMAP bmp = {0};
	::GetObject( hBitMap, sizeof( BITMAP ), &bmp );

	HIMAGELIST hImageList = ::ImageList_Create( bmp.bmHeight, bmp.bmHeight, ILC_COLOR24 | ILC_MASK,	 bmp.bmWidth / bmp.bmHeight, 1 );

	::ImageList_Add( hImageList, hBitMap, 0 );

	if( m_hhot_zone )
		::DestroyIcon( m_hhot_zone );
	m_hhot_zone = ::ImageList_GetIcon( hImageList, m_lindex, ILD_NORMAL );
	::SendMessage( m_hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hhot_zone );

	::ImageList_Destroy( hImageList );
	::DeleteObject( hBitMap );



}

long hot_btn::on_lbuttondown( const _point &point )
{
	char szScript[200];
	strcpy( szScript, m_szscript_text );

	::ExecuteScript( _bstr_t( szScript ), "CHotButtom::on_lbuttondown" );
	return 1L;
}

long hot_btn::on_setcursor( unsigned code, unsigned hit )
{														   
	::SetCursor( m_hcursor );
	return 0L;
}
