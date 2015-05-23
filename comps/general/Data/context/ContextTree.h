#if !defined(AFX_CONTEXTTREE_H__BBBFB489_9E5D_4445_AE61_61EF34AF9A84__INCLUDED_)
#define AFX_CONTEXTTREE_H__BBBFB489_9E5D_4445_AE61_61EF34AF9A84__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContextTree.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CContextDataTree window

#include "ContextItem.h"
#include "TitleTip.h"

// predefines
class CContextCtrl;
class CTitleTip;

// Use this as the classname when inserting this control as a custom control
// in the MSVC++ dialog editor
#define IDC_INPLACE_CONTROL   8                  // ID of inplace edit controls
#define ICON_SIZE		32
#define ICON_SIZE_SMALL	16


#define ODT_CONTEXTVIEW	ODT_LISTVIEW

// Handy functions
#define IsSHIFTpressed() ( (GetKeyState(VK_SHIFT) & (1 << (sizeof(SHORT)*8-1))) != 0   )
#define IsCTRLpressed()  ( (GetKeyState(VK_CONTROL) & (1 << (sizeof(SHORT)*8-1))) != 0 )


// Item data mask
#define CVIF_TEXT               LVIF_TEXT
#define CVIF_IMAGE              LVIF_IMAGE
#define CVIF_PARAM              LVIF_PARAM
#define CVIF_STATE              LVIF_STATE
#define CVIF_BKCLR              (CVIF_STATE<<1)
#define CVIF_FGCLR              (CVIF_STATE<<2)
#define CVIF_FORMAT             (CVIF_STATE<<3)
#define CVIF_HEIGHT             (CVIF_STATE<<4)

// Item states
#define CVIS_FOCUSED			0x0001
#define CVIS_SELECTED			0x0002
#define CVIS_NUMBERED			0x0004
#define CVIS_READONLY			0x0008
#define CVIS_TRACKED			0x0010
#define CVIS_DROPHILITED		0x0020
#define CVIS_EXPANDED			0x0040
#define CVIS_DISABLED			0x0080

// Item Searching options
#define CVNI_FOCUSED            0x0001
#define CVNI_SELECTED           0x0002
#define CVNI_NUMBERED			0x0004
#define CVNI_READONLY           0x0008
#define CVNI_TRACKED            0x000f
#define CVNI_DROPHILITED        0x0010
#define CVNI_EXPANDED           0x0020
#define CVNI_ROOT               0x0040
#define CVNI_NEXT               0x0080
#define CVNI_PREVIOUS           0x00f0
#define CVNI_PARENT             0x0100
#define CVNI_CHILD              0x0200
#define CVNI_FIRSTVISIBLE       0x0400
#define CVNI_NEXTVISIBLE        0x0800
#define CVNI_PREVIOUSVISIBLE    0x0f00
#define CVNI_LASTVISIBLE		0x1000

// Hit test values (not yet implemented)
#define CVHT_NOWHERE            0x0001
#define CVHT_ONITEMICON         0x0002
#define CVHT_ONITEMLABEL        0x0004
#define CVHT_ONITEM             0x0008
#define CVHT_ONITEMINDENT       0x0010
#define CVHT_ONITEMBUTTON       0x0020
#define CVHT_ONITEMNUMBER		0x0040

#define CVHT_ABOVE              0x0100
#define CVHT_BELOW              0x0200
#define CVHT_TORIGHT            0x0400
#define CVHT_TOLEFT             0x0800

#define CVE_COLLAPSE			TVE_COLLAPSE
#define CVE_EXPAND				TVE_EXPAND
#define CVE_TOGGLE				TVE_TOGGLE

// Messages sent to the grid's parent (More will be added in future)
#define CVN_SELCHANGING         TVN_SELCHANGING
#define CVN_SELCHANGED          TVN_SELCHANGED
#define CVN_GETDISPINFO         TVN_GETDISPINFO
#define CVN_SETDISPINFO         TVN_SETDISPINFO
#define CVN_ITEMEXPANDING       TVN_ITEMEXPANDING
#define CVN_ITEMEXPANDED        TVN_ITEMEXPANDED
#define CVN_BEGINDRAG           TVN_BEGINDRAG
#define CVN_BEGINRDRAG          TVN_BEGINRDRAG
#define CVN_DELETEITEM          TVN_DELETEITEM
#define CVN_BEGINLABELEDIT      TVN_BEGINLABELEDIT
#define CVN_ENDLABELEDIT        TVN_ENDLABELEDIT
#define CVN_ACTIVATE	        TVN_SINGLEEXPAND
#define CVN_NUMBER				TVN_GETINFOTIP
//#define CVN_SELECTED			TVN_GETINFOTIPW


