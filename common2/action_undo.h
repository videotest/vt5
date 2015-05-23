#ifndef __action_undo_h__
#define __action_undo_h__

#include "action5.h"
#include "data5.h"
#include "misc_list.h"


class ObjectData
{
protected:
	GuidKey m_lParentKey;
	bool	m_bObjectIsVirtual;
	bool	m_bObjectIsAdded;
	bool	m_bMap;
	INamedDataObject2Ptr	m_object;
	bool	m_bChangeDocData;
public:
	ObjectData( IUnknown *punkObject, bool bObjectAdded, bool bMapDataToParent = false );
public:
	void DoUndo( IUnknown *punkDocData );
	void DoRedo( IUnknown *punkDocData, bool  bCleanVirtualsBack=false );
protected:
	void AddObject( IUnknown *punkDocData );
	void RemoveObject( IUnknown *punkDocData, bool  bCleanVirtualsBack = false );
public:
	bool IsAdded() const			{return m_bObjectIsAdded;}
	IUnknown *GetObject() const		{return m_object;}
	bool IsDataChange() const		{return m_bChangeDocData;}
	bool IsVirtual() const			{return m_bObjectIsVirtual;}
};

/////////////////////////////////////////////////////////////////////////////////////////
//helper class for do/undo/redo changes in NamedData
class CDocHelper : public _list_ptr_t<ObjectData*>
{
public:
	CDocHelper();
	virtual ~CDocHelper();

	void DeInit();

	bool m_bCleanVirtualsBack;

public:
	void SetToDocData( IUnknown *punkDocData, IUnknown *punkObject );		//performs adding object to the named data
	void RemoveFromDocData( IUnknown *punkDocData, IUnknown *punkObject, bool bMapData = true, bool bCleanVirtualsBack = false );	//preform removing object from named data

	virtual void DoUndo( IUnknown *punkDocData );
	void DoRedo( IUnknown *punkDocData );
protected:
	ObjectData *AddObject( IUnknown *punk, bool bObjectAdded, bool bMapData );

};

class CUndoneableActionImpl : public IUndoneableAction, 
								public CDocHelper
{
public:
	CUndoneableActionImpl();
//IUndoneableAction
	com_call StoreState();
	com_call Undo();
	com_call Redo();
	com_call GetTargetKey( GUID *plKey );

	void OnAttachTarget( IUnknown *punkTarget );

	void SetModified( bool bSet );
	void ResetModified();

	com_call DoUndo();
	com_call DoRedo();


	bool PrepareTarget( bool bPrepare );
protected:
	IUnknown	*_GetTarget();
protected:
	GuidKey			m_guidTarget;
	bool			m_bOldState;
	bool			m_bFirstCall;
};


#endif //__action_undo_h__