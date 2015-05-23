#include "stdafx.h"
#include "AuditTrailTextView.h"
#include "resource.h"
#include "MenuRegistrator.h"
/////////////////////////////////////////////////////////////////////////////
// CAuditTrailTextView

#define GRID_ID 111111

BOOL CATGrid::SetColumnWidth(int col, int width)
{
	BOOL bRes = CGridCtrl::SetColumnWidth( col, width );
	
	if( bRes && m_pView )
		m_pView->OnSetColumnWidth( col, width );

	return bRes;
}



IMPLEMENT_DYNCREATE(CAuditTrailTextView, CCmdTarget)

CAuditTrailTextView::CAuditTrailTextView()
{
	m_sName = c_szCAuditTrailTextView;
	m_sUserName.LoadString(IDS_AT_TEXT_VIEW_NAME);
	EnableAggregation();
	m_sptrActiveAT = 0;	
	m_bShowActionParams = false;
	_OleLockApp( this );

	m_Grid.SetATView( this );
	m_bCanProcessWidthHandler = false;
}

CAuditTrailTextView::~CAuditTrailTextView()
{
	_OleUnlockApp( this );
}


BEGIN_MESSAGE_MAP(CAuditTrailTextView, CViewBase)
	//{{AFX_MSG_MAP(CAuditTrailTextView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_ERASEBKGND()
	ON_NOTIFY(GVN_SELCHANGED,	GRID_ID, OnSelChangedGrid)
	ON_WM_NCDESTROY()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// {E86D93F8-AFA7-4802-A0E5-D33614BF781C}
GUARD_IMPLEMENT_OLECREATE(CAuditTrailTextView, "AuditTrail.AuditTrailTextView", 
0xe86d93f8, 0xafa7, 0x4802, 0xa0, 0xe5, 0xd3, 0x36, 0x14, 0xbf, 0x78, 0x1c);

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailTextView message handlers

int CAuditTrailTextView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CViewBase::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	CRect rcClient = NORECT;
	GetClientRect(&rcClient);

	if (!m_Grid.Create(rcClient, this, GRID_ID))
		return -1;
	m_Grid.ModifyStyleEx(WS_BORDER, 0);

	m_Grid.SetFixedRowCount(1);


	m_Grid.EnableDragColumn(FALSE);
	m_Grid.SetEditable(FALSE);
	m_Grid.SetListMode();
	m_Grid.SetSingleRowSelection();
	m_Grid.SetHeaderSort(FALSE);
	m_Grid.SetDrawFocus(FALSE);
	m_Grid.EnableTitleTips( false );
	
	sptrIScrollZoomSite	sptrZS(GetControllingUnknown());
	sptrZS->SetAutoScrollMode(false);

	CString strHeader;
	long nColCounter = 2;
	m_bShowActionParams = ::GetValueInt(GetAppUnknown(), "AuditTrail", "ShowActionParams", 1) == 1;
	nColCounter +=  m_bShowActionParams ? 1 : 0;
	for(long i = 0; i < nColCounter; i++)
	{
		strHeader.LoadString(IDS_COL_HEADER_0 + i);
		int nCol = m_Grid.InsertColumn(strHeader); 
		m_Grid.SetColumnWidth(nCol, 200);
	}
	
	_RebuildList();
	
	return 0;
}

void CAuditTrailTextView::OnSize(UINT nType, int cx, int cy) 
{
	CViewBase::OnSize(nType, cx, cy);

	CRect rcClient = NORECT;
	GetClientRect(&rcClient);
	m_Grid.MoveWindow(&rcClient, true);

	/*IScrollZoomSitePtr sptrSZ(GetControllingUnknown());
	if(sptrSZ != 0)
	{
		CRect rcWindow = NORECT;
		m_ctrlList.GetWindowRect(&rcWindow);
		sptrSZ->SetClientSize(CSize(rcWindow.Width(), rcWindow.Height()));
	}*/
}

