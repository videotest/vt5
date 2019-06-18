#include "stdafx.h"
#include "Snapshot.h"
#include "math.h"
#include <stdio.h>
#include "measure5.h"
#include "alloc.h"
#include <limits.h>
#include "SnapshotDlg.h"
#include "SnapshotParamsDlg.h"
#include "docview5.h"
#include "core5.h"
#include "WindowsX.h"
#include "test_man.h"

_ainfo_base::arg CSnapshotInfo::s_pargs[] =
{
	{"ShowTargetDialog",	szArgumentTypeInt, "0", true, false },
	{"ResizeCX",	szArgumentTypeInt, "0", true, false },
	{"ResizeCY",	szArgumentTypeInt, "0", true, false },
	{"ScrollWindow",	szArgumentTypeInt, "0", true, false },
	{"WndPhoto",	szArgumentTypeString, "WndPhoto", true, false },
	{"SnapParents",	szArgumentTypeInt, "0", true, false },
	{"SnapX",	szArgumentTypeInt, "-1", true, false },
	{"SnapY",	szArgumentTypeInt, "-1", true, false },
	{0, 0, 0, false, false },
};

/////////////////////////////////////////////////////////////////////////////
CSnapshot::CSnapshot()
{
}

//---------------------------------------------------------------------------
CSnapshot::~CSnapshot()
{
}

#define round(x) int(floor((x)+0.5))

