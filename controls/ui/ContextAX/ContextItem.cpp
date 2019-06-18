// ContextItem.cpp : implementation file
//

#include "stdafx.h"
#include "ContextItem.h"
#include "ContextTree.h"
#include "InPlaceEdit.h"
#include "Resource.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CContextItemBase

IMPLEMENT_DYNCREATE(CContextItemBase,	CObject);
IMPLEMENT_DYNCREATE(CContextItem,		CContextItemBase);
IMPLEMENT_DYNCREATE(CContextItemHeader,	CContextItemBase);
IMPLEMENT_DYNCREATE(CContextRootItem,	CContextItemBase);


CContextItemBase::CContextItemBase(CContextItemBase * pParent)
{
	Reset();
	m_pParent = pParent;
    m_pTree   = NULL;
}

CContextItemBase::~CContextItemBase()
{
	RemoveAllChildren();

	if (m_pParent)
		m_pParent->RemoveChild(this);
	m_pParent = 0;
}

void CContextItemBase::Reset()
{
	m_strText.Empty();			//  name of object 
	m_dwState	= 0;			//  Item state
	m_nHeight	= 0;			// item's height
    m_lParam	= 0;			// 32-bit value to associate with item
    m_nImage	= -1;			// Index of the list view item’s icon
    m_nFormat	= DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS;// item text draw format
    m_crBkClr	= CLR_DEFAULT;	// Background colour (or CLR_DEFAULT)
    m_crFgClr	= CLR_DEFAULT;	// Forground colour (or CLR_DEFAULT)
    m_bEditing	= false;		// item being edited?
    m_pEditWnd	= 0;	// pointer to edit window
	m_bExpand = false;

//    m_pTree     = NULL;
	
//	RemoveAllChildren();
//	if (m_pParent)
//		m_pParent->RemoveChild(this);

//	m_pParent = 0;

}

bool CContextItemBase::ToCVITEM(CVITEM * pItem)
{
	if (!pItem)
		return false;

    pItem->hItem   = HCONTEXTITEM(this);

//    pItem->mask    = CVIF_TEXT|CVIF_FORMAT|CVIF_STATE|CVIF_IMAGE|CVIF_PARAM|CVIF_BKCLR|CVIF_FGCLR|CVIF_HEIGHT;

    if ((pItem->mask & CVIF_TEXT) == CVIF_TEXT)
		pItem->strText = m_strText;     // Text in cell
	
	if ((pItem->mask & CVIF_FORMAT) == CVIF_FORMAT)
	    pItem->nFormat = m_nFormat;     // Format of cell
	
	if ((pItem->mask & CVIF_STATE) == CVIF_STATE)
		pItem->nState  = m_dwState;      // cell state (focus/hilighted etc)

	if ((pItem->mask & CVIF_IMAGE) == CVIF_IMAGE)
		pItem->nImage  = m_nImage;      // index of the list view item’s icon
	
	if ((pItem->mask & CVIF_BKCLR) == CVIF_BKCLR)
	    pItem->crBkClr = m_crBkClr;     // Background colour (or CLR_DEFAULT)
    
	if ((pItem->mask & CVIF_FGCLR) == CVIF_FGCLR)
		pItem->crFgClr = m_crFgClr;     // Forground colour (or CLR_DEFAULT)
    
	if ((pItem->mask & CVIF_PARAM) == CVIF_PARAM)
		pItem->lParam  = m_lParam;      // 32-bit value to associate with item
    
	if ((pItem->mask & CVIF_HEIGHT) == CVIF_HEIGHT)
		pItem->nHeight = m_nHeight;     // Internal cell margin
	
	return true;
}

bool CContextItemBase::FromCVITEM(CVITEM * pItem)
{
	if (!pItem)
		return false;

    if ((pItem->mask & CVIF_TEXT) == CVIF_TEXT)
	    m_strText = pItem->strText;     // Text in cell

	if ((pItem->mask & CVIF_FORMAT) == CVIF_FORMAT)
	    m_nFormat = pItem->nFormat;     // Format of cell

	if ((pItem->mask & CVIF_IMAGE) == CVIF_IMAGE)
	    m_nImage = pItem->nImage;      // index of the list view item’s icon

	if ((pItem->mask & CVIF_PARAM) == CVIF_PARAM)
		m_lParam = pItem->lParam;      // 32-bit value to associate with item

	if ((pItem->mask & CVIF_BKCLR) == CVIF_BKCLR)
	    m_crBkClr = pItem->crBkClr;     // Background colour (or CLR_DEFAULT)

	if ((pItem->mask & CVIF_FGCLR) == CVIF_FGCLR)
	    m_crFgClr = pItem->crFgClr;     // Forground colour (or CLR_DEFAULT)

	if ((pItem->mask & CVIF_HEIGHT) == CVIF_HEIGHT)
	    m_nHeight = pItem->nHeight;     // Internal cell margin
	
	if ((pItem->mask & CVIF_STATE) == CVIF_STATE)
		m_dwState = pItem->nState;      // cell state (focus/hilighted etc)

	return true;
}

void CContextItemBase::Update(UINT nMask)
{
}

UINT CContextItemBase::HitTest(CRect rect, CPoint pt)
{
	return 0;
}

void CContextItemBase::SetExpand(bool bFlag)
{
	m_bExpand = bFlag;
	if (m_pTree)
		m_pTree->InvalidateItemRect(this);
}


CFont* CContextItemBase::GetFont()
{
	bool bHilight = false;
	if ((GetState() & CVIS_SELECTED) == CVIS_SELECTED)
		bHilight = true;

	if (!m_pTree)
		return 0;
    return (bHilight) ? m_pTree->GetBoldFont() : m_pTree->GetFont();
}


// work w/ children
void CContextItemBase::SetParent(HCONTEXTITEM hParent)
{
	m_pParent = hParent;
}

POSITION CContextItemBase::InsertChild(HCONTEXTITEM hItem, HCONTEXTITEM hItemAfter/* = CVI_LAST*/)
{
	if (!hItem)
		return 0;

	POSITION pos = FindChild(hItem);
	if (pos)
		m_listChildren.RemoveAt(pos);

	if (hItemAfter == CVI_FIRST)
	{
		pos = m_listChildren.AddHead(hItem);
	}
	else if (hItemAfter == CVI_LAST)
	{
		pos = m_listChildren.AddTail(hItem);
	}
	else
	{
		pos = FindChild(hItemAfter);
		if (pos)
			pos = m_listChildren.InsertAfter(pos, hItem);
		else
			pos = m_listChildren.AddTail(hItem);
	}

	hItem->SetParent(this);

	if (hItemAfter == CVI_SORT)
	{
		SortChildren();
	}
	return pos;
}

bool CContextItemBase::RemoveChild(HCONTEXTITEM hItem)
{
	POSITION pos = FindChild(hItem);
	if (!pos)
		return false;

	return RemoveChild(pos);
}

bool CContextItemBase::RemoveChild(POSITION pos)
{
	POSITION posChild = pos;
	CContextItemBase * pChild = m_listChildren.GetNext(posChild);
	if (!pChild)
		return false;

	pChild->SetParent(0);
	m_listChildren.RemoveAt(pos);
	return true;
}

void CContextItemBase::RemoveAllChildren()
{
	for (POSITION pos = m_listChildren.GetHeadPosition(); pos != NULL; )
	{
		CContextItemBase * pItem = m_listChildren.GetNext(pos);
		delete pItem;
	}
}


HCONTEXTITEM CContextItemBase::GetFirst()
{
	return m_listChildren.GetHead();
}

HCONTEXTITEM CContextItemBase::GetLast()
{
	return m_listChildren.GetTail();
}

