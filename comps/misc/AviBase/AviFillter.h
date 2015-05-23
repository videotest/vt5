// AviFillter.h: interface for the CAviFillter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVIFILLTER_H__02976AA1_F901_4AC5_8FC3_2C3B30AC2617__INCLUDED_)
#define AFX_AVIFILLTER_H__02976AA1_F901_4AC5_8FC3_2C3B30AC2617__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "file_main.h"

class CAviFillter : public CFileFilterBase,
				  public _dyncreate_t<CAviFillter>	
{
	route_unknown();
public:
	CAviFillter();
	virtual ~CAviFillter();


	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool _InitNew();
	com_call IsFileFormatReadonly( BOOL *pbIsReadOnly );
	com_call ShowSaveNotSupportedMessage();
};

#endif // !defined(AFX_AVIFILLTER_H__02976AA1_F901_4AC5_8FC3_2C3B30AC2617__INCLUDED_)
