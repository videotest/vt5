#if !defined(AFX_OBJECTVIEWS_H__ED30F8FF_C5FA_41A8_B142_C00F0DAD76B4__INCLUDED_)
#define AFX_OBJECTVIEWS_H__ED30F8FF_C5FA_41A8_B142_C00F0DAD76B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectViews.h : header file
//

#include "measlistwrp.h"
#include "ImageSourceView.h"
#include "\vt5\awin\misc_list.h"

#undef SupportLayers
/////////////////////////////////////////////////////////////////////////////
// CObjectViewBase command target

struct ObjectList
{
	ObjectList(IUnknown* punk)
	{
		m_nActiveNumber = -1;
		m_objects = punk;
		pNext = 0;
	};
	~ObjectList()
	{
		if(pNext)
			delete pNext;
	};
	long				m_nActiveNumber;
	CMeasObjListWrp		m_objects;
	ObjectList*			pNext;
};

class CObjectViewBase : public CImageSourceView
{
public:
	CObjectViewBase();
	~CObjectViewBase();

	virtual void DrawLayers(CDC* pdc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint, bool bUseScrollZoom);
	virtual void DrawHilightedObjects(CDC &dc, BITMAPINFOHEADER* pbmih, byte* pdibBits, CRect rectPaint);
	virtual void HilightObjectImage(CDC &dc, BITMAPINFOHEADER *pbmih, byte *pdibBits, byte *pMaskCounters, CRect rectPaint, IImage3Ptr ptrImage, COLORREF clr_highlight);
	virtual void DoPocessNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual IUnknown* __GetObjectByPoint( CPoint point);
	virtual void	_AttachObjects( IUnknown *punkContext );
	virtual void OnBeginDragDrop();
	virtual void OnEndDragDrop();
	virtual void LoadSettings();
	virtual DWORD GetMatchType( const char *szType );
	void	_select_measurement( IUnknown *punkObjectToActivate = 0 );
	void	_redraw_selection();
protected:
	POSITION _GetObjectByPoint(CMeasObjListWrp* pObjects, CPoint point);
	bool _DrawObjectList(ObjectList* pList, CDC &dc, CRect rectClip, BITMAPINFOHEADER *pbi, byte *pdib, bool bUseScrollZoom);
		
	bool CheckChildByPoint(CMeasObjListWrp* pObjects, IUnknown * punkObj, CPoint point);

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	DECLARE_MESSAGE_MAP()
	
	//CMeasObjListWrp		m_objects;
	ObjectList* m_pObjectListHead;
	ObjectList* m_pObjectListTail;

	bool	m_bClassHilightEnabled, 
			m_bShowContours,
			m_bShowChildObjects;
	int		m_nHighlightMethod; // 0 - нет подсветки, 1 - старый метод, 2 - пашин (extended), 3 - подсветка цветом класса (в остальном то же, что пашин метод)
	int		m_nHighlightOversectMethod;
	int		m_nHighlightPhases;
	COLORREF m_clrHighlightOversectColor;

	COLORREF
		m_clr_highlight_mask,
		m_clr_highlight_inter_mask;
	int		m_highlight_percent, m_highlight_inter_percent;

	//CPtrArray	m_hilighted_objects;
	_list_t<IUnknownPtr> m_hilighted_objects;
	bool _get_center_mode() { return false; };

	virtual bool	is_have_print_data();
	virtual void _recalc_rect(BOOL bUpdate = false, IUnknown *punkExclude = 0);
public:
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );
};


class CObjectsView : public CObjectViewBase
{
	DECLARE_DYNCREATE(CObjectsView);
	GUARD_DECLARE_OLECREATE(CObjectsView);
	PROVIDE_GUID_INFO( )
public:
	CObjectsView();

public:
	virtual const char* GetViewMenuName();
	
};

class CMasksView : public CObjectViewBase
{
	DECLARE_DYNCREATE(CMasksView);
	GUARD_DECLARE_OLECREATE(CMasksView);
	PROVIDE_GUID_INFO( )
public:
	CMasksView();

public:
	virtual const char* GetViewMenuName();

};

class CImageView : public CObjectViewBase
{
	DECLARE_DYNCREATE(CImageView);
	GUARD_DECLARE_OLECREATE(CImageView);
	PROVIDE_GUID_INFO( )
public:
	CImageView();

public:
	virtual const char* GetViewMenuName();
};

class CPseudoView : public CObjectViewBase
{
	DECLARE_DYNCREATE(CPseudoView);
	GUARD_DECLARE_OLECREATE(CPseudoView);
	PROVIDE_GUID_INFO( )
public:
	CPseudoView();

public:
	virtual const char* GetViewMenuName();
	virtual DWORD GetMatchType( const char *szType );
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTVIEWS_H__ED30F8FF_C5FA_41A8_B142_C00F0DAD76B4__INCLUDED_)
