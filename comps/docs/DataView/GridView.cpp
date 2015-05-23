// GridView.cpp : implementation file
//

#include "stdafx.h"
#include "DataView.h"
#include "GridView.h"

#include "Utils.h"
#include "Classes5.h"
#include "TimeTest.h"
#include "NameConsts.h"

#include "Propbag.h"


class CNumberFixedCell : public CVTGridCell
{
public:
	CNumberFixedCell( long rowFirst = 0 )				{m_rowFirst = rowFirst;}
	virtual LPCTSTR  GetText()							{ m_strText.Format( "%d", m_row-m_rowFirst ); return m_strText;}
protected:
	long	m_rowFirst;
};


bool FormatValueFromCalcObject( ParameterContainer *pc, ICalcObject *pcalc, CString &strValue )
{
	CString	strFmt;
	double	val = 0.;

	if( !pc || !pc->pDescr )	return false;

	if( pcalc->GetValue( pc->pDescr->lKey, &val ) == S_OK )
	{
		strFmt = pc->pDescr->bstrDefFormat;
		strValue.Format( strFmt, val*pc->pDescr->fCoeffToUnits );
	}
	else
		strValue = "-";
	return true;
}

static UINT uDefFormat = DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;
/////////////////////////////////////////////////////////////////////////////
// CGridViewBase

template<> UINT AFXAPI HashKey(GuidKey& key)
{
	return key.Data1;
}

IMPLEMENT_DYNAMIC(CGridViewBase, CCmdTargetEx)

CGridViewBase::CGridViewBase()
{
	m_uViewMode = 0;
	m_strSection		= _T("");
	m_bShowResBar		= true;

	m_clrLinear = RGB(205, 255, 205);//	205, 255, 205 - linear// green
	m_clrAngle = RGB(205, 255, 255);//	205, 255, 255 - Angle//blue
	m_clrCounter = RGB(255, 205, 205);//	255, 205, 205 - counter//red
	m_clrUndefine = RGB(255, 255, 205);//	255, 205, 255 - undefined//pink
	m_Grid.SetTextBkColor(RGB(255, 255, 205));//	255, 255, 205 - Background //yellow

	m_pframeRuntime = 0;
	m_rowPrintResult = -1;
	m_lStartPrintResultBarPosition = 0;

	m_hAccel = 0;
}

CGridViewBase::~CGridViewBase()
{
	ASSERT( m_rows.GetSize() == 0 );
	ASSERT( m_cols.GetSize() == 0 );
	TRACE("CGridViewBase::~CGridViewBase()\n");
	ASSERT(m_selection.GetSize() == 0 );
}


void CGridViewBase::OnFinalRelease()
{
	SaveState();
	Clear();

	if (GetSafeHwnd())
		DestroyWindow();

	delete this;
	//CViewBase::OnFinalRelease();
}


void CGridViewBase::Clear()
{
	m_Grid.DeleteAllItems();
	_clear_selection();
}

BEGIN_INTERFACE_MAP(CGridViewBase, CViewBase)
	INTERFACE_PART(CGridViewBase, IID_IGridView,		Grid)
	INTERFACE_PART(CGridViewBase, IID_IMultiSelection,	Frame)
END_INTERFACE_MAP()

BEGIN_MESSAGE_MAP(CGridViewBase, CViewBase)
	//{{AFX_MSG_MAP(CGridViewBase)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_NOTIFY(GVN_SELCHANGING,	IDC_GRID_CTRL, OnSelChangingGrid)
	ON_NOTIFY(GVN_SELCHANGED,	IDC_GRID_CTRL, OnSelChangedGrid)
    ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_CTRL, OnEndLabelEdit)
    ON_NOTIFY(GVN_ACTIVATE,		IDC_GRID_CTRL, OnActivateCell)
	ON_WM_CONTEXTMENU()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_SETFONT, OnSetFont)
    ON_MESSAGE(WM_GETFONT, OnGetFont)
END_MESSAGE_MAP()



IMPLEMENT_UNKNOWN(CGridViewBase, Grid)
IMPLEMENT_UNKNOWN(CGridViewBase, Frame)


/////////////////////////////////////////////////////////////////////////////
// CGridViewBase message handlers


/*bool CGridViewBase::AlignByCell(int nRow, int nCol)
{
	if (!m_Grid.GetSafeHwnd() || !::IsWindow(m_Grid.GetSafeHwnd()))
		return false;

	if (!m_Grid.IsValid(nRow, nCol))
		return false;


	CRect rc = NORECT;
	if (!m_Grid.GetCellRect(nRow, nCol, &rc))
		return false;

	m_Grid.SetRedraw(false, false);

	for (int i = 0; i < m_Grid.GetColumnCount(); i++)
		m_Grid.SetColumnWidth(i, rc.Width());
	
	for (i = 0; i < m_Grid.GetRowCount(); i++)
		m_Grid.SetRowHeight(i, rc.Height());
	
	m_Grid.SetRedraw(true, true);
	m_ResultBar.Invalidate();
	return true;
}*/

/*bool CGridViewBase::AlignByDefault()
{
	if (!m_Grid.GetSafeHwnd() || !::IsWindow(m_Grid.GetSafeHwnd()))
		return false;


#ifndef _WIN32_WCE
    // Initially use the system message font for the GridCtrl font
    NONCLIENTMETRICS ncm;
	LOGFONT lf;
	memset(&lf, 0, sizeof(lf));
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    VERIFY(::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, 0));
    memcpy(&lf, &(ncm.lfMessageFont), sizeof(LOGFONT));
#else
    LOGFONT lf;
    GetObject(GetStockObject(SYSTEM_FONT), sizeof(LOGFONT), &lf);
#endif
	CFont font;
	if (!font.CreateFontIndirect(&lf))
		return false;

	m_Grid.SetRedraw(false, false);
// change font to initial	
	m_Grid.SetFont(&font);
	font.DeleteObject();
// autosize
	m_Grid.AutoSize(false);

	m_Grid.SetRedraw(true, true);
	m_ResultBar.Invalidate();

	return true;
} */

// messages

void CGridViewBase::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );
	if( !punk )return;

	ICommandManager2Ptr ptrCommandManager = punk;
	punk->Release();
	if( ptrCommandManager == NULL )
		return;

	CPoint pt = point;
	ScreenToClient( &pt );

	pt = ::ConvertToClient( GetControllingUnknown(), pt );
	ptrCommandManager->ExecuteContextMenu2( _bstr_t(GetMenuName()), point, 0 );
}

BOOL CGridViewBase::PreTranslateMessage(MSG* pMsg) 
{
	// vanek!!!
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (m_hAccel != NULL)
		{
			if (::TranslateAccelerator(m_Grid, m_hAccel, pMsg))
				return TRUE;
		}
	}

	return CWnd::PreTranslateMessage(pMsg);
}

int CGridViewBase::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;

	LoadState();
	
	ModifyStyle(0, WS_CLIPSIBLINGS);
	CRect rc, crc;
	GetClientRect(&crc);
	rc = crc;
	if (!m_Grid.Create(rc, this, IDC_GRID_CTRL))
		return -1;
	m_Grid.ModifyStyleEx(WS_BORDER, 0);

	rc.top = rc.bottom - 50;
	if (!m_ResultBar.Create("", WS_CHILD|WS_VISIBLE, rc, this, IDC_RESULT_BAR))
		return -1;

	m_ResultBar.ModifyStyle(WS_BORDER, 0);
	m_ResultBar.ModifyStyleEx(0, WS_EX_WINDOWEDGE);

	if (!DefaultInit())
		return -1;

	sptrIScrollZoomSite	sptrZS(GetControllingUnknown());
	sptrZS->SetAutoScrollMode(false);

	if (m_bShowResBar)
	{
		rc = crc;
		rc.bottom -= m_ResultBar.GetHeight();
		m_Grid.MoveWindow(rc);

		rc.top = rc.bottom;
		rc.bottom += m_ResultBar.GetHeight();
		m_ResultBar.MoveWindow(rc);
	}
	else
	{
		m_ResultBar.ShowWindow(SW_HIDE);
		m_Grid.MoveWindow(&crc);
	}

	AttachActiveObjects();


	CCellID FocusCell(m_Grid.GetAdditionRow(), m_Grid.GetFixedColumnCount());
	if (!m_Grid.IsValid(FocusCell))
	{
		//m_Grid.SetSelectedRange(CCellRange(-1, -1, -1, -1));
		m_Grid.SetFocusCell(-1, -1);
	}
	else
	{
		/*if (m_Grid.GetListMode())
			m_Grid.SetSelectedRange(FocusCell.row, m_Grid.GetFixedColumnCount(), FocusCell.row, m_Grid.GetColumnCount() - 1);
		else
			m_Grid.SetSelectedRange(FocusCell.row, FocusCell.col, FocusCell.row, FocusCell.col);
		  */
		m_Grid.SetFocusCell(FocusCell);
//		m_Grid.EnsureVisible(FocusCell);
	}

	// vanek!!!
	ASSERT(m_hAccel == NULL);
	m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(), MAKEINTRESOURCE(IDR_DEFAULT_ACCEL));
	ASSERT(m_hAccel != NULL);

	return 0;
}

