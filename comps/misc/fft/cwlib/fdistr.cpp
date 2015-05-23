
//#include <stdafx.h>
#include "fdistr.h"

/*************************************************************************
Интегральная функция вероятности F-распределения со степенями свободы a и b

Входные параметры:
    a,b -   степени свободы
    x   -   аргумент
    
Результат: P(F<=x|a,b)

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
double fdistribution(int a, int b, double x)
{
    double result;
    double w;

    ap::ap_error::make_assertion(a>=1&&b>=1&&x>=0);
    w = a*x;
    w = w/(b+w);
    result = incompletebeta(0.5*a, 0.5*b, w);
    return result;
}


/*************************************************************************
Дополнение интегральной функции вероятности F-распределения
со степенями свободы a и b до единицы

Входные параметры:
    a,b -   степени свободы
    x   -   аргумент

Результат: 1-P(F<=x|a,b)

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
double fcdistribution(int a, int b, double x)
{
    double result;
    double w;

    ap::ap_error::make_assertion(a>=1&&b>=1&&x>=0);
    w = b/(b+a*x);
    result = incompletebeta(0.5*b, 0.5*a, w);
    return result;
}


/*************************************************************************
Функция, обратная к интегральной функции вероятности F-распределения
со степенями свободы a и b

Алгоритм находит такое x, что P(F<=x|a,b)=y

Входные параметры:
    a,b -   степени свободы
    y   -   аргумент

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
double invfdistribution(int a, int b, double y)
{
    double result;
    double w;
    double x;

    ap::ap_error::make_assertion(a>=1&&b>=1&&y>0&&y<=1);
    
    //
    // Compute probability for x = 0.5
    //
    w = incompletebeta(0.5*b, 0.5*a, 0.5);
    
    //
    // If that is greater than y, then the solution w < .5
    // Otherwise, solve at 1-y to remove cancellation in (b - b*w)
    //
    if( w>y||y<0.001 )
    {
        w = invincompletebeta(0.5*b, 0.5*a, y);
        result = (b-b*w)/(a*w);
    }
    else
    {
        w = invincompletebeta(0.5*a, 0.5*b, 1.0-y);
        result = b*w/(a*(1.0-w));
    }
    return result;
}



