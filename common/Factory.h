#ifndef __GUARD_FACTORY_H
#define __GUARD_FACTORY_H


#include "Defs.h"
#include "GuardInt.h"

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

	static DWORD creation_func(DWORD dwParam = 0);

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



#define GUARD_BEGIN_OLEFACTORY(class_name) 									\
protected: 																	\
	class class_name##Factory : public CVTFactory 							\
	{ 																		\
	public:																	\
		class_name##Factory(REFCLSID clsid, CRuntimeClass* pRuntimeClass, 	\
			BOOL bMultiInstance, LPCTSTR lpszProgID) : 						\
				CVTFactory(clsid, pRuntimeClass, bMultiInstance, 			\
				lpszProgID) {}												


#define GUARD_END_OLEFACTORY(class_name) 									\
	}; 																		\
	friend class class_name##Factory; 										\
	static AFX_DATA class_name##Factory factory; 							\
public: 																	\
	static AFX_DATA const GUID guid; 										


#define GUARD_END_OLEFACTORY_PROGID(class_name) 							\
	}; 																		\
	friend class class_name##Factory; 										\
	static AFX_DATA class_name##Factory factory; 							\
public: 																	\
	static AFX_DATA const GUID guid; 										\
	virtual CString	GetProgID();  											


#define GUARD_DECLARE_OLECREATE(class_name)									\
	GUARD_BEGIN_OLEFACTORY(class_name) 										\
	GUARD_END_OLEFACTORY(class_name)


#define GUARD_DECLARE_OLECREATE_PROGID(class_name)							\
	GUARD_BEGIN_OLEFACTORY(class_name)										\
	GUARD_END_OLEFACTORY_PROGID(class_name)


#define GUARD_IMPLEMENT_OLECREATE(class_name, external_name,				\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)						\
	const AFX_DATADEF GUID class_name::guid =								\
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; 					\
	AFX_DATADEF class_name::class_name##Factory class_name::factory(		\
		class_name::guid, RUNTIME_CLASS(class_name), FALSE,					\
		external_name);											

#define GUARD_IMPLEMENT_OLECREATE_SHORT(class_name, external_name, clsid)	\
	const AFX_DATADEF GUID class_name::guid = clsid;						\
	AFX_DATADEF class_name::class_name##Factory class_name::factory(		\
		class_name::guid, RUNTIME_CLASS(class_name), FALSE,					\
		external_name);													

#define GUARD_IMPLEMENT_OLECREATE_PROGID(class_name, external_name,			\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)						\
	const AFX_DATADEF GUID class_name::guid = 								\
		{ l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } }; 					\
	AFX_DATADEF class_name::class_name##Factory class_name::factory( 		\
		class_name::guid, RUNTIME_CLASS(class_name), FALSE,					\
		external_name);														\
	CString class_name::GetProgID() 										\
	{	return external_name;	} 

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
	GUARD_BEGIN_OLEFACTORY(class_name) 												\
		virtual BOOL UpdateRegistry(BOOL);											\
	GUARD_END_OLEFACTORY(class_name)												\
public:																				\
	virtual HRESULT GetClassID(LPCLSID pclsid); 



#define GUARD_IMPLEMENT_OLECREATE_CTRL(class_name, external_name,					\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8)								\
		GUARD_IMPLEMENT_OLECREATE(class_name, external_name, 						\
			l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) 								\
	HRESULT class_name::GetClassID(LPCLSID pclsid)									\
	{ return factory.GetClassID(pclsid); }




#endif// __GUARD_FACTORY_H