HCONTEXTITEM CContextItemBase::GetPrev(HCONTEXTITEM hItem)
{
	POSITION pos = FindChild(hItem);
	if (!pos)
		return 0;
	m_listChildren.GetPrev(pos);
	if (!pos)
		return 0;
	return m_listChildren.GetPrev(pos);
}

HCONTEXTITEM CContextItemBase::GetNext(HCONTEXTITEM hItem)
{
	POSITION pos = FindChild(hItem);
	if (!pos)
		return 0;
	m_listChildren.GetNext(pos);
	if (!pos)
		return 0;
	return m_listChildren.GetNext(pos);
}

POSITION CContextItemBase::GetFirstPos()
{
	return m_listChildren.GetHeadPosition();
}

POSITION CContextItemBase::GetLastPos()
{
	return m_listChildren.GetTailPosition();
}

POSITION CContextItemBase::GetPrevPos(POSITION pos)
{
	m_listChildren.GetPrev(pos);
	return pos;
}

POSITION CContextItemBase::GetNextPos(POSITION pos)
{
	if (!pos)
		return 0;

	m_listChildren.GetNext(pos);
	return pos;
}

HCONTEXTITEM CContextItemBase::GetChild(POSITION pos)
{
	return pos ? m_listChildren.GetNext(pos) : 0;
}

POSITION CContextItemBase::FindChild(HCONTEXTITEM hItem)
{
	if (hItem == CVI_FIRST || !hItem)
		return m_listChildren.GetHeadPosition();
	else if (hItem == CVI_LAST)
		return m_listChildren.GetTailPosition();

	return m_listChildren.Find(hItem);
}
#if 1
bool CContextItemBase::SortChildren()
{
	if (m_listChildren.GetCount() < 2)
		return true;

	CItemList list;
	int nCount = m_listChildren.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		POSITION pos = FindSmallest();
		if (!pos)
			continue;

		list.AddTail(m_listChildren.GetAt(pos));
		m_listChildren.RemoveAt(pos);
	}
	
	ASSERT(m_listChildren.GetCount() == 0);
	
	for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
		m_listChildren.AddTail(list.GetNext(pos));

	list.RemoveAll();


	return true;
}

bool CContextItemBase::SortChildrenCB(LPCVSORTCB pSort)
{
	if (m_listChildren.GetCount() < 2)
		return true;

	if (!pSort || !pSort->lpfnCompare)
		return false;

	CItemList list;
	int nCount = m_listChildren.GetCount();
	for (int i = 0; i < nCount; i++)
	{
		POSITION pos = FindSmallestCB(pSort);
		if (!pos)
			continue;

		list.AddTail(m_listChildren.GetAt(pos));
		m_listChildren.RemoveAt(pos);
	}

	ASSERT(m_listChildren.GetCount() == 0);

	for (POSITION pos = list.GetHeadPosition(); pos != NULL;)
		m_listChildren.AddTail(list.GetNext(pos));

	list.RemoveAll();

	return true;
}
#else

bool CContextItemBase::SortChildren()
{
	int nCount = m_listChildren.GetCount();
	if (nCount < 2)
		return true;

	CItemArray arr;
	arr.SetSize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		arr.SetAt(i, m_listChildren.GetHead());
		m_listChildren.RemoveHead();
	}
	
	ASSERT(m_listChildren.GetCount() == 0);

	LPVOID lptr = (LPVOID)arr.GetData();
	ContextItemQSort(arr.GetData(), nCount, CContextItemBase::DefaultCompareFunc, 0);

	for (i = 0; i < nCount; i++)
		m_listChildren.AddTail(arr[i]);

	arr.RemoveAll();

	return true;
}


int CALLBACK CContextItemBase::DefaultCompareFunc(HCONTEXTITEM hItem1, HCONTEXTITEM hItem2, LPARAM lParam)
{
	return lstrcmpi(hItem1->GetText(), hItem2->GetText());
}


bool CContextItemBase::SortChildrenCB(LPCVSORTCB pSort)
{
	if (!pSort || !pSort->lpfnCompare)
		return false;

	int nCount = m_listChildren.GetCount();
	if (nCount < 2)
		return true;

	CItemArray arr;
	arr.SetSize(nCount);
	for (int i = 0; i < nCount; i++)
	{
		arr.SetAt(i, m_listChildren.GetHead());
		m_listChildren.RemoveHead();
	}
	
	ASSERT(m_listChildren.GetCount() == 0);

	ContextItemQSort(arr.GetData(), nCount, pSort->lpfnCompare, pSort->lParam);

	for (i = 0; i < nCount; i++)
		m_listChildren.AddTail(arr[i]);

	arr.RemoveAll();

	return true;
}

#endif

POSITION CContextItemBase::FindSmallest()
{
	if (!m_listChildren.GetCount())
		return 0;

	POSITION RetPos = m_listChildren.GetHeadPosition();
	POSITION pos = RetPos;
	CContextItemBase * pItem = m_listChildren.GetNext(pos);

	for (; pos != 0; )
	{
		POSITION prevPos = pos;
		CContextItemBase * pCmpItem = m_listChildren.GetNext(pos);

		if (!pCmpItem)
			continue;
		
		if (pItem > pCmpItem)
		{
			RetPos = prevPos;
			pItem = pCmpItem;
		}
		
	}
	return RetPos;
}

POSITION CContextItemBase::FindSmallestCB(LPCVSORTCB pSort)
{
	if (!m_listChildren.GetCount())
		return 0;

	POSITION RetPos = m_listChildren.GetHeadPosition();
	POSITION pos = RetPos;
	CContextItemBase * pItem = m_listChildren.GetNext(pos);

	for (; pos != 0; )
	{
		POSITION prevPos = pos;
		CContextItemBase * pCmpItem = m_listChildren.GetNext(pos);

		if (!pCmpItem)
			continue;
		
		int nRet = pSort->lpfnCompare(pItem, pCmpItem, pSort->lParam);
		if (nRet > 0)
		{
			RetPos = prevPos;
			pItem = pCmpItem;
		}
	}
	return RetPos;
}

