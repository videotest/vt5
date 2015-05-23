#pragma once

// ResizeBin

class CResizeBin : public CFilterBase
{
	DECLARE_DYNCREATE(CResizeBin)
	GUARD_DECLARE_OLECREATE(CResizeBin)
public:

	CResizeBin();
	virtual bool InvokeFilter();
};
