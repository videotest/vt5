/************************************************
Этот модуль сгенерирован транслятором AlgoPascal.
************************************************/

#include "ap.h"

/*-----------------------------------------------
Эти подпрограммы должен определить программист:

double incompletebeta(double a, double b, double x);
double invincompletebeta(double a, double b, double y);
-----------------------------------------------*/

double studenttdistribution(int k, double t);
double invstudenttdistribution(int k, double p);

/*************************************************************************
Интегральная функция вероятности t-распределения Стьюдента

Вычисляет интеграл от минус бесконечности до t от функции плотности
вероятности t-распределения Стьюдента.

Входные параметры:
    k   -   число степеней свободы
    t   -   аргумент
    
Допустимые значения параметров:
    k>0
    
Результат:
    значение интегральной функции вероятности t-распределения Стьюдента

Относительная погрешность вычисления приведена в таблице ниже:

ОБЛАСТЬ k       ОБЛАСТЬ t       #ПРОВЕРОК       МАКС        СРЕДН.КВ.
     1,25       -100,  -2           50000       5.9e-15     1.4e-15
     1,25         -2, 100          500000       2.7e-15     4.9e-17

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
double studenttdistribution(int k, double t)
{
    double result;
    double x;
    double rk;
    double z;
    double f;
    double tz;
    double p;
    double xsqk;
    int j;

    ap::ap_error::make_assertion(k>0);
    if( t==0 )
    {
        result = 0.5;
        return result;
    }
    if( t<-2.0 )
    {
        rk = k;
        z = rk/(rk+t*t);
        result = 0.5*incompletebeta(0.5*rk, 0.5, z);
        return result;
    }
    if( t<0 )
    {
        x = -t;
    }
    else
    {
        x = t;
    }
    rk = k;
    z = 1.0+x*x/rk;
    if( k%2!=0 )
    {
        xsqk = x/sqrt(rk);
        p = atan(xsqk);
        if( k>1 )
        {
            f = 1.0;
            tz = 1.0;
            j = 3;
            while(j<=k-2&&tz/f>ap::machineepsilon)
            {
                tz = tz*(double(j-1)/double(z*j));
                f = f+tz;
                j = j+2;
            }
            p = p+f*xsqk/z;
        }
        p = p*2.0/ap::pi();
    }
    else
    {
        f = 1.0;
        tz = 1.0;
        j = 2;
        while(j<=k-2&&tz/f>ap::machineepsilon)
        {
            tz = tz*(double(j-1)/double(z*j));
            f = f+tz;
            j = j+2;
        }
        p = f*x/sqrt(z*rk);
    }
    if( t<0 )
    {
        p = -p;
    }
    result = 0.5+0.5*p;
    return result;
}


/*************************************************************************
Функция, обратная к интегральной функции вероятности t-распределения
Стьюдента

Вычисляет такое t, что интеграл от минус бесконечности до t от функции
плотности вероятности t-распределения Стьюдента равен входному параметру p.

Входные параметры:
    k   -   число степеней свободы
    p   -   аргумент

Допустимые значения:
    k>0, 0 <= t <= 1

Результат:
    значение функции, обратной к интегральной функции вероятности
    t-распределения Стьюдента

Относительная погрешность вычисления приведена в таблице ниже:

ОБЛАСТЬ k       ОБЛАСТЬ p       #ПРОВЕРОК       МАКС        СРЕДН.КВ.
   1, 100     0.001,0.999           25000       5.7e-15     8.0e-16
   1, 100     10^-6,0.001           25000       2.0e-12     2.9e-14

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
double invstudenttdistribution(int k, double p)
{
    double result;
    double t;
    double rk;
    double z;
    int rflg;

    ap::ap_error::make_assertion(k>0&&p>0&&p<1);
    rk = k;
    if( p>0.25&&p<0.75 )
    {
        if( p==0.5 )
        {
            result = 0;
            return result;
        }
        z = 1.0-2.0*p;
        z = invincompletebeta(0.5, 0.5*rk, fabs(z));
        t = sqrt(rk*z/(1.0-z));
        if( p<0.5 )
        {
            t = -t;
        }
        result = t;
        return result;
    }
    rflg = -1;
    if( p>=0.5 )
    {
        p = 1.0-p;
        rflg = 1;
    }
    z = invincompletebeta(0.5*rk, 0.5, 2.0*p);
    if( ap::maxrealnumber*z<rk )
    {
        result = rflg*ap::maxrealnumber;
        return result;
    }
    t = sqrt(rk/z-rk);
    result = rflg*t;
    return result;
}


