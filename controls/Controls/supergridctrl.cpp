// SuperGridCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "Resource.h"
#include "SuperGridCtrl.h"
#include "ListEditCtrl.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/*
    SuperGrid, YALC (yet another listview control) 

	Written by me(Allan Nielsen), that is .. if the code works :)
		mailto:allan@object-oriented.com

	Copyright (c) 1999.

	If you find bugs, please notify me and I'll contact the author who wrote the darn thing :)
	
    You may use it, abuse it, redistribute it, in any way you desire, but
	if you choose to use this control in any commercial or non commercial application then please 
	send me an email letting me know, makes me :) . 
	

	HISTORY 

	what's new since last update: 

	4 dec. 1998.
	
				- MeasureItem now uses current font	
				- Combobox support in CItemInfo				
				  see the ShowList in MySuperGrid.h/cpp, PreTranslateMessage functions
				  and the file ComboInListView.cpp/h
                - function to indicate the precens of a combobox it draws a down arrow ..see DrawComboBox
				  the idea came from Robert Bouwens. :)
				- in file TestTreeDlg.cpp:  due to the MeasureItem message-reflection	
				  the listctrl is now created in the CDialog::OnCreate function and later on resized
				- LVIS_DROPHILITED support when dragging items
				- Support for LVS_EX_CHECKBOXES	  
				- Added some examples on how to print preview selected item(or at least to get some information from of the grid)
				  the sample is in the CMySuperGrid.h/cpp which is derived from CSuperGridCtrl it shows you
				  how to initalize the grid, sort items, search for items and subitems, select and delete thems
				- added virtual GetIcon function to set your default icon handler for a listview index.
				- added virtual GetCellRGB() to set the color for current selected cell
				- renamed CTreeNode to CTreeItem, sorry ;-(
				- the nested class CTreeItem has been stripped down to nothing but simple (struct)data members
				  only servering as a linked list of lists now :-)
				- well.. moved all the importent stuff from CTreeItem to CSuperGridCtrl e.g
				  all operations on a CTreeItem now resides in the CSuperGridCtrl, much more OO-like ;-)
				- added Quicksort function
				- got rid of TheOpenCloseThing function.
				- added virtual function OnUpdateListViewItem..called when ever an Item is about to be updated
				- added virtual function OnControlLButtonDown...called when ever LButtondown in a cell
				- added virtual function CreateDragImageEx...

	11 jan 1999:
				- added SetFocus in OnDblclk and in OnLButtonDown, big thank you to Dieter Gollwitzer 
				- fixed a minor hittest error in the HitTestOnSign(), again thanks to Dieter Gollwitzer 


	16 jan 1999: 
				- added virtual OnItemExpanding(....
				- added virtual OnItemExpanded(...
				- added virtual OnCollapsing(....
				- added virtual OnItemCollapsed(...
				- added virtual OnDeleteItem(....
				- added virtual OnVKMultiply(...
				- added virtual OnVkSubTract(..
				- added virtual OnVKAdd(....
				- added SetChildrenFlag();

	1 feb 1999:
				- CItemInfo now supports individual cell-color
				- fixed some bugs in regards to listdata associated with in each cell
				- added virtual BOOL OnVkReturn(........
				- added virtual BOOL OnItemLButtonDown(.....

	4 mai 1999  - moved to Switzerland to work as a systemdeveloper for 2-3 years :).
				- new email :)
				- added new class for encapsulating drawing + - buttons.

   22.june 1999 - Added GetCount()
				- removed support for VC++ 5.0.
				- added GetTreeItem(.....) 
				- GetSelectedItem(.....)
				- Howto disable Drag/drop in CMySuperGrid
				- level 4 compiled
				- Clean up some redudant code crap

	30.june 99  - fixed minor error in DrawItem, a drawing error occurred when 1.st column was hidden, 
				  it was previous fixed, but some how it was introduced again :).
	
	
	9. juli 99  - CRAIG SCHMIDT "the beachboy":)

				  Craig did the implementation of the Multiple Root request
				  his email are "craig_a_schmidt@yahoo.com" if you have any Questions
				  in regards to multiple roots implementation, although I think I can help too :).

				  he added the following functions :

				  - CTreeItem*  InsertRootItem(.....
				  - void DeleteRootItem(....
				  - BOOL IsRoot(...
				  - CTreeItem* GetRootItem(..
				  - int GetRootIndex(..
				  - int GetRootCount(...
				  - and a member variable, CPtrList m_RootItems which of course are a collection of ..ta da RootItems a.k.a CTreeItem.
				  see the file CMySuperGrid.cpp for usage of the InsertRootItem.

   	10.juli 99    added simple wrappers for the rootitems collection

				  -	Added GetRootHeadPosition(...)
				  -	Added GetRootTailPosition();
				  -	Added CTreeItem* GetNextRoot(...
				  -	Added CTreeItem* GetPrevRoot(...


	12. juli 99  - fixed minor error in CreateDragImageEx, an error occurred when 1st column was hidden and a drag operation was initiated

	13. juli 99	 - Added void DeleteAll()...deletes all items in the grid, uhh
				 - Removed GetNextSiblingItem(...), GetPrevSiblingItem(...) due to support for multiple roots. 
				 - Added CTreeItem *GetNext(...) and CTreeItem* GetPrev(..) due to support for multiple roots.

	14. juli 99	 - Fixed a memory leak in CMySuperGrid::HowToInsertItemsAfterTheGridHasBeenInitialized....


	
	WHAT'S UP :
				- Vacation
				- ATL VERSION COMMING UP.
				- Better documentation :)
				- Peace on earth.
				- service pack 4 for VC++ 6.0 :=|
				- Windows 3000 :)
*/

/////////////////////////////////////////////////////////////////////////////
// CSuperGridCtrl
CSuperGridCtrl::CSuperGridCtrl()
{
	m_cxImage = m_cyImage = m_bIsDragging = m_CurSubItem = 0;
    m_nDragTarget=m_nDragItem = -1;
	m_psTreeLine.CreatePen(PS_SOLID, 1, RGB(192,192,192));
	m_psRectangle.CreatePen(PS_SOLID, 1, RGB(198,198,198));
	m_psPlusMinus.CreatePen(PS_SOLID, 1, RGB(0,0,0));
	m_brushErase.CreateSolidBrush(RGB(255,255,255));
	m_himl = NULL;
	m_nTreeSubItem = 0;
	m_dwFlags = 0;
	m_pSite = NULL;
}


CSuperGridCtrl::~CSuperGridCtrl()
{
	m_psPlusMinus.DeleteObject();
	m_psRectangle.DeleteObject();
	m_psTreeLine.DeleteObject();
	m_brushErase.DeleteObject();

	while(m_RootItems.GetCount())
	{
		CTreeItem * root = (CTreeItem*)m_RootItems.RemoveHead();
		if(root!=NULL && GetData(root) != NULL)
			delete GetData(root);
		delete root;
	}
	m_RootItems.RemoveAll();
}



BEGIN_MESSAGE_MAP(CSuperGridCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CSuperGridCtrl)
	ON_NOTIFY_REFLECT(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
	ON_WM_CREATE()
	ON_WM_HSCROLL()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_TIMER()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONUP()
	ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
	ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM_REFLECT()
	ON_WM_SYSCOLORCHANGE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSuperGridCtrl message handlers

BOOL CSuperGridCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= LVS_REPORT | LVS_SINGLESEL | LVS_SHAREIMAGELISTS | LVS_OWNERDRAWFIXED | LVS_SHOWSELALWAYS;	
	return CListCtrl::PreCreateWindow(cs);
}



int CSuperGridCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CListCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CSuperGridCtrl::SetTreeSubItem(int nTreeSubItem)
{
	m_nTreeSubItem = nTreeSubItem;
	Invalidate();
}

DWORD CSuperGridCtrl::SetFlags(DWORD dwFlags)
{
	DWORD dwPrevious = m_dwFlags;
	m_dwFlags = dwFlags;
	return dwPrevious;
}


#define OFFSET_FIRST	2 
#define OFFSET_OTHER	6
void CSuperGridCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	_TCHAR szBuff[MAX_PATH];
	if (lpDrawItemStruct->CtlType != ODT_LISTVIEW)
        return;
	if(lpDrawItemStruct->itemAction == ODA_DRAWENTIRE)
	{
		if(m_himl==NULL)
		{
			m_himl = (HIMAGELIST)::SendMessage(m_hWnd, LVM_GETIMAGELIST, (WPARAM)(int)(LVSIL_SMALL), 0L);
			if(m_himl==NULL)
				return;
		}
		// Initialize different values for entire item
		int nItem = lpDrawItemStruct->itemID;
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		ASSERT(pDC);
		CTreeItem *pSelItem = (CTreeItem*)lpDrawItemStruct->itemData;
		CItemInfo *pItemInfo = GetData(pSelItem);		
		COLORREF crBackground = GetSysColor(lpDrawItemStruct->itemState&ODS_SELECTED?COLOR_HIGHLIGHT:COLOR_WINDOW);
		COLORREF crText = GetSysColor(lpDrawItemStruct->itemState&ODS_SELECTED?COLOR_HIGHLIGHTTEXT:COLOR_WINDOWTEXT);
		pDC->SetBkColor(crBackground);
		UINT uiIconFlags = ILD_TRANSPARENT;
		if( GetItemState(nItem, LVIF_STATE) & LVIS_DROPHILITED)//if dragging show a SelectDropTarget alike effect :)
			uiIconFlags |= ILD_BLEND50;
		// Obtain item info
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM; 
		lvi.iItem = nItem;
		lvi.iSubItem =0;
		lvi.pszText = szBuff;
		lvi.cchTextMax = sizeof(szBuff);
		lvi.stateMask = 0xFFFF;		
		GetItem(&lvi);
		// Now draw subitems...
		LV_COLUMN lvc;
		lvc.mask = LVCF_FMT | LVCF_WIDTH;
		for(int nColumn=1; GetColumn(nColumn, &lvc); nColumn++)
		{
			// Obtain some values for subitem
			CRect rcs[7];
			GetMultyRects(nItem, nColumn, rcs);
			// Draw column's rectangle
			pDC->FillSolidRect(rcs[LVIR_BOUNDS], crBackground);
			COLORREF clrSIText = pItemInfo->GetSubitemColor(nColumn-1);
			if(clrSIText!=-1)
				pDC->SetTextColor(clrSIText);
			else
				pDC->SetTextColor(crText);
			// Draw tree stuff: node mark, lines, checkbox, icon, ect.
			if (nColumn == m_nTreeSubItem)
			{
				// Draw tree
				if (rcs[LVIR_EX_TREE].Width() > 0)
					DrawTreeItem(pDC, pSelItem, nItem, rcs[LVIR_EX_TREE]);
				// Draw checkbox (if needed)
				UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
				if (nStateImageMask)
				{
					int nImage = (nStateImageMask>>12) - 1;
					CImageList *pImageList = GetImageList(LVSIL_STATE);
					if (pImageList)
						if (rcs[LVIR_SELECTBOUNDS].Width() > 0)
							pImageList->Draw(pDC, nImage, rcs[LVIR_SELECTBOUNDS].TopLeft(), ILD_TRANSPARENT);
				}
			}
			// Draw icon. Icon can exist on plain - tree subitems.
			int iImage = GetData(pSelItem)->GetSubItemImage(nColumn-1);
			if (rcs[LVIR_ICON].Width() > 0 && iImage >= 0)
				ImageList_DrawEx(m_himl, iImage, pDC->m_hDC, rcs[LVIR_ICON].left, rcs[LVIR_ICON].top, 
					rcs[LVIR_ICON].Width(),	rcs[LVIR_ICON].Height(), CLR_DEFAULT, CLR_DEFAULT, uiIconFlags);
			// Draw column's text. Calculate rectangle firstly
			if (rcs[LVIR_LABEL].Width() > 0)
			{
				UINT nJustify=(lvc.fmt&LVCFMT_JUSTIFYMASK)==LVCFMT_RIGHT?DT_RIGHT:
					(lvc.fmt&LVCFMT_JUSTIFYMASK)==LVCFMT_CENTER?DT_CENTER:DT_LEFT;
				int nRetLen = GetItemText(nItem, nColumn, szBuff, sizeof(szBuff));
				LPCTSTR pszText = nRetLen==0?0:MakeShortString(pDC,szBuff,rcs[LVIR_LABEL].Width(),2*OFFSET_OTHER);
				if(pszText!=NULL)
					pDC->DrawText(pszText,-1,rcs[LVIR_LABEL], nJustify|DT_SINGLELINE|DT_NOPREFIX|DT_VCENTER|DT_EXTERNALLEADING);
			}
			//draw down arrow if combobox regardless of rcClipBox rgn
			if (lpDrawItemStruct->itemState & ODS_SELECTED)
				DrawComboBox(pDC, pSelItem, nItem, nColumn);
		}
	}//ODA_DRAWENTIRE
}

BOOL CSuperGridCtrl::GetMultyRects(int nItem, int nSubItem, CRect *pRects)
{
	_TCHAR szBuff[MAX_PATH];
	CTreeItem* pSelItem = GetTreeItem(nItem);
	for (int i = 0; i <= LVIR_EX_TREE_LINE; i++)
		pRects[i] = CRect(0,0,0,0);
	CRect rcSIBounds;
	if (!GetSubItemRect(nItem, nSubItem, LVIR_BOUNDS, rcSIBounds))
		return FALSE;
	pRects[LVIR_BOUNDS] = rcSIBounds;
	int nPosX = rcSIBounds.left;
	if (nSubItem == m_nTreeSubItem)
	{
		LV_ITEM lvi;
		lvi.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_INDENT | LVIF_PARAM; 
		lvi.iItem = nItem;
		lvi.iSubItem =0;
		lvi.pszText = szBuff;
		lvi.cchTextMax = sizeof(szBuff);
		lvi.stateMask = 0xFFFF;		
		GetItem(&lvi);
		// Find tree rectangles
		int nTreeX = GetIndent(pSelItem)*m_cxImage;
		CRect rcTree(nPosX, rcSIBounds.top, min(nPosX+nTreeX,rcSIBounds.right),
			min(rcSIBounds.top+m_cyImage,rcSIBounds.bottom));
		pRects[LVIR_EX_TREE] = rcTree;
		pRects[LVIR_EX_TREE_SIGN] = CRect(rcTree.left, rcTree.top, min(rcTree.left+m_cxImage,rcTree.right),
			min(rcTree.top+m_cyImage,rcTree.bottom));
		pRects[LVIR_EX_TREE_LINE] = CRect(rcTree.left+m_cxImage, rcTree.top, rcTree.right,
			min(rcTree.top+m_cyImage,rcTree.bottom));
		nPosX += nTreeX;
		// Find check rectangle
		UINT nStateImageMask = lvi.state & LVIS_STATEIMAGEMASK;
		if (nStateImageMask)
		{
			int nImage = (nStateImageMask>>12) - 1;
			CImageList *pImageList = GetImageList(LVSIL_STATE);
			if (pImageList)
			{
				int cxIcon,cyIcon=0;
				ImageList_GetIconSize(pImageList->m_hImageList, &cxIcon, &cyIcon);
				pRects[LVIR_SELECTBOUNDS] = CRect(nPosX,rcSIBounds.top,
					min(nPosX+cxIcon,rcSIBounds.right),	min(rcSIBounds.top+cyIcon,rcSIBounds.bottom));
				nPosX += cxIcon;
			}
		}
	}
	// Find icon rectangle
	int iImage = GetData(pSelItem)->GetSubItemImage(nSubItem-1);
	if (iImage >= 0)
	{
		pRects[LVIR_ICON] = CRect(nPosX,rcSIBounds.top, min(nPosX+m_cxImage,rcSIBounds.right),
			min(rcSIBounds.top+m_cyImage,rcSIBounds.bottom));
		nPosX += m_cxImage;
	}
	pRects[LVIR_LABEL] = CRect(nPosX+1, rcSIBounds.top, rcSIBounds.right-1, rcSIBounds.bottom);
	return TRUE;
}

BOOL CSuperGridCtrl::GetSubItemRect(int nItem, int nSubItem, int nArea, CRect &rcRet)
{
	rcRet = CRect(0,0,0,0);
	if (nArea == LVIR_BOUNDS)
	{
		if (nSubItem == 0)
		{
			BOOL b = GetItemRect(nItem, rcRet, LVIR_BOUNDS);
			rcRet.right = rcRet.left+GetColumnWidth(0);
			return b;
		}
		else
			return __super::GetSubItemRect(nItem, nSubItem, nArea, rcRet);
	}
	else if (nSubItem == m_nTreeSubItem)
	{
		CRect rcs[7];
		GetMultyRects(nItem, nSubItem, rcs);
		if (nArea >= LVIR_ICON && nArea <= LVIR_EX_TREE_LINE)
		{
			rcRet = rcs[nArea];
			return rcRet.Width()>0;
		}
		else
			return FALSE;
	}
	else if (nSubItem == 0)
	{
		if (nArea == LVIR_LABEL)
		{
			BOOL b = GetItemRect(nItem, rcRet, LVIR_BOUNDS);
			rcRet.right = rcRet.left+GetColumnWidth(0);
			return b;
		}
		else
			return FALSE;
	}
	else if (nArea == LVIR_BOUNDS || nArea == LVIR_LABEL || nArea == LVIR_ICON) 
		return __super::GetSubItemRect(nItem, nSubItem, nArea, rcRet);
	else
		return FALSE;
}

void CSuperGridCtrl::DoSetCheck(CTreeItem* pItem, int iItem, BOOL bCheck, DWORD dwFlags)
{
	BOOL bHiddenItem = (dwFlags&DoSetCheck_HIDDEN_ITEMS)?TRUE:FALSE;
	BOOL bSkipThis = (dwFlags&DoSetCheck_SKIP_THIS)?TRUE:FALSE;
	BOOL bRecursive = (dwFlags&DoSetCheck_RECURSIVE)?TRUE:FALSE;
	if (!bHiddenItem && !bSkipThis)
		SetCheck(iItem,bCheck);
	CItemInfo *pInfo = GetData(pItem);
	pInfo->SetCheck(bCheck);
	if (m_dwFlags&LVS_EX_EXPAND_CHECK)
	{
		dwFlags = (dwFlags&~DoSetCheck_SKIP_THIS)|DoSetCheck_RECURSIVE;
		if (IsCollapsed(pItem))
			dwFlags |= DoSetCheck_HIDDEN_ITEMS;
		POSITION pos = GetHeadPosition(pItem);
		while(pos)
		{
			CTreeItem* pChild = GetNextChild(pItem, pos);
			DoSetCheck(pChild, pChild->m_nIndex, bCheck, dwFlags);
		}
		if (bCheck && !bRecursive)
		{
			CTreeItem* pParent = pItem;
			do
			{
				pParent = GetParentItem(pParent);
				if (pParent == 0)
					break;
				SetCheck(pParent->m_nIndex,bCheck);
				CItemInfo *pInfo = GetData(pParent);
				pInfo->SetCheck(bCheck);
			}
			while(1);
		}
	}
}


void CSuperGridCtrl::DrawComboBox(CDC* pDC, CTreeItem *pSelItem, int nItem, int nColumn)
{
	CItemInfo* pInfo = GetData(pSelItem);
	CItemInfo::CONTROLTYPE ctrlType;
	if(pInfo->GetControlType(nColumn-1, ctrlType))
	{
		if(ctrlType == pInfo->CONTROLTYPE::combobox) 
		{
			CRect rect;
			GetSubItemRect(nItem, nColumn, LVIR_LABEL, rect);
			rect.left=rect.right - GetSystemMetrics(SM_CYVSCROLL);
			pDC->DrawFrameControl(rect, DFC_SCROLL, DFCS_SCROLLDOWN);
		}
	}
}


//this piece of code is borrowed from the wndproc.c file in the odlistvw.exe example from MSDN and was converted to mfc-style
void CSuperGridCtrl::MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    if (lpMeasureItemStruct->CtlType != ODT_LISTVIEW)
        return;

	TEXTMETRIC tm;
	CClientDC dc(this);	
	CFont* pFont = GetFont();
	CFont* pOldFont = dc.SelectObject(pFont);	
	dc.GetTextMetrics(&tm);
	int nItemHeight = tm.tmHeight + tm.tmExternalLeading;
	lpMeasureItemStruct->itemHeight = nItemHeight + 4; //or should I go for max(nItemheight+4, m_cxImage+2);
	dc.SelectObject(pOldFont);
}



//the basic rutine making the ... thing snatched it from some tedious code example some where in MSDN call odlistvw.exe
LPCTSTR CSuperGridCtrl::MakeShortString(CDC* pDC, LPCTSTR lpszLong, int nColumnLen, int nOffset)
{
	static const _TCHAR szThreeDots[]=_T("...");

	int nStringLen=lstrlen(lpszLong);

	if(nStringLen==0 || pDC->GetTextExtent(lpszLong,nStringLen).cx + nOffset < nColumnLen)
		return(lpszLong);

	static _TCHAR szShort[MAX_PATH];

	lstrcpy(szShort,lpszLong);
	int nAddLen = pDC->GetTextExtent(szThreeDots,sizeof(szThreeDots)).cx;

	for(int i=nStringLen-1; i > 0; i--)
	{
		szShort[i]=0;
		if(pDC->GetTextExtent(szShort,i).cx + nOffset + nAddLen < nColumnLen)
			break;
	}
	lstrcat(szShort,szThreeDots);
	return(szShort);
}




void CSuperGridCtrl::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_KEYDOWN* pLVKeyDow = (LV_KEYDOWN*)pNMHDR;
	switch(pLVKeyDow->wVKey)
	{
		case VK_SPACE: 
			{
				if(GetExtendedStyle() & LVS_EX_CHECKBOXES)
				{	
					int nIndex = GetSelectedItem();
					if(nIndex !=-1)
					{
						CTreeItem* pItem = GetTreeItem(nIndex);
						if(pItem!=NULL)
						{
							CItemInfo *pInfo = GetData(pItem);
							DoSetCheck(pItem, pItem->m_nIndex, !GetCheck(pItem->m_nIndex),
								DoSetCheck_SKIP_THIS);
						}
					}
				}
			}break;

		case VK_DELETE: 
			{
				int nItem = GetSelectedItem();
				if(nItem!=-1)
				{
					CTreeItem* pSelItem = GetTreeItem(nItem);
					if(pSelItem != NULL)
					{
						if(OnDeleteItem(pSelItem, nItem))
							DeleteItemEx(pSelItem, nItem);
					}	
				}
			}	break;


		case VK_MULTIPLY:
			{  
				int nIndex = GetSelectedItem();
				if(nIndex != -1)
				{
					CWaitCursor wait;
					SetRedraw(0);
					CTreeItem *pParent = GetTreeItem(nIndex);
					int nScroll=0;
					if(OnVKMultiply(pParent, nIndex))
					{	
						ExpandAll(pParent, nScroll);
					}		
					SetRedraw(1);
					RedrawItems(nIndex, nScroll);
					EnsureVisible(nScroll, TRUE);
				 }
			 }break;

		case VK_ADD:
			{
					int nIndex = GetSelectedItem();
					if(nIndex!=-1)
					{
						CWaitCursor wait;
						CTreeItem *pSelItem = GetTreeItem(nIndex);
						int nScrollIndex = 0;
						if(OnVKAdd(pSelItem, nIndex))
						{
							 nScrollIndex = Expand(pSelItem, nIndex);
						}
						CRect rc;
						GetItemRect(nIndex, rc, LVIR_BOUNDS);
						InvalidateRect(rc);
						UpdateWindow();
						EnsureVisible(nScrollIndex, 1);
					}

			}break;


		case VK_SUBTRACT:
			{
				int nIndex = GetSelectedItem();
				if(nIndex!=-1)
				{
					CWaitCursor wait;
					CTreeItem *pSelItem = GetTreeItem(nIndex);
					if(OnVkSubTract(pSelItem, nIndex))
					{
						Collapse(pSelItem);
					}
					CRect rc;
					GetItemRect(nIndex, rc, LVIR_BOUNDS);
					InvalidateRect(rc);
					UpdateWindow();
				}
			}break;
		default :break;
	}
	*pResult = 0;
}




BOOL CSuperGridCtrl::HitTestOnSign(CPoint point, LVHITTESTINFO& ht)
{
	ht.pt = point;
	// Test which subitem was clicked.
	SubItemHitTest(&ht);
	if(ht.iItem!=-1)
	{
		//first hittest on checkbox
		BOOL bHit = FALSE;
		if(GetExtendedStyle() & LVS_EX_CHECKBOXES)
		{
			if ((ht.flags & LVHT_ONITEM) && (GetStyle() & LVS_OWNERDRAWFIXED))//isn't this allways ownerdrawfixed :-)
			{
				CRect rcSelectBound;
				if (GetSubItemRect(ht.iItem, ht.iSubItem, LVIR_SELECTBOUNDS, rcSelectBound))
				{
					// check if hit was on a state icon 
					if (point.x > rcSelectBound.left && point.x < rcSelectBound.right)
						bHit = TRUE;
				}
			}
			else if (ht.flags & LVHT_ONITEMSTATEICON)
				bHit = TRUE;
		}

		CTreeItem* pItem = GetTreeItem(ht.iItem);
		if(pItem!=NULL)
		{
			if(bHit)//if checkbox
			{
				//yes I know..have to maintain to sets of checkstates here...
				//one for listview statemask and one for CTreeItem..but its located here so no harm done
				DoSetCheck(pItem, ht.iItem,!GetCheck(ht.iItem),0);
				if (m_pSite)
					m_pSite->OnCheckStateChanged(pItem->m_lpNodeInfo);
			}
			//if haschildren and clicked on + or - then expand/collapse
			if(ItemHasChildren(pItem))
			{
				//hittest on the plus/sign "button" 
				CRect rcTreeSign;
				GetSubItemRect(ht.iItem, ht.iSubItem, LVIR_EX_TREE_SIGN, rcTreeSign);
				CRectangle rect(this, NULL, GetIndent(pItem), rcTreeSign);
				BOOL bRedraw=0;//if OnItemExpanding or OnCollapsing returns false, dont redraw
				if(rect.HitTest(point))
				{
					SetRedraw(0);
					int nScrollIndex=0;
					if(IsCollapsed(pItem))
					{	
						if(OnItemExpanding(pItem, ht.iItem))
						{
							nScrollIndex = Expand(pItem, ht.iItem);
							OnItemExpanded(pItem, ht.iItem);
							bRedraw=1;
						}
					}	
					else {
					   if(OnCollapsing(pItem))
					   {
							Collapse(pItem);
							OnItemCollapsed(pItem);
							bRedraw=1;
					   }
					}
					SetRedraw(1);
					if(bRedraw)
					{
						CRect rc;
						GetItemRect(ht.iItem, rc, LVIR_BOUNDS);
						InvalidateRect(rc);
						UpdateWindow();
						EnsureVisible(nScrollIndex, 1);
						return 0;
					}	
				}
			}//has kids
		}//pItem!=NULL
	}
	return 1;
}



void CSuperGridCtrl::OnDblclk(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if( GetFocus() != this) 
		SetFocus();

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	LVHITTESTINFO ht;
	ht.pt = pNMListView->ptAction;
	SubItemHitTest(&ht);
	if(OnItemLButtonDown(ht))
	{
		BOOL bSelect=1;
		bSelect = HitTestOnSign(pNMListView->ptAction, ht);
		//normal selection
		if(bSelect && ht.iItem !=-1)
		{		
			int nIndex = GetSelectedItem();
			if(nIndex!=-1)
			{
				CTreeItem *pSelItem = GetTreeItem(nIndex);
				if (pSelItem != NULL)
				{
					BOOL bRedraw=0;
					if(ItemHasChildren(pSelItem))
					{
						SetRedraw(0);
						int nScrollIndex=0;
						if(IsCollapsed(pSelItem))
						{		
							if(OnItemExpanding(pSelItem, nIndex))
							{
								nScrollIndex = Expand(pSelItem, nIndex);
								OnItemExpanded(pSelItem, nIndex);
								bRedraw=1;
							}
						}	
					
						else 
						{
						   if(OnCollapsing(pSelItem))
						   {
								Collapse(pSelItem);
								OnItemCollapsed(pSelItem);
								bRedraw=1;
						   }
						}
						SetRedraw(1);

						if(bRedraw)
						{
							CRect rc;
							GetItemRect(nIndex,rc,LVIR_BOUNDS);
							InvalidateRect(rc);
							UpdateWindow();
							EnsureVisible(nScrollIndex,1);
						}
					}//ItemHasChildren	
				}//!=NULL
			}
		}
	}
	*pResult = 0;
}



void CSuperGridCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		m_nDragItem = pNMListView->iItem;
		CImageList* pDragImage=NULL;
		pDragImage = CreateDragImageEx(m_nDragItem);//override this if you want another dragimage or none at all.
		if(pDragImage)
		{
			pDragImage->BeginDrag(0, CPoint(0,0));
			pDragImage->DragEnter(this, pNMListView->ptAction);
			SetCapture();
			m_bIsDragging = TRUE;
		}
		delete pDragImage;
	}
	*pResult = 0;
}




//Create dragimage : Icon + the itemtext
CImageList *CSuperGridCtrl::CreateDragImageEx(int nItem)
{
    CImageList *pList = new CImageList;          
	//get image index
	LV_ITEM lvItem;
	lvItem.mask =  LVIF_IMAGE;
	lvItem.iItem = nItem;
	lvItem.iSubItem = 0;
	GetItem(&lvItem);

	CRect rc;
	GetItemRect(nItem, &rc, LVIR_BOUNDS);         

	CString str;
	str = GetItemText(nItem, 0);
	CFont *pFont = GetFont();

	rc.OffsetRect(-rc.left, -rc.top);            
	rc.right = GetColumnWidth(0);                
	pList->Create(rc.Width(), rc.Height(),ILC_COLOR24| ILC_MASK , 1, 1);
	CDC *pDC = GetDC();                          
	if(pDC) 
	{
		CDC dc;	      
		dc.CreateCompatibleDC(pDC);      
		CBitmap bmpMap;
		bmpMap.CreateCompatibleBitmap(pDC, rc.Width(), rc.Height());

		CBitmap *pOldBmp = dc.SelectObject(&bmpMap);
		CFont *pOldFont = dc.SelectObject(pFont);
		dc.FillSolidRect(rc, GetSysColor(COLOR_WINDOW));
		CImageList *pImgList = GetImageList(LVSIL_SMALL);
		if(pImgList)
			pImgList->Draw(&dc, lvItem.iImage, CPoint(0,0), ILD_TRANSPARENT);
		dc.TextOut(m_cxImage + 4, 0, str);
		dc.SelectObject(pOldFont);
		dc.SelectObject(pOldBmp);                 
		//causes an error if the 1st column is hidden so must check the imagelist
		if(pList->m_hImageList != NULL)
			pList->Add(&bmpMap, RGB(255,255,255));
		else { 
			delete pList;
			pList=NULL;
		}
		ReleaseDC(pDC);   
	}   
	return pList;
}



void CSuperGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
    if(m_bIsDragging)
    {
		
		KillTimer(1);
		if (CWnd::GetCapture() != this)
			m_bIsDragging=0;
		
		if(nFlags==MK_RBUTTON || nFlags==MK_MBUTTON)
			m_bIsDragging=0;

		
		if(GetKeyState(VK_ESCAPE) < 0)		
			m_bIsDragging=0;
		
		if(!m_bIsDragging)//why not put this in a funtion :)
		{
			SetItemState (m_nDragTarget, 0, LVIS_DROPHILITED);
			CImageList::DragLeave(this);
			CImageList::EndDrag();
			ReleaseCapture();
			InvalidateRect(NULL);
			UpdateWindow();
		}
		else
		{
			CPoint ptList(point);
			MapWindowPoints(this, &ptList, 1);
			CImageList::DragMove(ptList);
			UINT uHitTest = LVHT_ONITEM;
			m_nDragTarget = HitTest(ptList, &uHitTest);
			// try turn off hilight for previous DROPHILITED state
			int nPrev = GetNextItem(-1,LVNI_DROPHILITED);
			if(nPrev != m_nDragTarget)//prevents flicker 
				SetItemState(nPrev, 0, LVIS_DROPHILITED);

			CRect rect;
			GetClientRect (rect);
			int cy = rect.Height();
			if(m_nDragTarget!=-1)
			{
				SetItemState(m_nDragTarget, LVIS_DROPHILITED, LVIS_DROPHILITED);
				CTreeItem* pTarget = GetTreeItem(m_nDragTarget);
				if ((point.y >= 0 && point.y <= m_cyImage) || (point.y >= cy - m_cyImage && point.y <= cy) || 	
					( pTarget!=NULL && ItemHasChildren(pTarget) && IsCollapsed(pTarget)))
				{
					SetTimer(1,300,NULL);
				}
			}
		}
    }
	CListCtrl::OnMouseMove(nFlags, point);
}




void CSuperGridCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	CListCtrl::OnTimer(nIDEvent);
	if(nIDEvent==1)
	{
		if(CWnd::GetCapture()!=this)
		{
			SetItemState(m_nDragTarget, 0, LVIS_DROPHILITED);
			m_bIsDragging=0;
			CImageList::DragLeave(this);
			CImageList::EndDrag();
			ReleaseCapture();
			InvalidateRect(NULL);
			UpdateWindow();
			KillTimer(1);
			return;
		}

		SetTimer(1,300,NULL);//reset timer
		DWORD dwPos = ::GetMessagePos();
		CPoint ptList(LOWORD(dwPos),HIWORD(dwPos));
		ScreenToClient(&ptList);

		CRect rect;
		GetClientRect(rect);
		int cy = rect.Height();
		//
		// perform autoscroll if the cursor is near the top or bottom.
		//
		if (ptList.y >= 0 && ptList.y <= m_cyImage) 
		{
			int n = GetTopIndex(); 
			CImageList::DragShowNolock(0);
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), NULL);
			CImageList::DragShowNolock(1);
			if (GetTopIndex()== n)
				KillTimer (1);
			else {
				CImageList::DragShowNolock(0);
				CImageList::DragMove(ptList);
				CImageList::DragShowNolock(1);
				return;
			}
		}
		else if (ptList.y >= cy - m_cyImage && ptList.y <= cy) 
		{
			int n = GetTopIndex(); 
			CImageList::DragShowNolock(0);
			SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEDOWN, 0), NULL);
			CImageList::DragShowNolock(1);
			if (GetTopIndex()== n)
				KillTimer (1);
			else {
				CImageList::DragShowNolock(0);
				CImageList::DragMove(ptList);
				CImageList::DragShowNolock(1);
				return;
			}
		}
		//Hover test 
		CImageList::DragMove(ptList);
		UINT uHitTest = LVHT_ONITEM;
		m_nDragTarget = HitTest(ptList, &uHitTest);
	
		if(m_nDragTarget!=-1)
		{
			//if the target has children
			//expand them
			CTreeItem* pTarget=GetTreeItem(m_nDragTarget);
			if(pTarget != NULL && ItemHasChildren(pTarget) && IsCollapsed(pTarget) && (m_nDragItem!=-1))
			{
				CImageList::DragShowNolock(0);
				CTreeItem* pSource = GetTreeItem(m_nDragItem);
	
				SetRedraw(0);
				int nScrollIndex=0;
				if(ItemHasChildren(pTarget) && IsCollapsed(pTarget))
				{	
					if(OnItemExpanding(pTarget, m_nDragTarget))
					{	
						nScrollIndex = Expand(pTarget, m_nDragTarget);
						OnItemExpanded(pTarget, m_nDragTarget);
					}
				}		
				m_nDragItem = NodeToIndex(pSource);
				SetRedraw(1);
				EnsureVisible(nScrollIndex, 1);
				InvalidateRect(NULL);
				UpdateWindow();
				CImageList::DragShowNolock(1);
				KillTimer(1);
				return;
			}	
		}
		KillTimer(1);
	}
}



void CSuperGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_bIsDragging == TRUE)
    {
		KillTimer(1);
        CImageList::DragLeave(this);
        CImageList::EndDrag();
        ReleaseCapture();
        m_bIsDragging = FALSE;
		SetItemState(m_nDragTarget, 0, LVIS_DROPHILITED);
        if((m_nDragTarget != -1) && (m_nDragTarget != m_nDragItem) && (m_nDragItem!=-1))//no drop on me self
        {
		
			CTreeItem* pSource = GetTreeItem(m_nDragItem);
			CTreeItem* pTarget = GetTreeItem(m_nDragTarget);
			if(IsRoot(pSource))
				return;
			CTreeItem* pParent = GetParentItem(pSource);
			if(pParent==pTarget) //can't drag child to parent
				return;

			if(!IsChildOf(pSource, pTarget))//can't drag parent to child
			{
				CWaitCursor wait;
				SetRedraw(0);
				if(DoDragDrop(pTarget, pSource))
				{
					UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
					SetItemState(m_nDragTarget, uflag, uflag);
					m_nDragItem=-1;
					//delete source
					int nIndex = NodeToIndex(pSource);			
					DeleteItem(nIndex);
					HideChildren(pSource, TRUE, nIndex);
					Delete(pSource);
					InternaleUpdateTree();
					SetRedraw(1);
					InvalidateRect(NULL);
					UpdateWindow();
				}else
					SetRedraw(1);
			}
	    }
    }
    else
		CListCtrl::OnLButtonUp(nFlags, point);
}



