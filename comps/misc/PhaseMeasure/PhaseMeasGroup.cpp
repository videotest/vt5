#include "stdafx.h"
#include "PhaseMeasure.h"
#include "PhaseMeasGroup.h"
#include "Params.h"
#include "units_const.h"
#include "alloc.h"
#include "windows.h"

#include <math.h>

#define CALC_CHORD( chord_length, nCount, fMin, fMax, fAvg ) \
{ \
	if(chord>0) \
	{ \
		nCount++; \
		fMin=min(fMin,chord); \
		fMax=max(fMax,chord); \
		fAvg+=chord; \
	} \
}


IMPLEMENT_DYNCREATE(CPhaseMeasGroup, CMeasParamGroupBase)


CPhaseMeasGroup::CPhaseMeasGroup()
{
	_OleLockApp( this );

	m_sName = "PhaseMeas";
	m_bEnableNonVerticalChord = FALSE;
}

CPhaseMeasGroup::~CPhaseMeasGroup()
{
	_OleUnlockApp( this );
}

IMPLEMENT_GROUP_OLECREATE(CPhaseMeasGroup, "PhaseMeasure.PhaseMeasGroup",
0xd5072714, 0x8d3b, 0x4621, 0xa1, 0xe7, 0x47, 0x7c, 0xd3, 0xe5, 0xd1, 0x8d);

/////////////////////////////////////////////////////////////////////////////
// CPhaseMeasGroup message handlers

#define SET(key, value)	{if (ParamIsEnabledByKey(key)) ptrCalc->SetValue(key, value);}

bool CPhaseMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
	CImageWrp imageCalc = punkSource;
	double	fCalibr = 0.0025;
	ICalcObjectPtr	ptrCalc = punkCalcObject;
	ICalibrPtr	ptrCalibr = imageCalc;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, 0 );

	if (imageCalc.IsEmpty())
	{
//		ptrCalc->ClearValues();
		return true;
	}

	if(	!(
		ParamIsEnabledByKey(KEY_PHASE128AREA) ||
		ParamIsEnabledByKey(KEY_PHASE129AREA) ||
		ParamIsEnabledByKey(KEY_PHASE130AREA) ||
		ParamIsEnabledByKey(KEY_VCHORD_AVG) ||
		ParamIsEnabledByKey(KEY_VCHORD_MIN) ||
		ParamIsEnabledByKey(KEY_VCHORD_MAX)
		) )
		return true; // ничего не включено

	int cx=imageCalc.GetWidth();
	int cy=imageCalc.GetHeight();

	int areas[256];
	for(int i=0; i<256; i++) areas[i]=0;

	smart_alloc(ymin,int,cx);
	smart_alloc(ymax,int,cx);
	for(int x=0; x<cx; x++)
	{
		ymin[x] = -1;
		ymax[x] = -1;
	}

	for(int y=0; y<cy; y++)
	{
		byte *pm = imageCalc.GetRowMask(y);
		for(int x=0; x<cx; x++)
		{
			int c = pm[x];
			areas[c]++;
			if(c>=128)
			{
				if(ymin[x] == -1) ymin[x]=y;
				ymax[x]=y;
			}
		}
	}

	int nCount=0;
	double fMin=cy, fMax=0, fAvg=0;

	// [vanek] 18.12.2003: наклонные хорды
	if( m_bEnableNonVerticalChord )
	{   // наклонные хорды
		int nx_start = 0, nx_end = cx;
		// по верхнему краю
		if( cx > 0 )
		{
			get_min_distance( 0, ymin[ 0 ], ymax, cx, &nx_start );
			get_min_distance( cx-1, ymin[ cx-1 ], ymax, cx, &nx_end );
		}

		for(int x = nx_start; x <= nx_end; x ++)
		{
			int chord = get_min_distance( x, ymax[ x ], ymin, cx );
			CALC_CHORD( chord, nCount, fMin, fMax, fAvg)        
		}

		// по нижнему краю
		if( cx > 0 )
		{
			get_min_distance( 0, ymax[ 0 ], ymin, cx, &nx_start );
			get_min_distance( cx-1, ymax[ cx-1 ], ymin, cx, &nx_end );
		}

		for(int x = nx_start; x <= nx_end; x ++)
		{
			int chord = get_min_distance( x, ymax[ x ], ymin, cx );
			CALC_CHORD( chord, nCount, fMin, fMax, fAvg)        
		}
	}
	else
	{	// вертикальные хорды
		for(int x=0; x<cx; x++)
		{
            int chord = ymax[x]-ymin[x];
            CALC_CHORD( chord, nCount, fMin, fMax, fAvg)
		}
	}
             
	fAvg /= max(1,nCount);
	if(nCount==0) fMax=fMin=fAvg=0;

	SET(KEY_PHASE128AREA, areas[128]*fCalibr*fCalibr);
	SET(KEY_PHASE129AREA, areas[129]*fCalibr*fCalibr);
	SET(KEY_PHASE130AREA, areas[130]*fCalibr*fCalibr);
	SET(KEY_VCHORD_AVG, fAvg*fCalibr);
	SET(KEY_VCHORD_MIN, fMin*fCalibr);
	SET(KEY_VCHORD_MAX, fMax*fCalibr);

	return true;
}

