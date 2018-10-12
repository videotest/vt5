#include "StdAfx.h"
#include "snapshotdlg.h"
#include "resource.h"

CSnapshotDlg::CSnapshotDlg() : dlg_impl( IDD_SNAPSHOT )
{
	m_bSearchingWnd = false;
	m_hWndLastFound = 0;
	::ZeroMemory( &m_rc_special, sizeof( m_rc_special ) );
}

CSnapshotDlg::~CSnapshotDlg()
{
}

LRESULT CSnapshotDlg::on_initdialog()
{
	HWND hwnd = ::GetDlgItem(handle(), IDC_STATIC_ICON_FINDER_TOOL);
	ShowWindow(hwnd, SW_SHOW);
	hwnd = ::GetDlgItem(handle(), IDC_STATIC_ICON_FINDER_TOOL_EMPTY);
	ShowWindow(hwnd, SW_HIDE);
	return __super::on_initdialog();
}

LRESULT CSnapshotDlg::on_destroy()
{
	if (m_hWndLastFound)
	{
		RefreshWindow(m_hWndLastFound);
	}
	return __super::on_destroy();
}

LRESULT CSnapshotDlg::on_command( uint cmd )
{
	if( cmd == IDC_STATIC_ICON_FINDER_TOOL ||
		cmd == IDC_STATIC_ICON_FINDER_TOOL_EMPTY )
	{
		m_bSearchingWnd = true;

		SetCapture(handle());

		HWND hwnd = ::GetDlgItem(handle(), IDC_STATIC_ICON_FINDER_TOOL);
		ShowWindow(hwnd, SW_HIDE);
		hwnd = ::GetDlgItem(handle(), IDC_STATIC_ICON_FINDER_TOOL_EMPTY);
		ShowWindow(hwnd, SW_SHOW);

		SetCursor( ::LoadCursor( module::hrc(), MAKEINTRESOURCE(IDC_CURSOR_FINDER)) );
	}
	else if( cmd == IDC_BUTTON_PARENT_WND )
	{
		if(m_hWndLastFound)
			ProcessNewWindow( GetParent( m_hWndLastFound ) );
	}
	return __super::on_command( cmd );
}

LRESULT CSnapshotDlg::on_mousemove( const _point &point )
{
	if( m_bSearchingWnd )
	{
		POINT		screenpoint;
		HWND		hwndFoundWindow = NULL;
		char		szText[256];

		// Must use GetCursorPos() instead of calculating from "lParam".
		GetCursorPos (&screenpoint);  

		// Display global positioning in the dialog box.
		wsprintf (szText, "%d", screenpoint.x);
		SetDlgItemText(handle(), IDC_STATIC_XPOS, szText);

		wsprintf (szText, "%d", screenpoint.y);
		SetDlgItemText(handle(), IDC_STATIC_YPOS, szText);

		// Determine the window that lies underneath the mouse cursor.
		//////////////////////////////////////////////////////////////
		HWND hFirst	 = ::WindowFromPoint(screenpoint);
		if (hFirst && ::IsWindow(hFirst))
		{
			HWND hChild = 0;
			while(1)
			{
				POINT pt;
				pt.x = screenpoint.x;
				pt.y = screenpoint.y;
				::ScreenToClient(hFirst, &pt);
				HWND hChild = ::ChildWindowFromPointEx(hFirst, pt, CWP_SKIPINVISIBLE);
				if(!hChild || hChild==hFirst) break;
				hFirst = hChild;
			}
			hwndFoundWindow = hFirst;
		}

		ProcessNewWindow(hwndFoundWindow);
	}
	return __super::on_mousemove( point );
}

LRESULT CSnapshotDlg::on_lbuttondown( const _point &point )
{
	return __super::on_lbuttondown( point );
}

LRESULT CSnapshotDlg::on_lbuttonup( const _point &point )
{
	if(m_bSearchingWnd )
	{
		m_bSearchingWnd = false;
		ReleaseCapture();

		HWND hwnd = ::GetDlgItem(handle(), IDC_STATIC_ICON_FINDER_TOOL_EMPTY);
		ShowWindow(hwnd, SW_HIDE);
		hwnd = ::GetDlgItem(handle(), IDC_STATIC_ICON_FINDER_TOOL);
		ShowWindow(hwnd, SW_SHOW);

		SetCursor( ::LoadCursor( 0, MAKEINTRESOURCE(IDC_ARROW) ) );
	}

	return __super::on_lbuttonup( point );
}

#define TE_UPDATE_WND_INFO 1

LRESULT CSnapshotDlg::on_timer( ulong lEvent )
{
 	if(lEvent == TE_UPDATE_WND_INFO)
	{
		if (m_hWndLastFound )
		{
			RefreshWindow(m_hWndLastFound);
		}
	}
	return __super::on_timer( lEvent );
}

void CSnapshotDlg::ProcessNewWindow(HWND hWndNew)
{
	SetTimer(handle(), TE_UPDATE_WND_INFO, 1000, 0);	// update window info
 
	if ( !CheckWindowValidity (hWndNew) )	return;

	if (m_hWndLastFound)
	{
		RefreshWindow(m_hWndLastFound);
	}
	HighlightFoundWindow (hWndNew);
	DisplayInfoOnFoundWindow (hWndNew);
	ActivateWindowNavigation(hWndNew);
	m_hWndLastFound = hWndNew;
}

