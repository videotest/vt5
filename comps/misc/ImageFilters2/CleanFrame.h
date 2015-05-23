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
class CCleanFrame : public CFilter,
                      public _dyncreate_t<CCleanFrame>
{
public:
        route_unknown();
public:
	virtual bool InvokeFilter();
protected:
};


/////////////////////////////////////////////////////////////////////////////
class CCleanFrameInfo : public _ainfo_t<ID_ACTION_CLEANFRAME, _dyncreate_t<CCleanFrame>::CreateObject, 0>,
                                                        public _dyncreate_t<CCleanFrameInfo>
{
  route_unknown();
public:
  CCleanFrameInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};
