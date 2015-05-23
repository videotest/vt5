#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

#pragma once

/////////////////////////////////////////////////////////////////////////////
class CEvenByEdge : public CFilter,
					public _dyncreate_t<CEvenByEdge>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
protected:
	int		m_nWorkPane;
};

//////////////////////////////////////////////////////////////////////

class CEvenByEdgeInfo : public _ainfo_t<ID_ACTION_EVENBYEDGE, _dyncreate_t<CEvenByEdge>::CreateObject, 0>,
							public _dyncreate_t<CEvenByEdgeInfo>
{
	route_unknown();
public:
	CEvenByEdgeInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
