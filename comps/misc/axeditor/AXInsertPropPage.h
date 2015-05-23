#if !defined(AFX_AXINSERTPROPPAGE_H__012C8574_D93E_11D3_A096_0000B493A187__INCLUDED_)
#define AFX_AXINSERTPROPPAGE_H__012C8574_D93E_11D3_A096_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AXInsertPropPage.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CAXInsertPropPage dialog

class CAXInsertPropPage : public CPropertyPageBase,
							public CEventListenerImpl
{
	PROVIDE_GUID_INFO( )
	ENABLE_MULTYINTERFACE()
	DECLARE_DYNCREATE(CAXInsertPropPage);
	GUARD_DECLARE_OLECREATE(CAXInsertPropPage);
// Construction
public:
	CAXInsertPropPage(CWnd* pParent = NULL);   // standard constructor
	~CAXInsertPropPage();

public:
	virtual void DoLoadSettings();
	virtual void DoStoreSettings();

// Dialog Data
	//{{AFX_DATA(CAXInsertPropPage)
	enum { IDD = IDD_PROPPAGE_INSERT };
	CButton	m_btnAdd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAXInsertPropPage)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAXInsertPropPage)
	afx_msg void OnButtonAdd();
	afx_msg void OnButtonRemove();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	afx_msg void OnCommandToolBar(UINT nID);
	DECLARE_MESSAGE_MAP()

	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(Insert, IAXInsertPropPage)
		com_call GetCurProgID( BSTR *pbstrProgID );
		com_call UnCheckCurrentButton();
		com_call CheckCurrentButton();
		com_call OnActivateView();
	END_INTERFACE_PART(Insert)
	


	CToolBarCtrl*	m_pToolBarCtrl;
	int				m_nCurButton;
	CStringArray	m_arrProgIDs;
	CImageList		m_imgList;
	int				m_nAddedButtonsCount;
	CPtrArray		m_bitmaps;

	bool CreateToolbar();

private:
	void _LoadToolBar( );
	void _SaveToolBar( );
	void _AddCtrlButton(CString strProgID);
	HBITMAP _GetBitmapFromProgID(CString strProgID);
	void _CheckButton(int nBtn = -1);	

	void RunActionAXInsert( );	
	CString GetCurrentSection();
	CString GetPrevSection();
	bool m_bButtonPressed;
	CString m_strPrevSection;
	void OnActivateView();


	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AXINSERTPROPPAGE_H__012C8574_D93E_11D3_A096_0000B493A187__INCLUDED_)
