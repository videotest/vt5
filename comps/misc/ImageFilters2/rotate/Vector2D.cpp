// Vector2D.cpp: implementation of the CVector2D class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Vector2D.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVector2D::CVector2D()
{
  x=0; y=0;
}

CVector2D::~CVector2D()
{

}

CVector2D::CVector2D(double ax, double ay)
{
  x=ax; y=ay;
}

CVector2D CVector2D::operator +(CVector2D &b)
{
  return(CVector2D(x+b.x,y+b.y));
}

CVector2D CVector2D::operator -(CVector2D &b)
{
  return(CVector2D(x-b.x,y-b.y));
}

CVector2D CVector2D::operator -()
{
  return(CVector2D(-x,-y));
}

double CVector2D::operator *(CVector2D &b)
{
  return(x*b.x + y*b.y);
}

CVector2D operator *(CMatrix2x2 &a, CVector2D &b) //multiply by matrix
{
  return(CVector2D(a.a11*b.x + a.a12*b.y,
                   a.a21*b.x + a.a22*b.y));
}
