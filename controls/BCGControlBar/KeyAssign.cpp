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

// KeyAssign.cpp : implementation file
//

//*********************************************************
// The code is based on the Thierry Maurel's CKeyAssign:
//*********************************************************

////////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Thierry Maurel
// All rights reserved
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc., and
// I'll try to keep a version up to date.  I can be reached as follows:
//    tmaurel@caramail.com   (or tmaurel@hol.fr)
//
////////////////////////////////////////////////////////////////////////////////
// File    : KeyboardEdit.h
// Project : AccelsEditor
////////////////////////////////////////////////////////////////////////////////
// Version : 1.0                       * Authors : A.Lebatard + T.Maurel
// Date    : 17.08.98
//
// Remarks : 
//

#include "stdafx.h"
#include "KeyAssign.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Замена для некоторых стандартных функций,
// которые в либе MS реализованы с ошибкой
// (для русских букв возвращают мусор, да еще и с ассертом)
inline int __cdecl isdigit_ru(int c)
{ return isdigit(c & 255); }

inline int __cdecl isalpha_ru(int c)
{ return isalpha(c & 255); }

inline int __cdecl isalnum_ru(int c)
{ return isalnum(c & 255); }

inline int __cdecl isspace_ru(int c)
{ return isspace(c & 255); }

inline int __cdecl ispunct_ru(int c)
{ return ispunct(c & 255); }

/////////////////////////////////////////////////////////////////////////////
// CKeyAssign

CKeyAssign::CKeyAssign() :
	m_Helper (&m_Accel)
{
    m_bKeyDefined = FALSE;
	ResetKey ();
}
//******************************************************************
CKeyAssign::~CKeyAssign()
{
}

BEGIN_MESSAGE_MAP(CKeyAssign, CEdit)
	//{{AFX_MSG_MAP(CKeyAssign)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_GETDLGCODE, OnGetDlgCode)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKeyAssign message handlers

#pragma warning( disable : 4706 )

BOOL CKeyAssign::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_LBUTTONDOWN ||
		pMsg->message == WM_MBUTTONDOWN ||
		pMsg->message == WM_RBUTTONDOWN)
	{
		SetFocus ();
		return TRUE;
	}

    //andy version
	if( pMsg->message == WM_GETDLGCODE )
	{
		return DLGC_WANTALLKEYS;
	}

	if( pMsg->message == WM_KEYDOWN ||
		pMsg->message == WM_CHAR )return TRUE;
	if( pMsg->message == WM_KEYUP||
		pMsg->message == WM_SYSKEYDOWN )
	{
		if( ::isalnum_ru( pMsg->wParam ) )
		{
			ResetKey ();
			SetAccelFlag( FSHIFT, GetAsyncKeyState( VK_SHIFT )&0x8000 );
			SetAccelFlag( FCONTROL, GetAsyncKeyState( VK_CONTROL )&0x8000 );
			SetAccelFlag( FALT, GetAsyncKeyState( VK_MENU )&0x8000 );

			m_bKeyDefined = true;

			m_Accel.fVirt |= FVIRTKEY;
			m_Accel.key = (WORD) pMsg->wParam;
			CString strKbd;
			m_Helper.Format (strKbd);
			SetWindowText (strKbd);

			
			GetOwner()->SendMessage( WM_COMMAND, MAKELONG( EN_UPDATE, GetDlgCtrlID() ), (LPARAM)GetSafeHwnd() );
			return TRUE;
		}
	}

	/*BOOL bPressed;
    if ((bPressed = (pMsg->message == WM_KEYDOWN)) || pMsg->message == WM_KEYUP || (bPressed = (pMsg->message == WM_SYSKEYDOWN)) || pMsg->message == WM_SYSKEYUP) */
	/*{
        if (bPressed && m_bKeyDefined && !((1 << 30) & pMsg->lParam))
            ResetKey ();
        if (pMsg->wParam == VK_SHIFT && !m_bKeyDefined)
			SetAccelFlag (FSHIFT, bPressed);
        else if (pMsg->wParam == VK_CONTROL &&!m_bKeyDefined) {
			SetAccelFlag (FCONTROL, bPressed);
        }
        else if (pMsg->wParam == VK_MENU && !m_bKeyDefined)
			SetAccelFlag (FALT, bPressed);
        else {
            if (!m_bKeyDefined) {
				m_Accel.key = (WORD) pMsg->wParam;
                if (bPressed)
				{
                    m_bKeyDefined = TRUE;
					SetAccelFlag (FVIRTKEY, TRUE);
				}
            }
        }

		BOOL bDefaultProcess = FALSE;

		if ((m_Accel.fVirt & FCONTROL) == 0 &&
			(m_Accel.fVirt & FSHIFT) == 0 &&
			(m_Accel.fVirt & FALT) == 0 &&
			(m_Accel.fVirt & FVIRTKEY))
		{
			switch (m_Accel.key)
			{
			case VK_ESCAPE:
			case VK_TAB:
			case VK_BACK:
				bDefaultProcess = TRUE;
			}
		}

		if (!bDefaultProcess)
		{
			CString strKbd;
			m_Helper.Format (strKbd);

			SetWindowText (strKbd);
			return TRUE;
		}

		ResetKey ();
    }*/
        
    return CEdit::PreTranslateMessage(pMsg);
}

#pragma warning( default : 4706 )

//******************************************************************
void CKeyAssign::ResetKey ()
{
	memset (&m_Accel, 0, sizeof (ACCEL));
    m_bKeyDefined = FALSE;

	if (m_hWnd != NULL)
	{
		SetWindowText (_T(""));
	}
}
//******************************************************************
void CKeyAssign::SetAccelFlag (BYTE bFlag, BOOL bOn)
{
	if (bOn)
	{
		m_Accel.fVirt |= bFlag;
	}
	else
	{
		m_Accel.fVirt &= ~bFlag;
	}
}

LRESULT CKeyAssign::OnGetDlgCode( WPARAM, LPARAM )
{
	return DLGC_WANTALLKEYS;
}