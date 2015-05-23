#pragma once

#include "\vt5\awin\type_defs.h"
#include "\vt5\common2\misc_classes.h"
#include "\vt5\awin\win_base.h"

class hot_btn : public win_impl
{
	HICON m_hhot_zone;
	char m_szscript_text[50];
	HCURSOR m_hcursor;
	long m_lindex;

public:
	hot_btn();
	~hot_btn();

	void set_action( LPCTSTR szname ) { strcpy( m_szscript_text, szname ); }
	void set_index( long lindex  );
	
	LPCTSTR get_action() { return m_szscript_text; }
	long get_index() { return m_lindex; }

	LRESULT on_lbuttondown(const _point &point);
	LRESULT on_setcursor(unsigned code, unsigned hit);
	bool create_ex( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0, DWORD ex_style = 0 );
};