void CAuditTrailTextView::AttachActiveObjects()
{
	IAuditTrailObjectPtr sptrOldAT = m_sptrActiveAT;

	save_grid_data_to_object( sptrOldAT );

	m_sptrActiveAT = 0;

	IDataContextPtr	sptrContext(GetControllingUnknown());
	if(sptrContext == 0)
		return;

	{
	IUnknown* punkAT = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeAuditTrailObject), &punkAT);
	m_sptrActiveAT = punkAT;
	if(punkAT)
		punkAT->Release();

	if(m_sptrActiveAT != 0)
	{
		if( m_sptrActiveAT )
			m_sptrActiveAT->ReloadSettings( );

		_RebuildList();
		return;
	}
	}

	IUnknown* punkImage = 0;
	IUnknown* punkAT = 0;
	sptrContext->GetActiveObject(_bstr_t(szTypeImage), &punkImage);

	if(!punkImage)
		return;
	
	IUnknown* punkATM = _GetOtherComponent(GetAppUnknown(), IID_IAuditTrailMan);
	IAuditTrailManPtr sptrATman(punkATM);
	if(punkATM)
		punkATM->Release();
	
	if(sptrATman != 0)
	{
		sptrATman->GetAuditTrail(punkImage, &punkAT);
		m_sptrActiveAT = punkAT;
		if(punkAT)
			punkAT->Release();
	}

	punkImage->Release();

	if(sptrOldAT != m_sptrActiveAT && IsWindow(m_hWnd))
		_RebuildList();

}

void CAuditTrailTextView::_RebuildList()
{
	if(m_sptrActiveAT == 0 || !m_Grid.m_hWnd)
		return;

	m_bCanProcessWidthHandler = false;

	m_Grid.SetRowCount(1);
	
	long nCount = 0;
	m_sptrActiveAT->GetThumbnailsCount(&nCount);

	long nColsCount = 2 + (m_bShowActionParams ? 1 : 0);

	for(long i = 0; i < nCount; i++)
	{
		_AddToList(i);
	}

	long nIndex = 0;
	m_sptrActiveAT->GetActiveThumbnail(&nIndex);
	m_Grid.SetSelectedRange(nIndex+1, 0, nIndex+1, nColsCount-1);
	m_Grid.SetFocusCell(nIndex+1, 0);

	load_grid_data_from_object( m_sptrActiveAT );

	m_bCanProcessWidthHandler = true;

	Invalidate();
}

void CAuditTrailTextView::_AddToList(long nPos)
{
	long nColsCount = 2 + (m_bShowActionParams ? 1 : 0);
	BSTR bstrActionName = 0, bstrActionParams = 0, bstrActionTime = 0;
	m_sptrActiveAT->GetThumbnailInfo(nPos, &bstrActionName, &bstrActionParams, &bstrActionTime);

	CString str[3];
	str[0] = bstrActionTime;
	str[1] = bstrActionName;
	str[2] = bstrActionParams;
	int nRow = m_Grid.InsertRow(0);
	if (nRow > 0)
	{
		for(long j = 0; j < nColsCount; j++)
		{
			m_Grid.SetItemText(nRow, j, str[j]);
		}
	}
	::SysFreeString(bstrActionName);
	::SysFreeString(bstrActionParams);
	::SysFreeString(bstrActionTime);
}

POSITION CAuditTrailTextView::GetFisrtVisibleObjectPosition()
{
	return (POSITION)(int)(m_sptrActiveAT != 0);
}

IUnknown *CAuditTrailTextView::GetNextVisibleObject( POSITION &rPos )
{
	IUnknown* punkRet = m_sptrActiveAT;
	rPos = 0;
	if(punkRet)
		punkRet->AddRef();
	return punkRet;
}

void CAuditTrailTextView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	m_Grid.SendMessage(WM_HSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)(pScrollBar ? pScrollBar->m_hWnd : 0));
}

void CAuditTrailTextView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// TODO: Add your message handler code here and/or call default
	
	m_Grid.SendMessage(WM_VSCROLL, MAKELONG(nSBCode, nPos), (LPARAM)(pScrollBar ? pScrollBar->m_hWnd : 0));
}


BOOL CAuditTrailTextView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}

void CAuditTrailTextView::OnSelChangedGrid(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pnmgv	= (NM_GRIDVIEW*)pNMHDR;
	int nRow = pnmgv->iRow;
	int nCol = pnmgv->iColumn;
	
	*pResult = 0;

	long nIndex = 0;
	m_sptrActiveAT->GetActiveThumbnail(&nIndex);
	
	if(nRow-1 != nIndex)
	{
		m_sptrActiveAT->SetActiveThumbnail(nRow-1);
	}
}

