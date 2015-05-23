#pragma once

#include "action_misc.h"
#include "resource.h"

namespace STAT_DEPENDENCE
{
namespace STAT_DEPENDENCE_ACTIONS
{
/*************************************************************************/
class show_stat_dependence : public CActionShowView,
						public _dyncreate_t<show_stat_dependence>
{
public:
	route_unknown();	
public:
	virtual _bstr_t GetViewProgID();
};
/////////////////////////////////////////////////////////////////////////////
class show_stat_dependence_info : public _ainfo_t<IDS_SHOW_STAT_TABLE_DEPENDENCE, _dyncreate_t<show_stat_dependence>::CreateObject, 0>,
							public _dyncreate_t<show_stat_dependence_info>
{
	route_unknown();
public:
	show_stat_dependence_info(){}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetFrame; }	
};
/*************************************************************************/
};
};