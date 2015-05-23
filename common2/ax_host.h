#ifndef __axhost_h__
#define __axhost_h__

#include <ocidl.h>
#include "win_main.h"
#include "com_defs.h"
#include "com_main.h"
#include "misc_utils.h"

#define BASE_EXTENDED_PROPERTY      0x80010000
#define DISPID_NAME                 (BASE_EXTENDED_PROPERTY | 0x00)
#define DISPID_ALIGN                (BASE_EXTENDED_PROPERTY | 0x01)
#define DISPID_BASEHREF             (BASE_EXTENDED_PROPERTY | 0x02)

#define WM_GETCONTROL		(WM_USER+1001)
#define WM_GETAXCONTAINER	(WM_USER+1002)


class CAxContainerWin : public ComObjectBase,
						public CWinImpl,
						public IDispatch,
						//public IOleInPlaceSite,
						public IOleInPlaceSiteWindowless,
						public IOleInPlaceFrame,
						public IOleControlSite,
						public IOleClientSite

{
	route_unknown( );
public:
	CAxContainerWin( );
	virtual ~CAxContainerWin( );

	virtual bool CreateControl( const char *pszRegString, IStream *pstream = 0 );
	virtual void DestroyControl();
public:
	virtual void DoPaint( );
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam  );
	virtual LRESULT DoCreate( CREATESTRUCT *pcs  );
	virtual LRESULT DoGetControl();

	virtual IUnknown *DoGetInterface( const IID &iid );
protected:
	// *** IDispatch Methods ***
	com_call GetIDsOfNames( REFIID riid, OLECHAR FAR* FAR* rgszNames,	unsigned int cNames, LCID lcid,	DISPID FAR* rgdispid );
	com_call GetTypeInfo( unsigned int itinfo, LCID lcid, ITypeInfo FAR* FAR* pptinfo );
	com_call GetTypeInfoCount( unsigned int FAR * pctinfo );
	com_call Invoke( DISPID dispid, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS FAR *pdispparams, VARIANT FAR *pvarResult, EXCEPINFO FAR * pexecinfo, unsigned int FAR *puArgErr );
	// *** IOleClientSite methods ***
	com_call SaveObject(  );
	com_call GetMoniker( DWORD, DWORD, LPMONIKER * );
	com_call GetContainer( LPOLECONTAINER * );
	com_call ShowObject(  );
	com_call OnShowWindow( BOOL );
	com_call RequestNewObjectLayout(  );
	// *** IOleWindow Methods ***
	com_call GetWindow( HWND * phwnd );
	com_call ContextSensitiveHelp( BOOL fEnterMode );
	// *** IOleInPlaceSite Methods ***
	com_call CanInPlaceActivate( void );
	com_call OnInPlaceActivate( void );
	com_call OnUIActivate( void );
	com_call GetWindowContext( IOleInPlaceFrame ** ppFrame, IOleInPlaceUIWindow ** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo );
	com_call Scroll( SIZE scrollExtent );
	com_call OnUIDeactivate( BOOL fUndoable );
	com_call OnInPlaceDeactivate( void );
	com_call DiscardUndoState( void );
	com_call DeactivateAndUndo( void );
	com_call OnPosRectChange( LPCRECT lprcPosRect );
	// *** IOleInPlaceSiteEx Methods ***
	com_call OnInPlaceActivateEx( BOOL *pfNoRedraw, DWORD dwFlags );
	com_call OnInPlaceDeactivateEx( BOOL fNoRedraw );
	com_call RequestUIActivate( void );
	// *** IOleInPlaceSiteWindowless Methods ***
	com_call CanWindowlessActivate( void );
	com_call GetCapture( void );
	com_call SetCapture( BOOL fCapture );
	com_call GetFocus( void );
	com_call SetFocus( BOOL fFocus );
	com_call GetDC( LPCRECT pRect, DWORD grfFlags, HDC *phDC );
	com_call ReleaseDC( HDC hDC );
	com_call InvalidateRect( LPCRECT pRect, BOOL fErase );
	com_call InvalidateRgn( HRGN hRGN, BOOL fErase );
	com_call ScrollRect( INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip );
	com_call AdjustRect( LPRECT prc );
	com_call OnDefWindowMessage( UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult );
	// *** IOleInPlaceUIWindow Methods ***
	com_call GetBorder( LPRECT lprectBorder );
	com_call RequestBorderSpace( LPCBORDERWIDTHS lpborderwidths );
	com_call SetBorderSpace( LPCBORDERWIDTHS lpborderwidths );
	com_call SetActiveObject( IOleInPlaceActiveObject * pActiveObject,
						LPCOLESTR lpszObjName );
	// *** IOleInPlaceFrame Methods ***
	com_call InsertMenus( HMENU hmenuShared, LPOLEMENUGROUPWIDTHS lpMenuWidths );
	com_call SetMenu( HMENU hmenuShared, HOLEMENU holemenu, HWND hwndActiveObject );
	com_call RemoveMenus( HMENU hmenuShared );
	com_call SetStatusText( LPCOLESTR pszStatusText );
	com_call EnableModeless( BOOL fEnable );
	com_call TranslateAccelerator( LPMSG lpmsg, WORD wID );
	// *** IOleControlSite Methods ***
	com_call OnControlInfoChanged( void );
	com_call LockInPlaceActive( BOOL fLock );
	com_call GetExtendedControl( IDispatch **ppDisp );
	com_call TransformCoords( POINTL *pptlHimetric, POINTF *pptfContainer, DWORD dwFlags );
	com_call TranslateAccelerator( LPMSG pMsg, DWORD grfModifiers );
	com_call OnFocus( BOOL fGotFocus );
	com_call ShowPropertyFrame( void );
protected:
	HWND GetHostWindow();
protected:
	IUnknownPtr	m_ptr;
	IUnknown	*m_punkOuter;
	_bstr_t		m_bstrClassName;
	_rect		m_rectPos;
	bool		m_bCapture;
	HWND		m_hwndControl;
};




#endif //__axhost_h__