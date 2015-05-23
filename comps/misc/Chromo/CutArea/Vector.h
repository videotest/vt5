// Vector.h: interface for the CVector class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_VECTOR_H__975EA10B_75CF_47EB_ACC5_2C1B22563079__INCLUDED_)
#define AFX_VECTOR_H__975EA10B_75CF_47EB_ACC5_2C1B22563079__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CVector  
{
	double m_values[3];
public:
	void _normalize();
	CVector();

	CVector(double x, double y);
	CVector(double x, double y, double z);

	CVector(const CVector &vect);
	virtual ~CVector();

	double &operator[](int i) { return m_values[i]; }
	void operator=(CVector vect);
};

#endif // !defined(AFX_VECTOR_H__975EA10B_75CF_47EB_ACC5_2C1B22563079__INCLUDED_)
