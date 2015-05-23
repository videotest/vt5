#include "stdafx.h"
#include "statusutils.h"
#include "core5.h"
#include "utils.h"
#include "window5.h"

std_dll INewStatusBar *StatusGetBar()
{
	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkStatus = 0;
	ptrApp->GetStatusBar( &punkStatus );
	if( !punkStatus )return 0;
	INewStatusBarPtr	ptrStatus( punkStatus );
	punkStatus->Release();
	return ptrStatus.Detach();
}

std_dll HWND GetWindow( IUnknown *punk )
{
	IWindowPtr	ptrWindow( punk );
	if( ptrWindow == 0 )return 0;
	HWND	hwnd = 0;
	ptrWindow->GetHandle( (void**)&hwnd );
	return hwnd;
}

std_dll void StatusSetPaneIcon( const GUID guidPane, HICON hIcon )
{
	INewStatusBarPtr	ptrStatus( StatusGetBar(), false );
	if( ptrStatus == 0  )return;
	ptrStatus->SetPaneIcon( guidPane, hIcon );
}

std_dll void StatusSetPaneText( const GUID guidPane, const char *pszFormat, ... )
{
	INewStatusBarPtr	ptrStatus( StatusGetBar(), false );
	HWND	hwndStatus = ::GetWindow( ptrStatus );
	if( ptrStatus == 0 ||hwndStatus == 0 )return;
	long	nItem = -1;
	ptrStatus->GetPaneID( guidPane, &nItem );
	if( nItem == -1 )return;

	TCHAR szBuffer[512];

	if( pszFormat )
	{
		va_list args;
		va_start(args, pszFormat);

		_vsntprintf(szBuffer, sizeof(szBuffer), pszFormat, args);
			
		va_end(args);
	}
	else
		szBuffer[0] = 0;

	::SendMessage( hwndStatus, SB_SETTEXT, nItem, (LPARAM)szBuffer );
}

std_dll int StatusCalcWidth( const char *pszMaxText, bool bHaveIcon )
{
	INewStatusBarPtr	ptrStatus( StatusGetBar(), false );
	HWND	hwndStatus = ::GetWindow( ptrStatus );
	if( ptrStatus == 0 ||hwndStatus == 0 )return 0;

	RECT	rect;
	ZeroMemory( &rect, sizeof( rect ) );

	HDC	hdc = ::GetDC( hwndStatus );
	::DrawText( hdc, pszMaxText, strlen( pszMaxText ), &rect, DT_CALCRECT );
	::ReleaseDC( hwndStatus, hdc );

	if( bHaveIcon )rect.right += 20;

	return rect.right-rect.left;
}