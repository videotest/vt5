// ObjectMgr.h: interface for the CObjectManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECTMGR_H__702AD0E3_3B47_11D3_87C9_0000B493FF1B__INCLUDED_)
#define AFX_OBJECTMGR_H__702AD0E3_3B47_11D3_87C9_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "CompManager.h"
#include "AfxTempl.h"

#include <ComIp.h>

// predefines
class CContextObject;
class CObjectManager;
class CDataContext;

typedef CArray <CContextObject*, CContextObject*> CContextObjArray;
typedef CList  <CContextObject*, CContextObject*> CContextObjList;
typedef CMap   <GUID, GUID&, CContextObject*, CContextObject*> CContextObjMap;

// class for hold NamedObjects with support Named Data Object Hierarchy

class CContextObject
{
	friend class CDataContext;
	friend class CObjectManager;


public:
	CContextObject(CObjectManager * pMgr = 0, CContextObject* parent = 0);
	CContextObject(INamedDataObject2 *punk, CObjectManager * pMgr = 0, CContextObject* parent = 0);

	~CContextObject();

	
	// INamedDataObject2
	operator INamedDataObject2*()	
	{	
		return (INamedDataObject2*) m_sptr.GetInterfacePtr();	
	}
	INamedDataObject2 * GetPtr(bool addref = true)
	{	
		if (addref && m_sptr != 0) 
			m_sptr.AddRef();	
		return m_sptr.GetInterfacePtr();	
	}
	bool				SetPtr(INamedDataObject2 * punk)
	{	
		if (m_sptr != 0)
			return false;
		
		m_sptr = punk; 
		return true;	
	}

	// manager relations
	CObjectManager *	GetMgr()						{	return m_pMgr;		}
	void				SetMgr(CObjectManager * pMgr)	{	m_pMgr = pMgr;		}
	POSITION			GetPosInMgr()					{	return m_posMgr;	}
	void				SetPosInMgr(POSITION pos)		{	m_posMgr = pos;		}

//	// base relations
//	void				SetBase(CContextObject * pBase)	{	m_pBase = pBase;	}
//	CContextObject *	GetBase()						{	return m_pBase;		}
//	POSITION			GetPosInBase()					{	return m_posBase;	}
//	void				SetPosInBase(POSITION pos)		{	m_posBase = pos;	}
	
	// selected positions
	POSITION			GetPosInSelect()				{	return m_posSel;	}
	void				SetPosInSelect(POSITION pos)	{	m_posSel = pos;		}

	// parent
	void				SetParent(CContextObject * pParent)	{	m_pParent = pParent;	}
	CContextObject *	GetParent()						{	return m_pParent;	}
	bool				HasParent()						{	return (m_pParent) ? true : false;	} 
	// parent positions
	POSITION			GetPosInParent()				{	return m_posChild;	}
	void				SetPosInParent(POSITION pos)	{	m_posChild = pos;	}

	// children
	// get child first Pos
	POSITION			GetChildFirstPos()				{	return m_listChildren.GetHeadPosition();		}
	// get Next child by Pos
	CContextObject *	GetNextChild(POSITION & pos)	{	return pos ? m_listChildren.GetNext(pos) : 0;	}
	// get child last Pos
	POSITION			GetChildLastPos()				{	return m_listChildren.GetTailPosition();		}
	// get prev child by Pos
	CContextObject *	GetPrevChild(POSITION & pos)	{	return pos ? m_listChildren.GetPrev(pos) : 0;	}
	// get child by pos
	CContextObject *	GetChild(POSITION pos)			{	return GetNextChild(pos);						}
	// get children count
	int					GetChildrenCount()				{	return (int)m_listChildren.GetCount();				}
	bool				HasChildren()					{	return m_listChildren.GetCount() ? true : false;	}

