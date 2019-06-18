// MeasView.cpp : implementation file
//

#include "stdafx.h"
#include "dataview.h"
#include "MeasView.h"
#include "PropPage.h"

#include "NameConsts.h"
#include "Classes5.h"
#include "TimeTest.h"
#include "ObListWrp.h"
#include <math.h>
#include "thumbnail.h"
#include "\vt5\common2\class_utils.h"
#include "defs.h"	// sections
#include "MenuRegistrator.h"


IMPLEMENT_DYNCREATE(CVTImageCell, CVTGridCell);

/////////////////////////////////////////////////////////////////////////////
// CVTImageCell

CVTImageCell::CVTImageCell()
{	
}

CVTImageCell::~CVTImageCell()
{}

bool CVTImageCell::DrawImage( HDC hdc, IImage2 *pimage, RECT rect, COLORREF crBack, COLORREF crText, BOOL bForceStretch )
{
	if( !pimage )
		return true;

	static bool bFirstCall = true;
	static IRenderObject		*prender = 0;
	static IThumbnailManager	*pthumbmain = 0;
	if( bFirstCall )
	{
		bFirstCall = false;
		sptrIThumbnailManager spThumbMan( GetAppUnknown() );
		if( spThumbMan == NULL )return false;

		IUnknown* punkRenderObject = NULL;

		spThumbMan->GetRenderObject( pimage, &punkRenderObject );
		if( punkRenderObject == NULL )return false;

		sptrIRenderObject spRenderObject( punkRenderObject );
		punkRenderObject->Release();

		prender = spRenderObject;
		pthumbmain = spThumbMan;
	}

	if( !prender || !pthumbmain )return false;

	byte	*pdib = 0;

	CSize	size(rect.right-rect.left, rect.bottom-rect.top);

	// [vanek]: BT610 - 19.11.2003
	if( !bForceStretch )
	{
		int cx = 0, cy = 0;
		pimage->GetSize( &cx, &cy );
		if( cy < size.cy )
		{
			size.cy = cy;
			// centering by height
			rect.top = rect.top + (rect.bottom - rect.top) / 2 - size.cy / 2;
			rect.bottom = rect.top + size.cy;
		}

		if( cx < size.cx )
		{
			size.cx = cx;
			// centering by width
			rect.left = rect.left + ( rect.right - rect.left ) / 2 - size.cx / 2;
			rect.right = rect.left + size.cx;
		}
		
	}
	//

	short nSupported;
	prender->GenerateThumbnail( pimage, 24, RF_BACKGROUND, crBack, 
				size, &nSupported, &pdib, 0 );

	if( pdib == 0 )return false;

	pthumbmain->ThumbnailDraw( pdib, hdc, rect, 0 );
	pthumbmain->ThumbnailFree( pdib );

	return true;
}

void CVTImageCell::SetObject( IUnknown *punkObject )
{
	m_image = 0;
	IMeasureObjectPtr	ptrObject = punkObject;
	if( ptrObject == 0 )return;
	IUnknown	*punk = 0;
	ptrObject->GetImage( &punk );
	m_image = punk;
	if( punk )punk->Release();
}

