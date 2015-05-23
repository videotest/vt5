// ContextWrp.cpp : implementation file
//

#include "stdafx.h"
#include "ContextAX.h"
#include "ContextWrp.h"

#include "Utils.h"
#include "ObListWrp.h"
#include "NameConsts.h"
#include "TimeTest.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_INNER_TREE	0x1fff


IMPLEMENT_DYNCREATE(CContextWrpType, CWnd);
IMPLEMENT_DYNCREATE(CContextWrpObject, CContextWrpType);


/////////////////////////////////////////////////////////////////////////////
// CContextWrpObject

CContextWrpObject::CContextWrpObject()
{
	m_sptrContext = 0;
	m_sptrView = 0;
	m_sptrTypeMgr = 0;
	m_bEnableUpdateSelections = true;
	m_bSortByTime = true;

	m_mapTypes.RemoveAll();
	m_mapObjects.RemoveAll();
	m_mapBases.RemoveAll();
}

CContextWrpObject::~CContextWrpObject()
{
	m_sptrContext = 0;
	m_sptrView = 0;
	m_sptrTypeMgr = 0;
	m_bEnableUpdateSelections = true;

	m_mapTypes.RemoveAll();
	m_mapObjects.RemoveAll();
	m_mapBases.RemoveAll();
}

void CContextWrpObject::Init(IUnknown* punkView, IUnknown* punkData)
{
	if (!punkView)
		punkView = GetActiveContext(false);

	if (!punkData)
		punkData = GetActiveDocument(false);

	IDataContext2Ptr sptrC = GetContext(false);
	INamedDataPtr sptrD = GetNamedData(false);
	GuidKey	ContextKey = ::GetObjectKey(sptrC);
	GuidKey DataKey = ::GetObjectKey(sptrD);

	bool bNeedRestore = !(ContextKey != INVALID_KEY && ContextKey == ::GetObjectKey(punkView) &&
						  DataKey != INVALID_KEY && DataKey == ::GetObjectKey(punkData));

	if (bNeedRestore)
	{
		AttachData(punkView, punkData);
		ClearContens();
		AddAllObjects();
	}
	UpdateSelections();
	UpdateActive();

	if (bNeedRestore)
	{
		AttachData(sptrC, sptrD);
	}
}


BEGIN_MESSAGE_MAP(CContextWrpObject, CContextWrpType)
	//{{AFX_MSG_MAP(CContextWrpObject)
    ON_NOTIFY(CVN_DELETEITEM,		ID_INNER_TREE, OnDeleteItem)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CContextWrpObject message handlers

void CContextWrpObject::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCONTEXTVIEW * pnmcv	= (NMCONTEXTVIEW*)pNMHDR;
	*pResult = 0;

	HCONTEXTITEM hItem = pnmcv->itemNew;
	if (!hItem)
		return;

	CContextItemData * pData = (CContextItemData*)hItem->GetData();
	ASSERT (pData);
	if (pData)
	{
		if (ItemIsBase(hItem) && pData->GetKey() != INVALID_KEY)
			m_mapBases.RemoveKey(pData->GetKey());
		else if (pData->GetPointer(false))
			m_mapObjects.RemoveKey(::GetObjectKey(pData->GetPointer(false)));	
	}
	delete pData;

	hItem->SetData(0);
}

void CContextWrpObject::UpdateSelections()
{
	if (!m_bEnableUpdateSelections)
		return;

//	CTimeTest tt (true, "CContextWrpObject::UpdateSelections");
	DeselectAllObject();

	IDataContext2Ptr sptrC = m_sptrContext == 0 ? GetActiveView(false) : m_sptrContext;
	if (sptrC == 0)
		return;

	// get types count
	long nTypeCount = 0;
	sptrC->GetObjectTypeCount(&nTypeCount);

	// for all types
	for (long i = 0; i < nTypeCount; i++)
	{
		// get next object's type
		BSTR bstr = 0;
		sptrC->GetObjectTypeName(i, &bstr);
		_bstr_t bstrType(bstr, false);

		int nCount = 1;
		// get first position of selected objects
		LONG_PTR nPos = 0;
		if (FAILED(sptrC->GetFirstSelectedPos(bstrType, &nPos)) || !nPos)
			continue;

		// for all selected objects
		while (nPos)
		{
			// get object 
			IUnknownPtr sptrObj;
			if (FAILED(sptrC->GetNextSelected(bstrType, &nPos, &sptrObj)) || sptrObj == 0)
				continue;
			
//			int nNumber = 1;
//			sptrC->GetSelectedObjectNumber(_bstr_t(::GetObjectKind(sptrObj)), sptrObj, &nNumber);
//			ASSERT(nCount == nNumber);

			// set it to selected state  
			HCONTEXTITEM hItem = FindObjectItem(sptrObj);
			if (hItem)
				m_Tree.SetItemNumber(hItem, nCount);
			nCount++;
		} // continue to next type
	}// for all types on named_data
}


///////////////////////////////////////////////////////////////////////////////
// Objects functionality implementation

// clear contens of control
bool CContextWrpObject::ClearContens()
{
	// clear contens of tree control
	return RemoveAllBaseObjects();
}

void CContextWrpObject::SetSortByTime(bool bTime)
{
	if (m_bSortByTime == bTime)
		return;

	m_bSortByTime = bTime;
//	CTimeTest tt (true, "CContextWrpObject::SetSortByTime");

	BOOL bPrev = SetRedraw(false);
	if (m_Tree.GetCount())
	{
		HCONTEXTITEM hRoot = m_Tree.GetRootItem();
		for (POSITION pos = hRoot->GetFirstPos(); pos != NULL; )
		{
			HCONTEXTITEM hItem = hRoot->GetChild(pos);
			pos = hRoot->GetNextPos(pos);
			// sort children if need
			SortObjectsRecursively(hItem);
		}
		SortBases();
	}
	SetRedraw(bPrev);
	HCONTEXTITEM hFocus = m_Tree.GetItemFocus();
	if (hFocus)
		m_Tree.EnsureVisible(hFocus);
}


///////////////////////////////////////////////////////////////////////////////
// Type functionality implementation
// add Type - Zero level of tree
HCONTEXTITEM CContextWrpObject::AddBaseObject(GuidKey Key, bool bSort)
{
	// get NamedData
	INamedDataPtr sptrData = m_sptrTypeMgr;
	if (sptrData == 0)
		return 0;

	// get group base object 
	IUnknownPtr sptrBase;
	if (FAILED(sptrData->GetBaseGroupBaseObject(&Key, &sptrBase)))
		return 0;

	// if object does not exists
	if (sptrBase == 0)
	{
		// try to get first object in group (oldest)
		long lPos = 0;
		if (FAILED(sptrData->GetBaseGroupObjectFirstPos(&Key, &lPos)) || !lPos)
			return 0;

		sptrData->GetBaseGroupNextObject(&Key, &lPos, &sptrBase);
	}
	// if object does not exists
	if (sptrBase == 0)
		return 0;

	// get object name and creation order
	INumeredObjectPtr sptrNum = sptrBase;
	long lNum = -1;
	if (sptrNum)
		sptrNum->GetCreateNum(&lNum);

	CString strName = ::GetObjectName(sptrBase);

	// create item data
	CContextItemData * pdata = new CContextItemData(strName, Key, lNum);
	if (!pdata)
		return 0;

	// insert item
	HCONTEXTITEM hItem = m_Tree.InsertItem(pdata->GetName(), CVI_ROOT);
	if (!hItem)
		return 0;

	// and add data to item
	if (!m_Tree.SetItemData(hItem, (DWORD)pdata))
	{
		m_Tree.DeleteItem(hItem);
		return 0;
	}
	// set item to base map
	m_mapBases.SetAt(Key, hItem);
	
	// if need we'll sort types
	if (bSort)
		SortBases();

	hItem->SetExpand(true);

	return hItem;
}

// remove type with its children
bool CContextWrpObject::RemoveBaseObject(GuidKey Key)
{
	if (Key == INVALID_KEY)
		return false;

	// check exists
	HCONTEXTITEM hItem = GetBaseItem(Key);
	// if such type is not exists
	if (!hItem) 
		return true;

	// delete item	
	return m_Tree.DeleteItem(hItem) == TRUE;
}

// remove all types with its children (it's means remove all items from tree)
// RemoveAllItem
bool CContextWrpObject::RemoveAllBaseObjects()
{
	bool bRet = m_Tree.DeleteAllItems() == TRUE;
	
	m_mapTypes.RemoveAll();
	m_mapBases.RemoveAll();
	m_mapObjects.RemoveAll();
	return bRet;
}

// check item is type
bool CContextWrpObject::ItemIsBase(HCONTEXTITEM hItem)
{
	ASSERT(hItem);
	if (!hItem)
		return false;

	return m_Tree.ItemIsHeader(hItem) == TRUE;
}

// sort types
bool CContextWrpObject::SortBases()
{
//	CTimeTest tt(true, "CContextWrpObject::SortBases");
	CVSORTCB sort;
	sort.hParent = m_Tree.GetRootItem();
	sort.lpfnCompare = m_bSortByTime ?	CContextWrpType::CompareObjectFunc :
										CContextWrpType::CompareTypeFunc;

	sort.lParam = (LPARAM)this;

	return m_Tree.SortChildrenCB(&sort) == TRUE;
}

