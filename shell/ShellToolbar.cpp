#include "stdafx.h"
#include "shellbutton.h"
#include "shelltoolbar.h"
#include "resource.h"
#include "CommandManager.h"
#include "globals.h"
#include "menuhash.h"
#include "ShellFrame.h"
#include "Shell.h"

__declspec( dllimport )CMenuHash	g_menuHash;

__declspec(dllimport) GLOBAL_DATA *pGlobalData;



IMPLEMENT_SERIAL(CShellToolBar, CBCGToolBar, 1);
IMPLEMENT_SERIAL(CShellFrameToolBar, CBCGToolBar, VERSIONABLE_SCHEMA | 1)


BEGIN_MESSAGE_MAP( CShellToolBar, CBCGToolBar )
	//{{AFX_MSG_MAP(CShellToolBar)
	ON_COMMAND(ID_TOOLBAR_MENUBUTTON, OnToolbarMenubutton)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_MENUBUTTON, OnUpdateToolbarMenubutton)
	ON_COMMAND(ID_TOOLBAR_COMMANDLIST, OnToolbarCommandlist)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_COMMANDLIST, OnUpdateToolbarCommandlist)
	ON_COMMAND(ID_SHOW_AS_TEXT, OnShowAsText)
	ON_COMMAND(ID_SHOW_AS_IMAGE, OnShowAsImage)
	ON_COMMAND(ID_SHOW_AS_IMAGE_AND_TEXT, OnShowAsImageAndText)
	ON_COMMAND(ID_ACTION_INFO, OnActionInfo)
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

long g_lAllowCustomize = -1;

CShellToolBar::CShellToolBar() : CBCGToolBar()
{
	CString	strCustomize = ::LanguageLoadCString( IDS_CUSTOMIZE );
	//strCustomize.LoadString( IDS_CUSTOMIZE );
	int	nCustomizeCommand = g_CmdManager.GetActionCommand( "ToolsCustomize" );

	m_bEnableDrag = ::GetValueInt( GetAppUnknown(), "\\General", "EnableDock", 1L ) != 0;

	if( g_lAllowCustomize == -1 )
		g_lAllowCustomize = ::GetValueInt( GetAppUnknown(), "\\MainFrame", "EnableCustomizeButton", 1L );
	
	if( g_lAllowCustomize == 1L )
		EnableCustomizeButton (TRUE, nCustomizeCommand, strCustomize );
}

CShellToolBar::~CShellToolBar()
{
}

LRESULT CShellToolBar::OnHelpHitTest( WPARAM wParam, LPARAM lParam )
{
	int nIndex = HitTest ((DWORD) lParam);
	if (nIndex < 0)	return -1;

	CBCGToolbarButton* pbutton = GetButton( nIndex );
	if( !pbutton )return -1;

	// [vanek] : кнопка с m_nID, равным 0, является меню - 08.11.2004
	if( pbutton->m_nID == -1 || pbutton->m_nID == 0)
		if( pbutton->OnContextHelp( this ) )
			return -2;

	return pbutton->m_nID;
}

LRESULT CShellToolBar::OnGetInterface( WPARAM wParam, LPARAM lParam )
{
	int nIndex = HitTest ((DWORD) lParam);
	if (nIndex < 0)	
		return 0;

	CBCGToolbarButton* pbutton = GetButton( nIndex );
	if( !pbutton )
		return 0;

	if( !pbutton->IsKindOf(RUNTIME_CLASS(CShellToolbarButton)) )
		return 0;

	return (LRESULT)((CShellToolbarButton*)pbutton)->GetInterface( (long)wParam );
}

