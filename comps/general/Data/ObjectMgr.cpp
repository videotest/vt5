// ObjectMgr.cpp: implementation of the CObjectManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Data.h"
#include "ObjectMgr.h"
#include "DataContext.h"
#include <stdlib.h>
#include "Utils.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// NOTE:
// we are think that active object has zero position
// 1 добавить мапу для ускорения поиска объекта.
// 2 переписать Free
// 3 переписать все, что покажется медленным...


CObjectManager::CObjectManager(BSTR bstrtype) 
	:	m_bstrType(bstrtype)
{
	m_mapObjects.RemoveAll();
	m_listObjects.RemoveAll();
	m_listSelected.RemoveAll();
	m_CommonObject.SetMgr(this);
	m_CommonObject.RemoveAllChildren();

	m_lLastTimeUsed = CDataContext::s_lCount++;
	m_nSelCount = 1;
}

CObjectManager::~CObjectManager()
{
	Free();
}

// find object by name
CContextObject * CObjectManager::FindObject(CString strName)
{
	
	for (POSITION pos = m_listObjects.GetHeadPosition(); pos != NULL; )
	{
		CContextObject * pObj = m_listObjects.GetNext(pos);
		if (pObj && strName == ::GetObjectName(pObj->GetPtr(false)))
			return pObj;
	}
	// if we are here it's means we are not find this object
	return 0;
}

//add component
bool CObjectManager::Add(INamedDataObject2 *punk)
{
	if (!punk)
		return false;

	// get parent
	CContextObject* pObjParent = GetCommon();
	IUnknownPtr sptrParent; 
	punk->GetParent(&sptrParent);
	if (sptrParent != 0)
	{
		INamedDataObject2Ptr sptrNParent = sptrParent;
		// find parent object 
		pObjParent = (sptrNParent != 0) ? FindObject(sptrNParent) : GetCommon();
	}
	if (!pObjParent)
		return false;	// must never happens

	// add first level object
	CContextObject* pObj = _AddObject(pObjParent, punk);
	if (!pObj)
		return false;

	return true;
}

// Remove object by IUnknown
bool CObjectManager::Remove(INamedDataObject2 * punk)
{
	CContextObject* pObj = FindObject(punk);
	if (!pObj)
		return false;

	// deselect object if need
	DeselectObject(pObj);

//	// remove dependent relations
//	CContextObject * pBase = pObj->GetBase();
//	if (pBase)
//		pBase->RemoveDependent(pObj);
//
//	pObj->RemoveAllDependents();

	// remove children

	ASSERT(pObj->GetChildrenCount() == 0);
	if (pObj->GetChildrenCount())
		pObj->RemoveAllChildren();

	// get Parent
	CContextObject * pObjParent = pObj->GetParent();

	if (pObjParent)// must be always
	{
		pObjParent->RemoveChild(pObj);
		pObj->SetParent(0);
	}

	// and remove this object 
	return _Remove(pObj);
}

//add component
CContextObject * CObjectManager::_AddObject(CContextObject* pObjParent, INamedDataObject2 *punk)
{
	// create object 
	CContextObject * pObj = new CContextObject(punk, this, pObjParent);

	GuidKey Key = ::GetObjectKey(punk);
	// and add it to manager
	m_mapObjects.SetAt(Key, pObj);
	POSITION pos = m_listObjects.AddTail(pObj);
	pObj->SetPosInMgr(pos);

	// add reference to it to parent
	if (pObjParent)
		pObjParent->AddChild(pObj, false);

	return pObj;
}

// simply remove object from manager by object's pointer
bool CObjectManager::_Remove(CContextObject* pObj)
{
	if (!pObj)
		return false;

	GuidKey Key = ::GetObjectKey(pObj->GetPtr(false));
	m_mapObjects.RemoveKey(Key);

	POSITION pos = pObj->GetPosInMgr();
	if (pos)
		m_listObjects.RemoveAt(pos);
	delete pObj;

	return true;
}

/*
bool CObjectManager::_Remove(int nIndex)
{
	if (nIndex < 0 || nIndex >= m_arrObjects.GetSize())
		return false;

	CContextObject* pObj = m_arrObjects[nIndex];
	m_arrObjects.RemoveAt(nIndex, 1);
	delete pObj;

	return true;
}
*/

