// VTListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "VTControls.h"
#include "ListBaseCtrl.h"
#include "VTList.h"
#include "utils.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define ID_FLISTCTRL_TOOLTIP	1

/////////////////////////////////////////////////////////////////////////////
// CLVEdit

BEGIN_MESSAGE_MAP(CLVEdit, CEdit)
	//{{AFX_MSG_MAP(CLVEdit)
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CLVEdit::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
	lpwndpos->x=m_x;
	lpwndpos->y=m_y;
	lpwndpos->cx = m_width;
	lpwndpos->cy = m_height;

	CEdit::OnWindowPosChanging(lpwndpos);
}

/////////////////////////////////////////////////////////////////////////////
void CLVEdit::BuildAppearance( COLORREF clrText, COLORREF clBack )
{
	m_clText = clrText;
	m_clBack = clBack;

	m_brushCtrlEditBackColor.DeleteObject();
	m_brushCtrlEditBackColor.CreateSolidBrush( m_clBack );

}


HBRUSH CLVEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CEdit::OnCtlColor(pDC, pWnd, nCtlColor);
	
	
	pDC->SetTextColor( m_clText );
	pDC->SetBkColor( m_clBack );	
	return m_brushCtrlEditBackColor;

	return hbr;
}



BOOL CLVEdit::OnChildNotify(UINT message, WPARAM wParam,
                           LPARAM lParam, LRESULT* pLResult)
{
	if (message != WM_CTLCOLOREDIT)
	{
	 return CEdit::OnChildNotify(message,wParam,lParam,pLResult);
	}

	HDC hdc = (HDC)wParam;

	if( !hdc )
		return CEdit::OnChildNotify(message,wParam,lParam,pLResult);

	SetTextColor( hdc, m_clText );
	SetBkColor( hdc, m_clBack );


   // Send what would have been the return value of OnCtlColor() - the
   // brush handle - back in pLResult:
      *pLResult = (LRESULT)(m_brushCtrlEditBackColor.GetSafeHandle());


	return TRUE;
	
}


   

IMPLEMENT_SERIAL(CListBaseCtrl, CListCtrl, 1)

/////////////////////////////////////////////////////////////////////////////
// CListBaseCtrl


CListBaseCtrl::CListBaseCtrl()
{
	m_bWasCreated = FALSE;

	m_nCurRow = m_nCurCol = 0;

	m_nRowCount = m_nColCount = 0;

	m_b_in_editMode = FALSE;

	m_clrEditTextColor	= 0x80000000 + COLOR_HIGHLIGHTTEXT;
	m_clrEditBackColor	= 0x80000000 + COLOR_HIGHLIGHT;

	m_clrActiveCellTextColor	= 0x80000000 + COLOR_HIGHLIGHTTEXT;
	m_clrActiveCellBackColor	= 0x80000000 + COLOR_HIGHLIGHT;

	m_clrBack			= 0x80000000 + COLOR_BTNFACE;

	//---------  For tolltip -------------
	/*m_pText = NULL; 
	m_bToolTipCtrlCustomizeDone = FALSE;*/
	//------------------------------------
}

CListBaseCtrl::~CListBaseCtrl()
{
	//---------  For tolltip -------------
	/*if (m_pText) 
		delete [] m_pText;*/
	//------------------------------------

	DeleteDataContext();
}

BEGIN_MESSAGE_MAP(CListBaseCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CListBaseCtrl)
	ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)	

	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_MEASUREITEM_REFLECT( )
	ON_WM_MEASUREITEM()	
	ON_MESSAGE(CPN_SELENDOK, OnSelEndOK)	
	ON_CBN_CLOSEUP(ID_COMBO_PICKER, OnCloseupCombo)
	//}}AFX_MSG_MAP
	ON_WM_HSCROLL()
	//---------  For tolltip -------------
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	//------------------------------------
	ON_WM_MOUSEMOVE()
	//ON_WM_NCHITTEST()
	ON_WM_VSCROLL()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBaseCtrl message handlers

BOOL CListBaseCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_REPORT | WS_CHILD | WS_VISIBLE | WS_BORDER | WS_TABSTOP |
		LVS_EDITLABELS | LVS_OWNERDRAWFIXED|WS_CLIPCHILDREN|WS_CLIPSIBLINGS;	
	
	//cs.dwExStyle |= LVS_EX_GRIDLINES;

	return CListCtrl::PreCreateWindow(cs);
}


BOOL CListBaseCtrl::Create(CVTList* pParent)
{

	//OutputDebugString( "CListBaseCtrl::Create\n" );

	VERIFY(m_imageList.Create(16,16,ILC_COLOR4|ILC_MASK, 1, 1));	
	m_imageList.SetBkColor( CLR_NONE );


	ASSERT(pParent);
	m_pParent = pParent;

	BOOL b_result = CListCtrl::Create( WS_CHILD | WS_VISIBLE, 
			CRect(0, 0, 0, 0),
			(CWnd*)m_pParent,
			1 // [vanek] BT2000: 3718 - 16.03.2004
			);
	VERIFY(b_result);

	// [vanek] BT2000: 3542 - 26.12.2003 
	CHeaderCtrl *pheader = GetHeaderCtrl();
	if( b_result && pheader)
	{
		m_headerCtrl.m_plist = this;
		m_headerCtrl.SubclassWindow( pheader->GetSafeHwnd() );
	} 

	m_bWasCreated = TRUE;

	if( UseGrid() )
		SetExtendedStyle( GetExtendedStyle( ) | LVS_EX_GRIDLINES );

	//SetExtendedStyle(GetExtendedStyle() | LVS_EX_INFOTIP);

	m_ColourPicker.Create( "",WS_CHILD | WS_VISIBLE, 
			CRect(0, 0, 0, 0), this, ID_COLOR_PICKER);


	m_Combo.Create( WS_CHILDWINDOW | CBS_DROPDOWN |  WS_VSCROLL | WS_TABSTOP | CBS_HASSTRINGS, 
			CRect(0, 0, 0, 100 ), this, ID_COMBO_PICKER );

	HFONT hfont = (HFONT)::SendMessage( GetSafeHwnd(), WM_GETFONT, 0, 0 );
	::SendMessage( m_Combo.GetSafeHwnd(), WM_SETFONT, WPARAM(hfont), 0 );

	BuildControl( );

	m_ColourPicker.SetSelectionMode( CP_MODE_BK );
	m_ColourPicker.SetTrackSelection( TRUE );	

	m_ShadingPicker.Create( "",WS_CHILD | WS_VISIBLE, 
			CRect(0, 0, 0, 0), this, ID_SHADING_PICKER);
	m_ShadingPicker.SetSelectionMode( CP_MODE_BK );
	m_ShadingPicker.SetTrackSelection( TRUE );	

	b_tooltip=false;
	bForceEdit=false;
	return TRUE;

}


void CListBaseCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	//if(b_tooltip)
	//	return;
	PaintColorPicer();
	PaintShadingPicker();

	//Is selected item?
	BOOL b_SelectedItem;
	BOOL b_SelectedRow;
	CRect rc;

	//Curent row
	int iRow = lpDrawItemStruct->itemID;
			
	CDC *pDC= CDC::FromHandle(lpDrawItemStruct->hDC);

	for(int iCol=0;iCol<m_nColCount;iCol++)
	{
		b_SelectedItem = FALSE;
		b_SelectedRow  = FALSE;

		if( (m_nCurRow == iRow) && (m_nCurCol == iCol) )
			b_SelectedItem = TRUE;

		if( (m_nCurRow == iRow) )
			b_SelectedRow = TRUE;


		if ( (iCol<m_columns.GetSize( )) )
		{
			
			GetCellRect(iRow, iCol, LVIR_BOUNDS, rc);
			m_columns[iCol]->DrawItem( iRow, pDC, rc, b_SelectedItem, b_SelectedRow,
								this);
		}
		else
			TRACE("\nIndex out of range.Point CListBaseCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)");

	}
				
	
}


void CListBaseCtrl::ReMeasureItem()
{
	if( m_hWnd )
	{
		CRect rc,rect;
		GetWindowRect( &rc );
		
		WINDOWPOS wp;
		wp.hwnd = m_hWnd;
		wp.cx = rc.Width();
		wp.cy = rc.Height();
		wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
		SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );
	}
}


void CListBaseCtrl::MeasureItem ( LPMEASUREITEMSTRUCT lpMeasureItemStruct )
{
	int nMaxHeight = GetMaxCellHeight() + 3 * VT_LIST_SELECTION_ALIGN;
	if( nMaxHeight < VT_LIST_ICON_SIZE + 2)  
		nMaxHeight = VT_LIST_ICON_SIZE + 2;

	lpMeasureItemStruct->itemHeight = ( nMaxHeight>0 ? nMaxHeight : -nMaxHeight/*lpMeasureItemStruct->itemHeight*/ );
}


