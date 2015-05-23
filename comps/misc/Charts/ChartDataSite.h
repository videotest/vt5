#pragma once

#include "misc.h"
class CChartDataSite
{
protected:
	struct XChartDataItem
	{
		long nSz;
		double *pValuesX;
		double *pValuesY;
		double minX, maxX;
		double minY, maxY;
		bool bExternX;
		bool bExternY;

		XChartDataItem()
		{
			nSz = 0;
			bExternX = false;
			bExternY = false;
			pValuesX = pValuesY = 0;
			minY = minX = 1e308;
			maxY = maxX = -1e307;
		}

		~XChartDataItem()
		{
			if( pValuesX && !bExternX )
				delete []pValuesX;
			if( pValuesY && !bExternY  )
				delete []pValuesY;
		}

		XChartDataItem( double *pX, double *pY, long sz )
		{
			bExternX = false;
			bExternY = false;
			pValuesX = pValuesY = 0;
			nSz = sz;
			if( pX )
			{
				pValuesX = new double[ nSz ];
				memcpy( pValuesX, pX, sizeof(double)*nSz );
				minX = 1e308;
				maxX = -1e307;
			}
			if( pY )
			{
				pValuesY = new double[ nSz ];
				memcpy( pValuesY, pY, sizeof(double)*nSz );
				minY = 1e308;
				maxY = -1e307;
			}
		}

		void operator=(  const XChartDataItem &data  )
		{
			if( data.nSz > 0 )
			{
				nSz = data.nSz;

				if( data.pValuesX && !data.bExternX )
				{
					if( pValuesX && !bExternX )
						delete []pValuesX;
					pValuesX = new double[ nSz ];
					memcpy( pValuesX, data.pValuesX, sizeof(double)*nSz );
					
					minX = data.minX;
					maxX = data.maxX;
				}
				else if( data.pValuesX && data.bExternX )
				{
					if( pValuesX && !bExternX )
						delete []pValuesX;

					bExternX  = data.bExternX;
					pValuesX = data.pValuesX;

					minX = data.minX;
					maxX = data.maxX;
				}

				if( data.pValuesY && !data.bExternY )
				{
					if( pValuesY && !bExternY )
						delete []pValuesY;
					pValuesY = new double[ nSz ];
					memcpy( pValuesY, data.pValuesY, sizeof(double)*nSz );

					minY = data.minY;
					maxY = data.maxY;
				}
				else if( data.pValuesY && data.bExternY )
				{
					if( pValuesY && !bExternY )
						delete []pValuesY;

					bExternY  = data.bExternY;
					pValuesY = data.pValuesY;

					minY = data.minY;
					maxY = data.maxY;
				}
			}
		}
	};
	_map_t2<long,XChartDataItem> m_ChartsData;
public:
	CChartDataSite(void);
	~CChartDataSite(void);
};
