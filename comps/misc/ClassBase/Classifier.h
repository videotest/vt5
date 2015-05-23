#pragma once

#include "classifycontainerimpl.h"

class CManualClassifier : public ComObjectBase, public CClassifyContainerImpl
{
public:
	CManualClassifier(void);
	virtual ~CManualClassifier(void);

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );
};

class CAutomaticClassifier : public ComObjectBase, public CClassifyContainerImpl
{
	long m_lDivisionType;
	double m_fDivisionValue;

	bool m_bUseRange;
	double m_fMinVal;
	double m_fMaxVal;
	CString m_strFileName;
public:
	CAutomaticClassifier(void);
	virtual ~CAutomaticClassifier(void);

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	com_call Process( /*[in]*/ IUnknown *punkObject );
	com_call Load( /*[in]*/ BSTR bstrFile );
	com_call Store( /*[in]*/ BSTR bstrFile );
	com_call Finish();

	// IClassifyInfoProxy // vanek - 31.10.2003	
	com_call GetObjectDescrByClass( /*[in]*/ IUnknown *punkObject, /*[in]*/ long lClass, /*[out]*/ BSTR *pbstrDescription );
};
