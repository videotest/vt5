#include "stdafx.h"
#include "shellbutton.h"
#include "ActionControlWrp.h"
#include "CommandManager.h"

#include "resource.h"
#include "MainFrm.h"




int	CShellToolbarButton::s_nButtonStyle = 1;

IMPLEMENT_SERIAL(CShellToolbarButton, CBCGToolbarMenuButton, 1)
IMPLEMENT_SERIAL(CShellChooseToolbarButton, CShellToolbarButton, 1)
IMPLEMENT_SERIAL(CShellMenuButton, CShellToolbarButton, 1)
IMPLEMENT_SERIAL(CShellMenuComboBoxButton, CBCGToolbarComboBoxButton, 1)


IMPLEMENT_SERIAL(CShellPopupMenu, CBCGPopupMenu, 1)


#pragma message( "Linking with htmlhelp.lib" )
#pragma comment(lib, "htmlhelp.lib")

///ordinary toolbar button

void CShellToolbarButton::SetImage( int nImage )
{
	if( m_bUserButton )
		m_iUserImage = nImage;
	else
		m_iImage = nImage;

	if( GetImage() == -1 )
	{
		m_bImage = false;
		m_bText = true;
	}
}

void CShellToolbarButton::Serialize (CArchive& ar)
{
	if( m_bMenuSerialize )
		CBCGToolbarMenuButton::Serialize( ar );
	else
		CBCGToolbarButton::Serialize( ar );

	if( ar.IsLoading() )
		_UpdateUI();
}

void CShellToolbarButton::OnClosePopupMenu( int nCmd )
{
	CBCGToolbarMenuButton::OnClosePopupMenu( nCmd );

	m_bClickedOnMenu = false;
}

void CShellToolbarButton::SetButtonSize( CSize size )
{
	CSize	sizeButton = size;
	CSize	sizeImage = CSize (16, 15);
	if( GetButtonsStyle() == SemiFlat||
		GetButtonsStyle() == Ordinary )
	{
		sizeButton.cx += 2;
		sizeButton.cy += 2;
	}

	if( GetButtonsStyle() == Round )
	{
		sizeButton.cx += sizeButton.cx/2;
		sizeButton.cy += sizeButton.cy/2;
	}
	CBCGToolBar::SetSizes( sizeButton, CSize (16, 15) );
	CBCGToolBar::SetMenuSizes ( sizeButton, CSize (16, 15) );
}

///button with sub-menu
CShellToolbarButton::CShellToolbarButton() 
	: CBCGToolbarMenuButton() 
{
	m_pPopupParent = NULL;

	m_nDrawStyle = CShellToolbarButton::GetButtonsStyle();
	m_pbar = 0;
	m_bAlignLeft = false;
	m_bMenuMode = false;
	m_pChild = 0;
	m_bMenuSerialize = true;

	_UpdateUI();
}
CShellToolbarButton::CShellToolbarButton(UINT uiID, HMENU hMenu, int iImage, LPCTSTR lpszText, BOOL bUserButton) : 
	CBCGToolbarMenuButton( uiID, hMenu, iImage, lpszText, bUserButton )

{
	m_pPopupParent = NULL;

	m_nDrawStyle = CShellToolbarButton::GetButtonsStyle();
	m_pbar = 0;
	m_pChild = 0;
	m_bMenuSerialize = true;

	_UpdateUI();
}

CShellToolbarButton::CShellToolbarButton(UINT uiID, int iImage, LPCTSTR lpszText, BOOL bUserButton) :
	CBCGToolbarMenuButton( uiID, 0, iImage, lpszText, bUserButton )
{
	m_pPopupParent = NULL;

	m_nDrawStyle = CShellToolbarButton::GetButtonsStyle();
	m_pbar = 0;
	m_pChild = 0;
	m_bMenuSerialize = true;
	

	_UpdateUI();
}

CShellToolbarButton::~CShellToolbarButton()
{
	if( m_pChild )
	{
		m_pChild->DestroyWindow();
		delete m_pChild;
	}	
	
	DestroyPopup();
}

void CShellToolbarButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
				BOOL bHorz, BOOL bCustomizeMode,
				BOOL bHighlight,
				BOOL bDrawBorder,
				BOOL bGrayDisabledButtons)
{
	bHorz = true;

	if( m_pChild )
		return;

	CRect	rectDraw = rect;	
	rectDraw.bottom--;


	if( /*m_bMenuMode*/IsMenuMode() )
	{
		m_bClickedOnMenu = false;
		CBCGToolbarMenuButton::OnDraw( pDC, rect, pImages, bHorz, bCustomizeMode, false, true, bGrayDisabledButtons );

		if( bHighlight /*&& (!m_bClickedOnMenu || )*/)
		{
			if( !m_pbar->IsKindOf( RUNTIME_CLASS( CBCGMenuBar ) ) )
				rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
		}

		return;
	}


	CRgn	rgn;

	if( m_nDrawStyle == Round )
	{
		rgn.CreateEllipticRgn( rect.left, rect.top, rect.right, rect.bottom );
		pDC->SelectClipRgn( &rgn );
	}

	CBCGToolbarMenuButton::OnDraw( pDC, rect, pImages, true, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons );

	if( m_nDrawStyle == Round )
		pDC->SelectClipRgn( 0 );

	if( m_nDrawStyle == SemiFlat )
	{
		if( m_nStyle & TBBS_PRESSED || m_nStyle & TBBS_CHECKED )
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
		else
		{
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			if( bHighlight )
			{
				rectDraw.InflateRect( -1, -1 );
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
			}
		}
	} else 	if( m_nDrawStyle == Ordinary )
	{
		if( m_nStyle & TBBS_PRESSED || m_nStyle & TBBS_CHECKED )
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
		else
		{
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}
	} else if( m_nDrawStyle == Round )
	{

		
		pDC->SelectStockObject( NULL_BRUSH );
		
		CPen	penLight( PS_SOLID, 0, ::GetSysColor( COLOR_3DHILIGHT ) );
		CPen	penFace( PS_SOLID, 0, ::GetSysColor( COLOR_3DFACE ) );
		CPen	penShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DSHADOW ) );
		CPen	penDkShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DDKSHADOW ) );
		
		bool	bPushed = m_nStyle & TBBS_PRESSED || m_nStyle & TBBS_CHECKED;

		CPoint	point1 = CPoint( rectDraw.right, rectDraw.top );
		CPoint	point2 = CPoint( rectDraw.left, rectDraw.bottom );

		pDC->SelectObject( bPushed?&penDkShadow:&penLight );
		pDC->Arc( rectDraw, point1, point2 );

		pDC->SelectObject( bPushed?&penLight:&penDkShadow );
		pDC->Arc( rectDraw, point2, point1);

		if( bHighlight )
		{
			rectDraw.InflateRect( -1, -1 );
			pDC->SelectObject( bPushed?&penShadow:&penFace );
			pDC->Arc( rectDraw, point1, point2 );

			pDC->SelectObject( bPushed?&penFace:&penShadow );
			pDC->Arc( rectDraw, point2, point1);
		}

		pDC->SelectStockObject( BLACK_PEN );
	}
}

IUnknown *CShellToolbarButton::GetInterface( long lHit )
{
	if( m_nID > 0  )return 0;
	return this;
}


