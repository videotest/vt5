#include "StdAfx.h"
#include "StatusPane.h"
#include "core5.h"
#include "statusint.h"
#include "misc_utils.h"
#include <stdio.h>
#include <commctrl.h>
#include "PaintDC.h"

// {61EC9436-676B-4a3a-904D-FAE45321447B}
static const GUID guidPaneFocus = 
{ 0x61ec9436, 0x676b, 0x4a3a, { 0x90, 0x4d, 0xfa, 0xe4, 0x53, 0x21, 0x44, 0x7b } };

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

bool SetFocusIndicator(bool bAuto, int nFocus)
{
	//serg
	VTMessageBox("SetFocusIndicator", "VT5", MB_OK);
	//
	INewStatusBarPtr ptrStatus(::StatusGetBar());
	if (ptrStatus == 0) // Not status bar
		return false;
	IWindowPtr ptrWndStat(ptrStatus); // Status bar's window handle will be useful
	HWND hwndStat;
	ptrWndStat->GetHandle((HANDLE *)&hwndStat);
	long nID;
	ptrStatus->GetPaneID(guidPaneFocus, &nID);
	if (nID < 0) // No pane
	{
		CStatusPaneFocus *pPane = new CStatusPaneFocus;
		pPane->CreateWnd(hwndStat, NORECT, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, -1);
		ptrStatus->AddPane(guidPaneFocus, 60, pPane->hwnd(), 0);
		ptrStatus->GetPaneID(guidPaneFocus, &nID); // No will be found
	}
	char szBuff[50];
	_itoa(nFocus, szBuff, 10);
	::SendMessage(hwndStat, SB_SETTEXT, nID, (LPARAM)szBuff);
	return true;
}

bool RemoveFocusIndicator()
{
	INewStatusBarPtr ptrStatus(::StatusGetBar());
	if (ptrStatus == 0) // Not status bar
		return false;
	IWindowPtr ptrWndStat(ptrStatus); // Status bar's window handle will be useful
	HWND hwndStat;
	ptrWndStat->GetHandle((HANDLE *)&hwndStat);
	long nID;
	ptrStatus->GetPaneID(guidPaneFocus, &nID);
	if (nID >= 0)
		ptrStatus->RemovePane(guidPaneFocus);
	return true;
}

IUnknown *CStatusPaneFocus::DoGetInterface( const IID &iid )
{
	if (iid == IID_IStatusPane)
		return (IStatusPane *)this;
	else if (iid == IID_IWindow || iid == IID_IWindow2)
		return (IWindow2 *)this;
	else if (iid == IID_INamedObject2)
		return (INamedObject2 *)this;
	else if (iid == IID_IFocusPane)
		return (IFocusPane *)this;
	else
		return ComObjectBase::DoGetInterface(iid);
}

const int nMarg = 1;
const int nMarg1 = 1;
const int nWidth = 10;

void CStatusPaneFocus::DoPaint()
{
	char szBuf[256];
	_itoa(m_nFocus,szBuf,10);
	// Draw solid background
	paint_dc dc(hwnd());
	RECT rcClient;
	GetClientRect(hwnd(), &rcClient);
	HWND nwndParent = GetParent(hwnd());
	HBRUSH br1 = (HBRUSH)SendMessage(nwndParent, WM_CTLCOLORSTATIC, (WPARAM)dc.hdc, (LPARAM)hwnd());
	if (!br1) br1 = ::GetSysColorBrush(COLOR_3DFACE);
	::FillRect(dc.hdc, &rcClient, br1);
	// Now draw text
	_rect rc2(rcClient.left+nWidth+2*nMarg, rcClient.top+nMarg, rcClient.right-nMarg,
		rcClient.bottom-nMarg);
	int nMode = SetBkMode(dc.hdc,TRANSPARENT);
	HWND hwndParent = GetParent(hwnd());
	HFONT ftprev = (HFONT)::SelectObject(dc.hdc, m_ft?m_ft:(HFONT)::GetStockObject(DEFAULT_GUI_FONT));
	::DrawText(dc.hdc, szBuf+1,-1,&rc2,DT_SINGLELINE|DT_LEFT|DT_TOP);
	::SelectObject(dc.hdc, ftprev);
	SetBkMode(dc.hdc, nMode);
}

HRESULT CStatusPaneFocus::GetWidth(int *pWidth)
{
	if(pWidth)
		*pWidth = 10;
	return S_OK;
}

HRESULT CStatusPaneFocus::GetSide(DWORD *pdwSide)
{
	if(pdwSide)
		*pdwSide = psRight;
	return S_OK;
}

LRESULT	CStatusPaneFocus::DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam )
{
	if (nMsg == WM_CREATE)
	{
		LRESULT r = CWinImpl::DoMessage(nMsg,wParam,lParam);
		m_nTimer = SetTimer(hwnd(), 1, 1000, NULL);
		return r;
	}
	else if (nMsg == WM_SETFONT)
	{
		m_ft = (HFONT)wParam;
		return 0;
	}
	else if (nMsg == WM_TIMER)
	{
		return 0;
	}
	else if (nMsg == WM_DESTROY)
	{
		KillTimer(hwnd(), m_nTimer);
		LRESULT lr = CWinImpl::DoMessage(nMsg,wParam,lParam);
		delete this;
		return lr;
	}
	else
		return CWinImpl::DoMessage(nMsg,wParam,lParam);
}

HRESULT CStatusPaneFocus::SetFocusIndicator(bool bAuto, int nFocus)
{
	m_bAuto = bAuto;
	m_nFocus = nFocus;
	return S_OK;
}

HRESULT CStatusPaneFocus::GetFocusIndicator(bool *pbAuto, int *pnFocus)
{
	if (pbAuto) *pbAuto = m_bAuto;
	if (pnFocus) *pnFocus = m_nFocus;
	return S_OK;
}