void	CGridViewBase::OnDestroy()
{
	m_hAccel = NULL;  
}

LRESULT CGridViewBase::OnSetFont(WPARAM hFont, LPARAM lParam)
{
    LRESULT result = Default();

	result = m_Grid.SendMessage(WM_SETFONT, hFont, MAKELPARAM(true, 0));
	m_Grid.m_nDefCellHeight = m_Grid.m_nDefCellHeight+5;

	CRect rc;
	GetClientRect(&rc);

	if (m_bShowResBar)
	{
		CRect rcRes = rc;
		m_ResultBar.CalcDefCellHeight();
		int nDec = m_ResultBar.GetHeight();
		
		rc.bottom -= nDec;
		m_Grid.MoveWindow(&rc, true);

		rcRes.top = rc.bottom;
		m_ResultBar.MoveWindow(&rcRes, true);
	}
	else
	{
		if (::IsWindow(m_ResultBar.m_hWnd) && ::IsWindowVisible(m_ResultBar.m_hWnd))
			m_ResultBar.ShowWindow(SW_HIDE);

		m_Grid.MoveWindow(&rc, true);
	}

	return result;
}

LRESULT CGridViewBase::OnGetFont(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	return m_Grid.SendMessage(WM_GETFONT, 0, 0);
}

void CGridViewBase::OnSize(UINT nType, int cx, int cy)
{
	CViewBase::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(&rc);

	if (m_bShowResBar)
	{
		CRect rcRes = rc;
		int nDec = m_ResultBar.GetHeight();
		
		rc.bottom -= nDec;
		m_Grid.MoveWindow(&rc, true);

		rcRes.top = rc.bottom;
		m_ResultBar.MoveWindow(&rcRes, true);
	}
	else
	{
		if (::IsWindow(m_ResultBar.m_hWnd) && ::IsWindowVisible(m_ResultBar.m_hWnd))
			m_ResultBar.ShowWindow(SW_HIDE);

		m_Grid.MoveWindow(&rc, true);
	}
}

void CGridViewBase::OnSelChangingGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pnmgv	= (NM_GRIDVIEW*)pNMHDR;
	int nRow = pnmgv->iRow;
	int nCol = pnmgv->iColumn;
	*pResult = 0;

//	_selection_from_range();
}

void SafeExecute( _bstr_t	bstrSelectedAction )
{
	//[AY] disable associated action
	if( bstrSelectedAction.length() != 0 )
	{
		BSTR	bstrCurrent;
		IActionManagerPtr	ptrAM;
		IApplicationPtr ptrApp( GetAppUnknown() );
		{
			IUnknown* punkAM = 0;
			ptrApp->GetActionManager( &punkAM );
			if( punkAM )
			{
				ptrAM = punkAM;
				punkAM->Release();	punkAM = 0;
			}
		}						

		IUnknown	*punkRunning = 0;
		ptrAM->GetRunningInteractiveActionInfo( &punkRunning );

		if( punkRunning != 0 )
		{
			IActionInfoPtr	ptrAI( punkRunning );
			ptrAI->GetCommandInfo( &bstrCurrent, 0, 0, 0 );

			_bstr_t	btrCurrentAction = bstrCurrent;
			
			::SysFreeString( bstrCurrent );

			if( btrCurrentAction == bstrSelectedAction )
				return;
		}
		::ExecuteAction( bstrSelectedAction, 0, 0 );


	}
	//m_cols[i]->pparam->bstrAction;
}

void CGridViewBase::OnActivateCell(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pnmgv	= (NM_GRIDVIEW*)pNMHDR;

	if( m_objects == 0 )return;

	//activate object in object list
	long	row = pnmgv->iRow, col = pnmgv->iColumn;
	POSITION	lCurrentChildPos = 0, lNewChildPos = 0;
	POSITION	lCurrentParamPos = 0, lNewParamPos = 0;

	m_objects->GetActiveChild( &lCurrentChildPos );
	m_container->GetCurentPosition((LONG_PTR*)&lCurrentParamPos, 0);

	lNewChildPos = lCurrentChildPos;
	lNewParamPos = lCurrentParamPos;

	if( row != -1 )
	{
		if( m_Grid.GetAdditionRow() != row )
		{
			IUnknown	*punk = 0;

			if( m_Grid.GetLClickedCell().col <  m_Grid.GetFixedColumnCount() )
			{
				punk = _object_from_cell( row, col, true ); // return with AddRef
				if( punk )
					punk->Release();
			}
			else
				punk = m_rows[row]->pcalc; 

			INamedDataObject2Ptr sptrNDO2Obj(punk); // A.M. fix (BT3126). In some causes
			IUnknownPtr punkParent; // current object isn't child of the object list (manual measured objects e.g.).
			if (sptrNDO2Obj != 0) sptrNDO2Obj->GetParent(&punkParent);
			if (punkParent != 0 && ::GetObjectKey(punkParent) == ::GetObjectKey(m_objects))
				m_objects->GetChildPos( punk, &lNewChildPos );
			else //kir-- support of composite objects, needs because m_objects is just virtual object list
			{
				INamedDataObject2Ptr ndo(m_objects);
				IUnknown* unk;
				ndo->GetParent(&unk);//this return a real object list
				if (punkParent== unk)//object has a real object list as a parent
				m_objects->GetChildPos( punk, &lNewChildPos );				
			}
		}
		else
			lNewChildPos = 0;
	}

	if( col != -1 && col < m_cols.GetSize() && m_cols[col]->pparam )
	{
		lNewParamPos = (POSITION)m_cols[col]->pparam->lpos;
	}

	col = pnmgv->iColumn;

	/*long	lOldManual = -1;
	if( lCurrentParamPos )
	{
		ParameterContainer	*pc = 0;
		m_container->GetNextParameter( &lCurrentParamPos, &pc );
		lOldManual = pc->pDescr->pos != 0;
	}
	long	lNewManual = -1;
	if( lNewParamPos )
	{
		ParameterContainer	*pc = 0;
		m_container->GetNextParameter( &lNewParamPos, &pc );
		lNewManual = pc->pDescr->pos != 0;
	}*/


	IDataObjectListPtr	ptrObjectList( m_objects );
	BOOL	bOldLock;
	ptrObjectList->GetObjectUpdateLock( &bOldLock );
	//ptrObjectList->LockObjectUpdate( true );

	//if( lCurrentParamPos != lNewParamPos )
	{
		bool	bOld = LockNotification( true );
		m_container->SetCurentPosition((LONG_PTR)lNewParamPos);
		LockNotification( bOld );
	}

	//if( lNewChildPos != lCurrentChildPos )
	{
		bool	bOld = LockNotification( true );
		m_objects->SetActiveChild( lNewChildPos );
		LockNotification( bOld );
	}

	

	ptrObjectList->LockObjectUpdate( bOldLock );

	_selection_from_range();

	*pResult = 0;
}

   
void CGridViewBase::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pnmgv	= (NM_GRIDVIEW*)pNMHDR;

	int nRow = pnmgv->iRow;
	int nCol = pnmgv->iColumn;


	ColInfo	*pcol = m_cols[nCol];
	if( pcol->pparam )
	{
		GV_ITEM	item;
		item.mask = GVIF_TEXT;
		item.row = nRow;
		item.col = nCol;
		m_Grid.GetItem( &item );

		if( nRow == 0 )
		{
			CString	strParams;
			strParams.Format( "GridRenameParam %d, \"%s\"", pcol->pparam->pDescr->lKey, (const char*)item.strText );
			::ExecuteScript( _bstr_t( strParams ), "GridView::GridRenameParam" );
		}
		else if( nRow >= rowFirstObject() && nRow < m_Grid.GetAdditionRow() )
		{
			RowInfo	*prow = m_rows[nRow];

			if( item.strText == "--"||
				item.strText == "-" ||
				item.strText.IsEmpty() )
			{
				prow->pcalc->RemoveValue( pcol->pparam->pDescr->lKey );
			}
			else
			{
				double	f;
				if( ::sscanf( item.strText, "%lf", &f ) != 1 )
				{
					AfxMessageBox( IDS_INVALID_NUIMERIC_VALUE );
					prow->bFilled = false;
					OnGetDispInfoRow( nRow );
					return;
				}
				prow->pcalc->SetValue( pcol->pparam->pDescr->lKey, f/pcol->pparam->pDescr->fCoeffToUnits );
			}
			IDataObjectListPtr	ptrL( m_objects );
			if( ptrL != 0 )ptrL->UpdateObject( prow->pcalc );
		}
	}
}

