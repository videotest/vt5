#pragma once

namespace ViewSpace 
{
	template<class T>
	class CView:
		public ATL::CWindowImpl<T>,
		public WTL::CScrollImpl<CView<T> >
	{
		//MESSAGE_HANDLER(WM_SETTINGCHANGE, wnd::OnSettingChange)
		//MESSAGE_HANDLER(WM_CREATE, wnd::OnCreate)
		//MESSAGE_HANDLER(WM_PAINT, OnPaint)
		//MESSAGE_HANDLER(WM_VSCROLL, OnVScroll)
		//MESSAGE_HANDLER(WM_HSCROLL, OnHScroll)
		//MESSAGE_HANDLER(WM_MOUSEWHEEL, OnMouseWheel)
		BEGIN_MSG_MAP(CView)
			MESSAGE_HANDLER(WM_SIZE, OnSize)
			CHAIN_MSG_MAP(WTL::CScrollImpl<CView>)
		END_MSG_MAP()

		void DoPaint(WTL::CDCHandle dc)
		{
			DefWindowProc(WM_PAINT,(WPARAM)(HDC)dc,0);
		}

		void DoScroll(int nType, int nScrollCode, int& cxyOffset, int cxySizeAll,
			int cxySizePage, int cxySizeLine)
		{
			T* pT = static_cast<T*>(this); pT;  // avoid level 4 warning
			IScrollZoomSitePtr sptrZ = pT->GetControllingUnknown();
			HWND hScrollBar=0; sptrZ->GetScrollBarCtrl(nType, (HANDLE*)&hScrollBar);
			ATLASSERT(::IsWindow(hScrollBar));

			HWND hWndScrolled=*this;
			ATLASSERT(::IsWindow(hWndScrolled));
			RECT rect = { 0 };
			::GetClientRect(hWndScrolled,&rect);
			int cxyClient = (nType == SB_VERT) ? rect.bottom : rect.right;
			int cxyMax = cxySizeAll - cxyClient;

			if(cxyMax < 0)   // can't scroll, client area is bigger
				return;

			bool bUpdate = true;
			int cxyScroll = 0;

			switch(nScrollCode)
			{
			case SB_TOP:		// top or all left
				cxyScroll = cxyOffset;
				cxyOffset = 0;
				break;
			case SB_BOTTOM:		// bottom or all right
				cxyScroll = cxyOffset - cxyMax;
				cxyOffset = cxyMax;
				break;
			case SB_LINEUP:		// line up or line left
				if(cxyOffset >= cxySizeLine)
				{
					cxyScroll = cxySizeLine;
					cxyOffset -= cxySizeLine;
				}
				else
				{
					cxyScroll = cxyOffset;
					cxyOffset = 0;
				}
				break;
			case SB_LINEDOWN:	// line down or line right
				if(cxyOffset < cxyMax - cxySizeLine)
				{
					cxyScroll = -cxySizeLine;
					cxyOffset += cxySizeLine;
				}
				else
				{
					cxyScroll = cxyOffset - cxyMax;
					cxyOffset = cxyMax;
				}
				break;
			case SB_PAGEUP:		// page up or page left
				if(cxyOffset >= cxySizePage)
				{
					cxyScroll = cxySizePage;
					cxyOffset -= cxySizePage;
				}
				else
				{
					cxyScroll = cxyOffset;
					cxyOffset = 0;
				}
				break;
			case SB_PAGEDOWN:	// page down or page right
				if(cxyOffset < cxyMax - cxySizePage)
				{
					cxyScroll = -cxySizePage;
					cxyOffset += cxySizePage;
				}
				else
				{
					cxyScroll = cxyOffset - cxyMax;
					cxyOffset = cxyMax;
				}
				break;
			case SB_THUMBTRACK:
				if(IsNoThumbTracking())
					break;
				// else fall through
			case SB_THUMBPOSITION:
				{
					SCROLLINFO si = { sizeof(SCROLLINFO), SIF_TRACKPOS };
					if(::GetScrollInfo(hScrollBar, SB_CTL, &si))
					{
						cxyScroll = cxyOffset - si.nTrackPos;
						cxyOffset = si.nTrackPos;
					}
				}
				break;
			case SB_ENDSCROLL:
			default:
				bUpdate = false;
				break;
			}

			if(bUpdate && cxyScroll != 0)
			{
				::SetScrollPos(hScrollBar, SB_CTL, cxyOffset, TRUE);
				if(nType == SB_VERT)
					::ScrollWindowEx(hWndScrolled, 0, cxyScroll, 0, 0, 0, 0, m_uScrollFlags);
				else
					::ScrollWindowEx(hWndScrolled, cxyScroll, 0, 0, 0, 0, 0, m_uScrollFlags);

			}
		}

		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{

//			LRESULT lr=DefWindowProc(uMsg,wParam,lParam);

			T* pT = static_cast<T*>(this); pT;  // avoid level 4 warning
			IScrollZoomSitePtr sptrZ = pT->GetControllingUnknown();

			m_sizeClient.cx = GET_X_LPARAM(lParam);
			m_sizeClient.cy = GET_Y_LPARAM(lParam);
//			m_sizeAll=pT->_recalc_layout(m_sizeClient);

			HWND hWndScrolled=(CWnd&)*pT;
			ATLASSERT(::IsWindow(hWndScrolled));
			::SetWindowPos(hWndScrolled, NULL, 0, 0,
				m_sizeClient.cx, m_sizeClient.cy,
				SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);


//			HWND hWndSplit=pT->m_hwSplitRoot;
//			ATLASSERT(::IsWindow(hWndSplit));
//			if(0)
//			{
//				::SetWindowPos(hWndSplit, NULL, 0, 0,
//					m_sizeAll.cx, m_sizeAll.cy,
//					SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE/* |SWP_NOREDRAW*/);
////				::MoveWindow(hWndSplit,0,0,m_sizeAll.cx, m_sizeAll.cy,TRUE);
//			}
			
			// block: set horizontal scroll bar
			{
				SCROLLINFO si = { sizeof(SCROLLINFO) };
				si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
				si.nMin = 0;
				si.nMax = m_sizeAll.cx - 1;
				if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLH) != 0)
					si.fMask |= SIF_DISABLENOSCROLL;
				si.nPage = m_sizeClient.cx;
				si.nPos = m_ptOffset.x;
				StateScrollInfo(sptrZ,SB_HORZ,si);
			}

