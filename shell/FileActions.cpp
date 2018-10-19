#include "stdafx.h"
#include "shell.h"
#include "FileActions.h"
#include "resource.h"
#include "docs.h"
#include "dialogs.h"
#include "shelldoc.h"
#include "shellview.h"
#include "undoint.h"
#include "Input.h"
#include "\vt5\ifaces\aview_int.h"
#include "EnterNameDlg.h"
#include "resource.h"

class CLastFileMenu : public CCmdTargetEx
{
	DECLARE_DYNCREATE(CLastFileMenu);
protected:
	CLastFileMenu()
	{
		m_nCurrentBase = -1;
	}
	DECLARE_INTERFACE_MAP();
	BEGIN_INTERFACE_PART(Menu,IUIDropDownMenu)
		com_call AppendMenuItems( HMENU hMenu, UINT nCommandBase );
	 	com_call ExecuteCommand( UINT nCommand );
	END_INTERFACE_PART(Menu)

	int	m_nCurrentBase;
};

IMPLEMENT_DYNCREATE(CLastFileMenu, CCmdTargetEx)
IMPLEMENT_UNKNOWN(CLastFileMenu, Menu)

BEGIN_INTERFACE_MAP(CLastFileMenu, CCmdTargetEx)
	INTERFACE_PART(CLastFileMenu, IID_IUIDropDownMenu, Menu )
END_INTERFACE_MAP()

HRESULT CLastFileMenu::XMenu::AppendMenuItems( HMENU hMenu, UINT nCommandBase )
{
	METHOD_PROLOGUE_EX(CLastFileMenu, Menu)

	pThis->m_nCurrentBase = nCommandBase;
	CRecentFileList	*plist = theApp.GetRecentFileList();

	int	n = 0;
	bool	bOneAdded = false;

	for( n = 0; n < plist->GetSize(); n++ )
	{
		CString	str;
		char	szCurDir[255];
		::GetCurrentDirectory( 255, szCurDir );
		plist->GetDisplayName( str, n, szCurDir, (int)strlen( szCurDir ) );


		if( !str.IsEmpty() )
		{
			::AppendMenu( hMenu, MF_STRING, nCommandBase+n, str );
			bOneAdded = true;
		}
	}

	if( !bOneAdded )
	{
		CString	strNoRecent = ::LanguageLoadCString( IDS_NO_RECENT_FILES );
		//strNoRecent.LoadString( IDS_NO_RECENT_FILES );

		::AppendMenu( hMenu, MF_STRING|MF_GRAYED|MF_DISABLED, nCommandBase, strNoRecent );
	}

	CString	strOpen = ::LanguageLoadCString( IDS_FILE_OPEN );
	//strOpen.LoadString( IDS_FILE_OPEN );

	::AppendMenu( hMenu, MF_SEPARATOR, 0, 0 );
	::AppendMenu( hMenu, MF_STRING, nCommandBase+n, strOpen );

	return S_OK;
}


HRESULT CLastFileMenu::XMenu::ExecuteCommand( UINT nCommand )
{
	METHOD_PROLOGUE_EX(CLastFileMenu, Menu)

	ASSERT(pThis->m_nCurrentBase!=-1);
	nCommand-=pThis->m_nCurrentBase;
	CRecentFileList	*plist = theApp.GetRecentFileList();

	if( nCommand >= 0 && nCommand < (UINT)plist->GetSize() )
	{
		CString	strFileName = (*plist)[nCommand];

		ExecuteAction( "FileOpen", CString( "\"" )+strFileName+CString( "\"" ) );

		return S_OK;
	}
	else
		ExecuteAction( "FileOpen" );

	return S_FALSE;
	
}



//[ag]1. dyncreate

IMPLEMENT_DYNCREATE(CActionFileCloseAll, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileExit, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileOpen, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileNew, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileSave, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileSaveAs, CActionFileSave);
IMPLEMENT_DYNCREATE(CActionFileClose, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileSendEMail, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileSaveView, CCmdTargetEx);
IMPLEMENT_DYNCREATE(CActionFileSendView, CActionFileSaveView);
IMPLEMENT_DYNCREATE(CActionDumpMem, CCmdTargetEx);

//[ag]4. olecreate release


