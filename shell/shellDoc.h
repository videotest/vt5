// shellDoc.h : interface of the CShellDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SHELLDOC_H__C48CE9C3_F947_11D2_A596_0000B493FF1B__INCLUDED_)
#define AFX_SHELLDOC_H__C48CE9C3_F947_11D2_A596_0000B493FF1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CShellDocTemplate;
class CShellView;
class CShellFrame;


class CShellDoc : public CDocument,
				public CCompManagerImpl,
				public CNumeredObjectImpl,
				public CMapInterfaceImpl
{
protected:
	CShellView		*m_pActiveView;
	IUnknown		*m_punkActiveView;
	CPtrList		m_listFrames;
	CAggrManager	m_aggrs;	
	bool			m_bFirstViewInitialized;
	bool			m_bEmptyDoc;
	GUID			m_guidLoadedObject;
	CString			m_strFilterFileName;
	CString			m_strSaving;
	BOOL			m_bLockDoc;
protected: // create from serialization only
	CShellDoc();
	DECLARE_DYNCREATE(CShellDoc)

	ENABLE_MULTYINTERFACE()

	BOOL IsSDIMode();
	virtual BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace = TRUE);
	bool IsNativeFormat(IUnknownPtr &punkFF);
// Attributes
public:
	virtual bool Init();
	virtual void OnFirstViewInitialized();
	virtual void DeInitNotEmpty();


	IDocument *GetDocumentInterface( bool bAddRef = true );
	void EnsureViewRemoved( CView *pViewToRemove );

// Operations
	CShellDocTemplate *GetDocTemplate();	//the Document template associated with document
	CShellView	*GetActiveView();			//the active document view
	void SetActiveView( CShellView *pView );//setup the active document vioew

	void AddFrame( CShellFrame *pFrm );			//register frame in document's list
	void RemoveFrame( CShellFrame *pFrm );		//unregister frame in document's list

	POSITION GetFirstFramePosition();			//returns first position
	CShellFrame *GetNextFrame( POSITION &pos );	//returns frame by position
	afx_msg void OnFileSendMail();

public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CShellDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnFinalRelease();
	virtual void OnCloseDocument();
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL
	virtual void DeleteContents();
	virtual BOOL IsModified();
	virtual BOOL SaveModified();
	virtual void SetModifiedFlag(BOOL bModified);
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE);
	void ResetTemplate()	{m_pDocTemplate = 0;}


// Implementation
public:
	virtual ~CShellDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CShellDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CShellDoc)
	afx_msg long GetViewsCount();
	afx_msg LPDISPATCH GetView(long nPos);
	afx_msg LPDISPATCH _GetActiveView();
	afx_msg long GetFramesCount();
	afx_msg LPDISPATCH GetFrame(long nPos);
	afx_msg LPDISPATCH GetData();
	afx_msg LPDISPATCH GetActiveDataContext();
	afx_msg BSTR GetDocPathName();
	afx_msg LPDISPATCH GetActiveDocumentWindow();	
	afx_msg BOOL dispGetModifiedFlag();
	afx_msg void dispSetModifiedFlag(BOOL bModified);
	afx_msg BSTR GetDocType();
	afx_msg void Reload();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(Site, IDocumentSite2)
		com_call GetDocumentTemplate( IUnknown **punk );
		com_call GetFirstViewPosition(TPOS *plPos);
		com_call GetNextView(IUnknown **ppunkView, TPOS *plPos);
		com_call GetActiveView( IUnknown **ppunkView );
		com_call GetPathName( BSTR *pbstrPathName );
		com_call SetActiveView( IUnknown *punkView );
		com_call GetDocType( BSTR *pbstrDocType );
		com_call SaveDocument( BSTR bstrFileName );
		com_call GetPathName2( BSTR *pbstrPathName, BSTR *pbstrTitle );
		com_call GetSavingPathName( BSTR *pbstrPathName );
		com_call LockDocument( BOOL bLock );
		com_call IsDocumentLocked( BOOL *pbLock );
	END_INTERFACE_PART(Site)

public:
	virtual void FirenNewDocument( const char* psz_file_name );

	//cache interface
	virtual IUnknown* GetInterfaceHook( const void* p );
	virtual IUnknown* raw_get_interface( const GUID* piid );

//	afx_msg void OnFileSendMail1();
};

bool IsNeedPrompt(LPCTSTR lpstrFileName, CDocument *pdoc, CString *pstrExt = NULL);


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SHELLDOC_H__C48CE9C3_F947_11D2_A596_0000B493FF1B__INCLUDED_)
