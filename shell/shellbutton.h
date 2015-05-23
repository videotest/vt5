#ifndef __shellbutton_h__
#define __shellbutton_h__

#include "action_ui_int.h"

#include "PopupInt.h"

	enum ShellButtonStyles
	{
		Flat = 0,
		SemiFlat = 1,
		Ordinary = 2,
		Round = 3
	};

class CActionControlWrp;
class CShellToolbarButton;

#define DECLARE_SERIAL__(class_name) \
	DECLARE_DYNCREATE(class_name) \
	AFX_API friend CArchive& AFXAPI operator>>(CArchive& ar, class_name* &pOb);


/////////////////////////////////////////////////////////////////////////////
class CShellPopupMenu : public CBCGPopupMenu
{
	DECLARE_SERIAL(CShellPopupMenu)
public:
	CShellPopupMenu();	
	virtual BOOL PostCommand(UINT uiCommandID);
	virtual BOOL UpdateCmdUI(CCmdUI *pCmdUI);

protected:
	CShellToolbarButton* m_pParentButton;
public:
	void SetParentButton( CShellToolbarButton* pParentButton );
	CShellToolbarButton* GetParentButton( );
	virtual bool CanProcessGlobalCmd(){	return true; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellPopupMenu)
	public:
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CShellPopupMenu)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
class CShellToolbarButton : public CBCGToolbarMenuButton, public  IHelpInfo
{
	DECLARE_SERIAL(CShellToolbarButton);
public:
	CShellToolbarButton();
	CShellToolbarButton(UINT uiID, HMENU hMenu, int iImage, LPCTSTR lpszText = NULL,
								BOOL bUserButton = FALSE);
	CShellToolbarButton(UINT uiID, int iImage, LPCTSTR lpszText = NULL,
								BOOL bUserButton = FALSE);

	~CShellToolbarButton();

	virtual void Serialize (CArchive& ar);
	virtual IUnknown *GetInterface( long lHit );
protected:
	virtual void OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
					BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
					BOOL bHighlight = FALSE,
					BOOL bDrawBorder = TRUE,
					BOOL bGrayDisabledButtons = TRUE);
	virtual void OnChangeParentWnd (CWnd* /*pWndParent*/);
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual void OnClosePopupMenu( int nCmd );
	virtual void CShellToolbarButton::OnMove();
	virtual void CShellToolbarButton::OnSize (int iSize);
	virtual void SetImage( int nImage );
	virtual BOOL OnContextHelp( CWnd* );
public:
	void SetButtonStyle( int nButtonStyle )				{m_nDrawStyle = (ShellButtonStyles)nButtonStyle;}
	void SetAlignLeft( bool bSet )						{m_bAlignLeft = bSet;}
	void SetMenuMode( bool bSet )						{m_bMenuMode = bSet;}
	void SetDrawAccel( bool bSet )						{m_bDrawAccel = bSet;}
	void EnableMenuSerialize( bool bEnable )			{m_bMenuSerialize = bEnable;}
public:
	static void					SetButtonsStyle( int nButtonStyle )		{s_nButtonStyle = nButtonStyle;};
	static ShellButtonStyles	GetButtonsStyle()						{return (ShellButtonStyles)s_nButtonStyle;}
	static void					SetButtonSize( CSize size );

	bool		IsMenuMode();

public:
	virtual bool IsPopupButton();
	virtual BOOL OnClickUp();

	// [vanek] : добавил bUseCollection - использование/неспользование коллекции - 10.11.2004
	void StoreString( CStringArray &strs, int &idx, int level, bool bUseCollection = true );
	com_call GetHelpInfo( BSTR *pbstrHelpFile, BSTR *pbstrHelpTopic, DWORD *pdwFlags );
    com_call STDMETHODCALLTYPE QueryInterface( const IID &	iid, void **ppvObject );
	virtual ULONG STDMETHODCALLTYPE AddRef( void){return 1;}
    virtual ULONG STDMETHODCALLTYPE Release( void) {return 1;}


protected:
	CString	m_strHelpFileName;
	CString	m_strTopic;
	static int s_nButtonStyle;
	ShellButtonStyles	m_nDrawStyle;
	CBCGToolBar			*m_pbar;
	bool				m_bAlignLeft;
	bool				m_bMenuSerialize;
	CActionControlWrp	*m_pChild;

//Popup support
protected:
	void CreatePopup( bool bConstructNow = false );
	void DestroyPopup();
	

	//void FillPopup();	

	ISubMenuPtr	m_ptrSubMenu;	

	virtual void _UpdateUI();	
public:
	IUnknown* GetSubMenuPtr();

public:
	virtual CBCGPopupMenu* CreatePopupMenu ();

	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay = TRUE);

	virtual void CopyFrom (const CBCGToolbarButton& src);	

	void OnPopupCommand(UINT uiCommandID);

