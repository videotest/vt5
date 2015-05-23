
#pragma once

#include "PaneBar.h"
#include "method_int.h"

namespace MetodicsTabSpace
{
class CMetodicsTab;

class CMtdTabTarget : public COleDropTarget
{
	CMetodicsTab *m_pOwner;
public:
	CMtdTabTarget();
	virtual void Register( CMetodicsTab * pWnd );
protected:
	virtual DROPEFFECT OnDragEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	virtual DROPEFFECT OnDragOver(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
	virtual DROPEFFECT OnDragLeave(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);
};
using namespace EnviromentClasses;


class CMetodicsTab :	public CWnd,
						public CWindowImpl,
						public CDockWindowImpl,
						public CRootedObjectImpl,
						public CEventListenerImpl,
						public CNamedObjectImpl,
						public CMsgListenerImpl,
						public CHelpInfoImpl
{
	CString m_strCurContent;

	POINT m_ptContextPoint;
	DECLARE_DYNCREATE( CMetodicsTab )
	GUARD_DECLARE_OLECREATE( CMetodicsTab )
	ENABLE_MULTYINTERFACE();
	CMtdTabTarget m_target;
public:
	CMetodicsTab();
	~CMetodicsTab();
	DECLARE_MESSAGE_MAP()

	//{{AFX_DISPATCH(CFishContextViewer)
	//}}AFX_DISPATCH
	DECLARE_INTERFACE_MAP()

	BEGIN_INTERFACE_PART( MtdTab, IMethodTab )
		com_call AddPane( BSTR bstrName );
		com_call Remove( long lIndex );
		com_call GetActivePane( long *plIndex );
		com_call GetPaneCount( long *plCount );
		com_call LoadContent( BSTR bstrFile );
		com_call StoreContent( BSTR bstrFile );

		com_call GetContextPoint( POINT *lpptContext );

		com_call PanePaneItemByPoint( POINT ptPoint );
		com_call OpenPane( BSTR bstrPath, BOOL bOpen );

	END_INTERFACE_PART( MtdTab );

	virtual CWnd *GetWindow();
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
public:
	BOOL OnDrop(CWnd* pWnd, COleDataObject* pDataObject,
		DROPEFFECT dropEffect, CPoint point);
	
	BOOL OnEnter(CWnd* pWnd, COleDataObject* pDataObject,
		DWORD dwKeyState, CPoint point);

protected:
	virtual void PostNcDestroy();

	// Data
	CPaneBar	*m_pBar;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg LRESULT OnHelpHitTest(WPARAM wParam, LPARAM lParam);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnDestroy();
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
protected:
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
}