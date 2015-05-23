// ActionMergeCGH.h: interface for the CActionSetCGHPane class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONMERGECGH_H__7D51CC27_E241_494A_9CD7_78B3B2235FC7__INCLUDED_)
#define AFX_ACTIONMERGECGH_H__7D51CC27_E241_494A_9CD7_78B3B2235FC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OperationCGHBase.h"

class CActionSetPane : public COperationBase
{
	DECLARE_DYNCREATE( CActionSetPane )
	GUARD_DECLARE_OLECREATE(CActionSetPane)
public:
	CActionSetPane();
	virtual ~CActionSetPane();
public:
	bool IsAvaible();
	bool Initialize();
	bool Invoke();
};

class CActionRemovePane : public COperationBase
{
	DECLARE_DYNCREATE( CActionRemovePane )
	GUARD_DECLARE_OLECREATE( CActionRemovePane )
public:
	CActionRemovePane();
	virtual ~CActionRemovePane();
public:
	bool IsAvaible();
	bool Initialize();
	bool Invoke();
};

#endif // !defined(AFX_ACTIONMERGECGH_H__7D51CC27_E241_494A_9CD7_78B3B2235FC7__INCLUDED_)
