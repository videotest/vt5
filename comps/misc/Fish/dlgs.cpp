// dlgs.cpp : implementation file
//

#include "stdafx.h"
#include "Fish.h"
#include "dlgs.h"
#include "fishcontextviewer.h"
#include "PropBag.h"
#include "utils.h"
#include "MenuRegistrator.h"
#include "Settings.h"

void ExecuteContextMenu(CPoint point)
{
	IUnknownPtr punk(_GetOtherComponent(GetAppUnknown(), IID_ICommandManager), false);
	if (punk == 0)
		return;
	sptrICommandManager2 sptrCM = punk;
	if (sptrCM == 0)
		return;
	CMenuRegistrator rcm;
	_bstr_t	bstrMenuName = rcm.GetMenu(szFishContextMenu, 0);
	sptrCM->ExecuteContextMenu2(bstrMenuName, point, 0);
}

//*****************************************************************************/
IMPLEMENT_DYNAMIC(CResultPartDlg, CDialog)
CResultPartDlg::CResultPartDlg( CWnd* pParent, int id ) : CDialog( CResultPartDlg::IDD, pParent )
{
	m_pParent = pParent;
	m_Data.SetContextParent( pParent );
	m_nID = id;
	m_Data.SetID(id);
}

CResultPartDlg::~CResultPartDlg()
{
}

void CResultPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_PICTURE, m_Data );
	DDX_Control(pDX, IDC_SELECT_RECT, CChildWnd::m_wBorder);
}

BEGIN_MESSAGE_MAP(CResultPartDlg, CDialog)
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

//*****************************************************************************/

IMPLEMENT_DYNAMIC(CLayerPartDlg, CDialog)

CLayerPartDlg::CLayerPartDlg( CWnd *pParent ) : CDialog( CLayerPartDlg::IDD, pParent )
{
	m_pParent = pParent;
	m_bSelfChange = false;
	m_bCheched = true;
	m_Data.SetContextParent( m_pParent );
}

CLayerPartDlg::~CLayerPartDlg()
{
}

void CLayerPartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control( pDX, IDC_PICTURE, m_Data );
	DDX_Control(pDX, IDC_SELECT_RECT, CChildWnd::m_wBorder);
}

BEGIN_MESSAGE_MAP(CLayerPartDlg, CDialog)
	ON_BN_CLICKED(IDC_CHECK1, OnBnClicked)
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_LBUTTONDOWN()
	ON_WM_CONTEXTMENU()
	ON_WM_CTLCOLOR()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

void CLayerPartDlg::Refresh( int nID, IUnknown *punkNData )
{
	m_nID = nID;
	m_Data.SetID( m_nID );

	CString str;
	str.Format( "%d", nID );

	COLORREF clColor = ::GetValueColor( punkNData, FISH_COLORS, CString( FISH_COLOR_PANE ) + str, 0 );

	CWnd *pStaticWnd = GetDlgItem( IDC_NAME );

	CString sName = GetValueString( punkNData, FISH_ROOT, FISH_NAME_PANES+str, FISH_NAME_PANES+str);
	CString sSName = GetValueString( punkNData, FISH_ROOT, FISH_PANE_SNAME+str, 0);
	if (sSName.IsEmpty())
		pStaticWnd->SetWindowText(sName);
	else
	{
		str.Format(_T("%s(%s)"), (LPCTSTR)sName, sSName);
		pStaticWnd->SetWindowText(str);
	}
	SetColor(clColor);
}

void CLayerPartDlg::SetColor(COLORREF clColor)
{
	CWnd *pWnd = GetDlgItem( IDC_VTCOLORPICKERCTRL );
	IUnknown *punkControl = pWnd->GetControlUnknown();
	if( punkControl )
	{
		IDispatch *disp = 0;
		punkControl->QueryInterface( __uuidof( IDispatch ), (LPVOID*)&disp );

		if( disp )
		{
			CString str;
			str.Format( "(%d,%d,%d)", GetRValue( clColor ), GetGValue( clColor ), GetBValue( clColor ) );

			_bstr_t bstr = str;

			_variant_t varArg( bstr ), varRes;
			m_bSelfChange = true;
			HRESULT hr = _invoke_script_func( disp, _bstr_t( _SET_COLOR ), &varArg, 1, &varRes );

			if( hr != S_OK )
				AfxMessageBox( IDS_ERROR );
			disp->Release();
			m_bSelfChange = false;
		}		
	}
}

