#if !defined(AFX_PALETTECTL_H__57C4CB56_9E87_11D3_A537_0000B493A187__INCLUDED_)
#define AFX_PALETTECTL_H__57C4CB56_9E87_11D3_A537_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


struct CellInfo
{
	CRect		rcCell;
	COLORREF	color;
	bool		bDefault;
};

// PaletteCtl.h : Declaration of the CPaletteCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CPaletteCtrl : See PaletteCtl.cpp for implementation.

class CPaletteCtrl : public COleControl
{
	DECLARE_DYNCREATE(CPaletteCtrl)

// Constructor
public:
	CPaletteCtrl();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPaletteCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual void Serialize(CArchive& ar);
	virtual DWORD GetControlFlags();
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CPaletteCtrl();

	DECLARE_OLECREATE_EX(CPaletteCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CPaletteCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CPaletteCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CPaletteCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);
	LRESULT OnOcmDrawItem( WPARAM wParam, LPARAM lParam );

// Message maps
	//{{AFX_MSG(CPaletteCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CPaletteCtrl)
	short m_cols;
	afx_msg void OnColsChanged();
	short m_rows;
	afx_msg void OnRowsChanged();
	short m_cellWidth;
	afx_msg void OnCellWidthChanged();
	short m_cellHeight;
	afx_msg void OnCellHeightChanged();
	short m_intercellSpace;
	afx_msg void OnIntercellSpaceChanged();
	BOOL m_innerSunken;
	afx_msg void OnInnerSunkenChanged();
	BOOL m_outerRaised;
	afx_msg void OnOuterRaisedChanged();
	BOOL m_outerSunken;
	afx_msg void OnOuterSunkenChanged();
	short m_borderSize;
	afx_msg void OnBorderSizeChanged();
	BOOL m_innerRaised;
	afx_msg void OnInnerRaisedChanged();
	afx_msg void SetDefaultColors();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CPaletteCtrl)
	void FireLeftClick(OLE_COLOR Color)
		{FireEvent(eventidLeftClick,EVENT_PARAM(VTS_COLOR), Color);}
	void FireRightClick(OLE_COLOR Color)
		{FireEvent(eventidRightClick,EVENT_PARAM(VTS_COLOR), Color);}
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	//{{AFX_DISP_ID(CPaletteCtrl)
	dispidCols = 1L,
	dispidRows = 2L,
	dispidCellWidth = 3L,
	dispidCellHeight = 4L,
	dispidIntercellSpace = 5L,
	dispidInnerSunken = 6L,
	dispidOuterRaised = 7L,
	dispidOuterSunken = 8L,
	dispidBorderSize = 9L,
	dispidInnerRaised = 10L,
	dispidSetDefaultColors = 11L,
	eventidLeftClick = 1L,
	eventidRightClick = 2L,
	//}}AFX_DISP_ID
	};

	void	_SetCellsInfoArray();
	void	_KillCellsInfoArray();
	void	_DeterminatePushedCell(CPoint point);

	void	_LoadState();
	void	_SaveState();
	void	_ClearState();

	void	_SetDefaultColors();
protected:
	CTypedPtrArray<CPtrArray, CellInfo*> m_ptrCellsInfo;
	int		m_nPushedCell;
	bool	m_bRightClick;
	bool	m_bMouseUp;
	CSize	m_sizeControl;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PALETTECTL_H__57C4CB56_9E87_11D3_A537_0000B493A187__INCLUDED)
