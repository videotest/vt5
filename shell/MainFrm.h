// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__C48CE9BF_F947_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_MAINFRM_H__C48CE9BF_F947_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "settingswindow.h"
#include "MdiClient.h"
//#include "StatusBarMan.h"

#include "BCGDockContext.h"
#include "OutlookBar.h"

#include "wndmisc5.h"
#include "oledata.h"
#include "ShellToolBar.h"
#include "ShellStatusBar.h"
#include "ShellProgress.h"
#include "Shell.h"
#include "XPBarMgr.h"
#include "misc.h"

#include "\vt5\controls\xpbar_ctrl\xp_bar.h"

//#define IDS_MENU_TOOLBAR_CONTEXT ID_CMDMAN_BASE+555
//#define IDS_MENU_TOOLBAR_APPWINDOW ID_CMDMAN_BASE+556
//#define IDS_MENU_TOOLBAR_ACTIONLOG ID_CMDMAN_BASE+557

/*#define ID_FIRST_DOCKBAR	(ID_CMDMAN_BASE + 1000)
#define ID_LAST_DOCKBAR		(ID_CMDMAN_BASE + 1099)
#define ID_FIRST_TOOLBAR	(ID_CMDMAN_BASE + 1100)
#define ID_LAST_TOOLBAR		(ID_CMDMAN_BASE + 1199)
#define ID_CMD_POPUP_MIN	(ID_CMDMAN_BASE + 1200)
#define ID_CMD_POPUP_MAX	(ID_CMDMAN_BASE + 1299)
#define ID_OLE_VERB_MIN		(ID_CMDMAN_BASE + 1300)
#define ID_OLE_VERB_MAX		(ID_CMDMAN_BASE + 1399)*/


#define WM_IDLEUPDATE		(WM_USER+791)

struct CComboBoxInfo{
	HWND hWnd;
	IUnknownPtr ptrActionInfo;
};



class CMainFrame;
class CStatusBarMan;
class CShellToolDockBar;

class CMainFrameDropTarget : public COleDropTarget
{
protected:
	CMainFrame	*m_pMainFrame;
public:
	CMainFrameDropTarget();
	virtual void Register( CMainFrame *pFrm );
protected:
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
};


class CShellDockBar;




//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
class CShellFrame;

class CVTMiniDockFrameWnd : public CBCGMiniDockFrameWnd
{
	DECLARE_DYNCREATE(CVTMiniDockFrameWnd)
	CPoint m_ptStartDrag,m_ptStartPos;
public:
	CVTMiniDockFrameWnd() : m_ptStartDrag(-1,-1), m_ptStartPos(-1,-1)
	{}

// Operations
	virtual void RecalcLayout(BOOL bNotify = TRUE);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	//{{AFX_MSG(CMyMiniDockFrameWnd)
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
};


//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************


class CSplashWindow;
class render;

#include "\vt5\awin\win_base.h"

class CDistrMessageWnd : public win_impl
{
	LOGFONT m_lf;
public:
	CString m_strDecr;
	bool m_bRecreateOnDistr;
	CDistrMessageWnd()
	{
		::ZeroMemory( &m_lf, sizeof( m_lf ) );
		memset( &m_lf, 0, sizeof( LOGFONT ) );
		strcpy( m_lf.lfFaceName, "Arial" );
		m_lf.lfHeight = -20;
		m_lf.lfWeight = 400;
		m_lf.lfItalic = 0;
		m_lf.lfUnderline = 0;
		m_lf.lfStrikeOut = 0;
		m_lf.lfCharSet = DEFAULT_CHARSET;
		m_bRecreateOnDistr = true;
	}


