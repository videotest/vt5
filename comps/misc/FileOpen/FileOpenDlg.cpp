// FileOpenDlg.cpp : implementation file
//
#include "stdafx.h"

#include "FileOpen.h"
#include "wndsizer.h"
#include "FileOpenDlg.h"

#include "afxpriv2.h"
#include <afxmt.h>
#include "shlguid.h"
#include "resource.h"
#include "BCGCB.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int MAX_CHAR			= 250;
const int TB_BTN_UPONELEVEL = 40961;
const int TB_BTN_NEWFOLDER	= 40962;
//const int TB_BTN_LIST		= 40963;
//const int TB_BTN_DETAILS	= 40964;

const int TB_BTN_LIST			= WM_USER+100;
const int TB_BTN_DETAILS		= WM_USER+101;
const int TB_BTN_PREVIEW		= WM_USER+102;
const int TB_BTN_ADD_FAVORITE	= WM_USER+103;
const int TB_BTN_GOTO_FAVORITE	= WM_USER+104;

const int ID_CH_READ_ONLY	= 0x410;
const int ID_ST_FILE_TYPE	= 0x441;
const int ID_ST_FILE_NAME	= 0x442;
const int ID_ST_LOOK_IN		= 0x443;
const int ID_LB_MAIN		= 0x460;
const int ID_CB_FILE_TYPE	= 0x470;
const int ID_CB_LOOK_IN		= 0x471;
const int ID_PB_OPEN		= 0x001;
const int ID_PB_CANCEL		= 0x002;
const int ID_PB_HELP		= 0x40e;
const int ID_ED_FILE_NAME	= 0x480;

const int FAVORITE_BASE_ID	= 27000;

static int nToolBarLeft = 0;
static int nToolBarTop = 0;
static int nToolBarBottom = 0;
static int nToolBarRight = 0;

static bool bSendVScroll = true;
static bool bSendHScroll = true;
HHOOK HookHandle;
CFileOpenDlg* dlg;
HWND hwndListCtrl;
HWND hwndToolBar;

// {B189049B-7180-11D3-A4F6-0000B493A187}
static const IID IID_IFileOpenDlg =
{ 0xb189049B, 0x7180, 0x11d3, { 0xa4, 0xf6, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };

// {F360E264-7AF1-11d3-A504-0000B493A187}
static const GUID IID_IOpenFileDialogEvents = 
{ 0xf360e264, 0x7af1, 0x11d3, { 0xa5, 0x4, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87 } };


// {B1890499-7180-11D3-A4F6-0000B493A187}
GUARD_IMPLEMENT_OLECREATE(CFileOpenDlg, "FileOpen.FileOpenDlg", 0xb1890499, 0x7180, 0x11d3, 0xa4, 0xf6, 0x0, 0x0, 0xb4, 0x93, 0xa1, 0x87)

/////////////////////////////////////////////////////////////////////////////
// CFileOpenDlg

IMPLEMENT_DYNCREATE(CFileOpenDlg, CFileDialog)

//explicit CFileDialog(
//BOOL bOpenFileDialog,
//LPCTSTR lpszDefExt = NULL,
//LPCTSTR lpszFileName = NULL,
//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
//LPCTSTR lpszFilter = NULL,
//CWnd* pParentWnd = NULL,
//DWORD dwSize = 0,
//BOOL bVistaStyle = TRUE
//);

CFileOpenDlg::CFileOpenDlg() : CFileDialog(TRUE, 0, 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT , 0, 0, 0, FALSE), 
	m_pscrollHorz(0),
	m_pscrollVert(0)

{
	//no templates
	m_ofn.Flags |= OFN_ENABLETEMPLATE | OFN_EXPLORER;
	m_ofn.Flags ^= OFN_ENABLETEMPLATE;

	m_nWidthDlg = 0;
	m_nHeightDlg = 0;
	m_nListBoxWidth = 0;
	m_nListBoxHeight = 0;
	m_nMinDlgWidth = 0;
	m_nMinDlgHeight = 0;
	m_curMode = dmList;
	m_bCurViewNeedScroll = false;
	m_bNeedShowFileInfo = false;
	m_bNewPreview = true;
	m_punkData = NULL;
	m_punkContext = NULL;
	m_punkView = NULL;
	m_strLastPath = "";
	m_fontInfo.CreateFont(5*::GetSystemMetrics( SM_CYVSCROLL )/6, 3*::GetSystemMetrics( SM_CYVSCROLL )/6,  0, 0, FW_NORMAL, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_SWISS, "Courier");
	//m_fontInfo = GetFont();
	dlg = NULL;
	m_rcLastDraw = NORECT;
	m_bDrawContentWhileDrag = false;

	EnableAggregation();
	EnableAutomation();
	EnableConnections();


	m_piidEvents = &IID_IOpenFileDialogEvents;
	m_piidPrimary = &IID_IFileOpenDlg;

	m_sptrMainFrame = 0;

	m_sName = _T("FileOpen");

	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);
	m_bWeUnder98Chikaka = osVer.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS && osVer.dwMinorVersion > 0;
	m_bSeveralFilesInUse = false;
	_OleLockApp( this );


	m_strTitle = "";
	m_nRes = IDCANCEL;
}


CFileOpenDlg::~CFileOpenDlg()
{
	_OleUnlockApp( this );
}

BEGIN_MESSAGE_MAP(CFileOpenDlg, CCmdTarget)
	//{{AFX_MSG_MAP(CFileOpenDlg)
	ON_WM_DROPFILES()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()	
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_NOTIFYLONGOPERATION, OnLongNotify)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CFileOpenDlg, CFileDialog)
	//{{AFX_DISPATCH_MAP(CFileOpenDlg)
	DISP_PROPERTY_EX(CFileOpenDlg, "Flags", GetFlags, SetFlags, VT_I4)
	DISP_PROPERTY_EX(CFileOpenDlg, "Title", GetTitle, SetTitle, VT_BSTR)
	DISP_PROPERTY_EX(CFileOpenDlg, "FileName", GetFileName, SetFileName, VT_BSTR)
	DISP_PROPERTY_EX(CFileOpenDlg, "Extentions", GetExtentions, SetExtentions, VT_BSTR)
	DISP_PROPERTY_EX(CFileOpenDlg, "Preview", GetPreview, SetPreview, VT_BOOL)
	DISP_PROPERTY_EX(CFileOpenDlg, "DefaultExtention", GetDefaultExtention, SetDefaultExtention, VT_BSTR)
	DISP_PROPERTY_EX(CFileOpenDlg, "InitialDir", GetInitialDir, SetInitialDir, VT_BSTR)
	DISP_FUNCTION(CFileOpenDlg, "SaveDlgState", SaveDlgState, VT_BOOL, VTS_UNKNOWN VTS_BSTR)
	DISP_FUNCTION(CFileOpenDlg, "LoadDlgState", LoadDlgState, VT_BOOL, VTS_UNKNOWN VTS_BSTR)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IFileOpenDlg to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.


BEGIN_INTERFACE_MAP(CFileOpenDlg, CCmdTarget)
	INTERFACE_PART(CFileOpenDlg, IID_IOpenFileDialog, OpenFile)
	INTERFACE_PART(CFileOpenDlg, IID_IFileOpenDlg, Dispatch)
	INTERFACE_PART(CFileOpenDlg, IID_IScrollZoomSite, ScrollSite)
	INTERFACE_PART(CFileOpenDlg, IID_IScrollZoomSite2, ScrollSite)
	INTERFACE_PART(CFileOpenDlg, IID_IRootedObject, Rooted)
	INTERFACE_PART(CFileOpenDlg, IID_IConnectionPointContainer, ConnPtContainer)
	INTERFACE_PART(CFileOpenDlg, IID_IProvideClassInfo, ProvideClassInfo)
	INTERFACE_PART(CFileOpenDlg, IID_IMsgListener, MsgList)
	INTERFACE_PART(CFileOpenDlg, IID_INamedObject2, Name)
	INTERFACE_AGGREGATE(CFileOpenDlg, m_punkData)
	INTERFACE_AGGREGATE(CFileOpenDlg, m_punkContext)
	INTERFACE_AGGREGATE(CFileOpenDlg, m_punkView)
END_INTERFACE_MAP()

BEGIN_EVENT_MAP(CFileOpenDlg, COleEventSourceImpl)
	//{{AFX_EVENT_MAP(CFileOpenDlg)
	EVENT_CUSTOM("OnBeforeExecute", FireOnBeforeExecute, VTS_NONE)
	EVENT_CUSTOM("OnDialogInited", FireOnDialogInited, VTS_NONE)
	EVENT_CUSTOM("OnAfterExecute", FireOnAfterExecute, VTS_NONE)
	//}}AFX_EVENT_MAP
END_EVENT_MAP()

IMPLEMENT_OLETYPELIB(CFileOpenDlg, IID_ITypeLibID, 1, 0)


BOOL CALLBACK EnumChildProc(HWND hWnd, LPARAM lParam)
{
	char szClassName[MAX_CHAR];
			
	GetClassName(hWnd, szClassName, MAX_CHAR);
	if(strcmp(_strlwr(szClassName), "toolbarwindow32") == 0)
	{

		CSize size;
		TBBUTTON tbButton;
		int nRight, nBottom;
		CToolBarCtrl toolbar;
		toolbar.Attach(hWnd);
		int nButtonCount = toolbar.GetButtonCount();

		int nSeparCount = 0;
		for(int i = nButtonCount-1; i >= 0 ; i--)
		{
			toolbar.GetButton(i, &tbButton);
			if (tbButton.idCommand >= 0)
				if(tbButton.idCommand != TB_BTN_UPONELEVEL	&&
				   tbButton.idCommand != TB_BTN_NEWFOLDER )
				   //tbButton.idCommand != TB_BTN_LIST		&&
				   //tbButton.idCommand != TB_BTN_DETAILS)
				{
					if(tbButton.fsStyle == TBSTYLE_SEP && i > 0)
					{
						nSeparCount++;
						if(nSeparCount <= (dlg->m_bWeUnder98Chikaka ? 1:2))
							continue;
					}
					toolbar.DeleteButton(i);	
					nSeparCount = 0;
				}
		}

		
		CToolInfo toolInfo;
		CToolTipCtrl* pToolTip = toolbar.GetToolTips();
		pToolTip->GetToolInfo(toolInfo, &toolbar, TB_BTN_UPONELEVEL);
		CRect rc;
		int nIconIdx = toolbar.AddBitmap(5, IDR_TOOLBAR_PREVIEW);
		if (nIconIdx >= 0)
		{
			//add our list button
			tbButton.idCommand = TB_BTN_LIST;
			tbButton.iBitmap = nIconIdx++;
			tbButton.iString = NULL;
			tbButton.dwData = 0;
			if(dlg->m_curMode == dmList)
				tbButton.fsState = TBSTATE_ENABLED|TBSTATE_CHECKED;
			else
				tbButton.fsState = TBSTATE_ENABLED;
			tbButton.fsStyle = TBSTYLE_CHECKGROUP;
			toolbar.AddButtons(1, &tbButton);
			toolbar.GetMaxSize(&size);
			rc = CRect(size.cx-LOWORD(toolbar.GetButtonSize()), toolInfo.rect.top, size.cx, toolInfo.rect.bottom);
			pToolTip->AddTool(&toolbar, IDS_TOOLTIP_LIST, &rc, TB_BTN_LIST);
			//add our details button
			tbButton.idCommand = TB_BTN_DETAILS;
			if(dlg->m_curMode == dmReport)
				tbButton.fsState = TBSTATE_ENABLED|TBSTATE_CHECKED;
			else
				tbButton.fsState = TBSTATE_ENABLED;
			tbButton.iBitmap = nIconIdx++;
			toolbar.AddButtons(1, &tbButton);
			toolbar.GetMaxSize(&size);
			rc = CRect(size.cx-LOWORD(toolbar.GetButtonSize()), toolInfo.rect.top, size.cx, toolInfo.rect.bottom);
			pToolTip->AddTool(&toolbar, IDS_TOOLTIP_DETAILS, &rc, TB_BTN_DETAILS);
			//add our preview button
			tbButton.idCommand = TB_BTN_PREVIEW;
			if(dlg->m_curMode == dmPreview)
				tbButton.fsState = TBSTATE_ENABLED|TBSTATE_CHECKED;
			else
				tbButton.fsState = TBSTATE_ENABLED;
			tbButton.iBitmap = nIconIdx++;
			toolbar.AddButtons(1, &tbButton);
			toolbar.GetMaxSize(&size);
			rc = CRect(size.cx-LOWORD(toolbar.GetButtonSize()), toolInfo.rect.top, size.cx, toolInfo.rect.bottom);
			pToolTip->AddTool(&toolbar, IDS_TOOLTIP_PREVIEW, &rc, TB_BTN_PREVIEW);
			//add our add favorite button
			tbButton.idCommand = TB_BTN_ADD_FAVORITE;
			tbButton.fsStyle = TBSTYLE_BUTTON;
			tbButton.fsState = TBSTATE_ENABLED;
			tbButton.iBitmap = nIconIdx++;
			toolbar.AddButtons(1, &tbButton);
			toolbar.GetMaxSize(&size);
			rc = CRect(size.cx-LOWORD(toolbar.GetButtonSize()), toolInfo.rect.top, size.cx, toolInfo.rect.bottom);
			pToolTip->AddTool(&toolbar, IDS_TOOLTIP_ADD_FAV, &rc, TB_BTN_ADD_FAVORITE);
			//add our goto favorite button
			tbButton.idCommand = TB_BTN_GOTO_FAVORITE;
			tbButton.fsStyle = TBSTYLE_CHECK;
			tbButton.fsState = TBSTATE_ENABLED;
			tbButton.iBitmap = nIconIdx++;
			toolbar.AddButtons(1, &tbButton);
			toolbar.GetMaxSize(&size);
			rc = CRect(size.cx-LOWORD(toolbar.GetButtonSize()), toolInfo.rect.top, size.cx, toolInfo.rect.bottom);
			pToolTip->AddTool(&toolbar, IDS_TOOLTIP_GOTO_FAV, &rc, TB_BTN_GOTO_FAVORITE);
			
		}
		toolbar.GetMaxSize(&size);
		nBottom = nToolBarTop + size.cy;
		nRight = nToolBarLeft+size.cx;
		//::MoveWindow(hWnd, nToolBarLeft, nToolBarTop, nRight, nBottom, TRUE);
		::MoveWindow(hWnd, nToolBarLeft, nToolBarTop, nToolBarRight, nToolBarBottom, TRUE);
		hwndToolBar = hWnd;
		toolbar.Detach();	
		return FALSE;
	}
	
	return TRUE;
}


