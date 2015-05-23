#pragma once

#include "ifaces.h"
#include "ichartattributesimpl.h"
#include "chartdatasite.h"

class ISmartChartDrawerImpl : public ISmartChartDrawer, virtual public IChartAttributesImpl, virtual public CChartDataSite
{
public:
	ISmartChartDrawerImpl() 
	{ 
	}
	~ISmartChartDrawerImpl()
	{ 
	}
	//Установка и разбор выражения. 
	com_call CalcValues( long nID, BSTR bstrExpr, double fMin, double fMax, double fStep );
};
