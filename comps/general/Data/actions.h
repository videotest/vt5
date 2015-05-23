#ifndef __actions_h__
#define __actions_h__

//[ag]1. classes

class CActionShowContext : public CActionBase
{
	DECLARE_DYNCREATE(CActionShowContext)
	GUARD_DECLARE_OLECREATE(CActionShowContext)

public:
	CActionShowContext();
	virtual ~CActionShowContext();

public:
	virtual bool Invoke();
	//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
};

class CActionObjectUnselectAll : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectUnselectAll)
	GUARD_DECLARE_OLECREATE(CActionObjectUnselectAll)

public:
	CActionObjectUnselectAll();
	virtual ~CActionObjectUnselectAll();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
};

class CActionDataShowContent : public CActionBase
{
	DECLARE_DYNCREATE(CActionDataShowContent)
	GUARD_DECLARE_OLECREATE(CActionDataShowContent)

public:
	CActionDataShowContent();
	virtual ~CActionDataShowContent();

public:
	virtual bool Invoke();
};

class CActionDeleteObject : public CActionBase
{
	DECLARE_DYNCREATE(CActionDeleteObject)
	GUARD_DECLARE_OLECREATE(CActionDeleteObject)

ENABLE_UNDO();
public:
	CActionDeleteObject();
	virtual ~CActionDeleteObject();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
	CObjectListUndoRecord	m_changes;
};

class CActionObjectDrop : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectDrop)
	GUARD_DECLARE_OLECREATE(CActionObjectDrop)

	ENABLE_UNDO();
public:
	CActionObjectDrop();
	virtual ~CActionObjectDrop();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
protected:
	CPoint	m_point;
	sptrINamedDataObject2	m_sptrDrop,			//new "dropped" data object
							m_sptrActive;		//object to store
	GuidKey	m_lActiveParent;

	CObjectListUndoRecord	m_changes;
};

class CActionObjectDrag : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectDrag)
	GUARD_DECLARE_OLECREATE(CActionObjectDrag)

	ENABLE_UNDO();
public:
	//static long s_lCurrentParentKey;
	
public:
	CActionObjectDrag();
	virtual ~CActionObjectDrag();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	GuidKey	m_lParentKey;
	sptrINamedDataObject2	m_sptrDrag;
	IUndoneableActionPtr	m_ptrDropUndo;
	IActionPtr				m_ptrDropAction;

	CObjectListUndoRecord	m_changes;
};

class CActionEditCopy : public CActionBase
{
	DECLARE_DYNCREATE(CActionEditCopy)
	GUARD_DECLARE_OLECREATE(CActionEditCopy)
public:
	CActionEditCopy();
	virtual ~CActionEditCopy();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
protected:
	void PutNativeFormatToClipboard(IUnknown* punkDataObject);
	void PutObjectToClipboard(IUnknown* punkDataObject);

	IUnknown *m_punkDataObject;
};

class CActionEditCut : public CActionEditCopy
{
	DECLARE_DYNCREATE(CActionEditCut)
	GUARD_DECLARE_OLECREATE(CActionEditCut)

ENABLE_UNDO();
public:
	CActionEditCut();
	virtual ~CActionEditCut();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

	CObjectListUndoRecord	m_changes;
};

class CActionEditPaste : public CActionBase
{
	DECLARE_DYNCREATE(CActionEditPaste)
	GUARD_DECLARE_OLECREATE(CActionEditPaste)

ENABLE_UNDO();

	CArray<IUnknownPtr,IUnknownPtr&> m_arrUndoObjects;
public:
	CActionEditPaste();
	virtual ~CActionEditPaste();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
protected:
	IUnknown* CreateDataObject(CString strObjectType, CString strObjectName = "", CStreamEx*	par = 0);
	void SetObjectToDoc(IUnknown* punkObj);

	//IUnknown *m_punkDataObject;
	CObjectListUndoRecord	m_changes;
};

class CActionDataToExcelAvail : public CActionBase
{
	DECLARE_DYNCREATE(CActionDataToExcelAvail)
	GUARD_DECLARE_OLECREATE(CActionDataToExcelAvail)
public:
	CActionDataToExcelAvail();
	virtual ~CActionDataToExcelAvail();

	virtual bool IsAvaible();
	virtual bool Invoke();
};

class CActionClipboardAvail : public CActionBase
{
	DECLARE_DYNCREATE(CActionClipboardAvail)
	GUARD_DECLARE_OLECREATE(CActionClipboardAvail)

//ENABLE_UNDO();
public:
	CActionClipboardAvail();
	virtual ~CActionClipboardAvail();

public:
	virtual bool Invoke();
//undo interface
	//virtual bool DoUndo();
	//virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
protected:
	//IUnknown* CreateDataObject(CString strObjectType, CString strObjectName = "", CStreamEx*	par = 0);
	//void SetObjectToDoc(IUnknown* punkObj);

	//IUnknown *m_punkDataObject;
	//CObjectListUndoRecord	m_changes;
};

