//*******************************************************************************
// COPYRIGHT NOTES
// ---------------
// This source code is a part of BCGControlBar library.
// You may use, compile or redistribute it as part of your application 
// for free. You cannot redistribute it as a part of a software development 
// library without the agreement of the author. If the sources are 
// distributed along with the application, you should leave the original 
// copyright notes in the source code without any changes.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
// 
// For the latest updates to this library, check my site:
// http://welcome.to/bcgsoft
// 
// Stas Levin <bcgsoft@yahoo.com>
//*******************************************************************************

// BCGURLLinkButton.cpp : implementation file
//

#include "stdafx.h"
#include "bcgcontrolbar.h"
#include "BCGURLLinkButton.h"
#include "globals.h"
#include "bcglocalres.h"
#include "bcgbarres.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBCGURLLinkButton

CBCGURLLinkButton::CBCGURLLinkButton()
{
	//-------------------
	// Initialize colors:
	//-------------------
	m_clrRegular = ::GetSysColor (COLOR_HIGHLIGHT);

#ifdef COLOR_HOTLIGHT

	OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

	::GetVersionEx (&osvi);
	if (osvi.dwMajorVersion >= 5)
	{
		m_clrHover = ::GetSysColor (COLOR_HOTLIGHT);
	}
	else
#endif
	{
		m_clrHover = RGB (255, 0, 0);	// Light red
	}

	m_nFlatStyle = BUTTONSTYLE_NOBORDERS;
	m_sizePushOffset = CSize (0, 0);

	SetMouseCursorHand ();
}

CBCGURLLinkButton::~CBCGURLLinkButton()
{
}

BEGIN_MESSAGE_MAP(CBCGURLLinkButton, CBCGButton)
	//{{AFX_MSG_MAP(CBCGURLLinkButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBCGURLLinkButton message handlers

void CBCGURLLinkButton::OnDraw (CDC* pDC, const CRect& rect, UINT /*uiState*/)
{
	ASSERT_VALID (pDC);

	// Set font:
	CFont* pOldFont = pDC->SelectObject (&globalData.fontUnderline);
	ASSERT (pOldFont != NULL);

	// Set text parameters:
	pDC->SetTextColor (m_bHover ? m_clrHover : m_clrRegular);
	pDC->SetBkMode (TRANSPARENT);

	// Obtain label:
	CString strLabel;
	GetWindowText (strLabel);

	CRect rectText = rect;
	pDC->DrawText (strLabel, rectText, DT_SINGLELINE);

	pDC->SelectObject (pOldFont);
}
//******************************************************************************************
void CBCGURLLinkButton::OnClicked() 
{
	CWaitCursor wait;

	CString strURL = m_strURL;
	if (strURL.IsEmpty ())
	{
		GetWindowText (strURL);
	}

	if (::ShellExecute (NULL, NULL, m_strPrefix + strURL, NULL, NULL, NULL) < (HINSTANCE) 32)
	{
		TRACE(_T("Can't open URL: %s\n"), strURL);
	}

	m_bHover = FALSE;
	Invalidate ();
	UpdateWindow ();
}
//*******************************************************************************************
void CBCGURLLinkButton::SetURL (LPCTSTR lpszURL)
{
	if (lpszURL == NULL)
	{
		m_strURL.Empty ();
	}
	else
	{
		m_strURL = lpszURL;
	}
}
//*******************************************************************************************
void CBCGURLLinkButton::SetURLPrefix (LPCTSTR lpszPrefix)
{
	ASSERT (lpszPrefix != NULL);
	m_strPrefix = lpszPrefix;
}
//*******************************************************************************************
CSize CBCGURLLinkButton::SizeToContent (BOOL bVCenter, BOOL bHCenter)
{
	if (m_sizeImage != CSize (0, 0))
	{
		return CBCGButton::SizeToContent ();
	}

	ASSERT_VALID (this);
	ASSERT (GetSafeHwnd () != NULL);

	CClientDC dc (this);

	// Set font:
	CFont* pOldFont = dc.SelectObject (&globalData.fontUnderline);
	ASSERT (pOldFont != NULL);

	// Obtain label:
	CString strLabel;
	GetWindowText (strLabel);

	CRect rectClient;
	GetClientRect (rectClient);

	CRect rectText = rectClient;
	dc.DrawText (strLabel, rectText, DT_SINGLELINE | DT_CALCRECT);
	rectText.InflateRect (3, 3);

	if (bVCenter || bHCenter)
	{
		ASSERT (GetParent ()->GetSafeHwnd () != NULL);
		MapWindowPoints (GetParent (), rectClient);

		int dx = bHCenter ? (rectClient.Width () - rectText.Width ()) / 2 : 0;
		int dy = bVCenter ? (rectClient.Height () - rectText.Height ()) / 2 : 0;

		SetWindowPos (NULL, rectClient.left + dx, rectClient.top + dy, 
			rectText.Width (), rectText.Height (),
			SWP_NOZORDER | SWP_NOACTIVATE);
	}
	else
	{
		SetWindowPos (NULL, -1, -1, rectText.Width (), rectText.Height (),
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	dc.SelectObject (pOldFont);
	return rectText.Size ();
}
//*****************************************************************************
void CBCGURLLinkButton::OnDrawFocusRect (CDC* pDC, const CRect& rectClient)
{
	ASSERT_VALID (pDC);

	CRect rectFocus = rectClient;
	pDC->DrawFocusRect (rectFocus);
}
