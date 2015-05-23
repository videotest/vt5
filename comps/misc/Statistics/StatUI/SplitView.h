#pragma once

namespace ViewSpace 
{
	template <class T>
	class CCalcMinRect{
		BEGIN_MSG_MAP(CCalcMinRect)
			MESSAGE_HANDLER(WM_USER_MIN_SIZE, OnMinSize)
		END_MSG_MAP()
		LRESULT OnMinSize(UINT /*uMsg*/, WPARAM wParam, LPARAM /*lParam*/, BOOL& /*bHandled*/)
		{
			T* pT = static_cast<T*>(this); pT;  // avoid level 4 warning
			SIZE size=pT->calc_min_rect((BOOL)wParam);
			return MAKELRESULT(size.cx,size.cy);
		}
	};

	template <class T, bool t_bVertical = true>
	class CSplitterLtdWindowT : 
		public WTL::CSplitterWindowImpl<CSplitterLtdWindowT<T, t_bVertical> >,
		public CCalcMinRect<T>
	{
		typedef WTL::CSplitterWindowImpl<CSplitterLtdWindowT<T, t_bVertical> > CSplitWinImpl;
		SIZE _sizeMin[2];
	public:
		DECLARE_WND_CLASS_EX(_T("SplitterLtdWindow"), CS_DBLCLKS, COLOR_WINDOW)
		BEGIN_MSG_MAP(CSplitterLtdWindowT)
			MESSAGE_HANDLER(WM_USER_MIN_SIZE, CCalcMinRect<T>::OnMinSize)
			MESSAGE_HANDLER(WM_MOUSEMOVE, OnMouseMove)
			CHAIN_MSG_MAP(CSplitWinImpl)
		END_MSG_MAP()

		SIZE calc_min_rect(BOOL bPrintMode){
			T* pT = static_cast<T*>(this); pT;  // avoid level 4 warning

//			SIZE size[2];
			LRESULT lr=SendMessage(m_hWndPane[0],WM_USER_MIN_SIZE,bPrintMode,0);
			_sizeMin[0].cx=LOWORD(lr); _sizeMin[0].cy=HIWORD(lr); 
			lr=SendMessage(m_hWndPane[1],WM_USER_MIN_SIZE,bPrintMode,0);
			_sizeMin[1].cx=LOWORD(lr); _sizeMin[1].cy=HIWORD(lr); 

			SIZE sizeMin;
			if(t_bVertical){
				sizeMin.cx = _sizeMin[0].cx + _sizeMin[1].cx + m_cxySplitBar + m_cxyBarEdge;
				sizeMin.cy = __max(_sizeMin[0].cy, _sizeMin[1].cy);
			}else{
				sizeMin.cx = __max(_sizeMin[0].cx, _sizeMin[1].cx);
				sizeMin.cy = _sizeMin[0].cy + _sizeMin[1].cy + m_cxySplitBar + m_cxyBarEdge;
			}
			return sizeMin;
		}

		LRESULT OnMouseMove(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			int xPos = GET_X_LPARAM(lParam);
			int yPos = GET_Y_LPARAM(lParam);
			if((wParam & MK_LBUTTON) && ::GetCapture() == m_hWnd)
			{
				int xyNewSplitPos = 0;
				if(t_bVertical){
					xyNewSplitPos = xPos - m_rcSplitter.left - m_cxyDragOffset;
				}else{
					xyNewSplitPos = yPos - m_rcSplitter.top - m_cxyDragOffset;
				}

				if(xyNewSplitPos == -1)   // avoid -1, that means middle
					xyNewSplitPos = -2;

				if(m_xySplitterPos != xyNewSplitPos)
				{
					if(m_bFullDrag)
					{
						if(SetSplitterPos(xyNewSplitPos, true))
							UpdateWindow();
					}
					else
					{
						DrawGhostBar();
						SetSplitterPos(xyNewSplitPos, false);
						DrawGhostBar();
					}
				}
			}
			else		// not dragging, just set cursor
			{
				if(IsOverSplitterBar(xPos, yPos))
					::SetCursor(m_hCursor);
				bHandled = FALSE;
			}

			return 0;
		}

		bool SetSplitterPos(int xyPos = -1, bool bUpdate = true)
		{
			if(xyPos == -1)   // -1 == middle
			{
				if(t_bVertical)
					xyPos = (m_rcSplitter.right - m_rcSplitter.left - m_cxySplitBar - m_cxyBarEdge) / 2;
				else
					xyPos = (m_rcSplitter.bottom - m_rcSplitter.top - m_cxySplitBar - m_cxyBarEdge) / 2;
			}

			// Adjust if out of valid range
			int cxyMax = 0;
			int posMin = 0;
			int posMax = 0;
			if(t_bVertical){
				cxyMax = m_rcSplitter.right - m_rcSplitter.left;
				posMin = _sizeMin[0].cx;
				posMax = cxyMax - m_cxySplitBar - m_cxyBarEdge - _sizeMin[1].cx;
			}else{
				cxyMax = m_rcSplitter.bottom - m_rcSplitter.top;
				posMin = _sizeMin[0].cy;
				posMax = cxyMax - m_cxySplitBar - m_cxyBarEdge - _sizeMin[1].cy;
			}

			if(xyPos < posMin){
				xyPos = posMin;
			}else if(xyPos > posMax){
				xyPos = posMax;
			}

			// Set new position and update if requested
			bool bRet = (m_xySplitterPos != xyPos);
			m_xySplitterPos = xyPos;

			if(m_bUpdateProportionalPos)
			{
				if(IsProportional())
					StoreProportionalPos();
				else if(IsRightAligned())
					StoreRightAlignPos();
			}
			else
			{
				m_bUpdateProportionalPos = true;
			}

			if(bUpdate && bRet)
				UpdateSplitterLayout();

			return bRet;
		}
	};

	class CHorSplitterLtdWindow : 
		public CSplitterLtdWindowT<CHorSplitterLtdWindow,false>
	{	
		typedef CSplitterLtdWindowT<CHorSplitterLtdWindow,false> CHorSplitWinImpl;
		BEGIN_MSG_MAP(CHorSplitterLtdWindow)
			MESSAGE_HANDLER(WM_MOVE, OnMove)
			CHAIN_MSG_MAP(CHorSplitWinImpl)
		END_MSG_MAP()

		//CHorSplitScrollWindow implementation

		LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			CPoint pt(LOWORD(lParam),HIWORD(lParam)); pt;
			//// CPoint pt - in parent-client coordinates 
			//CStatObjectView* pStatObjectView=(CStatObjectView*)
			//	((char*)this-offsetof(CStatObjectView,m_wndHorzSplitter));
			//CDeferPos dp(1);
			//dp.MoveWindow(*this,-posScroll.x, -posScroll.y,
			//	size.cx, size.cy,FALSE);
			return 0;
		}
	};

	class CSplitterLtdWindow : 
		public CSplitterLtdWindowT<CSplitterLtdWindow,true>
	{	
		//CSplitScrollWindow implementation

		LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
		{
			// CPoint pt - in parent-client coordinates 
			UpdateSplitterLayout();
			LRESULT lr=DefWindowProc(uMsg,wParam,lParam);
			//	bHandled=FALSE;
			return 0;
		}
	};
}
