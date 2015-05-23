#ifndef __Segmentation_h__
#define __Segmentation_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
#include "image5.h"
#include "docview5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"
#include "ObjectPool.h"

/////////////////////////////////////////////////////////////////////////////

class CSegmSmallObjects : public CAction,
        public _dyncreate_t<CSegmSmallObjects>
{

public:
	route_unknown();
	CSegmSmallObjects();
	~CSegmSmallObjects();
	
	com_call DoInvoke();
private:
	IUnknown *GetContextObject(_bstr_t cName, _bstr_t cType);
	CObjectPool m_ObjectPool;
}; 

//---------------------------------------------------------------------------
class CSegmSmallObjectsInfo : public _ainfo_t<ID_ACTION_SEGMSMALLOBJECTS, _dyncreate_t<CSegmSmallObjects>::CreateObject, 0>,
public _dyncreate_t<CSegmSmallObjectsInfo>
{
	route_unknown();
public:
	CSegmSmallObjectsInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


#endif //__Segmentation_h__
