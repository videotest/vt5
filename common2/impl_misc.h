#ifndef __impl_misc_h__
#define __impl_misc_h__

#include "defs.h"
#include "misc5.h"


class CNumeredObjectImpl : public INumeredObject
{
public:
	CNumeredObjectImpl();

	com_call AssignKey( GUID Key );
	com_call GetKey( GUID *pKey );
	com_call GenerateNewKey();
	com_call GetCreateNum( long *plNum );
	com_call GenerateNewKey( GUID * pKey );
protected:
	long	m_nCreateNo;
	GuidKey	m_key;
};

class CNamedObjectImpl : public INamedObject2
{
public:
	CNamedObjectImpl();
public:
	com_call GetName( BSTR *pbstr );
	com_call GetUserName( BSTR *pbstr );
	com_call SetName( BSTR bstr );
	com_call SetUserName( BSTR bstr );
	com_call GenerateUniqueName( BSTR bstrBase );
protected:
	_bstr_t	m_bstrUserName;
	_bstr_t	m_bstrName;
};

#endif //__impl_misc_h__