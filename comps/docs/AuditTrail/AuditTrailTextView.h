#ifndef __AUDITTRAILTEXTVIEW_H__
#define __AUDITTRAILTEXTVIEW_H__

#define szAuditTrailTextViewProgID "AuditTrail.AuditTrailTextView"

/////////////////////////////////////////////////////////////////////////////
// CAuditTrailTextView command target
#include "GridCtrl.h"
#include "auditint.h"

class CAuditTrailTextView;

class CATGrid : public CGridCtrl
{
protected:
	CAuditTrailTextView*	m_pView;
public:
	
	CATGrid()
	{
		m_pView = 0;
	}
	void SetATView( CAuditTrailTextView* pView )
	{	m_pView = pView;}
	virtual BOOL SetColumnWidth(int col, int width);
};



struct grid_data
{
	DWORD	dwSize;
	DWORD	dwTimeWidth;
	DWORD	dwActionNameWidth;
	DWORD	dwParameterWidth;
};

class CAuditTrailTextView : public CViewBase
{
	DECLARE_DYNCREATE(CAuditTrailTextView)
	PROVIDE_GUID_INFO()

	CAuditTrailTextView();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAuditTrailTextView)
	public:
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );
	virtual CWnd *GetWindow(){return this;}
	virtual void AttachActiveObjects();
	virtual CScrollBar	*GetScrollBarCtrl(int iSBCode) const
	{	return CViewImpl::GetScrollBarCtrl(iSBCode);	}

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

// Implementation
protected:
	virtual ~CAuditTrailTextView();

	// Generated message map functions
	//{{AFX_MSG(CAuditTrailTextView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void	OnSelChangedGrid(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNcDestroy();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CAuditTrailTextView)


	void _RebuildList();
	void _AddToList(long nPos);
private:
	CATGrid					m_Grid;
	IAuditTrailObjectPtr	m_sptrActiveAT;
	bool					m_bShowActionParams;

public:
	virtual const char* GetViewMenuName();

	bool		save_grid_data_to_object( IUnknown* punkObject );
	bool		load_grid_data_from_object( IUnknown* punkObject );

	bool		m_bCanProcessWidthHandler;
	void		OnSetColumnWidth( int col, int width );

//print functions
	virtual bool GetPrintWidth(int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX); 
	virtual bool GetPrintHeight(int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY);
	virtual void Print(HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags);
};

/////////////////////////////////////////////////////////////////////////////
#endif //__AUDITTRAILTEXTVIEW_H__
