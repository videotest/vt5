#pragma once

class CActionRunTestWindow : public CActionBase
{
	DECLARE_DYNCREATE(CActionRunTestWindow)
	GUARD_DECLARE_OLECREATE(CActionRunTestWindow)
public:
	CActionRunTestWindow(void);
	~CActionRunTestWindow(void);
public:
	virtual bool Invoke();
};
