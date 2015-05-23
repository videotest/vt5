#ifndef __WoolzFilter_h__
#define __WoolzFilter_h__

// {82A9886B-4FEF-4457-B36E-8DAA4A4C2609}
static const GUID clsidWoolzFilter = 
{ 0x82a9886b, 0x4fef, 0x4457, { 0xb3, 0x6e, 0x8d, 0xaa, 0x4a, 0x4c, 0x26, 0x9 } };

#define pszWoolzFilterProgID "ChromExtrn.WoolzFilter"

#include "FilterPropBase.h"

class CWoolzFilter : public CFilterPropBase,
				  public _dyncreate_t<CWoolzFilter>	
{
public:
	CWoolzFilter();
	virtual ~CWoolzFilter();

	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool _InitNew();
};


#endif