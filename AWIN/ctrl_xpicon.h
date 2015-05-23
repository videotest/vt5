#ifndef __ctrl_xpicon_h__
#define __ctrl_xpicon_h__

#include "misc_xpdraw.h"
#include "win_base.h"
#include "misc_drawex.h"
#include "misc_32bpp.h"

inline bool update_static_icons( HWND hdlg, HINSTANCE hinst, const _char *psz_templ, HINSTANCE hinst_icons  );

#define STM_XP_SETRESOURCE	(STM_MSGMAX+10)

class xpicon_ctrl : public win_impl
{
protected:
	xp_image	*m_xpimage;
public:
	xpicon_ctrl()
	{
		m_xpimage = 0;
	}

	~xpicon_ctrl()
	{
		_free_image();
	}

	void set_resource( HINSTANCE h, const char *res_id )
	{
		_free_image();

		if( !h || !res_id )
		{
			::SendMessage( handle(), STM_SETICON, 0, 0 );
			return;
		}

		m_xpimage = ::XPImage_Create( 32, 32, 1, 1 );
		::XPImage_AddResource( m_xpimage, h, res_id );

		{
			_rect	rect;
			::GetWindowRect( handle(), &rect );
			::MapWindowPoints( 0, ::GetParent( handle() ), (POINT*)&rect, 2 );
			::MoveWindow( handle(), rect.left, rect.top, 32, 32, false );
		}
	}
protected:
	virtual long on_paint()
	{
		_rect	rect;
		::GetClientRect( handle(), &rect );
		rect.right = max( rect.right, rect.left+32 );
		rect.bottom = max( rect.bottom, rect.top+32 );

		PAINTSTRUCT	ps;
		HDC	hdc = ::BeginPaint( handle(), &ps );

		if( !m_xpimage )
		{
			HICON	hicon = (HICON)::SendMessage( handle(), STM_GETICON, 0, 0 );
			::DrawXPIcon( hdc, hicon, &rect, DI_CENTER|DI_VCENTER );
		}
		else
		{
			::XPImage_Draw( hdc, m_xpimage, 0, &rect, DI_CENTER|DI_VCENTER );
		}

		::EndPaint( handle(), &ps );
		

		return true;
	}
	virtual long handle_message( UINT m, WPARAM w, LPARAM l )
	{
		if( m == STM_SETICON )
		{
			_free_image();

		}
		else if( m == STM_XP_SETRESOURCE )
		{
			set_resource( (HINSTANCE)w, (const char*)l );
			::InvalidateRect( handle(), 0, 0 );
			return true;
		}

		long lret = win_impl::handle_message( m, w, l );

		if( m == STM_SETICON )
		{
			_rect	rect;
			HWND	hwnd_p = ::GetParent( handle() );
			::GetWindowRect( handle(), &rect );
			::MapWindowPoints( 0, hwnd_p, (POINT*)&rect, 2 );
			::InvalidateRect( hwnd_p, &rect, true );
			::UpdateWindow( hwnd_p );
		}

		return lret;
		
	}
protected:
	void _free_image()
	{
		if( m_xpimage )XPImage_Destroy( m_xpimage );
		m_xpimage = 0;
	}

	
};

////////////////////////////////////////////////////////////////////////////////////////////
#pragma pack(push, 1)

typedef struct
{
	WORD dlgVer;
	WORD signature;
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	WORD cDlgItems;
	short x;
	short y;
	short cx;
	short cy;
} DLGTEMPLATEEX;

typedef struct
{
	DWORD helpID;
	DWORD exStyle;
	DWORD style;
	short x;
	short y;
	short cx;
	short cy;
	DWORD id;
} DLGITEMTEMPLATEEX;

#pragma pack(pop)