	virtual long on_paint()
	{
		PAINTSTRUCT ps = {0};
		
		HDC hdc = ::BeginPaint( handle(), &ps );

		CString str = m_strDecr;

		RECT rc = {0};
		::GetClientRect( handle(), &rc );

		::SetBkMode( hdc, TRANSPARENT );

		HFONT hFont = ::CreateFontIndirect( &m_lf );
		
		HFONT hOldFont = (HFONT)::SelectObject( hdc, hFont );

		::DrawText( hdc, str, str.GetLength(), &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE );

		::SelectObject( hdc, hOldFont );

		::DeleteObject( hFont );

		::EndPaint( handle(), &ps );

		return 0L;
	}
	
	virtual long on_destroy()							
	{
		if( m_bRecreateOnDistr )
		{
			RECT rc = { 0, 0, 300, 50 };
			create( WS_POPUP | WS_VISIBLE, rc, 0, 0 );
			on_timer( 54321 );
		}
		return 1L;
	}

	virtual long on_create( CREATESTRUCT *pcs )
	{
		on_timer( 54321 );

		::SetTimer( handle(), 54321, 60000, 0 );
		return __super::on_create( pcs );
	}
	
	virtual long on_mousemove( const _point &point )
	{
		KillTimer( handle(), 54321);
		on_timer( 54321 );
		::SetTimer( handle(), 54321, 60000, 0 );

		return __super::on_mousemove( point );
	}
	
