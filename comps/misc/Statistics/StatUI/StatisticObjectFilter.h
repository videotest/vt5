#pragma once

#include "file_main.h"

namespace ObjectSpace
{
class CStatisticObjectFilter : public CFileFilterBase, public _dyncreate_t<CStatisticObjectFilter>
{
	route_unknown();
public:
	CStatisticObjectFilter();
	~CStatisticObjectFilter();

	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool _InitNew();
};
}