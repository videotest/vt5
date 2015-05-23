// DataContext.cpp : implementation file
//

#include "stdafx.h"
#include "Data.h"
#include "DataContext.h"
#include "ObjectMgr.h"
#include "data5.h"

#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDataContext

IMPLEMENT_DYNCREATE(CDataContext, CCmdTargetEx)

// {DCEE4109-9579-46d3-9E57-576CA16D60B4}
static const IID IID_IDataContextDisp =
{ 0xdcee4109, 0x9579, 0x46d3, { 0x9e, 0x57, 0x57, 0x6c, 0xa1, 0x6d, 0x60, 0xb4 } };
// {B438FADB-38E7-11D3-87C5-0000B493FF1B}
GUARD_IMPLEMENT_OLECREATE(CDataContext, "Data.DataContext", 
0xb438fadb, 0x38e7, 0x11d3, 0x87, 0xc5, 0x0, 0x0, 0xb4, 0x93, 0xff, 0x1b)

// string array to mem file
void TextToMemFile(CMemFile & file, CStringArray & sa);
// string array from mem file
void MemFileToText(CMemFile & file, CStringArray & sa);
//string_array to string
void ArrayToString(CString & strTarget, CStringArray & sa);
//string_array from string
void ArrayFromString(CStringArray & sa, CString & strSource);
// compare managers by time_used
int CompMgr(const void *arg1, const void *arg2);


// static data member
long CDataContext::s_lCount = 0;


// event "szEventActiveContextChange" is sended to event listeners of view
// possible codes of this event are:
//		AddObject
//		RemoveObject
//		ReplaceObject
//		UpdateSelection
//		Rescan
//		Empty
//		ChangeBaseObject
// this event is not processed by NamedData and any controls exclude CContextViewer.

// event "szActiveObjectChange" is sended directly to aggregative view (if any) w/o help of Document's
// NotifyEventController
// sometime this event is not sended 
/*
	enum NotifyCodes
	{
		ncAddObject,		// add punkNew
		ncRemoveObject,		// remove punkNew
		ncReplaceObject,	// replace punkNew to punkOld
		ncRescan,			// rescan (0, 0)
		ncChangeBase,		// change base key for given punkNew object (lparam contains ptr on old key)
		ncEmpty,			// empty context
		ncUpdateSelection
	};
*/

CDataContext::CDataContext()
{
	_OleLockApp( this );

	EnableAutomation();
	EnableAggregation();

	m_bContextNotificationEnabled = true;
	m_bInsideFilter = false;
	
	// vanek BT:618
	m_bLockObjectActivate = FALSE;
}

CDataContext::~CDataContext()
{
	Clear();
	if (m_sptrINamedData != 0)
		m_xContext.AttachData(0);

	_OleUnlockApp( this );
}

// clear array of managers
void CDataContext::Clear()
{
	for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
		delete m_arrObjManagers[i];

	m_arrObjManagers.RemoveAll();
}


void CDataContext::OnFinalRelease()
{
	CCmdTargetEx::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CDataContext, CCmdTargetEx)
	//{{AFX_MSG_MAP(CDataContext)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CDataContext, CCmdTargetEx)
	//{{AFX_DISPATCH_MAP(CDataContext)
	DISP_FUNCTION(CDataContext, "GetActiveObject", GetActiveObject, VT_DISPATCH, VTS_BSTR)
	DISP_FUNCTION(CDataContext, "SetActiveObject", SetActiveObject, VT_EMPTY, VTS_DISPATCH)
	DISP_FUNCTION(CDataContext, "GetObjectCount",  GetObjectCount, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CDataContext, "GetFirstObjectPos", GetFirstObjectPos, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CDataContext, "GetNextObject", GetNextObject, VT_DISPATCH, VTS_BSTR VTS_PI4)
	DISP_FUNCTION(CDataContext, "GetLastObjectPos", GetLastObjectPos, VT_I4, VTS_BSTR)
	DISP_FUNCTION(CDataContext, "GetPrevObject", GetPrevObject, VT_DISPATCH, VTS_BSTR VTS_PI4)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

	
// Note: we add support for IID_IDataContext to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

BEGIN_INTERFACE_MAP(CDataContext, CCmdTargetEx)
	INTERFACE_PART(CDataContext, IID_IDataContextDisp,	Dispatch)
	INTERFACE_PART(CDataContext, IID_IDataContext,		Context)
	INTERFACE_PART(CDataContext, IID_IDataContext2,		Context)
	INTERFACE_PART(CDataContext, IID_IDataContext3,		Context)
	INTERFACE_PART(CDataContext, IID_IDataContextSync,	Sync)
	INTERFACE_PART(CDataContext, IID_IDataContextMisc,	Misc)
END_INTERFACE_MAP()


IMPLEMENT_UNKNOWN(CDataContext, Context)
IMPLEMENT_UNKNOWN(CDataContext, Sync)
IMPLEMENT_UNKNOWN(CDataContext, Misc)

//////////////////////f///////////////////////////////////////////////////////
// CDataContext message handlers


/////////////////////////////////////////////////////////////////////////////
// implements of IDispatch
// return to us IDispath interface of active object of requested type
LPDISPATCH CDataContext::GetActiveObject(LPCTSTR szType) 
{
	// create BSTR from LPCTSTR
	_bstr_t bstrType(szType);
	// get IUnknown to active object
	IUnknownPtr sptrObj(_GetActiveObject(bstrType), false);

	IDispatchPtr sptrDispath(sptrObj);
	sptrDispath.AddRef();

	return sptrDispath.GetInterfacePtr();
}

//NOTE : object must be added to context before it will be activated
// set object defined by its IDispath interface to active state
BOOL CDataContext::SetActiveObject(LPDISPATCH lpdispObj) 
{
	INamedDataObject2Ptr sptr = lpdispObj;
	return _SetActiveObject(sptr, 0, true, true);
}

// return first position of object of given type
LONG_PTR CDataContext::GetFirstObjectPos(LPCTSTR szType)
{
	// create BSTR from LPCTSTR
	_bstr_t bstrType(szType);

	// get object manager of requested type
	CObjectManager* pMgr = GetManager(bstrType);

	if (!pMgr || !ManagerIsAvailable(pMgr))
		return 0;
	
	return (LONG_PTR)pMgr->GetFirstObjectPos();
}

// return next object of given type by position and advance position to next object
LPDISPATCH CDataContext::GetNextObject(LPCTSTR szType, LONG_PTR * plPos)
{
	// create BSTR from LPCTSTR
	_bstr_t bstrType(szType);

	// get object manager of requested type
	CObjectManager* pMgr = GetManager(bstrType);

	if (!pMgr || !ManagerIsAvailable(pMgr) || !plPos || !(*plPos))
		return 0;
	
	POSITION pos = (POSITION)*plPos;
	CContextObject * pObj = pMgr->GetNextObject(pos);
	*plPos = (LONG_PTR)pos;
	if (!pObj)
		return 0;

	IDispatchPtr sptrDispath = pObj->GetPtr(false);
	sptrDispath.AddRef();

	return sptrDispath.GetInterfacePtr();
}

// return last position of object of given type
long CDataContext::GetLastObjectPos(LPCTSTR szType)
{
	// create BSTR from LPCTSTR
	_bstr_t bstrType(szType);

	// get object manager of requested type
	CObjectManager* pMgr = GetManager(bstrType);

	if (!pMgr || !ManagerIsAvailable(pMgr))
		return 0;
	
	return (long)pMgr->GetLastObjectPos();
}

// return prev object of given type by position and advance position to prev object
LPDISPATCH CDataContext::GetPrevObject(LPCTSTR szType, LONG_PTR * plPos)
{
	// create BSTR from LPCTSTR
	_bstr_t bstrType(szType);

	// get object manager of requested type
	CObjectManager* pMgr = GetManager(bstrType);

	if (!pMgr || !ManagerIsAvailable(pMgr) || !plPos || !(*plPos))
		return 0;
	
	POSITION pos = (POSITION)*plPos;
	CContextObject * pObj = pMgr->GetPrevObject(pos);
	*plPos = (LONG_PTR)pos;
	if (!pObj)
		return 0;

	IDispatchPtr sptrDispath = pObj->GetPtr(false);
	sptrDispath.AddRef();

	return sptrDispath.GetInterfacePtr();
}

// return count of all objects of requested type
long CDataContext::GetObjectCount(LPCTSTR szType) 
{
	// create BSTR from LPCTSTR
	_bstr_t bstrType(szType);

	// get object manager of requested type
	CObjectManager* pMgr = GetManager(bstrType);

	if (!pMgr || !ManagerIsAvailable(pMgr))
		return 0;
	
	// get count of object that this manager is contains
	return pMgr->GetCount();
}


///////////////////////////////////////////////////////////////////////////////
// implement of IDataContext
//
// get active object by type
HRESULT CDataContext::XContext::GetActiveObject(BSTR bstrType, IUnknown ** ppunkObj)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!ppunkObj || !bstrType)
			return E_INVALIDARG;

		if (NULL == pThis->m_sptrINamedData)
			return E_FAIL;

		*ppunkObj = pThis->_GetActiveObject(bstrType);

		return S_OK;
	}
}

//NOTE : object must be added to context before it will be activated
// Set object defined by type and IUnknown to active state
HRESULT CDataContext::XContext::SetActiveObject( BSTR bstrType, IUnknown *punkObj, DWORD dwFlags )
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		bool	bActivateDepended = (dwFlags & aofActivateDepended)==aofActivateDepended;
		bool	bSkipIfAlreadyActive = (dwFlags & aofSkipIfAlreadyActive)==aofSkipIfAlreadyActive;

		INamedDataObject2Ptr sptr = punkObj;
		//???
		//if (!pThis->_SetActiveObject(sptr, bstrType, true, bActivateDepended))
		if (!pThis->_SetActiveObject(sptr, bstrType, pThis->m_bContextNotificationEnabled,
			bActivateDepended, bSkipIfAlreadyActive))
			return E_INVALIDARG;
		return S_OK;
	}
}

// Get data associate with ppunk
HRESULT CDataContext::XContext::GetData(IUnknown ** ppunk)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		ASSERT (pThis->m_sptrINamedData!=0);

		*ppunk = pThis->m_sptrINamedData;

		if (*ppunk)
			(*ppunk)->AddRef();

		return S_OK;
	}
}

// attach context to NamedData object
HRESULT CDataContext::XContext::AttachData(IUnknown * punkNamedData)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		// attach context to NamedData object
		INamedDataPtr sptr = punkNamedData;
		return pThis->AttachData(sptr) ? S_OK : E_FAIL;
	}
}


// Notify context by NamedData based on cod
HRESULT CDataContext::XContext::Notify(long cod, IUnknown * punkNew, IUnknown * punkOld, DWORD dwData)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		bool result = false,bAutoSelect;
		BOOL bSelected = FALSE;
		{
			INamedDataObject2Ptr sptrN(punkNew);
			INamedDataObject2Ptr sptrO(punkOld);

			switch (cod)
			{
			case ncAddObject:
				bAutoSelect = ::GetValueInt(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", TRUE)?true:false;
				result = sptrN != 0 ? pThis->_AddNamedObject(sptrN, NULL, false, bAutoSelect) : false;
				break;

			case ncRemoveObject:
				bAutoSelect = ::GetValueInt(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", TRUE)?true:false;
				result = sptrN != 0 ? pThis->_RemoveNamedObject(sptrN, NULL, true, bAutoSelect) : false;
				break;

			case ncReplaceObject:
				GetObjectSelect(sptrO, &bSelected);
				result = sptrO != 0 ? pThis->_RemoveNamedObject(sptrO, NULL, true, false) : false;
				result = sptrN != 0 ? pThis->_AddNamedObject(sptrN, NULL, false, false) : false;
				if (bSelected)
					SetObjectSelect(sptrN, TRUE);
				//result = (sptrN != 0 && sptrO != 0) ? pThis->_ReplaceNamedObject(sptrN, sptrO) : false;
				break;

			case ncRescan:
				result = pThis->Rescan();
				break;

			case ncEmpty:
				pThis->Clear();
//???[AY] - delete comment here
//				pThis->NotifyContextView(szEventActiveContextChange, 0, ncEmpty);
				pThis->NotifyView(szEventActiveObjectChange, 0, UPDATE_TREE);
				result = true;
				break;
				
			case ncChangeBase:
				result = pThis->ChangeBaseObject(sptrN);
				break;

			case ncChangeKey:
				{
					// lparam contains pointer to previous base key
					GuidKey PrevKey = dwData ? *((GUID*)dwData) : INVALID_KEY;//
					result = pThis->ChangeObjectKey(sptrN, PrevKey);					
					
					//need fix it this is the stub cos after key change context has duplicate items 
					//ASSERT( false );					
					//pThis->Rescan();
				}
				break;

			case ncEnterFilter:		// filter entered, punkNew contains ptr on filterAction
				result = pThis->EnterFilter(punkNew);
				break;

			case ncPassFilter:		// filter ended, punkNew contains ptr on filterAction
				result = pThis->PassFilter(punkNew);
				break;

			case ncCancelFilter:	// filter canceled, punkNew contains ptr on filterAction
				result = pThis->CancelFilter(punkNew);
				break;

			case ncFilterUndo:		// filter undo ended
				result = pThis->UndoFilter(punkNew);
				break;

			case ncFilterRedo:		// filter redo ended
				result = pThis->RedoFilter(punkNew);
				break;
			case ncSyncWithMaster:
				if( punkNew != pThis->GetControllingUnknown() )
				{
					pThis->SynchronizeWithContext( punkNew );
					pThis->NotifyContextView( szEventActiveContextChange, 0, ncRescan );
				}

			default:
				result = false;
				break;
			}
		}
		return S_OK;
	}
}