// overridable
bool CContextItemBase::Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd)
{
	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);

    if (!pTree || !pDC)
        return false;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return false;           //  though cell is hidden

	CBrush * pLightBrush = pTree->GetLightBrush();

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;
    if (GetTextClr() == CLR_DEFAULT)
        TextClr = pTree->GetTextColor();
    else
        TextClr = GetTextClr();

    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pTree->GetBkColor();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }

    // Draw cell background and highlighting (if necessary)
    if ((GetState() & CVIS_FOCUSED) || (GetState() & CVIS_DROPHILITED))
    {
        // Always draw even in list mode so that we can tell where the
        // cursor is at.  Use the highlight colors though.
        if (GetState() & CVIS_SELECTED)
        {
            TextBkClr = ::GetSysColor(COLOR_3DDKSHADOW);
            TextClr = ::GetSysColor(COLOR_3DHIGHLIGHT);
            bEraseBkgnd = TRUE;
        }

        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBkgnd)
        {
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
        }

        // Use same color as text to outline the cell so that it shows
        // up if the background is black.
        CBrush brush(TextClr);
        pDC->FrameRect(rect, &brush);
        pDC->SetTextColor(TextClr);

        rect.DeflateRect(1,1);
    }
    else if ((GetState() & CVIS_SELECTED))
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_3DDKSHADOW));
        rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_3DHIGHLIGHT));
    }
    else
    {
		
        bool bHilite = pTree->GetItemFocus() == this;
		bool bTracked = (GetState() & CVIS_TRACKED) == CVIS_TRACKED;
		
		if (bHilite && pLightBrush && !bTracked)
		{
			pDC->FillRect(rect, pLightBrush);
		}
        else if (bEraseBkgnd)
        {
            rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
			CBrush brush(TextBkClr);
			pDC->FillRect(rect, &brush);
            rect.right--; rect.bottom--;
        }
        pDC->SetTextColor(TextClr);

    }

    // Setup font and if fixed, draw fixed cell highlighting

	BOOL bHilite = false;
	bool bTracked = (GetState() & CVIS_TRACKED) == CVIS_TRACKED;
    // Draw lines only when wanted
    if (pTree->ItemIsHeader(this))
    {
        // As above, always show current location even in list mode so
        // that we know where the cursor is at.
        bHilite = pTree->GetItemFocus() == this;

        // If this fixed cell is on the same row/col as the focus cell,
        // highlight it.
        if (bHilite)
        {

			CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
				 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
				 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
				*pOldPen = pDC->GetCurrentPen();

			pDC->SelectObject(&medpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rect.right - 1, rect.top + 1);
			pDC->LineTo(rect.right - 1, rect.bottom - 1);
			pDC->LineTo(rect.left, rect.bottom - 1);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.right, rect.bottom);
			pDC->LineTo(rect.left - 1, rect.bottom);

			pDC->SelectObject(pOldPen);
			rect.DeflateRect(1,1);
        }
        else
        {
			CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
				 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
				 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
				*pOldPen = pDC->GetCurrentPen();

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.left, rect.top);
			pDC->LineTo(rect.left, rect.bottom);

			if (bTracked)
			{
				pDC->SelectObject(&medpen);
				pDC->MoveTo(rect.right - 1, rect.top + 1);
				pDC->LineTo(rect.right - 1, rect.bottom - 1);
				pDC->LineTo(rect.left + 1, rect.bottom - 1);
			}

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rect.right, rect.top);
			pDC->LineTo(rect.right, rect.bottom);
			pDC->LineTo(rect.left - 1, rect.bottom);

			pDC->SelectObject(pOldPen);

			rect.DeflateRect(1,1);
        }
    }
    CFont* pFont = (bHilite) ? pTree->GetBoldFont() : pTree->GetFont();
    pDC->SelectObject(pFont);

    // Draw Text and image

    rect.DeflateRect(pTree->GetMargin(), 0);

    if (pTree->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pTree->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            //  would like to use a clipping region but seems to have issue
            //  working with CMemDC directly.  Instead, don't display image
            //  if any part of it cut-off
            //
            // CRgn rgn;
            // rgn.CreateRectRgnIndirect(rect);
            // pDC->SelectClipRgn(&rgn);
            // rgn.DeleteObject();

            int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
            int nImageHeight = Info.rcImage.bottom-Info.rcImage.top+1;

            if (nImageWidth + rect.left <= rect.right + (int)(2*pTree->GetMargin())
                && nImageHeight + rect.top <= rect.bottom + (int)(2*pTree->GetMargin()))
            {
                pTree->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            }
            rect.left += nImageWidth+pTree->GetMargin();
        }
    }

    // We want to see '&' characters so use DT_NOPREFIX
    DrawText(pDC->m_hDC, GetText(), -1, rect, GetFormat() | DT_NOPREFIX);

    pDC->RestoreDC(nSavedDC);

    return TRUE;
}

bool CContextItemBase::GetTextRect(LPRECT pRect)
{
	if (GetImage() >= 0)
	{
		IMAGEINFO Info;

        CContextCtrl * pTree = GetTree();
        CImageList* pImageList = pTree->GetImageList();
        
		if (pImageList->GetImageInfo(GetImage(), &Info))
		{
			int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
			pRect->left += nImageWidth + pTree->GetMargin();
		}
	}

    return true;
}

CSize CContextItemBase::GetTextExtent(LPCTSTR str)
{
	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);

    CDC* pDC = pTree->GetDC();
    if (!pDC) 
		return CSize(0,0);

    CFont * pOldFont = (CFont*) pDC->SelectObject(((GetState() & CVIS_SELECTED) == CVIS_SELECTED) ? pTree->GetBoldFont() : pTree->GetFont());

	CString strText = str;
	if (strText.IsEmpty())
		strText = GetText();

    CSize size = pDC->GetTextExtent(strText);// + _T("."));

    pDC->SelectObject(pOldFont);
    pTree->ReleaseDC(pDC);

    return size + CSize(2*pTree->GetMargin(), 2*pTree->GetMargin());
}

CSize CContextItemBase::GetItemExtent(CDC * pDC)
{
	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);

    CFont* pfont = (GetState() & CVIS_FOCUSED) ? pTree->GetBoldFont() : pTree->GetFont();

    CSize size;
    int nFormat = GetFormat();

    // If the Item is a multiline item, then use the width of the item
    // to get the height
    CFont* pOldFont = pDC->SelectObject(pfont);
    if ((nFormat & DT_WORDBREAK) && !(nFormat & DT_SINGLELINE))
    {
        CString str = GetText();
		if (str.IsEmpty())
			str = _T("AA");
        int nMaxWidth = 0;
        while (TRUE)
        {
            int nPos = str.Find(_T('\n'));
            CString TempStr = (nPos < 0)? str : str.Left(nPos);
            int nTempWidth = pDC->GetTextExtent(TempStr).cx;
            if (nTempWidth > nMaxWidth)
                nMaxWidth = nTempWidth;

            if (nPos < 0)
                break;
            str = str.Mid(nPos + 1);    // Bug fix by Thomas Steinborn
        }
        
        CRect rect;
        rect.SetRect(0,0, nMaxWidth, 0);
        pDC->DrawText(GetText(), rect, nFormat | DT_CALCRECT);
        size = rect.Size();
    }
    else
	{
		CString str = GetText();
		if (str.IsEmpty())
			str = _T("AA");

        size = pDC->GetTextExtent(GetText());
	}
    pDC->SelectObject(pOldFont);
    
    size += CSize(2*pTree->GetMargin(), 2*pTree->GetMargin());
    
    CSize ImageSize(0,0);
    if (pTree->GetImageList()) 
    {
        int nImage = GetImage();
        if (nImage >= 0) 
        {
            IMAGEINFO Info;
            if (pTree->GetImageList()->GetImageInfo(nImage, &Info))
                ImageSize = CSize(Info.rcImage.right-Info.rcImage.left+1, 
                                  Info.rcImage.bottom-Info.rcImage.top+1);
        }
    }
    
    return CSize(size.cx + ImageSize.cx, max(size.cy, ImageSize.cy));
}