int CListBaseCtrl::GetMaxCellHeight()
{
		
	CClientDC dc(this);	

	int nMaxHeight = 0;
	CBaseCell* pCell = NULL;
	for(int i=0;i<m_nRowCount;i++){
		for(int j=0;j<m_nColCount;j++){
			pCell = GetCell( i, j );
			if(pCell){
				CFont font;

				if( m_pParent && m_pParent->_GetUseSystemFont() )
				{
					::GetFontFromDispatch( m_pParent, font, true );
				}
				else
				{
					LPLOGFONT lpLF = pCell->GetAsFont();				
					font.CreateFontIndirect( lpLF );
				}
				
					
				TEXTMETRIC tm;
				CFont* pOldFont = dc.SelectObject( &font );
				dc.GetTextMetrics( &tm );					
				dc.SelectObject( pOldFont );

				nMaxHeight = max( nMaxHeight, tm.tmHeight );			
				
			}
		}
	}

	return nMaxHeight;

}

// [vanek] - 13.01.2004: для корректной работы, когда разрешен drag-drop столбцов :)
BOOL CListBaseCtrl::get_rects_near_real_cells( int iRow, int iCol, int nArea, LPRECT lpRectLeftCell, LPRECT lpRectRightCell )
{
	int nColumnCount = GetColumnCount();
	int *parrColumnByOrder = 0;
	parrColumnByOrder = new int[ nColumnCount ];
	BOOL bRetVal = TRUE;

	::SetRectEmpty( lpRectLeftCell );
	::SetRectEmpty( lpRectRightCell );

	if( bRetVal = GetColumnOrderArray( parrColumnByOrder, nColumnCount ) )
	{
		for( int iCurrOrder = 0; iCurrOrder < nColumnCount; iCurrOrder ++ )
		{

			if( parrColumnByOrder[ iCurrOrder ] == iCol )
			{
				for( int iLeftOrder = iCurrOrder - 1; iLeftOrder >= 0; iLeftOrder--)
				{
					CRect rtCol;
					if(GetSubItemRect( iRow, parrColumnByOrder[ iLeftOrder ], nArea, rtCol) 
						&& !rtCol.IsRectEmpty() )
					{
                        *lpRectLeftCell = rtCol;                                        
						break;
					}
				}
				for( int iRightOrder = iCurrOrder + 1; iRightOrder < nColumnCount; iRightOrder++ )
				{
					CRect rtCol;
					if(GetSubItemRect( iRow, parrColumnByOrder[ iRightOrder ], nArea, rtCol) 
						&& !rtCol.IsRectEmpty() )
					{
                        *lpRectRightCell = rtCol;                                        
						break;
					}
				}

				break;
			}
		}
    }
	
	if( parrColumnByOrder )
		delete[] parrColumnByOrder;	parrColumnByOrder = 0;
	//

	return bRetVal;
        
}

BOOL CListBaseCtrl::GetCellRect(int iRow, int iCol, int nArea, CRect &rect)
{

	if(iCol)
		return GetSubItemRect(iRow, iCol, nArea, rect);

	if(m_nColCount == 1)
		return GetItemRect(iRow, rect, nArea);

    // [vanek] - 13.01.2004
	RECT rtNearLeftCell = {0}, rtNearRightCell = {0};
	if( !get_rects_near_real_cells( iRow, iCol, nArea, &rtNearLeftCell, &rtNearRightCell ) )
		return FALSE;

	if(!GetItemRect(iRow, rect, nArea))
		return FALSE;

	if( !::IsRectEmpty( &rtNearLeftCell) )
		rect.left = rtNearLeftCell.right;

	if( !::IsRectEmpty( &rtNearRightCell ) ) 
		rect.right = rtNearRightCell.left;


	return TRUE;
}


void CListBaseCtrl::InvalidateCell(int iRow, int iCol)
{
	if( (iRow >=0) && (iRow < m_nRowCount) )
	{
		CRect rcBounds;

		if( UseFullRowSelect( ) )
			GetItemRect( iRow, &rcBounds, LVIR_BOUNDS );
		else
			GetCellRect(iRow, iCol, LVIR_BOUNDS, rcBounds);

		//GetItemRect(uRow, rcBounds, LVIR_BOUNDS);
		InvalidateRect( &rcBounds );
	}
	
}



int CListBaseCtrl::HitTestEx(CPoint &point, int *col)
{
	int colnum = 0;
	if( col ) *col = 0;

	CHeaderCtrl* pHeader= NULL;
	pHeader= GetHeaderCtrl();
	if(!pHeader) return -1;

	int row = HitTest( point, NULL );
	

	// Make sure that the ListView is in LVS_REPORT
	VERIFY((GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) == LVS_REPORT);

	// Get the top and bottom row visible
	row = GetTopIndex();

	int bottom = row + GetCountPerPage();

	if( bottom > GetItemCount() )
		bottom = GetItemCount();
	

	// Get the number of columns
	int nColumnCount = pHeader->GetItemCount();

	
	// Loop through the visible rows
	for( ;row <= bottom;row++)
	{
		// Get bounding rect of item and check whether point falls in it.
		CRect rect;

		GetItemRect( row, &rect, LVIR_BOUNDS );

		if( rect.PtInRect(point) )
		{
			// Now find the column
			for( colnum = 0; colnum < nColumnCount; colnum++ )
			{
				HDITEM hti;
				hti.mask = HDI_WIDTH;
				pHeader->GetItem(colnum, &hti);

				int colwidth = hti.cxy;


				if( point.x >= rect.left 

					&& point.x <= (rect.left + colwidth ) )
				{

					if( col ) *col = colnum;
						return row;
				}
				rect.left += colwidth;
			}

		}

	}
	return -1;

}



void CListBaseCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonUp(nFlags, point);

	ReleaseCapture();
}
void CListBaseCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{		
	SetCapture();

	CListCtrl::OnLButtonDown(nFlags, point);
	int row,col;

	InvalidateCell( m_nCurRow, m_nCurCol );

	if( ( row = HitTestEx( point, &col )) != -1 )
	{

		int nOldRow = m_nCurRow;
		int nOldCol = m_nCurCol;

		m_nCurRow = row;
		m_nCurCol = col;

		if( m_nCurCol != nOldCol )
			FireCurentColumnChange( col );

		if( m_nCurRow != nOldRow )
			FireCurentRowChange( row );


        SetItemState( row, LVIS_SELECTED | LVIS_FOCUSED , 
                           LVIS_SELECTED | LVIS_FOCUSED);

		InvalidateCell( m_nCurRow, m_nCurCol );

		if( GetColumnType(m_nCurCol) == TYPE_COLOR )
		{
	
			CRect cellRect;
			GetCellRect(m_nCurRow, m_nCurCol, LVIR_BOUNDS, cellRect );

			CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

			// Get size of dropdown arrow
			int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
			int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
			CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));
			
			cellRect.left = cellRect.right - ArrowSize.cx;
			if( cellRect.PtInRect( point ) && !IsColumnReadOnly(m_nCurCol))
			{
				PaintColorPicer();
				m_ColourPicker.ShowPopup( );
			}
		}
		else
		if( GetColumnType(m_nCurCol) == TYPE_FONT )
		{
	
			CRect cellRect;
			GetCellRect(m_nCurRow, m_nCurCol, LVIR_BOUNDS, cellRect );

			cellRect.left = cellRect.right - 20;
			
			if( cellRect.PtInRect( point ) && !IsColumnReadOnly(m_nCurCol))
			{
				DoEdit();
			}
		}
		else
		if( GetColumnType(m_nCurCol) == TYPE_LOGICAL )
		{	
			DoEdit();
		}
		else if( GetColumnType(m_nCurCol) == TYPE_VALUE_COMBO )
		{
			CRect cellRect;
			GetCellRect(m_nCurRow, m_nCurCol, LVIR_BOUNDS, cellRect );

			CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

			// Get size of dropdown arrow
			int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
			int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
			CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));
			
			cellRect.left = cellRect.right - ArrowSize.cx;
			if( cellRect.PtInRect( point ) && !IsColumnReadOnly(m_nCurCol))
				SetValuePickerToCell( m_nCurCol, m_nCurRow );
		}
		else if( GetColumnType(m_nCurCol) == TYPE_SHADING )
		{
	
			CRect cellRect;
			GetCellRect(m_nCurRow, m_nCurCol, LVIR_BOUNDS, cellRect );

			CSize MarginSize(::GetSystemMetrics(SM_CXEDGE), ::GetSystemMetrics(SM_CYEDGE));

			// Get size of dropdown arrow
			int nArrowWidth = max(::GetSystemMetrics(SM_CXHTHUMB), 5*MarginSize.cx);
			int nArrowHeight = max(::GetSystemMetrics(SM_CYVTHUMB), 5*MarginSize.cy);
			CSize ArrowSize(max(nArrowWidth, nArrowHeight), max(nArrowWidth, nArrowHeight));
			
			cellRect.left = cellRect.right - ArrowSize.cx;
			if( cellRect.PtInRect( point ) && !IsColumnReadOnly(m_nCurCol))
			{
				PaintShadingPicker();
				m_ShadingPicker.ShowPopup( );
			}
		}
	}
}


void CListBaseCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL b_needRedraw = TRUE;

	InvalidateCell( m_nCurRow, m_nCurCol );

	switch(nChar)	
	{
		case VK_LEFT:			
			if (m_nCurCol >= 1)
			{
				m_nCurCol--;
				FireCurentColumnChange( m_nCurCol );
			}		

			else
				b_needRedraw = FALSE;
			break;
			
		case VK_RIGHT:
			if (m_nCurCol<(m_nColCount-1))
			{
				m_nCurCol++;
				FireCurentColumnChange( m_nCurCol );
			}
			else
				b_needRedraw = FALSE;
			break;

		case VK_UP:
			if (m_nCurRow >= 1)
			{
				m_nCurRow--;		
				FireCurentRowChange( m_nCurRow );
			}

			else
				b_needRedraw = FALSE;
			break;

		case VK_DOWN:
			if (m_nCurRow<(m_nRowCount-1))
			{
				m_nCurRow++;		
				FireCurentRowChange( m_nCurRow );
			}				

			else
				b_needRedraw = FALSE;			
			break;


        case VK_F2:
        	DoEdit();
			break;
					
		default:
			b_needRedraw = FALSE;
			break;
	}

	if (b_needRedraw)
	{					
		CListCtrl* pThis = NULL;
            SetItemState( m_nCurRow, LVIS_SELECTED | LVIS_FOCUSED , 
                               LVIS_SELECTED | LVIS_FOCUSED);

			InvalidateCell( m_nCurRow, m_nCurCol );
		
	}

	CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CListBaseCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	CListCtrl::OnLButtonDblClk(nFlags, point);

	int row,col;
	
	if( ( row = HitTestEx( point, &col )) != -1 )
	{
		if(m_pParent)
			m_pParent->FireDblClick( m_nCurCol,  m_nCurRow );	

		DoEdit();	
	}
}


BOOL CListBaseCtrl::DoEdit()
{	
	datatype_e CellDatatype = GetColumnType( m_nCurCol );

	if ((m_nCurCol < m_columns.GetSize( )) &&(m_nCurCol>=0))
	{
		if( m_columns[m_nCurCol] )
			if( m_columns[m_nCurCol]->IsReadOnly( ) )
				return FALSE;
	}
	

	if( m_pParent )
		m_pParent->SetModifiedFlag( );

	CBaseCell* pCell = NULL;
	pCell = GetCell( m_nCurRow, m_nCurCol );

	if(!pCell) return FALSE;		


	SetFocus( );
					
	

	switch(CellDatatype)
	{
		case TYPE_SUBJECT:								
		{	
			m_b_in_editMode = TRUE;
			EditLabel(m_nCurRow);
			break;
		}
		case TYPE_VALUE:								
//		case TYPE_VALUE_COMBO:
		{	
			m_b_in_editMode = TRUE;
			oldNameEdit=pCell->GetAsString(TYPE_VALUE);
			EditLabel(m_nCurRow);
			break;
		}

		case TYPE_LOGICAL:
			pCell->SetLogical(!pCell->GetAsLogical());
			InvalidateCell( m_nCurRow, m_nCurCol );
			FireDataChange( m_nCurCol, m_nCurRow );
			break;			
			
		

		case TYPE_FONT:
		{

			LOGFONT lf = {0};

			memcpy( (void*)&lf, (void*)pCell->GetAsFont(), sizeof( LOGFONT ) );

			if( pCell->m_nPointFontSize )
			{

				HDC hdc = ::CreateDC( _T( "DISPLAY" ), 0, 0, 0 );

				lf.lfHeight = -MulDiv( pCell->m_nPointFontSize / 10, GetDeviceCaps( hdc , LOGPIXELSY), 72 );
				lf.lfWidth = 0;

				::DeleteDC( hdc );

			}

			CFontDialog m_dlg( &lf );
			m_dlg.m_cf.rgbColors = pCell->m_crFontColor;


			if( m_pParent && m_pParent->GetUseSimpleFontType() )
				m_dlg.m_cf.Flags &= ~CF_EFFECTS;
	
			if( m_dlg.DoModal( ) == IDOK)
			{
				m_dlg.GetCurrentFont( &lf );				
				pCell->SetFont( lf );
				pCell->m_nPointFontSize = m_dlg.m_cf.iPointSize;
				pCell->m_crFontColor = m_dlg.m_cf.rgbColors;

				//ReMeasureItem();
				
				InvalidateCell( m_nCurRow, m_nCurCol );
				
				FireDataChange( m_nCurCol, m_nCurRow );

			}
		
			
			break;
		}

		default:
			break;
	}

	return TRUE;
}


datatype_e CListBaseCtrl::GetColumnType( int nCol )
{
	if ((nCol < m_columns.GetSize( )) &&(nCol>=0))
	{
		if(m_columns[m_nCurCol])
			return m_columns[nCol]->GetDataType( );
		else
			return TYPE_NOT_SUPPORTED;
	}else
		return TYPE_NOT_SUPPORTED;

}


CBaseCell* CListBaseCtrl::GetCell(int iRow, int iCol)
{	

	if ((iCol < m_columns.GetSize( )) && (iCol >=0))
	{
		CBaseCell* pCell = NULL;

		if(m_columns[iCol])
			return m_columns[iCol]->GetCell(iRow);		
	}
	return NULL;
}


void CListBaseCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult=1;	

	if( !m_b_in_editMode ) return;

	LV_DISPINFO* pDispInfo=(LV_DISPINFO*)pNMHDR;

	CRect rcItem;	

	GetCellRect(m_nCurRow, m_nCurCol, LVIR_BOUNDS, rcItem);
	
	HWND hWnd=(HWND)SendMessage(LVM_GETEDITCONTROL);
	ASSERT(hWnd!=NULL);
	VERIFY(m_LVEdit.SubclassWindow(hWnd));
			
	m_LVEdit.BuildAppearance(	((COleControl*)GetParent())->TranslateColor( m_clrEditTextColor ), 
								((COleControl*)GetParent())->TranslateColor( m_clrEditBackColor ) );

	m_LVEdit.m_x=rcItem.left;
	m_LVEdit.m_y=rcItem.top;
	m_LVEdit.m_width  = rcItem.Width( );
	m_LVEdit.m_height = rcItem.Height( );

	CBaseCell* pCell = NULL;
	pCell = GetCell( m_nCurRow, m_nCurCol );

	if(!pCell) return;	
	
	datatype_e ColumnType = GetColumnType( m_nCurCol );
	
	if(ColumnType == TYPE_SUBJECT)
		m_LVEdit.SetWindowText( pCell->GetAsSubject() );

	if(ColumnType == TYPE_VALUE || ColumnType == TYPE_VALUE_COMBO )
		m_LVEdit.SetWindowText( pCell->GetAsValue() );

	*pResult=0;
}

void CListBaseCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_b_in_editMode = FALSE;
	
	LV_DISPINFO* pDispInfo=(LV_DISPINFO*)pNMHDR;

	*pResult=1;
	
	CString sEdit=pDispInfo->item.pszText;

	if(!sEdit.IsEmpty())
	{
		CBaseCell* pCell = NULL;
		pCell = GetCell( m_nCurRow, m_nCurCol );

		if(!pCell) return;	


		datatype_e ColumnType = GetColumnType( m_nCurCol );

		if(ColumnType == TYPE_SUBJECT)
		{
			pCell->SetSubject(sEdit);
			FireDataChange( m_nCurCol, m_nCurRow );
		}

		if(ColumnType == TYPE_VALUE || ColumnType == TYPE_VALUE_COMBO )
		{
			pCell->SetValue(sEdit);
			FireDataChange( m_nCurCol, m_nCurRow );
		}
	}

	VERIFY(m_LVEdit.UnsubclassWindow()!=NULL);

	SetItemState(m_nCurRow,0,LVNI_FOCUSED|LVNI_SELECTED);

	*pResult=0;

	InvalidateCell( m_nCurRow, m_nCurCol );
}








//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//Column & Row operations

