#ifndef __SpermSegm_h__
#define __SpermSegm_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"
#include "Named2DArrays.h"
#include "trajectory.h"

/////////////////////////////////////////////////////////////////////////////

#define CSize SIZE

class CSpermSegm : public CFilter,
public _dyncreate_t<CSpermSegm>
{
public:
	route_unknown();
	CSpermSegm();
	~CSpermSegm();
	
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa )
	{	
		if(pa->m_bInArgument)
		{
			return false;
		}
		
		return CFilter::CanDeleteArgument(pa);
	};
	
protected:
	void FirstPass(color **src, byte **bin, int cx, int cy, double bg_tolerance);
	void FirstPass2(color **src, byte **bin, int cx, int cy, bool invert_ratio);
	int m_back, m_back_width;
	int m_fore, m_fore_width;
	int m_nForeMethod; // 0 - темное, 1 - светлое, -1 - автомат (выбор 0/1), 2 - черный фон (0), 3 - белый фон (65535)
}; 


//---------------------------------------------------------------------------
class CSpermSegmInfo : 
public _ainfo_t<ID_ACTION_SPERMSEGM, _dyncreate_t<CSpermSegm>::CreateObject, 0>,
public _dyncreate_t<CSpermSegmInfo>
{
	route_unknown();
public:
	CSpermSegmInfo() { }
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};

/////////////////////////////////////////////////////////////////////////////

class CSegmObjects : public CFilter,
public _dyncreate_t<CSegmObjects>
{
public:
	route_unknown();
public:
	CSegmObjects();
	virtual ~CSegmObjects();
	virtual bool InvokeFilter();
};

class CSegmObjectsInfo : public _ainfo_t<ID_ACTION_SEGMOBJECTS, _dyncreate_t<CSegmObjects>::CreateObject, 0>,
public _dyncreate_t<CSegmObjectsInfo>
{
	route_unknown();
public:
	CSegmObjectsInfo() { }
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target() {return "anydoc";}
	static arg	s_pargs[];
};


/////////////////////////////////////////////////////////////////////////////

class CSegmLines : public CFilter,
	public CNamed2DArrays,
	public _dyncreate_t<CSegmLines>
{
public:
	route_unknown();
public:
	CSegmLines();
	virtual ~CSegmLines();
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa )
	{	if(pa->m_bstrArgName == _bstr_t("Src")) return false;
		if(pa->m_bstrArgName == _bstr_t("SrcStopZone")) return false;
		return CFilter::CanDeleteArgument(pa);
	};
	virtual bool IsAllArgsRequired(){return false;};
private:
	double m_si[256],m_co[256]; // таблицы синусов и косинусов
	//int TestLine(int x, int y, int nAngle, color **rows, int cx, int cy);
	//int LineMedian(int x, int y, int nAngle, color **rows, int cx, int cy);
	int m_len, m_w;
};

class CSegmLinesInfo : public _ainfo_t<ID_ACTION_SEGMLINES, _dyncreate_t<CSegmLines>::CreateObject, 0>,
public _dyncreate_t<CSegmLinesInfo>
{
	route_unknown();
public:
	CSegmLinesInfo() { }
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target() {return "anydoc";}
	static arg	s_pargs[];
};

class CLineTracer
{
public:
	CLineTracer();
	~CLineTracer();
	int TraceStep(int i); // проследить, куда пойдет i точка; могут также меняться предшествующие
	int ExpandStep(int i); // преобразовать участок i-1..i в такое количество, чтобы каждый был не длиннее step

	int LineMedian(int x, int y, int nAngle);
	int TestLine(int x, int y, int nAngle);

	bool LoadData(
		_byte_ptr<int> pLineX, _byte_ptr<int> pLineY,
		_byte_ptr<byte> pLineExist,
		_byte_ptr<int> pLineIntensity,
		_byte_ptr<float> pLineWidth,
		int nFrameStart,
		int nFrames,
		int nFrameStep
		);
	bool StoreData(
		_byte_ptr<int> pLineX, _byte_ptr<int> pLineY,
		_byte_ptr<byte> pLineExist,
		_byte_ptr<int> pLineIntensity,
		_byte_ptr<float> pLineWidth,
		int nFrameStart,
		int nFrames,
		int nFrameStep
		);


	int m_cx, m_cy;
	color ** m_srcRows; // картинка для трассировки
	color ** m_stopRows; // вторая (черный - куда нельзя заходить)
	byte ** m_binRows; // куда нельзя заходить

	int m_px[200], m_py[200];
	int m_pangle[200]; // найденные координаты и углы
	int m_pprev[200]; // номер предыдущей точки
	int m_pintensity[200]; // "насыщенность" хвоста в этой точке
	double m_pLineWidth[200]; // ширина линии в точке
	double m_pw1[200], m_pw2[200]; // найденные значения ширины митохондриальной части вправо/влево

	double m_si[256],m_co[256]; // таблицы синусов и косинусов

	int m_len, m_w;
	int m_nStep;
	int m_nStepsBack;
	double m_fAngleTolerance;

	int m_nTraceAngles, m_nTraceAnglesStep;
	int m_nTestAngles, m_nTestAnglesStep;
};

// утилиты
void GetCenter(IImage3Ptr ptrI, POINT *ptCenter);
void CorrectEdge(IImage3Ptr ptrI, POINT *ptCenter, POINT *ptEdge);

#endif //__SpermSegm_h__
