// ChoiseGkSmbBase.cpp : implementation file
//

#include "stdafx.h"
#include "MeasCtrls.h"
#include "ChoiseGkSmbBase.h"


// CChoiseGkSmbBase

IMPLEMENT_DYNAMIC(CChoiseGkSmbBase, CComboBox)
CChoiseGkSmbBase::CChoiseGkSmbBase()
{
}

CChoiseGkSmbBase::~CChoiseGkSmbBase()
{
}


BEGIN_MESSAGE_MAP(CChoiseGkSmbBase, CComboBox)
	//ON_MESSAGE(WM_SETFONT, OnSetFont)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()



// CChoiseGkSmbBase message handlers
/*LRESULT CChoiseGkSmbBase::OnSetFont(WPARAM wParam, LPARAM)
{
	LRESULT res = Default();
    
	CRect rc;
	GetWindowRect( &rc );

	WINDOWPOS wp = {0};
	wp.hwnd = GetSafeHwnd( );
	wp.cx = rc.Width();
	wp.cy = rc.Height();
	wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );

	return res;
} */ 

void CChoiseGkSmbBase::MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	return;
	CFont *pFont = GetFont();
	if( !pFont )
		return;

	LOGFONT lf = {0};
	pFont->GetLogFont( &lf );

	if ( lf.lfHeight < 0 )
		lpMeasureItemStruct->itemHeight = -lf.lfHeight;// * 72 / GetDeviceCaps( ::GetDC( GetSafeHwnd() ), LOGPIXELSY); 
	else
		lpMeasureItemStruct->itemHeight = lf.lfHeight;// * 72 / GetDeviceCaps( ::GetDC( GetSafeHwnd() ), LOGPIXELSY);

	lf.lfHeight = 350;
	  
	//TEXTMETRIC tm;
	//font.QueryTextMetrics( &tm );
    //lpMeasureItemStruct->itemHeight = tm.tmHeight + tm.tmExternalLeading + 1;
}

void CChoiseGkSmbBase::DrawItem(  LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CFont *pFont = GetFont();
	if( !pFont )
		return;

	LOGFONT lf = {0};
	pFont->GetLogFont( &lf );

	lf.lfCharSet = m_btCharSet;
	HFONT hTempFont = ::CreateFontIndirect( &lf );
	if( !hTempFont )
		return;

	hTempFont = (HFONT) ::SelectObject( lpDrawItemStruct->hDC, hTempFont );
	char *str = new char[2];
	str[1] = '\0';
	str[0] = (char)((long)lpDrawItemStruct->itemID + m_lFirstChar);
	::DrawText( lpDrawItemStruct->hDC, str, strlen( str ), &lpDrawItemStruct->rcItem, DT_LEFT );
	delete[] str;
	str = 0;
	::DeleteObject( ::SelectObject( lpDrawItemStruct->hDC, hTempFont ) );
	hTempFont = 0;
}   