void CListBaseCtrl::AddColumn( CString strCaption, datatype_e datatype, int nWidth, DWORD dwCaptionAlign)
{
	
	CBaseColumn* pColumn = new CBaseColumn;
	pColumn->SetListBase( this );
	pColumn->AddColumn( m_nColCount, datatype, strCaption,  this, nWidth, dwCaptionAlign  );	
	m_columns.Add( pColumn );
	
	m_nColCount++;
	
}

void CListBaseCtrl::AddRow( int nRow )	
{	
	if( m_columns.GetSize() < 1 )
		return;

	for(int i=0;i<m_columns.GetSize( );i++)
	{		
		m_columns[i]->AddRow( i, m_nRowCount , this );
	}

	bool bNeedMove = ( nRow >= 0 && nRow < m_nRowCount );

	m_nRowCount++;

	if( bNeedMove )
	{
		for(int i=0;i<m_columns.GetSize( );i++)
		{
			CBaseColumn* pColumn = m_columns[i];
			int nRowCount = pColumn->m_columnValues.GetSize(); 
			if( nRowCount < 2 )
				continue;

			CBaseCell* pCellNew = pColumn->m_columnValues[ nRowCount - 1 ];

			for( int j=nRowCount-1;j>nRow;j-- )
			{				
				pColumn->m_columnValues[ j ] = pColumn->m_columnValues[ j - 1 ];
			}			

			pColumn->m_columnValues[ nRow ] = pCellNew;
		}

	}


	if( m_nCurRow >= m_nRowCount || m_nCurRow < 0 )
	{
		m_nCurRow = 0;
		if( m_nRowCount <=0 )
			m_nCurRow = -1;

		FireCurentRowChange( m_nCurRow );
	}

	if( GetSafeHwnd() )
	{
		Invalidate();
		UpdateWindow();
	}

	
}

void CListBaseCtrl::DeleteRow(short nRow)
{	
	for(int i=0;i<m_columns.GetSize();i++)
	{
		if(m_columns[i])
			m_columns[i]->DeleteRow( nRow );
	}

	DeleteItem( nRow );
		
	if(m_nRowCount > 0) m_nRowCount--;

	if( m_nCurRow >= m_nRowCount || m_nCurRow < 0 )
	{
		m_nCurRow = 0;		
		if( m_nRowCount <=0 )
			m_nCurRow = -1;
		FireCurentRowChange( m_nCurRow );
	}

	PaintColorPicer();

}



void CListBaseCtrl::ResetDefaults()
{

	DeleteListCtrlContext();
	DeleteDataContext();

	AddColumn( "Subject", TYPE_SUBJECT, 100, LVCFMT_CENTER );
	AddColumn( "Color", TYPE_COLOR,   100,   LVCFMT_CENTER );
	AddColumn( "Font", TYPE_FONT, 100, LVCFMT_CENTER );
	AddColumn( "Bool", TYPE_LOGICAL,   100,   LVCFMT_CENTER );

	AddRow();
	AddRow();
	AddRow();
	AddRow();
	AddRow();
	AddRow();
	
	//BuildControl( );

}

void CListBaseCtrl::DeleteDataContext()
{

	for(int i=0;i<m_columns.GetSize( );i++)
	{
		if( m_columns[i] )
			delete m_columns[i];
	}
	
	m_columns.RemoveAll();
	
	m_nCurRow = m_nCurCol = 0;
	m_nRowCount = m_nColCount = 0;

}



void CListBaseCtrl::DeleteListCtrlContext()
{
	if( GetSafeHwnd() )
	{
		DeleteAllItems();
		
		CHeaderCtrl* pHeader = NULL;
		pHeader = GetHeaderCtrl();
		
		int nColumnCount = 0;
		
		if( pHeader )
		{
			nColumnCount = pHeader->GetItemCount( );

			for( int i=nColumnCount-1;i>=0;i--)
				DeleteColumn( i );
		}		
	}

	m_nCurRow = m_nCurCol = 0;
	m_nRowCount = m_nColCount = 0;
	
}



void CListBaseCtrl::Serialize(CArchive& ar)
{
	//CListCtrl::Serialize( ar );	

	//Save column width
	if( ar.IsStoring( ) )
	{
		for( int i=0;i<m_columns.GetSize();i++ )
		{
			
			CHeaderCtrl* pHeader = GetHeaderCtrl();
			
			
			int nWidth = GetColumnWidth(i);
			if( m_columns[i] && pHeader)
			{
				HDITEM hti;
				hti.mask = HDI_WIDTH;
				pHeader->GetItem(i, &hti);

				m_columns[i]->SetColumnWidth( hti.cxy );
			}
		}	
	}

	
	if( ar.IsLoading() )
	{
		DeleteListCtrlContext();
		DeleteDataContext();
	}


	long	nVersion = 7;


	

	if( ar.IsStoring( ) )
	{	
		ar << nVersion;
		m_imageList.Write( &ar );

		ar << m_nCurRow;
		ar << m_nCurCol;
		ar << m_nRowCount;
		ar << m_nColCount;		

		m_columns.Serialize( ar );

		ar << m_clrEditTextColor;
		ar << m_clrEditBackColor;

        ar << m_clrBack;

		ar << m_clrActiveCellBackColor;
		ar << m_clrActiveCellTextColor;

		

	}
	else	
	{		
		ar >> nVersion;

		if( nVersion >= 6 )
		{
			m_imageList.DeleteImageList( );
			m_imageList.Read( &ar );			
		}


		ar >> m_nCurRow;
		ar >> m_nCurCol;
		ar >> m_nRowCount;
		ar >> m_nColCount;
		
		if( nVersion >= 3 )
		{
			m_columns.Serialize( ar );	

			for( int i=0;i<m_columns.GetSize();i++ )
			{
				m_columns[i]->SetListBase( this );	
			}

		}
		else
			ResetDefaults();


		if( nVersion >= 4 )
		{
			ar >> m_clrEditTextColor;
			ar >> m_clrEditBackColor;
		}

		if( nVersion >= 5 )
		{
			ar >> m_clrBack;			
		}

		if( nVersion >= 7 )
		{
			ar >> m_clrActiveCellBackColor;
			ar >> m_clrActiveCellTextColor;
		}
		

		if( m_bWasCreated ) // Loading after Creation 
		{
			BuildControl( );
		} //else BuildControl call OnCreate


		//PaintColorPicer();
	}

}

BOOL CListBaseCtrl::BuildControl()
{

	for(int i=0;i<(int)m_nColCount;i++)
	{
		if( (i < m_columns.GetSize()) && (i >= 0))
		{
			if(m_columns[i])
				m_columns[i]->AddColumnToListCtrl( i, this );
			
			for(int j=0;j<m_nRowCount;j++)			
				if(m_columns[i])
					m_columns[i]->AddRowToListCtrl( i, j, this );
			
		}else
		{
			TRACE("Index out of range(m_columns).Point:BuildControl()");
		}
		
	}

	if( m_nCurCol < 0 || m_nCurCol > m_nColCount )
	{
		m_nCurCol = 0;		

		if( m_nColCount <=0 )
			m_nCurCol = -1;				
	}

	FireCurentColumnChange( m_nCurCol );

	if( m_nCurRow >= m_nRowCount || m_nCurRow < 0 )
	{
		m_nCurRow = 0;
		if( m_nRowCount <=0 )
			m_nCurRow = -1;		
	}	

	FireCurentRowChange( m_nCurRow );


	SetBkColor( m_pParent->TranslateColor( m_clrBack ) );

	return TRUE;
}


void CListBaseCtrl::SetColorPickerToCell( short nColumn, short nRow )
{
	CRect rcItem;
	GetCellRect( nRow, nColumn, LVIR_BOUNDS, rcItem );
				
	m_ColourPicker.MoveWindow( rcItem.left, 
								rcItem.top, 
								rcItem.Width( ), 
								rcItem.Height( ) );
	m_ColourPicker.ShowWindow( SW_SHOWNA );
}

void CListBaseCtrl::SetValuePickerToCell( short nColumn, short nRow )
{
	CRect rcItem;
	GetCellRect( nRow, nColumn, LVIR_BOUNDS, rcItem );

	m_Combo.MoveWindow( rcItem.left, 
								rcItem.top - 5, 
								0, 
								100 );

	while( m_Combo.GetCount() )
		m_Combo.DeleteString( 0 );

	m_Combo.Clear();

	CBaseCell* pCell = 0;
	pCell = GetCell( nRow, nColumn );
	if( pCell )
	{
		int nSz = pCell->GetComboSize();

		if( nSz )
		{
			for( int i = 0; i < nSz; i++ )
				m_Combo.AddString( pCell->GetComboValue( i ) );
		}
		else
			m_Combo.AddString( "" );

		m_Combo.SetDroppedWidth( rcItem.Width() );


		m_Combo.ShowDropDown();
		m_Combo.ShowWindow( SW_SHOWNA );
	}


}

