#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"


#if !defined(AFX_AvgChrom_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_AvgChrom_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAvgChromFilter : public CFilter,
public _dyncreate_t<CAvgChromFilter>
{
public:
        route_unknown();
public:
        CAvgChromFilter();
        virtual ~CAvgChromFilter();
        virtual bool InvokeFilter();
};




/////////////////////////////////////////////////////////////////////////////
class CAvgChromInfo : public _ainfo_t<ID_ACTION_AVGCHROM, _dyncreate_t<CAvgChromFilter>::CreateObject, 0>,
public _dyncreate_t<CAvgChromInfo>
{
        route_unknown();
public:
        CAvgChromInfo()
        {
        }
        arg     *args() {return s_pargs;}
        virtual _bstr_t target() {return "anydoc";}
        static arg      s_pargs[];
};

#endif // !defined(AFX_AvgChrom_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