BEGIN_EVENTSINK_MAP(CLayerPartDlg, CDialog)
	ON_EVENT( CLayerPartDlg, IDC_VTCOLORPICKERCTRL, 1, OnChange, VTS_NONE )
END_EVENTSINK_MAP()

void CLayerPartDlg::OnChange()
{
	if( m_bSelfChange )
		return;

	CWnd *pWnd = GetDlgItem( IDC_VTCOLORPICKERCTRL );

	IUnknown *punkControl = pWnd->GetControlUnknown();
	if( punkControl )
	{
		IDispatch *disp = 0;
		punkControl->QueryInterface( __uuidof( IDispatch ), (LPVOID*)&disp );

		if( disp )
		{
			_variant_t varRes;
			HRESULT hr = _invoke_script_func( disp, _bstr_t( _GET_COLOR ), 0, 0, &varRes );

			if( hr != S_OK )
				AfxMessageBox( IDS_ERROR );

			disp->Release();

			COLORREF clColor = _str_to_color( varRes.bstrVal );
			varRes.Clear();
			ChangeColor(clColor);

			((CFishContextViewer*)m_pParent)->FireOnChangeColor(m_nID, clColor);
		}		
	}

}

void CLayerPartDlg::ChangeColor(COLORREF clColor)
{
	CString str;
	str.Format( "%d", m_nID );
	if( m_pParent )
	{
		CFishContextViewer *pPar = static_cast<CFishContextViewer*>( m_pParent );

		bool bFromShellData = true;
		IUnknown *punkObject = GetActualImageFromView( pPar->m_sptrView );
		if( punkObject )
		{
			int	r = GetRValue( clColor );
			int	g = GetGValue( clColor );
			int	b = GetBValue( clColor );

			char	sz[30];
			sprintf( sz, "(%d, %d, %d)", r, g, b );

 			pPar->StoreToNamedData( punkObject, _bstr_t( FISH_COLORS ), _bstr_t( CString( FISH_COLOR_PANE ) + str ), _bstr_t( sz ) );

			punkObject->Release();
			punkObject = 0;
			bFromShellData = false;
		}

		IViewPtr sptrV = pPar->m_sptrView;		
		if( sptrV != 0 )
		{
			IUnknown *punkD = 0;
			sptrV->GetDocument( &punkD );

			if( punkD )
			{
				IFileDataObject2Ptr ptrFDO2 = punkD;
				punkD->Release();

				if( ptrFDO2 != 0 )
					ptrFDO2->SetModifiedFlag( TRUE );
			}
		}

		if(bFromShellData || ::GetValueInt( ::GetAppUnknown(), FISH_ROOT, FISH_SHELLDATA_COPY, 1 ) != 0 )
			::SetValueColor( ::GetAppUnknown(), FISH_COLORS, CString( FISH_COLOR_PANE ) + str, clColor );
		if (bFromShellData)
			pPar->UpdateCC();
		else
			pPar->UpdateCC_ID( m_nID, clColor );
	}
}

long CLayerPartDlg::_str_to_color( CString strRGBColor )
{
	OLE_COLOR color = (OLE_COLOR)RGB(255,255,255);
	CString str = strRGBColor;
	CString strR = "", strG = "", strB = "";

	if( str.Find("(") != -1 && str.Find(")") != -1 )
	{
		int brCount = 0;
		for( int i = 1; i < str.GetLength() - 1 ;i++)
		{
			char ch = str.GetAt( i );
			if( ch != ',' )
			{
				switch( brCount )
				{
				case 0:	strR += ch; 
						break;
				case 1:	strG += ch; 
						break;
				case 2:	strB += ch; 
						break;
				}
			}
			else
				brCount++;
		}
		
		if( !strR.IsEmpty() && !strG.IsEmpty() && !strB.IsEmpty() )
			color = (OLE_COLOR)RGB( atoi(strR), atoi(strG), atoi(strB) );
	}
	return color;
}