//---------------------------------------------------------------------------
HRESULT CSnapshot::DoInvoke()
{
	IUnknown *punk=::CreateTypedObject(_bstr_t(szTypeImage));
	IImage4Ptr ptrImgDst(punk);
	if(punk!=0) punk->Release();
	punk=0;
	if( ptrImgDst==0 ) return S_FALSE;

	int nShowTargetDialog = GetArgLong("ShowTargetDialog");

	if(nShowTargetDialog & 2)
	{ // показать окно ввода параметров
		CSnapshotParamsDlg dlg;
		strncpy( dlg.m_szResizeCX, GetArgString("ResizeCX"), sizeof(dlg.m_szResizeCX) );
		strncpy( dlg.m_szResizeCY, GetArgString("ResizeCY"), sizeof(dlg.m_szResizeCY) );
		dlg.m_bSelectTarget = (nShowTargetDialog & 1) != 0;
		dlg.m_bScrollWindow = GetArgLong("ScrollWindow") != 0;
		uint result = dlg.do_modal( ::GetActiveWindow() );
		if(result != IDOK) return S_FALSE;
		_variant_t var;
		var = dlg.m_szResizeCX;
		SetArgument(_bstr_t("ResizeCX"),&var);
		var = dlg.m_szResizeCY;
		SetArgument(_bstr_t("ResizeCY"),&var);
		var = long( dlg.m_bSelectTarget ? 1 : 0 );
		SetArgument(_bstr_t("ShowTargetDialog"),&var);
		var = long( dlg.m_bScrollWindow ? 1 : 0 );
		SetArgument(_bstr_t("ScrollWindow"),&var);
	}

	nShowTargetDialog = GetArgLong("ShowTargetDialog");

	bool bShowTargetDialog = (nShowTargetDialog & 1) != 0;
	int nResizeCX= GetArgLong("ResizeCX");
	int nResizeCY= GetArgLong("ResizeCY");
	bool bScrollWindow = GetArgLong("ScrollWindow") != 0;
	int nSnapParents = GetArgLong("SnapParents");
	int nSnapX = GetArgLong("SnapX");
	int nSnapY = GetArgLong("SnapY");

	int nMaxScrollsX = GetValueInt( GetAppUnknown(), "\\Snapshot", "MaxScrollsX", 10 );
	int nMaxScrollsY = GetValueInt( GetAppUnknown(), "\\Snapshot", "MaxScrollsY", 10 );
	nMaxScrollsX = max(1,nMaxScrollsX);
	nMaxScrollsY = max(1,nMaxScrollsY);

	IScrollZoomSitePtr ptrSZ(0);

	int cx=320,cy=240;
	HWND hwnd=0;
	RECT rc_wnd = {0};
	int dx = 0, dy = 0;

	if(bShowTargetDialog)
	{
		CSnapshotDlg dlg;
		uint result = dlg.do_modal( ::GetActiveWindow() );
		if(result != IDOK) return S_FALSE;

		hwnd = dlg.m_hWndLastFound;
		memcpy( &rc_wnd, &dlg.m_rc_special, sizeof( dlg.m_rc_special ) );
	}
	else if(nSnapX>=0 && nSnapY>=0)
	{
		_point screenpoint(nSnapX, nSnapY);
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
			hwnd = hFirst;
		}
		for( int n=0; n<nSnapParents; n++ )
		{
			HWND hwnd1 = GetParent( hwnd );
			if(hwnd1 && IsWindow(hwnd1)) hwnd=hwnd1;
		}
	}
	else
	{
		IApplicationPtr ptrApp( GetAppUnknown() );
		if(ptrApp==0) return S_FALSE;

		IUnknownPtr ptrunkDS;
		ptrApp->GetActiveDocument( &ptrunkDS );
		IDocumentSitePtr ptrDS(ptrunkDS);
		if(ptrDS==0) return S_FALSE;

		IUnknownPtr ptrunkView;
		ptrDS->GetActiveView( &ptrunkView );
		IViewPtr ptrView(ptrunkView);
		if(ptrView==0) return S_FALSE;

		IWindowPtr ptrWnd(ptrView);
		if(ptrWnd==0) return S_FALSE;

		HANDLE h=0;
		ptrWnd->GetHandle(&h);
		if(h==0) return S_FALSE;
		hwnd = (HWND)h;

		for( int n=0; n<nSnapParents; n++ )
		{
			HWND hwnd1 = GetParent( hwnd );
			if(hwnd1 && IsWindow(hwnd1)) hwnd=hwnd1;
		}

		ptrSZ = ptrView; // запомним - чтоб скроллить
	}

	if (!hwnd) return S_FALSE;
	if (!IsWindow(hwnd)) return S_FALSE;

	HWND hwndFrame=hwnd;
	_rect rcFrameOld;

	if(nResizeCX && nResizeCY)
	{
		while(1)
		{ // найдем окно-родителя, которое можно ресайзить
			DWORD dwStyle = GetWindowStyle(hwndFrame);
			if(dwStyle & WS_THICKFRAME) break;
			HWND hwnd1 = hwndFrame;
			hwndFrame = GetParent( hwndFrame );
			if(hwndFrame==0 || hwndFrame==hwnd1) break;
		}

		if(hwndFrame)
		{
			::GetWindowRect (hwndFrame, &rcFrameOld);
			{
				HWND hwndFrameParent = GetParent( hwndFrame );
				if(hwndFrameParent!=0)
				{
					::ScreenToClient(hwndFrameParent,(POINT*)(&rcFrameOld));
					::ScreenToClient(hwndFrameParent,((POINT*)(&rcFrameOld))+1);
					//rcFrameOld.delta(-rcFrameParent.left, -rcFrameParent.top);
				}
			}
			for(int i=0; i<20; i++)
			{
				_rect rect;
				::GetWindowRect (hwnd, &rect);
				if(rect.width()==nResizeCX && rect.height()==nResizeCY)
					break;
				_rect rect2;
				::GetWindowRect (hwndFrame, &rect2);
				MoveWindow(hwndFrame, 0,0,
					nResizeCX+rect2.width()-rect.width(),
					nResizeCY+rect2.height()-rect.height(),
					TRUE);
			}
		}
	}

	if(ptrSZ==0) bScrollWindow = false; // Для произвольных окон - не скроллим, только для наших

	_rect rect;
 	::GetWindowRect (hwnd, &rect);
	
 	if( !bScrollWindow && !::IsRectEmpty( &rc_wnd ) )
	{
		dx = labs( rect.left - rc_wnd.left );
		dy = labs( rect.top - rc_wnd.top );
		memcpy( &rect, &rc_wnd, sizeof( rect ) );
	}


	cx=rect.width(); cy=rect.height();

	_size szClient(cx,cy);

	HWND hwndSBH=0,hwndSBV=0; // скроллбары
	SCROLLINFO si_h={0}, si_v={0};
	si_h.cbSize = sizeof(SCROLLINFO);
	si_v.cbSize = sizeof(SCROLLINFO);

	if(bScrollWindow)
	{
		if(ptrSZ!=0)
		{
			ptrSZ->GetClientSize(&szClient);

			ptrSZ->GetScrollBarCtrl( SB_HORZ, (HANDLE*)&hwndSBH );
			long nStyleH = ::GetWindowLong(hwndSBH, GWL_STYLE);
			if(nStyleH & WS_DISABLED) hwndSBH = 0;
            si_h.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
			if (hwndSBH && GetScrollInfo(hwndSBH, SB_CTL, &si_h) )
				szClient.cx = (si_h.nMax-si_h.nMin) * cx / max(1,si_h.nPage);
			else
			{
				hwndSBH=0; // не нужен нам такой скроллбар, который инфу не отдает
				si_h.nPage = cx;
			}
			if(si_h.nPage==0)
			{ // убогий скроллбар
				hwndSBH=0;
				si_h.nPage = cx;
			}
			if(si_h.nMax==si_h.nMin)
			{ // убогий скроллбар
				hwndSBH=0;
				si_h.nMax = si_h.nMin+cx;
				szClient.cx = cx;
			}

			ptrSZ->GetScrollBarCtrl( SB_VERT, (HANDLE*)&hwndSBV );
			long nStyleV = ::GetWindowLong(hwndSBV, GWL_STYLE);
			if(nStyleV & WS_DISABLED) hwndSBV = 0;
            si_v.fMask = SIF_PAGE | SIF_POS | SIF_RANGE | SIF_TRACKPOS;
			if (hwndSBV && GetScrollInfo(hwndSBV, SB_CTL, &si_v) )
				szClient.cy = (si_v.nMax-si_v.nMin) * cy / max(1,si_v.nPage);
			else
			{
				hwndSBV=0; // не нужен нам такой скроллбар, который инфу не отдает
				si_v.nPage = cy;
			}
			if(si_v.nPage==0)
			{ // убогий скроллбар
				hwndSBV=0;
				si_v.nPage = cy;
			}
			if(si_v.nMax==si_v.nMin)
			{ // убогий скроллбар
				hwndSBV=0;
				si_v.nMax = si_v.nMin+cy;
				szClient.cy = cy;
			}
		}
	}

	HDC hWindowDC = ::GetWindowDC (hwnd);
	if(hWindowDC)
	{
		int w34=(cx*3+3)/4*4;

		BITMAPINFOHEADER	bmih;
		ZeroMemory(&bmih, sizeof(BITMAPINFOHEADER));
		bmih.biBitCount = 24;
		bmih.biHeight	= cy;
		bmih.biWidth	= cx;
		bmih.biSize		= sizeof(BITMAPINFOHEADER);
		bmih.biPlanes	= 1;

		//create a DIB section
		byte	*pdibBits = 0;
		HBITMAP	hDIBSection = ::CreateDIBSection(hWindowDC, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdibBits, 0, 0);

		HDC hdcMem = ::CreateCompatibleDC(hWindowDC);
		HBITMAP hBitmapOld = (HBITMAP)::SelectObject(hdcMem, hDIBSection);

		int nx = (szClient.cx+cx-1)/max(cx,1);
		int ny = (szClient.cy+cy-1)/max(cy,1);

		nx = min(nx,nMaxScrollsX);
		ny = min(ny,nMaxScrollsY);

		szClient.cx = min(szClient.cx, nx*cx);
		szClient.cy = min(szClient.cy, ny*cy);

		ptrImgDst->CreateImage(szClient.cx, szClient.cy, _bstr_t("RGB"),-1);

		if(bScrollWindow && hwndSBV) ::SendMessage( hwnd, WM_VSCROLL, SB_TOP, LPARAM(hwndSBV) );
		for(int iy=0; iy<ny; iy++)
		{
			if(bScrollWindow && hwndSBH) ::SendMessage( hwnd, WM_HSCROLL, SB_LEFT, LPARAM(hwndSBH) );
			int y = max(0,min(szClient.cy-cy,iy*cy));
			for(int ix=0; ix<nx; ix++)
			{
				int x = max(0,min(szClient.cx-cx,ix*cx));
				if(bScrollWindow)
				{
					int x1 = si_h.nMin + ix*si_h.nPage;
					int y1 = si_v.nMin + iy*si_v.nPage;
					//SetScrollPos(hwndSBV, SB_CTL, si_v.nMin + iy*si_v.nPage, TRUE);
					//SetScrollPos(hwndSBH, SB_CTL, si_h.nMin + ix*si_h.nPage, TRUE);
					if(ptrSZ) ptrSZ->SetScrollPos(_point(x1,y1));
				}
				InvalidateRect(hwnd, 0, TRUE);
				UpdateWindow(hwnd);
				
				if(!bScrollWindow)
					BitBlt(hdcMem, x,y, cx,cy, hWindowDC, dx, dy, SRCCOPY);
				else
					BitBlt(hdcMem, x,y, cx,cy, hWindowDC, 0, 0, SRCCOPY);

				int dx = min(szClient.cx, x+cx) - x;
				int dy = min(szClient.cy, y+cy) - y;
				for(int yy=0; yy<dy; yy++)
				{
					color *p0=0, *p1=0, *p2=0;
					ptrImgDst->GetRow(y+yy, 0, &p0); p0+=x;
					ptrImgDst->GetRow(y+yy, 1, &p1); p1+=x;
					ptrImgDst->GetRow(y+yy, 2, &p2); p2+=x;
					byte *pbmp = pdibBits + (cy-1-yy)*w34;
					for(int xx=0; xx<dx; xx++)
					{
						*(p2++) = (*(pbmp++))*256;
						*(p1++) = (*(pbmp++))*256;
						*(p0++) = (*(pbmp++))*256;
					}
				}

				if(bScrollWindow && hwndSBH) ::SendMessage( hwnd, WM_HSCROLL, SB_PAGERIGHT, LPARAM(hwndSBH) );
			}
			if(bScrollWindow && hwndSBV) ::SendMessage( hwnd, WM_VSCROLL, SB_PAGEDOWN, LPARAM(hwndSBV) );
		}
		GdiFlush();

		::SelectObject(hdcMem, hBitmapOld);
		::DeleteObject(hDIBSection);
		::DeleteDC( hdcMem );
		::ReleaseDC (hwnd, hWindowDC);
	}

	if(nResizeCX && nResizeCY && hwndFrame)
	{
		MoveWindow(hwndFrame,
			rcFrameOld.left, rcFrameOld.top,
			rcFrameOld.width(), rcFrameOld.height(),
			TRUE);
	}

	IUnknownPtr ptrDoc(0);

	IUnknownPtr ptrunkTM(_GetOtherComponent(GetAppUnknown(), IID_ITestManager), false);
	ITestManagerPtr ptrTM(ptrunkTM);

	if(ptrTM!=0)
	{
		IUnknownPtr ptrunk(0);
		ptrTM->GetDocument(&ptrunk);
		ptrDoc = ptrunk;
	}

	if(ptrDoc==0)
	{
		IApplicationPtr ptrApp( ::GetAppUnknown() );
		if( ptrApp )
		{
			IUnknownPtr ptrunk(0);
			ptrApp->GetActiveDocument( &ptrunk );
			ptrDoc = ptrunk;
		}
	}

	if( ptrDoc )
	{
		INamedObject2Ptr	sptrN( ptrImgDst );
		if(sptrN!=0)
		{
			_bstr_t	bstrBase = GetArgString("WndPhoto");
			if(bstrBase==_bstr_t("")) bstrBase = "WndPhoto";
			sptrN->SetName( GenerateNameForArgument(bstrBase, ptrDoc) );
		}
		long nState=0;
		ITestProcessPtr ptrProcess(ptrTM);
		if(ptrProcess!=0)
			ptrProcess->GetRunningState(&nState);
		if(nState!=2)
		{
			long nCount = ::GetValueInt( GetAppUnknown(), "\\types", "Image", 0L );
			::SetValue( GetAppUnknown(), "\\types", "Image", 0L );

			::SetValue( ptrDoc, 0, 0/*path*/, _variant_t((IUnknown*)ptrImgDst));

			::SetValue( GetAppUnknown(), "\\types", "Image", nCount );
		}
	}

	return S_OK;
}
