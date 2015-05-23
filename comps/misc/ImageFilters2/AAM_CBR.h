// AamCbrFilter.h: interface for the CCloseByReconstructionFilter class.
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


#if !defined(AFX_AAMCBRFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_AAMCBRFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class CAamCbrFilter : public CFilterMorphoBase,
                                        public _dyncreate_t<CAamCbrFilter>
{
public:
        route_unknown();
public:
	bool CloseByReconstructionAAM3(int nMaskType, int nMaskSize, IImage2 *pSource, IImage2 *pResult);
	bool ReconstructionForClose(IImage2 *pSource, IImage2 *pResult);
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()                        {return false;};
protected:
};




// AamCbrInfo.h: interface for the CAamCbrInfo class.
//
//////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////
class CAamCbrInfo : public _ainfo_t<ID_ACTION_CLOSE_BY_RECONSTRUCTION_AAM, _dyncreate_t<CAamCbrFilter>::CreateObject, 0>,
                                                        public _dyncreate_t<CAamCbrInfo>
{
  route_unknown();
public:
  CAamCbrInfo()
  {
  }
  arg *args() {return s_pargs;}
  virtual _bstr_t target()  {return "anydoc";}
  static arg s_pargs[];
};

#endif // !defined(AFX_AAMCBRFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
