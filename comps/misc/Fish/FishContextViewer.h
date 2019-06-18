#pragma once

#include "dlgs.h"

#include <vector>

IUnknown* GetActualImageFromView(IUnknown *punkView);

class CFishContextViewer :  public CWnd,
						 	public CWindowImpl,
							public CDockWindowImpl,
							public CRootedObjectImpl,
							public CEventListenerImpl,
							public CNamedObjectImpl,
							public CMsgListenerImpl,
							public CHelpInfoImpl
{
	DECLARE_DYNCREATE( CFishContextViewer )
	GUARD_DECLARE_OLECREATE( CFishContextViewer )
	ENABLE_MULTYINTERFACE();

	CResultPartDlg m_ResultPart,m_Combi;
	std::vector<CLayerPartDlg *> m_arrLayers;
	typedef std::vector<CLayerPartDlg *>::iterator IT;
	GuidKey m_lLastViewKey;
	IUnknownPtr m_ptrLastImage;
	
	int m_nItemCount,m_nActiveItem;
	DWORD m_dwUsedPanes;
	double m_fPageCount;
	UINT m_nHeight;

	int m_nGapBetweenControls,m_nLeftOffset,m_nTopOffset;
//	int m_nWidth;

	CScrollBar *m_phrzScoll;
	CScrollBar *m_pvrtScoll;
	CStatic    m_StaticRightBottom;
	CBrush m_brBack;

	IUnknownPtr GetNData();
	void OnSettingsChanged();
public:
	void UnpressBtns( CWnd *pPressed );
	DWORD &UsedPanes() { return m_dwUsedPanes; }
	IViewPtr				m_sptrView;
	CFishContextViewer(void);
	~CFishContextViewer(void);
	void UpdateCC();
	void UpdateCC_ID( UINT nID, COLORREF clColor );
	void _RefreshWindow();
	void StoreToNamedData( IUnknownPtr sptrImage, const _bstr_t &bstrSect, const _bstr_t &bstrKey, const _variant_t &var );
	_variant_t GetFromNamedData( IUnknownPtr sptrImage, const _bstr_t &bstrSect, const _bstr_t &bstrKey );
	void _SetActiveItem(int nActiveItem, bool bSetToNPG, bool Fire);

	//{{AFX_DISPATCH(CFishContextViewer)
	void RefreshWindow();
	int GetFiltersCount();
	BSTR GetFilterName(int iLayer);
	BSTR GetFilterShortName(int iLayer);
	int GetFilterState(int iLayer); 
	int GetFilterColor(int iLayer); 
	void SetFilterName(int iLayer, LPCTSTR bstrName);
	void SetFilterShortName(int iLayer, LPCTSTR bstrName);
	void SetFilterState(int iLayer, int nState);
	void SetFilterColor(int iLayer, int nState);
	int GetActiveItem() {return m_nActiveItem;}
	void SetActiveItem(int nActiveItem);
	void EnableItem(int iLayer, int iEnabled);
	//}}AFX_DISPATCH
	
	// Events support
	void FireScriptEvent(LPCTSTR lpszEventName, int nParams, VARIANT *pvarParams);
	void FireOnChangeActive(int nNewActive);
	void FireOnChangeName(int nItem, LPCTSTR lpstrNewName);
	void FireOnChangeColor(int nItem, int nNewColor);
	void FireOnChangeCheck(int nItem, BOOL bNewCheck);

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	void OnChangeViewOrObject();

	virtual CWnd *GetWindow();
	virtual bool DoCreate(DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID);
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	void Register( IUnknown *punkCtrl = 0, const char *pszEvString = 0 );
	void UnRegister( IUnknown *punkCtrl = 0, const char *pszEvString = 0 );

protected:
	int re_create_context(IUnknown *punkNData, bool bAdd = false);
	void _reset_scroller( int nHeight );
	void _reposition_scrollers();
	void _reposition(int x, int y);
public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	void _reload_to_ndata(IUnknown * punkNDataFrom, IUnknown * punkNDataTo);
	void _apply_own_ndata(IUnknown * punkNData);
	void _create_empty_bar();
	virtual CScrollBar* GetScrollBarCtrl(int nType) const
	{
		if( nType == SB_HORZ )
			return m_phrzScoll;
		else if( nType == SB_VERT )
			return m_pvrtScoll;
		return __super::GetScrollBarCtrl( nType ); 
	}
	int GetScrollPos(int nBar);
public:
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
	afx_msg void OnDestroy();
protected:
	virtual void PostNcDestroy();
public:
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
//	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
};