void CListBaseCtrl::HideColorPicer()
{
	m_ColourPicker.ShowWindow( SW_HIDE );
}

void CListBaseCtrl::PaintColorPicer()
{
	if( !m_ColourPicker.GetSafeHwnd() ) return;

	CBaseCell* pCell = NULL;
	pCell = GetCell( m_nCurRow, m_nCurCol );

	BOOL bFindCell = FALSE;

	if( IsValidColumnIndex(m_nCurCol) )
	{
		if( m_columns[m_nCurCol]->GetDataType( ) == TYPE_COLOR )
		{
			if(IsColumnReadOnly(m_nCurCol))
				return;
			if( pCell )			
				bFindCell = TRUE;			
		}
	}

	if( bFindCell )
	{
		m_ColourPicker.SetBkColour( pCell->GetAsColor( ) );
		SetColorPickerToCell( m_nCurCol, m_nCurRow );
	}
	else
		HideColorPicer();
}



void CListBaseCtrl::SetShadingPickerToCell( short nColumn, short nRow )
{
	CRect rcItem;
	GetCellRect( nRow, nColumn, LVIR_BOUNDS, rcItem );
				
	m_ShadingPicker.MoveWindow( rcItem.left, 
								rcItem.top, 
								rcItem.Width( ), 
								rcItem.Height( ) );
	m_ShadingPicker.ShowWindow( SW_SHOWNA );
}

void CListBaseCtrl::HideShadingPicker()
{
	m_ShadingPicker.ShowWindow( SW_HIDE );
}

void CListBaseCtrl::PaintShadingPicker()
{
	if( !m_ShadingPicker.GetSafeHwnd() ) return;

	CBaseCell* pCell = NULL;
	pCell = GetCell( m_nCurRow, m_nCurCol );

	BOOL bFindCell = FALSE;

	if( IsValidColumnIndex(m_nCurCol) )
	{
		if( m_columns[m_nCurCol]->GetDataType( ) == TYPE_SHADING )
		{
			if(IsColumnReadOnly(m_nCurCol))
				return;
			if( pCell )			
				bFindCell = TRUE;			
		}
	}
	if( bFindCell )
	{
		m_ShadingPicker.SetBkColour( pCell->GetAsColor( ) );
		m_ShadingPicker.SetIndexShading(pCell->GetShadingType());
		SetShadingPickerToCell( m_nCurCol, m_nCurRow );
	}
	else
		HideShadingPicker();
}

LRESULT CListBaseCtrl::OnSelEndOK(WPARAM lParam, LPARAM wParam)
{
	CBaseCell* pCell = NULL;
	pCell = GetCell( m_nCurRow, m_nCurCol );

	if( IsValidColumnIndex(m_nCurCol) )
	{
		if( m_columns[m_nCurCol]->GetDataType( ) == TYPE_COLOR )
		{
			if( pCell )			
			{
				// vanek - 26.08.2003
				COLORREF crNew = m_ColourPicker.GetBkColour();
				if( crNew != pCell->GetAsColor() )
				{
					pCell->SetColor( m_ColourPicker.GetBkColour() );
					FireDataChange( m_nCurCol, m_nCurRow );
				}
				
				/*if( m_pParent )
				{
					if( m_pParent->IsAutoUpdate() )
						FireDataChange( m_nCurCol, m_nCurRow );
					m_pParent->SetModifiedFlag( );
				}*/
			}			
		}
		else
			if( m_columns[m_nCurCol]->GetDataType( ) == TYPE_SHADING )
			{
 				if( pCell )			
				{
					int newShading=m_ShadingPicker.GetIndexShading();
					if(newShading!=pCell->GetShadingType())
					{
						pCell->SetShadingType(newShading);
						FireDataChange( m_nCurCol, m_nCurRow );
					}
				}			
			}
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////
//
//	Properties
//
///////////////////////////////////////////////////////////////////////////
BOOL CListBaseCtrl::UseGrid()
{
	ASSERT(m_pParent);
	return m_pParent->UseGrid( );
}

BOOL CListBaseCtrl::UseFullRowSelect()
{
	ASSERT(m_pParent);
	return m_pParent->UseFullRowSelect( );
}


BOOL  CListBaseCtrl::IsEnableResizeColumns( )
{
    ASSERT(m_pParent);
	return m_pParent->IsEnableResizeColumns( );    
}


BOOL CListBaseCtrl::IsValidColumnIndex(short nColumn)
{
	if( (nColumn < m_columns.GetSize()) && (nColumn >= 0) )
	{
		if( m_columns[nColumn] )
			return TRUE;
		else
			return FALSE;		
	}
	else
		return FALSE;
}

OLE_COLOR CListBaseCtrl::GetColumnBackColor(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{
		return m_columns[nColumn]->GetColumnBackColor( );
	}else return (OLE_COLOR)RGB(0, 0, 0);
}

OLE_COLOR CListBaseCtrl::GetColumnForeColor(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{
		return m_columns[nColumn]->GetColumnForeColor( );
	}else return (OLE_COLOR)RGB(0, 0, 0);
}

CString CListBaseCtrl::GetColumnCaption(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{
		return m_columns[nColumn]->GetColumnCaption( );
	}else return _T("");
}

short CListBaseCtrl::_GetColumnType(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{
		return (short)m_columns[nColumn]->GetColumnType( );
	}else return (short)-1;
}

short CListBaseCtrl::GetColumnAlign(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{
		return (short)m_columns[nColumn]->GetColumnAlign( );
	}else return (short)-1;
}

short CListBaseCtrl::GetColumnWidth(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{
		return (short)m_columns[nColumn]->GetColumnWidth( );
	}else return (short)-1;
}

short CListBaseCtrl::GetColumnCount()
{
	return (short)m_nColCount;
}


void CListBaseCtrl::SetColumnAlign(short nColumn, short nAlign)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetColumnAlign( nAlign );
	}
}

void CListBaseCtrl::SetColumnBackColor(short nColumn, OLE_COLOR color)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetColumnBackColor( color );
	}
}

void CListBaseCtrl::SetColumnForeColor(short nColumn, OLE_COLOR color)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetColumnForeColor( color );
	}
}

void CListBaseCtrl::SetColumnType(short nColumn, short nType)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetColumnType( nType );
	}
}

void CListBaseCtrl::SetColumnWidth(short nColumn, short nWidth)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetColumnWidth( nWidth );
		CHeaderCtrl* pHeader = GetHeaderCtrl();						
		
		if( pHeader)
		{
			HDITEM hti;
			hti.mask = HDI_WIDTH;
			pHeader->GetItem( nColumn, &hti );
			hti.cxy = nWidth;
			pHeader->SetItem( nColumn, &hti );
		}
	}
}

void CListBaseCtrl::SetColumnCaption(short nColumn, CString strCaption)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetColumnCaption( strCaption );
		LVCOLUMN pColumn;
		ZeroMemory( &pColumn, sizeof(pColumn) );
		GetColumn( nColumn, &pColumn );

		pColumn.mask = LVCF_TEXT;
		
		TCHAR pStr[255];

		_tcscpy(pStr, strCaption);

		pColumn.pszText = pStr;
		SetColumn( nColumn, &pColumn );
	}
}

void CListBaseCtrl::_AddColumn()
{
	

	AddColumn("Value", TYPE_VALUE, 100, LVCFMT_CENTER);	

	if( !m_columns[m_nColCount-1] ) return;

	for(int i=0;i<m_nRowCount;i++)
	{
		m_columns[m_nColCount-1]->AddRow( m_nColCount-1, i , this );
	}

	if( m_nCurCol < 0 || m_nCurCol > m_nColCount )
	{
		m_nCurCol = 0;
		FireCurentColumnChange( m_nCurCol );
	}

}


void CListBaseCtrl::_DeleteColumn( short nColumn )
{
	if( IsValidColumnIndex(nColumn) )
	{
		delete m_columns[nColumn];
		m_columns.RemoveAt( nColumn );
		m_nColCount--;
		if( m_nCurCol >= m_nColCount )
		{
			m_nCurCol = 0;
			FireCurentColumnChange( m_nCurCol );			
		}


		DeleteColumn( nColumn );

		if(m_nColCount<=0){
			m_nColCount = 0;
			m_nRowCount = 0;
			m_nCurCol = -1;
			m_nCurRow = -1;
			FireCurentColumnChange( m_nCurCol );
			FireCurentRowChange( m_nCurRow );
			
			DeleteAllItems( );

		}

	}
}




BOOL CListBaseCtrl::GetDisplayLogicalAsText(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{
		return m_columns[nColumn]->GetDisplayLogicalAsText( );
	}

	return FALSE;
}

