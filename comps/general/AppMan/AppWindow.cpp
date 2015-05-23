// AppWindow.cpp : implementation file
//

#include "stdafx.h"
#include "AppMan.h"
#include "AppWindow.h"
#include "AppManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAppWindow

IMPLEMENT_DYNCREATE(CAppWindow, CGfxOutBarCtrl)

CAppWindow::CAppWindow()
{
	m_dwDockSide = AFX_IDW_DOCKBAR_LEFT;
	iSelFolder = 0;
	iLastSel = 0;
	EnableAutomation();
	
	Register( 0 );


	m_sName = "AppWindow"; 
	m_sUserName.LoadString(IDS_APP_TITLE);
	
	_OleLockApp( this );
}

CAppWindow::~CAppWindow()
{
	UnRegister( 0 );
	
	_OleUnlockApp( this );
}

void CAppWindow::PostNcDestroy()
{
	
}

void CAppWindow::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.
	if( GetSafeHwnd() )DestroyWindow();
	delete this;
}


BEGIN_MESSAGE_MAP(CAppWindow, CGfxOutBarCtrl)
	//{{AFX_MSG_MAP(CAppWindow)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_OUTBAR_NOTIFY, OnOutbarNotify)
	ON_MESSAGE(WM_GETINTERFACE, OnGetInterface)
END_MESSAGE_MAP()


BEGIN_DISPATCH_MAP(CAppWindow, CGfxOutBarCtrl)
	//{{AFX_DISPATCH_MAP(CAppWindow)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IAppWindow to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {E790124D-3A7D-11D3-A604-0090275995FE}
static const IID IID_IAppWindow =
{ 0xe790124d, 0x3a7d, 0x11d3, { 0xa6, 0x4, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe } };

BEGIN_INTERFACE_MAP(CAppWindow, CGfxOutBarCtrl)
	INTERFACE_PART(CAppWindow, IID_IAppWindow, Dispatch)
	INTERFACE_PART(CAppWindow, IID_IWindow, Wnd)
	INTERFACE_PART(CAppWindow, IID_IDockableWindow, Dock)
	INTERFACE_PART(CAppWindow, IID_IEventListener, EvList)
	INTERFACE_PART(CAppWindow, IID_INamedObject2, Name)
	INTERFACE_PART(CAppWindow, IID_IHelpInfo, Help)
END_INTERFACE_MAP()

// {FD981A72-3ACE-11d3-A604-0090275995FE}
GUARD_IMPLEMENT_OLECREATE(CAppWindow, "AppMan.AppWindow",
0xfd981a72, 0x3ace, 0x11d3, 0xa6, 0x4, 0x0, 0x90, 0x27, 0x59, 0x95, 0xfe);

/////////////////////////////////////////////////////////////////////////////
// CAppWindow message handlers

int CAppWindow::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CGfxOutBarCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_listSmall.Create( 16, 16,	0, 10, 10 );
	//m_listLarge.Create( 32, 32,	0, 10, 10 );

	m_listSmall.Create(IDB_SMALL_IMAGELIST, 16, 0, RGB(0,128,128));
	m_listLarge.Create(IDB_IMAGELIST, 32, 0, RGB(128,128,128));

	SetImageList( &m_listSmall, CGfxOutBarCtrl::fSmallIcon );
	SetImageList( &m_listLarge, CGfxOutBarCtrl::fLargeIcon );
	
/*	AddFolder( "Master", 0 );
	AddFolder( "Auto", 1 );
	AddFolder( "Size", 2 );
	AddFolder( "Image Database", 3 );*/

	int	nAppsCounter;
	int nMethCounter;
	
	for( nAppsCounter = 0; nAppsCounter < CAppManager::s_pAppManagerInstance->GetAppsCount(); nAppsCounter++ )
	{
		CAppManager::XApplication *papp = CAppManager::s_pAppManagerInstance->GetApp( nAppsCounter );

		AddFolder( papp->GetTitle(), nAppsCounter );

		for( nMethCounter = 0; nMethCounter < papp->GetMethodicsCount(); nMethCounter++ )
		{
			CAppManager::XMethodic *pmeth = papp->GetMethodic( nMethCounter );
			InsertItem( nAppsCounter, nMethCounter, pmeth->GetTitle(), /*image*/1, /*dwExData*/0 );
		}
	}

	SetSelFolder( CAppManager::s_pAppManagerInstance->m_nActiveApp );
	SetSelItem( CAppManager::s_pAppManagerInstance->m_nActiveMethodic );

	/*Master*/