void CGridViewBase::OnSelChangedGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pnmgv	= (NM_GRIDVIEW*)pNMHDR;
	int nRow = pnmgv->iRow;
	int nCol = pnmgv->iColumn;
	
	*pResult = 0;
}


// override virtuals
void CGridViewBase::AttachActiveObjects()
{
	if (!GetSafeHwnd() || !::IsWindow(m_Grid.GetSafeHwnd()))
		return;

	IUnknownPtr	sptrList(::GetActiveObjectFromContext(GetControllingUnknown(), GetObjectType()), false);

	if (::GetObjectKey(m_objects) == ::GetObjectKey(sptrList))
		return;
	
	RemoveAllObjects();

	ICompositeObjectPtr co(sptrList);
	if(co)
	{
		long bC=0;
		co->IsComposite(&bC);
		if(bC)
		{	
			IUnknown* unk;
			co->GetMainLevelObjects(&unk);
			if(unk)
			{
				m_objects = unk;
			}
		}
		else m_objects = sptrList;
	}
	else m_objects = sptrList;

	
	m_container = sptrList;

	m_ResultBar.UpdateParams( m_objects );
	
	FillAll();
	
	// [vanek] : replace invalidation in to FillAll( )
	//m_Grid.Invalidate();
}

void CGridViewBase::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CViewBase::OnNotify(pszEvent, punkHit, punkFrom, pdata, cbSize);
	if (!strcmp(pszEvent, szEventChangeObjectList))
	{
		if (punkFrom != NULL && ::GetObjectKey(m_objects) != ::GetObjectKey(punkFrom))
			return;

		int	nAction = *(long*)pdata;
		if (nAction == cncAdd)
		{
			// A.M. fix (BT 3113): sometimes cncAdd called after cncReset and adds object
			// wich already added in cncReset.
			ICalcObjectPtr sptrCO(punkHit);
			RowInfo *pRowInfo = _row_by_object(sptrCO);
			if (pRowInfo == NULL)
				AddObject( punkHit );
			UpdateActiveCell();
		}
		else if (nAction == cncRemove)
		{
			RemoveObject( punkHit );
			UpdateActiveCell();
		}
		else if (nAction == cncChange)
		{
			ICalcObjectPtr	ptrCalc( punkHit );
			RowInfo	*prow = _row_by_object( ptrCalc );
			if( prow )UpdateObject( ptrCalc, prow->row );
		}
		else if (nAction == cncReset)
		{
			FillAll();
		}
		else if (nAction == cncActivate)
		{
			//Kir - BT 4786
			UpdateActiveCellNoHScroll();
		}
		else if( nAction == cncAddParameter )
		{
			long	lParamKey = *((long*)pdata+1);
			ParameterContainer *p = _param_by_key( lParamKey );
			AddParameter( p );
		}
		else if( nAction == cncRemoveParameter )
		{
			long	lParamKey = *((long*)pdata+1);
			ParameterContainer *p = _param_by_key( lParamKey );
			RemoveParameter( p );
		}
		else if( nAction == cncActivateParameter )
		{
			long	lParamKey = *((long*)pdata+1);
			ParameterContainer *p = _param_by_key( lParamKey );
			UpdateActiveCell();
		}
		else if( nAction == cncResetParameters )
		{
			FillAll();
		}
		else if( nAction == cncChangeParameter )
		{
			long	lParamKey = *((long*)pdata+1);
			ParameterContainer *p = _param_by_key( lParamKey );
			UpdateParameter( p );
		}
		else if( nAction == cncMoveParameter )
		{
/*			long	lParamKey = *((long*)pdata+1);
			ParameterContainer *p = _param_by_key( lParamKey );

			ColInfo	*pcol = _col_by_param( p );
			if( !pcol )return;
			long	col = pcol->col;
			ParameterContainer	*pprev = 0;
			long	lpos = p->lpos;
			m_container->GetPrevParameter( &lpos, 0 );
			if( lpos )m_container->GetNextParameter( &lpos, &pprev );

			long	colAfter = colFirstParameter();
			if( pprev )
			{
				ColInfo	*pcol1 = _col_by_param( pprev );
				if( !pcol1 )return;
				colAfter = pcol1->col;
			}
			if( m_Grid.MoveColumn( col, colAfter ) )
			{
				long newCol = colAfter + 1;
				if( col < newCol )
				{
					m_cols.InsertAt( newCol, pcol );
					m_cols.RemoveAt( col );
				}
				else
				{
					m_cols.RemoveAt( col );
					m_cols.InsertAt( newCol, pcol );
				}
			}*/
			FillAll();
		}

//			m_Grid.SetRedraw(bPrevDraw, bResetScrollBars);
	}
	else if (!strcmp(pszEvent, szEventNewSettings) || !strcmp( pszEvent, szEventNewSettingsView ) ) // [vanek]: support szEventNewSettingsView
	{
		//BOOL bPrevDraw = m_Grid.SetRedraw(false, false);
		//bool bPrevShow = m_bShowResBar;

		LoadState();
		CalcLayout();
		
		//m_Grid.SettingChange();
		/*m_Grid.SetRedraw(bPrevDraw);
		if (m_bShowResBar)
		{
			m_ResultBar.Invalidate();
			m_ResultBar.UpdateWindow();
		}*/

		m_objects = 0;
		AttachActiveObjects();
	}
 }

void CGridViewBase::CalcLayout()
{
	CRect rc;
	GetClientRect(&rc);

	CRect rcRes = rc;
	if (m_bShowResBar)
	{
		CRect rcRes = rc;
		int nDec = m_ResultBar.GetHeight();
		
		if (::IsWindow(m_ResultBar.m_hWnd) && !::IsWindowVisible(m_ResultBar.m_hWnd))
			m_ResultBar.ShowWindow(SW_SHOW);

		rc.bottom -= nDec;
		m_Grid.MoveWindow(&rc, false);

		rcRes.top = rc.bottom;
		m_ResultBar.MoveWindow(&rcRes, false);
	}
	else
	{
		if (::IsWindow(m_ResultBar.m_hWnd) && ::IsWindowVisible(m_ResultBar.m_hWnd))
			m_ResultBar.ShowWindow(SW_HIDE);

		m_Grid.MoveWindow(&rc, false);
	}
}

CGridCellBase* CGridViewBase::CreateCell(int nRow, int nCol)	
{	
	if( nRow >= rowFirstObject() && nCol == 0 )
		return new CNumberFixedCell( rowFirstObject()-1 );
	return new CVTGridCell();
}

