#if !defined(AFX_SPLITTERTABBED_H__0EC201E5_5A2D_11D3_9999_0000B493A187__INCLUDED_)
#define AFX_SPLITTERTABBED_H__0EC201E5_5A2D_11D3_9999_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#define ResizerWidth 5

// SplitterTabbed.h : header file
//

class CTabBeam;

/////////////////////////////////////////////////////////////////////////////
// CSplitterTabbed command target

class CSplitterTabbed : public CSplitterBase,
						public CEventListenerImpl
{
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CSplitterTabbed)
	GUARD_DECLARE_OLECREATE(CSplitterTabbed)

	CSplitterTabbed();           // protected constructor used by dynamic creation

// Attributes
public:
	enum DragMode
	{
		dmHorz = 1,
		dmNone = 0
	};
	bool m_bFirstView;
	short m_numViews;
	short m_maxNumView;
	long m_lLeftHorzScroll;
	long m_lTopHorzScroll;
	short m_activeView;
	bool m_bScrollLeft;
	bool m_bScrollRight;

#ifdef _DEBUG
	CMemoryState m_startMemState;
#endif

	double	m_fposX, m_fposY;	//splitter position (from 0 to 1)
	DragMode m_dragMode;

	CSize	m_sizeMin;

	CScrollBar* m_pscrollHorz;
	CScrollBar* m_pscrollVert;
	//CButton* m_pbuttLeft;
	//CButton* m_pbuttRight;
	CScrollBar* m_pscrollTabs;
	CButton* m_pbuttMenu;
	CTabBeam* m_pTabber;
		
// Operations
public:
	void RenameView(short idx, CString strNewName);
	bool LoadViewsState(IUnknown* punkF = 0);
	void GetViewName(CString& strName);
	void RenameView(CString strNewName);
	void SaveViewsState(IUnknown* punkF = 0);
	void RemoveView(short Number, bool setActive = true);
	void RemoveView();
	void SetActiveView(short newActive);
	void AddView(CString strLabel="", bool setActive = true, BSTR bstrViewType = 0);
	void SetButtons(short left, short right);
	
	virtual bool DoRecalcLayout();
	virtual void OnActivateView( XSplitterEntry *pe, XSplitterEntry *peOld );
	virtual void OnCreateView( XSplitterEntry *pe );
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	virtual void OptimizeSplitter();
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterTabbed)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	DragMode _HitTest( CPoint pt );
	void _LayoutWindow( XSplitterEntry *pe, CRect rc, HDWP &hdwp );
	virtual ~CSplitterTabbed();

	// vanek - 22.09.2003
	virtual	bool IsEnabledMenuButton( );

	// Generated message map functions
	//{{AFX_MSG(CSplitterTabbed)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnButton( UINT nID );

	DECLARE_MESSAGE_MAP()
	

	BEGIN_INTERFACE_PART(Splitter, ISplitterTabbed)
		com_call AddView(BSTR bstrName);
		com_call RemoveActiveView();
		com_call RenameView(BSTR bstrNewName);
		com_call SaveState(IUnknown* punkND);
		com_call LoadState(IUnknown* punkND);
		com_call GetActiveViewName(BSTR* pbstrName);
	END_INTERFACE_PART(Splitter)

	DECLARE_INTERFACE_MAP()
private:

	bool	m_bRegisterNotify;
	IDocumentPtr	m_sptrDoc;
	
	HCURSOR	m_hCursorHORZ;
	HBITMAP m_hBitmapMenu;
	long	m_lShowTabControl;
	
	// vanek - 23.09.2003
	int		m_nEnabledMenuButton;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERTABBED_H__0EC201E5_5A2D_11D3_9999_0000B493A187__INCLUDED_)
