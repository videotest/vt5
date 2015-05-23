// PyrAreaPoint.h: interface for the CPyrAreaPoint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PYRAREAPOINT_H__7C774A5E_C9B8_44A7_BE6E_7095AA43B037__INCLUDED_)
#define AFX_PYRAREAPOINT_H__7C774A5E_C9B8_44A7_BE6E_7095AA43B037__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "misc_utils.h"

//#include "nameconsts.h"

class CPyrAreaPoint  
{
public:
	int m_nCount;
	long m_nSum;
	CPyrAreaPoint* m_ConnectedPoint;
	color m_nColor;
	CPyrAreaPoint();
	virtual ~CPyrAreaPoint();

};

#endif // !defined(AFX_PYRAREAPOINT_H__7C774A5E_C9B8_44A7_BE6E_7095AA43B037__INCLUDED_)
