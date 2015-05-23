// AxDocSite.h: interface for the CAxDocSite class.
//
//////////////////////////////////////////////////////////////////////

#if !defined __AXDOC_SITE_H__
#define __AXDOC_SITE_H__

#include "AxDocContainer.h"

class CAxDocSite : public ComObjectBase,
				public IOleClientSite,
				public IAdviseSink,
				public IOleInPlaceSite,
				public IOleDocumentSite

{
	friend class CAxDocContainer;
	friend class CAxDocSite;
	friend class COleItemData;

	CAxDocContainer* m_pContainer;

	route_unknown();

public:
	CAxDocSite( CAxDocContainer* pContainer );
	virtual ~CAxDocSite();

public:
	virtual IUnknown *DoGetInterface( const IID &iid );

public:
	//IOleClientSite
	com_call SaveObject(void);
	com_call GetMoniker(DWORD, DWORD, LPMONIKER *);
	com_call GetContainer(LPOLECONTAINER *);
	com_call ShowObject(void);
	com_call OnShowWindow(BOOL);
	com_call RequestNewObjectLayout(void);

	//IAdviseSink
	STDMETHODIMP_(void)  OnDataChange(LPFORMATETC, LPSTGMEDIUM);
	STDMETHODIMP_(void)  OnViewChange(DWORD, LONG);
	STDMETHODIMP_(void)  OnRename(LPMONIKER);
	STDMETHODIMP_(void)  OnSave(void);
	STDMETHODIMP_(void)  OnClose(void);

	//IOleInPlaceSite
	com_call GetWindow(HWND *);
	com_call ContextSensitiveHelp(BOOL);
	com_call CanInPlaceActivate(void);
	com_call OnInPlaceActivate(void);
	com_call OnUIActivate(void);
	com_call GetWindowContext(LPOLEINPLACEFRAME *
					, LPOLEINPLACEUIWINDOW *, LPRECT, LPRECT
					, LPOLEINPLACEFRAMEINFO);
	com_call Scroll(SIZE);
	com_call OnUIDeactivate(BOOL);
	com_call OnInPlaceDeactivate(void);
	com_call DiscardUndoState(void);
	com_call DeactivateAndUndo(void);
	com_call OnPosRectChange(LPCRECT);

	//IOleDocumentSite
	com_call ActivateMe(IOleDocumentView *);

};

#endif // __AXDOC_SITE_H__