HRESULT CShellToolbarButton::QueryInterface( const IID &	iid, void **ppvObject )
{
	if( iid == IID_IHelpInfo )
	{
		*ppvObject = (IHelpInfo*)this;
		((IUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}

	if( iid == IID_IUnknown )
	{
		*ppvObject = (IUnknown*)(IHelpInfo*)this;
		((IUnknown*)*ppvObject)->AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}


HRESULT CShellToolbarButton::GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags )
{
	if(pbstrHelpFile) *pbstrHelpFile = m_strHelpFileName.AllocSysString();
	if(pbstrHelpTopic) *pbstrHelpTopic = m_strTopic.AllocSysString();
	if(pdwFlags)*pdwFlags = 0;

	return S_OK;
}

void CShellToolbarButton::StoreString( CStringArray &strs, int &idx, int level, bool bUseCollection /*= true*/ )
{
	if( m_nID >= ID_CMDMAN_BASE || m_nID < ID_CMDMAN_BASE+ID_CMDMAN_MAX )
	{
		CActionInfoWrp	*p = g_CmdManager.GetActionInfo( m_nID - ID_CMDMAN_BASE );
		if( p )
		{
			CString	strActionName = p->GetActionShortName();
			CString	strActionUserName = p->GetActionUserName();

			IUnknown	*punk = p->Unknown();
			IHelpInfoPtr	ptrHelp( punk );
			if( punk )punk->Release();

			if( ptrHelp != 0 )
			{
				BSTR	bstrFile = 0, bstrTopic = 0, bstrPrefix = 0;
				DWORD	dwFlags = 0;

				// [vanek] : IHelpInfo2 - 09.11.2004
				IHelpInfo2Ptr sptr_help2 = ptrHelp;
				if( sptr_help2 != 0 )
                    sptr_help2->GetHelpInfo2( &bstrFile, &bstrTopic, &dwFlags, &bstrPrefix );
				else
					ptrHelp->GetHelpInfo( &bstrFile, &bstrTopic, &dwFlags );

				CString	strModuleName = bstrFile;
				CString	strTopicName = bstrTopic;
				CString strPrefix = bstrPrefix;

				::SysFreeString( bstrFile );	bstrFile = 0;
				::SysFreeString( bstrTopic );	bstrTopic = 0;
				::SysFreeString( bstrPrefix );	bstrPrefix = 0;

				char	szName[_MAX_PATH];
				::_splitpath( strModuleName, 0, 0, szName, 0 );


				CString	strLink;
				

				for( int l = 0; l < level; l++ )
					strLink+="   ";

				// [vanek] : новый вариант help - 10.11.2004
				if( bUseCollection )
				{	// старый выриант

					// откорректируем strModuleName с учетом префикса
					{
						TCHAR szNewModuleName[_MAX_PATH] = {0}, szDrive[_MAX_DRIVE] = {0}, szDir[_MAX_DIR] = {0},
							szFileName[_MAX_FNAME] = {0}, szExt[_MAX_EXT] = {0};
                        
						_tsplitpath( strModuleName, szDrive, szDir, szFileName, szExt );
						CString strNewFileName = strPrefix;
						strNewFileName += szFileName;
						_tmakepath( szNewModuleName, szDrive, szDir, strNewFileName, szExt );\
						strModuleName = szNewModuleName;
					}

                    strLink+= "<li><a href=\"ms-its:";
					strLink+= strModuleName;
					strLink+= ".chm::/";
					strLink+= strPrefix;		// используем префикс
					strLink+= szName;
					strLink+= "/";
					strLink+= strTopicName;
					strLink+= ".html";
					strLink+= "\">";

					strLink+= strActionUserName;
					strLink+= "</a></li>";
					strs.InsertAt( idx++, strLink );
				}
				else
				{	// новый вариант
					CString str_chm_name( szName );		
					if( strPrefix.GetLength() )
					{
						if( strPrefix == _T("ax") )
							str_chm_name = _T("Forms");
						else if( strPrefix == _T("as") )
							str_chm_name = _T("Scripts");
					}
                    
					CString str_help_path( _T("") );
					str_help_path = GetDirectory( "HelpPath", "Help" );

					// вначале пытаемся найти файл <str_chm_file> = "<help_path>\<str_chm_name>.chm"
					CString str_chm_file(str_help_path);
					str_chm_file += str_chm_name;
					str_chm_file += _T(".chm"); 
					HANDLE	hfile = ::CreateFile( str_chm_file, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING,  
						FILE_ATTRIBUTE_NORMAL, 0 );

					strLink+= "<li><a href=\"ms-its:";

					CString str_folder_name( str_chm_name );
					if( hfile != INVALID_HANDLE_VALUE )
					{
						::CloseHandle( hfile ); 
						hfile = 0;
					}
					else
						str_chm_name = ::GetValueString( GetAppUnknown(), "\\General", "ProgramName", "Main" );                 
					
					strLink+= str_chm_name;
					strLink+= ".chm::/";
					strLink+= str_folder_name;
					strLink+= "/";
					strLink+= strTopicName;
					strLink+= ".html";
					strLink+= "\">";						

					strLink+= strActionUserName;
					strLink+= "</a></li>";

					strs.InsertAt( idx++, strLink );
				}
			}
		}
	}

	POSITION	pos = m_listCommands.GetHeadPosition();
	while( pos )
	{
		CBCGToolbarButton	*pbtn = (CBCGToolbarButton	*)m_listCommands.GetNext( pos );

		if( pbtn->IsKindOf( RUNTIME_CLASS(CShellToolbarButton)))
			((CShellToolbarButton*)pbtn)->StoreString( strs, idx, level+1, bUseCollection );
	}
}

BOOL CShellToolbarButton::OnContextHelp( CWnd *pwnd )
{
	if( (int)m_nID > 0  )
		return CBCGToolbarMenuButton::OnContextHelp( pwnd );


	//get the help directory

	CString	strPathName = GetDirectory( "HelpPath", "Help" );
	strPathName += "list_template.html";

	// [vanek] : новый вариант help - без использования коллекции - 10.11.2004
	bool buse_col = ::GetValueInt( GetAppUnknown(), "\\Help", "UseCollection", 1 ) != 0;

	//parse html file
	CStringArrayEx	strs;

	try{
	strs.ReadFile( strPathName );
	}
	catch( CException *pe ) 
	{
		pe->ReportError();
		pe->Delete();
	}

	if( strs.GetSize() == 0 )
		return true;

	int	idx1 = strs.Find( "<!-- list start -->" );
	int	idx2 = strs.Find( "<!-- list end -->" );

	if( idx1 == -1 || idx2 == -1 || idx2 < idx1+1 )
		return true;

	strs.RemoveAt( idx1+1, idx2-idx1-2 );
	int	idx = idx1+1;

	strs.InsertAt( idx++, "<ul>" );

	StoreString( strs, idx, 0, buse_col );
	
	strs.InsertAt( idx++, "</ul>" );

	strs.WriteFile( strPathName );

	if( buse_col )
	{
		//define help window
		HWND	hwnd = *AfxGetMainWnd();
		::ReleaseCapture();


		//load collection
		::HelpLoadCollection("$GLOBAL_main");

		//execute htmlhelp
		HWND hwndHelp = ::HtmlHelp( hwnd, "text_menu.chm::/text_menu/text_menu.html>$GLOBAL_main", HH_DISPLAY_TOPIC, 0 );
		::ShowWindow( hwndHelp, SW_SHOW );
	}
	else
		HelpDisplayTopic( "text_menu", 0, "text_menu", "$GLOBAL_main" );

	return true;
}

SIZE CShellToolbarButton::OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz)
{
	if( !IsMenuMode() )
	{
		if (m_pChild->GetSafeHwnd () != NULL && !m_bIsHidden)
		{
			m_pChild->ShowWindow (SW_SHOWNOACTIVATE);
		}

		CSize	size = CBCGToolbarMenuButton::OnCalculateSize( pDC, sizeDefault, true );
		if( !m_pbar )
			return size;
		
		if( m_bText && !m_bImage )
			size.cx += 5;

		return size;

		
	}
	else
		return CBCGToolbarMenuButton::OnCalculateSize( pDC, sizeDefault, bHorz );

}

void CShellToolbarButton::OnChangeParentWnd( CWnd* pwnd )
{
	if( pwnd->IsKindOf( RUNTIME_CLASS( CBCGToolBar ) ) )
		m_pbar = (CBCGToolBar*)pwnd;
	else
		m_pbar = 0;

	CBCGToolbarMenuButton::OnChangeParentWnd( pwnd );

	if( !m_pChild )
		m_pChild = g_CmdManager.GetActionControl( m_nID );

	if( m_pChild )
	{
		if( m_pChild->GetSafeHwnd() )
		{
			CWnd	*pwndParent = m_pChild->GetParent();
			if( pwndParent != pwnd )
				m_pChild->DestroyWindow();
		}
		m_pChild->Create( pwnd );
		m_pChild->MoveWindow( Rect() );
	}

	if( m_ptrSubMenu != NULL )
	{
		m_bDrawDownArrow = true;
		//m_bMenuMode = true;
	}
}

bool CShellToolbarButton::IsMenuMode()
{
	if( !m_pbar )
		return false;
	if( m_pbar->IsKindOf( RUNTIME_CLASS( CBCGMenuBar ) ) )
		return true;
	return m_pbar->IsMenuMode()!=0;
}

void CShellToolbarButton::OnMove()
{
	if( m_pChild->GetSafeHwnd() )
		m_pChild->MoveWindow( Rect() );
}

void CShellToolbarButton::OnSize (int iSize)
{
	if( m_pChild->GetSafeHwnd() )
		m_pChild->MoveWindow( Rect() );
}

class CMenuInfo
{
public:
	CMenuInfo( UINT uiItemID){
		m_hMenu = ::CreateMenu();
		m_uiItemID = uiItemID;		
	}

	~CMenuInfo(){
		if( m_hMenu )
		::DestroyMenu( m_hMenu );
	}

protected:
	UINT m_uiItemID;
	HMENU m_hMenu;
public:
	UINT GetItemID()	{ return m_uiItemID; }
	HMENU GetMenu()		{ return m_hMenu; }

};

/////////////////////////////////////////////////////////////////////////////////////
HMENU FindMenuByPos( UINT uiItemID, CArray<CMenuInfo*,CMenuInfo*>& arMenu )
{

	for( int i=0;i<arMenu.GetSize();i++ )	
	{
		if( arMenu[i]->GetItemID() == uiItemID )
			return arMenu[i]->GetMenu();
	}

	return NULL;		
}

/////////////////////////////////////////////////////////////////////////////////////
void CShellToolbarButton::CreatePopup( bool bConstructNow )
{
	DestroyPopup();

	if( m_ptrSubMenu == NULL )
		return;


	//At first clear
	CreateFromMenu( NULL );

//	m_ptrSubMenu->SetStartItemID( ID_CMD_POPUP_MIN );

//	m_ptrSubMenu->SetSingleObjectName( _bstr_t( m_strText ) );

	TPOS lPos = 0;
	m_ptrSubMenu->GetFirstItemPos( &lPos );
	
	
	//For destroy
	CArray<CMenuInfo*,CMenuInfo*> arMenu;

	CMenuInfo* pRootMenu = new CMenuInfo( -1 );	
	arMenu.Add( pRootMenu );	

	while( lPos )
	{
		UINT uiFlags	= 0;
		BSTR bstrText	= 0;
		UINT uiItemID	= 0;
		UINT uiParentID	= -1;
		//long lCurPos	= lPos;

		m_ptrSubMenu->GetNextItem( &uiFlags, &uiItemID, &bstrText, &uiParentID, &lPos );
		CString strText = bstrText;
		::SysFreeString( bstrText );		

		bool bPopupMenu = false;
		if( uiFlags == MF_POPUP )
		{
			CMenuInfo* pMenu = new CMenuInfo( uiItemID );			
			arMenu.Add( pMenu );	
			bPopupMenu = true;
		}
		

		HMENU hMenuAppend = FindMenuByPos( uiParentID, arMenu );

		::AppendMenu( hMenuAppend, bPopupMenu ? MF_POPUP : uiFlags, 
			bPopupMenu ? (UINT)FindMenuByPos( uiItemID, arMenu ) : uiItemID, (LPCTSTR)strText );


		//menu.AppendMenu( uiFlags, uiItemID, strText );		
	}	
	

	CreateFromMenu( FindMenuByPos( -1/*Root*/, arMenu ) );

	for( int i=0;i<arMenu.GetSize();i++ )
	{
		delete arMenu[i];		
	}

	arMenu.RemoveAll();

}

/////////////////////////////////////////////////////////////////////////////////////
void CShellToolbarButton::OnPopupCommand(UINT uiCommandID)
{
	if( m_ptrSubMenu == NULL )
		return;

	m_ptrSubMenu->OnCommand( uiCommandID );

}

/////////////////////////////////////////////////////////////////////////////////////
void CShellToolbarButton::DestroyPopup()
{

}

BOOL CShellToolbarButton::OnClickUp()
{
	return m_ptrSubMenu==0?FALSE:TRUE;
}


/////////////////////////////////////////////////////////////////////////////////////
void CShellToolbarButton::_UpdateUI()
{
	CActionInfoWrp	*pi = g_CmdManager.GetActionInfo( m_nID - ID_CMDMAN_BASE );
	if( pi&&CheckInterface( pi->m_pActionInfo, IID_IActionInfo2 ) )
	{
		IUnknown	*punk = 0;
		IActionInfo2Ptr	ptrAI2 = pi->m_pActionInfo;
		ptrAI2->GetUIUnknown( &punk );

		m_ptrSubMenu = punk;		

		if( m_ptrSubMenu )
		{			
			m_ptrSubMenu->SetStartItemID( ID_CMD_POPUP_MIN );
			m_ptrSubMenu->SetSingleObjectName( _bstr_t( m_strText ) );
			m_bDrawDownArrow = true;
			//m_bMenuMode = true;

		}
		if( punk )punk->Release();
	}	
}

/////////////////////////////////////////////////////////////////////////////
bool CShellToolbarButton::IsPopupButton()
{
	return ( m_ptrSubMenu != NULL );
}

/////////////////////////////////////////////////////////////////////////////
BOOL CShellToolbarButton::OnClick (CWnd* pWnd, BOOL bDelay)
{
	if (m_pPopupMenu != NULL) //SBT1728(comment 2)
	{
		OnCancelMode();
		return true;
	}
	CreatePopup( true );
	return CBCGToolbarMenuButton::OnClick( pWnd, bDelay );
}

/////////////////////////////////////////////////////////////////////////////
void CShellToolbarButton::CopyFrom (const CBCGToolbarButton& src)
{	
	CBCGToolbarMenuButton::CopyFrom( src );
	_UpdateUI();	
}


/////////////////////////////////////////////////////////////////////////////////////
IUnknown* CShellToolbarButton::GetSubMenuPtr()
{
	if( m_ptrSubMenu )
	{
		m_ptrSubMenu.AddRef();
		return m_ptrSubMenu;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////
CBCGPopupMenu* CShellToolbarButton::CreatePopupMenu ()
{
	CShellPopupMenu* pPopup = new CShellPopupMenu( );			
	pPopup->SetParentButton( this );
	return (CBCGPopupMenu*)pPopup;
}

/////////////////////////////////////////////////////////////////////////////
void CShellToolbarButton::SetParentPopup( CShellPopupMenu* pPopupParent )
{
	m_pPopupParent = pPopupParent;
}


/////////////////////////////////////////////////////////////////////////////
CShellPopupMenu* CShellToolbarButton::GetParentPopup( )
{
	return m_pPopupParent;
}




/////////////////////////////////////////////////////////////////////////////
//
//
// CShellPopupMenu 
//
//
/////////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CShellPopupMenu, CBCGPopupMenu)
	//{{AFX_MSG_MAP(CShellPopupMenu)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


CShellPopupMenu::CShellPopupMenu()
{
	m_pParentButton = NULL;
}

/////////////////////////////////////////////////////////////////////////////
void CShellPopupMenu::SetParentButton( CShellToolbarButton* pParentButton )
{
	m_pParentButton = pParentButton;
}

/////////////////////////////////////////////////////////////////////////////
CShellToolbarButton* CShellPopupMenu::GetParentButton( )
{
	return m_pParentButton;
}

/////////////////////////////////////////////////////////////////////////////
int CShellPopupMenu::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBCGPopupMenu::OnCreate(lpCreateStruct) == -1)
		return -1;

	//Now set parent to button
	int nButtonCount = GetMenuItemCount();
	for( int i=0;i<nButtonCount;i++ )
	{
		CBCGToolbarMenuButton* pButton = GetMenuItem( i );
		if( pButton && pButton->IsKindOf( RUNTIME_CLASS( CShellToolbarButton ) ) )
		{
			CShellToolbarButton* pShellButton = (CShellToolbarButton*)pButton;
			pShellButton->SetParentPopup( this );
		}			
	}

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
BOOL CShellPopupMenu::PostCommand (UINT uiCommandID)
{
	BOOL bRes = CBCGPopupMenu::PostCommand( uiCommandID );

	ISubMenuPtr	ptrSubMenu;

	CShellPopupMenu* pMenu = this;

	bool bEnough = false;
	while( !bEnough )
	{
		CShellToolbarButton* pButton = pMenu->GetParentButton();

		if( pButton )
		{
			IUnknown* punkSubMenu = NULL;
			punkSubMenu = pButton->GetSubMenuPtr();
			if( punkSubMenu )
			{
				ptrSubMenu = punkSubMenu;
				punkSubMenu->Release();
			}
			else//Try get popup parent...
			{
				pMenu = pButton->GetParentPopup();
				if( !pMenu )
					bEnough = true;
			}
		}
		else
		{
			bEnough = true;
		}			

		if( ptrSubMenu != NULL )
			bEnough = true;
	}

	if( ptrSubMenu )
		ptrSubMenu->OnCommand( uiCommandID );

	return bRes;
}

BOOL CShellPopupMenu::UpdateCmdUI(CCmdUI *pCmdUI)
{
	ISubMenuPtr	ptrSubMenu;

	CShellPopupMenu* pMenu = this;

	bool bEnough = false;
	while( !bEnough )
	{
		CShellToolbarButton* pButton = pMenu->GetParentButton();

		if( pButton )
		{
			IUnknown* punkSubMenu = NULL;
			punkSubMenu = pButton->GetSubMenuPtr();
			if( punkSubMenu )
			{
				ptrSubMenu = punkSubMenu;
				punkSubMenu->Release();
			}
			else//Try get popup parent...
			{
				pMenu = pButton->GetParentPopup();
				if( !pMenu )
					bEnough = true;
			}
		}
		else
		{
			bEnough = true;
		}			

		if( ptrSubMenu != NULL )
			bEnough = true;
	}

	if (ptrSubMenu != 0)
	{
		TPOS lpos = 0;
		ptrSubMenu->GetFirstItemPos(&lpos);
		while (lpos)
		{
			UINT nID,nFlags = 0,nParentID;
			BSTR bstrName = NULL;
			ptrSubMenu->GetNextItem(&nFlags, &nID, &bstrName, &nParentID, &lpos);
			if (bstrName != NULL)
				SysFreeString(bstrName);
			if (nID == pCmdUI->m_nID)
			{
				if ((nFlags&MF_DISABLED) || (nFlags&MF_GRAYED))
					pCmdUI->Enable(FALSE);
				else
					pCmdUI->Enable(TRUE);
				return TRUE;
			}
		}
	}
	return FALSE;
}



/////////////////////////////////////////////////////////////////////////////
//
//
// CShellChooseToolbarButton 
//
//
/////////////////////////////////////////////////////////////////////////////////////
CShellChooseToolbarButton::CShellChooseToolbarButton()
{
	Initialize();
}

void CShellChooseToolbarButton::Initialize()
{
	CShellToolbarButton::Initialize();
	m_bClickedOnMenu = false;
	m_nLastUsedCmd = -1;
	m_bLocked = false;
	m_rectLastMenuBtn = NORECT;
	m_bMenu = false;

}

void CShellChooseToolbarButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
			BOOL bHorz, BOOL bCustomizeMode,
			BOOL bHighlight,
			BOOL bDrawBorder,
			BOOL bGrayDisabledButtons)
{
	bool	bPressed = (m_nStyle & TBBS_PRESSED) || 
					   (m_pPopupMenu != 0)||
					   (m_nStyle & TBBS_CHECKED);
	bool	bDisabled = (m_nStyle & TBBS_DISABLED) == TBBS_DISABLED;
	//CBCGToolbarMenuButton::OnDraw( pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons );
	CRect	rectArrow = rect;

	CBCGToolbarButton	*pButton = GetLastButton();
	if( pButton  )
	{

		if( m_nStyle & TBBS_CHECKED )
			CBCGToolBarImages::FillDitheredRect( pDC, rect );
		
		int nImage = GetImage();
		//pImages = (pButton->m_bUserButton) ? (CBCGToolBar::GetUserImages()):(CBCGToolBar::GetImages());

		//TRACE( "m_bUserButton = %d, Image = %d\n", pButton->m_bUserButton, nImage );

		if( pImages )
		{
			
			CSize sizeImage = pImages->GetImageSize (TRUE);
			CPoint	pointImageOffset( rect.left+ (rect.Height() -sizeImage.cy)/2,
									rect.CenterPoint().y -sizeImage.cy/2 );

			if( bPressed && !m_bMenu )
				pointImageOffset+=CPoint( 1, 1 );

			
			//CBCGDrawState ds;
			//pImages->PrepareDrawImage( ds/*, pImages->GetImageSize()*/ );

			//pImages->SetTransparentColor( RGB( 192, 192, 192 ) );
			pImages->Draw( pDC, pointImageOffset.x, pointImageOffset.y, nImage, false, bDisabled );

			//pImages->EndDrawImage (ds);

			
			rectArrow.left = pointImageOffset.x+(rect.Height()-sizeImage.cy)/2 + sizeImage.cx;
			if( bPressed && !m_bMenu )
				rectArrow.left--;

			pDC->FillRect( rectArrow, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );
		}
		{
			CSize	sizeArrow = CMenuImages::Size();
			CPoint	pointArrow( (rectArrow.CenterPoint().x -sizeArrow.cx/2),
							rectArrow.CenterPoint().y -sizeArrow.cy/2 );

			if( bPressed && m_bMenu )
				pointArrow+=CPoint( 1, 1 );

			CMenuImages::Draw( pDC, (CMenuImages::IMAGES_IDS)CMenuImages::IdArowDown, pointArrow);
		}

		/*CRect	rectSplitter = rectArrow;
		rectSplitter.right = rectSplitter.left - 1;
		pDC->Draw3dRect (&rectSplitter, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );*/

	}

	CRect	rectDraw = rect;
	rectDraw.right = rectArrow.left;

	m_rectLastMenuBtn = rectArrow;

	if( m_nDrawStyle == Round )
		pDC->SelectClipRgn( 0 );

	if( m_nDrawStyle == SemiFlat )
	{
		if( bPressed )
		{
			if( m_bMenu )
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			}
			else
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );	
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			}
		}
		else
		{
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
		}
		
		if( bHighlight )
		{
			/*rectArrow.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );*/
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}
	} 
	else if( m_nDrawStyle == Ordinary )
	{
		if( bPressed )
		{
			if( m_bMenu )
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );

				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
				rectDraw.InflateRect( -1, -1 );
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
			}
			else
			{
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );

				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
				rectArrow.InflateRect( -1, -1 );
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
			}
		}
		else
		{
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );

			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			rectArrow.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}

		if( bHighlight )
		{
			/*rectArrow.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );*/
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}
	} 
	else if( m_nDrawStyle == Round )
	{

		
		pDC->SelectStockObject( NULL_BRUSH );
		
		CPen	penLight( PS_SOLID, 0, ::GetSysColor( COLOR_3DHILIGHT ) );
		CPen	penFace( PS_SOLID, 0, ::GetSysColor( COLOR_3DFACE ) );
		CPen	penShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DSHADOW ) );
		CPen	penDkShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DDKSHADOW ) );
		
		bool	bPushed = m_nStyle & TBBS_PRESSED || m_nStyle & TBBS_CHECKED;

		CPoint	point1 = CPoint( rectDraw.right, rectDraw.top );
		CPoint	point2 = CPoint( rectDraw.left, rectDraw.bottom );

		pDC->SelectObject( bPushed?&penDkShadow:&penLight );
		pDC->Arc( rectDraw, point1, point2 );

		pDC->SelectObject( bPushed?&penLight:&penDkShadow );
		pDC->Arc( rectDraw, point2, point1);

		if( bHighlight )
		{
			rectDraw.InflateRect( -1, -1 );
			pDC->SelectObject( bPushed?&penShadow:&penFace );
			pDC->Arc( rectDraw, point1, point2 );

			pDC->SelectObject( bPushed?&penFace:&penShadow );
			pDC->Arc( rectDraw, point2, point1);
		}

		pDC->SelectStockObject( BLACK_PEN );

		if( bPressed )
		{
			if( m_bMenu )
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			else
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );	
		}
		else
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );

	}
	else if( m_nDrawStyle == Flat )
	{
		if( bHighlight )
		{
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
		}
	}


}

