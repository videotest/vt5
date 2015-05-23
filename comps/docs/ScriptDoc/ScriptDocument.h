#if !defined(AFX_SCRIPTDOCUMENT_H__A66BA364_0F48_11D3_A5BD_0000B493FF1B__INCLUDED_)
#define AFX_SCRIPTDOCUMENT_H__A66BA364_0F48_11D3_A5BD_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ScriptDocument.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CScriptDocument command target
#include "CCrystalTextBuffer.h"
#include "scriptdocint.h"


class CScriptDocument : public CDocBase
{
	DECLARE_DYNCREATE(CScriptDocument)
	GUARD_DECLARE_OLECREATE(CScriptDocument)
public:
	CScriptDocument();           // protected constructor used by dynamic creation
	~CScriptDocument();
// Attributes

	virtual bool OnNewDocument();	//create the required dataobjects here
public:
	DWORD	GetDocFlags(){return dfHasOwnFormat;}

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

// Operations
public:
	/*virtual bool OnNewDocument();
	virtual bool OnOpenDocument( const char *psz );
	virtual bool OnSaveDocument( const char *psz );*/

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CScriptDocument)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CScriptDocument)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CScriptDocument)
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCRIPTDOCUMENT_H__A66BA364_0F48_11D3_A5BD_0000B493FF1B__INCLUDED_)