//used with the drag/drop operation
void CSuperGridCtrl::CopyChildren(CTreeItem* pDest, CTreeItem* pSrc)
{
	if (ItemHasChildren(pSrc))
	{
		POSITION pos = pSrc->m_listChild.GetHeadPosition();
		while (pos != NULL)
		{
			CTreeItem* pItem = (CTreeItem *)pSrc->m_listChild.GetNext(pos);
			CItemInfo* lp = CopyData(GetData(pItem));
			CTreeItem* pNewItem = InsertItem(pDest, lp);
			CopyChildren(pNewItem, pItem);
		}
	}
}


//hmmm 
BOOL CSuperGridCtrl::DoDragDrop(CTreeItem* pTarget, CTreeItem* pSource)
{
	if(pTarget==NULL)
		return 0;

	BOOL bUpdate=FALSE;
	if(!IsCollapsed(pTarget))
		bUpdate=TRUE; //children are expanded, want to see update right away
	
	//make a copy of the source data
	CItemInfo* lp = CopyData(GetData(pSource));
	//create new node with the source data and make pTarget the parent
	
	CTreeItem* pNewParent = InsertItem(pTarget, lp, bUpdate);
	//if the source has children copy all source data and make the newly create item the parent
	if(ItemHasChildren(pSource))
		CopyChildren(pNewParent, pSource);

	return 1;
}


void CSuperGridCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	//its not meself
	if( GetFocus() != this) 
		SetFocus();
	
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}




void CSuperGridCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( GetFocus() != this) 
		SetFocus();
	CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
}




BOOL CSuperGridCtrl::PreTranslateMessage(MSG* pMsg) 
{
	if(pMsg->message == WM_KEYDOWN)
	{
		if(GetFocus()==this)
		{
			switch( pMsg->wParam )
			{
				case VK_LEFT:
					{
						// Decrement the order number.
						m_CurSubItem--;
						if(m_CurSubItem < 0) 
							m_CurSubItem = 0;
						else{
							
							CHeaderCtrl* pHeader = GetHeaderCtrl();
							// Make the column visible.
							// We have to take into account that the header may be reordered.
							MakeColumnVisible( Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem));
							// Invalidate the item.
							int iItem = GetSelectedItem();
							if( iItem != -1 )
							{
								CRect rcBounds;
								GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
								InvalidateRect(&rcBounds);
								UpdateWindow();
							}
						}
					}
					return TRUE;

				case VK_RIGHT:
					{
						// Increment the order number.
						m_CurSubItem++;
						CHeaderCtrl* pHeader = GetHeaderCtrl();
						int nColumnCount = pHeader->GetItemCount();
						// Don't go beyond the last column.
						if( m_CurSubItem > nColumnCount -1 ) 
							m_CurSubItem = nColumnCount-1;
						else
						{
							MakeColumnVisible(Header_OrderToIndex( pHeader->m_hWnd, m_CurSubItem));
							 
							int iItem = GetSelectedItem();
							// Invalidate the item.
							if( iItem != -1 )
							{
								CRect rcBounds;
								GetItemRect(iItem, rcBounds, LVIR_BOUNDS);
								InvalidateRect(&rcBounds);
								UpdateWindow();
							}
						}
					}
					return TRUE;

				case VK_RETURN://edit itemdata
					{
						BOOL bResult = OnVkReturn();
						if(!bResult)
						{
							int iItem = GetSelectedItem();
							if( m_CurSubItem != -1 && iItem != -1)
							{
								CHeaderCtrl* pHeader = GetHeaderCtrl();
								int iSubItem = Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem);
								if(iSubItem==0)//that's just me saying all nodes in col 0 are edit-controls, you may modify this
								{
									CRect rcItem;
									GetItemRect(iItem, rcItem, LVIR_LABEL);
									DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL|ES_LEFT;
									CEdit *pEdit = new CListEditCtrl(iItem, iSubItem, GetItemText(iItem, iSubItem));
									pEdit->Create(dwStyle, rcItem, this, 0x1233);	
								}
								else
									EditLabelEx(iItem, iSubItem);	
								return 1;
							}
						}
					}
					break;
				default:
					break;
			}
		}
	}
	return CListCtrl::PreTranslateMessage(pMsg);
}



