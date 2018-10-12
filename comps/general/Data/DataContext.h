#if !defined(AFX_DATACONTEXT_H__B438FADC_38E7_11D3_87C5_0000B493FF1B__INCLUDED_)
#define AFX_DATACONTEXT_H__B438FADC_38E7_11D3_87C5_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataContext.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDataContext command target
#include <afxTempl.h>
#include "ObjectMgr.h"

typedef CArray <CObjectManager*, CObjectManager*> CNDOManagers;

class CDataContext : public CCmdTargetEx,
					 public CSerializableObjectImpl
{
protected:
	DECLARE_DYNCREATE(CDataContext)
	GUARD_DECLARE_OLECREATE(CDataContext)
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	ENABLE_MULTYINTERFACE()

	CDataContext();           // protected constructor used by dynamic creation
	virtual ~CDataContext();

// Attributes
protected:
	sptrINamedData	m_sptrINamedData;	// ptr on unknown on NamedData
	CNDOManagers	m_arrObjManagers;   // array of object managers


// Operations
public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataContext)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// attach context to NamedData
	bool AttachData(INamedData * pNamedData);

// managers
	// fill array of managers
	bool			 CreateManagers(IDataTypeManager* pDataTypeManager);
	// create manager by type
	CObjectManager * CreateObjectManager(BSTR bstrtype);
	// get manager by type
	CObjectManager * GetManager(BSTR bstrtype);
	// find manager pos in manager array
	// obsolete
//	int				 FindManager(BSTR bstrType);


	// sort managers by used time
	bool Sort();
	// activate manager
	bool ActivateMgr(CObjectManager* pMgr);

	// load available types from aggregate view
//	bool FillTypesPermit();
	// check manager contains objects that can shows by view
	// type is enable
	bool ManagerIsAvailable(CObjectManager * pMgr);


	// filters 
	bool EnterFilter(IUnknown * punkFilter);
	bool PassFilter(IUnknown * punkFilter);
	bool CancelFilter(IUnknown * punkFilter);
	bool RedoFilter(IUnknown * punkFilter);
	bool UndoFilter(IUnknown * punkFilter);


// utility
	// initialize context contens	
	bool InitializeContext();
	// init context from Named data w/o a any events
	bool _InitContext(); 
	
	// walk throught NamedData and fill all managers from it
	// obsolete 
	bool Walk(LPCTSTR path);  
	// Rescan and  fill context from Named Data (after serialization, for example)
	bool Rescan();
	// clean up array of Managers
	void Clear();

	// notify aggragated view
	void NotifyView(LPCTSTR szEventDesc, IUnknown * punkObject, long lCode);
	// notify context view
	void NotifyContextView(LPCTSTR szEventDesc, IUnknown * punkObject, NotifyCodes ncCode);
	void NotifyContextView2(LPCTSTR szEventDesc, IUnknown * punkObject, LPARAM lParam);


// objects
	// add named object to corresponding manager
	bool _AddNamedObjectSimple(INamedDataObject2 * pNamedObject, BSTR bstrtype = NULL);
	// add named object to corresponding manager
	bool _AddNamedObject(INamedDataObject2 * pNamedObject, BSTR bstrtype = NULL, bool active = false, bool bAutoSelect = true);
	// remove named object to corresponding manager
	bool _RemoveNamedObject(INamedDataObject2 * pNamedObject, BSTR bstrtype = NULL, bool bEnableNotify = true, bool bAutoSelect = true);
	// replace old named object to new
	bool _ReplaceNamedObject(INamedDataObject2 * pObjectNew, INamedDataObject2 * pObjectOld);
	// return active object of request type
	INamedDataObject2 *	_GetActiveObject(BSTR bstrtype);
	// set active object corresponding with type
	bool _SetActiveObject(INamedDataObject2 * pNamedObject, BSTR bstrtype, bool bEnableNotify, bool bActivateDepended, bool bSkipIfAlreadyActive = false );
	// Find object by INamedDataObject2 and type (may be null) 
	int	 _ExistObject(INamedDataObject2 * pNamedObject, BSTR bstrtype = NULL); // return -1 if not found
	// select object
	bool _SelectObject(INamedDataObject2 * punkObject, BSTR bstrObjType = NULL);
	// unselect object
	bool _DeselectObject(INamedDataObject2 * punkObject, BSTR bstrObjType = NULL);
	// unselect all objects of given type or all types
	bool _UnselectAll(CString & str);
	// unselect all objects of given type or all types
	bool _UnselectAll();

	// A.M. BT 4652. There is a problem when image and it parent in arguments list
	// of single filter. This function creates sorted array of filter aruments when
	// parent objects are inserted before its children.
	void _MakeFilterArgumentsArray(IFilterAction2Ptr sptrF, CArray<TPOS, TPOS&> &arrArgs);


	bool NeedActivateObject(CContextObject * pObject, bool bActive = false, int nActiveMode = 1);

	// find context object by it's interface
	CContextObject * FindObject(INamedDataObject2 * pObject, BSTR bstrtype = 0);

	// get last used object one of given types
	IUnknown* GetLastActiveObject(BSTR bstrTypes, bool bAddRef = true);

