// DBaseGridView.h: interface for the CDBaseGridView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DBaseGridView_H__8603FF83_654F_4937_AD2E_2F54DF805585__INCLUDED_)
#define AFX_DBaseGridView_H__8603FF83_654F_4937_AD2E_2F54DF805585__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DBaseListenerImpl.h"
#include "DBaseGridCtrl.h"
#include "FlterViewImpl.h"

#define IDC_GRID_CTRL	200

class CDBaseGridView  : public CViewBase, 
						public CDBaseListenerImpl,
						public CFlterViewImpl
{
	ENABLE_MULTYINTERFACE();
	DECLARE_DYNCREATE(CDBaseGridView)	
	PROVIDE_GUID_INFO( )

public:
	CDBaseGridView();
public:	
	virtual void OnDBaseNotify( const char *pszEvent, IUnknown *punkObject, IUnknown *punkDBaseDocument, BSTR bstrTableName, BSTR bstrFieldName, const _variant_t var );

//print functions
	virtual bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX ); 
	virtual bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );

	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDBaseGridView)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	virtual CScrollBar	*GetScrollBarCtrl(int iSBCode) const	
	{	return CViewImpl::GetScrollBarCtrl(iSBCode);	}

// Implementation
public:
	virtual ~CDBaseGridView();
	DWORD GetMatchType( const char *szType );

	// Generated message map functions
protected:
	//{{AFX_MSG(CDBaseGridView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnDestroy();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CDBaseGridView)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CDBaseGridView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART(DBaseGridVw, IDBGridView)
		com_call SaveGridSettings();
	END_INTERFACE_PART(DBaseGridVw);

public:
	virtual void AttachActiveObjects();
	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );

protected:	
	
	CDBaseGridCtrl m_ctrlGrid;
	
	void ResizeGrid();	

//Filter Support
protected:
	virtual bool GetActiveField( CString& strTable, CString& strField );
	virtual bool GetActiveFieldValue( CString& strTable, CString& strField, CString& strValue );

//Filter property page
public:
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );
	bool ShowFilterPropPage( bool bShow );

};

#endif // !defined(AFX_DBaseGridView_H__8603FF83_654F_4937_AD2E_2F54DF805585__INCLUDED_)
