// DataBase.h: interface for the CDataRegistrator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATABASE_H__B438FADD_38E7_11D3_87C5_0000B493FF1B__INCLUDED_)
#define AFX_DATABASE_H__B438FADD_38E7_11D3_87C5_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "defs.h"
#include "filebase.h"
#include "cmnbase.h"
#include "ObjectProps.h"
#include "Object5.h"
#include "cmnbase.h"
#include "Factory.h"


#include "clone.h"

// base class for implementing of INamedDataInfo
class std_dll CDataObjectInfoBase : 
	public CCmdTargetEx,
	public CNamedObjectImpl
{
	DECLARE_DYNAMIC(CDataObjectInfoBase);
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();

protected:
	CString __m_szContainerName;
	long    __m_lCounter;
	DWORD	__m_dwContainerState;	// [vanek] 24.12.2003

public:
//	CDataObjectInfoBase();
	CDataObjectInfoBase(LPCTSTR szType, LPCTSTR szContainer);
	~CDataObjectInfoBase();

	virtual HINSTANCE GetHinstance() = 0;
	virtual CRuntimeClass *GetDataObjectRuntime() = 0;
	virtual CString __GetProgID() = 0;
	virtual HICON __GetObjectIcon( DWORD dwFlags );
	virtual const char *GetUserName();
	virtual UINT GetDefIconID()	{return 0;}
	virtual UINT __GetTypeStringID() = 0;

	

public:
	BEGIN_INTERFACE_PART_EXPORT(Info, INamedDataInfo2)
		com_call CreateObject(IUnknown **ppunk);
//		com_call GetName(BSTR *pbstrName);
		com_call GetContainerType(BSTR *pbstrTypeName);
		com_call GetProgID(BSTR *pbstrProgID);
		com_call GetObjectIcon(HICON* phIcon, DWORD dwFlags );
		com_call GetContainerState( DWORD *pdwState );
	END_INTERFACE_PART(Info)
};

// base class for creating Named Data objects
class std_dll  CDataObjectBase : public CCmdTargetEx,
								public CSerializableObjectImpl,
								public CTextObjectImpl,
								public CNamedObjectImpl,
								public CNumeredObjectImpl,
								public CDataObjectPropsImpl,
								public CPersistImpl
{
	DECLARE_DYNAMIC(CDataObjectBase);
	ENABLE_MULTYINTERFACE();
public:
	CDataObjectBase();
	virtual ~CDataObjectBase();

	virtual void OnFinalRelease();
public:
	INamedDataInfo		*m_pNamedDataInfo;
	INamedDataObject2	*m_pParent;


	DECLARE_INTERFACE_MAP()
	DECLARE_DISPATCH_MAP();

	BEGIN_INTERFACE_PART_EXPORT(DataObj, INamedDataObject2)
		com_call GetType( BSTR *pbstrType );
		com_call GetDataInfo( IUnknown **ppunk );
		com_call IsModified( BOOL *pbModified );
		com_call SetModifiedFlag( BOOL bSet );
		com_call SetTypeInfo( INamedDataInfo *pinfo );

//INamedDataObject2
		com_call CreateChild( IUnknown **ppunkChild );
		com_call GetParent( IUnknown **ppunkParent );
		com_call SetParent( IUnknown *punkParent, DWORD dwFlags );
		com_call StoreData( DWORD dwDirection );

		com_call RemoveChild( IUnknown *punkChild );
		com_call AddChild( IUnknown *punkChild );

		
		com_call GetChildsCount( long *plCount );
		com_call GetFirstChildPosition( long *plPos );
		com_call GetNextChild( long *plPos, IUnknown **ppunkChild );
		com_call GetLastChildPosition(long *plPos);
		com_call GetPrevChild(long *plPos, IUnknown **ppunkChild);

		com_call AttachData( IUnknown *punkNamedData );
		com_call GetObjectFlags( DWORD *pdwObjectFlags );

		com_call SetActiveChild( long lPos );
		com_call GetActiveChild( long *plPos );
		com_call SetUsedFlag( BOOL bSet );
		com_call SetHostedFlag( BOOL bSet );
		com_call GetData(IUnknown **ppunkNamedData );

		com_call GetObjectPosInParent( long *plPos );
		com_call SetObjectPosInParent( long lPos ); 
		com_call GetChildPos(IUnknown *punkChild, long *plPos);
		com_call InitAttachedData();

		com_call GetBaseKey(GUID * pBaseKey);
		com_call SetBaseKey(GUID * pBaseKey);
		com_call IsBaseObject(BOOL * pbFlag);

	END_INTERFACE_PART(DataObj)

	
	BEGIN_INTERFACE_PART_EXPORT(CompatibleObject, ICompatibleObject)
		com_call CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );
	END_INTERFACE_PART(CompatibleObject)
