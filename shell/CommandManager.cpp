// CommandManager.cpp : implementation file
//

#include "stdafx.h"
#include "shell.h"

#include "CommandManager.h"
#include "MainFrm.h"
#include "docs.h"
#include "misc.h"
#include "Utils.h"
#include "ShellDockBar.h"
#include "ShellButton.h"
#include "shelltoolbar.h"
#include "ActionControlWrp.h"

#include "MenuRegistrator.h"

#include "ShellFrame.h"

//#include "\vt5\awin\trace.h"

extern int iMaxUserToolbars;
extern UINT uiFirstUserToolBarId;
extern UINT uiLastUserToolBarId;


//button helper
class CMyTBMenuBtn : public CBCGToolbarMenuButton
{
public:
	CObList	&GetCommand(){return m_listCommands;}
};

//ToolBarData helper
CCommandManager::ToolBarData::ToolBarData( const char *sz )
{
	m_sGroupName = sz;
}

CCommandManager::ToolBarData::~ToolBarData()
{
}

void CCommandManager::ToolBarData::AddData( UINT nCmd, UINT nImageNo, UINT nBmpNo, const char *sz )
{
	m_arrIDs.Add( nCmd );
	m_arrPicts.Add( nImageNo );
	m_arrBmps.Add( nBmpNo );
	m_arrStrs.Add( sz );
}

long CCommandManager::ToolBarData::GetSize()
{
	ASSERT( m_arrIDs.GetSize() == m_arrPicts.GetSize() );
	ASSERT( m_arrIDs.GetSize() == m_arrBmps.GetSize() );

	return (long)m_arrIDs.GetSize();
}



/////////////////////////////////////////////////////////////////////////////
// CCommandManager
CCommandManager	g_CmdManager;

IMPLEMENT_DYNCREATE(CCommandManager, CCmdTargetEx)

CCommandManager::CCommandManager()
{
	EnableAutomation();

	SetName( _T("ToolBars") );

	m_hAccel = 0;
	m_uiAccelEntries = 0;

	m_bStateReady = false;
	m_loActionState = loaComplete; 

}

CCommandManager::~CCommandManager()
{
	DestroyTemplateInfoList();
	DeInit();	
}


