#ifndef __data_main_h__
#define __data_main_h__

#include "misc5.h"
#include "data5.h"
#include "com_main.h"
#include "impl_misc.h"
#include "misc_list.h"

void FreeObject(void *p);

class CObjectBase : public ComObjectBase,
						public INamedDataObject2,
						public CNumeredObjectImpl,
						public CNamedObjectImpl,
						public ISerializableObject,
						public IPersist
{
protected:
	IUnknown *m_punkParent; // Without AddRef
	_list_t2<IUnknown*,FreeObject>	m_listChilds;
	long FindChildPos(IUnknown *punk);
public:
	CObjectBase();
	virtual ~CObjectBase();

	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual HRESULT DoQueryAggregates(const IID &iid, void **ppRet);

	void SetInfo( INamedDataInfo	*pinfo );
public:
	com_call GetType( BSTR *pbstrType );
	com_call GetDataInfo( IUnknown **ppunk );	
	com_call IsModified( BOOL *pbModified );
	com_call SetModifiedFlag( BOOL bSet );
	com_call SetTypeInfo( INamedDataInfo	*pinfo );

	com_call CreateChild( IUnknown **ppunkChild );
	com_call GetParent( IUnknown **ppunkParent );
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ );
	com_call StoreData( DWORD dwDirection/*StoreDataFlags*/ );
//for internal usage - it is called only from framework
	com_call RemoveChild( IUnknown *punkChild );
	com_call AddChild( IUnknown *punkChild );

	com_call GetChildsCount( long *plCount );
	com_call GetFirstChildPosition( long *plPos );
	com_call GetNextChild( long *plPos, IUnknown **ppunkChild );
	com_call GetLastChildPosition(long *plPos);
	com_call GetPrevChild(long *plPos, IUnknown **ppunkChild);

	com_call AttachData( IUnknown *punkNamedData );	//called from NamedData
	com_call GetObjectFlags( DWORD *pdwObjectFlags );

	com_call SetActiveChild( long lPos );
	com_call GetActiveChild( long *plPos );
	com_call SetUsedFlag( BOOL bSet );				//set flag object used as part of another object, doesn't 
														//require to store object in NamedData
	com_call SetHostedFlag( BOOL bSet );			//set flag if object controlled by host
	com_call GetData(IUnknown **ppunkNamedData );

	com_call GetObjectPosInParent( long *plPos ); // return object position in parent's object child list
	com_call SetObjectPosInParent( long lPos ); 

	com_call GetChildPos(IUnknown *punkChild, long *plPos);
	com_call InitAttachedData();
// base and source key functionality
	com_call GetBaseKey(GUID * pBaseKey);
	com_call SetBaseKey(GUID * pBaseKey);
	com_call IsBaseObject(BOOL * pbFlag);

	//ISerializableObject
	com_call Load( IStream *pStream, SerializeParams *pparams );
	com_call Store( IStream *pStream, SerializeParams *pparams );

	//IPersist
	com_call GetClassID( CLSID* pClassID );

	virtual GuidKey GetInternalClassID() = 0;

protected:
	DWORD	m_dwFlags;
	GuidKey	m_keyBase;
	INamedDataInfo	*m_pinfo;
	BOOL		m_bModified;	

	IUnknown*	m_punkNamedData;
	//if object should obtain the features of data storage, it should Aggregate NamedData (Overriding OnCreateAggregates or any other place)
	IUnknown*	m_punkAggrNamedData;
//	void NotifyContexts();
};

// [vanek] 24.12.2003 - support INamedDataInfo2 interface
class CDataInfoBase : public ComObjectBase, 
					public INamedDataInfo2,
					public CNamedObjectImpl
{
	route_unknown();
public:
	CDataInfoBase( const CLSID &clsid, const char *pszTypeName, const char *pszContainerTypeName, UINT nIcon );
	virtual IUnknown *DoGetInterface( const IID &iid );
public:
	com_call CreateObject( IUnknown **ppunk );
	com_call GetName( BSTR *pbstrName );
	com_call GetContainerType( BSTR *pbstrTypeName );
	com_call GetProgID( BSTR *pbstrProgID );
	com_call GetObjectIcon(HICON* phIcon, DWORD dwFlags );
	com_call GetUserName( BSTR *pbstr );
	// INamedDataInfo2
	com_call GetContainerState( DWORD *pdwState );
protected:
	_bstr_t	m_bstrContainer;
	long	m_nIconID;
	CLSID	m_clsid;
	DWORD	m_dwContainerState;
};


class CChildObjectBase : public CObjectBase
{
public:
	CChildObjectBase()
	{
		m_pndoParent = 0;
		m_pos_parent = 0;
	}
protected:
	com_call GetParent( IUnknown **ppunkParent )
	{	
		if( m_pndoParent )
			return m_pndoParent->QueryInterface( IID_IUnknown, (void**)ppunkParent );
		else
			*ppunkParent = 0;
		return S_OK;

	}
	com_call SetParent( IUnknown *punkParent, DWORD dwFlags /*AttachParentFlag*/ )
	{	
		if( m_pndoParent )m_pndoParent->RemoveChild( Unknown() );
		m_pndoParent = 0;
		m_pos_parent = 0;
		if( punkParent )punkParent->QueryInterface( IID_INamedDataObject2, (void**)&m_pndoParent );
		if( m_pndoParent )
		{
			m_pndoParent->AddChild( Unknown() );
			m_pndoParent->Release();
		}
		return S_OK;
	}

	com_call GetObjectPosInParent( long *plPos )
	{	
		*plPos = m_pos_parent;
		return S_OK;
	}
	com_call SetObjectPosInParent( long lPos )
	{	
		m_pos_parent = lPos;
		return S_OK;	
	}

	com_call GetObjectFlags( DWORD *pdwObjectFlags )	
	{
		*pdwObjectFlags=nofStoreByParent;
		return S_OK;
	}

protected:
	INamedDataObject2	*m_pndoParent;
	long				m_pos_parent;
};


#endif // __data_main_h__