// lock/update methods
HRESULT CDataContext::XContext::LockUpdate(BOOL bLock, BOOL bUpdate)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		pThis->m_bContextNotificationEnabled = bLock != TRUE;
		if (pThis->m_bContextNotificationEnabled && bUpdate && pThis->m_sptrINamedData != 0)
		{
			pThis->NotifyView(szEventActiveObjectChange, 0, UPDATE_SELECTION);
			pThis->NotifyContextView(szEventActiveContextChange, 0, ncRescan);
		}
		return S_OK;
	}
}

HRESULT CDataContext::XContext::GetLockUpdate(BOOL * pbLock)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!pbLock)
			return E_INVALIDARG;

		*pbLock = !pThis->m_bContextNotificationEnabled;
		
		return S_OK;
	}
}

// IDataContext2
//
// object types
// return count of types in context
HRESULT CDataContext::XContext::GetObjectTypeCount(long * plCount)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plCount)
			return E_INVALIDARG;

		*plCount = pThis->m_arrObjManagers.GetSize();
		return S_OK;
	}
}

// return name of type by pos
// pos MUST be in range (0,  GetObjectTypeCount())
HRESULT CDataContext::XContext::GetObjectTypeName(LONG_PTR lPos, BSTR * pbstrType)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!pbstrType || (lPos < 0 || lPos > pThis->m_arrObjManagers.GetSize()))
			return E_INVALIDARG;

		CObjectManager * pMan = pThis->m_arrObjManagers[lPos];
		if (!pMan)
			return E_FAIL;
		
		CString str = pMan->GetType();
		*pbstrType = str.AllocSysString();
	
		return S_OK;
	}
}

// type enables

// get disable flag  for object manager from context 
// (objects w/ defined type can be disabled to get it from context in some cases)
HRESULT CDataContext::XContext::GetTypeEnable(BSTR bstrType, BOOL * pbEnable)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!bstrType || !pbEnable)
			return E_INVALIDARG;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*pbEnable = pThis->ManagerIsAvailable(pMgr);
		return S_OK;
	}
}

// types enable
// return string that contains array of string w/ '\n' 
// that shown which types of objects context's View can show
HRESULT CDataContext::XContext::GetTypesEnable(BSTR * pbstrTypes)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!pbstrTypes)
			return E_INVALIDARG;

		CString strTarget;
		// create string that contains string array of available types
		for (int i = 0; i < pThis->m_arrObjManagers.GetSize(); i++)
		{
			if (pThis->ManagerIsAvailable(pThis->m_arrObjManagers[i]))
				strTarget += CString(pThis->m_arrObjManagers[i]->GetType()) + _T('\n');
		}
		// and set it to param
		*pbstrTypes = strTarget.AllocSysString();
		return S_OK;
	}
}

// objects 

// return count of objects of given type  
HRESULT CDataContext::XContext::GetObjectCount(BSTR bstrType, long * plCount)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		_bstr_t bType(bstrType);
		if (!bType.length() || !plCount)
			return E_INVALIDARG;

		*plCount = 0;
		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*plCount = pMgr->GetCount();
		return S_OK;
	}
}

// return position of first object of given type  
HRESULT CDataContext::XContext::GetFirstObjectPos(BSTR bstrType, LONG_PTR * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		_bstr_t bType(bstrType);
		if (!bType.length() || !plPos)
			return E_INVALIDARG;

		*plPos = 0;
		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*plPos = (long)pMgr->GetFirstObjectPos();

		return S_OK;
	}
}

// return object of of given type  by given position and advance position to next object
HRESULT CDataContext::XContext::GetNextObject(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		_bstr_t bType(bstrType);
		if (!bType.length() || !plPos || !ppunkObject)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		*ppunkObject = 0;
		if (!pos)
			return E_INVALIDARG;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		CContextObject * pObj = pMgr->GetNextObject(pos);
		*plPos = (LONG_PTR)pos;

		if (pObj)
			*ppunkObject = pObj->GetPtr(true);

		return S_OK;
	}
}

// return position of last object of given type  
HRESULT CDataContext::XContext::GetLastObjectPos(BSTR bstrType, LONG_PTR * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		_bstr_t bType(bstrType);
		if (!bType.length() || !plPos)
			return E_INVALIDARG;

		*plPos = 0;
		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*plPos = (long)pMgr->GetLastObjectPos();

		return S_OK;
	}
}

// return object of given type by given position and advance position to prev object
HRESULT CDataContext::XContext::GetPrevObject(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		_bstr_t bType(bstrType);
		if (!bType.length() || !plPos || !ppunkObject)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		*ppunkObject = 0;
		if (!pos)
			return E_INVALIDARG;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		CContextObject * pObj = pMgr->GetPrevObject(pos);
		*plPos = (LONG_PTR)pos;

		if (pObj)
			*ppunkObject = pObj->GetPtr(true);

		return S_OK;
	}
}

// return position of object of given type
HRESULT CDataContext::XContext::GetObjectPos(BSTR bstrType, IUnknown * punkObject, LONG_PTR * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		_bstr_t bType(bstrType);
		INamedDataObject2Ptr sptrObj(punkObject);
		if (!bType.length() && punkObject)
			bType = ::GetObjectKind(punkObject);

		if (!bType.length() || !plPos || sptrObj == 0)
			return E_INVALIDARG;

		*plPos = 0;
		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*plPos = (long)pMgr->FindObjectPos(sptrObj);

		return S_OK;
	}
}


// return flag shows object is available to show in view
HRESULT CDataContext::XContext::GetObjectEnable(IUnknown * punkObject, BOOL * pbFlag)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!punkObject || !pbFlag)
			return E_INVALIDARG;

		_bstr_t bstrType = ::GetObjectKind(punkObject);
		*pbFlag = true;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr || !pThis->ManagerIsAvailable(pMgr))
			*pbFlag = false;

		return S_OK;
	}
}



// child objects
// return count of child objects of given parent of given type
HRESULT CDataContext::XContext::GetChildrenCount(BSTR bstrType, IUnknown * punkParent, long * plCount)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plCount)
			return E_INVALIDARG;

		*plCount = 0;

		_bstr_t bType(bstrType);
		if (!bType.length())
			bType = ::GetObjectKind(punkParent);

		CObjectManager * pMgr = pThis->GetManager(bType); 
		if (!pMgr)
			return E_INVALIDARG;
		
		CContextObject * pParent = pMgr->GetCommon();
		if (punkParent)
		{
			sptrINamedDataObject2 sptrParent(punkParent);
			pParent = pMgr->FindObject(sptrParent);
		}
		if (!pParent)
			return E_INVALIDARG;

		*plCount = pParent->GetChildrenCount();

		return S_OK;
	}
}

HRESULT CDataContext::XContext::GetFirstChildPos(BSTR bstrType, IUnknown * punkParent, long * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos)
			return E_INVALIDARG;
		*plPos = 0;

		_bstr_t bType(bstrType);
		if (!bType.length())
			bType = ::GetObjectKind(punkParent);

		CObjectManager * pMgr = pThis->GetManager(bType); 
		if (!pMgr)
			return E_INVALIDARG;
		
		CContextObject * pParent = pMgr->GetCommon();
		if (punkParent)
		{
			sptrINamedDataObject2 sptrParent(punkParent);
			pParent = pMgr->FindObject(sptrParent);
		}

		if (!pParent)
			return E_INVALIDARG;

		*plPos = (long)pParent->GetChildFirstPos();
		return S_OK;
	}
}

HRESULT CDataContext::XContext::GetNextChild(BSTR bstrType, IUnknown * punkParent, long * plPos, IUnknown ** ppunkChild)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos || !ppunkChild)
			return E_INVALIDARG;
		
		*ppunkChild = 0;

		_bstr_t bType(bstrType);
		if (!bType.length())
			bType = ::GetObjectKind(punkParent);

		CObjectManager * pMgr = pThis->GetManager(bType); 
		if (!pMgr)
			return E_INVALIDARG;

		CContextObject * pParent = pMgr->GetCommon();
		if (punkParent)
		{
			sptrINamedDataObject2 sptrParent(punkParent);
			pParent = pMgr->FindObject(sptrParent);
		}
		if (!pParent)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		CContextObject * pChild = pParent->GetNextChild(pos);
		*plPos = (LONG_PTR)pos;

		if (pChild)
			*ppunkChild = (IUnknown*)pChild->GetPtr(true);

		return S_OK;
	}
}

HRESULT CDataContext::XContext::GetLastChildPos(BSTR bstrType, IUnknown * punkParent, long * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos)
			return E_INVALIDARG;
		*plPos = 0;

		_bstr_t bType(bstrType);
		if (!bType.length())
			bType = ::GetObjectKind(punkParent);

		CObjectManager * pMgr = pThis->GetManager(bType); 
		if (!pMgr)
			return E_INVALIDARG;
		
		CContextObject * pParent = pMgr->GetCommon();
		if (punkParent)
		{
			sptrINamedDataObject2 sptrParent(punkParent);
			pParent = pMgr->FindObject(sptrParent);
		}
		if (!pParent)
			return E_INVALIDARG;

		*plPos = (long)pParent->GetChildLastPos();
		return S_OK;
	}
}

HRESULT CDataContext::XContext::GetPrevChild(BSTR bstrType, IUnknown * punkParent, long * plPos, IUnknown ** ppunkChild)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos || !ppunkChild)
			return E_INVALIDARG;
		
		*ppunkChild = 0;

		_bstr_t bType(bstrType);
		if (!bType.length())
			bType = ::GetObjectKind(punkParent);

		CObjectManager * pMgr = pThis->GetManager(bType); 
		if (!pMgr)
			return E_INVALIDARG;

		CContextObject * pParent = pMgr->GetCommon();
		if (punkParent)
		{
			sptrINamedDataObject2 sptrParent(punkParent);
			pParent = pMgr->FindObject(sptrParent);
		}
		if (!pParent)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		CContextObject * pChild = pParent->GetPrevChild(pos);
		*plPos = (LONG_PTR)pos;
		if (pChild)
			*ppunkChild = (IUnknown*)pChild->GetPtr(true);

		return S_OK;
	}
}

HRESULT CDataContext::XContext::GetChildPos(BSTR bstrType, IUnknown * punkParent, IUnknown * punkChild, long * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos || !punkChild || !plPos)
			return E_INVALIDARG;

		*plPos = 0;

		_bstr_t bType(bstrType);
		if (!bType.length())
			bType = ::GetObjectKind(punkParent);

		CObjectManager * pMgr = pThis->GetManager(bType); 
		if (!pMgr)
			return E_INVALIDARG;
		
		CContextObject * pParent = pMgr->GetCommon();
		if (punkParent)
		{
			sptrINamedDataObject2 sptrParent(punkParent);
			pParent = pMgr->FindObject(sptrParent);
		}
		if (!pParent)
			return E_INVALIDARG;

		sptrINamedDataObject2 sptrChild(punkChild);
		CContextObject * pChild = pMgr->FindObject(sptrChild);
		if (pChild)
			*plPos = (long)pParent->FindChild(pChild);

		return S_OK;
	}
}


// selected objects
// get flag that shows object is selected
HRESULT CDataContext::XContext::GetObjectSelect(IUnknown * punkObject, BOOL * pbFlag)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		INamedDataObject2Ptr sptr = punkObject;

		if (sptr == 0 || !pbFlag)
			return E_INVALIDARG;

		_bstr_t bstrType = ::GetObjectKind(punkObject);

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		CContextObject * pObj = pMgr->FindObject(sptr);
		if (!pObj)
			return E_INVALIDARG;

		*pbFlag = pMgr->IsObjectSelect(pObj);

		return S_OK;
	}
}

#include "scriptnotifyint.h"

