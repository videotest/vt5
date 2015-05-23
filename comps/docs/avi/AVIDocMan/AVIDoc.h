#if !defined(__AVIDoc_H__)
#define __AVIDoc_H__

#include "Common.h"

class CAVIDocument : public CDocBase
{
	DECLARE_DYNCREATE(CAVIDocument);
	GUARD_DECLARE_OLECREATE(CAVIDocument);
	DECLARE_INTERFACE_MAP();
public:
	CAVIDocument();
protected:
	virtual bool OnNewDocument();
	virtual bool OnOpenDocument( const char *psz );
};

class CAVIDocTemplate : public CDocTemplateBase
{
	DECLARE_DYNCREATE(CAVIDocTemplate);
	GUARD_DECLARE_OLECREATE(CAVIDocTemplate);
public:
	CAVIDocTemplate();
protected:
	virtual CString GetDocTemplString();
};


#endif