bool CGridViewBase::RemoveAllObjects()
{
	SaveState();
	m_Grid.SetSelectedRange(-1, -1, -1, -1);
	m_Grid.SetRowCount(2);
	m_mapObjectToRow.RemoveAll();

	return true;
}

bool CGridViewBase::DefaultInit()
{	
	bool bRet = true;

	m_Grid.SetEditable(true);
	m_Grid.SetHeaderSort(false);
	m_Grid.EnableDragAndDrop(false);
	m_Grid.EnableDragColumn(TRUE);
	m_Grid.EnableTitleTips(false);
	m_Grid.SetMarkerColor(RGB(0, 0, 255));
	m_Grid.SetAdditionRow(true);
	m_Grid.SetDefaultFont();

	// second row contains meas units
	m_Grid.SetColumnCount(colFirstParameter());
	m_Grid.SetFixedColumnCount(colFirstParameter());
	m_Grid.SetRowCount(rowFirstObject());
	m_Grid.SetFixedRowCount(rowFirstObject());
	

	InitResultBar();
	LoadState();

	

	return true;	
}

bool CGridViewBase::InitResultBar()
{
	m_ResultBar.Init();
	m_ResultBar.Invalidate();

	return true;
}


void CGridViewBase::TestInit()
{
	m_Grid.SetEditable(true);
	m_Grid.SetListMode(true);
//	m_Grid.SetHeaderSort(false);
	m_Grid.EnableDragAndDrop(false);
	m_Grid.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));

	try
	{
		m_Grid.SetRowCount(50);
		m_Grid.SetColumnCount(20);
		m_Grid.SetFixedRowCount(1);
		m_Grid.SetFixedColumnCount(1);
	}
	catch (CMemoryException * e)
	{
		e->ReportError();
		e->Delete();
		return ;
	}
 
    DWORD dwTextStyle = DT_RIGHT|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS;

	// fill rows/cols with text
	for (int row = 0; row < m_Grid.GetRowCount(); row++)
	{
		for (int col = 0; col < m_Grid.GetColumnCount(); col++)
		{ 
			GV_ITEM Item;
			Item.mask = GVIF_TEXT|GVIF_FORMAT;
			Item.row = row;
			Item.col = col;
			if (row < 1)
            {
				Item.nFormat = DT_LEFT|DT_WORDBREAK;
				Item.strText.Format(_T("Column %d"),col);
			}
            else if (col < 1) 
            {
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("Row %d"),row);
			}
            else 
            {
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("%d"),row*col);
			}
			m_Grid.SetItem(&Item);

			if (rand() % 10 == 1)
			{
                COLORREF clr = RGB(rand() % 128+128, rand() % 128+128, rand() % 128+128);
				m_Grid.SetItemBkColour(row, col, clr);
				m_Grid.SetItemFgColour(row, col, RGB(255,0,0));
			}
		}
	}
	// Make cell 1,1 read-only
    m_Grid.SetItemState(1,1, m_Grid.GetItemState(1,1) | GVIS_READONLY);

  
	m_Grid.AutoSize();
	m_Grid.SetRowHeight(0, 3*m_Grid.GetRowHeight(0)/2);
}

IUnknown* CGridViewBase::GetObjectByPoint(CPoint pt)
{
	return 0;
}

BOOL CGridViewBase::OnEraseBkgnd(CDC * pDC)
{
	return true;//CWnd::OnEraseBkgnd(pDC);
}

BOOL CGridViewBase::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPCHILDREN;
	return CViewBase::PreCreateWindow(cs);
}

void CGridViewBase::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_Grid.SendMessage(WM_HSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)(pScrollBar ? pScrollBar->m_hWnd : 0));
	m_ResultBar.SendMessage(WM_HSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)(pScrollBar ? pScrollBar->m_hWnd : 0));
}

void CGridViewBase::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	m_Grid.SendMessage(WM_VSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)(pScrollBar ? pScrollBar->m_hWnd : 0));
}



POSITION CGridViewBase::GetFisrtVisibleObjectPosition()
{
	if( m_objects != 0 )
		return (POSITION)1;
	return 0;
}

IUnknown *CGridViewBase::GetNextVisibleObject( POSITION &rPos )
{
	if( (long)rPos == 1 )
	{
		rPos = 0;
		IUnknown	*punk = m_objects;
		if( punk )punk->AddRef();
		return punk;
	}
	return 0;
}
COLORREF	CGridViewBase::GetCellBackColor(int nRow, int nCol)
{
	COLORREF	clr = m_clrUndefine;
	ColInfo	*pcol = m_cols[nCol];
	if( !pcol->pparam )return clr;

	if( pcol->pparam->type == etLinear )return m_clrLinear;
	else if( pcol->pparam->type == etCounter )return m_clrCounter;
	else if( pcol->pparam->type == etAngle )return m_clrAngle;
	else return m_clrUndefine;
	
}
bool CGridViewBase::SaveState()
{return true;}
bool CGridViewBase::LoadState()
{
	m_bShowResBar = ::GetValueInt( GetAppUnknown(), m_strSection, "ShowResultBar", m_bShowResBar )!=0;
	m_ResultBar.Show(m_bShowResBar);
	m_ResultBar.LoadState();
	m_ResultBar.UpdateParams();

	m_clrLinear = ::GetValueColor( GetAppUnknown(), m_strSection, "LinearColor", m_clrLinear );
	m_clrAngle  = ::GetValueColor( GetAppUnknown(), m_strSection, "AngleColor", m_clrAngle );
	m_clrCounter  = ::GetValueColor( GetAppUnknown(), m_strSection, "CounterColor", m_clrCounter );
	m_clrUndefine  = ::GetValueColor( GetAppUnknown(), m_strSection, "UndefineColor", m_clrUndefine );

	
	return true;
}

void CGridViewBase::FillAll()
{
	Clear();

	DefaultInit();
	RemoveAllParams();
	RemoveAllObjects();
	AddAllParams();
	AddAllObjects();

	UpdateActiveCell();
	// [vanek]
	m_Grid.Invalidate();
}


bool CGridViewBase::AddAllParams()
{
	_clear_selection();

	if( m_container == 0 )return true;

	long	nCount;
	m_container->GetParameterCount( &nCount );

	long	col = colFirstParameter();
	m_Grid.SetColumnCount( nCount+col );


	LONG_PTR	lpos;
	m_container->GetFirstParameterPos( &lpos );

	while( lpos )
	{
		ParameterContainer	*pc = 0;
		m_container->GetNextParameter( &lpos, &pc );

		DefineParameter( pc, col );
		col++;
	}

	m_ResultBar.UpdateParams();

	return true;
}

bool CGridViewBase::DefineParameter( ParameterContainer *pc, long col )
{
	if( !pc || !pc->pDescr )	return false;

	m_cols[col]->pparam = pc;
	m_mapParamDefToCol[pc]=m_cols[col];
	m_mapParamKeyToParamDef[pc->pDescr->lKey] = pc;

	GV_ITEM	item;
	item.col = col;
	item.mask = GVIF_TEXT;

	item.row = 0;
	item.strText = pc->pDescr->bstrUserName;
	m_Grid.SetItem( &item );

	item.strText = pc->pDescr->bstrUnit;
	item.row = rowMeasureUnit();
	if( item.strText == "mu"||item.strText == "mu*mu" )
	{
		if( item.strText == "mu" )item.strText = "m";
		else item.strText = "m*m";
		item.mask |=GVIF_FONT;
		ZeroMemory( &item.lfFont, sizeof( item.lfFont ) );
		item.lfFont.lfCharSet = SYMBOL_CHARSET;
		item.lfFont.lfHeight = 16;
		strcpy( item.lfFont.lfFaceName, "Symbol" );

	}
	m_Grid.SetItem( &item );



	bool	bOldEnable = m_Grid.m_bEnableWidthNotify;
	m_Grid.m_bEnableWidthNotify = false;
	m_Grid.SetColumnWidth( col, pc->cxWidth );
	m_Grid.m_bEnableWidthNotify = bOldEnable;

	return true;
}

