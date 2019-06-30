#pragma once 
#include "figurebutton.h"
#include "funcmethodlistctrl.h"

///////////////////////////////////////////////////////////////////////
// Class CMethodTitleWnd
class CMethodTitleWnd : 
	public CWnd
{
	HICON	m_hicon;

public :
	CString m_str_method_name;
	bool	m_bsemiflat;

public:
    CMethodTitleWnd (void);
	virtual ~CMethodTitleWnd (void);

	DECLARE_MESSAGE_MAP()
    
protected:
    afx_msg void OnPaint();
};


///////////////////////////////////////////////////////////////////////
// Class CMethodTitleWnd
class CMethodBar:
	public CWnd
{
public:
	// buttons
	CFigureButton			m_btn_new_method;
	//CFigureButton			m_btn_copy_method;
	CFigureButton			m_btn_rec_method;
	CFigureButton			m_btn_play_method;
	CFigureButton			m_btn_save_method;		
	CFigureButton			m_btn_delete_func;

	CFigureButton			m_btn_undo_method;		
	CFigureButton			m_btn_redo_method;
    
	CFigureButton			m_btn_menu;

	bool	m_bsemiflat;

public:
	CMethodBar(void);
	virtual ~CMethodBar(void);

	DECLARE_MESSAGE_MAP()

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

protected:
	 BOOL	create_buttons(void);
	 BOOL	calc_layout(void);
};

///////////////////////////////////////////////////////////////////////
// class CMethodPanel
class CMethodPanel : 
	public CWnd,
	public CWindowImpl,
	public CDockWindowImpl,
	public CRootedObjectImpl,
	public CEventListenerImpl,
	public CNamedObjectImpl,
	public CMsgListenerImpl,
	public CHelpInfoImpl
{
	DECLARE_DYNCREATE( CMethodPanel )
	GUARD_DECLARE_OLECREATE( CMethodPanel )
	ENABLE_MULTYINTERFACE();

protected:
	// title
	CMethodTitleWnd		m_title;
	CMethodBar			m_buttons_bar;
	CFuncMethodListCtrl	m_list_func_ctrl;

public:
	void	create_controls(void);
	void	calc_layout( );
		
public:
    CMethodPanel(void);
	virtual ~CMethodPanel(void);

	DECLARE_MESSAGE_MAP()

	DECLARE_INTERFACE_MAP()
    
	virtual CWnd *GetWindow();
	virtual bool DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID);
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnFinalRelease(void);
	virtual void PostNcDestroy();
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	// override from msg listner
	virtual BOOL OnListenMessage( MSG * pmsg, LRESULT *plResult );

			
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}

protected:
	BOOL	on_create_new_mtd(void);
	BOOL	on_copy_mtd(void);
	BOOL	on_save_mtd(void);
	BOOL	on_rec_mtd(void);
	BOOL	on_play_mtd(void);
	BOOL	on_del_active_func(void);
	BOOL	on_menu_btn(void);
	BOOL	on_undo_btn(void);
	BOOL	on_redo_btn(void);

	//BOOL	is_loop_empty( );
	//void	check_loop( );

	void	update_btns_states( bool bupdate_actions_btns = false );

	virtual BOOL OnCreateAggregates(void);
};