#if !defined(AFX_OBJECT_MANAGER_H__5A900D46_D059_4ABB_B1E4_98A19429AEE0__INCLUDED_)
#define AFX_OBJECT_MANAGER_H__5A900D46_D059_4ABB_B1E4_98A19429AEE0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// object_manager.h : header file
//

#include "obj_man.h"

class CObjectInfo
{
public:
	CObjectInfo()
	{
		m_bActive = false;
	}

	CString		m_strFileName;	
	IUnknownPtr	m_ptrObj;
	IUnknownPtr	m_ptrDoc;
	bool		m_bActive;
};

class CTypeInfo
{
public:
	CTypeInfo(){}
	~CTypeInfo()
	{
		POSITION pos = m_listObj.GetHeadPosition();
		while( pos )
		{
			 CObjectInfo* p = (CObjectInfo*)m_listObj.GetNext( pos );
			 delete p;
		}

		m_listObj.RemoveAll();
	}


	CString		m_strTypeName;
	CPtrList	m_listObj;
};

class CDocumentInfo
{
public:
	IUnknownPtr	m_ptrDocument;
	IUnknownPtr	m_ptrView;
};


class CFileFilterCache
{
public:
	IFileFilter2Ptr m_ptrFilter;
	CString			m_strType;
	CString			m_strExtension;
};


/////////////////////////////////////////////////////////////////////////////
// object_manager command target

class object_manager :		public CCmdTarget,
							public CEventListenerImpl,
							public CNamedObjectImpl
{
	DECLARE_DYNCREATE(object_manager)
	ENABLE_MULTYINTERFACE()
	object_manager();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(object_manager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~object_manager();

	// Generated message map functions
	//{{AFX_MSG(object_manager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(object_manager)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(object_manager)
	afx_msg long GetFirstObjectTypePos();
	afx_msg long GetNextObjectTypePos(long lPos);
	afx_msg BSTR GetObjectType(long lPos);
	afx_msg long GetNextObjectPos(LPCTSTR pszType, long lPos);
	afx_msg long GetFirstObjectPos(LPCTSTR pszType);
	afx_msg BSTR GetObjectName(LPCTSTR pszType, long lPos);
	afx_msg BSTR GetActiveObjectName(LPCTSTR pszType);
	afx_msg BOOL SetActiveObjectByName(LPCTSTR pszType, LPCTSTR pszName);
	afx_msg LPDISPATCH GetActiveObject(LPCTSTR pszType);
	afx_msg BOOL SetActiveObject(LPDISPATCH pIDispatch);
	afx_msg LPDISPATCH GetObjectByName(LPCTSTR pszType, LPCTSTR pszName);
	afx_msg BOOL IsObjectInDocuments(LPCTSTR pszType, LPCTSTR pszName);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


	BEGIN_INTERFACE_PART(ObjectManager, IObjectManager)
		//enum types
		com_call GetFirstObjectTypePos( long* plPos );
		com_call GetNextObjectType( long* plPos, BSTR* pbstrTypeName );

		//enum names
		com_call GetFirstObjectNamePos( BSTR bstrType, long * plPos );
		com_call GetNextObjectByName( BSTR bstrType, long * plPos, BSTR* pbstrName );

		//enum unknown
		com_call GetFirstObjectPos( BSTR bstrType, long * plPos );
		com_call GetNextObject( BSTR bstrType, long * plPos, IUnknown** ppunkObj );

		//by name activation
		com_call GetActiveObjectName( BSTR bstrType, BSTR* pbstrName );
		com_call SetActiveObjectByName( BSTR bstrType, BSTR bstrName );

		//by unknown activation
		com_call GetActiveObject( BSTR bstrType, IUnknown** ppunkObj );
		com_call SetActiveObject( IUnknown* punkObj );

		
		com_call FreeLoadedObject( BSTR bstrType, BSTR bstrName );
		com_call GetObject( BSTR bstrType, BSTR bstrName, IUnknown** ppunkObj );

		com_call GetObjectDocument( BSTR bstrType, BSTR bstrName, IUnknown** ppunkDoc );
	END_INTERFACE_PART(ObjectManager);


	//notification
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );


	
	//helpers
	IUnknown*	get_active_document();
	IUnknown*	get_active_view();


	//notifications
	bool		_on_activate_view( IUnknown* punkDoc, IUnknown* punkView );
	bool		_on_close_document( IUnknown* punkDoc );
	bool		_on_destroy_view( IUnknown* punkView );
	bool		_on_data_context_change( IUnknown* punkView, IUnknown* punkObj, NotifyCodes nc );

	//objects & types
	bool			append_type_list_from_dirs();	
	bool			append_type_list_from_doc( IUnknown* punkDoc );
	bool			remove_type_list_by_doc( IUnknown* punkDoc );
	

	
	void			_free_type_list();
	CPtrList		m_listType;
	
	
	CTypeInfo*		_add_type_info( const char* pszTypeInfoName );

	CTypeInfo*		_find_type_info( const char* pszTypeInfoName );
	CObjectInfo*	_find_active_object_info( const char* pszTypeInfoName );
	CObjectInfo*	_find_object_info( IUnknown* punkObj, CTypeInfo** ppti = 0, POSITION* ppos = 0  );
	CObjectInfo*	_find_object_info_by_name( const char* pszTypeName , const char* pszObjectName );
	
	

	CString			get_object_name( CObjectInfo* poi, bool bForceLoad = false );
	bool			load_object( CObjectInfo* poi, bool bForceReloadLoad = false  );

	
	//document list
	CPtrList		m_listDocs;
	void			_free_doc_list();

	CDocumentInfo*	_find_doc_info( IUnknown* punkDoc, POSITION* ppos = 0 );


	//file filters cache
	void				_free_file_filters_list();	
	CFileFilterCache*	_find_filter_by_ext( const char* pszExt );
	CString				_get_file_name_ext( const char* pszFileName );
	CFileFilterCache*	_create_file_filter_item( const char*  pszFileName );
	CPtrList			m_listFileFilters;


};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OBJECT_MANAGER_H__5A900D46_D059_4ABB_B1E4_98A19429AEE0__INCLUDED_)
