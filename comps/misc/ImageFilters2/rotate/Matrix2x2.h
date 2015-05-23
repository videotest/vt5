// Matrix2x2.h: interface for the CMatrix2x2 class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX2X2_H__FB25E486_1F10_11D5_8F84_008048FD42FE__INCLUDED_)
#define AFX_MATRIX2X2_H__FB25E486_1F10_11D5_8F84_008048FD42FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMatrix2x2  
{
public:
	CMatrix2x2 operator+(CMatrix2x2 &b);
	CMatrix2x2 operator*(CMatrix2x2 &b);
	CMatrix2x2 operator~(void);
	double a11,a12,a21,a22;
	CMatrix2x2();
	CMatrix2x2(double b11,double b12,double b21,double b22);
	virtual ~CMatrix2x2();
};

#endif // !defined(AFX_MATRIX2X2_H__FB25E486_1F10_11D5_8F84_008048FD42FE__INCLUDED_)
