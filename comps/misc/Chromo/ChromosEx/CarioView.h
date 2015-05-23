#if !defined(AFX_CCarioView_H__2FA3F08F_D3F9_47E3_9F24_ECE7EBC41C97__INCLUDED_)
#define AFX_CCarioView_H__2FA3F08F_D3F9_47E3_9F24_ECE7EBC41C97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// CCarioView.h : header file
//

#include "wndmisc5.h"
#include "carioimpl.h"
#include "\vt5\awin\misc_list.h"
#include "docmiscint.h"
#include "PropBagImpl.h"

/////////////////////////////////////////////////////////////////////////////
// CCarioView window

#include "EditCtrl.h"
#include "ndata_api.h"

class CCarioView : public CViewBase,
					public CCarioImpl,
					public CNamedPropBagImpl,
					public CEasyPreviewImpl
{
	HCURSOR m_hNoDrag, m_hDrop;
	ENABLE_MULTYINTERFACE();
	PROVIDE_GUID_INFO( )

	DECLARE_DYNCREATE(CCarioView)

	CNDataCell m_undoState, m_redoState;
	CString m_section;

// Construction
public:
	CCarioView();

	virtual LPUNKNOWN GetInterfaceHook(const void* p)
	{
		if (*(IID*)p == IID_INamedPropBag || *(IID*)p == IID_INamedPropBagSer)
			return (INamedPropBagSer*)this;
		else return __super::GetInterfaceHook(p);
	}

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual	CCarioFrame	*GetDragDropFrame()			{return (CCarioFrame*)m_pframeDrag;}

	virtual bool			DoDrag( CPoint point );
public:
	//drawing, etc...	
	void					do_draw( CDC* pdc, CRect rcUpdate, BITMAPINFOHEADER* pbmih, byte* pdibBits );

	//CCarioImpl virtuals	
	virtual IUnknownPtr		GetViewPtr();
	virtual HWND			GetHwnd();

	virtual void			AttachActiveObjects();

	void					on_deactivate_object_list( IUnknown* punkObjectList );
	void					on_activate_object_list( IUnknown* punkObjectList );

 	BEGIN_INTERFACE_PART(CarioView, ICarioView2)
		com_call SetMode( CarioViewMode viewMode );
		com_call GetMode( CarioViewMode* pViewMode );

		com_call GetLineCount( int* pnLineCount );
		com_call GetCellCount( int nLine, int* pnCellCount );

		com_call GetActiveLine( int* pnLine );
		com_call GetActiveCell( int* pnCell );

		com_call AddLineAtPos( int nLine );
		com_call AddCellAtPos( int nLine, int nCell );
		
		com_call DeleteLine( int nLine );
		com_call DeleteCell( int nLine, int nCell );
		
		com_call DeleteObj( int nLine, int nCell, int nObj );

		com_call GetActiveChromo( int* pnLine, int* pnCell, int* pnChromo );
		com_call SetActiveChromo( int nLine, int nCell, int nChromo );
		//rotating
		com_call SetChromoAngle( int nLine, int nCell, int nChromo, double fAngle );
		com_call GetChromoAngle( int nLine, int nCell, int nChromo, double* pfAngle );
		//Idiogramm
		com_call GetShowChromoIdiogramm( int nLine, int nCell, int nChromo, BOOL* pbShow );
		com_call SetShowChromoIdiogramm( int nLine, int nCell, int nChromo, BOOL bShow );
		com_call SetShowChromoAllIdiogramm( BOOL bShow );
		//zoom
		com_call GetChromoObject( int nLine, int nCell, int nChromo, IUnknown** ppunk );
		com_call SetChromoZoom( int nLine, int nCell, int nChromo, double fZoom );		
		com_call GetChromoZoom( int nLine, int nCell, int nChromo, double *fZoom );		
		com_call GetChromoPath( int nLine, int nCell, int nChromo, char **pstr );		
		//mirror

		com_call SetChromoMirror( int nLine, int nCell, int nChromo, int bMirror );
		com_call GetChromoMirror( int nLine, int nCell, int nChromo, int *bMirror );

		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		// A.M.
		com_call GetCarioSizes( int* pcx, int* pcy);
		com_call GetCellByPoint( POINT ptObj, int *pnLine, int *pnCell, int *pnChromoObj, BOOL bTakeLineHeight );
		com_call GetCellClass( int nLine, int nCell, int *pnClass );
		com_call SetActiveLine( int nLine );
		com_call SetActiveCell( int nCell );
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		com_call StoreState( IUnknown *punkData );
		com_call ArrageObjectsByClasses();
		com_call MirrorChromo( int nLine, int nCell, int nChromo );
		com_call GetChromoRect( long code, int nLine, int nCell, int nChromo, RECT *prect );
		com_call GetShowChromoAllIdiogramm( BOOL *pbShow );

		//Brightnes profile
		com_call SetShowChromoAllBrightness( BOOL bShow );
		com_call GetShowChromoAllBrightness( BOOL *pbShow );

		//FITC/TRITC ratio profile
		com_call SetShowChromoAllRatio( BOOL bShow );
		com_call GetShowChromoAllRatio( BOOL *pbShow );

		//Amplification and Delition
		com_call SetShowChromoAllAmplification( BOOL bShow );
		com_call GetShowChromoAllAmplification( BOOL *pbShow );

		com_call SetShowAllChromoObject( BOOL bShow );
		com_call GetShowAllChromoObject( BOOL *pbShow );

		com_call SetShowAllEditableIdio( BOOL bShow );
		com_call GetShowAllEditableIdio( BOOL *pbShow );

		com_call IsEmpty();
	
		com_call GetIdioObject(  int nLine, int nCell, int nChromo, IdioType idt, IUnknown **punkIdio );
// For Erect
		com_call SetErectedState(  int nLine, int nCell, int nChromo, bool bState );
		com_call GetErectedState(  int nLine, int nCell, int nChromo, bool *bState );
		com_call SetErectedStateAll( bool bState );

		com_call SetShowAllChromoAddLines(BOOL bObj );
		com_call GetShowAllChromoAddLines(BOOL *bObj );

		com_call GetChromoOffsets( int nLine, int nCell, int nChromo, double *pfZoom, long *pnOffset );
		com_call GetShowChromoAddLinesCoord( int nLine, int nCell, int nChromo, int nIndex, double *fPtX, double *fPtY, double *fPtX2, double *fPtY2 );
	END_INTERFACE_PART(CarioView)
	
	BEGIN_INTERFACE_PART(ViewData, IViewData)
		com_call GetObject( IUnknown** ppunkObject, BSTR bstr_type );
		com_call SetObject( IUnknown* punkObject, BSTR bstr_type );
	END_INTERFACE_PART(ViewData)

	BEGIN_INTERFACE_PART( CarioVDnD, ICarioVDnD )
		com_call GetUndoData( BSTR *pBstr, void *pUndoData, void *pRedoData );
	END_INTERFACE_PART(CarioVDnD)


	void set_client_size();	


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCarioView)
	public:
	virtual void OnFinalRelease();	
	protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
