#if !defined(AFX_DATATREECTRL_H__76B75363_520E_11D3_880E_0000B493FF1B__INCLUDED_)
#define AFX_DATATREECTRL_H__76B75363_520E_11D3_880E_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataTreeCtrl.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataTreeCtrl window

class CDataTreeCtrl : public CTreeCtrl
{
// Construction
public:
	CDataTreeCtrl();
	virtual ~CDataTreeCtrl();


	HTREEITEM	GetLastVisibleItem();
	HTREEITEM	HitTestEx(CPoint point);
	BOOL		SelectItem(HTREEITEM hitem, UINT code = TVGN_CARET);
	HTREEITEM   GetItemFromPoint(CPoint pt);

	bool IsFirstItem(HTREEITEM hitem);
	bool IsLastItem(HTREEITEM hitem);
	bool VertScrollVisible();

	// Generated message map functions
protected:
	HTREEITEM	m_hLastItem;
	bool		m_bTrack;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDataTreeCtrl)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CDataTreeCtrl)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
///////////////////////////////////////////////////////////////////////////////
// class CContextTreeData
// used for keep item data info in CContextTree

class CContextTreeData
{
	CString		m_strPath; // path to object in current NamedData
	CString		m_strName; //  name of object 
	IUnknownPtr m_UnkPtr;  // pointer to object interface

public:
	CContextTreeData(IUnknown* punk = NULL, LPCTSTR szName = NULL, LPCTSTR szPath = NULL)
	{	m_strName = szName;	m_UnkPtr = IUnknownPtr(punk); m_strPath = szPath;	}
	// constructor of copy by ref
	CContextTreeData(CContextTreeData & data)
	{	m_strName = data.m_strName;	m_UnkPtr = data.m_UnkPtr; m_strPath = data.m_strPath;	}
	// constructor of copy by ptr
	CContextTreeData(CContextTreeData * data)
	{	m_strName = data->m_strName; m_UnkPtr = data->m_UnkPtr; m_strPath = data->m_strPath;	}
	// assign operator 
	CContextTreeData & operator = (const CContextTreeData & data)
	{	m_strName = data.m_strName;	m_UnkPtr = data.m_UnkPtr; m_strPath = data.m_strPath;	}

// name
	void SetName(LPCTSTR szName)
	{	m_strName = szName;	}

	LPCTSTR GetName()
	{	return m_strName;	}

// path
	void SetPath(LPCTSTR szPath)
	{	m_strPath = szPath;	}

	LPCTSTR GetPath()
	{	return m_strPath;	}


// interface pointer
	void SetData(IUnknown* punk)
	{	m_UnkPtr = IUnknownPtr(punk);	}

	IUnknown* GetData() /// NOTE: we call AddRef when return interface pointer
	{	
		if (m_UnkPtr == NULL)
			return NULL;
		
		//m_UnkPtr.AddRef(); 
		return m_UnkPtr.GetInterfacePtr();	
	}
};


/////////////////////////////////////////////////////////////////////////////
// CContextTreeCtrl window
#define ID_INNER_TREE	1002

class CContextTreeCtrl : public CWnd
{
	DECLARE_DYNCREATE(CContextTreeCtrl)

// Construction
public:
	CContextTreeCtrl();
	virtual ~CContextTreeCtrl();

	BOOL Attach(DWORD dwStyle, CWnd* pParentWnd, UINT nID, bool bDynamic = false);

// Attributes
public:
	//active items
	bool GetHilightActiveItems() const
	{	return m_bHilightActiveItems;	}
	void SetHilightActiveItems( bool bHilight )
	{	m_bHilightActiveItems = bHilight;	}

	bool ClearContens();

	bool FillAllTypes(bool bBranch = false);
	bool FillType(LPCTSTR sztype);


// utilities function
	// get HTREEITEM by name
	HTREEITEM GetItemByName(LPCTSTR szname, HTREEITEM hparent = TVI_ROOT);
	
	// get HTREEITEM by object
	HTREEITEM GetItemByObject(IUnknown* punkObj, HTREEITEM hparent);
	
	// find object's parent HTREEITEM for any level objects
	HTREEITEM FindParentItem(IUnknown* punkObject);
	
