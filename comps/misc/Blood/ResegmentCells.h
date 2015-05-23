#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


/////////////////////////////////////////////////////////////////////////////
class CResegmentCells : public CFilter,
                      public _dyncreate_t<CResegmentCells>
{
public:
        route_unknown();
public:
	virtual bool InvokeFilter();
protected:
};


/////////////////////////////////////////////////////////////////////////////
class CResegmentCellsInfo : public _ainfo_t<ID_ACTION_RESEGMENTCELLS, _dyncreate_t<CResegmentCells>::CreateObject, 0>,
                                                        public _dyncreate_t<CResegmentCellsInfo>
{
  route_unknown();
public:
  CResegmentCellsInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};