/// add all objects of given type
bool CContextWrpObject::AddAllObjects()
{
	CTimeTest tt(true, "CContextWrpObject::AddAllObjects");

	INamedDataPtr sptrData = m_sptrTypeMgr;
	if (sptrData == 0 || m_sptrContext == 0)
		return false;

	// add all groups 
	bool bRet = true;
/*	long lGroupPos = 0;
	sptrData->GetBaseGroupFirstPos(&lGroupPos);
	while (lGroupPos)
	{
		GuidKey Key;
		sptrData->GetNextBaseGroup(&Key, &lGroupPos);
		if (!AddBaseObject(Key, false))
			bRet = false;
	}
*/
	long nTypeCount = 0;
	m_sptrContext->GetObjectTypeCount(&nTypeCount);

	// for all types
	for (long nType = 0; nType < nTypeCount; nType++)
	{
		BSTR bstrType = 0;
		if (FAILED(m_sptrContext->GetObjectTypeName(nType, &bstrType)) || !bstrType)
		{
			bRet = false;
			continue;
		}
		_bstr_t bstrT(bstrType, false);
		// get type enables
		BOOL bEnable = false;
		m_sptrContext->GetTypeEnable(bstrT, &bEnable);


		// child objects
//		CString strType;
//		strType.Format("CContextWrpObject::AddAllObjects : type - %s", (LPCTSTR)bstrT);
//		CTimeTest tt(true, strType);

		// check context has objects of given type
		// we want to get only unparentned objects
		// because such object will add it's children itself
		long nObjCount = 0;
		m_sptrContext->GetChildrenCount(bstrT, 0, &nObjCount);
		if (!nObjCount)
			continue;
	
		long lObjPos = 0;
		m_sptrContext->GetFirstChildPos(bstrT, 0, &lObjPos);
		while (lObjPos)
		{
			IUnknownPtr sptr;
			if (FAILED(m_sptrContext->GetNextChild(bstrT, 0, &lObjPos, &sptr)) || sptr == 0)
				continue;

			INamedDataObject2Ptr sptrObj = sptr;
			GuidKey BaseKey;
			if (sptrObj == 0)
			{
				bRet = false;
				continue;
			}
			// Get base key for object and find parent's hItem
			sptrObj->GetBaseKey(&BaseKey);

			HCONTEXTITEM hBase = GetBaseItem(BaseKey);

//			ASSERT (hBase != NULL);
			if (!hBase && !(hBase = AddBaseObject(BaseKey, false)))
			{
				ASSERT (hBase != NULL);
				bRet = false;
				continue;
			}
			// and add object and sort it
			if (!AddObject(hBase, sptr, true))
				bRet = false;
		}
	}
	// sort bases
	if (!SortBases())
		bRet = false;

	return bRet;
}

// add object
bool CContextWrpObject::AddObject(HCONTEXTITEM hParent, IUnknown * punkObject, bool bSort)
{
	if (!hParent || !punkObject)
		return false;

	GuidKey ObjKey = ::GetObjectKey(punkObject);
	// try to get object
	HCONTEXTITEM hItem = FindObjectItem(punkObject);
	ASSERT (hItem == 0);
	// if already exists
	if (hItem)
		m_Tree.DeleteItem(hItem);

	// create item data
	CContextItemData* pdata = new CContextItemData(punkObject);
	if (!pdata)
		return false;

	// insert item
	if (!(hItem = m_Tree.InsertItem(pdata->GetName(), hParent)))
		return false;

	// and add data to item
	if (!m_Tree.SetItemData(hItem, (DWORD)pdata))
	{	
		m_Tree.DeleteItem(hItem);
		return false;
	}

	// set object to map
	m_mapObjects.SetAt(ObjKey, hItem);

	m_Tree.SetItemImage(hItem, -1);

	// check object is available in active view
	BOOL bAvailable = true;
	_bstr_t bstrType = ::GetObjectKind(punkObject);
	m_sptrContext->GetTypeEnable(bstrType, &bAvailable);

	// availability
	if (!bAvailable)
		m_Tree.SetItemState(hItem, m_Tree.GetItemState(hItem) | CVIS_DISABLED);

	
	// and add all children
	long	nCount = 0;
	// get object's children count (for children that are exists in data context)
	m_sptrContext->GetChildrenCount(bstrType, punkObject, &nCount);
	if (nCount)
	{
		long lChildPos = 0;
		m_sptrContext->GetFirstChildPos(bstrType, punkObject, &lChildPos);
		while (lChildPos)
		{
			IUnknownPtr	sptr;
			if (FAILED(m_sptrContext->GetNextChild(bstrType, punkObject, &lChildPos, &sptr)) || sptr == 0)
				continue;

			// and try add it to control
			if (!AddObject(hItem, sptr, false))
				return false;
		}
	}
	// sort children
	if (hItem->HasChildren())
		SortObjects(hItem);
/*
	if (ItemIsBase(hParent))
	{
		CContextItemData * pParentData = (CContextItemData *)hParent->GetData();
		if (pParentData && pParentData->GetKey() == ObjKey)
		{
			CString strName = ::GetObjectName(punkObject);
			// update parent item name
			m_Tree.SetItemText(hParent, pdata->GetName());
			pParentData->SetName(pdata->GetName());
		}
	}
*/
	// sort parent's children if need
	if (bSort)
		SortObjects(hParent);
	return true;
}

bool CContextWrpObject::AddObject(IUnknown * punkObject, bool bSort)
{
	INamedDataObject2Ptr sptrN = punkObject;
	if (sptrN == 0)
		return false;



	// check object has parent
	IUnknownPtr sptrUnkParent;
	sptrN->GetParent(&sptrUnkParent);
	INamedDataObject2Ptr sptrNParent = sptrUnkParent;

	
	HCONTEXTITEM hBase = 0;
	
	bool bObjectHasParent = false;
	// if parent exists
	if (sptrNParent != 0)
	{
		hBase = FindObjectItem(sptrUnkParent);
		if (!hBase && !AddObject(sptrUnkParent, false))
		{
			return false;
		}
		hBase = FindObjectItem(sptrUnkParent);
		bObjectHasParent = hBase != 0;
		
	}
	
	// if object has no parent or parent was not added
	bool bSortBases = false;
	if (!hBase)
	{
		// get object key
		GuidKey BaseKey;
		sptrN->GetBaseKey(&BaseKey);
		// try to get base item
		hBase = GetBaseItem(BaseKey);
		// if not found
		if (!hBase)
		{
			// add new base item
			hBase = AddBaseObject(BaseKey, false);
			bSortBases = true;
		}
	}

	ASSERT (hBase != 0);
	// add object
	bool bRet = AddObject(hBase, punkObject, bSort);
	if (!bRet)
		return false;
	ASSERT (bRet);

	if (!bObjectHasParent)
	{
		CContextItemData * pData = (CContextItemData *)hBase->GetData();
		INamedDataPtr sptrD = m_sptrTypeMgr;

		if (sptrD != 0 && pData)
		{
			IUnknownPtr sptrBase;
			GuidKey BaseKey;
			sptrN->GetBaseKey(&BaseKey);
			ASSERT(BaseKey != INVALID_KEY);

			sptrD->GetBaseGroupBaseObject(&BaseKey, &sptrBase);
			if (::GetObjectKey(sptrBase) == ::GetObjectKey(sptrN))
			{
				CString strName = ::GetObjectName(punkObject);
				// update parent item name
				m_Tree.SetItemText(hBase, strName);
				pData->SetName(strName);
				bSortBases = true;
			}
		}
	}
	if (bSortBases)
		bRet = SortBases();

	return bRet;
}


// change object's base key
bool CContextWrpObject::ChangeBaseObject(IUnknown * punkObject)
{
	INamedDataObject2Ptr sptrObj = punkObject;
	if (sptrObj == 0)
		return false;

	GuidKey BaseKey = INVALID_KEY;
	sptrObj->GetBaseKey(&BaseKey);
	GuidKey ObjKey = ::GetObjectKey(sptrObj);

	// get context item from unknown
	HCONTEXTITEM hItem = FindObjectItem(punkObject);
	if (!hItem)
		return false;

	bool bNeedSort = false;
	HCONTEXTITEM hNewBase = GetBaseItem(BaseKey);
	HCONTEXTITEM hParent = hItem->GetParent();
	if (hParent && ItemIsBase(hParent) && hNewBase == hParent)
		return true;

	if (!m_Tree.DeleteItem(hItem))
		return false;

	if (hParent && ItemIsBase(hParent))
	{
		// if type_item has no objects -> remove type
		if (!hParent->HasChildren())
			m_Tree.DeleteItem(hParent);
	}

	if (!AddObject(punkObject, true))
		return false;

	UpdateSelections();
	UpdateActive();

	return true;
}
/*
// change object's key
bool CContextWrpObject::ChangeObjectKey(IUnknown* punkObject, GuidKey & PrevKey)
{
	if (!punkObject || PrevKey == INVALID_KEY)
		return false;

	GuidKey NewKey = ::GetObjectKey(punkObject);
	if (NewKey == INVALID_KEY || NewKey == PrevKey)
		return true;

	HCONTEXTITEM hItem = NULL;
	if (!m_mapObjects.Lookup(PrevKey, hItem) || !hItem)
		return false;

	m_mapObjects.RemoveKey(PrevKey);
	m_mapObjects.SetAt(NewKey, hItem);

	return true;
}
*/


// remove object
bool CContextWrpObject::RemoveObject(IUnknown * punkObject)
{
	if (!punkObject)
		return false;

	// get context item from unknown
	HCONTEXTITEM hItem = FindObjectItem(punkObject);
	if (!hItem)
		return false;

	// get parent item (type) for object
	HCONTEXTITEM hBaseItem = hItem->GetParent();
	// and remove item
	m_Tree.DeleteItem(hItem);

	// if parent item is type_item and valid
	if (hBaseItem && ItemIsBase(hBaseItem))
	{
		// if type_item has no objects -> remove type
		if (!hBaseItem->HasChildren())
			m_Tree.DeleteItem(hBaseItem);
	}
	return true;
}


