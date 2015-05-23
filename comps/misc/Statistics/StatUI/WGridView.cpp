// WGridView.cpp : implementation of the CWGridView class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "WGridView.h"
namespace ViewSpace{

	BOOL CWGridView::PreTranslateMessage(MSG* pMsg)
	{
		pMsg;
		return FALSE;
	}

	LRESULT CWGridView::OnCreate(LPCREATESTRUCT pCS)
	{
		LRESULT lr=DefWindowProc(WM_CREATE,0,(LRESULT)pCS);
		SetExtendedListViewStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);
		{
			LVCOLUMN col={LVCF_ORDER|LVCF_SUBITEM};
			col.iOrder=INT_MAX;
			col.iSubItem=33;
			InsertColumn(0, &col);
			GetColumn(0,&col);
			lr=lr;
			{
				HWND hdr=GetHeader();
				int colCount=Header_GetItemCount(hdr);
				valarray<int> iArr(-1,colCount);
				BOOL b=GetColumnOrderArray(iArr.size(), &iArr[0]);
			}
		}
		//m_hdr.SubclassWindow(GetHeader());
		return lr;
	}

	BOOL CWGridView::Scroll(int dx, int dy)
	{
		BOOL b=ListView_Scroll(*this, dx, dy);


		//SCROLLINFO si={sizeof(si),SIF_ALL};
		//BOOL b1=GetScrollInfo(SB_HORZ, &si);
		//si.fMask +=SIF_DISABLENOSCROLL;
		//b1=SetScrollInfo(SB_HORZ, &si, FALSE);
		//SCROLLINFO si={sizeof(si),SIF_DISABLENOSCROLL};

		//HDC hdc=GetDC();
		//POINT pt;
		//BOOL b3=GetViewportOrgEx(hdc,&pt);
		//b3=SetViewportOrgEx(hdc,pt.x+2,pt.y+2,0);
		//b3=ReleaseDC(hdc);
		return b;
	}

	LRESULT CWGridView::OnHScroll(int scrollType, short shift, HWND scroller)
	{
		LRESULT lr=1;
		HWND hdr=ListView_GetHeader(m_hWnd);
		int m_colCount=Header_GetItemCount(hdr);
		SetEmptyColWidth(m_colLastSwap);
		Scroll(0,0);
		SetColumnWidth(m_colEmpty,m_wEmpty);
		valarray<int> iArr(-1,m_colCount);
		switch(scrollType)
		{
		default:break;
		case SB_ENDSCROLL:
			EndUpdate();
			break;
		case SB_LINERIGHT:
			{
				BeginUpdate();
				BOOL b=ListView_GetColumnOrderArray(
					m_hWnd,
					iArr.size(),
					&iArr[0]);
				int iColZero=iArr[0];
				int iScroll=0;
				int colLeft=iColZero;
				if(colLeft<m_colLastSwap)
				{
					colLeft++;
					int widthLeft=ListView_GetColumnWidth(m_hWnd,0);
					RECT rect;
					GetClientRect(&rect);
					int iwidthRight=rect.right-rect.left-widthLeft;
					SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
					BOOL b=GetScrollInfo(SB_HORZ, &si);
					int iW=ListView_GetColumnWidth(m_hWnd,colLeft);
					iScroll=iW;
				}
				if(0!=iColZero)
				{
					swap(iArr[0],iArr[iColZero]);
				}
				swap(iArr[0],iArr[colLeft]);
				b=ListView_SetColumnOrderArray(
					m_hWnd,
					iArr.size(),
					&iArr[0]);
				Scroll(iScroll, 0);
				//			EndUpdate();
				lr=0;
			}break;
		case SB_LINELEFT:
			{
				BeginUpdate();
				BOOL b=ListView_GetColumnOrderArray(
					m_hWnd,
					iArr.size(),
					&iArr[0]);
				int iColZero=iArr[0];
				int iScroll=0;
				int colLeft;
				{
					int widthLeft=ListView_GetColumnWidth(m_hWnd,0);
					RECT rect;
					GetClientRect(&rect);
					int iwidthRight=rect.right-rect.left-widthLeft;
					SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
					BOOL b=GetScrollInfo(SB_HORZ, &si);
					{
						if(0==iColZero){
							iScroll=0;
							break;
						}else{
							colLeft=iColZero-1;
							iScroll=-ListView_GetColumnWidth(m_hWnd,iColZero);
						}
					}
				}
				if(0!=iColZero)
				{
					swap(iArr[0],iArr[iColZero]);
				}
				swap(iArr[0],iArr[colLeft]);
				b=ListView_SetColumnOrderArray(
					m_hWnd,
					iArr.size(),
					&iArr[0]);
				Scroll(iScroll, 0);
				//			EndUpdate();
				lr=0;
			}break;
		case SB_PAGERIGHT:
			{
				int nLVCols=Header_GetItemCount(GetHeader());
				int* arrCols=(int*)_alloca((nLVCols)*sizeof(int));
				BOOL b0=GetColumnOrderArray(nLVCols,arrCols);
				for(int i=nLVCols; --i>=0;)
				{
					LVCOLUMN lvCol={LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER};
					LRESULT lr=GetColumn(arrCols[i],&lvCol);
					_ASSERTE(lr);
				}
				int w=GetColumnWidth(m_colEmpty);
				BeginUpdate();
				BOOL b=ListView_GetColumnOrderArray(
					m_hWnd,
					iArr.size(),
					&iArr[0]);
				int iColZero=iArr[0];
				int iScroll=0;
				int widthLeft=GetColumnWidth(0);
				RECT rect;
				GetClientRect(&rect);
				SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
				BOOL b1=GetScrollInfo(SB_HORZ, &si);
				int posMin=si.nPos-(rect.right-rect.left);

				// compute next base column
				int iwidthRight=rect.right-rect.left-widthLeft;
				int iW=iwidthRight;
				int colLeft=iColZero;
				for(;colLeft<(int)iArr.size()-1; ++colLeft)
				{
					int colW=GetColumnWidth(colLeft+1);
					iW -= colW;
					if(iW<0){
						break;
					}
				}
				colLeft=__max(colLeft,iColZero+1);
				if(m_colLastSwap>=0)
					colLeft=__min(colLeft,m_colLastSwap);

				if(colLeft!=iColZero)
				{
					swap(iArr[0],iArr[iColZero]);
					swap(iArr[0],iArr[colLeft]);
					b=ListView_SetColumnOrderArray(
						m_hWnd,
						iArr.size(),
						&iArr[0]);
					b1=GetScrollInfo(SB_HORZ, &si);
					int xShift=0;
					for(int col=1; col<=colLeft; ++col)
						xShift+=GetColumnWidth(col);
					Scroll(xShift-si.nPos,0);
				}
				lr=0;
				//			EndUpdate();
			}break;
		case SB_PAGELEFT:
			{
				BeginUpdate();
				BOOL b=ListView_GetColumnOrderArray(
					m_hWnd,
					iArr.size(),
					&iArr[0]);
				int iColZero=iArr[0];
				int iScroll=0;
				int widthLeft=ListView_GetColumnWidth(m_hWnd,0);
				RECT rect;
				GetClientRect(&rect);
				int iwidthRight=rect.right-rect.left-widthLeft;
				SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
				b=GetScrollInfo(SB_HORZ, &si);
				int wCol0=ListView_GetColumnWidth(m_hWnd,0);
				int colLeft=0;
				for(int i=iColZero; i>=0; --i)
				{
					int iW=iScroll-ListView_GetColumnWidth(m_hWnd,i);
					if(iW+si.nPos <= wCol0){
						colLeft=0;
						iScroll = -si.nPos;
						break;
					}else if(-iW>iwidthRight){
						colLeft=i;
						break;
					}else{
						iScroll=iW;
					}
				}
				colLeft=__min(colLeft,iColZero-1);

				if(colLeft!=iColZero)
				{
					swap(iArr[0],iArr[iColZero]);
					swap(iArr[0],iArr[colLeft]);
					b=ListView_SetColumnOrderArray(
						m_hWnd,
						iArr.size(),
						&iArr[0]);
					b=GetScrollInfo(SB_HORZ, &si);
					int xShift=0;
					for(int col=1; col<=colLeft; ++col)
						xShift+=GetColumnWidth(col);
					Scroll(xShift-si.nPos,0);
				}
				//			EndUpdate();
				lr=0;
			}break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			{
				BeginUpdate();
				SCROLLINFO si={0};
				si.cbSize = sizeof(SCROLLINFO);
				si.fMask = SIF_ALL;
				BOOL b=GetScrollInfo(SB_HORZ, &si);

				int iScroll=0, iCol=0;
				for(unsigned i=1; i<iArr.size(); ++i)
				{
					int width=GetColumnWidth(i);
					iScroll += width;
					if(iScroll>shift){
						iCol=i-1;
						iScroll -= width;
						break;
					}
				}
				b=GetColumnOrderArray(
					iArr.size(),
					&iArr[0]);
				if(0!=iArr[0])
				{
					swap(iArr[0],iArr[iArr[0]]);
				}
				swap(iArr[0],iArr[iCol]);
				b=SetColumnOrderArray(
					iArr.size(),
					&iArr[0]);
				b=Scroll(iScroll-si.nPos,0);
				lr=0;
				if(SB_THUMBTRACK==scrollType)
					EndUpdate();
			}break;
		}
		//	SetMsgHandled(FALSE);
		return lr;
	}

	LRESULT CWGridView::OnVScroll(int scrollType, short shift, HWND scroller)
	{
		LRESULT lr=1;
		SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
		BOOL b=GetScrollInfo(SB_VERT, &si);
		int nPos=si.nPos;
		switch(scrollType)
		{
		default:
			SetMsgHandled(FALSE);
			return 0;
		case SB_TOP:
			nPos=0;
			break;
		case SB_BOTTOM:
			nPos=si.nMax-si.nPage;
			break;
		case SB_ENDSCROLL:
			break;
		case SB_LINEDOWN:
			++nPos;
			break;
		case SB_LINEUP:
			--nPos;
			break;
		case SB_PAGEDOWN:
			nPos+=si.nPage;
			break;
		case SB_PAGEUP:
			nPos-=si.nPage;
			break;
		case SB_THUMBPOSITION:
		case SB_THUMBTRACK:
			nPos=shift;
			break;
		}
		if(nPos>si.nMax-(int)si.nPage+1){
			nPos=si.nMax-si.nPage+1;
		}else if(nPos<si.nMin){
			nPos=si.nMin;
		}
			RECT rc;
			GetItemRect(si.nPos+si.nPage-1,&rc,LVIR_BOUNDS);
		if(si.nPos!=nPos){
			int h=rc.bottom-rc.top;
			InvalidateRect(&rc,FALSE);
			GetItemRect(nPos+si.nPage-1,&rc,LVIR_BOUNDS);
			InvalidateRect(&rc,FALSE);
			Scroll(0,(nPos-si.nPos)*h);
			//		__trace("Invalidate rows %d %d\n",si.nPos+si.nPage-1,nPos+si.nPage-1);
		}
		RECT rcRight;
		BOOL b3=GetClientRect(&rcRight);
		rcRight.left=rc.right;
		InvalidateRect(&rcRight,FALSE);
		return lr;
	}

	LRESULT CWGridView::OnMouseWheel(UINT fwKeys, short zDelta, CPoint pnt)
	{
		SetMsgHandled(FALSE);
		return 0;
	}

	struct EntEx{
		CString _s;
		EntEx(CString s){
			_s=s;
			OutputDebugString(CString("Enter ")+_s +"\n");
		}
		~EntEx(){
			OutputDebugString(CString("Exit ")+_s +"\n");
		}
	};

	//Returns TRUE to prevent the change, or FALSE to allow the change.
	LRESULT CWGridView::OnItemChanging(LPNMHDR pnmh)
	{
		EntEx ee("OnItemChanging");
		LPNMHEADER pNmHeader=(LPNMHEADER)pnmh;
		int iItem=pNmHeader->iItem;

		if(0==pNmHeader->iItem && GetHeader()==GetCapture()){
			int wColFreeze=GetColumnWidth(0);
			CRect rc; GetClientRect(rc);
			if(pNmHeader->pitem->cxy>rc.Width()-1){
				return TRUE;
			}
		}
		int wOld=GetColumnWidth(pNmHeader->iItem);
		int dif=pNmHeader->pitem->cxy - wOld;
		if(m_colEmpty>0 && pNmHeader->iItem==m_colEmpty){
			if(m_bSetColumnWidth){
				if(dif)
					return FALSE;
				else
					return TRUE;
			}else{
				{CString s; s.Format("Colunm %d Empty col %d\n",(pNmHeader->iItem),(m_colEmpty)); OutputDebugString(s);}
				return TRUE;
			}
		}
		SetMsgHandled(FALSE);
		return FALSE;
	}

	LRESULT CWGridView::OnItemChanged(LPNMHDR pnmh)
	{
		EntEx ee("OnItemChanged");
		LPNMHEADER pNmHeader=(LPNMHEADER)pnmh;
		if(m_colEmpty>0 && pNmHeader->iItem!=m_colEmpty)
		{
			if(GetHeader()==GetCapture()){
				SCROLLINFO si={sizeof(si),SIF_ALL};
				BOOL b1=GetScrollInfo(SB_HORZ, &si);

				HWND hdr=GetHeader();
				int colCount=Header_GetItemCount(hdr);
				valarray<int> iArr(-1,colCount);
				BOOL b=GetColumnOrderArray(iArr.size(), &iArr[0]);
				int colCurrent=iArr[0];
				if(colCurrent>0){
					int wEmpty=si.nPage;
					int col=colCount-2;
					for( ; col>=colCurrent; --col){
						int w=GetColumnWidth(iArr[col]);
						wEmpty -= w; 
						if(wEmpty<=0){
							wEmpty +=w;
							break;
						}
					}
					m_wEmpty=0;
					if(wEmpty>0){
						if(wEmpty!=GetColumnWidth(m_colEmpty)){
							BeginUpdate();
							SetColumnWidth(m_colEmpty,wEmpty);
							b1=GetScrollInfo(SB_HORZ, &si);
							int xShift=0;
							for(col=1; col<=colCurrent; ++col)
								xShift+=GetColumnWidth(col);
							Scroll(xShift-si.nPos,0);
							EndUpdate();
						}
					}
				}
			}
		}else if(pNmHeader->iItem==m_colEmpty){
			int itemSel=GetSelectionMark();
			RECT rc={0};
			BOOL b=GetSubItemRect(itemSel,m_colEmpty,LVIR_BOUNDS,&rc);
			InvalidateRect(&rc);
			return 0;
		}
		SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT CWGridView::OnTrack(LPNMHDR pnmh)
	{
		LPNMHEADER pNmHeader=(LPNMHEADER)pnmh;
		if(m_colEmpty>0 && pNmHeader->iItem!=m_colEmpty)
		{
			SCROLLINFO si={sizeof(si),SIF_ALL};
			BOOL b1=GetScrollInfo(SB_HORZ, &si);
			int wLast=GetColumnWidth(m_colEmpty);
			if((int)si.nPage>si.nMax-wLast){
				SetColumnWidth(m_colEmpty,0);
			}else{
				int colSwap;
				SetEmptyColWidth(colSwap);
			}
		}
		SetMsgHandled(FALSE);
		return 0;
	}

	LRESULT CWGridView::OnWindowPosChanging(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM lParam, BOOL& /*bHandled*/)
	{
		int wLast=GetColumnWidth(m_colEmpty);
		if(m_wEmpty>0 && wLast>0){
			CRect rcOld;
			GetWindowRect(&rcOld);
			WINDOWPOS& wp=*(WINDOWPOS*)lParam;
			int dif = wp.cx - rcOld.Width();
			if(dif>0){
				wLast=SetColumnWidth(m_colEmpty, wLast+dif);
				wLast=wLast;
			}
		}
		return 0;
	}

	LRESULT CWGridView::OnSize(UINT resizeType, CSize size)
	{
		if(SIZE_RESTORED==resizeType
			|| SIZE_MAXIMIZED==resizeType)
		{
			SCROLLINFO si={sizeof(si),SIF_ALL};
			BOOL b1=GetScrollInfo(SB_VERT, &si);

			// if selected item is last item then select new last item
			int itemSel=GetSelectionMark();
			if(itemSel==si.nMax){
				int iItemHot=SetItemState(si.nPos+(int)si.nPage-1,
					LVIS_FOCUSED|LVIS_SELECTED,LVIS_FOCUSED|LVIS_SELECTED);
			}

			CRect rc;
			GetItemRect(si.nPos+(int)si.nPage-1,&rc,LVIR_BOUNDS);
			rc.top -= 3*rc.Height();
			InvalidateRect(&rc);
			{
				SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
				BOOL b=GetScrollInfo(SB_VERT, &si);
				int nPos=si.nPos-1+(int)si.nPage;
				BOOL b3=GetItemRect(nPos, &rc, LVIR_BOUNDS );
				RECT rcBottom;
				b3=GetClientRect(&rcBottom);
				rcBottom.top=rc.bottom;
				InvalidateRect(&rcBottom);
//				int ir=::FillRect( dc, &rcBottom, br );
//				rcBottom.top=rc.bottom;
			}


			b1=GetScrollInfo(SB_HORZ, &si);

			if(GetHeader()==GetCapture())
			{
			}
			else if(m_colEmpty>=0)
			{
				if((int)si.nPage>si.nMax)
				{
					m_wEmpty=0;
					SetColumnWidth(m_colEmpty,0);
				}
				else
				{
					SetRedraw(FALSE);
					HWND hdr=GetHeader();
					int colCount=Header_GetItemCount(hdr);
					valarray<int> iArr(-1,colCount);
					BOOL b=GetColumnOrderArray(iArr.size(), &iArr[0]);
					int& colCurrent=iArr[0];
					SetEmptyColWidth(m_colLastSwap);
					if(m_colLastSwap>=0 && m_colLastSwap<colCurrent){
						swap(iArr[0],iArr[colCurrent]);
						swap(iArr[0],iArr[m_colLastSwap]);
						b=SetColumnOrderArray(iArr.size(), &iArr[0]);
					}
					int xShift=0;
					for(int col=1; col<=colCurrent; ++col)
						xShift+=GetColumnWidth(col);
					//				Scroll(0,0);
					Scroll(xShift-si.nPos,0);
					//si.nPos=xShift;
					//si.nMax += 200;
					//si.nPos += 200;
					//b1=SetScrollInfo(SB_HORZ, &si);
					SetRedraw(TRUE);
					InvalidateRect(0);
				}
			}
		}
		return 0;
	}

	BOOL CWGridView::SetEmptyColWidth(int& colSwap)
	{
		if(m_colEmpty>=0)
		{
			SCROLLINFO si={sizeof(si),SIF_ALL};
			BOOL b1=GetScrollInfo(SB_HORZ, &si);
			if((int)si.nPage<=si.nMax){
				int wColFreeze=GetColumnWidth(0);
				HWND hdr=GetHeader();
				int colCount=Header_GetItemCount(hdr);
				valarray<int> iArr(-1,colCount);
				BOOL b=GetColumnOrderArray(iArr.size(), &iArr[0]);
				int colCurrent=iArr[0];
				int col=GetColumnCount()-1;
				int wEmpty=si.nPage-wColFreeze;
				if(wEmpty>0){
					for( ; col>0; --col){
						int w=GetColumnWidth(col);
						wEmpty -= w; 
						if(wEmpty<=0){
							wEmpty +=w;
							m_wEmpty=wEmpty;
							goto FewWinWidth;
						}
					}
				}
				m_wEmpty=0;
FewWinWidth:
				colSwap=col;
				SetColumnWidth(m_colEmpty,m_wEmpty);
				return true;
			}else{
				BOOL b=DeleteColumn(m_colEmpty);
				m_wEmpty=0;
				colSwap=0;
				return true;
			}
		}
		return false;
	}

	BOOL CWGridView::ComputeLastColWidth(int iItem)
	{
		HWND hdr=ListView_GetHeader(m_hWnd);
		int m_colCount=Header_GetItemCount(hdr);
		if(m_colCount<=0)
			return FALSE;

		if(m_colCount-1!=iItem){
			valarray<int> iArr(-1,m_colCount);
			BOOL b=ListView_GetColumnOrderArray(m_hWnd, iArr.size(), &iArr[0]);
			int iColZero=iArr[0];
			int iScroll=0;
			int widthFreeze=ListView_GetColumnWidth(m_hWnd,0);
			RECT rect;
			GetClientRect(&rect);
			int iwidthRight=rect.right-rect.left-widthFreeze;
			static unsigned LockUpdate=0;
			int colLeft=0;
			int colLast=m_colCount-2;
			for(int col=colLast; col>=1; --col)
			{
				int w=GetColumnWidth(col);
				iwidthRight-=w;
				if(iwidthRight<0){
					iwidthRight+=w;
					colLeft=col;
					break;
				}
			}
			if(0>iwidthRight)
				return FALSE;
			//	BOOL bLockUpdate=BeginUpdate();
			_ASSERTE(0<=iwidthRight);
			SetColumnWidth(m_colEmpty,iwidthRight);
			//		::InvalidateRect(hdr,NULL,TRUE);

			if(colLeft<iColZero)
			{
				if(0!=iColZero)
				{
					swap(iArr[0],iArr[iColZero]);
				}
				swap(iArr[0],iArr[colLeft]);
				b=ListView_SetColumnOrderArray(
					m_hWnd,
					iArr.size(),
					&iArr[0]);
				iColZero=colLeft;
				//			InvalidateRect(0);
			}

			int posMax=0;
			for(int i=1; i<=iColZero; ++i)
			{
				posMax+=GetColumnWidth(i);
			}
			SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
			b=GetScrollInfo(SB_HORZ, &si);
			iScroll=posMax-si.nPos;
			//if(iScroll)
			Scroll(iScroll, 0);
			//if(bLockUpdate)
			//	EndUpdate();
			//		InvalidateRect(0);
		}

		return TRUE;
	}

	BOOL	CWGridView::Header_SetFont(LOGFONT* plf)
	{
		if( !plf )
			return FALSE;

		HWND hwndHeader = 0;
		hwndHeader =GetHeader();
		if( !hwndHeader )
			return FALSE;

		// create font and set it for header
		HFONT hfNewFont = 0;
		hfNewFont = ::CreateFontIndirect( plf );
		if( !hfNewFont )
			return FALSE;

		::SendMessage( hwndHeader, WM_SETFONT, (WPARAM)hfNewFont, (LPARAM) TRUE );

		if( m_hfHeaderFont )
		{	// delete old font
			::DeleteObject( m_hfHeaderFont );	
			m_hfHeaderFont = 0;
		}

		m_hfHeaderFont = hfNewFont;	// save new font
		return TRUE;    
	}

	int CWGridView::GetColumnCount()
	{
		HWND hdr=GetHeader();
		if(hdr){
			int cols=Header_GetItemCount(hdr);
			if(m_wEmpty>0)
				--cols;
			return cols;
		}else
			return -1;
	}
	void CWGridView::DeleteAllColumns(void)
	{
		int nLVCols=Header_GetItemCount(GetHeader());
		int* arrCols=(int*)_alloca((nLVCols)*sizeof(int));
		m_colEmpty=-1;
		BOOL b=GetColumnOrderArray(nLVCols,arrCols);
		for(int i=nLVCols; --i>=0;)
		{
			LVCOLUMN lvCol={LVCF_WIDTH|LVCF_SUBITEM|LVCF_ORDER};
			LRESULT lr=GetColumn(arrCols[i],&lvCol);
			b=DeleteColumn(i);
			_ASSERTE(b);
		}
	}

	int CWGridView::InsertColumn(int iCol, LPLVCOLUMN pLVColumn)
	{
		int col=__super::InsertColumn(iCol,pLVColumn);
		{
			HWND hdr=GetHeader();
			int colCount=Header_GetItemCount(hdr);
			valarray<int> iArr(-1,colCount);
			BOOL b=GetColumnOrderArray(iArr.size(), &iArr[0]);
		}
		if(INT_MAX==iCol){
			m_colEmpty=col;
			SetEmptyColWidth(m_colLastSwap);
			Scroll(0,0);
		}
		return col;
	}

	LRESULT CWGridView::OnPaint(HDC hdc)
	{
		CPaintDC dc(*this);

		POINT pt;
		BOOL b3=GetViewportOrgEx((HDC)dc,&pt);
		//		b3=SetViewportOrgEx((HDC)dc,pt.x-2,pt.y,0);

		RECT rcHdr;
		::GetWindowRect(GetHeader(),&rcHdr);
		int yHeader=rcHdr.bottom-rcHdr.top;
		ScreenToClient((LPPOINT)&rcHdr);
		ScreenToClient(((LPPOINT)&rcHdr)+1);

		// paint background under header
		RECT rc=rcHdr;
		rc.top=rc.bottom;
		rc.bottom += 1;
		HBRUSH br = ::GetSysColorBrush(COLOR_BTNFACE);
		int ir=::FillRect( dc, &rc, br );

		//paint backgroud next to last column
		b3=GetItemRect(GetItemCount()-1, &rc, LVIR_BOUNDS );
		RECT rcRight;
		b3=GetClientRect(&rcRight);
		rcRight.left=rc.right;
		ir=::FillRect( dc, &rcRight, ::GetSysColorBrush(COLOR_WINDOW));


		LRESULT lr=DefWindowProc(WM_PAINT,(WPARAM)(HDC)dc,0);

		// paint background left to freezing column
		b3=GetSubItemRect(GetItemCount()-1, 0, LVIR_BOUNDS, &rc );
		rc.top=rcHdr.bottom;
		rc.left = 0;
		rc.right = 4;
		ir=::FillRect( dc, &rc, ::GetSysColorBrush(COLOR_BTNFACE));

		//paint background under last visible row
		{
			SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
			BOOL b=GetScrollInfo(SB_VERT, &si);
			int nPos=si.nPos-1+(int)si.nPage;
			b3=GetItemRect(nPos, &rc, LVIR_BOUNDS );
		RECT rcBottom;
		b3=GetClientRect(&rcBottom);
		rcBottom.top=rc.bottom;
		ir=::FillRect( dc, &rcBottom, br );
			rcBottom.top=rc.bottom;
		}

		return lr;
	}

	LRESULT CWGridView::OnSetScrollInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
	{
		LRESULT lr=DefWindowProc(uMsg,wParam,lParam);
		return lr;
	}

	BOOL CWGridView::EnsureVisible(int nItem, BOOL bPartialOK)
	{
		BOOL b=__super::EnsureVisible(nItem, bPartialOK);
		SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
		BOOL b1=GetScrollInfo(SB_VERT, &si);
		RECT rc;
		if(nItem==si.nPos+si.nPage-1){
			GetItemRect(si.nPos+si.nPage-1,&rc,LVIR_BOUNDS);
			int h=rc.bottom-rc.top;
			InvalidateRect(&rc);
			Scroll(0,h);
		}else{
			GetItemRect(si.nPos+si.nPage-1,&rc,LVIR_BOUNDS);
			InvalidateRect(&rc);
		}
		return b;
	}

	BOOL CWGridView::SetItemState(int nItem, UINT nState, UINT nStateMaske)
	{
		SCROLLINFO si={sizeof(SCROLLINFO),SIF_ALL};
		BOOL b1=GetScrollInfo(SB_VERT, &si);
		RECT rc;
		if(nItem==si.nPos+si.nPage-1){
			GetItemRect(si.nPos+si.nPage-1,&rc,LVIR_BOUNDS);
			int h=rc.bottom-rc.top;
			InvalidateRect(&rc);
			Scroll(0,h);
		}else{
			GetItemRect(si.nPos+si.nPage-1,&rc,LVIR_BOUNDS);
			InvalidateRect(&rc);
		}
		return __super::SetItemState(nItem, nState, nStateMaske);
	}

	BOOL CWGridView::SetColumnWidth(int nCol, int cx)
	{
		int cols=Header_GetItemCount(GetHeader());
		BOOL b;
		if(nCol==m_colEmpty && m_colEmpty>0){
			m_bSetColumnWidth=true;

			if (cols-1==m_colEmpty && cx<=0){
				b=DeleteColumn(m_colEmpty);
				m_wEmpty=0;
				return b;
			}
			if(cols==m_colEmpty && cx>0){
				LVCOLUMN col={LVCF_WIDTH|LVCF_ORDER|LVCF_SUBITEM};
				col.cx=cx;
				col.iOrder=INT_MAX;
				col.iSubItem=INT_MAX;
				__super::InsertColumn(INT_MAX, &col);
				HWND hdr=GetHeader();
				int colCount=Header_GetItemCount(hdr);
				valarray<int> iArr(-1,colCount);
				b=GetColumnOrderArray(iArr.size(), &iArr[0]);
				m_wEmpty=0;
				return b;
			}
			b=__super::SetColumnWidth(nCol,cx);
			m_bSetColumnWidth=false;
			return b;
		}else{
			b=__super::SetColumnWidth(nCol,cx);
			return b;
		}
		b=__super::SetColumnWidth(nCol,cx);
		return 0;
	}

}