/////////////////////////////////////////////////////////////////////////////
// CContextCtrl window

class CContextCtrl : public CWnd
{
    DECLARE_DYNCREATE(CContextCtrl)
    friend class CContextItemBase;

// Construction
public:
	CContextCtrl();
	virtual ~CContextCtrl();

    BOOL Create(const RECT& rect, CWnd* parent, UINT nID,
                DWORD dwStyle = WS_CHILD | WS_TABSTOP | WS_VISIBLE);

// Attributes
public:
    void			SetImageList(CImageList* pList)			{	m_pImageList = pList;			}
    CImageList *	GetImageList() const					{	return m_pImageList;			}

    BOOL GetItemOrigin(HCONTEXTITEM hItem, LPPOINT p);
    BOOL GetTextRect(HCONTEXTITEM hItem, LPRECT pRect);
    CSize GetTextExtent(HCONTEXTITEM hItem, LPCTSTR str);
    CSize GetItemTextExtent(HCONTEXTITEM hItem)				{	return GetTextExtent(hItem, NULL);	}

	CFont* GetFont()										{	return &m_Font;					}
	CFont* GetBoldFont()									{	return &m_BoldFont;				}


    void SetEditable(BOOL bEditable = TRUE)					{	m_bEditable = bEditable;		}
    BOOL IsEditable() const									{	return m_bEditable;				}
    void SetSingleSelection(BOOL bSing = TRUE)				{	m_bSingleSelection = bSing;		}
    BOOL GetSingleSelection()								{	return m_bSingleSelection;		}
    void EnableSelection(BOOL bEnable = TRUE)				{	ResetSelected(); m_bEnableSelection = bEnable; ResetSelected();	}
    BOOL IsSelectable() const								{	return m_bEnableSelection;		}
    void EnableTracking(BOOL bEnable = TRUE)				{	CancelTracking();	m_bEnableTracking = bEnable;	}
    BOOL IsTracking() const									{	return m_bEnableTracking;		}

    int  GetDefItemHeight() const							{	return m_nDefItemHeight;		}
    void SetDefItemHeight(int nHeight)						{	m_nDefItemHeight = nHeight;		}
    int  GetDefHeaderHeight() const							{	return m_nDefHeaderHeight;		}
    void SetDefHeaderHeight(int nHeight)					{	m_nDefHeaderHeight = nHeight;	}
	int  GetIndent() const									{	return m_nIndent;				}			
	void SetIndent(int nIndent);
	int	 GetMargin()										{	return m_nMargin;				}
	void SetMargin(int nMargin);
	int	 GetBorder()										{	return m_nBorder;				}
	void SetBorder(int nBorder);
	int	 GetIconSize()										{	return m_nIconSize;				}
	void SetIconSize(int nIconSize);
	int	 GetNumberOffset()									{	return m_nNumOffset;			}
	void SetNumberOffset(int nOffset);
	int	 GetImageOffset()									{	return m_nImageOffset;			}
	void SetImageOffset(int nOffset);
	int  GetExtSize()										{	return m_nExtSize;				}
	void SetExtSize(int nExtSize);

    void SetDoubleBuffering(BOOL bBuffer = TRUE)			{	m_bDoubleBuffer = bBuffer;		}
    BOOL GetDoubleBuffering() const							{	return m_bDoubleBuffer;			}
    void EnableTitleTips(BOOL bEnable = TRUE);//				{	m_bTitleTips = bEnable;			}
    BOOL GetTitleTips()										{	return m_bTitleTips;			}

