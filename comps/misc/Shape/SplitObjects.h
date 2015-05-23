#pragma once
#include "filterbase.h"

class CSplitObjects :
	public CFilterBase
{
	DECLARE_DYNCREATE(CSplitObjects)
	GUARD_DECLARE_OLECREATE(CSplitObjects)
public:
	CSplitObjects(void);
	~CSplitObjects(void);

	virtual bool InvokeFilter();
};