	virtual long on_timer( ulong lEvent )
	{
		if( lEvent == 54321 )
		{
			RECT rcMon = {0};

			RECT rc = {0};
			GetClientRect( handle(), &rc );

			::SystemParametersInfo( SPI_GETWORKAREA, 0, &rcMon, 0 );
			int x = rand() % ( rcMon.right - rc.right );
			int y = rand() % ( rcMon.bottom - rc.bottom );


			RECT _rc = { x, y, x + rc.right, y + rc.bottom };
			ClipOrCenterRectToMonitor( &rc  );
			SetWindowPos( handle(), NULL, _rc.left, _rc.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
		}
		return __super::on_timer( lEvent );
	}

	void ClipOrCenterRectToMonitor(LPRECT prc )
	{
		HMONITOR hMonitor;
		MONITORINFO mi;
		RECT        rc;
		int         w = prc->right  - prc->left;
		int         h = prc->bottom - prc->top;

		//
		// get the nearest monitor to the passed rect.
		//
		hMonitor = MonitorFromRect(prc, MONITOR_DEFAULTTONEAREST);

		//
		// get the work area or entire monitor rect.
		//
		mi.cbSize = sizeof(mi);
		GetMonitorInfo(hMonitor, &mi);

		rc = mi.rcWork;

		prc->left   = max(rc.left, min(rc.right-w,  prc->left));
		prc->top    = max(rc.top,  min(rc.bottom-h, prc->top));
		prc->right  = prc->left + w;
		prc->bottom = prc->top  + h;
	}
};

inline long cmp_bstr_t( _bstr_t psz1, _bstr_t psz2 )
{	return _tcsicmp( (char*)psz1, (char*)psz2 ); }

#include <BCGMDIFrameWnd.h>
#include <BCGContextMenuManager.h>
#include <BCGKeyboardManager.h>

class CMainFrame : public CBCGMDIFrameWnd,
	public CWindow2Impl,
	public CNamedObjectImpl,
	public CNumeredObjectImpl,
	public CHelpInfoImpl
{
	DECLARE_DYNAMIC(CMainFrame)

	ENABLE_MULTYINTERFACE();
public:
	CDistrMessageWnd m_DistrWnd;
	CMainFrame();

	bool	HelpMode();
	bool	ProcessHelpMessage( MSG *pmsg, IUnknown **ppunkHelpTarget );
	void	CheckActivePopup( MSG *pMsg );

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |

	void AddChild( CShellFrame* pChildFrame );
	void RemoveChild( CShellFrame* pChildFrame );

	POSITION GetFirstChildFramePos();
	CShellFrame* GetNextChildFrame( POSITION& pos );


	CShellFrame* GetActiveChildFrame(){
		return m_pActiveChildFrame;
	}
	
	CMDIChildWnd* _GetActiveFrame( );

	void EnableDocking(DWORD dwDockStyle);
	virtual void OnUpdateFrameMenu(HMENU hMenuAlt);

	//outlook bar functions
	//create a new toolbar inside shell outlook bar
	CBCGToolBar *CreateNewOutlookBar( const char *szName )		{return m_wndOutlook.AddFolderBar( szName );}
	void		DeleteOutlookBar( CBCGToolBar *pbar )			{m_wndOutlook.DeleteFolderBar( pbar );}

	//SDI mode support
	BOOL EnterSDIMode( );
	BOOL LeaveSDIMode( );

	BOOL IsSDIMode(){
		return m_bSDIMode;
	}
	virtual bool IsVerticalDockEnabled();


protected:		

	BOOL m_bSDIMode;
	CShellFrame* m_pActiveChildFrame;
	CPtrList m_children;	
	int m_nChildCount;

	CPrioritiesMgr m_PrioritiesMgr;

	virtual void OnUpdateFrameTitle(BOOL bAddToTitle);
	
	virtual CFrameWnd* GetActiveFrame();

	BOOL VerifyChild( CShellFrame* pChild);
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

    
public:
	bool EnableOutlookAnimation( bool bEnable );
	void SetNewActiveChild(CShellFrame* pNewChildFrame);

	void AddDockChildFrame( void* pBar){
		m_ptrDockWindows.AddTail( pBar );
	}

	void RemoveDockChildFrame( void* pBar){
		POSITION pos;
		if ((pos = m_ptrDockWindows.Find(pBar)) != NULL)
			m_ptrDockWindows.RemoveAt(pos);
	}
	void MakeForeground();


// Attributes
public:
	BOOL m_bEnableAccelerators;
	

	//void CheckMDICommandState( UINT nID );
	bool ExecuteMDICommand( UINT nID );
	void CreateNewWindow();

	CMDIChildWnd	*GetWindowByTitle( const char *sz );

	POSITION		GetFirstMDIChildPosition();
	CMDIChildWnd	*GetNextMDIChild( POSITION &pos );
	POSITION		GetFirstDockBarPos();
	CShellDockBar	*GetNextDockBar( POSITION &pos );


	void				ResetDockSites();
	CShellToolDockBar	*GetDockSite( int nDSCode );
	bool GetEnableChangeMenu(LPCTSTR lpstrMenuName);
public:
	void RegisterMDIChild( CMDIChildWnd *pwnd );
	void UnRegisterMDIChild( CMDIChildWnd *pwnd );
	void KillActivePopup();
	void HideSplash();
// Operations
public:

//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
// virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	virtual void HtmlHelp(DWORD_PTR dwData, UINT nCmd = 0x000F);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);	
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//}}AFX_VIRTUAL
	virtual void GetMessageString(UINT nID, CString& rMessage) const;
	//virtual BOOL OnWndMsg( UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult );
	

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	//CStatusBar			m_wndStatusBar;
	
public:
	//CStatusBarMan			m_wndStatusBar;
	CShellStatusBar			m_wndStatusBar;
	CShellProgress			m_wndShellProgress;
	CShellMenuBar			m_wndMenuBar;
	COutlookBar				m_wndOutlook;
	CXPBarMgr               m_wndXPBarHolder;
	CBCGContextMenuManager	m_ContextMenuManager;
	CBCGKeyboardManager		m_KeyboardManager;


	CSettingsWindow		m_wndSettings;
	CMdiClient			m_wndMDIClient;

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_DISPATCH_MAP()


//*****************************************************************************
//							MDI/PDI/DDI		support. START point			  |
//	afx_msg void OnUpdateMDIWindowCmd(CCmdUI* pCmdUI);
//	afx_msg BOOL OnNcActivate(BOOL bActive);
//	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//							MDI/PDI/DDI		support. END point				  |
//*****************************************************************************

	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnUpdateMDIWindowCmd(CCmdUI* pCmdUI);
	afx_msg BOOL OnNcActivate(BOOL bActive);	
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg BOOL OnMDIWindowCmd(UINT nID);
	afx_msg void OnWindowNew();
	afx_msg LRESULT OnNcHitTest(CPoint point);
    afx_msg void OnWindowPosChanging(LPWINDOWPOS lpWndPos);

	//}}AFX_MSG
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CMainFrame)
	CString m_strTitle;
	afx_msg void OnTitleChanged();
	CString m_strLabel;
	afx_msg void OnLabelChanged();
	afx_msg LPDISPATCH GetActiveFrameDisp();
	afx_msg void SetActiveFrame(LPDISPATCH newValue);
	afx_msg BOOL GetVisible();
	afx_msg void SetVisible(BOOL bNewValue);
	afx_msg BOOL GetMaximized();
	afx_msg void SetMaximized(BOOL bNewValue);
	afx_msg BOOL GetIconic();
	afx_msg void SetIconic(BOOL bNewValue);
	afx_msg void SaveState(LPCTSTR szFileName);
	afx_msg void RestoreState(LPCTSTR szFileName);
	afx_msg void SetWindowPos(short X, short Y, short CX, short CY);
	afx_msg long GetFramesCount();
	afx_msg LPDISPATCH GetFrame(long nPos);
	afx_msg void GetDesktopSize(VARIANT FAR* varCX, VARIANT FAR* varCY);
	afx_msg void GetWindowPos(VARIANT FAR* varX, VARIANT FAR* varY, VARIANT FAR* varCX, VARIANT FAR* varCY);
	afx_msg void ResetUserButtonNames();
	afx_msg void ShowSplash(LPCTSTR szFileName);
	afx_msg void ShowXPBar(BOOL bShow);
	afx_msg long AddToolbarPane(LPCTSTR szCaption, long lInsertPos, long lImageList, long nImage);
	afx_msg void XPExpandPane(long lpos, long state);
	afx_msg void XPAddButton(long lpos, LPCTSTR pszAction, long nImage, long nState);
	afx_msg void XPSetButton(long lPane, LPCTSTR szActionName, long nImage, long nState);
	afx_msg void XPRemovePane(long lPos);
	afx_msg void XPEnsureVisible(long lpos);
	afx_msg long CreateImageList(LPCTSTR szFileName, long cx, long cy, long nTransparent);
	afx_msg void DestroyImageList(long lImageList);
	afx_msg void DestroyAllImageLists();
	afx_msg void XPSetPaneParam(long lpos, long param);
	afx_msg long XPGetPaneParam(long lpos);
	afx_msg long XPGetFirstPane();
	afx_msg long XPGetNextPane(long lpos);
	afx_msg void XPSetImageList(long lImageList);
	afx_msg void LogFontSetBold(long lpos, long fBold);
	afx_msg void LogFontSetFace(long lpos, LPCTSTR pszFaceName);
	afx_msg void LogFontSetColor(long lpos, long color);
	afx_msg void LogFontSetHeight(long lpos, long nHeight);
	afx_msg void RenderSetLayout(long lpos, long left, long top, long right, long bottom);
	afx_msg void RenderSetRect(long lpos, long left, long top, long right, long bottom);
	afx_msg long RectCreate(long lpos_parent);
	afx_msg void RectSetColor(long lpos, long color);
	afx_msg long PageCreate(long lpos_parent);
	afx_msg long TipCreate(long lpos_parent);
	afx_msg void TipAddText(long lpos, LPCTSTR psz);
	afx_msg void TipSetIcon(long lpos, LPCTSTR pszFileName);
	afx_msg long ImageCreate(long lpos_parent);
	afx_msg void ImageSetBitmap(long lpos, LPCTSTR psz_filename);
	afx_msg void ImageSetTransparent(long lpos, long color);
	afx_msg long TextCreate(long lpos_parent);
	afx_msg void TextAddText(long lpos, LPCTSTR psz);
	afx_msg long MenuCreate(long lpos_parent);
	afx_msg void MenuAddItem(long lpos, LPCTSTR pszAction);
	afx_msg void DeleteAllDrawing();
	afx_msg void MDIClientUpdate();
	afx_msg void IdleUpdate();
	afx_msg void UpdateSystemSettings();
	afx_msg void UpdateSystemSettingsForCurrentDoc();
	afx_msg void UpdateSystemSettingsForDoc( LPDISPATCH lpDisp );
	afx_msg BOOL GetWindowLock();
	afx_msg void SetWindowLock(BOOL bWindowLock, BOOL bRecalcLayout);
	afx_msg BSTR XPCreateStatusItem(LPCTSTR pszItem);
	afx_msg void XPSetStatusItem(LPCTSTR pszGUID, LPCTSTR pszText);
	afx_msg void XPRemoveStatusItem(LPCTSTR pszGUID);
	afx_msg void XPSetDefaultStatusText(LPCTSTR sz);
	afx_msg void UpdateViewSettings();
	afx_msg void ShowHiddenForm();
	afx_msg BOOL GetEnableAccelerators();
	afx_msg void SetEnableAccelerators(BOOL bNewValue);
	//}}AFX_DISPATCH
	afx_msg LRESULT OnToolbarContextMenu(WPARAM,LPARAM);
	afx_msg LRESULT OnHelpCustomizeToolbars(WPARAM wp, LPARAM lp);
	afx_msg LRESULT OnToolbarReset(WPARAM,LPARAM);