bool CContextItemBase::PrintItem(CDC * pDC, CRect rect)
{
/*    COLORREF crFG, crBG;
    CV_ITEM Item;

	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);
    if (!pTree || !pDC)
        return FALSE;

    if (rect.Width() <= 0
        || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return FALSE;           //  though cell is hidden

    int nSavedDC = pDC->SaveDC();

    BOOL bDrawFixed = ((GetState() & CVIS_FIXED) == CVIS_FIXED);

    pDC->SetBkMode(TRANSPARENT);

    if (pTree->GetShadedPrintOut())
    {
        // Use custom color if it doesn't match the default color and the
        // default tree background color.  If not, leave it alone.
        crBG = (GetBackClr() != CLR_DEFAULT && GetBackClr() != pTree->GetTextBkColor()) ?
                GetBackClr() : CLR_DEFAULT;

        // Use custom color if the background is different or if it doesn't
        // match the default color and the default tree text color.  If not,
        // use black to guarantee the text is visible.
        crFG = (GetBackClr() != CLR_DEFAULT ||
                (GetTextClr() != CLR_DEFAULT && GetTextClr() != pTree->GetTextColor())) ?
                 GetTextClr() : RGB(0, 0, 0);

        // If not printing on a color printer, adjust the foreground color
        // to a gray scale if the background color isn't used so that all
        // colors will be visible.  If not, some colors turn to solid black
        // or white when printed and may not show up.  This may be caused by
        // coarse dithering by the printer driver too (see image note below).
        if (pDC->GetDeviceCaps(NUMCOLORS) == 2 && crBG == CLR_DEFAULT)
            crFG = RGB(GetRValue(crFG) * 0.30, GetGValue(crFG) * 0.59,
                GetBValue(crFG) * 0.11);

        // Only erase the background if the color is not the default
        // tree background color.
        if (crBG != CLR_DEFAULT)
        {
            CBrush brush(crBG);
            rect.right++; rect.bottom++;
            pDC->FillRect(rect, &brush);
            rect.right--; rect.bottom--;
        }
    }
    else
    {
        crBG = CLR_DEFAULT;
        crFG = RGB(0, 0, 0);
    }

    pDC->SetTextColor(crFG);

    // Create the appropriate font and select into DC.
    CFont Font;

    // Bold the fixed cells if not shading the print out.  Use italic
    // font it it is enabled.
    const LOGFONT* plfFont = GetFont();
    if (bDrawFixed && !pTree->GetShadedPrintOut())
    {
        Font.CreateFont(plfFont->lfHeight, 0, 0, 0, FW_BOLD, plfFont->lfItalic, 0, 0,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            PROOF_QUALITY,
            VARIABLE_PITCH | FF_SWISS, plfFont->lfFaceName);
    }
    else
        Font.CreateFontIndirect(plfFont);

    pDC->SelectObject(&Font);

    // Draw lines only when wanted on fixed cells.  Normal cell tree lines
    // are handled in OnPrint.
    if (pTree->GetGridLines() != CVL_NONE && bDrawFixed)
    {
        CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
             darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
            *pOldPen = pDC->GetCurrentPen();

        pDC->SelectObject(&lightpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.left, rect.top);
        pDC->LineTo(rect.left, rect.bottom);

        pDC->SelectObject(&darkpen);
        pDC->MoveTo(rect.right, rect.top);
        pDC->LineTo(rect.right, rect.bottom);
        pDC->LineTo(rect.left, rect.bottom);

        rect.DeflateRect(1,1);
        pDC->SelectObject(pOldPen);
    }

    rect.DeflateRect(GetMargin(), 0);

    if (pTree->GetImageList() && GetImage() >= 0)
    {
        // NOTE: If your printed images look like fuzzy garbage, check the
        //       settings on your printer driver.  If it's using coarse
        //       dithering and/or vector graphics, they may print wrong.
        //       Changing to fine dithering and raster graphics makes them
        //       print properly.  My HP 4L had that problem.

        IMAGEINFO Info;
        if (pTree->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
            int nImageWidth = Info.rcImage.right-Info.rcImage.left;
            pTree->GetImageList()->Draw(pDC, GetImage(), rect.TopLeft(), ILD_NORMAL);
            rect.left += nImageWidth+GetMargin();
        }
    }

    // Draw without clipping so as not to lose text when printed for real
    DrawText(pDC->m_hDC, GetText(), -1, rect,
        GetFormat() | DT_NOCLIP | DT_NOPREFIX);

    pDC->RestoreDC(nSavedDC);
    Font.DeleteObject();
*/
    return true;
}


bool CContextItemBase::Edit(CRect rect, CPoint point, UINT nID, UINT nChar)
{
    DWORD dwStyle = ES_LEFT;
    if (GetFormat() & DT_RIGHT) 
        dwStyle = ES_RIGHT;
    else if (GetFormat() & DT_CENTER) 
        dwStyle = ES_CENTER;

    m_bEditing = true;
    
    // InPlaceEdit auto-deletes itself
	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);
	rect.right++; rect.bottom++;
    m_pEditWnd = new CInPlaceEdit((CWnd*)pTree, rect, dwStyle, nID, this, GetText(), nChar);
    
    return true;
}

void CContextItemBase::EndEdit()
{
    if (m_pEditWnd)
        ((CInPlaceEdit*)m_pEditWnd)->EndEdit();
}

LRESULT CContextItemBase::SendMessageToParent(int nMessage)
{
    if (m_pTree)
        return m_pTree->SendMessageToParent(this, 0, nMessage);
    return 0;
}

// protected overridable
void CContextItemBase::OnEndEdit()
{
    m_bEditing = false;
    m_pEditWnd = 0;
}

void CContextItemBase::OnMouseEnter(CPoint pt)
{
//	TRACE("CContextItemBase %x OnMouseEnter() at pos(%d, %d)\n", HCONTEXTITEM(this), pt.x, pt.y);
    UNUSED_ALWAYS(pt);

	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);

	if (!pTree)
		return;

	DWORD dwState = GetState();
	dwState |= CVIS_TRACKED;
	SetState(dwState);

	pTree->InvalidateItemRect(this);
	pTree->UpdateWindow();
}

void CContextItemBase::OnMouseOver(CPoint pt)
{
    UNUSED_ALWAYS(pt);
//	TRACE("CContextItemBase %x OnMouseOver() at pos(%d, %d)\n", HCONTEXTITEM(this), pt.x, pt.y);
}

void CContextItemBase::OnMouseLeave(CPoint pt)
{
//	TRACE("CContextItemBase %x OnMouseLeave() at pos(%d, %d)\n", HCONTEXTITEM(this), pt.x, pt.y);
    UNUSED_ALWAYS(pt);

	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);

	if (!pTree)
		return;

	DWORD dwState = GetState();
	dwState &= ~CVIS_TRACKED;
	SetState(dwState);

	pTree->InvalidateItemRect(this);
	pTree->UpdateWindow();
}

void CContextItemBase::OnClick(CPoint point)
{
    UNUSED_ALWAYS(point);
//	TRACE("CContextItemBase %x OnClick() at pos(%d, %d)\n", HCONTEXTITEM(this), point.x, point.y);
}

void CContextItemBase::OnClickDown(CPoint point)
{
    UNUSED_ALWAYS(point);
//	TRACE("CContextItemBase %x OnClickDown() at pos(%d, %d)\n", HCONTEXTITEM(this), point.x, point.y);
}

void CContextItemBase::OnRClick(CPoint point)
{
    UNUSED_ALWAYS(point);
//	TRACE("CContextItemBase %x OnRClick() at pos(%d, %d)\n", HCONTEXTITEM(this), point.x, point.y);
}

void CContextItemBase::OnDblClick(CPoint point)
{
    UNUSED_ALWAYS(point);
//	TRACE("CContextItemBase %x OnDblClick() at pos(%d, %d)\n", HCONTEXTITEM(this), point.x, point.y);
}

bool CContextItemBase::OnSetCursor(CPoint point)
{
    UNUSED_ALWAYS(point);
    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CContextItemHeader

CContextItemHeader::CContextItemHeader()
{
	m_bExpand = false;
}

CContextItemHeader::~CContextItemHeader()
{
}

void CContextItemHeader::Reset()
{
	CContextItemBase::Reset();
}


bool CContextItemHeader::Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd)
{
	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);

    if (!pTree || !pDC || !CalcLayout(rect))
        return false;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return false;           //  though cell is hidden

	CBrush * pLightBrush = pTree->GetLightBrush();

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    // Set up text and background colours
    COLORREF TextClr, TextBkClr;
    if (GetTextClr() == CLR_DEFAULT)
        TextClr = pTree->GetTextColor();
    else
        TextClr = GetTextClr();

    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pTree->GetBkColor();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }

	bool bHilite = (GetState() & CVIS_FOCUSED) == CVIS_FOCUSED;
	bool bTracked = (GetState() & CVIS_TRACKED) == CVIS_TRACKED;

    // Draw cell background and highlighting (if necessary)