void CLayerPartDlg::OnBnClicked()
{
	ShowPane(!m_bCheched);
	((CFishContextViewer*)m_pParent)->FireOnChangeCheck(m_nID,m_bCheched);
}

void CLayerPartDlg::ShowPane( bool bStat, bool bNoRefresh/* = false */)
{
	m_bCheched = bStat;
	CButton *pBtn = (CButton *)GetDlgItem( IDC_CHECK1 );
	pBtn->SetCheck( bStat );
	if (m_pParent)
	{
		CFishContextViewer *pPar = static_cast<CFishContextViewer*>( m_pParent );
		INamedPropBagPtr ptrBag = pPar->m_sptrView;
		_variant_t old_var;
		if (ptrBag != 0)
			ptrBag->GetProperty(_bstr_t(FISH_USED_PANES), &old_var);
		else
			old_var = _variant_t((long)pPar->UsedPanes());
		bool bCh = false;
		if (pPar->UsedPanes() == old_var.lVal)
			bCh = true;
		if( bStat )
			pPar->UsedPanes() |= 1 << m_nID;
		else
			pPar->UsedPanes() &= ~(1 << m_nID);
		if (bCh && !bNoRefresh)
		{
			if (ptrBag != 0)
			{
				_variant_t	var = pPar->UsedPanes();
				if (::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_BAR_SETS_PROPS, 1) != 0)
					ptrBag->SetProperty(_bstr_t(FISH_USED_PANES), var);
			}
			if (ptrBag == 0 || ::GetValueInt(::GetAppUnknown(), FISH_ROOT, FISH_SHELLDATA_COPY, 1) != 0)
				::SetValue(::GetAppUnknown(), FISH_ROOT, FISH_USED_PANES, (long)pPar->UsedPanes());
		}
		IViewPtr sptrV = pPar->m_sptrView;		
		if( sptrV != 0 )
		{
			IUnknown *punkD = 0;
			sptrV->GetDocument( &punkD );

			if( punkD )
			{
				IFileDataObject2Ptr ptrFDO2 = punkD;
				punkD->Release();

				bool bSetModified = 
					::GetValueInt(::GetAppUnknown(), FISH_ROOT, "SetModifiedOnPaneSwitch", 0) != 0;
				if( bSetModified && (ptrFDO2 != 0) )
					ptrFDO2->SetModifiedFlag( TRUE );
			}
		}
		IUnknown *punkObject = GetActualImageFromView( pPar->m_sptrView );
		if( punkObject )
		{
			pPar->StoreToNamedData( punkObject, _bstr_t( FISH_ROOT ), _bstr_t( FISH_USED_PANES ), _variant_t( (long)pPar->UsedPanes() ) );
			punkObject->Release();
			punkObject = 0;
		}
		CWnd *pWnd = ::GetWindowFromUnknown( pPar->m_sptrView );
		if( pWnd )
			pWnd->Invalidate();
		if( !bNoRefresh )
			pPar->_RefreshWindow();
		FireEvent(GetAppUnknown(), szEventNamedPropsChanged, NULL, ptrBag, NULL, 0);
	}
}
//*****************************************************************************/

