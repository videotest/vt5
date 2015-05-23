#pragma once


class CBranchSplit : public CFilterBase
{
	DECLARE_DYNCREATE(CBranchSplit)
	GUARD_DECLARE_OLECREATE(CBranchSplit)

public:
	CBranchSplit(void);
	~CBranchSplit(void);

public:
	virtual bool InvokeFilter();
};