void CCommandManager::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CCommandManager, CCmdTargetEx)
	//{{AFX_MSG_MAP(CCommandManager)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_CMDMAN_BASE, ID_CMDMAN_MAX, OnCommandAction)
	//ON_COMMAND_EX_RANGE(ID_CMDMAN_BASE, ID_CMDMAN_MAX, OnCommandActionEx)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CMDMAN_BASE, ID_CMDMAN_MAX, OnUpdateCommandAction)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CLASSMENU_BASE, ID_CLASSMENU_MAX, OnUpdateCommandClassMenu)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CMD_POPUP_MIN, ID_CMD_POPUP_MAX, OnUpdateCommandPopupMenu)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CCommandManager, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CCommandManager)
	DISP_FUNCTION(CCommandManager, "GetBarsCount", GetBarsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CCommandManager, "GetBarName", GetBarName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CCommandManager, "CreateNewBar", CreateNewBar, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "MakeEmpty", MakeEmpty, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CCommandManager, "FindNo", FindNo, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "AddButton", AddButton, VT_EMPTY, VTS_I4 VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "ShowBar", ShowBar, VT_EMPTY, VTS_I4 VTS_I4)
	DISP_FUNCTION(CCommandManager, "InsertButton", InsertButton, VT_EMPTY, VTS_I4 VTS_BSTR VTS_I4)
	DISP_FUNCTION(CCommandManager, "DeleteBar", DeleteBar, VT_EMPTY, VTS_I4)
	DISP_FUNCTION(CCommandManager, "LoadState", LoadState, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "SaveState", SaveState, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "AddOutlookBar", AddOutlookBar, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "IsBarVisible", IsBarVisible, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CCommandManager, "GetButtonCount", GetButtonCount, VT_I4, VTS_I4)
	DISP_FUNCTION(CCommandManager, "GetButton", GetButton, VT_BSTR, VTS_I4 VTS_I4)
	DISP_FUNCTION(CCommandManager, "IsOutlookToolBar", IsOutlookToolBar, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CCommandManager, "GetDockBarsCount", GetDockBarsCount, VT_I4, VTS_NONE)
	DISP_FUNCTION(CCommandManager, "GetDockBarName", GetDockBarName, VT_BSTR, VTS_I4)
	DISP_FUNCTION(CCommandManager, "IsDockBarVisible", IsDockBarVisible, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CCommandManager, "ShowDockBar", ShowDockBar, VT_EMPTY, VTS_I4 VTS_I2)
	DISP_FUNCTION(CCommandManager, "FindDockNo", FindDockNo, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "SetCanCloseDockBar", SetCanCloseDockBar, VT_EMPTY, VTS_I4 VTS_BOOL)
	DISP_FUNCTION(CCommandManager, "CanCloseDockBar", CanCloseDockBar, VT_BOOL, VTS_I4)
	DISP_FUNCTION(CCommandManager, "GetDockBarImplByName", GetDockBarImplByName, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CCommandManager, "SaveAsState", SaveAsState, VT_EMPTY, VTS_BSTR)
    DISP_FUNCTION(CCommandManager, "HasAction", HasAction, VT_BOOL, VTS_I4 VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_ICommandManager to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {F49AE623-0446-11D3-A5A3-0000B493FF1B}
static const IID IID_ICommandManagerDispatch =
{ 0xf49ae623, 0x446, 0x11d3, { 0xa5, 0xa3, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

BEGIN_INTERFACE_MAP(CCommandManager, CCmdTargetEx)
	INTERFACE_PART(CCommandManager, IID_ICommandManagerDispatch, Dispatch)
	INTERFACE_PART(CCommandManager, IID_INamedObject2, Name)
	INTERFACE_PART(CCommandManager, IID_ICommandManager, Man2)
	INTERFACE_PART(CCommandManager, IID_ICommandManager2, Man2)
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CCommandManager, Man2);

// derived interface
// execute context menu with customizing it by MenuCreator (if it's registered)

HRESULT CCommandManager::XMan2::ExecuteContextMenu3( BSTR bstrTargetName, POINT point, DWORD dwFlag, UINT *puCmd )
{
	_try_nested(CCommandManager, Man2, ExecuteContextMenu2)
	{
		if( CBCGToolBar::IsCustomizeMode() )return S_FALSE;
		UINT uCmd = 0;
		
		CString	strMenuName = bstrTargetName;
		// menu helper not registered
		if (pThis->m_sptrMenuHelper == NULL)
			uCmd = pThis->ExecuteContextMenu(strMenuName, &CPoint(point), 0, dwFlag);
		else
			uCmd = pThis->ExecuteContextMenuWithCreator(strMenuName, &CPoint(point), 0, dwFlag);

		if (puCmd)
			*puCmd = uCmd;

		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommandManager::XMan2::ExecuteContextMenu2( BSTR bstrTargetName, POINT point, DWORD dwFlag )
{
	_try_nested(CCommandManager, Man2, ExecuteContextMenu2)
	{
		if( CBCGToolBar::IsCustomizeMode() )return S_FALSE;

		CString	strMenuName = bstrTargetName;
		// menu helper not registered
		if (pThis->m_sptrMenuHelper == NULL)
			pThis->ExecuteContextMenu(strMenuName, &CPoint(point), 0, dwFlag);
		else
			pThis->ExecuteContextMenuWithCreator(strMenuName, &CPoint(point), 0, dwFlag);

		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommandManager::XMan2::ExecuteContextMenu( BSTR bstrTargetName, POINT point )
{
	_try_nested(CCommandManager, Man2, ExecuteContextMenu)
	{
		if( CBCGToolBar::IsCustomizeMode() )return S_FALSE;
		CString	strMenuName = bstrTargetName;
		// menu helper not registered
		if (pThis->m_sptrMenuHelper == NULL)
			pThis->ExecuteContextMenu(strMenuName, &CPoint(point), 0, 0);
		else
			pThis->ExecuteContextMenuWithCreator(strMenuName, &CPoint(point), 0, 0);

		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommandManager::XMan2::AddAction( IUnknown *punkActionInfo )
{
	_try_nested(CCommandManager, Man2, AddAction)
	{
		// [vanek] : catch action - 13.12.2004
		pThis->m_advbmps.CatchActionInfo( punkActionInfo );

		CActionInfoWrp	*pActionInfo = new CActionInfoWrp( punkActionInfo );

		pActionInfo->Init();
		pThis->AddActionInfo( pActionInfo );

		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommandManager::XMan2::RemoveAction( IUnknown *punkActionInfo )
{
	_try_nested(CCommandManager, Man2, RemoveAction)
	{				   
		/*INumeredObjectPtr	sptrN( punkActionInfo );

		GuidKey	idx;
		sptrN->GetKey( idx );*/
		IActionInfoPtr	ptrA( punkActionInfo );
		int	idx = -1;
		ptrA->GetLocalID( &idx );
		pThis->RemoveActionInfo( idx );

		return S_OK;
	}
	_catch_nested;
}

		
HRESULT CCommandManager::XMan2::RegisterMenuCreator( IUnknown *punk )
{
	_try_nested(CCommandManager, Man2, RegisterMenuCreator)
	{	
		// check argument to validity
		if (!CheckInterface(punk, IID_IMenuInitializer))
			return E_INVALIDARG;

		// check MenuHelper is empty
		if (pThis->m_sptrMenuHelper != NULL)
			return E_FAIL;

		pThis->m_sptrMenuHelper = punk;
		pThis->m_sptrMenuHelper2 = pThis->m_sptrMenuHelper;
//		punk->Release(); //?????

		return S_OK;
	}
	_catch_nested;
}

HRESULT CCommandManager::XMan2::UnRegisterMenuCreator( IUnknown *punk )
{
	_try_nested(CCommandManager, Man2, UnRegisterMenuCreator)
	{
		// check pointer to validity
		if (!CheckInterface(punk, IID_IMenuInitializer))
			return E_INVALIDARG;

		sptrIMenuInitializer sptr(punk);
//		punk->Release(); //???????????

		// check to equals
		if (pThis->m_sptrMenuHelper != sptr)
			return E_FAIL;
		
		if (pThis->m_sptrMenuHelper != 0)
			pThis->m_sptrMenuHelper.Release();
		if (pThis->m_sptrMenuHelper2 != 0)
			pThis->m_sptrMenuHelper2.Release();
		return S_OK;
	}
	_catch_nested;
}

// get action name by action ID
HRESULT CCommandManager::XMan2::GetActionName( BSTR *pbstrActionName, int nID )
{
	_try_nested(CCommandManager, Man2, GetActionName)
	{	
		// get index in actions array
		int idx = nID - ID_CMDMAN_BASE;
		// check this index to validity
		if (idx < 0 || idx >= pThis->m_ptrsInfo.GetSize())
			return E_INVALIDARG;

		// get action info
		CActionInfoWrp	*pinfo = (CActionInfoWrp*)pThis->m_ptrsInfo[idx];
		if (!pinfo)
			return E_FAIL;
		// get action short name 
		CString	strName = pinfo->GetActionShortName();
		// and delete action info
//		delete pinfo;
		// set this name to param
		*pbstrActionName = strName.AllocSysString();

		return S_OK;
	}
	_catch_nested;
}

// get action nID by action name
HRESULT CCommandManager::XMan2::GetActionID( BSTR bstrActionName, int *pnID )
{
	_try_nested(CCommandManager, Man2, GetActionID)
	{	
		// check action name on validity
		CString	strActionName = bstrActionName;
		if (strActionName.IsEmpty())
			return E_INVALIDARG;

		// preset
		*pnID = -1;
		
		// try to find action by name
		for (int i = 0; i < pThis->m_ptrsInfo.GetSize(); i++)
		{
			// get action info
			CActionInfoWrp	*pi = (CActionInfoWrp*)pThis->m_ptrsInfo[i];
			if (!pi)
				continue;
			// get short name 
			CString str = pi->GetActionShortName();
			// if this name is finded
			if (str == strActionName)
			{	// set nID and return succeess
				*pnID = i + ID_CMDMAN_BASE;
				return S_OK;
			}
		}// continue
		return E_INVALIDARG;
	}
	_catch_nested;
}

/*
TPM_LEFTALIGN 
TPM_CENTERALIGN
TPM_RIGHTALIGN
TPM_BOTTOMALIGN
TPM_VCENTERALIGN
TPM_TOPALIGN
*/

UINT CCommandManager::ExecuteContextMenu( const char *szMenuName, CPoint *ppt, CWnd *pwnd, DWORD dwFlag )
{
	CPoint	point;

	if( ppt )
		point =  *ppt;
	else
		::GetCursorPos( &point );

	if( !pwnd )
		pwnd = ::AfxGetMainWnd();

	CWnd	*pwnd1 = AfxGetMainWnd();

	ASSERT_KINDOF(CMainFrame, pwnd1);
	CMainFrame *pmain = (CMainFrame *)pwnd1;

	MenuData *pdata = GetMenuData( szMenuName );
	
	//paul 17.03.2003
	pmain->KillActivePopup();

	if( !pdata )
		return 0;

	HMENU	hMenu = pmain->m_ContextMenuManager.GetMenuByName( pdata->strMenuName );
	if( !hMenu )return 0;
	
	pmain->m_ContextMenuManager.ShowPopupMenu( ::GetSubMenu( hMenu, 0 ), point.x, point.y, pwnd, dwFlag );
	return 1;
}

__declspec( dllimport )
DWORD g_nLastCommandID;
__declspec( dllimport )
bool g_bWaitingForCommandFlag;

// extended execute context menu
// this function is use IMenuInitializer to customize menu
UINT CCommandManager::ExecuteContextMenuWithCreator( const char *szMenuName, CPoint *ppt, CWnd *pwnd, DWORD dwFlag )
{
//	sptrIMenuInitializer m_sptrMenuHelper;

	ASSERT(m_sptrMenuHelper != NULL);
	CPoint	point;

	if (ppt)
		point =  *ppt;
	else
		::GetCursorPos(&point);

	if (!pwnd) 
		pwnd = ::AfxGetMainWnd();

	CWnd	*pwnd1 = AfxGetMainWnd();

	ASSERT_KINDOF(CMainFrame, pwnd1);
	CMainFrame *pmain = (CMainFrame *)pwnd1;

	//paul 17.03.2003
	pmain->KillActivePopup();

	MenuData *pdata = GetMenuData(szMenuName);

	if (!pdata)
		return 0;

	HMENU hDstMenu = NULL, hSrcMenu = pmain->m_ContextMenuManager.ExportMenu(pdata->idx);

	bool bCustomized = true;
	if (hSrcMenu)
	{
		_bstr_t bstrMenuName(pdata->strMenuName);
		if (FAILED(m_sptrMenuHelper->OnInitPopup(bstrMenuName, hSrcMenu, &hDstMenu)))
			bCustomized = false;		
	}

	
	if (bCustomized)
	{
		CBCGPopupMenu	*pCreatedPopup = pmain->m_ContextMenuManager.ShowPopupMenu((hDstMenu) ? hDstMenu : hSrcMenu, point.x, point.y, pwnd, dwFlag);

		if( pCreatedPopup )
		{
			if( !( dwFlag & TPM_RETURNCMD ) )
				return 0;//bReturn;

			g_nLastCommandID = (DWORD)-1;
			g_bWaitingForCommandFlag = true;
			// for tracking the idle time state
			BOOL bIdle = TRUE;
			LONG lIdleCount = 0;


			IApplication	*pApp = GetAppUnknown();
			BOOL	bOld = FALSE;
			pApp->SetNestedMessageLoop( TRUE, &bOld );
			// acquire and dispatch messages until a WM_QUIT message is received.

			MSG *msgCur = 0;

#if _MSC_VER >= 1300
			msgCur = AfxGetCurrentMessage();
#else
			msgCur = &AfxGetApp()->m_msgCur;
#endif
			for (;;)
			{
				// phase1: check to see if we can do idle work
				while (bIdle &&
					!::PeekMessage(msgCur, NULL, NULL, NULL, PM_NOREMOVE))
				{
					// call OnIdle while in bIdle state
					if (!AfxGetApp()->OnIdle(lIdleCount++))
						bIdle = FALSE; // assume "no idle" state
				}

				// phase2: pump messages while available
				do
				{
					if (!::GetMessage(msgCur, NULL, NULL, NULL))
					{
						pApp->SetNestedMessageLoop( bOld, 0 );
						return 0;
					}

					if (msgCur->message != WM_KICKIDLE && !AfxGetApp()->PreTranslateMessage(msgCur))
					{
						::TranslateMessage(msgCur);
						::DispatchMessage(msgCur);
					}

					// reset "no idle" state after pumping "normal" message
					if (AfxGetApp()->IsIdleMessage(msgCur)) 
					{
						bIdle = TRUE;
						lIdleCount = 0;
					}


					if( !IsWindow( ((CMainFrame*)AfxGetMainWnd())->GetActivePopup()->GetSafeHwnd() ) )
					{
						g_bWaitingForCommandFlag = false;
						pApp->SetNestedMessageLoop( bOld, 0 );
						return g_nLastCommandID;
					}

				} while (::PeekMessage(msgCur, NULL, NULL, NULL, PM_NOREMOVE));
			}
			pApp->SetNestedMessageLoop( bOld, 0 );
		}
	}
	
//	TrackPopupMenu(hDstMenu, dwFlag, point.x, point.y, 0, AfxGetMainWnd()->GetSafeHwnd(), NULL);

	if (hDstMenu && hDstMenu != hSrcMenu)
		::DestroyMenu(hDstMenu);

	::DestroyMenu(hSrcMenu);

	g_bWaitingForCommandFlag = false;
	g_nLastCommandID = (DWORD)-1;
	return -1;
}

	
/////////////////////////////////////////////////////////////////////////////
// CCommandManager message handlers
void CCommandManager::Init()
{
	_try(CCommandManager, Init);
	{
		COLORREF	colorT = /*RGB( 192, 192, 192 );*/GetSysColor( COLOR_3DFACE );

		// [vanek] : loading new icons for actions - 13.12.2004
		m_advbmps.SetBackColor( colorT );
		m_advbmps.Load();

		m_ptrAliases = GetAppUnknown();
		
		m_uiAccelEntries = 0;


		if( m_ptrAliases != 0 )
		{
			_bstr_t	bstrAliaseFileName = ::MakeAppPathName( "shell.aliases" );
			m_ptrAliases->LoadFile( bstrAliaseFileName );
		}

	
		InitUserImages();

		// vanek : drawing disabled icons - 27.02.2005
		CBCGToolBarImages::SetImageDrawerSettings( 
			0 != GetValueInt( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsEnable, 0 ),
			GetValueDouble( GetAppUnknown(), szDrawDisImgsSect, szDrawDisImgsAlpha, 0.5 ) );
		
		CBCGToolBar::SetUserImages( &m_UserImages );
	    m_UserImages.SetTransparentColor( colorT );


		//add menus
		POSITION posTempl = theApp.m_pDocManager->GetFirstDocTemplatePosition();

		while( posTempl )
		{
			CDocTemplate	*ptempl = theApp.m_pDocManager->GetNextDocTemplate( posTempl );
			CString	strMenuTitle;

			ptempl->GetDocString( strMenuTitle, CDocTemplate::docName );

			CMenu	menu;
			menu.CreateMenu();

			HMENU	hMenu = menu.Detach();

			MenuData	*pdata = new MenuData;
			pdata->bDocumentMenu = true;
			pdata->hMenu = hMenu;
			pdata->bPopupMenu = false;
			pdata->strMenuName = strMenuTitle;
			pdata->idx = (int)m_ptrsMenuInfo.GetSize();

			m_ptrsMenuInfo.Add( pdata );

			ASSERT_KINDOF(CMultiDocTemplate, ptempl);

			((CMultiDocTemplate*)ptempl)->m_hMenuShared = hMenu;
		}


		//Add popup menu support 
		
		{
			CMenuRegistrator mr;
			long lMenuCount = 0;
			mr.GetMenuSectionContent( &lMenuCount, 0, 0 );

			if( lMenuCount > 0  )
			{
				_bstr_t* pbstrName = new _bstr_t[lMenuCount];
				_bstr_t* pbstrUserNames = new _bstr_t[lMenuCount];//can be for exaple rus name

				mr.GetMenuSectionContent( &lMenuCount, pbstrName, pbstrUserNames );

				for( int i=0;i<lMenuCount;i++ )
				{
					CMenu	menu;
					menu.CreateMenu();					
					
					{
						CMenu	menuPopup;
						menuPopup.CreateMenu();
						menu.InsertMenu( 0, MF_BYPOSITION|MF_STRING|MF_POPUP, (UINT)menuPopup.Detach(), pbstrUserNames[i] );
					}

					MenuData* pdata = new MenuData;
					pdata->bDocumentMenu = false;
					pdata->hMenu = menu.Detach();
					pdata->strMenuName = (LPCSTR)pbstrUserNames[i];
					pdata->idx = (int)m_ptrsMenuInfo.GetSize();
					pdata->bPopupMenu = true;

					m_ptrsMenuInfo.Add( pdata );
				}

				delete []pbstrName;			pbstrName = 0;
				delete []pbstrUserNames;	pbstrUserNames = 0;
			}
		}
		

	//	CActionManagerWrapper	*pam = theApp.GetActionManager();

		ICompManagerPtr	sptrCompManager( theApp.GetComponent( IID_IActionManager, false ) );

		int	iActionsCount = 0;
		sptrCompManager->GetCount( &iActionsCount );
							 
		for( int i = 0; i < iActionsCount; i++ )
		{
			IUnknown *punk = 0;
 			
			sptrCompManager->GetComponentUnknownByIdx( i, &punk );

			// [vanek] : catch action - 16.12.2004
			m_advbmps.CatchActionInfo( punk );

			CActionInfoWrp	*pActionInfo = new CActionInfoWrp( punk );

			if( pActionInfo->Init() )
				AddActionInfo( pActionInfo );			
			else
				delete pActionInfo;

			punk->Release();
		}

		//load action controls
		CActionControlsRegistrator	rac;
		
		long	nControlsCount = rac.GetRegistredCount(), n;
		for( n = 0; n < nControlsCount; n++ )
		{
			CString	strProgID;
			CString	strAction;

			rac.GetActionControl( n, strProgID, strAction );

			CActionInfoWrp	*pai = GetActionInfo( strAction );
			if( !pai )continue;

			ActionControlData	*pcd = new ActionControlData;
			pcd->m_bstrProgID = strProgID;
			pcd->m_nLocalID = pai->GetLocalID();
			m_ptrsControl.Add( pcd );
		}
		

		//construct accelerator table
		int	nAccelSize = (int)m_ptrsAccel.GetSize();

		if( nAccelSize != 0 )
		{
			ACCEL	*paccel = new ACCEL[nAccelSize];

			for( int nAccel = 0; nAccel < nAccelSize; nAccel++ )
				memcpy( &paccel[nAccel], m_ptrsAccel[nAccel], sizeof( ACCEL ) );

			m_hAccel = ::CreateAcceleratorTable( paccel, nAccelSize );
			m_uiAccelEntries = nAccelSize;

			delete []paccel;
		}

		m_compBars.AttachComponentGroup( szPluginDockableWindows );
		m_compBars.SetRootUnknown( theApp.GetControllingUnknown() );
		m_compBars.Init();

	}
	_catch;
}

void CCommandManager::DeInit()
{
	int	i;
	for( i = 0; i < m_ptrsInfo.GetSize(); i++ )
		if( m_ptrsInfo[i] )
			delete (CActionInfoWrp *)m_ptrsInfo[i];
	for( i = 0; i < m_ptrsBitmapInfo.GetSize(); i++ )
		delete (BitmapData *)m_ptrsBitmapInfo[i];
	for( i = 0; i < m_ptrsToolBarInfo.GetSize(); i++ )
		delete (ToolBarData *)m_ptrsToolBarInfo[i];
	for( i = 0; i < m_ptrsMenuInfo.GetSize(); i++ )
		delete (MenuData *)m_ptrsMenuInfo[i];
	for( i = 0; i < m_ptrsAccel.GetSize(); i++ )
		delete (ACCEL*)m_ptrsAccel[i];
	for( i = 0; i < m_ptrsControl.GetSize(); i++ )
		delete (ActionControlData*)m_ptrsControl[i];

	
	

	m_ptrsInfo.RemoveAll();
	m_ptrsBitmapInfo.RemoveAll();
	m_ptrsToolBarInfo.RemoveAll();
	m_ptrsMenuInfo.RemoveAll();
	m_ptrsAccel.RemoveAll();
	m_ptrsControl.RemoveAll();

	::DestroyAcceleratorTable( m_hAccel );
	m_hAccel = 0;

	m_compBars.DeInit();
	m_ptrAliases = 0;
}

bool _remove_button( CBCGToolbarButton	*pbutton, UINT nIDCommand )
{
	if( pbutton->IsKindOf( RUNTIME_CLASS(CBCGToolbarMenuButton) ) )
	{
		POSITION pos =((CMyTBMenuBtn*)pbutton)->GetCommand().GetHeadPosition();

		while( pos )
		{
			POSITION	posOld = pos;
			CBCGToolbarButton *pb = (CBCGToolbarButton *)((CMyTBMenuBtn*)pbutton)->GetCommand().GetNext( pos );
			if( _remove_button( pb, nIDCommand ) )
			{
				((CMyTBMenuBtn*)pbutton)->GetCommand().RemoveAt( posOld );
				delete pb;
			}
		}
	}

	return pbutton->m_nID == nIDCommand;
}

void CCommandManager::RemoveActionInfo( int idx )
{
	UINT	nIDCommand = idx;

	idx -= ID_CMDMAN_BASE;

	if( idx < 0 || idx >= m_ptrsInfo.GetSize() )
		return;
	CActionInfoWrp	*pinfo = (CActionInfoWrp	*)m_ptrsInfo[idx];
	if( !pinfo )
		return;

	CString	s = pinfo->GetActionShortName();
	delete pinfo;

	m_ptrsInfo[idx] = 0;

	//remove action from toolbars
	for( POSITION pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*) gAllToolbars.GetNext(pos);

		if( pToolBar->IsKindOf( RUNTIME_CLASS(CBCGMenuBar) ) )
			continue;

		for( int nButton = pToolBar->GetCount()-1; nButton >=0 ; nButton-- )
		{
			CBCGToolbarButton	*pbutton = pToolBar->GetButton( nButton );

			if( _remove_button( pbutton, nIDCommand ) )
				pToolBar->RemoveButton( nButton );
		}
	}
	//TRACE( "Action removed:%s\n\r", (LPCTSTR)s );
}

void CCommandManager::AddActionInfo( CActionInfoWrp *pinfo )
{
	for( int idx = 0; idx < m_ptrsInfo.GetSize(); idx++ )
	{
		if( !m_ptrsInfo[idx] )
			break;
	}

	if( idx == m_ptrsInfo.GetSize() )
		m_ptrsInfo.Add( pinfo );
	else
		m_ptrsInfo[idx] = pinfo;

	pinfo->SetLocalID( idx+ID_CMDMAN_BASE );

	AddGroup( pinfo->GetActionGroupName() );

	AddBitmap( pinfo );
	AddMenuData( pinfo );
	AddToolBarData( pinfo );
	AddAccel( pinfo );

	//kill trace 17.12.2002 paul
	//TRACE( "Action added:%s\n\r", (LPCTSTR)pinfo->GetActionShortName() );
}

//!!!todo : not implemented yet
int CCommandManager::AddGroup( const char *sz )
{
/*	for( int i = 0; i < m_strsGroups.GetSize(); i++ )
		if( m_strsGroups[i] == sz )
			return i;
	return m_strsGroups.Add( sz );*/
	return -1;
}
			
void CCommandManager::AddBitmap( CActionInfoWrp *pinfo )
{
	if( pinfo->GetBitmapIdx() == -1 )
		return;
	for( int i = 0; i < m_ptrsBitmapInfo.GetSize(); i++ )
	{
		BitmapData *pdata = (BitmapData *)m_ptrsBitmapInfo[i];

		if( pinfo->GetBitmapIdx() == pdata->lBitmapIndex &&
			pinfo->GetBitmapProviderKey() == pdata->ProviderKey )
		{
			pinfo->m_pBmpData = pdata;
			return;
		}
	}
//fill bitmap structure and add it to pointers
	IUnknown *punkBmps;
	IUnknown *punk = pinfo->Unknown();

	{
		sptrIActionInfo	spAI( punk );
		spAI->GetTBInfo( 0, &punkBmps );
	}
	punk->Release();

	HANDLE	hBmp = 0;

	{
		sptrIBitmapProvider	spBP( punkBmps );
		spBP->GetBitmap( pinfo->GetBitmapIdx(), &hBmp );
	}
	punkBmps->Release();

	BitmapData	*pdata = new BitmapData;

	pdata->hBitmap = (HBITMAP)hBmp;
	pdata->lBitmapIndex = pinfo->GetBitmapIdx();
	pdata->ProviderKey = pinfo->GetBitmapProviderKey();

	m_ptrsBitmapInfo.Add( pdata );
	pinfo->m_pBmpData = pdata;
}

void CCommandManager::AddMenuData( CActionInfoWrp *pinfo )
{
	for( int idxMenu = 0; idxMenu < pinfo->GetDefMenusCount(); idxMenu++ )
	{
		CString	sMenu = pinfo->GetDefMenuName( idxMenu );
		CString	sMenuName = ExtractMenuName( sMenu );

		bool	bAddDocMenus = false;

		if( sMenuName == _T("anydoc") )
			bAddDocMenus = GetMenuCount() > 0;

		for( int idxMenuSearch = 0; idxMenuSearch < max( GetMenuCount(), 1 ); idxMenuSearch++ )
		{

			MenuData	*pdata = 0;

			if( bAddDocMenus )
			{
				pdata = GetMenuData( idxMenuSearch );
				if( !pdata->bDocumentMenu )
					continue;
			}
			else
				pdata = GetMenuData( sMenuName );

			

			if( !pdata )
			{
				CMenu	menu;
				menu.CreateMenu();

				bool	bPopup = sMenuName != "main";

				if( bPopup )
				{
					CMenu	menuPopup;
					menuPopup.CreateMenu();
					menu.InsertMenu( 0, MF_BYPOSITION|MF_STRING|MF_POPUP, (UINT)menuPopup.Detach(), sMenuName );
				}

				pdata = new MenuData;
				pdata->bDocumentMenu = false;
				pdata->hMenu = menu.Detach();
				pdata->strMenuName = sMenuName;
				pdata->idx = (int)m_ptrsMenuInfo.GetSize();
				pdata->bPopupMenu = bPopup;

				m_ptrsMenuInfo.Add( pdata );
			}

			HMENU	hMenu = pdata->hMenu;

			if( pdata->bPopupMenu )
				hMenu = ::GetSubMenu( hMenu, 0 );
			
			CMenu	menu;
			
			menu.Attach( hMenu );
			hMenu = 0;

			UINT nCurrentCmd = (UINT)ID_CMDMAN_BASE+m_ptrsInfo.GetSize()-1;
			CString	s;

			int idx = 0;

			for( ; idx != -1 && !sMenu.IsEmpty();  )
			{
				idx = sMenu.Find( "\\" );
				
				if( idx == -1 )
					s = sMenu;
				else
				{
					s = sMenu.Left( idx );
					sMenu = sMenu.Right( sMenu.GetLength() - idx - 1 );
				}

				hMenu = 0;
				for( UINT iMenuPos = 0; iMenuPos < menu.GetMenuItemCount(); iMenuPos++ )
				{
					CString	sText;
					menu.GetMenuString( iMenuPos, sText, MF_BYPOSITION );

					if( sText == s )
					{
						hMenu = ::GetSubMenu( menu, iMenuPos );
						break;
					}
				}

				if( !hMenu )
				{
					CMenu	menuPopup;
					menuPopup.CreateMenu();
					menu.InsertMenu( idx, MF_BYPOSITION|MF_STRING|MF_POPUP, (UINT)(HMENU)menuPopup, s );
					hMenu = menuPopup.Detach();
				}

				menu.Detach();
				menu.Attach( hMenu );
			}

			menu.InsertMenu( menu.GetMenuItemCount(), MF_BYPOSITION|MF_STRING, pinfo->GetLocalID(), pinfo->GetActionUserName() );
			menu.Detach();

			if( !bAddDocMenus )
				break;
		}
	}
}

CCommandManager::ToolBarData *CCommandManager::GetToolBarData( int idx )
{
	return (ToolBarData *)m_ptrsToolBarInfo[idx];
}

void CCommandManager::AddToolBarData( CActionInfoWrp *pinfo )
{
	for( int idxTB = 0; idxTB < pinfo->GetDefToolBarsCount(); idxTB++ )
	{
		ToolBarData *pdata = 0;
		bool bFound = false;

		CString	sTBName = pinfo->GetDefToolBarName( idxTB );

		for( int i = 0; i < m_ptrsToolBarInfo.GetSize(); i++ )
		{
			if( ((ToolBarData *)m_ptrsToolBarInfo[i])->m_sGroupName == sTBName )
			{
				pdata = (ToolBarData *)m_ptrsToolBarInfo[i];
				bFound = true;
			}
		}


		if( !pdata )
			pdata = new ToolBarData( sTBName );

		int	iBitmapNo = GetBitmapNo( pinfo );
		pdata->AddData( pinfo->GetLocalID(), pinfo->GetPictureIdx(), iBitmapNo, pinfo->GetActionUserName() );

		if( !bFound )
			m_ptrsToolBarInfo.Add( pdata );
	}
}

int CCommandManager::GetBitmapNo( CActionInfoWrp *pinfo )
{
	for( int i = 0; i < m_ptrsBitmapInfo.GetSize(); i++ )
	{
		BitmapData *pdata = (BitmapData *)m_ptrsBitmapInfo[i];

		if( pinfo->GetBitmapIdx() == pdata->lBitmapIndex &&
			pinfo->GetBitmapProviderKey() == pdata->ProviderKey )
			return i;
	}
	return -1;
}

int	CCommandManager::GetMenuCount()
{
	return (int)m_ptrsMenuInfo.GetSize();
}

CCommandManager::MenuData *
	CCommandManager::GetMenuData( int idx )
{
	return  (MenuData*)m_ptrsMenuInfo[idx];
}

CCommandManager::MenuData *
	CCommandManager::GetMenuData( const char *szData )
{
	for( int i = 0; i < m_ptrsMenuInfo.GetSize(); i++ )
	{
		MenuData *pdata = (MenuData*)m_ptrsMenuInfo[i];

		if( pdata->strMenuName == szData )
			return pdata;
	}
	return 0;
}


CCommandManager::BitmapData *
	CCommandManager::GetBitmapData( CActionInfoWrp *pinfo )
{
	return pinfo->m_pBmpData;
}

CCommandManager::BitmapData *
	CCommandManager::GetBitmapData( int idx )
{
	if( idx == -1 )
		return 0;
	return (BitmapData*)m_ptrsBitmapInfo[idx];
}


CString CCommandManager::ExtractMenuName( CString &strMenu )
{
	int	idx = strMenu.Find( "\\" );

	if( idx == -1 )
	{
		CString s = (LPCSTR)strMenu;
		strMenu.Empty();
		return s;
	}
	CString s = strMenu.Left( idx );
	strMenu = strMenu.Right( strMenu.GetLength()-1-idx );

	return s;
}

HMENU	CCommandManager::GetMenu( const char *sz )
{
	CString	sMenuName;

	if( sMenuName.IsEmpty() )
		sMenuName = szMenuMain;

	HMENU	hMenu = 0;

	CString	strData = sz;

	if( strData.IsEmpty() )
		strData = _T("main");
	
	MenuData	*pdata = GetMenuData( strData );

	if( pdata )
		return pdata->hMenu;

	return 0;
}

//bool	g_bInsideInvoke = false;
void CCommandManager::OnCommandAction( UINT nCmd )
{
	int idx = nCmd - ID_CMDMAN_BASE;
	if (idx < 0 || idx > m_ptrsInfo.GetUpperBound())
		return;

	//TRACE( "ExecuteCommand\n" );
	if( !( m_loActionState == loaTerminate || m_loActionState == loaComplete ) )
		return;

	IDispatchPtr ptrDispAM( theApp.GetComponent( IID_IActionManager, false ) );

	CActionInfoWrp	*pActionInfo = (CActionInfoWrp *)m_ptrsInfo[idx];
	
	_bstr_t	bstr_t( pActionInfo->GetActionShortName() );
	BSTR	bstr=bstr_t;
	long	disp_id = 0;

	if( ptrDispAM->GetIDsOfNames( IID_NULL, &bstr, 1, 0, &disp_id )!= ERROR_SUCCESS )
		return;

	VARIANT	res;
	::VariantInit( &res );

	EXCEPINFO	ei;
	ZeroMemory( &ei, sizeof( ei ) );

	unsigned	uerr;

	IActionManagerPtr	ptrAM( ptrDispAM );
	BOOL	bRunning = ptrAM->IsScriptRunning() == S_OK;
	ptrAM->SetScriptRunningFlag( false );
	ptrDispAM->Invoke( disp_id, IID_NULL, 0, 0, 0, &res, &ei, &uerr );
	ptrAM->SetScriptRunningFlag( bRunning );
	
	theApp.OnIdle( 0 );
}

/*BOOL CCommandManager::OnCommandActionEx( UINT nCmd )
{
	int idx = nCmd - ID_CMDMAN_BASE;
	if (idx < 0 || idx > m_ptrsInfo.GetUpperBound())
		return FALSE;

	//TRACE( "ExecuteCommand\n" );
	if( !( m_loActionState == loaTerminate || m_loActionState == loaComplete ) )
		return FALSE;

	IDispatchPtr ptrDispAM( theApp.GetComponent( IID_IActionManager, false ) );

	CActionInfoWrp	*pActionInfo = (CActionInfoWrp *)m_ptrsInfo[idx];
	
	_bstr_t	bstr_t( pActionInfo->GetActionShortName() );
	BSTR	bstr=bstr_t;
	long	disp_id = 0;

	if( ptrDispAM->GetIDsOfNames( IID_NULL, &bstr, 1, 0, &disp_id )!= ERROR_SUCCESS )
		return FALSE;

	VARIANT	res;
	::VariantInit( &res );

	EXCEPINFO	ei;
	ZeroMemory( &ei, sizeof( ei ) );

	unsigned	uerr;

	IActionManagerPtr	ptrAM( ptrDispAM );
	BOOL	bRunning = ptrAM->IsScriptRunning() == S_OK;
	ptrAM->SetScriptRunningFlag( false );
	ptrDispAM->Invoke( disp_id, IID_NULL, 0, 0, 0, &res, &ei, &uerr );
    ptrAM->SetScriptRunningFlag( bRunning );
	if( res.vt == VT_I4 && res.lVal == 0 )
		return FALSE;
    	
	theApp.OnIdle( 0 );
    return TRUE;
}*/

static bool benable = true;


void CCommandManager::OnUpdateCommandAction( CCmdUI *pCmdUI )
{

	int idx = pCmdUI->m_nID - ID_CMDMAN_BASE;
	if (idx < 0 || idx > m_ptrsInfo.GetUpperBound())
		return;

	CActionInfoWrp	*pActionInfo = (CActionInfoWrp *)m_ptrsInfo[idx];

	if( !pActionInfo )
	{
		pCmdUI->Enable( FALSE );
		return;
	}

	sptrIActionManager sptrAM( theApp.GetComponent( IID_IActionManager, false ) );

	_bstr_t	bstrActionName( pActionInfo->GetActionShortName() );
	DWORD	dwFlags = 0;

	sptrAM->GetActionFlags( bstrActionName, &dwFlags );

	if( !benable )
		return;

	if (m_sptrMenuHelper2 != 0)
		m_sptrMenuHelper2->UpdateActionState( bstrActionName, pCmdUI->m_nID, &dwFlags );

	pCmdUI->Enable( (dwFlags & afEnabled) != 0 );
	pCmdUI->SetCheck( (dwFlags & afChecked) != 0);
}

void CCommandManager::OnUpdateCommandClassMenu( CCmdUI *pCmdUI )
{
	sptrIActionManager sptrAM( theApp.GetComponent( IID_IActionManager, false ) );
	DWORD	dwFlags = 0;
	sptrAM->GetActionFlags( _bstr_t("SetClass"), &dwFlags );
	pCmdUI->Enable( (dwFlags & afEnabled) != 0 );
}

void CCommandManager::OnUpdateCommandPopupMenu( CCmdUI *pCmdUI )
{
	if (pCmdUI->m_pOther != 0 && pCmdUI->m_pOther->IsKindOf(RUNTIME_CLASS(CBCGPopupMenuBar)))
	{
		CShellPopupMenu* pParentMenu = DYNAMIC_DOWNCAST (CShellPopupMenu, pCmdUI->m_pOther->GetParent ());
		if (pParentMenu != 0)
			pParentMenu->UpdateCmdUI(pCmdUI);
	}
}


int CCommandManager::GetToolBarCount()
{
	return (int)m_ptrsToolBarInfo.GetSize();
}

int CCommandManager::GetBitmapCount()
{
	return (int)m_ptrsBitmapInfo.GetSize();	
}

CActionInfoWrp *CCommandManager::GetActionInfo( const GUID guid )
{
	for( int i =0; i < m_ptrsInfo.GetSize(); i++ )
	{
		CActionInfoWrp	*pi = (CActionInfoWrp	*)m_ptrsInfo[i];
		if( !pi )
			continue;
		if( GetObjectKey( pi->m_pActionInfo ) == guid )
			return pi;
	}
	return 0;
}

CActionInfoWrp	*CCommandManager::GetActionInfo( int idx )
{
	if( idx < 0 || idx >=m_ptrsInfo.GetSize() )
		return 0;
	return (CActionInfoWrp	*)m_ptrsInfo[idx];
}

CActionInfoWrp	*CCommandManager::GetActionInfo( const char *szName )
{
	for( int i =0; i < m_ptrsInfo.GetSize(); i++ )
	{
		CActionInfoWrp	*pi = (CActionInfoWrp	*)m_ptrsInfo[i];
		if( !pi )
			continue;
		if( pi->GetActionShortName() == szName )
			return pi;
	}
	return 0;
}

int CCommandManager::GetActionCommand( const char *szActionName )
{
	for( int i =0; i < m_ptrsInfo.GetSize(); i++ )
	{
		CActionInfoWrp	*pi = (CActionInfoWrp	*)m_ptrsInfo[i];
		if( !pi )
			continue;
		if( pi->GetActionShortName() == szActionName )
			return i+ID_CMDMAN_BASE;
	}
	return -1;
}

int CCommandManager::GetActionInfoCount()
{
	return (int)m_ptrsInfo.GetSize();
}

bool CCommandManager::GetMessageString(UINT nID, CString& rMessage)
{
	if( (int)nID < ID_CMDMAN_BASE )return false;
	if( (int)nID >= ID_CMDMAN_BASE+GetActionInfoCount() )return false;

	CActionInfoWrp	*pai = GetActionInfo( nID - ID_CMDMAN_BASE );

	if( !pai )
	{
		//rMessage.LoadString( IDS_UNAVAIBLE_ACTION );
		rMessage = ::LanguageLoadCString( IDS_UNAVAIBLE_ACTION );
		return true;
	}

	rMessage = pai->GetActionHelpString();

	int	idx = rMessage.Find( "\n" );

	if(  idx != -1 )
		rMessage = rMessage.Left( idx );

	return true;
}

bool CCommandManager::GetToolTipString(UINT nID, CString& rMessage)
{
	if( (int)nID < ID_CMDMAN_BASE )return false;
	if( (int)nID >= ID_CMDMAN_BASE+GetActionInfoCount() )return false;

	CActionInfoWrp	*pai = GetActionInfo( nID - ID_CMDMAN_BASE );

	rMessage = pai->GetActionHelpString();

	int	idx = rMessage.Find( "\n" );

	if(  idx != -1 )
		rMessage = rMessage.Right( rMessage.GetLength() - idx - 1 );

	return true;
}

//dispatch interfac eand helpers
CBCGToolBar	*CCommandManager::GetToolBarNo( int iNo )
{
	for( POSITION pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext(pos);
		ASSERT_VALID(pToolBar);

		if( !iNo-- )
			return pToolBar;
	}
	return 0;

}

CShellDockBar	*CCommandManager::GetDockBarNo( int nNo )
{
	CMainFrame	*pmain = (CMainFrame*)AfxGetMainWnd();
	if( !pmain )return 0;

	POSITION	pos = pmain->GetFirstDockBarPos();

	while( pos )
	{
		CShellDockBar	*pbar = pmain->GetNextDockBar( pos );
		if( !nNo )return pbar;
		nNo --;
	}

	AfxMessageBox( "Wrong dockbar index" );

	return 0;
}

bool CCommandManager::CheckIdx( int	idx )
{
	if( idx == -1 || idx >= GetBarsCount() )
	{
		AfxMessageBox( IDS_BAD_TOOLBAR_IDX );
		return false;
	}
	return true;
}

//disp impl
long CCommandManager::GetBarsCount() 
{
	return (int)gAllToolbars.GetCount();
}

BSTR CCommandManager::GetBarName(long BarNo) 
{
	if( !CheckIdx( BarNo ) )
		return 0;

	CString strResult;

	CBCGToolBar	*ptoolBar = GetToolBarNo( BarNo );

	ptoolBar->GetWindowText( strResult );

	return strResult.AllocSysString();
}

long CCommandManager::CreateNewBar(LPCTSTR BarName) 
{
	long idx = FindNo( BarName );

	if( idx != -1 )
	{
		ShowBar( idx, 1 );
		return idx;
	}
	CMainFrame	*pwnd = (CMainFrame	*)AfxGetMainWnd();
	CBCGToolBar	*ptoolbar = (CBCGToolBar	*)pwnd->CreateNewToolbar( BarName );

	if( !ptoolbar )
		return -1;

//	ptoolbar->EnableCustomizeButton (TRUE, -1, _T(""));

	pwnd->DockControlBar( ptoolbar, AFX_IDW_DOCKBAR_TOP );

	return idx = FindNo( BarName );
}

void CCommandManager::MakeEmpty(long BarNo) 
{
	if( !CheckIdx( BarNo ) )
		return;

	CBCGToolBar	*ptool = GetToolBarNo( BarNo );
	ptool->RemoveAllButtons();

	CFrameWnd	*pwnd = ptool->GetParentFrame();
	pwnd->RecalcLayout();
}

void CCommandManager::AddButton(long BarNo, LPCTSTR ActionName) 
{
	InsertButton( BarNo, ActionName, -1 );
}

long CCommandManager::FindNo(LPCTSTR Name) 
{
	long	idx = 0;

	for( POSITION pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext(pos);
		ASSERT_VALID(pToolBar);

		CString	strText;

		pToolBar->GetWindowText( strText );

		if( strText == Name )
			return idx;

		idx ++;
	}

	return -1;
}

void CCommandManager::ShowBar(long BarNo, long Visible) 
{
	if( !CheckIdx( BarNo ) )
		return;

	CFrameWnd	*pwnd = (CFrameWnd	*)AfxGetMainWnd();
	CBCGToolBar	*ptool = GetToolBarNo( BarNo );

	/*{
		char szText[256];
		ptool->GetWindowText(szText, 256);
	trace("ShowBar %d:%s %d\n", BarNo, szText, Visible);
	}*/

	pwnd->ShowControlBar( ptool, Visible == 1, TRUE );
	ptool->SendMessage( WM_SHOWWINDOW, Visible == 1 );

	pwnd = ptool->GetParentFrame();
	pwnd->RecalcLayout();
}

void CCommandManager::InsertButton(long BarNo, LPCTSTR ActionName, long InsertAfter) 
{
	if( !CheckIdx( BarNo ) )
		return;

	CBCGToolBar	*ptool = GetToolBarNo( BarNo );

	if( !ptool )
		return;
	
	if( !strcmp( ActionName , "-" ) )
	{
		ptool->InsertSeparator();
		return;
	}

	CActionInfoWrp *pinfo = g_CmdManager.GetActionInfo( ActionName );

	if( !pinfo )
		return;

	BitmapData	*pBitmapData = g_CmdManager.GetBitmapData( pinfo );

	CShellToolbarButton	btn(
					pinfo->GetLocalID(),
					pBitmapData?(pinfo->GetPictureIdx() + pBitmapData->lImagesOffset):-1,
					pinfo->GetActionUserName(), 
					FALSE );

	int	nNewNo = ptool->InsertButton( btn, InsertAfter );

	if( ptool->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
		((CShellOutlookToolBar*)ptool)->AdjustLocations();
	else if( ptool->IsKindOf( RUNTIME_CLASS(CShellToolBar) ) )
		((CShellToolBar*)ptool)->AdjustLocations();

	CFrameWnd	*pwnd = ptool->GetParentFrame();
	pwnd->RecalcLayout();
}

void CCommandManager::DeleteBar(long BarNo) 
{
	if( !CheckIdx( BarNo ) )
		return;

	CBCGToolBar	*ptool = GetToolBarNo( BarNo );

	if( !ptool )
		return;

	CFrameWnd	*pwnd = (CFrameWnd	*)AfxGetMainWnd();
	if( pwnd->SendMessage( BCGM_DELETETOOLBAR, 0, (LPARAM)ptool ) == 0 )
		MessageBeep (MB_ICONASTERISK);
	
}

void CCommandManager::AddAccel( CActionInfoWrp *pinfo )
{
	DWORD	dw;
	BYTE	byte;

	if( pinfo->m_pActionInfo->GetHotKey( &dw, &byte ) == S_OK )
	{
		ACCEL	*paccel = new ACCEL;
		paccel->fVirt = byte;
		paccel->key = (USHORT)dw;
		paccel->cmd = pinfo->GetLocalID();

		m_ptrsAccel.Add( paccel );
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//SaveState & LoadState helpers
CMap<CString, const char*, long, long&>	g_mapUserImages;

const char *szSignature = "VideoTesT 5.0 shell configuration file";
const char *szVersion = "Version 0.9";
const char *szFloating = "floated";
const char *szVisible = "visible";
const char *szInvisible = "invisible";
const char *szSeparator = "-----";
const char *szEndmarker = "-finish-";
const char *szBeginPopup = "-pop-up-";
const char *szEndPopup = "-end-pop-up-";


const char *szTop = "dockedTop";
const char *szBottom = "dockedBottom";
const char *szLeft = "dockedLeft";
const char *szRight = "dockedRight";
const char *szOutlook = "outlook";

const char *szToolbars = "[Toolbars]";
const char *szMenus = "[Menus]";
const char *szKeyboard = "[Keyboard]";
const char *szControlBars = "[Controlbars]";
const char *szOutlookToolbars = "[Outlook]";
const char *szDockBar = "Appropriate DocBar information";
const char *szControlBarsNew = "[newControlBars]";
const char *szUserImageMapping = "[UserImageMapping]";
const char *szNotFound = "DockBar not found - it's very strange";

const char *szFrameToolBar = "[FrameToolBars]";


void _make_button_string( CBCGToolbarButton *pbutton, CStringArrayEx &strings )
{

	if( pbutton->m_nStyle & TBBS_SEPARATOR ||
		(pbutton->IsKindOf(RUNTIME_CLASS(CBCGToolbarMenuButton))&&
		pbutton->m_nID == 0 && pbutton->m_nStyle == 0 && pbutton->m_strText.IsEmpty() ) )
	{
		strings.Add( szSeparator );
		return;
	}

	CString	strRuntime;
	strRuntime = pbutton->GetRuntimeClass()->m_lpszClassName;

	if( strRuntime == "CCustomizeButton" )
		return;

	CString	strActionName;

	int	idx = pbutton->m_nID - ID_CMDMAN_BASE;

	if( pbutton->IsKindOf( RUNTIME_CLASS( CShellChooseToolbarButton ) ) )
		idx = -1;

	if( idx  >= 0 && idx < g_CmdManager.GetActionInfoCount() )
	{
		CActionInfoWrp	*pinfo = 0;
		pinfo = g_CmdManager.GetActionInfo( idx );
		if( !pinfo )
			return;

		strActionName = pinfo->GetActionShortName();
	}

	
	byte	*pdata = 0;
	long	nDataSize = 0;

	_try(pbutton, Serialize)
	{
		CMemFile	file;
		CArchive	ar( &file, CArchive::store );


		CShellToolbarButton	*pshellbtn = 0;
		if( pbutton->IsKindOf( RUNTIME_CLASS( CShellToolbarButton ) ) )
			pshellbtn = (CShellToolbarButton*)pbutton;

		//( pbutton->IsKindOf( RUNTIME_CLASS(CBCGToolbarMenuButton) ) )
		//	pbutton->CBCGToolbarButton::Serialize( ar );
		//else
		if( pshellbtn )pshellbtn->EnableMenuSerialize( false );
		pbutton->Serialize( ar );
		if( pshellbtn )pshellbtn->EnableMenuSerialize( true );

		ar.Close();

		nDataSize = (long)file.GetLength();
		pdata = file.Detach();
	}
	_catch;
	

	CString	strData;

	for( int i = 0; i < nDataSize; i++ )
	{
		CString	str;
		str.Format( "%02x ", pdata[i] );
		strData += str;
	}

	::free( pdata );

	CString	strResult;
	strResult.Format( "%s, %s, %s", (const char *)strRuntime, (const char *)strActionName, (const char *)strData );


	//ASSERT( strActionName != "Add_Item_to_Outlook" );

	strings.Add( strResult );

	//check user image. If button has user image, store it in mapping
	int	nImage = pbutton->GetImage();
	if( nImage == -1 )
	{
		nImage = CMD_MGR.GetCmdImage( pbutton->m_nID, true );
		if( nImage != -1 )
		{
			pbutton->SetImage( nImage );
			pbutton->m_bUserButton = true;
		}
	}

	if( pbutton->m_bUserButton && nImage != -1 )
		g_mapUserImages[strActionName] = nImage;

	if( pbutton->IsKindOf( RUNTIME_CLASS(CBCGToolbarMenuButton) ) && strActionName.IsEmpty() )
	{
		CMyTBMenuBtn	*pmenubtn = (CMyTBMenuBtn*)pbutton;

		strings.Add( szBeginPopup );
		POSITION pos =pmenubtn->GetCommand().GetHeadPosition();

		while( pos )
		{
			CBCGToolbarButton *pb = (CBCGToolbarButton *)
				pmenubtn->GetCommand().GetNext( pos );
			_make_button_string( pb, strings );
		}
		strings.Add( szEndPopup );
	}
}

CBCGToolbarButton *_create_button_from_string( CStringArray &strings, int &ipos, bool bMenuBar )
{
	CString	strRuntime, strActionName, strBinData;
	CString	str = strings[ipos++];

	if( str == szSeparator )
	{
		CBCGToolbarButton	*p = new CBCGToolbarButton();
		p->m_nStyle = TBBS_SEPARATOR;
		p->SetImage( 7 );
		return p;
	}

	int	idx1 = str.Find( "," );

	if( idx1 == -1 )
	{
		strRuntime = RUNTIME_CLASS(CBCGToolbarButton)->m_lpszClassName;
		strActionName = str;
	}
	else
	{
		strRuntime = str.Left( idx1 );
		if( strRuntime == "CCustomizeButton" )
			return 0;
		str = str.Right( str.GetLength() - idx1 - 2 );

		int	idx2 = str.Find( "," );

		if( idx2 == -1 )
			strActionName = str;
		else
		{
			strActionName = str.Left( idx2 );
			strBinData = str.Right( str.GetLength() - idx2 - 2 );
		}
	}

	CRuntimeClass *pclass = 0, *pclassTest;
	// search app specific classes
	AFX_MODULE_STATE* pModuleState = AfxGetAppModuleState();
	for (pclassTest = pModuleState->m_classList; pclassTest != NULL;
		pclassTest = pclassTest->m_pNextClass)
		if( strRuntime == pclassTest->m_lpszClassName )
		{
			pclass = pclassTest;
			break;
		}

	if( !pclass )
	{
		for (CDynLinkLibrary* pDLL = pModuleState->m_libraryList; pDLL != NULL;
			pDLL = pDLL->m_pNextDLL)
		{
			for (pclassTest = pDLL->m_classList; pclassTest != NULL;
				pclassTest = pclassTest->m_pNextClass)
				if( strRuntime == pclassTest->m_lpszClassName )
				{
					pclass = pclassTest;
					break;
				}
			if( pclass )
				break;
		}
	}



	if( !pclass )
		pclass = RUNTIME_CLASS(CBCGToolbarButton);

	if( pclass == RUNTIME_CLASS(CBCGToolbarButton) )
		pclass = RUNTIME_CLASS(CShellToolbarButton);

	//if( !bMenuBar )
		if( pclass == RUNTIME_CLASS(CBCGToolbarMenuButton) )
			pclass = RUNTIME_CLASS(CShellToolbarButton);

	CBCGToolbarButton *pbtn = (CBCGToolbarButton*)pclass->CreateObject();

	if( !pbtn )
		return false;

	if( !strBinData.IsEmpty() )
	{
		DWORD	dwDataSize = strBinData.GetLength()/3;
		BYTE	*pdata = new BYTE[dwDataSize];

		char	*sz = strBinData.GetBuffer( strBinData.GetLength() );

		for( int i = 0; i < (int)dwDataSize; i++ )
		{
			int	ival;
			sscanf( sz+i*3, "%x", &ival );
			pdata[i] = (byte)ival;
		}

		strBinData.ReleaseBuffer();

		CMemFile	file( pdata, dwDataSize );
		

		_try(pbtn, Serialize)
		{
		CArchive	ar( &file, CArchive::load );
			//if( pbtn->IsKindOf( RUNTIME_CLASS(CBCGToolbarMenuButton) ) )
			//	pbtn->CBCGToolbarButton::Serialize( ar );
			//else

			
		CShellToolbarButton	*pshellbtn = 0;
		if( pbtn->IsKindOf( RUNTIME_CLASS( CShellToolbarButton ) ) )
			pshellbtn = (CShellToolbarButton*)pbtn;

		//( pbutton->IsKindOf( RUNTIME_CLASS(CBCGToolbarMenuButton) ) )
		//	pbutton->CBCGToolbarButton::Serialize( ar );
		//else
		if( pshellbtn )pshellbtn->EnableMenuSerialize( false );
		pbtn->Serialize( ar );
		if( pshellbtn )pshellbtn->EnableMenuSerialize( true );
				
		}
		_catch;

		delete []pdata;
	}
	
	if( !strActionName.IsEmpty() )
	{
		CActionInfoWrp	*pinfo = g_CmdManager.GetActionInfo( strActionName );

		if( !pinfo )
		{
			delete pbtn;
			return 0;
		}

		//g_ImageHash.Clear( pbtn->m_nID );
		//g_CmdManager.m_UserImages.RemoveKey (uiCmd);
		//GetCmdMgr()->SetCmdImage( pbtn->m_nID, int iImage

		pbtn->m_nID = pinfo->GetLocalID();


		CCommandManager::BitmapData	*pBitmapData = g_CmdManager.GetBitmapData( pinfo );

		if( pbtn->m_bUserButton )
		{
			long nNewIndex = -1;
			g_mapUserImages.Lookup( strActionName, nNewIndex );

			if( nNewIndex == -1 )			// [Max] Bt: 3151	   Check-in version: 113
				nNewIndex = pbtn->GetImage();

			pbtn->SetImage( nNewIndex );

			if( nNewIndex != -1 )
			{
				CMD_MGR.SetCmdImage( pbtn->m_nID, -1, FALSE );
				CMD_MGR.SetCmdImage( pbtn->m_nID, nNewIndex, TRUE );
			}
			else
				pbtn->m_bUserButton = false;
		}

		if( !pbtn->m_bUserButton )
		{
			if( pBitmapData )
			{
				pbtn->SetImage( pinfo->GetPictureIdx() + pBitmapData->lImagesOffset );
				//pbtn->m_bImage = true;
				CMD_MGR.SetCmdImage( pbtn->m_nID, pbtn->GetImage(), false );
			}
			else
			{
				pbtn->SetImage( -1 );
				pbtn->m_bImage = false;
			}
		}

		//update name

		pbtn->m_strText = pinfo->GetActionUserName();
	}

	if( pbtn->IsKindOf( RUNTIME_CLASS(CBCGToolbarMenuButton) ) && strActionName.IsEmpty() )
	{
		CString	str = strings[ipos++];
		CMyTBMenuBtn	*pmenubtn = (CMyTBMenuBtn*)pbtn;

		while( true )
		{
			str = strings[ipos++];

			if( !str.CompareNoCase( szEndPopup ) )
				break;


			ipos --;	//step back

			if( !str.CompareNoCase( szEndmarker ) )
				break;

			

			CBCGToolbarButton *p1 = _create_button_from_string( strings, ipos, bMenuBar );

			if( !p1 )
				continue;

			if( p1->m_nStyle == TBBS_SEPARATOR )
			{
				delete p1;
				p1= new CBCGToolbarMenuButton();
			}

			pmenubtn->GetCommand().AddTail( p1 );

			if( ipos > strings.GetSize() )
				break;
		}

		if( pmenubtn->GetCommand().GetCount() == 0 ) 
		{
			delete pmenubtn;
			return 0;
		}
	}

	return pbtn;
}


void _make_controlbar_string( CControlBarInfo *pCBInfo, CStringArrayEx &strings )
{
	byte	*pdata = 0;
	long	nDataSize = 0;

	_try(pCBInfo, Serialize)
	{
		CMemFile	file;
		CArchive	ar( &file, CArchive::store );

		ar << (DWORD)pCBInfo->m_nBarID;
		ar << (DWORD)pCBInfo->m_bVisible;
		ar << (DWORD)pCBInfo->m_bFloating;
		ar << (DWORD)pCBInfo->m_bHorz;
		ar << pCBInfo->m_pointPos;

		ar << (DWORD)pCBInfo->m_nMRUWidth;
		ar << (DWORD)pCBInfo->m_bDocking;
		if (pCBInfo->m_bDocking)
		{
			ar << (DWORD)pCBInfo->m_uMRUDockID;
			ar << pCBInfo->m_rectMRUDockPos;
			ar << pCBInfo->m_dwMRUFloatStyle;
			ar << pCBInfo->m_ptMRUFloatPos;
		}
		
		ar << (WORD)pCBInfo->m_arrBarID.GetSize();
		if (pCBInfo->m_arrBarID.GetSize() != 0)
		{
#ifdef _AFX_BYTESWAP
			if (!ar.IsByteSwapping())
#endif
			ar.Write(&pCBInfo->m_arrBarID.ElementAt(0),	(UINT)pCBInfo->m_arrBarID.GetSize()*sizeof(DWORD));
#ifdef _AFX_BYTESWAP
			else
			{
				// write each ID individually so that it will be byte-swapped
				for (int i = 0; i < pCBInfo->m_arrBarID.GetSize(); i++)
					ar << (DWORD)pCBInfo->m_arrBarID[i];
			}
#endif
		}
		ar.Close();

		nDataSize = (long)file.GetLength();
		pdata = file.Detach();
	}
	_catch;
	

	CString	strData;

	for( int i = 0; i < nDataSize; i++ )
	{
		CString	str;
		str.Format( "%02x ", pdata[i] );
		strData += str;
	}

	::free( pdata );

	strings.Add( strData );
}

CControlBarInfo *_create_control_from_string( CStringArray &strings, int &ipos  )
{
	CString	str = strings[ipos++];
	
	DWORD	dwDataSize = str.GetLength()/3;
	BYTE	*pdata = new BYTE[dwDataSize];
	char	*sz = str.GetBuffer( str.GetLength() );
	for( int i = 0; i < (int)dwDataSize; i++ )
	{
		int	ival;
		sscanf( sz+i*3, "%x", &ival );
		pdata[i] = (byte)ival;
	}

	str.ReleaseBuffer();
	CMemFile	file( pdata, dwDataSize );
	CArchive	ar( &file, CArchive::load );

	CControlBarInfo *pCBInfo = new CControlBarInfo();

	_try(pbtn, Serialize)
	{
		DWORD dw;
		ar >> dw;
		pCBInfo->m_nBarID = (int)dw;
		ar >> dw;
		pCBInfo->m_bVisible = (BOOL)dw;
		ar >> dw;
		pCBInfo->m_bFloating = (BOOL)dw;
		ar >> dw;
		pCBInfo->m_bHorz = (BOOL)dw;
		ar >> pCBInfo->m_pointPos;

		ar >> dw;
		pCBInfo->m_nMRUWidth = (int)dw;
		ar >> dw;
		pCBInfo->m_bDocking = (BOOL)dw;
		if (pCBInfo->m_bDocking)
		{
			ar >> dw;
			pCBInfo->m_uMRUDockID = (DWORD)dw;
			ar >> pCBInfo->m_rectMRUDockPos;
			ar >> pCBInfo->m_dwMRUFloatStyle;
			ar >> pCBInfo->m_ptMRUFloatPos;
		}

		WORD w;
		ar >> w;
		pCBInfo->m_arrBarID.SetSize(w);
		if (w != 0)
		{
			ar.Read(&pCBInfo->m_arrBarID.ElementAt(0), (UINT)pCBInfo->m_arrBarID.GetSize()*sizeof(DWORD));
#ifdef _AFX_BYTESWAP
			if (ar.IsByteSwapping())
			{
				for (int i = 0; i < pCBInfo->m_arrBarID.GetSize(); i++)
					_AfxByteSwap((DWORD)pCBInfo->m_arrBarID[i], (BYTE*)&pCBInfo->m_arrBarID[i]);
			}
#endif
		}

		delete []pdata;
		return pCBInfo;
	}
	_catch;
	delete []pdata;
	delete pCBInfo;

	return 0;
}

static void _DockControlBar(CFrameWnd *pmain, CControlBar* ptool, UINT nDockBarID, CRect rc)
{
	CDockBar* pDockBar = (CDockBar*)pmain->GetControlBar(nDockBarID);
	pDockBar->ClientToScreen(rc);
	pmain->DockControlBar(ptool, nDockBarID, rc);
}


void CCommandManager::LoadState(LPCTSTR FileName) 
{
	_load_state(FileName);
}

bool CCommandManager::_load_state(LPCTSTR FileName) 
{
//	trace("LoadState %s\n", FileName);

 	CreateTemplateInfoList();	
	m_uiAccelEntries = 0;

	//get desktop window size
	CWnd	*pwndDesktop = CWnd::FromHandle( GetDesktopWindow() );
	CRect	rectDesktop,rcWorkArea;
	if (!SystemParametersInfo(SPI_GETWORKAREA, 0, (LPRECT)&rectDesktop, FALSE))
		pwndDesktop->GetClientRect( &rectDesktop );


	CDockState state;
	_try(CCommandManager, LoadState)
	{
		bool	bMaximizedMode = false;

		CMDIChildWnd	*pActiveChild = ((CMDIFrameWnd*)AfxGetMainWnd())->MDIGetActive();
		if( pActiveChild )
			bMaximizedMode = pActiveChild->IsZoomed() != 0;

//generate paths
		CString	strStatePathName;
		CString	strBmpPathName;
		CString	strStateDefPath;

		strStateDefPath = ::MakeAppPathName( "States" );
		strStateDefPath+= "\\";
		strStateDefPath = ::GetValueString( ::GetAppUnknown(), "Paths", "States", strStateDefPath );


		//firstly load user images
		char szDrvDef[_MAX_DRIVE], szPathDef[_MAX_PATH];
		char szDrv[_MAX_DRIVE], szPath[_MAX_PATH], szFileName[_MAX_PATH], szExt[_MAX_PATH];

		::_splitpath( FileName, szDrv, szPath, szFileName, szExt );
		::_splitpath( strStateDefPath, szDrvDef, szPathDef, 0, 0 );

		if( !strlen( szDrv ) )
			strcpy( szDrv, szDrvDef );
		if( !strlen( szPath ) )
			strcpy( szPath, szPathDef );
		if( !strlen( szExt ) )
			strcpy( szExt, ".state" );
			
		::_makepath( strStatePathName.GetBuffer( MAX_PATH ), szDrv, szPath, szFileName, szExt );
		::_makepath( strBmpPathName.GetBuffer( MAX_PATH ), szDrv, szPath, szFileName, ".tbbmp" );

		strStatePathName.ReleaseBuffer();
		strBmpPathName.ReleaseBuffer();

		if( !m_UserImages.Load( strBmpPathName ) )
		{
			HBITMAP	hbmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), 
				MAKEINTRESOURCE(IDR_USERIMAGES), 
				IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADMAP3DCOLORS );
			m_UserImages.AddImage( hbmp );
			m_UserImages.SetUserFlag( true );
			m_UserImages.Save( strBmpPathName );
		}

		::SetValue( GetAppUnknown(), "General", "CurrentState", strStatePathName );

		CStringArrayEx	strings;
		strings.ReadFile( strStatePathName );

		if( strings[0] != szSignature )
			return false;
		if( strings[1] != szVersion )
			return false;
		//reset all images cache
		CMD_MGR.ClearAllCmdImages();



//BCG 4.64 adapted
		CBCGToolbarButton::s_bInitProgMode = true;

//if we are here, let's destruct all toolbars
		CMainFrame	*pmain = (CMainFrame	*)AfxGetMainWnd();
		bool bOldAnimation = pmain->EnableOutlookAnimation( false );

		COutlookBar	&bar = pmain->m_wndOutlook;
		bar.DeleteAllFolderBars();

		for( POSITION pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext(pos);
			if(IsWindow(pToolBar->m_hWnd))
				pmain->DeleteToolBar( pToolBar );
		}
		pos  = pmain->GetFirstDockBarPos();
		while( pos )
		{
			CControlBar	*pbar = pmain->GetNextDockBar( pos );
			if(pbar == NULL || !pbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)))
				continue;
			pbar->ShowWindow(SW_HIDE);
		}

		//pmain->ResetDockSites();
		int	idx;
		
		g_mapUserImages.RemoveAll();

		idx = (int)strings.Find( szUserImageMapping );
		if( idx != -1 )
		{
			while( idx  <strings.GetSize() )
			{
				CString	str = strings[idx++];
				if( str == szEndmarker )
					break;
				if( str.IsEmpty() || str[0] == '\'' )
					continue;

				int	i = str.Find( "=" );
				if( i == -1 )
					continue;

				CString	strA = str.Left( i );
				str = str.Right( str.GetLength()-1-i );

				long	l;
				::sscanf( str, "%d", &l );

				g_mapUserImages[strA] = l;
			}
		}
//goto toolbars section
		BOOL bEnableTBVert = GetValueInt(::GetAppUnknown(), "\\MainFrame", "EnableDockToolbarsVertically", TRUE);
		idx = (int)strings.Find( szToolbars );

		if( idx != -1 )
		{
			idx++;

			while( idx < strings.GetSize() )
			{
				CString	str = strings[idx++];

				if( str.IsEmpty() || str[0] == '\'' )
					continue;
				if( str[0] == '[' )
					break;
				//toolbar here
				
/********************** Added by Maxim ( internal toolbars ) ***************************************/				
				BOOL bInternal = true;
				
				{
					if( !str.IsEmpty() )
					{
						int nID = str.Find( "#user" );
						if( nID != -1 )
						{
							bInternal = false;
							str = str.Left( nID );
						}
					}
				}
/********************** Added by Maxim ( internal toolbars ) ***************************************/				

				CString	strName = str;
				CString	strDockSite = strings[idx++];
				CString	strVisible = strings[idx++];
				CString	strWindowRect = strings[idx++];

				bool bVisible = strVisible.CompareNoCase( szVisible ) == 0;

				//create				
				CBCGToolBar *ptool = (CBCGToolBar *)pmain->CreateNewToolbar( strName );
				if (!bEnableTBVert)
					ptool->EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);

				if( !ptool )
					continue;

/********************** Added by Maxim ( internal toolbars ) ***************************************/				
				int nDeveloper = ::GetValueInt( GetAppUnknown() , "\\Toolbars", "Developer", 0 );
				int nID = ptool->GetDlgCtrlID();
				if( bInternal )
				{
					if( !nDeveloper )
						ptool->SetDlgCtrlID( nID - iMaxUserToolbars );
				}
				else
				{
					if( nDeveloper )
						ptool->SetDlgCtrlID( nID + iMaxUserToolbars );
				}
/********************** Added by Maxim ( internal toolbars ) ***************************************/				

				while( true )
				{
					CString	strAction = strings[idx++];

					if( strAction.IsEmpty() || strAction[0] == '\'' )
						continue;

					if( strAction == szEndmarker )
					{
						break;
					}

					idx--;	//step back

					CBCGToolbarButton	*pbtn = _create_button_from_string( strings, idx, false );

					if( !pbtn )
						continue;
					ptool->InsertButton( *pbtn );

					delete pbtn;
				}

				CRect	rc(0,0,0,0);
				int nMRUWidth = 0;
				int nFields = ::sscanf( strWindowRect, "%d %d %d %d %d", &rc.left, &rc.top, &rc.right, &rc.bottom, &nMRUWidth );

				//pmain->ClientToScreen( rc );

				if( strDockSite == szTop )
					_DockControlBar(pmain, ptool, AFX_IDW_DOCKBAR_TOP, rc);
//					pmain->DockControlBar( ptool, AFX_IDW_DOCKBAR_TOP, rc );
				else if( strDockSite == szLeft )
					_DockControlBar(pmain, ptool, AFX_IDW_DOCKBAR_LEFT, rc);
//					pmain->DockControlBar( ptool, AFX_IDW_DOCKBAR_LEFT, rc );
				else if( strDockSite == szRight )
					_DockControlBar(pmain, ptool, AFX_IDW_DOCKBAR_RIGHT, rc);
//					pmain->DockControlBar( ptool, AFX_IDW_DOCKBAR_RIGHT, rc );
				else if( strDockSite == szBottom )
					_DockControlBar(pmain, ptool, AFX_IDW_DOCKBAR_BOTTOM, rc);
//					pmain->DockControlBar( ptool, AFX_IDW_DOCKBAR_BOTTOM, rc );
				else
				{
					//pmain->ScreenToClient( rc );
					//pmain->ClientToScreen( rc );
					//rc.top = max( rc.top, -GetSystemMetrics( SM_CYSMCAPTION )-2 );

					if( rc.left < rectDesktop.left )
						rc.OffsetRect( rectDesktop.left-rc.left, 0 );
					if( rc.right > rectDesktop.right )
						rc.OffsetRect( rectDesktop.right-rc.right, 0 );
					if( rc.top < rectDesktop.top )
						rc.OffsetRect( 0, rectDesktop.top-rc.top );
					if( rc.bottom > rectDesktop.bottom )
						rc.OffsetRect( 0, rectDesktop.bottom-rc.bottom );

					if( nFields >= 5 )//BT5263
					{
						ptool->m_nMRUWidth = nMRUWidth;
						ptool->CalcDynamicLayout(-1, LM_HORZ | LM_COMMIT | LM_MRUWIDTH);
					}
					else
						ptool->CalcDynamicLayout(rc.Width(), LM_HORZ | LM_COMMIT);

					pmain->FloatControlBar( ptool, rc.TopLeft() );
				}

				pmain->ShowControlBar( ptool, bVisible, TRUE );

				if( ptool->IsKindOf( RUNTIME_CLASS(CShellToolBar)))
					((CShellToolBar*)ptool)->AdjustLocations();
				ptool->SendMessage( WM_SHOWWINDOW, bVisible );
			}
		}

		idx = (int)strings.Find( szOutlookToolbars );

		if( idx != -1 )
		{
			idx++;
			int	nVisible;
			if( ::sscanf( strings[idx], "%d", &nVisible ) )
			{
				pmain->ShowControlBar( &pmain->m_wndOutlook, nVisible, TRUE );
				idx++;
			}

			while( idx < strings.GetSize() )
			{
				CString	str = strings[idx++];

				if( str.IsEmpty() || str[0] == '\'' )
					continue;
				if( str[0] == '[' )
					break;
				//toolbar here
				CString	strName = str;

				CBCGToolBar *ptool = 0;
				ptool = pmain->CreateNewOutlookBar( strName );

				while( true )
				{
					CString	strAction = strings[idx++];

					if( strAction.IsEmpty() || strAction[0] == '\'' )
						continue;

					if( strAction == szEndmarker )
					{
						break;
					}

					idx--;	//step back

					CBCGToolbarButton	*pbtn = _create_button_from_string( strings, idx, false );

					if( !pbtn )
						continue;
					ptool->InsertButton( *pbtn );

					delete pbtn;
				}
			}
			
		}


		idx = (int)strings.Find( szMenus );

		idx++;

		while( idx < strings.GetSize() )
		{
			CString	str = strings[idx++];

			if( str.IsEmpty() || str[0] == '\'' )
				continue;
			if( str[0] == '[' )
				break;

			CString	strName = str;

			MenuData	*pdata = GetMenuData( strName );
			if( !pdata )
				continue;

			HMENU hMenu = pdata->hMenu;

			if( pdata->bPopupMenu )
				hMenu = ::GetSubMenu( pdata->hMenu, 0 );
			else
			{
				if( pdata->strMenuName == "main" )
				{
					CMainFrame	*pmain = (CMainFrame *)AfxGetMainWnd();
					hMenu = pmain->m_wndMenuBar.GetDefaultMenu();
				}
				else
				{												 
					CShellDocManager	*pman = theApp.GetDocManager();
					CMultiDocTemplate	*ptempl = (CMultiDocTemplate	*)pman->GetDocTemplate( pdata->strMenuName );

					if( !ptempl )
						continue;
					hMenu = ptempl->m_hMenuShared;
				}
			}

			CBCGPopupMenuBar bar;
			//???
			bar.ImportFromMenu( hMenu );
			bool	bActionFound = false;			

			while( true )
			{
				CString	strAction = strings[idx++];

				if( strAction.IsEmpty() || strAction[0] == '\'' )
					continue;

				if( strAction == szEndmarker )
				{
					break;
				}

				//???
				if( !bActionFound )
				{
					bar.RemoveAllButtons();
					bActionFound = true;
				}

				idx--;	//step back

				CBCGToolbarButton	*pbtn = _create_button_from_string( strings, idx, true );

				if( !pbtn )
					continue;
				
				bar.InsertButton( *pbtn );

				delete pbtn;
			}

			g_menuHash.SaveMenuBar( hMenu, &bar );
		}
		//update main menu

		if( bMaximizedMode )
			pmain->m_wndMenuBar.SetMaximizeMode( false );
		pmain->m_wndMenuBar.ReloadMenu();
		if( bMaximizedMode )
			pmain->m_wndMenuBar.SetMaximizeMode( true, pmain->MDIGetActive() );
		

//////////////////////////////////////////////////
//restore controlbars settings
//////////////////////////////////////////////////
		idx = (int)strings.Find( szControlBarsNew );

		// [vanek] : забиваем на всех, кто был до этого виден - 18.10.2004
		pmain->GetDockSite( AFX_IDW_DOCKBAR_LEFT )->ResetLastVisibleDockBars( );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_TOP )->ResetLastVisibleDockBars( );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_RIGHT )->ResetLastVisibleDockBars( );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_BOTTOM )->ResetLastVisibleDockBars( );


		CTypedPtrList<CObList, CShellDockBar*>listBars;

		pos  = pmain->GetFirstDockBarPos();

		while( pos )
		{
			CControlBar	*pbar = pmain->GetNextDockBar( pos );
			if(pbar == NULL || !pbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)))
				continue;

			listBars.AddTail( (CShellDockBar*)pbar );
		}

		if( idx != -1 )
		{
			idx++;
			while( idx < strings.GetSize() )
			{
				CString	str = strings[idx++];

				if( str.IsEmpty() || str[0] == '\'' )
					continue;
				if( str[0] == '[' )
					break;

				int	idxPos = str.Find( ", " );
				if( idxPos == -1 )continue;
				CString	strBarName = str.Left( idxPos );
				str = str.Right( str.GetLength()-idxPos-1 );

				//TRACE( "\n" );
				//TRACE( strBarName );


				//find this bar
				CRect	rectPos = CRect( 0, 0, 100, 100 );
				int		nDockSite = AFX_IDW_DOCKBAR_BOTTOM;
				int		nVisible = 1;

				if( sscanf( str, "%d%d%d%d%d%d", &rectPos.left, &rectPos.top, &rectPos.right, &rectPos.bottom, &nDockSite, &nVisible )!= 6 )
					continue;

				CControlBar	*pbar = 0;

				POSITION pos = listBars.GetHeadPosition();

				while( pos )
				{
					POSITION	posSave = pos;
					CShellDockBar	*pbar1 = listBars.GetNext( pos );

					CString	strName = pbar1->GetName();

					if( strName == strBarName )
					{
						pbar = pbar1;
						listBars.RemoveAt( posSave );
						break;
					}
				}

				if( !pbar )
					continue;

				if( pbar->IsKindOf( RUNTIME_CLASS(CChildFrameBar) ) )
					continue;


				
				str = strings[idx++];
				CShellDockBar	*psbar = (CShellDockBar*)pbar;
				::sscanf( str, "(%d %d) (%d %d) (%d %d)", &psbar->m_szVert.cx, &psbar->m_szVert.cy, &psbar->m_szHorz.cx, &psbar->m_szHorz.cy, &psbar->m_szFloat.cx, &psbar->m_szFloat.cy );


				pmain->ShowControlBar( pbar, false, true );
				pbar->SendMessage( WM_SHOWWINDOW, false );

				if( !nDockSite )
				{
					//pmain->ClientToScreen( rectPos );

					if( rectPos.left < rectDesktop.left )
						rectPos.OffsetRect( rectDesktop.left-rectPos.left, 0 );
					if( rectPos.right > rectDesktop.right )
						rectPos.OffsetRect( rectDesktop.right-rectPos.right, 0 );
					if( rectPos.top < rectDesktop.top )
						rectPos.OffsetRect( 0, rectDesktop.top-rectPos.top );
					if( rectPos.bottom > rectDesktop.bottom )
						rectPos.OffsetRect( 0, rectDesktop.bottom-rectPos.bottom );

					pmain->FloatControlBar( pbar, rectPos.TopLeft() );
					pbar->GetParentFrame()->MoveWindow( rectPos );
				}
				else
				{
					// [vanek] : корректнее работает придочка докбаров (связано с обновлением стилей) - 15.10.2004
					CShellToolDockBar	*pshellDB = pmain->GetDockSite( nDockSite );           
					pshellDB->ClientToScreen(rectPos);
					pmain->DockControlBar( pbar, pshellDB, rectPos);
				}

				pmain->ShowControlBar( pbar, nVisible, true );
				pbar->SendMessage( WM_SHOWWINDOW, nVisible );
			}
		}

		//dock unused bars
		pos = listBars.GetHeadPosition();
		while( pos )
		{
			CShellDockBar	*pbar = listBars.GetNext( pos );
			CString str_name;
			pbar->GetWindowText( str_name );
			//TRACE( "\n" );
			//TRACE( str_name );
			int nDockSite = pbar->GetDefDockSide();
			if (nDockSite == AFX_IDW_DOCKBAR_FLOAT)
			{
				pmain->ShowControlBar( pbar, false, true );
				pbar->SendMessage( WM_SHOWWINDOW, false );
			}
			else
			{
				CShellToolDockBar	*pshellDB = pmain->GetDockSite( nDockSite );
				pshellDB->AddDockBar( pbar );
				pmain->ShowControlBar( pbar, false, true );
				pbar->SendMessage( WM_SHOWWINDOW, false );
			}
		}

/*




		CMainFrame* pmain = (CMainFrame*)AfxGetMainWnd();
		CControlBarInfo *pCBInfo, *pCBInfo2;
		CControlBar* pDock = 0;
		

//goto controlbars section
		idx = strings.Find( szControlBars );
		idx++;

		while( idx < strings.GetSize() )
		{
			CString	str = strings[idx++];
			pCBInfo = NULL;
			pCBInfo2 = NULL;

			if( str.IsEmpty() || str[0] == '\'' )
				continue;
			if( str[0] == '[' || str == "AnotherDockBars")
				break;
			//controlbar here
			CString	strName = str;
			bool needCB = false;

			pos = pmain->GetFirstDockBarPos();
			if (!pos)
				break;
			while(pos)
			{
				BSTR bstr;
				CControlBar		*pcbar = pmain->GetNextDockBar( pos );
				if( pcbar->IsKindOf( RUNTIME_CLASS(CShellDockBar)) )
				{
					CShellDockBar *pbar = (CShellDockBar*)pcbar;
					pbar->GetObjectName( &bstr );
					str = bstr;
					SysFreeString(bstr);
				}
				else
				if( pcbar->IsKindOf( RUNTIME_CLASS(COutlookBar)) )
					str = "OutlookBar";

				if (str == strName)
				{
					pDock = pcbar;
					needCB = true;
					break;
				}
			}


			CString	strAction = strings[idx++];
			if( strAction.IsEmpty() || strAction[0] == '\'' )
				continue;
			if( pDock->IsKindOf( RUNTIME_CLASS( COutlookBar ) ) )
				continue;

			if( strAction == szEndmarker )
			{
				break;
			}

			idx--;	//step back
			pCBInfo = _create_control_from_string( strings, idx );
			if (strings[idx++] == szDockBar)
			{
				pCBInfo2 = _create_control_from_string( strings, idx );
			}
			else
				idx--;
			if (pCBInfo && needCB)
			{
				//add to CDockState array
				if (pCBInfo->m_nBarID != pDock->GetDlgCtrlID())
				{
					pCBInfo->m_nBarID = pDock->GetDlgCtrlID();
					//pCBInfo->m_pBar = (CControlBar*)pDock;
				}
				state.ScalePoint(pCBInfo->m_pointPos);
				if (pCBInfo->m_bDocking)
				{
					state.ScaleRectPos(pCBInfo->m_rectMRUDockPos);
					state.ScalePoint(pCBInfo->m_ptMRUFloatPos);
				}
				else
				{
					CPoint	point = pCBInfo->m_ptMRUFloatPos;
				}

				if (pCBInfo->m_nBarID > 0)
					state.m_arrBarInfo.Add(pCBInfo);
				if (pCBInfo2 && pCBInfo2->m_nBarID > 0)
					state.m_arrBarInfo.Add(pCBInfo2);
			}
			CString strSizes = strings[idx++];

			if( pDock->IsKindOf( RUNTIME_CLASS(CShellDockBar)))
			{
				CShellDockBar *pbar = (CShellDockBar*)pDock;
				if (::sscanf( strSizes, "%d%d%d%d%d%d", &pbar->m_szVert.cx, &pbar->m_szVert.cy, &pbar->m_szHorz.cx, &pbar->m_szHorz.cy, &pbar->m_szFloat.cx, &pbar->m_szFloat.cy ) != 6)
					idx--;
			}
		}
		
		/*idx = strings.Find( "AnotherDockBars" );

		idx++;

		while( idx < strings.GetSize() )
		{
			CString	strAction = strings[idx++];
			pCBInfo = NULL;
			pCBInfo2 = NULL;

			if( strAction.IsEmpty() || strAction[0] == '\'' )
				continue;
			if( strAction == szEndmarker )
			{
				break;
			}
			idx--;	//step back
			pCBInfo = _create_control_from_string( strings, idx );
			if (strings[idx++] == szDockBar)
			{
				pCBInfo2 = _create_control_from_string( strings, idx );
			}
			else
				idx--;
			if (pCBInfo)
			{
				state.ScalePoint(pCBInfo->m_pointPos);
				if (pCBInfo->m_bDocking)
				{
					state.ScaleRectPos(pCBInfo->m_rectMRUDockPos);
					state.ScalePoint(pCBInfo->m_ptMRUFloatPos);
				}
				else
				{
					CPoint	point = pCBInfo->m_ptMRUFloatPos;
				}
				if (pCBInfo->m_nBarID > 0)
					state.m_arrBarInfo.Add(pCBInfo);
				else
					delete pCBInfo;
				if (pCBInfo2 && pCBInfo2->m_nBarID > 0)
					state.m_arrBarInfo.Add(pCBInfo2);
				else
					delete pCBInfo2;
			}
		}*/

		//pmain->SetDockState(state);

		/*for(int i = 0; i <= state.m_arrBarInfo.GetUpperBound(); i++)
		{
			delete state.m_arrBarInfo.GetAt(i);
		}
		state.m_arrBarInfo.RemoveAll();*/


//restore accelerators
		idx = (int)strings.Find( szKeyboard );
		idx++;

		ACCEL	*paccel;
		char strAccName[40];
		long lVirt, lKey;
		CActionInfoWrp* pinfo;
		CPtrArray ptrsAccel;
		ptrsAccel.RemoveAll();

		while( idx < strings.GetSize() )
		{
			CString	strAction = strings[idx++];
			if( strAction.IsEmpty() || strAction[0] == '\'' )
				continue;
			if( strAction == szEndmarker )
			{
				break;
			}
			idx--;	//step back
			

			if (::sscanf( strAction, "%s %d %d", strAccName, &lVirt, &lKey) == 3)
			{
				pinfo = GetActionInfo(strAccName);
				if (pinfo)
				{
					paccel = new ACCEL;
					paccel->fVirt = (BYTE)lVirt;
					paccel->key = (USHORT)lKey;
					paccel->cmd = pinfo->GetLocalID();
					ptrsAccel.Add( paccel );
				}
			}
			idx++;
		}
		//construct accelerator table
		int	nAccelSize = (int)ptrsAccel.GetSize();

		if( nAccelSize != 0 )
		{
			paccel = new ACCEL[nAccelSize];

			for( int nAccel = 0; nAccel < nAccelSize; nAccel++ )
				memcpy( &paccel[nAccel], ptrsAccel[nAccel], sizeof( ACCEL ) );

			m_hAccel = ::CreateAcceleratorTable( paccel, nAccelSize );
			m_uiAccelEntries = nAccelSize;

			delete []paccel;
		}
		for (int i = 0; i <= ptrsAccel.GetUpperBound(); i++)
		{
			delete ptrsAccel.GetAt(i);
		}
		ptrsAccel.RemoveAll();

		//paul 18.02.2003
		//set new accel table to MainFrame
		if( nAccelSize )
		{
			CMainFrame* pmain = (CMainFrame*)AfxGetMainWnd();
			pmain->SetAccelTable( m_hAccel );
		}

		pmain->GetDockSite( AFX_IDW_DOCKBAR_LEFT )->CalcFixedLayout( false, false );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_TOP )->CalcFixedLayout( false, true );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_RIGHT )->CalcFixedLayout( false, false );
		pmain->GetDockSite( AFX_IDW_DOCKBAR_BOTTOM )->CalcFixedLayout( false, true );
		pmain->EnableOutlookAnimation( bOldAnimation );

		//DocTemplate & frame depended inside frame toolbars load
		{
			POSITION pos = m_ptrTemplateInfoList.GetHeadPosition();
			while( pos )
			{
				CTemplateInfo* pi = (CTemplateInfo*)m_ptrTemplateInfoList.GetNext( pos );				
				CString strSection;
				strSection.Format( "%s Template:%s", szFrameToolBar, pi->m_strDocTemplate );
				idx = (int)strings.Find( strSection );
				if( idx != -1 )
				{
					idx++;
					while( idx < strings.GetSize() )
					{
						CString	str = strings[idx++];

						if( str.IsEmpty() || str[0] == '\'' )
							continue;
						if( str[0] == '[' )
							break;

						idx--;
						CToolBarInfo* pti = new CToolBarInfo;
						bool bres = pti->Serialize( strings, true, idx );
						if( bres )
						{
							pi->m_ptrToolbarInfoList.AddHead( pti );
						}
						else
							delete pti;
					}
				}
			}
		}
		

		// AAM: SBT1001. Возможно, избыточно
		// (см. также CActionToolsLoadSave::Invoke, CShellApp::InitInstance)
        //pmain->GetDockSite( AFX_IDW_DOCKBAR_LEFT )->CalcFixedLayout( false, false );
		//pmain->GetDockSite( AFX_IDW_DOCKBAR_TOP )->CalcFixedLayout( false, true );
		//pmain->GetDockSite( AFX_IDW_DOCKBAR_RIGHT )->CalcFixedLayout( false, false );
		//pmain->GetDockSite( AFX_IDW_DOCKBAR_BOTTOM )->CalcFixedLayout( false, true );
		// [vanek] : видимо, избыточно, - глючит ресайз добаров - 15.10.2004
	}
	_catch return false;

	CBCGToolbarButton::s_bInitProgMode = false;
	m_bStateReady = true;

	AfterStateLoad();

	return true;
}