	afx_msg LRESULT OnLongNotify(WPARAM wParam, LPARAM lParam);
	afx_msg void OnShowControlBar( UINT nCmd );
	afx_msg void OnShowToolBar( UINT nCmd );
	afx_msg LRESULT OnIdleUpdate( WPARAM, LPARAM );
	afx_msg LRESULT OnOutlookNotify( WPARAM, LPARAM );
	afx_msg LRESULT OnFindApp( WPARAM w, LPARAM l );
	afx_msg LRESULT OnIdleUpdateCmdUI( WPARAM, LPARAM );
	afx_msg LRESULT OnSetSheetButton( WPARAM, LPARAM );
	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnFormCaptionChanged( WPARAM, LPARAM l );

	BEGIN_INTERFACE_PART(Main, IMainWindow)
		//com_call ResetPagesList();
		//com_call AddSettingsPage( IUnknown *punk );
		com_call EnterSettingsMode(HWND* hwndSettings, LONG nHeight);
		com_call GetFirstDockWndPosition( long *plPosition );
		com_call GetNextDockWnd( IUnknown **ppunkDock, long *plPosition );
		com_call CreateDockBar(IUnknown* punk, UINT nID, HWND* pHWND);
		com_call KillDockBar(BSTR bstrName);
		com_call LeaveSettingsMode();
		com_call GetMDIClientWnd(HWND* pHwnd);
		com_call GetFirstToolbarPosition( long *plPosition );
		com_call GetNextToolbar( HWND *phwnd, long *plPosition );
		com_call ShowControlBar( HWND hwnd, long bShow );
		com_call CreateDockBarEx(IUnknown* punk, UINT nID, HWND* pHWND, DWORD dwFlags/*1 - not float now*/);
		com_call IsHelpMode( BOOL *pbool );
	END_INTERFACE_PART(Main);