#define IDC_EDITCTRL 0x1234
CEdit* CSuperGridCtrl::EditLabelEx(int nItem, int nCol)
{
	CRect rect;
	int offset = 0;
	if(!EnsureVisible(nItem, TRUE)) 
		return NULL;
	GetSubItemRect(nItem, nCol, LVIR_LABEL, rect);
	// Now scroll if we need to expose the column
	CRect rcClient;
	GetClientRect(rcClient);
	if( offset + rect.left < 0 || offset + rect.left > rcClient.right )
	{
		CSize size(offset + rect.left,0);		
		Scroll(size);
		rect.left -= size.cx;
	}
	rect.left += offset;	
	rect.right += offset;
	if(rect.right > rcClient.right) 
	   rect.right = rcClient.right;

	// Get Column alignment	
	LV_COLUMN lvcol;
	lvcol.mask = LVCF_FMT;
	GetColumn(nCol, &lvcol);

	DWORD dwStyle;
	if((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_LEFT)
		dwStyle = ES_LEFT;
	else if((lvcol.fmt & LVCFMT_JUSTIFYMASK) == LVCFMT_RIGHT)
		dwStyle = ES_RIGHT;
	else 
		dwStyle = ES_CENTER;	

	
	dwStyle |=WS_BORDER|WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL;
	CEdit *pEdit = new CListEditCtrl(nItem, nCol, GetItemText(nItem, nCol));
	pEdit->Create(dwStyle, rect, this, IDC_EDITCTRL);	
	//pEdit->ModifyStyleEx(0,WS_EX_CLIENTEDGE); //funny thing happend here, uncomment this, 
												//and then edit an item, 
												//enter a long text so that the ES_AUTOHSCROLL comes to rescue
												//yes that's look funny, ???.
	return pEdit;
}




void CSuperGridCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO *plvDispInfo = (LV_DISPINFO*)pNMHDR;
 	LV_ITEM *plvItem = &plvDispInfo->item;
	if (plvItem->pszText != NULL)//valid text
	{
		if(plvItem->iItem != -1) //valid item
		{
			CTreeItem*pSelItem = GetTreeItem(plvItem->iItem);
			if(pSelItem != NULL)
			{
				OnUpdateListViewItem(pSelItem, plvItem);
			}
		}
	}
	*pResult = 0;
}



int CSuperGridCtrl::GetNumCol()
{
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	return pHeader ? pHeader->GetItemCount() : 0;
}



//Think Rex Myer is spooking here
void CSuperGridCtrl::MakeColumnVisible(int nCol)
{
	if(nCol < 0)
		return;
	// Get the order array to total the column offset.
	CHeaderCtrl* pHeader = GetHeaderCtrl();

	int nColCount = pHeader->GetItemCount();
	ASSERT( nCol < nColCount);
	int *pOrderarray = new int[nColCount];
	Header_GetOrderArray(pHeader->m_hWnd, nColCount, pOrderarray);
	// Get the column offset
	int offset = 0;
	for(int i = 0; pOrderarray[i] != nCol; i++)
		offset += GetColumnWidth(pOrderarray[i]);

	int colwidth = GetColumnWidth(nCol);
	delete[] pOrderarray;

	CRect rect;
	GetItemRect(0, &rect, LVIR_BOUNDS);
	// Now scroll if we need to show the column
	CRect rcClient;
	GetClientRect(&rcClient);
	if(offset + rect.left < 0 || offset + colwidth + rect.left > rcClient.right)
	{
		CSize size(offset + rect.left,0);
		Scroll(size);
		InvalidateRect(NULL);
		UpdateWindow();
	}
}



//Think Rex Myer is spooking here
int CSuperGridCtrl::IndexToOrder( int iIndex )
{
	// This translates a column index value to a column order value.
	CHeaderCtrl* pHeader = GetHeaderCtrl();
	int nColCount = pHeader->GetItemCount();
	int *pOrderarray = new int[nColCount];
	Header_GetOrderArray(pHeader->m_hWnd, nColCount, pOrderarray);
	for(int i=0; i<nColCount; i++)
	{
		if(pOrderarray[i] == iIndex )
		{
			delete[] pOrderarray;
			return i;
		}
	}
	delete[] pOrderarray;
	return -1;
}



void CSuperGridCtrl::DrawFocusCell(CDC *pDC, int nItem, int iSubItem)
{
	if(iSubItem==m_CurSubItem)
	{
		CRect rect;
		GetSubItemRect(nItem, iSubItem, LVIR_BOUNDS, rect);
		CBrush br(GetCellRGB());
		if(iSubItem==0)
			GetItemRect(iSubItem, rect, LVIR_LABEL);
		pDC->FillRect(rect, &br);
		pDC->DrawFocusRect(rect);
	}
}




//insert item and return new parent pointer.
CSuperGridCtrl::CTreeItem* CSuperGridCtrl::InsertItem(CTreeItem *pParent, CItemInfo* lpInfo,  BOOL bUpdate)
{
	if(pParent==NULL)
		return NULL;

	CTreeItem *pItem = NULL;
	pItem =  new CTreeItem();

	if(lpInfo==NULL)
		lpInfo = new CItemInfo;

	UpdateData(pItem, lpInfo);
	SetIndent(pItem, GetIndent(pParent)+1);
	SetParentItem(pItem, pParent);
	//add as the last child 
	pParent->m_listChild.AddTail(pItem);

	if(!bUpdate)
		 Hide(pParent, TRUE);	
	else
	{
		//calc listview index for the new node
		int nIndex = NodeToIndex(pItem);			
		CString str = GetData(pItem)->GetItemText();
		LV_ITEM     lvItem;
		lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM;
		lvItem.pszText = str.GetBuffer(1); 
		//insert item
		lvItem.iItem = nIndex;
		lvItem.iSubItem = 0;
		lvItem.lParam = (LPARAM)pItem;
		lvItem.iIndent = GetIndent(pItem);
		CListCtrl::InsertItem(&lvItem);
		if(lpInfo->GetCheck())
			SetCheck(nIndex);
		//Get subitems
		int nSize = GetData(pItem)->GetItemCount();
		for(int i=0; i < nSize;i++)
		{
		   CString str = GetData(pItem)->GetSubItem(i);
		   lvItem.iImage = GetData(pItem)->GetSubItemImage(i);
		   lvItem.mask = lvItem.iImage>=0?LVIF_TEXT|LVIF_IMAGE:LVIF_TEXT;
		   lvItem.iSubItem = i+1;
		   lvItem.pszText = str.GetBuffer(1);
		   SetItem(&lvItem);
		}
		InternaleUpdateTree();//better do this
	}
	return pItem;
}	



void CSuperGridCtrl::InternaleUpdateTree()
{
	int nItems = GetItemCount();
	for(int nItem=0; nItem < nItems; nItem++)
	{
		CTreeItem* pItem = GetTreeItem(nItem);
		SetCurIndex(pItem, nItem);
	}
}


int CSuperGridCtrl::NodeToIndex(CTreeItem *pNode)
{
	int nStartIndex=0;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * root = (CTreeItem*)m_RootItems.GetNext(pos);
		int ret = _NodeToIndex(root, pNode, nStartIndex);
		if(ret != -1)
			return ret;
	}
	return -1;
}




CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetRootItem(int nIndex)
{
	POSITION pos = m_RootItems.FindIndex(nIndex);
	if(pos==NULL)
		return NULL;
	return (CTreeItem*)m_RootItems.GetAt(pos);
}



int CSuperGridCtrl::GetRootIndex(CTreeItem * root)
{
	int nIndex = 0;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos != NULL)
	{
		CTreeItem * pItem = (CTreeItem*)m_RootItems.GetNext(pos);
		if(pItem== root)
			return nIndex;
		nIndex++;
	}
	return -1;
}



BOOL CSuperGridCtrl::IsRoot(CTreeItem * lpItem)
{
	return m_RootItems.Find(lpItem) != NULL;
}


void CSuperGridCtrl::DeleteRootItem(CTreeItem * root)
{
	POSITION pos = m_RootItems.Find(root);
	if(pos!=NULL)
	{
		CTreeItem* pRoot=(CTreeItem*)m_RootItems.GetAt(pos);
		if(pRoot->m_lpNodeInfo!=NULL)
				delete pRoot->m_lpNodeInfo;
		delete pRoot;
		m_RootItems.RemoveAt(pos);
	}
}



CSuperGridCtrl::CTreeItem*  CSuperGridCtrl::InsertRootItem(CItemInfo * lpInfo)
{
	if(lpInfo==NULL)
		lpInfo = new CItemInfo;

	CTreeItem* pRoot = NULL;
	
	pRoot =  new CTreeItem();

	CleanMe(pRoot);
	UpdateData(pRoot, lpInfo);
	SetIndent(pRoot, 1);
	SetCurIndex(pRoot, GetItemCount());
	SetParentItem(pRoot, NULL);

	CItemInfo* lp = GetData(pRoot);
	LV_ITEM lvItem;		
	lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM;
	CString strItem = lp->GetItemText();
	lvItem.pszText = strItem.GetBuffer(1); 
	lvItem.iItem = GetItemCount();
	lvItem.lParam = (LPARAM)pRoot;
	lvItem.iIndent = 1;
	lvItem.iSubItem = 0;
	CListCtrl::InsertItem(&lvItem);
	if(lpInfo->GetCheck())
		SetCheck(lvItem.iItem);
	int nSize = lp->GetItemCount();
	for(int i=0; i < nSize;i++)
	{
		CString str = lp->GetSubItem(i);
		lvItem.mask = LVIF_TEXT;
		lvItem.iSubItem = i+1;
		lvItem.pszText = str.GetBuffer(1);
		SetItem(&lvItem);
	}
	m_RootItems.AddTail(pRoot);
	return pRoot;
}



