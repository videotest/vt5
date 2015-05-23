// ClassView.cpp : implementation file
//

#include "stdafx.h"
#include "dataview.h"
#include "ClassView.h"
#include "NameConsts.h"
#include "ColourPopup.h"



// predefines 
//UINT GetLargestCmdIDFromMenu(HMENU hMenu);

/////////////////////////////////////////////////////////////////////////////
// CClassView

IMPLEMENT_DYNCREATE(CClassView, CCmdTargetEx)

// {03E6BDBA-E120-4f0b-B7EF-0121C1C8877C}
GUARD_IMPLEMENT_OLECREATE(CClassView, "DataView.ClassView",
0x3e6bdba, 0xe120, 0x4f0b, 0xb7, 0xef, 0x1, 0x21, 0xc1, 0xc8, 0x87, 0x7c);


POSITION CClassView::GetFisrtVisibleObjectPosition()
{
	if( m_container != 0 )
		return (POSITION)1;
	return 0;
}

IUnknown *CClassView::GetNextVisibleObject( POSITION &rPos )
{
	if( (long)rPos == 1 )
	{
		rPos = 0;
		IUnknown	*punk = m_container;
		if( punk )punk->AddRef();
		return punk;
	}
	return 0;
}
	
CClassView::CClassView()
{
	m_uViewMode = 0;
	EnableAutomation();
	EnableAggregation();

	_OleLockApp( this );

	SetParamMgrName(szStatistic);
	m_strSection = "\\statistics";

	m_SelectedColorCell = CCellID(-1, -1);
	
	m_sName = c_szCClassView;
	m_sUserName.LoadString(IDS_CLASS_VIEW);	


	LoadState();
}

CClassView::~CClassView()
{
	TRACE( "CClassView::~CClassView()\n" );
	_OleUnlockApp( this );
}

void CClassView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CGridViewBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CClassView, CGridViewBase)
	//{{AFX_MSG_MAP(CClassView)
	//}}AFX_MSG_MAP
    ON_MESSAGE(CPN_SELENDOK,     OnSelEndOK)
    ON_MESSAGE(CPN_SELENDCANCEL, OnSelEndCancel)
    ON_MESSAGE(CPN_SELCHANGE,    OnSelChange)
    ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_CTRL, OnEndLabelEdit)
END_MESSAGE_MAP()



BEGIN_INTERFACE_MAP(CClassView, CGridViewBase)
	INTERFACE_PART(CClassView, IID_IClassView,		 ClassView)
//	INTERFACE_PART(CClassView, IID_IGridView,		 Grid)
//	INTERFACE_PART(CClassView, IID_IMenuInitializer, GridMenu)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CClassView, ClassView);


/////////////////////////////////////////////////////////////////////////////
// CClassView message handlers



// overrite virtuals
LPCTSTR	CClassView::GetObjectType()
{
	return szTypeClassList;
}