static CRect _GetToolbarRect(CDockBar* pDockBar, CBCGToolBar *pToolBar)
{
	CRect	rc;
	if( pToolBar->IsFloating() )
		pToolBar->GetParentFrame()->GetWindowRect( rc );
	else
	{
		pToolBar->GetWindowRect(rc);
		if (pDockBar) pDockBar->ScreenToClient(rc);
	}
	return rc;
}

static const char *pstrDockSites[] = {szLeft, szTop, szRight, szBottom, szFloating};

void CCommandManager::SaveState(LPCTSTR FileName) 
{
	_try(CCommandManager, SaveState)
	 {
		 _save_state( _bstr_t(FileName), false );
	 }
	_catch_report;
}



int CCommandManager::GetDockWindowsCount()
{
	return m_compBars.GetComponentCount();
}

IUnknown *CCommandManager::GetDockWindow( int idx )
{
	return m_compBars.GetComponent( idx, false );
}

long CCommandManager::AddOutlookBar(LPCTSTR szName) 
{
	CMainFrame	*pmain = (CMainFrame*)AfxGetMainWnd();
	CBCGToolBar	*pbar = pmain->CreateNewOutlookBar( szName );

	long	idx = 0;

	for( POSITION pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext(pos);
		ASSERT_VALID(pToolBar);

		if( pbar == pToolBar )
			return idx;

		idx ++;
	}

	return -1;
}

