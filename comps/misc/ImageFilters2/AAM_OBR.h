// AamObrFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#if !defined(AFX_AAMOBRFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_AAMOBRFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAamObrFilter : public CFilterMorphoBase,
                                        public _dyncreate_t<CAamObrFilter>
{
public:
        route_unknown();
public:
	bool OpenByReconstructionAAM3(int nMaskType, int nMaskSize, IImage2 *pSource, IImage2 *pResult);
	bool ReconstructionForOpen(IImage2 *pSource, IImage2 *pResult);
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()                        {return false;};
protected:
};



#endif // !defined(AFX_AAMOBRFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)

// AamObrInfo.h: interface for the CAamObrInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AAMOBRINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
#define AFX_AAMOBRINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAamObrInfo : public _ainfo_t<ID_ACTION_OPEN_BY_RECONSTRUCTION_AAM, _dyncreate_t<CAamObrFilter>::CreateObject, 0>,
                                                        public _dyncreate_t<CAamObrInfo>
{
  route_unknown();
public:
  CAamObrInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};

#endif // !defined(AFX_AAMOBRINFO_H__B3010FDD_F551_47B2_8EBD_498C045A23AD__INCLUDED_)
