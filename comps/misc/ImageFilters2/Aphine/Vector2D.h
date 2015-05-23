// Vector2D.h: interface for the CVector2D class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR2D_H__AABCA682_212E_11D5_8F84_008048FD42FE__INCLUDED_)
#define AFX_VECTOR2D_H__AABCA682_212E_11D5_8F84_008048FD42FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Matrix2x2.h"

class CVector2D  
{
public:
	CVector2D();
	virtual ~CVector2D();
	CVector2D(double ax, double ay);
	CVector2D operator+(CVector2D &b);
	CVector2D operator-(CVector2D &b);
	CVector2D operator-();
	double operator*(CVector2D &b); //dot product
  friend CVector2D operator *(CMatrix2x2 &a, CVector2D &b); //multiply by matrix
  double x,y;
CVector2D  &operator=(const CVector2D  &p2 )
{
	x = p2.x;
	y = p2.y;
	return *this;
}
};


#endif // !defined(AFX_VECTOR2D_H__AABCA682_212E_11D5_8F84_008048FD42FE__INCLUDED_)