	BEGIN_INTERFACE_PART(Toolbar, IMenuInitializer)
		com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu );
	END_INTERFACE_PART(Toolbar);

	BEGIN_INTERFACE_PART(UserInterface, IUserInterface)
		com_call GetFirstComboButton( long *plPosition );
		com_call GetNextComboButton( HWND* phWnd, IUnknown** ppunkInfo, long *plPosition );	
	END_INTERFACE_PART(UserInterface);

	BEGIN_INTERFACE_PART(TranslatorWnd, ITranslatorWnd)
		com_call TranslateAccelerator( MSG* pMsg, BOOL* pbTranslate );
	END_INTERFACE_PART(TranslatorWnd);

	//Ole in place edit support
	BEGIN_INTERFACE_PART(OleFrame, IOleFrame)
		com_call GetMainHwnd( HWND* phWndMain );
		com_call ActivateOleObject( IUnknown* punkOleObject, HWND* phwndFrame );
		com_call CreateStdOLEMenu( IUnknown* punkOleObjectData, IUnknown* punkOleObject );
		com_call DeactivateOleObject( IUnknown* punkOleObject );
		//Get menu wanna see in main frame when ole object activate
		com_call GetMenu( HMENU* phMenu, BSTR* pbstrCaption/*[size - 6]*/ );		
		//Get space whithout doockbars
		com_call GetPseudoClientRect( RECT* prect, BOOL bIncludeBorders );
		//Set space exclude OLE APP toolbars etc
		com_call SetOleBorders( LPCBORDERWIDTHS pBW );
		//GetAccel info
		com_call GetAccelInfo( HACCEL* phaccel, UINT* pcAccelEntries );

	END_INTERFACE_PART(OleFrame);

	void OnActivateOleObject();
	void OnDeActivateOleObject( bool brecalc_layout = true );

	void CreateOleMenu();
	void DestroyOleMenu();
	
	HMENU m_hOleMenu[OLE_MENU_SIZE];
	CString m_strOleMenuCaption[OLE_MENU_SIZE];
	HMENU m_hMenuMain;
	HMENU m_hMenuOleVerbs;

	void DestroyOleToolBarArray();
	void CreateOleToolBarArray();
	void HideOleToolbars();
	void ShowOleToolbars();

	CArray< HWND, HWND > m_arOleToolbar;

	bool m_bInOleEditMode;

	CRect m_rcOleBorders;
	bool m_bOleResizeFrame;

	GuidKey m_guidOleObject;
	void OnOleFrameResizeBorders();
	void OnOleFrameActivate( BOOL bActivate );

	IOleObjectUIPtr GetActiveOleObject();

	void _RecalcLayout( );

	afx_msg void OnOleVerb( UINT nCmd );

	//Ole in place edit support



	void _CreateDockBar(IUnknown* punk, UINT nID, HWND* pHWND, bool bfloat = true, bool bCaption = true);
	void _KillDockBar(CString strName = "");
	//virtual BOOL OnMenuButtonToolHitTest (CBCGToolbarButton* /*pButton*/, TOOLINFO* /*pTI*/);