// Implementation
public:
	virtual ~CCarioView();

	//virtual void OnChange() {Invalidate();};

	// Generated message map functions
protected:
	//{{AFX_MSG(CCarioView)	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);		
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	GUARD_DECLARE_OLECREATE(CCarioView)
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CCarioView)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	virtual bool _OnLButtonDown(UINT nFlags, CPoint point);
	virtual bool _OnLButtonUp(UINT nFlags, CPoint point);
	virtual bool _OnMouseMove(UINT nFlags, CPoint point);
	virtual bool _OnRButtonUp(UINT nFlags, CPoint point);
public:
	DWORD GetMatchType( const char *szType );

//print functions
	virtual bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX ); 
	virtual bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );

	
	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );

	virtual IUnknown *GetObjectByPoint( CPoint pt );

	//Drawing
	bool _DrawObjects( CDC &dc, CRect rectClip, BITMAPINFOHEADER* pbmih, byte* pdibBits );
	virtual void DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state );
	virtual CRect GetObjectRect( IUnknown *punkObject );
	virtual void SetObjectOffset( IUnknown *punkObject, CPoint pointOffest );

	virtual void OnEnterPreviewMode();
	virtual void OnLeavePreviewMode();
	virtual void OnPreviewReady();
	
	void _AttachObjects( IUnknown *punkContextFrom );

	bool	rebuild_view();


	virtual void OnChange()
	{
		m_b_need_synchronize_dimmension=true; // нужно выполнить две следующие строчки
		//synchronize_dimmension( SDF_RECALC_ALL );
		//set_client_size();
		Invalidate();
	}
protected:
	HCURSOR	m_hcurLast;

	_list_t<IUnknown *>	m_listVisibleObjects;

	bool				ensure_visible( int nline, int ncell, int nobj );
	void _clear_cache();

	CToolTipCtrl m_ToolTipCtrl;

	bool m_b_need_synchronize_dimmension;
public:
	void OnEditCtrlChange(CString str);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CCarioView_H__2FA3F08F_D3F9_47E3_9F24_ECE7EBC41C97__INCLUDED_)
 