BOOL CCommandManager::IsBarVisible(long nBarNo) 
{
	if( !CheckIdx( nBarNo ) )
		return false;

	CBCGToolBar	*ptool = GetToolBarNo( nBarNo );
	ASSERT( ptool );
	return ptool->IsWindowVisible();
}

long CCommandManager::GetButtonCount(long nBarNo) 
{
	if( !CheckIdx( nBarNo ) )
		return 0;

	CBCGToolBar	*ptool = GetToolBarNo( nBarNo );
	ASSERT( ptool );
	return ptool->GetCount();
}

BSTR CCommandManager::GetButton(long nBarNo, long nButtonNo) 
{
	CString strResult;
	
	if( !CheckIdx( nBarNo ) )
		return 0;

	CBCGToolBar	*ptool = GetToolBarNo( nBarNo );
	ASSERT( ptool );

	if( nButtonNo < 0 || nButtonNo >= ptool->GetCount() )
	{
		AfxMessageBox( "invalid button index" );
		return 0;
	}
	CBCGToolbarButton	*pbtn = ptool->GetButton( nButtonNo );
	ASSERT( pbtn );

	if( pbtn->m_nStyle == TBBS_SEPARATOR )
	{
		strResult = "-";
	}
	else 
	{
		CActionInfoWrp	*pai = GetActionInfo( pbtn->m_nID-ID_CMDMAN_BASE );
		if( pai )strResult = pai->GetActionShortName();
	}

	return strResult.AllocSysString();
}

