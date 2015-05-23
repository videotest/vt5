// QueryFile.h: interface for the CQueryFileFilter class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUERYFILE_H__DBF59F19_7894_4C27_BF8C_161359B8E271__INCLUDED_)
#define AFX_QUERYFILE_H__DBF59F19_7894_4C27_BF8C_161359B8E271__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueryFileFilter : public CFileFilterBase  
{
	DECLARE_DYNCREATE(CQueryFileFilter);
	GUARD_DECLARE_OLECREATE(CQueryFileFilter);
public:
	CQueryFileFilter();
	virtual ~CQueryFileFilter();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );

	virtual bool BeforeReadFile();
	virtual bool AfterReadFile();
};

#endif // !defined(AFX_QUERYFILE_H__DBF59F19_7894_4C27_BF8C_161359B8E271__INCLUDED_)