CBCGToolbarButton* CShellToolBar::CreateDroppedButton (COleDataObject* pDataObject)
{
	CBCGToolbarButton* pButton = CBCGToolbarButton::CreateFromOleData (pDataObject);
	ASSERT (pButton != NULL);

	int iOffset = pButton->m_strText.Find (_T('\t'));
	if (iOffset >= 0)
	{
		pButton->m_strText = pButton->m_strText.Left (iOffset);
	}

	if( pButton->m_bDragFromCollection )
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
	}

	/*if (pButton->m_bDragFromCollection && 
		pButton->GetImage () == -1 &&
		pButton->m_strText.IsEmpty ())
	{
		CBCGLocalResource locaRes;
		CButtonAppearanceDlg dlg (pButton, m_pUserImages, this, 0, m_bMenuMode);

		if (dlg.DoModal () != IDOK)
		{
			delete pButton;
			return NULL;
		}
	}*/

	if (pButton->GetImage () < 0)
	{
		pButton->m_bText = TRUE;
		pButton->m_bImage = FALSE;
	}

	//CBCGToolbarButton* pButton  = CBCGToolBar::CreateDroppedButton( pDataObject );

	if( pButton->IsKindOf( RUNTIME_CLASS( CBCGToolbarMenuButton ) )
		&&(pButton->m_nID == 0)||(pButton->m_nID == -1) )
	{
		if( pButton->IsKindOf( RUNTIME_CLASS( CShellToolbarButton ) ) )
			return pButton;

		int	nImageIndex = pButton->IsLocked () ? -1 : pButton->GetImage ();
		CBCGToolbarMenuButton *pMenuButton = (CBCGToolbarMenuButton *)pButton;
		CShellToolbarButton *pShellMenuButton = new CShellToolbarButton( pButton->m_nID, 
			0,
			nImageIndex, 
			pButton->m_strText, 
			pButton->m_bUserButton );

		pShellMenuButton->CopyFrom( *pMenuButton );

		pShellMenuButton->m_bText = pMenuButton->m_bText;
		pShellMenuButton->m_bImage = pMenuButton->m_bImage;
		//pShellMenuButton->m_bDrawDownArrow = pMenuButton->m_bDrawDownArrow;
		//pShellMenuButton->m_bMenuMode = pMenuButton->m_bMenuMode;
		delete pMenuButton;
		return pShellMenuButton;

	}

	ASSERT (pButton != NULL);

	//ComboBox Button support
	//____Start

	CShellMenuComboBoxButton* pComboButton = 
		DYNAMIC_DOWNCAST (CShellMenuComboBoxButton, pButton);
	if( pComboButton )
	{
		int	nImageIndex = pButton->IsLocked () ? -1 : pButton->GetImage ();

		CBCGToolbarMenuButton *pMenuButton = (CBCGToolbarMenuButton *)pButton;

		pComboButton = new CShellMenuComboBoxButton( 
				pButton->m_nID,  
				nImageIndex,
				0
				);

		pComboButton->CopyFrom( *pMenuButton );
		pComboButton->m_bText = pMenuButton->m_bText;
		pComboButton->m_bImage = pMenuButton->m_bImage;
		//pShellMenuButton->m_bDrawDownArrow = pMenuButton->m_bDrawDownArrow;

		delete pButton;

		return pComboButton;

	}

	//____End
	//ComboBox Button support
	


	CShellToolbarButton* pShellButton = 
		DYNAMIC_DOWNCAST (CShellToolbarButton, pButton);

	if (pShellButton == NULL)
	{
		int	nImageIndex = pButton->IsLocked () ? -1 : pButton->GetImage ();
		pShellButton = new CShellToolbarButton( pButton->m_nID, 
			nImageIndex, 
			pButton->m_strText, 
			pButton->m_bUserButton 
			/*pButton->IsLocked ()*/ );

			pShellButton->CBCGToolbarButton::CopyFrom( *pButton );
		/*if( nImageIndex == -1 )
		{
			pShellButton->m_bText = TRUE;
			pShellButton->m_bImage = FALSE;
		}
		else
		{
			pShellButton->m_bText = FALSE;
			pShellButton->m_bImage = TRUE;
		}*/
		
		delete pButton;
		
	}

	return pShellButton;
}

void CShellToolBar::DoPaint(CDC* pDC)
{
	BOOL bHorz = m_dwStyle & CBRS_ORIENT_HORZ ? TRUE : FALSE;

	//m_dwStyle |= CBRS_ORIENT_HORZ;

	CBCGToolBar::DoPaint( pDC );

	if( !bHorz )
		m_dwStyle &= ~CBRS_ORIENT_HORZ;
}

BOOL CShellToolBar::EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup)
{
	if( !CBCGToolBar::EnableContextMenuItems( pButton, pPopup ) )
		return false;

	if( !pButton->IsKindOf( RUNTIME_CLASS( CShellToolbarButton ) ) )
		return true;

	CShellToolbarButton	*pshellButton = (CShellToolbarButton	*)pButton;
	//pPopup->RemoveMenu( ID_BCGBARRES_TOOLBAR_APPEARANCE, MF_BYCOMMAND );

	bool	bChooseButton = pshellButton->IsKindOf( RUNTIME_CLASS( CShellChooseToolbarButton ) )!=0;
	bool	bHasMenu = (pshellButton->m_nID == -1)||
						(pshellButton->m_nID == 0)||bChooseButton;
	
	CMenu	menu;
	if( bHasMenu )
		menu.LoadMenu( IDR_TOOLBAR_APPEND_POPUP );
	else
		menu.LoadMenu( IDR_TOOLBAR_APPEND );
	
	CMenu	*pPopupSource = menu.GetSubMenu( 0 );
	pPopup->AppendMenu( MF_SEPARATOR );

	for( int n = 0; n < (int)pPopupSource->GetMenuItemCount(); n++ )
	{
		CString	strMenu;
		UINT nCmdID = pPopupSource->GetMenuItemID( n );
		pPopupSource->GetMenuString( n, strMenu, MF_BYPOSITION );

		if( strMenu.IsEmpty() )
			pPopup->AppendMenu( MF_SEPARATOR );
		else
			pPopup->AppendMenu( MF_STRING, nCmdID, strMenu );
	}
	
	pPopup->CheckMenuItem( bChooseButton?ID_TOOLBAR_COMMANDLIST:ID_TOOLBAR_MENUBUTTON, true );

	return true;
}