void CListBaseCtrl::SetDisplayLogicalAsText(short nColumn, BOOL bValue)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetDisplayLogicalAsText( bValue );
	}
}

BOOL CListBaseCtrl::IsColumnUseIcon(short nColumn)
{	if( IsValidColumnIndex(nColumn) )
	{
		return m_columns[nColumn]->IsColumnUseIcon( );
	}	
	return FALSE;
}

void CListBaseCtrl::SetColumnUseIcon(short nColumn, BOOL bValue)
{
	if( IsValidColumnIndex(nColumn) )
	{
		m_columns[nColumn]->SetColumnUseIcon( bValue );
	}
}


LPLOGFONT CListBaseCtrl::GetFont( short nColumn )
{
	if( IsValidColumnIndex(nColumn) )
	{			
		return m_columns[nColumn]->GetDefFont( );		
		
	}
	return NULL;
}

void CListBaseCtrl::SetFont( short nColumn, LPLOGFONT lpLF )
{
	if( IsValidColumnIndex(nColumn) )
	{					
		m_columns[nColumn]->SetDefFont( lpLF );		
		ReMeasureItem();
		//EnableScrollBar(SB_VERT);
		//ShowScrollBar(SB_VERT,true);
		//m_listCtrl.EnableScrollBar(SB_VERT,);
		//ShowScrollBar(SB_HORZ,false);
	}
}

void CListBaseCtrl::SetColumnLogicalIcon( short nColumn, CString &strFileName, BOOL b_forValue )
{
	if( IsValidColumnIndex(nColumn) )
	{					
		m_columns[nColumn]->SetIconFileNameForLogical( strFileName, b_forValue );
	}

}

short CListBaseCtrl::CurColumnMove(short nDirection)
{
	int nDelta;
	if( nDirection>0 ) nDelta = 1;
	
	if( nDirection<0 ) nDelta = -1;
	
	if( (m_nCurCol + nDelta)>=0 && (m_nCurCol + nDelta)<m_nColCount )
	{
		m_nCurCol += nDelta;		
		FireCurentColumnChange( m_nCurCol );
	}	
	

	return m_nCurCol;
}

void CListBaseCtrl::SetColumnReadOnly(short nColumn, BOOL bValue)
{
	if( IsValidColumnIndex(nColumn) )
	{					
		m_columns[nColumn]->SetReadOnly( bValue );
	}
}

BOOL CListBaseCtrl::IsColumnReadOnly(short nColumn)
{
	if( IsValidColumnIndex(nColumn) )
	{					
		return m_columns[nColumn]->IsReadOnly( );
	}

	return TRUE;

}


///////////////////////////////////////////////////////////
OLE_COLOR CListBaseCtrl::GetEditBackColor()
{
	return m_clrEditBackColor;
}

///////////////////////////////////////////////////////////
void CListBaseCtrl::SetEditBackColor(OLE_COLOR nNewValue)
{
	m_clrEditBackColor = nNewValue;
}

///////////////////////////////////////////////////////////
OLE_COLOR CListBaseCtrl::GetEditTextColor()
{
	return m_clrEditTextColor;
}

///////////////////////////////////////////////////////////
void CListBaseCtrl::SetEditTextColor(OLE_COLOR nNewValue)
{
	m_clrEditTextColor = nNewValue;
}

///////////////////////////////////////////////////////////
OLE_COLOR CListBaseCtrl::GetActiveCellBackColor()
{
	return m_clrActiveCellBackColor;
}

///////////////////////////////////////////////////////////
void CListBaseCtrl::SetActiveCellBackColor(OLE_COLOR nNewValue)
{
	m_clrActiveCellBackColor = nNewValue;
}

///////////////////////////////////////////////////////////
OLE_COLOR CListBaseCtrl::GetActiveCellTextColor()
{
	return m_clrActiveCellTextColor;
}

///////////////////////////////////////////////////////////
void CListBaseCtrl::SetActiveCellTextColor(OLE_COLOR nNewValue)
{
	m_clrActiveCellTextColor = nNewValue;
}


///////////////////////////////////////////////////////////
void CListBaseCtrl::ForceEditCell( short nColumn, short nRow )
{
	SetCurentRow( nRow );
	SetCurentCol( nColumn );

	bForceEdit=false;

	int nrow=0;
	nrow=GetRowCount();
	if(nrow>0)
	{
		CBaseCell* pCell = NULL;
		CString s, bs;
		pCell = GetCell( nRow, m_nCurCol );
		bs=pCell->GetAsString(TYPE_VALUE);
		for(int i=0; i<nrow;i++)
		{
			pCell = GetCell( i, m_nCurCol );
			if(!pCell) return;	
			s=pCell->GetAsString(TYPE_VALUE);
			if(s==bs && i!=nRow)
				bForceEdit=true;
		}
	}
	if(!bForceEdit)
		DoEdit();
	//FireCurentRowChange( nRow );
	//bForceEdit=false;
	FireDataChange(nColumn, nRow);
}

///////////////////////////////////////////////////////////
//
//
//		Events
//
//
///////////////////////////////////////////////////////////

void CListBaseCtrl::FireCurentColumnChange(short nColumn)
{
	if(m_pParent)
	{	
		//PaintColorPicer();
		m_pParent->SetModifiedFlag( );
		m_pParent->FireCurentColumnChange( nColumn );	
	}
}

void CListBaseCtrl::FireCurentRowChange(short nRow)
{
	if(m_pParent)
	{
		//PaintColorPicer();
		m_pParent->SetModifiedFlag( );
		m_pParent->FireCurentRowChange( nRow );
	}
}

void CListBaseCtrl::FireDataChange(short nColumn, short nRow)
{
	if(m_pParent)
	{
		
		m_pParent->SetModifiedFlag( );
		m_pParent->FireDataChange( nColumn, nRow );	


		/*
		if( IsValidColumnIndex(nColumn) )
		{					
			CBaseCell* pCell = NULL;
			pCell = GetCell( nRow, nColumn );

			if( pCell )
			{
				if( m_pParent->IsAutoUpdate() ) 
					m_pParent->WriteToShellData( pCell, 
							m_columns[nColumn]->GetDataType( ) );
			}	
		}
		*/
		if(bForceEdit)
		{
			bForceEdit=false;
			CBaseCell* pCell = NULL;
			pCell = GetCell( m_nCurRow, m_nCurCol );
			if(!pCell) return;	
			datatype_e ColumnType = GetColumnType( m_nCurCol );
			if(ColumnType == TYPE_SUBJECT)
			{
				pCell->SetSubject(oldNameEdit);
			}
			if(ColumnType == TYPE_VALUE || ColumnType == TYPE_VALUE_COMBO )
			{
				pCell->SetValue(oldNameEdit);
			}
			SetItemState(m_nCurRow,0,LVNI_FOCUSED|LVNI_SELECTED);
			InvalidateCell( m_nCurRow, m_nCurCol );
			FireCurentRowChange(m_nCurRow);
		}
	}	
}



void CListBaseCtrl::SetCurentRow(short nRow)
{
	if (nRow >= 0 && nRow < m_nRowCount)
	{
		m_nCurRow = nRow;
		//FireCurentRowChange( m_nCurRow );

		EnsureVisible( m_nCurRow, 0 );
		if( ::IsWindow( GetSafeHwnd() ) )
			Invalidate();

		/*
		int nMaxHeight = GetMaxCellHeight() + 3 * VT_LIST_SELECTION_ALIGN;
		if( nMaxHeight < VT_LIST_ICON_SIZE + 2)  
			nMaxHeight = VT_LIST_ICON_SIZE + 2;

		
		CSize size( 0, nMaxHeight*m_nCurRow );
		Scroll( size );

		if( ::IsWindow( GetSafeHwnd() ) )
			Invalidate();
			*/
	}		

}

void CListBaseCtrl::SetCurentCol(short nCol)
{
	if (nCol >= 0 && nCol < m_nColCount)
	{
		m_nCurCol = nCol;
		//FireCurentColumnChange( m_nCurCol );

		if( ::IsWindow( GetSafeHwnd() ) )
			Invalidate();

	}		
}


/////////////////////////////////////////////////////////////////////////////////////
OLE_COLOR CListBaseCtrl::GetListBkColor()
{
	return m_clrBack;
}

/////////////////////////////////////////////////////////////////////////////////////
void CListBaseCtrl::SetListBkColor(OLE_COLOR nNewValue)
{
	m_clrBack = nNewValue;	
	if( GetSafeHwnd() )
		Invalidate();
}


