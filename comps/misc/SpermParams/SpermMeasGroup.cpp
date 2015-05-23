#include "stdafx.h"
#include "SpermParams.h"
#include "SpermMeasGroup.h"
#include "Params.h"
#include "units_const.h"
#include "trajectory_int.h"
#include "alloc.h"
#include "dpoint.h"
#include "windows.h"
#include "\vt5\common2\misc_calibr.h"

#include <math.h>

IMPLEMENT_DYNCREATE(CSpermMeasGroup, CMeasParamGroupBase)


CSpermMeasGroup::CSpermMeasGroup()
{
	_OleLockApp( this );

	m_sName = "Sperm";
	m_nPointsAverage = 5; //default
	m_nStepAverage = 2; //default

}

CSpermMeasGroup::~CSpermMeasGroup()
{
	_OleUnlockApp( this );
}

// {2D49B5E6-1899-4199-BCDF-C4532444F1BD}
IMPLEMENT_GROUP_OLECREATE(CSpermMeasGroup, "SpermParams.SpermMeasGroup",
0x2d49b5e6, 0x1899, 0x4199, 0xbc, 0xdf, 0xc4, 0x53, 0x24, 0x44, 0xf1, 0xbd);
/////////////////////////////////////////////////////////////////////////////
// CSpermMeasGroup message handlers


IUnknownPtr find_child_by_interface(IUnknownPtr sptrParent, const GUID guid)
{
	INamedDataObject2Ptr sptrNDOParent(sptrParent);
	POSITION lPos = 0;
	sptrNDOParent->GetFirstChildPosition(&lPos);
	while (lPos)
	{
		IUnknownPtr sptrChild;
		sptrNDOParent->GetNextChild(&lPos, &sptrChild);
		if (sptrChild!=0)
		{
			if (::CheckInterface(sptrChild, guid))
				return sptrChild;
		}
	}
	return NULL;
}

static void MakeSmoothAxis(_dpoint *axis, int len,
						   _dpoint *smooth_axis, _dpoint *vect_axis,
						   int nPointsAverage, int nStepAverage)
// На входе - ось axis и ее длина lAxisSize.
// На выходе - сглаженная ось smooth_axis
// орт ее направления в каждой точке vect_axis
{
	//посчитать вектора
	{ //посчитаем вектора направления
		for(int i=0; i<len; i++) 
		{
			int i0=max(i-5,0);
			int i1=min(i+5,len-1);
			double dx=axis[i1].x-axis[i0].x;
			double dy=axis[i1].y-axis[i0].y;
			double d=_hypot(dx,dy); d=max(d,1e-5);
			vect_axis[i].x=dx/d; vect_axis[i].y=dy/d;
		}
	}

	/*
	smart_alloc(shift, double, len); //сдвиг точки оси
	//сглаживание оси.
	for(int i=0; i<len; i++)
	{ //найдем, насколько точка оси сдвинута по перпендикуляру к оси относительно средней линии по ближайшим точкам
		double s=0;
		int n=0;
		int jmax=nPointsAverage/2;
		for(int j=-jmax; j<=jmax; j++)
		{
			int i1=i-j*nStepAverage;
			if(i1>=0 && i1<len)
			{
				s+=	(axis[i].x-axis[i1].x)*(vect_axis[i].y)-
					(axis[i].y-axis[i1].y)*(vect_axis[i].x);
				n++;
			}
		}
		shift[i]=s/n;
	}

	for(i=0; i<len; i++)
	{ //сдвинем точки перпендикулярно оси
		smooth_axis[i].x = axis[i].x - shift[i]*vect_axis[i].y;
		smooth_axis[i].y = axis[i].y + shift[i]*vect_axis[i].x;
	}
	*/

	for(int i=0; i<len; i++)
	{
		smooth_axis[i].x = axis[i].x;
		smooth_axis[i].y = axis[i].y;
	}

	double a=0.5;
	for(int j=0; j<5; j++)
	{
		for(int i=len-2; i>0; i--)
		{
			smooth_axis[i].x = a*smooth_axis[i].x + (1-a)*smooth_axis[i+1].x;
			smooth_axis[i].y = a*smooth_axis[i].y + (1-a)*smooth_axis[i+1].y;
		}

		for(i=1; i<len-1; i++)
		{
			smooth_axis[i].x = a*smooth_axis[i].x + (1-a)*smooth_axis[i-1].x;
			smooth_axis[i].y = a*smooth_axis[i].y + (1-a)*smooth_axis[i-1].y;
		}
	}
}

