#if !defined(AFX_AUDITTRAILVIEW_H__3D3807F0_7681_4FD6_BDAE_2DF02CE711E1__INCLUDED_)
#define AFX_AUDITTRAILVIEW_H__3D3807F0_7681_4FD6_BDAE_2DF02CE711E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuditTrailView.h : header file
//
#define szAuditTrailViewProgID "AuditTrail.AuditTrailView"
#include "auditint.h"

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailView command target

class CAuditTrailView : public CViewBase
{
	DECLARE_DYNCREATE(CAuditTrailView)
	PROVIDE_GUID_INFO()

	CAuditTrailView();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuditTrailView)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );
	virtual CWnd *GetWindow(){return this;}
	virtual void AttachActiveObjects();	//virtuals
	
	DWORD GetMatchType( const char *szType );

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );


//print functions
	virtual bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX ); 
	virtual bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );

	
// Implementation
protected:
	virtual ~CAuditTrailView();

	// Generated message map functions
	//{{AFX_MSG(CAuditTrailView)
	afx_msg void OnPaint();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnNcDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CAuditTrailView)

	bool SetActiveAT();
	void UpdateDocumentSize();
	void EnsureVisibleActiveThumbnail();

	bool					m_bSingle;
private:
	IAuditTrailObjectPtr	m_sptrActiveAT;
	BOOL					m_bHorz;

public:
	virtual const char* GetViewMenuName();
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_AUDITTRAILVIEW_H__3D3807F0_7681_4FD6_BDAE_2DF02CE711E1__INCLUDED_)
