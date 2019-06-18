#include "StdAfx.h"
#include "hotbuttom.h"
#include "resource.h"
#include "misc_utils.h"

CHotButtom::CHotButtom()
{
	::ZeroMemory( m_szScriptText, sizeof( m_szScriptText ) );
	m_hCursor = ::LoadCursor( 0, IDC_HAND );
	m_lIndex = -1;
}

CHotButtom::~CHotButtom()
{
	if( m_hHotZone )
		::DestroyIcon( m_hHotZone );
	m_hHotZone = 0;

	if( m_hCursor )
		::DeleteObject( m_hCursor );
	m_hCursor = 0;
}

bool CHotButtom::create_ex( DWORD style, const RECT &rect, const _char *pszTitle, HWND parent, HMENU hmenu, const _char *pszClass, DWORD ex_style )
{
	return __super::create_ex( style | BS_ICON, rect, pszTitle, parent, hmenu, pszClass, ex_style );
}

void CHotButtom::SetIndex( long lIndex  ) 
{
	m_lIndex = lIndex; 

	HBITMAP hBitMap = ::LoadBitmap( App::handle(), MAKEINTRESOURCE( IDB_HOTZONE) );
	BITMAP bmp = {0};
	::GetObject( hBitMap, sizeof( BITMAP ), &bmp );

	HIMAGELIST hImageList = ::ImageList_Create( bmp.bmHeight, bmp.bmHeight, ILC_COLOR24 | ILC_MASK,	 bmp.bmWidth / bmp.bmHeight, 1 );

	::ImageList_Add( hImageList, hBitMap, 0 );

	if( m_hHotZone )
		::DestroyIcon( m_hHotZone );
	m_hHotZone = 0;

	m_hHotZone = ::ImageList_GetIcon( hImageList, m_lIndex, ILD_NORMAL );
	::SendMessage( m_hwnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)m_hHotZone );

	::ImageList_Destroy( hImageList );
	::DeleteObject( hBitMap );



}

long CHotButtom::on_lbuttondown( const _point &point )
{
	char szScript[200];
	strcpy( szScript, m_szScriptText );

	::ExecuteScript( _bstr_t( szScript ), "CHotButtom::on_lbuttondown" );
	return 1L;
}

long CHotButtom::on_setcursor( unsigned code, unsigned hit )
{														   
	::SetCursor( m_hCursor );
	return 0L;
}
