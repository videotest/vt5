#if !defined(AFX_APPMANAGER_H__B91F7CC7_3B46_11D3_A605_0090275995FE__INCLUDED_)
#define AFX_APPMANAGER_H__B91F7CC7_3B46_11D3_A605_0090275995FE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AppManager.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CAppManager command target

//"Application's" manager. 

class CAppManager : public CCmdTargetEx,
					public CNamedObjectImpl,
					public COleEventSourceImpl
{
	ENABLE_MULTYINTERFACE();

	enum NotifyCodes
	{
		ncMethodicActivate,
		ncApplicationActivate,
	};
//interfal pointer to managet - allow loacl component use CAppManager without interface call
//( or as Application pointer
public:
	static CAppManager	*s_pAppManagerInstance;
	DECLARE_DYNCREATE(CAppManager)
	GUARD_DECLARE_OLECREATE(CAppManager)
	DECLARE_OLETYPELIB(CAppManager)


	CAppManager();           // protected constructor used by dynamic creation

// Attributes
public:
	void Init();	//initialize
	void DeInit();	//de-initialize

// Operations
public:
	//utils classes
	//base class for application and methodoc records. 
	class XAppMethBase
	{
	protected:
		CString	m_strFileName;
		CString m_strTitle;
		BITMAPINFOHEADER	*m_pbi;
	public:
		XAppMethBase();
		virtual ~XAppMethBase();
	
		bool ReadFile();
		bool WriteFile();

		virtual bool LoadFromData( IUnknown *punk );
		virtual bool StoreToData( IUnknown *punk );

		bool ExecutePropertiesDialog();

		//works with application/methodic title
		const char *GetTitle()
		{	return m_strTitle;		}
		void SetTitle( const char *szTitle )
		{	m_strTitle = szTitle;	}

		//works with application/methodic filename
		const char *GetFileName()
		{	return m_strFileName;	}
		void SetFileName( const char *szFileName );	//expand path, if nessesary

		//works with image icon
		BITMAPINFOHEADER *GetImageBI()
		{	return	m_pbi;	}
		void SetImageBI( BITMAPINFOHEADER *pbi )
		{	m_pbi = pbi;	}
	};
	
	class XMethodic : public XAppMethBase
	{
	protected:
		long m_lCount;
	public:
		XMethodic();
		~XMethodic();

		void AddRef(){m_lCount++;}
		long Release()
		{
			if( !--m_lCount )
			{
				delete this;
				return 0;
			}
			return m_lCount;
		}
	};

	class XApplication : public XAppMethBase
	{
	protected:
		CTypedPtrArray<CPtrArray, XAppMethBase*>
			m_arrMethodics;
		long	m_nActiveMeth;
		/*CTypedPtrArray<CPtrArray, XAppMethBase*>
			m_arrMethodics;*/
	public:
		XApplication();
		virtual ~XApplication();

		virtual bool LoadFromData( IUnknown *punk );
		virtual bool StoreToData( IUnknown *punk );

		long GetMethodicsCount();
		XMethodic *GetMethodic( int nPos );

		int AddCreateMethodic( const char *szFileName, bool bNativePath = false );
		void InsertMethodic( int nPos, XMethodic *pM );
		void DeleteMethodic( int nPos );

		void SetLastActiveMethodic( long nPos )
		{	m_nActiveMeth = nPos;	}
		long GetLastActiveMethodic()
		{	return m_nActiveMeth;	}
	};

	CTypedPtrArray<CPtrArray, XAppMethBase*>
		m_ptrApps;

//access to the applications
	XApplication	*GetApp( int nPos )
	{	if( m_ptrApps.GetSize() > nPos )return dynamic_cast<XApplication*>( m_ptrApps[nPos] );else return 0;	};
	XApplication	*GetActiveApp();	//return the active applicatoin
	//find methodic in appmanager by filename
	void LocateMethodicByFileName( CString strFileName, long &nAppPos, long &nMethPos );


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAppManager)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CAppManager();

	// Generated message map functions
	//{{AFX_MSG(CAppManager)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CAppManager)
	long m_nActiveApp;
	afx_msg void OnActiveAppChanged();
	long m_nActiveMethodic;
	afx_msg void OnActiveMethodicChanged();
	afx_msg long GetAppsCount();
	afx_msg BSTR GetAppName(long nPos);
	afx_msg long CreateNewApplication(LPCTSTR szName);
	afx_msg void RemoveApplication(long nPos);
	afx_msg long GetMethodicCount(long nAppPos);
	afx_msg BSTR GetMethodicName(long nAppPos, long nMethPos);
	afx_msg void CreateMethodic(long nAppPos, LPCTSTR nMethName);
	afx_msg void SetCurrentMethodic(long nIndex);
	afx_msg void SetCurrentApplication(long nIndex);
	afx_msg void RenameMethodic(long nIndex, LPCTSTR szNewName);
	afx_msg void RenameApplication(long nIndex, LPCTSTR szNewName);
	afx_msg void MoveMethodic(long nPos, long nPosTo);
	afx_msg void ReloadActiveMethodic();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_EVENT_MAP()

	//{{AFX_EVENT(CAppManager)
	void FireAfterMethodicChange()
		{FireEvent(eventidAfterMethodicChange,EVENT_PARAM(VTS_NONE));}
	void FireBeforeMethodicChange()
		{FireEvent(eventidBeforeMethodicChange,EVENT_PARAM(VTS_NONE));}
	void FireBeforeAppChange()
		{FireEvent(eventidBeforeAppChange,EVENT_PARAM(VTS_NONE));}
	void FireAfterAppChange()
		{FireEvent(eventidAfterAppChange,EVENT_PARAM(VTS_NONE));}
	//}}AFX_EVENT
	enum {
	//{{AFX_DISP_ID(CAppManager)
	dispidActiveApp = 1L,
	dispidActiveMethodic = 2L,
	dispidGetAppsCount = 3L,
	dispidGetAppName = 4L,
	dispidCreateNewApplication = 5L,
	dispidRemoveApplication = 6L,
	dispidGetMethodicCount = 7L,
	dispidGetMethodicName = 8L,
	dispidCreateMethodic = 9L,
	dispidSetCurrentMethodic = 10L,
	dispidSetCurrentApplication = 11L,
	dispidRenameMethodic = 12L,
	dispidRenameApplication = 13L,
	dispidMoveMethodic = 14L,
	dispidReloadActiveMethodic = 15L,
	eventidAfterMethodicChange = 1L,
	eventidBeforeMethodicChange = 2L,
	eventidBeforeAppChange = 3L,
	eventidAfterAppChange = 4L,
	//}}AFX_DISP_ID
	};
	
	ROUTE_OLE_VIRTUALS()
public:
	static CTypedPtrList<CPtrList, XMethodic*>	s_ptrAllMethodics;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPMANAGER_H__B91F7CC7_3B46_11D3_A605_0090275995FE__INCLUDED_)
