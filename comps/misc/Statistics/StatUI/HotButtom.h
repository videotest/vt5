#pragma once

#include "\vt5\awin\win_base.h"
#include <atlstr.h>

class CHotButtom : public win_impl
{
	HICON m_hHotZone;
	char m_szScriptText[50];
	HCURSOR m_hCursor;
	long m_lIndex;

public:
	CHotButtom();
	~CHotButtom();
	void SetAction( LPCTSTR szName ) { strcpy( m_szScriptText, szName ); }
	void SetIndex( long lIndex  );
	
	LPCTSTR GetAction() { return m_szScriptText; }
	long GetIndex() { return m_lIndex; }

	LRESULT on_lbuttondown(const _point &point);
	LRESULT on_setcursor(unsigned code, unsigned hit);
	bool create_ex( DWORD style, const RECT &rect, const _char *pszTitle = 0, HWND parent = 0, HMENU hmenu = 0, const _char *pszClass = 0, DWORD ex_style = 0 );
};

inline RECT MaxRect( RECT rc, RECT rc2 )
{
	RECT rc3 = { __min( rc.left, rc2.left ), __min( rc.top, rc2.top ), __max( rc.right, rc2.right ), __max( rc.bottom, rc2.bottom ) };
	return rc3;
}

inline CString _change_chars( CString str, CString strA, CString strB )
{
	CString strRet;

	long lPrev = 0;
	LPOS lPos = str.Find( strA, lPrev );

	while( lPos != -1 )
	{
		strRet += str.Mid( lPrev, lPos - lPrev ); 
		strRet +=  strB;
		lPos += strA.GetLength();
		lPrev = lPos;

		lPos = str.Find( strA, lPrev );
	}

	if( lPos == -1 )
		strRet += str.Right( str.GetLength() - lPrev );

	return strRet;
}