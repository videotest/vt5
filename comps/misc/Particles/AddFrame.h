#ifndef __AddFrame_h__
#define __AddFrame_h__

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
#include "Named2DArrays.h"
#include "trajectory.h"


/////////////////////////////////////////////////////////////////////////////

class CParticlesAddFrame :
public CAction,
public CNamed2DArrays,
public _dyncreate_t<CParticlesAddFrame>
{

public:
	route_unknown();
	CParticlesAddFrame();
	~CParticlesAddFrame();
	
	com_call DoInvoke();
private:
	IUnknown *GetContextObject(_bstr_t cName, _bstr_t cType);
}; 

//---------------------------------------------------------------------------
class CParticlesAddFrameInfo : public _ainfo_t<ID_ACTION_PARTICLESADDFRAME, _dyncreate_t<CParticlesAddFrame>::CreateObject, 0>,
public _dyncreate_t<CParticlesAddFrameInfo>
{
	route_unknown();
public:
	CParticlesAddFrameInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


#endif //__AddFrame_h__
