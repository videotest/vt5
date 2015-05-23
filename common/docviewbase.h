#ifndef __docviewbase_h__
#define __docviewbase_h__

#include "cmnbase.h"
#include "docview5.h"
#include "core5.h"
#include "filebase.h"

enum ObjectDrawState
{
	odsNormal,
	odsSelected,
	odsActive,
	odsAnimation
};

#define WM_MESSAGE_ENABLE_CENTER WM_USER + 45321

class std_dll CDocTemplateBase : public CCmdTargetEx,
		public CNamedObjectImpl,
		public CRootedObjectImpl
{
public:
	DECLARE_DYNAMIC(CDocTemplateBase);
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP()
public:

	CDocTemplateBase();
	~CDocTemplateBase();
//initialize here...
	virtual void OnChangeParent();

	virtual CString GetDocTemplString() = 0;
	virtual DWORD GetDocFlags(){return m_dwFlags;};
	virtual UINT GetResourceID(){return (UINT)-1;}
	virtual const char *GetSplitterProgId(){return m_strSplitterProgID;}
public:	
	BEGIN_INTERFACE_PART_EXPORT(Templ, IDocTemplate)
		com_call GetDocTemplString( DWORD dwCode, BSTR *pbstr );
		com_call GetTemplateFlags( DWORD *pdwFlags );
		com_call GetDocResource( HICON *phIcon );
		com_call GetSplitterProgId( BSTR *pbstr );
	END_INTERFACE_PART(Templ)
protected:
	
	virtual CString ExtractDocTemplString( DWORD dwCode );
	virtual CString GetDocTemplateName();
//
	CString	m_strSplitterProgID;	//splitter prog ID
	DWORD	m_dwFlags;				//document template flags
};


class std_dll CDocBase : public CCmdTargetEx,
						public CNumeredObjectImpl,
						public CRootedObjectImpl,
						public CEventListenerImpl
{
	DECLARE_DYNAMIC(CDocBase);
	ENABLE_MULTYINTERFACE();
	DECLARE_INTERFACE_MAP()
public:
	CDocBase();
	~CDocBase();

	virtual void Init();
	virtual void OnFinalRelease();
public:
	BEGIN_INTERFACE_PART_EXPORT(Doc, IDocument)
		com_call GetDocFlags( DWORD *pdwFlags );
		com_call Init( IUnknown *punkDocSite, IUnknown *punkDocTempl, IUnknown *punkApp );
		com_call CreateNew();
		com_call LoadNativeFormat( BSTR bstr );
		com_call SaveNativeFormat( BSTR bstr );
	END_INTERFACE_PART(Doc)

protected:
	virtual DWORD	GetDocFlags(){return 0;}
	virtual bool OnNewDocument(){return true;};
	virtual bool OnOpenDocument( const char *psz ){return false;};
	virtual bool OnSaveDocument( const char *psz ){return false;};

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	virtual void OnActivateObject( IUnknown *punkDataObject, IUnknown *punkView );
	void UpdateAllViews( const char *psz, IUnknown *punkDataObject = 0 );
};

class std_dll CViewImpl : public CImplBase 
{
protected:
	BEGIN_INTERFACE_PART_EXPORT(View, IView2)
		com_call Init( IUnknown *punkDoc, IUnknown *punkSite );
		com_call GetDocument( IUnknown **ppunkDoc );
		com_call OnUpdate( BSTR bstr, IUnknown *punkObject );
		com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );
		com_call GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags /*ObjectFlags*/);
		com_call DoDragDrop( DWORD *pdwDropEffect );
		com_call GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame);
		com_call MoveDragFrameToFrame();
		com_call GetDroppedDataObject(IUnknown** punkDO);
		com_call OnActivateView( BOOL bActivate, IUnknown *punkOtherView );
		com_call GetFirstVisibleObjectPosition(TPOS *plpos);
		com_call GetNextVisibleObject(IUnknown ** ppunkObject, TPOS *plPos);
		com_call GetViewFlags(DWORD *pdwViewFlags);
	END_INTERFACE_PART(View)