class CParamRecord
{
public:
	long m_nKey;
	double *m_pfVal;
	bool m_bEnabled;
	ICalcObject	*m_pCalc;
	CParamRecord(int nKey, double *pfVal, bool bEnabled, ICalcObject *pCalc)
	{ m_nKey=nKey; m_pfVal=pfVal; m_bEnabled=bEnabled; m_pCalc=pCalc; }
	~CParamRecord()
	{
		if(m_bEnabled && m_pCalc) m_pCalc->SetValue(m_nKey, *m_pfVal);
	}
};

#define PARAM2(name,key) \
	double fVal##name = 0.0; \
	bool bEnabled##name = ParamIsEnabledByKey(key); \
	CParamRecord param##name(key, &fVal##name, bEnabled##name, ptrCalc);\
	bThereIsParams |= bEnabled##name;

#define PARAM(key) PARAM2(key, key)

bool bThereIsParams = false;


bool CSpermMeasGroup::CalcValues( IUnknown *punkCalcObject, IUnknown *punkSource )
{
	CNamed2DArrays Arrs;

	CImageWrp imageCalc = punkSource;
	double	fCalibr = 0.0025;
	GUID guidC;
	::GetDefaultCalibration( &fCalibr, &guidC );

	ICalcObjectPtr	ptrCalc = punkCalcObject;
	ICalibrPtr	ptrCalibr = imageCalc;
	if( ptrCalibr != 0 )ptrCalibr->GetCalibration( &fCalibr, 0 );

	bool bCalcTrajectory=true;

	// получим траекторию
	ITrajectoryPtr ptrTrajectory = find_child_by_interface(punkCalcObject, IID_ITrajectory);
	if (ptrTrajectory==0) bCalcTrajectory=false;

	// получим номер объекта (траектории)
	long lObjNum=-1;
	if(ptrTrajectory!=0) ptrTrajectory->GetObjNum(&lObjNum);
	if(lObjNum==-1) bCalcTrajectory=false;

	// получим parent (object list)
	INamedDataObject2Ptr ptrO(punkCalcObject);
	if(ptrO==0) return false;
	IUnknown *punk2=0;
	ptrO->GetParent(&punk2);
	INamedDataObject2Ptr ptrList(punk2);
	if(punk2!=0) punk2->Release();
	if(ptrList==0) return false;

	//доберемся до его массивов
	Arrs.AttachArrays(ptrList);
	_byte_ptr<int> xTotal = Arrs.GetExistingArray("x"); //координаты
	_byte_ptr<int> yTotal = Arrs.GetExistingArray("y");
	_byte_ptr<byte> exist = Arrs.GetExistingArray("exist"); //существует ли на соответствующем кадре
	_byte_ptr<float> pLineWidth(Arrs.GetExistingArray("width")); // ширина

	int nObjectStep=Arrs.GetObjectStep();
	int nFrameStep=Arrs.GetFrameStep();
	int nFrames=::GetValueInt(ptrList, "\\particles", "FrameNo", 0);

	if(!bCalcTrajectory) nFrames=0;

	_ptr_t2<_dpoint> tr(nFrames);

	int nRealFrames=0;
	int nRealFrames2=0;
	bool bFound=false;
	for(int nFrame=0; nFrame<nFrames; nFrame++)
	{
		int posT = nFrameStep*nFrame + nObjectStep*lObjNum;
		byte bFrameExist = exist[posT]; // текущий кадр задан
		if(bFrameExist)
		{
			bFound = true;
			nRealFrames2++;
		}
		if(!bFound) continue; //пропускаем пустые кадры в начале
		tr[nRealFrames].x = xTotal[posT];
		tr[nRealFrames].y = yTotal[posT];
		nRealFrames++;
	}
	if(nRealFrames<2) bCalcTrajectory=false;

	double fRealFrames3;
	ptrCalc->GetValue(KEY_PARTICLES_NFRAMES, &fRealFrames3);

	double vap=0, vcl=0, vsl=0;
	double alh=0;
	double bcf = 0;
	double str=0;
	double lin=0;

	if(bCalcTrajectory)
	{
		// создадим сглаженную траекторию
		_ptr_t2<_dpoint> smooth_tr(nFrames);
		_ptr_t2<_dpoint> vect_tr(nFrames);
		MakeSmoothAxis(tr, nRealFrames, smooth_tr, vect_tr, m_nPointsAverage, m_nStepAverage);

		for(int i=1; i<nRealFrames; i++)
		{
			vcl += _hypot( tr[i].x-tr[i-1].x, tr[i].y-tr[i-1].y );
			vap += _hypot( smooth_tr[i].x-smooth_tr[i-1].x, smooth_tr[i].y-smooth_tr[i-1].y );
		}
		vsl = _hypot( tr[nRealFrames-1].x-tr[0].x, tr[nRealFrames-1].y-tr[0].y );
		vcl /= (nRealFrames-1);
		vap /= (nRealFrames-1);
		vsl /= (nRealFrames-1);

		for(int i=0; i<nRealFrames; i++)
		{
			alh += _hypot( tr[i].x-smooth_tr[i].x, tr[i].y-smooth_tr[i].y );
		}
		alh /= nRealFrames;

		double fs0=0, fs1=0;
		for(int i=1; i<nRealFrames; i++)
		{
			double dx0=tr[i].x-smooth_tr[i].x;
			double dy0=tr[i].y-smooth_tr[i].y;
			double dx1=tr[i-1].x-smooth_tr[i-1].x;
			double dy1=tr[i-1].y-smooth_tr[i-1].y;
			fs1 += _hypot(dx1-dx0, dy1-dy0);
			fs0 += (_hypot(dx0, dy0)+_hypot(dx1, dy1))/2;
		}
		if(fs0<1e-8) fs0=1e-8;
		bcf = sqrt(fs1/fs0)/(2*3.1415926);

		str=vsl/max(vap,1e-8)*100;
		lin=vsl/max(vcl,1e-8)*100;

		//переведем из pixel/frame в pixel/sec
		vap*=50;
		vcl*=50;
		vsl*=50;
		bcf*=50;
	}

	if (ParamIsEnabledByKey(KEY_PARTICLES_NFRAMES))
		ptrCalc->SetValue(KEY_PARTICLES_NFRAMES, nRealFrames2);
	if (ParamIsEnabledByKey(KEY_PARTICLES_VAP))
		ptrCalc->SetValue(KEY_PARTICLES_VAP, vap*fCalibr);
	if (ParamIsEnabledByKey(KEY_PARTICLES_VCL))
		ptrCalc->SetValue(KEY_PARTICLES_VCL, vcl*fCalibr);
	if (ParamIsEnabledByKey(KEY_PARTICLES_VSL))
		ptrCalc->SetValue(KEY_PARTICLES_VSL, vsl*fCalibr);
	if (ParamIsEnabledByKey(KEY_PARTICLES_ALH))
		ptrCalc->SetValue(KEY_PARTICLES_ALH, alh*fCalibr);
	if (ParamIsEnabledByKey(KEY_PARTICLES_BCF))
		ptrCalc->SetValue(KEY_PARTICLES_BCF, bcf);
	if (ParamIsEnabledByKey(KEY_PARTICLES_STR))
		ptrCalc->SetValue(KEY_PARTICLES_STR, str);
	if (ParamIsEnabledByKey(KEY_PARTICLES_LIN))
		ptrCalc->SetValue(KEY_PARTICLES_LIN, lin);

	//-----------------------------------------------------
	// вторая группа параметров - морфология
#undef PARAM
#define PARAM(name) PARAM2(name,KEY_SPERM_##name);
	bThereIsParams = false;
	PARAM(ASYMMETRY); //асимметрия головки
	PARAM(POINTED_FRONT); // заостренность морды
	PARAM(POINTED_BACK); // заостренность сзади
	PARAM(ACROSOME_PERCENT); // процент площади акросомы
	PARAM(MITO_LENGTH); // длина митохондриальной части хвоста
	PARAM(MITO_WIDTH);  // ширина
	PARAM(MITO_AREA); // площадь
	PARAM(TAIL_SHIFT); // сдвиг хвоста по отношению к большой оси эллипса
	PARAM(TAIL_ANGLE); // угол между хвостом и большой осью эллипса
	PARAM(TAIL_LENGTH); // длина хвоста

	if(bThereIsParams)
	{
		int cx=imageCalc.GetWidth();
		int cy=imageCalc.GetHeight();
		CPoint ptOffset = imageCalc.GetOffset();

		smart_alloc(ppM,byte*,cy);
		smart_alloc(ppC0,color*,cy);
		smart_alloc(ppC1,color*,cy);
		smart_alloc(ppC2,color*,cy);

		for(int y=0; y<cy; y++)
		{
			ppC0[y]=imageCalc.GetRow(y,0);
			ppC1[y]=imageCalc.GetRow(y,1);
			ppC2[y]=imageCalc.GetRow(y,2);
			ppM[y]=imageCalc.GetRowMask(y);
		}

		///////////////////////////////////////////////////////////////////////
		// подсчет морфологических параметров
		///////////////////////////////////////////////////////////////////////
		// подсчет параметров - интегралов по площади
		int count=0;
		int count_acro=0;
		double mx=0.0, my=0.0;
		double mxx=0.0, mxy=0.0, myy=0.0;
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				if(ppM[y][x]>=128)
				{
					if(ppM[y][x]==253) count_acro++;
					count++;
					mx += x; my += y;
					mxx += x*x; mxy += x*y; myy += y*y;
				}
			}
		}

		int s=max(count,1);

		mx /= s; my /= s;
		mxx /= s; mxy /= s; myy /= s;
		mxx -= mx*mx; mxy -= mx*my; myy -= my*my;

		fValACROSOME_PERCENT = double(count_acro)/s;

		double fAngleHead = ::atan2( 2*mxy, mxx-myy )/2;
		// посчитали угол, под которым расположена голова
		if(bCalcTrajectory)
		{
			int posT0 = nFrameStep*0/*nFrame*/ + nObjectStep*lObjNum;
			int posT1 = nFrameStep*1/*nFrame*/ + nObjectStep*lObjNum;
			int posT4 = nFrameStep*4/*nFrame*/ + nObjectStep*lObjNum;
			if(exist[posT0] && exist[posT1] && exist[posT4])
			{
				double fAngleTail = ::atan2( double(yTotal[posT4]-yTotal[posT0]), double(xTotal[posT4]-xTotal[posT0]) );
				fValTAIL_ANGLE = fAngleTail - fAngleHead;
				while (fValTAIL_ANGLE > PI/2) fValTAIL_ANGLE -= PI;
				while (fValTAIL_ANGLE < -PI/2) fValTAIL_ANGLE += PI;
				double fAngleTailStart = ::atan2( yTotal[posT0] - ptOffset.y - my, xTotal[posT0] - ptOffset.x - mx );
				fValTAIL_SHIFT = fAngleTailStart - fAngleHead;
				while (fValTAIL_SHIFT > PI/2) fValTAIL_SHIFT -= PI;
				while (fValTAIL_SHIFT < -PI/2) fValTAIL_SHIFT += PI;
			}
			else
			{ // не можем посчитать - не прописывать
				bEnabledTAIL_ANGLE = false;
			}
			for(int nFrame=1; nFrame<nFrames; nFrame++)
			{
				int posT = nFrameStep*nFrame + nObjectStep*lObjNum;
				int posT1 = nFrameStep*(nFrame-1) + nObjectStep*lObjNum;
				if(exist[posT] && exist[posT1])
					fValTAIL_LENGTH += _hypot( xTotal[posT]-xTotal[posT1], yTotal[posT]-yTotal[posT1] );
			}
			fValTAIL_LENGTH *= fCalibr;
		}

		// подсчет асимметрии - число точек маски, не совпадающих
		// с зеркальным отражением относительно продольной оси
		double co=cos(fAngleHead), si=sin(fAngleHead);
		int nBad=0;
		double x2Max=0, x2Min=0, x2SqrAvg=0;
		double y2Max=0, y2Min=0, y2SqrAvg=0;
		double sumPointed1=0, sumPointed2=0;
		int countPointed1=0, countPointed2=0;
		for(int y=0; y<cy; y++)
		{
			for(int x=0; x<cx; x++)
			{
				if(ppM[y][x]>=128)
				{
					double x1=x-mx, y1=y-my;
					double x2=x1*co+y1*si, y2=-x1*si+y1*co;
					if(x2>x2Max) x2Max=x2;
					if(x2<x2Min) x2Min=x2;
					x2SqrAvg += x2*x2;
					if(y2>y2Max) y2Max=y2;
					if(y2<y2Min) y2Min=y2;
					y2SqrAvg += y2*y2;
					if(x2>0)
					{

					}
					else
					{
					}
					double x3=x2, y3=-y2;
					double x4=x3*co-y3*si, y4=x3*si+y3*co;
					int xx=int(x4+mx), yy=int(y4+my);

					if(xx>=0 && yy>=0 && xx<cx && yy<cy)
					{
						if(ppM[yy][xx]<128) nBad++;
					}
					else nBad++;
				}
			}
		}
		x2SqrAvg /= s;
		y2SqrAvg /= s;
		fValASYMMETRY = double(nBad)/s;
		double fAxisBig = sqrt(x2SqrAvg)*2;
		double xAxis1 = fAxisBig*co, yAxis1 = fAxisBig*si;
		double r1=0, r2=0;
		if(bCalcTrajectory)
		{
			int posT0 = nFrameStep*0/*nFrame*/ + nObjectStep*lObjNum;
			double xt0=xTotal[posT0], yt0=yTotal[posT0];
			r1 = _hypot(ptOffset.x+mx+xAxis1 - xTotal[posT0], ptOffset.y+my+yAxis1 - yTotal[posT0]);
			r2 = _hypot(ptOffset.x+mx-xAxis1 - xTotal[posT0], ptOffset.y+my-yAxis1 - yTotal[posT0]);
		}
		if( r1<r2 )
		{	// x2Max ближе к хвосту, чем x2Min
			fValPOINTED_BACK = x2Max/fAxisBig - 1.0;
			fValPOINTED_FRONT = -x2Min/fAxisBig - 1.0;
		}
		else
		{
			fValPOINTED_FRONT = x2Max/fAxisBig - 1.0;
			fValPOINTED_BACK = -x2Min/fAxisBig - 1.0;
		}

		///////////////////////////////////////////////////////////////////////
		// подсчет параметров митохондриальной части хвоста
		fValMITO_LENGTH=0;
		fValMITO_AREA=0;
		if(bCalcTrajectory)
		{
			for(int nFrame=1; nFrame<nFrames; nFrame++)
			{
				int posT = nFrameStep*nFrame + nObjectStep*lObjNum;
				int posT1 = nFrameStep*(nFrame-1) + nObjectStep*lObjNum;
				if(exist[posT] && exist[posT1] && pLineWidth!=0)
				{
					double w = pLineWidth[posT];
					double w1 = pLineWidth[posT1];
					if(w>0 && w1>0)
					{
						double r = _hypot( xTotal[posT]-xTotal[posT1], yTotal[posT]-yTotal[posT1] );
						fValMITO_LENGTH += r;
						fValMITO_AREA += r*(w+w1);
					}
				}
			}
			fValMITO_WIDTH = fValMITO_AREA/max(1,fValMITO_LENGTH);

			fValMITO_LENGTH *= fCalibr;
			fValMITO_WIDTH *= fCalibr;
			fValMITO_AREA *= fCalibr*fCalibr;
		}
	}
	if(!bCalcTrajectory)
	{
		bEnabledPOINTED_FRONT=false;
		bEnabledPOINTED_BACK=false;
		bEnabledMITO_LENGTH=false;
		bEnabledMITO_WIDTH=false;
		bEnabledMITO_AREA=false;
		bEnabledTAIL_SHIFT=false;
		bEnabledTAIL_ANGLE=false;
		bEnabledTAIL_LENGTH=false;
	}

	return true;
}

