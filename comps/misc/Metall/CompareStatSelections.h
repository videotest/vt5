#pragma once
#include "\vt5\common2\action_main.h"
#include "resource.h"
#include <atlstr.h>

class CCompareStatSelections :
	public CAction,
	public _dyncreate_t<CCompareStatSelections>
{
public:
	route_unknown();
	CCompareStatSelections(void);
	~CCompareStatSelections(void);
	com_call DoInvoke();

protected:
	// сравнение двух ФРВ
	// pfrv1 и pfrv2 - массивы пар чисел: <длина_хорды_k> - <значение_ФРВ_k> -> pfrv1[k] и pfrv[k + 1]
    double compare( double *pfrv1, int nsize_frv1, double *pfrv2, int nsize_frv2 ); 
};

//---------------------------------------------------------------------------
class CCompareStatSelectionsInfo : 
public _ainfo_t<ID_ACTION_COMPARE_STAT_SELECTIONS, _dyncreate_t<CCompareStatSelections>::CreateObject, 0>,
public _dyncreate_t<CCompareStatSelectionsInfo>
{
	route_unknown();
public:
	CCompareStatSelectionsInfo() { }
	arg *args() {return s_pargs;}
	static arg s_pargs[];
};
