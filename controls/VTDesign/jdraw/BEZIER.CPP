#include "stdafx.h"
#include "bezier.h"
#include <math.h>

//	Copyright Llew S. Goodstadt 1998
//		http://www.lg.ndirect.co.uk    mailto:lg@ndirect.co.uk
//		you are hereby granted the right tofair use and distribution
//		including in both commercial and non-commercial applications.

// anonymous namespace for static linking
namespace
{
    void DoBezierPts(const LBezier& V, double error, double& len, double& t, double& result);
}
void LBezier::GetCPoint(CPoint* out)
{
    for (int i = 0; i <= 3; ++i)
        out[i] = p[i].GetCPoint();
}

// split into two halves
void LBezier::Split(LBezier& Left, LBezier& Right) const
{
    // Triangle Matrix
    LBezier   Vtemp[4];

    // Copy control LDPoints
    Vtemp[0] = *this;

    // Triangle computation
    for (int i = 1; i <= 3; i++) 
    {
        for (int j =0 ; j <= 3 - i; j++) 
        {
            Vtemp[i].p[j].x =   0.5 * Vtemp[i-1].p[j].x + 0.5 * Vtemp[i-1].p[j+1].x;
            Vtemp[i].p[j].y =   0.5 * Vtemp[i-1].p[j].y + 0.5 * Vtemp[i-1].p[j+1].y;
        }
    }
    for (int j = 0; j <= 3; j++)
    {
        Left.p[j]  = Vtemp[j].p[0];      
        Right.p[j] = Vtemp[3-j].p[j];    
    }   
}   

//Get length of bezier to within error
double LBezier::Length(double error) const
{
    // control polygon length
    double controlPolygonLen = 0.0;
    for (int i = 0; i <= 2; i++)
        controlPolygonLen += p[i].DistFrom(p[i+1]);

    // chord length
    double chordLen = p[0].DistFrom(p[3]);

    // split into two until each can be approximated by its chord
    if(controlPolygonLen - chordLen > error)  
    {
        // split in two recursively and add lengths of each
        LBezier left, right;
        Split(left, right);
        return left.Length(error) + right.Length(error);
    }

    return controlPolygonLen;
}

void LBezier::TSplit(LBezier& Output, double t)
{
    double s    = 1.0 - t;
    double ss   = s * s;
    double tt   = t * t;

    Output.p[0]     = p[0];
    Output.p[1]     = p[1] * t + p[0] * s;
    Output.p[2]     = p[0] * ss + p[1] * s * t * 2.0 + p[2] * tt;
    Output.p[3]     = p[0] * s * ss + p[1] * ss * t * 3.0 + 
                      p[2] * s * tt * 3.0 + p[3] * t * tt;

    p[0]        = Output.p[3];
    p[1]        = p[1] * ss + p[2] * s * t * 2.0 + p[3] * tt;
    p[2]        = p[2] * s + p[3] * t;
}

double LBezier::TAtLength(unsigned int& length) const
{
    double t = 0.0;
    double result = 0.0;
    double adjust = 0.0;
    double tempLength = length;
    if (length < 3)
    {
        LDPoint rnd(p[0].GetCPoint());
        adjust = p[0].DistFrom(p[3]) - rnd.DistFrom(p[3]); 
        tempLength += adjust;
    }
    DoBezierPts(*this, 0.5, tempLength, t, result);
    length -= static_cast<int>(result - adjust + 0.5);
    return t;
}

double LBezier::TAtLength(double& length, double error) const
{
    double t = 0.0;
    double result = 0.0;
    double adjust = 0.0;
    double tempLength = length;
    if (length < 3.0)
    {
        LDPoint rnd(p[0].GetCPoint());
        adjust = p[0].DistFrom(p[3]) - rnd.DistFrom(p[3]); 
        tempLength += adjust;
    }
    DoBezierPts(*this, 0.5, tempLength, t, result);
    length -= static_cast<int>(result - adjust + 0.5);
    return t;
}

// Create points to simulate ellipse using beziers
void EllipseToBezier(CRect& r, CPoint* cCtlPt)
{
    const double EToBConst = 0.2761423749154; 
    //  2/3*(sqrt(2)-1) 
    // error = +0.027%  - 0.0%
    CSize offset((int)(r.Width() * EToBConst), (int)(r.Height() * EToBConst));
    CPoint centre((r.left + r.right) / 2, (r.top + r.bottom) / 2);

    cCtlPt[0].x  =                                      //------------------------/
    cCtlPt[1].x  =                                      //                        /
    cCtlPt[11].x =                                      //        2___3___4       /
    cCtlPt[12].x = r.left;                              //     1             5    /
    cCtlPt[5].x  =                                      //     |             |    /
    cCtlPt[6].x  =                                      //     |             |    /
    cCtlPt[7].x  = r.right;                             //     0,12          6    /
    cCtlPt[2].x  =                                      //     |             |    /
    cCtlPt[10].x = centre.x - offset.cx;                //     |             |    /
    cCtlPt[4].x  =                                      //    11             7    /
    cCtlPt[8].x  = centre.x + offset.cx;                //       10___9___8       /
    cCtlPt[3].x  =                                      //                        /
    cCtlPt[9].x  = centre.x;                            //------------------------*

    cCtlPt[2].y  =
    cCtlPt[3].y  =
    cCtlPt[4].y  = r.top;
    cCtlPt[8].y  =
    cCtlPt[9].y  =
    cCtlPt[10].y = r.bottom;
    cCtlPt[7].y  =
    cCtlPt[11].y = centre.y + offset.cy;
    cCtlPt[1].y  =
    cCtlPt[5].y  = centre.y - offset.cy;
    cCtlPt[0].y  =
    cCtlPt[12].y =
    cCtlPt[6].y  = centre.y;
}


namespace
{
static int recurse = 0;
void DoBezierPts(const LBezier& V, double error, double& len, double& t, double& result)
{
//  TRACE0("BezierPts\r\n");
    if (len == 0.0)
    {   
        TRACE0("!!!!!!!!!!!!!!!!!!WARNING!!!!!!!!!!!!!");
        return;
    }

    // control polygon length
    double controlPolygonLen = 0.0;
    for (int i = 0; i <= 2; i++)
        controlPolygonLen += V.p[i].DistFrom(V.p[i+1]);

    // chord length
    double chordLen = V.p[0].DistFrom(V.p[3]);

    // call self recursively until accurate enough
    if(controlPolygonLen - chordLen > error || 
        controlPolygonLen + result > len + error)
    {

        // split in two
        LBezier left, right;
        V.Split(left, right);

        // add left and right sides
        ++recurse;
        DoBezierPts(left, error, len, t, result);
        if (len == 0.0l)
        {
            --recurse;
            return;
        }
        DoBezierPts(right, error, len, t, result);
        --recurse;
        return;
    }

    result  += controlPolygonLen;
    t       += 1.0 / (1 << recurse);
//  TRACE3("recurse=%4.d, length =%.5g, result = %.5g\r\n", recurse, controlPolygonLen, result);
    if (fabs(result - len)<=error)
    {
//      TRACE0("\tfinished!!\r\n");
        len = 0.0l;
        return;
    }
}

};
