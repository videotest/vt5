#ifndef __ChromSegm_h__
#define __ChromSegm_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

/////////////////////////////////////////////////////////////////////////////

#define CSize SIZE

class CChromSegm : public CFilter,
public _dyncreate_t<CChromSegm>
{
public:
	route_unknown();
	CChromSegm();
	~CChromSegm();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa )
	{	
		if(pa->m_bInArgument)
		{
			return false;
		}
		
		return CFilter::CanDeleteArgument(pa);
	};
	
protected:
	void LoadIni();
	void FirstPass(color **src, byte **bin, int cx, int cy, double bg_tolerance);
	void FirstPass2(color **src, byte **bin, int cx, int cy, bool invert_ratio);
	void SecondPass(color **src, byte **bin, int cx, int cy);
	void SecondPass2(color **src, byte **bin, int cx, int cy);
	void SecondPass3(color **src, byte **bin, int cx, int cy, int sqr_size);
	int m_back, m_back_width;
	int m_fore, m_fore_width;
}; 

class CChromSegmSecond : public CChromSegm,
public _dyncreate_t<CChromSegmSecond>
{
	
public:
	route_unknown();
	CChromSegmSecond();
	~CChromSegmSecond();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );
}; 


class CStrangeAverage : public CChromSegm,
public _dyncreate_t<CStrangeAverage>
{
	
public:
	route_unknown();
	CStrangeAverage();
	~CStrangeAverage();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );
}; 

class CBoundDistance : public CChromSegm,
public _dyncreate_t<CBoundDistance>
{
	
public:
	route_unknown();
	CBoundDistance();
	~CBoundDistance();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );
}; 

class CAutoAdjustBin : public CChromSegm,
public _dyncreate_t<CAutoAdjustBin>
{
	
public:
	route_unknown();
	CAutoAdjustBin();
	~CAutoAdjustBin();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );
}; 



//---------------------------------------------------------------------------
class CChromSegmInfo : 
public _ainfo_t<ID_ACTION_CHROMSEGM, _dyncreate_t<CChromSegm>::CreateObject, 0>,
public _dyncreate_t<CChromSegmInfo>
{
	route_unknown();
public:
	CChromSegmInfo()
	{
	}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


//---------------------------------------------------------------------------
class CChromSegmSecondInfo : 
public _ainfo_t<ID_ACTION_CHROMSEGMSECOND, _dyncreate_t<CChromSegmSecond>::CreateObject, 0>,
public _dyncreate_t<CChromSegmSecondInfo>
{
	route_unknown();
public:
	CChromSegmSecondInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


//---------------------------------------------------------------------------
class CStrangeAverageInfo : 
public _ainfo_t<ID_ACTION_STRANGEAVERAGE, _dyncreate_t<CStrangeAverage>::CreateObject, 0>,
public _dyncreate_t<CStrangeAverageInfo>
{
	route_unknown();
public:
	CStrangeAverageInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

//---------------------------------------------------------------------------
class CBoundDistanceInfo : 
public _ainfo_t<ID_ACTION_BOUNDDISTANCE, _dyncreate_t<CBoundDistance>::CreateObject, 0>,
public _dyncreate_t<CBoundDistanceInfo>
{
	route_unknown();
public:
	CBoundDistanceInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

//---------------------------------------------------------------------------
class CAutoAdjustBinInfo : 
public _ainfo_t<ID_ACTION_AUTOADJUSTBIN, _dyncreate_t<CAutoAdjustBin>::CreateObject, 0>,
public _dyncreate_t<CAutoAdjustBinInfo>
{
	route_unknown();
public:
	CAutoAdjustBinInfo() {}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


#endif //__ChromSegm_h__
