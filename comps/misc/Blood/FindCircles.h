#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
class CFindCircles : public CFilter,
                      public _dyncreate_t<CFindCircles>
{
public:
        route_unknown();
public:
	virtual bool InvokeFilter();
protected:
};


/////////////////////////////////////////////////////////////////////////////
class CFindCirclesInfo : public _ainfo_t<ID_ACTION_FINDCIRCLES, _dyncreate_t<CFindCircles>::CreateObject, 0>,
                                                        public _dyncreate_t<CFindCirclesInfo>
{
  route_unknown();
public:
  CFindCirclesInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};