public:
	bool IsInOleEditMode(){	return m_bInOleEditMode;}

	virtual BOOL GetToolbarButtonToolTipText (CBCGToolbarButton* /*pButton*/, CString& /*strTTText*/);
protected:	
	

	virtual CWnd *GetWindow(){return this;}
//special mode constants 
	void EnterMode(long nHeight);
	void LeaveMode();

	bool	m_bInSettingsMode;
	int		m_cySettingsWindowSize;

	CPtrList	m_ptrChildWindows;	//MDI child window list
	CPtrList	m_ptrDockWindows;	//DockBar list (not toolbar or menubar)
	CMainFrameDropTarget	
				m_target;

	
	CMap< CString, LPCSTR, bool, bool > m_mapFormDockDialogs;

	long	m_nOrigBottom;
	bool	m_bRestoreOnExit;

protected:
	CPtrList m_arComboButtons; //ComboButton		

public:
	bool RegisterComboButton( HWND hWnd, IUnknown* punkInfo );
	bool UnRegisterComboButton( HWND hWnd );

	POSITION GetFirstComboButton();
	CComboBoxInfo* GetNextComboButton( POSITION &pos );


	CSplashWindow	*m_pwndSplash;
//xpbar support
public:
	bool	IsXPBarAvailable()						{return m_hwndXPBar != 0;}
	void	UpdateClientStyle();