protected:
	virtual HRESULT CreateCompatibleObject( IUnknown *punkObjSource, void *pData, unsigned uDataSize );
	

	//class wizard support
	//{{AFX_DISPATCH(CDataObjectBase)
	afx_msg BSTR disp_GetName();
	afx_msg void disp_SetName(LPCTSTR lpszNewValue);
	afx_msg BSTR GetType();
	afx_msg LPDISPATCH disp_GetPrivateNamedData();
	afx_msg BOOL disp_IsVirtual();
	afx_msg void disp_StoreData(long dwDirection);
	afx_msg LPDISPATCH disp_GetParent();	
	afx_msg long disp_GetChildsCount();
	afx_msg void disp_GetFirstChildPos( VARIANT FAR* pvarPos );
	afx_msg void disp_GetLastChildPos( VARIANT FAR* pvarPos );
	afx_msg LPDISPATCH disp_GetNextChild( VARIANT FAR* pvarPos );	
	afx_msg LPDISPATCH disp_GetPrevChild( VARIANT FAR* pvarPos );
	afx_msg BOOL disp_IsEmpty();
	afx_msg BOOL IsPrivateNamedDataExist();
	afx_msg BOOL disp_GetModifiedFlag();
	afx_msg void disp_SetModifiedFlag(BOOL bModified);
	//}}AFX_DISPATCH
	//{{AFX_VIRTUAL(CDataObjectBase)
	//}}AFX_VIRTUAL
	//{{AFX_MSG(CDataObjectBase)
	//}}AFX_MSG

	virtual bool IsModified(){return m_bModified;}
	virtual void SetModifiedFlag( bool bSet = true ){m_bModified = bSet;}

	virtual void Detach();	//called when parent detached
	virtual void Attach();	//called when parent attached

	
	virtual bool ExchangeDataWithParent( DWORD flags ){return true;};	//data exchange between parent and child
	virtual bool SerializeObject( CStreamEx& ar, SerializeParams *pparams );
	virtual bool DoSetActiveChild( POSITION pos ){return false;};
	virtual POSITION DoGetActiveChild(){return 0;};

	virtual DWORD GetNamedObjectFlags();	//return the object flags

	//events
	virtual void OnAddChild( IUnknown *punkNewChild, POSITION pos ){};
	virtual void OnDeleteChild( IUnknown *punkChildToDelete, POSITION pos ){};
	virtual void OnBeforeSerialize( CStreamEx& ar ){};
	virtual void OnAfterSerialize( CStreamEx& ar ){};
	virtual void OnSetParent( IUnknown *punkNewParent ){};

	virtual void InitAggrNamedData();

	virtual void ReplaceObjectKey(GuidKey & rKey);

	virtual bool SetBaseObjectKey(GuidKey & BaseKey);
	virtual GuidKey GetBaseObjectKey();
	virtual bool CanBeBaseObject();
	virtual void OnSetBaseObject(GuidKey & NewKey);

	virtual bool IsObjectEmpty();
protected:
	POSITION	GetFirstChildPosition();
	POSITION	GetLastChildPosition();
	IUnknown	*GetNextChild( POSITION &pos );
	IUnknown	*GetPrevChild(POSITION & pos);

	POSITION		_FindChild( IUnknown *punkChild );
	virtual void	_DetachChildParent();

	void ErrorMsg(UINT nErrorID);

	bool _RemoveChild(IUnknown *punkChild, POSITION pos = 0);
	void _AddChild(IUnknown *punkChild);

protected:
	bool					m_bModified;
	CPtrList				m_listChilds;
	IUnknown *				m_punkNamedData;
	bool					m_bNotifyNamedData;
	bool					m_bUsedInOtherObject;		//flag object used as part 
	bool					m_bHostedByOtherObject;
							//of another object, doesn't require to store object in NamedData
	POSITION				m_PosInParent;
protected:
	//if object should obtain the features of data storage, it should Aggregate NamedData (Overriding OnCreateAggregates or any other place)
	IUnknown *				m_punkAggrNamedData;

	GuidKey					__m_lBaseObjKey;

};


//macros for auto generation NamedDataObjectInfo component
// consist from two part


// define macro for create info class name
#define NDO_INFO_NAME(class_name) __NDOInfo##class_name
#define NDO_NAME_PREFIX "VideoTesT5Object."