void _SetListBoxStyle()
{
	if (!(hwndListCtrl && ::IsWindow(hwndListCtrl)))
		return;


	DWORD dwStyle = GetWindowLong(hwndListCtrl, GWL_STYLE);

	if (dlg->m_curMode == dmReport)
	{
		dwStyle &= ~LVS_TYPEMASK;
        dwStyle |= LVS_REPORT | WS_HSCROLL;	
		SetWindowLong( hwndListCtrl, GWL_STYLE, dwStyle ); 

//		SendMessage( hwndListCtrl, WM_USER + 0x3251, 0, 0 );
//		SendMessage( hwndListCtrl, WM_USER + 0x3212, 0, 0 );
		
		PostMessage( hwndListCtrl, WM_RBUTTONUP, 0, 0 );
		PostMessage( hwndListCtrl, WM_LBUTTONDOWN, 0, 0 );
		PostMessage( hwndListCtrl, WM_LBUTTONUP, 0, 0 );

		PostMessage( hwndListCtrl, WM_RBUTTONUP, 0, 0 );
		PostMessage( hwndListCtrl, WM_LBUTTONDOWN, 0, 0 );
		PostMessage( hwndListCtrl, WM_LBUTTONUP, 0, 0 );


		return;
	}

	if ((dwStyle & LVS_REPORT) == LVS_REPORT && dlg->m_curMode != dmReport) 
	{
		dwStyle ^= LVS_REPORT;
	}
	else if ((dwStyle & LVS_LIST) == LVS_LIST && dlg->m_curMode != dmPreview  && dlg->m_curMode != dmList) 
	{
		dwStyle ^= LVS_LIST;
	}
	if (dlg->m_curMode == dmList || dlg->m_curMode == dmPreview)
		dwStyle |= LVS_LIST;
	else if (dlg->m_curMode == dmReport)
	{
		//dwStyle &= ~LVS_TYPEMASK;
        //dwStyle |= LVS_REPORT;	
	}

	SetWindowLong(hwndListCtrl, GWL_STYLE, dwStyle); 

	if (dlg->m_curMode == dmReport)
	{
	//[ay]все это нецивильно и вообще бред редкостный. однако под xp
	//это единственный найденный способ показать DETAILS.
	//недостаток - мигает меню при включении report				
		PostMessage( hwndListCtrl, WM_RBUTTONUP, 0, 0);
		PostMessage( hwndListCtrl, WM_LBUTTONDOWN, 0, 0);
		PostMessage( hwndListCtrl, WM_LBUTTONUP, 0, 0);			
	}

}

LRESULT CALLBACK Hooker(int nCode, WPARAM wParam, LPARAM lParam)
{
	CWPSTRUCT *x = (CWPSTRUCT*)lParam;
	char szClassName[MAX_CHAR];
	GetClassName(x->hwnd, szClassName, MAX_CHAR);
	if( dlg && strcmp(_strlwr(szClassName), "syslistview32") == 0 ) 
	{
		//int id = ::GetDlgCtrlID(::GetParent(x->hwnd));
		//if (id == 0x461)
		if (x->hwnd && (::GetParent(::GetParent(x->hwnd)) == ::GetParent(dlg->m_hWnd)))
		{
			hwndListCtrl = x->hwnd; 
			if(dlg->m_curMode == dmPreview)
			{
				CRect rc;
				::GetWindowRect(::GetParent(x->hwnd), &rc);
				if (!rc.IsRectEmpty() && ((dlg->m_nListBoxWidth > 0 && rc.Width() != dlg->m_nListBoxWidth/2) || (rc.Height() != dlg->m_nListBoxHeight && dlg->m_nListBoxHeight > 0)))
				{
					dlg->DoRecalcLayout();
				}
			}
			else
			{
				CRect rc;
				::GetWindowRect(::GetParent(x->hwnd), &rc);
				if (!rc.IsRectEmpty() && ((rc.Width() != dlg->m_nListBoxWidth && dlg->m_nListBoxWidth > 0)||(rc.Height() != dlg->m_nListBoxHeight && dlg->m_nListBoxHeight > 0)))
				{
					dlg->DoRecalcLayout();
				}
			}
		}
	}

	if (x->message == WM_NOTIFY)
	{
		OFNOTIFY* pofn = (LPOFNOTIFY)x->lParam;
		if(pofn->hdr.code == CDN_SELCHANGE)
		{
			//TRACE0("@@@@@@@HOOKER SAY SELCHANGE\n");
			if(	dlg->m_sptrLong != 0)	
			{
				dlg->m_sptrLong->Abort();
				dlg->m_sptrLong = 0;
			}
		}

	}
	if (x->message == WM_COMMAND)
	{
		CString strFavoritesDefPath = ::MakeAppPathName("Favorites")+"\\";
		CString strPathLink = ::GetValueString(GetAppUnknown(), "Paths", "FavoritesPath", strFavoritesDefPath);


		if(LOWORD(x->wParam) == TB_BTN_PREVIEW)
		{
			dlg->m_curMode = dmPreview;
			dlg->OnPreview();			
			::SendMessage(hwndListCtrl, WM_NCPAINT, 1, 0);
			_SetListBoxStyle();
		}
		else if(LOWORD(x->wParam) == TB_BTN_LIST)
		{
			dlg->m_curMode = dmList;
			dlg->OnEndPreview();			
			::SendMessage(hwndListCtrl, WM_NCPAINT, 1, 0);
			_SetListBoxStyle();
		}
		else if(LOWORD(x->wParam) == TB_BTN_DETAILS)
		{
			dlg->m_curMode = dmReport;
			dlg->OnEndPreview();			
			::SendMessage(hwndListCtrl, WM_NCPAINT, 1, 0);
			_SetListBoxStyle();
		}
		else if(LOWORD(x->wParam) == TB_BTN_ADD_FAVORITE)
		{
			CString strDir;
			GetCurrentDirectory(_MAX_PATH, strDir.GetBuffer(_MAX_PATH));
			strDir.ReleaseBuffer();

			HRESULT hres; 
			IShellLink* psl; 
 			// Get a pointer to the IShellLink interface. 
			hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl); 
			if (SUCCEEDED(hres)) 
			{
				IPersistFile* ppf; 
		        psl->SetPath(strDir);

				int	n = dlg->m_ofn.nFilterIndex;
				char	*pszNext = (char*)dlg->m_ofn.lpstrFilter;
				char	*pszExt;
				CString strDescr;
				while( *pszNext )
				{
					strDescr = pszNext;
					pszExt = pszNext+strlen( pszNext )+1;
					pszNext = pszExt+strlen( pszExt )+1;
					if(--n <= 0)
					{
						break;
					}
				}

				int nPos = strDescr.Find("(");
				if(nPos > 0)
					strDescr = strDescr.Left(nPos-1);
				psl->SetDescription(strDescr);
		        hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
				if (SUCCEEDED(hres)) 
				{ 
					WORD wsz[MAX_PATH]; 
					CString strLinkName = strDir;
					if(strLinkName[strLinkName.GetLength()-1] == '\\')
						strLinkName.Delete(strLinkName.GetLength()-1, 1);
					int nStartName = strLinkName.ReverseFind('\\');
					if(nStartName >= 0)
						strLinkName = strLinkName.Right(strLinkName.GetLength()-nStartName-1);
					else
						strLinkName = "Disk " + strLinkName.Left(strLinkName.Find(":", 0));
					strPathLink += strLinkName;
					strPathLink += ".lnk";
					MultiByteToWideChar(CP_ACP, 0, strPathLink, -1, (LPWSTR)wsz, MAX_PATH);

					try
					{
						{CFile file(strPathLink, CFile::modeCreate);}
						hres = ppf->Save((LPWSTR)wsz, FALSE); 
					}
					catch( CException *pe )
					{
						pe->ReportError();
					}
					ppf->Release(); 
				} 
				psl->Release();
			}

		}
		else if(LOWORD(x->wParam) == TB_BTN_GOTO_FAVORITE)
		{
			CToolBarCtrl toolbar;
			if(hwndToolBar)
			{
				toolbar.Attach(hwndToolBar);

				dlg->m_arrFavorites.RemoveAll();
				CMenu menu;
				if(menu.CreatePopupMenu()==TRUE)
				{					
					long nID = FAVORITE_BASE_ID;

					CString strFavorite;
					strFavorite.LoadString(IDS_FAVORITE_FOLDER);
					menu.AppendMenu(MF_STRING, nID++, strFavorite);
					menu.AppendMenu(MF_SEPARATOR, 0, "");

					WIN32_FIND_DATA find_data;
					HANDLE hFind = FindFirstFile(strPathLink+"*.lnk", &find_data);
					if(hFind != INVALID_HANDLE_VALUE)
					{	
						do
						{
							strFavorite = find_data.cFileName;
							dlg->m_arrFavorites.Add(strFavorite);
							strFavorite = strFavorite.Left(strFavorite.ReverseFind('.'));
							menu.AppendMenu(MF_STRING, nID++, strFavorite);
						}while(FindNextFile(hFind, &find_data) == TRUE);
					}

					CRect rcBtn = NORECT;
					toolbar.GetRect(TB_BTN_GOTO_FAVORITE, &rcBtn);
					toolbar.ClientToScreen(&rcBtn);
					menu.TrackPopupMenu(TPM_LEFTALIGN, rcBtn.left, rcBtn.bottom, (CWnd*)dlg);

					toolbar.SetState(TB_BTN_GOTO_FAVORITE, TBSTATE_ENABLED);
				}			

				toolbar.Detach();
			}
		}
		else if(LOWORD(x->wParam)==ID_PB_HELP )
		{
			HelpDisplay( "FileOpen" );
		}
	}
	
	
	if( dlg )
	{
		sptrIWindow sptrWin(dlg->m_punkView);
		if (x->message == WM_VSCROLL)
		{
			HWND scrollHWND = (HWND)x->lParam;
			if (scrollHWND == dlg->m_pscrollVert->m_hWnd)
			{
				ASSERT(sptrWin != 0);
				CWnd* wndView = GetWindowFromUnknown(sptrWin);
				if (wndView)
				{
					if(bSendVScroll)
					{
						bSendVScroll = false;
						wndView->SendMessage(WM_VSCROLL, x->wParam, x->lParam);
						bSendVScroll = true;
					}
				}
			}
		}
		else if (x->message == WM_HSCROLL)
		{
			HWND scrollHWND = (HWND)x->lParam;
			if (scrollHWND == dlg->m_pscrollHorz->m_hWnd)
			{
				ASSERT(sptrWin != 0);
				CWnd* wndView = GetWindowFromUnknown(sptrWin);
				if (wndView)
				{
					if(bSendHScroll)
					{
						bSendHScroll = false;
						wndView->SendMessage(WM_HSCROLL, x->wParam, x->lParam);
						bSendHScroll = true;
					}
				}
			}
		}
		sptrWin = 0;
	}
	return CallNextHookEx(HookHandle, nCode, wParam, lParam);
}


