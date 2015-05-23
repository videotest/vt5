// Vector.cpp: implementation of the CVector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CutArea.h"
#include "Vector.h"
#include <math.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVector::CVector()
{
	for(int i = 0;i < 3;i++)
		m_values[i] = 0;
}

CVector::~CVector()
{
}

CVector::CVector(double x, double y,double z)
{
	m_values[0] = x;
	m_values[1] = y;
	m_values[2] = z;

}

CVector::CVector(double x, double y)
{
	m_values[0] = x;
	m_values[1] = y;
	m_values[2] = 0;
}

CVector::CVector(const CVector &vect)
{
	for(int i = 0;i < 3;i++)
		m_values[i] = vect.m_values[i];
}

void CVector::operator=(CVector vect)
{
	for(int i = 0;i < 3;i++)
		m_values[i] = vect.m_values[i];
}

void CVector::_normalize()
{
	double len = _hypot( m_values[0], m_values[1] );

	if( fabs( len ) < EPS )
		len = 1; 

	for(int i = 0;i < 3;i++)
		m_values[i] /= len;
}
