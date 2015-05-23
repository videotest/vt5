// PyrImageSmartPointer.cpp: implementation of the CPyrImageSmartPointer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PyrImageSmartPointer.h"
#include "PyrAreaPoint.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPyrImageSmartPointer::CPyrImageSmartPointer()
{
	ptr = 0;
	m_size = 0;
}

CPyrImageSmartPointer::~CPyrImageSmartPointer()
{
	for(long i = 0; i<m_size; i++)
	{
		if(ptr[i].m_pPoints)
		{
//			CPyrAreaPoint* ptemp = ptr[i].m_pPoints;
			delete [] ptr[i].m_pPoints;
			ptr[i].m_pPoints = 0;
		}
	}
	if(ptr)
		delete [] ptr; ptr=0;
}

CPyrImageSmartPointer::CPyrImageSmartPointer(long size)
{
	ptr = new CPyrImage[size];
	m_size = size;
}