void CShellToolBar::OnToolbarMenubutton() 
{
	CShellToolbarButton *pbtn = GetSelectedButton();
	if( !pbtn )
		return;
	if( !pbtn->IsKindOf( RUNTIME_CLASS(CShellChooseToolbarButton)))
		return;
	CShellToolbarButton *pbtnNew = new CShellToolbarButton;
	pbtnNew->CopyFrom( *pbtn );
	pbtnNew->m_nID = -1;

	SetSelectedButton( pbtnNew );
}

void CShellToolBar::OnUpdateToolbarMenubutton(CCmdUI* pCmdUI) 
{
	CShellToolbarButton *pbtn = GetSelectedButton();
	pCmdUI->Enable( pbtn != 0 );
	if( !pbtn )	return;
	pCmdUI->SetCheck( !pbtn->IsKindOf( RUNTIME_CLASS(CShellChooseToolbarButton)) );
}

void CShellToolBar::OnToolbarCommandlist() 
{
	CShellToolbarButton *pbtn = GetSelectedButton();
	if( !pbtn )
		return;
	if( pbtn->IsKindOf( RUNTIME_CLASS(CShellChooseToolbarButton)))
		return;
	CShellToolbarButton *pbtnNew = new CShellChooseToolbarButton;
	pbtnNew->CopyFrom( *pbtn );

	SetSelectedButton( pbtnNew );
}

void CShellToolBar::OnUpdateToolbarCommandlist(CCmdUI* pCmdUI) 
{
	CShellToolbarButton *pbtn = GetSelectedButton();
	pCmdUI->Enable( pbtn != 0 );
	if( !pbtn )	return;
	pCmdUI->SetCheck( pbtn->IsKindOf( RUNTIME_CLASS(CShellChooseToolbarButton)) );
}

CShellToolbarButton *CShellToolBar::GetSelectedButton()
{
	if (m_iSelected >= m_Buttons.GetCount ())
		m_iSelected = -1;

	if( m_iSelected == -1 )
		return 0;
	CBCGToolbarButton	*pbtn = GetButton( m_iSelected );
	if( !pbtn )return 0;

	if( !pbtn->IsKindOf( RUNTIME_CLASS(CShellToolbarButton)))
		return 0;
	return (CShellToolbarButton*)pbtn;
}

void CShellToolBar::SetSelectedButton( CShellToolbarButton *pbtn )
{
	if (m_iSelected >= m_Buttons.GetCount ())
		m_iSelected = -1;

	if( m_iSelected == -1 )
		return;

	int	nPos = m_iSelected;

	RemoveButton( nPos );
	InsertButton( *pbtn, nPos );
	AdjustLayout();
}

BOOL CShellToolBar::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGToolBar::PreCreateWindow( cs ) )
		return false;

	if( CBCGToolbarButton::s_bInitProgMode )
		cs.style &= ~WS_VISIBLE;

	cs.style |= TBSTYLE_TRANSPARENT;

	return true;
}

void CShellToolBar::OnBarStyleChange(DWORD dwOldStyle, DWORD dwStyle)
{
//	m_dwStyle &= ~(CBRS_BORDER_ANY|CBRS_GRIPPER);
//	m_dwStyle |= TBSTYLE_TRANSPARENT;
}

void CShellToolBar::OnShowAsText() 
{
	for( int n = 0; n < GetCount (); n++ )
	{
		CBCGToolbarButton	*pbtn = GetButton( n );
		pbtn->m_bText = true;
		pbtn->m_bImage = false;
	}
	AdjustLayout();
}

void CShellToolBar::OnShowAsImage() 
{
	for( int n = 0; n < GetCount (); n++ )
	{
		CBCGToolbarButton	*pbtn = GetButton( n );
		pbtn->m_bText = false;
		pbtn->m_bImage = true;
	}
	AdjustLayout();
}

void CShellToolBar::OnShowAsImageAndText() 
{
	for( int n = 0; n < GetCount (); n++ )
	{
		CBCGToolbarButton	*pbtn = GetButton( n );
		pbtn->m_bText = true;
		pbtn->m_bImage = true;
	}
	AdjustLayout();
}