BEGIN_MESSAGE_MAP(CLayerDataItem, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CLayerDataItem::OnPaint()
{
	CPaintDC dc( this );

	CWnd *pParent = GetParent();
	BOOL bEnabled = pParent->IsWindowEnabled();

	BYTE *pbi = 0;
	short nSupported;
	CRect rect;

	GetClientRect( &rect );
	COLORREF clrBack = RGB(0,0,0);
	// [vanek] : no named data - no icon - 30.09.2004
	if( m_pParent != 0 && m_sptrNData != 0 )
	{
		IUnknown *punkRO = 0;
		m_sptrThumbMan->GetRenderObject( m_sptrNData , &punkRO );
		IRenderObjectPtr spRenderObject = punkRO;
		punkRO->Release();
		punkRO = 0;

		if( spRenderObject == 0 ) 
			return;


		CFishContextViewer *pPar = static_cast<CFishContextViewer*>( m_pParent );

		INamedPropBagPtr ptrBag = pPar->m_sptrView;

		if( ptrBag == 0 )
		{
			IApplicationPtr sptrApp(GetAppUnknown());
			IUnknownPtr punkActiveDoc;
			sptrApp->GetActiveDocument(&punkActiveDoc);
			IDocumentSitePtr sptrDSActiveDoc(punkActiveDoc);
			IUnknownPtr punkActiveView;
			if (sptrDSActiveDoc != 0) sptrDSActiveDoc->GetActiveView(&punkActiveView);
			ptrBag = punkActiveView;
		}

		if( ptrBag != 0 )
		{
			_variant_t varInvert;
			CString sInvert;
			sInvert.Format(_T("FishInvert%d"), m_nID);
			ptrBag->GetProperty(_bstr_t(sInvert), &varInvert);
			if (varInvert.vt != VT_EMPTY && varInvert.lVal != 0)
				clrBack = RGB(255,255,255);
			_variant_t var = long(m_nID), var_old;
			ptrBag->GetProperty( _bstr_t(FISH_SHOWN_PANE), &var_old );
			ptrBag->SetProperty( _bstr_t(FISH_SHOWN_PANE), var );
			if (bEnabled == FALSE)
				ptrBag->SetProperty( _bstr_t(FISH_ENABLED), _variant_t(0L));
			spRenderObject->GenerateThumbnail( m_sptrNData, 24, RF_BACKGROUND, 0,
											CSize( rect.Width(), rect.Height() ), 
											&nSupported, &pbi, ptrBag);
			if (bEnabled == FALSE)
				ptrBag->SetProperty( _bstr_t(FISH_ENABLED), _variant_t(1L));
			ptrBag->SetProperty( _bstr_t(FISH_SHOWN_PANE), var_old );
		}
	}
	dc.FillRect( &rect, &CBrush( clrBack ) );


	if (pbi)
	{
		if( (IUnknown*)(m_sptrNData)!=GetAppUnknown() )
		{
			::ThumbnailDraw( pbi, (CDC*)&dc, rect, m_sptrNData );
		}
		::ThumbnailFree( pbi );
		pbi = 0;
	}
}

void CLayerPartDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	_mouse_move( m_wBorder.m_hWnd );
	__super::OnMouseMove(nFlags, point);
}

void CLayerPartDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == TIMER_ID )
	{
		CRect rc;
		GetWindowRect( &rc );
		POINT pt;
		::GetCursorPos( &pt );

		if( !rc.PtInRect( pt ) )
		{
			KillTimer( TIMER_ID );
			_timer( m_wBorder.m_hWnd );
		}
	}

	__super::OnTimer(nIDEvent);
}

void CLayerPartDlg::Select()
{
	_lbutton_down( m_wBorder.m_hWnd );
	SetBackgroundColor(g_ActiveColor);
	if( m_pParent )
	{
		CFishContextViewer *pPar = static_cast<CFishContextViewer*>( m_pParent );
		pPar->UnpressBtns( this );
	}
}

void CResultPartDlg::Select()
{
	_lbutton_down( m_wBorder.m_hWnd );
	if( m_pParent )
	{
		CFishContextViewer *pPar = static_cast<CFishContextViewer*>( m_pParent );
		pPar->UnpressBtns( this );
	}
}

void CLayerPartDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_pParent )
	{
		CFishContextViewer *pPar = static_cast<CFishContextViewer*>( m_pParent );
		pPar->_SetActiveItem(m_nID,true,true);
	}
	__super::OnLButtonDown(nFlags, point);
}

void CResultPartDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	if( m_pParent )
	{
		CFishContextViewer *pPar = static_cast<CFishContextViewer*>( m_pParent );
		pPar->_SetActiveItem(m_nID,true,true);
	}
	__super::OnLButtonDown(nFlags, point);
}

void CLayerPartDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( m_pParent )
		((CFishContextViewer *)m_pParent)->_SetActiveItem(m_nID,true,true);
	__super::OnRButtonDown(nFlags, point);
}

