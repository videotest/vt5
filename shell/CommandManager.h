#if !defined(AFX_COMMANDMANAGER_H__F49AE624_0446_11D3_A5A3_0000B493FF1B__INCLUDED_)
#define AFX_COMMANDMANAGER_H__F49AE624_0446_11D3_A5A3_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "ActionControlWrp.h"
#include "aliaseint.h"
#include "BitmapProviderAdv.h"
#include "BCGToolBarImages.h"
// CommandManager.h : header file
//

class CShellFrame;
class CTemplateInfo;
class CShellDockBar;

/////////////////////////////////////////////////////////////////////////////
// CCommandManager command target
//#define ID_CMDMAN_BASE	1000
//#define ID_CMDMAN_MAX	2000
class CVtToolBarImages : public CBCGToolBarImages
{
public:
	void SetUserFlag( bool bSet );
	void DeleteAllImages();
	// [vanek] - 16.07.2004
	void SaveEx( LPCTSTR lpszBmpFileName, bool bsave_anyway = false );
};

class CBCGToolBar;

class CCommandManager : public CCmdTargetEx,
						public CNamedObjectImpl
{
	DECLARE_DYNCREATE(CCommandManager)
	ENABLE_MULTYINTERFACE();

public:
	CCommandManager();           // protected constructor used by dynamic creation
	virtual ~CCommandManager();
// Attributes
public:

	bool GetMessageString(UINT nID, CString& rMessage);
	bool GetToolTipString(UINT nID, CString& rMessage);

// Operations
public:
	void Init();
	void DeInit();


	void InitUserImages();

protected:
	void AddActionInfo( CActionInfoWrp *pinfo );
	void RemoveActionInfo( int idx );

	int AddGroup( const char *sz );
	void AddBitmap( CActionInfoWrp *pinfo );
	void AddMenuData( CActionInfoWrp *pinfo );
	void AddToolBarData( CActionInfoWrp *pinfo );
	void AddAccel( CActionInfoWrp *pinfo );

	//this function removes from menu definition string menu name and return it as result
	CString ExtractMenuName( CString &strMenu );	
	//return bitmap number in united namespace
	int GetBitmapNo( CActionInfoWrp *pinfo );

public:
	// [vanek] - 16.07.2004
	bool	_save_state( LPCSTR lpFileName, bool bsave_as = false );
	bool	_load_state(LPCTSTR FileName);


public:
	HMENU	GetMenu( const char *sz = 0 );
	bool	IsStateReady()const		{return m_bStateReady;}
//return "united" bitmap - each for all buttons
	struct ActionControlData
	{
		int		m_nLocalID;
		_bstr_t	m_bstrProgID;
	};
	struct ToolBarData
	{
		CUIntArray	m_arrIDs;
		CUIntArray	m_arrPicts;
		CUIntArray	m_arrBmps;
		CStringArray	m_arrStrs;

		CString	m_sGroupName;

		ToolBarData( const char *sz );
		~ToolBarData();

		void AddData( UINT nCmd, UINT nImageNo, UINT nBmpNo, const char *sz );
		long GetSize();
	};

	struct BitmapData
	{
		HBITMAP	hBitmap;
		GuidKey	ProviderKey;
		long	lBitmapIndex;
		long	lImagesOffset;
	};

	struct MenuData
	{
		HMENU	hMenu;
		bool	bDocumentMenu;
		bool	bPopupMenu;
		CString	strMenuName;
		int		idx;
	};


	int GetBitmapCount();
	//return bitmap number in united namespace
	BitmapData *GetBitmapData( CActionInfoWrp *pinfo );
	BitmapData *GetBitmapData( int idx );
	BitmapData *GetBitmapDataByBmpID( UINT nBmpID );


	int	GetToolBarCount();
	ToolBarData *GetToolBarData( int idx );

	int	GetMenuCount();
	MenuData *GetMenuData( const char *szData );
	MenuData *GetMenuData( int idx );


	CActionInfoWrp		*GetActionInfo( int idx );
	CActionInfoWrp		*GetActionInfo( const char *szName );
	CActionInfoWrp		*GetActionInfo( const GUID guid );
	CActionControlWrp	*GetActionControl( UINT nID );
	int GetActionInfoCount();
	int GetActionCommand( const char *szActionName );


	HACCEL	GetAcceleratorTable(){return m_hAccel;}
	UINT	GetAcceleratorEntries(){return m_uiAccelEntries;}
	

//dockable windows array access
	int GetDockWindowsCount();
	IUnknown *GetDockWindow( int idx );

	UINT ExecuteContextMenu( const char *szMenuName, CPoint *ppt = 0, CWnd *pwnd = 0, DWORD dwFlag  = 0 );
	UINT ExecuteContextMenuWithCreator( const char *szMenuName, CPoint *ppt = 0, CWnd *pwnd = 0, DWORD dwFlag  = 0 );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommandManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL



// Implementation
protected:

	CBCGToolBar	*GetToolBarNo( int iNo );
	CShellDockBar	*GetDockBarNo( int nNo );

	

	// Generated message map functions
	//{{AFX_MSG(CCommandManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	afx_msg void OnCommandAction( UINT nCmd );
	//afx_msg BOOL OnCommandActionEx( UINT nCmd );
	afx_msg void OnUpdateCommandAction( CCmdUI *pCmdUI );
//	afx_msg void OnCommandClassMenu( UINT nCmd );
	afx_msg void OnUpdateCommandClassMenu( CCmdUI *pCmdUI );
	afx_msg void OnUpdateCommandPopupMenu( CCmdUI *pCmdUI );
	DECLARE_MESSAGE_MAP()
public:
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCommandManager)
	afx_msg long GetBarsCount();
	afx_msg BSTR GetBarName(long BarNo);
	afx_msg long CreateNewBar(LPCTSTR BarName);
	afx_msg void MakeEmpty(long BarNo);
	afx_msg long FindNo(LPCTSTR Name);
	afx_msg void AddButton(long BarNo, LPCTSTR ActionName);
	afx_msg void ShowBar(long BarNo, long Visible);
	afx_msg void InsertButton(long BarNo, LPCTSTR ActionName, long InsertAfter);
	afx_msg void DeleteBar(long BarNo);
	afx_msg void LoadState(LPCTSTR FileName);
	afx_msg void SaveState(LPCTSTR FileName);
	afx_msg long AddOutlookBar(LPCTSTR szName);
	afx_msg BOOL IsBarVisible(long nBarNo);
	afx_msg long GetButtonCount(long nBarNo);
	afx_msg BSTR GetButton(long nBarNo, long nButtonNo);
	afx_msg BOOL IsOutlookToolBar(long nBarNo);
	afx_msg long GetDockBarsCount();
	afx_msg BSTR GetDockBarName(long nBarNo);
	afx_msg BOOL IsDockBarVisible(long nBarNo);
	afx_msg void ShowDockBar(long nBarNo, short bVisible);
	afx_msg long FindDockNo(LPCTSTR pszName);
	afx_msg  void SetCanCloseDockBar(long nBarNo, BOOL bCanClose);
	afx_msg  BOOL CanCloseDockBar( long nBarNo );
	afx_msg  LPDISPATCH GetDockBarImplByName(LPCTSTR szType) ;
	afx_msg  void SaveAsState(LPCTSTR FileName);
	afx_msg  BOOL HasAction(long BarNo, LPCTSTR ActionName);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	/*BEGIN_INTERFACE_PART(Man, ICommandManager)
		com_call ExecuteContextMenu( BSTR bstrTargetName, POINT point );
		com_call AddAction( IUnknown *punkActionInfo );
		com_call RemoveAction( IUnknown *punkActionInfo );
	END_INTERFACE_PART(Man)*/

	BEGIN_INTERFACE_PART(Man2, ICommandManager2)
		com_call ExecuteContextMenu( BSTR bstrTargetName, POINT point );
		com_call AddAction( IUnknown *punkActionInfo );
		com_call RemoveAction( IUnknown *punkActionInfo );
		com_call RegisterMenuCreator( IUnknown *punk );
		com_call UnRegisterMenuCreator( IUnknown *punk );
		com_call GetActionName( BSTR *pbstrActionName, int nID );
		com_call GetActionID( BSTR bstrActionName, int *pnID );
		com_call ExecuteContextMenu2( BSTR bstrTargetName, POINT point, DWORD dwFlag );
		com_call ExecuteContextMenu3( BSTR bstrTargetName, POINT point, DWORD dwFlag, UINT *puCmd );
	END_INTERFACE_PART(Man2)


	CPtrArray	m_ptrsInfo;

	CPtrArray	m_ptrsBitmapInfo;
	CPtrArray	m_ptrsToolBarInfo;
	CPtrArray	m_ptrsMenuInfo;
	CPtrArray	m_ptrsAccel;
	CPtrArray	m_ptrsControl;
	// menu helper
	sptrIMenuInitializer m_sptrMenuHelper;
	sptrIMenuInitializer2 m_sptrMenuHelper2;
	CCompManager	m_compBars;	//DockBar's window interfaces

	HACCEL	m_hAccel;
	UINT	m_uiAccelEntries;

	bool CheckIdx( int	idx );
	bool	m_bStateReady;
