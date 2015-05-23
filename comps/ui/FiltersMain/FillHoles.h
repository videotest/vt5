#if !defined(__FillHoles_H__)
#define __FillHoles_H__


class CActionFillHolesBin : public CFilterBase
{
	DECLARE_DYNCREATE(CActionFillHolesBin)
	GUARD_DECLARE_OLECREATE(CActionFillHolesBin)

public:
	CActionFillHolesBin();
	virtual ~CActionFillHolesBin();

public:
	virtual bool InvokeFilter();
};


#endif