void _KillExtension()
{
	if(dlg == NULL) return;
	if(dlg->m_ofn.lpstrFile == NULL)return;
	char* pszPoint = 0;
	char* pszFile = (char*)dlg->m_ofn.lpstrFile;
	pszPoint = (char*)strrchr(pszFile, '.');

	if( pszPoint ) 
		memset(dlg->m_ofn.lpstrFile+(pszPoint - pszFile), '\0', 2);
}

void _SetDefaultExt(int nFilter)
{
	if(dlg == NULL) return;
	char* pszNull1;
	char* pszNull2;
	pszNull1 = pszNull2 = (char*)dlg->m_ofn.lpstrFilter;
	if (nFilter == 0)
		nFilter = dlg->m_ofn.nFilterIndex;
	for(int i = 0; i < nFilter; i++)
	{
		pszNull1 = (char*)strchr(pszNull1, '\0');
		if(pszNull1 == NULL)
			return;
		if(i + 1 < nFilter)
		{
			pszNull2 = (char*)strchr(pszNull1+1, '\0');
			pszNull1 = pszNull2+1;
		}
		if(pszNull2 == NULL)
				return;
	}
	pszNull1 += 3;
	dlg->m_ofn.lpstrDefExt =  (LPCSTR)pszNull1;
	//CommDlg_OpenSave_SetDefExt(dlg->GetSafeHwnd(), strFilter);
}

