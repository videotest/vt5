// ActionScissors.h: interface for the CActionScissors class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONSCISSORS_H__59D50FC4_2139_4C9A_B955_2E0A5F0EF834__INCLUDED_)
#define AFX_ACTIONSCISSORS_H__59D50FC4_2139_4C9A_B955_2E0A5F0EF834__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ActionCutArea.h"

class CActionScissors : public CActionCutArea  
{
	DECLARE_DYNCREATE(CActionScissors)
	GUARD_DECLARE_OLECREATE(CActionScissors)

	ENABLE_UNDO();
public:
	CActionScissors();
	virtual ~CActionScissors();

};

#endif // !defined(AFX_ACTIONSCISSORS_H__59D50FC4_2139_4C9A_B955_2E0A5F0EF834__INCLUDED_)
