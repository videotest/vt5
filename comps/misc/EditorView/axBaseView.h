#ifndef __axbaseview_h__
#define __axbaseview_h__

#include "statusutils.h"
#include "editorView.h"

#include "editorViewdefs.h"


class CContainerWnd;

class CContainerWnd : public CWnd
{
public:
	CContainerWnd()
	{};
	~CContainerWnd()
	{};

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
public:
	CString	m_strControlProgID;
	CString	m_strControlName;
protected:
	virtual void PostNcDestroy(){};
};



/////////////////////////////////////////////////////////////////////////////
// CEditCtrl
class CEditCtrl : public CEdit
{
// Construction		   
public:
	CEditCtrl() :m_x(0),m_y(0),m_bEscapeKey(FALSE) {}
	void SetCtrlFont( CFont* pFont );

// Attributes
protected:
	int m_x;
	int m_y;
	int m_width;
	int m_height;
	BOOL m_bEscapeKey;
	CFont m_Font;

// Operations
public:
	void UpdateControlSize();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditCtrl)
	//}}AFX_VIRTUAL	
// Implementation
public:
	virtual ~CEditCtrl() {};			
	// Generated message map functions
protected:
	//{{AFX_MSG(CLVEdit)	
	afx_msg void OnKillfocus();
	afx_msg void OnChange();	
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:	

	virtual void PostNcDestroy();
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	/*
public:
	vitual BOOL PreTranslateMessage(MSG* pMsg);
	*/

};




class DLL CAxBaseFrame : public CFrame
{
	DECLARE_DYNCREATE(CAxBaseFrame);

	CAxBaseFrame();
public:
	virtual void	Draw(CDC& dc, CRect rectPaint, BITMAPINFOHEADER *pbih, byte *pdibBits );
	virtual void	Redraw( bool bOnlyPoints = false, bool bUpdate = false );
	virtual void SelectObject(IUnknown* punkObject, bool bAddToExist = true);

	virtual void OnBeginDragDrop();
	virtual void OnEndDragDrop();

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoEnterMode(CPoint point);
	void RedrawSelectRect( );
	
	virtual CPoint	CalcPointLocation( PointLocation location, double &fAngle );
	CRect GetCurentRect(){return m_rectCurrent;}

	POSITION	GetFirstObjectPosition();
	IUnknown	*GetNextObject( POSITION &rPos );

	bool	InTrackMode(){ return m_bInTrackMode;}
	

public:
	bool	m_bDragDropFrame;
	bool	m_bSelectFrameMode;
	CRect	m_rectSelect;
	CPoint	m_pointFirstPoint;

	bool m_bForceUpdate;
	bool m_bUseParentScroll;	
	bool m_bUseDesktopDC;

	CRect GetSelectRect() { return m_rectCurrent;}

	virtual CRect ConvertToDeskTop( CRect rc, bool bCorrectOffset = true ){ return rc;}	
};





/*
class DLL CAxDesktopFrame : public CAxBaseFrame
{
	DECLARE_DYNCREATE(CAxDesktopFrame);

	CAxDesktopFrame();
public:
	virtual void	Draw(CDC& dc, CRect rectPaint, BITMAPINFOHEADER *pbih, byte *pdibBits );
	virtual void	Redraw( bool bOnlyPoints = false, bool bUpdate = false );

	virtual void OnBeginDragDrop();
	virtual void OnEndDragDrop();

	virtual bool DoLButtonDown( CPoint pt );
	virtual bool DoLButtonUp( CPoint pt );
	virtual bool DoStartTracking( CPoint pt );
	virtual bool DoTrack( CPoint pt );
	virtual bool DoFinishTracking( CPoint pt );
	virtual bool DoEnterMode(CPoint point);
	void RedrawSelectRect();	
};
*/



#include "desktop_frame.h"
	  
class DLL CAxBaseView : public CViewBase
{
	ENABLE_MULTYINTERFACE();


	DECLARE_DYNCREATE(CAxBaseView);
	GUARD_DECLARE_OLECREATE(CAxBaseView);
	PROVIDE_GUID_INFO_NULL( )

	DECLARE_MESSAGE_MAP();
	DECLARE_INTERFACE_MAP();
	
	bool ResizeXY(int &dcx, int &dcy);
public:
	struct ControlData
	{
		//CContainerWnd	*pwnd;
		UINT		nID;
		IUnknown	*punkCtrlData;
		IUnknownPtr	ptrCtrl;
		CContainerWnd	*pwnd;
	};

	CAxBaseView();
	~CAxBaseView();	

	virtual void CreateFrame();

	virtual BOOL IsVisibleCtrlWnd(){ return TRUE;}

	//coordinate mapping - screen to client
	virtual CRect ConvertToClient( CRect rc);
	virtual CPoint ConvertToClient( CPoint pt);
	virtual CSize ConvertToClient( CSize size);
	//coordinate mapping - client to screen
	virtual CRect ConvertToWindow( CRect rc);
	virtual CPoint ConvertToWindow( CPoint pt);
	virtual CSize ConvertToWindow( CSize size );