bool CContextWrpObject::DeselectAllObject()
{
	IDataContext2Ptr sptrC = m_sptrContext != 0 ? m_sptrContext : GetActiveContext(false);
	if (sptrC == 0)
		return false;
//	CTimeTest tt (true, "CContextWrpObject::DeselectAllObject()");
	
	for (POSITION pos = m_mapObjects.GetStartPosition(); pos != NULL; )
	{
		GuidKey Key;
		HCONTEXTITEM hItem = 0;
		m_mapObjects.GetNextAssoc(pos, Key, hItem);
		if (hItem)
		{
			m_Tree.SetItemNumber(hItem, -1);

			CContextItemData * pData = (CContextItemData *)hItem->GetData();
			BOOL bEnable = false;
			if (pData && pData->GetPointer(false))
				sptrC->GetObjectEnable(pData->GetPointer(false), &bEnable);

			DWORD dwState = hItem->GetState();
			if (bEnable)
				dwState &= ~CVIS_DISABLED;
			else
				dwState |= CVIS_DISABLED;

			hItem->SetState(dwState);
		}
	}
	
	return true;
}

bool CContextWrpObject::DeSelectChildObject(IDataContext2 * pContext, HCONTEXTITEM hItem)
{
	if (!hItem)
		return true;

	m_Tree.SetItemNumber(hItem, -1);

	CContextItemData * pData = (CContextItemData *)hItem->GetData();
	BOOL bEnable = false;
	if (pData && pData->GetPointer(false) && pContext)
		pContext->GetObjectEnable(pData->GetPointer(false), &bEnable);

	DWORD dwState = hItem->GetState();
	if (bEnable)
		dwState &= ~CVIS_DISABLED;
	else
		dwState |= CVIS_DISABLED;

	hItem->SetState(dwState);

	for (POSITION pos = hItem->GetFirstPos(); pos != NULL; )
	{
		HCONTEXTITEM hChild = hItem->GetChild(pos);
		DeSelectChildObject(pContext, hChild);
		pos = hItem->GetNextPos(pos);
	}
	return true;
}



///////////////////////////////////////////////////////////////////////////////////
// utilities functionality

// return context item of first level(type) by given type
HCONTEXTITEM CContextWrpObject::GetBaseItem(GuidKey Key)
{
	if (Key == INVALID_KEY)
		return 0;

	HCONTEXTITEM hItem = 0;
	if (!m_mapBases.Lookup(Key, hItem))
		return 0;

	return hItem;
}


void CContextWrpObject::SetNumeric()
{
/*	CTimeTest tt (true, "CContextWrpObject::SetNumeric()");
	HCONTEXTITEM hFocusItem = m_Tree.GetItemFocus();
	if (!hFocusItem)
		return;

	// get type item
	HCONTEXTITEM hType = hFocusItem;
	while (hType && hType->GetParent() != m_Tree.GetRootItem())
		hType = hType->GetParent();

	// get object's type
	CContextItemData * pData = (CContextItemData*)m_Tree.GetItemData(hType);
	if (!pData)
		return;

	GuidKey BaseKey = pData->GetKey();

	// get context
	IDataContext2Ptr sptrC = m_sptrContext != 0 ? m_sptrContext : GetActiveContext(false);
	if (sptrC == 0)
		return;

	// get NamedData
	IUnknownPtr sptrUnkData;
	sptrC->GetData(&sptrUnkData);
	INamedDataPtr sptrData = sptrUnkData;
	if (sptrData == 0)
		return;

	sptrUnkData = 0;
	m_bEnableUpdateSelections = false;
	BOOL bPrevRedraw = SetRedraw(false);

	BOOL bPrevLock = false;
	sptrC->GetLockUpdate(&bPrevLock);	
	sptrC->LockUpdate(true, false);

	_try (CContextWrpObject, SetNumeric)
	{
		long lPos = 0;
		sptrData->GetBaseGroupObjectFirstPos(&BaseKey, &lPos);
		while (lPos)
		{
			IUnknownPtr sptrObj;
			sptrData->GetBaseGroupNextObject(&BaseKey, &lPos, &sptrObj);
			sptrC->SetObjectSelect(sptr, true);
		}
			
		sptrC = 0;
		sptrData = 0;
	}
	_catch;
	
	m_bEnableUpdateSelections = true;
	sptrC->LockUpdate(bPrevLock, false);

	UpdateSelections();

	m_Tree.SetItemFocus(hFocusItem);
	SetRedraw(bPrevRedraw);
*/
}


void CContextWrpObject::RemoveNumeric()
{
/*
	CTimeTest tt (true, "CContextWrpObject::RemoveNumeric()");
	HCONTEXTITEM hFocusItem = m_Tree.GetItemFocus();
	if (!hFocusItem)
		return;

	// get type item
	HCONTEXTITEM hType = hFocusItem;
	while (hType && hType->GetParent() != m_Tree.GetRootItem())
		hType = hType->GetParent();

	// get object's type
	CContextItemData * pData = (CContextItemData*)m_Tree.GetItemData(hType);
	if (!pData)
		return;

	GuidKey BaseKey = pData->GetKey();

	// get context
	IDataContext2Ptr sptrC = m_sptrContext != 0 ? m_sptrContext : GetActiveContext(false);
	if (sptrC == 0)
		return;

	// get NamedData
	IUnknownPtr sptrUnkData;
	sptrC->GetData(&sptrUnkData);
	INamedDataPtr sptrData = sptrUnkData;
	if (sptrData == 0)
		return;

	BOOL bPrevRedraw = SetRedraw(false);
	m_bEnableUpdateSelections = false;

	BOOL bPrevLock = false;
	sptrC->GetLockUpdate(&bPrevLock);	
	sptrC->LockUpdate(true, false);
	
	_try (CContextWrpObject, RemoveNumeric)
	{
		// get count of objects of given type
		int nCount = 0;
		sptrC->GetSelectedCount(bstrType, &nCount);

		if (nCount > 1)
		{
			int nPos = 0;

			IUnknown * punkObject = 0;
			// first selected object is active object
			// so we neeed to skip it (request by technologist)
			sptrC->GetFirstSelectedPos(bstrType, &nPos);
			if (nPos)
			{
				sptrC->GetNextSelected(bstrType, &nPos, &punkObject);
				if (punkObject)
				{
					punkObject->Release();
					punkObject = 0;
				}
			}

			// from second selected object and further
			while (nPos)
			{
				// get object
				sptrC->GetNextSelected(bstrType, &nPos, &punkObject);
				if (punkObject)
				{
					// and deselect it
					sptrC->SetObjectSelect(punkObject, false);
					punkObject->Release();
					punkObject = 0;
				}
			}
		}
		sptrC = 0;
	}
	_catch;

	m_bEnableUpdateSelections = true;
	sptrC->LockUpdate(bPrevLock, false);
	UpdateSelections();


	m_Tree.SetItemFocus(hFocusItem);
	SetRedraw(bPrevRedraw);
*/
}



/////////////////////////////////////////////////////////////////////////////
// CContextWrpType

CContextWrpType::CContextWrpType()
{
	m_sptrView = NULL;
	m_sptrContext = NULL;
	m_sptrTypeMgr = NULL;
	m_bEnableUpdateSelections = true;
	m_bSortByTime = true;

	m_mapTypes.RemoveAll();
	m_mapObjects.RemoveAll();
}

CContextWrpType::~CContextWrpType()
{
	m_sptrView = NULL;
	m_sptrContext = NULL;
	m_sptrTypeMgr = NULL;
	m_bEnableUpdateSelections = true;

	m_mapTypes.RemoveAll();
	m_mapObjects.RemoveAll();
}


BEGIN_MESSAGE_MAP(CContextWrpType, CWnd)
	//{{AFX_MSG_MAP(CContextWrpType)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_DESTROY()
    ON_NOTIFY(CVN_ACTIVATE,			ID_INNER_TREE, OnActivateItem)
    ON_NOTIFY(CVN_ENDLABELEDIT,		ID_INNER_TREE, OnEndLabelEdit)
    ON_NOTIFY(CVN_BEGINLABELEDIT,	ID_INNER_TREE, OnBeginLabelEdit)
    ON_NOTIFY(CVN_DELETEITEM,		ID_INNER_TREE, OnDeleteItem)
    ON_NOTIFY(CVN_NUMBER,			ID_INNER_TREE, OnNumberItem)
	ON_WM_DRAWITEM()
	ON_WM_SETTINGCHANGE()
	ON_WM_SYSCOLORCHANGE()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CContextWrpType::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID) 
{
	return CWnd::Create(AfxRegisterWndClass(CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW, 0, (HBRUSH)(COLOR_3DFACE + 1), 0), 
						"", dwStyle | WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, rect, pParentWnd, nID, NULL);
}

/*
BOOL CContextWrpType::Attach(DWORD dwStyle, CWnd* pParentWnd, UINT nID, bool bDynamic)
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

	BOOL bRet = Create(dwStyle, rcOld, pParentWnd, nID);
	ASSERT(bRet);

	ModifyStyleEx(0, WS_EX_CLIENTEDGE);
	SetWindowPos(pCtrlToReplace, 
				 rcOld.left, rcOld.top, rcOld.Width(), rcOld.Height(), 
		         SWP_SHOWWINDOW|SWP_NOREDRAW|SWP_NOACTIVATE);
	
	//Destroy the old control...we don't need it anymore
	pCtrlToReplace->DestroyWindow();

	UpdateWindow();

	return bRet;
}
*/

