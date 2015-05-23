#ifndef __actions_h__
#define __actions_h__

//[ag]1. classes

class CActionCompareScreen :	public CActionBase,
								public CLongOperationImpl
{
	ENABLE_UNDO()
	DECLARE_DYNCREATE(CActionCompareScreen)
	GUARD_DECLARE_OLECREATE(CActionCompareScreen)
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

public:
	CActionCompareScreen();
	virtual ~CActionCompareScreen();

public:
	virtual bool Invoke();

	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	bool _GetImages();
	bool _CalcBounds();
	IUnknown* _GetView(int nRow);


private:
	IImagePtr	m_sptrImg0;
	IImagePtr	m_sptrImg1;
	CPoint m_ptOrigin0;
	CPoint m_ptOrigin1;
	long m_nWidth0;
	CSize m_sizeImg;
	CString m_strCC0;
	CString m_strCC1;
	double m_fZoom0;
	double m_fZoom1;

	CObjectListUndoRecord	m_changes;
	IDocumentPtr			m_sptrDoc;
};

class CActionSaveViewState : public CActionBase
{
	DECLARE_DYNCREATE(CActionSaveViewState)
	GUARD_DECLARE_OLECREATE(CActionSaveViewState)

	ENABLE_SETTINGS();
public:
	CActionSaveViewState();
	virtual ~CActionSaveViewState();

public:
	virtual bool Invoke();
	bool ExecuteSettings( CWnd *pwndParent );

	CString m_strFileName;
};

class CActionLoadViewState : public CActionBase
{
	DECLARE_DYNCREATE(CActionLoadViewState)
	GUARD_DECLARE_OLECREATE(CActionLoadViewState)

	ENABLE_SETTINGS();
public:
	CActionLoadViewState();
	virtual ~CActionLoadViewState();

public:
	virtual bool Invoke();
	bool ExecuteSettings( CWnd *pwndParent );

	CString m_strFileName;
};

class CActionRenameView : public CActionBase
{
	DECLARE_DYNCREATE(CActionRenameView)
	GUARD_DECLARE_OLECREATE(CActionRenameView)

	ENABLE_SETTINGS();
public:
	CActionRenameView();
	virtual ~CActionRenameView();

	CString m_strName;

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
};

class CActionRemoveView : public CActionBase
{
	DECLARE_DYNCREATE(CActionRemoveView)
	GUARD_DECLARE_OLECREATE(CActionRemoveView)

public:
	CActionRemoveView();
	virtual ~CActionRemoveView();

public:
	virtual bool Invoke();
};

class CActionAddView : public CActionBase
{
	DECLARE_DYNCREATE(CActionAddView)
	GUARD_DECLARE_OLECREATE(CActionAddView)

	ENABLE_SETTINGS();
public:
	CActionAddView();
	virtual ~CActionAddView();

public:
	CString m_strName;

	virtual bool Invoke();
	virtual bool ExecuteSettings( CWnd *pwndParent );
};


#endif //__actions_h__
