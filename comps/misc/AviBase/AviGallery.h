// AviGallery.h: interface for the CAviGallery class.
//
//////////////////////////////////////////////////////////////////////
#ifndef __avigallery_h__
#define __avigallery_h__

#include "win_view.h"
#include "\vt5\ifaces\avi_int.h"

#define ITEM_DISTANCE	20

class CAviGallery : public CWinViewBase, 				
					//public IPrintView,
					public IPersist,
					public CNamedObjectImpl,
					public _dyncreate_t<CAviGallery >
{
public:
	CAviGallery();
	virtual ~CAviGallery();

public:
	virtual IUnknown *DoGetInterface( const IID &iid );
	
	route_unknown();
protected:
	virtual LRESULT		DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void		DoAttachObjects();

	virtual LRESULT OnPaint();									//WM_PAINT	
	virtual LRESULT OnEraseBackground( HDC hDC );				//WM_ERASEBKGND
	virtual LRESULT OnSize( int cx, int cy );					//WM_SIZE
	virtual LRESULT OnDestroy( );								//WM_DESTROY
	virtual LRESULT OnLButtonDown(long lflags, int x, int y );	//WM_LBUTTONDOWN
	virtual LRESULT OnLButtonDblClick( int x, int y );			//WM_LBUTTONDBLCLK


	IAviImagePtr	m_ptrAvi;

	

	long			GetStartFrame();
	long			GetEndFrame();
	long			GetActiveFrame();

	void			BuildView();


	SIZE			GetThumbnailSize();
	long			GetColCount();
	long			GetRowCount();
	long			ConvertToLinearIndex( long lCol, long lRow );
	void			ConvertToHorzVertIndex( long lLinearIndex, long& lCol, long& lRow );

	void			ProcessDrawing( HDC hdc, RECT rectPaint );
	void			DrawFrame( HDC hdc, long lIndex );
	long			GetNextVisibleIndex( RECT rcVisible, long lIndex );

	void			SetClientSize( SIZE size );
	void			ReAttachObject( bool bForceReattach );

	void			ReadSettings();
	long			GetNearestFrame( long lframe );
	RECT			GetItemRect( long lframe );

	COLORREF		m_clrBk;
	COLORREF		m_clrItem;
	COLORREF		m_clrSelectItem;

	//interface overriden
	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );
	
	com_call OnActivateView( BOOL bActivate, IUnknown *punkOtherView );


	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );

	//Persist Impl
	com_call GetClassID( CLSID *pClassID ); 

	com_call GetFirstVisibleObjectPosition(TPOS *plpos);
	com_call GetNextVisibleObject(IUnknown ** ppunkObject, TPOS *plPos);

	void ShowPropPage( bool bShow );


	HDRAWDIB	m_hDrawDib;

	BYTE*		m_pDibBits;
	long		m_lBufSize;
	long		m_lPrevKeyFrame;
	long		m_lPrevFrame;

	long		m_lframe_step;
	SIZE		m_size_thumbnail;
};

#endif //__avigallery_h__