// Set flag that shows object is selected
HRESULT CDataContext::XContext::SetObjectSelect(IUnknown * punkObject, BOOL bFlag)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		INamedDataObject2Ptr sptr = punkObject;
		if (sptr == 0)
			return E_INVALIDARG;

		CContextObject * pObj = pThis->FindObject(sptr);
		bool bUpdate = false;
		
		bool bRet = true;
		if (bFlag)
		{
			bRet = pThis->_SelectObject(sptr);
			bUpdate = pObj && ObjectIsActive(pObj);
		}
		else
		{
			bUpdate = pObj && ObjectIsActive(pObj);
			bRet = pThis->_DeselectObject(sptr);
		}
		// notify view if needed

		bUpdate = true;
		if (bRet && bUpdate)
			pThis->NotifyView(szEventActiveObjectChange, punkObject, ACTIVATE_OBJECT);

 		if (bRet)
			// notify context
			pThis->NotifyContextView(szEventActiveContextChange, punkObject, ncUpdateSelection);

		IScriptSitePtr	sptr_script_site = ::GetAppUnknown();
		if (bRet)
		{
			_bstr_t bstr_type = ::GetObjectKind( sptr );

			CObjectManager * pMgr = pThis->GetManager( bstr_type );

			if ( pMgr)
			{
				_variant_t vart_arg[2] = { pMgr->GetSelectedCount(), bstr_type };
				IScriptSitePtr	sptr_script_site = ::GetAppUnknown();
				sptr_script_site->Invoke( _bstr_t( L"DataContext_" szEventContextSelectionChange), vart_arg, 2, 0, fwFormScript | fwAppScript );
			}
		}

		return bRet ? S_OK : E_FAIL;
	}
}

// get count of selected object of given type
HRESULT CDataContext::XContext::GetSelectedCount(BSTR bstrType, long * plCount)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plCount)
			return E_INVALIDARG;
		*plCount = 0;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*plCount = pMgr->GetSelectedCount();
		return S_OK;
	}
}

// get position of first selected object or 0 
// if context has no selected objects of such type
HRESULT CDataContext::XContext::GetFirstSelectedPos(BSTR bstrType, LONG_PTR * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos)
			return E_INVALIDARG;

		*plPos = 0;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*plPos = (long)pMgr->GetFirstSelectedPos();
		return S_OK;
	}
}

// get selected object by its pos and advance position to next selected object or 0 
// if context has no more selected objects of such type
HRESULT CDataContext::XContext::GetNextSelected(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos || !(*plPos) || !ppunkObject)
			return E_INVALIDARG;

		*ppunkObject = 0;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		CContextObject * pObj = pMgr->GetNextSelected(pos);
		*plPos = (LONG_PTR)pos;

		if (!pObj)
			return E_INVALIDARG;

		*ppunkObject = pObj->GetPtr(true);
		return S_OK;
	}
}

// get position of last selected object or 0 
// if context has no selected objects of such type
HRESULT CDataContext::XContext::GetLastSelectedPos(BSTR bstrType, LONG_PTR * plPos)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos)
			return E_INVALIDARG;

		*plPos = 0;
		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		*plPos = (long)pMgr->GetLastSelectedPos();
		return S_OK;
	}
}

// get selected object by its pos and advance position to prev selected object or 0 
// if context has no more selected objects of such type
HRESULT CDataContext::XContext::GetPrevSelected(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!plPos || !(*plPos) || !ppunkObject)
			return E_INVALIDARG;

		*ppunkObject = 0;

		CObjectManager * pMgr = pThis->GetManager(bstrType);
		if (!pMgr)
			return E_INVALIDARG;

		POSITION pos = (POSITION)*plPos;
		CContextObject * pObj = pMgr->GetPrevSelected(pos);
		*plPos = (LONG_PTR)pos;

		if (!pObj)
			return E_INVALIDARG;

		*ppunkObject = pObj->GetPtr(true);
		return S_OK;
	}
}

HRESULT CDataContext::XContext::GetSelectedObjectNumber(BSTR bstrType, IUnknown * punkObject, long * plNumber)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		INamedDataObject2Ptr sptrObj = punkObject;
		if (sptrObj == 0 || !plNumber)
			return E_INVALIDARG;

		*plNumber = -1;
		_bstr_t bstrObjType(bstrType);
		if (!bstrObjType.length())
			bstrObjType = ::GetObjectKind(punkObject);

		CObjectManager * pMgr = pThis->GetManager(bstrObjType);
		if (!pMgr)
			return E_INVALIDARG;

		CContextObject * pObj = pMgr->FindObject(sptrObj);
		if (pObj && pObj->GetPosInSelect())
		{
			*plNumber = 1;
			for (POSITION pos = pMgr->GetFirstSelectedPos(); pos != NULL; )
			{
				CContextObject * pCmpObj = pMgr->GetNextSelected(pos);
				if (pCmpObj == pObj)
					break;

				(*plNumber)++;
			}
		}
		return S_OK;
	}
}

// set count to available selected object
HRESULT CDataContext::XContext::UnselectAll(BSTR bstrType)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		CString str = bstrType;
		pThis->_UnselectAll(str);
		return S_OK;
	}
}

// last active object
HRESULT CDataContext::XContext::GetLastActiveObject(BSTR bstrTypes, IUnknown ** ppunkObj)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)

	if (!ppunkObj)// || !bstrTypes)
		return E_INVALIDARG;

	(*ppunkObj) = pThis->GetLastActiveObject( bstrTypes, true );

	return S_OK;
}

// synchronize
HRESULT CDataContext::XContext::SynchronizeWithContext(IUnknown * punkContext)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		return pThis->SynchronizeWithContext(punkContext) ? S_OK : E_FAIL; 
	}
}


// serialize
// store context to text file (string)
HRESULT CDataContext::XContext::StoreToString(BSTR * pbstrString)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		if (!pbstrString)
			return E_INVALIDARG;

		CStringArray	sa;
		CString			strTarget;
		
		// store context to string array
		if (!pThis->StoreToText(sa))
			return E_FAIL;

		// transform string array to string 
		ArrayToString(strTarget, sa);
		*pbstrString = strTarget.AllocSysString();

		return S_OK;
	}
}

// load context from text file (string)
HRESULT CDataContext::XContext::LoadFromString(BSTR bstrString)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		CString			strSource = bstrString;
		CStringArray	sa;

		// transform string to string array
		ArrayFromString(sa, strSource);

		// and load context from string array
		if (!pThis->LoadFromText(sa))
			return E_FAIL;

		return S_OK;
	}
}

// serialize context to archive
HRESULT CDataContext::XContext::StoreToArchive(IUnknown * punkOleStream)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		/*
		// from stream create CArchive
		IStreamPtr sptrStream = punkOleStream;
		if (sptrStream == 0)
			return E_INVALIDARG;

		COleStreamFile	file(sptrStream);
		CArchive		ar(&file, CArchive::store, 35);
		*/


		IStreamPtr ptrStream = punkOleStream;
		CStreamEx stream( ptrStream, false );
		// and serialize object
		return pThis->SerializeObject( stream, 0 ) ? S_OK : E_FAIL;
	}
}

// serialize context from archive
HRESULT CDataContext::XContext::LoadFromArchive(IUnknown * punkOleStream)
{
	METHOD_PROLOGUE_EX(CDataContext, Context)
	{
		/*
		// from stream create CArchive
		IStreamPtr sptrStream = punkOleStream;
		if (sptrStream == 0)
			return E_INVALIDARG;

		COleStreamFile	file(sptrStream);
		CArchive		ar(&file, CArchive::load, 35);
		*/
		IStreamPtr ptrStream = punkOleStream;
		CStreamEx stream( ptrStream, true );

		// and serialize object
		return pThis->SerializeObject( stream, 0 ) ? S_OK : E_FAIL;
	}
}


// IDataContext3


// sync
// object managers
HRESULT CDataContext::XSync::GetObjManagerCount(int * pnCount)
{
	METHOD_PROLOGUE_EX(CDataContext, Sync)
	{
		if (!pnCount)
			return E_INVALIDARG;

		*pnCount = pThis->m_arrObjManagers.GetSize();
		return S_OK;
	}
}

HRESULT CDataContext::XSync::GetObjManager(int nPos, BSTR * pbstrType, long * plTime)
{
	METHOD_PROLOGUE_EX(CDataContext, Sync)
	{
		if (!pbstrType && !plTime)
			return E_INVALIDARG;

		if (nPos < 0 || nPos >= pThis->m_arrObjManagers.GetSize())
			return E_INVALIDARG;

		CObjectManager * pMan = pThis->m_arrObjManagers[nPos];
		if (!pMan)
			return E_FAIL;

		if (pbstrType)
		{
			CString str = pMan->GetType();
			*pbstrType  = str.AllocSysString();
		}

		if (plTime)
			*plTime = pMan->GetLastTimeUsed();

		return S_OK;
	}
}

HRESULT CDataContext::XSync::GetObjManagerTime(BSTR bstrType, long * plTime)
{
	METHOD_PROLOGUE_EX(CDataContext, Sync)
	{
		if (!bstrType || !plTime)
			return E_INVALIDARG;

		CObjectManager * pMan = pThis->GetManager(bstrType);
		if (!pMan)
			return E_INVALIDARG;

		if (plTime)
			*plTime = pMan->GetLastTimeUsed();

		return S_OK;
	}
}

HRESULT CDataContext::XSync::SetObjManagerTime(BSTR bstrType, long lTime)
{
	METHOD_PROLOGUE_EX(CDataContext, Sync)
	{
		if (!bstrType)
			return E_INVALIDARG;

		CObjectManager * pMan = pThis->GetManager(bstrType);
		if (!pMan)
			return E_INVALIDARG;

		pMan->SetLastTimeUsed(lTime);

		return S_OK;
	}
}

HRESULT CDataContext::XSync::GetObjManagerTime(int nPos, long * plTime)
{
	METHOD_PROLOGUE_EX(CDataContext, Sync)
	{
		if (!plTime)
			return E_INVALIDARG;

		if (nPos < 0 || nPos >= pThis->m_arrObjManagers.GetSize())
			return E_INVALIDARG;

		CObjectManager * pMan = pThis->m_arrObjManagers[nPos];
		if (!pMan)
			return E_FAIL;

		if (plTime)
			*plTime = pMan->GetLastTimeUsed();

		return S_OK;
	}
}

HRESULT CDataContext::XSync::SetObjManagerTime(int nPos, long lTime)
{
	METHOD_PROLOGUE_EX(CDataContext, Sync)
	{
		if (nPos < 0 || nPos >= pThis->m_arrObjManagers.GetSize())
			return E_INVALIDARG;

		CObjectManager *pMan = pThis->m_arrObjManagers[nPos];
		if (!pMan)
			return E_FAIL;

		pMan->SetLastTimeUsed(lTime);

		return S_OK;
	}
}


