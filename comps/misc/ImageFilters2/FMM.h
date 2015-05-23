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
class CDilateOnImage : public CFilter,
                      public _dyncreate_t<CDilateOnImage>
{
public:
        route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa );
};


/////////////////////////////////////////////////////////////////////////////
class CDilateOnImageInfo : public _ainfo_t<ID_ACTION_DILATEONIMAGE, _dyncreate_t<CDilateOnImage>::CreateObject, 0>,
                                                        public _dyncreate_t<CDilateOnImageInfo>
{
  route_unknown();
public:
  CDilateOnImageInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};