	void	 SetTitleTipBackClr(COLORREF clr = CLR_DEFAULT)	{	m_crTTipBackClr = clr;			}
	COLORREF GetTitleTipBackClr()							{	return m_crTTipBackClr == CLR_DEFAULT ? ::GetSysColor(COLOR_INFOBK) : m_crTTipBackClr;	}
	void	 SetTitleTipTextClr(COLORREF clr = CLR_DEFAULT)	{	m_crTTipTextClr = clr;			}
	COLORREF GetTitleTipTextClr()							{	return m_crTTipTextClr == CLR_DEFAULT ? ::GetSysColor(COLOR_INFOTEXT) : m_crTTipTextClr;	}

    COLORREF GetTextColor() const							{	return m_crText == CLR_DEFAULT ? ::GetSysColor(COLOR_WINDOWTEXT) : m_crText;	}
    void     SetTextColor(COLORREF clr = CLR_DEFAULT)		{	m_crText = clr;					}
    COLORREF GetBkColor() const								{	return m_crBk == CLR_DEFAULT ? ::GetSysColor(COLOR_3DFACE) : m_crBk;	}
    void     SetBkColor(COLORREF clr = CLR_DEFAULT)			{	m_crBk = clr;					}
    COLORREF GetLightColor() const							{	return m_crLight == CLR_DEFAULT ? ::GetSysColor(COLOR_3DLIGHT) : m_crLight;	}
    void     SetLightColor(COLORREF clr = CLR_DEFAULT)		{	m_crLight = clr;				}
    COLORREF GetHighlightColor() const						{	return m_crHighlight == CLR_DEFAULT ? ::GetSysColor(COLOR_3DHILIGHT) : m_crHighlight;	}
    void     SetHighlightColor(COLORREF clr = CLR_DEFAULT)	{	m_crHighlight = clr;			}
    COLORREF GetShadowColor() const							{	return m_crShadow == CLR_DEFAULT ? ::GetSysColor(COLOR_3DSHADOW) : m_crShadow;	}
    void     SetShadowColor(COLORREF clr = CLR_DEFAULT)		{	m_crShadow = clr;				}
    COLORREF GetDKShadowColor() const						{	return m_crDKShadow == CLR_DEFAULT ? ::GetSysColor(COLOR_3DDKSHADOW) : m_crDKShadow;	}
    void     SetDKShadowColor(COLORREF clr = CLR_DEFAULT)	{	m_crDKShadow = clr;				}
	COLORREF GetTrackColor()								{	return m_crTrack == CLR_DEFAULT ? ::GetSysColor(COLOR_HIGHLIGHT) : m_crTrack;	}
	void	 SetTrackColor(COLORREF clr = CLR_DEFAULT)		{	m_crTrack = clr;				}
	COLORREF GetDisableColor()								{	return m_crDisable == CLR_DEFAULT ? ::GetSysColor(COLOR_3DFACE) : m_crDisable;	}
	void	 SetDisableColor(COLORREF clr = CLR_DEFAULT)	{	m_crDisable = clr;				}
	COLORREF GetEnableColor()								{	return m_crEnable == CLR_DEFAULT ? ::GetSysColor(COLOR_WINDOW) : m_crEnable;	}
	void	 SetEnableColor(COLORREF clr = CLR_DEFAULT)		{	m_crEnable = clr;				}

	CBrush * GetLightBrush()								{	return &m_brLight;	}


    BOOL IsItemEditing(HCONTEXTITEM hItem);
	BOOL IsItemVisible(HCONTEXTITEM hItem);
    BOOL IsItemSelected(HCONTEXTITEM hItem);
    BOOL IsItemEditable(HCONTEXTITEM hItem);
	BOOL IsValid(HCONTEXTITEM hItem);


	HCONTEXTITEM GetNextItem(HCONTEXTITEM hItem, UINT nCode);
	HCONTEXTITEM GetChildItem(HCONTEXTITEM hItem);
	HCONTEXTITEM GetNextSiblingItem(HCONTEXTITEM hItem);
	HCONTEXTITEM GetPrevSiblingItem(HCONTEXTITEM hItem);
	HCONTEXTITEM GetParentItem(HCONTEXTITEM hItem);
	HCONTEXTITEM GetFirstVisibleItem();
	HCONTEXTITEM GetLastVisibleItem();
	HCONTEXTITEM GetNextVisibleItem(HCONTEXTITEM hItem);
	HCONTEXTITEM GetPrevVisibleItem(HCONTEXTITEM hItem);
	HCONTEXTITEM GetRootItem();
	HCONTEXTITEM GetTrackedItem();
	HCONTEXTITEM FindRootItem(HCONTEXTITEM hItem);

