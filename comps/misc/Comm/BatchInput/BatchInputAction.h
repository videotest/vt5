#if !defined(__BatchInputAction_H__)
#define __BatchInputAction_H__

class CActionBatchInput : public CActionBase
{
	DECLARE_DYNCREATE(CActionBatchInput)
	GUARD_DECLARE_OLECREATE(CActionBatchInput)
public:
	CActionBatchInput();
	~CActionBatchInput();
public:
	
	virtual bool Invoke();
};

#endif