	virtual CPoint GetScrollPos( );


	void OnEditCtrlChange( CString str, BOOL bSave );
	BOOL IsEditCtrlActivate(){ return m_ptrEditObject != 0;}

	CObjectListWrp &GetControls()	{return m_sptrControls;}

	bool ShowInsertCtrlSheet( bool bShow );

	//{{AFX_MSG(CAxBaseView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);	
	//}}AFX_MSG
	//{{AFX_VIRTUAL(CAxBaseView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	bool _OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
	virtual void AttachActiveObjects();
	virtual DWORD GetMatchType( const char *szType );

	virtual bool CanResizeObject( IUnknown *punkObject ){return true;};
	virtual CRect GetObjectRect( IUnknown *punkObject );
	virtual void SetObjectOffset( IUnknown *punkObject, CPoint pointOffest );
	virtual IUnknown *GetObjectByPoint( CPoint );
	virtual bool PtInObject( IUnknown *punkObject, CPoint point );
	virtual bool DoDrop( CPoint point );
	virtual void DoResizeRotateObject( IUnknown *punkObject, CFrame *pframe );
	bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );


	virtual void UpdateSelectionRect();
	virtual void UpdateControlRect( IUnknown *punkControl );
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView );	


	//void DoDrawControl( CDC &dc, IUnknown *punkControl, CRect rectDraw );
	virtual void DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state );
protected:
	BEGIN_INTERFACE_PART_EXP(LayoutVw, ILayoutView)
		com_call GetControlObject( IUnknown *punkAXCtrl, IUnknown **ppAX );		
		com_call GetFormOffset( POINT *ppoint );
	END_INTERFACE_PART(LayoutVw);
protected:
	
	virtual void _AddControl( IUnknown *punkControl );
	virtual void _RemoveControl( IUnknown *punkControl );

	virtual void _AddAllControls();
	virtual void _RemoveAllControls();

	POSITION	_Find( IUnknown *punkControl );
	POSITION	_Find( CWnd *pwnd );
	ControlData	*_Get( IUnknown *punkControl );
	ControlData	*_Get( CWnd *pwnd );
	UINT 		_FindUniqueID( UINT nID );

	virtual void _UpdateLayout( bool bRepositionControls = true );
	bool		_IsControlWindow( HWND hWnd, bool bLookChild = true );
	virtual POSITION GetFisrtVisibleObjectPosition();
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos );

protected:
	CObjectListWrp	m_sptrControls;

	CTypedPtrList<CPtrList, ControlData*>	
		m_controlInfos;

	CRect	m_rectForm;
	CPoint	m_pointFormOffset;	
	
	bool	m_bCanResizeForm;
	bool	m_bDrawFrame;
	IUnknownPtr	m_ptrEditObject;
	CRect	m_rectCaption;
	

	BEGIN_INTERFACE_PART_EXP(EditorMenu, IMenuInitializer)
		com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu );
	END_INTERFACE_PART(EditorMenu);


public:	
	virtual void BeforLButtonDown( CPoint pt );
	virtual void OnSelectObject(IUnknown* punkObject );	
	virtual void AfterLButtonDown( CPoint pt );
	virtual void CalcFormRect( CSize size );
	virtual CPoint Offset()		{return m_pointFormOffset;}
protected:
	bool m_bObjectSelectProcess;
	IUnknownPtr m_ptrPrevSelectedObject;

	bool GetObjectsByPont( CPoint pt, CPtrList& arObject, bool& bCurSelectedUnderPoint, 
			bool bIncludeSelected = false );
	virtual bool IsObjectCanBeSelected(IUnknown *punkObj) { return true;}

public:
	bool UpdateContainerWnd( IUnknown* punkObject );

protected:
	bool	m_bRemoveCtrlInProgress;
	bool	m_bCangeCtrlInProgress;
public:
	bool	IsRemoveCtrlInProgress(){ return m_bRemoveCtrlInProgress;}

//Pane support
protected:
	void			CreateCtrlPane();
	void			DestroyCtrlPane();

	
public:	
	void			UpdateSizePane( CString strText );
	void			UpdatePosPane( CString strText );
	virtual	CPoint	PaneConvertOffset( CPoint ptOffset );
	virtual	CRect	PaneConvertObjectRect( IUnknown* punkObject );

	virtual void	OnSetObjectOffset( IUnknown *punkObject, CPoint pointOffset );

public:	
	virtual void OnEndDragDrop();

	virtual void OnFrameTrack( CPoint pt );
	virtual void OnFrameFinishTrack( CPoint pt );
	virtual bool CanSaveToImage() {return false;}

	
protected:	
	virtual bool	get_object_text( IUnknown* punk_ax, CString& str_text ){ return false; }
	virtual bool	set_object_text( IUnknown* punk_ax, CString str_text ){ return false; }
	
};

#endif //__axbaseview_h__
