/************************************************
Этот модуль сгенерирован транслятором AlgoPascal.
************************************************/

#include "ap.h"

/*-----------------------------------------------
Эти подпрограммы должен определить программист:

double lngamma(double x, double& sgngam);
double incompletegammac(double a, double x);
double invnormaldistribution(double y0);
-----------------------------------------------*/

double invincompletegammac(double a, double y0);

/*************************************************************************
Вычисление обратного дополнительного неполного интеграла
гамма-функции (функции, обратной к IncompleteGammaC).

function InvIncompleteGammaC(a:Real; y0:Real):Real;

    0<a<10000
    0<y0<1

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1995, 2000 by Stephen L. Moshier
*************************************************************************/
double invincompletegammac(double a, double y0)
{
    double result;
    double igammaepsilon;
    double iinvgammabignumber;
    double x0;
    double x1;
    double x;
    double yl;
    double yh;
    double y;
    double d;
    double lgm;
    double dithresh;
    int i;
    int dir;
    double tmp;

    igammaepsilon = 0.000000000000001;
    iinvgammabignumber = 4503599627370496;
    x0 = iinvgammabignumber;
    yl = 0;
    x1 = 0;
    yh = 1;
    dithresh = 5*igammaepsilon;
    d = double(1)/double(9*a);
    y = 1-d-invnormaldistribution(y0)*sqrt(d);
    x = a*y*y*y;
    lgm = lngamma(a, tmp);
    i = 0;
    while(i<10)
    {
        if( x>x0||x<x1 )
        {
            d = 0.0625;
            break;
        }
        y = incompletegammac(a, x);
        if( y<yl||y>yh )
        {
            d = 0.0625;
            break;
        }
        if( y<y0 )
        {
            x0 = x;
            yl = y;
        }
        else
        {
            x1 = x;
            yh = y;
        }
        d = (a-1)*log(x)-x-lgm;
        if( d<-709.78271289338399 )
        {
            d = 0.0625;
            break;
        }
        d = -exp(d);
        d = (y-y0)/d;
        if( fabs(d/x)<igammaepsilon )
        {
            result = x;
            return result;
        }
        x = x-d;
        i = i+1;
    }
    if( x0==iinvgammabignumber )
    {
        if( x<=0 )
        {
            x = 1;
        }
        while(x0==iinvgammabignumber)
        {
            x = (1+d)*x;
            y = incompletegammac(a, x);
            if( y<y0 )
            {
                x0 = x;
                yl = y;
                break;
            }
            d = d+d;
        }
    }
    d = 0.5;
    dir = 0;
    i = 0;
    while(i<400)
    {
        x = x1+d*(x0-x1);
        y = incompletegammac(a, x);
        lgm = (x0-x1)/(x1+x0);
        if( fabs(lgm)<dithresh )
        {
            break;
        }
        lgm = (y-y0)/y0;
        if( fabs(lgm)<dithresh )
        {
            break;
        }
        if( x<=0.0 )
        {
            break;
        }
        if( y>=y0 )
        {
            x1 = x;
            yh = y;
            if( dir<0 )
            {
                dir = 0;
                d = 0.5;
            }
            else
            {
                if( dir>1 )
                {
                    d = 0.5*d+0.5;
                }
                else
                {
                    d = (y0-yl)/(yh-yl);
                }
            }
            dir = dir+1;
        }
        else
        {
            x0 = x;
            yl = y;
            if( dir>0 )
            {
                dir = 0;
                d = 0.5;
            }
            else
            {
                if( dir<-1 )
                {
                    d = 0.5*d;
                }
                else
                {
                    d = (y0-yl)/(yh-yl);
                }
            }
            dir = dir-1;
        }
        i = i+1;
    }
    result = x;
    return result;
}


