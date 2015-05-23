#ifndef __ClassifyAam_h__
#define __ClassifyAam_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
//#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

/////////////////////////////////////////////////////////////////////////////
class CClassifyAam : public CFilter,
		public _dyncreate_t<CClassifyAam>
{
protected:
	//virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
	CClassifyAam();
	~CClassifyAam();
public:
	virtual bool InvokeFilter();
	//com_call DoInvoke();
	//com_call DoUndo();
	//com_call DoRedo();
	//com_call GetActionState(DWORD *pdwState);
}; 

//---------------------------------------------------------------------------
class CClassifyAamInfo : public _ainfo_t<ID_ACTION_CLASSIFYAAM, _dyncreate_t<CClassifyAam>::CreateObject, 0>,
public _dyncreate_t<CClassifyAamInfo>
{
        route_unknown();
public:
	CClassifyAamInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

/////////////////////////////////////////////////////////////////////////////
class CLearnAam : public CFilter,
		public _dyncreate_t<CLearnAam>
{
protected:
	//virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
	CLearnAam();
	~CLearnAam();
public:
	virtual bool InvokeFilter();
	//com_call DoInvoke();
	//com_call DoUndo();
	//com_call DoRedo();
	//com_call GetActionState(DWORD *pdwState);
}; 

//---------------------------------------------------------------------------
class CLearnAamInfo : public _ainfo_t<ID_ACTION_LEARNAAM, _dyncreate_t<CLearnAam>::CreateObject, 0>,
public _dyncreate_t<CLearnAamInfo>
{
        route_unknown();
public:
	CLearnAamInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

/////////////////////////////////////////////////////////////////////////////
class CLearnLinkAam : public CAction,
		public _dyncreate_t<CLearnLinkAam>
{
protected:
	//virtual IUnknown *DoGetInterface( const IID &iid );
public:
	route_unknown();
	CLearnLinkAam();
	~CLearnLinkAam();
public:
	//virtual bool InvokeFilter();
	com_call DoInvoke();
	//com_call DoUndo();
	//com_call DoRedo();
	//com_call GetActionState(DWORD *pdwState);
}; 

//---------------------------------------------------------------------------
class CLearnLinkAamInfo : public _ainfo_t<ID_ACTION_LEARNLINKAAM, _dyncreate_t<CLearnLinkAam>::CreateObject, 0>,
public _dyncreate_t<CLearnLinkAamInfo>
{
        route_unknown();
public:
	CLearnLinkAamInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


#endif //__ClassifyAam_h__
