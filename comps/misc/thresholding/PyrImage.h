// PyrImage.h: interface for the CPyrImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PYRIMAGE_H__C986EBE9_508A_4D7F_9953_683C86947339__INCLUDED_)
#define AFX_PYRIMAGE_H__C986EBE9_508A_4D7F_9953_683C86947339__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "PyrAreaPoint.h"

class CPyrImage  
{
public:
	bool CreateNewPyrImage( SIZE size, int level );
	CPyrAreaPoint* m_pPoints;
	SIZE m_size;
	int m_nLevel;
	CPyrImage();
	virtual ~CPyrImage();

};

#endif // !defined(AFX_PYRIMAGE_H__C986EBE9_508A_4D7F_9953_683C86947339__INCLUDED_)
