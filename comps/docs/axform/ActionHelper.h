#if !defined(AFX_ACTIONHELPER_H__7AC206BA_E6AE_4959_B7A4_72081B2DF887__INCLUDED_)
#define AFX_ACTIONHELPER_H__7AC206BA_E6AE_4959_B7A4_72081B2DF887__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActionHelper.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CActionHelper command target
struct ArgInfo
{
public:
	bool		m_bInput;
	IUnknownPtr	m_ptrObject;
	_bstr_t		m_bstrType;
	int			m_nContextIndex; // вроде не используется
	bool		m_bNamedDataChanged;
	bool		m_bHaveToRemove;
	bool		m_bWasVirtual;
	IUnknownPtr	m_ptrStoreParent;

	_bstr_t		m_bstr_original_name;
	_bstr_t		m_bstr_given_name; // имя, указанное в командной строке
	long		m_lArgumentPosition;

	// debug features - сохраняемая дополнительная инфа
	IUnknown* m_punkActionInfo; // без AddRef
	IUnknown* m_punkAction; // без AddRef
	_bstr_t m_bstrActionName;
};

class CActiveXSite;


class CPreviewData : public CCmdTargetEx, public CNumeredObjectImpl
{
	ENABLE_MULTYINTERFACE()
public:
	CPreviewData();
	~CPreviewData();

	void Init();
	operator IUnknown*()	{return GetControllingUnknown();}

	DECLARE_INTERFACE_MAP()
protected:	
	IUnknown	*m_ptrAggrData;
	IUnknown	*m_ptrAggrContext;
};

class CLockerFlag
{
public:
	CLockerFlag( bool *pbFlag )
	{
		m_pbFlag = pbFlag;
		*m_pbFlag = true;
	}
	~CLockerFlag()	
	{
		*m_pbFlag = false;
	}
	bool	*m_pbFlag;
};

class CActionHelper : public CCmdTargetEx,
						public CNamedObjectImpl,	
						public CEventListenerImpl
{
	class XLock
	{
	public:
		XLock( CActionHelper *ph )
		{m_ph = ph;m_bStore = ph->m_bLockUpdate;ph->m_bLockUpdate = true;}
		~XLock()
		{m_ph->m_bLockUpdate = m_bStore;}
	protected:
		CActionHelper	*m_ph;
		bool			m_bStore;

	};
	class XSiteButton
	{
	public:
		HWND	m_hwnd;
		bool	m_bOriginalEnabled;
		XSiteButton	*m_pnext;
	};

private:
	bool m_bHelperInGroup;
	long m_lExecuteActionCount;
	bool m_bTerminateFilterGroup;
public:

	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CActionHelper)
public:
	CActionHelper();           // protected constructor used by dynamic creation
	virtual ~CActionHelper();

	void	Init( CActiveXSite *psite );
	void	DeInit();
	void	DeInitArgs();
// Attributes
public:
	ArgInfo *GetArgDescription( bool bOut, BSTR bstrArg, BSTR bstrArgName, int nPosition );
	void	ExecuteCurrentAction();
	void	TerminateCurrentAction();
	void	FinalizeCurrentAction( bool bTerminate );


// Operations
public:
	void	BeginFilterGroup();
	void	EndFilterGroup();
	void	MoveNewArgsToData();
	void	ApplyPreview();
	void	RemovePreview();
	void	SetActionToExecuteList( IUnknown *punk );

	void	DoApplyPreview();
	void	DoRemovePreview();

	void	ActivatePreview();
	void	DeActivatePreview();
	void	RegisterDocListener( bool bRegister );
	void	UpdatePreviewView( bool bReAttachObjects );
	void	CheckObjects();
	void	AttachActivesObjectFromContext( bool bAttach );
	IUnknown	*GetObject( ArgInfo *pai, IUnknown *punkFrom );
	void	InitSiteButtons();
	void	EnableSiteButtons( bool bEnable, bool bForceEnable = false );
	void	FreeSiteButtons();
	void	AddSiteButton( HWND hwnd );


	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );


	void AddHeadArg(IUnknown* punkDataObject);
	void AttachPreviewView( IUnknown *punkView );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActionHelper)
	//}}AFX_VIRTUAL

// Implementation
	// Generated message map functions
	//{{AFX_MSG(CActionHelper)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	BEGIN_INTERFACE_PART(Dispatch, IDispatch)
		com_call GetTypeInfoCount( UINT *pctinfo);
        com_call GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo );
        com_call GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId );
        com_call Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr );
	END_INTERFACE_PART(Dispatch)

	DECLARE_INTERFACE_MAP()

	IActionManagerPtr	m_ptrActionManager;
	IDispatchPtr		m_ptrActionManagerDispatch;

	typedef CTypedPtrArray<CPtrArray, ArgInfo*> ArgInfoArr;
	CTypedPtrArray<CPtrArray, ArgInfoArr*> m_argsArr;
	CTypedPtrArray<CPtrArray, ArgInfoArr*> m_argsArrSaved;

	IDocumentSitePtr	m_ptrActiveDocument;

	//IActionPtr			m_ptrCurrentAction;
	//IActionPtr			m_ptrCurrentCompleteAction;
	//IActionPtr			m_ptrWaitAction;//paul 14.05.2003 WM_TIMER problem. m_ptrCurrentAction == 0, that set on SetActionToExecuteList

	class XIActionPtrArray: public CArray<IActionPtr,IActionPtr&>
	{
	public:
		XIActionPtrArray& operator = ( XIActionPtrArray &arr )
		{
			RemoveAll();
			Copy( arr );
			return *this;
		}

		bool operator==( XIActionPtrArray &arr )
		{
			if( GetSize() != arr.GetSize() )
				return false;

			for( int i = 0; i < GetSize(); i++ )
			{
				if( GetAt( i ) != arr.GetAt( i ) )
					return false;
			}

			return true;
		}
	};
	XIActionPtrArray m_arrPtrCurrActions;
	XIActionPtrArray m_arrPtrCurrComleteActions;
	XIActionPtrArray m_arrPtrWaitActions;
	int		m_nAction;
	bool				m_bPtrCurrActionsChanged;

	bool				m_bRunning;
	bool				m_bTerminated;
	bool				m_bAllowTerminate;
	bool				m_bApplyOnComplete;
	bool				m_bRemoveOnComplete;
	bool				m_bInitialized;

	CObjectListUndoRecord	m_changes;

	//new
	IEasyPreviewPtr		m_ptrPreviewView;
	//old
	//IPreviewSitePtr		m_ptrPreviewView;
	

	CActiveXSite			*m_psite;
	CPreviewData			*m_pPreviewTarget;
	CObjectListUndoRecord	m_changesPreview;
	XSiteButton				*m_pSiteButton;
	bool					m_bDelayEnableButtons;
public:
	bool				m_bLockUpdate;
protected:
	void delete_args_arr();
	BOOL PrepareArgDescriptions( int nAction, IFilterAction *pIFilterAction );
	void _delete_args( int nFrom, int nCount );
	void save_args_arr();
	void delete_saved_args_arr();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIONHELPER_H__7AC206BA_E6AE_4959_B7A4_72081B2DF887__INCLUDED_)
