#pragma once

#include "action_filter.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
class CSplitCircles : public CFilter,
        public _dyncreate_t<CSplitCircles>
{
public:
	route_unknown();
	CSplitCircles(void);
	virtual ~CSplitCircles(void);
	virtual bool InvokeFilter();
protected:
	IImage3Ptr CreateImageCopy(IImage3Ptr ptrSrc, _rect rect=NORECT, bool bCopyMask=true); // создать копию куска изображения
	IImage3Ptr CreateImagePart(IImage3Ptr ptrSrc, int i1, int i2); // отрезать кусок объекта линией от точки i1 к точке i2
	bool SplitImage(IImage3* image, IUnknown** ppDst1, IUnknown** ppDst2); // разрезать на два, false - не удалось
	long m_lMaxSplits; // максимальное число разрезов на объекте
	double m_fMinArea; // минимальная площадь обрезка
	long m_lMaxPerimeter; // максимальный периметр обрезка (чтобы не резать зря конгломераты
	long m_lMinAngleDetector; // минимальная длина отрезка, рассматриваемого при вычислении углов
	double m_fAngleDetectorChordCoeff; // длина отрезка = min + coeff*chord
	double m_fAngleThreshold; // минимальный угол (степень вогнутости) на краях разреза
	_bstr_t m_bstrClassification; //классификация, используемая для деления на резаные (1) / целые (0)
	long m_lClassWhole; // класс для целых, -2 - не присваивать
	long m_lClassSplitted; // класс для разрезанных, -2 - не присваивать
private:
	ContourPoint *m_pcont; //указатель на текущий обрабатываемый контур
	int m_len; //длина текущего контура
	double CalcAngle(int i, int step);
};

//---------------------------------------------------------------------------
class CSplitCirclesInfo : public _ainfo_t<ID_ACTION_SPLITCIRCLES, _dyncreate_t<CSplitCircles>::CreateObject, 0>,
public _dyncreate_t<CSplitCirclesInfo>
{
        route_unknown();
public:
        CSplitCirclesInfo() { }
        arg *args() {return s_pargs;}
        virtual _bstr_t target()  {return "anydoc";}
        static arg s_pargs[];
};
