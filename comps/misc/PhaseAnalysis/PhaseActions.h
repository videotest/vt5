
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

class CBinThresholdAllPhases : public CFilter, public _dyncreate_t<CBinThresholdAllPhases>
{
public:
	route_unknown();
public:
	
	CBinThresholdAllPhases();
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument(CFilterArgument *pa) {return false;}
};


class CBinThresholdAllPhasesInfo : public _ainfo_t<ID_ACTION_THRESHOLD_ALL_PHASES, _dyncreate_t<CBinThresholdAllPhases>::CreateObject, 0>,
	public _dyncreate_t<CBinThresholdAllPhasesInfo>
{
	route_unknown();
public:
	CBinThresholdAllPhasesInfo()
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

class CNewPhasePacked : public CFilter, public _dyncreate_t<CNewPhasePacked>
{
public:
	route_unknown();
public:
	
	CNewPhasePacked();
	virtual bool InvokeFilter();
};


class CNewPhasePackedInfo : public _ainfo_t<ID_ACTION_NEW_PHASE_PACKED, _dyncreate_t<CNewPhasePacked>::CreateObject, 0>,
	public _dyncreate_t<CNewPhasePackedInfo>
{
	route_unknown();
public:
	CNewPhasePackedInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CPackPhase : public CAction, CUndoneableActionImpl, public _dyncreate_t<CPackPhase>
{
public:
	route_unknown();
protected:
	virtual IUnknown *DoGetInterface(const IID &iid);
public:
	CPackPhase();
	com_call DoInvoke();
};

class CPackPhaseInfo : public _ainfo_t<ID_ACTION_PACK_PHASE, _dyncreate_t<CPackPhase>::CreateObject, 0>,
	public _dyncreate_t<CPackPhaseInfo>
{
	route_unknown();
public:
	CPackPhaseInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "view site";}
	static arg	s_pargs[];
};



class CUnpackPhase : public CAction, CUndoneableActionImpl, public _dyncreate_t<CUnpackPhase>
{
public:
	route_unknown();
protected:
	virtual IUnknown *DoGetInterface(const IID &iid);
public:
	CUnpackPhase();
	com_call DoInvoke();
};

class CUnpackPhaseInfo : public _ainfo_t<ID_ACTION_UNPACK_PHASE, _dyncreate_t<CUnpackPhase>::CreateObject, 0>,
	public _dyncreate_t<CUnpackPhaseInfo>
{
	route_unknown();
public:
	CUnpackPhaseInfo()
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