// syncronize
	// syncronize this context w/ given
	bool SynchronizeWithContext(IUnknown * punkContext);

// base - dependent realtions
	// activate dependents
	bool ActivateDependents(CContextObject * pObj, GuidKey & BaseKey, bool bDeselectPrev = true);
	// change base-dependent relations for given object
	bool ChangeBaseObject(INamedDataObject2 * punkObj);
	// change key for for given object
	bool ChangeObjectKey(INamedDataObject2 * punkObj, GuidKey & rPrevKey);


// save/load/serialization
	// store context to string array
	bool StoreToText(CStringArray & sa);
	// load context from string array
	bool LoadFromText(CStringArray & sa);
	// load manager from string array
	bool LoadManagerFromText(int & nPos, CStringArray & sa);

	// function name say itself about what it's do
	bool LoadManagerFromArchive(CObjectManager * pMgr, CStreamEx & ar);
	bool StoreManagerToArchive(CObjectManager * pMgr, CStreamEx & ar);
	//
	bool LoadSelectFromArchive(CObjectManager * pMgr, CStreamEx & ar);
	bool StoreSelectToArchive(CObjectManager * pMgr, CStreamEx & ar);

// override virtual function
	// CSerializableObjectImpl
	virtual bool SerializeObject( CStreamEx &ar, SerializeParams *pparams );


	// Generated message map functions
	//{{AFX_MSG(CDataContext)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDataContext)
	afx_msg LPDISPATCH GetActiveObject(LPCTSTR szType);
	afx_msg BOOL SetActiveObject(LPDISPATCH lpdispObj);
	afx_msg long GetObjectCount(LPCTSTR szType);
	afx_msg LONG_PTR GetFirstObjectPos(LPCTSTR szType);
	afx_msg LPDISPATCH GetNextObject(LPCTSTR szType, LONG_PTR * plPos);
	afx_msg long GetLastObjectPos(LPCTSTR szType);
	afx_msg LPDISPATCH GetPrevObject(LPCTSTR szType, LONG_PTR * plPos);
	//}}AFX_DISPATCH

	BEGIN_INTERFACE_PART(Context, IDataContext3)
// IDataContext
		com_call GetActiveObject(BSTR bstrType, IUnknown ** ppunkObj);
		com_call SetActiveObject(BSTR bstrType, IUnknown * punkObj, DWORD dwFlags );
		com_call GetData(IUnknown ** ppunk);
		com_call AttachData(IUnknown * punkNamedData);
		com_call Notify(long cod, IUnknown * punkNew, IUnknown * punkOld, GUID* dwData);
	// lock Update	
		com_call LockUpdate(BOOL bLock, BOOL bUpdate);
		com_call GetLockUpdate(BOOL * pbLock);

