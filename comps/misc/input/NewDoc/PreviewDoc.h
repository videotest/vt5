#if !defined(__PreviewDoc_H__)
#define __PreviewDoc_H__

#include "Common.h"

class CPreviewDocument : public CDocBase
{
	DECLARE_DYNCREATE(CPreviewDocument);
	GUARD_DECLARE_OLECREATE(CPreviewDocument);
	DECLARE_INTERFACE_MAP();
public:
	CPreviewDocument();
protected:
	virtual bool OnNewDocument();
};

class CPreviewDocTemplate : public CDocTemplateBase
{
	DECLARE_DYNCREATE(CPreviewDocTemplate);
	GUARD_DECLARE_OLECREATE(CPreviewDocTemplate);
public:
	CPreviewDocTemplate();
protected:
	virtual CString GetDocTemplString();
};

#endif