#ifndef __apportionment_actions_h__
#define __apportionment_actions_h__

#include "resource.h"
#include "action_misc.h"
#include "nameconsts.h"
#include "apportionment.h"


/////////////////////////////////////////////////////////////////////////////
//CActionShowApportionment
class CActionShowApportionment : public CActionShowView,
								 public _dyncreate_t<CActionShowApportionment>
{
public:
	route_unknown();
public:
	CActionShowApportionment(){}
	//com_call DoInvoke();
	virtual _bstr_t GetViewProgID()
	{		return _bstr_t( szAViewProgID ); 	}
};

class CActionShowApportionmentInfo : public _ainfo_t<IDS_SHOW_APPORTIONMENT, _dyncreate_t<CActionShowApportionment>::CreateObject, 0>,
									 public _dyncreate_t<CActionShowApportionmentInfo>
{
	route_unknown();
public:
	CActionShowApportionmentInfo(){}
	arg	*args()	{return NULL;}
	virtual _bstr_t target()			{return szTargetFrame;}	
};

/////////////////////////////////////////////////////////////////////////////
//CActionClassesArrange
class CActionClassesArrange : public CAction,
							  public _dyncreate_t<CActionClassesArrange>
{
public:
	route_unknown();
public:
	CActionClassesArrange(){}
	com_call DoInvoke();
};

class CActionClassesArrangeInfo : public _ainfo_t<IDS_CLASSES_ARRANGE, _dyncreate_t<CActionClassesArrange>::CreateObject, 0>,
								  public _dyncreate_t<CActionClassesArrangeInfo>
{
	route_unknown();
public:
	CActionClassesArrangeInfo(){}
	arg	*args()		{return s_pargs;}
	virtual _bstr_t target()			{return szTargetAnydoc;}	

	static arg	s_pargs[];
};



#endif