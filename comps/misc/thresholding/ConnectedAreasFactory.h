// ConnectedAreasFactory.h: interface for the CConnectedAreasFactory class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTEDAREASFACTORY_H__F45434DD_DF70_4235_948C_C470E8C41752__INCLUDED_)
#define AFX_CONNECTEDAREASFACTORY_H__F45434DD_DF70_4235_948C_C470E8C41752__INCLUDED_

#include "ConnectedArea.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConnectedAreasFactory  
{
public:
	void CreateConnectedArea(int nPane,IImage2* pSource);
	void AddFilledPoint(IImage2* pSource, int nPane, long x, long y,  long* pMap, long &nFilledCount, POINT* points, SIZE size, color CurrentColor, long lIndex);
	long m_nAreaCounter;
	CConnectedArea *areas;
	POINT *pPoints;
	long *pMap;
	CConnectedAreasFactory();
	virtual ~CConnectedAreasFactory();

};

#endif // !defined(AFX_CONNECTEDAREASFACTORY_H__F45434DD_DF70_4235_948C_C470E8C41752__INCLUDED_)
