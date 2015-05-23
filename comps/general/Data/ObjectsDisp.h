#if !defined(AFX_OBJECTSDISP_H__F247F7B7_6AFD_441C_8244_386896389F51__INCLUDED_)
#define AFX_OBJECTSDISP_H__F247F7B7_6AFD_441C_8244_386896389F51__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ObjectsDisp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CObjectsDisp command target

class CObjectsDisp : public CCmdTarget, 
					 public CNamedObjectImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CObjectsDisp)

	CObjectsDisp();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CObjectsDisp)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CObjectsDisp();

	// Generated message map functions
	//{{AFX_MSG(CObjectsDisp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CObjectsDisp)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CObjectsDisp)
	afx_msg LPDISPATCH GetActiveImage();
	afx_msg void SetActiveImage(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetActiveObjectList();
	afx_msg void SetActiveObjectList(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetActiveClasses();
	afx_msg void SetActiveClasses(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetActiveObject(LPCTSTR szType);
	afx_msg long GetObjectCount(LPCTSTR szType);
	afx_msg long GetTypesCount();
	afx_msg BSTR GetType(long nTypeNo);
	afx_msg void AddObject(LPDISPATCH pdispObject);
	afx_msg void RemoveObject(LPDISPATCH pdispObject);
	afx_msg void SetObjectActive(LPDISPATCH pdispObject);
	afx_msg LPDISPATCH CreateNewObject(LPCTSTR szType);
	afx_msg LPDISPATCH GetObjectByName(LPCTSTR szName);
	afx_msg LONG_PTR GetFirstObjectPos(LPCTSTR szType);
	afx_msg long GetNextObjectPos(LPCTSTR szType, long lPos);
	afx_msg long GetLastObjectPos(LPCTSTR szType);
	afx_msg long GetPrevObjectPos(LPCTSTR szType, long lPos);
	afx_msg LPDISPATCH GetObject(LPCTSTR szType, long lPos);
	afx_msg void GetFirstSelectedObjectPos(LPCTSTR szType, VARIANT FAR* pvarPos);
	afx_msg void GetLastSelectedObjectPos(LPCTSTR szType, VARIANT FAR* pvarPos);
	afx_msg LPDISPATCH GetNextSelectedObject(LPCTSTR szType, VARIANT FAR* pvarPos);
	afx_msg LPDISPATCH GetPrevSelectedObject(LPCTSTR szType, VARIANT FAR* pvarPos);
	afx_msg long GetSelectedObjectsCount(LPCTSTR szType);
	afx_msg void SelectObject(LPDISPATCH pObject, BOOL bSelect);
	afx_msg LPDISPATCH GetSelectedObject(LPCTSTR szType, LONG_PTR lPos);
	afx_msg LPDISPATCH GetSelectedByIndex(LPCTSTR szType, long nIndex);
	afx_msg long SetToActiveDocument(LPDISPATCH pobj);
	afx_msg LPDISPATCH CloneObject(LPDISPATCH obj);
	afx_msg BSTR GenerateNewKey();
	afx_msg BOOL SetBaseObjectKey( LPCTSTR szType, long lPos, LPCTSTR lpctVal );
	afx_msg BSTR GetBaseObjectKey( LPCTSTR szType, long lPos );
	afx_msg BOOL IsBaseObject(LPCTSTR szType, long lPos );
	afx_msg BSTR GetObjectKey( LPCTSTR szType, long lPos );
	afx_msg LPDISPATCH GetLastActiveObject(LPCTSTR szType);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

private:
	void ErrorMsg(UINT nErrorID);
	LPDISPATCH _GetActiveObject(CString strType);
	void _SetActiveObject(IUnknown* punkObj);
	bool CheckType(CString strType);
	IUnknown* _GetActiveContext();
	BSTR _guid_to_bstr( LPGUID lpGuid );
	void _bstr_to_guid( LPCTSTR lpctVal ,LPGUID lpGuid );
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECTSDISP_H__F247F7B7_6AFD_441C_8244_386896389F51__INCLUDED_)
