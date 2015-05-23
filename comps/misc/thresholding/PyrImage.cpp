// PyrImage.cpp: implementation of the CPyrImage class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PyrImage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPyrImage::CPyrImage()
{
	m_nLevel = -1;
	m_pPoints = 0;
	m_size.cx = 0;
	m_size.cy = 0;
}

CPyrImage::~CPyrImage()
{
	/*
	if(m_pPoints)
		delete [] m_pPoints;
		*/
}

bool CPyrImage::CreateNewPyrImage(SIZE size, int level)
{
	if(m_pPoints!=0)
		delete [] m_pPoints;
	m_pPoints = new CPyrAreaPoint[size.cx*size.cy];
//	ZeroMemory(m_pPoints,sizeof(CPyrAreaPoint)*size.cx*size.cy);
	m_nLevel = level;
	m_size = size;
	return true;
}
