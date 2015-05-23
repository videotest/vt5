#pragma once

#include "action_filter.h"
#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
class CTopHatFilter : public CFilter,
                      public _dyncreate_t<CTopHatFilter>
{
public:
        route_unknown();
public:
	virtual bool InvokeFilter();
protected:
};


/////////////////////////////////////////////////////////////////////////////
class CTopHatInfo : public _ainfo_t<ID_ACTION_TOPHAT, _dyncreate_t<CTopHatFilter>::CreateObject, 0>,
                                                        public _dyncreate_t<CTopHatInfo>
{
  route_unknown();
public:
  CTopHatInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};