/*	if (bHilite && pLightBrush && !bTracked)
	{
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		pDC->FillRect(rect, pLightBrush);
        rect.right--; rect.bottom--;
	}
    else */
		if (bEraseBkgnd)
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
		CBrush brush(TextBkClr);
		pDC->FillRect(rect, &brush);
        rect.right--; rect.bottom--;
    }
//	if (bTracked)
//        TextClr = pTree->GetTrackColor();

    pDC->SetTextColor(TextClr);

    // If this fixed cell is on the same row/col as the focus cell,
    // highlight it.
/*    if (bHilite)
    {
		CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
			 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
			 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
			*pOldPen = pDC->GetCurrentPen();

		pDC->SelectObject(&medpen);
		pDC->MoveTo(rect.right, rect.top);
		pDC->LineTo(rect.left, rect.top);
		pDC->LineTo(rect.left, rect.bottom);

		pDC->SelectObject(&lightpen);
		pDC->MoveTo(rect.right - 1, rect.top + 1);
		pDC->LineTo(rect.right - 1, rect.bottom - 1);
		pDC->LineTo(rect.left, rect.bottom - 1);

		pDC->SelectObject(&darkpen);
		pDC->MoveTo(rect.right, rect.top);
		pDC->LineTo(rect.right, rect.bottom);
		pDC->LineTo(rect.left - 1, rect.bottom);

		pDC->SelectObject(pOldPen);
		rect.DeflateRect(1, 1);

	}
    else
*/
	{
		CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
			 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
			 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
			*pOldPen = pDC->GetCurrentPen();

		pDC->SelectObject(&lightpen);
		pDC->MoveTo(rect.right, rect.top);
		pDC->LineTo(rect.left, rect.top);
		pDC->LineTo(rect.left, rect.bottom);

		if (bTracked)//pTree->GetBorder() > 1)
		{
			pDC->SelectObject(&medpen);
			pDC->MoveTo(rect.right - 1, rect.top + 1);
			pDC->LineTo(rect.right - 1, rect.bottom - 1);
			pDC->LineTo(rect.left, rect.bottom - 1);
		}

		pDC->SelectObject(&darkpen);
		pDC->MoveTo(rect.right, rect.top);
		pDC->LineTo(rect.right, rect.bottom);
		pDC->LineTo(rect.left - 1, rect.bottom);

		pDC->SelectObject(pOldPen);

		rect.DeflateRect(1, 1);
    }

    CFont* pFont = /* (bHilite) ? pTree->GetBoldFont() : */pTree->GetFont();
/*	bool bReleaseFont = false;
	CFont TrackFont;
	if (bTracked && pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		lf.lfUnderline = true;

		if (TrackFont.CreateFontIndirect(&lf))
		{
			bReleaseFont = true;
			pFont = &TrackFont;
		}
	}
*/
    pDC->SelectObject(pFont);

    // Draw button and Text
	if (IsExpanded())
	{
		DrawArrowUp(pDC, m_ButtonRect);
	}
	else
	{
		DrawArrowDown(pDC, m_ButtonRect);
	}

    // We want to see '&' characters so use DT_NOPREFIX
    DrawText(pDC->m_hDC, GetText(), -1, m_TextRect, GetFormat());

	if (bHilite)
	{
//		CRect rcFocus = m_TextRect;
//		rcFocus.InflateRect(1, 1, 1, 1);
		pDC->DrawFocusRect(m_TextRect);
	}

    pDC->RestoreDC(nSavedDC);

//	if (bReleaseFont)
//		TrackFont.DeleteObject();

    return TRUE;
}

bool CContextItemHeader::DrawArrowDown(CDC * pDC, CRect rect)
{
	rect.DeflateRect(1, 1, 1, 1);
	if (!pDC || rect.IsRectEmpty())
		return false;

	CPen pen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW));
	CPen * pOldPen = pDC->SelectObject(&pen);

	int ncy = rect.top + (int)floor((double)rect.Width() / 2.) - 1;
	if (ncy < rect.top)
		ncy = rect.top;

	int ncx = (int)floor((double)rect.Width() / 2.);

	for (int i = 0; i <= ncx; i++)
	{
		if (rect.left + i <= rect.right - i)
		{
			pDC->MoveTo(rect.left + i, ncy + i);
			pDC->LineTo(rect.right - i, ncy + i);
		}
	}

	pDC->SelectObject(pOldPen);
	
	return true;
}

bool CContextItemHeader::DrawArrowUp(CDC * pDC, CRect rect)
{
//	rect.DeflateRect(1, 1, 1, 1);
	if (!pDC || rect.IsRectEmpty())
		return false;

	CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
		 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
		 darkpen(PS_SOLID, 1, ::GetSysColor(COLOR_3DDKSHADOW)),
		*pOldPen = pDC->GetCurrentPen();

	pDC->SelectObject(&medpen);
	pDC->MoveTo(rect.right, rect.top);
	pDC->LineTo(rect.left - 1, rect.top);
	pDC->LineTo(rect.left - 1, rect.bottom + 1);


	pDC->SelectObject(&lightpen);
	pDC->MoveTo(rect.right, rect.top + 1);
	pDC->LineTo(rect.right, rect.bottom);
	pDC->LineTo(rect.left - 1, rect.bottom);


	rect.DeflateRect(1, 1, 1, 1);
	pDC->SelectObject(&darkpen);

	int ncy = rect.top + (int)ceil((double)rect.Width() * 2. / 3.) + 1;
	if (ncy > rect.bottom)
		ncy = rect.bottom;

	int ncx = (int)floor((double)rect.Width() / 2.);

	for (int i = 0; i <= ncx; i++)
	{
		if (rect.left + i <= rect.right - i)
		{
			pDC->MoveTo(rect.left + i, ncy - i);
			pDC->LineTo(rect.right - i, ncy - i);
		}
	}

	pDC->SelectObject(pOldPen);

	return true;
}

bool CContextItemHeader::Edit(CRect rect, CPoint point, UINT nID, UINT nChar)
{
	return false;
}

void CContextItemHeader::Update(UINT nMask)
{
}


UINT CContextItemHeader::HitTest(CRect rect, CPoint pt)
{
	if (!CalcLayout(rect))
		return CVHT_NOWHERE;

	UINT nRet = CVHT_NOWHERE;

	CRect rc = rect;
	CRect buttonRc = m_ButtonRect;
	CRect textRc = m_TextRect;
	CRect innRc = m_InnerRect;

	rc.InflateRect(1, 1);
	buttonRc.InflateRect(1, 1);
	textRc.InflateRect(1, 1);
	innRc.InflateRect(1, 1);

	if (!rc.PtInRect(pt))
		return CVHT_NOWHERE;

	else if (buttonRc.PtInRect(pt))
		nRet = CVHT_ONITEMBUTTON;

	else if (textRc.PtInRect(pt))
		nRet = CVHT_ONITEMLABEL;

	else if (innRc.PtInRect(pt))
		nRet = CVHT_ONITEM;

	return nRet;
}

