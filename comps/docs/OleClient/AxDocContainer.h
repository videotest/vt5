// AxDocContainer.h: interface for the CAxDocContainer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined __AXDOC_CONTAINER__
#define __AXDOC_CONTAINER__

#include "OleData.h"
#include "AxDocSite.h"

class CAxDocContainer : public ComObjectBase,
				public IOleInPlaceFrame, 
				public IOleCommandTarget
{
	route_unknown();

	friend class CAxDocSite;
	friend class COleItemData;

	DWORD m_dwID;	
	COleItemData* m_pOleItemData;

	//Site object
	CAxDocSite* m_pSite;

	bool		m_bWasModifiedServerData;

protected:
	void InitDefaults();
	//Full destroyer
	void Free();	
	


public:
	CAxDocContainer( COleItemData* pOleItemData );
	virtual ~CAxDocContainer();

	bool IsModifiedServerData();	
	void SetModifiedServerData( bool bModified );

public:
	virtual IUnknown *DoGetInterface( const IID &iid );

protected:
	OleItemState	m_itemState;
	bool			m_bCreateOleObject;	
	bool			m_bWasFullActivated;
	bool			m_bWasDeactivateUI;

public:	
	bool			CreateOleObject( );
	void			DestroyOleObject( );

	OleItemState	GetItemState(){ return m_itemState; }

	void			DestroyOleUI();

	bool			Activate( HWND hWndMain, HWND hWndClient );
	bool			Deactivate( HWND hWndView );
	bool			ActivateUI( HWND hWndView );
	bool			DeactivateUI( HWND hWndView );

	void			OnResizeClient( int cx, int cy );
	
	HWND			GetInplaceActiveHwnd(){ return m_hWndClient;}

	HRESULT			DoVerb( UINT uiVerb );

	HRESULT			ForceUpdate();
	
	OleObjectType	GetOleObjectType();

protected:
	bool			ExecVerb( int nVerb );
protected:

	HWND			m_hwndMain;
	HWND			m_hWndClient;
	HWND			m_hWndObj;			//UI Active object	

//storage
	ILockBytes		*m_lpLockBytes;
	IStorage		*m_pIStorage;        //Temp file for all needs
	IStream			*m_pIStream;  //Temp file for all needs

//internal buffer	
	BYTE*			m_pOleData;
	UINT			m_uOleDataLength;
	void			DestroyOleDataBuf();
	bool			IsDataBufOK(){ return ( m_uOleDataLength >0 && m_pOleData ); }

	bool			StoreStreamToBuf( bool bSameStream = false );

	void			DestroyStorage();
	bool			CreateStorage( bool bInitFromOleData );
	bool			CanProcessDrawig( HWND hWnd );



	BORDERWIDTHS    m_bwIP;             //In-place tool allocations
	IOleInPlaceActiveObject *m_pIOleIPActiveObject;		

	BOOL				m_bDocObj;
	//Object interfaces
	IUnknown			*m_pObj;
	IOleObject			*m_pIOleObject;
	IOleInPlaceObject	*m_pIOleIPObject;
	IOleDocumentView	*m_pIOleDocView;
	IViewObject2		*m_lpViewObject;


	void				FireEventStateChange();

	HICON				GetIconFromRegistry();

	HIMAGELIST			m_hImageList;
	int					m_nIconNumber;
	bool				CreateImageList();
	void				DestroyImageList();

	void				StoreImageList( IStream *pStream );
	void				LoadImageList( IStream *pStream );
	
	//bool				AddVerbMenu();
	


public:
	bool	LoadDataBuf( IStream *pStream );
	bool	StoreDataBuf( IStream *pStream );
	
	com_call	DrawMeta( int nDrawAspect, HWND hWnd, HDC hdc, RECT rcTarget, POINT ptOffset, SIZE sizeClip );
	SIZEL		GetViewObjectExtent();
	SIZE		GetScrollSize( HWND hWnd );
	double		GetZoomExtent();


public:
	//IOleInPlaceFrame implementation
    com_call GetWindow(HWND *);
    com_call ContextSensitiveHelp(BOOL);
    com_call GetBorder(LPRECT);
    com_call RequestBorderSpace(LPCBORDERWIDTHS);
    com_call SetBorderSpace(LPCBORDERWIDTHS);
    com_call SetActiveObject(LPOLEINPLACEACTIVEOBJECT
                             , LPCOLESTR);
    com_call InsertMenus(HMENU, LPOLEMENUGROUPWIDTHS);
    com_call SetMenu(HMENU, HOLEMENU, HWND);
    com_call RemoveMenus(HMENU);
    com_call SetStatusText(LPCOLESTR);
    com_call EnableModeless(BOOL);
    com_call TranslateAccelerator(LPMSG, WORD);

	//IOleCommandTarget
    com_call QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds
        , OLECMD prgCmds[], OLECMDTEXT *pCmdText);
    
    com_call Exec(const GUID *pguidCmdGroup, DWORD nCmdID
        , DWORD nCmdexecopt, VARIANTARG *pvaIn, VARIANTARG *pvaOut);            


	
	HMENU m_hFrameMenu[OLE_MENU_SIZE];
	_bstr_t m_bstrMenuCaption[OLE_MENU_SIZE];
	void ClearMenu();


	_bstr_t		m_bstrStreamName;	


	bool			m_bNeedFireObjectChange;


};

#endif // __AXDOC_CONTAINER__
