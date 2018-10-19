#ifndef __CONTEXT_ITEM_H__
#define __CONTEXT_ITEM_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ContextItem.h : header file
//

#include <AfxTempl.h>

// predefines
class CContextCtrl;
class CContextDataTree;
class CContextItemBase;
class CContextItem;
class CContextItemHeader;

#define SELECTED_ITEM_FONT_WEIGHT 800    // weight of text for selected items

///////////////////////////////////////////////////////////////////////////////
// CContextItemBase - base item 
typedef class CContextItemBase * HCONTEXTITEM;
typedef CList <HCONTEXTITEM, HCONTEXTITEM> CItemList;
typedef CArray <HCONTEXTITEM, HCONTEXTITEM> CItemArray;

#define CVI_ROOT	((HCONTEXTITEM)((LPVOID)0xffff0000))
#define CVI_FIRST	((HCONTEXTITEM)((LPVOID)0xffff0001))
#define CVI_LAST	((HCONTEXTITEM)((LPVOID)0xffff0002))
#define CVI_SORT	((HCONTEXTITEM)((LPVOID)0xffff0003))


// structures
typedef struct _CV_ITEM 
{
    HCONTEXTITEM	hItem;       // item
    UINT			mask;        // Mask for use in getting/setting cell data
    UINT			nState;      // cell state (focus/hilighted etc)
    UINT			nFormat;     // Format of cell
    CString			strText;     // Text in cell
    int				nImage;      // index of the list view item’s icon
    COLORREF		crBkClr;     // Background colour (or CLR_DEFAULT)
    COLORREF		crFgClr;     // Forground colour (or CLR_DEFAULT)
    LPARAM			lParam;      // 32-bit value to associate with item
    UINT			nHeight;     // Internal cell margin
} CVITEM;


typedef struct _CV_INSERTSTRUCT 
{
    HCONTEXTITEM hParent;
    HCONTEXTITEM hInsertAfter;
    CVITEM		 item;
} CVINSERTSTRUCT, * LPCVINSERTSTRUCT;


typedef struct _NM_CONTEXTVIEW 
{
    NMHDR	hdr;
    UINT	action;
    HCONTEXTITEM itemOld;
    HCONTEXTITEM itemNew;
    POINT		 point;
} NMCONTEXTVIEW, * LPNMCONTEXTVIEW;


typedef struct _CV_DISPINFO 
{
    NMHDR	hdr;
    CVITEM	item;
} NMCVDISPINFO, * LPNMCVDISPINFOA;


typedef int (CALLBACK *PFNCVCOMPARE)(HCONTEXTITEM hItem1, HCONTEXTITEM hItem2, LPARAM lParamSort);

typedef struct _CV_SORTCB
{
	HCONTEXTITEM	hParent;
	PFNCVCOMPARE	lpfnCompare;
	LPARAM			lParam;
} CVSORTCB, * LPCVSORTCB;



///////////////////////////////////////////////////////////////////////////////
// CContextItemBase class

class CContextItemBase : public CObject
{
	friend class CContextCtrl;
	DECLARE_DYNCREATE(CContextItemBase);

// Construction/Destruction
public:
	CContextItemBase(CContextItemBase * pParent = 0);
	virtual ~CContextItemBase();
	operator HCONTEXTITEM()	{	return this;	} 
	virtual bool ToCVITEM(CVITEM * pItem);
	virtual bool FromCVITEM(CVITEM * pItem);

// Attributes
public:
	virtual void SetText(LPCTSTR szText)			{	m_strText = szText;		}
	virtual void SetImage(int nImage)				{	m_nImage = nImage;		}
	virtual void SetState(DWORD dwState)			{	m_dwState = dwState;	}
	virtual void SetData(LPARAM lParam)				{	m_lParam = lParam;		}
	virtual void SetFormat(DWORD nFormat)			{	m_nFormat = nFormat;	}
	virtual void SetTextClr(COLORREF clr)			{	m_crFgClr = clr;		}
	virtual void SetBackClr(COLORREF clr)			{	m_crBkClr = clr;		}
	virtual void SetHeight(int nHeight)				{	m_nHeight = nHeight;	}
	virtual void SetTree(CContextCtrl * pTree)		{	m_pTree = pTree;		}

	virtual LPCTSTR		GetText()					{	return m_strText;		}
	virtual LPCTSTR		GetTipText()				{	return m_strText;		}
	virtual int			GetImage()					{	return m_nImage;		}
	virtual DWORD		GetState()					{	return m_dwState;		}
	virtual LPARAM		GetData()					{	return m_lParam;		}
	virtual DWORD		GetFormat()					{	return m_nFormat;		}
	virtual COLORREF	GetTextClr()				{	return m_crFgClr;		}
	virtual COLORREF	GetBackClr()				{	return m_crBkClr;		}
	virtual int			GetHeight()					{	return m_nHeight;		}
	CContextCtrl *		GetTree()					{	return m_pTree;			}
	CFont *				GetFont();

	virtual bool		IsEditing()					{	return m_bEditing;		}
	virtual bool		IsExpanded()				{	return m_bExpand;		}
	virtual void		SetExpand(bool bFlag = true);

	void			SetParent(HCONTEXTITEM hParent);
	HCONTEXTITEM	GetParent()						{	return m_pParent;					}
	bool			HasParent()						{	return m_pParent ? true : false;	} 

	POSITION		InsertChild(HCONTEXTITEM hItem, HCONTEXTITEM hItemAfter = CVI_LAST);
	bool 			RemoveChild(HCONTEXTITEM hItem);
	bool			RemoveChild(POSITION pos);
	void			RemoveAllChildren();

