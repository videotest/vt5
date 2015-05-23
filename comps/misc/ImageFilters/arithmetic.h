#ifndef __arithmetic_h__
#define __arithmetic_h__

#include "action_filter.h"
#include "resource.h"
#include "nameconsts.h"

/////////////////////////////////////////////////////////////////////////////
class CCombineFilterBase : public CFilter
{
public:
	route_unknown();
public:
	CCombineFilterBase();
	virtual bool InvokeFilter();
	virtual bool InitCoefficients()
	{m_k1 = 1;m_k2 = 0;m_add = 0;return true;}
protected:
	double	m_k1, m_k2, m_add;
};


class CMulFilter : public CFilter,
					public _dyncreate_t<CFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
};

class CDivFilter : public CFilter,
					public _dyncreate_t<CFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
};


class CCombineFilter : public CCombineFilterBase,
					public _dyncreate_t<CCombineFilter>
{
public:
	virtual bool InitCoefficients(); 
};

class CAddFilter : public CCombineFilterBase, public _dyncreate_t<CAddFilter>
{
public:
	virtual bool InitCoefficients()
	{m_k1 = .5;m_k2 = .5; m_add = 0;return true;}
};

class CSubFilter : public CCombineFilterBase, public _dyncreate_t<CSubFilter>
{
public:
	virtual bool InitCoefficients()
	{m_k1 = 1;m_k2 = -1; m_add = 0;return true;}
};

class CSubFilterEx : public CCombineFilterBase, public _dyncreate_t<CSubFilter>
{
public:
	virtual bool InitCoefficients()
	{m_k1 = 1;m_k2 = -1; m_add = 0;return true;}
public:
	virtual bool InvokeFilter();
};

class CSubLightFilter : public CCombineFilterBase, public _dyncreate_t<CSubLightFilter>
{
public:
	virtual bool InitCoefficients()
	{m_k1 = 1;m_k2 = -1; m_add = 1;return true;}
};

class CDiffFilter : public CCombineFilterBase, public _dyncreate_t<CDiffFilter>
{
public:
	virtual bool InitCoefficients()
	{m_k1 = 1;m_k2 = -1; m_add = .5;return true;}
};

//infos
/////////////////////////////////////////////////////////////////////////////
class CCombineFilterInfo : public _ainfo_t<ID_ACTION_IMAGECOMBINE, _dyncreate_t<CCombineFilter>::CreateObject, 0>,
							public _dyncreate_t<CCombineFilterInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
class CAddFilterInfo : public _ainfo_t<ID_ACTION_IMAGEADD, _dyncreate_t<CAddFilter>::CreateObject, 0>,
							public _dyncreate_t<CAddFilterInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
class CSubFilterInfo : public _ainfo_t<ID_ACTION_IMAGESUB, _dyncreate_t<CSubFilter>::CreateObject, 0>,
							public _dyncreate_t<CSubFilterInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CSubFilterExInfo : public _ainfo_t<ID_ACTION_IMAGESUB_EX, _dyncreate_t<CSubFilterEx>::CreateObject, 0>,
							public _dyncreate_t<CSubFilterExInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CSubLightFilterInfo : public _ainfo_t<ID_ACTION_IMAGESUB_LIGHT, _dyncreate_t<CSubLightFilter>::CreateObject, 0>,
							public _dyncreate_t<CSubLightFilterInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};
class CDiffFilterInfo : public _ainfo_t<ID_ACTION_IMAGEDIFF, _dyncreate_t<CDiffFilter>::CreateObject, 0>,
							public _dyncreate_t<CDiffFilterInfo>
{
	route_unknown();
public:
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CMulFilterInfo : public _ainfo_t<ID_ACTION_IMAGEMUL, _dyncreate_t<CMulFilter>::CreateObject, 0>,
							public _dyncreate_t<CMulFilterInfo>
{
	route_unknown();
public:
	arg	*args()	
	{
		static arg	args[] = {
		{"mul",		szArgumentTypeDouble, "1", true, false },
		{"img1",	szArgumentTypeImage, 0, true, true },
		{"img2",	szArgumentTypeImage, 0, true, true },
		{"result",	szArgumentTypeImage, 0, false, true },	
		{0, 0, 0, false, false },};
		return args;
	}
	virtual _bstr_t target()			{return "anydoc";}
};

class CDivFilterInfo : public _ainfo_t<ID_ACTION_IMAGEDIV, _dyncreate_t<CDivFilter>::CreateObject, 0>,
							public _dyncreate_t<CDivFilterInfo>
{
	route_unknown();
public:
	arg	*args()	
	{
		static arg	args[] = {
		{"mul",		szArgumentTypeDouble, "1", true, false },
		{"img1",	szArgumentTypeImage, 0, true, true },
		{"img2",	szArgumentTypeImage, 0, true, true },
		{"result",	szArgumentTypeImage, 0, false, true },	
		{0, 0, 0, false, false },};
		return args;
	}
	virtual _bstr_t target()			{return "anydoc";}
};



#endif //__arithmetic_h__