protected:
	HWND		m_hwndXPBar;
	long		m_nXPWidth;
	CList<HIMAGELIST, HIMAGELIST&>	m_listImageList;
	HIMAGELIST	m_hImageListAction, m_hImageListButtons;
	long		m_lposStatus, m_lposProperties, m_lposForm;
//render support
	render *get_render( long lpos, long type, const char *pfunc );
	long set_render( render *p, long lpos_p );

	virtual BOOL CanShowMenuBar();

	IActionManager	*m_pAM;

//window lock
	bool		m_bLockRecalc;

//script notify for LongOperation support
	int			m_nEnableScriptNotify;
	long		m_lStep;
	long		m_lLag;
	int			m_nScriptSetPosLastPos;	// position(%)
	DWORD		m_dwScriptSetPosTime;
	
	void		_init_script_notify( );
	HRESULT		_fire_script_event( LPCTSTR lpctstrEvent, LPCTSTR lpctstrActionName, BOOL bSetPos = FALSE, long lPos = 0 );
	BOOL		_get_action_name( IUnknown *punk, CString *pstrActionName );
    
public:
	void		SetLockRecalc( bool bset );
	bool		GetLockRecalc( );

	CString		m_strDefText;

	void		SetAccelTable( HACCEL haccel );

	UINT		m_nTimerID;
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// [vanek] BT2000:3928,3923 для Dispatch-методов MainFrame работы с xpbar вместо позиций пан - индексы пан;
	// каждой пане присваивается индекс по ее имени, т.о. имена пан в этом случае д.б. уникальными - 18.08.2004

	// мапа: <имя_паны> - <позиция_в_xpbar>, содержит список пан, содержащихся в xpbar
	_list_map_t<long, _bstr_t, cmp_bstr_t> m_map_xppanename2intpos;	

	// мапа: <имя_паны> - <уникальный_индекс_паны>,  если при добавлении паны ее нет в текущей мапе, она заноситься
	// в данную мапу и ей присваивается уникальный номер; ничего из мапы в процессе работы не удаляется, т.о. пана
	// с именем "Метафаза" будет всегда иметь один и тот же уникальный номер, даже если реально ее удалили, а потом
	// опять создали
	_list_map_t<long, _bstr_t, cmp_bstr_t> m_map_xppanename2extpos;	
	
	// вспомогательные функции для перевода внешней позиции паны (уникального номера) во внутреннюю (позиция в 
	// xpbar) и наоборот
	long	xppane_pos_ext2int( long lpos_ext );
	long	xppane_pos_int2ext( long lpos_int );
	// функция поиска позиции в соответсвующей мапе по внутренней/внешней позиции паны
	long	xppane_find_by_pos( long lpos_find, bool bext_pos, _bstr_t *pbstr_found_name );

	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	void UpdateAllowCustomizeButton();

// [vanek] : lock window showing - 20.10.2004
public:
    void SetLockShow(bool block);
	bool GetLockShow(void);

protected:
    bool	m_bLockShow;
public:
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
protected:
	long	m_nModeHeight;
	void ShowProgress(LPCTSTR text, LONG percent);

	enum 
	{
		dispidShowProgress = 65L
	};
};

#include "menuhash.h"
__declspec( dllimport )CObList		gAllToolbars;
__declspec( dllimport )CImageHash	g_ImageHash;
__declspec( dllimport )CMenuHash	g_menuHash;

/*POSITION GetFirstToolBarPosition();
CControlBar *GetNextToolBar( POSITION &rPos );
long	GetToolBarsCount();*/

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__C48CE9BF_F947_11D2_A596_0000B493FF1B__INCLUDED_)

