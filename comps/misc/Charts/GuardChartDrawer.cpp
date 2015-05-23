// GuardChartDrawer.cpp: implementation of the CGuardChartDrawer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GuardChartDrawer.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CGuardChartDrawer, CCmdTargetEx);


// {B7D2C875-8C05-46d4-9BE0-2B47E6B98668}
GUARD_IMPLEMENT_OLECREATE( CGuardChartDrawer, "Charts.ChartDrawer", 
0xB7D2C875, 0x8C05, 0x46d4, 0x9B, 0xE0, 0x2B, 0x47, 0xE6, 0xB9, 0x86, 0x68);

BEGIN_INTERFACE_MAP(CGuardChartDrawer, CGuardChartDrawer)
	INTERFACE_PART(CGuardChartDrawer, IID_IChartAttributes, Attributes )
	INTERFACE_PART(CGuardChartDrawer, IID_IChartDrawer, ChartDrawer )
	INTERFACE_PART(CGuardChartDrawer, IID_ISmartChartDrawer, SmartChartDrawer )
	INTERFACE_PART(CGuardChartDrawer, IID_IChartMiscHelper, ChartMiscHelper )
END_INTERFACE_MAP()

IMPLEMENT_UNKNOWN(CGuardChartDrawer, Attributes);
IMPLEMENT_UNKNOWN(CGuardChartDrawer, ChartDrawer);
IMPLEMENT_UNKNOWN(CGuardChartDrawer, SmartChartDrawer);
IMPLEMENT_UNKNOWN(CGuardChartDrawer, ChartMiscHelper);


CGuardChartDrawer::CGuardChartDrawer()
{

}

CGuardChartDrawer::~CGuardChartDrawer()
{

}

