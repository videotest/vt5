#ifndef __selbinconv_h__
#define __selbinconv_h__

#include "imgfilterbase.h"

//////////////////////////////////////////////////////////////////////
//CActionCreateSelectionByBinary class

class CActionCreateSelectionByBinary : public CActionBase
{
	DECLARE_DYNCREATE(CActionCreateSelectionByBinary)
	GUARD_DECLARE_OLECREATE(CActionCreateSelectionByBinary)
	CObjectListUndoRecord	m_changes;
public:
	CActionCreateSelectionByBinary();
	virtual ~CActionCreateSelectionByBinary();

	ENABLE_UNDO();
public:
//	virtual bool InvokeFilter();	
	virtual bool Invoke();
	virtual bool DoUndo();
	virtual bool DoRedo();

//protected:
//	virtual bool CanDeleteArgument( CFilterArgument *pa );	
};

//////////////////////////////////////////////////////////////////////
//CActionCreateBinaryBySelection class
class CActionCreateBinaryBySelection : public CFilterBase
{
	DECLARE_DYNCREATE(CActionCreateBinaryBySelection)
	GUARD_DECLARE_OLECREATE(CActionCreateBinaryBySelection)
public:
	CActionCreateBinaryBySelection();
	virtual ~CActionCreateBinaryBySelection();

	ENABLE_UNDO();
public:
	virtual bool InvokeFilter();	

protected:
	virtual bool IsAvaible();
	virtual bool CanDeleteArgument( CFilterArgument *pa );	

	bool CheckSelection();
};



#endif //__selbinconv_h__