inline DLGITEMTEMPLATE* _first_item( DLGTEMPLATE* ptempl, bool is_extended )
{
	DWORD dwStyle = ptempl->style;

	WORD* pw;
	
	if( is_extended )
	{
		pw = (WORD*)((DLGTEMPLATEEX*)ptempl + 1);
		dwStyle = ((DLGTEMPLATEEX*)ptempl)->style;
	}
	else 
		pw = (WORD*)(ptempl + 1);

	

	if (*pw == (WORD)-1)        // Skip menu name ordinal or string
		pw += 2; // WORDs
	else
		while (*pw++);

	if (*pw == (WORD)-1)        // Skip class name ordinal or string
		pw += 2; // WORDs
	else
		while (*pw++);

	while (*pw++);              // Skip caption string

	if (dwStyle & DS_SETFONT)
	{
		pw += is_extended ? 3 : 1;;    // Skip font size, weight, (italic, charset)
		while (*pw++);              // Skip font name
	}

	// Dword-align and return
	return (DLGITEMTEMPLATE*)(((DWORD)pw + 3) & ~3);
}

inline DLGITEMTEMPLATE* _next_item_on_template( DLGITEMTEMPLATE* ptempl, bool is_extended )
{
	WORD* pw;

	if( is_extended )pw = (WORD*)((DLGITEMTEMPLATEEX*)ptempl + 1);
	else pw = (WORD*)(ptempl + 1);

	if (*pw == (WORD)-1)            // Skip class name ordinal or string
		pw += 2; // WORDs
	else
		while (*pw++);

	if (*pw == (WORD)-1)            // Skip text ordinal or string
		pw += 2; // WORDs
	else
		while (*pw++);

	WORD cbExtra = *pw++;           // Skip extra data

	// Dword-align and return
	return (DLGITEMTEMPLATE*)(((DWORD)pw + cbExtra + 3) & ~3);
}


//////////////////////////
inline bool update_static_icons( HWND hdlg, HINSTANCE hinst, const _char *psz_templ, HINSTANCE hinst_icons  )
{
	HRSRC hrsrc = ::FindResource( hinst, psz_templ, RT_DIALOG );
	HGLOBAL hrc = ::LoadResource( hinst, hrsrc );
	void	*pres = ::LockResource( hrc );
	if( !pres )return false;

	bool	is_exteneded = ((DLGTEMPLATEEX*)pres)->signature == 0xFFFF;
	DLGTEMPLATE* ptempl = (DLGTEMPLATE*)pres;

	int	item_count = is_exteneded?((DLGTEMPLATEEX*)pres)->cDlgItems:ptempl->cdit;

	HWND	hwnd = ::GetNextWindow( hdlg, GW_CHILD );

	int item;
	DLGITEMTEMPLATE* ptempl_item;

	for(ptempl_item = _first_item( ptempl, is_exteneded ),
		item = 0; 
		item < item_count; 
		item++, 
		ptempl_item = _next_item_on_template( ptempl_item, is_exteneded ) )
	{
		short	*ptr = 0;
		unsigned	style = ptempl_item->style;
		unsigned	item_id = ptempl_item->id;
		
		if( is_exteneded )
		{
			style = ((DLGITEMTEMPLATEEX*)ptempl_item)->style;
			item_id = ((DLGITEMTEMPLATEEX*)ptempl_item)->id;
			ptr = (short*)((DLGITEMTEMPLATEEX*)ptempl_item + 1);
		}
		else
			ptr = (short*)(ptempl_item + 1);
		unsigned	control_class = *(unsigned*)ptr;

		if ( *ptr == -1 )        // Skip menu name ordinal or string
			ptr+=2; // WORDs
		else
			while(*ptr++);

		unsigned	control_name = *(unsigned*)ptr;
		if ( *ptr == -1 )        // Skip class name ordinal or string
			ptr += 2; // WORDs
		else
			while(*ptr++);

		if( control_class == MAKELONG(-1, 130) )
		{
			if( (style & SS_TYPEMASK )==SS_ICON )
			{
				if( item_id != -1 && item_id != 65535 )
					hwnd = ::GetDlgItem( hdlg, item_id );

				xpicon_ctrl	*p = new xpicon_ctrl;
				p->subclass( hwnd );
				p->m_kill_on_destroy = true;
				p->set_resource( hinst_icons, MAKEINTRESOURCE(control_name>>16) );
			}
		}

		hwnd = ::GetNextWindow( hwnd, GW_HWNDNEXT );
	}

	::FreeResource( hrc );

	return true;
}



#endif //__ctrl_xpicon_h__