bool CContextItemHeader::CalcLayout(CRect rect)
{
	CContextCtrl * pTree = GetTree();
	ASSERT(pTree);
	if (!pTree)
		return false;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return false;           //  though cell is hidden

	int nBorder		= pTree->GetBorder();
	int nMargin		= pTree->GetMargin();
	int nIndent		= pTree->GetIndent();

	bool bFocused = (GetState() & CVIS_FOCUSED) == CVIS_FOCUSED;
	// inner rect;
	m_InnerRect = rect;
	m_InnerRect.DeflateRect(1, 1);
    if (m_InnerRect.Width() <= 0 || m_InnerRect.Height() <= 0)  // prevents imagelist item from drawing even
        return false;           //  though cell is hidden

	m_ButtonRect.top = rect.top + 1;
	m_ButtonRect.bottom = rect.bottom - 2;//((nBorder > 1) ? 2 : 1);
	m_ButtonRect.right = m_InnerRect.right - 2;//((nBorder > 1) ? 2 : 1);
	m_ButtonRect.left = m_InnerRect.right - m_ButtonRect.Height() + 1;

	CSize size = GetTextExtent(NULL);

	m_TextRect = m_InnerRect;
	m_TextRect.left += nIndent;
	m_TextRect.right = m_TextRect.left + size.cx;
//	if (bFocused)
//		m_TextRect.OffsetRect(1, 1);
	if (m_TextRect.right > m_ButtonRect.left - 2 * nMargin)
		m_TextRect.right = m_ButtonRect.left - 2 * nMargin;


	return true;
}

bool CContextItemHeader::GetTextRect(LPRECT pRect)
{
	if (!pRect)
		return false;

	CRect rect;
	CContextCtrl * pTree = GetTree();
	ASSERT (pTree);
	if (!pTree)
		return false;

	if (!pTree->GetItemRect(this, rect))
		return false;

	if (!CalcLayout(rect))
		return false;

	pRect->left		= m_TextRect.left;
	pRect->top		= m_TextRect.top;
	pRect->right	= m_TextRect.right;
	pRect->bottom	= m_TextRect.bottom;

	return true;
}

