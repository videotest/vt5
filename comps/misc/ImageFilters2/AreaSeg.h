#ifndef __AreaSeg_h__
#define __AreaSeg_h__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action_filter.h"
#include "FilterMorphoBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CFilterNegative class

class CAreaSeg : public CFilterMorphoBase,
	public _dyncreate_t<CAreaSeg>
{

public:
	route_unknown();
	CAreaSeg();
	~CAreaSeg();

	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ){return false;};

	bool ReSetObj(DWORD seg1,DWORD seg2,DWORD weight);	
	virtual void Smoothing( BYTE *p_arg, BYTE *p_res , int cx , int cy ,int colors, int Mask );
	virtual void FillSmallSegments1(int cx,int cy,DWORD MinObjSize);
	virtual void FillSmallSegments2(int cx,int cy,DWORD MinObjSize);
	virtual void CorrectBord(BYTE *pArg,int cx,int cy);
	virtual void FindObjects(BYTE *p_work, WORD *Palette, int cx,int cy);
	//Найти объекты (после того, как все поделено на области)
	void EnumAreas(int cx, int cy); //перенумеровать области, создав и заполнив p_AreaNumber
	void EvalAreaParams(BYTE *p_work, int cx, int cy);

public:
	DWORD *p_ObjNumber;
	int *p_PixMap;
	float *p_InWeight;
	int *p_ObjSize;
	int *p_ObgCoutE;
	int m_AreaCount; //число областей (вычисляется в процессе)
	int *p_AreaBase; //аналог p_ObjNumber, но нужного размера
	// (по номеру области - номер опорного пиксела)
	int *p_AreaNumber; // (по номеру пиксела - номер области)
	double *p_ColorAvg; //средний цвет по области
	double *p_C2Avg; //среднеквадратичное отклонение цвета
	double *p_XAvg;
	double *p_YAvg;
	double *p_R2Avg; //среднеквадратичное отклонение r (~лин. размер)
	double *p_C2R2; //параметр для классификации областей: sqrt(<C2>/<R2>)
	int *p_AreaSize; //площади областей

	/*
	DWORD *p_ObjNumber_sh;
	int *p_PixMap_sh;
	int *p_FirstPix_sh;	
	float *p_InWeight_sh;
	int *p_ObjSize_sh;
	int *p_ObgCoutE_sh;
	*/
	int m_smooth, m_smooth1, m_smooth2;
	double m_k9;
	double m_MinObjSize; //минимальный размер областей
	int m_SelectionMethod; //метод выбора первого куска фона из областей (по координатам, площади, лин.размеру)
	int m_ExpandMethod; //метод расширения фона (по средн. цвету или C2R2)
	double m_ExpandPercent; //какую часть (по параметру расширения) областей считать фоном
private:
}; 

//---------------------------------------------------------------------------
class CAreaSegInfo : public _ainfo_t<ID_ACTION_AREA_SEG, _dyncreate_t<CAreaSeg>::CreateObject, 0>,
public _dyncreate_t<CAreaSegInfo>
{
	route_unknown();
public:
	CAreaSegInfo()
	{
	}
	arg *args() {return s_pargs;}
	virtual _bstr_t target()  {return "anydoc";}
	static arg s_pargs[];
};


#endif //__AreaSeg_h__