HRESULT CGuardChartDrawer::XAttributes::SetColor( DWORD lColor, CommonChartColors dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetColor)
	{
		return pThis->m_Chart.SetColor( lColor, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::GetColor( DWORD *lColor, CommonChartColors dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetColor)
	{
		return pThis->m_Chart.GetColor( lColor, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetChartColor( long nID, DWORD lColor, ChartColors dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetChartColor)
	{
		return pThis->m_Chart.SetChartColor( nID, lColor, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetChartColor( long nID, DWORD *lColor, ChartColors dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetChartColor)
	{
		return pThis->m_Chart.GetChartColor( nID, lColor, dwFlag );
	}
	_catch_nested;
}

// Установа/Получение общего стиля. 
HRESULT CGuardChartDrawer::XAttributes::SetStyle( CommonChartViewParam dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetStyle)
	{
		return pThis->m_Chart.SetStyle( dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetStyle( CommonChartViewParam *dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetStyle)
	{
		return pThis->m_Chart.GetStyle( dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetChartStyle(long nID, ChartViewParam dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetChartStyle)
	{
		return pThis->m_Chart.SetChartStyle( nID, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetChartStyle(long nID, ChartViewParam *dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetChartStyle)
	{
		return pThis->m_Chart.GetChartStyle( nID, dwFlag );
	}
	_catch_nested;
}

// Установа/Получение стиля линии.
HRESULT CGuardChartDrawer::XAttributes::SetLineStyle( ULONG lStyle, CommonChartLineStyles dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetLineStyle)
	{
		return pThis->m_Chart.SetLineStyle( lStyle, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetLineStyle( ULONG *lStyle, CommonChartLineStyles dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetLineStyle)
	{
		return pThis->m_Chart.GetLineStyle( lStyle, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetChartLineStyle(long nID, ULONG lStyle, ChartLineStyles dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetChartLineStyle)
	{
		return pThis->m_Chart.SetChartLineStyle( nID, lStyle, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetChartLineStyle(long nID, ULONG *lStyle, ChartLineStyles dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetChartLineStyle)
	{
		return pThis->m_Chart.GetChartLineStyle( nID, lStyle, dwFlag );
	}
	_catch_nested;
}

// Установа/Получение толoщины линии. 
// ChartLineThickness
HRESULT CGuardChartDrawer::XAttributes::SetLineThick( long nThick,   CommonChartLineThickness dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetLineThick)
	{
		return pThis->m_Chart.SetLineThick( nThick, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetLineThick( long *nThick, CommonChartLineThickness dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetLineThick)
	{
		return pThis->m_Chart.GetLineThick( nThick, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetChartLineThick(long nID, long nThick,   ChartLineThickness dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetChartLineThick)
	{
		return pThis->m_Chart.SetChartLineThick( nID, nThick, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetChartLineThick(long nID, long *nThick, ChartLineThickness dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetChartLineThick)
	{
		return pThis->m_Chart.GetChartLineThick(nID, nThick, dwFlag );
	}
	_catch_nested;
}

// Область графика на которую будут рисовать
HRESULT CGuardChartDrawer::XAttributes::SetChartRange(long nID, double lfMin, double lfMax, ChartRange dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetChartRange)
	{
		return pThis->m_Chart.SetChartRange( nID, lfMin, lfMax, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetChartRange(long nID, double *lfMin, double *lfMax, ChartRange dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetChartRange)
	{
		return pThis->m_Chart.GetChartRange( nID, lfMin, lfMax, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetRangeState(long nID, int bEnable, ChartRange dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetRangeState)
	{
		return pThis->m_Chart.SetRangeState( nID, bEnable, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetRangeState(long nID, int *bEnable, ChartRange dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetRangeState)
	{
		return pThis->m_Chart.GetRangeState( nID, bEnable, dwFlag );
	}
	_catch_nested;
}

// Установа положение визира.
HRESULT CGuardChartDrawer::XAttributes::SetTargetPos( POINT ptPos )
{
	_try_nested(CGuardChartDrawer, Attributes, SetTargetPos)
	{
		return pThis->m_Chart.SetTargetPos( ptPos );
	}
	_catch_nested;
}

// Установа формата вывода текста.
HRESULT CGuardChartDrawer::XAttributes::SetTextFormat( long nID, BSTR bstrFormat, ChartTextFormat dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetTextFormat)
	{
		return pThis->m_Chart.SetTextFormat( nID, bstrFormat, dwFlag );
	}
	_catch_nested;
}

// Удаляет график и данные связанные с ним.
HRESULT CGuardChartDrawer::XAttributes::RemoveChart( long nID )
{
	_try_nested(CGuardChartDrawer, Attributes, RemoveChart)
	{
		return pThis->m_Chart.RemoveChart( nID );
	}
	_catch_nested;
}

// Удаляет график и данные связанные с ним.
HRESULT CGuardChartDrawer::XAttributes::ShowChart( long nID, int bShow )
{
	_try_nested(CGuardChartDrawer, Attributes, ShowChart)
	{
		return pThis->m_Chart.ShowChart( nID, bShow );
	}
	_catch_nested;
}

// Установа/Получение обозначений осей.
HRESULT CGuardChartDrawer::XAttributes::SetAxisText( long nID, BSTR bstrText, ChartAxis dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetAxisText)
	{
		return pThis->m_Chart.SetAxisText( nID, bstrText, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetAxisText( long nID, BSTR *bstrText, ChartAxis dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetAxisText)
	{
		return pThis->m_Chart.GetAxisText( nID, bstrText, dwFlag );
	}
	_catch_nested;
}

// Установа/Получение различный параметров.
HRESULT CGuardChartDrawer::XAttributes::SetConstParams( long nVal, CommonChartConst dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, SetConstParams)
	{
		return pThis->m_Chart.SetConstParams( nVal, dwFlag );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::GetConstParams( long *nVal, CommonChartConst dwFlag )
{
	_try_nested(CGuardChartDrawer, Attributes, GetConstParams)
	{
		return pThis->m_Chart.GetConstParams( nVal, dwFlag );
	}
	_catch_nested;
}

// Установа/Получение различный параметров.
HRESULT CGuardChartDrawer::XAttributes::SetColorizedMap( long nID,DWORD lColor, double lfMinX, double lfMinY, double lfMaxX, double lfMaxY )
{
	_try_nested(CGuardChartDrawer, Attributes, SetColorizedMap)
	{
		return pThis->m_Chart.SetColorizedMap( nID, lColor, lfMinX, lfMinY, lfMaxX, lfMaxY );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::ClearColorized( long nID )
{
	_try_nested(CGuardChartDrawer, Attributes, ClearColorized)
	{
		return pThis->m_Chart.ClearColorized( nID );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::EnableGridLines( CommonChartGrid dwFlags )
{
	_try_nested(CGuardChartDrawer, Attributes, EnableGridLines )
	{
		return pThis->m_Chart.EnableGridLines( dwFlags );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::SetGridLines( double *fGridVals, long lCount, CommonChartGrid dwFlags )
{
	_try_nested(CGuardChartDrawer, Attributes, SetGridLines )
	{
		return pThis->m_Chart.SetGridLines( fGridVals, lCount, dwFlags );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetScaleLabels( long nID, double *pfVals, long lCount, ChartAxis dwFlags )
{
	_try_nested(CGuardChartDrawer, Attributes, SetScaleLabels )
	{
		return pThis->m_Chart.SetScaleLabels( nID, pfVals, lCount, dwFlags );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetScaleTextLabels( long nID, double *pfVals, double *pfLabels, long lCount, ChartAxis dwFlags )
{
	_try_nested(CGuardChartDrawer, Attributes, SetScaleTextLabels )
	{
		return pThis->m_Chart.SetScaleTextLabels( nID, pfVals, pfLabels, lCount, dwFlags );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetAxisTextFont( long nID, BYTE *plfFont, ChartAxis dwFlags, ChartFont dwFlag2 )
{
	_try_nested(CGuardChartDrawer, Attributes, SetAxisTextFont )
	{
		return pThis->m_Chart.SetAxisTextFont( nID, plfFont, dwFlags, dwFlag2 );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetHintText( long nID, double fX, double fY, BSTR bstrHint, int nAlign, int XOffsetInPixel, int YOffsetInPixel  )
{
	_try_nested(CGuardChartDrawer, Attributes, SetHintText )
	{
		return pThis->m_Chart.SetHintText( nID, fX, fY, bstrHint, nAlign, XOffsetInPixel, YOffsetInPixel  );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::RemoveHints( long nID )
{
	_try_nested(CGuardChartDrawer, Attributes, SetHintText )
	{
		return pThis->m_Chart.RemoveHints( nID );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetHintFont( long nID, BYTE *plfFont )
{
	_try_nested(CGuardChartDrawer, Attributes, SetHintFont )
	{
		return pThis->m_Chart.SetHintFont( nID, plfFont );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XAttributes::SetTextZoom( double fZoom )
{
	_try_nested(CGuardChartDrawer, Attributes, SetTextZoom )
	{
		return pThis->m_Chart.SetTextZoom( fZoom );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::GetTextZoom( double *pfZoom )
{
	_try_nested(CGuardChartDrawer, Attributes, GetTextZoom )
	{
		return pThis->m_Chart.GetTextZoom( pfZoom );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XAttributes::SetPixelUnit( /*[in]*/ int nUnit )
{
	_try_nested(CGuardChartDrawer, Attributes, GetTextZoom )
	{
		return pThis->m_Chart.SetPixelUnit( nUnit );
	}
	_catch_nested;
}

//Установка набора данных.
HRESULT CGuardChartDrawer::XChartDrawer::SetData(long nID, double*pValues, long nCount, ChartDataType dwFlag )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, SetData)
	{
		return pThis->m_Chart.SetData( nID, pValues, nCount, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XChartDrawer::GetData(long nID, double**pValues, long * nCount, ChartDataType dwFlag )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, GetData)
	{
		return pThis->m_Chart.GetData( nID, pValues, nCount, dwFlag );
	}
	_catch_nested;
}

//Установка набора данных данные по Y, диапазон изменения х. 
HRESULT CGuardChartDrawer::XChartDrawer::SetData2(long nID, VARIANT PtrToFunct, double lfMinX, double lfMaxX, double lfStep )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, SetData2)
	{
		return pThis->m_Chart.SetData2( nID, PtrToFunct, lfMinX, lfMaxX, lfStep );
	}
	_catch_nested;
}

// Данные на копируются. Они должны храниться на стороне клиента
HRESULT CGuardChartDrawer::XChartDrawer::SetExternData(long nID, double*pValues, long nCount, ChartDataType dwFlag )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, SetExternData)
	{
		return pThis->m_Chart.SetExternData( nID, pValues, nCount, dwFlag );
	}
	_catch_nested;
}

//Отрисовка графика(ов)
HRESULT CGuardChartDrawer::XChartDrawer::Draw( DWORD hDC,  RECT rcDraw ) 
{
	_try_nested(CGuardChartDrawer, ChartDrawer, Draw)
	{
		return pThis->m_Chart.Draw( hDC, rcDraw ) ;
	}
	_catch_nested;
}

// Возвражает значение под ptMiceInDC.
HRESULT CGuardChartDrawer::XChartDrawer::GetToolTipValue( long nID, POINT ptMiceInDC, double *pX, double *pY, short bYfromX, int nHalfHotZone )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, GetToolTipValue)
	{
		return pThis->m_Chart.GetToolTipValue( nID, ptMiceInDC, pX, pY, bYfromX, nHalfHotZone );
	}
	_catch_nested;
}
// Cглаживание. 
HRESULT CGuardChartDrawer::XChartDrawer::SmoothChart(long nID, double fSmoothParam, short bYFromX, ChartLineSmooth dwFlag )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, SmoothChart)
	{
		return pThis->m_Chart.SmoothChart( nID, fSmoothParam, bYFromX, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XChartDrawer::ClearAll()
{
	_try_nested(CGuardChartDrawer, ChartDrawer, ClearAll)
	{
		return pThis->m_Chart.ClearAll();
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XChartDrawer::LogarifmScale( ChartDataType dwFlag )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, LogarifmScale)
	{
		return pThis->m_Chart.LogarifmScale( dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XChartDrawer::AddMarker( double fX, double fY, long nPixelInSize, COLORREF clColor, ChartMarker dwFlag )
{
	_try_nested(CGuardChartDrawer, ChartDrawer, AddMarker)
	{
		return pThis->m_Chart.AddMarker( fX, fY, nPixelInSize, clColor, dwFlag );
	}
	_catch_nested;
}


//Установка и разбор выражения. 
HRESULT CGuardChartDrawer::XSmartChartDrawer::CalcValues( long nID, BSTR bstrExpr, double fMin, double fMax, double fStep )
{
	_try_nested(CGuardChartDrawer, SmartChartDrawer, CalcValues)
	{
		return pThis->m_Chart.CalcValues( nID, bstrExpr, fMin, fMax, fStep );
	}
	_catch_nested;
}

//Отрисовка графика(ов)
HRESULT CGuardChartDrawer::XSmartChartDrawer::Draw( DWORD hDC,  RECT rcDraw ) 
{
	_try_nested(CGuardChartDrawer, SmartChartDrawer, Draw)
	{
		return pThis->m_Chart.Draw( hDC, rcDraw );
	}
	_catch_nested;
}

// Возвражает значение под ptMiceInDC.
HRESULT CGuardChartDrawer::XSmartChartDrawer::GetToolTipValue( long nID, POINT ptMiceInDC, double *pX, double *pY, short bYfromX, int nHalfHotZone )
{
	_try_nested(CGuardChartDrawer, SmartChartDrawer, GetToolTipValue)
	{
		return pThis->m_Chart.GetToolTipValue( nID, ptMiceInDC, pX, pY, bYfromX, nHalfHotZone );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XSmartChartDrawer::SmoothChart(long nID, double fSmoothParam, short bYFromX, ChartLineSmooth dwFlag )
{
	_try_nested(CGuardChartDrawer, SmartChartDrawer, SmoothChart)
	{
		return pThis->m_Chart.SmoothChart( nID, fSmoothParam, bYFromX, dwFlag );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XSmartChartDrawer::ClearAll()
{
	_try_nested(CGuardChartDrawer, SmartChartDrawer, ClearAll)
	{
		return pThis->m_Chart.ClearAll();
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XSmartChartDrawer::LogarifmScale( ChartDataType dwFlag )
{
	_try_nested(CGuardChartDrawer, SmartChartDrawer, LogarifmScale)
	{
		return pThis->m_Chart.LogarifmScale( dwFlag );
	}
	_catch_nested;
}


//Установка и разбор выражения. 
HRESULT CGuardChartDrawer::XChartMiscHelper::LPtoDP( DWORD hDC, POINT *pt )
{
	_try_nested(CGuardChartDrawer, ChartMiscHelper, LPtoDP)
	{
		return pThis->m_Chart.LPtoDP( hDC, pt );
	}
	_catch_nested;
}
HRESULT CGuardChartDrawer::XChartMiscHelper::DPtoLP( DWORD hDC, POINT *pt )
{
	_try_nested(CGuardChartDrawer, ChartMiscHelper, DPtoLP)
	{
		return pThis->m_Chart.DPtoLP( hDC, pt );
	}
	_catch_nested;
}

HRESULT CGuardChartDrawer::XChartMiscHelper::ConvertToDC( long nID, RECT rcDraw, double x, double y, POINT *pt )
{
	_try_nested(CGuardChartDrawer, ChartMiscHelper, ConvertToDC)
	{
		return pThis->m_Chart.ConvertToDC( nID,rcDraw, x, y, pt );
	}
	_catch_nested;
}