bool CContextItemHeader::GetTipTextRect(LPRECT pRect)
{
	CContextCtrl * pTree = GetTree();
	ASSERT (pTree);
	if (!pRect || !pTree)
		return false;

	CRect rect;

	if (!pTree->GetItemRect(this, rect))
		return false;

	if (!CalcLayout(rect))
		return false;

	pRect->left		= m_TextRect.left;
	pRect->top		= m_TextRect.top;
	pRect->right	= m_TextRect.right;
	pRect->bottom	= m_TextRect.bottom;
	
	CSize size = GetTextExtent(NULL);

	if (size.cx < m_TextRect.Width())
		pRect->right = pRect->left + size.cx;
//	if (size.cy < m_TextRect.Height())
//		pRect->bottom = pRect->top + size.cy;

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CContextItem

CContextItem::CContextItem()
{
	m_nNumber = -1;
}

CContextItem::~CContextItem()
{
}

void CContextItem::Reset()
{
	CContextItemBase::Reset();
	m_nNumber = -1;
}

void CContextItem::Update(UINT nMask)
{
	CContextCtrl * pTree = GetTree();
	ASSERT(pTree);
	if (!pTree)
		return;

	if ((nMask & CVIF_IMAGE) == CVIF_IMAGE)
	{
		int nHeight = pTree->GetDefItemHeight();
		int nPrevHeight = GetHeight();
		if (nPrevHeight < nHeight)
			SetHeight(nHeight);
	}

}
// return where mouse is 
UINT CContextItem::HitTest(CRect rect, CPoint pt)
{
	if (!CalcLayout(rect))
		return CVHT_NOWHERE;

	UINT nRet = CVHT_NOWHERE;

	CRect rc = rect;
	CRect imageRc = m_ImageRect;
	CRect numberRc = m_NumberRect;
	CRect textRc = m_TextRect;
	CRect innRc = m_InnerRect;

	rc.InflateRect(1, 1);
	imageRc.InflateRect(1, 1);
	numberRc.InflateRect(1, 1);
	textRc.InflateRect(1, 1);
	innRc.InflateRect(1, 1);

	if (!rc.PtInRect(pt))
		return CVHT_NOWHERE;

	if (imageRc.PtInRect(pt))
		nRet = CVHT_ONITEMICON | CVHT_ONITEMBUTTON | CVHT_ONITEM;

	else if (numberRc.PtInRect(pt))
		nRet = CVHT_ONITEMNUMBER;

	else if (textRc.PtInRect(pt))
		nRet = CVHT_ONITEMLABEL|CVHT_ONITEM;

	else if (innRc.PtInRect(pt))
		nRet = CVHT_ONITEM;


	return nRet;
}

// calc layout 
// used in draw and in hittest functions
bool CContextItem::CalcLayout(CRect rect)
{
	CContextCtrl * pTree = GetTree();
	ASSERT(pTree);
	if (!pTree)
		return false;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return false;           //  though cell is hidden

	int nExtSize		= pTree->GetExtSize();
	int nBorder			= pTree->GetBorder();
	int nImageSize		= pTree->GetIconSize();
	int nMargin			= pTree->GetMargin();
	int nIndent			= pTree->GetIndent();
	int nNumHeight		= pTree->GetDefHeaderHeight();
	int nImageOffset	= pTree->GetImageOffset();
	int nNumberOffset	= pTree->GetNumberOffset();

	bool bFocused = (GetState() & CVIS_FOCUSED) == CVIS_FOCUSED;
	bool bSelected = (GetState() & CVIS_SELECTED) == CVIS_SELECTED;

	HCONTEXTITEM hRoot = pTree->GetRootItem();
	HCONTEXTITEM hParent = GetParent();

	int nCount = -1;
	while (hRoot != hParent && hParent)
	{
		nCount++;
		hParent = hParent->GetParent();
	}

	// inner rect;
	m_InnerRect = rect;
	m_InnerRect.DeflateRect(nMargin, 0, nMargin, 0);

	// number rect
	int ndx = (m_InnerRect.Height() - nNumHeight) / 2;
	ndx = ndx > 0 ? ndx : 0;

	m_NumberRect = m_InnerRect;
	m_NumberRect.left += nNumberOffset;
	m_NumberRect.right = m_NumberRect.left + nNumHeight - 1;

	m_NumberRect.bottom -= ndx;
	m_NumberRect.top = m_NumberRect.bottom - nNumHeight;


	// button rect
	m_ButtonRect = m_InnerRect;
	m_ButtonRect.DeflateRect(0, nBorder, 0, nBorder);
	m_ButtonRect.left = m_NumberRect.right + nCount*nIndent + nMargin + nImageOffset;
	m_ButtonRect.right = m_ButtonRect.left + nImageSize/* - 1*/ + 2*nExtSize;
	if (m_ButtonRect.Height() > (nImageSize - 1 + 2*nExtSize))
	{
		ndx = (int)((double)(m_ButtonRect.Height() - (nImageSize - 1 + 2*nExtSize)) / 2.);
		m_ButtonRect.bottom -= ndx;
		m_ButtonRect.top = m_ButtonRect.bottom - (nImageSize + 2*nExtSize);
	}

	// image rect
	m_ImageRect = m_ButtonRect;
	m_ImageRect.DeflateRect(nExtSize, nExtSize, nExtSize, nExtSize);
/*
	m_ImageRect.left = m_NumberRect.right + nCount*nIndent + nMargin + nImageOffset + nExtSize;
	m_ImageRect.right = m_ImageRect.left + nImageSize;// - 1;
	if (m_ImageRect.Height() > nImageSize - 1)
	{
		ndx = (int)((double)(m_ImageRect.Height() - nImageSize + 0.5)) / 2;
		m_ImageRect.bottom -= ndx;
		m_ImageRect.top = m_ImageRect.bottom - nImageSize;
	}
*/
	if (bSelected)
		m_ImageRect.OffsetRect(1, 1);

	CSize sizeText = GetTextExtent(NULL);
	ndx = (m_InnerRect.Height() - sizeText.cy) / 2;
	ndx = ndx > 0 ? ndx : 0;

	m_TextRect.left = m_ButtonRect.right + nExtSize + nBorder + 2*nMargin;
	m_TextRect.bottom = m_InnerRect.bottom - ndx + 1;
	m_TextRect.top = m_TextRect.bottom - sizeText.cy;
	m_TextRect.right = m_InnerRect.right - nBorder;

	if (m_TextRect.top <= m_InnerRect.top)
		m_TextRect.top = m_InnerRect.top + 1;
	if (m_TextRect.Width() > sizeText.cx)
		m_TextRect.right = m_TextRect.left + sizeText.cx;

	m_InnerRect.left = m_TextRect.left - 2 * nMargin;
	if (m_InnerRect.right < m_ButtonRect.right)
		m_InnerRect.right = m_ButtonRect.right;

	m_InnerRect.top = m_ButtonRect.top - ((nBorder > 1) ? 2 : 1);
	m_InnerRect.bottom = m_ButtonRect.bottom + ((nBorder > 1) ? 2 : 1);

	return true;
}

bool CContextItem::OnSetCursor(CPoint point)
{
	HCURSOR hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
	CContextCtrl * pTree = GetTree();
	ASSERT(pTree);
	CRect rect(0, 0, 0, 0);
	if (pTree && pTree->GetItemRect(this, rect))
	{
		if ((HitTest(rect, point) & CVHT_ONITEMNUMBER) == CVHT_ONITEMNUMBER)
			hCursor = AfxGetApp()->LoadCursor(IDC_CURSOR_NUMBER);
	}
	
    SetCursor(hCursor);
	return true;
}

void CContextItem::SetExpand(bool bFlag)
{
    UNUSED_ALWAYS(bFlag);
	m_bExpand = true;
}

bool CContextItem::Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd)
{
	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);

    if (!pTree || !pDC || !CalcLayout(rect))
        return false;

    if (rect.Width() <= 0 || rect.Height() <= 0)  // prevents imagelist item from drawing even
        return false;           //  though cell is hidden

	CBrush * pLightBrush = pTree->GetLightBrush();

    int nSavedDC = pDC->SaveDC();
    pDC->SetBkMode(TRANSPARENT);

    // Set up text and background colours
    
	COLORREF TextClr = (GetTextClr() == CLR_DEFAULT) ? pTree->GetTextColor() : GetTextClr();
	COLORREF TextBkClr;
    if (GetBackClr() == CLR_DEFAULT)
        TextBkClr = pTree->GetBkColor();
    else
    {
        bEraseBkgnd = TRUE;
        TextBkClr = GetBackClr();
    }

    bool bHilite = (GetState() & CVIS_SELECTED) == CVIS_SELECTED;
	bool bFocused = (GetState() & CVIS_FOCUSED) == CVIS_FOCUSED;
	bool bTracked = (GetState() & CVIS_TRACKED) == CVIS_TRACKED;

    // Draw cell background and highlighting (if necessary)
	rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
    CBrush brush(TextBkClr);
    pDC->FillRect(rect, &brush);
	rect.right--; rect.bottom--;    // FillRect doesn't draw RHS or bottom

	int nMargin = pTree->GetMargin();

    if (bHilite)
    {
		if (pLightBrush && !bTracked)
		{
			CRect rc = m_InnerRect;
	        rc.right++; rc.bottom++;    // FillRect doesn't draw RHS or bottom
			pDC->FillRect(rc, pLightBrush);
	        rc.right--; rc.bottom--;    // FillRect doesn't draw RHS or bottom
		}
    }
    // Draw Text and image
	CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
		 bkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DFACE)),
		 darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
		 medpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DSHADOW)),
		*pOldPen = pDC->GetCurrentPen();

	// draw number
	{
		CRect rc = m_NumberRect;
        rc.right++; rc.bottom++;
		if ((GetState() & CVIS_DISABLED) == CVIS_DISABLED)
		{
			CBrush BkBrush(pTree->GetDisableColor());
			pDC->FillRect(rc, &BkBrush);
		}
		else
		{
			CBrush BkBrush(pTree->GetEnableColor());
			pDC->FillRect(rc, &BkBrush);
		}
        rc.right--; rc.bottom--;

		pDC->SelectObject(&darkpen);
		pDC->MoveTo(m_NumberRect.right, m_NumberRect.top);
		pDC->LineTo(m_NumberRect.left, m_NumberRect.top);
		pDC->LineTo(m_NumberRect.left, m_NumberRect.bottom);
		pDC->LineTo(m_NumberRect.right, m_NumberRect.bottom);
		pDC->LineTo(m_NumberRect.right, m_NumberRect.top);

		pDC->SelectObject(&bkpen);
		m_NumberRect.DeflateRect(1, 1, 1, 1);
		pDC->MoveTo(m_NumberRect.right, m_NumberRect.top);
		pDC->LineTo(m_NumberRect.left, m_NumberRect.top);
		pDC->LineTo(m_NumberRect.left, m_NumberRect.bottom);

		pDC->SelectObject(&medpen);
		pDC->LineTo(m_NumberRect.right, m_NumberRect.bottom);
		pDC->LineTo(m_NumberRect.right, m_NumberRect.top);

		int nNum = GetNumber();
		if (nNum != -1)
		{
			pDC->SelectObject(pTree->GetFont());
			pDC->SetTextColor(pTree->GetTextColor());

			CString strNum;
			strNum.Format("%d", nNum);
			m_NumberRect.DeflateRect(1, 1, 1, 1);
			// We want to see '&' characters so use DT_NOPREFIX
			DrawText(pDC->m_hDC, strNum, -1, m_NumberRect, DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
		}
	}

	// draw image
	if ((GetState() & CVIS_SELECTED) == CVIS_SELECTED && pLightBrush)
	{
		CRect rc = m_ButtonRect;
		int nBorder = pTree->GetBorder();
		rc.InflateRect(nBorder, nBorder, nBorder, nBorder);
	    rc.right++; rc.bottom++;    // FillRect doesn't draw RHS or bottom
		pDC->FillRect(rc, pLightBrush);
	    rc.right--; rc.bottom--;    // FillRect doesn't draw RHS or bottom
	}

    if (pTree->GetImageList() && GetImage() >= 0)
    {
        IMAGEINFO Info;
        if (pTree->GetImageList()->GetImageInfo(GetImage(), &Info))
        {
			CPoint pt(m_ImageRect.left, m_ImageRect.top);
            pTree->GetImageList()->Draw(pDC, GetImage(), pt, ILD_NORMAL);
        }
    }
	else
	{
		pTree->DrawIcon(pDC, m_ImageRect, this);
	}
	
	// draw hightlight
    if (bHilite)
    {
		CRect rectHighlight = m_InnerRect;
		int nBorder = pTree->GetBorder();
		if (nBorder > 1)
		{
			pDC->SelectObject(&medpen);
			pDC->MoveTo(rectHighlight.right, rectHighlight.top);
			pDC->LineTo(rectHighlight.left, rectHighlight.top);
			pDC->LineTo(rectHighlight.left, rectHighlight.bottom + 1);

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rectHighlight.right, rectHighlight.top + 1);
			pDC->LineTo(rectHighlight.right, rectHighlight.bottom);
			pDC->LineTo(rectHighlight.left, rectHighlight.bottom);
			
			rectHighlight.DeflateRect(1, 1, 1, 1);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(rectHighlight.right, rectHighlight.top);
			pDC->LineTo(rectHighlight.left, rectHighlight.top);
			pDC->LineTo(rectHighlight.left, rectHighlight.bottom + 1);

			pDC->SelectObject(&bkpen);
			pDC->MoveTo(rectHighlight.right, rectHighlight.top + 1);
			pDC->LineTo(rectHighlight.right, rectHighlight.bottom);
			pDC->LineTo(rectHighlight.left, rectHighlight.bottom);
		}
		else
		{
			pDC->SelectObject(&medpen);
			pDC->MoveTo(rectHighlight.right, rectHighlight.top);
			pDC->LineTo(rectHighlight.left, rectHighlight.top);
			pDC->LineTo(rectHighlight.left, rectHighlight.bottom + 1);

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(rectHighlight.right, rectHighlight.top + 1);
			pDC->LineTo(rectHighlight.right, rectHighlight.bottom);
			pDC->LineTo(rectHighlight.left, rectHighlight.bottom);
		}

		pDC->SelectObject(pOldPen);
    }
    else if (bTracked)
    {
		CRect rectHighlight = m_InnerRect;
		pDC->SelectObject(&lightpen);
		pDC->MoveTo(rectHighlight.right, rectHighlight.top);
		pDC->LineTo(rectHighlight.left, rectHighlight.top);
		pDC->LineTo(rectHighlight.left, rectHighlight.bottom + 1);

		pDC->SelectObject(&medpen);
		pDC->MoveTo(rectHighlight.right, rectHighlight.top + 1);
		pDC->LineTo(rectHighlight.right, rectHighlight.bottom);
		pDC->LineTo(rectHighlight.left, rectHighlight.bottom);

		pDC->SelectObject(pOldPen);
    }