void _SetFilterIndex()
{
	if(dlg == NULL) return;
	char* pszNull1;
	char* pszNull2;

	dlg->m_ofn.nFilterIndex = 0;

	pszNull1 = pszNull2 = (char*)dlg->m_ofn.lpstrFilter;

	int i = 0;
	while( *pszNull1 )
	{
		i++;
		pszNull1 = (char*)strchr(pszNull1, '\0');

		ASSERT( pszNull1 );

		char	*pszExts = pszNull1+1;
		char	*pszExtsNext;
		while( pszExts ) 
		{
			if( *pszExts == '*' )pszExts++;

			pszExtsNext = strchr(pszExts, ';' );
			CString	strExt;
			if( pszExtsNext )
			{
				pszExtsNext++;
				strExt = CString( pszExts, pszExtsNext-pszExts-1 );
			}
			else
				strExt = pszExts;

			TRACE( "Add filter %s\n", strExt );

			CString strA = dlg->m_strExtension;
			CString strB = strExt;

			strA.MakeLower();
			strB.MakeLower();

			if( !strExt.CompareNoCase( dlg->m_strExtension ) || strA.Find( strB ) != -1 )
			{
				dlg->m_ofn.nFilterIndex = i;
				break;
			}

			pszExts = pszExtsNext;
		}
			

		pszNull2 = (char*)strchr(pszNull1+1, '\0');

		ASSERT( pszNull2 );

		pszNull1 = pszNull2+1;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CFileOpenDlg message handlers

void CFileOpenDlg::GetOFN(LPOPENFILENAME& lpofn)
{
	lpofn = &m_ofn;
}



int  CFileOpenDlg::Execute(DWORD dwFlags)
{
	dlg = this;
	m_bOpenFileDialog = (dwFlags & offOpen) ? TRUE : FALSE;
	m_ofn.Flags |= OFN_ENABLETEMPLATE|OFN_ENABLESIZING|OFN_EXPLORER|OFN_ALLOWMULTISELECT;
	m_ofn.Flags ^= OFN_ENABLETEMPLATE;
	m_ofn.Flags ^= OFN_ENABLESIZING;
	if(dlg->m_bOpenFileDialog == TRUE)
		m_ofn.Flags |= OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
	else
		m_ofn.Flags |= OFN_OVERWRITEPROMPT;
	//m_ofn.Flags ^= OFN_ALLOWMULTISELECT;

	_SetFilterIndex();

	LoadState(0);

	_SetDefaultExt();

	CString	strPathName = m_ofn.lpstrFile;
	if( !strPathName.IsEmpty() && (m_ofn.Flags & 0x80000000)==0 )
		::_splitpath( strPathName, 0, 0, 0, m_strExt.GetBuffer( _MAX_PATH ) );
	SetCurrentFilter();

	m_ofn.Flags = m_ofn.Flags & 0x7FFFFFFF;

	if(m_bOpenFileDialog==FALSE)
	{
		_KillExtension();
	}

	FireOnBeforeExecute();


	if((!m_strLastPath.IsEmpty()) && m_bOpenFileDialog==TRUE)
	{
		m_ofn.lpstrInitialDir = dlg->m_strLastPath;
	}
	
	dlg = NULL;

	//paul 30.01.2003 бред какой-то, причем тут CustomizeMode
	//CBCGToolBar::SetCustomizeMode();

	::SetTimer(this->GetSafeHwnd(), 691, 100, (TIMERPROC)___TimerProc);

	int res = CFileDialog::DoModal();
	m_nRes = res;

	::KillTimer(this->GetSafeHwnd(), 691);

	//paul 30.01.2003
	//CBCGToolBar::SetCustomizeMode(FALSE);

	FireOnAfterExecute();


	//if file has no extension, append it
	{
		char	szDrv[_MAX_DRIVE], szPath[_MAX_PATH], szName[_MAX_PATH], szExt[_MAX_PATH];
		CString	str = GetPathName();
		if( m_bSeveralFilesInUse )
		{
			POSITION pos = GetStartPosition();
			str = GetNextPathName( pos );
		}

		::_splitpath( str, szDrv, szPath, szName, szExt );

		if( ::strlen( szExt ) == 0 )
		{
			int	nFilter = m_ofn.nFilterIndex;
			char	*pszExt = 0;
			char	*pszFilterName = (char*)m_ofn.lpstrFilter;

			while( nFilter )
			{
				pszExt = pszFilterName+strlen( pszFilterName )+1;
				if( !*pszExt )break;
				pszFilterName = pszExt+strlen( pszExt )+1;
				if( !*pszFilterName )break;
				nFilter--;
			}

			if( pszExt )
			{
				if( *pszExt == '*' )pszExt++;
				strcpy( szExt, pszExt );
				char	*pszNext = ::strchr( szExt, ';' );
				if( pszNext )*pszNext = 0;
			}
			::_makepath( m_ofn.lpstrFile, szDrv, szPath, szName, pszExt );
		}
	}

	//strExt.ReleaseBuffer();

	CString str1, str2, strDir;
	str1 = GetPathName();
	str2 = dlg->m_strLastPath;
	if(str2.IsEmpty() || str1.Find(str2) >= 0)
	{
		str2 = str1;
	}
	if(m_bSeveralFilesInUse && str1 != str2)
	{
		strDir = str1 + '\\' + str2;
		memcpy(dlg->m_ofn.lpstrFile, strDir.GetBuffer(_MAX_PATH), _MAX_PATH);
		strDir.ReleaseBuffer();
	}
	str1 = GetPathName();


	SaveState(0);


	char* pszNull2 = 0;
	char* pszNull1 = pszNull2 = (char*)dlg->m_ofn.lpstrFilter;
	
	for(int i = 0; i < (int)dlg->m_ofn.nFilterIndex; i++)
	{
		pszNull1 = (char*)strchr(pszNull1, '\0');
		if(pszNull1 == NULL)
			break;
		if(i + 1 < (int)dlg->m_ofn.nFilterIndex)
		{
			pszNull2 = (char*)strchr(pszNull1+1, '\0');
			pszNull1 = pszNull2+1;
		}
		if(pszNull2 == NULL)
			break;
	}
	dlg->m_strExtension = (LPCSTR)pszNull1+1;


	if (dlg->m_pscrollHorz)
		delete dlg->m_pscrollHorz;
	if (dlg->m_pscrollVert)
		delete dlg->m_pscrollVert;

	UnhookWindowsHookEx(HookHandle);

	{
		sptrINamedData sptrData(dlg->m_punkData);
		ASSERT(sptrData != 0);
		sptrData->DeleteEntry(0); //remove all
	}
	if(	dlg->m_sptrLong != 0)	
	{
		dlg->m_sptrLong->Abort();
		dlg->m_sptrLong = 0;
	}
	if (dlg->m_punkView != 0)
	{
		{
		sptrIWindow sptrWin(dlg->m_punkView);
		ASSERT(sptrWin != 0);
		sptrWin->DestroyWindow();
		}
		dlg->m_punkView->Release();
		dlg->m_punkView = 0;
	}
	if (dlg->m_punkContext != 0)
	{
		{
		sptrIDataContext	sptrContext(dlg->m_punkContext);
		ASSERT(sptrContext != 0);
		sptrContext->AttachData(0);
		}
		dlg->m_punkContext->Release();
		dlg->m_punkContext = 0;

	}
	if (dlg->m_punkData != 0)
	{
		dlg->m_punkData->Release();
		dlg->m_punkData = 0;
	}

	if(m_sptrMainFrame)
	{
		m_sptrMainFrame->DetachMsgListener(GetControllingUnknown());
		m_sptrMainFrame = 0;
	}
	// Active view de-facto changed, and it's needed to redraw active view - related UI-items.
	// (e.g. FishContextViewer)
	IApplicationPtr sptrApp(GetAppUnknown());
	IUnknownPtr punkActiveDoc;
	sptrApp->GetActiveDocument(&punkActiveDoc);
	IDocumentSitePtr sptrDSActiveDoc(punkActiveDoc);
	IUnknownPtr punkActiveView;
	if (sptrDSActiveDoc != 0) sptrDSActiveDoc->GetActiveView(&punkActiveView);
	if (punkActiveView != 0)
		::FireEvent(GetAppUnknown(), szEventActivateObject, punkActiveView, NULL);	

	//m_target.GetControllingUnknown()->Release();
	dlg = NULL;

//	ULONG ref = Release();

	return res;
}

void CFileOpenDlg::DoRecalcLayout()
{
	_try(CFileOpenDlg, DoRecalcLayout);
	{
		CRect rcClient;
		CWnd wndControl;
		int nWidth, nHeight;
		CWnd* pParent = ((CFileDialog*)dlg)->GetParent();
	
		if (!pParent)
			return;
		pParent->GetClientRect(&rcClient);
		if (rcClient == CRect(0, 0, 0, 0))
			return;
	
		const int bottomBorder = 20;
		const int leftBorder = 10;
		const int topBorder = 10;
	
	//st sizer
		CRect rcStSizer;
		rcStSizer.bottom = rcClient.bottom;//max(1, ::GetSystemMetrics(SM_CYDLGFRAME));
		rcStSizer.top = rcStSizer.bottom - 2*bottomBorder/3;
		rcStSizer.right = rcClient.right;//max(1, ::GetSystemMetrics(SM_CXDLGFRAME));
		rcStSizer.left = rcStSizer.right - 2*bottomBorder/3;
	
	//progress
		CRect rcProgress;
		rcProgress.left = rcClient.left + 1;//max(1, ::GetSystemMetrics(SM_CXDLGFRAME));
		rcProgress.bottom = rcStSizer.bottom-1;
		rcProgress.top = rcStSizer.top;
		rcProgress.right = rcStSizer.left - 3;
		
	//st_filetype
		wndControl.Attach(::GetDlgItem(pParent->GetSafeHwnd(), ID_ST_FILE_TYPE)); 
		CRect rcStFType;
		wndControl.GetWindowRect(&rcStFType);
		nWidth = rcStFType.Width();
		nHeight = rcStFType.Height();
		rcStFType.left = rcClient.left + leftBorder;
		rcStFType.bottom = rcProgress.top - bottomBorder;
		rcStFType.top = rcStFType.bottom - nHeight;
		rcStFType.right = rcStFType.left + nWidth;
		wndControl.Detach();
	//st_filename
		wndControl.Attach(::GetDlgItem(pParent->GetSafeHwnd(), ID_ST_FILE_NAME)); 
		CRect rcStFName;
		wndControl.GetWindowRect(&rcStFName);
		nWidth = rcStFName.Width();
		nHeight = rcStFName.Height();
		rcStFName.left = rcClient.left + leftBorder;
		rcStFName.bottom = rcStFType.top - bottomBorder;
		rcStFName.top = rcStFName.bottom - nHeight;
		rcStFName.right = rcStFName.left + nWidth;
		wndControl.Detach();
	//pb_open
		wndControl.Attach(::GetDlgItem(pParent->GetSafeHwnd(), ID_PB_OPEN)); 
		CRect rcPbOpen;
		wndControl.GetWindowRect(&rcPbOpen);
		nWidth = rcPbOpen.Width();
		nHeight = rcPbOpen.Height();
		rcPbOpen.right = rcClient.right - leftBorder;
		rcPbOpen.left = rcPbOpen.right - nWidth;
		
		wndControl.Detach();
	//pb_cancel
		CRect rcPbCancel;
		rcPbCancel.right = rcPbOpen.right;
		rcPbCancel.left = rcPbOpen.left;
	//pb_help
		CRect rcPbHelp;
		rcPbHelp.right = rcPbOpen.right;
		rcPbHelp.left = rcPbOpen.left;
		

			
	//cb_filetype
		wndControl.Attach(::GetDlgItem(pParent->GetSafeHwnd(), ID_CB_FILE_TYPE)); 
		CRect rcCbFType;
		wndControl.GetWindowRect(&rcCbFType);
		nWidth = rcCbFType.Width();
		nHeight = rcCbFType.Height();
		rcCbFType.left = rcStFType.right + 1;
		rcCbFType.bottom = rcStFType.bottom;
		rcCbFType.top = rcCbFType.bottom - nHeight;
		//rcCbFType.right = rcStFInfo.left - bottomBorder;//rcCbFType.left + nWidth;
		wndControl.Detach();
		if (rcCbFType.Height() - rcStFType.Height() > 0) 
			rcStFType.OffsetRect(0, -(rcCbFType.Height() - rcStFType.Height())/2);

	//ed_filename
		wndControl.Attach(::GetDlgItem(pParent->GetSafeHwnd(), ID_ED_FILE_NAME)); 
		CRect rcEdFName = NORECT;
//		if( wndControl.GetSafeHwnd() )
		{
			wndControl.GetWindowRect(&rcEdFName);
			nWidth = rcEdFName.Width();
			nHeight = rcEdFName.Height();
			rcEdFName.left = rcCbFType.left;
			rcEdFName.bottom = rcStFName.bottom;
			rcEdFName.top = rcEdFName.bottom - nHeight;
			rcEdFName.right = rcCbFType.right;
			wndControl.Detach();
		}
		if (rcEdFName.Height() - rcStFName.Height() > 0) 
			rcStFName.OffsetRect(0, -(rcEdFName.Height() - rcStFName.Height())/2);
		


	//st_lookin
		wndControl.Attach(::GetDlgItem(pParent->GetSafeHwnd(), ID_ST_LOOK_IN)); 
		CRect rcStLookIn;
		wndControl.GetWindowRect(&rcStLookIn);
		nWidth = rcStLookIn.Width();
		nHeight = rcStLookIn.Height();
		rcStLookIn.left = rcClient.left + leftBorder;
		rcStLookIn.top = rcClient.top + topBorder;
		rcStLookIn.bottom = rcStLookIn.top + nHeight;
		rcStLookIn.right = rcStLookIn.left + nWidth;
		wndControl.Detach();
	//cb_lookin
		wndControl.Attach(::GetDlgItem(pParent->GetSafeHwnd(), ID_CB_LOOK_IN)); 
		CRect rcCbLookIn;
		wndControl.GetWindowRect(&rcCbLookIn);
		dlg->ScreenToClient(&rcCbLookIn);
		nWidth = rcCbLookIn.Width();
		nHeight = rcCbLookIn.Height();
		//rcCbLookIn.left = rcCbFType.left;
		//rcCbLookIn.bottom = rcStLookIn.bottom; 
		//rcCbLookIn.top = rcCbLookIn.bottom - nHeight;
		//rcCbLookIn.right = rcCbLookIn.left + nWidth;
		wndControl.Detach();
		//if(rcCbLookIn.bottom-rcStLookIn.bottom > 0)
		//rcStLookIn.OffsetRect(0, -(rcCbLookIn.bottom-rcStLookIn.bottom)/2);
		int nH = rcStLookIn.Height();
		rcStLookIn.bottom = rcCbLookIn.bottom -  (rcCbLookIn.Height()-nH)/2;
		rcStLookIn.top = rcStLookIn.bottom - nH;
		

		rcPbOpen.top = rcCbLookIn.bottom + bottomBorder/2;
		rcPbOpen.bottom = rcPbOpen.top + nHeight;
		rcPbCancel.top = rcPbOpen.bottom + bottomBorder/2;
		rcPbCancel.bottom = rcPbCancel.top + nHeight; 
		rcPbHelp.top = rcPbCancel.bottom + bottomBorder/2;
		rcPbHelp.bottom = rcPbHelp.top + nHeight; 
		
	//lb_main 
		CRect rcLbMain;
		CRect rcStPreview;
		CRect rcScrbHorz;
		CRect rcScrbVert;
		rcLbMain.top = rcPbOpen.top;
		rcLbMain.left = rcClient.left + leftBorder;
		rcLbMain.bottom = rcEdFName.top - bottomBorder/2;
		rcLbMain.right = rcPbOpen.left - leftBorder;
		dlg->m_nListBoxWidth = rcLbMain.Width();
		dlg->m_nListBoxHeight = rcLbMain.Height();
		if (dlg->m_curMode == dmPreview)
		{
			rcStPreview.right = rcLbMain.right;
			rcStPreview.top = rcLbMain.top;
			rcStPreview.bottom = rcLbMain.bottom;	
			//rcLbMain.right /= 2;
			rcLbMain.right = rcLbMain.left + dlg->m_nListBoxWidth/2;
			rcStPreview.left = rcLbMain.right + leftBorder;
			//dlg->m_nListBoxWidth /= 2;
			rcScrbHorz.left = rcStPreview.left;
			rcScrbHorz.right = rcStPreview.right - ::GetSystemMetrics(SM_CXVSCROLL)*2/3;
			rcScrbHorz.bottom = rcStPreview.bottom;
			rcScrbHorz.top = rcScrbHorz.bottom - ::GetSystemMetrics(SM_CYHSCROLL)*2/3;
			rcScrbVert.top = rcStPreview.top;
			rcScrbVert.bottom = rcStPreview.bottom - ::GetSystemMetrics(SM_CYHSCROLL)*2/3;
			rcScrbVert.right = rcStPreview.right;
			rcScrbVert.left = rcStPreview.right - ::GetSystemMetrics(SM_CXVSCROLL)*2/3;
		}

	//cb_filetype
		rcCbFType.right = rcLbMain.left + dlg->m_nListBoxWidth/2;
	//ed_filename
		rcEdFName.right = rcLbMain.left + dlg->m_nListBoxWidth/2;
	//lb file info
		CRect rcStFInfo;
		rcStFInfo.top = rcEdFName.top;
		rcStFInfo.bottom = rcCbFType.bottom; 
		rcStFInfo.right = rcPbOpen.left - leftBorder;
		rcStFInfo.left = rcEdFName.right + leftBorder;

		
		rcCbLookIn.right = (dlg->m_nMinDlgWidth - leftBorder*2 -  rcPbOpen.Width())/2+2;
		
		//toolbar
		nToolBarLeft = rcCbLookIn.right + leftBorder;
		nToolBarTop = rcCbLookIn.top-2;
		nToolBarBottom = rcCbLookIn.bottom-2;
		nToolBarRight = rcClient.right - leftBorder;
		
		BOOL bShowHelp = ::GetValueInt(::GetAppUnknown(), "FileOpen", "ShowHelp", FALSE);		
		HDWP hdwp = ::BeginDeferWindowPos( 30 );
		DWORD	dwFlags = SWP_NOACTIVATE|SWP_NOZORDER;
		//hide garbage
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_CH_READ_ONLY), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
		//reposition
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_ST_FILE_NAME), HWND_NOTOPMOST, rcStFName.left, rcStFName.top, rcStFName.Width(), rcStFName.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_ST_FILE_TYPE), HWND_NOTOPMOST, rcStFType.left, rcStFType.top, rcStFType.Width(), rcStFType.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_ED_FILE_NAME), HWND_NOTOPMOST, rcEdFName.left, rcEdFName.top, rcEdFName.Width(), rcEdFName.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_CB_FILE_TYPE), HWND_NOTOPMOST, rcCbFType.left, rcCbFType.top, rcCbFType.Width(), rcCbFType.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_ST_LOOK_IN), HWND_NOTOPMOST, rcStLookIn.left, rcStLookIn.top, rcStLookIn.Width(), rcStLookIn.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_CB_LOOK_IN), HWND_NOTOPMOST, rcCbLookIn.left, rcCbLookIn.top, rcCbLookIn.Width(), rcCbLookIn.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_PB_OPEN), HWND_NOTOPMOST, rcPbOpen.left, rcPbOpen.top, rcPbOpen.Width(), rcPbOpen.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_PB_CANCEL), HWND_NOTOPMOST, rcPbCancel.left, rcPbCancel.top, rcPbCancel.Width(), rcPbCancel.Height(), dwFlags);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), ID_PB_HELP), HWND_NOTOPMOST, rcPbHelp.left, rcPbHelp.top, rcPbHelp.Width(), rcPbHelp.Height(), bShowHelp?dwFlags|SWP_SHOWWINDOW:dwFlags|SWP_HIDEWINDOW);
		hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_PROGRESS), HWND_NOTOPMOST, rcProgress.left, rcProgress.top, rcProgress.Width(), rcProgress.Height(), dwFlags);
	//	if(!m_bWeUnder98Chikaka)
			hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_ST_SIZER), HWND_NOTOPMOST, rcStSizer.left, rcStSizer.top, rcStSizer.Width(), rcStSizer.Height(), dwFlags);
		hdwp = ::DeferWindowPos(hdwp, hwndListCtrl==0? ::GetDlgItem(pParent->GetSafeHwnd(), ID_LB_MAIN) : ::GetParent(hwndListCtrl), HWND_NOTOPMOST, rcLbMain.left, rcLbMain.top, rcLbMain.Width(), rcLbMain.Height(), dwFlags);

		if (dlg->m_curMode == dmPreview)
		{
			hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_PREVIEW), HWND_NOTOPMOST, rcStPreview.left, rcStPreview.top, rcStPreview.Width(), rcStPreview.Height(), dwFlags);
			if(dlg->m_bNeedShowFileInfo)
			{
				rcStFInfo.InflateRect(1,1,1,1);
				//hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_LIST_FILE_INFO), HWND_NOTOPMOST, rcStFInfo.left, rcStFInfo.top, rcStFInfo.Width(), rcStFInfo.Height(), dwFlags);
				//rcStFInfo.DeflateRect(3,3,3,3);
				hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_LIST_FILE_INFO+200), HWND_NOTOPMOST, rcStFInfo.left, rcStFInfo.top, rcStFInfo.Width(), rcStFInfo.Height(), dwFlags);
			}
			else
			{
				//hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_LIST_FILE_INFO), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
				hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_LIST_FILE_INFO+200), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
			}

			CWnd* wndView = GetWindowFromUnknown(dlg->m_punkView);
			if (wndView && ::IsWindow(wndView->m_hWnd))
			{
				int nWidth = rcStPreview.Width()- ((dlg->m_bCurViewNeedScroll) ? ::GetSystemMetrics(SM_CXVSCROLL)*2/3 +2 : 0);
				int nHeight = rcStPreview.Height() - ((dlg->m_bCurViewNeedScroll) ? ::GetSystemMetrics(SM_CYHSCROLL)*2/3 +2 : 0);
				wndView->SetWindowPos(&wndNoTopMost, 1, 1, nWidth-1, nHeight-1, dwFlags);

				if (!dlg->m_bCurViewNeedScroll)
				{
					sptrIScrollZoomSite sptrZoom(dlg->GetControllingUnknown());
					CSize size;
					if(sptrZoom != 0)
					{
						sptrZoom->GetClientSize(&size);
						double dX = ((double)nWidth)/size.cx;
						double dY = ((double)nHeight)/size.cy;
						sptrZoom->SetZoom(min(dX, dY));
					}
					sptrZoom = 0;
				}
			}

			if (dlg->m_bCurViewNeedScroll)
			{
				hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_SCROLLBAR_HORZ), HWND_NOTOPMOST, rcScrbHorz.left, rcScrbHorz.top, rcScrbHorz.Width(), rcScrbHorz.Height(), dwFlags);
				hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_SCROLLBAR_VERT), HWND_NOTOPMOST, rcScrbVert.left, rcScrbVert.top, rcScrbVert.Width(), rcScrbVert.Height(), dwFlags);
			}
			else
			{
				hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_SCROLLBAR_HORZ), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
				hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_SCROLLBAR_VERT), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
			}
		}
		else
		{
			hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_PREVIEW), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
			hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_SCROLLBAR_HORZ), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
			hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_SCROLLBAR_VERT), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
			//hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_LIST_FILE_INFO), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
			hdwp = ::DeferWindowPos( hdwp, ::GetDlgItem(pParent->GetSafeHwnd(), IDC_LIST_FILE_INFO+200), HWND_NOTOPMOST, 0, 0, 0, 0, dwFlags);
		}

		::EndDeferWindowPos( hdwp );

		::EnableWindow(::GetDlgItem(pParent->GetSafeHwnd(), ID_PB_HELP), TRUE);

		dlg->m_pscrollHorz->Invalidate();
		dlg->m_pscrollVert->Invalidate();
		

	//if(m_bWeUnder98Chikaka)
	//		::MoveWindow(hwndToolBar, nToolBarLeft, nToolBarTop, nToolBarRight, nToolBarBottom, TRUE);
	}
	_catch;
}