void CShellToolBar::AdjustLocations()
{
	CBCGToolBar::AdjustLocations();

	DWORD	dwbarStyle = GetBarStyle();
	if(  dwbarStyle&CBRS_ALIGN_LEFT ||dwbarStyle&CBRS_ALIGN_RIGHT )
	{
		CClientDC dc (this);
		dc.SelectObject (&pGlobalData->fontRegular);

		POSITION pos = m_Buttons.GetHeadPosition();
		CSize	sizeMax = CSize( 0, 0 );

		while( pos )
		{
			CBCGToolbarButton	*pbtn = (CBCGToolbarButton*)m_Buttons.GetNext( pos );
			CSize	sizeButton = pbtn->OnCalculateSize (&dc, CSize ( GetColumnWidth(), GetRowHeight()), true );

			sizeMax.cx = max( sizeButton.cx, sizeMax.cx );
			sizeMax.cy = max( sizeButton.cy, sizeMax.cy );
		}

		int	yOfs = 0;

		CRect	rect;
		GetClientRect( &rect );
		CSize	size( rect.Width(), /*GetRowHeight()+5*/sizeMax.cy );

		pos = m_Buttons.GetHeadPosition();
		while( pos )
		{
			CBCGToolbarButton	*pbtn = (CBCGToolbarButton*)m_Buttons.GetNext( pos );
			
			CRect	rectButton = NORECT;
			rectButton = CRect( 0, yOfs, size.cx, yOfs+size.cy );
			pbtn->SetRect( rectButton );
			yOfs+=size.cy;
		}
	}
}

int CShellToolBar::InsertButtonStoreImage(const CBCGToolbarButton& button, int iInsertAt)
{
	int nReturnVal = InsertButton( button, iInsertAt );
	int nImage = button.GetImage ();
	UINT nID = button.m_nID;

	if( nImage != -1 )
		m_DefaultImages[nID] = nImage;

	return nReturnVal;
}

int CShellToolBar::GetMyCommandButtons( UINT uiCmd, CObList *plistButtons )
{
    if( plistButtons )
		plistButtons->RemoveAll( );

	if( uiCmd == 0 )
		return 0;
	

	int nbtn_count = 0;
    for (POSITION pos = m_Buttons.GetHeadPosition (); pos != NULL;)
	{
		CBCGToolbarButton* pButton = (CBCGToolbarButton*)(m_Buttons.GetNext (pos));
		ASSERT (pButton != NULL);
        if (pButton->m_nID == uiCmd)
		{
            nbtn_count ++;
			if( plistButtons )
				plistButtons->AddTail( pButton );
		}
	}

	return plistButtons ? (int)plistButtons->GetCount() : nbtn_count;
}

void CShellToolBar::OnActionInfo() 
{
	if( m_iSelected == -1 )
		return;

	CBCGToolbarButton	*pbtn = GetButton( m_iSelected );

	UINT	nID = pbtn->m_nID;
	CActionInfoWrp	*pai = g_CmdManager.GetActionInfo( nID-ID_CMDMAN_BASE );
	if( !pai )
		return;

	AfxMessageBox( pai->GetActionShortName() );
	
}

BOOL CShellToolBar::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!CanDragDropButtons())
		return FALSE;
	/*if (m_hWnd)
	{
		CString sName;
		GetWindowText(sName);
		if (!GetValueInt(GetAppUnknown(), "\\MainFrame\\EnableChangeToolbar", sName, TRUE))
			return FALSE;
	}*/
	return CBCGToolBar::OnDrop(pDataObject,dropEffect,point);
}

DROPEFFECT CShellToolBar::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	if (!CanDragDropButtons())
		return DROPEFFECT_NONE;
	/*if (m_hWnd)
	{
		CString sName;
		GetWindowText(sName);
		if (!GetValueInt(GetAppUnknown(), "\\MainFrame\\EnableChangeToolbar", sName, TRUE))
			return DROPEFFECT_NONE;
	}*/
	return CBCGToolBar::OnDragOver(pDataObject, dwKeyState, point);
}


bool CShellToolBar::CanDragDropButtons()
{
	if (m_hWnd)
	{
		CString sName;
		GetWindowText(sName);
		if (!sName.IsEmpty())
		{
			if (!GetValueInt(GetAppUnknown(), "\\MainFrame\\EnableChangeToolbar", sName, TRUE))
				return false;
		}
	}
	return ( g_lAllowCustomize == 1 );
}

void CShellToolBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// проапдейтим настройку возможности драга
	m_bEnableDrag = ::GetValueInt( GetAppUnknown(), "\\General", "EnableDock", 1L ) != 0;

	__super::OnLButtonDown(nFlags, point);
}

/////////////////////////////////////////////////////////////////////////////////////////
//
IMPLEMENT_DYNCREATE(CShellOutlookToolBar, CShellToolBar);