// attach data and view and context
bool CContextWrpType::AttachData(IUnknown* punkView, IUnknown* punkData)
{
	// release interfaces
	m_sptrContext = 0;
	m_sptrView = 0;
	m_sptrTypeMgr = 0;

	if (!punkView && !punkData)
		return true;

	if (punkView && CheckInterface(punkView, IID_IDataContext2))
	{
		m_sptrContext	= punkView; 
		m_sptrView		= punkView; 
	}
	
	if (punkData && CheckInterface(punkData, IID_IDataTypeManager))
		m_sptrTypeMgr = punkData; 

	if (!punkView && m_sptrTypeMgr != 0)
	{
		// try to get context by NamedData
		IUnknown* punk = 0;
		sptrIDocumentSite sptrD = m_sptrTypeMgr;
		if (FAILED(sptrD->GetActiveView(&punk)) || !punk)
			return false;

		m_sptrContext	= punk;
		m_sptrView		= punk;
		punk->Release();
	} 
	else if (!punkData && m_sptrView != NULL)
	{
		// try to get NamedData by view
		IUnknown* punk = 0;
		if (FAILED(m_sptrView->GetDocument(&punk)) || !punk)
			return false;

		m_sptrTypeMgr	= punk;
		punk->Release();
	}

	return true;
}

void CContextWrpType::Init(IUnknown* punkView, IUnknown* punkData)
{
	if (!punkView)
		punkView = GetActiveContext(false);

	if (!punkData)
		punkData = GetActiveDocument(false);

	IDataContext2Ptr sptrC = GetContext(false);
	INamedDataPtr sptrD = GetNamedData(false);
	GuidKey ContextKey = ::GetObjectKey(sptrC);
	GuidKey DataKey = ::GetObjectKey(sptrD);

	bool bNeedRestore = !(ContextKey != INVALID_KEY && ContextKey == ::GetObjectKey(punkView) &&
						  DataKey != INVALID_KEY && DataKey == ::GetObjectKey(punkData));

	if (bNeedRestore)
	{
		AttachData(punkView, punkData);
		ClearContens();
		AddAllTypes(true);
	}

	UpdateSelections();
	UpdateActive();

	if (bNeedRestore)
	{
		AttachData(sptrC, sptrD);
	}

}

IUnknown * CContextWrpType::GetContext(bool bAddRef)
{
	IUnknownPtr sptrC = m_sptrContext;
	if (sptrC != 0 && bAddRef)
		sptrC.AddRef();

	return sptrC.GetInterfacePtr();
}

IUnknown * CContextWrpType::GetNamedData(bool bAddRef)
{
	IUnknownPtr sptrD = m_sptrTypeMgr;
	if (sptrD != 0 && bAddRef)
		sptrD.AddRef();

	return sptrD.GetInterfacePtr();
}


IUnknown * CContextWrpType::GetActiveContext(bool bAddRef)
{
	IApplicationPtr sptrA = ::GetAppUnknown(false);
	if (sptrA == 0)
		return 0;

	IUnknown * punkDoc = 0;
	sptrA->GetActiveDocument(&punkDoc);
	if (!punkDoc)
		return 0;
	INamedDataPtr sptrND = punkDoc;
	punkDoc->Release();

	if (sptrND == 0)
		return 0;

	IUnknown * punkContext = 0;
	sptrND->GetActiveContext(&punkContext);
	if (punkContext && !bAddRef)
		punkContext->Release();

	return punkContext;
}

IUnknown * CContextWrpType::GetActiveView(bool bAddRef)
{
	IApplicationPtr sptrA = ::GetAppUnknown(false);
	if (sptrA == 0)
		return 0;

	IUnknown * punkDoc = 0;
	sptrA->GetActiveDocument(&punkDoc);
	if (!punkDoc)
		return 0;
	IDocumentSitePtr sptrDS = punkDoc;
	punkDoc->Release();

	if (sptrDS == 0)
		return 0;

	IUnknown * punkView = 0;
	sptrDS->GetActiveView(&punkView);
	if (punkView && !bAddRef)
		punkView->Release();
	
	return punkView;
}

IUnknown * CContextWrpType::GetActiveDocument(bool bAddRef)
{
	IApplicationPtr sptrA = ::GetAppUnknown(false);
	if (sptrA == 0)
		return 0;

	IUnknown * punkDoc = 0;
	sptrA->GetActiveDocument(&punkDoc);
	if (punkDoc && !bAddRef)
		punkDoc->Release();

	return punkDoc;
}


/////////////////////////////////////////////////////////////////////////////
// CContextWrpType message handlers

int CContextWrpType::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	CRect rc;
	GetClientRect(&rc);

	if (!m_Tree.Create(rc, this, ID_INNER_TREE))
		return -1;
	
	// and addition style
	ModifyStyle(WS_BORDER, 0);
	ModifyStyleEx(0, WS_EX_CLIENTEDGE, SWP_NOACTIVATE);
/*
	if (!m_ImageList.Create(IDB_CONTEXT_BITMAP, 32, 1, GetSysColor(COLOR_3DFACE)))
		return -1;

	m_Tree.SetImageList(&m_ImageList);
*/
	m_sptrThumbMan = GetAppUnknown();
	return 0;
}

void CContextWrpType::OnSetFocus(CWnd* pOldWnd) 
{
	CWnd::OnSetFocus(pOldWnd);
	m_Tree.SetFocus();	
}

void CContextWrpType::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);
	
	CRect rect;
	GetClientRect(&rect);

	m_Tree.MoveWindow(rect, false);
}

void CContextWrpType::OnDestroy() 
{
	ClearContens();
	CWnd::OnDestroy();
}

void CContextWrpType::OnSettingChange(UINT uFlags, LPCTSTR lpszSection) 
{
	CWnd::OnSettingChange(uFlags, lpszSection);
}

void CContextWrpType::OnSysColorChange() 
{
	CWnd::OnSysColorChange();
}

void CContextWrpType::OnActivateItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCONTEXTVIEW * pnmcv	= (NMCONTEXTVIEW*)pNMHDR;
	*pResult = 0;

	SCROLLINFO si;
	memset(&si, 0, sizeof (SCROLLINFO));
	BOOL bScrollSave = SaveScrollInfo(&si);
	
	// get activated item
	HCONTEXTITEM hItem = pnmcv->itemNew;
	if (!hItem || m_Tree.ItemIsHeader(hItem))
		return;

	CContextItemData * pData = (CContextItemData *)hItem->GetData();
	if (!pData)
		return;

	INamedDataObject2Ptr    sptrObj = pData->GetPointer(false);
	if (sptrObj == 0)
		return;

	bool bPrevSelected = (hItem->GetState() & CVIS_SELECTED) == CVIS_SELECTED;
	CString	strObject = pData->GetName();

	IDataContext2Ptr sptrC = GetActiveView(false);
	if (sptrC == 0)
	{
		*pResult = ::ExecuteAction("ObjectSelect", CString("\"") + strObject + CString("\""));

		if (!*pResult && bScrollSave)
			RestoreScrollInfo(&si);

		SetFocusObject(sptrObj);
		if (CWnd::GetFocus() != this)
			SetFocus();

		return;
	}

	// if item is not actived -> activate it
	_bstr_t bstrType = ::GetObjectKind(sptrObj);

	IUnknownPtr sptrActive;
	sptrC->GetActiveObject(bstrType, &sptrActive);

	bool bOldActive = ::GetObjectKey(sptrActive) == ::GetObjectKey(sptrObj);
	if (bOldActive && bPrevSelected)
	{
		long nSelCount = 0;
		sptrC->GetSelectedCount(bstrType, &nSelCount);

		if (nSelCount < 2)
		{
			sptrC->SetActiveObject(bstrType, 0, aofActivateDepended);
			bScrollSave = false;
			*pResult = 1;
		}
		else
		{
			sptrC->SetObjectSelect(sptrActive, false);
			IUnknownPtr sptrNew(::GetActiveObjectFromContextEx(sptrC), false);
			if (sptrNew != 0)
				bScrollSave = false;
//			strObject = ::GetObjectName(sptrNew);
//			*pResult = strObject.IsEmpty() ? 0 : ::ExecuteAction("ObjectSelect", CString("\"") + strObject + CString("\""));
		}
	}
	else
	{
		sptrC = 0;
		strObject = ::GetObjectName(sptrObj);
		*pResult = strObject.IsEmpty() ? 0 : ::ExecuteAction("ObjectSelect", CString("\"") + strObject + CString("\""));
	}

	SetFocusObject(sptrObj);
	if (CWnd::GetFocus() != this)
		SetFocus();

	if (bScrollSave)
		RestoreScrollInfo(&si);
}

void CContextWrpType::OnNumberItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCONTEXTVIEW * pnmcv	= (NMCONTEXTVIEW*)pNMHDR;
	*pResult = 0;

	HCONTEXTITEM hItem = pnmcv->itemNew;
	if (!hItem || m_Tree.ItemIsHeader(hItem) || m_Tree.IsItemSelected(hItem))
		return;

	CContextItemData * pData = (CContextItemData*)m_Tree.GetItemData(hItem);
	if (!pData)
		return;

	// get object and context pointers
	INamedDataObject2Ptr sptrObj = pData->GetPointer(false);
	IDataContext2Ptr	 sptrC = GetActiveView(false);
	if (sptrObj == 0 || sptrC == 0)
		return;