protected:
	CShellPopupMenu* m_pPopupParent;
public:
	void SetParentPopup( CShellPopupMenu* pPopupParent );
	CShellPopupMenu* GetParentPopup( );


};

class CShellChooseToolbarButton : public CShellToolbarButton
{
	DECLARE_SERIAL(CShellChooseToolbarButton);
public:
	CShellChooseToolbarButton();
	void Initialize();
protected:
	virtual void OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
					BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
					BOOL bHighlight = FALSE,
						BOOL bDrawBorder = TRUE,
					BOOL bGrayDisabledButtons = TRUE);
	

	virtual void OnClosePopupMenu( int nCmd );
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay = TRUE);
	virtual BOOL OnClickUp();
	virtual BOOL OnPreClick (CWnd* pWnd );
	virtual void OnCancelMode ();

	virtual void Serialize (CArchive& ar);

	CBCGToolbarButton	*GetLastButton();
protected:
	int		m_nLastUsedCmd;
	CRect	m_rectLastMenuBtn;
	bool	m_bMenu;
	bool	m_bClickedOnMenu;
};


class CShellMenuButton : public CShellToolbarButton
{
	DECLARE_SERIAL(CShellMenuButton);
public:
	CShellMenuButton();
	CShellMenuButton(UINT uiID, int iImage, IUnknown *punkMenuProvider, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE);
	~CShellMenuButton();

	virtual void CopyFrom (const CBCGToolbarButton& src);

	void Initialize();
	virtual void Serialize (CArchive& ar);
protected:
	virtual void OnDraw (CDC* pDC, const CRect& rect, CBCGToolBarImages* pImages,
					BOOL bHorz = TRUE, BOOL bCustomizeMode = FALSE,
					BOOL bHighlight = FALSE,
					BOOL bDrawBorder = TRUE,
					BOOL bGrayDisabledButtons = TRUE);
	

	virtual void OnChangeParentWnd( CWnd* pwnd );
	virtual SIZE OnCalculateSize (CDC* pDC, const CSize& sizeDefault, BOOL bHorz);
	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay = TRUE);
	virtual BOOL OnClickUp();
	virtual BOOL OnPreClick (CWnd* pWnd );
	virtual void OnCancelMode ();
protected:
	virtual void _UpdateUI();

	virtual void OnMove();
	virtual void OnSize (int iSize);

protected:
	IUIDropDownMenuPtr	m_ptrMenu;
	CRect		m_rectLastMenuBtn;
	bool		m_bClickedOnMenu;
	int			m_nOldWidth;
	bool		m_bMenu;

	IUIControlPtr	m_ptr_action_ctrl;
};




/*
class CShellPopupMenuButon : public CShellToolbarButton
{
	DECLARE_SERIAL(CShellPopupMenuButon);
public:
	CShellPopupMenuButon();	
	CShellPopupMenuButon(UINT uiID, int iImage, IUnknown *punkSubMenuProvider, LPCTSTR lpszText = NULL, BOOL bUserButton = FALSE);
	

	virtual void Serialize(CArchive& ar);		

	virtual BOOL NotifyCommand( int nID );
	virtual BOOL OnClick (CWnd* pWnd, BOOL bDelay = TRUE);
protected:
	ISubMenuPtr	m_ptrSubMenu;
	virtual void _UpdateUI();

	bool CreateDefaultMenu();
	bool DestroyMenu();
	bool InitMenu();

public:
	
};
*/


//////////////////////////////////////////////////////////////////////
//
//	Class CShellMenuComboBoxButton
//
//////////////////////////////////////////////////////////////////////
		
										//CShellToolbarButton
class CShellMenuComboBoxButton : public CBCGToolbarComboBoxButton
{
	DECLARE_SERIAL(CShellMenuComboBoxButton);	
public:
	CShellMenuComboBoxButton();
	~CShellMenuComboBoxButton();

	CShellMenuComboBoxButton(UINT uiID, int iImage, 
							IUnknown *punkMenuProvider, 
							DWORD dwStyle = CBS_DROPDOWNLIST, 
							int iWidth = 0);	
	virtual void CopyFrom (const CBCGToolbarButton& src);

	virtual void Serialize(CArchive& ar);
	

	virtual CComboBox* CreateCombo (CWnd* pWndParent, const CRect& rect);
	virtual BOOL NotifyCommand (int iNotifyCode);	

protected:
	void Init( CComboBox* pCombo );
	void DeInit();	

	bool m_bWasCreateCombo;

protected:
	IUIComboBoxPtr	m_ptrCombo;
	virtual void _UpdateUI();
};
//class CShellOutlookButton : public 

#endif // __shellbutton_h__