	UINT		 GetVisibleCount();
	UINT		 GetVisibleItems(CItemList & rList, LPRECT lpRect);
	UINT		 GetVisibleItems(HCONTEXTITEM * phItems, int nCount, LPRECT lpRect);
	
	UINT		 GetSelectedCount();
	UINT		 GetSelectedItems(HCONTEXTITEM * phItems, int nCount);
	UINT		 GetSelectedItems(CItemList & rList);
	HCONTEXTITEM GetSelectedItem(HCONTEXTITEM hItem);

	BOOL		 SelectItem(HCONTEXTITEM hItem, BOOL bSelect = TRUE);

	BOOL		 SetItemNumber(HCONTEXTITEM hItem, int nNumber);
	int			 GetItemNumber(HCONTEXTITEM hItem);
	BOOL		 ItemHasNumber(HCONTEXTITEM hItem);

	BOOL	  GetItem(CVITEM* pItem);
	CString	  GetItemText(HCONTEXTITEM hItem);
	int		  GetItemImage(HCONTEXTITEM hItem);
	UINT	  GetItemState(HCONTEXTITEM hItem);
	UINT	  GetItemFormat(HCONTEXTITEM hItem);
	SHORT	  GetItemHeight(HCONTEXTITEM hItem);
	COLORREF  GetItemTextColor(HCONTEXTITEM hItem);
	COLORREF  GetItemBkColor(HCONTEXTITEM hItem);
	DWORD_PTR     GetItemData(HCONTEXTITEM hItem);
	BOOL	  GetItemRect(HCONTEXTITEM hItem, LPRECT lpRect);

	BOOL      SetItem(CVITEM* pItem);
	BOOL      SetItem(HCONTEXTITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage, UINT nState, 
					  UINT nFormat, LPARAM lParam, UINT	nHeight, COLORREF clrBack = CLR_DEFAULT, 
					  COLORREF clrText = CLR_DEFAULT);
	BOOL      SetItemText(HCONTEXTITEM hItem, LPCTSTR lpszItem);
	BOOL      SetItemImage(HCONTEXTITEM hItem, int nImage);
	BOOL      SetItemState(HCONTEXTITEM hItem, UINT nState);
	UINT	  SetItemFormat(HCONTEXTITEM hItem, UINT nFormat);
	SHORT	  SetItemHeight(HCONTEXTITEM hItem, SHORT cyHeight);
	COLORREF  SetItemTextColor(HCONTEXTITEM hItem, COLORREF clr);
	COLORREF  SetItemBkColor(HCONTEXTITEM hItem, COLORREF clr);
	BOOL      SetItemData(HCONTEXTITEM hItem, DWORD_PTR dwData);
	
	BOOL      ItemHasChildren(HCONTEXTITEM hItem);
	BOOL      ItemIsHeader(HCONTEXTITEM hItem);
	UINT	  GetCount();

// Operations

	virtual CScrollBar* GetScrollBarCtrl(int nBar);

	HCONTEXTITEM InsertItem(LPCVINSERTSTRUCT lpInsertStruct);
	HCONTEXTITEM InsertItem(LPCTSTR lpszItem, int nImage, UINT nState, UINT nFormat, 
							UINT nHeight, COLORREF clrBack, COLORREF clrText, LPARAM lParam,
							HCONTEXTITEM hParent, HCONTEXTITEM hInsertAfter);
	HCONTEXTITEM InsertItem(LPCTSTR lpszItem, HCONTEXTITEM hParent = CVI_ROOT,
							HCONTEXTITEM hInsertAfter = CVI_LAST, int nHeight = -1 );
	BOOL DeleteItem(HCONTEXTITEM hItem);
	BOOL DeleteAllItems();

	BOOL Expand(HCONTEXTITEM hItem, UINT nCode, BOOL bUpdate = true);
	BOOL ExpandAll(UINT nCode = CVE_EXPAND, BOOL bUpdate = true);
	