bool CSnapshotDlg::CheckWindowValidity (HWND hwndToCheck)
{
	HWND hwndTemp = NULL;
	
	if (hwndToCheck == NULL) return false;
	
	if (IsWindow(hwndToCheck) == FALSE)
		return false;
	
	// Ensure that the window is not the current one which has already been found.
	if (hwndToCheck == m_hWndLastFound)
		return false;
	
	// It also must not be the "Search Window" dialog box itself.
	if (hwndToCheck == handle())
		return false;
	
	// It also must not be one of the dialog box's children...
	hwndTemp = ::GetParent (hwndToCheck);
	if (hwndTemp == handle())
		return false;

	return true;
}

void CSnapshotDlg::HighlightFoundWindow (HWND hwndFoundWindow)
{
	HDC		hWindowDC = NULL;  // The DC of the found window.
	HGDIOBJ	hPrevPen = NULL;   // Handle of the existing pen in the DC of the found window.
	HGDIOBJ	hPrevBrush = NULL; // Handle of the existing brush in the DC of the found window.
	RECT	rect;              // Rectangle area of the found window.
	
	::GetWindowRect (hwndFoundWindow, &rect);
	hWindowDC = ::GetWindowDC (hwndFoundWindow);

	if (hWindowDC)
	{
		HPEN hRectanglePen = CreatePen (PS_SOLID, 3, RGB(255, 0, 0));
		hPrevPen = SelectObject (hWindowDC, hRectanglePen);
		hPrevBrush = SelectObject (hWindowDC, GetStockObject(HOLLOW_BRUSH));
		// Draw a rectangle in the DC covering the entire window area of the found window.

		Rectangle (hWindowDC, 0, 0, rect.right - rect.left, rect.bottom - rect.top);

		SelectObject (hWindowDC, hPrevPen);
		SelectObject (hWindowDC, hPrevBrush);
		DeleteObject(hRectanglePen);
		
		::ReleaseDC (hwndFoundWindow, hWindowDC);
	}
}
void CSnapshotDlg::DisplayInfoOnFoundWindow (HWND hWnd)
{
	char szText[1024];
	char szClassName[256];
	GetClassName(hWnd, szClassName, 250);
	_rect rect;
	GetWindowRect(hWnd, &rect);

	wsprintf (szText, 
		"Window Handle == 0x%08X.\r\n"
		"Class Name : %s.\r\n"
		"RECT.left == %d.\r\n"
		"RECT.top == %d.\r\n"
		"RECT.right == %d.\r\n"
		"RECT.bottom == %d.\r\n",
		hWnd,
		szClassName, 
		rect.left,
		rect.top,
		rect.right,
		rect.bottom
		);
	SetDlgItemText(handle(), IDC_EDIT_DETAILS, szText);

	char sz1[10] = {0};
	char sz2[10] = {0};
	char sz3[10] = {0};
	char sz4[10] = {0};

	sprintf( sz1, "%d", rect.left );
	sprintf( sz2, "%d", rect.top );
	sprintf( sz3, "%d", rect.right );
	sprintf( sz4, "%d", rect.bottom );

	SetDlgItemText( handle(), IDC_EDIT1, sz1 );
	SetDlgItemText( handle(), IDC_EDIT2, sz2 );
	SetDlgItemText( handle(), IDC_EDIT3, sz3 );
	SetDlgItemText( handle(), IDC_EDIT4, sz4 );
}

void CSnapshotDlg::ActivateWindowNavigation(HWND hWnd)
{
}

void CSnapshotDlg::RefreshWindow (HWND hwndWindowToBeRefreshed)
{
	::InvalidateRect (hwndWindowToBeRefreshed, NULL, TRUE);
	::UpdateWindow (hwndWindowToBeRefreshed);
	::RedrawWindow (hwndWindowToBeRefreshed, NULL, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_ERASENOW | RDW_UPDATENOW | RDW_ALLCHILDREN);

	HWND hParent = ::GetParent(hwndWindowToBeRefreshed);
	if (hParent)
	{
		_rect rc;
		::GetWindowRect(hwndWindowToBeRefreshed, &rc);

		_point ptTopLeft		= rc.top_left();
		
		::ScreenToClient(hParent, &ptTopLeft);
		rc.set_offset(ptTopLeft);

		::InvalidateRect (hParent, &rc, TRUE);
		::UpdateWindow (hParent);
		::RedrawWindow (hParent, &rc, NULL, RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
	}
}

void CSnapshotDlg::on_ok()
{
	char sz_text[20] = {0};

	::GetDlgItemText( handle(), IDC_EDIT1,  sz_text, sizeof( sz_text ) / sizeof( char ) );
	m_rc_special.left = atoi( sz_text );

	::GetDlgItemText( handle(), IDC_EDIT2,  sz_text, sizeof( sz_text ) / sizeof( char ) );
	m_rc_special.top = atoi( sz_text );

	::GetDlgItemText( handle(), IDC_EDIT3,  sz_text, sizeof( sz_text ) / sizeof( char ) );
	m_rc_special.right = atoi( sz_text );

	::GetDlgItemText( handle(), IDC_EDIT4,  sz_text,  sizeof( sz_text ) / sizeof( char ) );
	m_rc_special.bottom = atoi( sz_text );
	
	dlg_impl::on_ok();
}