BEGIN_MESSAGE_MAP(CShellOutlookToolBar, CShellToolBar)
	//{{AFX_MSG_MAP(CShellOutlookToolBar)
	ON_WM_NCCALCSIZE()
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_COMMAND(ID_TOOLBAR_MENUBUTTON, OnToolbarMenubutton)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_MENUBUTTON, OnUpdateToolbarMenubutton)
	ON_COMMAND(ID_TOOLBAR_COMMANDLIST, OnToolbarCommandlist)
	ON_UPDATE_COMMAND_UI(ID_TOOLBAR_COMMANDLIST, OnUpdateToolbarCommandlist)
	ON_COMMAND(ID_SHOW_AS_TEXT, OnShowAsText)
	ON_COMMAND(ID_SHOW_AS_IMAGE, OnShowAsImage)
	ON_COMMAND(ID_SHOW_AS_IMAGE_AND_TEXT, OnShowAsImageAndText)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CShellOutlookToolBar::CShellOutlookToolBar()
{
	SetMaskMode( true );
	CString	str;
	EnableCustomizeButton( false, -1, str );
	EnableTextLabels();
	m_bDrawTextLabels = true;

	m_nVisibleCount = 0;
	m_nVisibleOffset = 0;
	m_rectDown =NORECT;
	m_rectUp =NORECT;
	m_nTime  = 0;
}

void CShellOutlookToolBar::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	//CShellToolBar::OnNcCalcSize(bCalcValidRects, lpncsp);
	return;
}


void CShellOutlookToolBar::AdjustLocations()
{
	CBCGToolBar::AdjustLocations();

	m_nMaxBtnHeight = CalcMaxButtonHeight ();

	CClientDC dc (this);
	dc.SelectObject (&pGlobalData->fontRegular);

	POSITION pos = m_Buttons.GetHeadPosition();
	CSize	sizeMax = CSize( 0, 0 );

	CRect	rectClient;
	GetClientRect( &rectClient );

	m_rectUp = m_rectDown = rectClient;
	m_rectUp.bottom = m_rectUp.top + 5;
	m_rectDown.top = m_rectDown.bottom - 5;

	m_nVisibleCount = 0;
	int	nPos = 0;

	while( pos )
	{
		CBCGToolbarButton	*pbtn = (CBCGToolbarButton*)m_Buttons.GetNext( pos );
		CSize	sizeButton = pbtn->OnCalculateSize (&dc, CSize ( GetColumnWidth(), GetRowHeight()), true );

		sizeMax.cx = max( sizeButton.cx, sizeMax.cx );
		sizeMax.cy = max( sizeButton.cy, sizeMax.cy );
	}
	//sizeCustButton = m_pCustomizeBtn->OnCalculateSize (&dc, CSize (	GetColumnWidth (), GetRowHeight ()), bHorz);

	CRect	rect;
	GetClientRect( &rect );
	CSize	size( rect.Width(), /*GetRowHeight()+5*/sizeMax.cy );
	int		yOfs = m_rectUp.bottom;

	pos = m_Buttons.GetHeadPosition();
	while( pos )
	{
		CBCGToolbarButton	*pbtn = (CBCGToolbarButton*)m_Buttons.GetNext( pos );
		
		if( pbtn->IsKindOf( RUNTIME_CLASS( CShellToolbarButton ) ) )
		{
			((CShellToolbarButton*)pbtn)->SetButtonStyle( 0 );
			((CShellToolbarButton*)pbtn)->SetMenuMode( true );
			((CShellToolbarButton*)pbtn)->SetDrawAccel( false );

		}


		CRect	rectButton = NORECT;
		
		if( nPos >= m_nVisibleOffset )
		{
			rectButton = CRect( 0, yOfs, size.cx, yOfs+size.cy );
			if( rectButton.bottom < m_rectDown.top )
			{
				m_nVisibleCount ++;
				yOfs+=size.cy;
			}
			else
				rectButton = NORECT;
		}

		pbtn->SetRect( rectButton );
		nPos++;


		
	} 
}

int CShellOutlookToolBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CShellToolBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	OnUpdateCmdUI( (CFrameWnd*)AfxGetMainWnd(), true );

	SetTimer( 776, 50, 0 );

	return 0;
}

BOOL CShellOutlookToolBar::IsMenuMode() const
{
	return true;
}

BOOL CShellOutlookToolBar::OnEraseBkgnd(CDC* pDC)
{
	return ::ProcessEraseBackground( pDC->GetSafeHdc(), GetSafeHwnd() );
}


void CShellOutlookToolBar::OnTimer(UINT_PTR nIDEvent) 
{
	if( nIDEvent == 776 )
	{
		CPoint	point;
		GetCursorPos( &point );
		ScreenToClient( &point );

		if( m_rectUp.PtInRect( point ) )
		{
			if( m_nVisibleOffset > 0 )
			{
				if( !m_nTime || m_nTime >= 4 )
				{
					m_nVisibleOffset--;
					AdjustLocations();
					Invalidate();
				}
				m_nTime++;
			}
		}
		else
		if( m_rectDown.PtInRect( point ) )
		{
			if( m_nVisibleOffset < GetCount()-m_nVisibleCount )
			{
				if( !m_nTime || m_nTime >= 4 )
				{
					m_nVisibleOffset++;
					AdjustLocations();
					Invalidate();
				}
				m_nTime++;
			}
		}
		else 
			m_nTime = 0;
	}
	
	CShellToolBar::OnTimer(nIDEvent);
}