public:
	CVtToolBarImages	m_UserImages;
	IAliaseManPtr		m_ptrAliases;

protected:
	LongOperationAction m_loActionState;
public:
	LongOperationAction SetLongOperationActionState( LongOperationAction loa );
	LongOperationAction GetLongOperationActionState();

protected:
	CPtrList m_ptrTemplateInfoList;
	void AfterStateLoad();
	void BeforStateSave();
	CTemplateInfo* FindTemplateInfo( CString strTemplName );

	void DestroyTemplateInfoList();
	void CreateTemplateInfoList();
public:
	
	void UpdateShellFrameToolbarInfo( CShellFrame* pFrame );
	void CreateShellFrameToolbars( CShellFrame* pFrame );
	void GatherShellFrameToolbarsInfo( CShellFrame* pFrameExcept );

	static bool GetFrameOptions( CShellFrame* pFrame, CString& strTemplName, CString& strFrameName );
	static bool IsFrameCanHaveToolbar( CShellFrame* pFrame );

protected:
	// [vanek] : use overloaded icons for actions - 06.12.2004
	CAdvancedBitmapProvider m_advbmps;        

};

extern CCommandManager	g_CmdManager;

/////////////////////////////////////////CActionInfo warper
class CActionInfoWrp
{
public:
	CActionInfoWrp( IUnknown *pUnk );
	~CActionInfoWrp();

