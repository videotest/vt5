#if !defined(AFX_SHELLDOCKBAR_H__265274C5_1EFD_11D3_A5D3_0000B493FF1B__INCLUDED_)
#define AFX_SHELLDOCKBAR_H__265274C5_1EFD_11D3_A5D3_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ShellDockBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CShellDockBar window
/*#include "dockbar\\sizecbar.h"
#include "dockbar\\scbarg.h"*/

#define classDockBarBase		CBCGDialogBar
#define classDockBarBaseRoot	CBCGSizingControlBar
#define classBarArray			CSCBArray
//#define classDockBarBase	CSizingControlBarG
//#define classDockBarBase1	CSizingControlBar


class CShellDockBar : public classDockBarBase
{
	DECLARE_DYNAMIC(CShellDockBar);
	int m_nEnableDock;
// Construction
public:
	CShellDockBar( IUnknown *punkBar );

// Attributes
public:
	IUnknown	*GetClientUnknown()	const	{return m_sptrDockClient;}
	void	InitDockSizes();
	virtual void	UpdateFrameTitle();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellDockBar)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD dwMode);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual void OnTrackUpdateSize(CPoint& point);
	virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);
	virtual CString	GetName( bool bUserName = false );

	virtual ~CShellDockBar();

	DWORD	GetDefDockSide();//return side where bar should be docked by default
	CSize	GetDefSize();// return size of desired client area
	CSize	CalcFrameSizeForFixedBar();
	CSize	ExpandSizes( CSize sizeOrg );

	CSize	GetMinSize()	{return m_szMin;}
	void	CommitSize( CSize size );


	//If this bar use for ChildFrame;
	bool	IsFixed();
	BOOL IsChildFrameBar(){return m_bChildFrameBar;	}
	

	// Generated message map functions
protected:

	BOOL	m_bChildFrameBar;

	//{{AFX_MSG(CShellDockBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcPaint();
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnCaptureChanged(CWnd *pWnd);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnNcLButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnNcRButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnWindowPosChanging(WINDOWPOS FAR* lpwndpos);
	afx_msg void OnPaint();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnNcMouseMove(UINT nHitTest, CPoint point);
	afx_msg void OnNcMButtonDblClk(UINT nHitTest, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
	afx_msg LRESULT OnSetCanCloseWnd(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CShellDockBar)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	sptrIDockableWindow	m_sptrDockClient;
	sptrIWindow	m_sptrWindow;
	INamedObject2Ptr m_sptrName;

	HWND	m_hwndChild;


public:
	bool	m_bStoreVisibleState;

	//can close
protected:
	bool	m_bcan_close;
public:
	void	SetCanClose( bool bcan_close );
	bool	GetCanClose( );
	virtual bool IsAllowMoving();

	// [vanek] : close button's state - 22.10.2004
	virtual bool IsCloseButtonEnabled( );
protected:
	bool m_bMiniFrameCaption;
public:
	void	SetMiniFrameCaption( bool bMiniFrameCaption );
	bool	GetMiniFrameCaption( );

protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void DelayShow(BOOL bShow);
};

class CShellToolDockBar : public CBCGToolDockBar
{
	DECLARE_DYNCREATE(CShellToolDockBar);
public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	void ResetArray();
	void AddDockBar( CControlBar *pbar );

	POSITION	GetFirstDockBarPosition();
	CShellDockBar	*GetNextDockBar( POSITION &rPos );
	void ValidateBarArray();
	
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// [vanek] : support flat style - 07.12.2004
	virtual void CalcInsideRect(CRect& rect, BOOL bHorz) const 
	{
		static long lbtn_style = -1;		
		if( lbtn_style == -1 )
			lbtn_style = ::GetValueInt( GetAppUnknown(), "\\General", "ButtonStyle", 0 );

		if( !lbtn_style )
			return;	// flat

		__super::CalcInsideRect( rect, bHorz );
    }
	

protected:
	// [vanek] : array of last visible dockbars
	CArray<int> m_arrLastVisibleDockBars;

public:
	void	ResetLastVisibleDockBars(void)
	{ m_arrLastVisibleDockBars.RemoveAll(); }

};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLDOCKBAR_H__265274C5_1EFD_11D3_A5D3_0000B493FF1B__INCLUDED_)
