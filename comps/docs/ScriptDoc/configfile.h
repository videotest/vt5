#ifndef __configfile_h__
#define __configfile_h__

class CScriptFileFilter : public CFileFilterBase
{
	DECLARE_DYNCREATE(CScriptFileFilter);
	GUARD_DECLARE_OLECREATE(CScriptFileFilter);
protected:
	CString	m_strFilter;
	CString	m_strDocument;

	CScriptFileFilter();
	
protected:
	virtual bool ReadFile( const char *pszFileName );
	virtual bool WriteFile( const char *pszFileName );
	virtual bool InitNew();
};


#endif //__configfile_h__