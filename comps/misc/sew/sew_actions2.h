#pragma once

#include "sew.h"
#include "resource.h"
#include "action_filter.h"
#include "action_interactive.h"
#include "image5.h"


class CAutoSew2Filter : public CFilter, public _dyncreate_t<CAutoSew2Filter>
{
	route_unknown();
	int m_nMinOvr,m_nStages,m_nFoundOpt;
	POINT GradientDecent(IImage3 *pimgRes, IImage3 *pimgBase, IImage3 *pimgSew);
	void MakeNotification(int nStage, int nNow, int nTotal);
public:
	CAutoSew2Filter();
	virtual bool InvokeFilter();
	com_call CanDeleteArgument( IUnknown *punk, BOOL *pbCan );
	com_call GetActionState(DWORD *pdwState);
};


class CAutoSew2FilterInfo : public _ainfo_t<ID_ACTION_AUTOSEW2, _dyncreate_t<CAutoSew2Filter>::CreateObject, 0>,
							public _dyncreate_t<CAutoSew2FilterInfo>
{
	route_unknown();
public:
	CAutoSew2FilterInfo() {}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];

};
