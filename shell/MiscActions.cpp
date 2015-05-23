#include "stdafx.h"
#include "resource.h"
#include "MiscActions.h"

#include "CommandManager.h"
#include "misc.h"
#include "mainfrm.h"
#include "ShellButton.h"

#include "PopupInt.h"
#include "shell.h"
#include "ShellDockBar.h"


//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionToolsLoadSave, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionToolsStateSave, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionToolsCustomize, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionToolsLoadSaveEx, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionToolsStateSaveAs, CCmdTargetEx);


//[ag]4. olecreate release

// {78635F77-17F3-11d3-A5CA-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionToolsLoadSave, "Shell.ToolsLoadSave",
0x78635f77, 0x17f3, 0x11d3, 0xa5, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {78635F73-17F3-11d3-A5CA-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionToolsStateSave, "Shell.ToolsStateSave",
0x78635f73, 0x17f3, 0x11d3, 0xa5, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

// {DB2C2908-0BF8-4bc0-A661-CB8C64EDD356}
GUARD_IMPLEMENT_OLECREATE(CActionToolsLoadSaveEx, "Shell.ToolsLoadSaveEx",
0xdb2c2908, 0xbf8, 0x4bc0, 0xa6, 0x61, 0xcb, 0x8c, 0x64, 0xed, 0xd3, 0x56);

// {17801133-14E7-11d3-A5C8-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionToolsCustomize, "Shell.ToolsCustomize", 
0x17801133, 0x14e7, 0x11d3, 0xa5, 0xc8, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);

// {D7A4002D-9D6B-48a4-B40B-855975CCCAF3}
GUARD_IMPLEMENT_OLECREATE(CActionToolsStateSaveAs, "Shell.ToolsStateSaveAs", 
0xd7a4002d, 0x9d6b, 0x48a4, 0xb4, 0xb, 0x85, 0x59, 0x75, 0xcc, 0xca, 0xf3);


//[ag]5. guidinfo release