bool CGridViewBase::AddParameter( ParameterContainer *pc )
{
	ASSERT(_col_by_param( pc ) == 0 );
	CString	str =  pc->pDescr->bstrUserName;
	long	col = m_Grid.InsertColumn( str );

	DefineParameter( pc, col );

	//clear col info
	
	for( long	row = rowFirstObject(); row < m_rows.GetSize(); row++ )
		m_rows[row]->bFilled = false;

	m_ResultBar.UpdateParams( m_objects );

	return true;
}

bool CGridViewBase::OnGetDispInfoRow( long row )
{
	RowInfo	*prow = m_rows[row];
	if( prow->bFilled )return false;

	prow->bFilled = true;
	if( !prow->pcalc )return false;


	long	colsCount = m_Grid.GetColumnCount(), col;

	LONG_PTR	lpos;
	m_container->GetFirstParameterPos( &lpos );


	GV_ITEM	item;
	item.row = row;
	item.col = 0;
	item.mask = GVIF_TEXT;
	item.strText.Format( "%d", row-rowFirstObject()+1 );
	m_Grid.SetItem( &item );

	col = colFirstParameter();

	while( lpos )
	{
		ParameterContainer	*pc = 0;
		m_container->GetNextParameter( &lpos, &pc );

		CString	strValue;
		FormatValueFromCalcObject( pc, prow->pcalc, strValue );

		CGridCellBase* pCell = m_Grid.GetCell( row, col );
		if( !pCell )continue;
		pCell->SetText( strValue );

		//manual object
		CVTGridCell	*pVTCell = (CVTGridCell*)pCell;
		if( !pc->pDescr->pos && pc->type == etUndefined ) pVTCell->SetType( egctManualParam );
		else pVTCell->SetType( egctRegularParam );
		col++;
	}

	return true;
}

bool CGridViewBase::RemoveParameter( ParameterContainer *pc )
{
	ColInfo	*pcol = _col_by_param( pc );
	if( !pcol  )return false;

	CRect	rect;
	m_Grid.GetCellRect( 0, pcol->col, &rect );

	m_Grid.DeleteColumn( pcol->col );
	m_mapParamDefToCol.RemoveKey( pc );
	m_mapParamKeyToParamDef.RemoveKey( pc->pDescr->lKey );

	m_ResultBar.UpdateParams( m_objects );
	UpdateActiveCell();


	CRect	rectClient;
	m_Grid.GetClientRect( &rectClient );
	rect.right = rectClient.right;
	rect.bottom=  rectClient.bottom;

	m_Grid.InvalidateRect( rectClient );


	return true;
}

bool CGridViewBase::UpdateParameter( ParameterContainer *pc )
{
	ColInfo	*pcol = _col_by_param( pc );
	if( !pcol  )return false;

	//m_Grid.DeleteColumn( pcol->col  );
	//m_Grid.InsertColumn( str,DT_CENTER|DT_VCENTER|DT_SINGLELINE, pcol->col );

	GV_ITEM	item;
	item.row = 0;
	item.col = pcol->col;
	item.strText = pc->pDescr->bstrUserName;
	item.mask = GVIF_TEXT;
	m_Grid.SetItem( &item );

	bool	bOldEnable = m_Grid.m_bEnableWidthNotify;
	m_Grid.m_bEnableWidthNotify = false;
	m_Grid.SetColumnWidth( pcol->col, pc->cxWidth );
	m_Grid.m_bEnableWidthNotify = bOldEnable;

	CRect	rect;
	m_Grid.GetCellRect( 0, pcol->col, &rect );
	CRect	rectClient;
	m_Grid.GetClientRect( &rectClient );
	rect.right = rectClient.right;
	rect.bottom=  rectClient.bottom;

	m_Grid.InvalidateRect( rect );

	if (m_bShowResBar)
		SyncResBar();
	return true;	
}


bool CGridViewBase::RemoveAllParams()
{
	m_Grid.SetColumnCount( colFirstParameter() );
	m_mapParamKeyToParamDef.RemoveAll();
	m_mapParamDefToCol.RemoveAll();
	return true;
}

bool CGridViewBase::AddAllObjects()
{
	CTimeTest	test( true, "CGridViewBase::AddAllObjects" );

	m_Grid.AutoSizeRows();
	m_Grid.ResetScrolls();

	if( m_objects == 0  )
	{
		m_ResultBar.Recalc();
		return true;
	}
	
	long	lCount = 0;
	m_objects->GetChildsCount( &lCount );

	long	row = rowFirstObject();
	m_Grid.SetRowCount( lCount+row );

	TPOS	lpos;
	m_objects->GetFirstChildPosition( &lpos );

	

	while( lpos ) 
	{
		IUnknown *punk = 0;
		m_objects->GetNextChild( &lpos, &punk );
		if( punk )
		{
			// vanek
			SetRowHeightByObject( row, punk );
			//

			ICalcObjectPtr	ptrCalc = punk;
			punk->Release();
			if( ptrCalc != 0 )
			{
				m_rows[row]->pcalc = ptrCalc;
				m_rows[row]->bFilled = false;
				m_mapObjectToRow[ptrCalc]=m_rows[row];
				
				// [vanek] BT2000: 3673 - set row info 26.02.1004
				OnGetDispInfoRow( row );
         	}
		}
		row++;
	}

	m_ResultBar.Recalc();

	return true;
}

long CGridViewBase::AddObject( ICalcObjectPtr ptrCalc )
{
	return UpdateObject( ptrCalc, -1 );
}

long CGridViewBase::UpdateObject( ICalcObjectPtr ptrCalc, long row )
{
	if( row == -1 )
	{
		ICompositeSupportPtr cs(ptrCalc);
		if(cs!=0)
		{
			long lvl;
			cs->GetLevel(&lvl);
			if(lvl!=0) return -1;
		}
		row = m_Grid.InsertRow( "*" );
	}
		

	// vanek
	bool	bOldEnable = m_Grid.m_bEnableHeightNotify;
	m_Grid.m_bEnableHeightNotify = false;
	SetRowHeightByObject( row, (IUnknown *) ptrCalc );
	m_Grid.m_bEnableHeightNotify = bOldEnable;
	//
    	
	m_rows[row]->pcalc = ptrCalc;
	m_rows[row]->bFilled = false;
	m_mapObjectToRow[ptrCalc]=m_rows[row];

	OnGetDispInfoRow( row );
	m_Grid.RedrawRow( row );

	m_ResultBar.Recalc();
	
	return row;
}

bool CGridViewBase::RemoveObject( ICalcObjectPtr ptrCalc )
{
	if( ptrCalc == 0 )return true;
	CRect	rect, rectCell;
	RowInfo	*prow = _row_by_object( ptrCalc );
	if(prow==0)
	{
		ICompositeSupportPtr cs(ptrCalc);
		if(cs!=0)
		{
			/*long lvl;
			cs->GetLevel(&lvl);
			if(lvl!=0) 
			{
				UpdateActiveCell();
				m_Grid.GetClientRect( &rect );
				m_Grid.InvalidateRect( &rect );
				m_ResultBar.Recalc();
				
			}*/
			return true;
		}
	}
	ASSERT( prow );

	m_mapObjectToRow.RemoveKey( (ICalcObject *)ptrCalc );
	if( !prow )return false;


	m_Grid.GetClientRect( &rect );
	m_Grid.GetCellRect( prow->row, 0, &rectCell );
//	rect.top = rectCell.top;

	if( !m_Grid.DeleteRow( prow->row ) )
		return false;

	UpdateActiveCell();
	m_Grid.InvalidateRect( &rect );
	m_ResultBar.Recalc();

	return true;
}