void CSuperGridCtrl::DrawTreeItem(CDC* pDC, CTreeItem* pSelItem, int nListItem, const CRect& rcBounds)
{
	int nColWidth = GetColumnWidth(m_nTreeSubItem);
	int yDown = rcBounds.top;
    CPen* pPenTreeLine = pDC->SelectObject(&m_psTreeLine);
	int iIndent = GetIndent(pSelItem);
	int nHalfImage = (m_cxImage >> 1);
	int nBottomDown = yDown + nHalfImage + ((rcBounds.Height() - m_cyImage) >> 1);
	//
	BOOL bChild = ItemHasChildren(pSelItem);
	BOOL bCollapsed = IsCollapsed(pSelItem);
	//draw outline	
	while(1)
	{
		CTreeItem* pParent = GetParentItem(pSelItem);
		if(pParent==NULL)//no more parents, stop
			break;

		POSITION pos = pParent->m_listChild.GetTailPosition();
		while(pos!=NULL)
		{
			CTreeItem *pLastChild = (CTreeItem*)pParent->m_listChild.GetPrev(pos);
			int nIndex = GetCurIndex(pLastChild);
			int nCurIndent = GetIndent(pLastChild);
			if(nListItem > nIndex && iIndent > nCurIndent)//no need to go further in this loop
				break;

			//no drawing outside the 1st columns right
			// Draw last item!
			int xLine =  rcBounds.left + nCurIndent * m_cxImage - nHalfImage;
			if(nIndex == nListItem && nCurIndent==iIndent)
			{
				//draw '-
				int x;
				pDC->MoveTo(xLine, yDown);
				pDC->LineTo(xLine, nBottomDown);
				// -
				xLine + nHalfImage > nColWidth ? x = nColWidth: x = xLine + nHalfImage;
				
				pDC->MoveTo(xLine, nBottomDown);
				pDC->LineTo(x, nBottomDown);
				break;
			}
			else
			if(nIndex > nListItem && nCurIndent==iIndent)
			{
				//draw |-
				int x;
				xLine + nHalfImage > nColWidth ? x = nColWidth : x = xLine + nHalfImage;
				pDC->MoveTo(xLine, nBottomDown);
				pDC->LineTo(x, nBottomDown);
				//-
				pDC->MoveTo(xLine, yDown);
				pDC->LineTo(xLine, rcBounds.bottom);
				break;
			}
			else
			if(nIndex > nListItem && nCurIndent < iIndent)
			{
				//draw |
				pDC->MoveTo(xLine, yDown);
				pDC->LineTo(xLine, rcBounds.bottom);
				break;
			}
		}			
		pSelItem = pParent;//next
	}

	//draw plus/minus sign
	if(bChild)
	{
		CRectangle rect(this, pDC, iIndent, rcBounds);

		rect.DrawRectangle(this);

		CPen* pPenPlusMinus = pDC->SelectObject(&m_psPlusMinus);
		if(bCollapsed)
			rect.DrawPlus();
		else {
			rect.DrawMinus();
			//draw line up to parent folder
			CPen* pLine = pDC->SelectObject(&m_psTreeLine);
			int nOffset = (rcBounds.Height() - m_cyImage)/2;
			pDC->MoveTo(rect.GetLeft()+ m_cxImage, rcBounds.top + m_cyImage+nOffset);
			pDC->LineTo(rect.GetLeft() + m_cxImage, rcBounds.bottom);
			pDC->SelectObject(pLine);		
		}
		pDC->SelectObject(pPenPlusMinus);		
	}
	pDC->SelectObject(pPenTreeLine);
}



//walk all over the place setting the hide/show flag of the nodes.
//it also deletes items from the listviewctrl.
void CSuperGridCtrl::HideChildren(CTreeItem *pItem, BOOL bHide,int nItem)
{
	if(!IsCollapsed(pItem))
	if(ItemHasChildren(pItem))
	{
		Hide(pItem, bHide);
		POSITION pos = pItem->m_listChild.GetHeadPosition();
		while (pos != NULL)
		{
			HideChildren((CTreeItem *)pItem->m_listChild.GetNext(pos),bHide,nItem+1);
			DeleteItem(nItem);

		}
	}
}




void CSuperGridCtrl::Collapse(CTreeItem *pItem)
{
	if(pItem != NULL && ItemHasChildren(pItem))
	{
		SetRedraw(0);
		int nIndex = NodeToIndex(pItem);			
		HideChildren(pItem, TRUE, nIndex+1);
		InternaleUpdateTree();
		SetRedraw(1);
	}
}


void CSuperGridCtrl::ExpandAll(CTreeItem *pItem, int& nScroll)
{
	const int nChildren = pItem->m_listChild.GetCount();
	if (nChildren > 0)
	{
		int nIndex = NodeToIndex(pItem);
		nScroll = Expand(pItem, nIndex);
	}

	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos)
	{
		CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		ExpandAll(pChild, nScroll);
	}
	
}



int CSuperGridCtrl::Expand(CTreeItem* pSelItem, int nIndex)
{
	if(ItemHasChildren(pSelItem) && IsCollapsed(pSelItem))
	{
		LV_ITEM lvItem;
		lvItem.mask = LVIF_INDENT;
		lvItem.iItem = nIndex;
		lvItem.iSubItem = 0;
		lvItem.lParam=(LPARAM)pSelItem;
		lvItem.iIndent = GetIndent(pSelItem);
		SetItem(&lvItem);

		Hide(pSelItem, FALSE);
		//expand children
		POSITION pos = pSelItem->m_listChild.GetHeadPosition();
		while(pos != NULL)
		{
			CTreeItem* pNextNode = (CTreeItem*)pSelItem->m_listChild.GetNext(pos);
			CString str = GetData(pNextNode)->GetItemText();
			LV_ITEM lvItem;
			lvItem.mask = LVIF_TEXT | LVIF_INDENT | LVIF_PARAM;
			lvItem.pszText =str.GetBuffer(1); 
			lvItem.iItem = nIndex + 1;
			lvItem.iSubItem = 0;
			lvItem.lParam=(LPARAM)pNextNode;
			lvItem.iIndent = GetIndent(pSelItem)+1;
			CListCtrl::InsertItem(&lvItem);
			if(GetData(pNextNode)->GetCheck())
				SetCheck(nIndex + 1);
			//get subitems
			int nSize = GetData(pNextNode)->GetItemCount();
			for(int i=0; i< nSize;i++)
			{
			   CString str=GetData(pNextNode)->GetSubItem(i);
			   lvItem.mask = LVIF_TEXT;
			   lvItem.iSubItem = i+1;
			   lvItem.pszText=str.GetBuffer(1);
			   SetItem(&lvItem);
			}
			nIndex++;
		}
	}
	InternaleUpdateTree();
	return nIndex;
}




int CSuperGridCtrl::SelectNode(CTreeItem *pLocateNode)
{
	if(IsRoot(pLocateNode))
	{
		UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
		SetItemState(0, uflag, uflag);
		return 0;
	}
	int nSelectedItem=-1;
	CTreeItem* pNode = pLocateNode;
	CTreeItem* pTopLevelParent=NULL;
	//Get top parent
	while(1)
	{
		CTreeItem *pParent = GetParentItem(pLocateNode);
		if(IsRoot(pParent))
			break;
		pLocateNode = pParent;
	}
	pTopLevelParent = pLocateNode;//on top of all
	//Expand the folder
	if(pTopLevelParent != NULL)
	{
		SetRedraw(0);
		CWaitCursor wait;
		CTreeItem *pRoot = GetParentItem(pTopLevelParent);

		if(IsCollapsed(pRoot))
			Expand(pRoot,0);

		ExpandUntil(pTopLevelParent, pNode);

		UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
		nSelectedItem = NodeToIndex(pNode);

		SetItemState(nSelectedItem, uflag, uflag);

		SetRedraw(1);
		EnsureVisible(nSelectedItem, TRUE);
	}
	return nSelectedItem;
}




void CSuperGridCtrl::ExpandUntil(CTreeItem *pItem, CTreeItem* pStopAt)
{
	const int nChildren = pItem->m_listChild.GetCount();
	if (nChildren > 0)
	{
		POSITION pos = pItem->m_listChild.GetHeadPosition();
		while (pos)
		{
			CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
			if(pChild == pStopAt)	
			{
				int nSize = GetIndent(pChild);
				CTreeItem** ppParentArray = new CTreeItem*[nSize];
				int i=0;
				while(1)
				{
					CTreeItem *pParent = GetParentItem(pChild);
					
					if(IsRoot(pParent))
						break;
					pChild = pParent;
					ppParentArray[i] = pChild;
					i++;
				}

				for(int x=i; x > 0; x--)
				{
					CTreeItem *pParent = ppParentArray[x-1];
					Expand(pParent, NodeToIndex(pParent));
				}
				delete [] ppParentArray;
				return;
			}
		}
	}

	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos)
	{
		CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		ExpandUntil(pChild, pStopAt);
	}
	
}



void CSuperGridCtrl::DeleteItemEx(CTreeItem *pSelItem, int nItem)
{
	SetRedraw(0);
	DeleteItem(nItem);//delete cur item in listview
	//delete/hide all children in pSelItem
	HideChildren(pSelItem, TRUE, nItem);
	//delete all internal nodes
	// If root, must delete from m_rootData
	if(GetParentItem(pSelItem) == NULL )
	{
		DeleteRootItem(pSelItem);
	}
	else
		Delete(pSelItem);

	InternaleUpdateTree();
	if(nItem-1<0)//no more items in list
	{
		SetRedraw(1); 
		InvalidateRect(NULL);
		UpdateWindow();
		return;
	}

	UINT uflag = LVIS_SELECTED | LVIS_FOCUSED;
	CRect rcTestIfItemIsValidToSelectOtherWiseSubtrackOneFromItem;//just to get the documention right :)
	GetItemRect(nItem, rcTestIfItemIsValidToSelectOtherWiseSubtrackOneFromItem ,LVIR_LABEL) ? SetItemState(nItem, uflag, uflag) : SetItemState(nItem-1, uflag, uflag);
	
	SetRedraw(1);
	InvalidateRect(NULL);
	UpdateWindow();
}



