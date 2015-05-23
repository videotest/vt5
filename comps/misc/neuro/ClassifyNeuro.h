#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "\VT5\comps\misc\ClassBase\ClassifyContainerImpl.h"
#include "misc_str.h"
#include "misc_templ.h"
#include "net.h"

/////////////////////////////////////////////////////////////////////////////
class CClassifyNeuro : public CFilter,
		public _dyncreate_t<CClassifyNeuro>
{
protected:
	//virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
	CClassifyNeuro();
	~CClassifyNeuro();
public:
	virtual bool InvokeFilter();
}; 

//---------------------------------------------------------------------------
class CClassifyNeuroInfo : public _ainfo_t<ID_ACTION_CLASSIFYNEURO, _dyncreate_t<CClassifyNeuro>::CreateObject, 0>,
public _dyncreate_t<CClassifyNeuroInfo>
{
        route_unknown();
public:
	CClassifyNeuroInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

class CTeachNeuro : public CAction,	CLongOperationImpl,
		public _dyncreate_t<CTeachNeuro>
{
protected:
	virtual IUnknown *DoGetInterface( const IID &iid )	
	{
		if( iid == IID_ILongOperation ) return (ILongOperation*)this;
		return CAction::DoGetInterface( iid );
	}
public:
	route_unknown();
	CTeachNeuro();
	~CTeachNeuro();
public:
	com_call DoInvoke();
}; 

//---------------------------------------------------------------------------
class CTeachNeuroInfo : public _ainfo_t<ID_ACTION_TEACHNEURO, _dyncreate_t<CTeachNeuro>::CreateObject, 0>,
public _dyncreate_t<CTeachNeuroInfo>
{
        route_unknown();
public:
	CTeachNeuroInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

//---------------------------------------------------------------------------
class CNeuroClassifier : public ComObjectBase, public CClassifyContainerImpl
{
public:
	CNeuroClassifier(void);
	virtual ~CNeuroClassifier(void);

	route_unknown();
	virtual IUnknown *DoGetInterface( const IID &iid );

	_ptr_t2<long> m_lClassArr;


// IClassifyProxy
	com_call Process( /*[in]*/ IUnknown *punkObject );
	com_call Load( /*[in]*/ BSTR bstrFile );
	com_call Start();
	//com_call Finish();

// IClassifyProxy2
	com_call Teach();

	//то, что должно было быть в базовом классе
	_ptr_t2<long> m_Classes;
	_ptr_t2<long> m_Params;

	CNet m_net;
	_ptr_t2<double> m_in_shift;
	_ptr_t2<double> m_in_scale;
	long m_lOutScale;

	virtual bool RealProcess(
		/*in*/ long nParams, double *pfParams, IUnknown *punkObject,
		/*out*/ long *plClass
		);
	// параметры упорядочены в соответствии с m_Params (0..m_Params.size-1)
	// в *plClass следует вернуть номер класса по m_Classes (0..m_Classes.size-1) или -1
};
