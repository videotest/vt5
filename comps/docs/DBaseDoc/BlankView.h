#if !defined(AFX_BLANKVIEW_H__B575C937_678B_4F12_B6D3_C8ECB4F51689__INCLUDED_)
#define AFX_BLANKVIEW_H__B575C937_678B_4F12_B6D3_C8ECB4F51689__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlankView.h : header file
//

#include "axbaseview.h"
#include "DBaseListenerImpl.h"
#include "FlterViewImpl.h"
#include "docmiscint.h"



/////////////////////////////////////////////////////////////////////////////
// CBlankView window

class CBlankView :	public CAxBaseView, 
					public CDBaseListenerImpl, 
					public CFlterViewImpl

{	
	ENABLE_MULTYINTERFACE();

	DECLARE_DYNCREATE(CBlankView);
	GUARD_DECLARE_OLECREATE(CBlankView);

	PROVIDE_GUID_INFO();

	DECLARE_MESSAGE_MAP();
	DECLARE_INTERFACE_MAP();


// Construction
public:
	CBlankView();
	virtual void CreateFrame();

	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );

// Attributes
public:

	virtual void AttachActiveObjects();

	//When site have not enough size do display control	
	virtual void _UpdateLayout( bool bRepositionControls = true );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );	
	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );
	
protected:
	virtual void RepositionControls( );		

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBlankView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	virtual void OnSelectObject(IUnknown* punkObject);

// Implementation
public:
	virtual ~CBlankView();
	virtual DWORD GetMatchType( const char *szType );
	virtual void _AddControl( IUnknown *punkControl );
	virtual void MoveDragFrameToFrame();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBlankView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG
	afx_msg LRESULT OnDndOnDragOver(WPARAM wParam, LPARAM lParam);


protected:
	BlankViewMode m_BlankViewMode;

	BlankViewMode GetBlankViewMode();
	void SetBlankViewMode( BlankViewMode blankViewMode );

	void SetNewModeToControls( BlankViewMode oldBlankViewMode, BlankViewMode newBlankViewMode, IUnknown* punk_ctrl = 0 );

protected:		
	BEGIN_INTERFACE_PART(BlankVw, IBlankView)
		com_call GetMode( short* pnBlankViewMode );
		com_call SetMode( short nBlankViewMode );
		com_call GetActiveAXData( IUnknown** ppunkAXData );
	END_INTERFACE_PART(BlankVw);

	BEGIN_INTERFACE_PART(ClipboardProvider, IClipboardProvider)
		com_call GetActiveObject( IUnknown** ppunkObject );
	END_INTERFACE_PART(ClipboardProvider);


	POSITION FindControlByPoint( CPoint pt );

	CBitmap m_bmpAdd, m_bmpEdit, m_bmpDelete;

	IUnknown* GetActiveControlIPreviewMode();
	

//Filter Support
protected:		
	virtual bool GetActiveField( CString& strTable, CString& strField );
	virtual bool GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue );

//Filter property page
public:
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );
	bool ShowFilterPropPage( bool bShow );

protected:
	CPoint m_ptPrevScroll;

public:
	virtual void UpdateControlRect( IUnknown *punkControl );

	bool OnDBCtrlProp();

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	bool	m_block_set_mode;

protected:	
	virtual bool	get_object_text( IUnknown* punk_ax, CString& str_text );
	virtual bool	set_object_text( IUnknown* punk_ax, CString str_text );

	/*
protected:
	bool			m_bLoadingForm;
	virtual void	_AddAllControls();

	
	virtual void	_AddControl( IUnknown *punkControl );
	virtual void	_RemoveControl( IUnknown *punkControl );

	bool			TryCreateField( IUnknown* punkControl );


	bool			CanVisualEdit( bool bReportError );
	bool			IsDBaseControl( IUnknown* punkControl );
	bool			IsManualAdd( IUnknown* punkControl );
	

	bool			IsChangeCtrlProp( IUnknown* punkData );	
	bool			IsCtrlHasCopyInList( IUnknown* punkData );

	bool			m_bAddDBCtrl;
	bool			m_bDeleteDBCtrl;
	*/
};


/*
/////////////////////////////////////////////////////////////////////////////
// CMyBar dialog

class CDlgBar : public CDialogBar
{
// Construction
public:	
	CDlgBar();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgBar)
	enum { IDD = IDD_CTRL_BAR };
	CString	m_strCaption;
	CString	m_strDefValue;
	BOOL	m_bHasDefValue;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgBar)
	public:	
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support	
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);	
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgBar)	
	afx_msg void OnHasDefValue();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


public:
	IDBaseStructPtr		m_ptrDBaseDoc;
	bstr_t				m_bstrTable;
	bstr_t				m_bstrField;

	bool GetData();
	bool SetData();	

};
*/

/*
/////////////////////////////////////////////////////////////////////////////
// CMyWnd window

class CFrameProp : public CMiniFrameWnd
{
// Construction
public:
	CFrameProp();
	
	CDlgBar m_bar;

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFrameProp)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFrameProp();

	// Generated message map functions
protected:
	//{{AFX_MSG(CFrameProp)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCancelMode();
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnNcDestroy();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	

	bool m_bDestroyInProgress;

public:
	void destroy_frame();
	CSize GetBarSize();
};

*/


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLANKVIEW_H__B575C937_678B_4F12_B6D3_C8ECB4F51689__INCLUDED_)