void CSuperGridCtrl::CleanMe(CTreeItem *pItem)
{
	// delete child nodes
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CTreeItem* pItemData = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		if(pItemData!=NULL)
		{
			if(pItemData->m_lpNodeInfo!=NULL)
				delete pItemData->m_lpNodeInfo;

			pItemData->m_listChild.RemoveAll();
			delete pItemData;
		}
	}
	pItem->m_listChild.RemoveAll();
}




CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetNext(CTreeItem* pStartAt, CTreeItem* pNode, BOOL bInit, BOOL bDontIncludeHidden)
{
	static BOOL bFound;
	if (bInit)
		bFound = FALSE;
		
	if (pNode == pStartAt)
		bFound = TRUE;

	if(bDontIncludeHidden)
	{
		if (!IsCollapsed(pStartAt))
		{
			POSITION pos = pStartAt->m_listChild.GetHeadPosition();
			while (pos != NULL)
			{
				CTreeItem* pChild = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
				if (bFound)
					return pChild;
				pChild = GetNext(pChild, pNode, FALSE, TRUE);
				if (pChild != NULL)
					return pChild;
			}
		}
	}
	else {
			POSITION pos = pStartAt->m_listChild.GetHeadPosition();
			while (pos != NULL)
			{
				CTreeItem* pChild = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
				if (bFound)
					return pChild;
				pChild = GetNext(pChild, pNode, FALSE,FALSE);
				if (pChild != NULL)
					return pChild;
			}
	}
	// if reached top and last level return original
	if (bInit)
		return pNode;
	else
		return NULL;
}



CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetPrev(CTreeItem* pStartAt, CTreeItem* pNode, BOOL bInit, BOOL bDontIncludeHidden)
{
	static CTreeItem* pPrev;
	if (bInit)
		pPrev = pStartAt;

	if (pNode == pStartAt)
		return pPrev;

	pPrev = pStartAt;

	if(bDontIncludeHidden)
	{
		if (!IsCollapsed(pStartAt))
		{
			POSITION pos = pStartAt->m_listChild.GetHeadPosition();
			while (pos != NULL)
			{
				CTreeItem* pCur = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
				CTreeItem* pChild = GetPrev(pCur,pNode, FALSE,TRUE);
				if (pChild != NULL)
					return pChild;
			}
		}
	}
	else {
		POSITION pos = pStartAt->m_listChild.GetHeadPosition();
		while (pos != NULL)
		{
			CTreeItem* pCur = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
			CTreeItem* pChild = GetPrev(pCur,pNode, FALSE,FALSE);
			if (pChild != NULL)
				return pChild;
		}
	}

	if (bInit)
		return pPrev;
	else
		return NULL;
}


int CSuperGridCtrl::_NodeToIndex(CTreeItem *pStartpos, CTreeItem* pNode, int& nIndex, BOOL binit)
{
	static BOOL bFound;	
	// Account for other root nodes
	if(GetParentItem(pStartpos) == NULL && GetRootIndex(pStartpos) !=0)
		nIndex++;

	if(binit)
		bFound=FALSE;

	if(pStartpos==pNode)
		bFound=TRUE;

	if(!IsCollapsed(pStartpos))
	{
		POSITION pos = GetHeadPosition(pStartpos);
		while (pos)
		{
			CTreeItem *pChild = GetNextChild(pStartpos, pos);
			if(bFound)
				return nIndex;

//	Craig Schmidt: Cannot set nIndex as return value.  Worked find with single root but
//				   the calling function get confused since the return value may indicate
//				   that the next root needs to be searched.  Didn'd spend much time on
//				   this so there is probably a better way of doing this.
//			nIndex = _NodeToIndex(pChild, pNode, nIndex, binit);
			_NodeToIndex(pChild, pNode, nIndex, binit);
			nIndex++;
		}
	}
	if(binit && bFound)
		return nIndex;
	else
		return -1;
}


BOOL CSuperGridCtrl::Delete(CTreeItem* pNode, BOOL bClean)
{
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * pRoot = (CTreeItem*)m_RootItems.GetNext(pos);
		if(_Delete(pRoot, pNode, bClean))
			return TRUE;
	}
	return FALSE;
}



BOOL CSuperGridCtrl::_Delete(CTreeItem* pStartAt, CTreeItem* pNode, BOOL bClean)
{
	POSITION pos = pStartAt->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		POSITION posPrev = pos;
		CTreeItem *pChild = (CTreeItem*)pStartAt->m_listChild.GetNext(pos);
		if (pChild == pNode)
		{
			pStartAt->m_listChild.RemoveAt(posPrev);
			if(bClean)
			{
				if(GetData(pNode)!=NULL)
					delete GetData(pNode);
				delete pNode;
			}
			return TRUE;
		}
		if (_Delete(pChild, pNode) == TRUE)
			return TRUE;
	}
	return FALSE;
}




BOOL CSuperGridCtrl::IsChildOf(const CTreeItem* pParent, const CTreeItem* pChild) const
{
	if (pChild == pParent)
		return TRUE;
	POSITION pos = pParent->m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CTreeItem* pNode = (CTreeItem*)pParent->m_listChild.GetNext(pos);
		if (IsChildOf(pNode, pChild))
			return TRUE;
	}
	return FALSE;
}



void CSuperGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if( GetFocus() != this) 
		SetFocus();

	LVHITTESTINFO ht;
	ht.pt = point;
	SubItemHitTest(&ht);
	if(OnItemLButtonDown(ht))
	{
		BOOL bSelect=1;
		bSelect = HitTestOnSign(point, ht);
		if(bSelect && ht.iItem!=-1)
		{
			m_CurSubItem = IndexToOrder(ht.iSubItem);
			CHeaderCtrl* pHeader = GetHeaderCtrl();
			// Make the column fully visible.
			MakeColumnVisible(Header_OrderToIndex(pHeader->m_hWnd, m_CurSubItem));
			CListCtrl::OnLButtonDown(nFlags, point);
			OnControlLButtonDown(nFlags, point, ht);
			//update row anyway for selection bar
			CRect rc;
			GetItemRect(ht.iItem, rc, LVIR_BOUNDS);
			InvalidateRect(rc);
			UpdateWindow();
		}
	}
}


void CSuperGridCtrl::OnUpdateListViewItem(CTreeItem* lpItem, LV_ITEM *plvItem)
{
	//default implementation you would go for this 9 out of 10 times
	CItemInfo *lp = GetData(lpItem);
	CString str = (CString)plvItem->pszText;
	if(lp!=NULL)
	{
		if(plvItem->iSubItem==0)
			lp->SetItemText(str);
		else //subitem data 
			lp->SetSubItemText(plvItem->iSubItem-1, str);
	   UpdateData(lpItem, lp);
	}
	SetItemText(plvItem->iItem, plvItem->iSubItem, plvItem->pszText);
}



void CSuperGridCtrl::DeleteAll()
{
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * pRoot = (CTreeItem*)m_RootItems.GetNext(pos);
		if(pRoot!=NULL)
			DeleteItemEx(pRoot, 0);
	}
}


POSITION CSuperGridCtrl::GetRootHeadPosition() const
{
	return m_RootItems.GetHeadPosition();
}


POSITION CSuperGridCtrl::GetRootTailPosition() const
{
	return m_RootItems.GetTailPosition();
}


CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetNextRoot(POSITION& pos) const
{
	return (CTreeItem*)m_RootItems.GetNext(pos);
}


CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetPrevRoot(POSITION& pos) const
{
	return (CTreeItem*)m_RootItems.GetNext(pos);
}


POSITION CSuperGridCtrl::GetHeadPosition(CTreeItem* pItem) const
{
	return pItem->m_listChild.GetHeadPosition();
}



CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetNextChild(CTreeItem *pItem, POSITION& pos) const
{
	return (CTreeItem*)pItem->m_listChild.GetNext(pos);
}



CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetPrevChild(CTreeItem *pItem, POSITION& pos) const
{
	return (CTreeItem*)pItem->m_listChild.GetPrev(pos);
}



POSITION CSuperGridCtrl::GetTailPosition(CTreeItem *pItem) const
{
	return pItem->m_listChild.GetTailPosition();
}



void CSuperGridCtrl::AddTail(CTreeItem *pParent, CTreeItem *pChild)
{
	pParent->m_listChild.AddTail(pChild);
}


inline int StrComp(const CString* pElement1, const CString* pElement2)
{
	return pElement1->Compare(*pElement2);
}



int CSuperGridCtrl::CompareChildren(const void* p1, const void* p2)
{
	CTreeItem * pChild1 = *(CTreeItem**)p1;
	CTreeItem * pChild2 = *((CTreeItem**)p2);
	CItemInfo *pItem1=(*pChild1).m_lpNodeInfo;
	CItemInfo *pItem2=(*pChild2).m_lpNodeInfo;
	return StrComp(&(pItem1->GetItemText()), &(pItem2->GetItemText()));
}


void CSuperGridCtrl::Sort(CTreeItem* pParent, BOOL bSortChildren)
{
	const int nChildren = NumChildren(pParent);
	if (nChildren > 1)
	{
		CTreeItem** ppSortArray = new CTreeItem*[nChildren];
		// Fill in array with pointers to our children.
		POSITION pos = pParent->m_listChild.GetHeadPosition();
		for (int i=0; pos; i++)
		{
			ASSERT(i < nChildren);
			ppSortArray[i] = (CTreeItem*)pParent->m_listChild.GetAt(pos);
			pParent->m_listChild.GetNext(pos);
		}

		qsort(ppSortArray, nChildren, sizeof(CTreeItem*), CompareChildren);
		// reorg children with new sorted list
		pos = pParent->m_listChild.GetHeadPosition();
		for (i=0; pos; i++)
		{
			ASSERT(i < nChildren);
			pParent->m_listChild.SetAt(pos, ppSortArray[i]);
			pParent->m_listChild.GetNext(pos);
		}

		delete [] ppSortArray;
	}

	if(bSortChildren)
	{
		POSITION pos = pParent->m_listChild.GetHeadPosition();
		while (pos)
		{
			CTreeItem *pChild = (CTreeItem*)pParent->m_listChild.GetNext(pos);
			Sort(pChild, TRUE);
		}
	}
}



