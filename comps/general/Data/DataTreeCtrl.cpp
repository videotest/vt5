// DataTreeCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "data.h"
#include "DataTreeCtrl.h"
#include "TimeTest.h"
#include "aliaseint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



//#define ID_INNER_TREE	1002
//#define GET_X_LPARAM(lp)   ((int)(short)LOWORD(lp))
//#define GET_Y_LPARAM(lp)   ((int)(short)HIWORD(lp))



/////////////////////////////////////////////////////////////////////////////
// CDataTreeCtrl

CDataTreeCtrl::CDataTreeCtrl()
{
	m_bTrack = false;
	m_hLastItem = NULL;
}

CDataTreeCtrl::~CDataTreeCtrl()
{
}


BEGIN_MESSAGE_MAP(CDataTreeCtrl, CTreeCtrl)
	//{{AFX_MSG_MAP(CDataTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDataTreeCtrl message handlers

int CDataTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	ModifyStyle(0, TVS_HASBUTTONS | TVS_HASLINES | TVS_LINESATROOT | 
				TVS_SHOWSELALWAYS | TVS_FULLROWSELECT | TVS_DISABLEDRAGDROP);
	return 0;
}


void CDataTreeCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this)
		SetFocus();

//	HTREEITEM hitem = GetItemFromPoint(point);
//	
//	if (hitem)
//		SelectItem(hitem);

	CTreeCtrl::OnLButtonDown(nFlags, point);
}


void CDataTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	UINT uFlags = 0;
	if (nFlags & MK_RBUTTON && m_bTrack)
	{
		HTREEITEM hitem = GetItemFromPoint(point);
		
		if (hitem == GetFirstVisibleItem() && VertScrollVisible())
		{
			// scroll window
			CRect rc;
			GetItemRect(hitem, &rc, false);
			CScrollBar* pbar = GetScrollBarCtrl(SB_VERT);
			if (pbar && ::IsWindow(pbar->GetSafeHwnd()))
			{
				pbar->SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), (LPARAM)0);
				SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0), (LPARAM)pbar->GetSafeHwnd());
			}
			else
				SendMessage(WM_VSCROLL, MAKEWPARAM(SB_LINEUP, 0));
				
		}

		if (hitem)
			SelectItem(hitem);
		else 
			SelectItem(GetLastVisibleItem());

		CWnd::OnMouseMove(nFlags, point);
	}
	else
		CTreeCtrl::OnMouseMove(nFlags, point);
}

bool CDataTreeCtrl::VertScrollVisible()
{
	int sMin, sMax;
	GetScrollRange(SB_VERT, &sMin, &sMax);
	return sMax != 0;
}

void CDataTreeCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (GetFocus() != this)
		SetFocus();

	if (!m_bTrack)
	{
		SetCapture();
		m_bTrack = true;
	}

	HTREEITEM hitem = GetItemFromPoint(point);
	
	if (hitem)
		SelectItem(hitem);
	else 
		SelectItem(GetLastVisibleItem());

	CWnd::OnRButtonDown(nFlags, point);
}

void CDataTreeCtrl::OnRButtonUp(UINT nFlags, CPoint point) 
{
	if (m_bTrack)
	{
		ReleaseCapture();
		m_bTrack = false;
	}

	SelectItem(m_hLastItem);
	CWnd::OnRButtonUp(nFlags, point);
}

HTREEITEM CDataTreeCtrl::GetItemFromPoint(CPoint pt)
{
	CRect rc;
	GetClientRect(&rc);
	
	if (!rc.PtInRect(pt))
		return NULL;

	int ncount = (int)GetVisibleCount();
	if (ncount == 0)
		return NULL;

	HTREEITEM hitem = GetFirstVisibleItem();

	while (hitem)
	{
		if (GetItemRect(hitem, &rc, false))
		{
			if (pt.y >= rc.top && pt.y <= rc.bottom)
				return hitem; 
		}

		hitem = GetNextVisibleItem(hitem);
	}

	return NULL;
}


HTREEITEM CDataTreeCtrl::GetLastVisibleItem()
{
	int ncount = (int)GetVisibleCount();
	if (ncount == 0)
		return NULL;

	HTREEITEM hitemlast = GetFirstVisibleItem();

	while (1)
	{
		HTREEITEM hitem = GetNextVisibleItem(hitemlast);
		if (!hitem)
			break;
		hitemlast = hitem;
	}

	return hitemlast;
}

BOOL CDataTreeCtrl::SelectItem(HTREEITEM hitem, UINT code)
{
	if (!hitem)
		return false;

	if (hitem == GetSelectedItem())
		return true;

	m_hLastItem = hitem;

	if (!Select(hitem, code))
		return CTreeCtrl::SelectItem(hitem);
	
	return true;
}