void CObjectManager::Free()
{
	DeselectAll();
	for (POSITION posObj = m_listObjects.GetHeadPosition(); posObj != NULL; )
	{
		CContextObject * pObj = m_listObjects.GetNext(posObj);
		if (pObj)
		{
			// remove object from parent
			if (pObj->GetParent())
			{
				pObj->GetParent()->RemoveChild(pObj);
				pObj->SetParent(0);
			}
			// remove all children
			for (POSITION posChild = pObj->GetChildFirstPos(); posChild != NULL; )
			{
				CContextObject * pChild = pObj->GetNextChild(posChild);
				if (pChild)
				{
					pObj->RemoveChild(pChild);
					pChild->SetParent(0);
				}
			}
		}
	}

	for (POSITION pos = m_mapObjects.GetStartPosition(); pos != NULL; )
	{
		GuidKey Key;
		CContextObject * pObj = 0;
		m_mapObjects.GetNextAssoc(pos, Key, pObj);
		
//		// remove dependent relations
//		CContextObject * pBase = pObj->GetBase();
//		if (pBase)
//			pBase->RemoveDependent(pObj);
//
//		pObj->RemoveAllDependents();
		
		_Remove(pObj);
	}
	m_mapObjects.RemoveAll();
	m_listObjects.RemoveAll();
	ASSERT(m_listObjects.GetCount() == 0);

}

// deselect all objects
void CObjectManager::DeselectAll()
{
	for (POSITION pos = m_listSelected.GetHeadPosition(); pos != NULL; )
	{
		CContextObject * pObj = m_listSelected.GetNext(pos);
		pObj->SetPosInSelect(0);
	}
	m_listSelected.RemoveAll();

}

// return IUnknown interface on active Object with AddRef
INamedDataObject2 * CObjectManager::GetActive(bool bAddRef)
{
	CContextObject * pObj = 0;
	if (m_listSelected.GetCount())
		pObj = (CContextObject*)m_listSelected.GetHead();
//	else if (m_arrObjects.GetSize())
//		pObj = m_arrObjects.GetAt(0);

	if (!pObj)
		return NULL;

	// return interface on object on zero position 
	return pObj->GetPtr(bAddRef);
}

// set Active child object 
// NOTE: object must be contained in array before call this function !!!!!

bool CObjectManager::ActivateObject(CContextObject * pObj)
{
	// activate only this object
	// try to find object
	POSITION pos = FindObjectPos(pObj);
	if (!pos)
		return false;

	// set active
	if (pos && pos != m_listObjects.GetHeadPosition())
	{
		m_listObjects.RemoveAt(pos);
		pos = m_listObjects.AddHead(pObj);
		pObj->SetPosInMgr(pos);
	}

	if (!SelectObject(pObj, true))
		return false;

	// if we have parent => set this object to active in parent
	if (pObj->GetParent())
		pObj->GetParent()->ActivateChild(pObj);

	return true;
}

/*
#if 0
bool CObjectManager::SortByTime()
{
	if (m_arrObjects.GetSize() < 2)
		return true;

	for (int i = 0; i < m_arrObjects.GetSize() - 1; i++)
	{
		int nPos = i;
		for (int j = i; j < m_arrObjects.GetSize(); j++)
		{
			if (m_arrObjects[nPos]->GetLastTimeUsed() < m_arrObjects[j]->GetLastTimeUsed())
				nPos = j;
		}
		if (nPos != i)
		{
			CContextObject *pObj = m_arrObjects[nPos];
			m_arrObjects[nPos] = m_arrObjects[i];
			m_arrObjects[i] = pObj;
		}
	}

	return true;
}

#else

int CompObj(const void *arg1, const void *arg2)
{
	CContextObject* pObj1 = (CContextObject*)arg1;
	CContextObject* pObj2 = (CContextObject*)arg2;

	if (!pObj2 && !pObj1)
		return 0;
	else if (!pObj1)
		return -1;
	else if (!pObj2)
		return 1;

	return pObj1->GetLastTimeUsed() - pObj2->GetLastTimeUsed();
}

// new version that used 'qsort'
bool CObjectManager::SortByTime()
{
	int nCount = m_arrObjects.GetSize();
	if (nCount < 2)
		return true;

	LPVOID lptr = (LPVOID)m_arrObjects.GetData();
	qsort(lptr, nCount, sizeof(CContextObject*), CompObj);

	return true;
}

#endif


bool CObjectManager::Swap(int nFirst, int nLast)
{
	if (nFirst == nLast)
		return true;

	if (nFirst < 0 || nFirst >= m_arrObjects.GetSize() ||
		nLast < 0 || nLast >= m_arrObjects.GetSize())
		return false;

	if (m_arrObjects[nFirst]->GetLastTimeUsed() > m_arrObjects[nLast]->GetLastTimeUsed())
		return true;

	CContextObject *pObj = m_arrObjects[nFirst];
	m_arrObjects[nFirst] = m_arrObjects[nLast];
	m_arrObjects[nLast] = pObj;

	return true;
}
*/


