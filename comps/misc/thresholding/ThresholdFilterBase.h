// ThresholdFilterBase.h: interface for the CThresholdFilterBase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THRESHOLDFILTERBASE_H__0A930E73_3E9D_49DB_ACC6_F79D08E2DCC5__INCLUDED_)
#define AFX_THRESHOLDFILTERBASE_H__0A930E73_3E9D_49DB_ACC6_F79D08E2DCC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "image5.h"
#include "action_filter.h"
#include "resource.h"
#include "morph1.h"

class CThresholdFilterBase : public CFilter  
{
	int		m_nWorkPane;

public:
	color	m_Edge;
	
	com_call DoInvoke();
	virtual bool InvokeFilter();
	virtual color SelectEdge(IImage3* pSrc, long *pHist ,long *pAprHist);
	bool CreateHistogram(IImage3 *pSrc, int nPane, long* pHist ,long *pAprHist);
    virtual bool IsWorksWithGreyscaleOnly( ) { return true; }
	
	bool	TestImage( IImage3 *pimage );
	int		GetWorkPane( ) { return m_nWorkPane; }

	CThresholdFilterBase();
	virtual ~CThresholdFilterBase();
protected:
//	CMorphImage	*m_pnew;
};

#endif // !defined(AFX_THRESHOLDFILTERBASE_H__0A930E73_3E9D_49DB_ACC6_F79D08E2DCC5__INCLUDED_)