BOOL CCommandManager::IsOutlookToolBar(long nBarNo) 
{
	if( !CheckIdx( nBarNo ) )
		return 0;

	CBCGToolBar	*ptool = GetToolBarNo( nBarNo );
	ASSERT( ptool );
	return ptool->IsKindOf( RUNTIME_CLASS( CShellOutlookToolBar ) );
}

long CCommandManager::GetDockBarsCount() 
{
	CMainFrame	*pmain = (CMainFrame*)AfxGetMainWnd();
	if( !pmain )return 0;

	int	nCount = 0;
	POSITION	pos = pmain->GetFirstDockBarPos();
	while( pos ){pmain->GetNextDockBar( pos ); nCount++;}
	return nCount;
}

BSTR CCommandManager::GetDockBarName(long nBarNo) 
{
	CString strResult;
	CShellDockBar	*pbar = GetDockBarNo( nBarNo );
	if( !pbar )return 0;

	strResult = pbar->GetName();
	return strResult.AllocSysString();
}

BOOL CCommandManager::IsDockBarVisible(long nBarNo) 
{
	CString strResult;
	CShellDockBar	*pbar = GetDockBarNo( nBarNo );
	if( !pbar )return false;

	return pbar->IsWindowVisible();
}

void CCommandManager::ShowDockBar(long nBarNo, short bVisible) 
{
	CString strResult;
	CShellDockBar	*pbar = GetDockBarNo( nBarNo );
	if( !pbar )return;

	CMainFrame	*pmain = (CMainFrame*)AfxGetMainWnd();
	pmain->ShowControlBar( pbar, bVisible, false );
	pbar->SendMessage( WM_SHOWWINDOW, bVisible );
}