//////////////////////////////////////////////////////////////////////////////////////////////////

/*
// set Active object
// NOTE: object must be contained in array before call this function !!!!!
bool CObjectManager::SetActiveAll(INamedDataObject2 * punk)
{
	_try(CObjectManager, SetActiveAll)
	{
		// create pointer for new active
		if (!CheckInterface(punk, IID_IUnknown))
			return false;

		sptrINamedDataObject2 sptrActive(punk);

		// find parent object 
		CContextObject* pObjParent = FindParent(punk);
		
		if (!pObjParent)
			return false; // must never happens
		
		if (!pObjParent->ActivateChild(punk))
			return false;

		if (!(pObjParent->GetChild(0))->GetFirst(true))
			return false;

	}
	_catch
	{
		// for all exception 
		return false;
	}
	// if we are here it's means we are not find this object
	// so we shall return false and not change active object 
	return true;
}

*/


///////////////////////////////////////////////////////////////////////////////

// utility class for inner use in NamedData
CContextManager::CContextManager() 
	: CCompManager() 
{
}

// notify all contexts about event from NamedData
bool CContextManager::Notify(long cod, IUnknown * punkNew, IUnknown * punkOld, DWORD dwData)
{
	bool flag = true;
	_try(CContextManager, Notify)
	{
		for (int i = 0; i < GetComponentCount(); i++)
		{
			if (!CheckInterface(GetComponent(i, false), IID_IDataContext))
			{
				flag = false;
				continue;
			}
			
			// create pointer
			sptrIDataContext sptr(GetComponent(i, false));
			// call notify		
			sptr->Notify(cod, punkNew, punkOld, dwData);
		}	
	}
	_catch
	{
		// for all exception 
		return false;
	}
	// success
	return flag;
}


bool CContextManager::SetActive(IUnknown* punk)
{	
	int pos = Find(punk);
	if (pos == -1)
		return false;

	if (pos == 0)
		return true;

	m_ptrs.RemoveAt(pos);
	m_ptrs.InsertAt(0, punk);
	
	return true;
}

bool CContextManager::Remove(IUnknown* punk)
{
	int pos = Find(punk);
	if (pos == -1)
		return false;

	RemoveComponent(pos);
	return true;
}


int CContextManager::Find(IUnknown* punk)
{
	GuidKey Key = ::GetObjectKey(punk);
	if (Key == INVALID_KEY)
	{
		for (int i = 0; i < m_ptrs.GetSize(); i++)
			if (m_ptrs[i] == punk)
				return i;
	}
	else
	{
		for (int i = 0; i < m_ptrs.GetSize(); i++)
			if (::GetObjectKey((IUnknown*)m_ptrs[i]) == Key)
				return i;
	}
	return -1;
}


///////////////////////////////////////////////////////////////////////////////
// class CContextObject
CContextObject::CContextObject(CObjectManager * pMgr, CContextObject * parent)
{
	m_pMgr		= pMgr;
	m_pParent	= parent;
//	m_pBase		= 0;
	
	m_posMgr	= 0;
	m_posChild	= 0;
//	m_posBase	= 0;
}

CContextObject::CContextObject(INamedDataObject2 * punk, CObjectManager * pMgr, CContextObject * parent)
{
	m_sptr		= punk;
	m_pMgr		= pMgr;
	m_pParent	= parent;
//	m_pBase		= 0;

	m_posMgr	= 0;
	m_posChild	= 0;
	m_posSel	= 0;
//	m_posBase	= 0;
}