	BOOL		 SetItemFocus(HCONTEXTITEM hItem);
	HCONTEXTITEM GetItemFocus();
	HCONTEXTITEM GetPrevItemFocus()	{	return m_hPrevFocusItem;	}
	
	HCONTEXTITEM HitTest(CPoint pt, UINT* pFlags = NULL);
//	HCONTEXTITEM HitTest(TVHITTESTINFO* pHitTestInfo) const;

	BOOL SortChildren(HCONTEXTITEM hItem);
	BOOL EnsureVisible(HCONTEXTITEM hItem);
	BOOL SortChildrenCB(LPCVSORTCB pSort);

//    BOOL SetItemType(HCONTEXTITEM hItem, CRuntimeClass* pRuntimeClass);

    BOOL SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars = FALSE);
	void Invalidate(BOOL bErase = TRUE);
	void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);

	void UpdateWindow();

    BOOL RedrawItem(HCONTEXTITEM hItem, CDC * pDC = NULL);
    BOOL InvalidateItemRect(HCONTEXTITEM hItem);

	POSITION		GetItemPos(HCONTEXTITEM hItem);
	POSITION		GetFirstItemPos();
	POSITION		GetLastItemPos();
	POSITION		GetNextPos(POSITION pos);
	POSITION		GetPrevPos(POSITION pos);
	HCONTEXTITEM	GetItem(POSITION pos);
	
	virtual bool DrawIcon(CDC * pDC, CRect ImageRect, HCONTEXTITEM hItem);
	COLORREF CreateDitherColor();

	// return next visible header item
	HCONTEXTITEM GetNextVisibleHeaderItem(HCONTEXTITEM hItem);
	// return Prev visible header item
	HCONTEXTITEM GetPrevVisibleHeaderItem(HCONTEXTITEM hItem);


protected:
    BOOL	RegisterWindowClass();
    LRESULT SendMessageToParent(HCONTEXTITEM hItemNew, HCONTEXTITEM hItemOld, int nMessage, CPoint pt = CPoint(-1, -1));
	LRESULT SendMessageToParentDisp(CVITEM * pItem, int nMessage);
	void CopyCVITEM(CVITEM * pFrom, CVITEM * pTo);
	void RecalcNumber();

    void EraseBkgnd(CDC* pDC);

    int  SetMouseMode(int nMode)	{	int nOldMode = m_MouseMode; m_MouseMode = nMode; return nOldMode;	}
    int  GetMouseMode()				{	return m_MouseMode;	}

	void CancelTracking();

    HCONTEXTITEM GetItemFromPt(CPoint point);
    
    BOOL IsVisibleVScroll();
    virtual void ResetSelected();
    virtual void ResetScrollBars();
	virtual BOOL RecalcVisible();
	bool		 CalcVisible(HCONTEXTITEM hRoot, CItemList & list);
	CPoint	     GetPointClicked(HCONTEXTITEM hItem, const CPoint & point);


	int		GetVirtualHeight();
	BOOL	MakeVisible(HCONTEXTITEM hItem);

    void EnableScrollBars(int nBar, BOOL bEnable = TRUE);
    int  GetScrollPos32(int nBar, BOOL bGetTrackPos = FALSE);
    BOOL SetScrollPos32(int nBar, int nPos, BOOL bRedraw = TRUE);

    // Editing
    virtual void  OnEndEditItem(HCONTEXTITEM hItem, CString str);
    virtual void  OnEditItem(HCONTEXTITEM hItem, CPoint point, UINT nChar);

public:    
	virtual void  EndEditing();

protected:
    // Drawing
    virtual void  OnDraw(CDC* pDC);

	void	CreateLightBrush();
	HBITMAP	CreateDitherBitmap(HDC hDC);

	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO * pTI) const;