void CShellChooseToolbarButton::OnClosePopupMenu( int nCmd )
{
	m_nID = m_nLastUsedCmd = nCmd;
	m_bMenu = false;
}

CBCGToolbarButton	*CShellChooseToolbarButton::GetLastButton()
{
	if( m_nLastUsedCmd == -1 )
	{
		if( m_listCommands.GetCount() == 0 )
			return 0;

		CBCGToolbarButton	*pbtn = (CBCGToolbarButton	*)m_listCommands.GetHead();
		m_nID = m_nLastUsedCmd = pbtn->m_nID;
	}

	if( m_nLastUsedCmd == -1 ) 
		return 0;

	POSITION	pos = m_listCommands.GetHeadPosition();

	while( pos )
	{
		CBCGToolbarButton	*pbtn = (CBCGToolbarButton	*)m_listCommands.GetNext( pos );
		if( m_nLastUsedCmd == pbtn->m_nID )
		{
//			pbtn->SetImage( -1 );
			m_bUserButton = pbtn->m_bUserButton;
			m_iImage = CMD_MGR.GetCmdImage (pbtn->m_nID, false);
			m_iUserImage = CMD_MGR.GetCmdImage (pbtn->m_nID, true);
			
			return pbtn;
		}
	}

	return 0;
}

