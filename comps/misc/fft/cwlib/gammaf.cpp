
#include <stdafx.h>
#include "gammaf.h"

double gammastirf(double x);

/*************************************************************************
Вычисление гамма-функции.

Входные параметры:
    X   -   аргумент

Допустимые значения:
    0 < X < 171.6
    -170 < X < 0, X не целое.
        
Относительная погрешность вычисления:

    ОБЛАСТЬ         #ТЕСТОВ        МАКС.          СРЕДН.
    (-170, -33)       20000      2.3e-15         3.3e-16
    (-33,   33)       20000      9.4e-16         2.2e-16
    ( 33,  171.6)     20000      2.3e-15         3.2e-16

Cephes Math Library Release 2.8:  June, 2000
Original copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
Translated to AlgoPascal by Bochkanov Sergey (2005, 2006).
*************************************************************************/
double gamma(double x)
{
    double result;
    double p;
    double pp;
    double q;
    double qq;
    double z;
    int i;
    double sgngam;

    sgngam = 1;
    q = fabs(x);
    if( q>33.0 )
    {
        if( x<0.0 )
        {
            p = ap::ifloor(q);
            i = ap::round(p);
            if( i%2==0 )
            {
                sgngam = -1;
            }
            z = q-p;
            if( z>0.5 )
            {
                p = p+1;
                z = q-p;
            }
            z = q*sin(ap::pi()*z);
            z = fabs(z);
            z = ap::pi()/(z*gammastirf(q));
        }
        else
        {
            z = gammastirf(x);
        }
        result = sgngam*z;
        return result;
    }
    z = 1;
    while(x>=3)
    {
        x = x-1;
        z = z*x;
    }
    while(x<0)
    {
        if( x>-0.000000001 )
        {
            result = z/((1+0.5772156649015329*x)*x);
            return result;
        }
        z = z/x;
        x = x+1;
    }
    while(x<2)
    {
        if( x<0.000000001 )
        {
            result = z/((1+0.5772156649015329*x)*x);
            return result;
        }
        z = z/x;
        x = x+1.0;
    }
    if( x==2 )
    {
        result = z;
        return result;
    }
    x = x-2.0;
    pp = 1.60119522476751861407E-4;
    pp = 1.19135147006586384913E-3+x*pp;
    pp = 1.04213797561761569935E-2+x*pp;
    pp = 4.76367800457137231464E-2+x*pp;
    pp = 2.07448227648435975150E-1+x*pp;
    pp = 4.94214826801497100753E-1+x*pp;
    pp = 9.99999999999999996796E-1+x*pp;
    qq = -2.31581873324120129819E-5;
    qq = 5.39605580493303397842E-4+x*qq;
    qq = -4.45641913851797240494E-3+x*qq;
    qq = 1.18139785222060435552E-2+x*qq;
    qq = 3.58236398605498653373E-2+x*qq;
    qq = -2.34591795718243348568E-1+x*qq;
    qq = 7.14304917030273074085E-2+x*qq;
    qq = 1.00000000000000000320+x*qq;
    result = z*pp/qq;
    return result;
    return result;
}


/*************************************************************************
Формула Стирлинга
Для внутреннего использования

Cephes Math Library Release 2.8:  June, 2000
Copyright 1984, 1987, 1989, 1992, 2000 by Stephen L. Moshier
*************************************************************************/
double gammastirf(double x)
{
    double result;
    double y;
    double w;
    double v;
    double stir;

    w = 1/x;
    stir = 7.87311395793093628397E-4;
    stir = -2.29549961613378126380E-4+w*stir;
    stir = -2.68132617805781232825E-3+w*stir;
    stir = 3.47222221605458667310E-3+w*stir;
    stir = 8.33333333333482257126E-2+w*stir;
    w = 1+w*stir;
    y = exp(x);
    if( x>143.01608 )
    {
        v = pow(x, 0.5*x-0.25);
        y = v*(v/y);
    }
    else
    {
        y = pow(x, x-0.5)/y;
    }
    result = 2.50662827463100050242*y*w;
    return result;
}



