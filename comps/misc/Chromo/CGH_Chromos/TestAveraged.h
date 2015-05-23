#pragma once

#include "filterbase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

class CTestAveraged: public CFilterBase
{
public:
	DECLARE_DYNCREATE( CTestAveraged )
	GUARD_DECLARE_OLECREATE( CTestAveraged )
	CTestAveraged(void);
	virtual ~CTestAveraged(void);
	virtual bool InvokeFilter();
	virtual bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
};