	HCONTEXTITEM	GetFirst();
	HCONTEXTITEM	GetLast();
	HCONTEXTITEM	GetPrev(HCONTEXTITEM hItem);
	HCONTEXTITEM	GetNext(HCONTEXTITEM hItem);

	POSITION		GetFirstPos();
	POSITION		GetLastPos();
	POSITION		GetPrevPos(POSITION pos);
	POSITION		GetNextPos(POSITION pos);


	HCONTEXTITEM	GetChild(POSITION pos);
	POSITION		FindChild(HCONTEXTITEM hItem);

	int				GetChildrenCount()				{	return (int)m_listChildren.GetCount();		}
	bool			HasChildren()					{	return m_listChildren.GetCount() != 0;	}
	bool			IsChild(HCONTEXTITEM hItem)		{	return FindChild(hItem) != 0;			}

	bool			SortChildren();
	bool			SortChildrenCB(LPCVSORTCB pSort);
	POSITION FindSmallest();
	POSITION FindSmallestCB(LPCVSORTCB pSort);

// operations
public:
    virtual void Reset();

	virtual bool Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual bool GetTextRect(LPRECT pRect);
    virtual bool GetTipTextRect(LPRECT pRect)	{	return GetTextRect(pRect);	}
    virtual CSize GetTextExtent(LPCTSTR str);
    virtual CSize GetItemExtent(CDC * pDC);

    virtual bool PrintItem(CDC * pDC, CRect rect);
    
    virtual bool Edit(CRect rect, CPoint point, UINT nID, UINT nChar);
    virtual void EndEdit();

    // add additional protected view members required of items
    LRESULT SendMessageToParent(int nMessage);


	virtual void	Update(UINT nMask);
	virtual UINT	HitTest(CRect rect, CPoint pt);


protected:
    virtual void OnEndEdit();
    virtual void OnMouseEnter(CPoint pt);
    virtual void OnMouseOver(CPoint pt);
    virtual void OnMouseLeave(CPoint pt);
    virtual void OnClick(CPoint pt);
    virtual void OnClickDown(CPoint pt);
    virtual void OnRClick(CPoint pt);
    virtual void OnDblClick(CPoint pt);
    virtual bool OnSetCursor(CPoint point);

protected:
	CString		m_strText;	// name of object 
	DWORD		m_dwState;	// Item state
	int			m_nHeight;	// item height
    LPARAM		m_lParam;	// 32-bit value to associate with item
    int			m_nImage;	// Index of the list view item’s icon
    DWORD		m_nFormat;	// item text draw format
    COLORREF	m_crFgClr;	// text color
    COLORREF	m_crBkClr;	// background color

    bool		m_bEditing;	// item being edited?
	bool		m_bExpand;  // item being expanded?
 
	CItemList			m_listChildren; // array of pointers to item's children 
	CContextItemBase *	m_pParent;

    CWnd *				m_pEditWnd;	// pointer to edit window
	CContextCtrl *		m_pTree;	// pointer to parent tree


public:
	static int CALLBACK DefaultCompareFunc(HCONTEXTITEM hItem1, HCONTEXTITEM hItem2, LPARAM lParam);

};



///////////////////////////////////////////////////////////////////////////////
// ÑContextItemHeader

class CContextItem : public CContextItemBase
{
	DECLARE_DYNCREATE(CContextItem);

// Construction/Destruction
public:
	CContextItem();
	virtual ~CContextItem();

public:
	virtual void SetNumber(int nNum)	{	m_nNumber = nNum;	}
	virtual int	 GetNumber()			{	return m_nNumber;	}

// overrided
    virtual void Reset();
	virtual bool Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual bool Edit(CRect rect, CPoint point, UINT nID, UINT nChar);

	virtual void SetExpand(bool bFlag = true);
	virtual void Update(UINT nMask);
	virtual UINT HitTest(CRect rect, CPoint pt);
	virtual bool CalcLayout(CRect rect);

    virtual bool GetTextRect(LPRECT pRect);
    virtual bool GetTipTextRect(LPRECT pRect);

// overrided
protected:
    virtual bool OnSetCursor(CPoint point);


protected:
	int		m_nNumber;

	CRect	m_InnerRect;
	CRect	m_ImageRect;
	CRect	m_NumberRect;
	CRect	m_TextRect;
	CRect	m_ButtonRect;

};

///////////////////////////////////////////////////////////////////////////////
// ÑContextItemHeader

class CContextItemHeader : public CContextItemBase
{
	DECLARE_DYNCREATE(CContextItemHeader);

// Construction/Destruction
public:
	CContextItemHeader();
	virtual ~CContextItemHeader();

//overrided
public:
    virtual void Reset();
	virtual bool Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual bool Edit(CRect rect, CPoint point, UINT nID, UINT nChar);

	virtual void Update(UINT nMask);
	virtual UINT HitTest(CRect rect, CPoint pt);
	virtual bool CalcLayout(CRect rect);

    virtual bool GetTextRect(LPRECT pRect);
    virtual bool GetTipTextRect(LPRECT pRect);

protected:
	bool DrawArrowDown(CDC * pDC, CRect rect);
	bool DrawArrowUp(CDC * pDC, CRect rect);

protected:
	CRect	m_ButtonRect;
	CRect	m_TextRect;
	CRect	m_InnerRect;

};

class CContextRootItem : public CContextItemBase
{
	DECLARE_DYNCREATE(CContextRootItem);

// Construction/Destruction
public:
	CContextRootItem();
	virtual ~CContextRootItem();

//overrided
public:
    virtual void Reset();
	virtual bool Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd = TRUE);
    virtual bool Edit(CRect rect, CPoint point, UINT nID, UINT nChar);


};



void ContextItemQSort(HCONTEXTITEM * base, unsigned num, PFNCVCOMPARE comp, LPARAM lParam);



#endif // __CONTEXT_ITEM_H__