public:
	IDocument		*GetDocument();

	virtual DWORD OnGetViewFlags() {return 0;}

	virtual void GetMultiFrame(IUnknown **ppunkObject, bool bDragFrame){*ppunkObject=0;};
	virtual void MoveDragFrameToFrame(){};
	virtual void OnActivateObject( IUnknown *punkDataObject );
	

	virtual void OnUpdate( const char *szHint, IUnknown *punkObject );
	virtual DWORD DoDragDrop()										{return DROPEFFECT_NONE;}

	virtual CScrollBar *GetScrollBarCtrl( int sbCode ) const;
	virtual DWORD GetMatchType( const char *szType );
	virtual ObjectFlags GetObjectFlags( const char *szType )		{return ofNormal;}
	virtual POSITION GetFisrtVisibleObjectPosition() = 0;
	virtual IUnknown * GetNextVisibleObject( POSITION &rPos ) = 0;
	virtual IUnknown* GetDroppedDataObject()	{	return 0;	}
	virtual void OnActivateView( bool bActivate, IUnknown *punkOtherView )		{bActivate;punkOtherView;}

	sptrIDocument	m_sptrIDocument;
};


class std_dll CPrintImpl : public CImplBase
{
protected:
	BEGIN_INTERFACE_PART_EXPORT(PrintView, IPrintView2)
		com_call GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX );
		com_call GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY );
		com_call Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags );
		com_call GetPrintFlags(DWORD *pdwFlags);
	END_INTERFACE_PART(PrintView)

public:
	//print functions
	virtual bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX ) = 0; 
	virtual bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY ) = 0;
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags ) = 0;
	virtual bool CanSaveToImage() {return true;}
};


#include "dragdrop.h"
#include "wndbase.h"
#include "contourapi.h"



class std_dll CViewBase : public CWnd,
				public CWindow2Impl,
				public CViewImpl,
				public CRootedObjectImpl,
				public CEventListenerImpl,
				public CNamedObjectImpl,
				public CPrintImpl,
				public CPersistImpl,
				public CHelpInfoImpl
{
	ENABLE_MULTYINTERFACE();
protected:
	DECLARE_INTERFACE_MAP();
	DECLARE_MESSAGE_MAP();

public:
	CViewBase();
	~CViewBase();
public:
//print functions
	virtual bool GetPrintWidth( int nMaxWidth, int& nReturnWidth, int nUserPosX, int& nNewUserPosX ); 
	virtual bool GetPrintHeight( int nMaxHeight, int& nReturnHeight, int nUserPosY, int& nNewUserPosY );
	virtual void Print( HDC hdc, CRect rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, bool bUseScrollZoom, DWORD dwFlags );

//service function's
	virtual CWnd *GetWindow(){return this;}

	virtual void OnActivateObject( IUnknown *punkDataObject );
	virtual void AttachActiveObjects();	//virtuals

	virtual bool DoAttachDragDropObject( IUnknown *punkDataObject, CPoint point = CPoint( -1, -1 ) );
	virtual bool DoDrop( CPoint point );
	virtual bool DoDrag( CPoint point );
	virtual IUnknown *GetDragDropObject( CPoint point = CPoint( -1, -1 ), CRect *prect = 0 );
	virtual DWORD DoDragDrop();

	virtual void OnBeginDragDrop() {}
	virtual void OnEndDragDrop() {}
	virtual bool CanStartTracking( CPoint pt )	{return true;}

	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	virtual void GetMultiFrame(IUnknown **ppunkObject, bool bDragFrame);
	virtual void MoveDragFrameToFrame();

	virtual CRect GetObjectRect( IUnknown *punkObject ){return NORECT;}
	virtual CRect GetObjectAdditionRect(IUnknown *punkObject) {return GetObjectRect(punkObject);}
	virtual void SetObjectOffset( IUnknown *punkObject, CPoint pointOffest ){}
 
	virtual void DoResizeRotateObject( IUnknown *punkObject, CFrame *pframe ){};
	virtual bool CanResizeObject( IUnknown *punkObject ) {return false;}
	virtual bool CanRotateObject( IUnknown *punkObject ) {return false;}
 
	virtual void DoDrawFrameObjects( IUnknown *punkObject, CDC &dc, CRect rectInvalidate, BITMAPINFOHEADER *pbih, byte *pdibBits, ObjectDrawState state );
	virtual IUnknown *GetObjectByPoint( CPoint ){return 0;}
	virtual bool PtInObject( IUnknown *punkObject, CPoint point ){return false;}

	virtual IUnknown* GetDroppedDataObject();

  virtual void OnDeselect(IUnknown* punkWillBeDeselectedNow){};

	//virtual void GetClassID( CLSID* pClassID);

	//{{AFX_MSG(CViewBase)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	//{{AFX_VIRTUAL(CViewBase)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//virtual void OnFinalRelease();
	//protected:
	//virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
	afx_msg LRESULT OnGetInterface(WPARAM, LPARAM){return (LRESULT)GetControllingUnknown();}
public:
	CRuntimeClass	*m_pframeRuntime;
	//CDataSource	m_dataSource;
	CFrame	*m_pframe;
	CFrame	*m_pframeDrag;

	UINT	m_nIDEventRepaintSelection;
	UINT_PTR	m_nIDTimerRepaintSelection;
protected:
	int m_nDrawFrameStage;
	CDropTarget				m_dropTarget;

	bool					m_bEventRegistered;

//dispatch
	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CViewBase)
	VARIANT disp_GetProperty( LPCTSTR str_name );
	BOOL disp_SetProperty( LPCTSTR str_name, VARIANT FAR* pvar );
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	BEGIN_INTERFACE_PART(ViewDispatch, IViewDispatch)
		com_call GetDispIID( GUID* pguid );
	END_INTERFACE_PART(ViewDispatch);

	virtual GUID get_view_dispiid( );

};