SIZE CShellChooseToolbarButton::OnCalculateSize( CDC* pDC, const CSize& sizeDefault, BOOL bHorz )
{
	CSize	size = sizeDefault;
	size.cx +=size.cx/2;

	return size;
}

BOOL CShellChooseToolbarButton::OnPreClick (CWnd* pWnd )
{
	m_bClickedOnMenu = false;

	CPoint	point;
	GetCursorPos( &point );
	pWnd->ScreenToClient( &point );

	m_bMenu = false;

	if( !m_rectLastMenuBtn.PtInRect( point ) )
		return false;

	m_bMenu = true;
	m_bClickedOnMenu = true;
	CShellToolbarButton::OnClick ( pWnd, false );

	//m_bMenu = false;

	return false;
}

BOOL CShellChooseToolbarButton::OnClick (CWnd* pWnd, BOOL bDelay )
{
	if( m_bClickedOnMenu && !CBCGToolBar::IsCustomizeMode() )
		return true;

	return CShellToolbarButton::OnClick ( pWnd, bDelay );
}

BOOL CShellChooseToolbarButton::OnClickUp()
{
	return false;
}

void CShellChooseToolbarButton::Serialize( CArchive &ar )
{
	int	nStoredID = m_nID;
	m_nID = -1;

	CShellToolbarButton::Serialize( ar );

	if( ar.IsStoring() )
		m_nID = nStoredID;
}

