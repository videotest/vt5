#pragma once

class CFishByClustering : public CFilterBase
{
public:
	DECLARE_DYNCREATE( CFishByClustering )
	GUARD_DECLARE_OLECREATE( CFishByClustering )
	CFishByClustering(void);
	virtual ~CFishByClustering(void);
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
};
