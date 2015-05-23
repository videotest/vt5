#include "StdAfx.h"
#include "StatusPane.h"
#include "core5.h"
#include "statusint.h"
#include "misc_utils.h"
#include <stdio.h>
#include <commctrl.h>
#include "PaintDC.h"

// {F005B839-8FDE-4691-BEB5-E73A5F21C2B6}
static const GUID guidPaneClassifyColor = 
{ 0xf005b839, 0x8fde, 0x4691, { 0xbe, 0xb5, 0xe7, 0x3a, 0x5f, 0x21, 0xc2, 0xb6 } };


// {B8C5B4B9-5769-477F-A330-5398B305EC35}
//static const GUID guidPaneClassifyText = 
//{ 0xb8c5b4b9, 0x5769, 0x477f, { 0xa3, 0x30, 0x53, 0x98, 0xb3, 0x5, 0xec, 0x35 } };


INewStatusBarPtr StatusGetBar()
{
	IApplicationPtr	ptrApp( GetAppUnknown() );
	IUnknown	*punkStatus = 0;
	ptrApp->GetStatusBar( &punkStatus );
	if( !punkStatus )return 0;
	INewStatusBarPtr	ptrStatus( punkStatus );
	punkStatus->Release();
	return ptrStatus;
}

static char __CharByValue(double dResult)
{
	if (dResult >= 18.)
		return '4';
	else if (dResult >= 16.)
		return '3';
	else if (dResult >= 14.)
		return '2';
	else if (dResult >= 12.)
		return '1';
	else
		return '0';
}

bool SetClassifyResult(double dResult)
{
	INewStatusBarPtr ptrStatus(::StatusGetBar());
	if (ptrStatus == 0) // Not status bar
		return false;
	IWindowPtr ptrWndStat(ptrStatus); // Status bar's window handle will be useful
	HWND hwndStat;
	ptrWndStat->GetHandle((HANDLE *)&hwndStat);
	long nID;
	ptrStatus->GetPaneID(guidPaneClassifyColor, &nID);
	if (nID < 0) // No pane
	{
		CStatusPaneClassify *pPane = new CStatusPaneClassify;
		pPane->CreateWnd(hwndStat, NORECT, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, -1);
		ptrStatus->AddPane(guidPaneClassifyColor, 60, pPane->hwnd(), 0);
		ptrStatus->GetPaneID(guidPaneClassifyColor, &nID); // No will be found
	}
	char szBuff[50];
	szBuff[0] = __CharByValue(dResult);
	sprintf(szBuff+1, "%.2f", dResult);
	::SendMessage(hwndStat, SB_SETTEXT, nID, (LPARAM)szBuff);
	return true;
}



IUnknown *CStatusPaneClassify::DoGetInterface( const IID &iid )
{
	if (iid == IID_IStatusPane)
		return (IStatusPane *)this;
	else if (iid == IID_IWindow || iid == IID_IWindow2)
		return (IWindow2 *)this;
	else if (iid == IID_INamedObject2)
		return (INamedObject2 *)this;
	else
		return ComObjectBase::DoGetInterface(iid);
}

const int nMarg = 1;
const int nMarg1 = 1;
const int nWidth = 10;

void CStatusPaneClassify::DoPaint()
{
	char szBuf[20];
	::GetWindowText(hwnd(), szBuf, 20);
	COLORREF clr;
	if (szBuf[0] == '4')
		clr = RGB(0,128,0);
	else if (szBuf[0] == '3')
		clr = RGB(64,255,0);
	else if (szBuf[0] == '2')
		clr = RGB(255,255,0);
	else if (szBuf[0] == '1')
		clr = RGB(255,128,0);
	else
		clr = RGB(128,0,0);
	paint_dc dc(hwnd());
	// Draw solid background
	RECT rcClient;
	GetClientRect(hwnd(), &rcClient);
	HWND nwndParent = GetParent(hwnd());
	HBRUSH br1 = (HBRUSH)SendMessage(nwndParent, WM_CTLCOLORSTATIC, (WPARAM)dc.hdc, (LPARAM)hwnd());
	if (!br1) br1 = ::GetSysColorBrush(COLOR_3DFACE);
	::FillRect(dc.hdc, &rcClient, br1);
#if 1
/*	_brush br1(::CreateSolidBrush(::GetSysColor(COLOR_BTNFACE)));
	_rect rc(dc.ps.rcPaint.left+nMarg, dc.ps.rcPaint.top+nMarg, dc.ps.rcPaint.right-nMarg,
		dc.ps.rcPaint.bottom-nMarg);
	::FillRect(dc.hdc, &rc, br1);*/
	// Draw color square
	_brush br2(::CreateSolidBrush(clr));
	_rect rc1(rcClient.left+nMarg1, rcClient.top+nMarg1, rcClient.left+nWidth+nMarg1,
		rcClient.bottom-nMarg1);
	::FillRect(dc.hdc, &rc1, br2);
	// Now draw text
	_rect rc2(rcClient.left+nWidth+2*nMarg, rcClient.top+nMarg, rcClient.right-nMarg,
		rcClient.bottom-nMarg);
	int nMode = SetBkMode(dc.hdc,TRANSPARENT);
	HWND hwndParent = GetParent(hwnd());
	HFONT ftprev = (HFONT)::SelectObject(dc.hdc, m_ft?m_ft:(HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	::DrawText(dc.hdc, szBuf+1,-1,&rc2,DT_SINGLELINE|DT_LEFT|DT_TOP);
	::SelectObject(dc.hdc, ftprev);
	SetBkMode(dc.hdc, nMode);
#endif
}

HRESULT CStatusPaneClassify::GetWidth(int *pWidth)
{
	if(pWidth)
		*pWidth = 10;
	return S_OK;
}

HRESULT CStatusPaneClassify::GetSide(DWORD *pdwSide)
{
	if(pdwSide)
		*pdwSide = psRight;
	return S_OK;
}

LRESULT	CStatusPaneClassify::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if (nMsg == WM_SETFONT)
	{
		m_ft = (HFONT)wParam;
		return 0;
	}
	else if (nMsg == WM_DESTROY)
	{
		LRESULT lr = CWinImpl::DoMessage(nMsg,wParam,lParam);
		delete this;
		return lr;
	}
	else
		return CWinImpl::DoMessage(nMsg,wParam,lParam);
}

