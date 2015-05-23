// Matrix2x2.cpp: implementation of the CMatrix2x2 class.
//
//////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "Matrix2x2.h"
#include "math.h"
//#include "common.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatrix2x2::CMatrix2x2()
{
  a11=a22=1;
  a12=a21=0;
}

CMatrix2x2::CMatrix2x2(double b11,double b12,double b21,double b22)
{
  a11=b11; a12=b12;
  a21=b21; a22=b22;
}

CMatrix2x2::~CMatrix2x2()
{

}

CMatrix2x2 CMatrix2x2::operator +(CMatrix2x2 &b)
{
  return(CMatrix2x2(a11+b.a11,a12+b.a12,
                    a21+b.a21,a22+b.a22));
}

CMatrix2x2 CMatrix2x2::operator *(CMatrix2x2 &b)
{
  return(CMatrix2x2(
    a11*b.a11+a12*b.a21, a11*b.a12+a12*b.a22,
    a21*b.a11+a22*b.a21, a21*b.a12+a22*b.a22));
}

static void dbg_assert( bool bExpression, const char *pszErrorDescription )
{
	if( !bExpression )
	{
		
#ifdef _DEBUG
		DebugBreak();
#endif
		
	}
}

CMatrix2x2 CMatrix2x2::operator ~(void)
{
  double d=a11*a22-a12*a21;
  dbg_assert(fabs(d)>1e-20); //ругнуться на вырожденную матрицу
  return(CMatrix2x2( a22/d, -a12/d,
                    -a21/d,  a11/d));
}