class CActionObjectMoveBase : public CActionBase
{
	ENABLE_UNDO();
public:
	CActionObjectMoveBase();
	virtual ~CActionObjectMoveBase();
public:	
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();
protected:
	bool MoveObjects();
protected:
	CString		m_strObjectName;
	IUnknown	*m_punkObjToMove;
	IUnknown	*m_punkFrom;
	IUnknown	*m_punkTo;
	GuidKey		m_lParentKey;
};

class CActionDataMoveToDoc : public CActionObjectMoveBase
{
	DECLARE_DYNCREATE(CActionDataMoveToDoc)
	GUARD_DECLARE_OLECREATE(CActionDataMoveToDoc)
public:
	virtual bool Invoke();
};

class CActionDataMoveToApp : public CActionObjectMoveBase
{
	DECLARE_DYNCREATE(CActionDataMoveToApp)
	GUARD_DECLARE_OLECREATE(CActionDataMoveToApp)
public:
	virtual bool Invoke();
};

class CActionDataOrganizer : public CActionBase
{
	DECLARE_DYNCREATE(CActionDataOrganizer)
	GUARD_DECLARE_OLECREATE(CActionDataOrganizer)
public:
	CActionDataOrganizer();
	virtual ~CActionDataOrganizer();

public:
	virtual bool Invoke();
};

class CActionObjectSave : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectSave)
	GUARD_DECLARE_OLECREATE(CActionObjectSave)

	ENABLE_SETTINGS();
public:
	CActionObjectSave();
	virtual ~CActionObjectSave();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

protected:
	IUnknownPtr			m_ptrObj;
	CString				m_strPath;
	IFileFilter2Ptr		m_ptrFilter;
};

class CActionObjectLoad : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectLoad)
	GUARD_DECLARE_OLECREATE(CActionObjectLoad)

	ENABLE_SETTINGS();
	ENABLE_UNDO();

	
	//bool InsertToDBaseDocument();

public:
	CActionObjectLoad();
	virtual ~CActionObjectLoad();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	CCompManager		m_manObjects;

	CString				m_strPath;
	CString				m_strType;
	sptrIFileFilter2	m_spFilter;
	CObjectListUndoRecord	m_changes;
};

class CActionObjectProperties : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectProperties)
	GUARD_DECLARE_OLECREATE(CActionObjectProperties)

	ENABLE_SETTINGS();
public:
	CActionObjectProperties();
	virtual ~CActionObjectProperties();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

protected:
	IUnknown*	m_punkObj;

};

class CActionObjectRename : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectRename)
	GUARD_DECLARE_OLECREATE(CActionObjectRename)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionObjectRename();
	virtual ~CActionObjectRename();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	IUnknown *	m_punkObjToRename;
	CString		m_strNewName;
	CString		m_strOldName;
	
	CObjectListUndoRecord	m_changes;
};

class CActionObjectDelete : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectDelete)
	GUARD_DECLARE_OLECREATE(CActionObjectDelete)

	ENABLE_UNDO();
public:
	CActionObjectDelete();
	virtual ~CActionObjectDelete();

public:
	virtual bool Invoke();

	virtual bool DoUndo();
	virtual bool DoRedo();
protected:
	CObjectListUndoRecord	m_changes;

};

class CActionObjectSelect : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectSelect)
	GUARD_DECLARE_OLECREATE(CActionObjectSelect)

	ENABLE_SETTINGS();
public:
	CActionObjectSelect();
	virtual ~CActionObjectSelect();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

protected:	
	IUnknownPtr	m_sptrObjToActivate;
	CString		m_strType;
};

class CActionObjectCreate : public CActionBase
{
	DECLARE_DYNCREATE(CActionObjectCreate)
	GUARD_DECLARE_OLECREATE(CActionObjectCreate)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionObjectCreate();
	virtual ~CActionObjectCreate();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );

	virtual bool DoUndo();
	virtual bool DoRedo();


	_bstr_t	m_bstrType;
	_bstr_t	m_bstrName;
	IUnknown *	m_punkCreatedObject;

	CObjectListUndoRecord	m_changes;
};



class CActionCreateImageObjectByView : public CActionBase, public CLongOperationImpl
{
	DECLARE_DYNCREATE(CActionCreateImageObjectByView)
	GUARD_DECLARE_OLECREATE(CActionCreateImageObjectByView)

	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();


	ENABLE_UNDO();
public:
	CActionCreateImageObjectByView();
	virtual ~CActionCreateImageObjectByView();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();
	CObjectListUndoRecord	m_changes;

protected:
	IPrintViewPtr GetActivePrintView();	
	
};



class CActionTouchObject : public CActionBase
{
	DECLARE_DYNCREATE(CActionTouchObject)
	GUARD_DECLARE_OLECREATE(CActionTouchObject)

ENABLE_UNDO();
public:
	CActionTouchObject();
	virtual ~CActionTouchObject();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
	CObjectListUndoRecord	m_changes;
};






#endif //__actions_h__