bool CSpermMeasGroup::LoadCreateParam()
{
	DefineParameter(KEY_PARTICLES_NFRAMES,	"Frames",	"%0.0f");
	DefineParameter(KEY_PARTICLES_VAP,	"SpermVAP",	"%0.3f");
	DefineParameter(KEY_PARTICLES_VSL,	"SpermVSL",	"%0.3f");
	DefineParameter(KEY_PARTICLES_VCL,	"SpermVCL",	"%0.3f");
	DefineParameter(KEY_PARTICLES_ALH,	"SpermALH",	"%0.3f");
	DefineParameter(KEY_PARTICLES_BCF,	"SpermBCF",	"%0.3f");
	DefineParameter(KEY_PARTICLES_STR,	"SpermSTR",	"%0.3f");
	DefineParameter(KEY_PARTICLES_LIN,	"SpermLIN",	"%0.3f");

	DefineParameter(KEY_SPERM_ASYMMETRY,	"SpermAsymmetry",	"%0.3f");
	DefineParameter(KEY_SPERM_POINTED_FRONT,	"SpermTaperedFront",	"%0.3f");
	DefineParameter(KEY_SPERM_POINTED_BACK,	"SpermTaperedBack",	"%0.3f");
	DefineParameter(KEY_SPERM_ACROSOME_PERCENT,	"SpermAcrosomePercent",	"%0.3f");
	DefineParameter(KEY_SPERM_MITO_LENGTH,	"SpermMidleLength",	"%0.3f");
	DefineParameter(KEY_SPERM_MITO_WIDTH,	"SpermMidleWidth",	"%0.3f");
	DefineParameter(KEY_SPERM_MITO_AREA,	"SpermMidleArea",	"%0.3f");
	DefineParameter(KEY_SPERM_TAIL_SHIFT,	"SpermTailShift",	"%0.3f");
	DefineParameter(KEY_SPERM_TAIL_ANGLE,	"SpermTailAngle",	"%0.3f");
	DefineParameter(KEY_SPERM_TAIL_LENGTH,	"SpermTailLength",	"%0.3f");

	return true;
}

