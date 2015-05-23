#if !defined(AFX_CONTEXTWRP_H__D794BA47_3200_4AC2_B315_647248C0B21B__INCLUDED_)
#define AFX_CONTEXTWRP_H__D794BA47_3200_4AC2_B315_647248C0B21B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContextWrp.h : header file
//

#include "Context\ContextTree.h"
#include "data5.h"
#include "docview5.h"
#include "thumbnail.h"

#define IDC_ICON_SCRIPT			0
#define IDC_ICON_CLASS_LIST		1
#define IDC_ICON_OBJECT_LIST	2
#define IDC_ICON_AX_FORM		3
#define IDC_ICON_REPORT_FORM	4
#define IDC_ICON_DRAWING_OBJECT	5

#include <ComDef.h>

CString GetProviderUserName( IUnknown* punkObj, bool bReturnObjNameIfFailed );
long	GetTypeOrder(LPCTSTR lpstrType);

class CContextItemData
{
	CString		m_strName;
	CString		m_strUserName;	// name of object 
	IUnknownPtr	m_sptr;		// pointer to object interface
	GuidKey		m_BaseKey;
	long		m_lSortNum;
	bool 		m_bEnable;
	long		m_lTypeOrder;

public:
	CContextItemData(IUnknown * punk) // for object 
	{	
		m_BaseKey = INVALID_KEY;
		m_sptr = punk;	
		m_strName = ::GetObjectName(m_sptr);
		m_lSortNum = -1; 
		m_bEnable = false;

		INumeredObjectPtr sptrN = m_sptr;
		if (sptrN)
			sptrN->GetCreateNum(&m_lSortNum);

		m_strUserName = ::GetProviderUserName( m_sptr, true );
		m_lTypeOrder = ::GetTypeOrder(GetObjectKind(m_sptr));
	}
	
	CContextItemData(LPCTSTR szType, bool bEnable) // for type
	{	
		m_strName = szType;
		m_strUserName = ::GetTypeAliase( szType );
		m_bEnable = bEnable;

		m_BaseKey = INVALID_KEY;
		m_sptr = 0;	
		m_lSortNum = -1; 
		m_lTypeOrder = ::GetTypeOrder(szType);
	}

	CContextItemData(LPCTSTR szName, GuidKey key, long lSortNum, LPCTSTR szType) // for base 
	{	
		m_strName = szName;
		m_strUserName = szName;
		m_BaseKey = key;
		m_lSortNum = lSortNum; 

		m_bEnable = false;
		m_sptr = 0;	
		m_lTypeOrder = ::GetTypeOrder(szType);
	}
	
	// constructor of copy by ref
	CContextItemData(CContextItemData & data)
	{	
		m_strName = data.m_strName;	
		m_strUserName = data.m_strUserName;	
		m_sptr = data.m_sptr; 
		m_BaseKey = data.m_BaseKey; 	
		m_lSortNum = data.m_lSortNum;
		m_bEnable = data.m_bEnable;
	}
	// constructor of copy by ptr
	CContextItemData(CContextItemData * data)
	{	
		m_strName = data->m_strName; 
		m_strUserName = data->m_strUserName;	
		m_sptr = data->m_sptr; 
		m_BaseKey = data->m_BaseKey;	
		m_lSortNum = data->m_lSortNum;
		m_bEnable = data->m_bEnable;
	}

	// assign operator 
	CContextItemData & operator = (const CContextItemData & data)
	{	
		m_strName = data.m_strName;	
		m_strUserName = data.m_strUserName;	
		m_sptr = data.m_sptr; 
		m_BaseKey = data.m_BaseKey;	
		m_lSortNum = data.m_lSortNum;
		m_bEnable = data.m_bEnable;
	}

// name
	void	SetName(LPCTSTR szName)	{	m_strName = szName;	}
	LPCTSTR GetName()				{	return m_strName;	}

// name
	void	SetDisplayName(LPCTSTR szName)	{	m_strUserName = szName;	}
	LPCTSTR GetDisplayName()				{	return m_strUserName;	}

// sort number
	void	SetSortNum(long lNum)	{	m_lSortNum = lNum;	}
	long	GetSortNum()			{	return m_lSortNum;	}

// base key
	void	SetKey(GuidKey	Key)	{	m_BaseKey = Key;	}
	GuidKey	GetKey()				{	return m_BaseKey;	}

// enable 
	void	SetEnable(bool bFlag)	{	m_bEnable = bFlag;	}
	bool	GetEnable()				{	return m_bEnable;	}

