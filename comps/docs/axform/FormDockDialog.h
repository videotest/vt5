#if !defined(AFX_FORMDOCKDIALOG_H__E975FB83_E2AC_11D3_A0A1_0000B493A187__INCLUDED_)
#define AFX_FORMDOCKDIALOG_H__E975FB83_E2AC_11D3_A0A1_0000B493A187__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FormDockDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFormDockDialog dialog
#include "FormDialog.h"

#define _CFormDialog	CFormDialog

class CFormDockDialog : public _CFormDialog,
						public CDockWindowImpl,
						public CWindowImpl,
						public CRootedObjectImpl,
						public CNamedObjectImpl
{
	ENABLE_MULTYINTERFACE();
// Construction
public:
	bool m_bLockHideEvent;
	CFormDockDialog(long lDockSite = AFX_IDW_DOCKBAR_BOTTOM, bool bFixed = false);
	~CFormDockDialog();

	virtual void DoCaptionChange();
	void SetDlgTitle(CString strTitle);

	virtual CWnd *GetWindow(){return this;};
	virtual bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );

	virtual void OnShow();
	virtual void OnHide();

	void LockHideEvent( bool bLock ) { m_bLockHideEvent =  bLock; } ;

		
// Dialog Data
	//{{AFX_DATA(CFormDockDialog)
	enum { IDD = IDD_CONTAINER_DOCKDIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFormDockDialog)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFormDockDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_INTERFACE_MAP()
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FORMDOCKDIALOG_H__E975FB83_E2AC_11D3_A0A1_0000B493A187__INCLUDED_)