// data key
HRESULT CDataContext::XSync::GetDataKey(GUID * pDataKey)
{
	METHOD_PROLOGUE_EX(CDataContext, Sync)
	{
		if (!pDataKey)
			return E_INVALIDARG;
		*pDataKey = (pThis->m_sptrINamedData != 0) ? ::GetObjectKey(pThis->m_sptrINamedData) : INVALID_KEY;
		return S_OK;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
// inner function implementation
// attach context to NamedData object
bool CDataContext::AttachData(INamedData * pNamedData)
{
	XNotifyLock lock(this);

	// if punkNamedData == 0 and previous current INamedData != 0
	// we think we need to detach context from data
	if (m_sptrINamedData != NULL)
	{
		m_sptrINamedData->RemoveContext(GetControllingUnknown());
		m_sptrINamedData = 0;
		Clear();
	}

	// get INamedData
	if (!pNamedData)
		return true;
	
	bool bDataContext = false;

	// check context contained in document. 
	if (CheckInterface(GetControllingUnknown(), IID_INamedData))
	{
		// in this case we must init context itself
		ASSERT(::GetObjectKey(pNamedData) ==  ::GetObjectKey(GetControllingUnknown()));
		m_sptrINamedData = GetControllingUnknown();
		bDataContext = true;
	}
	else
		m_sptrINamedData = pNamedData;    // named data
		

	// initialize context
	bool bInit = InitializeContext();
	ASSERT (bInit);

	IUnknown * punkActiveContext = 0;
	if (m_sptrINamedData != 0)
		m_sptrINamedData->GetActiveContext(&punkActiveContext);

	if (!bDataContext && punkActiveContext)
	{
		bool bRet = SynchronizeWithContext(punkActiveContext);
		ASSERT(bRet);
	}
	if (punkActiveContext)
		punkActiveContext->Release();

	// add this context to named Data
	m_sptrINamedData->AddContext(GetControllingUnknown());

	return true;
}

// check context can give objects from given manager
bool CDataContext::ManagerIsAvailable(CObjectManager * pMgr)
{
	if (!pMgr)
		return false;

	IViewPtr sptrV = GetControllingUnknown();
	if (sptrV == 0)
		return true;
	
	DWORD dwMatch = 0;
	if (FAILED(sptrV->GetMatchType(pMgr->GetType(), &dwMatch)))
		return false;

	return dwMatch != mvNone;
}

bool CDataContext::ChangeBaseObject(INamedDataObject2 * punkObj)
{
	NotifyContextView(szEventActiveContextChange, punkObj, ncChangeBase);
/*
	BOOL bBase = false;
	if (punkObj)
		punkObj->IsBaseObject(&bBase);

	CContextObject * pObj = FindObject(punkObj);
	if (pObj && ObjectIsActive(pObj) && bBase)
		_SetActiveObject(punkObj);
*/
	return true;
}

bool CDataContext::ChangeObjectKey(INamedDataObject2 * punkObj, GuidKey & rPrevKey)
{
	if (rPrevKey == INVALID_KEY)
		return false;

	GuidKey NewKey = ::GetObjectKey(punkObj);
	if (rPrevKey == NewKey || NewKey == INVALID_KEY)
		return false;

	bool bFound = false;
	for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
	{	
		CObjectManager * pMgr = m_arrObjManagers[i];
		if (pMgr)
		{
			CContextObject * pObj = 0;
			if (pMgr->m_mapObjects.Lookup(rPrevKey, pObj))
			{
				pMgr->m_mapObjects.RemoveKey(rPrevKey);
				pMgr->m_mapObjects.SetAt(NewKey, pObj);
				bFound = true;
				break;
			}
		}
	}

	NotifyContextView2(szEventActiveContextChangeObjectKey, punkObj, (LPARAM)&rPrevKey);

	return bFound;
}

bool CDataContext::EnterFilter(IUnknown * punkFilter)
{
	IFilterActionPtr sptrF = punkFilter;
	if (sptrF == 0)
		return false;

	m_bInsideFilter = true;

	return true;
}

bool CDataContext::PassFilter(IUnknown * punkFilter)
{
	IFilterAction2Ptr sptrF = punkFilter;
	if (sptrF == 0)
		return false;

	m_bInsideFilter = false;

	bool bRet = true;
	_try(CDataContext, PassFilter)
	{
		bool bAnyDeletedArgs = false;
		TPOS lPos = 0;
		sptrF->GetFirstArgumentPosition(&lPos);

		while (lPos)
		{
			TPOS lObjPos = lPos;
			IUnknownPtr sptrUnkObj;
			sptrF->GetNextArgument(&sptrUnkObj, &lPos);

			if (sptrUnkObj == 0)
				continue;

			BSTR bstrArgType = 0;
			BSTR bstrArgName = 0;
			BOOL bOut = false;
			sptrF->GetArgumentInfo(lObjPos, &bstrArgType, &bstrArgName, &bOut);
			_bstr_t bstrType(bstrArgType, false);
			_bstr_t bstrNamed(bstrArgName, false);

			BOOL bCanDelete = false; 
			int  bNeedActivate = 0; 
			sptrF->CanDeleteArgument(sptrUnkObj, &bCanDelete);
			sptrF->NeedActivateResult(sptrUnkObj, &bNeedActivate);
			if (bOut)
			{
				INamedDataObject2Ptr sptrN = sptrUnkObj;
				CContextObject * pObj = FindObject(sptrN);
				if (bNeedActivate &&
					NeedActivateObject(pObj, false, bNeedActivate) &&
					!_SetActiveObject(sptrN, 0, true, true))
					bRet = false;
			}
			else if (bCanDelete)
				bAnyDeletedArgs = true;
		}
		if (bAnyDeletedArgs)
			NotifyView(szEventActiveObjectChange, 0, ACTIVATE_OBJECT);
	}
	_catch
	{
		return false;
	}
	return true;
}

bool CDataContext::CancelFilter(IUnknown * punkFilter)
{
	IFilterActionPtr sptrF = punkFilter;
	if (sptrF == 0)
		return false;

	m_bInsideFilter = false;
	return true;
}

bool CDataContext::RedoFilter(IUnknown * punkFilter)
{
	return PassFilter(punkFilter);
}

bool CDataContext::UndoFilter(IUnknown * punkFilter)
{
	IFilterAction2Ptr sptrF = punkFilter;
	if (sptrF == 0)
		return false;

	m_bInsideFilter = false;

	bool bRet = true;
	_try(CDataContext, PassFilter)
	{
		bool bAnyDeletedArgs = false;
		// A.M. A.M. BT 4652. See comment on declaration of _MakeFilterArgumentsArray.
		CArray<TPOS, TPOS&> arrArgs;
		_MakeFilterArgumentsArray(sptrF,arrArgs);
		for (int i = 0; i < arrArgs.GetSize(); i++)
		{
			TPOS lPos = arrArgs.GetAt(i);
			TPOS lObjPos =lPos;
			IUnknownPtr sptrUnkObj;
			sptrF->GetNextArgument(&sptrUnkObj, &lPos);

			if (sptrUnkObj == 0)
				continue;

			BSTR bstrArgType = 0;
			BSTR bstrArgName = 0;
			BOOL bOut = false;
			sptrF->GetArgumentInfo(lObjPos, &bstrArgType, &bstrArgName, &bOut);
			_bstr_t bstrType(bstrArgType, false);
			_bstr_t bstrNamed(bstrArgName, false);

			CString sObjName = GetObjectName(sptrUnkObj);
			BOOL bCanDelete = false; 
			sptrF->CanDeleteArgument(sptrUnkObj, &bCanDelete);
			int  bNeedActivate = 0; 
			sptrF->NeedActivateResult(sptrUnkObj, &bNeedActivate);
			if (!bOut)
			{
				INamedDataObject2Ptr sptrN = sptrUnkObj;
				CContextObject * pObj = FindObject(sptrN);
				if (NeedActivateObject(pObj, false, bNeedActivate) && !_SetActiveObject(sptrN, 0, true, true))
					bRet = false;
			}
			else if (bCanDelete)
				bAnyDeletedArgs = true;
		}
		if (bAnyDeletedArgs)
			NotifyView(szEventActiveObjectChange, 0, ACTIVATE_OBJECT);
	}
	_catch
	{
		return false;
	}
	return true;
}

void CDataContext::_MakeFilterArgumentsArray(IFilterAction2Ptr sptrF, CArray<TPOS, TPOS&> &arrArgs)
{
	TPOS lPos = 0;
	sptrF->GetFirstArgumentPosition(&lPos);
	while (lPos)
	{
		TPOS lObjPos = lPos;
		IUnknownPtr sptrUnkObj;
		sptrF->GetNextArgument(&sptrUnkObj, &lPos);
		bool bParentOfSomeObject = false;
		for (int i = 0; i < arrArgs.GetSize(); i++)
		{
			TPOS lPosTest = arrArgs.GetAt(i);
			IUnknownPtr sptrUnkTest;
			sptrF->GetNextArgument(&sptrUnkTest, &lPosTest);
			if (::GetParentKey(sptrUnkTest) == ::GetObjectKey(sptrUnkObj))
			{
				bParentOfSomeObject = true;
				break;
			}

		}
		if (bParentOfSomeObject)
			arrArgs.InsertAt(0, lObjPos);
		else
			arrArgs.Add(lObjPos);
	}
}


// deselect all object of given type w/ notification
bool CDataContext::_UnselectAll(CString & str)
{
	// if manager is available
	CObjectManager * pMgr = GetManager(_bstr_t(str));
	if (!pMgr)
		return false;

	pMgr->DeselectAll();
	
	return true;
}

// select object
bool CDataContext::_SelectObject(INamedDataObject2 * pObject, BSTR bstrObjType)
{
	if (!pObject)
		return false;

	_bstr_t bstrType(bstrObjType);
	// get manager 
	if (!bstrType.length())
		bstrType = ::GetObjectKind(pObject);
	 
	CObjectManager * pMgr = GetManager(bstrType);
	if (!pMgr)
		return false;

	// find object
	CContextObject * pObj = pMgr->FindObject(pObject);
	if (!pObj)
		return false;

	// select object
	return pMgr->SelectObject(pObj);
}


/////////////////////////////////////////////
bool CDataContext::_DeselectObject(INamedDataObject2 * pObject, BSTR bstrObjType)
{
	if (!pObject)
		return false;

	_bstr_t bstrType(bstrObjType);
	// get manager 
	if (!bstrType.length())
		bstrType = ::GetObjectKind(pObject);
	 
	CObjectManager * pMgr = GetManager(bstrType);
	if (!pMgr)
		return false;

	// find object
	CContextObject * pObj = pMgr->FindObject(pObject);
	if (!pObj)
		return false;

	// check previously selected
	return (pMgr->IsObjectSelect(pObj)) ? pMgr->DeselectObject(pObj) : false;
}

IUnknown* CDataContext::GetLastActiveObject(BSTR bstrTypes, bool bAddRef)
{
	Sort();

	// get types array
	CStringList list;
	CString strTypes = bstrTypes;
	
	if (strTypes.GetLength() && strTypes[strTypes.GetLength() - 1] != _T('\n'))
		strTypes += _T('\n');

	int nLast = strTypes.Find(_T('\n'));

	int nFirst = 0;
	while (nLast != -1)
	{
		list.AddTail(strTypes.Mid(nFirst, nLast - nFirst));
		nFirst = nLast + 1;
		nLast = strTypes.Find(_T('\n'), nFirst);
	}

	INamedDataObject2Ptr sptr;
	CObjectManager * pMan = 0;


	// find 
	for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
	{
		CString strType = m_arrObjManagers[i]->GetType();
		if((!list.GetCount() || list.Find(strType)) && ManagerIsAvailable(m_arrObjManagers[i]))
		{
			pMan = m_arrObjManagers[i];
			IUnknown	*punkObject = pMan->GetActive( bAddRef );

			if( punkObject != 0 )
				return punkObject;
		}
	}
	return 0;
}


int CompMgr(const void *arg1, const void *arg2)
{
	CObjectManager * pMan1 = (CObjectManager*)arg1;
	CObjectManager * pMan2 = (CObjectManager*)arg2;

	if (!pMan2 && !pMan1)
		return 0;
	else if (!pMan1)
		return -1;
	else if (!pMan2)
		return 1;

	return pMan1->GetLastTimeUsed() - pMan2->GetLastTimeUsed();
}

int FindGreatest(CNDOManagers & arr)
{
	int nSize = arr.GetSize();
	if (!nSize)
		return -1;

	if (nSize == 1)
		return 0;

	CObjectManager * pMgr = arr[0];
	int nIndex = 0;

	if (!pMgr)
		return -1;

	for (int i = 1; i < nSize; i++)
	{
		CObjectManager * pCmpMgr = arr[i];

		if (!pCmpMgr)
			continue;
		
		if (pMgr->GetLastTimeUsed() < pCmpMgr->GetLastTimeUsed())
		{
			nIndex = i;
			pMgr = pCmpMgr;
		}
		
	}
	return nIndex;
}

// new version that used 'qsort'
bool CDataContext::Sort()
{
	int nCount = m_arrObjManagers.GetSize();
	if (nCount < 2)
		return true;

	CNDOManagers arr;
	for (int i = 0; i < nCount; i++)
	{
		int nIndex = FindGreatest(m_arrObjManagers);
		if (nIndex < 0)
		{
			ASSERT(FALSE);
			continue;
		}
		arr.Add(m_arrObjManagers[nIndex]);
		m_arrObjManagers.RemoveAt(nIndex);
	}
	ASSERT(m_arrObjManagers.GetSize() == 0);
	m_arrObjManagers.Copy(arr);


//	LPVOID lptr = (LPVOID)m_arrObjManagers.GetData();
//	qsort(lptr, nCount, sizeof(CObjectManager*), CompMgr);

	return true;
}

/*
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
*/
bool CDataContext::ActivateMgr(CObjectManager * pMgr)
{
	if (!pMgr)
		return false;

	int nPos = -1;
	for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
	{
		if (m_arrObjManagers[i] == pMgr)
		{
			nPos = i;
			break;
		}
	}
	if (nPos == -1)
		return false;

	pMgr->SetLastTimeUsed(CDataContext::s_lCount++);

	if (nPos == 0)
		return true;

	m_arrObjManagers.RemoveAt(nPos, 1);
	m_arrObjManagers.InsertAt(0, pMgr);

	return true;
}

// syncronize  this context w /given
bool CDataContext::SynchronizeWithContext(IUnknown * punkContext)
{
	bool bRet = true;
	_try(CDataContext, SynchronizeWithContext)
	{
		// source context
		IDataContext2Ptr sptrC = punkContext;
		if (sptrC == 0)
			return false;

		bool bNeedInit = true; // flag : initialization needed

		// Get source Doc key
		IUnknown * punkSrcData = 0;
		if (SUCCEEDED(sptrC->GetData(&punkSrcData)) && punkSrcData)
		{
			GuidKey SrcDataKey = ::GetObjectKey(punkSrcData);
			punkSrcData->Release();

			GuidKey SrcKey = ::GetObjectKey(sptrC);

			// get own doc key
			GuidKey OwnDataKey = ::GetObjectKey(m_sptrINamedData);
			GuidKey OwnKey = ::GetObjectKey(GetControllingUnknown());
		
			// if both contexts has one named data
			if (OwnDataKey == SrcDataKey && SrcKey != OwnKey)
				// we not need to init context
				bNeedInit = false;
		}

		if (bNeedInit) // we need init context 
		{
			
			
			//CTimeTest tt(true, "CDataContext, SynchronizeWithContext w/ Init");
			// so we sync contexts w/ help serialization 
			// create temporary mem file
			CMemFile file;
			// create store_archive from this file
			CArchive ar(&file, CArchive::store, 35);
			ar.Flush();
			// create archive stream from file
			CArchiveStream	stream(&ar);

			// serialize given context to archive stream
			if (FAILED(sptrC->StoreToArchive(&stream)))
				return false;

			ar.Flush();
			file.SeekToBegin();

			CArchive ar2(&file, CArchive::load, 35);			
			// create archive stream from file
			CArchiveStream	stream2( &ar2 );

			// create load_archive form mem file
			//CArchive arLoad(&file, CArchive::load, 35);
			// serialize this context from archive			
			
			CStreamEx _stream( &stream2, true );

			if (!SerializeObject( _stream, 0 ))
				return false;
				
			return true;
		}
		else // simply sync contexts w/ help sync_interface
		{
			//CTimeTest tt(true, "CDataContext, SynchronizeWithContext w/o Init");
			IDataContextSyncPtr sptrSync = punkContext;

			int nManCount = 0;
			if (FAILED(sptrSync->GetObjManagerCount(&nManCount)))
				return false;

			// for all managers
			for (int i = 0; i < nManCount; i++)
			{
				long lTime = 0;
				BSTR bstr = 0;
				sptrSync->GetObjManager(i, &bstr, &lTime);
				_bstr_t bstrType(bstr, false);

				// get our Manager
				CObjectManager* pMan = GetManager(bstrType);
				if (!pMan)
				{
					bRet = false;
					continue;
				}
				// set to our man's last time from same
				pMan->SetLastTimeUsed(lTime);

				// for all objects of this type
				LONG_PTR lObjPos = 0;
				if (FAILED(sptrC->GetLastObjectPos(bstrType, &lObjPos)) || !lObjPos)
					continue;

				while (lObjPos)
				{
					// get next object
					IUnknownPtr sptrUnkObject;
					if (FAILED(sptrC->GetPrevObject(bstrType, &lObjPos, &sptrUnkObject)))
					{
						bRet = false;
						break;
					}

					INamedDataObject2Ptr sptrObj = sptrUnkObject;

					// looking for this object in our context
					CContextObject* pObj = pMan->FindObject(sptrObj);
					if (!pObj)
					{
						bRet = false;
						continue;
					}
					pMan->ActivateObject(pObj);
				}

				// selected objects
				pMan->DeselectAll();

				LONG_PTR nSelPos = 0;
				sptrC->GetFirstSelectedPos(bstrType, &nSelPos);
				while (nSelPos)
				{
					IUnknownPtr sptrUnkSelObj;
					if (FAILED(sptrC->GetNextSelected(pMan->GetType(), &nSelPos, &sptrUnkSelObj)))
					{
						bRet = false;
						continue;
					}
					INamedDataObject2Ptr sptrSel = sptrUnkSelObj;
					if (!_SelectObject(sptrSel, bstrType))
						bRet = false;
				}

			}
			// sort managers ? 
			Sort();

			// for test purpose only
//			{
//				CStringArrayEx sa;
//				bool bStoreRet = StoreToText(sa);
//				sa.WriteFile("Context.txt");
//				sa.ReadFile("Context.txt");
//				bool bLoadRet = LoadFromText(sa);
//			}
		}

		// get types_string
		CString strTypes;
		for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
			strTypes += CString(m_arrObjManagers[i]->GetType()) + _T('\n');

		_bstr_t bstrTypes = strTypes;

		// get last used object from contex 
		INamedDataObject2Ptr sptrActiveObj = GetLastActiveObject(bstrTypes, false);

		NotifyView(szEventActiveObjectChange, sptrActiveObj, ACTIVATE_OBJECT);
    }
	_catch
	{
		return false;
	}
	return true;
}

bool CDataContext::_UnselectAll()
{
	bool bRet = true;
	for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
	{
		if (!_UnselectAll(CString(m_arrObjManagers[i]->GetType())))
			bRet = false;
	}
	return bRet;
}

// initialize context contens
bool CDataContext::InitializeContext()
{
	bool bRet = true;
	_try(CDataContext, InitializeContext)
	{
		//CTimeTest tt(true, "CDataContext::InitializeContext");
		// get IDataTypeManager
		sptrIDataTypeManager sptrTypeMgr(m_sptrINamedData);
		if (sptrTypeMgr == 0)
			return false;
		// get type count
		long nTypeCount = 0;
		sptrTypeMgr->GetTypesCount(&nTypeCount);
		
		for (int nCnt = 0; nCnt < nTypeCount; nCnt++)
		{
			// get type name
			BSTR pbstr;
			sptrTypeMgr->GetType(nCnt, &pbstr);
			_bstr_t bstrType(pbstr, false);

			// create manager of this type and add it to array
			CObjectManager * pMgr = CreateObjectManager(bstrType);
			if (!pMgr)
			{
				bRet = false;
				continue;
			}

			IUnknownPtr sptrObj;
			LONG_PTR lpos = 0;
			// get first object pos 
			sptrTypeMgr->GetObjectFirstPosition(nCnt, &lpos);

			while (lpos)
			{
				if (FAILED(sptrTypeMgr->GetNextObject(nCnt, &lpos, &sptrObj)))
				{
					bRet = false;
					continue;
				}

				INamedDataObject2Ptr sptr = sptrObj;
				if (sptr == 0)
				{
					bRet = false;
					continue;
				}

				if (!_AddNamedObjectSimple(sptr, bstrType))
					bRet = false;

			}
//			if (pMgr->GetCount())
//				_SetActiveObject(pMgr->GetObject(0)->GetPtr(false), pMgr->GetType());

		}
	}
	_catch // for all exceptions
	{
		return false;
	}
	return bRet;
}

// initialize context contens
bool CDataContext::_InitContext()
{
	bool bRet = true;
	_try(CDataContext, _InitContext)
	{
		//CTimeTest tt(true, "CDataContext::_InitContext");

		sptrIDataTypeManager sptrTypeMgr(m_sptrINamedData);
		if (sptrTypeMgr == 0)
			return false;

		// get type count
		long nTypeCount = 0;
		sptrTypeMgr->GetTypesCount(&nTypeCount);
		
		for (int nCnt = 0; nCnt < nTypeCount; nCnt++)
		{
			// get type name
			BSTR pbstr;
			sptrTypeMgr->GetType(nCnt, &pbstr);
			_bstr_t bstrType(pbstr, false);

			// create manager of this type and add it to array
			CObjectManager * pMgr = CreateObjectManager(bstrType);
			if (!pMgr)
			{
				bRet = false;
				continue;
			}

			IUnknownPtr sptrObj;
			LONG_PTR	lpos = 0;
			// get first object pos 
			sptrTypeMgr->GetObjectFirstPosition(nCnt, &lpos);

			while (lpos)
			{
				if (FAILED(sptrTypeMgr->GetNextObject(nCnt, &lpos, &sptrObj)))
				{
					bRet = false;
					continue;
				}
		
				INamedDataObject2Ptr sptr = sptrObj;
				if (sptr == 0)
				{
					bRet = false;
					continue;
				}

				if (!_AddNamedObjectSimple(sptr, bstrType))
					bRet = false;
			}
		}
	}
	_catch // for all exceptions
	{
		return false;
	}
	return bRet;
}


// walk throught NamedData and fill all managers from it
bool CDataContext::Walk(LPCTSTR path)
{
	_try(CDataContext, Walk)
	{
		CString CurrentPath((!path) ? "" : path);
		_bstr_t bstrPath(CurrentPath);

		// set section
		m_sptrINamedData->SetupSection(bstrPath);

		// get count of entries
		long EntryCount = 0;
		m_sptrINamedData->GetEntriesCount(&EntryCount);

		// for all entries
		for (int  i = 0; i < (int)EntryCount; i++)
		{
			_try(CDataContext, subWalk)
			{
				BSTR bstr = NULL;
				// get entry name
				m_sptrINamedData->GetEntryName(i, &bstr);

				// destructor will release system memory from bstr
				_bstr_t bstrName(bstr, false);
				// format a full path for entry
				_bstr_t bstrPathNew = bstrPath + "\\" + bstrName;
//				bstrPathNew += bstrName;
		
				// get value
				_variant_t var;
				m_sptrINamedData->GetValue(bstrPathNew, &var);

				// check for IUnknown
				if (var.vt == VT_UNKNOWN)
				{
					// it's must be namedObject
					IUnknown* punk = (IUnknown*)var;

					// get INamedDataObject2
					sptrINamedDataObject2 sptrObject(punk);

					//release the unknown pointer
					punk->Release();
			
					// get type of object
					BSTR bstrtype = NULL;
					sptrObject->GetType(&bstrtype);
					CString strObjType = bstrtype;
					::SysFreeString(bstrtype);

				// process this object in context

					// add object to corresponding manager					
					_AddNamedObject(sptrObject, _bstr_t(strObjType));


				// end of process this object in context

				} // IUnknown
				
				// if this entry has children => we want to walk to them
				if (EntryCount > 0)
					Walk(bstrPathNew);

				// for next entry on this level restore Section
				m_sptrINamedData->SetupSection(bstrPath);

			}
			_catch // for all exceptions
			{
				continue;
			}
		} // for all entries
	}
	_catch
	{
		return false;
	}
	return true;
}


// Rescan ind  refill context by Named Data
bool CDataContext::Rescan()
{
	bool bret = SynchronizeWithContext(GetControllingUnknown());
	NotifyContextView(szEventActiveContextChange, 0, ncRescan);

	return bret;
}

void CDataContext::NotifyView(LPCTSTR szEventDesc, IUnknown * punkObject, long lCode)
{
	if (!m_bContextNotificationEnabled || m_bInsideFilter)
		return;

	IViewPtr sptrV = GetControllingUnknown();
	if (sptrV == 0)
		return;

	if (!strcmp(szEventDesc, szEventActiveObjectChange))
		sptrV->OnUpdate(_bstr_t(szEventDesc), punkObject);
	else
	{
		IEventListenerPtr spEL = sptrV;
		if (spEL != 0)
			spEL->Notify(_bstr_t(szEventDesc), punkObject, GetControllingUnknown(), &lCode, sizeof( lCode ) );
	}
}


// notify context view
void CDataContext::NotifyContextView(LPCTSTR szEventDesc, IUnknown * punkObject, NotifyCodes ncCode)
{
	if (!m_bContextNotificationEnabled)
		return;

	IViewPtr sptrV = GetControllingUnknown();
	if (sptrV == 0)
		return;

	INotifyControllerPtr sptrNC = sptrV;
	if (sptrNC != 0)
		sptrNC->FireEvent(_bstr_t(szEventDesc), punkObject, GetControllingUnknown(), &ncCode, sizeof(ncCode) );
}

void CDataContext::NotifyContextView2(LPCTSTR szEventDesc, IUnknown * punkObject, LPARAM lParam)
{
	if (!m_bContextNotificationEnabled)
		return;

	IViewPtr sptrV = GetControllingUnknown();
	if (sptrV == 0)
		return;

	INotifyControllerPtr sptrNC = sptrV;
	if (sptrNC != 0)
		sptrNC->FireEvent(_bstr_t(szEventDesc), punkObject, GetControllingUnknown(), (void*)lParam, sizeof( lParam ) );
}



// managers ///////////////////////////////////////////////////////////////////

// fill array of managers
bool CDataContext::CreateManagers(IDataTypeManager* pDataTypeManager)
{
	if (!pDataTypeManager)
		return false;

	// get type count
	long TypeCount = 0;
	pDataTypeManager->GetTypesCount(&TypeCount);
	
	bool flag = true;
	for (int i = 0; i < TypeCount; i++)
	{
		// get type name
		BSTR pbstr;
		pDataTypeManager->GetType(i, &pbstr);
		_bstr_t bstrType(pbstr, false);

		// create manager of this type and add it to array
		if (!CreateObjectManager(bstrType))
			flag = false;
	}
	return flag;
}

// get manager by type
CObjectManager* CDataContext::GetManager(BSTR bstrtype)
{
	_bstr_t bstrType(bstrtype);
	for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
	{
		_bstr_t bstrMgrType(m_arrObjManagers[i]->GetType());
		if (bstrMgrType == bstrType)
			return m_arrObjManagers[i];
	}
	// not found
	return NULL;
}

// create manager by type and add it to array
CObjectManager* CDataContext::CreateObjectManager(BSTR bstrtype)
{
	CObjectManager * pobjmgr = NULL;
	// get type name
	_bstr_t bstrType(bstrtype);

	// check existing of ObjectManager with request type
	if (pobjmgr = GetManager(bstrType))
	// and return poinetr on it if it's exists
		return pobjmgr;
		
	// create manager of this type
	if (!(pobjmgr = new CObjectManager(bstrType)))
		return NULL;

	// and add it to array
	m_arrObjManagers.Add(pobjmgr);

	return pobjmgr;
}


// Objects ////////////////////////////////////////////////////////////////////

// add named object to corresponding manager
bool CDataContext::_AddNamedObjectSimple(INamedDataObject2 * pNamedObject, BSTR bstrtype)
{
	if (!pNamedObject)
		return false;

	_bstr_t	bstrTypeNamed(bstrtype);

	if (!bstrTypeNamed.length())
		bstrTypeNamed = ::GetObjectKind(pNamedObject);

	CString strName = ::GetObjectName(pNamedObject);
	// get corresponding manager
	CObjectManager * pMgr = GetManager(bstrTypeNamed);
	
	// if yet we have not manager of this type we must create it
	if (!pMgr && !(pMgr = CreateObjectManager(bstrTypeNamed)))
		return false;

	// add object if it's yet not exists in manager 
	if (!pMgr->FindObject(pNamedObject)) 
	{
		// if object has parent
		IUnknownPtr sptrUnkParent;
		pNamedObject->GetParent(&sptrUnkParent);
		INamedDataObject2Ptr sptrNParent = sptrUnkParent;

		// try to find parent in manager
		if (sptrNParent != 0 && !pMgr->FindObject(sptrNParent))
		{
			// if parent yet not exists in mgr
			// try to add it first 
			if (!_AddNamedObjectSimple(sptrNParent, bstrTypeNamed))
				return false;
		}
		// add object after parent if it's exists
		if (!pMgr->Add(pNamedObject))
			return false;
	}
	return true;
}

// add named object to corresponding manager
bool CDataContext::_AddNamedObject(INamedDataObject2 * pNamedObject, BSTR bstrtype, bool active, bool bAutoSelect)
{
	if (!pNamedObject)
		return false;

	_bstr_t	bstrTypeNamed(bstrtype);
	if (!bstrTypeNamed.length())
		bstrTypeNamed = ::GetObjectKind(pNamedObject);

	CString strName = ::GetObjectName(pNamedObject);

	// get corresponding manager
	CObjectManager * pMgr = GetManager(bstrTypeNamed);
	// if yet we have not manager of this type we must create it
	if (!pMgr && !(pMgr = CreateObjectManager(bstrTypeNamed)))
		return false;


	// add object if it's yet not exists in manager 
	if (pMgr->FindObject(pNamedObject))
	{
		INumeredObjectPtr pNO(pNamedObject);
		GUID Key;
		pNO->GenerateNewKey(&Key);
	}
	{
		IUnknownPtr sptrParent;
		pNamedObject->GetParent(&sptrParent);
		INamedDataObject2Ptr sptrNParent = sptrParent;

		// if object has parent try to find parent in manager
		if (sptrNParent != 0 && !pMgr->FindObject(sptrNParent))
		{
			// if parent yet not exists in mgr try to add it first 
			if (!_AddNamedObject(sptrNParent, bstrTypeNamed, true))
				return false;
		}
		if (!pMgr->Add(pNamedObject))
			return false;

		// event about object added
		NotifyContextView(szEventActiveContextChange, pNamedObject, ncAddObject);
	}

	// for named data's context
	if (CheckInterface(GetControllingUnknown(), IID_INamedData))
	{
		// if object is only single in manager
		if ((pMgr->GetCount() == 1))// && bObjectIsBase)
			active = true;

		if (active)
			return _SetActiveObject(pNamedObject, pMgr->GetType(), true, true);
	}

	//paul 5.09.2002 BT 2668.
	if( !pMgr->GetFirstSelectedPos() )
	{
//		BOOL bAutoSelect = ::GetValueInt(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", TRUE);
		if (bAutoSelect)
		{
			IViewPtr ptr_view( GetControllingUnknown() );
			if( ptr_view )
			{
				DWORD dw_type = 0;
				ptr_view->GetMatchType( pMgr->GetType(), &dw_type );
				if( dw_type & mvFull && !(dw_type & mvFullWithBinary ) )
					_SetActiveObject( pNamedObject, pMgr->GetType(), true, true );
			}
		}
	}


	// else context is view
	//!!![AY]
/*	active = NeedActivateObject(pMgr->FindObject(pNamedObject), active);
	if (active)
		// set object to active 
		return _SetActiveObject(pNamedObject, pMgr->GetType(), true, true);		 */

	return true;
}

// this function to decide object can be activated
bool CDataContext::NeedActivateObject(CContextObject * pObject, bool bActive, int nActiveMode)
{
	if (!pObject)
		return false;

	if (bActive)
		return true;

	bool bActiveView = false;
	// check this context is active view
	IDocumentSitePtr sptrD = m_sptrINamedData;
	IViewPtr		 sptrV = GetControllingUnknown();
	if (sptrV == 0)
		return false;

	if (sptrD != 0)
	{
		IUnknownPtr sptrActiveView;
		sptrD->GetActiveView(&sptrActiveView);

		// if so we need
		if (::GetObjectKey(sptrActiveView) == ::GetObjectKey(sptrV))
			// set object to active 
			bActiveView = true;
	}

	// for active view
	if (bActiveView)
	{
		CContextObject * pObjParent = pObject->GetParent();
		// for child object
		if (pObjParent && pObjParent != pObject->GetMgr()->GetCommon())
		{
			// if parent object is visible
			if (ObjectIsVisible(sptrV, pObjParent->GetPtr(false)))
				return true;
		}
		else
		{
			// check  object is base
			BOOL bObjectIsBase = false;
			pObject->GetPtr(false)->IsBaseObject(&bObjectIsBase);

			// if object is can be base object for object's group & can be visible
			if (bObjectIsBase && ManagerIsAvailable(pObject->GetMgr()))
				// set object to active 
				return true;

			// get object's base key
			GuidKey BaseKey;
			pObject->GetPtr(false)->GetBaseKey(&BaseKey);

			bool bBaseObjVisible = false;
			// try to find object w/ given base key within view's visible objects
			TPOS lPos = 0;
			sptrV->GetFirstVisibleObjectPosition(&lPos);
			while (lPos)
			{
				IUnknownPtr sptrUnkO;
				sptrV->GetNextVisibleObject(&sptrUnkO, &lPos);
				INamedDataObject2Ptr sptrCmpObj = sptrUnkO;

				if (sptrCmpObj != 0)
				{
					// check object has same base key as given 
					GuidKey CmpKey;
					if (FAILED(sptrCmpObj->GetBaseKey(&CmpKey)) || CmpKey != BaseKey)
						continue;

					// if object can be base
					BOOL bCanBeBase = FALSE;
					sptrCmpObj->IsBaseObject(&bCanBeBase);
					if (bCanBeBase)
					{
						bBaseObjVisible = true;
						break;
					}
				}
			}// while 
			// if base object is visible -> activate given object
			if (bBaseObjVisible || nActiveMode >= 2)// && ManagerIsAvailable(pObject->GetMgr()))
				return true;
		}
	}
	else // non active view
	{
		// check  object is base
		BOOL bObjectIsBase = false;
		pObject->GetPtr(false)->IsBaseObject(&bObjectIsBase);

		// get object's base key
		GuidKey BaseKey;
		pObject->GetPtr(false)->GetBaseKey(&BaseKey);

		// if object can be visible
		if (ManagerIsAvailable(pObject->GetMgr()))
		{
			// if object can be base object for object's group 
			if (bObjectIsBase)
			{
				bool bBaseObjVisible = false;
				// try to find object w/ given base key within view's visible objects
				TPOS lPos = 0;
				sptrV->GetFirstVisibleObjectPosition(&lPos);
				while (lPos)
				{
					IUnknownPtr sptrUnkO;
					sptrV->GetNextVisibleObject(&sptrUnkO, &lPos);
					INamedDataObject2Ptr sptrCmpObj = sptrUnkO;

					if (sptrCmpObj != 0)
					{
						// check object has same base key as given 
						GuidKey CmpKey;
						if (FAILED(sptrCmpObj->GetBaseKey(&CmpKey)) || CmpKey != BaseKey)
							continue;

						// if object can be base
						BOOL bCanBeBase = FALSE;
						sptrCmpObj->IsBaseObject(&bCanBeBase);
						if (bCanBeBase && !FindObject(sptrCmpObj))
						{
							bBaseObjVisible = true;
							break;
						}
					}
				}// while 
				// if base object is visible -> activate given object
				if (bBaseObjVisible)// && ManagerIsAvailable(pObject->GetMgr()))
					return true;
			} 
			else// object can not be base
			{
				// if base object (or one of objects in group that can be base) is active
				if (m_sptrINamedData == 0)
					return false;

				bool bBaseActive = false;
				
				// [vanek] BT2000: 3364
				long lPos = 0;
				m_sptrINamedData->GetBaseGroupObjectFirstPos(&BaseKey, &lPos);
				while (lPos)
				{
					IUnknownPtr sptrUnkObj;
					m_sptrINamedData->GetBaseGroupNextObject(&BaseKey, &lPos, &sptrUnkObj);
					INamedDataObject2Ptr sptrN = sptrUnkObj;
					if (sptrN == 0)
						continue;

					BOOL bCanBeBase = false;
					sptrN->IsBaseObject(&bCanBeBase);
					if (!bCanBeBase)
						continue;

					CContextObject * pObj = FindObject(sptrN);
					if (!pObj)
						continue;

					if (::ObjectIsActive(pObj))
					{
						bBaseActive = true;
						break;
					}
				}
				if (bBaseActive)
					return true;
	
			//////////
			}
		}
	}
	return false;
}


// remove named object(!NULL) of request type(may be NULL)
bool CDataContext::_RemoveNamedObject(INamedDataObject2 * pNamedObject, BSTR bstrtype, bool bEnableNotify,
	bool bAutoSelect)
{
	// get type
	_bstr_t bstrType(bstrtype);
	if (!pNamedObject)
		return false;

		// if bstrtype == NULL get type from object
	if (!bstrType.length())
		bstrType = ::GetObjectKind(pNamedObject);
	
	// get object manager with corresponding type
	CObjectManager * pMgr = GetManager(bstrType);
	// if obj_manager with this type does not exists or
	if (!pMgr)
		return false;

	// if mgr does not contains this named object it's means the bad parameter occured
	CContextObject * pObj = pMgr->FindObject(pNamedObject);
	if (!pObj)
		return false;

	// check this object previously was active
	bool bActive = ObjectIsActive(pObj);
	bool	bFound = false;

	{
		IViewPtr	ptrView( GetControllingUnknown() );
		if( ptrView != 0 )
		{
			TPOS	lpos = 0;
			ptrView->GetFirstVisibleObjectPosition(&lpos );
			while( lpos )
			{
				IUnknown	*punk = 0;
				ptrView->GetNextVisibleObject( &punk, &lpos );

				if( !punk )continue;
				
				if( GetObjectKey( punk ) == GetObjectKey( pObj->GetPtr(false)) )
					bFound = true;
				punk->Release();
			}
		}
		if( !bFound )bActive = false;
	}

	CContextObject * pParentObj = pObj->GetParent();
	// remove object
	if (!pMgr->Remove(pNamedObject))
		return false;

	// notify context views about object removed
	NotifyContextView(szEventActiveContextChange, pNamedObject, ncRemoveObject);

	if (bEnableNotify)
	{
//		BOOL bAutoSelect = ::GetValueInt(::GetAppUnknown(), "DataContext", "AutoSelectOnEmpty", TRUE);
		if (bActive && bAutoSelect)
		{
			CContextObject * pNextObj = 0;
			// if given object is child object 
			if (pParentObj != pMgr->GetCommon())
			{
				// try to find next child(if any)
				pNextObj = pParentObj->GetChild(pParentObj->GetChildFirstPos());
			}
			// if next object is not found
			if (!pNextObj)
			{
				// if exists next selected object
				POSITION pos = pMgr->GetFirstSelectedPos();
				pNextObj = pos ? pMgr->GetNextSelected(pos) : 0;
			}
			// if previously was only one selected object
			// first  - try to find next object in mgr w/ such base key
			if (!pNextObj)
			{
				// get base key
				GuidKey BaseKey;
				pNamedObject->GetBaseKey(&BaseKey);
	
				// for all objects in mgr
				POSITION pos = pMgr->GetFirstObjectPos();
				while (pos)
				{
					CContextObject * pGroupObj = pMgr->GetNextObject(pos);
					if (!pGroupObj->GetPtr(false))
						continue;

					// get object base key
					GuidKey CmpKey; 
					pGroupObj->GetPtr(false)->GetBaseKey(&CmpKey);

					if (CmpKey == BaseKey)
					{
						pNextObj = pGroupObj;
						break;
					}
				}
			}
			// if we have no next object in group of removed object
			// we'll get last used object from manager
			if (!pNextObj)
			{
				BOOL bCanBeBase = false;
				pNamedObject->IsBaseObject(&bCanBeBase);
				if (bCanBeBase)
					pNextObj = pMgr->GetObject(pMgr->GetFirstObjectPos());
			}

			if (pNextObj)
				return _SetActiveObject(pNextObj->GetPtr(false), pMgr->GetType(), true, true);
		}

		// change view active object if removed object was visible in view
		IViewPtr sptrV = GetControllingUnknown();
		if (sptrV != 0)
		{
			if (ObjectIsVisible(sptrV, pNamedObject))
			{
				NotifyView(szEventActiveObjectChange, 0, ACTIVATE_OBJECT);
				NotifyContextView(szEventActiveContextChange, 0, ncUpdateSelection);
			}
		}
	}
	// if we are here it's means that's all right
	return true;
}

bool ObjectIsVisible(IView * pView, INamedDataObject2 * pObj)
{
	if (!pView || !pObj)
		return false;

	// get object key
	GuidKey ObjKey = ::GetObjectKey(pObj);

	// try to find given object in view's visible objects
	TPOS lPos = 0;
	pView->GetFirstVisibleObjectPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptrO;
		pView->GetNextVisibleObject(&sptrO, &lPos);
		CString sName = GetObjectName(sptrO);
		if (::GetObjectKey(sptrO) == ObjKey)
			return true;;
	}
	return false;
}


// return active object of request type
INamedDataObject2 * CDataContext::_GetActiveObject(BSTR bstrtype)
{
	// get type
	_bstr_t bstrType(bstrtype);

	// if bstrtype == NULL it's means the bad parameter 
	// we can not make nothing
	if (!bstrType.length())
		return NULL;
		
	// get object manager with corresponding type
	CObjectManager * pMgr = GetManager(bstrType);
	// if obj_manager with this type does not exists 
	if (!pMgr)
		return NULL;

	// return active object from this manager
	return pMgr->GetActive(true);

}

// set active object corresponding with its type(may be NULL)
bool CDataContext::_SetActiveObject(INamedDataObject2 * pNamedObject, BSTR bstrtype, bool bEnableNotify, bool bActivateDepended, bool bSkipIfAlreadyActive )
{
	// vanek BT:618
	if( m_bLockObjectActivate )
		return false;
	
	// get type
	_bstr_t bstrType(bstrtype);

	// if bstrtype == NULL get type from object
	if (!bstrType.length())
		bstrType = ::GetObjectKind(pNamedObject);

	// get object manager with corresponding type
	CObjectManager * pMgr = GetManager(bstrType);
	// if obj_manager with this type does not exists 
	if (!pMgr)
		return false;

	// check whether object is already active. If so, skip step.
	if (bSkipIfAlreadyActive)
	{
		IUnknown *punkActiveNow = pMgr->GetActive();
		if (::GetObjectKey(punkActiveNow)==::GetObjectKey(pNamedObject))
			return true;
	}

	// remove all selected
	pMgr->DeselectAll(); // ??????????????????????????????
	
	// if object is invalid
	if (!pNamedObject)
	{
		// activate mgr
		ActivateMgr(pMgr);
		if (bEnableNotify)
		{
			NotifyView(szEventActiveObjectChange, 0, ACTIVATE_OBJECT);
			NotifyContextView(szEventActiveContextChange, 0, ncUpdateSelection);

			_variant_t vart_arg[2] = { pMgr->GetSelectedCount(), bstrType  };
			IScriptSitePtr	sptr_script_site = ::GetAppUnknown();
			sptr_script_site->Invoke(_bstr_t(L"DataContext_" szEventContextSelectionChange), vart_arg, 2, 0, fwFormScript | fwAppScript );
		}
		return true;
	}
	
	// find object
	CContextObject * pObj = pMgr->FindObject(pNamedObject);
	if (!pObj)
		return false;

	bool bRet = true;
	// get object's parent
	IUnknownPtr sptrUnkParent;
	pNamedObject->GetParent(&sptrUnkParent);
	// if object has parent (example : frame on image)
	// if object has parent -> activate parent
	if (sptrUnkParent != 0)
	{
		// first we must activate parent
		if (!_SetActiveObject(INamedDataObject2Ptr(sptrUnkParent), bstrType, false, false, bSkipIfAlreadyActive))
			bRet = false;
	}

	BOOL bCanBeBase = FALSE;
	// check object can be base
	pNamedObject->IsBaseObject(&bCanBeBase);

	// activate objects dependents if object is base
	if( bCanBeBase && bActivateDepended )
	{
		// get base object for given group
		GuidKey BaseKey;
		// get group base key
		pNamedObject->GetBaseKey(&BaseKey);
		// get base object
		IUnknownPtr sptrBase;
		m_sptrINamedData->GetBaseGroupBaseObject(&BaseKey, &sptrBase);
		// check given object is base object
		bool bBaseObject = ::GetObjectKey(sptrBase) == ::GetObjectKey(pNamedObject);

		// activate objects dependents
		// if object has parent we need not unselect all dependent objects are activated by parent
		if (bBaseObject && !ActivateDependents(pObj, BaseKey, true))//sptrUnkParent == 0))
			bRet = false;
	}

	// and set it to active
	bRet = pMgr->ActivateObject(pObj) && bRet;
	// select object
	bRet = pMgr->SelectObject(pObj, true) && bRet;

	// activate mgr
	ActivateMgr(pMgr);

	if (bEnableNotify)
	{
		NotifyView(szEventActiveObjectChange, pNamedObject, ACTIVATE_OBJECT);
		NotifyContextView(szEventActiveContextChange, 0, ncUpdateSelection);
	}

	return bRet;
}

// activate 
bool CDataContext::ActivateDependents(CContextObject * pObj, GuidKey & BaseKey, bool bDeselectPrev)
{
	if (!pObj || BaseKey == INVALID_KEY || m_sptrINamedData == 0)
		return true;

	// need deselect all ? 
	if (bDeselectPrev)
	{
		// for all managers exclude managet w/ given type
		for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
		{
			// get manager
			CObjectManager * pMgr = m_arrObjManagers[i];
			// check it's valid and it's not equal object's type
			if (!pMgr || pMgr == pObj->GetMgr())
				continue;

			// unselect all objects 
			pMgr->DeselectAll();
		}
	}

	// activate dependents
	bool bRet = true;
	GuidKey ObjKey = ::GetObjectKey(pObj->GetPtr(false));

	// for all objects in group
	// get first pos of base group
	long lPos = 0;
	m_sptrINamedData->GetBaseGroupObjectFirstPos(&BaseKey, &lPos);
	while (lPos)
	{
		// get next object from base group
		IUnknownPtr sptrUnkObj;
		m_sptrINamedData->GetBaseGroupNextObject(&BaseKey, &lPos, &sptrUnkObj);

		CString sType = ::GetObjectKind(sptrUnkObj);
		CString sName = ::GetObjectName(sptrUnkObj);
		TRACE("Activating %s(%s)\n", (const char *)sName, (const char *)sType);

		// if next object is not base object
		if (ObjKey != ::GetObjectKey(sptrUnkObj))
		{
			// activate it
			CContextObject * pDep = sptrUnkObj != 0 ? FindObject(INamedDataObject2Ptr(sptrUnkObj)) : 0;
			
			if (pDep)
			{
				ASSERT (::AfxIsValidAddress(pDep, sizeof(CContextObject), true));
				if (::AfxIsValidAddress(pDep, sizeof(CContextObject), true) && 
					pDep->GetMgr() && pDep->GetMgr() != pObj->GetMgr())
				{
					if (!pDep->GetMgr()->ActivateObject(pDep))
						bRet = false;
					// and activate mgr
					ActivateMgr(pDep->GetMgr());
				}
			}
		}
	}
	return bRet;
}

// return CContextObject by interface and type (if it's exists)
CContextObject * CDataContext::FindObject(INamedDataObject2 * pObject, BSTR bstrtype)
{
	if (!pObject)
		return 0;

	_bstr_t bstrType(bstrtype);
	
	// get object type
	if (!bstrType.length())
		bstrType = ::GetObjectKind(pObject);

	// get object manager by type
	CObjectManager * pMgr = GetManager(bstrType);
	if (!pMgr)
		return 0;

	return pMgr->FindObject(pObject);
}

// check named object is exists. For make it we will try to find object by IUnknown and type (may be null)  
// return false if object is not found
int	CDataContext::_ExistObject(INamedDataObject2 * pNamedObject, BSTR bstrtype) 
{
	return FindObject(pNamedObject, bstrtype) != 0;
}

// replace old named object to new
bool CDataContext::_ReplaceNamedObject(INamedDataObject2 * pObjectNew, INamedDataObject2 * pObjectOld)
{
	// check new  & old objects that it's named_object 
	if (!pObjectNew || !pObjectOld)
		return false;

	if (::GetObjectKey(pObjectNew) == ::GetObjectKey(pObjectOld))
		return true;
	// get type from old object (we are sure it is  in container)
	_bstr_t bstrType = ::GetObjectKind(pObjectOld);
	CString strName = ::GetObjectName(pObjectOld);

	if ((char*)bstrType != ::GetObjectKind(pObjectNew))
		return false;

	// check old object that it's active
	CObjectManager * pMgr = GetManager(bstrType);
	bool bOldIsActive = false;
	if (pMgr)
	{
		if (pMgr->GetSelectedCount())
		{
			CContextObject * pObj = pMgr->FindObject(pObjectOld);
			bOldIsActive = ObjectIsActive(pObj);
		}
	}

	if (!_RemoveNamedObject(pObjectOld, bstrType, false))
		return false;

	if (!_AddNamedObject(pObjectNew, bstrType, bOldIsActive))
		return false;

	// if we are here it's means that's all right
	return true;
}

// CSerializableObjectImpl
bool CDataContext::SerializeObject( CStreamEx &ar, SerializeParams *pparams )
{
	bool bRet = true;

	XNotifyLock lock(this);

	if (ar.IsStoring())
	{
		Sort();

		int nVersion = 3;
		ar << nVersion; //version

		// save number of obj managers
		int nCount = m_arrObjManagers.GetSize();
		ar << nCount;

		// for all managers from last to first
		for (int i = nCount - 1; i >= 0; i--)
		{
			ar << (int)1; // sign of begin
			// save manager type
			CString str = m_arrObjManagers[i]->GetType();
			ar << str;

			// and serialize manager itself
			if (!StoreManagerToArchive(m_arrObjManagers[i], ar))
				bRet = false;
			// version == 2
			if (!StoreSelectToArchive(m_arrObjManagers[i], ar))
				bRet = false;
		}
		ar << (int)2;// sign of end
	}
	else
	{
		// clear previous contents of context
		Clear();

		// init context from named data
		if (!_InitContext())
			return false;

		_UnselectAll();
		// create saved state of managers in reverse order
		CPtrList listMgr;
		for (int i = 0; i < m_arrObjManagers.GetSize(); i++)
			listMgr.AddHead(m_arrObjManagers[i]);

		// load image of old context and try to synchronize it
		int nVersion = 2;
		ar >> nVersion; //version

		if (nVersion < 3)
		{
			listMgr.RemoveAll();
			return true;
		}
			
		int nCount = 0;
		ar >> nCount;

		// serialize managers in reverse order (from last to first)
		int nF = 0;
		while (nF != 2)
		{
			ar >> nF;
			if (nF == 1)
			{
				CString strType;
				ar >> strType;

				// get manager w/ type 
				CObjectManager * pMgr = GetManager(_bstr_t(strType));
				if (!pMgr)
					bRet = false;

				// load it with check
				if (!LoadManagerFromArchive(pMgr, ar))
					bRet = false;

				if (pMgr)
					bRet = _UnselectAll(CString(pMgr->GetType()));

				if (!LoadSelectFromArchive(pMgr, ar))
					bRet = false;

				// if manager exists
				if (pMgr)
				{
					// remove it from saved list
					POSITION pos = listMgr.Find(pMgr);
					listMgr.RemoveAt(pos);
				}
				nCount--;
			}
		}
		ASSERT (nCount == 0);
		// all not serialized managers must be last used
		for (POSITION pos = listMgr.GetHeadPosition(); pos != NULL; )
		{
			CObjectManager * pMgr = (CObjectManager*)listMgr.GetNext(pos);
			if (pMgr)
				pMgr->SetLastTimeUsed(CDataContext::s_lCount++);
		}
		listMgr.RemoveAll();

		Sort();
	}

	return bRet;
}

bool CDataContext::LoadSelectFromArchive(CObjectManager * pMgr, CStreamEx & ar)
{
	if (!ar.IsLoading())
		return false;
	
	bool bRet = pMgr != NULL;

	int nCount = 0;
	ar >> nCount;

	int nF = 0;
	while (nF != 2) // for children
	{
		// read sign
		ar >> nF;
		if (nF == 1)
		{
			// load object name
			GuidKey key;
			ar >> key;
	
			CContextObject * pObj = pMgr ? pMgr->FindObject(key) : 0;
			if (pObj)
				pMgr->SelectObject(pObj, false);
			else 
				bRet = false;
			nCount--;
//			if (!nCount)
//				if (!pMgr->ActivateObject(pMgr))
//					bRet = false;


		}
	}// end of while loop for selected objects
	ASSERT(nCount == 0);
	return bRet;
}


bool CDataContext::StoreSelectToArchive(CObjectManager * pMgr, CStreamEx & ar)
{
	if (!ar.IsStoring() || !pMgr)
		return false;

	bool bRet = true;
	// save selected count 
	int nCount = pMgr ? pMgr->GetSelectedCount() : 0;
	ar << nCount;

	// save all children
	for (POSITION pos = pMgr->GetFirstSelectedPos(); pos != NULL; )
	{
		CContextObject * pObj = pMgr->GetNextSelected(pos);
		if (!pObj)
		{
			bRet = false;
			ar << (int)0; // sign of begin && no exists
			continue;
		}
		ar << (int)1; // sign of begin & exists
		GuidKey key = ::GetObjectKey(pObj->GetPtr(false));
		ar << key;
	}
	ar << (int)2; // sign of end

	return bRet;
}

bool CDataContext::LoadManagerFromArchive(CObjectManager * pMgr, CStreamEx & ar)
{
	if (!ar.IsLoading())
		return false;
	
	bool bRet = pMgr != NULL;

	// load manager properties and set it to manager
	long lLastTime = 0;
	ar >> lLastTime;
	if (pMgr)
		pMgr->SetLastTimeUsed(CDataContext::s_lCount++);
	
	// load objects
	int nCount = 0;
	ar >> nCount;

	int nF = 0;
	while (nF != 2) // for children
	{
		// read sign
		ar >> nF;
		if (nF == 1)
		{
			// read object key
			GuidKey ObjKey;
			ar >> ObjKey;

			// try to find object
			CContextObject * pObj = pMgr ? pMgr->FindObject(ObjKey) : 0;
			INamedDataObject2Ptr sptrObj = pObj ? pObj->GetPtr(false) : 0;

			// children count
			int nChildrenCount = 0;
			ar >> nChildrenCount;

			int nCF = 0;
			while (nCF != 5)
			{
				ar >> nCF; // read feature
				if (nCF == 4)
				{
					// read child Key
					GuidKey ChildKey;
					ar >> ChildKey; 
					
					CContextObject * pObjChild = pMgr ? pMgr->FindObject(ChildKey) : 0;
					if (pObj && pObjChild && pObj->IsChild(pObjChild))
					{
						// we need to activate child and exclude child from childList
						pObj->ActivateChild(pObjChild);
					}
					// decrement children count
					nChildrenCount--;
				}
			}
			ASSERT (nChildrenCount == 0);
			nCount--; // decrement object count
			// activate object in mgr
			if (pObj && pMgr)
				pMgr->ActivateObject(pObj);
		}
	}// end of while loop for children 
	ASSERT(nCount == 0);
	return bRet;
}

bool CDataContext::StoreManagerToArchive(CObjectManager * pMgr, CStreamEx & ar)
{
	if (!ar.IsStoring())
		return false;

	bool bRet = true;

	// save manager properties
	ar << pMgr->GetLastTimeUsed();
	
	// save manager objects
	int nCount = pMgr->GetCount();
	ar << nCount;

	// save all children
	for (POSITION pos = pMgr->GetLastObjectPos(); pos != NULL; )
	{
		CContextObject * pObj = pMgr->GetPrevObject(pos);
		if (!pObj)
		{
			bRet = false;
			ar << (int)0; // sign of begin && no exists
			continue;
		}
		ar << (int)1; // sign of begin & exists
		GuidKey ObjKey = ::GetObjectKey(pObj->GetPtr(false));
		// save object key
		ar << ObjKey;

		// save children count
		int nChildrenCount = pObj->GetChildrenCount();
		ar << nChildrenCount;

		// save children in reverse order
		for (POSITION ChildPos = pObj->GetChildLastPos(); ChildPos != NULL; )
		{
			CContextObject * pChild = pObj->GetPrevChild(ChildPos);
			if (!pChild)
			{
				ar << (int)3; // sign child is not exists
				bRet = false;
			}
			else 
			{
				ar << (int)4; // sign child is exists
				GuidKey ChildKey = ::GetObjectKey(pChild->GetPtr(false));
				ar << ChildKey;
			}
		}
		ar << (int)5; // sign end of children
	}
	ar << (int)2; // sign of end

	return bRet;
}

// store to text
bool CDataContext::StoreToText(CStringArray & sa)
{
	bool bRet = true;

	XNotifyLock lock(this);

	Sort();
	// for all mamangers in reverse order
	for (int i = m_arrObjManagers.GetSize() - 1; i >= 0; i--)
	{
		CObjectManager * pMgr = m_arrObjManagers[i];
		if (!pMgr)
			continue;

		if (!::ObjectManagerToArray(pMgr, sa))
			bRet = false;
	}
	return bRet;

}

// load from text
bool CDataContext::LoadFromText(CStringArray & sa)
{
	bool bRet = true;

	XNotifyLock lock(this);

	if (!sa.GetSize())
		return false;

	// initialize context
	Clear();

	if (!_InitContext())
		return false;

	// find first manager_string in array
	int nFirst = 0;
	for (int i = 0; i < sa.GetSize(); i++)
	{
		if (sa[i].IsEmpty())
			continue;

		if (::IsTypeString(sa[i]))
		{
			if (!LoadManagerFromText(i, sa))
				bRet = false;
		}
	}
	// sort
	Sort();

	return bRet;
}

bool CDataContext::LoadManagerFromText(int & nPos, CStringArray & sa)
{
	// we need interface to named data
	if (m_sptrINamedData == 0)
		return false;

	bool bRet = true;

	CString strType;
	long lTime = -1;
	long lObjCount = 0;
	long lSelCount = 0;
	CString strManager = sa[nPos++]; // get manager string

	if (!GetMgrFromMgrString(strType, lTime, lObjCount, lSelCount, strManager))
		return false;

	CStringArray arrObg;

	// fill object's string array
	for (nPos; nPos < sa.GetSize(); nPos++)
	{
		if (sa[nPos].IsEmpty())
			continue;

		// if found next manager it's means we already have all string form our manager
		if (::IsTypeString(sa[nPos]))
		{
			// save pos and break
			nPos--;
			break;
		}
		// add next object string to array
		arrObg.Add(sa[nPos]);
	}

	// restore manager previous state from saved array
	CObjectManager * pMgr = GetManager(_bstr_t(strType));
	if (!pMgr && !(pMgr = CreateObjectManager(_bstr_t(strType))))
		return false;

	//set last used time
	pMgr->SetLastTimeUsed(lTime < 0 ? CDataContext::s_lCount++ : lTime);
	lObjCount--;

	int nArrPos = 0;
	while (nArrPos < arrObg.GetSize() && lObjCount >= 0)
	{
		CString strObj = arrObg[nArrPos++];
		if (strObj.IsEmpty())
			continue;
		
		GuidKey ObjKey;
		GuidKey ParentKey;
		long lChildrenCount = 0;
		// get object's props
		if (!GetObjectFromString(ObjKey, ParentKey, lChildrenCount, strObj, lObjCount--))
			bRet = false;

		// find object
		CContextObject * pObj = pMgr->FindObject(ObjKey);
		if (!pObj)
			bRet = false;

		// check object valid
		if (pObj && pObj->GetParent())
		{
			GuidKey cmpKey = GetObjectKey(pObj->GetParent()->GetPtr(false));
			ASSERT (cmpKey == ParentKey);
			if (cmpKey != ParentKey)
				bRet = false;
		}

		// get object's children
		for (int nCI = lChildrenCount - 1; nCI >= 0; nCI--)
		{
			CString strChild = arrObg[nArrPos++];

			// get child props
			GuidKey ChildKey;
			long lCount = -1;
			if (!GetChildFromString(ChildKey, lCount, strChild))
			{
				bRet = false;
				continue;
			}
			ASSERT(lCount == nCI);

			// find child in object's children list
			CContextObject * pChild = pMgr->FindObject(ChildKey);
			if (!pChild || !pObj && !pObj->IsChild(pChild))
			{
				bRet = false;
				continue;
			}
			// activate child
			if (pObj && pChild)
				pObj->ActivateChild(pChild);

		} // for children

		// activate object in mgr
		if (pObj)
			pMgr->ActivateObject(pObj);

	} // while loop for objects

	pMgr->DeselectAll();
	// select needed objects
	lSelCount--;
	long lSelIndex = -1;
	while (nArrPos < arrObg.GetSize() && lSelCount >= 0)
	{
		lSelIndex++;
		CString strSel = arrObg[nArrPos++];

		GuidKey SelKey;
		long lCount = -1;
		if (!GetChildFromString(SelKey, lCount, strSel))
		{
			bRet = false;
			continue;
		}
		ASSERT(lCount == lSelIndex);
		CContextObject * pSel = pMgr->FindObject(SelKey);
		if (pSel)
			pMgr->SelectObject(pSel);

	}

	return bRet;
}

// vanek BT:618
// IDataContextMisc Implementation
HRESULT CDataContext::XMisc::SetLockObjectActivate( BOOL bActivate )
{
	METHOD_PROLOGUE_EX(CDataContext, Misc)
	{
		pThis->m_bLockObjectActivate = bActivate;
		return S_OK;
	}
}

HRESULT CDataContext::XMisc::GetLockObjectActivate( BOOL *pbActivate )
{
    METHOD_PROLOGUE_EX(CDataContext, Misc)
	{
		if( !pbActivate )
			return S_FALSE;

		*pbActivate = pThis->m_bLockObjectActivate;
		return S_OK;
	}
}


/*
bool IsTypeString(CString str);
GuidKey GuidFromString(LPCTSTR szString);
bool ContextObjectToArray(CContextObject * pObj, int nObjCount, CStringArray & sa);
bool ObjectManagerToArray(CObjectManager * pMgr, CStringArray & sa);
bool GetMgrFromMgrString(CString & strType, long & lTime, long & lObjCount, long & lSelCount, CString str);
bool GetObjectFromString(GuidKey & ObjKey, GuidKey & ParentKey, long & lChildrenCount, CString str, int nCount);
bool GetChildFromString(GuidKey & ObjKey, long & lCount, CString str);
*/
//helpers
//string_array from string

void ArrayFromString(CStringArray & sa, CString & strSource)
{
	sa.RemoveAll();
	if (strSource.IsEmpty())
		return;

	// add last '\n'
	if (strSource[strSource.GetLength() - 1] != _T('\n'))
		strSource += _T('\n');

	int nFirst = 0;
	int nLast = strSource.Find(_T('\n'));

	while (nLast != -1)
	{
		CString str = strSource.Mid(nFirst, nLast - nFirst);
		sa.Add(str);
		nFirst = nLast + 1;
		nLast = strSource.Find(_T('\n'), nFirst);
	}
}

//string_array to string
void ArrayToString(CString & strTarget, CStringArray & sa)
{
	strTarget.Empty();
	if (sa.GetSize() == 0)
		return;

	for (int i = 0; i < sa.GetSize(); i++)
		strTarget += sa[i] + _T('\n');
}

void MemFileToText(CMemFile & file, CStringArray & sa)
{
	CString	strLine;
	char szLongString[255];
	int	iChPos = 0;

	UINT	uFileLen = (UINT)file.GetLength();
	int		nLength = (int)file.GetLength();
	BYTE *	ptr = file.Detach();

	for (int i = 0; i < nLength; i++)
	{
		iChPos += ::sprintf(szLongString + iChPos, "%02X ", ptr[i]);

		if (iChPos > 90 || i == (nLength - 1))
		{
			szLongString[iChPos] = 0;
			sa.Add(szLongString);
			iChPos = 0;
		}
	}

	file.Attach(ptr, uFileLen);
}

void TextToMemFile(CMemFile & file, CStringArray & sa)
{
	// calc needed buffer size
	int nLength = 0;
	for (int i = 0; i < sa.GetSize(); i++ )
		nLength += sa[i].GetLength() / 3;

	BYTE * ptr = (BYTE*)::malloc(nLength);
	if (!ptr)
		return;

	long nCount = 0;
	for (i = 0; i < sa.GetSize(); i++ )
	{
		const char * sz = sa[i];
		int	nLineLen = ::strlen(sz);

		for (int j = 0; j < nLineLen; j += 3)
		{
			int	nVal;
			::sscanf(sz + j, "%x", &nVal);
			ptr[nCount++] = (byte)nVal;
		}
	}

	file.Attach(ptr, nLength);
}