void CCommandManager::SetCanCloseDockBar(long nBarNo, BOOL bCanClose)
{
	CShellDockBar* pbar = GetDockBarNo( nBarNo );
	if( !pbar )	return;

	pbar->SetCanClose( bCanClose == TRUE );
}

BOOL CCommandManager::CanCloseDockBar( long nBarNo )
{
	CShellDockBar* pbar = GetDockBarNo( nBarNo );
	if( !pbar )	return false;

	return pbar->GetCanClose( );
}

LPDISPATCH CCommandManager::GetDockBarImplByName(LPCTSTR szName)
{
	CMainFrame	*pmain = (CMainFrame*)AfxGetMainWnd();
	if( !pmain )return 0;
	POSITION pos = pmain->GetFirstDockBarPos();
	while( pos )
	{
		CShellDockBar *pDockBar = pmain->GetNextDockBar(pos);
		CString strName = pDockBar->GetName();
		if (strName == szName)
		{
			IUnknown *punkClient = pDockBar->GetClientUnknown();
			IDispatch *pDisp;
			punkClient->QueryInterface(IID_IDispatch, (void**)&pDisp);
			return pDisp;
		}
	}
	return NULL;
};

// [vanek] - 16.07.2004
void CCommandManager::SaveAsState(LPCTSTR FileName)
{
	_try(CCommandManager, SaveAsState)
	 {
		 _save_state( _bstr_t(FileName), true );
	 }
	_catch_report;            
}

// [vanek] - 20.10.2004
BOOL CCommandManager::HasAction(long BarNo, LPCTSTR ActionName)
{
	_try(CCommandManager, HasAction)
	 {
        if( !CheckIdx( BarNo ) )
			return FALSE;

		if( !_tcsclen( ActionName ) )
			return FALSE;

		int naction_cmd = -1;
		naction_cmd = GetActionCommand( (const char*)(_bstr_t(ActionName)) );
		         
		CBCGToolBar	*ptool = GetToolBarNo( BarNo );
		if( !ptool )
			return	FALSE;

		if( ptool->IsKindOf( RUNTIME_CLASS(CShellToolBar) ) )
			return ((CShellToolBar*)ptool)->GetMyCommandButtons( naction_cmd, 0 );
	 }
     _catch_report;            

	 return FALSE;
}

