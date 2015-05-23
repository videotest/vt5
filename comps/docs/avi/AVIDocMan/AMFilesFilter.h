#ifndef __AMFilesFilter_h__
#define __AMFilesFilter_h__

#include "Common.h"

class CAMFilesFilter : public CFileFilterBase
{
public:
	CAMFilesFilter();
protected:
	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual int  GetPropertyCount();
	virtual void GetPropertyByNum(int nNum, VARIANT *pvarVal, BSTR *pbstrName, DWORD* pdwFlags, DWORD* pGroupIdx);
};

class CAVIFilesFilter : public CAMFilesFilter
{
	DECLARE_DYNCREATE(CAVIFilesFilter);
	GUARD_DECLARE_OLECREATE(CAVIFilesFilter);
public:
	CAVIFilesFilter();
};

class CMOVFilesFilter : public CAMFilesFilter
{
	DECLARE_DYNCREATE(CMOVFilesFilter);
	GUARD_DECLARE_OLECREATE(CMOVFilesFilter);
public:
	CMOVFilesFilter();
};

class CMPGFilesFilter : public CAMFilesFilter
{
	DECLARE_DYNCREATE(CMPGFilesFilter);
	GUARD_DECLARE_OLECREATE(CMPGFilesFilter);
public:
	CMPGFilesFilter();
};



#endif //__AMFilesFilter_h__