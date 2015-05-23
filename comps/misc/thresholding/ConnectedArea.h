// ConnectedArea.h: interface for the CConnectedArea class.
//
//////////////////////////////////////////////////////////////////////
#include "image5.h"

#if !defined(AFX_CONNECTEDAREA1_H__BC826EBA_786C_4358_99E6_ED12265AE414__INCLUDED_)
#define AFX_CONNECTEDAREA1_H__BC826EBA_786C_4358_99E6_ED12265AE414__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CConnectedArea  
{
public:
	long m_EndPoint;
	long m_lStartPoint;
	color m_NeighborColor;
	bool m_Deleted;
	color m_AreaColor;
	CConnectedArea();
	virtual ~CConnectedArea();

};

#endif // !defined(AFX_CONNECTEDAREA1_H__BC826EBA_786C_4358_99E6_ED12265AE414__INCLUDED_)
