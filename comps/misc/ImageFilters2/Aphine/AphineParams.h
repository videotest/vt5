// AphineParams.h: interface for the CAphineParams class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_APHINEPARAMS_H__AABCA681_212E_11D5_8F84_008048FD42FE__INCLUDED_)
#define AFX_APHINEPARAMS_H__AABCA681_212E_11D5_8F84_008048FD42FE__INCLUDED_

#include "Matrix2x2.h"	// Added by ClassView
#include "Vector2D.h"	// Added by ClassView
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CAphineParams
{
public:
  CAphineParams CAphineParams::operator ~(void);
	CVector2D center;
	CVector2D center_new;
	CMatrix2x2 a;
	CAphineParams();
	CAphineParams(CMatrix2x2 aa, CVector2D acenter, CVector2D acenter_new);
	virtual ~CAphineParams();
  friend CVector2D operator *(CAphineParams &a, CVector2D &b);
};

#endif // !defined(AFX_APHINEPARAMS_H__AABCA681_212E_11D5_8F84_008048FD42FE__INCLUDED_)
