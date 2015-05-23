// OleView.h: interface for the COleView class.
//
//////////////////////////////////////////////////////////////////////

#ifndef __oleclient_oleview_h__
#define __oleclient_oleview_h__

#include "oledata.h"
#include "win_view.h"


class COleView  : public CWinViewBase, 
				public IOleView,
				public IPrintView,
				public IPersist,
				public CNamedObjectImpl,
				public _dyncreate_t<COleView >
{
public:
	COleView();
	virtual ~COleView();

public:
	virtual IUnknown *DoGetInterface( const IID &iid );
	
	route_unknown();
protected:
	virtual LRESULT		DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void		DoAttachObjects();

	virtual LRESULT OnPaint();							//WM_PAINT	
	virtual LRESULT OnEraseBackground( HDC hDC );		//WM_ERASEBKGND
	virtual LRESULT OnSize( int cx, int cy );			//WM_SIZE
	virtual LRESULT OnDestroy( );						//WM_DESTROY
	virtual LRESULT OnLButtonDblClick( int x, int y );	//WM_LBUTTONDBLCLK
	virtual LRESULT OnContextMenu( HWND hWnd, int x, int y );	//WM_CONTEXTMENU
	virtual LRESULT OnSetFocus( HWND hWndOld );			//WM_SETFOCUS

	//interface overriden
	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );
	
	com_call OnActivateView( BOOL bActivate, IUnknown *punkOtherView );

	IOleObjectUIPtr m_ptrActiveObject;	

	void SetClientSize( SIZE size );
	void ReAttachObject( bool bForceReattach );

public:
	com_call GetHwnd( HWND* phwnd );

	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//Print support
	com_call GetPrintWidth( int nMaxWidth, int *pnReturnWidth, BOOL *pbContinue, int nUserPosX, int *pnNewUserPosX );
	com_call GetPrintHeight( int nMaxHeight, int *pnReturnHeight, BOOL *pbContinue, int nUserPosY, int *pnNewUserPosY );
	com_call Print( HDC hdc, RECT rectTarget, int nUserPosX, int nUserPosY, int nUserPosDX, int nUserPosDY, BOOL bUseScrollZoom, DWORD dwFlags );

	//Persist Impl
	com_call GetClassID(CLSID *pClassID ); 


	com_call GetFirstVisibleObjectPosition(TPOS *plpos);
	com_call GetNextVisibleObject(IUnknown ** ppunkObject, TPOS *plPos);

	void ShowPropPage( bool bShow );
	

};

#endif // __oleclient_oleview_h__