//	if we have no active item now, we need to activate
	_bstr_t bstrType = ::GetObjectKind(sptrObj);

	long nCount = 0;
	sptrC->GetSelectedCount(bstrType, &nCount);
	if (!nCount)
	{
		sptrC = 0;
		CString strObject = ::GetObjectName(sptrObj);
		*pResult = strObject.IsEmpty() ? 0 : ::ExecuteAction("ObjectSelect", CString("\"") + strObject + CString("\""));
	}
	else
	{
		// get object is select
		BOOL bSelected = false;
		sptrC->GetObjectSelect(sptrObj, &bSelected);
		// set object select
		sptrC->SetObjectSelect(sptrObj, !bSelected);
		sptrC = 0;
	}

	SetFocusObject(sptrObj);
	if (CWnd::GetFocus() != this)
		SetFocus();
}
   
void CContextWrpType::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCONTEXTVIEW * pnmcv	= (NMCONTEXTVIEW*)pNMHDR;
	*pResult = 0;

	HCONTEXTITEM hItem = pnmcv->itemNew;
	if (!hItem)
		return;

	CContextItemData * pData = (CContextItemData*)hItem->GetData();
	ASSERT (pData);
	if (pData)
	{
		if (ItemIsType(hItem) && pData->GetName())
			m_mapTypes.RemoveKey(pData->GetName());
		else if (pData->GetPointer(false))
			m_mapObjects.RemoveKey(::GetObjectKey(pData->GetPointer(false)));	
	}
	delete pData;

	hItem->SetData(0);
}

void CContextWrpType::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCVDISPINFO * pnmcvdisp = (NMCVDISPINFO*)pNMHDR;

	*pResult = 0;
	
	// get item
	HCONTEXTITEM hItem = pnmcvdisp->item.hItem;

	if (!hItem || m_Tree.ItemIsHeader(hItem))
		return;

	CString strNewName = pnmcvdisp->item.strText;
	CContextItemData * pData = (CContextItemData *)m_Tree.GetItemData(hItem);
	CString strOldName = pData ? pData->GetName() : strNewName;

	if (!pData || strNewName == strOldName)
	{
		*pResult = 1;
		return; 
	}
	
	INamedDataObject2Ptr sptrObj = pData->GetPointer(false);
	SCROLLINFO si;
	memset(&si, 0, sizeof (SCROLLINFO));
	BOOL bScrollSave = SaveScrollInfo(&si);
	BOOL bPrevRedraw = SetRedraw(false);

	if (!strNewName.IsEmpty())
	{
		IDataContext2Ptr sptrC = GetActiveContext(false);
		bool bActive = false;
		bool bSelect = false;
		if (sptrC)
		{
			IUnknownPtr sptrActive;
			sptrC->GetActiveObject(_bstr_t(::GetObjectKind(sptrObj)), &sptrActive);
			bActive = ::GetObjectKey(sptrActive) == ::GetObjectKey(sptrObj);
			BOOL bSel = false;
			sptrC->GetObjectSelect(sptrObj, &bSel);
			bSelect = bSel == TRUE;
			sptrC = 0;
		}

		CString strArg = CString("\"") + strOldName + CString ("\", \"") + strNewName + CString("\"");
		*pResult = ::ExecuteAction("ObjectRename", strArg, aefShowInterfaceByRequired);

		sptrC = GetActiveContext(false);
		if (sptrC)
		{
			if (bActive)
				sptrC->SetActiveObject(0, sptrObj, aofActivateDepended);
			else if (bSelect)
				sptrC->SetObjectSelect(sptrObj, true);
		}

	}
	if (*pResult)
	{
		UpdateSelections();
		UpdateActive();
		if (bScrollSave)
			RestoreScrollInfo(&si);
	}

	SetFocusObject(sptrObj);
	if (CWnd::GetFocus() != this)
		SetFocus();

	bPrevRedraw = SetRedraw(bPrevRedraw, true);
}

void CContextWrpType::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMCVDISPINFO * pnmcvdisp = (NMCVDISPINFO*)pNMHDR;
	HCONTEXTITEM hItem = pnmcvdisp->item.hItem;

	*pResult = 0;

	if (!hItem || m_Tree.ItemIsHeader(hItem))
		return;

	CContextItemData * pData = (CContextItemData *)hItem->GetData();
	if (!pData)
		return;

	*pResult = pData->GetPointer(false) ? 1 : 0;
}

BOOL CContextWrpType::OnEraseBkgnd(CDC* pDC) 
{
	// TODO: Add your message handler code here and/or call default
	
	return true;//CWnd::OnEraseBkgnd(pDC);
}
BOOL CContextWrpType::SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars)
{
//	m_Tree.EnableWindow(bAllowDraw);
	return m_Tree.SetRedraw(bAllowDraw, bResetScrollBars);
}

BOOL CContextWrpType::SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
{
	return ::IsWindow(m_Tree.GetSafeHwnd()) ? m_Tree.SetScrollInfo(nBar, lpScrollInfo, bRedraw) : false;
}

BOOL CContextWrpType::GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask)
{
	return ::IsWindow(m_Tree.GetSafeHwnd()) ? m_Tree.GetScrollInfo(nBar, lpScrollInfo, nMask) : false;
}


///////////////////////////////////////////////////////////////////////////////
// inner functionality implementation
// static callback compare functions
// for type compare
int CALLBACK CContextWrpType::CompareTypeFunc(HCONTEXTITEM hItem1, HCONTEXTITEM hItem2, LPARAM lParamSort)
{
	return lstrcmpi(hItem1->GetText(), hItem2->GetText());
}

// for object compare
int CALLBACK CContextWrpType::CompareObjectFunc(HCONTEXTITEM hItem1, HCONTEXTITEM hItem2, LPARAM lParamSort)
{
	ASSERT (hItem1);
	ASSERT (hItem2);

	CContextItemData * pData1 = (CContextItemData*)hItem1->GetData();
	CContextItemData * pData2 = (CContextItemData*)hItem2->GetData();

	if (!pData1 || !pData2)
		return 0;

	int nRet = pData1->GetSortNum() - pData2->GetSortNum();
	ASSERT (nRet != 0);
	return nRet;
}

// clear contens of control
bool CContextWrpType::ClearContens()
{
	// clear contens of tree control
	return RemoveAllTypes();
}

void CContextWrpType::SetSortByTime(bool bTime)
{
	if (m_bSortByTime == bTime)
		return;

	m_bSortByTime = bTime;
//	CTimeTest tt (true, "CContextWrpType::SetSortByTime");
	BOOL bPrev = SetRedraw(false);
	if (m_Tree.GetCount())
	{
		HCONTEXTITEM hRoot = m_Tree.GetRootItem();
		for (POSITION pos = hRoot->GetFirstPos(); pos != NULL; )
		{
			HCONTEXTITEM hItem = hRoot->GetChild(pos);
			pos = hRoot->GetNextPos(pos);
			// sort children if need
			SortObjectsRecursively(hItem);
		}
		SortTypes();
	}
	SetRedraw(bPrev);
	HCONTEXTITEM hFocus = m_Tree.GetItemFocus();
	if (hFocus)
		m_Tree.EnsureVisible(hFocus);
}

///////////////////////////////////////////////////////////////////////////////
// Type functionality implementation

// fill all types w/ children
bool CContextWrpType::AddAllTypes(bool bSort)
{
	CTimeTest tt (true, "CContextWrpType::AddAllTypes");
	bool bRet = true;

	if (m_sptrContext == 0)
		return false;

	// get type count
	long nTypeCount = 0;
	m_sptrContext->GetObjectTypeCount(&nTypeCount);

	// get all types
	for (long nType = 0; nType < nTypeCount; nType++)
	{
		// get type name
		BSTR bstrTypeName = 0;
		if (FAILED(m_sptrContext->GetObjectTypeName(nType, &bstrTypeName)) || !bstrTypeName)
		{
			bRet = false;
			continue;
		}

		_bstr_t bstrtype(bstrTypeName, false);
		// type enables
		BOOL bTypeEnable = false;
		m_sptrContext->GetTypeEnable(bstrTypeName, &bTypeEnable);

		// check context has objects of given type
		long nObjCount = 0;
		if (FAILED(m_sptrContext->GetObjectCount(bstrtype, &nObjCount)) || !nObjCount)
			continue;

		// add type for last usage
		HCONTEXTITEM hType = AddType(bstrtype, bTypeEnable == TRUE);
		if (!hType)
		{
			bRet = false;
			continue;
		}

		// if need we'll add all objects of this type
		if (!AddAllObjects(hType, bstrtype, bTypeEnable == TRUE, true))
			bRet = false;
	}
	{
		if (bSort && !SortTypes())
			bRet = false;
	}
	return bRet;
}

// add Type - Zero level of tree
HCONTEXTITEM CContextWrpType::AddType(LPCTSTR szType, bool bEnable)
{
	CString strType(szType);
	if (strType.IsEmpty())
		return 0;

	// create item data
	CContextItemData * pdata = new CContextItemData(strType, bEnable);
	if (!pdata)
		return NULL;

	// insert item
	HCONTEXTITEM hItem = m_Tree.InsertItem(strType, CVI_ROOT);
	if (!hItem)
		return NULL;

	// and add data to item
	if (!m_Tree.SetItemData(hItem, (DWORD)pdata))
	{
		m_Tree.DeleteItem(hItem);
		return NULL;
	}
	m_mapTypes.SetAt(strType, hItem);
	hItem->SetExpand(true);

	return hItem;
}

// remove type with its children
bool CContextWrpType::RemoveType(LPCTSTR szType)
{
	CString strType(szType);

	// check exists
	HCONTEXTITEM hitem = GetTypeItem(strType);
	// if such type is not exists
	if (!hitem) 
		return true;

	// delete item	
	return m_Tree.DeleteItem(hitem) == TRUE;
}

// remove all types with its children (it's means remove all items from tree)
// RemoveAllItem
bool CContextWrpType::RemoveAllTypes()
{
	bool bRet = m_Tree.DeleteAllItems() == TRUE;
	
	m_mapTypes.RemoveAll();
	m_mapObjects.RemoveAll();
	return bRet;
}

