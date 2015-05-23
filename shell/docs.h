#ifndef __docs_h__
#define __docs_h__

class CShellDocTemplate;
class CShellDocManagerDisp;

class CShellDocManager : public CDocManager
{
	DECLARE_DYNAMIC(CShellDocManager);
public:
	CShellDocManager();
	~CShellDocManager();

	void Init();
	void DeInit();

	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate, CString customFilter);
	virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
			DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate)
	{
		return DoPromptFileName(fileName, nIDSTitle, lFlags, bOpenFileDialog, pTemplate, CString(_T("")));
	}
public:
	void ReloadFiles();

	CShellDocTemplate *GetDocTemplate( const char *lpszTemplateName );
	CShellDocTemplate *GetMatchDocTemplate( const char *lpszFileName );

	CDocument *GetActiveDocument();
	CDocument *GetDocumentByTitle( const char *szDocumentTitle );

	long GetDocTemplateCount();					//size of DocumentTemplate list
	CShellDocTemplate	*GetDocTemplateByPos( long nPos );	//return Document template by position
	
//operation with dispatch implementation
	CShellDocManagerDisp	*GetDispatchImpl() const;
	void SetDispatchImpl( CShellDocManagerDisp	* pimpl );
protected:
	bool	m_bInitialized;
	CCompManager	m_man;
	CShellDocManagerDisp	*m_pDispImpl;
};

//helper class for script engeene
class CShellDocManagerDisp : public CCmdTargetEx,
			public COleEventSourceImpl, public CNamedObjectImpl
{
	ENABLE_MULTYINTERFACE();
	ROUTE_OLE_VIRTUALS();
	
	DECLARE_OLETYPELIB(CShellDocManagerDisp)
	GUARD_DECLARE_OLECREATE(CShellDocManagerDisp)
	DECLARE_DYNCREATE(CShellDocManagerDisp)
public:
	CShellDocManagerDisp();
protected:
	DECLARE_EVENT_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_MESSAGE_MAP()
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellDocManagerDisp)
	public:
	//}}AFX_VIRTUAL
	// Generated message map functions
	//{{AFX_MSG(CShellDocManagerDisp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CShellDocManagerDisp)
	afx_msg LPDISPATCH GetActiveDocumentDisp();
	afx_msg void SetActiveDocumentDisp(LPDISPATCH newValue);
	afx_msg LPDISPATCH GetActiveDocumentByType(LPCTSTR szType);
	afx_msg long GetDocTypesCount();
	afx_msg BSTR GetDocTypeName(long nPos);
	afx_msg LPDISPATCH GetDocument(long nPos, LPCTSTR szType);
	afx_msg long GetDocumentsCount(LPCTSTR szType);
	afx_msg LPDISPATCH GetActiveObject(LPCTSTR lpszProgID);
	afx_msg BSTR GenerateGUID();
	afx_msg BOOL IsPointerNull(const VARIANT& var);
	afx_msg void IdleUpdateActions();
	afx_msg BSTR LoadString(LPCTSTR sz_id);
	afx_msg BSTR LoadStringEx(LPCTSTR sz_id, LPCTSTR sz_sect);
	//}}AFX_DISPATCH
protected:
	CShellDocManager	*m_pDocManager;	//Application document manager pointer. Initialized in constructor

	enum {
	//{{AFX_DISP_ID(CShellDocManagerDisp)
	dispidActiveDocument = 1L,
	dispidGetActiveDocument = 2L,
	dispidGetDocTypesCount = 3L,
	dispidGetDocTypeName = 4L,
	dispidGetDocument = 5L,
	dispidGetDocumentsCount = 6L,
	dispidGetActiveObject = 7L,
	dispidGenerateGUID = 8L,
	dispidIsPointerNull = 9L,
	dispidIdleUpdateActions = 10L,
	dispidLoadString = 11L,
	dispidLoadStringEx = 12L,
	eventidOnOpenDocument = 1L,
	eventidOnSaveDocument = 2L,
	eventidOnCloseDocument = 3L,
	eventidOnNewDocument = 4L,
	eventidOnActivateDocument = 5L,
	eventidOnActivateView = 6L,
	//}}AFX_DISP_ID
	};
