#ifndef __axactions_h__
#define __axactions_h__


class CAxActionBase : public CActionBase
{
public:
	CAxActionBase();
	virtual ~CAxActionBase();
public:
	POSITION	GetFirstPosition();
	IUnknown	*GetNext( POSITION &pos );
	int			GetControlsCount();
protected:
	bool		InitList();
public:
	sptrIActiveXForm	m_form;
	CPtrList			m_ptrList;
};

class CAxLayoutActionBase : public CAxActionBase
{
public:
	CAxLayoutActionBase();
	virtual ~CAxLayoutActionBase();

	ENABLE_UNDO();
public:
	virtual bool DoChangeLayout() = 0;
	virtual int	 GetMinControlsCount(){return 2;};
public:
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool IsAvaible();
protected:
	CRect	*m_prectSource;
	CRect	*m_prectTarget;
	long	m_nCount;
	CSize	m_sizeOld;
	CSize	m_sizeNew;
	bool	m_bNoResize; // Если акция не ресайзила объектов - просигналить
};

//[ag]1. classes

class CActionShowControlID : public CAxActionBase
{
	DECLARE_DYNCREATE(CActionShowControlID)
	GUARD_DECLARE_OLECREATE(CActionShowControlID)

public:
	CActionShowControlID();
	virtual ~CActionShowControlID();

public:
	virtual bool Invoke();
};

class CActionincMoveSize : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionincMoveSize)
	GUARD_DECLARE_OLECREATE(CActionincMoveSize)

public:
	virtual bool DoChangeLayout();
	virtual int	 GetMinControlsCount(){return 1;};
};

class CActionAddEventHandlers : public CAxActionBase
{
	DECLARE_DYNCREATE(CActionAddEventHandlers)
	GUARD_DECLARE_OLECREATE(CActionAddEventHandlers)

public:
	CActionAddEventHandlers();
	virtual ~CActionAddEventHandlers();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();

	CStringArray	m_arrEventParams;

private:
	bool _CheckIsEventPresent(CString str);
	long _GetLineNumber(CString strText);
	void _DeleteSub(CString strText);
};

class CActionAxSetSize : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxSetSize)
	GUARD_DECLARE_OLECREATE(CActionAxSetSize)

public:
	virtual bool DoChangeLayout();
	virtual int	 GetMinControlsCount(){return 1;};
};

class CActionAxMakeSameSize : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxMakeSameSize)
	GUARD_DECLARE_OLECREATE(CActionAxMakeSameSize)
public:
	virtual bool DoChangeLayout();
};

class CActionAxMakeSameHeight : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxMakeSameHeight)
	GUARD_DECLARE_OLECREATE(CActionAxMakeSameHeight)
public:
	virtual bool DoChangeLayout();
};

class CActionAxMakeSameWidth : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxMakeSameWidth)
	GUARD_DECLARE_OLECREATE(CActionAxMakeSameWidth)
public:
	virtual bool DoChangeLayout();
};

class CActionAxSpaceDown : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxSpaceDown)
	GUARD_DECLARE_OLECREATE(CActionAxSpaceDown)
public:
	virtual bool DoChangeLayout();
	virtual int	 GetMinControlsCount(){return 3;};
};

class CActionAxSpaceAcross : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxSpaceAcross)
	GUARD_DECLARE_OLECREATE(CActionAxSpaceAcross)
public:
	virtual bool DoChangeLayout();
	virtual int	 GetMinControlsCount(){return 3;};
};

class CActionAxAlignBottom : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxAlignBottom)
	GUARD_DECLARE_OLECREATE(CActionAxAlignBottom)
public:
	virtual bool DoChangeLayout();
};

class CActionAxAlignRight : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxAlignRight)
	GUARD_DECLARE_OLECREATE(CActionAxAlignRight)
public:
	virtual bool DoChangeLayout();
};

class CActionAxAlignTop : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxAlignTop)
	GUARD_DECLARE_OLECREATE(CActionAxAlignTop)
public:
	virtual bool DoChangeLayout();
};

class CActionAxAlignLeft : public CAxLayoutActionBase
{
	DECLARE_DYNCREATE(CActionAxAlignLeft)
	GUARD_DECLARE_OLECREATE(CActionAxAlignLeft)
public:
	virtual bool DoChangeLayout();
};

class CActionAxProperties : public CAxActionBase
{
	DECLARE_DYNCREATE(CActionAxProperties)
	GUARD_DECLARE_OLECREATE(CActionAxProperties)

ENABLE_UNDO();
public:
	CActionAxProperties();
	virtual ~CActionAxProperties();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
	virtual bool IsAvaible();
protected:
	CMemFile m_fileUndo;
	CMemFile m_fileRedo;

	IUnknownPtr m_ptrControlData;

	bool SerializeControl( bool bUseUndoBuffer, bool bLoad, bool bUpdateParent );
	bool IsControlChanged();
};



class CActionAxResize : public CAxActionBase//CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionAxResize)
	GUARD_DECLARE_OLECREATE(CActionAxResize)
ENABLE_UNDO();
public:
	CActionAxResize();
	virtual ~CActionAxResize();
/*
public:
	virtual void Initialize();
	virtual void Finalize();
	virtual bool DoLButtonDown( CPoint point );
	virtual bool DoSetCursor( CPoint point );
	virtual void DoDraw( CDC &dc );
  */
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
	
protected:

	//sptrIActiveXForm	m_form;
	CSize				m_sizeOld;
	CSize				m_sizeNew;
	//CRectTracker		m_tracker;	
};

class CActionAxInsertControl : public CInteractiveActionBase
{
	DECLARE_DYNCREATE(CActionAxInsertControl)
	GUARD_DECLARE_OLECREATE(CActionAxInsertControl)

//ENABLE_SETTINGS();
ENABLE_UNDO();
public:
	CActionAxInsertControl();
	virtual ~CActionAxInsertControl();

	virtual bool DoStartTracking( CPoint point );
	virtual bool DoSetCursor( CPoint point );
	
	virtual bool Initialize();
	virtual void Finalize();

	virtual void LeaveMode();

public:
	virtual bool Invoke();
	//virtual bool ExecuteSettings( CWnd *pwndParent );
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
//update interface
	virtual bool IsAvaible();
protected:

	CObjectListUndoRecord	m_changes;
	CString					m_strProgID;
	CSize					m_sizeOld;
	CSize					m_sizeNew;
	CRectTracker			m_tracker;
	HCURSOR					m_hcur_add;
};

class CActionShowAxLayout : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowAxLayout)
	GUARD_DECLARE_OLECREATE(CActionShowAxLayout)
public:
	virtual CString GetViewProgID();
};


#endif //__axactions_h__
