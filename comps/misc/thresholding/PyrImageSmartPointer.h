// PyrImageSmartPointer.h: interface for the CPyrImageSmartPointer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PYRIMAGESMARTPOINTER_H__029CDB9C_2D24_4640_BAB3_5AB545BF1E78__INCLUDED_)
#define AFX_PYRIMAGESMARTPOINTER_H__029CDB9C_2D24_4640_BAB3_5AB545BF1E78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "PyrImage.h"
class CPyrImageSmartPointer  
{
public:
	long m_size;
	CPyrImage* ptr;
	CPyrImageSmartPointer();
	CPyrImageSmartPointer(long size);
	virtual ~CPyrImageSmartPointer();

};

#endif // !defined(AFX_PYRIMAGESMARTPOINTER_H__029CDB9C_2D24_4640_BAB3_5AB545BF1E78__INCLUDED_)
