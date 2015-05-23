#pragma once
#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "misc_math.h"

class CSegmSerial3 : public CFilter,
					public _dyncreate_t<CSegmSerial3>
{
public:
	route_unknown();
public:
	CSegmSerial3(void);
	virtual ~CSegmSerial3(void);
	virtual bool InvokeFilter();
protected:
	int		m_nWorkPane;
};

class CSegmSerial3Info : public _ainfo_t<ID_ACTION_SEGMSERIAL3, _dyncreate_t<CSegmSerial3>::CreateObject, 0>,
							public _dyncreate_t<CSegmSerial3Info>
{
	route_unknown();
public:
	CSegmSerial3Info() {}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