	// find object's HTREEITEM for any level objects
	HTREEITEM FindObjectItem(IUnknown* punkObject);

	// find object's HTREEITEM for any level objects
	HTREEITEM FindObjectItem( const char *pszObjectName );

	// find object's HTREEITEM for any level objects by object's key
	HTREEITEM FindItemByKey(GuidKey lKey, HTREEITEM hParent = 0);

	// check item has children
	bool ItemHasChildren(HTREEITEM hparent);

	//return selected object
	//IUnknown *GetSelectedObject();

// object's data
	// get object for any level item
	IUnknown* GetObjectByItem(HTREEITEM hitem);
	// get name for any level item
	LPCTSTR	  GetNameByItem(HTREEITEM hitem);
	// get Path for any level item
	LPCTSTR	  GetPathByItem(HTREEITEM hitem);
 
// activities
	// get active object by type
	HTREEITEM GetActiveObject(LPCTSTR sztype);
	// get active object for parent
	HTREEITEM GetActiveChild(HTREEITEM hParent);
	HTREEITEM GetActiveChild(IUnknown* punkParent);
	
	// check object active 
	bool IsObjectActive(IUnknown* punkObj);
	bool IsObjectActive(HTREEITEM hitem);

	// set bold font to object
	bool SetObjectActive(HTREEITEM hitem, bool bActive);

	// set item highlight
	bool SetItemHighlight(HTREEITEM hitem);
	bool SetObjectHighlight(IUnknown* punkObj);

	bool ChangeActive(HTREEITEM hitem, bool bActive);

	// remove all children from hparent
	bool RemoveChilds(HTREEITEM hparent = TVI_ROOT);

	// zero level //////////////////////////////////////
	// add Type - Zero level of tree
	bool AddType(LPCTSTR sztype);
	// remove type with its children
	bool RemoveType(LPCTSTR sztype);
	// remove all types with its children (it's means remove all items from tree)
	// RemoveAllItem
	bool RemoveAllTypes();
	// check hitem id type
	bool ItemIsType(HTREEITEM hitem);

	// first level //////////////////////////////////////
	// add object
	bool AddObject(IUnknown* punkObject, bool bActive = false);
	// remove object
	bool RemoveObject(IUnknown* punkObject);
	// object change active
	bool ChangeActive(IUnknown* punkObject, bool bActive);
	// remove all object of this type
	bool RemoveAllObjects(LPCTSTR sztype);

	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	// without AddRef !!!!!!!!!!!!!!!
	IUnknown*	GetSelectedObject()
	{	return (m_pSelObjectData) ? m_pSelObjectData->GetData() : NULL;	}

	LPCTSTR		GetSelectedObjectName()
	{	return (m_pSelObjectData) ? m_pSelObjectData->GetName() : NULL;	}

	LPCTSTR		GetSelectedObjectPath()
	{	return (m_pSelObjectData) ? m_pSelObjectData->GetPath() : NULL;	}

	bool AttachData(IUnknown* punkView, IUnknown* punkData);

protected:
	// remove child with its data
	bool _RemoveItem(HTREEITEM hitem);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContextTreeCtrl)
	public:
	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL

// Generated message map functions
protected:
	sptrIView				m_sptrView;    // current active view
	sptrIDataContext2		m_sptrContext; // view's context
	sptrIDataTypeManager	m_sptrTypeMgr; // NamedData's Interface of current view

	long					m_lLastViewKey;
	long					m_lLastDocKey;

	CContextTreeData *	m_pSelObjectData;	// for selected object only
	
	CDataTreeCtrl	m_Tree;	// MS Tree Control
	bool			m_bDynamic; // flag of dynamic creation

	//{{AFX_MSG(CContextTreeCtrl)
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
/*	afx_msg void OnDblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRdblclkTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnReturnTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnBeginlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEndlabeleditTree(NMHDR* pNMHDR, LRESULT* pResult);
*/	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

protected:
	
	bool m_bHilightActiveItems;
};



/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATATREECTRL_H__76B75363_520E_11D3_880E_0000B493FF1B__INCLUDED_)