CContextObject::~CContextObject()
{
	RemoveAllChildren();
//	RemoveAllDependents();

	m_pParent	= 0;
//	m_pBase		= 0;
	m_pMgr		= 0;
	
	m_posMgr	= 0;
	m_posChild	= 0;
	m_posSel	= 0;
//	m_posBase	= 0;
}

/*
// walk on chain (parent <-> first_child) and return first parent and last child respectively
CContextObject* CContextObject::GetLast()
{
	CContextObject* pObj = this;

	while (pObj->HasChildren())
		pObj = pObj->GetChild(0);

	return pObj;
}

CContextObject* CContextObject::GetFirst(bool bActivate)
{
	CContextObject* pObj = m_pParent;
	while (pObj)
	{
		if (bActivate)
			pObj->ActivateChild(this); // ??
		
		if (!pObj->m_pParent)
			break;

		pObj = pObj->m_pParent;
	}
	return pObj;
}
*/
/*
///////////////////////////////////////////////////////////////////////////////
// base-dependents relation functionality implements
// add dependent
inline bool CContextObject::AddDependent(CContextObject * pObj)
{
	if (pObj->GetBase() && pObj->GetBase() != this)
		return false;

	POSITION pos = pObj->GetPosInBase();
	if (!pos)
	{
		pObj->SetBase(this);
		pObj->SetPosInBase(m_listDependent.AddTail(pObj));
	}

	return true;
}

// remove dependent
inline bool CContextObject::RemoveDependent(CContextObject * pObj)
{
	if (pObj->GetBase() != this)
		return false;

	POSITION pos = pObj->GetPosInBase();
	if (!pos)
		return false;

	m_listDependent.RemoveAt(pos);
	pObj->SetPosInBase(0);
	pObj->SetBase(0);

	return true;
}

// find dependent
inline bool CContextObject::FindDependent(CContextObject * pObj)
{
	if (pObj->GetBase() != this)
		return false;
	
	POSITION pos = pObj->GetPosInBase();
	if (!pos)
		return false;

	return pObj == m_listDependent.GetNext(pos);
}

// clean up dependents
void CContextObject::RemoveAllDependents()
{
	for (POSITION pos = m_listDependent.GetHeadPosition(); pos != NULL; )
		RemoveDependent(m_listDependent.GetNext(pos));
}
*/
// childrent
// add child

// clean up
bool CContextObject::RemoveAllChildren()
{
	for (POSITION pos = m_listChildren.GetHeadPosition(); pos != NULL; )
	{
		POSITION PrevPos = pos;
		CContextObject * pChild = m_listChildren.GetNext(pos);
		if (::AfxIsValidAddress(pChild, sizeof(CContextObject), true))
		{
			RemoveChild(pChild);
			pChild->SetParent(0);
		}
		else
			m_listChildren.RemoveAt(PrevPos);
	}
	ASSERT(m_listChildren.GetCount() == 0);
	m_listChildren.RemoveAll();
	return true;
}

// check obj is child
bool CContextObject::IsChild(CContextObject * pObj)
{
	if (pObj)
	{
		CContextObject * pParent = pObj->GetParent();
		if (!pParent && pObj->GetPtr(false))
		{
			IUnknownPtr sptrParent;
			if (SUCCEEDED(pObj->GetPtr(false)->GetParent(&sptrParent)) && sptrParent != 0)
			{
				INamedDataObject2Ptr sptrN = sptrParent;
				pParent = m_pMgr->FindObject(sptrN);
			}
		}
		if (pParent && pParent == this)
			return true;
	}
	return false;
}