			// block: set vertical scroll bar
			{
				SCROLLINFO si = { sizeof(SCROLLINFO) };
				si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
				si.nMin = 0;
				si.nMax = m_sizeAll.cy - 1;
				if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLV) != 0)
					si.fMask |= SIF_DISABLENOSCROLL;
				si.nPage = m_sizeClient.cy;
				si.nPos = m_ptOffset.y;
				StateScrollInfo(sptrZ,SB_VERT,si);
			}

			int x = m_ptOffset.x;
			int y = m_ptOffset.y;
			if(AdjustScrollOffset(x, y))
			{
				// Children will be moved in SetScrollOffset, if needed
				//				pT->ScrollWindowEx(m_ptOffset.x - x, m_ptOffset.y - y, (m_uScrollFlags & ~SCRL_SCROLLCHILDREN));
				if(hWndScrolled)
					::ScrollWindowEx(hWndScrolled, m_ptOffset.x - x, m_ptOffset.y - y, 0, 0, 0, 0,
						(m_uScrollFlags & ~SCRL_SCROLLCHILDREN));
				SetScrollOffset(x, y, FALSE);
			}

			//			bHandled = FALSE;
			return 1;
		}

		// offset operations
		void SetScrollOffset(int x, int y, BOOL bRedraw = TRUE)
		{
			T* pT = static_cast<T*>(this); pT;  // avoid level 4 warning
			IScrollZoomSitePtr sptrZ = pT->GetControllingUnknown();

			pT->AdjustScrollOffset(x, y);

			int dx = m_ptOffset.x - x;
			int dy = m_ptOffset.y - y;
			m_ptOffset.x = x;
			m_ptOffset.y = y;

			// block: set horizontal scroll bar
			{
				SCROLLINFO si = { sizeof(SCROLLINFO) };
				si.fMask = SIF_POS;
				if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLH) != 0)
					si.fMask |= SIF_DISABLENOSCROLL;
				si.nPos = m_ptOffset.x;
				StateScrollInfo(sptrZ,SB_HORZ,si);
			}

			// block: set vertical scroll bar
			{
				SCROLLINFO si = { sizeof(SCROLLINFO) };
				si.fMask = SIF_POS;
				if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLV) != 0)
					si.fMask |= SIF_DISABLENOSCROLL;
				si.nPos = m_ptOffset.y;
				StateScrollInfo(sptrZ,SB_VERT,si);
			}

			HWND hWndScrolled=(CWnd&)*pT;
			ATLASSERT(::IsWindow(hWndScrolled));
			// Move all children if needed
			if(IsScrollingChildren() && (dx != 0 || dy != 0))
			{
				for(HWND hWndChild = ::GetWindow(hWndScrolled, GW_CHILD); hWndChild != NULL; hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT))
				{
					RECT rect = { 0 };
					::GetWindowRect(hWndChild, &rect);
					::MapWindowPoints(NULL, hWndScrolled, (LPPOINT)&rect, 1);
					::SetWindowPos(hWndChild, NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
				}
			}

			if(bRedraw)
				::InvalidateRect(hWndScrolled, NULL, TRUE);
		}

		// size operations
		void SetScrollSize(int cx, int cy, BOOL bRedraw = TRUE, bool bResetOffset = true)
		{
			T* pT = static_cast<T*>(this); pT;  // avoid level 4 warning
			ATLASSERT(::IsWindow(pT->m_hWnd));
			IScrollZoomSitePtr sptrZ = pT->Unknown();

			m_sizeAll.cx = cx;
			m_sizeAll.cy = cy;

			int x = 0;
			int y = 0;
			if(!bResetOffset)
			{
				x = m_ptOffset.x;
				y = m_ptOffset.y;
				pT->AdjustScrollOffset(x, y);
			}

			int dx = m_ptOffset.x - x;
			int dy = m_ptOffset.y - y;
			m_ptOffset.x = x;
			m_ptOffset.y = y;

			// block: set horizontal scroll bar
			{
				SCROLLINFO si = { sizeof(SCROLLINFO) };
				si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
				if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLH) != 0)
					si.fMask |= SIF_DISABLENOSCROLL;
				si.nMin = 0;
				si.nMax = m_sizeAll.cx - 1;
				si.nPage = m_sizeClient.cx;
				si.nPos = m_ptOffset.x;
				StateScrollInfo(sptrZ,SB_HORZ,si);
			}

			// block: set vertical scroll bar
			{
				SCROLLINFO si = { sizeof(SCROLLINFO) };
				si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
				if((m_dwExtendedStyle & SCRL_DISABLENOSCROLLV) != 0)
					si.fMask |= SIF_DISABLENOSCROLL;
				si.nMin = 0;
				si.nMax = m_sizeAll.cy - 1;
				si.nPage = m_sizeClient.cy;
				si.nPos = m_ptOffset.y;
				StateScrollInfo(sptrZ,SB_VERT,si);
			}

			HWND hWndScrolled=pT->hwnd();
			ATLASSERT(::IsWindow(hWndScrolled));
			// Move all children if needed
			if(IsScrollingChildren() && (dx != 0 || dy != 0))
			{
				for(HWND hWndChild = ::GetWindow(hWndScrolled, GW_CHILD); hWndChild != NULL; hWndChild = ::GetWindow(hWndChild, GW_HWNDNEXT))
				{
					RECT rect = { 0 };
					::GetWindowRect(hWndChild, &rect);
					::MapWindowPoints(NULL, pT->m_hWnd, (LPPOINT)&rect, 1);
					::SetWindowPos(hWndChild, NULL, rect.left + dx, rect.top + dy, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
				}
			}

			SetScrollLine(0, 0);
			SetScrollPage(0, 0);

			if(bRedraw)
				::InvalidateRect(hWndScrolled, NULL, TRUE);
		}

		int StateScrollInfo(IScrollZoomSite* sptrZ, int nType, SCROLLINFO& si)
		{
			HWND hScrollBar=0; sptrZ->GetScrollBarCtrl(nType, (HANDLE*)&hScrollBar);
			if(::IsWindow(hScrollBar))
			{
				bool	bDisable = (si.nMax - int(si.nPage)) <= 0;

				if( bDisable )
				{
					si.nPos = 0;
	//				int pos=::SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);
					::EnableWindow( hScrollBar, false );
	//				::SendMessage( hScrollBar, SBM_ENABLE_ARROWS, ESB_DISABLE_BOTH, 0 );
					return 0;
				}
				else
				{
					::EnableWindow( hScrollBar, true );
					::SendMessage( hScrollBar, SBM_ENABLE_ARROWS, ESB_ENABLE_BOTH, 0 );
					return ::SetScrollInfo(hScrollBar, SB_CTL, &si, TRUE);
				}
			}
			return 0;
		}
	};
}