#ifndef __send_h__
#define __send_h__

#include "PopupInt.h"


class CBaseObject
{
public:
	CBaseObject();
	~CBaseObject();
	bool IsChild( INamedDataPtr ptrND, GuidKey guidChild );

	GuidKey m_guidBaseObject;
	GuidKey m_guidGroup;

	CPtrList m_arGuidChild;
};


//[ag]1. classes

class CActionSendTo : public CActionBase, public CLongOperationImpl
{
	DECLARE_DYNCREATE(CActionSendTo)
	GUARD_DECLARE_OLECREATE(CActionSendTo)	
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP();

public:
	ENABLE_UNDO();
	CActionSendTo();
	virtual ~CActionSendTo();

public:
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();
	virtual bool IsChecked();

	virtual bool DoUndo();
	virtual bool DoRedo();

protected:
	bool CreateObjectList();
	void DestroyObjectList();
	
	//Return base group key
	GuidKey IsBaseObject( INamedDataPtr ptrND, GuidKey guidObj );	
	bool IsObjectExistInBaseArray( GuidKey guidObj );
	
	UINT		m_uiItemID;
	BOOL		m_bPutToDocument;
	BOOL		m_bNewDocument;
	GuidKey		m_guidSourceDoc;
	GuidKey		m_guidTargetDoc;

	CString		m_strSingleObjectName;

	CPtrList	m_arBaseObject;
	CPtrList	m_arGuidNotInGroup;

	IUnknownPtr PutToDocument( IUnknown* punkObjectSrc );

	bool PutToData( IUnknown* punkND, _bstr_t bstrName, IUnknown* punkObject, const char* psz_obj_name = 0, GUID *pGuidBase = NULL/*A.M. SBT4961*/ );
	
	bool IsPutToDocument();

	//If parent not found kill parent ptr in child...
	void SafeCorrectParent( IUnknown* punkNamedData, IUnknown* punkObj );

	CObjectListUndoRecord	m_changes;
};

#endif//__send_h__