protected:

    BOOL			m_bMustUninitOLE;       // Do we need to uninitialise OLE?

	CContextRootItem m_RootItem; // root

	HCONTEXTITEM	m_FirstVisibleItem;	
	HCONTEXTITEM	m_LastVisibleItem;	
	HCONTEXTITEM	m_FocusItem;	
	HCONTEXTITEM	m_LeftClickDownItem;
	HCONTEXTITEM	m_TrackedItem;
	HCONTEXTITEM	m_hPrevFocusItem;

	COLORREF		m_crTTipBackClr, m_crTTipTextClr;
	CBrush			m_brLight;
	int				m_nDefHeaderHeight;
	int				m_nDefItemHeight;
	int				m_nMargin;
	int				m_nIndent;
	int				m_nNumOffset;
	int				m_nImageOffset;
	int				m_nMinMoveDistance;
	int				m_nBorder;
	int				m_nIconSize;
	int				m_nExtSize;

	COLORREF    m_crText, m_crBk, m_crLight, m_crHighlight, m_crShadow, m_crDKShadow;
	COLORREF    m_crEnable, m_crDisable, m_crTrack;
	BOOL		m_bEditable;
    BOOL        m_bSingleSelection;
	BOOL		m_bTitleTips;
    BOOL        m_bAllowDraw;
    int         m_nVScrollMax;
	BOOL		m_bDoubleBuffer;
	BOOL		m_bEnableSelection;
	BOOL		m_bEnableTracking;
	int			m_nRowsPerWheelNotch;
	bool		m_bFocusOnSelected;

    LOGFONT     m_Logfont;
    CFont       m_Font,
				m_BoldFont;
				
    CImageList* m_pImageList;

	int			m_MouseMode;
    BOOL        m_bMouseButtonDown;
	BOOL        m_bMouseLDblClick;

    CPoint      m_LeftClickDownPoint, m_LastMousePoint;

    int         m_nVTimerID;
    int         m_nMTimerID;
	int         m_nETimerID;
	int         m_nETimerCount;
    int         m_nTimerInterval;

    CRuntimeClass* m_pRtcItem;
    CRuntimeClass* m_pRtcHeader;       // determines kind of CGridCellLite created by default

    CTitleTip   m_TitleTip;             // Title tips for cells

	CItemList	m_list;
	CItemList	m_listSelected;
	CItemList	m_listVisible;
	CItemList	m_listNumeric;


protected:
    // Generated message map functions
    //{{AFX_MSG(CContextCtrl)
    afx_msg void OnPaint();
    afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    afx_msg UINT OnGetDlgCode();
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnCancelMode();
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
#if _MSC_VER >= 1300
	afx_msg void OnActivateApp(BOOL bActive, DWORD hTask);
#else
	afx_msg void OnActivateApp(BOOL bActive, HTASK hTask);
#endif
	//}}AFX_MSG
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnSysColorChange();
    afx_msg void OnCaptureChanged(CWnd *pWnd);
    afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
    afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
    afx_msg LRESULT OnSetFont(WPARAM hFont, LPARAM lParam);
    afx_msg LRESULT OnGetFont(WPARAM hFont, LPARAM lParam);
    afx_msg void OnEndInPlaceEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnNeedToolTipText(UINT id, NMHDR * pTTTStruct, LRESULT * pResult);
    DECLARE_MESSAGE_MAP()

    enum eMouseModes 
	{ 
		MOUSE_NOTHING, 
        MOUSE_SELECT, 
		MOUSE_PREPARE_NUMBER_ITEM, 
		MOUSE_PREPARE_SELECT_ITEM, 
		MOUSE_SELECT_ITEM,
        MOUSE_PREPARE_EDIT,
        MOUSE_PREPARE_DRAG, 
		MOUSE_DRAGGING
    };
};



