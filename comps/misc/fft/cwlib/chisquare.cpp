/************************************************
Этот модуль сгенерирован транслятором AlgoPascal.
************************************************/

#include "ap.h"

/*-----------------------------------------------
Эти подпрограммы должен определить программист:

double incompletegammac(double a, double x);
double incompletegamma(double a, double x);
double invincompletegammac(double a, double y0);
-----------------------------------------------*/


/*************************************************************************
Площадь под правым "хвостом" графика функции плотности вероятности
распределения хи-квадрат с v степенями свободы

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double chisquarecdistribution(double v, double x)
{
    double result;

    ap::ap_error::make_assertion(x>=0&&v>=1);
    result = incompletegammac(v/2.0, x/2.0);
    return result;
}


/*************************************************************************
Площадь под левым "хвостом" графика функции плотности вероятности
распределения хи-квадрат с v степенями свободы

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double chisquaredistribution(double v, double x)
{
    double result;

    ap::ap_error::make_assertion(x>=0&&v>=1);
    result = incompletegamma(v/2.0, x/2.0);
    return result;
}


/*************************************************************************
Находит такое x, что площадь под правым "хвостом" графика функции плотности
вероятности распределения хи-квадрат с v степенями свободы равна y.

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double invchisquaredistribution(double v, double y)
{
    double result;

    ap::ap_error::make_assertion(y>=0&&y<=1&&v>=1);
    result = 2*invincompletegammac(0.5*v, y);
    return result;
}