// {78635F75-17F3-11d3-A5CA-0000B493FF1B}
static const GUID guidToolsLoadSave =
{ 0x78635f75, 0x17f3, 0x11d3, { 0xa5, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {78635F71-17F3-11d3-A5CA-0000B493FF1B}
static const GUID guidToolsStateSave =
{ 0x78635f71, 0x17f3, 0x11d3, { 0xa5, 0xca, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {17801134-14E7-11d3-A5C8-0000B493FF1B}
static const GUID guidToolsCustomizeInfo = 
{ 0x17801134, 0x14e7, 0x11d3, { 0xa5, 0xc8, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };

// {28C69C80-4429-48d4-A91C-6978B67E1B5D}
static const GUID guidToolsLoadSaveEx =
{ 0x28c69c80, 0x4429, 0x48d4, { 0xa9, 0x1c, 0x69, 0x78, 0xb6, 0x7e, 0x1b, 0x5d } };

// {A0DAFF12-ED2F-4130-852F-685880016345}
static const GUID guidToolsStateSaveAs = 
{ 0xa0daff12, 0xed2f, 0x4130, { 0x85, 0x2f, 0x68, 0x58, 0x80, 0x1, 0x63, 0x45 } };


//[ag]6. action info

ACTION_INFO_FULL(CActionToolsLoadSave, IDS_ACTION_TOOLS_LOAD_STATE, IDS_MENU_TOOLS, -1, guidToolsLoadSave);
ACTION_INFO_FULL(CActionToolsStateSave, IDS_ACTION_TOOLS_SAVE_STATE, IDS_MENU_TOOLS, -1, guidToolsStateSave);
ACTION_INFO_FULL(CActionToolsCustomize, IDS_ACTION_TOOLS_CUSTOMIZE, IDS_MENU_TOOLS, -1, guidToolsCustomizeInfo);
ACTION_INFO_FULL(CActionToolsLoadSaveEx, IDS_ACTION_TOOLS_LOAD_STATE_EX, IDS_MENU_TOOLS, -1, guidToolsLoadSaveEx);
ACTION_INFO_FULL(CActionToolsStateSaveAs, IDS_ACTION_TOOLS_SAVEAS_STATE, IDS_MENU_TOOLS, -1, guidToolsStateSaveAs);


//[ag]7. targets

//[ag]8. arguments
ACTION_ARG_LIST(CActionToolsLoadSave)
	ARG_STRING("FileName", 0)
END_ACTION_ARG_LIST()

//[ag]8. arguments
ACTION_ARG_LIST(CActionToolsLoadSaveEx)
	ARG_STRING("FileName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionToolsStateSave)
	ARG_STRING("FileName", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionToolsStateSaveAs)
	ARG_STRING("FileName", 0)
END_ACTION_ARG_LIST()


//CActionToolsCustomize
bool CActionToolsCustomize::ExecuteSettings( CWnd *pwndParent )
{
	if( ForceNotShowDialog() )
		return false;

	CBCGToolbarCustomize* pDlgCust = new CBCGToolbarCustomize( (CFrameWnd*)AfxGetMainWnd(), false );

	for( int i = 0; i < g_CmdManager.GetActionInfoCount(); i++ )
	{
		CActionInfoWrp *pinfo = g_CmdManager.GetActionInfo( i );

		if( !pinfo )
			continue;

		if( !strcmp( pinfo->GetActionGroupName(), "---" ) )
			continue;


		IUnknown	*punkUI  = 0;
		if( CheckInterface( pinfo->m_pActionInfo, IID_IActionInfo2 ) )
		{
			IActionInfo2Ptr	ptrAI = pinfo->m_pActionInfo;
			ptrAI->GetUIUnknown( &punkUI );
		}


		CCommandManager::BitmapData	*pBitmapData = g_CmdManager.GetBitmapData( pinfo );

		int	iBitmapIdx = -1;

		if( pBitmapData )
			iBitmapIdx =  pinfo->GetPictureIdx() + pBitmapData->lImagesOffset;
		
		
		if( CheckInterface( punkUI, IID_IUIComboBox ))
		{
			CShellMenuComboBoxButton	btn(
							pinfo->GetLocalID(),
							iBitmapIdx,
							punkUI,
							CBS_DROPDOWNLIST,
							220);
			
			btn.m_strText = pinfo->GetActionUserName();
			btn.m_bText = TRUE;

			pDlgCust->AddButton( pinfo->GetActionGroupName(), btn );
			punkUI->Release();
		}
		else/*		
		if( CheckInterface( punkUI, IID_ISubMenu ))
		{
			CShellPopupMenuButon	btn(
							pinfo->GetLocalID(),
							iBitmapIdx,
							punkUI,
							pinfo->GetActionUserName(), 
							FALSE );
			
			btn.m_strText = pinfo->GetActionUserName();
			btn.m_bText = TRUE;

			pDlgCust->AddButton( pinfo->GetActionGroupName(), btn );
			punkUI->Release();
		}
		else  */
		if( punkUI )
		{
			CShellMenuButton	btn(
							pinfo->GetLocalID(),
							iBitmapIdx,
							punkUI,
							pinfo->GetActionUserName(), 
							FALSE );

			pDlgCust->AddButton( pinfo->GetActionGroupName(), btn );
			punkUI->Release();
		}
		else
		{
			CBCGToolbarButton	btn(
							pinfo->GetLocalID(),
							iBitmapIdx,
							pinfo->GetActionUserName(), 
							FALSE );

			pDlgCust->AddButton( pinfo->GetActionGroupName(), btn );
		}

	}

	CString	strNewMenu = ::LanguageLoadCString( IDS_NEW_MENU_CUST );
	//strNewMenu.LoadString( IDS_NEW_MENU_CUST );
	
	CString	strDefGroup = ::LanguageLoadCString( IDS_DEFAULT_GROUP_CUST );
	//strDefGroup.LoadString( IDS_DEFAULT_GROUP_CUST );

	pDlgCust->AddButton( strDefGroup, CShellToolbarButton(0, -1, strNewMenu));
	


	pDlgCust->SetUserCategory ("User");

	pDlgCust->EnableUserDefinedToolbars ();
	pDlgCust->Create ();

	MSG *msgCur = 0;

#if _MSC_VER >= 1300
	msgCur = AfxGetCurrentMessage();
#else
	msgCur = &AfxGetApp()->m_msgCur;
#endif

	bool	bIdle = false;
	long	lIdleCount = 0;
		// acquire and dispatch messages until a WM_QUIT message is received.
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
				return 0;

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

			if( !CBCGToolBar::IsCustomizeMode() )
				break;

		} while (::PeekMessage(msgCur, NULL, NULL, NULL, PM_NOREMOVE));
		if( !CBCGToolBar::IsCustomizeMode() )
			break;
	}

	return true;
}

bool CActionToolsCustomize::Invoke()
{
	return true;
}

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionToolsLoadSave implementation
static void CmdManLoadState(CString strFileName)
{
	if (!g_CmdManager._load_state(strFileName))
	{
		if( GetValueInt( GetAppUnknown(), "\\General", "BackupStates", 0 ) )
		{
			char	szDrv[_MAX_DRIVE], szPath[_MAX_PATH], szName[_MAX_PATH], szExt[_MAX_PATH];
			::_splitpath( strFileName, szDrv, szPath, szName, szExt );
			CString	strNewName = szName;
			strNewName += "_backup";
			CString	strBackupName;
			::_makepath( strBackupName.GetBuffer( _MAX_PATH ), szDrv, szPath, strNewName, szExt );
			strBackupName.ReleaseBuffer();
			CopyFile( strBackupName, strFileName, FALSE );
			CString strTbbmpFile;
			::_makepath( strTbbmpFile.GetBuffer( _MAX_PATH ), szDrv, szPath, szName, _T(".tbbmp"));
			::_makepath( strBackupName.GetBuffer( _MAX_PATH ), szDrv, szPath, strNewName, _T(".tbbmp"));
			CopyFile(strBackupName, strTbbmpFile, FALSE);
			strTbbmpFile.ReleaseBuffer();
			strBackupName.ReleaseBuffer();

			g_CmdManager._load_state(strFileName);
		}
	}
}


CActionToolsLoadSave::CActionToolsLoadSave()
{
}

CActionToolsLoadSave::~CActionToolsLoadSave()
{
}

bool CActionToolsLoadSave::ExecuteSettings( CWnd *pwndParent )
{
	m_strFileName = GetArgumentString( "FileName" );

	if( ForceNotShowDialog() )
	{
		if( m_strFileName.IsEmpty() )
			m_strFileName = "default.state";
		return true;
	}

	if( m_strFileName.IsEmpty() || ForceShowDialog() )
	{
		CString	str = ::LanguageLoadCString( IDS_FILTER_STATE );
		//str.LoadString( IDS_FILTER_STATE );

		CString	strLast = ::GetValueString( ::GetAppUnknown(), "General", "CurrentState", "default.state" );
		CFileDialog	dlg( TRUE, ".state", strLast,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, 0, _FILE_OPEN_SIZE_ );
																								   
		if( dlg.DoModal() == IDOK )
			m_strFileName = dlg.GetPathName();
		else
			return false;

		SetArgument( "FileName", COleVariant( m_strFileName ) );
	}

	return true;
}

bool CActionToolsLoadSave::Invoke()
{
 	CMainFrame* pframe = GetMainFrame();

	if( CBCGToolBar::m_bPressed )
	{
		int nLen = m_strFileName.GetLength();

		char *szFile = new char[nLen + 1];
		::strcpy( szFile, m_strFileName );

		pframe->PostMessage( WM_USER + 54321, (WPARAM)szFile, nLen  );
		return true;
	}


	bool bold_lock = pframe->GetLockRecalc( );
	pframe->SetLockRecalc( true );

//   	g_CmdManager.LoadState( m_strFileName );
	CmdManLoadState(m_strFileName);
	pframe->SetLockRecalc( bold_lock );
	if( !bold_lock )
	{	// AAM: все, кроме RecalcLayout, добавлено в рамках фикса SBT1001. Возможно, избыточно
		// (см. также CCommandManager::LoadState, CShellApp::InitInstance)
		theApp.OnIdle( 0 );
		pframe->GetDockSite( AFX_IDW_DOCKBAR_LEFT )->CalcFixedLayout( false, false );
		pframe->GetDockSite( AFX_IDW_DOCKBAR_TOP )->CalcFixedLayout( false, true );
		pframe->GetDockSite( AFX_IDW_DOCKBAR_RIGHT )->CalcFixedLayout( false, false );
		pframe->GetDockSite( AFX_IDW_DOCKBAR_BOTTOM )->CalcFixedLayout( false, true );

		// [vanek] - 15.10.2004
		pframe->RecalcLayout();
	}


	return true;
}
//////////////////////////////////////////////////////////////////////
//CActionToolsLoadSaveEx implementation
CActionToolsLoadSaveEx::CActionToolsLoadSaveEx()
{
}

CActionToolsLoadSaveEx::~CActionToolsLoadSaveEx()
{
}

bool CActionToolsLoadSaveEx::ExecuteSettings( CWnd *pwndParent )
{
	m_strFileName = GetArgumentString( "FileName" );

	if( ForceNotShowDialog() )
	{
		if( m_strFileName.IsEmpty() )
			m_strFileName = "default.state";
		return true;
	}

	if( m_strFileName.IsEmpty() || ForceShowDialog() )
	{
		CString	str = ::LanguageLoadCString( IDS_FILTER_STATE );
		//str.LoadString( IDS_FILTER_STATE );

		CString	strLast = ::GetValueString( ::GetAppUnknown(), "General", "CurrentState", "default.state" );
		CFileDialog	dlg( TRUE, ".state", strLast,  OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, 0, _FILE_OPEN_SIZE_ );
																								   
		if( dlg.DoModal() == IDOK )
			m_strFileName = dlg.GetPathName();
		else
			return false;

		SetArgument( "FileName", COleVariant( m_strFileName ) );
	}

	return true;
}

bool CActionToolsLoadSaveEx::Invoke()
{
 	CMainFrame* pframe = GetMainFrame();

	bool bold_lock = pframe->GetLockRecalc( );
	pframe->SetLockRecalc( true );

	CmdManLoadState(m_strFileName);
//   	g_CmdManager.LoadState( m_strFileName );
	pframe->SetLockRecalc( bold_lock );
	if( !bold_lock )
		pframe->RecalcLayout();


	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionToolsStateSave implementation
static void BackupState(CString strFileName)
{
	if( GetValueInt( GetAppUnknown(), "\\General", "BackupStates", 0 ) )
	{
		char	szDrv[_MAX_DRIVE], szPath[_MAX_PATH], szName[_MAX_PATH], szExt[_MAX_PATH];
		::_splitpath( strFileName, szDrv, szPath, szName, szExt );
		CString	strNewName = szName;
		strNewName += "_backup";
		CString	strBackupName;
		::_makepath( strBackupName.GetBuffer( _MAX_PATH ), szDrv, szPath, strNewName, szExt );
		strBackupName.ReleaseBuffer();
		CopyFile( strFileName, strBackupName, FALSE );
		CString strTbbmpFile;
		::_makepath( strTbbmpFile.GetBuffer( _MAX_PATH ), szDrv, szPath, szName, _T(".tbbmp"));
		::_makepath( strBackupName.GetBuffer( _MAX_PATH ), szDrv, szPath, strNewName, _T(".tbbmp"));
		CopyFile(strTbbmpFile, strBackupName, FALSE);
		strTbbmpFile.ReleaseBuffer();
		strBackupName.ReleaseBuffer();
	}
}

CActionToolsStateSave::CActionToolsStateSave()
{
}

CActionToolsStateSave::~CActionToolsStateSave()
{
}

bool CActionToolsStateSave::ExecuteSettings( CWnd *pwndParent )
{
	m_strFileName = GetArgumentString( "FileName" );

	if( ForceNotShowDialog() )
	{
		if( m_strFileName.IsEmpty() )
			m_strFileName = "default.state";
		return true;
	}

	if( m_strFileName.IsEmpty() || ForceShowDialog() )
	{
		CString	str = ::LanguageLoadCString( IDS_FILTER_STATE );
		//str.LoadString( IDS_FILTER_STATE );

		CString	strLast = ::GetValueString( ::GetAppUnknown(), "General", "CurrentState", "default.state" );
		CFileDialog	dlg( FALSE, ".state", strLast, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, 0, _FILE_OPEN_SIZE_ );


		if( dlg.DoModal() == IDOK )
			m_strFileName = dlg.GetPathName();
		else
			return false;

		SetArgument( "FileName", COleVariant( m_strFileName ) );
	}

	return true;
}

bool CActionToolsStateSave::Invoke()
{
	BackupState(m_strFileName);
	g_CmdManager.SaveState( m_strFileName );
	//GetMainFrame()->m_strLastStateFileName = m_strFileName;

	return true;
}

//////////////////////////////////////////////////////////////////////
//CActionToolsStateSave implementation
CActionToolsStateSaveAs::CActionToolsStateSaveAs()
{
}

CActionToolsStateSaveAs::~CActionToolsStateSaveAs()
{
}

bool CActionToolsStateSaveAs::ExecuteSettings( CWnd *pwndParent )
{
	m_strFileName = GetArgumentString( "FileName" );

	if( ForceNotShowDialog() )
	{
		if( m_strFileName.IsEmpty() )
			m_strFileName = "default.state";
		return true;
	}

	if( m_strFileName.IsEmpty() || ForceShowDialog() )
	{
		CString	str = ::LanguageLoadCString( IDS_FILTER_STATE );
		//str.LoadString( IDS_FILTER_STATE );

		CString	strLast = ::GetValueString( ::GetAppUnknown(), "General", "CurrentState", "default.state" );
		CFileDialog	dlg( FALSE, ".state", strLast, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, str, 0, _FILE_OPEN_SIZE_ );


		if( dlg.DoModal() == IDOK )
			m_strFileName = dlg.GetPathName();
		else
			return false;

		SetArgument( "FileName", COleVariant( m_strFileName ) );
	}

	return true;
}

bool CActionToolsStateSaveAs::Invoke()
{
/*	if( GetValueInt( GetAppUnknown(), "General", "BackupStates", 0 ) )
	{
		char	szDrv[_MAX_DRIVE], szPath[_MAX_PATH], szName[_MAX_PATH], szExt[_MAX_PATH];
		::_splitpath( m_strFileName, szDrv, szPath, szName, szExt );
		CString	strNewName = szName;
		strNewName += "_backup";
		CString	strBackupName;
		::_makepath( strBackupName.GetBuffer( _MAX_PATH ), szDrv, szPath, strNewName, szExt );
		strBackupName.ReleaseBuffer();

		try{
			CStdioFile	file( m_strFileName, CFile::typeText|CFile::modeRead );
			CStringArrayEx	sae;
			sae.ReadFile( &file );
			sae.WriteFile( strBackupName );
		}
		catch( CException *pe )
		{
			pe->Delete();
		}
	}*/
	BackupState(m_strFileName);
	g_CmdManager.SaveAsState( m_strFileName );
	return true;
}