// save to string array
bool ContextObjectToArray(CContextObject * pObj, int nObjCount, CStringArray & sa)
{
	if (!pObj)
		return false;

	CString str;
	bool bRet = true;
	LPOLESTR pOleStr = (LPOLESTR)::CoTaskMemAlloc(sizeof(OLECHAR) * 256);
	if (!pOleStr)
		return false;

	// add object
	GuidKey key = ::GetObjectKey(pObj->GetPtr(false));
	if (FAILED(StringFromGUID2(key, pOleStr, 256)))
		bRet = false;

	str.Format("%d=%d:%s:", nObjCount, pObj->GetChildrenCount(), CString(pOleStr));

	// add object parent
	CContextObject * pParent = pObj->GetParent();
	GuidKey ParentKey;
	if (pParent && pParent != pObj->GetMgr()->GetCommon())
		ParentKey = ::GetObjectKey(pParent->GetPtr(false));

	if (FAILED(StringFromGUID2(ParentKey, pOleStr, 256)))
		bRet = false;

	str += CString(pOleStr);
	// Object_number=children_count:Object_key:ParentKey
	sa.Add(str);

	// add object's children
	int nChildCount = pObj->GetChildrenCount() - 1;
	for (POSITION pos = pObj->GetChildLastPos(); pos != NULL; )
	{
		CContextObject * pChild = pObj->GetPrevChild(pos);
		if (pChild)
		{
			GuidKey ChildKey = ::GetObjectKey(pChild->GetPtr(false));

			if (FAILED(StringFromGUID2(ChildKey, pOleStr, 256)))
				bRet = false;

			str.Format("%d=", nChildCount);
			sa.Add(str + CString(pOleStr));
		}
		nChildCount--;
	}
	ASSERT(nChildCount == -1);

	::CoTaskMemFree((LPVOID)pOleStr);
	pOleStr = 0;


	return bRet;
}

bool ObjectManagerToArray(CObjectManager * pMgr, CStringArray & sa)
{
	if (!pMgr)
		return false;

	// ["name":time:object_count:selected_count]
	CString strType;
	strType.Format("[%s:%d:%d:%d]", CString(pMgr->GetType()), pMgr->GetLastTimeUsed(), 
									pMgr->GetCount(), pMgr->GetSelectedCount());

	// type
	sa.Add(strType);

	// add objects to array in reverse order
	bool bRet = true;
	int nCount = pMgr->GetCount() - 1;
	for (POSITION pos = pMgr->GetLastObjectPos(); pos != NULL; )
	{
		CContextObject * pObj = pMgr->GetPrevObject(pos);
		if (!ContextObjectToArray(pObj, nCount, sa))
		{
			bRet = false;
			continue;
		}
		nCount--;
	}
	ASSERT (nCount == -1);

	// save to array selected objects (keys)
	int nSelCount = 0;
	LPOLESTR pOleStr = (LPOLESTR)::CoTaskMemAlloc(sizeof(OLECHAR) * 256);
	if (pOleStr)
	{
		for (pos = pMgr->GetFirstSelectedPos(); pos != NULL; )
		{
			CContextObject * pChild = pMgr->GetNextSelected(pos);
			if (!pChild)
				continue;

			GuidKey key = ::GetObjectKey(pChild->GetPtr(false));
			if (FAILED(StringFromGUID2(key, pOleStr, 256)) || !pOleStr)
				bRet = false;

			CString strTemp;
			strTemp.Format("%d=", nSelCount++);
			strTemp += CString(pOleStr);
			sa.Add(strTemp);
		}
		::CoTaskMemFree((LPVOID)pOleStr);
		pOleStr = 0;
		ASSERT (nSelCount == pMgr->GetSelectedCount());
	}

	return bRet;
}


// check string is manager string
bool IsTypeString(CString str)
{
	return str.GetLength() && str[0] == _T('[') && str[str.GetLength() - 1] == _T(']');  
}

// extract mgr from manager string
bool GetMgrFromMgrString(CString & strType, long & lTime, long & lObjCount, long & lSelCount, CString str)
{
	strType.Empty();
	lTime = -1;
	lObjCount = 0;
	lSelCount = 0;

	if (!IsTypeString(str))
		return false;

	// delet brackes
	str.Delete(0, 1);
	str.Delete(str.GetLength() - 1, 1);

// type	
	int nType = str.Find(":", 0);
	if (nType < 0)
		return false;

	strType = str.Mid(0, nType);

// time
	nType += 1;
	int nTime = str.Find(":", nType);
	if (nTime < 0)
		return false;

	CString strTemp = str.Mid(nType, nTime > nType ? nTime - nType : 0);
	lTime = atol(strTemp);

// object_number
	nTime += 1;
	int nCnt = str.Find(":", nTime);
	strTemp = str.Mid(nTime, nCnt > nTime ? nCnt - nTime : 0);
	lObjCount = atol(strTemp);

// selected_number
	nCnt += 1;
	strTemp = str.Right(str.GetLength() - nCnt);
	lSelCount = atol(strTemp);

	return true;
}