void CShellOutlookToolBar::OnClose() 
{
	KillTimer( 776 );
	
	CShellToolBar::OnClose();
}

void CShellOutlookToolBar::OnDestroy() 
{
	KillTimer( 776 );
	CShellToolBar::OnDestroy();
	
}

void  CShellOutlookToolBar::OnFillBackground (CDC* pDC )
{
	ProcessEraseBackground( pDC->GetSafeHdc(), GetSafeHwnd() );
}

void CShellOutlookToolBar::DoPaint(CDC* pDC)
{

	CShellToolBar::DoPaint(pDC);

	if( m_nVisibleCount < GetCount() )
	{
		CPoint	points1[3];
		CPoint	points2[3];

		points1[0] = CPoint( m_rectUp.CenterPoint().x, m_rectUp.CenterPoint().y-1 );
		points1[1] = CPoint( m_rectUp.CenterPoint().x-3, m_rectUp.CenterPoint().y+3 );
		points1[2] = CPoint( m_rectUp.CenterPoint().x+3, m_rectUp.CenterPoint().y+3 );

		points2[0] = CPoint( m_rectDown.CenterPoint().x, m_rectDown.CenterPoint().y+1 );
		points2[1] = CPoint( m_rectDown.CenterPoint().x-3, m_rectDown.CenterPoint().y-3 );
		points2[2] = CPoint( m_rectDown.CenterPoint().x+3, m_rectDown.CenterPoint().y-3 );

		
		pDC->SelectStockObject( NULL_PEN );
		if( m_nVisibleOffset > 0 )
		{
			pDC->SelectStockObject( BLACK_BRUSH );
			pDC->Polygon( points1, 3 );
		}
		else
		{
			//pDC->SelectStockObject( GRAY_BRUSH );
		}

		

		if( m_nVisibleOffset < GetCount()-m_nVisibleCount )
		{
			pDC->SelectStockObject( BLACK_BRUSH );
			pDC->Polygon( points2, 3 );
		}
		else
		{
			//pDC->SelectStockObject( GRAY_BRUSH );
		}

		
	}
}

BOOL CShellOutlookToolBar::EnableContextMenuItems (CBCGToolbarButton* pButton, CMenu* pPopup)
{
	if( !CBCGToolBar::EnableContextMenuItems( pButton, pPopup ) )
		return false;

	if( !pButton->IsKindOf( RUNTIME_CLASS( CShellToolbarButton ) ) )
		return true;

	CShellToolbarButton	*pshellButton = (CShellToolbarButton	*)pButton;
	pPopup->RemoveMenu( ID_BCGBARRES_TOOLBAR_APPEARANCE, MF_BYCOMMAND );
	pPopup->RemoveMenu( ID_BCGBARRES_TOOLBAR_TEXT, MF_BYCOMMAND );
	pPopup->RemoveMenu( ID_BCGBARRES_TOOLBAR_IMAGE_AND_TEXT, MF_BYCOMMAND );
	pPopup->RemoveMenu( ID_BCGBARRES_TOOLBAR_IMAGE, MF_BYCOMMAND );

	CMenu	menu;
	menu.LoadMenu( IDR_TOOLBAR_APPEND );
	
	CMenu	*pPopupSource = menu.GetSubMenu( 0 );
	pPopup->AppendMenu( MF_SEPARATOR );

	for( int n = 0; n < (int)pPopupSource->GetMenuItemCount(); n++ )
	{
		CString	strMenu;
		UINT nCmdID = pPopupSource->GetMenuItemID( n );
		pPopupSource->GetMenuString( n, strMenu, MF_BYPOSITION );

		if( strMenu.IsEmpty() )
			pPopup->AppendMenu( MF_SEPARATOR );
		else
			pPopup->AppendMenu( MF_STRING, nCmdID, strMenu );
	}

	pPopup->RemoveMenu( ID_SHOW_AS_TEXT, MF_BYCOMMAND );
	pPopup->RemoveMenu( ID_SHOW_AS_IMAGE, MF_BYCOMMAND );
	pPopup->RemoveMenu( ID_SHOW_AS_IMAGE_AND_TEXT, MF_BYCOMMAND );
	
	for( int nCount = pPopup->GetMenuItemCount()-1; nCount > 0; nCount-- )
	{
		CString	strMenu1, strMenu2;
		pPopup->GetMenuString( nCount-1, strMenu1, MF_BYPOSITION );
		pPopup->GetMenuString( nCount, strMenu2, MF_BYPOSITION );
		if( strMenu1.IsEmpty() && strMenu2.IsEmpty() )
		{
			pPopup->RemoveMenu( nCount, MF_BYPOSITION );
		}
	}
	return true;
}




