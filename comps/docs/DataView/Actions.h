#ifndef __Actions_h__
#define __Actions_h__

#include "Measure5.h"
#include "ClassBase.h"
#include "CreateParamDlg.h"
#include "ObListWrp.h"

#include <AfxTempl.h>
#include "Propbag.h"
#include "\vt5\awin\misc_list.h"
#include "\vt5\awin\misc_ptr.h"

//[ag]1. classes

class CActionSetParamName : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetParamName)
	GUARD_DECLARE_OLECREATE(CActionSetParamName)

	ENABLE_UNDO();
public:
	CActionSetParamName();
	virtual ~CActionSetParamName();
public:
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();
protected:
	long	m_lParam;
	_bstr_t	m_bstrOldName, m_bstrNewName;
	ICalcObjectContainerPtr	m_ptrC;
};

class CActionGridSetParamWidth : public CActionBase
{
	DECLARE_DYNCREATE(CActionGridSetParamWidth)
	GUARD_DECLARE_OLECREATE(CActionGridSetParamWidth)

public:
	CActionGridSetParamWidth();

	ENABLE_UNDO();
public:
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	long	m_lParam, m_lOldWidth, m_lNewWidth;
	ICalcObjectContainerPtr	m_ptrC;
};

// vanek
class CActionGridSetObjectHeight : public CActionBase
{
	DECLARE_DYNCREATE(CActionGridSetObjectHeight)
	GUARD_DECLARE_OLECREATE(CActionGridSetObjectHeight)

public:
	CActionGridSetObjectHeight();

	ENABLE_UNDO();
public:
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	long	m_lOldHeight, m_lNewHeight;
	INamedPropBagPtr m_sptrPropBag;
};
//


class CActionGridMoveParam : public CActionBase
{
	DECLARE_DYNCREATE(CActionGridMoveParam)
	GUARD_DECLARE_OLECREATE(CActionGridMoveParam)

public:
	CActionGridMoveParam();

	ENABLE_UNDO();
	public:
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();

	long	m_lParam, m_lOldPrevParam, m_lNewPrevParam;
	ICalcObjectContainerPtr	m_ptrC;
};


class CActionAlignByDefault : public CActionBase
{
	DECLARE_DYNCREATE(CActionAlignByDefault)
	GUARD_DECLARE_OLECREATE(CActionAlignByDefault)

public:
	CActionAlignByDefault();
	virtual ~CActionAlignByDefault();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
};

class CHeightUndoImpl
{
public:
	bool Remember(IUnknown *punkList );
protected:
	_list_t<long> m_Heights;
	_list_t<INamedPropBagPtr> m_Objects;
	IUnknownPtr m_sptrObjList;
	virtual bool _undo_redo( IUnknown *punkDoc );
    void get_named_data_object_list( IUnknown **ppunkND );
	
	void swap( long *plPrev, long *plNext )
	{
		long lTmp = *plPrev;
		*plPrev = *plNext;
		*plNext = lTmp;
	}
};

class CGridViewBase;
class CActionAlignByCell : public CActionBase
{
	DECLARE_DYNCREATE(CActionAlignByCell)
	GUARD_DECLARE_OLECREATE(CActionAlignByCell)

	ENABLE_SETTINGS();

public:
	CActionAlignByCell();
	virtual ~CActionAlignByCell();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings(CWnd *pwndParent);
//update interface
	virtual bool IsAvaible();

protected:
	int nRow;
	int nCol;
	CGridViewBase * m_pView;
};

class CActionRemoveAllMMObjects : public CActionBase
{
	DECLARE_DYNCREATE(CActionRemoveAllMMObjects)
	GUARD_DECLARE_OLECREATE(CActionRemoveAllMMObjects)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionRemoveAllMMObjects();
	virtual ~CActionRemoveAllMMObjects();

public:
	virtual bool Invoke();
	virtual bool ExecuteSettings(CWnd * pwndParent);
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

//update interface
	bool IsAvaible();

protected:
	CObjectListUndoRecord	m_changes;	//undo
	CObjectListWrp			m_list;
	IUnknown *				m_punkDoc;

};

class CActionZoomOutTable : public CActionBase
{
	DECLARE_DYNCREATE(CActionZoomOutTable)
	GUARD_DECLARE_OLECREATE(CActionZoomOutTable)
	int m_nMinFontSize;
public:
	CActionZoomOutTable();
	virtual ~CActionZoomOutTable();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
};