BOOL CVTImageCell::Draw(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBkgnd)
{
	if( !CVTGridCell::Draw( pDC, nRow, nCol, rect, bEraseBkgnd ) )
		return false;

	rect.InflateRect( -1, -1 );

    CVTGridCtrl* pGrid = GetVTGrid();
    ASSERT(pGrid);

    if (!pGrid || !pDC)
        return FALSE;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

    int nSavedDC = pDC->SaveDC();
    int nOldMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF clrOldTextColor = pDC->GetTextColor();

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;
    if (GetTextClr() == CLR_DEFAULT)
        TextClr = pGrid->GetFixedTextColor();
    else
        TextClr = GetTextClr();

    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pGrid->GetFixedBkColor();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }
    if (GetState() & GVIS_SELECTED)
    {
        TextBkClr = ::GetSysColor(COLOR_3DDKSHADOW);
        TextClr = ::GetSysColor(COLOR_3DHIGHLIGHT);
	}
	CRect	rectImage = rect;
	//rectImage.InflateRect( -1, -1 );

    // Draw cell background and highlighting (if necessary)
    if ((GetState() & GVIS_FOCUSED) || (GetState() & GVIS_DROPHILITED))
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if (GetState() & GVIS_SELECTED)
        {
            TextBkClr = ::GetSysColor(COLOR_3DDKSHADOW);
            TextClr = ::GetSysColor(COLOR_3DHIGHLIGHT);
            bEraseBkgnd = TRUE;
        }

        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBkgnd)
        {
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
			brush.DeleteObject();
        }

        // Don't adjust frame rect if no grid lines so that the
        // whole cell is enclosed.
        if (pGrid->GetGridLines() != GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

        // Use same color as text to outline the cell so that it shows
        // up if the background is black.
        CBrush brush(TextClr);
        pDC->FrameRect(rect, &brush);
		brush.DeleteObject();
        pDC->SetTextColor(TextClr);

        // Adjust rect after frame draw if no grid lines
        if (pGrid->GetGridLines() == GVL_NONE)
        {
            rect.right--;
            rect.bottom--;
        }

        rect.DeflateRect(1,1);
    }
    else if ((GetState() & GVIS_SELECTED))
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DDKSHADOW));
        rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_3DHIGHLIGHT));
    }
	else
	{
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		CBrush brush(TextBkClr);
		pDC->FillRect(rect, &brush);
        rect.right--; rect.bottom--;

        pDC->SetTextColor(TextClr);
		brush.DeleteObject();

	}

	// [vanek]: BT610 - 19.11.2003
	DWORD dwFlags = 0;
	if( pGrid->m_pGridView )
		dwFlags = pGrid->m_pGridView->GetViewFlags();

	DrawImage( *pDC, m_image, rectImage, TextBkClr, TextClr, dwFlags & mvfStretchSmallImages );
	pDC->SetTextColor(clrOldTextColor);
    pDC->SetBkMode(nOldMode);
    pDC->RestoreDC(nSavedDC);

    return TRUE;
}






/////////////////////////////////////////////////////////////////////////////
// CMeasView


IMPLEMENT_DYNCREATE(CMeasView, CGridViewBase)

// {6FDA8C15-0959-4F25-971B-87FAE672A07A}
GUARD_IMPLEMENT_OLECREATE(CMeasView, "DataView.MeasView", 
0x6fda8c15, 0x959, 0x4f25, 0x97, 0x1b, 0x87, 0xfa, 0xe6, 0x72, 0xa0, 0x7a)

BEGIN_INTERFACE_MAP(CMeasView, CGridViewBase)
	INTERFACE_PART(CMeasView, IID_IMeasureView,		 MeasView)
	INTERFACE_PART(CMeasView, IID_IMenuInitializer, MenuInitializer)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CMeasView, MeasView);


CMeasView::CMeasView()
: m_bShowUnknown(false)
, m_iSkip(0)
{
	m_bShowClass = m_bShowImage = true;
	m_bStretchSmallImages = true;
    // [vanek] SBT: 864 - 25.03.2004
	m_bEditableGrid = true;	

	m_bEditableStandartParams = true;
	m_bEditableUniqueParams = true;

	m_uViewMode = 0;
	EnableAutomation();
	EnableAggregation();

	_OleLockApp( this );

	SetParamMgrName(szMeasurement);
	m_strSection = szMeasViewSection;//"\\measurement";
	m_ResultBar.SetSection( szMeasViewSection/*"\\measurement"*/ );

	m_sName = c_szMeasView;
	m_sUserName.LoadString(IDS_MEAS_VIEW);	

	m_strUnknownClass = _T("Unknown");
	m_clrUnknownClass = RGB(255, 255, 0);

	m_bShowUnknown = ::GetPrivateProfileInt("Classes", "AppointUnknownClass", 0, full_classifiername(NULL))!=0;
}

CMeasView::~CMeasView()
{
	TRACE("CMeasView::~CMeasView()\n");
	
	_OleUnlockApp( this );
}


void CMeasView::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CGridViewBase::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CMeasView, CGridViewBase)
	//{{AFX_MSG_MAP(CMeasView)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMeasView message handlers