#include "actionbase.h"

class std_dll CActionShowViewBase: public CActionBase
{
protected:
	virtual bool Invoke();
	virtual bool IsChecked();
	virtual CString GetViewProgID() = 0;

	virtual void AfterInvoke(){};
};


class std_dll CShellDocTemplateImpl : public CImplBase
{
public:
	virtual IFileFilter *GetFileFilter( const char *pszFileName, bool bExcludeAlreadyOpen = false ){return 0;};
protected:
	BEGIN_INTERFACE_PART_EXPORT(ShellTempl, IShellDocTemplate)
		com_call GetFileFilterByFile(BSTR bstr, IUnknown **punk);
	END_INTERFACE_PART(ShellTempl)
};

//the preview site base class implementation.
//The main class should draw the objects from Preview Site object הרûו
class std_dll CPreviewSiteImpl : public CImplBase
{
public:
	CPreviewSiteImpl();
	virtual ~CPreviewSiteImpl();
public:
	bool		IsPreviewModeActive() const		{return m_bProcessPreview;}

	int			GetCurPosition() const			{return m_nCurrentPosition;}
	int			GetCurStage() const				{return m_nCurrentStage;}

	POSITION	GetFisrtPreviewPosition()				{return m_listObjects.GetHeadPosition();}
	//without AddRef!!!
	IUnknown	*GetNextObject( POSITION &rPosObject )	{return m_listObjects.GetNext( rPosObject );}
	IUnknown	*GetFirstPreviewObject();
public:
	virtual void DoEnterPreviewMode();
	virtual void DoLeavePreviewMode();
	virtual void DoTerminatePreview();
	virtual void DoAddPreviewObject( IUnknown *punkObject );
	virtual void GetOperationInfo( IUnknown *punkLoog );
protected:
	virtual bool CanPreviewObject( IUnknown *punkObject ) = 0;
	virtual CRect GetPreviewObjectRect( IUnknown *punkObject, int nPosition = -1 ) = 0;
	virtual CWnd *GetTargetWindow() = 0;
protected:
	BEGIN_INTERFACE_PART_EXPORT(Preview, IPreviewSite)
		com_call InitPreview( IUnknown *punkLong );
		com_call AddPreviewObject( IUnknown *punkObject );
		com_call ProgressPreview( int nStage, int nProgress );
		com_call DeInitPreview();
		com_call TerminatePreview();
		com_call CanThisObjectBeDisplayed( IUnknown *punk, BOOL *pbCan );
	END_INTERFACE_PART(Preview)
protected:
	void	_PreviewStep( int nNewPos );
protected:
	CTypedPtrList<CPtrList, IUnknown*>	m_listObjects;
	int		m_nCurrentStage;
	int		m_nCurrentPosition;
	bool	m_bProcessPreview;

	int		m_nMaxProgress;

	double	m_fCurrentZoom;
	CPoint	m_nCurrentScroll;
};


class std_dll CEasyPreviewImpl : public CImplBase
{
	BEGIN_INTERFACE_PART_EXPORT(Preview, IEasyPreview)
		com_call AttachPreviewData( IUnknown *punkND );
		com_call PreviewReady();
	END_INTERFACE_PART(Preview)

	bool	IsPreviewModeActive()	{return m_ptrPreviewData != 0;}

	virtual void OnEnterPreviewMode(){};
	virtual void OnLeavePreviewMode(){};
	virtual void OnPreviewReady(){};

	IUnknownPtr	m_ptrPreviewData;
};


#endif //__docviewbase_h__
