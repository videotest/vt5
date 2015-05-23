// EditEx.cpp : implementation file
//

#include "stdafx.h"
#include "PrintFieldAX.h"
#include "EditEx.h"

// CEditEx

IMPLEMENT_DYNAMIC(CEditEx, CEdit)
CEditEx::CEditEx( DWORD dwFlags /*= 0*/ )
{
	m_dwFlags = dwFlags;
	m_bPaste = FALSE;
	m_strPrevText.Empty();
}

CEditEx::~CEditEx()
{
}


BEGIN_MESSAGE_MAP(CEditEx, CEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_SETTEXT, OnSetText)
	ON_MESSAGE(WM_PASTE, OnPaste)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChanged)
END_MESSAGE_MAP()



// CEditEx message handlers
BOOL CEditEx::ValidateChar( UINT nChar )
{
	BOOL bValid = FALSE;
	int  nPosStart = 0, nPosEnd = 0;
	CString strText( _T("") );

	//  Get the current text in the edit control and 
	//	the current position of the cursor
    GetWindowText( strText );			 
	GetSel( nPosStart, nPosEnd );

    if ( (m_dwFlags & EDITEX_TYPE_MASKS) == EDITEX_TYPE_NUMERIC )
	{
		bValid = ( (nChar <  VK_SPACE ) ||	(nChar >= '0'   &&  nChar <= '9') );
		if( !bValid )
			if( ( (( nChar == '-' ) || ( nChar == '+' )) && ( nPosStart == 0 ) ) ||
				( ( (m_dwFlags & EDITEX_NUMERIC_TYPE_MASK) == EDITEX_NUMERIC_TYPE_DOUBLE ) &&
				( nChar == '.' ) && 
				( (-1 != strText.Mid( nPosStart, nPosEnd - nPosStart).Find( '.' ))
				|| (-1 == strText.Find( '.' )) || (nPosEnd == (strText.GetLength() - 1)) ) ) )
				bValid = TRUE;
	}
	else
		bValid = TRUE;

	return bValid;
}


void CEditEx::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if( !ValidateChar( nChar ) )
	{
		MessageBeep(0);
		return;
	}
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

LRESULT CEditEx::OnSetText( WPARAM w, LPARAM l)
{
	CString strText = (LPCTSTR) l; 
	strText.TrimLeft( _T(" .01234567890+-") );
	if( strText.IsEmpty() )
		return Default( );
	return TRUE;
}

LRESULT CEditEx::OnPaste( WPARAM, LPARAM )
{   
	GetWindowText( m_strPrevText );
	m_bPaste = TRUE;

	LRESULT lres = Default( ); 
   return lres;
}

void CEditEx::OnChanged()
{
	if( m_bPaste )
	{
			m_bPaste = FALSE;
		CString	strText;
		GetWindowText( strText );
		int nPos = strText.Find( ".", 0 );

		if( nPos != -1 )
			nPos = strText.Find( ".", nPos + 1 );

		BOOL bNeedUndo = ( nPos != -1 );
		if( !bNeedUndo )
		{
			strText.TrimLeft( _T(" .01234567890+-") );
			bNeedUndo = !strText.IsEmpty();
		}

		if( bNeedUndo )
			SetWindowText( m_strPrevText );
	}

   return;
}