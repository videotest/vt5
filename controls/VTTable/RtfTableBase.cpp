// RtfTableBase.cpp : implementation file
//

#include "stdafx.h"
#include "VTTable.h"
#include "RtfTableBase.h"

#define	RICHEDIT50W_CLASS _T("RICHEDIT50W")


// CRtfTableBase

IMPLEMENT_DYNAMIC(CRtfTableBase, CRichEditCtrl)
CRtfTableBase::CRtfTableBase()
{
	m_hinstanse_ctrl = 0;

#ifndef _UNICODE
	m_bUnicodeCtrl = FALSE;
#endif

}

CRtfTableBase::~CRtfTableBase()
{
	_deinit_ctrl( );
}

BOOL	CRtfTableBase::_init_ctrl( )
{
	if( m_hinstanse_ctrl )
		return FALSE;
	
	m_hinstanse_ctrl = ::LoadLibrary( _T("MSFTEDIT.DLL") );
	return (m_hinstanse_ctrl != 0);
}

BOOL	CRtfTableBase::_deinit_ctrl( )
{
	if( !m_hinstanse_ctrl )
		return FALSE;
    
	return ::FreeLibrary( m_hinstanse_ctrl );
}


BEGIN_MESSAGE_MAP(CRtfTableBase, CRichEditCtrl)
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


BOOL	CRtfTableBase::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
    if( !_init_ctrl() )
		return __super::Create( dwStyle, rect, pParentWnd, nID );

#ifndef _UNICODE
	m_bUnicodeCtrl = TRUE;
#endif

	CWnd* pWnd = this;
	return pWnd->Create( RICHEDIT50W_CLASS /*MSFTEDIT_CLASS*/, NULL, dwStyle, rect,
		pParentWnd, nID);
}

#ifndef _UNICODE
CString CRtfTableBase::GetSelText() const
{
	if( m_bUnicodeCtrl )
	{
		CHARRANGE cr;
		cr.cpMin = cr.cpMax = 0;
		::SendMessage( GetSafeHwnd( ), EM_EXGETSEL, 0, (LPARAM)&cr);
	    LPWSTR lpwstr = (WCHAR*) _alloca( (cr.cpMax - cr.cpMin + 1) * sizeof(WCHAR) );
		lpwstr[0] = '\0';
        LONG lLen = (LONG)::SendMessage( GetSafeHwnd( ), EM_GETSELTEXT, 0, (LPARAM)lpwstr);	
		return CString( lpwstr );
	}
	else
		return __super::GetSelText( );
}
#endif

// CRtfTableBase message handlers
CString CRtfTableBase::GetRTF()
{
	// Return the RTF string of the text in the control.
	
	// Stream out here.
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = CBStreamOut;		// Set the callback

	CString sRTF = "";

	es.dwCookie = (DWORD) &sRTF;	// so sRTF receives the string
	
	StreamOut(SF_RTF, es);			// Call CRichEditCtrl::StreamOut to get the string.
	///

	return sRTF;

}

void CRtfTableBase::SetRTF(CString sRTF)
{
	// Put the RTF string sRTF into the rich edit control.

	// Read the text in
	EDITSTREAM es;
	es.dwError = 0;
	es.pfnCallback = CBStreamIn;
	es.dwCookie = (DWORD) &sRTF;
	StreamIn(SF_RTF, es);	// Do it.
	
}

/*
	Callback function to stream an RTF string into the rich edit control.
*/
DWORD CALLBACK CRtfTableBase::CBStreamIn(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// We insert the rich text here.

/*	
	This function taken from CodeGuru.com
	http://www.codeguru.com/richedit/rtf_string_streamin.shtml
	Zafir Anjum
*/

	CString *pstr = (CString *) dwCookie;

	if (pstr->GetLength() < cb)
	{
		*pcb = pstr->GetLength();
		memcpy(pbBuff, (LPCSTR) *pstr, *pcb);
		pstr->Empty();
	}
	else
	{
		*pcb = cb;
		memcpy(pbBuff, (LPCSTR) *pstr, *pcb);
		*pstr = pstr->Right(pstr->GetLength() - cb);
	}
	///

	return 0;
}

/*
	Callback function to stream the RTF string out of the rich edit control.
*/
DWORD CALLBACK CRtfTableBase::CBStreamOut(DWORD_PTR dwCookie, LPBYTE pbBuff, LONG cb, LONG *pcb)
{
	// Address of our string var is in psEntry
	CString *psEntry = (CString*) dwCookie;
	

	CString tmpEntry = "";
	tmpEntry = (CString) pbBuff;

	// And write it!!!
	*psEntry += tmpEntry.Left(cb);

	return 0;
}


void CRtfTableBase::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	Paste( );

	CRichEditCtrl::OnLButtonDblClk(nFlags, point);
}