bool CClassView::DefaultInit()
{
	if (!CGridViewBase::DefaultInit())
		return false;

	m_Grid.SetRowResize(false);
	m_Grid.SetColumnCount(3);
	m_Grid.SetEditable(true);

	m_Grid.SetItemFormat(0, 0, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(0, 0, "Num");
	m_Grid.SetItemData(0, 0, -1);
//	m_Grid.SetItemState(0, 0, m_Grid.GetItemState(0, 0) & GVIS_READONLY);

	m_Grid.SetItemFormat(0, 1, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(0, 1, "Color");
	m_Grid.SetItemData(0, 1, -1);
//	m_Grid.SetItemState(0, 1, m_Grid.GetItemState(0, 1) & GVIS_READONLY);

	m_Grid.SetItemFormat(0, 2, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(0, 2, "Name");
	m_Grid.SetItemData(0, 2, -1);
//	m_Grid.SetItemState(0, 2, m_Grid.GetItemState(0, 2) & GVIS_READONLY);

	m_Grid.SetItemFormat(1, 0, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(1, 0, "*");
	m_Grid.SetItemData(1, 0, -1);
//	m_Grid.SetItemState(1, 0, m_Grid.GetItemState(1, 0) & GVIS_READONLY);

	m_Grid.SetItemFormat(1, 1, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(1, 1, "");
	m_Grid.SetItemData(1, 1, -1);
//	m_Grid.SetItemState(1, 1, m_Grid.GetItemState(1, 1) & GVIS_READONLY);
	
	m_Grid.SetItemFormat(1, 2, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(1, 2, "");
	m_Grid.SetItemData(1, 2, -1);
//	m_Grid.SetItemState(1, 2, m_Grid.GetItemState(1, 2) & GVIS_READONLY);

	return true;//AddUnkClass();
}



LRESULT CClassView::OnSelEndOK(WPARAM lParam, LPARAM /*wParam*/)
{
/*    COLORREF crNewColour = (COLORREF) lParam;

	if (!m_SelectedColorCell.IsValid())
		return false;

	COLORREF clOld = m_Grid.GetItemBkColour(m_SelectedColorCell.row, m_SelectedColorCell.col);

    m_Grid.SetItemBkColour(m_SelectedColorCell.row, m_SelectedColorCell.col, crNewColour);
    m_Grid.SetItemFgColour(m_SelectedColorCell.row, m_SelectedColorCell.col, crNewColour);

    if (crNewColour != clOld)
		m_Grid.RedrawCell(m_SelectedColorCell);
	
	IClassObjectPtr sptrClass = GetObject(m_SelectedColorCell.row, false);
	if (sptrClass)
		sptrClass->SetColor((DWORD)crNewColour);

	m_Grid.SetFocusCell(m_SelectedColorCell);
	m_SelectedColorCell = CCellID(-1, -1);
	m_Grid.SetFocus();*/

    return TRUE;
}

LRESULT CClassView::OnSelEndCancel(WPARAM lParam, LPARAM /*wParam*/)
{
    COLORREF crNewColour = (COLORREF) lParam;
	
	if (!m_SelectedColorCell.IsValid())
		return false;

	COLORREF clOld = m_Grid.GetItemBkColour(m_SelectedColorCell.row, m_SelectedColorCell.col);

    m_Grid.SetItemBkColour(m_SelectedColorCell.row, m_SelectedColorCell.col, crNewColour);
    m_Grid.SetItemFgColour(m_SelectedColorCell.row, m_SelectedColorCell.col, crNewColour);

    if (crNewColour != clOld)
		m_Grid.RedrawCell(m_SelectedColorCell);

	m_Grid.SetFocusCell(m_SelectedColorCell);
	m_SelectedColorCell = CCellID(-1, -1);
	m_Grid.SetFocus();
    return TRUE;
}

LRESULT CClassView::OnSelChange(WPARAM lParam, LPARAM /*wParam*/)
{
    COLORREF crNewColour = (COLORREF) lParam;
	if (!m_SelectedColorCell.IsValid())
		return false;

	COLORREF clOld = m_Grid.GetItemBkColour(m_SelectedColorCell.row, m_SelectedColorCell.col);
    m_Grid.SetItemBkColour(m_SelectedColorCell.row, m_SelectedColorCell.col, crNewColour);
    m_Grid.SetItemFgColour(m_SelectedColorCell.row, m_SelectedColorCell.col, crNewColour);

    if (crNewColour != clOld)
		m_Grid.RedrawCell(m_SelectedColorCell);

    return TRUE;
}


void CClassView::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_GRIDVIEW* pnmgv	= (NM_GRIDVIEW*)pNMHDR;
//    GV_DISPINFO *pgvDispInfo = (GV_DISPINFO *)pNMHDR;
//    GV_ITEM     *pgvItem = &pgvDispInfo->item;

/*	int nRow = pnmgv->iRow;
	int nCol = pnmgv->iColumn;
	*pResult = 0;

	if (nCol == 2)
	{
		CVTGridCell* pCell = m_Grid.GetGridCell(nRow, nCol);
		if (!pCell || pCell->GetType() != egctClassName)
			return;

		CString strNewName = pCell->GetText();

		INamedObject2Ptr sptrObj = GetObject(nRow, false);
		if (sptrObj)
		{
			BSTR bstrName = 0;
			sptrObj->GetName(&bstrName);
			CString strOldName = bstrName;
			::SysFreeString(bstrName);
			
			if (strNewName == strOldName)
				return;

			bstrName = strNewName.AllocSysString();
			sptrObj->SetName(bstrName);
//			sptrObj->SetUserName(bstrName);
			::SysFreeString(bstrName);
			*pResult = TRUE;
		}
	}*/
	*pResult = TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CColorCell 
IMPLEMENT_DYNCREATE(CColorCell, CVTGridCell);

HCURSOR CColorCell::s_hColorCursor = 0;

BOOL CColorCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	bEraseBkgnd;
	m_nRow = nRow;
	m_nCol = nCol;

    if (!m_pGrid || !pDC)
        return FALSE;
    
    int nSavedDC = pDC->SaveDC();
    int nOldMode = pDC->SetBkMode(TRANSPARENT);
    
    BOOL bPrinting = FALSE;
#if !defined(GRIDCONTROL_NO_PRINTING)
    bPrinting = pDC->IsPrinting();
#endif
    
    // Set up text and background colours
    
	CBrush bkBrush(m_pGrid->GetFixedBkColor());
	CBrush brush(m_crBkClr);

    // Draw cell background and highlighting (if necessary)
//	rect.InflateRect(1, 1);
	pDC->FillRect(rect, &bkBrush);
//	rect.DeflateRect(1, 1);


	int nDiam = rect.Height() < rect.Width() ? rect.Height() : rect.Width();
	CRect rcCycle;
	rcCycle.left = rect.left + (rect.Width() - nDiam) / 2;
	rcCycle.top = rect.top + (rect.Height() - nDiam) / 2;
	rcCycle.right = rcCycle.left + nDiam;
	rcCycle.bottom = rcCycle.top + nDiam;
	CRgn rgn;
	rgn.CreateEllipticRgnIndirect(rcCycle);
	
	pDC->FillRgn(&rgn, &brush);

    if ((((m_nState & GVIS_FOCUSED) == GVIS_FOCUSED) || 
		 ((m_nState & GVIS_DROPHILITED) == GVIS_DROPHILITED)) && !bPrinting)
    {
		rect.InflateRect(1, 1);
		pDC->DrawEdge(rect, EDGE_RAISED, BF_RECT);
		rect.DeflateRect(1, 1);
    }

	rgn.DeleteObject();

	bkBrush.DeleteObject();
	brush.DeleteObject();
    
    pDC->SetBkMode(nOldMode);
    pDC->RestoreDC(nSavedDC);
    
    return TRUE;
}

BOOL CColorCell::Edit(int nRow, int nCol, CRect rect, CPoint point, UINT nID, UINT nChar)
{
	if (!m_pGrid)
		return false;

	CClassView * pView = (CClassView*)m_pGrid->GetParent();
	if (!pView)
		return false;

	if (m_nRow == -1 || m_nCol == -1)
		return false;

	pView->m_SelectedColorCell = CCellID(nRow, nCol);
	pView->m_cOldColor = m_crBkClr;

	CRect rc = rect;
	m_pGrid->ClientToScreen(&rc);

	new CColourPopup(CPoint(rc.left, rc.bottom), GetBackClr(), pView, _T(""), _T("More colors ..."));
	return false;
}

void CColorCell::EndEdit()
{
}

void CColorCell::OnClick()
{
/*	if (!m_pGrid)
		return;

	CClassView * pView = (CClassView*)m_pGrid->GetParent();
	if (!pView)
		return;

	if (m_nRow == -1 || m_nCol == -1)
		return;

	pView->m_SelectedColorCell = CCellID(m_nRow, m_nCol);
	pView->m_cOldColor = m_crBkClr;

	CRect rc;
	m_pGrid->GetCellRect(CCellID(m_nRow, m_nCol), &rc);
	m_pGrid->ClientToScreen(&rc);
//	pView->ScreenToClient(&rc);

	new CColourPopup(CPoint(rc.left, rc.bottom), GetBackClr(), pView, _T(""), _T("More colors ..."));
*/
}

BOOL CColorCell::OnSetCursor()
{
    SetCursor(GetCursor());
	return true;
}


HCURSOR CColorCell::GetCursor()
{
	if (!s_hColorCursor)		// No cursor handle - load our own
	{
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
		s_hColorCursor = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_COLOR));
	}
	return s_hColorCursor;
}