HTREEITEM CDataTreeCtrl::HitTestEx(CPoint point)
{
	HTREEITEM hitem = NULL;
	UINT uFlags = 0;
	
	hitem = HitTest(point, &uFlags);
	
	if (!hitem)
	{
		if (uFlags == TVHT_NOWHERE) 
			hitem = GetLastVisibleItem();
		else if (uFlags == TVHT_TOLEFT) 
			hitem = m_hLastItem;
		else if (uFlags == TVHT_TORIGHT)
			hitem = m_hLastItem;
		else if (uFlags == TVHT_ABOVE)
			hitem = GetFirstVisibleItem();
		else if (uFlags == TVHT_BELOW)
			hitem = GetLastVisibleItem();
	}
	return hitem;
}

void CDataTreeCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);

	if (!m_hLastItem)
		m_hLastItem = GetFirstVisibleItem();
	SelectItem(m_hLastItem);
}


bool CDataTreeCtrl::IsFirstItem(HTREEITEM hitem)
{
	HTREEITEM hParent = GetParentItem(hitem);
	HTREEITEM hPrev = GetPrevSiblingItem(hitem);

	if (hPrev)
		return false;
	if (hParent != NULL && hParent != TVI_ROOT)
		return false;
	return true;
}

bool CDataTreeCtrl::IsLastItem(HTREEITEM hitem)
{
	HTREEITEM hNext = GetNextSiblingItem(hitem);

	if (hNext)
		return false;
	
	if (ItemHasChildren(hitem))
		return false;

	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
// CContextTreeCtrl

IMPLEMENT_DYNCREATE(CContextTreeCtrl, CWnd)


CContextTreeCtrl::CContextTreeCtrl()
{
	m_lLastViewKey = -1;
	m_lLastDocKey = -1;
	m_pSelObjectData = 0;
	m_bDynamic = true;

	m_bHilightActiveItems = true;
}

CContextTreeCtrl::~CContextTreeCtrl()
{
	m_sptrView = NULL;
	m_sptrContext = NULL;
	m_sptrTypeMgr = NULL;
}


BEGIN_MESSAGE_MAP(CContextTreeCtrl, CWnd)
	//{{AFX_MSG_MAP(CContextTreeCtrl)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP

//	ON_NOTIFY(NM_DBLCLK, ID_INNER_TREE, OnDblclkTree)
//	ON_NOTIFY(NM_RDBLCLK, ID_INNER_TREE, OnRdblclkTree)
//	ON_NOTIFY(NM_RETURN, ID_INNER_TREE, OnReturnTree)
//	ON_NOTIFY(TVN_BEGINLABELEDIT, ID_INNER_TREE, OnBeginlabeleditTree)
//	ON_NOTIFY(TVN_ENDLABELEDIT, ID_INNER_TREE, OnEndlabeleditTree)
	ON_NOTIFY(TVN_SELCHANGING, ID_INNER_TREE, OnSelchanging)
	ON_NOTIFY(TVN_SELCHANGED, ID_INNER_TREE, OnSelchanged)

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CContextTreeCtrl message handlers

BOOL CContextTreeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	return CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW), 
						"", dwStyle | WS_CHILD | WS_VISIBLE, rect, pParentWnd, nID, NULL);
}

BOOL CContextTreeCtrl::Attach(DWORD dwStyle, CWnd* pParentWnd, UINT nID, bool bDynamic/* = false*/)
{
	// not dinamic creation
	m_bDynamic = false;
	//Gotta have a parent and he/she must be valid
	ASSERT_VALID(pParentWnd);
	
	//Get the control to replace
	CWnd* pCtrlToReplace = pParentWnd->GetDlgItem(nID);
	ASSERT_VALID(pCtrlToReplace);

	CRect rcOld;
	pCtrlToReplace->GetWindowRect(&rcOld);
	pParentWnd->ScreenToClient(&rcOld);

	BOOL bRet = this->Create(dwStyle, rcOld, pParentWnd, nID);
	ASSERT(bRet);

	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	this->SetWindowPos(pCtrlToReplace, rcOld.left, rcOld.top, rcOld.Width(), rcOld.Height(), 
		               SWP_SHOWWINDOW|SWP_NOREDRAW|SWP_NOACTIVATE);
	
	//Destroy the old control...we don't need it anymore
	pCtrlToReplace->DestroyWindow();

	UpdateWindow();

	return bRet;
}

int CContextTreeCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rc;
	GetClientRect(&rc);

	if (!m_Tree.Create(WS_CHILD | WS_VISIBLE , rc, this, ID_INNER_TREE))
		return -1;
	
	// and addition style
	ModifyStyle(WS_BORDER, 0);
	ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_NOACTIVATE);

	return 0;
}

void CContextTreeCtrl::OnDestroy() 
{
	ClearContens();
	CWnd::OnDestroy();
}

void CContextTreeCtrl::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(&rc);
	m_Tree.MoveWindow(rc, true);	
}