	long	GetTypeOrder()			{	return m_lTypeOrder;}

// interface pointer
	void		SetPointer(IUnknown * punk)	
	{	
		m_sptr = punk;	
		INumeredObjectPtr sptrN = m_sptr;
		if (sptrN)
			sptrN->GetCreateNum(&m_lSortNum);
		m_strName = ::GetObjectName(m_sptr);

		if( CheckInterface( m_sptr, IID_INamedDataInfo ) )
			m_strUserName = ::GetTypeAliase( m_strName );
		else
			m_strUserName = m_strName;
	}
	IUnknown *	GetPointer(bool bAddRef = true)
	{	
		if (m_sptr == 0)	
			return 0;
		if (bAddRef)		
			m_sptr.AddRef(); 
		return m_sptr.GetInterfacePtr();	
	}

};



/////////////////////////////////////////////////////////////////////////////
// CContextWrpType window

#include <AfxTempl.h>

typedef CMap <GUID, GUID&, HCONTEXTITEM, HCONTEXTITEM> CObjectMap;

class CContextWrpType : public CWnd
{
	DECLARE_DYNCREATE(CContextWrpType)
// Construction

public:
	CContextWrpType();
	virtual ~CContextWrpType();

// Attributes
public:
	CContextCtrl	m_Tree;
	CMapStringToPtr m_mapTypes;
	CObjectMap		m_mapObjects;

	IThumbnailManagerPtr m_sptrThumbMan;

// Operations
public:
	// clear content of control
	virtual bool ClearContens();

	// zero level //////////////////////////////////////
	// add Type - Zero level of tree
	HCONTEXTITEM AddType(LPCTSTR szType, bool bEnable = true);
	// fill all types
	virtual bool AddAllTypes(bool bSort = true);
	// remove type with its children
	virtual bool RemoveType(LPCTSTR szType);
	// remove all types with its children (it's means remove all items from tree)
	virtual bool RemoveAllTypes();
	// sort types
	virtual bool SortTypes();
	// check item is type
	virtual bool ItemIsType(HCONTEXTITEM hItem);

	// first and all next levels //////////////////////////////////////
	// add all object of given type
	virtual bool AddAllObjects(HCONTEXTITEM nParent, BSTR bstrType, bool bEnable = true, bool bSort = false);
	// add object
	virtual bool AddObject(HCONTEXTITEM hParent, IUnknown * punkObject, bool bEnable = true);
	virtual bool AddObject(IUnknown * punk, bool bSort);

	// activate (or deactivate) object
	virtual bool ActivateObject(IUnknown* punkObject, bool bActive);
	// select (or deselect) object
	virtual bool SelectObject(IUnknown* punkObject, bool bSelect);
	// remove object
	virtual bool RemoveObject(IUnknown* punkObject);
	// sort children for given parent (for objects)
	virtual bool SortObjects(HCONTEXTITEM hTypeItem);
	virtual bool SortObjectsRecursively(HCONTEXTITEM hItem);
	
	// change object's key
	virtual bool ChangeObjectKey(IUnknown* punkObject, GuidKey & PrevKey);


//	virtual void UpdateEnables();
//	virtual void UpdateEnablesChildren(HCONTEXTITEM hParent, bool bEnable);

	virtual bool DeselectAllObject();
	virtual bool DeselectAllObject(LPCTSTR szType);
	virtual bool DeactivateAllObject();

	virtual void UpdateSelections();
	virtual void UpdateSelections(LPCTSTR szType);
	virtual void UpdateActive();

	virtual void SetNumeric();
	virtual void RemoveNumeric();

	IUnknown * GetObject(HCONTEXTITEM hItem, bool bAddRef = false);
	
	bool AttachData(IUnknown* punkView, IUnknown* punkData);
	virtual void Init(IUnknown* punkView = 0, IUnknown* punkData = 0);

	virtual void SetSortByTime(bool bTime = true);

