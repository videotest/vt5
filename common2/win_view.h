#ifndef __win_view_h__
#define __win_view_h__

#include "com_unknown.h"
#include "win_main.h"
#include "misc_list.h"
#include "impl_event.h"

#include "docview5.h"


class CWinViewBase : public ComAggregableBase, 
					public CWinImpl,
					public CEventListenerImpl,
					public IView
{
public:
	CWinViewBase();
	virtual ~CWinViewBase();
public:	//IView
	com_call Init( IUnknown *punkDoc, IUnknown *punkSite );
	com_call GetDocument( IUnknown **ppunkDoc );
	com_call OnUpdate( BSTR bstr, IUnknown *punkObject );
	com_call GetMatchType( BSTR bstrObjectType, DWORD *pdwMatch );
	//drag-drop extension

	com_call GetObjectFlags( BSTR bstrObjectType, DWORD *pdwFlags /*ObjectFlags*/);
	com_call DoDragDrop( DWORD *pdwDropEffect );
	com_call GetMultiFrame(IUnknown **ppunkObject, BOOL bDragFrame);
	com_call MoveDragFrameToFrame();

	com_call GetDroppedDataObject(IUnknown** punkDO);
	com_call OnActivateView( BOOL bActivate, IUnknown *punkOtherView );

	com_call GetFirstVisibleObjectPosition(TPOS *plpos);
	com_call GetNextVisibleObject( IUnknown ** ppunkObject, TPOS *plPos );
public:
	virtual IUnknown *DoGetInterface( const IID &iid );
	virtual void DoAttachObjects();
	virtual LRESULT	DoMessage( UINT nMsg, WPARAM wParam, LPARAM lParam );
	virtual void OnNotify( const char *pszEvent, IUnknown *punkHit, IUnknown *punkFrom, void *pdata, long cbSize );
//message cracker prototipes
	/*WM_CREATE*/	LRESULT	DoCreate( CREATESTRUCT *pcs );
protected:
	IUnknown		*m_punkDocument;
	_list_ptr_t<IUnknown*, FreeReleaseUnknown>	m_listObjects;	
};

#endif // __win_view_h__