/************************************************
Этот модуль сгенерирован транслятором AlgoPascal.
************************************************/

#include "ap.h"

/*-----------------------------------------------
Эти подпрограммы должен определить программист:

double lngamma(double x, double& sgngam);
-----------------------------------------------*/

double incompletegammac(double a, double x);
double incompletegamma(double a, double x);

/*************************************************************************
Вычисление неполного интеграла гамма-функции.

function IncompleteGamma(a: Real; x:Real):Real;

Параметры:
    0<x<100
    0<a<100

Cephes Math Library Release 2.8:  June, 2000
Copyright 1985, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double incompletegammac(double a, double x)
{
    double result;
    double igammaepsilon;
    double igammabignumber;
    double igammabignumberinv;
    double ans;
    double ax;
    double c;
    double yc;
    double r;
    double t;
    double y;
    double z;
    double pk;
    double pkm1;
    double pkm2;
    double qk;
    double qkm1;
    double qkm2;
    double tmp;

    igammaepsilon = 0.000000000000001;
    igammabignumber = 4503599627370496;
    igammabignumberinv = 2.22044604925031308085*0.0000000000000001;
    if( x<=0||a<=0 )
    {
        result = 1;
        return result;
    }
    if( x<1||x<a )
    {
        result = 1-incompletegamma(a, x);
        return result;
    }
    ax = a*log(x)-x-lngamma(a, tmp);
    if( ax<-709.78271289338399 )
    {
        result = 0;
        return result;
    }
    ax = exp(ax);
    y = 1-a;
    z = x+y+1;
    c = 0;
    pkm2 = 1;
    qkm2 = x;
    pkm1 = x+1;
    qkm1 = z*x;
    ans = pkm1/qkm1;
    do
    {
        c = c+1;
        y = y+1;
        z = z+2;
        yc = y*c;
        pk = pkm1*z-pkm2*yc;
        qk = qkm1*z-qkm2*yc;
        if( qk!=0 )
        {
            r = pk/qk;
            t = fabs((ans-r)/r);
            ans = r;
        }
        else
        {
            t = 1;
        }
        pkm2 = pkm1;
        pkm1 = pk;
        qkm2 = qkm1;
        qkm1 = qk;
        if( fabs(pk)>igammabignumber )
        {
            pkm2 = pkm2*igammabignumberinv;
            pkm1 = pkm1*igammabignumberinv;
            qkm2 = qkm2*igammabignumberinv;
            qkm1 = qkm1*igammabignumberinv;
        }
    }
    while(t>igammaepsilon);
    result = ans*ax;
    return result;
}


/*************************************************************************
Вычисление дополнительного неполного интеграла гамма-функции.

function IncompleteGammaC(a: Real; x:Real):Real;

Параметры:
    0<x<100
    0<a<100

Cephes Math Library Release 2.8:  June, 2000
Copyright 1985, 1987, 2000 by Stephen L. Moshier
*************************************************************************/
double incompletegamma(double a, double x)
{
    double result;
    double igammaepsilon;
    double ans;
    double ax;
    double c;
    double r;
    double tmp;

    igammaepsilon = 0.000000000000001;
    if( x<=0||a<=0 )
    {
        result = 0;
        return result;
    }
    if( x>1&&x>a )
    {
        result = 1-incompletegammac(a, x);
        return result;
    }
    ax = a*log(x)-x-lngamma(a, tmp);
    if( ax<-709.78271289338399 )
    {
        result = 0;
        return result;
    }
    ax = exp(ax);
    r = a;
    c = 1;
    ans = 1;
    do
    {
        r = r+1;
        c = c*x/r;
        ans = ans+c;
    }
    while(c/ans>igammaepsilon);
    result = ans*ax/a;
    return result;
}


