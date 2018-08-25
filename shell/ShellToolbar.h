#ifndef __shelltoolbar_h__
#define __shelltoolbar_h__

//#include <BCGMenuBar.h>

class CShellToolbarButton;

class CShellToolBar : public CMFCToolBar
{
	DECLARE_SERIAL(CShellToolBar)
public:
	CShellToolBar();
	~CShellToolBar();
protected:
	virtual CMFCToolBarButton* CreateDroppedButton (COleDataObject* pDataObject);
	virtual void DoPaint(CDC* pDC);
	virtual BOOL EnableContextMenuItems (CMFCToolBarButton* pButton, CMenu* pPopup);
	virtual CShellToolbarButton *GetSelectedButton();
	virtual void SetSelectedButton( CShellToolbarButton *pbtn );
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);

public:
	virtual void AdjustLocations ();
	int InsertButtonStoreImage(const CMFCToolBarButton& button, int iInsertAt = -1);
	// [vanek] get tollbar's buttons by command's index - 20.10.2004
	int GetMyCommandButtons (UINT uiCmd, CObList *plistButtons);
protected:
	DECLARE_MESSAGE_MAP();
	//{{AFX_MSG(CShellToolBar)
	afx_msg void OnToolbarMenubutton();
	afx_msg void OnUpdateToolbarMenubutton(CCmdUI* pCmdUI);
	afx_msg void OnToolbarCommandlist();
	afx_msg void OnUpdateToolbarCommandlist(CCmdUI* pCmdUI);
	afx_msg void OnShowAsText();
	afx_msg void OnShowAsImage();
	afx_msg void OnShowAsImageAndText();
	afx_msg void OnActionInfo();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MsheSG
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnHelpHitTest( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnGetInterface( WPARAM wParam, LPARAM lParam );

	virtual bool CanDragDropButtons();
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
};

class CShellOutlookToolBar : public CShellToolBar
{
	DECLARE_DYNCREATE(CShellOutlookToolBar);
public:
	CShellOutlookToolBar();

	DECLARE_MESSAGE_MAP();
	//{{AFX_MSG(CShellOutlookToolBar)
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
public:
	virtual void AdjustLocations ();
	virtual BOOL IsMenuMode() const;
	virtual BOOL EnableContextMenuItems(CMFCToolBarButton* pButton, CMenu* pPopup);
	virtual void  OnFillBackground (CDC* /*pDC*/);


	virtual void DoPaint(CDC* pDC);

protected:
	int		m_nVisibleCount;
	int		m_nVisibleOffset;
	CRect	m_rectDown;
	CRect	m_rectUp;
	int		m_nTime;
};

class CShellFrameToolBar : public CShellToolBar
{
public:
	CShellFrameToolBar();
	~CShellFrameToolBar();
	
	DECLARE_SERIAL(CShellFrameToolBar)
public:
	virtual void AdjustLocations ();

	//{{AFX_MSG(CShellFrameToolBar)	
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	bool	m_bOldVisible;

/*protected:
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragEnter(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
	virtual void OnDragLeave();
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);*/

};

class CBCGMultiDocTemplate;

class CShellMenuBar : public CMFCMenuBar
{
protected:
	virtual HMENU LoadMainMenu();
	virtual HMENU LoadDocumentMenu( CBCGMultiDocTemplate * );
public:
	HMENU GetDefaultMenu(){return m_hDefaultMenu;}
	void SetMenuName();
	void ReloadMenu();

	DECLARE_MESSAGE_MAP();
	afx_msg LRESULT OnHelpHitTest( WPARAM wParam, LPARAM lParam );
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	virtual void  OnFillBackground (CDC* /*pDC*/);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);

	virtual bool CanDragDropButtons();

protected:
	virtual BOOL OnDrop(COleDataObject* pDataObject, DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(COleDataObject* pDataObject, DWORD dwKeyState, CPoint point);
};

BOOL ProcessEraseBackground( HDC hdc, HWND hwnd );
extern long g_lAllowCustomize;


#endif //__shelltoolbar_h__