void CShellChooseToolbarButton::OnCancelMode()
{
	m_bMenu = false;
	CShellToolbarButton::OnCancelMode();
}


//////CShellMenuButton
CShellMenuButton::CShellMenuButton()
{
	Initialize();
	_UpdateUI();
}

CShellMenuButton::CShellMenuButton(UINT uiID, int iImage, IUnknown *punkMenuProvider, LPCTSTR lpszText, BOOL bUserButton )
	:CShellToolbarButton( uiID, iImage, lpszText, bUserButton )
{
	Initialize();

	m_ptrMenu = punkMenuProvider;
	_UpdateUI();
}

CShellMenuButton::~CShellMenuButton()
{
	if( m_ptr_action_ctrl )
	{
		HWND hwnd =  0;
		m_ptr_action_ctrl->GetHwnd( &hwnd );
		if( hwnd )
			::DestroyWindow( hwnd );
	}
}

void CShellMenuButton::Initialize()
{
	m_nOldWidth = 0;
	m_bMenu = false;
}



void CShellMenuButton::OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
			BOOL bHorz, BOOL bCustomizeMode,
			BOOL bHighlight,
			BOOL bDrawBorder,
			BOOL bGrayDisabledButtons)
{
	if( m_ptr_action_ctrl )
	{
		if( S_OK == m_ptr_action_ctrl->Draw( pDC->m_hDC, rect, 0 ) )
			return;
	}

	bool bPopupBtn = ( m_ptrSubMenu != NULL );
					
	//if( (m_bMenuMode && !bPopupBtn) || bPopupBtn )
	if ( m_bMenuMode )
	{
		CRect rectDraw = rect;
		//m_bClickedOnMenu = true;
		CBCGPopupMenu* pOldMenu = m_pPopupMenu;
		m_pPopupMenu = NULL;
		CBCGToolbarMenuButton::OnDraw( pDC, rect, pImages, bHorz, bCustomizeMode, false, true, bGrayDisabledButtons );		
		m_pPopupMenu = pOldMenu;

		if( bHighlight /*&& (!m_bClickedOnMenu || )*/)
		{
			if( !m_pbar->IsKindOf( RUNTIME_CLASS( CBCGMenuBar ) ) )
				rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
		}

		return;
	}



	bool	bPressed = (m_nStyle & TBBS_PRESSED) || 
					   (m_pPopupMenu != 0)||
					   (m_nStyle & TBBS_CHECKED);
	bool	bDisabled = (m_nStyle & TBBS_DISABLED) == TBBS_DISABLED;
	//CBCGToolbarMenuButton::OnDraw( pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons );
	CRect	rectArrow = rect;
	CRect	rectDraw = rect;

	rectDraw.bottom--;
	rectArrow.bottom--;

	rectDraw.right = rect.left+m_nOldWidth;
	rectArrow.left = rectDraw.right;

	rectDraw.right = rectArrow.left;
	m_rectLastMenuBtn = rectArrow;

	if( m_nStyle & TBBS_CHECKED )
		CBCGToolBarImages::FillDitheredRect( pDC, rect );
	
	int	nImage = GetImage();

	if( pImages )
	{
		CSize sizeImage = pImages->GetImageSize (TRUE);
		CPoint	pointImageOffset( rect.left+ (rect.Height() -sizeImage.cy)/2,
								rect.CenterPoint().y -sizeImage.cy/2 );

		if( (bPressed && !m_bMenu) || ( bPressed && bPopupBtn ) )
			pointImageOffset+=CPoint( 1, 1 );

		
		//pImages->SetTransparentColor( RGB( 192, 192, 192 ) );
		pImages->Draw( pDC, pointImageOffset.x, pointImageOffset.y, nImage, false, bDisabled  );

		
		rectDraw.right = 
		rectArrow.left = pointImageOffset.x+(rect.Height()-sizeImage.cy)/2 + sizeImage.cx;
		if( bPressed && !m_bMenu )
			rectArrow.left--;

		

		
//		pDC->FillRect( rectArrow, &CBrush( ::GetSysColor( COLOR_3DFACE ) ) );
	}
	{
		CSize	sizeArrow = CMenuImages::Size();
		CPoint	pointArrow( (rectArrow.CenterPoint().x -sizeArrow.cx/2),
						rectArrow.CenterPoint().y -sizeArrow.cy/2 );

		if( (bPressed && m_bMenu) || (bPressed && bPopupBtn ) )
			pointArrow+=CPoint( 1, 1 );

		CMenuImages::Draw( pDC, (CMenuImages::IMAGES_IDS)CMenuImages::IdArowDown, pointArrow);
	}


	if( bPopupBtn )
	{
		rectDraw = rect;
		rectArrow = NORECT;
	}


	if( m_nDrawStyle == Round )
		pDC->SelectClipRgn( 0 );

	if( m_nDrawStyle == SemiFlat )
	{
		if( bPressed )
		{
			if( m_bMenu  || bPopupBtn )
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			}
			else
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );	
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			}
		}
		else
		{
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
		}
		
		if( bHighlight )
		{
			/*rectArrow.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );*/
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}
	} 
	else if( m_nDrawStyle == Ordinary )
	{
		if( bPressed )
		{
			if( m_bMenu  || bPopupBtn )
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );

				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
				rectDraw.InflateRect( -1, -1 );
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
			}
			else
			{
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );

				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
				rectArrow.InflateRect( -1, -1 );
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
			}
		}
		else
		{
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );

			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			rectArrow.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}

		if( bHighlight )
		{
			/*rectArrow.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );*/
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}
	} 
	else if( m_nDrawStyle == Round )
	{

		
		pDC->SelectStockObject( NULL_BRUSH );
		
		CPen	penLight( PS_SOLID, 0, ::GetSysColor( COLOR_3DHILIGHT ) );
		CPen	penFace( PS_SOLID, 0, ::GetSysColor( COLOR_3DFACE ) );
		CPen	penShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DSHADOW ) );
		CPen	penDkShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DDKSHADOW ) );
		
		bool	bPushed = m_nStyle & TBBS_PRESSED || m_nStyle & TBBS_CHECKED;

		CPoint	point1 = CPoint( rectDraw.right, rectDraw.top );
		CPoint	point2 = CPoint( rectDraw.left, rectDraw.bottom );

		pDC->SelectObject( bPushed?&penDkShadow:&penLight );
		pDC->Arc( rectDraw, point1, point2 );

		pDC->SelectObject( bPushed?&penLight:&penDkShadow );
		pDC->Arc( rectDraw, point2, point1);

		if( bHighlight )
		{
			rectDraw.InflateRect( -1, -1 );
			pDC->SelectObject( bPushed?&penShadow:&penFace );
			pDC->Arc( rectDraw, point1, point2 );

			pDC->SelectObject( bPushed?&penFace:&penShadow );
			pDC->Arc( rectDraw, point2, point1);
		}

		pDC->SelectStockObject( BLACK_PEN );

		if( bPopupBtn )
		{
			if( bPressed || m_bMenu )
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			else
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );	
		}
		else
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );

	}
	else if( m_nDrawStyle == Flat )
	{
		if( bHighlight )
		{
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
		}
		if( bPressed )
		{
			if( m_bMenu  || bPopupBtn )
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			}
			else
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );	
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			}
		}
	}
	