/*
class CContextCtrl : public CWnd
{
    DECLARE_DYNCREATE(CContextCtrl)
    friend class CContextItemBase;

// Constructors
public:
	CContextCtrl();
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, 
				UINT nID = WS_CHILD | WS_BORDER | WS_TABSTOP | WS_VISIBLE);

// Attributes
	BOOL GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly) const;
	UINT GetCount() const;
	UINT GetIndent() const;
	void SetIndent(UINT nIndent);
	
	CImageList* GetImageList(UINT nImageList) const;
	CImageList* SetImageList(CImageList* pImageList, int nImageListType);

	HTREEITEM GetNextItem(HTREEITEM hItem, UINT nCode) const;
	HTREEITEM GetChildItem(HTREEITEM hItem) const;
	HTREEITEM GetNextSiblingItem(HTREEITEM hItem) const;
	HTREEITEM GetPrevSiblingItem(HTREEITEM hItem) const;
	HTREEITEM GetParentItem(HTREEITEM hItem) const;
	HTREEITEM GetFirstVisibleItem() const;
	HTREEITEM GetNextVisibleItem(HTREEITEM hItem) const;
	HTREEITEM GetPrevVisibleItem(HTREEITEM hItem) const;
	HTREEITEM GetSelectedItem() const;
	HTREEITEM GetDropHilightItem() const;
	HTREEITEM GetRootItem() const;
	BOOL	  GetItem(TVITEM* pItem) const;
	CString	  GetItemText(HTREEITEM hItem) const;
	BOOL	  GetItemImage(HTREEITEM hItem, int& nImage, int& nSelectedImage) const;
	UINT	  GetItemState(HTREEITEM hItem, UINT nStateMask) const;
	DWORD     GetItemData(HTREEITEM hItem) const;
	BOOL      SetItem(TVITEM* pItem);
	BOOL      SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
					  int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam);
	BOOL      SetItemText(HTREEITEM hItem, LPCTSTR lpszItem);
	BOOL      SetItemImage(HTREEITEM hItem, int nImage, int nSelectedImage);
	BOOL      SetItemState(HTREEITEM hItem, UINT nState, UINT nStateMask);
	BOOL      SetItemData(HTREEITEM hItem, DWORD_PTR dwData);
	BOOL      ItemHasChildren(HTREEITEM hItem) const;
	CEdit*	  GetEditControl() const;
	UINT	  GetVisibleCount() const;
	CToolTipCtrl* GetToolTips() const;
	CToolTipCtrl* SetToolTips(CToolTipCtrl* pWndTip);
	COLORREF GetBkColor() const;
	COLORREF SetBkColor(COLORREF clr);
	SHORT	 GetItemHeight(HTREEITEM hItem) const;
	SHORT	 SetItemHeight(HTREEITEM hItem, SHORT cyHeight);
	COLORREF GetTextColor() const;
	COLORREF SetTextColor(COLORREF clr);
	BOOL     SetInsertMark(HTREEITEM hItem, BOOL fAfter = TRUE);
	BOOL     GetCheck(HTREEITEM hItem) const;
	BOOL     SetCheck(HTREEITEM hItem, BOOL fCheck = TRUE);
	COLORREF GetInsertMarkColor() const;
	COLORREF SetInsertMarkColor(COLORREF clrNew);

// Operations
	HTREEITEM InsertItem(LPTVINSERTSTRUCT lpInsertStruct);
	HTREEITEM InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
						 int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
						 HTREEITEM hParent, HTREEITEM hInsertAfter);
	HTREEITEM InsertItem(LPCTSTR lpszItem, HTREEITEM hParent = TVI_ROOT,
						 HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(LPCTSTR lpszItem, int nImage, int nSelectedImage,
						 HTREEITEM hParent = TVI_ROOT, HTREEITEM hInsertAfter = TVI_LAST);
	BOOL DeleteItem(HTREEITEM hItem);
	BOOL DeleteAllItems();
	BOOL Expand(HTREEITEM hItem, UINT nCode);
	BOOL Select(HTREEITEM hItem, UINT nCode);
	BOOL SelectItem(HTREEITEM hItem);
	BOOL SelectDropTarget(HTREEITEM hItem);
	BOOL SelectSetFirstVisible(HTREEITEM hItem);
	CEdit* EditLabel(HTREEITEM hItem);
	HTREEITEM HitTest(CPoint pt, UINT* pFlags = NULL) const;
	HTREEITEM HitTest(TVHITTESTINFO* pHitTestInfo) const;
	CImageList* CreateDragImage(HTREEITEM hItem);
	BOOL SortChildren(HTREEITEM hItem);
	BOOL EnsureVisible(HTREEITEM hItem);
	BOOL SortChildrenCB(LPTVSORTCB pSort);

// Implementation
protected:
	void RemoveImageList(int nImageList);

public:
	virtual ~CContextCtrl();
	//{{AFX_MSG(CTreeCtrl)
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


*/
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTTREE_H__BBBFB489_9E5D_4445_AE61_61EF34AF9A84__INCLUDED_)
