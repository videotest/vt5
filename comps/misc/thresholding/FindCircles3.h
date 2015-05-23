#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CFindCircles3 : public CFilter,
					public _dyncreate_t<CFindCircles3>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool CanDeleteArgument( CFilterArgument *pa );
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
	IImage3Ptr m_sptrSrc; // входное изображение
	INamedDataObject2Ptr m_sptrList; // выход - список объектов

	color** m_ppSrc; // входные данные
	int m_cx, m_cy; // размеры изображения

	const int static m_na=16;
	const int static m_nr=16;
	double m_co[m_na], m_si[m_na];
	double m_rr[m_nr];
	int m_dx[m_na*m_nr], m_dy[m_na*m_nr];
	
protected:
};

//////////////////////////////////////////////////////////////////////

class CFindCircles3Info : public _ainfo_t<ID_ACTION_FINDCIRCLES3, _dyncreate_t<CFindCircles3>::CreateObject, 0>,
							public _dyncreate_t<CFindCircles3Info>
{
	route_unknown();
public:
	CFindCircles3Info()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


