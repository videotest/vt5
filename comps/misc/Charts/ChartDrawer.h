#pragma once

#include "ichartdrawerimpl.h"
#include "ismartchartdrawerimpl.h"
#include "ichartattributesimpl.h"

#include "gdiplusmanager.h"
class CChartDrawer : public IChartDrawerImpl, public ISmartChartDrawerImpl, public IUnknownImpl<CChartDrawer>, public IChartMiscHelper
{
	double m_lfMinX, m_lfMaxX;
	double m_lfMinY, m_lfMaxY;
	static CGDIPlusManager *m_pGDIPlusManager;
	static long m_nChartsCounter;
	RECT m_rcCorrected, m_rcOld;
	POINT m_ptOffset;
	bool m_bDrawHint;
	long m_lDrawingFlag;

	struct x_markers
	{
		double x;
		double y;

		long lsz;
		long ltype;
		COLORREF color;

		x_markers()
		{
			init();
		}

		x_markers( const x_markers &item )
		{
			init();
			*this = item;
		}

		void operator=( const x_markers &item )
		{
			x		= item.x;
			y		= item.y;

			lsz		= item.lsz;
			ltype	= item.ltype;

			color = item.color;
		}

		void init()
		{
			x = y = 0;

			lsz = 2;
			ltype = 0;
			color = 0;
		}
	};

	vector<x_markers> m_list_markers;

public:
	CChartDrawer(void);
	~CChartDrawer(void);

	BEGIN_INTERFACE_SUPPORT_MAP( CChartDrawer )
		INTERFACE_SUPPORT_ENTRY( IChartAttributes )
		INTERFACE_SUPPORT_ENTRY( IChartDrawer )
		INTERFACE_SUPPORT_ENTRY( ISmartChartDrawer )
		INTERFACE_SUPPORT_ENTRY( IChartMiscHelper )
	END_INTERFACE_SUPPORT_MAP()

	//Отрисовка графика(ов)
	com_call Draw( DWORD hDC,  RECT rcDraw ); 

	// Возвражает значение под ptMiceInDC.
	com_call GetToolTipValue( long nID, POINT ptMiceInDC, double *pX, double *pY, short bYfromX, int nHalfHotZone );

	// Cглаживание.
	com_call SmoothChart(long nID, double fSmoothParam, short bYFromX, ChartLineSmooth dwFlag );

	com_call ClearAll();
	com_call LogarifmScale( ChartDataType dwFlag );

	com_call LPtoDP( DWORD hDC, POINT *pt );
	com_call DPtoLP( DWORD hDC, POINT *pt );

	com_call ConvertToDC( long nID, RECT rcDraw, double x, double y, POINT *pt );
	com_call AddMarker( double fX, double fY, long nPixelInSize, COLORREF clColor, ChartMarker dwFlag );
protected:
	HRESULT _draw_graph( DWORD hDC, long nID,  RECT rcDraw ); 
	HRESULT _draw_hint( DWORD hDC, long nID,  RECT rcDraw ); 
	HRESULT _draw_axis( DWORD hDC, RECT *rcDraw,  const RECT& rcGraph );
	HRESULT _calc_draw_range( int nID, double &lfMinX, double &lfMaxX, double &lfMinY, double &lfMaxY);
	void do_default( bool bConst = true );
private:
	HRESULT MeasureAxis( DWORD hDC, RECT *rcDraw);
	float m_YDigitsWidth;
};
