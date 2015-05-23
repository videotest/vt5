#pragma once

#include "action_filter.h"
#include "binimageint.h"
#include "resource.h"

class CLinesArray;

class CDelLineBase : public CFilter
{
protected:
	void ConnectLines(CLinesArray &arrLines, IImage3 *pSrc, int cx, int cy, int nStart, int nEnd);
	virtual bool InvokeFilter();
	virtual void MakeResult(IImage3 *pSrc, IImage3 *pBin, int cx, int cy, bool bGS,
		int nDarkLines, int nLineWidth, int nDebug) = 0;
};

class CDelLine : public CDelLineBase, public _dyncreate_t<CDelLine>
{
public:
	route_unknown();
public:
	
	CDelLine();
	virtual void MakeResult(IImage3 *pSrc, IImage3 *pBin, int cx, int cy, bool bGS,
		int nDarkLines, int nLineWidth, int nDebug);
	virtual bool CanDeleteArgument(CFilterArgument *pa) {return false;}
};


class CDelLineInfo : public _ainfo_t<ID_ACTION_DELETE_LINE, _dyncreate_t<CDelLine>::CreateObject, 0>,
	public _dyncreate_t<CDelLineInfo>
{
	route_unknown();
public:
	CDelLineInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};

class CDelLineBin : public CDelLineBase, public _dyncreate_t<CDelLineBin>
{
public:
	route_unknown();
public:
	
	CDelLineBin();
	virtual void MakeResult(IImage3 *pSrc, IImage3 *pBin, int cx, int cy, bool bGS,
		int nDarkLines, int nLineWidth, int nDebug);
	virtual bool CanDeleteArgument(CFilterArgument *pa) {return false;}
};


class CDelLineBinInfo : public _ainfo_t<ID_ACTION_DELETE_LINE_BIN, _dyncreate_t<CDelLineBin>::CreateObject, 0>,
	public _dyncreate_t<CDelLineBinInfo>
{
	route_unknown();
public:
	CDelLineBinInfo()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


