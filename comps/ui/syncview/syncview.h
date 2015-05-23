// syncview.h : main header file for the SYNCVIEW DLL
//

#if !defined(AFX_SYNCVIEW_H__3A8890D5_D31D_44CC_A5A1_D880732D5B57__INCLUDED_)
#define AFX_SYNCVIEW_H__3A8890D5_D31D_44CC_A5A1_D880732D5B57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

//it shoult aggregate to document
class CSyncManager : public CCmdTargetEx,
					public CEventListenerImpl
{
	DECLARE_DYNCREATE(CSyncManager);
	GUARD_DECLARE_OLECREATE(CSyncManager);
	DECLARE_INTERFACE_MAP();

	ENABLE_MULTYINTERFACE();
public:
	CSyncManager();
	~CSyncManager();

	BEGIN_INTERFACE_PART(Sync, ISyncManager)
		com_call SetSyncMode( BOOL bEnterSyncMode, BOOL bRelative, BOOL bSyncZoom, IUnknown *punkSyncTargetDoc );
		com_call GetSyncMode( BOOL *pbSyncMode, BOOL *pbSyncZoom, BOOL *pbRelative);
		com_call SetLockSyncMode(BOOL bLock);
		com_call SetSyncContext( BOOL bSync );
		com_call GetSyncContext( BOOL *pbSync );
	END_INTERFACE_PART(Sync);

	virtual BOOL OnCreateAggregates();
	virtual void OnFinalRelease();

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	void _AddView( IUnknown *punkView );
	void _RemoveView( IUnknown *punkView );
	void _UpdateScrollPosition( IUnknown *punkView );

	void _UpdatePositionForView( IUnknown *punkView );
	void _SetCurrentScroll(IUnknown* punkView);

	void _CalcRelative();

	void _AttachToView( IUnknown *punkView );
	void _SyncContexts();

protected:
	CPtrList	m_ptrViews;
	bool		m_bSyncScroll;
	bool		m_bSyncZoom;

	bool		m_bRelative; //sync scrolls relatively
	bool		m_bLock;
	CPoint		m_pointPrevScroll;
	bool		m_bSyncContext;


	double	m_fCurrentZoom;
	BOOL	m_bCurrentFit;
	double	m_fCurrentCalibr;
	CPoint	m_pointCurrentScroll;
	CSize	m_sizeRelative;
	GuidKey m_nActiveKey;
	IUnknownPtr		m_ptrActiveView;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNCVIEW_H__3A8890D5_D31D_44CC_A5A1_D880732D5B57__INCLUDED_)
