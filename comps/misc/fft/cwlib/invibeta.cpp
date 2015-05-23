/************************************************
Этот модуль сгенерирован транслятором AlgoPascal.
************************************************/

#include "ap.h"

/*-----------------------------------------------
Эти подпрограммы должен определить программист:

double lngamma(double x, double& sgngam);
double incompletebeta(double a, double b, double x);
double invnormaldistribution(double y0);
-----------------------------------------------*/

double invincompletebeta(double a, double b, double y);

/*************************************************************************
Обращение неполной бета-функции.

По заданному y ищет такое x, что IncompleteBeta(a, b, x) = y.

Входные параметры:
    a,b,y   -   аргументы
    
Результат: значение обратной неполной бета-функции

Относительная погрешность вычисления (при a, b, x изменяющихся в указанном
диапазоне):

ОБЛАСТЬ x       ОБЛАСТЬ a,b     #ПРОВЕРОК       МАКС        СРЕДН.КВ.
(при a и b произвольно меняющихся в указанном диапазоне)
0,1             0.5,  10000      50000          5.8e-12     1.3e-13
0,1             0.25,   100     100000          1.8e-13     3.9e-15
0,1             0,        5      50000          1.1e-12     5.5e-15

(при целых или полуцелых a и b)
0,1             0.5,  10000      50000          5.8e-12     1.1e-13
0,1             0.5,    100     100000          1.7e-14     7.9e-16

(при a=0.5, b целое или полуцелое)
0,1             0.5,  10000      10000          8.3e-11     1.0e-11

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1996, 2000 by Stephen L. Moshier
*************************************************************************/
double invincompletebeta(double a, double b, double y)
{
    double result;
    double aaa;
    double bbb;
    double y0;
    double d;
    double yyy;
    double x;
    double x0;
    double x1;
    double lgm;
    double yp;
    double di;
    double dithresh;
    double yl;
    double yh;
    double xt;
    int i;
    int rflg;
    int dir;
    int nflg;
    double s;
    int mainlooppos;
    int ihalve;
    int ihalvecycle;
    int newt;
    int newtcycle;
    int breaknewtcycle;
    int breakihalvecycle;

    i = 0;
    ap::ap_error::make_assertion(y>=0&&y<=1);
    if( y==0 )
    {
        result = 0;
        return result;
    }
    if( y==1.0 )
    {
        result = 1;
        return result;
    }
    x0 = 0.0;
    yl = 0.0;
    x1 = 1.0;
    yh = 1.0;
    nflg = 0;
    mainlooppos = 0;
    ihalve = 1;
    ihalvecycle = 2;
    newt = 3;
    newtcycle = 4;
    breaknewtcycle = 5;
    breakihalvecycle = 6;
    while(true)
    {
        if( mainlooppos==0 )
        {
            if( a<=1.0||b<=1.0 )
            {
                dithresh = 1.0e-6;
                rflg = 0;
                aaa = a;
                bbb = b;
                y0 = y;
                x = aaa/(aaa+bbb);
                yyy = incompletebeta(aaa, bbb, x);
                mainlooppos = ihalve;
                continue;
            }
            else
            {
                dithresh = 1.0e-4;
            }
            yp = -invnormaldistribution(y);
            if( y>0.5 )
            {
                rflg = 1;
                aaa = b;
                bbb = a;
                y0 = 1.0-y;
                yp = -yp;
            }
            else
            {
                rflg = 0;
                aaa = a;
                bbb = b;
                y0 = y;
            }
            lgm = (yp*yp-3.0)/6.0;
            x = 2.0/(1.0/(2.0*aaa-1.0)+1.0/(2.0*bbb-1.0));
            d = yp*sqrt(x+lgm)/x-(1.0/(2.0*bbb-1.0)-1.0/(2.0*aaa-1.0))*(lgm+5.0/6.0-2.0/(3.0*x));
            d = 2.0*d;
            if( d<log(ap::minrealnumber) )
            {
                x = 0;
                break;
            }
            x = aaa/(aaa+bbb*exp(d));
            yyy = incompletebeta(aaa, bbb, x);
            yp = (yyy-y0)/y0;
            if( fabs(yp)<0.2 )
            {
                mainlooppos = newt;
                continue;
            }
            mainlooppos = ihalve;
            continue;
        }
        if( mainlooppos==ihalve )
        {
            dir = 0;
            di = 0.5;
            i = 0;
            mainlooppos = ihalvecycle;
            continue;
        }
        if( mainlooppos==ihalvecycle )
        {
            if( i<=99 )
            {
                if( i!=0 )
                {
                    x = x0+di*(x1-x0);
                    if( x==1.0 )
                    {
                        x = 1.0-ap::machineepsilon;
                    }
                    if( x==0.0 )
                    {
                        di = 0.5;
                        x = x0+di*(x1-x0);
                        if( x==0.0 )
                        {
                            break;
                        }
                    }
                    yyy = incompletebeta(aaa, bbb, x);
                    yp = (x1-x0)/(x1+x0);
                    if( fabs(yp)<dithresh )
                    {
                        mainlooppos = newt;
                        continue;
                    }
                    yp = (yyy-y0)/y0;
                    if( fabs(yp)<dithresh )
                    {
                        mainlooppos = newt;
                        continue;
                    }
                }
                if( yyy<y0 )
                {
                    x0 = x;
                    yl = yyy;
                    if( dir<0 )
                    {
                        dir = 0;
                        di = 0.5;
                    }
                    else
                    {
                        if( dir>3 )
                        {
                            di = 1.0-(1.0-di)*(1.0-di);
                        }
                        else
                        {
                            if( dir>1 )
                            {
                                di = 0.5*di+0.5;
                            }
                            else
                            {
                                di = (y0-yyy)/(yh-yl);
                            }
                        }
                    }
                    dir = dir+1;
                    if( x0>0.75 )
                    {
                        if( rflg==1 )
                        {
                            rflg = 0;
                            aaa = a;
                            bbb = b;
                            y0 = y;
                        }
                        else
                        {
                            rflg = 1;
                            aaa = b;
                            bbb = a;
                            y0 = 1.0-y;
                        }
                        x = 1.0-x;
                        yyy = incompletebeta(aaa, bbb, x);
                        x0 = 0.0;
                        yl = 0.0;
                        x1 = 1.0;
                        yh = 1.0;
                        mainlooppos = ihalve;
                        continue;
                    }
                }
                else
                {
                    x1 = x;
                    if( rflg==1&&x1<ap::machineepsilon )
                    {
                        x = 0.0;
                        break;
                    }
                    yh = yyy;
                    if( dir>0 )
                    {
                        dir = 0;
                        di = 0.5;
                    }
                    else
                    {
                        if( dir<-3 )
                        {
                            di = di*di;
                        }
                        else
                        {
                            if( dir<-1 )
                            {
                                di = 0.5*di;
                            }
                            else
                            {
                                di = (yyy-y0)/(yh-yl);
                            }
                        }
                    }
                    dir = dir-1;
                }
                i = i+1;
                mainlooppos = ihalvecycle;
                continue;
            }
            else
            {
                mainlooppos = breakihalvecycle;
                continue;
            }
        }
        if( mainlooppos==breakihalvecycle )
        {
            if( x0>=1.0 )
            {
                x = 1.0-ap::machineepsilon;
                break;
            }
            if( x<=0.0 )
            {
                x = 0.0;
                break;
            }
            mainlooppos = newt;
            continue;
        }
        if( mainlooppos==newt )
        {
            if( nflg!=0 )
            {
                break;
            }
            nflg = 1;
            lgm = lngamma(aaa+bbb, s)-lngamma(aaa, s)-lngamma(bbb, s);
            i = 0;
            mainlooppos = newtcycle;
            continue;
        }
        if( mainlooppos==newtcycle )
        {
            if( i<=7 )
            {
                if( i!=0 )
                {
                    yyy = incompletebeta(aaa, bbb, x);
                }
                if( yyy<yl )
                {
                    x = x0;
                    yyy = yl;
                }
                else
                {
                    if( yyy>yh )
                    {
                        x = x1;
                        yyy = yh;
                    }
                    else
                    {
                        if( yyy<y0 )
                        {
                            x0 = x;
                            yl = yyy;
                        }
                        else
                        {
                            x1 = x;
                            yh = yyy;
                        }
                    }
                }
                if( x==1.0||x==0.0 )
                {
                    mainlooppos = breaknewtcycle;
                    continue;
                }
                d = (aaa-1.0)*log(x)+(bbb-1.0)*log(1.0-x)+lgm;
                if( d<log(ap::minrealnumber) )
                {
                    break;
                }
                if( d>log(ap::maxrealnumber) )
                {
                    mainlooppos = breaknewtcycle;
                    continue;
                }
                d = exp(d);
                d = (yyy-y0)/d;
                xt = x-d;
                if( xt<=x0 )
                {
                    yyy = (x-x0)/(x1-x0);
                    xt = x0+0.5*yyy*(x-x0);
                    if( xt<=0.0 )
                    {
                        mainlooppos = breaknewtcycle;
                        continue;
                    }
                }
                if( xt>=x1 )
                {
                    yyy = (x1-x)/(x1-x0);
                    xt = x1-0.5*yyy*(x1-x);
                    if( xt>=1.0 )
                    {
                        mainlooppos = breaknewtcycle;
                        continue;
                    }
                }
                x = xt;
                if( fabs(d/x)<128.0*ap::machineepsilon )
                {
                    break;
                }
                i = i+1;
                mainlooppos = newtcycle;
                continue;
            }
            else
            {
                mainlooppos = breaknewtcycle;
                continue;
            }
        }
        if( mainlooppos==breaknewtcycle )
        {
            dithresh = 256.0*ap::machineepsilon;
            mainlooppos = ihalve;
            continue;
        }
    }
    if( rflg!=0 )
    {
        if( x<=ap::machineepsilon )
        {
            x = 1.0-ap::machineepsilon;
        }
        else
        {
            x = 1.0-x;
        }
    }
    result = x;
    return result;
}


