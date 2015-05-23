#ifndef __splitbase_h__
#define __splitbase_h__

#include "defs.h"
#include "wndbase.h"
#include "cmnbase.h"
#include "nameconsts.h"

class std_dll CSplitterBase : public CWnd,
					public CWindowImpl,
					public CRootedObjectImpl,
					public CHelpInfoImpl,
					public CNamedObjectImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DISPATCH_MAP();
	DECLARE_INTERFACE_MAP();
	DECLARE_MESSAGE_MAP();

public:
	//splitter entry about view
	struct XSplitterEntry
	{
		int			nRow, nCol;
		IUnknown	*punkView;
		_bstr_t		bstrViewRuntime;
		CScrollBar	*m_pscrollVert;
		CScrollBar	*m_pscrollHorz;
		bool		m_bOwnScrollBars;
		CRect		m_rc;

		CWnd		*m_pwnd1;	//reserved windows pointer1
		CWnd		*m_pwnd2;	//reserved windows pointer2
		CWnd		*m_pwnd3;	//reserved windows pointer3
		CWnd		*m_pwnd4;	//reserved windows pointer4
		CWnd		*m_pwnd5;	//reserved windows pointer5

		WORD        m_wEnableMask;

		XSplitterEntry( int row, int col, BSTR bstr );
		virtual ~XSplitterEntry();
	};
public:
	CSplitterBase();
	virtual ~CSplitterBase();
protected:
//CWindowImpl helper
	bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );
	virtual CWnd *GetWindow();
//splitter functions
	bool CreateView( int nRow, int nCol, BSTR bstrViewType=0 );
	virtual bool DeleteView( int nRow, int nCol );
	virtual bool ChangeViewType( int nRow, int nCol, BSTR bstrViewType );
	virtual void ExecuteMenu( CPoint ptMenuPos, DWORD dwFlags );
//virtuals
	virtual bool DoSplit( int nRowCount, int nColCount ){return false;};
	virtual bool DoRecalcLayout(){return false;};
	// vanek - 23.09.2003
	virtual	bool IsEnabledMenuButton( ){return true;};
//"events" 
	virtual void OnCreateView( XSplitterEntry *pe ){};
	virtual void OnDeleteView( XSplitterEntry *pe ){};
	virtual void OnActivateView( XSplitterEntry *pe, XSplitterEntry *peOld ){};


	virtual void OptimizeSplitter(){};
protected:	//view list function's
	POSITION		GetFirstViewEntryPosition();			//View list navigation 
	XSplitterEntry	*GetNextViewEntry( POSITION &pos );		//return view entry by position
	XSplitterEntry	*GetViewEntry( int nRow, int nCol, POSITION *ppos = 0 );	//return view entry by row and col
	XSplitterEntry	*GetViewEntry( IUnknown *punkWnd, POSITION *ppos = 0 );		//return view entry by window's unknown
	XSplitterEntry	*GetViewEntry( int nChildID, POSITION *ppos = 0 );		//return entry by scrollbar
	BSTR GetViewType( BSTR bstrDefProgID = 0 );

	BEGIN_INTERFACE_PART_EXPORT(Splitter, ISplitterWindow)
		com_call GetActiveView( IUnknown **punkView );
		com_call ChangeViewType( int nRow, int nCol, BSTR bstrViewType );
		com_call GetRowColCount( int *pnRows, int *pnCols );
		com_call GetViewRowCol( int	nRow, int nCol, IUnknown **ppunkView );
		com_call GetViewTypeRowCol( int	nRow, int nCol, BSTR *pbstrViewType );
		com_call Split( int nRowCount, int nColCount );
		com_call GetActivePane( int *piRow, int *piCol );
		com_call GetScrollBarCtrl( IUnknown *punkView, DWORD sbCode, HANDLE *phCtrl );
		com_call OptimizeSplitter();
	END_INTERFACE_PART(Splitter)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CSplitterBase)
	afx_msg void Split(short nHorz, short nVert);
	afx_msg BSTR GetActiveViewType(VARIANT FAR* pvarRow, VARIANT FAR* pvarCol);	
	afx_msg void ActivateView(long lCol, long lRow, LPCTSTR szType);
	afx_msg long GetColumnsCount();
	afx_msg long GetRowCount();
	afx_msg long GetWidth();
	afx_msg long GetHeight();
	afx_msg long GetRowHeight(long lRow);
	afx_msg long GetColWidth(long lCol);
	afx_msg void SetRowHeight(long lRow, long lHeight);
	afx_msg void SetColWidth(long lCol, long lWidth);
	afx_msg double GetViewZoom(long lCol, long lRow);
	afx_msg void SetViewZoom(long lCol, long lRow, double fZoom);
	afx_msg long GetViewClientWidth(long lCol, long lRow);
	afx_msg long GetViewClientHeight(long lCol, long lRow);
	afx_msg long GetViewXScroll(long lCol, long lRow);
	afx_msg long GetViewYScroll(long lCol, long lRow);
	afx_msg void SetViewScroll(long lCol, long lRow, long lXScroll, long lYScroll );
	//}}AFX_DISPATCH

	virtual long get_width();
	virtual long get_height();
	virtual long get_row_height(long lRow);
	virtual long get_col_width(long lCol);
	virtual void set_row_height(long lRow, long lHeight);
	virtual void set_col_width(long lCol, long lWidth);

	// Generated message map functions
	//{{AFX_MSG(CSplitterBase)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	afx_msg void OnActivatePane();
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplitterBase)
	protected:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL
//	virtual void PostNcDestroy();
//	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	CPtrList			m_listViews;
	CCompRegistrator	m_regViews;
	_bstr_t				m_bstrLastViewProgID;
	XSplitterEntry		*m_pveActiveView;


	CString		m_strDocTemplateName;	// of the cirrent document template name 
};


#endif //__splitbase_h__