/*
	{
		CRect rcPopupBtn = rectDraw;
		rcPopupBtn.right = rectArrow.right;

		if( bPressed )
		{						
			if( m_bMenu || bPopupBtn )
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			}
			else
			{
				pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );	
				pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
			}
		}
		else
		{
			pDC->Draw3dRect (&rectArrow, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
		}
		
		if( bHighlight )
		{
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}
	} 
	else 	if( m_nDrawStyle == Ordinary )
	{
		if( m_nStyle & TBBS_PRESSED || m_nStyle & TBBS_CHECKED )
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DDKSHADOW ), ::GetSysColor( COLOR_3DHILIGHT ) );
		else
		{
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DHILIGHT ), ::GetSysColor( COLOR_3DDKSHADOW ) );
			rectDraw.InflateRect( -1, -1 );
			pDC->Draw3dRect (&rectDraw, ::GetSysColor( COLOR_3DFACE ), ::GetSysColor( COLOR_3DSHADOW ) );
		}
	} else if( m_nDrawStyle == Round )
	{

		
		pDC->SelectStockObject( NULL_BRUSH );
		
		CPen	penLight( PS_SOLID, 0, ::GetSysColor( COLOR_3DHILIGHT ) );
		CPen	penFace( PS_SOLID, 0, ::GetSysColor( COLOR_3DFACE ) );
		CPen	penShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DSHADOW ) );
		CPen	penDkShadow( PS_SOLID, 0, ::GetSysColor( COLOR_3DDKSHADOW ) );
		
		bool	bPushed = m_nStyle & TBBS_PRESSED || m_nStyle & TBBS_CHECKED;

		CPoint	point1 = CPoint( rectDraw.right, rectDraw.top );
		CPoint	point2 = CPoint( rectDraw.left, rectDraw.bottom );

		pDC->SelectObject( bPushed?&penDkShadow:&penLight );
		pDC->Arc( rectDraw, point1, point2 );

		pDC->SelectObject( bPushed?&penLight:&penDkShadow );
		pDC->Arc( rectDraw, point2, point1);

		if( bHighlight )
		{
			rectDraw.InflateRect( -1, -1 );
			pDC->SelectObject( bPushed?&penShadow:&penFace );
			pDC->Arc( rectDraw, point1, point2 );

			pDC->SelectObject( bPushed?&penFace:&penShadow );
			pDC->Arc( rectDraw, point2, point1);
		}

		pDC->SelectStockObject( BLACK_PEN );
	}*/
}


