//[ag]1. classes

class CActionEven : public CFilterBase
{
	DECLARE_DYNCREATE(CActionEven)
	GUARD_DECLARE_OLECREATE(CActionEven)

public:
	CActionEven();
	virtual ~CActionEven();

public:
	virtual bool InvokeFilter();
};

