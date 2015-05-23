
#pragma once

#include "action_filter.h"
#include "binimageint.h"
#include "resource.h"

class CBinThresholdPhase : public CFilter, public _dyncreate_t<CBinThresholdPhase>
{
public:
	route_unknown();
public:
	
	CBinThresholdPhase();
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument(CFilterArgument *pa) {return false;}
};


class CBinThresholdPhaseInfo : public _ainfo_t<ID_ACTION_THRESHOLD_PHASE, _dyncreate_t<CBinThresholdPhase>::CreateObject, 0>,
	public _dyncreate_t<CBinThresholdPhaseInfo>
{
	route_unknown();
public:
	CBinThresholdPhaseInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CSwitchPhase : public CAction, public _dyncreate_t<CSwitchPhase>
{
public:
	route_unknown();
public:

	CSwitchPhase();
	com_call DoInvoke();
};

class CSwitchPhaseInfo : public _ainfo_t<ID_ACTION_SWITCH_PHASE, _dyncreate_t<CSwitchPhase>::CreateObject, 0>,
	public _dyncreate_t<CSwitchPhaseInfo>
{
	route_unknown();
public:
	CSwitchPhaseInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};

class CNewPhase : public CAction, CUndoneableActionImpl, public _dyncreate_t<CNewPhase>
{
public:
	route_unknown();
protected:
	virtual IUnknown *DoGetInterface(const IID &iid);
public:
	CNewPhase();
	com_call DoInvoke();
};

class CNewPhaseInfo : public _ainfo_t<ID_ACTION_NEW_PHASE, _dyncreate_t<CNewPhase>::CreateObject, 0>,
	public _dyncreate_t<CNewPhaseInfo>
{
	route_unknown();
public:
	CNewPhaseInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};


class CSleep : public CAction, public _dyncreate_t<CSleep>
{
public:
	route_unknown();
public:

	CSleep();
	com_call DoInvoke();
};

class CSleepInfo : public _ainfo_t<ID_ACTION_SLEEP, _dyncreate_t<CSleep>::CreateObject, 0>,
	public _dyncreate_t<CSleepInfo>
{
	route_unknown();
public:
	CSleepInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "app";}
	static arg	s_pargs[];
};

class CUpdateWindow : public CAction, public _dyncreate_t<CUpdateWindow>
{
public:
	route_unknown();
public:

	CUpdateWindow();
	com_call DoInvoke();
};

class CUpdateWindowInfo : public _ainfo_t<ID_ACTION_UPDATE_WINDOW, _dyncreate_t<CUpdateWindow>::CreateObject, 0>,
	public _dyncreate_t<CUpdateWindowInfo>
{
	route_unknown();
public:
	CUpdateWindowInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};


