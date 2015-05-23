// Matrix.h: interface for the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MATRIX_H__23ECDE7A_0B50_4044_9D92_71F417565EAB__INCLUDED_)
#define AFX_MATRIX_H__23ECDE7A_0B50_4044_9D92_71F417565EAB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMatrix  
{
	double m_values[4][4];
public:
	CMatrix();
	virtual ~CMatrix();

	CMatrix(const CMatrix &mt);
	double *operator[](int i) { return m_values[i]; }

	CMatrix Invert();
};

#endif // !defined(AFX_MATRIX_H__23ECDE7A_0B50_4044_9D92_71F417565EAB__INCLUDED_)
