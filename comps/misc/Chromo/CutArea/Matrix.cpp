// Matrix.cpp: implementation of the CMatrix class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CutArea.h"
#include "Matrix.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMatrix::CMatrix()
{
	for(int i = 0;i < 4;i++)
		for(int j = 0;j < 4;j++)
			m_values[i][j] = 0;

}

CMatrix::~CMatrix()
{

}

CMatrix::CMatrix(const CMatrix &mt)
{
	for(int i = 0;i < 4;i++)
		for(int j = 0;j < 4;j++)
			m_values[i][j] = mt.m_values[i][j];
}


CMatrix CMatrix::Invert()
{
	CMatrix ret;

    double fDetInv = ( m_values[0][0] * ( m_values[1][1] * m_values[2][2] - m_values[1][2] * m_values[2][1] ) -
                             m_values[0][1] * ( m_values[1][0] * m_values[2][2] - m_values[1][2] * m_values[2][0] ) +
                             m_values[0][2] * ( m_values[1][0] * m_values[2][1] - m_values[1][1] * m_values[2][0] ) );


	if( fabs( fDetInv ) < EPS )

	fDetInv = 1 / fDetInv;

    ret.m_values[0][0] =  fDetInv * ( m_values[1][1] * m_values[2][2] - m_values[1][2] * m_values[2][1] );
    ret.m_values[0][1] = -fDetInv * ( m_values[0][1] * m_values[2][2] - m_values[0][2] * m_values[2][1] );
    ret.m_values[0][2] =  fDetInv * ( m_values[0][1] * m_values[1][2] - m_values[0][2] * m_values[1][1] );
    ret.m_values[0][3] = 0.0f;

    ret.m_values[1][0] = -fDetInv * ( m_values[1][0] * m_values[2][2] - m_values[1][2] * m_values[2][0] );
    ret.m_values[1][1] =  fDetInv * ( m_values[0][0] * m_values[2][2] - m_values[0][2] * m_values[2][0] );
    ret.m_values[1][2] = -fDetInv * ( m_values[0][0] * m_values[1][2] - m_values[0][2] * m_values[1][0] );
    ret.m_values[1][3] = 0.0f;

    ret.m_values[2][0] =  fDetInv * ( m_values[1][0] * m_values[2][1] - m_values[1][1] * m_values[2][0] );
    ret.m_values[2][1] = -fDetInv * ( m_values[0][0] * m_values[2][1] - m_values[0][1] * m_values[2][0] );
    ret.m_values[2][2] =  fDetInv * ( m_values[0][0] * m_values[1][1] - m_values[0][1] * m_values[1][0] );
    ret.m_values[2][3] = 0.0f;

    ret.m_values[3][0] = -( m_values[3][0] * ret.m_values[0][0] + m_values[3][1] * ret.m_values[1][0] + m_values[3][2] * ret.m_values[2][0] );
    ret.m_values[3][1] = -( m_values[3][0] * ret.m_values[0][1] + m_values[3][1] * ret.m_values[1][1] + m_values[3][2] * ret.m_values[2][1] );
    ret.m_values[3][2] = -( m_values[3][0] * ret.m_values[0][2] + m_values[3][1] * ret.m_values[1][2] + m_values[3][2] * ret.m_values[2][2] );
    ret.m_values[3][3] = 1.0f;

    return ret;
}