HRESULT CMeasView::XMeasView::GetObjectList(IUnknown ** ppunkList)
{
	_try_nested(CMeasView, MeasView, GetObjectList)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CMeasView::XMeasView::GetActiveObject(IUnknown ** ppunkObject)
{
	_try_nested(CMeasView, MeasView, GetActiveObject)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CMeasView::XMeasView::SetActiveObject(IUnknown * punkObject)
{
	_try_nested(CMeasView, MeasView, SetActiveObject)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CMeasView::XMeasView::GetActiveParam(long * plParamKey)
{
	_try_nested(CMeasView, MeasView, GetActiveParam)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

HRESULT CMeasView::XMeasView::SetActiveParam(long lParamKey)
{
	_try_nested(CMeasView, MeasView, SetActiveParam)
	{
		return E_NOTIMPL;
	}
	_catch_nested;
}

LPCTSTR	CMeasView::GetObjectType()
{
	return szTypeObjectList;
}


bool CMeasView::FillClassArray(CStringArray & sa)
{
	sa.RemoveAll();
	
	// get active class List
	CObjectListWrp listC(::GetActiveObjectFromContext(GetControllingUnknown(), szTypeClassList), false);
	if (listC == 0)
		return false;

	// for all objects in list
	for (POSITION pos = listC.GetFirstObjectPosition(); pos != NULL;)
	{
		IUnknown * punk = listC.GetNextObject(pos);
		if (!punk)
			continue;

		sa.Add(::GetObjectName(punk));
		punk->Release();
	}
	
	return true;
}



DWORD CMeasView::GetMatchType(const char *szType)
{
	//BT 3281
	if( ::GetValueInt( ::GetAppUnknown(), m_strSection, "EnableViewMatch", 1L ) != 1L )
		return mvNone;

	if (!lstrcmp(szType, szTypeObject))
		return mvFull;

	else if (!lstrcmp(szType, szTypeObjectList))
		return mvFull;

	else
		return mvNone;
}



bool CMeasView::DefaultInit()
{
	if (!CGridViewBase::DefaultInit())
		return false;


	bool	bOldEnable = m_Grid.m_bEnableWidthNotify;
	m_Grid.m_bEnableWidthNotify = false;

	//m_Grid.SetEditable( m_bEditableGrid ); // 09.02.2006 build 98	[BT4901] - убрано
	m_Grid.SetHeaderSort(false);

	CString	str_no, str_class, str_view;

	str_no.LoadString( IDS_PARAM_NO );
	str_class.LoadString( IDS_PARAM_CLASS );
	str_view.LoadString( IDS_PARAM_VIEW );


	m_Grid.SetItemFormat(0, 0, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(0, 0, str_no );
	m_Grid.SetItemData(0, 0, -1);

	m_Grid.SetItemFormat(1, 0, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
	m_Grid.SetItemText(1, 0, "*");
	m_Grid.SetItemData(1, 0, -1);

	m_Grid.SetColumnWidth( colNumber(), GetValueInt( GetAppUnknown(), m_strSection, "NumWidth", 60 ) );
	
	if( colClass() >= 0 )
	{
		m_Grid.SetItemFormat(0, colClass(), DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_Grid.SetItemText(0, colClass(), str_class );
		m_Grid.SetItemData(0, colClass(), -1);
		m_Grid.SetItemFormat(1, colClass(), DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_Grid.SetItemText(1, colClass(), "");
		m_Grid.SetItemData(1, colClass(), -1);
		m_Grid.SetColumnWidth( colClass(), GetValueInt( GetAppUnknown(), m_strSection, "ClassWidth", 100 ) );
	}

	if( colImage() >= 0 )
	{
		m_Grid.SetItemFormat(0, colImage(), DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_Grid.SetItemText(0, colImage(), str_view );
		m_Grid.SetItemData(0, colImage(), -1);

		m_Grid.SetItemFormat(1, colImage(), DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		m_Grid.ChangeCellType(1, colImage(), egctImage);
		m_Grid.SetItemText(1, colImage(), "");
		m_Grid.SetItemData(1, colImage(), -1);

		//[vanek]: сначала считываем ширину столбца из NamedData ObjectList'a
		long lwidth = -1;
		if( m_objects != 0)
		{
			IUnknown *punkND = 0;
			m_objects->GetData( &punkND );
			if( !punkND )
			{
                m_objects->InitAttachedData( );
				m_objects->GetData( &punkND );
			}
			
			INamedDataPtr sptrND = punkND;
			if( punkND )
				punkND->Release(); punkND = 0;
            
			if( sptrND != 0 )
			{
	            _variant_t _var( long( -1 ) );
				sptrND->GetValue( _bstr_t( szMeasViewImageColumnWidthSection ), &_var );
				lwidth = _var.lVal;
			}
		}

		// если нет - берем из shell.data
		if( lwidth == -1 )
			lwidth = GetValueInt( GetAppUnknown(), m_strSection, /*"ImageWidth"*/szMeasViewImageColumnWidth, 60 );

        m_Grid.SetColumnWidth( colImage(), lwidth );
	}


	m_Grid.m_bEnableWidthNotify = bOldEnable;

	return true;
}


COLORREF CMeasView::GetCellBackColor(int nRow, int nCol)
{
	/*if (nRow >= m_Grid.GetFixedRowCount() && nCol == colImage() )
		return m_Grid.GetFixedBkColor();*/

	return CGridViewBase::GetCellBackColor(nRow, nCol);
}


void CMeasView::OnActivateView( bool bActivate, IUnknown *punkOtherView )
{
	IUnknown* punkPage = NULL;
	int nPageNum = -1;

	if( ::GetPropertyPageByProgID( szMeasurementPropPageProgID, &punkPage, &nPageNum) )
	{
		::ShowPropertyPage( nPageNum, bActivate );
		if( punkPage )
			punkPage->Release();
	}
}

CGridCellBase* CMeasView::CreateCell(int nRow, int nCol )
{
	CGridCellBase* pCell=0;
	if( nCol == colImage() && nRow >= rowFirstObject() )
	{
		pCell = new CVTImageCell();
	}

	if(!pCell)
		pCell = CGridViewBase::CreateCell( nRow, nCol );

	ParameterContainer *pc = 0;
	if( nCol>=0 && nCol<m_cols.GetSize() )
		pc = m_cols[nCol]->pparam;

	// 09.02.2006 build 98 [BT4901]
	if( (!m_bEditableGrid) && pc && !(pc->type == etUndefined && !pc->pDescr->pos) )
	{
		if(pCell)
			pCell->SetState( pCell->GetState() | GVIS_READONLY );

		//m_Grid.SetItemState( nRow, nCol, m_Grid.GetItemState(nRow, nCol) | GVIS_READONLY );
	}

	return pCell;
}

bool CMeasView::OnGetDispInfoRow( long row )
{
	if( !CGridViewBase::OnGetDispInfoRow( row ) )return true;
	RowInfo	*prow = m_rows[row];
	CGridCellBase*	pcell = 0;

	if( colImage() >= 0 )
	{
		pcell = m_Grid.GetCell( row, colImage() );

		if( pcell->IsKindOf( RUNTIME_CLASS(CVTImageCell) ) )
		{
			CVTImageCell	*pimageCell = (CVTImageCell*)pcell;
			pimageCell->SetObject( prow->pcalc );
			if( !pimageCell->HasImage( ) )
			{
				GV_ITEM	item;
				item.row = row;
				item.col = colImage();
				item.mask = GVIF_TEXT;
				item.strText = ::GetObjectName( prow->pcalc );
				m_Grid.SetItem( &item );
			}
		}
	}

	if( colClass() >= 0 )
	{
		GV_ITEM	item;
		item.row = row;
		item.col = colClass();
		item.mask = GVIF_TEXT|GVIF_FGCLR;

		long	lclass = m_ClassCache.get_object_class( prow->pcalc );
		item.strText = m_ClassCache.get_class_name( lclass ).c_str();
		item.crFgClr = m_ClassCache.get_class_color( lclass );

		m_Grid.SetItem( &item );
	}

	return true;
}

bool CMeasView::DefineParameter( ParameterContainer *pc, long col )
{
	bool bret = __super::DefineParameter( pc, col );
	if( bret && pc && pc->pDescr )
	{
		if( (!m_bEditableStandartParams && pc->pDescr->pos) || (!m_bEditableUniqueParams && !pc->pDescr->pos) ) 
            m_Grid.SetItemState( 0, col, m_Grid.GetItemState(0, col) | GVIS_READONLY );

		if( (!m_bEditableGrid) && !(pc->type == etUndefined && !pc->pDescr->pos) )
		{
			int nRows = m_Grid.GetRowCount();
			for(int row=0; row<nRows; row++)
			{
				m_Grid.SetItemState( row, col, m_Grid.GetItemState(row, col) | GVIS_READONLY );
			}
		}

	}
    
	return bret;
}

void CMeasView::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	CGridViewBase::OnNotify( pszEvent, punkHit, punkFrom, pdata, cbSize );

	if (!strcmp(pszEvent, szEventChangeObjectList))
	{
		if (punkFrom != NULL && ::GetObjectKey(m_objects) != ::GetObjectKey(punkFrom))
			return;

		if (punkFrom != NULL)
		{
			INamedDataObject2Ptr	ptrNamed( punkFrom );
			IUnknown	*punkParent = 0;
			ptrNamed->GetParent( &punkParent );
			if( !punkParent )return;

			IUnknownPtr	ptrP = punkParent;
			punkParent->Release();

			if (::GetObjectKey(m_objects) != ::GetObjectKey(ptrP))
				return;
		}

		UpdateActiveCell();
	}

}

bool CMeasView::LoadState()
{
	m_ClassCache.set_classifier(0); // "заглотить" активный классификатор

	m_clrUnknownClass = get_class_color( -1 );
		//::GetValueColor( GetAppUnknown(), szPluginClasses, szUnkClassColor, m_clrUnknownClass );
	m_strUnknownClass = get_class_name( -1 );
		//::GetValueString( GetAppUnknown(), szPluginClasses, szUnkClassName, m_strUnknownClass );
	m_bShowClass = ::GetValueInt( GetAppUnknown(), m_strSection, "ShowClass", m_bShowClass )!=0;
	m_bShowImage = ::GetValueInt( GetAppUnknown(), m_strSection, "ShowImage", m_bShowImage )!=0;
	m_bStretchSmallImages = ::GetValueInt(  GetAppUnknown(), m_strSection, "StretchSmallImages", m_bStretchSmallImages ) != 0;
	m_bEditableGrid = ::GetValueInt(  GetAppUnknown(), m_strSection, "EditableGrid", m_bEditableGrid ) != 0;
	m_bEditableStandartParams = ::GetValueInt(  GetAppUnknown(), m_strSection, "EditableStandartParams", m_bEditableStandartParams ) != 0;
	m_bEditableUniqueParams = ::GetValueInt(  GetAppUnknown(), m_strSection, "EditableUniqueParams", m_bEditableUniqueParams ) != 0;
	
	return CGridViewBase::LoadState();
}

IUnknown *CMeasView::_object_from_cell( long row, long &col, bool bChParam/* = false */)
{
	if( row == m_Grid.GetAdditionRow() )return 0;
	if( col < 0 || col >= m_Grid.GetColumnCount())return 0;
	RowInfo	*prow = m_rows[row];
	ColInfo	*pcol = m_cols[col];

	if( !prow->pcalc )return 0;
	if( !pcol->pparam )
	{
		// A.M. fix. BT4686, SBT1444
		prow->pcalc->AddRef();
		return prow->pcalc;
	}

	bool bScanLine = false;

	if( pcol->pparam->pDescr->pos == 0 )
	{
		long	lKey = pcol->pparam->pDescr->lKey;
		INamedDataObject2Ptr	ptrNamed( prow->pcalc );

		long	lpos;
		ptrNamed->GetFirstChildPosition( &lpos );

		while( lpos )
		{
			IUnknown	*punk = 0;
			ptrNamed->GetNextChild( &lpos, &punk );
			IManualMeasureObjectPtr	ptrMMO( punk );
			if( punk )punk->Release();
			if( ptrMMO == 0 )continue;

			long	lTestKey = -1;
			ptrMMO->GetParamInfo( &lTestKey, 0 );

			if( lTestKey == lKey )
				return ptrMMO.Detach();
		}
		
		bScanLine = true;
	}

	if( bScanLine )
	{
		int nSz = m_Grid.GetColumnCount();
		for( int i = col + 1; i < nSz; i++ )
		{
			pcol = m_cols[i];

			if( !prow->pcalc )return 0;
			if( !pcol->pparam )return 0;

			if( pcol->pparam->pDescr->pos == 0 )
			{
				long	lKey = pcol->pparam->pDescr->lKey;
				INamedDataObject2Ptr	ptrNamed( prow->pcalc );

				long	lpos;
				ptrNamed->GetFirstChildPosition( &lpos );

				while( lpos )
				{
					IUnknown	*punk = 0;
					ptrNamed->GetNextChild( &lpos, &punk );
					IManualMeasureObjectPtr	ptrMMO( punk );
					if( punk )punk->Release();
					if( ptrMMO == 0 )continue;

					long	lTestKey = -1;
					ptrMMO->GetParamInfo( &lTestKey, 0 );

					if( lTestKey == lKey )
					{
						if( bChParam )
							col = i;
						return ptrMMO.Detach();
					}
				}
			}
			else
			{

				if( bChParam )
					col = i;
				prow->pcalc->AddRef();
				return prow->pcalc;
			}
		}
	}

	if( bScanLine )
		return 0;

	prow->pcalc->AddRef();
	return prow->pcalc;
}


void CMeasView::AttachActiveObjects()
{
	CGridViewBase::AttachActiveObjects();

	IUnknown* punkPage = NULL;
	int nPageNum = -1;

	if( ::GetPropertyPageByProgID( szMeasurementPropPageProgID, &punkPage, &nPageNum) )
	{
		IOptionsPagePtr	ptrPage( punkPage );
		if( punkPage )punkPage->Release();

		if( ptrPage )ptrPage->LoadSettings();
	}
}

// [vanek]: BT610 - 19.11.2003
DWORD CMeasView::GetViewFlags()
{
	return m_bStretchSmallImages ? mvfStretchSmallImages : 0;
}

void CMeasView::StoreColumnWidth( long col, long width )
{
	if( col == colNumber() )
		SetValue( GetAppUnknown(), m_strSection, "NumWidth", width );
	else if( col == colClass() )
		SetValue( GetAppUnknown(), m_strSection, "ClassWidth", width );
	else if( col == colImage() )
	{
		// [vanek]
		bool bset_in_ND = false;
		if( m_objects != 0)
		{
			IUnknown *punkND = 0;
			m_objects->GetData( &punkND );
			if( !punkND )
			{
                m_objects->InitAttachedData( );
				m_objects->GetData( &punkND );
			}

			INamedDataPtr sptrND = punkND;
			if( punkND )
				punkND->Release(); punkND = 0;
            
			if( sptrND != 0 )
			{
				_variant_t _var( width );
				sptrND->SetValue( _bstr_t( szMeasViewImageColumnWidthSection ), _var );
				bset_in_ND = true;
			}
		}

		if( !bset_in_ND )
			SetValue( GetAppUnknown(), m_strSection, /*"ImageWidth"*/szMeasViewImageColumnWidth, width );
	}
}

HRESULT CMeasView::XMeasView::AlignByImage()
{
	METHOD_PROLOGUE_EX(CMeasView, MeasView);
	int ncol_img = pThis->colImage();
	if( ncol_img < 0 )	return S_FALSE;

	int ncol_width = 10;
	int nrow_count = pThis->m_Grid.GetRowCount();
	for( int nrow=0; nrow<nrow_count; nrow++ )
	{
		CGridCellBase* pcell = pThis->m_Grid.GetCell( nrow, ncol_img );
		if( pcell->IsKindOf( RUNTIME_CLASS(CVTImageCell) ) )		
		{
			CVTImageCell* pimageCell = (CVTImageCell*)pcell;
			
			int cx, cy;
			cx = cy = 0;
			IImagePtr ptr_image = pimageCell->GetRawImage();
			if( ptr_image )
			{
				ptr_image->GetSize( &cx, &cy );
				int def_height = pThis->m_Grid.GetDefCellHeight();
				if( cy && cy > def_height )
				{
					pThis->m_Grid.SetRowHeight( nrow, cy );
				}

				ncol_width = max( ncol_width, cx );
			}
		}
	}
    
	if( ncol_width )
	{
		// [vanek]: новое значение ширины должно быть сохранено
		bool bold_value = pThis->m_Grid.m_bEnableWidthNotify;
		pThis->m_Grid.m_bEnableWidthNotify = true;
		pThis->m_Grid.SetColumnWidth( ncol_img, ncol_width );
		pThis->m_Grid.m_bEnableWidthNotify = false;
	}

	pThis->SyncResBar( );		// [vanek] BT2000: 3538
	pThis->m_Grid.Invalidate( 0 );
	pThis->m_Grid.UpdateWindow();
	return S_OK;
}


HRESULT CMeasView::XMeasView::CopyToClipboard( DWORD dwFlags )
{
	METHOD_PROLOGUE_EX(CMeasView, MeasView);

	CWaitCursor wait;

	if (dwFlags&cf_grid)
	{
		CVTGridCtrl* pGrid = &pThis->m_Grid;
		pGrid->OnEditCopy();
		return S_OK;
	}

	bool bcopy_all = ( dwFlags == cf_all );

	if( !::OpenClipboard( pThis->m_hWnd ) )
		return S_FALSE;

	if( !EmptyClipboard() )
		return S_FALSE;

	IStream*	pi_stream = 0;
	::CreateStreamOnHGlobal( 0, FALSE, &pi_stream );
	if( !pi_stream )
	{			
		CloseClipboard();
		return S_FALSE;
	}

	//export here
	CCellRange range;
	if( bcopy_all )
		range = pThis->m_Grid.GetCellRange();
	else
	{
		range = pThis->m_Grid.GetSelectedCellRange();		
		if( !pThis->m_Grid.IsValid( range ) )
		{
			CCellID cid = pThis->m_Grid.GetFocusCell();
			range = CCellRange( cid.row, cid.col, cid.row, cid.col );
		}			
	}

	char sz_buf[1024];	sz_buf[0] = 0;
	DWORD dw_size = 0;
	HRESULT hr = S_OK;

	if( pThis->m_Grid.IsValid( range ) )
	{		
		for( int nrow = range.Top(); nrow <= range.Bottom(); nrow++ )
		{
			pThis->m_Grid.GetDispInfoRow( nrow );

			for( int ncol = range.Left(); ncol <= range.Right(); ncol++ )
			{
				CString str_text;
				if( !ncol && nrow > 1 && nrow < pThis->m_Grid.GetRowCount() - 1  )
					str_text.Format( "%d", nrow - 1 );
				else
					str_text = pThis->m_Grid.GetItemText( nrow, ncol );
					
				//write text
				dw_size = str_text.GetLength();
				hr = pi_stream->Write( (const char*)str_text, dw_size, &dw_size );
				//write tab
				if( ncol != range.Right() )
				{					
					strcpy( sz_buf, "\t" );
					dw_size = strlen( sz_buf );
					hr = pi_stream->Write( sz_buf, dw_size, &dw_size );
				}
			}
			
			//write new line
			if( nrow != range.Bottom() )
			{				
				strcpy( sz_buf, "\r\n" );
				dw_size = strlen( sz_buf );
				hr = pi_stream->Write( sz_buf, dw_size, &dw_size );
			}
		}
	}		

	//copy footer
	if( bcopy_all )
	{
		//write new line
		strcpy( sz_buf, "\r\n" );
		dw_size = strlen( sz_buf );
		hr = pi_stream->Write( sz_buf, dw_size, &dw_size );
		int ntabs = 0;
		if( pThis->m_bShowClass )	ntabs++;
		if( pThis->m_bShowImage )	ntabs++;

		pThis->m_ResultBar.PasteParams( pi_stream, ntabs );
	}

	//write "zero" char
	{
		char sz_zero = 0;
		DWORD dw_size = sizeof( sz_zero );
		pi_stream->Write( &sz_zero, dw_size, &dw_size );
	}

	HGLOBAL h = 0;
	GetHGlobalFromStream( pi_stream, &h );
	if( ::SetClipboardData( CF_TEXT, h ) == 0 )
	{
		CloseClipboard();
		return S_FALSE;
	}
	
	CloseClipboard();


	return S_OK;
}

//#define ID_CLASSMENU_BASE (ID_CMDMAN_BASE + 1200)

void CMeasView::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	//if( ::GetValueInt( ::GetAppUnknown(), get_section(), "ShowContext", 1 ) == 0 )
	//	return;

	IUnknown *punk = _GetOtherComponent( GetAppUnknown(), IID_ICommandManager );

	if( !punk )
		return;

	sptrICommandManager2 sptrCM = punk;
	punk->Release();

	if( sptrCM == NULL )
		return;
	HRESULT hr=sptrCM->GetActionID(_bstr_t(L"SetClass"),&m_iSkip);

	CPoint pt = point;
	//ScreenToClient( &pt );


	//pt = ::ConvertToClient( GetControllingUnknown(), pt );

	CMenuRegistrator rcm;	


	IUnknownPtr ptrObject;//

	_bstr_t	bstrMenuName = GetMenuName();

	if( bstrMenuName.length()==0 )
		return;

	// Context menu for selection, drawing objects, etc...

//	IUnknownPtr punkObject;

	if( m_selection.GetCount() > 0 )
	{
		//ptrObject = punkObject;

		//CString strObjectType = ::GetObjectKind( ptrObject );

		//CString strObjectMenuName = (LPCSTR)rcm.GetMenu( szImageCloneView, strObjectType );
	
		//if( !strObjectMenuName.IsEmpty() )
		//{
		//	bstrMenuName = strObjectMenuName;
		//}		

		// register context menu initializer
		sptrCM->RegisterMenuCreator(&m_xMenuInitializer);
		// execute context menu (return value is action's ID which selected by user)
		UINT uCMD=0;
		sptrCM->ExecuteContextMenu3((BSTR)bstrMenuName, pt, TPM_RETURNCMD, &uCMD);
		sptrCM->UnRegisterMenuCreator(&m_xMenuInitializer);
		{
			long nClassCount = class_count();
			if(uCMD>=ID_CLASSMENU_BASE && uCMD<ID_CLASSMENU_BASE+UINT(nClassCount)+1)
			{
				{
					int nClass = uCMD - ID_CLASSMENU_BASE - 1;
					char sz_cl[60];
					_itoa(nClass, sz_cl, 10);
					strcat(sz_cl,",\"\", 0"); // A.M. fix BT5076
					::ExecuteAction("SetClass",sz_cl,0);
				}
			}else{
				// set selected action arguments
				bool bExec = true;

				BSTR	bstrActionName;
				_bstr_t strArgs = "";
				DWORD	dwFlag = 0;

				if (!SUCCEEDED(sptrCM->GetActionName(&bstrActionName, uCMD)))
					return ;
				
				_bstr_t strActionName = bstrActionName;
				::SysFreeString(bstrActionName);

				//AFX_MANAGE_STATE(AfxGetStaticModuleState()); 
				::ExecuteAction(strActionName, strArgs, dwFlag);
			}
		}
	}
	else
	{
		sptrCM->ExecuteContextMenu2( bstrMenuName, point, 0 );
	}
}

IMPLEMENT_UNKNOWN(CMeasView, MenuInitializer)

HRESULT CMeasView::XMenuInitializer::OnInitPopup(
	BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu )
{
	_try_nested(CMeasView, MenuInitializer, OnInitPopup)
	{
		UINT posSetClass;
		*phOutMenu = ::CopyMenuSkipSubMenu(hMenu,pThis->m_iSkip, posSetClass);
		if(posSetClass>=0){
			if (!*phOutMenu) return S_FALSE;

			long nClass=-2;
			if(1==pThis->m_selection.GetCount()){
				ICalcObjectPtr sptrCO( pThis->m_selection[0]);
				if((ICalcObject*)sptrCO)
					nClass = get_object_class( sptrCO );
			}
			
			UINT uState = MF_ENABLED;

			::InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_SEPARATOR, 0, 0);

			long nClassCount = class_count();
			for(int i=0; i<nClassCount; i++)
			{
				_bstr_t bstr = get_class_name( i );
				UINT uState2 = (i==nClass) ? MF_GRAYED | MF_CHECKED : MF_ENABLED;
				::InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_STRING | uState2,
					ID_CLASSMENU_BASE+1+i, bstr);
			}
			if(pThis->m_bShowUnknown)
			{
				_bstr_t bstr = get_class_name( -1 );
				UINT uState2 = (-1==nClass) ? MF_GRAYED | MF_CHECKED : MF_ENABLED;
				::InsertMenu(*phOutMenu, -1, MF_BYPOSITION | MF_STRING | uState2,
					ID_CLASSMENU_BASE, bstr);
			}
		}
		//pThis->_ChangeSelectionMode( lSelectType );

		return S_OK;
	}
	_catch_nested;
}