//	else 
	// for image button
	if (bHilite)
	{
		CRect btnRect = m_ButtonRect;
		int nBorder = pTree->GetBorder();
		btnRect.InflateRect(nBorder, nBorder, nBorder, nBorder);
		if (nBorder > 0)
		{
			pDC->SelectObject(&medpen);

			pDC->MoveTo(btnRect.right, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.bottom + 1);

			pDC->SelectObject(&lightpen);
			pDC->MoveTo(btnRect.right, btnRect.top + 1);
			pDC->LineTo(btnRect.right, btnRect.bottom);
			pDC->LineTo(btnRect.left, btnRect.bottom);
		}
		if (nBorder > 1)
		{
			btnRect.InflateRect(1, 1, 1, 1);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(btnRect.right, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.bottom + 1);

			pDC->SelectObject(&medpen);
			pDC->MoveTo(btnRect.right, btnRect.top + 1);
			pDC->LineTo(btnRect.right, btnRect.bottom);
			pDC->LineTo(btnRect.left, btnRect.bottom);
		}
	}
	else// if (!bHilite && !bTracked)
	{
		CRect btnRect = m_ButtonRect;
		int nBorder = pTree->GetBorder();
		btnRect.InflateRect(nBorder, nBorder, nBorder, nBorder);
		if (nBorder > 0)
		{
			pDC->SelectObject(&lightpen);
			pDC->MoveTo(btnRect.right, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.bottom + 1);

			pDC->SelectObject(&medpen);
			pDC->MoveTo(btnRect.right, btnRect.top + 1);
			pDC->LineTo(btnRect.right, btnRect.bottom);
			pDC->LineTo(btnRect.left, btnRect.bottom);
		}
		if (nBorder > 1)
		{
			btnRect.InflateRect(1, 1, 1, 1);

			pDC->SelectObject(&medpen);
			pDC->MoveTo(btnRect.right, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.top);
			pDC->LineTo(btnRect.left, btnRect.bottom + 1);

			pDC->SelectObject(&darkpen);
			pDC->MoveTo(btnRect.right, btnRect.top + 1);
			pDC->LineTo(btnRect.right, btnRect.bottom);
			pDC->LineTo(btnRect.left/* - 1*/, btnRect.bottom);
		}
	}
	pDC->SelectObject(pOldPen);

//	if (bTracked)
//		TextClr = pTree->GetTrackColor();
	pDC->SetTextColor(TextClr);

    CFont* pFont = (bHilite) ? pTree->GetBoldFont() : pTree->GetFont();
/*	bool bReleaseFont = false;
	CFont TrackFont;
	if (bTracked && pFont)
	{
		LOGFONT lf;
		pFont->GetLogFont(&lf);
		lf.lfUnderline = true;

		if (TrackFont.CreateFontIndirect(&lf))
		{
			bReleaseFont = true;
			pFont = &TrackFont;
		}
	}
*/
    pDC->SelectObject(pFont);

    // We want to see '&' characters so use DT_NOPREFIX
    DrawText(pDC->m_hDC, GetText(), -1, m_TextRect, GetFormat() | DT_NOPREFIX);
	if (bFocused)
	{
//		CRect rcFocus = m_TextRect;
//		rcFocus.InflateRect(1, 1, 1, 1);
		pDC->DrawFocusRect(m_TextRect);
	}

    pDC->RestoreDC(nSavedDC);

//	if (bReleaseFont)
//		TrackFont.DeleteObject();

    return TRUE;
}

bool CContextItem::Edit(CRect rect, CPoint point, UINT nID, UINT nChar)
{
	if (!CalcLayout(rect))
		return false;
    DWORD dwStyle = ES_LEFT;
    if (GetFormat() & DT_RIGHT) 
        dwStyle = ES_RIGHT;
    else if (GetFormat() & DT_CENTER) 
        dwStyle = ES_CENTER;

    m_bEditing = true;
    
    // InPlaceEdit auto-deletes itself
	CContextCtrl * pTree = GetTree();
    ASSERT(pTree);
	m_TextRect.InflateRect(1, 1);
    m_pEditWnd = new CInPlaceEdit((CWnd*)pTree, m_TextRect, dwStyle, nID, this, GetText(), nChar);

    return true;
}

bool CContextItem::GetTextRect(LPRECT pRect)
{
	if (!pRect)
		return false;

	CRect rect;
	CContextCtrl * pTree = GetTree();
	ASSERT (pTree);
	if (!pTree)
		return false;

	if (!pTree->GetItemRect(this, rect))
		return false;

	if (!CalcLayout(rect))
		return false;

	pRect->left		= m_TextRect.left;
	pRect->top		= m_TextRect.top;
	pRect->right	= m_TextRect.right;
	pRect->bottom	= m_TextRect.bottom;

	return true;
}

bool CContextItem::GetTipTextRect(LPRECT pRect)
{
	CContextCtrl * pTree = GetTree();
	ASSERT (pTree);
	if (!pRect || !pTree)
		return false;

	CRect rect;

	if (!pTree->GetItemRect(this, rect))
		return false;

	if (!CalcLayout(rect))
		return false;

	pRect->left		= m_TextRect.left;
	pRect->top		= m_TextRect.top;
	pRect->right	= m_TextRect.right;
	pRect->bottom	= m_TextRect.bottom;
	
	CSize size = GetTextExtent(NULL);

	if (size.cx > m_TextRect.Width())
		pRect->right = pRect->left + size.cx;
//	if (size.cy > m_TextRect.Height())
//		pRect->bottom = pRect->top + size.cy;

	return true;
}

/////////////////////////////////////////////////////////////////////////////
// CContextRootItem


CContextRootItem::CContextRootItem()
{
	m_bExpand = true;
}

CContextRootItem::~CContextRootItem()
{
}

void CContextRootItem::Reset()
{
	CContextItemBase::Reset();
	m_bExpand = true;
}

bool CContextRootItem::Draw(CDC * pDC, CRect rect, BOOL bEraseBkgnd)
{
    UNUSED_ALWAYS(pDC);
    UNUSED_ALWAYS(rect);
    UNUSED_ALWAYS(bEraseBkgnd);
	return true;
}

bool CContextRootItem::Edit(CRect rect, CPoint point, UINT nID, UINT nChar)
{
    UNUSED_ALWAYS(rect);
    UNUSED_ALWAYS(point);
    UNUSED_ALWAYS(nID);
    UNUSED_ALWAYS(nChar);
	return false;
}
