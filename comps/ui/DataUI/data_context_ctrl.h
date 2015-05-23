#if !defined(AFX_DATA_CONTEXT_CTRL_H__1B72311B_2C78_4722_B8C5_B2A2F5B3A39D__INCLUDED_)
#define AFX_DATA_CONTEXT_CTRL_H__1B72311B_2C78_4722_B8C5_B2A2F5B3A39D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// data_context_ctrl.h : header file
//


interface IDataContextCtrl : public IUnknown
{
	com_call AddComboBox( HWND hwnd_combo ) = 0;
	com_call RemoveComboBox( HWND hwnd_combo ) = 0;
	com_call OnSelChange( HWND hwnd_combo ) = 0;
	com_call IsAvailable( BOOL* pbavailable ) = 0;
	[helpstring("method Refresh")] HRESULT Refresh(void);
};

declare_interface(IDataContextCtrl, "F3B7D24B-5E87-4dbe-B734-8A96CA34E988" );

IDataContextCtrlPtr	GetDataContextCtrl();


/////////////////////////////////////////////////////////////////////////////
// data_context_ctrl command target

class data_context_ctrl :	public CCmdTarget,
							public CEventListenerImpl,
							public CNamedObjectImpl
{
	DECLARE_DYNCREATE(data_context_ctrl)
	ENABLE_MULTYINTERFACE()

	data_context_ctrl();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(data_context_ctrl)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~data_context_ctrl();

	// Generated message map functions
	//{{AFX_MSG(data_context_ctrl)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(data_context_ctrl)

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(data_context_ctrl)
	afx_msg long GetItemCount();
	afx_msg BSTR GetItemString(long idx);
	afx_msg long GetItemData(long idx);
	afx_msg BOOL DeleteItem(long idx);
	afx_msg long AddItem(LPCTSTR str_item, long data, long idx);
	afx_msg long GetCurSel();
	afx_msg BOOL SetCurSel(long idx);
	afx_msg BOOL ResetContent();
	afx_msg void SetEnable(BOOL benable);
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()


	//notification
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	BEGIN_INTERFACE_PART(DataContextCtrl, IDataContextCtrl)
		com_call AddComboBox( HWND hwnd_combo );
		com_call RemoveComboBox( HWND hwnd_combo );
		com_call OnSelChange( HWND hwnd_combo );
		com_call IsAvailable( BOOL* pbavailable );
	END_INTERFACE_PART(DataContextCtrl);

	//all combo boxes
	CArray<HWND, HWND> m_ar_combo;
	BOOL				m_mEnable;

	//active
	IUnknownPtr	m_ptr_active_doc;
	IUnknownPtr	m_ptr_active_view;

	void		register_doc( IUnknown* punk_doc );
	void		unregister_doc();

	void		register_view( IUnknown* punk_view );
	void		unregister_view();


	//notifications
	bool		_on_activate_view( IUnknown* punk_doc, IUnknown* punk_view );
	bool		_on_close_document( IUnknown* punk_doc );
	bool		_on_destroy_view( IUnknown* punk_view );
	bool		_on_data_context_change( IUnknown* punk_view, IUnknown* punk_obj, NotifyCodes nc );
	
	/*
	//fill
	bool		fill_controls( );
	bool		fill_control( HWND hwnd );

	bool		set_active_object_to_controls( const char* psz_object_name );
	bool		set_active_object_to_control( HWND hwnd, const char* psz_object_name );
	

	//element type
	const char*	get_object_type();
	CString		m_str_object_type;
	bool		is_object_match( IUnknown* punk_obj );

	CString		get_active_object_name_from_context();
	CString		m_str_active_object;

	int			get_item_idx_by_name( HWND hwnd, const char* psz_item );
	*/
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATA_CONTEXT_CTRL_H__1B72311B_2C78_4722_B8C5_B2A2F5B3A39D__INCLUDED_)
