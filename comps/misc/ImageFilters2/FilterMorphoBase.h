// FilterMorphoBase.h: interface for the CFilterMorphoBase class.
//
//////////////////////////////////////////////////////////////////////
#include "image5.h"
//#include "morph1.h"
#include "binimageint.h"
#include "action_filter.h"
#include "ConnectedArea.h"

#if !defined(AFX_FILTERMORPHOBASE_H__676189C4_69EB_472E_BE81_EFAF5C7BA25C__INCLUDED_)
#define AFX_FILTERMORPHOBASE_H__676189C4_69EB_472E_BE81_EFAF5C7BA25C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFilterMorphoBase : public CFilter  
{
public:
	long m_nAreaCounter;
	bool IsPaneEnable(int nPane);
	bool Erode(int nMaskType, int nMaskSize, IImage2* pSource, IImage2* pResult);
	bool Dilate(int nMaskType, int nMaskSize, IImage2* pSource, IImage2* pResult);
	void CreateConnectedArea(int nPane,long* pMap,IImage2* pSource,IImage2* pImageA1,POINT* pPoints,CConnectedArea* areas, int epsilon);
	void CreateConnectedArea2(int nPane,long* pMap,IBinaryImagePtr pSource,color* pImageA1,POINT* pPoints,CConnectedArea* areas, int epsilon);
	void AddFilledPoint(IImage2* pSource, int nPane, long x, long y,  long *pMap, long &nFilledCount, POINT* points, SIZE size, color CurrentColor, long lIndex, int nEpsilon);
	void AddFilledPoint2(IBinaryImagePtr pSource, int nPane, long x, long y,  long *pMap, long &nFilledCount, POINT* points, SIZE size, color CurrentColor, long lIndex, int nEpsilon);
	bool OpenByReconstruction(int nMaskType,int nMaskSize, int epsilon, IImage2* pSource,IImage2* pResult);
	bool CloseByReconstruction(int nMaskType,int nMaskSize, int epsilon, IImage2* pSource,IImage2* pResult);
	CFilterMorphoBase();
	virtual ~CFilterMorphoBase();

};

#endif // !defined(AFX_FILTERMORPHOBASE_H__676189C4_69EB_472E_BE81_EFAF5C7BA25C__INCLUDED_)