//	InsertItem( /*iFolder*/0, /*iIndex*/0, /*text*/"Count and size", /*image*/1, /*dwExData*/0 );
//	InsertItem( /*iFolder*/0, /*iIndex*/1, /*text*/"Motility", /*image*/1, /*dwExData*/0 );
//	InsertItem( /*iFolder*/0, /*iIndex*/2, /*text*/"No methodic", /*image*/1, /*dwExData*/0 );
//	InsertItem( /*iFolder*/0, /*iIndex*/3, /*text*/"Structure", /*image*/1, /*dwExData*/0 );
//	InsertItem( /*iFolder*/0, /*iIndex*/4, /*text*/"Phase Analisys", /*image*/1, /*dwExData*/0 );
//	InsertItem( /*iFolder*/0, /*iIndex*/4, /*text*/"Measure", /*image*/1, /*dwExData*/0 );
	
	/*Auto*/
//	InsertItem( /*iFolder*/1, /*iIndex*/0, /*text*/"Count and size", /*image*/2, /*dwExData*/0 );
//	InsertItem( /*iFolder*/1, /*iIndex*/1, /*text*/"Motility", /*image*/2, /*dwExData*/0 );
//	InsertItem( /*iFolder*/1, /*iIndex*/2, /*text*/"No methodic", /*image*/2, /*dwExData*/0 );

	/*size*/
//	InsertItem( /*iFolder*/2, /*iIndex*/0, /*text*/"Count and size", /*image*/3, /*dwExData*/0 );
//	InsertItem( /*iFolder*/2, /*iIndex*/1, /*text*/"No methodic", /*image*/3, /*dwExData*/0 );
	/*database*/
//	InsertItem( /*iFolder*/3, /*iIndex*/1, /*text*/"No methodic", /*image*/3, /*dwExData*/0 );

	return 0;
}

//use own create methodics
bool CAppWindow::DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID )
{
	DWORD dwf = CGfxOutBarCtrl::fDragItems|CGfxOutBarCtrl::fEditGroups|CGfxOutBarCtrl::fEditItems|CGfxOutBarCtrl::fRemoveGroups|
				CGfxOutBarCtrl::fRemoveItems|CGfxOutBarCtrl::fAddGroups|CGfxOutBarCtrl::fAnimation
				|CGfxOutBarCtrl::fSelHighlight;


	if( !Create( dwStyle, rc, pparent, nID, dwf ) )
		return false;
	SetOwner( this );

	SetWindowText("Application Manager");
	return true;


}

LRESULT CAppWindow::OnOutbarNotify(WPARAM wParam, LPARAM lParam)
{
	switch (wParam)
	{
		case NM_FOLDERCHANGE:
		{
			int index = (int) lParam;
			CAppManager::s_pAppManagerInstance->SetCurrentApplication( index );

			return 0;
		}
		case NM_OB_ITEMCLICK:
		// cast the lParam to an integer to get the clicked item
			{
				int index = (int) lParam;
				CAppManager::s_pAppManagerInstance->SetCurrentMethodic( index );
			}
		return 0;

		case NM_OB_ONLABELENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited item
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				CAppManager::s_pAppManagerInstance->RenameMethodic( pOI->index, pOI->cText );

				TRACE2("Editing item %d, new text:%s\n", pOI->index, pOI->cText);
			}
		return 1;

		case NM_OB_ONGROUPENDEDIT:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the edited folder
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				CAppManager::s_pAppManagerInstance->RenameApplication( pOI->index, pOI->cText );
			}
		return 1;

		case NM_OB_DRAGITEM:
		// cast the lParam to an OUTBAR_INFO * struct; it will contain info about the dragged items
		// return 1 to do the change and 0 to cancel it
			{
				OUTBAR_INFO * pOI = (OUTBAR_INFO *) lParam;
				CAppManager::s_pAppManagerInstance->MoveMethodic( pOI->iDragFrom, pOI->iDragTo );
			}
		return 1;
	}
	return 0;
}