BOOL CFileOpenDlg::OnInitDialog() 
{
	CFileDialog::OnInitDialog();
//	::ShowWindow(::GetDlgItem(::GetParent(m_hWnd), IDHELP), SW_HIDE);
	
	hwndListCtrl = 0;
	//DragAcceptFiles();
	m_target.Register( CWnd::GetParent() );

	HookHandle = SetWindowsHookEx(WH_CALLWNDPROC, (HOOKPROC) Hooker,(HINSTANCE)NULL, (DWORD)GetCurrentThreadId());

	dlg = this;
	m_bSeveralFilesInUse = false;
	m_bCurViewNeedScroll = false;
	m_bNeedShowFileInfo = false;

	//m_ofn.nFilterIndex = dlg->m_nExtention;
	
	CRect rc;
	CWnd  wnd;
	wnd.Attach(::GetParent(this->m_hWnd));
	wnd.GetWindowRect(&rc);
	m_nMinDlgWidth = rc.Width()+180;//225;
	m_nMinDlgHeight = rc.Height()+40;
	if (m_nWidthDlg == 0)
		m_nWidthDlg = rc.Width()+180;
	if (m_nHeightDlg == 0)
		m_nHeightDlg = rc.Height()+40;
	wnd.MoveWindow(rc.left, rc.top, m_nWidthDlg, m_nHeightDlg);
	//m_stFileInfo.Create(NULL, WS_CHILD|WS_VISIBLE|SS_SUNKEN, NORECT, &wnd, IDC_ST_FILE_INFO);
	//m_lbFileInfo.CreateEx(/*WS_EX_TRANSPARENT|*/WS_EX_CLIENTEDGE, "ListBox", "NULL", WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|LBS_NOINTEGRALHEIGHT|LBS_NOSEL|LBS_OWNERDRAWFIXED, NORECT, &wnd, IDC_LIST_FILE_INFO);
	//m_lbFileInfo.Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|LBS_NOINTEGRALHEIGHT|LBS_NOSEL/*|LBS_OWNERDRAWFIXED*/, NORECT, &wnd, IDC_LIST_FILE_INFO);
	//m_lbFileInfo.SetFont(&m_fontInfo);
	//m_lbFileInfo.SetFont(GetFont());
	//SetClassLong(m_lbFileInfo.GetSafeHwnd(), GCL_HBRBACKGROUND, (LONG)GetSysColorBrush(COLOR_BTNFACE));	
	//DWORD dwErr = GetLastError();

	//CString strInfoCapt;
	//strInfoCapt.LoadString(IDS_INFO_CAPTION);
	//m_lbFileInfo.Create("", WS_CHILDWINDOW|WS_VISIBLE|BS_GROUPBOX, NORECT, &wnd, IDC_LIST_FILE_INFO);
	//m_lbFileInfo.SetFont(GetFont());
	m_stFileInfo.Create("",WS_CHILDWINDOW|WS_VISIBLE|SS_LEFTNOWORDWRAP|SS_SUNKEN, NORECT, &wnd, IDC_LIST_FILE_INFO+200);
	m_stFileInfo.SetFont(GetFont());


	m_Progress.Create(WS_CHILD|WS_VISIBLE|PBS_SMOOTH, NORECT, &wnd, IDC_PROGRESS);
	m_stPreview.Create(NULL, WS_CHILD|WS_VISIBLE|SS_SUNKEN, NORECT, &wnd, IDC_PREVIEW);
	m_stSizer.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, CRect(0,0,0,0), &wnd, IDC_ST_SIZER);
	m_pscrollHorz = new CScrollBar();
	m_pscrollVert = new CScrollBar();
	m_pscrollHorz->Create( WS_CHILD|SBS_HORZ|WS_VISIBLE, NORECT, &wnd, IDC_SCROLLBAR_HORZ);
	m_pscrollVert->Create( WS_CHILD|SBS_VERT|WS_VISIBLE, NORECT, &wnd, IDC_SCROLLBAR_VERT);
	wnd.Detach();

	
	_SetListBoxStyle();

	DoRecalcLayout();

	EnumChildWindows((HWND)::GetParent(this->m_hWnd), EnumChildProc, (LPARAM)::GetParent(this->m_hWnd));

	
//create namedata
	HRESULT hr;

	BSTR bstr;
	CLSID	clsid;
	CString strProgID = szNamedDataProgID; 
	bstr = strProgID.AllocSysString();
	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return FALSE;
	::SysFreeString( bstr );
	hr = CoCreateInstance(clsid,
       GetControllingUnknown(), CLSCTX_INPROC_SERVER,
       IID_IUnknown, (LPVOID*)&m_punkData);

	if( hr != S_OK)
		return FALSE;
	
	strProgID = szContextProgID; 
	bstr = strProgID.AllocSysString();
	if( ::CLSIDFromProgID( bstr, &clsid ) )
		return FALSE;
	::SysFreeString( bstr );
	hr = CoCreateInstance(clsid,
       GetControllingUnknown(), CLSCTX_INPROC_SERVER,
       IID_IUnknown, (LPVOID*)&m_punkContext);
	
	if(hr != S_OK)
		return FALSE;

	{
		sptrIDataContext	sptrContext(m_punkContext);
		ASSERT(sptrContext != 0);
		if (sptrContext->AttachData(m_punkData) != S_OK)
			return FALSE;
	}
	


//attach to main frame msg's
	m_sptrMainFrame = GetAppUnknown();
	if(m_sptrMainFrame != 0)
		m_sptrMainFrame->AttachMsgListener(GetControllingUnknown());

	//LONG style = WS_POPUP|WS_BORDER|WS_CLIPSIBLINGS| WS_CLIPCHILDREN|WS_CAPTION|WS_SYSMENU|DS_CONTEXTHELP|DS_3DLOOK|DS_SETFONT;
	//SetWindowLong(::GetParent(this->m_hWnd), GWL_STYLE, style);

	HWND hWnd;
	sptrIApplication sptrApp(GetAppUnknown());
	sptrApp->GetMainWindowHandle( &hWnd );
	::DragAcceptFiles( hWnd, false );

	FireOnDialogInited();

	return TRUE;
}

void CFileOpenDlg::OnPreview()
{
	OnFileNameChange();
}

void CFileOpenDlg::OnEndPreview()
{
	_try(CFileOpenDlg, OnEndPreview);
	{
		m_bNewPreview = true;
		{
		sptrINamedData		sptrData(dlg->m_punkData); 
		ASSERT(sptrData != 0);
		sptrData->DeleteEntry(0); //remove all
		}
		if (dlg->m_punkView != 0)
		{
			{
			sptrIWindow sptrWin(dlg->m_punkView);
			sptrWin->DestroyWindow();
			}
			dlg->m_punkView->Release();
			dlg->m_punkView = 0;
		}
	}
	_catch;
}

void CFileOpenDlg::OnTypeChange()
{
	_SetDefaultExt();
	//kill'em all
	if (dlg->m_punkView != 0)
	{
		{
		sptrIWindow sptrWin(dlg->m_punkView);
		sptrWin->DestroyWindow();
		}
		dlg->m_punkView->Release();
		dlg->m_punkView = 0;
	}
	dlg->m_bCurViewNeedScroll = false;
	dlg->m_bNeedShowFileInfo = false;
	DoRecalcLayout();
}

void CFileOpenDlg::OnFolderChange()
{
	/*CString strPath = GetPathName();
	if(!strPath.IsEmpty())
		dlg->m_strLastPath = strPath;*/
	_SetListBoxStyle();
	//kill'em all
	if (dlg->m_punkView != 0)
	{
		{
		sptrIWindow sptrWin(dlg->m_punkView);
		sptrWin->DestroyWindow();
		}
		dlg->m_punkView->Release();
		dlg->m_punkView = 0;
	}
	dlg->m_bCurViewNeedScroll = false;
	dlg->m_bNeedShowFileInfo = false;
	DoRecalcLayout();
}

void _MakePropStr(_variant_t var, CString bstrText, CString& strInfo, DWORD dwFlags)
{
	CString strVal = "";
	strInfo = "";
	if(var.vt == VT_BSTR)
		strVal = (const char *)(_bstr_t)var;
	else
	{
		if(var.intVal >= 0)
			strVal.Format("%d", var.intVal);
	}
	if(strVal.IsEmpty())return;
	if((dwFlags & ffpfTextAfterProp) == ffpfTextAfterProp)
	{
		strInfo.Format("%s %s",(const char *)strVal , (const char *)bstrText);
	}
	else
	{
		strInfo.Format("%s %s", (const char *)bstrText, (const char *)strVal);
	}
	if(dwFlags & ffpfNeedBrackets)
	{
		strInfo.Insert(0, "(");
		strInfo.Insert(strInfo.GetLength(), ")");
	}
}

static CCriticalSection	criticalSection, criticalSection1;
static bool s_bCanEnter = true;
static bool s_bKillQueueProceed = false;
static CString s_strPath = "";
static DWORD s_dwTimeToCall = 0;

void CALLBACK EXPORT ___TimerProc(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	if(s_bCanEnter && s_strPath != "" && GetTickCount() >= s_dwTimeToCall && dlg)
		dlg->DoFileNameChange();
}

void CFileOpenDlg::OnFileNameChange()
{
	// A.M. fix, BT 3941.
	CFileDialog::OnFileNameChange();
	s_strPath = GetPathName();
	s_dwTimeToCall = GetTickCount()+300;
}

void _SelectObjectIfNoActive(IUnknown *punkView, LPCSTR lpstrType)
{
	IDataContext2Ptr ptrContext(punkView);
	if (ptrContext == 0) return;
	_bstr_t bstrType(lpstrType);
	LONG_PTR lPos = 0;
	ptrContext->GetFirstSelectedPos(bstrType, &lPos);
	if (lPos != 0)
		return; // Any object already selected
	ptrContext->GetFirstObjectPos(bstrType, &lPos);
	while (lPos)
	{
		IUnknownPtr punk;
		ptrContext->GetNextObject(bstrType, &lPos, &punk);
		if (punk != 0)
		{
			ptrContext->SetObjectSelect( punk, 1 );
			break;
		}
	}
}


