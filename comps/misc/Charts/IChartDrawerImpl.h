#pragma once

#include "ifaces.h"
#include "ichartattributesimpl.h"
#include "chartdatasite.h"

class IChartDrawerImpl : public IChartDrawer, virtual public IChartAttributesImpl, virtual public CChartDataSite
{
public:
	IChartDrawerImpl() {}
	~IChartDrawerImpl(){}

	//Установка и разбор выражения. 
	com_call SetData(long nID,  double*pValues,  long nCount, ChartDataType dwFlag );
	com_call SetData2(long nID, VARIANT PtrToFunct, double lfMinX, double lfMaxX, double lfStep );
	com_call GetData(long nID,  double**pValues, long *nCount, ChartDataType dwFlag );
	com_call SetExternData(long nID, double*pValues, long nCount, ChartDataType dwFlag );
};