SIZE CShellMenuButton::OnCalculateSize ( CDC* pDC, const CSize& sizeDefault, BOOL bHorz )
{
	CSize	size = sizeDefault;
	m_nOldWidth = size.cx;
	size.cx +=size.cx/2;

	if( m_ptr_action_ctrl != 0 )
	{
		CSize size = sizeDefault;
		if( S_OK == m_ptr_action_ctrl->GetSize( &size, bHorz ) )
		{
			size.cx +=5; 
			return size;
		}
	}
	else 
	{
		CSize sizeDefault1 = sizeDefault;
		CSize szBase = CBCGToolbarMenuButton::OnCalculateSize(pDC, sizeDefault1, bHorz);
		size.cx = max(size.cx, szBase.cx);
	}

	return size;
}

BOOL CShellMenuButton::OnClick (CWnd* pWnd, BOOL bDelay )
{
	if( m_bMenu && m_ptrMenu != 0 )
	{
		CMenu	menu;
		menu.CreatePopupMenu();

		CRect	rect = Rect();
		pWnd->ClientToScreen( &rect );

		
		m_ptrMenu->AppendMenuItems( menu, ID_CMD_POPUP_MIN  );
		int nCmd = menu.TrackPopupMenu( TPM_LEFTBUTTON|TPM_RETURNCMD, rect.left, rect.bottom, AfxGetMainWnd(), 0 );


		if( nCmd >= ID_CMD_POPUP_MIN )
			m_ptrMenu->ExecuteCommand( nCmd );

		m_bMenu = false;

		return true;
	}

	
	return CShellToolbarButton::OnClick ( pWnd, bDelay );
}
BOOL CShellMenuButton::OnClickUp()
{
	return false;
}
BOOL CShellMenuButton::OnPreClick (CWnd* pWnd )
{
	m_bMenu = false;

	CPoint	point;
	GetCursorPos( &point );
	pWnd->ScreenToClient( &point );

	if( !m_rectLastMenuBtn.PtInRect( point ) )
		return false;

	m_bMenu = true;

	return false;
}

void CShellMenuButton::OnCancelMode ()
{
	CBCGToolbarMenuButton::OnCancelMode ();
}

void CShellMenuButton::Serialize( CArchive &ar )
{
	CShellToolbarButton::Serialize( ar );
	if( ar.IsLoading() )
		_UpdateUI();
}

void CShellMenuButton::OnChangeParentWnd( CWnd* pwnd )
{
	CShellToolbarButton::OnChangeParentWnd( pwnd );

	if( m_ptr_action_ctrl != 0 && pwnd )
	{
		HWND hwnd =  0;
		m_ptr_action_ctrl->GetHwnd( &hwnd );
		if( hwnd )
			::DestroyWindow( hwnd );
		
		m_ptr_action_ctrl->Create( pwnd->GetSafeHwnd(), m_rect );
	}
}

void CShellMenuButton::_UpdateUI()
{
	CShellToolbarButton::_UpdateUI();

		//find the UI component
	CActionInfoWrp	*pi = g_CmdManager.GetActionInfo( m_nID - ID_CMDMAN_BASE );
	if( pi&&CheckInterface( pi->m_pActionInfo, IID_IActionInfo2 ) )
	{
		IUnknown	*punk = 0;
		IActionInfo2Ptr	ptrAI2 = pi->m_pActionInfo;
		ptrAI2->GetUIUnknown( &punk );
		m_ptrMenu = punk;
		m_ptr_action_ctrl = punk;
		if( punk )punk->Release();
	}
}

void CShellMenuButton::OnMove( )
{
	CShellToolbarButton::OnMove( );
	if( m_ptr_action_ctrl )
	{
		HWND hwnd =  0;
		m_ptr_action_ctrl->GetHwnd( &hwnd );
		if( hwnd )
		{
			::MoveWindow( hwnd, 				
				m_rect.left + 5, 
				m_rect.top,
				m_rect.Width() - 5, m_rect.Height(),
				TRUE );
			/*::SetWindowPos( hwnd, NULL, 
				m_rect.left + 5, 
				m_rect.top,
				m_rect.Width() - 5, m_rect.Height(),
				SWP_NOZORDER | SWP_NOACTIVATE);			
			*/
		}

	}
		
}

void CShellMenuButton::OnSize( int iSize )
{
	CShellToolbarButton::OnSize( iSize );
	if( m_ptr_action_ctrl )
	{
		HWND hwnd =  0;
		m_ptr_action_ctrl->GetHwnd( &hwnd );
		if( hwnd )
		{
			::MoveWindow( hwnd, 				
				m_rect.left + 5, 
				m_rect.top,
				m_rect.Width() - 5, m_rect.Height(),
				TRUE );
			/*::SetWindowPos( hwnd, NULL, 
				m_rect.left + 5, 
				m_rect.top,
				m_rect.Width(), m_rect.Height(),
				SWP_NOZORDER | SWP_NOACTIVATE);			
			*/
		}

	}
	
}


void CShellMenuButton::CopyFrom (const CBCGToolbarButton& src)
{
	CShellToolbarButton::CopyFrom( src );
	_UpdateUI();
}