#define DATA_OBJECT_INFO_FULL(stringID, class_name, type, container, clsid, iconID)						\
class __NDOInfo##class_name : public CDataObjectInfoBase												\
{																										\
	DECLARE_DYNCREATE(__NDOInfo##class_name);															\
	GUARD_DECLARE_OLECREATE(__NDOInfo##class_name);														\
public:	/* constructor */																				\
	__NDOInfo##class_name() : CDataObjectInfoBase(type, container) {};									\
	/* members */																						\
	virtual HINSTANCE GetHinstance()																	\
	{		return AfxGetApp()->m_hInstance;/*InstanceHandle();*/	}									\
	virtual CRuntimeClass *GetDataObjectRuntime()														\
	{	return RUNTIME_CLASS(class_name);	}															\
	virtual CString __GetProgID()																		\
	{ return __NDOInfo##class_name::GetProgID(); }														\
	virtual UINT __GetTypeStringID()																	\
	{ return stringID; }																				\
	static CString	GetProgID()																			\
	{																									\
		static CString	s; 																				\
		s = CDataRegistrator::_generate_progid(#class_name); 											\
		return s;																						\
	}																									\
	virtual UINT GetDefIconID(){return (UINT)iconID;}													\
	virtual HICON __GetObjectIcon( DWORD dwFlags )														\
	{																									\
		AFX_MANAGE_STATE(AfxGetStaticModuleState()); 													\
		return CDataObjectInfoBase::__GetObjectIcon( dwFlags );											\
	}																									\
																										\
};																										\
/* implementation of defines of DataObjectClassInfo*/													\
IMPLEMENT_DYNCREATE(__NDOInfo##class_name, CDataObjectInfoBase);										\
GUARD_IMPLEMENT_OLECREATE_SHORT(__NDOInfo##class_name, __NDOInfo##class_name::GetProgID(), clsid)

#define DATA_OBJECT_INFO(stringID, class_name, type, clsid, iconID)	DATA_OBJECT_INFO_FULL(stringID, class_name, type, 0, clsid, iconID)


// class for registration of named data object
// must be placed in Dll(Un)RegisterServer
class std_dll CDataRegistrator:public CCompRegistrator
{
public:
	CDataRegistrator();

	void RegisterDataObject(bool bRegister = true);
public:
	static CString _generate_progid( const char *sz )
	{	return CString(NDO_NAME_PREFIX) + sz;	}

	bool IsDataObject(const char *szProgID);
};

/////////////////////////////////////////////////////////////////////////////////////////
//CDataObjectListBase
//the base class for "object list"
class std_dll CDataObjectListBase : public CDataObjectBase
{
	DECLARE_INTERFACE_MAP();
	ENABLE_MULTYINTERFACE();
public:
	CDataObjectListBase();
protected:
	virtual void OnAddChild( IUnknown *punkNewChild, POSITION pos );
	virtual void OnDeleteChild( IUnknown *punkChildToDelete, POSITION pos );
	virtual void OnBeforeSerialize( CStreamEx& ar );
	virtual void OnAfterSerialize( CStreamEx& ar );

	virtual bool DoSetActiveChild( POSITION pos );
	virtual POSITION DoGetActiveChild();

	void UpdateActiveObject();
	void UpdateObject(IUnknown* punkObj);
	void UpdateDeactiveObject();
	void _MoveObject(GuidKey keyFrom, bool bDirection);

	
	BEGIN_INTERFACE_PART_EXPORT(DataObjList, IDataObjectList)
		com_call UpdateObject( IUnknown* punkObj );
		com_call LockObjectUpdate( BOOL bLock );
		com_call GetObjectUpdateLock( BOOL *pbLock );
		com_call MoveObject(GUID keyFrom, BOOL bDirection);
	END_INTERFACE_PART(DataObjList)

	DECLARE_DISPATCH_MAP();

	//class wizard support
	//{{AFX_DISPATCH(CDataObjectListBase)
		afx_msg LPDISPATCH GetActiveObject();
		afx_msg void SetActiveObject(LPDISPATCH newValue);
		afx_msg long GetCount();		
		afx_msg LPDISPATCH GetObject(long nPos);		
		afx_msg long AddObject(LPDISPATCH pdispObject);		
		afx_msg void RemoveObject(long nPos);		
		afx_msg void RemoveObjectByPos(long nPos);
		afx_msg void MakeEmpty();		
		afx_msg void disp_UpdateObject(VARIANT FAR* pvarObj);
	//}}AFX_DISPATCH
	
protected:

	bool		m_bLockUpdate;
	POSITION	m_posActiveObject;

};




#endif // !defined(AFX_DATABASE_H__B438FADD_38E7_11D3_87C5_0000B493FF1B__INCLUDED_)