void CFileOpenDlg::DoFileNameChange()
{
	CString strPath;
	if(s_bCanEnter && s_strPath != "")
		strPath = s_strPath;
	else
		strPath = GetPathName();
	
	if(	dlg->m_sptrLong != 0)	
	{
		dlg->m_sptrLong->Abort();
		dlg->m_sptrLong = 0;
	}
	//TRACE("@@@@@@@OnFileNameChange() %s\n", (const char*)strPath);


	if(!s_bCanEnter)
	{
		s_bKillQueueProceed = true;
		s_strPath = strPath;
		return;
	}

	s_bCanEnter = false;
	s_strPath = "";


	m_bSeveralFilesInUse = false;
	CString str1, str2;
	str1 = strPath;
	if(!str1.IsEmpty())
	{	
		//str2 = str1.Mid(dlg->m_ofn.nFileOffset);
		int nNullPos1 = str1.Find('\"');
		int	nNullPos2 = str1.Find('\"', nNullPos1+1);
		if(nNullPos1 >=0 && nNullPos2 >= 0)
		{
			dlg->m_strLastPath = strPath;
			str2 = str1.Mid(nNullPos1+1, nNullPos2-nNullPos1-1);
			strPath = str2;
			//int nPoint = str2.Find('.');
			//str1 = str2.Left(nPoint);
			CWnd* pParent = ((CFileDialog*)dlg)->GetParent();
			HWND hwnd = ::GetDlgItem(pParent->GetSafeHwnd(), ID_ED_FILE_NAME);
			::SetWindowText(hwnd, str2);
			m_bSeveralFilesInUse = true;
		}
	}

	//paul 21.04.2003
	/*
	WIN32_FIND_DATA findData;
	HANDLE hFind = FindFirstFile(strPath, &findData);
	if(hFind == INVALID_HANDLE_VALUE)
	{
		dlg->m_strLastPath.Empty();
		FindClose(hFind);
		s_bCanEnter = true;
		return;
	}
	FindClose(hFind);
	*/
	{
		HANDLE hfile = ::CreateFile( strPath, 0, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );
		if( hfile == INVALID_HANDLE_VALUE )
		{
			dlg->m_strLastPath.Empty();			
			s_bCanEnter = true;
			return;
		}

		::CloseHandle( hfile );	hfile = 0;
	}


	
	if((((dlg->m_strLastPath == strPath) && (dlg->m_sptrLong != 0)) || strPath.IsEmpty()) && !m_bNewPreview)
	{
		s_bCanEnter = true;
		return;
	}
	if(	dlg->m_sptrLong != 0)	
	{
		TRACE("@@@@@@@!!!!!!!!Long operation need abort!!!! %s\n", (const char*)strPath);
		//dlg->m_sptrLong->Abort();
		//dlg->m_sptrLong = 0;
	}

	dlg->m_strLastPath = strPath;

	//Sleep(2000);
	/*MSG message;
	while(::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&message);
		::DispatchMessage(&message);
	}*/
		
	
	if (dlg->m_curMode == dmPreview)
	{	
		sptrIFileFilter3 sptrFileFilter = 0;
		{
			sptrIApplication	sptrApp( GetAppUnknown() );
//			ReleaseCapture();
			m_bNewPreview = false;
			dlg->m_stFileInfo.SetWindowText("");
			{
			sptrINamedData		sptrData(dlg->m_punkData); 
			ASSERT(sptrData != 0);
			sptrData->DeleteEntry(0); //remove all
			}
			if (dlg->m_punkView != 0)
			{
				{
				sptrIWindow sptrWin(dlg->m_punkView);
				sptrWin->DestroyWindow();
				}
				dlg->m_punkView->Release();
				dlg->m_punkView = 0;
			}			
			//sptrIApplication sptrApp(GetAppUnknown());
			IUnknown* punk;
			sptrIShellDocTemplate sptrShellDocTempl;
			sptrIView sptrView;
			_bstr_t bstrName(strPath);

			sptrApp->GetMatchDocTemplate(bstrName, &punk );
			sptrShellDocTempl = punk;
			if( punk )punk->Release();
			punk = 0;
			if (sptrShellDocTempl != 0)
				sptrShellDocTempl->GetFileFilterByFile(bstrName, &punk);
			else
			{
				s_bCanEnter = true;
				return;
			}
			sptrIFileDataObject sptrFile;
			sptrINamedData		sptrData(dlg->m_punkData); 
			sptrIDataContext	sptrContext(dlg->m_punkContext); 
			ASSERT(sptrData != 0);
			ASSERT(sptrContext != 0);

			if (punk != 0)
			{
				sptrFileFilter = punk;
				dlg->m_sptrLong = punk;
				if (dlg->m_sptrLong != 0)
					dlg->m_sptrLong->AttachNotifyWindow(dlg->m_hWnd);
				punk->Release();
				if (sptrFileFilter->AttachNamedData(sptrData, sptrContext)!=S_OK)
				{
					s_bCanEnter = true;
					return;
				}
				sptrFile = sptrFileFilter;
			}
			else
			{
				sptrFile = sptrData;
			}


			//criticalSection1.Unlock();

			s_bKillQueueProceed = false;

			HRESULT hr = sptrFile->LoadFile(bstrName);

			//andy - detach data here
			if (sptrFileFilter)
					sptrFileFilter->AttachNamedData( 0, 0 );

			if( hr  == S_OK )
			{

				//TRACE("@@@@@@@Load file == OK %s\n", (const char*)strPath);
				//criticalSection.Lock();

				dlg->m_sptrLong = 0;
				sptrIDataTypeManager sptrTypeManager(sptrData);
				BSTR bstrType;
				sptrTypeManager->GetType(0, &bstrType);
				CString strType(bstrType);
				::SysFreeString(bstrType);
				// A.M. fix. BT 4215.  ака€-то проблема при показе статистических вьюх
				// в этом окне. ѕока будем показывать экран "Ќет изображени€".
				if (strType == _T("StatTable"))
					strType = _T("Image");
				DWORD dwMatch;
				CString strViewID = FindMostMatchView(strType, dwMatch);
				if (strViewID.IsEmpty())
				{
					s_bCanEnter = true;
					return;
				}
				BSTR bstr;
				CLSID	clsid;
				HRESULT hr;
				bstr = strViewID.AllocSysString();
				if( ::CLSIDFromProgID( bstr, &clsid ) )
				{
					s_bCanEnter = true;
					return;
				}
				::SysFreeString( bstr );
				hr = CoCreateInstance(clsid,
				GetControllingUnknown(), CLSCTX_INPROC_SERVER, IID_IUnknown, (LPVOID*)&dlg->m_punkView);
				if (dlg->m_punkView == 0 || hr != S_OK)
				{
					s_bCanEnter = true;
					return;
				}
		
				CRect	rectWnd;
				dlg->m_stPreview.GetClientRect( &rectWnd );
				rectWnd.InflateRect(-1, -1, 0, 0);
				/*dlg->m_strType = strType;
				if (strType == "Script")
				{
					//need scrollbars
					rectWnd.bottom -= ::GetSystemMetrics(SM_CYHSCROLL)*2/3;
					rectWnd.right -= ::GetSystemMetrics(SM_CXVSCROLL)*2/3;
				
				}*/

				sptrIWindow sptrWin(dlg->m_punkView);
				if (sptrWin == 0)
				{
					s_bCanEnter = true;
					return;
				}


				sptrIScrollZoomSite sptrZoom(GetControllingUnknown());
				sptrZoom->SetAutoScrollMode(TRUE);

				sptrWin->SetWindowFlags(wfPlaceInDialog);	//for prevent registrate drop targer

				if (sptrWin->CreateWnd(dlg->m_stPreview.m_hWnd, rectWnd, WS_CHILD|WS_VISIBLE, WM_USER+120) != S_OK)
				{
					s_bCanEnter = true;
					return;
				}
				// A.M. fix. BT 4215. ≈сли при сохранении во вьюхе не было показано ни одного
				// изображени€ (например была активна статистическа€ вьюха), то в превью будет
				// показан экран "No image". «аактивизируем вместо него первую попавшеес€
				// изображение.
				if (strType == _T("Image"))
					_SelectObjectIfNoActive(dlg->GetControllingUnknown(), strType);
				::FireEvent(GetAppUnknown(), szEventActivateObject, sptrWin, NULL);
				
				dlg->m_bCurViewNeedScroll = !GetAutoScrollMode();

				int nPropCount = 0;
				if(sptrFileFilter)
				{
					_variant_t var, var2;
					//BSTR bstrName;
					CString strInfo;
					sptrFileFilter->GetPropertyCount(&nPropCount);
					if(nPropCount > 0)
					{
						CFileStatus rStatus;
						CFile::GetStatus(strPath, rStatus);
						ULONGLONG nSizeK = rStatus.m_size / 1024;
						CString strFileSize;
						if(nSizeK != 0)
						{
							ULONGLONG nSizeM = nSizeK / 1024;
							if(nSizeM >= 1000)
							{
								//in Mbytes
								strFileSize.Format("%dM", nSizeM);
							}
							else
							{
								//in Kbytes
								strFileSize.Format("%dK", nSizeK);
							}
						}
						else
						{
							//in bytes
							strFileSize.Format("%d", rStatus.m_size);
						}
						DWORD dwFlags, dwGroup, dwGroup2;
						BSTR bstrText;
						_variant_t var;
						CString strInfo, strVal, str, strTotal;
						int i=0;
						while(i < nPropCount)
						{
							strInfo = "";
							sptrFileFilter->GetPropertyByNum(i, &var, &bstrText, &dwFlags, &dwGroup);
							//single line
							CString strText(bstrText);
							::SysFreeString(bstrText);
							_MakePropStr(var, strText, str, dwFlags);
							strInfo = str;
							i++;
							dwGroup2 = dwGroup; 
							if(dwGroup != 0)
							{
								while(dwGroup == dwGroup2)
								{
									sptrFileFilter->GetPropertyByNum(i, &var, &bstrText, &dwFlags, &dwGroup2);
									CString strText(bstrText);
									::SysFreeString(bstrText);
									if(dwGroup == dwGroup2)
									{
										_MakePropStr(var, strText, str, dwFlags);
										strInfo += "   ";
										strInfo += str;
										i++;
									}	
								}
								if(dwGroup == 1)
								{
									strInfo += "   ";
									str.LoadString(IDS_INFO_FILESIZE);
									strInfo += str;
									strInfo += " ";
									strInfo += strFileSize;
								}
							}
							if(!strInfo.IsEmpty())
							{
								strInfo.Insert(0, " ");
								strTotal += strInfo;
								if(i != nPropCount)
									strTotal += '\n';
							}
						//dlg->m_lbFileInfo.AddString(strInfo);
						}

						dlg->m_bNeedShowFileInfo = true;
						dlg->m_stFileInfo.SetWindowText(strTotal);
					}
				}
				else if( sptrShellDocTempl )
				{
					CString strInfo;
					IDocTemplatePtr ptr_dt( sptrShellDocTempl );
					if( ptr_dt )
					{
						BSTR bstr = 0;
						ptr_dt->GetDocTemplString( 2, &bstr );
						strInfo = bstr;
						if( bstr )
							::SysFreeString( bstr );	bstr = 0;

					}
					//sptrShellDocTempl-

					CFileStatus rStatus;
					CFile::GetStatus(strPath, rStatus);
					ULONGLONG nSizeK = rStatus.m_size / 1024;
					CString strFileSize;
					if(nSizeK != 0)
					{
						ULONGLONG nSizeM = nSizeK / 1024;
						if(nSizeM >= 1000)
						{
							//in Mbytes
							strFileSize.Format("%dM", nSizeM);
						}
						else
						{
							//in Kbytes
							strFileSize.Format("%dK", nSizeK);
						}
					}
					else
					{
						//in bytes
						strFileSize.Format("%d", rStatus.m_size);
					}


					strInfo += "   ";
					CString str;
					str.LoadString( IDS_INFO_FILESIZE );
					strInfo += str;
					strInfo += " ";
					strInfo += strFileSize;

					// vanek 22.10.2003 (доработка): отображение информации из NamedData документа
					if( sptrData )
					{
						_variant_t	var;
						CString		str;
						sptrData->GetValue( bstr_t( "DocumentInfo" ),  &var );
						str = var;
						if( !str.IsEmpty( ) )
						{
							strInfo += ("\n" + CString( var ) );
							var.Clear( );
						}

						sptrData->GetValue( bstr_t( "DocumentInfo2" ),  &var );
						str = var;
						if( !str.IsEmpty( ) )
						{
							strInfo += ("\n" + CString( var ) );
							var.Clear( );
						}

						sptrData->GetValue( bstr_t( "DocumentInfo3" ),  &var );
						str = var;
						if( !str.IsEmpty( ) )
						{
							strInfo += ("\n" + CString( var ) );                       
							var.Clear( );
						}
					}
					//
					
					dlg->m_stFileInfo.SetWindowText( strInfo );
					dlg->m_bNeedShowFileInfo = true;
				}

				//criticalSection.Unlock();

				CWnd* wndView = GetWindowFromUnknown(sptrWin);
				if( wndView &&	IsWindow(wndView->m_hWnd) )
				{
					if(dlg->m_bCurViewNeedScroll)
					{
						//dewflate window for place scroll bars
						//wndView->SendMessage(WM_SIZE);
						rectWnd.bottom -= ::GetSystemMetrics(SM_CYHSCROLL)*2/3 -1;
						rectWnd.right -= ::GetSystemMetrics(SM_CXVSCROLL)*2/3 -1;
						//wndView->SetWindowPos(&wndNoTopMost, 0, 0, rectWnd.Width(), rectWnd.Height(), SWP_NOMOVE|SWP_NOZORDER);
						DoRecalcLayout();
					}
					wndView->SendMessage(SW_SHOW);
					wndView->SendMessage(WM_INITIALUPDATE);
					wndView->SendMessage(WM_SIZE);
				}
				
				CSize size;
				if(sptrZoom == 0)
				{
					dlg->DoRecalcLayout();
					s_bCanEnter = true;
					return;
				}
				if (!dlg->m_bCurViewNeedScroll)
				{
					sptrZoom->GetClientSize(&size);
					double dX = ((double)rectWnd.Width())/size.cx;
					double dY = ((double)rectWnd.Height())/size.cy;
					sptrZoom->SetZoom(min(dX, dY));
	
					/*sptrZoom->GetClientSize(&size);
					if (size.cx < rectWnd.Width()) 
					{
						rectWnd.left += (rectWnd.Width()-size.cx)/2;
					}
					if (size.cy < rectWnd.Height()) 
					{
						rectWnd.top += (rectWnd.Height()-size.cy)/2;
					}
					sptrZoom->SetScrollOffsets(rectWnd);*/
				}
				//
					
			}
			else
			{
				//TRACE("@@@@@@@Load file FAIL %s\n", (const char*)strPath);

				CString strError;
				strError.LoadString(IDS_ERROR_LOAD_FILE);
				strError.Insert(0, " ");
				dlg->m_stFileInfo.SetWindowText(strError);
				if(	dlg->m_sptrLong != 0)	
				{
					dlg->m_sptrLong->Abort();
					dlg->m_sptrLong = 0;
				}
				dlg->m_bCurViewNeedScroll = false;
				dlg->m_bNeedShowFileInfo = true;

				sptrINamedData		sptrData(dlg->m_punkData); 
				if(sptrData != 0);
					sptrData->DeleteEntry(0); //remove all
				if (dlg->m_punkView != 0)
				{
					{
					sptrIWindow sptrWin(dlg->m_punkView);
					sptrWin->DestroyWindow();
					}
					dlg->m_punkView->Release();
					dlg->m_punkView = 0;
				}			
			}

			dlg->DoRecalcLayout();
			//andy - remove it
			/*if (sptrFileFilter)
					sptrFileFilter->AttachNamedData( 0, 0 );*/
			//m_lbFileInfo.UpdateWindow();
			m_stFileInfo.UpdateWindow();
			//criticalSection2.Unlock();
			//TRACE("@@@@@@@Leave critical section %s\n", (const char*)strPath);
			//return;

		}		
	}
	s_bCanEnter = true;
}