bool	CCommandManager::_save_state( LPCSTR lpFileName, bool bsave_as /*= false*/ )
{
//	trace("_save_state %s. MainWnd:%d\n", lpFileName, AfxGetMainWnd()->IsWindowVisible());
	if( bsave_as && !_tcslen( lpFileName ) )
		return false;
	if (!AfxGetMainWnd()->IsWindowVisible())
		return false;

	BeforStateSave();

	g_mapUserImages.RemoveAll();

	CString	strStatePathName;
	CString	strBmpPathName;
	CString	strStateDefPath;

	strStateDefPath = ::MakeAppPathName( "States" );
	strStateDefPath+= "\\";
	strStateDefPath = ::GetValueString( ::GetAppUnknown(), "Paths", "States", strStateDefPath );


	//firstly load user images
	char szDrvDef[_MAX_DRIVE], szPathDef[_MAX_PATH];
	char szDrv[_MAX_DRIVE], szPath[_MAX_PATH], szFileName[_MAX_PATH], szExt[_MAX_PATH];

	::_splitpath( lpFileName, szDrv, szPath, szFileName, szExt );
	::_splitpath( strStateDefPath, szDrvDef, szPathDef, 0, 0 );

	if( !strlen( szDrv ) )
		strcpy( szDrv, szDrvDef );
	if( !strlen( szPath ) )
		strcpy( szPath, szPathDef );
	if( !strlen( szExt ) )
		strcpy( szExt, ".state" );
	::_makepath( strStatePathName.GetBuffer( MAX_PATH ), szDrv, szPath, szFileName, szExt );
	::_makepath( strBmpPathName.GetBuffer( MAX_PATH ), szDrv, szPath, szFileName, ".tbbmp" );

	strStatePathName.ReleaseBuffer();
	strBmpPathName.ReleaseBuffer();

	m_UserImages.SetReadOnly( false );

	// [vanek] - 16.07.2004
	m_UserImages.SaveEx( strBmpPathName, bsave_as );
	if( !bsave_as )
		::SetValue( GetAppUnknown(), "General", "CurrentState", strStatePathName );
	
	CShellToolDockBar	*pleft,*ptop, *pright, *pbottom;

	CMainFrame	*pmain = (CMainFrame	*)AfxGetMainWnd();

	pleft = pmain->GetDockSite( AFX_IDW_DOCKBAR_LEFT );
	ptop = pmain->GetDockSite( AFX_IDW_DOCKBAR_TOP );
	pright = pmain->GetDockSite( AFX_IDW_DOCKBAR_RIGHT );
	pbottom = pmain->GetDockSite( AFX_IDW_DOCKBAR_BOTTOM );

	CStringArrayEx	strings;

	strings.Add( szSignature );
	strings.Add( szVersion );
	strings.Add( "" );

	strings.Add( szToolbars );

	CObList gAllToolbarsDock[5];
	for( POSITION pos = gAllToolbars.GetHeadPosition(); pos != NULL; )
	{
		CBCGToolBar* pToolBar = (CBCGToolBar*)gAllToolbars.GetNext(pos);
		if( pToolBar->IsKindOf( RUNTIME_CLASS(CBCGMenuBar) ) )
			continue;
		if( !pToolBar->IsKindOf( RUNTIME_CLASS(CShellToolBar) ) )
			continue;
		if( pToolBar->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
			continue;
		int nTypePos;
		if( pleft->FindBar( pToolBar ) >= 0 )
			nTypePos = 0;
		else if( ptop->FindBar( pToolBar ) >= 0 )
			nTypePos = 1;
		else if( pright->FindBar( pToolBar ) >= 0 )
			nTypePos = 2;
		else if( pbottom->FindBar( pToolBar ) >= 0 )
			nTypePos = 3;
		else
			nTypePos = 4;
		CObList *pList = &gAllToolbarsDock[nTypePos];
		CShellToolDockBar *pDockBar = nTypePos==0?pleft:nTypePos==1?ptop:nTypePos==2?pright:nTypePos==3?pbottom:NULL;
		CRect	rc = _GetToolbarRect(pDockBar, pToolBar);
		bool bHorz = nTypePos == 1 || nTypePos == 3 || nTypePos == 4;
		//			bool bNoMore = nTypePos == 0 || nTypePos == 1 || nTypePos == 4;
		int nPos = bHorz?rc.top:rc.left;
		bool bAdded = false;
		for (POSITION pos1 = pList->GetHeadPosition(); pos1 != NULL; )
		{
			POSITION posPrev = pos1;
			CBCGToolBar* pToolBar1 = (CBCGToolBar*)pList->GetNext(pos1);
			CRect	rc1 = _GetToolbarRect(pDockBar, pToolBar1);
			int nPos1 = bHorz?rc1.top:rc1.left;
			if (nPos < nPos1)
			{
				pList->InsertBefore(posPrev, pToolBar);
				bAdded = true;
				break;
			}
		}
		if (bAdded == false) pList->AddTail(pToolBar);
	}

	for (int nPosType = 0; nPosType < 5; nPosType++)
	{
		CObList *pList = &gAllToolbarsDock[nPosType];
		CShellToolDockBar *pDockBar = nPosType==0?pleft:nPosType==1?ptop:nPosType==2?pright:nPosType==3?pbottom:NULL;
		for( POSITION pos =pList->GetHeadPosition(); pos != NULL; )
		{
			CBCGToolBar* pToolBar = (CBCGToolBar*)pList->GetNext(pos);
			/*			if( pToolBar->IsKindOf( RUNTIME_CLASS(CBCGMenuBar) ) )
			continue;
			if( !pToolBar->IsKindOf( RUNTIME_CLASS(CShellToolBar) ) )
			continue;
			if( pToolBar->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
			continue;*/
			//extract tb data
			CString	str;
			if(::IsWindow(pToolBar->m_hWnd))
				pToolBar->GetWindowText( str );
			CString	strDockSite = pstrDockSites[nPosType];
			/*			if( pleft->FindBar( pToolBar ) >= 0 )
			strDockSite = szLeft;
			else if( ptop->FindBar( pToolBar ) >= 0 )
			strDockSite = szTop;
			else if( pright->FindBar( pToolBar ) >= 0 )
			strDockSite = szRight;
			else if( pbottom->FindBar( pToolBar ) >= 0 )
			strDockSite = szBottom;
			else
			strDockSite = szFloating;*/
			CString	strComment;
			strComment.Format( "'*** %s Toolbar - common information ***", (const char *)str );
			strings.Add( strComment );
			//1. store toolbar name
			/********************** Added by Maxim ( internal toolbars ) ***************************************/				
			int nID = pToolBar->GetDlgCtrlID();
			if( nID >= int(uiLastUserToolBarId - iMaxUserToolbars) )
				str += "#user";
			/********************** Added by Maxim ( internal toolbars ) ***************************************/				
			strings.Add( str );
			//2. Store dock state
			strings.Add( strDockSite );
			//3. Store visible state
			if( pToolBar->IsWindowVisible() )
				strings.Add( szVisible );
			else
				strings.Add( szInvisible );
			//4. Store rectangle
			CRect	rc = _GetToolbarRect(pDockBar, pToolBar);
			/*			CRect	rc;
			if( pToolBar->IsFloating() )
			pToolBar->GetParentFrame()->GetWindowRect( rc );
			else
			{
			pToolBar->GetWindowRect( rc );
			//pmain->ScreenToClient( rc );
			}*/
			str.Format( "%d %d %d %d %d", rc.left, rc.top, rc.right, rc.bottom, pToolBar->m_nMRUWidth );
			strings.Add( str );
			//5. store buttons
			strings.Add( "" );
			for( int nButton = 0; nButton < pToolBar->GetCount(); nButton++ )
			{
				CBCGToolbarButton	*pbutton = pToolBar->GetButton( nButton );

				//				ASSERT( !pbutton->IsKindOf( RUNTIME_CLASS( CShellChooseToolbarButton ) ) );
				_make_button_string( pbutton, strings );
			}
			strings.Add( szEndmarker );
			strings.Add( "" );
		}
	}

	//5.5 store outlook
	strings.Add( "" );

	strings.Add( szOutlookToolbars );

	COutlookBar	&bar = pmain->m_wndOutlook;
	if( bar.IsWindowVisible() )
		strings.Add( "1" );
	else
		strings.Add( "0" );
	strings.Add( "" );

	for( int nFolderBar = 0; nFolderBar < bar.GetFolderCount(); nFolderBar++ )
	{
		//extract tb data
		CString	str;
		str = bar.GetFolderName( nFolderBar );

		CBCGToolBar* pToolBar = (CBCGToolBar*)bar.GetFolderWindow( nFolderBar );
		if( !pToolBar->IsKindOf( RUNTIME_CLASS(CShellOutlookToolBar) ) )
			continue;

		CString	strDockSite;
		//5. store buttons
		strings.Add( str );
		strings.Add( "" );
		for( int nButton = 0; nButton < pToolBar->GetCount(); nButton++ )
		{
			CBCGToolbarButton	*pbutton = pToolBar->GetButton( nButton );
			_make_button_string( pbutton, strings );
		}
		strings.Add( szEndmarker );
		strings.Add( "" );
	}


	//6. store menus

	//test purposes
#ifdef _DEBUG		
	{
		for( int i=0;i<GetMenuCount()-1;i++ )
		{
			MenuData *pdata1 = GetMenuData( i);
			if( !pdata1 )
			{
				ASSERT( false );
				continue;
			}

			for( int j=i+1;j<GetMenuCount();j++ )
			{
				MenuData *pdata2 = GetMenuData( j );
				if( !pdata2 )
				{
					ASSERT( false );
					continue;
				}

				if( pdata1->strMenuName == pdata2->strMenuName )
				{
					ASSERT( false );
					continue;
				}
			}
		}
	}
#endif //_DEBUG


	g_menuHash.SaveMenuBar((HMENU)*pmain->m_wndMenuBar.GetMenu(), &pmain->m_wndMenuBar);
	strings.Add( szMenus );
	for( int idx = 0; idx < GetMenuCount(); idx++ )
	{
		MenuData *pdata = GetMenuData( idx );

		strings.Add( pdata->strMenuName );

		HMENU hMenu = pdata->hMenu;	

		if( pdata->bPopupMenu )
			hMenu = ::GetSubMenu( pdata->hMenu, 0 );
		else
		{
			if( pdata->strMenuName == "main" )
			{
				CMainFrame	*pmain = (CMainFrame *)AfxGetMainWnd();
				hMenu = pmain->m_wndMenuBar.GetDefaultMenu();
			}
			else
			{
				CShellDocManager	*pman = theApp.GetDocManager();
				CMultiDocTemplate	*ptempl = (CMultiDocTemplate	*)pman->GetDocTemplate( pdata->strMenuName );

				if( !ptempl )
					continue;
				hMenu = ptempl->m_hMenuShared;
			}
		}

		CBCGPopupMenuBar	bar;

		bar.ImportFromMenu( hMenu );

		g_menuHash.LoadMenuBar( hMenu, &bar );


		for( int nButton = 0; nButton < bar.GetCount(); nButton++ )
		{

			CBCGToolbarButton	*pbutton = bar.GetButton( nButton );

			_make_button_string( pbutton, strings );
		}
		strings.Add( szEndmarker );
		strings.Add( "" );

	}


	//store control bars using new method
	strings.Add( szControlBarsNew );

	CTypedPtrList<CObList, CShellDockBar*>listBars;

	pos = pmain->GetFirstDockBarPos();


	while( pos )
	{
		CControlBar	*pbar=  pmain->GetNextDockBar( pos );

		if(pbar == NULL || !pbar->IsKindOf( RUNTIME_CLASS( CShellDockBar ) ) )
			continue;
		listBars.AddTail( (CShellDockBar*)pbar );
	}
	//left dock bars
	pos = pleft->GetFirstDockBarPosition();
	while( pos )
	{
		CShellDockBar	*pbar = pleft->GetNextDockBar( pos );
		CString	strName = pbar->GetName();

		CRect	rectPos = CRect( 0, 0, 100, 100 );
		int		nDockSite = AFX_IDW_DOCKBAR_LEFT;
		int		nVisible = 1;

		rectPos = pbar->m_pDockContext->m_rectMRUDockPos;
		nVisible = pbar->IsWindowVisible();


		CString	str;
		str.Format( "%s, %d %d %d %d %d %d", (const char *)strName, 
			rectPos.left, rectPos.top, rectPos.right, rectPos.bottom, nDockSite, nVisible );

		strings.Add( str );

		str.Format( "(%d %d) (%d %d) (%d %d)", pbar->m_szVert.cx, pbar->m_szVert.cy, pbar->m_szHorz.cx, pbar->m_szHorz.cy, pbar->m_szFloat.cx, pbar->m_szFloat.cy );
		strings.Add( str );

		POSITION	posBars = listBars.Find( pbar );
		ASSERT( posBars );
		listBars.RemoveAt( posBars );
	}
	//right dock bars
	pos = pright->GetFirstDockBarPosition();
	while( pos )
	{
		CShellDockBar	*pbar = pright->GetNextDockBar( pos );
		CString	strName = pbar->GetName();


		CRect	rectPos = CRect( 0, 0, 100, 100 );
		int		nDockSite = AFX_IDW_DOCKBAR_RIGHT;
		int		nVisible = 1;

		rectPos = pbar->m_pDockContext->m_rectMRUDockPos;
		nVisible = pbar->IsWindowVisible();

		CString	str;
		str.Format( "%s, %d %d %d %d %d %d", (const char *)strName, 
			rectPos.left, rectPos.top, rectPos.right, rectPos.bottom, nDockSite, nVisible );

		strings.Add( str );

		str.Format( "(%d %d) (%d %d) (%d %d)", pbar->m_szVert.cx, pbar->m_szVert.cy, pbar->m_szHorz.cx, pbar->m_szHorz.cy, pbar->m_szFloat.cx, pbar->m_szFloat.cy );
		strings.Add( str );

		POSITION	posBars = listBars.Find( pbar );
		ASSERT( posBars );
		listBars.RemoveAt( posBars );
	}
	//top dock bars
	pos = ptop->GetFirstDockBarPosition();
	while( pos )
	{
		CShellDockBar	*pbar = ptop->GetNextDockBar( pos );
		CString	strName = pbar->GetName();

		CRect	rectPos = CRect( 0, 0, 100, 100 );
		int		nDockSite = AFX_IDW_DOCKBAR_TOP;
		int		nVisible = 1;

		rectPos = pbar->m_pDockContext->m_rectMRUDockPos;
		nVisible = pbar->IsWindowVisible();

		CString	str;
		str.Format( "%s, %d %d %d %d %d %d", (const char *)strName, 
			rectPos.left, rectPos.top, rectPos.right, rectPos.bottom, nDockSite, nVisible );

		strings.Add( str );

		str.Format( "(%d %d) (%d %d) (%d %d)", pbar->m_szVert.cx, pbar->m_szVert.cy, pbar->m_szHorz.cx, pbar->m_szHorz.cy, pbar->m_szFloat.cx, pbar->m_szFloat.cy );
		strings.Add( str );

		POSITION	posBars = listBars.Find( pbar );
		ASSERT( posBars );
		listBars.RemoveAt( posBars );
	}
	//bottom dock bars
	pos = pbottom->GetFirstDockBarPosition();
	while( pos )
	{
		CShellDockBar	*pbar = pbottom->GetNextDockBar( pos );
		CString	strName = pbar->GetName();

		CRect	rectPos = CRect( 0, 0, 100, 100 );
		int		nDockSite = AFX_IDW_DOCKBAR_BOTTOM;
		int		nVisible = 1;

		rectPos = pbar->m_pDockContext->m_rectMRUDockPos;
		nVisible = pbar->IsWindowVisible();

		CString	str;
		str.Format( "%s, %d %d %d %d %d %d", (const char *)strName, 
			rectPos.left, rectPos.top, rectPos.right, rectPos.bottom, nDockSite, nVisible );
		strings.Add( str );

		str.Format( "(%d %d) (%d %d) (%d %d)", pbar->m_szVert.cx, pbar->m_szVert.cy, pbar->m_szHorz.cx, pbar->m_szHorz.cy, pbar->m_szFloat.cx, pbar->m_szFloat.cy );
		strings.Add( str );

		POSITION	posBars = listBars.Find( pbar );
		ASSERT( posBars );
		listBars.RemoveAt( posBars );
	}

	pos = listBars.GetHeadPosition();
	while( pos )
	{
		CShellDockBar	*pbar = listBars.GetNext( pos );
		CString	strName = pbar->GetName();

		CRect	rectPos = CRect( 0, 0, 100, 100 );
		int		nDockSite = 0;
		int		nVisible = 1;

		pbar->GetParentFrame()->GetWindowRect( &rectPos );
		nVisible = pbar->IsWindowVisible();
		//pmain->ScreenToClient( rectPos );


		CString	str;
		str.Format( "%s, %d %d %d %d %d %d", (const char *)strName, 
			rectPos.left, rectPos.top, rectPos.right, rectPos.bottom, nDockSite, nVisible );
		strings.Add( str );

		str.Format( "(%d %d) (%d %d) (%d %d)", pbar->m_szVert.cx, pbar->m_szVert.cy, pbar->m_szHorz.cx, pbar->m_szHorz.cy, pbar->m_szFloat.cx, pbar->m_szFloat.cy );
		strings.Add( str );
	}

	////////////////////////////////////////////////
	//store controlbars settings
	////////////////////////////////////////////////
	/**		strings.Add( szControlBars );
	CMainFrame* pmain = (CMainFrame*)AfxGetMainWnd();
	CShellDockBar* pShellDock;
	CControlBar		*pCtrlBar;
	CString strName;
	BSTR bstrName;
	pos = pmain->GetFirstDockBarPos();

	CDockState state;
	CDockBar* pDock;
	CControlBarInfo *pCBInfo;//, *pCBInfo2;
	pmain->GetDockState(state);

	while(pos)
	{
	pCtrlBar = pmain->GetNextDockBar(pos);

	if( pCtrlBar->IsKindOf( RUNTIME_CLASS(CShellDockBar) ) )
	pShellDock = (CShellDockBar*)pCtrlBar;
	else
	pShellDock = 0;

	//Don't save ChildFrame bar
	if( pShellDock && pShellDock->IsChildFrameBar() )
	continue;

	//get object name
	if( pShellDock )
	{
	pShellDock->GetObjectName(&bstrName);
	strName = bstrName;
	::SysFreeString( bstrName );
	}
	else if( pCtrlBar->IsKindOf( RUNTIME_CLASS( COutlookBar ) ) )
	strName = "OutlookBar";
	strings.Add( strName );


	for(int i = 0; i <= state.m_arrBarInfo.GetUpperBound(); i++)
	{
	pCBInfo = (CControlBarInfo*)state.m_arrBarInfo.GetAt(i);
	if (pCtrlBar->GetDlgCtrlID() == pCBInfo->m_nBarID)
	{
	_make_controlbar_string(pCBInfo, strings);
	pDock = pCBInfo->m_pBar->m_pDockBar;
	if (pDock && strcmp(pDock->GetRuntimeClass()->m_lpszClassName, "CDockBar") == 0)
	{
	//It's floating & need save CDockBar object
	strings.Add( szDockBar );
	bool find = false;
	for(int i = 0; i <= state.m_arrBarInfo.GetUpperBound(); i++)
	{
	pCBInfo = (CControlBarInfo*)state.m_arrBarInfo.GetAt(i);
	if (pDock == pCBInfo->m_pBar)
	{
	_make_controlbar_string(pCBInfo, strings);
	find = true;
	break;
	}
	}				
	if (!find)
	strings.Add( szNotFound );
	}
	}
	}

	if( pShellDock )
	{
	CString str;
	str.Format("%d %d %d %d %d %d", pShellDock->m_szVert.cx, pShellDock->m_szVert.cy, pShellDock->m_szHorz.cx, pShellDock->m_szHorz.cy, pShellDock->m_szFloat.cx, pShellDock->m_szFloat.cy);
	strings.Add(str);
	}
	}
	//save another dockbars
	strings.Add( "AnotherDockBars" );
	for(int i = 0; i <= state.m_arrBarInfo.GetUpperBound(); i++)
	{
	pCBInfo = (CControlBarInfo*)state.m_arrBarInfo.GetAt(i);
	if (strcmp(pCBInfo->m_pBar->GetRuntimeClass()->m_lpszClassName, "CBCGToolDockBar") == 0||
	strcmp(pCBInfo->m_pBar->GetRuntimeClass()->m_lpszClassName, "COutlookBar") == 0	)
	{
	_make_controlbar_string(pCBInfo, strings);
	if (pCBInfo->m_pBar->m_pDockBar && strcmp(pCBInfo->m_pBar->m_pDockBar->GetRuntimeClass()->m_lpszClassName, "CDockBar") == 0)
	{
	strings.Add( szDockBar );
	bool find = false;
	for(int i = 0; i <= state.m_arrBarInfo.GetUpperBound(); i++)
	{
	pCBInfo = (CControlBarInfo*)state.m_arrBarInfo.GetAt(i);
	if (pDock == pCBInfo->m_pBar)
	{
	_make_controlbar_string(pCBInfo, strings);
	find = true;
	break;
	}
	}				
	if (!find)
	strings.Add( szNotFound );
	}
	}
	}
	strings.Add( szEndmarker );
	strings.Add( "" );*/

	strings.Add( szKeyboard );
	//Store  states of accelerators
	HACCEL hAccel = ((CFrameWnd*)AfxGetMainWnd())->m_hAccelTable;
	//HACCEL hAccel = GetAcceleratorTable();
	int nAccelSize = ::CopyAcceleratorTable( hAccel, NULL, 0 );

	LPACCEL lpAccel = new ACCEL [nAccelSize];
	ASSERT (lpAccel != NULL);
	::CopyAcceleratorTable( hAccel, lpAccel, nAccelSize );

	CActionInfoWrp* pActInfo;
	CString strEntire;
	bool bFind;
	for (idx = 0; idx < GetActionInfoCount(); idx++)
	{
		pActInfo = GetActionInfo(idx);
		bFind = false;
		for( int nAccel = 0; nAccel < nAccelSize; nAccel++ )
		{
			if( lpAccel[nAccel].cmd == idx + ID_CMDMAN_BASE)
			{
				strEntire.Format("%s %d %d", pActInfo->GetActionShortName(), lpAccel[nAccel].fVirt, lpAccel[nAccel].key);
				strings.Add(strEntire);
			}
		}
	}
	delete lpAccel;
	strings.Add( szEndmarker );
	strings.Add( "" );


	//store user image mapping
	strings.Add( szUserImageMapping );
	strings.Add( "" );

	POSITION	posA = g_mapUserImages.GetStartPosition();

	while( posA )
	{
		CString	strA;
		long	l;

		g_mapUserImages.GetNextAssoc( posA, strA, l );
		CString	str;
		str.Format( "%s=%d", (const char *)strA, l );
		strings.Add( str );
	}

	strings.Add( szEndmarker );
	strings.Add( "" );


	//DocTemplate & frame depended inside frame toolbars store
	{
		POSITION pos = m_ptrTemplateInfoList.GetHeadPosition();
		while( pos )
		{
			CTemplateInfo* pi = (CTemplateInfo*)m_ptrTemplateInfoList.GetNext( pos );				
			if( pi->m_ptrToolbarInfoList.GetCount() > 0 )
			{
				CString strSection;
				strSection.Format( "%s Template:%s", szFrameToolBar, pi->m_strDocTemplate );

				strings.Add( strSection );

				POSITION posTB = pi->m_ptrToolbarInfoList.GetHeadPosition();
				while( posTB )
				{
					CToolBarInfo* ptbi = (CToolBarInfo*)pi->m_ptrToolbarInfoList.GetNext( posTB );
					int nInd = 0;
					ptbi->Serialize( strings, false, nInd );
				}
			}
		}
	}




	strings.WriteFile( strStatePathName );		

	return true;
}

long CCommandManager::FindDockNo(LPCTSTR pszName) 
{
	CMainFrame	*pmain = (CMainFrame*)AfxGetMainWnd();
	if( !pmain )return -1;
	POSITION	pos = pmain->GetFirstDockBarPos();
	int	nCount = 0;

	while( pos )
	{
		CString	strWindowTitle;
		CShellDockBar	*pbar = pmain->GetNextDockBar( pos );
		strWindowTitle = pbar->GetName();

		if( !strWindowTitle.CompareNoCase( pszName ) )
			return nCount;
		nCount ++;
	}

	return -1;
}

void CCommandManager::InitUserImages()
{
	//m_UserImages.DeleteAllImages();

	HBITMAP	hbmp = (HBITMAP)::LoadImage( AfxGetInstanceHandle(), 
		MAKEINTRESOURCE(IDR_USERIMAGES), 
		IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE|LR_LOADMAP3DCOLORS );
	m_UserImages.AddImage( hbmp );
	m_UserImages.SetUserFlag( true );
}

CActionControlWrp	*CCommandManager::GetActionControl( UINT nID )
{
	for( int i = 0; i < m_ptrsControl.GetSize(); i++ )
	{
		ActionControlData	*pac = (ActionControlData	*)m_ptrsControl[i];
		if( pac->m_nLocalID == nID )
		{
			CActionControlWrp	*p = new CActionControlWrp;
			p->Init( pac->m_bstrProgID );
			return p;
		}

	}
	return 0;
}


/////////////////////////////////////////////////////////////////////////////////////////
LongOperationAction CCommandManager::SetLongOperationActionState( LongOperationAction loa )
{
	LongOperationAction loaOld = m_loActionState;
	m_loActionState = loa;

	/*CString str = "loaState:";
	if( m_loActionState == loaStart )
		str += "loaStart";
	else
	if( m_loActionState == loaSetPos )
		str += "loaSetPos";
	else
	if( m_loActionState == loaTerminate )
		str += "loaTerminate";
	else
	if( m_loActionState == loaComplete )
		str += "loaComplete";
	   
	str += "\n";

	TRACE( str );	*/
		
		
	return loaOld;
}

/////////////////////////////////////////////////////////////////////////////////////////
LongOperationAction CCommandManager::GetLongOperationActionState()
{
	return m_loActionState;
}


///////////////////////////////////////////////////////////////////////////////
//ActionInfo wrapper
CActionInfoWrp::CActionInfoWrp( IUnknown *punk )
{
	if( punk->QueryInterface( IID_IActionInfo2, (void **)&m_pActionInfo ) != S_OK )
		m_pActionInfo = 0;
	m_nBitmapIdx = -1;
	m_lPictureIdx = -1;
	m_pBmpData = 0;
}
CActionInfoWrp::~CActionInfoWrp()
{
	ASSERT( m_pActionInfo );
	m_pActionInfo->Release();
}

	//load information from action
bool CActionInfoWrp::Init()
{
	if( !m_pActionInfo )
		return false;

	BSTR	bstrName = 0, bstrUserName = 0, bstrHelpString = 0, bstrGroupName = 0;

	VERIFY( m_pActionInfo->GetCommandInfo( &bstrName, 0, 0, 0 ) == 0 );
	if (bstrName == 0)
		return false;

	if( g_CmdManager.m_ptrAliases != 0 )
	{
		BSTR	bstrActionString = 0;
		if( g_CmdManager.m_ptrAliases->GetActionString( bstrName, &bstrActionString ) == S_OK )
		{
			m_pActionInfo->SetActionString( bstrActionString );
			::SysFreeString( bstrActionString );
		}
	}
	
	//get the general information
	VERIFY( m_pActionInfo->GetCommandInfo( 0, &bstrUserName, &bstrHelpString, &bstrGroupName ) == 0 );
	if( bstrName )m_sName = bstrName;
	if( bstrUserName )m_sUserName = bstrUserName;
	if( bstrHelpString )m_sToolTip = bstrHelpString;
	if( bstrGroupName )m_sGroupName = bstrGroupName;


	::SysFreeString( bstrName );
	::SysFreeString( bstrUserName );
	::SysFreeString( bstrHelpString );
	::SysFreeString( bstrGroupName );
	//
	BSTR	bstrMenus;
	DWORD	dwFlag;

	VERIFY( m_pActionInfo->GetMenuInfo( &bstrMenus, &dwFlag ) == 0 );

	CString	sMenus = bstrMenus;
	int idx = 0;

	if( !sMenus.IsEmpty() )
	{
		for( ; idx != -1;  )
		{
			idx = sMenus.Find( "\n" );
			CString	s;

			if( idx == -1 )
				s = sMenus;
			else
				s = sMenus.Left( idx );
			sMenus = sMenus.Right( sMenus.GetLength() - idx - 1 );

			m_sMenus.Add( s );
		}
	}

	//load bitmap informations
	{
		BSTR	bstrToolBarName;
		IUnknown *punkBms = 0;

		DWORD	dwCmdCode, dwDllCode;

 		VERIFY( m_pActionInfo->GetTBInfo( &bstrToolBarName, &punkBms ) == S_OK );
		VERIFY( m_pActionInfo->GetRuntimeInformaton( &dwDllCode, &dwCmdCode ) == S_OK );

		if( punkBms )
		{
			sptrIBitmapProvider	sptrBP( punkBms );
			sptrBP->GetBmpIndexFromCommand( dwCmdCode, dwDllCode, &m_lPictureIdx, &m_nBitmapIdx );
			
			INumeredObjectPtr	sptrN( punkBms );
			sptrN->GetKey( &m_lBitmapProviderKey );

			punkBms->Release();
		}

		if( bstrToolBarName )
		{
			m_sToolBars.Add( CString( bstrToolBarName ) );
			::SysFreeString( bstrToolBarName );
		}
	}
	return true;
}
			   

int CActionInfoWrp::GetLocalID()
{
	int	nID = -1;
	m_pActionInfo->GetLocalID( &nID );
	return nID;

}

void CActionInfoWrp::SetLocalID( int nID )
{
	m_pActionInfo->SetLocalID( nID );
}


/////////////////////////////////////////////////////////////////////////////
void CCommandManager::DestroyTemplateInfoList()
{
	POSITION pos = m_ptrTemplateInfoList.GetHeadPosition();
	while( pos )
	{
		delete (CTemplateInfo*)m_ptrTemplateInfoList.GetNext( pos );
	}

	m_ptrTemplateInfoList.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void CCommandManager::CreateTemplateInfoList()
{

	DestroyTemplateInfoList();

	IApplicationPtr ptrApp( ::GetAppUnknown() );
	if( ptrApp == NULL )
		return;

	LONG_PTR lPos = 0;
	ptrApp->GetFirstDocTemplPosition( &lPos );
	while( lPos )
	{
		IUnknown* punk = 0;
		BSTR bstrName = 0;
		ptrApp->GetNextDocTempl( &lPos, &bstrName, &punk );
		if( punk ) 
			punk->Release(); punk = 0;

		CTemplateInfo* pti = new CTemplateInfo;
		pti->m_strDocTemplate = bstrName;		

		m_ptrTemplateInfoList.AddTail( pti );

		::SysFreeString( bstrName );
	}
		
}

/////////////////////////////////////////////////////////////////////////////
void CCommandManager::AfterStateLoad()
{
	CWnd	*pwnd = AfxGetMainWnd();
	if( !pwnd->IsKindOf( RUNTIME_CLASS(CMainFrame) ) )
		return;
	CMainFrame *pmain = (CMainFrame *)pwnd;
	POSITION pos = pmain->GetFirstChildFramePos();
	while( pos )
	{
		CShellFrame* pFrame = pmain->GetNextChildFrame( pos );
		CreateShellFrameToolbars( pFrame );
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCommandManager::BeforStateSave()
{
	GatherShellFrameToolbarsInfo( NULL );
}

bool CCommandManager::IsFrameCanHaveToolbar( CShellFrame* pFrame )
{
	CString strDocTemplate, strFrameType;
	if( !GetFrameOptions( pFrame, strDocTemplate, strFrameType ) )
	{
		return false;
	}

	strDocTemplate = "\\" + strDocTemplate;

	if( ::GetValueInt( ::GetAppUnknown(), strDocTemplate, "EnableFrameToolbars", 0 ) != 1 )
		return false;

	CString strEnableFrameType;
	strEnableFrameType = ::GetValueString( ::GetAppUnknown(), strDocTemplate, "FrameTypeSupportedToolbars", "PDI" );

	FrameType ft = pFrame->GetFrameType();

	if( ft == ftMDI )
		return strEnableFrameType.Find( "MDI" ) != -1;
	else
	if( ft == ftDDI )
		return strEnableFrameType.Find( "DDI" ) != -1;		
	else
	if( ft == ftPDI )
		return strEnableFrameType.Find( "PDI" ) != -1;		

	return false;
}

/////////////////////////////////////////////////////////////////////////////
CTemplateInfo* CCommandManager::FindTemplateInfo( CString strDocTemplate )
{
	if( strDocTemplate.IsEmpty() )
		return NULL;

	
	if( ::GetValueInt( ::GetAppUnknown(), strDocTemplate, "EnableFrameToolbars", 0 ) != 1 )
		return NULL;

	CTemplateInfo* pti = NULL;
	POSITION pos = m_ptrTemplateInfoList.GetHeadPosition();
	while( pos )
	{
		CTemplateInfo* pi = (CTemplateInfo*)m_ptrTemplateInfoList.GetNext( pos );				
		if( pi->m_strDocTemplate == strDocTemplate )
			return pi;
	}

	return NULL;
}

/////////////////////////////////////////////////////////////////////////////
bool CCommandManager::GetFrameOptions( CShellFrame* pFrame, CString& strDocTemplate, CString& strFrameType )
{
	if( !pFrame )
		return false;

	FrameType ft = pFrame->GetFrameType();

	if( ft == ftMDI )
		strFrameType = "MDI";
	else
	if( ft == ftDDI )
		strFrameType = "DDI";
	else
	if( ft == ftPDI )
		strFrameType = "PDI";

	if( strFrameType.IsEmpty() )
		return false;


	CDocument* pDoc = pFrame->GetActiveDocument();
	if( !pDoc )
		return false;

	IDocumentSitePtr ptrDS( pDoc->GetControllingUnknown() );
	if( ptrDS == NULL )
		return false;

	IUnknown* punkDT = 0;
	ptrDS->GetDocumentTemplate( &punkDT );
	if( !punkDT )
		return false;

	IDocTemplatePtr ptrDT( punkDT );
	punkDT->Release();

	if( ptrDT == NULL )
		return false;

	BSTR bstr = 0;			 
	ptrDT->GetDocTemplString( CDocTemplate::docName, &bstr );
	strDocTemplate = bstr;
	::SysFreeString( bstr );

	return true;
}

/////////////////////////////////////////////////////////////////////////////
void CCommandManager::GatherShellFrameToolbarsInfo( CShellFrame* pFrameExcept )
{
	CWnd	*pwnd = AfxGetMainWnd();
	if( !pwnd->IsKindOf( RUNTIME_CLASS(CMainFrame) ) )
		return;
	CMainFrame *pmain = (CMainFrame *)pwnd;
	POSITION pos = pmain->GetFirstChildFramePos();
	while( pos )
	{
		CShellFrame* pFrame = pmain->GetNextChildFrame( pos );
		if( pFrame == pFrameExcept )
			continue;

		UpdateShellFrameToolbarInfo( pFrame );
	}

	
}

/////////////////////////////////////////////////////////////////////////////
void CCommandManager::UpdateShellFrameToolbarInfo( CShellFrame* pFrame )
{	
	if( !IsFrameCanHaveToolbar( pFrame ) )
		return;


	CString strDocTemplate, strFrameType;
	if( !GetFrameOptions( pFrame, strDocTemplate, strFrameType ) )
		return;

	CTemplateInfo* pti = FindTemplateInfo( strDocTemplate );

	if( !pti )
	{
		return;
	}

	pti->DestroyList();

	POSITION pos = pFrame->GetFirstToolBarPos();
	while( pos )
	{
		CShellFrameToolBar* ptb = pFrame->GetNextToolBar( pos );
		CToolBarInfo* ptbInfo = new CToolBarInfo;
		ptbInfo->CreateContext( ptb, pFrame );
		pti->m_ptrToolbarInfoList.AddHead( ptbInfo );		
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCommandManager::CreateShellFrameToolbars( CShellFrame* pFrame )
{

	if( !pFrame )
		return;

	pFrame->DestroyAllToolBars();

	if( !IsFrameCanHaveToolbar( pFrame ) )
		return;


	CString strDocTemplate, strFrameType;
	if( !GetFrameOptions( pFrame, strDocTemplate, strFrameType ) )
		return;

	CString strToolbarName;
	strToolbarName.Format( "%s %s", strDocTemplate, "Toolbar" );		


	CTemplateInfo* pti = FindTemplateInfo( strDocTemplate );

	if( !pti )
	{
		return;
	}

	POSITION pos = pti->m_ptrToolbarInfoList.GetHeadPosition();
	while( pos )
	{
		CToolBarInfo* pbi = (CToolBarInfo*)pti->m_ptrToolbarInfoList.GetNext( pos );		
		pFrame->CreateToolBar( pbi );
	}


	if( pti->m_ptrToolbarInfoList.GetCount() <= 0 )
	{
		CToolBarInfo ti( strToolbarName, NORECT, AFX_IDW_DOCKBAR_TOP );		
		pFrame->CreateToolBar( &ti );			
	}



}

/////////////////////////////////////////////////////////////////////////////
//
//	class CTemplateInfo
//
//
/////////////////////////////////////////////////////////////////////////////
CTemplateInfo::CTemplateInfo()
{
	Init();
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateInfo::Init()
{
	m_strDocTemplate.Empty();	
}

/////////////////////////////////////////////////////////////////////////////
CTemplateInfo::~CTemplateInfo()
{
	DestroyList();
}

/////////////////////////////////////////////////////////////////////////////
void CTemplateInfo::DestroyList()
{
	POSITION pos = m_ptrToolbarInfoList.GetHeadPosition();
	while( pos )
	{
		CToolBarInfo* pi = (CToolBarInfo*)m_ptrToolbarInfoList.GetNext( pos );
		delete pi;
	}

	m_ptrToolbarInfoList.RemoveAll();

}
/////////////////////////////////////////////////////////////////////////////
//
//	class CToolBarInfo
//
//
/////////////////////////////////////////////////////////////////////////////

CToolBarInfo::CToolBarInfo( CString strName, CRect rc, UINT uiDocPos )
{
	Init();
	
	if( !strName.IsEmpty() )
		m_strName = strName;

	m_bVisible = true;
	m_rect = rc;	
	m_uiDocPos = uiDocPos;
	m_bFloating = false;
	m_arBtnString.RemoveAll();
}


CToolBarInfo::CToolBarInfo()
{
	Init();
}

/////////////////////////////////////////////////////////////////////////////
CToolBarInfo::~CToolBarInfo()
{

}

/////////////////////////////////////////////////////////////////////////////
void CToolBarInfo::Init()
{
	m_strName = " ";
	m_bVisible = false;
	m_rect = NORECT;	
	m_uiDocPos = AFX_IDW_DOCKBAR_TOP;
	m_bFloating = false;
	m_arBtnString.RemoveAll();
}

/////////////////////////////////////////////////////////////////////////////
void CToolBarInfo::CreateContext( CBCGToolBar* ptb, CFrameWnd* pParent )
{
	Init();

	if( !ptb )
		return;
	
	if( !pParent )
		return;

	CControlBar* pleft = pParent->GetControlBar( AFX_IDW_DOCKBAR_LEFT );
	CControlBar* ptop = pParent->GetControlBar( AFX_IDW_DOCKBAR_TOP );
	CControlBar* pright = pParent->GetControlBar( AFX_IDW_DOCKBAR_RIGHT );
	CControlBar* pbottom = pParent->GetControlBar( AFX_IDW_DOCKBAR_BOTTOM );
	
	ptb->GetWindowText( m_strName );
	m_bVisible = ( ptb->IsWindowVisible() ? true : false );	


	CDockBar* pDocLeft = 0;
	CDockBar* pDocTop = 0;
	CDockBar* pDocRight = 0;
	CDockBar* pDocBottom = 0;

	if( pleft && pleft->IsKindOf( RUNTIME_CLASS(CDockBar) ) )
		pDocLeft = (CDockBar*)pleft;

	if( pright && pright->IsKindOf( RUNTIME_CLASS(CDockBar) ) )
		pDocRight = (CDockBar*)pright;

	if( ptop && ptop->IsKindOf( RUNTIME_CLASS(CDockBar) ) )
		pDocTop = (CDockBar*)ptop;

	if( pbottom && pbottom->IsKindOf( RUNTIME_CLASS(CDockBar) ) )
		pDocBottom = (CDockBar*)pbottom;

	if( pDocLeft && pDocLeft->FindBar( ptb ) >= 0 )
		m_uiDocPos = AFX_IDW_DOCKBAR_LEFT;
	else if( pDocTop && pDocTop->FindBar( ptb ) >= 0 )
		m_uiDocPos = AFX_IDW_DOCKBAR_TOP;
	else if( pDocRight && pDocRight->FindBar( ptb ) >= 0 )
		m_uiDocPos = AFX_IDW_DOCKBAR_RIGHT;
	else if( pDocBottom && pDocBottom->FindBar( ptb ) >= 0 )
		m_uiDocPos = AFX_IDW_DOCKBAR_BOTTOM;
	else
		m_bFloating = true;


	CRect	rc;
	if( ptb->IsFloating() )
		ptb->GetParentFrame()->GetWindowRect( rc );
	else
	{
		ptb->GetWindowRect( rc );		
	}

	pParent->ScreenToClient( rc );
	m_rect = rc;

	for( int nButton = 0; nButton < ptb->GetCount(); nButton++ )
	{
		CBCGToolbarButton	*pb = ptb->GetButton( nButton );
		_make_button_string( pb, m_arBtnString );
	}
}

/////////////////////////////////////////////////////////////////////////////
bool CToolBarInfo::Serialize( CStringArrayEx& strings, bool bLoad, int& idx )
{
	if( bLoad )
	{
		Init();
		//toolbar here
		CString	str = strings[idx++];

		CString	strName = str;
		CString	strDockSite = strings[idx++];
		CString	strVisible = strings[idx++];
		CString	strWindowRect = strings[idx++];

		bool bVisible = strVisible.CompareNoCase( szVisible ) == 0;

		while( true )
		{
			CString	strAction = strings[idx++];

			if( strAction.IsEmpty() || strAction[0] == '\'' )
				continue;

			if( strAction == szEndmarker )
			{
				break;
			}

			idx--;	//step back

			CBCGToolbarButton	*pbtn = _create_button_from_string( strings, idx, false );

			if( !pbtn )
				continue;

			_make_button_string( pbtn, m_arBtnString );
			delete pbtn;
		}

		CRect	rc;
		::sscanf( strWindowRect, "%d %d %d %d", &m_rect.left, &m_rect.top, &m_rect.right, &m_rect.bottom );

		if( strDockSite == szTop )
			m_uiDocPos = AFX_IDW_DOCKBAR_TOP;				
		else
		if( strDockSite == szLeft )
			m_uiDocPos = AFX_IDW_DOCKBAR_LEFT;				
		else
		if( strDockSite == szRight )
			m_uiDocPos = AFX_IDW_DOCKBAR_RIGHT;				
		else
		if( strDockSite == szBottom )
			m_uiDocPos = AFX_IDW_DOCKBAR_BOTTOM;
		else
			m_bFloating = true;
	}
	else//storing
	{
		//1. store toolbar name
		strings.Add( m_strName );
		//2. Store dock state
		CString strDockSite;

		if( m_bFloating )
			strDockSite = szFloating;
		else
		if( m_uiDocPos == AFX_IDW_DOCKBAR_LEFT )		
			strDockSite = szLeft;
		else 
		if( m_uiDocPos == AFX_IDW_DOCKBAR_TOP )
			strDockSite = szTop;
		else 
		if( m_uiDocPos == AFX_IDW_DOCKBAR_RIGHT )
			strDockSite = szRight;
		else 
		if( m_uiDocPos == AFX_IDW_DOCKBAR_BOTTOM )
			strDockSite = szBottom;		

		strings.Add( strDockSite );
		//3. Store visible state
		if( m_bVisible )
			strings.Add( szVisible );
		else
			strings.Add( szInvisible );
		//4. Store rectangle
		CString str;
					
		str.Format( "%d %d %d %d", m_rect.left, m_rect.top, m_rect.right, m_rect.bottom );
		strings.Add( str );
		//5. store buttons
		strings.Add( "" );
		for( int i=0;i<m_arBtnString.GetSize();i++ )
		{
			strings.Add( m_arBtnString[i] );
		}

		strings.Add( szEndmarker );
		strings.Add( "" );
	}
	

	return true;	
}
