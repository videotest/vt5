#pragma once

#include "filterbase.h"
#include "image5.h"
#include "misc_utils.h"

#include "nameconsts.h"
#include "clone.h"
#include "clonedata.h"
#include "resource.h"
#include "binimageint.h"

class CTestPlate: public CActionBase
{
	DECLARE_DYNCREATE( CTestPlate )
	GUARD_DECLARE_OLECREATE(CTestPlate)
public:
	CTestPlate(void);
	virtual ~CTestPlate(void);
	//virtual bool InvokeFilter();
	//virtual bool CanDeleteArgument( CFilterArgument *pa ) {return false;};
	virtual bool Invoke();
private:
	IUnknown* GetContextObject(CString cName, CString cType);
};