void CFileOpenDlg::OnInitDone()
{
}

LRESULT CFileOpenDlg::OnLongNotify(WPARAM wParam, LPARAM lParam)
{
	int pnStage, pnPosition, pnPercent;
	if(wParam == loaStart)
	{
		dlg->m_Progress.SetRange(0, 100);
	}
	else if(dlg->m_sptrLong != 0)	
	{
		dlg->m_sptrLong->GetCurrentPosition(&pnStage, &pnPosition, &pnPercent);
		dlg->m_Progress.SetPos(pnPercent);
	}

	if(wParam == loaTerminate || wParam == loaComplete)
	{
		//m_sptrLong = 0;
		//criticalSection1.Unlock();
		dlg->m_Progress.SetPos(0);
		return 0;
	}
	MSG message;

	sptrIApplication	sptrApp( GetAppUnknown() );
	if (sptrApp == 0)
		return 1;

	//sptrApp->ProcessMessage();
	//hwndListCtrl
	while(::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
	{
		if(s_bKillQueueProceed)
			break;
		if(message.message == WM_LBUTTONDBLCLK && message.hwnd == hwndListCtrl)
		{
			if(	dlg->m_sptrLong != 0)	
			{
				dlg->m_sptrLong->Abort();
				dlg->m_sptrLong = 0;
			}

			//remove this msg
			//::GetMessage(&message, NULL, 0, 0);
			//fill queue 
			::PostMessage(message.hwnd, WM_LBUTTONDOWN, message.wParam, message.lParam);
			::PostMessage(message.hwnd, WM_LBUTTONUP, message.wParam, message.lParam);
			::PostMessage(message.hwnd, WM_LBUTTONDBLCLK, message.wParam, message.lParam);
			return 0;
		}
		//::GetMessage(&message, NULL, 0, 0);
		if(message.message == WM_LBUTTONUP && message.hwnd == hwndListCtrl)
		{
			::SendMessage(message.hwnd, WM_CAPTURECHANGED, (WPARAM)0, (LPARAM)0);
			//TRACE0("WM_LBUTTONUP\n");
		}
		::TranslateMessage(&message);
		::DispatchMessage(&message);
		if(message.message == WM_LBUTTONDOWN && message.hwnd == hwndListCtrl)
		{
			::SendMessage(message.hwnd, WM_CAPTURECHANGED, (WPARAM)0, (LPARAM)0);
			//TRACE0("WM_LBUTTONDOWN\n");
		}
		
		
	}
	return 0;
}


bool CFileOpenDlg::SaveState(IUnknown *punkDD, const char* pszSection)
{
	IUnknown* punk; 
	if (punkDD)
		punk = punkDD;
	else
		punk = GetAppUnknown();
	if( !CheckInterface( punk, IID_INamedData ))
		return false;

 	if (strcmp(pszSection, "") == 0)
		pszSection = m_strSection;

	_variant_t	var;
	
	var = (long)dlg->m_nWidthDlg;
	::SetValue(punk, pszSection, "Width",  var);
	var = (long)dlg->m_nHeightDlg;
	::SetValue(punk, pszSection, "Height",  var);
//	CString strPath = dlg->m_strLastPath;
	if (m_nRes == IDOK)
	{
		CString strPath = dlg->GetPathName();
		int idx = 0;
		int res = 0;
		do
		{	idx = res;
			res = strPath.Find("\\", idx+1);
		}while(res >= 0);
		strPath = strPath.Left(idx+1);
		var = (const char*)strPath;
		::SetValue(punk, pszSection, "LastPath",  var);
	}
	var = (long)dlg->m_curMode;
	::SetValue(punk, pszSection, "CurrentMode",  var);
	//var = (long)GetWindowLong(hwndListCtrl, GWL_STYLE);
	//::SetValue(punk, pszSection, "ListStyle",  var);
	::SetValue(punk, pszSection, "LastExt",  dlg->GetCurrentFilter());

	punk->Release();
	return true;
}

bool CFileOpenDlg::LoadState(IUnknown *punkDD, const char* pszSection)
{
	IUnknown* punk; 
	if (punkDD)
		punk = punkDD;
	else
		punk = GetAppUnknown();
	if( !CheckInterface( punk, IID_INamedData ))
		return false;

	if (strcmp(pszSection, "") == 0)
		pszSection = m_strSection;

	_variant_t	var;
	dlg->m_nWidthDlg = ::GetValueInt(punk, pszSection, "Width", _variant_t((long)0));
	dlg->m_nHeightDlg = ::GetValueInt(punk, pszSection, "Height", _variant_t((long)0));
	dlg->m_strLastPath = ::GetValueString(punk, pszSection, "LastPath");
	dlg->m_strExt = ::GetValueString(punk, pszSection, "LastExt");
	//m_ofn.nFilterIndex = ::GetValueInt(punk, pszSection, "LastExt", _variant_t((long)0));
	dlg->m_nHeightDlg = ::GetValueInt(punk, pszSection, "Height", _variant_t((long)0));

	long lMode = ::GetValueInt(punk, pszSection, "CurrentMode", _variant_t((long)1));
	if(lMode == 1)
		dlg->m_curMode = dmList;
	else if(lMode == 2)
		dlg->m_curMode = dmReport;
	else 
		dlg->m_curMode = dmPreview;
	
	return true;
}

void CFileOpenDlg::OnEndSize()
{
	if(dlg->m_bDrawContentWhileDrag)return;
	CRect rc;
	::GetWindowRect(::GetParent(dlg->m_hWnd),&rc);

	CWindowDC	dc(0);
	int nBorderX = GetSystemMetrics(SM_CXSIZEFRAME);
	int nBorderY = GetSystemMetrics(SM_CYSIZEFRAME);
	dc.DrawDragRect(&dlg->m_rcLastDraw, CSize(nBorderX,nBorderY), NULL, CSize(nBorderX,nBorderY));

	::MoveWindow(::GetParent(dlg->m_hWnd), rc.left, rc.top, dlg->m_nWidthDlg, dlg->m_nHeightDlg, TRUE);
	DoRecalcLayout();
}

void CFileOpenDlg::OnStartSize()
{
	dlg->m_rcLastDraw = NORECT;
	BOOL bEnabled;
	::SystemParametersInfo(SPI_GETDRAGFULLWINDOWS, 0, &bEnabled, 0);
	dlg->m_bDrawContentWhileDrag = bEnabled == TRUE;
}

void CFileOpenDlg::OnOurSize(CPoint point)
{
	CPoint  pt;
	GetCursorPos( &pt );
	CRect rc;
	::GetWindowRect(::GetParent(dlg->m_hWnd),&rc);
	rc.right = pt.x+8;//max(1, point.x)+3;
	rc.bottom = pt.y+8;//max(1, point.y)+3;
	int nWidth  = max (dlg->m_nMinDlgWidth, rc.Width());
	int nHeight = max(dlg->m_nMinDlgHeight, rc.Height());
	if(nWidth != dlg->m_nWidthDlg || nHeight != dlg->m_nHeightDlg)
	{
		dlg->m_nWidthDlg = nWidth;
		dlg->m_nHeightDlg = nHeight;
		rc.right = rc.left + nWidth;
		rc.bottom = rc.top + nHeight;
		if(dlg->m_bDrawContentWhileDrag)
		{
			::MoveWindow(::GetParent(dlg->m_hWnd), rc.left, rc.top, dlg->m_nWidthDlg, dlg->m_nHeightDlg, TRUE);
			DoRecalcLayout();
		}
		else
		{
			CWindowDC	dc(0);
			//dc.Rectangle();
			//dc.SetROP2(R2_XORPEN);
			//CRect rcTmp = m_rcLastDraw;
			int nBorderX = GetSystemMetrics(SM_CXSIZEFRAME);
			int nBorderY = GetSystemMetrics(SM_CYSIZEFRAME);
			dc.DrawDragRect(&rc, CSize(nBorderX,nBorderY), ((dlg->m_rcLastDraw == NORECT)?NULL:&dlg->m_rcLastDraw), CSize(nBorderX,nBorderY));
			//dc.FrameRect(dlg->m_rcLastDraw, (CBrush*)::GetStockObject(BLACK_BRUSH));
			//dc.FrameRect(rc, (CBrush*)::GetStockObject(BLACK_BRUSH));
			dlg->m_rcLastDraw = rc;
			//::MoveWindow(::GetParent(dlg->m_hWnd), rc.left, rc.top, dlg->m_nWidthDlg, dlg->m_nHeightDlg, TRUE);
			//DoRecalcLayout();
		}
	}
}

void CFileOpenDlg::OnDropFiles(HDROP hDropInfo) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnDropFiles(hDropInfo);
}

CScrollBar *CFileOpenDlg::GetScrollBarCtrl( int sbCode )
{
	if (sbCode == SB_HORZ)
		return m_pscrollHorz;
	else if (sbCode == SB_VERT)
		return m_pscrollVert;
	return NULL;
}

CWnd *CFileOpenDlg::GetWindow()
{
	return (CWnd*)&dlg->m_stPreview;
}


long CFileOpenDlg::GetFlags() 
{
	// TODO: Add your property handler here
	return dlg->m_bOpenFileDialog ? offOpen : offSave;
}

void CFileOpenDlg::SetFlags(long nNewValue) 
{
	// TODO: Add your property handler here
	dlg->m_bOpenFileDialog = (nNewValue & offOpen) ? TRUE : FALSE;
}

BSTR CFileOpenDlg::GetTitle() 
{
	CString strResult;
	// TODO: Add your property handler here
	strResult = m_ofn.lpstrTitle;
	return strResult.AllocSysString();
}

void CFileOpenDlg::SetTitle(LPCTSTR lpszNewValue) 
{
	// TODO: Add your property handler here
	m_strTitle = lpszNewValue;
	m_ofn.lpstrTitle = m_strTitle;
}

BSTR CFileOpenDlg::GetFileName() 
{
	CString strResult;
	// TODO: Add your property handler here
	strResult = m_ofn.lpstrFile;
	return strResult.AllocSysString();
}