class CActionZoomInTable : public CActionBase
{
	DECLARE_DYNCREATE(CActionZoomInTable)
	GUARD_DECLARE_OLECREATE(CActionZoomInTable)

public:
	CActionZoomInTable();
	virtual ~CActionZoomInTable();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
};

class CActionRemoveParam : public CActionBase
{
	DECLARE_DYNCREATE(CActionRemoveParam)
	GUARD_DECLARE_OLECREATE(CActionRemoveParam)

	ENABLE_UNDO();

	// For undo
	bool m_bAnyObjectsWasExist;
	CString m_sName,m_sUserName;
	long m_lKey;
	ParamType m_type;
	_ptr_t<double> m_Values;
	_ptr_t<BOOL> m_ValuesInit;
public:
	CActionRemoveParam();
	virtual ~CActionRemoveParam();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
protected:
	virtual bool IsAvaible();
	long					m_lParamKey;
	ICalcObjectContainerPtr	m_ptrC;

	CObjectListUndoRecord	m_changes;
};


class CActionCreateParam : public CActionBase
{
	DECLARE_DYNCREATE(CActionCreateParam)
	GUARD_DECLARE_OLECREATE(CActionCreateParam)

	ENABLE_UNDO();
public:
	CActionCreateParam();
	virtual ~CActionCreateParam();

public:
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();
protected:
	ParamType		m_type;
	CString			m_strParamName;
	long			*m_pCreateKeys, m_nCount;
	ICalcObjectContainerPtr	m_ptrC;
	IMeasParamGroupPtr		m_ptrG;
	CObjectListUndoRecord	m_changes;
};


/*class CActionSetParamValue : public CActionBase
{
	DECLARE_DYNCREATE(CActionSetParamValue)
	GUARD_DECLARE_OLECREATE(CActionSetParamValue)

	ENABLE_SETTINGS();
	ENABLE_UNDO();
public:
	CActionSetParamValue();
	virtual ~CActionSetParamValue();

public:
	virtual bool ExecuteSettings(CWnd *pwndParent);
	virtual bool Invoke();
//undo interface
	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
//	virtual bool IsAvaible();
	IUnknown *	GetActiveList();

protected:
	long	m_lParamKey;	// param key
	double	m_fNewValue;	// new value
	bool	m_bNeedNewValue;// need new value
	double	m_fPrevValue;	// old value
	bool	m_bHasPrevValue;// old value exists ? 

	ICalcObjectPtr			m_sptrObject;
//	IManualMeasObjectPtr	m_sptrMMObject;
	CObjectListWrp			m_list;
	CObjectListUndoRecord	m_changes;	//undo
};*/


class CActionShowMeasView : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowMeasView)
	GUARD_DECLARE_OLECREATE(CActionShowMeasView)
public:
	virtual CString GetViewProgID();
};

class CActionShowClassView : public CActionShowViewBase
{
	DECLARE_DYNCREATE(CActionShowClassView)
	GUARD_DECLARE_OLECREATE(CActionShowClassView)
public:
	virtual CString GetViewProgID();
};

/////////////////////////////////////////////////////////////////////////////
class CActionAlignByImage : public CActionBase, public CHeightUndoImpl
{
	DECLARE_DYNCREATE(CActionAlignByImage)
	GUARD_DECLARE_OLECREATE(CActionAlignByImage)

public:
	CActionAlignByImage();
	virtual ~CActionAlignByImage();

	ENABLE_UNDO();
public:
	virtual bool Invoke( );
	virtual bool DoUndo( ) { return _undo_redo( m_punkTarget ); }
	virtual bool DoRedo( ) { return _undo_redo( m_punkTarget ); }
//update interface
	virtual bool IsAvaible();
	virtual bool _undo_redo( IUnknown *punkDoc );

protected:
	// [vanek] SBT: 659
	long m_lold_img_col_width;
	void get_image_column_width( long *plwidth );
    void set_image_column_width( long lwidth );
};

/////////////////////////////////////////////////////////////////////////////
class CActionCopySelected : public CActionBase
{
	DECLARE_DYNCREATE(CActionCopySelected)
	GUARD_DECLARE_OLECREATE(CActionCopySelected)

public:
	CActionCopySelected();

	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
protected:
	copy_flags	m_cf;
};

/////////////////////////////////////////////////////////////////////////////
class CActionCopyAll : public CActionCopySelected
{
	DECLARE_DYNCREATE(CActionCopyAll)
	GUARD_DECLARE_OLECREATE(CActionCopyAll)

public:
	CActionCopyAll();
};

#endif //__Actions_h__
