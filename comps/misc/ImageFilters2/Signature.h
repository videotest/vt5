#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

class CSignatureFilter :	public CFilter,
							public _dyncreate_t<CSignatureFilter>
{
public:
	route_unknown();

	CSignatureFilter(void);
	~CSignatureFilter(void);

	virtual bool InvokeFilter();
};
/////////////////////////////////////////////////////////////////////////////

class CSignatureFilterInfo : public _ainfo_t< ID_ACTION_SIGNATURE, _dyncreate_t<CSignatureFilter>::CreateObject, 0>,
                                                        public _dyncreate_t<CSignatureFilterInfo>
{
  route_unknown();
public:
  CSignatureFilterInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};
