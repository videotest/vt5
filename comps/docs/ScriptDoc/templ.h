#ifndef __templ_h__
#define __templ_h__

#include "scriptdocument.h"

class CScriptDocTemplate : public CDocTemplateBase
{
	DECLARE_DYNCREATE(CScriptDocTemplate);
	GUARD_DECLARE_OLECREATE(CScriptDocTemplate);
public:
	virtual CString GetDocTemplString();
	virtual DWORD GetDocFlags()
	{	return dtfSingleDocumentOnly;}
public:
	static CString s_strProgID;
};

#endif //__templ_h__