void CListBaseCtrl::OnCloseupCombo() 
{
	m_Combo.ShowWindow( SW_HIDE );
	int nSel = m_Combo.GetCurSel();
	
	if( nSel != -1 )
	{
		CString str;
		m_Combo.GetLBText( nSel, str );

		if( !str.IsEmpty() )
		{
			CBaseCell* pCell = NULL;
			pCell = GetCell( m_nCurRow, m_nCurCol );

			if(!pCell) return;	

			datatype_e ColumnType = GetColumnType( m_nCurCol );

			if( ColumnType == TYPE_VALUE_COMBO )
			{
				pCell->SetValue( str );
				FireDataChange( m_nCurCol, m_nCurRow );
				InvalidateCell( m_nCurRow, m_nCurCol );
			}
		}
	}
}

OLE_COLOR CListBaseCtrl::GetCellBackColor(short nColumn, short nRow )
{
	if ( nColumn < m_columns.GetSize( ) && ( nColumn >= 0 ) )
	{
		if( m_columns[nColumn] )
		{
			CBaseCell *pCell = m_columns[nColumn]->GetCell( nRow );
			
			if( pCell )
				return pCell->GetBackColor();
		}
	}
	return 0;
}

OLE_COLOR CListBaseCtrl::GetCellForeColor(short nColumn, short nRow )
{
	if ( nColumn < m_columns.GetSize( ) && ( nColumn >= 0 ) )
	{
		if( m_columns[nColumn] )
		{
			CBaseCell *pCell = m_columns[nColumn]->GetCell( nRow );
			
			if( pCell )
				return pCell->GetForeColor();
		}
	}
	return 0;
}

void CListBaseCtrl::SetCellBackColor(short nColumn, short nRow, OLE_COLOR color)
{
	if ( nColumn < m_columns.GetSize( ) && ( nColumn >= 0 ) )
	{
		if( m_columns[nColumn] )
		{
			CBaseCell *pCell = m_columns[nColumn]->GetCell( nRow );
			
			if( pCell )
				pCell->SetBackColor( color );
		}
	}
}

void CListBaseCtrl::SetCellForeColor(short nColumn, short nRow, OLE_COLOR color)
{
	if ( nColumn < m_columns.GetSize( ) && ( nColumn >= 0 ) )
	{
		if( m_columns[nColumn] )
		{
			CBaseCell *pCell = m_columns[nColumn]->GetCell( nRow );
			
			if( pCell )
				pCell->SetForeColor( color );
		}
	}
}


// CListHeaderCtrl

bool CListHeaderCtrl::m_bLock = false;

IMPLEMENT_DYNAMIC(CListHeaderCtrl, CHeaderCtrl)
CListHeaderCtrl::CListHeaderCtrl()
{
	m_plist = 0;	// pointer to listvew control
	m_pMan = 0;
	m_bUseExt = false;
	//s_xy.SetPoint(0,0);
}

CListHeaderCtrl::~CListHeaderCtrl()
{
}


BEGIN_MESSAGE_MAP(CListHeaderCtrl, CHeaderCtrl)
	ON_WM_SETCURSOR()
	// [vanek]  SBT: 580 - 13.01.2004
	ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGINGA, OnHdnItemchanging)		
	ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGINGW, OnHdnItemchanging)		
	ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGEDA, OnHdnItemchanged)
	ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGEDW, OnHdnItemchanged)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnNMCustomdraw)
	ON_WM_CREATE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListHeaderCtrl message handlers

BOOL CListHeaderCtrl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if( !_is_resizing_columns( 0, 0, 0 ) )
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return TRUE;		
	}
	else
	    return __super::OnSetCursor(pWnd, nHitTest, message);
}

BOOL CListHeaderCtrl::OnHdnItemchanging(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	// [vanek] BT2000: 3713 - 16.03.04
	if( phdr->pitem && (phdr->pitem->mask & HDI_WIDTH) )
	{
		HDITEM hItem = {0};
		
		hItem.mask |= HDI_WIDTH;

		Header_GetItem( phdr->hdr.hwndFrom,  phdr->iItem,  &hItem );

        *pResult = _is_resizing_columns( phdr->hdr.hwndFrom, phdr->iItem, phdr->pitem->cxy -  hItem.cxy ) ? 0 : 1;
	}
	else
		*pResult = 0;

	return *pResult == 1 ? TRUE : FALSE;
}


void CListHeaderCtrl::_apply_header_resize( HANDLE dwFrom, int iItem, int nWidth )
{
	if( m_bLock || !m_plist )
		return;

	m_bLock = true;
	::SendMessage( m_plist->GetParent()->m_hWnd, WM_USER + 0x4554, MAKELONG( nWidth, iItem ), LPARAM( dwFrom ) );
	m_bLock = false;
}

BOOL CListHeaderCtrl::_is_resizing_columns( HANDLE dwFrom, int iItem, int nDir  )
{
    if( !m_plist )
		return TRUE;

	if( !m_bUseExt )
		return m_plist->IsEnableResizeColumns( );

	return m_plist->IsEnableResizeColumns( ) && ( ( dwFrom != 0 && nDir < 0 && !m_bLock ) ? ::SendMessage( m_plist->GetParent()->m_hWnd, WM_USER + 0x5445,  MAKELONG( -nDir, iItem ), LPARAM( dwFrom ) ) : 1 );
}


LRESULT CListHeaderCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if( message == WM_USER + 0x9876 )
	{
		m_bUseExt =  ::SendMessage( m_plist->GetParent()->m_hWnd, WM_USER + 0x9876,  0, 0 );
	}

	/*if( m_bUseExt && message == WM_WINDOWPOSCHANGED )
	{
		if( m_pMan && !m_pMan->Find( this ) )
		{
			LPOS lPos = m_pMan->GetFirstPos();

			while( lPos )
			{
				CHeaderCtrl *pCtrl = 0;
				lPos = m_pMan->GetNext( lPos, &pCtrl );

				WINDOWPOS *pwpos = (WINDOWPOS *)lParam;

				//
				s_xy.x=pwpos->x;
				s_xy.y=pwpos->y;

				RECT rcClient = {0};
				m_plist->GetClientRect( &rcClient );
			    
				WINDOWPOS wpos;
				
			    HD_LAYOUT layout;
  				layout.pwpos = &wpos;

				POINT ptlRptOrigin = { -pwpos->x, pwpos->y };

				rcClient.left -= (int)ptlRptOrigin.x;
				layout.prc = &rcClient;

 				pCtrl->Layout( &layout );

				RECT rcRect = {0};
				::GetWindowRect( pCtrl->m_hWnd, &rcRect );
				
				::ScreenToClient( m_plist->GetParent()->m_hWnd, (LPPOINT)&rcRect );
  				::ScreenToClient( m_plist->GetParent()->m_hWnd, (LPPOINT)&rcRect + 1 );

				
				
				::SetWindowPos( pCtrl->m_hWnd, 0,wpos.x+1, rcRect.top + wpos.y,
 							wpos.cx, wpos.cy, 0);
				::UpdateWindow(pCtrl->m_hWnd);
				
				
			}

		}
	}

	/*if( m_bUseExt && message == WM_WINDOWPOSCHANGING)
	{
		if( m_pMan && !m_pMan->Find( this ) )
		{
			LPOS lPos = m_pMan->GetFirstPos();

			while( lPos )
			{
				CHeaderCtrl *pCtrl = 0;
				lPos = m_pMan->GetNext( lPos, &pCtrl );
				::InvalidateRect( pCtrl->m_hWnd, 0, TRUE );
				::UpdateWindow(pCtrl->m_hWnd);
			}
		}
	}*/
	
	return CHeaderCtrl::WindowProc(message, wParam, lParam);
}



BOOL CListHeaderCtrl::OnHdnItemchanged(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;
 	if( !m_bUseExt  )
	{
		//m_plist->InvalidateRect( 0 );
		return false;
	}

	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	_apply_header_resize( phdr->hdr.hwndFrom, phdr->iItem, phdr->pitem->cxy );
	return false;
}

void CListHeaderCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	if( !m_bUseExt )
		return;

  	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	switch( pNMCD->dwDrawStage )
	{
		case CDDS_PREPAINT:
			*pResult = CDRF_NOTIFYITEMDRAW | CDRF_NOTIFYPOSTPAINT;
			break;
		case CDDS_ITEMPREPAINT:
	 		if( pNMCD->lItemlParam != ALL_BORDER )
			{
				::SetTextColor( pNMCD->hdc, GetSysColor( COLOR_BTNFACE ) );
				*pResult = CDRF_NOTIFYPOSTPAINT;
			}
			break;
		case CDDS_ITEMPOSTPAINT:
			{
				long lItem = (long)( pNMCD->dwItemSpec );

				RECT rcItem = pNMCD->rc;

				HPEN hPen = ::CreatePen( PS_SOLID, 3, GetSysColor( COLOR_BTNFACE ) );
				HPEN hold = (HPEN)::SelectObject( pNMCD->hdc, hPen );
				HBRUSH holdB = (HBRUSH)::SelectObject( pNMCD->hdc, GetStockObject( NULL_BRUSH ) );

				if( !( pNMCD->lItemlParam & LEFT_BORDER ) )
 					::Rectangle( pNMCD->hdc, rcItem.left, rcItem.top, rcItem.left, rcItem.bottom );

 				if( !( pNMCD->lItemlParam & RIGHT_BORDER ) )
 					::Rectangle( pNMCD->hdc, rcItem.right, rcItem.top, rcItem.right, rcItem.bottom );

      			if( !( pNMCD->lItemlParam & TOP_BORDER ) )
   					::Rectangle( pNMCD->hdc, rcItem.left, rcItem.top, rcItem.right - 1, rcItem.top + 1 );

				if( !( pNMCD->lItemlParam & BOTTOM_BORDER ) )
				{
  					::Rectangle( pNMCD->hdc, rcItem.left, rcItem.bottom - 1, rcItem.right - 1, rcItem.bottom  );

					HPEN hPen = ::CreatePen( PS_SOLID, 3, GetSysColor( COLOR_BTNSHADOW ) );
					HPEN hold = (HPEN)::SelectObject( pNMCD->hdc, hPen );

   					::MoveToEx( pNMCD->hdc,  rcItem.right - 2, rcItem.bottom, 0 );
					::LineTo( pNMCD->hdc,  rcItem.right - 2, rcItem.bottom );

					::SelectObject( pNMCD->hdc, hold );
					
					::DeleteObject( hPen );
				}

				HDITEM hItem = {0};

				char szText[MAX_PATH];
				
				hItem.mask |= HDI_TEXT;
				hItem.cchTextMax = MAX_PATH;
				hItem.pszText = szText;

				GetItem( lItem, &hItem );


				rcItem.right		-= 2;
				rcItem.left		+= 2;

	 			if( pNMCD->lItemlParam != ALL_BORDER )
				{
					HEADER_DRAW_PROP prop = {0};

					prop.hHdr	= m_hWnd;
					prop.iItem	= lItem;

					if( ::SendMessage( m_plist->GetParent()->m_hWnd, WM_USER + 0x7887, 0, LPARAM( &prop ) ) )
					{
						strcpy( szText, prop.szText );

						int h = rcItem.bottom - rcItem.top;

						rcItem.bottom = rcItem.top + prop.iGrCount * h;

						::SetTextColor( pNMCD->hdc, GetSysColor( COLOR_BTNTEXT ) );
						
						::OffsetRect( &rcItem, 0, -prop.iGrIndex * h + ( rcItem.bottom - rcItem.top ) / 2 - h / 2 );
					}
				}

				::DrawText( pNMCD->hdc, szText, strlen( szText ), &rcItem, DT_WORDBREAK | DT_CENTER | DT_END_ELLIPSIS | DT_VCENTER );

				::SelectObject( pNMCD->hdc, hold );
				::SelectObject( pNMCD->hdc, holdB );
				
				::DeleteObject( hPen );

				
				/*if(s_xy.x<0)
				{
					if(rcItem.left-2+s_xy.x<0 && rcItem.right+2+s_xy.x>0)
					{
						//	::MoveToEx( pNMCD->hdc,  rcItem.left-s_xy.x, rcItem.top, 0 );
						//	::LineTo( pNMCD->hdc,  rcItem.left-s_xy.x, rcItem.bottom );
						/*if( m_pMan && !m_pMan->Find( this ) )
						{
							LPOS lPos = m_pMan->GetFirstPos();
							while( lPos )
							{
								CHeaderCtrl *pCtrl = 0;
								lPos = m_pMan->GetNext( lPos, &pCtrl );
								CDC *hdc = pCtrl->GetWindowDC(); 
								
								RECT rcRect = {0};
								
								pCtrl->GetClientRect(&rcRect);
								HPEN hPen = ::CreatePen( PS_SOLID, 1, RGB(0,0,0) );
								HPEN hold = (HPEN)::SelectObject( hdc->m_hDC, hPen );
								hdc->MoveTo(-s_xy.x-1,rcRect.top);
								hdc->LineTo(-s_xy.x-1,rcRect.bottom);
								::SelectObject( hdc->m_hDC, hold );
								::DeleteObject( hPen );
								ReleaseDC(hdc); 
								pCtrl->UpdateWindow();
							}
						}*/
				//	}
				//}


				*pResult = CDRF_SKIPDEFAULT;
			}
			break;
		default:
			*pResult = 0;
	}
}

int CListHeaderCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CHeaderCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_bUseExt =  ::SendMessage( m_plist->GetParent()->m_hWnd, WM_USER + 0x9876,  0, 0 );

	return 0;
}

void CListBaseCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	/*if(m_headerCtrl)
	{
		m_headerCtrl.Invalidate();
	}*/
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CListBaseCtrl::PreSubclassWindow() 
{
    // TODO: Add your specialized code here and/or call the base class
    
	CListCtrl::PreSubclassWindow();
    CListCtrl::EnableToolTips(TRUE);    
}

/*int CListBaseCtrl::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
    int row, col;
    RECT cellrect;
    row = CellRectFromPoint(point, &cellrect, &col );
    
    if ( row == -1 ) 
        return -1;
    
	HWND m_ctrlHwnd = m_pParent->m_listCtrl.m_hWnd;
	pTI->hwnd = m_ctrlHwnd;//m_hWnd;
    pTI->uId = (UINT)((row << 10) + (col & 0x3ff) + 1);
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->rect = cellrect;
    return 0;//pTI->uId; 
}*/


// CellRectFromPoint    - Determine the row, col and bounding rect of a cell
// Returns        - row index on success, -1 otherwise
// point        - point to be tested.
// cellrect        - to hold the bounding rect
// col            - to hold the column index
int CListBaseCtrl::CellRectFromPoint(CPoint &point, RECT *cellrect, int *col) const
{
    int colnum;

    // Make sure that the ListView is in LVS_REPORT
    if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
        return -1;

    // Get the top and bottom row visible
    int row = GetTopIndex();
    int bottom = row + GetCountPerPage();
    if( bottom > GetItemCount() )
        bottom = GetItemCount();
    
    // Get the number of columns
    CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();
    
    // Loop through the visible rows
    for( ; row <= bottom; row++)
    {
        // Get bounding rect of item and check whether point falls in it.
        CRect rect;
        GetItemRect( row, &rect, LVIR_BOUNDS );
        if( rect.PtInRect(point) )
        {
            // Now find the column
            for( colnum = 0; colnum < nColumnCount; colnum++ )
            {
				int colwidth = CListCtrl::GetColumnWidth(colnum);
                if( point.x >= rect.left 
                    && point.x <= (rect.left + colwidth ) )
                {
                    RECT rectClient;
                    GetClientRect( &rectClient );
                    if( col ) *col = colnum;
                    rect.right = rect.left + colwidth;
                    
                    // Make sure that the right extent does not exceed
                    // the client area
                    if( rect.right > rectClient.right ) 
                        rect.right = rectClient.right;
                    *cellrect = rect;
                    return row;
                }
                rect.left += colwidth;
            }
        }
    }
    return -1;
}

BOOL CListBaseCtrl::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	
	b_tooltip=true;
	
	int nRow=CListCtrl::HitTest(xy_tooltip);
	
	if(m_pParent)
	{
		int row, col;
		RECT cellrect;
		row = CellRectFromPoint(xy_tooltip, &cellrect, &col );

		CString strResult;
		datatype_e datatype = CBaseColumn::ConvertFromShortToEnum( GetColumnType( col ) );
		CBaseCell* pCell = m_pParent->m_listCtrl.GetCell(row, col);
		if( pCell && datatype!=TYPE_LOGICAL)
		{
			CClientDC dc(this);

			strTipText = pCell->GetAsString( datatype ); 
			if(strTipText.GetLength()>0)
			{
				HFONT pFont = CreateFontIndirect( pCell->GetAsFont( ) );
				HFONT pOldFont = (HFONT)SelectObject( dc.m_hDC,pFont );
				CSize size;
				GetTextExtentPoint32(dc.m_hDC, strTipText, strTipText.GetLength(), &size);
				dc.SelectObject(pOldFont);
				if(size.cx > cellrect.right-cellrect.left)
					_mbstowcsz(pTTTW->szText, strTipText, sizeof(pTTTW->szText));
			}
		}
	}
	*pResult = 0;
	return TRUE;
}

void CListBaseCtrl::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	xy_tooltip=point;
	if(b_tooltip)
	{
		b_tooltip=false;
		return ;
	}
	CListCtrl::OnMouseMove(nFlags, point);
}


void CListBaseCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default

	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}