// check item is type
bool CContextWrpType::ItemIsType(HCONTEXTITEM hItem)
{
	if (!hItem)
		return false;

	return m_Tree.ItemIsHeader(hItem) == TRUE;
}

// sort types
bool CContextWrpType::SortTypes()
{
	CVSORTCB sort;
	sort.hParent = m_Tree.GetRootItem();
	sort.lpfnCompare = CContextWrpType::CompareTypeFunc;
	sort.lParam = (LPARAM)this;

	return m_Tree.SortChildrenCB(&sort) == TRUE;
}


///////////////////////////////////////////////////////////////////////////////
// Objects functionality implementation

/// add all objects of given type
bool CContextWrpType::AddAllObjects(HCONTEXTITEM hParent, BSTR bstrType, bool bEnable, bool bSort)
{
//	CTimeTest tt (true, "CContextWrpType::AddAllObjects");

	if (!bstrType || !hParent || m_sptrContext == 0)
		return false;

	bool bRet = true;
	// get objects count
	long nCount = 0;
	m_sptrContext->GetChildrenCount(bstrType, 0, &nCount);

	if (nCount)
	{
		long lChildPos = 0;
		m_sptrContext->GetFirstChildPos(bstrType, 0, &lChildPos);
		while (lChildPos)
		{
			IUnknownPtr sptr;			
			// get object pointer
			if (FAILED(m_sptrContext->GetNextChild(bstrType, NULL, &lChildPos, &sptr)) || sptr == 0)
				continue;

			if (!AddObject(hParent, sptr, bEnable))
				bRet = false;
		}
	}

	// sort children if need
	if (bSort && !SortObjects(hParent))
		bRet = false;

	return bRet;
}

// add object
bool CContextWrpType::AddObject(HCONTEXTITEM hParent, IUnknown* punkObject, bool bEnable)
{
	if (!punkObject || !hParent)
		return false;

	bool bRet = true;

	// get object name
	CString strName = ::GetObjectName(punkObject);

	// try to get object
	HCONTEXTITEM hItem = FindObjectItem(punkObject);
	ASSERT (hItem == 0);
	// if already exists
	if (hItem)
		m_Tree.DeleteItem(hItem);

	// create item data
	CContextItemData* pdata = new CContextItemData(punkObject);
	if (!pdata)
		return false;

	// insert item
	if (!(hItem = m_Tree.InsertItem(strName, hParent)))
		return false;

	// and add data to item
	if (!m_Tree.SetItemData(hItem, (DWORD)pdata))
	{	
		m_Tree.DeleteItem(hItem);
		return false;
	}
	// set object to map
	m_mapObjects.SetAt(::GetObjectKey(punkObject), hItem);

	m_Tree.SetItemImage(hItem, -1);

	// availability
	if (!bEnable)
		m_Tree.SetItemState(hItem, m_Tree.GetItemState(hItem) | CVIS_DISABLED);

	// and add all children
	_bstr_t	bstrType = GetObjectKind(punkObject);
	long	nCount = 0;
	// get object's children count (for children that are exists in data context)
	m_sptrContext->GetChildrenCount(bstrType, punkObject, &nCount);
	if (nCount)
	{
		// for all children 
		long lChildPos = 0;
		m_sptrContext->GetFirstChildPos(bstrType, punkObject, &lChildPos);
		while (lChildPos)
		{
			IUnknownPtr sptr;
			// get next obeject
			if (FAILED(m_sptrContext->GetNextChild(bstrType, punkObject, &lChildPos, &sptr)) || sptr == 0)
				continue;
			
			// and try add it to control
			if (!AddObject(hItem, sptr, bEnable))
				bRet = false;
		}
	}

	// sort children
	if (hItem->HasChildren() && !SortObjects(hItem))
		bRet = false;

	return bRet;
}

bool CContextWrpType::AddObject(IUnknown * punk, bool bSort)
{
	if (!punk)
		return false;

	HCONTEXTITEM hItem = FindObjectItem(punk);
	HCONTEXTITEM hParent = 0;
	if (!hItem)
	{
		// get obejct type
		_bstr_t bstrType = ::GetObjectKind(punk);
		// get type item
		hParent = GetTypeItem(bstrType);
		// if such type does not exists
		if (!hParent)
		{
			// add it to view
			hParent = AddType(bstrType, false);
			SortTypes();
		}

		// check object has parent
		INamedDataObject2Ptr sptrN = punk;
		if (sptrN)
		{
			IUnknownPtr sptrParent;
			sptrN->GetParent(&sptrParent);
			// if parent exists
			if (sptrParent)
			{
				// try to find parent
				hParent = FindObjectItem(sptrParent);
				// if parent item does not exists
				if (!hParent)
				{
					// add parent to tree
					if (!AddObject(sptrParent, true))
						return false;
					// and het parent item
					hParent = FindObjectItem(sptrParent);
				}
				ASSERT(hParent != 0);
			}
		}

		// check type enable
		BOOL bEnable = true;
		if (m_sptrContext)
			m_sptrContext->GetTypeEnable(bstrType, &bEnable);

		// add object item
		if (!AddObject(hParent, punk, bEnable == TRUE))
			return false;
	}

	// sort if it's needed
	if (bSort && hParent)
		SortObjects(hParent);
	return true;
}


// remove object
bool CContextWrpType::RemoveObject(IUnknown* punkObject)
{
	if (!punkObject)
		return false;

	// get context item from unknown
	HCONTEXTITEM hItem = FindObjectItem(punkObject);
	if (!hItem)
		return false;

	// get parent item (type) for object
	HCONTEXTITEM hTypeItem = hItem->GetParent();

	// and remove item
	if (!m_Tree.DeleteItem(hItem))
		return false;

	// if parent item is type_item and valid
	if (ItemIsType(hTypeItem) && hTypeItem)
	{
		// if type_item has no objects -> remove type
		if (!hTypeItem->HasChildren() && !m_Tree.DeleteItem(hTypeItem))
			return false;
	}

	return true;
}

// change object's key
bool CContextWrpType::ChangeObjectKey(IUnknown* punkObject, GuidKey & PrevKey)
{
	if (!punkObject || PrevKey == INVALID_KEY)
		return false;

	GuidKey NewKey = ::GetObjectKey(punkObject);
	if (NewKey == INVALID_KEY || NewKey == PrevKey)
		return true;


	HCONTEXTITEM hItem = NULL;
	if (!m_mapObjects.Lookup(PrevKey, hItem) || !hItem)
		return false;

	m_mapObjects.RemoveKey(PrevKey);
	m_mapObjects.SetAt(NewKey, hItem);

	return true;
}

// activate (or deactivate) object
bool CContextWrpType::ActivateObject(IUnknown* punkObject, bool bActive)
{
//	DeactivateAllObject();
	if (!punkObject)
		return true;

	// check object is can be shown in current view  
	CString strType = ::GetObjectKind(punkObject);
	BOOL bAvailable = true;
	if (m_sptrContext)
		m_sptrContext->GetTypeEnable(_bstr_t(strType), &bAvailable);

	if (!bAvailable)
		return true;

	// get object hitem
	HCONTEXTITEM hItem = FindObjectItem(punkObject);
	if (!hItem)
		return false;

	bool bRet = true;
	if (!m_Tree.SelectItem(hItem, bActive))
		bRet = false;

	UpdateSelections();
	UpdateActive();

	if (bActive && m_Tree.GetItemFocus() != hItem && !m_Tree.SetItemFocus(hItem))
		bRet = false;
	m_Tree.EnsureVisible(hItem);
	return bRet;
}

// select (or deselect) object
bool CContextWrpType::SelectObject(IUnknown* punkObject, bool bSelect)
{
	if (!punkObject || m_sptrContext == 0)
		return false;

	if (!m_bEnableUpdateSelections)
		return true;

	// get object hitem
	HCONTEXTITEM hItem = FindObjectItem(punkObject);
	if (!hItem)
		return false;

	CString strType = ::GetObjectKind(punkObject);

	long nNumber = -1;
	m_sptrContext->GetSelectedObjectNumber(_bstr_t(strType), punkObject, &nNumber);
	if (hItem)
		m_Tree.SetItemNumber(hItem, nNumber);

	UpdateSelections(strType);

	return true;
}


bool CContextWrpType::DeselectAllObject()
{
	bool bRet = true;
	HCONTEXTITEM hRoot = m_Tree.GetRootItem();
	if (!hRoot)
		return false;

	for (POSITION pos = hRoot->GetFirstPos(); pos != NULL; )
	{
		HCONTEXTITEM hItem = hRoot->GetChild(pos);
		pos = hRoot->GetNextPos(pos);
		if (!hItem || !hItem->HasChildren())
			continue;

		CContextItemData * pData = (CContextItemData*)m_Tree.GetItemData(hItem);
		if (!pData)
			continue;

		if (!DeselectAllObject(pData->GetName()))
			bRet = false;

	}

	return bRet;
}

bool CContextWrpType::DeselectAllObject(LPCTSTR szType)
{
	bool bRet = true;
	CString strType(szType);
	if (strType.IsEmpty())
		return true;
	

	BOOL bEnable = false;
	IDataContext2Ptr sptrC = m_sptrView ? m_sptrView : GetActiveContext(false);
	if (sptrC)
		sptrC->GetTypeEnable(_bstr_t(strType), &bEnable);

	HCONTEXTITEM hItem = GetTypeItem(szType);
	CContextItemData * pData = (CContextItemData *)hItem->GetData();
	if (pData)
		pData->SetEnable(bEnable == TRUE);

	if (!hItem || !hItem->HasChildren())
		return true;

	for (POSITION pos = hItem->GetFirstPos(); pos != NULL; )
	{
		HCONTEXTITEM hChild = hItem->GetChild(pos);
		DeNumberChildObject(hChild, bEnable == TRUE);
		pos = hItem->GetNextPos(pos);
	}

	return bRet;
}

