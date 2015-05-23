#ifndef __axview_h__
#define __axview_h__

#include "..\\EditorView\\axbaseview.h"

class CAxView : public CAxBaseView
{	
	DECLARE_DYNCREATE(CAxView);
	GUARD_DECLARE_OLECREATE(CAxView);
	DECLARE_MESSAGE_MAP();
	PROVIDE_GUID_INFO( )
  public:
	CAxView();
	~CAxView();
	virtual void AttachActiveObjects();
	virtual DWORD GetMatchType( const char *szType );
	virtual IUnknown * GetNextVisibleObject(LPCTSTR szObjType, IUnknown * punkPrevObject);

	virtual void CreateFrame();
	virtual void SetObjectOffset( IUnknown *punkObject, CPoint pointOffest );

	virtual CRect GetObjectRect( IUnknown *punkObject );
	virtual IUnknown *GetObjectByPoint( CPoint );
  
  protected:
	virtual void _RemoveControl( IUnknown *punkControl );

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

};
/*
class CContainerWnd;

class CAxView : public CViewBase
{
	ENABLE_MULTYINTERFACE();
	struct ControlData
	{
		//CContainerWnd	*pwnd;
		UINT		nID;
		IUnknown	*punkCtrlData;
		IUnknownPtr	ptrCtrl;
		CContainerWnd	*pwnd;
	};
	

	DECLARE_DYNCREATE(CAxView);
	GUARD_DECLARE_OLECREATE(CAxView);

	DECLARE_MESSAGE_MAP();
	DECLARE_INTERFACE_MAP();
public:
	CAxView();
	~CAxView();

	CObjectListWrp &GetControls()	{return m_sptrControls;}

	//{{AFX_MSG(CAxView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	//{{AFX_VIRTUAL(CAxView)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnFinalRelease();
	//}}AFX_VIRTUAL

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, const _variant_t var );
	virtual void AttachActiveObjects();
	virtual DWORD GetMatchType( const char *szType );

	virtual bool CanResizeObject( IUnknown *punkObject ){return true;};
	virtual CRect GetObjectRect( IUnknown *punkObject );
	virtual void SetObjectOffset( IUnknown *punkObject, CPoint pointOffest );
	virtual IUnknown *GetObjectByPoint( CPoint );
	virtual bool DoDrop( CPoint point );
	virtual void DoResizeRotateObject( IUnknown *punkObject, CFrame *pframe );
	bool DoCreate( DWORD dwStyle, CRect rc, CWnd *pparent, UINT nID );


	virtual void UpdateSelectionRect();
	virtual void UpdateControlRect( IUnknown *punkControl );

	


	//void DoDrawControl( CDC &dc, IUnknown *punkControl, CRect rectDraw );
	virtual void DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state );
protected:
	BEGIN_INTERFACE_PART(LayoutVw, ILayoutView)
		com_call GetControlObject( IUnknown *punkAXCtrl, IUnknown **ppAX );
	END_INTERFACE_PART(LayoutVw);
protected:
	void _AddControl( IUnknown *punkControl );
	void _RemoveControl( IUnknown *punkControl );

	void _AddAllControls();
	void _RemoveAllControls();

	POSITION	_Find( IUnknown *punkControl );
	POSITION	_Find( CWnd *pwnd );
	ControlData	*_Get( IUnknown *punkControl );
	ControlData	*_Get( CWnd *pwnd );
	UINT 		_FindUniqueID( UINT nID );

	void		_UpdateLayout( bool bRepositionControls = true );
	bool		_IsControlWindow( HWND hWnd, bool bLookChild = true );

protected:
	CObjectListWrp	m_sptrControls;

	CTypedPtrList<CPtrList, ControlData*>	
		m_controlInfos;

	CRect	m_rectForm;
	CPoint	m_pointFormOffset;


	BEGIN_INTERFACE_PART(EditorMenu, IMenuInitializer)
		com_call OnInitPopup( BSTR bstrMenuName, HMENU hMenu, HMENU *phOutMenu );
	END_INTERFACE_PART(EditorMenu);
};
*/
#endif //__axview_h__
