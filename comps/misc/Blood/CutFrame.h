#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

class CCutFrame : public CAction,
	public CUndoneableActionImpl,
	public CLongOperationImpl,
	public _dyncreate_t<CCutFrame>

{
public:
	route_unknown();
	CCutFrame(void);
	virtual ~CCutFrame(void);
	com_call DoInvoke();
private:
	IUnknown *GetContextObject(_bstr_t bstrName, _bstr_t bstrType);
	virtual IUnknown *DoGetInterface( const IID &iid );
};

//---------------------------------------------------------------------------
class CCutFrameInfo : public _ainfo_t<ID_ACTION_CUTFRAME, _dyncreate_t<CCutFrame>::CreateObject, 0>,
public _dyncreate_t<CCutFrameInfo>
{
        route_unknown();
public:
        CCutFrameInfo()  {   }
        arg *args() {return s_pargs;}
        virtual _bstr_t target()  {return "anydoc";}
        static arg s_pargs[];
};
