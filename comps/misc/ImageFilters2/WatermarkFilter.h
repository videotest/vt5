#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

class CWatermarkFilter :	public CFilter,
							public _dyncreate_t<CWatermarkFilter>
{
public:
	route_unknown();

	CWatermarkFilter(void);
	~CWatermarkFilter(void);

	virtual bool InvokeFilter();
protected:
	color _inverse_convert_color( color clValue, int x, int y );
	color _convert_color( color clValue, int x, int y );
};
/////////////////////////////////////////////////////////////////////////////

class CWatermarkFilterInfo : public _ainfo_t< ID_ACTION_WATERMARK, _dyncreate_t<CWatermarkFilter>::CreateObject, 0>,
                                                        public _dyncreate_t<CWatermarkFilterInfo>
{
  route_unknown();
public:
  CWatermarkFilterInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};