bool CPhaseMeasGroup::LoadCreateParam()
{
	DefineParameter(KEY_PHASE128AREA, "Phase128 Area", "%0.1f");
	DefineParameter(KEY_PHASE129AREA, "Phase129 Area", "%0.1f");
	DefineParameter(KEY_PHASE130AREA, "Phase130 Area", "%0.1f");

	DefineParameter(KEY_VCHORD_AVG, "Average vertical chord", "%0.1f");
	DefineParameter(KEY_VCHORD_MIN, "Min vertical chord", "%0.1f");
	DefineParameter(KEY_VCHORD_MAX, "Max vertical chord", "%0.1f");

	return true;
}

bool CPhaseMeasGroup::OnInitCalculation()
{
	// [vanek] 18.12.2003: наклонные хорды
	m_bEnableNonVerticalChord = ::GetValueInt( GetAppUnknown(), "\\measurement", "VChordEnableNonVertical", 0) == 1;
	return CMeasParamGroupBase::OnInitCalculation();
}

bool CPhaseMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	switch (lKey)
	{
	case KEY_PHASE128AREA:
	case KEY_PHASE129AREA:
	case KEY_PHASE130AREA:
		{
			CString	strUnitName, str;

			str = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
			fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
			strUnitName.Format( "%s*%s", (const char*)str, (const char*)str );

			fMeterPerUnit*=fMeterPerUnit;
			bstrType = strUnitName;
			break;
		}
	case KEY_VCHORD_AVG:
	case KEY_VCHORD_MIN:
	case KEY_VCHORD_MAX:
	{
		bstrType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
		break;
	}
	default:
		{
			bstrType = "-";
			fMeterPerUnit = 1;
		}
	}

	return true;
}

/////////////////////////////////////////////////////////////////////////////
//IPersist
void CPhaseMeasGroup::GetClassID( CLSID* pClassID )
{
	if(pClassID==0) return; //так, на всякий пожарный
	memcpy( pClassID, &guid, sizeof(CLSID) );
}

// находит минимальное расстояние между точкой (nx1,ny1) и кривой y2
int CPhaseMeasGroup::get_min_distance( int nx1, int ny1,  int *py2, int ny2size, int *px_min /*= 0*/ )
{
    if( !py2 || !ny2size )
		return -1;
    
	double fdistance = 0.;
	int nx_delta = abs(py2[nx1] - ny1),
		nstart = nx1 - nx_delta,
		nend = nx1 + nx_delta;

	if( nstart > nend )
	{
		int ntemp = nstart;
		nstart = nend;
		nend = ntemp;
	}

	if( nstart < 0 )  
		nstart = 0; 

	if( nend >= ny2size )
		nend = ny2size - 1;

	fdistance = 1e10;
	int x_min = nx1;
	for( int nx2 = nstart; nx2 <= nend; nx2 ++ )
	{
		double fcurr_dist = _hypot( nx1 - nx2, ny1 - py2[ nx2 ]);
		if( fdistance > fcurr_dist  )
		{
			fdistance = fcurr_dist;
			x_min = nx2; 
		}
	}

	if( px_min )
		*px_min = x_min;
			    
	return (int)(fdistance + 0.5);
}