////////////////////////////////////////////////////////////////////////////
CShellFrameToolBar::CShellFrameToolBar()
{
	m_bOldVisible = false;
}

/////////////////////////////////////////////////////////////////////////////
CShellFrameToolBar::~CShellFrameToolBar()
{

}

BEGIN_MESSAGE_MAP(CShellFrameToolBar, CShellToolBar)
	//{{AFX_MSG_MAP(CShellFrameToolBar)	
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CShellFrameToolBar::PreTranslateMessage(MSG* pMsg)
{
	BOOL bRet = CShellToolBar::PreTranslateMessage( pMsg );
//	CBCGToolBar::m_bShowTooltips = true;
	return bRet;
}

BOOL CShellFrameToolBar::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if( message == WM_WINDOWPOSCHANGED || message == WM_PAINT )
	{
		BOOL bres = CShellToolBar::OnWndMsg(message, wParam, lParam, pResult);

		bool	bVisible = false;
		if( message == WM_WINDOWPOSCHANGED )
			bVisible = !( ((WINDOWPOS*)lParam)->flags & SWP_HIDEWINDOW );
		else
			bVisible = ( IsWindowVisible() == TRUE );
	
		if( bVisible != m_bOldVisible )
		{
			CWnd* pParent = GetParent();
			if( pParent && pParent->IsKindOf( RUNTIME_CLASS(CShellFrame) ) )
			{
				CShellFrame* pframe = (CShellFrame*)pParent;
				pframe->_RecalcLayoyt( true );
			}
		}

		m_bOldVisible = bVisible;

		return bres;
	}

	return CShellToolBar::OnWndMsg(message, wParam, lParam, pResult);

}



/////////////////////////////////////////////////////////////////////////////
void CShellFrameToolBar::AdjustLocations ()
{
	/*
	CWnd* pWndParent = GetParent();
	if( !pWndParent || !pWndParent->GetSafeHwnd() )
	{
		CBCGToolBar::AdjustLocations();
		return;
	}

	

	CRect rcParent = NORECT;
	pWndParent->GetClientRect( &rcParent );	

	CRect rc = NORECT;
	GetWindowRect( &rc );
	rc.right = rc.left + rcParent.Width();

	ScreenToClient( &rc );
	MoveWindow( &rc );
	*/

	CBCGToolBar::AdjustLocations();
	
}

/*/////////////////////////////////////////////////////////////////////////////
BOOL CShellFrameToolBar::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point)
{
	return CBCGToolBar::OnDrop( pDataObject, dropEffect, point );

}

/////////////////////////////////////////////////////////////////////////////
DROPEFFECT CShellFrameToolBar::OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return CBCGToolBar::OnDragEnter( pDataObject, dwKeyState, point );

}

/////////////////////////////////////////////////////////////////////////////
void CShellFrameToolBar::OnDragLeave()
{
	CBCGToolBar::OnDragLeave();
}

/////////////////////////////////////////////////////////////////////////////
DROPEFFECT CShellFrameToolBar::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	return CBCGToolBar::OnDragOver( pDataObject, dwKeyState, point );
}*/


/////////////////////////////////menu bar


BEGIN_MESSAGE_MAP(CShellMenuBar, CBCGMenuBar)
	ON_MESSAGE(WM_HELPHITTEST, OnHelpHitTest)
	ON_WM_NCHITTEST()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

LRESULT CShellMenuBar::OnNcHitTest(CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	return CBCGMenuBar::OnNcHitTest(point);
}

BOOL CShellMenuBar::OnEraseBkgnd(CDC* pDC)
{
	return ::ProcessEraseBackground( pDC->GetSafeHdc(), GetSafeHwnd() );
}



LRESULT CShellMenuBar::OnHelpHitTest( WPARAM wParam, LPARAM lParam )
{
	int nIndex = HitTest ((DWORD) lParam);
	if (nIndex < 0)	return -1;
	CBCGToolbarButton* pButton = GetButton (nIndex);

	if( pButton->OnContextHelp(this ) )
		return (DWORD)-2;

	return pButton->m_nID;
}


//own menubar
HMENU CShellMenuBar::LoadMainMenu()
{
	return g_CmdManager.GetMenu();
}

HMENU CShellMenuBar::LoadDocumentMenu( CBCGMultiDocTemplate *pTemplate )
{
	return g_CmdManager.GetMenu();
}

void CShellMenuBar::SetMenuName()
{
	if( GetSafeHwnd() )
	{
		CString str_menu_name = ::LanguageLoadCString( IDS_MENU_BAR_NAME );
		//str_menu_name.LoadString( IDS_MENU_BAR_NAME );
		SetWindowText( str_menu_name );
	}
}