/*
//////////////////////////////////////////////////////////////////////
//
//	Class CShellPopupMenuButon
//
//////////////////////////////////////////////////////////////////////
CShellPopupMenuButon::CShellPopupMenuButon() : CShellToolbarButton()
{		
	//m_bDrawDownArrow = true;	
}

//////////////////////////////////////////////////////////////////////
CShellPopupMenuButon::CShellPopupMenuButon(UINT uiID, int iImage, IUnknown *punkSubMenuProvider, 
									   LPCTSTR lpszText , BOOL bUserButton )
									   :CShellToolbarButton(uiID, iImage, 
														lpszText , bUserButton)
{
	//m_bDrawDownArrow = true;	
	m_ptrSubMenu = punkSubMenuProvider;			

	CreateDefaultMenu();
}

//////////////////////////////////////////////////////////////////////
void CShellPopupMenuButon::Serialize(CArchive& ar)
{
	CShellToolbarButton::Serialize( ar );
	if( ar.IsLoading() )	
		_UpdateUI();		
}

//////////////////////////////////////////////////////////////////////
void CShellPopupMenuButon::_UpdateUI()
{		

	CActionInfoWrp	*pi = g_CmdManager.GetActionInfo( m_nID - ID_CMDMAN_BASE );
	if( pi&&CheckInterface( pi->m_pActionInfo, IID_IActionInfo2 ) )
	{
		IUnknown	*punk = 0;
		IActionInfo2Ptr	ptrAI2 = pi->m_pActionInfo;
		ptrAI2->GetUIUnknown( &punk );
		m_ptrSubMenu = punk;
		CreateDefaultMenu();
		if( punk )punk->Release();
	}

	//m_bDrawDownArrow = true;

}

//////////////////////////////////////////////////////////////////////
bool CShellPopupMenuButon::DestroyMenu()
{
	return true;
}

//////////////////////////////////////////////////////////////////////
bool CShellPopupMenuButon::CreateDefaultMenu()
{
	return InitMenu();
}


//////////////////////////////////////////////////////////////////////
bool CShellPopupMenuButon::InitMenu()
{
	DestroyMenu();

	CMenu menu;
	menu.CreateMenu();
	menu.AppendMenu( MF_STRING|MF_ENABLED, ID_CMD_POPUP_MIN, "AA" );
	menu.AppendMenu( MF_STRING|MF_ENABLED, ID_CMD_POPUP_MIN+1, "ABB" );
	menu.AppendMenu( MF_STRING|MF_ENABLED, ID_CMD_POPUP_MIN+2, "AA" );	
	CreateFromMenu( menu.m_hMenu );

	return true;
}

//////////////////////////////////////////////////////////////////////
BOOL CShellPopupMenuButon::OnClick (CWnd* pWnd, BOOL bDelay )
{
	InitMenu();
	BOOL bRes = CShellToolbarButton::OnClick ( pWnd, bDelay );

	CBCGPopupMenu* pMenu = GetPopupMenu();
	if( pMenu )
	{
		CWnd* p = pMenu->GetMessageWnd();//m_pMessageWnd
	}
	return bRes;
}


//////////////////////////////////////////////////////////////////////
BOOL CShellPopupMenuButon::NotifyCommand( int nID )
{
	return TRUE;
}
*/

//////////////////////////////////////////////////////////////////////
//
//	Class CShellMenuComboBoxButton
//
//////////////////////////////////////////////////////////////////////

CShellMenuComboBoxButton::CShellMenuComboBoxButton():CBCGToolbarComboBoxButton()
{	
	m_ptrCombo = NULL;
	m_bWasCreateCombo = false;
}

/////////////////////////////////////////////////////////////////////////////
CShellMenuComboBoxButton::CShellMenuComboBoxButton(UINT uiID, int iImage, 
							IUnknown *punkMenuProvider, 
							DWORD dwStyle, 
							int iWidth)
							:CBCGToolbarComboBoxButton( uiID, iImage, dwStyle, iWidth)
{
	
	m_ptrCombo = punkMenuProvider;	
	m_bWasCreateCombo = false;
}

//////////////////////////////////////////////////////////////////////
CShellMenuComboBoxButton::~CShellMenuComboBoxButton()
{
	DeInit();
}

//////////////////////////////////////////////////////////////////////
void CShellMenuComboBoxButton::CopyFrom (const CBCGToolbarButton& src)
{
	CBCGToolbarComboBoxButton::CopyFrom(src);
	_UpdateUI();
}

//////////////////////////////////////////////////////////////////////
void CShellMenuComboBoxButton::Serialize(CArchive& ar)
{
	CBCGToolbarComboBoxButton::Serialize( ar );
	if( ar.IsLoading() )
		_UpdateUI();
	
}

//////////////////////////////////////////////////////////////////////
void CShellMenuComboBoxButton::_UpdateUI()
{
	//find the UI component
	CActionInfoWrp	*pi = g_CmdManager.GetActionInfo( m_nID - ID_CMDMAN_BASE );
	if( pi&&CheckInterface( pi->m_pActionInfo, IID_IActionInfo2 ) )
	{
		IUnknown	*punk = 0;
		IActionInfo2Ptr	ptrAI2 = pi->m_pActionInfo;
		ptrAI2->GetUIUnknown( &punk );
		if( m_ptrCombo != NULL )
		{
			m_ptrCombo = NULL;
		}
		m_ptrCombo = punk;

		if( punk )punk->Release();
	}
}

//////////////////////////////////////////////////////////////////////
void CShellMenuComboBoxButton::Init( CComboBox* pCombo )
{
	if( m_ptrCombo )
	{		
		HWND hWnd = NULL;
		if( pCombo )
		{
			hWnd = pCombo->GetSafeHwnd( );
			if( ::IsWindow( hWnd ) )
			{
				//Put to list
				CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
				if( pMainFrame )
				{
					CActionInfoWrp	*pi = g_CmdManager.GetActionInfo( m_nID - ID_CMDMAN_BASE );
					IUnknown* punActionInfo = NULL;
					if( pi->m_pActionInfo )
					{
						punActionInfo = pi->m_pActionInfo;
					}
					
					pMainFrame->RegisterComboButton( hWnd, punActionInfo );
				}
				m_ptrCombo->OnInitList( hWnd );
			}
		}
	}						   

}

//////////////////////////////////////////////////////////////////////
void CShellMenuComboBoxButton::DeInit()
{
	if( m_bWasCreateCombo && (m_ptrCombo != NULL ) )
	{
		//Remove from list
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetApp()->GetMainWnd();
		CComboBox* pCombo = GetComboBox();
		if( pMainFrame && pCombo )
		{
			pMainFrame->UnRegisterComboButton( pCombo->GetSafeHwnd() );
		}
		m_ptrCombo->OnDetachWindow( );	
	}
}


//////////////////////////////////////////////////////////////////////
CComboBox* CShellMenuComboBoxButton::CreateCombo (CWnd* pWndParent, const CRect& rect)
{
	CComboBox* pCombo = CBCGToolbarComboBoxButton::CreateCombo( pWndParent, rect );
	
	DeInit();
	m_bWasCreateCombo = true;
	Init( pCombo );	
	
	return pCombo;
}


//////////////////////////////////////////////////////////////////////
BOOL CShellMenuComboBoxButton::NotifyCommand (int iNotifyCode)
{
	BOOL bResult = CBCGToolbarComboBoxButton::NotifyCommand( iNotifyCode );
	if( iNotifyCode == CBN_SELENDOK )
	{
		if( m_ptrCombo != NULL )
			m_ptrCombo->OnSelChange( );
	}

	return bResult;
}



//////////////////////////////////////////////////////////////////////
//CBCGToolbarComboBoxButton