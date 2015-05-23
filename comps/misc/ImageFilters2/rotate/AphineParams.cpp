// AphineParams.cpp: implementation of the CAphineParams class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AphineParams.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAphineParams::CAphineParams()
{

}

CAphineParams::CAphineParams(CMatrix2x2 aa, CVector2D acenter, CVector2D acenter_new)
{
  a=aa;
  center=acenter;
  center_new=acenter_new;
}

CAphineParams::~CAphineParams()
{

}

CAphineParams CAphineParams::operator ~(void)
{
  return(CAphineParams(~a,center_new,center));
}

CVector2D operator *(CAphineParams &a, CVector2D &b)
{
  return(a.a*(b-a.center)+a.center_new);
}