	// check obj is child
	bool IsChild(CContextObject * pObj);
	// add child
	bool AddChild(CContextObject * pChild, bool bActive = true);
	// remove child
	bool RemoveChild(CContextObject * pChild);
	// activate child
	bool ActivateChild(CContextObject * pChild);
	// find child
	POSITION FindChild(CContextObject * pChild)	{	return !IsChild(pChild) ? 0 : pChild->GetPosInParent();	}


	// clean up children
	bool RemoveAllChildren();
/*
// dependents	
	// get dependent first Pos
	POSITION			GetFirstDependentPos()				{	return m_listDependent.GetHeadPosition();		}
	// get Next dependent by Pos
	CContextObject *	GetNextDependent(POSITION & pos)	{	return pos ? m_listDependent.GetNext(pos) : 0;	}
	// get dependents count
	int					GetDependentCount()					{	return m_listDependent.GetCount();				}
	bool				HasDependent()						{	return m_listDependent.GetCount() ? true : false;	}
	// add dependent
	bool				AddDependent(CContextObject * pObj);	
	// remove dependent
	bool				RemoveDependent(CContextObject * pObj);	
	// find dependent
	bool				FindDependent(CContextObject * pObj);	
	
	// clean up dependents
	void RemoveAllDependents();
*/	

protected:
	CObjectManager *		m_pMgr;
	CContextObject *		m_pParent;		// pointer on parent object
//	CContextObject *		m_pBase;		// pointer on Base object
	CContextObjList			m_listChildren; // array of pointers on child objects
//	CContextObjList			m_listDependent;// array of pointers on child objects

	POSITION				m_posMgr;
	POSITION				m_posChild;
	POSITION				m_posSel;
//	POSITION				m_posBase;

	sptrINamedDataObject2	m_sptr;
};



// class used for managing of any objects that has IUnknown interface
// example : 
//	CContextManager = class derived from this class is used by NamedData
//	it contains ptr on all contexts associated with this NamedData
//	and Active Context

class CObjectManager
{
	friend class CDataContext;

public:
	CObjectManager(BSTR bstrtype = NULL);
	virtual ~CObjectManager();

	// get active object
	INamedDataObject2 *	GetActive(bool bAddRef = false);

	// set active object (it must be added before activate)
	bool ActivateObject(CContextObject* pObj);
	// select object (it must be in manager before select)
	bool SelectObject(CContextObject* pObj, bool bFirst = false);
	// deselect object (it must be in manager before deselect)
	bool DeselectObject(CContextObject* pObj);
	// check object is select
	bool IsObjectSelect(CContextObject* pObj)	{	return 	pObj ? pObj->GetPosInSelect() != NULL : false;	}

	// deselect all objects (remove all selected objetc from selected list)
	void				DeselectAll();
	// selected object pos functions
	POSITION			GetFirstSelectedPos()			{	return m_listSelected.GetHeadPosition();		}
	CContextObject *	GetNextSelected(POSITION & pos)	{	return pos ? m_listSelected.GetNext(pos) : 0;	}
	POSITION			GetLastSelectedPos()			{	return m_listSelected.GetTailPosition();		}
	CContextObject *	GetPrevSelected(POSITION & pos)	{	return pos ? m_listSelected.GetPrev(pos) : 0;	}

	//add component 
	bool Add(INamedDataObject2 *punk);	
	// Remove object by IUnknown
	bool Remove(INamedDataObject2 * punk);

	void Free();

	// get component count 
	int GetCount()		{	return (int)m_listObjects.GetCount();	}
	// return object by index
	INamedDataObject2 *	Get(POSITION pos, bool bAddRef = true)
	{	
		CContextObject * pObj = pos ? m_listObjects.GetNext(pos) : 0;
		return pObj ? pObj->GetPtr(bAddRef) : 0;
	}

