#pragma once
#include "_list_idx_t.h"
#include "DrawParam.h"
#include "works_modes.h"

// UserParamsListBoxCtrl.h : Declaration of the CUserParamsListBoxCtrl ActiveX Control class.

inline void	free_DrawParam( void* pdata )
{
	delete( (CDrawParam*)pdata );
}

// CUserParamsListBoxCtrl : See UserParamsListBoxCtrl.cpp for implementation.

class CUserParamsListBoxCtrl : public COleControl
{
	DECLARE_DYNCREATE(CUserParamsListBoxCtrl)

	
// Constructor
public:
	CUserParamsListBoxCtrl();

// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CUserParamsListBoxCtrl();

	DECLARE_OLECREATE_EX(CUserParamsListBoxCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CUserParamsListBoxCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CUserParamsListBoxCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CUserParamsListBoxCtrl)		// Type name and misc status

	// Subclassed control support
	BOOL IsSubclassedControl();
	LRESULT OnOcmCommand(WPARAM wParam, LPARAM lParam);

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
		dispidWorksMode = 10,		dispidFirstColumnsWidth = 9,				eventidOnActiveItemChange = 1L,		dispidGetItemData = 8L,		dispidSetItemData = 7L,		dispidRefresh = 6L,		dispidActiveItem = 5,		dispidRemoveItem = 4L,		dispidAddParam = 3L,		dispidGetParamSection = 2L,		dispidGetItemCount = 1L
	};
private:
	_list_idx_t<CDrawParam*,free_DrawParam>		m_ParamList;			// список параметров
	long										m_lFirstColumnsWidth;	// ширина первого столбца
	CMap<long, long&, CString, CString&>		m_mapKeyToSection;	
	long										m_lActiveItem;
	long										m_lWorksMode;			// режим работы ("works_modes.h")

	HBRUSH	 m_brBkgnd;

	// gdi+
	CGDIPlusManager m_gdiplus_man;
	
    
	long	LoadParams( );
	long	GetMaxSecondColumnWidth( );
	BOOL	SetHorzExtent( );
	BOOL	RecalcItemsRects( );
	BOOL	SetItemsHeights( );
	void	InitDefault( );
	//void	SetWorksModes( );
	void	Clear( );
	void	ReLoadList( );
	BOOL	SearchItemListBySection( CString strSection, long *plIdx);
	

protected:
	LONG GetItemCount(void);
	BSTR GetParamSection(LONG nIdx);
	LONG AddParam(LPCTSTR Section);
	VARIANT_BOOL RemoveItem(LONG nIdx);
	LONG GetActiveItem(void);
	void SetActiveItem(LONG newVal);
	void Refresh(void);
	VARIANT_BOOL SetItemData(LONG nIdx, LONG lData);
	LONG GetItemData(LONG nIdx);

	void OnActiveItemChange(LONG nIdx)
	{
		FireEvent(eventidOnActiveItemChange, EVENT_PARAM(VTS_I4), nIdx);
	}
public:
	afx_msg void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg HBRUSH CtlColor ( CDC* pDC, UINT nCtlColor );
	virtual void OnFontChanged();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual void Serialize(CArchive& ar);
protected:
	LONG GetFirstColumnsWidth(void);
	void SetFirstColumnsWidth(LONG newVal);
	LONG GetWorksMode(void);
	void SetWorksMode(LONG newVal);

	// [vanek] - 19.03.2005
	void UpdateScrollBars();
};