int CSuperGridCtrl::NumChildren(const CTreeItem *pItem) const
{
	return pItem->m_listChild.GetCount();
}



BOOL CSuperGridCtrl::ItemHasChildren(const CTreeItem* pItem) const
{ 
	BOOL bChildren = pItem->m_listChild.GetCount() != 0;
	//see if we have a flag
	int nFlag = pItem->m_bSetChildFlag;
	if(nFlag!=-1)
		return 1;
	else
		return bChildren;
}


void CSuperGridCtrl::SetChildrenFlag(CTreeItem *pItem, int nFlag) const
{
	pItem->m_bSetChildFlag = nFlag;
}


BOOL CSuperGridCtrl::IsCollapsed(const CTreeItem* pItem) const
{
	return pItem->m_bHideChildren;//e.g not visible
}


void CSuperGridCtrl::Hide(CTreeItem* pItem, BOOL bFlag)
{
	pItem->m_bHideChildren=bFlag;
}


int CSuperGridCtrl::GetIndent(const CTreeItem* pItem) const
{
	return pItem->m_nIndent;
}


void CSuperGridCtrl::SetIndent(CTreeItem *pItem, int iIndent)
{
	pItem->m_nIndent = iIndent;
}



int CSuperGridCtrl::GetCurIndex(const CTreeItem *pItem) const
{
	return pItem->m_nIndex;
}


void CSuperGridCtrl::SetCurIndex(CTreeItem* pItem, int nIndex) 
{
	pItem->m_nIndex = nIndex;
}


void CSuperGridCtrl::SetParentItem(CTreeItem*pItem, CTreeItem* pParent)
{
	pItem->m_pParent=pParent;

}


CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetParentItem(const CTreeItem* pItem) 
{
	return pItem->m_pParent;
};



CItemInfo* CSuperGridCtrl::GetData(const CTreeItem* pItem) 
{
	return pItem->m_lpNodeInfo;
}



void CSuperGridCtrl::UpdateData(CTreeItem* pItem, CItemInfo* lpInfo)
{
	pItem->m_lpNodeInfo = lpInfo;
}


//overrides
CItemInfo* CSuperGridCtrl::CopyData(CItemInfo* lpSrc)
{
	ASSERT(FALSE);//debug
	return NULL;  //release
}

//default implementation for setting icons
int CSuperGridCtrl::GetIcon(const CTreeItem* pItem)
{
	
	return 0;//just take the first item in CImageList ..what ever that is
}


void CSuperGridCtrl::OnControlLButtonDown(UINT nFlags, CPoint point, LVHITTESTINFO& ht)
{
	if (GetStyle() & LVS_EDITLABELS)
	{
		CRect rcItem;
		if (ht.iSubItem == 0)
		{
			GetItemRect(ht.iItem, rcItem, LVIR_LABEL);
			DWORD dwStyle = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL|ES_LEFT;
			CEdit *pEdit = new CListEditCtrl(ht.iItem, ht.iSubItem, GetItemText(ht.iItem, ht.iSubItem));
			pEdit->Create(dwStyle, rcItem, this, 0x1233);	
		}
		else
			EditLabelEx(ht.iItem, ht.iSubItem);	
	}
}
	

COLORREF CSuperGridCtrl::GetCellRGB()
{
	return RGB(192,0,0);
}

	
BOOL CSuperGridCtrl::OnVKMultiply(CTreeItem *pItem, int nIndex)
{
	return 1;
}



BOOL CSuperGridCtrl::OnVkSubTract(CTreeItem *pItem, int nIndex)
{
	return 1;
}



BOOL CSuperGridCtrl::OnVKAdd(CTreeItem *pItem, int nIndex)
{
	return 1;
}


BOOL CSuperGridCtrl::OnDeleteItem(CTreeItem* pItem, int nIndex)
{
	return 1;
}



BOOL CSuperGridCtrl::OnItemExpanding(CTreeItem *pItem, int iItem)
{
	return 1;
}


BOOL CSuperGridCtrl::OnItemExpanded(CTreeItem* pItem, int iItem)
{
	return 1;
}


BOOL CSuperGridCtrl::OnCollapsing(CTreeItem *pItem)
{
	return 1;
}


BOOL CSuperGridCtrl::OnItemCollapsed(CTreeItem *pItem)
{
	return 1;
}


BOOL CSuperGridCtrl::OnItemLButtonDown(LVHITTESTINFO& ht)
{
	return 1;
}


BOOL CSuperGridCtrl::OnVkReturn(void)
{
	return 0;
}


void CSuperGridCtrl::OnSysColorChange() 
{
	CListCtrl::OnSysColorChange();
	//nop nothing yet
}



UINT CSuperGridCtrl::_GetCount(CTreeItem* pItem, UINT& nCount)
{
	POSITION pos = pItem->m_listChild.GetHeadPosition();
	while (pos)
	{
		CTreeItem *pChild = (CTreeItem*)pItem->m_listChild.GetNext(pos);
		nCount = _GetCount(pChild, nCount);
		nCount++;				
	}
	return nCount;
}



UINT CSuperGridCtrl::GetCount(void) 
{
	UINT nCount=0;
	UINT _nCount=0;
	POSITION pos = m_RootItems.GetHeadPosition();
	while(pos!=NULL)
	{
		CTreeItem * pRoot = (CTreeItem*)m_RootItems.GetNext(pos);
		nCount += _GetCount(pRoot, _nCount) + 1;
	}
	return nCount;
}



CSuperGridCtrl::CTreeItem* CSuperGridCtrl::GetTreeItem(int nIndex /*nIndex must be valid of course*/ ) 
{
	return reinterpret_cast<CTreeItem*>(GetItemData(nIndex));
}



int CSuperGridCtrl::GetSelectedItem(void) const
{
	return GetNextItem(-1, LVNI_ALL | LVNI_SELECTED); 
}

//////////////////////////////////////////////////////////////////////////
//
// not much but ... 
//
//////////////////////////////////////////////////////////////////////////

CSuperGridCtrl::CTreeItem::~CTreeItem()
{
	// delete child nodes
	POSITION pos = m_listChild.GetHeadPosition();
	while (pos != NULL)
	{
		CTreeItem* pItem = (CTreeItem*)m_listChild.GetNext(pos);
		if(pItem!=NULL)
		{
			if(pItem->m_lpNodeInfo!=NULL)
				delete pItem->m_lpNodeInfo;
			delete pItem;
		}
	}
	m_listChild.RemoveAll();
}



//////////////////////////////////////////////////////////////////////////
//
// Simple class CRectangle for the + - sign, 
//
//////////////////////////////////////////////////////////////////////////

CRectangle::CRectangle(CSuperGridCtrl* pCtrl, CDC* pDC, int iIndent, const CRect& rcBounds)
{
	m_pDC=pDC;
	int nHalfImage = (pCtrl->m_cxImage >> 1);
	int nBottomDown = rcBounds.top + nHalfImage + ((rcBounds.Height() - pCtrl->m_cyImage) >> 1);
	m_right_bottom.cx = (pCtrl->m_cxImage>>1)+2+1;
	m_right_bottom.cy = (pCtrl->m_cyImage>>1)+2+1;
	m_left = rcBounds.left  + iIndent * pCtrl->m_cxImage - nHalfImage;
	m_top = nBottomDown - (m_right_bottom.cy >> 1);
	m_left_top.x = m_left -  (m_right_bottom.cx >> 1);
	m_left_top.y = m_top;
	m_topdown = nBottomDown;
}

	
void CRectangle::DrawRectangle(CSuperGridCtrl* pCtrl)
{
	//erase bkgrnd
	CRect rc(m_left_top, m_right_bottom);
	m_pDC->FillRect(rc, &pCtrl->m_brushErase);
	//draw rectangle	
	CPen* pPenRectangle = m_pDC->SelectObject(&pCtrl->m_psRectangle);
	m_pDC->Rectangle(rc);
	m_pDC->SelectObject(pPenRectangle);		
}


CRectangle::~CRectangle()
{
}


BOOL CRectangle::HitTest(CPoint pt)
{
	CRect rc = GetHitTestRect();
	return rc.PtInRect(pt);
}


void CRectangle::DrawPlus(void)
{
	m_pDC->MoveTo(m_left, m_topdown-2);
	m_pDC->LineTo(m_left, m_topdown+3);

	m_pDC->MoveTo(m_left-2, m_topdown);
	m_pDC->LineTo(m_left+3, m_topdown);
}


void CRectangle::DrawMinus(void)
{
	m_pDC->MoveTo(m_left-2, m_topdown);
	m_pDC->LineTo(m_left+3, m_topdown);
}


CSuperGridCtrlEx::CSuperGridCtrlEx() : CSuperGridCtrl()
{
	m_nBmpId = -1;
}

BEGIN_MESSAGE_MAP(CSuperGridCtrlEx, CSuperGridCtrl)
	//{{AFX_MSG_MAP(CSuperGridCtrlEx)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


int CSuperGridCtrlEx::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CSuperGridCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	/////////////////////
	//remember this	
	///////////////////
	//associate imagelist with listviewctrl	
	if (m_nBmpId != -1)
	{
		if(!m_image.Create(m_nBmpId,16,1,RGB(0, 255, 255)))
			return -1;
		SetImageList(&m_image, LVSIL_SMALL);
		CImageList *pImageList = GetImageList(LVSIL_SMALL);
		if(pImageList)
			ImageList_GetIconSize(pImageList->m_hImageList, &m_cxImage, &m_cyImage);
		else
			return -1;
	}
	return 0;
}

bool CSuperGridCtrlEx::CreateImageList(int id, int cx, int nGrow, COLORREF clrBack)
{
	m_nBmpId = id;
	if(!m_image.Create(m_nBmpId,cx,nGrow,clrBack))
		return false;
	SetImageList(&m_image, LVSIL_SMALL);
	CImageList *pImageList = GetImageList(LVSIL_SMALL);
	if(pImageList)
		ImageList_GetIconSize(pImageList->m_hImageList, &m_cxImage, &m_cyImage);
	else
		return false;
	return true;
}