bool CGridViewBase::UpdateActiveCell()
{
	if( m_objects == 0 || m_container == 0 )
		return false;

	long	row = m_Grid.GetAdditionRow(), col = 0;

	//active row
	POSITION	lpos;
	m_objects->GetActiveChild( &lpos );
	if( lpos )
	{
		IUnknown	*punkO = 0;
		m_objects->GetNextChild( &lpos, &punkO );
		if( punkO )
		{
			ICalcObjectPtr	ptrC=punkO;
			punkO->Release();

			if( ptrC != 0 )
			{
				RowInfo	*prow = _row_by_object( ptrC );
				if(prow) row  = prow->row;
			}
		}
	}
	//active column

	long	lkey = -1;
	m_container->GetCurentPosition((LONG_PTR*)&lpos, &lkey);

	

	if( lkey != -1 )
	{
		ParameterContainer	*pc = _param_by_key( lkey );
		if( pc )
		{
			ColInfo	*pcol = _col_by_param( pc );
			col = pcol->col;
		}
	}
	
	m_Grid.ResetSelectedRange();	
/*	if( col == 0 )
	{
		m_Grid.SetFocusCell( row, -1 );
		return true;
	}
	else*/
	{
		m_Grid.SetSelectedRange( row, col, row, col );
		m_Grid.EnsureVisible( row, col );
		if( row == m_Grid.GetAdditionRow()-1 )
			m_Grid.EnsureVisible( m_Grid.GetAdditionRow(), col );
		m_Grid.SetFocusCell( row, col );
	}

	_selection_from_range();


	return true;
}


CGridViewBase::RowInfo *CGridViewBase::_row_by_object( ICalcObject *pcalc )
{
	RowInfo	*prow = 0;
	if( m_mapObjectToRow.Lookup( pcalc, prow ) )
		return prow;
	return 0;
}

ParameterContainer	*CGridViewBase::_param_by_key( long lKey )
{
	ParameterContainer	*pparam = 0;
	if( m_mapParamKeyToParamDef.Lookup( lKey, pparam ) )
		return pparam;
	m_container->ParamDefByKey( lKey, &pparam );
	if( pparam != 0 )
		m_mapParamKeyToParamDef[lKey] = pparam;
	return pparam;
}	

CGridViewBase::ColInfo *CGridViewBase::_col_by_param( ParameterContainer *pparam )
{
	ColInfo	*pcol = 0;
	if( m_mapParamDefToCol.Lookup( pparam, pcol ) )
		return pcol;
	return 0;
}

void CGridViewBase::OnSetFocus(CWnd* pOldWnd) 
{
//	CViewBase::OnSetFocus(pOldWnd);
	if( m_Grid.GetSafeHwnd() )
		m_Grid.SetFocus();
}

void CGridViewBase::OnSetColumnWidth(long col, long width)
{
	ColInfo	*pi = m_cols[col];
	
	
	if( !pi->pparam )
	{
		StoreColumnWidth( col, width );	
		SyncResBar();
		return;
	}

	long	lKey = pi->pparam->pDescr->lKey;
	char	sz[100];

	sprintf( sz, "GridParamWidth %d, %d", lKey, width );
	::ExecuteScript( _bstr_t( sz ), "GridView::GridParamWidth" );
}

void CGridViewBase::OnSetRowHeight(long row, long height)
{
    char	sz[100];
	sprintf( sz, "GridObjectHeight %d, %d ", row, height );
	
	::ExecuteScript( _bstr_t( sz ), "GridView::GridObjectHeight" );    
}

void CGridViewBase::OnEndColumnDrag(int col, int nColAfter)
{
	if( col == nColAfter )return;
	TRACE("CGridViewBase::OnEndColumnDrag(%d, %d)\n", col, nColAfter );
	ColInfo	*pi = m_cols[col];
	ColInfo	*piAfter = m_cols[nColAfter];


	if( !pi->pparam )
		return;
	long	lKey = pi->pparam->pDescr->lKey;
	long	lKeyAfter = piAfter->pparam?piAfter->pparam->pDescr->lKey:-1;
	char	sz[100];

	sprintf( sz, "GridMoveParam %d, %d", lKey, lKeyAfter );
	::ExecuteScript( _bstr_t( sz ), "GridView::GridMoveParam" );
}

void CGridViewBase::OnInsertColumn(long nIndex)
{
	ColInfo	*p = new ColInfo;
	p->pparam = 0;
	m_cols.InsertAt( nIndex, p );
	ArrangeCols();
}

void CGridViewBase::OnInsertRow(long nIndex)
{
	RowInfo	*p = new RowInfo;
	::ZeroMemory( p, sizeof(RowInfo) );
	
	p->bFilled = false;
	m_rows.InsertAt( nIndex, p );
	ArrangeRows();
}

void CGridViewBase::OnDeleteColumn(long nColumn)
{
	ColInfo	*p = m_cols[nColumn];
	m_cols.RemoveAt( nColumn );
	delete p;

	ArrangeCols();
}
void CGridViewBase::OnDeleteRow(long nRow)
{
	RowInfo	*p = m_rows[nRow];
	m_rows.RemoveAt( nRow );
	delete p;

	ArrangeRows();
}

void CGridViewBase::OnSetColumnCount(long nCols)
{
	if( nCols > m_cols.GetSize() )
	{
		for (long col = (long)m_cols.GetSize(); col < nCols; col++)
		{
			ColInfo	*p = new ColInfo;
			p->pparam = 0;
			m_cols.Add( p );
		}
		ArrangeCols();
	}
	else
	{
		for( long col = nCols; col < m_cols.GetSize(); col++ )
			delete m_cols[col];
		m_cols.SetSize( nCols );
	}
}
void CGridViewBase::OnSetRowCount(long nRows)
{
	if( nRows > m_rows.GetSize() )
	{
		for (long row = (long)m_rows.GetSize(); row < nRows; row++)
		{
			RowInfo	*p = new RowInfo;
			::ZeroMemory( p, sizeof(RowInfo) );
			
			m_rows.Add( p );
		}
		ArrangeRows();
	}
	else
	{
		for( long row = nRows; row < m_rows.GetSize(); row++ )
			delete m_rows[row];
		m_rows.SetSize( nRows );
	}
}

void CGridViewBase::ArrangeRows()
{
	for( long	row = 0; row < m_rows.GetSize(); row++ )
		m_rows[row]->row = row;
}

void CGridViewBase::ArrangeCols()
{
	for( long	col = 0; col < m_cols.GetSize(); col++ )
		m_cols[col]->col = col;
}

bool CGridViewBase::GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX)
{
//	_add_result_to_grid();
	bool bRet = m_Grid.OnGetPrintWidth(nMaxWidth, nReturnWidth, nUserPosX, nNewUserPosX);
//	_remove_result_from_grid();
	return bRet;
}

bool CGridViewBase::GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY)
{
	if( nUserPosY == 0 )//start printing
		m_lStartPrintResultBarPosition = -1;
/*	_add_result_to_grid();
	bool bRet = m_Grid.OnGetPrintHeight(nMaxHeight, nReturnHeight, nUserPosY, nNewUserPosY);
	_remove_result_from_grid();
	return bRet;*/
	//nNewUserPosY = nUserPosY;
	//nReturnHeight = 0;

	if( m_lStartPrintResultBarPosition == -1 ||
		m_lStartPrintResultBarPosition < nUserPosY )
	{
		if( m_Grid.OnGetPrintHeight(nMaxHeight, nReturnHeight, nUserPosY, nNewUserPosY) ) 
			return true;
		if( !m_bShowResBar)
			return false;
		m_lStartPrintResultBarPosition = nNewUserPosY;
	}


	int	nStartPos = nNewUserPosY - m_lStartPrintResultBarPosition;
	long	lPrinted = 0;
	int	nNewRetHeight = nReturnHeight;
	int	nResultRowCount = (int)m_ResultBar.GetParamArray().GetSize();

	for( long row = 0; row < nResultRowCount; row++ )
	{
		if( nStartPos > 0 )
		{
			lPrinted++;
			nStartPos-=m_ResultBar.GetDefCellHeight();
			continue;
		}
		if( nReturnHeight+m_ResultBar.GetDefCellHeight() > nMaxHeight )
			return true;
		nReturnHeight+=m_ResultBar.GetDefCellHeight();
		nNewUserPosY+=m_ResultBar.GetDefCellHeight();
	}
	return false;
}