bool CContextWrpType::DeNumberChildObject(HCONTEXTITEM hItem, bool bEnable)
{
	if (!hItem)
		return true;

	m_Tree.SetItemNumber(hItem, -1);

	DWORD dwState = hItem->GetState();
	if (bEnable)
		dwState &= ~CVIS_DISABLED;
	else
		dwState |= CVIS_DISABLED;

	hItem->SetState(dwState);

	for (POSITION pos = hItem->GetFirstPos(); pos != NULL; )
	{
		HCONTEXTITEM hChild = hItem->GetChild(pos);
		DeNumberChildObject(hChild);
		pos = hItem->GetNextPos(pos);
	}
	return true;
}


bool CContextWrpType::DeactivateAllObject()
{
	if (!m_Tree.GetSelectedCount())
		return true;

	CItemList SelList;
	m_Tree.GetSelectedItems(SelList);

	bool bRet = true;
	for (POSITION pos = SelList.GetHeadPosition(); pos != NULL; )
	{
		HCONTEXTITEM hItem = SelList.GetNext(pos);
		if (hItem && !m_Tree.SelectItem(hItem, false))
			bRet = false;
	}
//	if (!m_Tree.SetItemFocus(0))
//		bRet = false;

	return bRet;
}

// sort children for given parent (for objects)
bool CContextWrpType::SortObjects(HCONTEXTITEM hTypeItem)
{
//	CString str;
//	str.Format("CContextWrpType::SortObjects : %s\n", hTypeItem->GetText());
//	CTimeTest tt (true, str);
//	ASSERT (hTypeItem);
	if (!hTypeItem)
		return true;

	if (hTypeItem->GetChildrenCount() < 2)
		return true;
	CVSORTCB sort;
	sort.hParent = hTypeItem;
	sort.lpfnCompare = m_bSortByTime ?	CContextWrpType::CompareObjectFunc : 
										CContextWrpType::CompareTypeFunc;
	sort.lParam = (LPARAM)this;

	return m_Tree.SortChildrenCB(&sort) == TRUE;
}

bool CContextWrpType::SortObjectsRecursively(HCONTEXTITEM hItem)
{
	bool bRet = true;
	if (hItem && hItem->HasChildren())
	{
		for (POSITION pos = hItem->GetFirstPos(); pos != NULL; )
		{
			HCONTEXTITEM hChild = hItem->GetChild(pos);
			pos = hItem->GetNextPos(pos);
			if (!SortObjectsRecursively(hChild))
				bRet = false;
		}
		if (hItem->GetChildrenCount() > 1 && !SortObjects(hItem))
			bRet = false;
	}

	return bRet;
}


// find object's HCONTEXTITEM by object's IUnknown
HCONTEXTITEM CContextWrpType::FindObjectItem(IUnknown* punkObject)
{
	HCONTEXTITEM hItem = NULL;
	if (!punkObject)
		return 0;

	if (!m_mapObjects.Lookup(::GetObjectKey(punkObject), hItem))
		return 0;

	return hItem;
}

// return IUnknown to object by context item w/ addref
IUnknown * CContextWrpType::GetObject(HCONTEXTITEM hItem, bool bAddRef)
{
	ASSERT(hItem);
	if (!hItem || m_Tree.ItemIsHeader(hItem))
		return 0;

	CContextItemData * pData = (CContextItemData*)hItem->GetData();
	if (!pData)
		return 0;

	return pData->GetPointer(bAddRef);
}


// return context item of first level(type) by given type
HCONTEXTITEM CContextWrpType::GetTypeItem(LPCTSTR szType)
{
	HCONTEXTITEM hItem = 0;
	if (!m_mapTypes.Lookup(szType, (void*&)hItem))
		return 0;

	return hItem;
}

void CContextWrpType::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{

	COLORREF clrBack = m_Tree.GetBkColor();
	if (m_sptrThumbMan == NULL)
		return;

	DRAWITEMSTRUCT * lpdis = lpDrawItemStruct;
	if (lpdis->CtlType != ODT_CONTEXTVIEW)
		return;

	if (lpdis->CtlID != ID_INNER_TREE)
		return;

	if ((lpdis->itemState & ODS_SELECTED) == ODS_SELECTED)
		clrBack = m_Tree.CreateDitherColor();

	HCONTEXTITEM hItem = (HCONTEXTITEM)lpdis->itemID;
	CContextItemData * pData = (CContextItemData*)lpdis->itemData;
	
	CRect rect(lpdis->rcItem);
	if (rect.IsRectEmpty())
		return;
/*
	if ((lpdis->itemState & ODS_SELECTED) == ODS_SELECTED)
	{
//		rect.OffsetRect(1, 1);
		rect.bottom--;
		rect.right--;
	}
*/

	if (!pData && !hItem)
		return;
	
	if (!pData)
		pData = (CContextItemData*)hItem->GetData();

	if (!pData || !pData->GetPointer(false))
		return;

	CDC*  pDC = CDC::FromHandle(lpdis->hDC);
	if (!pDC)
		return;

	IUnknownPtr sptrUnkRO;
	m_sptrThumbMan->GetRenderObject(pData->GetPointer(false), &sptrUnkRO);
	IRenderObjectPtr spRenderObject = sptrUnkRO;

	if (spRenderObject == NULL) 
		return;
/*
	CRect rectPaint = rect;

	CImageWrp image = pData->GetPointer(false);
	if (image)
	{
		CRect rectImage = image.GetRect();

		double fRatioY = double(rect.Height()) / double(rectImage.Height());
		double fRatioX = double(rect.Width()) / double(rectImage.Width());
		double fZoom = fRatioX < fRatioY ? fRatioX : fRatioY;

		int nW = (int)::ceil(rectImage.right*fZoom) - (int)::floor(rectImage.left*fZoom);
		int nH = (int)::ceil(rectImage.bottom*fZoom) - (int)::floor(rectImage.top*fZoom);

		rectPaint = NORECT;
		rectPaint.left = rect.left + (rect.Width() - nW) / 2;
		if (rectPaint.left < rect.left)
			rectPaint.left = rect.left;
		
		rectPaint.right = rectPaint.left + nW;
		if (rectPaint.right > rect.right)
			rectPaint.right = rect.right;
		
		rectPaint.top = rect.top + (rect.Height() - nH) / 2;
		if (rectPaint.top < rect.top)
			rectPaint.top = rect.top;

		rectPaint.bottom = rectPaint.top + nH;
		if (rectPaint.bottom > rect.bottom)
			rectPaint.bottom = rect.bottom;
	}
*/
	BYTE * pbi = NULL;
	short nSupported;
	spRenderObject->GenerateThumbnail(pData->GetPointer(false), 24, RF_BACKGROUND, clrBack,
									  CSize(rect.Width(), rect.Height()), 
									  &nSupported, &pbi);

	if (pbi)
	{
//		if ((lpdis->itemState & ODS_SELECTED) == ODS_SELECTED)
//			rect.OffsetRect(1, 1);
		::ThumbnailDraw(pbi, pDC, rect, pData->GetPointer(false) );
		::ThumbnailFree(pbi);
		pbi = NULL;
	}
}


void CContextWrpType::PostNcDestroy() 
{
	ClearContens();
//	delete this;	
}

BOOL CContextWrpType::SaveScrollInfo(SCROLLINFO * lpsf)
{
	if (!lpsf || !::IsWindow(GetSafeHwnd()))
		return false;
	memset(lpsf, 0, sizeof(SCROLLINFO));
	lpsf->cbSize = sizeof(SCROLLINFO);
	return GetScrollInfo(SB_VERT, lpsf, SIF_ALL);
}

BOOL CContextWrpType::RestoreScrollInfo(SCROLLINFO * lpsf, bool bRedraw)
{
	if (!lpsf || !::IsWindow(GetSafeHwnd()))
		return false;

	lpsf->fMask = SIF_ALL;
	SetScrollInfo(SB_VERT, lpsf, bRedraw);
	return true;
}
/*
void CContextWrpType::UpdateEnables()
{
	CTimeTest tt (true, "CContextWrpType::UpdateEnables");

	if (m_sptrContext == 0)
		return;

	// set enable flags for all types
	for (POSITION pos = m_mapTypes.GetStartPosition(); pos != NULL; )
	{
		CString strType;
		HCONTEXTITEM hItem = 0;
		m_mapTypes.GetNextAssoc(pos, strType, (void*&)hItem);

		if (!hItem)
			continue;

		BOOL bEnable = true;
		m_sptrContext->GetTypeEnable(_bstr_t(strType), &bEnable);
		CContextItemData * pData = (CContextItemData *)hItem->GetData();
		if (pData)
			pData->SetEnable(bEnable == TRUE);

		UpdateEnablesChildren(hItem, bEnable == TRUE);
	}
}

void CContextWrpType::UpdateEnablesChildren(HCONTEXTITEM hParent, bool bEnable)
{
	if (!hParent || !hParent->HasChildren())
		return;

	for (POSITION ChildPos = hParent->GetFirstPos(); ChildPos != NULL; )
	{
		HCONTEXTITEM hChild = hParent->GetChild(ChildPos);
		DWORD dwState = hChild->GetState();
		if (bEnable)
			dwState &= ~CVIS_DISABLED;
		else
			dwState |= CVIS_DISABLED;

		hChild->SetState(dwState);
		if (hChild->HasChildren())
			UpdateEnablesChildren(hChild, bEnable);

		ChildPos = hParent->GetNextPos(ChildPos);
	}
}
*/
void CContextWrpType::UpdateSelections()
{
	if (!m_bEnableUpdateSelections)
		return;
//	CTimeTest tt (true, "CContextWrpType::UpdateSelections");

	DeselectAllObject();

	IDataContext2Ptr sptrC = m_sptrContext == 0 ? GetActiveView(false) : m_sptrContext;
	if (sptrC == 0)
		return;

	// get types count
	long nTypeCount = 0;
	sptrC->GetObjectTypeCount(&nTypeCount);

	// for all types
	for (long i = 0; i < nTypeCount; i++)
	{
		// get next object's type
		BSTR bstr = 0;
		sptrC->GetObjectTypeName(i, &bstr);
		_bstr_t bstrType(bstr, false);

		int nCount = 1;
		// get first position of selected objects
		LONG_PTR nPos = 0;
		if (FAILED(sptrC->GetFirstSelectedPos(bstrType, &nPos)) || !nPos)
			continue;

		// for all selected objects
		while (nPos)
		{
			// get object 
			IUnknownPtr sptrObj;
			if (FAILED(sptrC->GetNextSelected(bstrType, &nPos, &sptrObj)) || sptrObj == 0)
				continue;

			// set it to selected state  
			HCONTEXTITEM hItem = FindObjectItem(sptrObj);
			if (hItem)
				m_Tree.SetItemNumber(hItem, nCount++);
		} // continue to next type
	}// for all types on named_data
}

