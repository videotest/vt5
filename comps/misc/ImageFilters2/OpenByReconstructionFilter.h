// OpenByReconstructionFilter.h: interface for the COpenByReconstructionFilter class.
//
//////////////////////////////////////////////////////////////////////
#include "action_filter.h"
#include "FilterMorphoBase.h"

#if !defined(AFX_OPENBYRECONSTRUCTIONFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
#define AFX_OPENBYRECONSTRUCTIONFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
class COpenByReconstructionFilter : public CFilterMorphoBase,
					public _dyncreate_t<COpenByReconstructionFilter>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
//	virtual bool IsAllArgsRequired()			{return false;};
protected:
};



#endif // !defined(AFX_OPENBYRECONSTRUCTIONFILTER_H__CE6E9F95_2085_45C9_90EA_B442BFB1EF56__INCLUDED_)