void CAppWindow::OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize )
{
	if( !strcmp( szEventActiveAppChange, pszEvent ) )
	{
		long	nNewApplication = *(long*)pdata;

		if( nNewApplication == GetFolderCount() )
		{
			CAppManager::XApplication *papp = CAppManager::s_pAppManagerInstance->GetApp( nNewApplication );
			AddFolder( papp->GetTitle(), nNewApplication );
		}

		if( GetSelFolder() != nNewApplication )
			SetSelFolder( nNewApplication );
	}
	else
	if( !strcmp( szEventActiveMethodicChange, pszEvent ) )
	{
		long	nNewMeth = *(long*)pdata;

		if( nNewMeth == GetItemCount() )
		{
			long	nApp = GetSelFolder();
			CAppManager::XApplication *papp = CAppManager::s_pAppManagerInstance->GetApp( nApp );
			CAppManager::XMethodic *pmeth = papp->GetMethodic( nNewMeth );
			InsertItem( nApp, nNewMeth, pmeth->GetTitle(), /*image*/1, /*dwExData*/0 );
		}
		//if( GetSelItem() != nNewMeth )
		//	SetSelItem( nNewMeth );
	}
	else
	if( !strcmp( szEventAfterOpen, pszEvent ) )
	{
		sptrIDocumentSite	sptrDS( punkFrom );

		IUnknown *punkT = 0;
		sptrDS->GetDocumentTemplate( &punkT );
		ASSERT( punkT );
		sptrIDocTemplate	sptrT( punkT );
		punkT->Release();

		BSTR	bstrTemplateName;
		sptrT->GetDocTemplString( CDocTemplate::docName, &bstrTemplateName );
		CString	strTemplate = bstrTemplateName;
		::SysFreeString( bstrTemplateName );

		if( strTemplate == szDocumentScript )
		{
			//get the oppened methodic filename
			_bstr_t	strFileName = (char*)pdata;

			long	nAppPos = GetSelFolder();
			long	nMethPos = GetSelItem();
			CAppManager::s_pAppManagerInstance->LocateMethodicByFileName( (const char*)strFileName, nAppPos, nMethPos );
			if( nAppPos != GetSelFolder() )
				SetSelFolder( nAppPos );
			if( nMethPos != GetSelItem() )
				SetSelItem( nMethPos );
		}
		
//the file was opened
	}
	if( !strcmp( szEventActiveMethodicChange, pszEvent ) )
	{
		long nRenamedMethodicPos = *(long*)pdata;
		CString	str = CAppManager::s_pAppManagerInstance->GetMethodicName( 
			CAppManager::s_pAppManagerInstance->m_nActiveApp, nRenamedMethodicPos );
		SetItemText( nRenamedMethodicPos, str );
	}
	else
	if( !strcmp( szEventRenameApp, pszEvent ) )
	{
		long nRenamedAppPos = *(long*)pdata;
		CString	str = CAppManager::s_pAppManagerInstance->GetAppName( nRenamedAppPos );
		SetFolderText( nRenamedAppPos, str );
	}
	else
	if( !strcmp( szBeforeAppTerminate, pszEvent ) )
	{
//		UnRegister( 0 );
	}
}

long CAppWindow::GetSelItem()
{
	return iLastSel;
}

void CAppWindow::SetSelItem( long nPos )
{
	CRect	rc;

	if( iLastSel != -1 )
	{
		GetItemRect(iSelFolder, iLastSel, rc);
		rc.InflateRect( 3, 3 );
		InvalidateRect( rc );
	}
	iLastSel = nPos;

	iLastSel = min( GetItemCount()-1, iLastSel );

	
	if( iLastSel != -1 )
	{
		GetItemRect(iSelFolder, iLastSel, rc);
		rc.InflateRect( 3, 3 );
		InvalidateRect( rc );
	}
}
