#pragma once

#include "file_main.h"

//////////////////////////////////////////////////////////////////////
//
//	class CStatTableFilter
//
//////////////////////////////////////////////////////////////////////
class CStatTableFilter :	public CFileFilterBase,
							public _dyncreate_t<CStatTableFilter>
{
	route_unknown();
public:
	CStatTableFilter();
	virtual ~CStatTableFilter();

	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool _InitNew();
};
