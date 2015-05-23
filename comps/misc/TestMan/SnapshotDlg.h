#pragma once
#include "stdafx.h"
#include "misc_classes.h"
#include "\vt5\awin\win_dlg.h"

class CSnapshotDlg :
	public dlg_impl
{
public:
	CSnapshotDlg();
	virtual ~CSnapshotDlg();
	virtual long on_command( uint cmd );
	virtual long on_mousemove( const _point &point );
	virtual long on_lbuttondown( const _point &point );
	virtual long on_lbuttonup( const _point &point );
	virtual long on_initdialog();
	virtual long on_destroy();
	virtual long on_timer( ulong lEvent );

	virtual void on_ok();

	void ProcessNewWindow(HWND hWndNew);
	bool CheckWindowValidity (HWND hwndToCheck);
	void HighlightFoundWindow (HWND hwndFoundWindow);
	void DisplayInfoOnFoundWindow (HWND hWnd);
	void ActivateWindowNavigation(HWND hWnd);
	void RefreshWindow (HWND hwndWindowToBeRefreshed);

	bool m_bSearchingWnd;
	HWND m_hWndLastFound;
	RECT m_rc_special;
};
