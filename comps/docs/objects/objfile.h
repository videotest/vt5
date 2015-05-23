#ifndef __objfile_h__
#define __objfile_h__

class CObjectFileFilter : public CFileFilterBase
{
	DECLARE_DYNCREATE(CObjectFileFilter);
	GUARD_DECLARE_OLECREATE(CObjectFileFilter);
public:
	CObjectFileFilter();
	~CObjectFileFilter();

	virtual bool InitNew(){return true;}
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
};

#endif // __objfile_h__