    BOOL SetRedraw(BOOL bAllowDraw, BOOL bResetScrollBars = FALSE);
	BOOL SetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, BOOL bRedraw = TRUE);
	BOOL GetScrollInfo(int nBar, LPSCROLLINFO lpScrollInfo, UINT nMask = SIF_ALL);

	BOOL SaveScrollInfo(LPSCROLLINFO lpsf);
	BOOL RestoreScrollInfo(LPSCROLLINFO lpsf, bool bRedraw = false);

	CContextCtrl* GetTree()		{	return &m_Tree;	}
	IUnknown * GetContext(bool bAddRef = true);
	IUnknown * GetNamedData(bool bAddRef = true);

	virtual bool		SetFocusObject(IUnknown * punk);
	virtual IUnknown *	GetFocusObject(bool bAddRef = true);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextWrpType)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// find object's context item by object's IUnknown
	HCONTEXTITEM FindObjectItem(IUnknown* punkObject);
	// return context item of first level(type) by given type
	HCONTEXTITEM GetTypeItem(LPCTSTR szType);

	bool DeNumberChildObject(HCONTEXTITEM hItem, bool bEnable = true);

	IUnknown * GetActiveContext(bool bAddRef = true);
	IUnknown * GetActiveView(bool bAddRef = true);
	IUnknown * GetActiveDocument(bool bAddRef = true);
protected:
	sptrIView				m_sptrView;    // current active view
	sptrIDataContext2		m_sptrContext; // view's context
	sptrIDataTypeManager	m_sptrTypeMgr; // NamedData's Interface of current view
/*
	long					m_lLastViewKey;
	long					m_lLastDocKey;
*/
	CImageList				m_ImageList;
	bool m_bEnableUpdateSelections;
	bool m_bSortByTime;


	// Generated message map functions
protected:
	//{{AFX_MSG(CContextWrpType)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnActivateItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNumberItem(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg void OnSysColorChange();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	static int CALLBACK CompareTypeFunc(HCONTEXTITEM hItem1, HCONTEXTITEM hItem2, LPARAM lParamSort);
	static int CALLBACK CompareObjectFunc(HCONTEXTITEM hItem1, HCONTEXTITEM hItem2, LPARAM lParamSort);

};


/////////////////////////////////////////////////////////////////////////////
// CContextWrpObject window

class CContextWrpObject : public CContextWrpType
{
	DECLARE_DYNCREATE(CContextWrpObject)
// Construction
public:
	CContextWrpObject();
	virtual ~CContextWrpObject();

// Attributes
public:
	CObjectMap		m_mapBases;

// Operations
public:
	// clear content of control
	virtual bool ClearContens();

	// zero level //////////////////////////////////////
	// add Type - Zero level of tree
	virtual HCONTEXTITEM AddBaseObject(GuidKey BaseKey, bool bSort = true);
	// remove type with its children
	virtual bool RemoveBaseObject(GuidKey Key);
	// remove all types with its children (it's means remove all items from tree)
	virtual bool RemoveAllBaseObjects();
	// check item is type
	virtual bool ItemIsBase(HCONTEXTITEM hItem);
	// sort types for root parent
	virtual bool SortBases();

	// first and all next levels //////////////////////////////////////
	// add object (and activate)
	virtual bool AddObject(HCONTEXTITEM hParent, IUnknown * punkObject, bool bSort = false);
	// add object
	virtual bool AddObject(IUnknown * punkObject, bool bSort);
	// add all object of given type
	virtual bool AddAllObjects();
	// remove object
	virtual bool RemoveObject(IUnknown * punkObject);
	// change object's base key
	virtual bool ChangeBaseObject(IUnknown * punkObject);
//	// change object's key
//	virtual bool ChangeObjectKey(IUnknown* punkObject, GuidKey & PrevKey);

	virtual bool DeselectAllObject();
	virtual void UpdateSelections();
	
	virtual void SetNumeric();
	virtual void RemoveNumeric();


	virtual void Init(IUnknown* punkView = 0, IUnknown* punkData = 0);

	virtual void SetSortByTime(bool bTime = true);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextWrpObject)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	// return context item of first level(type) by given type
	HCONTEXTITEM GetBaseItem(GuidKey Key);
	bool DeSelectChildObject(IDataContext2 * pContext, HCONTEXTITEM hItem);

	//{{AFX_MSG(CContextWrpObject)
	afx_msg void OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////



//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTEXTWRP_H__D794BA47_3200_4AC2_B315_647248C0B21B__INCLUDED_)