void CResultPartDlg::OnRButtonDown(UINT nFlags, CPoint point)
{
	if( m_pParent )
		((CFishContextViewer *)m_pParent)->_SetActiveItem(m_nID,true,true);
	__super::OnRButtonDown(nFlags, point);
}

void CResultPartDlg::OnTimer(UINT_PTR nIDEvent)
{
	if( nIDEvent == TIMER_ID )
	{
		CRect rc;
		GetWindowRect( &rc );
		POINT pt;
		::GetCursorPos( &pt );

		if( !rc.PtInRect( pt ) )
		{
			KillTimer( TIMER_ID );
			_timer( m_wBorder.m_hWnd );
		}
	}

	__super::OnTimer(nIDEvent);
}

void CResultPartDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	_mouse_move( m_wBorder.m_hWnd );
	__super::OnMouseMove(nFlags, point);
}

void CLayerPartDlg::RepositionWindow(int id, CRect rcBase, int nShift)
{
	CWnd *pWnd = GetDlgItem(id);
	if (pWnd)
	{
		CRect rc;
		pWnd->GetWindowRect(rc);
		GetParent()->ScreenToClient(&rc);
		rc.top += nShift;
		rc.bottom += nShift;
		pWnd->MoveWindow(rc);
	}
}

void CLayerPartDlg::SetWindowSize(int nWidth)
{
	CRect rcData,rcBorder,rcTotal;
	m_Data.GetWindowRect( &rcData );
	GetParent()->ScreenToClient( &rcData );
	rcData.right += nWidth; 
	rcData.bottom += nWidth; 
	m_Data.MoveWindow( &rcData );
	GetWindowRect( &rcTotal );
	GetParent()->ScreenToClient( &rcTotal );
	rcTotal.right += nWidth; 
	rcTotal.bottom += nWidth; 
	MoveWindow( &rcTotal );
	m_wBorder.GetWindowRect( &rcBorder );
	GetParent()->ScreenToClient( &rcBorder );
	rcBorder.right += nWidth; 
	rcBorder.bottom += nWidth; 
	m_wBorder.MoveWindow( &rcBorder );
	RepositionWindow(IDC_CHECK1, rcData, nWidth);
	RepositionWindow(IDC_NAME, rcData, nWidth);
	RepositionWindow(IDC_VTCOLORPICKERCTRL, rcData, nWidth);
}

void CResultPartDlg::SetWindowSize(int nWidth)
{
	CRect rcData,rcBorder,rcTotal;
	m_Data.GetWindowRect( &rcData );
	GetParent()->ScreenToClient( &rcData );
	rcData.right += nWidth; 
	rcData.bottom += nWidth; 
	m_Data.MoveWindow( &rcData );
	GetWindowRect( &rcTotal );
	GetParent()->ScreenToClient( &rcTotal );
	rcTotal.right += nWidth; 
	rcTotal.bottom += nWidth; 
	MoveWindow( &rcTotal );
	m_wBorder.GetWindowRect( &rcBorder );
	GetParent()->ScreenToClient( &rcBorder );
	rcBorder.right += nWidth; 
	rcBorder.bottom += nWidth; 
	m_wBorder.MoveWindow( &rcBorder );
}

void CLayerPartDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	ExecuteContextMenu(point);
}

void CResultPartDlg::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
	ExecuteContextMenu(point);
}

HBRUSH CLayerPartDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = __super::OnCtlColor(pDC, pWnd, nCtlColor);
	if (nCtlColor == CTLCOLOR_STATIC)
	{
		pDC->SetTextColor(g_PaneText);
		pDC->SetBkMode(TRANSPARENT);
		hbr = m_brBack;
	}
	return hbr;
}

void CLayerPartDlg::SetBackgroundColor(COLORREF clr)
{
	m_brBack.DeleteObject();
	m_brBack.CreateSolidBrush(clr);
	CWnd *pWnd = GetDlgItem(IDC_NAME);
	if (pWnd) pWnd->Invalidate();
	pWnd = GetDlgItem(IDC_CHECK1);
	if (pWnd) pWnd->Invalidate();
}