	POSITION			GetFirstObjectPos()				{	return m_listObjects.GetHeadPosition();				}
	POSITION			GetLastObjectPos()				{	return m_listObjects.GetTailPosition();				}
	POSITION			GetNextObjectPos(POSITION pos)	{	if (pos) m_listObjects.GetNext(pos); return pos;	}
	CContextObject *	GetNextObject(POSITION & pos)	{	return pos ? m_listObjects.GetNext(pos) : 0;		}
	POSITION			GetPrevObjectPos(POSITION pos)	{	if (pos) m_listObjects.GetPrev(pos); return pos;	}
	CContextObject *	GetPrevObject(POSITION & pos)	{	return pos ? m_listObjects.GetPrev(pos) : 0;		}
	// get object by pos
	CContextObject *	GetObject(POSITION pos)			{	return pos ? m_listObjects.GetNext(pos) : 0;		}
	// for unparented objects
	CContextObject *	GetCommon()						{	return &m_CommonObject;	}

	// find object by object's interface
	CContextObject *	FindObject(INamedDataObject2 * punk) // return NULL if not found
	{
		GuidKey Key = ::GetObjectKey(punk);
		if (Key == INVALID_KEY)
			return 0;

		CContextObject* pObj = 0;
		if (!m_mapObjects.Lookup(Key, pObj) || !pObj)
			return 0;

		return pObj;
	}

	// find object by object's key
	CContextObject *	FindObject(GuidKey & Key) // return NULL if not found
	{
		if (Key == INVALID_KEY)
			return 0;

		CContextObject* pObj = 0;
		if (!m_mapObjects.Lookup(Key, pObj) || !pObj)
			return 0;

		return pObj;
	}

	// find parent object by object's interface
	CContextObject *	FindParent(INamedDataObject2 * punk);// return NULL if not found
	// find and return first level object index by Object
	POSITION			FindObjectPos(CContextObject * pObj)
	{	return pObj ? pObj->GetPosInMgr() : 0;	}
	// find and return first level object index by Object's interface
	POSITION			FindObjectPos(INamedDataObject2 * punk) // return -1 if not found
	{
		GuidKey Key = ::GetObjectKey(punk);
		if (Key == INVALID_KEY)
			return 0;

		CContextObject* pObj = 0;
		if (!m_mapObjects.Lookup(Key, pObj) || !pObj)
			return 0;
		return pObj->GetPosInMgr();
	}
	// find object by name
	CContextObject *	FindObject(CString strName); // return NULL if not found

	// get type of manager
	BSTR GetType()						{	return m_bstrType; }

	long GetLastTimeUsed()				{	return m_lLastTimeUsed;	}
	void SetLastTimeUsed(long lTime)	{	m_lLastTimeUsed = lTime;	}

	void SetSelectedCount(int nCount)	{	m_nSelCount = nCount;	}
	int	 GetAvailableSelectedCount()	{	return (int)m_nSelCount;	}
	int	 GetSelectedCount()				{	return (int)m_listSelected.GetCount();	}


protected:
	_bstr_t				m_bstrType; // type of object that this class contains

	CContextObjMap		m_mapObjects;
	CContextObjList		m_listObjects;
	CContextObject		m_CommonObject; // for unparented object
	long				m_lLastTimeUsed;

	CContextObjList		m_listSelected;
	int					m_nSelCount; // count that shows how many object we can select 
	// 
	CContextObject* _AddObject(CContextObject* pObjParent, INamedDataObject2 * punk);
	bool			_Remove(CContextObject * pObj);

};


//  for use in NamedData
class CContextManager : public CCompManager
{
public:
	CContextManager();

	// notify all objects 
	bool Notify(long cod, IUnknown * punkNew, IUnknown * punkOld, GUID* dwData = 0);
	
	IUnknown* GetActive()
	{	return m_ptrs.GetSize() ? (IUnknown*)m_ptrs[0] : NULL;	}
	
	bool SetActive(IUnknown* punk);
	bool Remove(IUnknown* punk);
	int  Find(IUnknown* punk);

};