bool CSpermMeasGroup::OnInitCalculation()
{
	m_nPointsAverage = ::GetValueInt( ::GetAppUnknown(), "\\SpermMeasGroup", "PointsAverage", 5);
	m_nStepAverage = ::GetValueInt( ::GetAppUnknown(), "\\SpermMeasGroup", "StepAverage", 2);
	return CMeasParamGroupBase::OnInitCalculation();
}

bool CSpermMeasGroup::GetUnit( long lKey, long lType, _bstr_t &bstrType, double &fMeterPerUnit )
{
	if( lKey == KEY_PARTICLES_NFRAMES )
	{
		bstrType = GetValueString( GetAppUnknown(), szCountUnits, szUnitName, szDefCountUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szCountUnits, szUnitCoeff, 1 );
		return true;
	}
	else if(
		lKey == KEY_PARTICLES_VAP || 
		lKey == KEY_PARTICLES_VSL ||
		lKey == KEY_PARTICLES_VCL )
	{
		bstrType = GetValueString( GetAppUnknown(), szVelocityUnits, szUnitName, szDefVelocityUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szVelocityUnits, szUnitCoeff, 1 );
		return true;
	}
	else if( lKey == KEY_PARTICLES_ALH ||
		lKey == KEY_SPERM_MITO_LENGTH ||
		lKey == KEY_SPERM_MITO_WIDTH ||
		lKey == KEY_SPERM_TAIL_LENGTH )
	{
		bstrType = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
		return true;
	}
	else if( lKey == KEY_SPERM_MITO_AREA )
	{
		CString	strUnitName, str;

		str = GetValueString( GetAppUnknown(), szLinearUnits, szUnitName, szDefLinearUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szLinearUnits, szUnitCoeff, 1 );
		strUnitName.Format( "%s*%s", (const char*)str, (const char*)str );

		fMeterPerUnit*=fMeterPerUnit;
		bstrType = strUnitName;
		return true;
	}
	else if( lKey == KEY_PARTICLES_BCF )
	{
		bstrType = GetValueString( GetAppUnknown(), szFrequencyUnits, szUnitName, szDefFrequencyUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szFrequencyUnits, szUnitCoeff, 1 );
		return true;
	}
	else if(
		lKey == KEY_PARTICLES_LIN || 
		lKey == KEY_PARTICLES_STR )
	{
		bstrType = GetValueString( GetAppUnknown(), szRelativeUnits, szUnitName, szDefRelativeUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szRelativeUnits, szUnitCoeff, 1 );
		return true;
	}
	else if(
		lKey == KEY_SPERM_TAIL_ANGLE ||
		lKey == KEY_SPERM_TAIL_SHIFT )
	{
		bstrType = GetValueString( GetAppUnknown(), szAngleUnits, szUnitName, szDefAngleUnits );
		fMeterPerUnit = GetValueDouble( GetAppUnknown(), szAngleUnits, szUnitCoeff, 1 );
		return true;
	}
	else
	{
		bstrType = "-";
		fMeterPerUnit = 1;
		return true;
	}

	return false;
}

/////////////////////////////////////////////////////////////////////////////
//IPersist
void CSpermMeasGroup::GetClassID( CLSID* pClassID )
{
	if(pClassID==0) return; //так, на всякий пожарный
	memcpy( pClassID, &guid, sizeof(CLSID) );
}