		//load information from action
	bool Init();
public:
	IUnknown *Unknown()
	{	m_pActionInfo->AddRef();return m_pActionInfo; }
//general information
	CString GetActionShortName()
	{ return m_sName; }
	CString GetActionUserName()
	{ return m_sUserName; }
	CString GetActionHelpString()
	{ return m_sToolTip; };
	CString GetActionGroupName()
	{ return m_sGroupName; }
//toolbar support
	int GetDefToolBarsCount()
	{ return (int)m_sToolBars.GetSize(); }
	const char *GetDefToolBarName( int iTBIdx )
	{ return m_sToolBars[iTBIdx]; }
//menu information
//return the menu count, where action defaultly present
	int	GetDefMenusCount()
	{ return (int)m_sMenus.GetSize(); }
	CString GetDefMenuName( int iMenuIdx )
	{ return m_sMenus[iMenuIdx]; }
//special action processing (such as - compo-box in toolbar, sub-menu in toolbar) - comming soom
//now return 0
	virtual CObject *GetInterfaceObject()
	{ return 0;}

	int GetLocalID();
	void SetLocalID( int nLocalID );

//bitmap information
	long GetBitmapIdx()
	{	return m_nBitmapIdx;}
	long GetPictureIdx()
	{	return m_lPictureIdx;}
	GuidKey GetBitmapProviderKey()
	{	return m_lBitmapProviderKey;}


	IUnknown *CreateAction();
public:
//operations
	CString MakeHelpTooltip();

	IActionInfo2	*m_pActionInfo;
	CCommandManager::BitmapData	*m_pBmpData;
protected:
	CStringArray	m_sMenus,
					m_sToolBars;
	CString			m_sName, 
					m_sUserName,
					m_sGroupName,
					m_sToolTip,
					m_sActionName;
//bitmap information
	long			m_nBitmapIdx;
	long			m_lPictureIdx;
	GUID			m_lBitmapProviderKey;
};

class CTemplateInfo
{
public:
	CTemplateInfo();
	~CTemplateInfo();

	CString m_strDocTemplate;
	//CString m_strFrameType;
	CPtrList m_ptrToolbarInfoList;
	void DestroyList();
protected:
	void Init();
	

};


class CToolBarInfo
{
public:
	CToolBarInfo( CString strName, CRect rc, UINT uiDocPos );
	CToolBarInfo();
	~CToolBarInfo();
	void CreateContext( CBCGToolBar* ptb, CFrameWnd* pParent );

	bool Serialize( CStringArrayEx& strings, bool bLoad, int& idx );

	CString GetName(){ return m_strName;}
	bool IsVisible(){ return m_bVisible;}
	CRect GetRect() { return m_rect;}
	UINT GetDocPos(){ return m_uiDocPos;}

protected:
	void Init();

	CString m_strName;
	bool	m_bVisible;
	CRect	m_rect;
	UINT	m_uiDocPos;
	bool	m_bFloating;

public:
	CStringArrayEx m_arBtnString;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMANDMANAGER_H__F49AE624_0446_11D3_A5A3_0000B493FF1B__INCLUDED_)