void CGridViewBase::Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags)
{
	CDC* pdc = CDC::FromHandle(hdc);
	if (!pdc)return;

//	_add_result_to_grid();

	int	nLastPosX = nUserPosX + nUserPosDX - 1;
	int	nLastPosY = nUserPosY + nUserPosDY - 1;

	CRect	RectDraw = CRect(nUserPosX, nUserPosY, nLastPosX, nLastPosY);
	int	nBottom = rectTarget.top;
	if(m_lStartPrintResultBarPosition == -1 || nUserPosY < m_lStartPrintResultBarPosition )
		nBottom = m_Grid.OnPrintDraw(pdc, rectTarget, RectDraw );
	if( m_lStartPrintResultBarPosition == -1 || nLastPosY < m_lStartPrintResultBarPosition-1 )
		return;
	CPen darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW));

	int nSave = pdc->SaveDC();

	pdc->SetMapMode(MM_ANISOTROPIC);
	pdc->SetViewportOrg(rectTarget.TopLeft());
	pdc->SetWindowExt(CSize(RectDraw.Width(), RectDraw.Height()));
	pdc->SetViewportExt(CSize(rectTarget.Width(), rectTarget.Height()));
	pdc->SetBkMode( TRANSPARENT );

	pdc->SelectObject(m_Grid.GetFont());

	CRgn ClipRegion;
	if (ClipRegion.CreateRectRgnIndirect(rectTarget))
		pdc->SelectClipRgn(&ClipRegion);

	ClipRegion.DeleteObject();

	int	row = nLastPosY - m_lStartPrintResultBarPosition+1;
	int	nResultRowCount = (int)m_ResultBar.GetParamArray().GetSize();

	CRect	rectParam;

	CRect	rectGrid = m_ResultBar.CalcGridRect();
	CRect	rectName = m_ResultBar.CalcParamNameRect();

	rectParam.top = m_lStartPrintResultBarPosition;

	CRect	rectResultBar;
	rectResultBar.SetRect( 0-nUserPosX, m_lStartPrintResultBarPosition-nUserPosY,  nUserPosDX, m_lStartPrintResultBarPosition+nUserPosDY-nUserPosY );
	pdc->FillRect( rectResultBar, &CBrush( GetSysColor( COLOR_3DFACE ) ) );


	for( row = 0; row < nResultRowCount; row++ )
	{
		rectParam.bottom = rectParam.top+m_ResultBar.GetDefCellHeight();
		if( rectParam.bottom < nUserPosY || rectParam.top > nLastPosY )
			continue;


		//rectParam.left = nUserPosX;

		CResultParam	&param = m_ResultBar.GetParamArray().GetAt( row );
		int	colCount = (int)param.GetValues().GetSize();

		CRect	rectP = param.GetParamRect( 0 );
		rectP.left = rectName.left-nUserPosX;
		rectP.right = rectName.right-nUserPosX;
		rectP.top = rectParam.top-nUserPosY;
		rectP.bottom = rectParam.bottom-nUserPosY;
		pdc->DrawText(param.m_strName, &rectP, DT_VCENTER|DT_LEFT|DT_SINGLELINE|DT_END_ELLIPSIS);
		if (m_ResultBar.IsDrawLine() && row < nResultRowCount-1)
		{
			CPen * pOldPen = pdc->SelectObject(&darkpen);
			pdc->MoveTo(RectDraw.left + 3, rectP.bottom);
			pdc->LineTo(RectDraw.right - 3, rectP.bottom);
			pdc->SelectObject(pOldPen);
		}

		//draw parameters
		for( int col = 0; col < colCount; col++ )
		{
			SValue	&value = param.GetValues().GetAt( col );

			if( !col )rectParam.left = rectGrid.left-nUserPosX+4;;
			rectParam.right = rectParam.left+m_Grid.GetColumnWidth( col+colFirstParameter() );

			rectP = rectParam;
			rectP.top -= nUserPosY;
			rectP.bottom -= nUserPosY;

			int nOffset = 0;
			int nLast = 0;
			m_ResultBar.CalcOffset(pdc, col, nOffset, nLast);
			m_ResultBar.DrawParam( pdc, rectP, value.strFormat, value.fValue, value.bEmpty, nOffset, nLast );
			rectParam.left = rectParam.right;
		}
		rectParam.top = rectParam.bottom;
	}
	if (nSave)
		pdc->RestoreDC(nSave);




//	_remove_result_from_grid();
}

bool CGridViewBase::SyncResBar()
{
	m_ResultBar.CalcGridRect();
	m_ResultBar.SyncParamColumn();
	m_ResultBar.Invalidate();
	return true;
}

void CGridViewBase::OnHorizontalScroll(long nSBCode, long nPos, CScrollBar*pScrollBar)
{
	m_ResultBar.SyncParamColumn();
}

///////////////////////////
//IGridView
HRESULT CGridViewBase::XGrid::GetObjectByRow( long lrow, IUnknown **ppunkObject )
{
	METHOD_PROLOGUE_EX(CGridViewBase, Grid);
	
	if ( ppunkObject && lrow >= 0 && lrow < pThis->m_rows.GetSize() )
	{
		*ppunkObject = (IUnknown *) pThis->m_rows[ lrow ]->pcalc;
		if( *ppunkObject )
		{
			(*ppunkObject)->AddRef();
			return S_OK;
		}
	}

	return S_FALSE;
}

HRESULT CGridViewBase::XGrid::GetRowHeight( long lrow, long *lpheight )
{
	METHOD_PROLOGUE_EX(CGridViewBase, Grid);

	if( !lpheight )
		return S_FALSE;

	*lpheight = pThis->m_Grid.GetRowHeight( lrow );
	return S_OK;
}

HRESULT CGridViewBase::XGrid::SetRowHeight( long lrow, long lheight )
{
	METHOD_PROLOGUE_EX(CGridViewBase, Grid);

	pThis->m_Grid.SetRowHeight( lrow, lheight );
	return S_OK;
}

HRESULT CGridViewBase::XGrid::GetColumnWidth( long lcolumn, long *lpwidth )
{
	METHOD_PROLOGUE_EX(CGridViewBase, Grid);

	if( !lpwidth )
		return S_FALSE;
    
    *lpwidth = pThis->m_Grid.GetColumnWidth( lcolumn );
	return S_OK;
}

HRESULT CGridViewBase::XGrid::SetColumnWidth( long lcolumn, long lwidth )
{	
	METHOD_PROLOGUE_EX(CGridViewBase, Grid);

	pThis->m_Grid.SetColumnWidth( lcolumn, lwidth );
	return S_OK;
}

///////////////////////////
//grid multiframe
HRESULT CGridViewBase::XFrame::SelectObject( IUnknown* punkObj, BOOL bAddToExist )
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	return S_FALSE;
}

HRESULT CGridViewBase::XFrame::UnselectObject( IUnknown* punkObj )
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	return S_FALSE;
}

HRESULT CGridViewBase::XFrame::GetObjectsCount(DWORD* pnCount)
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	*pnCount = (DWORD)pThis->m_selection.GetSize();
	return S_OK;
}

HRESULT CGridViewBase::XFrame::GetObjectByIdx(DWORD nIdx, IUnknown** ppunkObj)
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	*ppunkObj = pThis->m_selection[nIdx];
	if( *ppunkObj )(*ppunkObj)->AddRef();
	return S_OK;
}

HRESULT CGridViewBase::XFrame::GetFlags(DWORD* pnFlags)
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	* pnFlags = 0;
	return S_OK;
}

HRESULT CGridViewBase::XFrame::SetFlags(DWORD nFlags)
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	return S_OK;
}

HRESULT CGridViewBase::XFrame::GetFrameOffset(POINT* pptOffset)
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	pptOffset->x = 0;
	pptOffset->y = 0;
	return S_OK;
}

HRESULT CGridViewBase::XFrame::SetFrameOffset(POINT ptOffset)
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	return S_OK;
}

HRESULT CGridViewBase::XFrame::GetObjectOffsetInFrame(IUnknown* punkObject, POINT* pptOffset)
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	pptOffset->x = 0;
	pptOffset->y = 0;

	return S_OK;
}

HRESULT CGridViewBase::XFrame::RedrawFrame()
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	return S_OK;
}