// IDataContext2
	// object types
		com_call GetObjectTypeCount(long * plCount);
		com_call GetObjectTypeName(LONG_PTR lPos, BSTR * pbstrType);

	// type enables
		com_call GetTypeEnable(BSTR bstrType, BOOL * pbEnable);
	// types enable
		com_call GetTypesEnable(BSTR * pbstrTypes);

	// objects 
		com_call GetObjectCount(BSTR bstrType, long * pnCount);
		com_call GetFirstObjectPos(BSTR bstrType, LONG_PTR * plPos);
		com_call GetNextObject(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject);
		com_call GetLastObjectPos(BSTR bstrType, LONG_PTR * plPos);
		com_call GetPrevObject(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject);

		com_call GetObjectPos(BSTR bstrType, IUnknown * punkObject, LONG_PTR * plPos);
		com_call GetObjectEnable(IUnknown * punkObject, BOOL * pbFlag);

	// child objects
		com_call GetChildrenCount(BSTR bstrType, IUnknown * punkParent, long * pnCount);
		com_call GetFirstChildPos(BSTR bstrType, IUnknown * punkParent, long * plPos);
		com_call GetNextChild(BSTR bstrType, IUnknown * punkParent, long * plPos, IUnknown ** ppunkChild);
		com_call GetLastChildPos(BSTR bstrType, IUnknown * punkParent, long * plPos);
		com_call GetPrevChild(BSTR bstrType, IUnknown * punkParent, long * plPos, IUnknown ** ppunkChild);
		com_call GetChildPos(BSTR bstrType, IUnknown * punkParent, IUnknown * punkChild, long * plPos);

	// selected objects
		com_call GetObjectSelect(IUnknown * punkObject, BOOL * pbFlag);
		com_call SetObjectSelect(IUnknown * punkObject, BOOL bFlag);

		com_call GetSelectedCount(BSTR bstrType, long * plCount);
		com_call GetFirstSelectedPos(BSTR bstrType, LONG_PTR * plPos);
		com_call GetNextSelected(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject);
		com_call GetLastSelectedPos(BSTR bstrType, LONG_PTR * plPos);
		com_call GetPrevSelected(BSTR bstrType, LONG_PTR * plPos, IUnknown ** ppunkObject);
		com_call GetSelectedObjectNumber(BSTR bstrType, IUnknown * punkObject, long * plNumber);

		com_call UnselectAll(BSTR bstrType);

	// last active object
		com_call GetLastActiveObject(BSTR bstrTypes, IUnknown ** ppunkObject);

		// synchronize
		com_call SynchronizeWithContext(IUnknown * punkContext);

		// serialize
		com_call StoreToString(BSTR * pbstrString);
		com_call LoadFromString(BSTR bstrString);

		com_call StoreToArchive(IUnknown * punkOleStream);
		com_call LoadFromArchive(IUnknown * punkOleStream);

	END_INTERFACE_PART(Context);

	BEGIN_INTERFACE_PART(Sync, IDataContextSync)
		com_call GetObjManagerCount(int * pnCount);
		com_call GetObjManager(int nPos, BSTR * pbstrType, long * plTime);
		
		com_call GetObjManagerTime(BSTR bstrType, long * plTime);
		com_call SetObjManagerTime(BSTR bstrType, long lTime);

		com_call GetObjManagerTime(int nPos, long * plTime);
		com_call SetObjManagerTime(int nPos, long lTime);

		com_call GetDataKey(GUID * pDataKey);
	END_INTERFACE_PART(Sync);

	// vanek BT:618
	BEGIN_INTERFACE_PART(Misc, IDataContextMisc)
		com_call	SetLockObjectActivate( BOOL bActivate );
		com_call	GetLockObjectActivate( BOOL *pbActivate );
	END_INTERFACE_PART(Misc);

protected:
	BOOL	m_bLockObjectActivate;
    
public:
	// nested class for lock notifications events
	class XNotifyLock
	{
	protected:
		bool			m_bOldState;
		CDataContext *	m_pData;
	public:
		XNotifyLock(CDataContext * pData)
		{
			m_pData = pData;
			m_bOldState = m_pData ? m_pData->EnableContextNotification(false) : true;
		}
		~XNotifyLock()	{	if (m_pData) m_pData->EnableContextNotification(m_bOldState);	}
		void Unlock()	{	if (m_pData) m_pData->EnableContextNotification(true);	}
		void Lock()		{	if (m_pData) m_pData->EnableContextNotification(false);	}
	};
	//enable/disable context notifications. return old state
	bool EnableContextNotification(bool bEnable = true)
	{	
		bool bOld = m_bContextNotificationEnabled;
		m_bContextNotificationEnabled = bEnable;
		return bOld;
	}

	bool		m_bContextNotificationEnabled;
	bool		m_bInsideFilter;

public:
	static long	s_lCount;

};

bool ObjectIsVisible(IView * pView, INamedDataObject2 * pObj);

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATACONTEXT_H__B438FADC_38E7_11D3_87C5_0000B493FF1B__INCLUDED_)