void CAuditTrailTextView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if(!strcmp(pszEvent, szEventChangeObject))
	{
		if(m_sptrActiveAT == 0)//this situation can occur after filter action
			AttachActiveObjects();

		if(::GetObjectKey(punkFrom) == ::GetObjectKey(m_sptrActiveAT))
		{
			short nNewActive = 0;
			if( pdata )
			{
				long nVal = *(long*)pdata;
				short nPrevActive = (short)(nVal>>16);
				nNewActive = (short)nVal;
			}
			else
			{
				long nCount = 0;
				m_sptrActiveAT->GetThumbnailsCount(&nCount);

				if( m_Grid.GetRowCount() < nCount+1 )
					_AddToList(nCount-1);
				else
					m_Grid.SetRowCount( nCount+1 );

				nNewActive = nCount-1;
			}

			long nColsCount = 2 + (m_bShowActionParams ? 1 : 0);
			m_Grid.SetSelectedRange(nNewActive+1, 0, nNewActive+1, nColsCount-1);
			m_Grid.SetFocusCell(nNewActive+1, 0);
		}
	}
}

void CAuditTrailTextView::OnFinalRelease() 
{
	if( GetSafeHwnd() )
		DestroyWindow();

	delete this;
}

void CAuditTrailTextView::OnNcDestroy() 
{
	CViewBase::OnNcDestroy();
	
	// TODO: Add your message handler code here
	
}

void CAuditTrailTextView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	// TODO: Add your message handler code here
	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );
	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();
	if( sptrCM == NULL )
		return;

	CPoint pt = point;
	ScreenToClient( &pt );

	pt = ::ConvertToClient( GetControllingUnknown(), pt );

	CMenuRegistrator rcm;	
	CString strMenuName;
	
	if( strMenuName.IsEmpty() )
	{
		CString strViewName = GetViewMenuName();
		strMenuName = (LPCSTR)rcm.GetMenu( strViewName, 0 );
	}		

	if( strMenuName.IsEmpty() )
		return;
	
	sptrCM->ExecuteContextMenu2( _bstr_t(strMenuName), point, 0 );	
}

const char* CAuditTrailTextView::GetViewMenuName()
{
	return szAuditTrailTextView;
}


bool CAuditTrailTextView::save_grid_data_to_object( IUnknown* punkObject )
{
	if( !m_Grid.GetSafeHwnd() )
		return false;

	IAuditTrailObjectPtr ptrAT( punkObject );
	if( ptrAT == 0 )
		return false;


	grid_data	gd;
	::ZeroMemory( &gd, sizeof(grid_data) );
	
	gd.dwSize				= sizeof(grid_data);
	gd.dwTimeWidth			= m_Grid.GetColumnWidth( 0 );
	gd.dwActionNameWidth	= m_Grid.GetColumnWidth( 1 );
	gd.dwParameterWidth		= m_Grid.GetColumnWidth( 2 );

	ptrAT->SetGridData( (BYTE*)&gd, sizeof(grid_data) );

	return true;
}

bool CAuditTrailTextView::load_grid_data_from_object( IUnknown* punkObject )
{
	if( !m_Grid.GetSafeHwnd() )
		return false;

	IAuditTrailObjectPtr ptrAT( punkObject );
	if( ptrAT == 0 )
		return false;


	BYTE*	pBuf = 0;
	long	lSize = 0;

	ptrAT->GetGridData( &pBuf, &lSize );
	if( lSize < sizeof( grid_data ) )
		return false;

	grid_data*	pgd = (grid_data*)pBuf;

	if( pgd->dwTimeWidth > 0 )
		m_Grid.SetColumnWidth( 0, pgd->dwTimeWidth );
	
	if( pgd->dwActionNameWidth > 0 )
		m_Grid.SetColumnWidth( 1, pgd->dwActionNameWidth );

	if( pgd->dwParameterWidth > 0 )
		m_Grid.SetColumnWidth( 2, pgd->dwParameterWidth );	

	return true;
}

void CAuditTrailTextView::OnSetColumnWidth(int col, int width)
{
	if( !m_bCanProcessWidthHandler )
		return;

	save_grid_data_to_object( m_sptrActiveAT );
}


bool CAuditTrailTextView::GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX )
{
	return m_Grid.OnGetPrintWidth( nMaxWidth, nReturnWidth, nUserPosX, nNewUserPosX );
}

bool CAuditTrailTextView::GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY )
{
	return m_Grid.OnGetPrintHeight( nMaxHeight, nReturnHeight, nUserPosY, nNewUserPosY );
		
}

void CAuditTrailTextView::Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags)
{
	CDC* pdc = CDC::FromHandle(hdc);
	if (!pdc)return;

	int	nLastPosX = nUserPosX + nUserPosDX - 1;
	int	nLastPosY = nUserPosY + nUserPosDY - 1;

	CRect	RectDraw = CRect(nUserPosX, nUserPosY, nLastPosX, nLastPosY);
	
	m_Grid.OnPrintDraw(pdc, rectTarget, RectDraw );
}
