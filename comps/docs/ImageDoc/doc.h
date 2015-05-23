#ifndef __doc_h__
#define __doc_h__

class CImageDocument : public CDocBase
{
	DECLARE_DYNCREATE(CImageDocument);
	GUARD_DECLARE_OLECREATE(CImageDocument);
	DECLARE_INTERFACE_MAP();
public:
	CImageDocument();
protected:
	virtual bool OnNewDocument();
};

class CImageDocTemplate : public CDocTemplateBase
{
	DECLARE_DYNCREATE(CImageDocTemplate);
	GUARD_DECLARE_OLECREATE(CImageDocTemplate);
public:
	CImageDocTemplate();
protected:
	virtual CString GetDocTemplString();
};

#endif //__doc_h__