public:
	//{{AFX_EVENT(CShellDocManagerDisp)
	void FireOnOpenDocument(LPCTSTR FileName, LPDISPATCH Document)
		{FireEvent(eventidOnOpenDocument,EVENT_PARAM(VTS_BSTR  VTS_DISPATCH), FileName, Document);}
	void FireOnSaveDocument(LPCTSTR FileName, LPDISPATCH Document)
		{FireEvent(eventidOnSaveDocument,EVENT_PARAM(VTS_BSTR  VTS_DISPATCH), FileName, Document);}
	void FireOnCloseDocument(LPDISPATCH Document, VARIANT *bCanClose)
		{FireEvent(eventidOnCloseDocument,EVENT_PARAM(VTS_DISPATCH  VTS_PVARIANT ), Document, bCanClose );}
	void FireOnNewDocument(LPDISPATCH Document)
		{FireEvent(eventidOnNewDocument,EVENT_PARAM(VTS_DISPATCH), Document);}
	void FireOnActivateDocument(LPDISPATCH Document)
		{FireEvent(eventidOnActivateDocument,EVENT_PARAM(VTS_DISPATCH), Document);}
	void FireOnActivateView(LPDISPATCH Document, long nRow, long nCol, BOOL bActivate)
		{FireEvent(eventidOnActivateView,EVENT_PARAM(VTS_DISPATCH VTS_I4 VTS_I4 VTS_BOOL), Document, nRow, nCol, bActivate);}
	//}}AFX_EVENT

	char	sz_string_file[MAX_PATH];
	CDocument *GetActiveDocument();

};

class CShellDocTemplate : public CMultiDocTemplate
{
	DECLARE_DYNAMIC(CShellDocTemplate);
	ENABLE_MULTYINTERFACE();
public:
	IUnknown*			m_punkTemplate;
protected:
	CCompManager		m_manFilters;
	COleTemplateServer m_server;

	bool	m_bLoaded;

	DWORD	m_dwFlags;

	CString	m_strLastDocumentFileName;
	CDocument	*m_pActiveDocument;
public:
	CShellDocTemplate( IUnknown *punk );
	~CShellDocTemplate();

	void SaveState();

	virtual void LoadTemplate();
	virtual CDocument* CreateNewDocument();
	virtual CFrameWnd* CreateNewFrame(CDocument* pDoc, CFrameWnd* pOther);
	virtual Confidence MatchDocType(LPCTSTR lpszPathName, CDocument*& rpDocMatch);
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszPathName, BOOL bMakeVisible = TRUE);
public:
	bool IsSingleDocument()const{return (m_dwFlags & dtfSingleDocumentOnly) != 0; }
	bool IsReloadAtStartup()const{return (m_dwFlags & dtfReopenLastAtStartup) != 0; }
	bool IsAutoSave()const{return (m_dwFlags & dtfAutoSave) != 0; }
public:
	void SetLastDocumentFileName( const char *sz );
	const char *GetLastDocumentFileName();

	CString	GetTemplateName();
	virtual Confidence GetFileFilter( const char *pszFileName, bool bExcludeAlreadyOpen = false, IFileFilter **ppf = 0 );
	void AppendFilterSuffix( CString &strFilter, OPENFILENAME *pofn = 0, CString *pstrDefault = 0, bool bOpenFile = false);
	
	long GetDocumentCount();						//return count of documents
	CDocument	*GetDocumentByPos( long nPos );		//return document by no
	CDocument	*GetActiveDocument();				//return last active document of this template
	void SetActiveDocument( CDocument *pActivateDoc, bool bActivateWindow = false );	//setup the active document

	virtual void AddDocument(CDocument* pDoc);
	virtual void RemoveDocument(CDocument* pDoc);
	virtual POSITION GetFirstDocPosition() const;
	virtual CDocument* GetNextDoc(POSITION& rPos) const;
	virtual void SetDefaultTitle(CDocument* pDocument);

	// check this template contains filefilter
	bool ContainsFilter(LPCTSTR szFilterName);

protected:  // standard implementation
	CDocument* m_pOnlyDoc;
	DECLARE_INTERFACE_MAP();

	BEGIN_INTERFACE_PART(ShellTempl, IShellDocTemplate)
		com_call GetFileFilterByFile(BSTR bstr, IUnknown **ppunk );
	END_INTERFACE_PART(ShellTempl)
};

#endif //__docs_h__