void CFileOpenDlg::SetFileName(LPCTSTR lpszNewValue) 
{
	// TODO: Add your property handler here
	strcpy(m_ofn.lpstrFile, lpszNewValue);
}

BSTR CFileOpenDlg::GetExtentions() 
{
	CString strResult;
	// TODO: Add your property handler here
	strResult = m_ofn.lpstrFilter;
	return strResult.AllocSysString();
}

void CFileOpenDlg::SetExtentions(LPCTSTR lpszNewValue) 
{
	// TODO: Add your property handler here
	CString strFilter(lpszNewValue);

	m_ofn.nFilterIndex = 0;
	char* pszNull1;
	char* pszNull2;

	pszNull1 = pszNull2 = (char*)dlg->m_ofn.lpstrFilter;

	int i = 0;
	while( *pszNull1 )
	{
		i++;
		pszNull1 = (char*)strchr(pszNull2, '\0');

		CString strExt(pszNull2, pszNull1-pszNull2+1);

    int nPos = strExt.Find(strFilter);
		if(nPos != -1)
    {
      if(strExt.Find("*.", nPos+strFilter.GetLength()) == -1)
      {
			  m_ofn.nFilterIndex = i;
			  break;
      }
		}

		pszNull2 = (char*)strchr(pszNull1+1, '\0');
		pszNull2++;
		pszNull1 = pszNull2;
		
	}
	
}

BOOL CFileOpenDlg::SaveDlgState(LPUNKNOWN punkNameData, LPCTSTR pszSection) 
{
	// TODO: Add your dispatch handler code here
	if (SaveState(punkNameData, pszSection))
		return TRUE;
	else
		return FALSE;
}

BOOL CFileOpenDlg::LoadDlgState(LPUNKNOWN punkNameData, LPCTSTR pszSection) 
{
	// TODO: Add your dispatch handler code here
	if (LoadState(punkNameData, pszSection))
		return TRUE;
	else
		return FALSE;
}

BOOL CFileOpenDlg::GetPreview() 
{
	// TODO: Add your property handler here
	if (dlg->m_curMode == dmPreview)
		return TRUE;
	else
		return FALSE;
}

void CFileOpenDlg::SetPreview(BOOL bNewValue) 
{
	// TODO: Add your property handler here
	if (bNewValue == TRUE)
		dlg->m_curMode = dmPreview;
	else
		dlg->m_curMode = dmList;

	_SetListBoxStyle();
}


void CFileOpenDlg::OnFinalRelease() 
{
	// TODO: Add your specialized code here and/or call the base class
	//CFileDialog::OnFinalRelease();
	delete this;
}
/*BOOL CFileOpenDlg::OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult )
{
	return CFileDialog::OnWndMsg(message, wParam, lParam, pResult);
}
*/

void CFileOpenDlg::GetDlgWindowHandle( HWND *phWnd )
{
	if(dlg != NULL)
	{
		if(phWnd)
			*phWnd = dlg->GetSafeHwnd();
	}
}

BSTR CFileOpenDlg::GetDefaultExtention() 
{
	CString strResult;
	// TODO: Add your property handler here
	strResult = dlg->m_ofn.lpstrDefExt;
	return strResult.AllocSysString();
}

void CFileOpenDlg::SetDefaultExtention(LPCTSTR lpszNewValue) 
{
	// TODO: Add your property handler here
	m_strDefExt = lpszNewValue;
	dlg->m_ofn.lpstrDefExt =  m_strDefExt;
}



void CFileOpenDlg::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	// TODO: Add your message handler code here and/or call default
	
	CFileDialog::OnGetMinMaxInfo(lpMMI);
}

//OnClose
//m_target.Revoke();

void CFileOpenDlg::OnDestroy() 
{
	m_target.Revoke();
	CFileDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	
}

BOOL CFileOpenDlg::OnFileNameOK()
{
	return CFileDialog::OnFileNameOK();
}

CString CFileOpenDlg::GetCurrentFilter()
{
	int	n = m_ofn.nFilterIndex;
	char	*pszNext = (char*)m_ofn.lpstrFilter;
	char	*pszExt;

	m_strExt = ".*";
	if( !pszNext )return m_strExt;
	if( !n )return m_strExt;
	n--;

	while( *pszNext )
	{
		pszExt = pszNext+strlen( pszNext )+1;
		pszNext = pszExt+strlen( pszExt )+1;


		if( !n )
		{
			m_strExt = pszExt+1;
			break;
		}

		n--;
	}

	return m_strExt;
}

void CFileOpenDlg::SetCurrentFilter()
{
	m_ofn.nFilterIndex = 0;
	int	n = 0;
	char	*pszNext = (char*)m_ofn.lpstrFilter;
	char	*pszExt;
	if( !pszNext )return;

 	while( *pszNext )
	{
		pszExt = pszNext+strlen( pszNext )+1;
		pszNext = pszExt+strlen( pszExt )+1;

		CString strA = pszExt+1;
		CString strB = m_strExt;

		strA.MakeLower();
		strB.MakeLower();

		if( !m_strExt.CompareNoCase( pszExt+1 ) || strA.Find( strB ) != -1 )
		{
			m_ofn.nFilterIndex = n+1;
			break;
		}

		n++;
	}
}

BSTR CFileOpenDlg::GetInitialDir() 
{
	CString strResult = m_strLastPath;
	// TODO: Add your property handler here
	return strResult.AllocSysString();
}

void CFileOpenDlg::SetInitialDir(LPCTSTR lpszNewValue) 
{
	// TODO: Add your property handler here
	m_strLastPath = lpszNewValue;
}

BOOL CFileOpenDlg::OnCommand( WPARAM wParam, LPARAM lParam )
{

	long nIdx = LOWORD(wParam)%FAVORITE_BASE_ID;

	CString strFavoritesDefPath = ::MakeAppPathName("Favorites") + "\\";
	CString strPathLink = ::GetValueString(GetAppUnknown(), "Paths", "FavoritesPath", strFavoritesDefPath);
	CString strPath;
	CString strExt;
	HRESULT hres; 

	if(nIdx == 0)
	{
		strPath = strPathLink;
	}

	if(nIdx > 0 && nIdx <= m_arrFavorites.GetSize())
	{
		IShellLink* psl; 
 		// Get a pointer to the IShellLink interface. 
		hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&psl); 
		if (SUCCEEDED(hres)) 
		{
			IPersistFile* ppf; 
			hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf); 
			if (SUCCEEDED(hres)) 
			{ 
				strPathLink += m_arrFavorites[nIdx-1];

				WORD wsz [MAX_PATH]; // buffer for Unicode string
				MultiByteToWideChar (CP_ACP, 0, strPathLink, -1, (LPWSTR)wsz, MAX_PATH);
				
				hres = ppf->Load((LPWSTR)wsz, STGM_READ);
				if(SUCCEEDED(hres))
				{
					hres = psl->Resolve(0, SLR_NO_UI);
					if(SUCCEEDED(hres))
					{
						WIN32_FIND_DATA	 wfd;
						char szGotPath [MAX_PATH];
						psl->GetDescription(strExt.GetBuffer(MAX_PATH), MAX_PATH);

						strExt.ReleaseBuffer();
						hres = psl->GetPath(szGotPath, MAX_PATH, &wfd, SLGP_UNCPRIORITY);
					
						if(SUCCEEDED(hres))
						{
							strPath = szGotPath;
						}
					}
				}
				ppf->Release();
			}
			psl->Release();
		}
	}


	TRACE( "\nCompare Extension" );

	if(!strPath.IsEmpty())
	{
		if(!strExt.IsEmpty())
		{
			TCHAR charControl = strExt[0];
			TCHAR charFirstControl = 0;
			long nCountSendControl = 0;

			char	*pszNext = (char*)m_ofn.lpstrFilter;
			char	*pszExt;
			long nExtIndex = 0;
			while( *pszNext )
			{
				CString str(pszNext);
				if(!charFirstControl)
					charFirstControl = str[0];
				if(str[0] == charControl)
					nCountSendControl++;

				nExtIndex++;

				int nPos = str.Find("(");
				if(nPos > 0)str = str.Left(nPos-1);

				TRACE( "\n[%s]  [%s]", str, strExt );

				if(str == strExt)
				{
					break;
				}
				pszExt = pszNext+strlen( pszNext )+1;
				pszNext = pszExt+strlen( pszExt )+1;
			}


			CString Ext(pszNext);
			if( !Ext.IsEmpty() )
			{  
				CComboBox *pWnd = ((CComboBox*)CWnd::FromHandle(::GetDlgItem(((CFileDialog*)dlg)->GetParent()->GetSafeHwnd(), ID_CB_FILE_TYPE)));
				int nCount = pWnd->GetCount();

				CString Str;

				int nID = 0;
				int nfound_idx = -1;
				for( int nID=0;nID<nCount;nID++ )
				{
					pWnd->GetLBText( nID, Str );
					char* psz_find = (char*)strstr( Str, Ext );
					const char* psz_buf_start = Str;
					if( psz_find == psz_buf_start )
					{
						nfound_idx = nID;
						break;
					}
				}
				/*
				commented by paul 8.08.2003 BT 3227. Ѕред какой то, зачем через одно место, непон€тно...
				do
				{
					::SendMessage(::GetDlgItem(((CFileDialog*)dlg)->GetParent()->GetSafeHwnd(), ID_CB_FILE_TYPE), WM_CHAR, (WPARAM)Ext[0], 0);

					pWnd->GetLBText( nID = pWnd->GetCurSel(), Str );
					nCount--;
					if( nCount < 0 )
						break;
				} while( Str != Ext );
				*/

				m_ofn.nFilterIndex = ( nfound_idx != -1 ? nfound_idx : 0 );
				::SendMessage( pWnd->GetSafeHwnd(), CB_SETCURSEL, m_ofn.nFilterIndex, 0 );
				CWnd *pParent = pWnd->GetParent();
				if (pParent)
				{
					int id = pWnd->GetDlgCtrlID();
					pParent->SendMessage(WM_COMMAND, MAKELONG(id,CBN_SELCHANGE), (LPARAM)pWnd->GetSafeHwnd());
					pParent->SendMessage(WM_COMMAND, MAKELONG(id,CBN_SELENDOK), (LPARAM)pWnd->GetSafeHwnd());
				}
			}
  			


// Comment by Maxim
/*
 			if(m_ofn.nFilterIndex != nExtIndex)
			{
				if(nCountSendControl > 0)
				{
					while(m_ofn.nFilterIndex != 1)
						::SendMessage(::GetDlgItem(((CFileDialog*)dlg)->GetParent()->GetSafeHwnd(), ID_CB_FILE_TYPE), WM_CHAR, (WPARAM)charFirstControl, 0);
				}

				for(long i = 0; i < nCountSendControl; i++)
					if(charFirstControl != charControl || i != 0)
						::SendMessage(::GetDlgItem(((CFileDialog*)dlg)->GetParent()->GetSafeHwnd(), ID_CB_FILE_TYPE), WM_CHAR, (WPARAM)charControl, 0);

			}
*/
		}

		IShellBrowser* browser = (IShellBrowser*)::SendMessage(((CDialog*)this)->GetParent()->GetSafeHwnd(), WM_USER+7, 0, 0L);
		if(browser)
		{	
			HWND hwnd = 0;
			browser->GetWindow(&hwnd);

			IShellFolder* pDesktop = 0;
			SHGetDesktopFolder(&pDesktop);
			if(pDesktop)
			{									
				ITEMIDLIST* pidl = 0;
				ULONG nEaten = 0;
				DWORD dwAttrib = 0;//SFGAO_FOLDER;
				hres = pDesktop->ParseDisplayName(GetSafeHwnd(), NULL, _bstr_t(strPath), &nEaten, &pidl, &dwAttrib);
				if(SUCCEEDED(hres))
				{
					hres = browser->BrowseObject(pidl, SBSP_ABSOLUTE|SBSP_DEFMODE|SBSP_DEFBROWSER);

					/*SHELLEXECUTEINFO  sei;         
					ZeroMemory(&sei, sizeof(sei));         
					sei.cbSize = sizeof(sei);
					sei.fMask = SEE_MASK_IDLIST | SEE_MASK_CLASSNAME;
					sei.lpIDList = pidl;         
					sei.lpClass = TEXT("folder");
					sei.hwnd = GetSafeHwnd();         
					sei.nShow = SW_SHOWNORMAL;
					sei.lpVerb = TEXT("explore");
					ShellExecuteEx(&sei);*/
				}
			}
		}
		

		return TRUE;
	}
	
	return FALSE;
}
