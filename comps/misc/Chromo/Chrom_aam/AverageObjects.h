#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"


#if !defined(AFX_AverageObjects_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_AverageObjects_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAverageObjectsFilter : public CFilter,
public _dyncreate_t<CAverageObjectsFilter>
{
public:
	route_unknown();
public:
	CAverageObjectsFilter();
	virtual ~CAverageObjectsFilter();
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
	virtual bool IsAvaible();
protected:
	com_call GetActionState( DWORD *pdwState );
	com_call DoInvoke();
	com_call DoUndo();
	com_call DoRedo();
private:
	IUnknownPtr m_ActivateMe;
	IUnknownPtr m_PrevActive;
};




/////////////////////////////////////////////////////////////////////////////
class CAverageObjectsInfo : public _ainfo_t<ID_ACTION_AVERAGEOBJECTS, _dyncreate_t<CAverageObjectsFilter>::CreateObject, 0>,
public _dyncreate_t<CAverageObjectsInfo>
{
        route_unknown();
public:
        CAverageObjectsInfo()
        {
        }
        arg     *args() {return s_pargs;}
        virtual _bstr_t target() {return "anydoc";}
        static arg      s_pargs[];
};

#endif // !defined(AFX_AverageObjects_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
