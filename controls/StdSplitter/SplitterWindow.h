#if !defined(AFX_SPLITTERWINDOW_H__AC8FC6D4_27A1_11D3_A5DC_0000B493FF1B__INCLUDED_)
#define AFX_SPLITTERWINDOW_H__AC8FC6D4_27A1_11D3_A5DC_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

const int	cxLeft = 2,
			cxRight = 2,
			cyTop = 2,
			cyBottom = 2,
			cxSplitter = 2,
			cySplitter = 2;


class CSplitterStandard : public CSplitterBase
{
	DECLARE_DYNCREATE(CSplitterStandard)
	GUARD_DECLARE_OLECREATE(CSplitterStandard)
	
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
public:
	enum DragMode
	{
		dmHorz = 1,
		dmVert = 2,
		dmBoth = 3,
		dmNone = 0
	};
protected:
	CSplitterStandard();           // protected constructor used by dynamic creation
	virtual ~CSplitterStandard();

	// vanek - 22.09.2003
	virtual	bool IsEnabledMenuButton( );
public:
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
	virtual bool DoRecalcLayout();
	virtual bool DoSplit( int nRowCount, int nColCount );

	virtual bool MergeCol( int nColToDelete );
	virtual bool MergeRow( int nRowToDelete );
	virtual bool SplitCol();
	virtual bool SplitRow();
	virtual bool DoCancelMode();

	virtual void OptimizeSplitter();

	virtual CRect CalcWindowRect( XSplitterEntry *pe );
	virtual void LayoutWindow( XSplitterEntry *pe, CRect rc, HDWP &hdwp );
	virtual DragMode HitTest( CPoint pt );
	virtual void AfterRecalcLayout();
	virtual void AfterSplitCol(){};
	virtual void AfterSplitRow(){};
	
	// Generated message map functions
	//{{AFX_MSG(CSplitterStandard)
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnCancelMode();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnClose();
	//}}AFX_MSG
	afx_msg void OnButton( UINT nID );
public:
	int GetRowsCount(){return (m_fposY > 0)?2:1;}
	int GetColsCount(){return (m_fposX > 0)?2:1;}

	virtual void OnCreateView( XSplitterEntry *pe );
	virtual void OnActivateView( XSplitterEntry *pe, XSplitterEntry *peOld );

	bool m_bButCreate;
protected:
	
	CRect _CalcWindowRect2( XSplitterEntry *pe );
	
	void _ValidateControls();
	void _ValidatePane( XSplitterEntry *pe );

	
	DECLARE_DISPATCH_MAP();
	//class wizard support
	//{{AFX_DISPATCH(CSplitterStandard)
		afx_msg BSTR GetViewTypeDisp(long nCol, long nRow);
		afx_msg void SetViewTypeDisp(long nCol, long nRow, LPCTSTR szType);
		afx_msg long GetColsCountDisp();
		afx_msg long GetRowsCountDisp();
		afx_msg LPDISPATCH GetActiveView();
		afx_msg void SetActiveView(LPDISPATCH pdispView);
		afx_msg void EnableButton(long lRow, long lCol, long lButton, long lEnable);
		afx_msg void BlockButton(bool bBlockBut);
		afx_msg BOOL GetMergeSmall();
		afx_msg void SetMergeSmall(BOOL bMergeSmall);
	//}}AFX_DISPATCH


	void ErrorMsg(UINT nErrorID);
	void GetPaneSize(XSplitterEntry	*pe, CSize& size);

protected:
	double	m_fposX, m_fposY;	//splitter position (from 0 to 1)
	DragMode	m_dragMode;

	CSize	m_sizeMin;

	HCURSOR m_hCursorVERT;
	HCURSOR m_hCursorHORZ;
	HCURSOR m_hCursorBOTH;

	CMap<UINT, UINT, HBITMAP, HBITMAP&> m_bitmaps;
	bool	m_bMergeSmall;

	// vanek - 23.09.2003
	int		m_nEnabledMenuButton;

};

HBITMAP _MakeBitmapButton( CWnd *pwnd, UINT nID, CMap<UINT, UINT, HBITMAP, HBITMAP&>* pBitmaps = NULL );

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLITTERWINDOW_H__AC8FC6D4_27A1_11D3_A5DC_0000B493FF1B__INCLUDED_)