void CContextTreeCtrl::PostNcDestroy() 
{
	if (m_bDynamic)
		delete this;
}


// clear contens of control
bool CContextTreeCtrl::ClearContens()
{
	// clear contens of tree control
	RemoveAllTypes();

	// clear interface pointers
	return true;
}


bool CContextTreeCtrl::AttachData(IUnknown* punkView, IUnknown* punkData)
{
	if (punkView && CheckInterface(punkView, IID_IDataContext))
	{
		m_sptrContext	= punkView; 
		m_sptrView		= punkView; 
	}
	
	if (punkData && CheckInterface(punkData, IID_IDataTypeManager))
		m_sptrTypeMgr = punkData; 

	if (!punkView && m_sptrTypeMgr != NULL)
	{
		// try to get context by NamedData
		IUnknown* punk = 0;
		sptrINamedData sptrD((IUnknown*)m_sptrTypeMgr.GetInterfacePtr());
		if (FAILED(sptrD->GetActiveContext(&punk)))
			return false;

		if (punk)
		{
			m_sptrContext	= punk;
			m_sptrView		= punk;
			punk->Release();
		}
		else
			return false;
	} 
	else if (!punkData && m_sptrView != NULL)
	{
		// try to get NamedData by view
		IUnknown* punk = 0;
		if (FAILED(m_sptrView->GetDocument(&punk)) || !punk)
			return false;

		if (punk)
		{
			m_sptrTypeMgr	= punk;
			punk->Release();
		}
		else
			return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// zero level 
// add Type - Zero level of tree
bool CContextTreeCtrl::AddType(LPCTSTR sztype)
{
	_bstr_t	bstrIntTypeName = sztype;
	BSTR	bstrUserName;
	CString	strItemName = sztype;

	IAliaseManPtr	ptrAM( GetAppUnknown () );
	if( ptrAM != 0 )
	{
		if( ptrAM->GetObjectString( bstrIntTypeName, &bstrUserName ) == S_OK )
		{
			strItemName = bstrUserName;
			::SysFreeString( bstrUserName );
		}
	}

	ASSERT(sztype != NULL);
	ASSERT(lstrlen(sztype) != 0);

	// check exists
	HTREEITEM hitem = GetItemByName(sztype, TVI_ROOT);
	// if already exists
	if (hitem) // simply return
		return true;

	// create item data
	CContextTreeData* pdata = new CContextTreeData(NULL, sztype);
	if (!pdata)
		return false;
	// else add item
	hitem = m_Tree.InsertItem( strItemName, TVI_ROOT);
	if (!hitem)
		return false;
	// and add it to item
	if (!m_Tree.SetItemData(hitem, (DWORD)pdata))
		return false;
	
	TVSORTCB sort;
	sort.hParent = NULL;
	sort.lpfnCompare = CContextTreeCtrl::CompareFunc;
	sort.lParam = (LPARAM)this;

	m_Tree.SortChildrenCB(&sort);

	return true;
}

// remove type with its children
bool CContextTreeCtrl::RemoveType(LPCTSTR sztype)
{
	ASSERT(sztype != NULL);
	ASSERT(lstrlen(sztype) != 0);

	// check exists
	HTREEITEM hitem = GetItemByName(sztype, TVI_ROOT);
	// if such type is not exists
	if (!hitem) 
		return true;
	
	// remove its children
	if (!RemoveChilds(hitem))
		return false;

	// and remove it
	return 	_RemoveItem(hitem);
}

// remove all types with its children (it's means remove all items from tree)
// RemoveAllItem
bool CContextTreeCtrl::RemoveAllTypes()
{
	return RemoveChilds(TVI_ROOT);
}

// add object
bool CContextTreeCtrl::AddObject(IUnknown* punkObject, bool bActive)
{
	_try(CContextTreeCtrl, AddObject)
	{
		// get type of object
		CString strType = ::GetObjectKind(punkObject);
		// get object name
		CString strName = ::GetObjectName(punkObject);

		CString strPath = ::GetObjectPath((IUnknown*)m_sptrTypeMgr.GetInterfacePtr(), punkObject);

		if (!CheckInterface(punkObject, IID_INamedDataObject2))
			return false;

		sptrINamedDataObject2 sptr(punkObject);
		

		HTREEITEM hitem = FindObjectItem(punkObject);
		// if already exists
		if (hitem)
		{
			if (bActive && !ChangeActive(hitem, true))
				return false;
			else 
				return true;
		}
		// get parent object Item
		HTREEITEM hParent = FindParentItem(punkObject);

		ASSERT(hParent != NULL);

		if (!hParent)
			return false;
		
		// create item data
		CContextTreeData* pdata = new CContextTreeData(punkObject, strName, strPath);
		
		if (!pdata)
			return false;

		// add item
		if (!(hitem = m_Tree.InsertItem(pdata->GetName(), hParent)))
			return false;

		// and add it to item
		if (!m_Tree.SetItemData(hitem, (DWORD_PTR)pdata))
			return false;

		if (bActive)
		{
			if (!ChangeActive(hitem, true))
				return false;
		}

		// and add all children
		_bstr_t		bstrType(strType);
		IUnknown *	punk = 0;
		TPOS lPos = 0;
		// get object's children count (for children that are exists in data context
		m_sptrContext->GetFirstChildPos(bstrType, sptr, &lPos);

		while (lPos)
		{
			if (FAILED(m_sptrContext->GetNextChild(bstrType, sptr, &lPos, &punk)) || !punk)
				continue;

			if (!AddObject(punk, false))
				return false;

			if (punk)
				punk->Release();
		}

		// and expand this branch
		m_Tree.Expand(hParent, TVE_EXPAND);

		TVSORTCB sort;
		sort.hParent = hParent;
		sort.lpfnCompare = (PFNTVCOMPARE)CContextTreeCtrl::CompareFunc;
		sort.lParam = (LPARAM)this;
		m_Tree.SortChildrenCB(&sort);

	}
	_catch
	{
		return false;
	}
	return true;
}

// find object's HTREEITEM for any level objects
HTREEITEM CContextTreeCtrl::FindObjectItem( const char *pszObjectName )
{
	IUnknown	*punk = ::GetObjectByName( m_sptrTypeMgr, pszObjectName, 0 );
	if( punk )
	{
		HTREEITEM hti = FindObjectItem( punk );
		punk->Release();
		return hti;
	}
	return 0;
}
	// find object's HTREEITEM by object's IUnknown
HTREEITEM CContextTreeCtrl::FindObjectItem(IUnknown* punkObject)
{
	HTREEITEM hitem = NULL;
	IUnknown *punkParent = 0;

	_try(CContextTreeCtrl, FindObjectItem)
	{
		if (!CheckInterface(punkObject, IID_INamedDataObject2))
			return NULL;

		sptrINamedDataObject2 sptr(punkObject);
		
		// get Parent
		sptr->GetParent(&punkParent);
		// create pointer on it 
		sptrINamedDataObject2 sptrParent(punkParent);
		// and release it
		if (punkParent)
		{
			punkParent->Release();
			punkParent = 0;
		}

		if (sptrParent == NULL)		// first level object
		{
			// get type of object
			CString strType = ::GetObjectKind(punkObject);
			// get parent item
			HTREEITEM hType = GetItemByName(strType, TVI_ROOT);
			// if it's not already exists
			if (!hType) 
			{
				if (!(AddType(strType) && (hType = GetItemByName(strType, TVI_ROOT))))
					return NULL;
			}
			hitem = GetItemByObject(punkObject, hType);
		}
		else // non first level object
		{
			// recursive call this function
			HTREEITEM hParent = FindObjectItem((IUnknown*)sptrParent.GetInterfacePtr());
			
			if (!hParent) // must be never
				return NULL;
			
			hitem = GetItemByObject(punkObject, hParent);
		}
		// try to find object by key
		if (!hitem)
		{
			GuidKey lKey = ::GetObjectKey(punkObject);
			if (lKey != INVALID_KEY)
				hitem = FindItemByKey(lKey, 0);
		}
	}
	_catch
	{
		if (punkParent)
			punkParent->Release();
		return NULL;
	}
	return hitem;
}

HTREEITEM CContextTreeCtrl::FindItemByKey(GuidKey lKey, HTREEITEM hParent)
{
	HTREEITEM hItem = 0;
	if (lKey == INVALID_KEY)
		return NULL;

	if (hParent == 0)
		hParent = TVI_ROOT;

	// check this parent
//	if (!(m_Tree.GetSafeHwnd() && ::IsWindow(m_Tree.GetSafeHwnd())))
//		return NULL;

//	if (!m_Tree.ItemHasChildren(hParent))
//		return NULL;

	// get first item from parent
	HTREEITEM hChild = m_Tree.GetChildItem(hParent);
	while (hChild)
	{
		CContextTreeData *pdata = (CContextTreeData*) m_Tree.GetItemData(hChild);
		if (pdata)
		{
			if (::GetObjectKey(pdata->GetData()) == lKey)
				return hChild;
		}

		// check this child as parent
		hItem = FindItemByKey(lKey, hChild);
		if (hItem)
			return hItem;

		// get next sibling
		hChild = m_Tree.GetNextSiblingItem(hChild);
	}

	// if we are here it's means we can not find item with requested name
	return NULL;
}



HTREEITEM CContextTreeCtrl::FindParentItem(IUnknown* punkObject)
{
	HTREEITEM hitem = NULL;
	IUnknown *punkParent = 0;

	_try(CContextTreeCtrl, FindParentItem)
	{
		if (!CheckInterface(punkObject, IID_INamedDataObject2))
			return NULL;

		sptrINamedDataObject2 sptr(punkObject);
		
		// get Parent
		sptr->GetParent(&punkParent);
		// create pointer on it 
		sptrINamedDataObject2 sptrParent(punkParent);
		// and release it
		if (punkParent)
		{
			punkParent->Release();
			punkParent = 0;
		}

		if (sptrParent == NULL)		// first level object
		{
			// get type of object
			CString strType = ::GetObjectKind(punkObject);
			// get parent item
			hitem = GetItemByName(strType, TVI_ROOT);
			// if it's not already exists
			if (!hitem) 
			{
				if (!(AddType(strType) && (hitem = GetItemByName(strType, TVI_ROOT))))
					return NULL;
			}
		}
		else // non first level object
			hitem = FindObjectItem((IUnknown*)sptrParent.GetInterfacePtr());
	}
	_catch
	{
		if (punkParent)
			punkParent->Release();
		return NULL;
	}
	return hitem;
}



// remove object
bool CContextTreeCtrl::RemoveObject(IUnknown* punkObject)
{
	if (!punkObject)
		return false;

	HTREEITEM hitem = FindObjectItem(punkObject);
	if (!hitem)
		return false;

	// remove its children
	if (!RemoveChilds(hitem))
		return false;


	// and remove it
	if (!_RemoveItem(hitem))
		return false;

	return true;
}

// object change active
bool CContextTreeCtrl::ChangeActive(IUnknown* punkObject, bool bActive)
{
	if (!punkObject)
		return false;

	// get object hitem
	HTREEITEM hitem = FindObjectItem(punkObject);
	if (!hitem)
		return false;
	
	return ChangeActive(hitem, bActive);
}

// object change active
bool CContextTreeCtrl::ChangeActive(HTREEITEM hitem, bool bActive)
{
	ASSERT(hitem != NULL);

	if (!hitem)
		return false;

	if (bActive)
	{
		// Reset current active
		IUnknown * punk = GetObjectByItem(hitem);
		CString strType = ::GetObjectKind(punk);
		if (punk)
			punk->Release();

		HTREEITEM hCurActive = GetActiveObject(strType);
		// if current active exists we need to set it to nonactive
		if (hCurActive)
			SetObjectActive(hCurActive, false);
	}

	// set requested object to correspondiiing state
	return SetObjectActive(hitem, bActive);
}


// get active object by type
HTREEITEM CContextTreeCtrl::GetActiveChild(HTREEITEM hParent)
{
	ASSERT(hParent != NULL);

	if (!hParent)
		return NULL;
	
	// get first item from parent
	HTREEITEM hChild = m_Tree.GetChildItem(hParent);

	// and check bold fort of sibling items with sztype 
	while (hChild != NULL)
	{
		// get state
		UINT state = m_Tree.GetItemState(hChild, TVIS_BOLD | TVIS_SELECTED);
	
		// check state contains bold font
		if (state & TVIS_BOLD)
			// we finded it so we need to return object from this hitem
			return hChild;//GetObjectByItem(hitem);

		if (m_Tree.ItemHasChildren(hChild))
		{
			HTREEITEM hItem = GetActiveChild(hChild);
			if (hItem)
				return hItem; 
		}
		// get next sibling
		hChild = m_Tree.GetNextSiblingItem(hChild);
	}
	return NULL;
}

// get active object for parent
HTREEITEM CContextTreeCtrl::GetActiveChild(IUnknown* punkParent)
{
	ASSERT(punkParent != NULL);

	if (!punkParent)
		return NULL;

	// Get Parent hItem
	HTREEITEM hParent = FindObjectItem(punkParent);
	if (hParent)
		return NULL;

	// get first item from parent
	HTREEITEM hChild = m_Tree.GetChildItem(hParent);
	// and check bold fort of sibling items with sztype 
	while (hChild != NULL)
	{
		// get state
		UINT state = m_Tree.GetItemState(hChild, TVIS_BOLD | TVIS_SELECTED);
		// check state contains bold font
		if (state & TVIS_BOLD)
			// we finded it so we need to return object from this hitem
			return hChild;//GetObjectByItem(hitem);
		
		// get next sibling
		hChild = m_Tree.GetNextSiblingItem(hChild);
	}
	return NULL;
}


// get active object by type
HTREEITEM CContextTreeCtrl::GetActiveObject(LPCTSTR sztype)
{
	ASSERT(sztype != NULL);
	ASSERT(lstrlen(sztype) != 0);

	HTREEITEM hparent = GetItemByName(sztype, TVI_ROOT);
	// if this type is not already exists
	if (!hparent) 
		return NULL;
	
	return GetActiveChild(hparent);
}


// remove all object of this type
bool CContextTreeCtrl::RemoveAllObjects(LPCTSTR sztype)
{
	ASSERT(sztype != NULL);
	ASSERT(lstrlen(sztype) != 0);

	// check exists this type
	HTREEITEM hitem = GetItemByName(sztype, TVI_ROOT);
	// if such type is not exists
	if (!hitem) 
		return true;
	
	// remove its children
	if (!RemoveChilds(hitem))
		return false;

	return true;
}



// utilities function /////////////////////////////////////////////
// get HTREEITEM by name
HTREEITEM CContextTreeCtrl::GetItemByName(LPCTSTR szname, HTREEITEM hparent)
{
	CString strName(szname);

	// get first item from parent
	HTREEITEM hitem = m_Tree.GetChildItem(hparent);
	// and compare names of sibling items with sztype 
	while (hitem != NULL)
	{
		CContextTreeData *pdata = (CContextTreeData*) m_Tree.GetItemData(hitem);
		CString strItem = m_Tree.GetItemText(hitem);
		if( pdata )strItem = pdata->GetName();
		// if such type exists
		if (strItem == strName)
			// return HTREEITEM
			return hitem;
		
		// get next sibling
		hitem = m_Tree.GetNextSiblingItem(hitem);
	}
	// if we are here we can not find item with requested name
	return NULL;
}

// get HTREEITEM by object and parents HTREEITEM
HTREEITEM CContextTreeCtrl::GetItemByObject(IUnknown* punkObj, HTREEITEM hparent)
{
	IUnknownPtr UnkPtr(punkObj);
	// get first item from parent
	HTREEITEM hitem = m_Tree.GetChildItem(hparent);
	// and compare names of sibling items with sztype 
	while (hitem != NULL)
	{
		CContextTreeData *pdata = (CContextTreeData*) m_Tree.GetItemData(hitem);
		// if no data exists
		if (!pdata) //
			continue;

		IUnknown* punk = pdata->GetData();
		// if not exists ptr on interface
		if (!punk)
			continue;

		IUnknownPtr ptr(punk);
		/*if (punk)
			punk->Release();*/
		// if pointers is equals
		if (::GetObjectKey(ptr) == ::GetObjectKey(UnkPtr))
			return hitem;

		// get next sibling
		hitem = m_Tree.GetNextSiblingItem(hitem);
	}
	// if we are here we can not find item with requested name
	return NULL;
}

// get object for any level item
IUnknown*  CContextTreeCtrl::GetObjectByItem(HTREEITEM hitem)
{
	ASSERT(hitem != NULL);
	// item is valid
	if (!hitem)
		return NULL;
	// get data
	CContextTreeData *pdata = (CContextTreeData*) m_Tree.GetItemData(hitem);
	// check data
	if (!pdata)
		return NULL;
	// return pointer on interface with AddRef

	IUnknown *punk = pdata->GetData();
	if( punk )punk->AddRef();
	return punk;
}

// remove all children from hparent
bool CContextTreeCtrl::RemoveChilds(HTREEITEM hparent)
{
	ASSERT(hparent != NULL);
	if (hparent)
	{
		// get first item from parent
		HTREEITEM hitem = m_Tree.GetChildItem(hparent);
		// and remove it and its sibling items 
		while (hitem != NULL)
		{
			if (!RemoveChilds(hitem))
				return false;

			// get next sibling
			HTREEITEM hnext = m_Tree.GetNextSiblingItem(hitem);

			if (!_RemoveItem(hitem))
				return false;

			hitem = hnext;
		}
	}
	return true;
}


// remove item with its data
bool CContextTreeCtrl::_RemoveItem(HTREEITEM hitem)
{
	ASSERT(hitem != NULL);
	// get parent item
	HTREEITEM hParent = m_Tree.GetParentItem(hitem);
	// get data
	CContextTreeData *pdata = (CContextTreeData*) m_Tree.GetItemData(hitem);
	// delete data
	if (pdata)
		delete pdata;
	// set data to 0
	if (!m_Tree.SetItemData(hitem, 0))
		return false;
	// and delete item
	if (!m_Tree.DeleteItem(hitem))
		return false;

	TVSORTCB sort;
	sort.hParent = hParent;
	sort.lpfnCompare = (PFNTVCOMPARE)CContextTreeCtrl::CompareFunc;
	sort.lParam = (LPARAM)this;
	m_Tree.SortChildrenCB(&sort);

	return true;
}

// check object representation
bool CContextTreeCtrl::IsObjectActive(IUnknown* punkObj)
{
	IUnknownPtr UnkPtr(punkObj);

	HTREEITEM hitem =  FindObjectItem(punkObj);
	if (!hitem)
		return false;

	return IsObjectActive(hitem);
}

// check item has bold font
bool CContextTreeCtrl::IsObjectActive(HTREEITEM hitem)
{
	ASSERT(hitem != NULL);

	if (!hitem)
		return false;
	// get item state
	UINT state = m_Tree.GetItemState(hitem, TVIS_BOLD | TVIS_SELECTED);
	// check state contains bold font
	return (state & TVIS_BOLD) ? true : false;
}

bool CContextTreeCtrl::SetObjectActive(HTREEITEM hitem, bool bActive)
{
	ASSERT(hitem != NULL);

	if (!hitem)
		return false;

	if( !GetHilightActiveItems() )
		return false;
	
	// get item state
	UINT state = m_Tree.GetItemState(hitem, TVIS_BOLD | TVIS_SELECTED);
	// change it depend from flag
	state = (bActive) ? (state | TVIS_BOLD) : (state & (~TVIS_BOLD));
	// and set it
	if (!m_Tree.SetItemState(hitem, state, TVIS_BOLD | TVIS_SELECTED))
		return false;

	if( bActive )
		m_Tree.SelectItem( hitem );
	
	return true;
}

bool CContextTreeCtrl::SetItemHighlight(HTREEITEM hitem)
{
	ASSERT(hitem != NULL);
	// check item valid
	if (!hitem)
		return false;

	// get current selected item	
	HTREEITEM hcur = m_Tree.GetSelectedItem();
	if (hcur)
	{
		// and set it to unselect
		if (!m_Tree.SetItemState(hitem, 0, TVIS_SELECTED))
			return false;
	}

	// and set new selected item
	DWORD	dwNewState = TVIS_SELECTED;

	if( GetHilightActiveItems() )
		dwNewState |= TVIS_BOLD;

	return (m_Tree.SetItemState(hitem, dwNewState, TVIS_BOLD | TVIS_SELECTED)) ? true : false;
}

bool CContextTreeCtrl::SetObjectHighlight(IUnknown* punkObj)
{
	// get objects item
	HTREEITEM hitem = FindObjectItem(punkObj);
	if (!hitem)
		return false;

	return SetItemHighlight(hitem);
}

// 
bool CContextTreeCtrl::FillAllTypes(bool bBranch)
{
	//CTimeTest tt (true, "CContextTreeCtrl::FillAllTypes");
	if (::IsWindow(GetSafeHwnd()))
		ShowWindow(SW_HIDE);
	_try(CContextTreeCtrl, FillAllTypes)
	{
		long nCount = 0;
		m_sptrTypeMgr->GetTypesCount(&nCount);

		// get all types
		for (long nType = 0; nType < nCount; nType++)
		{
			BSTR	bstrType;
			m_sptrTypeMgr->GetType(nType, &bstrType);

			INamedDataInfo	*pTypeInfo = 0;
			IDataTypeInfoManagerPtr	ptrMan( GetAppUnknown() );
			ptrMan->GetTypeInfo( bstrType, &pTypeInfo);
			::SysFreeString( bstrType );
			INamedObject2Ptr	ptrNamed( pTypeInfo );
			pTypeInfo->Release( );
			BSTR bstrTypeName = 0;
			ptrNamed->GetName( &bstrTypeName );

			
			_bstr_t bstrtype(bstrTypeName, false);

			//store type name for last ussage
			if (!AddType(bstrtype))
			{
				if (::IsWindow(GetSafeHwnd()))
					ShowWindow(SW_SHOW);
				return false;
			}

			if (bBranch && !FillType(bstrtype))
			{
				if (::IsWindow(GetSafeHwnd()))
					ShowWindow(SW_SHOW);
				return false;
			}
		}
	}
	_catch
	{
		RemoveAllTypes();
		{
			if (::IsWindow(GetSafeHwnd()))
				ShowWindow(SW_SHOW);
			return false;
		}
	}
	if (::IsWindow(GetSafeHwnd()))
		ShowWindow(SW_SHOW);
	return true;
}

bool CContextTreeCtrl::FillType(LPCTSTR sztype)
{
	CString strText;
	strText.Format("CContextTreeCtrl::FillType - %s", sztype);
	//CTimeTest tt (true, strText);
	HTREEITEM	hParent = NULL;
	_bstr_t		bstrType(sztype);
	IUnknown *	punkObj = 0;
	
	ASSERT(sztype != NULL);
	ASSERT(lstrlen(sztype) != 0);

	_try(CContextTreeCtrl, FillType)
	{
		// check exists this type
		if (!AddType(sztype))
			return false;
		// get hitem  == parent for all objects of this type
		hParent = GetItemByName(sztype); 

		TPOS lPos = 0;
		m_sptrContext->GetFirstChildPos(bstrType, NULL, &lPos);

		while (lPos) 
		{
			if (FAILED(m_sptrContext->GetNextChild(bstrType, 0, &lPos, &punkObj)) || !punkObj)
				continue;

			// and add object
			if (!AddObject(punkObj, false))
				return false;

			if (punkObj)
				punkObj->Release();
		}
				// get active object of this type
		m_sptrContext->GetActiveObject(bstrType, &punkObj);
		IUnknownPtr sptrActive(punkObj);

		if (punkObj)
			punkObj->Release();

		// and expand this branch
		m_Tree.Expand(hParent, TVE_EXPAND);
		// set active object
		ChangeActive(sptrActive, true);
	}
	_catch
	{
		RemoveChilds(hParent);
		return false;
	}
	return true;
}

bool CContextTreeCtrl::ItemHasChildren(HTREEITEM hparent)
{
	ASSERT(hparent != NULL);

	if (hparent)
	{
		// get first item from parent
		HTREEITEM hitem = m_Tree.GetChildItem(hparent);
		
		if (hitem)
			return true;
	}
	return false;
}

//return selected object
/*IUnknown *CContextTreeCtrl::GetSelectedObject()
{
	HTREEITEM	hti = m_Tree.GetSelectedItem();
	if( !hti )
		return 0;

	return GetObjectByItem( hti );
}*/

// get name for any level item
LPCTSTR	CContextTreeCtrl::GetNameByItem(HTREEITEM hitem)
{
	ASSERT(hitem);
	if (!hitem)
		return NULL;

	CContextTreeData* pdata = (CContextTreeData*)m_Tree.GetItemData(hitem);
	if (!pdata)
		return NULL;

	return pdata->GetName();
}

// get Path for any level item
LPCTSTR	CContextTreeCtrl::GetPathByItem(HTREEITEM hitem)
{
	ASSERT(hitem);
	if (!hitem)
		return NULL;

	CContextTreeData* pdata = (CContextTreeData*)m_Tree.GetItemData(hitem);
	if (!pdata)
		return NULL;

	return pdata->GetPath();
}

// check hitem id type
bool CContextTreeCtrl::ItemIsType(HTREEITEM hitem)
{
	ASSERT(hitem);
	if (!hitem)
		return false;
	
	HTREEITEM hparent  = m_Tree.GetParentItem(hitem);
	return (hparent == TVI_ROOT || hparent == NULL) ? true : false;
}


int CALLBACK CContextTreeCtrl::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CString str1(((CContextTreeData*)lParam1)->GetName());
	CString str2(((CContextTreeData*)lParam2)->GetName());
	
	return (str1 < str2) ? -1 : 
			(str1 > str2) ? 1 : 0;
}


///////////////////////////////////////////////////////////////////////////////
// User events handlers
/*
void CContextTreeCtrl::OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	IUnknown * punkObj = NULL;
	// get selected item
	HTREEITEM hitem = m_Tree.GetSelectedItem();

	if (!hitem)
		return;
	// convertit to object
	if (!(punkObj = GetObjectByItem(hitem)))
		return;
	punkObj->Release();

	CString	strObject = GetNameByItem(hitem);

	strObject = CString("\"") + strObject + CString("\"");
	::ExecuteAction("ObjectSelect", strObject);//, const char *szActionparam);

	*pResult = 1;
}

void CContextTreeCtrl::OnRdblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}

void CContextTreeCtrl::OnReturnTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	IUnknown * punkObj = NULL;
	// get selected item
	HTREEITEM hitem = m_Tree.GetSelectedItem();

	if (!hitem)
		return;
	// convertit to object
	if (!(punkObj = GetObjectByItem(hitem)))
		return;

	punkObj->Release();

	CString	strObject = GetNameByItem(hitem);

	strObject = CString( "\"" )+strObject+"\"";
	::ExecuteAction("ObjectSelect", strObject);//, const char *szActionparam);

	*pResult = 0;
}

void CContextTreeCtrl::OnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;
	
//	m_strOldItemName = pTVDispInfo->item.pszText;
	// prevent edit types
	IUnknown* punk = GetObjectByItem(pTVDispInfo->item.hItem);
	*pResult = (punk) ? 0 : 1;
	if (punk)
		punk->Release();
}

void CContextTreeCtrl::OnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pTVDispInfo = (TV_DISPINFO*)pNMHDR;

	CString strNewName = pTVDispInfo->item.pszText;
	CString strOldName = GetNameByItem(pTVDispInfo->item.hItem);
	
	// check validity of new name
	if (strNewName == strOldName)
	{
		// name is not changed
		*pResult = 1;
		return;
	}
	if (!strNewName.IsEmpty())
	{
		// temporary
		CString strArg = CString("\"") + strOldName + CString ("\", \"") + strNewName + CString("\"");
		::ExecuteAction("ObjectRename", strArg, aefShowInterfaceByRequired);
		*pResult = 0;
		return;
	}

	*pResult = 0;
}
*/
void CContextTreeCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	*pResult = 0;
}

void CContextTreeCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	if (pNMTreeView->itemNew.lParam)
		m_pSelObjectData = (CContextTreeData*)pNMTreeView->itemNew.lParam;

	*pResult = 0;
}

