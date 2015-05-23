#ifndef __JPGFilter_h__
#define __JPGFilter_h__
/*
#ifdef _DEBUG
// {3BA9D86D-3019-4798-810A-402D35AA70F5}
static const GUID clsidJPGFilter = 
{ 0x3ba9d86d, 0x3019, 0x4798, { 0x81, 0xa, 0x40, 0x2d, 0x35, 0xaa, 0x70, 0xf5 } };

#define pszJPGFilterProgID "FileFilters2.JPGFilterD"
#else
*/
// {69FE8EDF-DDFD-44a0-A1C8-1DE1C17BF54E}
static const GUID clsidJPGFilter = 
{ 0x69fe8edf, 0xddfd, 0x44a0, { 0xa1, 0xc8, 0x1d, 0xe1, 0xc1, 0x7b, 0xf5, 0x4e } };

#define pszJPGFilterProgID "FileFilters2.JPGFilter"
/*
#endif
*/

#include "FilterPropBase.h"

class CJPGFilter : public CFilterPropBase,
				  public _dyncreate_t<CJPGFilter>	
{
public:
	CJPGFilter();
	virtual ~CJPGFilter();

	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool _InitNew();
};


#endif