// select object (but not activate it)
inline bool CObjectManager::SelectObject(CContextObject* pObj, bool bFirst)
{
	CString sName = ::GetObjectName(pObj->GetPtr(false));
	if (!pObj)
		return false;

	// if object already selected
	POSITION pos = pObj->GetPosInSelect();
	if (pos)
	{
		// and we not need select it first 
		if (!bFirst)
			return true;

		// remove from last pos
		m_listSelected.RemoveAt(pos);
	}

	// and add to head of selected list(if need)
	pObj->SetPosInSelect(bFirst ? m_listSelected.AddHead(pObj) : m_listSelected.AddTail(pObj));
	return true;
}

// deselect object
inline bool CObjectManager::DeselectObject(CContextObject* pObj)
{
	if (!pObj)
		return false;
	// if object already selected
	POSITION pos = pObj->GetPosInSelect();
	// deselect it
	if (pos)
	{
		m_listSelected.RemoveAt(pos);
		pObj->SetPosInSelect(0);
	}
	return true;
}

// find and return parent object for child
inline CContextObject* CObjectManager::FindParent(INamedDataObject2 * punk)
{
	if (!punk)
		return 0;

	IUnknownPtr sptrParent;

	// get parent
	if (FAILED(punk->GetParent(&sptrParent)))
		return 0;

	sptrINamedDataObject2 sptrNParent = sptrParent;

	// find parent
	if (sptrNParent == 0)// first level
		return NULL;

	return FindObject(sptrNParent);
}

inline bool ObjectIsActive(CContextObject * pObj)
{
	return pObj && pObj->GetMgr() && pObj->GetMgr()->GetSelectedCount() && pObj->GetMgr()->GetFirstSelectedPos() == pObj->GetPosInSelect();
}

inline bool ObjectIsSelected(CContextObject * pObj)
{
	return pObj && pObj->GetMgr() && pObj->GetMgr()->GetSelectedCount() && pObj->GetPosInSelect();
}

inline bool CContextObject::AddChild(CContextObject* pChild, bool bActive/* = true*/)
{
	if (pChild->GetParent() && pChild->GetParent() != this)
		return false;

	POSITION pos = pChild->GetPosInParent();
	pChild->SetParent(this);
	
	if (!pos)
	{
		if (bActive)
			pos = m_listChildren.AddHead(pChild);
		else
			pos = m_listChildren.AddTail(pChild);

		pChild->SetPosInParent(pos);
	}
	else if (bActive)
		return ActivateChild(pChild);
	return true;
}

// remove child
inline bool CContextObject::RemoveChild(CContextObject* pChild)
{
	if (pChild->GetParent() != this)
		return false;

	POSITION pos = pChild->GetPosInParent();
	
	if (!pos)
		return false;

	m_listChildren.RemoveAt(pos);
	pChild->SetPosInParent(0);

	return true;
}

// activate child
inline bool CContextObject::ActivateChild(CContextObject* pChild)
{
	if (pChild->GetParent() != this)
		return false;

	POSITION pos = pChild->GetPosInParent();
	if (!pos)
		return false;
	
	if (m_listChildren.GetHeadPosition() == pos)
		return true;

	m_listChildren.RemoveAt(pos);
	pChild->SetPosInParent(m_listChildren.AddHead(pChild));

	return true;
}


// save/load function-helpers for save / load objects & mgrs to/from string_array
bool IsTypeString(CString str);
GuidKey GuidFromString(LPCTSTR szString);
bool ContextObjectToArray(CContextObject * pObj, int nObjCount, CStringArray & sa);
bool ObjectManagerToArray(CObjectManager * pMgr, CStringArray & sa);
bool GetMgrFromMgrString(CString & strType, long & lTime, long & lObjCount, long & lSelCount, CString str);
bool GetObjectFromString(GuidKey & ObjKey, GuidKey & ParentKey, long & lChildrenCount, CString str, int nCount);
bool GetChildFromString(GuidKey & ObjKey, long & lCount, CString str);


#endif // !defined(AFX_OBJECTMGR_H__702AD0E3_3B47_11D3_87C9_0000B493FF1B__INCLUDED_)
