#pragma once
#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
class CSegmSerial : public CFilter,
					public _dyncreate_t<CSegmSerial>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
protected:
	int		m_nWorkPane;
};

//////////////////////////////////////////////////////////////////////

class CSegmSerialInfo : public _ainfo_t<ID_ACTION_SEGMSERIAL, _dyncreate_t<CSegmSerial>::CreateObject, 0>,
							public _dyncreate_t<CSegmSerialInfo>
{
	route_unknown();
public:
	CSegmSerialInfo() {}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

/////////////////////////////////////////////////////////////////////////////
class CSegmSerial2 : public CFilter,
					public _dyncreate_t<CSegmSerial2>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
protected:
	int		m_nWorkPane;
};

//////////////////////////////////////////////////////////////////////

class CSegmSerial2Info : public _ainfo_t<ID_ACTION_SEGMSERIAL2, _dyncreate_t<CSegmSerial2>::CreateObject, 0>,
							public _dyncreate_t<CSegmSerial2Info>
{
	route_unknown();
public:
	CSegmSerial2Info() {}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