HRESULT CGridViewBase::XFrame::EmptyFrame()
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	return S_OK;
}

HRESULT CGridViewBase::XFrame::SyncObjectsRect()
{
	METHOD_PROLOGUE_EX(CGridViewBase, Frame);
	return S_OK;
}

void CGridViewBase::GetMultiFrame(IUnknown **ppunkObject, bool bDragFrame)
{
	bDragFrame;

	*ppunkObject = GetControllingUnknown();
	(*ppunkObject)->AddRef();
}

void	CGridViewBase::_selection_from_range()
{
	_clear_selection();

	POSITION pos = m_Grid.m_Sel.GetRanges().GetHeadPosition();

	CCellID	cellFocus = m_Grid.GetFocusCell();
	bool	bAddFocused = cellFocus.row >= rowFirstObject() && 
		cellFocus.row < m_Grid.GetAdditionRow() &&
		cellFocus.col >= colFirstParameter();

	while( pos )
	{
		CCellID	cell;
		CCellRange range = m_Grid.m_Sel.GetRanges().GetNext( pos );

		if( range.InRange( cellFocus ) )
			bAddFocused = false;

		long	row, col;
		for( col = range.Left(); col <= range.Right(); col++ )
			for( row = range.Top(); row <= range.Bottom(); row++ )
			{
				IUnknown	*punk = _object_from_cell( row, col );
				if( !punk )continue;
				if( _is_selected( punk ) )
				{
					punk->Release();
					continue;
				}
				m_selection.Add( punk );
			}
	}


	if( bAddFocused )
	{
		long c = cellFocus.col;
		IUnknown	*punk = _object_from_cell( cellFocus.row, c );
		if( punk )
		{
			if( !_is_selected( punk ) )
			{
				m_selection.Add( punk );

/*				IUnknown	*punkParent;
				INamedDataObject2Ptr	ptrN( punk );
				ptrN->GetParent( &punkParent );

				if( punkParent )
				{
					INamedDataObject2Ptr	ptrNP( punk );
					punkParent->Release();
					long	lposA;
					ptrN->GetObjectPosInParent( &lposA );
					ptrNP->SetActiveChild( lposA );
				}*/
			}
			else
				punk->Release();
		}
	}
}

void	CGridViewBase::_range_from_selection()
{
/*clear_selection();

	POSITION	pos =m_Grid.m_SelectedCellMap.GetStartPosition();

	while( pos )
	{
		CCellID	cell;
		DWORD	dw;

		m_Grid.m_SelectedCellMap.GetNextAssoc( pos, dw, cell );

		IUnknwon	*punk = _object_from_cell( cell.row, cell.col );
		if( !punk )continue;

		if( _is_selected( punk ) )
		{
			punk->Release();
			continue;
		}
		m_selection.Add( punk );
	}*/
}

void	CGridViewBase::_clear_selection()
{
	for( long l = 0; l < m_selection.GetSize(); l++ )
		m_selection[l]->Release();

	m_selection.RemoveAll();
}

IUnknown *CGridViewBase::_object_from_cell( long row, long &col, bool bChParam/* = false */)
{
	if( row == m_Grid.GetAdditionRow() )return 0;
	RowInfo	*prow = m_rows[row];
	if( !prow->pcalc )return 0;
	prow->pcalc->AddRef();
	return prow->pcalc;
}

bool	CGridViewBase::_is_selected( IUnknown *punk )
{
	for( long l = 0; l < m_selection.GetSize(); l++ )
	{
		if( m_selection[l] == punk )
			return true;
	}
	return false;
}

void CGridViewBase::OnChangeSelection()
{
	_selection_from_range();
}

void CGridViewBase::_add_result_to_grid()
{
	if( m_rowPrintResult != -1 )
		return;

	m_Grid.DeleteAdditionRow();
	m_Grid.SetAdditionRow( false );
	m_Grid.SetFocusCell( -1, -1 );

	m_rowPrintResult = m_Grid.GetRowCount();
	CResParamArray		&param = m_ResultBar.GetParamArray();
	m_Grid.SetRowCount( (int)m_rowPrintResult+param.GetSize() );

	long	row, col, idx, n;
	for( row = m_rowPrintResult, idx = 0; idx < param.GetSize(); idx++, row++ )
	{
		CResultParam	&result = param[idx];
		CValueArray &values = result.GetValues();

		col = colFirstParameter()-1;

		GV_ITEM item;
		item.mask = GVIF_TEXT|GVIF_STATE|GVIF_BKCLR;
		item.row = row;
		item.col = 0;
		item.nState = GVIS_FIXED;
		item.crBkClr = GetSysColor( COLOR_3DFACE );
		item.strText = result.m_strName;
		m_Grid.SetItem( &item );

		for( col = colFirstParameter(), n = 0; col < m_Grid.GetColumnCount(); col++, n++ )
		{
			//item.row = row;
			item.col = col;

			SValue	&value = values[n];

			if( value.bEmpty )
				item.strText = "--";
			else
				item.strText.Format( "%g", value.fValue );

			m_Grid.SetItem( &item );
		}
	}
}

void CGridViewBase::_remove_result_from_grid()
{
	if( m_rowPrintResult == -1 )return;
	m_Grid.SetRowCount( m_rowPrintResult );
	m_rowPrintResult = -1;
    m_Grid.SetAdditionRow( true );
    m_Grid.AddAdditionRow();
}

BOOL	CGridViewBase::SetRowHeightByObject( long lrow, IUnknown *punkObject )
{
	if( !punkObject )
		return FALSE;

	INamedPropBagPtr sptrPropBag( punkObject ); 

	if( sptrPropBag != 0 )
	{
		_variant_t var;
		sptrPropBag->GetProperty( bstr_t( "RowHeight" ), &var );
		long lOldHeight = m_Grid.GetRowHeight( lrow );
		if( var.vt != VT_EMPTY /*&& lOldHeight != var.lVal*/ )
		{
			m_Grid.SetRowHeight( lrow, var.lVal );
            if( lOldHeight != var.lVal )
			{
				m_Grid.Invalidate( 0 );
				m_Grid.UpdateWindow( );
			}
		}
    }


	return TRUE;
}

bool CGridViewBase::DoTranslateAccelerator( MSG *pMsg )
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
	{
		if (m_hAccel != NULL)
		{
			if (::TranslateAccelerator(m_Grid, m_hAccel, pMsg))
				return true;
		}
	} 

	return false;
}
// update active cell but no scroll horizontally
void CGridViewBase::UpdateActiveCellNoHScroll(void)
{
	if( m_objects == 0 || m_container == 0 )
		return ;

	CCellRange cr = m_Grid.GetVisibleNonFixedCellRange();
	
	long	row = m_Grid.GetAdditionRow(), col = 0;

	//active row
	POSITION	lpos;
	m_objects->GetActiveChild( &lpos );
	if( lpos )
	{
		IUnknown	*punkO = 0;
		m_objects->GetNextChild( &lpos, &punkO );
		if( punkO )
		{
			ICalcObjectPtr	ptrC=punkO;
			punkO->Release();

			if( ptrC != 0 )
			{
				RowInfo	*prow = _row_by_object( ptrC );
				if(prow) row  = prow->row;
			}
		}
	}

	long	lkey = -1;
	m_container->GetCurentPosition( (LONG_PTR*)&lpos, &lkey );

	if( lkey != -1 )
	{
		ParameterContainer	*pc = _param_by_key( lkey );
		if( pc )
		{
			ColInfo	*pcol = _col_by_param( pc );
			col = pcol->col;
		}
	}
	
	m_Grid.ResetSelectedRange();	
	{
		m_Grid.SetSelectedRange( row, col, row, col );
		int c = (cr.Left()<=col && cr.Right()>=col)? col:cr.Left();
		m_Grid.EnsureVisible( row, c );
		if( row == m_Grid.GetAdditionRow()-1 )
			m_Grid.EnsureVisible( m_Grid.GetAdditionRow(), c);
		m_Grid.SetFocusCell( row,c);
	}

	_selection_from_range();


}
