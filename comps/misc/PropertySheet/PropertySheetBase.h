#ifndef __PropertySheetBase_h__
#define __PropertySheetBase_h__


class CExtPage;


class CPropertySheetBase : public CPropertySheet,
						  public CWindowImpl,
						  public CRootedObjectImpl,
						  public CNamedObjectImpl,
						  public CCompManagerImpl,
						  public CHelpInfoImpl
{
	ENABLE_MULTYINTERFACE();

public:	//cnstructor/dfestructor
	CPropertySheetBase( const char *szSection = 0 );
	virtual ~CPropertySheetBase();

protected:
	virtual CWnd *GetWindow();
protected:
	BEGIN_INTERFACE_PART(Sheet, IPropertySheet)
		com_call IncludePage(int nPage);
		com_call ExcludePage(int nPage);
		
		com_call GetPagesCount(int* pNumPages);
		com_call GetPage( int nPage, IUnknown **ppunkPage );

		com_call ActivatePage( int nPage );
		com_call GetActivePage( int *pnPage );
		com_call SetModified( long bSetModified );
	END_INTERFACE_PART(Sheet);

	// Generated message map functions
	//{{AFX_MSG(CPropertySheetBase)
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CPropertySheetBase)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_MESSAGE_MAP()
	DECLARE_INTERFACE_MAP()
	DECLARE_DISPATCH_MAP()
protected:
	void DelayShowPage( int nPage, bool bShow );
	void PreformDelayOperations();
	bool IsPageVisible( int nPage );


	CTypedPtrArray<CObArray, CExtPage*> 
					m_PageList;	//list of containers
	CCompManager	m_manPages;
	int				m_nPages;
	CArray<int, int&>	m_PagesToDelayShow;
	CArray<int, int&>	m_PagesToDelayHide;

};


#endif //__PropertySheetBase_h__