// {DDD58983-1F2B-11d3-A5D3-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionFileCloseAll, "Shell.FileCloseAll",
0xddd58983, 0x1f2b, 0x11d3, 0xa5, 0xd3, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {D9A99282-087E-11d3-A5B0-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionFileExit, "Shell.FileExit", 
0xd9a99282, 0x87e, 0x11d3, 0xa5, 0xb0, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {5BF1D1E4-0CF7-11d3-A5BA-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionFileOpen, "Shell.FileOpen", 
0x5bf1d1e4, 0xcf7, 0x11d3, 0xa5, 0xba, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {5BF1D1E6-0CF7-11d3-A5BA-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionFileNew, "Shell.FileNew", 
0x5bf1d1e6, 0xcf7, 0x11d3, 0xa5, 0xba, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {47F74303-0EC8-11d3-A5BC-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionFileSave, "Shell.FileSave", 
0x47f74303, 0xec8, 0x11d3, 0xa5, 0xbc, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {7B8E8A32-141D-11d3-A5C7-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionFileSaveAs, "Shell.FileSaveAs", 
0x7b8e8a32, 0x141d, 0x11d3, 0xa5, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {83383E54-1403-11d3-A5C7-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CActionFileClose, "Shell.FileClose", 
0x83383e54, 0x1403, 0x11d3, 0xa5, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b);
// {50942194-E5B5-48b3-B521-9655BB79F28B}
GUARD_IMPLEMENT_OLECREATE(CActionFileSendEMail, "Shell.FileSendEMail", 
0x50942194, 0xe5b5, 0x48b3, 0xb5, 0x21, 0x96, 0x55, 0xbb, 0x79, 0xf2, 0x8b);
// {CE7827A1-0DBA-40a0-8F5C-64B193326E1D}
GUARD_IMPLEMENT_OLECREATE(CActionFileSaveView, "Shell.FileSaveView", 
0xce7827a1, 0xdba, 0x40a0, 0x8f, 0x5c, 0x64, 0xb1, 0x93, 0x32, 0x6e, 0x1d);
// {4E9D181F-7229-4003-A904-8C052E64DE88}
GUARD_IMPLEMENT_OLECREATE(CActionFileSendView, "Shell.FileSendView", 
0x4e9d181f, 0x7229, 0x4003, 0xa9, 0x4, 0x8c, 0x5, 0x2e, 0x64, 0xde, 0x88);
// {BA69F78D-2981-4c9e-91BA-AD9D43B81E03}
GUARD_IMPLEMENT_OLECREATE(CActionDumpMem, "Shell.DumpMem", 
0xba69f78d, 0x2981, 0x4c9e, 0x91, 0xba, 0xad, 0x9d, 0x43, 0xb8, 0x1e, 0x3);

//[ag]5. guidinfo release

// {DDD58981-1F2B-11d3-A5D3-0000B493FF1B}
static const GUID guidFileCloseAll =
{ 0xddd58981, 0x1f2b, 0x11d3, { 0xa5, 0xd3, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {D9A99284-087E-11d3-A5B0-0000B493FF1B}
static const GUID guidFileExitInfo = 
{ 0xd9a99284, 0x87e, 0x11d3, { 0xa5, 0xb0, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {5BF1D1E2-0CF7-11d3-A5BA-0000B493FF1B}
static const GUID guidFileOpenInfo = 
{ 0x5bf1d1e2, 0xcf7, 0x11d3, { 0xa5, 0xba, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {5BF1D1E8-0CF7-11d3-A5BA-0000B493FF1B}
static const GUID guidFileNewInfo = 
{ 0x5bf1d1e8, 0xcf7, 0x11d3, { 0xa5, 0xba, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {47F74305-0EC8-11d3-A5BC-0000B493FF1B}
static const GUID guidFileSaveInfo = 
{ 0x47f74305, 0xec8, 0x11d3, { 0xa5, 0xbc, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {7B8E8A34-141D-11d3-A5C7-0000B493FF1B}
static const GUID guidFileSaveAsInfo = 
{ 0x7b8e8a34, 0x141d, 0x11d3, { 0xa5, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {83383E55-1403-11d3-A5C7-0000B493FF1B}
static const GUID guidFileCloseInfo = 
{ 0x83383e56, 0x1403, 0x11d3, { 0xa5, 0xc7, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b } };
// {91929364-A3A0-4b12-B011-6A4A690E83B5}
static const GUID guidFileSendEMailInfo = 
{ 0x91929364, 0xa3a0, 0x4b12, { 0xb0, 0x11, 0x6a, 0x4a, 0x69, 0xe, 0x83, 0xb5 } };
// {61A46A91-0F58-4878-B252-47A6980DFE1B}
static const GUID guidFileSaveView = 
{ 0x61a46a91, 0xf58, 0x4878, { 0xb2, 0x52, 0x47, 0xa6, 0x98, 0xd, 0xfe, 0x1b } };
// {AEC27CA7-4962-497f-881F-670C3EA3FC06}
static const GUID guidFileSendView = 
{ 0xaec27ca7, 0x4962, 0x497f, { 0x88, 0x1f, 0x67, 0xc, 0x3e, 0xa3, 0xfc, 0x6 } };
// {9B8E8472-1ACF-447b-BD30-C7EB1A56A70F}
static const GUID guidDumpMem = 
{ 0x9b8e8472, 0x1acf, 0x447b, { 0xbd, 0x30, 0xc7, 0xeb, 0x1a, 0x56, 0xa7, 0xf } };

//[ag]6. action info

ACTION_INFO_FULL(CActionFileCloseAll, IDS_ACTION_FILE_CLOSE_ALL, IDS_MENU_FILE_DOC, -1, guidFileCloseAll);
ACTION_INFO_FULL(CActionFileNew, IDS_ACTION_FILE_NEW, IDS_MENU_FILE, IDS_TB_FILE, guidFileNewInfo);
ACTION_INFO_FULL(CActionFileOpen, IDS_ACTION_FILE_OPEN, IDS_MENU_FILE, IDS_TB_FILE, guidFileOpenInfo);
ACTION_INFO_FULL(CActionFileExit, IDS_ACTION_FILE_EXIT, IDS_MENU_FILE, IDS_TB_FILE, guidFileExitInfo);
ACTION_INFO_FULL(CActionFileSave, IDS_ACTION_FILE_SAVE, IDS_MENU_FILE_DOC, IDS_TB_FILE, guidFileSaveInfo);
ACTION_INFO_FULL(CActionFileSaveAs, IDS_ACTION_FILE_SAVEAS, IDS_MENU_FILE_DOC, IDS_TB_FILE, guidFileSaveAsInfo);
ACTION_INFO_FULL(CActionFileClose, IDS_ACTION_FILE_CLOSE, IDS_MENU_FILE_DOC, -1, guidFileCloseInfo);
ACTION_INFO_FULL(CActionFileSendEMail, IDS_ACTION_FILE_SENDEMAIL, IDS_MENU_FILE_DOC, -1, guidFileSendEMailInfo);
ACTION_INFO_FULL(CActionFileSaveView, IDS_ACTION_FILE_SAVEVIEW, IDS_MENU_FILE_DOC, -1, guidFileSaveView);
ACTION_INFO_FULL(CActionFileSendView, IDS_ACTION_FILE_SENDVIEW, IDS_MENU_FILE_DOC, -1, guidFileSendView);
ACTION_INFO_FULL(CActionDumpMem, IDS_ACTION_DUMP_MEM, -1, -1, guidDumpMem);

//[ag]7. targets

ACTION_TARGET(CActionFileSave, "anydoc")
ACTION_TARGET(CActionFileSaveAs, "anydoc")
ACTION_TARGET(CActionFileSendEMail, "anydoc")
ACTION_TARGET(CActionFileSaveView, "view site")
ACTION_TARGET(CActionFileSendView, "view site")
ACTION_TARGET(CActionDumpMem, "app")
//ACTION_TARGET(CActionFileClose, "anydoc")

//[ag]8. arguments

ACTION_ARG_LIST(CActionFileNew)
	ARG_STRING("Template", 0)
	RES_STRING("NewDocName")
	ARG_INT("DialogMode", -1)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionFileOpen)
	ARG_STRING("FileName", 0)
	ARG_STRING("Template", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionFileSave)
	ARG_STRING("FileName", 0)
	ARG_STRING("DocumentTitle", 0)
END_ACTION_ARG_LIST()

ACTION_ARG_LIST(CActionFileClose)
	ARG_STRING("DocumentTitle", 0)
END_ACTION_ARG_LIST()

ACTION_UI(CActionFileOpen, CLastFileMenu)

//[ag]9. implementation

//////////////////////////////////////////////////////////////////////
//CActionFileCloseAll implementation
CActionFileCloseAll::CActionFileCloseAll()
{
}

CActionFileCloseAll::~CActionFileCloseAll()
{
}

bool CActionFileCloseAll::Invoke()
{
	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();

	POSITION posTempl = pman->GetFirstDocTemplatePosition();

	while( posTempl )
	{
		CDocTemplate *ptempl = pman->GetNextDocTemplate( posTempl );
		POSITION	posDoc = ptempl->GetFirstDocPosition();

		while( posDoc )
		{
			CDocument	*pdoc = ptempl->GetNextDoc( posDoc );
			if( !pdoc->SaveModified() )return false;
			pdoc->OnCloseDocument();
		}
	}

	return true;
}

bool CActionFileExit::Invoke()
{
	CShellDocManager	*pman = theApp.GetDocManager();
	if( pman && !pman->SaveAllModified() )
		return false;

	AfxGetMainWnd()->PostMessage( WM_COMMAND, ID_APP_EXIT );
	return true;
}

DWORD  SeekWaitingProc(LPVOID param)
{
	PD* pd = (PD*)param;
    TCHAR strPathName[1000];
	_tcscpy(strPathName, pd->path);
	WPARAM wParamCancel = MAKEWPARAM(IDCANCEL,NM_CLICK);
	WPARAM wParamOK = MAKEWPARAM(IDOK,NM_CLICK);	

	SHFILEINFO sfi ={0};
	if(pd->bFile && !PathFileExists(strPathName)/*!SHGetFileInfo(strPathName, 0, &sfi, sizeof(SHFILEINFO),SHGFI_DISPLAYNAME) || *sfi.szDisplayName ==0*/)
	{	
		::PostMessage(pd->hWnd, WM_COMMAND, wParamCancel, 0);
		return 0;
	}
	else if(!pd->bFile && !PathIsDirectory(strPathName))
	{
		::PostMessage(pd->hWnd, WM_COMMAND, wParamCancel, 0);
		return 0;
	}
	::PostMessage(pd->hWnd, WM_COMMAND, wParamOK, 0);	
	return 1;
}
BOOL EnshureCanAccess(CString path)
{
	PD pd;
	pd.bFile = TRUE;
	CString dirPath;
	if(path.IsEmpty()) 
	{
		dirPath = GetValueString(::GetAppUnknown(), "FileOpen", "LastPath", "");
		if(dirPath.IsEmpty()) 
		{
			//SetValue(::GetAppUnknown(), "FileOpen", "LastPath", GetValueString(::GetAppUnknown(), "Paths", "CurrentPath", ""));
			return TRUE;
		}
		pd.bFile = FALSE;
	}
	if(pd.bFile) pd.path = path.GetBuffer();
	else pd.path = dirPath.GetBuffer();

	CWaitCursor wc;
	CWaitDlg wd;
	wd.SetPath(pd.path, pd.bFile);


	wd.Create(MAKEINTRESOURCE( IDD_WAITDLG), 0);
	pd.hWnd =wd.m_hWnd;
	DWORD thId;
	HANDLE  h = CreateThread(0,0,(LPTHREAD_START_ROUTINE)SeekWaitingProc, &pd, 0, &thId);

	DWORD dwStart = GetTickCount();
	DWORD ec =STILL_ACTIVE;
	bool bTimeOut =false;
	while(GetExitCodeThread(h, &ec) && ec==STILL_ACTIVE)
	{
		if(GetTickCount()- dwStart>3000)
		{
			bTimeOut =true;
			break;
		}
		Sleep(10);
	}
	CloseHandle(h);
	if(bTimeOut)
	{
		wd.ShowWindow(SW_SHOW);
		if(wd.RunModalLoop(MLF_NOIDLEMSG |MLF_SHOWONIDLE)==IDOK) return TRUE;
		else 
		{
			if(pd.bFile)
			{
				CString s;
				s.Format(IDS_CANT_OPEN, path);
				AfxMessageBox(s, MB_OK | MB_ICONERROR, 0);
			}
			else 
			{
				SetValue(::GetAppUnknown(), "FileOpen", "LastPath", GetValueString(::GetAppUnknown(), "Paths", "CurrentPath", ""));
				return TRUE;
			}
			return FALSE;
		}
	}
	else 
	{
		wd.DestroyWindow();
		if(!ec)
		{
			if(pd.bFile)
			{
				CString s;
				s.Format(IDS_CANT_OPEN, path);
				AfxMessageBox(s, MB_OK | MB_ICONERROR, 0);
			}
			else 
			{
				SetValue(::GetAppUnknown(), "FileOpen", "LastPath", GetValueString(::GetAppUnknown(), "Paths", "CurrentPath", ""));
				return TRUE;
			}
		}
		return ec;
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////
//FileOpen
bool CActionFileOpen::ExecuteSettings( CWnd *pwndParent )
{
	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();
	CString	strPathName = GetArgumentString( "FileName" );

	try
	{//Kir - enshure file can be accessed
		if(!EnshureCanAccess(strPathName))
		{
			return false;
		}

	}catch(...)
	{
		return false;
	}

	CString	strDocTemplate = GetArgumentString( "Template" );

	CShellDocTemplate	*pdocTempl = 0;
	
	if( !strDocTemplate.IsEmpty() )
		pdocTempl = pman->GetDocTemplate( strDocTemplate );

	

	if( !pdocTempl && !strDocTemplate.IsEmpty() )
	{
		AfxMessageBox( IDS_BAD_DOCUMENT_TEMPLATE );
		return false;
	}
//no settings required - every argument specified
	if( pdocTempl && !strPathName.IsEmpty() ) 
		return true;

	if( !pdocTempl && !strPathName.IsEmpty() ) 
	{
		pdocTempl = pman->GetMatchDocTemplate( strPathName );
		if( !pdocTempl )
		{
			AfxMessageBox( IDS_CANT_FIND_DOCUMENT_TEMPLATE );
			return false;
		}
		strDocTemplate = pdocTempl->GetTemplateName();
		SetArgument( "Template", COleVariant( strDocTemplate ) );

		return true;
	}

	if( m_dwExecuteSettings == aefNoShowInterfaceAnyway && strPathName.IsEmpty() )
		return false;

	if( !pman->DoPromptFileName( strPathName, IDS_FILE_OPEN_DLG_TITLE, 0, TRUE, pdocTempl ) )
		return false;


	if( strDocTemplate.IsEmpty() ) 
	{
		pdocTempl = pman->GetMatchDocTemplate( strPathName );

		if( !pdocTempl )
		{
			AfxMessageBox( IDS_CANT_FIND_DOCUMENT_TEMPLATE );
			return false;
		}
		strDocTemplate = pdocTempl->GetTemplateName();
	}

	SetArgument( "FileName", COleVariant( strPathName ) );
	SetArgument( "Template", COleVariant( strDocTemplate ) );

	return true;
}

bool CActionFileOpen::Invoke()
{
//	CWaitCursor	wait;

	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();

	CString	strPathName = GetArgumentString( "FileName" );

	CString	strDocTemplate = GetArgumentString( "Template" );

	//02.10.2002 mb file alredy open? then only activate them
	IApplicationPtr ptr_app( GetAppUnknown() );
	if( ptr_app )
	{
		LONG_PTR	lposTempl = 0;

		ptr_app->GetFirstDocTemplPosition( &lposTempl );

		while( lposTempl )
		{
			LONG_PTR	lposDoc = 0;
			ptr_app->GetFirstDocPosition( lposTempl, &lposDoc );


			while( lposDoc )
			{
				IUnknown	*punk_doc = 0;
				ptr_app->GetNextDoc( lposTempl, &lposDoc, &punk_doc );
				if( punk_doc )
				{
					IDocumentSitePtr ptr_ds( punk_doc );
					punk_doc->Release();	punk_doc = 0;
					if( ptr_ds )
					{
						BSTR bstr = 0;
						ptr_ds->GetPathName( &bstr );

						CString str_path = bstr;

						if( bstr )
							::SysFreeString( bstr );	bstr = 0;

						if( !stricmp( str_path, strPathName ) )
						{
							// 23.06.2003 - активизация вновь разрешена
							CString str = str_path.Right( str_path.GetLength() - str_path.ReverseFind( '\\' ) - 1 );
							ExecuteAction( "WindowActivate", CString( "\"" )+str+CString( "\"" ) );
							return true;
						}
					}
				}
			}
			ptr_app->GetNextDocTempl( &lposTempl, 0, 0 );
		}
	}

	CDocTemplate	*pdocTempl = pman->GetDocTemplate( strDocTemplate );

	if( !pdocTempl )
		//bad document type specified
		return false;

	CShellDoc	*pdoc = (CShellDoc*)pdocTempl->OpenDocumentFile( strPathName );
	if( !pdoc )
		return false;

	IUndoListPtr ptr_undo_list( pdoc->GetControllingUnknown() );
	if( ptr_undo_list )
		ptr_undo_list->ClearUndoRedoList();

	pdoc->FirenNewDocument( (const char*)strPathName );

	CShellView	*pview = (CShellView*)pdoc->GetActiveView();
	if( !pview )return true;

	
	IFrameWindowPtr	sptrFrame(((CMDIChildWnd*)pview->GetFrame())->GetControllingUnknown());
	if(sptrFrame == 0)return true;

	IUnknown* punkSplitter = 0;
	sptrFrame->GetSplitter(&punkSplitter);
	ISplitterWindowPtr	sptrSplitter(punkSplitter);
	if(punkSplitter)
		punkSplitter->Release();
	if(sptrSplitter == 0)
		return true;

	sptrSplitter->OptimizeSplitter();		
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//FileNew
bool CActionFileNew::ExecuteSettings( CWnd *pwndParent )
{
	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();

	CString	strDocTemplate = GetArgumentString( "Template" );

	CDocTemplate	*pdocTempl = pman->GetDocTemplate( strDocTemplate );

//no settings required - document template specified
	if( pdocTempl )
		return true;

	if( !pdocTempl && !strDocTemplate.IsEmpty() )
	{
		AfxMessageBox( IDS_BAD_DOCUMENT_TEMPLATE );
		return false;
	}

	CNewTypeDlg dlg;

	if( dlg.DoModal() != IDOK )
		return false;
	pdocTempl = dlg.m_pSelectedTemplate;

	ASSERT_KINDOF( CShellDocTemplate, pdocTempl);

	strDocTemplate = ((CShellDocTemplate*)pdocTempl)->GetTemplateName();

	SetArgument( "Template", COleVariant( strDocTemplate ) );

	return true;
}

bool CActionFileNew::Invoke()
{
	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();

	CString	strDocTemplate = GetArgumentString( "Template" );
	int nDialogMode = GetArgumentInt("DialogMode");

	IDocCreaterPtr sptrDC;
	if (nDialogMode > -1)
	{
		CString strCreator(_T("DocCreators."));
		strCreator += strDocTemplate;
		strCreator += _T("DocCreator");
		sptrDC.CreateInstance(strCreator, NULL, CLSCTX_INPROC_SERVER);
		if (sptrDC == 0 || FAILED(sptrDC->ExecuteSettings(AfxGetMainWnd()->m_hWnd,_bstr_t(strDocTemplate),
			nDialogMode)))
			return false;
	}

	CDocTemplate	*pdocTempl = pman->GetDocTemplate( strDocTemplate );

	ASSERT( pdocTempl );

	CDocument *pdoc = pdocTempl->OpenDocumentFile( 0, sptrDC == 0 );

	if( !pdoc )
		return false;

	IUndoListPtr ptr_undo_list( pdoc->GetControllingUnknown() );
	if( ptr_undo_list )
		ptr_undo_list->ClearUndoRedoList();
	
	CString	strNewDocTitle = pdoc->GetTitle();

	SetResult( "NewDocName", &COleVariant( strNewDocTitle ) );

	if (sptrDC != 0)
		sptrDC->InitDocument(pdoc->GetControllingUnknown());

	((CShellDoc*)pdoc)->FirenNewDocument( 0 );

	if (sptrDC != 0) // was created initially hidden
	{
		POSITION pos = pdoc->GetFirstViewPosition();
		while (pos)
		{
			CView *pview = pdoc->GetNextView(pos);
			CFrameWnd *pFrame = pview->GetParentFrame();
			pFrame->ShowWindow(SW_SHOW);
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//FileSave


bool CActionFileSave::IsNeedPrompt()
{
	if( !m_pdoc )
		return false;

	CString sExt;
	bool bRet = ::IsNeedPrompt(m_strFileName, m_pdoc, &sExt);
	if (bRet && !sExt.IsEmpty() && !m_strFileName.IsEmpty())
	{
		TCHAR szPath[_MAX_PATH],szDrive[_MAX_DRIVE],szDir[_MAX_DIR],szFName[_MAX_FNAME];
		_tsplitpath(m_strFileName, szDrive, szDir, szFName, NULL);
		_tmakepath(szPath, szDrive, szDir, szFName, sExt);
		m_strFileName = szPath;
	}
	return bRet;

/*	sptrIDocument sptrDoc( m_pdoc->GetControllingUnknown() );
	if( sptrDoc == NULL )
		return false;

	CShellDocTemplate* pdocTemplate = (CShellDocTemplate*)m_pdoc->GetDocTemplate();
//get file filter by filename
	IFileFilter *punk  = 0;
	if( pdocTemplate->GetFileFilter( m_strFileName, true, &punk ) == CDocTemplate::noAttempt )
	{
		if( punk )
		{
			punk->Release();
			punk = 0;
		}

		return false;
	}

	if( !punk ) //native format
	{
		return false;
	}

	punk->Release();
	punk = 0;


	IDataContext2Ptr	ptrContext( sptrDoc );
	
	long	lObjectsCount = 0;

	long lTypesCount = 0;
	ptrContext->GetObjectTypeCount( &lTypesCount );

	for( LPOS lPos=0; lPos<lTypesCount; lPos++ )
	{
		BSTR bstrType = 0;
		ptrContext->GetObjectTypeName( lPos, &bstrType );
		
		long lCount = 0;
		ptrContext->GetObjectCount( bstrType, &lCount );
		::SysFreeString( bstrType );

		lObjectsCount += lCount;

		if( lObjectsCount > 1 )
		{
			if( AfxMessageBox( IDS_CANT_SAVE_ALL_OBJECTS, MB_YESNO ) == IDYES )
			{

				CString	strFilter;
				pdocTemplate->GetDocString( strFilter, CDocTemplate::filterExt );
				::SetValue( GetAppUnknown(), "\\FileOpen", "LastExt", strFilter );
				return true;
			}
			else
				return false;
		}
	}

	return false;*/

}


bool CActionFileSave::ExecuteSettings( CWnd *pwndParent )
{
	CString	strDocumentTitle = GetArgumentString( "DocumentTitle" );

	m_strFileName = GetArgumentString( "FileName" );

	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();


	m_pdoc = 0;

	if( strDocumentTitle.IsEmpty() )
	{
		//get active 
		m_pdoc = pman->GetActiveDocument();
	}
	else
	{
		m_pdoc = pman->GetDocumentByTitle( strDocumentTitle );
	}

	if( !m_pdoc )
	//no active documents for close or invalid document title specified
		return false;

	strDocumentTitle = m_pdoc->GetTitle();
	
	if( m_strFileName.IsEmpty() )
		m_strFileName = m_pdoc->GetPathName();

	if( m_strFileName.IsEmpty() || ShowDlgAnyway() || IsNeedPrompt() )
	{
		if( !pman->DoPromptFileName( m_strFileName, GetDialogTitleID(), 0x80000000, FALSE, m_pdoc->GetDocTemplate() ) )
			//user cancel
			return false;

		if( m_strFileName.IsEmpty() )
			//bad filename
			return false;
	}

	SetArgument( "DocumentTitle", COleVariant( strDocumentTitle ) );
	SetArgument( "FileName", COleVariant( m_strFileName ) );

	return true;
}

bool CActionFileSave::Invoke()
{
//	CWaitCursor	wait;

	ASSERT( m_pdoc );
	ASSERT( !m_strFileName.IsEmpty() );

	if( m_pdoc->OnSaveDocument( m_strFileName ) )
	{
		m_pdoc->SetPathName( m_strFileName );
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////////////////////
//FileClose
CActionFileClose::CActionFileClose()
{
	m_pdoc = 0;
}

bool CActionFileClose::ExecuteSettings( CWnd *pwndParent )
{
	ASSERT_KINDOF(CShellDocManager, theApp.m_pDocManager);
	CShellDocManager	*pman = theApp.GetDocManager();

	CString	strDocumentTitle = GetArgumentString( "DocumentTitle" );

	m_pdoc = 0;

	if( strDocumentTitle.IsEmpty() )
	{
		//get active 
		m_pdoc = pman->GetActiveDocument();
	}
	else
	{
		m_pdoc = pman->GetDocumentByTitle( strDocumentTitle );
	}

	if( !m_pdoc )
	//no active documents for close or invalid document title specified
		return false;

	strDocumentTitle = m_pdoc->GetTitle();
	SetArgument( "DocumentTitle", COleVariant( strDocumentTitle ) );

	return true;
}

bool CActionFileClose::Invoke()
{
	ASSERT( m_pdoc );

	// A.M. fix, BT4813. Active document in CShellTemplate will be set to NULL in CShellDoc::~CShellDoc
	CShellDocTemplate *pTempl = (CShellDocTemplate *)m_pdoc->GetDocTemplate();
	BOOL bWasActive = pTempl->GetActiveDocument() == m_pdoc;

	if( !m_pdoc->SaveModified() )return false;
	m_pdoc->OnCloseDocument();

	if (bWasActive)
	{
		CShellDoc *pDoc = (CShellDoc *)theApp.GetDocManager()->GetActiveDocument();
		if (pDoc)
		{
			if (pDoc->GetDocTemplate() == pTempl)
				pTempl->SetActiveDocument(pDoc);
		}

	}

	return true;
}
/////////////////////////////////////////////////////////////////////////////////////////
//send e-mail
bool CActionFileSendEMail::Invoke()
{
	CShellDocManager	*pman = theApp.GetDocManager();
	CShellDoc			*pdoc = (CShellDoc*)pman->GetActiveDocument();
	pdoc->OnFileSendMail();
	return true;
}

bool CActionFileSendEMail::IsAvaible()
{
	sptrIDataTypeManager sptrM(m_punkTarget);
	if( sptrM == 0 )
		return false;
	long	nTypesCounter = 0;
	sptrM->GetTypesCount( &nTypesCounter );
	for( long nType = 0; nType < nTypesCounter; nType ++ )
	{
		_bstr_t	bstrTypeName;
		sptrM->GetType(nType, bstrTypeName.GetAddress());
		IUnknownPtr punkObj;
		LONG_PTR	lpos = 0;
		sptrM->GetObjectFirstPosition(nType, &lpos);
		while( lpos )
		{
			sptrM->GetNextObject(nType, &lpos, &punkObj);
			INamedDataObject2Ptr sptrNDO2(punkObj);
			if (sptrNDO2 != 0)
			{
				DWORD dwFlags;
				sptrNDO2->GetObjectFlags(&dwFlags);
				if (dwFlags&nofCantSendTo)
					return false;
			}
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////////////////
//save and send current view
#include "compressint.h"
#pragma warning(disable: 4228)
#include <mapi.h>
#pragma warning(default: 4228)

UINT AFXAPI AfxGetFileName(LPCTSTR lpszPathName, LPTSTR lpszTitle, UINT nMax);

bool CActionFileSaveView::save_to_file( const char *psz_filename )
{
	IPrintViewPtr	ptrPrint( m_punkTarget );
	if( ptrPrint == 0 )
	{
		AfxMessageBox( IDS_PRINT_UNSUPPORTED );
		return false;
	}

	ICompressionManagerPtr	ptrCompressor( GetAppUnknown() );
	if( ptrCompressor == 0 )
	{
		AfxMessageBox( IDS_COMPRESSOR_NOT_FOUND );
		return  false;
	}

	IScrollZoomSite2Ptr	ptr_site( m_punkTarget );
	if(ptr_site==0) return false;

	SIZE	size;
	POINT	scroll_pos, point = {0,0};
	double	zoom;
	BOOL bFit=FALSE;

	ptr_site->GetClientSize( &size );
	ptr_site->GetZoom( &zoom );
	ptr_site->GetScrollPos( &scroll_pos );
	ptr_site->GetFitDoc(&bFit);

	ptr_site->SetFitDoc(FALSE);
	ptr_site->SetZoom( 1 );
	ptr_site->SetScrollPos( point );


	IViewPtr	ptr_view( m_punkTarget );
	IUnknown	*punk_f = 0;
	ptr_view->GetMultiFrame( &punk_f, false );

	if( punk_f )
	{
		IMultiSelectionPtr	ptr_frame( punk_f );
		punk_f->Release();
		ptr_frame->EmptyFrame();
	}


	int	cx = int(size.cx), 
		cy = int(size.cy);
	BOOL	bFlag = FALSE;
	int	dx=0, dy=0;

	int cx1=100000, cy1=100000;
	{
		IAViewPtr ptrAView(ptrPrint);
		if(ptrAView!=0) cx1=cx;
	}

	ptrPrint->GetPrintWidth( cx1, &cx, &bFlag, 0, &dx ); 
	ptrPrint->GetPrintHeight( cy1, &cy, &bFlag, 0, &dy ); 

	HDC	hdc_screen = ::GetDC( 0 );
	HDC	hdc = ::CreateCompatibleDC( hdc_screen );

	BITMAPINFOHEADER	bmih;
	ZeroMemory( &bmih, sizeof( bmih ) );
	bmih.biSize = sizeof( bmih );
	bmih.biWidth = cx;
	bmih.biHeight = cy;
	bmih.biPlanes = 1;
	bmih.biBitCount = 24;

	byte	*pdib = 0;
	HBITMAP hbmp = ::CreateDIBSection( hdc_screen, (BITMAPINFO*)&bmih, DIB_RGB_COLORS, (void**)&pdib, 0, 0 ); 
	::ReleaseDC( 0, hdc_screen );

	CRect	rect( 0, 0, cx, cy );

	::SelectObject( hdc, hbmp );
	::FillRect( hdc, &rect, (HBRUSH)::GetStockObject( WHITE_BRUSH ) );

	::SetROP2( hdc, R2_COPYPEN );



	ptrPrint->Print( hdc, rect, 0, 0, dx, dy, false, 1 );

	ptr_site->SetFitDoc(bFit);
	ptr_site->SetZoom( zoom );
	ptr_site->SetScrollPos( scroll_pos );


	ptrCompressor->WriteDIB24ToFile( &bmih, pdib, _bstr_t( psz_filename ) );

	::DeleteDC( hdc );
	::DeleteObject( hbmp );
	return true;
}

bool CActionFileSaveView::IsAvaible()
{
	IViewPtr	ptrV( m_punkTarget );
	TPOS	lpos = 0;
	ptrV->GetFirstVisibleObjectPosition( &lpos );
	if (lpos == 0) return false;
	if (!CheckInterface(m_punkTarget, IID_IPrintView))
		return false;
	IPrintView2Ptr sptrPV2(m_punkTarget);
	if (sptrPV2 == 0) return true;
	DWORD dwFlags;
	sptrPV2->GetPrintFlags(&dwFlags);
	if (dwFlags & PrintView_CanSaveToImage)
		return true;
	else return false;
}

bool CActionFileSaveView::Invoke()
{
	CString	strPathName = GetArgumentString( "FileName" );
	if( strPathName.IsEmpty() )
	{
		char	sz_name[MAX_PATH];
		strcpy( sz_name, ::GetObjectName( m_punkTarget ) );
		strcat( sz_name, ".jpg" );
		CFileDialog	dlg( false, ".jpg", sz_name, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "JPEG Files|*.jpg||", AfxGetMainWnd(), _FILE_OPEN_SIZE_ );
		if( dlg.DoModal() != IDOK )return false;
		strPathName = dlg.GetPathName();
	}
	return save_to_file( strPathName );
}

bool CActionFileSendView::Invoke()
{
	CWaitCursor wait;

	HMODULE	hInstMAPI = ::LoadLibrary( "mapi32.dll" );

	if( !hInstMAPI )
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_LOAD);
		return false;
	}

	ULONG (PASCAL *lpfnSendMail)(ULONG, ULONG, MapiMessage*, FLAGS, ULONG);
	(FARPROC&)lpfnSendMail = ::GetProcAddress( hInstMAPI, "MAPISendMail" );
	if (lpfnSendMail == NULL)
	{
		::FreeLibrary( hInstMAPI );
		AfxMessageBox(AFX_IDP_INVALID_MAPI_DLL);
		return false;
	}

	ASSERT(lpfnSendMail != NULL);

	char szTempName[MAX_PATH];
	char szPath[MAX_PATH];
		
	// save to temporary path
	::GetTempPath( sizeof( szPath ), szPath );
	//::GetTempFileName( szPath, "vtshot", 0, szTempName );
	
	CString	str = ::GetObjectName( m_punkTarget );
	strcpy( szTempName, szPath );
	strcat( szTempName, str );
	strcat( szTempName, ".jpg" );

	if( !save_to_file( szTempName ) )
	{
		::FreeLibrary( hInstMAPI );
		return false;
	}

	// build an appropriate title for the attachment
	char szTitle[MAX_PATH];
	AfxGetFileName( szTempName, szTitle, sizeof(szTitle) );

	// prepare the file description (for the attachment)
	MapiFileDesc fileDesc;
	memset(&fileDesc, 0, sizeof(fileDesc));
	fileDesc.nPosition = (ULONG)-1;
	fileDesc.lpszPathName = szTempName;
	fileDesc.lpszFileName = szTitle;

	// prepare the message (empty with 1 attachment)
	MapiMessage message;
	memset(&message, 0, sizeof(message));
	message.nFileCount = 1;
	message.lpFiles = &fileDesc;

	// prepare for modal dialog box
	AfxGetApp()->EnableModeless(FALSE);
	HWND hWndTop;
	CWnd* pParentWnd = CWnd::GetSafeOwner(NULL, &hWndTop);

	// some extra precautions are required to use MAPISendMail as it
	// tends to enable the parent window in between dialogs (after
	// the login dialog, but before the send note dialog).
	pParentWnd->SetCapture();
	::SetFocus(NULL);
	pParentWnd->m_nFlags |= WF_STAYDISABLED;

	int nError = lpfnSendMail(0, (ULONG)pParentWnd->GetSafeHwnd(),
		&message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

	// after returning from the MAPISendMail call, the window must
	// be re-enabled and focus returned to the frame to undo the workaround
	// done before the MAPI call.
	::ReleaseCapture();
	pParentWnd->m_nFlags &= ~WF_STAYDISABLED;

	pParentWnd->EnableWindow(TRUE);
	::SetActiveWindow(NULL);
	pParentWnd->SetActiveWindow();
	pParentWnd->SetFocus();
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	AfxGetApp()->EnableModeless(TRUE);

	::DeleteFile(  szTempName );
	
	if (nError != SUCCESS_SUCCESS &&
		nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
	{
		AfxMessageBox(AFX_IDP_FAILED_MAPI_SEND);
		return false;
	}

	// remove temporary file, if temporary file was used
	return true;
}


ACTION_ARG_LIST(CActionDumpMem)
	ARG_STRING("Flags", 0)
END_ACTION_ARG_LIST()
struct	mem_alloc
{
	char	*psz_fname;
	int		line;
	size_t	size, count;

	mem_alloc()
	{
		psz_fname = 0;
		line = 0;
		count = 0;
		size = 0;
	}
	~mem_alloc()
	{
	}

	static void free( mem_alloc *p )
	{
		::free( p->psz_fname );
		delete p;
	}
	static long compare( mem_alloc *p1, mem_alloc *p2 )
	{
		if( !p1->psz_fname && p2->psz_fname )return -1;
		else if( p1->psz_fname && !p2->psz_fname )return 1;
		else if( !p1->psz_fname && !p2->psz_fname )return p1->line-p2->line;
		int cmp = _tcscmp( p1->psz_fname, p2->psz_fname );
		if( cmp )return cmp;
		return p1->line-p2->line;
	}
	static int cmp_alloc_size( const void *p1, const void *p2 )
	{
		const mem_alloc **palloc1 = (const mem_alloc **)p1;
		const mem_alloc **palloc2 = (const mem_alloc **)p2;

		return (*palloc2)->size-(*palloc1)->size;
	}
};


bool CActionDumpMem::Invoke()
{
	CWaitCursor wait;
	const char sz_heap_log[] = "heap.log";
	_trace_file_clear( sz_heap_log );

#ifdef _DEBUG

	struct crt_mem_block
	{
	   struct crt_mem_block *next;
	   struct crt_mem_block *prev;
	   char *psz_fname;
	   int line;           // Line number
	   size_t size;    // Size of user block
//	   int nBlockUse;       // Type of block
//	   long lRequest;       // Allocation number
	};


	_CrtMemState	mem_state;
	_CrtMemCheckpoint( &mem_state );


	_list_map_t<mem_alloc*,mem_alloc*, mem_alloc::compare, mem_alloc::free>	
		allocations;
	

	mem_alloc	a, *palloc;
	for( crt_mem_block	*p = (crt_mem_block*)mem_state.pBlockHeader; p; p = p->next )
	{
		
		a.psz_fname = p->psz_fname;
		a.line = p->line;
		TPOS lpos = allocations.find(&a);

		if( !lpos )
		{
			palloc = new mem_alloc;
			palloc->psz_fname = _tcsdup( p->psz_fname );
			palloc->line = p->line;
			allocations.set( palloc, palloc );
		}
		else
			palloc = allocations.get( lpos );
		palloc->size += p->size;
		palloc->count++;
	}

	INT_PTR	count = 0;
	int	size = 0;

	INT_PTR	alloc_count = allocations.count();

	mem_alloc	**pall = (mem_alloc**)malloc( sizeof(mem_alloc*)*alloc_count );
	INT_PTR	idx = 0;

	_trace_file( sz_heap_log, "----all CRT heap" );

	for (TPOS lpos = allocations.head(); lpos; lpos = allocations.next(lpos))
	{
		palloc = allocations.get( lpos );
		pall[idx] = palloc;idx++;
		_trace_file( sz_heap_log, "[%d/%d] %s, %d", palloc->count, palloc->size, palloc->psz_fname, palloc->line );
		count+=palloc->count;
		size+=palloc->size;
	}

	_trace_file( sz_heap_log, "----top 10" );

	::qsort( pall, alloc_count, sizeof( mem_alloc* ), mem_alloc::cmp_alloc_size );

	for( idx = 0; idx  < min( 10, alloc_count ); idx++ )
	{
		palloc = pall[idx];
		_trace_file( sz_heap_log, "[%d/%d] %s, %d", palloc->count, palloc->size, palloc->psz_fname, palloc->line );
	}

	::free( pall );

	_trace_file( sz_heap_log, "total [%d/%d]", count, size );
#endif

	return true;
}