// extract object's parameters from object's string
bool GetObjectFromString(GuidKey & ObjKey, GuidKey & ParentKey, long & lChildrenCount, CString str, int nCount)
{
	ObjKey = INVALID_KEY;
	ParentKey = INVALID_KEY;
	lChildrenCount = 0;

	if (str.IsEmpty())
		return false;

// Object_number=children_count:Object_key:ParentKey

// get Object_Number
	long nNumberPos = str.Find("=", 0);
	if (nNumberPos < 0)
		return false;

	CString strTemp = str.Left(nNumberPos);
	int nNumber = atol(strTemp);
	ASSERT (nNumber == nCount);

// get children_count
	nNumberPos += 1;
	int nChildPos = str.Find(":", nNumberPos);
	if (nChildPos < 0)
		return false;

	strTemp = str.Mid(nNumberPos, nChildPos > nNumberPos ? nChildPos - nNumberPos : 0);
	lChildrenCount = atol(strTemp);

// get objectKey
	nChildPos += 1;
	int nObjKeyPos = str.Find(":", nChildPos);
	if (nObjKeyPos < 0)
		return false;

	strTemp = str.Mid(nChildPos, nObjKeyPos > nChildPos ? nObjKeyPos - nChildPos : 0);
	ObjKey = GuidFromString(strTemp);

// Get Parent Key
	nObjKeyPos += 1;
	strTemp = str.Right((str.GetLength() - nObjKeyPos) > 0 ? str.GetLength() - nObjKeyPos : 0);
	ParentKey = GuidFromString(strTemp);

	return true;

}

// extract child's parameters from child's string
bool GetChildFromString(GuidKey & ObjKey, long & lCount, CString str)
{
	ObjKey = INVALID_KEY;
	lCount = -1;

	if (str.IsEmpty())
		return false;

// Object_number=children_count:Object_key:ParentKey

// get Object_Number
	long nNumberPos = str.Find("=", 0);
	if (nNumberPos < 0)
		return false;

	CString strTemp = str.Left(nNumberPos);
	lCount = atol(strTemp);

// get objectKey
	nNumberPos += 1;
	strTemp = str.Right((str.GetLength() - nNumberPos) > 0 ? str.GetLength() - nNumberPos : 0);
	ObjKey = GuidFromString(strTemp);

	return true;
}

GuidKey GuidFromString(LPCTSTR szString)
{
	GuidKey	key;

	CString str = szString;
	if (str.IsEmpty() || str.GetLength() < 38 || str[0] != _T('{') || 
		str[9] != _T('-') || str[14] != _T('-') || str[19] != _T('-') || 
		str[24] != _T('-') || str[37] != _T('}'))
		return key;

	// remove brackets
//	01234567890123456789012345678901234567
//	{c200e360-38c5-11ce-ae62-08002b2b79ef} 

	TCHAR * tcStop;
	WORD wTempH = (WORD)_tcstol(str.Mid(1, 4), &tcStop, 16);
	WORD wTempL = (WORD)_tcstol(str.Mid(5, 4), &tcStop, 16);

//	DWORD MAKELONG(WORD wLow,WORD wHigh);
	key.Data1 = MAKELONG(wTempL, wTempH);
	key.Data2 = (WORD)_tcstol(str.Mid(10, 4), &tcStop, 16);
	key.Data3 = (WORD)_tcstol(str.Mid(15, 4), &tcStop, 16);
	key.Data4[0] = (BYTE)_tcstol(str.Mid(20, 2), &tcStop, 16);
	key.Data4[1] = (BYTE)_tcstol(str.Mid(22, 2), &tcStop, 16);
	key.Data4[2] = (BYTE)_tcstol(str.Mid(25, 2), &tcStop, 16);
	key.Data4[3] = (BYTE)_tcstol(str.Mid(27, 2), &tcStop, 16);
	key.Data4[4] = (BYTE)_tcstol(str.Mid(29, 2), &tcStop, 16);
	key.Data4[5] = (BYTE)_tcstol(str.Mid(31, 2), &tcStop, 16);
	key.Data4[6] = (BYTE)_tcstol(str.Mid(33, 2), &tcStop, 16);
	key.Data4[7] = (BYTE)_tcstol(str.Mid(35, 2), &tcStop, 16);

	return key;
}