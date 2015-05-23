#pragma once
class CIdioDBDocument : public CDocBase
{
	DECLARE_DYNCREATE(CIdioDBDocument);
	GUARD_DECLARE_OLECREATE(CIdioDBDocument);
	DECLARE_INTERFACE_MAP();
public:
	CIdioDBDocument();
	DWORD	GetDocFlags(){return dfHasOwnFormat;}
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
protected:
	virtual bool OnNewDocument();
	void OnFinalRelease();
	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CIdioDBDocument)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
};
class CIdioDBTemplate : public CDocTemplateBase
{
	DECLARE_DYNCREATE(CIdioDBTemplate);
	GUARD_DECLARE_OLECREATE(CIdioDBTemplate);
public:
	CIdioDBTemplate();
protected:
	virtual CString GetDocTemplString();
};