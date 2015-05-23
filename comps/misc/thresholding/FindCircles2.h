#include "ThresholdFilterBase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"


#pragma once

/////////////////////////////////////////////////////////////////////////////
class CFindCircles2 : public CFilter,
					public _dyncreate_t<CFindCircles2>
{
public:
	route_unknown();
public:
	virtual bool InvokeFilter();
	virtual bool IsAllArgsRequired()			{return false;};
	virtual bool CanDeleteArgument( CFilterArgument *pa );
	virtual bool IsWorksWithGreyscaleOnly( )	{ return false; }   //[vanek]: support colors images
protected:
	IImage3Ptr m_sptrSrc; // входное изображение
	INamedDataObject2Ptr m_sptrList; // выход - список объектов

	color** m_ppSrc; // входные данные
	byte** m_ppBin; // входные данные - binary
	float** m_ppDist; // расстояние от края
	int** m_ppSeed; // карта номеров зародышей

	color** m_ppDst; // результат
	int m_cx, m_cy; // размеры изображения
	int m_nRadius; // максимальный радиус искомых окружностей

	const int static m_na=16;
	const int static m_nr=16;
	double m_co[m_na], m_si[m_na];
	double m_rr[m_nr];
	int m_dx[m_na*m_nr], m_dy[m_na*m_nr];
	
	bool m_bSmoothContour; // сглаживать ли контура (отсекая выступающие части)
	bool m_bCorrectByBin; // корректировать ли контура по binary
	
protected:
	// пара функций для преобразования индекса в радиус и обратно
	inline double j2r(int j) { return 2.0 + j*(m_nRadius-2.0)/(m_nr-1); };
	inline int r2j(double r) { return int( (r-2.0)*(m_nr-1)/(m_nRadius-2.0) + 0.5 ); };

	void CalcDistFromBinBound(); // Заполнение таблицы расстояний от края Binary
	double CheckCircle(int x, int y, int j); // проверить точку (x,y) на наличие окружности радиуса rr[j]; возвращает "качество"
	double FindSpot(int x, int y, int* jj); // поиск округлого пятна
	double FindSpot2(int x, int y, int* jj_max, int* jj_wanted, int* jj, double wanted_dev=1.0, int *px=0, int *py=0); // поиск округлого пятна,
	// при этом по возможности выдерживается заданный радиус и не превышается максимальный
	// если задать px и py - возвращаются уточненные координаты центра пятна
	double SmoothContour(int x, int y, int* jj, int nArcLength, int nHandleLength);
	// подрихтовать контур - заменяя сомнительные куски на нечто округлое
	INamedDataObject2Ptr CreateObject(int x, int y, int* jj);
};

//////////////////////////////////////////////////////////////////////

class CFindCircles2Info : public _ainfo_t<ID_ACTION_FINDCIRCLES2, _dyncreate_t<CFindCircles2>::CreateObject, 0>,
							public _dyncreate_t<CFindCircles2Info>
{
	route_unknown();
public:
	CFindCircles2Info()
	{
	}
	arg	*args()	{return s_pargs;}
	virtual _bstr_t target()			{return "anydoc";}
	static arg	s_pargs[];
};


