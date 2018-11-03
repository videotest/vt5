#ifndef __FACTORY_NOGUARD_H
#define __FACTORY_NOGUARD_H


class std_dll CRuntimeInfoPatch
{
public:
	CRuntimeInfoPatch( CRuntimeClass* pRuntimeClass )
	{
		if( !pRuntimeClass->m_pfnGetBaseClass )
		{
			#if _MSC_VER >= 1300
			pRuntimeClass->m_pfnGetBaseClass = &CCmdTarget::GetThisClass;
			#endif
		}
	}
};

class std_dll CVTFactory : public CRuntimeInfoPatch, public COleObjectFactory
{
	DECLARE_DYNAMIC(CVTFactory);
public:
	CVTFactory(REFCLSID clsid, CRuntimeClass* pRuntimeClass, BOOL bMultiInstance, LPCTSTR lpszProgID);

//	virtual BOOL Register();
	virtual CCmdTarget* OnCreateObject();
	virtual BOOL UpdateRegistry(BOOL);
	virtual void LoadInfo();

	BEGIN_INTERFACE_PART(VTClass, IVTClass)
		com_call VTCreateObject(LPUNKNOWN, LPUNKNOWN, REFIID, BSTR, LPVOID*);
		com_call VTRegister(BOOL bRegister);
		com_call VTGetProgID(BSTR * pbstrProgID);
	END_INTERFACE_PART(VTClass)

	DECLARE_INTERFACE_MAP()

public:	
	virtual CString GetProgID();
	REFCLSID GetClassID() const;
	virtual HRESULT GetClassID(LPCLSID pclsid)
	{	*pclsid = GetClassID(); return NOERROR;	}

	REFCLSID GetExternClassID() const;

	static DWORD_PTR creation_func(DWORD_PTR dwParam = 0);

protected:
	BOOL RegisterThis();
	BOOL UnregisterThis();
	BOOL UpdateRegistryCtrl(BOOL bRegister, HINSTANCE nInstance, UINT idTypeName, UINT idBitmap, 
							int nRegFlags, DWORD dwMiscStatus, REFGUID tlid, WORD wVerMajor, WORD wVerMinor);

	BOOL UpdateRegistryPage(BOOL bRegister, HINSTANCE nInstance, UINT idTypeName);


protected:
	GuidKey	m_guidExtern;
};

////////////////////////////////////////////////////////////////////////////////
//	No Guard factory
//
class std_dll CNoGuardFactory : public CRuntimeInfoPatch, public COleObjectFactory
{
	DECLARE_DYNAMIC(CNoGuardFactory);
public:
	CNoGuardFactory(REFCLSID clsid, CRuntimeClass* pRuntimeClass, BOOL bMultiInstance, LPCTSTR lpszProgID);
};

#define NOGUARD_DECLARE_OLECREATE(class_name) \
public: \
	static CNoGuardFactory factory; \
	static const GUID guid; \

#define NOGUARD_IMPLEMENT_OLECREATE(class_name, external_name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	CNoGuardFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), FALSE, _T(external_name)); \
	AFX_COMDAT const GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \

////////////////////////////////////////////////////////////////////////////////


/*
#define BEGIN_OLEFACTORY(class_name)										\
protected:																	\
	class class_name##Factory : public COleObjectFactoryEx					\
	{																		\
	public:																	\
		class_name##Factory(REFCLSID clsid, CRuntimeClass* pRuntimeClass,	\
			BOOL bMultiInstance, LPCTSTR lpszProgID) :						\
				COleObjectFactoryEx(clsid, pRuntimeClass, bMultiInstance,	\
				lpszProgID) {}												
		virtual BOOL UpdateRegistry(BOOL);
*/

#define GUARD_BEGIN_OLEFACTORY(class_name) 									\
	BEGIN_OLEFACTORY(class_name)

/*
#define END_OLEFACTORY(class_name) \
	}; \
	friend class class_name##Factory; \
	static class_name##Factory factory; \
public: \
	static const GUID guid; \
	virtual HRESULT GetClassID(LPCLSID pclsid);
*/
#define GUARD_END_OLEFACTORY(class_name) 									\
	END_OLEFACTORY(class_name)

#define GUARD_END_OLEFACTORY_PROGID(class_name) 							\
	}; 																		\
	friend class class_name##Factory; 										\
	static AFX_DATA class_name##Factory factory; 							\
public: 																	\
	static AFX_DATA const GUID guid; 										\
	virtual CString	GetProgID();  											


/*
#define DECLARE_OLECREATE(class_name) \
public: \
	static COleObjectFactory factory; \
	static const GUID guid; \
*/
#define GUARD_DECLARE_OLECREATE(class_name)									\
	DECLARE_OLECREATE(class_name)

#define GUARD_DECLARE_OLECREATE_PROGID(class_name)							\
	DECLARE_OLECREATE(class_name)

/*
#define IMPLEMENT_OLECREATE(class_name, external_name,						\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
	COleObjectFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), FALSE, _T(external_name)); \
	AFX_COMDAT const GUID class_name::guid = \
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; \

*/
#define GUARD_IMPLEMENT_OLECREATE(class_name, external_name,				\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)						\
	IMPLEMENT_OLECREATE(class_name, external_name,							\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

#define IMPLEMENT_OLECREATE_SHORT(class_name, external_name, clsid)	\
	AFX_COMDAT const GUID class_name::guid = clsid;						\
	COleObjectFactory class_name::factory(class_name::guid, \
		RUNTIME_CLASS(class_name), FALSE, _T(external_name)); 

#define GUARD_IMPLEMENT_OLECREATE_SHORT(class_name, external_name, clsid)	\
	IMPLEMENT_OLECREATE_SHORT(class_name, external_name, clsid)	


#define GUARD_IMPLEMENT_OLECREATE_PROGID(class_name, external_name,			\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)						\
	IMPLEMENT_OLECREATE(class_name, external_name,							\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)

#define GUARD_IMPLEMENT_OLECREATE_PROGID_SHORT(class_name, external_name, clsid) 	\
	const AFX_DATADEF GUID class_name::guid = clsid; 								\
	AFX_DATADEF class_name::class_name##Factory class_name::factory( 				\
			class_name::guid, RUNTIME_CLASS(class_name), FALSE,						\
			external_name);														 	\
	CString class_name::GetProgID()													\
	{	return external_name;	}



/////////////////////////////////////////////////////////////////////////////
// Macros for ActiveX controls

#define GUARD_DECLARE_OLECREATE_CTRL(class_name) 									\
	BEGIN_OLEFACTORY(class_name) 													\
	END_OLEFACTORY(class_name)														


#define IMPLEMENT_OLECREATE_CTRL(class_name, external_name,							\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)								\
	IMPLEMENT_OLECREATE_EX(class_name, external_name, 								\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) 								

//	HRESULT class_name::GetClassID(LPCLSID pclsid)	

//	{ return factory.GetClassID(pclsid); }

#define GUARD_IMPLEMENT_OLECREATE_CTRL(class_name, external_name,					\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)								\
	IMPLEMENT_OLECREATE_CTRL(class_name, external_name,								\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8);


#endif// __FACTORY_NOGUARD_H