void CShellMenuBar::ReloadMenu()
{
	if (m_hMenu != NULL && g_menuHash.LoadMenuBar (m_hMenu, this))
	{
		SetMenuName();
		GetParentFrame ()->RecalcLayout ();
		Invalidate ();
		UpdateWindow ();
	}
}

BOOL CShellMenuBar::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CBCGMenuBar::PreCreateWindow( cs ) )
		return false;
	
	return true;
}
void CShellMenuBar::OnBarStyleChange(DWORD, DWORD)
{
//	m_dwStyle &= ~(CBRS_BORDER_ANY|CBRS_GRIPPER);
	m_dwStyle |= TBSTYLE_TRANSPARENT;
}


void  CShellMenuBar::OnFillBackground (CDC* pDC )
{
	ProcessEraseBackground( pDC->GetSafeHdc(), GetSafeHwnd() );
}

BOOL CShellMenuBar::OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point) 
{
	if (!CanDragDropButtons())
		return FALSE;
	return __super::OnDrop(pDataObject,dropEffect,point);
}

DROPEFFECT CShellMenuBar::OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point)
{
	if (!CanDragDropButtons())
		return DROPEFFECT_NONE;
	return __super::OnDragOver(pDataObject, dwKeyState, point);
}


bool CShellMenuBar::CanDragDropButtons()
{
	if (m_hWnd)
	{
		if (!GetValueInt(GetAppUnknown(), "\\MainFrame\\EnableChangeMenu", "Main menu", TRUE))
			return false;
	}
	return ( g_lAllowCustomize == 1 );
}

BOOL CShellToolBar::OnEraseBkgnd(CDC* pDC) 
{
	return ::ProcessEraseBackground( pDC->GetSafeHdc(), GetSafeHwnd() );
//	return CBCGToolBar::OnEraseBkgnd(pDC);
}
///////////////////////////////////////////////////////////////////////////////
BOOL ProcessEraseBackground( HDC hdc, HWND hwnd )
{
	RECT	rect_client;
	::GetClientRect( hwnd, &rect_client );
	
	if( !GetValueInt( GetAppUnknown(), "\\Interface", "EnableSkin", 0 ) )
	{
		::FillRect( hdc, &rect_client, ::GetSysColorBrush( COLOR_3DFACE ) );
		return true;
	}

	static HWND hwnd_main = 0;
	if( hwnd_main == 0 )hwnd_main = AfxGetMainWnd()->GetSafeHwnd();

	RECT	rect_main, rect_tool;
	GetWindowRect( hwnd_main, &rect_main );
	CopyRect( &rect_tool, &rect_client );
	::MapWindowPoints( hwnd, hwnd_main, (POINT*)&rect_tool, 2 );

	static char	sz_filename[MAX_PATH] = "";
	CString	str_file = GetValueString( GetAppUnknown(), "\\Interface", "SkinFile", "skin.bmp" );

	static HBITMAP	hbmp = 0;

	if( stricmp( sz_filename, str_file ) )
	{
		strcpy( sz_filename, str_file );

		if( hbmp )::DeleteObject( hbmp );
		hbmp = 0;

		if( strlen( sz_filename ) )
			hbmp = (HBITMAP)::LoadImage(	0, sz_filename, IMAGE_BITMAP, 0, 0, 
										LR_DEFAULTSIZE|LR_LOADFROMFILE );
	}

	if( !hbmp )
	{
		::FillRect( hdc, &rect_client, ::GetSysColorBrush( COLOR_3DFACE ) );
		return true;
	}

	BITMAP	bmp;
	::GetObject( hbmp, sizeof( bmp ), &bmp );

	int	x_ofs = rect_tool.left%bmp.bmWidth;
	int	y_ofs = rect_tool.top%bmp.bmHeight;

	HDC	hdc_mem = ::CreateCompatibleDC( hdc );
	::SelectObject( hdc_mem, hbmp ); 

	//win9x не суппортит кисти больше 8x8
	for( int x_pos = rect_client.left-x_ofs; x_pos < rect_client.right; x_pos += bmp.bmWidth )
	{
		for( int y_pos = rect_client.top-y_ofs; y_pos < rect_client.bottom; y_pos += bmp.bmHeight )
		{
			int	x = max( x_pos, 0 );
			int	y = max( y_pos, 0 );
			int	cx = min( bmp.bmWidth-max( -x_pos, 0 ), rect_client.right );
			int	cy = min( bmp.bmHeight-max( -y_pos, 0 ), rect_client.bottom );

			::BitBlt( hdc, x, y, cx, cy,
				hdc_mem, max( -x_pos, 0 ), max( -y_pos, 0 ), SRCCOPY );
		}
	}

	::DeleteDC( hdc_mem );
	return true;
}
