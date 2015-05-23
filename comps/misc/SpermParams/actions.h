#ifndef __actions_h__
#define __actions_h__

#include "CompManager.h"
#include "ObListWrp.h"
#include "resource.h"

class CRectCtrl;
class CFixedRectCtrl;
class CFixedEllipseCtrl;
class CEditorCtrl;

//[ag]1. classes

class CActionCalcPartResult : public CActionBase
{
	DECLARE_DYNCREATE(CActionCalcPartResult)
	GUARD_DECLARE_OLECREATE(CActionCalcPartResult)

public:
	CActionCalcPartResult();
	virtual ~CActionCalcPartResult();

public:
	IUnknown* GetActiveList(LPCTSTR szObjectType);
	virtual bool Invoke();
//update interface
	virtual bool IsAvaible();

protected:
	CObjectListWrp	m_listObjects;		// active object list
	CCompManager	m_manGroup;			// manager for groups

	IUnknown *GetContextObject(_bstr_t cName, _bstr_t cType);
};


#endif //__actions_h__
