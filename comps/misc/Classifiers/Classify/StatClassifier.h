#pragma once

#include "classify_int.h"
#include "mahalanobis.h"
#include "StatFile.h"

#include "ClsArray.h"

class CClassParamsEx : public mahalanobis
{
public:
	void read_class_params(const char *lpszIniFile, FILE *rpt, int nDenverClass = -1);
};

struct CObjectRecord
{
	IUnknown *punk;
	long lClass;
	GuidKey key;
};

class CStatClassifier : public ComObjectBase, public IClassifyProxy2
{
	_bstr_t m_bstrIniFileName;
	int m_nObjects;
	CSimpleList<CObjectRecord> m_ObjectsList;
	CSimpleListIterator<CObjectRecord> m_ObjectsIterator;
public:
	CStatClassifier(void);
	virtual ~CStatClassifier(void);
	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );
	com_call Load( /*[in]*/ BSTR bstrFile );
	com_call Store( /*[in]*/ BSTR bstrFile );
	com_call Start();
	com_call Process( /*[in]*/ IUnknown *punkObject );
	com_call Finish();
	com_call GetObjectClass( /*[in]*/ IUnknown *punkObject, /*out*/ long *plClass );
	com_call GetFlags( /*[out]*/ DWORD *pdwFlag );
	com_call SetFlags( /*[in]*/ DWORD dwFlag );
	com_call Teach();
};
