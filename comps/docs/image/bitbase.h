#ifndef __BITBASE_H__
#define __BITBASE_H__

#include "action_filter.h"

enum BitOperationType
{
	botNot,
	botOr,
	botAnd,
	botXor
};

class CBitFilter : public CFilter
{
public:
	CBitFilter();
	
protected:
	virtual bool InvokeFilter();

	BitOperationType m_nBitOperationType;
};

#endif //__BITBASE_H__