void CContextWrpType::UpdateSelections(LPCTSTR szType)
{
	if (!m_bEnableUpdateSelections)
		return;

	_bstr_t bstrCmpType(szType);
	if (!bstrCmpType.length())
		return;

	DeselectAllObject(szType);

	IDataContext2Ptr sptrC = m_sptrContext == 0 ? GetActiveView(false) : m_sptrContext;
	if (sptrC == 0)
		return;

	// get types count
	long nTypeCount = 0;
	sptrC->GetObjectTypeCount(&nTypeCount);

	// for all types
	for (long i = 0; i < nTypeCount; i++)
	{
		// get next object's type
		BSTR bstr = 0;
		sptrC->GetObjectTypeName(i, &bstr);
		_bstr_t bstrType(bstr, false);
		if (bstrType != bstrCmpType)
			continue;

		BOOL bEnable = false;
		sptrC->GetTypeEnable(bstrType, &bEnable);

		// get selected objects of appropriate type
		// if context hasn't selected object of given type -> to next type
		long nSelCount = 0;
		if (FAILED(sptrC->GetSelectedCount(bstrType, &nSelCount)) || !nSelCount)
			continue;

//		int nNumber = 1;
		int nCount = 1;
		// get first position of selected objects
		LONG_PTR nPos = 0;
		if (FAILED(sptrC->GetFirstSelectedPos(bstrType, &nPos)) || !nPos)
			continue;

		// for all selected objects
		while (nPos)
		{
			// get object 
			IUnknownPtr sptrObj;
			if (FAILED(sptrC->GetNextSelected(bstrType, &nPos, &sptrObj)) || sptrObj == 0)
				continue;

//				sptrC->GetSelectedObjectNumber(_bstr_t(::GetObjectKind(sptrSel)), sptrSel, &nNumber);
//				ASSERT(nCount == nNumber);

			HCONTEXTITEM hItem = FindObjectItem(sptrObj);
			if (hItem)
				m_Tree.SetItemNumber(hItem, nCount);//nNumber);

			nCount++;
		} // continue to next type
	}// for all types on named_data
}

void CContextWrpType::UpdateActive()
{
	DeactivateAllObject();

	IDataContext2Ptr sptrC = m_sptrContext ? m_sptrContext : GetActiveView(false);
	IViewPtr sptrView = sptrC;
	if (sptrC == 0 || sptrView == 0)
		return;

	TPOS lPos = 0;
	sptrView->GetFirstVisibleObjectPosition(&lPos);

	while (lPos)
	{
		IUnknownPtr sptrObj;
		sptrView->GetNextVisibleObject(&sptrObj, &lPos);

		HCONTEXTITEM hItem = FindObjectItem(sptrObj);
		if (hItem)
			m_Tree.SelectItem(hItem, true);
	}

/*
	// get last active object
	IUnknownPtr sptrActiveObj;
	sptrActiveObj.Attach(::GetActiveObjectFromContextEx(sptrC), false);
	// and set it active in context view
	// get object hitem
	HCONTEXTITEM hItem = FindObjectItem(sptrActiveObj);
	if (hItem)
	{
		m_Tree.SelectItem(hItem, true);
		if (m_Tree.GetItemFocus() != hItem)
		{
			m_Tree.SetItemFocus(hItem);
			m_Tree.EnsureVisible(hItem);
		}
	}
*/
}

void CContextWrpType::SetNumeric()
{
//	CTimeTest tt (true, "CContextWrpType::SetNumeric()");
	CWnd * pFocusWnd = CWnd::GetFocus();
	HCONTEXTITEM hFocusItem = (pFocusWnd == &m_Tree || pFocusWnd == this) ? m_Tree.GetItemFocus() : m_Tree.GetPrevItemFocus();
	if (!hFocusItem)
		return;

	// get type item
	HCONTEXTITEM hType = hFocusItem;
	while (hType && hType->GetParent() != m_Tree.GetRootItem())
		hType = hType->GetParent();

	// get object's type
	CContextItemData * pData = (CContextItemData*)m_Tree.GetItemData(hType);
	if (!pData)
		return;
	_bstr_t bstrType = pData->GetName();

	// get context
	IDataContext2Ptr sptrC = m_sptrContext != 0 ? m_sptrContext : GetActiveContext(false);
	if (sptrC == 0)
		return;

	m_bEnableUpdateSelections = false;
	BOOL bPrevRedraw = SetRedraw(false);
	_try (CContextWrpType, SetNumeric)
	{
		// get count of objects of given type
		long nCount = 0;
		sptrC->GetObjectCount(bstrType, &nCount);

		if (nCount)
		{
			// for all objects
			LONG_PTR lPos = 0;
			sptrC->GetFirstObjectPos(bstrType, &lPos);
			while (lPos)
			{
				IUnknownPtr sptr;
				if (FAILED(sptrC->GetNextObject(bstrType, &lPos, &sptr)) || sptr == 0)
					continue;

				// and select it
				sptrC->SetObjectSelect(sptr, true);
			}
		}
		sptrC = 0;
	}
	_catch;
	m_bEnableUpdateSelections = true;
	UpdateSelections(pData->GetName());

	m_Tree.SetItemFocus(hFocusItem);
	SetRedraw(bPrevRedraw);
}

void CContextWrpType::RemoveNumeric()
{
//	CTimeTest tt (true, "CContextWrpType::RemoveNumeric()");
	HCONTEXTITEM hFocusItem = m_Tree.GetItemFocus();
	if (!hFocusItem)
		return;

	// get type item
	HCONTEXTITEM hType = hFocusItem;
	while (hType && hType->GetParent() != m_Tree.GetRootItem())
		hType = hType->GetParent();

	// get object's type
	CContextItemData * pData = (CContextItemData*)m_Tree.GetItemData(hType);
	if (!pData)
		return;
	_bstr_t bstrType = pData->GetName();

	// get context
	IDataContext2Ptr sptrC = m_sptrContext != 0 ? m_sptrContext : GetActiveContext(false);
	if (sptrC == 0)
		return;

	BOOL bPrevRedraw = SetRedraw(false);
	m_bEnableUpdateSelections = false;
	_try (CContextWrpType, RemoveNumeric)
	{
		// get count of objects of given type
		long nCount = 0;
		sptrC->GetSelectedCount(bstrType, &nCount);

		if (nCount > 1)
		{
			LONG_PTR nPos = 0;

			IUnknown * punkObject = 0;
			// first selected object is active object
			// so we neeed to skip it (request by technologist)
			sptrC->GetFirstSelectedPos(bstrType, &nPos);
			if (nPos)
			{
				sptrC->GetNextSelected(bstrType, &nPos, &punkObject);
				if (punkObject)
				{
					punkObject->Release();
					punkObject = 0;
				}
			}

			// from second selected object and further
			while (nPos)
			{
				// get object
				sptrC->GetNextSelected(bstrType, &nPos, &punkObject);
				if (punkObject)
				{
					// and deselect it
					sptrC->SetObjectSelect(punkObject, false);
					punkObject->Release();
					punkObject = 0;
				}
			}
		}
		sptrC = 0;
	}
	_catch;

	m_bEnableUpdateSelections = true;
	UpdateSelections(pData->GetName());

	m_Tree.SetItemFocus(hFocusItem);
	SetRedraw(bPrevRedraw);
	
}

bool CContextWrpType::SetFocusObject(IUnknown * punk)
{
	HCONTEXTITEM hitem = FindObjectItem(punk);
	if (!hitem)
		return false;

	m_Tree.SetItemFocus(hitem);
	return true;
}

IUnknown * CContextWrpType::GetFocusObject(bool bAddRef)
{
	
	HCONTEXTITEM hFocusItem = GetFocus() == &m_Tree ? m_Tree.GetItemFocus() : m_Tree.GetPrevItemFocus();
	if (!hFocusItem)
		return 0;

	if (m_Tree.ItemIsHeader(hFocusItem))
		hFocusItem = hFocusItem->GetFirst();

	CContextItemData * pData = (CContextItemData*)hFocusItem->GetData();
	if (!pData)
		return 0;

	return pData->GetPointer(bAddRef);
}

